const json = (body, status, origin) => new Response(JSON.stringify(body), {
  status,
  headers: {
    "content-type": "application/json; charset=utf-8",
    "access-control-allow-origin": origin,
    "access-control-allow-methods": "GET, POST, OPTIONS",
    "access-control-allow-headers": "content-type, authorization",
    "vary": "Origin",
    "cache-control": "no-store",
    "x-content-type-options": "nosniff",
  },
});

function allowedOrigin(request, env) {
  const origin = request.headers.get("Origin") || "";
  if (origin === env.ALLOWED_ORIGIN || /^http:\/\/(localhost|127\.0\.0\.1)(:\d+)?$/.test(origin)) {
    return origin;
  }
  return "";
}

async function verifyGoogleCredential(credential, env) {
  if (!env.GOOGLE_CLIENT_ID || env.GOOGLE_CLIENT_ID === "not-configured") {
    throw new Error("Google login is not configured yet");
  }
  const url = new URL("https://oauth2.googleapis.com/tokeninfo");
  url.searchParams.set("id_token", credential);
  const response = await fetch(url);
  if (!response.ok) throw new Error("Google rejected this credential");
  const profile = await response.json();
  const issuerOk = profile.iss === "accounts.google.com" || profile.iss === "https://accounts.google.com";
  const expiresAt = Number(profile.exp || 0);
  if (!issuerOk || profile.aud !== env.GOOGLE_CLIENT_ID || profile.email_verified !== "true" || expiresAt <= Date.now() / 1000) {
    throw new Error("Credential validation failed");
  }
  return {
    id: profile.sub,
    email: profile.email,
    name: profile.name || profile.email,
    picture: profile.picture || "",
    expiresAt,
  };
}

async function authenticatedUser(request, env) {
  const authorization = request.headers.get("Authorization") || "";
  if (!authorization.startsWith("Bearer ")) throw new Error("Authentication required");
  const token = authorization.slice(7);
  if (!token || token.length > 4096) throw new Error("Invalid session");
  return parseSessionToken(token, env);
}

async function saveUser(user, env) {
  await env.DB.prepare(`
    INSERT INTO users (google_sub, email, name) VALUES (?, ?, ?)
    ON CONFLICT(google_sub) DO UPDATE SET
      email = excluded.email,
      name = excluded.name,
      updated_at = CURRENT_TIMESTAMP
  `).bind(user.id, user.email, user.name).run();
}

function validChallengeIds(value) {
  if (!Array.isArray(value) || value.length > 100) return null;
  const ids = [...new Set(value)];
  return ids.every(id => typeof id === "string" && /^[a-z0-9][a-z0-9-]{0,63}$/.test(id)) ? ids : null;
}

const encoder = new TextEncoder();

async function hmacHex(secret, message) {
  const key = await crypto.subtle.importKey("raw", encoder.encode(secret), { name: "HMAC", hash: "SHA-256" }, false, ["sign"]);
  const signature = await crypto.subtle.sign("HMAC", key, encoder.encode(message));
  return [...new Uint8Array(signature)].map(byte => byte.toString(16).padStart(2, "0")).join("");
}

async function sha256Hex(value) {
  const digest = await crypto.subtle.digest("SHA-256", encoder.encode(value));
  return [...new Uint8Array(digest)].map(byte => byte.toString(16).padStart(2, "0")).join("");
}

function safeEqual(left, right) {
  if (typeof left !== "string" || left.length !== right.length) return false;
  let difference = 0;
  for (let index = 0; index < left.length; index += 1) difference |= left.charCodeAt(index) ^ right.charCodeAt(index);
  return difference === 0;
}

function base64urlEncode(value) {
  const bytes = encoder.encode(value);
  return btoa(String.fromCharCode(...bytes)).replace(/\+/g, "-").replace(/\//g, "_").replace(/=+$/, "");
}

function base64urlDecode(value) {
  const padded = value.replace(/-/g, "+").replace(/_/g, "/") + "===".slice((value.length + 3) % 4);
  return new TextDecoder().decode(Uint8Array.from(atob(padded), character => character.charCodeAt(0)));
}

async function createSessionToken(user, env) {
  if (!env.SESSION_SECRET) throw new Error("Session signing is not configured");
  const payload = base64urlEncode(JSON.stringify({
    sub: user.id,
    email: user.email,
    name: user.name,
    picture: user.picture,
    exp: Math.floor(Date.now() / 1000) + 60 * 60 * 24 * 30,
  }));
  return `${payload}.${await hmacHex(env.SESSION_SECRET, `session:${payload}`)}`;
}

async function parseSessionToken(token, env) {
  if (!env.SESSION_SECRET) throw new Error("Session signing is not configured");
  const [payload, signature, extra] = token.split(".");
  if (!payload || !signature || extra || !safeEqual(signature, await hmacHex(env.SESSION_SECRET, `session:${payload}`))) {
    throw new Error("Invalid session");
  }
  const data = JSON.parse(base64urlDecode(payload));
  if (typeof data.sub !== "string" || !data.sub || Number(data.exp) <= Date.now() / 1000) {
    throw new Error("Session expired");
  }
  return {
    id: data.sub,
    email: typeof data.email === "string" ? data.email : "",
    name: typeof data.name === "string" ? data.name : data.email || "TofuCTF user",
    picture: typeof data.picture === "string" ? data.picture : "",
    expiresAt: Number(data.exp),
  };
}

async function createLaunchToken(userId, challengeId, env) {
  const payload = base64urlEncode(JSON.stringify({ sub: userId, challengeId, exp: Math.floor(Date.now() / 1000) + 900 }));
  return `${payload}.${await hmacHex(env.FLAG_SECRET, `launch:${payload}`)}`;
}

async function parseLaunchToken(token, env) {
  if (typeof token !== "string" || token.length > 2048) throw new Error("Invalid launch token");
  const [payload, signature, extra] = token.split(".");
  if (!payload || !signature || extra || !safeEqual(signature, await hmacHex(env.FLAG_SECRET, `launch:${payload}`))) {
    throw new Error("Invalid launch token");
  }
  const data = JSON.parse(base64urlDecode(payload));
  if (!data.sub || !/^[a-z0-9][a-z0-9-]{0,63}$/.test(data.challengeId || "") || Number(data.exp) <= Date.now() / 1000) {
    throw new Error("Launch token expired or invalid");
  }
  return data;
}

async function progressFor(userId, env) {
  const result = await env.DB.prepare(
    "SELECT challenge_id, solved_at FROM solves WHERE google_sub = ? ORDER BY solved_at"
  ).bind(userId).all();
  return result.results.map(row => ({ id: row.challenge_id, solvedAt: row.solved_at }));
}

export default {
  async fetch(request, env) {
    const origin = allowedOrigin(request, env);
    const url = new URL(request.url);

    if (request.method === "OPTIONS") {
      return origin ? new Response(null, {
        status: 204,
        headers: {
          "access-control-allow-origin": origin,
          "access-control-allow-methods": "GET, POST, OPTIONS",
          "access-control-allow-headers": "content-type, authorization",
          "access-control-max-age": "86400",
          "vary": "Origin",
        },
      }) : new Response(null, { status: 403 });
    }

    if (url.pathname === "/health" && request.method === "GET") {
      return json({ ok: true, googleConfigured: env.GOOGLE_CLIENT_ID !== "not-configured" }, 200, origin || env.ALLOWED_ORIGIN);
    }

    if (!origin) return json({ error: "Origin not allowed" }, 403, env.ALLOWED_ORIGIN);

    if (url.pathname === "/auth/google" && request.method === "POST") {
      try {
        const { credential } = await request.json();
        if (typeof credential !== "string" || credential.length > 4096) {
          return json({ error: "Invalid credential" }, 400, origin);
        }
        const user = await verifyGoogleCredential(credential, env);
        await saveUser(user, env);
        return json({ user, token: await createSessionToken(user, env) }, 200, origin);
      } catch (error) {
        return json({ error: error.message }, 401, origin);
      }
    }

    if (url.pathname === "/auth/session" && request.method === "GET") {
      try {
        return json({ user: await authenticatedUser(request, env) }, 200, origin);
      } catch (error) {
        return json({ error: error.message }, 401, origin);
      }
    }

    if ((url.pathname === "/api/progress" || url.pathname === "/progress") && request.method === "GET") {
      try {
        const user = await authenticatedUser(request, env);
        await saveUser(user, env);
        return json({ solved: await progressFor(user.id, env) }, 200, origin);
      } catch (error) {
        return json({ error: error.message }, 401, origin);
      }
    }

    if ((url.pathname === "/api/progress/import" || url.pathname === "/progress/import") && request.method === "POST") {
      try {
        const user = await authenticatedUser(request, env);
        const body = await request.json();
        const challengeIds = validChallengeIds(body.challengeIds);
        if (!challengeIds) return json({ error: "Invalid challenge IDs" }, 400, origin);
        await saveUser(user, env);
        const migration = await env.DB.prepare("SELECT local_migrated_at FROM users WHERE google_sub = ?").bind(user.id).first();
        if (!migration.local_migrated_at) {
          const statements = challengeIds.map(id => env.DB.prepare(`
              INSERT INTO solves (google_sub, challenge_id, source) VALUES (?, ?, 'local-import')
              ON CONFLICT(google_sub, challenge_id) DO NOTHING
            `).bind(user.id, id));
          statements.push(env.DB.prepare("UPDATE users SET local_migrated_at = CURRENT_TIMESTAMP WHERE google_sub = ?").bind(user.id));
          await env.DB.batch(statements);
        }
        return json({ solved: await progressFor(user.id, env), migrated: !migration.local_migrated_at }, 200, origin);
      } catch (error) {
        const status = error instanceof SyntaxError ? 400 : 401;
        return json({ error: error.message }, status, origin);
      }
    }

    if (url.pathname === "/api/submit" && request.method === "POST") {
      try {
        const { challengeId, flag } = await request.json();
        const ids = validChallengeIds([challengeId]);
        if (!ids || typeof flag !== "string" || flag.length > 128) return json({ error: "Invalid submission" }, 400, origin);
        const challenge = await env.DB.prepare("SELECT flag_hash FROM challenge_flags WHERE challenge_id = ?").bind(challengeId).first();
        if (!challenge) return json({ error: "Unknown challenge" }, 404, origin);
        if (!safeEqual(await sha256Hex(flag.trim()), challenge.flag_hash)) return json({ correct: false }, 200, origin);
        const authorization = request.headers.get("Authorization") || "";
        if (!authorization) return json({ correct: true, accountSaved: false }, 200, origin);
        const user = await authenticatedUser(request, env);
        await saveUser(user, env);
        await env.DB.prepare(`
          INSERT INTO solves (google_sub, challenge_id, source) VALUES (?, ?, 'verified')
          ON CONFLICT(google_sub, challenge_id) DO NOTHING
        `).bind(user.id, challengeId).run();
        return json({ correct: true, accountSaved: true, solved: await progressFor(user.id, env) }, 200, origin);
      } catch (error) {
        return json({ error: error.message }, error instanceof SyntaxError ? 400 : 401, origin);
      }
    }

    return json({ error: "Not found" }, 404, origin);
  },
};

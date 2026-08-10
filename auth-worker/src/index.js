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
  const credential = authorization.slice(7);
  if (!credential || credential.length > 4096) throw new Error("Invalid credential");
  return verifyGoogleCredential(credential, env);
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
        return json({ user }, 200, origin);
      } catch (error) {
        return json({ error: error.message }, 401, origin);
      }
    }

    if (url.pathname === "/progress" && request.method === "GET") {
      try {
        const user = await authenticatedUser(request, env);
        await saveUser(user, env);
        const result = await env.DB.prepare(
          "SELECT challenge_id, solved_at FROM solves WHERE google_sub = ? ORDER BY solved_at"
        ).bind(user.id).all();
        return json({ solved: result.results.map(row => ({ id: row.challenge_id, solvedAt: row.solved_at })) }, 200, origin);
      } catch (error) {
        return json({ error: error.message }, 401, origin);
      }
    }

    if (url.pathname === "/progress/import" && request.method === "POST") {
      try {
        const user = await authenticatedUser(request, env);
        const body = await request.json();
        const challengeIds = validChallengeIds(body.challengeIds);
        if (!challengeIds) return json({ error: "Invalid challenge IDs" }, 400, origin);
        await saveUser(user, env);
        if (challengeIds.length) {
          await env.DB.batch(challengeIds.map(id => env.DB.prepare(`
            INSERT INTO solves (google_sub, challenge_id, source) VALUES (?, ?, 'local-import')
            ON CONFLICT(google_sub, challenge_id) DO NOTHING
          `).bind(user.id, id)));
        }
        const result = await env.DB.prepare(
          "SELECT challenge_id, solved_at FROM solves WHERE google_sub = ? ORDER BY solved_at"
        ).bind(user.id).all();
        return json({ solved: result.results.map(row => ({ id: row.challenge_id, solvedAt: row.solved_at })) }, 200, origin);
      } catch (error) {
        const status = error instanceof SyntaxError ? 400 : 401;
        return json({ error: error.message }, status, origin);
      }
    }

    return json({ error: "Not found" }, 404, origin);
  },
};

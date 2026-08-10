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

    return json({ error: "Not found" }, 404, origin);
  },
};

CREATE TABLE IF NOT EXISTS users (
  google_sub TEXT PRIMARY KEY,
  email TEXT NOT NULL,
  name TEXT NOT NULL,
  local_migrated_at TEXT,
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS solves (
  google_sub TEXT NOT NULL,
  challenge_id TEXT NOT NULL,
  solved_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  source TEXT NOT NULL DEFAULT 'web',
  PRIMARY KEY (google_sub, challenge_id),
  FOREIGN KEY (google_sub) REFERENCES users(google_sub) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS solves_by_user ON solves(google_sub, solved_at);

CREATE TABLE IF NOT EXISTS challenge_flags (
  challenge_id TEXT PRIMARY KEY,
  flag_hash TEXT NOT NULL,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

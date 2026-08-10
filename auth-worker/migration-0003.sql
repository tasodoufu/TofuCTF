CREATE TABLE IF NOT EXISTS challenge_flags (
  challenge_id TEXT PRIMARY KEY,
  flag_hash TEXT NOT NULL,
  updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
INSERT INTO challenge_flags (challenge_id, flag_hash)
VALUES ('warm-tofu', '615a515b69753cbce23bd9407c71b79a2302984bb6280a9bb0229c8995dd25a8')
ON CONFLICT(challenge_id) DO UPDATE SET flag_hash=excluded.flag_hash, updated_at=CURRENT_TIMESTAMP;

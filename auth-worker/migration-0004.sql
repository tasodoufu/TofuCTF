INSERT INTO challenge_flags (challenge_id, flag_hash)
VALUES
  ('warm-tofu', 'a69b794f2592a777efe8b07facd9f7166dcb8ee6424b92646361302419863e3e'),
  ('cool-tofu', '5d6ce6a69616863f6004f7a7fe4105b9d93f37f355ed8ec986d418537ed48ea9')
ON CONFLICT(challenge_id) DO UPDATE SET
  flag_hash=excluded.flag_hash,
  updated_at=CURRENT_TIMESTAMP;

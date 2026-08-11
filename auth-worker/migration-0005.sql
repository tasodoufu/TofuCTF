UPDATE challenge_flags
SET flag_hash = CASE challenge_id
  WHEN 'warm-tofu' THEN 'b86081267d0f684775708a9df4912f7237e69b79c4705c009439a9fa407e30d1'
  WHEN 'cool-tofu' THEN '1c6683eb1704459095dfada2efed0c66ac22c03d75769aa1f9dceedc08ffc360'
END,
updated_at = CURRENT_TIMESTAMP
WHERE challenge_id IN ('warm-tofu', 'cool-tofu');

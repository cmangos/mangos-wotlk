
-- Fix spell 46338 (quest 11881)
DELETE FROM area_group_template WHERE id=113;
INSERT INTO area_group_template VALUES
(113, 4116, 4117, 4119, 4035, 4037, 4043, 0);

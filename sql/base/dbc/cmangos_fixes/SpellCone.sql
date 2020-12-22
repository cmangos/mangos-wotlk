-- romulo hits targets in the back and does not turn at all
-- Pain and Suffering: Lich King hits targets behind him: 74117
UPDATE spell_cone SET ConeDegrees=-180 WHERE id IN(30815,74117);
DELETE FROM spell_cone WHERE id IN(48562); -- reused in Cataclysm for a different purpose


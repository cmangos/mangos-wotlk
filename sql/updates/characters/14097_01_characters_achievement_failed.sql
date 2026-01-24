ALTER TABLE character_db_version CHANGE COLUMN required_14087_01_characters_equip_size required_14097_01_characters_achievement_failed bit;

ALTER TABLE `character_achievement_progress` ADD COLUMN `failed` TINYINT UNSIGNED DEFAULT 0 AFTER `date`;


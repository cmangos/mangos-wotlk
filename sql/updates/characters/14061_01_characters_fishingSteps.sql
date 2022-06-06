ALTER TABLE character_db_version CHANGE COLUMN required_14044_01_characters_extend_state required_14061_01_characters_fishingSteps bit;

ALTER TABLE characters ADD COLUMN `fishingSteps` TINYINT UNSIGNED NOT NULL DEFAULT '0' AFTER `grantableLevels`;



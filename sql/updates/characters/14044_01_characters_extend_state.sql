ALTER TABLE character_db_version CHANGE COLUMN required_14035_01_characters_raf required_14044_01_characters_extend_state bit;

ALTER TABLE character_instance ADD COLUMN ExtendState TINYINT UNSIGNED DEFAULT '1' COMMENT 'Extended Lock' AFTER permanent;



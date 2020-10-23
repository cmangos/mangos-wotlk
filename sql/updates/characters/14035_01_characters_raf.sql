ALTER TABLE character_db_version CHANGE COLUMN required_14030_01_characters_item_instance_duration_default required_14035_01_characters_raf bit;

ALTER TABLE characters ADD COLUMN grantableLevels INT UNSIGNED DEFAULT '0' AFTER actionBars;


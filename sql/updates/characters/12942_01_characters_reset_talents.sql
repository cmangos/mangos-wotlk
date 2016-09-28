ALTER TABLE character_db_version CHANGE COLUMN required_12937_01_characters_pvpstats required_12942_01_characters_reset_talents bit;

UPDATE characters SET at_login = at_login | 0x4;

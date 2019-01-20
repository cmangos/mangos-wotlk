ALTER TABLE character_db_version CHANGE COLUMN required_13991_01_characters_skills_cleanup_1829 required_13993_01_characters_fix_skills_overflow bit;

UPDATE `character_skills` SET `value` = `max` WHERE `value` > `max`;

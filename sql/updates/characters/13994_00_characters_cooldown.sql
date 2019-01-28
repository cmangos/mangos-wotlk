ALTER TABLE character_db_version CHANGE COLUMN required_13993_01_characters_fix_skills_overflow required_13994_00_characters_cooldown bit;

ALTER TABLE character_spell_cooldown CHANGE COLUMN `LowGuid` `guid` int(11);

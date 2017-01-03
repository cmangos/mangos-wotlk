ALTER TABLE character_db_version CHANGE COLUMN required_12942_01_characters_reset_talents required_12949_01_characters_mangle_cleanup bit;

UPDATE characters as c JOIN character_spell as cs ON c.guid = cs.guid SET c.at_login = c.at_login | 4 WHERE cs.spell in (33876,33878,33982,33983,33986,33987,48563,48564,48565,48566);
DELETE FROM character_spell WHERE spell in (33876,33878,33982,33983,33986,33987,48563,48564,48565,48566);

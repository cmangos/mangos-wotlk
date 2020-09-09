ALTER TABLE db_version CHANGE COLUMN required_14019_01_mangos_ability_sets required_14021_01_mangos_command_ahbot bit;

DELETE FROM command WHERE name LIKE 'ahbot%';

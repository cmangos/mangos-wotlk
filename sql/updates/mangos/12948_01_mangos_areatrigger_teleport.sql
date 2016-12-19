ALTER TABLE db_version CHANGE COLUMN required_12947_01_mangos_game_event required_12948_01_mangos_areatrigger_teleport bit;

ALTER TABLE areatrigger_teleport ADD COLUMN `condition_id` INT(11) unsigned NOT NULL default '0' AFTER target_orientation;

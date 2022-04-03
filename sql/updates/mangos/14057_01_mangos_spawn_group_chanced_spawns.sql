ALTER TABLE db_version CHANGE COLUMN required_14056_01_mangos_dbscript_extension required_14057_01_mangos_spawn_group_chanced_spawns bit;

ALTER TABLE spawn_group_spawn ADD COLUMN `Chance` INT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Chance for a spawn to occur';



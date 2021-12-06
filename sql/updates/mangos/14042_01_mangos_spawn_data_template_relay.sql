ALTER TABLE db_version CHANGE COLUMN required_14041_01_mangos_creature_spell_lists required_14042_01_mangos_spawn_data_template_relay bit;

ALTER TABLE creature_spawn_data_template ADD COLUMN RelayId INT UNSIGNED NOT NULL COMMENT 'dbscripts_on_relay' AFTER SpawnFlags;


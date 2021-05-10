ALTER TABLE db_version CHANGE COLUMN required_14033_01_mangos_command required_14034_01_mangos_column_fix bit;

ALTER TABLE creature_spawn_data_template MODIFY CurHealth INT UNSIGNED NOT NULL DEFAULT '0';



ALTER TABLE db_version CHANGE COLUMN required_12821_01_mangos_command required_12823_01_mangos_command bit;

DELETE FROM command WHERE name='wp import';

ALTER TABLE db_version CHANGE COLUMN required_13959_01_mangos_dbscript_random_template required_13960_01_mangos_dbscript_string_rename bit;

DROP TABLE IF EXISTS `dbscript_string`;
RENAME TABLE `db_script_string` TO `dbscript_string`;

ALTER TABLE conditions
ADD comments varchar(300) AFTER value2;



ALTER TABLE db_version CHANGE COLUMN required_12235_01_mangos_mangos_string required_12258_01_mangos_mangos_string bit;

INSERT INTO mangos_string VALUES
(1509,'Pool %u not found (Maximal pool entry is %u)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

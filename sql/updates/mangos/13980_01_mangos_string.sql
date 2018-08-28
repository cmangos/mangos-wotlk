ALTER TABLE db_version CHANGE COLUMN required_13979_01_mangos_creature_conditional_spawn required_13980_01_mangos_string bit;

DELETE FROM mangos_string WHERE entry IN (555);
INSERT INTO mangos_string VALUES
(555,'Phase Mask: %u.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

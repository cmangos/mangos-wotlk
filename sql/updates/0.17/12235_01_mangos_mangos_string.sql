ALTER TABLE db_version CHANGE COLUMN required_12233_01_mangos_mangos_string required_12235_01_mangos_mangos_string bit;

INSERT INTO mangos_string VALUES
(1194,'Current State Information: GOState %u, LootState %u. Collision %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1195,'Current State Information: GOState %u, LootState %u. Collision %s, (door %s by default)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

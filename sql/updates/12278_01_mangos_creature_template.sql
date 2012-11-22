ALTER TABLE db_version CHANGE COLUMN required_12269_14_mangos_gossip_menu_option required_12278_01_mangos_creature_template bit;

ALTER TABLE creature_template DROP COLUMN spell1, DROP COLUMN spell2, DROP COLUMN spell3, DROP COLUMN spell4;

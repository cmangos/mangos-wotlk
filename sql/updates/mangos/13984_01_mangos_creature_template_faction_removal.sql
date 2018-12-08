ALTER TABLE db_version CHANGE COLUMN required_13983_01_mangos_cones required_13984_01_mangos_creature_template_faction_removal bit;

ALTER TABLE creature_template DROP COLUMN FactionHorde;
ALTER TABLE creature_template CHANGE FactionAlliance Faction SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0';


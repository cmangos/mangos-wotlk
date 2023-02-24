ALTER TABLE db_version CHANGE COLUMN required_14069_01_mangos_addon_rename required_14070_01_mangos_creature_drops bit;

ALTER TABLE creature DROP COLUMN modelid;
ALTER TABLE creature DROP COLUMN currentwaypoint;
ALTER TABLE creature DROP COLUMN curhealth;
ALTER TABLE creature DROP COLUMN curmana;
ALTER TABLE creature DROP COLUMN DeathState;



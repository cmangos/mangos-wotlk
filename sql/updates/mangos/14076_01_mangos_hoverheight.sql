ALTER TABLE db_version CHANGE COLUMN required_14075_01_mangos_creature_static_flags required_14076_01_mangos_hoverheight bit;

ALTER TABLE creature_template ADD COLUMN `HoverHeight` float NOT NULL DEFAULT 1 AFTER `SpeedRun`;


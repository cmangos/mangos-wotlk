ALTER TABLE db_version CHANGE COLUMN required_14048_01_mangos_waypoint_path required_14049_01_mangos_unitflags2 bit;

ALTER TABLE creature_template ADD COLUMN `UnitFlags2` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER UnitFlags;



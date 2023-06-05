ALTER TABLE db_version CHANGE COLUMN required_14079_01_mangos_worldstate_expression_spawn_group required_14080_01_mangos_pursuit bit;

UPDATE creature_template SET Pursuit=15000 WHERE Pursuit=0;



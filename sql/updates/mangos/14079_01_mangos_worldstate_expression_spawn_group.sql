ALTER TABLE db_version CHANGE COLUMN required_14078_01_mangos_visibility required_14079_01_mangos_worldstate_expression_spawn_group bit;

ALTER TABLE `spawn_group` ADD COLUMN `WorldStateExpression` INT(11) NOT NULL DEFAULT 0 AFTER `WorldState`;



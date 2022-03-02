ALTER TABLE db_version CHANGE COLUMN required_14050_01_mangos_npc_vendor required_14051_01_mangos_creature_addon bit;

ALTER TABLE `creature_template` ADD COLUMN `InteractionPauseTimer` INT(10) DEFAULT -1 NOT NULL AFTER `GossipMenuId`;


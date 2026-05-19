ALTER TABLE db_version CHANGE COLUMN required_14098_01_mangos_wmogroupgen required_14099_01_mangos_quest_template_additions bit;

ALTER TABLE `quest_template` ADD COLUMN `RewFactionFlags` INT UNSIGNED DEFAULT '0' AFTER `RewMaxRepValue5`;
ALTER TABLE `quest_template` ADD COLUMN `RewReputationMask` INT UNSIGNED DEFAULT '0' AFTER `RewFactionFlags`;
ALTER TABLE `quest_template` ADD COLUMN `RewArenaPoints` INT UNSIGNED DEFAULT '0' AFTER `RewHonorMultiplier`;


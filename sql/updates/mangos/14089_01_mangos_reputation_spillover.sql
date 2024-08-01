ALTER TABLE db_version CHANGE COLUMN required_14088_01_mangos_spawn_group_squad required_14089_01_mangos_reputation_spillover bit;

ALTER TABLE quest_template ADD COLUMN ReputationSpilloverMask tinyint unsigned NOT NULL DEFAULT '0' AFTER `RewMaxRepValue5`;

ALTER TABLE db_version CHANGE COLUMN required_14099_01_mangos_quest_template_additions required_14100_01_mangos_spillover_drop bit;

-- column i added is official naming
UPDATE quest_template SET RewFactionFlags=ReputationSpilloverMask WHERE ReputationSpilloverMask> 0;

ALTER TABLE `quest_template` DROP COLUMN `ReputationSpilloverMask`;


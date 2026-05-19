ALTER TABLE db_version CHANGE COLUMN required_14100_01_mangos_spillover_drop required_14101_01_mangos_quest_template_mistake_rename bit;

ALTER TABLE `quest_template` CHANGE `RewReputationMask` `RewUnk` INT UNSIGNED DEFAULT '0';

UPDATE quest_template SET RewUnk=8 WHERE RewUnk=0; -- unused in client and always sends 8 in 3.3.5


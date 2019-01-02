ALTER TABLE db_version CHANGE COLUMN required_13985_01_mangos_seal_of_righteousness_restored required_13986_01_mangos_seal_of_righteousness_cleanup bit;

DELETE FROM `npc_trainer_template` WHERE `spell` IN (20154, 20271, 21084);

DELETE FROM `npc_trainer` WHERE `spell` IN (20154, 20271, 21084);


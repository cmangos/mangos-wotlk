ALTER TABLE db_version CHANGE COLUMN required_13986_01_mangos_seal_of_righteousness_cleanup required_13987_01_mangos_seal_of_righteousness_wrath bit;

-- Revert Judgement traching spell back to Judgement based on video data for wrath
UPDATE `npc_trainer_template` SET `spell` = 20271 WHERE `spell` = 10321;

UPDATE `npc_trainer` SET `spell` = 20271 WHERE `spell` = 10321;

-- Insert Seal of Righteousness Rank 1.5 back to trainer lists for 20 copper available at lvl 1 based on video data for wrath
DELETE FROM `npc_trainer_template` WHERE `spell` = 21084;
INSERT INTO `npc_trainer_template` VALUES
(20,21084,20,0,0,0,0),
(21,21084,20,0,0,0,0);

-- Delete Seal of Righteousness Rank 1.5 from "spell learn spells" table: not needed, trained separately in wrath
DELETE FROM `spell_learn_spell` WHERE `SpellID` = 21084;


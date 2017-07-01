ALTER TABLE db_version CHANGE COLUMN required_13955_01_mangos_spell_chain_totems required_13956_01_mangos_spell_chain_totems_typos bit;

DELETE FROM `spell_chain` WHERE `spell_id` IN (10534, 58750);
INSERT INTO `spell_chain` VALUES
(10534,8185,8185,2,0),
(58750,58748,10596,6,0);

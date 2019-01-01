ALTER TABLE db_version CHANGE COLUMN required_13984_01_mangos_creature_template_faction_removal required_13985_01_mangos_seal_of_righteousness_restored bit;

DELETE FROM `spell_chain` WHERE `spell_id` IN (20154, 21084);

UPDATE `playercreateinfo_action` SET `action` = 20154 WHERE `action` = 21084 AND `type` = 0;

UPDATE `playercreateinfo_spell` SET `Spell` = 20154 WHERE `Spell` = 21084;

UPDATE `npc_trainer_template` SET `spell` = 10321 WHERE `spell` = 20271;

UPDATE `npc_trainer` SET `spell` = 10321 WHERE `spell` = 20271;

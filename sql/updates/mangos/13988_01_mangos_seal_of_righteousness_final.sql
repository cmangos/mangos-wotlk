ALTER TABLE db_version CHANGE COLUMN required_13987_01_mangos_seal_of_righteousness_wrath required_13988_01_mangos_seal_of_righteousness_final bit;

-- Reverting starting rank of Seal of Righteousness to 1.5 acorrding to 3.3.x patches (details in commit message)
UPDATE `playercreateinfo_action` SET `action` = 21084 WHERE `action` = 20154 AND `type` = 0;

UPDATE `playercreateinfo_spell` SET `Spell` = 21084 WHERE `Spell` = 20154;


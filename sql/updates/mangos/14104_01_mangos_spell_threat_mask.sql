ALTER TABLE db_version CHANGE COLUMN required_14103_01_mangos_pet_spell_lists required_14104_01_mangos_spell_threat_mask bit;

ALTER TABLE `spell_threat` ADD COLUMN `inverseEffectMask` INT UNSIGNED DEFAULT '0' AFTER `ap_bonus`;


ALTER TABLE db_version CHANGE COLUMN required_14081_01_mangos_precision_decimal required_14082_01_mangos_spell_template bit;

ALTER TABLE `spell_template`
	ADD COLUMN `EffectBonusCoefficientFromAP1` FLOAT NOT NULL DEFAULT '0' AFTER `EffectBonusCoefficient3`,
	ADD COLUMN `EffectBonusCoefficientFromAP2` FLOAT NOT NULL DEFAULT '0' AFTER `EffectBonusCoefficientFromAP1`,
	ADD COLUMN `EffectBonusCoefficientFromAP3` FLOAT NOT NULL DEFAULT '0' AFTER `EffectBonusCoefficientFromAP2`;

DROP TABLE `spell_bonus_data`;

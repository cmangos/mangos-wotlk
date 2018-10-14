ALTER TABLE db_version CHANGE COLUMN required_13982_01_mangos_event_ai required_13983_01_mangos_spell_template bit;

UPDATE `wotlkmangos`.`spell_template` SET `EffectSpellClassMask1_1` = 4097 WHERE id IN (53215, 53216, 53217);

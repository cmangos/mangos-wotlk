ALTER TABLE db_version CHANGE COLUMN required_14104_01_mangos_spell_threat_mask required_14105_01_mangos_king_of_the_jungle bit;

-- King of the Jungle: Enrage and Tiger's Fury are abilities (SPELL_ATTR_IS_ABILITY), they proc with PROC_FLAG_DEAL_HELPFUL_ABILITY
UPDATE `spell_proc_event` SET `procFlags` = 0x00000400 WHERE `entry` = 48492;

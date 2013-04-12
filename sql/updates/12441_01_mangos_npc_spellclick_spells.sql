ALTER TABLE db_version CHANGE COLUMN required_12440_01_mangos_spell_area required_12441_01_mangos_npc_spellclick_spells bit;

ALTER TABLE npc_spellclick_spells ADD COLUMN `condition_id` mediumint(8) unsigned NOT NULL default '0' AFTER cast_flags;

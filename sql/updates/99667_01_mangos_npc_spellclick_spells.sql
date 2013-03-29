ALTER TABLE npc_spellclick_spells ADD COLUMN `condition_id` mediumint(8) unsigned NOT NULL default '0' AFTER cast_flags;

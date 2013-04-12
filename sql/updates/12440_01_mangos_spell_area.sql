ALTER TABLE db_version CHANGE COLUMN required_12434_01_mangos_spell_template required_12440_01_mangos_spell_area bit;

ALTER TABLE spell_area ADD COLUMN `condition_id` mediumint(8) unsigned NOT NULL default '0' AFTER quest_end;

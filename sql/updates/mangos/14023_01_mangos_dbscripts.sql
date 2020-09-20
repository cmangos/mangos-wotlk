ALTER TABLE db_version CHANGE COLUMN required_14022_01_mangos_playercreateinfo_spell required_14023_01_mangos_dbscripts bit;

ALTER TABLE dbscripts_on_creature_death MODIFY datalong int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE dbscripts_on_creature_movement MODIFY datalong int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE dbscripts_on_event MODIFY datalong int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE dbscripts_on_go_use MODIFY datalong int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE dbscripts_on_go_template_use MODIFY datalong int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE dbscripts_on_gossip MODIFY datalong int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE dbscripts_on_quest_end MODIFY datalong int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE dbscripts_on_quest_start MODIFY datalong int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE dbscripts_on_relay MODIFY datalong int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE dbscripts_on_spell MODIFY datalong int(10) unsigned NOT NULL DEFAULT '0';

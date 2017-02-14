ALTER TABLE character_db_version CHANGE COLUMN required_13961_01_characters_spell_cooldown required_13962_01_characters_event_group_chosen bit;

DROP TABLE IF EXISTS `event_group_chosen`;
CREATE TABLE `event_group_chosen` (
`eventGroup` mediumint(8) unsigned NOT NULL DEFAULT '0',
`entry` mediumint(8) unsigned NOT NULL DEFAULT '0',
PRIMARY KEY (`eventGroup`,`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Quest Group picked';



ALTER TABLE character_db_version CHANGE COLUMN required_13957_01_characters_playerbot_talentspec required_13957_02_characters_playerbot_saved_data bit;

-- ----------------------------
-- Table structure for `playerbot_saved_data`
-- ----------------------------
DROP TABLE IF EXISTS `playerbot_saved_data`;
CREATE TABLE `playerbot_saved_data` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0',
  `combat_order` int(11) unsigned NOT NULL DEFAULT '0',
  `primary_target` int(11) unsigned NOT NULL DEFAULT '0',
  `secondary_target` int(11) unsigned NOT NULL DEFAULT '0',
  `pname` varchar(12) NOT NULL DEFAULT '',
  `sname` varchar(12) NOT NULL DEFAULT '',
  `combat_delay` INT(11) unsigned NOT NULL DEFAULT '0',
  `auto_follow` INT(11) unsigned NOT NULL DEFAULT '1',
  `autoequip` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Persistent Playerbot settings per alt';


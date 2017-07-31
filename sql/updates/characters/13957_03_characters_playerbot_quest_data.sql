ALTER TABLE character_db_version CHANGE COLUMN required_13957_02_characters_playerbot_saved_data required_13957_03_characters_playerbot_quest_data bit;

-- ----------------------------
-- Table structure for `playerbot_quest_data`
-- ----------------------------
DROP TABLE IF EXISTS `playerbot_quest_data`;
CREATE TABLE `playerbot_quest_data` (
  `autocomplete` mediumint(8) unsigned NOT NULL,
  PRIMARY KEY (`autocomplete`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

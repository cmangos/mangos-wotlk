ALTER TABLE character_db_version CHANGE COLUMN required_14097_01_characters_achievement_failed required_14105_01_characters_playerbot_tables_drop bit;

DROP TABLE IF EXISTS `playerbot_talentspec`;
DROP TABLE IF EXISTS `playerbot_saved_data`;
DROP TABLE IF EXISTS `playerbot_quest_data`;

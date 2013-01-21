ALTER TABLE db_version CHANGE COLUMN required_12338_01_mangos_character_calendar required_12339_01_mangos_calendar bit;

DROP TABLE IF EXISTS `calendar_events`;
DROP TABLE IF EXISTS `calendar_invites`;

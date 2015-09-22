ALTER TABLE character_db_version CHANGE COLUMN required_12339_01_characters_calendar_events required_12339_02_characters_calendar_invites bit;

DROP TABLE IF EXISTS `calendar_invites`;
CREATE TABLE `calendar_invites` (
  `inviteId` bigint(10) unsigned NOT NULL DEFAULT '0',
  `eventId` bigint(10) unsigned NOT NULL DEFAULT '0',
  `inviteeGuid` int(10) unsigned NOT NULL DEFAULT '0',
  `senderGuid` int(3) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(10) unsigned NOT NULL DEFAULT '0',
  `lastUpdateTime` int(10) unsigned NOT NULL DEFAULT '0',
  `rank` tinyint(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY  (`inviteId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
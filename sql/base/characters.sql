-- MySQL dump 10.13
--
-- Host: localhost    Database: characters
-- ------------------------------------------------------
-- Server version	5.5.32

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `character_db_version`
--

DROP TABLE IF EXISTS `character_db_version`;
CREATE TABLE `character_db_version` (
  `required_14035_01_characters_raf` bit(1) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Last applied sql update to DB';

--
-- Dumping data for table `character_db_version`
--

LOCK TABLES `character_db_version` WRITE;
/*!40000 ALTER TABLE `character_db_version` DISABLE KEYS */;
INSERT INTO `character_db_version` VALUES
(NULL);
/*!40000 ALTER TABLE `character_db_version` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ahbot_items`
--

DROP TABLE IF EXISTS `ahbot_items`;
CREATE TABLE `ahbot_items` (
  `item` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Item Identifier',
  `value` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Item value, a value of 0 bans item from being sold/bought by AHBot',
  `add_chance` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Chance item is added to AH upon bot visit, 0 for normal loot sources',
  `min_amount` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Min amount added, not used when add_chance is 0',
  `max_amount` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Max amount added, not used when add_chance is 0',
PRIMARY KEY (`item`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='AuctionHouseBot overridden item settings';

--
-- Table structure for table `account_data`
--

DROP TABLE IF EXISTS `account_data`;
CREATE TABLE `account_data` (
  `account` int(11) unsigned NOT NULL DEFAULT '0',
  `type` int(11) unsigned NOT NULL DEFAULT '0',
  `time` bigint(11) unsigned NOT NULL DEFAULT '0',
  `data` longblob NOT NULL,
  PRIMARY KEY (`account`,`type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `account_data`
--

LOCK TABLES `account_data` WRITE;
/*!40000 ALTER TABLE `account_data` DISABLE KEYS */;
/*!40000 ALTER TABLE `account_data` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `account_instances_entered`
--

DROP TABLE IF EXISTS `account_instances_entered`;
CREATE TABLE `account_instances_entered` (
   `AccountId` INT(11) UNSIGNED NOT NULL COMMENT 'Player account',
   `ExpireTime` BIGINT(40) NOT NULL COMMENT 'Time when instance was entered',
   `InstanceId` INT(11) UNSIGNED NOT NULL COMMENT 'ID of instance entered',
   PRIMARY KEY(`AccountId`,`InstanceId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='Instance reset limit system';

--
-- Dumping data for table `account_instances_entered`
--

LOCK TABLES `account_instances_entered` WRITE;
/*!40000 ALTER TABLE `account_instances_entered` DISABLE KEYS */;
/*!40000 ALTER TABLE `account_instances_entered` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `arena_team`
--

DROP TABLE IF EXISTS `arena_team`;
CREATE TABLE `arena_team` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `name` char(255) NOT NULL,
  `captainguid` int(10) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `BackgroundColor` int(10) unsigned NOT NULL DEFAULT '0',
  `EmblemStyle` int(10) unsigned NOT NULL DEFAULT '0',
  `EmblemColor` int(10) unsigned NOT NULL DEFAULT '0',
  `BorderStyle` int(10) unsigned NOT NULL DEFAULT '0',
  `BorderColor` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`arenateamid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `arena_team`
--

LOCK TABLES `arena_team` WRITE;
/*!40000 ALTER TABLE `arena_team` DISABLE KEYS */;
/*!40000 ALTER TABLE `arena_team` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `arena_team_member`
--

DROP TABLE IF EXISTS `arena_team_member`;
CREATE TABLE `arena_team_member` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `guid` int(10) unsigned NOT NULL DEFAULT '0',
  `played_week` int(10) unsigned NOT NULL DEFAULT '0',
  `wons_week` int(10) unsigned NOT NULL DEFAULT '0',
  `played_season` int(10) unsigned NOT NULL DEFAULT '0',
  `wons_season` int(10) unsigned NOT NULL DEFAULT '0',
  `personal_rating` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`arenateamid`,`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `arena_team_member`
--

LOCK TABLES `arena_team_member` WRITE;
/*!40000 ALTER TABLE `arena_team_member` DISABLE KEYS */;
/*!40000 ALTER TABLE `arena_team_member` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `arena_team_stats`
--

DROP TABLE IF EXISTS `arena_team_stats`;
CREATE TABLE `arena_team_stats` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `rating` int(10) unsigned NOT NULL DEFAULT '0',
  `games_week` int(10) unsigned NOT NULL DEFAULT '0',
  `wins_week` int(10) unsigned NOT NULL DEFAULT '0',
  `games_season` int(10) unsigned NOT NULL DEFAULT '0',
  `wins_season` int(10) unsigned NOT NULL DEFAULT '0',
  `rank` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`arenateamid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `arena_team_stats`
--

LOCK TABLES `arena_team_stats` WRITE;
/*!40000 ALTER TABLE `arena_team_stats` DISABLE KEYS */;
/*!40000 ALTER TABLE `arena_team_stats` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `auction`
--

DROP TABLE IF EXISTS `auction`;
CREATE TABLE `auction` (
  `id` int(11) unsigned NOT NULL DEFAULT '0',
  `houseid` int(11) unsigned NOT NULL DEFAULT '0',
  `itemguid` int(11) unsigned NOT NULL DEFAULT '0',
  `item_template` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Item Identifier',
  `item_count` int(11) unsigned NOT NULL DEFAULT '0',
  `item_randompropertyid` int(11) NOT NULL DEFAULT '0',
  `itemowner` int(11) unsigned NOT NULL DEFAULT '0',
  `buyoutprice` int(11) NOT NULL DEFAULT '0',
  `time` bigint(40) unsigned NOT NULL DEFAULT '0',
  `moneyTime` bigint(40) unsigned NOT NULL DEFAULT '0',
  `buyguid` int(11) unsigned NOT NULL DEFAULT '0',
  `lastbid` int(11) NOT NULL DEFAULT '0',
  `startbid` int(11) NOT NULL DEFAULT '0',
  `deposit` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `auction`
--

LOCK TABLES `auction` WRITE;
/*!40000 ALTER TABLE `auction` DISABLE KEYS */;
/*!40000 ALTER TABLE `auction` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `bugreport`
--

DROP TABLE IF EXISTS `bugreport`;
CREATE TABLE `bugreport` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `type` longtext NOT NULL,
  `content` longtext NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Debug System';

--
-- Dumping data for table `bugreport`
--

LOCK TABLES `bugreport` WRITE;
/*!40000 ALTER TABLE `bugreport` DISABLE KEYS */;
/*!40000 ALTER TABLE `bugreport` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `calendar_events`
--

DROP TABLE IF EXISTS `calendar_events`;
CREATE TABLE `calendar_events` (
  `eventId` bigint(10) unsigned NOT NULL DEFAULT '0',
  `creatorGuid` int(10) unsigned NOT NULL DEFAULT '0',
  `guildId` int(10) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '4',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `dungeonId` int(10) NOT NULL DEFAULT '-1',
  `eventTime` int(10) unsigned NOT NULL DEFAULT '0',
  `title` varchar(128) NOT NULL DEFAULT '',
  `description` varchar(1024) NOT NULL DEFAULT '',
  PRIMARY KEY (`eventId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `calendar_events`
--

LOCK TABLES `calendar_events` WRITE;
/*!40000 ALTER TABLE `calendar_events` DISABLE KEYS */;
/*!40000 ALTER TABLE `calendar_events` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `calendar_invites`
--

DROP TABLE IF EXISTS `calendar_invites`;
CREATE TABLE `calendar_invites` (
  `inviteId` bigint(10) unsigned NOT NULL DEFAULT '0',
  `eventId` bigint(10) unsigned NOT NULL DEFAULT '0',
  `inviteeGuid` int(10) unsigned NOT NULL DEFAULT '0',
  `senderGuid` int(3) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(10) unsigned NOT NULL DEFAULT '0',
  `lastUpdateTime` int(10) unsigned NOT NULL DEFAULT '0',
  `rank` tinyint(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`inviteId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `calendar_invites`
--

LOCK TABLES `calendar_invites` WRITE;
/*!40000 ALTER TABLE `calendar_invites` DISABLE KEYS */;
/*!40000 ALTER TABLE `calendar_invites` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_account_data`
--

DROP TABLE IF EXISTS `character_account_data`;
CREATE TABLE `character_account_data` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0',
  `type` int(11) unsigned NOT NULL DEFAULT '0',
  `time` bigint(11) unsigned NOT NULL DEFAULT '0',
  `data` longblob NOT NULL,
  PRIMARY KEY (`guid`,`type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `character_account_data`
--

LOCK TABLES `character_account_data` WRITE;
/*!40000 ALTER TABLE `character_account_data` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_account_data` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_achievement`
--

DROP TABLE IF EXISTS `character_achievement`;
CREATE TABLE `character_achievement` (
  `guid` int(11) unsigned NOT NULL,
  `achievement` int(11) unsigned NOT NULL,
  `date` bigint(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`achievement`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `character_achievement`
--

LOCK TABLES `character_achievement` WRITE;
/*!40000 ALTER TABLE `character_achievement` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_achievement` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_achievement_progress`
--

DROP TABLE IF EXISTS `character_achievement_progress`;
CREATE TABLE `character_achievement_progress` (
  `guid` int(11) unsigned NOT NULL,
  `criteria` int(11) unsigned NOT NULL,
  `counter` int(11) unsigned NOT NULL,
  `date` bigint(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`criteria`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `character_achievement_progress`
--

LOCK TABLES `character_achievement_progress` WRITE;
/*!40000 ALTER TABLE `character_achievement_progress` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_achievement_progress` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_action`
--

DROP TABLE IF EXISTS `character_action`;
CREATE TABLE `character_action` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `spec` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `button` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `action` int(11) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`spec`,`button`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `character_action`
--

LOCK TABLES `character_action` WRITE;
/*!40000 ALTER TABLE `character_action` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_action` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_aura`
--

DROP TABLE IF EXISTS `character_aura`;
CREATE TABLE `character_aura` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `caster_guid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'Full Global Unique Identifier',
  `item_guid` int(11) unsigned NOT NULL DEFAULT '0',
  `spell` int(11) unsigned NOT NULL DEFAULT '0',
  `stackcount` int(11) unsigned NOT NULL DEFAULT '1',
  `remaincharges` int(11) unsigned NOT NULL DEFAULT '0',
  `basepoints0` int(11) NOT NULL DEFAULT '0',
  `basepoints1` int(11) NOT NULL DEFAULT '0',
  `basepoints2` int(11) NOT NULL DEFAULT '0',
  `periodictime0` int(11) unsigned NOT NULL DEFAULT '0',
  `periodictime1` int(11) unsigned NOT NULL DEFAULT '0',
  `periodictime2` int(11) unsigned NOT NULL DEFAULT '0',
  `maxduration` int(11) NOT NULL DEFAULT '0',
  `remaintime` int(11) NOT NULL DEFAULT '0',
  `effIndexMask` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`caster_guid`,`item_guid`,`spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `character_aura`
--

LOCK TABLES `character_aura` WRITE;
/*!40000 ALTER TABLE `character_aura` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_aura` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_battleground_data`
--

DROP TABLE IF EXISTS `character_battleground_data`;
CREATE TABLE `character_battleground_data` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `instance_id` int(11) unsigned NOT NULL DEFAULT '0',
  `team` int(11) unsigned NOT NULL DEFAULT '0',
  `join_x` float NOT NULL DEFAULT '0',
  `join_y` float NOT NULL DEFAULT '0',
  `join_z` float NOT NULL DEFAULT '0',
  `join_o` float NOT NULL DEFAULT '0',
  `join_map` int(11) NOT NULL DEFAULT '0',
  `mount_spell` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `character_battleground_data`
--

LOCK TABLES `character_battleground_data` WRITE;
/*!40000 ALTER TABLE `character_battleground_data` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_battleground_data` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_battleground_random`
--

DROP TABLE IF EXISTS `character_battleground_random`;
CREATE TABLE `character_battleground_random` (
  `guid` int(11) unsigned NOT NULL default '0' COMMENT 'Global Unique Identifier',
  PRIMARY KEY  (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player Battleground System';

--
-- Dumping data for table `character_battleground_random`
--

LOCK TABLES `character_battleground_random` WRITE;
/*!40000 ALTER TABLE `character_battleground_random` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_battleground_random` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_declinedname`
--

DROP TABLE IF EXISTS `character_declinedname`;
CREATE TABLE `character_declinedname` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `genitive` varchar(15) NOT NULL DEFAULT '',
  `dative` varchar(15) NOT NULL DEFAULT '',
  `accusative` varchar(15) NOT NULL DEFAULT '',
  `instrumental` varchar(15) NOT NULL DEFAULT '',
  `prepositional` varchar(15) NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `character_declinedname`
--

LOCK TABLES `character_declinedname` WRITE;
/*!40000 ALTER TABLE `character_declinedname` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_declinedname` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_equipmentsets`
--

DROP TABLE IF EXISTS `character_equipmentsets`;
CREATE TABLE `character_equipmentsets` (
  `guid` int(11) NOT NULL DEFAULT '0',
  `setguid` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `setindex` tinyint(4) NOT NULL DEFAULT '0',
  `name` varchar(100) NOT NULL,
  `iconname` varchar(100) NOT NULL,
  `ignore_mask` int(11) unsigned NOT NULL DEFAULT '0',
  `item0` int(11) unsigned NOT NULL DEFAULT '0',
  `item1` int(11) unsigned NOT NULL DEFAULT '0',
  `item2` int(11) unsigned NOT NULL DEFAULT '0',
  `item3` int(11) unsigned NOT NULL DEFAULT '0',
  `item4` int(11) unsigned NOT NULL DEFAULT '0',
  `item5` int(11) unsigned NOT NULL DEFAULT '0',
  `item6` int(11) unsigned NOT NULL DEFAULT '0',
  `item7` int(11) unsigned NOT NULL DEFAULT '0',
  `item8` int(11) unsigned NOT NULL DEFAULT '0',
  `item9` int(11) unsigned NOT NULL DEFAULT '0',
  `item10` int(11) unsigned NOT NULL DEFAULT '0',
  `item11` int(11) unsigned NOT NULL DEFAULT '0',
  `item12` int(11) unsigned NOT NULL DEFAULT '0',
  `item13` int(11) unsigned NOT NULL DEFAULT '0',
  `item14` int(11) unsigned NOT NULL DEFAULT '0',
  `item15` int(11) unsigned NOT NULL DEFAULT '0',
  `item16` int(11) unsigned NOT NULL DEFAULT '0',
  `item17` int(11) unsigned NOT NULL DEFAULT '0',
  `item18` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`setguid`),
  UNIQUE KEY `idx_set` (`guid`,`setguid`,`setindex`),
  KEY `Idx_setindex` (`setindex`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `character_equipmentsets`
--

LOCK TABLES `character_equipmentsets` WRITE;
/*!40000 ALTER TABLE `character_equipmentsets` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_equipmentsets` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_gifts`
--

DROP TABLE IF EXISTS `character_gifts`;
CREATE TABLE `character_gifts` (
  `guid` int(20) unsigned NOT NULL DEFAULT '0',
  `item_guid` int(11) unsigned NOT NULL DEFAULT '0',
  `entry` int(20) unsigned NOT NULL DEFAULT '0',
  `flags` int(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`item_guid`),
  KEY `idx_guid` (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `character_gifts`
--

LOCK TABLES `character_gifts` WRITE;
/*!40000 ALTER TABLE `character_gifts` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_gifts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_glyphs`
--

DROP TABLE IF EXISTS `character_glyphs`;
CREATE TABLE `character_glyphs` (
  `guid` int(11) unsigned NOT NULL,
  `spec` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `slot` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `glyph` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`spec`,`slot`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `character_glyphs`
--

LOCK TABLES `character_glyphs` WRITE;
/*!40000 ALTER TABLE `character_glyphs` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_glyphs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_homebind`
--

DROP TABLE IF EXISTS `character_homebind`;
CREATE TABLE `character_homebind` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `map` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Map Identifier',
  `zone` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Zone Identifier',
  `position_x` float NOT NULL DEFAULT '0',
  `position_y` float NOT NULL DEFAULT '0',
  `position_z` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `character_homebind`
--

LOCK TABLES `character_homebind` WRITE;
/*!40000 ALTER TABLE `character_homebind` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_homebind` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_instance`
--

DROP TABLE IF EXISTS `character_instance`;
CREATE TABLE `character_instance` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0',
  `instance` int(11) unsigned NOT NULL DEFAULT '0',
  `permanent` tinyint(1) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`instance`),
  KEY `instance` (`instance`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `character_instance`
--

LOCK TABLES `character_instance` WRITE;
/*!40000 ALTER TABLE `character_instance` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_instance` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_inventory`
--

DROP TABLE IF EXISTS `character_inventory`;
CREATE TABLE `character_inventory` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `bag` int(11) unsigned NOT NULL DEFAULT '0',
  `slot` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `item` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Item Global Unique Identifier',
  `item_template` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Item Identifier',
  PRIMARY KEY (`item`),
  KEY `idx_guid` (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `character_inventory`
--

LOCK TABLES `character_inventory` WRITE;
/*!40000 ALTER TABLE `character_inventory` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_inventory` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_pet`
--

DROP TABLE IF EXISTS `character_pet`;
CREATE TABLE `character_pet` (
  `id` int(11) unsigned NOT NULL DEFAULT '0',
  `entry` int(11) unsigned NOT NULL DEFAULT '0',
  `owner` int(11) unsigned NOT NULL DEFAULT '0',
  `modelid` int(11) unsigned DEFAULT '0',
  `CreatedBySpell` int(11) unsigned NOT NULL DEFAULT '0',
  `PetType` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `level` int(11) unsigned NOT NULL DEFAULT '1',
  `exp` int(11) unsigned NOT NULL DEFAULT '0',
  `Reactstate` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `name` varchar(100) DEFAULT 'Pet',
  `renamed` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `slot` int(11) unsigned NOT NULL DEFAULT '0',
  `curhealth` int(11) unsigned NOT NULL DEFAULT '1',
  `curmana` int(11) unsigned NOT NULL DEFAULT '0',
  `curhappiness` int(11) unsigned NOT NULL DEFAULT '0',
  `savetime` bigint(20) unsigned NOT NULL DEFAULT '0',
  `resettalents_cost` int(11) unsigned NOT NULL DEFAULT '0',
  `resettalents_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `abdata` longtext,
  PRIMARY KEY (`id`),
  KEY `owner` (`owner`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Pet System';

--
-- Dumping data for table `character_pet`
--

LOCK TABLES `character_pet` WRITE;
/*!40000 ALTER TABLE `character_pet` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_pet` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_pet_declinedname`
--

DROP TABLE IF EXISTS `character_pet_declinedname`;
CREATE TABLE `character_pet_declinedname` (
  `id` int(11) unsigned NOT NULL DEFAULT '0',
  `owner` int(11) unsigned NOT NULL DEFAULT '0',
  `genitive` varchar(12) NOT NULL DEFAULT '',
  `dative` varchar(12) NOT NULL DEFAULT '',
  `accusative` varchar(12) NOT NULL DEFAULT '',
  `instrumental` varchar(12) NOT NULL DEFAULT '',
  `prepositional` varchar(12) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `owner_key` (`owner`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `character_pet_declinedname`
--

LOCK TABLES `character_pet_declinedname` WRITE;
/*!40000 ALTER TABLE `character_pet_declinedname` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_pet_declinedname` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_queststatus`
--

DROP TABLE IF EXISTS `character_queststatus`;
CREATE TABLE `character_queststatus` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `quest` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Quest Identifier',
  `status` int(11) unsigned NOT NULL DEFAULT '0',
  `rewarded` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `explored` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `timer` bigint(20) unsigned NOT NULL DEFAULT '0',
  `mobcount1` int(11) unsigned NOT NULL DEFAULT '0',
  `mobcount2` int(11) unsigned NOT NULL DEFAULT '0',
  `mobcount3` int(11) unsigned NOT NULL DEFAULT '0',
  `mobcount4` int(11) unsigned NOT NULL DEFAULT '0',
  `itemcount1` int(11) unsigned NOT NULL DEFAULT '0',
  `itemcount2` int(11) unsigned NOT NULL DEFAULT '0',
  `itemcount3` int(11) unsigned NOT NULL DEFAULT '0',
  `itemcount4` int(11) unsigned NOT NULL DEFAULT '0',
  `itemcount5` int(11) unsigned NOT NULL DEFAULT '0',
  `itemcount6` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`quest`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `character_queststatus`
--

LOCK TABLES `character_queststatus` WRITE;
/*!40000 ALTER TABLE `character_queststatus` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_queststatus` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_queststatus_daily`
--

DROP TABLE IF EXISTS `character_queststatus_daily`;
CREATE TABLE `character_queststatus_daily` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `quest` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Quest Identifier',
  PRIMARY KEY (`guid`,`quest`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `character_queststatus_daily`
--

LOCK TABLES `character_queststatus_daily` WRITE;
/*!40000 ALTER TABLE `character_queststatus_daily` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_queststatus_daily` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_queststatus_monthly`
--

DROP TABLE IF EXISTS `character_queststatus_monthly`;
CREATE TABLE `character_queststatus_monthly` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `quest` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Quest Identifier',
  PRIMARY KEY (`guid`,`quest`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `character_queststatus_monthly`
--

LOCK TABLES `character_queststatus_monthly` WRITE;
/*!40000 ALTER TABLE `character_queststatus_monthly` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_queststatus_monthly` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_queststatus_weekly`
--

DROP TABLE IF EXISTS `character_queststatus_weekly`;
CREATE TABLE `character_queststatus_weekly` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `quest` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Quest Identifier',
  PRIMARY KEY (`guid`,`quest`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `character_queststatus_weekly`
--

LOCK TABLES `character_queststatus_weekly` WRITE;
/*!40000 ALTER TABLE `character_queststatus_weekly` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_queststatus_weekly` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_reputation`
--

DROP TABLE IF EXISTS `character_reputation`;
CREATE TABLE `character_reputation` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `faction` int(11) unsigned NOT NULL DEFAULT '0',
  `standing` int(11) NOT NULL DEFAULT '0',
  `flags` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`faction`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `character_reputation`
--

LOCK TABLES `character_reputation` WRITE;
/*!40000 ALTER TABLE `character_reputation` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_reputation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_skills`
--

DROP TABLE IF EXISTS `character_skills`;
CREATE TABLE `character_skills` (
  `guid` int(11) unsigned NOT NULL COMMENT 'Global Unique Identifier',
  `skill` mediumint(9) unsigned NOT NULL,
  `value` mediumint(9) unsigned NOT NULL,
  `max` mediumint(9) unsigned NOT NULL,
  PRIMARY KEY (`guid`,`skill`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `character_skills`
--

LOCK TABLES `character_skills` WRITE;
/*!40000 ALTER TABLE `character_skills` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_skills` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_social`
--

DROP TABLE IF EXISTS `character_social`;
CREATE TABLE `character_social` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Character Global Unique Identifier',
  `friend` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Friend Global Unique Identifier',
  `flags` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT 'Friend Flags',
  `note` varchar(48) NOT NULL DEFAULT '' COMMENT 'Friend Note',
  PRIMARY KEY (`guid`,`friend`,`flags`),
  KEY `guid_flags` (`guid`,`flags`),
  KEY `friend_flags` (`friend`,`flags`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `character_social`
--

LOCK TABLES `character_social` WRITE;
/*!40000 ALTER TABLE `character_social` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_social` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_spell`
--

DROP TABLE IF EXISTS `character_spell`;
CREATE TABLE `character_spell` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `spell` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Spell Identifier',
  `active` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `disabled` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`spell`),
  KEY `Idx_spell` (`spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `character_spell`
--

LOCK TABLES `character_spell` WRITE;
/*!40000 ALTER TABLE `character_spell` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_spell` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_spell_cooldown`
--

DROP TABLE IF EXISTS `character_spell_cooldown`;
CREATE TABLE `character_spell_cooldown` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier, Low part',
  `SpellId` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Spell Identifier',
  `SpellExpireTime` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'Spell cooldown expire time',
  `Category` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Spell category',
  `CategoryExpireTime` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'Spell category cooldown expire time',
  `ItemId` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Item Identifier',
  PRIMARY KEY (`guid`,`SpellId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `character_spell_cooldown`
--

LOCK TABLES `character_spell_cooldown` WRITE;
/*!40000 ALTER TABLE `character_spell_cooldown` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_spell_cooldown` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_stats`
--

DROP TABLE IF EXISTS `character_stats`;
CREATE TABLE `character_stats` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier, Low part',
  `maxhealth` int(10) unsigned NOT NULL DEFAULT '0',
  `maxpower1` int(10) unsigned NOT NULL DEFAULT '0',
  `maxpower2` int(10) unsigned NOT NULL DEFAULT '0',
  `maxpower3` int(10) unsigned NOT NULL DEFAULT '0',
  `maxpower4` int(10) unsigned NOT NULL DEFAULT '0',
  `maxpower5` int(10) unsigned NOT NULL DEFAULT '0',
  `maxpower6` int(10) unsigned NOT NULL DEFAULT '0',
  `maxpower7` int(10) unsigned NOT NULL DEFAULT '0',
  `strength` int(10) unsigned NOT NULL DEFAULT '0',
  `agility` int(10) unsigned NOT NULL DEFAULT '0',
  `stamina` int(10) unsigned NOT NULL DEFAULT '0',
  `intellect` int(10) unsigned NOT NULL DEFAULT '0',
  `spirit` int(10) unsigned NOT NULL DEFAULT '0',
  `armor` int(10) unsigned NOT NULL DEFAULT '0',
  `resHoly` int(10) unsigned NOT NULL DEFAULT '0',
  `resFire` int(10) unsigned NOT NULL DEFAULT '0',
  `resNature` int(10) unsigned NOT NULL DEFAULT '0',
  `resFrost` int(10) unsigned NOT NULL DEFAULT '0',
  `resShadow` int(10) unsigned NOT NULL DEFAULT '0',
  `resArcane` int(10) unsigned NOT NULL DEFAULT '0',
  `blockPct` float unsigned NOT NULL DEFAULT '0',
  `dodgePct` float unsigned NOT NULL DEFAULT '0',
  `parryPct` float unsigned NOT NULL DEFAULT '0',
  `critPct` float unsigned NOT NULL DEFAULT '0',
  `rangedCritPct` float unsigned NOT NULL DEFAULT '0',
  `spellCritPct` float unsigned NOT NULL DEFAULT '0',
  `attackPower` int(10) unsigned NOT NULL DEFAULT '0',
  `rangedAttackPower` int(10) unsigned NOT NULL DEFAULT '0',
  `spellPower` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `character_stats`
--

LOCK TABLES `character_stats` WRITE;
/*!40000 ALTER TABLE `character_stats` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_stats` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_talent`
--

DROP TABLE IF EXISTS `character_talent`;
CREATE TABLE `character_talent` (
  `guid` int(11) unsigned NOT NULL,
  `talent_id` int(11) unsigned NOT NULL,
  `current_rank` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `spec` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`talent_id`,`spec`),
  KEY `talent_key` (`talent_id`),
  KEY `spec_key` (`spec`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `character_talent`
--

LOCK TABLES `character_talent` WRITE;
/*!40000 ALTER TABLE `character_talent` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_talent` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `character_tutorial`
--

DROP TABLE IF EXISTS `character_tutorial`;
CREATE TABLE `character_tutorial` (
  `account` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Account Identifier',
  `tut0` int(11) unsigned NOT NULL DEFAULT '0',
  `tut1` int(11) unsigned NOT NULL DEFAULT '0',
  `tut2` int(11) unsigned NOT NULL DEFAULT '0',
  `tut3` int(11) unsigned NOT NULL DEFAULT '0',
  `tut4` int(11) unsigned NOT NULL DEFAULT '0',
  `tut5` int(11) unsigned NOT NULL DEFAULT '0',
  `tut6` int(11) unsigned NOT NULL DEFAULT '0',
  `tut7` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `character_tutorial`
--

LOCK TABLES `character_tutorial` WRITE;
/*!40000 ALTER TABLE `character_tutorial` DISABLE KEYS */;
/*!40000 ALTER TABLE `character_tutorial` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `characters`
--

DROP TABLE IF EXISTS `characters`;
CREATE TABLE `characters` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `account` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Account Identifier',
  `name` varchar(12) NOT NULL DEFAULT '',
  `race` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `class` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `gender` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `level` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `xp` int(10) unsigned NOT NULL DEFAULT '0',
  `money` int(10) unsigned NOT NULL DEFAULT '0',
  `playerBytes` int(10) unsigned NOT NULL DEFAULT '0',
  `playerBytes2` int(10) unsigned NOT NULL DEFAULT '0',
  `playerFlags` int(10) unsigned NOT NULL DEFAULT '0',
  `position_x` float NOT NULL DEFAULT '0',
  `position_y` float NOT NULL DEFAULT '0',
  `position_z` float NOT NULL DEFAULT '0',
  `map` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Map Identifier',
  `dungeon_difficulty` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `orientation` float NOT NULL DEFAULT '0',
  `taximask` longtext,
  `online` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `cinematic` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `totaltime` int(11) unsigned NOT NULL DEFAULT '0',
  `leveltime` int(11) unsigned NOT NULL DEFAULT '0',
  `logout_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `is_logout_resting` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `rest_bonus` float NOT NULL DEFAULT '0',
  `resettalents_cost` int(11) unsigned NOT NULL DEFAULT '0',
  `resettalents_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `trans_x` float NOT NULL DEFAULT '0',
  `trans_y` float NOT NULL DEFAULT '0',
  `trans_z` float NOT NULL DEFAULT '0',
  `trans_o` float NOT NULL DEFAULT '0',
  `transguid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `extra_flags` int(11) unsigned NOT NULL DEFAULT '0',
  `stable_slots` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `at_login` int(11) unsigned NOT NULL DEFAULT '0',
  `zone` int(11) unsigned NOT NULL DEFAULT '0',
  `death_expire_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `taxi_path` text,
  `arenaPoints` int(10) unsigned NOT NULL DEFAULT '0',
  `totalHonorPoints` int(10) unsigned NOT NULL DEFAULT '0',
  `todayHonorPoints` int(10) unsigned NOT NULL DEFAULT '0',
  `yesterdayHonorPoints` int(10) unsigned NOT NULL DEFAULT '0',
  `totalKills` int(10) unsigned NOT NULL DEFAULT '0',
  `todayKills` smallint(5) unsigned NOT NULL DEFAULT '0',
  `yesterdayKills` smallint(5) unsigned NOT NULL DEFAULT '0',
  `chosenTitle` int(10) unsigned NOT NULL DEFAULT '0',
  `knownCurrencies` bigint(20) unsigned NOT NULL DEFAULT '0',
  `watchedFaction` int(10) unsigned NOT NULL DEFAULT '0',
  `drunk` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `health` int(10) unsigned NOT NULL DEFAULT '0',
  `power1` int(10) unsigned NOT NULL DEFAULT '0',
  `power2` int(10) unsigned NOT NULL DEFAULT '0',
  `power3` int(10) unsigned NOT NULL DEFAULT '0',
  `power4` int(10) unsigned NOT NULL DEFAULT '0',
  `power5` int(10) unsigned NOT NULL DEFAULT '0',
  `power6` int(10) unsigned NOT NULL DEFAULT '0',
  `power7` int(10) unsigned NOT NULL DEFAULT '0',
  `specCount` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `activeSpec` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `exploredZones` longtext,
  `equipmentCache` longtext,
  `ammoId` int(10) unsigned NOT NULL DEFAULT '0',
  `knownTitles` longtext,
  `actionBars` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `grantableLevels` INT UNSIGNED DEFAULT '0',
  `deleteInfos_Account` int(11) unsigned DEFAULT NULL,
  `deleteInfos_Name` varchar(12) DEFAULT NULL,
  `deleteDate` bigint(20) unsigned DEFAULT NULL,
  PRIMARY KEY (`guid`),
  KEY `idx_account` (`account`),
  KEY `idx_online` (`online`),
  KEY `idx_name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

--
-- Dumping data for table `characters`
--

LOCK TABLES `characters` WRITE;
/*!40000 ALTER TABLE `characters` DISABLE KEYS */;
/*!40000 ALTER TABLE `characters` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `corpse`
--

DROP TABLE IF EXISTS `corpse`;
CREATE TABLE `corpse` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `player` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Character Global Unique Identifier',
  `position_x` float NOT NULL DEFAULT '0',
  `position_y` float NOT NULL DEFAULT '0',
  `position_z` float NOT NULL DEFAULT '0',
  `orientation` float NOT NULL DEFAULT '0',
  `map` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Map Identifier',
  `phaseMask` smallint(5) unsigned NOT NULL DEFAULT '1',
  `time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `corpse_type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `instance` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  KEY `idx_type` (`corpse_type`),
  KEY `instance` (`instance`),
  KEY `Idx_player` (`player`),
  KEY `Idx_time` (`time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Death System';

--
-- Dumping data for table `corpse`
--

LOCK TABLES `corpse` WRITE;
/*!40000 ALTER TABLE `corpse` DISABLE KEYS */;
/*!40000 ALTER TABLE `corpse` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `creature_respawn`
--

DROP TABLE IF EXISTS `creature_respawn`;
CREATE TABLE `creature_respawn` (
  `guid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `respawntime` bigint(20) unsigned NOT NULL DEFAULT '0',
  `instance` mediumint(8) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`instance`),
  KEY `instance` (`instance`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Grid Loading System';

--
-- Dumping data for table `creature_respawn`
--

LOCK TABLES `creature_respawn` WRITE;
/*!40000 ALTER TABLE `creature_respawn` DISABLE KEYS */;
/*!40000 ALTER TABLE `creature_respawn` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `event_group_chosen`
--

DROP TABLE IF EXISTS `event_group_chosen`;
CREATE TABLE `event_group_chosen` (
`eventGroup` mediumint(8) unsigned NOT NULL DEFAULT '0',
`entry` mediumint(8) unsigned NOT NULL DEFAULT '0',
PRIMARY KEY (`eventGroup`,`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Quest Group picked';

--
-- Dumping data for table `event_group_chosen`
--

LOCK TABLES `event_group_chosen` WRITE;
/*!40000 ALTER TABLE `event_group_chosen` DISABLE KEYS */;
/*!40000 ALTER TABLE `event_group_chosen` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_event_status`
--

DROP TABLE IF EXISTS `game_event_status`;
CREATE TABLE `game_event_status` (
  `event` smallint(6) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`event`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Game event system';

--
-- Dumping data for table `game_event_status`
--

LOCK TABLES `game_event_status` WRITE;
/*!40000 ALTER TABLE `game_event_status` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_event_status` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `gm_tickets`
--

DROP TABLE IF EXISTS `gm_tickets`;
CREATE TABLE `gm_tickets` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT 'GM Ticket\'s unique identifier',
  `category` tinyint(2) unsigned NOT NULL DEFAULT '0' COMMENT 'GM Ticket Category DBC entry\'s identifier',
  `state` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT 'Ticket\'s current state',
  `status` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT 'Ticket\'s current status',
  `level` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT 'Ticket\'s security level',
  `author_guid` int(11) unsigned NOT NULL COMMENT 'Player\'s character Global Unique Identifier',
  `author_name` varchar(12) NOT NULL COMMENT 'Player\'s character name',
  `locale` varchar(4) NOT NULL DEFAULT 'enUS' COMMENT 'Player\'s client locale name',
  `mapid` int(4) unsigned NOT NULL DEFAULT '0' COMMENT 'Character\'s map identifier on submission',
  `x` float NOT NULL DEFAULT '0' COMMENT 'Character\'s x coordinate on submission',
  `y` float NOT NULL DEFAULT '0' COMMENT 'Character\'s y coordinate on submission',
  `z` float NOT NULL DEFAULT '0' COMMENT 'Character\'s z coordinate on submission',
  `o` float NOT NULL DEFAULT '0' COMMENT 'Character\'s orientation angle on submission',
  `text` text NOT NULL COMMENT 'Ticket\'s message',
  `created` bigint(40) unsigned NOT NULL COMMENT 'Timestamp: ticket created by a player',
  `updated` bigint(40) unsigned NOT NULL DEFAULT 0 COMMENT 'Timestamp: ticket text\'s last update',
  `seen` bigint(40) unsigned NOT NULL DEFAULT 0 COMMENT 'Timestamp: ticket\'s last time opened by a GM',
  `answered` bigint(40) unsigned NOT NULL DEFAULT 0 COMMENT 'Timestamp: ticket\'s last time answered by a GM',
  `closed` bigint(40) unsigned NOT NULL DEFAULT 0 COMMENT 'Timestamp: ticket closed by a GM',
  `assignee_guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Assignee\'s character\'s Global Unique Identifier',
  `assignee_name` varchar(12) NOT NULL DEFAULT '' COMMENT 'Assignee\'s character\'s name',
  `conclusion` varchar(255) NOT NULL DEFAULT '' COMMENT 'Assignee\'s final conclusion on this ticket',
  `notes` varchar(10000) NOT NULL DEFAULT '' COMMENT 'Additional notes for GMs',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='GM Tickets System';

--
-- Dumping data for table `gm_tickets`
--

LOCK TABLES `gm_tickets` WRITE;
/*!40000 ALTER TABLE `gm_tickets` DISABLE KEYS */;
/*!40000 ALTER TABLE `gm_tickets` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `gm_surveys`
--

DROP TABLE IF EXISTS `gm_surveys`;
CREATE TABLE `gm_surveys` (
  `ticketid` int(11) unsigned NOT NULL COMMENT 'GM Ticket\'s unique identifier',
  `surveyid` int(2) unsigned NOT NULL COMMENT 'Survey DBC entry\'s identifier',
  `answer1` tinyint(2) unsigned,
  `answer2` tinyint(2) unsigned,
  `answer3` tinyint(2) unsigned,
  `answer4` tinyint(2) unsigned,
  `answer5` tinyint(2) unsigned,
  `answer6` tinyint(2) unsigned,
  `answer7` tinyint(2) unsigned,
  `answer8` tinyint(2) unsigned,
  `answer9` tinyint(2) unsigned,
  `answer10` tinyint(2) unsigned,
  `comment` text COMMENT 'Player\'s feedback',
  PRIMARY KEY (`ticketid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='GM Tickets System';

--
-- Dumping data for table `gm_surveys`
--

LOCK TABLES `gm_surveys` WRITE;
/*!40000 ALTER TABLE `gm_surveys` DISABLE KEYS */;
/*!40000 ALTER TABLE `gm_surveys` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `gameobject_respawn`
--

DROP TABLE IF EXISTS `gameobject_respawn`;
CREATE TABLE `gameobject_respawn` (
  `guid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `respawntime` bigint(20) unsigned NOT NULL DEFAULT '0',
  `instance` mediumint(8) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`instance`),
  KEY `instance` (`instance`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Grid Loading System';

--
-- Dumping data for table `gameobject_respawn`
--

LOCK TABLES `gameobject_respawn` WRITE;
/*!40000 ALTER TABLE `gameobject_respawn` DISABLE KEYS */;
/*!40000 ALTER TABLE `gameobject_respawn` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `group_instance`
--

DROP TABLE IF EXISTS `group_instance`;
CREATE TABLE `group_instance` (
  `leaderGuid` int(11) unsigned NOT NULL DEFAULT '0',
  `instance` int(11) unsigned NOT NULL DEFAULT '0',
  `permanent` tinyint(1) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`leaderGuid`,`instance`),
  KEY `instance` (`instance`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `group_instance`
--

LOCK TABLES `group_instance` WRITE;
/*!40000 ALTER TABLE `group_instance` DISABLE KEYS */;
/*!40000 ALTER TABLE `group_instance` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `group_member`
--

DROP TABLE IF EXISTS `group_member`;
CREATE TABLE `group_member` (
  `groupId` int(11) unsigned NOT NULL,
  `memberGuid` int(11) unsigned NOT NULL,
  `assistant` tinyint(1) unsigned NOT NULL,
  `subgroup` smallint(6) unsigned NOT NULL,
  PRIMARY KEY (`groupId`,`memberGuid`),
  KEY `Idx_memberGuid` (`memberGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Groups';

--
-- Dumping data for table `group_member`
--

LOCK TABLES `group_member` WRITE;
/*!40000 ALTER TABLE `group_member` DISABLE KEYS */;
/*!40000 ALTER TABLE `group_member` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `groups`
--

DROP TABLE IF EXISTS `groups`;
CREATE TABLE `groups` (
  `groupId` int(11) unsigned NOT NULL,
  `leaderGuid` int(11) unsigned NOT NULL,
  `mainTank` int(11) unsigned NOT NULL,
  `mainAssistant` int(11) unsigned NOT NULL,
  `lootMethod` tinyint(4) unsigned NOT NULL,
  `looterGuid` int(11) unsigned NOT NULL,
  `lootThreshold` tinyint(4) unsigned NOT NULL,
  `icon1` int(11) unsigned NOT NULL,
  `icon2` int(11) unsigned NOT NULL,
  `icon3` int(11) unsigned NOT NULL,
  `icon4` int(11) unsigned NOT NULL,
  `icon5` int(11) unsigned NOT NULL,
  `icon6` int(11) unsigned NOT NULL,
  `icon7` int(11) unsigned NOT NULL,
  `icon8` int(11) unsigned NOT NULL,
  `groupType` tinyint(1) unsigned NOT NULL,
  `difficulty` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `raiddifficulty` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`groupId`),
  UNIQUE KEY `leaderGuid` (`leaderGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Groups';

--
-- Dumping data for table `groups`
--

LOCK TABLES `groups` WRITE;
/*!40000 ALTER TABLE `groups` DISABLE KEYS */;
/*!40000 ALTER TABLE `groups` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `guild`
--

DROP TABLE IF EXISTS `guild`;
CREATE TABLE `guild` (
  `guildid` int(6) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL DEFAULT '',
  `leaderguid` int(6) unsigned NOT NULL DEFAULT '0',
  `EmblemStyle` int(5) NOT NULL DEFAULT '0',
  `EmblemColor` int(5) NOT NULL DEFAULT '0',
  `BorderStyle` int(5) NOT NULL DEFAULT '0',
  `BorderColor` int(5) NOT NULL DEFAULT '0',
  `BackgroundColor` int(5) NOT NULL DEFAULT '0',
  `info` varchar(500) NOT NULL DEFAULT '',
  `motd` varchar(128) NOT NULL DEFAULT '',
  `createdate` bigint(20) unsigned NOT NULL DEFAULT '0',
  `BankMoney` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guildid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Guild System';

--
-- Dumping data for table `guild`
--

LOCK TABLES `guild` WRITE;
/*!40000 ALTER TABLE `guild` DISABLE KEYS */;
/*!40000 ALTER TABLE `guild` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `guild_bank_eventlog`
--

DROP TABLE IF EXISTS `guild_bank_eventlog`;
CREATE TABLE `guild_bank_eventlog` (
  `guildid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Guild Identificator',
  `LogGuid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Log record identificator - auxiliary column',
  `TabId` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'Guild bank TabId',
  `EventType` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'Event type',
  `PlayerGuid` int(11) unsigned NOT NULL DEFAULT '0',
  `ItemOrMoney` int(11) unsigned NOT NULL DEFAULT '0',
  `ItemStackCount` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `DestTabId` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT 'Destination Tab Id',
  `TimeStamp` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'Event UNIX time',
  PRIMARY KEY (`guildid`,`LogGuid`,`TabId`),
  KEY `Idx_PlayerGuid` (`PlayerGuid`),
  KEY `Idx_LogGuid` (`LogGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `guild_bank_eventlog`
--

LOCK TABLES `guild_bank_eventlog` WRITE;
/*!40000 ALTER TABLE `guild_bank_eventlog` DISABLE KEYS */;
/*!40000 ALTER TABLE `guild_bank_eventlog` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `guild_bank_item`
--

DROP TABLE IF EXISTS `guild_bank_item`;
CREATE TABLE `guild_bank_item` (
  `guildid` int(11) unsigned NOT NULL DEFAULT '0',
  `TabId` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `SlotId` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `item_guid` int(11) unsigned NOT NULL DEFAULT '0',
  `item_entry` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guildid`,`TabId`,`SlotId`),
  KEY `Idx_item_guid` (`item_guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `guild_bank_item`
--

LOCK TABLES `guild_bank_item` WRITE;
/*!40000 ALTER TABLE `guild_bank_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `guild_bank_item` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `guild_bank_right`
--

DROP TABLE IF EXISTS `guild_bank_right`;
CREATE TABLE `guild_bank_right` (
  `guildid` int(11) unsigned NOT NULL DEFAULT '0',
  `TabId` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `rid` int(11) unsigned NOT NULL DEFAULT '0',
  `gbright` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `SlotPerDay` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guildid`,`TabId`,`rid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `guild_bank_right`
--

LOCK TABLES `guild_bank_right` WRITE;
/*!40000 ALTER TABLE `guild_bank_right` DISABLE KEYS */;
/*!40000 ALTER TABLE `guild_bank_right` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `guild_bank_tab`
--

DROP TABLE IF EXISTS `guild_bank_tab`;
CREATE TABLE `guild_bank_tab` (
  `guildid` int(11) unsigned NOT NULL DEFAULT '0',
  `TabId` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `TabName` varchar(100) NOT NULL DEFAULT '',
  `TabIcon` varchar(100) NOT NULL DEFAULT '',
  `TabText` varchar(500) DEFAULT NULL,
  PRIMARY KEY (`guildid`,`TabId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `guild_bank_tab`
--

LOCK TABLES `guild_bank_tab` WRITE;
/*!40000 ALTER TABLE `guild_bank_tab` DISABLE KEYS */;
/*!40000 ALTER TABLE `guild_bank_tab` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `guild_eventlog`
--

DROP TABLE IF EXISTS `guild_eventlog`;
CREATE TABLE `guild_eventlog` (
  `guildid` int(11) unsigned NOT NULL COMMENT 'Guild Identificator',
  `LogGuid` int(11) unsigned NOT NULL COMMENT 'Log record identificator - auxiliary column',
  `EventType` tinyint(1) unsigned NOT NULL COMMENT 'Event type',
  `PlayerGuid1` int(11) unsigned NOT NULL COMMENT 'Player 1',
  `PlayerGuid2` int(11) unsigned NOT NULL COMMENT 'Player 2',
  `NewRank` tinyint(2) unsigned NOT NULL COMMENT 'New rank(in case promotion/demotion)',
  `TimeStamp` bigint(20) unsigned NOT NULL COMMENT 'Event UNIX time',
  PRIMARY KEY (`guildid`,`LogGuid`),
  KEY `Idx_PlayerGuid1` (`PlayerGuid1`),
  KEY `Idx_PlayerGuid2` (`PlayerGuid2`),
  KEY `Idx_LogGuid` (`LogGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Guild Eventlog';

--
-- Dumping data for table `guild_eventlog`
--

LOCK TABLES `guild_eventlog` WRITE;
/*!40000 ALTER TABLE `guild_eventlog` DISABLE KEYS */;
/*!40000 ALTER TABLE `guild_eventlog` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `guild_member`
--

DROP TABLE IF EXISTS `guild_member`;
CREATE TABLE `guild_member` (
  `guildid` int(6) unsigned NOT NULL DEFAULT '0',
  `guid` int(11) unsigned NOT NULL DEFAULT '0',
  `rank` tinyint(2) unsigned NOT NULL DEFAULT '0',
  `pnote` varchar(31) NOT NULL DEFAULT '',
  `offnote` varchar(31) NOT NULL DEFAULT '',
  `BankResetTimeMoney` int(11) unsigned NOT NULL DEFAULT '0',
  `BankRemMoney` int(11) unsigned NOT NULL DEFAULT '0',
  `BankResetTimeTab0` int(11) unsigned NOT NULL DEFAULT '0',
  `BankRemSlotsTab0` int(11) unsigned NOT NULL DEFAULT '0',
  `BankResetTimeTab1` int(11) unsigned NOT NULL DEFAULT '0',
  `BankRemSlotsTab1` int(11) unsigned NOT NULL DEFAULT '0',
  `BankResetTimeTab2` int(11) unsigned NOT NULL DEFAULT '0',
  `BankRemSlotsTab2` int(11) unsigned NOT NULL DEFAULT '0',
  `BankResetTimeTab3` int(11) unsigned NOT NULL DEFAULT '0',
  `BankRemSlotsTab3` int(11) unsigned NOT NULL DEFAULT '0',
  `BankResetTimeTab4` int(11) unsigned NOT NULL DEFAULT '0',
  `BankRemSlotsTab4` int(11) unsigned NOT NULL DEFAULT '0',
  `BankResetTimeTab5` int(11) unsigned NOT NULL DEFAULT '0',
  `BankRemSlotsTab5` int(11) unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `guid_key` (`guid`),
  KEY `guildid_rank_key` (`guildid`,`rank`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Guild System';

--
-- Dumping data for table `guild_member`
--

LOCK TABLES `guild_member` WRITE;
/*!40000 ALTER TABLE `guild_member` DISABLE KEYS */;
/*!40000 ALTER TABLE `guild_member` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `guild_rank`
--

DROP TABLE IF EXISTS `guild_rank`;
CREATE TABLE `guild_rank` (
  `guildid` int(6) unsigned NOT NULL DEFAULT '0',
  `rid` int(11) unsigned NOT NULL,
  `rname` varchar(255) NOT NULL DEFAULT '',
  `rights` int(3) unsigned NOT NULL DEFAULT '0',
  `BankMoneyPerDay` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guildid`,`rid`),
  KEY `Idx_rid` (`rid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Guild System';

--
-- Dumping data for table `guild_rank`
--

LOCK TABLES `guild_rank` WRITE;
/*!40000 ALTER TABLE `guild_rank` DISABLE KEYS */;
/*!40000 ALTER TABLE `guild_rank` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `instance`
--

DROP TABLE IF EXISTS `instance`;
CREATE TABLE `instance` (
  `id` int(11) unsigned NOT NULL DEFAULT '0',
  `map` int(11) unsigned NOT NULL DEFAULT '0',
  `resettime` bigint(40) unsigned NOT NULL DEFAULT '0',
  `difficulty` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `encountersMask` int(10) unsigned NOT NULL DEFAULT '0',
  `data` longtext,
  PRIMARY KEY (`id`),
  KEY `map` (`map`),
  KEY `resettime` (`resettime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `instance`
--

LOCK TABLES `instance` WRITE;
/*!40000 ALTER TABLE `instance` DISABLE KEYS */;
/*!40000 ALTER TABLE `instance` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `instance_reset`
--

DROP TABLE IF EXISTS `instance_reset`;
CREATE TABLE `instance_reset` (
  `mapid` int(11) unsigned NOT NULL DEFAULT '0',
  `difficulty` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `resettime` bigint(40) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`mapid`,`difficulty`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `instance_reset`
--

LOCK TABLES `instance_reset` WRITE;
/*!40000 ALTER TABLE `instance_reset` DISABLE KEYS */;
/*!40000 ALTER TABLE `instance_reset` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `item_instance`
--

DROP TABLE IF EXISTS `item_instance`;
CREATE TABLE `item_instance` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0',
  `owner_guid` int(11) unsigned NOT NULL DEFAULT '0',
  `itemEntry` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `creatorGuid` int(10) unsigned NOT NULL default '0',
  `giftCreatorGuid` int(10) unsigned NOT NULL default '0',
  `count` int(10) unsigned NOT NULL default '1',
  `duration` int(10) unsigned NOT NULL default '0',
  `charges` text NOT NULL,
  `flags` int(8) unsigned NOT NULL default '0',
  `enchantments` text NOT NULL,
  `randomPropertyId` smallint(5) NOT NULL default '0',
  `durability` int(5) unsigned NOT NULL default '0',
  `playedTime` int(10) unsigned NOT NULL default '0',
  `text` longtext,
  PRIMARY KEY (`guid`),
  KEY `idx_owner_guid` (`owner_guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Item System';

--
-- Dumping data for table `item_instance`
--

LOCK TABLES `item_instance` WRITE;
/*!40000 ALTER TABLE `item_instance` DISABLE KEYS */;
/*!40000 ALTER TABLE `item_instance` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `item_loot`
--

DROP TABLE IF EXISTS `item_loot`;
CREATE TABLE `item_loot` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0',
  `owner_guid` int(11) unsigned NOT NULL DEFAULT '0',
  `itemid` int(11) unsigned NOT NULL DEFAULT '0',
  `amount` int(11) unsigned NOT NULL DEFAULT '0',
  `suffix` int(11) unsigned NOT NULL DEFAULT '0',
  `property` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`itemid`),
  KEY `idx_owner_guid` (`owner_guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Item System';

--
-- Dumping data for table `item_loot`
--

LOCK TABLES `item_loot` WRITE;
/*!40000 ALTER TABLE `item_loot` DISABLE KEYS */;
/*!40000 ALTER TABLE `item_loot` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mail`
--

DROP TABLE IF EXISTS `mail`;
CREATE TABLE `mail` (
  `id` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Identifier',
  `messageType` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `stationery` tinyint(3) NOT NULL DEFAULT '41',
  `mailTemplateId` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `sender` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Character Global Unique Identifier',
  `receiver` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Character Global Unique Identifier',
  `subject` longtext,
  `body` longtext,
  `has_items` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `expire_time` bigint(40) unsigned NOT NULL DEFAULT '0',
  `deliver_time` bigint(40) unsigned NOT NULL DEFAULT '0',
  `money` int(11) unsigned NOT NULL DEFAULT '0',
  `cod` int(11) unsigned NOT NULL DEFAULT '0',
  `checked` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `idx_receiver` (`receiver`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Mail System';

--
-- Dumping data for table `mail`
--

LOCK TABLES `mail` WRITE;
/*!40000 ALTER TABLE `mail` DISABLE KEYS */;
/*!40000 ALTER TABLE `mail` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mail_items`
--

DROP TABLE IF EXISTS `mail_items`;
CREATE TABLE `mail_items` (
  `mail_id` int(11) NOT NULL DEFAULT '0',
  `item_guid` int(11) NOT NULL DEFAULT '0',
  `item_template` int(11) NOT NULL DEFAULT '0',
  `receiver` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Character Global Unique Identifier',
  PRIMARY KEY (`mail_id`,`item_guid`),
  KEY `idx_receiver` (`receiver`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

--
-- Dumping data for table `mail_items`
--

LOCK TABLES `mail_items` WRITE;
/*!40000 ALTER TABLE `mail_items` DISABLE KEYS */;
/*!40000 ALTER TABLE `mail_items` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pet_aura`
--

DROP TABLE IF EXISTS `pet_aura`;
CREATE TABLE `pet_aura` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `caster_guid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'Full Global Unique Identifier',
  `item_guid` int(11) unsigned NOT NULL DEFAULT '0',
  `spell` int(11) unsigned NOT NULL DEFAULT '0',
  `stackcount` int(11) unsigned NOT NULL DEFAULT '1',
  `remaincharges` int(11) unsigned NOT NULL DEFAULT '0',
  `basepoints0` int(11) NOT NULL DEFAULT '0',
  `basepoints1` int(11) NOT NULL DEFAULT '0',
  `basepoints2` int(11) NOT NULL DEFAULT '0',
  `periodictime0` int(11) unsigned NOT NULL DEFAULT '0',
  `periodictime1` int(11) unsigned NOT NULL DEFAULT '0',
  `periodictime2` int(11) unsigned NOT NULL DEFAULT '0',
  `maxduration` int(11) NOT NULL DEFAULT '0',
  `remaintime` int(11) NOT NULL DEFAULT '0',
  `effIndexMask` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`caster_guid`,`item_guid`,`spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Pet System';

--
-- Dumping data for table `pet_aura`
--

LOCK TABLES `pet_aura` WRITE;
/*!40000 ALTER TABLE `pet_aura` DISABLE KEYS */;
/*!40000 ALTER TABLE `pet_aura` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pet_spell`
--

DROP TABLE IF EXISTS `pet_spell`;
CREATE TABLE `pet_spell` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `spell` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Spell Identifier',
  `active` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Pet System';

--
-- Dumping data for table `pet_spell`
--

LOCK TABLES `pet_spell` WRITE;
/*!40000 ALTER TABLE `pet_spell` DISABLE KEYS */;
/*!40000 ALTER TABLE `pet_spell` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pet_spell_cooldown`
--

DROP TABLE IF EXISTS `pet_spell_cooldown`;
CREATE TABLE `pet_spell_cooldown` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier, Low part',
  `spell` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Spell Identifier',
  `time` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `pet_spell_cooldown`
--

LOCK TABLES `pet_spell_cooldown` WRITE;
/*!40000 ALTER TABLE `pet_spell_cooldown` DISABLE KEYS */;
/*!40000 ALTER TABLE `pet_spell_cooldown` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `petition`
--

DROP TABLE IF EXISTS `petition`;
CREATE TABLE `petition` (
  `ownerguid` int(10) unsigned NOT NULL,
  `petitionguid` int(10) unsigned DEFAULT '0',
  `name` varchar(255) NOT NULL DEFAULT '',
  `type` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ownerguid`,`type`),
  UNIQUE KEY `index_ownerguid_petitionguid` (`ownerguid`,`petitionguid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Guild System';

--
-- Dumping data for table `petition`
--

LOCK TABLES `petition` WRITE;
/*!40000 ALTER TABLE `petition` DISABLE KEYS */;
/*!40000 ALTER TABLE `petition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `petition_sign`
--

DROP TABLE IF EXISTS `petition_sign`;
CREATE TABLE `petition_sign` (
  `ownerguid` int(10) unsigned NOT NULL,
  `petitionguid` int(11) unsigned NOT NULL DEFAULT '0',
  `playerguid` int(11) unsigned NOT NULL DEFAULT '0',
  `player_account` int(11) unsigned NOT NULL DEFAULT '0',
  `type` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`petitionguid`,`playerguid`),
  KEY `Idx_playerguid` (`playerguid`),
  KEY `Idx_ownerguid` (`ownerguid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Guild System';

--
-- Dumping data for table `petition_sign`
--

LOCK TABLES `petition_sign` WRITE;
/*!40000 ALTER TABLE `petition_sign` DISABLE KEYS */;
/*!40000 ALTER TABLE `petition_sign` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `playerbot_talentspec`
--

DROP TABLE IF EXISTS `playerbot_talentspec`;
CREATE TABLE `playerbot_talentspec` (
  `talentspec_id` mediumint(8) NOT NULL AUTO_INCREMENT UNIQUE,
  `name` varchar(255),
  `class` tinyint(3) NOT NULL,
  `purpose` int(11) unsigned NOT NULL,
  `talent_10` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_11` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_12` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_13` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_14` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_15` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_16` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_17` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_18` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_19` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_20` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_21` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_22` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_23` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_24` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_25` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_26` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_27` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_28` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_29` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_30` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_31` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_32` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_33` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_34` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_35` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_36` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_37` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_38` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_39` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_40` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_41` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_42` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_43` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_44` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_45` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_46` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_47` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_48` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_49` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_50` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_51` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_52` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_53` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_54` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_55` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_56` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_57` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_58` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_59` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_60` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_61` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_62` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_63` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_64` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_65` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_66` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_67` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_68` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_69` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_70` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_71` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_72` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_73` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_74` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_75` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_76` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_77` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_78` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_79` smallint(5) unsigned NOT NULL DEFAULT '0',
  `talent_80` smallint(5) unsigned NOT NULL DEFAULT '0',
  `major_glyph_15` smallint(5) unsigned NOT NULL DEFAULT '0',
  `major_glyph_30` smallint(5) unsigned NOT NULL DEFAULT '0',
  `major_glyph_80` smallint(5) unsigned NOT NULL DEFAULT '0',
  `minor_glyph_15` smallint(5) unsigned NOT NULL DEFAULT '0',
  `minor_glyph_50` smallint(5) unsigned NOT NULL DEFAULT '0',
  `minor_glyph_70` smallint(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`talentspec_id`,`class`,`purpose`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='PlayerbotAI TalentSpec system';

--
-- Dumping data for table `playerbot_talentspec`
--

--
-- PLAYERBOTAI TALENT / GLYPH SPECS
--
--    Invalid builds will simply be ignored. If yours isn't showing up, this is (most likely) why. Try "talent errorcheck" with a GM account.
--    talentspec_id: AUTO_INCREMENT -> always NULL   (SOFT permanence, values may be different next DB update, do not use for any permanent settings)
--    Build Name: Any string of text NOT containing quote "'". Also recommend against semicolon ';' double quote '"' or new lines.
--    Class Id: See "SharedDefines.h" enum Classes for characters and "PlayerbotAI.h" enum ClassesCombatPets for pets.
--    Purpose(s): See "PlayerbotAI.h" enum TalentSpecPurpose. Please combine hex manually. E.g. 0x00000008 & 0x00000004 & 0x00000020 = 0x0000002C
--    Talent_N Id: The talent IDs to be used, in order. None (for e.g. twink specs) uses value '0' (smallint so no quotes).
--    MajorGlyph1,2,3,MinorGlyph1,2,3 Id: Glyph IDs to be used (see levels above). None uses value '0' (smallints, so no quotes).
--

-- When editing manually remember LAST LINE is ended by semicolon ';' not comma ','
LOCK TABLES `playerbot_talentspec` WRITE;
INSERT INTO `playerbot_talentspec` VALUES
(NULL,'Death Knight Fast Leveling (54/17/0)',6,0x00000006,1939,1939,1945,1945,1945,2217,2217,1948,1948,1948,1941,2086,2086,2086,1943,1942,1942,1942,1953,1953,1953,1943,1943,1943,1943,1944,1944,1944,2015,2015,1960,1960,1960,1936,1936,1955,1955,1955,2259,2259,1957,1958,1958,1958,2015,2034,2034,2034,2034,2034,1950,1950,1950,2031,2031,2031,2020,2020,1973,1973,1973,1973,1973,2042,2042,2042,2042,2042,1971,1971,1938,520,558,524,522,553,555),
(NULL,'Death Knight PvP (50/3/18)',6,0x00260000,1939,1939,1945,1945,1945,1938,1938,1938,1938,1938,2217,2217,2086,2086,2086,1943,1943,1943,1943,1943,1950,1950,1950,2015,2015,1944,1944,1944,2015,1941,1954,1960,1960,1960,1936,1955,1955,1955,2259,2259,1957,1958,1958,1958,1936,2034,2034,2034,2034,2034,2031,2031,2031,2082,2082,1932,1932,1932,1934,1934,1934,1963,1963,1933,1933,1933,2008,2008,2004,2004,2004,513,558,771,522,514,553),
(NULL,'Death Knight Blood Tanking (53/8/10)',6,0x00000015,1968,1968,1968,1968,1968,2031,2031,2031,2218,2218,2218,2218,2218,1933,1933,1933,1963,1963,2017,2017,2017,2017,2017,1938,1938,1938,1938,1938,1943,1943,1943,1943,1943,2086,2086,2086,1941,2217,2217,1942,1942,1942,1950,1950,1950,1944,1944,1944,1936,1936,2105,2105,2015,2015,2015,2019,2259,2259,1957,1958,1958,1958,1959,1959,1959,2034,2034,2034,2034,2034,1960,772,558,532,522,555,553),
(NULL,'Death Knight Unholy Raid DPS (0/17/54)',6,0x00000031,1932,1932,1932,2082,2082,1934,1934,1934,1963,1963,2047,2047,2047,2047,2047,2225,2225,2004,2004,2004,2005,2005,2005,2005,2005,1984,2008,2008,2008,2011,2011,1996,2285,2285,2285,2285,2285,1962,1962,1962,2007,2216,2043,2043,2043,2036,2036,2036,2036,2036,2000,2003,2003,2003,2031,2031,2031,2020,2020,1973,1973,1973,1973,1973,2042,2042,2042,2042,2042,1971,1971,771,524,527,514,555,553),
(NULL,'Druid Feral Leveling (0/55/16)',11,0x00000006,796,796,796,796,796,805,805,799,799,799,798,798,798,807,807,803,803,803,1914,1914,801,801,802,802,804,808,808,808,808,808,809,1798,1798,1794,1794,1795,1795,1795,1794,1792,1796,1921,1921,1921,1920,1918,1918,1918,1918,1918,1927,1920,1920,2266,821,821,822,822,822,824,824,824,824,824,827,826,826,826,1915,1915,1792,166,165,674,431,551,435),
(NULL,'Druid Feral PVP (0/60/11)',11,0x00020000,796,796,796,796,796,805,805,794,794,794,798,798,798,807,807,803,803,803,1914,1914,801,801,1792,1792,804,808,808,808,808,808,809,1798,1798,1794,1794,1795,1795,1795,1794,1919,1796,1919,1919,1920,1920,1918,1918,1918,1918,1918,1927,1920,2266,1793,1793,1793,1921,1921,1921,799,821,821,822,822,822,824,824,824,824,824,827,166,165,674,431,551,435),
(NULL,'Druid Balance Leveling (58/0/13)',11,0x00000006,762,762,762,762,762,783,783,783,1822,1822,789,789,789,2240,764,792,792,792,792,792,788,2239,2239,2239,1782,790,790,790,1784,1784,793,1912,1912,1912,1785,1913,1913,1786,1786,1786,1923,1787,1924,1924,1924,1928,1928,1928,1785,1785,1926,1786,1786,784,784,784,1782,1782,821,821,822,822,822,826,826,826,822,822,827,1915,1915,176,175,180,431,433,435),
(NULL,'Druid Balance PVP (58/0/13)',11,0x00020000,762,762,762,762,762,1822,1822,763,763,783,2240,764,764,789,789,792,792,792,792,792,788,2239,2239,2239,1782,790,790,790,784,784,793,1912,1912,1912,784,1913,1913,1913,1786,1786,1923,1787,1924,1924,1924,1928,1928,1928,1925,1925,1926,1786,1786,1786,789,1782,1782,1783,1783,821,821,822,822,822,826,826,826,822,822,827,1915,176,175,180,431,433,435),
(NULL,'Druid Restoration AoE (14/0/57)',11,0x00000038,823,823,823,821,821,841,841,826,826,826,1915,1915,827,829,829,829,830,830,830,843,843,843,843,843,831,828,828,828,828,828,825,825,825,825,825,1789,1789,1789,1789,1789,1791,1930,1930,1930,844,1797,1797,1797,1916,1916,1916,1916,1916,1917,1922,1922,1922,2238,2238,2238,2238,2238,1822,1822,783,783,783,789,789,789,2240,172,168,672,435,434,433),
(NULL,'Druid Restoration Single (14/0/57)',11,0x00000018,821,821,823,823,823,826,826,826,824,824,824,824,1915,1915,827,829,829,829,830,830,830,831,828,828,828,828,828,1788,1788,825,825,844,1797,1797,1797,1789,1789,1789,1789,1789,1791,1930,1930,1930,1922,1922,1922,1916,1916,1916,1916,1916,1917,843,843,843,843,2238,2238,2238,2238,2238,1822,1822,783,783,783,789,789,789,2240,891,168,673,435,434,433),
(NULL,'Druid Feral Tanking (0/58/13)',11,0x00000011,796,796,796,796,796,794,794,794,805,805,798,798,798,807,807,803,803,803,801,801,804,1162,799,799,799,808,808,808,808,808,809,1798,1798,1794,1794,1794,2241,2241,2241,1919,1796,1920,1920,1920,1919,1918,1918,1918,1918,1918,2266,1927,802,802,2242,2242,2242,1919,821,821,822,822,822,824,824,824,824,824,827,822,822,162,811,161,433,435,434),
(NULL,'Druid Balance DPS (56/0/15)',11,0x00000012,762,762,762,762,762,783,783,783,1822,1822,2240,789,789,789,764,764,792,792,792,792,792,788,2239,2239,2239,1783,1783,790,790,790,793,1912,1912,1912,1785,1785,1785,1786,1786,1786,1786,1786,1787,1924,1924,1924,1928,1928,1928,1782,1926,1782,1782,784,784,784,822,822,822,822,822,821,821,826,826,826,1915,1915,827,829,829,176,175,180,431,433,435),
(NULL,'Hunter Beast Mastery Leveling (53/18/0)',3,0x00000016,1382,1382,1382,1382,1382,1395,1395,1395,1624,1624,2138,1396,1396,1396,1396,1393,1393,1393,1393,1393,1387,1388,1388,1390,1390,1397,1397,1397,1397,1397,1386,1800,1800,1800,1385,1802,1802,1802,1802,1802,1803,2140,2140,2140,2137,2227,2227,2227,2227,2227,2139,1344,1344,1344,1344,1344,1343,1343,1343,1806,1806,1806,1818,1818,1349,1349,1349,1349,1349,1385,1396,361,354,356,440,439,441),
(NULL,'Hunter PVP (11/53/7)',3,0x00060000,1344,1344,1344,1344,1344,1349,1349,1349,1349,1349,1345,1343,1343,1343,1806,1806,1806,1819,1819,1348,1348,1348,1353,2197,2197,1362,1362,1362,1804,1804,1361,2130,2130,2130,2197,1807,1807,1807,1807,1807,1808,2132,2132,2132,2133,2134,2134,2134,2134,2134,2135,2133,2133,1382,1382,1382,1382,1382,1389,1389,1389,1389,1389,2138,1623,1623,1623,1623,1623,1810,1810,361,351,369,440,439,441),
(NULL,'Hunter Survival DPS (0/15/56)',3,0x00000012,1623,1623,1623,1623,1623,1305,1305,1305,1810,1810,1622,1622,1622,1622,1622,1306,1306,1306,2229,2229,1321,1321,1321,2228,2228,1303,1303,1303,1303,1303,1325,1812,1812,2228,2141,2141,2141,1813,1813,1322,2143,2143,2143,1811,1811,1811,2144,2144,1813,2145,1813,1813,2229,1809,1809,1809,1344,1344,1344,1344,1344,1349,1349,1349,1349,1349,1806,1806,1806,1345,1818,368,366,691,440,439,441),
(NULL,'Mage Frost Leveling (20/0/51)',8,0x00000006,37,37,37,37,37,73,73,73,1649,1649,69,61,61,61,1649,67,67,67,38,38,72,38,741,741,64,68,68,68,1737,1737,71,1738,1738,1738,1738,1740,1740,1853,1853,1738,1741,64,64,62,62,1856,1856,1856,1856,1856,1857,74,74,76,76,76,75,75,75,75,75,81,81,81,1845,1845,2222,2222,2222,2211,1845,331,700,323,445,451,448),
(NULL,'Mage Frost PVP (20/0/51)',8,0x0002000,38,38,38,37,37,65,65,65,73,73,69,61,61,61,73,741,741,67,67,67,72,1736,1736,1736,37,68,68,68,1737,1737,71,1738,1738,1738,1738,1740,1740,1853,1853,1738,1741,2214,2214,37,37,1856,1856,1856,1856,1856,1857,76,76,76,74,74,1650,1650,85,85,85,81,81,81,2211,75,2222,2222,2222,88,88,321,700,323,445,446,447),
(NULL,'Mage Fire Leveling (20/51/0)',8,0x00000006,26,26,26,26,26,34,34,34,34,34,29,23,23,27,27,25,25,25,1639,1639,32,33,33,33,1639,35,35,35,35,35,36,1733,1733,1733,30,1734,1734,1734,30,30,1735,1849,1849,1850,1850,1851,1851,1851,1851,1851,1852,74,74,76,76,76,75,75,75,75,75,81,81,81,1845,1845,2222,2222,2222,1142,1142,324,328,697,445,451,611),
(NULL,'Mage Arcane DPS (57/3/11)',8,0x00000012,76,76,76,74,74,75,75,75,75,75,2211,81,81,81,82,1142,1142,1142,2222,2222,86,77,77,77,77,421,421,421,1725,1725,87,1727,1727,1727,77,1843,1843,1728,1728,1728,2209,2209,2209,2209,2209,1846,1846,1846,1826,1826,1847,1728,1728,2222,1845,1845,1845,62,62,62,38,38,1649,1649,1649,73,73,69,1141,1141,1141,312,328,651,445,451,450),
(NULL,'Paladin Protection Tank (0/53/18)',2,0x00000011,2185,2185,2185,2185,2185,1629,1629,1629,1629,1629,1501,1501,1501,1423,1423,1422,1422,1422,1423,1423,1431,1748,1748,1748,1423,1750,1750,1429,1429,1429,1751,1751,1751,1430,2282,1753,1753,1753,1421,1421,1754,2195,2195,2195,2194,2204,2204,2204,2200,2200,2196,2194,1421,1403,1403,1403,1403,1403,1631,1631,1407,1407,1407,1633,1633,1634,1634,1407,1755,1755,1755,705,702,561,452,455,456),
(NULL,'Paladin Holy SpellPower Focus (51/20/0)',2,0x00000018,1432,1432,1432,1432,1432,1449,1449,1449,1449,1449,1461,1461,1461,1461,1461,1444,1444,1444,1435,1450,1450,1450,1433,1465,1465,1465,1627,1627,1627,1627,1627,1502,1745,1745,1745,1746,1746,1746,1746,1746,2199,2199,2199,2199,2199,2193,2193,2191,2191,1747,2192,1442,1442,1442,1442,1442,1748,1748,1748,1425,1425,2280,1423,1423,1423,1423,1422,1422,1422,2281,2281,196,200,706,452,455,454),
(NULL,'Paladin Holy SpellCrit Focus (51/0/20)',2,0x00000008,1432,1432,1432,1432,1432,1449,1449,1449,1449,1449,1444,1444,1444,1461,1461,1461,1461,1461,1435,1450,1433,1465,1465,1465,1450,1627,1627,1627,1627,1627,1502,1745,1745,1745,1450,1746,1746,1746,1746,1746,2199,2199,2199,2199,2199,2193,2193,2191,2191,1747,2192,1407,1407,1407,1407,1407,1464,1464,1464,1401,1401,1411,1411,1411,1411,1411,1761,1761,1761,1634,1634,196,199,706,452,455,457),
(NULL,'Paladin Fast Leveling (0/10/61)',2,0x00000006,1407,1407,1407,1407,1407,1631,1631,1464,1464,1464,1481,1411,1411,1411,1411,1761,1761,1761,1755,1755,1756,1410,1410,1410,1411,1402,1402,1402,1634,1634,1758,1758,1758,1441,2176,1759,1759,1759,2147,2147,1823,2179,2179,2179,2176,2149,2149,2149,2148,2148,2150,2185,2185,2185,2185,2185,1401,1401,1633,1633,1755,1757,1757,2148,1632,1632,1425,1425,1748,1748,1748,183,703,184,453,455,456),
(NULL,'Paladin PVP Leveling (0/17/54)',2,0x00060000,1407,1407,1407,1407,1407,1631,1631,1464,1464,1464,1481,1411,1411,1411,1411,1761,1761,1761,1634,1634,1756,1410,1410,1410,1411,1402,1402,1402,1757,1757,1758,1758,1758,1441,2176,1759,1759,1759,2147,2147,1823,2179,2179,2179,2176,2149,2149,2149,1401,1401,2150,2185,2185,2185,2185,2185,1632,1632,1425,1425,1748,1748,1748,1501,1501,1501,1423,1423,1521,1521,2148,183,703,198,453,455,452),
(NULL,'Priest Shadow Leveling Build (13/0/58)',5,0x00000006,465,465,465,2027,2027,463,463,463,482,482,501,462,462,462,462,461,461,461,881,881,484,1638,1638,462,481,2267,2267,2267,1781,1781,521,1778,1778,1778,1778,1816,1816,1816,1906,1906,1779,1909,1909,1909,1778,1907,1907,1907,1907,1907,1910,1898,1898,1898,1898,1898,346,346,346,344,344,347,347,347,481,481,481,481,1777,1777,1777,261,262,708,460,459,462),
(NULL,'Priest Shadow PVP Build (13/0/58)',5,0x00020000,465,465,465,2027,2027,482,482,462,462,462,501,542,542,462,462,461,461,461,881,881,484,541,1638,1638,481,2267,2267,2267,1781,1781,521,1778,1778,1778,1778,1816,1816,1816,1906,1906,1779,1908,1909,1909,1909,1907,1907,1907,1907,1907,1910,1778,481,481,481,481,463,463,1898,1898,1898,1898,1898,346,346,346,344,344,347,347,347,261,262,708,460,459,462),
(NULL,'Priest Holy Healing (14/57/0)',5,0x00000018,401,401,401,401,401,1181,1181,1181,1181,1181,361,361,361,406,406,406,408,408,408,442,1561,402,402,402,402,402,1766,1766,404,404,404,404,404,1768,1768,1767,1767,1767,1767,1767,1815,1768,1902,1902,1902,1905,1905,1905,1905,1905,1911,1903,1903,1903,1904,1904,1904,1898,1898,1898,1898,1898,346,346,346,344,344,347,347,347,348,255,251,709,460,463,462),
(NULL,'Priest Discipline Healing (57/14/0)',5,0x00000018,1898,1898,1898,1898,1898,346,346,346,344,344,347,347,347,343,343,343,341,341,341,348,1201,1201,1201,1201,1201,1771,1771,1772,1772,1772,322,1858,1858,1858,351,1896,1896,1896,2235,2235,1774,1895,1895,1895,1901,1202,1202,1202,1202,1202,1897,1901,1894,1894,1773,1773,1773,401,401,401,401,401,1181,1181,1181,1181,1181,361,361,361,442,255,263,710,460,463,462),
(NULL,'Priest Shadow DPS (13/0/58)',5,0x00000012,465,465,465,2027,2027,482,482,463,463,463,501,462,462,462,462,461,461,461,881,881,484,1777,1777,1777,462,2267,2267,2267,1781,1781,521,1778,1778,1778,1778,1906,1906,1816,1816,1816,1779,1909,1909,1909,1778,1907,1907,1907,1907,1907,1910,483,483,481,481,481,481,481,1898,1898,1898,1898,1898,346,346,346,344,344,347,347,347,261,262,269,460,459,463),
(NULL,'Priest Shadow Raiding',5,0x00000022,1898,1898,1898,1898,1898,346,346,346,344,344,347,347,347,348,462,462,462,462,462,466,466,482,482,463,463,463,481,481,481,481,481,501,483,483,881,881,461,461,461,484,1638,1638,1777,1777,1777,1781,1781,2267,2267,2267,521,1778,1778,1778,1778,1778,1906,1906,1816,1816,1816,1779,1909,1909,1909,1907,1907,1907,1907,1907,1910,261,262,269,460,459,463),
(NULL,'Rogue Leveling (13/53/5)',4,0x00000006,201,201,221,221,221,1827,1827,181,181,181,221,221,181,181,204,186,186,186,1122,1122,223,242,242,242,242,242,1706,1706,1703,1703,205,1705,1705,1705,1122,1825,1825,1825,1825,1825,1709,2074,2074,2073,2073,2075,2075,2075,2075,2075,2076,1122,1122,2244,2244,2244,2244,2244,270,270,270,270,270,273,273,273,2068,2068,269,269,269,409,398,715,465,468,466),
(NULL,'Rogue PVP (16/0/55)',4,0x00020000,241,241,241,261,261,244,244,244,262,262,1123,1123,1123,247,247,263,263,245,245,245,1701,1701,265,265,284,1713,1713,1713,1702,1702,381,1722,1722,1722,1702,2077,2077,1712,1712,1712,1714,1712,1712,1702,1702,2080,2080,2080,2080,2080,2081,270,270,270,270,270,277,277,277,2068,2068,382,269,269,269,269,269,1700,1700,2079,2079,392,408,405,468,469,464),
(NULL,'Rogue Assassination (51/15/5)',4,0x00000002,270,270,270,270,270,277,277,277,273,273,273,269,269,269,269,269,268,268,268,268,268,682,682,682,280,283,283,283,283,283,281,274,274,1721,1721,1718,1718,1718,2069,2069,2069,1719,1715,1715,1715,2070,2070,2070,2070,2070,2071,221,221,221,221,221,181,181,181,181,181,182,182,182,182,182,2244,2244,2244,2244,2244,406,733,714,465,469,468),
(NULL,'Shaman Enhancement Leveling/DPS (16/55/0)',7,0x00000016,610,610,610,614,614,613,613,613,613,613,617,611,611,611,607,602,602,602,602,602,616,2083,2083,2083,607,1643,1643,1643,1689,1689,1690,1692,1692,1692,901,2249,2054,2054,2055,2055,1693,1691,1691,1691,1689,2057,2057,2057,2057,2057,2058,607,2055,563,563,563,563,563,1645,1645,1645,561,561,561,565,565,565,565,565,1647,1647,231,228,229,476,552,473),
(NULL,'Shaman Enhancement PVP (17/54/0)',7,0x00020000,610,610,610,614,614,605,605,613,613,613,617,613,613,611,611,602,602,602,602,602,616,2083,2083,2083,611,1643,1643,1643,1689,1689,1690,1692,1692,1692,901,2054,2054,2249,2055,2055,1693,1691,1691,1691,2055,2057,2057,2057,2057,2057,2058,2056,2056,563,563,563,563,563,1640,1640,1640,1645,1645,1645,565,565,565,565,565,574,1689,231,229,215,476,552,473),
(NULL,'Shaman Elemental Leveling/DPS (57/14/0)',7,0x00000016,563,563,563,563,563,561,561,561,564,564,574,565,565,565,565,565,1642,1642,1642,564,562,1682,1682,1682,1641,721,721,721,721,721,573,2052,2052,2052,1641,2049,2049,1686,1686,1686,1687,2051,2051,2051,2262,2252,2252,2252,2252,2252,2053,2262,1685,1685,1685,564,564,614,614,614,614,614,613,613,613,613,613,617,611,611,611,226,219,214,475,476,473),
(NULL,'Shaman Elemental PVP (58/13/0)',7,0x00020000,563,563,563,563,563,1640,1640,1640,561,561,574,565,565,565,565,1642,1642,1642,567,567,562,565,1641,1641,561,721,721,721,721,721,573,2052,2052,2052,575,1686,1686,1686,2262,2262,2050,2050,2050,1687,2051,2252,2252,2252,2252,2252,2053,2051,2051,1685,1685,614,614,614,614,614,605,605,613,613,613,617,611,611,611,613,613,220,219,213,475,552,473),
(NULL,'Shaman Restoration (0/16/55)',7,0x00000018,586,586,586,586,586,593,593,593,593,593,582,583,583,583,581,588,588,588,594,594,591,594,594,594,581,592,592,592,592,592,590,2084,587,587,587,1696,1696,1696,1697,1697,1698,2059,2059,2061,2061,2063,2063,2063,2063,2063,2064,2060,2060,2061,581,614,614,614,614,614,613,613,613,613,613,611,611,611,607,607,607,751,737,212,475,473,470),
(NULL,'Warlock Affliction Leveling (56/0/15)',9,0x00000006,1003,1003,1003,1003,1003,1004,1004,1101,1101,1284,1061,1284,1001,1001,1001,1764,1764,1764,1002,1002,1041,1763,1763,1763,1763,1042,1042,1042,1042,1042,1669,1669,1669,1669,1669,1667,1667,1667,1873,1873,1670,2245,1763,1878,1878,1876,1876,1876,1876,1876,2041,1878,1022,1875,1007,1007,944,944,944,944,944,943,943,943,943,943,967,967,967,967,967,755,760,911,482,477,478),
(NULL,'Warlock Demonology Leveling (16/55/0)',9,0x00000006,1883,1883,1223,1223,1223,1242,1242,1242,1224,1224,1282,1226,1671,1671,1671,1262,1262,1262,1262,1262,1281,1227,1227,1261,1261,1244,1244,1244,1244,1244,1880,1263,1263,1263,1680,1673,1673,1673,1673,1673,1672,1882,1882,1882,1680,1885,1885,1885,1885,1885,1886,1884,1884,1884,1680,1003,1003,1003,1003,1003,1101,1101,1284,1284,1004,1001,1001,1001,1004,1007,1007,275,278,760,478,482,479),
(NULL,'Warlock Affliction PVP (54/17/0)',9,0x00020000,1003,1003,1003,1003,1003,1004,1004,1284,1284,1005,1061,1001,1001,1001,1005,1021,1021,1002,1002,1764,1081,1041,2205,2205,1764,1042,1042,1042,1042,1042,1669,1669,1669,1669,1669,1668,1668,1667,1667,1667,1670,2245,1878,1878,1878,1876,1876,1876,1876,1876,2041,1764,1007,1007,1223,1223,1223,1883,1883,1242,1242,1242,1224,1224,1671,1671,1671,1282,1226,1227,1227,911,282,274,477,478,482),
(NULL,'Warlock Destruction DPS (0/13/58)',9,0x00000012,943,943,943,943,943,941,941,941,982,982,967,967,967,967,967,985,985,964,983,983,981,961,961,961,1817,966,966,966,966,966,968,986,986,986,1817,1677,1677,1677,1677,1677,2045,2045,2045,1888,1888,1890,1890,1890,1890,1890,1891,1888,1817,1678,1678,1678,1889,1889,1222,1222,1222,1223,1223,1242,1242,1242,1883,1883,1671,1671,1671,283,273,272,477,478,482),
(NULL,'Warlock Demonology DPS (0/56/15)',9,0x00000012,1223,1223,1223,1883,1883,1225,1225,1225,1242,1242,1671,1671,1671,1282,1226,1262,1262,1262,1262,1262,1261,1261,1227,1227,1242,1244,1244,1244,1244,1244,1880,1263,1263,1263,1283,1673,1673,1673,1673,1673,1672,1884,1884,1884,1283,1885,1885,1885,1885,1885,1886,2261,2261,1882,1882,1882,943,943,943,943,943,944,944,944,944,944,967,967,967,967,967,283,278,272,477,478,482),
(NULL,'Warrior Fury Leveling (18/53/0)',1,0x00000006,157,157,157,157,157,2250,2250,2250,159,159,661,661,661,166,166,1581,1581,1581,1581,1581,165,1657,1657,1657,155,156,156,156,156,156,167,1655,1655,1864,1864,1658,1658,1658,1658,1658,1866,1866,1866,1864,155,2234,2234,2234,2234,2234,1867,155,155,124,124,124,130,130,130,130,130,641,641,662,662,121,121,121,136,136,136,496,490,509,483,488,484),
(NULL,'Warrior PVP Leveling Sword Spec (56/15/0)',1,0x00060000,130,130,130,130,130,641,641,641,126,126,662,662,121,121,121,136,136,136,131,131,123,123,123,123,123,133,134,134,1859,1859,135,1663,1663,1862,1862,2283,1860,1860,1824,1824,1661,1662,1662,1662,1664,2231,2231,2231,2231,2231,1863,1664,1824,2232,2232,2232,157,157,157,157,157,2250,2250,2250,161,161,160,661,661,661,161,762,489,494,483,485,484),
(NULL,'Warrior PVP Leveling Poleaxe Spec (56/15/0)',1,0x00060000,130,130,130,130,130,641,641,641,126,126,662,662,121,121,121,136,136,136,131,131,132,132,132,132,132,133,134,134,1859,1859,135,1663,1663,1862,1862,2283,1860,1860,1824,1824,1661,1662,1662,1662,1664,2231,2231,2231,2231,2231,1863,1664,1824,2232,2232,2232,157,157,157,157,157,2250,2250,2250,161,161,160,661,661,661,161,762,489,494,483,485,484),
(NULL,'Warrior PVP Leveling Mace Spec (56/15/0)',1,0x00060000,130,130,130,130,130,641,641,641,126,126,662,662,121,121,121,136,136,136,131,131,125,125,125,125,125,133,134,134,1859,1859,135,1663,1663,1862,1862,2283,1860,1860,1824,1824,1661,1662,1662,1662,1664,2231,2231,2231,2231,2231,1863,1664,1824,2232,2232,2232,157,157,157,157,157,2250,2250,2250,161,161,160,661,661,661,161,762,489,494,483,485,484),
(NULL,'Warrior Fury DPS (19/52/0)',1,0x00000006,157,157,157,157,157,2250,2250,2250,159,159,166,166,166,154,154,1581,1581,1581,1581,1581,165,1657,1657,1657,155,156,156,156,156,156,167,1655,1655,1864,1864,1658,1658,1658,1658,1658,1659,1866,1866,1866,155,2234,2234,2234,2234,2234,1867,1864,124,124,124,130,130,641,641,641,130,130,662,662,121,121,121,136,136,136,137,496,509,492,484,483,851),
(NULL,'Warrior Arms DPS Sword Spec (54/17/0)',1,0x00000006,130,130,130,130,130,641,641,641,127,127,662,662,121,121,121,136,136,136,2232,2232,133,123,123,123,123,123,1859,1859,2232,137,135,1663,1862,1862,131,1824,1824,1824,2283,131,1661,1662,1662,1662,1664,2231,2231,2231,2231,2231,1863,1664,1860,1860,157,157,157,157,157,2250,2250,2250,161,161,161,161,161,160,154,1542,1542,500,494,489,484,485,483),
(NULL,'Warrior Arms DPS Poleaxe Spec (54/17/0)',1,0x00000006,130,130,130,130,130,641,641,641,127,127,662,662,121,121,121,136,136,136,2232,2232,133,132,132,132,132,132,1859,1859,2232,137,135,1663,1862,1862,131,1824,1824,1824,2283,131,1661,1662,1662,1662,1664,2231,2231,2231,2231,2231,1863,1664,1860,1860,157,157,157,157,157,2250,2250,2250,161,161,161,161,161,160,154,1542,1542,500,494,489,484,485,483),
(NULL,'Warrior Arms DPS Mace Spec (54/17/0)',1,0x00000006,130,130,130,130,130,641,641,641,127,127,662,662,121,121,121,136,136,136,2232,2232,133,125,125,125,125,125,1859,1859,2232,137,135,1663,1862,1862,131,1824,1824,1824,2283,131,1661,1662,1662,1662,1664,2231,2231,2231,2231,2231,1863,1664,1860,1860,157,157,157,157,157,2250,2250,2250,161,161,161,161,161,160,154,1542,1542,500,494,489,484,485,483),
(NULL,'Warrior Protection (15/3/53)',1,0x00000011,1601,1601,1601,1601,1601,138,138,138,138,138,140,140,140,140,140,141,141,141,147,147,152,153,146,146,146,702,702,702,702,702,1660,1660,1660,1652,1652,1653,1653,1653,1654,1654,1666,1893,1893,1893,2236,1871,1871,1871,2246,2246,1872,148,2250,2250,2250,130,130,130,130,130,641,641,641,124,124,662,662,121,121,121,144,502,501,492,487,485,851),
(NULL,'Pet Ferocity BM Hunter (Wolf suggested)',-2,0x00000036,2112,2112,2112,2125,2125,2125,2113,2113,2107,2129,2129,2129,2157,2107,2128,2253,2253,2128,2254,2254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
(NULL,'Pet Tenacity BM Hunter (Bear suggested)',-3,0x00000001,2116,2116,2116,2173,2173,2117,2277,2117,2122,2163,2163,2122,2172,2237,2123,2255,2255,2258,2258,2123,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
(NULL,'Pet Cunning PvP (Bird of Prey suggested)',-1,0x00220000,2118,2118,2119,2127,2127,2127,2120,2120,2120,2165,2167,2167,2257,2257,2184,2278,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
UNLOCK TABLES;

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

-- ----------------------------
-- Table structure for `playerbot_quest_data`
-- ----------------------------
DROP TABLE IF EXISTS `playerbot_quest_data`;
CREATE TABLE `playerbot_quest_data` (
  `autocomplete` mediumint(8) unsigned NOT NULL,
  PRIMARY KEY (`autocomplete`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `pvpstats_battlegrounds`
--

DROP TABLE IF EXISTS `pvpstats_battlegrounds`;
CREATE TABLE `pvpstats_battlegrounds` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `winner_team` tinyint(3) NOT NULL,
  `bracket_id` tinyint(3) unsigned NOT NULL,
  `type` tinyint(3) unsigned NOT NULL,
  `date` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `pvpstats_battlegrounds`
--

LOCK TABLES `pvpstats_battlegrounds` WRITE;
/*!40000 ALTER TABLE `pvpstats_battlegrounds` DISABLE KEYS */;
/*!40000 ALTER TABLE `pvpstats_battlegrounds` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pvpstats_players`
--

DROP TABLE IF EXISTS `pvpstats_players`;
CREATE TABLE `pvpstats_players` (
  `battleground_id` bigint(20) unsigned NOT NULL,
  `character_guid` int(10) unsigned NOT NULL,
  `score_killing_blows` mediumint(8) unsigned NOT NULL,
  `score_deaths` mediumint(8) unsigned NOT NULL,
  `score_honorable_kills` mediumint(8) unsigned NOT NULL,
  `score_bonus_honor` mediumint(8) unsigned NOT NULL,
  `score_damage_done` mediumint(8) unsigned NOT NULL,
  `score_healing_done` mediumint(8) unsigned NOT NULL,
  `attr_1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `attr_2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `attr_3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `attr_4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `attr_5` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`battleground_id`, `character_guid`)
) ENGINE=InnoDB;

--
-- Dumping data for table `pvpstats_players`
--

LOCK TABLES `pvpstats_players` WRITE;
/*!40000 ALTER TABLE `pvpstats_players` DISABLE KEYS */;
/*!40000 ALTER TABLE `pvpstats_players` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `saved_variables`
--

DROP TABLE IF EXISTS `saved_variables`;
CREATE TABLE `saved_variables` (
  `NextArenaPointDistributionTime` bigint(40) unsigned NOT NULL DEFAULT '0',
  `NextDailyQuestResetTime` bigint(40) unsigned NOT NULL DEFAULT '0',
  `NextWeeklyQuestResetTime` bigint(40) unsigned NOT NULL DEFAULT '0',
  `NextMonthlyQuestResetTime` bigint(40) unsigned NOT NULL DEFAULT '0',
  `NextRandomBattlegroundResetTime` bigint(40) unsigned NOT NULL default '0',
  `cleaning_flags` int(11) unsigned NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Variable Saves';

--
-- Dumping data for table `saved_variables`
--

LOCK TABLES `saved_variables` WRITE;
/*!40000 ALTER TABLE `saved_variables` DISABLE KEYS */;
/*!40000 ALTER TABLE `saved_variables` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `world`
--

DROP TABLE IF EXISTS `world`;
CREATE TABLE `world` (
  `map` int(11) unsigned NOT NULL DEFAULT '0',
  `data` longtext,
  PRIMARY KEY (`map`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `world`
--

LOCK TABLES `world` WRITE;
/*!40000 ALTER TABLE `world` DISABLE KEYS */;
/*!40000 ALTER TABLE `world` ENABLE KEYS */;
UNLOCK TABLES;


/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

DROP TABLE IF EXISTS world_state;
CREATE TABLE world_state(
   Id INT(11) UNSIGNED NOT NULL COMMENT 'Internal save ID',
   Data longtext,
   PRIMARY KEY(`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='WorldState save system';

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2013-09-10  0:00:00

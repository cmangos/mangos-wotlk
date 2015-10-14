ALTER TABLE playerbotai_db_version CHANGE COLUMN required_6_playerbotai_combatorders required_7_playerbotai_quest_autocomplete bit;

-- MySQL dump 10.13  Distrib 5.5.25, for Linux (i686)
--
-- Host: localhost    Database: characters
-- ------------------------------------------------------
-- Server version	5.5.25-log
-- Date: 2012-09-26 20:59:31

--
-- Table structure for table `playerbot_quest_data`
--

DROP TABLE IF EXISTS `playerbot_quest_data`;
CREATE TABLE `playerbot_quest_data` (
  `autocomplete` mediumint(8) unsigned NOT NULL,
  PRIMARY KEY (`autocomplete`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Dumping data for table `playerbot_quest_data`
--

LOCK TABLES `playerbot_quest_data` WRITE;
UNLOCK TABLES;

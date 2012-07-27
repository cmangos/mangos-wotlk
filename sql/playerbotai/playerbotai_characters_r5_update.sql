ALTER TABLE playerbotai_db_version CHANGE COLUMN required_4_playerbotai_autoequip required_5_playerbotai_auto_follow bit;

/*
Navicat MySQL Data Transfer

Source Server         : WoW
Source Server Version : 50145
Source Host           : localhost:****
Source Database       : characters

Target Server Type    : MYSQL
Target Server Version : 50145
File Encoding         : 65001

Date: 2012-07-25 00:00:52
*/

ALTER TABLE `playerbot_saved_data`
    ADD COLUMN `auto_follow` INT(11) unsigned NOT NULL DEFAULT '1' AFTER combat_delay;

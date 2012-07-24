ALTER TABLE playerbotai_db_version CHANGE COLUMN required_3_playerbotai_combat_delay required_4_playerbotai_autoequip bit;

/*
Navicat MySQL Data Transfer

Source Server         : WoW
Source Server Version : 50145
Source Host           : localhost:****
Source Database       : characters

Target Server Type    : MYSQL
Target Server Version : 50145
File Encoding         : 65001

Date: 2012-07-23 00:00:00
*/

ALTER TABLE `playerbot_saved_data`
    ADD COLUMN `autoequip` tinyint(1) NOT NULL DEFAULT '0';

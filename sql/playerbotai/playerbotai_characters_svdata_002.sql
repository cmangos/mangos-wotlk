/*
Navicat MySQL Data Transfer

Source Server         : WoW
Source Server Version : 50145
Source Host           : localhost:****
Source Database       : characters

Target Server Type    : MYSQL
Target Server Version : 50145
File Encoding         : 65001

Date: 2012-03-15 00:00:52
*/

ALTER TABLE `playerbot_saved_data`
    ADD COLUMN `combat_delay` INT(11) unsigned NOT NULL DEFAULT '0';
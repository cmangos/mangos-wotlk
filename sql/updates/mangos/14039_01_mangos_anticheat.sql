ALTER TABLE db_version CHANGE COLUMN required_14038_01_mangos_gameobject_spawn_entry required_14039_01_mangos_anticheat bit;

/*
SQLyog Community v13.1.5  (64 bit)
MySQL - 5.7.20-log : Database - vengeance_world
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
/*Table structure for table `warden_scans` */

DROP TABLE IF EXISTS `warden_scans`;

CREATE TABLE `warden_scans` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `type` int(2) DEFAULT '0',
  `str` text,
  `data` text,
  `address` int(8) DEFAULT '0',
  `length` int(2) DEFAULT '0',
  `result` tinytext NOT NULL,
  `flags` smallint(5) unsigned NOT NULL,
  `comment` tinytext NOT NULL,
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=83 DEFAULT CHARSET=utf8;

/*Data for the table `warden_scans` */

insert  into `warden_scans`(`id`,`type`,`str`,`data`,`address`,`length`,`result`,`flags`,`comment`) values 
(1,2,NULL,'82D7E5CBC8D2F78A791E189BAB3FD5D4342BF7EB0CA3F129',74044,NULL,'0',2,'Cheat Engine dll'),
(2,2,NULL,'A444519CC419521B6D39990C1D95329C8D94B59226CBAA98',16507,NULL,'0',2,'WPE PRO dll'),
(3,2,NULL,'3A0F8985E701343E439C74B675C72BBE2D8810A745569913',372624,NULL,'0',2,'rPE dll'),
(4,2,NULL,'6A006A00E897AC0100',7072,NULL,'1',2,'Warden packet process code search sanity check'),
(5,1,'kernel32.dll',NULL,0,0,'1',2,'Warden module search bypass sanity check'),
(6,1,'wpespy.dll',NULL,0,0,'0',2,'WPE Pro'),
(7,1,'speedhack-i386.dll',NULL,0,0,'0',2,'CheatEngine'),
(8,1,'Maelstrom_Keys_Hook.dll',NULL,0,0,'0',2,'WoW Maelstrom keys hook'),
(9,1,'Maelstrom_Mess_Hook.dll',NULL,0,0,'0',2,'WoW Maelstrom mess hook'),
(10,1,'SZATYOR.DLL',NULL,0,0,'0',2,'Packet Sniffer - Injected DLL'),
(11,1,'ICANHAZSPEED.DLL',NULL,0,0,'0',2,'ICanHazSpeed - Injected DLL'),
(12,1,'RPE.DLL',NULL,0,0,'0',2,'rEdoX Packet Editor'),
(13,1,'SETPRIV.DLL',NULL,0,0,'0',2,'HideToolz - Injected DLL'),
(14,1,'SPEEDHACK.DLL',NULL,0,0,'0',2,'Cheat Engine - Injected DLL'),
(15,1,'TAMIA.DLL',NULL,0,0,'0',2,'Tamia hack'),
(16,1,'WHIFF.DLL',NULL,0,0,'0',2,'Packet Sniffer - Injected DLL'),
(17,1,'AUTH_BYPASS.DLL',NULL,0,0,'0',2,'Namreeb Auth Password Bypass');

DROP TABLE IF EXISTS `warden_check_driver`; 
DROP TABLE IF EXISTS `warden_check_lua`; 
DROP TABLE IF EXISTS `warden_check_memory`; 
DROP TABLE IF EXISTS `warden_check_memory_dynamic`; 
DROP TABLE IF EXISTS `warden_check_module`; 
DROP TABLE IF EXISTS `warden_check_mpq`; 
DROP TABLE IF EXISTS `warden_check_page_a`; 
DROP TABLE IF EXISTS `warden_check_page_b`; 
DROP TABLE IF EXISTS `warden_module`;

REPLACE INTO mangos_string VALUES
(67,'|c00FFFFFF|Announce:',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

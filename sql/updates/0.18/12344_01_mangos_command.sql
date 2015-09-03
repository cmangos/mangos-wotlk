ALTER TABLE db_version CHANGE COLUMN required_12339_01_mangos_calendar required_12344_01_mangos_command bit;

INSERT INTO `command` VALUES ('wp show', '2', 'Syntax: .wp show [#creature_guid or Select a Creature]\r\nwhere command can have one of the following values\r\non (to show all related wp)\r\nfirst (to see only first one)\r\nlast (to see only last one)\r\noff (to hide all related wp)\r\ninfo (to get more info about theses wp)\r\n\r\nFor using info you have to do first show on and than select a Visual-Waypoint and do the show info!')
ON DUPLICATE KEY UPDATE help=VALUES(help);
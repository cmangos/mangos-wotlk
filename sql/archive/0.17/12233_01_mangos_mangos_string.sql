ALTER TABLE db_version CHANGE COLUMN required_12189_01_mangos_creature_template_spells required_12233_01_mangos_mangos_string bit;

DELETE FROM `mangos_string` WHERE `entry`=175;
INSERT INTO `mangos_string` (`entry`,`content_default`) VALUES
(175, 'Liquid level: %f, ground: %f, type flags %u, status: %d.');

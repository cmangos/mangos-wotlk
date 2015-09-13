ALTER TABLE db_version CHANGE COLUMN required_12606_01_mangos_creature_template required_12615_01_mangos_creature_template_classlevelstats bit;

DROP TABLE IF EXISTS `creature_template_classlevelstats`;

CREATE TABLE `creature_template_classlevelstats` (
  `Level` tinyint(4) NOT NULL,
  `Class` tinyint(4) NOT NULL,
  `BaseHealthExp0` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `BaseHealthExp1` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `BaseHealthExp2` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `BaseMana` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `BaseDamageExp0` float NOT NULL DEFAULT '0',
  `BaseDamageExp1` float NOT NULL DEFAULT '0',
  `BaseDamageExp2` float NOT NULL DEFAULT '0',
  `BaseMeleeAttackPower` float NOT NULL DEFAULT '0',
  `BaseRangedAttackPower` float NOT NULL DEFAULT '0',
  `BaseArmor` mediumint(8) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`Level`,`Class`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

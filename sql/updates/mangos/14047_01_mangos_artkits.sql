ALTER TABLE db_version CHANGE COLUMN required_14046_01_mangos_groups_formation required_14047_01_mangos_artkits bit;

DROP TABLE IF EXISTS `gameobject_template_addon`;
CREATE TABLE `gameobject_template_addon`(
`Entry` INT UNSIGNED NOT NULL COMMENT 'gameobject_template',
`Artkit0` INT NOT NULL DEFAULT 0,
`Artkit1` INT NOT NULL DEFAULT 0,
`Artkit2` INT NOT NULL DEFAULT 0,
`Artkit3` INT NOT NULL DEFAULT 0,
PRIMARY KEY(entry)
);


ALTER TABLE db_version CHANGE COLUMN required_14014_01_mangos_conditions_upgrade required_14015_01_mangos_gameobject_template bit;

ALTER TABLE `gameobject_template`
    MODIFY COLUMN `data1` int(10) NOT NULL DEFAULT '0',
    MODIFY COLUMN `data6` int(10) NOT NULL DEFAULT '0';

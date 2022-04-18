ALTER TABLE db_version CHANGE COLUMN required_14054_01_mangos_command required_14055_01_mangos_worldstate_name bit;

DROP TABLE IF EXISTS worldstate_name;
CREATE TABLE worldstate_name (
`Id` INT(11) NOT NULL COMMENT 'Worldstate variable Id',
`Name` VARCHAR(200) NOT NULL COMMENT 'Name and use of variable',
PRIMARY KEY(`Id`)
);


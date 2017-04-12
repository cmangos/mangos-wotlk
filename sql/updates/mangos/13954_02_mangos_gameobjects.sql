ALTER TABLE db_version CHANGE COLUMN required_13954_01_mangos_creatures required_13954_02_mangos_gameobjects bit;

ALTER TABLE gameobject CHANGE spawntimesecs spawntimesecsmin INT(11) DEFAULT 0 NOT NULL COMMENT 'Gameobject respawn time minimum';

ALTER TABLE gameobject ADD spawntimesecsmax INT(11) DEFAULT 0 NOT NULL COMMENT 'Gameobject respawn time maximum' AFTER spawntimesecsmin;

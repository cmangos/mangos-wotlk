ALTER TABLE db_version CHANGE COLUMN required_14058_01_mangos_quest_maxlevel required_14059_01_mangos_vehicle_seat_accessory bit;

DROP TABLE IF EXISTS vehicle_seat_addon;
CREATE TABLE vehicle_seat_addon(
`SeatEntry` INT(11) UNSIGNED NOT NULL,
`SeatOrientation` FLOAT NOT NULL DEFAULT 0,
`ExitParamX` FLOAT NOT NULL DEFAULT 0,
`ExitParamY` FLOAT NOT NULL DEFAULT 0,
`ExitParamZ` FLOAT NOT NULL DEFAULT 0,
`ExitParamO` FLOAT NOT NULL DEFAULT 0,
`ExitParamValue` TINYINT(1) NOT NULL DEFAULT 0,
PRIMARY KEY(`SeatEntry`)
);

ALTER TABLE npc_spellclick_spells MODIFY `quest_start` MEDIUMINT(7) UNSIGNED NOT NULL DEFAULT '0';



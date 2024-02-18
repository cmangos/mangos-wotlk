ALTER TABLE realmd_db_version CHANGE COLUMN required_14064_01_realmd_platform required_14083_01_realmd_joindate_datetime bit;

ALTER TABLE `account` MODIFY joindate DATETIME NOT NULL DEFAULT NOW();



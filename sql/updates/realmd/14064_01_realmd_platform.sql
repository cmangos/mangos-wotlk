ALTER TABLE realmd_db_version CHANGE COLUMN required_14039_01_realmd_anticheat required_14064_01_realmd_platform bit;

ALTER TABLE account ADD COLUMN `platform` VARCHAR(4) NOT NULL DEFAULT '0' AFTER `os`;



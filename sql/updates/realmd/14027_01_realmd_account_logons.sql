ALTER TABLE realmd_db_version CHANGE COLUMN required_14004_01_realmd_banning required_14027_01_realmd_account_logons bit;

DROP TABLE IF EXISTS account_logons;
CREATE TABLE account_logons (
id INT PRIMARY KEY NOT NULL AUTO_INCREMENT,
accountId INT UNSIGNED NOT NULL,
ip varchar(30) NOT NULL,
loginTime timestamp NOT NULL,
loginSource INT UNSIGNED NOT NULL
);

ALTER TABLE account CHANGE `last_ip` `lockedIp` VARCHAR(30) NOT NULL DEFAULT '0.0.0.0';
ALTER TABLE account DROP COLUMN last_login;


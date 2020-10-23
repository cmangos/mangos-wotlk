ALTER TABLE realmd_db_version CHANGE COLUMN required_14028_01_realmd_account_locale_agnostic required_14035_01_realmd_raf bit;

DROP TABLE IF EXISTS account_raf;
CREATE TABLE account_raf(
referrer INT UNSIGNED NOT NULL DEFAULT '0',
referred INT UNSIGNED NOT NULL DEFAULT '0',
PRIMARY KEY(referrer, referred)
);


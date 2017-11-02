ALTER TABLE realmd_db_version CHANGE COLUMN required_10008_01_realmd_realmd_db_version required_13970_01_realmd_totp bit;

ALTER TABLE account ADD COLUMN token text AFTER locale;

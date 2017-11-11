ALTER TABLE db_version CHANGE COLUMN required_13969_01_mangos_spam_records required_13971_01_mangos_spam_records_length bit;

ALTER TABLE spam_records MODIFY record VARCHAR(512) NOT NULL;


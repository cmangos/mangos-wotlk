ALTER TABLE character_db_version CHANGE COLUMN required_13977_01_characters_taxi_system_format_update required_13991_01_characters_skills_cleanup_1829 bit;

DELETE a FROM character_skills AS a WHERE skill IN (760) AND guid IN (SELECT guid FROM characters b WHERE race IN (5) AND a.guid=b.guid);

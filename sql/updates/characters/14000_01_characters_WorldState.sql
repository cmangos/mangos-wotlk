ALTER TABLE character_db_version CHANGE COLUMN required_13994_00_characters_cooldown required_14000_01_characters_WorldState bit;

DROP TABLE IF EXISTS world_state;
CREATE TABLE world_state(
   Id INT(11) UNSIGNED NOT NULL COMMENT 'Internal save ID',
   Data longtext,
   PRIMARY KEY(`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='WorldState save system';



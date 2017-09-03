ALTER TABLE character_db_version CHANGE COLUMN required_13962_01_characters_event_group_chosen required_13963_01_characters_account_instances_entered bit;

DROP TABLE IF EXISTS `account_instances_entered`;
CREATE TABLE `account_instances_entered` (
   `AccountId` INT(11) UNSIGNED NOT NULL COMMENT 'Player account',
   `ExpireTime` BIGINT(40) NOT NULL COMMENT 'Time when instance was entered',
   `InstanceId` INT(11) UNSIGNED NOT NULL COMMENT 'ID of instance entered',
   PRIMARY KEY(`AccountId`,`InstanceId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='Instance reset limit system';

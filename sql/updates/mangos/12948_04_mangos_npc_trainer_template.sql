ALTER TABLE db_version CHANGE COLUMN required_12948_03_mangos_npc_trainer required_12948_04_mangos_npc_trainer_template bit;

ALTER TABLE npc_trainer_template ADD COLUMN `condition_id` INT(11) unsigned NOT NULL default '0' AFTER reqlevel;

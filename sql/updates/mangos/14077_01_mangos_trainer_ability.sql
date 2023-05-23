ALTER TABLE db_version CHANGE COLUMN required_14076_01_mangos_hoverheight required_14077_01_mangos_trainer_ability bit;

ALTER TABLE npc_trainer ADD COLUMN `ReqAbility1` INT(11) UNSIGNED DEFAULT NULL AFTER reqLevel;
ALTER TABLE npc_trainer ADD COLUMN `ReqAbility2` INT(11) UNSIGNED DEFAULT NULL AFTER ReqAbility1;
ALTER TABLE npc_trainer ADD COLUMN `ReqAbility3` INT(11) UNSIGNED DEFAULT NULL AFTER ReqAbility2;

ALTER TABLE npc_trainer_template ADD COLUMN `ReqAbility1` INT(11) UNSIGNED DEFAULT NULL AFTER reqLevel;
ALTER TABLE npc_trainer_template ADD COLUMN `ReqAbility2` INT(11) UNSIGNED DEFAULT NULL AFTER ReqAbility1;
ALTER TABLE npc_trainer_template ADD COLUMN `ReqAbility3` INT(11) UNSIGNED DEFAULT NULL AFTER ReqAbility2;



ALTER TABLE db_version CHANGE COLUMN required_14095_01_mangos_dbscript_breaking_change required_14096_01_mangos_closing_text bit;

ALTER TABLE `gameobject_template` CHANGE `castBarCaption` `OpeningText` varchar(100) NOT NULL DEFAULT '';
ALTER TABLE `gameobject_template` CHANGE `unk1` `ClosingText` varchar(100) NOT NULL DEFAULT '';

ALTER TABLE `locales_gameobject` CHANGE `name_loc1` `name_loc1` varchar(100);
ALTER TABLE `locales_gameobject` CHANGE `name_loc2` `name_loc2` varchar(100);
ALTER TABLE `locales_gameobject` CHANGE `name_loc3` `name_loc3` varchar(100);
ALTER TABLE `locales_gameobject` CHANGE `name_loc4` `name_loc4` varchar(100);
ALTER TABLE `locales_gameobject` CHANGE `name_loc5` `name_loc5` varchar(100);
ALTER TABLE `locales_gameobject` CHANGE `name_loc6` `name_loc6` varchar(100);
ALTER TABLE `locales_gameobject` CHANGE `name_loc7` `name_loc7` varchar(100);
ALTER TABLE `locales_gameobject` CHANGE `name_loc8` `name_loc8` varchar(100);

ALTER TABLE `locales_gameobject` CHANGE `castbarcaption_loc1` `opening_text_loc1` varchar(100);
ALTER TABLE `locales_gameobject` CHANGE `castbarcaption_loc2` `opening_text_loc2` varchar(100);
ALTER TABLE `locales_gameobject` CHANGE `castbarcaption_loc3` `opening_text_loc3` varchar(100);
ALTER TABLE `locales_gameobject` CHANGE `castbarcaption_loc4` `opening_text_loc4` varchar(100);
ALTER TABLE `locales_gameobject` CHANGE `castbarcaption_loc5` `opening_text_loc5` varchar(100);
ALTER TABLE `locales_gameobject` CHANGE `castbarcaption_loc6` `opening_text_loc6` varchar(100);
ALTER TABLE `locales_gameobject` CHANGE `castbarcaption_loc7` `opening_text_loc7` varchar(100);
ALTER TABLE `locales_gameobject` CHANGE `castbarcaption_loc8` `opening_text_loc8` varchar(100);

ALTER TABLE `locales_gameobject` ADD COLUMN `closing_text_loc1` varchar(100);
ALTER TABLE `locales_gameobject` ADD COLUMN `closing_text_loc2` varchar(100);
ALTER TABLE `locales_gameobject` ADD COLUMN `closing_text_loc3` varchar(100);
ALTER TABLE `locales_gameobject` ADD COLUMN `closing_text_loc4` varchar(100);
ALTER TABLE `locales_gameobject` ADD COLUMN `closing_text_loc5` varchar(100);
ALTER TABLE `locales_gameobject` ADD COLUMN `closing_text_loc6` varchar(100);
ALTER TABLE `locales_gameobject` ADD COLUMN `closing_text_loc7` varchar(100);
ALTER TABLE `locales_gameobject` ADD COLUMN `closing_text_loc8` varchar(100);


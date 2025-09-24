ALTER TABLE db_version CHANGE COLUMN required_14092_01_mangos_proc_cooldown required_14093_01_mangos_vehicle_ridespell bit;

ALTER TABLE `vehicle_accessory` ADD COLUMN `RideSpellId` INT(10) UNSIGNED AFTER `accessory_entry`;
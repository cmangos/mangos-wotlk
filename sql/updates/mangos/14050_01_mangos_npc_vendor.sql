ALTER TABLE db_version CHANGE COLUMN required_14049_01_mangos_unitflags2 required_14050_01_mangos_npc_vendor bit;

ALTER TABLE `npc_vendor` ADD COLUMN `slot` TINYINT(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `incrtime`;
ALTER TABLE `npc_vendor_template` ADD COLUMN `slot` TINYINT(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `incrtime`;

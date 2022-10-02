ALTER TABLE db_version CHANGE COLUMN required_14062_01_mangos_eai_dbguid_support required_14063_01_mangos_battleground_reflooot bit;

ALTER TABLE battleground_template ADD COLUMN PlayerSkinReflootId MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'reference_loot_template entry';



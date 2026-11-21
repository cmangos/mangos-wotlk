ALTER TABLE db_version CHANGE COLUMN required_14104_01_mangos_spell_threat_mask required_14106_01_mangos_creature_immunity_flags bit;

ALTER TABLE creature_template ADD COLUMN `ImmunityFlags` INT UNSIGNED NOT NULL DEFAULT 0 AFTER `SchoolImmuneMask`;



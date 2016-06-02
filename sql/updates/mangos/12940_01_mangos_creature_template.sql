ALTER TABLE db_version CHANGE COLUMN required_12939_01_mangos_spell_template required_12940_01_mangos_creature_template bit;

ALTER TABLE creature_template ADD COLUMN SchoolImmuneMask INT(10) UNSIGNED DEFAULT 0 NOT NULL AFTER MechanicImmuneMask; 
ALTER TABLE db_version CHANGE COLUMN required_14102_01_mangos_cls_rework required_14103_01_mangos_pet_spell_lists bit;

DROP TABLE IF EXISTS pet_autocast_spell_list;
CREATE TABLE pet_autocast_spell_list (
`CreatureEntry` INT UNSIGNED NOT NULL,
`SpellId` INT UNSIGNED NOT NULL,
`CombatCondition` INT NOT NULL DEFAULT '-1',
`TargetId` INT NOT NULL,
`Comments` VARCHAR(255) NOT NULL,
PRIMARY KEY(`CreatureEntry`, `SpellId`)
);


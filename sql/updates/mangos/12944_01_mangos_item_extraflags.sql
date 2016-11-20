ALTER TABLE db_version CHANGE COLUMN required_12943_01_mangos_flametongue required_12944_01_mangos_item_extraflags bit;

UPDATE item_template SET extraFlags = 1 WHERE extraFlags != 0;
ALTER TABLE db_version CHANGE COLUMN required_12941_01_mangos_event_linkedto required_12943_01_mangos_flametongue bit;

DELETE FROM spell_bonus_data WHERE entry=8026;
INSERT INTO spell_bonus_data VALUES('8026', '3.85', '0', '0', '0', 'Shaman - Flametongue Weapon Proc');

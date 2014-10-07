ALTER TABLE db_version CHANGE COLUMN required_12670_01_mangos_spell_template required_12748_01_mangos_spell_template bit;

DELETE FROM spell_template WHERE id IN (11756,19394);
INSERT INTO spell_template VALUES
(11756, 0x00000180, 0x10000000, 0x00000000, 0x00000000, 0x00000000, 101,   4,  76,  47,    0,  15,   0, 144064,  0,    0,     'Summon Gordunni chest (COBALT)'),
(19394, 0x00000180, 0x10000000, 0x00000000, 0x00000000, 0x00000000, 101,   4,  76,  47,    0,  15,   0, 177681,  0,    0,     'Summon Gordunni chest (JUNK)');

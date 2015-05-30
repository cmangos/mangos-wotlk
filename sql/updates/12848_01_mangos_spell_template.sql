ALTER TABLE db_version CHANGE COLUMN required_12823_04_mangos_creature required_12848_01_mangos_spell_template bit;

DELETE FROM spell_template WHERE id IN (68572,68574,68575);
INSERT INTO spell_template VALUES
(68572, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - Grand Champions'),
(68574, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - Confessor Paletress'),
(68575, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - Eadric the Pure');

ALTER TABLE db_version CHANGE COLUMN required_12848_01_mangos_spell_template required_12864_01_mangos_spell_template bit;

DELETE FROM spell_template WHERE id IN (68523,68620);
INSERT INTO spell_template VALUES
(68523, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - Trial of the Crusader - Not One, But Two Jormungars'),
(68620, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - Trial of the Crusader - Resilience Will Fix It');

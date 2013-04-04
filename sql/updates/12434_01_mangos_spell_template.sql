ALTER TABLE db_version CHANGE COLUMN required_12401_01_mangos_spell_template required_12434_01_mangos_spell_template bit;

ALTER TABLE spell_template ADD COLUMN attr int(11) unsigned NOT NULL DEFAULT '0' AFTER id;
ALTER TABLE spell_template ADD COLUMN attr_ex int(11) unsigned NOT NULL DEFAULT '0' AFTER attr;
ALTER TABLE spell_template ADD COLUMN attr_ex2 int(11) unsigned NOT NULL DEFAULT '0' AFTER attr_ex;
ALTER TABLE spell_template ADD COLUMN effect0_implicit_target_b int(11) unsigned NOT NULL DEFAULT '0' AFTER effect0_implicit_target_a;

DELETE FROM spell_template WHERE id IN (21387, 23363, 25192, 26133, 37264, 37278, 37365, 58630, 59046, 59450, 64899, 64985, 65074, 65195, 68184, 72845);
INSERT INTO spell_template VALUES
(21387, 0x00000140, 0x10000000, 0x00000000, 0x00000028,  15,  21,   6,   1,    0,   0,  42, 0,       0,    21388, 'Melt-Weapon trigger aura related used by Ragnaros'),
(23363, 0x00000100, 0x00000000, 0x00000000, 0x00000000, 101,  21,  76,  18,    0,   0,   0, 179804,  0,    0,     'Summon Drakonid Corpse Trigger'),
(25192, 0x00000100, 0x00000000, 0x00000004, 0x00000000, 101,  21,  76,  18,    0,   0,   0, 180619,  0,    0,     'Summon Ossirian Crystal'),
(26133, 0x00000100, 0x00000000, 0x00000000, 0x00000000, 101,  21,  76,  18,    0,   0,   0, 180795,  0,    0,     'Summon Sandworm Base'),
(37264, 0x00000180, 0x00000000, 0x00000004, 0x00000000, 101,  21,  28,  18,    0,   7,   0, 21729,  64,    0,     'Power Converters: Summon Electromental (from cata)'),
(37278, 0x00000180, 0x00000000, 0x00000004, 0x00000000, 101,  21,  28,  18,    0,   1,   0, 21737,  64,    0,     'Power Converters: Summon Mini-Electromental (from cata)'),
(37365, 0x00000180, 0x00000000, 0x00000004, 0x00000000, 101,  21,  28,  18,    0,   1,   0, 21757,  64,    0,     'Power Converters: Summon Big Flavor Electromental (from cata)'),
(58630, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - Mal\'Ganis'),
(59046, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - Tribunal of Ages'),
(59450, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - The Four Horsemen'),
(64899, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - Hodir'),
(64985, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - Thorim'),
(65074, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - Freya'),
(65195, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - Assembly of Iron'),
(68184, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - Faction Champions'),
(72845, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - Pit of Saron - Don\'t Look Up');

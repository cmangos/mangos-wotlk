ALTER TABLE db_version CHANGE COLUMN required_12363_02_mangos_npc_vendor_template required_12401_01_mangos_spell_template bit;

DELETE FROM spell_template WHERE id IN (37264, 37278, 37365);
INSERT INTO spell_template VALUES
(37264, 0x00000000, 101,  21,  28,  18,   7,   0, 21729, 64,     0,     'Power Converters: Summon Electromental (from cata)'),
(37278, 0x00000000, 101,  21,  28,  18,   1,   0, 21737, 64,     0,     'Power Converters: Summon Mini-Electromental (from cata)'),
(37365, 0x00000000, 101,  21,  28,  18,   1,   0, 21757, 64,     0,     'Power Converters: Summon Big Flavor Electromental (from cata)');

ALTER TABLE db_version CHANGE COLUMN required_12938_01_mangos_scriptdev2_tables required_12939_01_mangos_spell_template bit;

DELETE FROM spell_template WHERE id IN (72827);
INSERT INTO spell_template VALUES
(72827, 0x00800180, 0x00000000, 0x00000005, 0x00000000, 0x00000000, 101,  37,   6,  22,    7,  28,   4, 0,       0,    0,     'Achievement check - Icecrown Citadel - Full House');

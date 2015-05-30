ALTER TABLE db_version CHANGE COLUMN required_12654_01_mangos_creature_template_power required_12670_01_mangos_spell_template bit;

DELETE FROM spell_template WHERE id=21883;
INSERT INTO spell_template VALUES
(21883, 0x00000180, 0x10000000, 0x00000000, 0x00000000, 0x00000000, 101, 225,  76,  22,    0,   0,   0, 178904,  0,    0,     'Summon Healed Celebrian Vine');

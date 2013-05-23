ALTER TABLE db_version CHANGE COLUMN required_12465_01_mangos_spell_template required_12505_01_mangos_spell_template bit;

ALTER TABLE spell_template ADD COLUMN attr_ex3 int(11) unsigned NOT NULL DEFAULT '0' AFTER attr_ex2;

DELETE FROM spell_template WHERE id=23770;
INSERT INTO spell_template VALUES
(23770, 0x24800100, 0x10000088, 0x00000001, 0x00100000, 0x00000000, 101, 367,   6, 25,     0,   0,   4, 0,       0,    0,     'Sayge''s timer - Darkmoon Faire');

ALTER TABLE db_version CHANGE COLUMN required_12401_01_mangos_spell_template required_12416_01_mangos_anticheat bit;

-- Anticheat
DELETE FROM `mangos_string` WHERE `entry`='11000';
INSERT INTO `mangos_string`
    (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`)
VALUES
    (11000, '|cffff0000[Anticheat]: Cheater detected. Nickname: %s. Cheat type: %s.|r', NULL, NULL, NULL, NULL, NULL, NULL, NULL, '|cffff0000[Античит]: Читер обнаружен. Имя: %s. Тип чита: %s.|r');

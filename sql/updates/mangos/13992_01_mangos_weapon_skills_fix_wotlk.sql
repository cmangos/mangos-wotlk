ALTER TABLE db_version CHANGE COLUMN required_13990_01_mangos_playercreate_skills_wotlk required_13992_01_mangos_weapon_skills_fix_wotlk bit;

-- Remove existing Dual Wield entries for Rogues (3.3+)
DELETE FROM `playercreateinfo_skills` WHERE `skill` IN (118) AND `classMask` IN (8);

-- Remove existing weapon skill entries
DELETE FROM `playercreateinfo_skills` WHERE `skill` IN (43, 44, 45, 46, 54, 55, 136, 160, 172, 173, 176, 226, 228, 229);

INSERT INTO `playercreateinfo_skills` (`raceMask`, `classMask`, `skill`, `step`, `note`) VALUES
-- WARRIOR CLASS:
(167,   1,  44, 0,  'Weapon: Axes (Warrior)'),
(216,   1, 173, 0,  'Weapon: Daggers (Warrior)'),
(1133,  1,  54, 0,  'Weapon: Maces (Warrior)'),
(1115,  1,  43, 0,  'Weapon: Swords (Warrior)'),
(128,   1, 176, 0,  'Weapon: Thrown (Warrior)'),
(6,     1, 172, 0,  'Weapon: Two-Handed Axes (Warrior)'),
(32,    1, 160, 0,  'Weapon: Two-Handed Maces (Warrior)'),
(1040,  1,  55, 0,  'Weapon: Two-Handed Swords (Warrior)'),
-- PALADIN CLASS:
(1029,  2,  54, 0,  'Weapon: Maces (Paladin)'),
(512,   2,  43, 0,  'Weapon: Swords (Paladin)'),
(1029,  2, 160, 0,  'Weapon: Two-Handed Maces (Paladin)'),
(512,   2,  55, 0,  'Weapon: Two-Handed Swords (Paladin)'),
-- HUNTER CLASS:
(166,   4,  44, 0,  'Weapon: Axes (Hunter)'),
(650,   4,  45, 0,  'Weapon: Bows (Hunter)'),
(1024,  4, 226, 0,  'Weapon: Crossbows (Hunter)'),
(520,   4, 173, 0,  'Weapon: Daggers (Hunter)'),
(36,    4,  46, 0,  'Weapon: Guns (Hunter)'),
(1024,  4,  43, 0,  'Weapon: Swords (Hunter)'),
-- ROGUE CLASS:
(0,     8, 173, 0,  'Weapon: Daggers (Rogue)'),
(0,     8, 176, 0,  'Weapon: Thrown (Rogue)'),
-- PRIEST CLASS:
(0,    16,  54, 0,  'Weapon: Maces (Priest)'),
(0,    16, 228, 0,  'Weapon: Wands (Priest)'),
-- DEATH KNIGHT CLASS:
(0,    32,  44, 0,  'Weapon: Axes (Death Knight)'),
(0,    32, 229, 0,  'Weapon: Polearms (Death Knight)'),
(0,    32,  43, 0,  'Weapon: Swords (Death Knight)'),
(0,    32, 172, 0,  'Weapon: Two-Handed Axes (Death Knight)'),
(0,    32,  55, 0,  'Weapon: Two-Handed Swords (Death Knight)'),
-- SHAMAN CLASS:
(0,    64,  54, 0,  'Weapon: Maces (Shaman)'),
(0,    64, 136, 0,  'Weapon: Staves (Shaman)'),
-- MAGE CLASS:
(0,   128, 136, 0,  'Weapon: Staves (Mage)'),
(0,   128, 228, 0,  'Weapon: Wands (Mage)'),
-- WARLOCK CLASS:
(0,   256, 173, 0,  'Weapon: Daggers (Warlock)'),
(0,   256, 228, 0,  'Weapon: Wands (Warlock)'),
-- DRUID CLASS:
(8,  1024, 173, 0,  'Weapon: Daggers (Druid)'),
(32, 1024,  54, 0,  'Weapon: Maces (Druid)'),
(0,  1024, 136, 0,  'Weapon: Staves (Druid)');

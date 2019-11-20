DROP TABLE IF EXISTS spell_scripts;
CREATE TABLE spell_scripts(
Id INT NOT NULL COMMENT 'Spell ID',
ScriptName CHAR(64) NOT NULL COMMENT 'Core Spell Script Name',
PRIMARY KEY(Id)
);

-- Classic
INSERT INTO spell_scripts(Id, ScriptName) VALUES
(785,'spell_true_fulfillment'),
(794,'spell_initialize_images'),
(7054,'spell_forsaken_skill'),
(19832,'spell_possess_razorgore'),
(19873,'spell_destroy_egg'),
(20038,'spell_explosion_razorgore'),
(26591,'spell_teleport_image');

-- TBC
INSERT INTO spell_scripts(Id, ScriptName) VALUES
(38915,'spell_mental_interference'),
(40186,'spell_summon_blossom_move_target'),
(40251,'spell_shadow_of_death'),
(41289,'spell_ethereal_ring_signal_flare'),
(41999,'spell_shadow_of_death_remove'),
(44006,'spell_teleport_self_akilzon');

-- Wotlk


-- Priest
INSERT INTO spell_scripts(Id, ScriptName) VALUES
(32676,'spell_consume_magic');


-- Warlock
INSERT INTO spell_scripts(Id, ScriptName) VALUES
(980,'spell_curse_of_agony'),
(1014,'spell_curse_of_agony'),
(6217,'spell_curse_of_agony'),
(11711,'spell_curse_of_agony'),
(11712,'spell_curse_of_agony'),
(11713,'spell_curse_of_agony'),
(27218,'spell_curse_of_agony'),
(30108,'spell_unstable_affliction'),
(30404,'spell_unstable_affliction'),
(30405,'spell_unstable_affliction');


/* INITIAL CLEANUP */
-- Clear all ScriptNames
-- This will clear all ScriptNames from any table in the World-Database

TRUNCATE scripted_areatrigger;
TRUNCATE scripted_event_id;
UPDATE creature_template SET ScriptName='';
UPDATE gameobject_template SET ScriptName='';
UPDATE item_template SET ScriptName='';
UPDATE instance_template SET ScriptName='';
UPDATE world_template SET ScriptName='';
/*  */

/* AREATRIGGER */
INSERT INTO scripted_areatrigger VALUES
(171,'at_huldar_miran'),
(1726,'at_scent_larkorwi'),
(1727,'at_scent_larkorwi'),
(1728,'at_scent_larkorwi'),
(1729,'at_scent_larkorwi'),
(1730,'at_scent_larkorwi'),
(1731,'at_scent_larkorwi'),
(1732,'at_scent_larkorwi'),
(1733,'at_scent_larkorwi'),
(1734,'at_scent_larkorwi'),
(1735,'at_scent_larkorwi'),
(1736,'at_scent_larkorwi'),
(1737,'at_scent_larkorwi'),
(1738,'at_scent_larkorwi'),
(1739,'at_scent_larkorwi'),
(1740,'at_scent_larkorwi'),
(1966,'at_murkdeep'),
(2026,'at_blackrock_spire'),
(2046,'at_blackrock_spire'),
(2066,'at_blackrock_spire'),
(2067,'at_blackrock_spire'),
(3066,'at_ravenholdt'),
(3146,'at_hive_tower'),
-- Darnassian bank
(3546,'at_childrens_week_spot'),
-- Undercity - Throne Room
(3547,'at_childrens_week_spot'),
-- Stonewrought Dam
(3548,'at_childrens_week_spot'),
-- The Mor'shan Rampart
(3549,'at_childrens_week_spot'),
-- Ratchet Docks
(3550,'at_childrens_week_spot'),
-- Westfall Lighthouse
(3552,'at_childrens_week_spot'),
(3587,'at_ancient_leaf'),
(3626,'at_vaelastrasz'),
(3958,'at_zulgurub'),
(3960,'at_zulgurub'),
(4017,'at_twilight_grove'),
(4047,'at_temple_ahnqiraj'),
(4052,'at_temple_ahnqiraj'),
(4089,'at_cathedral_entrance'),
(4112,'at_naxxramas'),
(4113,'at_naxxramas'),
(4288,'at_dark_portal'),
(4422,'at_area_52'),
(4466,'at_area_52'),
(4471,'at_area_52'),
(4472,'at_area_52'),
(4479,'at_haramad_teleport'),
(4485,'at_dark_portal'),
(4524,'at_shattered_halls'),
(4560,'at_legion_teleporter'),
(4591,'at_coilfang_waterfall'),
(4871,'at_warsong_farms'),
(4872,'at_warsong_farms'),
(4873,'at_warsong_farms'),
(4937,'at_sunwell_plateau'),
(5030,'at_spearborn_encampment'),
(5046,'at_waygate'),
(5047,'at_waygate'),
(5108,'at_stormwright_shelf'),
(5284,'at_aldurthar_gate'),
(5285,'at_aldurthar_gate'),
(5286,'at_aldurthar_gate'),
(5287,'at_aldurthar_gate'),
(5710,'at_hot_on_the_trail'),
(5711,'at_hot_on_the_trail'),
(5712,'at_hot_on_the_trail'),
(5714,'at_hot_on_the_trail'),
(5715,'at_hot_on_the_trail'),
(5716,'at_hot_on_the_trail');

/* BATTLEGROUNDS */
UPDATE creature_template SET ScriptName='npc_spirit_guide' WHERE entry IN (13116, 13117);

/* WORLD BOSS */
UPDATE creature_template SET ScriptName='boss_ysondre' WHERE entry=14887;
UPDATE creature_template SET ScriptName='boss_emeriss' WHERE entry=14889;
UPDATE creature_template SET ScriptName='boss_taerar' WHERE entry=14890;
UPDATE creature_template SET ScriptName='boss_azuregos' WHERE entry=6109;
UPDATE creature_template SET ScriptName='boss_lethon' WHERE entry=14888;
UPDATE creature_template SET ScriptName='npc_spirit_shade' WHERE entry=15261;
UPDATE creature_template SET ScriptName='boss_highlord_kruul' WHERE entry=18338;
UPDATE creature_template SET ScriptName='npc_infernal_hound' WHERE entry=19207;

/* GO */
UPDATE gameobject_template SET ScriptName='go_ethereum_prison' WHERE entry BETWEEN 184418 AND 184431;
UPDATE creature_template SET ScriptName='npc_ethereum_prisoner' WHERE entry IN(20520,20889);
UPDATE gameobject_template SET ScriptName='go_ethereum_stasis' WHERE entry BETWEEN 185465 AND 185467;
UPDATE gameobject_template SET ScriptName='go_ethereum_stasis' WHERE entry=184595;
UPDATE gameobject_template SET ScriptName='go_ethereum_stasis' WHERE entry BETWEEN 185461 AND 185464;
UPDATE gameobject_template SET ScriptName='go_stasis_chamber_alpha' WHERE entry IN(185512);
UPDATE gameobject_template SET ScriptName='go_jump_a_tron' WHERE entry=183146;
UPDATE gameobject_template SET ScriptName='go_mysterious_snow_mound' WHERE entry=195308;
UPDATE gameobject_template SET ScriptName='go_tele_to_dalaran_crystal' WHERE entry=191230;
UPDATE gameobject_template SET ScriptName='go_tele_to_violet_stand' WHERE entry=191229;
UPDATE gameobject_template SET ScriptName='go_andorhal_tower' WHERE entry IN (176094,176095,176096,176097);
UPDATE gameobject_template SET ScriptName='go_scourge_enclosure' WHERE entry=191548;
UPDATE gameobject_template SET ScriptName='go_bells' WHERE entry IN(182064,175885,176573);
UPDATE gameobject_template SET ScriptName='go_darkmoon_faire_music' WHERE entry=180335;
UPDATE gameobject_template SET ScriptName='go_pirate_day_music' WHERE entry=190363;
UPDATE gameobject_template SET ScriptName='go_midsummer_music' WHERE entry=188174;
UPDATE gameobject_template SET ScriptName='go_brewfest_music' WHERE entry=186221;
UPDATE gameobject_template SET ScriptName='go_elemental_rift' WHERE entry IN (179664, 179665, 179666, 179667);
UPDATE gameobject_template SET ScriptName='go_dragon_head' WHERE entry IN(179556,179558,179881,179882);
UPDATE gameobject_template SET ScriptName='go_unadorned_spike' WHERE entry IN(175787);
UPDATE gameobject_template SET ScriptName='go_containment_coffer' WHERE entry=122088;
UPDATE gameobject_template SET ScriptName='go_large_jack_o_lantern' WHERE entry=186887;
UPDATE gameobject_template SET ScriptName='go_imp_in_a_ball' WHERE entry=185898;

/* Outdoor PVP*/
-- si
UPDATE gameobject_template SET ScriptName='go_outdoor_pvp_notify' WHERE entry IN(181597,181598);
-- ep
UPDATE gameobject_template SET ScriptName='go_outdoor_pvp_notify' WHERE entry IN(181682,181955);
-- halaa
UPDATE gameobject_template SET ScriptName='go_outdoor_pvp_notify' WHERE entry IN(182297,182298,182299,182300,182266,182275,182276,182277);
UPDATE gameobject_template SET ScriptName='go_outdoor_pvp_notify' WHERE entry IN(182301,182302,182303,182304,182267,182280,182281,182282);
UPDATE gameobject_template SET ScriptName='go_outdoor_pvp_notify' WHERE entry IN(182222,182272,182273,182274,182305,182306,182307,182308);

/* GUARD */
UPDATE creature_template SET ScriptName='guard_azuremyst' WHERE entry=18038;
UPDATE creature_template SET ScriptName='guard_orgrimmar' WHERE entry IN (3296,14304);
UPDATE creature_template SET ScriptName='guard_stormwind' WHERE entry IN (68,1756,1976);
UPDATE creature_template SET ScriptName='guard_contested' WHERE entry IN (9460,4624,3502,11190,15184);
UPDATE creature_template SET ScriptName='guard_elwynnforest' WHERE entry=1423;
UPDATE creature_template SET ScriptName='guard_eversong' WHERE entry=16221;
UPDATE creature_template SET ScriptName='guard_darnassus' WHERE entry=4262;
UPDATE creature_template SET ScriptName='guard_teldrassil' WHERE entry=3571;
UPDATE creature_template SET ScriptName='guard_ironforge' WHERE entry=5595;
UPDATE creature_template SET ScriptName='guard_dunmorogh' WHERE entry IN (727,13076);
UPDATE creature_template SET ScriptName='guard_undercity' WHERE entry=5624;
UPDATE creature_template SET ScriptName='guard_bluffwatcher' WHERE entry=3084;
UPDATE creature_template SET ScriptName='guard_durotar' WHERE entry=5953;
UPDATE creature_template SET ScriptName='guard_mulgore' WHERE entry IN (3212,3215,3217,3218,3219,3220,3221,3222,3223,3224);
UPDATE creature_template SET ScriptName='guard_tirisfal' WHERE entry IN (1735,1738,2210,1744,1745,5725,1743,2209,1746,1742);
UPDATE creature_template SET ScriptName='guard_silvermoon' WHERE entry=16222;
UPDATE creature_template SET ScriptName='guard_exodar' WHERE entry=16733;
UPDATE creature_template SET ScriptName='guard_shattrath' WHERE entry=19687;
UPDATE creature_template SET ScriptName='guard_shattrath_aldor' WHERE entry=18549;
UPDATE creature_template SET ScriptName='guard_shattrath_scryer' WHERE entry=18568;
UPDATE creature_template SET ScriptName='guard_hallows_end_helper' WHERE entry=24491; -- Deathguard Florence (only spawned for Hallow's End) + AI is base of guard_azuremyst, guard_elwynnforest, guard_eversong, guard_dunmorogh, and guard_durotar

/* ITEM */
UPDATE item_template SET ScriptName='item_arcane_charges' WHERE entry=34475;
UPDATE item_template SET ScriptName='item_flying_machine' WHERE entry IN (34060,34061);
UPDATE item_template SET ScriptName='item_gor_dreks_ointment' WHERE entry=30175;
UPDATE item_template SET ScriptName='item_petrov_cluster_bombs' WHERE entry=33098;
UPDATE item_template SET ScriptName='item_orb_of_draconic_energy' WHERE entry=12300;

/* NPC (usually creatures to be found in more than one specific zone) */
UPDATE creature_template SET ScriptName='npc_air_force_bots' WHERE entry IN (2614, 2615, 21974, 21993, 21996, 21997, 21999, 22001, 22002, 22003, 22063, 22065, 22066, 22068, 22069, 22070, 22071, 22078, 22079, 22080, 22086, 22087, 22088, 22090, 22124, 22125, 22126);
UPDATE creature_template SET ScriptName='npc_chicken_cluck' WHERE entry=620;
UPDATE creature_template SET ScriptName='npc_garments_of_quests' WHERE entry IN (12429,12423,12427,12430,12428);
UPDATE creature_template SET ScriptName='npc_guardian' WHERE entry=5764;
UPDATE creature_template SET ScriptName='npc_doctor' WHERE entry IN (12939,12920);
UPDATE creature_template SET ScriptName='npc_injured_patient' WHERE entry IN (12936,12937,12938,12923,12924,12925);
UPDATE creature_template SET ScriptName='npc_prof_blacksmith' WHERE entry IN (5164,11145,11146,11176,11177,11178,11191,11192,11193);
UPDATE creature_template SET ScriptName='npc_prof_leather' WHERE entry IN (7866,7867,7868,7869,7870,7871);
-- disabled, but can be used for custom
-- UPDATE creature_template SET ScriptName='' WHERE npcflag!=npcflag|65536 AND ScriptName='npc_innkeeper';
-- UPDATE creature_template SET ScriptName='npc_innkeeper' WHERE npcflag=npcflag|65536;
UPDATE creature_template SET ScriptName='npc_spring_rabbit' WHERE entry=32791;
UPDATE creature_template SET ScriptName='npc_redemption_target' WHERE entry IN (6172,6177,17542,17768);
UPDATE creature_template SET ScriptName='npc_burster_worm' WHERE entry IN (16844,16857,16968,17075,18678,21380,21849,22038,22466,22482,23285);
UPDATE creature_template SET ScriptName='npc_aoe_damage_trigger' WHERE entry IN (16697,17471,18370);
UPDATE creature_template SET ScriptName='npc_the_cleaner' WHERE entry=14503;
UPDATE creature_template SET ScriptName='npc_shaman_earth_elemental' WHERE entry IN(15352); -- greater earth elemental
UPDATE creature_template SET ScriptName='npc_shaman_fire_elemental' WHERE entry IN(15438); -- greater fire elemental
UPDATE creature_template SET ScriptName='npc_snakes' WHERE entry IN(19921,19833);
UPDATE creature_template SET ScriptName='npc_nether_ray' WHERE entry IN(18880,21901,23501);
UPDATE creature_template SET ScriptName='npc_mage_mirror_image' WHERE entry=31216;
UPDATE creature_template SET ScriptName='npc_mojo' WHERE entry=24480;
UPDATE creature_template SET ScriptName='npc_fire_nova_totem' WHERE entry IN (15483,24320);
UPDATE creature_template SET ScriptName='npc_orphan_matron' WHERE entry IN (24519,23973);
UPDATE creature_template SET ScriptName='npc_orphan' WHERE entry IN (23712,23971);
UPDATE creature_template SET ScriptName='npc_shade_of_the_horseman' WHERE entry=23543;
UPDATE creature_template SET ScriptName='npc_headless_horseman_fire' WHERE entry=23537;
UPDATE creature_template SET ScriptName='npc_child_tbc' WHERE entry IN(22817,22818);
UPDATE creature_template SET ScriptName='npc_advanced_target_dummy' WHERE entry IN (2674); -- Advanced Target Dummy
UPDATE creature_template SET ScriptName='npc_imp_in_a_ball' WHERE entry IN (23224,23229);
UPDATE creature_template SET ScriptName='npc_gossip_npc' WHERE entry IN (18927,19148,19171,19172,19173,19169,19175,19176,19177,19178,20102);

/*Quest (quest scripts which are not in one zone)*/
UPDATE creature_template SET ScriptName='npc_quest_attunement' WHERE entry IN(22421,18528,19935);

/* SPELL */
UPDATE creature_template SET ScriptName='spell_dummy_npc' WHERE entry IN (
-- eastern kingdoms
8888,13016,
-- kalimdor
9299,12296,12298,
-- outland
16880,16518,16847,17157,17326,17654,18879,21729,24918,24922,25084,25085,
-- northrend
23678,25752,25753,25758,25792,25793,26268,26270,26421,26616,26643,26841,27122,27808,28053,28054,28068,28093,28465,28600,29319,29327,29329,29330,29338,30146,30169,32149);

UPDATE gameobject_template SET ScriptName='spell_dummy_go' WHERE entry IN (181616,186949);

/* WORLD MAP SCRIPTS */
DELETE FROM world_template WHERE map IN (0, 1, 530, 571, 609);
INSERT INTO world_template VALUES
(0,'world_map_eastern_kingdoms'),
(1,'world_map_kalimdor'),
(530,'world_map_outland'),
(571,'world_map_northrend'),
(609,'world_map_ebon_hold');

-- gromgol UC
INSERT INTO scripted_event_id VALUES
(15314,'event_transports'),
(15315,'event_transports'),
(15312,'event_transports'),
(15313,'event_transports');
-- OG UC
INSERT INTO scripted_event_id VALUES
(15318,'event_transports'),
(15319,'event_transports'),
(15320,'event_transports'),
(15321,'event_transports');
-- gromgol OG
INSERT INTO scripted_event_id VALUES
(15322,'event_transports'),
(15323,'event_transports'),
(15324,'event_transports'),
(15325,'event_transports');
-- WK
INSERT INTO scripted_event_id VALUES
(15430,'event_transports'),
(15431,'event_transports');
-- UC VL
INSERT INTO scripted_event_id VALUES
(19126,'event_transports'),
(19127,'event_transports');
-- OG BT
INSERT INTO scripted_event_id VALUES
(19137,'event_transports'),
(19139,'event_transports');
-- OG TB
INSERT INTO scripted_event_id VALUES
(21868,'event_transports'),
(21869,'event_transports'),
(21870,'event_transports'),
(21871,'event_transports');

/* War Effort*/
UPDATE creature_template SET ScriptName='npc_war_effort' WHERE
entry IN(15383,15431,15432,15434,15437,15445,15446,15448,15450,15451,15452,15453,15455,15456,15457,15459,15460,15469,15477,15508,15512,15515,15522,15525,15528,15529,15532,15533,15534,15535);
UPDATE gameobject_template SET ScriptName='go_scarab_gong' WHERE entry=180717; -- The Scarab Gong
UPDATE creature_template SET ScriptName='npc_silithus_boss' WHERE entry IN(15742,15741,15740);

/*Midsummer*/
UPDATE gameobject_template SET ScriptName='go_midsummer_bonfire' WHERE entry IN(187946,187945,187944,187943,187942,187941,187940,187939,187938,187937,187936,187935,187934,187933,187932,187931,187930,187929,187928,187927,187926,187925,187924,187923,187922,187921,187920,187919,187917,187916,187914,187564,187971,187973,187952,187963,187950,187961,187959,187957,187968,187948,187953,187970,187966,187975,187969,187951,187956,187954,187947,187972,187964,187559,187965,187949,187955,187967,187958,187974,187960,187962,181332,181333,181334,181335,181336,181337,188128,188129);
UPDATE creature_template SET ScriptName='npc_torch_tossing_bunny_controller' WHERE entry IN(25536);

/* Brewfest */
UPDATE creature_template SET ScriptName='npc_brewfest_barker' WHERE entry IN (23683,23684,23685,23710,24492,24493,24495,24710,24711); -- Maeve Barleybrew, Ita Thunderbrew, Gordok Brew Barker, Belbi Quikswitch, Drohn's Distillery Barker, T'chali's Voodoo Brewery Barker, Blix Fixwidget, Ipfelkofer Ironkeg, Tapper Swindlekeg
INSERT INTO scripted_areatrigger VALUES
(4712,'at_brewfest_barker'), -- Ita Thunderbrew
(4715,'at_brewfest_barker'), -- Maeve Barleybrew
(4716,'at_brewfest_barker'), -- Gordok Brew Barker (Alliance)
(4718,'at_brewfest_barker'), -- Belbi Quikswitch
(4797,'at_brewfest_barker'), -- Gordok Brew Barker (Horde)
(4798,'at_brewfest_barker'), -- Drohn's Distillery Barker
(4799,'at_brewfest_barker'), -- T'chali's Voodoo Brewery Barker
(4800,'at_brewfest_barker'), -- Blix Fixwidget
(4820,'at_brewfest_barker'), -- Ipfelkofer Ironkeg
(4829,'at_brewfest_barker'); -- Tapper Swindlekeg
INSERT INTO scripted_areatrigger VALUES
(4769,'at_brewfest_quest_barking'),
(4770,'at_brewfest_quest_barking'),
(4772,'at_brewfest_quest_barking'),
(4774,'at_brewfest_quest_barking'),
(4801,'at_brewfest_quest_barking'),
(4802,'at_brewfest_quest_barking'),
(4803,'at_brewfest_quest_barking'),
(4804,'at_brewfest_quest_barking');
-- Horde
INSERT INTO scripted_areatrigger VALUES
(4807,'at_brewfest_receive_keg'),
(4808,'at_brewfest_send_keg');
-- Alliance
INSERT INTO scripted_areatrigger VALUES
(4786,'at_brewfest_receive_keg'),
(4787,'at_brewfest_send_keg');

/* Scourge Invasion */
UPDATE creature_template SET ScriptName='scourge_invasion_necrotic_shard' WHERE entry IN (16136,16172);
UPDATE creature_template SET ScriptName='scourge_invasion_necropolis' WHERE entry=16401;
UPDATE creature_template SET ScriptName='scourge_invasion_mouth' WHERE entry=16995;
UPDATE creature_template SET ScriptName='scourge_invasion_necropolis_health' WHERE entry=16421;
UPDATE creature_template SET ScriptName='scourge_invasion_necropolis_relay' WHERE entry=16386;
UPDATE creature_template SET ScriptName='scourge_invasion_necropolis_proxy' WHERE entry=16398;
UPDATE creature_template SET ScriptName='scourge_invasion_minion_spawner' WHERE entry IN (16306,16336,16338);
UPDATE creature_template SET ScriptName='scourge_invasion_cultist_engineer' WHERE entry=16230;
UPDATE creature_template SET ScriptName='scourge_invasion_minion' WHERE entry IN (16143,16383);
UPDATE creature_template SET ScriptName='npc_pallid_horror' WHERE entry IN (16394,16382);
UPDATE gameobject_template SET ScriptName='scourge_invasion_go_circle' WHERE entry=181136;
UPDATE gameobject_template SET ScriptName='scourge_invasion_go_necropolis' WHERE entry IN (181154,181215,181223,181374,181373);

/* Shaman */
UPDATE creature_template SET ScriptName='npc_sentry_totem' WHERE entry=3968;

/* Death Knight */
UPDATE creature_template SET ScriptName='npc_gargoyle_dk' WHERE entry=27829;

/*  */
/* ZONE */
/* */

/* ALTERAC MOUNTAINS */

/* ALTERAC VALLEY */

/* ARATHI HIGHLANDS */
UPDATE creature_template SET ScriptName='npc_professor_phizzlethorpe' WHERE entry=2768;
UPDATE creature_template SET ScriptName='npc_kinelory' WHERE entry=2713;

/* ASHENVALE */
UPDATE creature_template SET ScriptName='npc_muglash' WHERE entry=12717;
UPDATE gameobject_template SET ScriptName='go_naga_brazier' WHERE entry=178247;
UPDATE creature_template SET ScriptName='npc_ruul_snowhoof' WHERE entry=12818;
UPDATE creature_template SET ScriptName='npc_torek' WHERE entry=12858;
UPDATE creature_template SET ScriptName='npc_feero_ironhand' WHERE entry=4484;

/*  */
/* AUCHINDOUN */
/*  */

/* MANA TOMBS */
UPDATE creature_template SET ScriptName='boss_pandemonius' WHERE entry=18341;
UPDATE creature_template SET ScriptName='boss_nexusprince_shaffar' WHERE entry=18344;
UPDATE creature_template SET ScriptName='npc_shaheen' WHERE entry=19671;
UPDATE creature_template SET ScriptName='npc_ethereum_prisoner_dungeon' WHERE entry IN(22927);
UPDATE gameobject_template SET ScriptName='go_stasis_chamber_mana_tombs' WHERE entry IN(185519);
UPDATE gameobject_template SET ScriptName='go_stasis_chamber_shaffar' WHERE entry IN(185522);

/* AUCHENAI CRYPTS */
UPDATE creature_template SET ScriptName='boss_exarch_maladaar' WHERE entry=18373;
UPDATE creature_template SET ScriptName='mob_stolen_soul' WHERE entry=18441;
UPDATE creature_template SET ScriptName='boss_shirrak' WHERE entry=18371;

/* SETHEKK HALLS */
UPDATE instance_template SET ScriptName='instance_sethekk_halls' WHERE map=556;
UPDATE creature_template SET ScriptName='boss_talon_king_ikiss' WHERE entry=18473;
UPDATE creature_template SET ScriptName='boss_darkweaver_syth' WHERE entry=18472;
UPDATE creature_template SET ScriptName='boss_anzu' WHERE entry=23035;
INSERT INTO scripted_event_id VALUES
(14797,'event_spell_summon_raven_god');
UPDATE creature_template SET ScriptName='npc_anzu_bird_spirit' WHERE entry IN(23134,23135,23136);

/* SHADOW LABYRINTH */
UPDATE instance_template SET ScriptName='instance_shadow_labyrinth' WHERE map=555;
UPDATE creature_template SET ScriptName='boss_murmur' WHERE entry=18708;
UPDATE creature_template SET ScriptName='boss_grandmaster_vorpil' WHERE entry=18732;
UPDATE creature_template SET ScriptName='boss_blackheart_the_inciter' WHERE entry=18667;
UPDATE creature_template SET ScriptName='boss_ambassador_hellmaw' WHERE entry=18731;
UPDATE creature_template SET ScriptName='npc_void_traveler' WHERE entry=19226;
UPDATE creature_template SET ScriptName='dummy_blackheart_the_inciter' WHERE entry IN(19300,19301,19302,19303,19304);
UPDATE gameobject_template SET ScriptName='go_screaming_hall_door' WHERE entry=183295;

/*  */
/* AZJOL-NERUB */
/*  */

/* AHN'KAHET */
UPDATE creature_template SET ScriptName='boss_amanitar' WHERE entry=30258;
UPDATE creature_template SET ScriptName='npc_amanitar_mushroom' WHERE entry IN (30391,30435);
UPDATE creature_template SET ScriptName='boss_jedoga' WHERE entry=29310;
UPDATE creature_template SET ScriptName='npc_twilight_volunteer' WHERE entry=30385;
UPDATE creature_template SET ScriptName='boss_nadox' WHERE entry=29309;
UPDATE creature_template SET ScriptName='mob_ahnkahar_egg' WHERE entry IN (30172,30173);
UPDATE creature_template SET ScriptName='boss_taldaram' WHERE entry=29308;
UPDATE gameobject_template SET ScriptName='go_nerubian_device' WHERE entry IN (193093,193094);
UPDATE creature_template SET ScriptName='boss_volazj' WHERE entry=29311;
UPDATE instance_template SET ScriptName='instance_ahnkahet' WHERE map=619;

/* AZJOL-NERUB */
UPDATE creature_template SET ScriptName='boss_anubarak' WHERE entry=29120;
UPDATE creature_template SET ScriptName='npc_impale_target' WHERE entry=29184;
UPDATE creature_template SET ScriptName='boss_hadronox' WHERE entry=28921;
UPDATE creature_template SET ScriptName='boss_krikthir' WHERE entry=28684;
UPDATE instance_template SET ScriptName='instance_azjol-nerub' WHERE map=601;

/* AZSHARA */
UPDATE creature_template SET ScriptName='npc_rizzle_sprysprocket' WHERE entry=23002;
UPDATE creature_template SET ScriptName='npc_depth_charge' WHERE entry=23025;
UPDATE gameobject_template SET ScriptName='go_southfury_moonstone' WHERE entry=185566;
UPDATE creature_template SET ScriptName='mobs_spitelashes' WHERE entry IN (6190,6193,6194,6195,6196);
UPDATE creature_template SET ScriptName='npc_loramus_thalipedes' WHERE entry=7783;
UPDATE creature_template SET ScriptName='npc_felhound_tracker' WHERE entry=8668;
UPDATE creature_template SET ScriptName='boss_maws' WHERE entry=15571;
UPDATE gameobject_template SET ScriptName='go_lightning' WHERE entry=183356;
INSERT INTO scripted_event_id VALUES
(9542,'event_arcanite_buoy');

/* AZUREMYST ISLE */
UPDATE creature_template SET ScriptName='npc_draenei_survivor' WHERE entry=16483;
UPDATE creature_template SET ScriptName='npc_magwin' WHERE entry=17312;

/* BADLANDS */

/* BARRENS */
UPDATE creature_template SET ScriptName='npc_gilthares' WHERE entry=3465;
UPDATE creature_template SET ScriptName='npc_taskmaster_fizzule' WHERE entry=7233;
UPDATE creature_template SET ScriptName='npc_twiggy_flathead' WHERE entry=6248;
INSERT INTO scripted_areatrigger VALUES
(522,'at_twiggy_flathead');
UPDATE creature_template SET ScriptName='npc_wizzlecranks_shredder' WHERE entry=3439;
UPDATE creature_template SET ScriptName='npc_gallywix' WHERE entry=7288;
UPDATE creature_template SET ScriptName='npc_foreman_silixiz' WHERE entry=7287;
UPDATE creature_template SET ScriptName='npc_venture_co_lookout' WHERE entry=7307;
UPDATE creature_template SET ScriptName = 'npc_regthar_deathgate' WHERE entry = 3389;

/* BLACK TEMPLE */
UPDATE instance_template SET ScriptName='instance_black_temple' WHERE map=564;
UPDATE creature_template SET ScriptName='npc_akama_shade' WHERE entry=23191; -- Akama at Shade of Akama
UPDATE creature_template SET ScriptName='npc_akama_illidan' WHERE entry=23089; -- Akama at Illidan
UPDATE creature_template SET ScriptName='mob_illidari_council' WHERE entry=23426; -- Illidari Council controller mob
UPDATE creature_template SET ScriptName='mob_blood_elf_council_voice_trigger' WHERE entry=23499; -- Voice Trigger Mob (Controls Aggro + Enrage yells)
UPDATE creature_template SET ScriptName='boss_veras_darkshadow' WHERE entry=22952; -- Rogue of Illidari Council
UPDATE creature_template SET ScriptName='boss_teron_gorefiend' WHERE entry=22871; -- Teron Gorefiend
UPDATE creature_template SET ScriptName='npc_doom_blossom' WHERE entry=23123; -- Doom Blossom
UPDATE creature_template SET ScriptName='npc_shadow_construct' WHERE entry IN(23111); -- Shadowy Construct
INSERT INTO scripted_areatrigger VALUES (4665, 'at_teron_gorefiend');
UPDATE creature_template SET ScriptName='boss_supremus' WHERE entry=22898; -- Supremus
UPDATE creature_template SET ScriptName='boss_shade_of_akama' WHERE entry=22841; -- Shade of Akama
UPDATE creature_template SET ScriptName='npc_creature_generator' WHERE entry IN(23210);
UPDATE creature_template SET ScriptName='boss_reliquary_of_souls' WHERE entry=22856; -- Reliquary Controller Mob
UPDATE creature_template SET ScriptName='boss_essence_of_suffering' WHERE entry=23418; -- Essence Of Suffering
UPDATE creature_template SET ScriptName='boss_essence_of_desire' WHERE entry=23419; -- Essence of Desire
UPDATE creature_template SET ScriptName='boss_essence_of_anger' WHERE entry=23420; -- Essence of Anger
UPDATE creature_template SET ScriptName='npc_reliquary_LOS_aggro_trigger' WHERE entry IN(23502); -- Reliquary Aggro Trigger
UPDATE creature_template SET ScriptName='npc_reliquary_combat_trigger' WHERE entry IN(23417); -- Reliquary Combat Trigger
UPDATE creature_template SET ScriptName='boss_najentus' WHERE entry=22887; -- High Warlord Naj'entus
UPDATE creature_template SET ScriptName='boss_gurtogg_bloodboil' WHERE entry=22948; -- Gurtogg Bloodboil
UPDATE creature_template SET ScriptName='boss_mother_shahraz' WHERE entry=22947; -- Mother Shahraz
UPDATE creature_template SET ScriptName='boss_lady_malande' WHERE entry=22951; -- Priest <3 at Illidari Council
UPDATE creature_template SET ScriptName='boss_illidan_stormrage' WHERE entry=22917; -- Illidan The Betrayer!
UPDATE creature_template SET ScriptName='boss_high_nethermancer_zerevor' WHERE entry=22950; -- Mage at Illidari Council
UPDATE creature_template SET ScriptName='boss_gathios_the_shatterer' WHERE entry=22949; -- Paladin at Illidari Council
UPDATE creature_template SET ScriptName='boss_maiev_shadowsong' WHERE entry=23197; -- Maiev Shadowsong
UPDATE creature_template SET ScriptName='mob_flame_of_azzinoth' WHERE entry=22997; -- Flame of Azzinoth (Illidan Phase 2)
UPDATE creature_template SET ScriptName='mob_blade_of_azzinoth' WHERE entry=22996; -- Blade of Azzinoth (Illidan Phase 2)
UPDATE creature_template SET ScriptName='mob_cage_trap_trigger' WHERE entry=23304; -- Cage Trap mob in Illidan Phase 3/4 Normal
UPDATE creature_template SET ScriptName='mob_shadow_demon' WHERE entry=23375; -- Shadow Demon in Illidan Demon Form
UPDATE creature_template SET ScriptName='npc_parasitic_shadowfiend' WHERE entry=23498; -- Parasitic Shadowfiend
UPDATE gameobject_template SET ScriptName='go_cage_trap' WHERE entry IN(185916);
UPDATE creature_template SET ScriptName='mob_ashtongue_channeler' WHERE entry=23421; -- Ashtongue Channeler in Shade of AKama
UPDATE creature_template SET ScriptName='mob_ashtongue_sorcerer' WHERE entry=23215; -- Ashtongue Sorcerer in Shade of Akama
UPDATE creature_template SET ScriptName='npc_enslaved_soul' WHERE entry=23469;
UPDATE creature_template SET ScriptName='npc_greater_shadowfiend' WHERE entry=22929;
UPDATE creature_template SET ScriptName='npc_bonechewer_brawler' WHERE entry=23222;
UPDATE creature_template SET ScriptName='npc_bonechewer_combatant' WHERE entry=23239;

/* BLACKFATHOM DEPTHS */
UPDATE instance_template SET ScriptName='instance_blackfathom_deeps' WHERE map=48;
UPDATE gameobject_template SET ScriptName='go_fire_of_akumai' WHERE entry IN (21118,21119,21120,21121);
UPDATE gameobject_template SET ScriptName='go_fathom_stone' WHERE entry=177964;

/* BLACKROCK DEPTHS */
INSERT INTO scripted_areatrigger VALUES
(1526,'at_ring_of_law'),
(1786,'at_shadowforge_bridge');
UPDATE instance_template SET ScriptName='instance_blackrock_depths' WHERE map =230;
UPDATE creature_template SET ScriptName='boss_emperor_dagran_thaurissan' WHERE entry=9019;
UPDATE creature_template SET ScriptName='boss_moira_bronzebeard' WHERE entry=8929;
UPDATE creature_template SET ScriptName='boss_ambassador_flamelash' WHERE entry=9156;
UPDATE creature_template SET ScriptName='boss_doomrel' WHERE entry=9039;
UPDATE creature_template SET ScriptName='boss_general_angerforge' WHERE entry=9033;
UPDATE creature_template SET ScriptName='boss_high_interrogator_gerstahn' WHERE entry=9018;
UPDATE creature_template SET ScriptName='boss_coren_direbrew' WHERE entry=23872;
UPDATE creature_template SET ScriptName='npc_grimstone' WHERE entry=10096;
UPDATE creature_template SET ScriptName='npc_theldren_trigger' WHERE entry=16079;
UPDATE creature_template SET ScriptName='npc_phalanx' WHERE entry=9502;
UPDATE creature_template SET ScriptName='npc_mistress_nagmara' WHERE entry=9500;
UPDATE creature_template SET ScriptName='npc_rocknot' WHERE entry=9503;
UPDATE creature_template SET ScriptName='npc_marshal_windsor' WHERE entry=9023;
UPDATE creature_template SET ScriptName='npc_dughal_stormwing' WHERE entry=9022;
UPDATE creature_template SET ScriptName='npc_tobias_seecher' WHERE entry=9679;
UPDATE creature_template SET ScriptName='npc_hurley_blackbreath' WHERE entry=9537;
UPDATE creature_template SET ScriptName='boss_plugger_spazzring' WHERE entry=9499;
UPDATE creature_template SET ScriptName='npc_ironhand_guardian' WHERE entry=8982;
UPDATE gameobject_template SET ScriptName='go_shadowforge_brazier' WHERE entry IN (174744, 174745);
UPDATE gameobject_template SET ScriptName='go_relic_coffer_door' WHERE entry IN (174554, 174555, 174556, 174557, 174558, 174559, 174560, 174561, 174562, 174563, 174564, 174566);
UPDATE gameobject_template SET ScriptName='go_bar_beer_keg' WHERE entry=164911;
UPDATE gameobject_template SET ScriptName='go_bar_ale_mug' WHERE entry IN (165738,165739);

/* BLACKROCK SPIRE */
UPDATE instance_template SET ScriptName='instance_blackrock_spire' WHERE map=229;
UPDATE creature_template SET ScriptName='boss_overlord_wyrmthalak' WHERE entry=9568;
UPDATE creature_template SET ScriptName='boss_gyth' WHERE entry=10339;
UPDATE creature_template SET ScriptName='boss_pyroguard_emberseer' WHERE entry=9816;
UPDATE creature_template SET ScriptName='npc_rookery_hatcher' WHERE entry=10683;
INSERT INTO scripted_event_id VALUES
(4884,'event_spell_altar_emberseer');
UPDATE gameobject_template SET ScriptName='go_father_flame' WHERE entry=175245;

/* BLACKWING LAIR */
UPDATE instance_template SET ScriptName='instance_blackwing_lair' WHERE map=469;
UPDATE creature_template SET ScriptName='boss_razorgore' WHERE entry=12435;
UPDATE creature_template SET ScriptName='npc_blackwing_orb' WHERE entry=14449;
INSERT INTO scripted_event_id VALUES
(8302,'event_razorgore_possess');
UPDATE creature_template SET ScriptName='boss_vaelastrasz' WHERE entry=13020;
UPDATE gameobject_template SET ScriptName='go_suppression' WHERE entry=179784;
UPDATE creature_template SET ScriptName='boss_broodlord' WHERE entry=12017;
UPDATE creature_template SET ScriptName='boss_firemaw' WHERE entry=11983;
UPDATE creature_template SET ScriptName='boss_ebonroc' WHERE entry=14601;
UPDATE creature_template SET ScriptName='boss_flamegor' WHERE entry=11981;
UPDATE creature_template SET ScriptName='boss_chromaggus' WHERE entry=14020;
UPDATE creature_template SET ScriptName='boss_victor_nefarius' WHERE entry=10162;
UPDATE creature_template SET ScriptName='boss_nefarian' WHERE entry=11583;
INSERT INTO scripted_event_id VALUES
(8446,'event_weekly_chromatic_selection'),
(8447,'event_weekly_chromatic_selection'),
(8448,'event_weekly_chromatic_selection'),
(8449,'event_weekly_chromatic_selection'),
(8450,'event_weekly_chromatic_selection'),
(8451,'event_weekly_chromatic_selection'),
(8452,'event_weekly_chromatic_selection'),
(8453,'event_weekly_chromatic_selection'),
(8454,'event_weekly_chromatic_selection'),
(8455,'event_weekly_chromatic_selection'),
(8520,'event_weekly_chromatic_selection'),
(8521,'event_weekly_chromatic_selection'),
(8522,'event_weekly_chromatic_selection'),
(8523,'event_weekly_chromatic_selection'),
(8524,'event_weekly_chromatic_selection'),
(8525,'event_weekly_chromatic_selection'),
(8526,'event_weekly_chromatic_selection'),
(8527,'event_weekly_chromatic_selection'),
(8528,'event_weekly_chromatic_selection'),
(8529,'event_weekly_chromatic_selection');


/* BLADE'S EDGE MOUNTAINS */
UPDATE creature_template SET ScriptName='mobs_nether_drake' WHERE entry IN (20021,21817,21820,21821,21823);
UPDATE creature_template SET ScriptName='npc_daranelle' WHERE entry=21469;
UPDATE creature_template SET ScriptName='npc_bloodmaul_stout_trigger' WHERE entry=21241;
UPDATE creature_template SET ScriptName='npc_simon_game_bunny' WHERE entry IN(22923,23378);
UPDATE creature_template SET ScriptName='npc_light_orb_collector' WHERE entry IN (21926,22333);
UPDATE creature_template SET ScriptName='npc_bloodmaul_dire_wolf' WHERE entry=20058;
UPDATE creature_template SET ScriptName='npc_obelisk_trigger' WHERE entry=22422;
UPDATE creature_template SET ScriptName='npc_vimgol_visual_bunny' WHERE entry=23040;
UPDATE creature_template SET ScriptName='npc_vimgol_middle_bunny' WHERE entry=23081;
UPDATE creature_template SET ScriptName='npc_vimgol' WHERE entry=22911;
UPDATE creature_template SET ScriptName='npc_bird_spirit' WHERE entry=22023;
UPDATE creature_template SET ScriptName='npc_spirit_prisoner_of_bladespire' WHERE entry IN(22460);
UPDATE creature_template SET ScriptName='npc_deadsoul_orb' WHERE entry=20845;
UPDATE creature_template SET ScriptName='npc_evergrove_druid' WHERE entry=22423;
UPDATE creature_template SET ScriptName='npc_apexis_flayer' WHERE entry=22175;
INSERT INTO scripted_areatrigger VALUES
(4613,'mobs_grishna_arrakoa'),
(4615,'mobs_grishna_arrakoa'),
(4616,'mobs_grishna_arrakoa'),
(4617,'mobs_grishna_arrakoa');
UPDATE creature_template SET ScriptName='npc_soulgrinder' WHERE entry=23019;
UPDATE creature_template SET ScriptName='npc_mogdorg_the_wizened' WHERE entry=22941;
UPDATE creature_template SET ScriptName='npc_supplicant' WHERE entry IN(23052,23053);
INSERT INTO scripted_event_id VALUES(14739,'event_into_the_soulgrinder');
UPDATE creature_template SET ScriptName='npc_warp_gate' WHERE entry IN(22471,22472);
UPDATE creature_template SET ScriptName='npc_fel_cannon' WHERE entry=22443;
UPDATE gameobject_template SET ScriptName='go_fel_cannonball_stack_trap' WHERE entry IN(185862);
UPDATE creature_template SET ScriptName='npc_frequency_scanner' WHERE entry=21760;
UPDATE gameobject_template SET ScriptName='go_aura_generator_000' WHERE entry=184926;
UPDATE creature_template SET ScriptName='npc_skyguard_aether_tech' WHERE entry IN(23241);
UPDATE creature_template SET ScriptName='npc_skyguard_ranger' WHERE entry IN(23242);
UPDATE creature_template SET ScriptName='npc_skyguard_lieutenant' WHERE entry IN(23430);
UPDATE creature_template SET ScriptName='npc_aether_tech_vendor' WHERE entry IN(23243,23244,23245);
UPDATE creature_template SET ScriptName='npc_bashir_flesh_fiend' WHERE entry IN(23249);
UPDATE creature_template SET ScriptName='npc_disruptor_tower' WHERE entry IN(23250);
UPDATE creature_template SET ScriptName='npc_grand_collector' WHERE entry IN(23333);
UPDATE gameobject_template SET ScriptName='go_nether_drake_egg_trap' WHERE entry=184958;
UPDATE creature_template SET ScriptName='npc_doomguard_punisher' WHERE entry IN(23113);
UPDATE creature_template SET ScriptName='npc_shivan_assassin' WHERE entry IN(23220);
UPDATE creature_template SET ScriptName='npc_eye_of_shartuul' WHERE entry IN(23228);
UPDATE creature_template SET ScriptName='npc_dreadmaw' WHERE entry IN(23275);
UPDATE creature_template SET ScriptName='npc_shartuul' WHERE entry IN(23230);
UPDATE creature_template SET ScriptName='npc_stun_field' WHERE entry IN(23312);
INSERT INTO scripted_event_id VALUES
(14913,'event_doomguard_punisher'),
(14942,'event_shivan_assassin');

/* BLASTED LANDS */
UPDATE creature_template SET ScriptName='boss_kazzak' WHERE entry=12397;
UPDATE creature_template SET ScriptName='npc_agent_proudwell' WHERE entry=19942;
UPDATE gameobject_template SET ScriptName='go_infernaling_summoner_portal_hound' WHERE entry=183357;

/* BLOODMYST ISLE */
UPDATE creature_template SET ScriptName='mob_webbed_creature' WHERE entry=17680;
UPDATE creature_template SET ScriptName='npc_demolitionist_legoso' WHERE entry=17982;

/* BOREAN TUNDRA */
UPDATE creature_template SET ScriptName='npc_nesingwary_trapper' WHERE entry=25835;
UPDATE creature_template SET ScriptName='npc_oil_stained_wolf' WHERE entry=25791;
UPDATE creature_template SET ScriptName='npc_sinkhole_kill_credit' WHERE entry IN (26248,26249);
UPDATE creature_template SET ScriptName='npc_lurgglbr' WHERE entry=25208;
UPDATE creature_template SET ScriptName='npc_beryl_sorcerer' WHERE entry=25316;
UPDATE creature_template SET ScriptName='npc_captured_beryl_sorcerer' WHERE entry=25474;
UPDATE creature_template SET ScriptName='npc_nexus_drake_hatchling' WHERE entry=26127;
UPDATE creature_template SET ScriptName='npc_scourged_flamespitter' WHERE entry=25582;
UPDATE creature_template SET ScriptName='npc_bonker_togglevolt' WHERE entry=25589;
UPDATE creature_template SET ScriptName='npc_jenny' WHERE entry=25969;
UPDATE creature_template SET ScriptName='npc_seaforium_depth_charge' WHERE entry=25401;
UPDATE creature_template SET ScriptName='npc_mootoo_the_younger' WHERE entry=25504;
UPDATE creature_template SET ScriptName='npc_storm_totem' WHERE entry=26048;
UPDATE creature_template SET ScriptName='npc_proudhoof' WHERE entry=25335;
UPDATE creature_template SET ScriptName='npc_orphaned_mammoth_calf' WHERE entry=25861;
UPDATE gameobject_template SET ScriptName='go_mammoth_trap_borean' WHERE entry IN(188022,188024,188025,188026,188027,188028,188029,188030,188031,188032,188033,188034,188035,188036,188037,188038,188039,188040,188041,188042,188043,188044);

/* BURNING STEPPES */
UPDATE creature_template SET ScriptName='npc_grark_lorkrub' WHERE entry=9520;
UPDATE creature_template SET ScriptName='npc_klinfran' WHERE entry IN (14529,14534);

/*  */
/* CAVERNS OF TIME */
/*  */

/* MT. HYJAL */
UPDATE instance_template SET ScriptName='instance_hyjal' WHERE map=534;
UPDATE creature_template SET ScriptName='npc_tyrande_whisperwind' WHERE entry=17948;
UPDATE creature_template SET ScriptName='npc_thrall' WHERE entry=17852;
UPDATE creature_template SET ScriptName='npc_jaina_proudmoore' WHERE entry=17772;
UPDATE creature_template SET ScriptName='boss_archimonde' WHERE entry=17968;
UPDATE creature_template SET ScriptName='npc_doomfire_targeting' WHERE entry=18104;
UPDATE creature_template SET ScriptName='boss_azgalor' WHERE entry IN(17842);
UPDATE creature_template SET ScriptName='boss_kazrogal' WHERE entry IN(17888);
UPDATE creature_template SET ScriptName='boss_anetheron' WHERE entry IN(17808);
UPDATE creature_template SET ScriptName='boss_rage_winterchill' WHERE entry IN(17767);
UPDATE creature_template SET ScriptName='npc_building_trigger' WHERE entry=18304;

/* OLD HILLSBRAD */
UPDATE instance_template SET ScriptName='instance_old_hillsbrad' WHERE map=560;
UPDATE creature_template SET ScriptName='npc_erozion' WHERE entry=18723;
UPDATE creature_template SET ScriptName='npc_taretha' WHERE entry=18887;
UPDATE creature_template SET ScriptName='npc_thrall_old_hillsbrad' WHERE entry=17876;
UPDATE creature_template SET ScriptName='npc_bartolo_ginsetti' WHERE entry=20365;
INSERT INTO scripted_event_id VALUES
(11111,'event_go_barrel_old_hillsbrad');
INSERT INTO scripted_areatrigger VALUES
(4501,'at_bartolo_ginsetti'),
(4502,'at_beggar'),
(4503,'at_beggar'),
(4504,'at_beggar');

/* THE CULLING OF STRATHOLME */
UPDATE instance_template SET ScriptName='instance_culling_of_stratholme' WHERE map=595;
UPDATE creature_template SET ScriptName='npc_chromie' WHERE entry IN (26527, 27915);
UPDATE creature_template SET ScriptName='spell_dummy_npc_crates_bunny' WHERE entry=27827;
UPDATE creature_template SET ScriptName='npc_spell_dummy_crusader_strike' WHERE entry IN (28167,28169);
UPDATE creature_template SET ScriptName='npc_arthas' WHERE entry=26499;
INSERT INTO scripted_areatrigger VALUES
(5291,'at_culling_of_stratholme');

/* THE DARK PORTAL */
UPDATE creature_template SET ScriptName='boss_chrono_lord_deja' WHERE entry=17879;
UPDATE creature_template SET ScriptName='boss_aeonus' WHERE entry=17881;
UPDATE creature_template SET ScriptName='boss_temporus' WHERE entry=17880;
UPDATE instance_template SET ScriptName='instance_dark_portal' WHERE map=269;
UPDATE creature_template SET ScriptName='npc_medivh_black_morass' WHERE entry=15608;
UPDATE creature_template SET ScriptName='npc_time_rift' WHERE entry=17838;

/*  */
/* COILFANG RESERVOIR */
/*  */

/* THE SLAVE PENS */
UPDATE creature_template SET ScriptName='boss_ahune' WHERE entry=25740;
UPDATE creature_template SET ScriptName='npc_frozen_core' WHERE entry=25865;
UPDATE creature_template SET ScriptName='npc_ice_spear_bunny' WHERE entry=25985;
UPDATE instance_template SET ScriptName='instance_slave_pens' WHERE map=547;
INSERT INTO scripted_areatrigger VALUES
(4295,'at_naturalist_bite');
UPDATE creature_template SET ScriptName='npc_naturalist_bite' WHERE entry IN(17893); -- gossip only

/* THE UNDERBOG */
UPDATE creature_template SET ScriptName='mob_underbog_mushroom' WHERE entry=17990;
UPDATE creature_template SET ScriptName='boss_hungarfen' WHERE entry=17770;
UPDATE creature_template SET ScriptName='boss_swamplord_muselek' WHERE entry=17826;
UPDATE instance_template SET ScriptName='instance_underbog' WHERE map=546;
INSERT INTO scripted_areatrigger VALUES
(4302,'at_ghazan_surface');
UPDATE creature_template SET ScriptName='boss_black_stalker' WHERE entry=17882;

/* THE STEAMVAULT */
UPDATE instance_template SET ScriptName='instance_steam_vault' WHERE map=545;
UPDATE creature_template SET ScriptName='boss_hydromancer_thespia' WHERE entry=17797;
UPDATE creature_template SET ScriptName='boss_mekgineer_steamrigger' WHERE entry=17796;
UPDATE creature_template SET ScriptName='boss_warlord_kalithresh' WHERE entry=17798;
UPDATE gameobject_template SET ScriptName='go_main_chambers_access_panel' WHERE entry IN (184125,184126);
UPDATE creature_template SET ScriptName='mob_naga_distiller' WHERE entry=17954;
UPDATE creature_template SET ScriptName='mob_steamrigger_mechanic' WHERE entry=17951;

/* SERPENTSHRINE CAVERN */
UPDATE instance_template SET ScriptName='instance_serpent_shrine' WHERE map=548;
UPDATE gameobject_template SET ScriptName='go_ssc_boss_consoles' WHERE entry IN(185117,185118,185115,185114,185116);
UPDATE creature_template SET ScriptName='npc_serpentshrine_parasite' WHERE entry=22379;
/* Hydross the Unstable event */
UPDATE creature_template SET ScriptName='boss_hydross_the_unstable' WHERE entry=21216;
UPDATE creature_template SET ScriptName='npc_spawn_of_hydross' WHERE entry IN(22035,22036);
/* Leotheras the Blind event */
UPDATE creature_template SET ScriptName='boss_leotheras_the_blind' WHERE entry=21215;
UPDATE creature_template SET ScriptName='npc_inner_demon' WHERE entry=21857;
/* Fathom-lord Karathress event */
UPDATE creature_template SET ScriptName='boss_fathomlord_karathress' WHERE entry=21214;
UPDATE creature_template SET ScriptName='boss_fathomguard_sharkkis' WHERE entry=21966;
UPDATE creature_template SET ScriptName='boss_fathomguard_tidalvess' WHERE entry=21965;
UPDATE creature_template SET ScriptName='boss_fathomguard_caribdis' WHERE entry=21964;
UPDATE creature_template SET ScriptName='npc_caribdis_cyclone' WHERE entry=22104;
/* Morogrim Tidewalker event */
UPDATE creature_template SET ScriptName='boss_morogrim_tidewalker' WHERE entry=21213;
UPDATE creature_template SET ScriptName='mob_water_globule' WHERE entry=21913;
/* Lady Vashj event */
UPDATE creature_template SET ScriptName='boss_lady_vashj' WHERE entry=21212;
UPDATE creature_template SET ScriptName='mob_enchanted_elemental' WHERE entry=21958;
UPDATE gameobject_template SET ScriptName='go_shield_generator' WHERE entry IN (185051,185052,185053,185054);
UPDATE item_template SET ScriptName='item_tainted_core' WHERE entry=31088;
UPDATE gameobject_template SET ScriptName='go_vashj_bridge' WHERE entry IN(184568);
/* The Lurker Below event */
UPDATE gameobject_template SET ScriptName='go_strange_pool' WHERE entry=184956;
UPDATE creature_template SET ScriptName='boss_the_lurker_below' WHERE entry=21217;

/* CRYSTALSONG FOREST */

/*  */
/* CRUSADER COLISEUM */
/*  */

/* TRAIL OF THE CHAMPION */
UPDATE instance_template SET ScriptName='instance_trial_of_the_champion' WHERE map=650;
UPDATE creature_template SET ScriptName='npc_toc_herald' WHERE entry IN (35004, 35005);
UPDATE creature_template SET ScriptName='boss_champion_warrior' WHERE entry IN (34705,35572);
UPDATE creature_template SET ScriptName='boss_champion_mage' WHERE entry IN (34702,35569);
UPDATE creature_template SET ScriptName='boss_champion_shaman' WHERE entry IN (34701,35571);
UPDATE creature_template SET ScriptName='boss_champion_hunter' WHERE entry IN (34657,35570);
UPDATE creature_template SET ScriptName='boss_champion_rogue' WHERE entry IN (34703,35617);
UPDATE creature_template SET ScriptName='npc_champion_mount' WHERE entry IN (35644,36559,35637,35633,35768,34658,35636,35638,35635,35640,35641,35634);
UPDATE creature_template SET ScriptName='npc_trial_grand_champion' WHERE entry IN (35328,35331,35330,35332,35329,35314,35326,35325,35323,35327);
UPDATE creature_template SET ScriptName='boss_eadric' WHERE entry=35119;
UPDATE creature_template SET ScriptName='boss_paletress' WHERE entry=34928;
UPDATE creature_template SET ScriptName='boss_black_knight' WHERE entry=35451;
UPDATE creature_template SET ScriptName='npc_black_knight_gryphon' WHERE entry=35491;
UPDATE creature_template SET ScriptName='npc_black_knight_ghoul' WHERE entry IN (35545,35564,35590);

/* TRIAL OF THE CRUSADER */
UPDATE instance_template SET ScriptName='instance_trial_of_the_crusader' WHERE map=649;
UPDATE creature_template SET ScriptName='npc_barrett_ramsey' WHERE entry IN (34816, 35035, 35766, 35770, 35771);
UPDATE creature_template SET ScriptName='npc_beast_combat_stalker' WHERE entry=36549;
UPDATE creature_template SET ScriptName='boss_gormok' WHERE entry=34796;
UPDATE creature_template SET ScriptName='boss_acidmaw' WHERE entry=35144;
UPDATE creature_template SET ScriptName='boss_dreadscale' WHERE entry=34799;
UPDATE creature_template SET ScriptName='boss_icehowl' WHERE entry=34797;
UPDATE creature_template SET ScriptName='boss_jaraxxus' WHERE entry=34780;
UPDATE creature_template SET ScriptName='boss_anubarak_trial' WHERE entry=34564;
UPDATE creature_template SET ScriptName='boss_fjola' WHERE entry=34497;
UPDATE creature_template SET ScriptName='boss_eydis' WHERE entry=34496;
UPDATE creature_template SET ScriptName='npc_concentrated_bullet' WHERE entry IN (34628,34630);
UPDATE creature_template SET ScriptName='npc_anubarak_spike' WHERE entry=34660;
UPDATE creature_template SET ScriptName='npc_frost_sphere' WHERE entry=34606;
UPDATE creature_template SET ScriptName='npc_nerubian_burrower' WHERE entry=34607;
UPDATE creature_template SET ScriptName='boss_crusader_death_knight' WHERE entry IN (34461,34458);
UPDATE creature_template SET ScriptName='boss_crusader_druid_balance' WHERE entry IN (34460,34451);
UPDATE creature_template SET ScriptName='boss_crusader_druid_resto' WHERE entry IN (34469,34459);
UPDATE creature_template SET ScriptName='boss_crusader_hunter' WHERE entry IN (34467,34448);
UPDATE creature_template SET ScriptName='boss_crusader_mage' WHERE entry IN (34468,34449);
UPDATE creature_template SET ScriptName='boss_crusader_paladin_holy' WHERE entry IN (34465,34445);
UPDATE creature_template SET ScriptName='boss_crusader_paladin_retri' WHERE entry IN (34471,34456);
UPDATE creature_template SET ScriptName='boss_crusader_priest_disc' WHERE entry IN (34466,34447);
UPDATE creature_template SET ScriptName='boss_crusader_priest_shadow' WHERE entry IN (34473,34441);
UPDATE creature_template SET ScriptName='boss_crusader_rogue' WHERE entry IN (34472,34454);
UPDATE creature_template SET ScriptName='boss_crusader_shaman_enha' WHERE entry IN (34463,34455);
UPDATE creature_template SET ScriptName='boss_crusader_shaman_resto' WHERE entry IN (34470,34444);
UPDATE creature_template SET ScriptName='boss_crusader_warlock' WHERE entry IN (34474,34450);
UPDATE creature_template SET ScriptName='boss_crusader_warrior' WHERE entry IN (34475,34453);
UPDATE gameobject_template SET ScriptName='go_coliseum_floor' WHERE entry IN(195527);

/* DALARAN */
UPDATE creature_template SET ScriptName='npc_dalaran_guardian_mage' WHERE entry IN (29255, 29254);

/* DARKSHORE */
UPDATE creature_template SET ScriptName='npc_kerlonian' WHERE entry=11218;
UPDATE creature_template SET ScriptName='npc_prospector_remtravel' WHERE entry=2917;
UPDATE creature_template SET ScriptName='npc_threshwackonator' WHERE entry=6669;
UPDATE creature_template SET ScriptName='npc_volcor' WHERE entry=3692;
UPDATE creature_template SET ScriptName='npc_therylune' WHERE entry=3584;
UPDATE creature_template SET ScriptName='npc_rabid_bear' WHERE entry=2164;
UPDATE creature_template SET ScriptName='npc_blackwood_warrior' WHERE entry=2168;
UPDATE creature_template SET ScriptName='npc_blackwood_totemic' WHERE entry=2169;
UPDATE gameobject_template SET ScriptName='go_furbolg_food' WHERE entry=175329;
UPDATE gameobject_template SET ScriptName='go_furbolg_food' WHERE entry=175330;
UPDATE gameobject_template SET ScriptName='go_furbolg_food' WHERE entry=175331;
INSERT INTO scripted_event_id VALUES
(3938,'event_purify_food');

/* DARNASSUS */

/* DEADMINES */
UPDATE creature_template SET ScriptName='boss_mr_smite' WHERE entry=646;
UPDATE instance_template SET ScriptName='instance_deadmines' WHERE map=36;
UPDATE gameobject_template SET ScriptName='go_defias_cannon' WHERE entry=16398;

/* DEADWIND PASS */

/* DESOLACE */
UPDATE creature_template SET ScriptName='npc_aged_dying_ancient_kodo' WHERE entry IN (4700, 4701, 4702, 11627);
UPDATE creature_template SET ScriptName='npc_dalinda_malem' WHERE entry=5644;
UPDATE creature_template SET ScriptName='npc_melizza_brimbuzzle' WHERE entry=12277;
UPDATE creature_template SET ScriptName='npc_cork_gizelton' WHERE entry=11625;
UPDATE creature_template SET ScriptName='npc_rigger_gizelton' WHERE entry=11626;
UPDATE creature_template SET ScriptName='npc_magrami_spectre' WHERE entry=11560;

/* DIRE MAUL */
UPDATE instance_template SET ScriptName='instance_dire_maul' WHERE map=429;
UPDATE gameobject_template SET ScriptName='go_fixed_trap' WHERE entry=179512;
UPDATE creature_template SET ScriptName='npc_mizzle_crafty' WHERE entry=14353;
UPDATE creature_template SET ScriptName='npc_warlock_mount_ritual' WHERE entry=14501;
UPDATE gameobject_template SET ScriptName='go_dreadsteed_quest_objects' WHERE entry IN(179672,179673,179674);
INSERT INTO scripted_event_id VALUES
(8420,'event_spells_warlock_dreadsteed'),
(8428,'event_spells_warlock_dreadsteed');

/* DRAGONBLIGHT */
UPDATE creature_template SET ScriptName='npc_destructive_ward' WHERE entry=27430;
UPDATE creature_template SET ScriptName='npc_crystalline_ice_giant' WHERE entry=26809;
UPDATE gameobject_template SET ScriptName='go_scrying_orb' WHERE entry=189292;
UPDATE gameobject_template SET ScriptName='go_portal_to_orgrimmar' WHERE entry=193948;
UPDATE gameobject_template SET ScriptName='go_portal_to_undercity' WHERE entry=193955;

/* DRAK'THARON KEEP */
UPDATE creature_template SET ScriptName='boss_novos' WHERE entry=26631;
UPDATE creature_template SET ScriptName='npc_crystal_channel_target' WHERE entry=26712;
UPDATE creature_template SET ScriptName='boss_tharonja' WHERE entry=26632;
UPDATE creature_template SET ScriptName='boss_trollgore' WHERE entry=26630;
UPDATE instance_template SET ScriptName='instance_draktharon_keep' WHERE map=600;

/* DUN MOROGH */

/* DUROTAR */
UPDATE creature_template SET ScriptName='npc_lazy_peon' WHERE entry=10556;

/* DUSKWOOD */

/* DUSTWALLOW MARSH */
UPDATE creature_template SET ScriptName='mobs_risen_husk_spirit' WHERE entry IN (23554,23555);
UPDATE creature_template SET ScriptName='npc_ogron' WHERE entry=4983;
UPDATE creature_template SET ScriptName='npc_morokk' WHERE entry=4500;
UPDATE creature_template SET ScriptName='npc_restless_apparition' WHERE entry=23861;
UPDATE creature_template SET ScriptName='npc_private_hendel' WHERE entry=4966;
UPDATE creature_template SET ScriptName='npc_stinky_ignatz' WHERE entry=4880;
UPDATE creature_template SET ScriptName='boss_tethyr' WHERE entry=23899;
UPDATE creature_template SET ScriptName='npc_major_mills' WHERE entry=23905;
UPDATE creature_template SET ScriptName='mob_invis_firework_helper' WHERE entry=24025;
UPDATE creature_template SET ScriptName='npc_smolderwing' WHERE entry=23789;
UPDATE creature_template SET ScriptName='npc_theramore_spar_controller' WHERE entry=5090;

INSERT INTO scripted_areatrigger VALUES
(302,'at_sentry_point');
INSERT INTO scripted_areatrigger VALUES
(4752,'at_nats_landing');

/* EASTERN PLAGUELANDS */
UPDATE creature_template SET ScriptName='npc_eris_havenfire' WHERE entry=14494;

/* EBON HOLD */
UPDATE creature_template SET ScriptName='npc_death_knight_initiate' WHERE entry=28406;
UPDATE creature_template SET ScriptName='npc_a_special_surprise' WHERE entry IN (29032,29061,29065,29067,29068,29070,29074,29072,29073,29071);
UPDATE creature_template SET ScriptName='npc_eye_of_acherus' WHERE entry=28511;
UPDATE creature_template SET ScriptName='npc_scarlet_ghoul' WHERE entry=28845;
UPDATE creature_template SET ScriptName='npc_highlord_darion_mograine' WHERE entry=29173;
UPDATE creature_template SET ScriptName='npc_scarlet_courier' WHERE entry=29076;
UPDATE gameobject_template SET ScriptName='go_plague_cauldron' WHERE entry = 190936;

/* ELWYNN FOREST */

/* EVERSONG WOODS */
UPDATE creature_template SET ScriptName='npc_kelerun_bloodmourn' WHERE entry=17807;
UPDATE gameobject_template SET ScriptName='go_harbinger_second_trial' WHERE entry=182052;
UPDATE creature_template SET ScriptName='npc_prospector_anvilward' WHERE entry=15420;
UPDATE creature_template SET ScriptName='npc_apprentice_mirveda' WHERE entry=15402;
UPDATE creature_template SET ScriptName='npc_infused_crystal' WHERE entry=16364;

/* FELWOOD */
INSERT INTO scripted_event_id VALUES
(8328,'npc_kroshius');
UPDATE creature_template SET ScriptName='npc_kitten' WHERE entry=9937;
UPDATE creature_template SET ScriptName='npc_corrupt_saber' WHERE entry=10042;
UPDATE creature_template SET ScriptName='npc_niby_the_almighty' WHERE entry=14469;
UPDATE creature_template SET ScriptName='npc_kroshius' WHERE entry=14467;
UPDATE creature_template SET ScriptName='npc_captured_arkonarin' WHERE entry=11016;
UPDATE creature_template SET ScriptName='npc_arei' WHERE entry=9598;

/* FERALAS */
UPDATE creature_template SET ScriptName='npc_oox22fe' WHERE entry=7807;
UPDATE creature_template SET ScriptName='npc_shay_leafrunner' WHERE entry=7774;
UPDATE creature_template SET ScriptName='npc_captured_sprite_darter' WHERE entry=7997;
UPDATE creature_template SET ScriptName='npc_kindal_moonweaver' WHERE entry=7956;
UPDATE gameobject_template SET ScriptName='go_cage_door' WHERE entry=143979;

/* GHOSTLANDS */
UPDATE creature_template SET ScriptName='npc_ranger_lilatha' WHERE entry=16295;

/* GNOMEREGAN */
UPDATE creature_template SET ScriptName='boss_thermaplugg' WHERE entry=7800;
UPDATE gameobject_template SET ScriptName='go_gnomeface_button' WHERE entry BETWEEN 142214 AND 142219;
UPDATE creature_template SET ScriptName='npc_blastmaster_emi_shortfuse' WHERE entry=7998;
UPDATE creature_template SET ScriptName='npc_kernobee' WHERE entry=7850;
UPDATE instance_template SET ScriptName='instance_gnomeregan' WHERE map=90;

/* GRIZZLY HILLS */
UPDATE creature_template SET ScriptName='npc_depleted_war_golem' WHERE entry=27017;
UPDATE creature_template SET ScriptName='npc_harrison_jones' WHERE entry=26814;
UPDATE creature_template SET ScriptName='npc_emily' WHERE entry=26588;

/* GRUUL'S LAIR */
UPDATE instance_template SET ScriptName='instance_gruuls_lair' WHERE map =565;
UPDATE creature_template SET ScriptName='boss_gruul' WHERE entry=19044;
/* Maulgar and Event */
UPDATE creature_template SET ScriptName='boss_high_king_maulgar' WHERE entry=18831;
UPDATE creature_template SET ScriptName='boss_kiggler_the_crazed' WHERE entry=18835;
UPDATE creature_template SET ScriptName='boss_blindeye_the_seer' WHERE entry=18836;
UPDATE creature_template SET ScriptName='boss_olm_the_summoner' WHERE entry=18834;
UPDATE creature_template SET ScriptName='boss_krosh_firehand' WHERE entry=18832;

/* GUNDRAK */
UPDATE creature_template SET ScriptName='boss_drakkari_colossus' WHERE entry=29307;
UPDATE creature_template SET ScriptName='boss_drakkari_elemental' WHERE entry=29573;
UPDATE creature_template SET ScriptName='npc_living_mojo' WHERE entry=29830;
UPDATE creature_template SET ScriptName='boss_galdarah' WHERE entry=29306;
UPDATE creature_template SET ScriptName='boss_moorabi' WHERE entry=29305;
UPDATE creature_template SET ScriptName='boss_sladran' WHERE entry=29304;
UPDATE gameobject_template SET ScriptName='go_gundrak_altar' WHERE entry IN (192518, 192519, 192520);
UPDATE instance_template SET ScriptName='instance_gundrak' WHERE map=604;

/*  */
/* HELLFIRE CITADEL */
/*  */

/* BLOOD FURNACE */
/* The Maker,Broggok,Kelidan,Broggok's cloud */
UPDATE creature_template SET ScriptName='boss_the_maker' WHERE entry=17381;
UPDATE creature_template SET ScriptName='boss_broggok' WHERE entry=17380;
UPDATE creature_template SET ScriptName='boss_kelidan_the_breaker' WHERE entry=17377;
UPDATE creature_template SET ScriptName='mob_shadowmoon_channeler' WHERE entry=17653;
UPDATE gameobject_template SET ScriptName='go_prison_cell_lever' WHERE entry=181982;
UPDATE instance_template SET ScriptName='instance_blood_furnace' WHERE map=542;

/* HELLFIRE RAMPARTS */
/* Vazruden,Omor the Unscarred,Watchkeeper Gargolmar */
UPDATE creature_template SET ScriptName='boss_omor_the_unscarred' WHERE entry=17308;
UPDATE creature_template SET ScriptName='boss_watchkeeper_gargolmar' WHERE entry=17306;
UPDATE creature_template SET ScriptName='boss_vazruden_herald' WHERE entry=17307;
UPDATE creature_template SET ScriptName='boss_vazruden' WHERE entry=17537;
UPDATE instance_template SET ScriptName='instance_ramparts' WHERE map=543;

/* SHATTERED HALLS */
/* Nethekurse and his spawned shadowfissure */
UPDATE creature_template SET ScriptName='npc_shattered_hand_legionnaire' WHERE Entry=16700;
UPDATE creature_template SET ScriptName='boss_grand_warlock_nethekurse' WHERE entry=16807;
UPDATE creature_template SET ScriptName='boss_warbringer_omrogg' WHERE entry=16809;
UPDATE creature_template SET ScriptName='mob_fel_orc_convert' WHERE entry=17083;
UPDATE creature_template SET ScriptName='mob_omrogg_heads' WHERE entry IN (19523,19524);
UPDATE creature_template SET ScriptName='boss_warchief_kargath_bladefist' WHERE entry=16808;
UPDATE creature_template SET ScriptName='npc_blade_dance_target' WHERE entry IN(20709);
UPDATE creature_template SET ScriptName='npc_shattered_hand_zealot' WHERE entry=17462;
UPDATE creature_template SET ScriptName='npc_shattered_hand_scout' WHERE entry=17693;
UPDATE instance_template SET ScriptName='instance_shattered_halls' WHERE map=540;

/* MAGTHERIDON'S LAIR */
UPDATE instance_template SET ScriptName='instance_magtheridons_lair' WHERE map=544;
UPDATE gameobject_template SET ScriptName='go_manticron_cube' WHERE entry=181713;
UPDATE creature_template SET ScriptName='boss_magtheridon' WHERE entry=17257;
UPDATE creature_template SET ScriptName='mob_hellfire_channeler' WHERE entry=17256;

/* HELLFIRE PENINSULA */
UPDATE creature_template SET ScriptName='boss_doomlord_kazzak' WHERE entry=18728;
UPDATE creature_template SET ScriptName='npc_aeranas' WHERE entry=17085;
UPDATE creature_template SET ScriptName='npc_ancestral_wolf' WHERE entry=17077;
UPDATE creature_template SET ScriptName='npc_demoniac_scryer' WHERE entry=22258;
UPDATE creature_template SET ScriptName='npc_wounded_blood_elf' WHERE entry=16993;
UPDATE creature_template SET ScriptName='npc_fel_guard_hound' WHERE entry=21847;
UPDATE creature_template SET ScriptName='npc_anchorite_barada' WHERE entry=22431;
UPDATE creature_template SET ScriptName='npc_colonel_jules' WHERE entry=22432;
UPDATE creature_template SET ScriptName='npc_magister_aledis' WHERE entry=20159;
UPDATE creature_template SET ScriptName='npc_living_flare' WHERE entry=24916;
UPDATE creature_template SET ScriptName='npc_danath_trollbane' WHERE `entry`='16819';
UPDATE creature_template SET ScriptName='npc_nazgrel' WHERE `entry`='3230';
UPDATE creature_template SET ScriptName='npc_vindicator_sedai' WHERE entry=17404;
UPDATE creature_template SET ScriptName='npc_krun' WHERE entry=17405;
UPDATE creature_template SET ScriptName='npc_laughing_skull' WHERE entry=17418;
UPDATE creature_template SET ScriptName='npc_maghar_escort' WHERE entry=17417;
DELETE FROM scripted_event_id WHERE id=10745;
INSERT INTO scripted_event_id VALUES(10745,'npc_vindicator_sedai');
UPDATE gameobject_template SET ScriptName='go_kaliri_nest' WHERE entry IN(181582);
UPDATE gameobject_template SET ScriptName='go_smoke_beacon' WHERE entry=184661;
UPDATE creature_template SET ScriptName='npc_credit_marker_they_must_burn' WHERE entry=21173;
UPDATE creature_template SET ScriptName='npc_razorthorn_ravager' WHERE entry=24922;
UPDATE creature_template SET ScriptName='npc_living_flare' WHERE entry=24916;

/* HILLSBRAD FOOTHILLS */

/* HINTERLANDS */
UPDATE creature_template SET ScriptName='npc_00x09hl' WHERE entry=7806;
UPDATE creature_template SET ScriptName='npc_rinji' WHERE entry=7780;
INSERT INTO scripted_event_id VALUES
(8608,'event_wildhammer_message');

/* HOWLING FJORD */
INSERT INTO scripted_areatrigger VALUES
(4778,'at_ancient_male_vrykul'),
(4779,'at_nifflevar');
UPDATE creature_template SET ScriptName='npc_ancient_male_vrykul' WHERE entry=24314;
UPDATE creature_template SET ScriptName='npc_silvermoon_harry' WHERE entry=24539;
UPDATE creature_template SET ScriptName='npc_lich_king_village' WHERE entry=24248;
UPDATE creature_template SET ScriptName='npc_king_ymiron' WHERE entry=24321;
UPDATE creature_template SET ScriptName='npc_firecrackers_bunny' WHERE entry=24230;
UPDATE creature_template SET ScriptName='npc_apothecary_hanes' WHERE entry=23784;
UPDATE creature_template SET ScriptName='npc_scalawag_frog' WHERE entry=26503;

/*  */
/* ICECROWN CITADEL */
/*  */

/* ICECROWN CITADEL */
UPDATE instance_template SET ScriptName='instance_icecrown_citadel' WHERE map=631;
UPDATE creature_template SET ScriptName='boss_lord_marrowgar' WHERE entry=36612;
UPDATE creature_template SET ScriptName='npc_bone_spike' WHERE entry IN (36619,38711,38712);
UPDATE creature_template SET ScriptName='boss_lady_deathwhisper' WHERE entry=36855;
UPDATE creature_template SET ScriptName='boss_deathbringer_saurfang' WHERE entry=37813;
UPDATE creature_template SET ScriptName='npc_queen_lanathel_intro' WHERE entry=38004;
UPDATE creature_template SET ScriptName='npc_blood_orb_control' WHERE entry=38008;
UPDATE creature_template SET ScriptName='npc_ball_of_flame' WHERE entry IN (38332,38451);
UPDATE creature_template SET ScriptName='npc_kinetic_bomb' WHERE entry=38454;
UPDATE creature_template SET ScriptName='npc_dark_nucleus' WHERE entry=38369;
UPDATE creature_template SET ScriptName='boss_taldaram_icc' WHERE entry=37973;
UPDATE creature_template SET ScriptName='boss_keleseth_icc' WHERE entry=37972;
UPDATE creature_template SET ScriptName='boss_valanar_icc' WHERE entry=37970;
UPDATE creature_template SET ScriptName='boss_blood_queen_lanathel' WHERE entry=37955;
UPDATE creature_template SET ScriptName='boss_sindragosa' WHERE entry=36853;
UPDATE creature_template SET ScriptName='npc_rimefang_icc' WHERE entry=37533;
UPDATE creature_template SET ScriptName='npc_spinestalker_icc' WHERE entry=37534;
UPDATE creature_template SET ScriptName='mob_frost_bomb' WHERE entry=37186;
UPDATE creature_template SET ScriptName='boss_festergut' WHERE entry=36626;
UPDATE creature_template SET ScriptName='npc_puddle_stalker' WHERE entry=37013;
UPDATE creature_template SET ScriptName='npc_orange_gas_stalker' WHERE entry=36659;
UPDATE creature_template SET ScriptName='boss_rotface' WHERE entry=36627;
UPDATE creature_template SET ScriptName='mob_little_ooze' WHERE entry=36897;
UPDATE creature_template SET ScriptName='mob_big_ooze' WHERE entry=36899;
UPDATE creature_template SET ScriptName='boss_valithria_dreamwalker' WHERE entry=36789;
UPDATE creature_template SET ScriptName='boss_professor_putricide' WHERE entry=36678;
UPDATE creature_template SET ScriptName='npc_putricides_trap' WHERE entry=38879;
UPDATE creature_template SET ScriptName='npc_volatile_ooze_icc' WHERE entry=37697;
UPDATE creature_template SET ScriptName='npc_gas_cloud_icc' WHERE entry=37562;
UPDATE creature_template SET ScriptName='npc_growing_ooze_puddle' WHERE entry=37690;
UPDATE creature_template SET ScriptName='npc_choking_gas_bomb' WHERE entry=38159;
UPDATE creature_template SET ScriptName='boss_the_lich_king_icc' WHERE entry=36597;
UPDATE creature_template SET ScriptName='npc_saurfang_gunship' WHERE entry=36939;
UPDATE creature_template SET ScriptName='npc_muradin_gunship' WHERE entry=36948;
UPDATE gameobject_template SET ScriptName='go_icc_teleporter' WHERE entry IN (202235,202242,202243,202244,202245,202246);
INSERT INTO scripted_event_id VALUES
(23426,'event_gameobject_citadel_valve'),
(23438,'event_gameobject_citadel_valve');
INSERT INTO scripted_areatrigger VALUES
(5604,'at_icecrown_citadel'),
(5611,'at_lights_hammer'),
(5612,'at_lights_hammer'),
(5628,'at_rampart_skull'),
(5630,'at_rampart_skull'),
(5647,'at_putricides_trap'),
(5709,'at_icecrown_citadel'),
(5718,'at_frozen_throne_tele'),
(5732,'at_icecrown_citadel');
UPDATE gameobject_template SET ScriptName='go_lady_deathwhisper_elevator' WHERE entry IN(202220);

/* FORGE OF SOULS */
UPDATE creature_template SET ScriptName='boss_bronjahm' WHERE entry=36497;
UPDATE creature_template SET ScriptName='npc_corrupted_soul_fragment' WHERE entry=36535;
UPDATE creature_template SET ScriptName='boss_devourer_of_souls' WHERE entry=36502;
UPDATE instance_template SET ScriptName='instance_forge_of_souls' WHERE map=632;

/* HALLS OF REFLECTION */
UPDATE instance_template SET ScriptName='instance_halls_of_reflection' WHERE map=668;
UPDATE creature_template SET ScriptName='npc_spell_aura_dummy_frostmourne_equip' WHERE entry=37226;
UPDATE creature_template SET ScriptName='npc_spell_summon_reflections' WHERE entry=36723;
UPDATE creature_template SET ScriptName='boss_lich_king_hor' WHERE entry=36954;
UPDATE creature_template SET ScriptName='npc_jaina_sylvanas_hor' WHERE entry IN (36955,37554);
INSERT INTO scripted_areatrigger VALUES
(5697,'at_frostmourne_chamber'),
(5740,'at_frostworn_general'),
(5605,'at_wrath_lich_king'),
(5660,'at_queldelar_start');

/* PIT OF SARON */
UPDATE instance_template SET ScriptName='instance_pit_of_saron' WHERE map=658;
UPDATE creature_template SET ScriptName='boss_forgemaster_garfrost' WHERE entry=36494;
UPDATE creature_template SET ScriptName='boss_krick' WHERE entry=36477;
UPDATE creature_template SET ScriptName='boss_ick' WHERE entry=36476;
UPDATE creature_template SET ScriptName='npc_collapsing_icicle' WHERE entry=36847;
UPDATE creature_template SET ScriptName='boss_tyrannus' WHERE entry=36658;
UPDATE creature_template SET ScriptName='boss_rimefang_pos' WHERE entry=36661;
INSERT INTO scripted_areatrigger VALUES
(5578,'at_pit_of_saron'),
(5581,'at_pit_of_saron');

/* ICECROWN */
UPDATE creature_template SET ScriptName='npc_squad_leader' WHERE entry IN (31737,31833);
UPDATE creature_template SET ScriptName='npc_infantry' WHERE entry IN (31701,31832);
UPDATE creature_template SET ScriptName='npc_father_kamaros' WHERE entry IN (31279,32800);
UPDATE creature_template SET ScriptName='npc_saronite_mine_slave' WHERE entry=31397;
UPDATE creature_template SET ScriptName='npc_grand_admiral_westwind' WHERE entry=29621;

/* IRONFORGE */

/* ISLE OF QUEL'DANAS */
UPDATE creature_template SET ScriptName='npc_dawnblade_blood_knight' WHERE entry=24976;
UPDATE creature_template SET ScriptName='npc_converted_sentry' WHERE entry=24981;
UPDATE creature_template SET ScriptName='npc_suns_reach_reclamation' WHERE entry IN(24965,24967,25061,25057,24932,25108,25069,25046,24975,25112,25163,25169);
UPDATE creature_template SET ScriptName='npc_shattered_sun_marksman' WHERE entry=24938;
UPDATE creature_template SET ScriptName='npc_shattered_sun_warrior' WHERE entry=25115;

/* KARAZHAN */
UPDATE instance_template SET ScriptName='instance_karazhan' WHERE map=532;
UPDATE creature_template SET ScriptName='boss_midnight' WHERE entry=16151;
UPDATE creature_template SET ScriptName='boss_attumen' WHERE entry IN (15550,16152);
UPDATE creature_template SET ScriptName='boss_moroes' WHERE entry=15687;
UPDATE creature_template SET ScriptName='boss_maiden_of_virtue' WHERE entry=16457;
UPDATE creature_template SET ScriptName='boss_curator' WHERE entry=15691;
UPDATE creature_template SET ScriptName='boss_julianne' WHERE entry=17534;
UPDATE creature_template SET ScriptName='boss_romulo' WHERE entry=17533;
UPDATE creature_template SET ScriptName='boss_dorothee' WHERE entry=17535;
UPDATE creature_template SET ScriptName='boss_strawman' WHERE entry=17543;
UPDATE creature_template SET ScriptName='boss_tinhead' WHERE entry=17547;
UPDATE creature_template SET ScriptName='boss_roar' WHERE entry=17546;
UPDATE creature_template SET ScriptName='boss_crone' WHERE entry=18168;
UPDATE creature_template SET ScriptName='boss_terestian_illhoof' WHERE entry=15688;
UPDATE creature_template SET ScriptName='boss_shade_of_aran' WHERE entry=16524;
UPDATE creature_template SET ScriptName='boss_netherspite' WHERE entry=15689;
UPDATE creature_template SET ScriptName='boss_malchezaar' WHERE entry=15690;
UPDATE creature_template SET ScriptName='boss_nightbane' WHERE entry=17225;
UPDATE creature_template SET ScriptName='boss_bigbadwolf' WHERE entry=17521;
UPDATE creature_template SET ScriptName='npc_fiendish_portal' WHERE entry=17265;
UPDATE creature_template SET ScriptName='npc_netherspite_portal' WHERE entry IN (17367,17368,17369);
UPDATE creature_template SET ScriptName='npc_barnes' WHERE entry=16812;
UPDATE creature_template SET ScriptName='npc_grandmother' WHERE entry=17603;
UPDATE creature_template SET ScriptName='npc_image_of_medivh' WHERE entry=17651;
UPDATE creature_template SET ScriptName='npc_image_arcanagos' WHERE entry=17652;
UPDATE creature_template SET ScriptName='npc_echo_of_medivh' WHERE entry=16816;
UPDATE creature_template SET ScriptName='npc_king_llane' WHERE entry=21684;
UPDATE creature_template SET ScriptName='npc_warchief_blackhand' WHERE entry=21752;
UPDATE creature_template SET ScriptName='npc_human_conjurer' WHERE entry=21683;
UPDATE creature_template SET ScriptName='npc_orc_warlock' WHERE entry=21750;
UPDATE creature_template SET ScriptName='npc_human_footman' WHERE entry=17211;
UPDATE creature_template SET ScriptName='npc_orc_grunt' WHERE entry=17469;
UPDATE creature_template SET ScriptName='npc_water_elemental' WHERE entry=21160;
UPDATE creature_template SET ScriptName='npc_summoned_daemon' WHERE entry=21726;
UPDATE creature_template SET ScriptName='npc_human_charger' WHERE entry=21664;
UPDATE creature_template SET ScriptName='npc_orc_wolf' WHERE entry=21748;
UPDATE creature_template SET ScriptName='npc_human_cleric' WHERE entry=21682;
UPDATE creature_template SET ScriptName='npc_orc_necrolyte' WHERE entry=21747;
INSERT INTO scripted_event_id VALUES
(10591,'event_spell_summon_nightbane'),
(10951,'event_spell_medivh_journal');
UPDATE gameobject_template SET ScriptName='go_chessboard' WHERE entry IN(185324);

/* LOCH MODAN */
UPDATE creature_template SET ScriptName='npc_miran' WHERE entry=1379;

/* MAGISTER'S TERRACE */
UPDATE instance_template SET ScriptName='instance_magisters_terrace' WHERE map=585;
UPDATE creature_template SET ScriptName='boss_selin_fireheart' WHERE entry=24723;
UPDATE creature_template SET ScriptName='mob_fel_crystal' WHERE entry=24722;
UPDATE creature_template SET ScriptName='boss_vexallus' WHERE entry=24744;
UPDATE creature_template SET ScriptName='mob_pure_energy' WHERE entry=24745;
UPDATE creature_template SET ScriptName='boss_priestess_delrissa' WHERE entry=24560;
UPDATE creature_template SET ScriptName='npc_kagani_nightstrike' WHERE entry=24557;
UPDATE creature_template SET ScriptName='npc_ellrys_duskhallow' WHERE entry=24558;
UPDATE creature_template SET ScriptName='npc_eramas_brightblaze' WHERE entry=24554;
UPDATE creature_template SET ScriptName='npc_yazzai' WHERE entry=24561;
UPDATE creature_template SET ScriptName='npc_warlord_salaris' WHERE entry=24559;
UPDATE creature_template SET ScriptName='npc_garaxxas' WHERE entry=24555;
UPDATE creature_template SET ScriptName='npc_apoko' WHERE entry=24553;
UPDATE creature_template SET ScriptName='npc_zelfan' WHERE entry=24556;
UPDATE creature_template SET ScriptName='boss_felblood_kaelthas' WHERE entry=24664;
UPDATE creature_template SET ScriptName='mob_arcane_sphere' WHERE entry=24708;
UPDATE creature_template SET ScriptName='mob_phoenix' WHERE entry=24674;
UPDATE creature_template SET ScriptName='npc_kalecgos' WHERE entry=24844;
INSERT INTO scripted_event_id VALUES
(16547,'event_go_scrying_orb');

/* MARAUDON */
UPDATE instance_template SET ScriptName="instance_maraudon" WHERE map=349;
UPDATE creature_template SET ScriptName='boss_noxxion' WHERE entry=13282;
UPDATE gameobject_template SET ScriptName="go_larva_spewer" WHERE entry=178559;

/* MOLTEN CORE */
UPDATE instance_template SET ScriptName='instance_molten_core' WHERE map=409;
UPDATE creature_template SET ScriptName='boss_lucifron' WHERE entry=12118;
UPDATE creature_template SET ScriptName='boss_magmadar' WHERE entry=11982;
UPDATE creature_template SET ScriptName='boss_gehennas' WHERE entry=12259;
UPDATE creature_template SET ScriptName='boss_garr' WHERE entry=12057;
UPDATE creature_template SET ScriptName='boss_baron_geddon' WHERE entry=12056;
UPDATE creature_template SET ScriptName='boss_shazzrah' WHERE entry=12264;
UPDATE creature_template SET ScriptName='boss_golemagg' WHERE entry=11988;
UPDATE creature_template SET ScriptName='boss_sulfuron' WHERE entry=12098;
UPDATE creature_template SET ScriptName='boss_majordomo' WHERE entry=12018;
UPDATE creature_template SET ScriptName='boss_ragnaros' WHERE entry=11502;
UPDATE creature_template SET ScriptName='mob_firesworn' WHERE entry=12099;
UPDATE creature_template SET ScriptName='mob_core_rager' WHERE entry=11672;
UPDATE creature_template SET ScriptName='mob_flamewaker_priest' WHERE entry=11662;

/* MOONGLADE */
UPDATE creature_template SET ScriptName='npc_clintar_dw_spirit' WHERE entry=22916;
UPDATE creature_template SET ScriptName='npc_keeper_remulos' WHERE entry=11832;
UPDATE creature_template SET ScriptName='boss_eranikus' WHERE entry=15491;
UPDATE gameobject_template SET ScriptName='go_omen_cluster' WHERE entry=180859;

/* MULGORE */
UPDATE creature_template SET ScriptName='npc_kyle_the_frenzied' WHERE entry=23616;

/* NAGRAND */
UPDATE creature_template SET ScriptName='mob_lump' WHERE entry=18351;
UPDATE creature_template SET ScriptName='npc_nagrand_captive' WHERE entry IN (18209,18210);
UPDATE creature_template SET ScriptName='npc_creditmarker_visit_with_ancestors' WHERE entry IN (18840,18841,18842,18843);
UPDATE creature_template SET ScriptName='npc_rethhedron' WHERE entry=22357;
UPDATE creature_template SET ScriptName='npc_gurthock' WHERE entry=18471;

/* NAXXRAMAS */
UPDATE instance_template SET ScriptName='instance_naxxramas' WHERE map=533;
UPDATE creature_template SET ScriptName='boss_anubrekhan' WHERE entry=15956;
UPDATE creature_template SET ScriptName='boss_faerlina' WHERE entry=15953;
UPDATE creature_template SET ScriptName='boss_maexxna' WHERE entry=15952;
UPDATE creature_template SET ScriptName='npc_web_wrap' WHERE entry=16486;
UPDATE creature_template SET ScriptName='boss_noth' WHERE entry=15954;
UPDATE creature_template SET ScriptName='boss_heigan' WHERE entry=15936;
UPDATE creature_template SET ScriptName='boss_loatheb' WHERE entry=16011;
UPDATE creature_template SET ScriptName='boss_razuvious' WHERE entry=16061;
UPDATE creature_template SET ScriptName='boss_gothik' WHERE entry=16060;
UPDATE creature_template SET ScriptName='spell_anchor' WHERE entry=16137;
UPDATE creature_template SET ScriptName='boss_thane_korthazz' WHERE entry=16064;
UPDATE creature_template SET ScriptName='boss_sir_zeliek' WHERE entry=16063;
UPDATE creature_template SET ScriptName='boss_lady_blaumeux' WHERE entry=16065;
UPDATE creature_template SET ScriptName='boss_rivendare_naxx' WHERE entry=30549;
UPDATE creature_template SET ScriptName='boss_patchwerk' WHERE entry=16028;
UPDATE creature_template SET ScriptName='boss_grobbulus' WHERE entry=15931;
UPDATE creature_template SET ScriptName='boss_gluth' WHERE entry=15932;
UPDATE creature_template SET ScriptName='boss_thaddius' WHERE entry=15928;
UPDATE creature_template SET ScriptName='boss_stalagg' WHERE entry=15929;
UPDATE creature_template SET ScriptName='boss_feugen' WHERE entry=15930;
UPDATE creature_template SET ScriptName='npc_tesla_coil' WHERE entry=16218;
UPDATE creature_template SET ScriptName='boss_sapphiron' WHERE entry=15989;
UPDATE gameobject_template SET ScriptName='go_sapphiron_birth' WHERE entry=181356;
UPDATE creature_template SET ScriptName='boss_kelthuzad' WHERE entry=15990;

/* NETHERSTORM */
INSERT INTO scripted_areatrigger VALUES
(4497,'at_commander_dawnforge');
UPDATE gameobject_template SET ScriptName='go_manaforge_control_console' WHERE entry IN (183770,183956,184311,184312);
UPDATE creature_template SET ScriptName='npc_manaforge_control_console' WHERE entry IN (20209,20417,20418,20440);
UPDATE creature_template SET ScriptName='npc_manaforge_spawn' WHERE entry IN(20218,20438,20439,20460);
UPDATE creature_template SET ScriptName='npc_commander_dawnforge' WHERE entry=19831;
UPDATE creature_template SET ScriptName='npc_bessy' WHERE entry=20415;
UPDATE creature_template SET ScriptName='npc_maxx_a_million' WHERE entry=19589;
UPDATE creature_template SET ScriptName='npc_zeppit' WHERE entry=22484;
UPDATE creature_template SET ScriptName='npc_protectorate_demolitionist' WHERE entry=20802;
UPDATE creature_template SET ScriptName='npc_captured_vanguard' WHERE entry=20763;
UPDATE creature_template SET ScriptName='npc_drijya' WHERE entry=20281;
UPDATE creature_template SET ScriptName='npc_dimensius' WHERE entry=19554;
UPDATE creature_template SET ScriptName='npc_saeed' WHERE entry=20985;
UPDATE creature_template SET ScriptName='npc_void_conduit' WHERE entry=20899;
UPDATE creature_template SET ScriptName='npc_salhadaar' WHERE entry=20454;
UPDATE creature_template SET ScriptName='npc_energy_ball' WHERE entry=20769;
UPDATE creature_template SET ScriptName='npc_scrap_reaver' WHERE entry IN(19849);
UPDATE creature_template SET ScriptName='npc_scrapped_fel_reaver' WHERE entry IN(20243);
UPDATE creature_template SET ScriptName='npc_adyen_the_lightwarden' WHERE entry IN(18537);
UPDATE creature_template SET ScriptName='npc_kaylaan_the_lost' WHERE entry IN(20794);
INSERT INTO scripted_areatrigger VALUES
(4523,'at_socrethar_seat');
UPDATE creature_template SET ScriptName='npc_commander_hobb' WHERE entry IN(23434);
UPDATE creature_template SET ScriptName='npc_commander_arcus' WHERE entry IN(23452);

/*  */
/* THE NEXUS */
/*  */

/* EYE OF ETERNITY */
UPDATE instance_template SET ScriptName='instance_eye_of_eternity' WHERE map=616;
UPDATE creature_template SET ScriptName='boss_malygos' WHERE entry=28859;
UPDATE creature_template SET ScriptName='npc_power_spark' WHERE entry=30084;
INSERT INTO scripted_event_id VALUES
(20711,'event_go_focusing_iris');

/* NEXUS */
UPDATE creature_template SET ScriptName='boss_anomalus' WHERE entry=26763;
UPDATE creature_template SET ScriptName='boss_keristrasza' WHERE entry=26723;
UPDATE creature_template SET ScriptName='boss_ormorok' WHERE entry=26794;
UPDATE creature_template SET ScriptName='boss_telestra' WHERE entry=26731;
UPDATE gameobject_template SET ScriptName='go_containment_sphere' WHERE entry IN (188526, 188527, 188528);
UPDATE instance_template SET ScriptName='instance_nexus' WHERE map=576;

/* OCULUS */
UPDATE instance_template SET ScriptName='instance_oculus' WHERE map=578;
UPDATE creature_template SET ScriptName='boss_eregos' WHERE entry=27656;
UPDATE creature_template SET ScriptName='boss_urom' WHERE entry=27655;
UPDATE creature_template SET ScriptName='boss_varos' WHERE entry=27447;
UPDATE creature_template SET ScriptName='npc_arcane_beam' WHERE entry=28239;
UPDATE creature_template SET ScriptName='npc_planar_anomaly' WHERE entry=30879;
INSERT INTO scripted_event_id VALUES
(10665,'event_spell_call_captain'),
(12229,'event_spell_call_captain'),
(18454,'event_spell_call_captain'),
(18455,'event_spell_call_captain');

/* OBSIDIAN SANCTUM */
UPDATE instance_template SET ScriptName='instance_obsidian_sanctum' WHERE map=615;
UPDATE creature_template SET ScriptName='boss_sartharion' WHERE entry=28860;
UPDATE creature_template SET ScriptName='mob_vesperon' WHERE entry=30449;
UPDATE creature_template SET ScriptName='mob_shadron' WHERE entry=30451;
UPDATE creature_template SET ScriptName='mob_tenebron' WHERE entry=30452;
UPDATE creature_template SET ScriptName='mob_twilight_eggs' WHERE entry IN (30882,31204);
UPDATE creature_template SET ScriptName='npc_tenebron_egg_controller' WHERE entry=31138;
UPDATE creature_template SET ScriptName='npc_flame_tsunami' WHERE entry=30616;
UPDATE creature_template SET ScriptName='npc_fire_cyclone' WHERE entry=30648;

/* ONYXIA'S LAIR */
UPDATE instance_template SET ScriptName='instance_onyxias_lair' WHERE map=249;
UPDATE creature_template SET ScriptName='boss_onyxia' WHERE entry=10184;

/* ORGRIMMAR */
UPDATE creature_template SET ScriptName='npc_overlord_runthalak' WHERE entry=14392;
UPDATE creature_template SET ScriptName='npc_high_overlord_saurfang' WHERE entry=14720;

/* RAGEFIRE CHASM */

/* RAZORFEN DOWNS */
UPDATE instance_template SET ScriptName='instance_razorfen_downs' WHERE map=129;
UPDATE creature_template SET ScriptName='npc_belnistrasz' WHERE entry=8516;
INSERT INTO scripted_event_id VALUES (3130, 'event_go_tutenkash_gong');

/* RAZORFEN KRAUL */
UPDATE instance_template SET ScriptName='instance_razorfen_kraul' WHERE map=47;
UPDATE creature_template SET ScriptName='npc_willix_the_importer' WHERE entry=4508;
UPDATE creature_template SET ScriptName='npc_snufflenose_gopher' WHERE entry=4781;

/* REDRIDGE MOUNTAINS */
UPDATE creature_template SET ScriptName='npc_corporal_keeshan' WHERE entry=349;

/* RUBY SANCTUM */
UPDATE instance_template SET ScriptName='instance_ruby_sanctum' WHERE map=724;
UPDATE creature_template SET ScriptName='boss_baltharus' WHERE entry=39751;
UPDATE creature_template SET ScriptName='boss_saviana' WHERE entry=39747;
UPDATE creature_template SET ScriptName='boss_zarithrian' WHERE entry=39746;
UPDATE creature_template SET ScriptName='boss_halion_real' WHERE entry=39863;
UPDATE creature_template SET ScriptName='boss_halion_twilight' WHERE entry=40142;
UPDATE creature_template SET ScriptName='npc_halion_controller' WHERE entry=40146;
UPDATE creature_template SET ScriptName='npc_meteor_strike_initial' WHERE entry=40029;
UPDATE creature_template SET ScriptName='npc_orb_carrier' WHERE entry=40081;
UPDATE gameobject_template SET ScriptName='go_twilight_portal' WHERE entry IN (202794,202795);

/* RUINS OF AHN'QIRAJ */
UPDATE instance_template SET ScriptName='instance_ruins_of_ahnqiraj' WHERE map=509;
UPDATE creature_template SET ScriptName='mob_anubisath_guardian' WHERE entry=15355;
UPDATE creature_template SET ScriptName='boss_kurinnaxx' WHERE entry=15348;
UPDATE creature_template SET ScriptName='boss_ayamiss' WHERE entry=15369;
UPDATE creature_template SET ScriptName='boss_moam' WHERE entry=15340;
UPDATE creature_template SET ScriptName='boss_ossirian' WHERE entry=15339;
UPDATE gameobject_template SET ScriptName='go_ossirian_crystal' WHERE entry=180619;
UPDATE creature_template SET ScriptName='npc_hive_zara_larva' WHERE entry=15555;
UPDATE creature_template SET ScriptName='boss_buru' WHERE entry=15370;
UPDATE creature_template SET ScriptName='npc_buru_egg' WHERE entry=15514;
UPDATE creature_template SET ScriptName='npc_general_andorov' WHERE entry=15471;
UPDATE creature_template SET ScriptName='npc_kaldorei_elite' WHERE entry=15473;

/* SCARLET MONASTERY */
UPDATE instance_template SET ScriptName='instance_scarlet_monastery' WHERE map=189;
UPDATE creature_template SET ScriptName='boss_arcanist_doan' WHERE entry=6487;
UPDATE creature_template SET ScriptName='boss_herod' WHERE entry=3975;
UPDATE creature_template SET ScriptName='boss_high_inquisitor_whitemane' WHERE entry=3977;
UPDATE creature_template SET ScriptName='boss_scarlet_commander_mograine' WHERE entry=3976;
UPDATE creature_template SET ScriptName='mob_scarlet_trainee' WHERE entry=6575;
UPDATE creature_template SET ScriptName='boss_headless_horseman' WHERE entry=23682;
UPDATE creature_template SET ScriptName='boss_head_of_horseman' WHERE entry=23775;

/* SCHOLOMANCE */
UPDATE instance_template SET ScriptName='instance_scholomance' WHERE map=289;
UPDATE creature_template SET ScriptName='boss_darkmaster_gandling' WHERE entry=1853;
UPDATE creature_template SET ScriptName='boss_jandice_barov' WHERE entry=10503;
UPDATE creature_template SET ScriptName='npc_spectral_tutor' WHERE entry=10498;
INSERT INTO scripted_event_id VALUES
(5140,'event_dawn_gambit'),
(5618,'event_spell_gandling_shadow_portal'),
(5619,'event_spell_gandling_shadow_portal'),
(5620,'event_spell_gandling_shadow_portal'),
(5621,'event_spell_gandling_shadow_portal'),
(5622,'event_spell_gandling_shadow_portal'),
(5623,'event_spell_gandling_shadow_portal');

/* SEARING GORGE */
UPDATE creature_template SET ScriptName='npc_dorius_stonetender' WHERE entry=8284;

/* SHADOWFANG KEEP */
UPDATE instance_template SET ScriptName='instance_shadowfang_keep' WHERE map=33;
UPDATE creature_template SET ScriptName='npc_shadowfang_prisoner' WHERE entry IN (3849,3850);
UPDATE creature_template SET ScriptName='npc_deathstalker_vincent' WHERE entry=4444;
UPDATE creature_template SET ScriptName='mob_arugal_voidwalker' WHERE entry=4627;
UPDATE creature_template SET ScriptName='boss_arugal' WHERE entry=4275;
UPDATE creature_template SET ScriptName='boss_apothecary_hummel' WHERE entry=36296;
UPDATE creature_template SET ScriptName='npc_valentine_boss_manager' WHERE entry=36643;

/* SHADOWMOON VALLEY */
UPDATE creature_template SET ScriptName='boss_doomwalker' WHERE entry=17711;
UPDATE creature_template SET ScriptName='npc_dragonmaw_peon' WHERE entry=22252;
UPDATE creature_template SET ScriptName='mob_mature_netherwing_drake' WHERE entry=21648;
UPDATE creature_template SET ScriptName='mob_enslaved_netherwing_drake' WHERE entry=21722;
UPDATE creature_template SET ScriptName='npc_wilda' WHERE entry=21027;
UPDATE creature_template SET ScriptName='mob_torloth' WHERE entry=22076;
UPDATE creature_template SET ScriptName='npc_totem_of_spirits' WHERE entry=21071;
INSERT INTO scripted_event_id VALUES
(13513,'event_spell_soul_captured_credit'),
(13514,'event_spell_soul_captured_credit'),
(13515,'event_spell_soul_captured_credit'),
(13516,'event_spell_soul_captured_credit');
UPDATE creature_template SET ScriptName='npc_lord_illidan_stormrage' WHERE entry=22083;
UPDATE gameobject_template SET ScriptName='go_crystal_prison' WHERE entry=185126;
UPDATE creature_template SET ScriptName='npc_spawned_oronok_tornheart' WHERE entry=21685;
UPDATE creature_template SET ScriptName='npc_domesticated_felboar' WHERE entry=21195;
UPDATE creature_template SET ScriptName='npc_shadowmoon_tuber_node' WHERE entry=21347;
UPDATE creature_template SET ScriptName='npc_veneratus_spawn_node' WHERE entry=21334;
UPDATE creature_template SET ScriptName='mob_shadowmoon_soulstealer' WHERE entry=22061;
UPDATE creature_template SET ScriptName='npc_shadowlord_deathwail' WHERE entry=22006;
UPDATE creature_template SET ScriptName='npc_disobedient_dragonmaw_peon' WHERE entry IN(23311);
UPDATE creature_template SET ScriptName='npc_dragonmaw_racer_muckjaw' WHERE entry=23340;
UPDATE creature_template SET ScriptName='npc_dragonmaw_racer_trope' WHERE entry=23342;
UPDATE creature_template SET ScriptName='npc_dragonmaw_racer_corlok' WHERE entry=23344;
UPDATE creature_template SET ScriptName='npc_dragonmaw_racer_ichman' WHERE entry=23345;
UPDATE creature_template SET ScriptName='npc_dragonmaw_racer_mulverick' WHERE entry=23346;
UPDATE creature_template SET ScriptName='npc_dragonmaw_racer_skyshatter' WHERE entry=23348;
UPDATE creature_template SET ScriptName='mob_bt_battle_fighter' WHERE entry IN (22857, 22858, 22860, 22861, 22862, 22863, 22864, 22988);
UPDATE creature_template SET ScriptName='npc_bt_battle_sensor' WHERE entry=22934;

/* SHATTRATH */
UPDATE creature_template SET ScriptName='npc_dirty_larry' WHERE entry=19720;
UPDATE creature_template SET ScriptName='npc_khadgars_servant' WHERE entry=19685;
UPDATE creature_template SET ScriptName='npc_salsalabim' WHERE entry=18584;
UPDATE creature_template SET ScriptName='npc_adal' WHERE entry IN(18481);

/* SHOLAZAR BASIN */
UPDATE creature_template SET ScriptName='npc_helice' WHERE entry=28787;
UPDATE creature_template SET ScriptName='npc_injured_rainspeaker' WHERE entry=28217;
UPDATE creature_template SET ScriptName='npc_mosswalker_victim' WHERE entry=28113;
UPDATE creature_template SET ScriptName='npc_tipsy_mcmanus' WHERE entry=28566;
UPDATE creature_template SET ScriptName='npc_wants_fruit_credit' WHERE entry IN (28535,28536,28537);
UPDATE gameobject_template SET ScriptName='go_quest_still_at_it_credit' WHERE entry IN (190635,190636);

/* SILITHUS */
UPDATE creature_template SET ScriptName='npc_anachronos_the_ancient' WHERE entry=15381;
UPDATE gameobject_template SET ScriptName='go_crystalline_tear' WHERE entry=180633;
UPDATE creature_template SET ScriptName='npc_solenor' WHERE entry IN (14530,14536);

/* SILVERMOON */

/* SILVERPINE FOREST */
UPDATE creature_template SET ScriptName='npc_deathstalker_erland' WHERE entry=1978;
UPDATE creature_template SET ScriptName='npc_deathstalker_faerleia' WHERE entry=2058;

/* STOCKADES */

/* STONETALON MOUNTAINS */
UPDATE creature_template SET ScriptName='npc_kaya' WHERE entry=11856;
UPDATE gameobject_template SET ScriptName = 'go_covert_ops' WHERE entry=19590;

/* STORM PEAKS */
UPDATE creature_template SET ScriptName='npc_floating_spirit' WHERE entry IN (30141,30143,30145);
UPDATE creature_template SET ScriptName='npc_restless_frostborn' WHERE entry IN (29974,30135,30144);
UPDATE creature_template SET ScriptName='npc_injured_miner' WHERE entry=29434;

/* STORMWIND CITY */
UPDATE creature_template SET ScriptName='npc_bartleby' WHERE entry=6090;
UPDATE creature_template SET ScriptName='npc_dashel_stonefist' WHERE entry=4961;
UPDATE creature_template SET ScriptName='npc_lady_katrana_prestor' WHERE entry=1749;
UPDATE creature_template SET ScriptName='npc_squire_rowe' WHERE entry=17804;
UPDATE creature_template SET ScriptName='npc_reginald_windsor' WHERE entry =12580;
UPDATE creature_template SET ScriptName='npc_major_mattingly' WHERE entry=14394;
UPDATE creature_template SET ScriptName='npc_field_marshal_afrasiabi' WHERE entry=14721;

/* STRANGLETHORN VALE */
UPDATE creature_template SET ScriptName='mob_yenniku' WHERE entry=2530;
UPDATE gameobject_template SET ScriptName='go_transpolyporter_bb' WHERE entry IN(142172);

/* STRATHOLME */
UPDATE instance_template SET ScriptName='instance_stratholme' WHERE map=329;
UPDATE creature_template SET ScriptName='boss_dathrohan_balnazzar' WHERE entry=10812;
UPDATE creature_template SET ScriptName='boss_maleki_the_pallid' WHERE entry=10438;
UPDATE creature_template SET ScriptName='boss_cannon_master_willey' WHERE entry=10997;
UPDATE creature_template SET ScriptName='boss_baroness_anastari' WHERE entry=10436;
UPDATE creature_template SET ScriptName='boss_silver_hand_bosses' WHERE entry IN (17910,17911,17912,17913,17914);
UPDATE creature_template SET ScriptName='npc_aurius' WHERE entry=10917;
UPDATE creature_template SET ScriptName='mobs_spectral_ghostly_citizen' WHERE entry IN (10384,10385);
UPDATE creature_template SET ScriptName='mob_restless_soul' WHERE entry=11122;
UPDATE gameobject_template SET ScriptName='go_gauntlet_gate' WHERE entry=175357;
UPDATE gameobject_template SET ScriptName='go_service_gate' WHERE entry=175368;
UPDATE gameobject_template SET ScriptName='go_stratholme_postbox' WHERE entry IN (176346,176349,176350,176351,176352,176353);

/* SUNKEN TEMPLE */
UPDATE instance_template SET ScriptName='instance_sunken_temple' WHERE map=109;
INSERT INTO scripted_areatrigger VALUES
(4016,'at_shade_of_eranikus');
UPDATE creature_template SET ScriptName='npc_malfurion_stormrage' WHERE entry=15362;
INSERT INTO scripted_event_id VALUES
(3094,'event_antalarion_statue_activation'),
(3095,'event_antalarion_statue_activation'),
(3097,'event_antalarion_statue_activation'),
(3098,'event_antalarion_statue_activation'),
(3099,'event_antalarion_statue_activation'),
(3100,'event_antalarion_statue_activation');
UPDATE gameobject_template SET ScriptName='go_eternal_flame' WHERE entry IN (148418,148419,148420,148421);
INSERT INTO scripted_event_id VALUES
(8502,'event_avatar_of_hakkar');

/* SUNWELL PLATEAU */
UPDATE instance_template SET ScriptName='instance_sunwell_plateau' WHERE map=580;
UPDATE creature_template SET ScriptName='boss_brutallus' WHERE entry=24882;
UPDATE creature_template SET ScriptName='boss_kalecgos' WHERE entry=24850;
UPDATE creature_template SET ScriptName='boss_kalecgos_humanoid' WHERE entry=24891;
UPDATE creature_template SET ScriptName='boss_sathrovarr' WHERE entry=24892;
INSERT INTO scripted_areatrigger VALUES 
(4853,'at_madrigosa');
UPDATE creature_template SET ScriptName='boss_alythess' WHERE entry=25166;
UPDATE creature_template SET ScriptName='boss_sacrolash' WHERE entry=25165;
UPDATE creature_template SET ScriptName='npc_shadow_image' WHERE entry=25214;
UPDATE creature_template SET ScriptName='npc_sunblade_scout' WHERE entry=25372;
UPDATE creature_template SET ScriptName='npc_sunblade_protector' WHERE entry=25507;
UPDATE creature_template SET ScriptName='boss_muru' WHERE entry=25741;
UPDATE creature_template SET ScriptName='boss_entropius' WHERE entry=25840;
UPDATE creature_template SET ScriptName='npc_portal_target' WHERE entry=25770;
UPDATE creature_template SET ScriptName='boss_kiljaeden' WHERE entry=25315;
UPDATE creature_template SET ScriptName='npc_kiljaeden_controller' WHERE entry=25608;
UPDATE creature_template SET ScriptName='npc_sinister_reflection' WHERE entry=25708;
UPDATE creature_template SET ScriptName='boss_felmyst' WHERE entry=25038;
UPDATE creature_template SET ScriptName='npc_shield_orb' WHERE entry=25502;
UPDATE creature_template SET ScriptName='npc_power_blue_flight' WHERE entry=25653;
UPDATE creature_template SET ScriptName='npc_demonic_vapor' WHERE entry=25265;
UPDATE creature_template SET ScriptName='npc_darkness' WHERE entry=25879;
UPDATE creature_template SET ScriptName='npc_singularity' WHERE entry=25855;
UPDATE creature_template SET ScriptName='npc_dark_fiend' WHERE entry=25744;
UPDATE creature_template SET ScriptName='npc_core_of_entropius' WHERE entry=26262;

/* SWAMP OF SORROWS */
UPDATE creature_template SET ScriptName='npc_galen_goodward' WHERE entry=5391;

/* TANARIS */
UPDATE creature_template SET ScriptName='mob_aquementas' WHERE entry=9453;
UPDATE creature_template SET ScriptName='npc_custodian_of_time' WHERE entry=20129;
UPDATE creature_template SET ScriptName='npc_oox17tn' WHERE entry=7784;
UPDATE creature_template SET ScriptName='npc_stone_watcher_of_norgannon' WHERE entry=7918;
UPDATE creature_template SET ScriptName='npc_tooga' WHERE entry=5955;

/* TELDRASSIL */
UPDATE creature_template SET ScriptName='npc_mist' WHERE entry=3568;

/*  */
/* TEMPEST KEEP */
/*  */

/* THE MECHANAR */
UPDATE creature_template SET ScriptName='boss_mechano_lord_capacitus' WHERE entry = 19219;
UPDATE creature_template SET ScriptName='mob_nether_charge' WHERE entry IN(20405);
UPDATE creature_template SET ScriptName='boss_nethermancer_sepethrea' WHERE entry=19221;
UPDATE creature_template SET ScriptName='boss_pathaleon_the_calculator' WHERE entry=19220;
UPDATE creature_template SET ScriptName='mob_nether_wraith' WHERE entry=21062;
UPDATE instance_template SET ScriptName='instance_mechanar' WHERE map=554;
UPDATE creature_template SET ScriptName='npc_raging_flames' WHERE entry=20481;

/* THE BOTANICA */
UPDATE creature_template SET ScriptName='boss_high_botanist_freywinn' WHERE entry=17975;
UPDATE creature_template SET ScriptName='boss_laj' WHERE entry=17980;
UPDATE creature_template SET ScriptName='boss_warp_splinter' WHERE entry=17977;
UPDATE creature_template SET ScriptName='mob_warp_splinter_treant' WHERE entry=19949;
UPDATE creature_template SET ScriptName='boss_thorngrin' WHERE entry IN(17978);

/* THE ARCATRAZ */
UPDATE instance_template SET ScriptName='instance_arcatraz' WHERE map=552;
UPDATE creature_template SET ScriptName='boss_harbinger_skyriss' WHERE entry=20912;
UPDATE creature_template SET ScriptName='boss_dalliah' WHERE entry=20885;
UPDATE creature_template SET ScriptName='boss_soccothrates' WHERE entry=20886;
UPDATE creature_template SET ScriptName='npc_warden_mellichar' WHERE entry=20904;
UPDATE creature_template SET ScriptName='npc_millhouse_manastorm' WHERE entry=20977;
UPDATE creature_template SET ScriptName='npc_arcatraz_defender' WHERE entry=20857;

/* THE EYE */
UPDATE instance_template SET ScriptName='instance_the_eye' WHERE map=550;
/* Al'ar event */
UPDATE creature_template SET ScriptName='boss_alar' WHERE entry=19514;
/* Void Reaver event */
UPDATE creature_template SET ScriptName='boss_void_reaver' WHERE entry=19516;
/* Astromancer event */
UPDATE creature_template SET ScriptName='boss_high_astromancer_solarian' WHERE entry=18805;
UPDATE creature_template SET ScriptName='mob_solarium_priest' WHERE entry=18806;
/* Kael'thas event */
UPDATE creature_template SET ScriptName='boss_kaelthas' WHERE entry=19622;
UPDATE creature_template SET ScriptName='boss_thaladred_the_darkener' WHERE entry=20064;
UPDATE creature_template SET ScriptName='boss_lord_sanguinar' WHERE entry=20060;
UPDATE creature_template SET ScriptName='boss_grand_astromancer_capernian' WHERE entry=20062;
UPDATE creature_template SET ScriptName='boss_master_engineer_telonicus' WHERE entry=20063;
UPDATE creature_template SET ScriptName='mob_phoenix' WHERE entry=21362;
UPDATE creature_template SET ScriptName='npc_nether_vapor' WHERE entry=21002;

/* TEMPLE OF AHN'QIRAJ */
UPDATE instance_template SET ScriptName='instance_temple_of_ahnqiraj' WHERE map=531;
UPDATE creature_template SET ScriptName='boss_cthun' WHERE entry=15727;
UPDATE creature_template SET ScriptName='boss_skeram' WHERE entry=15263;
UPDATE creature_template SET ScriptName='boss_vem' WHERE entry=15544;
UPDATE creature_template SET ScriptName='boss_yauj' WHERE entry=15543;
UPDATE creature_template SET ScriptName='boss_kri' WHERE entry=15511;
UPDATE creature_template SET ScriptName='boss_sartura' WHERE entry=15516;
UPDATE creature_template SET ScriptName='boss_fankriss' WHERE entry=15510;
UPDATE creature_template SET ScriptName='boss_viscidus' WHERE entry=15299;
UPDATE creature_template SET ScriptName='boss_huhuran' WHERE entry=15509;
UPDATE creature_template SET ScriptName='boss_veklor' WHERE entry=15276;
UPDATE creature_template SET ScriptName='boss_veknilash' WHERE entry=15275;
UPDATE creature_template SET ScriptName='boss_ouro' WHERE entry=15517;
UPDATE creature_template SET ScriptName='npc_ouro_spawner' WHERE entry=15957;
UPDATE creature_template SET ScriptName='npc_ouro_mound' WHERE entry=15712;
UPDATE creature_template SET ScriptName='npc_ouro_trigger' WHERE entry=15717;
UPDATE creature_template SET ScriptName='npc_ouro_scarab' WHERE entry=15718;
UPDATE creature_template SET ScriptName='boss_eye_of_cthun' WHERE entry=15589;
UPDATE creature_template SET ScriptName='mob_giant_claw_tentacle' WHERE entry=15728;
UPDATE creature_template SET ScriptName='mob_anubisath_sentinel' WHERE entry=15264;
UPDATE creature_template SET ScriptName='mob_anubisath_defender' WHERE entry=15277;
INSERT INTO scripted_areatrigger VALUES
(4033,'at_stomach_cthun'),
(4034,'at_stomach_cthun'),
(4036,'at_stomach_cthun');
INSERT INTO scripted_event_id VALUES
(9738,'event_cthun_reset');

/* TEROKKAR FOREST */
UPDATE creature_template SET ScriptName='mob_netherweb_victim' WHERE entry=22355;
UPDATE creature_template SET ScriptName='mob_unkor_the_ruthless' WHERE entry=18262;
UPDATE creature_template SET ScriptName='npc_akuno' WHERE entry=22377;
UPDATE creature_template SET ScriptName='npc_hungry_nether_ray' WHERE entry=23439;
UPDATE creature_template SET ScriptName='npc_letoll' WHERE entry=22458;
UPDATE creature_template SET ScriptName='npc_isla_starmane' WHERE entry=18760;
UPDATE creature_template SET ScriptName="npc_skywing" WHERE entry=22424;
UPDATE creature_template SET ScriptName="npc_cenarion_sparrowhawk" WHERE entry=22972;
UPDATE creature_template SET ScriptName="npc_skyguard_prisoner" WHERE entry=23383;
UPDATE creature_template SET ScriptName='npc_avatar_of_terokk' WHERE entry=22375;
UPDATE creature_template SET ScriptName='npc_minion_of_terokk' WHERE entry=22376;
UPDATE creature_template SET ScriptName='boss_terokk' WHERE entry IN(21838);
UPDATE creature_template SET ScriptName='boss_terokk' WHERE entry=21838;
UPDATE gameobject_template SET ScriptName='go_monstrous_kaliri_egg' WHERE entry=185549;

INSERT INTO scripted_event_id VALUES
(15014,'event_summon_terokk');

/* THOUSAND NEEDLES */
UPDATE creature_template SET ScriptName='npc_kanati' WHERE entry=10638;
UPDATE creature_template SET ScriptName='npc_plucky_johnson' WHERE entry=6626;
UPDATE creature_template SET ScriptName='npc_paoka_swiftmountain' WHERE entry=10427;
UPDATE creature_template SET ScriptName='npc_lakota_windsong' WHERE entry=10646;

/* THUNDER BLUFF */

/* TIRISFAL GLADES */
UPDATE gameobject_template SET ScriptName='go_mausoleum_trigger' WHERE entry=104593;
UPDATE gameobject_template SET ScriptName='go_mausoleum_door' WHERE entry=176594;
UPDATE creature_template SET ScriptName='npc_calvin_montague' WHERE entry=6784;

/* ULDAMAN */
INSERT INTO scripted_event_id VALUES
(2228,'event_spell_altar_boss_aggro'),
(2268,'event_spell_altar_boss_aggro');
UPDATE creature_template SET ScriptName='boss_archaedas' WHERE entry=2748;
UPDATE creature_template SET ScriptName='mob_archaeras_add' WHERE entry IN (7309,7076,7077,10120);
UPDATE instance_template SET ScriptName='instance_uldaman' WHERE map=70;

/*  */
/* ULDUAR */
/*  */

/* HALLS OF LIGHTNING */
UPDATE instance_template SET ScriptName='instance_halls_of_lightning' WHERE map=602;
UPDATE creature_template SET ScriptName='boss_bjarngrim' WHERE entry=28586;
UPDATE creature_template SET ScriptName='mob_stormforged_lieutenant' WHERE entry=29240;
UPDATE creature_template SET ScriptName='boss_volkhan' WHERE entry=28587;
UPDATE creature_template SET ScriptName='mob_molten_golem' WHERE entry=28695;
UPDATE creature_template SET ScriptName='npc_volkhan_anvil' WHERE entry=28823;
UPDATE creature_template SET ScriptName='boss_ionar' WHERE entry=28546;
UPDATE creature_template SET ScriptName='mob_spark_of_ionar' WHERE entry=28926;
UPDATE creature_template SET ScriptName='boss_loken' WHERE entry=28923;

/* HALLS OF STONE */
UPDATE instance_template SET ScriptName='instance_halls_of_stone' WHERE map=599;
UPDATE creature_template SET ScriptName='boss_sjonnir' WHERE entry=27978;
UPDATE creature_template SET ScriptName='npc_brann_hos' WHERE entry=28070;
UPDATE creature_template SET ScriptName='npc_dark_matter' WHERE entry=28235;
INSERT INTO scripted_event_id VALUES
(20645,'event_spell_brann_achievement_fail');

/* ULDUAR */
UPDATE instance_template SET ScriptName='instance_ulduar' WHERE map=603;
UPDATE gameobject_template SET ScriptName='go_ulduar_teleporter' WHERE entry=194569;
UPDATE creature_template SET ScriptName='boss_general_vezax' WHERE entry=33271;
UPDATE creature_template SET ScriptName='npc_saronite_vapor' WHERE entry=33488;
UPDATE creature_template SET ScriptName='boss_auriaya' WHERE entry=33515;
UPDATE creature_template SET ScriptName='boss_feral_defender' WHERE entry=34035;
UPDATE creature_template SET ScriptName='boss_brundir' WHERE entry=32857;
UPDATE creature_template SET ScriptName='boss_molgeim' WHERE entry=32927;
UPDATE creature_template SET ScriptName='boss_steelbreaker' WHERE entry=32867;
UPDATE creature_template SET ScriptName='boss_ignis' WHERE entry=33118;
UPDATE creature_template SET ScriptName='npc_iron_construct' WHERE entry=33121;
UPDATE creature_template SET ScriptName='npc_scorch' WHERE entry=33221;
UPDATE creature_template SET ScriptName='boss_xt_002' WHERE entry=33293;
UPDATE creature_template SET ScriptName='boss_heart_deconstructor' WHERE entry=33329;
UPDATE creature_template SET ScriptName='npc_scrapbot' WHERE entry=33343;
UPDATE creature_template SET ScriptName='npc_xt_toy_pile' WHERE entry=33337;
UPDATE creature_template SET ScriptName='boss_razorscale' WHERE entry=33186;
UPDATE creature_template SET ScriptName='npc_expedition_commander' WHERE entry=33210;
UPDATE creature_template SET ScriptName='npc_razorscale_spawner' WHERE entry=33245;
UPDATE creature_template SET ScriptName='npc_harpoon_fire_state' WHERE entry=33282;
UPDATE creature_template SET ScriptName='npc_keeper_norgannon' WHERE entry=33686;
UPDATE creature_template SET ScriptName='npc_brann_ulduar' WHERE entry=33579;
UPDATE creature_template SET ScriptName='boss_flame_leviathan' WHERE entry=33113;
UPDATE creature_template SET ScriptName='npc_hodir_fury_reticle' WHERE entry=33108;
UPDATE creature_template SET ScriptName='npc_hodir_fury' WHERE entry=33212;
UPDATE creature_template SET ScriptName='npc_freya_ward' WHERE entry=33367;
UPDATE creature_template SET ScriptName='npc_mimiron_inferno' WHERE entry=33370;
UPDATE creature_template SET ScriptName='boss_kologarn' WHERE entry=32930;
UPDATE creature_template SET ScriptName='npc_focused_eyebeam' WHERE entry IN (33802,33632);
UPDATE creature_template SET ScriptName='npc_rubble_stalker' WHERE entry=33809;
UPDATE creature_template SET ScriptName='npc_storm_tempered_keeper' WHERE entry IN (33699,33722);
UPDATE creature_template SET ScriptName='npc_charged_sphere' WHERE entry=33715;
UPDATE creature_template SET ScriptName='boss_algalon' WHERE entry=32871;
UPDATE creature_template SET ScriptName='npc_living_constellation' WHERE entry=33052;
UPDATE creature_template SET ScriptName='npc_worm_hole' WHERE entry=34099;
UPDATE creature_template SET ScriptName='npc_black_hole' WHERE entry=32953;
UPDATE creature_template SET ScriptName='npc_collapsing_star' WHERE entry=32955;
UPDATE gameobject_template SET ScriptName='go_celestial_access' WHERE entry IN (194628,194752);
UPDATE creature_template SET ScriptName='boss_hodir' WHERE entry=32845;
UPDATE creature_template SET ScriptName='npc_flash_freeze' WHERE entry IN (32926,32938);
UPDATE creature_template SET ScriptName='npc_icicle_target' WHERE entry=33174;
UPDATE creature_template SET ScriptName='boss_thorim' WHERE entry=32865;
UPDATE creature_template SET ScriptName='boss_sif' WHERE entry=33196;
UPDATE creature_template SET ScriptName='npc_thunder_orb' WHERE entry=33378;
UPDATE creature_template SET ScriptName='npc_runic_colossus' WHERE entry=32872;
UPDATE creature_template SET ScriptName='boss_freya' WHERE entry=32906;
UPDATE creature_template SET ScriptName='npc_eonars_gift' WHERE entry=33228;
UPDATE creature_template SET ScriptName='npc_nature_bomb' WHERE entry=34129;
UPDATE creature_template SET ScriptName='npc_iron_roots' WHERE entry IN (33088,33168);
UPDATE creature_template SET ScriptName='npc_healthy_spore' WHERE entry=33215;
UPDATE creature_template SET ScriptName='npc_water_spirit' WHERE entry=33202;
UPDATE creature_template SET ScriptName='npc_snaplasher' WHERE entry=32916;
UPDATE creature_template SET ScriptName='npc_storm_lasher' WHERE entry=32919;
UPDATE creature_template SET ScriptName='boss_mimiron' WHERE entry=33350;
UPDATE creature_template SET ScriptName='boss_leviathan_mk2' WHERE entry=33432;
UPDATE creature_template SET ScriptName='boss_vx001' WHERE entry=33651;
UPDATE creature_template SET ScriptName='boss_aerial_unit' WHERE entry=33670;
UPDATE creature_template SET ScriptName='npc_proximity_mine' WHERE entry=34362;
UPDATE creature_template SET ScriptName='npc_bot_trigger' WHERE entry=33856;
UPDATE creature_template SET ScriptName='npc_rocket_strike' WHERE entry=34047;
UPDATE creature_template SET ScriptName='npc_frost_bomb' WHERE entry=34149;
UPDATE creature_template SET ScriptName='npc_mimiron_flames' WHERE entry IN (34363,34121);
UPDATE creature_template SET ScriptName='boss_leviathan_mk2_turret' WHERE entry=34071;
UPDATE creature_template SET ScriptName='npc_computer' WHERE entry=34143;
UPDATE gameobject_template SET ScriptName='go_big_red_button' WHERE entry=194739;
UPDATE gameobject_template SET ScriptName='go_activate_tram' WHERE entry IN (194914,194912,194437);
UPDATE creature_template SET ScriptName='npc_ulduar_keeper' WHERE entry IN (33241,33242,33244,33213);
UPDATE creature_template SET ScriptName='boss_sara' WHERE entry=33134;
UPDATE creature_template SET ScriptName='boss_yogg_saron' WHERE entry=33288;
UPDATE creature_template SET ScriptName='npc_ominous_cloud' WHERE entry=33292;
UPDATE creature_template SET ScriptName='npc_death_ray' WHERE entry=33881;
UPDATE creature_template SET ScriptName='npc_voice_yogg_saron' WHERE entry=33280;
UPDATE creature_template SET ScriptName='npc_brain_yogg_saron' WHERE entry=33890;
UPDATE creature_template SET ScriptName='npc_guardian_of_yogg' WHERE entry=33136;
UPDATE creature_template SET ScriptName='npc_immortal_guardian' WHERE entry=33988;
UPDATE creature_template SET ScriptName='npc_constrictor_tentacle' WHERE entry=33983;
UPDATE creature_template SET ScriptName='npc_descent_madness' WHERE entry=34072;
UPDATE creature_template SET ScriptName='npc_keeper_mimiron' WHERE entry=33412;
UPDATE creature_template SET ScriptName='npc_keeper_thorim' WHERE entry=33413;
INSERT INTO scripted_event_id VALUES
-- Vezax saronite barrier event
(9735,'event_spell_saronite_barrier'),
-- Hodir shatter chest event
(20907,'event_boss_hodir'),
-- Razorscale harpoon event
(20964,'event_spell_harpoon_shot'),
-- Tower of Life destroyed event
(21030,'event_go_ulduar_tower'),
-- Tower of Storms destroyed event
(21031,'event_go_ulduar_tower'),
-- Tower of Frost destroyed event
(21032,'event_go_ulduar_tower'),
-- Tower of Flame destroyed event
(21033,'event_go_ulduar_tower'),
-- Hodir attack start event
(21045,'event_boss_hodir'),
-- Flame Leviathan shutdown event
(21605,'event_ulduar'),
-- XT-002 Scrap repair event
(21606,'event_ulduar'),
-- Ignis construct shatter event
(21620,'event_ulduar');

/* UN'GORO CRATER */
UPDATE creature_template SET ScriptName='npc_ame01' WHERE entry=9623;
UPDATE creature_template SET ScriptName='npc_ringo' WHERE entry=9999;
UPDATE creature_template SET ScriptName='npc_simone_seductress' WHERE entry=14533;
UPDATE creature_template SET ScriptName='npc_simone_the_inconspicuous' WHERE entry=14527;
UPDATE creature_template SET ScriptName='npc_precious_the_devourer' WHERE entry=14538;

/* UNDERCITY */
UPDATE creature_template SET ScriptName='npc_lady_sylvanas_windrunner' WHERE entry=10181;

/*  */
/* UTGARDE KEEP */
/*  */

/* UTGARDE KEEP */
UPDATE instance_template SET ScriptName='instance_utgarde_keep' WHERE map=574;
UPDATE creature_template SET ScriptName='mob_dragonflayer_forge_master' WHERE entry=24079;
UPDATE creature_template SET ScriptName='boss_ingvar' WHERE entry=23954;
UPDATE creature_template SET ScriptName='npc_annhylde' WHERE entry=24068;
UPDATE creature_template SET ScriptName='boss_keleseth' WHERE entry=23953;
UPDATE creature_template SET ScriptName='mob_vrykul_skeleton' WHERE entry=23970;
UPDATE creature_template SET ScriptName='npc_ingvar_throw_dummy' WHERE entry=23997;

/* UTGARDE PINNACLE */
UPDATE creature_template SET ScriptName='boss_skadi' WHERE entry=26693;
UPDATE creature_template SET ScriptName='npc_grauf' WHERE entry=26893;
UPDATE creature_template SET ScriptName='boss_svala' WHERE entry=26668;
UPDATE creature_template SET ScriptName='boss_ymiron' WHERE entry=26861;
UPDATE instance_template SET ScriptName='instance_pinnacle' WHERE map=575;
INSERT INTO scripted_areatrigger VALUES
(4991,'at_skadi'),
(5140,'at_svala_intro');
INSERT INTO scripted_event_id VALUES
(17728,'event_spell_gortok_event'),
(17841,'event_spell_call_flames'),
(20651,'event_achiev_kings_bane');

/* VAULT OF ARCHAVON */

/* VIOLET HOLD */
UPDATE instance_template SET ScriptName='instance_violet_hold' WHERE map=608;
UPDATE creature_template SET ScriptName='npc_door_seal' WHERE entry=30896;
UPDATE creature_template SET ScriptName='npc_sinclari' WHERE entry=30658;
UPDATE creature_template SET ScriptName='npc_prison_event_controller' WHERE entry=30883;
UPDATE creature_template SET ScriptName='npc_teleportation_portal' WHERE entry IN (30679,32174);
UPDATE creature_template SET ScriptName='boss_ichoron' WHERE entry IN (29313,32234);
UPDATE creature_template SET ScriptName='boss_xevozz' WHERE entry IN (29266,32231);
UPDATE creature_template SET ScriptName='npc_ethereal_sphere' WHERE entry IN (29271,32582);

/* WAILING CAVERNS */
UPDATE instance_template SET ScriptName='instance_wailing_caverns' WHERE map=43;
UPDATE creature_template SET ScriptName='npc_disciple_of_naralex' WHERE entry=3678;

/* WESTERN PLAGUELANDS */
UPDATE creature_template SET ScriptName='npc_the_scourge_cauldron' WHERE entry=11152;
UPDATE creature_template SET ScriptName='npc_anchorite_truuen' WHERE entry=17238;
UPDATE creature_template SET ScriptName='npc_taelan_fordring' WHERE entry=1842;
UPDATE creature_template SET ScriptName='npc_isillien' WHERE entry=1840;
UPDATE creature_template SET ScriptName='npc_tirion_fordring' WHERE entry=12126;

/* WESTFALL */
UPDATE creature_template SET ScriptName='npc_daphne_stilwell' WHERE entry=6182;
UPDATE creature_template SET ScriptName='npc_defias_traitor' WHERE entry=467;
UPDATE creature_template SET ScriptName='npc_foreman_klaven_mortwake' WHERE entry=7053;

/* WETLANDS */
UPDATE creature_template SET ScriptName='npc_tapoke_slim_jahn' WHERE entry=4962;
UPDATE creature_template SET ScriptName='npc_mikhail' WHERE entry=4963;

/* WINTERGRASP */
UPDATE creature_template SET ScriptName='npc_spirit_guide_wintergrasp' WHERE entry IN (31841,31842);
UPDATE gameobject_template SET ScriptName='go_vehicle_teleporter' WHERE entry=192951;
INSERT INTO scripted_event_id VALUES
(19677,'event_go_tower_destroy'),
(19676,'event_go_tower_destroy'),
(19675,'event_go_tower_destroy'),
(19661,'event_go_tower_destroy'),
(19662,'event_go_tower_destroy'),
(19663,'event_go_tower_destroy'),
(19664,'event_go_tower_destroy');

/* WINTERSPRING */
UPDATE creature_template SET ScriptName='npc_ranshalla' WHERE entry=10300;
UPDATE gameobject_template SET ScriptName='go_elune_fire' WHERE entry IN (177417, 177404);
UPDATE creature_template SET ScriptName='npc_artorius' WHERE entry IN (14531,14535);

/* ZANGARMARSH */
INSERT INTO scripted_event_id VALUES (11225,'event_taxi_stormcrow');
UPDATE creature_template SET ScriptName='npc_cooshcoosh' WHERE entry=18586;
UPDATE creature_template SET ScriptName='npc_kayra_longmane' WHERE entry=17969;
UPDATE creature_template SET ScriptName='npc_fhwoor' WHERE entry=17877;
UPDATE creature_template SET ScriptName='npc_frostbite' WHERE entry=20061;

/* ZUL'AMAN */
UPDATE instance_template SET ScriptName='instance_zulaman' WHERE map=568;
UPDATE creature_template SET ScriptName='npc_harrison_jones_za' WHERE entry=24358;
INSERT INTO scripted_event_id VALUES (15897,'event_ritual_of_power');
UPDATE creature_template SET ScriptName='boss_akilzon' WHERE entry=23574;
UPDATE creature_template SET ScriptName='mob_soaring_eagle' WHERE entry=24858;
UPDATE creature_template SET ScriptName='boss_halazzi' WHERE entry=23577;
UPDATE creature_template SET ScriptName='boss_spirit_lynx' WHERE entry=24143;
UPDATE creature_template SET ScriptName='npc_corrupted_lightning_totem' WHERE entry IN(24224);
UPDATE creature_template SET ScriptName='boss_janalai' WHERE entry=23578;
UPDATE creature_template SET ScriptName='boss_malacrass' WHERE entry=24239;
UPDATE creature_template SET ScriptName='boss_nalorakk' WHERE entry=23576;
UPDATE creature_template SET ScriptName='boss_zuljin' WHERE entry=23863;
UPDATE creature_template SET ScriptName='npc_feather_vortex' WHERE entry=24136;
UPDATE creature_template SET ScriptName='npc_amanishi_hatcher' WHERE entry IN (23818,24504);
UPDATE creature_template SET ScriptName='npc_forest_frog' WHERE entry=24396;
UPDATE creature_template SET ScriptName='npc_amanishi_lookout' WHERE entry=24175;
UPDATE creature_template SET ScriptName='npc_amanishi_tempest' WHERE entry=24549;
UPDATE creature_template SET ScriptName='npc_harkor' WHERE entry=23999;
UPDATE creature_template SET ScriptName='npc_tanzar' WHERE entry=23790;
UPDATE creature_template SET ScriptName='npc_kraz' WHERE entry=24024;
UPDATE creature_template SET ScriptName='npc_ashli' WHERE entry=24001;
UPDATE creature_template SET ScriptName='npc_amanishi_scout' WHERE entry=23586;
UPDATE gameobject_template SET ScriptName='go_wooden_door' WHERE entry=186306;
UPDATE creature_template SET ScriptName='npc_eagle_trash_aggro_trigger' WHERE entry IN(24223);

/* ZUL'DRAK */
UPDATE creature_template SET ScriptName='npc_gurgthock' WHERE entry=30007;
UPDATE creature_template SET ScriptName='npc_ghoul_feeding_bunny' WHERE entry=28591;
UPDATE creature_template SET ScriptName='npc_decaying_ghoul' WHERE entry=28565;
INSERT INTO scripted_areatrigger VALUES
(5056,'at_overlord_drakuru'),
(5057,'at_overlord_drakuru'),
(5058,'at_overlord_drakuru'),
(5059,'at_overlord_drakuru'),
(5060,'at_overlord_drakuru'),
(5095,'at_overlord_drakuru'),
(5096,'at_overlord_drakuru'),
(5097,'at_overlord_drakuru'),
(5098,'at_overlord_drakuru');
UPDATE creature_template SET ScriptName='npc_argent_stand_unit' WHERE entry IN(28028,28029);


/* ZUL'FARRAK */
UPDATE instance_template SET ScriptName='instance_zulfarrak' WHERE map=209;
INSERT INTO scripted_event_id VALUES
(2488,'event_go_zulfarrak_gong'),
(2609,'event_spell_unlocking');
INSERT INTO scripted_areatrigger VALUES
(1447,'at_zulfarrak');
UPDATE creature_template SET ScriptName='boss_zumrah' WHERE entry=7271;

/* ZUL'GURUB */
UPDATE instance_template SET ScriptName='instance_zulgurub' WHERE map=309;
UPDATE creature_template SET ScriptName='boss_jeklik' WHERE entry=14517;
UPDATE creature_template SET ScriptName='boss_venoxis' WHERE entry=14507;
UPDATE creature_template SET ScriptName='boss_marli' WHERE entry=14510;
UPDATE creature_template SET ScriptName='boss_mandokir' WHERE entry=11382;
UPDATE creature_template SET ScriptName='mob_ohgan' WHERE entry=14988;
UPDATE creature_template SET ScriptName='boss_hakkar' WHERE entry=14834;
UPDATE creature_template SET ScriptName='boss_thekal' WHERE entry=14509;
UPDATE creature_template SET ScriptName='boss_arlokk' WHERE entry=14515;
UPDATE gameobject_template SET ScriptName='go_gong_of_bethekk' WHERE entry=180526;
UPDATE creature_template SET ScriptName='boss_renataki' WHERE entry=15084;
UPDATE creature_template SET ScriptName='mob_zealot_lorkhan' WHERE entry=11347;
UPDATE creature_template SET ScriptName='mob_zealot_zath' WHERE entry=11348;
UPDATE creature_template SET ScriptName='npc_gurubashi_bat_rider' WHERE entry=14750;
UPDATE creature_template SET ScriptName='npc_zulian_prowler' WHERE entry=15101;
UPDATE creature_template SET ScriptName='npc_soulflayer' WHERE entry=11359;



--
-- Below contains data for table `script_texts` mainly used in C++ parts.
-- valid entries for table are between -1000000 and -1999999
--

TRUNCATE script_texts;

--
-- -1 000 000 First 100 entries are reserved for special use, do not add regular text here.
--

INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1000000','<ScriptDev2 Text Entry Missing!>','0','0','0','0','0','DEFAULT_TEXT'),
('-1000001','%s goes into a killing frenzy!','0','2','0','0','9764','EMOTE_GENERIC_FRENZY_KILL'),
('-1000002','%s goes into a frenzy!','0','2','0','0','10677','EMOTE_GENERIC_FRENZY'),
('-1000003','%s becomes enraged!','0','2','0','0','7798','EMOTE_GENERIC_ENRAGED'),
('-1000004','%s goes into a berserker rage!','0','2','0','0','11694','EMOTE_GENERIC_BERSERK'),
('-1000005','%s goes into a frenzy!','0','3','0','0','10677','EMOTE_BOSS_GENERIC_FRENZY'),
('-1000006','%s becomes enraged!','0','3','0','0','7798','EMOTE_BOSS_GENERIC_ENRAGED'),
('-1000007','%s attempts to run away in fear!','0','2','0','0','1150','EMOTE_GENERIC_FLEE');

--
-- Normal text entries below. Say/Yell/Whisper/Emote for any regular world object.
-- Text entries for text used by creatures in instances are using map id as part of entry.
-- Example: for map 430, the database text entry for all creatures normally on this map start with -1430
-- Values decrement as they are made.
-- For creatures outside instance, use -1 000 100 and below. Decrement value as they are made.
--
-- Comment should contain a unique name that can be easily identified later by using sql queries like for example
-- SELECT * FROM script_texts WHERE comment LIKE '%defias%';
-- Place the define used in script itself at the end of the comment.
--
-- Do not change entry without a very good reason. Localization projects depends on using entries that does not change!
-- Example: UPDATE script_texts SET content_loc1 = 'the translated text' WHERE entry = -1000100;
--

-- -1 000 100 GENERAL MAPS (not instance maps)
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1000100','Come, little ones. Face me!','0','1','0','0','9071','azuregos SAY_TELEPORT'),

('-1000101','Follow me, $N. I''ll take you to the Defias hideout. But you better protect me or I am as good as dead','0','0','7','0','9','defias traitor SAY_START'),
('-1000102','The entrance is hidden here in Moonbrook. Keep your eyes peeled for thieves. They want me dead.','0','0','7','0','10','defias traitor SAY_PROGRESS'),
('-1000103','You can go tell Stoutmantle this is where the Defias Gang is holed up, $N.','0','0','7','0','11','defias traitor SAY_END'),
('-1000104','$N coming in fast! Prepare to fight!','0','0','7','0','489','defias traitor SAY_AGGRO_1'),
('-1000105','Help!','0','0','7','0','1277','defias traitor SAY_AGGRO_2'),

('-1000106','Everyone ready?','0','0','1','0','8284','torek SAY_READY'),
('-1000107','Ok, Lets move out!','0','0','1','0','8278','torek SAY_MOVE'),
('-1000108','Prepare yourselves. Silverwing is just around the bend.','0','0','1','0','8282','torek SAY_PREPARE'),
('-1000109','Silverwing is ours!','0','0','1','0','8280','torek SAY_WIN'),
('-1000110','Go report that the outpost is taken. We will remain here.','0','0','1','0','8281','torek SAY_END'),

('-1000111','Our house is this way, through the thicket.','0','0','7','1','13744','magwin SAY_START'),
('-1000112','Help me!','0','0','7','0','27791','magwin SAY_AGGRO'),
('-1000113','My poor family. Everything has been destroyed.','0','0','7','5','13745','magwin SAY_PROGRESS'),
('-1000114','Father! Father! You''re alive!','0','0','7','22','13746','magwin SAY_END1'),
('-1000115','You can thank $N for getting me back here safely, father.','0','0','7','1','13748','magwin SAY_END2'),
('-1000116','%s hugs her father.','0','2','7','5','13747','magwin EMOTE_HUG'),

('-1000117','Thank you for agreeing to help. Now, let''s get out of here $N.','0','0','1','0','13289','wounded elf SAY_ELF_START'),
('-1000118','Over there! They''re following us!','0','0','1','0','13255','wounded elf SAY_ELF_SUMMON1'),
('-1000119','Allow me a moment to rest. The journey taxes what little strength I have.','0','0','1','16','13290','wounded elf SAY_ELF_RESTING'),
('-1000120','Did you hear something?','0','0','1','0','492','wounded elf SAY_ELF_SUMMON2'),
('-1000121','Falcon Watch, at last! Now, where''s my... Oh no! My pack, it''s missing! Where has -','0','0','1','0','13292','wounded elf SAY_ELF_COMPLETE'),
('-1000122','You won''t keep me from getting to Falcon Watch!','0','0','1','0','13659','wounded elf SAY_ELF_AGGRO'),

('-1000123','Ready when you are, $c.','0','0','0','15','61828','big will SAY_BIG_WILL_READY'),
('-1000124','The Affray has begun. $n, get ready to fight!','0','0','0','0','2301','twiggy SAY_TWIGGY_BEGIN'),
('-1000125','You! Enter the fray!','0','0','0','0','2318','twiggy SAY_TWIGGY_FRAY'),
('-1000126','Challenger is down!','0','0','0','0','2355','twiggy SAY_TWIGGY_DOWN'),
('-1000127','The Affray is over.','0','0','0','0','2320','twiggy SAY_TWIGGY_OVER'),

('-1000128','We need you to send reinforcements to Manaforge Duro, Ardonis. This is not a request, it''s an order.','0','0','0','0','18165','dawnforge SAY_COMMANDER_DAWNFORGE_1'),
('-1000129','You cannot be serious! We are severely understaffed and can barely keep this manaforge functional!','0','0','0','0','18166','dawnforge SAY_ARCANIST_ARDONIS_1'),
('-1000130','You will do as ordered. Manaforge Duro has come under heavy attack by mana creatures and the situation is out of control. Failure to comply will not be tolerated!','0','0','0','0','18167','dawnforge SAY_COMMANDER_DAWNFORGE_2'),
('-1000131','Indeed, it is not a request.','0','0','0','0','19185','dawnforge SAY_PATHALEON_CULATOR_IMAGE_1'),
('-1000132','My lord!','0','0','0','0','19186','dawnforge SAY_COMMANDER_DAWNFORGE_3'),
('-1000133','Duro will be reinforced! Ultris was a complete disaster. I will NOT have that mistake repeated!','0','0','0','0','19187','dawnforge PATHALEON_CULATOR_IMAGE_2'),
('-1000134','We''ve had too many setbacks along the way: Hellfire Citadel, Fallen Sky Ridge, Firewing Point... Prince Kael''thas will tolerate no further delays. I will tolerate nothing other than complete success!','0','0','0','0','19188','dawnforge PATHALEON_CULATOR_IMAGE_2_1'),
('-1000135','I am returning to Tempest Keep. See to it that I do not have reason to return!','0','0','0','0','19191','dawnforge PATHALEON_CULATOR_IMAGE_2_2'),
('-1000136','Yes, my lord.','0','0','0','0','19189','dawnforge COMMANDER_DAWNFORGE_4 SAY_ARCANIST_ARDONIS_2'),
('-1000137','See to it, Ardonis!','0','0','0','0','19190','dawnforge COMMANDER_DAWNFORGE_5'),

('-1000138','Prepare to enter oblivion, meddlers. You have unleashed a god!','0','1','0','15','18443','Salhadaar SAY'),
('-1000139','Avruu''s magic is broken! I''m free once again!','0','0','0','0','13447','aeranas SAY_FREE'),

('-1000140','Let''s go.','0','0','1','0','15894','lilatha SAY_START'),
('-1000141','$N, let''s use the antechamber to the right.','0','0','1','0','12187','lilatha SAY_PROGRESS1'),
('-1000142','I can see the light at the end of the tunnel!','0','0','1','0','12188','lilatha SAY_PROGRESS2'),
('-1000143','There''s Farstrider Enclave now, $C. Not far to go... Look out! Troll ambush!!','0','0','1','0','12189','lilatha SAY_PROGRESS3'),
('-1000144','Thank you for saving my life and bringing me back to safety, $N','0','0','1','0','12191','lilatha SAY_END1'),
('-1000145','Captain Helios, I''ve been rescued from the Amani Catacombs. Reporting for duty, sir!','0','0','1','0','12190','lilatha SAY_END2'),
('-1000146','Liatha, get someone to look at those injuries. Thank you for bringing her back safely.','0','0','1','0','12193','lilatha CAPTAIN_ANSWER'),

('-1000147','I remember well the sting of defeat at the conclusion of the Third War. I have waited far too long for my revenge. Now the shadow of the Legion falls over this world. It is only a matter of time until all of your failed creation... is undone.','11332','1','0','0','20076','kazzak SAY_INTRO'),
('-1000148','The Legion will conquer all!','11333','1','0','0','20077','kazzak SAY_AGGRO1'),
('-1000149','All mortals will perish!','11334','1','0','0','20078','kazzak SAY_AGGRO2'),
('-1000150','All life must be eradicated!','11335','1','0','0','20079','kazzak SAY_SURPREME1'),
('-1000151','I''ll rip the flesh from your bones!','11336','1','0','0','20080','kazzak SAY_SURPREME2'),
('-1000152','Kirel Narak!','11337','1','0','0','20081','kazzak SAY_KILL1'),
('-1000153','Contemptible wretch!','11338','1','0','0','20082','kazzak SAY_KILL2'),
('-1000154','The universe will be remade.','11339','1','0','0','20083','kazzak SAY_KILL3'),
('-1000155','The Legion... will never... fall.','11340','1','0','0','20084','kazzak SAY_DEATH'),

('-1000156','Bloodmaul Brew? Me favorite!','0','0','0','0','18171','bladespire ogre SAY_BREW_1'),

('-1000157','Invaders, you dangle upon the precipice of oblivion! The Burning Legion comes and with it comes your end.','0','1','0','0','15895','kazzak SAY_RAND1'),
('-1000158','Impudent whelps, you only delay the inevitable. Where one has fallen, ten shall rise. Such is the will of Kazzak...','0','1','0','0','16432','kazzak SAY_RAND2'),

('-1000159','Do not proceed. You will be eliminated.','11344','1','0','0','20941','doomwalker SAY_AGGRO'),
('-1000160','Tectonic disruption commencing.','11345','1','0','0','20942','doomwalker SAY_EARTHQUAKE_1'),
('-1000161','Magnitude set. Release.','11346','1','0','0','20943','doomwalker SAY_EARTHQUAKE_2'),
('-1000162','Trajectory locked.','11347','1','0','0','20944','doomwalker SAY_OVERRUN_1'),
('-1000163','Engage maximum speed.','11348','1','0','0','20945','doomwalker SAY_OVERRUN_2'),
('-1000164','Threat level zero.','11349','1','0','0','20946','doomwalker SAY_SLAY_1'),
('-1000165','Directive accomplished.','11350','1','0','0','20947','doomwalker SAY_SLAY_2'),
('-1000166','Target exterminated.','11351','1','0','0','20948','doomwalker SAY_SLAY_3'),
('-1000167','System failure in five, f-o-u-r...','11352','1','0','0','20949','doomwalker SAY_DEATH'),

('-1000168','Who dares awaken Aquementas?','0','1','0','0','5465','aquementas AGGRO_YELL_AQUE'),

('-1000169','Muahahahaha! You fool! You''ve released me from my banishment in the interstices between space and time!','0','1','0','0','19468','nether_drake SAY_NIHIL_1'),
('-1000170','All of Draenor shall quick beneath my feet! I will destroy this world and reshape it in my image!','0','1','0','0','19469','nether_drake SAY_NIHIL_2'),
('-1000171','Where shall I begin? I cannot bother myself with a worm such as yourself. There is a world to be conquered!','0','1','0','0','19470','nether_drake SAY_NIHIL_3'),
('-1000172','No doubt the fools that banished me are long dead. I shall take wing survey my demesne. Pray to whatever gods you hold dear that we do not meet again.','0','1','0','0','19471','nether_drake SAY_NIHIL_4'),
('-1000173','NOOOOooooooo!','0','1','0','0','27371','nether_drake SAY_NIHIL_INTERRUPT'),

('-1000174','Good, $n, you are under the spell''s influence. I must analyze it quickly, then we can talk.','0','0','0','0','19158','daranelle SAY_SPELL_INFLUENCE'),

('-1000175','Thank you, mortal.','0','0','11','0','19849',' SAY_JUST_EATEN'),

('-1000176','What is this? Where am I? How... How did I... Cursed ethereals! I was on an escort mission out of Shattrath when we were ambushed! I must return to the city at once! Your name will be recorded as a friend of the Lower City, $n.','0','0','0','5','20438','SAY_LC'),
('-1000177','Blessed Light! Free of my confines at last... Thank you, hero. A''dal will be told of your heroics.','0','0','0','5','20439','SAY_SHAT'),
('-1000178','At last I am released! Thank you, gentle $r. I must return to the expedition at once! They will know of your deeds. This I vow.','0','0','0','5','20435','SAY_CE'),
('-1000179','I will tell the tale of your heroics to Haramad myself! All Consortium will know your name, $n. Thank you, my friend.','0','0','0','5','20436','SAY_CON'),
('-1000180','The Keepers of Time thank you for your assistance. The Timeless One shall be pleased with this outcome...','0','0','0','5','20437','SAY_KT'),
('-1000181','$R save me! Thank you! My people thank you too!','0','0','0','5','20440','SAY_SPOR'),
('-1000182','REUSE ME','0','0','0','0','0','REUSE_ME'),
('-1000183','REUSE ME','0','0','0','0','0','REUSE_ME'),

('-1000184','Daughter!','0','0','7','5','13749','cowlen SAY_DAUGHTER'),

('-1000185','There''s no turning back now, $n. Stay close and watch my back.','0','0','0','1','14648','demolitionist_legoso SAY_ESCORT_START'),
('-1000186','There it is! Do you see where the large red crystal is jutting out from the Vector Coil? That''s where I need to plant the first set of explosives.','0','0','0','25','14649','demolitionist_legoso SAY_ESCORT_1'),

('-1000187','Thank you, dear $C, you just saved my life.','0','0','0','0','0','npc_redemption_target SAY_HEAL'),

('-1000188','Deployment sucessful. Trespassers will be neutralized.','0','0','0','0','24016','converted_sentry SAY_CONVERTED_1'),
('-1000189','Objective acquired. Initiating security routines.','0','0','0','0','24014','converted_sentry SAY_CONVERTED_2'),

('-1000190','In Nagrand, food hunt ogre!','0','0','0','0','15278',' SAY_LUMP_0'),
('-1000191','You taste good with maybe a little salt and pepper.','0','0','0','0','15279',' SAY_LUMP_1'),
('-1000192','OK, OK! Lump give up!','0','0','0','0','15282',' SAY_LUMP_DEFEAT'),

('-1000193','%s looks down at the discarded necklace.  In her sadness, the lady incants a glamour, which beckons forth Highborne spirits.  The chamber resonates with their ancient song about the Sin''dorei...','10896','2','1','0','19197','lady_sylvanas EMOTE_LAMENT_START'),

('-1000194','I give up! Please don''t kill me!','0','0','0','0','15097','unkor SAY_SUBMIT'),

('-1000195','Thank you again, $N. I''ll make my way to the road now. When you can, find Terenthis and let him know we escaped.','0','0','0','1','1236','volcor SAY_ESCAPE'),

('-1000196','Belore...','0','0','1','0','19263','lady_sylvanas SAY_LAMENT_END'),
('-1000197','Sighing, %s kneels down and picks up the amulet.','0','2','1','16','19244','lady_sylvanas EMOTE_LAMENT_END'),

('-1000198','Taste blade, mongrel!','0','0','0','0','10950','SAY_GUARD_SIL_AGGRO1'),
('-1000199','Please tell me that you didn''t just do what I think you just did. Please tell me that I''m not going to have to hurt you...','0','0','0','0','10951','SAY_GUARD_SIL_AGGRO2'),
('-1000200','As if we don''t have enough problems, you go and create more!','0','0','0','0','10953','SAY_GUARD_SIL_AGGRO3'),

('-1000201','I''m saved! Thank you, doctor!','0','0','0','0','8355','injured_patient SAY_DOC1'),
('-1000202','HOORAY! I AM SAVED!','0','0','0','0','8359','injured_patient SAY_DOC2'),
('-1000203','Sweet, sweet embrace... take me...','0','0','0','0','8361','injured_patient SAY_DOC3'),

('-1000204','%s looks up at you quizzically. Maybe you should inspect it?','0','2','0','0','4714','cluck EMOTE_CLUCK_TEXT1'),
('-1000205','%s looks at you unexpectedly.','0','2','0','0','5170','cluck EMOTE_CLUCK_TEXT2'),

('-1000206','REUSE ME','0','0','0','0','0','REUSE ME'),

('-1000207','Mmm. Me thirsty!','0','0','0','0','18172','bladespire ogre SAY_BREW_2'),
('-1000208','Ohh, look! Bloodmaul Brew! Mmmm...','0','0','0','0','18170','bladespire ogre SAY_BREW_3'),

('-1000211','Warning! %s emergency shutdown process initiated by $N. Shutdown will complete in two minutes.','0','2','0','0','18155','manaforge_control EMOTE_START'),
('-1000212','Emergency shutdown will complete in one minute.','0','2','0','0','18156','manaforge_control EMOTE_60'),
('-1000213','Emergency shutdown will complete in thirty seconds.','0','2','0','0','17884','manaforge_control EMOTE_30'),
('-1000214','Emergency shutdown will complete in ten seconds.','0','2','0','0','17885','manaforge_control EMOTE_10'),
('-1000215','Emergency shutdown complete.','0','2','0','0','17900','manaforge_control EMOTE_COMPLETE'),
('-1000216','Emergency shutdown aborted.','0','2','0','0','17886','manaforge_control EMOTE_ABORT'),

('-1000217','Greetings, $N. I will guide you through the cavern. Please try and keep up.','0','4','0','0','17656','WHISPER_CUSTODIAN_1'),
('-1000218','We do not know if the Caverns of Time have always been accessible to mortals. Truly, it is impossible to tell as the Timeless One is in perpetual motion, changing our timeways as he sees fit. What you see now may very well not exist tomorrow. You may wake up and have no memory of this place.','0','4','0','0','17657','WHISPER_CUSTODIAN_2'),
('-1000219','It is strange, I know... Most mortals cannot actually comprehend what they see here, as often, what they see is not anchored within their own perception of reality.','0','4','0','0','17658','WHISPER_CUSTODIAN_3'),
('-1000220','Follow me, please.','0','4','0','0','17668','WHISPER_CUSTODIAN_4'),
('-1000221','There are only two truths to be found here: First, that time is chaotic, always in flux, and completely malleable and second, perception does not dictate reality.','0','4','0','0','17667','WHISPER_CUSTODIAN_5'),
('-1000222','As custodians of time, we watch over and care for Nozdormu''s realm. The master is away at the moment, which means that attempts are being made to dramatically alter time. The master never meddles in the affairs of mortals but instead corrects the alterations made to time by others. He is reactionary in this regard.','0','4','0','0','17659','WHISPER_CUSTODIAN_6'),
('-1000223','For normal maintenance of time, the Keepers of Time are sufficient caretakers. We are able to deal with most ordinary disturbances. I speak of little things, such as rogue mages changing something in the past to elevate their status or wealth in the present.','0','4','0','0','17660','WHISPER_CUSTODIAN_7'),
('-1000224','These tunnels that you see are called timeways. They are infinite in number. The ones that currently exist in your reality are what the master has deemed as ''trouble spots.'' These trouble spots may differ completely in theme but they always share a cause. That is, their existence is a result of the same temporal disturbance. Remember that should you venture inside one...','0','4','0','0','17661','WHISPER_CUSTODIAN_8'),
('-1000225','This timeway is in great disarray! We have agents inside right now attempting to restore order. What information I have indicates that Thrall''s freedom is in jeopardy. A malevolent organization known as the Infinite Dragonflight is trying to prevent his escape. I fear without outside assistance, all will be lost.','0','4','0','0','17663','WHISPER_CUSTODIAN_9'),
('-1000226','We have very little information on this timeway. Sa''at has been dispatched and is currently inside. The data we have gathered from his correspondence is that the Infinite Dragonflight are once again attempting to alter time. Could it be that the opening of the Dark Portal is being targeted for sabotage? Let us hope not...','0','4','0','0','17664','WHISPER_CUSTODIAN_10'),
('-1000227','This timeway is currently collapsing. What that may hold for the past, present and future is currently unknown...','0','4','0','0','0','WHISPER_CUSTODIAN_11'),
('-1000228','The timeways are currently ranked in order from least catastrophic to most catastrophic. Note that they are all classified as catastrophic, meaning that any single one of these timeways collapsing would mean that your world would end. We only classify them in such a way so that the heroes and adventurers that are sent here know which timeway best suits their abilities.','0','4','0','0','17662','WHISPER_CUSTODIAN_12'),
('-1000229','All we know of this timeway is that it leads to Mount Hyjal. The Infinite Dragonflight have gone to great lengths to prevent our involvement. We know next to nothing, mortal. Soridormi is currently attempting to break through the timeway''s defenses but has thus far been unsuccessful. You might be our only hope of breaking through and resolving the conflict.','0','4','0','0','17665','WHISPER_CUSTODIAN_13'),
('-1000230','Our time is at an end $N. I would wish you luck, if such a thing existed.','0','4','0','0','17666','WHISPER_CUSTODIAN_14'),

('-1000231','Ah, $GPriest:Priestess; you came along just in time. I appreciate it.','0','0','0','20','7668','garments SAY_COMMON_HEALED'),
('-1000232','Thank you! Thank you, $GPriest:Priestess;. Now I can take on those gnolls with your power to back me!','0','0','1','4','7792','garments SAY_DG_KEL_THANKS'),
('-1000233','Farewell to you, and may shadow always protect you!','0','0','1','3','7793','garments SAY_DG_KEL_GOODBYE'),

('-1000234','Follow me, stranger.  This won''t take long.','0','0','0','0','17239','SAY_KHAD_SERV_0'),
('-1000235','Shattrath was once the draenei capital of this world. Its name means "dwelling of light."','0','4','0','0','17240','SAY_KHAD_SERV_1'),
('-1000236','When the Burning Legion turned the orcs against the draenei, the fiercest battle was fought here. The draenei fought tooth and nail, but in the end the city fell.','0','4','0','0','17268','SAY_KHAD_SERV_2'),
('-1000237','The city was left in ruins and darkness... until the Sha''tar arrived.','0','4','0','0','17269','SAY_KHAD_SERV_3'),
('-1000238','Let us go into the Lower City.  I will warn you that as one of the only safe havens in Outland, Shattrath has attracted droves of refugees from all wars, current and past.','0','4','0','0','17270','SAY_KHAD_SERV_4'),
('-1000239','The Sha''tar, or "born from light" are the naaru that came to Outland to fight the demons of the Burning Legion.','0','4','0','0','17242','SAY_KHAD_SERV_5'),
('-1000240','They were drawn to the ruins of Shattrath City where a small remnant of the draenei priesthood conducted its rites inside a ruined temple on this very spot.','0','4','0','0','17271','SAY_KHAD_SERV_6'),
('-1000241','The priesthood, known as the Aldor, quickly regained its strength as word spread that the naaru had returned and reconstruction soon began.  The ruined temple is now used as an infirmary for injured refugees.','0','4','0','0','17272','SAY_KHAD_SERV_7'),
('-1000242','It wouldn''t be long, however, before the city came under attack once again.  This time, the attack came from Illidan''s armies. A large regiment of blood elves had been sent by Illidan''s ally, Kael''thas Sunstrider, to lay waste to the city.','0','4','0','0','17245','SAY_KHAD_SERV_8'),
('-1000243','As the regiment of blood elves crossed this very bridge, the Aldor''s exarchs and vindicators lined up to defend the Terrace of Light.  But then the unexpected happened.','0','4','0','0','17274','SAY_KHAD_SERV_9'),
('-1000244','The blood elves laid down their weapons in front of the city''s defenders; their leader, a blood elf elder known as Voren''thal, stormed into the Terrace of Light and demanded to speak to A''dal.','0','4','0','0','17247','SAY_KHAD_SERV_10'),
('-1000245','As the naaru approached him, Voren''thal kneeled before him and uttered the following words: "I''ve seen you in a vision, naaru. My race''s only hope for survival lies with you. My followers and I are here to serve you."','0','4','0','0','17275','SAY_KHAD_SERV_11'),
('-1000246','The defection of Voren''thal and his followers was the largest loss ever incurred by Kael''s forces. And these weren''t just any blood elves.  Many of the best and brightest amongst Kael''s scholars and magisters had been swayed by Voren''thal''s influence.','0','4','0','0','17250','SAY_KHAD_SERV_12'),
('-1000247','The naaru accepted the defectors, who would be known as the Scryers; their dwelling lies in the platform above.  Only those initiated with the Scryers are allowed there.','0','4','0','0','17276','SAY_KHAD_SERV_13'),
('-1000248','The Aldor are followers of the Light and forgiveness and redemption are values they understand. However, they found hard to forget the deeds of the blood elves while under Kael''s command.','0','4','0','0','17253','SAY_KHAD_SERV_14'),
('-1000249','Many of the priesthood had been slain by the same magisters who now vowed to serve the naaru.  They were not happy to share the city with their former enemies.','0','4','0','0','17277','SAY_KHAD_SERV_15'),
('-1000250','The Aldor''s most holy temple and its surrounding dwellings lie on the terrace above.  As a holy site, only the initiated are welcome inside.','0','4','0','0','17278','SAY_KHAD_SERV_16'),
('-1000251','The attacks against Shattrath continued, but the city did not fall, as you can see. On the contrary, the naaru known as Xi''ri led a successful incursion into Shadowmoon Valley - Illidan''s doorstep.','0','4','0','0','17257','SAY_KHAD_SERV_17'),
('-1000252','There he continues to wage war on Illidan with the assistance of the Aldor and the Scryers.  The two factions have not given up on their old feuds, though.','0','4','0','0','17279','SAY_KHAD_SERV_18'),
('-1000253','Such is their animosity that they vie for the honor of being sent to assist the naaru there. Each day, that decision is made here by A''dal.  The armies gather here to receive A''dal''s blessing before heading to Shadowmoon.','0','4','0','0','17280','SAY_KHAD_SERV_19'),
('-1000254','Khadgar should be ready to see you again.  Just remember that to serve the Sha''tar you will most likely have to ally with the Aldor or the Scryers. And seeking the favor of one group will cause the others'' dislike.','0','4','0','0','17262','SAY_KHAD_SERV_20'),
('-1000255','Good luck stranger, and welcome to Shattrath City.','0','4','0','0','17281','SAY_KHAD_SERV_21'),

('-1000256','Thank you! Thank you, $GPriest:Priestess;. Now I can take on those murlocs with the Light on my side!','0','0','7','4','7670','garments SAY_ROBERTS_THANKS'),
('-1000257','Farewell to you, and may the Light be with you always.','0','0','7','3','7789','garments SAY_ROBERTS_GOODBYE'),
('-1000258','Thank you! Thank you, $GPriest:Priestess;. Now I can take on those humans with your power to back me!','0','0','1','4','7784','garments SAY_KORJA_THANKS'),
('-1000259','Farewell to you, and may our ancestors be with you always!','0','0','1','3','7785','garments SAY_KORJA_GOODBYE'),
('-1000260','Thank you! Thank you, $GPriest:Priestess;. Now I can take on those wendigo with the Light on my side!','0','0','7','4','7788','garments SAY_DOLF_THANKS'),
('-1000261','Farewell to you, and may the Light be with you always.','0','0','7','3','7789','garments SAY_DOLF_GOODBYE'),
('-1000262','Thank you! Thank you, $GPriest:Priestess;. Now I can take on those corrupt timberlings with Elune''s power behind me!','0','0','2','4','7780','garments SAY_SHAYA_THANKS'),
('-1000263','Farewell to you, and may Elune be with you always.','0','0','2','3','7781','garments SAY_SHAYA_GOODBYE'),

('-1000264','Ok, $N. Follow me to the cave where I''ll attempt to harness the power of the rune stone into these goggles.','0','0','0','1','845','phizzlethorpe SAY_PROGRESS_1'),
('-1000265','I discovered this cave on our first day here. I believe the energy in the stone can be used to our advantage.','0','0','0','1','846','phizzlethorpe SAY_PROGRESS_2'),
('-1000266','I''ll begin drawing energy from the stone. Your job, $N, is to defend me. This place is cursed... trust me.','0','0','0','1','847','phizzlethorpe SAY_PROGRESS_3'),
('-1000267','%s begins tinkering with the goggles before the stone.','0','2','0','0','848','phizzlethorpe EMOTE_PROGRESS_4'),
('-1000268','Help!!! Get these things off me so I can get my work done!','0','0','0','0','859','phizzlethorpe SAY_AGGRO'),
('-1000269','Almost done! Just a little longer!','0','0','0','1','849','phizzlethorpe SAY_PROGRESS_5'),
('-1000270','I''ve done it! I have harnessed the power of the stone into the goggles! Let''s get out of here!','0','0','0','1','850','phizzlethorpe SAY_PROGRESS_6'),
('-1000271','Phew! Glad to be back from that creepy cave.','0','0','0','1','851','phizzlethorpe SAY_PROGRESS_7'),
('-1000272','%s hands one glowing goggles over to Doctor Draxlegauge.','0','2','0','0','889','phizzlethorpe EMOTE_PROGRESS_8'),
('-1000273','Doctor Draxlegauge will give you further instructions, $N. Many thanks for your help!','0','0','0','1','890','phizzlethorpe SAY_PROGRESS_9'),

('-1000274','Time to teach you a lesson in manners, little $Gboy:girl;!','0','0','0','5','17353','larry SAY_START'),
('-1000275','Now I''m gonna give you to the count of ''3'' to get out of here before I sick the dogs on you.','0','0','0','1','17354','larry SAY_COUNT'),
('-1000276','1...','0','0','0','0','17355','larry SAY_COUNT_1'),
('-1000277','2...','0','0','0','0','17356','larry SAY_COUNT_2'),
('-1000278','Time to meet your maker!','0','0','0','0','17357','larry SAY_ATTACK_5'),
('-1000279','Alright, we give up! Don''t hurt us!','0','0','0','20','17361','larry SAY_GIVEUP'),

('-1000280','A shadowy, sinister presence has invaded the Emerald Dream. Its power is poised to spill over into our world, $N. We must oppose it! That''s why I cannot accompany you in person.','0','0','0','1','20559','clintar SAY_START'),
('-1000281','The Emerald Dream will never be yours!','0','0','0','0','20568','clintar SAY_AGGRO_1'),
('-1000282','Begone from this place!','0','0','0','0','20567','clintar SAY_AGGRO_2'),
('-1000283','That''s the first relic, but there are still two more. Follow me, $N.','0','0','0','0','20563','clintar SAY_RELIC1'),
('-1000284','I''ve recovered the second relic. Take a moment to rest, and then we''ll continue to the last reliquary.','0','0','0','0','20564','clintar SAY_RELIC2'),
('-1000285','We have all three of the relics, but my energy is rapidly fading. We must make our way back to Dreamwarden Lurosa! He will let you know what to do next.','0','0','0','0','20596','clintar SAY_RELIC3'),
('-1000286','Lurosa, I am entrusting the Relics of Aviana to $N, who will take them to Morthis Whisperwing. I must return completely to the Emerald Dream now. Do not let $N fail!','0','0','0','1','20601','clintar SAY_END'),

('-1000287','Emergency power activated! Initializing ambulatory motor! CLUCK!','0','0','0','0','3794','oox SAY_OOX_START'),
('-1000288','Physical threat detected! Evasive action! CLUCK!','0','0','0','0','3795','oox SAY_OOX_AGGRO1'),
('-1000289','Threat analyzed! Activating combat plan beta! CLUCK!','0','0','0','0','3796','oox SAY_OOX_AGGRO2'),
('-1000290','CLUCK! Sensors detect spatial anomaly - danger imminent! CLUCK!','0','0','0','0','3822','oox SAY_OOX_AMBUSH'),
('-1000291','No one challenges the Wastewander nomads - not even robotic chickens! ATTACK!','0','0','0','0','3812','oox SAY_OOX17_AMBUSH_REPLY'),
('-1000292','Cloaking systems online! CLUCK! Engaging cloak for transport to Booty Bay!','0','0','0','0','3816','oox SAY_OOX_END'),

('-1000293','To the house! Stay close to me, no matter what! I have my gun and ammo there!','0','0','7','0','2360','stilwell SAY_DS_START'),
('-1000294','We showed that one!','0','0','7','0','5269','stilwell SAY_WAVE_DOWN_1'),
('-1000295','One more down!','0','0','7','0','2369','stilwell SAY_WAVE_DOWN_2'),
('-1000296','We''ve done it! We won!','0','0','7','0','2358','stilwell SAY_DS_DOWN_3'),
('-1000297','Meet me down by the orchard--I just need to put my gun away.','0','0','7','0','3090','stilwell SAY_DS_PROLOGUE'),

('-1000298','Alright, alright I think I can figure out how to operate this thing...','0','0','0','393','1031','wizzlecrank SAY_START'),
('-1000299','Arrrgh! This isn''t right!','0','0','0','0','1039','wizzlecrank SAY_STARTUP1'),
('-1000300','Okay, I think I''ve got it, now. Follow me, $n!','0','0','0','1','1032','wizzlecrank SAY_STARTUP2'),
('-1000301','There''s the stolen shredder! Stop it or Lugwizzle will have our hides!','0','1','0','0','1040','wizzlecrank SAY_MERCENARY'),
('-1000302','Looks like we''re out of woods, eh? Wonder what this does...','0','0','0','0','1033','wizzlecrank SAY_PROGRESS_1'),
('-1000303','Come on, don''t break down on me now!','0','0','0','393','1043','wizzlecrank SAY_PROGRESS_2'),
('-1000304','That was a close one! Well, let''s get going, it''s still a ways to Ratchet!','0','0','0','0','1041','wizzlecrank SAY_PROGRESS_3'),
('-1000305','Hmm... I don''t think this blinking red light is a good thing...','0','0','0','0','1044','wizzlecrank SAY_END'),

('-1000306','Let''s get to the others, and keep an eye open for those wolves cutside...','0','0','1','0','481','erland SAY_START_1'),
('-1000307','Be careful, $N. Those wolves like to hide among the trees.','0','0','1','0','482','erland SAY_START_2'),
('-1000308','A $C attacks!','0','0','1','0','543','erland SAY_AGGRO_1'),
('-1000309','Beware! I am under attack!','0','0','1','0','544','erland SAY_AGGRO_2'),
('-1000310','Oh no! A $C is upon us!','0','0','1','0','541','erland SAY_AGGRO_3'),
('-1000311','We''re almost there!','0','0','1','0','483','erland SAY_PROGRESS'),
('-1000312','We made it! Thanks, $N. I couldn''t have gotten without you.','0','0','1','0','0','erland SAY_END'),
('-1000313','It''s good to see you again, Erland. What is your report?','0','0','33','1','534','erland SAY_RANE'),
('-1000314','Masses of wolves are to the east, and whoever lived at Malden''s Orchard is gone.','0','0','1','1','535','erland SAY_RANE_REPLY'),
('-1000315','If I am excused, then I''d like to check on Quinn...','0','0','1','1','536','erland SAY_CHECK_NEXT'),
('-1000316','Hello, Quinn. How are you faring?','0','0','1','1','537','erland SAY_QUINN'),
('-1000317','I''ve been better. Ivar the Foul got the better of me...','0','0','33','1','539','erland SAY_QUINN_REPLY'),
('-1000318','Try to take better care of yourself, Quinn. You were lucky this time.','0','0','1','1','538','erland SAY_BYE'),

('-1000319','Let the trial begin, Bloodwrath, attack!','0','1','1','0','14459','kelerun SayId1'),
('-1000320','Champion Lightrend, make me proud!','0','1','1','0','14460','kelerun SayId2'),
('-1000321','Show this upstart how a real Blood Knight fights, Swiftblade!','0','1','1','0','14462','kelerun SayId3'),
('-1000322','Show $n the meaning of pain, Sunstriker!','0','1','1','0','14463','kelerun SayId4'),

('-1000323','Mist! I feared I would never see you again! Yes, I am well, do not worry for me. You must rest and recover your health.','0','0','7','0','1330','mist SAY_AT_HOME'),
('-1000324','%s growls in acknowledgement before straightening and making her way off into the forest.','0','2','0','0','1340','mist EMOTE_AT_HOME'),

('-1000325','"Threshwackonator First Mate unit prepared to follow"','0','2','0','0','3012','threshwackonator EMOTE_START'),
('-1000326','YARRR! Swabie, what have ye done?! He''s gone mad! Baton him down the hatches! Hoist the mast! ARRRR! Every man for hi''self!','0','0','7','0','2704','threshwackonator SAY_AT_CLOSE'),

('-1000327','Ok, $n, let''s go find where I left that mysterious fossil. Follow me!','0','0','7','0','925','remtravel SAY_REM_START'),
('-1000328','Now where did I put that mysterious fossil? Ah, maybe up there...','0','0','7','0','926','remtravel SAY_REM_RAMP1_1'),
('-1000329','Hrm, nothing up here.','0','0','7','0','927','remtravel SAY_REM_RAMP1_2'),
('-1000330','No mysterious fossil here... Ah, but my copy of Green Hills of Stranglethorn. What a good book!','0','0','7','0','928','remtravel SAY_REM_BOOK'),
('-1000331','I bet you I left it in the tent!','0','0','7','0','929','remtravel SAY_REM_TENT1_1'),
('-1000332','Oh wait, that''s Hollee''s tent... and it''s empty.','0','0','7','0','930','remtravel SAY_REM_TENT1_2'),
('-1000333','Interesting... I hadn''t noticed this earlier...','0','0','7','0','931','remtravel SAY_REM_MOSS'),
('-1000334','%s inspects the ancient, mossy stone.','0','2','7','0','932','remtravel EMOTE_REM_MOSS'),
('-1000335','Oh wait! I''m supposed to be looking for that mysterious fossil!','0','0','7','0','933','remtravel SAY_REM_MOSS_PROGRESS'),
('-1000336','Nope. didn''t leave the fossil back here!','0','0','7','0','935','remtravel SAY_REM_PROGRESS'),
('-1000337','Ah. I remember now! I gave the mysterious fossil to Hollee! Check with her.','0','0','7','0','936','remtravel SAY_REM_REMEMBER'),
('-1000338','%s goes back to work, oblivious to everything around him.','0','2','7','0','937','remtravel EMOTE_REM_END'),
('-1000339','Something tells me this $r wants the mysterious fossil too. Help!','0','0','7','0','941','remtravel SAY_REM_AGGRO'),

('-1000340','%s howls in delight at the sight of his lunch!','0','2','0','0','22068','kyle EMOTE_SEE_LUNCH'),
('-1000341','%s eats his lunch.','0','2','0','0','22069','kyle EMOTE_EAT_LUNCH'),
('-1000342','%s thanks you with a special dance.','0','2','0','0','22070','kyle EMOTE_DANCE'),

('-1000343','Pitiful mortal, Onyxia answers to no one!','0','1','0','0','22285','smolderwing SAY_1'),
('-1000344','Your pathetic challenge has not gone unnoticed. I shall enjoy toying with you before you die.','0','1','0','0','22286','smolderwing SAY_2'),

-- REUSE 345-347

('-1000348','Ah...the wondrous sound of kodos. I love the way they make the ground shake... inspect the beast for me.','0','0','0','0','7303','kodo round SAY_SMEED_HOME_1'),
('-1000349','Hey, look out with that kodo! You had better inspect that beast before i give you credit!','0','0','0','0','7301','kodo round SAY_SMEED_HOME_2'),
('-1000350','That kodo sure is a beauty. Wait a minute, where are my bifocals? Perhaps you should inspect the beast for me.','0','0','0','0','7302','kodo round SAY_SMEED_HOME_3'),

('-1000351','You, there! Hand over that moonstone and nobody gets hurt!','0','1','0','0','20715','sprysprocket SAY_START'),
('-1000352','%s takes the Southfury moonstone and escapes into the river. Follow her!','0','3','0','0','20708','sprysprocket EMOTE_START'),
('-1000353','Just chill!','0','4','0','0','21320','sprysprocket SAY_WHISPER_CHILL'),
('-1000354','Stupid grenade picked a fine time to backfire! So much for high quality goblin engineering!','0','1','0','0','21271','sprysprocket SAY_GRENADE_FAIL'),
('-1000355','All right, you win! I surrender! Just don''t hurt me!','0','1','0','0','20720','sprysprocket SAY_END'),

('-1000356','Okay, okay... gimme a minute to rest now. You gone and beat me up good.','0','0','1','14','3043','calvin SAY_COMPLETE'),

('-1000357','Let''s go before they find out I''m free!','0','0','0','1','8327','KAYA_SAY_START'),
('-1000358','Look out! We''re under attack!','0','0','0','0','8328','KAYA_AMBUSH'),
('-1000359','Thank you for helping me. I know my way back from here.','0','0','0','0','8329','KAYA_END'),

('-1000360','The strands of LIFE have been severed! The Dreamers must be avenged!','0','1','0','0','10880',' ysondre SAY_AGGRO'),
('-1000361','Come forth, ye Dreamers - and claim your vengeance!','0','1','0','0','10881',' ysondre SAY_SUMMONDRUIDS'),

('-1000362','Let''s go $N. I am ready to reach Whitereach Post.','0','0','1','0','5648','paoka SAY_START'),
('-1000363','Now this looks familiar. If we keep heading east, I think we can... Ahh, Wyvern on the attack!','0','0','1','0','5654','paoka SAY_WYVERN'),
('-1000364','Thanks a bunch... I can find my way back to Whitereach Post from here. Be sure to talk with Motega Firemane; perhaps you can keep him from sending me home.','0','0','1','0','5683','paoka SAY_COMPLETE'),

('-1000365','Be on guard... Arnak has some strange power over the Grimtotem... they will not be happy to see me escape.','0','0','1','0','5926','lakota SAY_LAKO_START'),
('-1000366','Look out, the Grimtotem are upon us!','0','0','1','0','5927','lakota SAY_LAKO_LOOK_OUT'),
('-1000367','Here they come.','0','0','1','0','56214','lakota SAY_LAKO_HERE_COME'),
('-1000368','More Grimtotems are coming this way!','0','0','1','0','5929','lakota SAY_LAKO_MORE'),
('-1000369','Finally, free at last... I must be going now, thanks for helping me escape. I can get back to Freewind Post by myself.','0','0','1','0','5930','lakota SAY_LAKO_END'),

('-1000370','Stay close, $n. I''ll need all the help I can get to break out of here. Let''s go!','0','0','1','1','1065','gilthares SAY_GIL_START'),
('-1000371','At last! Free from Northwatch Hold! I need a moment to catch my breath...','0','0','1','5','1066','gilthares SAY_GIL_AT_LAST'),
('-1000372','Now I feel better. Let''s get back to Ratchet. Come on, $n.','0','0','1','23','1067','gilthares SAY_GIL_PROCEED'),
('-1000373','Looks like the Southsea Freebooters are heavily entrenched on the coast. This could get rough.','0','0','1','25','1068','gilthares SAY_GIL_FREEBOOTERS'),
('-1000374','Help! $C attacking!','0','0','1','0','1074','gilthares SAY_GIL_AGGRO_1'),
('-1000375','$C heading this way fast! Time for revenge!','0','0','1','0','1075','gilthares SAY_GIL_AGGRO_2'),
('-1000376','$C coming right at us!','0','0','1','0','1072','gilthares SAY_GIL_AGGRO_3'),
('-1000377','Get this $C off of me!','0','0','1','0','1073','gilthares SAY_GIL_AGGRO_4'),
('-1000378','Almost back to Ratchet! Let''s keep up the pace...','0','0','1','0','1069','gilthares SAY_GIL_ALMOST'),
('-1000379','Ah, the sweet salt air of Ratchet.','0','0','1','0','1070','gilthares SAY_GIL_SWEET'),
('-1000380','Captain Brightsun, $N here has freed me! $N, I am certain the Captain will reward your bravery.','0','0','1','66','1071','gilthares SAY_GIL_FREED'),

('-1000381','I sense the tortured spirits, $n. They are this way, come quickly!','0','0','0','1','18668','wilda SAY_WIL_START'),
('-1000382','Watch out!','0','0','0','0','14828','wilda SAY_WIL_AGGRO1'),
('-1000383','Naga attackers! Defend yourself!','0','0','0','0','18688','wilda SAY_WIL_AGGRO2'),
('-1000384','Grant me protection, $n. I must break through their foul magic!','0','0','0','0','18669','wilda SAY_WIL_PROGRESS1'),
('-1000385','The naga of Coilskar are exceptionally cruel to their prisoners. It is a miracle that I survived inside that watery prison for as long as I did. Earthmother be praised.','0','0','0','0','18685','wilda SAY_WIL_PROGRESS2'),
('-1000386','Now we must find the exit.','0','0','0','1','18670','wilda SAY_WIL_FIND_EXIT'),
('-1000387','Lady Vashj must answer for these atrocities. She must be brought to justice!','0','0','0','0','18684','wilda SAY_WIL_PROGRESS4'),
('-1000388','The tumultuous nature of the great waterways of Azeroth and Draenor are a direct result of tormented water spirits.','0','0','0','0','18675','wilda SAY_WIL_PROGRESS5'),
('-1000389','It shouldn''t be much further, $n. The exit is just up ahead.','0','0','0','1','18671','wilda SAY_WIL_JUST_AHEAD'),
('-1000390','Thank you, $n. Please return to my brethren at the Altar of Damnation, near the Hand of Gul''dan, and tell them that Wilda is safe. May the Earthmother watch over you...','0','0','0','3','18672','wilda SAY_WIL_END'),

('-1000391','I''m Thirsty.','0','0','0','0','2222','tooga SAY_TOOG_THIRST'),
('-1000392','Torta must be so worried.','0','0','0','0','2228','tooga SAY_TOOG_WORRIED'),
('-1000393','Torta, my love! I have returned at long last.','0','0','0','0','2137','tooga SAY_TOOG_POST_1'),
('-1000394','You have any idea how long I''ve been waiting here? And where''s dinner? All that time gone and nothing to show for it?','0','0','0','0','2138','tooga SAY_TORT_POST_2'),
('-1000395','My dearest Torta. I have been gone for so long. Finally we are reunited. At long last our love can blossom again.','0','0','0','0','2139','tooga SAY_TOOG_POST_3'),
('-1000396','Enough with the rambling. I am starving! Now, get your dusty shell into that ocean and bring momma some grub.','0','0','0','0','2140','tooga SAY_TORT_POST_4'),
('-1000397','Yes Torta. Whatever your heart desires...','0','0','0','0','2141','tooga SAY_TOOG_POST_5'),
('-1000398','And try not to get lost this time...','0','0','0','0','2145','tooga SAY_TORT_POST_6'),

('-1000399','Peace is but a fleeting dream! Let the NIGHTMARE reign!','0','1','0','0','10886','taerar SAY_AGGRO'),
('-1000400','Children of Madness - I release you upon this world!','0','1','0','0','10887','taerar SAY_SUMMONSHADE'),

('-1000401','Hope is a DISEASE of the soul! This land shall wither and die!','0','1','0','0','10885','emeriss SAY_AGGRO'),
('-1000402','Taste your world''s corruption!','0','1','0','0','10884','emeriss SAY_CASTCORRUPTION'),

('-1000403','Rin''ji is free!','0','0','0','0','3787','SAY_RIN_FREE'),
('-1000404','Attack my sisters! The troll must not escape!','0','0','0','0','3827','SAY_RIN_BY_OUTRUNNER'),
('-1000405','Rin''ji needs help!','0','0','1','0','3862','SAY_RIN_HELP_1'),
('-1000406','Rin''ji is being attacked!','0','0','1','0','3861','SAY_RIN_HELP_2'),
('-1000407','Rin''ji can see road now, $n. Rin''ji knows the way home.','0','0','1','0','3790','SAY_RIN_COMPLETE'),
('-1000408','Rin''ji will tell you secret now... $n, should go to the Overlook Cliffs. Rin''ji hid something on island  there','0','0','1','0','3817','SAY_RIN_PROGRESS_1'),
('-1000409','You find it, you keep it! Don''t tell no one that Rin''ji talked to you!','0','0','1','0','3818','SAY_RIN_PROGRESS_2'),

('-1000411','Come, $N. See what the Nightmare brings...','0','4','0','0','11271','Twilight Corrupter SAY_TWILIGHT_CORRUPTER_SPAWN'),

('-1000412','Kill her! Take the farm!','0','1','0','0','2366','Daphne Stilwell YELL_DEFIAS_START'),
('-1000413','You won''t ruin my lands, you scum!','0','1','0','0','2359','Daphne Stilwell YELL_DAPHNE_START'),
('-1000414','The Light is with us this day!','0','0','0','0','2372','Daphne Stilwell SAY_WAVE_DOWN_3'),

('-1000415','%s, too injured, gives up the chase.','0','2','0','0','8224','hendel EMOTE_SURRENDER'),

('-1000416','Well, I''m not sure how far I''ll make it in this state... I''m feeling kind of faint...','0','0','0','0','5391','ringo SAY_RIN_START_1'),
('-1000417','Remember, if I faint again, the water that Spraggle gave you will revive me.','0','0','0','0','5392','ringo SAY_RIN_START_2'),
('-1000418','The heat... I can''t take it...','0','0','0','0','5396','ringo SAY_FAINT_1'),
('-1000419','Maybe... you could carry me?','0','0','0','0','5397','ringo SAY_FAINT_2'),
('-1000420','Uuuuuuggggghhhhh....','0','0','0','0','5394','ringo SAY_FAINT_3'),
('-1000421','I''m not feeling so well...','0','0','0','0','5395','ringo SAY_FAINT_4'),
('-1000422','Where... Where am I?','0','0','0','0','5400','ringo SAY_WAKE_1'),
('-1000423','I am feeling a little better now, thank you.','0','0','0','0','5399','ringo SAY_WAKE_2'),
('-1000424','Yes, I must go on.','0','0','0','0','5398','ringo SAY_WAKE_3'),
('-1000425','How am I feeling? Quite soaked, thank you.','0','0','0','0','5401','ringo SAY_WAKE_4'),
('-1000426','Spraggle! I didn''t think I''d make it back!','0','0','0','0','5402','ringo SAY_RIN_END_1'),
('-1000427','Ringo! You''re okay!','0','0','0','0','5405','ringo SAY_SPR_END_2'),
('-1000428','Oh... I''m feeling faint...','0','0','0','0','5403','ringo SAY_RIN_END_3'),
('-1000429','%s collapses onto the ground.','0','2','0','0','0','ringo EMOTE_RIN_END_4'),
('-1000430','%s stands up after a short pause.','0','2','0','0','0','ringo EMOTE_RIN_END_5'),
('-1000431','Ugh.','0','0','0','0','5404','ringo SAY_RIN_END_6'),
('-1000432','Ringo? Wake up! Don''t worry, I''ll take care of you.','0','0','0','0','5406','ringo SAY_SPR_END_7'),
('-1000433','%s fades away after a long pause.','0','2','0','0','0','ringo EMOTE_RIN_END_8'),

('-1000434','Liladris has been waiting for me at Maestra''s Post, so we should make haste, $N.','6209','0','0','0','6540','kerlonian SAY_KER_START'),
('-1000435','%s looks very sleepy...','0','2','0','0','6811','kerlonian EMOTE_KER_SLEEP_1'),
('-1000436','%s suddenly falls asleep','0','2','0','0','6542','kerlonian EMOTE_KER_SLEEP_2'),
('-1000437','%s begins to drift off...','0','2','0','0','6541','kerlonian EMOTE_KER_SLEEP_3'),
('-1000438','This looks like the perfect place for a nap...','0','0','0','0','6813','kerlonian SAY_KER_SLEEP_1'),
('-1000439','Yaaaaawwwwwnnnn...','0','0','0','0','6543','kerlonian SAY_KER_SLEEP_2'),
('-1000440','Oh, I am so tired...','0','0','0','0','6544','kerlonian SAY_KER_SLEEP_3'),
('-1000441','You don''t mind if I stop here for a moment, do you?','0','0','0','0','6545','kerlonian SAY_KER_SLEEP_4'),
('-1000442','Be on the alert! The Blackwood furbolgs are numerous in the area...','0','0','0','0','6867','kerlonian SAY_KER_ALERT_1'),
('-1000443','It''s quiet... Too quiet...','0','0','0','0','6868','kerlonian SAY_KER_ALERT_2'),
('-1000444','Oh, I can see Liladris from here... Tell her I''m here, won''t you?','0','0','0','0','6643','kerlonian SAY_KER_END'),
('-1000445','%s wakes up!','6700','2','0','0','6612','kerlonian EMOTE_KER_AWAKEN'),

('-1000446','A-Me good. Good, A-Me. Follow... follow A-Me. Home. A-Me go home.','0','0','0','0','5062','ame01 SAY_AME_START'),
('-1000447','Good... good, A-Me. A-Me good. Home. Find home.','0','0','0','0','5063','ame01 SAY_AME_PROGRESS'),
('-1000448','A-Me home! A-Me good! Good A-Me. Home. Home. Home.','0','0','0','0','5156','ame01 SAY_AME_END'),
('-1000449','$c, no hurt A-Me. A-Me good.','0','0','0','0','5158','ame01 SAY_AME_AGGRO1'),
('-1000450','Danger. Danger! $c try hurt A-Me.','0','0','0','0','5159','ame01 SAY_AME_AGGRO2'),
('-1000451','Bad, $c. $c, bad!','0','0','0','0','5157','ame01 SAY_AME_AGGRO3'),

('-1000452','I noticed some fire on that island over there. A human, too. Let''s go check it out, $n.','0','0','1','0','1669','ogron SAY_OGR_START'),
('-1000453','That''s Reethe alright. Let''s go see what he has to say, yeah?','0','0','1','1','1670','ogron SAY_OGR_SPOT'),
('-1000454','W-what do you want? Just leave me alone...','0','0','0','6','1674','ogron SAY_OGR_RET_WHAT'),
('-1000455','I swear. I didn''t steal anything from you! Here, take some of my supplies, just go away!','0','0','0','27','1675','ogron SAY_OGR_RET_SWEAR'),
('-1000456','Just tell us what you know about the Shady Rest Inn, and I won''t bash your skull in.','0','0','1','0','1707','ogron SAY_OGR_REPLY_RET'),
('-1000457','I... Well, I may of taken a little thing or two from the inn... but what would an ogre care about that?','0','0','0','6','1708','ogron SAY_OGR_RET_TAKEN'),
('-1000458','Look here, if you don''t tell me about the fire--','0','0','1','0','1709','ogron SAY_OGR_TELL_FIRE'),
('-1000459','Not one step closer, ogre!','0','0','0','27','1710','ogron SAY_OGR_RET_NOCLOSER'),
('-1000460','And I don''t know anything about this fire of yours...','0','0','0','0','1711','ogron SAY_OGR_RET_NOFIRE'),
('-1000461','What was that? Did you hear something?','0','0','0','0','1731','ogron SAY_OGR_RET_HEAR'),
('-1000462','Paval Reethe! Found you at last. And consorting with ogres now? No fear, even deserters and traitors are afforded some mercy.','0','0','0','0','1714','ogron SAY_OGR_CAL_FOUND'),
('-1000463','Private, show Lieutenant Reethe some mercy.','0','0','0','29','1720','ogron SAY_OGR_CAL_MERCY'),
('-1000464','Gladly, sir.','0','0','0','0','1721','ogron SAY_OGR_HALL_GLAD'),
('-1000465','%s staggers backwards as the arrow lodges itself deeply in his chest.','0','2','0','0','1732','ogron EMOTE_OGR_RET_ARROW'),
('-1000466','Ugh... Hallan, didn''t think you had it in you...','0','0','0','34','1722','ogron SAY_OGR_RET_ARROW'),
('-1000467','Now, let''s clean up the rest of the trash, men!','0','0','0','0','1723','ogron SAY_OGR_CAL_CLEANUP'),
('-1000468','Damn it! You''d better not die on me, human!','0','0','1','0','1724','ogron SAY_OGR_NODIE'),
('-1000469','Still with us, Reethe?','0','0','1','0','1726','ogron SAY_OGR_SURVIVE'),
('-1000470','Must be your lucky day. Alright, I''ll talk. Just leave me alone. Look, you''re not going to believe me, but it wa... oh, Light, looks like the girl could shoot...','0','0','0','0','1725','ogron SAY_OGR_RET_LUCKY'),
('-1000471','By the way, thanks for watching my back.','0','0','1','0','1730','ogron SAY_OGR_THANKS'),

('-1000472','The %s, having fought the good fight, falls to pieces.','0','2','0','0','18587','Scrap Reaver X6000 - on death anim emote'),
('-1000473','The %s''s mechanical heart begins to beat softly.','0','2','0','0','18271','Scrapped Fel Reaver - on event start emote'),
('-1000474','The naaru''s emergency shutdown will disable the manaforge permanently! It must be aborted!','0','0','0','0','17932','manaforge shutdown'),
('-1000475','Stop this immediately!','0','0','0','0','18131','manaforge shutdown say 2'),
('-1000476','You will not shut my manaforge down, scum!','0','1','0','0','18162','manaforge ara elite yell'),

('-1000477','Let us leave this place. I''ve had enough of these madmen!','0','0','0','0','20161','akuno SAY_AKU_START'),
('-1000478','You''ll go nowhere, fool!','0','0','0','0','20164','akuno SAY_AKU_AMBUSH_A'),
('-1000479','Beware! More cultists come!','0','0','0','0','20162','akuno SAY_AKU_AMBUSH_B'),
('-1000480','You will not escape us so easily!','0','0','0','0','20165','akuno SAY_AKU_AMBUSH_B_REPLY'),
('-1000481','I can find my way from here. My friend Mekeda will reward you for your kind actions.','0','0','0','0','20163','akuno SAY_AKU_COMPLETE'),

('-1000482','Look out!','0','0','0','25','14999','maghar captive SAY_MAG_START'),
('-1000483','Don''t let them escape! Kill the strong one first!','0','0','0','0','15002','maghar captive SAY_MAG_NO_ESCAPE'),
('-1000484','More of them coming! Watch out!','0','0','0','5','15000','maghar captive SAY_MAG_MORE'),
('-1000485','Where do you think you''re going? Kill them all!','0','0','0','0','15003','maghar captive SAY_MAG_MORE_REPLY'),
('-1000486','Ride the lightning, filth!','0','0','0','0','15006','maghar captive SAY_MAG_LIGHTNING'),
('-1000487','FROST SHOCK!!!','0','0','0','0','15009','maghar captive SAY_MAG_SHOCK'),
('-1000488','It is best that we split up now, in case they send more after us. Hopefully one of us will make it back to Garrosh. Farewell stranger.','0','0','0','1','15001','maghar captive SAY_MAG_COMPLETE'),

('-1000489','Show our guest around Shattrath, will you? Keep an eye out for pickpockets in the lower city.','0','0','0','0','17266','SAY_KHAD_START'),
('-1000490','A newcomer to Shattrath! Make sure to drop by later. We can always use a hand with the injured.','0','0','0','0','17282','SAY_KHAD_INJURED'),
('-1000491','Be mindful of what you say, this one''s being shown around by Khadgar''s pet.','0','0','0','0','17284','SAY_KHAD_MIND_YOU'),
('-1000492','Are you joking? I''m a Scryer, I always watch what i say. More enemies than allies in this city, it seems.','0','0','0','0','17285','SAY_KHAD_MIND_ALWAYS'),
('-1000493','Light be with you, $n. And welcome to Shattrath.','0','0','0','0','17286','SAY_KHAD_ALDOR_GREET'),
('-1000494','We''re rather selective of who we befriend, $n. You think you have what it takes?','0','0','0','0','17287','SAY_KHAD_SCRYER_GREET'),
('-1000495','Khadgar himself is showing you around? You must have made a good impression, $n.','0','0','0','0','17283','SAY_KHAD_HAGGARD'),

('-1000496','%s lifts its head into the air, as if listening for something.','0','2','0','0','13430','ancestral wolf EMOTE_WOLF_LIFT_HEAD'),
('-1000497','%s lets out a howl that rings across the mountains to the north and motions for you to follow.','9036','2','0','0','13446','ancestral wolf EMOTE_WOLF_HOWL'),
('-1000498','Welcome, kind spirit. What has brought you to us?','0','0','0','0','13512','ancestral wolf SAY_WOLF_WELCOME'),

('-1000499','Puny $r wanna fight %s? Me beat you! Me boss here!','0','0','1','0','1515','morokk SAY_MOR_CHALLENGE'),
('-1000500','Me scared! Me run now!','0','1','0','0','1523','morokk SAY_MOR_SCARED'),

('-1000501','Are you sure that you are ready? If we do not have a group of your allies to aid us, we will surely fail.','0','0','1','0','8566','muglash SAY_MUG_START1'),
('-1000502','This will be a tough fight, $n. Follow me closely.','0','0','1','0','8555','muglash SAY_MUG_START2'),
('-1000503','This is the brazier, $n. Put it out. Vorsha is a beast, worthy of praise from no one!','0','0','1','0','8556','muglash SAY_MUG_BRAZIER'),
('-1000504','Now we must wait. It won''t be long before the naga realize what we have done.','0','0','1','0','8410','muglash SAY_MUG_BRAZIER_WAIT'),
('-1000505','Be on your guard, $n!','0','0','1','0','8412','muglash SAY_MUG_ON_GUARD'),
('-1000506','Perhaps we will get a moment to rest. But i will not give up until we have faced off against Vorsha!','0','0','1','0','8413','muglash SAY_MUG_REST'),
('-1000507','We have done it!','0','0','1','0','8569','muglash SAY_MUG_DONE'),
('-1000508','You have my deepest gratitude. I thank you.','0','0','1','0','8558','muglash SAY_MUG_GRATITUDE'),
('-1000509','I am going to patrol the area for a while longer and ensure that things are truly safe.','0','0','1','0','8564','muglash SAY_MUG_PATROL'),
('-1000510','Please return to Zoram''gar and report our success to the Warsong runner.','0','0','1','0','8565','muglash SAY_MUG_RETURN'),

('-1000511','Aright, listen up! Form a circle around me and move out!','0','0','0','0','20314','letoll SAY_LE_START'),
('-1000512','Aright, $r, just keep us safe from harm while we work. We''ll pay you when we return.','0','0','0','0','20328','letoll SAY_LE_KEEP_SAFE'),
('-1000513','The dig site is just north of here.','0','0','0','0','20329','letoll SAY_LE_NORTH'),
('-1000514','We''re here! Start diggin''!','0','0','0','0','20330','letoll SAY_LE_ARRIVE'),
('-1000515','I think there''s somethin'' buried here, beneath the sand!','0','0','0','0','20331','letoll SAY_LE_BURIED'),
('-1000516','Almost got it!','0','0','0','0','20332','letoll SAY_LE_ALMOST'),
('-1000517','By brann''s brittle bananas! What is it!? It... It looks like a drum.','0','0','0','0','20333','letoll SAY_LE_DRUM'),
('-1000518','Wow... a drum.','0','0','0','0','20334','letoll SAY_LE_DRUM_REPLY'),
('-1000519','This discovery will surely rock the foundation of modern archaeology.','0','0','0','0','20335','letoll SAY_LE_DISCOVERY'),
('-1000520','Yea, great. Can we leave now? This desert is giving me hives.','0','0','0','0','20336','letoll SAY_LE_DISCOVERY_REPLY'),
('-1000521','Have ye gone mad? You expect me to leave behind a drum without first beatin'' on it? Not this son of Ironforge! No sir!','0','0','0','0','20337','letoll SAY_LE_NO_LEAVE'),
('-1000522','This reminds me of that one time where you made us search Silithus for evidence of sand gnomes.','0','0','0','0','20339','letoll SAY_LE_NO_LEAVE_REPLY1'),
('-1000523','Or that time when you told us that you''d discovered the cure for the plague of the 20th century. What is that even? 20th century?','0','0','0','0','20340','letoll SAY_LE_NO_LEAVE_REPLY2'),
('-1000524','I don''t think it can top the one time where he told us that he''d heard that Artha''s "cousin''s" skeleton was frozen beneath a glacier in Winterspring. I''ll never forgive you for that one, Letoll. I mean honestly... Artha''s cousin?','0','0','0','0','20342','letoll SAY_LE_NO_LEAVE_REPLY3'),
('-1000525','I dunno. It can''t possibly beat the time he tried to convince us that we''re all actually a figment of some being''s imagination and that they only use us for their own personal amusement. That went over well during dinner with the family.','0','0','0','0','20343','letoll SAY_LE_NO_LEAVE_REPLY4'),
('-1000526','Shut yer yaps! I''m gonna bang on this drum and that''s that!','0','0','0','0','20344','letoll SAY_LE_SHUT'),
('-1000527','Say, do you guys hear that?','0','0','0','0','20346','letoll SAY_LE_REPLY_HEAR'),
('-1000528','IN YOUR FACE! I told you there was somethin'' here!','0','0','0','0','20348','letoll SAY_LE_IN_YOUR_FACE'),
('-1000529','Don''t just stand there! Help him out!','0','0','0','0','20361','letoll SAY_LE_HELP_HIM'),
('-1000530','%s picks up the drum.','0','2','0','0','20362','letoll EMOTE_LE_PICK_UP'),
('-1000531','You''ve been a tremendous help, $r! Let''s get out of here before more of those things show up! I''ll let Dwarfowitz know you did the job asked of ya'' admirably.','0','0','0','0','20363','letoll SAY_LE_THANKS'),

('-1000532','At your command, my liege...','0','0','0','0','19774','torloth TORLOTH_DIALOGUE1'),
('-1000533','As you desire, Lord Illidan.','0','0','0','0','19785','torloth TORLOTH_DIALOGUE2'),
('-1000534','For Lord Illidan, I would sacrifice even this magnificent physique. On this day, you will fall - another victim of Torloth...','0','0','0','25','19788','torloth TORLOTH_DIALOGUE3'),
('-1000535','Destroy them, Torloth. Let loose their blood like a river upon this hallowed ground.','0','0','0','0','19779','lordillidan ILLIDAN_DIALOGUE'),
('-1000536','What manner of fool dares stand before Illidan Stormrage? Soldiers, destroy these insects!','0','1','0','0','19797','lordillidan ILLIDAN_SUMMON1'),
('-1000537','You are no challenge for the Crimson Sigil. Mind breakers, end this nonsense.','0','1','0','0','19798','lordillidan ILLIDAN_SUMMON2'),
('-1000538','Run while you still can. The highlords come soon...','0','1','0','0','19799','lordillidan ILLIDAN_SUMMON3'),
('-1000539','Torloth, your master calls!','0','1','0','0','19800','lordillidan ILLIDAN_SUMMON4'),
('-1000540','So you have defeated the Crimson Sigil. You now seek to challenge my rule? Not even Arthas could defeat me, yet you dare to even harbor such thoughts? Then I say to you, come! Come $n! The Black Temple awaits...','0','1','0','0','19801','lordillidan EVENT_COMPLETED'),

('-1000541','%s jumps into the moonwell and goes underwater...','0','2','0','0','5939','kitten EMOTE_SAB_JUMP'),
('-1000542','%s follows $n obediently.','0','2','0','0','5940','kitten EMOTE_SAB_FOLLOW'),

('-1000543','Why have you come here, outsider? You will only find pain! Our fate will be yours...','0','0','0','25','22363','restless app SAY_RAND_1'),
('-1000544','It was ... terrible... the demon...','0','0','0','25','22360','restless app SAY_RAND_2'),
('-1000545','The darkness... the corruption... they came too quickly for anyone to know...','0','0','0','25','22362','restless app SAY_RAND_3'),
('-1000546','The darkness will consume all... all the living...','0','0','0','25','22364','restless app SAY_RAND_4'),
('-1000547','It is too late for us, living one. Take yourself and your friend away from here before you both are... claimed...','0','0','0','25','22365','restless app SAY_RAND_5'),
('-1000548','It is too late for Jarl... its hold is too strong...','0','0','0','25','22366','restless app SAY_RAND_6'),
('-1000549','Go away, whoever you are! Witch Hill is mine... mine!','0','0','0','25','22359','restless app SAY_RAND_7'),
('-1000550','The manor... someone else... will soon be consumed...','0','0','0','25','22361','restless app SAY_RAND_8'),

('-1000551','The %s is angered by your request and attacks!','0','2','0','0','26042','woodlands EMOTE_AGGRO'),
('-1000552','Breaking off a piece of its bark, the %s hands it to you before departing.','0','2','0','0','26041','woodlands EMOTE_CREATE'),

('-1000553','Be ready, $N. I hear the council returning. Prepare to ambush!','0','0','0','0','542','deathstalker_faerleia SAY_START'),
('-1000554','Well done. A blow to Arugal no doubt!','0','0','0','0','545','deathstalker_faerleia SAY_END'),

('-1000555','Back... to work...','0','0','0','0','30796','exhausted vrykul SAY_RAND_WORK1'),
('-1000556','You treat us worse than animals!','0','0','0','0','30797','exhausted vrykul SAY_RAND_WORK2'),
('-1000557','We will have revenge...some day.','0','0','0','0','30798','exhausted vrykul SAY_RAND_WORK3'),
('-1000558','Curse you! You will not treat me like a beast!','0','0','0','0','30793','exhausted vrykul SAY_RAND_ATTACK1'),
('-1000559','I''d rather die fighting than live like a slave.','0','0','0','0','30795','exhausted vrykul SAY_RAND_ATTACK2'),
('-1000560','Enough! I will teach you some manners, wench!','0','0','0','0','30794','exhausted vrykul SAY_RAND_ATTACK3'),

('-1000561','My wounds are grave. Forgive my slow pace but my injuries won''t allow me to walk any faster.','0','0','0','0','25','SAY_CORPORAL_KEESHAN_1'),
('-1000562','Ah, fresh air, at last! I need a moment to rest.','0','0','0','0','26','SAY_CORPORAL_KEESHAN_2'),
('-1000563','The Blackrock infestation is thick in these parts. I will do my best to keep the pace. Let''s go!','0','0','0','0','27','SAY_CORPORAL_KEESHAN_3'),
('-1000564','Marshal Marris, sir. Corporal Keeshan of the 12th Sabre Regiment returned from battle and reporting for duty!','0','0','0','0','29','SAY_CORPORAL_KEESHAN_4'),
('-1000565','Brave adventurer, thank you for rescuing me! I am sure Marshal Marris will reward your kind deed.','0','0','0','0','30','SAY_CORPORAL_KEESHAN_5'),

('-1000566','Stand back! Stand clear! The infernal will need to be given a wide berth!','0','0','0','0','9634','SAY_NIBY_1'),
('-1000567','BOW DOWN TO THE ALMIGHTY! BOW DOWN BEFORE MY INFERNAL DESTRO... chicken?','0','0','0','0','9635','SAY_NIBY_2'),
('-1000568','%s rolls on the floor laughing.','0','2','0','0','9636','EMOTE_IMPSY_1'),
('-1000569','Niby, you'' re an idiot.','0','0','0','0','9637','SAY_IMPSY_1'),
('-1000570','Silence, servant! Vengeance will be mine! Death to Stormwind! Death by chicken!','0','0','0','0','9638','SAY_NIBY_3'),

('-1000571','Help! I''ve only one hand to defend myself with.','0','0','0','0','510','SAY_MIRAN_1'),
('-1000572','Feel the power of the Dark Iron Dwarves!','0','0','0','0','1936','SAY_DARK_IRON_DWARF'),
('-1000573','Send them on! I''m not afraid of some scrawny beasts!','0','0','0','0','511','SAY_MIRAN_2'),
('-1000574','Ah, here at last! It''s going to feel so good to get rid of these barrels.','0','0','0','0','498','SAY_MIRAN_3'),

('-1000575','Together we will fight our way out of here. Are you ready?','0','0','0','0','24310','Lurgglbr - SAY_START_1'),
('-1000576','Then we leave.','0','0','0','0','24311','Lurgglbr - SAY_START_2'),
('-1000577','This is far enough. I can make it on my own from here.','0','0','0','0','24312','Lurgglbr - SAY_END_1'),
('-1000578','Thank you for helping me $r. Please tell the king I am back.','0','0','0','0','24313','Lurgglbr - SAY_END_2'),

('-1000579','There! Destroy him! The Cipher must be recovered!','0','0','0','25','19012','spirit hunter - SAY_VENERATUS_SPAWN'),

('-1000580','Sleep now, young one ...','0','0','0','0','25323','Raelorasz SAY_SLEEP'),
('-1000581','A wonderful specimen.','0','0','0','0','25324','Raeloarsz SAY_SPECIMEN'),

('-1000582','Help! Please, You must help me!','0','0','0','0','7124','Galen - periodic say'),
('-1000583','Let us leave this place.','0','0','0','0','1854','Galen - quest accepted'),
('-1000584','Look out! The $c attacks!','0','0','0','0','0','Galen - aggro 1'),
('-1000585','Help! I''m under attack!','0','0','0','0','1629','Galen - aggro 2'),
('-1000586','Thank you $N. I will remember you always. You can find my strongbox in my camp, north of Stonard.','0','0','0','0','1855','Galen - quest complete'),
('-1000587','%s whispers to $N the secret to opening his strongbox.','0','2','0','0','2076','Galen - emote whisper'),
('-1000588','%s disappears into the swamp.','0','2','0','0','1856','Galen - emote disappear'),

('-1000589','Kroshius live? Kroshius crush!','0','1','0','0','9633','SAY_KROSHIUS_REVIVE'),

('-1000590','The feast of curruption is no more! Magtheridon has fallen to the battle prowess of $N and $Ghis:her; allies! Witness the might of the Alliance! A pit lord commander is destroyed!','0','6','0','15','20754','Yell Magtheridon Death1 A'),
('-1000591','Hear me brothers and sisters-in-arms! The time of our enemies is at an end! We must strike at the fel orcs that remain! Exterminate them like the vermin that they are! Carry with you the favor of the Sons of Lothar!','0','6','0','22','20755','Yell Magtheridon Death2 A'),
('-1000592','Hellfire Citadel is ours! The source of Illidan''s fel orc army is no more! Magtheridon has fallen to the might of $n and $g his:her; allies!','0','6','0','15','20765','Yell Magtheridon Death1 H'),
('-1000593','The time for us to rise and strike at the fel orcs that remain is now! Do this for your Warchief! Do this for the Horde!','0','6','0','22','20766','Yell Magtheridon Death2 H'),

('-1000594','At last... now I can rest.','0','0','0','0','35291','hero spirit SAY_BLESS_1'),
('-1000595','I''m so tired. Just let me rest for a moment.','0','0','0','0','35295','hero spirit SAY_BLESS_2'),
('-1000596','I can''t hear the screams anymore. Is this the end?','0','0','0','0','35294','hero spirit SAY_BLESS_3'),
('-1000597','My nightmare, is it finally over?','0','0','0','0','35292','hero spirit SAY_BLESS_4'),
('-1000598','It was awful... I dreamt I was fighting against my friends.','0','0','0','0','35293','hero spirit SAY_BLESS_5'),

('-1000599','It''s a miracle! The beast skinned itself!','0','0','0','5','25029','nesingwary trapper SAY_PHRASE_1'),
('-1000600','Jackpot!','0','0','0','5','25034','nesingwary trapper SAY_PHRASE_2'),
('-1000601','This is the last one i need for that set of Nesingwary steak knives!','0','0','0','5','25031','nesingwary trapper SAY_PHRASE_3'),
('-1000602','Silly beasts!','0','0','0','5','25030','nesingwary trapper SAY_PHRASE_4'),

('-1000603','Do not test me, scurvy dog! I''m trained in the way of the Blood Knights!','0','0','0','0','23810','silvermoon harry SAY_AGGRO'),
('-1000604','I''ll pay! I''ll pay! Eeeek! Please don''t hurt me!','0','0','0','0','23812','silvermoon harry SAY_BEATEN'),

('-1000605','We wait until you ready.','0','0','0','0','27824','rainspeaker SAY_ACCEPT'),
('-1000606','Home time!','0','0','0','0','27751','rainspeaker SAY_START'),
('-1000607','Thanks!','0','0','0','0','27752','rainspeaker SAY_END_1'),
('-1000608','Oh no! Some puppy-men followed!','0','0','0','0','27799','rainspeaker SAY_END_2'),
('-1000609','Dumb big-tongue lover! You not friend of Frenzyheart no more. Frenzyheart will get you good.','0','1','0','0','27798','rainspeaker SAY_TRACKER'),

('-1000610','The mosswalker victim groans in pain.','0','2','0','0','28638','mosswalker victim EMOTE_PAIN'),

('-1000611','Maybe you make weather better too?','0','0','0','0','28906','mosswalker victim SAY_RESCUE_1'),
('-1000612','We saved. You nice, dryskin.','0','0','0','0','28631','mosswalker victim SAY_RESCUE_2'),
('-1000613','You save us! Yay for you!','0','0','0','0','28909','mosswalker victim SAY_RESCUE_3'),
('-1000614','Thank you! You good!','0','0','0','0','28905','mosswalker victim SAY_RESCUE_4'),

('-1000615','Use my shinies...make weather good again...make undead things go away.','0','0','0','0','28901','mosswalker victim SAY_DIE_1'),
('-1000616','We gave shinies to shrine... we not greedy... why this happen?','0','0','0','0','28900','mosswalker victim SAY_DIE_2'),
('-1000617','I do something bad? I sorry....','0','0','0','0','28904','mosswalker victim SAY_DIE_3'),
('-1000618','We not do anything... to them... I no understand.','0','0','0','0','28903','mosswalker victim SAY_DIE_4'),
('-1000619','Thank...you.','0','0','0','0','28902','mosswalker victim SAY_DIE_5'),
('-1000620','Please take... my shinies. All done...','0','0','0','0','28630','mosswalker victim SAY_DIE_6'),

('-1000621','All systems on-line.  Prepare yourself, we leave shortly.','0','0','0','0','19481','maxx SAY_START'),
('-1000622','Be careful in there and come back in one piece!','0','0','0','0','17003','maxx SAY_ALLEY_FAREWELL'),
('-1000623','Proceed.','0','0','0','0','19484','maxx SAY_CONTINUE'),
('-1000624','You''re back!  Were you able to get all of the machines?','0','0','0','0','17014','maxx SAY_ALLEY_FINISH'),

('-1000625','%s gathers the warp chaser''s blood.','0','2','0','0','20371','zeppit EMOTE_GATHER_BLOOD'),

('-1000626','Intiating energy collection.','0','0','0','0','26153','depleted golem SAY_GOLEM_CHARGE'),
('-1000627','Energy collection complete.','0','0','0','0','26154','depleted golem SAY_GOLEM_COMPLETE'),

('-1000628','%s feeds on the freshly-killed warp chaser.','0','2','0','0','21657','hungry ray EMOTE_FEED'),

('-1000629','<cough> <cough> Damsel in distress over here!','0','0','0','0','15879','isla starmane - SAY_ISLA_PERIODIC_1'),
('-1000630','Hello? Help?','0','0','0','0','15878','isla starmane - SAY_ISLA_PERIODIC_2'),
('-1000631','Don''t leave me in here! Cause if you do I will find you!','0','0','0','0','15881','isla starmane - SAY_ISLA_PERIODIC_3'),
('-1000632','Ok, let''s get out of here!','0','0','0','0','17946','isla starmane - SAY_ISLA_START'),
('-1000633','You sure you''re ready? Take a moment.','0','0','0','0','15883','isla starmane - SAY_ISLA_WAITING'),
('-1000634','Alright, let''s do this!','0','0','0','0','15884','isla starmane - SAY_ISLA_LEAVE_BUILDING'),

('-1000635','So then we too are cursed?','0','0','0','0','23116','ancient vrykul SAY_VRYKUL_CURSED'),
('-1000636','%s points to the infant.','0','2','0','0','23117','ancient vrykul EMOTE_VRYKUL_POINT'),
('-1000637','%s sobs.','0','2','0','0','29285','ancient vrykul EMOTE_VRYKUL_SOB'),
('-1000638','The gods have forsaken us! We must dispose of it before Ymiron is notified!','0','0','0','0','23119','ancient vrykul SAY_VRYKUL_DISPOSE'),
('-1000639','NO! You cannot! I beg of you! It is our child!','0','0','0','0','23120','ancient vrykul SAY_VRYKUL_BEG'),
('-1000640','Then what are we to do, wife? The others cannot find out. Should they learn of this aberration, we will all be executed.','0','0','0','0','23121','ancient vrykul SAY_VRYKUL_WHAT'),
('-1000641','I... I will hide it. I will hide it until I find it a home, far away from here...','0','0','0','0','23122','ancient vrykul SAY_VRYKUL_HIDE'),

('-1000642','It''s a female.','0','5','0','0','33192','leopard icepaw SAY_ITS_FEMALE'),
('-1000643','It''s an angry male!','0','5','0','0','33191','leopard icepaw SAY_ITS_MALE'),

('-1000644','Ouch! That''s it, I quit the target business!','0','0','0','0','27757','SAY_LUCKY_HIT_1'),
('-1000645','My ear! You grazed my ear!','0','0','0','0','27756','SAY_LUCKY_HIT_2'),
('-1000646','Not the ''stache! Now I''m asymmetrical!','0','0','0','5','27755','SAY_LUCKY_HIT_3'),
('-1000647','Good shot!','0','0','0','4','27778','SAY_LUCKY_HIT_APPLE'),
('-1000648','Stop whining. You''ve still got your luck.','0','0','0','0','27764','SAY_DROSTAN_GOT_LUCKY_1'),
('-1000649','Bah, it''s an improvement.','0','0','0','11','27762','SAY_DROSTAN_GOT_LUCKY_2'),
('-1000650','Calm down lad, it''s just a birdshot!','0','0','0','0','27761','SAY_DROSTAN_HIT_BIRD_1'),
('-1000651','The only thing hurt is your pride, lad! Buck up!','0','0','0','0','27765','SAY_DROSTAN_HIT_BIRD_2'),

('-1000652','Me so hungry! YUM!','0','0','0','71','21120','dragonmaw peon SAY_PEON_1'),
('-1000653','Hey... me not feel so good.','0','0','0','0','21122','dragonmaw peon SAY_PEON_2'),
('-1000654','You is bad orc... baaad... or... argh!','0','0','0','0','21123','dragonmaw peon SAY_PEON_3'),
('-1000655','Time for eating!?','0','0','0','71','21118','dragonmaw peon SAY_PEON_4'),
('-1000656','It put the mutton in the stomach!','0','0','0','71','21121','dragonmaw peon SAY_PEON_5'),

('-1000657','Let''s get the hell out of here.','0','0','0','5','28856','helice SAY_HELICE_ACCEPT'),
('-1000658','Listen up, Venture Company goons! Rule #1: Never keep the prisoner near the explosives.','0','0','0','25','28857','helice SAY_HELICE_EXPLOSIVES_1'),
('-1000659','Or THIS is what you get.','0','0','0','0','28859','helice SAY_HELICE_EXPLODE_1'),
('-1000660','It''s getting a little hot over here. Shall we move on?','0','0','0','11','28860','helice SAY_HELICE_MOVE_ON'),
('-1000661','Oh, look, it''s another cartload of explosives! Let''s help them dispose of it.','0','0','0','25','28862','helice SAY_HELICE_EXPLOSIVES_2'),
('-1000662','You really shouldn''t play with this stuff. Someone could get hurt.','0','0','0','5','28864','helice SAY_HELICE_EXPLODE_2'),
('-1000663','We made it! Thank you for getting me out of that hell hole. Tell Hemet to expect me!','0','0','0','4','28865','helice SAY_HELICE_COMPLETE'),

('-1000664','The Destructive Ward gains in power.','0','5','0','0','26601','destructive ward SAY_WARD_POWERUP'),
('-1000665','The Destructive Ward is fully charged!','0','5','0','0','26602','destructive ward SAY_WARD_CHARGED'),

('-1000666','I can sense the SHADOW on your hearts. There can be no rest for the wicked!','0','1','0','0','10883','lethon SAY_LETHON_AGGRO'),
('-1000667','Your wicked souls shall feed my power!','0','1','0','0','10882','lethon SAY_LETHON_SHADE'),

('-1000668','%s releases the last of its energies into the nearby runestone, successfully reactivating it.','0','2','0','0','12272','infused crystal SAY_DEFENSE_FINISH'),

('-1000669','We will locate the origin of the Nightmare through the fragments you collected, $N. From there, we will pull Eranikus through a rift in the Dream. Steel yourself, $C. We are inviting the embodiment of the Nightmare into our world.','0','0','0','0','11282','remulos SAY_REMULOS_INTRO_1'),
('-1000670','To Nighthaven! Keep your army close, champion. ','0','0','0','0','11283','remulos SAY_REMULOS_INTRO_2'),
('-1000671','The rift will be opened there, above the Lake Elun''ara. Prepare yourself, $N. Eranikus''s entry into our world will be wrought with chaos and strife.','0','0','0','0','11290','remulos SAY_REMULOS_INTRO_3'),
('-1000672','He will stop at nothing to get to Malfurion''s physical manifestation. That must not happen... We must keep the beast occupied long enough for Tyrande to arrive.','0','0','0','0','11291','remulos SAY_REMULOS_INTRO_4'),
('-1000673','Defend Nightaven, hero...','0','0','0','0','11292','remulos SAY_REMULOS_INTRO_5'),
('-1000674','%s has entered our world','0','3','0','0','11277','eranikus EMOTE_SUMMON_ERANIKUS'),
('-1000675','Pitful predictable mortals... You know not what you have done! The master''s will fulfilled. The Moonglade shall be destroyed and Malfurion along with it!','0','1','0','0','11030','eranikus SAY_ERANIKUS_SPAWN'),
('-1000676','Fiend! Face the might of Cenarius!','0','1','0','1','11293','remulos SAY_REMULOS_TAUNT_1'),
('-1000677','%s lets loose a sinister laugh.','0','2','0','0','11296','eranikus EMOTE_ERANIKUS_LAUGH'),
('-1000678','You are certainly not your father, insect. Should it interest me, I would crush you with but a swipe of my claws. Turn Shan''do Stormrage over to me and your pitiful life will be spared along with the lives of your people.','0','1','0','0','11294','eranikus SAY_ERANIKUS_TAUNT_2'),
('-1000679','Who is the predictable one, beast? Surely you did not think that we would summon you on top of Malfurion? Your redemption comes, Eranikus. You will be cleansed of this madness - this corruption.','0','1','0','1','11295','remulos SAY_REMULOS_TAUNT_3'),
('-1000680','My redemption? You are bold, little one. My redemption comes by the will of my god.','0','1','0','0','11297','eranikus SAY_ERANIKUS_TAUNT_4'),
('-1000681','%s roars furiously.','0','2','0','0','11298','eranikus EMOTE_ERANIKUS_ATTACK'),
('-1000682','Hurry, $N! We must find protective cover!','0','0','0','0','11300','remulos SAY_REMULOS_DEFEND_1'),
('-1000683','Please, champion, protect our people.','0','0','0','1','11301','remulos SAY_REMULOS_DEFEND_2'),
('-1000684','Rise, servants of the Nightmare! Rise and destroy this world! Let there be no survivors...','0','1','0','0','11299','eranikus SAY_ERANIKUS_SHADOWS'),
('-1000685','We will battle these fiends, together! Nighthaven''s Defenders are also among us. They will fight to the death if asked. Now, quickly, we must drive these aberrations back to the Nightmare. Destroy them all!','0','0','0','1','11302','remulos SAY_REMULOS_DEFEND_3'),
('-1000686','Where is your savior? How long can you hold out against my attacks?','0','1','0','0','11304','eranikus SAY_ERANIKUS_ATTACK_1'),
('-1000687','Defeated my minions? Then face me, mortals!','0','1','0','0','11305','eranikus SAY_ERANIKUS_ATTACK_2'),
('-1000688','Remulos, look how easy they fall before me? You can stop this, fool. Turn the druid over to me and it will all be over...','0','1','0','0','11306','eranikus SAY_ERANIKUS_ATTACK_3'),
('-1000689','Elune, hear my prayers. Grant us serenity! Watch over our fallen...','0','1','0','0','11309','tyrande SAY_TYRANDE_APPEAR'),
('-1000690','Tend to the injuries of the wounded, sisters!','0','0','0','0','11317','tyrande SAY_TYRANDE_HEAL'),
('-1000691','Seek absolution, Eranikus. All will be forgiven...','0','1','0','0','11310','tyrande SAY_TYRANDE_FORGIVEN_1'),
('-1000692','You will be forgiven, Eranikus. Elune will always love you. Break free of the bonds that command you!','0','1','0','0','11311','tyrande SAY_TYRANDE_FORGIVEN_2'),
('-1000693','The grasp of the Old Gods is unmoving. He is consumed by their dark thoughts... I... I... I cannot... cannot channel much longer... Elune aide me.','0','0','0','0','11312','tyrande SAY_TYRANDE_FORGIVEN_3'),
('-1000694','IT BURNS! THE PAIN.. SEARING...','0','1','0','0','11314','eranikus SAY_ERANIKUS_DEFEAT_1'),
('-1000695','WHY? Why did this happen to... to me? Where were you Tyrande? Where were you when I fell from the grace of Elune?','0','1','0','0','11315','eranikus SAY_ERANIKUS_DEFEAT_2'),
('-1000696','I... I feel... I feel the touch of Elune upon my being once more... She smiles upon me... Yes... I...','0','1','0','0','11316','eranikus SAY_ERANIKUS_DEFEAT_3'),
('-1000697','%s is wholly consumed by the Light of Elune. Tranquility sets in over the Moonglade','0','3','0','0','11313','eranikus EMOTE_ERANIKUS_REDEEM'),
('-1000698','%s falls to one knee.','0','2','0','0','7421','tyrande EMOTE_TYRANDE_KNEEL'),
('-1000699','Praise be to Elune... Eranikus is redeemed.','0','1','0','0','11320','tyrande SAY_TYRANDE_REDEEMED'),
('-1000700','For so long, I was lost... The Nightmare''s corruption had consumed me... And now, you... all of you... you have saved me. Released me from its grasp.','0','0','0','0','11323','eranikus SAY_REDEEMED_1'),
('-1000701','But... Malfurion, Cenarius, Ysera... They still fight. They need me. I must return to the Dream at once.','0','0','0','0','11324','eranikus SAY_REDEEMED_2'),
('-1000702','My lady, I am unworthy of your prayer. Truly, you are an angel of light. Please, assist me in returning to the barrow den so that I may return to the Dream. I, like Malfurion, also have a love awaiting me... I must return to her... to protect her...','0','0','0','0','11326','eranikus SAY_REDEEMED_3'),
('-1000703','And heroes... I hold that which you seek. May it once more see the evil dissolved. Remulos, see to it that our champion receives the shard of the Green Flight.','0','0','0','0','11327','eranikus SAY_REDEEMED_4'),
('-1000704','It will be done, Eranikus. Be well, ancient one.','0','0','0','0','11303','remulos SAY_REMULOS_OUTRO_1'),
('-1000705','Let us leave Nighthave, hero. Seek me out at the grove.','0','0','0','0','11329','remulos SAY_REMULOS_OUTRO_2'),
('-1000706','Your world shall suffer an unmerciful end. The Nightmare comes for you!','0','0','0','0','11027','eranikus SAY_ERANIKUS_KILL'),

('-1000707','This blue light... It''s strange. What do you think it means?','0','0','0','0','7375','Ranshalla SAY_ENTER_OWL_THICKET'),
('-1000708','We''ve found it!','0','0','0','0','0','Ranshalla SAY_REACH_TORCH_1'),
('-1000709','Please, light this while I am channeling','0','0','0','0','7378','Ranshalla SAY_REACH_TORCH_2'),
('-1000710','This is the place. Let''s light it.','0','0','0','0','7379','Ranshalla SAY_REACH_TORCH_3'),
('-1000711','Let''s find the next one.','0','0','0','0','7410','Ranshalla SAY_AFTER_TORCH_1'),
('-1000712','We must continue on now.','0','0','0','0','7411','Ranshalla SAY_AFTER_TORCH_2'),
('-1000713','It is time for the final step; we must activate the altar.','0','0','0','0','7385','Ranshalla SAY_REACH_ALTAR_1'),
('-1000714','I will read the words carved into the stone, and you must find a way to light it.','0','0','0','0','7386','Ranshalla SAY_REACH_ALTAR_2'),
('-1000715','The altar is glowing! We have done it!','0','0','0','0','7412','Ranshalla SAY_RANSHALLA_ALTAR_1'),
('-1000716','What is happening? Look!','0','0','0','0','7388','Ranshalla SAY_RANSHALLA_ALTAR_2'),
('-1000717','It has been many years...','0','0','0','0','7389','Priestess of Elune SAY_PRIESTESS_ALTAR_3'),
('-1000718','Who has disturbed the altar of the goddess?','0','0','0','0','7390','Priestess of Elune SAY_PRIESTESS_ALTAR_4'),
('-1000719','Please, priestesses, forgive us for our intrusion. We do not wish any harm here.','0','0','0','0','7391','Ranshalla SAY_RANSHALLA_ALTAR_5'),
('-1000720','We only wish to know why the wildkin guard this area...','0','0','0','0','7392','Ranshalla SAY_RANSHALLA_ALTAR_6'),
('-1000721','Enu thora''serador. This is a sacred place.','0','0','0','0','7393','Priestess of Elune SAY_PRIESTESS_ALTAR_7'),
('-1000722','We will show you...','0','0','0','0','7394','Priestess of Elune SAY_PRIESTESS_ALTAR_8'),
('-1000723','Look above you; thara dormil dorah...','0','0','0','0','7413','Priestess of Elune SAY_PRIESTESS_ALTAR_9'),
('-1000724','This gem once allowed direct communication with Elune, herself.','0','0','0','0','7414','Priestess of Elune SAY_PRIESTESS_ALTAR_10'),
('-1000725','Through the gem, Elune channeled her infinite wisdom...','0','0','0','0','7415','Priestess of Elune SAY_PRIESTESS_ALTAR_11'),
('-1000726','Realizing that the gem needed to be protected, we turned to the goddess herself.','0','0','0','0','7417','Priestess of Elune SAY_PRIESTESS_ALTAR_12'),
('-1000727','Soon after, we began to have visions of a creature... A creature with the feathers of an owl, but the will and might of a bear...','0','0','0','0','7418','Priestess of Elune SAY_PRIESTESS_ALTAR_13'),
('-1000728','It was on that day that the wildkin were given to us. Fierce guardians, the goddess assigned the wildkin to protect all of her sacred places.','0','0','0','0','7419','Priestess of Elune SAY_PRIESTESS_ALTAR_14'),
('-1000729','Anu''dorini Talah, Ru shallora enudoril.','0','0','0','0','7416','Voice of Elune SAY_VOICE_ALTAR_15'),
('-1000730','But now, many years later, the wildkin have grown more feral, and without the guidance of the goddess, they are confused...','0','0','0','0','7422','Priestess of Elune SAY_PRIESTESS_ALTAR_16'),
('-1000731','Without a purpose, they wander... But many find their way back to the sacred areas that they once were sworn to protect.','0','0','0','0','7424','Priestess of Elune SAY_PRIESTESS_ALTAR_17'),
('-1000732','Wildkin are inherently magical; this power was bestowed upon them by the goddess.','0','0','0','0','7427','Priestess of Elune SAY_PRIESTESS_ALTAR_18'),
('-1000733','Know that wherever you might find them in the world, they are protecting something of importance, as they were entrusted to do so long ago.','0','0','0','0','7428','Priestess of Elune SAY_PRIESTESS_ALTAR_19'),
('-1000734','Please, remember what we have shown you...','0','0','0','0','7431','Priestess of Elune SAY_PRIESTESS_ALTAR_20'),
('-1000735','Farewell.','0','0','0','0','23348','Priestess of Elune SAY_PRIESTESS_ALTAR_21'),
('-1000736','Thank you for your help, $n. I wish you well in your adventures.','0','0','0','0','7434','Ranshalla SAY_QUEST_END_1'),
('-1000737','I want to stay here and reflect on what we have seen. Please see Erelas and tell him what we have learned.','0','0','0','0','7435','Ranshalla SAY_QUEST_END_2'),
('-1000738','%s begins chanting a strange spell...','0','2','0','0','7467','Ranshalla EMOTE_CHANT_SPELL'),
('-1000739','Remember, I need your help to properly channel. I will ask you to aid me several times in our path, so please be ready.','0','0','0','0','7365','Ranshalla SAY_QUEST_START'),

('-1000740','We must act quickly or all shall be lost!','0','0','0','1','10909','SAY_ANACHRONOS_INTRO_1'),
('-1000741','My forces cannot overcome the Qiraji defenses. We will not be able to get close enough to place your precious barrier, dragon.','0','0','0','0','10910','SAY_FANDRAL_INTRO_2'),
('-1000742','There is a way...','0','0','0','22','10911','SAY_MERITHRA_INTRO_3'),
('-1000743','%s nods knowingly.','0','2','0','0','10913','EMOTE_ARYGOS_NOD'),
('-1000744','Aye, Fandral, remember these words: Let not your grief guide your faith. These thoughts you hold... dark places you go, night elf. Absolution cannot be had through misguided vengeance.','0','0','0','1','10914','SAY_CAELESTRASZ_INTRO_4'),
('-1000745','%s glances at her compatriots.','0','2','0','0','10912','EMOTE_MERITHRA_GLANCE'),
('-1000746','We will push him back, Anachronos. This I vow. Uphold your end of this task. Let not your hands falter as you seal our fates behind the barrier.','0','0','0','1','10908','SAY_MERITHRA_INTRO_5'),
('-1000747','Succumb to the endless dream, little ones. Let it consume you!','0','1','0','22','10903','SAY_MERITHRA_ATTACK_1'),
('-1000748','Anachronos, this diversion will give you and the young druid time enough to seal the gate. Do not falter. Now, let us see how they deal with chaotic magic.','0','0','0','1','10904','SAY_ARYGOS_ATTACK_2'),
('-1000749','Let them feel the wrath of the Blue Flight! May Malygos protect me!','0','1','0','22','10901','SAY_ARYGOS_ATTACK_3'),
('-1000750','Do not forget the sacrifices made on this day, night elf. We have all suffered immensely at the hands of these beasts.','0','0','0','1','10907','SAY_CAELESTRASZ_ATTACK_4'),
('-1000751','Alexstrasza grant me the resolve to drive our enemies back!','0','1','0','22','10902','SAY_CAELESTRASZ_ATTACK_5'),
('-1000752','NOW, STAGHELM! WE GO NOW! Prepare your magic!','0','0','0','22','10915','SAY_ANACHRONOS_SEAL_1'),
('-1000753','It is done, dragon. Lead the way!','0','0','0','25','10916','SAY_FANDRAL_SEAL_2'),
('-1000754','Stay close...','0','0','0','0','10917','SAY_ANACHRONOS_SEAL_3'),
('-1000755','The sands of time will halt, but only for a moment! I will now conjure the barrier.','0','0','0','0','10930','SAY_ANACHRONOS_SEAL_4'),
('-1000756','FINISH THE SPELL, STAGHELM! I CANNOT HOLD THE GLYPHS OF WARDING IN PLACE MUCH LONGER! CALL FORTH THE ROOTS!','0','0','0','0','10920','SAY_ANACHRONOS_SEAL_5'),
('-1000757','Ancient ones guide my hand... Wake from your slumber! WAKE AND SEAL THIS CURSED PLACE!','0','0','0','0','10921','SAY_FANDRAL_SEAL_6'),
('-1000758','%s falls to one knee - exhausted.','0','2','0','0','10922','EMOTE_FANDRAL_EXHAUSTED'),
('-1000759','It... It is over, Lord Staghelm. We are victorious. Albeit the cost for this victory was great.','0','0','0','1','10923','SAY_ANACHRONOS_EPILOGUE_1'),
('-1000760','There is but one duty that remains...','0','0','0','1','10924','SAY_ANACHRONOS_EPILOGUE_2'),
('-1000761','Before I leave this place, I make one final offering for you, Lord Staghelm. Should a time arise in which you must gain entry to this accursed fortress, use the Scepter of the Shifting Sands on the sacred gong. The magic holding the barrier together will dissipate and the horrors of Ahn''Qiraj will be unleashed upon the world once more.','0','0','0','1','10925','SAY_ANACHRONOS_EPILOGUE_3'),
('-1000762','%s hands the Scepter of the Shifting Sands to $N.','0','2','0','0','10926','EMOTE_ANACHRONOS_SCEPTER'),
('-1000763','After the savagery that my people have witnessed and felt, you expect me to accept another burden, dragon? Surely you are mad.','0','0','0','1','10927','SAY_FANDRAL_EPILOGUE_4'),
('-1000764','I want nothing to do with Silithus, the Qiraji and least of all, any damned dragons!','0','0','0','1','10928','SAY_FANDRAL_EPILOGUE_5'),
('-1000765','%s hurls the Scepter of the Shifting Sands into the barrier, shattering it.','0','2','0','0','10929','EMOTE_FANDRAL_SHATTER'),
('-1000766','Lord Staghelm, where are you going? You would shatter our bond for the sake of pride?','0','0','0','1','10931','SAY_ANACHRONOS_EPILOGUE_6'),
('-1000767','My son''s soul will find no comfort in this hollow victory, dragon. I will have him back. Though it takes a millennia, I WILL have my son back!','0','0','0','1','10932','SAY_FANDRAL_EPILOGUE_7'),
('-1000768','%s shakes his head in disappointment.','0','2','0','25','10933','EMOTE_ANACHRONOS_DISPPOINTED'),
('-1000769','%s kneels down to pickup the fragments of the shattered scepter.','0','2','0','0','10934','EMOTE_ANACHRONOS_PICKUP'),
('-1000770','And now you know all that there is to know, mortal...','0','0','0','0','10935','SAY_ANACHRONOS_EPILOGUE_8'),

('-1000771','Let''s go $N!','0','0','0','0','21649','Feero Ironhand SAY_QUEST_START'),
('-1000772','It looks like we''re in trouble. Look lively, here they come!','0','0','0','0','1372','Feero Ironhand SAY_FIRST_AMBUSH_START'),
('-1000773','Assassins from that cult you found... Let''s get moving before someone else finds us out here.','0','0','0','0','1294','Feero Ironhand SAY_FIRST_AMBUSH_END'),
('-1000774','Hold! I sense an evil presence... Undead!','0','0','0','0','1373','Feero Ironhand SAY_SECOND_AMBUSH_START'),
('-1000775','A $C! Slaying him would please the master. Attack!','0','0','0','0','1309','Forsaken Scout SAY_SCOUT_SECOND_AMBUSH'),
('-1000776','They''re coming out of the woodwork today. Let''s keep moving or we may find more things that want me dead.','0','0','0','0','1310','Feero Ironhand SAY_SECOND_AMBUSH_END'),
('-1000777','These three again?','0','0','0','0','1374','Feero Ironhand SAY_FINAL_AMBUSH_START'),
('-1000778','Not quite so sure of yourself without the Purifier, hm?','0','0','0','0','1313','Balizar the Umbrage SAY_BALIZAR_FINAL_AMBUSH'),
('-1000779','I''ll finish you off for good this time!','0','0','0','0','1499','Feero Ironhand SAY_FINAL_AMBUSH_ATTACK'),
('-1000780','Well done! I should be fine on my own from here. Remember to talk to Delgren when you return to Maestra''s Post in Ashenvale.','0','0','0','0','1315','Feero Ironhand SAY_QUEST_END'),

('-1000781','I knew Lurielle would send help! Thank you, friend, and give Lurielle my thanks as well!','0','0','0','0','23044','Chill Nymph SAY_FREE_1'),
('-1000782','Where am I? What happend to me? You... you freed me?','0','0','0','0','23043','Chill Nymph SAY_FREE_2'),
('-1000783','Thank you. I thought I would die without seeing my sisters again!','0','0','0','0','23042','Chill Nymph SAY_FREE_3'),

('-1000784','Thanks $N. Now let''s get out of here!','0','0','0','0','7540','melizza SAY_MELIZZA_START'),
('-1000785','We made it! Thanks again! I''m going to run ahead!','0','0','0','0','7544','melizza SAY_MELIZZA_FINISH'),
('-1000786','Hey Hornizz! I''m back! And there are some people behind me who helped me out of a jam.','0','0','0','1','7550','melizza SAY_MELIZZA_1'),
('-1000787','We''re going to have to scratch the Maraudines off our list. Too hard to work with...','0','0','0','1','7551','melizza SAY_MELIZZA_2'),
('-1000788','Well, I''m off to the Gelkis. They''re not as dumb as the Maraudines, but they''re more reasonable.','0','0','0','3','7552','melizza SAY_MELIZZA_3'),

('-1000789','Well, now or never I suppose. Remember, once we get to the road safety, return to Terenthis to let him know we escaped.','0','0','0','0','1237','volcor SAY_START'),
('-1000790','We made it, My friend. Remember to find Terenthis and let him know we''re safe. Thank you again.','0','0','0','0','1243','volcor SAY_END'),
('-1000791','Here they come.','0','0','0','0','56214','volcor SAY_FIRST_AMBUSH'),
('-1000792','We can overcome these foul creatures.','0','0','0','0','1251','volcor SAY_AGGRO_1'),
('-1000793','We shall earn our deaths at the very least!','0','0','0','0','1252','volcor SAY_AGGRO_2'),
('-1000794','Don''t give up! Fight, to the death!','0','0','0','0','1253','volcor SAY_AGGRO_3'),

('-1000795','Ow! Ok, I''ll get back to work, $N!','0','0','1','0','5774','Lazy Peon SAY_PEON_AWOKEN'),
('-1000796','REUSE_ME','0','0','0','0','0','REUSE_ME'),

('-1000797','%s squawks and heads toward Veil Shalas. Hurry and follow!','0','2','0','0','20254','skywing SAY_SKYWING_START'),
('-1000798','%s pauses briefly before the tree and then heads inside.','0','2','0','0','20255','skywing SAY_SKYWING_TREE_DOWN'),
('-1000799','%s seems to be looking for something. He wants you to follow.','0','2','0','0','20256','skywing SAY_SKYWING_TREE_UP'),
('-1000800','%s flies to the platform below! You''d better jump if you want to keep up. Hurry!','0','2','0','0','20257','skywing SAY_SKYWING_JUMP'),
('-1000801','%s bellows a loud squawk!','0','2','0','0','20284','skywing SAY_SKYWING_SUMMON'),
('-1000802','Free at last from that horrible curse! Thank you! Please send word to Rilak the Redeemed that I am okay. My mission lies in Skettis. Terokk must be defeated!','0','0','0','0','20295','skywing SAY_SKYWING_END'),

('-1000803','You do not fight alone, $n! Together, we will banish this spawn of hellfire!','0','1','0','0','19304','Oronok SAY_ORONOK_TOGETHER'),
('-1000804','We will fight when you are ready.','0','0','0','0','19310','Oronok SAY_ORONOK_READY'),
('-1000805','We will set the elements free of your grasp by force!','0','1','0','0','19309','Oronok SAY_ORONOK_ELEMENTS'),
('-1000806','What say the elements, Torlok? I only hear silence.','0','0','0','1','19334','Oronok SAY_ORONOK_EPILOGUE_1'),
('-1000807','I hear what you hear, brother. Look behind you...','0','0','0','1','19336','Torlok SAY_TORLOK_EPILOGUE_2'),
('-1000808','They are redeemed! Then we have won?','0','0','0','1','19337','Oronok SAY_ORONOK_EPILOGUE_3'),
('-1000809','It is now as it should be, shaman. You have done well.','0','0','0','0','19338','Spirit of Earth SAY_EARTH_EPILOGUE_4'),
('-1000810','Yes... Well enough for the elements that are here, but the cipher is known to another... The spirits of fire are in turmoil... If this force is not stopped, the world where these mortals came from will cease.','0','0','0','0','19339','Spirit of Fire SAY_FIRE_EPILOGUE_5'),
('-1000811','Farewell, mortals... The earthmender knows what fire feels...','0','0','0','0','19340','Spirit of Earth SAY_EARTH_EPILOGUE_6'),
('-1000812','We leave, then, Torlok. I have only one request...','0','0','0','1','19341','Oronok SAY_ORONOK_EPILOGUE_7'),
('-1000813','The Torn-heart men give their weapons to Earthmender Torlok.','0','2','0','0','19342','Torlok EMOTE_GIVE_WEAPONS'),
('-1000814','Give these to the heroes that made this possible.','0','0','0','1','19343','Oronok SAY_ORONOK_EPILOGUE_8'),

('-1000815','Be healed!','0','1','0','0','49889','Eris Havenfire SAY_PHASE_HEAL'),
('-1000816','We are saved! The peasants have escaped the Scourge!','0','1','0','0','9728','Eris Havenfire SAY_EVENT_END'),
('-1000817','I have failed once more...','0','1','0','0','9648','Eris Havenfire SAY_EVENT_FAIL_1'),
('-1000818','I now return to whence I came, only to find myself here once more to relive the same epic tragedy.','0','0','0','0','9649','Eris Havenfire SAY_EVENT_FAIL_2'),
('-1000819','The Scourge are upon us! Run! Run for your lives!','0','1','0','0','9712','Peasant SAY_PEASANT_APPEAR_1'),
('-1000820','Please help us! The Prince has gone mad!','0','1','0','0','9713','Peasant SAY_PEASANT_APPEAR_2'),
('-1000821','Seek sanctuary in Hearthglen! It is our only hope!','0','1','0','0','9714','Peasant SAY_PEASANT_APPEAR_3'),

('-1000822','The signal has been sent. He should be arriving shortly.','0','0','0','1','14389','squire rowe SAY_SIGNAL_SENT'),
('-1000823','Yawww!','0','0','0','35','8091','reginald windsor SAY_DISMOUNT'),
('-1000824','I knew you would come, $N. It is good to see you again, friend.','0','0','0','1','8090','reginald windsor SAY_WELCOME'),

('-1000825','On guard, friend. The lady dragon will not give in without a fight.','0','0','0','1','8107','reginald windsor SAY_QUEST_ACCEPT'),
('-1000826','As was fated a lifetime ago in Karazhan, monster - I come - and with me I bring justice.','0','6','0','22','8109','reginald windsor SAY_GET_READY'),
('-1000827','Seize him! Seize the worthless criminal and his allies!','0','6','0','0','8119','prestor SAY_GONNA_DIE'),
('-1000828','Reginald, you know that I cannot let you pass.','0','0','0','1','8121','jonathan SAY_DIALOG_1'),
('-1000829','You must do what you think is right, Marcus. We served together under Turalyon. He made us both the men that we are today. Did he err with me? Do you truly believe my intent is to cause harm to our alliance? Would I shame our heroes?','0','0','0','1','8123','reginald windsor SAY_DIALOG_2'),
('-1000830','Holding me here is not the right decision, Marcus.','0','0','0','1','8133','reginald windsor SAY_DIALOG_3'),
('-1000831','%s appears lost in contemplation.','0','2','0','0','8124','jonathan EMOTE_CONTEMPLATION'),
('-1000832','I am ashamed, old friend. I know not what I do anymore. It is not you that would dare bring shame to the heroes of legend - it is I. It is I and the rest of these corrupt politicians. They fill our lives with empty promises, unending lies.','0','0','0','1','8125','jonathan SAY_DIALOG_4'),
('-1000833','We shame our ancestors. We shame those lost to us... forgive me, Reginald.','0','0','0','1','8132','jonathan SAY_DIALOG_5'),
('-1000834','Dear friend, you honor them with your vigilant watch. You are steadfast in your allegiance. I do not doubt for a moment that you would not give as great a sacrifice for your people as any of the heroes you stand under.','0','0','0','1','8126','reginald windsor SAY_DIALOG_6'),
('-1000835','Now, it is time to bring her reign to an end, Marcus. Stand down, friend.','0','0','0','1','8134','reginald windsor SAY_DIALOG_7'),
('-1000836','Stand down! Can you not see that heroes walk among us?','0','0','0','5','8127','jonathan SAY_DIALOG_8'),
('-1000837','Move aside! Let them pass!','0','0','0','5','8128','jonathan SAY_DIALOG_9'),
('-1000838','Reginald Windsor is not to be harmed! He shall pass through untouched!','0','1','0','22','8129','jonathan SAY_DIALOG_10'),
('-1000839','Go, Reginald. May the light guide your hand.','0','0','0','1','8130','jonathan SAY_DIALOG_11'),
('-1000840','Thank you, old friend. You have done the right thing.','0','0','0','1','8205','reginald windsor SAY_DIALOG_12'),
('-1000841','Follow me, friends. To Stormwind Keep!','0','0','0','0','8206','reginald windsor SAY_DIALOG_13'),
('-1000842','Light be with you, sir.','0','0','0','66','8167','guard SAY_1'),
('-1000843','We are but dirt beneath your feet, sir.','0','0','0','66','8177','guard SAY_2'),
('-1000844','...nerves of thorium.','0','0','0','66','8183','guard SAY_3'),
('-1000845','Make way!','0','0','0','66','8175','guard SAY_4'),
('-1000846','A living legend...','0','0','0','66','8184','guard SAY_5'),
('-1000847','A moment I shall remember for always.','0','0','0','66','8180','guard SAY_6'),
('-1000848','You are an inspiration to us all, sir.','0','0','0','66','8170','guard SAY_7'),
('-1000849','Be brave, friends. The reptile will thrash wildly. It is an act of desperation. When you are ready, give me the word.','0','0','0','25','8207','reginald windsor SAY_BEFORE_KEEP'),
('-1000850','Onward!','0','0','0','5','8208','reginald windsor SAY_GO_TO_KEEP'),
('-1000851','Majesty, run while you still can. She is not what you think her to be...','0','0','0','1','8210','reginald windsor SAY_IN_KEEP_1'),
('-1000852','To the safe hall, your majesty.','0','0','0','1','8212','bolvar SAY_IN_KEEP_2'),
('-1000853','The masquerade is over, Lady Prestor. Or should I call you by your true name... Onyxia...','0','0','0','25','8211','reginald windsor SAY_IN_KEEP_3'),
('-1000854','%s laughs.','0','2','0','11','10315','prestor EMOTE_IN_KEEP_LAUGH'),
('-1000855','You will be incarcerated and tried for treason, Windsor. I shall watch with glee as they hand down a guilty verdict and sentence you to death by hanging...','0','0','0','1','8215','prestor SAY_IN_KEEP_4'),
('-1000856','And as your limp body dangles from the rafters, I shall take pleasure in knowing that a mad man has been put to death. After all, what proof do you have? Did you expect to come in here and point your fingers at royalty and leave unscathed?','0','0','0','6','8216','prestor SAY_IN_KEEP_5'),
('-1000857','You will not escape your fate, Onyxia. It has been prophesied - a vision resonating from the great halls of Karazhan. It ends now...','0','0','0','1','8218','reginald windsor SAY_IN_KEEP_6'),
('-1000858','%s reaches into his pack and pulls out the encoded tablets...','0','2','0','0','8226','reginald windsor EMOTE_IN_KEEP_REACH'),
('-1000859','The Dark Irons thought these tablets to be encoded. This is not any form of coding, it is the tongue of ancient dragon.','0','0','0','1','8227','reginald windsor SAY_IN_KEEP_7'),
('-1000860','Listen, dragon. Let the truth resonate throughout these halls.','0','0','0','1','8219','reginald windsor SAY_IN_KEEP_8'),
('-1000861','%s reads from the tablets. Unknown, unheard sounds flow through your consciousness','0','2','0','0','8228','reginald windsor EMOTE_IN_KEEP_READ'),
('-1000862','%s gasps.','0','2','0','0','8236','bolvar EMOTE_IN_KEEP_GASP'),
('-1000863','Curious... Windsor, in this vision, did you survive? I only ask because one thing that I can and will assure is your death. Here and now.','0','0','0','1','8235','onyxia SAY_IN_KEEP_9'),
('-1000864','Dragon filth! Guards! Guards! Seize this monster!','0','1','0','22','8237','bolvar SAY_IN_KEEP_1'),
('-1000865','Yesss... Guards, come to your lord''s aid!','0','0','0','1','8239','onyxia SAY_IN_KEEP_10'),
('-1000866','DO NOT LET HER ESCAPE!','0','0','0','1','8247','reginald windsor SAY_IN_KEEP_11'),
('-1000867','Was this fabled, Windsor? If it was death that you came for then the prophecy has been fulfilled. May your consciousness rot in the Twisting Nether. Finish the rest of these meddlesome insects, children. Bolvar, you have been a pleasureable puppet.','0','0','0','0','8246','onyxia SAY_IN_KEEP_12'),
('-1000868','You have failed him, mortalsss... Farewell!','0','1','0','0','8248','onyxia SAY_IN_KEEP_12'),
('-1000869','Reginald... I... I am sorry.','0','0','0','0','8249','bolvar SAY_IN_KEEP_13'),
('-1000870','Bol... Bolvar... the medallion... use...','0','0','0','0','8250','reginald windsor SAY_IN_KEEP_14'),
('-1000871','%s dies.','0','2','0','0','12195','reginald windsor EMOTE_IN_KEEP_DIE'),
('-1000872','%s hisses','0','2','0','0','8245','reginald windsor EMOTE_GUARD_TRANSFORM'),

('-1000873','I know the way, insect. There is no need to prod me as if I were cattle.','0','0','0','1','4903','grark SAY_START'),
('-1000874','Surely you do not think that you will get away with this incursion. They will come for me and you shall pay for your insolence.','0','0','0','1','4904','grark SAY_PAY'),
('-1000875','RUN THEM THROUGH BROTHERS!','0','0','0','5','4905','grark SAY_FIRST_AMBUSH_START'),
('-1000876','I doubt you will be so lucky the next time you encounter my brethren.','0','0','0','1','4906','grark SAY_FIRST_AMBUSH_END'),
('-1000877','They come for you, fool!','0','0','0','5','4907','grark SAY_SEC_AMBUSH_START'),
('-1000878','What do you think you accomplish from this, fool? Even now, the Blackrock armies make preparations to destroy your world.','0','0','0','1','4908','grark SAY_SEC_AMBUSH_END'),
('-1000879','On darkest wing they fly. Prepare to meet your end!','0','0','0','5','4909','grark SAY_THIRD_AMBUSH_START'),
('-1000880','The worst is yet to come!','0','0','0','1','4911','grark SAY_THIRD_AMBUSH_END'),
('-1000881','%s laughs.','0','2','0','11','10315','grark EMOTE_LAUGH'),
('-1000882','Time to make your final stand, Insect.','0','0','0','0','4913','grark SAY_LAST_STAND'),
('-1000883','Kneel, Grark','0','0','0','1','4928','lexlort SAY_LEXLORT_1'),
('-1000884','Grark Lorkrub, you have been charged and found guilty of treason against Horde. How you plead is unimportant. High Executioner Nuzrak, step forward.','0','0','0','1','4929','lexlort SAY_LEXLORT_2'),
('-1000885','%s raises his massive axe over Grark.','0','2','0','27','4930','nuzark EMOTE_RAISE_AXE'),
('-1000886','%s raises his hand and then lowers it.','0','2','0','0','4932','lexlort EMOTE_LOWER_HAND'),
('-1000887','End him...','0','0','0','0','4931','lexlort SAY_LEXLORT_3'),
('-1000888','You, soldier, report back to Kargath at once!','0','0','0','1','4933','lexlort SAY_LEXLORT_4'),
('-1000889','%s submits.','0','2','0','0','4918','grark EMOTE_SUBMIT'),
('-1000890','You have come to play? Then let us play!','0','0','0','0','4927','grark SAY_AGGRO'),

('-1000891','Let''s do this... Just keep me covered and I''ll deliver the package.','0','0','0','0','18432','demolitionist SAY_INTRO'),
('-1000892','I''m under attack! I repeat, I am under attack!','0','0','0','0','18439','demolitionist SAY_ATTACK_1'),
('-1000893','I need to find a new line of work.','0','0','0','0','18440','demolitionist SAY_ATTACK_2'),
('-1000894','By the second sun of K''aresh, look at this place! I can only imagine what Salhadaar is planning. Come on, let''s keep going.','0','0','0','1','18433','demolitionist SAY_STAGING_GROUNDS'),
('-1000895','With this much void waste and run off, a toxic void horror can''t be too far behind.','0','0','0','0','18434','demolitionist SAY_TOXIC_HORROR'),
('-1000896','Look there, fleshling! Salhadaar''s conduits! He''s keeping well fed...','0','0','0','1','18435','demolitionist SAY_SALHADAAR'),
('-1000897','Alright, keep me protected while I plant this disruptor. This shouldn''t take very long...','0','0','0','0','18436','demolitionist SAY_DISRUPTOR'),
('-1000898','Protect the conduit! Stop the intruders!','0','0','0','0','18441','nexus stalkers SAY_PROTECT'),
('-1000899','Done! Back up! Back up!','0','0','0','0','18437','demolitionist SAY_FINISH_1'),
('-1000900','Looks like my work here is done. Report to the holo-image of Ameer over at the transporter.','0','0','0','1','18442','demolitionist SAY_FINISH_2'),

('-1000901','Thanks, friend. Will you help me get out of here?','0','0','0','1','18562','vanguard SAY_VANGUARD_INTRO'),
('-1000902','We''re not too far from the Protectorate Watch Post, $N. This way!','0','0','0','1','18566','vanguard SAY_VANGUARD_START'),
('-1000903','Commander! This fleshling rescued me!','0','0','0','0','18567','vanguard SAY_VANGUARD_FINISH'),
('-1000904','%s salutes $N.','0','2','0','0','18568','vanguard EMOTE_VANGUARD_FINISH'),

('-1000905','Ok, let''s go!!','0','0','0','1','1189','therylune SAY_THERYLUNE_START'),
('-1000906','I can make it the rest of the way. $N. THANKS!','0','0','0','1','1188','therylune SAY_THERYLUNE_START'),

('-1000907','%s sniffs at the air. A tuber is near!','0','2','0','0','18875','domesticated felboar EMOTE_SNIFF_AIR'),
('-1000908','%s starts to dig.','0','2','0','0','18876','domesticated felboar EMOTE_START_DIG'),
('-1000909','%s squeals with glee at its discovery.','0','2','0','0','18877','domesticated felboar EMOTE_SQUEAL'),

('-1000910','Shall we begin, my friend?','0','0','0','0','13613','anchorite truuen SAY_BEGIN'),
('-1000911','This area is known to be full of foul Scourge. You may want to take a moment to prepare any defenses at your disposal.','0','0','0','0','13614','anchorite truuen SAY_FIRST_STOP'),
('-1000912','Very well, let us continue.','0','0','0','0','13615','anchorite truuen SAY_CONTINUE'),
('-1000913','Beware! We are attacked!','0','0','0','0','13616','anchorite truuen SAY_FIRST_ATTACK'),
('-1000914','It must be the purity of the Mark of the Lightbringer that is drawing forth the Scourge to us. We must proceed with caution lest we overwhelmed!','0','0','0','0','13617','anchorite truuen SAY_PURITY'),
('-1000915','We are beset upon again! Defend yourself!','0','0','0','0','13618','anchorite truuen SAY_SECOND_ATTACK'),
('-1000916','The land truly needs to be cleansed by the Light! Let us continue on the tomb. It isn''t far now.','0','0','0','0','13619','anchorite truuen SAY_CLEANSE'),
('-1000917','Be welcome, friends!','0','0','0','0','13621','high priest thel''danis SAY_WELCOME'),
('-1000918','Thank you for coming in remembrance of me. Your efforts in recovering that symbol, while unnecessary, are certainly touching to an old man''s heart.','0','0','0','0','13622','ghost of uther SAY_EPILOGUE_1'),
('-1000919','Please, rise my friend. Keep the Blessing as a symbol of the strength of the Light and how heroes long gone might once again rise in each of us to inspire.','0','0','0','0','13623','ghost of uther SAY_EPILOGUE_2'),

('-1000920','%s turns to face you.','0','2','0','0','5639','lich_king_wyrmskull EMOTE_LICH_KING_FACE'),
('-1000921','Shamanism has brought you here... Its scent permeates the air. *The Lich King laughs* I was once a shaman.','14742','0','0','0','23147','lich_king_wyrmskull SAY_LICH_KING_1'),
('-1000922','Shall we prepare it for you, my lord?','0','0','0','0','23152','valkyr_soulclaimer SAY_PREPARE'),
('-1000923','No, minion. This one is not ready.','14743','0','0','0','23148','lich_king_wyrmskull SAY_LICH_KING_2'),
('-1000924','Do you feel it, mortal? Death seeps through me, enveloping all that I touch. With just a snap of my finger your soul will languish in damnation for all eternity.','14744','0','0','0','23149','lich_king_wyrmskull SAY_LICH_KING_3'),
('-1000925','But... It is not yet your time to serve the Lich King. Yes, a greater destiny awaits you. Power... You must become more powerful before you are to serve me.','14745','0','0','0','23150','lich_king_wyrmskull SAY_LICH_KING_4'),
('-1000926','Now watch, val''kyr. Observe as I apply pressure. Can you see that it is not yet ripe? Watch as it pops and falls lifeless to the floor.','14746','0','0','0','23151','lich_king_wyrmskull SAY_LICH_KING_5'),
('-1000927','Persistence or stupidity? It matters not. Let this be a lesson learned, mortal!','14747','0','0','0','27351','lich_king_wyrmskull SAY_LICH_KING_6'),

('-1000928','%s motions for silence.','0','3','0','25','23130','king_ymiron EMOTE_KING_SILENCE'),
('-1000929','Vrykul, your king implores you listen!','0','1','0','22','23134','king_ymiron SAY_KING_YMIRON_SPEECH_1'),
('-1000930','The Gods have abandonned us!','0','1','0','22','23135','king_ymiron SAY_KING_YMIRON_SPEECH_2'),
('-1000931','The crowd gasps in horror.','0','2','0','0','23136','king_ymiron EMOTE_YMIRON_CROWD_1'),
('-1000932','Even now, in our darkest hour, they mock us!','0','1','0','22','23137','king_ymiron SAY_KING_YMIRON_SPEECH_3'),
('-1000933','Where are the titans in out time of greatest need? Our women birth abberations - disfigured runts unable to even stand on their own! Weak and ugly... Useless...','0','1','0','22','23138','king_ymiron SAY_KING_YMIRON_SPEECH_4'),
('-1000934','Ymiron has toiled. Long have I sat upon my throne and thought hard of our plight. There is only one answer... One reason...','0','1','0','22','23139','king_ymiron SAY_KING_YMIRON_SPEECH_5'),
('-1000935','For who but the titans themselves could bestow such a curse? What could have such power?','0','1','0','22','23140','king_ymiron SAY_KING_YMIRON_SPEECH_6'),
('-1000936','And the answer is nothing... For it is the titans who have cursed us!','0','1','0','22','23141','king_ymiron SAY_KING_YMIRON_SPEECH_7'),
('-1000937','The crowd clamours.','0','2','0','0','23142','king_ymiron EMOTE_YMIRON_CROWD_2'),
('-1000938','On this day all Vrykul will shed their old beliefs! We denounce our old gods! All Vrykul will pledge their allegiance to Ymiron! Ymiron will protect our noble race!','0','1','0','22','23143','king_ymiron SAY_KING_YMIRON_SPEECH_8'),
('-1000939','The crowd cheers.','0','2','0','0','23144','king_ymiron EMOTE_YMIRON_CROWD_3'),
('-1000940','And now my first decree upon the Vrykul! All malformed infants born of Vrykul mother and father are to be destroyed upon birth! Our blood must remain pure always! Those found in violation of Ymiron''s decree will be taken to Gjalerbron for execution!','0','1','0','22','23145','king_ymiron SAY_KING_YMIRON_SPEECH_9'),
('-1000941','Vrykul must remain pure!','0','0','0','0','23127','king_ymiron_crowd SAY_YMIRON_CROWD_1'),
('-1000942','Show the aberrations no mercy, Ymiron!','0','0','0','0','23125','king_ymiron_crowd SAY_YMIRON_CROWD_2'),
('-1000943','Show them mercy, my king! They are of our flesh and blood!','0','0','0','0','23128','king_ymiron_crowd SAY_YMIRON_CROWD_3'),
('-1000944','They weaken us! Our strength is dilluted by their very existence! Destroy them all!','0','0','0','0','23129','king_ymiron_crowd SAY_YMIRON_CROWD_4'),
('-1000945','All hail our glorious king, Ymiron!','0','0','0','0','23123','king_ymiron_crowd SAY_YMIRON_CROWD_5'),
('-1000946','The King is going to speak!','0','0','0','0','23132','king_ymiron_crowd SAY_YMIRON_CROWD_6'),
('-1000947','Let him speak! Be silent!','0','0','0','0','23124','king_ymiron_crowd SAY_YMIRON_CROWD_7'),

('-1000948','Well then, let''s get this started. The longer we''re here, the more damage the undead could be doing back in Hilsbrad.','0','0','0','0','816','kinelory SAY_START'),
('-1000949','All right, this is where we really have to be on our paws. Be ready!','0','0','0','0','817','kinelory SAY_REACH_BOTTOM'),
('-1000950','Attack me if you will, but you won''t stop me from getting back to Quae.','0','0','0','0','897','kinelory SAY_AGGRO_KINELORY'),
('-1000951','You have my word that I shall find a use for your body after I''ve killed you, Kinelory.','0','0','0','0','896','jorell SAY_AGGRO_JORELL'),
('-1000952','Watch my rear! I''ll see what I can find in all this junk...','0','0','0','0','818','kinelory SAY_WATCH_BACK'),
('-1000953','%s begins rummaging through the apothecary''s belongings.','0','2','0','0','819','kinelory EMOTE_BELONGINGS'),
('-1000954','I bet Quae''ll think this is important. She''s pretty knowledgeable about these things--no expert, but knowledgeable.','0','0','0','0','821','kinelory SAY_DATA_FOUND'),
('-1000955','Okay, let''s get out of here quick quick! Try and keep up. I''m going to make a break for it.','0','0','0','0','822','kinelory SAY_ESCAPE'),
('-1000956','We made it! Quae, we made it!','0','0','0','0','892','kinelory SAY_FINISH'),
('-1000957','%s hands her pack to Quae.','0','2','0','0','891','kinelory EMOTE_HAND_PACK'),

('-1000958','Ok, let''s get started.','0','0','0','0','1610','stinky ignatz SAY_STINKY_BEGIN'),
('-1000959','Now let''s look for the herb.','0','0','0','0','1611','stinky ignatz SAY_STINKY_FIRST_STOP'),
('-1000960','Help! The beast is on me!','0','0','0','0','1630','stinky ignatz SAY_AGGRO_1'),
('-1000961','Help! I''m under attack!','0','0','0','0','1629','stinky ignatz SAY_AGGRO_2'),
('-1000962','I can make it from here. Thanks, $N! And talk to my employer about a reward!','0','0','0','0','1618','stinky ignatz SAY_STINKY_END'),

('-1000963','%s looks at you for a moment, then motions for you to follow.','0','2','0','0','20689','cenarion sparrowhawk EMOTE_FOLLOW'),
('-1000964','%s surveys the ground for the buried raven stones.','0','2','0','0','20675','cenarion sparrowhawk EMOTE_SURVEY'),
('-1000965','%s locates a buried raven stone.','0','2','0','0','20676','cenarion sparrowhawk EMOTE_LOCATE'),

('-1000966','I WILL CRUSH YOU LIKE A GNAT!','0','1','0','0','21557','reth''hedron SAY_LOW_HP'),
('-1000967','You will regret this, mortal! Reth''hedron will return... I will have my vengeance!','0','1','0','53','21606','reth''hedron SAY_EVENT_END'),

('-1000968','Very well. Before we head down there, take a moment to prepare yourself.','0','0','0','1','17940','drijya SAY_DRIJYA_START'),
('-1000969','Let''s proceed at a brisk pace.','0','0','0','0','17941','drijya SAY_DRIJYA_1'),
('-1000970','We''ll start at that first energy pylon, straight ahead. Remember, try to keep them off of me.','0','0','0','1','17942','drijya SAY_DRIJYA_2'),
('-1000971','Keep them off me!','0','0','0','0','17950','drijya SAY_DRIJYA_3'),
('-1000972','I''m done with this pylon. On to the next.','0','0','0','1','17943','drijya SAY_DRIJYA_4'),
('-1000973','Alright, pylon two down. Now for the heat mainfold.','0','0','0','1','17944','drijya SAY_DRIJYA_5'),
('-1000974','That should do it. The teleporter should blow any second now!','0','0','0','5','17945','drijya SAY_DRIJYA_6'),
('-1000975','Ok, let''s get out of here!','0','0','0','1','17946','drijya SAY_DRIJYA_7'),
('-1000976','Thank you, $n! I couldn''t have done it without you. You''ll let Gahruj know?','0','0','0','1','17949','drijya SAY_DRIJYA_COMPLETE'),

('-1000977','Oh, it''s on now! But you thought I''d be alone too, huh?!','0','0','0','0','5827','tapoke slim jahn SAY_AGGRO'),
('-1000978','Okay, okay! No need to get all violent. I''ll talk. I''ll talk!','0','0','0','20','1743','tapoke slim jahn SAY_DEFEAT'),
('-1000979','Whoa! This is way more than what I bargained for, you''re on your own, Slim!','0','0','0','0','5828','slim''s friend SAY_FRIEND_DEFEAT'),
('-1000980','I have a few notes from the job back at my place. I''ll get them and then meet you back in the inn.','0','0','0','1','1744','tapoke slim jahn SAY_NOTES'),

('-1000981','It is time. The rite of exorcism will now commence...','0','0','0','0','20400','anchorite barada SAY_EXORCISM_1'),
('-1000982','Prepare yourself. Do not allow the ritual to be interrupted or we may lose our patient...','0','0','0','1','20401','anchorite barada SAY_EXORCISM_2'),
('-1000983','Keep away. The fool is mine.','0','0','0','0','20402','colonel jules SAY_EXORCISM_3'),
('-1000984','Back, foul beings of darkness! You have no power here!','0','0','0','0','20404','anchorite barada SAY_EXORCISM_4'),
('-1000985','No! Not yet! This soul is ours!','0','0','0','0','20412','colonel jules SAY_EXORCISM_5'),
('-1000986','Back! I cast you back... corrupter of faith! Author of pain! Do not return, or suffer the same fate as you did here today!','0','0','0','2','20419','anchorite barada SAY_EXORCISM_6'),
('-1000987','I... must not...falter!','0','0','0','0','20407','anchorite barada SAY_EXORCISM_RANDOM_1'),
('-1000988','Be cleansed with Light, human! Let not the demonic corruption overwhelm you.','0','0','0','0','20403','anchorite barada SAY_EXORCISM_RANDOM_2'),
('-1000989','Back, foul beings of darkness! You have no power here!','0','0','0','0','20404','anchorite barada SAY_EXORCISM_RANDOM_3'),
('-1000990','This is fruitless, draenei! You and your little helper cannot wrest control of this pathetic human. He is mine!','0','0','0','0','20416','colonel jules SAY_EXORCISM_RANDOM_4'),
('-1000991','I see your ancestors, Anchorite! They writhe and scream in the darkness... they are with us!','0','0','0','0','20415','colonel jules SAY_EXORCISM_RANDOM_5'),
('-1000992','I will tear your soul into morsels and slow roast them over demon fire!','0','0','0','0','20417','colonel jules SAY_EXORCISM_RANDOM_6'),

('-1000993','It''s on! $N, meet my fists. Fists, say hello to $N.','0','0','0','0','4353','dorius stonetender SAY_AGGRO_1'),
('-1000994','I''m about to open a can on this $N.','0','0','0','0','4351','dorius stonetender SAY_AGGRO_2'),

('-1000995','Fhwoor go now, $N. Get ark, come back.','0','0','0','0','14639','fhwoor SAY_ESCORT_START'),
('-1000996','Take moment... get ready.','0','0','0','0','14640','fhwoor SAY_PREPARE'),
('-1000997','We go!','0','0','0','0','14641','fhwoor SAY_CAMP_ENTER'),
('-1000998','Uh oh...','0','0','0','0','10828','fhwoor SAY_AMBUSH'),
('-1000999','Ha ha, squishy naga!','0','0','0','0','14644','fhwoor SAY_AMBUSH_CLEARED'),
('-1001000','Fhwoor do good!','0','0','0','0','14646','fhwoor SAY_ESCORT_COMPLETE'),

('-1001001','We must leave before more are alerted.','0','0','0','1','15046','kurenai captive SAY_KUR_START'),
('-1001002','It''s an ambush! Defend yourself!','0','0','0','5','15044','kurenai captive SAY_KUR_AMBUSH_1'),
('-1001003','We are surrounded!','0','0','0','5','15047','kurenai captive SAY_KUR_AMBUSH_2'),
('-1001004','Up ahead is the road to Telaar. We will split up when we reach the fork as they will surely send more Murkblood after us. Hopefully one of us makes it back to Telaar alive.','0','0','0','25','15048','kurenai captive SAY_KUR_COMPLETE'),
('-1001005','Farewell, stranger. Your heroics will be remembered by my people. Now, hurry to Telaar!','0','0','0','1','15049','kurenai captive SAY_KUR_COMPLETE_2'),

('-1001006','Thanks for your help. Let''s get out of here!','0','0','0','1','21481','skyguard prisoner SAY_ESCORT_START'),
('-1001007','Let''s keep moving. I don''t like this place.','0','0','0','1','21484','skyguard prisoner SAY_AMBUSH_END'),
('-1001008','Thanks again. Sergeant Doryn will be glad to hear he has one less scout to replace this week.','0','0','0','1','21485','skyguard prisoner SAY_ESCORT_COMPLETE'),
('-1001009','Death to our enemies!','0','0','0','0','21493','skettis wing guard SAY_AMBUSH_1'),
('-1001010','No one escapes Skettis!','0','0','0','0','21492','skettis wing guard SAY_AMBUSH_2'),
('-1001011','Skettis prevails!','0','0','0','0','21131','skettis wing guard SAY_AMBUSH_3'),
('-1001012','You''ll go nowhere, Skyguard scum!','0','0','0','0','21490','skettis wing guard SAY_AMBUSH_4'),

('-1001013','Right then, no time to waste. Let''s get outa here!','0','0','0','1','24772','bonker togglevolt SAY_BONKER_START'),
('-1001014','Here we go.','0','0','0','0','24773','bonker togglevolt SAY_BONKER_GO'),
('-1001015','I AM NOT AN APPETIZER!','0','0','0','0','24780','bonker togglevolt SAY_BONKER_AGGRO'),
('-1001016','I think it''s up this way to the left. Let''s go!','0','0','0','1','24776','bonker togglevolt SAY_BONKER_LEFT'),
('-1001017','Ah, fresh air! I can get myself back to the airstrip from here. Be sure to tell Fizzcrank I''m back and safe. Thanks so much, $N!','0','0','0','1','24774','sbonker togglevolt SAY_BONKER_COMPLETE'),

('-1001018','On the move, men!','0','0','0','0','32409','kor''kron squad leader SAY_HORDER_RUN'),
('-1001019','Alright boys, let''s do this!','0','0','0','0','32351','skybreaker squad leader SAY_ALLIANCE_RUN'),
('-1001020','Incoming!','0','1','0','0','32397','squad leader SAY_AGGRO_1'),
('-1001021','Ambush!','0','1','0','0','32400','squad leader SAY_AGGRO_2'),
('-1001022','For the Horde!','0','1','0','0','11642','kor''kron squad leader SAY_HORDE_AGGRO_1'),
('-1001023','Time for some blood, men!','0','1','0','0','32416','kor''kron squad leader SAY_HORDE_AGGRO_2'),
('-1001024','Vrykul!','0','1','0','0','32414','kor''kron squad leader SAY_HORDE_AGGRO_3'),
('-1001025','Weapons out!','0','1','0','0','32415','kor''kron squad leader SAY_HORDE_AGGRO_4'),
('-1001026','Find some cover!','0','1','0','0','32402','skybreaker squad leader SAY_ALLIANCE_AGGRO_1'),
('-1001027','Group up!','0','1','0','0','32403','skybreaker squad leader SAY_ALLIANCE_AGGRO_2'),
('-1001028','On your feet, boys!','0','1','0','0','32399','skybreaker squad leader SAY_ALLIANCE_AGGRO_3'),
('-1001029','Vrykul attack!','0','1','0','0','32404','skybreaker squad leader SAY_ALLIANCE_AGGRO_4'),
('-1001030','Quickly, catch your breaths before we press for the gate!','0','0','0','0','32410','kor''kron squad leader SAY_HORDE_BREAK'),
('-1001031','On your feet, men! Move, move move!','0','0','0','0','32411','kor''kron squad leader SAY_HORDE_BREAK_DONE'),
('-1001032','Nice work! We can only rest a moment.','0','0','0','0','32352','skybreaker squad leader SAY_ALLIANCE_BREAK'),
('-1001033','On your feet, boys! Move, move move!','0','0','0','0','32405','skybreaker squad leader SAY_ALLIANCE_BREAK_DONE'),
('-1001034','Thanks for keeping us covered back there! We''ll hold the gate while we wait for reinforcements.','0','0','0','1','32353','squad leader SAY_EVENT_COMPLETE'),
('-1001035','Die, maggot!','0','0','0','0','30501','ymirheim defender SAY_DEFENDER_AGGRO_1'),
('-1001036','Haraak foln!','0','0','0','0','30506','ymirheim defender SAY_DEFENDER_AGGRO_2'),
('-1001037','I spit on you!','0','0','0','0','30503','ymirheim defender SAY_DEFENDER_AGGRO_3'),
('-1001038','I will feed you to the dogs!','0','0','0','0','30499','ymirheim defender SAY_DEFENDER_AGGRO_4'),
('-1001039','I will take pleasure in gutting you!','0','0','0','0','30498','ymirheim defender SAY_DEFENDER_AGGRO_5'),
('-1001040','I''ll eat your heart!','0','0','0','0','30508','ymirheim defender SAY_DEFENDER_AGGRO_6'),
('-1001041','Sniveling pig!','0','0','0','0','30504','ymirheim defender SAY_DEFENDER_AGGRO_7'),
('-1001042','Ugglin oo bjorr!','0','0','0','0','30505','ymirheim defender SAY_DEFENDER_AGGRO_8'),
('-1001043','You come to die!','0','0','0','0','30502','ymirheim defender SAY_DEFENDER_AGGRO_9'),

('-1001044','The Light''s blessing be upon you for aiding me in my time of need, $N.','0','0','0','0','31941','father kamaros SAY_ESCORT_START_1'),
('-1001045','I''ve had my fill of this place. Let us depart.','0','0','0','1','31942','father kamaros SAY_ESCORT_START_2'),
('-1001046','Face your judgment by the Light!','0','0','0','0','31950','father kamaros SAY_AGGRO_1'),
('-1001047','The Argent Crusade never surrenders!','0','0','0','0','31953','father kamaros SAY_AGGRO_2'),
('-1001048','You will never take me alive!','0','0','0','0','31952','father kamaros SAY_AGGRO_3'),
('-1001049','I have you to thank for my life. I will return to my comrades and spread word of your bravery. Fight the Scourge with all the strength you can muster, and we will be by your side.','0','0','0','1','31943','father kamaros SAY_ESCORT_COMPLETE_2'),
('-1001050','You must tell my brothers that I live.','0','0','0','1','31944','father kamaros SAY_ESCORT_COMPLETE_1'),

('-1001051','Let me know when you''re ready. I''d prefer sooner than later... what with the slowly dying from poison and all.','0','0','0','1','30645','injured goblin miner SAY_ESCORT_READY'),
('-1001052','I''m going to bring the venom sac to Ricket... and then... you know... collapse. Thank you for helping me!','0','0','0','1','30646','injured goblin miner SAY_ESCORT_COMPLETE'),

('-1001053','Alright, kid. Stay behind me and you''ll be fine.','0','0','0','36','26027','harrison jones SAY_ESCORT_START'),
('-1001054','Their ceremonial chamber, where I was to be sacrificed...','0','0','0','1','26028','harrison jones SAY_CHAMBER_1'),
('-1001055','Time to put an end to all this!','0','0','0','1','26029','harrison jones SAY_CHAMBER_2'),
('-1001056','You''re free to go, miss.','0','0','0','1','26057','harrison jones SAY_CHAMBER_RELEASE'),
('-1001057','Thank you!','0','0','0','71','18708','Adarrah SAY_THANK_YOU'),
('-1001058','Odd. That usually does it.','0','0','0','1','26030','harrison jones SAY_CHAMBER_3'),
('-1001059','Just as well, I''ve had enough of this place.','0','0','0','1','26053','harrison jones SAY_CHAMBER_4'),
('-1001060','What''s this?','0','0','0','0','26036','harrison jones SAY_CHAMBER_5'),
('-1001061','Aww, not a snake!','0','0','0','1','26037','harrison jones SAY_CHAMBER_6'),
('-1001062','Listen, kid. I can handle this thing. You just watch my back!','0','0','0','1','26065','harrison jones SAY_CHAMBER_7'),
('-1001063','See ya ''round, kid!','0','0','0','1','26066','harrison jones SAY_ESCORT_COMPLETE'),

('-1001064','You couldn''t have come at a better time! Let''s get out of here.','0','0','0','0','22485','apothecary hanes SAY_ESCORT_START'),
('-1001065','Yes, let us leave... but not before we leave our Alliance hosts something to remember us by!','0','0','0','0','22486','apothecary hanes SAY_FIRE_1'),
('-1001066','They have limited supplies in this camp. It would be a real shame if something were to happen to them.','0','0','0','16','22487','apothecary hanes SAY_FIRE_2'),
('-1001067','Ah, yes... watch it burn!','0','0','0','0','22489','apothecary hanes SAY_SUPPLIES_1'),
('-1001068','We''re almost done!','0','0','0','0','22490','apothecary hanes SAY_SUPPLIES_2'),
('-1001069','Let''s high-tail it out of here.','0','0','0','0','22492','apothecary hanes SAY_SUPPLIES_ESCAPE'),
('-1001070','That''ll teach you to mess with an apothecary, you motherless Alliance dogs!','0','1','0','22','22491','apothecary hanes SAY_SUPPLIES_COMPLETE'),
('-1001071','Don''t shoot! Apothecary coming through!','0','1','0','0','22493','apothecary hanes SAY_ARRIVE_BASE'),

('-1001072','Something is wrong with the Highlord. Do something!','0','0','0','1','7320','scarlet cavalier SAY_CAVALIER_WORRY_1'),
('-1001073','Hey, what is going on over there? Sir, are you alright?','0','0','0','1','7322','scarlet cavalier SAY_CAVALIER_WORRY_2'),
('-1001074','What the....','0','0','0','1','7321','scarlet cavalier SAY_CAVALIER_WORRY_3'),
('-1001075','Sir?','0','0','0','1','7319','scarlet cavalier SAY_CAVALIER_WORRY_4'),
('-1001076','NOOOOOOOOOOOOO!','0','1','0','15','0','taelan fordring SAY_SCARLET_COMPLETE_1'),
('-1001077','I will lead us through Hearthglen to the forest''s edge. From there, you will take me to my father.','0','0','0','1','7313','taelan fordring SAY_SCARLET_COMPLETE_2'),
('-1001078','Remove your disguise, lest you feel the bite of my blade when the fury has taken control.','0','0','0','1','7314','taelan fordring SAY_ESCORT_START'),
('-1001079','Halt.','0','0','0','0','7315','taelan fordring SAY_TAELAN_MOUNT'),
('-1001080','%s calls for his mount.','0','2','0','22','7316','taelan fordring EMOTE_TAELAN_MOUNT'),
('-1001081','It''s not much further. The main road is just up ahead.','0','0','0','1','7329','taelan fordring SAY_REACH_TOWER'),
('-1001082','You will not make it to the forest''s edge, Fordring.','0','1','0','1','7395','isillien SAY_ISILLIEN_1'),
('-1001083','Isillien!','0','1','0','25','7370','taelan fordring SAY_ISILLIEN_2'),
('-1001084','This is not your fight, stranger. Protect yourself from the attacks of the Crimson Elite. I shall battle the Grand Inquisitor.','0','0','0','1','7371','taelan fordring SAY_ISILLIEN_3'),
('-1001085','You disappoint me, Taelan. I had plans for you... grand plans. Alas, it was only a matter of time before your filthy bloodline would catch up with you.','0','0','0','1','7337','isillien SAY_ISILLIEN_4'),
('-1001086','It is as they say: Like father, like son. You are as weak of will as Tirion... perhaps more so. I can only hope my assassins finally succeeded in ending his pitiful life.','0','0','0','1','7338','isillien SAY_ISILLIEN_5'),
('-1001087','The Grand Crusader has charged me with destroying you and your newfound friends, Taelan, but know this: I do this for pleasure, not of obligation or duty.','0','0','0','1','7359','isillien SAY_ISILLIEN_6'),
('-1001088','%s calls for his guardsman.','0','2','0','22','7360','isillien EMOTE_ISILLIEN_ATTACK'),
('-1001089','The end is now, Fordring.','0','0','0','25','7361','isillien SAY_ISILLIEN_ATTACK'),
('-1001090','Enough!','0','0','0','0','7381','isillien SAY_KILL_TAELAN_1'),
('-1001091','%s laughs.','0','2','0','11','10315','isillien EMOTE_ISILLIEN_LAUGH'),
('-1001092','Did you really believe that you could defeat me? Your friends are soon to join you, Taelan.','0','0','0','0','7383','isillien SAY_KILL_TAELAN_2'),
('-1001093','%s turns his attention towards you.','0','2','0','0','7384','isillien EMOTE_ATTACK_PLAYER'),
('-1001094','What have you done, Isillien? You once fought with honor, for the good of our people... and now... you have murdered my boy...','0','0','0','0','7372','tirion fordring SAY_TIRION_1'),
('-1001095','Tragic. The elder Fordring lives on... You are too late, old man. Retreat back to your cave, hermit, unless you wish to join your son in the Twisting Nether.','0','0','0','0','7433','isillien SAY_TIRION_2'),
('-1001096','May your soul burn in anguish, Isillien! Light give me strength to battle this fiend.','0','0','0','15','7373','tirion fordring SAY_TIRION_3'),
('-1001097','Face me, coward. Face the faith and strength that you once embodied.','0','0','0','25','7374','tirion fordring SAY_TIRION_4'),
('-1001098','Then come, hermit!','0','0','0','0','7436','isillien SAY_TIRION_5'),
('-1001099','A thousand more like him exist. Ten thousand. Should one fall, another will rise to take the seat of power.','0','0','0','0','7420','tirion fordring SAY_EPILOG_1'),
('-1001100','%s falls to one knee.','0','2','0','16','7421','tirion fordring EMOTE_FALL_KNEE'),
('-1001101','Look what they did to my boy.','0','0','0','5','7423','tirion fordring SAY_EPILOG_2'),
('-1001102','%s holds the limp body of Taelan Fordring and softly sobs.','0','2','0','0','7425','tirion fordring EMOTE_HOLD_TAELAN'),
('-1001103','Too long have I sat idle, gripped in this haze... this malaise, lamenting what could have been... what should have been.','0','0','0','0','7429','tirion fordring SAY_EPILOG_3'),
('-1001104','Your death will not have been in vain, Taelan. A new Order is born on this day... an Order which will dedicate itself to extinguishing the evil that plagues this world. An evil that cannot hide behind politics and pleasantries.','0','0','0','0','7426','tirion fordring SAY_EPILOG_4'),
('-1001105','This I promise... This I vow...','0','0','0','15','7430','tirion fordring SAY_EPILOG_5'),

('-1001106','Don''t forget to get my bell out of the chest here. And remember, if do happen to wander off, just ring it and I''ll find you again.','0','0','0','1','3921','shay leafrunner SAY_ESCORT_START'),
('-1001107','Are we taking the scenic route?','0','0','0','0','3912','shay leafrunner SAY_WANDER_1'),
('-1001108','Oh, what a beautiful flower over there...','0','0','0','0','3907','shay leafrunner SAY_WANDER_2'),
('-1001109','Are you sure this is the right way? Maybe we should go this way instead...','0','0','0','0','3909','shay leafrunner SAY_WANDER_3'),
('-1001110','Hmmm, I wonder what''s over this way?','0','0','0','0','3911','shay leafrunner SAY_WANDER_4'),
('-1001111','This is quite an adventure!','0','0','0','0','3914','shay leafrunner SAY_WANDER_DONE_1'),
('-1001112','Oh, I wandered off again. I''m sorry.','0','0','0','0','3913','shay leafrunner SAY_WANDER_DONE_2'),
('-1001113','The bell again, such a sweet sound.','0','0','0','0','3916','shay leafrunner SAY_WANDER_DONE_3'),
('-1001114','%s begins to wander off.','0','2','0','0','3918','shay leafrunner EMOTE_WANDER'),
('-1001115','Oh, here you are, Rockbiter! I''m sorry, I know I''m not supposed to wander off.','0','0','0','1','3917','shay leafrunner SAY_EVENT_COMPLETE_1'),
('-1001116','I''m so glad yer back Shay. Please, don''t ever run off like that again! What would I tell yer parents if I lost ya?','0','0','0','1','3922','rockbiter SAY_EVENT_COMPLETE_2'),

('-1001117','AHAHAHAHA... you''ll join us soon enough!','0','1','0','0','32473','saronite mine slave SAY_MINER_SUICIDE_1'),
('-1001118','I don''t want to leave! I want to stay here!','0','1','0','0','32471','saronite mine slave SAY_MINER_SUICIDE_2'),
('-1001119','I must get further underground to where he is. I must jump!','0','1','0','0','32474','saronite mine slave SAY_MINER_SUICIDE_3'),
('-1001120','I won''t leave!','0','1','0','0','32469','saronite mine slave SAY_MINER_SUICIDE_4'),
('-1001121','I''ll never return. The whole reason for my existence awaits below!','0','1','0','0','32472','saronite mine slave SAY_MINER_SUICIDE_5'),
('-1001122','I''m coming, master!','0','1','0','0','32466','saronite mine slave SAY_MINER_SUICIDE_6'),
('-1001123','My life for you!','0','1','0','0','32467','saronite mine slave SAY_MINER_SUICIDE_7'),
('-1001124','NO! You''re wrong! The voices in my head are beautiful!','0','1','0','0','32470','saronite mine slave SAY_MINER_SUICIDE_8'),

('-1001125','Beginning the distillation in 5 seconds.','0','0','0','0','28514','tipsy mcmanus SAY_DISTILLATION_START'),
('-1001126','Add another orange! Quickly!','0','0','0','25','28515','tipsy mcmanus SAY_ADD_ORANGE'),
('-1001127','Add bananas!','0','0','0','25','28516','tipsy mcmanus SAY_ADD_BANANAS'),
('-1001128','Put a papaya in the still!','0','0','0','25','28517','tipsy mcmanus SAY_ADD_PAPAYA'),
('-1001129','The still needs heat! Light the brazier!','0','0','0','5','28518','tipsy mcmanus SAY_LIGHT_BRAZIER'),
('-1001130','Pressure''s too high! Open the pressure valve!','0','0','0','5','28519','tipsy mcmanus SAY_OPEN_VALVE'),
('-1001131','Good job! Keep your eyes open, now.','0','0','0','4','28534','tipsy mcmanus SAY_ACTION_COMPLETE_1'),
('-1001132','Nicely handled! Stay on your toes!','0','0','0','4','28533','tipsy mcmanus SAY_ACTION_COMPLETE_2'),
('-1001133','Well done! Be ready for anything!','0','0','0','4','28524','tipsy mcmanus SAY_ACTION_COMPLETE_3'),
('-1001134','That''ll do. Never know what it''ll need next...','0','0','0','4','28535','tipsy mcmanus SAY_ACTION_COMPLETE_4'),
('-1001135','It''s no good! I''m shutting it down...','0','0','0','0','28523','tipsy mcmanus SAY_DISTILLATION_FAIL'),
('-1001136','We''ve done it! Come get the cask.','0','0','0','0','28521','tipsy mcmanus SAY_DISTILLATION_COMPLETE'),

('-1001137','The duel will begin in...','0','5','0','0','29258','death knight initiate EMOTE_DUEL_BEGIN'),
('-1001138','3...','0','5','0','0','18678','death knight initiate EMOTE_DUEL_BEGIN_3'),
('-1001139','2...','0','5','0','0','17356','death knight initiate EMOTE_DUEL_BEGIN_2'),
('-1001140','1...','0','5','0','0','17355','death knight initiate EMOTE_DUEL_BEGIN_1'),

('-1001141','Nope, not here...','0','0','0','0','1612','stinky ignatz SAY_SECOND_STOP'),
('-1001142','There must be one around here somewhere...','0','0','0','0','1613','stinky ignatz SAY_THIRD_STOP_1'),
('-1001143','Ah, there''s one!','0','0','0','0','1614','stinky ignatz SAY_THIRD_STOP_2'),
('-1001144','Come, $N! Let''s go over there and collect it!','0','0','0','0','1615','stinky ignatz SAY_THIRD_STOP_3'),
('-1001145','Ok, let''s get out of here!','0','0','0','0','17946','stinky ignatz SAY_PLANT_GATHERED'),
('-1001146','I''m glad you''re here! Because I need your help!!','0','0','0','0','1631','stinky ignatz SAY_AGGRO_3'),
('-1001147','Look out! The $N attacks!','0','0','0','0','1628','stinky ignatz SAY_AGGRO_4'),

('-1001148','I am ready, $N. Let''s find my equipment and get out of here. I think I know where it is.','0','0','0','1','6433','captured arko''narin SAY_ESCORT_START'),
('-1001149','Oh my! Look at this... all these candles. I''m sure they''re used for some terrible ritual or dark summoning. We best make haste.','0','0','0','18','6456','captured arko''narin SAY_FIRST_STOP'),
('-1001150','There! Over there!','0','0','0','25','6457','captured arko''narin SAY_SECOND_STOP'),
('-1001151','You will not stop me from escaping here, $N!','0','0','0','0','6801','captured arko''narin SAY_AGGRO'),
('-1001152','All I need now is a golden lasso.','0','0','0','0','6458','captured arko''narin SAY_EQUIPMENT'),
('-1001153','DIE DEMON DOGS!','0','0','0','0','6460','captured arko''narin SAY_ESCAPE'),
('-1001154','Ah! Fresh air at last! I never thought I''d see the day.','0','0','0','4','6461','captured arko''narin SAY_FRESH_AIR'),
('-1001155','BETRAYER!','0','1','0','0','6466','spirit of trey lightforge SAY_BETRAYER'),
('-1001156','What was that?! Trey? TREY?','0','0','0','22','6463','captured arko''narin SAY_TREY'),
('-1001157','You kept me in the cell for too long, monster!','0','0','0','0','6802','captured arko''narin SAY_ATTACK_TREY'),
('-1001158','No! My friend... what''s happened? This is all my fault...','0','0','0','18','6468','captured arko''narin SAY_ESCORT_COMPLETE'),

('-1001159','Please, help me to get through this cursed forest, $r.','0','0','0','0','5004','arei SAY_ESCORT_START'),
('-1001160','This creature suffers from the effect of the fel... We must end its misery.','0','0','0','0','5474','arei SAY_ATTACK_IRONTREE'),
('-1001161','The corruption of the fel has not left any of the creatures of Felwood untouched, $N. Please, be on your guard.','0','0','0','0','5005','arei SAY_ATTACK_TOXIC_HORROR'),
('-1001162','I sense the taint of corruption upon this $N. Help me detroy it.','0','0','0','0','5473','arei SAY_EXIT_WOODS'),
('-1001163','That I must fight against my own kind deeply saddens me.','0','0','0','0','5008','arei SAY_CLEAR_PATH'),
('-1001164','I can sense it now, $N. Ashenvale lies down this path.','0','0','0','0','5023','arei SAY_ASHENVALE'),
('-1001165','I feel... something strange...','0','0','0','0','5026','arei SAY_TRANSFORM'),
('-1001166','$N my form has now changed! The true strength of my spirit is returning to me now... The cursed grasp of the forest is leaving me.','0','0','0','0','5029','arei SAY_LIFT_CURSE'),
('-1001167','Thank you, $N. Now my spirit will finally be at peace.','0','0','0','0','5036','arei SAY_ESCORT_COMPLETE'),

('-1001168','The naga torture the spirits of water. They invoke chaos and destruction!','0','0','0','0','18674','wilda SAY_WIL_PROGRESS_4'),
('-1001169','The naga do not respect nature. They twist and corrupt it to meet their needs. They live to agitate the spirits.','0','0','0','0','18673','wilda SAY_WIL_PROGRESS_5'),

('-1001170','Time only has meaning to mortals, insect. Dimensius is infinite!','0','1','0','0','18647','dimensius SAY_AGGRO'),
('-1001171','I hunger! Feed me the power of this forge, my children!','0','1','0','0','19420','dimensius SAY_SUMMON'),

('-1001172','Spare my life! I will tell you about Arelion''s secret.','0','0','0','0','17755','magister_aledis SAY_ALEDIS_DEFEAT'),

('-1001173','Are you ready, Mr. Floppy? Stay close to me and watch out for those wolves!','0','0','0','0','25802','emily SAY_ESCORT_START'),
('-1001174','Um... I think one of those wolves is back...','0','0','0','0','25803','emily SAY_FIRST_WOLF'),
('-1001175','He''s going for Mr. Floppy!','0','0','0','0','25804','emily SAY_WOLF_ATTACK'),
('-1001176','There''s a big meanie attacking Mr. Floppy! Help!','0','0','0','0','26893','emily SAY_HELP_FLOPPY_1'),
('-1001177','Let''s get out of here before more wolves find us!','0','0','0','0','25805','emily SAY_FIRST_WOLF_DEFEAT'),
('-1001178','Oh, no! Look, it''s another wolf, and it''s a biiiiiiig one!','0','0','0','0','25806','emily SAY_SECOND_WOLF'),
('-1001179','He''s gonna eat Mr. Floppy! You gotta help Mr. Floppy! You just gotta!','0','0','0','0','26892','emily SAY_HELP_FLOPPY_2'),
('-1001180','Don''t go toward the light, Mr. Floppy!','0','0','0','0','25809','emily SAY_FLOPPY_ALMOST_DEAD'),
('-1001181','Mr. Floppy, you''re ok! Thank you so much for saving Mr. Floppy!','0','0','0','0','25810','emily SAY_SECOND_WOLF_DEFEAT'),
('-1001182','I think I see the camp! We''re almost home, Mr. Floppy! Let''s go!','0','0','0','0','26894','emily SAY_RESUME_ESCORT'),
('-1001183','Thank you for helping me to get back to the camp. Go tell Walter that I''m safe now!','0','0','0','0','26891','emily SAY_ESCORT_COMPLETE'),

('-1001184','How did you find me? Did Landgren tell?','14201','0','0','0','30281','admiral_westwind SAY_AGGRO'),
('-1001185','You thought I would just let you kill me?','14205','0','0','0','30279','admiral_westwind SAY_SPHERE'),
('-1001186','WHAT?! No matter. Even without my sphere, I will crush you! Behold my true identity and despair!','14207','1','0','0','30273','admiral_westwind SAY_NO_MATTER'),
('-1001187','Gah! I spent too much time in that weak little shell.','14426','1','0','0','30275','malganis_icecrown SAY_TRANSFORM'),
('-1001188','Kirel narak! I am Mal''Ganis. I AM ETERNAL!','14427','1','0','0','30276','malganis_icecrown SAY_20_HP'),
('-1001189','ENOUGH! I waste my time here. I must gather my strength on the homeworld.','14428','1','0','0','30277','malganis_icecrown SAY_DEFEATED'),
('-1001190','You''ll never defeat the Lich King without my forces. I''ll have my revenge... on him AND you!','14429','1','0','0','30278','malganis_icecrown SAY_ESCAPE'),

('-1001191','Hey, you call yourself a body guard? Get to work and protect us...','0','0','0','0','7310','SAY_CORK_AMBUSH1'),
('-1001192','Mister body guard, are you going to earn your money or what?','0','0','0','0','7311','SAY_CORK_AMBUSH2'),
('-1001193','You''re fired! <Cough...Cork clears throat.> I mean, help!','0','0','0','0','7312','SAY_CORK_AMBUSH3'),
('-1001194','Blast those stupid centaurs! Sigh - well, it seems you kept your bargain. Up the road you shall find Smeed Scrabblescrew, he has your money.','0','0','0','1','7334','SAY_CORK_END'),
('-1001195','Eeck! Demons hungry for the kodos!','0','0','0','0','7330','SAY_RIGGER_AMBUSH1'),
('-1001196','What am I paying you for? The kodos are nearly dead!','0','0','0','0','7331','SAY_RIGGER_AMBUSH2'),
('-1001197','Wow! We did it... not sure why we thought we needed the likes of you. Nevertheless, speak with Smeed Srablescrew; he will give you your earnings!','0','0','0','0','7333','SAY_RIGGER_END'),

('-1001198','Cover me!','0','0','0','5','26852','demolitionist_legoso SAY_ESCORT_2'),
('-1001199','It won''t be much longer, $n. Just keep them off me while I work.','0','0','0','133','14651','demolitionist_legoso SAY_ESCORT_3'),
('-1001200','That''ll do it! Quickly, take cover!','0','0','0','5','14652','demolitionist_legoso SAY_ESCORT_4'),
('-1001201','3...','0','0','0','0','18678','demolitionist_legoso SAY_ESCORT_COUNT_3'),
('-1001202','2...','0','0','0','0','17356','demolitionist_legoso SAY_ESCORT_COUNT_2'),
('-1001203','1...','0','0','0','0','17355','demolitionist_legoso SAY_ESCORT_COUNT_1'),
('-1001204','Don''t get too excited, hero, that was the easy part. The challenge lies ahead! Let''s go.','0','0','0','1','14656','demolitionist_legoso SAY_ESCORT_CONTINUE'),
('-1001205','What in the Nether is that?!?!','0','0','0','5','14657','demolitionist_legoso SAY_ESCORT_5'),
('-1001206','Be ready for anything, $n.','0','0','0','1','14658','demolitionist_legoso SAY_ESCORT_6'),
('-1001207','Blessed Light! She''s siphoning energy right out of the Vector Coil!','0','0','0','34','14659','demolitionist_legoso SAY_ESCORT_7'),
('-1001208','Cover me, we have to do this quickly. Once I blow the support on this side, it will disrupt the energy beams and she''ll break out! I doubt very much that she''ll be happy to see us.','0','0','0','1','14660','demolitionist_legoso SAY_ESCORT_8'),
('-1001209','I''ve almost got it! Just a little more time...','0','0','0','0','14661','demolitionist_legoso SAY_ESCORT_9'),
('-1001210','Take cover and be ready for the fight of your life, $n!','0','0','0','5','14662','demolitionist_legoso SAY_ESCORT_10'),
('-1001211','Holy mother of O''ros!','0','0','0','5','14663','demolitionist_legoso SAY_ESCORT_AGGRO'),
('-1001212','I... I can''t believe it''s over. You did it! You''ve destroyed the blood elves and their leader!','0','0','0','1','14664','demolitionist_legoso SAY_ESCORT_COMPLETE_1'),
('-1001213','Get back to Blood Watch. I''ll see you there...','0','0','0','1','14665','demolitionist_legoso SAY_ESCORT_COMPLETE_2'),

('-1001214','%s becomes unstable with brimming energy.','0','2','0','0','24001','living_flare EMOTE_UNSTABLE'),
('-1001215','%s releases its energy, engulfing its surroundings in flames!','0','2','0','0','24002','living_flare EMOTE_BURST'),

('-1001216','The last thing I remember is the ship falling and us getting into the pods. I''ll go see how I can help. Thank you!','0','0','7','0','13408','draenei_survivor SAY_HEAL1'),
('-1001217','Where am I? Who are you? Oh no! What happened to the ship?','0','0','7','0','13409','draenei_survivor SAY_HEAL2'),
('-1001218','$C You saved me! I owe you a debt that I can never repay. I''ll go see if I can help the others.','0','0','7','0','12451','draenei_survivor SAY_HEAL3'),
('-1001219','Ugh... what is this place? Is that all that''s left of the ship over there?','0','0','7','0','13410','draenei_survivor SAY_HEAL4'),
('-1001220','Many thanks to you, $c. I''d best get to the crash site and see how I can help out. Until we meet again.','0','0','7','0','12450','draenei_survivor SAY_HEAL5'),
('-1001221','Huh? What happened? Oh... my head feels like it''s going to explode! I''d best get back to the crash site.','0','0','7','0','12449','draenei_survivor SAY_HEAL6'),
('-1001222','Oh, the pain...','0','0','7','0','13440','draenei_survivor SAY_HELP1'),
('-1001223','Everything hurts. Please, make it stop...','0','0','7','0','13441','draenei_survivor SAY_HELP2'),
('-1001224','Ughhh... I hurt. Can you help me?','0','0','7','0','13411','draenei_survivor SAY_HELP3'),
('-1001225','I don''t know if I can make it. Please help me...','0','0','7','0','13442','draenei_survivor SAY_HELP4'),

('-1001226','Thank you for doing this, $r. I... I think I know how to get back to my father. Please don''t let the bad things hurt me.','0','0','0','1','24746','SAY_MOOTOO_Y_START'),
('-1001227','I thought for sure that I would die like all the others!','0','0','0','1','27244','SAY_1_MOOTOO_Y'),
('-1001228','I can feel my father''s aura. He''s definitely this way... come on, $n.','0','0','0','1','24747','SAY_2_MOOTOO_Y'),
('-1001229','The mist is death... senseless, without compassion.','0','0','0','1','24757','SAY_3_MOOTOO_Y'),
('-1001230','So cold...','0','0','0','1','39048','SAY_4_MOOTOO_Y'),
('-1001231','What was that! Did you see that?','0','0','0','0','27245','SAY_5_MOOTOO_Y'),
('-1001232','Everyone is dead... Everyone and everything...','0','0','0','1','24965','SAY_6_MOOTOO_Y'),
('-1001233','I see a break in the mist up ahead. We''re getting closer!','0','0','0','5','24748','SAY_7_MOOTOO_Y'),
('-1001234','How will we overcome the mist?','0','0','0','1','27243','SAY_8_MOOTOO_Y'),
('-1001235','FATHER! Father, I''m saved!','0','0','0','1','24749','SAY_CREDIT_MOOTOO_Y'),
('-1001236','Daughter! You have returned!','0','0','0','5','24751','SAY_1_ELDER_MOOTOO'),
('-1001237','%s hugs Elder Mootoo.','0','2','0','5','24750','SAY_9_MOOTOO_Y'),
('-1001238','Mootoo thanks you, $n.','0','0','0','1','24753','SAY_2_ELDER_MOOTOO'),

('-1001239','%s is drawn to the ghost magnet...','0','2','0','0','7585','Magrami Spectre on spawn 1'),
('-1001240','%s is angered!','0','2','0','0','7583','Magrami Spectre on spawn 2'),
('-1001241','The Avatar of Terokk has been defeated! Death to Terokk! Death to Skettis!','0','0','7','0','20270','Defender Grashna on Avatar of Terokk death'),

('-1001242','%s cracks his knuckles.','0','2','0','23','1717','larry EMOTE_KNUCKLES'),
('-1001243','Is the way clear? Let''s get out while we can, $N.','0','0','0','0','14724','kayra SAY_START'),
('-1001244','Looks like we won''t get away so easy. Get ready!','0','0','0','0','14725','kayra SAY_AMBUSH1'),
('-1001245','Let''s keep moving. We''re not safe here!','0','0','0','0','14726','kayra SAY_PROGRESS1'),
('-1001246','Look out, $N! Enemies ahead!','0','0','0','0','14727','kayra SAY_AMBUSH2'),
('-1001247','We''re almost to the refuge! Let''s go.','0','0','0','0','14728','kayra SAY_PROGRESS2'),
('-1001248','I can see my fellow druids from here.  Thank you, $n.  I''m sure Ysiel will reward you for your actions!','0','0','0','0','14729','kayra SAY_END'),
('-1001249','You won''t escape us, druid!','0','0','0','0','14731','Umbrafen Slavebinder SAY_AMBUSH2'),
('-1001250','%s is silenced by the venemous sting.','0','2','0','0','9762','npc_simone EMOTE_SILENCE'),
('-1001251','%s is stricken by a virulent poison.','0','2','0','0','9786','npc_artorius EMOTE_POISON'),
('-1001252','%s is immobilized.','0','2','0','0','9785','npc_solenor EMOTE_IMMOBILIZED'),
('-1001253','You dare interfere with this being''s testing? The battle must be fought alone! You shall all pay for this interference!','0','0','0','0','9726','the_cleaner SAY_CLEANER_AGGRO'),
('-1001254','We''re here for you, lost brother.  It is custom to offer you a chance to repent before you are destroyed.  We offer you this chance, as the naaru''s law commands.','0','0','0','0','18389','SAY_ADYEN_1'),
('-1001255','Do not make me laugh.  Is this the mighty Aldor army that''s come to defeat me?','0','1','0','153','18390','SAY_SOCRETHAR_1'),
('-1001256','We may be few, Socrethar, but our faith is strong.  Something you will never understand.  Now that custom has been served, prepare to meet your end.','0','0','0','0','18391','SAY_ADYEN_2'),
('-1001257','Yes, let us settle this.  Before we begin, however, there''s somebody I''d like you to meet.','0','1','0','1','18392','SAY_SOCRETHAR_2'),
('-1001258','How... how could you?!','0','0','0','0','18393','SAY_ORELIS_1'),
('-1001259','My heart has been filled with hate since our sworn enemies were allowed into our city.  I knew the one known as Voren''thal before he was called a Seer.  It was by his hand that my brother was slain.','0','0','0','0','18394','SAY_KAYLAAN_1'),
('-1001260','I turned that hate on the Illidari and the Burning Legion... but they weren''t the ones who betrayed us.  We were the naaru''s chosen!  We lived and died for them!','0','0','0','0','18395','SAY_KAYLAAN_2'),
('-1001261','Once the hatred in my heart became focused, everything became clear to me.  Shattrath must be destroyed and the naaru with it.','0','0','0','0','18396','SAY_KAYLAAN_3'),
('-1001262','Socrethar is clouding your mind, Kaylaan!  You do not mean these words!  I remember training you when you were but a youngling.  Your will was strong even then!','0','0','0','0','18397','SAY_ADYEN_3'),
('-1001263','You are wrong, Adyen.  My mind has never been clearer.','0','0','0','0','18398','SAY_KAYLAAN_4'),
('-1001264','Slay these dogs, Kaylaan!  Earn your place in the Burning Legion!','0','1','0','25','18407','SAY_SOCRETHAR_3'),
('-1001265','What are you waiting for?  Finish them, young one.  Let your hatred burn!','0','1','0','53','18408','SAY_SOCRETHAR_4'),
('-1001266','Yes... master.','0','0','0','0','18409','SAY_KAYLAAN_5'),
('-1001267','Teacher...','0','0','0','0','18399','SAY_KAYLAAN_6'),
('-1001268','The Light wants its lost son back, Socrethar.','0','0','0','0','18400','SAY_ISHANAH_1'),
('-1001269','I offer myself in exchange.  You will have Ishanah, the High Priestess of the Aldor, as your prisoner if you release Kaylaan from your dark grasp.','0','0','0','0','18401','SAY_ISHANAH_2'),
('-1001270','You foolish old hag... Why would I do that when I can have you both?','0','1','0','0','18402','SAY_SOCRETHAR_5'),
('-1001271','No!  What have I done?','0','1','0','0','18404','SAY_KAYLAAN_7'),
('-1001272','Light grant me strength!','0','1','0','0','18405','SAY_KAYLAAN_8'),
('-1001273','Oh, please!  This is sickening; I''m going to have to kill you all myself.','0','1','0','0','18406','SAY_SOCRETHAR_6'),
('-1001274','Now you''re gonna get it good, $n!','0','0','0','0','1961','dashel stonefist SAY_STONEFIST_1'),
('-1001275','Okay, okay! Enough fighting. No one else needs to get hurt.','0','0','0','0','1712','dashel stonefist SAY_STONEFIST_2'),
('-1001276','It''s okay, boys. Back off. You''ve done enough. I''ll meet up with you later.','0','0','0','0','1713','dashel stonefist SAY_STONEFIST_3'),
('-1001277','That could have gone better, $n.  Come, I have something for you.','0','4','0','0','19200','Ambassador Sunsorrow - During Lament of the Highborne script'),
('-1001278','Who calls me to this world?  The stars are not yet aligned... my powers fail me!  You will pay for this!','0','0','0','1','21639','Terokk SAY_SPAWN'),
('-1001279','Show me what you''re made of, $n!','0','6','0','1','21327','Terokk SAY_CHOSEN_ONE'),
('-1001280','Kwa! You cannot kill me, I am immortal!','0','6','0','1','24020','Terokk SAY_DIVINE_SHIELD'),
('-1001281','%s becomes enraged as his shield shatters.','0','3','0','1','21328','Terokk SAY_ENRAGE'),
('-1001282','Enemy sighted!  Fall into formation and prepare for bombing maneuvers!','0','6','0','1','21439','Skyguard Ace SAY_SPAWN'),
('-1001283','Quickly! Use the flames and support the ground troops. Its ancient magic should cleanse Terokk''s shield.','0','6','0','1','24021','Skyguard Ace SAY_FLAMES'),
('-1001284','They did it!  Enemy down!  Return to base!','0','6','0','1','21437','Skyguard Ace SAY_TEROKK_DOWN'),
('-1001285','Work is da poop! NO MORE!','0','0','0','15','21325','Dosobedient Dragonmaw Peon SAY_IDLE1'),
('-1001286','Me no work no more!','0','0','0','15','21324','Dosobedient Dragonmaw Peon SAY_IDLE2'),
('-1001287','HEY! No more booterang! Me sorry! Me work!','0','0','0','34','21332','Dosobedient Dragonmaw Peon SAY_BOOTERANG1'),
('-1001288','ARGH! BOOTERANG!','0','0','0','34','21335','Dosobedient Dragonmaw Peon SAY_BOOTERANG2'),
('-1001289','OWWWW! Ok, ok, me go back to work!','0','0','0','34','21331','Dosobedient Dragonmaw Peon SAY_BOOTERANG3'),
('-1001290','WHY IT PUT DA BOOTERANG ON DA SKIN?? WHY??','0','0','0','34','21334','Dosobedient Dragonmaw Peon SAY_BOOTERANG4'),
('-1001291','AYAYA! One day me have dat booterang...','0','0','0','34','21336','Dosobedient Dragonmaw Peone SAY_BOOTERANG5'),
('-1001292','OOF! Booterang hurted me! Me tink work better den booterang!','0','0','0','34','21333','Dosobedient Dragonmaw Peon SAY_BOOTERANG6'),
('-1001293','The Dragonmaw must be stopped...','0','1','0','22','21685','Commander Arcus SAY_EVENT_ACCEPT'),
('-1001294','Stand tall, soldiers. Show them no quarter!','0','0','0','5','21633','Commander Arcus SAY_EVENT_START'),
('-1001295','Victory to the Aldor! The Dragonmaw have been defeated!','0','1','0','0','21728','Commander Arcus SAY_EVENT_END'),
('-1001296','Defenders, show these mongrels the fury of a Scryer!','0','1','0','22','21632','Commander Hobb SAY_EVENT_ACCEPT'),
('-1001297','Stand tall, soldiers. Show them no quarter!','0','0','0','1','21633','Commander Hobb SAY_EVENT_START'),
('-1001298','Victory to the Scryers! The Dragonmaw have been defeated!','0','1','0','0','21663','Commander Hobb SAY_EVENT_END'),
('-1001299','I may be old but I can still take on a young whippersnapper like you, $n. Try not to fall behind...','0','0','0','0','21362','Murg \'Oldie\' Muckjaw SAY_MUCKJAW_START'),
('-1001300','Well, you won... I guess.','0','0','0','0','21363','Murg \'Oldie\' Muckjaw SAY_MUCKJAW_END'),
('-1001301','Trope will show you how to fly like a Dragonmaw... You will show Trope how to die like a scrub.','0','0','0','1','21418','Trope the Filth-Belcher SAY_TROPE_START'),
('-1001302','You did well. Certainly a surprise to Trope... Report back to Ja''y.','0','0','0','0','21419','Trope the Filth-Belcher SAY_TROPE_END'),
('-1001303','Let''s get this over with...','0','0','0','1','21421','Corlok the Vet SAY_START_CORLOK'),
('-1001304','You put up a hell of a fight, newbie. Hell of a fight...','0','0','0','1','21422','Corlok the Vet SAY_END_CORLOK'),
('-1001305','I''m taking this back to the old school. I''ll be the Alliance and you be Frostwolf Village. BOMBS AWAY!','0','0','0','1','21423','Wing Commander Ichman SAY_START_ICHMAN'),
('-1001306','Thank you for that... It was humbling to be served in such a manner.','0','0','0','1','21424','Wing Commander Ichman SAY_END_ICHMAN'),
('-1001307','You''re in for a rough ride, $n. I hope you''ve already made funeral arrangements.','0','0','0','1','21425','Wing Commander Mulverick SAY_START_MULVERICK'),
('-1001308','You''re the best I''ve ever seen. I can''t believe I''m saying this but you might have a chance against Skyshatter. And hey, if that doesn''t go so well you can be my wing man...','0','0','0','1','21426','Wing Commander Mulverick SAY_END_MULVERICK'),
('-1001309','Prepare a funeral pyre! $n has challenged Skyshatter!','0','1','0','1','21430','Captain Skyshatter - SAY_START_SKYSHATTER'),
('-1001310','I weep for you, $n. You really have no idea what you''ve gotten yourself into...','0','0','0','1','21432','Captain Skyshatter - SAY_MID_SKYSHATTER'),
('-1001311','I... I am undone... The new top orc is $n!','0','1','0','1','21431','Captain Skyshatter - SAY_END_SKYSHATTER'),
-- warning ids pending export
('-1001312','WE STRIKE!','0','1','0','15','24562','proudhoof SAY_QUEST_START'),
('-1001313','$N is going to join us on our assault. Let us bring peace to my ancestors!','0','0','0','396','24561','proudhoof SAY_QUEST_INTRO'),
('-1001314','An ambush. Return them to their rest!','0','0','0','5','24563','proudhoof SAY_AMBUSH'),
('-1001315','I will not stop until this place is once again at peace.','0','0','0','0','24611','proudhoof SAY_AGGRO'),
('-1001316','Regain your strength. The place where Steeljaw was felled is just ahead.','0','0','0','396','24564','proudhoof SAY_AMBUSH_COMPLETE'),
('-1001317','There''s the dog''s banner and there''s his corpse. What''s that? He''s upon us!','0','0','0','25','24565','proudhoof SAY_FINAL_BATTLE'),
('-1001318','You fought well. Now go north to your orc outpost and inform Overlord Bor''gorok of our success!','0','0','0','113','24566','proudhoof SAY_QUEST_END'),

-- -1 010 000 Classic texts
('-1010000','The beast returns from whence it came. The wrath of Neptulon has subsided.','0','3','0','0','11160','Maws EMOTE_MAWS_KILL'),

('-1010001','Beware, $N! Look to the west!','0','0','0','0','4926','regthar SAY_START_REGTHAR'),
('-1010002','A defender has fallen!','0','0','0','0','4757','regthar SAY_DEFENDER'),
('-1010003','The Kolkar invaders are retreating!','0','1','0','0','8897','regthar YELL_RETREAT'),
('-1010004','%s is ripped apart as it is banished from this world.','0','3','0','0','853','Cresting Exile - on Spell 4131'),
('-1010005','Who would be foolish enough to enter the tower of the Grand Foreman?','0','0','0','0','3322','gallywix stealth alert'),
('-1010006','*cough* who goes there? *cough*','0','0','0','0','3321','Silixiz stealth alert'),
('-1010007','Did you hear that?','0','0','0','0','3331','Venture Co. Lookout stealth alert'),
('-1010008','I smell rogue.','0','0','0','0','3332','Venture Co. Lookout stealth alert'),
('-1010009','Wow, it''s 3 degrees Kraklenheit. Keep looking.','0','0','0','0','6011','Krakles Thermometer'),
('-1010010','The temperature is 12 degrees Kraklenheit. How exciting.','0','0','0','0','5767','Krakles Thermometer'),
('-1010011','It''s 40 degrees Kraklenheit. Can you feel the heat? No? I really couldn''t either.','0','0','0','0','6010','Krakles Thermometer'),
('-1010012','*sigh* ...113 degrees Kraklenheit. Can''t you do any better?','0','0','0','0','6412','Krakles Thermometer'),
('-1010013','The temperature is 122 degrees Kraklenheit.','0','0','0','0','5766','Krakles Thermometer'),
('-1010014','The temperature is 564 degrees Kraklenheit.','0','0','0','0','5765','Krakles Thermometer'),
('-1010015','You''re getting warmer... 7039 degrees Kraklenheit!','0','0','0','0','6413','Krakles Thermometer'),
('-1010016','8536 degrees Kraklenheit is the reading... Tired of me yet?','0','0','0','0','6414','Krakles Thermometer'),
('-1010017','The temperature is 9280 degrees Kraklenheit! That''s HOT!','0','0','0','0','5764','Krakles Thermometer'),
('-1010018','DING! 428,000 degrees Kraklenheit, exactly! Well, approximately. Almost. Somewhere around there...','0','0','0','0','6007','Krakles Thermometer'),
('-1010019','It''s 428,000 degrees Kraklenheit... What''s happening, hot stuff!?','0','0','0','0','6006','Krakles Thermometer'),
('-1010020','Measuring by Kraklenheit, it is 428,000 degrees! That''s Krakley!','0','0','0','0','6008','Krakles Thermometer'),
('-1010021','Holy Krakle! The temperature is 428,000 degrees Kraklenheit!','0','0','0','0','5763','Krakles Thermometer'),

('-1010022','Thank you for freeing me.  Speak with my sister in Splintertree.  I... I must find a place to rest...','0','0','0','0','8265','Ruul Snowhoof'),

('-1010023','I''m ready when you are, $N. Lead the way!','0','0','0','0','4079','Kindal Moonweaver SAY_QUEST_START'),
('-1010024','Die, $n. Elune will not have you harm her creatures any longer!','0','0','0','0','4123','Kindal Moonweaver SAY_INITIAL_AGGRO'),
('-1010025','You dare touch me?!','0','0','0','0','4125','Kindal Moonweaver SAY_AGGRO'),
('-1010026','We''ve done it! Meet me back by Jer''kai above the Grim Totem camp.','0','0','0','0','4080','Kindal Moonweaver SAY_QUEST_COMPLETE'),

('-1010027','%s is lured out by the sweet scent of the purified food.','0','2','0','0','5617','Blackwood Furbolg - on event 3938'),
('-1010028','%s begins to greedily devour the food.','0','2','0','0','5618','Blackwood Furbolg - on event 3938'),

('-1010029','Help! We''re being attacked! Dark Irons!','0','1','0','0','494','SAY_MIRAN_AMBUSH'),

('-1010030','Bye!  Better luck next time!','0','1','0','0','7553','Hornizz to Melizza 1'),
('-1010031','Always on the go, that girl.','0','1','0','0','7554','Hornizz to Melizza 2'),
('-1010032','%s disappears back into the swamp.','0','2','0','0','1619','Stinky end emote');

-- -1 015 000 TBC texts
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1015001','%s lets loose a low, gultural growl.','0','2','0','0','20100','Enraged Netherwing Drake - EMOTE on script spell hit'),
('-1015002','You will never get the Heart of Fury!  Its power belongs to Illidan!','0','1','0','0','19744','Shadowlord Deathwail - yell on descent'),
('-1015003','%s retrieves the Heart of Fury.','0','2','0','0','19830','Shadowlord Deathwail - grabs heart'),
('-1015004','Master... I''ve failed you...','0','1','0','0','19820','Shadowlord Deathwail - death'),
('-1015005','The power of Light compels you! Back to your pit!','0','0','0','0','20405','anchorite barada SAY_EXORCISM_RANDOM_6'),
('-1015006','All is lost, Anchorite! Abandon what hope remains.','0','0','0','0','20418','colonel jules SAY_EXORCISM_RANDOM_2'),
('-1015007','The Light is my guide... it is my sustenance!','0','0','0','0','20408','anchorite barada SAY_EXORCISM_RANDOM_7'),
('-1015008','Ah! Cease the incantations, Anchorite! Cease, or I will show you such pain that your pathetic people have never imagined!','0','0','0','0','20414','colonel jules SAY_EXORCISM_RANDOM_3'),
('-1015009','You... will... leave... this... man!','0','0','0','0','20410','anchorite barada SAY_EXORCISM_RANDOM_2'),
('-1015010','In the name of the Light! It is Light that commands you! It is Light that flung you to the depths of darkness!','0','0','0','0','20406','anchorite barada SAY_EXORCISM_RANDOM_4'),
('-1015011','You will not succeed, mortal! This shell will lie decrepit, blistered and bleeding before I am done with it. And its spirit will be long cast into darkness.','0','0','0','0','20413','colonel jules SAY_EXORCISM_RANDOM_4'),
('-1015012','From the darkest night shall rise again the raven, shall take flight in the shadows, shall reveal the nature of its kind. Prepare yourself for its coming, for the faithful shall be elevated to take flight with the raven, the rest be forgotten to walk upon the ground, clipped wings and shame.','0','4','0','0','19475','First Prophecy for Whispers of the Raven God'),
('-1015013','Steel your minds and guard your thoughts. The dark wings will cloud and consume the minds of the weak, a flock of thralls whose feet may never leave the ground.','0','4','0','0','19476','Second Prophecy for Whispers of the Raven God'),
('-1015014','The old blood will flow once again with the coming of the raven, the return of the darkness in the skies. Scarlet night, and the rise of the old.','0','4','0','0','19477','Third Prophecy for Whispers of the Raven God'),
('-1015015','The raven was struck down once for flying too high, unready. The eons have prepared the Dark Watcher for its ascent, to draw the dark cloak across the horizon.','0','4','0','0','19478','Fourth Prophecy for Whispers of the Raven God'),
('-1015016','Argh! I will shred your soul and grind it to dust!','0','1','0','0','20726','Skulloc Soulgrinder on last unbanishing phase.'),
('-1015017','Weak, little bugs!  Your souls aren''t worth eating...','0','1','0','0','20727','Skulloc Soulgrinder on last unbanishing phase.'),
('-1015018','%s brushes up against $n and farts like only an ogre can.','0','2','0','0','20926','Generic Blades Edge Ogre /fart emote response'),
('-1015019','Me like to fart, too!','0','0','0','1','20927','Generic Blades Edge Ogre /fart emote response'),
('-1015020','The %s cowers at the sight of his angered $g king : queen;.','0','2','0','0','20925','Generic Blades Edge Ogre /angry emote response'),
('-1015021','Shh... quiet, it da new $g king : queen;.','0','0','0','0','20964','Bloodmaul/Bladespire Supplicant LoS text'),
('-1015022','$G King : Queen; $n.','0','0','0','0','20960','Bloodmaul/Bladespire Supplicant LoS text'),
('-1015023','All hail, $n, the new King of the ogres!!!','0','6','0','22','20791','Mog''dorg the Wizened on quest end Into the Soulgrinder'),
('-1015024','Aw, me not think it work out between us.','0','0','0','1','20928','Generic Blades Edge Ogre /kiss emote response'),
('-1015025','What me do wrong, my $g king : queen;?','0','0','0','16','20929','Generic Blades Edge Ogre /point emote response'),
('-1015026','$G Him : Her; so tiny!  How $g him : her; rule?','0','0','0','0','20965','Bloodmaul/Bladespire Supplicant LoS text'),
('-1015027','$G Him : Her; look too pretty to be da ruler.','0','0','0','0','20962','Bloodmaul/Bladespire Supplicant LoS text'),
('-1015028','Me live to serve.','0','0','0','0','20966','Bloodmaul/Bladespire Supplicant LoS text'),
('-1015029','For the Deceiver! For Kil''jaeden!','0','0','0','0','20448','Trelopades on aggro 1'),
('-1015030','Only flesh and blood...','0','0','0','0','20447','Trelopades on aggro 2'),
('-1015031','Onward, Scryers!  Show Illidan''s lackeys the temper of our steel!','0','1','0','0','20696','Onward Scryers'),
('-1015032','Forward, vindicators!  Drive these demons from our sacred temple!','0','1','0','0','20761','Forward vindicators'),
('-1015033','You heard the fleshling! MOVE OUT!','0','0','0','1','18642','Captain Saeed SAY_SAEED_START'),
('-1015034','Tell me when you are ready, $N. We will attack on your command.','0','0','0','0','18643','Captain Saeed SAY_SAEED_PREPARE'),
('-1015035','It''s now or never, soldiers! Let''s do this! For K''aresh! For the Protectorate!','0','0','0','0','18644','Captain Saeed SAY_SAEED_ATTACK'),
('-1015036','%s laughs.','0','2','0','0','10315','Dimensius EMOTE_DIMENSIUS_LAUGH'),
('-1015037','The time for your destruction has finally come, Dimensius!','0','0','0','0','18645','Captain Saeed SAY_SAEED_TAUNT'),
('-1015038','We are victorious! Dimensius is no more! Report back to the Professor, hero.','0','0','0','0','18649','Protectorate Avenger SAY_AVENGER_QUEST_COMPLETE'),
('-1015039','Rangers, listen up! We stand ready to take the pain directly to the heathen Bash''ir devils.  Our mission is simple. Hold the area long enough to get accurate readings on their new technology. ','0','0','0','5','21619','Bashir Intro - Lieutenant 1'),
('-1015040','But don''t underestimate our enemies.  We are invading them.  We are taking the fight to them.  We have them backed into a corner and they will strike back.','0','0','0','5','21620','Bashir Intro - Lieutenant 2'),
('-1015041','We are Skyguard!  This is what we are trained to do.  We do not retreat, we do not surrender.  We will see our mission through even if it means death! We will secure that information or we will die on the field of battle.','0','0','0','0','21621','Bashir Intro - Lieutenant 3'),
('-1015042','Anyone have any questions?','0','0','0','6','21622','Bashir Intro - Lieutenant 4'),
('-1015043','Look man, I only need to know one thing, where they are.','0','0','0','1','21625','Bashir Intro - Ranger 1'),
('-1015044','You are about to find out!','0','0','0','0','21623','Bashir Intro - Lieutenant 5'),
('-1015045','Skyguard Rangers, MOUNT UP!!','0','0','0','0','21624','Bashir Intro - Lieutenant 6'),
('-1015046','A discovery!  I need aid with these measurements... I''ll send a signal to my colleague!','0','0','0','0','21792','Bashir - Tech Phase 1 end'),
('-1015047','This forge can make unstable concoctions... to study them, I need Apexis Shards!','0','0','0','0','21795','Bashir - Assistant arrived'),
('-1015048','The disruptor towers over charged the crystalforge, and uncovered more secrets!  Hold them!  Hold them while we gather new data!','0','0','0','0','21793','Bashir - Tech Phase 2 end'),
('-1015049','I am growing amazing crystals from these waters.  For catalysts, I must have more Apexis shards!','0','0','0','0','21796','Bashir - Adept arrived'),
('-1015050','They called me down from K''aresh to deal with this band of misfits?  Unbelievable!  Prepare to be collected!','0','0','0','0','21881','Bashir - Collector spawned'),
('-1015051','Enough!  I will take care of these fools myself!','0','0','0','0','21887','Bashir - Collector attacks'),
('-1015052','At last!  The information we seek!  Gather quickly, skyguard.  We must soon return to headquarters with our findings!','0','0','0','0','21794','Bashir - Tech Phase 3 end'),
('-1015053','Its properties are amazing!  Provide me with Apexis crystals, and I''ll form weapons and armor within crystal geodes.','0','0','0','0','21797','Bashir - Master arrived'),
('-1015054','Our studies are complete!  Well fought!  Headquarters will pour over our gained measurements and, with luck, uncover the Bash''ir''s intentions.  Now, to the skies!','0','0','0','0','21276','Bashir - Tech Event end'),
('-1015055','LET''S ROCK!','0','0','0','0','21653','Bashir Ranger - On Aggro 1'),
('-1015056','Heads up!','0','0','0','0','32398','Bashir Ranger - On Aggro 2'),
('-1015057','Let''s go!','0','0','0','0','21649','Bashir Ranger - On Aggro 3'),
('-1015058','Who''s next?','0','0','0','0','21654','Bashir Ranger - On Aggro 4'),
('-1015059','Come on!','0','0','0','0','21650','Bashir Ranger - On Aggro 5'),
('-1015060','Oh, you want some of this?','0','0','0','0','21652','Bashir Ranger - On Aggro 6'),
('-1015061','Come on!','0','0','0','0','21650','Bashir Ranger - On Aggro 7'),
('-1015062','The enemy is upon us!  Stand your ground!','0','0','0','0','20665','Vindicator on aggro 1'),
('-1015063','Illidan''s lapdogs!  Destroy them all!','0','0','0','0','20669','Magister on aggro 1'),
('-1015064','Come closer, demon!  Death awaits!','0','0','0','0','20670','Magister on aggro 2'),
('-1015065','I''ve a message for your master, scum!','0','0','0','0','20672','Magister on aggro 3'),

('-1015066','Do not return, draenei scum. Next time we won''t spare your life, unarmed or not!','0','0','0','0','13986','Maghar escort SAY'),
('-1015067','I''ve failed... peace is impossible.','0','0','0','0','13982','Vindicator Sedai SAY 1'),
('-1015068','What in the Light''s name...?','0','0','0','0','13983','Vindicator Sedai SAY 2'),
('-1015069','Fel orcs!','0','0','0','0','13984','Vindicator Sedai SAY 3'),
('-1015070','The cycle of bloodshed is unending... Is there nothing I can do?','0','0','0','0','13985','Vindicator Sedai SAY 4'),
('-1015071','You can die!','0','0','0','0','13988','Krun Spinebreaker SAY 1'),
('-1015072','%s plants the Flag of Ownership in the corpse of $N.','0','0','0','0','28008','Item - Flag of Ownership'),

('-1015073','Azeroth has cowered too long under our shadow!  Now, feel the power of the Burning Crusade and despair!','0','1','0','0','16046','kruul SAY_INTRO1'),
('-1015074','Your fate is sealed, Azeroth!  I will find the Aspect Shards, and then you will not stand against our might!','0','1','0','0','16047','kruul SAY_INTRO2'),
('-1015075','Cower, little worms!  Your feeble heroes are nothing!  Your saviors will be our first feast!','0','1','0','0','16045','kruul SAY_AGGRO1'),
('-1015076','Where?  Where are the Shards!  You cannot hide them from us!','0','1','0','0','17097','kruul SAY_AGGRO2'),
('-1015077','Your world will die, mortals!  Your doom is now at hand!','0','1','0','0','16044','kruul SAY_AGGRO3'),
('-1015078','Your own strength feeds me, $n!','0','1','0','0','8461','kruul SAY_KILL'),
('-1015079','Hah!  This place is not yet worthy of my infliction!','0','1','0','0','16048','kruul SAY_DESPAWN'),

('-1015080','Marshals... brothers and sisters of the sword.  I''d rather fight alongside you than any army of thousands.  We are lions!  You know what''s waiting through these demons and beyond that Dark Portal?  Immortality!  Take it!','0','1','0','0','16545','lord marshal raynor LMR_SPAWN'),
('-1015081','Yes. Fight and you may die. Run and you will live, but for how long?  The devils are upon us!  And those not here, dying in their villages weeks from now, would they be willing to trade all the days from this day to that, for one chance to come here and stop these horrors from invading our world?','0','0','0','0','16548','lord marshal raynor LMR_RANDOM_TEXT_1'),
('-1015082','Everyone fights.  No one quits.','0','0','0','0','16549','lord marshal raynor LMR_RANDOM_TEXT_2'),
('-1015083','Hold your ground! Hold your ground! Sons of the Alliance!  Of the Horde! My brothers and sisters!  I see in your eyes the same fear that would wither my own heart. A day may come when our courage fails, when we forsake our friends and break our fellowships... but it is not this day.','0','0','0','0','16550','lord marshal raynor LMR_RANDOM_TEXT_3'),
('-1015084','This day we fight!! By all that you hold dear in these good lands, I bid you stand, heroes of Azeroth!','0','0','0','0','16551','lord marshal raynor LMR_RANDOM_TEXT_4'),
('-1015085','We few, we happy few, we band of brothers and sisters.  For those today that shed their blood with me, Shall be my kin; be they never so vile, This day shall gentle their conditions.','0','0','0','0','16552','lord marshal raynor LMR_RANDOM_TEXT_5'),
('-1015086','Argent Dawn! We go to gather more aid!  Fight on, brothers and sisters.  And know that you will not long fight alone!','0','1','0','0','16934','lord marshal raynor LMR_DESPAWN'),

('-1015087','Soldiers of Azeroth, you now fight against the vile Burning Crusade.  And the hopes and prayers of all Light-loving people march with you.  In the company of our brave allies and brothers-in-arms you will hold your ground and break this demonic charge.  You will stem the destructive tyranny that rages across so many worlds.','0','1','0','0','16625','justinius the harbinger JTH_SPAWN'),
('-1015088','With each passing moment, I see the people of Azeroth prove again the reasons we have come.  It is with solemn pride that I fight with you, and if I die this day, then I die among friends.','0','0','0','0','16633','justinius the harbinger JTH_RANDOM_TEXT_1'),
('-1015089','The demons should be in no doubt that we are their nemesis and, though they wish our destruction, it is we who will bring about their ruin.  Kazzak and his forces will be diminished and defeated by this coalition.  And to do this, we must throw at them our bodies and hearts, for they seek no less than the obliteration of this world.','0','0','0','0','16627','justinius the harbinger JTH_RANDOM_TEXT_2'),
('-1015090','Yes!  Hold your feet firm!  Lend aid to your cohorts and together our banded strength will overcome these demons!  Cling fiercely to hope and grip tightly your weapons, and trust that the Light will pierce this darkness and shine through to morrow''s dawn!','0','0','0','0','16631','justinius the harbinger JTH_RANDOM_TEXT_4'),
('-1015091','Fight on, good people of Azeroth.  And take heart!  Your brethren rush to this hellish portal and will soon throw themselves at these demons as you have so bravely done.  May the Light keep you!','0','1','0','0','16933','justinius the harbinger JTH_DESPAWN_1'),

('-1015092','Hunters, to me! Let us show these demons what it is to fight and die in Azeroth!','0','1','0','0','16616','melgromm highmountain MH_SPAWN'),
('-1015093','The spirits sing to us!  They sing the song of battle!','0','0','0','0','16617','melgromm highmountain MH_RANDOM_1'),
('-1015094','Strike true, brothers and sisters!  Today our quarry is demons!','0','0','0','0','16618','melgromm highmountain MH_RANDOM_2'),
('-1015095','Once again, we will show this Burning Legion that our world is not for the taking!','0','0','0','0','16620','melgromm highmountain MH_RANDOM_3'),
('-1015096','These fiends will fail!  For Azeroth!','0','0','0','0','16621','melgromm highmountain MH_RANDOM_4'),
('-1015097','If it is our fate to die this day, then we will not go easy!  Mark me, these demons will fall like wheat!','0','0','0','0','16622','melgromm highmountain MH_RANDOM_5'),
('-1015098','Though your sinews tire and your blood flows from cuts and wounds to this sodden ground, you fight for the spirits of your world.  No evil force can stand against us!','0','0','0','0','16624','melgromm highmountain MH_RANDOM_6'),
('-1015099','My brethren, my comrades-in-arms!  I must go to seek the forces of the Horde and direct them here!  Keep your spirits strong, brave hunters!  Do not fall to the portal-spawned fiends!','0','0','0','0','16935','melgromm highmountain MH_DESPAWN'),

('-1015100','Mooooo...','0','0','0','0','18179','bessy SAY_ESCORT_1'),
('-1015101','Moooooooooo!','0','0','0','0','18180','bessy SAY_ESCORT_2'),
('-1015102','Bessy, you''re home. Thank the Light!','0','0','0','0','18181','bessy SAY_THADELL'),
('-1015103','Quickly, get to the middle of the platform! Illidan''s fury is soon to be unleashed!','0','4','0','0','19796','Marcus Auralion - On Accept'),
('-1015104','[Demonic] Ka kalix!','0','0','0','0','15551','Sal''salabim - On Gossip Select'),

-- -1015105 - -1015123 RESERVED for Shartuul's Transporter event

('-1015124','Prepare yourselves, the bells have tolled! Shelter your weak, your young and your old! Each of you shall pay the final sum. Cry for mercy, the reckoning has come!','11966','1','0','0','22022','SAY_SHADE_SPAWN'),
('-1015125','The sky is dark.  The fire burns.  You strive in vain as Fate''s wheel turns.','12570','1','0','0','22034','SAY_SHADE_FAILING_1'),
('-1015126','The town still burns, a cleansing fire!  Time is short, I''ll soon retire!','12571','1','0','0','22035','SAY_SHADE_FAILING_2'),
('-1015127','My flames have died, left not a spark. I shall send you myself, to the lifeless dark.','11968','1','0','0','22027','SAY_SHADE_FLAMES_DIED'),
('-1015128','Harken, cur!  Tis you I spurn!  Now feel... the burn!','0','2','0','0','22587','EMOTE_SHADE_CONFLAGRATION'),
('-1015129','Harken, cur!  Tis you I spurn!  Now feel... the burn!','0','0','0','0','22587','SAY_SHADE_CONFLAGRATION'),
('-1015130','So eager you are, for my blood to spill. Yet to vanquish me, ''tis my head you must kill!','11969','1','0','0','22757','SAY_SHADE_DEATH'),
('-1015131','Fire consumes! You''ve tried and failed.  Let there be no doubt, justice prevailed!','11967','1','0','0','22037','SAY_SHADE_FAILED'),

('-1015132','What''s happening?','0','0','0','0','22177','SAY_ORPHAN_SHADE_1'),
('-1015133','I want to go back to the orphanage!','0','0','0','0','22344','SAY_ORPHAN_SHADE_2'),
('-1015134','Matron!  Is that bad man going to eat us?','0','0','0','0','22345','SAY_ORPHAN_SHADE_3'),
('-1015135','If my dad were alive, he would put out the fires!','0','0','0','0','22346','SAY_ORPHAN_SHADE_4'),
('-1015136','If my mom were alive, she would put out the fires!','0','0','0','0','22347','SAY_ORPHAN_SHADE_5'),

('-1015137','Well done!  We''ll be ready when the real fires begin!','0','0','0','0','23342','SAY_GUARD_RELIGHT_FIRE'),

('-1015138','By the Light!','0','0','0','1','20469','Dark portal 1 say Ally'),
('-1015139','Dornaa stands and gapes in awe at the immense Dark Portal.','0','2','0','0','20510','Dark portal 2 emote Ally'),
('-1015140','For our children and the future of the Alliance! The Legion shall not prevail!','0','0','0','66','21064','Dark portal 3 soldier salute Ally'),

('-1015141','Whoa, no way!','0','0','0','1','20466','Dark portal 1 say Horde'),
('-1015142','Salandria stands and gapes in awe at the immense Dark Portal.','0','2','0','0','20467','Dark portal 2 emote Horde'),
('-1015143','For the younglings! For the Horde!','0','0','0','66','21063','Dark portal 3 soldier salute Horde'),

('-1015144','Dornaa begins to examine the stone edifice, and then touches it.','0','2','0','0','20458','Auchindoun 2 emote'),
('-1015145','Is this where all of the adventurers come from?  Help me.','0','4','0','0','20464','Auchindoun 3 say'),
('-1015146','Thank you for the summon, little one. My group left for the dungeon without me!','0','0','0','2','20987','Auchindoun 4 say Adventurer'),
('-1015147','The next person who asks me for a potion is getting a knuckle sandwich. I quit alchemy!','0','0','0','1','20983','Auchindoun 5 say Adventurer'),
('-1015148','That was neat!','0','4','0','1','20990','Auchindoun 6 say'),

('-1015149','Jheel!!!','0','0','0','1','20475','Aeris Landing 1 say'),
('-1015150','Dornaa!!!','0','0','0','0','20479','Aeris Landing 2 say Jheel'),
('-1015151','Dornaa gives her brother a big hug, and the two children begin to talk.','0','2','0','0','20476','Aeris Landing 3 emote'),
('-1015152','See you soon, big brother!','0','0','0','0','20484','Aeris Landing 4 say'),
('-1015153','Back to your studies, boy!','0','0','0','1','20482','Aeris Landing 5 say teacher'),
('-1015154','Yes, sir.','0','0','0','0','20483','Aeris Landing 6 say Jheel'),
('-1015155','I hope that Jheel will be able to come back to the orphanage soon.','0','4','0','1','20477','Aeris Landing 7 say'),

('-1015156','Dornaa walks slowly and turns in reverence as a blessing is bestowed upon her by O''ros.','0','2','0','0','20487','Seat of the Naaru 1 emote'),
('-1015157','O''ros says that he wants to talk with you.','0','4','0','1','20691','Seat of the Naaru 2 say'),
('-1015158','Tee hee, your mind-talk tickles.  Thank you, O''ros!  I promise to be good!!','0','0','0','1','20489','Seat of the Naaru 3 say'),

('-1015159','You''re the man from my dream!','0','0','0','1','20543','Farseer 1 say'),
('-1015160','Indeed I am, child.  And do you remember what you showed me in those dreams?','0','0','0','0','20545','Farseer 2 say Nobundo'),
('-1015161','You mean, I can do it here, too?','0','0','0','1','20547','Farseer 3 say'),
('-1015162','Try.','0','0','0','0','20548','Farseer 4 say Nobundo'),
('-1015163','Concentrating a moment, Dornaa summons forth the elemental forces.','0','2','0','0','20553','Farseer 5 emote'),
('-1015164','I did it, I did it!!!','0','0','0','1','20550','Farseer 6 emote'),
('-1015165','Amazing, Dornaa.  Truly amazing!','0','0','0','0','20549','Farseer 7 say Nobundo'),

('-1015166','Dornaa''s jaw nearly hits the ground at the sight of Zaladormu.','0','2','0','0','20514','COT 1 emote Ally'),
('-1015167','Wow, he''s a big one!  Um... what''s going on?  Save me!','0','4','0','1','20528','COT 2 say Ally'),
('-1015168','Scary!','0','4','0','1','20976','COT 3 say Ally'),
('-1015169','WAIT!','0','1','0','0','20520','COT 4 say Zaladormu'),
('-1015170','This girl has done nothing, and will not be held accountable for what she might do, or fail to do, in the future.','0','1','0','0','20521','COT 5 say Zaladormu'),
('-1015171','Go in peace, child.','0','1','0','0','20522','COT 6 say Zaladormu'),

('-1015172','Salandria''s jaw nearly hits the ground at the sight of Zaladormu.','0','2','0','0','20527','COT 1 emote Horde'),
('-1015173','Okay, I like!  Um... what''s going on?  Save me!','0','4','0','0','20515','COT 2 whisper Horde'),
('-1015174','Scary!','0','4','0','1','20977','COT 3 say Horde'),

('-1015175','Hch''uu!!!','0','0','0','1','20457','Sporeggar 1 say'),
('-1015176','Salandria?  Salandria!!!','0','0','0','0','20481','Sporeggar 2 say Hchuu'),
('-1015177','Salandria and Hch''uu begin to catch up on all that has happened to them this past year.','0','2','0','0','20478','Sporeggar 3 emote'),
('-1015178','Thank you for bringing me here to see my friend, Hch''uu!','0','4','0','1','20461','Sporeggar 4 whisper'),

('-1015179','Hah, it''s true!  I''m gonna go have a closer look at the fire one!','0','4','0','0','20485','Throne 1 whisper'),
('-1015180','Ow!  Hot!!','0','0','0','1','20486','Throne 2 say'),
('-1015181','Heh, I got a little too close.  Don''t worry, I''m fine.','0','4','0','0','20690','Throne 3 whisper'),

('-1015182','Hello sir, I''m ready to rock!','0','0','0','0','25608','Silvermoon LVL70ETC 1 say'),
('-1015183','Salandria waves her hands in the air.','0','2','0','0','25612','Silvermoon LVL70ETC 2 emote'),
('-1015184','Hey guys, I''m your biggest fan!  I''m ready to rock!','0','0','0','0','25609','Silvermoon LVL70ETC 3 say'),
('-1015185','Check this out guys!  I can rock like you guys!','0','0','0','0','25610','Silvermoon LVL70ETC 4 say'),
('-1015186','Salandria begins to play air guitar and sing The Power of the Horde.','0','2','0','0','25613','Silvermoon LVL70ETC 5 emote'),

('-1015187','Oh wow!','0','0','0','1','20463','Auchindoun 1 say');

-- -1 020 000 WOTLK texts
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1020001','The zeppelin to Orgrimmar has just arrived! All aboard for Durotar!','0','1','0','0','22077','zeppelinMaster - SAY_DUROTAR_FROM_OG_ARRIVAL'),
('-1020002','The zeppelin to Undercity has just arrived! All aboard for Tirisfal Glades!','0','1','0','0','22099','zeppelinMaster - SAY_TIRISFAL_FROM_UC_ARRIVAL'),
('-1020003','The zeppelin to Grom''gol has just arrived! All aboard for Stranglethorn!','0','1','0','0','22075','zeppelinMaster - SAY_ST_FROM_GROMGOL_ARRIVAL'),
('-1020004','The zeppelin''s leaving for a tour around the bay in less than one minute!  Oh, and if anyone feels like setting me free, I''m on the dock.','0','1','0','0','22337','zeppelinMaster - SAY_WK_DEPARTURE'),
('-1020005','My zeppelin has docked. Despite my being held captive, and the sorry state that she''s in, she''ll be departing the dock again in two minutes.','0','1','0','0','22336','zeppelinMaster - SAY_WK_ARRIVAL'),
('-1020006','The zeppelin to Vengeance Landing has just arrived! All aboard for Howling Fjord!','0','1','0','0','29540','zeppelinMaster - SAY_UC_FROM_VL_ARRIVAL'),
('-1020007','The zeppelin to Warsong Hold has just arrived! All aboard for Borean Tundra!','0','1','0','0','29549','zeppelinMaster - SAY_OG_FROM_BT_ARRIVAL'),
('-1020008','The zeppelin to Thunder Bluff has arrived! All aboard for a smooth ride across the Barrens!','0','1','0','0','35039','zeppelinMaster - SAY_OG_FROM_TB_ARRIVAL'),

('-1020009','You be it now, brudda!','0','1','0','0','25668','budd - SAY_GET_TROLL');

-- -1 033 000 SHADOWFANG KEEP
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1033000','Follow me and I''ll open the courtyard door for you.','0','0','7','25','1331','prisoner ashcrombe SAY_FREE_AS'),
('-1033001','I have just the spell to get this door open. Too bad the cell doors weren''t locked so haphazardly.','0','0','7','0','1332','prisoner ashcrombe SAY_OPEN_DOOR_AS'),
('-1033002','There it is! Wide open. Good luck to you conquering what lies beyond. I must report back to the Kirin Tor at once!','0','0','7','0','1334','prisoner ashcrombe SAY_POST_DOOR_AS'),

('-1033003','Free from this wretched cell at last! Let me show you to the courtyard....','0','0','1','1','1320','prisoner adamant SAY_FREE_AD'),
('-1033004','You are indeed courageous for wanting to brave the horrors that lie beyond this door.','0','0','1','1','1321','prisoner adamant SAY_OPEN_DOOR_AD'),
('-1033005','There we go!','0','0','1','25','1323','prisoner adamant SAY_POST1_DOOR_AD'),
('-1033006','Good luck with Arugal. I must hurry back to Hadrec now.','0','0','1','0','1324','prisoner adamant SAY_POST2_DOOR_AD'),

('-1033007','About time someone killed the wretch.','0','0','1','1','1328','prisoner adamant SAY_BOSS_DIE_AD'),
('-1033008','For once I agree with you... scum.','0','0','7','1','1329','prisoner ashcrombe SAY_BOSS_DIE_AS'),

('-1033009','I have changed my mind loyal servants, you do not need to bring the prisoner all the way to my study, I will deal with him here and now.','0','0','0','1','1456','arugal SAY_INTRO_1'),
('-1033010','Vincent!  You and your pathetic ilk will find no more success in routing my sons and I than those beggardly remnants of the Kirin Tor.','0','0','0','0','5680','arugal SAY_INTRO_2'),
('-1033011','If you will not serve my Master with your sword and knowledge of his enemies...','0','0','0','1','5681','arugal SAY_INTRO_3'),
('-1033012','Your moldering remains will serve ME as a testament to what happens when one is foolish enough to trespass in my domain!','0','0','0','0','5682','arugal SAY_INTRO_4'),

('-1033013','Who dares interfere with the Sons of Arugal?','5791','1','0','0','1435','boss_arugal YELL_FENRUS'),
('-1033014','%s vanishes.','0','2','0','0','1343','prisoner ashcrombe EMOTE_VANISH_AS'),
('-1033015','%s fumbles with the rusty lock on the courtyard door.','0','2','0','432','1322','prisoner adamant EMOTE_UNLOCK_DOOR_AD'),
('-1033016','Arrrgh!','0','0','0','0','1458','deathstalker vincent SAY_VINCENT_DIE'),
('-1033017','You, too, shall serve!','5793','1','0','0','6115','boss_arugal YELL_AGGRO'),
('-1033018','Another falls!','5795','1','0','0','6116','boss_arugal YELL_KILLED_PLAYER'),
('-1033019','Release your rage!','5797','1','0','0','6535','boss_arugal YELL_WORGEN_CURSE'),

('-1033020','Did they bother to tell you who I am and why I am doing this?','0','0','0','6','38586','hummel SAY_INTRO_1'),
('-1033021','...or are they just using you like they do everybody else?','0','0','0','396','38600','hummel SAY_INTRO_2'),
('-1033022','But what does it matter. It is time for this to end.','0','0','0','397','38591','hummel SAY_INTRO_3'),
('-1033023','Baxter! Get in there and help! NOW!','0','0','0','0','38592','hummel SAY_CALL_BAXTER'),
('-1033024','It is time, Frye! Attack!','0','0','0','0','38593','hummel SAY_CALL_FRYE'),
('-1033025','...please don''t think less of me.','0','0','0','0','38597','hummel SAY_DEATH'),

('-1033026','I can''t believe it! You''ve destroyed my pack... Now face my wrath!','0','1','0','0','2086','wolf master nandos YELL_PACK_DEAD');

-- -1 034 000 STOCKADES

-- -1 036 000 DEADMINES
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1036000','You there! Check out that noise.','5775','6','7','0','1148','smite INST_SAY_ALARM1'),
('-1036001','We''re under attack!  A vast, ye swabs! Repel the invaders!','5777','6','7','0','1149','smite INST_SAY_ALARM2'),
('-1036002','You land lubbers are tougher than I thought! I''ll have to improvise!','5778','0','0','21','1344','smite SAY_PHASE_2'),
('-1036003','D''ah! Now you''re making me angry!','5779','0','0','15','1345','smite SAY_PHASE_3');

-- -1 043 000 WAILING CAVERNS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1043000','At last! Naralex can be awakened! Come aid me, brave adventurers!','0','6','0','0','2101','Disciple of Naralex - SAY_INTRO'),
('-1043001','I must make the necessary preparations before the awakening ritual can begin. You must protect me!','0','0','0','0','1255','SAY_PREPARE'),
('-1043002','These caverns were once a temple of promise for regrowth in the Barrens. Now, they are the halls of nightmares.','0','0','0','0','1256','Disciple of Naralex - SAY_FIRST_CORNER'),
('-1043003','Come. We must continue. There is much to be done before we can pull Naralex from his nightmare.','0','0','0','0','1257','Disciple of Naralex - SAY_CONTINUE'),
('-1043004','Within this circle of fire I must cast the spell to banish the spirits of the slain Fanglords.','0','0','0','0','1258','Disciple of Naralex - SAY_CIRCLE_BANISH'),
('-1043005','The caverns have been purified. To Naralex''s chamber we go!','0','0','0','0','1259','Disciple of Naralex - SAY_PURIFIED'),
('-1043006','Beyond this corridor, Naralex lies in fitful sleep. Let us go awaken him before it is too late.','0','0','0','0','1263','Disciple of Naralex - SAY_NARALEX_CHAMBER'),
('-1043007','Protect me brave souls as I delve into the Emerald Dream to rescue Naralex and put an end to this corruption!','0','1','0','0','1264','Disciple of Naralex - SAY_BEGIN_RITUAL'),
('-1043008','%s begins to perform the awakening ritual on Naralex.','0','2','0','0','1265','Disciple of Naralex - EMOTE_RITUAL_BEGIN'),
('-1043009','%s tosses fitfully in troubled sleep.','0','2','0','0','1268','Naralex - EMOTE_NARALEX_AWAKE'),
('-1043010','%s writhes in agony. The Disciple seems to be breaking through.','0','2','0','0','1269','Naralex - EMOTE_BREAK_THROUGH'),
('-1043011','%s dreams up a horrendous vision. Something stirs beneath the murky waters.','0','2','0','0','1270','Naralex - EMOTE_VISION'),
('-1043012','This $N is a minion from Naralex''s nightmare no doubt!.','0','0','0','0','1276','Disciple of Naralex - SAY_MUTANUS'),
('-1043013','I AM AWAKE, AT LAST!','5789','1','0','0','1271','Naralex - SAY_NARALEX_AWAKE'),
('-1043014','At last! Naralex awakes from the nightmare.','0','0','0','0','1267','Disciple of Naralex - SAY_AWAKE'),
('-1043015','Ah, to be pulled from the dreaded nightmare! I thank you, my loyal Disciple, along with your brave companions.','0','0','0','0','1272','Naralex - SAY_NARALEX_THANKYOU'),
('-1043016','We must go and gather with the other Disciplies. There is much work to be done before I can make another attempt to restore the Barrens. Farewell, brave souls!','0','0','0','0','2103','Naralex - SAY_FAREWELL'),
('-1043017','Attacked! Help get this $N off of me!','0','0','0','0','1273','Disciple of Naralex - SAY_AGGRO_1'),
('-1043018','Help!','0','0','0','0','1277','Disciple of Naralex - SAY_AGGRO_2'),
('-1043019','Deal with this $N! I need to prepare to awake Naralex!','0','0','0','0','1274','Disciple of Naralex - SAY_AGGRO_3');

-- -1 047 000 RAZORFEN KRAUL
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1047000','Woo hoo! Finally getting out of here. It''s going to be rough though. Keep your eyes peeled for trouble.','0','0','0','0','1482','willix SAY_READY'),
('-1047001','Up there is where Charlga Razorflank resides. Blasted old crone.','0','0','0','25','1483','willix SAY_1'),
('-1047002','There''s blueleaf tuber in this trench! It''s like gold waiting to be mined I tell you!','0','0','0','0','1484','willix SAY_2'),
('-1047003','There could be danger around every corner here.','0','0','0','0','1485','willix SAY_3'),
('-1047004','I don''t see how these foul animals live in this place... sheesh it smells!','0','0','0','0','1486','willix SAY_4'),
('-1047005','I think I see a way for us to get out of this big twisted mess of a bramble.','0','0','0','0','1487','willix SAY_5'),
('-1047006','Glad to be out of that wretched trench. Not much nicer up here though!','0','0','0','0','1488','willix SAY_6'),
('-1047007','Finally! I''ll be glad to get out of this place.','0','0','0','0','1490','willix SAY_7'),
('-1047008','I think I''ll rest a moment and catch my breath before heading back to Ratchet. Thanks for all the help!','0','0','0','0','1493','willix SAY_END'),
('-1047009','$N heading this way fast! To arms!','0','0','0','0','1546','willix SAY_AGGRO_1'),
('-1047010','Eek! $N coming right at us!','0','0','0','0','1544','willix SAY_AGGRO_2'),
('-1047011','Egads! $N on me!','0','0','0','0','1545','willix SAY_AGGRO_3'),
('-1047012','Help! Get this $N off of me!','0','0','0','0','1547','willix SAY_AGGRO_4');

-- -1 048 000 BLACKFATHOM DEEPS

-- -1 070 000 ULDAMAN
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1070000','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1070001','Who dares awaken Archaedas? Who dares the wrath of the makers!','5855','1','0','0','3400','archaedas SAY_AGGRO'),
('-1070002','Awake ye servants, defend the discs!','5856','1','0','0','6536','archaedas SAY_AWAKE_GUARDIANS'),
('-1070003','To my side, brothers. For the makers!','5857','1','0','0','6537','archaedas SAY_AWAKE_WARDERS'),
('-1070004','Reckless mortal.','5858','1','0','0','6215','archaedas SAY_UNIT_SLAIN'),
('-1070005','%s breaks free from his stone slumber!','0','2','0','0','3399','archaedas EMOTE_BREAK_FREE');

-- -1 090 000 GNOMEREGAN
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1090000','With your help, I can evaluate these tunnels.','0','0','0','1','4050','emi shortfuse SAY_START'),
('-1090001','Let''s see if we can find out where these Troggs are coming from.... and put a stop to the invasion!','0','0','0','1','4051','emi shortfuse SAY_INTRO_1'),
('-1090002','Such devastation... what a horrible mess...','0','0','0','5','4052','emi shortfuse SAY_INTRO_2'),
('-1090003','It''s quiet here....','0','0','0','1','4129','emi shortfuse SAY_INTRO_3'),
('-1090004','...too quiet.','0','0','0','1','4130','emi shortfuse SAY_INTRO_4'),
('-1090005','Look! Over there at the tunnel wall!','0','0','0','25','4131','emi shortfuse SAY_LOOK_1'),
('-1090006','Trogg incursion! Defend me while I blast the hole closed!','0','0','0','5','4132','emi shortfuse SAY_HEAR_1'),
('-1090007','Get this, $n off of me!','0','0','0','0','1073','emi shortfuse SAY_AGGRO_1'),
('-1090008','I don''t think one charge is going to cut it. Keep fending them off!','0','0','0','0','4133','emi shortfuse SAY_CHARGE_1'),
('-1090009','The charges are set. Get back before they blow!','0','0','0','5','4134','emi shortfuse SAY_CHARGE_2'),
('-1090010','Incoming blast in 10 seconds!','0','1','0','5','4135','emi shortfuse SAY_BLOW_1_10'),
('-1090011','Incoming blast in 5 seconds. Clear the tunnel! Stay back!','0','1','0','5','4136','emi shortfuse SAY_BLOW_1_5'),
('-1090012','FIRE IN THE HOLE!','0','1','0','25','32326','emi shortfuse SAY_BLOW_1'),
('-1090013','Well done! Without your help I would have never been able to thwart that wave of troggs.','0','0','0','4','4206','emi shortfuse SAY_FINISH_1'),
('-1090014','Did you hear something?','0','0','0','6','492','emi shortfuse SAY_LOOK_2'),
('-1090015','I heard something over there.','0','0','0','25','4208','emi shortfuse SAY_HEAR_2'),
('-1090016','More troggs! Ward them off as I prepare the explosives!','0','0','0','0','4209','emi shortfuse SAY_CHARGE_3'),
('-1090017','The final charge is set. Stand back!','0','0','0','1','4325','emi shortfuse SAY_CHARGE_4'),
('-1090018','10 seconds to blast! Stand back!!!','0','1','0','5','4326','emi shortfuse SAY_BLOW_2_10'),
('-1090019','5 seconds until detonation!!!!!','0','1','0','5','4327','emi shortfuse SAY_BLOW_2_5'),
('-1090020','Nice work! I''ll set off the charges to prevent any more troggs from making it to the surface.','0','0','0','1','4329','emi shortfuse SAY_BLOW_SOON'),
('-1090021','FIRE IN THE HOLE!','0','1','0','0','32326','emi shortfuse SAY_BLOW_2'),
('-1090022','Superb! Because of your help, my people stand a chance of re-taking our beloved city. Three cheers to you!','0','0','0','0','4446','emi shortfuse SAY_FINISH_2'),

('-1090023','We come from below! You can never stop us!','0','1','0','1','4328','grubbis SAY_GRUBBIS_SPAWN'),

('-1090024','Usurpers! Gnomeregan is mine!','5807','1','0','0','6173','thermaplugg SAY_AGGRO'),
('-1090025','My machines are the future! They''ll destroy you all!','5808','1','0','0','6174','thermaplugg  SAY_PHASE'),
('-1090026','Explosions! MORE explosions! I''ve got to have more explosions!','5809','1','0','0','6176','thermaplugg  SAY_BOMB'),
('-1090027','...and stay dead! He got served','5810','1','0','0','6175','thermaplugg  SAY_SLAY'),

('-1090028','$n attacking! Help!','0','0','0','0','5164','emi shortfuse SAY_AGGRO_2');

-- -1 109 000 SUNKEN TEMPLE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1109000','The walls of the chamber tremble. Something is happening...','0','2','0','0','11191','malfurion stormrage EMOTE_MALFURION'),
('-1109001','Be steadfast, champion. I know why it is that you are here and I know what it is that you seek. Eranikus will not give up the shard freely. He has been twisted... twisted by the same force that you seek to destroy.','0','0','0','0','11193','malfurion stormrge SAY_MALFURION1'),
('-1109002','Are you really surprised? Is it hard to believe that the power of an Old God could reach even inside the Dream? It is true - Eranikus, Tyrant of the Dream, wages a battle against us all. The Nightmare follows in his wake of destruction.','0','0','0','0','11194','malfurion stormrge SAY_MALFURION2'),
('-1109003','Understand this, Eranikus wants nothing more than to be brought to Azeroth from the Dream. Once he is out, he will stop at nothing to destroy my physical manifestation. This, however, is the only way in which you could recover the scepter shard.','0','0','0','0','11195','malfurion stormrge SAY_MAFLURION3'),
('-1109004','You will bring him back into this world, champion.','0','0','0','0','11196','malfurion Stormrge SAY_MALFURION4'),

('-1109005','The shield be down! Rise up Atal''ai! Rise up!','5861','6','0','0','4490','jammalan SAY_JAMMALAN_INTRO'),

('-1109006','HAKKAR LIVES!','5870','1','0','0','6254','avatar SAY_AVATAR_BRAZIER_1'),
('-1109007','I TASTE THE BLOOD OF LIFE!','5868','1','0','0','6252','avatar SAY_AVATAR_BRAZIER_2'),
('-1109008','I DRAW CLOSER TO THIS WORLD!','5867','1','0','0','4543','avatar SAY_AVATAR_BRAZIER_3'),
('-1109009','I AM NEAR!','5869','1','0','0','6253','avatar SAY_AVATAR_BRAZIER_4'),
('-1109010','I AM HERE!','0','1','0','0','4545','avatar SAY_AVATAR_SPAWN'),
('-1109011','You must not summon our god!','0','1','0','0','4530','Nightmare Suppressor SAY_SPAWN_1'),
('-1109012','Stop!  Infidels!','0','1','0','0','4532','Nightmare Suppressor SAY_SPAWN_2'),
('-1109013','No!  We cannot allow you to summon Hakkar!','0','1','0','0','4529','Nightmare Suppressor SAY_SPAWN_3'),
('-1109014','You must not summon our god!','0','1','0','0','4531','Nightmare Suppressor SAY_SPAWN_4'),
('-1109015','NOOOO!!','0','1','0','0','4544','avatar SAY_AVATAR_DESPAWN');

-- -1 129 000 RAZORFEN DOWNS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1129000','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1129001','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1129002','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1129003','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1129004','REUSE_ME','0','0','0','0','0','REUSE_ME'),

('-1129005','All right, stay close. These fiends will jump right out of the shadows at you if you let your guard down.','0','0','0','0','4493','belnistrasz SAY_READY'),
('-1129006','Okay, here we go. It''s going to take about five minutes to shut this thing down through the ritual. Once I start, keep the vermin off of me or it will be the end of us all!','0','0','0','0','4501','belnistrasz SAY_START_RIT'),
('-1129007','You''ll rue the day you crossed me, $N','0','0','0','0','9008','belnistrasz SAY_AGGRO_1'),
('-1129008','Incoming $N - look sharp, friends!','0','0','0','0','9007','belnistrasz SAY_AGGRO_2'),
('-1129009','Three minutes left -- I can feel the energy starting to build! Keep up the solid defense!','0','1','0','0','4504','belnistrasz SAY_3_MIN'),
('-1129010','Just two minutes to go! We''re half way there, but don''t let your guard down!','0','1','0','0','4505','belnistrasz SAY_2_MIN'),
('-1129011','One more minute! Hold on now, the ritual is about to take hold!','0','1','0','0','4506','belnistrasz SAY_1_MIN'),
('-1129012','That''s it -- we made it! The ritual is set in motion, and idol fires are about to go out for good! You truly are the heroes I thought you would be!','0','1','0','4','4507','belnistrasz SAY_FINISH');

-- -1 189 000 SCARLET MONASTERY
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1189000','Ah, I have been waiting for a real challenge!','5830','1','0','0','6194','herod SAY_AGGRO'),
('-1189001','Blades of Light!','5832','1','0','0','6534','herod SAY_WHIRLWIND'),
('-1189002','Light, give me strength!','5833','1','0','0','6195','herod SAY_ENRAGE'),
('-1189003','Hah, is that all?','5831','1','0','0','0','herod SAY_KILL'),
('-1189004','REUSE_ME','0','0','0','0','0','REUSE_ME'),

('-1189005','Infidels! They must be purified!','5835','1','0','0','2847','mograine SAY_MO_AGGRO'),
('-1189006','Unworthy!','5836','1','0','0','75038','mograine SAY_MO_KILL'),
('-1189007','At your side, milady!','5837','1','0','0','6533','mograine SAY_MO_RESSURECTED'),

('-1189008','What, Mograine has fallen? You shall pay for this treachery!','5838','1','0','0','2973','whitemane SAY_WH_INTRO'),
('-1189009','The Light has spoken!','5839','1','0','0','6198','whitemane SAY_WH_KILL'),
('-1189010','Arise, my champion!','5840','1','0','0','6532','whitemane SAY_WH_RESSURECT'),

('-1189011','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1189012','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1189013','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1189014','REUSE_ME','0','0','0','0','0','REUSE_ME'),

('-1189015','The monster got what he deserved.','0','0','1','0','0','vishas SAY_TRIGGER_VORREL'),

('-1189016','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1189017','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1189018','REUSE_ME','0','0','0','0','0','REUSE_ME'),

('-1189019','You will not defile these mysteries!','5842','1','0','0','6199','doan SAY_AGGRO'),
('-1189020','Burn in righteous fire!','5843','1','0','0','6200','doan SAY_SPECIALAE'),

('-1189021','REUSE_ME','0','0','0','0','0','REUSE_ME'),

('-1189022','It is over, your search is done! Let fate choose now, the righteous one.','11961','1','0','0','22261','horseman SAY_ENTRANCE'),
('-1189023','Here''s my body, fit and pure! Now, your blackened souls I''ll cure!','12567','1','0','0','22271','horseman SAY_REJOINED'),
('-1189024','So eager you are for my blood to spill, yet to vanquish me this my head you must kill!','11969','1','0','0','22757','horseman SAY_BODY_DEFEAT'),
('-1189025','Over here, you idiot!','12569','1','0','0','22415','horseman SAY_LOST_HEAD'),
('-1189026','Harken, cur! Tis you I spurn! Now, $N, feel the burn!','12573','1','0','0','22587','horseman SAY_CONFLAGRATION'),
('-1189027','Soldiers arise, stand and fight! Bring victory at last to this fallen knight!','11963','1','0','0','23861','horseman SAY_SPROUTING_PUMPKINS'),
('-1189028','Your body lies beaten, battered and broken. Let my curse be your own, fate has spoken.','11962','1','0','0','40546','horseman SAY_SLAY'),
('-1189029','This end have I reached before. What new adventure lies in store?','11964','1','0','0','23455','horseman SAY_DEATH'),
('-1189030','%s laughs.','0','2','0','0','10315','horseman EMOTE_LAUGH'),
('-1189031','Horseman rise...','0','0','0','0','22695','horseman SAY_PLAYER1'),
('-1189032','Your time is night...','0','0','0','0','22696','horseman SAY_PLAYER2'),
('-1189033','You felt death once...','0','0','0','0','22720','horseman SAY_PLAYER3'),
('-1189034','Now, know demise!','0','0','0','0','22721','horseman SAY_PLAYER4'),

('-1189035','The master has fallen! Avenge him my brethren!','5834','1','0','0','2842','trainee SAY_TRAINEE_SPAWN'),

('-1189036','Bow down! Kneel before the Ashbringer! A new dawn approaches, brother and sisters! Our message will be delivered to the filth of this world through the chosen one!','0','6','0','0','12389','mograine SAY_ASHBRINGER_ENTRANCE');

-- -1 209 000 ZUL'FARRAK
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1209000','How dare you enter my sanctum!','0','0','0','0','3622','zumrah SAY_INTRO'),
('-1209001','Sands consume you!','5872','1','14','0','6221','zumrah SAY_AGGRO'),
('-1209002','Fall!','5873','1','14','0','6222','zumrah SAY_KILL'),
('-1209003','Come to me, my children!','0','0','8','0','3621','zumrah SAY_SUMMON');

-- -1 229 000 BLACKROCK SPIRE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1229000','%s begins to regain its strength!','0','2','0','0','5565','pyroguard EMOTE_BEGIN'),
('-1229001','%s is nearly at full strength!','0','2','0','0','5566','pyroguard EMOTE_NEAR'),
('-1229002','%s regains its power and breaks free of its bonds!','0','2','0','0','5567','pyroguard EMOTE_FULL'),
('-1229003','Ha! Ha! Ha! Thank you for freeing me, fools. Now let me repay you by charring the flesh from your bones.','0','1','0','0','5268','pyroguard SAY_FREE'),

('-1229004','Excellent... it would appear as if the meddlesome insects have arrived just in time to feed my legion. Welcome, mortals!','0','1','0','1','5635','nefarius SAY_INTRO_1'),
('-1229005','Let not even a drop of their blood remain upon the arena floor, my children. Feast on their souls!','0','1','0','1','5640','nefarius SAY_INTRO_2'),
('-1229006','Foolsss... Kill the one in the dress!','0','1','0','0','5665','nefarius SAY_NEFARIUS_TAUNT1'),
('-1229007','Sire, let me join the fray! I shall tear their spines out with my bare hands!','0','1','0','1','5672','rend SAY_REND_TAUNT1'),
('-1229008','Concentrate your attacks upon the healer!','0','1','0','0','5671','nefarius SAY_NEFARIUS_TAUNT2'),
('-1229009','Inconceivable!','0','1','0','0','5666','nefarius SAY_NEFARIUS_TAUNT3'),
('-1229010','Do not force my hand, children! I shall use your hides to line my boots.','0','1','0','0','5667','nefarius SAY_NEFARIUS_TAUNT4'),
('-1229011','Defilers!','0','1','0','0','5678','rend SAY_REND_TAUNT2'),
('-1229012','Impossible!','0','1','0','0','5673','rend SAY_REND_TAUNT3'),
('-1229013','Your efforts will prove fruitless. None shall stand in our way!','0','1','0','0','5668','nefarius SAY_NEFARIUS_TAUNT5'),
('-1229014','THIS CANNOT BE!!! Rend, deal with these insects.','0','1','0','1','5709','nefarius SAY_NEFARIUS_LOSE1'),
('-1229015','With pleasure...','0','1','0','0','5722','rend SAY_REND_ATTACK'),
('-1229016','The Warchief shall make quick work of you, mortals. Prepare yourselves!','0','1','0','25','5720','nefarius SAY_WARCHIEF'),
('-1229017','Taste in my power!','0','1','0','0','5726','nefarius SAY_BUFF_GYTH'),
('-1229018','Your victory shall be short lived. The days of both the Alliance and Horde are coming to an end. The next time we meet shall be the last.','0','1','0','1','5824','nefarius SAY_VICTORY'),

('-1229019','%s is knocked off his drake!','0','2','0','0','5711','rend EMOTE_KNOCKED_OFF'),

('-1229020','Intruders are destroying our eggs! Stop!!','0','1','0','0','5538','rookery hatcher - SAY_ROOKERY_EVENT_START'),

('-1229021','I promise you an eternity of dung clean up duty for this failure!','0','1','0','0','5669','nefarius SAY_NEFARIUS_TAUNT6'),
('-1229022','Use the freezing breath, imbecile!','0','1','0','0','5664','nefarius SAY_NEFARIUS_TAUNT7'),
('-1229023','I will wear your skin as a fashion accessory!','0','1','0','0','66134','nefarius SAY_NEFARIUS_TAUNT8'),
('-1229024','Flee while  you still have chance, mortals. You will pray for a swift death should I enter the arena.','0','1','0','0','5674','rend SAY_REND_TAUNT3');

-- -1 230 000 BLACKROCK DEPTHS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1230000','Ah, hits the spot!','0','0','0','5','5172','rocknot SAY_GOT_BEER'),
('-1230001','Come to aid the Throne!','0','1','0','0','5457','dagran SAY_AGGRO'),
('-1230002','Hail to the king, baby!','0','1','0','0','5431','dagran SAY_SLAY'),
('-1230003','You have challenged the Seven, and now you will die!','0','0','0','0','4894','doomrel SAY_DOOMREL_START_EVENT'),

('-1230004','The Sons of Thaurissan shall watch you perish in the Ring of the Law!','0','1','0','5','5442','grimstone SAY_START_1'),
('-1230005','You have been sentenced to death for crimes against the Dark Iron Nation!','0','1','0','25','5441','grimstone SAY_START_2'),
('-1230006','Unleash the fury and let it be done!','0','1','0','15','5443','grimstone SAY_OPEN_EAST_GATE'),
('-1230007','But your real punishment lies ahead.','0','1','0','1','5445','grimstone SAY_SUMMON_BOSS_1'),
('-1230008','Haha! I bet you thought you were done!','0','1','0','153','5444','grimstone SAY_SUMMON_BOSS_2'),
('-1230009','Good Riddance!','0','1','0','5','5446','grimstone SAY_OPEN_NORTH_GATE'),

('-1230010','Thank you, $N! I''m free!!!','0','0','0','0','5210','dughal SAY_FREE'),
('-1230011','You locked up the wrong Marshal, $N. Prepare to be destroyed!','0','0','0','0','5253','windsor SAY_AGGRO_1'),
('-1230012','I bet you''re sorry now, aren''t you?','0','0','0','0','5252','windsor SAY_AGGRO_2'),
('-1230013','You better hold me back or $N is going to feel some prison house beatings.','0','0','0','0','5250','windsor SAY_AGGRO_3'),
('-1230014','Let''s get a move on. My gear should be in the storage area up this way...','0','0','0','0','5205','windsor SAY_START'),
('-1230015','Check that cell, $N. If someone is alive in there, we need to get them out.','0','0','0','25','5207','windsor SAY_CELL_DUGHAL_1'),
('-1230016','Good work! We''re almost there, $N. This way.','0','0','0','0','5213','windsor SAY_CELL_DUGHAL_3'),
('-1230017','This is it, $N. My stuff should be in that room. Cover me, I''m going in!','0','0','0','0','5214','windsor SAY_EQUIPMENT_1'),
('-1230018','Ah, there it is!','0','0','0','0','5215','windsor SAY_EQUIPMENT_2'),
('-1230019','Can you feel the power, $N??? It''s time to ROCK!','0','0','0','0','5216','reginald_windsor SAY__EQUIPMENT_3'),
('-1230020','Now we just have to free Tobias and we can get out of here. This way!','0','0','0','0','5217','reginald_windsor SAY__EQUIPMENT_4'),
('-1230021','Open it.','0','0','0','25','5222','reginald_windsor SAY_CELL_JAZ_1'),
('-1230022','I never did like those two. Let''s get moving.','0','0','0','0','5223','reginald_windsor SAY_CELL_JAZ_2'),
('-1230023','Open it and be careful this time!','0','0','0','25','5224','reginald_windsor SAY_CELL_SHILL_1'),
('-1230024','That intolerant dirtbag finally got what was coming to him. Good riddance!','0','0','0','66','5225','reginald_windsor SAY_CELL_SHILL_2'),
('-1230025','Alright, let''s go.','0','0','0','0','5227','reginald_windsor SAY_CELL_SHILL_3'),
('-1230026','Open it. We need to hurry up. I can smell those Dark Irons coming a mile away and I can tell you one thing, they''re COMING!','0','0','0','25','5228','reginald_windsor SAY_CELL_CREST_1'),
('-1230027','He has to be in the last cell. Unless... they killed him.','0','0','0','0','5229','reginald_windsor SAY_CELL_CREST_2'),
('-1230028','Get him out of there!','0','0','0','25','5230','reginald_windsor SAY_CELL_TOBIAS_1'),
('-1230029','Excellent work, $N. Let''s find the exit. I think I know the way. Follow me!','0','0','0','0','5221','reginald_windsor SAY_CELL_TOBIAS_2'),
('-1230030','We made it!','0','0','0','4','5231','reginald_windsor SAY_FREE_1'),
('-1230031','Meet me at Maxwell''s encampment. We''ll go over the next stages of the plan there and figure out a way to decode my tablets without the decryption ring.','0','0','0','1','5232','reginald_windsor SAY_FREE_2'),
('-1230032','Thank you! I will run for safety immediately!','0','0','0','0','0','tobias SAY_TOBIAS_FREE_1'),
('-1230033','Finally!! I can leave this dump.','0','0','0','0','5218','tobias SAY_TOBIAS_FREE_2'),

('-1230034','You''ll pay for this insult, $c!','0','0','0','15','25921','coren direbrew SAY_AGGRO'),

('-1230035','%s cries out an alarm!','0','2','0','0','5286','general_angerforge EMOTE_ALARM'),

('-1230036','I want more ale! Give me more ale!','0','0','0','0','5166','rocknot SAY_MORE_BEER'),
('-1230037','You''ll pay for that!','0','0','0','0','5263','Grim Patron SAY_PISSED_PATRON_1'),
('-1230038','No!','0','0','0','0','6352','Grim Patron SAY_PISSED_PATRON_2'),
('-1230039','Oh! Now I''m pissed!','0','0','0','0','5264','Grim Patron SAY_PISSED_PATRON_3'),
('-1230040','Violence! Property damage! None shall pass!!','0','1','0','0','5300','Phalanx YELL_PHALANX_AGGRO'),
('-1230041','Get away from those kegs!','0','1','0','0','4934','Hurley Blackbreath YELL_HURLEY_SPAWN'),
('-1230042','You''ll pay for that!','0','0','0','0','5263','Hurley Blackbreath SAY_HURLEY_AGGRO'),
('-1230043','You can''t hide from us. Prepare to burn!','0','1','0','0','5271','anvilrage guardsman SAY_GUARD_AGGRO'),
('-1230044','That one''s empty!!','0','0','0','60','5167','rocknot SAY_BARREL_1'),
('-1230045','Ah, empty again!','0','0','0','35','5168','rocknot SAY_BARREL_2'),
('-1230046','ALE!','0','0','0','60','5169','rocknot SAY_BARREL_3'),
('-1230047','Im out of here!','0','0','0','0','22412','rocknot SAY_ROCKNOT_DESPAWN'),
('-1230048','HALT! There were...reports...of a...disturbance.','0','1','0','0','5389','Fireguard Destroyer YELL_PATROL_1'),
('-1230049','We...are...investigating.','0','1','0','0','5390','Fireguard Destroyer YELL_PATROL_2'),
('-1230050','Try the boar! It''s my new recipe!','0','0','0','0','5310','Plugger Spazzring SAY_OOC_1'),
('-1230051','Enjoy! You won''t find better ale anywhere!','0','0','0','0','5308','Plugger Spazzring SAY_OOC_2'),
('-1230052','Drink up! There''s more where that came from!','0','0','0','0','5307','Plugger Spazzring SAY_OOC_3'),
('-1230053','Have you tried the Dark Iron Ale? It''s the best!','0','0','0','0','5309','Plugger Spazzring SAY_OOC_4'),
('-1230054','What are you doing over there?','0','1','0','0','5054','Plugger Spazzring YELL_STOLEN_1'),
('-1230055','Hey! Get away from that!','0','1','0','0','5053','Plugger Spazzring YELL_STOLEN_2'),
('-1230056','No stealing the goods!','0','1','0','0','5055','Plugger Spazzring YELL_STOLEN_3'),
('-1230057','That''s it! You''re going down!','0','1','0','0','5060','Plugger Spazzring YELL_AGRRO_1'),
('-1230058','That''s it! No more beer until this mess is sorted out!','0','1','0','0','5267','Plugger Spazzring YELL_AGRRO_2'),
('-1230059','Hey, my pockets were picked!','0','1','0','0','5266','Plugger Spazzring YELL_PICKPOCKETED'),
('-1230060','Ha! You can''t even begin to imagine the futility of your efforts.','0','1','0','0','5314','Emperor Thaurissan YELL_SENATOR_1'),
('-1230061','Is that the best you can do? Do you really expect that you could defeat someone as awe inspiring as me?','0','1','0','0','5315','Emperor Thaurissan YELL_SENATOR_2'),
('-1230062','They were just getting in the way anyways.','0','1','0','0','5313','Emperor Thaurissan YELL_SENATOR_3'),
('-1230063','Your efforts are utterly pointless, fools! You will never be able to defeat me!','0','1','0','0','5312','Emperor Thaurissan YELL_SENATOR_4'),
('-1230064','I will crush you into little tiny pieces!','0','1','0','0','0','Emperor Thaurissan YELL_AGGRO_2'),
('-1230065','Prepare to meet your doom at the hands of Ragnaros'' most powerful servant!','0','1','0','0','5311','Emperor Thaurissan YELL_AGGRO_3'),
('-1230066','Hey, Rocknot!','0','0','0','0','5000','Nagmara SAY_NAGMARA_1'),
('-1230067','Let''s go, honey.','0','0','0','0','5001','Nagmara SAY_NAGMARA_2'),
('-1230068','%s kisses her lover.','0','2','0','0','5002','Nagmara TEXTEMOTE_NAGMARA'),
('-1230069','%s kisses Mistress Nagmara','0','2','0','17','5003','Rocknot TEXTEMOTE_ROCKNOT'),
('-1230070','Emperor Thaurissan does not wish to be disturbed! Turn back now or face your doom, weak mortals!','0','6','0','0','5430','Magmus YELL_MAGMUS_INTRO');

-- -1 249 000 ONYXIA'S LAIR
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1249000','How fortuitous. Usually, I must leave my lair to feed.','0','1','0','0','8286','onyxia SAY_AGGRO'),
('-1249001','Learn your place mortal!','0','1','0','0','8287','onyxia SAY_KILL'),
('-1249002','This meaningless exertion bores me. I''ll incinerate you all from above!','0','1','0','254','8288','onyxia SAY_PHASE_2_TRANS'),
('-1249003','It seems you''ll need another lesson, mortals!','0','1','0','293','8290','onyxia SAY_PHASE_3_TRANS'),
('-1249004','%s takes in a deep breath...','0','3','0','0','7213','onyxia EMOTE_BREATH'),
('-1249005','You seek to lure me from my clutch? You shall pay for your insolence!','0','1','0','0','8570','onyxia SAY_KITE');

-- -1 269 000 OPENING OF THE DARK PORTAL (BLACK MORASS)
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1269000','Why do you persist? Surely you can see the futility of it all. It is not too late! You may still leave with your lives ...','10442','1','0','0','16821','temporus SAY_ENTER'),
('-1269001','So be it ... you have been warned.','10444','1','0','0','16822','temporus SAY_AGGRO'),
('-1269002','Time... sands of time is run out for you.','10443','1','0','0','16824','temporus SAY_BANISH'),
('-1269003','You should have left when you had the chance.','10445','1','0','0','16825','temporus SAY_SLAY1'),
('-1269004','Your days are done.','10446','1','0','0','16826','temporus SAY_SLAY2'),
('-1269005','My death means ... little.','10447','1','0','0','16823','temporus SAY_DEATH'),

('-1269006','Why do you aid the Magus? Just think of how many lives could be saved if the portal is never opened, if the resulting wars could be erased ...','10412','1','0','0','16814','chrono_lord_deja SAY_ENTER'),
('-1269007','If you will not cease this foolish quest, then you will die!','10414','1','0','0','16815','chrono_lord_deja SAY_AGGRO'),
('-1269008','You have outstayed your welcome, Timekeeper. Begone!','10413','1','0','0','16817','chrono_lord_deja SAY_BANISH'),
('-1269009','I told you it was a fool''s quest!','10415','1','0','0','16818','chrono_lord_deja SAY_SLAY1'),
('-1269010','Leaving so soon?','10416','1','0','0','16819','chrono_lord_deja SAY_SLAY2'),
('-1269011','Time ... is on our side.','10417','1','0','0','16816','chrono_lord_deja SAY_DEATH'),

('-1269012','The time has come to shatter this clockwork universe forever! Let us no longer be slaves of the hourglass! I warn you: those who do not embrace the greater path shall become victims of its passing!','10400','1','0','0','16827','aeonus SAY_ENTER'),
('-1269013','Let us see what fate lays in store...','10402','1','0','0','16828','aeonus SAY_AGGRO'),
('-1269014','Your time is up, slave of the past!','10401','1','0','0','16829','aeonus SAY_BANISH'),
('-1269015','One less obstacle in our way!','10403','1','0','0','16831','aeonus SAY_SLAY1'),
('-1269016','No one can stop us! No one!','10404','1','0','0','16832','aeonus SAY_SLAY2'),
('-1269017','It is only a matter...of time.','10405','1','0','0','16830','aeonus SAY_DEATH'),
('-1269018','REUSE ME','0','0','0','0','0','REUSE ME'),

('-1269019','Stop! Do not go any further, mortal. You are ill-prepared to face the forces of the Infinite Dragonflight. Come, let me help you.','0','0','0','0','17814','saat SAY_SAAT_WELCOME'),

('-1269020','The time has come! Gul''dan, order your warlocks to double their efforts! Moments from now the gateway will open, and your Horde will be released upon this ripe, unsuspecting world!','10435','1','0','0','16787','medivh SAY_ENTER'),
('-1269021','What is this? Champions, coming to my aid? I sense the hand of the dark one in this. Truly this sacred event bears his blessing?','10436','1','0','0','16789','medivh SAY_INTRO'),
('-1269022','Champions, my shield grows weak!','10437','1','0','0','16792','medivh SAY_WEAK75'),
('-1269023','My powers must be concentrated on the portal! I do not have time to hold the shield!','10438','1','0','0','16794','medivh SAY_WEAK50'),
('-1269024','The shield is nearly gone! All that I have worked for is in danger!','10439','1','0','0','16795','medivh SAY_WEAK25'),
('-1269025','No! Damn this feeble, mortal coil!','10441','1','0','0','16790','medivh SAY_DEATH'),
('-1269026','I am grateful for your aid, champions. Now, Gul''dan''s Horde will sweep across this world, like a locust swarm, and all my designs, all my carefully laid plans will at last fall into place.','10440','1','0','0','16796','medivh SAY_WIN'),
('-1269027','Orcs of the Horde! This portalis the gateway to your new destiny! Azeroth lies before you, ripe for the taking!','0','1','0','0','19909','medivh SAY_ORCS_ENTER'),
('-1269028','Gul''dan speaks the truth! We should return at once to tell our brothers of the news! Retreat back trought the portal!','0','1','0','0','19911','medivh SAY_ORCS_ANSWER');

-- -1 289 000 SCHOLOMANCE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1289000','School is in session!','0','1','0','0','7145','gandling SAY_GANDLING_SPAWN'),
('-1289001','What is this?! How dare you!','0','1','0','0','6883','vectus YELL_VECTUS_GAMBIT');

-- -1 309 000 ZUL'GURUB
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1309000','Let the coils of hate unfurl!','8421','1','0','0','10026','venoxis SAY_TRANSFORM'),
('-1309001','Ssserenity..at lassst!','0','1','0','0','10460','venoxis SAY_DEATH'),

('-1309002','Lord Hir''eek, grant me wings of vengance!','8417','1','0','0','10027','jeklik SAY_AGGRO'),
('-1309003','I command you to rain fire down upon these invaders!','0','1','0','0','10369','jeklik SAY_RAIN_FIRE'),
('-1309004','Finally ...death. Curse you Hakkar! Curse you!','8422','1','0','0','10452','jeklik SAY_DEATH'),

('-1309005','Draw me to your web mistress Shadra. Unleash your venom!','8418','1','0','0','0','marli SAY_AGGRO'),
('-1309006','Shadra, make of me your avatar!','0','1','0','0','10443','marli SAY_TRANSFORM'),
('-1309007','Aid me my brood!','0','1','0','0','10448','marli SAY_SPIDER_SPAWN'),
('-1309008','Bless you mortal for this release. Hakkar controls me no longer...','8423','1','0','0','10459','marli SAY_DEATH'),

('-1309009','Shirvallah, fill me with your RAGE!','8419','1','0','0','10455','thekal SAY_AGGRO'),
('-1309010','Hakkar binds me no more! Peace at last!','8424','1','0','0','10451','thekal SAY_DEATH'),

('-1309011','Bethekk, your priestess calls upon your might!','8416','1','0','0','10461','arlokk SAY_AGGRO'),
('-1309012','Feast on $n, my pretties!','0','1','0','0','10472','arlokk SAY_FEAST_PANTHER'),
('-1309013','At last, I am free of the Soulflayer!','8412','1','0','0','10450','arlokk SAY_DEATH'),

('-1309015','I''ll feed your souls to Hakkar himself!','8413','1','0','0','10446','mandokir SAY_AGGRO'),
('-1309016','DING!','0','1','0','0','10505','mandokir SAY_DING_KILL'),
('-1309017','GRATS!','0','1','0','0','10601','mandokir SAY_GRATS_JINDO'),
('-1309018','$N! I''m watching you!','0','1','0','0','10604','mandokir SAY_WATCH'),
('-1309019','Don''t make me angry. You won''t like it when I''m angry.','0','4','0','0','10628','mandokir SAY_WATCH_WHISPER'),

('-1309020','PRIDE HERALDS THE END OF YOUR WORLD. COME, MORTALS! FACE THE WRATH OF THE SOULFLAYER!','8414','1','0','0','10447','hakkar SAY_AGGRO'),
('-1309021','Fleeing will do you no good, mortals!','0','1','0','0','10635','hakkar SAY_FLEEING'),
('-1309022','You dare set foot upon Hakkari holy ground? Minions of Hakkar, destroy the infidels!','0','6','0','0','10594','hakkar SAY_MINION_DESTROY'),
('-1309023','Minions of Hakkar, hear your God. The sanctity of this temple has been compromised. Invaders encroach upon holy ground! The Altar of Blood must be protected. Kill them all!','0','6','0','0','10546','hakkar SAY_PROTECT_ALTAR'),

('-1309024','%s goes into a rage after seeing his raptor fall in battle!','0','2','0','0','10545','mandokir EMOTE_RAGE'),

('-1309025','The brood shall not fall!','0','1','0','0','10444','marli SAY_TRANSFORM_BACK'),

('-1309026','%s emits a deafening shriek!','0','2','0','0','10370','jeklik SAY_SHRIEK'),
('-1309027','%s begins to cast a Great Heal!','0','2','0','0','10494','jeklik SAY_HEAL'),

('-1309028','%s fully engulfs in flame and a maddened look appears in his eyes!','0','2','0','0','10425','gurubashi bat rider - SAY_SELF_DETONATE');

-- -1 329 000 STRATHOLME
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1329000','Thanks to Egan','0','0','0','0','6453','freed_soul SAY_ZAPPED0'),
('-1329001','Rivendare must die','0','0','0','0','6455','freed_soul SAY_ZAPPED1'),
('-1329002','Who you gonna call?','0','0','0','0','0','freed_soul SAY_ZAPPED2'),
('-1329003','Don''t cross those beams!','0','0','0','0','0','freed_soul SAY_ZAPPED3'),

('-1329004','An Ash''ari Crystal has fallen! Stay true to the Lich King, my brethren, and attempt to resummon it.','0','6','0','0','6526','thuzadin acolyte SAY_ANNOUNCE_ZIGGURAT_1'),
('-1329005','One of the Ash''ari Crystals has been destroyed! Slay the intruders!','0','6','0','0','6492','thuzadin acolyte SAY_ANNOUNCE_ZIGGURAT_2'),
('-1329006','An Ash''ari Crystal has been toppled! Restore the ziggurat before the Necropolis is vulnerable!','0','6','0','0','6527','thuzadin acolyte SAY_ANNOUNCE_ZIGGURAT_3'),
('-1329007','The Ash''ari Crystals have been destroyed! The Slaughterhouse is vulnerable!','0','6','0','0','6289','baron rivendare SAY_ANNOUNCE_RIVENDARE'),

('-1329008','Intruders at the Service Gate! Lord Rivendare must be warned!','0','6','0','0','6162','barthilas SAY_WARN_BARON'),
('-1329009','Intruders! More pawns of the Argent Dawn, no doubt. I already count one of their number among my prisoners. Withdraw from my domain before she is executed!','0','6','0','0','11812','baron rivendare SAY_ANNOUNCE_RUN_START'),
('-1329010','You''re still here? Your foolishness is amusing! The Argent Dawn wench needn''t suffer in vain. Leave at once and she shall be spared!','0','6','0','0','11813','baron rivendare SAY_ANNOUNCE_RUN_10_MIN'),
('-1329011','I shall take great pleasure in taking this poor wretch''s life! It''s not too late, she needn''t suffer in vain. Turn back and her death shall be merciful!','0','6','0','0','11815','baron rivendare SAY_ANNOUNCE_RUN_5_MIN'),
('-1329012','May this prisoner''s death serve as a warning. None shall defy the Scourge and live!','0','6','0','0','11814','baron rivendare SAY_ANNOUNCE_RUN_FAIL'),
('-1329013','So you see fit to toy with the Lich King''s creations? Ramstein, be sure to give the intruders a proper greeting.','0','6','0','0','6398','baron rivendare SAY_ANNOUNCE_RAMSTEIN'),
('-1329014','Time to take matters into my own hands. Come. Enter my domain and challenge the might of the Scourge!','0','6','0','0','6401','baron rivendare SAY_UNDEAD_DEFEAT'),
('-1329015','You did it... you''ve slain Baron Rivendare! The Argent Dawn shall hear of your valiant deeds!','0','0','0','0','11931','ysida SAY_EPILOGUE'),

('-1329016','Today you have unmade what took me years to create! For this you shall all die by my hand!','0','1','0','0','6441','dathrohan SAY_AGGRO'),
('-1329017','You fools think you can defeat me so easily? Face the true might of the Nathrezim!','0','1','0','0','6447','dathrohan SAY_TRANSFORM'),
('-1329018','Damn you mortals! All my plans of revenge, all my hate... all burned to ash...','0','0','0','0','6442','dathrohan SAY_DEATH'),
('-1329019','Don''t worry about me!  Slay this dreadful beast and cleanse this world of his foul taint!','0','6','0','0','11816','ysida YSIDA_SAY_RUN_5_MIN'),
('-1329020','My death means nothing.... light... will... prevail!','0','6','0','0','11817','ysida YSIDA_SAY_RUN_FAIL'),

('-1329021','Rivendare! I come for you!','0','1','0','0','6236','aurius YELL_AURIUS_AGGRO'),
('-1329022','Argh!','0','0','0','0','6237','aurius SAY_AURIUS_DEATH'),

('-1329023','Move back and hold the line! We cannot fail or all will be lost!','0','1','0','0','6377','YELL_BASTION_BARRICADE'),
('-1329024','Move to the stairs and defend!','0','1','0','0','6378','YELL_BASTION_STAIRS'),
('-1329025','The Scourge have broken into the Bastion!  Redouble your efforts!  We must not fail!','0','1','0','0','6436','YELL_BASTION_ENTRANCE'),
('-1329026','They have broken into the Hall of Lights!  We must stop the intruders!','0','1','0','0','6379','YELL_BASTION_HALL_LIGHTS'),
-- They have broken into the Hall of Lights!  We must stop the intruders!
-- Our defenses are failing!  By the light we must prevail!
('-1329027','The Scourge have broken through in all wings!  May the light defeat these foul creatures!  We shall fight to the last!','0','1','0','0','6439','YELL_BASTION_INNER_1'),
('-1329028','This will not be the end of the Scarlet Crusade!  You will not break our line!','0','1','0','0','6380','YELL_BASTION_INNER_2');

-- -1 349 000 MARAUDON

-- -1 389 000 RAGEFIRE CHASM

-- -1 409 000 MOLTEN CORE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1409000','%s performs one last service for Ragnaros...','0','2','0','0','8253','geddon EMOTE_SERVICE'),
('-1409001','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1409002','%s refuses to die while its master is in trouble.','0','2','0','0','7865','core rager EMOTE_LOWHP'),

('-1409003','Reckless mortals! None may challenge the Sons of the Living flame!','8035','1','0','0','7612','majordomo SAY_AGGRO'),
('-1409004','The runes of warding have been destroyed! Hunt down the infidels, my brethren!','8039','6','0','0','7566','majordomo SAY_SPAWN'),
('-1409005','Ashes to Ashes!','8037','1','0','0','9425','majordomo SAY_SLAY_1'),
('-1409006','Burn mortals! Burn for this transgression!','8036','1','0','0','0','majordomo SAY_SLAY_2'),
('-1409007','Impossible! Stay your attack, mortals... I submit! I submit!','8038','1','0','0','7561','majordomo SAY_DEFEAT_1'),

('-1409008','Behold Ragnaros - the Firelord! He who was ancient when this world was young! Bow before him, mortals! Bow before your ending!','8040','1','0','0','7657','ragnaros SAY_SUMMON_MAJ'),
('-1409009','TOO SOON! YOU HAVE AWAKENED ME TOO SOON, EXECUTUS! WHAT IS THE MEANING OF THIS INTRUSION???','8043','1','0','15','7636','ragnaros SAY_ARRIVAL1_RAG'),
('-1409010','These mortal infidels, my lord! They have invaded your sanctum and seek to steal your secrets!','8041','1','0','0','7661','ragnaros SAY_ARRIVAL2_MAJ'),
('-1409011','FOOL! YOU ALLOWED THESE INSECTS TO RUN RAMPANT THROUGH THE HALLOWED CORE? AND NOW YOU LEAD THEM TO MY VERY LAIR? YOU HAVE FAILED ME, EXECUTUS! JUSTICE SHALL BE MET, INDEED!','8044','1','0','15','7662','ragnaros SAY_ARRIVAL3_RAG'),
('-1409012','NOW FOR YOU, INSECTS! BOLDLY, YOU SOUGHT THE POWER OF RAGNAROS. NOW YOU SHALL SEE IT FIRSTHAND!','8045','1','0','15','7685','ragnaros SAY_ARRIVAL5_RAG'),

('-1409013','COME FORTH, MY SERVANTS! DEFEND YOUR MASTER!','8049','1','0','0','8572','ragnaros SAY_REINFORCEMENTS1'),
('-1409014','YOU CANNOT DEFEAT THE LIVING FLAME! COME YOU MINIONS OF FIRE! COME FORTH YOU CREATURES OF HATE! YOUR MASTER CALLS!','8050','1','0','0','8573','ragnaros SAY_REINFORCEMENTS2'),
('-1409015','BY FIRE BE PURGED!','8046','1','0','0','9426','ragnaros SAY_HAND'),
('-1409016','TASTE THE FLAMES OF SULFURON!','8047','1','0','0','9427','ragnaros SAY_WRATH'),
('-1409017','DIE INSECT!','8051','1','0','0','52204','ragnaros SAY_KILL'),
('-1409018','MY PATIENCE IS DWINDLING! COME GNATS: TO YOUR DEATH!','8048','1','0','0','0','ragnaros SAY_MAGMABURST'),

('-1409019','You think you''ve won already? Perhaps you''ll need another lesson in pain!','0','1','0','0','8545','majordomo SAY_LAST_ADD'),
('-1409020','Brashly, you have come to wrest the secrets of the Living Flame! You will soon regret the recklessness of your quest.','0','1','0','0','7567','majordomo SAY_DEFEAT_2'),
('-1409021','I go now to summon the lord whose house this is. Should you seek an audience with him, your paltry lives will surely be forfeit! Nevertheless, seek out his lair, if you dare!','0','1','0','0','7568','majordomo SAY_DEFEAT_3'),
('-1409022','My flame! Please don''t take away my flame... ','8042','1','0','0','0','ragnaros SAY_ARRIVAL4_MAJ'),
('-1409023','Very well, $n.','0','0','0','0','7649','majordomo SAY_SUMMON_0'),
('-1409024','Impudent whelps! You''ve rushed headlong to your own deaths! See now, the master stirs!','0','1','0','0','7655','majordomo SAY_SUMMON_1'),
('-1409025','%s forces one of his Firesworn minions to erupt!','0','2','0','0','8254','Garr EMOTE_MASSIVE_ERUPTION');

-- -1 429 000 DIRE MAUL
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1429000','The demon is loose! Quickly we must restrain him!','0','6','0','0','9364','highborne summoner SAY_FREE_IMMOLTHAR'),
('-1429001','Who dares disrupt the sanctity of Eldre''Thalas? Face me, cowards!','0','6','0','0','9407','prince tortheldrin SAY_KILL_IMMOLTHAR'),

('-1429002','At last... Freed from his cursed grasp!','0','6','0','0','9104','old ironbark SAY_IRONBARK_REDEEM'),

('-1429003','The king is dead - OH NOES! Summon Mizzle da Crafty! He knows what to do next!','0','1','0','0','9472','cho''rush SAY_KING_DEAD'),

('-1429004','Doh!','0','0','0','0','9482','slip''kik SAY_SLIPKIK_TRAP');

-- -1 469 000 BLACKWING LAIR
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1469000','None of your kind should be here!  You''ve doomed only yourselves!','8286','1','0','0','9967','broodlord SAY_AGGRO'),
('-1469001','Clever, mortals - but I am not so easily lured away from my sanctum!','8287','1','0','0','9968','broodlord SAY_LEASH'),

('-1469002','Run! They are coming!','0','1','0','0','9031','vaelastrasz blackwing tech SAY_INTRO_TECH'),
('-1469003','%s flinches as its skin shimmers.','0','2','0','0','9793','chromaggus EMOTE_SHIMMER'),

('-1469004','In this world where time is your enemy, it is my greatest ally. This grand game of life that you think you play in fact plays you. To that I say...','0','1','0','1','9907','victor_nefarius SAY_GAMESBEGIN_1'),
('-1469005','Let the games begin!','8280','1','0','22','9845','victor_nefarius SAY_GAMESBEGIN_2'),
('-1469006','Ah...the heroes. You are persistent, aren''t you? Your ally here attempted to match his power against mine - and paid the price. Now he shall serve me...by slaughtering you.','8279','1','0','23','9794','victor_nefarius SAY_NEFARIUS_CORRUPT'),

('-1469007','Well done, my minions. The mortals'' courage begins to wane! Now, let''s see how they contend with the true Lord of Blackrock Spire!!!','8288','6','0','0','9973','nefarian SAY_AGGRO'),
('-1469008','Enough! Now you vermin shall feel the force of my birthright, the fury of the earth itself.','8289','1','0','0','0','nefarian SAY_XHEALTH'),
('-1469009','BURN! You wretches! BURN!','8290','6','0','0','9974','nefarian SAY_SHADOWFLAME'),
('-1469010','Impossible! Rise my minions!  Serve your master once more!','8291','1','0','0','9883','nefarian SAY_RAISE_SKELETONS'),
('-1469011','Worthless $N! Your friends will join you soon enough!','8293','1','0','0','9972','nefarian SAY_SLAY'),
('-1469012','This cannot be!  I am the master here!  You mortals are nothing to my kind!  Do you hear me? Nothing!','8292','1','0','0','9971','nefarian SAY_DEATH'),
('-1469013','Mages too? You should be more careful when you play with magic...','0','1','0','0','9850','nefarian SAY_MAGE'),
('-1469014','Warriors, I know you can hit harder than that! Let''s see it!','0','1','0','0','9855','nefarian SAY_WARRIOR'),
('-1469015','Druids and your silly shapeshifting. Let''s see it in action!','0','1','0','0','9851','nefarian SAY_DRUID'),
('-1469016','Priests! If you''re going to keep healing like that, we might as well make it a little more interesting!','0','1','0','0','9848','nefarian SAY_PRIEST'),
('-1469017','Paladins, I''ve heard you have many lives. Show me.','0','1','0','0','9853','nefarian SAY_PALADIN'),
('-1469018','Shamans, show me what your totems can do!','0','1','0','0','9854','nefarian SAY_SHAMAN'),
('-1469019','Warlocks, you shouldn''t be playing with magic you don''t understand. See what happens?','0','1','0','0','9852','nefarian SAY_WARLOCK'),
('-1469020','Hunters and your annoying pea-shooters!','0','1','0','0','9849','nefarian SAY_HUNTER'),
('-1469021','Rogues? Stop hiding and face me!','0','1','0','0','9856','nefarian SAY_ROGUE'),

('-1469022','You''ll pay for forcing me to do this.','8275','1','0','0','9961','razorgore SAY_EGGS_BROKEN1'),
('-1469023','Fools! These eggs are more precious than you know!','8276','1','0','0','9962','razorgore SAY_EGGS_BROKEN2'),
('-1469024','No - not another one! I''ll have your heads for this atrocity!','8277','1','0','0','9963','razorgore SAY_EGGS_BROKEN3'),
('-1469025','If I fall into the abyss I''ll take all of you mortals with me...','8278','1','0','0','9591','razorgore SAY_RAZORGORE_DEATH'),

('-1469026','Too late, friends! Nefarius'' corruption has taken hold...I cannot...control myself.','8281','1','0','1','9886','vaelastrasz SAY_LINE1'),
('-1469027','I beg you, mortals - FLEE! Flee before I lose all sense of control! The black fire rages within my heart! I MUST- release it! ','8282','1','0','1','9887','vaelastrasz SAY_LINE2'),
('-1469028','FLAME! DEATH! DESTRUCTION! Cower, mortals before the wrath of Lord...NO - I MUST fight this! Alexstrasza help me, I MUST fight it! ','8283','1','0','1','9888','vaelastrasz SAY_LINE3'),
('-1469029','Nefarius'' hate has made me stronger than ever before! You should have fled while you could, mortals! The fury of Blackrock courses through my veins!','8285','1','0','0','9965','vaelastrasz SAY_HALFLIFE'),
('-1469030','Forgive me, $N! Your death only adds to my failure!','8284','1','0','0','9964','vaelastrasz SAY_KILLTARGET'),

('-1469031','Death Knights, get over here!','0','1','0','0','30668','nefarian SAY_DEATH_KNIGHT'),

('-1469032','Get up, little red wyrm...and destroy them!','0','1','0','1','9844','victor_nefarius SAY_NEFARIUS_CORRUPT_2'),

('-1469033','%s flee as the controlling power of the orb is drained.','0','2','0','0','9592','razorgore EMOTE_TROOPS_FLEE'),

('-1469034','Run! They are coming.','0','1','0','0','9031','blackwing technician SAY_TECHNICIAN_RUN'),

('-1469035','Orb of Domination loses power and shuts off!','0','2','0','0','8092','razorgore EMOTE_ORB_SHUT_OFF'),

('-1469036','Looking for this: [Red Scepter Shard]? Come and get it...','0','6','0','0','11267','nefarius YELL_REDSHARD_TAUNT_1'),
('-1469037','I wonder, heroes, what will you do when I manage to destroy this shard? Will the world be beyond Salvation?','0','6','0','0','11214','nefarius YELL_REDSHARD_TAUNT_2'),
('-1469038','%s''s laughter echoes through the halls of Blackwing.','0','7','0','0','11230','nefarius EMOTE_REDSHARD_TAUNT_1'),
('-1469039','What''s this? A chip in the shard? Finally, a weakness exposed... It won''t be long now.','0','6','0','0','11215','nefarius YELL_REDSHARD_TAUNT_3'),
('-1469040','Not even my father could have planned such tragedy... such chaos. You will never make it! Time is running out!','0','6','0','0','11217','nefarius YELL_REDSHARD_TAUNT_4'),
('-1469041','Would it not be glorious if you somehow managed to defeat me but not before I could destroy the only hope Kalimdor may have? I am giddy with anticipation. Soon... Soon it will be dust.','0','6','0','0','11216','nefarius YELL_REDSHARD_TAUNT_5'),
('-1469042','Ah, the protective layer is nearly gone. Do you hear that, heroes? That... That is the sound of inevitability. Of your hopes and dreams being crushed under the weight of my being.','0','6','0','0','11218','nefarius YELL_REDSHARD_TAUNT_6'),
('-1469044','%s''laughter can once more be heard through the halls of Blackwing.','0','7','0','0','11231','nefarius EMOTE_REDSHARD_TAUNT_2'),
('-1469043','The scepter shard is no more! Your beloved Kalimdor shall soon be a desolate wasteland. I will, of course, turn the Eastern Kingdoms into an ashen, lifeless void...','0','6','0','0','11219','nefarius YELL_REDSHARD_TAUNT_7');

-- -1 509 000 RUINS OF AHN'QIRAJ
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1509000','%s senses your fear.','0','2','0','0','11441','moam EMOTE_AGGRO'),
('-1509001','%s bristles with energy!','0','2','0','0','12781','moan EMOTE_MANA_FULL'),
('-1509028','%s drains your mana and turns to stone.','0','2','0','0','11474','moam EMOTE_ENERGIZING'),

('-1509002','%s sets eyes on $n!','0','2','0','0','11074','buru EMOTE_TARGET'),

('-1509003','They come now. Try not to get yourself killed, young blood.','0','1','0','22','11012','andorov SAY_ANDOROV_INTRO_3'),
('-1509004','Remember, Rajaxx, when I said I''d kill you last?','0','1','0','22','11028','andorov SAY_ANDOROV_INTRO_1'),

('-1509005','The time of our retribution is at hand! Let darkness reign in the hearts of our enemies!','8612','1','0','0','11440','rajaxx SAY_WAVE3'),
('-1509006','No longer will we wait behind barred doors and walls of stone! No longer will our vengeance be denied! The dragons themselves will tremble before our wrath!','8610','1','0','0','11439','rajaxx SAY_WAVE4'),
('-1509007','Fear is for the enemy! Fear and death!','8608','1','0','0','11438','rajaxx SAY_WAVE5'),
('-1509008','Staghelm will whimper and beg for his life, just as his whelp of a son did! One thousand years of injustice will end this day!','8611','1','0','0','11437','rajaxx SAY_WAVE6'),
('-1509009','Fandral! Your time has come! Go and hide in the Emerald Dream and pray we never find you!','8607','1','0','0','11436','rajaxx SAY_WAVE7'),
('-1509010','Impudent fool! I will kill you myself!','8609','1','0','0','11435','rajaxx SAY_INTRO'),
('-1509011','Attack and make them pay dearly!','8603','1','0','0','0','rajaxx SAY_UNK1'),
('-1509012','Crush them! Drive them out!','8605','1','0','0','0','rajaxx SAY_UNK2'),
('-1509013','Do not hesitate! Destroy them!','8606','1','0','0','0','rajaxx SAY_UNK3'),
('-1509014','Warriors, Captains, continue the fight! ','8613','1','0','0','11434','rajaxx SAY_UNK4'),
('-1509015','You are not worth my time $N!','8614','1','0','0','11067','rajaxx SAY_DEAGGRO'),
('-1509016','Breathe your last!','8604','1','0','0','11433','rajaxx SAY_KILLS_ANDOROV'),
('-1509017','Soon you will know the price of your meddling, mortals... The master is nearly whole... And when he rises, your world will cease!','0','1','0','0','11429','rajaxx SAY_COMPLETE_QUEST'),

('-1509018','I am rejuvenated!','8593','1','0','0','0','ossirian SAY_SURPREME1'),
('-1509019','My powers are renewed!','8595','1','0','0','0','ossirian SAY_SURPREME2'),
('-1509020','My powers return!','8596','1','0','0','0','ossirian SAY_SURPREME3'),
('-1509021','Protect the city at all costs!','8597','1','0','0','0','ossirian SAY_RAND_INTRO1'),
('-1509022','The walls have been breached!','8599','6','0','0','11720','ossirian SAY_RAND_INTRO2'),
('-1509023','To your posts. Defend the city.','8600','1','0','0','0','ossirian SAY_RAND_INTRO3'),
('-1509024','Tresspassers will be terminated.','8601','1','0','0','0','ossirian SAY_RAND_INTRO4'),
('-1509025','Sands of the desert, rise and block out the sun!','8598','1','0','0','11449','ossirian SAY_AGGRO'),
('-1509026','You are terminated.','8602','1','0','0','11450','ossirian SAY_SLAY'),
('-1509027','I...have...failed. ','8594','1','0','0','11451','ossirian SAY_DEATH'),
-- 28 (above) = EMOTE_ENERGIZING
('-1509029','Come get some!','0','0','0','0','11478','andorov SAY_ANDOROV_INTRO_4'),
('-1509030','Kill first, ask questions later... Incoming!','0','1','0','0','11032','andorov SAY_ANDOROV_ATTACK_START'),
('-1509031','I lied...','0','1','0','0','11477','andorov SAY_ANDOROV_INTRO_2'),
('-1509032','My work here is done. I''ll be sure to let Mar''alith and the others at Cenarion Hold know that you helped me out a little. Be careful in here, you don''t have me holdin'' your hand past this point.','0','0','0','0','11560','andorov SAY_ANDOROV_DESPAWN');

-- -1 531 000 TEMPLE OF AHN'QIRAJ
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1531000','Are you so eager to die? I will be happy to accommodate you...','8615','1','0','0','11445','skeram SAY_AGGRO'),
('-1531001','Let your death serve as an example!','8617','1','0','0','11446','skeram SAY_SLAY'),
('-1531002','You only delay the inevitable!','8622','1','0','0','11447','skeram SAY_DEATH'),

('-1531003','REUSE_ME','0','0','0','0','0','REUSE'),
('-1531004','REUSE_ME','0','0','0','0','0','REUSE'),
('-1531005','REUSE_ME','0','0','0','0','0','REUSE'),
('-1531006','REUSE_ME','0','0','0','0','0','REUSE'),
('-1531007','REUSE_ME','0','0','0','0','0','REUSE'),

('-1531008','You will be judged for defiling these sacred grounds!  The laws of the Ancients will not be challenged!  Trespassers will be annihilated!','8646','1','0','0','11442','sartura SAY_AGGRO'),
('-1531009','I sentence you to death!','8647','1','0','0','11443','sartura SAY_SLAY'),
('-1531010','I serve to the last!','8648','1','0','0','11444','sartura SAY_DEATH'),

('-1531011','%s is weakened!','0','2','0','0','11476','cthun EMOTE_WEAKENED'),

('-1531012','The massive floating eyeball in the center of the chamber turns its gaze upon you. You stand before a god.','0','7','0','0','11700','eye cthun EMOTE_INTRO'),
('-1531013','Only flesh and bone. Mortals are such easy prey...','0','1','0','1','11702','veklor SAY_INTRO_1'),
('-1531014','Where are your manners, brother. Let us properly welcome our guests.','0','1','0','5','11706','veknilash SAY_INTRO_2'),
('-1531015','There will be pain...','0','1','0','0','11707','veklor SAY_INTRO_3'),
('-1531016','Oh so much pain...','0','1','0','5','11708','veknilash SAY_INTRO_4'),
('-1531017','Come, little ones.','0','1','0','15','11709','veklor SAY_INTRO_5'),
('-1531018','The feast of souls begins now...','0','1','0','15','11710','veknilash SAY_INTRO_6'),

('-1531019','It''s too late to turn away.','8623','1','0','0','0','veklor SAY_AGGRO_1'),
('-1531020','Prepare to embrace oblivion!','8626','1','0','0','0','veklor SAY_AGGRO_2'),
('-1531021','Like a fly in a web.','8624','1','0','0','0','veklor SAY_AGGRO_3'),
('-1531022','Your brash arrogance!','8628','1','0','0','0','veklor SAY_AGGRO_4'),
('-1531023','You will not escape death!','8629','1','0','0','11453','veklor SAY_SLAY'),
('-1531024','My brother...NO!','8625','1','0','0','11452','veklor SAY_DEATH'),
('-1531025','To decorate our halls!','8627','1','0','0','0','veklor SAY_SPECIAL'),

('-1531026','Ah, lambs to the slaughter!','8630','1','0','0','25628','veknilash SAY_AGGRO_1'),
('-1531027','Let none survive!','8632','1','0','0','0','veknilash SAY_AGGRO_2'),
('-1531028','Join me brother, there is blood to be shed!','8631','1','0','0','0','veknilash SAY_AGGRO_3'),
('-1531029','Look brother, fresh blood!','8633','1','0','0','0','veknilash SAY_AGGRO_4'),
('-1531030','Your fate is sealed!','8635','1','0','0','11455','veknilash SAY_SLAY'),
('-1531031','Vek''lor, I feel your pain!','8636','1','0','0','11454','veknilash SAY_DEATH'),
('-1531032','Shall be your undoing!','8634','1','0','0','0','veknilash SAY_SPECIAL'),

('-1531033','Death is close...','8580','4','0','0','0','cthun SAY_WHISPER_1'),
('-1531034','You are already dead.','8581','4','0','0','0','cthun SAY_WHISPER_2'),
('-1531035','Your courage will fail.','8582','4','0','0','0','cthun SAY_WHISPER_3'),
('-1531036','Your friends will abandon you.','8583','4','0','0','0','cthun SAY_WHISPER_4'),
('-1531037','You will betray your friends.','8584','4','0','0','0','cthun SAY_WHISPER_5'),
('-1531038','You will die.','8585','4','0','0','0','cthun SAY_WHISPER_6'),
('-1531039','You are weak.','8586','4','0','0','65263','cthun SAY_WHISPER_7'),
('-1531040','Your heart will explode.','8587','4','0','0','0','cthun SAY_WHISPER_8'),

('-1531041','%s begins to slow!','0','2','0','0','0','viscidus EMOTE_SLOW'),
('-1531042','%s is freezing up!','0','2','0','0','0','viscidus EMOTE_FREEZE'),
('-1531043','%s is frozen solid!','0','2','0','0','0','viscidus EMOTE_FROZEN'),
('-1531044','%s begins to crack!','0','2','0','0','0','viscidus EMOTE_CRACK'),
('-1531045','%s looks ready to shatter!','0','2','0','0','0','viscidus EMOTE_SHATTER'),
('-1531046','%s explodes!','0','2','0','0','28803','viscidus EMOTE_EXPLODE'),

('-1531047','%s shares his powers with his brethren.','0','2','0','0','11692','anubisath sentinelle EMOTE_SHARE'),

('-1531048','%s is devoured!','0','2','0','0','11115','silithid royalty EMOTE_CONSUMED');

-- -1 532 000 KARAZHAN
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1532000','Well done Midnight!','9173','1','0','0','15334','attumen SAY_MIDNIGHT_KILL'),
('-1532001','Cowards! Wretches!','9167','1','0','0','13459','attumen SAY_APPEAR1'),
('-1532002','Who dares attack the steed of the Huntsman?','9298','1','0','0','15378','attumen SAY_APPEAR2'),
('-1532003','Perhaps you would rather test yourselves against a more formidable opponent?!','9299','1','0','0','15379','attumen SAY_APPEAR3'),
('-1532004','Come Midnight, let''s disperse this petty rabble!','9168','1','0','0','13456','attumen SAY_MOUNT'),
('-1532005','It was... inevitable.','9169','1','0','0','13460','attumen SAY_KILL1'),
('-1532006','Another trophy to add to my collection!','9300','1','0','0','15333','attumen SAY_KILL2'),
('-1532007','Weapons are merely a convenience for a warrior of my skill!','9166','1','0','0','13490','attumen SAY_DISARMED'),
('-1532008','Always knew... someday I would become... the hunted.','9165','1','0','0','13462','attumen SAY_DEATH'),
('-1532009','Such easy sport.','9170','1','0','0','0','attumen SAY_RANDOM1'),
('-1532010','Amateurs! Do not think you can best me! I kill for a living.','9304','1','0','0','0','attumen SAY_RANDOM2'),

('-1532011','Hm, unannounced visitors. Preparations must be made...','9211','1','0','0','15021','moroes SAY_AGGRO'),
('-1532012','Now, where was I? Oh, yes...','9215','1','0','0','15339','moroes SAY_SPECIAL_1'),
('-1532013','You rang?','9316','1','0','0','15340','moroes SAY_SPECIAL_2'),
('-1532014','One more for dinner this evening.','9214','1','0','0','13461','moroes SAY_KILL_1'),
('-1532015','Time... Never enough time.','9314','1','0','0','15085','moroes SAY_KILL_2'),
('-1532016','I''ve gone and made a mess.','9315','1','0','0','0','moroes SAY_KILL_3'),
('-1532017','How terribly clumsy of me.','9213','1','0','0','15022','moroes SAY_DEATH'),

('-1532018','Your behavior will not be tolerated.','9204','1','0','0','15079','maiden SAY_AGGRO'),
('-1532019','Ah ah ah...','9207','1','0','0','15082','maiden SAY_SLAY1'),
('-1532020','This is for the best.','9312','1','0','0','15084','maiden SAY_SLAY2'),
('-1532021','Impure thoughts lead to profane actions.','9311','1','0','0','15083','maiden SAY_SLAY3'),
('-1532022','Cast out your corrupt thoughts.','9313','1','0','0','15080','maiden SAY_REPENTANCE1'),
('-1532023','Your impurity must be cleansed.','9208','1','0','0','13396','maiden SAY_REPENTANCE2'),
('-1532024','Death comes. Will your conscience be clear?','9206','1','0','0','15081','maiden SAY_DEATH'),

('-1532025','Oh at last, at last I can go home!','9190','1','0','0','15068','dorothee SAY_DOROTHEE_DEATH'),
('-1532026','Don''t let them hurt us Tito! Oh, you won''t, will you?','9191','1','0','0','15066','dorothee SAY_DOROTHEE_SUMMON'),
('-1532027','Tito! Oh Tito, no!','9192','1','0','0','15069','dorothee SAY_DOROTHEE_TITO_DEATH'),
('-1532028','Oh Tito, we simply must find a way home! The old wizard could be our only hope! Strawman, Roar, Tinhead, will you - wait... oh golly, look we have visitors!','9195','1','0','0','15067','dorothee SAY_DOROTHEE_AGGRO'),

('-1532029','I''m not afraid a'' you! Do you wanna'' fight? Huh, do ya''? C''mon! I''ll fight ya'' with both paws behind my back!','9227','1','0','0','19276','roar SAY_ROAR_AGGRO'),
('-1532030','You didn''t have to go and do that!','9229','1','0','0','15133','roar SAY_ROAR_DEATH'),
('-1532031','I think I''m going to go take fourty winks.','9230','1','0','0','0','roar SAY_ROAR_SLAY'),

('-1532032','Now what should I do with you? I simply can''t make up my mind.','9254','1','0','0','19277','strawman SAY_STRAWMAN_AGGRO'),
('-1532033','Don''t let them make... a mattress outta'' me.','9256','1','0','0','15135','strawman SAY_STRAWMAN_DEATH'),
('-1532034','I guess I''m not a failure after all.','9257','1','0','0','15136','strawman SAY_STRAWMAN_SLAY'),

('-1532035','I could really use a heart. Say, can I have yours?','9268','1','0','0','19278','tinhead SAY_TINHEAD_AGGRO'),
('-1532036','Back to being an old rust bucket.','9270','1','0','0','15155','tinhead SAY_TINHEAD_DEATH'),
('-1532037','Guess I''m not so rusty, after all.','9271','1','0','0','15156','tinhead SAY_TINHEAD_SLAY'),
('-1532038','%s begins to rust!','0','2','0','0','14361','tinhead EMOTE_RUST'),

('-1532039','Woe to each and every one of you, my pretties!','9179','1','0','25','15050','crone SAY_CRONE_INTRO'),
('-1532040','It will all be over soon!','9307','1','0','25','15332','crone SAY_CRONE_INTRO2'),
('-1532041','How could you? What a cruel, cruel world...','9178','1','0','0','15052','crone SAY_CRONE_DEATH'),
('-1532042','Fixed you, didn''t I?','9180','1','0','0','15051','crone SAY_CRONE_SLAY'),

('-1532043','The better to own you with!','9276','1','0','0','14212','wolf SAY_WOLF_AGGRO'),
('-1532044','Mmmm... delicious.','9277','1','0','0','15153','wolf SAY_WOLF_SLAY'),
('-1532045','Run away little girl, run away!','9278','1','0','0','14213','wolf SAY_WOLF_HOOD'),

('-1532046','What devil art thou, that dost torment me thus?','9196','1','0','0','15070','julianne SAY_JULIANNE_AGGRO'),
('-1532047','Where is my lord? Where is my Romulo?','9199','1','0','0','0','julianne SAY_JULIANNE_ENTER'),
('-1532048','Romulo, I come! Oh... this do I drink to thee!','9198','1','0','0','15072','julianne SAY_JULIANNE_DEATH01'),
('-1532049','O happy dagger! This is thy sheath; there rust, and let me die!','9310','1','0','0','15074','julianne SAY_JULIANNE_DEATH02'),
('-1532050','Come, gentle night; and give me back my Romulo!','9200','1','0','0','15073','julianne SAY_JULIANNE_RESURRECT'),
('-1532051','Parting is such sweet sorrow.','9201','1','0','0','15071','julianne SAY_JULIANNE_SLAY'),

('-1532052','Wilt thou provoke me? Then have at thee, boy!','9233','1','0','0','15075','romulo SAY_ROMULO_AGGRO'),
('-1532053','Thou smilest... upon the stroke that... murders me.','9235','1','0','0','15078','romulo SAY_ROMULO_DEATH'),
('-1532054','This day''s black fate on more days doth depend. This but begins the woe. Others must end.','9236','1','0','0','0','romulo SAY_ROMULO_ENTER'),
('-1532055','Thou detestable maw, thou womb of death; I enforce thy rotten jaws to open!','9237','1','0','0','15077','romulo SAY_ROMULO_RESURRECT'),
('-1532056','How well my comfort is revived by this!','9238','1','0','0','15076','romulo SAY_ROMULO_SLAY'),

('-1532057','The Menagerie is for guests only.','9183','1','0','0','15055','curator SAY_AGGRO'),
('-1532058','Gallery rules will be strictly enforced.','9188','1','0','0','15337','curator SAY_SUMMON1'),
('-1532059','This Curator is equipped for gallery protection.','9309','1','0','0','15338','curator SAY_SUMMON2'),
('-1532060','Your request cannot be processed.','9186','1','0','0','15059','curator SAY_EVOCATE'),
('-1532061','Failure to comply will result in offensive action.','9185','1','0','0','15060','curator SAY_ENRAGE'),
('-1532062','Do not touch the displays.','9187','1','0','0','15335','curator SAY_KILL1'),
('-1532063','You are not a guest.','9308','1','0','0','15336','curator SAY_KILL2'),
('-1532064','Curator is no longer operation-a-l.','9184','1','0','0','15057','curator SAY_DEATH'),

('-1532065','Your blood will anoint my circle.','9264','1','0','0','15145','terestian SAY_SLAY1'),
('-1532066','The great one will be pleased.','9329','1','0','0','15146','terestian SAY_SLAY2'),
('-1532067','My life is yours, oh great one...','9262','1','0','0','15137','terestian SAY_DEATH'),
('-1532068','Ah, you''re just in time. The rituals are about to begin!','9260','1','0','0','15141','terestian SAY_AGGRO'),
('-1532069','Please accept this humble offering, oh great one...','9263','1','0','0','15147','terestian SAY_SACRIFICE1'),
('-1532070','Let the sacrifice serve his testament to my fealty.','9330','1','0','0','0','terestian SAY_SACRIFICE2'),
('-1532071','Come, you dwellers in the dark! Rally to my call!','9265','1','0','0','15150','terestian SAY_SUMMON1'),
('-1532072','Gather, my pets. There is plenty for all.','9331','1','0','0','15151','terestian SAY_SUMMON2'),

('-1532073','Please, no more. My son... he''s gone mad!','9241','1','0','0','13529','aran SAY_AGGRO1'),
('-1532074','I''ll not be tortured again!','9323','1','0','0','15353','aran SAY_AGGRO2'),
('-1532075','Who are you? What do you want? Stay away from me!','9324','1','0','0','13530','aran SAY_AGGRO3'),
('-1532076','I''ll show you: this beaten dog still has some teeth!','9245','1','0','0','13519','aran SAY_FLAMEWREATH1'),
('-1532077','Burn you hellish fiends!','9326','1','0','0','13520','aran SAY_FLAMEWREATH2'),
('-1532078','I''ll freeze you all!','9246','1','0','0','13522','aran SAY_BLIZZARD1'),
('-1532079','Back to the cold dark with you!','9327','1','0','0','22914','aran SAY_BLIZZARD2'),
('-1532080','Yes, yes my son is quite powerful... but I have powers of my own!','9242','1','0','0','13517','aran SAY_EXPLOSION1'),
('-1532081','I am not some simple jester! I am Nielas Aran!','9325','1','0','0','13518','aran SAY_EXPLOSION2'),
('-1532082','Surely you wouldn''t deny an old man a replenishing drink? No, no, I thought not.','9248','1','0','0','13514','aran SAY_DRINK'),
('-1532083','I''m not finished yet! No, I have a few more tricks up my sleeve...','9251','1','0','0','13513','aran SAY_ELEMENTALS'),
('-1532084','I want this nightmare to be over!','9250','1','0','0','13526','aran SAY_KILL1'),
('-1532085','Torment me no more!','9328','1','0','0','13527','aran SAY_KILL2'),
('-1532086','You''ve wasted enough of my time. Let these games be finished!','9247','1','0','0','13569','aran SAY_TIMEOVER'),
('-1532087','At last the nightmare is over...','9244','1','0','0','15345','aran SAY_DEATH'),
('-1532088','Where did you get that?! Did HE send you?!','9249','1','0','0','13532','aran SAY_ATIESH'),

('-1532089','%s cries out in withdrawal, opening gates to the nether.','0','3','0','0','19880','netherspite EMOTE_PHASE_PORTAL'),
('-1532090','%s goes into a nether-fed rage!','0','3','0','0','19877','netherspite EMOTE_PHASE_BANISH'),

('-1532091','Madness has brought you here to me. I shall be your undoing!','9218','1','0','0','15127','malchezaar SAY_AGGRO'),
('-1532092','Simple fools! Time is the fire in which you''ll burn!','9220','1','0','0','15341','malchezaar SAY_AXE_TOSS1'),
('-1532093','I see the subtlety of conception is beyond primitives such as you.','9317','1','0','0','0','malchezaar SAY_AXE_TOSS2'),
('-1532094','Who knows what secrets hide in the dark.','9223','1','0','0','0','malchezaar SAY_SPECIAL1'),
('-1532095','The cerestial forces are mine to manipulate.','9320','1','0','0','0','malchezaar SAY_SPECIAL2'),
('-1532096','How can you hope to stand against such overwhelming power?','9321','1','0','0','15342','malchezaar SAY_SPECIAL3'),
('-1532097','Surely you did not think you could win.','9222','1','0','0','15124','malchezaar SAY_SLAY1'),
('-1532098','Your greed, your foolishness has brought you to this end.','9318','1','0','0','15125','malchezaar SAY_SLAY2'),
('-1532099','You are, but a plaything, unfit even to amuse.','9319','1','0','0','15126','malchezaar SAY_SLAY3'),
('-1532100','All realities, all dimensions are open to me!','9224','1','0','0','15343','malchezaar SAY_SUMMON1'),
('-1532101','You face not Malchezaar alone, but the legions I command!','9322','1','0','0','15344','malchezaar SAY_SUMMON2'),
('-1532102','I refuse to concede defeat! I am a prince of the Eredar! I... am...','9221','1','0','0','15123','malchezaar SAY_DEATH'),

('-1532103','Welcome ladies and gentlemen, to this evening''s presentation!','9174','1','0','0','14118','barnes OZ1'),
('-1532104','Tonight we plumb the depths of the human soul as we join a lost, lonely girl trying desperately--with the help of her loyal companions--to find her way home!','9338','1','0','0','15362','barnes OZ2'),
('-1532105','But she is pursued... by a wicked, malevolent crone!','9339','1','0','0','15363','barnes OZ3'),
('-1532106','Will she survive? Will she prevail? Only time will tell. And now... on with the show!','9340','1','0','0','15364','barnes OZ4'),
('-1532107','Good evening, Ladies and Gentlemen! Welcome to this evening''s presentation!','9175','1','0','0','14057','barnes HOOD1'),
('-1532108','Tonight, things are not what they seem. For tonight, your eyes may not be trusted','9335','1','0','0','15359','barnes HOOD2'),
('-1532109','Take for instance, this quiet, elderly woman, waiting for a visit from her granddaughter. Surely there is nothing to fear from this sweet, grey-haired, old lady.','9336','1','0','0','15360','barnes HOOD3'),
('-1532110','But don''t let me pull the wool over your eyes. See for yourself what lies beneath those covers! And now... on with the show!','9337','1','0','0','15361','barnes HOOD4'),
('-1532111','Welcome, Ladies and Gentlemen, to this evening''s presentation!','9176','1','0','0','14122','barnes RAJ1'),
('-1532112','Tonight... we explore a tale of forbidden love!','9341','1','0','0','15365','barnes RAJ2'),
('-1532113','But beware, for not all love stories end happily, as you may find out. Sometimes, love pricks like a thorn!','9342','1','0','0','15366','barnes RAJ3'),
('-1532114','But don''t take it from me; see for yourself what tragedy lies ahead when the paths of star crossed lovers meet! And now... on with the show!','9343','1','0','0','15367','barnes RAJ4'),
('-1532115','Splendid. I''m going to get the audience ready. Break a leg!','0','0','0','0','14056','barnes SAY_EVENT_START'),

('-1532116','You''ve got my attention, dragon. You''ll find I''m not as easily scared as the villagers below.','0','1','0','0','14295','image of medivh SAY_MEDIVH_1'),
('-1532117','Your dabbling in the arcane has gone too far, Medivh. You''ve attracted the attention of powers beyond your understanding. You must leave Karazhan at once!','0','1','0','0','14296','arcanagos SAY_ARCANAGOS_2'),
('-1532118','You dare challenge me at my own dwelling? Your arrogance is astounding, even for a dragon.','0','1','0','0','14297','image of medivh SAY_MEDIVH_3'),
('-1532119','A dark power seeks to use you, Medivh! If you stay, dire days will follow. You must hurry, we don''t have much time!','0','1','0','0','14298','arcanagos SAY_ARCANAGOS_4'),
('-1532120','I do not know what you speak of, dragon... but I will not be bullied by this display of insolence. I''ll leave Karazhan when it suits me!','0','1','0','0','14299','image of medivh SAY_MEDIVH_5'),
('-1532121','You leave me no alternative. I will stop you by force if you wont listen to reason.','0','1','0','0','14300','arcanagos SAY_ARCANAGOS_6'),
('-1532122','%s begins to cast a spell of great power, weaving his own essence into the magic.','0','2','0','0','14308','image of medivh EMOTE_CAST_SPELL'),
('-1532123','What have you done, wizard? This cannot be! I''m burning from... within!','0','1','0','0','14294','arcanagos SAY_ARCANAGOS_7'),
('-1532124','He should not have angered me. I must go... recover my strength now...','0','0','0','0','14316','image of medivh SAY_MEDIVH_8'),

('-1532125','An ancient being awakens in the distance...','0','2','0','0','13647','nightbane EMOTE_AWAKEN'),
('-1532126','What fools! I shall bring a quick end to your suffering!','0','1','0','0','13649','nightbane SAY_AGGRO'),
('-1532127','Miserable vermin. I shall exterminate you from the air!','0','1','0','0','20023','nightbane SAY_AIR_PHASE'),
('-1532128','Enough! I shall land and crush you myself!','0','1','0','0','13676','nightbane SAY_LAND_PHASE_1'),
('-1532129','Insects! Let me show you my strength up close!','0','1','0','0','13677','nightbane SAY_LAND_PHASE_2'),
('-1532130','%s takes a deep breath.','0','3','0','0','20021','nightbane EMOTE_DEEP_BREATH'),

('-1532131','The halls of Karazhan shake, as the curse binding the doors of the Gamesman''s Hall is lifted.','0','2','0','0','20430','echo_of_medivh EMOTE_LIFT_CURSE'),
('-1532132','Echo of Medivh cheats!','0','3','0','0','21910','echo_of_medivh EMOTE_CHEAT'),

('-1532133','%s flaps down from the ceiling...','0','7','0','0','12322','shadikith_the_glider EMOTE_BAT_SPAWN'),
('-1532134','%s howls through the halls...','0','7','0','0','12323','rokad_the_ravager EMOTE_DOG_SPAWN'),
('-1532135','%s emerges from the shadows...','0','7','0','0','22783','hyakiss_the_lurker EMOTE_SPIDER_SPAWN'),
('-1532136','%s shrieks in pain and points at his master.','0','3','0','0','13582','Kil''Rek EMOTE_DEATH'),
('-1532137','%s calls for her master!','0','3','0','0','13439','Midnight EMOTE_CALL'),
('-1532138','%s skitters out from the shadows...','0','7','0','0','12324','hyakiss_the_lurker EMOTE_SPIDER_SPAWN2'),
('-1532139','%s begins channelling his mana into a powerful arcane spell.','0','3','0','0','13515','aran EMOTE_ARCANE_SPELL');

-- -1 533 000 NAXXRAMAS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1533000','Ahh... welcome to my parlor.','8788','1','0','0','13004','anubrekhan SAY_GREET'),
('-1533001','Just a little taste...','8785','1','0','0','13000','anubrekhan SAY_AGGRO1'),
('-1533002','There is no way out.','8786','1','0','0','13002','anubrekhan SAY_AGGRO2'),
('-1533003','Yes, Run! It makes the blood pump faster!','8787','1','0','0','13003','anubrekhan SAY_AGGRO3'),
('-1533004','I hear little hearts beating. Yesss... beating faster now. Soon the beating will stop.','8790','1','0','0','13006','anubrekhan SAY_TAUNT1'),
('-1533005','Where to go? What to do? So many choices that all end in pain, end in death.','8791','1','0','0','13007','anubrekhan SAY_TAUNT2'),
('-1533006','Which one shall I eat first? So difficult to choose... the all smell so delicious.','8792','1','0','0','13008','anubrekhan SAY_TAUNT3'),
('-1533007','Closer now... tasty morsels. I''ve been too long without food. Without blood to drink.','8793','1','0','0','13009','anubrekhan SAY_TAUNT4'),
('-1533008','Shh... it will all be over soon.','8789','1','0','0','13005','anubrekhan SAY_SLAY'),

('-1533009','Your old lives, your mortal desires, mean nothing. You are acolytes of the master now, and you will serve the cause without question! The greatest glory is to die in the master''s service!','8799','1','0','0','12852','faerlina SAY_GREET'),
('-1533010','Slay them in the master''s name!','8794','1','0','0','12856','faerlina SAY_AGGRO1'),
('-1533011','You cannot hide from me!','8795','1','0','0','12857','faerlina SAY_AGGRO2'),
('-1533012','Kneel before me, worm!','8796','1','0','0','12858','faerlina SAY_AGGRO3'),
('-1533013','Run while you still can!','8797','1','0','0','61582','faerlina SAY_AGGRO4'),
('-1533014','You have failed!','8800','1','0','0','12854','faerlina SAY_SLAY1'),
('-1533015','Pathetic wretch!','8801','1','0','0','12855','faerlina SAY_SLAY2'),
('-1533016','The master... will avenge me!','8798','1','0','0','12853','faerlina SAY_DEATH'),

('-1533017','Patchwerk want to play!','8909','1','0','0','13068','patchwerk SAY_AGGRO1'),
('-1533018','Kel''Thuzad make Patchwerk his Avatar of War!','8910','1','0','0','13069','patchwerk SAY_AGGRO2'),
('-1533019','No more play?','8912','1','0','0','13071','patchwerk SAY_SLAY'),
('-1533020','What happened to... Patch...','8911','1','0','0','0','patchwerk SAY_DEATH'),

('-1533021','%s sprays slime across the room!','0','3','0','0','32318','grobbulus EMOTE_SPRAY_SLIME'),

('-1533022','%s lifts off into the air!','0','3','0','0','0','sapphiron EMOTE_FLY'),

('-1533023','Stalagg crush you!','8864','1','0','0','13083','stalagg SAY_STAL_AGGRO'),
('-1533024','Stalagg kill!','8866','1','0','0','13085','stalagg SAY_STAL_SLAY'),
('-1533025','Master save me...','8865','1','0','0','13084','stalagg SAY_STAL_DEATH'),
('-1533026','Feed you to master!','8802','1','0','0','13023','feugen SAY_FEUG_AGGRO'),
('-1533027','Feugen make master happy!','8804','1','0','0','13025','feugen SAY_FEUG_SLAY'),
('-1533028','No... more... Feugen...','8803','1','0','0','13024','feugen SAY_FEUG_DEATH'),

('-1533029','You are too late... I... must... OBEY!','8872','1','0','0','13091','thaddius SAY_GREET'),
('-1533030','KILL!','8867','1','0','0','11110','thaddius SAY_AGGRO1'),
('-1533031','EAT YOUR BONES!','8868','1','0','0','13087','thaddius SAY_AGGRO2'),
('-1533032','BREAK YOU!','8869','1','0','0','29559','thaddius SAY_AGGRO3'),
('-1533033','You die now!','8877','1','0','0','71545','thaddius SAY_SLAY'),
('-1533034','Now YOU feel pain!','8871','1','0','0','13090','thaddius SAY_ELECT'),
('-1533035','Thank... you...','8870','1','0','0','13089','thaddius SAY_DEATH'),
('-1533036','REUSE_ME','0','0','0','0','0','REUSE'),
('-1533037','REUSE_ME','0','0','0','0','0','REUSE'),
('-1533038','REUSE_ME','0','0','0','0','0','REUSE'),
('-1533039','REUSE_ME','0','0','0','0','0','REUSE'),

('-1533040','Foolishly you have sought your own demise.','8807','1','0','0','13030','gothik SAY_SPEECH_1'),
('-1533041','Death is the only escape.','8806','1','0','0','29039','gothik SAY_KILL'),
('-1533042','I... am... undone!','8805','1','0','0','13026','gothik SAY_DEATH'),
('-1533043','I have waited long enough! Now, you face the harvester of souls!','8808','1','0','0','13028','gothik SAY_TELEPORT'),

('-1533044','Defend yourself!','8892','0','0','0','13010','blaumeux SAY_BLAU_AGGRO'),
('-1533045','Come, Zeliek, do not drive them out. Not before we''ve had our fun.','8896','6','0','0','13014','blaumeux SAY_BLAU_TAUNT1'),
('-1533046','I do hope they stay alive long enough for me to... introduce myself.','8897','6','0','0','13015','blaumeux SAY_BLAU_TAUNT2'),
('-1533047','The first kill goes to me! Anyone care to wager?','8898','6','0','0','13016','blaumeux SAY_BLAU_TAUNT3'),
('-1533048','Your life is mine!','8895','1','0','0','13013','blaumeux SAY_BLAU_SPECIAL'),
('-1533049','Who''s next?','8894','0','0','0','21654','blaumeux SAY_BLAU_SLAY'),
('-1533050','Tou... che!','8893','0','0','0','0','blaumeux SAY_BLAU_DEATH'),

('-1533051','Come out and fight, ye wee ninny!','8899','0','0','0','0','korthazz SAY_KORT_AGGRO'),
('-1533052','To arms, ye roustabouts! We''ve got company!','8903','6','0','0','13038','korthazz SAY_KORT_TAUNT1'),
('-1533053','I heard about enough of yer sniveling. Shut yer fly trap ''afore I shut it for ye!','8904','6','0','0','13039','korthazz SAY_KORT_TAUNT2'),
('-1533054','I''m gonna enjoy killin'' these slack-jawed daffodils!','8905','6','0','0','13040','korthazz SAY_KORT_TAUNT3'),
('-1533055','I like my meat extra crispy!','8902','0','0','0','13037','korthazz SAY_KORT_SPECIAL'),
('-1533056','Next time, bring more friends!','8901','0','0','0','13036','korthazz SAY_KORT_SLAY'),
('-1533057','What a bloody waste this is!','8900','0','0','0','13035','korthazz SAY_KORT_DEATH'),

('-1533058','Flee, before it''s too late!','8913','0','0','0','13097','zeliek SAY_ZELI_AGGRO'),
('-1533059','Invaders, cease this foolish venture at once! Turn away while you still can!','8917','6','0','0','13101','zeliek SAY_ZELI_TAUNT1'),
('-1533060','Perhaps they will come to their senses, and run away as fast as they can!','8918','6','0','0','13102','zeliek SAY_ZELI_TAUNT2'),
('-1533061','Do not continue! Turn back while there''s still time!','8919','6','0','0','13103','zeliek SAY_ZELI_TAUNT3'),
('-1533062','I- I have no choice but to obey!','8916','0','0','0','13100','zeliek SAY_ZELI_SPECIAL'),
('-1533063','Forgive me!','8915','0','0','0','63280','zeliek SAY_ZELI_SLAY'),
('-1533064','It is... as it should be.','8914','0','0','0','13098','zeliek SAY_ZELI_DEATH'),

('-1533065','You seek death?','14571','0','0','0','13051','rivendare_naxx SAY_RIVE_AGGRO1'),
('-1533066','None shall pass!','14572','0','0','0','13052','rivendare_naxx SAY_RIVE_AGGRO2'),
('-1533067','Be still!','14573','0','0','0','13053','rivendare_naxx SAY_RIVE_AGGRO3'),
('-1533068','You will find no peace in death.','14574','0','0','0','13055','rivendare_naxx SAY_RIVE_SLAY1'),
('-1533069','The master''s will is done.','14575','0','0','0','13056','rivendare_naxx SAY_RIVE_SLAY2'),
('-1533070','Bow to the might of the scourge!','14576','0','0','0','0','rivendare_naxx SAY_RIVE_SPECIAL'),
('-1533071','Enough prattling. Let them come! We shall grind their bones to dust.','14577','6','0','0','13058','rivendare_naxx SAY_RIVE_TAUNT1'),
('-1533072','Conserve your anger! Harness your rage! You will all have outlets for your frustration soon enough.','14578','6','0','0','13059','rivendare_naxx SAY_RIVE_TAUNT2'),
('-1533073','Life is meaningless. It is in death that we are truly tested.','14579','6','0','0','13060','rivendare_naxx SAY_RIVE_TAUNT3'),
('-1533074','Death... will not stop me...','14580','0','0','0','33116','rivendare_naxx SAY_RIVE_DEATH'),

('-1533075','Glory to the master!','8845','1','0','0','13061','noth SAY_AGGRO1'),
('-1533076','Your life is forfeit!','8846','1','0','0','13062','noth SAY_AGGRO2'),
('-1533077','Die, trespasser!','8847','1','0','0','13063','noth SAY_AGGRO3'),
('-1533078','Rise, my soldiers! Rise and fight once more!','8851','1','0','0','13067','noth SAY_SUMMON'),
('-1533079','My task is done!','8849','1','0','0','13065','noth SAY_SLAY1'),
('-1533080','Breathe no more!','8850','1','0','0','13066','noth SAY_SLAY2'),
('-1533081','I will serve the master... in... death!','8848','1','0','0','13064','noth SAY_DEATH'),

('-1533082','%s takes in a deep breath...','0','3','0','0','7213','sapphiron EMOTE_BREATH'),
('-1533083','%s resumes his attacks!','0','3','0','0','32802','sapphiron EMOTE_GROUND'),

('-1533084','Your forces are nearly marshalled to strike back against your enemies, my liege.','14467','6','0','0','0','kelthuzad SAY_SAPP_DIALOG1'),
('-1533085','Soon we will eradicate the Alliance and Horde, then the rest of Azeroth will fall before the might of my army.','14768','6','0','0','0','lich_king SAY_SAPP_DIALOG2_LICH'),
('-1533086','Yes, Master. The time of their ultimate demise grows close...What is this?','14468','6','0','0','0','kelthuzad SAY_SAPP_DIALOG3'),
('-1533087','Invaders...here?! DESTROY them, Kel''Thuzad! Naxxramas must not fall!','14769','6','0','0','0','lich_king SAY_SAPP_DIALOG4_LICH'),
('-1533088','As you command, Master!','14469','6','0','0','0','kelthuzad SAY_SAPP_DIALOG5'),
('-1533089','No!!! A curse upon you, interlopers! The armies of the Lich King will hunt you down. You will not escape your fate...','14484','6','0','0','13150','kelthuzad SAY_CAT_DIED'),
('-1533090','Who dares violate the sanctity of my domain? Be warned, all who trespass here are doomed.','14463','6','0','0','0','kelthuzad SAY_TAUNT1'),
('-1533091','Fools, you think yourselves triumphant? You have only taken one step closer to the abyss! ','14464','6','0','0','12985','kelthuzad SAY_TAUNT2'),
('-1533092','I grow tired of these games. Proceed, and I will banish your souls to oblivion!','14465','6','0','0','12986','kelthuzad SAY_TAUNT3'),
('-1533093','You have no idea what horrors lie ahead. You have seen nothing! The frozen heart of Naxxramas awaits you!','14466','6','0','0','12987','kelthuzad SAY_TAUNT4'),
('-1533094','Pray for mercy!','14475','1','0','0','12995','kelthuzad SAY_AGGRO1'),
('-1533095','Scream your dying breath!','14476','1','0','0','12996','kelthuzad SAY_AGGRO2'),
('-1533096','The end is upon you!','14477','1','0','0','12997','kelthuzad SAY_AGGRO3'),
('-1533097','The dark void awaits you!','14478','1','0','0','13021','kelthuzad SAY_SLAY1'),
('-1533098','<Kel''Thuzad cackles maniacally!>','14479','1','0','0','0','kelthuzad SAY_SLAY2'),
('-1533099','AAAAGHHH!... Do not rejoice... your victory is a hollow one... for I shall return with powers beyond your imagining!','14480','1','0','0','13019','kelthuzad SAY_DEATH'),
('-1533100','Your soul, is bound to me now!','14472','1','0','0','13017','kelthuzad SAY_CHAIN1'),
('-1533101','There will be no escape!','14473','1','0','0','13018','kelthuzad SAY_CHAIN2'),
('-1533102','I will freeze the blood in your veins!','14474','1','0','0','13020','kelthuzad SAY_FROST_BLAST'),
('-1533103','Master! I require aid! ','14470','1','0','0','12998','kelthuzad SAY_REQUEST_AID'),
('-1533104','Very well... warriors of the frozen wastes, rise up! I command you to fight, kill, and die for your master. Let none survive...','0','1','0','0','12994','kelthuzad SAY_ANSWER_REQUEST'),
('-1533105','Minions, servants, soldiers of the cold dark, obey the call of Kel''Thuzad!','14471','1','0','0','12999','kelthuzad SAY_SUMMON_MINIONS'),
('-1533106','Your petty magics are no challenge to the might of the Scourge! ','14481','1','0','0','13492','kelthuzad SAY_SPECIAL1_MANA_DET'),
('-1533107','Enough! I grow tired of these distractions! ','14483','1','0','0','0','kelthuzad SAY_SPECIAL3_MANA_DET'),
('-1533108','Fools, you have spread your powers too thin. Be free, my minions!','14482','1','0','0','0','kelthuzad SAY_SPECIAL2_DISPELL'),

('-1533109','You are mine now!','8825','1','0','0','18258','heigan SAY_AGGRO1'),
('-1533110','I see you!','8826','1','0','0','73351','heigan SAY_AGGRO2'),
('-1533111','You...are next!','8827','1','0','0','13043','heigan SAY_AGGRO3'),
('-1533112','Close your eyes... sleep!','8829','1','0','0','13045','heigan SAY_SLAY'),
('-1533113','The races of the world will perish. It is only a matter of time.','8830','1','0','0','13046','heigan SAY_TAUNT1'),
('-1533114','I see endless suffering, I see torment, I see rage. I see... everything!','8831','1','0','0','13047','heigan SAY_TAUNT2'),
('-1533115','Soon... the world will tremble!','8832','1','0','0','13048','heigan SAY_TAUNT3'),
('-1533116','The end is upon you.','8833','1','0','0','13049','heigan SAY_CHANNELING'),
('-1533117','Hungry worms will feast on your rotten flesh!','8834','1','0','0','13050','heigan SAY_TAUNT4'),
('-1533118','Noo... o...','8828','1','0','0','0','heigan SAY_DEATH'),

('-1533119','%s spots a nearby Zombie to devour!','0','3','0','0','0','gluth EMOTE_ZOMBIE'),

('-1533120','Hah hah, I''m just getting warmed up!','8852','1','0','0','13072','razuvious SAY_AGGRO1'),
('-1533121','Stand and fight!','8853','1','0','0','13073','razuvious SAY_AGGRO2'),
('-1533122','Show me what you''ve got!','8854','1','0','0','13074','razuvious SAY_AGGRO3'),
('-1533123','Sweep the leg! Do you have a problem with that?','8861','1','0','0','13080','razuvious SAY_SLAY1'),
('-1533124','You should have stayed home!','8862','1','0','0','13081','razuvious SAY_SLAY2'),
('-1533125','Do as I taught you!','8855','1','0','0','13075','razuvious SAY_COMMAND1'),
('-1533126','Show them no mercy!','8856','1','0','0','13076','razuvious SAY_COMMAND2'),
('-1533127','You disappoint me, students!','8858','1','0','0','13077','razuvious SAY_COMMAND3'),
('-1533128','The time for practice is over! Show me what you''ve learned!','8859','1','0','0','13078','razuvious SAY_COMMAND4'),
('-1533129','An honorable... death...','8860','1','0','0','13079','razuvious SAY_DEATH'),

('-1533130','%s summons forth Skeletal Warriors!','0','3','0','0','32974','noth EMOTE_WARRIOR'),
('-1533131','%s raises more skeletons!','0','3','0','0','32977','noth EMOTE_SKELETON'),
('-1533132','%s teleports to the balcony above!','0','3','0','0','32331','noth EMOTE_TELEPORT'),
('-1533133','%s teleports back into the battle!','0','3','0','0','32976','noth EMOTE_TELEPORT_RETURN'),

('-1533134','A Guardian of Icecrown enters the fight!','0','3','0','0','32804','kelthuzad EMOTE_GUARDIAN'),
('-1533135','%s strikes!','0','3','0','0','0','kelthuzad EMOTE_PHASE2'),

('-1533136','%s teleports and begins to channel a spell!','0','3','0','0','32332','heigan EMOTE_TELEPORT'),
('-1533137','%s rushes to attack once more!','0','3','0','0','32333','heigan EMOTE_RETURN'),

('-1533138','%s teleports into the fray!','0','3','0','0','32306','gothik EMOTE_TO_FRAY'),
('-1533139','The central gate opens!','0','3','0','0','32307','gothik EMOTE_GATE'),
('-1533140','Brazenly you have disregarded powers beyond your understanding.','0','1','0','0','13031','gothik SAY_SPEECH_2'),
('-1533141','You have fought hard to invade the realm of the harvester.','0','1','0','0','13032','gothik SAY_SPEECH_3'),
('-1533142','Now there is only one way out - to walk the lonely path of the damned.','0','1','0','0','13033','gothik SAY_SPEECH_4'),

('-1533143','An aura of necrotic energy blocks all healing!','0','3','0','0','32334','Loatheb EMOTE_AURA_BLOCKING'),
('-1533144','The power of Necrotic Aura begins to wane!','0','3','0','0','0','Loatheb EMOTE_AURA_WANE'),
('-1533145','The aura fades away, allowing healing once more!','0','3','0','0','32335','Loatheb EMOTE_AURA_FADING'),

('-1533146','%s spins her web into a cocoon!','0','3','0','0','32303','maexxna EMOTE_SPIN_WEB'),
('-1533147','Spiderlings appear on the web!','0','3','0','0','32305','maexxna EMOTE_SPIDERLING'),
('-1533148','%s sprays strands of web everywhere!','0','3','0','0','32304','maexxna EMOTE_SPRAY'),

('-1533149','%s loses its link!','0','3','0','0','12156','tesla_coil EMOTE_LOSING_LINK'),
('-1533150','%s overloads!','0','3','0','0','12178','tesla_coil EMOTE_TESLA_OVERLOAD'),
('-1533151','The polarity has shifted!','0','3','0','0','32324','thaddius EMOTE_POLARITY_SHIFT'),

('-1533152','%s decimates all nearby flesh!','0','3','0','0','32321','gluth EMOTE_DECIMATE'),

('-1533153','A %s joins the fight!','0','3','0','0','29887','crypt_guard EMOTE_CRYPT_GUARD'),
('-1533154','%s begins to unleash an insect swarm!','0','3','0','0','0','anubrekhan EMOTE_INSECT_SWARM'),
('-1533155','Corpse Scarabs appear from a Crypt Guard''s corpse!','0','3','0','0','32796','anubrekhan EMOTE_CORPSE_SCARABS'),

('-1533156','%s casts Unyielding Pain on everyone!','0','3','0','0','33087','lady_blaumeux EMOTE_UNYIELDING_PAIN'),
('-1533157','%s casts Condemation on everyone!','0','3','0','0','0','sir_zeliek EMOTE_CONDEMATION'),

('-1533158','%s injects you with a mutagen!','0','5','0','0','32319','grobbulus EMOTE_INJECTION');

-- -1 534 000 THE BATTLE OF MT. HYJAL
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1534000','I''m in jeopardy! Help me if you can!','11007','6','0','0','18537','jaina hyjal ATTACKED 1'),
('-1534001','They''ve broken through!','11049','6','0','0','18538','jaina hyjal ATTACKED 2'),
-- (-1534002,'Stay alert! Another wave approaches.',11008,6,0,0,'jaina hyjal INCOMING'), -- Text based on sound file (Unknown if this was ever really used)
('-1534003','Don''t give up! We must prevail!','11006','6','0','0','18540','jaina hyjal RALLY 1'),
('-1534004','Hold them back as long as possible!','11050','6','0','0','18541','jaina hyjal RALLY 2'),
('-1534005','We must hold strong!','11051','6','0','0','18542','jaina hyjal RALLY 3'),
('-1534006','We are lost! Fall back!','11009','6','0','0','18543','jaina hyjal RETREAT'),
('-1534007','We have won valuable time. Now we must pull back.','11011','6','0','0','15309','jaina hyjal WIN'),
('-1534008','I did... my best.','11010','6','0','0','18539','jaina hyjal DEATH'),

('-1534009','I will lay down for no one!','11031','6','0','0','18544','thrall hyjal ATTACKED 1'),
('-1534010','Bring the fight to me and pay with your lives!','11061','6','0','0','18545','thrall hyjal ATTACKED 2'),
-- (-1534011,'Make ready for another wave! LOK-TAR OGAR!',11032,6,0,0,'thrall hyjal INCOMING'), -- Text based on sound file (Unknown if this was ever really used)
('-1534012','Hold them back! Do not falter!','11030','6','0','0','18731','thrall hyjal RALLY 1'),
('-1534013','Victory or death!','11059','6','0','0','15184','thrall hyjal RALLY 2'),
('-1534014','Do not give an inch of ground!','11060','6','0','0','18733','thrall hyjal RALLY 3'),
('-1534015','It is over! Withdraw! We have failed....','11033','6','0','0','18734','thrall hyjal RETREAT'),
('-1534016','We have played our part, and done well at that. It is up to the others now.','11035','6','0','0','15312','thrall hyjal WIN'),
-- (-1534017,'Uraaa...',11034,6,0,0,'thrall hyjal DEATH'), -- Text based on sound file (Most likely only sound should be used)

('-1534018','All of your efforts have been in vain, for the draining of the World Tree has already begun!  Soon the heart of your world will beat no more!','10986','6','0','0','20432','archimonde SAY_PRE_EVENTS_COMPLETE'),
('-1534019','Your resistance is insignificant.','10987','6','0','0','18257','archimonde SAY_AGGRO'),
('-1534020','This world will burn!','10990','6','0','0','18262','archimonde SAY_DOOMFIRE1'),
('-1534021','Manach sheek-thrish!','11041','6','0','0','18263','archimonde SAY_DOOMFIRE2'),
('-1534022','A-kreesh!','10989','6','0','0','17636','archimonde SAY_AIR_BURST1'),
('-1534023','Away vermin!','11043','6','0','0','18261','archimonde SAY_AIR_BURST2'),
('-1534024','All creation will be devoured!','11044','6','0','0','18265','archimonde SAY_SLAY1'),
('-1534025','Your soul will languish for eternity.','10991','6','0','0','18264','archimonde SAY_SLAY2'),
('-1534026','I am the coming of the end!','11045','6','0','0','18266','archimonde SAY_SLAY3'),
('-1534027','At last it is here. Mourn and lament the passing of all you have ever known and all that would have been! Akmin-kurai!','10993','6','0','0','14990','archimonde SAY_ENRAGE'),
('-1534028','No, it cannot be! Nooo!','10992','6','0','0','14991','archimonde SAY_DEATH'),
('-1534029','You are mine now.','10988','6','0','0','13041','archimonde SAY_SOUL_CHARGE1'),
('-1534030','Bow to my will.','11042','6','0','0','18259','archimonde SAY_SOUL_CHARGE2'),

('-1534040','The Legion''s final conquest has begun! Once again the subjugation of this world is within our grasp. Let none survive!','11022','6','0','0','18221','RAGE_WINTERCHILL_ENTER'),
('-1534041','Succumb to the icy chill... of death.','11024','6','0','0','18224','RAGE_WINTERCHILL_FROST_NOVA1'),
('-1534042','It will be much colder in your grave.','11058','6','0','0','18225','RAGE_WINTERCHILL_FROST_NOVA2'),
('-1534043','Crumble and rot!','11023','6','0','0','18222','RAGE_WINTERCHILL_DND1'),
('-1534044','Ashes to ashes, dust to dust!','11055','6','0','0','18223','RAGE_WINTERCHILL_DND2'),
('-1534045','All life must perish!','11025','6','0','0','18226','RAGE_WINTERCHILL_KILL1'),
('-1534046','Victory to the Legion!','11057','6','0','0','18228','RAGE_WINTERCHILL_KILL2'),
('-1534047','Your world is ours now.','11056','6','0','0','18227','RAGE_WINTERCHILL_KILL3'),
('-1534048','You have won this battle, but not... the... war.','11026','6','0','0','18220','RAGE_WINTERCHILL_DEATH'),

('-1534049','You are defenders of a doomed world! Flee here, and perhaps you will prolong your pathetic lives!','10977','6','0','0','18230','ANETHERON_ENTER'),
('-1534050','Your hopes are lost!','10981','6','0','0','18237','ANETHERON_KILL1'),
('-1534051','Scream for me!','11038','6','0','0','18238','ANETHERON_KILL2'),
('-1534052','Pity, no time for a slow death!','11039','6','0','0','18239','ANETHERON_KILL3'),
('-1534053','The clock... is still... ticking.','10982','6','0','0','18229','ANETHERON_DEATH'),
('-1534054','The swarm is eager to feed.','10979','6','0','0','18233','ANETHERON_CARRION_SWARM1'),
('-1534055','Pestilence upon you!','11037','6','0','0','18234','ANETHERON_CARRION_SWARM2'),
('-1534056','You look tired....','10978','6','0','0','18231','ANETHERON_SLEEP1'),
('-1534057','Sweet dreams.','11545','6','0','0','18232','ANETHERON_SLEEP2'),
('-1534058','Let fire rain from above!','10980','6','0','0','18235','ANETHERON_INFERNO1'),
('-1534059','Earth and sky shall burn!','11036','6','0','0','18236','ANETHERON_INFERNO2'),

('-1534060','Cry for mercy! Your meaningless lives will soon be forfeit!','11015','6','0','0','18240','KAZROGAL_ENTER'),
('-1534061','Your death will be a painful one.','11016','6','0','0','18241','KAZROGAL_MARK_OF_KAZROGAL1'),
('-1534062','You... are marked.','11052','6','0','0','18242','KAZROGAL_MARK_OF_KAZROGAL2'),
('-1534063','You... are nothing!','11053','6','0','0','72461','KAZROGAL_KILL1'),
('-1534064','Miserable nuisance!','11054','6','0','0','18245','KAZROGAL_KILL2'),
('-1534065','Shaza-Kiel!','11017','6','0','0','18243','KAZROGAL_KILL3'),

('-1534067','Abandon all hope! The Legion has returned to finish what was begun so many years ago. This time, there will be no escape!','10999','6','0','0','18247','AZGALOR_ENTER'),
('-1534068','Reesh, hokta!','11001','6','0','0','18253','AZGALOR_KILL1'),
('-1534069','Don''t fight it.','11047','6','0','0','18255','AZGALOR_KILL2'),
('-1534070','No one is going to save you.','11048','6','0','0','18254','AZGALOR_KILL3'),
('-1534071','Just a taste... of what awaits you.','11046','6','0','0','18252','AZGALOR_DOOM1'),
('-1534072','Suffer you despicable insect!','11000','6','0','0','18251','AZGALOR_DOOM2'),
('-1534073','Your time is almost... up.','11002','6','0','0','18248','AZGALOR_DEATH');

-- -1 540 000 SHATTERED HALLS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1540000','You wish to fight us all at once? This should be amusing!','10262','1','0','0','15594','nethekurse SAY_AGGRO_1'),
('-1540001','Thank you for saving me the trouble. Now it''s my turn to have some fun!','10270','1','0','0','15589','nethekurse SAY_AGGRO_2'),
('-1540002','Come on, show me a real fight!','10272','1','0','0','15595','nethekurse SAY_AGGRO_3'),
('-1540003','You can have that one, I no longer need him!','10263','1','0','11','15569','nethekurse PEON_ATTACK_1'),
('-1540004','Yes, beat him mercilessly! His skull is as thick as an ogre''s!','10264','1','0','0','15575','nethekurse PEON_ATTACK_2'),
('-1540005','Don''t waste your time on that one, he''s weak!','10265','1','0','11','15573','nethekurse PEON_ATTACK_3'),
('-1540006','You want him? Very well, take him!','10266','1','0','0','15572','nethekurse PEON_ATTACK_4'),
('-1540007','One pitiful wretch down. Go on, take another one.','10267','1','0','0','15579','nethekurse PEON_DIE_1'),
('-1540008','Ah, what a waste... next!','10268','1','0','0','15584','nethekurse PEON_DIE_2'),
('-1540009','I was going to kill him anyway!','10269','1','0','0','15582','nethekurse PEON_DIE_3'),
('-1540010','Beg for your pitiful life!','10259','1','0','0','14130','nethekurse SAY_TAUNT_1'),
('-1540011','Run, coward, run!   ','10260','1','0','11','14132','nethekurse SAY_TAUNT_2'),
('-1540012','Your pain amuses me!','10261','1','0','11','14148','nethekurse SAY_TAUNT_3'),
('-1540013','I had more fun torturing the peons!','10273','1','0','0','16863','nethekurse SAY_SLAY_1'),
('-1540014','I''m already bored!','10271','1','0','0','16864','nethekurse SAY_SAY_2'),
('-1540015','You lose.','10274','1','0','0','16865','nethekurse SAY_SLAY_3'),
('-1540016','Oh, just die!','10275','1','0','0','16866','nethekurse SAY_SLAY_4'),
('-1540017','What... a shame.','10276','1','0','0','16862','nethekurse SAY_DIE'),

('-1540018','Smash!','10306','1','0','0','30471','omrogg GoCombat_1'),
('-1540019','If you nice me let you live.','10308','1','0','0','14048','omrogg GoCombat_2'),
('-1540020','Me hungry!','10309','1','0','0','19842','omrogg GoCombat_3'),
('-1540021','Why don''t you let me do the talking?','10317','1','0','0','14047','omrogg GoCombatDelay_1'),
('-1540022','No, we will NOT let you live!','10318','1','0','0','16916','omrogg GoCombatDelay_2'),
('-1540023','You always hungry. That why we so fat!','10319','1','0','0','0','omrogg GoCombatDelay_3'),
('-1540024','You stay here. Me go kill someone else!','10303','1','0','0','16898','omrogg Threat_1'),
('-1540025','What are you doing!','10315','1','0','0','58049','omrogg Threat_2'),
('-1540026','Me kill someone else...','10302','1','0','0','0','omrogg Threat_3'),
('-1540027','Me not like this one...','10300','1','0','0','14043','omrogg Threat_4'),
('-1540028','That''s not funny!','10314','1','0','0','16899','omrogg ThreatDelay1_1'),
('-1540029','Me get bored...','10305','1','0','0','16902','omrogg ThreatDelay1_2'),
('-1540030','I''m not done yet, idiot!','10313','1','0','0','16896','omrogg ThreatDelay1_3'),
('-1540031','Hey you numbskull!','10312','1','0','0','14044','omrogg ThreatDelay1_4'),
('-1540032','Ha ha ha.','10304','1','0','0','16900','omrogg ThreatDelay2_1'),
('-1540033','Whhy! He almost dead!','10316','1','0','0','0','omrogg ThreatDelay2_2'),
('-1540034','H''ey...','10307','1','0','0','0','omrogg ThreatDelay2_3'),
('-1540035','We kill his friend!','10301','1','0','0','14045','omrogg ThreatDelay2_4'),
('-1540036','This one die easy!','10310','1','0','0','16922','omrogg Killing_1'),
('-1540037','I''m tired. You kill next one!','10320','1','0','0','16921','omrogg Killing_2'),
('-1540038','That''s because I do all the hard work!','10321','1','0','0','16923','omrogg KillingDelay_1'),
('-1540039','This all...your fault!','10311','1','0','0','16924','omrogg YELL_DIE_L'),
('-1540040','I...hate...you...','10322','1','0','0','16925','omrogg YELL_DIE_R'),
('-1540041','%s enrages!','0','2','0','0','0','omrogg EMOTE_ENRAGE'),

('-1540042','Ours is the TRUE Horde! The only Horde!','10323','1','0','0','17614','kargath SAY_AGGRO1'),
('-1540043','I''ll carve the meat from your bones!','10324','1','0','0','17616','kargath SAY_AGGRO2'),
('-1540044','I am called Bladefist for a reason, as you will see!','10325','1','0','0','17617','kargath SAY_AGGRO3'),
('-1540045','For the real Horde!','10326','1','0','0','17619','kargath SAY_SLAY1'),
('-1540046','I am the only Warchief!','10327','1','0','0','17620','kargath SAY_SLAY2'),
('-1540047','The true Horde... will.. prevail...','10328','1','0','0','17621','kargath SAY_DEATH'),
('-1540048','Cowards! You''ll never pull me into the shadows!','0','1','0','0','18367','kargath SAY_EVADE'),

('-1540049','The Alliance dares to intrude this far into my fortress? Bring out the Honor Hold prisoners and call for the executioner! They''ll pay with their lives for this trespass!','0','6','0','0','13721','kargath SAY_EXECUTE_ALLY'),
('-1540050','It looks like we have a ranking officer among our captives...how amusing. Execute the green-skinned dog at once!','0','6','0','0','13727','kargath SAY_EXECUTE_HORDE'),

('-1540051','Invaders have breached the defenses!','10285','6','0','0','14309','Gauntlet of fire yell 1'),
('-1540052','Archers, form ranks! On my mark!','10156','6','0','0','14310','Gauntlet of fire yell 2'),
('-1540053','Ready!','10157','6','0','0','14311','Gauntlet of fire yell 3'),
('-1540054','Aim!','10158','6','0','0','17910','Gauntlet of fire yell 4'),
('-1540055','Fire!','10159','6','0','0','17911','Gauntlet of fire yell 5'),

('-1540056','Fighter down!','10172','1','0','5','16356','Shattered Hand Legionnaire - Call for Reinforcements'),
('-1540057','Replacement, quickly!','10173','1','0','5','16357','Shattered Hand Legionnaire - Call for Reinforcements'),
('-1540058','Next warrior, now!','10174','1','0','5','16358','Shattered Hand Legionnaire - Call for Reinforcements'),
('-1540059','Fall in! Mok-thora ka!','10175','1','0','5','16359','Shattered Hand Legionnaire - Call for Reinforcements'),
('-1540060','Where''s my support?','10176','1','0','5','16360','Shattered Hand Legionnaire - Call for Reinforcements'),
('-1540061', 'Look Alive!', '10177', '1', '0', '5', 'XXX', 'Shattered Hand Legionnaire - Call for Reinforcements'), -- sound.10202,10227
('-1540062', 'Engage the enemy!', '10178', '1', '0', '5', 'XXX', 'Shattered Hand Legionnaire - Call for Reinforcements'), -- sound.10203,10228
('-1540063','Attack!','10179','1','0','5','35826','Shattered Hand Legionnaire - Call for Reinforcements'),
('-1540064','Next warrior, step up!','10180','1','0','5','16361','Shattered Hand Legionnaire - Call for Reinforcements'),
('-1540065','Join the fight! Agrama-ka!','10181','1','0','5','16362','Shattered Hand Legionnaire - Call for Reinforcements'),
('-1540066','%s goes into a rage after seeing a friend fall in battle!','0','2','0','0','1151','Shattered Hand Legionnaire - Call for Reinforcements'),

-- Unused
-- ('-XXX', 'Wake up, we''re under attack!', '10182', '1', '0', '5', '16363', 'Shattered Hand Legionnaire - Call for Sleeping Reinforcements'), -- sound.10207,10232
-- ('-XXX', 'Sleep on your own time!', '10183', '1', '0', '5', '16364', 'Shattered Hand Legionnaire - Call for Sleeping Reinforcements'), -- sound.10208,10233
-- ('-XXX', 'Get up!', '10184', '1', '0', '5', '16365', 'Shattered Hand Legionnaire - Call for Sleeping Reinforcements'), -- sound.10209,10234
-- ('-XXX', 'On your feet!', '10185', '1', '0', '5', '16366', 'Shattered Hand Legionnaire - Call for Sleeping Reinforcements'), -- sound.10210,10235
-- ('-XXX', 'No time for slumber! Join the fight!', '10186', '1', '0', '5', '16367', 'Shattered Hand Legionnaire - Call for Sleeping Reinforcements'), -- sound.10211,10236
-- ('-XXX', 'Line up and crush these fools!', '10187', '1', '0', '5', '16346', 'Shattered Hand Legionnaire - Positioning Call'), -- sound.10212,10237
-- ('-XXX', 'Form up! Let''s make quick work of them!', '10188', '1', '0', '5', '16347', 'Shattered Hand Legionnaire - Positioning Call'), -- sound.10213,10238
-- ('-XXX', 'Get ready! This shouldn''t take long...', '10189', '1', '0', '5', '16349', 'Shattered Hand Legionnaire - Positioning Call'), -- sound.10214,10239
-- ('-XXX', 'Form ranks and make the intruders pay!', '10190', '1', '0', '5', '17461', 'Shattered Hand Legionnaire - Positioning Call'), -- sound.10215,10240
-- ('-XXX', 'Show them no quarter! Form up!', '10191', '1', '0', '5', '16350', 'Shattered Hand Legionnaire - Positioning Call'), -- sound.10216,10241
-- ('-XXX', 'Lok-Narash! Defensive positions!', '10192', '1', '0', '5', '16352', 'Shattered Hand Legionnaire - Positioning Call'), -- sound.10217,10242
-- ('-XXX', 'Hold the line! They must not get through!', '10193', '1', '0', '5', '16353', 'Shattered Hand Legionnaire - Positioning Call'), -- sound.10218,10243
-- ('-XXX', 'Gakarah ma!', '10194', '1', '0', '5', '16354', 'Shattered Hand Legionnaire - Positioning Call'), -- sound.10219,10244
-- ('-XXX', 'Hold them back at all costs!', '10195', '1', '0', '5', '17462', 'Shattered Hand Legionnaire - Positioning Call'), -- sound.10220,10245
-- ('-XXX', 'We must halt their advance! Take your positions!', '10196', '1', '0', '5', '16355', 'Shattered Hand Legionnaire - Positioning Call'), -- sound.10221,10246

('-1540200','The blood is our power!','0','0','1','0','16700','Common Hellfire Orc Text'),
('-1540201','Lok narash!','0','0','0','0','16703','Common Hellfire Orc Text'),
('-1540202','For Kargath!  For Victory!','0','0','0','0','16698','Common Hellfire Orc Text'),
('-1540203','Lok''tar Illadari!','0','0','0','0','16701','Common Hellfire Orc Text'),
('-1540204','This world is OURS!','0','0','0','0','16702','Common Hellfire Orc Text'),
('-1540205','We are the true Horde!','0','0','0','0','16697','Common Hellfire Orc Text'),
('-1540206','Gakarah ma!','0','0','0','0','16699','Common Hellfire Orc Text');

-- -1 542 000 BLOOD FURNACE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1542000','Who dares interrupt--What is this; what have you done? You''ll ruin everything!','10164','1','0','0','15130','kelidan SAY_WAKE'),
('-1542001','You mustn''t let him loose!','10166','1','0','0','17673','kelidan SAY_ADD_AGGRO_1'),
('-1542002','Ignorant whelps!','10167','1','0','0','17674','kelidan SAY_ADD_AGGRO_2'),
('-1542003','You fools! He''ll kill us all!','10168','1','0','0','17675','kelidan SAY_ADD_AGGRO_3'),
('-1542004','Just as you deserve!','10169','1','0','0','17670','kelidan SAY_KILL_1'),
('-1542005','Your friends will soon be joining you.','10170','1','0','0','17671','kelidan SAY_KILL_2'),
('-1542006','Closer! Come closer... and burn!','10165','1','0','0','15132','kelidan SAY_NOVA'),
('-1542007','Good...luck. You''ll need it.','10171','1','0','0','17672','kelidan SAY_DIE'),

('-1542008','Come intruders....','0','1','0','0','14259','broggok SAY_AGGRO'),

('-1542009','My work must not be interrupted!','10286','1','0','0','17679','the_maker SAY_AGGRO_1'),
('-1542010','Perhaps I can find a use for you...','10287','1','0','0','17680','the_maker SAY_AGGRO_2'),
('-1542011','Anger...hate... These are tools I can use.','10288','1','0','0','17681','the_maker SAY_AGGRO_3'),
('-1542012','Let''s see what I can make of you!','10289','1','0','0','17683','the_maker SAY_KILL_1'),
('-1542013','It is pointless to resist.','10290','1','0','0','17684','the_maker SAY_KILL_2'),
('-1542014','Stay away from... Me!','10291','1','0','0','17685','the_maker SAY_DIE'),

('-1542015','Kill them!','0','1','0','0','15115','broggok SAY_BROGGOK_INTRO');

-- -1 543 000 HELLFIRE RAMPARTS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1543000','Do you smell that? Fresh meat has somehow breached our citadel. Be wary of any intruders.','0','1','0','0','13999','gargolmar SAY_TAUNT'),
('-1543001','Heal me! QUICKLY!','10329','1','0','0','17498','gargolmar SAY_HEAL'),
('-1543002','Back off, pup!','10330','1','0','0','17499','gargolmar SAY_SURGE'),
('-1543003','What have we here...?','10331','1','0','0','56502','gargolmar SAY_AGGRO_1'),
('-1543004','Heh... this may hurt a little.','10332','1','0','0','17503','gargolmar SAY_AGGRO_2'),
('-1543005','I''m gonna enjoy this.','10333','1','0','0','0','gargolmar SAY_AGGRO_3'),
('-1543006','Say farewell!','10334','1','0','0','17500','gargolmar SAY_KILL_1'),
('-1543007','Much too easy...','10335','1','0','0','17501','gargolmar SAY_KILL_2'),
('-1543008','Hahah.. <cough> ..argh!','10336','1','0','0','0','gargolmar SAY_DIE'),

('-1543009','You dare stand against me?!','10280','1','0','0','17634','omor SAY_AGGRO_1'),
('-1543010','I will not be defeated!','10279','1','0','0','17633','omor SAY_AGGRO_2'),
('-1543011','Your insolence will be your death.','10281','1','0','0','17635','omor SAY_AGGRO_3'),
('-1543012','Achor-she-ki! Feast my pet! Eat your fill!','10277','1','0','0','17637','omor SAY_SUMMON'),
('-1543013','A-Kreesh!','10278','1','0','0','17636','omor SAY_CURSE'),
('-1543014','Die, weakling!','10282','1','0','0','17631','omor SAY_KILL_1'),
('-1543015','It is... not over.','10284','1','0','0','17630','omor SAY_DIE'),
('-1543016','I am victorious!','10283','1','0','0','17632','omor SAY_WIPE'),

('-1543017','You have faced many challenges.... Pity they were all in vain! Soon your people will kneel to MY lord!','10292','1','0','0','13981','vazruden SAY_INTRO'),
('-1543018','Your time is running out!','10294','1','0','0','17492','vazruden SAY_AGGRO1'),
('-1543019','You are nothing! I answer a higher call....','10295','1','0','0','17493','vazruden SAY_AGGRO2'),
('-1543020','The Dark Lord laughs at you!','10296','1','0','0','17494','vazruden SAY_AGGRO3'),
('-1543021','Is there no one left to test me?','10293','1','0','0','17491','vazruden SAY_TAUNT'),
('-1543022','It is over. Finished!','10297','1','0','0','17495','vazruden SAY_KILL1'),
('-1543023','Your days are done!','10298','1','0','0','17496','vazruden SAY_KILL2'),
('-1543024','My lord will be... the end of you all!','10299','1','0','0','17497','vazruden SAY_DEATH'),
('-1543025','Nazan descends from the sky.','0','3','0','0','20472','vazruden EMOTE_DESCEND');

-- -1 544 000 MAGTHERIDON'S LAIR
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1544000','Wretched, meddling insects. Release me and perhaps i will grant you a merciful death!','10247','6','0','0','17339','magtheridon SAY_TAUNT1'),
('-1544001','Vermin! Leeches! Take my blood and choke on it!','10248','6','0','0','17340','magtheridon SAY_TAUNT2'),
('-1544002','Illidan is an arrogant fool. I will crush him and reclaim Outland as my own.','10249','6','0','0','17341','magtheridon SAY_TAUNT3'),
('-1544003','Away, you mindless parasites. My blood is my own!','10250','6','0','0','17342','magtheridon SAY_TAUNT4'),
('-1544004','How long do you believe your pathetic sorcery can hold me?','10251','6','0','0','17343','magtheridon SAY_TAUNT5'),
('-1544005','My blood will be the end of you!','10252','6','0','0','17344','magtheridon SAY_TAUNT6'),
('-1544006','I... am... unleashed!','10253','1','0','0','17346','magtheridon SAY_FREED'),
('-1544007','Thank you for releasing me. Now...die!','10254','1','0','0','0','magtheridon SAY_AGGRO'),
('-1544008','Not again... NOT AGAIN!','10256','1','0','0','17348','magtheridon SAY_BANISH'),
('-1544009','I will not be taken so easily! Let the walls of this prison tremble... and fall!','10257','1','0','0','17336','magtheridon SAY_CHAMBER_DESTROY'),
('-1544010','Did you think me weak? Soft? Who is the weak one now?!','10255','1','0','0','17349','magtheridon SAY_PLAYER_KILLED'),
('-1544011','The Legion will consume you all!','10258','1','0','0','17347','magtheridon SAY_DEATH'),
('-1544012','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1544013','%s begins to cast Blast Nova!','0','3','0','0','18739','magtheridon EMOTE_BLASTNOVA'),
('-1544014','%s''s bonds begin to weaken!','0','2','0','0','13689','magtheridon EMOTE_BEGIN'),
('-1544015','%s breaks free!','0','2','0','0','13691','magtheridon EMOTE_FREED'),
('-1544016','%s is nearly free of his bonds!','0','2','0','0','13690','magtheridon EMOTE_NEARLY_FREE');

-- -1 545 000 THE STEAMVAULT
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1545000','Surge forth my pets!','10360','1','0','0','0','thespia SAY_SUMMON'),
('-1545001','The depths will consume you!','10361','1','0','0','17696','thespia SAY_AGGRO_1'),
('-1545002','Meet your doom, surface dwellers!','10362','1','0','0','17697','thespia SAY_AGGRO_2'),
('-1545003','You will drown in blood!','10363','1','0','0','17698','thespia SAY_AGGRO_3'),
('-1545004','To the depths of oblivion with you!','10364','1','0','0','17701','thespia SAY_SLAY_1'),
('-1545005','For my lady and master!','10365','1','0','0','17702','thespia SAY_SLAY_2'),
('-1545006','Our matron will be.. the end of.. you..','10366','1','0','0','17704','thespia SAY_DEAD'),

('-1545007','I''m bringin'' the pain!','10367','1','0','0','17719','mekgineer SAY_MECHANICS'),
('-1545008','You''re in for a world of hurt!','10368','1','0','0','0','mekgineer SAY_AGGRO_1'),
('-1545009','Eat hot metal, scumbag!','10369','1','0','0','17717','mekgineer SAY_AGGRO_2'),
('-1545010','I''ll come over there!','10370','1','0','0','17718','mekgineer SAY_AGGRO_3'),
('-1545011','I''m bringin'' the pain!','10371','1','0','0','17719','mekgineer SAY_AGGRO_4'),
('-1545012','You just got served, punk!','10372','1','0','0','17720','mekgineer SOUND_SLAY_1'),
('-1545013','I own you!','10373','1','0','0','17721','mekgineer SOUND_SLAY_2'),
('-1545014','Have fun dyin'', cupcake!','10374','1','0','0','17722','mekgineer SOUND_SLAY_3'),
('-1545015','Mommy!','10375','1','0','0','832','mekgineer SAY_DEATH'),

('-1545016','You deem yourselves worthy simply because you bested my guards? Our work here will not be compromised!','10390','1','0','0','17724','kalithresh SAY_INTRO'),
('-1545017','This is not nearly over...','10391','1','0','0','17725','kalithresh SAY_REGEN'),
('-1545018','Your head will roll!','10392','1','0','0','17726','kalithresh SAY_AGGRO1'),
('-1545019','I despise all of your kind!','10393','1','0','0','17727','kalithresh SAY_AGGRO2'),
('-1545020','Ba''ahntha sol''dorei!','10394','1','0','0','0','kalithresh SAY_AGGRO3'),
('-1545021','Scram, surface filth!','10395','1','0','0','17729','kalithresh SAY_SLAY1'),
('-1545022','Ah ha ha ha ha ha ha!','10396','1','0','0','0','kalithresh SAY_SLAY2'),
('-1545023','For her Excellency... for... Vashj!','10397','1','0','0','17730','kalithresh SAY_DEATH'),

('-1545024','Enjoy the storm, warm bloods!','0','1','0','0','19456','thespia SAY_CLOUD');

-- -1 546 000 THE UNDERBOG
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1546000','I will end this quickly...','10385','1','0','0','17506','MUSELEK AGGRO_1'),
('-1546001','We fight to the death!','10384','1','0','0','17505','MUSELEK AGGRO_2'),
('-1546002','Acalah pek ecta!','10386','1','0','0','17507','MUSELEK AGGRO_3'),
('-1546003','Beast! Obey me! Kill them at once!','10383','1','0','0','17511','MUSELEK COMMAND'),
('-1546004','Krypta!','10387','1','0','0','17508','MUSELEK SLAY_1'),
('-1546005','It is finished.','10388','1','0','0','17509','MUSELEK SLAY_2'),
('-1546006','Well... done...','10389','1','0','0','17510','MUSELEK DEATH');

-- -1 547 000 THE SLAVE PENS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1547000','Hey!  Over here!  Yeah, over here... I''m in this cage!!!','0','1','0','0','14668','Naturalist Bite on Areatrigger');

-- -1 548 000 SERPENTSHRINE CAVERN
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1548000','I cannot allow you to interfere!','11289','1','0','0','20231','hydross SAY_AGGRO'),
('-1548001','Better, much better.','11290','1','0','0','19607','hydross SAY_SWITCH_TO_CLEAN'),
('-1548002','They have forced me to this...','11291','1','0','0','20235','hydross SAY_CLEAN_SLAY1'),
('-1548003','I have no choice.','11292','1','0','0','0','hydross SAY_CLEAN_SLAY2'),
('-1548004','I am... released...','11293','1','0','0','37993','hydross SAY_CLEAN_DEATH'),
('-1548005','Aaghh, the poison...','11297','1','0','0','19606','hydross SAY_SWITCH_TO_CORRUPT'),
('-1548006','I will purge you from this place.','11298','1','0','0','20233','hydross SAY_CORRUPT_SLAY1'),
('-1548007','You are no better than they!','11299','1','0','0','20234','hydross SAY_CORRUPT_SLAY2'),
('-1548008','You are the disease, not I','11300','1','0','0','20239','hydross SAY_CORRUPT_DEATH'),

('-1548009','Finally, my banishment ends!','11312','1','0','0','19596','leotheras SAY_AGGRO'),
('-1548010','Be gone, trifling elf.  I am in control now!','11304','1','0','0','19595','leotheras SAY_SWITCH_TO_DEMON'),
('-1548011','We all have our demons...','11305','1','0','0','19597','leotheras SAY_INNER_DEMONS'),
('-1548012','I have no equal.','11306','1','0','0','19993','leotheras SAY_DEMON_SLAY1'),
('-1548013','Perish, mortal.','11307','1','0','0','19994','leotheras SAY_DEMON_SLAY2'),
('-1548014','Yes, YES! Ahahah!','11308','1','0','0','0','leotheras SAY_DEMON_SLAY3'),
('-1548015','Kill! KILL!','11314','1','0','0','19996','leotheras SAY_NIGHTELF_SLAY1'),
('-1548016','That''s right! Yes!','11315','1','0','0','19997','leotheras SAY_NIGHTELF_SLAY2'),
('-1548017','Who''s the master now?','11316','1','0','0','19998','leotheras SAY_NIGHTELF_SLAY3'),
('-1548018','No... no! What have you done? I am the master! Do you hear me? I am... aaggh! Can''t... contain him...','11313','1','0','0','19501','leotheras SAY_FINAL_FORM'),
('-1548019','At last I am liberated. It has been too long since I have tasted true freedom!','11309','1','0','0','19502','leotheras SAY_FREE'),
('-1548020','You cannot kill me! Fools, I''ll be back! I''ll... aarghh...','11317','1','0','0','19999','leotheras SAY_DEATH'),

('-1548021','Guards, attention! We have visitors....','11277','1','0','0','20000','karathress SAY_AGGRO'),
('-1548022','Your overconfidence will be your undoing! Guards, lend me your strength!','11278','1','0','0','19854','karathress SAY_GAIN_BLESSING'),
('-1548023','Go on, kill them! I''ll be the better for it!','11279','1','0','0','19643','karathress SAY_GAIN_ABILITY1'),
('-1548024','I am more powerful than ever!','11280','1','0','0','19645','karathress SAY_GAIN_ABILITY2'),
('-1548025','More knowledge, more power!','11281','1','0','0','19644','karathress SAY_GAIN_ABILITY3'),
('-1548026','Land-dwelling scum!','11282','1','0','0','20001','karathress SAY_SLAY1'),
('-1548027','Alana be''lendor!','11283','1','0','0','20002','karathress SAY_SLAY2'),
('-1548028','I am rid of you.','11284','1','0','0','20003','karathress SAY_SLAY3'),
('-1548029','Her ... excellency ... awaits!','11285','1','0','0','20004','karathress SAY_DEATH'),

('-1548030','Flood of the deep, take you!','11321','1','0','0','20931','morogrim SAY_AGGRO'),
('-1548031','By the Tides, kill them at once!','11322','1','0','0','0','morogrim SAY_SUMMON1'),
('-1548032','Destroy them my subjects!','11323','1','0','0','20934','morogrim SAY_SUMMON2'),
('-1548033','There is nowhere to hide!','11324','1','0','0','20935','morogrim SAY_SUMMON_BUBL1'),
('-1548034','Soon it will be finished!','11325','1','0','0','20936','morogrim SAY_SUMMON_BUBL2'),
('-1548035','It is done!','11326','1','0','0','20937','morogrim SAY_SLAY1'),
('-1548036','Strugging only makes it worse.','11327','1','0','0','20938','morogrim SAY_SLAY2'),
('-1548037','Only the strong survive.','11328','1','0','0','20939','morogrim SAY_SLAY3'),
('-1548038','Great... currents of... Ageon.','11329','1','0','0','20940','morogrim SAY_DEATH'),
('-1548039','%s sends his enemies to their watery graves!','0','3','0','0','20701','morogrim EMOTE_WATERY_GRAVE'),
('-1548040','The violent earthquake has alerted nearby Murlocs!','0','3','0','0','20700','morogrim EMOTE_EARTHQUAKE'),
('-1548041','%s summons watery globules!','0','3','0','0','20703','morogrim EMOTE_WATERY_GLOBULES'),

('-1548042','Water is life. It has become a rare commodity here in Outland. A commodity that we alone shall control. We are the Highborne, and the time has come at last for us to retake our rightful place in the world!','11531','6','0','0','20085','vashj SAY_INTRO'),
('-1548043','I''ll split you from stem to stern!','11532','1','0','0','20088','vashj SAY_AGGRO1'),
('-1548044','Victory to Lord Illidan!','11533','1','0','0','20089','vashj SAY_AGGRO2'),
('-1548045','I spit on you, surface filth!','11534','1','0','0','20091','vashj SAY_AGGRO3'),
('-1548046','Death to the outsiders!','11535','1','0','0','20090','vashj SAY_AGGRO4'),
('-1548047','I did not wish to lower myself by engaging your kind, but you leave me little choice...','11538','1','0','0','19679','vashj SAY_AGGRO5'),
('-1548048','The time is now! Leave none standing!','11539','1','0','0','20092','vashj SAY_PHASE2'),
('-1548049','You may want to take cover.','11540','1','0','0','20210','vashj SAY_PHASE3'),
('-1548050','Straight to the heart!','11536','1','0','0','20086','vashj SAY_BOWSHOT1'),
('-1548051','Seek your mark!','11537','1','0','0','20087','vashj SAY_BOWSHOT2'),
('-1548052','Your time ends now!','11541','1','0','0','20093','vashj SAY_SLAY1'),
('-1548053','You have failed!','11542','1','0','0','12854','vashj SAY_SLAY2'),
('-1548054','Be''lemer an-delei!','11543','1','0','0','20095','vashj SAY_SLAY3'),
('-1548055','Lord Illidan, I... I am... sorry.','11544','1','0','0','20096','vashj SAY_DEATH'),

('-1548056','%s takes a deep breath!','0','3','0','0','20774','lurker below EMOTE_DEEP_BREATH');

-- -1 550 000 THE EYE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1550000','Alert! You are marked for extermination.','11213','1','0','0','20906','voidreaver SAY_AGGRO'),
('-1550001','Extermination, successful.','11215','1','0','0','20908','voidreaver SAY_SLAY1'),
('-1550002','Imbecile life form, no longer functional.','11216','1','0','0','20909','voidreaver SAY_SLAY2'),
('-1550003','Threat neutralized.','11217','1','0','0','20910','voidreaver SAY_SLAY3'),
('-1550004','Systems... shutting... d-o-w-n...','11214','1','0','0','20907','voidreaver SAY_DEATH'),
('-1550005','Alternative measure commencing...','11218','1','0','0','20911','voidreaver SAY_POUNDING1'),
('-1550006','Calculating force parameters...','11219','1','0','0','20912','voidreaver SAY_POUNDING2'),

('-1550007','Tal anu''men no sin''dorei!','11134','1','0','0','20849','solarian SAY_AGGRO'),
('-1550008','Ha ha ha! You are hopelessly outmatched!','11139','1','0','0','56464','solarian SAY_SUMMON1'),
('-1550009','I will crush your delusions of grandeur!','11140','1','0','0','20855','solarian SAY_SUMMON2'),
('-1550010','Your soul belongs to the Abyss!','11136','1','0','0','20851','solarian SAY_KILL1'),
('-1550011','By the blood of the Highborne!','11137','1','0','0','20852','solarian SAY_KILL2'),
('-1550012','For the Sunwell!','11138','1','0','0','20853','solarian SAY_KILL3'),
('-1550013','The warmth of the sun... awaits.','11135','1','0','0','20850','solarian SAY_DEATH'),
('-1550014','Enough of this!  Now I call upon the fury of the cosmos itself.','0','1','0','0','20372','solarian SAY_VOIDA'),
('-1550015','I become ONE... with the VOID!','0','1','0','0','20373','solarian SAY_VOIDB'),

('-1550016','Energy. Power. My people are addicted to it... a dependence made manifest after the Sunwell was destroyed. Welcome... to the future. A pity you are too late to stop it. No one can stop me now! Selama ashal''anore!','11256','1','0','0','20195','kaelthas SAY_INTRO note: was changed sometime after TBC'),
('-1550017','Capernian will see to it that your stay here is a short one.','11257','1','0','0','19004','kaelthas SAY_INTRO_CAPERNIAN'),
('-1550018','Well done, you have proven worthy to test your skills against my master engineer, Telonicus.','11258','1','0','0','19005','kaelthas SAY_INTRO_TELONICUS'),
('-1550019','Let us see how your nerves hold up against the Darkener, Thaladred!','11259','1','0','0','19006','kaelthas SAY_INTRO_THALADRED'),
('-1550020','You have persevered against some of my best advisors... but none can withstand the might of the Blood Hammer. Behold, Lord Sanguinar!','11260','1','0','0','19003','kaelthas SAY_INTRO_SANGUINAR'),
('-1550021','As you see, I have many weapons in my arsenal....','11261','1','0','0','19053','kaelthas SAY_PHASE2_WEAPON'),
('-1550022','Perhaps I underestimated you. It would be unfair to make you fight all four advisors at once, but... fair treatment was never shown to my people. I''m just returning the favor.','11262','1','0','0','19060','kaelthas SAY_PHASE3_ADVANCE'),
('-1550023','Alas, sometimes one must take matters into one''s own hands. Balamore shanal!','11263','1','0','0','19195','kaelthas SAY_PHASE4_INTRO2'),
('-1550024','I have not come this far to be stopped! The future I have planned will not be jeopardized! Now you will taste true power!!','11273','1','0','0','20208','kaelthas SAY_PHASE5_NUTS'),
('-1550025','You will not prevail.','11270','1','0','0','20204','kaelthas SAY_SLAY1'),
('-1550026','You gambled...and lost.','11271','1','0','0','20205','kaelthas SAY_SLAY2'),
('-1550027','This was Child''s play.','11272','1','0','0','20206','kaelthas SAY_SLAY3'),
('-1550028','Obey me.','11268','1','0','0','20202','kaelthas SAY_MINDCONTROL1'),
('-1550029','Bow to my will.','11269','1','0','0','18259','kaelthas SAY_MINDCONTROL2'),
('-1550030','Let us see how you fare when your world is turned upside down.','11264','1','0','0','20198','kaelthas SAY_GRAVITYLAPSE1'),
('-1550031','Having trouble staying grounded?','11265','1','0','0','20199','kaelthas SAY_GRAVITYLAPSE2'),
('-1550032','Anar''anel belore!','11267','1','0','0','20201','kaelthas SAY_SUMMON_PHOENIX1'),
('-1550033','By the power of the sun!','11266','1','0','0','20200','kaelthas SAY_SUMMON_PHOENIX2'),
('-1550034','For... Quel''...Thalas!','11274','1','0','0','20207','kaelthas SAY_DEATH'),

('-1550035','Prepare yourselves!','11203','1','0','0','20905','thaladred SAY_THALADRED_AGGRO'),
('-1550036','Forgive me, my prince! I have... failed.','11204','1','0','0','20904','thaladred SAY_THALADRED_DEATH'),
('-1550037','%s sets eyes on $n!','0','2','0','0','11074','thaladred EMOTE_THALADRED_GAZE'),

('-1550038','Blood for blood!','11152','1','0','0','20859','sanguinar SAY_SANGUINAR_AGGRO'),
('-1550039','NO! I... will... not...','11153','1','0','0','20858','sanguinar SAY_SANGUINAR_DEATH'),

('-1550040','The sin''dorei reign supreme!','11117','1','0','0','20847','capernian SAY_CAPERNIAN_AGGRO'),
('-1550041','This is not over!','11118','1','0','0','29826','capernian SAY_CAPERNIAN_DEATH'),

('-1550042','Anar''alah belore!','11157','1','0','0','20860','telonicus SAY_TELONICUS_AGGRO'),
('-1550043','More perils... await...','11158','1','0','0','20861','telonicus SAY_TELONICUS_DEATH'),

('-1550044','%s begins to cast Pyroblast!','0','3','0','0','20775','kaelthas EMOTE_PYROBLAST');

-- -1 552 000 THE ARCATRAZ
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1552000','It is a small matter to control the mind of the weak... for I bear allegiance to powers untouched by time, unmoved by fate. No force on this world or beyond harbors the strength to bend our knee... not even the mighty Legion!','11122','1','0','0','19112','skyriss SAY_INTRO'),
('-1552001','Bear witness to the agent of your demise!','11123','1','0','0','19113','skyriss SAY_AGGRO'),
('-1552002','Your fate is written!','11124','1','0','0','19813','skyriss SAY_KILL_1'),
('-1552003','The chaos I have sown here is but a taste...','11125','1','0','0','19814','skyriss SAY_KILL_2'),
('-1552004','You will do my bidding, weakling.','11127','1','0','0','19816','skyriss SAY_MIND_1'),
('-1552005','Your will is no longer your own.','11128','1','0','0','19817','skyriss SAY_MIND_2'),
('-1552006','Flee in terror!','11129','1','0','0','19818','skyriss SAY_FEAR_1'),
('-1552007','I will show you horrors undreamed of!','11130','1','0','0','19819','skyriss SAY_FEAR_2'),
('-1552008','We span the universe, as countless as the stars!','11131','1','0','0','19821','skyriss SAY_IMAGE'),
('-1552009','I am merely one of... infinite multitudes.','11126','1','0','0','19815','skyriss SAY_DEATH'),

('-1552010','Where in Bonzo''s Brass Buttons am I? And who are-- yaaghh, that''s one mother of a headache!','11171','1','0','6','19108','millhouse SAY_INTRO_1'),
('-1552011','"Lowly"? I don''t care who you are friend, no one refers to the mighty Millhouse Manastorm as "Lowly"! I have no idea what goes on here, but I will gladly join your fight against this impudent imbecile! Prepare to defend yourself, cretin!','11172','1','0','5','19116','millhouse SAY_INTRO_2'),
('-1552012','I just need to get some things ready first. You guys go ahead and get started. I need to summon up some water...','11173','1','0','0','19117','millhouse SAY_WATER'),
('-1552013','Fantastic! Next, some protective spells. Yes! Now we''re cookin''','11174','1','0','0','19119','millhouse SAY_BUFFS'),
('-1552014','And of course i''ll need some mana. You guys are gonna love this, just wait.','11175','1','0','0','19120','millhouse SAY_DRINK'),
('-1552015','Aaalllriiiight!! Who ordered up an extra large can of whoop-ass?','11176','1','0','0','19121','millhouse SAY_READY'),
('-1552016','I didn''t even break a sweat on that one.','11177','1','0','0','19824','millhouse SAY_KILL_1'),
('-1552017','You guys, feel free to jump in anytime.','11178','1','0','0','19825','millhouse SAY_KILL_2'),
('-1552018','I''m gonna light you up, sweet cheeks!','11179','1','0','0','19826','millhouse SAY_PYRO'),
('-1552019','Ice, ice, baby!','11180','1','0','0','19827','millhouse SAY_ICEBLOCK'),
('-1552020','Heal me! Oh, for the love of all that is holy, HEAL me! I''m dying!','11181','1','0','0','19828','millhouse SAY_LOWHP'),
('-1552021','You''ll be hearing from my lawyer...','11182','1','0','0','19829','millhouse SAY_DEATH'),
('-1552022','Who''s bad? Who''s bad? That''s right: we bad!','11183','1','0','4','19146','millhouse SAY_COMPLETE'),

('-1552023','I knew the prince would be angry but, I... I have not been myself. I had to let them out! The great one speaks to me, you see. Wait--outsiders. Kael''thas did not send you! Good... I''ll just tell the prince you released the prisoners!','11222','1','0','0','19103','mellichar YELL_INTRO1'),
('-1552024','The naaru kept some of the most dangerous beings in existence here in these cells. Let me introduce you to another...','11223','1','0','0','19104','mellichar YELL_INTRO2'),
('-1552025','Yes, yes... another! Your will is mine!','11224','1','0','0','19106','mellichar YELL_RELEASE1'),
('-1552026','Behold another terrifying creature of incomprehensible power!','11225','1','0','0','19107','mellichar YELL_RELEASE2A'),
('-1552027','What is this? A lowly gnome? I will do better, O''great one.','11226','1','0','0','19109','mellichar YELL_RELEASE2B'),
('-1552028','Anarchy! Bedlam! Oh, you are so wise! Yes, I see it now, of course!','11227','1','0','0','19110','mellichar YELL_RELEASE3'),
('-1552029','One final cell remains. Yes, O''great one, right away!','11228','1','0','0','19111','mellichar YELL_RELEASE4'),
('-1552030','Welcome, O''great one. I am your humble servant.','11229','1','0','0','19114','mellichar YELL_WELCOME'),

('-1552031','It is unwise to anger me.','11086','1','0','0','19978','dalliah SAY_AGGRO'),
('-1552032','Ahh... That is much better.','11091','1','0','0','19984','dalliah SAY_HEAL_1'),
('-1552033','Ahh... Just what I needed.','11092','1','0','0','19985','dalliah SAY_HEAL_2'),
('-1552034','Completely ineffective. Just like someone else I know.','11087','1','0','0','19980','dalliah SAY_KILL_1'),
('-1552035','You chose the wrong opponent.','11088','1','0','0','19981','dalliah SAY_KILL_2'),
('-1552036','I''ll cut you to pieces!','11090','1','0','0','19983','dalliah SAY_WHIRLWIND_1'),
('-1552037','Reap the Whirlwind!','11089','1','0','0','19982','dalliah SAY_WHIRLWIND_2'),
('-1552038','Now I''m really... angry...','11093','1','0','0','19986','dalliah SAY_DEATH'),

('-1552039','Have you come to kill Dalliah? Can I watch?','11237','1','0','1','19968','soccothrates SAY_DALLIAH_AGGRO_1'),
('-1552040','This may be the end for you, Dalliah. What a shame that would be.','11245','1','0','1','19975','soccothrates SAY_DALLIAH_TAUNT_1'),
('-1552041','Facing difficulties, Dalliah? How nice.','11244','1','0','1','19974','soccothrates SAY_DALLIAH_TAUNT_2'),
('-1552042','I suggest a new strategy, you draw the attackers while I gather reinforcements. Hahaha!','11246','1','0','1','19976','soccothrates SAY_DALLIAH_TAUNT_3'),
('-1552043','Finally! Well done!','11247','1','0','66','19977','soccothrates SAY_DALLIAH_DEAD'),
('-1552044','On guard!','11241','1','0','0','19971','soccothrates SAY_CHARGE_1'),
('-1552045','Defend yourself, for all the good it will do...','11242','1','0','0','19972','soccothrates SAY_CHARGE_2'),
('-1552046','Knew this was... the only way out','11243','1','0','0','19973','soccothrates SAY_DEATH'),
('-1552047','Yes, that was quite satisfying','11239','1','0','0','19969','soccothrates SAY_KILL'),
('-1552048','At last, a target for my frustrations!','11238','1','0','0','19967','soccothrates SAY_AGGRO'),

('-1552049','Did you call on me?','11236','1','0','397','20051','soccothrates SAY_INTRO_1'),
('-1552050','Why would I call on you?','0','1','0','396','20055','dalliah SAY_INTRO_2'),
('-1552051','To do your heavy lifting, most likely.','0','1','0','396','20052','soccothrates SAY_INTRO_3'),
('-1552052','When I need someone to prance around like an overstuffed peacock, I''ll call on you.','0','1','0','396','20056','dalliah SAY_INTRO_4'),
('-1552053','Then I''ll commit myself to ignoring you.','0','1','0','396','20053','soccothrates SAY_INTRO_5'),
('-1552054','What would you know about commitment, sheet-sah?','0','1','0','396','20057','dalliah SAY_INTRO_6'),
('-1552055','You''re the one who should be-- Wait, we have company...','0','1','0','396','20054','soccothrates SAY_INTRO_7');

-- -1 553 000 THE BOTANICA
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1553000','What are you doing? These specimens are very delicate!','11144','1','0','0','19944','freywinn SAY_AGGRO'),
('-1553001','Your life cycle is now concluded!','11145','1','0','0','19945','freywinn SAY_KILL_1'),
('-1553002','You will feed the worms.','11146','1','0','0','19946','freywinn SAY_KILL_2'),
('-1553003','Endorel aluminor!','11147','1','0','0','19947','freywinn SAY_TREE_1'),
('-1553004','Nature bends to my will!','11148','1','0','0','19948','freywinn SAY_TREE_2'),
('-1553005','The specimens...must be preserved.','11149','1','0','0','19949','freywinn SAY_DEATH'),

('-1553006','%s emits a strange noise.','0','2','0','0','10755','laj EMOTE_SUMMON'),

('-1553007','Who disturbs this sanctuary?','11230','1','0','0','20024','warp SAY_AGGRO'),
('-1553008','You must die! But wait: this does not--No, no... you must die!','11231','1','0','0','20025','warp SAY_SLAY_1'),
('-1553009','What am I doing? Why do I...','11232','1','0','0','20026','warp SAY_SLAY_2'),
('-1553010','Children, come to me!','11233','1','0','0','18376','warp SAY_SUMMON_1'),
('-1553011','Maybe this is not--No, we fight! Come to my aid.','11234','1','0','0','20022','warp SAY_SUMMON_2'),
('-1553012','So... confused. Do not... belong here!','11235','1','0','0','20028','warp SAY_DEATH'),
('-1553013','You won''t... get far.','11212','1','0','0','20019','Thorngrin YELL on Death'),
('-1553014','I hate to say I told you so...','11207','1','0','0','20014','Thorngrind YELL on Player Kill'),
('-1553015','Your life will be mine!','11208','1','0','0','20015','Thorngrin YELL on Sacrifice'),
('-1553016','You seek a prize, eh? How about death?','11206','1','0','0','20013','Thorngrin YELL on 20% HP'),
('-1553017','I revel in your pain!','11209','1','0','0','20016','Thorngrin YELL on 50% HP'),
('-1553018','I''ll incinerate you!','11210','1','0','0','20017','Thorngrin YELL on Hellfire 1'),
('-1553019','Scream while you burn! ','11211','1','0','0','20018','Thorngrin YELL on Hellfire 2'),
('-1553020','What aggravation is this? You will die!','11205','1','0','0','20012','Thorngrin YELL on Aggro');

-- -1 554 000 THE MECHANAR
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1554000','Think you can hurt me, huh? Think I''m afraid of you?!','11165','1','0','0','19955','Say on Ability use 1'),
('-1554001','Go ahead, gimme your best shot. I can take it!','11166','1','0','0','19956','Say on Ability use 2'),
('-1554002','Bully!','11167','1','0','0','19957','Say on Death'),
('-1554003','Can''t say I didn''t warn you!','11163','1','0','0','19953','Say on Player Kill 2'),
('-1554004','Damn, I''m good!','11164','1','0','0','19954','Say on Player Kill 1'),
('-1554005','You should split while you can.','11162','1','0','0','19952','Say on Aggro'),
('-1554006','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1554007','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1554008','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1554009','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1554010','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1554011','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1554012','REUSE_ME','0','0','0','0','0','REUSE_ME'),

('-1554013','Don''t value your life very much, do you?','11186','1','0','0','19959','sepethrea SAY_AGGRO'),
('-1554014','I am not alone.','11191','1','0','0','19965','sepethrea SAY_SUMMON'),
('-1554015','Think you can take the heat?','11189','1','0','0','19963','sepethrea SAY_DRAGONS_BREATH_1'),
('-1554016','Anar''endal dracon!','11190','1','0','0','19964','sepethrea SAY_DRAGONS_BREATH_2'),
('-1554017','And don''t come back!','11187','1','0','0','19961','sepethrea SAY_SLAY1'),
('-1554018','En''dala finel el''dal','11188','1','0','0','19962','sepethrea SAY_SLAY2'),
('-1554019','Anu... bala belore...alon.','11192','1','0','0','19966','sepethrea SAY_DEATH'),

('-1554020','We are on a strict timetable. You will not interfere!','11193','1','0','0','20041','pathaleon SAY_AGGRO'),
('-1554021','I''m looking for a team player...','11197','1','0','0','18716','pathaleon SAY_DOMINATION_1'),
('-1554022','You work for me now!','11198','1','0','0','20040','pathaleon SAY_DOMINATION_2'),
('-1554023','Time to supplement my work force.','11196','1','0','0','18717','pathaleon SAY_SUMMON'),
('-1554024','I prefer to be hands-on...','11199','1','0','0','18725','pathaleon SAY_ENRAGE'),
('-1554025','A minor inconvenience.','11194','1','0','0','20042','pathaleon SAY_SLAY_1'),
('-1554026','Looks like you lose.','11195','1','0','0','20043','pathaleon SAY_SLAY_2'),
('-1554027','The project will... continue.','11200','1','0','0','20044','pathaleon SAY_DEATH'),
('-1554028','I have been waiting for you!','0','1','0','53','19059','pathaleon SAY_INTRO');

-- -1 555 000 SHADOW LABYRINTH
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1555000','Infidels have invaded the sanctuary! Sniveling pests...You have yet to learn the true meaning of agony!','10473','1','0','0','0','hellmaw SAY_INTRO'),
('-1555001','Pathetic mortals! You will pay dearly!','10475','1','0','0','17860','hellmaw SAY_AGGRO1'),
('-1555002','I will break you!','10476','1','0','0','17861','hellmaw SAY_AGGRO2'),
('-1555003','Finally! Something to relieve the tedium!','10477','1','0','0','17863','hellmaw SAY_AGGRO3'),
('-1555004','Aid me, you fools, before it''s too late!','10474','1','0','0','17859','hellmaw SAY_HELP'),
('-1555005','Do you fear death?','10478','1','0','0','17864','hellmaw SAY_SLAY1'),
('-1555006','This is the part I enjoy most.','10479','1','0','0','17865','hellmaw SAY_SLAY2'),
('-1555007','Do not...grow...overconfident, mortal.','10480','1','0','0','17866','hellmaw SAY_DEATH'),

('-1555008','All flesh must burn.','10482','1','0','0','0','blackhearth SAY_INTRO1'),
('-1555009','All creation must be unmade!','10483','1','0','0','0','blackhearth SAY_INTRO2'),
('-1555010','Power will be yours!','10484','1','0','0','0','blackhearth SAY_INTRO3'),
('-1555011','You''ll be sorry!','10486','1','0','0','17563','blackhearth SAY_AGGRO1'),
('-1555012','Time for fun!','10487','1','0','0','16433','blackhearth SAY_AGGRO2'),
('-1555013','I see dead people!','10488','1','0','0','0','blackhearth SAY_AGGRO3'),
('-1555014','No comin'' back for you!','10489','1','0','0','0','blackhearth SAY_SLAY1'),
('-1555015','Nice try!','10490','1','0','0','19528','blackhearth SAY_SLAY2'),
('-1555016','Help us, hurry!','10485','1','0','0','0','blackhearth SAY_HELP'),
('-1555017','This... no... good...','10491','1','0','0','17569','blackhearth SAY_DEATH'),

('-1555018','Be ready for Dark One''s return.','10492','1','0','0','0','blackhearth SAY2_INTRO1'),
('-1555019','So we have place in new universe.','10493','1','0','0','0','blackhearth SAY2_INTRO2'),
('-1555020','Dark one promise!','10494','1','0','0','0','blackhearth SAY2_INTRO3'),
('-1555021','You''ll be sorry!','10496','1','0','0','17563','blackhearth SAY2_AGGRO1'),
('-1555022','Time to kill!','10497','1','0','0','17565','blackhearth SAY2_AGGRO2'),
('-1555023','You be dead people!','10498','1','0','0','0','blackhearth SAY2_AGGRO3'),
('-1555024','Now you gone for good.','10499','1','0','0','17576','blackhearth SAY2_SLAY1'),
('-1555025','You failed, haha haha','10500','1','0','0','0','blackhearth SAY2_SLAY2'),
('-1555026','Help us, hurry!','10495','1','0','0','0','blackhearth SAY2_HELP'),
('-1555027','Arrgh, aah...ahhh','10501','1','0','0','0','blackhearth SAY2_DEATH'),

('-1555028','Keep your minds focused for the days of reckoning are close at hand. Soon, the destroyer of worlds will return to make good on his promise. Soon the destruction of all that is will begin!','10522','1','0','0','0','vorpil SAY_INTRO'),
('-1555029','I''ll make an offering of your blood!','10524','1','0','0','17868','vorpil SAY_AGGRO1'),
('-1555030','You''ll be a fine example, for the others.','10525','1','0','0','17869','vorpil SAY_AGGRO2'),
('-1555031','Good, a worthy sacrifice.','10526','1','0','0','17870','vorpil SAY_AGGRO3'),
('-1555032','Come to my aid, heed your master now!','10523','1','0','0','17867','vorpil SAY_HELP'),
('-1555033','I serve with pride.','10527','1','0','0','17871','vorpil SAY_SLAY1'),
('-1555034','Your death is for the greater cause!','10528','1','0','0','17872','vorpil SAY_SLAY2'),
('-1555035','I give my life... Gladly.','10529','1','0','0','17873','vorpil SAY_DEATH'),

('-1555036','%s draws energy from the air.','0','2','0','0','18799','murmur EMOTE_SONIC_BOOM');

-- -1 556 000 SETHEKK HALLS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1556000','I have pets..<squawk>..of my own!','10502','1','0','0','17733','syth SAY_SUMMON'),
('-1556001','Hrrmm.. Time to.. hrrm.. make my move.','10503','1','0','0','17734','syth SAY_AGGRO_1'),
('-1556002','Nice pets..hrm.. Yes! <squawking>','10504','1','0','0','17735','syth SAY_AGGRO_2'),
('-1556003','Nice pets have.. weapons. No so..<squawk>..nice.','10505','1','0','0','17736','syth SAY_AGGRO_3'),
('-1556004','Death.. meeting life is.. <squawking>','10506','1','0','0','0','syth SAY_SLAY_1'),
('-1556005','Uhn.. Be free..<squawk>','10507','1','0','0','17738','syth SAY_SLAY_2'),
('-1556006','No more life..hrm. No more pain. <squawks weakly>','10508','1','0','0','17739','syth SAY_DEATH'),

('-1556007','<squawk>..Trinkets yes pretty Trinkets..<squawk>..power, great power.<squawk>..power in Trinkets..<squawk>','10557','1','0','0','17769','ikiss SAY_INTRO'),
('-1556008','You make war on Ikiss?..<squawk>','10554','1','0','0','17765','ikiss SAY_AGGRO_1'),
('-1556009','Ikiss cut you pretty..<squawk>..slice you. Yes!','10555','1','0','0','17767','ikiss SAY_AGGRO_2'),
('-1556010','No escape for..<squawk>..for you','10556','1','0','0','17768','ikiss SAY_AGGRO_3'),
('-1556011','You die..<squawk>..stay away from Trinkets','10558','1','0','0','17763','ikiss SAY_SLAY_1'),
('-1556012','<squawk>','10559','1','0','0','0','ikiss SAY_SLAY_2'),
('-1556013','Ikiss will not..<squawk>..die','10560','1','0','0','17762','ikiss SAY_DEATH'),
('-1556015','%s begins to channel arcane energy...','0','3','0','0','19738','ikiss EMOTE_ARCANE_EXP'),

('-1556016','No! How can this be?','0','1','0','0','20797','anzu SAY_INTRO_1'),
('-1556017','Pain will be the price for your insolence! You cannot stop me from claiming the Emerald Dream as my own!','0','1','0','0','20799','anzu SAY_INTRO_2'),
('-1556018','Awaken, my children and assist your master!','0','1','0','0','20991','anzu SAY_BANISH'),
('-1556019','Your magics shall be your undoing... ak-a-ak...','0','5','0','0','21015','anzu SAY_WHISPER_MAGIC_1'),
('-1556020','%s returns to stone.','0','2','0','0','20980','anzu EMOTE_BIRD_STONE'),
('-1556021','Your powers... ak-ak... turn against you...','0','5','0','0','21016','anzu SAY_WHISPER_MAGIC_2'),
('-1556022','Your spells... ke-kaw... are weak magics... easy to turn against you...','0','5','0','0','21017','anzu SAY_WHISPER_MAGIC_3');

-- -1 557 000 MANA TOMBS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1557000','What is this? You must forgive me, but I was not expecting company. As you can see, we are somewhat preoccupied right now. But no matter. As I am a gracious host, I will tend to you... personally.','10539','1','0','0','17784','shaffar SAY_INTRO'),
('-1557001','We have not yet been properly introduced.','10541','1','0','0','17780','shaffar SAY_AGGRO_1'),
('-1557002','An epic battle. How exciting!','10542','1','0','0','17781','shaffar SAY_AGGRO_2'),
('-1557003','I have longed for a good adventure.','10543','1','0','0','17782','shaffar SAY_AGGRO_3'),
('-1557004','It has been... entertaining.','10544','1','0','0','17779','shaffar SAY_SLAY_1'),
('-1557005','And now we part company.','10545','1','0','0','17778','shaffar SAY_SLAY_2'),
('-1557006','I have such fascinating things to show you.','10540','1','0','0','17783','shaffar SAY_SUMMON'),
('-1557007','I must bid you... farewell.','10546','1','0','0','17777','shaffar SAY_DEAD'),

('-1557008','I will feed on your soul.','10561','1','0','0','17771','pandemonius SAY_AGGRO_1'),
('-1557009','So... full of life!','10562','1','0','0','17772','pandemonius SAY_AGGRO_2'),
('-1557010','Do not... resist.','10563','1','0','0','17773','pandemonius SAY_AGGRO_3'),
('-1557011','Yes! I am... empowered!','10564','1','0','0','17774','pandemonius SAY_KILL_1'),
('-1557012','More... I must have more!','10565','1','0','0','17775','pandemonius SAY_KILL_2'),
('-1557013','To the void... once... more..','10566','1','0','0','17776','pandemonius SAY_DEATH'),
('-1557014','%s shifts into the void...','0','3','0','0','21067','pandemonius EMOTE_DARK_SHELL'),

('-1557015','This should''t take very long. Just watch my back as I empty these nether collectors.','0','0','0','0','17246','sha''heen SAY_ESCORT_START'),
('-1557016','Fantastic! Let''s move on, shall we?','0','0','0','0','17248','sha''heen SAY_START'),
('-1557017','Looking at these energy levels, Shaffar was set to make a killing!','0','0','0','28','17249','sha''heen SAY_FIRST_STOP'),
('-1557018','That should do it...','0','0','0','0','17251','sha''heen SAY_FIRST_STOP_COMPLETE'),
('-1557019','Hrm, now where is the next collector?','0','0','0','0','17252','sha''heen SAY_COLLECTOR_SEARCH'),
('-1557020','Ah, there it is. Follow me, fleshling.','0','0','0','0','17254','sha''heen SAY_COLLECTOR_FOUND'),
('-1557021','There can''t be too many more of these collectors. Just keep me safe as I do my job.','0','0','0','28','17255','sha''heen SAY_SECOND_STOP'),
('-1557022','What do we have here? I thought you said the area was secure? This is now the third attack? If we make it out of here, I will definitely be deducting this from your reward. Now don''t just stand there, destroy them so I can get to that collector.','0','0','0','0','17256','sha''heen SAY_THIRD_STOP'),
('-1557023','We''re close to the exit. I''ll let you rest for about thirty seconds, but then we''re out of here.','0','0','0','0','17258','sha''heen SAY_REST'),
('-1557024','Are you ready to go?','0','0','0','0','17260','sha''heen SAY_READY_GO'),
('-1557025','Ok break time is OVER. Let''s go!','0','0','0','0','17261','sha''heen SAY_BREAK_OVER'),
('-1557026','Bravo! Bravo! Good show... I couldn''t convince you to work for me, could I? No, I suppose the needless slaughter of my employees might negatively impact your employment application.','0','1','0','0','17241','xiraxis SAY_SPAWN'),
('-1557027','Your plan was a good one, Sha''heen, and you would have gotten away with it if not for one thing...','0','0','0','1','17243','xiraxis SAY_FINAL_STOP_1'),
('-1557028','Oh really? And what might that be?','0','0','0','1','17263','sha''heen SAY_FINAL_STOP_2'),
('-1557029','Never underestimate the other ethereal''s greed!','0','0','0','0','17244','xiraxis SAY_FINAL_STOP_3'),
('-1557030','He was right, you know. I''ll have to take that tag-line for my own... It''s not like he''ll have a use for it anymore!','0','0','0','1','17264','sha''heen SAY_XIRAXIS_DEAD'),
('-1557031','Thanks and good luck!','0','0','0','1','17265','sha''heen SAY_COMPLETE'),
('-1557032','%s checks to make sure his body is intact.','0','2','0','0','17228','sha''heen EMOTE_TELEPORT'),
('-1557033','You made it! Well done, $r. Now if you''ll excuse me, I have to get the rest of our crew inside.','0','0','0','1','17229','sha''heen SAY_SPAWN'),
('-1557034','%s expertly manipulates the control panel.','0','2','0','28','17230','sha''heen EMOTE_PANEL'),
('-1557035','Let''s not waste any time! Take anything that isn''t nailed down to the floor and teleport directly to Stormspire! Chop chop!','0','0','0','1','17231','sha''heen SAY_ESCORT_READY'),

('-1557036','I... I am free! Ethereum were planning on torturing me until I gave in to their requests. I fear that, had you not come along, I would have surely given in to their torment.','0','0','0','1','20633','Ambassador SAY_RELEASED_1'),
('-1557037','Know this, $r. Within Shaffar''s chambers at the end of these tombs rests a creature of unimaginable power. It is held in check by the power of the stasis chamber. You must return to Ameer before you venture further if you wish to destroy it!','0','0','0','1','20634','Ambassador SAY_RELEASED_2');

-- -1 558 000 AUCHENAI CRYPTS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1558000','You have defiled the resting place of our ancestors. For this offense, there can be but one punishment. It is fitting that you have come to a place of the dead... for you will soon be joining them.','10509','1','0','0','0','maladaar SAY_INTRO'),
('-1558001','Rise my fallen brothers. Take form and fight!','10512','1','0','0','15466','maladaar SAY_SUMMON'),
('-1558002','You will pay with your life!','10513','1','0','0','17647','maladaar SAY_AGGRO_1'),
('-1558003','There''s no turning back now!','10514','1','0','0','17648','maladaar SAY_AGGRO_2'),
('-1558004','Serve your penitence!','10515','1','0','0','17649','maladaar SAY_AGGRO_3'),
('-1558005','Let your mind be clouded.','10510','1','0','0','17644','maladaar SAY_ROAR'),
('-1558006','Stare into the darkness of your soul.','10511','1','0','0','17645','maladaar SAY_SOUL_CLEAVE'),
('-1558007','These walls will be your doom.','10516','1','0','0','0','maladaar SAY_SLAY_1'),
('-1558008','<laugh> Now, you''ll stay for eternity!','10517','1','0','0','0','maladaar SAY_SLAY_2'),
('-1558009','This is... where.. I belong...','10518','1','0','0','17646','maladaar SAY_DEATH'),

('-1558010','%s focuses on $N','0','3','0','0','21077','shirrak EMOTE_FOCUS');

-- -1 560 000 ESCAPE FROM DURNHOLDE (OLD HILLSBRAD)
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1560000','Thrall! You didn''t really think you would escape did you? You and your allies shall answer to Blackmoore - after I''ve had my fun!','10406','0','0','1','15909','skarloc SAY_ENTER'),

('-1560001','My magical power can turn back time to before Thrall''s death, but be careful. My power to manipulate time is limited.','0','0','0','0','16729','image of eronzion SAY_RESET_THRALL'),
('-1560002','I have set back the flow of time just once more. If you fail to prevent Thrall''s death, then all is lost.','0','0','0','0','16731','image of eronzion SAY_RESET_THRALL_LAST'),

('-1560003','What''s the meaning of this? GUARDS!','0','0','0','0','15891','armorer SAY_CALL_GUARDS'),
('-1560004','All that you know... will be undone.','0','0','0','0','19537','infinite dragon SAY_INFINITE_AGGRO_1'),
('-1560005','Let''s go.','0','0','0','0','15894','thrall hillsbrad SAY_TH_ARMORY2'),
('-1560006','%s startles the horse with a fierce yell!','0','2','0','5','16499','thrall hillsbrad EMOTE_TH_STARTLE_HORSE'),
('-1560007','I thought I saw something go into the barn.','0','0','0','0','15958','tarren mill lookout SAY_LOOKOUT_BARN_1'),
('-1560008','I didn''t see anything.','0','0','0','0','15968','tarren mill lookout SAY_PROTECTOR_BARN_2'),
('-1560009','%s tries to calm the horse down.','0','2','0','0','15970','thrall hillsbrad EMOTE_TH_CALM_HORSE'),
('-1560010','Something riled that horse. Let''s go!','0','0','0','0','15969','tarren mill lookout SAY_PROTECTOR_BARN_3'),
('-1560011','Taretha isn''t here. Let''s head into town.','0','0','0','0','15974','thrall hillsbrad SAY_TH_HEAD_TOWN'),
('-1560012','She''s not here.','0','0','0','0','15975','thrall hillsbrad SAY_TH_CHURCH_ENTER'),

('-1560013','Thrall! Come outside and face your fate!','10418','1','0','0','16016','epoch SAY_ENTER1'),
('-1560014','Taretha''s life hangs in the balance. Surely you care for her. Surely you wish to save her...','10419','1','0','0','16019','epoch SAY_ENTER2'),
('-1560015','Ah, there you are. I had hoped to accomplish this with a bit of subtlety, but I suppose direct confrontation was inevitable. Your future, Thrall, must not come to pass and so...you and your troublesome friends must die!','10420','1','0','0','16021','epoch SAY_ENTER3'),

('-1560016','Thrall''s trapped himself in the chapel. He can''t escape now.','0','0','0','0','15976','tarren mill lookout SAY_LOOKOUT_CHURCH'),
('-1560017','He''s here, stop him!','0','0','0','0','15986','tarren mill lookout SAY_LOOKOUT_INN'),
('-1560018','We have all the time in the world....','0','0','0','0','19536','infinite dragon SAY_INFINITE_AGGRO_2'),
('-1560019','You cannot escape us!','0','0','0','0','19538','infinite dragon SAY_INFINITE_AGGRO_3'),
('-1560020','Do not think you can win!','0','0','0','0','19534','infinite dragon SAY_INFINITE_AGGRO_4'),

('-1560021','REUSE_ME','0','0','0','0','0','REUSE_ME'),
('-1560022','REUSE_ME','0','0','0','0','0','REUSE_ME'),

('-1560023','Very well then. Let''s go!','10465','0','0','0','17622','thrall hillsbrad SAY_TH_START_EVENT_PART1'),
('-1560024','As long as we''re going with a new plan, I may aswell pick up a weapon and some armor.','0','0','0','0','15893','thrall hillsbrad SAY_TH_ARMORY'),
('-1560025','A rider approaches!','10466','0','0','0','15908','thrall hillsbrad SAY_TH_SKARLOC_MEET'),
('-1560026','I''ll never be chained again!','10467','1','0','0','17623','thrall hillsbrad SAY_TH_SKARLOC_TAUNT'),
('-1560027','Very well. Tarren Mill lies just west of here. Since time is of the essence...','10468','0','0','0','15916','thrall hillsbrad SAY_TH_START_EVENT_PART2'),
('-1560028','Let''s ride!','10469','0','0','1','15948','thrall hillsbrad SAY_TH_MOUNTS_UP'),
('-1560029','Taretha must be in the inn. Let''s go.','0','0','0','0','15977','thrall hillsbrad SAY_TH_CHURCH_END'),
('-1560030','Taretha! What foul magic is this?','0','0','0','0','15978','thrall hillsbrad SAY_TH_MEET_TARETHA'),
('-1560031','Who or what was that?','10470','0','0','1','16017','thrall hillsbrad SAY_TH_EPOCH_WONDER'),
('-1560032','No!','10471','0','0','5','6352','thrall hillsbrad SAY_TH_EPOCH_KILL_TARETHA'),
('-1560033','Goodbye, Taretha. I will never forget your kindness.','10472','0','0','0','16089','thrall hillsbrad SAY_TH_EVENT_COMPLETE'),
('-1560034','Things are looking grim...','10458','1','0','0','17610','thrall hillsbrad SAY_TH_RANDOM_LOW_HP1'),
('-1560035','I will fight to the last!','10459','1','0','0','17611','thrall hillsbrad SAY_TH_RANDOM_LOW_HP2'),
('-1560036','Taretha....','10460','1','0','0','17612','thrall hillsbrad SAY_TH_RANDOM_DIE1'),
('-1560037','A good day... to die.','10461','1','0','0','17613','thrall hillsbrad SAY_TH_RANDOM_DIE2'),
('-1560038','I have earned my freedom!','10448','0','0','0','17600','thrall hillsbrad SAY_TH_RANDOM_AGGRO1'),
('-1560039','This day is long overdue. Out of my way!','10449','0','0','0','17601','thrall hillsbrad SAY_TH_RANDOM_AGGRO2'),
('-1560040','I am a slave no longer!','10450','0','0','0','17602','thrall hillsbrad SAY_TH_RANDOM_AGGRO3'),
('-1560041','Blackmoore has much to answer for!','10451','0','0','0','17603','thrall hillsbrad SAY_TH_RANDOM_AGGRO4'),
('-1560042','You have forced my hand!','10452','0','0','0','17604','thrall hillsbrad SAY_TH_RANDOM_KILL1'),
('-1560043','It should not have come to this!','10453','0','0','0','17605','thrall hillsbrad SAY_TH_RANDOM_KILL2'),
('-1560044','I did not ask for this!','10454','0','0','0','17606','thrall hillsbrad SAY_TH_RANDOM_KILL3'),
('-1560045','I am truly in your debt, strangers.','10455','0','0','0','17607','thrall hillsbrad SAY_TH_LEAVE_COMBAT1'),
('-1560046','Thank you, strangers. You have given me hope.','10456','0','0','0','17608','thrall hillsbrad SAY_TH_LEAVE_COMBAT2'),
('-1560047','I will not waste this chance. I will seek out my destiny.','10457','0','0','0','17609','thrall hillsbrad SAY_TH_LEAVE_COMBAT3'),

('-1560048','I''m free! Thank you all!','0','0','0','0','16082','taretha SAY_TA_FREE'),
('-1560049','Thrall, you escaped!','0','0','0','0','15979','taretha SAY_TA_ESCAPED'),

('-1560050','That''s enough out of him.','0','0','0','0','15892','thrall hillsbrad SAY_TH_KILL_ARMORER'),
('-1560051','That spell should wipe their memories of us and what just happened. All they should remember now is what reality would be like without the attempted temporal interference. Well done. Thrall will journey on to find his destiny, and Taretha...','0','0','0','0','16087','erozion SAY_WIPE_MEMORY'),
('-1560052','Her fate is regrettably unavoidable.','0','0','0','0','16088','erozion SAY_ABOUT_TARETHA'),
('-1560053','They call you a monster. But they''re the monsters, not you. Farewell Thrall.','0','0','0','0','16090','taretha SAY_TA_FAREWELL'),

('-1560054','I''m glad you''re safe, Taretha. None of this would have been possible without your friends. They made all of this happen.','0','0','0','0','16083','thrall hillsbrad SAY_TR_GLAD_SAFE'),
('-1560055','Thrall, I''ve never met these people before in my life.','0','0','0','0','16085','taretha SAY_TA_NEVER_MET'),
('-1560056','Then who are these people?','0','0','0','0','16084','thrall hillsbrad SAY_TR_THEN_WHO'),
('-1560057','I believe I can explain everything to you two if you give me a moment of your time.','0','0','0','0','16086','erozion SAY_PRE_WIPE'),
('-1560058','You have done a great thing. Alas, the young warchief''s memory of these events must be as they originally were ... Andormu awaits you in the master''s lair.','0','0','0','0','0','erozion SAY_AFTER_WIPE');

-- -1 564 000 BLACK TEMPLE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1564000','You will die in the name of Lady Vashj!','11450','1','0','0','21085','SAY_AGGRO'),
('-1564001','Stick around!','11451','1','0','0','37696','SAY_NEEDLE1'),
('-1564002','I''ll deal with you later!','11452','1','0','0','21090','SAY_NEEDLE2'),
('-1564003','Your success was short lived!','11455','1','0','0','21091','SAY_SLAY1'),
('-1564004','Time for you to go!','11456','1','0','0','21092','SAY_SLAY2'),
('-1564005','Be''lanen dalorai!','11453','1','0','0','21088','SAY_SPECIAL1'),
('-1564006','Blood will flow!','11454','1','0','0','21087','SAY_SPECIAL2'),
('-1564007','Bal, lamer zhita!','11457','1','0','0','21095','SAY_ENRAGE1'),
('-1564008','Enough! Taste the full fury of High Warlord Naj''entus!','0','1','0','0','20922','SAY_ENRAGE2'),
('-1564009','Lord Illidan will... crush you!','11459','1','0','0','21093','SAY_DEATH'),
('-1564138','My patience has ran out! Die! Die!','11458','1','0','0','21096','SAY_UNK'),

('-1564010','Supremus acquires a new target!','0','3','0','0','21959','supremus EMOTE_NEW_TARGET'),
('-1564011','Supremus punches the ground in anger!','0','3','0','53','21019','supremus EMOTE_PUNCH_GROUND'),
('-1564012','The ground begins to crack open!','0','3','0','0','21018','supremus EMOTE_GROUND_CRACK'),

('-1564013','No! Not yet...','11386','1','0','0','21785','akama shade SAY_LOW_HEALTH'),
('-1564014','I will not last much longer...','11385','1','0','0','37877','akama shade SAY_DEATH'),
('-1564015','Come out from the shadows! I''ve returned to lead you against our true enemy! Shed your chains and raise your weapons against your Illidari masters!','0','1','0','397','21344','akama shade SAY_FREE_3'),
('-1564016','Hail our leader! Hail Akama!','0','1','0','0','21350','akama shade broken SAY_BROKEN_FREE_01'),
('-1564017','Hail Akama!','0','1','0','0','21348','akama shade broken SAY_BROKEN_FREE_02'),

('-1564018','You play, you pay.','11501','1','0','0','21766','shahraz SAY_TAUNT1'),
('-1564019','I''m not impressed.','11502','1','0','0','21767','shahraz SAY_TAUNT2'),
('-1564020','Enjoying yourselves?','11503','1','0','0','21768','shahraz SAY_TAUNT3'),
('-1564021','So... business or pleasure?','11504','1','0','0','21769','shahraz SAY_AGGRO'),
('-1564022','You seem a little tense...','11505','1','0','0','21770','shahraz SAY_SPELL1'),
('-1564023','Don''t be shy.','11506','1','0','0','21771','shahraz SAY_SPELL2'),
('-1564024','I''m all... yours.','11507','1','0','0','21772','shahraz SAY_SPELL3'),
('-1564025','Easy come, easy go.','11508','1','0','0','21774','shahraz SAY_SLAY1'),
('-1564026','So much for a happy ending.','11509','1','0','0','21776','shahraz SAY_SLAY2'),
('-1564027','Stop toying with my emotions!','11510','1','0','0','21357','shahraz SAY_ENRAGE'),
('-1564028','I wasn''t finished.','11511','1','0','0','21777','shahraz SAY_DEATH'),

('-1564029','Horde will... crush you.','11432','1','0','0','21740','bloodboil SOUND_AGGRO'),
('-1564030','Time to feast!','11433','1','0','0','21741','bloodboil SAY_SLAY1'),
('-1564031','More! I want more!','11434','1','0','0','21742','bloodboil SAY_SLAY2'),
('-1564032','Drink your blood. Eat your flesh.','11435','1','0','0','21743','bloodboil SAY_SPECIAL1'),
('-1564033','I hunger!','11436','1','0','0','24036','bloodboil SAY_SPECIAL2'),
('-1564034','<babbling>','11437','1','0','0','0','bloodboil SAY_ENRAGE1'),
('-1564035','I''ll rip the meat from your bones!','11438','1','0','0','21745','bloodboil SAY_ENRAGE2'),
('-1564036','Aaaahrg...','11439','1','0','0','0','bloodboil SAY_DEATH'),

('-1564037','I was the first you know. For me the wheel of death has spun many times. So much time has passed... I have a lot of catching up to do. ','11512','1','0','0','21098','teron SAY_INTRO'),
('-1564038','Vengeance is mine!','11513','1','0','0','21097','teron SAY_AGGRO'),
('-1564039','I have use for you!','11514','1','0','0','21099','teron SAY_SLAY1'),
('-1564040','It gets worse...','11515','1','0','0','21100','teron SAY_SLAY2'),
('-1564041','What are you afraid of?','11517','1','0','0','21102','teron SAY_SPELL1'),
('-1564042','Death really isn''t so bad.','11516','1','0','0','21101','teron SAY_SPELL2'),
('-1564043','Give in.','11518','1','0','0','21103','teron SAY_SPECIAL1'),
('-1564044','I have something for you...','11519','1','0','0','21104','teron SAY_SPECIAL2'),
('-1564045','YOU WILL SHOW THE PROPER RESPECT!','11520','1','0','0','21105','teron SAY_UNK'),
('-1564046','The wheel... spins... again.','11521','1','0','0','21106','teron SAY_DEATH'),

('-1564047','Pain and suffering are all that await you!','11415','1','0','0','21759','essence SUFF_SAY_FREED'),
('-1564048','Don''t leave me alone!','11416','1','0','0','21760','essence SUFF_SAY_AGGRO'),
('-1564049','Look at what you make me do!','11417','1','0','0','0','essence SUFF_SAY_SLAY1'),
('-1564050','I didn''t ask for this!','11418','1','0','0','21762','essence SUFF_SAY_SLAY2'),
('-1564051','The pain is only beginning...','11419','1','0','0','21763','essence SUFF_SAY_FRENZY'),
('-1564052','I don''t want to go back!','11420','1','0','0','21764','essence SUFF_SAY_RECAP'),
('-1564053','Now what do I do?','11421','1','0','0','21765','essence SUFF_SAY_AFTER'),
('-1564054','REUSE_ME','0','0','0','0','0','REUSE_ME'),

('-1564055','You can have anything you desire... for a price.','11408','1','0','0','21752','essence DESI_SAY_FREED'),
('-1564056','Fulfillment is at hand.','11409','1','0','0','21753','essence DESI_SAY_SLAY1'),
('-1564057','Yes... you''ll stay with us now...','11410','1','0','0','21754','essence DESI_SAY_SLAY2'),
('-1564058','Your reach exceeds your grasp.','11412','1','0','0','21756','essence DESI_SAY_SLAY3'),
('-1564059','Be careful what you wish for.','11411','1','0','0','21755','essence DESI_SAY_SPEC'),
('-1564060','I''ll be waiting...','11413','1','0','0','21757','essence DESI_SAY_RECAP'),
('-1564061','I won''t be far!','11414','1','0','0','21758','essence DESI_SAY_AFTER'),

('-1564062','Beware: I live!','11399','1','0','0','21746','essence ANGER_SAY_FREED'),
('-1564063','So... foolish.','11400','1','0','0','21747','essence ANGER_SAY_FREED2'),
('-1564064','<maniacal cackle>','11401','1','0','0','0','essence ANGER_SAY_SLAY1'),
('-1564065','Enough. No more.','11402','1','0','0','21748','essence ANGER_SAY_SLAY2'),
('-1564066','On your knees!','11403','1','0','0','21749','essence ANGER_SAY_SPEC'),
('-1564067','Beware, coward!','11405','1','0','0','21751','essence ANGER_SAY_BEFORE'),
('-1564068','I won''t... be... ignored.','11404','1','0','0','21750','essence ANGER_SAY_DEATH'),
('-1564142','%s seethes in anger!','0','1','0','0','21878','essence ANGER_SAY_SEETHE'),

('-1564069','You wish to test me?','11524','1','0','0','21698','council vera AGGRO'),
('-1564070','I have better things to do...','11422','1','0','0','21734','council gath AGGRO'),
('-1564071','Flee or die!','11482','1','0','0','21711','council mala AGGRO'),
('-1564072','Common... such a crude language. Bandal!','11440','1','0','0','21724','council zere AGGRO'),

('-1564073','Enough games!','11428','1','0','0','21960','council gath ENRAGE'),
('-1564074','You wish to kill me? You first!','11530','1','0','0','21707','council vera ENRAGE'),
('-1564075','For Quel''Thalas! For the Sunwell!','11488','1','0','0','21717','council mala ENRAGE'),
('-1564076','Sha''amoor sine menoor!','11446','1','0','0','0','council zere ENRAGE'),

('-1564077','Enjoy your final moments.','11426','1','0','0','21738','council gath SPECIAL1'),
('-1564078','You''re not caught up for this!','11528','1','0','0','0','council vera SPECIAL1'),
('-1564079','No second chances!','11486','1','0','0','21716','council mala SPECIAL1'),
('-1564080','Diel fin''al.','11444','1','0','0','21730','council zere SPECIAL1'),

('-1564081','You are mine!','11427','1','0','0','21739','council gath SPECIAL2'),
('-1564082','Anar''alah belore!','11529','1','0','0','20860','council vera SPECIAL2'),
('-1564083','I''m full of surprises!','11487','1','0','0','21719','council mala SPECIAL2'),
('-1564084','Sha''amoor ara mashal?','11445','1','0','0','21731','council zere SPECIAL2'),

('-1564085','Selama am''oronor!','11423','1','0','0','21735','council gath SLAY'),
('-1564086','Valiant effort!','11525','1','0','0','21699','council vera SLAY'),
('-1564087','My work is done.','11483','1','0','0','21712','council mala SLAY'),
('-1564088','Shorel''aran.','11441','1','0','0','21725','council zere SLAY'),

('-1564089','Well done!','11424','1','0','0','6038','council gath SLAY_COMT'),
('-1564090','A glorious kill!','11526','1','0','0','21701','council vera SLAY_COMT'),
('-1564091','As it should be!','11484','1','0','0','28383','council mala SLAY_COMT'),
('-1564092','Belesa menoor!','11442','1','0','0','21727','council zere SLAY_COMT'),

('-1564093','Lord Illidan... I...','11425','1','0','0','21737','council gath DEATH'),
('-1564094','You got... lucky.','11527','1','0','0','21703','council vera DEATH'),
('-1564095','Destiny... awaits.','11485','1','0','0','21715','council mala DEATH'),
('-1564096','Diel ma''ahn... orindel''o.','11443','1','0','0','21729','council zere DEATH'),

('-1564097','Akama. Your duplicity is hardly surprising. I should have slaughtered you and your malformed brethren long ago.','11463','1','0','0','20867','illidan SAY_ILLIDAN_SPEECH_1'),
('-1564098','We''ve come to end your reign, Illidan. My people, and all of Outland, shall be free!','11389','1','0','25','20893','akama(illidan) SAY_AKAMA_SPEECH_2'),
('-1564099','Boldly said. But I remain... unconvinced.','11464','1','0','6','20868','illidan SAY_ILLIDAN_SPEECH_3'),
('-1564100','The time has come! The moment is at hand!','11380','1','0','22','20894','akama(illidan) SAY_AKAMA_SPEECH_4'),
('-1564101','You are not prepared!','11466','1','0','406','20884','illidan SAY_ILLIDAN_SPEECH_5'),
('-1564102','Is this it, mortals? Is this all the fury you can muster?','11476','1','0','0','21068','illidan SAY_ILLIDAN_SPEECH_6'),
('-1564103','Their fury pales before mine, Illidan. We have some unsettled business between us.','11491','1','0','6','21070','maiev SAY_MAIEV_SPEECH_7'),
('-1564104','Maiev... How is it even possible?','11477','1','0','1','21069','illidan SAY_ILLIDAN_SPEECH_8'),
('-1564105','My long hunt is finally over. Today, Justice will be done!','11492','1','0','5','21071','maiev SAY_MAIEV_SPEECH_9'),
('-1564106','Feel the hatred of ten thousand years!','11470','1','0','0','21501','illidan SAY_FRENZY'),
('-1564107','It is finished. You are beaten.','11496','1','0','0','21507','maiev SAY_MAIEV_EPILOGUE_1'),
('-1564108','You have won... Maiev. But the huntress... is nothing without the hunt. You... are nothing... without me.','11478','1','0','0','21506','illidan SAY_ILLIDAN_EPILOGUE_2'),
('-1564109','He''s right. I feel nothing... I am... nothing.','11497','1','0','0','21508','maiev SAY_MAIEV_EPILOGUE_3'),
('-1564110','Farewell, champions.','11498','1','0','0','21509','maiev SAY_MAIEV_EPILOGUE_4'),
('-1564111','The Light will bless these dismal halls once again.... I swear it.','11387','1','0','0','21514','akama(illidan) SAY_AKAMA_EPILOGUE_5'),
('-1564112','I can feel your hatred.','11467','1','0','0','0','illidan SAY_TAUNT_1'),
('-1564113','Give in to your fear!','11468','1','0','0','30484','illidan SAY_TAUNT_2'),
('-1564114','You know nothing of power!','11469','1','0','0','21500','illidan SAY_TAUNT_3'),
('-1564115','Such arrogance!','11471','1','0','0','21338','illidan SAY_TAUNT_4'),
('-1564116','That is for Naisha!','11493','1','0','0','21489','maiev SAY_MAIEV_TAUNT_1'),
('-1564117','Bleed as I have bled!','11494','1','0','0','0','maiev SAY_MAIEV_TAUNT_2'),
('-1564118','There shall be no prison for you this time!','11495','1','0','0','22208','maiev SAY_MAIEV_TRAP'),
('-1564119','Meet your end, demon!','11500','1','0','0','0','maiev SAY_MAIEV_TAUNT_4'),
('-1564120','Be wary, friends. The Betrayer meditates in the court just beyond.','11388','0','0','0','20902','akama(illidan) SAY_AKAMA_BEWARE'),
('-1564121','Come, my minions. Deal with this traitor as he deserves!','11465','1','0','0','21251','illidan SAY_AKAMA_MINION'),
('-1564122','I will deal with these mongrels! Strike now, friends! Strike at the Betrayer!','11390','1','0','22','21250','akama(illidan) SAY_AKAMA_LEAVE'),
('-1564123','Who shall be next to taste my blades?!','11473','1','0','0','21499','illidan SAY_KILL1'),
('-1564124','This is too easy!','11472','1','0','0','21498','illidan SAY_KILL2'),
('-1564125','I will not be touched by rabble such as you!','11479','1','0','0','21252','illidan SAY_TAKEOFF'),
('-1564126','Behold the flames of Azzinoth!','11480','1','0','0','21275','illidan SAY_SUMMONFLAMES'),
('-1564127','Stare into the eyes of the Betrayer!','11481','1','0','0','21497','illidan SAY_EYE_BLAST'),
('-1564128','Behold the power... of the demon within!','11475','1','0','0','21066','illidan SAY_MORPH'),
('-1564129','You''ve wasted too much time mortals, now you shall fall!','11474','1','0','0','22036','illidan SAY_ENRAGE'),

('-1564130','Broken of the Ashtongue tribe, your leader speaks!','0','1','0','15','21342','akama(shade) SAY_FREE_1'),

('-1564131','This door is all that stands between us and the Betrayer.  Stand aside, friends.','0','0','0','1','21563','akama(illidan) SAY_OPEN_DOOR_1'),
('-1564132','I cannot do this alone...','0','0','0','274','21548','akama(illidan) SAY_OPEN_DOOR_2'),
('-1564133','You are not alone, Akama.','0','0','0','0','21545','spirit_Udalo SAY_OPEN_DOOR_3'),
('-1564134','Your people will always be with you!','0','0','0','0','21546','spirit_Olum SAY_OPEN_DOOR_4'),
('-1564135','The Betrayer no longer holds sway over us.  His dark magic over the Ashtongue soul has been destroyed!','0','1','0','1','21343','akama shade SAY_FREE_2'),
('-1564136','I thank you for your aid, brothers.  Our people will be redeemed!','0','0','0','66','21554','akama(illidan) SAY_OPEN_DOOR_5'),

('-1564137','You hear a loud rumble of metal grinding on stone...','0','7','0','0','18600','Black Temple Trigger - Open Najentus Door'),
-- -1564138 taken on Najentus
('-1564139','The door to The Den of Mortal Delights has opened.','0','7','0','0','21494','Black Temple Trigger - Open Mother Door'),
('-1564140','Let us finish what we''ve started.  I will lead you to Illidan''s abode once you''ve recovered your strength.','0','0','0','1','21520','akama(illidan) SAY_AKAMA_COUNCIL_DEAD'),
('-1564141','%s falls to the floor.','0','7','0','0','21317','Maiev');
-- -1564142 taken on Essence of Anger


-- -1 565 000 GRUUL'S LAIR
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1565000','Gronn are the real power in Outland!','11367','1','0','0','20066','maulgar SAY_AGGRO'),
('-1565001','You will not defeat the hand of Gruul!','11368','1','0','0','20067','maulgar SAY_ENRAGE'),
('-1565002','You won''t kill next one so easy.','11369','1','0','0','20068','maulgar SAY_OGRE_DEATH1'),
('-1565003','Pah! Does not prove anything!','11370','1','0','0','20069','maulgar SAY_OGRE_DEATH2'),
('-1565004','I''m not afraid of you.','11371','1','0','0','33611','maulgar SAY_OGRE_DEATH3'),
('-1565005','Good, now you fight me!','11372','1','0','0','20071','maulgar SAY_OGRE_DEATH4'),
('-1565006','You not so tough after all!','11373','1','0','0','20072','maulgar SAY_SLAY1'),
('-1565007','Aha-ha ha ha!','11374','1','0','0','0','maulgar SAY_SLAY2'),
('-1565008','Maulgar is king!','11375','1','0','0','20074','maulgar SAY_SLAY3'),
('-1565009','Gruul... will crush you.','11376','1','0','0','20075','maulgar SAY_DEATH'),

('-1565010','Come.... and die.','11355','1','0','0','20116','gruul SAY_AGGRO'),
('-1565011','Scurry.','11356','1','0','0','20117','gruul SAY_SLAM1'),
('-1565012','No escape.','11357','1','0','0','20118','gruul SAY_SLAM2'),
('-1565013','Stay.','11358','1','0','0','20119','gruul SAY_SHATTER1'),
('-1565014','Beg for life.','11359','1','0','0','20120','gruul SAY_SHATTER2'),
('-1565015','No more.','11360','1','0','0','20121','gruul SAY_SLAY1'),
('-1565016','Unworthy.','11361','1','0','0','6197','gruul SAY_SLAY2'),
('-1565017','Die.','11362','1','0','0','20123','gruul SAY_SLAY3'),
('-1565018','Aaargh...','11363','1','0','0','0','gruul SAY_DEATH'),
('-1565019','%s grows in size!','0','2','0','0','18790','gruul EMOTE_GROW'),
('-1565020','%s roars!','0','2','0','0','16402','gruul EMOTE_SHATTER');

-- -1 568 000 ZUL'AMAN
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1568000','Spirits of da wind be your doom!','12031','1','0','0','23590','janalai SAY_AGGRO'),
('-1568001','I burn ya now!','12032','1','0','0','23589','janalai SAY_FIRE_BOMBS'),
('-1568002','Where ma hatcha? Get to work on dem eggs!','12033','1','0','0','23528','janalai SAY_SUMMON_HATCHER'),
('-1568003','I show you strength... in numbers.','12034','1','0','0','22838','janalai SAY_ALL_EGGS'),
('-1568004','You done run outta time!','12035','1','0','0','23588','janalai SAY_BERSERK'),
('-1568005','It all be over now, mon!','12036','1','0','0','23585','janalai SAY_SLAY_1'),
('-1568006','Tazaga-choo!','12037','1','0','0','23586','janalai SAY_SLAY_2'),
('-1568007','Zul''jin... got a surprise for you....','12038','1','0','0','23587','janalai SAY_DEATH'),
('-1568008','Come, strangers. The spirit of the dragonhawk hot be hungry for worthy souls.','12039','1','0','0','23314','janalai SAY_EVENT_STRANGERS'),
('-1568009','Come, friends. Your bodies gonna feed ma hatchlings, and your souls are going to feed me with power!','12040','1','0','0','23313','janalai SAY_EVENT_FRIENDS'),

('-1568010','Get da move on, guards! It be killin'' time!','12066','1','0','51','22144','nalorakk SAY_WAVE1_AGGRO'),
('-1568011','Guards, go already! Who you more afraid of, dem... or me?','12067','1','0','51','22146','nalorakk SAY_WAVE2_STAIR1'),
('-1568012','Ride now! Ride out dere and bring me back some heads!','12068','1','0','51','22151','nalorakk SAY_WAVE3_STAIR2'),
('-1568013','I be losin'' me patience! Go on: make dem wish dey was never born!','12069','1','0','51','22155','nalorakk SAY_WAVE4_PLATFORM'),
('-1568014','What could be better than servin'' da bear spirit for eternity? Come closer now. Bring your souls to me!','12078','1','0','0','23305','nalorakk SAY_EVENT1_SACRIFICE'),
('-1568015','Don''t be delayin'' your fate. Come to me now. I make your sacrifice quick.','12079','1','0','0','23306','nalorakk SAY_EVENT2_SACRIFICE'),
('-1568016','You be dead soon enough!','12070','1','0','0','23166','nalorakk SAY_AGGRO'),
('-1568017','I bring da pain!','12071','1','0','0','23167','nalorakk SAY_SURGE'),
('-1568018','You call on da beast, you gonna get more dan you bargain for!','12072','1','0','0','23168','nalorakk SAY_TOBEAR'),
('-1568019','Make way for Nalorakk!','12073','1','0','0','23169','nalorakk SAY_TOTROLL'),
('-1568020','You had your chance, now it be too late!','12074','1','0','0','23170','nalorakk SAY_BERSERK'),
('-1568021','Mua-ha-ha! Now whatchoo got to say?','12075','1','0','0','23171','nalorakk SAY_SLAY1'),
('-1568022','Da Amani gonna rule again!','12076','1','0','0','23172','nalorakk SAY_SLAY2'),
('-1568023','I... be waitin'' on da udda side....','12077','1','0','0','23173','nalorakk SAY_DEATH'),

('-1568024','My eagles gonna bear your spirits to me. Your sacrifices not gonna be in vain....','12122','1','0','0','23307','akilzon SAY_EVENT1'),
('-1568025','Your death gonna be quick, strangers. You shoulda never come to this place....','12123','1','0','0','23308','akilzon SAY_EVENT2'),
('-1568026','I be da predator! You da prey...','12013','1','0','0','23235','akilzon SAY_AGGRO'),
('-1568027','Feed, me bruddahs!','12014','1','0','0','23931','akilzon SAY_SUMMON'),
('-1568028','Come, and join me bruddahs!','12015','1','0','0','0','akilzon SAY_SUMMON_ALT'),
('-1568029','All you be doing is wasting my time!','12016','1','0','0','0','akilzon SAY_ENRAGE'),
('-1568030','Ya got nothin''!','12017','1','0','0','23234','akilzon SAY_SLAY1'),
('-1568031','Stop your cryin''!','12018','1','0','0','23233','akilzon SAY_SLAY2'),
('-1568032','You can''t... kill... me spirit!','12019','1','0','0','23232','akilzon SAY_DEATH'),
('-1568033','An Electrical Storm Appears!','0','2','0','0','23603','akilzon EMOTE_STORM'),

('-1568034','Get on ya knees and bow.... to da fang and claw!','12020','1','0','0','23612','halazzi SAY_AGGRO'),
('-1568035','I fight wit'' untamed spirit....','12021','1','0','0','22963','halazzi SAY_SPLIT'),
('-1568036','Spirit, come back to me!','12022','1','0','0','22964','halazzi SAY_MERGE'),
('-1568037','Me gonna carve ya now!','12023','1','0','0','23615','halazzi SAY_SABERLASH1'),
('-1568038','You gonna leave in pieces!','12024','1','0','0','23616','halazzi SAY_SABERLASH2'),
('-1568039','Whatch you be doing? Pissin'' yourselves...','12025','1','0','0','0','halazzi SAY_BERSERK'),
('-1568040','You can''t fight da power!','12026','1','0','0','23613','halazzi SAY_KILL1'),
('-1568041','You gonna fail!','12027','1','0','0','0','halazzi SAY_KILL2'),
('-1568042','Chaga... choka''jinn.','12028','1','0','0','23611','halazzi SAY_DEATH'),
('-1568043','Come, fools. Fill ma empty cages...','12029','1','0','0','23309','halazzi SAY_EVENT1'),
('-1568044','I be waitin, strangers. Your deaths gonna make me stronger!','12030','1','0','0','23310','halazzi SAY_EVENT2'),

('-1568045','Da shadow gonna fall on you....','12041','1','0','0','23591','malacrass SAY_AGGRO'),
('-1568046','Ya don''t kill me yet, ya don''t get another chance!','12042','1','0','0','0','malacrass SAY_ENRAGE'),
('-1568047','Dis a nightmare ya don'' wake up from!','12043','1','0','0','23593','malacrass SAY_KILL1'),
('-1568048','Azzaga choogo zinn!','12044','1','0','0','23594','malacrass SAY_KILL2'),
('-1568049','Your will belong ta me now!','12045','1','0','0','23595','malacrass SAY_SOUL_SIPHON'),
('-1568050','Darkness comin'' for you....','12046','1','0','0','23596','malacrass SAY_DRAIN_POWER'),
('-1568051','Your soul gonna bleed!','12047','1','0','0','23597','malacrass SAY_SPIRIT_BOLTS'),
('-1568052','It not gonna make no difference.','12048','1','0','0','23598','malacrass SAY_ADD_DIED1'),
('-1568053','You gonna die worse dan him.','12049','1','0','0','23599','malacrass SAY_ADD_DIED2'),
('-1568054','Dat no bodda me.','12050','1','0','0','23600','malacrass SAY_ADD_DIED3'),
('-1568055','Dis not... da end for me!','12051','1','0','0','23601','malacrass SAY_DEATH'),

('-1568056','Everybody always wanna take from us. Now we gonna start takin'' back. Anybody who get in our way...gonna drown in dey own blood! Da Amani empire be back now...seekin'' vengeance. And we gonna start wit'' you.','12090','6','0','0','0','zuljin SAY_INTRO'),
('-1568057','Nobody badduh dan me!','12091','1','0','0','0','zuljin SAY_AGGRO'),
('-1568058','Got me some new tricks... like me brudda bear....','12092','1','0','0','23175','zuljin SAY_BEAR_TRANSFORM'),
('-1568059','Dere be no hidin'' from da eagle!','12093','1','0','0','23176','zuljin SAY_EAGLE_TRANSFORM'),
('-1568060','Let me introduce you to me new bruddas: fang and claw!','12094','1','0','0','23177','zuljin SAY_LYNX_TRANSFORM'),
('-1568061','Ya don'' have to look to da sky to see da dragonhawk!','12095','1','0','0','23178','zuljin SAY_DRAGONHAWK_TRANSFORM'),
('-1568062','Fire kill you just as quick!','12096','1','0','0','23179','zuljin SAY_FIRE_BREATH'),
('-1568063','You too slow! Me too strong!','12097','1','0','0','0','zuljin SAY_BERSERK'),
('-1568064','Da Amani de chuka!','12098','1','0','0','23181','zuljin SAY_KILL1'),
('-1568065','Lot more gonna fall like you!','12099','1','0','0','0','zuljin SAY_KILL2'),
('-1568066','Mebbe me fall... but da Amani empire... never gonna die.','12100','1','0','0','23183','zuljin SAY_DEATH'),

('-1568067','Zul''jin got a surprise for ya...','12052','6','0','0','23587','zulaman SAY_INST_RELEASE'),
('-1568068','Da spirits gonna feast today! Begin da ceremonies, sacrifice da prisoners... make room for our new guests!','12053','6','0','0','23253','zulaman SAY_INST_BEGIN'),
('-1568069','Take your pick, trespassers! Any of ma priests be happy to accommodate ya.','12054','6','0','0','23316','zulaman SAY_INST_PROGRESS_1'),
('-1568070','Don''t be shy. Thousands have come before you. Ya not be alone in your service.','12055','6','0','0','23317','zulaman SAY_INST_PROGRESS_2'),
('-1568071','Ya gonna fail, strangers. Many try before you, but dey only make us stronger!','12056','6','0','0','23318','zulaman SAY_INST_PROGRESS_3'),
('-1568072','Your efforts was in vain, trespassers. The rituals nearly be complete.','12057','6','0','0','23326','zulaman SAY_INST_WARN_1'),
('-1568073','Soon da cages gonna be empty, da sacrifices be complete, and you gonna take dere places.','12058','6','0','0','23327','zulaman SAY_INST_WARN_2'),
('-1568074','Time be running low, strangers. Soon you gonna join da souls of dem ya failed to save.','12059','6','0','0','23328','zulaman SAY_INST_WARN_3'),
('-1568075','Make haste, ma priests! Da rituals must not be interrupted!','12060','6','0','0','23329','zulaman SAY_INST_WARN_4'),
('-1568076','Ya make a good try... but now you gonna join da ones who already fall.','12061','6','0','0','23333','zulaman SAY_INST_SACRIF1'),
('-1568077','Ya not do too bad.... Your efforts delay da inevitable for a small time. Come to me now. Ya prove yourselves worthy offerings.','12062','6','0','0','23335','zulaman SAY_INST_SACRIF2'),
('-1568078','Watch now. Every offering gonna strengthen our ties to da spirit world. Soon, we gonna be unstoppable!','12065','6','0','0','23334','zulaman SAY_INST_COMPLETE'),

('-1568079','Suit yourself. At least five of you must assist me if we''re to get inside. Follow me....','0','1','0','0','24294','harrison SAY_START'),
('-1568080','According to my calculations, if enough of us bang the gong at once the seal on these doors will break and we can enter.','0','1','0','0','24295','harrison SAY_AT_GONG'),
('-1568081','I''ve researched this site extensively and I won''t allow any dim-witted treasure hunters to swoop in and steal what belongs in a museum. I''ll lead this charge.','0','1','0','0','23241','harrison SAY_OPEN_ENTRANCE'),

('-1568082','%s absorbs the essence of the bear spirit!','0','2','0','0','22410','zuljin EMOTE_BEAR_SPIRIT'),
('-1568083','%s absorbs the essence of the eagle spirit!','0','2','0','0','22432','zuljin EMOTE_EAGLE_SPIRIT'),
('-1568084','%s absorbs the essence of the lynx spirit!','0','2','0','0','22433','zuljin EMOTE_LYNX_SPIRIT'),
('-1568085','%s absorbs the essence of the dragonhawk spirit!','0','2','0','0','22435','zuljin EMOTE_DRAGONHAWK_SPIRIT'),

('-1568086','In fact, it would be best if you just stay here. You''d only get in my way....','0','1','0','0','23242','harrison SAY_OPEN_ENTRANCE_2'),

('-1568087','More intruders! Sound da alarm!','12104','1','0','25','23252','amanishi guardian SAY_SOUND_ALARM'),

('-1568088','Akil''zon, the invaders approach!','0','1','0','5','22971','amanishi lookout SAY_START_GAUNTLET'),

('-1568089','Hallooo.... You just gunna sit down there an'' pat yerselves on the back? What kinda rescuers are ya?','2716','1','0','1','22967','harkor SAY_HARKOR_HELP_1'),
('-1568090','Pssst! Up ''ere, in the cage. Come and lemme get a look at yeh.','2716','1','0','1','22966','harkor SAY_HARKOR_HELP_2'),
('-1568091','So.... How''s about one ''a ya stroll on up ''ere an'' help me out, eh?','2716','1','0','1','22968','harkor SAY_HARKOR_HELP_3'),
('-1568092','Yoohoo. Over here... the trapped dwarf. Come on, lemme have a word with yeh.','2716','1','0','1','22969','harkor SAY_HARKOR_HELP_4'),
('-1568093','Ahh, freedom!','2725','0','0','4','22994','harkor SAY_HARKOR_EVENT_1'),
('-1568094','A''course it only be fair that I reward yeh fer me rescue. Now where''d them savages put me satchel?','0','0','0','1','22995','harkor SAY_HARKOR_EVENT_2'),
('-1568095','Don''t be shy, now. Help yerselves. If it weren''t fer yer help, them basterds''d be shrinkin'' me head right about now.','0','0','0','1','22996','harkor SAY_HARKOR_EVENT_3'),

('-1568096','You kill the trollbear, mon? It be safe for Tanzar to come out now?','2850','1','0','1','23011','tanzar SAY_TANZAR_HELP_1'),
('-1568097','Hey mon! Don''t ya be forgettin'' about Tanzar over here.','2850','1','0','1','23010','tanzar SAY_TANZAR_HELP_2'),
('-1568098','Hello out there. Anybody be hearin'' Tanzar?','2850','1','0','1','23012','tanzar SAY_TANZAR_HELP_3'),
('-1568099','Pleaase! Someone save poor ol'' Tanzar.','2850','1','0','1','23014','tanzar SAY_TANZAR_HELP_4'),
('-1568100','Wahoo! Tanzar be free!','2859','0','0','4','23027','tanzar SAY_TANZAR_EVENT_1'),
('-1568101','What luck! Here be one of Tanzar''s things....','0','0','0','0','23031','tanzar SAY_TANZAR_EVENT_2'),
('-1568102','Hrmm... left, right, wiggle, press....','0','0','0','0','23034','tanzar SAY_TANZAR_EVENT_3'),
('-1568103','A little deeper now - almost got it....','0','0','0','0','23035','tanzar SAY_TANZAR_EVENT_3_ALT'),
('-1568104','There now. It be a gift to you, to celebrate Tanzar''s great escape!','0','0','0','1','23032','tanzar SAY_TANZAR_EVENT_4'),

('-1568105','My liberators! Come down and speak with Kraz.','2692','1','0','1','23051','kraz SAY_KRAZ_HELP_1'),
('-1568106','Kraz must meet the slayers of the great troll-beast!','2692','1','0','1','23050','kraz SAY_KRAZ_HELP_2'),
('-1568107','Come to Kraz. Accept my thanks.','2692','1','0','1','23049','kraz SAY_KRAZ_HELP_3'),
('-1568108','Kraz lives!','2701','0','0','4','23048','kraz SAY_KRAZ_EVENT_1'),
('-1568109','Now, where have they put Kraz''s things?','0','0','0','6','23115','kraz SAY_KRAZ_EVENT_2'),
('-1568110','Stand clear....','0','0','0','0','23110','kraz SAY_KRAZ_EVENT_3'),
('-1568111','Please, friends, accept this offering with Kraz''s thanks.','6364','0','0','66','23111','kraz SAY_KRAZ_EVENT_4'),

('-1568112','Wowee! It was amazing how you beat that big bad troll into the ground like that and it''s too bad I can''t reach the handle of this cage or I would have jumped in and blown him to bits to get him back for killing all of the other poor prisoners and scaring me so badly and I feel bad for asking you for any more help but could you maybe come over and let me out of here?','0','1','0','21','23156','ashli SAY_ASHLI_HELP_1'),
('-1568113','Ahem... so, I most certainly don''t blame you for taking your time and catching your breath and congratulating yourselves for such heroism after such a long and hard-fought battle with that horrible monster just so long as when you''re finished you don''t forget about me over here because even though I''m small I think I can still find some way to repay you for your amazing bravery....','0','1','0','1','23158','ashli SAY_ASHLI_HELP_2'),
('-1568114','So... as much as I hate to complain after being saved from a horrible death I just thought you should know that I''m still over here stuck in one of these messy cages and it would really be great if you could come on over and let me out so I could help you celebrate your amazing victory over that big crazy scary troll-cat... thing.','0','1','0','1','23159','ashli SAY_ASHLI_HELP_3'),
('-1568115','I''m sure you all must be in a big hurry and probably have some more big dangerous monsters to kill but if you would just take a moment and come help me out of this cage before rushing off it would make me so happy and excited that I would barely be able to contain myself so if you get a moment I''ll just be over here... waiting... patiently....','0','1','0','1','23160','ashli SAY_ASHLI_HELP_4'),
('-1568116','Weeee! I''m alive, I''m alive, I''m alive! Oh, I hope it''s not too forward of me to tell you that I think I love you all and I hope I can find my bag because you certainly deserve a little something extra, you know, for the effort and all that you must have went through to save me...','0','0','0','4','23184','ashli SAY_ASHLI_EVENT_1'),
('-1568117','Hmm, maybe it''s this one - oh by the way, did I ever tell you about the time Fudgerick Two-toots snuck up on me while I was casting a fireball? We call him two-toots because, well, nevermind that, it was just terrible timing, and momma was soo mad - my eyebrows are just now growing back....','0','0','0','0','23185','ashli SAY_ASHLI_EVENT_2'),
('-1568118','Nope, not in that one either - well, it must be around here somewhere, by the way, did I mention how wonderful it is to be out of that stinky cage? It was almost impossible to run around in there but I did anyway but sometimes I got going so fast that I ended up walking on the walls of the  cage so finally I stopped trying and just turned around in circles....','2691','0','0','0','23206','ashli SAY_ASHLI_EVENT_3'),
('-1568119','There''s my bag! At last I''ve found it and now it''s my gift to you and I truly hope you like it because I want you to all remember this wonderful moment for as long as you live and I hope that when you think of it you''ll smile inside because I know I''ll never forget it in a million years and I can''t wait to tell momma all about it when I get back home.... Do you like my earrings?','0','0','0','0','23207','ashli SAY_ASHLI_EVENT_4'),

('-1568120','Invaders! Sound the alarm!','0','1','0','0','23109','amanishi scout SAY_ALARM'),

('-1568121','%s shimmers and begins to fade away...','0','2','0','0','23604','zuljin EMOTE_FADE_AWAY'),

('-1568122','Greetings, kind stranger, and thank you for your selfless act.','6378','0','0','0','23346','mannuth SAY_MANNUTH_1'),
('-1568123','These heathens have robbed me of my belongings, but I can offer you this charm I took from my captors.','0','0','0','0','23347','mannuth SAY_MANNUTH_2'),
('-1568124','May it serve you well, $n.','0','0','0','0','23369','mannuth SAY_MANNUTH_3'),
('-1568125','Farewell.','6379','0','0','0','23348','mannuth SAY_MANNUTH_4'),

('-1568126','Hmm, thank you stranger.','6414','0','0','0','23373','deez SAY_DEEZ_1'),
('-1568127','I fear I have nothing but this old chest I discovered here. You''re welcome to it.','0','0','0','0','23374','deez SAY_DEEZ_2'),
('-1568128','I hope you find its contents... useful.','0','0','0','0','23375','deez SAY_DEEZ_3'),
('-1568129','Until we next meet....','6415','0','0','0','23376','deez SAY_DEEZ_4'),

('-1568130','Oooh! It''s about time someone rescued me....','9635','0','0','0','23379','galathryn SAY_GALATHRYN_1'),
('-1568131','I suppose I should be grateful. Please, take this. I want nothing to remind me of this place.','0','0','0','0','23380','galathryn SAY_GALATHRYN_2'),
('-1568132','I took it from one of these trolls as I was captured. Maybe you can find a way to use it against them.','0','0','0','0','23381','galathryn SAY_GALATHRYN_3'),
('-1568133','I''ve had quite enough of this place. Goodbye.','9636','0','0','0','23382','galathryn SAY_GALATHRYN_4'),

('-1568134','Ahhh, finally!','6134','0','0','0','23383','adarrah SAY_ADARRAH_1'),
('-1568135','Say, I found this charm just before they caught me. Maybe it''ll do you some good here....','0','0','0','0','23384','adarrah SAY_ADARRAH_2'),
('-1568136','I hope it helps. These vile beasts simply must be stopped!','0','0','0','0','23385','adarrah SAY_ADARRAH_3'),
('-1568137','Good luck to you!','6135','0','0','0','23386','adarrah SAY_ADARRAH_4'),

('-1568138','Well, now. What a pleasant turn of events.','0','0','0','0','23387','darwen SAY_DARWEN_1'),
('-1568139','It''s most fortunate for you that you rescued me, and not one of these other poor sots.','0','0','0','0','23388','darwen SAY_DARWEN_2'),
('-1568140','Please accept this payment, as well as my gratitude, for your trouble.','0','0','0','0','23389','darwen SAY_DARWEN_3'),
('-1568141','Be well, $n.','0','0','0','0','23390','darwen SAY_DARWEN_4'),

('-1568142','Woohoo! I''m saved! I thank you, $n.','6129','0','0','0','23391','fudgerick SAY_FUDGERICK_1'),
('-1568143','Oh, my. What IS that smell - oh wait, heh, whoops! Guess my senses will take a little time to get readjusted.','0','0','0','0','23392','fudgerick SAY_FUDGERICK_2'),
('-1568144','My apologies, friends. Here, now. This should help to cover the expenses involved in my rescue.','0','0','0','0','23393','fudgerick SAY_FUDGERICK_3'),
('-1568145','See ya later!','6126','0','0','113','23394','fudgerick SAY_FUDGERICK_4'),

('-1568146','Oh, can it be? I... I''m free of that hideous curse?','0','0','0','0','23401','gunter SAY_GUNTER_1'),
('-1568147','Words cannot express my gratitude, $n. Thank you for your kindness','0','0','0','0','23402','gunter SAY_GUNTER_2'),
('-1568148','As you can see, I have but little after these savages got hold of me, but you''re welcome to what I have. Come, have a look.','0','0','0','0','23403','gunter SAY_GUNTER_3'),
('-1568149','I must go now, before I get myself caught again. If you need anything, I won''t be far.','0','0','0','0','23404','gunter SAY_GUNTER_4'),

('-1568150','Mmmm, flies! Even better now that I can taste them.','0','0','0','0','23405','kyren SAY_KYREN_1'),
('-1568151','Nice to be back among the unliving, thanks to you.','0','0','0','0','23406','kyren SAY_KYREN_2'),
('-1568152','Perhaps I am able to offer something of interest to you? Come, see if there''s anything you like.','0','0','0','0','23407','kyren SAY_KYREN_3'),
('-1568153','I must be off now, $n. See you around.','0','0','0','0','23408','kyren SAY_KYREN_4'),

('-1568154','Weee! I''m a girl again!','0','0','0','0','23411','mitzi SAY_MITZI_1'),
('-1568155','Thank you for rescuing me. I simply hate being a yucky frog!','0','0','0','0','23412','mitzi SAY_MITZI_2'),
('-1568156','I''m really not supposed to talk to strangers, but you''ve been so nice to me. Here''s a special something I found just before I was kidnapped.','0','0','0','0','23413','mitzi SAY_MITZI_3'),
('-1568157','I should go now. Be careful!','0','0','0','0','23414','mitzi SAY_MITZI_4'),

('-1568158','Aww, nuts! You''ve ruined everything!','0','0','0','0','23417','christian SAY_CHRISTIAN_1'),
('-1568159','Do you have any idea how neat it is to hop that far?','0','0','0','0','23418','christian SAY_CHRISTIAN_2'),
('-1568160','Oh well, I was starting to miss my pals anyway - they''re not gunna believe this! Oh, here - you can have this, I found it playin'' hide and seek.','0','0','0','0','23419','christian SAY_CHRISTIAN_3'),
('-1568161','Let''s see if I can still make that sound....','0','0','0','0','23420','christian SAY_CHRISTIAN_4'),

('-1568162','Hrmmph. Thanks all the same, but I didn''t need any help.','0','0','0','0','23421','brennan SAY_BRENNAN_1'),
('-1568163','I was working on a cantrip that would counteract my predicament and I was very close to finding a <CROOAAK>... Oh my!','8353','0','0','0','23422','brennan SAY_BRENNAN_2'),
('-1568164','Well, I suppose I do owe you a little something - here, take one of these. I''ve got lots.','0','0','0','0','23423','brennan SAY_BRENNAN_3'),
('-1568165','You may want to consider leaving this place. I think being transformed into a frog is one of the better things that can happen to one here.','0','0','0','0','23424','brennan SAY_BRENNAN_4'),

('-1568166','Oh! Back to my beautiful self? How lovely! I thought I would be a frog forever.','0','0','0','0','23425','hollee SAY_HOLLEE_1'),
('-1568167','It was just terrible. I was all slimy, and I kept licking my eyes!','0','0','0','0','23426','hollee SAY_HOLLEE_1'),
('-1568168','Thank goodness you came along when you did. Please, take one of these troll boxes to remember me by.','0','0','0','0','23427','hollee SAY_HOLLEE_1'),
('-1568169','I never want to see this nasty pond again. Thank you, $n. I''ll never forget you!','0','0','0','0','23428','hollee SAY_HOLLEE_1'),

('-1568170','Mojo leaps into $n''s pocket!','0','2','0','0','23485','forest frog SAY_MOJO'),

('-1568171','Mua-ha-ha!','0','0','0','0','22145','Nalorakk - Say Run to next wave'),
('-1568172','%s transforms into a bear!','0','0','0','0','24263','Nalorakk - To Bear emote');

-- -1 574 000 UTGARDE KEEP
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1574000','Your blood is mine!','13221','1','0','0','22736','keleseth SAY_AGGRO'),
('-1574001','Not so fast.','13222','1','0','0','22734','keleseth SAY_FROSTTOMB'),
('-1574002','Aranal, lidel! Their fate shall be yours!','13224','1','0','0','22729','keleseth SAY_SKELETONS'),
('-1574003','Darkness waits!','13223','1','0','0','29591','keleseth SAY_KILL'),
('-1574004','I join... the night.','13225','1','0','0','29592','keleseth SAY_DEATH'),

('-1574005','I''ll paint my face with your blood!','13207','1','0','0','29974','ingvar SAY_AGGRO_FIRST'),
('-1574006','I return! A second chance to carve out your skull!','13209','1','0','0','29977','ingvar SAY_AGGRO_SECOND'),
('-1574007','My life for the... death god!','13213','1','0','0','29976','ingvar SAY_DEATH_FIRST'),
('-1574008','No! I can do... better! I can...','13211','1','0','0','29979','ingvar SAY_DEATH_SECOND'),
('-1574009','Mjul orm agn gjor!','13212','1','0','0','29978','ingvar SAY_KILL_FIRST'),
('-1574010','I am a warrior born!','13214','1','0','0','29975','ingvar SAY_KILL_SECOND'),

('-1574011','REUSE ME','0','0','0','0','0','REUSE ME'),

('-1574012','Not... over... yet.','13230','1','0','0','29534','skarvald SAY_SKA_DEATH'),
('-1574013','A warrior''s death.','13231','1','0','0','29535','skarvald SAY_SKA_DEATH_REAL'),

('-1574014','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1574015','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1574016','REUSE ME','0','0','0','0','0','REUSE ME'),

('-1574017','See... you... soon.','13200','1','0','0','29529','dalronn SAY_DAL_DEATH'),
('-1574018','There''s no... greater... glory.','13201','1','0','0','29530','dalronn SAY_DAL_DEATH_REAL'),

('-1574019','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1574020','REUSE ME','0','0','0','0','0','REUSE ME'),

('-1574021','%s casts Frost Tomb on $N','0','3','0','0','27152','keleseth EMOTE_FROST_TOMB'),

('-1574022','%s roars!','0','3','0','0','16402','ingvar EMOTE_ROAR'),
('-1574023','Ingvar! Your pathetic failure will serve as a warning to all... you are damned! Arise and carry out the masters will!','13754','1','0','0','29980','annhylde REZZ'),

('-1574024','%s rises from the floor!','0','2','0','0','26607','keleseth EMOTE_RISE_SKELETON');

-- -1 575 000 UTGARDE PINNACLE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1575000','My liege! I have done as you asked, and now beseech you for your blessing!','13856','1','0','1','29684','svala SAY_INTRO_1'),
('-1575001','Your sacrifice is a testament to your obedience. Indeed you are worthy of this charge. Arise, and forever be known as Svala Sorrowgrave!','14732','1','0','1','29688','svala SAY_INTRO_2_ARTHAS'),
('-1575002','The sensation is... beyond my imagining. I am yours to command, my king.','13857','1','0','1','29686','svala SAY_INTRO_3'),
('-1575003','Your first test awaits you. Destroy our uninvited guests.','14733','1','0','25','29689','svala SAY_INTRO_4_ARTHAS'),
('-1575004','I will be happy to slaughter them in your name! Come, enemies of the Scourge! I will show you the might of the Lich King!','13858','1','0','0','29687','svala SAY_INTRO_5'),
('-1575005','I will vanquish your soul!','13842','1','0','0','29672','svala SAY_AGGRO'),
('-1575006','You were a fool to challenge the power of the Lich King!','13845','1','0','0','29675','svala SAY_SLAY_1'),
('-1575007','Your will is done, my king.','13847','1','0','0','29676','svala SAY_SLAY_2'),
('-1575008','Another soul for my master.','13848','1','0','0','29677','svala SAY_SLAY_3'),
('-1575009','Your death approaches.','13850','1','0','0','29678','svala SAY_SACRIFICE_1'),
('-1575010','Go now to my master.','13851','1','0','0','29679','svala SAY_SACRIFICE_2'),
('-1575011','Your end is inevitable.','13852','1','0','0','29680','svala SAY_SACRIFICE_3'),
('-1575012','Yor-guul mak!','13853','1','0','0','29681','svala SAY_SACRIFICE_4'),
('-1575013','Any last words?','13854','1','0','0','29682','svala SAY_SACRIFICE_5'),
('-1575014','Nooo! I did not come this far... to...','13855','1','0','0','29683','svala SAY_DEATH'),

-- REUSE ME -1575015 to -1575018

('-1575019','What mongrels dare intrude here? Look alive, my brothers! A feast for the one that brings me their heads!','13497','1','0','22','31454','skadi SAY_AGGRO'),
('-1575020','Sear them to the bone!','13498','1','0','0','0','skadi SAY_DRAKEBREATH_1'),
('-1575021','Go now! Leave nothing but ash in your wake!','13499','1','0','0','0','skadi SAY_DRAKEBREATH_2'),
('-1575022','Cleanse our sacred halls with flame!','13500','1','0','0','0','skadi SAY_DRAKEBREATH_3'),
('-1575023','I ask for ... to kill them, yet all I get is feeble whelps! By Ye.. SLAUGHTER THEM!','13501','1','0','0','31457','skadi SAY_DRAKE_HARPOON_1'),
('-1575024','If one more harpoon touches my drake I''ll flae my miserable heins.','13502','1','0','0','31458','skadi SAY_DRAKE_HARPOON_2'),
('-1575025','Mjor Na Ul Kaval!','13503','1','0','0','0','skadi SAY_KILL_1'),
('-1575026','Not so brash now, are you?','13504','1','0','0','31461','skadi SAY_KILL_2'),
('-1575027','I''ll mount your skull from the highest tower!','13505','1','0','0','31462','skadi SAY_KILL_3'),
('-1575028','ARGH! You call that... an attack? I''ll... show... aghhhh...','13506','1','0','0','31463','skadi SAY_DEATH'),
('-1575029','You motherless knaves! Your corpses will make fine morsels for my new drake!','13507','1','0','0','31459','skadi SAY_DRAKE_DEATH'),
('-1575030','%s is within range of the harpoon launchers!','0','3','0','0','27809','skadi EMOTE_HARPOON_RANGE'),

('-1575031','You invade my home and then dare to challenge me? I will tear the hearts from your chests and offer them as gifts to the death god! Rualg nja gaborr!','13609','1','0','0','31464','ymiron SAY_AGGRO'),
('-1575032','Bjorn of the Black Storm! Honor me now with your presence!','13610','1','0','0','31465','ymiron SAY_SUMMON_BJORN'),
('-1575033','Haldor of the rocky cliffs, grant me your strength!','13611','1','0','0','31466','ymiron SAY_SUMMON_HALDOR'),
('-1575034','Ranulf of the screaming abyss, snuff these maggots with darkest night!','13612','1','0','0','31467','ymiron SAY_SUMMON_RANULF'),
('-1575035','Tor of the Brutal Siege! Bestow your might upon me!','13613','1','0','0','31468','ymiron SAY_SUMMON_TORGYN'),
('-1575036','Your death is only the beginning!','13614','1','0','0','31469','ymiron SAY_SLAY_1'),
('-1575037','You have failed your people!','13615','1','0','0','31470','ymiron SAY_SLAY_2'),
('-1575038','There is a reason I am king!','13616','1','0','0','31471','ymiron SAY_SLAY_3'),
('-1575039','Bleed no more!','13617','1','0','0','0','ymiron SAY_SLAY_4'),
('-1575040','What... awaits me... now?','13618','1','0','0','31473','ymiron SAY_DEATH'),

('-1575041','%s takes a deep breath.','0','3','0','0','20021','grauf EMOTE_DEEP_BREATH');

-- -1 576 000 NEXUS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1576000','You know what they say about curiosity.','13319','1','0','0','29593','telestra SAY_AGGRO'),
('-1576001','I''ll give you more than you can handle.','13321','1','0','0','29594','telestra SAY_SPLIT_1'),
('-1576002','There''s plenty of me to go around.','13322','1','0','0','29595','telestra SAY_SPLIT_2'),
('-1576003','Now to finish the job!','13323','1','0','0','29596','telestra SAY_MERGE'),
('-1576004','Death becomes you!','13324','1','0','0','29597','telestra SAY_KILL'),
('-1576005','Damn the... luck.','13320','1','0','0','29598','telestra SAY_DEATH'),

('-1576006','Chaos beckons.','13186','1','0','0','29599','anomalus SAY_AGGRO'),
('-1576007','Reality... unwoven.','13188','1','0','0','29601','anomalus SAY_RIFT'),
('-1576008','Indestructible.','13189','1','0','0','29602','anomalus SAY_SHIELD'),
('-1576009','Expiration... is necesarry.','13274','1','0','0','29603','anomalus SAY_KILL'),
('-1576010','Of course.','13187','1','0','0','23157','anomalus SAY_DEATH'),

('-1576011','Noo!','13328','1','0','0','29605','ormorok SAY_AGGRO'),
('-1576012','???','13329','1','0','0','0','ormorok SAY_KILL'),
('-1576013','Baaack!','13331','1','0','0','0','ormorok SAY_REFLECT'),
('-1576014','Bleeeed!','13332','1','0','0','0','ormorok SAY_ICESPIKE'),
('-1576015','Aaggh!','13330','1','0','0','29608','ormorok SAY_DEATH'),

('-1576016','Preserve? Why? There''s no truth in it. No no no... only in the taking! I see that now!','13450','1','0','0','26176','keristrasza SAY_AGGRO'),
('-1576017','Stay. Enjoy your final moments.','13451','1','0','0','29618','keristrasza SAY_CRYSTAL_NOVA'),
('-1576018','Finish it! Finish it! Kill me, or I swear by the Dragonqueen you''ll never see daylight again!','13452','1','0','0','29619','keristrasza SAY_ENRAGE'),
('-1576019','Now we''ve come to the truth!','13453','1','0','0','29620','keristrasza SAY_KILL'),
('-1576020','Dragonqueen... Life-Binder... preserve... me.','13454','1','0','0','29621','keristrasza SAY_DEATH'),

('-1576021','%s opens a Chaotic Rift!','0','3','0','0','27362','anomalus EMOTE_OPEN_RIFT'),
('-1576022','%s shields himself and divert his power to the rifts!','0','3','0','0','27363','anomalus EMOTE_SHIELD');

-- -1 578 000 OCULUS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1578000','What do we have here... those would defy the Spell-Weaver? Those without foresight or understanding. How could I make you see? Malygos is saving the world from itself! Bah! You are hardly worth my time!','13635','1','0','0','27411','urom SAY_SUMMON_1'),
('-1578001','Clearly my pets failed. Perhaps another demonstration is in order.','13636','1','0','0','27413','urom SAY_SUMMON_2'),
('-1578002','Still you fight. Still you cling to misguided principles. If you survive, you''ll find me in the center ring.','13637','1','0','0','27414','urom SAY_SUMMON_3'),
('-1578003','Poor blind fools!','13638','1','0','0','27412','urom SAY_AGGRO'),
('-1578004','A taste... just a small taste... of the Spell-Weaver''s power!','13639','1','0','0','31829','urom SAY_EXPLOSION_1'),
('-1578005','So much unstable energy... but worth the risk to destroy you!','13640','1','0','0','31830','urom SAY_EXPLOSION_2'),
('-1578006','If only you understood!','13641','1','0','0','31831','urom SAY_KILL_1'),
('-1578007','Now do you see? Do you?!','13642','1','0','0','31832','urom SAY_KILL_2'),
('-1578008','Unfortunate, but necessary.','13643','1','0','0','31833','urom SAY_KILL_3'),
('-1578009','Everything I''ve done... has been for Azeroth...','13644','1','0','0','31834','urom SAY_DEATH'),

('-1578010','Simpletons! You cannot comprehend the forces you have set in motion. The ley line conduit will not be disrupted! Your defeat shall be absolute!','13622','6','0','0','31841','eregos SAY_SPAWN'),
('-1578011','You brash interlopers are out of your element! I will ground you!','13623','1','0','0','31873','eregos SAY_AGGRO'),
('-1578012','We command the arcane! It shall not be used against us.','13626','1','0','0','31876','eregos SAY_ARCANE_SHIELD'),
('-1578013','It is trivial to extinguish your fire!','13627','1','0','0','31877','eregos SAY_FIRE_SHIELD'),
('-1578014','No magic of nature will help you now!','13625','1','0','0','31875','eregos SAY_NATURE_SHIELD'),
('-1578015','Such insolence... such arrogance... must be PUNISHED!','13624','1','0','0','31874','eregos SAY_FRENZY'),
('-1578016','It''s a long way down...','13628','1','0','0','31878','eregos SAY_KILL_1'),
('-1578017','Back to the earth with you!','13629','1','0','0','31879','eregos SAY_KILL_2'),
('-1578018','Enjoy the fall!','13630','1','0','0','31880','eregos SAY_KILL_3'),
('-1578019','Savor this small victory, foolish little creatures. You and your dragon allies have won this battle. But we will win... the Nexus War.','13631','1','0','0','31881','eregos SAY_DEATH'),

('-1578020','There will be no mercy!','13649','1','0','0','31813','varos SAY_AGGRO'),
('-1578021','Blast them! Destroy them!','13650','1','0','0','31814','varos SAY_CALL_CAPTAIN_1'),
('-1578022','Take no prisoners! Attack!','13651','1','0','0','31815','varos SAY_CALL_CAPTAIN_2'),
('-1578023','Strike now! Obliterate them!','13652','1','0','0','31816','varos SAY_CALL_CAPTAIN_3'),

('-1578024','Anomalies form as %s shifts into the Astral Plane!','0','3','0','0','31497','eregos EMOTE_ASTRAL_PLANE'),
('-1578025','%s begins to cast Empowered Arcane Explosion!','0','3','0','0','27903','urom EMOTE_EXPLOSION'),

('-1578026','You were warned!','13653','1','0','0','25500','varos SAY_KILL_1'),
('-1578027','The Oculus is ours!','13654','1','0','0','31818','varos SAY_KILL_2'),
('-1578028','They are... too strong! Underestimated their... fortitude.','13655','1','0','0','31819','varos SAY_DEATH'),
('-1578029','%s calls an Azure Ring Captain!','0','3','0','0','33077','varos EMOTE_CAPTAIN'),

('-1578030','%s flies away.','0','2','0','0','25992','drakes EMOTE_FLY_AWAY');

-- -1 580 000 SUNWELL PLATEAU
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1580000','Aggh!! No longer will I be a slave to Malygos! Challenge me and you will be destroyed!','12422','1','0','0','25496','kalecgos SAY_EVIL_AGGRO'),
('-1580001','I will purge you!','12423','1','0','0','25497','kalecgos SAY_EVIL_SPELL1'),
('-1580002','Your pain has only begun!','12424','1','0','0','25498','kalecgos SAY_EVIL_SPELL2'),
('-1580003','In the name of Kil''jaeden!','12425','1','0','0','25499','kalecgos SAY_EVIL_SLAY1'),
('-1580004','You were warned!','12426','1','0','0','25500','kalecgos SAY_EVIL_SLAY2'),
('-1580005','My awakening is complete! You shall all perish!','12427','1','0','0','25501','kalecgos SAY_EVIL_ENRAGE'),
('-1580006','I need... your help! Cannot... resist him much longer!','12428','1','0','0','23981','kalecgos humanoid SAY_GOOD_AGGRO'),
('-1580007','Agghh!! Help me before I lose my mind!','12429','1','0','0','23982','kalecgos humanoid SAY_GOOD_NEAR_DEATH'),
('-1580008','Hurry! There is not much of me left!','12430','1','0','0','23983','kalecgos humanoid SAY_GOOD_NEAR_DEATH2'),
('-1580009','I am forever in your debt. Once we have triumphed over Kil''jaeden, this entire world will be in your debt as well.','12431','0','0','0','25263','kalecgos humanoid SAY_GOOD_PLRWIN'),
('-1580010','There will be no reprieve! My work here is nearly finished!','12451','1','0','0','25504','sathrovarr SAY_SATH_AGGRO'),
('-1580011','I''m never on the... losing... side!','12452','1','0','0','25505','sathrovarr SAY_SATH_DEATH'),
('-1580012','Your misery is my delight!','12453','1','0','0','25506','sathrovarr SAY_SATH_SPELL1'),
('-1580013','I will watch you bleed!','12454','1','0','0','25507','sathrovarr SAY_SATH_SPELL2'),
('-1580014','Piteous mortal!','12455','1','0','0','25508','sathrovarr SAY_SATH_SLAY1'),
('-1580015','Haven''t you heard? I always win!','12456','1','0','0','25509','sathrovarr SAY_SATH_SLAY2'),
('-1580016','I have toyed with you long enough!','12457','1','0','0','25510','sathrovarr SAY_SATH_ENRAGE'),

('-1580017','Puny lizard! Death is the only answer you''ll find here!','12458','1','0','0','25222','brutallus YELL_INTRO'),
('-1580018','Bagh! Your magic is weak!','12459','1','0','0','25223','brutallus YELL_INTRO_BREAK_ICE'),
('-1580019','I will crush you!','12460','1','0','0','25224','brutallus YELL_INTRO_CHARGE'),
('-1580020','That was fun, but I still await a true challenge!','12461','1','0','0','25225','brutallus YELL_INTRO_KILL_MADRIGOSA'),
('-1580021','Come try your luck!','12462','1','0','0','25220','brutallus YELL_INTRO_TAUNT'),
('-1580022','Ah, more lambs to the slaughter!','12463','1','0','0','25628','brutallus YELL_AGGRO'),
('-1580023','Perish, insect!','12464','1','0','0','25629','brutallus YELL_KILL1'),
('-1580024','You are meat!','12465','1','0','0','25630','brutallus YELL_KILL2'),
('-1580025','Too easy!','12466','1','0','0','25631','brutallus YELL_KILL3'),
('-1580026','Bring the fight to me!','12467','1','0','0','25632','brutallus YELL_LOVE1'),
('-1580027','Another day, another glorious battle!','12468','1','0','0','25633','brutallus YELL_LOVE2'),
('-1580028','I live for this!','12469','1','0','0','25634','brutallus YELL_LOVE3'),
('-1580029','So much for a real challenge. Die!','12470','1','0','0','25635','brutallus YELL_BERSERK'),
('-1580030','Agghh! Well done! Now this gets... interesting...','12471','1','0','0','25636','brutallus YELL_DEATH'),

('-1580031','Hold, friends! There is information to be had before this devil meets his fate!','12472','1','0','0','25221','madrigosa YELL_MADR_ICE_BARRIER'),
('-1580032','Where is Anveena, demon? What has become of Kalec?','12473','1','0','293','25226','madrigosa YELL_MADR_INTRO'),
('-1580033','You will tell me where they are!','12474','1','0','0','25227','madrigosa YELL_MADR_ICE_BLOCK'),
('-1580034','Speak! I grow weary of asking! ','12475','1','0','0','25228','madrigosa YELL_MADR_TRAP'),
('-1580035','Malygos, my lord... I did my... best.','12476','1','0','0','25229','madrigosa YELL_MADR_DEATH'),

('-1580036','Glory to Kil''jaeden! Death to all who oppose!','12477','1','0','0','25637','felmyst SAY_INTRO'),
('-1580037','I kill for the master!','12480','1','0','0','25640','felmyst SAY_KILL_1'),
('-1580038','The end has come!','12481','1','0','0','25641','felmyst SAY_KILL_2'),
('-1580039','Choke on your final breath!','12478','1','0','0','25638','felmyst SAY_BREATH'),
('-1580040','I am stronger than ever before!','12479','1','0','0','25639','felmyst SAY_TAKEOFF'),
('-1580041','No more hesitation! Your fates are written!','12482','1','0','0','25642','felmyst SAY_BERSERK'),
('-1580042','Kil''jaeden will... prevail!','12483','1','0','0','25643','felmyst SAY_DEATH'),
('-1580043','Madrigosa deserved a far better fate. You did what had to be done, but this battle is far from over!','12439','1','0','0','25261','kalecgos SAY_KALECGOS_OUTRO'),

-- 25512 Misery...confusion...mistrust. These are the hallmarks.
-- 25511 Depravity...hatred...chaos. These are the pillars.
('-1580044','Misery...confusion...mistrust. These are the hallmarks.','12484','1','0','0','25512','sacrolash SAY_INTRO_1'),
('-1580045','Depravity...hatred...chaos. These are the pillars.','0','1','0','0','25511','alythess SAY_INTRO_2'),

('-1580052','Shadow to the aid of fire!','12485','1','0','0','24423','sacrolash SAY_SACROLASH_SHADOW_NOVA'),
('-1580053','Alythess! Your fire burns within me!','12488','1','0','0','24452','sacrolash SAY_SACROLASH_EMPOWER'),
('-1580054','Shadows engulf.','12486','1','0','0','25519','sacrolash SAY_SACROLASH_KILL_1'),
('-1580055','Ee-nok Kryul!','12487','1','0','0','25520','sacrolash SAY_SACROLASH_KILL_2'),
('-1580056','I... fade.','12399','1','0','0','25522','sacrolash SAY_SACROLASH_DEAD'),
('-1580057','Time is a luxury you no longer possess!','0','1','0','0','25521','sacrolash SAY_SACROLASH_BERSERK'),
('-1580058','Fire to the aid of shadow!','12489','1','0','0','25513','alythess SAY_ALYTHESS_CANFLAGRATION'),
('-1580059','Sacrolash!','12492','1','0','0','25516','alythess SAY_ALYTHESS_EMPOWER'),
('-1580060','Fires consume.','12490','1','0','0','25514','alythess SAY_ALYTHESS_KILL_1'),
('-1580061','Edir harach!','12491','1','0','0','25515','alythess SAY_ALYTHESS_KILL_2'),
('-1580062','Der''ek... manul.','12494','1','0','0','25518','alythess SAY_ALYTHESS_DEAD'),
('-1580063','Your luck has run its course!','12493','1','0','0','25517','alythess SAY_ALYTHESS_BERSERK'),

('-1580064','All my plans have led to this!','12495','6','0','0','25544','kiljaeden SAY_ORDER_1'),
('-1580065','Stay on task, do not waste time!','12496','6','0','0','25545','kiljaeden SAY_ORDER_2'),
('-1580066','I have waited long enough!','12497','6','0','0','25546','kiljaeden SAY_ORDER_3'),
('-1580067','Fail me and suffer for eternity!','12498','6','0','0','25547','kiljaeden SAY_ORDER_4'),
('-1580068','Drain the girl! Drain her power until there is nothing but a vacant shell!','12499','6','0','0','25548','kiljaeden SAY_ORDER_5'),
('-1580069','The expendable have perished. So be it! Now I shall succeed where Sargeras could not! I will bleed this wretched world and secure my place as the true master of the Burning Legion! The end has come! Let the unravelling of this world commence!','12500','1','0','0','24808','kiljaeden SAY_EMERGE'),
('-1580070','Another step toward destruction!','12501','1','0','0','25549','kiljaeden SAY_SLAY_1'),
('-1580071','Anakh Kyree!','12502','1','0','0','25550','kiljaeden SAY_SLAY_2'),
('-1580072','Who can you trust!','12503','1','0','0','25551','kiljaeden SAY_REFLECTION_1'),
('-1580073','The enemy is among you!','12504','1','0','0','25552','kiljaeden SAY_REFLECTION_2'),
('-1580074','Chaos!','12505','1','0','0','25553','kiljaeden SAY_DARKNESS_1'),
('-1580075','Destruction!','12506','1','0','0','25554','kiljaeden SAY_DARKNESS_2'),
('-1580076','Oblivion!','12507','1','0','0','25555','kiljaeden SAY_DARKNESS_3'),
('-1580077','I will not be denied! This world shall fall!','12508','1','0','0','24754','kiljaeden SAY_PHASE_3'),
('-1580078','Do not harbor false hope! You cannot win!','12509','1','0','0','24825','kiljaeden SAY_PHASE_4'),
('-1580079','Aggghh! The powers of the Sunwell... turn... against me! What have you done? What have you done???','12510','1','0','0','25199','kiljaeden SAY_PHASE_5'),
('-1580080','You are not alone! The blue dragonflight shall help you vanquish the Deceiver!','12438','1','0','0','25541','kalecgos SAY_KALECGOS_INTRO'),
('-1580081','Anveena, you must awaken! This world needs you!','12445','1','0','0','25190','kalecgos SAY_KALECGOS_AWAKE_1'),
('-1580082','I serve only the master now.','12511','0','0','0','25195','anveena SAY_ANVEENA_IMPRISONED'),
('-1580083','You must let go! You must become what you were always meant to be! The time is now Anveena!','12446','1','0','0','25191','kalecgos SAY_KALECGOS_AWAKE_2'),
('-1580084','But I''m... lost. I cannot find my way back.','12512','0','0','0','25196','anveena SAY_ANVEENA_LOST'),
('-1580085','Anveena I love you! Focus on my voice! Come back for me now! Only you can cleanse the Sunwell!','12447','1','0','0','25192','kalecgos SAY_KALECGOS_AWAKE_4'),
('-1580086','Kalec... Kalec?','12513','0','0','0','25197','anveena SAY_ANVEENA_AWAKE'),
('-1580087','Yes Anveena! Let fate embrace you now!','12448','1','0','0','25193','kalecgos SAY_KALECGOS_AWAKE_5'),
('-1580088','The nightmare is over! The spell is broken! Goodbye Kalec, my love!','12514','0','0','0','25198','anveena SAY_ANVEENA_SACRIFICE'),
('-1580089','Goodbye Anveena, my love. Few will remember your name, yet this day you changed the course of destiny. What was once corrupt, is now pure. Heroes, do not let her sacrifice be in vain.','12450','0','0','0','25194','kalecgos SAY_KALECGOS_GOODBYE'),
('-1580090','Strike now heroes, while he is weakened! Vanquish the Deceiver!','12449','1','0','0','25822','kalecgos SAY_KALECGOS_ENCOURAGE'),
('-1580091','I will channel my powers into the orbs! Be ready!','12440','1','0','0','24822','kalecgos SAY_KALECGOS_ORB_1'),
('-1580092','I have empowered another orb! Use it quickly!','12441','1','0','0','25542','kalecgos SAY_KALECGOS_ORB_2'),
('-1580093','Another orb is ready! Make haste!','12442','1','0','0','25543','kalecgos SAY_KALECGOS_ORB_3'),
('-1580094','I have channeled all I can! The power is in your hands!','12443','1','0','0','24824','kalecgos SAY_KALECGOS_ORB_4'),

('-1580095','Mortal heroes - your victory here today was foretold long ago. My brother''s anguished cry of defeat will echo across the universe - bringing renewed hope to all those who still stand against the Burning Crusade.','12515','0','0','1','25403','velen SAY_OUTRO_1'),
('-1580096','As the Legion''s final defeat draws ever-nearer, stand proud in the knowledge that you have saved worlds without number from the flame.','12516','0','0','1','25404','velen SAY_OUTRO_2'),
('-1580097','Just as this day marks an ending, so too does it herald a new beginning...','12517','0','0','1','25405','velen SAY_OUTRO_3'),
('-1580098','The creature Entropius, whom you were forced to destroy, was once the noble naaru, M''uru. In life, M''uru channeled vast energies of LIGHT and HOPE. For a time, a misguided few sought to steal those energies...','12518','0','0','1','25407','velen SAY_OUTRO_4'),
('-1580099','Our arrogance was unpardonable. We damned one of the most noble beings of all. We may never atone for this sin.','12524','0','0','1','25408','liadrin SAY_OUTRO_5'),
('-1580100','Then fortunate it is, that I have reclaimed the noble naaru''s spark from where it fell! Where faith dwells, hope is never lost, young blood elf.','12519','0','0','1','25409','velen SAY_OUTRO_6'),
('-1580101','Can it be?','12525','0','0','1','25410','liadrin SAY_OUTRO_7'),
('-1580102','Gaze now, mortals - upon the HEART OF M''URU! Unblemished. Bathed by the light of Creation - just as it was at the Dawn.','12520','0','0','1','25411','velen SAY_OUTRO_8'),
('-1580103','In time, the light and hope held within - will rebirth more than this mere fount of power... Mayhap, they will rebirth the soul of a nation. ','12521','0','0','1','25412','velen SAY_OUTRO_9'),
('-1580104','Blessed ancestors! I feel it... so much love... so much grace... there are... no words... impossible to describe...','12526','0','0','1','25413','liadrin SAY_OUTRO_10'),
('-1580105','Salvation, young one. It waits for us all.','12522','0','0','1','25414','velen SAY_OUTRO_11'),
('-1580106','Farewell...','12523','0','0','1','25415','velen SAY_OUTRO_12'),

('-1580107','%s takes a deep breath.','0','3','0','0','20021','felmyst EMOTE_DEEP_BREATH'),
('-1580108','Kil''jaeden must be destroyed!','12436','0','0','1','25502','kalec SAY_KALEC_OUTRO_2'),

('-1580109','Unit entering energy conservation mode.','0','1','0','0','25200','npc_sunblade_protector SAY_PROTECTOR_REACHED_HOME'),
('-1580110','Local proximity threat detected. Exiting energy conservation mode.','0','1','0','0','25201','npc_sunblade_protector SAY_INACTIVE_PROTECTOR_AGGRO'),
('-1580111','Enemies spotted! Attack while I try to activate a Protector!','0','1','0','0','25202','npc_sunblade_scout SAY_SCOUT_AGGRO'),
('-1580112','Enemy presence detected.','0','1','0','0','25203','npc_sunblade_protector SAY_ACTIVE_PROTECTOR_AGGRO'),
('-1580113','Unit is now operational and attacking targets.','0','1','0','0','25206','npc_sunblade_protector SAY_PROTECTOR_ACTIVATED_AGGRO'),

('-1580114','There is no time to waste!','0','1','0','0','24993','Kalec Wipe'),
('-1580115','The fate of the world hangs in the balance!','12437','0','0','1','25503','kalec SAY_KALEC_OUTRO_3'),
-- gauntlet
('-1580116','Bring forth the imps!','0','6','0','1','25050','gauntlet Start'),
-- twins
('-1580117','Sacrolash directs Shadow Nova at $n.','0','3','0','1','24424','emote shadow nova'),
('-1580118','Alythess directs Conflagration at $n.','0','3','0','1','24426','emote conflagration');

-- -1 585 000 MAGISTER'S TERRACE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1585000','You only waste my time!','12378','1','0','0','25423','selin SAY_AGGRO'),
('-1585001','My hunger knows no bounds!','12381','1','0','0','25424','selin SAY_ENERGY'),
('-1585002','Yes!! I am a god!','12382','1','0','0','25425','selin SAY_EMPOWERED'),
('-1585003','Enough distractions!','12388','1','0','0','25428','selin SAY_KILL_1'),
('-1585004','I am invincible!','12385','1','0','0','25427','selin SAY_KILL_2'),
('-1585005','No... more, I must have more!!','12383','1','0','0','25426','selin SAY_DEATH'),
('-1585006','%s begins to channel from the nearby Fel Crystal. Destroy it!','0','3','0','0','23753','selin EMOTE_CRYSTAL'),

('-1585007','Drain... life!','12389','1','0','0','25623','vexallus SAY_AGGRO'),
('-1585008','Un...con...tainable.','12392','1','0','0','25626','vexallus SAY_ENERGY'),
('-1585009','Un...leash.','12390','1','0','0','25625','vexallus SAY_OVERLOAD'),
('-1585010','Con...sume.','12393','1','0','0','25627','vexallus SAY_KILL'),
('-1585011','Vexallus discharges pure energy!','0','3','0','0','23752','vexallus EMOTE_DISCHARGE_ENERGY'),

('-1585012','Annihilate them.','12395','1','0','0','26471','delrissa SAY_AGGRO'),
('-1585013','Oh, the horror!','12398','1','0','0','25432','delrissa LackeyDeath1'),
('-1585014','Well aren''t you lucky?','12400','1','0','0','25433','delrissa LackeyDeath2'),
('-1585015','Now I''m getting annoyed.','12401','1','0','0','25434','delrissa LackeyDeath3'),
('-1585016','Lackeys be damned! I''ll finish you myself!','12403','1','0','0','25435','delrissa LackeyDeath4'),
('-1585017','I call that a good start.','12405','1','0','0','25436','delrissa PlayerDeath1'),
('-1585018','I could have sworn there were more of you...','12407','1','0','0','25437','delrissa PlayerDeath2'),
('-1585019','Not really much of a "group" anymore, is it?','12409','1','0','0','25438','delrissa PlayerDeath3'),
('-1585020','One is such a lonely number.','12410','1','0','0','25439','delrissa PlayerDeath4'),
('-1585021','It''s been a kick, really.','12411','1','0','0','25440','delrissa PlayerDeath5'),
('-1585022','Not what I had... planned.','12397','1','0','0','25431','delrissa SAY_DEATH'),

('-1585023','Don''t look so smug! I know what you''re thinking, but Tempest Keep was merely a setback. Did you honestly believe I would trust the future to some blind, half-night elf mongrel?','12413','1','0','0','25386','kaelthas MT SAY_INTRO_1'),
('-1585024','Vengeance burns!','12415','1','0','0','25388','kaelthas MT SAY_PHOENIX'),
('-1585025','Felomin Ashal! ','12417','1','0','0','25389','kaelthas MT SAY_FLAMESTRIKE'),
('-1585026','I''ll turn your world... upside... down.','12418','1','0','0','25390','kaelthas MT SAY_GRAVITY_LAPSE'),
('-1585027','Master, grant me strength.','12419','1','0','0','25391','kaelthas MT SAY_TIRED'),
('-1585028','Do not get... too comfortable.','12420','1','0','0','25392','kaelthas MT SAY_RECAST_GRAVITY'),
('-1585029','My demise accomplishes nothing! The master will have you! You will drown in your own blood! The world shall burn! Aaaghh!','12421','1','0','0','25393','kaelthas MT SAY_DEATH'),
('-1585030','Oh no, he was merely an instrument, a stepping stone to a much larger plan! It has all led to this... and this time you will not interfere!','0','1','0','0','25387','kaelthas MT SAY_INTRO_2'),

('-1585031','Vexallus overloads!','0','3','0','0','23781','vexallus EMOTE_OVERLOAD'),
('-1585032','Be still, mortals, and hearken to my words.','0','6','0','0','23936','Kalecgos on spawn');

-- -1 595 000 CULLING OF STRATHOLME
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1595000','All soldiers of Lordaeron should immediately report to the entrance of Stratholme, and await further orders from Prince Arthas.','0','6','0','0','27263','lordaeron crier SAY_SOLDIERS_REPORT'),
('-1595001','Good work with the crates! Come talk to me in front of Stratholme for your next assignment!','0','4','0','0','30509','chromie WHISPER_CHROMIE_CRATES'),
('-1595002','Oh, no! Adventurers, something awful has happened! A colleague of mine has been captured by the Infinite Dragonflight, and they''re doing something horrible to him! Keeping Arthas is still your highest priority, but if you act fast you could help save a Guardian of Time!','0','4','0','0','32670','chromie WHISPER_CHROMIE_GUARDIAN'),
('-1595003','Scourge forces have been spotted near the Festival Lane Gate!','0','6','0','0','27644','lordaeron crier SAY_SCOURGE_FESTIVAL_LANE'),
('-1595004','Scourge forces have been spotted near the King''s Square fountain!','0','6','0','0','27642','lordaeron crier SAY_SCOURGE_KINGS_SQUARE'),
('-1595005','Scourge forces have been spotted near the Market Row Gate!','0','6','0','0','27643','lordaeron crier SAY_SCOURGE_MARKET_ROW'),
('-1595006','Scourge forces have been spotted near the Town Hall!','0','6','0','0','27646','lordaeron crier SAY_SCOURGE_TOWN_HALL'),
('-1595007','Scourge forces have been spotted near the Elder''s Square Gate!','0','6','0','0','27645','lordaeron crier SAY_SCOURGE_ELDERS_SQUARE'),
('-1595008','Champions, meet me at the Town Hall at once. We must take the fight to Mal''Ganis.','14297','6','0','0','28890','arthas SAY_MEET_TOWN_HALL'),
('-1595009','Follow me, I know the way through.','14298','0','0','1','27800','arthas SAY_FOLLOW'),
('-1595010','Ah, you''ve finally arrived Prince Arthas. You''re here just in the nick of time.','0','0','0','1','27811','citizen SAY_ARRIVED'),
('-1595011','Yes, I''m glad I could get to you before the plague.','14299','0','0','0','27812','arthas SAY_GET_BEFORE_PLAGUE'),
('-1595012','What is this sorcery?','14300','0','0','0','27814','arthas SAY_SORCERY'),
('-1595013','There''s no need for you to understand, Arthas. All you need to do is die.','0','0','0','1','27815','citizen SAY_NO_UNDERSTAND'),
('-1595014','Mal''Ganis appears to have more than Scourge in his arsenal. We should make haste.','14301','0','0','1','27840','arthas SAY_MORE_THAN_SCOURGE'),
('-1595015','More vile sorcery! Be ready for anything!','14302','0','0','0','27882','arthas SAY_MORE_SORCERY'),
('-1595016','Let''s move on.','14303','0','0','396','27889','arthas SAY_MOVE_ON'),
('-1595017','Watch your backs: they have us surrounded in this hall.','14304','0','0','1','27901','arthas SAY_WATCH_BACKS'),
('-1595018','Mal''Ganis is not making this easy.','14305','0','0','396','27902','arthas SAY_NOT_EASY'),
('-1595019','They''re very persistent.','14306','0','0','396','27904','arthas SAY_PERSISTENT'),
('-1595020','What else can he put in my way?','14307','0','0','396','27905','arthas SAY_ELSE'),
('-1595021','Prince Arthas Menethil, on this day, a powerful darkness has taken hold of your soul. The death you are destined to visit upon others will this day be your own.','13408','1','0','0','27911','chrono-lord SAY_DARKNESS'),
('-1595022','I do what I must for Lordaeron, and neither your words nor your actions will stop me.','14309','0','0','396','27916','arthas SAY_DO_WHAT_MUST'),
('-1595023','The quickest path to Mal''Ganis lies behind that bookshelf ahead.','14308','0','0','0','27906','arthas SAY_QUICK_PATH'),
('-1595024','This will only take a moment.','14310','0','0','432','28040','arthas SAY_TAKE_A_MOMENT'),
('-1595025','I''m relieved this secret passage still works.','14311','0','0','0','28041','arthas SAY_PASSAGE'),
('-1595026','Let''s move through here as quickly as possible. If the undead don''t kill us, the fires might.','14312','0','0','396','28562','arthas SAY_MOVE_QUICKLY'),
('-1595027','Rest a moment and clear your lungs, but we must move again soon.','14313','0','0','396','28045','arthas SAY_REST'),
('-1595028','That''s enough; we must move again. Mal''Ganis awaits.','14314','0','0','396','28047','arthas SAY_REST_COMPLETE'),
('-1595029','At last some good luck. Market Row has not caught fire yet. Mal''Ganis is supposed to be in Crusaders'' Square, which is just ahead. Tell me when you''re ready to move forward.','14315','0','0','396','28048','arthas SAY_CRUSADER_SQUARE'),
('-1595030','Justice will be done.','14316','0','0','0','28563','arthas SAY_JUSTICE'),
('-1595031','We''re going to finish this right now, Mal''Ganis. Just you... and me.','14317','0','0','5','28520','arthas SAY_FINISH_MALGANIS'),
('-1595032','Your journey has just begun, young prince. Gather your forces and meet me in the arctic land of Northrend. It is there that we shall settle the score between us. It is there that your true destiny will unfold.','14412','1','0','378','28528','malganis SAY_JOURNEY_BEGUN'),
('-1595033','I''ll hunt you to the ends of the earth if I have to! Do you hear me? To the ends of the earth!','14318','0','0','0','28529','arthas SAY_HUNT_MALGANIS'),
('-1595034','You performed well this day. Anything that Mal''Ganis has left behind is yours. Take it as your reward. I must now begin plans for an expedition to Northrend.','14319','0','0','1','28530','arthas SAY_ESCORT_COMPLETE'),
('-1595035','Protect your prince, soldiers of Lordaeron! I am in need of aid!','14320','0','0','0','31352','arthas SAY_HALF_HP'),
('-1595036','I am being overwhelmed, assist me!','14321','0','0','0','31353','arthas SAY_LOW_HP'),
('-1595037','Mal''Ganis will pay for this.','14322','0','0','0','31354','arthas SAY_SLAY_1'),
('-1595038','I can''t afford to spare you.','14323','0','0','0','31355','arthas SAY_SLAY_2'),
('-1595039','One less obstacle to deal with.','14324','0','0','0','31356','arthas SAY_SLAY_3'),
('-1595040','Agh! Damn you, Mal''Ganis! Father...Jaina...I have failed Lordaeron...','14325','0','0','0','31351','arthas SAY_DEATH'),
('-1595041','My work here is finished!','0','6','0','0','19128','infinite corruptor SAY_CORRUPTOR_DESPAWN');

-- -1 599 000 HALLS OF STONE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1599000','Soft, vulnerable shells. Brief, fragile lives. You can not escape the curse of flesh!','14180','1','0','0','29582','sjonnir SAY_AGGRO'),
('-1599001','Folvynn buul hrom onn!','14181','1','0','0','29583','sjonnir SAY_SLAY_1'),
('-1599002','Flesh is no match for iron!','14182','1','0','0','29584','sjonnir SAY_SLAY_2'),
('-1599003','Armies of iron will smother the world!','14183','1','0','0','29585','sjonnir SAY_SLAY_3'),
('-1599004','Loken will not rest, until the forge is retaken. You changed nothing!','14184','1','0','0','29586','sjonnir SAY_DEATH'),

-- RE-USE -1599005 to -1599011

('-1599012','Now that''s owning your supper!','14244','1','0','0','30525','brann SAY_KILL_1'),
('-1599013','Press on, that''s the way!','14245','1','0','0','30526','brann SAY_KILL_2'),
('-1599014','Keep it up now. Plenty of death-dealing for everyone!','14246','1','0','0','0','brann SAY_KILL_3'),
('-1599015','I''m all kinds of busted up. Might not... make it...','14257','1','0','0','28121','brann SAY_LOW_HEALTH'),
('-1599016','Not yet, not... yet-','14258','1','0','0','30522','brann SAY_DEATH'),
('-1599017','I''m doing everything I can!','14260','1','0','0','30562','brann SAY_PLAYER_DEATH_1'),
('-1599018','Light preserve you!','14261','1','0','0','30563','brann SAY_PLAYER_DEATH_2'),
('-1599019','I hope this is all worth it!','14262','1','0','0','30564','brann SAY_PLAYER_DEATH_3'),
('-1599020','Time to get some answers! Let''s get this show on the road!','14259','1','0','0','30524','brann SAY_ESCORT_START'),

('-1599021','REUSE ME','0','0','0','0','0','REUSE ME'),

('-1599022','This is a wee bit trickier that before... Oh, bloody--incomin''!','14275','1','0','0','27589','brann SAY_SPAWN_TROGG'),
('-1599023','What in the name o'' Madoran did THAT do? Oh! Wait: I just about got it...','14276','1','0','0','27590','brann SAY_SPAWN_OOZE'),
('-1599024','Ha, that did it. Help''s a-coming. Take this you glow-eying brute!','14277','1','0','0','27591','brann SAY_SPAWN_EARTHEN'),

('-1599025','REUSE ME','0','0','0','0','0','REUSE ME'),

('-1599026','Now keep an eye out! I''ll have this licked in two shakes of a--','14248','1','0','432','30552','brann SAY_EVENT_INTRO_2'),
('-1599027','Warning! Life form pattern not recognized. Archival processing terminated. Continued interference will result in targeted response.','13765','1','0','0','29561','brann SAY_EVENT_INTRO_3_ABED'),

('-1599028','Oh, that doesn''t sound good. We might have a complication or two...','14249','1','0','1','30553','brann SAY_EVENT_A_1'),
('-1599029','Security breach in progress. Analysis of historical archives transferred to lower priority queue. Countermeasures engaged.','13756','1','0','0','29575','brann SAY_EVENT_A_2_KADD'),
('-1599030','Ah, you want to play hardball, eh? That''s just my game!','14250','1','0','0','30554','brann SAY_EVENT_A_3'),

('-1599031','Couple more minutes and I''ll--','14251','1','0','0','30555','brann SAY_EVENT_B_1'),
('-1599032','Threat index threshold exceeded. Celestial archive aborted. Security level heightened.','13761','1','0','0','29576','brann SAY_EVENT_B_2_MARN'),
('-1599033','Heightened? What''s the good news?','14252','1','0','0','30556','brann SAY_EVENT_B_3'),

('-1599034','So that was the problem? Now I''m makin'' progress...','14253','1','0','0','30557','brann SAY_EVENT_C_1'),
('-1599035','Critical threat index. Void analysis diverted. Initiating sanitization protocol.','13767','1','0','0','29577','brann SAY_EVENT_C_2_ABED'),
('-1599036','Hang on! Nobody''s gonna'' be sanitized as long as I have a say in it!','14254','1','0','0','30558','brann SAY_EVENT_C_3'),

('-1599037','Ha! The old magic fingers finally won through! Now let''s get down to-','14255','1','0','0','30559','brann SAY_EVENT_D_1'),
('-1599038','Alert! Security fail safes deactivated. Beginning memory purge...','13768','1','0','0','29579','brann SAY_EVENT_D_2_ABED'),
('-1599039','Purge? No no no no no! Where did I-- Aha, this should do the trick...','14256','1','0','0','30560','brann SAY_EVENT_D_3'),
('-1599040','System online. Life form pattern recognized. Welcome Branbronzan. Query?','13769','1','0','0','29580','brann SAY_EVENT_D_4_ABED'),

('-1599041','Query? What do you think I''m here for? Tea and biscuits? Spill the beans already!','14263','1','0','0','30565','brann SAY_EVENT_END_01'),
('-1599042','Tell me how that dwarfs came to be! And start at the beginning!','14264','1','0','0','30566','brann SAY_EVENT_END_02'),
('-1599043','Accessing prehistoric data. Retrieved. In the beginning Earthen were created to-','13770','1','0','0','30611','brann SAY_EVENT_END_03_ABED'),
('-1599044','Right, right! I know that the Earthen were made of stone to shape the deep reaches of the world but what about the anomalies? Matrix non-stabilizing and whatnot.','14265','1','0','0','30567','brann SAY_EVENT_END_04'),
('-1599045','Accessing. In the early stages of its development cycle Azeroth suffered infection by parasitic, necrophotic symbiotes.','13771','1','0','0','30612','brann SAY_EVENT_END_05_ABED'),
('-1599046','Necro-what? Speak bloody common will ya?','14266','1','0','0','30568','brann SAY_EVENT_END_06'),
('-1599047','Designation: Old Gods. Old Gods rendered all systems, including Earthen defenseless in order to facilitate assimilation. This matrix destabilization has been termed the Curse of Flesh. Effects of destabilization increased over time.','13772','1','0','0','30613','brann SAY_EVENT_END_07_ABED'),
('-1599048','Old Gods eh? So they zapped the Earthen with this Curse of Flesh. And then what?','14267','1','0','0','30577','brann SAY_EVENT_END_08'),
('-1599049','Accessing. Creators arrived to extirpate symbiotic infection. Assessment revealed that Old God infestation had grown malignant. Excising parasites would result in loss of host.','13757','1','0','0','30603','brann SAY_EVENT_END_09_KADD'),
('-1599050','If they killed the Old Gods Azeroth would have been destroyed.','14268','1','0','0','30578','brann SAY_EVENT_END_10'),
('-1599051','Correct. Creators neutralized parasitic threat and contained it within the host. Forge of Wills and other systems were instituted to create new Earthen. Safeguards were implemented and protectors were appointed.','13758','1','0','0','30604','brann SAY_EVENT_END_11_KADD'),
('-1599052','What protectors?','14269','1','0','0','30579','brann SAY_EVENT_END_12'),
('-1599053','Designations: Aesir and Vanir or in common nomenclator Storm and Earth Giants. Sentinel Loken designated supreme. Dragon Aspects appointed to monitor evolution of Azeroth.','13759','1','0','0','30605','brann SAY_EVENT_END_13_KADD'),
('-1599054','Aesir and Vanir. Okay. So the Forge of Wills started to make new Earthen. But what happened to the old ones?','14270','1','0','0','30580','brann SAY_EVENT_END_14'),
('-1599055','Additional background is relevant to your query. Following global combat between-','13762','1','0','0','30606','brann SAY_EVENT_END_15_MARN'),
('-1599056','Hold everything! The Aesir and Vanir went to war? Why?','14271','1','0','0','30581','brann SAY_EVENT_END_16'),
('-1599057','Unknown. Data suggests that impetus for global combat originated with prime designate Loken who neutralized all remaining Aesir and Vanir affecting termination of conflict. Prime designate Loken then initiated stasis of several seed races including Earthen, Giant and Vrykul at designated holding facilities.','13763','1','0','0','30607','brann SAY_EVENT_END_17_MARN'),
('-1599058','This Loken sounds like a nasty character. Glad we don''t have to worry about the likes of him anymore. So if I''m understanding you lads the original Earthen eventually woke up from this statis. And by that time this destabily-whatever had turned them into our brother dwarfs. Or at least dwarf ancestors. Hm?','14272','1','0','0','30582','brann SAY_EVENT_END_18'),
('-1599059','Essentially that is correct.','13764','1','0','0','30608','brann SAY_EVENT_END_19_MARN'),
('-1599060','Well now. That''s a lot to digest. I''m gonna need some time to take all of this in. Thank you!','14273','1','0','0','30583','brann SAY_EVENT_END_20'),
('-1599061','Acknowledged Branbronzan. Session terminated.','13773','1','0','0','30614','brann SAY_EVENT_END_21_ABED'),

('-1599062','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1599063','REUSE ME','0','0','0','0','0','REUSE ME'),

('-1599064','I think it''s time to see what''s behind the door near the entrance. I''m going to sneak over there, nice and quiet. Meet me at the door and I''ll get us in.','0','1','0','0','31660','brann SAY_ENTRANCE_MEET');

-- -1 600 000 DRAK'THARON KEEP
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1600000','More grunts, more glands, more FOOD!','13181','1','0','0','0','trollgore SAY_AGGRO'),
('-1600001','So hungry! Must feed!','13182','1','0','0','30043','trollgore SAY_CONSUME'),
('-1600002','Aaaargh...','13183','1','0','0','0','trollgore SAY_DEATH'),
('-1600003','Corpse go boom!','13184','1','0','0','30044','trollgore SAY_EXPLODE'),
('-1600004','You have gone, me gonna eat you!','13185','1','0','0','0','trollgore SAY_KILL'),

('-1600005','The chill that you feel is the herald of your doom!','13173','1','0','0','26932','novos SAY_AGGRO'),
('-1600006','Your efforts... are in vain.','13174','1','0','0','15168','novos SAY_DEATH'),
('-1600007','Such is the fate of all who oppose the Lich King.','13175','1','0','0','28035','novos SAY_KILL'),
('-1600008','Bolster my defenses! Hurry, curse you!','13176','1','0','0','26933','novos SAY_ADDS'),
('-1600009','Surely you can see the futility of it all!','13177','1','0','0','27777','novos SAY_BUBBLE_1'),
('-1600010','Just give up and die already!','13178','1','0','0','30036','novos SAY_BUBBLE_2'),
('-1600011','%s calls for assistance.','0','3','0','0','14265','novos EMOTE_ASSISTANCE'),

('-1600012','Tharon''ja sees all! The work of mortals shall not end the eternal dynasty!','13862','1','0','0','29606','tharonja SAY_AGGRO'),
('-1600013','As Tharon''ja predicted.','13863','1','0','0','29611','tharonja SAY_KILL_1'),
('-1600014','As it was written.','13864','1','0','0','29612','tharonja SAY_KILL_2'),
('-1600015','Your flesh serves Tharon''ja now!','13865','1','0','0','29613','tharonja SAY_FLESH_1'),
('-1600016','Tharon''ja has a use for your mortal shell!','13866','1','0','0','29614','tharonja SAY_FLESH_2'),
('-1600017','No! A taste... all too brief!','13867','1','0','0','29615','tharonja SAY_SKELETON_1'),
('-1600018','Tharon''ja will have more!','13868','1','0','0','29616','tharonja SAY_SKELETON_2'),
('-1600019','Im... impossible! Tharon''ja is eternal! Tharon''ja... is...','13869','1','0','0','29617','tharonja SAY_DEATH');

-- -1 601 000 AZJOL-NERUB
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1601000','This kingdom belongs to the Scourge! Only the dead may enter.','14075','1','0','0','32271','krikthir SAY_AGGRO'),
('-1601001','???','14076','1','0','0','0','krikthir SAY_KILL_1'),
('-1601002','You were foolish to come.','14077','1','0','0','32273','krikthir SAY_KILL_2'),
('-1601003','As Anub''Arak commands!','14078','1','0','0','32274','krikthir SAY_KILL_3'),
('-1601004','We are besieged. Strike out and bring back their corpses!','14079','1','0','0','32275','krikthir SAY_SEND_GROUP_1'),
('-1601005','We must hold the gate. Attack! Tear them limb from limb!','14080','1','0','0','32276','krikthir SAY_SEND_GROUP_2'),
('-1601006','The gate must be protected at all costs. Rip them to shreds!','14081','1','0','0','32277','krikthir SAY_SEND_GROUP_3'),
('-1601007','Keep an eye on the tunnel. We must not let anyone through!','14082','1','0','0','32278','krikthir SAY_PREFIGHT_1'),
('-1601008','I hear footsteps. Be on your guard.','14083','1','0','0','32279','krikthir SAY_PREFIGHT_2'),
('-1601009','I sense the living. Be ready.','14084','1','0','0','32280','krikthir SAY_PREFIGHT_3'),
('-1601010','They hunger.','14085','1','0','0','32281','krikthir SAY_SWARM_1'),
('-1601011','Dinner time, my pets.','14086','1','0','0','32282','krikthir SAY_SWARM_2'),
('-1601012','I should be grateful. But I long ago lost the capacity.','14087','1','0','0','32283','krikthir SAY_DEATH'),

('-1601013','%s moves up the tunnel!','0','3','0','0','29852','hadronox EMOTE_MOVE_TUNNEL'),

('-1601014','I was king of this empire once, long ago. In life I stood as champion. In death I returned as conqueror. Now I protect the kingdom once more. Ironic, yes?','14053','1','0','0','0','anubarak SAY_INTRO'),
('-1601015','Eternal agony awaits you!','14054','1','0','0','32308','anubarak SAY_AGGRO'),
('-1601016','You shall experience my torment, first-hand!','14055','1','0','0','32309','anubarak SAY_KILL_1'),
('-1601017','You have made your choice.','14056','1','0','0','32310','anubarak SAY_KILL_2'),
('-1601018','Soon, the Master''s voice will call to you.','14057','1','0','0','32311','anubarak SAY_KILL_3'),
('-1601019','Come forth, my brethren. Feast on their flesh!','14059','1','0','0','32313','anubarak SAY_SUBMERGE_1'),
('-1601020','Auum na-l ak-k-k-k, isshhh.','14058','1','0','0','32312','anubarak SAY_SUBMERGE_2'),
('-1601021','Your armor is useless against my locusts!','14060','1','0','0','32314','anubarak SAY_LOCUST_1'),
('-1601022','The pestilence upon you!','14068','1','0','0','18234','anubarak SAY_LOCUST_2'),
('-1601023','Uunak-hissss tik-k-k-k-k!','14067','1','0','0','32315','anubarak SAY_LOCUST_3'),
('-1601024','Ahhh... RAAAAAGH! Never thought... I would be free of him...','14069','1','0','0','32317','anubarak SAY_DEATH'),

('-1601025','The gate has been breached! Quickly, divert forces to deal with these invaders!','13941','1','0','0','29473','anub''ar crusher SAY_AGGRO'),
('-1601026','There''s no time left! All remaining forces, attack the invaders!','13942','1','0','0','29472','anub''ar crusher SAY_SPECIAL');

-- -1 602 000 HALLS OF LIGHTNING
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1602000','I am the greatest of my father''s sons! Your end has come!','14149','1','0','0','31407','bjarngrim SAY_AGGRO'),
('-1602001','So ends your curse!','14153','1','0','0','31411','bjarngrim SAY_SLAY_1'),
('-1602002','Flesh... is... weak!','14154','1','0','0','31412','bjarngrim SAY_SLAY_2'),
('-1602003','...','14155','1','0','0','11044','bjarngrim SAY_SLAY_3'),
('-1602004','How can it be...? Flesh is not... stronger!','14156','1','0','0','31414','bjarngrim SAY_DEATH'),
('-1602005','Defend yourself, for all the good it will do!','14151','1','0','0','31409','bjarngrim SAY_BATTLE_STANCE'),
('-1602006','%s switches to Battle Stance!','0','3','0','0','29832','bjarngrim EMOTE_BATTLE_STANCE'),
('-1602007','GRAAAAAH! Behold the fury of iron and steel!','14152','1','0','0','31410','bjarngrim SAY_BERSEKER_STANCE'),
('-1602008','%s switches to Berserker Stance!','0','3','0','0','29833','bjarngrim EMOTE_BERSEKER_STANCE'),
('-1602009','Give me your worst!','14150','1','0','0','31408','bjarngrim SAY_DEFENSIVE_STANCE'),
('-1602010','%s switches to Defensive Stance!','0','3','0','0','29834','bjarngrim EMOTE_DEFENSIVE_STANCE'),

('-1602011','You wish to confront the master? You must weather the storm!','14453','1','0','0','30528','ionar SAY_AGGRO'),
('-1602012','Shocking ... I know!','14456','1','0','0','30531','ionar SAY_SLAY_1'),
('-1602013','You atempt the unpossible.','14457','1','0','0','30532','ionar SAY_SLAY_2'),
('-1602014','Your spark of light is ... extinguish.','14458','1','0','0','30533','ionar SAY_SLAY_3'),
('-1602015','Master... you have guests.','14459','1','0','0','30534','ionar SAY_DEATH'),
('-1602016','The slightest spark shall be your undoing.','14454','1','0','0','30529','ionar SAY_SPLIT_1'),
('-1602017','No one is safe!','14455','1','0','0','30530','ionar SAY_SPLIT_2'),

('-1602018','What hope is there for you? None!','14162','1','0','0','30535','loken SAY_AGGRO0'),
('-1602019','I have witnessed the rise and fall of empires. The birth and extinction of entire species. Over countless millennia the foolishness of mortals has remained beyond a constant. Your presence here confirms this.','14160','1','0','0','30536','loken SAY_INTRO_1'),
('-1602020','My master has shown me the future, and you have no place in it. Azeroth will be reborn in darkness. Yogg-Saron shall be released! The Pantheon shall fall!','14161','1','0','0','30537','loken SAY_INTRO_2'),
('-1602021','Only mortal...','14166','1','0','0','30541','loken SAY_SLAY_1'),
('-1602022','I... am... FOREVER!','14167','1','0','0','30542','loken SAY_SLAY_2'),
('-1602023','What little time you had, you wasted!','14168','1','0','0','30543','loken SAY_SLAY_3'),
('-1602024','My death... heralds the end of this world.','14172','1','0','0','30544','loken SAY_DEATH'),
('-1602025','You cannot hide from fate!','14163','1','0','0','30538','loken SAY_NOVA_1'),
('-1602026','Come closer. I will make it quick.','14164','1','0','0','30539','loken SAY_NOVA_2'),
('-1602027','Your flesh cannot hold out for long.','14165','1','0','0','30540','loken SAY_NOVA_3'),
('-1602028','You stare blindly into the abyss!','14169','1','0','0','30545','loken SAY_75HEALTH'),
('-1602029','Your ignorance is profound. Can you not see where this path leads?','14170','1','0','0','30546','loken SAY_50HEALTH'),
('-1602030','You cross the precipice of oblivion!','14171','1','0','0','30547','loken SAY_25HEALTH'),
('-1602031','%s begins to cast Lightning Nova!','0','3','0','0','29865','loken EMOTE_NOVA'),

('-1602032','It is you who have destroyed my children? You... shall... pay!','13960','1','0','0','31415','volkhan SAY_AGGRO'),
('-1602033','The armies of iron will conquer all!','13965','1','0','0','31420','volkhan SAY_SLAY_1'),
('-1602034','Ha, pathetic!','13966','1','0','0','0','volkhan SAY_SLAY_2'),
('-1602035','You have cost me too much work!','13967','1','0','0','31422','volkhan SAY_SLAY_3'),
('-1602036','The master was right... to be concerned.','13968','1','0','0','31423','volkhan SAY_DEATH'),
('-1602037','I will crush you beneath my boots!','13963','1','0','0','31418','volkhan SAY_STOMP_1'),
('-1602038','All my work... undone!','13964','1','0','0','31419','volkhan SAY_STOMP_2'),
('-1602039','Life from the lifelessness... death for you.','13961','1','0','0','31416','volkhan SAY_FORGE_1'),
('-1602040','Nothing is wasted in the process. You will see....','13962','1','0','0','31417','volkhan SAY_FORGE_2'),
('-1602041','%s runs to his anvil!','0','3','0','0','32214','volkhan EMOTE_TO_ANVIL'),
('-1602042','%s prepares to shatter his Brittle Golems!','0','3','0','0','29823','volkhan EMOTE_SHATTER');

-- -1 603 000 ULDUAR
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1603000','The Conservatory must be protected!','15526','1','0','0','33303','freya SAY_AGGRO'),
('-1603001','Elders, grant me your strength!','15527','1','0','0','33520','freya SAY_AGGRO_HARD'),
('-1603002','Eonar, your servant requires aid!','15528','1','0','0','33517','freya SAY_ADDS_CONSERVATOR'),
('-1603003','Children, assist me!','15533','1','0','0','33519','freya SAY_ADDS_TRIO'),
('-1603004','The swarm of the elements shall overtake you!','15534','1','0','0','33518','freya SAY_ADDS_LASHER'),
('-1603005','Forgive me.','15529','1','0','0','13099','freya SAY_SLAY_1'),
('-1603006','From your death springs life anew!','15530','1','0','0','34107','freya SAY_SLAY_2'),
('-1603007','His hold on me dissipates. I can see clearly once more. Thank you, heroes.','15531','1','0','0','33524','freya SAY_DEATH'),
('-1603008','You have strayed too far, wasted too much time!','15532','1','0','0','33605','freya SAY_BERSERK'),
('-1603009','Eonar, your servant calls for your blessing!','15535','1','0','0','34367','freya SAY_HELP_YOGG'),

('-1603010','Allies of Nature have appeared!','0','3','0','0','33427','freya EMOTE_ALLIES_NATURE'),
('-1603011','The %s withers into the earth and begins to regenerate.','0','2','0','0','33463','freya EMOTE_REGEN_ALLIES'),

('-1603012','As you wish, $N.','0','0','0','0','34417','keeper SAY_KEEPER_ACTIVE'),
('-1603013','REUSE ME','0','0','0','0','0','REUSE ME'),

('-1603014','Matron, the Conservatory has been breached!','15483','1','0','0','34115','brightleaf SAY_AGGRO_BRIGHT'),
('-1603015','Fertilizer.','15485','1','0','0','34118','brightleaf SAY_SLAY_1_BRIGHT'),
('-1603016','Your corpse will nourish the soil!','15486','1','0','0','34119','brightleaf SAY_SLAY_2_BRIGHT'),
('-1603017','Matron, one has fallen!','15487','1','0','0','34121','brightleaf SAY_DEATH_BRIGHT'),

('-1603018','Mortals have no place here!','15493','1','0','0','34123','ironbranch SAY_AGGRO_IRON'),
('-1603019','I return you whence you came!','15494','1','0','0','34124','ironbranch SAY_SLAY_1_IRON'),
('-1603020','BEGONE!','15495','1','0','0','34125','ironbranch SAY_SLAY_2_IRON'),
('-1603021','Freya! They come for you.','15496','1','0','0','34126','ironbranch SAY_DEATH_IRON'),

('-1603022','This place will serve as your graveyard.','15500','1','0','0','34129','stonebark SAY_AGGRO_STONE'),
('-1603023','<Angry roar>','15501','1','0','0','0','stonebark SAY_SLAY_1_STONE'),
('-1603024','Such a waste.','15502','1','0','0','34130','stonebark SAY_SLAY_2_STONE'),
('-1603025','Matron, flee! They are ruthless....','15503','1','0','0','34132','stonebark SAY_DEATH_STONE'),

('-1603026','Insolent whelps! Your blood will temper the weapons used to reclaim this world!','15564','1','0','0','34306','ignis SAY_AGGRO'),
('-1603027','Let the inferno consume you!','15567','1','0','0','34309','ignis SAY_SCORCH_1'),
('-1603028','BURN! Burn in the makers fire!','15568','1','0','0','34310','ignis SAY_SCORCH_2'),
('-1603029','I will burn away your impurities!','15566','1','0','0','34308','ignis SAY_SLAGPOT'),
('-1603030','Arise, soldiers of the Iron Crucible! The Makers'' will be done!','15565','1','0','0','34307','ignis SAY_ADDS'),
('-1603031','More scraps for the scrapheap!','15569','1','0','0','34311','ignis SAY_SLAY_1'),
('-1603032','Your bones will serve as kindling!','15570','1','0','0','34312','ignis SAY_SLAY_2'),
('-1603033','Let it be finished!','15571','1','0','0','33871','ignis SAY_BERSERK'),
('-1603034','I. Have. Failed.','15572','1','0','0','34313','ignis SAY_DEATH'),
('-1603035','Ignis the Furnace Master begins to cast Flame Jets!','0','3','0','0','0','ignis EMOTE_FLAME_JETS'),

('-1603036','Welcome, champions! All of our attempts at grounding her have failed. We could use a hand in bring her down with these harpoon guns.','15647','0','0','0','0','razorscale SAY_INTRO_WELCOME'),
('-1603037','Give us a moment to prepare to build the turrets.','0','1','0','0','33820','razorscale SAY_INTRO_1'),
('-1603038','Be on the lookout! Mole machines will be surfacing soon with those nasty Iron dwarves aboard!','0','1','0','0','33607','razorscale SAY_INTRO_2'),
('-1603039','Ready to move out, keep those dwarves off of our backs!','0','1','0','0','33816','razorscale SAY_INTRO_3'),
('-1603040','Move quickly! She won''t remain grounded for long!','15648','1','0','0','33606','razorscale SAY_GROUNDED'),
('-1603041','Razorscale takes a deep breath...','0','3','0','0','0','razorscale EMOTE_BREATH'),
('-1603042','Fires out! Let''s rebuild those turrets!','0','1','0','0','33821','razorscale SAY_EXTINGUISH_FIRE'),
('-1603043','Harpoon Turret is ready for use!','0','3','0','0','33857','razorscale EMOTE_HARPOON_READY'),
('-1603044','Razorscale grounded permanently!','0','3','0','0','0','razorscale EMOTE_GROUNDED'),

('-1603045','New toys? For me? I promise I won''t break them this time!','15724','1','0','0','33508','xt-002 SAY_AGGRO'),
('-1603046','I... I think I broke it.','15728','1','0','0','33509','xt-002 SAY_SLAY_1'),
('-1603047','I guess it doesn''t bend that way.','15729','1','0','0','33510','xt-002 SAY_SLAY_2'),
('-1603048','I''m tired of these toys. I don''t want to play anymore!','15730','1','0','0','33512','xt-002 SAY_BERSERK'),
('-1603049','Time for a new game! My old toys will fight my new toys!','15732','1','0','0','0','xt-002 SAY_ADDS'),
('-1603050','You are bad... Toys... Very... Baaaaad!','15731','1','0','0','33511','xt-002 SAY_DEATH'),
('-1603051','So tired. I will rest for just a moment!','15725','1','0','0','33446','xt-002 SAY_HEART_OPEN'),
('-1603052','I''m ready to play!','15726','1','0','0','33448','xt-002 SAY_HEART_CLOSE'),
('-1603053','NO! NO! NO! NO! NO!','15727','1','0','0','33453','xt-002 SAY_TANCTRUM'),
('-1603054','%s''s heart is exposed and leaking energy.','0','3','0','0','33447','xt-002 EMOTE_EXPOSE_HEART'),
('-1603055','%s consumes a scrapbot to repair himself!','0','3','0','0','33856','xt-002 EMOTE_REPAIR'),

('-1603056','Whether the world''s greatest gnats or the world''s greatest heroes, you''re still only mortal.','15684','1','0','0','0','brundir SAY_BRUNDIR_AGGRO'),
('-1603057','Stand still and stare into the light!','15687','1','0','0','33962','brundir SAY_BRUNDIR_WHIRL'),
('-1603058','The power of the storm lives on...','15689','1','0','0','34318','brundir SAY_BRUNDIR_DEATH_1'),
('-1603059','You rush headlong into the maw of madness!','15690','1','0','0','34319','brundir SAY_BRUNDIR_DEATH_2'),
('-1603060','A merciful kill!','15685','1','0','0','34315','brundir SAY_BRUNDIR_SLAY_1'),
('-1603061','HAH!','15686','1','0','0','34316','brundir SAY_BRUNDIR_SLAY_2'),
('-1603062','This meeting of the Assembly of Iron is adjourned!','15691','1','0','0','34320','brundir SAY_BRUNDIR_BERSERK'),
('-1603063','Let the storm clouds rise and rain down death from above!','15688','1','0','0','34317','brundir SAY_BRUNDIR_FLY'),

('-1603064','Nothing short of total decimation will suffice!','15657','1','0','0','34328','molgeim SAY_MOLGEIM_AGGRO'),
('-1603065','The legacy of storms shall not be undone...','15662','1','0','0','34333','molgeim SAY_MOLGEIM_DEATH_1'),
('-1603066','What have you gained from my defeat? You are no less doomed, mortals...','15663','1','0','0','34334','molgeim SAY_MOLGEIM_DEATH_2'),
('-1603067','Decipher this!','15660','1','0','0','34331','molgeim SAY_MOLGEIM_DEATH_RUNE'),
('-1603068','Face the lightning surge!','15661','1','0','0','0','molgeim SAY_MOLGEIM_SURGE'),
('-1603069','The world on suffers yet another insignificant loss!','15658','1','0','0','0','molgeim SAY_MOLGEIM_SLAY_1'),
('-1603070','Death is the price of your arrogance.','15659','1','0','0','34330','molgeim SAY_MOLGEIM_SLAY_2'),
('-1603071','This meeting of the Assembly of Iron is adjourned!','15664','1','0','0','34320','molgeim SAY_MOLGEIM_BERSERK'),

('-1603072','You will not defeat the Assembly of Iron so easily, invaders!','15674','1','0','0','34321','steelbreaker SAY_STEEL_AGGRO'),
('-1603073','My death only serves to hasten your demise.','15678','1','0','0','34325','steelbreaker SAY_STEEL_DEATH_1'),
('-1603074','Impossible!','15679','1','0','0','5673','steelbreaker SAY_STEEL_DEATH_2'),
('-1603075','So fragile and weak!','15675','1','0','0','34322','steelbreaker SAY_STEEL_SLAY_1'),
('-1603076','Flesh... such a hindrance.','15676','1','0','0','34323','steelbreaker SAY_STEEL_SLAY_2'),
('-1603077','You seek the secrets of Ulduar? Then take them!','15677','1','0','0','34324','steelbreaker SAY_STEEL_OVERWHELM'),
('-1603078','This meeting of the Assembly of Iron is adjourned!','15680','1','0','0','34320','steelbreaker SAY_STEEL_BERSERK'),

('-1603079','Some things are better left alone!','15473','1','0','0','34341','auriaya SAY_AGGRO'),
('-1603080','There is no escape!','15475','1','0','0','4211','auriaya SAY_SLAY_1'),
('-1603081','The secret dies with you!','15474','1','0','0','34354','auriaya SAY_SLAY_2'),
('-1603082','You waste my time!','15477','1','0','0','34358','auriaya SAY_BERSERK'),
('-1603083','Auriaya screams in agony.','15476','2','0','0','0','auriaya SAY_DEATH'),
('-1603084','Auriaya begins to cast Terrifying Screech.','0','3','0','0','0','auriaya EMOTE_SCREECH'),
('-1603085','Auriaya begins to activate the Feral Defender!','0','3','0','0','0','auriaya EMOTE_DEFENDER'),

('-1603086','You will suffer for this trespass!','15552','1','0','0','34336','hodir SAY_AGGRO'),
('-1603087','Tragic. To come so far, only to fail.','15553','1','0','0','34337','hodir SAY_SLAY_1'),
('-1603088','Welcome to the endless winter.','15554','1','0','0','34338','hodir SAY_SLAY_2'),
('-1603089','Winds of the north consume you!','15555','1','0','0','34339','hodir SAY_FLASH_FREEZE'),
('-1603090','<Furious Roar>','15556','2','0','0','0','hodir SAY_FROZEN_BLOWS'),
('-1603091','I... I am released from his grasp... at last.','15557','1','0','0','33484','hodir SAY_DEATH'),
('-1603092','Enough! This ends now!','15558','1','0','0','34340','hodir SAY_BERSERK'),
('-1603093','The veil of winter will protect you, champions!','15559','1','0','0','34366','hodir SAY_HELP_YOGG'),
('-1603094','Hodir begins to cast Flash Freeze!','0','3','0','0','0','hodir EMOTE_FLASH_FREEZE'),
('-1603095','Hodir gains Frozen Blows!','0','3','0','0','0','hodir EMOTE_FROZEN_BLOWS'),

('-1603096','Your destruction will herald a new age of suffering!','15542','1','0','0','34053','vezax SAY_AGGRO'),
('-1603097','You thought to stand before the legions of death... and survive?','15543','1','0','0','34050','vezax SAY_SLAY_1'),
('-1603098','Defiance... a flaw of mortality.','15544','1','0','0','34051','vezax SAY_SLAY_2'),
('-1603099','The black blood of Yogg-Saron courses through me! I. AM. UNSTOPPABLE!','15545','1','0','0','34055','vezaz SAY_SURGE'),
('-1603100','Oh, what horrors await....','15546','1','0','0','34054','vezax SAY_DEATH'),
('-1603101','Your defeat was inevitable!','15547','1','0','0','34052','vezax SAY_ENRAGE'),
('-1603102','Behold, now! Terror, absolute!','15548','1','0','0','0','vezax SAY_HARD_MODE'),
('-1603103','A cloud of saronite vapors coalesces nearby!','0','3','0','0','33587','vezax EMOTE_VAPOR'),
('-1603104','General Vezax roars and surges with dark might!','0','3','0','0','33471','vezax EMOTE_SURGE'),
('-1603105','The saronite vapors mass and swirl violently, merging into a monstrous form!','0','3','0','0','33594','vezax EMOTE_ANIMUS'),

('-1603106','Translocation complete. Commencing planetary analysis of Azeroth.','15405','1','0','0','34026','algalon SAY_INTRO_1'),
('-1603107','Stand back, mortals. I''m not here to fight you.','15406','1','0','0','34027','algalon SAY_INTRO_2'),
('-1603108','It is in the universe''s best interest to re-originate this planet should my analysis find systemic corruption. Do not interfere.','15407','1','0','0','34028','algalon SAY_INTRO_3'),

('-1603109','See your world through my eyes. A universe so vast as to be immeasurable. Incomprehensible even to your greatest minds.','15390','1','0','0','0','algalon SAY_ENGAGE'),
('-1603110','Your actions are illogical. All possible results for this encounter have been calculated. The pantheon will receive the observer''s message regardless outcome.','15386','1','0','0','0','algalon SAY_AGGRO'),
('-1603111','Loss of life, unavoidable.','15387','1','0','0','33996','algalon SAY_SLAY_1'),
('-1603112','I do what I must.','15388','1','0','0','33997','algalon SAY_SLAY_2'),
('-1603113','The stars come to my aid.','15392','1','0','0','33245','algalon SAY_SUMMON_STAR'),
('-1603114','Witness the fury of cosmos!','15396','1','0','0','33269','algalon SAY_BIG_BANG_1'),
('-1603115','Behold the tools of creation!','15397','1','0','0','33994','algalon SAY_BIG_BANG_2'),
('-1603116','Beware!','15391','1','0','0','34838','algalon SAY_PHASE_2'),
('-1603117','You are... out of time.','15394','1','0','0','73002','algalon SAY_BERSERK'),

('-1603118','Analysis complete. There is partial corruption in the plane''s life-support systems as well as complete corruption in most of the planet''s defense mechanisms.','15398','1','0','0','0','algalon SAY_DESPAWN_1'),
('-1603119','Begin uplink: Reply Code: ''Omega''. Planetary re-origination requested.','15399','1','0','0','34014','algalon SAY_DESPAWN_2'),
('-1603120','Farewell, mortals. Your bravery is admirable, for such flawed creatures.','15400','1','0','0','34015','algalon SAY_DESPAWN_3'),

('-1603121','I have seen worlds bathed in the Makers'' flames, their denizens fading without as much as a whimper. Entire planetary systems born and razed in the time that it takes your mortal hearts to beat once. Yet all throughout, my own heart devoid of emotion... of empathy. I. Have. Felt. Nothing. A million-million lives wasted. Had they all held within them your tenacity? Had they all loved life as you do?','15393','1','0','0','34000','algalon SAY_OUTRO_1'),
('-1603122','Perhaps it is your imperfections... that which grants you free will... that allows you to persevere against all cosmically calculated odds. You prevail where the Titan''s own perfect creations have failed.','15401','1','0','0','34012','algalon SAY_OUTRO_2'),
('-1603123','I''ve rearranged the reply code - your planet will be spared. I cannot be certain of my own calculations anymore.','15402','1','0','0','34013','algalon SAY_OUTRO_3'),
('-1603124','I lack the strength to transmit the signal. You must... hurry... find a place of power... close to the skies.','15403','1','0','0','34016','algalon SAY_OUTRO_4'),
('-1603125','Do not worry about my fate, Bronzen. If the signal is not transmitted in time, re-origination will proceed regardless. Save... your world...','15404','1','0','0','34018','algalon SAY_OUTRO_5'),

('-1603126','None shall pass!','15586','1','0','0','13052','kologarn SAY_AGGRO'),
('-1603127','OBLIVION!','15591','1','0','0','25555','kologarn SAY_SHOCKWAVE'),
('-1603128','I will squeeze the life from you!','15592','1','0','0','34138','kologarn SAY_GRAB'),
('-1603129','Just a scratch!','15589','1','0','0','34143','kologarn SAY_ARM_LOST_LEFT'),
('-1603130','Only a flesh wound!','15590','1','0','0','34144','kologarn SAY_ARM_LOST_RIGHT'),
('-1603131','KOL-THARISH!','15587','1','0','0','34135','kologarn SAY_SLAY_1'),
('-1603132','YOU FAIL!','15588','1','0','0','17577','kologarn SAY_SLAY_2'),
('-1603133','I am invincible!','15594','1','0','0','25427','kologarn SAY_BERSERK'),
('-1603134','Master, they come...','15593','1','0','0','34141','kologarn SAY_DEATH'),
('-1603135','The Right Arm has regrown!','0','3','0','0','34296','kologarn EMOTE_ARM_RIGHT'),
('-1603136','The Left Arm has regrown!','0','3','0','0','34295','kologarn EMOTE_ARM_LEFT'),
('-1603137','Kologarn casts Stone Grip!','0','3','0','0','33864','kologarn EMOTE_STONE_GRIP'),

('-1603138','Interlopers! You mortals who dare to interfere with my sport will pay... Wait--you...','15733','1','0','0','33145','thorim SAY_AGGRO_1'),
('-1603139','I remember you... In the mountains... But you... what is this? Where am--','15734','1','0','0','33270','thorim SAY_AGGRO_2'),

('-1603140','Behold the power of the storms and despair!','15735','1','0','0','0','thorim SAY_SPECIAL_1'),
('-1603141','Do not hold back! Destroy them!','15736','1','0','0','34241','thorim SAY_SPECIAL_2'),
('-1603142','Have you begun to regret your intrusion?','15737','1','0','0','0','thorim SAY_SPECIAL_3'),

('-1603143','Impertinent whelps! You dare challenge me atop my pedestal! I will crush you myself!','15738','1','0','0','0','thorim SAY_JUMP'),
('-1603144','Can''t you at least put up a fight!?','15739','1','0','0','34239','thorim SAY_SLAY_1'),
('-1603145','Pathetic!','15740','1','0','0','70319','thorim SAY_SLAY_2'),
('-1603146','My patience has reached its limit!','15741','1','0','0','33365','thorim SAY_BERSERK'),

('-1603147','Failures! Weaklings!','15742','1','0','0','33274','thorim SAY_ARENA_WIPE'),
('-1603148','Stay your arms! I yield!','15743','1','0','0','33948','thorim SAY_DEFEATED'),

('-1603149','I feel as though I am awakening from a nightmare, but the shadows in this place yet linger.','15744','1','0','0','33949','thorim SAY_OUTRO_1'),
('-1603150','Sif... was Sif here? Impossible--she died by my brother''s hand. A dark nightmare indeed....','15745','1','0','0','0','thorim SAY_OUTRO_2'),
('-1603151','I need time to reflect.... I will aid your cause if you should require it. I owe you at least that much. Farewell.','15746','1','0','0','33951','thorim SAY_OUTRO_3'),

('-1603152','You! Fiend! You are not my beloved! Be gone!','15747','1','0','0','33952','thorim SAY_OUTRO_HARD_1'),
('-1603153','Behold the hand behind all the evil that has befallen Ulduar! Left my kingdom in ruins, corrupted my brother and slain my wife!','15748','1','0','0','33953','thorim SAY_OUTRO_HARD_2'),
('-1603154','And now it falls to you, champions, to avenge us all! The task before you is great, but I will lend you my aid as I am able. You must prevail!','15749','1','0','0','33954','thorim SAY_OUTRO_HARD_3'),

('-1603155','Golganneth, lend me your strengh! Grant my mortal allies the power of thunder!','15750','1','0','0','0','thorim SAY_HELP_YOGG'),

('-1603156','Thorim, my lord, why else would these invaders have come into your sanctum but to slay you? They must be stopped!','15668','1','0','0','0','thorim SAY_SIF_BEGIN'),
('-1603157','Impossible! Lord Thorim, I will bring your foes a frigid death!','15670','1','0','0','33369','thorim SAY_SIF_EVENT'),
('-1603158','These pathetic mortals are harmless, beneath my station. Dispose of them!','15669','1','0','0','33368','thorim SAY_SIF_DESPAWN'),

('-1603159','Hostile entities detected. Threat assessment protocol active. Primary target engaged. Time minus thirty seconds to re-evaluation.','15506','1','0','0','0','leviathan SAY_AGGRO'),
('-1603160','Threat assessment routine modified. Current target threat level: zero. Acquiring new target.','15521','1','0','0','0','leviathan SAY_SLAY'),
('-1603161','Total systems failure. Defense protocols breached. Leviathan Unit shutting down.','15520','1','0','0','33506','leviathan SAY_DEATH'),
('-1603162','Threat re-evaluated. Target assessment complete. Changing course.','15507','1','0','0','33488','leviathan SAY_CHANGE_1'),
('-1603163','Pursuit objective modified. Changing course.','15508','1','0','0','33489','leviathan SAY_CHANGE_2'),
('-1603164','Hostile entity stratagem predicted. Rerouting battle function. Changing course.','15509','1','0','0','0','leviathan SAY_CHANGE_3'),
('-1603165','Unauthorized entity attempting circuit overload. Activating anti-personnel countermeasures.','15516','1','0','0','33501','leviathan SAY_PLAYER_RIDE'),
('-1603166','System malfunction. Diverting power to support systems.','15517','1','0','0','33503','leviathan SAY_OVERLOAD_1'),
('-1603167','Combat matrix overload. Powering do-o-o-own...','15518','1','0','0','0','leviathan SAY_OVERLOAD_2'),
('-1603168','System restart required. Deactivating weapon systems.','15519','1','0','0','0','leviathan SAY_OVERLOAD_3'),
('-1603169','Orbital countermeasures enabled.','15510','1','0','0','33491','leviathan SAY_HARD_MODE'),
('-1603170','\'Hodir''s Fury\' online. Acquiring target.','15512','1','0','0','33493','leviathan SAY_TOWER_FROST'),
('-1603171','\'Mimiron''s Inferno\' online. Acquiring target.','15513','1','0','0','33495','leviathan SAY_TOWER_FIRE'),
('-1603172','\'Thorim''s Hammer\' online. Acquiring target.','15515','1','0','0','33499','leviathan SAY_TOWER_ENERGY'),
('-1603173','\'Freya''s Ward\' online. Acquiring target.','15514','1','0','0','33497','leviathan SAY_TOWER_NATURE'),
('-1603174','Alert! Static defense system failure. Orbital countermeasures disabled.','15511','1','0','0','0','leviathan SAY_TOWER_DOWN'),
('-1603175','%s pursues $N','0','3','0','0','33502','leviathan EMOTE_PURSUE'),

('-1603176','Oh, my! I wasn''t expecting company! The workshop is such a mess! How embarrassing!','15611','1','0','0','34298','mimiron SAY_AGGRO'),
('-1603177','Now why would you go and do something like that? Didn''t you see the sign that said ''DO NOT PUSH THIS BUTTON!''? How will we finish testing with the self-destruct mechanism active?','15629','1','0','0','0','mimiron SAY_HARD_MODE'),
('-1603178','Oh, my! It would seem that we are out of time, my friends!','15628','1','0','0','34305','mimiron SAY_BERSERK'),

('-1603179','We haven''t much time, friends! You''re going to help me test out my latest and greatest creation. Now, before you change your minds, remember, that you kind of owe it to me after the mess you made with the XT-002.','15612','1','0','0','33636','mimiron SAY_TANK_ACTIVE'),
('-1603180','MEDIC!','15613','1','0','0','34299','mimiron SAY_TANK_SLAY_1'),
('-1603181','I can fix that... or, maybe not! Sheesh, what a mess...','15614','1','0','0','34300','mimiron SAY_TANK_SLAY_2'),
('-1603182','WONDERFUL! Positively marvelous results! Hull integrity at 98.9 percent! Barely a dent! Moving right along.','15615','1','0','0','33638','mimiron SAY_TANK_DEATH'),

('-1603183','Behold the VX-001 Anti-personnel Assault Cannon! You might want to take cover.','15616','1','0','0','33639','mimiron SAY_TORSO_ACTIVE'),
('-1603184','Fascinating. I think they call that a ''clean kill''.','15617','1','0','0','0','mimiron SAY_TORSO_SLAY_1'),
('-1603185','Note to self: Cannon highly effective against flesh.','15618','1','0','0','34302','mimiron SAY_TORSO_SLAY_2'),
('-1603186','Thank you, friends! Your efforts have yielded some fantastic data! Now, where did I put-- oh, there it is!','15619','1','0','0','33668','mimiron SAY_TORSO_DEATH'),

('-1603187','Isn''t it beautiful? I call it the magnificent aerial command unit!','15620','1','0','0','33669','mimiron SAY_HEAD_ACTIVE'),
('-1603188','Outplayed!','15621','1','0','0','34303','mimiron SAY_HEAD_SLAY_1'),
('-1603189','You can do better than that!','15622','1','0','0','34304','mimiron SAY_HEAD_SLAY_2'),
('-1603190','Preliminary testing phase complete. Now comes the true test!!','15623','1','0','0','33966','mimiron SAY_HEAD_DEATH'),

('-1603191','Gaze upon its magnificence! Bask in its glorious, um, glory! I present you... V-07-TR-0N!','15624','1','0','0','0','mimiron SAY_ROBOT_ACTIVE'),
('-1603192','Prognosis: Negative!','15625','1','0','0','34448','mimiron SAY_ROBOT_SLAY_1'),
('-1603193','You''re not going to get up from that one, friend.','15626','1','0','0','34449','mimiron SAY_ROBOT_SLAY_2'),
('-1603194','It would appear that I''ve made a slight miscalculation. I allowed my mind to be corrupted by the fiend in the prison, overriding my primary directive. All systems seem to be functional now. Clear.','15627','1','0','1','34086','mimiron SAY_ROBOT_DEATH'),

('-1603195','Combat matrix enhanced. Behold wonderous rapidity!','15630','1','0','0','34368','mimiron SAY_HELP_YOGG'),
('-1603196','%s begins to cast Plasma Blast!','0','3','0','0','34217','mimiron EMOTE_PLASMA_BLAST'),

('-1603197','Aaaaaaaaaaaaaaaaa... Help me!!! Please got to help me!','15771','1','0','0','0','yogg SAY_SARA_INTRO_1'),
('-1603198','What do you want from me? Leave me alone!','15772','1','0','0','34343','yogg SAY_SARA_INTRO_2'),
('-1603199','The time to strike at the head of the beast will soon be upon us! Focus your anger and hatred on his minions!','15775','1','0','0','34346','yogg SAY_SARA_AGGRO'),
('-1603201','Yes! YES! Show them no mercy! Give no pause to your attacks!','15773','1','0','0','34344','yogg SAY_SARA_HELP_1'),
('-1603202','Let hatred and rage guide your blows!','15774','1','0','0','34345','yogg SAY_SARA_HELP_2'),
('-1603203','Could they have been saved?','15779','1','0','0','34350','yogg SAY_SARA_SLAY_1'),
('-1603204','Powerless to act...','15778','1','0','0','34349','yogg SAY_SARA_SLAY_2'),

('-1603205','Weak-minded fools!','15780','4','0','0','0','yogg SAY_WIPE_PHASE_1'),

('-1603206','I am the lucid dream.','15754','1','0','457','34351','yogg SAY_PHASE_2_INTRO_1'),
('-1603207','Tremble, mortals, before the coming of the end!','15777','1','0','0','34348','yogg SAY_SARA_PHASE_2_INTRO_A'),
('-1603208','Suffocate upon your own hate!','15776','1','0','0','34347','yogg SAY_SARA_PHASE_2_INTRO_B'),

('-1603209','MADNESS WILL CONSUME YOU!','15756','1','0','0','34361','yogg SAY_MADNESS'),
('-1603210','Look upon the true face of death and know that your end comes soon!','15755','1','0','0','34360','yogg SAY_PHASE_3'),
('-1603211','%s prepares to unleash Empowering Shadows!','0','3','0','0','34170','yogg EMOTE_EMPOWERING_SHADOWS'),
('-1603212','Eternal suffering awaits!','15758','1','0','0','34362','yogg SAY_SLAY_2'),
('-1603213','Your fate is sealed. The end of days is finally upon you and ALL who inhabit this miserable little seedling. Uulwi ifis halahs gag erh''ongg w''ssh.','15761','1','0','0','34365','yogg SAY_DEATH'),
('-1603214','Your will is no longer you own...','15759','4','0','0','0','yogg SAY_TO_INSANE_1'),
('-1603215','Destroy them minion, your master commands it!','15760','4','0','0','34364','yogg SAY_TO_INSANE_2'),

('-1603216','Your resilience is admirable.','15598','0','0','0','33608','yogg SAY_LICH_KING_1'),
('-1603217','Arrrrrrgh!','15470','1','0','0','33610','yogg SAY_CHAMPION_1'),
('-1603218','I''m not afraid of you!','15471','0','0','0','20070','yogg SAY_CHAMPION_2'),
('-1603219','I will break you as I broke him.','15599','0','0','0','33609','yogg SAY_LICH_KING_2'),
('-1603220','Yrr n''lyeth... shuul anagg!','15766','0','0','0','33628','yogg SAY_YOGG_V3_1'),
('-1603221','He will learn... no king rules forever; only death is eternal!','15767','0','0','0','33629','yogg SAY_YOGG_V3_2'),

('-1603222','It is done... All have been given that which must be given. I now seal the Dragon Soul forever...','15631','0','0','1','0','yogg SAY_NELTHARION_1'),
('-1603223','That terrible glow... should that be?','15784','0','0','1','33658','yogg SAY_YSERA'),
('-1603224','For it to be as it must, yes.','15632','0','0','1','33659','yogg SAY_NELTHARION_2'),
('-1603225','It is a weapon like no other. It must be like no other.','15610','0','0','1','33660','yogg SAY_MALYGOS'),
('-1603226','His brood learned their lesson before too long, you shall soon learn yours!','15765','0','0','0','0','yogg SAY_YOGG_V2'),

('-1603227','Bad news sire.','15538','0','0','1','33664','yogg SAY_GARONA_1'),
('-1603228','Gul''dan is bringing up his warlocks by nightfall. Until then, the Blackrock clan will be trying to take the Eastern Wall.','15540','0','0','1','33666','yogg SAY_GARONA_3'),
('-1603229','A thousand deaths... ','15762','0','0','0','33616','yogg SAY_YOGG_V1_1'),
('-1603230','or one murder.','15763','0','0','0','33617','yogg SAY_YOGG_V1_2'),
('-1603231','We will hold until the reinforcements come. As long as men with stout hearts are manning the walls and throne Stormwind will hold.','15585','0','0','1','0','yogg SAY_KING_LLANE'),
('-1603232','The orc leaders agree with your assessment.','15541','0','0','0','33667','yogg SAY_GARONA_4'),
('-1603233','Your petty quarrels only make me stronger!','15764','0','0','0','34188','yogg SAY_YOGG_V1_3'),

('-1603234','Portals open into Yogg-Saron''s mind!','0','3','0','0','0','yogg EMOTE_VISION_BLAST'),
('-1603235','The illusion shatters and a path to the central chamber opens!','0','3','0','0','34294','yogg EMOTE_SHATTER_BLAST'),

('-1603236','%s''s heart is severed from his body.','0','3','0','0','34023','xt-002 EMOTE_KILL_HEART'),
('-1603237','%s begins to cause the earth to quake.','0','3','0','0','33454','xt-002 EMOTE_EARTH_QUAKE'),
('-1603238','%s is extinguished by the water!','0','2','0','0','0','ignis EMOTE_EXTINGUISH_SCORCH'),

('-1603239','You''ve done it! You''ve broken the defenses of Ulduar. In a few moments, we will be dropping in to...','15804','0','0','0','34154','bronzebeard radio SAY_PRE_LEVIATHAN_1'),
('-1603240','What is that? Be careful! Something''s headed your way!','15805','0','0','0','34155','bronzebeard radio SAY_PRE_LEVIATHAN_2'),
('-1603241','Quickly! Evasive action! Evasive act--','15806','0','0','0','34156','bronzebeard radio SAY_PRE_LEVIATHAN_3'),

('-1603242','%s activates Hodir''s Fury.','0','3','0','0','33494','leviathan EMOTE_HODIR_FURY'),
('-1603243','%s activates Freya''s Ward.','0','3','0','0','33498','leviathan EMOTE_FREYA_WARD'),
('-1603244','%s activates Mimiron''s Inferno.','0','3','0','0','33496','leviathan EMOTE_MIMIRON_INFERNO'),
('-1603245','%s activates Thorim''s Hammer.','0','3','0','0','33500','leviathan EMOTE_THORIM_HAMMER'),

('-1603246','I know just the place. Will you be all right?','15823','1','0','0','34017','brann SAY_BRANN_OUTRO'),

('-1603247','%s surrounds itself with a crackling Runic Barrier!','0','3','0','0','33267','thorim EMOTE_RUNIC_BARRIER'),

('-1603248','Self-destruct sequence initiated.','15413','1','0','0','26909','mimiron SAY_SELF_DESTRUCT'),
('-1603249','This area will self-destruct in ten minutes.','15415','1','0','0','34283','mimiron SAY_DESTRUCT_10_MIN'),
('-1603250','This area will self-destruct in nine minutes.','15416','1','0','0','34282','mimiron SAY_DESTRUCT_9_MIN'),
('-1603251','This area will self-destruct in eight minutes.','15417','1','0','0','34281','mimiron SAY_DESTRUCT_8_MIN'),
('-1603252','This area will self-destruct in seven minutes.','15418','1','0','0','34280','mimiron SAY_DESTRUCT_7_MIN'),
('-1603253','This area will self-destruct in six minutes.','15419','1','0','0','34273','mimiron SAY_DESTRUCT_6_MIN'),
('-1603254','This area will self-destruct in five minutes.','15420','1','0','0','34274','mimiron SAY_DESTRUCT_5_MIN'),
('-1603255','This area will self-destruct in four minutes.','15421','1','0','0','34275','mimiron SAY_DESTRUCT_4_MIN'),
('-1603256','This area will self-destruct in three minutes.','15422','1','0','0','34276','mimiron SAY_DESTRUCT_3_MIN'),
('-1603257','This area will self-destruct in two minutes.','15423','1','0','0','34277','mimiron SAY_DESTRUCT_2_MIN'),
('-1603258','This area will self-destruct in one minute.','15424','1','0','0','34278','mimiron SAY_DESTRUCT_1_MIN'),
('-1603259','Self-destruct sequence finalized. Have a nice day.','15425','1','0','0','34279','mimiron SAY_DESTRUCT_0_MIN'),
('-1603260','Self-destruct sequence terminated. Overide code A905.','15414','1','0','0','0','mimiron SAY_SELF_DESTRUCT_END'),

('-1603261','%s begins to boil upon touching $n!','0','2','0','0','35163','ominous cloud EMOTE_CLOUD_BOIL'),
('-1603262','The monster in your nightmares.','0','1','0','457','34352','yogg SAY_PHASE_2_INTRO_2'),
('-1603263','The fiend of a thousand faces.','0','1','0','457','34353','yogg SAY_PHASE_2_INTRO_3'),
('-1603264','Cower before my true form.','0','1','0','457','34356','yogg SAY_PHASE_2_INTRO_4'),
('-1603265','BOW DOWN BEFORE THE GOD OF DEATH!','0','1','0','0','34357','yogg SAY_PHASE_2_INTRO_5'),
('-1603266','%s opens his mouth wide!','0','3','0','0','34022','yogg EMOTE_DEAFENING_ROAR'),
('-1603267','The clans are united under Blackhand in this assault. They will stand together until Stormwind has fallen.','15539','0','0','1','33665','yogg SAY_GARONA_2');

-- -1 604 000 GUNDRAK
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1604000','Drakkari gonna kill anybody who trespass on these lands!','14443','1','0','0','31711','sladran SAY_AGGRO'),
('-1604001','Minions of the scale, heed my call!','14444','1','0','0','30369','sladran SAY_SUMMON_SNAKE'),
('-1604002','Our thousand fangs gonna rend your flesh! ','14445','1','0','0','30370','sladran SAY_SUMMON_CONSTRICTOR'),
('-1604003','Ye not breathin''! Good.','14446','1','0','0','31712','sladran SAY_SLAY_1'),
('-1604004','You scared now?','14447','1','0','0','31713','sladran SAY_SLAY_2'),
('-1604005','I''ll eat you next, mon!','14448','1','0','0','31714','sladran SAY_SLAY_3'),
('-1604006','I sssee now... Ssscourge wasss not... our greatessst enemy...','14449','1','0','0','31715','sladran SAY_DEATH'),
('-1604007','%s begins to cast Poison Nova!','0','3','0','0','30757','sladran EMOTE_NOVA'),

('-1604008','%s surges forward!','0','2','0','0','30717','colossus EMOTE_SURGE'),
('-1604009','%s seep into the ground.','0','2','0','0','31226','colossus EMOTE_SEEP'),
('-1604010','%s begins to glow faintly.','0','2','0','0','31199','colossus EMOTE_GLOW'),

('-1604011','We fought back da Scourge. What chance joo be thinkin'' JOO got?','14721','1','0','0','31427','moorabi SAY_AGGRO'),
('-1604012','Da ground gonna swallow you up! ','14723','1','0','0','31429','moorabi SAY_QUAKE'),
('-1604013','Get ready for somethin''... much... BIGGAH!','14722','1','0','0','31428','moorabi SAY_TRANSFORM'),
('-1604014','I crush you, cockroaches!','14725','1','0','0','31430','moorabi SAY_SLAY_1'),
('-1604015','Who gonna stop me; you?','14726','1','0','0','31431','moorabi SAY_SLAY_2'),
('-1604016','Not so tough now.','14727','1','0','0','31432','moorabi SAY_SLAY_3'),
('-1604017','If our gods can die... den so can we...','14728','1','0','0','31433','moorabi SAY_DEATH'),
('-1604018','%s begins to transform!','0','3','0','0','30385','moorabi EMOTE_TRANSFORM'),

('-1604019','I''m gonna spill your guts, mon!','14430','1','0','0','32534','galdarah SAY_AGGRO'),
('-1604020','Ain''t gonna be nottin'' left after this!','14431','1','0','0','32535','galdarah SAY_TRANSFORM_1'),
('-1604021','You wanna see power? I''m gonna show you power!','14432','1','0','0','32536','galdarah SAY_TRANSFORM_2'),
('-1604022','Gut them! Impale them!','14433','1','0','0','32537','galdarah SAY_SUMMON_1'),
('-1604023','Kill them all!','14434','1','0','0','1690','galdarah SAY_SUMMON_2'),
('-1604024','Say hello to my BIG friend!','14435','1','0','0','32539','galdarah SAY_SUMMON_3'),
('-1604025','What a rush!','14436','1','0','0','32540','galdarah SAY_SLAY_1'),
('-1604026','Who needs gods, when WE ARE GODS!','14437','1','0','0','32541','galdarah SAY_SLAY_2'),
('-1604027','I told ya so!','14438','1','0','0','32542','galdarah SAY_SLAY_3'),
('-1604028','Even the mighty... can fall.','14439','1','0','0','32543','galdarah SAY_DEATH'),

('-1604029','%s transforms into a Mammoth!','14724','2','0','0','30388','moorabi EMOTE_TRANSFORMED'),
('-1604030','$N is impaled!','0','3','0','0','30718','EMOTE_IMPALED');

-- -1 608 000 VIOLET HOLD
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1608000','It seems my freedom must be bought... with blood.','14499','1','0','0','31805','xevozz SAY_AGGRO'),
('-1608001','Intriguing... a high quantity of arcane energy is near. Time for some prospecting...','14500','1','0','0','29981','xevozz SAY_SUMMON_1'),

('-1608002','Adventurers, the door is beinning to weaken!','0','1','0','0','32557','sinclair SAY_SEAL_75'),
('-1608003','Only half of the door seal''s strength remains! You must fight on!','0','1','0','0','32558','sinclair SAY_SEAL_50'),
('-1608004','The door seal is about to collapse! All is lost if the Blue Dragonflight breaks through the door!','0','1','0','0','32559','sinclair SAY_SEAL_5'),

('-1608005','A Portal Guardian defends the new portal!','0','3','0','0','32995','EMOTE_GUARDIAN_PORTAL'),
('-1608006','An elite Blue Dragonflight squad appears from the portal!','0','3','0','0','32994','EMOTE_DRAGONFLIGHT_PORTAL'),
('-1608007','A Guardian Keeper emerges from the portal!','0','3','0','0','0','EMOTE_KEEPER_PORTAL'),

('-1608008','Free to--mm--fly now. Ra-aak... Not find us--ekh-ekh! Escape!','14218','1','0','0','31328','erekem SAY_RELEASE_EREKEM'),
('-1608009','I... am fury... unrestrained!','14229','1','0','0','31330','ichoron SAY_RELEASE_ICHORON'),
('-1608010','Back in business! Now to execute an exit strategy.','14498','1','0','396','31337','xevozz SAY_RELEASE_XEVOZZ'),
('-1608011','I am... renewed.','13995','1','0','0','31339','zuramat SAY_RELEASE_ZURAMAT'),

('-1608012','Nothing personal...','14504','1','0','0','31808','xevozz SAY_SLAY_1'),
('-1608013','Business concluded.','14505','1','0','0','31809','xevozz SAY_SLAY_2'),
('-1608014','Profit!','14506','1','0','0','31810','xevozz SAY_SLAY_3'),
('-1608015','The air teems with latent energy... quite the harvest!','14501','1','0','0','29931','xevozz SAY_SUMMON_2'),
('-1608016','Plentiful, exploitable resources... primed for acquisition!','14502','1','0','0','31806','xevozz SAY_SUMMON_3'),
('-1608017','This... is an unrecoverable... loss.','14507','1','0','0','31811','xevozz SAY_DEATH'),

('-1608018','%s begins to summon!','0','3','0','0','30087','ethereal sphere EMOTE_SUMMON'),

('-1608019','Stand aside, mortals!','14230','1','0','0','32056','ichoron SAY_AGGRO'),
('-1608020','I will not be contained! Ngyah!!','14233','1','0','0','32053','ichoron SAY_SHATTERING'),
('-1608021','Water can hold any form, take any shape... overcome any obstacle.','14232','1','0','0','32055','ichoron SAY_SHIELD'),
('-1608022','I am a force of nature!','14234','1','0','0','32050','ichoron SAY_SLAY_1'),
('-1608023','I shall pass!','14235','1','0','0','32051','ichoron SAY_SLAY_2'),
('-1608024','You can not stop the tide!','14236','1','0','0','32052','ichoron SAY_SLAY_3'),
('-1608025','I shall consume,decimate, devastate,and destroy! Yield now to the wrath of the pounding sea!','14231','1','0','0','32054','ichoron SAY_ENRAGE'),
('-1608026','I... recede.','14237','1','0','0','32049','ichoron SAY_DEATH'),

('-1608027','REUSE ME','0','0','0','0','0','REUSE ME'),

('-1608028','%s''s Protective Bubble shatters!','0','3','0','0','30086','ichoron EMOTE_BUBBLE');

-- -1 609 000 EBON HOLD (DK START)
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1609000','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1609001','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1609002','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1609003','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1609004','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1609005','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1609006','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1609007','REUSE ME','0','0','0','0','0','REUSE ME'),

('-1609008','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1609009','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1609010','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1609011','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1609012','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1609013','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1609014','REUSE ME','0','0','0','0','0','REUSE ME'),

('-1609016','No potions!','0','0','0','0','29265','dk_initiate SAY_DUEL_A'),
('-1609017','Remember this day, $n, for it is the day that you will be thoroughly owned.','0','0','0','0','29270','dk_initiate SAY_DUEL_B'),
('-1609018','I''m going to tear your heart out, cupcake!','0','0','0','0','29263','dk_initiate SAY_DUEL_C'),
('-1609019','Don''t make me laugh.','0','0','0','0','29263','dk_initiate SAY_DUEL_D'),
('-1609020','Here come the tears...','0','0','0','0','29264','dk_initiate SAY_DUEL_E'),
('-1609021','You have challenged death itself!','0','0','0','0','29262','dk_initiate SAY_DUEL_F'),
('-1609022','The Lich King will see his true champion on this day!','0','0','0','0','29268','dk_initiate SAY_DUEL_G'),
('-1609023','You''re going down!','0','0','0','0','29266','dk_initiate SAY_DUEL_H'),
('-1609024','You don''t stand a chance, $n','0','0','0','0','29267','dk_initiate SAY_DUEL_I'),

('-1609025','Come to finish the job, have you?','0','0','0','6','29286','special_surprise SAY_EXEC_START_1'),
('-1609026','Come to finish the job, have ye?','0','0','0','6','29335','special_surprise SAY_EXEC_START_2'),
('-1609027','Come ta finish da job, mon?','0','0','0','6','29403','special_surprise SAY_EXEC_START_3'),

('-1609028','You''ll look me in the eyes when...','0','0','0','25','29287','special_surprise SAY_EXEC_PROG_1'),
('-1609029','Well this son o'' Ironforge would like...','0','0','0','25','29336','special_surprise SAY_EXEC_PROG_2'),
('-1609030','Ironic, isn''t it? To be killed...','0','0','0','25','29394','special_surprise SAY_EXEC_PROG_3'),
('-1609031','If you''d allow me just one...','0','0','0','25','29348','special_surprise SAY_EXEC_PROG_4'),
('-1609032','I''d like to stand for...','0','0','0','25','29311','special_surprise SAY_EXEC_PROG_5'),
('-1609033','I want to die like an orc...','0','0','0','25','29384','special_surprise SAY_EXEC_PROG_6'),
('-1609034','Dis troll gonna stand for da...','0','0','0','25','29404','special_surprise SAY_EXEC_PROG_7'),

('-1609035','$N?','0','0','0','6','29288','special_surprise SAY_EXEC_NAME_1'),
('-1609036','$N? Mon?','0','0','0','6','29405','special_surprise SAY_EXEC_NAME_2'),

('-1609037','$N, I''d recognize that face anywhere... What... What have they done to you, $N?','0','0','0','6','29313','special_surprise SAY_EXEC_RECOG_1'),
('-1609038','$N, I''d recognize those face tentacles anywhere... What... What have they done to you, $N?','0','0','0','6','29360','special_surprise SAY_EXEC_RECOG_2'),
('-1609039','$N, I''d recognize that face anywhere... What... What have they done to ye, $Glad:lass;?','0','0','0','6','29338','special_surprise SAY_EXEC_RECOG_3'),
('-1609040','$N, I''d recognize that decay anywhere... What... What have they done to you, $N?','0','0','0','6','29396','special_surprise SAY_EXEC_RECOG_4'),
('-1609041','$N, I''d recognize those horns anywhere... What have they done to you, $N?','0','0','0','6','29289','special_surprise SAY_EXEC_RECOG_5'),
('-1609042','$N, I''d recognize dem tusks anywhere... What... What have dey done ta you, mon?','0','0','0','6','29406','special_surprise SAY_EXEC_RECOG_6'),

('-1609043','You don''t remember me, do you? Blasted Scourge... They''ve tried to drain you of everything that made you a righteous force of reckoning. Every last ounce of good... Everything that made you a draenei!','0','0','0','6','29361','special_surprise SAY_EXEC_NOREM_1'),
('-1609044','Ye don''t remember me, do ye? Blasted Scourge... They''ve tried to drain ye o'' everything that made ye a righteous force o'' reckoning. Every last ounce o'' good... Everything that made you a $Gson:daughter; of Ironforge!','0','0','0','6','29339','special_surprise SAY_EXEC_NOREM_2'),
('-1609045','You don''t remember me, do you? We were humans once - long, long ago - until Lordaeron fell to the Scourge. Your transformation to a Scourge zombie came shortly after my own. Not long after that, our minds were freed by the Dark Lady.','0','0','0','6','29397','special_surprise SAY_EXEC_NOREM_3'),
('-1609046','You don''t remember me, do you? Blasted Scourge... They''ve tried to drain you of everything that made you a pint-sized force of reckoning. Every last ounce of good... Everything that made you a gnome!','0','0','0','6','29351','special_surprise SAY_EXEC_NOREM_4'),
('-1609047','You don''t remember me, do you? Blasted Scourge...They''ve tried to drain of everything that made you a righteous force of reckoning. Every last ounce of good...Everything that made you a human!','0','0','0','6','29314','special_surprise SAY_EXEC_NOREM_5'),
('-1609048','You don''t remember me? When you were a child your mother would leave you in my care while she served at the Temple of the Moon. I held you in my arms and fed you with honey and sheep''s milk to calm you until she would return. You were my little angel. Blasted Scourge... What have they done to you, $N?','0','0','0','6','29326','special_surprise SAY_EXEC_NOREM_6'),
('-1609049','You don''t recognize me, do you? Blasted Scourge... They''ve tried to drain you of everything that made you a righteous force of reckoning. Every last ounce of good... Everything that made you an orc!','0','0','0','6','29387','special_surprise SAY_EXEC_NOREM_7'),
('-1609050','You don''t remember me, do you? Blasted Scourge... They''ve tried to drain you of everything that made you a righteous force of reckoning. Every last ounce of good... Everything that made you a tauren!','0','0','0','6','29290','special_surprise SAY_EXEC_NOREM_8'),
('-1609051','You don''t remember me, mon? Damn da Scourge! Dey gone ta drain you of everytin dat made ya a mojo masta. Every last ounce of good... Everytin'' dat made ya a troll hero, mon!','0','0','0','6','29407','special_surprise SAY_EXEC_NOREM_9'),

('-1609015','You don''t remember me, do you? Blasted Scourge... They''ve tried to drain you of everything that made you a righteous force of reckoning. Every last ounce of good... Everything that made you a blood elf!','0','0','0','6','29373','special_surprise SAY_EXEC_NOREM_10'),

('-1609052','A pact was made, $Gbrother:sister;! We vowed vengeance against the Lich King! For what he had done to us! We battled the Scourge as Forsaken, pushing them back into the plaguelands and freeing Tirisfal! You and I were champions of the Forsaken!','0','0','0','1','29398','special_surprise SAY_EXEC_THINK_1'),
('-1609053','You must remember the splendor of life, $Gbrother:sister;. You were a champion of the Kaldorei once! This isn''t you!','0','0','0','1','29328','special_surprise SAY_EXEC_THINK_2'),
('-1609054','Think, $N. Think back. Try and remember the majestic halls of Silvermoon City, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the sin''dorei once! This isn''t you.','0','0','0','1','29374','special_surprise SAY_EXEC_THINK_3'),
('-1609055','Think, $N. Think back. Try and remember the proud mountains of Argus, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the draenei once! This isn''t you.','0','0','0','1','29362','special_surprise SAY_EXEC_THINK_4'),
('-1609056','Think, $N. Think back. Try and remember the snow capped mountains o'' Dun Morogh! Ye were born there, $Glad:lass;. Remember the splendor o'' life, $N! Ye were a champion o'' the dwarves once! This isn''t ye!','0','0','0','1','29340','special_surprise SAY_EXEC_THINK_5'),
('-1609057','Think, $N. Think back. Try and remember Gnomeregan before those damned troggs! Remember the feel of an [arclight spanner] $Gbrother:sister;. You were a champion of gnome-kind once! This isn''t you.','0','0','0','1','29352','special_surprise SAY_EXEC_THINK_6'),
('-1609058','Think, $N. Think back. Try and remember the hills and valleys of Elwynn, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the Alliance once! This isn''t you.','0','0','0','1','29315','special_surprise SAY_EXEC_THINK_7'),
('-1609059','Think, $N. Think back. Try and remember Durotar, $Gbrother:sister;! Remember the sacrifices our heroes made so that we could be free of the blood curse. Harken back to the Valley of Trials, where we were reborn into a world without demonic influence. We found the splendor of life, $N. Together! This isn''t you. You were a champion of the Horde once!','0','0','0','1','29388','special_surprise SAY_EXEC_THINK_8'),
('-1609060','Think, $N. Think back. Try and remember the rolling plains of Mulgore, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the tauren once! This isn''t you.','0','0','0','1','29294','special_surprise SAY_EXEC_THINK_9'),
('-1609061','TINK $N. Tink back, mon! We be Darkspear, mon! Bruddas and sistas! Remember when we fought the Zalazane and done took he head and freed da Echo Isles? MON! TINK! You was a champion of da Darkspear trolls!','0','0','0','1','29408','special_surprise SAY_EXEC_THINK_10'),

('-1609062','Listen to me, $N. You must fight against the Lich King''s control. He is a monster that wants to see this world - our world - in ruin. Don''t let him use you to accomplish his goals. You were once a hero and you can be again. Fight, damn you! Fight his control!','0','0','0','1','29295','special_surprise SAY_EXEC_LISTEN_1'),
('-1609063','Listen to me, $N Ye must fight against the Lich King''s control. He''s a monster that wants to see this world - our world - in ruin. Don''t let him use ye to accomplish his goals. Ye were once a hero and ye can be again. Fight, damn ye! Fight his control!','0','0','0','1','29341','special_surprise SAY_EXEC_LISTEN_2'),
('-1609064','Listen to me, $N. You must fight against the Lich King''s control. He is a monster that wants to see this world - our world - in ruin. Don''t let him use you to accomplish his goals AGAIN. You were once a hero and you can be again. Fight, damn you! Fight his control!','0','0','0','1','29399','special_surprise SAY_EXEC_LISTEN_3'),
('-1609065','Listen ta me, $Gbrudda:sista;. You must fight against da Lich King''s control. He be a monstar dat want ta see dis world - our world - be ruined. Don''t let he use you ta accomplish he goals. You be a hero once and you be a hero again! Fight it, mon! Fight he control!','0','0','0','1','29409','special_surprise SAY_EXEC_LISTEN_4'),

('-1609066','What''s going on in there? What''s taking so long, $N?','0','1','0','0','29309','special_surprise SAY_PLAGUEFIST'),

('-1609067','There... There''s no more time for me. I''m done for. Finish me off, $N. Do it or they''ll kill us both. $N... Remember Silvermoon. This world is worth saving!','0','0','0','18','29376','special_surprise SAY_EXEC_TIME_1'),
('-1609068','There... There''s no more time for me. I''m done for. Finish me off, $N. Do it or they''ll kill us both. $N... Remember Argus. Don''t let that happen to this world.','0','0','0','18','29364','special_surprise SAY_EXEC_TIME_2'),
('-1609069','There... There''s no more time for me. I''m done for. Finish me off, $N. Do it or they''ll kill us both $N... For KHAAAAAAAAZZZ MODAAAAAANNNNNN!!!','0','0','0','18','29342','special_surprise SAY_EXEC_TIME_3'),
('-1609070','There... There''s no more time for me. I''m done for. Finish me off, $N. Do it or they''ll kill us both. $N... Remember Tirisfal! This world is worth saving!','0','0','0','18','29400','special_surprise SAY_EXEC_TIME_4'),
('-1609071','There... There''s no more time for me. I''m done for. Finish me off, $N. Do it or they''ll kill us both. $N... Remember Gnomeregan! This world is worth saving.','0','0','0','18','29354','special_surprise SAY_EXEC_TIME_5'),
('-1609072','There... There''s no more time for me. I''m done for. FInish me off, $N. Do it or they''ll kill us both. $N...Remember Elwynn. This world is worth saving.','0','0','0','18','29317','special_surprise SAY_EXEC_TIME_6'),
('-1609073','There... There''s no more time for me. I''m done for. Finish me off, $N. Do it or they''ll kill us both. $N... Remember Teldrassil, our beloved home. This world is worth saving.','0','0','0','18','29330','special_surprise SAY_EXEC_TIME_7'),
('-1609074','There... There''s no more time for me. I''m done for. Finish me off, $N. Do it or they''ll kill us both. $N... For the Horde! This world is worth saving.','0','0','0','18','29390','special_surprise SAY_EXEC_TIME_8'),
('-1609075','There... There''s no more time for me. I''m done for. Finish me off, $N. Do it or they''ll kill us both. $N... Remember Mulgore. This world is worth saving.','0','0','0','18','29301','special_surprise SAY_EXEC_TIME_9'),
('-1609076','Der... Der''s no more time for me. I be done for. Finish me off $N. Do it or they''ll kill us both. $N... Remember Sen''jin Village, mon! Dis world be worth saving!','0','0','0','18','29410','special_surprise SAY_EXEC_TIME_10'),

('-1609077','Do it, $N! Put me out of my misery!','0','0','0','1','29302','special_surprise SAY_EXEC_WAITING'),
('-1609078','%s dies from his wounds.','0','2','0','0','29303','special_surprise EMOTE_DIES'),

('-1609079','Hrm, what a strange tree. I must investigate.','0','0','0','1','29413','scarlet courier SAY_TREE_1'),
('-1609080','What''s this!? This isn''t a tree at all! Guards! Guards!','0','0','0','5','29415','scarlet courier SAY_TREE_2'),

('-1609081','%s throws rotten apple on $N.','0','2','0','0','31671','city guard EMOTE_APPLE'),
('-1609082','%s throws rotten banana on $N.','0','2','0','0','31672','city guard EMOTE_BANANA'),
('-1609083','%s spits on $N.','0','2','0','0','31673','city guard EMOTE_SPIT'),
('-1609084','Monster!','0','0','0','14','31662','city guard SAY_RANDOM_1'),
('-1609085','Murderer!','0','0','0','14','31663','city guard SAY_RANDOM_2'),
('-1609086','GET A ROPE!','0','0','0','25','10952','city guard SAY_RANDOM_3'),
('-1609087','How dare you set foot in our city!','0','0','0','25','31668','city guard SAY_RANDOM_4'),
('-1609088','You disgust me.','0','0','0','14','31664','city guard SAY_RANDOM_5'),

('-1609089','The Eye of Acherus launches towards its destination','0','3','0','0','28507','eye of acherus EMOTE_DESTIANTION'),
('-1609090','The Eye of Acherus is in your control','0','3','0','0','28465','eye of acherus EMOTE_CONTROL'),

('-1609091','Mommy?','0','0','0','434','28959','scarlet ghoul SAY_GHUL_SPAWN_1'),
('-1609092','GIVE ME BRAINS!','0','0','0','434','28957','scarlet ghoul SAY_GHUL_SPAWN_2'),
('-1609093','Must feed...','0','0','0','434','28956','scarlet ghoul SAY_GHUL_SPAWN_3'),
('-1609094','So hungry...','0','0','0','434','28955','scarlet ghoul SAY_GHUL_SPAWN_4'),
('-1609095','$gPoppy:Mama;!','0','0','0','434','28960','scarlet ghoul SAY_GHUL_SPAWN_5'),
('-1609096','It puts the ghoul in the pit or else it gets the lash!','0','0','0','25','28939','gothik the harvester SAY_GOTHIK_THROW_IN_PIT_1'),
('-1609097','Get in that pit before I turn you into a pile of ash and feces!','0','0','0','25','28941','gothik the harvester SAY_GOTHIK_THROW_IN_PIT_2'),

('-1609098','REUSE ME','0','0','0','0','0','REUSE ME'),

('-1609201','Soldiers of the Scourge, stand ready! Prepare to unleash your fury upon the Argent Dawn!','14677','1','0','22','29694','Highlord Darion Mograine'),
('-1609202','The sky weeps at the devastation of these lands! Soon, Azeroth''s futile tears will rain down upon us!','14678','1','0','22','29696','Highlord Darion Mograine'),
('-1609203','Death knights of Acherus, the death march begins!','14681','1','0','0','29697','Highlord Darion Mograine'),
('-1609204','Soldiers of the Scourge, death knights of Acherus, minions of the darkness: hear the call of the Highlord!','14679','1','0','22','29699','Highlord Darion Mograine'),
('-1609205','RISE!','14680','1','0','15','24929','Highlord Darion Mograine'),
('-1609206','The skies turn red with the blood of the fallen! The Lich King watches over us, minions! Leave only ashes and misery in your destructive wake!','14682','1','0','25','29722','Highlord Darion Mograine'),
('-1609207','Scourge armies approach!','0','1','0','0','29724','Korfax, Champion of the Light'),
('-1609208','Stand fast, brothers and sisters! The Light will prevail!','14487','1','0','0','29723','Lord Maxwell Tyrosus'),
('-1609209','Kneel before the Highlord!','14683','0','0','0','29727','Highlord Darion Mograine'),
('-1609210','You stand no chance!','14684','0','0','0','29728','Highlord Darion Mograine'),
('-1609211','The Scourge will destroy this place!','14685','0','0','0','29729','Highlord Darion Mograine'),
('-1609212','Your life is forfeit.','14686','0','0','0','46677','Highlord Darion Mograine'),
('-1609213','Life is meaningless without suffering.','14687','0','0','0','29731','Highlord Darion Mograine'),
('-1609214','How much longer will your forces hold out?','14688','0','0','0','29732','Highlord Darion Mograine'),
('-1609215','The Argent Dawn is finished!','14689','0','0','0','29733','Highlord Darion Mograine'),
('-1609216','Spare no one!','14690','0','0','0','29734','Highlord Darion Mograine'),
('-1609217','What is this?! My... I cannot strike...','14691','0','0','0','29735','Highlord Darion Mograine'),
('-1609218','Obey me, blade!','14692','1','0','0','29736','Highlord Darion Mograine'),
('-1609219','You will do as I command! I am in control here!','14693','0','0','0','29737','Highlord Darion Mograine'),
('-1609220','I can not... the blade fights me.','14694','0','0','0','29738','Highlord Darion Mograine'),
('-1609221','What is happening to me?','14695','0','0','0','15703','Highlord Darion Mograine'),
('-1609222','Power...wanes...','14696','0','0','0','29740','Highlord Darion Mograine'),
('-1609223','Ashbringer defies me...','14697','0','0','0','29741','Highlord Darion Mograine'),
('-1609224','Minions, come to my aid!','14698','0','0','0','29742','Highlord Darion Mograine'),
('-1609225','You cannot win, Darion!','14584','1','0','0','29743','Highlord Tirion Fordring'),
('-1609226','Bring them before the chapel!','14585','1','0','0','29756','Highlord Tirion Fordring'),
('-1609227','Stand down, death knights. We have lost... The Light... This place... No hope...','14699','0','0','68','29760','Highlord Darion Mograine'),
('-1609228','Have you learned nothing, boy? You have become all that your father fought against! Like that coward, Arthas, you allowed yourself to be consumed by the darkness...the hate... Feeding upon the misery of those you tortured and killed!','14586','0','0','1','29744','Highlord Tirion Fordring'),
('-1609229','Your master knows what lies beneath the chapel. It is why he dares not show his face! He''s sent you and your death knights to meet their doom, Darion.','14587','0','0','25','29745','Highlord Tirion Fordring'),
('-1609230','What you are feeling right now is the anguish of a thousand lost souls! Souls that you and your master brought here! The Light will tear you apart, Darion!','14588','0','0','1','29746','Highlord Tirion Fordring'),
('-1609231','Save your breath, old man. It might be the last you ever draw.','14700','0','0','25','29761','Highlord Darion Mograine'),
('-1609232','My son! My dear, beautiful boy!','14493','0','0','0','29762','Highlord Alexandros Mograine'),
('-1609233','Father!','14701','0','0','5','29767','Highlord Darion Mograine'),
('-1609234','Argh...what...is...','14702','0','0','68','29768','Highlord Darion Mograine'),
('-1609235','Father, you have returned!','14703','0','0','0','29769','Darion Mograine'),
('-1609236','You have been gone a long time, father. I thought...','14704','0','0','0','29770','Darion Mograine'),
('-1609237','Nothing could have kept me away from here, Darion. Not from my home and family.','14494','0','0','1','29773','Highlord Alexandros Mograine'),
('-1609238','Father, I wish to join you in the war against the undead. I want to fight! I can sit idle no longer!','14705','0','0','6','29774','Darion Mograine'),
('-1609239','Darion Mograine, you are barely of age to hold a sword, let alone battle the undead hordes of Lordaeron! I couldn''t bear losing you. Even the thought...','14495','0','0','1','29775','Highlord Alexandros Mograine'),
('-1609240','If I die, father, I would rather it be on my feet, standing in defiance against the undead legions! If I die, father, I die with you!','14706','0','0','6','29776','Darion Mograine'),
('-1609241','My son, there will come a day when you will command the Ashbringer and, with it, mete justice across this land. I have no doubt that when that day finally comes, you will bring pride to our people and that Lordaeron will be a better place because of you. But, my son, that day is not today.','14496','0','0','1','29777','Highlord Alexandros Mograine'),
('-1609242','Do not forget...','14497','0','0','6','29778','Highlord Alexandros Mograine'),
('-1609243','Touching...','14803','1','0','0','29779','The Lich King'),
('-1609244','You have''ve betrayed me! You betrayed us all you monster! Face the might of Mograine!','14707','1','0','397','29781','Highlord Darion Mograine'),
('-1609245','He''s mine now...','14805','0','0','0','29780','The Lich King'),
('-1609246','Pathetic...','14804','0','0','0','13132','The Lich King'),
('-1609247','You''re a damned monster, Arthas!','14589','0','0','25','29783','Highlord Tirion Fordring'),
('-1609248','You were right, Fordring. I did send them in to die. Their lives are meaningless, but yours...','14806','0','0','1','29784','The Lich King'),
('-1609249','How simple it was to draw the great Tirion Fordring out of hiding. You''ve left yourself exposed, paladin. Nothing will save you...','14807','0','0','1','29785','The Lich King'),
('-1609250','ATTACK!!!','14488','1','0','0','61812','Lord Maxwell Tyrosus'),
('-1609251','APOCALYPSE!','14808','1','0','0','28736','The Lich King'),
('-1609252','That day is not today...','14708','0','0','0','29824','Highlord Darion Mograine'),
('-1609253','Tirion!','14709','1','0','0','29801','Highlord Darion Mograine'),
('-1609254','ARTHAS!!!!','14591','1','0','15','29803','Highlord Tirion Fordring'),
('-1609255','What is this?','14809','1','0','0','29804','The Lich King'),
('-1609256','Your end.','14592','1','0','0','29861','Highlord Tirion Fordring'),
('-1609257','Impossible...','14810','1','0','0','29825','The Lich King'),
('-1609258','This... isn''t... over...','14811','1','0','397','0','The Lich King'),
('-1609259','When next we meet it won''t be on holy ground, paladin.','14812','1','0','1','29827','The Lich King'),
('-1609260','Rise, Darion, and listen...','14593','0','0','0','29835','Highlord Tirion Fordring'),
('-1609261','We have all been witness to a terrible tragedy. The blood of good men has been shed upon this soil! Honorable knights, slain defending their lives - our lives!','14594','0','0','0','29843','Highlord Tirion Fordring'),
('-1609262','And while such things can never be forgotten, we must remain vigilant in our cause!','14595','0','0','0','29845','Highlord Tirion Fordring'),
('-1609263','The Lich King must answer for what he has done and must not be allowed to cause further destruction to our world.','14596','0','0','0','29846','Highlord Tirion Fordring'),
('-1609264','I make a promise to you now, brothers and sisters: The Lich King will be defeated! On this day, I call for a union.','14597','0','0','0','29847','Highlord Tirion Fordring'),
('-1609265','The Argent Dawn and the Order of the Silver Hand will come together as one! We will succeed where so many before us have failed!','14598','0','0','0','29848','Highlord Tirion Fordring'),
('-1609266','We will take the fight to Arthas and tear down the walls of Icecrown!','14599','0','0','15','29849','Highlord Tirion Fordring'),
('-1609267','The Argent Crusade comes for you, Arthas!','14600','1','0','15','29850','Highlord Tirion Fordring'),
('-1609268','So too do the Knights of the Ebon Blade... While our kind has no place in your world, we will fight to bring an end to the Lich King. This I vow!','14710','0','0','1','29851','Highlord Darion Mograine'),
('-1609269','Thousands of Scourge rise up at the Highlord''s command.','0','3','0','0','0',''),
('-1609270','The army marches towards Light''s Hope Chapel.','0','3','0','0','0',''),
('-1609271','After over a hundred Defenders of the Light fall, Highlord Tirion Fordring arrives.','0','3','0','0','0',''),
('-1609272','%s flee','0','2','0','0','29758','Orbaz'),
('-1609273','%s kneels in defeat before Tirion Fordring.','0','3','0','0','29759','Highlord Darion Mograine'),
('-1609274','%s arrives.','0','2','0','0','1415','Highlord Alexandros Mograine'),
('-1609275','%s becomes a shade of his past, and walks up to his father.','0','2','0','0','0','Highlord Darion Mograine'),
('-1609276','%s hugs his father.','0','2','0','0','29771','Darion Mograine'),
('-1609277','%s disappears, and the Lich King appears.','0','2','0','0','0','Alexandros'),
('-1609278','%s becomes himself again...and is now angry.','0','2','0','0','0','Highlord Darion Mograine'),
('-1609279','%s casts a spell on Tirion.','0','2','0','0','0','The Lich King'),
('-1609280','%s gasps for air.','0','2','0','0','29786','Highlord Tirion Fordring'),
('-1609281','%s casts a powerful spell, killing the Defenders and knocking back the others.','0','2','0','0','0','The Lich King'),
('-1609282','%s throws the Corrupted Ashbringer to Tirion, who catches it. Tirion becomes awash with Light, and the Ashbringer is cleansed.','0','2','0','0','0','Highlord Darion Mograine'),
('-1609283','%s collapses.','0','2','0','0','17163','Highlord Darion Mograine'),
('-1609284','%s charges towards the Lich King, Ashbringer in hand and strikes the Lich King.','0','2','0','0','0','Highlord Tirion Fordring'),
('-1609285','%s disappears. Tirion walks over to where Darion lay','0','2','0','0','0','The Lich King'),
('-1609286','Light washes over the chapel -- the Light of Dawn is uncovered.','0','2','0','0','0',''),

('-1609287','Looks like we''re going to have ourselves an execution.','0','0','0','25','31666','city guard SAY_RANDOM_6'),
('-1609288','Traitorous dog.','0','0','0','14','31681','city guard SAY_RANDOM_7'),
('-1609289','My family was wiped out by the Scourge! MONSTER!','0','0','0','25','31667','city guard SAY_RANDOM_8');

-- -1 615 000 OBSIDIAN SANCTUM
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1615000','I fear nothing! Least of all you!','14111','1','0','0','31985','shadron SAY_SHADRON_AGGRO'),
('-1615001','You are insignificant!','14112','1','0','0','31986','shadron SAY_SHADRON_SLAY_1'),
('-1615002','Such mediocre resistance!','14113','1','0','0','31987','shadron SAY_SHADRON_SLAY_2'),
('-1615003','We...are superior! How could this...be...','14118','1','0','0','31984','shadron SAY_SHADRON_DEATH'),
('-1615004','You are easily bested! ','14114','1','0','0','31988','shadron SAY_SHADRON_BREATH'),
('-1615005','I will take pity on you Sartharion, just this once.','14117','1','0','0','31991','shadron SAY_SHADRON_RESPOND'),
('-1615006','Father tought me well!','14115','1','0','0','0','shadron SAY_SHADRON_SPECIAL_1'),
('-1615007','On your knees!','14116','1','0','0','21749','shadron SAY_SHADRON_SPECIAL_2'),
('-1615008','A Shadron Disciple appears in the Twilight!','0','3','0','0','32958','shadron WHISPER_SHADRON_DICIPLE'),

('-1615009','You have no place here. Your place is among the departed.','14122','1','0','0','31993','tenebron SAY_TENEBRON_AGGRO'),
('-1615010','No contest.','14123','1','0','0','31994','tenebron SAY_TENEBRON_SLAY_1'),
('-1615011','Typical... Just as I was having fun.','14124','1','0','0','31995','tenebron SAY_TENEBRON_SLAY_2'),
('-1615012','I should not... have held back...','14129','1','0','0','32000','tenebron SAY_TENEBRON_DEATH'),
('-1615013','To darkness I condemn you...','14125','1','0','0','31996','tenebron SAY_TENEBRON_BREATH'),
('-1615014','It is amusing to watch you struggle. Very well, witness how it is done.','14128','1','0','0','31999','tenebron SAY_TENEBRON_RESPOND'),
('-1615015','Arrogant little creatures! To challenge powers you do not yet understand...','14126','1','0','0','31997','tenebron SAY_TENEBRON_SPECIAL_1'),
('-1615016','I am no mere dragon! You will find I am much, much, more...','14127','1','0','0','31998','tenebron SAY_TENEBRON_SPECIAL_2'),
('-1615017','%s begins to hatch eggs in the twilight!','0','3','0','0','32959','tenebron WHISPER_HATCH_EGGS'),

('-1615018','It is my charge to watch over these eggs. I will see you burn before any harm comes to them!','14093','1','0','0','31400','sartharion SAY_SARTHARION_AGGRO'),
('-1615019','This pathetic siege ends NOW!','14103','1','0','0','31368','sartharion SAY_SARTHARION_BERSERK'),
('-1615020','Burn, you miserable wretches!','14098','1','0','0','31907','sartharion SAY_SARTHARION_BREATH'),
('-1615021','Shadron! Come to me, all is at risk!','14105','1','0','0','31318','sartharion SARTHARION_CALL_SHADRON'),
('-1615022','Tenebron! The eggs are yours to protect as well!','14106','1','0','0','31319','sartharion SAY_SARTHARION_CALL_TENEBRON'),
('-1615023','Vesperon! The clutch is in danger! Assist me!','14104','1','0','0','31320','sartharion SAY_SARTHARION_CALL_VESPERON'),
('-1615024','Such is the price... of failure...','14107','1','0','0','31341','sartharion SAY_SARTHARION_DEATH'),
('-1615025','Such flammable little insects....','14099','1','0','0','31363','sartharion SAY_SARTHARION_SPECIAL_1'),
('-1615026','Your charred bones will litter the floor!','14100','1','0','0','31904','sartharion SAY_SARTHARION_SPECIAL_2'),
('-1615027','How much heat can you take?','14101','1','0','0','31905','sartharion SAY_SARTHARION_SPECIAL_3'),
('-1615028','All will be reduced to ash!','14102','1','0','0','31906','sartharion SAY_SARTHARION_SPECIAL_4'),
('-1615029','You will make a fine meal for the hatchlings.','14094','1','0','0','31908','sartharion SAY_SARTHARION_SLAY_1'),
('-1615030','You are the grave disadvantage.','14096','1','0','0','31909','sartharion SAY_SARTHARION_SLAY_2'),
('-1615031','This is why we call you lesser beeings.','14097','1','0','0','31910','sartharion SAY_SARTHARION_SLAY_3'),
('-1615032','The lava surrounding %s churns!','0','3','0','0','31362','sartharion WHISPER_LAVA_CHURN'),

('-1615033','You pose no threat, lesser beings...give me your worst!','14133','1','0','0','32002','vesperon SAY_VESPERON_AGGRO'),
('-1615034','The least you could do is put up a fight...','14134','1','0','0','32003','vesperon SAY_VESPERON_SLAY_1'),
('-1615035','Was that the best you can do?','14135','1','0','0','32004','vesperon SAY_VESPERON_SLAY_2'),
('-1615036','I still have some...fight..in...me...','14140','1','0','0','32009','vesperon SAY_VESPERON_DEATH'),
('-1615037','I will pick my teeth with your bones!','14136','1','0','0','32005','vesperon SAY_VESPERON_BREATH'),
('-1615038','Father was right about you, Sartharion...You are a weakling!','14139','1','0','0','32008','vesperon SAY_VESPERON_RESPOND'),
('-1615039','Aren''t you tricky...I have a few tricks of my own...','14137','1','0','0','32006','vesperon SAY_VESPERON_SPECIAL_1'),
('-1615040','Unlike, I have many talents.','14138','1','0','0','32007','vesperon SAY_VESPERON_SPECIAL_2'),
('-1615041','A Vesperon Disciple appears in the Twilight!','0','3','0','0','32960','shadron WHISPER_VESPERON_DICIPLE'),

('-1615042','%s begins to open a Twilight Portal!','0','3','0','0','32554','sartharion drake WHISPER_OPEN_PORTAL');

-- -1 616 000 EYE OF ETERNITY
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1616000','Lesser beings, intruding here! A shame that your excess courage does not compensate for your stupidity!','14512','1','0','0','31759','malygos SAY_INTRO_1'),
('-1616001','None but the blue dragonflight are welcome here. Perhaps this is the work of Alexstrasza? Well, then, she has sent you to your deaths!','14513','1','0','0','31760','malygos SAY_INTRO_2'),
('-1616002','What could you hope to accomplish? To storm brazenly into my domain... to employ magic... against ME?','14514','1','0','0','31761','malygos SAY_INTRO_3'),
('-1616003','I am without limits here. The rules of your cherished reality do not apply. In this realm, I am in control!','14515','1','0','0','31762','malygos SAY_INTRO_4'),
('-1616004','I give you one chance. Pledge fealty to me, and perhaps I will not slaughter you for your insolence.','14516','1','0','0','31763','malygos SAY_INTRO_5'),
('-1616005','My patience has reached its limit. I will be rid of you!','14517','1','0','0','31764','malygos SAY_AGGRO'),
('-1616006','Watch helplessly as your hopes are swept away!','14525','1','0','0','31774','malygos SAY_VORTEX'),
('-1616007','I AM UNSTOPPABLE!','14533','1','0','0','31783','malygos SAY_SPARK_BUFF'),
('-1616008','Your stupidity has finally caught up to you!','14519','1','0','0','31767','malygos SAY_SLAY_1_A'),
('-1616009','More artifacts to confiscate...','14520','1','0','0','31768','malygos SAY_SLAY_1_B'),
('-1616010','<Laughs> How very... naive...','14521','1','0','0','31769','malygos SAY_SLAY_1_C'),
('-1616011','I had hoped to end your lives quickly, but you have proven more... resilient than I anticipated. Nonetheless, your efforts are in vain. It is you reckless, careless mortals who are to blame for this war. I do what I must, and if it means your extinction.... then SO BE IT!!','14522','1','0','0','31772','malygos SAY_END_PHASE_1'),
('-1616012','Few have experienced the pain I will now inflict upon you!','14523','1','0','0','31765','malygos SAY_START_PHASE_2'),
('-1616013','You will not succeed while I draw breath!','14518','1','0','0','31781','malygos SAY_DEEP_BREATH'),
('-1616014','I will teach you IGNORANT children just how little you know of magic...','14524','1','0','0','31773','malygos SAY_SHELL'),
('-1616015','Your energy will be put to good use!','14526','1','0','0','31775','malygos SAY_SLAY_2_A'),
('-1616016','I am the spell-weaver! My power is infinite!','14527','1','0','0','31776','malygos SAY_SLAY_2_B'),
('-1616017','Your spirit will linger here forever!','14528','1','0','0','31777','malygos SAY_SLAY_2_C'),
('-1616018','ENOUGH! If you intend to reclaim Azeroth''s magic, then you shall have it!','14529','1','0','0','31778','malygos SAY_END_PHASE_2'),
('-1616019','Now your benefactors make their appearance, but they are too late! The powers contained here are sufficient to destroy the world ten times over. What do you think they will do to you?','14530','1','0','0','31779','malygos SAY_INTRO_PHASE_3'),
('-1616020','SUBMIT!','14531','1','0','0','31780','malygos SAY_START_PHASE_3'),
('-1616021','Alexstrasza! Another of your brood falls!','14534','1','0','0','31784','malygos SAY_SLAY_3_A'),
('-1616022','Little more then gnats!','14535','1','0','0','31785','malygos SAY_SLAY_3_B'),
('-1616023','Your red allies will share your fate...','14536','1','0','1','31786','malygos SAY_SLAY_3_C'),
('-1616024','The powers at work here exceed anything you could possibly imagine!','14532','1','0','0','31782','malygos SAY_SURGE'),
('-1616025','Still standing? Not for long...','14537','1','0','0','31787','malygos SAY_SPELL_1'),
('-1616026','Your cause is lost!','14538','1','0','0','31788','malygos SAY_SPELL_2'),
('-1616027','Your fragile mind will be shattered!','14539','1','0','0','31789','malygos SAY_SPELL_3'),
('-1616028','Unthinkable! The mortals will destroy... everything! My sister, what have you...','14540','1','0','0','31790','malygos SAY_DEATH'),
('-1616029','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1616030','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1616031','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1616032','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1616033','A Power Spark forms from a nearby rift!','0','3','0','0','32957','malygos SAY_EMOTE_SPARK'),
('-1616034','%s takes a deep breath.','0','3','0','0','20021','malygos SAY_EMOTE_BREATH');

-- -1 619 000 AHN'KAHET
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1619000','The secrets of the deep shall remain hidden.','14033','1','0','0','31884','nadox SAY_AGGRO'),
('-1619001','The young must not grow hungry...','14034','1','0','0','31886','nadox SAY_SUMMON_EGG_1'),
('-1619002','Shhhad ak kereeesshh chak-k-k!','14035','1','0','0','31887','nadox SAY_SUMMON_EGG_2'),
('-1619003','Sleep now, in the cold dark.','14036','1','0','0','31888','nadox SAY_SLAY_1'),
('-1619004','For the Lich King!','14037','1','0','0','23677','nadox SAY_SLAY_2'),
('-1619005','Perhaps we will be allies soon.','14038','1','0','0','31890','nadox SAY_SLAY_3'),
('-1619006','Master, is my service complete?','14039','1','0','0','31891','nadox SAY_DEATH'),
('-1619007','An Ahn''kahar Guardian hatches!','0','3','0','0','31894','nadox EMOTE_HATCH'),

('-1619008','I will feast on your remains.','14360','1','0','0','31895','taldaram SAY_AGGRO'),
('-1619009','Your heartbeat is music to my ears.','14361','1','0','0','31896','taldaram SAY_VANISH_1'),
('-1619010','I am nowhere. I am everywhere. I am the watcher, unseen.','14362','1','0','0','31897','taldaram SAY_VANISH_2'),
('-1619011','So appetizing.','14363','1','0','0','31898','taldaram SAY_FEED_1'),
('-1619012','Fresh, warm blood. It has been too long.','14364','1','0','0','31899','taldaram SAY_FEED_2'),
('-1619013','Bin-dor''el','14365','1','0','0','0','taldaram SAY_SLAY_1'),
('-1619014','I will drink no blood before it''s time.','14366','1','0','0','31901','taldaram SAY_SLAY_2'),
('-1619015','One final embrace.','14367','1','0','0','31902','taldaram SAY_SLAY_3'),
('-1619016','Still I hunger, still I thirst.','14368','1','0','0','31903','taldaram SAY_DEATH'),

('-1619017','These are sacred halls! Your intrusion will be met with death.','14343','1','0','0','30789','jedoga SAY_AGGRO'),
('-1619018','Who among you is devoted?','14344','1','0','0','30862','jedoga SAY_CALL_SACRIFICE_1'),
('-1619019','You there! Step forward!','14345','1','0','0','31699','jedoga SAY_CALL_SACRIFICE_2'),
('-1619020','Yogg-Saron, grant me your power!','14346','1','0','0','0','jedoga SAY_SACRIFICE_1'),
('-1619021','Master, a gift for you!','14347','1','0','0','31701','jedoga SAY_SACRIFICE_2'),
('-1619022','Glory to Yogg-Saron!','14348','1','0','0','31702','jedoga SAY_SLAY_1'),
('-1619023','You are unworthy!','14349','1','0','0','31703','jedoga SAY_SLAY_2'),
('-1619024','Get up! You haven''t suffered enough.','14350','1','0','0','31704','jedoga SAY_SLAY_3'),
('-1619025','Do not expect your sacrilege... to go unpunished.','14351','1','0','0','31705','jedoga SAY_DEATH'),
('-1619026','The elements themselves will rise up against the civilized world! Only the faithful will be spared!','14352','1','0','0','31706','jedoga SAY_PREACHING_1'),
('-1619027','Immortality can be yours. But only if you pledge yourself fully to Yogg-Saron!','14353','1','0','0','31707','jedoga SAY_PREACHING_2'),
('-1619028','Here on the very borders of his domain. You will experience powers you would never have imagined! ','14354','1','0','0','0','jedoga SAY_PREACHING_3'),
('-1619029','You have traveled long and risked much to be here. Your devotion shall be rewarded.','14355','1','0','0','31709','jedoga SAY_PREACHING_4'),
('-1619030','The faithful shall be exalted! But there is more work to be done. We will press on until all of Azeroth lies beneath his shadow!','14356','1','0','0','0','jedoga SAY_PREACHING_5'),
('-1619031','I have been chosen!','0','1','0','0','31179','jedoga SAY_VOLUNTEER_1'),
('-1619032','I give myself to the master!','0','1','0','0','30863','jedoga SAY_VOLUNTEER_2'),

('-1619033','Shgla''yos plahf mh''naus.','14043','1','0','0','32596','volazj SAY_AGGRO'),
('-1619034','Gul''kafh an''shel. Yoq''al shn ky ywaq nuul.','14044','1','0','0','32603','volazj SAY_INSANITY'),
('-1619035','Ywaq puul skshgn: on''ma yeh''glu zuq.','14045','1','0','0','32600','volazj SAY_SLAY_1'),
('-1619036','Ywaq ma phgwa''cul hnakf.','14046','1','0','0','32598','volazj SAY_SLAY_2'),
('-1619037','Ywaq maq oou; ywaq maq ssaggh. Ywaq ma shg''fhn.','14047','1','0','0','32602','volazj SAY_SLAY_3'),
('-1619038',' ','14048','1','0','0','0','volazj SAY_DEATH_1'),
('-1619039','Iilth vwah, uhn''agth fhssh za.','14049','1','0','0','32601','volazj SAY_DEATH_2');

-- -1 631 000 ICC: ICECROWN CITADEL
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1631001','This is the beginning AND the end, mortals. None may enter the master''s sanctum!','16950','1','0','0','38508','marrowgar SAY_INTRO'),
('-1631002','The Scourge will wash over this world as a swarm of death and destruction!','16941','1','0','0','37684','marrowgar SAY_AGGRO'),
('-1631003','BONE STORM!','16946','1','0','0','36554','marrowgar SAY_BONE_STORM'),
('-1631004','Bound by bone!','16947','1','0','0','37695','marrowgar SAY_BONE_SPIKE_1'),
('-1631005','Stick Around!','16948','1','0','0','37696','marrowgar SAY_BONE_SPIKE_2'),
('-1631006','The only escape is death!','16949','1','0','0','37693','marrowgar SAY_BONE_SPIKE_3'),
('-1631007','More bones for the offering!','16942','1','0','0','37686','marrowgar SAY_SLAY_1'),
('-1631008','Languish in damnation!','16943','1','0','0','37687','marrowgar SAY_SLAY_2'),
('-1631009','I see... only darkness...','16944','1','0','0','37688','marrowgar SAY_DEATH'),
('-1631010','THE MASTER''S RAGE COURSES THROUGH ME!','16945','1','0','0','37690','marrowgar SAY_BERSERK'),

('-1631011','You have found your way here, because you are among the few gifted with true vision in a world cursed with blindness.','17272','1','0','0','38139','deathwhisper SAY_SPEECH_1'),
('-1631012','You can see through the fog that hangs over this world like a shroud, and grasp where true power lies.','17273','1','0','0','38140','deathwhisper SAY_SPEECH_2'),
('-1631013','Fix your eyes upon your crude hands: the sinew, the soft meat, the dark blood coursing within.','16878','1','0','0','38134','deathwhisper SAY_SPEECH_3'),
('-1631014','It is a weakness; a crippling flaw.... A joke played by the Creators upon their own creations.','17268','1','0','0','38135','deathwhisper SAY_SPEECH_4'),
('-1631015','The sooner you come to accept your condition as a defect, the sooner you will find yourselves in a position to transcend it.','17269','1','0','0','38136','deathwhisper SAY_SPEECH_5'),
('-1631016','Through our Master, all things are possible. His power is without limit, and his will unbending.','17270','1','0','0','38137','deathwhisper SAY_SPEECH_6'),
('-1631017','Those who oppose him will be destroyed utterly, and those who serve -- who serve wholly, unquestioningly, with utter devotion of mind and soul -- elevated to heights beyond your ken.','17271','1','0','0','38138','deathwhisper SAY_SPEECH_7'),
('-1631018','What is this disturbance?! You dare trespass upon this hallowed ground? This shall be your final resting place.','16868','1','0','0','38122','deathwhisper SAY_AGGRO'),
('-1631019','Enough! I see I must take matters into my own hands!','16877','1','0','0','38126','deathwhisper SAY_PHASE_TWO'),
('-1631020','Take this blessing and show these intruders a taste of our master''s power.','16873','1','0','0','38130','deathwhisper SAY_DARK_EMPOWERMENT'),
('-1631021','I release you from the curse of flesh!','16874','1','0','0','38132','deathwhisper SAY_DARK_TRANSFORMATION'),
('-1631022','Arise and exalt in your pure form!','16875','1','0','0','38133','deathwhisper SAY_ANIMATE_DEAD'),
('-1631023','You are weak, powerless to resist my will!','16876','1','0','0','38127','deathwhisper SAY_DOMINATE_MIND'),
('-1631024','This charade has gone on long enough.','16872','1','0','0','38128','deathwhisper SAY_BERSERK'),
('-1631025','All part of the masters plan! Your end is... inevitable!','16871','1','0','0','38125','deathwhisper SAY_DEATH'),
('-1631026','Do you yet grasp of the futility of your actions?','16869','1','0','0','38123','deathwhisper SAY_SLAY_1'),
('-1631027','Embrace the darkness... Darkness eternal!','16870','1','0','0','38124','deathwhisper SAY_SLAY_2'),

('-1631028','BY THE MIGHT OF THE LICH KING!','16694','1','0','0','37990','saurfang SAY_AGGRO'),
('-1631029','The ground runs red with your blood!','16699','1','0','0','37995','saurfang SAY_FALLENCHAMPION'),
('-1631030','Feast, my minions!','16700','1','0','0','37996','saurfang SAY_BLOODBEASTS'),
('-1631031','You are nothing!','16695','1','0','0','18244','saurfang SAY_SLAY_1'),
('-1631032','Your soul will find no redemption here!','16696','1','0','0','37992','saurfang SAY_SLAY_2'),
('-1631033','I have become...DEATH!','16698','1','0','0','37994','saurfang SAY_BERSERK'),
('-1631034','I... Am... Released.','16697','1','0','0','37993','saurfang SAY_DEATH'),

('-1631035','Fire up the engines! We got a meetin'' with destiny, lads!','16962','1','0','0','37677','muradin SAY_GUNSHIP_START_1'),
('-1631036','Hold on to yer hats!','16963','1','0','0','37678','muradin SAY_GUNSHIP_START_2'),
('-1631037','What in the world is that? Grab me spyglass, crewman!','16964','1','0','0','37679','muradin SAY_GUNSHIP_START_3'),
('-1631038','By me own beard! HORDE SAILIN'' IN FAST ''N HOT!','16965','1','0','0','37680','muradin SAY_GUNSHIP_START_4'),
('-1631039','EVASIVE ACTION! MAN THE GUNS!','16966','1','0','0','37681','muradin SAY_GUNSHIP_START_5'),
('-1631040','Cowardly dogs! Ye blindsided us!','16967','1','0','0','37682','muradin SAY_GUNSHIP_START_6'),
('-1631041','This is not your battle, dwarf. Back down or we will be forced to destroy your ship.','17093','1','0','0','37685','saurfang SAY_GUNSHIP_START_7'),
('-1631042','Not me battle? I dunnae who ye think ye are, mister, but I got a score to settle with Arthas and yer not gettin'' in me way! FIRE ALL GUNS! FIRE! FIRE!','16968','1','0','0','37683','muradin SAY_GUNSHIP_START_8'),
('-1631043','Reavers, Sergeants, attack!','17081','1','0','0','37710','saurfang SAY_SUMMON_SOLDIERS'),
('-1631044','We''re taking hull damage, get a battle-mage out here to shut down those cannons!','17082','1','0','0','37711','saurfang SAY_SUMMON_MAGES'),
('-1631045','Axethrowers, hurl faster!','17079','1','0','0','37708','saurfang SAY_RANDOM_ATTACK_1'),
('-1631046','Rocketeers, reload!','0','1','0','0','37709','saurfang SAY_RANDOM_ATTACK_2'),
('-1631047','Don''t say I didn''t warn ya, scoundrels! Onward, brothers and sisters!','16959','1','0','0','37705','muradin SAY_WIN'),

('-1631048','Rise up, sons and daughters of the Horde! Today we battle a hated enemy of the Horde! LOK''TAR OGAR!','17087','1','0','0','37689','saurfang SAY_GUNSHIP_START_1'),
('-1631049','Kor''kron, take us out!','17088','1','0','0','37691','saurfang SAY_GUNSHIP_START_2'),
('-1631050','What is that? Something approaching in the distance.','17089','1','0','0','37692','saurfang SAY_GUNSHIP_START_3'),
('-1631051','ALLIANCE GUNSHIP! ALL HANDS ON DECK!','17090','1','0','0','37694','saurfang SAY_GUNSHIP_START_4'),
('-1631052','You answer to Saurfang now!','17091','1','0','0','37697','saurfang SAY_GUNSHIP_START_5'),
('-1631053','Move yer jalopy or we''ll blow it out of the sky, orc! The Horde''s got no business here. ','16969','1','0','0','37699','muradin SAY_GUNSHIP_START_6'),
('-1631054','You will know our business soon. Kor''kron, ANNIHILATE THEM!','17092','1','0','0','37698','saurfang SAY_GUNSHIP_START_7'),
('-1631055','Marines, Sergeants, attack!','16956','1','0','0','37702','muradin SAY_SUMMON_SOLDIERS'),
('-1631056','We''re taking hull damage, get a sorcerer out here to shut down those cannons!','16957','1','0','0','37703','muradin SAY_SUMMON_MAGES'),
('-1631057','Riflemen, shoot faster!','16954','1','0','0','37700','muradin SAY_RANDOM_ATTACK_1'),
('-1631058','Mortar team, reload!','0','1','0','0','37701','muradin SAY_RANDOM_ATTACK_2'),
('-1631059','The Alliance falter. Onward to the Lich King!','17084','1','0','0','37713','saurfang SAY_WIN'),

('-1631060','What''s this then?! Ye won\t be takin'' this son o'' Ironforge''s vessel without a fight!','16958','1','0','0','37704','muradin SAY_AGGRO'),
('-1631061','You DARE board my ship? Your death will come swiftly.','17083','1','0','0','37712','saurfang SAY_AGGRO'),

-- RE-USE -1631062 TO -1631069

('-1631070','What? Precious? Noooooooooo!!!','16993','6','0','0','37837','rotface SAY_PRECIOUS_DIES'),
('-1631071','WEEEEEE!','16986','1','0','0','37831','rotface SAY_AGGRO'),
('-1631072','Icky sticky.','16991','1','0','0','37836','rotface SAY_SLIME_SPRAY'),
('-1631073','I think I made an angry poo-poo. It gonna blow!','16992','1','0','0','37063','rotface SAY_OOZE_EXPLODE'),
('-1631074','Great news, everyone! The slime is flowing again!','17126','1','0','0','38120','putricide SAY_SLIME_FLOW_1'),
('-1631075','Good news, everyone! I''ve fixed the poison slime pipes!','17123','1','0','0','37049','putricide SAY_SLIME_FLOW_2'),
('-1631076','Daddy make toys out of you!','16987','1','0','0','37833','rotface SAY_SLAY_1'),
('-1631077','I brokes-ded it...','16988','1','0','0','37832','rotface SAY_SLAY_2'),
('-1631078','Sleepy Time!','16990','1','0','0','37835','rotface SAY_BERSERK'),
('-1631079','Bad news daddy.','16989','1','0','0','37834','rotface SAY_DEATH'),
('-1631080','Terrible news, everyone, Rotface is dead! But great news everyone, he left behind plenty of ooze for me to use! Whaa...? I''m a poet, and I didn''t know it? Astounding!','17146','6','0','0','37847','putricide SAY_ROTFACE_DEATH'),

('-1631081','NOOOO! You kill Stinky! You pay!','16907','6','0','0','37830','festergut SAY_STINKY_DIES'),
('-1631082','Fun time!','16901','1','0','0','37823','festergut SAY_AGGRO'),
('-1631083','Just an ordinary gas cloud. But watch out, because that''s no ordinary gas cloud! ','17119','1','0','0','37843','putricide SAY_BLIGHT'),
('-1631084','%s farts.','16911','2','0','0','0','festergut SAY_SPORE'),
('-1631085','I not feel so good...','16906','1','0','0','37829','festergut SAY_PUNGUENT_BLIGHT'),
('-1631086','%s vomits','0','2','0','0','0','festergut SAY_PUNGUENT_BLIGHT_EMOTE'),
('-1631087','Daddy, I did it','16902','1','0','0','37824','festergut SAY_SLAY_1'),
('-1631088','Dead, dead, dead!','16903','1','0','0','37825','festergut SAY_SLAY_2'),
('-1631089','Fun time over!','16905','1','0','0','37827','festergut SAY_BERSERK'),
('-1631090','Da ... Ddy...','16904','1','0','0','37826','festergut SAY_DEATH'),
('-1631091','Oh, Festergut. You were always my favorite. Next to Rotface. The good news is you left behind so much gas, I can practically taste it!','17124','6','0','0','37848','putricide SAY_FESTERGUT_DEATH'),

('-1631092','Good news, everyone! I think I perfected a plague that will destroy all life on Azeroth!','17114','1','0','0','37838','putricide SAY_AGGRO'),
('-1631093','You can''t come in here all dirty like that! You need that nasty flesh scrubbed off first!','17125','1','0','0','38535','putricide SAY_AIRLOCK'),
('-1631094','Two oozes, one room! So many delightful possibilities...','17122','1','0','0','37846','putricide SAY_PHASE_CHANGE'),
('-1631095','Hmm. I don''t feel a thing. Whaa...? Where''d those come from?','17120','1','0','0','37844','putricide SAY_TRANSFORM_1'),
('-1631096','Tastes like... Cherry! Oh! Excuse me!','17121','1','0','0','37845','putricide SAY_TRANSFORM_2'),
('-1631097','Hmm... Interesting...','17115','1','0','0','33033','putricide SAY_SLAY_1'),
('-1631098','That was unexpected!','17116','1','0','0','37840','putricide SAY_SLAY_2'),
('-1631099','Great news, everyone!','17118','1','0','0','37842','putricide SAY_BERSERK'),
('-1631100','Bad news, everyone! I don''t think I''m going to make it','17117','1','0','0','37841','putricide SAY_DEATH'),

('-1631101','Foolish mortals. You thought us defeated so easily? The San''layn are the Lich King''s immortal soldiers! Now you shall face their might combined!','16795','6','0','1','37997','lanathel SAY_COUNCIL_INTRO_1'),
('-1631102','Rise up, brothers, and destroy our enemies','16796','6','0','0','38079','lanathel SAY_COUNCIL_INTRO_2'),

('-1631103','Such wondrous power! The Darkfallen Orb has made me INVINCIBLE!','16727','1','0','0','38006','keleseth SAY_KELESETH_INVOCATION'),
('-1631104','Blood will flow!','16728','1','0','0','21087','keleseth SAY_KELESETH_SPECIAL'),
('-1631105','Were you ever a threat?','16723','1','0','0','38003','keleseth SAY_KELESETH_SLAY_1'),
('-1631106','Truth is found in death.','16724','1','0','0','38004','keleseth SAY_KELESETH_SLAY_2'),
('-1631107','%s cackles maniacally!','16726','2','0','0','29805','keleseth SAY_KELESETH_BERSERK'),
('-1631108','My queen... they come...','16725','1','0','0','38005','keleseth SAY_KELESETH_DEATH'),

('-1631109','Tremble before Taldaram, mortals, for the power of the orb flows through me!','16857','1','0','0','38010','taldaram SAY_TALDARAM_INVOCATION'),
('-1631110','Delight in the pain!','16858','1','0','0','38011','taldaram SAY_TALDARAM_SPECIAL'),
('-1631111','Worm food.','16853','1','0','0','38008','taldaram SAY_TALDARAM_SLAY_1'),
('-1631112','Beg for mercy!','16854','1','0','0','38009','taldaram SAY_TALDARAM_SLAY_2'),
('-1631113','%s laughs.','16856','2','0','0','10315','taldaram SAY_TALDARAM_BERSERK'),
('-1631114','%s gurgles and dies.','16855','2','0','0','0','taldaram SAY_TALDARAM_DEATH'),

('-1631115','Naxxanar was merely a setback! With the power of the orb, Valanar will have his vengeance!','16685','1','0','0','38001','valanar SAY_VALANAR_INVOCATION'),
('-1631116','My cup runneth over.','16686','1','0','0','38002','valanar SAY_VALANAR_SPECIAL'),
('-1631117','Dinner... is served.','16681','1','0','0','37998','valanar SAY_VALANAR_SLAY_1'),
('-1631118','Do you see NOW the power of the Darkfallen?','16682','1','0','0','37999','valanar SAY_VALANAR_SLAY_2'),
('-1631119','BOW DOWN BEFORE THE SAN''LAYN!','16684','1','0','0','38000','valanar SAY_VALANAR_BERSERK'),
('-1631120','...why...?','16683','1','0','0','13802','valanar SAY_VALANAR_DEATH'),

('-1631121','You have made an... unwise... decision.','16782','1','0','0','38048','blood_queen SAY_AGGRO'),
('-1631122','Just a taste...','16783','1','0','0','38053','blood_queen SAY_BITE_1'),
('-1631123','Know my hunger!','16784','1','0','0','38058','blood_queen SAY_BITE_2'),
('-1631124','SUFFER!','16786','1','0','0','48466','blood_queen SAY_SHADOWS'),
('-1631125','Can you handle this?','16787','1','0','0','38062','blood_queen SAY_PACT'),
('-1631126','Yes... feed my precious one! You''re mine now!','16790','1','0','0','0','blood_queen SAY_MC'),
('-1631127','Here it comes.','16788','1','0','0','16893','blood_queen SAY_AIR_PHASE'),
('-1631128','THIS ENDS NOW!','16793','1','0','0','38068','blood_queen SAY_BERSERK'),
('-1631129','But... we were getting along... so well...','16794','1','0','0','38069','blood_queen SAY_DEATH'),

('-1631130','Ready your arms, my Argent Brothers. The Vrykul will protect the Frost Queen with their lives.','16819','1','0','0','36945','scourgebane SAY_SVALNA_EVENT_1'),
('-1631131','Even dying here beats spending another day collecting reagents for that madman, Finklestein.','16585','1','0','0','36948','arnath SAY_SVALNA_EVENT_2'),
('-1631132','Enough idle banter! Our champions have arrived - support them as we push our way through the hall!','16820','1','0','0','36946','scourgebane SAY_SVALNA_EVENT_3'),
('-1631133','You may have once fought beside me, Crok, but now you are nothing more than a traitor. Come, your second death approaches!','17017','1','0','0','37022','svalna SAY_SVALNA_EVENT_4'),
('-1631134','Miserable creatures, Die!','17018','1','0','0','0','svalna SAY_KILLING_CRUSADERS'),
('-1631135','Foolish Crok, you brought my reinforcements with you! Arise Argent Champions and serve the Lich King in death!','17019','1','0','0','37020','svalna SAY_RESSURECT'),
('-1631136','Come Scourgebane, I''ll show the Lich King which one of us is truly worthy of the title, champion!','17020','1','0','0','37653','svalna SAY_SVALNA_AGGRO'),
('-1631137','What? They died so easily? No matter.','17022','1','0','0','37163','svalna SAY_KILL_CAPTAIN'),
('-1631138','What a pitiful choice of an ally Crok.','17021','1','0','0','37654','svalna SAY_KILL_PLAYER'),
('-1631139','Perhaps... you were right... Crok.','17023','1','0','0','37135','svalna SAY_DEATH'),

('-1631140','Heroes, lend me your aid! I... I cannot hold them off much longer! You must heal my wounds!','17064','1','0','0','37875','dreamwalker SAY_AGGRO'),
('-1631141','I have opened a portal into the Dream. Your salvation lies within, heroes.','17068','1','0','0','37876','dreamwalker SAY_PORTAL'),
('-1631142','My strength is returning! Press on, heroes!','17070','1','0','0','37878','dreamwalker SAY_75_HEALTH'),
('-1631143','I will not last much longer!','17069','1','0','0','21784','dreamwalker SAY_25_HEALTH'),
('-1631144','Forgive me for what I do! I... cannot... stop... ONLY NIGHTMARES REMAIN!','17072','1','0','0','0','dreamwalker SAY_0_HEALTH'),
('-1631145','A tragic loss...','17066','1','0','0','0','dreamwalker SAY_PLAYER_DIES'),
('-1631146','FAILURES!','17067','1','0','0','0','dreamwalker SAY_BERSERK'),
('-1631147','I am renewed! Ysera grants me the favor to lay these foul creatures to rest!','17071','1','0','0','37852','dreamwalker SAY_VICTORY'),

('-1631148','You are fools who have come to this place! The icy winds of Northrend will consume your souls!','17007','1','0','0','37511','sindragosa SAY_AGGRO'),
('-1631149','Suffer, mortals, as your pathetic magic betrays you!','17014','1','0','0','37138','sindragosa SAY_UNCHAINED_MAGIC'),
('-1631150','Can you feel the cold hand of death upon your heart?','17013','1','0','0','37137','sindragosa SAY_BLISTERING_COLD'),
('-1631151','Aaah! It burns! What sorcery is this?!','17015','1','0','0','38550','sindragosa SAY_RESPIRE'),
('-1631152','Your incursion ends here! None shall survive!','17012','1','0','0','37136','sindragosa SAY_TAKEOFF'),
('-1631153','Now feel my master''s limitless power and despair!','17016','1','0','0','37109','sindragosa SAY_PHASE_3'),
('-1631154','Perish!','17008','1','0','0','47717','sindragosa SAY_SLAY_1'),
('-1631155','A flaw of mortality...','17009','1','0','0','0','sindragosa SAY_SLAY_2'),
('-1631156','Enough! I tire of these games!','17011','1','0','0','37140','sindragosa SAY_BERSERK'),
('-1631157','Free...at last...','17010','1','0','0','13948','sindragosa SAY_DEATH'),

('-1631158','So...the Light''s vaunted justice has finally arrived. Shall I lay down Frostmourne and throw myself at your mercy, Fordring?','17349','1','0','0','38070','lich_king SAY_INTRO_1'),
('-1631159','We will grant you a swift death, Arthas. More than can be said for the thousands you''ve tortured and slain.','17390','1','0','0','38109','tirion SAY_INTRO_2'),
('-1631160','You will learn of that first hand. When my work is complete, you will beg for mercy -- and I will deny you. Your anguished cries will be testament to my unbridled power.','17350','1','0','0','38071','lich_king SAY_INTRO_3'),
('-1631161','So be it. Champions, attack!','17391','1','0','0','38110','tirion SAY_INTRO_4'),
('-1631162','I''ll keep you alive to witness the end, Fordring. I would not want the Light''s greatest champion to miss seeing this wretched world remade in my image.','17351','1','0','0','38072','lich_king SAY_INTRO_5'),
('-1631163','Come then champions, feed me your rage!','17352','1','0','0','38073','lich_king SAY_AGGRO'),
('-1631164','I will freeze you from within until all that remains is an icy husk!','17369','1','0','0','38046','lich_king SAY_REMORSELESS_WINTER'),
('-1631165','Watch as the world around you collapses!','17370','1','0','0','38047','lich_king SAY_SHATTER_ARENA'),
('-1631166','Val''kyr, your master calls!','17373','1','0','0','38052','lich_king SAY_SUMMON_VALKYR'),
('-1631167','Frostmourne hungers...','17366','1','0','0','38042','lich_king SAY_HARVEST_SOUL'),
('-1631168','You have come to bring Arthas to justice? To see the Lich King destroyed?','17394','1','0','0','38114','terenas SAY_FM_TERENAS_AID_1'),
('-1631169','First, you must escape Frostmourne''s hold, or be damned as I am; trapped within this cursed blade for all eternity.','17395','1','0','0','38115','terenas SAY_FM_TERENAS_AID_2'),
('-1631170','Aid me in destroying these tortured souls! Together we will loosen Frostmourne''s hold and weaken the Lich King from within!','17396','1','0','0','38116','terenas SAY_FM_TERENAS_AID_3'),
('-1631171','Argh... Frostmourne, obey me!','17367','1','0','0','38043','lich_king SAY_FM_PLAYER_ESCAPE'),
('-1631172','Frostmourne feeds on the soul of your fallen ally!','17368','1','0','0','38044','lich_king SAY_FM_PLAYER_DEATH'),
('-1631173','Apocalypse!','17371','1','0','0','28736','lich_king SAY_SPECIAL_1'),
('-1631174','Bow down before your lord and master!','17372','1','0','0','38050','lich_king SAY_SPECIAL_2'),
('-1631175','You gnats actually hurt me! Perhaps I''ve toyed with you long enough, now taste the vengeance of the grave!','17359','1','0','0','38081','lich_king SAY_LAST_PHASE'),
('-1631176','Hope wanes!','17363','1','0','0','38038','lich_king SAY_SLAY_1'),
('-1631177','The end has come!','17364','1','0','0','38040','lich_king SAY_SLAY_2'),
('-1631178','Face now your tragic end!','17365','1','0','0','38041','lich_king SAY_ENRAGE'),
('-1631179','No question remains unanswered. No doubts linger. You are Azeroth''s greatest champions! You overcame every challenge I laid before you. My mightiest servants have fallen before your relentless onslaught, your unbridled fury...','17353','1','0','0','38074','lich_king SAY_OUTRO_1'),
('-1631180','Is it truly righteousness that drives you? I wonder','17354','1','0','0','38075','lich_king SAY_OUTRO_2'),
('-1631181','You trained them well, Fordring. You delivered the greatest fighting force this world has ever known... right into my hands -- exactly as I intended. You shall be rewarded for your unwitting sacrifice.','17355','1','0','0','38076','lich_king SAY_OUTRO_3'),
('-1631182','Watch now as I raise them from the dead to become masters of the Scourge. They will shroud this world in chaos and destruction. Azeroth''s fall will come at their hands -- and you will be the first to die.','17356','1','0','0','38077','lich_king SAY_OUTRO_4'),
('-1631183','I delight in the irony.','17357','1','0','0','38078','lich_king SAY_OUTRO_5'),
('-1631184','LIGHT, GRANT ME ONE FINAL BLESSING. GIVE ME THE STRENGTH... TO SHATTER THESE BONDS!','17392','1','0','0','38111','tirion SAY_OUTRO_6'),
('-1631185','Impossible...','17358','1','0','0','29825','lich_king SAY_OUTRO_7'),
('-1631186','No more, Arthas! No more lives will be consumed by your hatred!','17393','1','0','0','38112','tirion SAY_OUTRO_8'),
('-1631187','Free at last! It is over, my son. This is the moment of reckoning.','17397','1','0','0','38117','terenas SAY_OUTRO_9'),
('-1631188','Rise up, champions of the Light!','17398','1','0','0','38118','terenas SAY_OUTRO_10'),
('-1631189','THE LICH KING...MUST...FALL!','17389','1','0','0','38113','tirion SAY_OUTRO_11'),
('-1631190','Now I stand, the lion before the lambs... and they do not fear.','17361','1','0','0','39003','lich_king SAY_OUTRO_12'),
('-1631191','They cannot fear.','17362','1','0','0','39004','lich_king SAY_OUTRO_13'),
('-1631192','%s dies','17374','2','0','0','8251','lich_king SAY_OUTRO_14'),

('-1631193','%s goes into a frenzy!','0','3','0','0','10677','saurfang EMOTE_FRENZY'),
('-1631194','%s''s Blood Beasts gain the scent of blood!','0','3','0','0','38631','saurfang EMOTE_SCENT'),
('-1631195','Really... Is that all you got?','16791','1','0','0','38066','blood_queen SAY_SLAY_1'),
('-1631196','Such a pity...','16792','1','0','0','49269','blood_queen SAY_SLAY_2'),

('-1631197','Invocation of Blood jumps to %s!','0','3','0','0','0','blood_princes EMOTE_INVOCATION'),
('-1631198','%s begins casting Empowered Shock Vortex!','0','3','0','0','38972','valanar EMOTE_SHOCK_VORTEX'),
('-1631199','%s speed toward $N!','0','3','0','0','0','taldaram EMOTE_FLAMES'),

('-1631200','Terrible news everyone. Rotface is dead, but great news everyone! He left behind plenty of ooze for me to use! What? I''m a poet and I didn''t know it.... Astounding!','17146','1','0','0','37847','festergut SAY_BLIGHT_2'),
('-1631201','%s releases Gas Spores!','0','3','0','0','36706','festergut EMOTE_SPORES'),
('-1631202','%s begins to cast Slime Spray!','0','3','0','0','38363','rotface EMOTE_SLIME_SPRAY'),
('-1631203','%s grows more unstable!','0','2','0','0','36950','rotface EMOTE_OOZE_GROW_1'),
('-1631204','%s is growing volatile!','0','2','0','0','36952','rotface EMOTE_OOZE_GROW_2'),
('-1631205','%s can barely maintain its form!','0','2','0','0','36956','rotface EMOTE_OOZE_GROW_3'),
('-1631206','%s begins to cast Unstable Experiment!','0','3','0','0','38500','putricide EMOTE_EXPERIMENT');

-- -1 632 000 ICC: FORGE OF SOULS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1632000','More souls to power the engine!','0','1','0','0','36524','boss_bronjahm SAY_AGGRO_1'),
('-1632001','Finally...a captive audience!','16595','1','0','0','38486','boss_bronjahm SAY_AGGRO_2'),
('-1632002','Fodder for the engine!','16596','1','0','0','38490','boss_bronjahm SAY_SLAY_1'),
('-1632003','Another soul to strengthen the host!','16597','1','0','0','38491','boss_bronjahm SAY_SLAY_2'),
('-1632004','My soul for you, master.','16598','1','0','0','36525','boss_bronjahm SAY_DEATH'),
('-1632005','The vortex of the harvested calls to you!','16599','1','0','0','36526','boss_bronjahm SAY_SOULSTORM'),
('-1632006','I will sever the soul from your body!','16600','1','0','0','38489','boss_bronjahm SAY_CORRUPT_SOUL'),

('-1632007','You dare look upon the host of souls?! I SHALL DEVOUR YOU WHOLE!','16884','1','0','0','38012','boss_devourer SAY_MALE_1_AGGRO'),
('-1632008','You dare look upon the host of souls?! I SHALL DEVOUR YOU WHOLE!','16890','1','0','0','38012','boss_devourer SAY_FEMALE_AGGRO'),
('-1632009','Damnation!','16885','1','0','0','38013','boss_devourer SAY_MALE_1_SLAY_1'),
('-1632010','Damnation!','16891','1','0','0','38013','boss_devourer SAY_FEMALE_SLAY_1'),
('-1632011','Damnation!','16896','1','0','0','38013','boss_devourer SAY_MALE_2_SLAY_1'),
('-1632012','Doomed for eternity!','16886','1','0','0','38015','boss_devourer SAY_MALE_1_SLAY_2'),
('-1632013','Doomed for eternity!','16892','1','0','0','38015','boss_devourer SAY_FEMALE_SLAY_2'),
('-1632014','Doomed for eternity!','16897','1','0','0','38015','boss_devourer SAY_MALE_2_SLAY_2'),
('-1632015','The swell of souls will not be abated! You only delay the inevitable!','16887','1','0','0','38547','boss_devourer SAY_MALE_1_DEATH'),
('-1632016','The swell of souls will not be abated! You only delay the inevitable!','16893','1','0','0','38547','boss_devourer SAY_FEMALE_DEATH'),
('-1632017','The swell of souls will not be abated! You only delay the inevitable!','16898','1','0','0','38547','boss_devourer SAY_MALE_2_DEATH'),
('-1632018','SUFFERING! ANGUISH! CHAOS! RISE AND FEED!','16888','1','0','0','38019','boss_devourer SAY_MALE_1_SOUL_ATTACK'),
('-1632019','SUFFERING! ANGUISH! CHAOS! RISE AND FEED!','16894','1','0','0','38019','boss_devourer SAY_FEMALE_SOUL_ATTACK'),
('-1632020','SUFFERING! ANGUISH! CHAOS! RISE AND FEED!','16899','1','0','0','38019','boss_devourer SAY_MALE_2_SOUL_ATTACK'),
('-1632021','Stare into the abyss, and see your end!','16889','1','0','0','32949','boss_devourer SAY_MALE_1_DARK_GLARE'),
('-1632022','Stare into the abyss, and see your end!','16895','1','0','0','32949','boss_devourer SAY_FEMALE_DARK_GLARE'),
('-1632023','%s begins to cast Mirrored Soul!','0','3','0','0','37368','boss_devourer EMOTE_MIRRORED_SOUL'),
('-1632024','%s begins to Unleash Souls!','0','3','0','0','36659','boss_devourer EMOTE_UNLEASH_SOULS'),
('-1632025','%s begins to cast Wailing Souls!','0','3','0','0','36658','boss_devourer EMOTE_WAILING_SOULS');

-- -1 649 000 TRIAL OF THE CRUSADER
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1649000','Welcome champions, you have heard the call of the argent crusade and you have boldly answered. It is here in the crusaders coliseum that you will face your greatest challenges. Those of you who survive the rigors of the coliseum will join the Argent Crusade on it''s marsh to ice crown citadel.','16036','1','0','0','35708','tirion SAY_TIRION_RAID_INTRO_LONG'),
('-1649001','Welcome to the trials of the crusader. Only the most powerful combatants of azeroth are allowed to undergo these trials. You are among the worthy few.','16053','1','0','0','35725','tirion SAY_RAID_TRIALS_INTRO'),

('-1649002','Hailing from the deepest, darkest carverns of the storm peaks, Gormok the Impaler! Battle on, heroes!','16038','1','0','0','35710','tirion SAY_TIRION_BEAST_1'),
('-1649003','Your beast will be no match for my champions Tirion.','16069','1','0','1','35693','varian SAY_VARIAN_BEAST_1'),
('-1649004','I have seen more worthy challenges in the ring of blood, you waste our time paladin.','16026','1','0','1','35707','garrosh SAY_GARROSH_BEAST_1'),
('-1649005','Steel yourselves, heroes, for the twin terrors Acidmaw and Dreadscale. Enter the arena!','16039','1','0','0','35711','tirion SAY_TIRION_BEAST_2'),
('-1649006','The air freezes with the introduction of our next combatant, Icehowl! Kill or be killed, champions!','16040','1','0','0','35712','tirion SAY_TIRION_BEAST_3'),
('-1649007','The monstrous menagerie has been vanquished!','16041','1','0','0','35713','tirion SAY_TIRION_BEAST_SLAY'),
('-1649008','Tragic... They fought valiantly, but the beasts of Northrend triumphed. Let us observe a moment of silence for our fallen heroes.','16042','1','0','0','35714','tirion SAY_TIRION_BEAST_WIPE'),

('-1649009','Grand Warlock Wilfred Fizzlebang will summon forth your next challenge. Stand by for his entry!','16043','1','0','0','35715','tirion SAY_TIRION_JARAXXUS_INTRO_1'),
('-1649010','Thank you, Highlord. Now, challengers, I will begin the ritual of summoning. When I am done a fearsome doomguard will appear!','16268','1','0','2','35726','wilfred SAY_WILFRED_JARAXXUS_INTRO_1'),
('-1649011','Prepare for oblivion!','16269','1','0','0','35727','wilfred SAY_WILFRED_JARAXXUS_INTRO_2'),
('-1649012','Ah ha! Behold the absolute power of Wilfred Fizzlebang, master summoner! You are bound to ME, demon!','16270','1','0','5','35728','wilfred SAY_WILFRED_JARAXXUS_INTRO_3'),
('-1649013','Trifling gnome, your arrogance will be your undoing!','16143','1','0','397','35730','jaraxxus SAY_JARAXXUS_JARAXXAS_INTRO_1'),
('-1649014','But I''m in charge her-','16271','1','0','5','35729','wilfred SAY_WILFRED_DEATH'),
('-1649015','Quickly, heroes! Destroy the demon lord before it can open a portal to its twisted demonic realm!','16044','1','0','5','35716','tirion SAY_TIRION_JARAXXUS_INTRO_2'),
('-1649016','The loss of Wilfred Fizzlebang, while unfortunate, should be a lesson to those that dare dabble in dark magic. Alas, you are victorious and must now face the next challenge.','16045','1','0','0','35717','tirion SAY_TIRION_JARAXXUS_EXIT_1'),
('-1649017','Treacherous Alliance dogs! You summon a demon lord against warriors of the Horde!? Your deaths will be swift!','16021','1','0','5','35699','garrosh SAY_GARROSH_JARAXXUS_EXIT_1'),
('-1649018','The Alliance doesn''t need the help of a demon lord to deal with Horde filth. Come, pig!','16064','1','0','5','35688','varian SAY_VARIAN_JARAXXUS_SLAY'),
('-1649019','Everyone, calm down! Compose yourselves! There is no conspiracy at play here. The warlock acted on his own volition - outside of influences from the Alliance. The tournament must go on!','16046','1','0','5','35718','tirion SAY_TIRION_JARAXXUS_EXIT_2'),

('-1649020','The next battle will be against the Argent Crusade''s most powerful knights! Only by defeating them will you be deemed worthy...','16047','1','0','0','35719','tirion SAY_TIRION_PVP_INTRO_1'),
('-1649021','The Horde demands justice! We challenge the Alliance. Allow us to battle in place of your knights, paladin. We will show these dogs what it means to insult the Horde!','16023','1','0','5','35703','garrosh SAY_GARROSH_PVP_A_INTRO_1'),
('-1649022','Our honor has been besmirched! They make wild claims and false accusations against us. I demand justice! Allow my champions to fight in place of your knights, Tirion. We challenge the Horde!','16066','1','0','5','35690','varian SAY_VARIAN_PVP_H_INTRO_1'),
('-1649023','Very well, I will allow it. Fight with honor!','16048','1','0','1','35720','tirion SAY_TIRION_PVP_INTRO_2'),
('-1649024','Fight for the glory of the Alliance, heroes! Honor your king and your people!','16065','1','0','5','35689','varian SAY_VARIAN_PVP_H_INTRO_2'),
('-1649025','Show them no mercy, Horde champions! LOK''TAR OGAR!','16022','1','0','1','35701','garrosh SAY_GARROSH_PVP_A_INTRO_2'),
('-1649026','GLORY TO THE ALLIANCE!','16067','1','0','5','35691','varian SAY_VARIAN_PVP_A_WIN'),
('-1649027','That was just a taste of what the future brings. FOR THE HORDE!','16024','1','0','1','35705','garrosh SAY_GARROSH_PVP_H_WIN'),
('-1649028','A shallow and tragic victory. We are weaker as a whole from the losses suffered today. Who but the Lich King could benefit from such foolishness? Great warriors have lost their lives. And for what? The true threat looms ahead - the Lich King awaits us all in death.','16049','1','0','0','35721','tirion SAY_TIRION_PVP_WIN'),

('-1649029','Only by working together will you overcome the final challenge. From the depths of Icecrown come two of the Scourge''s most powerful lieutenants: fearsome val''kyr, winged harbingers of the Lich King!','16050','1','0','0','35722','tirion SAY_TIRION_TWINS_INTRO'),
('-1649030','Let the games begin!','16037','1','0','0','9845','tirion SAY_RAID_INTRO_SHORT'),
('-1649031','Not even the lich king''s most powerful minions could stand against the alliance. All hail our victors.','16068','1','0','1','35692','varian SAY_VARIAN_TWINS_A_WIN'),
('-1649032','Do you still question the might of the Horde, paladin? We will take on all comers!','16025','1','0','0','35706','garrosh SAY_GARROSH_TWINS_H_WIN'),
('-1649033','A mighty blow has been dealt to the Lich King! You have proven yourselves able bodied champions of the Argent Crusade. Together we will strike at Icecrown Citadel and destroy what remains of the Scourge! There is no challenge that we cannot face united!','16051','1','0','5','35723','tirion SAY_TIRION_TWINS_WIN'),

('-1649034','You will have your challenge, Fordring.','16321','1','0','0','35747','lich_king SAY_LKING_ANUB_INTRO_1'),
('-1649035','Arthas! You are hopelessly outnumbered! Lay down Frostmourne and I will grant you a just death.','16052','1','0','25','35724','tirion SAY_TIRION_ABUN_INTRO_1'),
('-1649036','The Nerubians built an empire beneath the frozen wastes of Northrend. An empire that you so foolishly built your structures upon. MY EMPIRE.','16322','1','0','11','35748','lich_king SAY_LKING_ANUB_INTRO_2'),
('-1649037','The souls of your fallen champions will be mine, Fordring.','16323','1','0','0','35749','lich_king SAY_LKING_ANUB_INTRO_3'),
('-1649038','Ahhh, our guests have arrived, just as the master promised.','16235','1','0','0','35750','anubarak SAY_ANUB_ANUB_INTRO_1'),

('-1649039','%s glares at $N and lets out a bellowing roar!','0','3','0','0','35675','icehowl EMOTE_MASSIVE_CRASH'),

('-1649040','You face Jaraxxus, eredar lord of the Burning Legion!','16144','1','0','0','35731','jaraxxus SAY_AGGRO'),
('-1649041','Insignificant gnat!','16145','1','0','0','35732','jaraxxus SAY_SLAY_1'),
('-1649042','Banished to the Nether!','16146','1','0','0','35733','jaraxxus SAY_SLAY_2'),
('-1649043','Another will take my place. Your world is doomed.','16147','1','0','0','35734','jaraxxus SAY_DEATH'),
('-1649044','<SD2 Text missing>','16148','1','0','0','0','jaraxxus SAY_BERSERK'),
('-1649045','Flesh from bone!','16149','1','0','0','35735','jaraxxus SAY_INCINERATE'),
('-1649046','Come forth, sister! Your master calls!','16150','1','0','0','35736','jaraxxus SAY_MISTRESS'),
('-1649047','Inferno!','16151','1','0','0','45231','jaraxxus SAY_INFERNO'),

('-1649048','Weakling!','16017','1','0','0','35694','garrosh SAY_GARROSH_PVP_A_SLAY_1'),
('-1649049','Pathetic!','16018','1','0','274','70319','garrosh SAY_GARROSH_PVP_A_SLAY_2'),
('-1649050','Overpowered.','16019','1','0','25','35697','garrosh SAY_GARROSH_PVP_A_SLAY_3'),
('-1649051','Lok''tar!','16020','1','0','5','34904','garrosh SAY_GARROSH_PVP_A_SLAY_4'),
('-1649052','Hah!','16060','1','0','5','34316','varian SAY_VARIAN_PVP_H_SLAY_1'),
('-1649053','Hardly a challenge!','16061','1','0','274','35685','varian SAY_VARIAN_PVP_H_SLAY_2'),
('-1649054','Worthless scrub.','16062','1','0','25','35686','varian SAY_VARIAN_PVP_H_SLAY_3'),
('-1649055','Is this the best the Horde has to offer?','16063','1','0','6','35687','varian SAY_VARIAN_PVP_H_SLAY_4'),

('-1649056','In the name of our dark master. For the Lich King. You. Will. Die.','16272','1','0','0','35738','twin_valkyr SAY_AGGRO'),
('-1649057','You are finished!','16273','1','0','0','71863','twin_valkyr SAY_BERSERK'),
('-1649058','Chaos!','16274','1','0','0','25553','twin_valkyr SAY_COLORSWITCH'),
('-1649059','The Scourge cannot be stopped...','16275','1','0','0','35741','twin_valkyr SAY_DEATH'),
('-1649060','You have been measured, and found wanting!','16276','1','0','0','27866','twin_valkyr SAY_SLAY_1'),
('-1649061','Unworthy!','16277','1','0','0','75038','twin_valkyr SAY_SLAY_2'),
('-1649062','Let the dark consume you!','16278','1','0','0','35745','twin_valkyr SAY_TO_BLACK'),
('-1649063','Let the light consume you!','16279','1','0','0','35746','twin_valkyr SAY_TO_WHITE'),

('-1649064','This place will serve as your tomb!','16234','1','0','0','35751','anubarak SAY_AGGRO'),
('-1649065','F-lakkh shir!','16236','1','0','0','35752','anubarak SAY_SLAY_1'),
('-1649066','Another soul to sate the host.','16237','1','0','0','35753','anubarak SAY_SLAY_2'),
('-1649067','I have failed you, master...','16238','1','0','0','35754','anubarak SAY_DEATH'),
('-1649068','<SD2 Text missing>','16239','1','0','0','0','anubarak SAY_BERSERK'),
('-1649069','Auum na-l ak-k-k-k, isshhh. Rise, minions. Devour...','16240','1','0','0','35756','anubarak SAY_SUBMERGE'),
('-1649070','The swarm shall overtake you!','16241','1','0','0','35757','anubarak SAY_LEECHING_SWARM'),

('-1649071','%s burrows into the ground!','0','3','0','0','35071','anubarak EMOTE_BURROW'),
('-1649072','%s spikes pursue $N!','0','3','0','0','35676','anubarak EMOTE_PURSUE'),
('-1649073','%s emerges from the ground!','0','3','0','0','35968','anubarak EMOTE_EMERGE'),
('-1649074','%s unleashes a Leeching Swarm to heal himself!','0','3','0','0','35070','anubarak EMOTE_SWARM'),

('-1649075','Champions, you''re alive! Not only have you defeated every challenge of the Trial of the Crusader, but also thwarted Arthas'' plans! Your skill and cunning will prove to be a powerful weapon against the Scourge. Well done! Allow one of the Crusade''s mages to transport you to the surface!','0','0','0','1','36129','tirion SAY_EPILOGUE'),

('-1649076','As its companion perishes, %s becomes enraged!','0','3','0','0','33770','twin jormungars EMOTE_JORMUNGAR_ENRAGE'),
('-1649077','%s crashes into the Coliseum wall and is stunned!','0','3','0','0','35317','icehowl EMOTE_WALL_CRASH');

-- -1 650 000 TRIAL OF THE CHAMPION
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1650000','The Silver Covenant is pleased to present their contenders for this event, Highlord.','0','1','0','396','35259','toc herald SAY_HORDE_CHALLENGE'),
('-1650001','Presenting the fierce Grand Champion of Orgrimmar, Mokra the Skullcrusher!','0','0','0','0','35334','toc herald SAY_HORDE_WARRIOR'),
('-1650002','Coming out of the gate is Eressea Dawnsinger, skilled mage and Grand Champion of Silvermoon!','0','0','0','0','35338','toc herald SAY_HORDE_MAGE'),
('-1650003','Tall in the saddle of his kodo, here is the venerable Runok Wildmane, Grand Champion of Thunder Bluff!','0','0','0','0','35336','toc herald SAY_HORDE_SHAMAN'),
('-1650004','Entering the arena is the lean and dangerous Zul''tore, Grand Champion of Sen''jin!','0','0','0','0','35335','toc herald SAY_HORDE_HUNTER'),
('-1650005','Representing the tenacity of the Forsaken, here is the Grand Champion of the Undercity, Deathstalker Visceri!','0','0','0','0','35337','toc herald SAY_HORDE_ROGUE'),

('-1650006','The Sunreavers are proud to present their representatives in this trial by combat.','0','1','0','396','35260','toc herald SAY_ALLIANCE_CHALLENGE'),
('-1650007','Proud and strong, give a cheer for Marshal Jacob Alerius, the Grand Champion of Stormwind!','0','0','0','0','35245','toc herald SAY_ALLIANCE_WARRIOR'),
('-1650008','Here comes the small but deadly Ambrose Boltspark, Grand Champion of Gnomeregan!','0','0','0','0','35248','toc herald SAY_ALLIANCE_MAGE'),
('-1650009','Coming out of the gate is Colosos, the towering Grand Champion of the Exodar!','0','0','0','0','35247','toc herald SAY_ALLIANCE_SHAMAN'),
('-1650010','Entering the arena is the Grand Champion of Darnassus, the skilled sentinel Jaelyne Evensong!','0','0','0','0','35249','toc herald SAY_ALLIANCE_HUNTER'),
('-1650011','The might of the dwarves is represented today by the Grand Champion of Ironforge, Lana Stouthammer!','0','0','0','0','35246','toc herald SAY_ALLIANCE_ROGUE'),

('-1650012','Welcome, champions. Today, before the eyes of your leaders and peers, you will prove yourselves worthy combatants.','0','1','0','1','35321','tirion SAY_TIRION_WELCOME'),
('-1650013','You will first be facing three of the Grand Champions of the Tournament! These fierce contenders have beaten out all others to reach the pinnacle of skill in the joust.','0','1','0','1','35330','tirion SAY_TIRION_FIRST_CHALLENGE'),
('-1650014','Fight well, Horde! Lok''tar Ogar!','0','1','0','22','35326','thrall SAY_THRALL_ALLIANCE_CHALLENGE'),
('-1650015','Finally, a fight worth watching.','0','1','0','396','35327','garrosh SAY_GARROSH_ALLIANCE_CHALLENGE'),
('-1650016','I have no taste for these games, Tirion. Still... I trust they will perform admirably.','0','1','0','1','35322','king varian SAY_VARIAN_HORDE_CHALLENGE'),
('-1650017','Begin!','0','1','0','0','35331','tirion SAY_TIRION_CHAMPIONS_BEGIN'),
('-1650018','The blood elves of Silvermoon cheer for $n.','0','2','0','0','35143','raid spectator EMOTE_BLOOD_ELVES'),
('-1650019','The trolls of the Sen''jin Village begin a chant to celebrate $n.','0','2','0','0','35151','raid spectator EMOTE_TROLLS'),
('-1650020','The tauren of Thunder Bluff cheer for $n.','0','2','0','0','35150','raid spectator EMOTE_TAUREN'),
('-1650021','The forsaken of the Undercity cheer for $n.','0','2','0','0','35152','raid spectator EMOTE_UNDEAD'),
('-1650022','The orcs of Orgrimmar cheer for $n.','0','2','0','0','35149','raid spectator EMOTE_ORCS'),
('-1650023','The dwarves of Ironforge begin a cheer for $n.','0','2','0','0','35145','raid spectator EMOTE_BLOOD_DWARVES'),
('-1650024','The gnomes of Gnomeregan cheer for $n.','0','2','0','0','35146','raid spectator EMOTE_GNOMES'),
('-1650025','The night elves of Darnassus cheer for $n.','0','2','0','0','35148','raid spectator EMOTE_NIGHT_ELVES'),
('-1650026','The humans of Stormwind cheer for $n.','0','2','0','0','35147','raid spectator EMOTE_HUMANS'),
('-1650027','The draenei of the Exodar cheer for $n.','0','2','0','0','35144','raid spectator EMOTE_DRAENEI'),

('-1650028','Well fought! Your next challenge comes from the Crusade''s own ranks. You will be tested against their considerable prowess.','0','1','0','0','35541','tirion SAY_TIRION_ARGENT_CHAMPION'),
('-1650029','You may begin!','0','1','0','22','35677','tirion SAY_TIRION_ARGENT_CHAMPION_BEGIN'),
('-1650030','Entering the arena, a paladin who is no stranger to the battlefield or tournament ground, the Grand Champion of the Argent Crusade, Eadric the Pure!','0','1','0','0','35542','toc herald SAY_EADRIC'),
('-1650031','The next combatant is second to none in her passion for upholding the Light. I give you Argent Confessor Paletress!','0','1','0','0','35543','toc herald SAY_PALETRESS'),
('-1650032','The Horde spectators cheer for $n.','0','2','0','0','35162','raid spectator EMOTE_HORDE_ARGENT_CHAMPION'),
('-1650033','The Alliance spectators cheer for $n.','0','2','0','0','35161','raid spectator EMOTE_ALLIANCE_ARGENT_CHAMPION'),
('-1650034','Are you up to the challenge? I will not hold back.','16134','0','0','397','35347','eadric SAY_EADRIC_INTRO'),
('-1650035','Thank you, good herald. Your words are too kind.','16245','0','0','2','35762','paletress SAY_PALETRESS_INTRO_1'),
('-1650036','May the Light give me strength to provide a worthy challenge.','16246','0','0','16','35763','paletress SAY_PALETRESS_INTRO_2'),

('-1650037','Well done. You have proven yourself today-','0','1','0','0','35544','tirion SAY_ARGENT_CHAMPION_COMPLETE'),
('-1650038','What''s that, up near the rafters?','0','0','0','25','35545','toc herald SAY_BLACK_KNIGHT_SPAWN'),
('-1650039','You spoiled my grand entrance, rat.','16256','0','0','0','35546','black knight SAY_BLACK_KNIGHT_INTRO_1'),
('-1650040','What is the meaning of this?','0','1','0','0','35547','tirion SAY_TIRION_BLACK_KNIGHT_INTRO_2'),
('-1650041','Did you honestly think an agent of the Lich King would be bested on the field of your pathetic little tournament?','16257','0','0','396','35548','black knight SAY_BLACK_KNIGHT_INTRO_3'),
('-1650042','I''ve come to finish my task.','16258','0','0','396','35549','black knight SAY_BLACK_KNIGHT_INTRO_4'),

('-1650043','My congratulations, champions. Through trials both planned and unexpected, you have triumphed.','0','1','0','0','35796','tirion SAY_EPILOG_1'),
('-1650044','Go now and rest; you''ve earned it.','0','1','0','0','35797','tirion SAY_EPILOG_2'),
('-1650045','You fought well.','0','1','0','66','35795','king varian SAY_VARIAN_EPILOG_3'),
('-1650046','Well done, Horde!','0','1','0','66','35794','thrall SAY_THRALL_HORDE_EPILOG_3'),

('-1650047','Tear him apart!','0','1','0','22','35551','garrosh SAY_GARROSH_OTHER_1'),
('-1650048','Garrosh, enough.','0','1','0','396','35325','thrall SAY_THRALL_OTHER_2'),
('-1650049','Admirably? Hah! I will enjoy watching your weak little champions fail, human.','0','1','0','22','35324','garrosh SAY_GARROSH_OTHER_3'),
('-1650050','Don''t just stand there; kill him!','0','1','0','22','35550','king varian SAY_VARIAN_OTHER_4'),
('-1650051','I did not come here to watch animals tear at each other senselessly, Tirion.','0','1','0','1','35328','king varian SAY_VARIAN_OTHER_5'),

('-1650052','Prepare yourselves!','16135','1','0','0','20905','eadric SAY_AGGRO'),
('-1650053','Hammer of the Righteous!','16136','1','0','0','35442','eadric SAY_HAMMER'),
('-1650054','You... You need more practice.','16137','1','0','0','35759','eadric SAY_KILL_1'),
('-1650055','Nay! Nay! And I say yet again nay! Not good enough!','16138','1','0','0','35760','eadric SAY_KILL_2'),
('-1650056','I yield! I submit. Excellent work. May I run away now?','16139','1','0','0','35761','eadric SAY_DEFEAT'),
('-1650057','%s begins to radiate light. Shield your eyes!','0','3','0','0','35415','eadric EMOTE_RADIATE'),
('-1650058','%s targets $N with the Hammer of the Righteous!','0','3','0','0','35408','eadric EMOTE_HAMMER'),

('-1650059','Well then, let us begin.','16247','1','0','0','0','paletress SAY_AGGRO'),
('-1650060','Take this time to consider your past deeds.','16248','1','0','0','35764','paletress SAY_MEMORY'),
('-1650061','Even the darkest memory fades when confronted.','16249','1','0','0','35231','paletress SAY_MEMORY_DIES'),
('-1650062','Take your rest.','16250','1','0','0','35765','paletress SAY_KILL_1'),
('-1650063','Be at ease.','16251','1','0','0','35766','paletress SAY_KILL_2'),
('-1650064','Excellent work!','16252','1','0','0','18048','paletress SAY_DEFEAT'),

('-1650065','This farce ends here!','16259','1','0','0','35767','black knight SAY_AGGRO'),
('-1650066','My rotting flesh was just getting in the way!','16262','1','0','0','35771','black knight SAY_PHASE_2'),
('-1650067','I have no need for bones to best you!','16263','1','0','0','35772','black knight SAY_PHASE_3'),
('-1650068','A waste of flesh.','16260','1','0','0','35769','black knight SAY_KILL_1'),
('-1650069','Pathetic.','16261','1','0','0','7234','black knight SAY_KILL_2'),
('-1650070','No! I must not fail... again...','16264','1','0','0','35770','black knight SAY_DEATH');

-- -1 658 000 ICC: PIT OF SARON
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES

-- REUSE -1658001 to -1658013

('-1658014','Tiny creatures under feet, you bring Garfrost something good to eat!','16912','1','0','0','37218','garfrost SAY_AGGRO'),
('-1658015','Will save for snack. For later.','16913','1','0','0','37219','garfrost SAY_SLAY_1'),
('-1658016','That one maybe not so good to eat now. Stupid Garfrost! BAD! BAD!','16914','1','0','0','37220','garfrost SAY_BOULDER_HIT'),
('-1658017','Garfrost hope giant underpants clean. Save boss great shame. For later.','16915','1','0','0','37221','garfrost SAY_DEATH'),
('-1658018','Axe too weak. Garfrost make better and CRUSH YOU!','16916','1','0','0','36445','garfrost SAY_FORGE_1'),
('-1658019','That one maybe not so good to eat now. Stupid Garfrost! BAD! BAD!','16917','1','0','0','37220','garfrost SAY_FORGE_2'),
('-1658020','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1658021','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1658022','%s hurls a massive saronite boulder at you!','0','5','0','0','37438','garfrost EMOTE_THROW_SARONITE'),
('-1658023','%s casts Deep Freeze at $N.','0','3','0','0','37260','garfrost EMOTE_DEEP_FREEZE'),

('-1658024','Our work must not be interrupted! Ick! Take care of them!','16926','1','0','0','38502','krick SAY_AGGRO'),
('-1658025','Ooh...We could probably use these parts!','16927','1','0','0','37223','krick SAY_SLAY_1'),
('-1658026','Arms and legs are in short supply...Thanks for your contribution!','16928','1','0','0','38504','krick SAY_SLAY_2'),
('-1658027','Enough moving around! Hold still while I blow them all up!','16929','1','0','0','36538','krick SAY_ORDER_STOP'),
('-1658028','Quickly! Poison them all while they''re still close!','16930','1','0','0','36537','krick SAY_ORDER_BLOW'),
('-1658029','No! That one! That one! Get that one!','16931','1','0','0','38505','krick SAY_TARGET_1'),
('-1658030','I''ve changed my mind...go get that one instead!','16932','1','0','0','38506','krick SAY_TARGET_2'),
('-1658031','What are you attacking him for? The dangerous one is over there,fool!','16933','1','0','0','37226','krick SAY_TARGET_3'),
('-1658032','%s begins rapidly conjuring explosive mines!','0','3','0','0','36550','krick EMOTE_KRICK_MINES'),
('-1658033','%s begins to unleash a toxic poison cloud!','0','3','0','0','36531','ick EMOTE_ICK_POISON'),
('-1658034','%s is chasing you!','0','5','0','0','36529','ick EMOTE_ICK_CHASING'),

-- REUSE -1658035 to -1658046

('-1658047','Your pursuit shall be in vain, adventurers, for the Lich King has placed an army of undead at my command! Behold!','16755','6','0','0','36708','tyrannus SAY_TYRANNUS_AMBUSH_1'),
('-1658048','Persistent whelps! You will not reach the entrance of my lord''s lair! Soldiers, destroy them!','16756','6','0','0','36709','tyrannus SAY_TYRANNUS_AMBUSH_2'),
('-1658049','Rimefang! Trap them within the tunnel! Bury them alive!','16757','6','0','0','36714','tyrannus SAY_GAUNTLET'),

('-1658050','Alas, brave, brave adventurers, your meddling has reached its end. Do you hear the clatter of bone and steel coming up the tunnel behind you? That is the sound of your impending demise.','16758','1','0','0','37233','tyrannus SAY_PREFIGHT_1'),
('-1658051','REUSE ME','0','0','0','0','0','REUSE ME'),
('-1658052','Ha, such an amusing gesture from the rabble. When I have finished with you, my master''s blade will feast upon your souls. Die!','16759','1','0','0','37234','tyrannus SAY_PREFIGHT_2'),
('-1658053','I shall not fail The Lich King! Come and meet your end!','16760','1','0','0','38718','tyrannus SAY_AGGRO'),
('-1658054','Such a shameful display...','16761','1','0','0','38715','tyrannus SAY_SLAY_1'),
('-1658055','Perhaps you should have stayed in the mountains!','16762','1','0','0','38716','tyrannus SAY_SLAY_2'),
('-1658056','Impossible! Rimefang...Warn...','16763','1','0','0','38714','tyrannus SAY_DEATH'),
('-1658057','Rimefang, destroy this fool!','16764','1','0','0','36648','tyrannus SAY_MARK'),
('-1658058','Power... overwhelming!','16765','1','0','0','38717','tyrannus SAY_SMASH'),
('-1658059','The frostwyrm %s gazes at $N and readies an icy attack!','0','3','0','0','36649','rimefang EMOTE_RIMEFANG_ICEBOLT'),
('-1658060','%s roars and swells with dark might!','0','3','0','0','36628','tyrannus EMOTE_SMASH');

-- -1 668 000 ICC: HALLS OF REFLECTION
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1668000','The master surveyed his kingdom and found it... Lacking. His judgement was swift and without mercy: DEATH TO ALL!','16738','1','0','0','37952','marwyn SAY_GAUNTLET'),

('-1668001','Your allies have arrived, Jaina, just as you promised. You will all become powerful agents of the Scourge.','17212','1','0','0','37172','lich king SAY_ALLY_INTRO_1'),
('-1668002','He is too powerful, we must leave this place at once! My magic will hold him in place for only a short time! Come quickly, heroes!','16644','1','0','1','36821','jaina SAY_ALLY_INTRO_2'),
('-1668003','I will not make the same mistake again, Sylvanas. This time there will be no escape. You will all serve me in death!','17213','1','0','0','37173','lich king SAY_HORDE_INTRO_1'),
('-1668004','He''s too powerful! Heroes, quickly, come to me! We must leave this place immediately! I will do what I can do hold him in place while we flee.','17058','1','0','1','37168','sylvanas SAY_HORDE_INTRO_2'),

-- REUSE -1668005 to -1668008

('-1668009','Halt! Do not carry that blade any further!','16675','1','0','1','37201','Uther - SAY_INTRO_QUELDELAR');

-- -1 724 000 RUBY SANCTUM
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1724000','Help! I am trapped within this tree! I require aid!','17490','6','0','0','40598','xerestraza SAY_HELP'),
('-1724001','Your power wanes, ancient one! Soon, you will join your friends!','17525','6','0','0','40614','baltharus SAY_INTRO'),

-- REUSE -1724002 to -1724009

('-1724010','Ah, the entertainment has arrived.','17520','1','0','0','39935','baltharus SAY_AGGRO'),
('-1724011','Baltharus leaves no survivors!','17521','1','0','0','39937','baltharus SAY_SLAY_1'),
('-1724012','This world has enough heroes.','17522','1','0','0','39938','baltharus SAY_SLAY_2'),
('-1724013','I... Didn''t see that coming...','17523','1','0','0','39936','baltharus SAY_DEATH'),
('-1724014','Twice the pain and half the fun.','17524','1','0','0','39939','baltharus SAY_SPLIT'),

('-1724015','You will suffer for this intrusion!','17528','1','0','0','39932','saviana SAY_AGGRO'),
('-1724016','As it should be...','17529','1','0','0','28383','saviana SAY_SLAY_1'),
('-1724017','Halion will be pleased.','17530','1','0','0','39934','saviana SAY_SLAY_2'),
('-1724018','Burn in the master''s flame!','17532','1','0','0','39925','saviana SAY_SPECIAL'),

('-1724019','Alexstrasza has chosen capable allies... A pity that I must END YOU!','17512','1','0','0','39781','zarithrian SAY_AGGRO'),
('-1724020','You thought you stood a chance?','17513','1','0','0','39782','zarithrian SAY_SLAY_1'),
('-1724021','It''s for the best.','17514','1','0','0','39783','zarithrian SAY_SLAY_2'),
('-1724022','HALION! I...','17515','1','0','0','39784','zarithrian SAY_DEATH'),
('-1724023','Turn them to ash, minions!','17516','1','0','0','39785','zarithrian SAY_SUMMON'),

('-1724024','Meddlesome insects! You''re too late: The Ruby Sanctum''s lost.','17499','6','0','0','39942','halion SAY_SPAWN'),
('-1724025','Your world teeters on the brink of annihilation. You will ALL bear witness to the coming of a new age of DESTRUCTION!','17500','1','0','0','40064','halion SAY_AGGRO'),
('-1724026','Another hero falls.','17501','1','0','0','0','halion SAY_SLAY'),
('-1724027','Relish this victory, mortals, for it will be your last! This world will burn with the master''s return!','17503','1','0','0','40065','halion SAY_DEATH'),
('-1724028','Not good enough.','17504','1','0','0','41619','halion SAY_BERSERK'),
('-1724029','The heavens burn!','17505','1','0','0','40068','halion SAY_FIREBALL'),
('-1724030','Beware the shadow!','17506','1','0','0','40069','halion SAY_SPHERES'),
('-1724031','You will find only suffering within the realm of twilight! Enter if you dare!','17507','1','0','0','40067','halion SAY_PHASE_2'),
('-1724032','I am the light and the darkness! Cower, mortals, before the herald of Deathwing!','17508','1','0','0','40072','halion SAY_PHASE_3'),
('-1724033','The orbiting spheres pulse with dark energy!','0','3','0','0','40071','halion EMOTE_SPHERES'),
('-1724034','Your efforts force %s further out of the twillight realm!','0','3','0','0','40227','halion EMOTE_OUT_OF_TWILLIGHT'),
('-1724035','Your efforts force %s further out of the physical realm!','0','3','0','0','40228','halion EMOTE_OUT_OF_PHYSICAL'),
('-1724036','Your companions'' efforts force %s further into the twilight realm!','0','3','0','0','40080','halion EMOTE_INTO_TWILLIGHT'),
('-1724037','Your companions'' efforts force %s further into the physical realm!','0','3','0','0','40079','halion EMOTE_INTO_PHYSICAL'),
('-1724038','Without pressure in both realms %s begins to regenerate.','0','3','0','0','40476','halion EMOTE_REGENERATE');


-- -1 999 900 EXAMPLE TEXT
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,broadcast_text_id,comment) VALUES
('-1999900','Let the games begin.','8280','1','0','0','35709','example_creature SAY_AGGRO'),
('-1999901','I see endless suffering. I see torment. I see rage. I see everything.','8831','1','0','0','13047','example_creature SAY_RANDOM_0'),
('-1999902','Muahahahaha','8818','1','0','0','75082','example_creature SAY_RANDOM_1'),
('-1999903','These mortal infedels my lord, they have invaded your sanctum and seek to steal your secrets.','8041','1','0','0','7661','example_creature SAY_RANDOM_2'),
('-1999904','You are already dead.','8581','1','0','0','0','example_creature SAY_RANDOM_3'),
('-1999905','Where to go? What to do? So many choices that all end in pain, end in death.','8791','1','0','0','13007','example_creature SAY_RANDOM_4'),
('-1999906','$N, I sentance you to death!','8588','1','0','0','11443','example_creature SAY_BESERK'),
('-1999907','The suffering has just begun!','0','1','0','0','0','example_creature SAY_PHASE'),
('-1999908','I always thought I was a good dancer.','0','0','0','0','0','example_creature SAY_DANCE'),
('-1999909','Move out Soldier!','0','0','0','0','0','example_creature SAY_SALUTE'),

('-1999910','Help $N! I''m under attack!','0','0','0','0','1629','example_escort SAY_AGGRO1'),
('-1999911','Die scum!','0','0','0','0','0','example_escort SAY_AGGRO2'),
('-1999912','Hmm a nice day for a walk alright','0','0','0','0','0','example_escort SAY_WP_1'),
('-1999913','Wild Felboar attack!','0','0','0','0','0','example_escort SAY_WP_2'),
('-1999914','Time for me to go! See ya around $N!','0','0','0','3','0','example_escort SAY_WP_3'),
('-1999915','Bye Bye!','0','0','0','3','0','example_escort SAY_WP_4'),
('-1999916','How dare you leave me like that! I hate you! =*(','0','3','0','0','0','example_escort SAY_DEATH_1'),
('-1999917','...no...how could you let me die $N','0','0','0','0','0','example_escort SAY_DEATH_2'),
('-1999918','ugh...','0','0','0','0','27890','example_escort SAY_DEATH_3'),
('-1999919','Taste death!','0','0','0','0','0','example_escort SAY_SPELL'),
('-1999920','Fireworks!','0','0','0','0','66661','example_escort SAY_RAND_1'),
('-1999921','Hmm, I think I could use a buff.','0','0','0','0','0','example_escort SAY_RAND_2'),

('-1999922','Normal select, guess you''re not interested.','0','0','0','0','0','example_gossip_codebox SAY_NOT_INTERESTED'),
('-1999923','Wrong!','0','0','0','0','0','example_gossip_codebox SAY_WRONG'),
('-1999924','You''re right, you are allowed to see my inner secrets.','0','0','0','0','0','example_gossip_codebox SAY_CORRECT'),

('-1999925','Hi!','0','0','0','0','29129','example_areatrigger SAY_HI');

--
-- GOSSIP TEXTS
--

--
-- Below contains data for table `gossip_texts`
-- valid entries for table are between -3000000 and -3999999
--

TRUNCATE gossip_texts;

-- -3 000 000 RESERVED (up to 100)
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3000000,'[PH] SD2 unknown text','GOSSIP_ID_UNKNOWN_TEXT');

-- -3 000 100 GENERAL MAPS (not instance maps)
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3000101,'Taruk send me to collect what you owe.','silvermoon harry GOSSIP_ITEM_GAMBLING_DEBT'),
(-3000102,'Pay up, Harry!','silvermoon harry GOSSIP_ITEM_PAYING'),
(-3000103,'I am ready to travel to you village now.','rainspeaker GOSSIP_ITEM_READY'),
(-3000104,'<Check for pulse...>','mosswalker victim GOSSIP_ITEM_PULSE'),
(-3000105,'Ezekiel said that you might have a certain book...','dirty larry GOSSIP_ITEM_BOOK'),
(-3000106,'Let Marshal Windsor know that I am ready.','squire rowe GOSSIP_ITEM_WINDSOR'),
(-3000107,'I am ready, as are my forces. Let us end this masquerade!','reginald windsor GOSSIP_ITEM_START'),
(-3000108,'I need a moment of your time, sir.','prospector anvilward GOSSIP_ITEM_MOMENT'),
(-3000109,'I am ready, Oronok. Let us destroy Cyrukh and free the elements!','oronok torn-heart GOSSIP_ITEM_FIGHT'),
(-3000110,'Why... yes, of course. I''ve something to show you right inside this building, Mr. Anvilward.','prospector anvilward GOSSIP_ITEM_SHOW'),
(-3000111,'I am ready, Anchorite. Let us begin the exorcism.','anchorite barada GOSSIP_ITEM_EXORCISM'),
(-3000112,'I''m ready - let''s get out of here.','injured goblin miner GOSSIP_ITEM_ESCORT_READY'),
(-3000113,'Go on, you''re free. Get out of here!','saronite mine slave GOSSIP_ITEM_SLAVE_FREE'),
(-3000114,'I''m ready to start the distillation, uh, Tipsy.','tipsy mcmanus GOSSIP_ITEM_START_DISTILLATION'),
(-3000115,'Turn the key to start the machine.','threshwackonator  GOSSIP_ITEM_TURN_KEY');

-- -3 033 000 SHADOWFANG KEEP
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3033000,'Please unlock the courtyard door.','deathstalker adamant/ sorcerer ashcrombe - GOSSIP_ITEM_DOOR');

-- -3 043 000 WAILING CAVERNS
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3043000,'Let the event begin!','Disciple of Naralex - GOSSIP_ITEM_BEGIN');

-- -3 090 000 GNOMEREGAN
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3090000,'I am ready to begin.','emi shortfuse GOSSIP_ITEM_START');

-- -3 230 000 BLACKROCK DEPTHS
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3230000,'You''re free, Dughal! Get out of here!','dughal GOSSIP_ITEM_DUGHAL'),
(-3230001,'Get out of here, Tobias, you''re free!','tobias GOSSIP_ITEM_TOBIAS'),
(-3230002,'Your bondage is at an end, Doom''rel. I challenge you!','doomrel GOSSIP_ITEM_CHALLENGE'),
(-3230003,'Why don''t you and Rocknot go find somewhere private...','nagmara GOSSIP_ITEM_NAGMARA');

-- -3 409 000 MOLTEN CORE
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3409000,'Tell me more.','majordomo_executus GOSSIP_ITEM_SUMMON_1'),
(-3409001,'What else do you have to say?','majordomo_executus GOSSIP_ITEM_SUMMON_2'),
(-3409002,'You challenged us and we have come. Where is this master you speak of?','majordomo_executus GOSSIP_ITEM_SUMMON_3');

-- -3 469 000 BLACKWING LAIR
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3469000,'I''ve made no mistakes.','victor_nefarius GOSSIP_ITEM_NEFARIUS_1'),
(-3469001,'You have lost your mind, Nefarius. You speak in riddles.','victor_nefarius GOSSIP_ITEM_NEFARIUS_2'),
(-3469002,'Please do.','victor_nefarius GOSSIP_ITEM_NEFARIUS_3'),

(-3469003,'I cannot, Vaelastrasz! Surely something can be done to heal you!','vaelastrasz GOSSIP_ITEM_VAEL_1'),
(-3469004,'Vaelastrasz, no!!!','vaelastrasz GOSSIP_ITEM_VAEL_2');

-- -3 509 000 RUINS OF AHN'QIRAJ
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3509000,'Let''s find out.','andorov GOSSIP_ITEM_START'),
(-3509001,'Let''s see what you have.','andorov GOSSIP_ITEM_TRADE');

-- -3 532 000 KARAZHAN
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3532000,'Teleport me to the Guardian''s Library','berthold GOSSIP_ITEM_TELEPORT'),
(-3532001,'I''m not an actor.','barnes GOSSIP_ITEM_OPERA_1'),
(-3532002,'Ok, I''ll give it a try, then.','barnes GOSSIP_ITEM_OPERA_2'),
(-3532003,'I''ve never been more ready.','barnes GOSSIP_ITEM_OPERA_JULIANNE_WIPE'),
(-3532004,'The wolf''s going down.','barnes GOSSIP_ITEM_OPERA_WOLF_WIPE'),
(-3532005,'Oh, grandmother, what phat lewts you have.','grandma GOSSIP_ITEM_GRANDMA_THIRD'),

(-3532006,'Control Orc Grunt','orc grunt GOSSIP_ITEM_ORC_GRUNT'),
(-3532007,'Control Orc Wolf','orc wolf GOSSIP_ITEM_ORC_WOLF'),
(-3532008,'Control Summoned Daemon','summoned deamon GOSSIP_ITEM_SUMMONED_DEAMON'),
(-3532009,'Control Orc Warlock','orc warlock GOSSIP_ITEM_ORC_WARLOCK'),
(-3532010,'Control Orc Necrolyte','orc necrolyte GOSSIP_ITEM_ORC_NECROLYTE'),
(-3532011,'Control Warchief Blackhand','warchief blackhand GOSSIP_ITEM_WARCHIEF_BLACKHAND'),
(-3532012,'Control Human Footman','human footman GOSSIP_ITEM_HUMAN_FOOTMAN'),
(-3532013,'Control Human Charger','human charger GOSSIP_ITEM_HUMAN_CHARGER'),
(-3532014,'Control Conjured Water Elemental','conjured water elemental GOSSIP_ITEM_WATER_ELEMENTAL'),
(-3532015,'Control Human Conjurer','human conjurer GOSSIP_ITEM_HUMAN_CONJURER'),
(-3532016,'Control Human Cleric','human cleric GOSSIP_ITEM_HUMAN_CLERIC'),
(-3532017,'Control King Llane','king llane GOSSIP_ITEM_KING_LLANE'),
(-3532018,'Please reset the chess board, we would like to play again.','medivh GOSSIP_ITEM_RESET_BOARD'),
(-3532019,'I''ll nail it!','barnes GOSSIP_ITEM_OPERA_OZ_WIPE'),
(-3532020,'Oh, grandmother, what big ears you have.','grandma GOSSIP_ITEM_GRANDMA_FIRST'),
(-3532021,'Oh, grandmother, what big eyes you have.','grandma GOSSIP_ITEM_GRANDMA_SECOND');

-- -3 534 000 THE BATTLE OF MT. HYJAL
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3534000,'My companions and I are with you, Lady Proudmoore.','jaina GOSSIP_ITEM_JAIN_START'),
(-3534001,'We are ready for whatever Archimonde might send our way, Lady Proudmoore.','jaina GOSSIP_ITEM_ANATHERON'),
(-3534002,'Until we meet again, Lady Proudmoore.','jaina GOSSIP_ITEM_SUCCESS'),
(-3534003,'I am with you, Thrall.','thrall GOSSIP_ITEM_THRALL_START'),
(-3534004,'We have nothing to fear.','thrall GOSSIP_ITEM_AZGALOR'),
(-3534005,'Until we meet again, Thrall.','thrall GOSSIP_ITEM_SUCCESS'),
(-3534006,'I would be grateful for any aid you can provide, Priestess.','tyrande GOSSIP_ITEM_AID');

-- -3 560 000 ESCAPE FROM DURNHOLDE (OLD HILLSBRAD)
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3560000,'We are ready to get you out of here, Thrall. Let''s go!','thrall GOSSIP_ITEM_START'),
(-3560001,'I need a pack of Incendiary Bombs.','erozion GOSSIP_ITEM_NEED_BOMBS'),
(-3560002,'Taretha cannot see you, Thrall.','thrall GOSSIP_ITEM_SKARLOC1'),
(-3560003,'The situation is rather complicated, Thrall. It would be best for you to head into the mountains now, before more of Blackmoore''s men show up. We''ll make sure Taretha is safe.','thrall GOSSIP_ITEM_SKARLOC2'),
(-3560004,'We''re ready, Thrall.','thrall GOSSIP_ITEM_TARREN_2'),
(-3560005,'Strange wizard?','taretha GOSSIP_ITEM_EPOCH1'),
(-3560006,'We''ll get you out. Taretha. Don''t worry. I doubt the wizard would wander too far away.','taretha GOSSIP_ITEM_EPOCH2'),
(-3560007,'Tarren Mill.','thrall GOSSIP_ITEM_TARREN_1');

-- -3 564 000 BLACK TEMPLE
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3564000,'I''m with you, Akama.','akama(shade) GOSSIP_ITEM_START_ENCOUNTER'),
(-3564001,'I''m ready, Akama.','akama(illidan) GOSSIP_ITEM_PREPARE'),
(-3564002,'We''re ready to face Illidan.','akama(illidan) GOSSIP_ITEM_START_EVENT');

-- -3 568 000 ZUL'AMAN
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3568000,'Thanks for the concern, but we intend to explore Zul''Aman.','harrison jones GOSSIP_ITEM_BEGIN'),
(-3568001,'The coast is clear. You''re free!','harkor GOSSIP_ITEM_ID_HARKOR_FREE'),
(-3568002,'So, how does a dwarf like you end up in a place like this?','harkor GOSSIP_ITEM_ID_HARKOR_DONE'),
(-3568003,'Nalorakk is dead, you''re free to go.','tanzar GOSSIP_ITEM_ID_TANZAR_FREE'),
(-3568004,'You''re welcome... Now tell us what''s going on here!','tanzar GOSSIP_ITEM_ID_TANZAR_DONE'),
(-3568005,'What can you tell us about Budd?','tanzar GOSSIP_ITEM_ID_TANZAR_DONE2'),
(-3568006,'We''ve killed your captors. You''re free to go.','kraz GOSSIP_ITEM_ID_KRAZ_FREE'),
(-3568007,'What happened to you, orc?','kraz GOSSIP_ITEM_ID_KRAZ_DONE'),
(-3568008,'It''s safe, little gnome. You can come out now.','ashli GOSSIP_ITEM_ID_ASHLI_FREE'),
(-3568009,'How''d a perky little gnome like you get caught up in a mess like this?','ashli GOSSIP_ITEM_ID_ASHLI_DONE');

-- -3 595 000 CULLING OF STRATHOLME
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3595000,'What do you think they''re up to?','chromie GOSSIP_ITEM_ENTRANCE_1'),
(-3595001,'You want me to do what?','chromie GOSSIP_ITEM_ENTRANCE_2'),
(-3595002,'Very well, Chromie.','chromie GOSSIP_ITEM_ENTRANCE_3'),
(-3595003,'Why have I been sent back to this particular place and time?','chromie GOSSIP_ITEM_INN_1'),
(-3595004,'What was this decision?','chromie GOSSIP_ITEM_INN_2'),
(-3595005,'So how does the Infinite Dragonflight plan to interfere?','chromie GOSSIP_ITEM_INN_3'),
(-3595006,'Chromie, you and I both know what''s going to happen in this time stream. We''ve seen this all before. Can you just skip us ahead to all the real action?','chromie GOSSIP_ITEM_INN_SKIP'),
(-3595007,'Yes, please!','chromie GOSSIP_ITEM_INN_TELEPORT'),
(-3595008,'Yes, my Prince. We are ready.','arthas GOSSIP_ITEM_CITY_GATES'),
(-3595009,'We''re only doing what is best for Lordaeron, your Highness.','arthas GOSSIP_ITEM_TOWN_HALL'),
(-3595010,'Lead the way, Prince Arthas','arthas GOSSIP_ITEM_TOWN_HALL_2'),
(-3595011,'I''m ready.','arthas GOSSIP_ITEM_EPOCH'),
(-3595012,'For Lordaeron!','arthas GOSSIP_ITEM_ESCORT'),
(-3595013,'I''m ready to battle the dreadlord, sire.','arthas GOSSIP_ITEM_DREADLORD');

-- -3 599 000 HALLS OF STONE
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3599000,'Brann, it would be our honor!','brann GOSSIP_ITEM_ID_START'),
(-3599001,'Let''s move Brann, enough of the history lessons!','brann GOSSIP_ITEM_ID_PROGRESS'),
(-3599002,'There will be plenty of time for this later Brann, we need to get moving!','brann GOSSIP_ITEM_ID_END_TRIBUNAL'),
(-3599003,'We''re with you Brann! Open it!','brann GOSSIP_ITEM_ID_START_SJONNIR');

-- -3 603 000 ULDUAR
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3603000,'Teleport to the Expedition Base Camp.','GOSSIP_ITEM_TELE_BASE_CAMP'),
(-3603001,'Teleport to the Formation Grounds.','GOSSIP_ITEM_TELE_FORMATION_GROUNDS'),
(-3603002,'Teleport to the Colossal Forge.','GOSSIP_ITEM_TELE_COLOSSAR_FORGE'),
(-3603003,'Teleport to the Scrapyard.','GOSSIP_ITEM_TELE_SCRAPYARD'),
(-3603004,'Teleport to the Antechamber of Ulduar.','GOSSIP_ITEM_TELE_ANTECHAMBER'),
(-3603005,'Teleport to the Shattered Walkway.','GOSSIP_ITEM_TELE_WALKWAY'),
(-3603006,'Teleport to the Conservatory of Life.','GOSSIP_ITEM_TELE_CONSERVATORY'),
(-3603007,'Teleport to the Spark of Imagination.','GOSSIP_ITEM_TELE_SPARK_IMAGINATION'),
(-3603008,'Teleport to the Prison of Yogg-Saron.','GOSSIP_ITEM_TELE_YOGG_SARON'),

(-3603009,'We are ready to help!','Expedition Commander GOSSIP_ITEM_START_RAZORSCALE'),
(-3603010,'Activate secondary defensive systems.','Lore Keeper of Norgannon GOSSIP_ITEM_ACTIVATE_SYSTEMS'),
(-3603011,'Confirmed.','Lore Keeper of Norgannon GOSSIP_ITEM_CONFIRMED'),
(-3603012,'We''re ready. Begin the assault!','Brann Bronzebeard GOSSIP_ITEM_BEGIN_ASSAULT'),

(-3603013,'Lend us your aid, keeper. Together we will defeat Yogg-Saron.','Ulduar Keeper GOSSIP_ITEM_LEND_AID'),
(-3603014,'Yes.','Ulduar Keeper GOSSIP_ITEM_KEEPER_CONFIRM');

-- -3 608 000 VIOLET HOLD
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3608000,'Activate the crystals when we get in trouble, right?','sinclari GOSSIP_ITEM_INTRO'),
(-3608001,'Get your people to safety, we''ll keep the Blue Dragonflight''s forces at bay.','sinclari GOSSIP_ITEM_START'),
(-3608002,'I''m not fighting, so send me in now!','sinclari GOSSIP_ITEM_TELEPORT');

-- -3 609 000 EBON HOLD (DK START)
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3609000,'I challenge you, death knight!','Death Knight Initiate GOSSIP_ITEM_ACCEPT_DUEL'),
(-3609001,'I am ready, Highlord. Let the siege of Light''s Hope begin!','Highlord Darion Mograine GOSSIP_ITEM_READY');

-- -3 631 000 ICECROWN CITADEL
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3631000,'Teleport to the Light''s Hammer.','GOSSIP_ITEM_TELE_LIGHTS_HAMMER'),
(-3631001,'Teleport to the Oratory of the Damned.','GOSSIP_ITEM_TELE_ORATORY_DAMNED'),
(-3631002,'Teleport to the Rampart of Skulls.','GOSSIP_ITEM_TELE_RAMPART_OF_SKULLS'),
(-3631003,'Teleport to the Deathbringer''s Rise.','GOSSIP_ITEM_TELE_DEATHBRINGERS_RISE'),
(-3631004,'Teleport to the Upper Spire.','GOSSIP_ITEM_TELE_UPPER_SPIRE'),
(-3631005,'Teleport to the Sindragosa''s Lair.','GOSSIP_ITEM_TELE_SINDRAGOSAS_LAIR'),
(-3631006,'My companions are all accounted for, Saurfang. Let''s go!','GOSSIP_ITEM_GUNSHIP_START_HORDE'),
(-3631007,'My companions are all accounted for, Muradin. Let''s go!','GOSSIP_ITEM_GUNSHIP_START_ALLIANCE');

-- -3 649 000 TRIAL OF CRUSADER
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3649000,'Yes. We are prepared for the challenges ahead of us.','barrett GOSSIP_ITEM_BEAST_INIT'),
(-3649001,'Bring forth the first challenge!','barrett GOSSIP_ITEM_BEAST_START'),
(-3649002,'We want another shot at those beasts!','barrett GOSSIP_ITEM_BEAST_WIPE_INIT'),
(-3649003,'What new challenge awaits us?','barrett GOSSIP_ITEM_JARAXXUS_INIT'),
(-3649004,'We''re ready to fight the sorceror again.','barrett GOSSIP_ITEM_JARAXXUS_WIPE_INIT'),
(-3649005,'Of course!','barrett GOSSIP_ITEM_PVP_INIT'),
(-3649006,'Give the signal! We''re ready to go!','barrett GOSSIP_ITEM_PVP_START'),
(-3649007,'That tough, huh?','barrett GOSSIP_ITEM_TWINS_INIT'),
(-3649008,'Val''kyr? We''re ready for them','barrett GOSSIP_ITEM_TWINS_START'),
(-3649009,'Your words of praise are appreciated, Coliseum Master.','barrett GOSSIP_ITEM_ANUB_INIT'),
(-3649010,'That is strange...','barrett GOSSIP_ITEM_ANUB_START'),
(-3649011,'We''re ready for the next challenge.','barrett GOSSIP_ITEM_JARAXXUS_START'),
(-3649012,'You''ll be even more amazed after we take them out!','barrett GOSSIP_ITEM_PVP_WIPE_INIT'),
(-3649013,'We''re ready for anything!','barrett GOSSIP_ITEM_PVP_WIPE_START'),
(-3649014,'We''re ready. This time things will be different.','barrett GOSSIP_ITEM_BEAST_WIPE_START'),
(-3649015,'Now.','barrett GOSSIP_ITEM_JARAXXUS_WIPE_START'),
(-3649016,'We''ll just have to improve our teamwork to match the two of them.','barrett GOSSIP_ITEM_TWINS_WIPE_INIT'),
(-3649017,'Just bring them out again, then watch.','barrett GOSSIP_ITEM_TWINS_WIPE_START');

-- -3 650 000 TRIAL OF THE CHAMPION
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3650000,'I am ready.','herald GOSSIP_ITEM_READY'),
(-3650001,'I am ready.  However, I''d like to skip the pageantry.','herald GOSSIP_ITEM_READY_SKIP_INTRO'),
(-3650002,'I am ready for the next challenge.','herald GOSSIP_ITEM_READY_NEXT_CHALLENGE');

--
-- Below just for beautiful view in table, run at own desire
--

-- ALTER TABLE script_texts ORDER BY entry desc;
-- ALTER TABLE gossip_texts ORDER BY entry desc;

--
-- Below contains all waypoints used by escortAI scripts
-- Entry is entry == creature_template.entry
--

TRUNCATE script_waypoint;
INSERT INTO script_waypoint (Entry, PathId, Point, PositionX, PositionY, PositionZ, Orientation, WaitTime, ScriptId, Comment) VALUES
(349,0,1,-8769.59,-2185.73,141.975,0,0,0,''),
(349,0,2,-8776.54,-2193.78,140.96,0,0,0,''),
(349,0,3,-8783.29,-2194.82,140.462,0,0,0,''),
(349,0,4,-8792.52,-2188.8,142.078,0,0,0,''),
(349,0,5,-8807.55,-2186.1,141.504,0,0,0,''),
(349,0,6,-8818,-2184.8,139.153,0,0,0,''),
(349,0,7,-8825.81,-2188.84,138.459,0,0,0,''),
(349,0,8,-8827.52,-2199.81,139.622,0,0,0,''),
(349,0,9,-8821.14,-2212.64,143.126,0,0,0,''),
(349,0,10,-8809.18,-2230.46,143.438,0,0,0,''),
(349,0,11,-8797.04,-2240.72,146.548,0,0,0,''),
(349,0,12,-8795.24,-2251.81,146.808,0,0,0,''),
(349,0,13,-8780.16,-2258.62,148.554,0,0,0,''),
(349,0,14,-8762.65,-2259.56,151.144,0,0,0,''),
(349,0,15,-8754.36,-2253.74,152.243,0,0,0,''),
(349,0,16,-8741.87,-2251,154.486,0,0,0,''),
(349,0,17,-8733.22,-2251.01,154.36,0,0,0,''),
(349,0,18,-8717.47,-2245.04,154.686,0,0,0,''),
(349,0,19,-8712.24,-2246.83,154.709,0,0,0,''),
(349,0,20,-8693.84,-2240.41,152.91,0,0,0,''),
(349,0,21,-8681.82,-2245.33,155.518,0,0,0,''),
(349,0,22,-8669.86,-2252.77,154.854,0,0,0,''),
(349,0,23,-8670.56,-2264.69,156.978,0,0,0,''),
(349,0,24,-8676.56,-2269.2,155.411,0,0,0,''),
(349,0,25,-8673.34,-2288.65,157.054,0,0,0,''),
(349,0,26,-8677.76,-2302.56,155.917,0,16000,0,'Corp. Keeshan - Short Break Outside'),
(349,0,27,-8682.46,-2321.69,155.917,0,0,0,''),
(349,0,28,-8690.4,-2331.78,155.971,0,0,0,''),
(349,0,29,-8715.1,-2353.95,156.188,0,0,0,''),
(349,0,30,-8748.04,-2370.7,157.988,0,0,0,''),
(349,0,31,-8780.9,-2421.37,156.109,0,0,0,''),
(349,0,32,-8792.01,-2453.38,142.746,0,0,0,''),
(349,0,33,-8804.78,-2472.43,134.192,0,0,0,''),
(349,0,34,-8841.35,-2503.63,132.276,0,0,0,''),
(349,0,35,-8867.57,-2529.89,134.739,0,0,0,''),
(349,0,36,-8870.67,-2542.08,131.044,0,0,0,''),
(349,0,37,-8922.05,-2585.31,132.446,0,0,0,''),
(349,0,38,-8949.08,-2596.87,132.537,0,0,0,''),
(349,0,39,-8993.46,-2604.04,130.756,0,0,0,''),
(349,0,40,-9006.71,-2598.47,127.966,0,0,0,''),
(349,0,41,-9038.96,-2572.71,124.748,0,0,0,''),
(349,0,42,-9046.92,-2560.64,124.447,0,0,0,''),
(349,0,43,-9066.69,-2546.63,123.11,0,0,0,''),
(349,0,44,-9077.54,-2541.67,121.17,0,0,0,''),
(349,0,45,-9125.32,-2490.06,116.057,0,0,0,''),
(349,0,46,-9145.06,-2442.24,108.232,0,0,0,''),
(349,0,47,-9158.2,-2425.36,105.5,0,0,0,''),
(349,0,48,-9151.92,-2393.67,100.856,0,0,0,''),
(349,0,49,-9165.19,-2376.03,94.8215,0,0,0,''),
(349,0,50,-9187.1,-2360.52,89.9231,0,0,0,''),
(349,0,51,-9235.44,-2305.24,77.9253,0,0,0,''),
(349,0,52,-9264.73,-2292.92,70.0089,0,0,0,''),
(349,0,53,-9277.47,-2296.19,68.0896,0,2500,0,'Corp. Keeshan - quest-finish'),
(349,0,54,-9277.47,-2296.19,68.0896,0,0,0,'Corp. Keeshan - Say Goodbye'),
(467,0,1,-10508.4,1068,55.21,0,0,0,''),
(467,0,2,-10518.3,1074.84,53.96,0,0,0,''),
(467,0,3,-10534.8,1081.92,49.88,0,0,0,''),
(467,0,4,-10546.5,1084.88,50.13,0,0,0,''),
(467,0,5,-10555.3,1084.45,45.75,0,0,0,''),
(467,0,6,-10566.6,1083.53,42.1,0,0,0,''),
(467,0,7,-10575.8,1082.34,39.46,0,0,0,''),
(467,0,8,-10585.7,1081.08,37.77,0,0,0,''),
(467,0,9,-10600.1,1078.19,36.23,0,0,0,''),
(467,0,10,-10608.7,1076.08,35.88,0,0,0,''),
(467,0,11,-10621.3,1073,35.4,0,0,0,''),
(467,0,12,-10638.1,1060.18,33.61,0,0,0,''),
(467,0,13,-10655.9,1038.99,33.48,0,0,0,''),
(467,0,14,-10664.7,1030.54,32.7,0,0,0,''),
(467,0,15,-10708.7,1033.86,33.32,0,0,0,''),
(467,0,16,-10754.4,1017.93,32.79,0,0,0,''),
(467,0,17,-10802.3,1018.01,32.16,0,0,0,''),
(467,0,18,-10832.6,1009.04,32.71,0,0,0,''),
(467,0,19,-10866.6,1006.51,31.71,0,0,0,''),
(467,0,20,-10880,1005.1,32.84,0,0,0,''),
(467,0,21,-10892.5,1001.32,34.46,0,0,0,''),
(467,0,22,-10906.1,997.11,36.15,0,0,0,''),
(467,0,23,-10922.3,1002.23,35.74,0,0,0,''),
(467,0,24,-10936.3,1023.38,36.52,0,0,0,''),
(467,0,25,-10933.3,1052.61,35.85,0,0,0,''),
(467,0,26,-10940.2,1077.66,36.49,0,0,0,''),
(467,0,27,-10957.1,1099.33,36.83,0,0,0,''),
(467,0,28,-10956.5,1119.9,36.73,0,0,0,''),
(467,0,29,-10939.3,1150.75,37.42,0,0,0,''),
(467,0,30,-10915.1,1202.09,36.55,0,0,0,''),
(467,0,31,-10892.6,1257.03,33.37,0,0,0,''),
(467,0,32,-10891.9,1306.66,35.45,0,0,0,''),
(467,0,33,-10896.2,1327.86,37.77,0,0,0,''),
(467,0,34,-10906,1368.05,40.91,0,0,0,''),
(467,0,35,-10910.2,1389.33,42.62,0,0,0,''),
(467,0,36,-10915.4,1417.72,42.93,0,0,0,''),
(467,0,37,-10926.4,1421.18,43.04,0,0,0,'walk here and say'),
(467,0,38,-10952.3,1421.74,43.4,0,0,0,''),
(467,0,39,-10980,1411.38,42.79,0,0,0,''),
(467,0,40,-11006.1,1420.47,43.26,0,0,0,''),
(467,0,41,-11022,1450.59,43.09,0,0,0,''),
(467,0,42,-11025.4,1491.59,43.15,0,0,0,''),
(467,0,43,-11036.1,1508.32,43.28,0,0,0,''),
(467,0,44,-11060.7,1526.72,43.19,0,0,0,''),
(467,0,45,-11072.8,1527.77,43.2,3.22,5000,0,'say and quest credit'),
(1379,0,1,-5751.12,-3441.01,301.743,0,0,0,''),
(1379,0,2,-5738.58,-3485.14,302.41,0,0,0,''),
(1379,0,3,-5721.62,-3507.85,304.011,0,0,0,''),
(1379,0,4,-5710.21,-3527.97,304.708,0,0,0,''),
(1379,0,5,-5706.92,-3542.89,304.871,0,0,0,''),
(1379,0,6,-5701.53,-3551.24,305.962,0,0,0,''),
(1379,0,7,-5699.53,-3555.69,306.505,0,0,0,''),
(1379,0,8,-5690.56,-3571.98,309.035,0,0,0,''),
(1379,0,9,-5678.61,-3587.17,310.607,0,0,0,''),
(1379,0,10,-5677.05,-3594.35,311.527,0,0,0,''),
(1379,0,11,-5674.39,-3605.19,312.239,0,0,0,''),
(1379,0,12,-5674.45,-3614.39,312.337,0,0,0,''),
(1379,0,13,-5673.05,-3630.56,311.105,0,0,0,''),
(1379,0,14,-5680.34,-3645.44,315.185,0,0,0,''),
(1379,0,15,-5684.46,-3650.05,314.687,0,0,0,''),
(1379,0,16,-5693.9,-3674.14,313.03,0,0,0,''),
(1379,0,17,-5701.43,-3712.54,313.959,0,0,0,''),
(1379,0,18,-5698.79,-3720.88,316.943,0,0,0,''),
(1379,0,19,-5699.95,-3733.63,318.597,0,0,0,'Protecting the Shipment - Ambush'),
(1379,0,20,-5698.61,-3754.74,322.047,0,0,0,''),
(1379,0,21,-5688.68,-3769,323.957,0,0,0,''),
(1379,0,22,-5688.14,-3782.65,322.667,0,0,0,''),
(1379,0,23,-5699.23,-3792.65,322.448,0,30000,0,'Protecting the Shipment - End'),
(1379,0,24,-5700.8,-3792.78,322.588,0,0,5000,''),
(1840,0,1,2689.68,-1937.47,72.14,0,0,0,''),
(1840,0,2,2683.11,-1926.82,72.14,0,0,0,''),
(1840,0,3,2678.73,-1919.42,68.86,2.11887,1000,0,'escort paused'),
(1842,0,1,2941.75,-1391.82,167.237,0,0,0,'SAY_ESCORT_START'),
(1842,0,2,2940.56,-1393.64,165.943,0,0,0,''),
(1842,0,3,2932.19,-1410.66,165.943,0,0,0,''),
(1842,0,4,2921.81,-1405.09,165.943,0,0,0,''),
(1842,0,5,2916.48,-1402.58,165.943,0,0,0,''),
(1842,0,6,2918.52,-1398.12,165.943,0,0,0,''),
(1842,0,7,2922.8,-1389.49,160.842,0,0,0,''),
(1842,0,8,2924.93,-1385.65,160.842,0,0,0,''),
(1842,0,9,2930.93,-1388.65,160.842,0,0,0,''),
(1842,0,10,2946.7,-1396.65,160.842,0,0,0,''),
(1842,0,11,2948.72,-1392.79,160.842,0,0,0,''),
(1842,0,12,2951.98,-1386.62,155.948,0,0,0,''),
(1842,0,13,2953.84,-1383.33,155.948,0,0,0,''),
(1842,0,14,2951.19,-1381.74,155.948,0,0,0,''),
(1842,0,15,2946.68,-1379.29,152.02,0,0,0,''),
(1842,0,16,2942.79,-1377.66,152.02,0,0,0,''),
(1842,0,17,2935.49,-1392.52,152.02,0,0,0,''),
(1842,0,18,2921.17,-1384.8,152.02,0,0,0,''),
(1842,0,19,2915.33,-1395.35,152.02,0,0,0,''),
(1842,0,20,2926.25,-1401.26,152.028,0,0,0,''),
(1842,0,21,2930.32,-1403.48,150.521,0,0,0,''),
(1842,0,22,2933.94,-1405.36,150.521,0,0,0,''),
(1842,0,23,2929.22,-1415.79,150.504,0,0,0,''),
(1842,0,24,2921.17,-1431.68,150.781,0,0,0,''),
(1842,0,25,2917.47,-1438.78,150.781,0,0,0,''),
(1842,0,26,2913.05,-1453.52,148.098,4.81279,1000,0,'SAY_TAELAN_MOUNT'),
(1842,0,27,2913.83,-1474.93,146.224,0,0,0,''),
(1842,0,28,2906.81,-1487.06,146.224,0,0,0,''),
(1842,0,29,2900.64,-1496.57,146.306,0,0,0,''),
(1842,0,30,2885.25,-1501.58,146.02,0,0,0,''),
(1842,0,31,2863.88,-1500.38,146.681,0,0,0,''),
(1842,0,32,2846.51,-1487.18,146.332,0,0,0,''),
(1842,0,33,2823.75,-1490.99,145.782,0,0,0,''),
(1842,0,34,2800.98,-1510.91,145.049,0,0,0,''),
(1842,0,35,2789.49,-1525.21,143.729,0,0,0,''),
(1842,0,36,2776.96,-1542.31,139.435,0,0,0,''),
(1842,0,37,2762.03,-1561.8,133.763,0,0,0,''),
(1842,0,38,2758.74,-1569.6,131.514,0,0,0,''),
(1842,0,39,2765.49,-1588.79,129.721,0,0,0,''),
(1842,0,40,2779.61,-1613.12,129.132,0,0,0,''),
(1842,0,41,2757.65,-1638.03,128.236,0,0,0,''),
(1842,0,42,2741.31,-1659.79,126.457,0,0,0,''),
(1842,0,43,2729.8,-1677.57,126.499,0,0,0,''),
(1842,0,44,2716.78,-1694.65,126.301,0,0,0,''),
(1842,0,45,2706.66,-1709.47,123.42,0,0,0,''),
(1842,0,46,2699.51,-1720.57,120.265,0,0,0,''),
(1842,0,47,2691.98,-1738.47,114.994,0,0,0,''),
(1842,0,48,2690.51,-1757.05,108.764,0,0,0,''),
(1842,0,49,2691.95,-1780.31,99.89,0,0,0,''),
(1842,0,50,2689.34,-1803.26,89.13,0,0,0,''),
(1842,0,51,2697.85,-1820.55,80.681,0,0,0,''),
(1842,0,52,2701.93,-1836.71,73.7,0,0,0,''),
(1842,0,53,2698.09,-1853.87,68.999,0,0,0,''),
(1842,0,54,2693.66,-1870.24,66.882,0,0,0,''),
(1842,0,55,2682.35,-1885.25,66.009,0,0,0,''),
(1842,0,56,2668.23,-1900.8,66.256,3.50205,15000,0,'SAY_REACH_TOWER - escort paused'),
(1842,0,57,2668.23,-1900.8,66.256,3.50205,0,0,''),
(1978,0,1,1406.32,1083.1,52.55,0,0,0,''),
(1978,0,2,1400.49,1080.42,52.5,0,0,0,'SAY_START_2'),
(1978,0,3,1388.48,1083.1,52.52,0,0,0,''),
(1978,0,4,1370.16,1084.02,52.3,0,0,0,''),
(1978,0,5,1359.02,1080.85,52.46,0,0,0,''),
(1978,0,6,1341.43,1087.39,52.69,0,0,0,''),
(1978,0,7,1321.93,1090.51,50.66,0,0,0,''),
(1978,0,8,1312.98,1095.91,47.49,0,0,0,''),
(1978,0,9,1301.09,1102.94,47.76,0,0,0,''),
(1978,0,10,1297.73,1106.35,50.18,0,0,0,''),
(1978,0,11,1295.49,1124.32,50.49,0,0,0,''),
(1978,0,12,1294.84,1137.25,51.75,0,0,0,''),
(1978,0,13,1292.89,1158.99,52.65,0,0,0,''),
(1978,0,14,1290.75,1168.67,52.56,0,2000,0,'quest complete SAY_END'),
(1978,0,15,1287.12,1203.49,52.66,0,5000,0,'SAY_RANE'),
(1978,0,16,1288.3,1203.89,52.68,0,5000,0,'SAY_RANE_REPLY'),
(1978,0,17,1288.3,1203.89,52.68,0,5000,0,'SAY_CHECK_NEXT'),
(1978,0,18,1290.72,1207.44,52.69,0,0,0,''),
(1978,0,19,1297.5,1207.18,53.74,0,0,0,''),
(1978,0,20,1301.32,1220.9,53.74,0,0,0,''),
(1978,0,21,1298.55,1220.43,53.74,0,0,0,''),
(1978,0,22,1297.38,1212.87,58.51,0,0,0,''),
(1978,0,23,1297.8,1210.04,58.51,0,0,0,''),
(1978,0,24,1305.01,1206.1,58.51,0,0,0,''),
(1978,0,25,1310.51,1207.36,58.51,0,5000,0,'SAY_QUINN'),
(1978,0,26,1312.59,1207.21,58.51,0,5000,0,'SAY_QUINN_REPLY'),
(1978,0,27,1312.59,1207.21,58.51,0,30000,0,'SAY_BYE'),
(2713,0,1,-1416.91,-3044.12,36.21,0,0,0,''),
(2713,0,2,-1408.43,-3051.35,37.79,0,0,0,''),
(2713,0,3,-1399.45,-3069.2,31.25,0,0,0,''),
(2713,0,4,-1400.28,-3083.14,27.06,0,0,0,''),
(2713,0,5,-1405.3,-3096.72,26.36,0,0,0,''),
(2713,0,6,-1406.12,-3105.95,24.82,0,0,0,''),
(2713,0,7,-1417.41,-3106.8,16.61,0,0,0,''),
(2713,0,8,-1433.06,-3101.55,12.56,0,0,0,''),
(2713,0,9,-1439.86,-3086.36,12.29,0,0,0,''),
(2713,0,10,-1450.48,-3065.16,12.58,0,5000,0,'SAY_REACH_BOTTOM'),
(2713,0,11,-1456.15,-3055.53,12.54,0,0,0,''),
(2713,0,12,-1459.41,-3035.16,12.11,0,0,0,''),
(2713,0,13,-1472.47,-3034.18,12.44,0,0,0,''),
(2713,0,14,-1495.57,-3034.48,12.55,0,0,0,''),
(2713,0,15,-1524.91,-3035.47,13.15,0,0,0,''),
(2713,0,16,-1549.05,-3037.77,12.98,0,0,0,''),
(2713,0,17,-1555.69,-3028.02,13.64,0,3000,0,'SAY_WATCH_BACK'),
(2713,0,18,-1555.69,-3028.02,13.64,0,5000,0,'SAY_DATA_FOUND'),
(2713,0,19,-1555.69,-3028.02,13.64,0,2000,0,'SAY_ESCAPE'),
(2713,0,20,-1551.19,-3037.78,12.96,0,0,0,''),
(2713,0,21,-1584.6,-3048.77,13.67,0,0,0,''),
(2713,0,22,-1602.14,-3042.82,15.12,0,0,0,''),
(2713,0,23,-1610.68,-3027.42,17.22,0,0,0,''),
(2713,0,24,-1601.65,-3007.97,24.65,0,0,0,''),
(2713,0,25,-1581.05,-2992.32,30.85,0,0,0,''),
(2713,0,26,-1559.95,-2979.51,34.3,0,0,0,''),
(2713,0,27,-1536.51,-2969.78,32.64,0,0,0,''),
(2713,0,28,-1511.81,-2961.09,29.12,0,0,0,''),
(2713,0,29,-1484.83,-2960.87,32.54,0,0,0,''),
(2713,0,30,-1458.23,-2966.8,40.52,0,0,0,''),
(2713,0,31,-1440.2,-2971.2,43.15,0,0,0,''),
(2713,0,32,-1427.85,-2989.15,38.09,0,0,0,''),
(2713,0,33,-1420.27,-3008.91,35.01,0,0,0,''),
(2713,0,34,-1427.58,-3032.53,32.31,0,5000,0,'SAY_FINISH'),
(2713,0,35,-1427.4,-3035.17,32.26,0,0,0,''),
(2768,0,1,-2077.73,-2091.17,9.49,0,0,0,''),
(2768,0,2,-2077.99,-2105.33,13.24,0,0,0,''),
(2768,0,3,-2074.6,-2109.67,14.24,0,0,0,''),
(2768,0,4,-2076.6,-2117.46,16.67,0,0,0,''),
(2768,0,5,-2073.51,-2123.46,18.42,0,2000,0,''),
(2768,0,6,-2073.51,-2123.46,18.42,0,4000,0,''),
(2768,0,7,-2066.6,-2131.85,21.56,0,0,0,''),
(2768,0,8,-2053.85,-2143.19,20.31,0,0,0,''),
(2768,0,9,-2043.49,-2153.73,20.2,0,10000,0,''),
(2768,0,10,-2043.49,-2153.73,20.2,0,20000,0,''),
(2768,0,11,-2043.49,-2153.73,20.2,0,10000,0,''),
(2768,0,12,-2043.49,-2153.73,20.2,0,2000,0,''),
(2768,0,13,-2053.85,-2143.19,20.31,0,0,0,''),
(2768,0,14,-2066.6,-2131.85,21.56,0,0,0,''),
(2768,0,15,-2073.51,-2123.46,18.42,0,0,0,''),
(2768,0,16,-2076.6,-2117.46,16.67,0,0,0,''),
(2768,0,17,-2074.6,-2109.67,14.24,0,0,0,''),
(2768,0,18,-2077.99,-2105.33,13.24,0,0,0,''),
(2768,0,19,-2077.73,-2091.17,9.49,0,0,0,''),
(2768,0,20,-2066.41,-2086.21,8.97,0,6000,0,''),
(2768,0,21,-2066.41,-2086.21,8.97,0,2000,0,''),
(2917,0,1,4675.81,598.615,17.6457,0,0,0,'SAY_REM_START'),
(2917,0,2,4672.84,599.325,16.4176,0,0,0,''),
(2917,0,3,4663.45,607.43,10.4948,0,0,0,''),
(2917,0,4,4655.97,613.761,8.52327,0,0,0,''),
(2917,0,5,4640.8,623.999,8.37705,0,0,0,''),
(2917,0,6,4631.68,630.801,6.415,0,5000,0,'SAY_REM_RAMP1_1'),
(2917,0,7,4633.53,632.476,6.50983,0,0,0,'ambush'),
(2917,0,8,4639.41,637.121,13.3381,0,0,0,''),
(2917,0,9,4642.35,637.668,13.4374,0,0,0,''),
(2917,0,10,4645.08,634.464,13.4372,0,5000,0,'SAY_REM_RAMP1_2'),
(2917,0,11,4642.35,637.585,13.4352,0,0,0,''),
(2917,0,12,4639.63,637.234,13.3398,0,0,0,''),
(2917,0,13,4633.36,632.462,6.48844,0,0,0,''),
(2917,0,14,4624.71,631.724,6.26403,0,0,0,''),
(2917,0,15,4623.53,629.719,6.20134,0,5000,0,'SAY_REM_BOOK'),
(2917,0,16,4623.45,630.37,6.21894,0,0,0,'SAY_REM_TENT1_1'),
(2917,0,17,4622.62,637.222,6.31285,0,0,0,'ambush'),
(2917,0,18,4619.76,637.386,6.31205,0,5000,0,'SAY_REM_TENT1_2'),
(2917,0,19,4620.03,637.368,6.31205,0,0,0,''),
(2917,0,20,4624.15,637.56,6.3139,0,0,0,''),
(2917,0,21,4622.97,634.016,6.29498,0,0,0,''),
(2917,0,22,4616.93,630.303,6.23919,0,0,0,''),
(2917,0,23,4614.55,616.983,5.68764,0,0,0,''),
(2917,0,24,4610.28,610.029,5.44254,0,0,0,''),
(2917,0,25,4601.15,604.112,2.05486,0,0,0,''),
(2917,0,26,4589.62,597.686,1.05715,0,0,0,''),
(2917,0,27,4577.59,592.146,1.12019,0,0,0,'SAY_REM_MOSS (?)'),
(2917,0,28,4569.85,592.177,1.26087,0,5000,0,'EMOTE_REM_MOSS (?)'),
(2917,0,29,4568.79,590.871,1.21134,0,3000,0,'SAY_REM_MOSS_PROGRESS (?)'),
(2917,0,30,4566.72,564.078,1.34308,0,0,0,'ambush'),
(2917,0,31,4568.27,551.958,5.0042,0,0,0,''),
(2917,0,32,4566.73,551.558,5.42631,0,5000,0,'SAY_REM_PROGRESS'),
(2917,0,33,4566.74,560.768,1.70326,0,0,0,''),
(2917,0,34,4573.92,582.566,0.749801,0,0,0,''),
(2917,0,35,4594.21,598.533,1.03406,0,0,0,''),
(2917,0,36,4601.19,604.283,2.06015,0,0,0,''),
(2917,0,37,4609.54,610.845,5.40222,0,0,0,''),
(2917,0,38,4624.8,618.076,5.85154,0,0,0,''),
(2917,0,39,4632.41,623.778,7.28624,0,0,0,''),
(2917,0,40,4645.92,621.984,8.57997,0,0,0,''),
(2917,0,41,4658.67,611.093,8.89175,0,0,0,''),
(2917,0,42,4671.92,599.752,16.0124,0,5000,0,'SAY_REM_REMEMBER'),
(2917,0,43,4676.98,600.65,17.8257,0,5000,0,'EMOTE_REM_END'),
(3439,0,1,1105.09,-3101.25,82.706,0,1000,0,'SAY_STARTUP1'),
(3439,0,2,1103.2,-3104.35,83.113,0,1000,0,''),
(3439,0,3,1107.82,-3106.5,82.739,0,1000,0,''),
(3439,0,4,1104.73,-3100.83,82.747,0,1000,0,''),
(3439,0,5,1103.24,-3106.27,83.133,0,1000,0,''),
(3439,0,6,1112.81,-3106.29,82.32,0,1000,0,''),
(3439,0,7,1112.83,-3108.91,82.377,0,1000,0,''),
(3439,0,8,1108.05,-3115.16,82.894,0,0,0,''),
(3439,0,9,1108.36,-3104.37,82.377,0,5000,0,''),
(3439,0,10,1100.31,-3097.54,83.15,0,0,0,'SAY_STARTUP2'),
(3439,0,11,1100.56,-3082.72,82.768,0,0,0,''),
(3439,0,12,1097.51,-3069.23,82.206,0,0,0,''),
(3439,0,13,1092.96,-3053.11,82.351,0,0,0,''),
(3439,0,14,1094.01,-3036.96,82.888,0,0,0,''),
(3439,0,15,1095.62,-3025.76,83.392,0,0,0,''),
(3439,0,16,1107.66,-3013.53,85.653,0,0,0,''),
(3439,0,17,1119.65,-3006.93,87.019,0,0,0,''),
(3439,0,18,1129.99,-3002.41,91.232,0,7000,0,'SAY_MERCENARY'),
(3439,0,19,1133.33,-2997.71,91.675,0,1000,0,'SAY_PROGRESS_1'),
(3439,0,20,1131.8,-2987.95,91.976,0,1000,0,''),
(3439,0,21,1122.03,-2993.4,91.536,0,0,0,''),
(3439,0,22,1116.61,-2981.92,92.103,0,0,0,''),
(3439,0,23,1102.24,-2994.25,92.074,0,0,0,''),
(3439,0,24,1096.37,-2978.31,91.873,0,0,0,''),
(3439,0,25,1091.97,-2985.92,91.73,0,40000,0,'SAY_PROGRESS_2'),
(3465,0,1,-2095.84,-3650,61.716,0,0,0,''),
(3465,0,2,-2100.19,-3613.95,61.604,0,0,0,''),
(3465,0,3,-2098.55,-3601.56,59.154,0,0,0,''),
(3465,0,4,-2093.8,-3595.23,56.658,0,0,0,''),
(3465,0,5,-2072.58,-3578.83,48.844,0,0,0,''),
(3465,0,6,-2023.86,-3568.15,24.636,0,0,0,''),
(3465,0,7,-2013.58,-3571.5,22.203,0,0,0,''),
(3465,0,8,-2009.81,-3580.55,21.791,0,0,0,''),
(3465,0,9,-2015.3,-3597.39,21.76,0,0,0,''),
(3465,0,10,-2020.68,-3610.3,21.759,0,0,0,''),
(3465,0,11,-2019.99,-3640.16,21.759,0,0,0,''),
(3465,0,12,-2016.11,-3664.13,21.758,0,0,0,''),
(3465,0,13,-1999.4,-3679.44,21.316,0,0,0,''),
(3465,0,14,-1987.46,-3688.31,18.495,0,0,0,''),
(3465,0,15,-1973.97,-3687.67,14.996,0,0,0,''),
(3465,0,16,-1949.16,-3678.05,11.293,0,0,0,''),
(3465,0,17,-1934.09,-3682.86,9.897,0,30000,0,'SAY_GIL_AT_LAST'),
(3465,0,18,-1935.38,-3682.32,10.029,0,1500,0,'SAY_GIL_PROCEED'),
(3465,0,19,-1879.04,-3699.5,6.582,0,7500,0,'SAY_GIL_FREEBOOTERS'),
(3465,0,20,-1852.73,-3703.78,6.875,0,0,0,''),
(3465,0,21,-1812.99,-3718.5,10.572,0,0,0,''),
(3465,0,22,-1788.17,-3722.87,9.663,0,0,0,''),
(3465,0,23,-1767.21,-3739.92,10.082,0,0,0,''),
(3465,0,24,-1750.19,-3747.39,10.39,0,0,0,''),
(3465,0,25,-1729.34,-3776.67,11.779,0,0,0,''),
(3465,0,26,-1716,-3802.4,12.618,0,0,0,''),
(3465,0,27,-1690.71,-3829.26,13.905,0,0,0,''),
(3465,0,28,-1674.7,-3842.4,13.872,0,0,0,''),
(3465,0,29,-1632.73,-3846.11,14.401,0,0,0,''),
(3465,0,30,-1592.73,-3842.23,14.981,0,0,0,''),
(3465,0,31,-1561.61,-3839.32,19.118,0,0,0,''),
(3465,0,32,-1544.57,-3834.39,18.761,0,0,0,''),
(3465,0,33,-1512.51,-3831.72,22.914,0,0,0,''),
(3465,0,34,-1486.89,-3836.64,23.964,0,0,0,''),
(3465,0,35,-1434.19,-3852.7,18.843,0,0,0,''),
(3465,0,36,-1405.79,-3854.49,17.276,0,0,0,''),
(3465,0,37,-1366.59,-3852.38,19.273,0,0,0,''),
(3465,0,38,-1337.36,-3837.83,17.352,0,2000,0,'SAY_GIL_ALMOST'),
(3465,0,39,-1299.74,-3810.69,20.801,0,0,0,''),
(3465,0,40,-1277.14,-3782.79,25.918,0,0,0,''),
(3465,0,41,-1263.69,-3781.25,26.447,0,0,0,''),
(3465,0,42,-1243.67,-3786.33,25.281,0,0,0,''),
(3465,0,43,-1221.88,-3784.12,24.051,0,0,0,''),
(3465,0,44,-1204.01,-3775.94,24.437,0,0,0,''),
(3465,0,45,-1181.71,-3768.93,23.368,0,0,0,''),
(3465,0,46,-1156.91,-3751.56,21.074,0,0,0,''),
(3465,0,47,-1138.83,-3741.81,17.843,0,0,0,''),
(3465,0,48,-1080.1,-3738.78,19.805,0,0,0,'SAY_GIL_SWEET'),
(3465,0,49,-1069.07,-3735.01,19.302,0,0,0,''),
(3465,0,50,-1061.94,-3724.06,21.086,0,0,0,''),
(3465,0,51,-1053.59,-3697.61,27.32,0,0,0,''),
(3465,0,52,-1044.11,-3690.13,24.856,0,0,0,''),
(3465,0,53,-1040.26,-3690.74,25.342,0,0,0,''),
(3465,0,54,-1028.15,-3688.72,23.843,0,7500,0,'SAY_GIL_FREED'),
(3584,0,1,4520.4,420.235,33.5284,0,2000,0,''),
(3584,0,2,4512.26,408.881,32.9308,0,0,0,''),
(3584,0,3,4507.94,396.47,32.9476,0,0,0,''),
(3584,0,4,4507.53,383.781,32.995,0,0,0,''),
(3584,0,5,4512.1,374.02,33.166,0,0,0,''),
(3584,0,6,4519.75,373.241,33.1574,0,0,0,''),
(3584,0,7,4592.41,369.127,31.4893,0,0,0,''),
(3584,0,8,4598.55,364.801,31.4947,0,0,0,''),
(3584,0,9,4602.76,357.649,32.9265,0,0,0,''),
(3584,0,10,4597.88,352.629,34.0317,0,0,0,''),
(3584,0,11,4590.23,350.9,36.2977,0,0,0,''),
(3584,0,12,4581.5,348.254,38.3878,0,0,0,''),
(3584,0,13,4572.05,348.059,42.3539,0,0,0,''),
(3584,0,14,4564.75,344.041,44.2463,0,0,0,''),
(3584,0,15,4556.63,341.003,47.6755,0,0,0,''),
(3584,0,16,4554.38,334.968,48.8003,0,0,0,''),
(3584,0,17,4557.63,329.783,49.9532,0,0,0,''),
(3584,0,18,4563.32,316.829,53.2409,0,0,0,''),
(3584,0,19,4566.09,303.127,55.0396,0,0,0,''),
(3584,0,20,4561.65,295.456,57.0984,0,4000,0,'SAY_THERYLUNE_FINISH'),
(3584,0,21,4551.03,293.333,57.1534,0,2000,0,''),
(3678,0,1,-134.925,125.468,-78.16,0,0,0,''),
(3678,0,2,-125.684,132.937,-78.42,0,0,0,''),
(3678,0,3,-113.812,139.295,-80.98,0,0,0,''),
(3678,0,4,-109.854,157.538,-80.2,0,0,0,''),
(3678,0,5,-108.64,175.207,-79.74,0,0,0,''),
(3678,0,6,-108.668,195.457,-80.64,0,0,0,''),
(3678,0,7,-111.007,219.007,-86.58,0,0,0,''),
(3678,0,8,-102.408,232.821,-91.52,0,0,0,'first corner SAY_FIRST_CORNER'),
(3678,0,9,-92.434,227.742,-90.75,0,0,0,''),
(3678,0,10,-82.456,224.853,-93.57,0,0,0,''),
(3678,0,11,-67.789,208.073,-93.34,0,0,0,''),
(3678,0,12,-43.343,205.295,-96.37,0,0,0,''),
(3678,0,13,-34.676,221.394,-95.82,0,0,0,''),
(3678,0,14,-32.582,238.573,-93.51,0,0,0,''),
(3678,0,15,-42.149,258.672,-92.88,0,0,0,''),
(3678,0,16,-55.257,274.696,-92.83,0,0,0,'circle of flames SAY_CIRCLE_BANISH'),
(3678,0,17,-48.604,287.584,-92.46,0,0,0,''),
(3678,0,18,-47.236,296.093,-90.88,0,0,0,''),
(3678,0,19,-35.618,309.067,-89.73,0,0,0,''),
(3678,0,20,-23.573,311.376,-88.6,0,0,0,''),
(3678,0,21,-8.692,302.389,-87.43,0,0,0,''),
(3678,0,22,-1.237,293.268,-85.55,0,0,0,''),
(3678,0,23,10.398,279.294,-85.86,0,0,0,''),
(3678,0,24,23.108,264.693,-86.69,0,0,0,''),
(3678,0,25,31.996,251.436,-87.62,0,0,0,''),
(3678,0,26,43.374,233.073,-87.61,0,0,0,''),
(3678,0,27,54.438,212.048,-89.5,0,3000,0,'chamber entrance SAY_NARALEX_CHAMBER'),
(3678,0,28,78.794,208.895,-92.84,0,0,0,''),
(3678,0,29,88.392,225.231,-94.46,0,0,0,''),
(3678,0,30,98.758,233.938,-95.84,0,0,0,''),
(3678,0,31,107.248,233.054,-95.98,0,0,0,''),
(3678,0,32,112.825,233.907,-96.39,0,0,0,''),
(3678,0,33,114.634,236.969,-96.04,0,1000,0,'naralex SAY_BEGIN_RITUAL'),
(3678,0,34,127.385,252.279,-90.07,0,0,0,''),
(3678,0,35,121.595,264.488,-91.55,0,0,0,''),
(3678,0,36,115.472,264.253,-91.5,0,0,0,''),
(3678,0,37,99.988,252.79,-91.51,0,0,0,''),
(3678,0,38,96.347,245.038,-90.34,0,0,0,''),
(3678,0,39,82.201,216.273,-86.1,0,0,0,''),
(3678,0,40,75.112,206.494,-84.8,0,0,0,''),
(3678,0,41,27.174,201.064,-72.31,0,0,0,''),
(3678,0,42,-41.114,204.149,-78.94,0,0,0,''),
(3692,0,1,4608.43,-6.32,69.74,0,1000,0,'stand up'),
(3692,0,2,4608.43,-6.32,69.74,0,4000,0,'SAY_START'),
(3692,0,3,4604.54,-5.17,69.51,0,0,0,''),
(3692,0,4,4604.26,-2.02,69.42,0,0,0,''),
(3692,0,5,4607.75,3.79,70.13,0,1000,0,'first ambush'),
(3692,0,6,4607.75,3.79,70.13,0,0,0,'SAY_FIRST_AMBUSH'),
(3692,0,7,4619.77,27.47,70.4,0,0,0,''),
(3692,0,8,4626.28,42.46,68.75,0,0,0,''),
(3692,0,9,4633.13,51.17,67.4,0,0,0,''),
(3692,0,10,4639.67,79.03,61.74,0,0,0,''),
(3692,0,11,4647.54,94.25,59.92,0,0,0,'second ambush'),
(3692,0,12,4682.08,113.47,54.83,0,0,0,''),
(3692,0,13,4705.28,137.81,53.36,0,0,0,'last ambush'),
(3692,0,14,4730.3,158.76,52.33,0,0,0,''),
(3692,0,15,4756.47,195.65,53.61,0,10000,0,'SAY_END'),
(3692,0,16,4608.43,-6.32,69.74,0,1000,0,'bow'),
(3692,0,17,4608.43,-6.32,69.74,0,4000,0,'SAY_ESCAPE'),
(3692,0,18,4608.43,-6.32,69.74,0,4000,0,'SPELL_MOONSTALKER_FORM'),
(3692,0,19,4604.54,-5.17,69.51,0,0,0,''),
(3692,0,20,4604.26,-2.02,69.42,0,0,0,''),
(3692,0,21,4607.75,3.79,70.13,0,0,0,''),
(3692,0,22,4607.75,3.79,70.13,0,0,0,''),
(3692,0,23,4619.77,27.47,70.4,0,0,0,''),
(3692,0,24,4640.33,33.74,68.22,0,0,0,'quest complete'),
(3849,0,1,-250.923,2116.26,81.179,0,0,0,'SAY_FREE_AD'),
(3849,0,2,-255.049,2119.39,81.179,0,0,0,''),
(3849,0,3,-254.129,2123.45,81.179,0,0,0,''),
(3849,0,4,-253.898,2130.87,81.179,0,0,0,''),
(3849,0,5,-249.889,2142.31,86.972,0,0,0,''),
(3849,0,6,-248.205,2144.02,87.013,0,0,0,''),
(3849,0,7,-240.553,2140.55,87.012,0,0,0,''),
(3849,0,8,-237.514,2142.07,87.012,0,0,0,''),
(3849,0,9,-235.638,2149.23,90.587,0,0,0,''),
(3849,0,10,-237.188,2151.95,90.624,0,0,0,''),
(3849,0,11,-238.84,2153.6,90.625,0,0,0,''),
(3849,0,12,-239.56,2155.80,90.625,1.889,2000,0,'SAY_OPEN_DOOR_AD SAY_UNLOCK_DOOR_AD'),
(3849,0,13,-208.764,2141.6,90.6257,0,0,0,''),
(3849,0,14,-206.441,2143.51,90.4287,0,0,0,''),
(3849,0,15,-203.715,2145.85,88.7052,0,0,0,''),
(3849,0,16,-199.199,2144.88,86.501,0,0,0,''),
(3849,0,17,-195.798,2143.58,86.501,0,0,0,''),
(3849,0,18,-190.029,2141.38,83.2712,0,0,0,''),
(3849,0,19,-189.353,2138.65,83.1102,0,0,0,''),
(3849,0,20,-190.304,2135.73,81.5288,0,0,0,''),
(3849,0,21,-207.325,2112.43,81.0548,0,0,0,''),
(3849,0,22,-208.754,2109.9,81.0527,0,0,0,''),
(3849,0,23,-206.248,2108.62,81.0555,0,0,0,''),
(3849,0,24,-202.017,2106.64,78.6836,0,0,0,''),
(3849,0,25,-200.928,2104.49,78.5569,0,0,0,''),
(3849,0,26,-201.845,2101.17,76.9256,0,0,0,''),
(3849,0,27,-202.844,2100.11,76.8911,0,0,0,''),
(3849,0,28,-213.326,2105.83,76.8925,0,0,0,''),
(3849,0,29,-226.993,2111.47,76.8892,0,0,0,''),
(3849,0,30,-227.955,2112.34,76.8895,0,0,0,''),
(3849,0,31,-230.05,2106.64,76.8895,0,0,0,''),
(3850,0,1,-241.817,2122.9,81.179,0,0,0,'SAY_FREE_AS'),
(3850,0,2,-247.139,2124.89,81.179,0,0,0,''),
(3850,0,3,-253.179,2127.41,81.179,0,0,0,''),
(3850,0,4,-253.898,2130.87,81.179,0,0,0,''),
(3850,0,5,-249.889,2142.31,86.972,0,0,0,''),
(3850,0,6,-248.205,2144.02,87.013,0,0,0,''),
(3850,0,7,-240.553,2140.55,87.012,0,0,0,''),
(3850,0,8,-237.514,2142.07,87.012,0,0,0,''),
(3850,0,9,-235.638,2149.23,90.587,0,0,0,''),
(3850,0,10,-237.188,2151.95,90.624,0,0,0,''),
(3850,0,11,-241.162,2153.65,90.624,0,0,0,'SAY_OPEN_DOOR_AS'),
(3850,0,12,-241.153,2154.67,90.625,1.1534,2000,0,'SAY_OPEN_DOOR_AS'),
(4484,0,1,3178.57,188.52,4.27,0,0,0,'SAY_QUEST_START'),
(4484,0,2,3189.82,198.56,5.62,0,0,0,''),
(4484,0,3,3215.21,185.78,6.43,0,0,0,''),
(4484,0,4,3224.05,183.08,6.74,0,0,0,''),
(4484,0,5,3228.11,194.97,7.51,0,0,0,''),
(4484,0,6,3225.33,201.78,7.25,0,0,0,''),
(4484,0,7,3233.33,226.88,10.18,0,0,0,''),
(4484,0,8,3274.12,225.83,10.72,0,0,0,''),
(4484,0,9,3321.63,209.82,12.36,0,0,0,''),
(4484,0,10,3369.66,226.21,11.69,0,0,0,''),
(4484,0,11,3402.35,227.2,9.48,0,0,0,''),
(4484,0,12,3441.92,224.75,10.85,0,0,0,''),
(4484,0,13,3453.87,220.31,12.52,0,0,0,''),
(4484,0,14,3472.51,213.68,13.26,0,0,0,''),
(4484,0,15,3515.49,212.96,9.76,0,5000,0,'SAY_FIRST_AMBUSH_START'),
(4484,0,16,3516.21,212.84,9.52,0,20000,0,'SAY_FIRST_AMBUSH_END'),
(4484,0,17,3548.22,217.12,7.34,0,0,0,''),
(4484,0,18,3567.57,219.43,5.22,0,0,0,''),
(4484,0,19,3659.85,209.68,2.27,0,0,0,''),
(4484,0,20,3734.9,177.64,6.75,0,0,0,''),
(4484,0,21,3760.24,162.51,7.49,0,5000,0,'SAY_SECOND_AMBUSH_START'),
(4484,0,22,3761.58,161.14,7.37,0,20000,0,'SAY_SECOND_AMBUSH_END'),
(4484,0,23,3801.17,129.87,9.38,0,0,0,''),
(4484,0,24,3815.53,118.53,10.14,0,0,0,''),
(4484,0,25,3894.58,44.88,15.49,0,0,0,''),
(4484,0,26,3972.83,0.42,17.34,0,0,0,''),
(4484,0,27,4026.41,-7.63,16.77,0,0,0,''),
(4484,0,28,4086.24,12.32,16.12,0,0,0,''),
(4484,0,29,4158.79,50.67,25.86,0,0,0,''),
(4484,0,30,4223.48,99.52,35.47,0,5000,0,'SAY_FINAL_AMBUSH_START'),
(4484,0,31,4224.28,100.02,35.49,0,10000,0,'SAY_QUEST_END'),
(4484,0,32,4243.45,117.44,38.83,0,0,0,''),
(4484,0,33,4264.18,134.22,42.96,0,0,0,''),
(4500,0,1,-3125.6,-2885.67,34.731,0,2500,0,''),
(4500,0,2,-3120.26,-2877.83,34.917,0,0,0,''),
(4500,0,3,-3116.49,-2850.67,34.869,0,0,0,''),
(4500,0,4,-3093.47,-2819.19,34.432,0,0,0,''),
(4500,0,5,-3104.73,-2802.02,33.954,0,0,0,''),
(4500,0,6,-3105.91,-2780.23,34.469,0,0,0,''),
(4500,0,7,-3116.08,-2757.9,34.734,0,0,0,''),
(4500,0,8,-3125.23,-2733.96,33.189,0,0,0,''),
(4508,0,1,2194.38,1791.65,65.48,0,5000,0,''),
(4508,0,2,2188.56,1805.87,64.45,0,0,0,''),
(4508,0,3,2186.2,1836.28,59.859,0,5000,0,'SAY_WILLIX_1'),
(4508,0,4,2163.27,1851.67,56.73,0,0,0,''),
(4508,0,5,2140.22,1845.02,48.32,0,0,0,''),
(4508,0,6,2131.5,1804.29,46.85,0,0,0,''),
(4508,0,7,2096.18,1789.03,51.13,0,3000,0,'SAY_WILLIX_2'),
(4508,0,8,2074.46,1780.09,55.64,0,0,0,''),
(4508,0,9,2055.12,1768.67,58.46,0,0,0,''),
(4508,0,10,2037.83,1748.62,60.27,0,5000,0,'SAY_WILLIX_3'),
(4508,0,11,2037.51,1728.94,60.85,0,0,0,''),
(4508,0,12,2044.7,1711.71,59.71,0,0,0,''),
(4508,0,13,2067.66,1701.84,57.77,0,0,0,''),
(4508,0,14,2078.91,1704.54,56.77,0,0,0,''),
(4508,0,15,2097.65,1715.24,54.74,0,3000,0,'SAY_WILLIX_4'),
(4508,0,16,2106.44,1720.98,54.41,0,0,0,''),
(4508,0,17,2123.96,1732.56,52.27,0,0,0,''),
(4508,0,18,2153.82,1728.73,51.92,0,0,0,''),
(4508,0,19,2163.49,1706.33,54.42,0,0,0,''),
(4508,0,20,2158.75,1695.98,55.7,0,0,0,''),
(4508,0,21,2142.6,1680.72,58.24,0,0,0,''),
(4508,0,22,2118.31,1671.54,59.21,0,0,0,''),
(4508,0,23,2086.02,1672.04,61.24,0,0,0,''),
(4508,0,24,2068.81,1658.93,61.24,0,0,0,''),
(4508,0,25,2062.82,1633.31,64.35,0,0,0,''),
(4508,0,26,2060.92,1600.11,62.41,0,3000,0,'SAY_WILLIX_5'),
(4508,0,27,2063.05,1589.16,63.26,0,0,0,''),
(4508,0,28,2063.67,1577.22,65.89,0,0,0,''),
(4508,0,29,2057.94,1560.68,68.4,0,0,0,''),
(4508,0,30,2052.56,1548.05,73.35,0,0,0,''),
(4508,0,31,2045.22,1543.4,76.65,0,0,0,''),
(4508,0,32,2034.35,1543.01,79.7,0,0,0,''),
(4508,0,33,2029.95,1542.94,80.79,0,0,0,''),
(4508,0,34,2021.34,1538.67,80.8,0,0,0,'SAY_WILLIX_6'),
(4508,0,35,2012.45,1549.48,79.93,0,0,0,''),
(4508,0,36,2008.05,1554.92,80.44,0,0,0,''),
(4508,0,37,2006.54,1562.72,81.11,0,0,0,''),
(4508,0,38,2003.8,1576.43,81.57,0,0,0,''),
(4508,0,39,2000.57,1590.06,80.62,0,0,0,''),
(4508,0,40,1998.96,1596.87,80.22,0,0,0,''),
(4508,0,41,1991.19,1600.82,79.39,0,0,0,''),
(4508,0,42,1980.71,1601.44,79.77,0,0,0,''),
(4508,0,43,1967.22,1600.18,80.62,0,0,0,''),
(4508,0,44,1956.43,1596.97,81.75,0,0,0,''),
(4508,0,45,1954.87,1592.02,82.18,0,3000,0,'SAY_WILLIX_7'),
(4508,0,46,1948.35,1571.35,80.96,0,30000,0,'SAY_WILLIX_END'),
(4508,0,47,1947.02,1566.42,81.8,0,30000,0,''),
(4880,0,1,-2670.22,-3446.19,34.085,0,0,0,''),
(4880,0,2,-2683.96,-3451.09,34.707,0,0,0,''),
(4880,0,3,-2703.24,-3454.82,33.395,0,0,0,''),
(4880,0,4,-2721.61,-3457.41,33.626,0,0,0,''),
(4880,0,5,-2739.98,-3459.84,33.329,0,0,0,''),
(4880,0,6,-2756.24,-3460.52,32.037,0,5000,0,'SAY_STINKY_FIRST_STOP'),
(4880,0,7,-2764.52,-3472.71,33.75,0,0,0,''),
(4880,0,8,-2773.68,-3482.91,32.84,0,0,0,''),
(4880,0,9,-2781.39,-3490.61,32.598,0,0,0,''),
(4880,0,10,-2788.31,-3492.9,30.761,0,0,0,''),
(4880,0,11,-2794.58,-3489.19,31.119,0,5000,0,'SAY_SECOND_STOP'),
(4880,0,12,-2789.43,-3498.04,31.05,0,0,0,''),
(4880,0,13,-2786.97,-3508.17,31.983,0,0,0,''),
(4880,0,14,-2786.77,-3519.95,31.079,0,0,0,''),
(4880,0,15,-2789.36,-3525.02,31.831,0,0,0,''),
(4880,0,16,-2797.95,-3523.69,31.697,0,0,0,''),
(4880,0,17,-2812.97,-3519.84,29.864,0,0,0,''),
(4880,0,18,-2818.33,-3521.4,30.563,0,0,0,''),
(4880,0,19,-2824.77,-3528.73,32.399,0,0,0,''),
(4880,0,20,-2830.7,-3539.88,32.505,0,0,0,''),
(4880,0,21,-2836.24,-3549.96,31.18,0,0,0,''),
(4880,0,22,-2837.58,-3561.05,30.74,0,0,0,''),
(4880,0,23,-2834.45,-3568.26,30.751,0,0,0,''),
(4880,0,24,-2827.35,-3569.81,31.316,0,0,0,''),
(4880,0,25,-2817.38,-3566.96,30.947,0,5000,0,'SAY_THIRD_STOP_1'),
(4880,0,26,-2817.38,-3566.96,30.947,0,2000,0,'SAY_THIRD_STOP_2'),
(4880,0,27,-2817.38,-3566.96,30.947,0,0,0,'SAY_THIRD_STOP_3'),
(4880,0,28,-2818.81,-3579.42,28.525,0,0,0,''),
(4880,0,29,-2820.21,-3590.64,30.269,0,0,0,''),
(4880,0,30,-2820.85,-3593.94,31.15,0,3000,0,''),
(4880,0,31,-2820.85,-3593.94,31.15,0,3000,0,'SAY_PLANT_GATHERED'),
(4880,0,32,-2834.21,-3592.04,33.79,0,0,0,''),
(4880,0,33,-2840.31,-3586.21,36.288,0,0,0,''),
(4880,0,34,-2847.49,-3576.42,37.66,0,0,0,''),
(4880,0,35,-2855.72,-3565.18,39.39,0,0,0,''),
(4880,0,36,-2861.78,-3552.9,41.243,0,0,0,''),
(4880,0,37,-2869.54,-3545.58,40.701,0,0,0,''),
(4880,0,38,-2877.78,-3538.37,37.274,0,0,0,''),
(4880,0,39,-2882.68,-3534.17,34.844,0,0,0,''),
(4880,0,40,-2888.57,-3534.12,34.298,0,4000,0,'SAY_STINKY_END'),
(4880,0,41,-2888.57,-3534.12,34.298,0,0,0,''),
(4962,0,1,-3804.44,-828.048,10.0931,0,0,0,''),
(4962,0,2,-3803.93,-835.772,10.0777,0,0,0,''),
(4962,0,3,-3792.63,-835.671,9.65566,0,0,0,''),
(4962,0,4,-3772.43,-835.346,10.869,0,0,0,''),
(4962,0,5,-3765.94,-840.129,10.8856,0,0,0,''),
(4962,0,6,-3738.63,-830.997,11.0574,0,0,0,''),
(4962,0,7,-3690.22,-862.262,9.96045,0,0,0,''),
(4983,0,1,-3322.65,-3124.63,33.842,0,0,0,''),
(4983,0,2,-3326.34,-3126.83,34.426,0,0,0,''),
(4983,0,3,-3336.98,-3129.61,30.692,0,0,0,''),
(4983,0,4,-3342.6,-3132.15,30.422,0,0,0,''),
(4983,0,5,-3355.83,-3140.95,29.534,0,0,0,''),
(4983,0,6,-3365.83,-3144.28,35.176,0,0,0,''),
(4983,0,7,-3368.9,-3147.27,36.091,0,0,0,''),
(4983,0,8,-3369.36,-3169.83,36.325,0,0,0,''),
(4983,0,9,-3371.44,-3183.91,33.454,0,0,0,''),
(4983,0,10,-3373.82,-3190.86,34.717,0,5000,0,'SAY_OGR_SPOT'),
(4983,0,11,-3368.53,-3198.21,34.926,0,0,0,'SAY_OGR_RET_WHAT'),
(4983,0,12,-3366.27,-3210.87,33.733,0,5000,0,'pause'),
(4983,0,13,-3368.53,-3198.21,34.926,0,0,0,''),
(4983,0,14,-3373.82,-3190.86,34.717,0,0,0,''),
(4983,0,15,-3371.44,-3183.91,33.454,0,0,0,''),
(4983,0,16,-3369.36,-3169.83,36.325,0,0,0,''),
(4983,0,17,-3368.9,-3147.27,36.091,0,0,0,''),
(4983,0,18,-3365.83,-3144.28,35.176,0,0,0,''),
(4983,0,19,-3355.83,-3140.95,29.534,0,0,0,''),
(4983,0,20,-3342.6,-3132.15,30.422,0,0,0,''),
(4983,0,21,-3336.98,-3129.61,30.692,0,0,0,''),
(4983,0,22,-3326.34,-3126.83,34.426,0,0,0,''),
(4983,0,23,-3322.65,-3124.63,33.842,0,0,0,''),
(5391,0,1,-9901.12,-3727.29,22.11,3.8316,3000,0,'Galen Goodward - Escort starts'),
(5391,0,2,-9909.27,-3727.81,23.25,0,0,0,''),
(5391,0,3,-9935.25,-3729.02,22.11,0,0,0,''),
(5391,0,4,-9945.83,-3719.34,21.68,0,0,0,''),
(5391,0,5,-9963.41,-3710.18,21.71,0,0,0,''),
(5391,0,6,-9972.75,-3690.13,21.68,0,0,0,''),
(5391,0,7,-9989.7,-3669.67,21.67,0,0,0,''),
(5391,0,8,-9989.21,-3647.76,23,0,0,0,''),
(5391,0,9,-9992.27,-3633.74,21.67,0,0,0,''),
(5391,0,10,-10002.3,-3611.67,22.26,0,0,0,''),
(5391,0,11,-9999.25,-3586.33,21.85,0,0,0,''),
(5391,0,12,-10006.5,-3571.99,21.67,0,0,0,''),
(5391,0,13,-10014.3,-3545.24,21.67,0,0,0,''),
(5391,0,14,-10018.9,-3525.03,21.68,0,0,0,''),
(5391,0,15,-10030.2,-3514.77,21.67,0,0,0,''),
(5391,0,16,-10045.1,-3501.49,21.67,0,0,0,''),
(5391,0,17,-10052.9,-3479.13,21.67,0,0,0,''),
(5391,0,18,-10060.7,-3460.31,21.67,0,0,0,''),
(5391,0,19,-10074.7,-3436.85,20.97,0,0,0,''),
(5391,0,20,-10074.7,-3436.85,20.97,0,0,0,'Galen Goodward - Set Run'),
(5391,0,21,-10072.9,-3408.92,20.43,2.8412,1000,0,'Galen Goodward - Escort complete and SAY_QUEST_COMPLETE'),
(5391,0,22,-10078.2,-3407.64,20.44,0,0,0,'Galen Goodward - EMOTE_DISAPPEAR'),
(5391,0,23,-10108,-3406.05,22.06,0,1000,0,'Galen Goodward - Despawn'),
(5644,0,1,-339.679,1752.04,139.482,0,0,0,''),
(5644,0,2,-328.957,1734.95,139.327,0,0,0,''),
(5644,0,3,-338.29,1731.36,139.327,0,0,0,''),
(5644,0,4,-350.747,1731.12,139.338,0,0,0,''),
(5644,0,5,-365.064,1739.04,139.376,0,0,0,''),
(5644,0,6,-371.105,1746.03,139.374,0,0,0,''),
(5644,0,7,-383.141,1738.62,138.93,0,0,0,''),
(5644,0,8,-390.445,1733.98,136.353,0,0,0,''),
(5644,0,9,-401.368,1726.77,131.071,0,0,0,''),
(5644,0,10,-416.016,1721.19,129.807,0,0,0,''),
(5644,0,11,-437.139,1709.82,126.342,0,0,0,''),
(5644,0,12,-455.83,1695.61,119.305,0,0,0,''),
(5644,0,13,-459.862,1687.92,116.059,0,0,0,''),
(5644,0,14,-463.565,1679.1,111.653,0,0,0,''),
(5644,0,15,-461.485,1670.94,109.033,0,0,0,''),
(5644,0,16,-471.786,1647.34,102.862,0,0,0,''),
(5644,0,17,-477.146,1625.69,98.342,0,0,0,''),
(5644,0,18,-475.815,1615.81,97.07,0,0,0,''),
(5644,0,19,-474.329,1590.01,94.4982,0,0,0,''),
(6182,0,1,-11466.8,1530.15,50.2636,0,0,0,'Daphne Stilwell - Quest start'),
(6182,0,2,-11465.2,1528.34,50.9544,0,0,0,'Daphne Stilwell - entrance hut'),
(6182,0,3,-11463,1525.24,50.9377,0,0,0,''),
(6182,0,4,-11461,1526.61,50.9377,0,5000,0,'Daphne Stilwell - pick up rifle'),
(6182,0,5,-11463,1525.24,50.9377,0,0,0,''),
(6182,0,6,-11465.2,1528.34,50.9544,0,0,0,''),
(6182,0,7,-11468.4,1535.08,50.4009,0.9271,500,0,'Daphne Stilwell - pause escort'),
(6182,0,8,-11468.4,1535.08,50.4009,0.9271,5000,0,'Daphne Stilwell - pause escort'),
(6182,0,9,-11467.9,1532.46,50.3489,0,0,0,'Daphne Stilwell - end of ambush waves'),
(6182,0,10,-11466.1,1529.86,50.2094,0,0,0,''),
(6182,0,11,-11463,1525.24,50.9377,0,0,0,''),
(6182,0,12,-11461,1526.61,50.9377,0,5000,0,'Daphne Stilwell - deliver rifle'),
(6182,0,13,-11463,1525.24,50.9377,0,0,0,''),
(6182,0,14,-11465.2,1528.34,50.9544,0,0,0,''),
(6182,0,15,-11470.3,1537.28,50.3785,0,0,0,''),
(6182,0,16,-11475.6,1548.68,50.1844,0,0,0,'Daphne Stilwell - Complete quest'),
(6182,0,17,-11482.3,1557.41,48.6245,0,0,0,'Daphne Stilwell - resume movement'),
(6575,0,1,1945.81,-431.54,16.36,0,0,0,''),
(6575,0,2,1946.21,-436.41,16.36,0,0,0,''),
(6575,0,3,1950.01,-444.11,14.63,0,0,0,''),
(6575,0,4,1956.08,-449.34,13.12,0,0,0,''),
(6575,0,5,1966.59,-450.55,11.27,0,0,0,''),
(6575,0,6,1976.09,-447.51,11.27,0,0,0,''),
(6575,0,7,1983.42,-435.85,11.27,0,0,0,''),
(6575,0,8,1978.17,-428.81,11.27,0,0,0,''),
(6575,0,9,1973.97,-422.08,9.04,0,0,0,''),
(6575,0,10,1963.84,-418.9,6.17,0,0,0,''),
(6575,0,11,1961.22,-422.74,6.17,0,0,0,''),
(6575,0,12,1964.8,-431.26,6.17,0,300000,0,''),
(7780,0,1,261.059,-2757.88,122.553,0,0,0,''),
(7780,0,2,259.812,-2758.25,122.555,0,0,0,'SAY_RIN_FREE'),
(7780,0,3,253.823,-2758.62,122.562,0,0,0,''),
(7780,0,4,241.395,-2769.75,123.309,0,0,0,''),
(7780,0,5,218.916,-2783.4,123.355,0,0,0,''),
(7780,0,6,209.088,-2789.68,122.001,0,0,0,''),
(7780,0,7,204.454,-2792.21,120.62,0,0,0,''),
(7780,0,8,182.013,-2810,113.887,0,0,0,'summon'),
(7780,0,9,164.412,-2825.16,107.779,0,0,0,''),
(7780,0,10,149.728,-2833.7,106.224,0,0,0,''),
(7780,0,11,142.448,-2838.81,109.665,0,0,0,''),
(7780,0,12,133.275,-2845.14,112.606,0,0,0,''),
(7780,0,13,111.247,-2861.07,116.305,0,0,0,''),
(7780,0,14,96.1041,-2874.89,114.397,0,0,0,'summon'),
(7780,0,15,73.3699,-2881.18,117.666,0,0,0,''),
(7780,0,16,58.5792,-2889.15,116.253,0,0,0,''),
(7780,0,17,33.2142,-2906.34,115.083,0,0,0,''),
(7780,0,18,19.5865,-2908.71,117.276,0,7500,0,'SAY_RIN_COMPLETE'),
(7780,0,19,10.2825,-2911.61,118.394,0,0,0,''),
(7780,0,20,-37.5804,-2942.73,117.145,0,0,0,''),
(7780,0,21,-68.5994,-2953.69,116.685,0,0,0,''),
(7780,0,22,-102.054,-2956.97,116.677,0,0,0,''),
(7780,0,23,-135.994,-2955.74,115.788,0,0,0,''),
(7780,0,24,-171.562,-2951.42,115.451,0,0,0,''),
(7784,0,1,-8845.65,-4373.98,43.87,0,5000,0,'SAY_START'),
(7784,0,2,-8840.79,-4373.73,44.24,0,0,0,''),
(7784,0,3,-8837.43,-4373.56,45.6,0,0,0,''),
(7784,0,4,-8832.74,-4373.32,45.68,0,0,0,''),
(7784,0,5,-8829.37,-4373.14,44.33,0,0,0,''),
(7784,0,6,-8817.38,-4372.41,35.58,0,0,0,''),
(7784,0,7,-8803.47,-4371.6,30.34,0,0,0,''),
(7784,0,8,-8795.1,-4365.61,26.08,0,0,0,''),
(7784,0,9,-8766.78,-4367.13,25.15,0,0,0,''),
(7784,0,10,-8755.63,-4367.54,24.63,0,0,0,''),
(7784,0,11,-8754.42,-4365.59,24.15,0,0,0,''),
(7784,0,12,-8728.82,-4353.13,20.9,0,0,0,''),
(7784,0,13,-8706.6,-4356.55,17.93,0,0,0,''),
(7784,0,14,-8679,-4380.23,12.64,0,0,0,''),
(7784,0,15,-8642.96,-4393.82,12.52,0,0,0,''),
(7784,0,16,-8611.19,-4399.11,9.55,0,0,0,''),
(7784,0,17,-8554.87,-4409.32,13.05,0,0,0,''),
(7784,0,18,-8531.64,-4411.96,11.2,0,0,0,''),
(7784,0,19,-8510.4,-4414.38,12.84,0,0,0,''),
(7784,0,20,-8476.92,-4418.34,9.71,0,0,0,''),
(7784,0,21,-8435.89,-4426.74,9.67,0,0,0,''),
(7784,0,22,-8381.89,-4446.4,10.23,0,0,0,''),
(7784,0,23,-8351.15,-4447.79,9.99,0,5000,0,'first ambush SAY_AMBUSH'),
(7784,0,24,-8324.18,-4445.05,9.71,0,0,0,''),
(7784,0,25,-8138.94,-4384.78,10.92,0,0,0,''),
(7784,0,26,-8036.87,-4443.38,9.65,0,0,0,''),
(7784,0,27,-7780.92,-4761.81,9.5,0,0,0,''),
(7784,0,28,-7587.67,-4765.01,8.96,0,0,0,''),
(7784,0,29,-7497.65,-4792.86,10.01,0,0,0,'second ambush SAY_AMBUSH'),
(7784,0,30,-7391.54,-4774.26,12.44,0,0,0,''),
(7784,0,31,-7308.42,-4739.87,12.65,0,0,0,''),
(7784,0,32,-7016.11,-4751.12,10.06,0,0,0,''),
(7784,0,33,-6985.52,-4777.41,10.26,0,0,0,''),
(7784,0,34,-6953.02,-4786,6.32,0,0,0,''),
(7784,0,35,-6940.37,-4831.03,0.67,0,10000,0,'quest complete SAY_END'),
(7806,0,1,495.404,-3478.35,114.837,0,0,0,''),
(7806,0,2,492.705,-3486.11,108.627,0,0,0,''),
(7806,0,3,487.25,-3485.76,107.89,0,0,0,''),
(7806,0,4,476.852,-3489.88,99.985,0,0,0,''),
(7806,0,5,467.212,-3493.36,99.819,0,0,0,''),
(7806,0,6,460.017,-3496.98,104.481,0,0,0,''),
(7806,0,7,439.619,-3500.73,110.534,0,0,0,''),
(7806,0,8,428.326,-3495.87,118.662,0,0,0,''),
(7806,0,9,424.664,-3489.38,121.999,0,0,0,''),
(7806,0,10,424.137,-3470.95,124.333,0,0,0,''),
(7806,0,11,421.791,-3449.24,119.126,0,0,0,''),
(7806,0,12,404.247,-3429.38,117.644,0,0,0,''),
(7806,0,13,335.465,-3430.72,116.456,0,0,0,''),
(7806,0,14,317.16,-3426.71,116.226,0,0,0,''),
(7806,0,15,331.18,-3464,117.143,0,0,0,''),
(7806,0,16,336.394,-3501.88,118.201,0,0,0,''),
(7806,0,17,337.251,-3544.76,117.284,0,0,0,''),
(7806,0,18,337.749,-3565.42,116.797,0,0,0,''),
(7806,0,19,336.011,-3597.36,118.225,0,0,0,''),
(7806,0,20,324.619,-3622.88,119.811,0,0,0,''),
(7806,0,21,308.027,-3648.6,123.047,0,0,0,''),
(7806,0,22,276.325,-3685.74,128.356,0,0,0,''),
(7806,0,23,239.981,-3717.33,131.874,0,0,0,''),
(7806,0,24,224.951,-3730.17,132.125,0,0,0,''),
(7806,0,25,198.708,-3768.29,129.42,0,0,0,''),
(7806,0,26,183.758,-3791.07,128.045,0,0,0,''),
(7806,0,27,178.111,-3801.58,128.37,0,3000,0,'SAY_OOX_DANGER'),
(7806,0,28,162.215,-3827.01,129.424,0,0,0,''),
(7806,0,29,141.665,-3864.52,131.419,0,0,0,''),
(7806,0,30,135.302,-3880.09,132.12,0,0,0,''),
(7806,0,31,122.461,-3910.07,135.605,0,0,0,''),
(7806,0,32,103.376,-3937.73,137.342,0,0,0,''),
(7806,0,33,81.4145,-3958.61,138.469,0,0,0,''),
(7806,0,34,55.3781,-3982,136.52,0,0,0,''),
(7806,0,35,13.9831,-4013.95,126.903,0,0,0,''),
(7806,0,36,-21.658,-4048.71,118.068,0,0,0,''),
(7806,0,37,-52.4431,-4081.21,117.477,0,0,0,''),
(7806,0,38,-102.711,-4116.76,118.666,0,0,0,''),
(7806,0,39,-92.9962,-4135.85,119.31,0,0,0,''),
(7806,0,40,-86.3913,-4153.33,122.502,0,0,0,''),
(7806,0,41,-85.7461,-4163.6,121.892,0,0,0,''),
(7806,0,42,-90.544,-4183.58,117.587,0,0,0,''),
(7806,0,43,-110.224,-4205.86,121.878,0,0,0,''),
(7806,0,44,-115.258,-4211.96,121.878,0,3000,0,'SAY_OOX_DANGER'),
(7806,0,45,-128.595,-4233.34,117.766,0,0,0,''),
(7806,0,46,-135.359,-4258.12,117.562,0,0,0,''),
(7806,0,47,-156.832,-4258.96,120.059,0,0,0,''),
(7806,0,48,-167.12,-4274.1,117.062,0,0,0,''),
(7806,0,49,-176.291,-4287.59,118.721,0,0,0,''),
(7806,0,50,-196.993,-4315.82,117.588,0,0,0,''),
(7806,0,51,-209.329,-4331.67,115.142,0,0,0,''),
(7806,0,52,-232.292,-4356.02,108.543,0,0,0,''),
(7806,0,53,-232.16,-4370.9,102.815,0,0,0,''),
(7806,0,54,-210.271,-4389.9,84.167,0,0,0,''),
(7806,0,55,-187.94,-4407.53,70.987,0,0,0,''),
(7806,0,56,-181.354,-4418.77,64.778,0,0,0,''),
(7806,0,57,-170.53,-4440.44,58.943,0,0,0,''),
(7806,0,58,-141.429,-4465.32,45.963,0,0,0,''),
(7806,0,59,-120.994,-4487.09,32.075,0,0,0,''),
(7806,0,60,-104.135,-4501.84,25.051,0,0,0,''),
(7806,0,61,-84.1547,-4529.44,11.952,0,0,0,''),
(7806,0,62,-88.6989,-4544.63,9.055,0,0,0,''),
(7806,0,63,-100.603,-4575.03,11.388,0,0,0,''),
(7806,0,64,-106.909,-4600.41,11.046,0,0,0,''),
(7806,0,65,-106.832,-4620.5,11.057,0,3000,0,'SAY_OOX_COMPLETE'),
(7807,0,1,-4943.74,1715.74,62.74,0,0,0,'SAY_START'),
(7807,0,2,-4944.93,1706.66,63.16,0,0,0,''),
(7807,0,3,-4942.82,1690.22,64.25,0,0,0,''),
(7807,0,4,-4946.47,1669.62,63.84,0,0,0,''),
(7807,0,5,-4955.93,1651.88,63,0,0,0,''),
(7807,0,6,-4967.58,1643.86,64.31,0,0,0,''),
(7807,0,7,-4978.12,1607.9,64.3,0,0,0,''),
(7807,0,8,-4975.38,1596.16,64.7,0,0,0,''),
(7807,0,9,-4972.82,1581.89,61.75,0,0,0,''),
(7807,0,10,-4958.65,1581.05,61.81,0,0,0,''),
(7807,0,11,-4936.72,1594.89,65.96,0,0,0,''),
(7807,0,12,-4885.69,1598.1,67.45,0,4000,0,'first ambush SAY_AMBUSH'),
(7807,0,13,-4874.2,1601.73,68.54,0,0,0,''),
(7807,0,14,-4816.64,1594.47,78.2,0,0,0,''),
(7807,0,15,-4802.2,1571.92,87.01,0,0,0,''),
(7807,0,16,-4746.4,1576.11,84.59,0,0,0,''),
(7807,0,17,-4739.72,1707.16,94.04,0,0,0,''),
(7807,0,18,-4674.03,1840.44,89.17,0,0,0,''),
(7807,0,19,-4667.94,1864.11,85.18,0,0,0,''),
(7807,0,20,-4668.08,1886.39,81.14,0,0,0,''),
(7807,0,21,-4679.43,1932.32,73.76,0,0,0,''),
(7807,0,22,-4674.17,1946.66,70.83,0,5000,0,'second ambush SAY_AMBUSH'),
(7807,0,23,-4643.94,1967.45,65.27,0,0,0,''),
(7807,0,24,-4595.6,2010.75,52.1,0,0,0,''),
(7807,0,25,-4562.65,2029.28,45.41,0,0,0,''),
(7807,0,26,-4538.56,2032.65,45.28,0,0,0,''),
(7807,0,27,-4531.96,2034.15,48.34,0,0,0,''),
(7807,0,28,-4507.75,2039.32,51.57,0,0,0,''),
(7807,0,29,-4482.74,2045.67,48.15,0,0,0,''),
(7807,0,30,-4460.87,2051.54,45.55,0,0,0,''),
(7807,0,31,-4449.97,2060.03,45.51,0,10000,0,'third ambush SAY_AMBUSH'),
(7807,0,32,-4448.99,2079.05,44.64,0,0,0,''),
(7807,0,33,-4436.64,2134.48,28.83,0,0,0,''),
(7807,0,34,-4429.25,2170.2,15.44,0,0,0,''),
(7807,0,35,-4424.83,2186.11,11.48,0,0,0,''),
(7807,0,36,-4416.71,2209.76,7.36,0,0,0,''),
(7807,0,37,-4405.25,2231.77,5.94,0,0,0,''),
(7807,0,38,-4377.61,2265.45,6.71,0,15000,0,'complete quest SAY_END'),
(7997,0,1,-4531.98,807.74,59.96,5.584,0,0,'Captured Sprite Darter leaves cage'),
(7997,0,2,-4531.45,804.41,59.76,4.42633,0,0,''),
(7997,0,3,-4536.84,803.49,60.26,3.15948,0,0,''),
(7997,0,4,-4548.07,807.62,60.73,2.63169,0,0,''),
(7997,0,5,-4554.02,815.75,61.37,2.09212,0,0,''),
(7997,0,6,-4558.88,824.21,60.99,2.09212,0,0,''),
(7997,0,7,-4568.32,837.43,59.12,2.05207,0,0,''),
(7997,0,8,-4569.9,841.76,59.74,1.85886,0,0,'Captured Sprite Darter despawn and send kill credit'),
(7997,1,1,-4531.98,807.74,59.96,5.584,0,0,'Captured Sprite Darter leaves cage'),
(7997,1,2,-4528.44,818.16,60.29,1.92405,0,0,''),
(7997,1,3,-4532.99,823.72,60.45,2.57672,0,0,''),
(7997,1,4,-4540.03,826.1,60.51,3.18462,0,0,''),
(7997,1,5,-4548.23,824.09,60.47,3.47914,0,0,''),
(7997,1,6,-4554.04,823.4,61.07,3.04796,0,0,''),
(7997,1,7,-4559.95,827.46,60.28,2.21151,0,0,''),
(7997,1,8,-4568.8,840.24,59.62,2.04422,0,0,'Captured Sprite Darter despawn and send kill credit'),
(7997,2,1,-4531.98,807.74,59.96,5.584,0,0,'Captured Sprite Darter leaves cage'),
(7997,2,2,-4527.93,805.21,59.68,5.51725,0,0,''),
(7997,2,3,-4523.21,801.48,59.55,6.00262,0,0,''),
(7997,2,4,-4517.49,800.73,59.44,0.331263,0,0,''),
(7997,2,5,-4510.99,803.04,60.55,0.489129,0,0,''),
(7997,2,6,-4505.74,805.83,62.06,0.689405,0,0,''),
(7997,2,7,-4502.42,810.73,62.92,1.05462,0,0,''),
(7997,2,8,-4497.04,819.5,63.52,1.00042,0,0,'Captured Sprite Darter despawn and send kill credit'),
(7997,3,1,-4531.98,807.74,59.96,5.584,0,0,'Captured Sprite Darter leaves cage'),
(7997,3,2,-4524.9,798.4,59.44,5.25964,0,0,''),
(7997,3,3,-4518.62,787.26,59.91,4.85437,0,0,''),
(7997,3,4,-4516.15,768.63,60.59,4.73892,0,0,''),
(7997,3,5,-4514.92,755.28,60.38,4.5999,0,0,''),
(7997,3,6,-4515.78,744.25,60.54,4.5999,0,0,''),
(7997,3,7,-4514.1,727.7,62.07,4.82531,0,0,''),
(7997,3,8,-4510.5,696.02,64.68,4.82531,0,0,'Captured Sprite Darter despawn and send kill credit'),
(7998,0,1,-510.13,-132.69,-152.5,0,0,0,''),
(7998,0,2,-511.099,-129.74,-153.845,0,0,0,''),
(7998,0,3,-511.79,-127.476,-155.551,0,0,0,''),
(7998,0,4,-512.969,-124.926,-156.115,0,5000,0,''),
(7998,0,5,-513.972,-120.236,-156.116,0,0,0,''),
(7998,0,6,-514.388,-115.19,-156.117,0,0,0,''),
(7998,0,7,-514.304,-111.478,-155.52,0,0,0,''),
(7998,0,8,-514.84,-107.663,-154.893,0,0,0,''),
(7998,0,9,-518.994,-101.416,-154.648,0,27000,0,''),
(7998,0,10,-526.998,-98.1488,-155.625,0,0,0,''),
(7998,0,11,-534.569,-105.41,-155.989,0,30000,0,''),
(7998,0,12,-535.534,-104.695,-155.971,0,0,0,''),
(7998,0,13,-541.63,-98.6583,-155.858,0,25000,0,''),
(7998,0,14,-535.092,-99.9175,-155.974,0,0,0,''),
(7998,0,15,-519.01,-101.51,-154.677,0,3000,0,''),
(7998,0,16,-504.466,-97.848,-150.955,0,30000,0,''),
(7998,0,17,-506.907,-89.1474,-151.083,0,23000,0,''),
(7998,0,18,-512.758,-101.902,-153.198,0,0,0,''),
(7998,0,19,-519.988,-124.848,-156.128,0,86400000,0,'this npc should not reset on wp end'),
(8284,0,1,-7007.21,-1749.16,234.182,0,3000,0,'stand up'),
(8284,0,2,-7007.32,-1729.85,234.162,0,0,0,''),
(8284,0,3,-7006.39,-1726.52,234.099,0,0,0,''),
(8284,0,4,-7003.26,-1726.9,234.594,0,0,0,''),
(8284,0,5,-6994.78,-1733.57,238.281,0,0,0,''),
(8284,0,6,-6987.9,-1735.94,240.727,0,0,0,''),
(8284,0,7,-6978.7,-1736.99,241.809,0,0,0,''),
(8284,0,8,-6964.26,-1740.25,241.713,0,0,0,''),
(8284,0,9,-6946.7,-1746.28,241.667,0,0,0,''),
(8284,0,10,-6938.75,-1749.38,240.744,0,0,0,''),
(8284,0,11,-6927,-1768.78,240.744,0,0,0,''),
(8284,0,12,-6909.45,-1791.26,240.744,0,0,0,''),
(8284,0,13,-6898.23,-1804.87,240.744,0,0,0,''),
(8284,0,14,-6881.28,-1821.79,240.744,0,0,0,''),
(8284,0,15,-6867.65,-1832.67,240.706,0,0,0,''),
(8284,0,16,-6850.18,-1839.25,243.006,0,0,0,''),
(8284,0,17,-6829.38,-1847.64,244.19,0,0,0,''),
(8284,0,18,-6804.62,-1857.54,244.209,0,0,0,''),
(8284,0,19,-6776.42,-1868.88,244.142,0,0,0,''),
(8284,0,20,-6753.47,-1876.91,244.17,0,10000,0,'stop'),
(8284,0,21,-6753.47,-1876.91,244.17,0,0,0,'ambush'),
(8284,0,22,-6731.03,-1884.94,244.144,0,0,0,''),
(8284,0,23,-6705.74,-1896.78,244.144,0,0,0,''),
(8284,0,24,-6678.96,-1909.61,244.369,0,0,0,''),
(8284,0,25,-6654.26,-1916.76,244.145,0,0,0,''),
(8284,0,26,-6620.6,-1917.61,244.149,0,0,0,''),
(8284,0,27,-6575.96,-1922.41,244.149,0,0,0,''),
(8284,0,28,-6554.81,-1929.88,244.162,0,0,0,''),
(8284,0,29,-6521.86,-1947.32,244.151,0,0,0,''),
(8284,0,30,-6493.32,-1962.65,244.151,0,0,0,''),
(8284,0,31,-6463.35,-1975.54,244.213,0,0,0,''),
(8284,0,32,-6435.43,-1983.85,244.548,0,0,0,''),
(8284,0,33,-6418.38,-1985.78,246.554,0,0,0,''),
(8284,0,34,-6389.78,-1986.54,246.771,0,30000,0,'quest complete'),
(8516,0,1,2603.18,725.259,54.6927,0,0,0,''),
(8516,0,2,2587.13,734.392,55.231,0,0,0,''),
(8516,0,3,2570.69,753.572,54.5855,0,0,0,''),
(8516,0,4,2558.51,747.66,54.4482,0,0,0,''),
(8516,0,5,2544.23,772.924,47.9255,0,0,0,''),
(8516,0,6,2530.08,797.475,45.97,0,0,0,''),
(8516,0,7,2521.83,799.127,44.3061,0,0,0,''),
(8516,0,8,2502.61,789.222,39.5074,0,0,0,''),
(8516,0,9,2495.25,789.406,39.499,0,0,0,''),
(8516,0,10,2488.07,802.455,42.9834,0,0,0,''),
(8516,0,11,2486.64,826.649,43.6363,0,0,0,''),
(8516,0,12,2492.64,835.166,45.1427,0,0,0,''),
(8516,0,13,2505.02,847.564,47.6487,0,0,0,''),
(8516,0,14,2538.96,877.362,47.6781,0,0,0,''),
(8516,0,15,2546.07,885.672,47.6789,0,0,0,''),
(8516,0,16,2548.02,897.584,47.7277,0,0,0,''),
(8516,0,17,2544.29,909.116,46.2506,0,0,0,''),
(8516,0,18,2523.6,920.306,45.8717,0,0,0,''),
(8516,0,19,2522.69,933.546,47.5769,0,0,0,''),
(8516,0,20,2531.63,959.893,49.4111,0,0,0,''),
(8516,0,21,2540.23,973.338,50.1241,0,0,0,''),
(8516,0,22,2547.21,977.489,49.9759,0,0,0,''),
(8516,0,23,2558.75,969.243,50.7353,0,0,0,''),
(8516,0,24,2575.6,950.138,52.846,0,0,0,''),
(8516,0,25,2575.6,950.138,52.846,0,0,0,''),
(9023,0,1,316.336,-225.528,-77.7258,0,2000,0,'SAY_WINDSOR_START'),
(9023,0,2,322.96,-207.13,-77.87,0,0,0,''),
(9023,0,3,281.05,-172.16,-75.12,0,0,0,''),
(9023,0,4,272.19,-139.14,-70.61,0,0,0,''),
(9023,0,5,283.62,-116.09,-70.21,0,0,0,''),
(9023,0,6,296.18,-94.3,-74.08,0,0,0,''),
(9023,0,7,294.57,-93.11,-74.08,0,0,0,'escort paused - SAY_WINDSOR_CELL_DUGHAL_1'),
(9023,0,8,294.57,-93.11,-74.08,0,10000,0,''),
(9023,0,9,294.57,-93.11,-74.08,0,3000,0,'SAY_WINDSOR_CELL_DUGHAL_3'),
(9023,0,10,314.31,-74.31,-76.09,0,0,0,''),
(9023,0,11,360.22,-62.93,-66.77,0,0,0,''),
(9023,0,12,383.38,-69.4,-63.25,0,0,0,''),
(9023,0,13,389.99,-67.86,-62.57,0,0,0,''),
(9023,0,14,400.98,-72.01,-62.31,0,0,0,'SAY_WINDSOR_EQUIPMENT_1'),
(9023,0,15,404.22,-62.3,-63.5,0,2000,0,''),
(9023,0,16,404.22,-62.3,-63.5,0,1500,0,'open supply door'),
(9023,0,17,407.65,-51.86,-63.96,0,0,0,''),
(9023,0,18,403.61,-51.71,-63.92,0,1000,0,'SAY_WINDSOR_EQUIPMENT_2'),
(9023,0,19,403.61,-51.71,-63.92,0,2000,0,''),
(9023,0,20,403.61,-51.71,-63.92,0,1000,0,'open supply crate'),
(9023,0,21,403.61,-51.71,-63.92,0,1000,0,'update entry to Reginald Windsor'),
(9023,0,22,403.61,-52.71,-63.92,0,4000,0,'SAY_WINDSOR_EQUIPMENT_3'),
(9023,0,23,403.61,-52.71,-63.92,0,4000,0,'SAY_WINDSOR_EQUIPMENT_4'),
(9023,0,24,406.33,-54.87,-63.95,0,0,0,''),
(9023,0,25,403.86,-73.88,-62.02,0,0,0,''),
(9023,0,26,428.8,-81.34,-64.91,0,0,0,''),
(9023,0,27,557.03,-119.71,-61.83,0,0,0,''),
(9023,0,28,573.4,-124.39,-65.07,0,0,0,''),
(9023,0,29,593.91,-130.29,-69.25,0,0,0,''),
(9023,0,30,593.21,-132.16,-69.25,0,0,0,'escort paused - SAY_WINDSOR_CELL_JAZ_1'),
(9023,0,31,593.21,-132.16,-69.25,0,1000,0,''),
(9023,0,32,593.21,-132.16,-69.25,0,3000,0,'SAY_WINDSOR_CELL_JAZ_2'),
(9023,0,33,622.81,-135.55,-71.92,0,0,0,''),
(9023,0,34,634.68,-151.29,-70.32,0,0,0,''),
(9023,0,35,635.06,-153.25,-70.32,0,0,0,'escort paused - SAY_WINDSOR_CELL_SHILL_1'),
(9023,0,36,635.06,-153.25,-70.32,0,3000,0,''),
(9023,0,37,635.06,-153.25,-70.32,0,5000,0,'SAY_WINDSOR_CELL_SHILL_2'),
(9023,0,38,635.06,-153.25,-70.32,0,2000,0,'SAY_WINDSOR_CELL_SHILL_3'),
(9023,0,39,655.25,-172.39,-73.72,0,0,0,''),
(9023,0,40,654.79,-226.3,-83.06,0,0,0,''),
(9023,0,41,622.85,-268.85,-83.96,0,0,0,''),
(9023,0,42,579.45,-275.56,-80.44,0,0,0,''),
(9023,0,43,561.19,-266.85,-75.59,0,0,0,''),
(9023,0,44,547.91,-253.92,-70.34,0,0,0,''),
(9023,0,45,549.2,-252.4,-70.34,0,0,0,'escort paused - SAY_WINDSOR_CELL_CREST_1'),
(9023,0,46,549.2,-252.4,-70.34,0,1000,0,''),
(9023,0,47,549.2,-252.4,-70.34,0,4000,0,'SAY_WINDSOR_CELL_CREST_2'),
(9023,0,48,555.33,-269.16,-74.4,0,0,0,''),
(9023,0,49,554.31,-270.88,-74.4,0,0,0,'escort paused - SAY_WINDSOR_CELL_TOBIAS_1'),
(9023,0,50,554.31,-270.88,-74.4,0,10000,0,''),
(9023,0,51,554.31,-270.88,-74.4,0,4000,0,'SAY_WINDSOR_CELL_TOBIAS_2'),
(9023,0,52,536.1,-249.6,-67.47,0,0,0,''),
(9023,0,53,520.94,-216.65,-59.28,0,0,0,''),
(9023,0,54,505.99,-148.74,-62.17,0,0,0,''),
(9023,0,55,484.21,-56.24,-62.43,0,0,0,''),
(9023,0,56,470.39,-6.01,-70.1,0,0,0,''),
(9023,0,57,452.45,29.85,-70.37,0,1500,0,'SAY_WINDSOR_FREE_1'),
(9023,0,58,452.45,29.85,-70.37,0,15000,0,'SAY_WINDSOR_FREE_2'),
(9502,0,1,847.848,-230.067,-43.614,0,0,0,''),
(9502,0,2,868.122,-223.884,-43.695,0,0,0,'YELL_PHALANX_AGGRO'),
(9503,0,1,885.185,-194.007,-43.4584,0,0,0,''),
(9503,0,2,885.185,-194.007,-43.4584,0,0,0,''),
(9503,0,3,872.764,-185.606,-43.7037,0,5000,0,'b1'),
(9503,0,4,867.923,-188.006,-43.7037,0,5000,0,'b2'),
(9503,0,5,863.296,-190.795,-43.7037,0,5000,0,'b3'),
(9503,0,6,856.14,-194.653,-43.7037,0,5000,0,'b4'),
(9503,0,7,851.879,-196.928,-43.7037,0,15000,0,'b5'),
(9503,0,8,877.035,-187.048,-43.7037,0,0,0,''),
(9503,0,9,891.198,-197.924,-43.6204,0,0,0,'home'),
(9503,0,10,876.935,-189.007,-43.4584,0,0,0,'Nagmara escort'),
(9503,0,11,885.185,-194.007,-43.4584,0,0,0,''),
(9503,0,12,869.124,-202.852,-43.7088,0,0,0,''),
(9503,0,13,869.465,-202.878,-43.4588,0,0,0,''),
(9503,0,14,864.244,-210.826,-43.459,0,0,0,''),
(9503,0,15,866.824,-220.959,-43.4472,0,0,0,''),
(9503,0,16,867.074,-221.959,-43.4472,0,0,0,''),
(9503,0,17,870.419,-225.675,-43.5566,0,0,0,'open door'),
(9503,0,18,872.169,-227.425,-43.5566,0,0,0,''),
(9503,0,19,872.919,-228.175,-43.5566,0,0,0,''),
(9503,0,20,875.919,-230.925,-43.5566,0,0,0,''),
(9503,0,21,876.919,-230.175,-43.5566,0,0,0,''),
(9503,0,22,877.919,-229.425,-43.5566,0,0,0,''),
(9503,0,23,882.395,-225.949,-46.7405,0,0,0,''),
(9503,0,24,885.895,-223.699,-49.2405,0,0,0,''),
(9503,0,25,887.645,-222.449,-49.2405,0,0,0,''),
(9503,0,26,885.937,-223.351,-49.2954,0,0,0,''),
(9503,0,27,887.437,-222.351,-49.2954,0,0,0,''),
(9503,0,28,888.937,-221.601,-49.5454,0,0,0,''),
(9503,0,29,887.687,-220.101,-49.5454,0,0,0,''),
(9503,0,30,886.687,-218.851,-49.5454,0,0,0,''),
(9503,0,31,887.567,-220.04,-49.7059,0,0,0,''),
(9503,0,32,886.567,-218.79,-49.7059,0,0,0,''),
(9503,0,33,886.067,-218.29,-49.7059,0,0,0,''),
(9503,0,34,880.825,-221.389,-49.9562,0,0,0,'stop'),
(9520,0,1,-7699.62,-1444.29,139.87,0,4000,0,'SAY_START'),
(9520,0,2,-7670.67,-1458.25,140.74,0,0,0,''),
(9520,0,3,-7675.26,-1465.58,140.74,0,0,0,''),
(9520,0,4,-7685.84,-1472.66,140.75,0,0,0,''),
(9520,0,5,-7700.08,-1473.41,140.79,0,0,0,''),
(9520,0,6,-7712.55,-1470.19,140.79,0,0,0,''),
(9520,0,7,-7717.27,-1481.7,140.72,0,5000,0,'SAY_PAY'),
(9520,0,8,-7726.23,-1500.78,132.99,0,0,0,''),
(9520,0,9,-7744.61,-1531.61,132.69,0,0,0,''),
(9520,0,10,-7763.08,-1536.22,131.93,0,0,0,''),
(9520,0,11,-7815.32,-1522.61,134.16,0,0,0,''),
(9520,0,12,-7850.26,-1516.87,138.17,0,0,0,'SAY_FIRST_AMBUSH_START'),
(9520,0,13,-7850.26,-1516.87,138.17,0,3000,0,'SAY_FIRST_AMBUSH_END'),
(9520,0,14,-7881.01,-1508.49,142.37,0,0,0,''),
(9520,0,15,-7888.91,-1458.09,144.79,0,0,0,''),
(9520,0,16,-7889.18,-1430.21,145.31,0,0,0,''),
(9520,0,17,-7900.53,-1427.01,150.26,0,0,0,''),
(9520,0,18,-7904.15,-1429.91,150.27,0,0,0,''),
(9520,0,19,-7921.48,-1425.47,140.54,0,0,0,''),
(9520,0,20,-7941.43,-1413.1,134.35,0,0,0,''),
(9520,0,21,-7964.85,-1367.45,132.99,0,0,0,''),
(9520,0,22,-7989.95,-1319.12,133.71,0,0,0,''),
(9520,0,23,-8010.43,-1270.23,133.42,0,0,0,''),
(9520,0,24,-8025.62,-1243.78,133.91,0,0,0,'SAY_SEC_AMBUSH_START'),
(9520,0,25,-8025.62,-1243.78,133.91,0,3000,0,'SAY_SEC_AMBUSH_END'),
(9520,0,26,-8015.22,-1196.98,146.76,0,0,0,''),
(9520,0,27,-7994.68,-1151.38,160.7,0,0,0,''),
(9520,0,28,-7970.91,-1132.81,170.16,0,0,0,'summon Searscale Drakes'),
(9520,0,29,-7927.59,-1122.79,185.86,0,0,0,''),
(9520,0,30,-7897.67,-1126.67,194.32,0,0,0,'SAY_THIRD_AMBUSH_START'),
(9520,0,31,-7897.67,-1126.67,194.32,0,3000,0,'SAY_THIRD_AMBUSH_END'),
(9520,0,32,-7864.11,-1135.98,203.29,0,0,0,''),
(9520,0,33,-7837.31,-1137.73,209.63,0,0,0,''),
(9520,0,34,-7808.72,-1134.9,214.84,0,0,0,''),
(9520,0,35,-7786.85,-1127.24,214.84,0,0,0,''),
(9520,0,36,-7746.58,-1125.16,215.08,0,5000,0,'EMOTE_LAUGH'),
(9520,0,37,-7746.41,-1103.62,215.62,0,0,0,''),
(9520,0,38,-7740.25,-1090.51,216.69,0,0,0,''),
(9520,0,39,-7730.97,-1085.55,217.12,0,0,0,''),
(9520,0,40,-7697.89,-1089.43,217.62,0,0,0,''),
(9520,0,41,-7679.3,-1059.15,220.09,0,0,0,''),
(9520,0,42,-7661.39,-1038.24,226.24,0,0,0,''),
(9520,0,43,-7634.49,-1020.96,234.3,0,0,0,''),
(9520,0,44,-7596.22,-1013.16,244.03,0,0,0,''),
(9520,0,45,-7556.53,-1021.74,253.21,0,0,0,'SAY_LAST_STAND'),
(9537,0,1,854.977,-150.308,-49.671,0,0,0,''),
(9537,0,2,855.816,-149.763,-49.671,0,0,0,'YELL_HURLEY_SPAWN'),
(9537,0,3,882.63,-148.166,-49.7597,0,0,0,''),
(9537,0,4,883.96,-148.087,-49.76,0,0,0,''),
(9537,0,5,896.846,-147.319,-49.7575,0,0,0,''),
(9537,0,6,896.846,-147.319,-49.7575,0,0,0,''),
(9598,0,1,6004.27,-1180.49,376.377,0,0,0,'SAY_ESCORT_START'),
(9598,0,2,6002.51,-1157.29,381.407,0,0,0,''),
(9598,0,3,6029.23,-1139.72,383.127,0,0,0,''),
(9598,0,4,6042.48,-1128.96,386.582,0,0,0,''),
(9598,0,5,6062.82,-1115.52,386.85,0,0,0,''),
(9598,0,6,6089.19,-1111.91,383.105,0,0,0,''),
(9598,0,7,6104.39,-1114.56,380.49,0,0,0,''),
(9598,0,8,6115.08,-1128.57,375.779,0,0,0,''),
(9598,0,9,6119.35,-1147.31,372.518,0,0,0,''),
(9598,0,10,6119,-1176.08,371.072,0,0,0,''),
(9598,0,11,6120.98,-1198.41,373.432,0,0,0,''),
(9598,0,12,6123.52,-1226.64,374.119,0,0,0,''),
(9598,0,13,6127.74,-1246.04,373.574,0,0,0,''),
(9598,0,14,6133.43,-1253.64,369.1,0,0,0,''),
(9598,0,15,6150.79,-1269.15,369.24,0,0,0,''),
(9598,0,16,6155.81,-1275.03,373.627,0,0,0,''),
(9598,0,17,6163.54,-1307.13,376.234,0,0,0,''),
(9598,0,18,6174.8,-1340.89,379.039,0,0,0,''),
(9598,0,19,6191.14,-1371.26,378.453,0,0,0,''),
(9598,0,20,6215.65,-1397.52,376.012,0,0,0,''),
(9598,0,21,6243.78,-1407.68,371.594,0,0,0,''),
(9598,0,22,6280.77,-1408.26,370.554,0,0,0,''),
(9598,0,23,6325.36,-1406.69,370.082,0,0,0,''),
(9598,0,24,6355,-1404.34,370.646,0,0,0,''),
(9598,0,25,6374.68,-1399.18,372.105,0,0,0,''),
(9598,0,26,6395.8,-1367.06,374.91,0,0,0,''),
(9598,0,27,6408.57,-1333.49,376.616,0,0,0,''),
(9598,0,28,6409.08,-1312.17,379.598,0,0,0,''),
(9598,0,29,6418.69,-1277.7,381.638,0,0,0,''),
(9598,0,30,6441.69,-1247.32,387.246,0,0,0,''),
(9598,0,31,6462.14,-1226.32,397.61,0,0,0,''),
(9598,0,32,6478.02,-1216.26,408.284,0,0,0,''),
(9598,0,33,6499.63,-1217.09,419.461,0,0,0,''),
(9598,0,34,6523.17,-1220.78,430.549,0,0,0,''),
(9598,0,35,6542.72,-1217,437.788,0,0,0,''),
(9598,0,36,6557.28,-1211.12,441.452,0,0,0,''),
(9598,0,37,6574.57,-1204.59,443.216,0,0,0,'SAY_EXIT_IRONTREE'),
(9623,0,1,-6383.07,-1964.37,-258.709,0,0,0,'SAY_AME_START'),
(9623,0,2,-6393.65,-1949.57,-261.449,0,0,0,''),
(9623,0,3,-6397.85,-1931.1,-263.366,0,0,0,''),
(9623,0,4,-6397.5,-1921.47,-263.876,0,0,0,''),
(9623,0,5,-6389.63,-1910,-259.601,0,0,0,''),
(9623,0,6,-6380.07,-1905.45,-255.858,0,0,0,''),
(9623,0,7,-6373.44,-1900.28,-254.774,0,0,0,''),
(9623,0,8,-6372.87,-1893.5,-255.678,0,0,0,''),
(9623,0,9,-6379.73,-1877.63,-259.654,0,0,0,''),
(9623,0,10,-6380.26,-1871.14,-260.617,0,0,0,''),
(9623,0,11,-6373.83,-1855.62,-259.566,0,0,0,''),
(9623,0,12,-6368.82,-1847.77,-259.246,0,0,0,''),
(9623,0,13,-6370.9,-1835.04,-260.212,0,0,0,''),
(9623,0,14,-6376.59,-1821.59,-260.856,0,0,0,''),
(9623,0,15,-6381.93,-1810.43,-266.18,0,0,0,''),
(9623,0,16,-6396.71,-1807.12,-269.329,0,0,0,''),
(9623,0,17,-6400.27,-1795.05,-269.744,0,0,0,''),
(9623,0,18,-6402.68,-1747.51,-272.961,0,0,0,''),
(9623,0,19,-6397,-1710.05,-273.719,0,0,0,''),
(9623,0,20,-6388.11,-1676.33,-272.133,0,5000,0,'SAY_AME_PROGRESS'),
(9623,0,21,-6370.71,-1638.64,-272.031,0,0,0,''),
(9623,0,22,-6366.71,-1592.65,-272.201,0,0,0,''),
(9623,0,23,-6333.87,-1534.6,-270.493,0,0,0,''),
(9623,0,24,-6305.36,-1477.91,-269.518,0,0,0,''),
(9623,0,25,-6311.59,-1419.02,-267.622,0,0,0,''),
(9623,0,26,-6330.01,-1400.06,-266.425,0,0,0,''),
(9623,0,27,-6356.02,-1392.61,-267.123,0,0,0,''),
(9623,0,28,-6370.86,-1386.18,-270.218,0,0,0,''),
(9623,0,29,-6381.53,-1369.78,-272.11,0,0,0,''),
(9623,0,30,-6405.38,-1321.52,-271.699,0,0,0,''),
(9623,0,31,-6406.58,-1307.57,-271.802,0,0,0,''),
(9623,0,32,-6386.33,-1286.85,-272.074,0,0,0,''),
(9623,0,33,-6364.25,-1264.71,-269.075,0,0,0,''),
(9623,0,34,-6343.64,-1239.84,-268.364,0,0,0,''),
(9623,0,35,-6335.57,-1202.45,-271.515,0,0,0,''),
(9623,0,36,-6325.62,-1184.46,-270.461,0,0,0,''),
(9623,0,37,-6317.8,-1177.67,-269.792,0,0,0,''),
(9623,0,38,-6303.02,-1180.25,-269.332,0,0,0,'SAY_AME_END'),
(9623,0,39,-6301.98,-1184.79,-269.371,0,1000,0,''),
(9623,0,40,-6297.58,-1186.41,-268.962,0,5000,0,''),
(10096,0,1,604.803,-191.082,-54.0586,0,0,0,'ring'),
(10096,0,2,604.073,-222.107,-52.7438,0,0,0,'first gate'),
(10096,0,3,621.4,-214.499,-52.8145,0,0,0,'hiding in corner'),
(10096,0,4,601.301,-198.557,-53.9503,0,0,0,'ring'),
(10096,0,5,631.818,-180.548,-52.6548,0,0,0,'second gate'),
(10096,0,6,627.39,-201.076,-52.6929,0,0,0,'hiding in corner'),
(10300,0,1,5728.81,-4801.15,778.18,0,0,0,''),
(10300,0,2,5730.22,-4818.34,777.11,0,0,0,''),
(10300,0,3,5728.12,-4835.76,778.15,0,1000,0,'SAY_ENTER_OWL_THICKET'),
(10300,0,4,5718.85,-4865.62,787.56,0,0,0,''),
(10300,0,5,5697.13,-4909.12,801.53,0,0,0,''),
(10300,0,6,5684.2,-4913.75,801.6,0,0,0,''),
(10300,0,7,5674.67,-4915.78,802.13,0,0,0,''),
(10300,0,8,5665.61,-4919.22,804.85,0,0,0,''),
(10300,0,9,5638.22,-4897.58,804.97,0,0,0,''),
(10300,0,10,5632.67,-4892.05,805.44,0,0,0,'Cavern 1 - EMOTE_CHANT_SPELL'),
(10300,0,11,5664.58,-4921.84,804.91,0,0,0,''),
(10300,0,12,5684.21,-4943.81,802.8,0,0,0,''),
(10300,0,13,5724.92,-4983.69,808.25,0,0,0,''),
(10300,0,14,5753.39,-4990.73,809.84,0,0,0,''),
(10300,0,15,5765.62,-4994.89,809.42,0,0,0,'Cavern 2 - EMOTE_CHANT_SPELL'),
(10300,0,16,5724.94,-4983.58,808.29,0,0,0,''),
(10300,0,17,5699.61,-4989.82,808.03,0,0,0,''),
(10300,0,18,5686.8,-5012.17,807.27,0,0,0,''),
(10300,0,19,5691.43,-5037.43,807.73,0,0,0,''),
(10300,0,20,5694.24,-5054.64,808.85,0,0,0,'Cavern 3 - EMOTE_CHANT_SPELL'),
(10300,0,21,5686.88,-5012.18,807.23,0,0,0,''),
(10300,0,22,5664.94,-5001.12,807.78,0,0,0,''),
(10300,0,23,5647.12,-5002.84,807.54,0,0,0,''),
(10300,0,24,5629.23,-5014.88,807.94,0,0,0,''),
(10300,0,25,5611.26,-5025.62,808.36,0,0,0,'Cavern 4 - EMOTE_CHANT_SPELL'),
(10300,0,26,5647.13,-5002.85,807.57,0,0,0,''),
(10300,0,27,5641.12,-4973.22,809.39,0,0,0,''),
(10300,0,28,5622.97,-4953.58,811.12,0,0,0,''),
(10300,0,29,5601.52,-4939.49,820.77,0,0,0,''),
(10300,0,30,5571.87,-4936.22,831.35,0,0,0,''),
(10300,0,31,5543.23,-4933.67,838.33,0,0,0,''),
(10300,0,32,5520.86,-4942.05,843.02,0,0,0,''),
(10300,0,33,5509.15,-4946.31,849.36,0,0,0,''),
(10300,0,34,5498.45,-4950.08,849.98,0,0,0,''),
(10300,0,35,5485.78,-4963.4,850.43,0,0,0,''),
(10300,0,36,5467.92,-4980.67,851.89,0,0,0,'Cavern 5 - EMOTE_CHANT_SPELL'),
(10300,0,37,5498.68,-4950.45,849.96,0,0,0,''),
(10300,0,38,5518.68,-4921.94,844.65,0,0,0,''),
(10300,0,39,5517.66,-4920.82,845.12,0,0,0,'SAY_REACH_ALTAR_1'),
(10300,0,40,5518.38,-4913.47,845.57,0,0,0,''),
(10300,0,41,5511.31,-4913.82,847.17,0,5000,0,'light the spotlights'),
(10300,0,42,5511.31,-4913.82,847.17,0,0,0,'start altar cinematic - SAY_RANSHALLA_ALTAR_2'),
(10300,0,43,5510.36,-4921.17,846.33,0,0,0,''),
(10300,0,44,5517.66,-4920.82,845.12,0,0,0,'escort paused'),
(10427,0,1,-5185.46,-1185.93,45.951,0,0,0,''),
(10427,0,2,-5184.88,-1154.21,45.035,0,0,0,''),
(10427,0,3,-5175.88,-1126.53,43.701,0,0,0,''),
(10427,0,4,-5138.65,-1111.87,44.024,0,0,0,''),
(10427,0,5,-5134.73,-1104.8,47.365,0,0,0,''),
(10427,0,6,-5129.68,-1097.88,49.449,0,2500,0,''),
(10427,0,7,-5125.3,-1080.57,47.033,0,0,0,''),
(10427,0,8,-5146.67,-1053.69,28.415,0,0,0,''),
(10427,0,9,-5147.46,-1027.54,13.818,0,0,0,''),
(10427,0,10,-5139.24,-1018.89,8.22,0,0,0,''),
(10427,0,11,-5121.17,-1013.13,-0.619,0,0,0,''),
(10427,0,12,-5091.92,-1014.21,-4.902,0,0,0,''),
(10427,0,13,-5069.24,-994.299,-4.631,0,0,0,''),
(10427,0,14,-5059.98,-944.112,-5.377,0,0,0,''),
(10427,0,15,-5013.55,-906.184,-5.49,0,0,0,''),
(10427,0,16,-4992.46,-920.983,-4.98,0,5000,0,'SAY_WYVERN'),
(10427,0,17,-4976.35,-1003,-5.38,0,0,0,''),
(10427,0,18,-4958.48,-1033.19,-5.433,0,0,0,''),
(10427,0,19,-4953.35,-1052.21,-10.836,0,0,0,''),
(10427,0,20,-4937.45,-1056.35,-22.139,0,0,0,''),
(10427,0,21,-4908.46,-1050.43,-33.458,0,0,0,''),
(10427,0,22,-4905.53,-1056.89,-33.722,0,0,0,''),
(10427,0,23,-4920.83,-1073.28,-45.515,0,0,0,''),
(10427,0,24,-4933.37,-1082.7,-50.186,0,0,0,''),
(10427,0,25,-4935.31,-1092.35,-52.785,0,0,0,''),
(10427,0,26,-4929.55,-1101.27,-50.637,0,0,0,''),
(10427,0,27,-4920.68,-1100.03,-51.944,0,10000,0,'SAY_COMPLETE'),
(10427,0,28,-4920.68,-1100.03,-51.944,0,0,0,'quest complete'),
(10646,0,1,-4792.4,-2137.78,82.423,0,0,0,''),
(10646,0,2,-4813.51,-2141.54,80.774,0,0,0,''),
(10646,0,3,-4828.63,-2154.31,82.074,0,0,0,''),
(10646,0,4,-4833.77,-2149.18,81.676,0,0,0,''),
(10646,0,5,-4846.42,-2136.05,77.871,0,0,0,''),
(10646,0,6,-4865.08,-2116.55,76.483,0,0,0,''),
(10646,0,7,-4888.43,-2090.73,80.907,0,0,0,''),
(10646,0,8,-4893.07,-2085.47,82.094,0,0,0,''),
(10646,0,9,-4907.26,-2074.93,84.437,0,5000,0,'SAY_LAKO_LOOK_OUT'),
(10646,0,10,-4899.9,-2062.14,83.78,0,0,0,''),
(10646,0,11,-4897.76,-2056.52,84.184,0,0,0,''),
(10646,0,12,-4888.33,-2033.18,83.654,0,0,0,''),
(10646,0,13,-4876.34,-2003.92,90.887,0,0,0,''),
(10646,0,14,-4872.23,-1994.17,91.513,0,0,0,''),
(10646,0,15,-4879.57,-1976.99,92.185,0,5000,0,'SAY_LAKO_HERE_COME'),
(10646,0,16,-4879.05,-1964.35,92.001,0,0,0,''),
(10646,0,17,-4874.72,-1956.94,90.737,0,0,0,''),
(10646,0,18,-4869.47,-1952.61,89.206,0,0,0,''),
(10646,0,19,-4842.47,-1929,84.147,0,0,0,''),
(10646,0,20,-4804.44,-1897.3,89.362,0,0,0,''),
(10646,0,21,-4798.07,-1892.38,89.368,0,0,0,''),
(10646,0,22,-4779.45,-1882.76,90.169,0,5000,0,'SAY_LAKO_MORE'),
(10646,0,23,-4762.08,-1866.53,89.481,0,0,0,''),
(10646,0,24,-4766.27,-1861.87,87.847,0,0,0,''),
(10646,0,25,-4782.93,-1852.17,78.354,0,0,0,''),
(10646,0,26,-4793.61,-1850.96,77.658,0,0,0,''),
(10646,0,27,-4803.32,-1855.1,78.958,0,0,0,''),
(10646,0,28,-4807.97,-1854.5,77.743,0,0,0,''),
(10646,0,29,-4837.21,-1848.49,64.488,0,0,0,''),
(10646,0,30,-4884.62,-1840.4,56.219,0,0,0,''),
(10646,0,31,-4889.71,-1839.62,54.417,0,0,0,''),
(10646,0,32,-4893.9,-1843.69,53.012,0,0,0,''),
(10646,0,33,-4903.14,-1872.38,32.266,0,0,0,''),
(10646,0,34,-4910.94,-1879.86,29.94,0,0,0,''),
(10646,0,35,-4920.05,-1880.94,30.597,0,0,0,''),
(10646,0,36,-4924.46,-1881.45,29.292,0,0,0,''),
(10646,0,37,-4966.12,-1886.03,10.977,0,0,0,''),
(10646,0,38,-4999.37,-1890.85,4.43,0,0,0,''),
(10646,0,39,-5007.27,-1891.67,2.771,0,0,0,''),
(10646,0,40,-5013.33,-1879.59,-1.947,0,0,0,''),
(10646,0,41,-5023.33,-1855.96,-17.103,0,0,0,''),
(10646,0,42,-5038.51,-1825.99,-35.821,0,0,0,''),
(10646,0,43,-5048.73,-1809.8,-46.457,0,0,0,''),
(10646,0,44,-5053.19,-1791.68,-57.186,0,0,0,''),
(10646,0,45,-5062.09,-1794.4,-56.515,0,0,0,''),
(10646,0,46,-5052.66,-1797.04,-54.734,0,5000,0,'SAY_LAKO_END'),
(11016,0,1,5004.98,-440.237,319.059,0,4000,0,'SAY_ESCORT_START'),
(11016,0,2,4992.22,-449.964,317.057,0,0,0,''),
(11016,0,3,4988.55,-457.438,316.289,0,0,0,''),
(11016,0,4,4989.98,-464.297,316.846,0,0,0,''),
(11016,0,5,4994.04,-467.754,318.055,0,0,0,''),
(11016,0,6,5002.31,-466.318,319.965,0,0,0,''),
(11016,0,7,5011.8,-462.889,321.501,0,0,0,''),
(11016,0,8,5020.53,-460.797,321.97,0,0,0,''),
(11016,0,9,5026.84,-463.171,321.345,0,0,0,''),
(11016,0,10,5028.66,-476.805,318.726,0,0,0,''),
(11016,0,11,5029.5,-487.131,318.179,0,0,0,''),
(11016,0,12,5031.18,-497.678,316.533,0,0,0,''),
(11016,0,13,5032.72,-504.748,314.744,0,0,0,''),
(11016,0,14,5035,-513.138,314.372,0,0,0,''),
(11016,0,15,5037.49,-521.733,313.221,0,6000,0,'SAY_FIRST_STOP'),
(11016,0,16,5049.06,-519.546,313.221,0,0,0,''),
(11016,0,17,5059.17,-522.93,313.221,0,0,0,''),
(11016,0,18,5062.75,-529.933,313.221,0,0,0,''),
(11016,0,19,5063.9,-538.827,313.221,0,0,0,''),
(11016,0,20,5062.22,-545.635,313.221,0,0,0,''),
(11016,0,21,5061.69,-552.333,313.101,0,0,0,''),
(11016,0,22,5060.33,-560.349,310.873,0,0,0,''),
(11016,0,23,5055.62,-565.541,308.737,0,0,0,''),
(11016,0,24,5049.8,-567.604,306.537,0,0,0,''),
(11016,0,25,5043.01,-564.946,303.682,0,0,0,''),
(11016,0,26,5038.22,-559.823,301.463,0,0,0,''),
(11016,0,27,5039.46,-548.675,297.824,0,0,0,''),
(11016,0,28,5043.44,-538.807,297.801,0,0,0,''),
(11016,0,29,5056.4,-528.954,297.801,0,0,0,''),
(11016,0,30,5064.4,-521.904,297.801,0,0,0,''),
(11016,0,31,5067.62,-512.999,297.196,0,0,0,''),
(11016,0,32,5065.99,-505.329,297.214,0,0,0,''),
(11016,0,33,5062.24,-499.086,297.448,0,0,0,''),
(11016,0,34,5065.09,-492.069,298.054,0,0,0,''),
(11016,0,35,5071.19,-491.173,297.666,0,5000,0,'SAY_SECOND_STOP'),
(11016,0,36,5087.47,-496.478,296.677,0,0,0,''),
(11016,0,37,5095.55,-508.639,296.677,0,0,0,''),
(11016,0,38,5104.3,-521.014,296.677,0,0,0,''),
(11016,0,39,5110.13,-532.123,296.677,0,4000,0,'open equipment chest'),
(11016,0,40,5110.13,-532.123,296.677,0,4000,0,'cast SPELL_STRENGHT_ARKONARIN'),
(11016,0,41,5110.13,-532.123,296.677,0,4000,0,'SAY_EQUIPMENT'),
(11016,0,42,5110.13,-532.123,296.677,0,0,0,'SAY_ESCAPE'),
(11016,0,43,5099.75,-510.823,296.677,0,0,0,''),
(11016,0,44,5091.94,-497.516,296.677,0,0,0,''),
(11016,0,45,5079.38,-486.811,297.638,0,0,0,''),
(11016,0,46,5069.21,-488.77,298.082,0,0,0,''),
(11016,0,47,5064.24,-496.051,297.275,0,0,0,''),
(11016,0,48,5065.08,-505.239,297.361,0,0,0,''),
(11016,0,49,5067.82,-515.245,297.125,0,0,0,''),
(11016,0,50,5064.62,-521.17,297.801,0,0,0,''),
(11016,0,51,5053.22,-530.739,297.801,0,0,0,''),
(11016,0,52,5045.73,-538.311,297.801,0,0,0,''),
(11016,0,53,5039.69,-548.112,297.801,0,0,0,''),
(11016,0,54,5038.78,-557.588,300.787,0,0,0,''),
(11016,0,55,5042.01,-566.749,303.838,0,0,0,''),
(11016,0,56,5050.56,-568.149,306.782,0,0,0,''),
(11016,0,57,5056.98,-564.674,309.342,0,0,0,''),
(11016,0,58,5060.79,-556.801,311.936,0,0,0,''),
(11016,0,59,5059.58,-551.626,313.221,0,0,0,''),
(11016,0,60,5062.83,-541.994,313.221,0,0,0,''),
(11016,0,61,5063.55,-531.288,313.221,0,0,0,''),
(11016,0,62,5057.93,-523.088,313.221,0,0,0,''),
(11016,0,63,5049.47,-519.36,313.221,0,0,0,''),
(11016,0,64,5040.79,-519.809,313.221,0,0,0,''),
(11016,0,65,5034.3,-515.361,313.948,0,0,0,''),
(11016,0,66,5032,-505.532,314.663,0,0,0,''),
(11016,0,67,5029.92,-495.645,316.821,0,0,0,''),
(11016,0,68,5028.87,-487,318.179,0,0,0,''),
(11016,0,69,5028.11,-475.531,318.839,0,0,0,''),
(11016,0,70,5027.76,-465.442,320.643,0,0,0,''),
(11016,0,71,5019.96,-460.892,321.969,0,0,0,''),
(11016,0,72,5009.43,-464.793,321.248,0,0,0,''),
(11016,0,73,4999.57,-468.062,319.426,0,0,0,''),
(11016,0,74,4992.03,-468.128,317.894,0,0,0,''),
(11016,0,75,4988.17,-461.293,316.369,0,0,0,''),
(11016,0,76,4990.62,-447.459,317.104,0,0,0,''),
(11016,0,77,4993.48,-438.643,318.272,0,0,0,''),
(11016,0,78,4995.45,-430.178,318.462,0,0,0,''),
(11016,0,79,4993.56,-422.876,318.864,0,0,0,''),
(11016,0,80,4985.4,-420.864,320.205,0,0,0,''),
(11016,0,81,4976.52,-426.168,323.112,0,0,0,''),
(11016,0,82,4969.83,-429.755,325.029,0,0,0,''),
(11016,0,83,4960.7,-425.44,325.834,0,0,0,''),
(11016,0,84,4955.45,-418.765,327.433,0,0,0,''),
(11016,0,85,4949.7,-408.796,328.004,0,0,0,''),
(11016,0,86,4940.02,-403.222,329.956,0,0,0,''),
(11016,0,87,4934.98,-401.475,330.898,0,0,0,''),
(11016,0,88,4928.69,-399.302,331.744,0,0,0,''),
(11016,0,89,4926.94,-398.436,333.079,0,0,0,''),
(11016,0,90,4916.16,-393.822,333.729,0,0,0,''),
(11016,0,91,4908.39,-396.217,333.217,0,0,0,''),
(11016,0,92,4905.61,-396.535,335.05,0,0,0,''),
(11016,0,93,4897.88,-395.245,337.346,0,0,0,''),
(11016,0,94,4895.21,-388.203,339.295,0,0,0,''),
(11016,0,95,4896.94,-382.429,341.04,0,0,0,''),
(11016,0,96,4901.88,-378.799,342.771,0,0,0,''),
(11016,0,97,4908.09,-380.635,344.597,0,0,0,''),
(11016,0,98,4911.91,-385.818,346.491,0,0,0,''),
(11016,0,99,4910.1,-393.444,348.798,0,0,0,''),
(11016,0,100,4903.5,-396.947,350.812,0,0,0,''),
(11016,0,101,4898.08,-394.226,351.821,0,0,0,''),
(11016,0,102,4891.33,-393.436,351.801,0,0,0,''),
(11016,0,103,4881.2,-395.211,351.59,0,0,0,''),
(11016,0,104,4877.84,-395.536,349.713,0,0,0,''),
(11016,0,105,4873.97,-394.919,349.844,0,5000,0,'SAY_FRESH_AIR'),
(11016,0,106,4873.97,-394.919,349.844,0,3000,0,'SAY_BETRAYER'),
(11016,0,107,4873.97,-394.919,349.844,0,2000,0,'SAY_TREY'),
(11016,0,108,4873.97,-394.919,349.844,0,0,0,'SAY_ATTACK_TREY'),
(11016,0,109,4873.97,-394.919,349.844,0,5000,0,'SAY_ESCORT_COMPLETE'),
(11016,0,110,4873.97,-394.919,349.844,0,1000,0,''),
(11016,0,111,4863.02,-394.521,350.65,0,0,0,''),
(11016,0,112,4848.7,-397.612,351.215,0,0,0,''),
(11832,0,1,7848.39,-2216.36,470.888,3.9095,15000,0,'SAY_REMULOS_INTRO_1'),
(11832,0,2,7848.39,-2216.36,470.888,3.9095,5000,0,'SAY_REMULOS_INTRO_2'),
(11832,0,3,7829.79,-2244.84,463.853,0,0,0,''),
(11832,0,4,7819.01,-2304.34,455.957,0,0,0,''),
(11832,0,5,7931.1,-2314.35,473.054,0,0,0,''),
(11832,0,6,7943.55,-2324.69,477.677,0,0,0,''),
(11832,0,7,7952.02,-2351.14,485.235,0,0,0,''),
(11832,0,8,7963.67,-2412.99,488.953,0,0,0,''),
(11832,0,9,7975.18,-2551.6,490.08,0,0,0,''),
(11832,0,10,7948.05,-2570.83,489.751,0,0,0,''),
(11832,0,11,7947.16,-2583.4,490.066,0,0,0,''),
(11832,0,12,7951.09,-2596.22,489.831,0,0,0,''),
(11832,0,13,7948.27,-2610.06,492.34,0,0,0,''),
(11832,0,14,7928.52,-2625.95,492.448,3.72,500,0,'escort paused - SAY_REMULOS_INTRO_3'),
(11832,0,15,7948.27,-2610.06,492.34,0,0,0,''),
(11832,0,16,7952.32,-2594.12,490.07,0,0,0,''),
(11832,0,17,7913.99,-2567,488.331,0,0,0,''),
(11832,0,18,7835.45,-2571.1,489.289,0,500,0,'escort paused - SAY_REMULOS_DEFEND_2'),
(11832,0,19,7897.28,-2560.65,487.461,0,500,0,'escort paused'),
(11856,0,1,113.91,-350.13,4.55,0,0,0,''),
(11856,0,2,109.54,-350.08,3.74,0,0,0,''),
(11856,0,3,106.95,-353.4,3.6,0,0,0,''),
(11856,0,4,100.28,-338.89,2.97,0,0,0,''),
(11856,0,5,110.11,-320.26,3.47,0,0,0,''),
(11856,0,6,109.78,-287.8,5.3,0,0,0,''),
(11856,0,7,105.02,-269.71,4.71,0,0,0,''),
(11856,0,8,86.71,-251.81,5.34,0,0,0,''),
(11856,0,9,64.1,-246.38,5.91,0,0,0,''),
(11856,0,10,-2.55,-243.58,6.3,0,0,0,''),
(11856,0,11,-27.78,-267.53,-1.08,0,0,0,''),
(11856,0,12,-31.27,-283.54,-4.36,0,0,0,''),
(11856,0,13,-28.96,-322.44,-9.19,0,0,0,''),
(11856,0,14,-35.63,-360.03,-16.59,0,0,0,''),
(11856,0,15,-58.3,-412.26,-30.6,0,0,0,''),
(11856,0,16,-58.88,-474.17,-44.54,0,0,0,''),
(11856,0,17,-45.92,-496.57,-46.26,0,5000,0,'AMBUSH'),
(11856,0,18,-40.25,-510.07,-46.05,0,0,0,''),
(11856,0,19,-38.88,-520.72,-46.06,0,5000,0,'END'),
(12126,0,1,2631.23,-1917.93,72.59,0,0,0,''),
(12126,0,2,2643.53,-1914.07,71,0,0,0,''),
(12126,0,3,2653.83,-1907.54,69.34,0,1000,0,'escort paused'),
(12126,0,4,2653.83,-1907.54,69.34,0,1000,0,''),
(12277,0,1,-1154.87,2708.16,111.123,0,1000,0,'SAY_MELIZZA_START'),
(12277,0,2,-1162.62,2712.86,111.549,0,0,0,''),
(12277,0,3,-1183.37,2709.45,111.601,0,0,0,''),
(12277,0,4,-1245.09,2676.43,111.572,0,0,0,''),
(12277,0,5,-1260.54,2672.48,111.55,0,0,0,''),
(12277,0,6,-1272.71,2666.38,111.555,0,0,0,''),
(12277,0,7,-1342.95,2580.82,111.557,0,0,0,''),
(12277,0,8,-1362.24,2561.74,110.848,0,0,0,''),
(12277,0,9,-1376.56,2514.06,95.6146,0,0,0,''),
(12277,0,10,-1379.06,2510.88,93.3256,0,0,0,''),
(12277,0,11,-1383.14,2489.17,89.009,0,0,0,''),
(12277,0,12,-1395.34,2426.15,88.6607,0,0,0,'SAY_MELIZZA_FINISH'),
(12277,0,13,-1366.23,2317.17,91.8086,0,0,0,''),
(12277,0,14,-1353.81,2213.52,90.726,0,0,0,''),
(12277,0,15,-1354.19,2208.28,88.7386,0,0,0,''),
(12277,0,16,-1354.59,2193.77,77.6702,0,0,0,''),
(12277,0,17,-1367.62,2160.64,67.1482,0,0,0,''),
(12277,0,18,-1379.44,2132.77,64.1326,0,0,0,''),
(12277,0,19,-1404.81,2088.68,61.8162,0,0,0,'SAY_MELIZZA_1'),
(12277,0,20,-1417.15,2082.65,62.4112,0,0,0,''),
(12277,0,21,-1423.28,2074.19,62.2046,0,0,0,''),
(12277,0,22,-1432.99,2070.56,61.7811,0,0,0,''),
(12277,0,23,-1469.27,2078.68,63.1141,0,0,0,''),
(12277,0,24,-1507.21,2115.12,62.3578,0,0,0,''),
(12423,0,1,-9509.72,-147.03,58.74,0,0,0,''),
(12423,0,2,-9517.07,-172.82,58.66,0,0,0,''),
(12427,0,1,-5689.2,-456.44,391.08,0,0,0,''),
(12427,0,2,-5700.37,-450.77,393.19,0,0,0,''),
(12428,0,1,2454.09,361.26,31.51,0,0,0,''),
(12428,0,2,2472.03,378.08,30.98,0,0,0,''),
(12429,0,1,9654.19,909.58,1272.11,0,0,0,''),
(12429,0,2,9642.53,908.11,1269.1,0,0,0,''),
(12430,0,1,161.65,-4779.34,14.64,0,0,0,''),
(12430,0,2,140.71,-4813.56,17.04,0,0,0,''),
(12580,0,1,-8997.63,486.402,96.622,0,0,0,''),
(12580,0,2,-8971.08,507.541,96.349,0.666,1000,0,'SAY_DIALOG_1'),
(12580,0,3,-8953.17,518.537,96.355,0,0,0,''),
(12580,0,4,-8936.33,501.777,94.066,0,0,0,''),
(12580,0,5,-8922.52,498.45,93.869,0,0,0,''),
(12580,0,6,-8907.64,509.941,93.84,0,0,0,''),
(12580,0,7,-8925.26,542.51,94.274,0,0,0,''),
(12580,0,8,-8832.28,622.285,93.686,0,0,0,''),
(12580,0,9,-8824.8,621.713,94.084,0,0,0,''),
(12580,0,10,-8796.46,590.922,97.466,0,0,0,''),
(12580,0,11,-8769.85,607.883,97.118,0,0,0,''),
(12580,0,12,-8737.14,574.741,97.398,0,0,0,'reset jonathan'),
(12580,0,13,-8746.27,563.446,97.399,0,0,0,''),
(12580,0,14,-8745.5,557.877,97.704,0,0,0,''),
(12580,0,15,-8730.95,541.477,101.12,0,0,0,''),
(12580,0,16,-8713.16,520.692,97.227,0,0,0,''),
(12580,0,17,-8677.09,549.614,97.438,0,0,0,''),
(12580,0,18,-8655.72,552.732,96.941,0,0,0,''),
(12580,0,19,-8641.68,540.516,98.972,0,0,0,''),
(12580,0,20,-8620.08,520.12,102.812,0,0,0,''),
(12580,0,21,-8591.09,492.553,104.032,0,0,0,''),
(12580,0,22,-8562.45,463.583,104.517,0,0,0,''),
(12580,0,23,-8548.63,467.38,104.517,5.41,1000,0,'SAY_WINDSOR_BEFORE_KEEP'),
(12580,0,24,-8487.77,391.44,108.386,0,0,0,''),
(12580,0,25,-8455.95,351.225,120.88,0,0,0,''),
(12580,0,26,-8446.87,339.904,121.33,5.3737,1000,0,'SAY_WINDSOR_KEEP_1'),
(12580,0,27,-8446.87,339.904,121.33,0,10000,0,''),
(12717,0,1,3346.25,1007.88,3.59,0,0,0,'SAY_MUG_START2'),
(12717,0,2,3367.39,1011.51,3.72,0,0,0,''),
(12717,0,3,3418.64,1013.96,2.905,0,0,0,''),
(12717,0,4,3426.84,1015.1,3.449,0,0,0,''),
(12717,0,5,3437.03,1020.79,2.742,0,0,0,''),
(12717,0,6,3460.56,1024.26,1.353,0,0,0,''),
(12717,0,7,3479.87,1037.96,1.023,0,0,0,''),
(12717,0,8,3490.53,1043.35,3.338,0,0,0,''),
(12717,0,9,3504.28,1047.77,8.205,0,0,0,''),
(12717,0,10,3510.73,1049.79,12.143,0,0,0,''),
(12717,0,11,3514.41,1051.17,13.235,0,0,0,''),
(12717,0,12,3516.94,1052.91,12.918,0,0,0,''),
(12717,0,13,3523.64,1056.3,7.563,0,0,0,''),
(12717,0,14,3531.94,1059.86,6.175,0,0,0,''),
(12717,0,15,3535.48,1069.96,1.697,0,0,0,''),
(12717,0,16,3546.98,1093.49,0.68,0,0,0,''),
(12717,0,17,3549.73,1101.88,-1.123,0,0,0,''),
(12717,0,18,3555.14,1116.99,-4.326,0,0,0,''),
(12717,0,19,3571.94,1132.18,-0.634,0,0,0,''),
(12717,0,20,3574.28,1137.58,3.684,0,0,0,''),
(12717,0,21,3579.31,1137.25,8.205,0,0,0,''),
(12717,0,22,3590.22,1143.65,8.291,0,0,0,''),
(12717,0,23,3595.97,1145.83,6.773,0,0,0,''),
(12717,0,24,3603.65,1146.92,9.763,0,0,0,''),
(12717,0,25,3607.08,1146.01,10.692,0,5000,0,'SAY_MUG_BRAZIER'),
(12717,0,26,3614.52,1142.63,10.248,0,0,0,''),
(12717,0,27,3616.66,1140.84,10.682,0,3000,0,'SAY_MUG_PATROL'),
(12717,0,28,3621.08,1138.11,10.369,0,0,0,'SAY_MUG_RETURN'),
(12717,0,29,3615.48,1145.53,9.614,0,0,0,''),
(12717,0,30,3607.19,1152.72,8.871,0,0,0,''),
(12818,0,1,3347.25,-694.701,159.926,0,0,0,''),
(12818,0,2,3341.53,-694.726,161.125,0,1000,0,''),
(12818,0,3,3338.35,-686.088,163.444,0,0,0,''),
(12818,0,4,3352.74,-677.722,162.316,0,0,0,''),
(12818,0,5,3370.29,-669.367,160.751,0,0,0,''),
(12818,0,6,3381.48,-659.449,162.545,0,0,0,''),
(12818,0,7,3389.55,-648.5,163.652,0,0,0,''),
(12818,0,8,3396.65,-641.509,164.216,0,0,0,''),
(12818,0,9,3410.5,-634.3,165.773,0,0,0,''),
(12818,0,10,3418.46,-631.792,166.478,0,0,0,''),
(12818,0,11,3429.5,-631.589,166.921,0,0,0,''),
(12818,0,12,3434.95,-629.245,168.334,0,0,0,''),
(12818,0,13,3438.93,-618.503,171.503,0,0,0,''),
(12818,0,14,3444.22,-609.294,173.078,0,1000,0,'Ambush 1'),
(12818,0,15,3460.51,-593.794,174.342,0,0,0,''),
(12818,0,16,3480.28,-578.21,176.652,0,0,0,''),
(12818,0,17,3492.91,-562.335,181.396,0,0,0,''),
(12818,0,18,3495.23,-550.978,184.652,0,0,0,''),
(12818,0,19,3496.25,-529.194,188.172,0,0,0,''),
(12818,0,20,3497.62,-510.411,188.345,0,0,0,''),
(12818,0,21,3498.5,-497.788,185.806,0,0,0,''),
(12818,0,22,3484.22,-489.718,182.39,0,0,0,''),
(12818,0,23,3469.44,-481.94,175.62,0,0,0,''),
(12818,0,24,3449.15,-471.29,168.49,0,0,0,''),
(12818,0,25,3426.67,-456.34,158.85,0,0,0,''),
(12818,0,26,3406.53,-446.61,153.57,0,0,0,''),
(12818,0,27,3386.2,-437.82,151.93,0,0,0,''),
(12818,0,28,3349.5782,-439.28,151.92,0,0,0,''),
(12818,0,29,3310.3,-467.28,152.24,0,0,0,''),
(12818,0,30,3290.63,-507.6,153.61,0,0,0,''),
(12818,0,31,3272.89,-524.97,154.31,0,1000,0,'Ambush 2'),
(12818,0,32,3231.15,-524.41,147.63,0,2000,0,'Quest credit'),
(12818,0,33,3231.15,-524.41,147.63,0,4000,0,'Thanks players'),
(12818,0,34,3231.15,-524.41,147.63,0,4000,0,'Shapeshift'),
(12818,0,35,3175.96,-494.54,140.79,0,0,0,''),
(12818,0,36,3168.17,-480.43,139.36,0,0,0,'Despawn'),
(12858,0,1,1782.63,-2241.11,109.73,0,5000,0,''),
(12858,0,2,1788.88,-2240.17,111.71,0,0,0,''),
(12858,0,3,1797.49,-2238.11,112.31,0,0,0,''),
(12858,0,4,1803.83,-2232.77,111.22,0,0,0,''),
(12858,0,5,1806.65,-2217.83,107.36,0,0,0,''),
(12858,0,6,1811.81,-2208.01,107.45,0,0,0,''),
(12858,0,7,1820.85,-2190.82,100.49,0,0,0,''),
(12858,0,8,1829.6,-2177.49,96.44,0,0,0,''),
(12858,0,9,1837.98,-2164.19,96.71,0,0,0,'prepare'),
(12858,0,10,1839.99,-2149.29,96.78,0,0,0,''),
(12858,0,11,1835.14,-2134.98,96.8,0,0,0,''),
(12858,0,12,1823.57,-2118.27,97.43,0,0,0,''),
(12858,0,13,1814.99,-2110.35,98.38,0,0,0,''),
(12858,0,14,1806.6,-2103.09,99.19,0,0,0,''),
(12858,0,15,1798.27,-2095.77,100.04,0,0,0,''),
(12858,0,16,1783.59,-2079.92,100.81,0,0,0,''),
(12858,0,17,1776.79,-2069.48,101.77,0,0,0,''),
(12858,0,18,1776.82,-2054.59,109.82,0,0,0,''),
(12858,0,19,1776.88,-2047.56,109.83,0,0,0,''),
(12858,0,20,1776.86,-2036.55,109.83,0,0,0,''),
(12858,0,21,1776.9,-2024.56,109.83,0,0,0,'win'),
(12858,0,22,1776.87,-2028.31,109.83,0,60000,0,'stay'),
(12858,0,23,1776.9,-2028.3,109.83,0,0,0,''),
(15420, 0, 1, 9296.278, -6676.996, 22.358725, 100, 0, 0, 'Prospector Anvilward'),
(15420, 0, 2, 9299.323, -6668.9614, 22.41846, 100, 0, 0, 'Prospector Anvilward'),
(15420, 0, 3, 9303.353, -6666.7637, 22.432236, 100, 0, 0, 'Prospector Anvilward'),
(15420, 0, 4, 9307.93, -6660.8057, 22.43064, 100, 0, 0, 'Prospector Anvilward'),
(15420, 0, 5, 9309.302, -6656.1987, 23.005793, 100, 0, 0, 'Prospector Anvilward'),
(15420, 0, 6, 9307.386, -6651.9053, 24.834118, 100, 0, 0, 'Prospector Anvilward'),
(15420, 0, 7, 9300.505, -6648.1587, 28.052294, 100, 0, 0, 'Prospector Anvilward'),
(15420, 0, 8, 9293.393, -6650.7246, 30.57717, 100, 0, 0, 'Prospector Anvilward'),
(15420, 0, 9, 9290.5205, -6654.362, 31.830189, 100, 0, 0, 'Prospector Anvilward'),
(15420, 0, 10, 9289.944, -6657.774, 31.828085, 100, 0, 0, 'Prospector Anvilward'),
(15420, 0, 11, 9290.866, -6658.0156, 31.823935, 0.104719758033752441, 60000, 0, 'Prospector Anvilward'),
(16295,0,1,7545.07,-7359.87,162.354,0,4000,0,'SAY_START'),
(16295,0,2,7550.05,-7362.24,162.236,0,0,0,''),
(16295,0,3,7566.98,-7364.32,161.739,0,0,0,''),
(16295,0,4,7578.83,-7361.68,161.739,0,0,0,''),
(16295,0,5,7590.97,-7359.05,162.258,0,0,0,''),
(16295,0,6,7598.35,-7362.82,162.257,0,4000,0,'SAY_PROGRESS_1'),
(16295,0,7,7605.86,-7380.42,161.937,0,0,0,''),
(16295,0,8,7605.3,-7387.38,157.254,0,0,0,''),
(16295,0,9,7606.13,-7393.89,156.942,0,0,0,''),
(16295,0,10,7615.21,-7400.19,157.143,0,0,0,''),
(16295,0,11,7618.96,-7402.65,158.202,0,0,0,''),
(16295,0,12,7636.85,-7401.76,162.145,0,0,0,'SAY_PROGRESS_2'),
(16295,0,13,7637.06,-7404.94,162.207,0,4000,0,''),
(16295,0,14,7636.91,-7412.59,162.366,0,0,0,''),
(16295,0,15,7637.61,-7425.59,162.631,0,0,0,''),
(16295,0,16,7637.82,-7459.06,163.303,0,0,0,''),
(16295,0,17,7638.86,-7470.9,162.517,0,0,0,''),
(16295,0,18,7641.4,-7488.22,157.381,0,0,0,''),
(16295,0,19,7634.46,-7505.45,154.682,0,0,0,'SAY_PROGRESS_3'),
(16295,0,20,7631.91,-7516.95,153.597,0,0,0,''),
(16295,0,21,7622.23,-7537.04,151.587,0,0,0,''),
(16295,0,22,7610.92,-7550.67,149.639,0,0,0,''),
(16295,0,23,7598.23,-7562.55,145.954,0,0,0,''),
(16295,0,24,7588.51,-7577.76,148.294,0,0,0,''),
(16295,0,25,7567.34,-7608.46,146.006,0,0,0,''),
(16295,0,26,7562.55,-7617.42,148.098,0,0,0,''),
(16295,0,27,7561.51,-7645.06,151.245,0,0,0,''),
(16295,0,28,7563.34,-7654.65,151.227,0,0,0,''),
(16295,0,29,7565.53,-7658.3,151.249,0,0,0,''),
(16295,0,31,7579.12,-7662.21,151.652,0,0,0,'quest complete'),
(16295,0,32,7603.77,-7667,153.998,0,0,0,''),
(16295,0,33,7603.77,-7667,153.998,0,4000,0,'SAY_END_1'),
(16295,0,34,7603.77,-7667,153.998,0,8000,0,'SAY_END_2'),
(16295,0,35,7603.77,-7667,153.998,0,0,0,''),
(16295,0,40,7571.16,-7659.12,151.245,0,0,0,''),
(16812,0,1,-10868.3,-1779.84,90.476,0,2500,0,'Open door, begin walking'),
(16812,0,2,-10875.6,-1779.58,90.478,0,0,0,''),
(16812,0,3,-10887.4,-1779.26,90.476,0,0,0,''),
(16812,0,4,-10894.6,-1780.67,90.476,0,0,0,''),
(16812,0,5,-10895,-1782.04,90.476,4.633,2500,0,'Begin Speech after this'),
(16812,0,6,-10894.6,-1780.67,90.476,0,0,0,'Resume walking (back to spawn point now) after speech'),
(16812,0,7,-10887.4,-1779.26,90.476,0,0,0,''),
(16812,0,8,-10875.6,-1779.58,90.478,0,0,0,''),
(16812,0,9,-10868.3,-1779.84,90.476,0,5000,0,'close door'),
(16812,0,10,-10866.8,-1780.96,90.47,0,2000,0,'Summon mobs, open curtains'),
(16993,0,1,-1137.73,4272.1,14.0485,0,3500,0,''),
(16993,0,2,-1142.87,4251.58,13.9909,0,0,0,''),
(16993,0,3,-1142.67,4225.87,14.3029,0,0,0,''),
(16993,0,4,-1127.21,4213.17,13.6458,0,0,0,''),
(16993,0,5,-1123.65,4205.01,15.2288,0,0,0,''),
(16993,0,6,-1106.34,4201.89,15.9839,0,0,0,''),
(16993,0,7,-1091.46,4200.83,16.7737,0,0,0,''),
(16993,0,8,-1079.59,4201.02,16.7962,0,0,0,''),
(16993,0,9,-1054.96,4198.35,15.2285,0,0,0,''),
(16993,0,10,-1021.06,4189.7,16.3929,0,0,0,''),
(16993,0,11,-1005.45,4188.33,17.9265,0,0,0,''),
(16993,0,12,-973.96,4187.16,24.1454,0,0,0,''),
(16993,0,13,-958.115,4185.98,27.9229,0,2000,0,'Over there - wait'),
(16993,0,14,-958.115,4185.98,27.9229,0,4000,0,'Over there - say & spawn'),
(16993,0,15,-930.922,4216.79,33.6533,0,0,0,''),
(16993,0,16,-909.867,4210.45,40.5747,0,0,0,''),
(16993,0,17,-888.726,4206.29,43.9528,0,0,0,''),
(16993,0,18,-872.329,4210.79,44.0723,0,0,0,''),
(16993,0,19,-859.831,4218.63,44.5452,0,750,0,'Allow me a moment - stop'),
(16993,0,20,-859.831,4218.63,44.5452,0,6000,0,'Allow me a moment - kneel say'),
(16993,0,21,-859.831,4218.63,44.5452,0,1000,0,'stand'),
(16993,0,22,-859.831,4218.63,44.5452,0,4000,0,'Do you hear something'),
(16993,0,23,-834.075,4207.8,46.2378,0,0,0,''),
(16993,0,24,-821.511,4205.95,45.812,0,0,0,''),
(16993,0,25,-808.365,4201.51,45.1883,0,0,0,''),
(16993,0,26,-786.704,4204.68,44.6353,0,0,0,''),
(16993,0,27,-766.773,4208.33,48.118,0,0,0,''),
(16993,0,28,-745.265,4203.63,48.733,0,0,0,''),
(16993,0,29,-724.692,4197.74,51.5662,0,0,0,''),
(16993,0,30,-711.92,4195.35,54.742,0,0,0,''),
(16993,0,31,-698.287,4191.03,57.3717,0,0,0,''),
(16993,0,32,-694.414,4183.79,57.2819,0,0,0,''),
(16993,0,33,-690.789,4173.94,57.7047,0,0,0,''),
(16993,0,34,-687.706,4163.23,59.3383,0,0,0,''),
(16993,0,35,-683.433,4152.94,62.6162,0,0,0,''),
(16993,0,36,-679.359,4146.83,64.3739,0,0,0,''),
(16993,0,37,-668.823,4148.17,64.2382,0,0,0,''),
(16993,0,38,-654.019,4146.35,64.1158,0,0,0,''),
(16993,0,39,-650.826,4138,64.6607,0,3000,0,'Falcon Watch - say'),
(16993,0,40,-650.826,4138,64.6607,0,2000,0,'Falcon Watch - faint'),
(16993,0,41,-650.826,4138,64.6607,0,3000,0,'Falcon Watch - Teleris kneel - complete'),
(16993,0,42,-650.826,4138,64.6607,0,7000,0,'Falcon Watch - Teleris react'),
(17077,0,1,-16.9501,3801.41,95.064,0,5000,0,'EMOTE_WOLF_LIFT_HEAD'),
(17077,0,2,-15.5774,3805.17,94.833,0,2500,0,''),
(17077,0,3,-20.0118,3806.61,92.476,0,5000,0,'EMOTE_WOLF_HOWL'),
(17077,0,4,-18.5947,3816.21,91.482,0,0,0,''),
(17077,0,5,-19.2935,3838.22,85.012,0,0,0,''),
(17077,0,6,-16.5044,3871.03,82.327,0,0,0,''),
(17077,0,7,2.06451,3898.68,85.623,0,0,0,''),
(17077,0,8,16.4039,3921.17,86.024,0,0,0,''),
(17077,0,9,47.3079,3932,83.302,0,0,0,''),
(17077,0,10,90.0672,3942.91,77,0,0,0,''),
(17077,0,11,106.886,3944.39,76.502,0,0,0,''),
(17077,0,12,139.085,3941.9,80.617,0,0,0,''),
(17077,0,13,150.092,3942.78,80.399,0,0,0,''),
(17077,0,14,193.511,3950.4,74.366,0,0,0,''),
(17077,0,15,226.275,3958,73.257,0,0,0,''),
(17077,0,16,246.687,3963.31,76.376,0,0,0,''),
(17077,0,17,264.206,3977.73,83.704,0,0,0,''),
(17077,0,18,279.857,3986.42,88.245,0,0,0,''),
(17077,0,19,304.04,3998.35,95.649,0,0,0,''),
(17077,0,20,328.072,3995.83,104.434,0,0,0,''),
(17077,0,21,347.485,3990.82,113.608,0,0,0,''),
(17077,0,22,351.257,3954.26,125.747,0,0,0,''),
(17077,0,23,345.626,3932.02,132.358,0,0,0,''),
(17077,0,24,347.972,3908.55,135.52,0,0,0,''),
(17077,0,25,351.888,3891.06,139.957,0,0,0,''),
(17077,0,26,346.117,3864.63,146.647,0,0,0,''),
(17077,0,27,330.012,3839.86,154.148,0,0,0,''),
(17077,0,28,297.251,3811.86,166.893,0,0,0,''),
(17077,0,29,290.783,3800.19,172.13,0,0,0,''),
(17077,0,30,288.125,3782.47,180.825,0,0,0,''),
(17077,0,31,296.818,3771.63,184.961,0,0,0,''),
(17077,0,32,305.256,3765.38,185.36,0,0,0,''),
(17077,0,33,311.448,3757.9,184.312,0,0,0,''),
(17077,0,34,325.258,3730.28,184.076,0,0,0,''),
(17077,0,35,341.159,3717.76,183.904,0,0,0,''),
(17077,0,36,365.589,3717.2,183.902,0,0,0,''),
(17077,0,37,387.395,3731.75,183.645,0,0,0,''),
(17077,0,38,396.574,3732.6,179.831,0,0,0,''),
(17077,0,39,404.303,3737.31,180.151,0,0,0,''),
(17077,0,40,410.996,3742.29,183.364,0,0,0,''),
(17077,0,41,434.905,3761.06,186.219,0,0,0,''),
(17077,0,42,460.129,3774.44,186.348,0,0,0,''),
(17077,0,43,467.644,3788.51,186.446,0,0,0,''),
(17077,0,44,491.552,3815.45,189.848,0,0,0,''),
(17077,0,45,496.958,3836.88,193.078,0,0,0,''),
(17077,0,46,502.889,3855.46,194.834,0,0,0,''),
(17077,0,47,508.208,3863.69,194.024,0,0,0,''),
(17077,0,48,528.908,3887.35,189.762,0,0,0,''),
(17077,0,49,527.722,3890.69,189.24,0,0,0,''),
(17077,0,50,524.637,3891.77,189.149,0,0,0,''),
(17077,0,51,519.146,3886.7,190.128,0,60000,0,'SAY_WOLF_WELCOME'),
(17225,0,1,-11018.86,-1797.269,172.8525,0,3000,0,''),
(17225,0,2,-11105.76,-1875.9,158.978,0,0,0,''),
(17225,0,3,-11175.1,-1857.224,101.0059,0,0,0,''),
(17225,0,4,-11296.93,-1764.531,101.0059,0,0,0,''),
(17225,0,5,-11258.9,-1722.372,101.0059,0,0,0,''),
(17225,0,6,-11176.76,-1809.599,101.0059,0,0,0,''),
(17225,0,7,-11191.11,-1889.396,107.8948,0,0,0,''),
(17225,0,8,-11152.18,-1863.318,101.0059,0,0,0,''),
(17225,0,9,-11130.68,-1891.423,107.8963,0,0,0,''),
(17225,0,10,-11110.67,-1878.771,107.8969,0,3000,0,''),
(17225,0,11,-11142.71,-1891.193,92.25038,0,0,0,'start combat'),
(17238,0,1,954.21,-1433.72,63,0,0,0,''),
(17238,0,2,972.7,-1438.85,65.56,0,0,0,''),
(17238,0,3,984.79,-1444.15,64.13,0,0,0,''),
(17238,0,4,999,-1451.74,61.2,0,0,0,''),
(17238,0,5,1030.94,-1470.39,63.49,0,25000,0,'SAY_FIRST_STOP'),
(17238,0,6,1030.94,-1470.39,63.49,0,3000,0,'SAY_CONTINUE'),
(17238,0,7,1036.5,-1484.25,64.6,0,0,0,''),
(17238,0,8,1039.11,-1501.22,65.32,0,0,0,''),
(17238,0,9,1038.44,-1522.18,64.55,0,0,0,''),
(17238,0,10,1037.19,-1543.15,62.33,0,0,0,''),
(17238,0,11,1036.79,-1563.88,61.93,0,5000,0,'SAY_FIRST_ATTACK'),
(17238,0,12,1036.79,-1563.88,61.93,0,5000,0,'SAY_PURITY'),
(17238,0,13,1035.61,-1587.64,61.66,0,0,0,''),
(17238,0,14,1035.43,-1612.97,61.54,0,0,0,''),
(17238,0,15,1035.36,-1630.66,61.53,0,0,0,''),
(17238,0,16,1038.85,-1653.02,60.35,0,0,0,''),
(17238,0,17,1042.27,-1669.36,60.75,0,0,0,''),
(17238,0,18,1050.41,-1687.22,60.52,0,0,0,''),
(17238,0,19,1061.15,-1704.45,60.59,0,0,0,''),
(17238,0,20,1073.51,-1716.99,60.65,0,0,0,''),
(17238,0,21,1084.2,-1727.24,60.95,0,0,0,''),
(17238,0,22,1100.71,-1739.89,60.64,0,5000,0,'SAY_SECOND_ATTACK'),
(17238,0,23,1100.71,-1739.89,60.64,0,0,0,'SAY_CLEANSE'),
(17238,0,24,1117.03,-1749.01,60.87,0,0,0,''),
(17238,0,25,1123.58,-1762.29,62.4,0,0,0,''),
(17238,0,26,1123.36,-1769.29,62.83,0,0,0,''),
(17238,0,27,1115.78,-1779.59,62.09,0,0,0,''),
(17238,0,28,1109.56,-1789.78,61.03,0,0,0,''),
(17238,0,29,1094.81,-1797.62,61.22,0,0,0,''),
(17238,0,30,1079.3,-1801.58,64.95,0,0,0,''),
(17238,0,31,1060.24,-1803.4,70.36,0,0,0,''),
(17238,0,32,1047.69,-1804.49,73.92,0,0,0,''),
(17238,0,33,1032.59,-1805.99,76.13,0,0,0,''),
(17238,0,34,1013.6,-1812.36,77.32,0,0,0,''),
(17238,0,35,1007.01,-1814.38,80.48,0,0,0,''),
(17238,0,36,999.93,-1816.39,80.48,0,2000,0,'SAY_WELCOME'),
(17238,0,37,984.72,-1822.05,80.48,0,0,0,''),
(17238,0,38,977.77,-1824.8,80.79,0,0,0,''),
(17238,0,39,975.33,-1824.91,81.24,0,12000,0,'event complete'),
(17238,0,40,975.33,-1824.91,81.24,0,10000,0,'SAY_EPILOGUE_1'),
(17238,0,41,975.33,-1824.91,81.24,0,8000,0,'SAY_EPILOGUE_2'),
(17238,0,42,975.33,-1824.91,81.24,0,30000,0,''),
(17312,0,1,-4781.36,-11054.6,2.475,0,5000,0,'SAY_START'),
(17312,0,2,-4786.9,-11050.3,3.868,0,0,0,''),
(17312,0,3,-4830.3,-11043.2,1.651,0,0,0,''),
(17312,0,4,-4833.82,-11038,1.607,0,0,0,''),
(17312,0,5,-4856.62,-11034.5,1.665,0,0,0,''),
(17312,0,6,-4876.67,-11030.6,1.942,0,0,0,''),
(17312,0,7,-4881.06,-11035.6,5.067,0,0,0,''),
(17312,0,8,-4904.61,-11042.7,10.689,0,0,0,''),
(17312,0,9,-4913.1,-11051,12.655,0,0,0,''),
(17312,0,10,-4924.45,-11059.2,14.604,0,0,0,''),
(17312,0,11,-4939.89,-11066.8,13.77,0,0,0,''),
(17312,0,12,-4951.94,-11067.6,14.174,0,0,0,''),
(17312,0,13,-4971.94,-11064.9,16.977,0,0,0,''),
(17312,0,14,-4995.02,-11068.2,21.874,0,0,0,''),
(17312,0,15,-5017.98,-11055.2,22.739,0,0,0,''),
(17312,0,16,-5036.87,-11039.7,25.646,0,0,0,''),
(17312,0,17,-5064,-11027,27.466,0,0,0,''),
(17312,0,18,-5086.48,-11029.3,28.866,0,0,0,''),
(17312,0,19,-5108.68,-11034.5,29.54,0,0,0,''),
(17312,0,20,-5133.68,-11038.8,29.169,0,0,0,''),
(17312,0,21,-5163.79,-11042.2,28.07,0,4000,0,'SAY_PROGRESS'),
(17312,0,22,-5161.12,-11052.9,31.664,0,0,0,''),
(17312,0,23,-5160.37,-11066.5,31.499,0,0,0,''),
(17312,0,24,-5165.48,-11071.7,27.989,0,0,0,''),
(17312,0,25,-5177.59,-11076.6,21.986,0,0,0,''),
(17312,0,26,-5193.23,-11084.3,20.009,0,0,0,''),
(17312,0,27,-5207.54,-11090.4,21.519,0,0,0,''),
(17312,0,28,-5252.69,-11098.8,15.572,0,0,0,''),
(17312,0,29,-5268.33,-11105.3,12.53,0,0,0,''),
(17312,0,30,-5282.29,-11113.1,7.134,0,0,0,''),
(17312,0,31,-5302.01,-11127.3,6.704,0,0,0,''),
(17312,0,32,-5317.62,-11139.3,4.684,0,0,0,''),
(17312,0,33,-5319.09,-11140.5,5.57,0,0,0,''),
(17312,0,34,-5344.95,-11163.4,6.959,0,2000,0,'SAY_END1'),
(17312,0,35,-5344.95,-11163.4,6.959,0,1000,0,'SAY_DAUGHTER'),
(17312,0,36,-5355.86,-11173.8,6.853,0,2000,0,'EMOTE_HUG'),
(17312,0,37,-5355.86,-11173.8,6.853,0,5000,0,'SAY_END2'),
(17312,0,38,-5355.86,-11173.8,6.853,0,0,0,''),
(17804,0,1,-9054.86,443.58,93.05,0,0,0,''),
(17804,0,2,-9079.33,424.49,92.52,0,0,0,''),
(17804,0,3,-9086.21,419.02,92.32,3.80996,3000,0,''),
(17804,0,4,-9086.21,419.02,92.32,3.80996,1000,0,''),
(17804,0,5,-9079.33,424.49,92.52,0,0,0,''),
(17804,0,6,-9054.38,436.3,93.05,0,0,0,''),
(17804,0,7,-9042.23,434.24,93.37,2.23402,5000,0,'SAY_SIGNAL_SENT'),
(17876,0,1,2230.91,118.765,82.2947,0,2000,0,'open the prison door'),
(17876,0,2,2230.33,114.98,82.2946,0,0,0,''),
(17876,0,3,2233.36,111.057,82.2996,0,0,0,''),
(17876,0,4,2231.17,108.486,82.6624,0,0,0,''),
(17876,0,5,2220.22,114.605,89.4264,0,0,0,''),
(17876,0,6,2215.23,115.99,89.4549,0,0,0,''),
(17876,0,7,2210,106.849,89.4549,0,0,0,''),
(17876,0,8,2205.66,105.234,89.4549,0,0,0,''),
(17876,0,9,2192.26,112.618,89.4549,0,2000,0,'SAY_ARMORER_CALL_GUARDS'),
(17876,0,10,2185.32,116.593,89.4548,0,2000,0,'SAY_TH_ARMORER_HIT'),
(17876,0,11,2182.11,120.328,89.4548,0,3000,0,'SAY_TH_ARMORY_1'),
(17876,0,12,2182.11,120.328,89.4548,0,2000,0,''),
(17876,0,13,2182.11,120.328,89.4548,0,2000,0,''),
(17876,0,14,2182.11,120.328,89.4548,0,2000,0,''),
(17876,0,15,2182.11,120.328,89.4548,0,3000,0,''),
(17876,0,16,2182.11,120.328,89.4548,0,3000,0,'SAY_TH_ARMORY_2'),
(17876,0,17,2189.44,113.922,89.4549,0,0,0,''),
(17876,0,18,2195.63,110.584,89.4549,0,0,0,''),
(17876,0,19,2201.09,115.115,89.4549,0,0,0,''),
(17876,0,20,2204.34,121.036,89.4355,0,0,0,''),
(17876,0,21,2208.66,129.127,87.956,0,0,0,'first ambush'),
(17876,0,22,2193.09,137.94,88.2164,0,0,0,''),
(17876,0,23,2173.39,149.064,87.9227,0,0,0,''),
(17876,0,24,2164.25,137.965,85.0595,0,0,0,'second ambush'),
(17876,0,25,2149.31,125.645,77.0858,0,0,0,''),
(17876,0,26,2142.78,127.173,75.5954,0,0,0,''),
(17876,0,27,2139.28,133.952,73.6386,0,0,0,'third ambush'),
(17876,0,28,2139.54,155.235,67.1269,0,0,0,''),
(17876,0,29,2145.38,167.551,64.8974,0,0,0,'fourth ambush'),
(17876,0,30,2134.28,175.304,67.9446,0,0,0,''),
(17876,0,31,2118.08,187.387,68.8141,0,0,0,''),
(17876,0,32,2105.88,195.461,65.1854,0,0,0,''),
(17876,0,33,2096.77,196.939,65.2117,0,0,0,''),
(17876,0,34,2083.9,209.395,64.8736,0,0,0,''),
(17876,0,35,2063.4,229.509,64.4883,0,0,0,'summon Skarloc'),
(17876,0,36,2063.4,229.509,64.4883,0,10000,0,'SAY_SKARLOC_ENTER'),
(17876,0,37,2063.4,229.509,64.4883,0,5000,0,'attack Skarloc'),
(17876,0,38,2063.4,229.509,64.4883,0,0,0,'gossip after skarloc'),
(17876,0,39,2046.7,251.941,62.7851,0,4000,0,'mount up'),
(17876,0,40,2046.7,251.941,62.7851,0,3000,0,'SAY_TH_MOUNTS_UP'),
(17876,0,41,2011.77,278.478,65.3388,0,0,0,''),
(17876,0,42,2005.08,289.676,66.1179,0,0,0,''),
(17876,0,43,2033.11,337.45,66.0948,0,0,0,''),
(17876,0,44,2070.3,416.208,66.0893,0,0,0,''),
(17876,0,45,2086.76,469.768,65.9182,0,0,0,''),
(17876,0,46,2101.7,497.955,61.7881,0,0,0,''),
(17876,0,47,2133.39,530.933,55.37,0,0,0,''),
(17876,0,48,2157.91,559.635,48.5157,0,0,0,''),
(17876,0,49,2167.34,586.191,42.4394,0,0,0,''),
(17876,0,50,2174.17,637.643,33.9002,0,0,0,''),
(17876,0,51,2179.31,656.053,34.723,0,0,0,''),
(17876,0,52,2183.65,670.941,34.0318,0,0,0,''),
(17876,0,53,2201.5,668.616,36.1236,0,0,0,''),
(17876,0,54,2221.56,652.747,36.6153,0,0,0,''),
(17876,0,55,2238.97,640.125,37.2214,0,0,0,''),
(17876,0,56,2251.17,620.574,40.1473,0,0,0,''),
(17876,0,57,2261.98,595.303,41.4117,0,0,0,''),
(17876,0,58,2278.67,560.172,38.909,0,0,0,''),
(17876,0,59,2336.72,528.327,40.9369,0,0,0,''),
(17876,0,60,2381.04,519.612,37.7312,0,0,0,''),
(17876,0,61,2412.2,515.425,39.2068,0,0,0,''),
(17876,0,62,2452.39,516.174,42.9387,0,0,0,''),
(17876,0,63,2467.38,539.389,47.4992,0,0,0,''),
(17876,0,64,2470.7,554.333,46.6668,0,0,0,''),
(17876,0,65,2478.07,575.321,55.4549,0,0,0,''),
(17876,0,66,2480,585.408,56.6921,0,0,0,''),
(17876,0,67,2482.67,608.817,55.6643,0,0,0,''),
(17876,0,68,2485.62,626.061,58.0132,0,2000,0,'dismount'),
(17876,0,69,2486.91,626.356,58.0761,0,2000,0,'EMOTE_TH_STARTLE_HORSE'),
(17876,0,70,2486.91,626.356,58.0761,0,0,0,'gossip before barn'),
(17876,0,71,2488.58,660.94,57.3913,0,0,0,''),
(17876,0,72,2502.56,686.059,55.6252,0,0,0,''),
(17876,0,73,2502.08,694.36,55.5083,0,0,0,''),
(17876,0,74,2491.46,694.321,55.7163,0,0,0,'enter barn'),
(17876,0,75,2491.1,703.3,55.763,0,0,0,''),
(17876,0,76,2485.64,702.992,55.7917,0,0,0,''),
(17876,0,77,2479.63,696.521,55.7901,0,500,0,''),
(17876,0,78,2479.63,696.521,55.7901,0,1500,0,'spawn mobs'),
(17876,0,79,2476.24,696.204,55.8093,0,500,0,''),
(17876,0,80,2476.24,696.204,55.8093,0,0,0,'start dialogue'),
(17876,0,81,2475.39,695.983,55.8146,0,0,0,''),
(17876,0,82,2477.75,694.473,55.7945,0,0,0,''),
(17876,0,83,2481.27,697.747,55.791,0,0,0,''),
(17876,0,84,2486.31,703.131,55.7861,0,0,0,''),
(17876,0,85,2490.76,703.511,55.7662,0,0,0,''),
(17876,0,86,2491.3,694.792,55.7195,0,0,0,'exit barn'),
(17876,0,87,2502.08,694.36,55.5083,0,0,0,''),
(17876,0,88,2507.99,679.298,56.376,0,0,0,''),
(17876,0,89,2524.79,669.919,54.9258,0,0,0,''),
(17876,0,90,2543.19,665.289,56.2957,0,0,0,''),
(17876,0,91,2566.49,664.354,54.5034,0,0,0,''),
(17876,0,92,2592,664.611,56.4394,0,0,0,''),
(17876,0,93,2614.43,663.806,55.3921,0,2000,0,''),
(17876,0,94,2616.14,665.499,55.161,0,0,0,''),
(17876,0,95,2623.56,666.965,54.3983,0,0,0,''),
(17876,0,96,2629.99,661.059,54.2738,0,0,0,''),
(17876,0,97,2629,656.982,56.0651,0,0,0,'enter the church'),
(17876,0,98,2620.84,633.007,56.03,0,3000,0,'SAY_TH_CHURCH_ENTER'),
(17876,0,99,2620.84,633.007,56.03,0,5000,0,'church ambush'),
(17876,0,100,2620.84,633.007,56.03,0,0,0,'SAY_TH_CHURCH_END'),
(17876,0,101,2622.99,639.178,56.03,0,0,0,''),
(17876,0,102,2628.73,656.693,56.061,0,0,0,''),
(17876,0,103,2630.34,661.135,54.2738,0,0,0,''),
(17876,0,104,2635.38,672.243,54.4508,0,0,0,''),
(17876,0,105,2644.13,668.158,55.3797,0,0,0,''),
(17876,0,106,2646.82,666.74,56.9898,0,0,0,''),
(17876,0,107,2658.22,665.432,57.1725,0,0,0,''),
(17876,0,108,2661.88,674.849,57.1725,0,0,0,''),
(17876,0,109,2656.23,677.208,57.1725,0,0,0,''),
(17876,0,110,2652.28,670.27,61.9353,0,0,0,''),
(17876,0,111,2650.79,664.29,61.9302,0,0,0,'inn ambush'),
(17876,0,112,2660.48,659.409,61.937,0,5000,0,'SAY_TA_ESCAPED'),
(17876,0,113,2660.48,659.409,61.937,0,0,0,'SAY_TH_MEET_TARETHA - gossip before epoch'),
(17876,0,114,2660.48,659.409,61.937,0,0,0,'SAY_EPOCH_ENTER1'),
(17876,0,115,2650.62,666.643,61.9305,0,0,0,''),
(17876,0,116,2652.37,670.561,61.9368,0,0,0,''),
(17876,0,117,2656.05,676.761,57.1727,0,0,0,''),
(17876,0,118,2658.49,677.166,57.1727,0,0,0,''),
(17876,0,119,2659.28,667.117,57.1727,0,0,0,''),
(17876,0,120,2649.71,665.387,57.1727,0,0,0,''),
(17876,0,121,2634.79,672.964,54.4577,0,0,0,'outside inn'),
(17876,0,122,2635.06,673.892,54.4713,0,18000,0,'SAY_EPOCH_ENTER3'),
(17876,0,123,2635.06,673.892,54.4713,0,0,0,'fight begins'),
(17876,0,124,2635.06,673.892,54.4713,0,0,0,'fight ends'),
(17876,0,125,2634.3,661.698,54.4147,0,0,0,'run off'),
(17876,0,126,2652.21,644.396,56.1906,0,0,0,''),
(17877,0,1,231.403,8479.94,17.928,0,3000,0,''),
(17877,0,2,214.645,8469.64,23.121,0,0,0,''),
(17877,0,3,208.538,8463.48,24.738,0,0,0,''),
(17877,0,4,196.524,8446.08,24.814,0,0,0,''),
(17877,0,5,188.186,8431.67,22.625,0,0,0,''),
(17877,0,6,181.196,8420.15,23.73,0,0,0,''),
(17877,0,7,171.919,8406.29,21.844,0,0,0,''),
(17877,0,8,166.613,8396.48,23.585,0,0,0,''),
(17877,0,9,167.237,8386.69,21.546,0,0,0,''),
(17877,0,10,169.401,8372.67,19.599,0,0,0,''),
(17877,0,11,174.148,8342.33,20.409,0,0,0,''),
(17877,0,12,173.195,8324.18,21.126,0,0,0,''),
(17877,0,13,172.415,8310.29,21.702,0,0,0,''),
(17877,0,14,173.233,8298.75,19.564,0,0,0,''),
(17877,0,15,173.984,8287.92,18.839,0,0,0,''),
(17877,0,16,189.984,8266.26,18.5,0,0,0,''),
(17877,0,17,204.057,8256.02,19.701,0,0,0,''),
(17877,0,18,212.95,8248.74,21.583,0,0,0,''),
(17877,0,19,223.152,8240.16,20.001,0,0,0,''),
(17877,0,20,230.73,8232.99,18.99,0,0,0,''),
(17877,0,21,238.261,8223.8,20.72,0,0,0,''),
(17877,0,22,247.651,8214.21,19.146,0,0,0,''),
(17877,0,23,259.231,8207.8,19.278,0,0,0,''),
(17877,0,24,272.36,8204.75,19.98,0,0,0,''),
(17877,0,25,282.211,8202.09,22.09,0,20000,0,'SAY_PREPARE'),
(17877,0,26,282.211,8202.09,22.09,0,0,0,'SAY_CAMP_ENTER'),
(17877,0,27,296.006,8191.64,21.68,0,0,0,''),
(17877,0,28,304.472,8188.05,20.707,0,0,0,''),
(17877,0,29,317.574,8182.04,18.296,0,0,0,''),
(17877,0,30,340.046,8178.78,17.937,0,0,0,''),
(17877,0,31,353.799,8181.22,18.557,0,0,0,''),
(17877,0,32,368.231,8186.32,22.45,0,0,0,''),
(17877,0,33,375.737,8187.03,23.916,0,0,0,''),
(17877,0,34,390.067,8186.64,21.19,0,0,0,''),
(17877,0,35,398.699,8181.82,18.648,0,0,0,''),
(17877,0,36,412.325,8172.61,17.927,0,0,0,''),
(17877,0,37,424.541,8161.96,19.575,0,0,0,''),
(17877,0,38,436.9,8157.41,22.115,0,0,0,''),
(17877,0,39,444.548,8155.41,23.553,0,0,0,''),
(17877,0,40,457.201,8154.23,23.429,0,0,0,''),
(17877,0,41,470.989,8154.14,21.65,0,0,0,''),
(17877,0,42,483.435,8154.15,20.706,0,0,0,''),
(17877,0,43,507.558,8157.52,21.729,0,0,0,''),
(17877,0,44,528.036,8162.03,22.795,0,0,0,''),
(17877,0,45,542.402,8161.1,22.914,0,0,0,''),
(17877,0,46,557.286,8160.27,23.708,0,13000,0,''),
(17877,0,47,557.286,8160.27,23.708,0,0,0,'take the Ark'),
(17877,0,48,539.767,8144.84,22.217,0,0,0,''),
(17877,0,49,531.296,8139.48,22.146,0,0,0,''),
(17877,0,50,509.056,8139.26,20.705,0,0,0,''),
(17877,0,51,499.975,8136.23,20.408,0,0,0,''),
(17877,0,52,485.511,8129.39,22.01,0,0,0,''),
(17877,0,53,474.371,8128.53,22.657,0,0,0,''),
(17877,0,54,460.708,8130.12,20.946,0,0,0,''),
(17877,0,55,449.248,8129.27,21.033,0,0,0,''),
(17877,0,56,433.67,8125.06,18.44,0,0,0,''),
(17877,0,57,412.822,8121.58,17.603,0,0,0,''),
(17877,0,58,391.15,8117.81,17.736,0,0,0,''),
(17877,0,59,379.024,8114.19,17.889,0,0,0,''),
(17877,0,60,365.11,8106.99,18.22,0,0,0,''),
(17877,0,61,352.531,8108.94,17.932,0,0,0,''),
(17877,0,62,340.894,8120.64,17.374,0,0,0,''),
(17877,0,63,328.48,8134.93,18.112,0,0,0,''),
(17877,0,64,317.573,8143.25,20.604,0,0,0,''),
(17877,0,65,311.146,8146.8,21.097,0,0,0,''),
(17877,0,66,299.359,8152.58,18.676,0,0,0,''),
(17877,0,67,276.115,8160.44,17.735,0,0,0,''),
(17877,0,68,262.704,8170.51,17.478,0,0,0,''),
(17877,0,69,243.755,8177.75,17.744,0,0,0,''),
(17877,0,70,233.496,8178.43,17.528,0,0,0,''),
(17877,0,71,219.874,8182.55,19.637,0,0,0,'SAY_AMBUSH - escort paused'),
(17877,0,72,219.874,8182.55,19.637,0,20000,0,'SAY_AMBUSH_CLEARED'),
(17877,0,73,210.978,8193.98,20.777,0,0,0,''),
(17877,0,74,203.699,8213.04,22.768,0,0,0,''),
(17877,0,75,199.246,8225.54,24.847,0,0,0,''),
(17877,0,76,195.064,8239.91,22.64,0,0,0,''),
(17877,0,77,193.198,8253.62,20.083,0,0,0,''),
(17877,0,78,189.151,8264.83,18.714,0,0,0,''),
(17877,0,79,178.814,8281.04,19.07,0,0,0,''),
(17877,0,80,173.952,8293.24,18.533,0,0,0,''),
(17877,0,81,174.399,8305.46,21.006,0,0,0,''),
(17877,0,82,175.124,8319.51,21.626,0,0,0,''),
(17877,0,83,175.69,8339.65,20.375,0,0,0,''),
(17877,0,84,172.754,8362.67,19.181,0,0,0,''),
(17877,0,85,176.465,8379.8,18.445,0,0,0,''),
(17877,0,86,186.433,8393.13,18.933,0,0,0,''),
(17877,0,87,199.438,8407.83,18.763,0,0,0,''),
(17877,0,88,211.874,8422.38,18.785,0,0,0,''),
(17877,0,89,219.9,8436.26,21.927,0,0,0,''),
(17877,0,90,225.062,8450.57,22.832,0,0,0,''),
(17877,0,91,226.942,8464.41,19.822,0,0,0,''),
(17877,0,92,231.403,8479.94,17.928,0,0,0,''),
(17877,0,93,247.625,8483.8,22.464,0,13000,0,''),
(17877,0,94,231.403,8479.94,17.928,0,10000,0,'SAY_ESCORT_COMPLETE'),
(17969,0,1,-930.049,5288.08,23.8484,0,0,0,''),
(17969,0,2,-925.678,5296.48,18.1837,0,0,0,''),
(17969,0,3,-924.297,5299.02,17.7109,0,0,0,''),
(17969,0,4,-928.39,5317.02,18.2086,0,0,0,''),
(17969,0,5,-930.621,5329.92,18.7734,0,0,0,'SAY_AMBUSH1'),
(17969,0,6,-931.49,5357.65,18.0272,0,0,0,'SAY_PROGRESS1'),
(17969,0,7,-934.778,5369.34,22.278,0,0,0,''),
(17969,0,8,-934.522,5373.41,22.8347,0,0,0,''),
(17969,0,9,-937.009,5382.98,22.6991,0,0,0,''),
(17969,0,10,-941.948,5404.14,22.6697,0,0,0,''),
(17969,0,11,-931.244,5415.85,23.064,0,0,0,'at crossroad'),
(17969,0,12,-901.498,5420.32,24.2133,0,0,0,''),
(17969,0,13,-860.312,5415.62,23.6711,0,0,0,''),
(17969,0,14,-777.989,5391.98,23.0017,0,0,0,''),
(17969,0,15,-750.362,5385.79,22.7658,0,0,0,''),
(17969,0,16,-731.339,5382.45,22.5171,0,0,0,''),
(17969,0,17,-681.236,5381.38,22.0502,0,3000,0,'end bridge SAY_AMBUSH2'),
(17969,0,18,-681.236,5381.38,22.0502,0,3000,0,'end bridge SAY_SLAVEBINDER_AMBUSH2'),
(17969,0,19,-637.944,5384.34,22.2056,0,0,0,'SAY_PROGRESS2'),
(17969,0,20,-608.954,5408.72,21.6304,0,0,0,''),
(17969,0,21,-598.134,5413.61,21.4123,0,0,0,''),
(17969,0,22,-571.269,5420.77,21.1849,0,0,0,''),
(17969,0,23,-553.1,5424.62,21.1937,0,0,0,''),
(17969,0,24,-524.745,5443.95,20.977,0,0,0,''),
(17969,0,25,-502.985,5446.28,22.1494,0,0,0,''),
(17969,0,26,-472.464,5449.55,22.5615,0,0,0,''),
(17969,0,27,-454.533,5461.3,22.6028,0,30000,0,'quest complete SAY_END'),
(17982,0,1,-1778.69,-11063.5,77.14,0,0,0,''),
(17982,0,2,-1786.79,-11051.7,77.624,0,0,0,''),
(17982,0,3,-1793.5,-11045.1,76.965,0,0,0,''),
(17982,0,4,-1793.67,-11038.3,76.308,0,0,0,''),
(17982,0,5,-1809.28,-11025.8,68.239,0,0,0,''),
(17982,0,6,-1828.21,-11007.5,67.414,0,0,0,''),
(17982,0,7,-1848.78,-10995.2,65.269,0,0,0,''),
(17982,0,8,-1875.19,-10981.4,61.316,0,0,0,''),
(17982,0,9,-1893.58,-10962.1,61.258,0,0,0,''),
(17982,0,10,-1912.96,-10935.2,61.436,0,0,0,''),
(17982,0,11,-1927.93,-10909.6,62.588,0,0,0,''),
(17982,0,12,-1936.61,-10866.2,66.684,0,0,0,''),
(17982,0,13,-1939.89,-10854,69.186,0,0,0,''),
(17982,0,14,-1936.7,-10839.4,73.829,0,0,0,''),
(17982,0,15,-1936.74,-10817.2,81.554,0,0,0,''),
(17982,0,16,-1942.17,-10803.5,86.075,0,0,0,''),
(17982,0,17,-1962.06,-10791.6,88.658,0,0,0,''),
(17982,0,18,-1973.29,-10780.2,88.985,0,0,0,''),
(17982,0,19,-1976.27,-10763.6,90.655,0,0,0,''),
(17982,0,20,-1964.07,-10745,95.73,0,0,0,''),
(17982,0,21,-1953.44,-10728.2,104.113,0,0,0,''),
(17982,0,22,-1950.74,-10710.5,111.087,0,0,0,''),
(17982,0,23,-1956.33,-10689,110.965,0,5000,0,'SAY_ESCORT_LEGOSO_1'),
(17982,0,24,-1955.62,-10658.6,110.883,0,1000,0,'escort paused, SAY_ESCORT_LEGOSO_2, SAY_ESCORT_LEGOSO_3'),
(17982,0,25,-1976.11,-10688.8,112.68,0,0,0,'SAY_ESCORT_LEGOSO_4'),
(17982,0,26,-1985.78,-10699.4,115.46,0,1000,0,'escort paused, SAY_ESCORT_COUNT_3'),
(17982,0,27,-2000.17,-10676.8,118.616,0,0,0,''),
(17982,0,28,-2016.45,-10657.8,125.049,0,0,0,''),
(17982,0,29,-2033.14,-10641.4,137.672,0,0,0,''),
(17982,0,30,-2039.43,-10628.2,144.266,0,0,0,''),
(17982,0,31,-2032.07,-10609.5,148.143,0,0,0,''),
(17982,0,32,-2017.39,-10608.1,153.232,0,0,0,''),
(17982,0,33,-1989.03,-10613.3,162.852,0,0,0,''),
(17982,0,34,-1975.88,-10616.8,163.837,0,5000,0,'SAY_ESCORT_LEGOSO_5, SAY_ESCORT_LEGOSO_6'),
(17982,0,35,-1957.05,-10596.5,168.726,0,0,0,''),
(17982,0,36,-1944.29,-10580.7,174.467,0,13000,0,'SAY_ESCORT_LEGOSO_7, SAY_ESCORT_LEGOSO_8'),
(17982,0,37,-1936.1,-10581.6,176.737,0,0,0,''),
(17982,0,38,-1915.01,-10581.5,178.105,0,1000,0,'escort paused, SAY_ESCORT_LEGOSO_9, SAY_ESCORT_LEGOSO_10'),
(17982,0,39,-1952.08,-10560.3,177.657,0,4000,0,'SAY_ESCORT_COUNT_3, SAY_ESCORT_COUNT_2, SAY_ESCORT_COUNT_1'),
(17982,0,40,-1952.08,-10560.3,177.657,0,1000,0,'escort paused, SAY_LEGOSO_AGGRO'),
(18209,0,1,-1516.63,8462.39,-4.01942,0,500,0,''),
(18209,0,2,-1530.59,8456.21,-3.97776,0,1500,0,'We must leave'),
(18209,0,3,-1526.25,8450.41,-3.97776,0,0,0,''),
(18209,0,4,-1506.32,8435.58,-2.11936,0,0,0,''),
(18209,0,5,-1495.65,8442.8,0.286346,0,0,0,''),
(18209,0,6,-1480.39,8465.27,0.275359,0,0,0,''),
(18209,0,7,-1464.41,8488.97,3.63549,0,0,0,''),
(18209,0,8,-1433.32,8507.52,7.55011,0,500,0,'Surrounded - say'),
(18209,0,9,-1433.32,8507.52,7.55011,0,2500,0,'Surrounded - spawn'),
(18209,0,10,-1433.32,8507.52,7.55011,0,4500,0,'Up ahead'),
(18209,0,11,-1420.74,8514.73,8.33551,0,0,0,''),
(18209,0,12,-1387.99,8538.25,11.0952,0,0,0,''),
(18209,0,13,-1386.31,8551.87,11.4702,0,5000,0,'Farewell - Complete'),
(18209,0,14,-1417,8603.66,15.5289,0,0,0,''),
(18209,0,15,-1440.22,8636.15,17.5314,0,0,0,''),
(18209,0,16,-1462.38,8672.52,20.4273,0,0,0,''),
(18210,0,1,-1583.63,8564.92,2.08442,0,500,0,'Look out'),
(18210,0,2,-1579.65,8555.47,2.78855,0,0,0,''),
(18210,0,3,-1554.27,8517.98,0.483805,0,0,0,''),
(18210,0,4,-1549.79,8514.52,0.145915,0,0,0,''),
(18210,0,5,-1517.2,8516.44,0.658376,0,500,0,'More Coming - say'),
(18210,0,6,-1517.2,8516.44,0.658376,0,2000,0,'More Coming - spawn'),
(18210,0,7,-1467.36,8492.89,3.12625,0,0,0,''),
(18210,0,8,-1459.1,8490.89,4.27294,0,0,0,''),
(18210,0,9,-1424.02,8512.76,8.08551,0,0,0,''),
(18210,0,10,-1404.68,8522.54,9.60284,0,0,0,''),
(18210,0,11,-1385.26,8542.59,11.2687,0,500,0,'Split up - complete'),
(18210,0,12,-1385.26,8542.59,11.2687,0,5000,0,''),
(18210,0,13,-1329.9,8513.73,12.8272,0,0,0,''),
(18210,0,14,-1303.28,8482.24,14.9539,0,0,0,''),
(18210,0,15,-1278.47,8461.38,16.3372,0,0,0,''),
(18760,0,1,-2267.07,3091.46,13.8271,0,0,0,''),
(18760,0,2,-2270.92,3094.19,13.8271,0,0,0,''),
(18760,0,3,-2279.08,3100.04,13.8271,0,0,0,''),
(18760,0,4,-2290.05,3105.07,13.8271,0,0,0,''),
(18760,0,5,-2297.64,3112.32,13.8271,0,0,0,''),
(18760,0,6,-2303.89,3118.22,13.8231,0,10000,0,'building exit'),
(18760,0,7,-2307.77,3123.47,13.7257,0,0,0,''),
(18760,0,8,-2310.67,3126.2,12.5841,0,0,0,''),
(18760,0,9,-2311.48,3126.98,12.2769,0,0,0,''),
(18760,0,10,-2316.91,3132.13,11.9261,0,0,0,''),
(18760,0,11,-2320.43,3135.54,11.7436,0,0,0,''),
(18760,0,12,-2327.38,3139.36,10.9431,0,0,0,''),
(18760,0,13,-2332.02,3142.05,9.81277,0,0,0,''),
(18760,0,14,-2338.21,3145.32,9.31001,0,0,0,''),
(18760,0,15,-2343.1,3148.91,8.84879,0,0,0,''),
(18760,0,16,-2347.76,3153.15,7.71049,0,0,0,''),
(18760,0,17,-2351.04,3156.12,6.66476,0,0,0,''),
(18760,0,18,-2355.15,3163.18,5.11997,0,0,0,''),
(18760,0,19,-2359.01,3169.83,3.64343,0,0,0,''),
(18760,0,20,-2364.85,3176.81,2.32802,0,0,0,''),
(18760,0,21,-2368.77,3181.69,1.53285,0,0,0,''),
(18760,0,22,-2371.76,3185.11,0.979932,0,0,0,''),
(18760,0,23,-2371.85,3191.89,-0.293048,0,0,0,''),
(18760,0,24,-2370.99,3199.6,-1.10504,0,0,0,'turn left 1'),
(18760,0,25,-2376.24,3205.54,-1.04152,0,0,0,''),
(18760,0,26,-2380.99,3211.61,-1.16891,0,0,0,''),
(18760,0,27,-2384.04,3218.4,-1.15279,0,0,0,''),
(18760,0,28,-2385.41,3226.22,-1.23403,0,0,0,''),
(18760,0,29,-2386.02,3233.89,-1.31723,0,0,0,''),
(18760,0,30,-2384.7,3239.82,-1.51903,0,0,0,''),
(18760,0,31,-2382.98,3247.94,-1.39163,0,0,0,''),
(18760,0,32,-2379.68,3254.22,-1.25927,0,0,0,''),
(18760,0,33,-2375.27,3259.69,-1.22925,0,0,0,''),
(18760,0,34,-2369.62,3264.55,-1.1879,0,0,0,''),
(18760,0,35,-2364.01,3268.32,-1.48348,0,0,0,''),
(18760,0,36,-2356.61,3272.31,-1.5505,0,0,0,''),
(18760,0,37,-2352.3,3274.63,-1.35693,0,0,0,''),
(18760,0,38,-2348.54,3278.04,-1.04161,0,0,0,'turn left 2'),
(18760,0,39,-2347.56,3282.41,-0.75979,0,0,0,''),
(18760,0,40,-2348.29,3288.91,-0.63215,0,0,0,''),
(18760,0,41,-2349.68,3298.84,-1.07864,0,0,0,''),
(18760,0,42,-2351.15,3308.52,-1.38864,0,0,0,''),
(18760,0,43,-2352.2,3317.14,-1.59873,0,0,0,''),
(18760,0,44,-2351.59,3325.51,-1.92624,0,0,0,''),
(18760,0,45,-2350.88,3333.38,-2.32506,0,0,0,''),
(18760,0,46,-2350.05,3342.68,-2.51886,0,0,0,''),
(18760,0,47,-2350.12,3347.32,-2.57528,0,0,0,''),
(18760,0,48,-2348.72,3353.7,-2.72689,0,0,0,''),
(18760,0,49,-2346.53,3360.85,-2.9756,0,0,0,''),
(18760,0,50,-2344.83,3365.46,-3.3311,0,0,0,''),
(18760,0,51,-2342.68,3368.91,-3.74526,0,0,0,''),
(18760,0,52,-2340.25,3371.44,-4.10499,0,0,0,''),
(18760,0,53,-2337.4,3373.47,-4.44362,0,0,0,''),
(18760,0,54,-2332.68,3376.02,-5.19648,0,0,0,''),
(18760,0,55,-2326.69,3379.64,-6.24757,0,0,0,''),
(18760,0,56,-2321.2,3383.98,-7.28247,0,0,0,''),
(18760,0,57,-2317.81,3387.78,-8.40093,0,0,0,''),
(18760,0,58,-2315.3,3392.47,-9.63431,0,0,0,''),
(18760,0,59,-2314.69,3396.6,-10.2031,0,0,0,''),
(18760,0,60,-2315.48,3402.35,-10.8211,0,0,0,'gate'),
(18760,0,61,-2317.55,3409.27,-11.3309,0,5000,0,'Firewing point exit'),
(18760,0,62,-2320.69,3412.99,-11.5207,0,0,0,''),
(18760,0,63,-2326.88,3417.89,-11.6105,0,0,0,''),
(18760,0,64,-2332.73,3421.74,-11.5659,0,0,0,''),
(18760,0,65,-2337.23,3424.89,-11.496,0,0,0,''),
(18760,0,66,-2339.57,3429.17,-11.3782,0,0,0,''),
(18760,0,67,-2341.66,3435.86,-11.3746,0,5000,0,'Wave and transform'),
(18760,0,68,-2342.15,3443.94,-11.2562,0,2000,0,'final destination'),
(18887,0,1,2650.06,665.473,61.9305,0,0,0,''),
(18887,0,2,2652.44,670.761,61.937,0,0,0,''),
(18887,0,3,2655.96,676.913,57.1725,0,0,0,''),
(18887,0,4,2659.4,677.317,57.1725,0,0,0,''),
(18887,0,5,2651.75,664.482,57.1725,0,0,0,''),
(18887,0,6,2647.49,666.595,57.0824,0,0,0,''),
(18887,0,7,2644.37,668.167,55.4182,0,0,0,''),
(18887,0,8,2638.57,671.231,54.52,0,0,0,'start dialogue - escort paused'),
(18887,0,9,2636.56,679.894,54.6595,0,0,0,''),
(18887,0,10,2640.79,689.647,55.3215,0,0,0,''),
(18887,0,11,2639.35,706.777,56.0667,0,0,0,''),
(18887,0,12,2617.7,731.884,55.5571,0,0,0,''),
(19589,0,1,3358.22,3728.25,141.204,0,16000,0,''),
(19589,0,2,3368.05,3715.51,142.057,0,0,0,''),
(19589,0,3,3389.04,3701.21,144.648,0,0,0,''),
(19589,0,4,3419.51,3691.41,146.598,0,0,0,''),
(19589,0,5,3437.83,3699.2,147.235,0,0,0,''),
(19589,0,6,3444.85,3700.89,147.088,0,0,0,''),
(19589,0,7,3449.89,3700.14,148.118,0,12000,0,'first object'),
(19589,0,8,3443.55,3682.09,149.219,0,0,0,''),
(19589,0,9,3452.6,3674.65,150.226,0,0,0,''),
(19589,0,10,3462.6,3659.01,152.436,0,0,0,''),
(19589,0,11,3469.18,3649.47,153.178,0,0,0,''),
(19589,0,12,3475.11,3639.41,157.213,0,0,0,''),
(19589,0,13,3482.26,3617.69,159.126,0,0,0,''),
(19589,0,14,3492.7,3606.27,156.419,0,0,0,''),
(19589,0,15,3493.52,3595.06,156.581,0,0,0,''),
(19589,0,16,3490.4,3588.45,157.764,0,0,0,''),
(19589,0,17,3485.21,3585.69,159.979,0,12000,0,'second object'),
(19589,0,18,3504.68,3594.44,152.862,0,0,0,''),
(19589,0,19,3523.6,3594.48,145.393,0,0,0,''),
(19589,0,20,3537.01,3576.71,135.748,0,0,0,''),
(19589,0,21,3551.73,3573.12,128.013,0,0,0,''),
(19589,0,22,3552.12,3614.08,127.847,0,0,0,''),
(19589,0,23,3536.14,3639.78,126.031,0,0,0,''),
(19589,0,24,3522.94,3646.47,131.989,0,0,0,''),
(19589,0,25,3507.21,3645.69,138.153,0,0,0,''),
(19589,0,26,3485.15,3645.64,137.755,0,0,0,''),
(19589,0,27,3472.18,3633.88,140.352,0,0,0,''),
(19589,0,28,3435.34,3613.69,140.725,0,0,0,''),
(19589,0,29,3417.4,3612.4,141.143,0,12000,0,'third object'),
(19589,0,30,3411.04,3621.14,142.454,0,0,0,''),
(19589,0,31,3404.47,3636.89,144.434,0,0,0,''),
(19589,0,32,3380.55,3657.06,144.332,0,0,0,''),
(19589,0,33,3375,3676.86,145.298,0,0,0,''),
(19589,0,34,3388.87,3685.48,146.818,0,0,0,''),
(19589,0,35,3393.99,3699.4,144.858,0,0,0,''),
(19589,0,36,3354.95,3726.02,141.428,0,0,0,''),
(19589,0,37,3351.4,3722.33,141.4,0,0,0,'home position'),
(19671,0,1,-362.876,-71.333,-0.96,0,5000,0,'SAY_ESCORT_START'),
(19671,0,2,-372.647,-88.728,-0.958,0,0,0,''),
(19671,0,3,-373.163,-114.959,-0.958,0,0,0,''),
(19671,0,4,-373.137,-138.439,-0.958,0,0,0,''),
(19671,0,5,-373.687,-152.964,-0.958,0,0,0,''),
(19671,0,6,-379.091,-158.089,-0.958,0,0,0,''),
(19671,0,7,-381.149,-164.796,-0.958,0,0,0,''),
(19671,0,8,-375.292,-164.111,0.715,0,0,0,'SAY_FIRST_STOP - escort paused'),
(19671,0,9,-375.292,-164.111,0.715,0,1000,0,'SAY_FIRST_STOP_COMPLETE'),
(19671,0,10,-381.149,-164.796,-0.958,0,0,0,''),
(19671,0,11,-374.292,-193.614,-0.967,0,0,0,''),
(19671,0,12,-370.799,-217.796,-0.959,0,0,0,''),
(19671,0,13,-344.132,-222.647,-0.959,0,0,0,''),
(19671,0,14,-310.88,-219.357,-0.945,0,9000,0,'SAY_COLLECTOR_SEARCH'),
(19671,0,15,-299.879,-201.809,-1.181,0,0,0,''),
(19671,0,16,-285.874,-187.791,-0.694,0,0,0,''),
(19671,0,17,-271.884,-164.856,-1.912,0,0,0,''),
(19671,0,18,-260.729,-159.094,-1.19,0,0,0,'SAY_SECOND_STOP - escort paused'),
(19671,0,19,-244.372,-163.136,-0.924,0,0,0,''),
(19671,0,20,-236.428,-185.952,-0.953,0,0,0,''),
(19671,0,21,-210.659,-206.71,0.906,0,0,0,''),
(19671,0,22,-193.375,-219.378,-0.518,0,0,0,''),
(19671,0,23,-171.121,-223.043,-0.955,0,0,0,''),
(19671,0,24,-148.658,-221.031,-0.77,0,0,0,''),
(19671,0,25,-128.15,-215.657,-0.187,0,0,0,''),
(19671,0,26,-93.429,-219.264,-1.32,0,0,0,''),
(19671,0,27,-72.886,-222.278,-0.184,0,0,0,'SAY_THIRD_STOP - escort paused'),
(19671,0,28,-42.343,-224.004,-0.268,0,0,0,''),
(19671,0,29,-15.603,-223.78,0.793,0,0,0,''),
(19671,0,30,-12.975,-223.157,1.811,0,42000,0,'SAY_REST'),
(19671,0,31,-12.975,-223.157,1.811,0,5000,0,'SAY_BREAK_OVER'),
(19671,0,32,-14.898,-222.906,0.786,0,0,0,''),
(19671,0,33,-44.684,-221.592,-0.146,0,0,0,''),
(19671,0,34,-59.746,-211.847,0.216,0,0,0,''),
(19671,0,35,-63.1,-195.288,-0.615,0,0,0,''),
(19671,0,36,-63.218,-191.959,-1.725,0,0,0,''),
(19671,0,37,-67.063,-160.281,-0.955,0,0,0,''),
(19671,0,38,-67.888,-128.704,-1.226,0,0,0,''),
(19671,0,39,-68.226,-107.766,-0.289,0,0,0,''),
(19671,0,40,-68.251,-105.624,-1.631,0,0,0,''),
(19671,0,41,-66.342,-88.147,-1.167,0,0,0,'SAY_XIRAXIS_SPAWN - escort paused'),
(19671,0,42,-67.831,-78.571,-2.114,0,0,0,''),
(19671,0,43,-67.692,-76.665,-0.941,0,0,0,''),
(19671,0,44,-67.624,-56.025,-0.844,0,0,0,'quest complete'),
(19671,0,45,-64.493,-15.776,-0.943,0,0,0,''),
(19685,0,1,-1856.448,5412.245,-12.18742,0,0,0,''),
(19685,0,2,-1849.448,5400.995,-12.18742,0,0,0,''),
(19685,0,3,-1843.448,5389.745,-12.18742,0,0,0,''),
(19685,0,4,-1830.198,5363.245,-12.18742,0,0,0,''),
(19685,0,5,-1817.948,5337.745,-12.18742,0,0,0,''),
(19685,0,6,-1798.698,5309.745,-13.93742,0,0,0,''),
(19685,0,7,-1779.544,5277.36,-34.69139,100,2500,0,'pause'),
(19685,0,8,-1776.06,5270.25,-38.809,0,0,0,''),
(19685,0,9,-1772.22,5262.78,-38.81,0,0,0,''),
(19685,0,10,-1762.2,5261.72,-38.85,0,0,0,''),
(19685,0,11,-1759.24,5259.75,-40.208,0,0,0,''),
(19685,0,12,-1743.43,5259.66,-40.208,0,0,0,''),
(19685,0,13,-1744.36,5251.18,-44.523,0,0,0,''),
(19685,0,14,-1740.12,5240.12,-47.74,0,0,0,''),
(19685,0,15,-1737.64,5238.29,-49.793,0,0,0,''),
(19685,0,16,-1727.41,5233.87,-50.477,0,0,0,''),
(19685,0,17,-1707.49,5230.44,-51.05,0,0,0,''),
(19685,0,18,-1684.12,5223.63,-49.415,0,0,0,''),
(19685,0,19,-1669.97,5221.93,-46.336,0,0,0,''),
(19685,0,20,-1662.87,5221.71,-44.959,0,0,0,''),
(19685,0,21,-1657.17,5225.21,-45.708,0,0,0,''),
(19685,0,22,-1645.03,5238.36,-40.212,0,0,0,''),
(19685,0,23,-1631.66,5252.76,-40.962,0,0,0,''),
(19685,0,24,-1631.37,5276.54,-41.032,0,0,0,''),
(19685,0,25,-1621.73,5298.55,-40.209,0,0,0,''),
(19685,0,26,-1614.27,5298.1,-40.20916,100,2500,0,'pause'),
(19685,0,27,-1636.98,5302.68,-40.209,0,0,0,''),
(19685,0,28,-1655.33,5315.74,-40.207,0,0,0,''),
(19685,0,29,-1656.88,5321.65,-40.209,0,0,0,''),
(19685,0,30,-1663.98,5335.21,-46.526,0,0,0,''),
(19685,0,31,-1659.14,5359.13,-45.846,0,0,0,''),
(19685,0,32,-1644.21,5390.89,-45.542,0,0,0,''),
(19685,0,33,-1646.18,5405.27,-44.649,0,0,0,''),
(19685,0,34,-1650.2,5414.54,-46.324,0,0,0,''),
(19685,0,35,-1656.05,5424.68,-40.461,0,0,0,''),
(19685,0,36,-1661.63,5423.93,-40.405,0,0,0,''),
(19685,0,37,-1664.65,5423.66,-38.848,0,0,0,''),
(19685,0,38,-1681.77,5426,-38.809,0,0,0,''),
(19685,0,39,-1729.79,5427.25,-12.445,0,0,0,''),
(19685,0,40,-1735.37,5423.66,-12.427,0,0,0,''),
(19685,0,41,-1741.63,5386.77,-12.427,0,0,0,''),
(19685,0,42,-1764.79,5363.74,-12.427,0,0,0,''),
(19685,0,43,-1816.37,5340.66,-12.427,0,0,0,''),
(19685,0,44,-1880.02,5309.8,-12.427,0,0,0,''),
(19685,0,45,-1887.37,5315.43,-12.427,0,0,0,''),
(19685,0,46,-1888.77,5324.52,-5.146,0,0,0,''),
(19685,0,47,-1888.4,5334.15,0.151,0,0,0,''),
(19685,0,48,-1890.22,5337.66,0.921,0,0,0,''),
(19685,0,49,-1897.54,5323.04,1.256,0,0,0,''),
(19685,0,50,-1900.25,5319.8,0.831,0,0,0,''),
(19685,0,51,-1910.04,5291.26,1.288,0,0,0,''),
(19685,0,52,-1916.781,5270.458,3.353774,100,2500,0,'pause'),
(19685,0,53,-1927.23,5273.25,2.703,0,0,0,''),
(19685,0,54,-1926.98,5278.47,0.109,0,0,0,''),
(19685,0,55,-1927.67,5299.21,-12.427,0,0,0,''),
(19685,0,56,-1922.84,5319.26,-12.427,0,0,0,''),
(19685,0,57,-1925.78,5347.41,-12.427,0,0,0,''),
(19685,0,58,-1954.91,5384.23,-12.427,0,0,0,''),
(19685,0,59,-1966.73,5428.2,-12.427,0,0,0,''),
(19685,0,60,-1979.48,5448.42,-12.427,0,0,0,''),
(19685,0,61,-1977.53,5453.86,-12.385,0,0,0,''),
(19685,0,62,-1968.06,5455.78,-4.343,0,0,0,''),
(19685,0,63,-1959.22,5454.9,0.202,0,0,0,''),
(19685,0,64,-1954.63,5457.01,0.9,0,0,0,''),
(19685,0,65,-1957.061,5461.438,1.027785,100,2500,0,'pause'),
(19685,0,66,-1952.87,5462.96,0.956,0,0,0,''),
(19685,0,67,-1955.34,5467.12,0.445,0,0,0,''),
(19685,0,68,-1962.03,5472.8,-4.243,0,0,0,''),
(19685,0,69,-1968.01,5480.91,-12.427,0,0,0,''),
(19685,0,70,-1945.9,5515.95,-12.427,0,0,0,''),
(19685,0,71,-1874.87,5549.78,-12.427,0,0,0,''),
(19685,0,72,-1840.64,5544.23,-12.427,0,0,0,''),
(19685,0,73,-1838.96,5536.06,-5.639,0,0,0,''),
(19685,0,74,-1839.58,5525.63,0.193,0,0,0,''),
(19685,0,75,-1837.93,5521.12,0.844,0,0,0,''),
(19685,0,76,-1832.781,5523.799,1.035073,100,2500,0,'pause'),
(19685,0,77,-1847.987,5477.499,0.8568287,100,40000,0,'end'),
(20129,0,1,-8374.93,-4250.21,-204.38,0,5000,0,''),
(20129,0,2,-8374.93,-4250.21,-204.38,0,16000,0,''),
(20129,0,3,-8374.93,-4250.21,-204.38,0,10000,0,''),
(20129,0,4,-8374.93,-4250.21,-204.38,0,2000,0,''),
(20129,0,5,-8439.4,-4180.05,-209.25,0,0,0,''),
(20129,0,6,-8437.82,-4120.84,-208.59,0,10000,0,''),
(20129,0,7,-8437.82,-4120.84,-208.59,0,16000,0,''),
(20129,0,8,-8437.82,-4120.84,-208.59,0,13000,0,''),
(20129,0,9,-8437.82,-4120.84,-208.59,0,18000,0,''),
(20129,0,10,-8437.82,-4120.84,-208.59,0,15000,0,''),
(20129,0,11,-8437.82,-4120.84,-208.59,0,2000,0,''),
(20129,0,12,-8467.26,-4198.63,-214.21,0,0,0,''),
(20129,0,13,-8667.76,-4252.13,-209.56,0,0,0,''),
(20129,0,14,-8703.71,-4234.58,-209.5,0,14000,0,''),
(20129,0,15,-8703.71,-4234.58,-209.5,0,2000,0,''),
(20129,0,16,-8642.81,-4304.37,-209.57,0,0,0,''),
(20129,0,17,-8649.06,-4394.36,-208.46,0,6000,0,''),
(20129,0,18,-8649.06,-4394.36,-208.46,0,18000,0,''),
(20129,0,19,-8649.06,-4394.36,-208.46,0,2000,0,''),
(20129,0,20,-8468.72,-4437.67,-215.45,0,0,0,''),
(20129,0,21,-8427.54,-4426,-211.13,0,0,0,''),
(20129,0,22,-8364.83,-4393.32,-205.91,0,0,0,''),
(20129,0,23,-8304.54,-4357.2,-208.2,0,18000,0,''),
(20129,0,24,-8304.54,-4357.2,-208.2,0,2000,0,''),
(20129,0,25,-8375.42,-4250.41,-205.14,0,5000,0,''),
(20129,0,26,-8375.42,-4250.41,-205.14,0,5000,0,''),
(20281,0,1,3096.42,2801.41,118.149,0,7000,0,'SAY_DRIJYA_START'),
(20281,0,2,3096.52,2801.06,118.128,0,0,0,'SAY_DRIJYA_1'),
(20281,0,3,3100,2796.67,118.118,0,0,0,''),
(20281,0,4,3098.76,2786.17,117.125,0,0,0,''),
(20281,0,5,3087.79,2754.6,115.441,0,0,0,''),
(20281,0,6,3080.72,2730.79,115.93,0,9000,0,'SAY_DRIJYA_2'),
(20281,0,7,3060.24,2731.31,115.122,0,0,0,''),
(20281,0,8,3050.9,2727.82,114.127,0,0,0,''),
(20281,0,9,3050.9,2727.82,114.127,0,8000,0,'SAY_DRIJYA_4'),
(20281,0,10,3055.01,2724.97,113.687,0,0,0,''),
(20281,0,11,3053.78,2718.43,113.684,0,0,0,''),
(20281,0,12,3028.62,2693.38,114.67,0,0,0,''),
(20281,0,13,3022.43,2695.3,113.406,0,0,0,''),
(20281,0,14,3022.43,2695.3,113.406,0,8000,0,'SAY_DRIJYA_5'),
(20281,0,15,3025.46,2700.75,113.514,0,0,0,''),
(20281,0,16,3011.34,2716.78,113.691,0,0,0,''),
(20281,0,17,3010.88,2726.99,114.239,0,0,0,''),
(20281,0,18,3009.18,2729.08,114.324,0,0,0,''),
(20281,0,19,3009.18,2729.08,114.324,0,15000,0,'SAY_DRIJYA_6'),
(20281,0,20,3009.18,2729.08,114.324,0,6000,0,'SPELL_EXPLOSION_VISUAL'),
(20281,0,21,3009.18,2729.08,114.324,0,8000,0,'SAY_DRIJYA_7'),
(20281,0,22,3033.89,2736.44,114.369,0,0,0,''),
(20281,0,23,3071.49,2741.5,116.462,0,0,0,''),
(20281,0,24,3087.79,2754.6,115.441,0,0,0,''),
(20281,0,25,3094.5,2770.2,115.744,0,0,0,''),
(20281,0,26,3103.51,2784.36,116.857,0,0,0,''),
(20281,0,27,3100,2796.67,118.118,0,0,0,''),
(20281,0,28,3096.29,2801.03,118.096,0,0,0,'SAY_DRIJYA_COMPLETE'),
(20415,0,1,2488.77,2184.89,104.64,100,0,0,''),
(20415,0,2,2478.72,2184.77,98.58,100,0,0,''),
(20415,0,3,2473.52,2184.71,99,100,0,0,''),
(20415,0,4,2453.15,2184.96,97.09,100,4000,0,''),
(20415,0,5,2424.18,2184.15,94.11,100,0,0,''),
(20415,0,6,2413.18,2184.15,93.42,100,0,0,''),
(20415,0,7,2402.02,2183.9,87.59,100,0,0,''),
(20415,0,8,2333.31,2181.63,90.03,100,4000,0,''),
(20415,0,9,2308.73,2184.34,92.04,100,0,0,''),
(20415,0,10,2303.1,2196.89,94.94,100,0,0,''),
(20415,0,11,2304.58,2272.23,96.67,100,0,0,''),
(20415,0,12,2297.09,2271.4,95.16,100,0,0,''),
(20415,0,13,2297.68,2266.79,95.07,100,4000,0,''),
(20415,0,14,2297.67,2266.76,95.07,100,4000,0,''),
(20763,0,1,4084.09,2297.25,110.277,0,0,0,''),
(20763,0,2,4107.17,2294.92,106.625,0,0,0,''),
(20763,0,3,4154.13,2296.79,102.331,0,0,0,''),
(20763,0,4,4166.02,2302.82,103.422,0,0,0,''),
(20763,0,5,4195.04,2301.09,113.786,0,0,0,''),
(20763,0,6,4205.25,2297.12,117.992,0,0,0,''),
(20763,0,7,4230.43,2294.64,127.307,0,0,0,''),
(20763,0,8,4238.98,2293.58,129.332,0,0,0,''),
(20763,0,9,4250.18,2293.27,129.009,0,0,0,''),
(20763,0,10,4262.81,2290.77,126.485,0,0,0,''),
(20763,0,11,4265.85,2278.56,128.235,0,0,0,''),
(20763,0,12,4265.61,2265.73,128.452,0,0,0,''),
(20763,0,13,4258.84,2245.35,132.804,0,0,0,''),
(20763,0,14,4247.98,2221.21,137.668,0,0,0,''),
(20763,0,15,4247.97,2213.88,137.721,0,0,0,''),
(20763,0,16,4249.88,2204.26,137.121,0,4000,0,''),
(20763,0,17,4249.88,2204.26,137.121,0,0,0,'SAY_VANGUARD_FINISH'),
(20763,0,18,4252.46,2170.89,137.677,0,3000,0,'EMOTE_VANGUARD_FINISH'),
(20763,0,19,4252.46,2170.89,137.677,0,5000,0,''),
(20802,0,1,4017.86,2325.04,114.029,0,3000,0,'SAY_INTRO'),
(20802,0,2,4006.37,2324.59,111.455,0,0,0,''),
(20802,0,3,3998.39,2326.36,113.164,0,0,0,''),
(20802,0,4,3982.31,2330.26,113.846,0,7000,0,'SAY_STAGING_GROUNDS'),
(20802,0,5,3950.65,2329.25,113.924,0,0,0,'SAY_TOXIC_HORROR'),
(20802,0,6,3939.23,2330.99,112.197,0,0,0,''),
(20802,0,7,3927.86,2333.64,111.33,0,0,0,''),
(20802,0,8,3917.85,2337.7,113.493,0,0,0,''),
(20802,0,9,3907.74,2343.34,114.062,0,0,0,''),
(20802,0,10,3878.76,2378.61,114.037,0,8000,0,'SAY_SALHADAAR'),
(20802,0,11,3863.15,2355.88,114.987,0,0,0,''),
(20802,0,12,3861.24,2344.89,115.201,0,0,0,''),
(20802,0,13,3872.46,2323.11,114.671,0,0,0,'escort paused - SAY_DISRUPTOR'),
(20802,0,14,3863.74,2349.79,115.382,0,0,0,'SAY_FINISH_2'),
(20985,0,1,4264.29,2102.7,140.839,6.108,0,0,''),
(20985,0,2,4257.76,2099.95,143.481,0,0,0,''),
(20985,0,3,4249.97,2105.68,144.083,0,0,0,''),
(20985,0,4,4246.45,2111.34,144.552,0,0,0,''),
(20985,0,5,4244.12,2117.8,145.031,0,0,0,''),
(20985,0,6,4233.49,2118.49,149.54,0,0,0,''),
(20985,0,7,4221.95,2117.72,153.124,0,0,0,''),
(20985,0,8,4209.7,2114.4,155.606,0,0,0,''),
(20985,0,9,4204.6,2110.8,160.063,0,0,0,''),
(20985,0,10,4180.8,2081.76,161.666,0,0,0,''),
(20985,0,11,4174.75,2070.9,164.385,0,0,0,''),
(20985,0,12,4174.37,2058.48,169.121,0,0,0,''),
(20985,0,13,4178.27,2051.2,172.75,0,0,0,''),
(20985,0,14,4182.15,2044.52,176.056,0,0,0,''),
(20985,0,15,4185.74,2035.18,179.999,0,0,0,''),
(20985,0,16,4186.65,2023.36,184.971,0,0,0,''),
(20985,0,17,4185.54,2014.54,189.862,0,0,0,''),
(20985,0,18,4182.17,2006.35,195.371,0,0,0,''),
(20985,0,19,4177.94,1999.77,200.164,0,0,0,''),
(20985,0,20,4173.38,1994.37,203.833,0,0,0,''),
(20985,0,21,4168.69,1989.66,206.921,0,0,0,''),
(20985,0,22,4162.75,1984.9,209.942,0,0,0,''),
(20985,0,23,4157.4,1981.32,212.398,0,0,0,''),
(20985,0,24,4150.51,1977.62,215.067,0,0,0,''),
(20985,0,25,4144.56,1974.64,217.155,0,0,0,''),
(20985,0,26,4134.58,1969.06,220.354,0,0,0,''),
(20985,0,27,4117.35,1985,223.929,0,0,0,''),
(20985,0,28,4110.04,1994.4,226.897,0,0,0,''),
(20985,0,29,4104.33,2005.52,229.987,0,0,0,''),
(20985,0,30,4097.22,2020.26,234.464,0,0,0,''),
(20985,0,31,4083.09,2037.36,240.329,0,0,0,''),
(20985,0,32,4070.09,2051.12,246.065,0,0,0,''),
(20985,0,33,4057.96,2062.31,250.666,0,0,0,''),
(20985,0,34,4034.9,2077.34,254.478,0,0,0,''),
(20985,0,35,4002.98,2097.8,254.212,0,0,0,''),
(20985,0,36,3992.89,2088.86,254.33,4.073,0,0,''),
(20985,0,37,3989.38,2083.54,256.337,0,0,0,''),
(20985,0,38,3984.63,2077.76,256.405,0,0,0,''),
(20985,0,39,3978.75,2066.79,256.405,0,0,0,''),
(20985,0,40,3959.94,2039.18,257.63,0,0,0,''),
(20985,0,41,3955.86,2030.96,257.812,4.133,60000,0,'Captain Saeed - Wait and Kill Dimensius the All-Devouring 19554'),
(21027,0,1,-2638.89,1358.96,35.9607,0,0,0,''),
(21027,0,2,-2652.62,1354.46,34.8615,0,0,0,''),
(21027,0,3,-2670.7,1348.18,34.446,0,0,0,''),
(21027,0,4,-2690.82,1339.95,34.446,0,0,0,''),
(21027,0,5,-2709.98,1330.77,34.446,0,0,0,''),
(21027,0,6,-2727.74,1322.39,33.3863,0,0,0,''),
(21027,0,7,-2742.53,1314.21,33.6043,0,0,0,''),
(21027,0,8,-2750.69,1308.32,33.5114,0,0,0,''),
(21027,0,9,-2758.5,1290.76,33.2167,0,0,0,'spawn assassin'),
(21027,0,10,-2755.04,1280.85,33.4304,0,0,0,''),
(21027,0,11,-2747.99,1268.58,33.1957,0,0,0,''),
(21027,0,12,-2741.46,1257.92,33.082,0,0,0,''),
(21027,0,13,-2732.63,1245.35,33.6119,0,0,0,''),
(21027,0,14,-2724.68,1235.12,33.3635,0,15000,0,'SAY_WIL_PROGRESS1'),
(21027,0,15,-2724.68,1235.12,33.3635,0,6000,0,'SAY_WIL_FIND_EXIT'),
(21027,0,16,-2746.38,1266.39,33.192,0,0,0,'spawn assassin'),
(21027,0,17,-2746.38,1266.39,33.192,0,0,0,''),
(21027,0,18,-2758.93,1285.13,33.3417,0,0,0,''),
(21027,0,19,-2761.85,1292.31,33.209,0,0,0,''),
(21027,0,20,-2758.87,1300.68,33.2853,0,0,0,''),
(21027,0,21,-2753.93,1307.76,33.4525,0,0,0,''),
(21027,0,22,-2738.61,1316.19,33.483,0,0,0,''),
(21027,0,23,-2727.9,1320.01,33.3811,0,0,0,''),
(21027,0,24,-2709.46,1315.74,33.3018,0,0,0,''),
(21027,0,25,-2704.66,1301.62,32.4633,0,0,0,''),
(21027,0,26,-2704.12,1298.92,32.7682,0,0,0,''),
(21027,0,27,-2691.8,1292.85,33.8526,0,0,0,'spawn assassin'),
(21027,0,28,-2682.88,1288.85,32.9954,0,0,0,''),
(21027,0,29,-2661.87,1279.68,26.6868,0,0,0,''),
(21027,0,30,-2648.94,1270.27,24.1475,0,0,0,''),
(21027,0,31,-2642.51,1262.94,23.5124,0,0,0,'spawn assassin'),
(21027,0,32,-2636.98,1252.43,20.4183,0,0,0,''),
(21027,0,33,-2648.11,1224.98,8.69182,0,0,0,'spawn assassin'),
(21027,0,34,-2658.39,1200.14,5.49224,0,0,0,''),
(21027,0,35,-2668.5,1190.45,3.12741,0,0,0,''),
(21027,0,36,-2685.93,1174.36,5.16392,0,0,0,''),
(21027,0,37,-2701.61,1160.03,5.61131,0,0,0,''),
(21027,0,38,-2714.66,1149.98,4.34237,0,0,0,''),
(21027,0,39,-2721.44,1145,1.91347,0,0,0,''),
(21027,0,40,-2733.96,1143.44,2.62041,0,0,0,'spawn assassin'),
(21027,0,41,-2757.88,1146.94,6.184,0,2000,0,'SAY_WIL_JUST_AHEAD'),
(21027,0,42,-2772.3,1166.05,6.33181,0,0,0,''),
(21027,0,43,-2790.27,1189.94,5.20796,0,0,0,''),
(21027,0,44,-2805.45,1208.66,5.55762,0,0,0,'spawn assassin'),
(21027,0,45,-2820.62,1225.87,6.2661,0,0,0,''),
(21027,0,46,-2831.93,1237.73,5.80851,0,0,0,''),
(21027,0,47,-2842.58,1252.87,6.80748,0,0,0,''),
(21027,0,48,-2846.34,1258.73,7.38617,0,0,0,''),
(21027,0,49,-2847.56,1266.77,8.20879,0,0,0,''),
(21027,0,50,-2841.65,1285.81,7.93322,0,0,0,''),
(21027,0,51,-2841.75,1289.83,6.9903,0,0,0,''),
(21027,0,52,-2861.97,1298.77,6.80733,0,0,0,'spawn assassin'),
(21027,0,53,-2871.4,1302.35,6.80733,0,7500,0,'SAY_WIL_END'),
(22377,0,1,-2770.46,5418.41,-34.538,0,0,0,''),
(22377,0,2,-2778.18,5416.25,-34.538,0,0,0,''),
(22377,0,3,-2816.96,5414.94,-34.529,0,0,0,''),
(22377,0,4,-2827.53,5414.74,-28.265,0,0,0,''),
(22377,0,5,-2841.61,5413.02,-28.261,0,0,0,''),
(22377,0,6,-2863.61,5411.96,-28.262,0,1000,0,'SAY_AKU_AMBUSH_A'),
(22377,0,7,-2874.56,5413.8,-28.26,0,0,0,''),
(22377,0,8,-2878.78,5413.81,-28.261,0,0,0,''),
(22377,0,9,-2892.59,5413.48,-18.784,0,0,0,''),
(22377,0,10,-2896.04,5413.14,-18.589,0,0,0,''),
(22377,0,11,-2896.32,5409.43,-18.45,0,0,0,''),
(22377,0,12,-2896,5396.91,-8.855,0,0,0,''),
(22377,0,13,-2895.73,5386.62,-9.26,0,0,0,''),
(22377,0,14,-2895.32,5367.61,-9.456,0,0,0,''),
(22377,0,15,-2890.31,5353.88,-11.28,0,1000,0,'SAY_AKU_AMBUSH_B'),
(22377,0,16,-2880.42,5334.63,-10.629,0,0,0,''),
(22377,0,17,-2866.39,5314.25,-9.678,0,0,0,''),
(22377,0,18,-2864.75,5277.73,-11.087,0,0,0,''),
(22377,0,19,-2856.33,5255.9,-11.496,0,5000,0,'SAY_AKU_COMPLETE'),
(22424,0,1,-3620.54,4164.57,1.81,0,0,0,'SKYWING_START'),
(22424,0,2,-3624.78,4149.65,7.44,0,0,0,''),
(22424,0,3,-3630.3,4124.84,21.28,0,0,0,''),
(22424,0,4,-3629.14,4093.65,44.35,0,0,0,''),
(22424,0,5,-3626.34,4080.29,52.39,0,0,0,''),
(22424,0,6,-3619.35,4063.86,60.86,0,3000,0,'SAY_SKYWING_TREE_DOWN'),
(22424,0,7,-3615.09,4054.17,62.46,0,0,0,''),
(22424,0,8,-3611.39,4046.6,65.07,0,0,0,''),
(22424,0,9,-3606.68,4040.5,66,0,0,0,''),
(22424,0,10,-3600.88,4038.69,67.14,0,0,0,''),
(22424,0,11,-3597.88,4033.84,68.53,0,0,0,''),
(22424,0,12,-3602.19,4027.89,69.36,0,0,0,''),
(22424,0,13,-3609.85,4028.37,70.78,0,0,0,''),
(22424,0,14,-3613.01,4031.1,72.14,0,0,0,''),
(22424,0,15,-3613.18,4035.63,73.52,0,0,0,''),
(22424,0,16,-3609.84,4039.73,75.25,0,0,0,''),
(22424,0,17,-3604.55,4040.12,77.01,0,0,0,''),
(22424,0,18,-3600.61,4036.03,78.84,0,0,0,''),
(22424,0,19,-3602.63,4029.99,81.01,0,0,0,''),
(22424,0,20,-3608.87,4028.64,83.27,0,0,0,''),
(22424,0,21,-3612.53,4032.74,85.24,0,0,0,''),
(22424,0,22,-3611.08,4038.13,87.31,0,0,0,''),
(22424,0,23,-3605.09,4039.35,89.55,0,0,0,''),
(22424,0,24,-3601.87,4035.44,91.64,0,0,0,''),
(22424,0,25,-3603.08,4030.58,93.66,0,0,0,''),
(22424,0,26,-3608.47,4029.23,95.91,0,0,0,''),
(22424,0,27,-3611.68,4033.35,98.09,0,0,0,''),
(22424,0,28,-3609.51,4038.25,100.45,0,0,0,''),
(22424,0,29,-3604.54,4038.01,102.72,0,0,0,''),
(22424,0,30,-3602.4,4033.48,105.12,0,0,0,''),
(22424,0,31,-3606.17,4029.69,107.63,0,0,0,''),
(22424,0,32,-3609.93,4031.26,109.53,0,0,0,''),
(22424,0,33,-3609.38,4035.86,110.67,0,0,0,''),
(22424,0,34,-3603.58,4043.03,112.89,0,0,0,''),
(22424,0,35,-3600.99,4046.49,111.81,0,0,0,''),
(22424,0,36,-3602.32,4051.77,111.81,0,3000,0,'SAY_SKYWING_TREE_UP'),
(22424,0,37,-3609.55,4055.95,112,0,0,0,''),
(22424,0,38,-3620.93,4043.77,111.99,0,0,0,''),
(22424,0,39,-3622.44,4038.95,111.99,0,0,0,''),
(22424,0,40,-3621.64,4025.39,111.99,0,0,0,''),
(22424,0,41,-3609.62,4015.2,111.99,0,0,0,''),
(22424,0,42,-3598.37,4017.72,111.99,0,0,0,''),
(22424,0,43,-3590.21,4026.62,111.99,0,0,0,''),
(22424,0,44,-3586.55,4034.13,112,0,0,0,''),
(22424,0,45,-3580.39,4033.46,112,0,0,0,''),
(22424,0,46,-3568.83,4032.53,107.16,0,0,0,''),
(22424,0,47,-3554.81,4031.23,105.31,0,0,0,''),
(22424,0,48,-3544.39,4030.1,106.58,0,0,0,''),
(22424,0,49,-3531.91,4029.25,111.7,0,0,0,''),
(22424,0,50,-3523.5,4030.24,112.47,0,0,0,''),
(22424,0,51,-3517.48,4037.42,112.66,0,0,0,''),
(22424,0,52,-3510.4,4040.77,112.92,0,0,0,''),
(22424,0,53,-3503.83,4041.35,113.17,0,0,0,''),
(22424,0,54,-3498.31,4040.65,113.11,0,0,0,''),
(22424,0,55,-3494.05,4031.67,113.11,0,0,0,''),
(22424,0,56,-3487.71,4025.58,113.12,0,0,0,''),
(22424,0,57,-3500.42,4012.93,113.11,0,0,0,''),
(22424,0,58,-3510.86,4010.15,113.1,0,0,0,''),
(22424,0,59,-3518.07,4008.62,112.97,0,0,0,''),
(22424,0,60,-3524.74,4014.55,112.41,0,2000,0,'SAY_SKYWING_JUMP'),
(22424,0,61,-3537.81,4008.59,92.53,0,0,0,''),
(22424,0,62,-3546.25,4008.81,92.79,0,0,0,''),
(22424,0,63,-3552.07,4006.48,92.84,0,0,0,''),
(22424,0,64,-3556.29,4000.14,92.92,0,0,0,''),
(22424,0,65,-3556.16,3991.24,92.92,0,0,0,''),
(22424,0,66,-3551.48,3984.28,92.91,0,0,0,''),
(22424,0,67,-3542.9,3981.64,92.91,0,0,0,''),
(22424,0,68,-3534.82,3983.98,92.92,0,0,0,''),
(22424,0,69,-3530.58,3989.91,92.85,0,0,0,''),
(22424,0,70,-3529.85,3998.77,92.59,0,0,0,''),
(22424,0,71,-3534.15,4008.45,92.34,0,0,0,''),
(22424,0,72,-3532.87,4012.97,91.64,0,0,0,''),
(22424,0,73,-3530.57,4023.42,86.82,0,0,0,''),
(22424,0,74,-3528.24,4033.91,85.69,0,0,0,''),
(22424,0,75,-3526.22,4043.75,87.26,0,0,0,''),
(22424,0,76,-3523.84,4054.29,92.42,0,0,0,''),
(22424,0,77,-3522.44,4059.06,92.92,0,0,0,''),
(22424,0,78,-3514.26,4060.72,92.92,0,0,0,''),
(22424,0,79,-3507.76,4065.21,92.92,0,0,0,''),
(22424,0,80,-3503.24,4076.63,92.92,0,0,0,'SAY_SKYWING_SUMMON'),
(22424,0,81,-3504.23,4080.47,92.92,0,7000,0,'SPELL_TRANSFORM'),
(22424,0,82,-3504.23,4080.47,92.92,0,20000,0,'SAY_SKYWING_END'),
(22458,0,1,-3739.91,5393.69,-4.213,0,5000,0,'SAY_LE_KEEP_SAFE'),
(22458,0,2,-3733.33,5389.24,-5.331,0,0,0,''),
(22458,0,3,-3728.77,5385.65,-3.704,0,0,0,''),
(22458,0,4,-3717.27,5379.18,-4.4,0,0,0,''),
(22458,0,5,-3705.63,5379.26,-7.711,0,0,0,''),
(22458,0,6,-3688.28,5379.72,-9.4,0,0,0,''),
(22458,0,7,-3649.19,5389.11,-11.917,0,0,0,''),
(22458,0,8,-3612.79,5392.81,-13.655,0,0,0,''),
(22458,0,9,-3574.87,5412.7,-16.543,0,0,0,''),
(22458,0,10,-3564.44,5422.62,-16.104,0,0,0,''),
(22458,0,11,-3553.39,5444.73,-12.184,0,2500,0,'arivve dig site SAY_LE_ARRIVE'),
(22458,0,12,-3557.29,5465.32,-9.282,0,7500,0,'dig 1'),
(22458,0,13,-3548.1,5453.42,-12.282,0,10000,0,'dig 2 SAY_LE_BURIED pause'),
(22458,0,14,-3556.58,5446.48,-11.92,0,0,0,'start returning'),
(22458,0,15,-3564.44,5422.62,-16.104,0,0,0,''),
(22458,0,16,-3574.87,5412.7,-16.543,0,0,0,''),
(22458,0,17,-3612.79,5392.81,-13.655,0,0,0,''),
(22458,0,18,-3649.19,5389.11,-11.917,0,0,0,''),
(22458,0,19,-3688.28,5379.72,-9.4,0,0,0,''),
(22458,0,20,-3705.63,5379.26,-7.711,0,0,0,''),
(22458,0,21,-3717.27,5379.18,-4.4,0,0,0,''),
(22458,0,22,-3728.77,5385.65,-3.704,0,0,0,''),
(22458,0,23,-3733.33,5389.24,-5.331,0,0,0,''),
(22458,0,24,-3739.91,5393.69,-4.213,0,0,0,''),
(22861,0,1,-3567.673,390.162,31.162,0,0,0,''),
(22861,0,2,-3558.926,410.604,29.328,0,0,0,''),
(22861,0,3,-3569.454,408.951,29.623,0,0,0,''),
(22861,0,4,-3552.207,393.217,31.119,0,0,0,''),
(22861,0,5,-3565.558,398.519,30.333,0,0,0,''),
(22861,0,6,-3550.822,406.948,30.070,0,0,0,''),
(22861,0,7,-3573.805,401.071,30.696,0,0,0,''),
(22861,0,8,-3569.454,408.951,29.623,0,0,0,''),
(22861,0,9,-3560.967,404.486,29.785,0,0,0,''),
(22863,0,1,-3567.673,390.162,31.162,0,0,0,''),
(22863,0,2,-3558.926,410.604,29.328,0,0,0,''),
(22863,0,3,-3569.454,408.951,29.623,0,0,0,''),
(22863,0,4,-3552.207,393.217,31.119,0,0,0,''),
(22863,0,5,-3565.558,398.519,30.333,0,0,0,''),
(22863,0,6,-3550.822,406.948,30.070,0,0,0,''),
(22863,0,7,-3573.805,401.071,30.696,0,0,0,''),
(22863,0,8,-3569.454,408.951,29.623,0,0,0,''),
(22863,0,9,-3560.967,404.486,29.785,0,0,0,''),
(22864,0,1,-3567.673,390.162,31.162,0,0,0,''),
(22864,0,2,-3558.926,410.604,29.328,0,0,0,''),
(22864,0,3,-3569.454,408.951,29.623,0,0,0,''),
(22864,0,4,-3552.207,393.217,31.119,0,0,0,''),
(22864,0,5,-3565.558,398.519,30.333,0,0,0,''),
(22864,0,6,-3550.822,406.948,30.070,0,0,0,''),
(22864,0,7,-3573.805,401.071,30.696,0,0,0,''),
(22864,0,8,-3569.454,408.951,29.623,0,0,0,''),
(22864,0,9,-3560.967,404.486,29.785,0,0,0,''),
(22916,0,1,7461.49,-3121.06,438.21,0,7000,0,'SAY_START'),
(22916,0,2,7465.26,-3115.5,439.315,0,0,0,''),
(22916,0,3,7470.03,-3109.29,439.333,0,0,0,''),
(22916,0,4,7473.77,-3104.65,442.366,0,0,0,''),
(22916,0,5,7478.67,-3098.55,443.551,0,0,0,''),
(22916,0,6,7482.78,-3093.35,441.883,0,0,0,''),
(22916,0,7,7486.23,-3089.19,439.698,0,0,0,''),
(22916,0,8,7484.64,-3084.55,439.566,0,0,0,''),
(22916,0,9,7477.09,-3084.43,442.132,0,0,0,''),
(22916,0,10,7470.66,-3084.86,443.194,0,0,0,''),
(22916,0,11,7456.51,-3085.83,438.863,0,0,0,''),
(22916,0,12,7446,-3085.59,438.21,0,0,0,''),
(22916,0,13,7444.6,-3084.1,438.323,0,0,0,''),
(22916,0,14,7445.58,-3080.92,439.374,0,5000,0,'collect 1'),
(22916,0,15,7446.18,-3085.36,438.21,0,5000,0,'SAY_RELIC1'),
(22916,0,16,7453.9,-3086.69,439.454,0,0,0,''),
(22916,0,17,7459.41,-3085.5,439.158,0,0,0,''),
(22916,0,18,7465.9,-3085.01,442.329,0,0,0,''),
(22916,0,19,7472.8,-3084.81,443.085,0,0,0,''),
(22916,0,20,7480.58,-3084.56,440.642,0,0,0,''),
(22916,0,21,7484.59,-3084.71,439.568,0,0,0,''),
(22916,0,22,7491.81,-3090.55,440.052,0,0,0,''),
(22916,0,23,7497.13,-3095.34,437.505,0,0,0,''),
(22916,0,24,7496.61,-3113.62,434.554,0,0,0,''),
(22916,0,25,7501.79,-3123.79,435.347,0,0,0,''),
(22916,0,26,7506.6,-3130.78,434.179,0,0,0,''),
(22916,0,27,7504.53,-3133.46,435.579,0,5000,0,'collect 2'),
(22916,0,28,7505.2,-3130.03,434.151,0,15000,0,'SAY_RELIC2'),
(22916,0,29,7502.04,-3124.44,435.298,0,0,0,''),
(22916,0,30,7495.9,-3113.93,434.538,0,0,0,''),
(22916,0,31,7488.79,-3111.1,434.31,0,0,0,''),
(22916,0,32,7477.81,-3105.37,430.541,0,0,0,'summon'),
(22916,0,33,7471.49,-3092.55,429.006,0,0,0,''),
(22916,0,34,7472.35,-3062.72,428.341,0,0,0,''),
(22916,0,35,7472.26,-3054.92,427.15,0,0,0,''),
(22916,0,36,7475.03,-3053.39,428.672,0,5000,0,'collect 3'),
(22916,0,37,7472.4,-3057.21,426.87,0,5000,0,'SAY_RELIC3'),
(22916,0,38,7472.39,-3062.86,428.301,0,0,0,''),
(22916,0,39,7470.24,-3087.69,429.045,0,0,0,''),
(22916,0,40,7475.24,-3099.03,429.917,0,0,0,''),
(22916,0,41,7484.24,-3109.85,432.719,0,0,0,''),
(22916,0,42,7489.1,-3111.31,434.4,0,0,0,''),
(22916,0,43,7497.02,-3108.54,434.798,0,0,0,''),
(22916,0,44,7497.75,-3097.7,437.031,0,0,0,''),
(22916,0,45,7492.53,-3090.12,440.041,0,0,0,''),
(22916,0,46,7490.43,-3085.44,439.807,0,0,0,''),
(22916,0,47,7501.02,-3069.7,441.875,0,0,0,''),
(22916,0,48,7509.15,-3064.67,445.012,0,0,0,''),
(22916,0,49,7515.78,-3060.16,445.727,0,0,0,''),
(22916,0,50,7516.46,-3058.11,445.682,0,10000,0,'quest credit'),
(23002,0,1,3687.11,-3960.69,31.8726,0,0,0,''),
(23002,0,2,3676.28,-3953.76,29.9396,0,0,0,''),
(23002,0,3,3658.54,-3952.15,30.0414,0,0,0,''),
(23002,0,4,3628.91,-3956.9,29.405,0,0,0,''),
(23002,0,5,3602.54,-3968.16,31.511,0,0,0,''),
(23002,0,6,3564.96,-3978,30.3622,0,0,0,''),
(23002,0,7,3542.47,-3981.81,29.1465,0,0,0,''),
(23002,0,8,3511.34,-3981.25,30.2822,0,0,0,''),
(23002,0,9,3473.45,-3992.67,30.2861,0,0,0,''),
(23002,0,10,3439.1,-4006.73,29.2737,0,0,0,''),
(23002,0,11,3415.66,-4026.24,25.2498,0,0,0,''),
(23002,0,12,3380.88,-4045.38,26.3114,0,0,0,''),
(23002,0,13,3355.23,-4051.42,25.5665,0,0,0,''),
(23002,0,14,3312,-4055.65,28.3297,0,0,0,''),
(23002,0,15,3286.34,-4079.27,28.2464,0,0,0,''),
(23002,0,16,3260.68,-4087.29,31.4043,0,0,0,''),
(23002,0,17,3236.83,-4087.65,32.6894,0,0,0,''),
(23002,0,18,3215.06,-4082.1,32.4181,0,0,0,''),
(23002,0,19,3203.59,-4082.47,32.7436,0,0,0,''),
(23002,0,20,3166.41,-4062.09,33.2357,0,0,0,''),
(23002,0,21,3147.51,-4055.33,33.5683,0,0,0,''),
(23002,0,22,3125.41,-4050.01,34.61,0,0,0,''),
(23002,0,23,3121.16,-4045.07,36.5481,0,0,0,''),
(23002,0,24,3101.54,-4023.78,33.7169,0,0,0,''),
(23002,0,25,3094.16,-4016.89,33.8487,0,0,0,''),
(23002,0,26,3079.57,-4011.01,35.7546,0,0,0,''),
(23002,0,27,3058.83,-4001.71,34.3039,0,0,0,''),
(23002,0,28,3037.83,-3986.6,33.4216,0,0,0,''),
(23002,0,29,3016.93,-3970.83,33.3743,0,0,0,''),
(23002,0,30,2998.05,-3954.89,33.2338,0,0,0,''),
(23002,0,31,2969.35,-3929.27,33.4831,0,0,0,''),
(23002,0,32,2941.23,-3909.56,31.3506,0,0,0,''),
(23002,0,33,2911.42,-3895.07,32.095,0,0,0,''),
(23002,0,34,2892.44,-3875.52,30.8123,0,0,0,''),
(23002,0,35,2870.52,-3858.97,32.1977,0,0,0,''),
(23002,0,36,2865.84,-3836.99,32.1108,0,0,0,''),
(23002,0,37,2850.52,-3814.52,32.8635,0,0,0,''),
(23002,0,38,2836.63,-3796.94,33.1473,0,0,0,''),
(23002,0,39,2820.73,-3780.22,28.6916,0,0,0,''),
(23002,0,40,2795.82,-3770.13,30.1327,0,0,0,''),
(23002,0,41,2773.15,-3765.54,30.2947,0,0,0,''),
(23002,0,42,2742.31,-3761.65,30.1218,0,0,0,''),
(23002,0,43,2708.43,-3748.46,21.2468,0,0,0,''),
(23002,0,44,2661.45,-3741.11,21.9603,0,0,0,''),
(23002,0,45,2623.89,-3735.29,25.8979,0,0,0,''),
(23002,0,46,2585.93,-3728.85,28.5146,0,0,0,''),
(23002,0,47,2554.93,-3730.1,26.6795,0,0,0,''),
(23002,0,48,2538.68,-3721.28,28.1589,0,0,0,''),
(23002,0,49,2508.54,-3708.71,29.6718,0,0,0,''),
(23002,0,50,2474.69,-3710.37,31.0805,0,0,0,''),
(23002,0,51,2456.4,-3698.83,31.6187,0,0,0,''),
(23002,0,52,2430.54,-3701.87,31.0494,0,0,0,''),
(23002,0,53,2390.13,-3681.76,29.5484,0,0,0,''),
(23002,0,54,2357.06,-3673.96,29.8845,0,0,0,''),
(23002,0,55,2330.15,-3672.73,31.1314,0,0,0,''),
(23002,0,56,2302.77,-3665.22,29.411,0,0,0,''),
(23002,0,57,2279.24,-3659.46,29.6247,0,0,0,''),
(23002,0,58,2254.65,-3661.12,29.6984,0,0,0,''),
(23002,0,59,2223.32,-3654.92,31.0149,0,0,0,''),
(23002,0,60,2194.29,-3645.4,32.0417,0,0,0,''),
(23002,0,61,2153.05,-3650.82,31.2292,0,0,0,''),
(23002,0,62,2114.15,-3639.96,31.7371,0,0,0,''),
(23002,0,63,2093.68,-3646.65,31.3745,0,0,0,''),
(23002,0,64,2069.86,-3670.59,30.6172,0,0,0,''),
(23002,0,65,2024.4,-3677.64,29.7682,0,0,0,''),
(23002,0,66,1988.61,-3680.02,31.8937,0,0,0,''),
(23002,0,67,1962.68,-3692.17,32.7811,0,0,0,''),
(23002,0,68,1931.94,-3708.48,31.3641,0,0,0,''),
(23002,0,69,1893.36,-3710.02,33.0193,0,0,0,''),
(23002,0,70,1865.73,-3718.35,32.1664,0,0,0,''),
(23002,0,71,1839.74,-3732.92,32.5322,0,0,0,''),
(23002,0,72,1805.08,-3757.76,32.6295,0,0,0,''),
(23002,0,73,1780.24,-3775.53,30.5931,0,0,0,''),
(23002,0,74,1753.28,-3786.79,30.7445,0,0,0,''),
(23002,0,75,1731.09,-3796.64,36.8866,0,0,0,''),

-- Dragonmaw Races
-- Murg "Oldie" Muckjaw
(23340,0,1,-5100.367,646.988,86.75992,0,0,0,''),
(23340,0,2,-5098.652,661.8313,87.08841,0,0,0,''),
(23340,0,3,-5092.219,664.3353,87.73563,0,0,0,''),
(23340,0,4,-5081.346,664.3699,88.98929,0,1000,0,'start flying'),
(23340,0,5,-5070.526,664.555,92.49088,0,0,0,''),
(23340,0,6,-5058.811,664.4887,96.62975,0,0,0,''),
(23340,0,7,-5046.153,664.4279,97.79643,0,1,0,'start race & failure check'),
(23340,0,8,-5021.002,664.601,115.265,0,0,0,''),
(23340,0,9,-4996.027,664.6683,115.2651,0,1,0,'start attack'),
(23340,0,10,-4957.755,673.371,115.2651,0,0,0,''),
(23340,0,11,-4922.072,686.9524,115.2651,0,0,0,''),
(23340,0,12,-4892.223,711.1226,115.2651,0,0,0,''),
(23340,0,13,-4899.887,760.2698,115.2651,0,0,0,''),
(23340,0,14,-4938.349,778.5325,115.2651,0,0,0,''),
(23340,0,15,-4971.547,797.5603,115.2651,0,0,0,''),
(23340,0,16,-5006.853,803.2005,115.2651,0,0,0,''),
(23340,0,17,-5052.229,803.8342,115.2651,0,0,0,''),
(23340,0,18,-5084.652,784.2522,115.2651,0,0,0,''),
(23340,0,19,-5112.79,768.5128,115.2651,0,0,0,''),
(23340,0,20,-5131.785,755.4612,115.2651,0,0,0,''),
(23340,0,21,-5156.098,731.316,115.2651,0,0,0,''),
(23340,0,22,-5151.251,703.8534,115.2651,0,0,0,''),
(23340,0,23,-5132.957,672.912,115.2651,0,0,0,''),
(23340,0,24,-5098.931,650.7557,115.2651,0,0,0,''),
(23340,0,25,-5062.084,634.9383,135.515,0,0,0,''),
(23340,0,26,-5027.877,621.5608,129.1817,0,0,0,''),
(23340,0,27,-5011.851,622.0031,127.8948,0,0,0,''),
(23340,0,28,-4977.219,627.5208,123.8948,0,0,0,''),
(23340,0,29,-4955.773,632.8429,106.7004,0,0,0,''),
(23340,0,30,-4959.098,655.7993,100.4226,0,0,0,''),
(23340,0,31,-4990.86,667.564,100.3392,0,0,0,''),
(23340,0,32,-5016.541,664.3353,95.70948,0,0,0,''),
(23340,0,33,-5030.557,664.4094,94.87615,0,0,0,''),
(23340,0,34,-5053.869,664.2513,91.20948,0,0,0,''),
(23340,0,35,-5076.172,664.1518,89.73725,0,1000,0,'stop flying, complete quest, SAY_MUCKJAW_FINISH'),
(23340,0,36,-5100.444,648.4188,86.75992,0,0,0,''),
(23340,0,37,-5088.555,640.8356,86.57706,1.48353,1000,0,''),

-- Trope the Filth-Belcher
(23342,0,1,-5094.403,632.7684,86.1046,0,0,0,''),
(23342,0,2,-5099.95,639.8563,86.36429,0,0,0,''),
(23342,0,3,-5098.756,659.2725,87.07596,0,0,0,''),
(23342,0,4,-5089.76,664.9246,88.03055,0,0,0,''),
(23342,0,5,-5077.378,664.5198,89.23929,0,1000,0,'start flying'),
(23342,0,6,-5070.048,664.1371,90.72233,0,0,0,''),
(23342,0,7,-5058.118,664.4636,94.05566,0,1,0,'start race & failure check'),
(23342,0,8,-5053.475,664.35,98.57059,0,0,0,''),
(23342,0,9,-5038.57,663.9222,98.57059,0,1,0,'start attack'),
(23342,0,10,-5019.736,677.1379,105.154,0,0,0,''),
(23342,0,11,-5005.494,696.0763,105.154,0,0,0,''),
(23342,0,12,-5003.47,736.425,109.7403,0,0,0,''),
(23342,0,13,-5014.967,758.2792,138.2687,0,0,0,''),
(23342,0,14,-5049.924,776.3499,154.991,0,0,0,''),
(23342,0,15,-5088.135,764.1559,147.2965,0,0,0,''),
(23342,0,16,-5109.327,751.0446,147.2965,0,0,0,''),
(23342,0,17,-5139.665,725.494,147.2965,0,0,0,''),
(23342,0,18,-5155.796,699.1519,135.4354,0,0,0,''),
(23342,0,19,-5168.645,673.1464,135.4354,0,0,0,''),
(23342,0,20,-5170.585,645.9178,135.4354,0,0,0,''),
(23342,0,21,-5169.893,609.8694,135.4354,0,0,0,''),
(23342,0,22,-5166.877,572.1507,135.4354,0,0,0,''),
(23342,0,23,-5166.384,546.2206,135.4354,0,0,0,''),
(23342,0,24,-5149.06,514.8846,135.4354,0,0,0,''),
(23342,0,25,-5117.409,493.8354,135.4354,0,0,0,''),
(23342,0,26,-5071.81,485.4799,135.4354,0,0,0,''),
(23342,0,27,-5049.221,487.5898,135.4354,0,0,0,''),
(23342,0,28,-5016.186,497.7139,135.4354,0,0,0,''),
(23342,0,29,-4997.888,513.4749,135.4354,0,0,0,''),
(23342,0,30,-4969.008,530.6617,135.4354,0,0,0,''),
(23342,0,31,-4928.234,535.1053,135.4354,0,0,0,''),
(23342,0,32,-4888.207,519.6169,135.4354,0,0,0,''),
(23342,0,33,-4861.231,501.207,135.4354,0,0,0,''),
(23342,0,34,-4851.031,486.3386,135.4354,0,0,0,''),
(23342,0,35,-4846.359,450.8282,133.6854,0,0,0,''),
(23342,0,36,-4859.994,421.5214,120.2409,0,0,0,''),
(23342,0,37,-4884.146,398.1595,119.4354,0,0,0,''),
(23342,0,38,-4918.608,387.1728,120.0465,0,0,0,''),
(23342,0,39,-4957.875,395.0519,120.0465,0,0,0,''),
(23342,0,40,-4977.369,423.309,107.4076,0,0,0,''),
(23342,0,41,-4999.41,454.0138,107.4076,0,0,0,''),
(23342,0,42,-5015.119,480.192,107.4076,0,0,0,''),
(23342,0,43,-4997.813,519.8029,107.4076,0,0,0,''),
(23342,0,44,-4969.666,535.1463,122.9632,0,0,0,''),
(23342,0,45,-4927.669,545.6583,122.9632,0,0,0,''),
(23342,0,46,-4910.169,580.5968,120.5187,0,0,0,''),
(23342,0,47,-4923.051,609.6332,116.3243,0,0,0,''),
(23342,0,48,-4939.956,638.0411,111.491,0,0,0,''),
(23342,0,49,-4966.525,654.2751,107.3798,0,0,0,''),
(23342,0,50,-4993.766,664.4593,106.2132,0,0,0,''),
(23342,0,51,-5009.622,664.3647,100.2095,0,0,0,''),
(23342,0,52,-5070.048,664.1371,90.72233,0,0,0,''),
(23342,0,53,-5077.378,664.5198,89.23929,0,1000,0,'stop flying, complete quest, SAY_TROPE_END'),
(23342,0,54,-5089.76,664.9246,88.03055,0,0,0,''),
(23342,0,55,-5098.756,659.2725,87.07596,0,0,0,''),
(23342,0,56,-5100.571,656.3891,87.00992,0,0,0,''),
(23342,0,57,-5100.548,640.0574,86.37918,0,0,0,''),
(23342,0,58,-5082.932,631.0391,86.1046,0,0,0,''),
(23342,0,59,-5081.618,640.9318,86.58853,0,0,0,''),
(23342,0,60,-5081.618,640.9318,86.58853,1.5708,1000,0,''),

-- Corlok the Vet
(23344,0,1,-5084.937,633.847,86.23929,0,0,0,''),
(23344,0,2,-5098.979,634.7803,86.10049,0,0,0,''),
(23344,0,3,-5101.355,648.9661,86.75992,0,0,0,''),
(23344,0,4,-5098.781,663.6365,86.99918,0,0,0,''),
(23344,0,5,-5079.276,664.713,89.1636,0,0,0,''),
(23344,0,6,-5070.444,664.3661,89.46468,0,1000,0,'start flying'),
(23344,0,7,-5058.834,663.9255,92.7363,0,0,0,''),
(23344,0,8,-5037.865,660.0336,100.0141,0,0,0,''),
(23344,0,9,-5024.242,636.4905,112.4249,0,1,0,'start race & failure check'),
(23344,0,10,-5005.374,618.8023,128.8068,0,0,0,''),
(23344,0,11,-4966.559,600.2526,129.0568,0,0,0,''),
(23344,0,12,-4965.978,571.0858,129.0568,0,1,0,'start attack'),
(23344,0,13,-4991.398,536.4358,129.0568,0,0,0,''),
(23344,0,14,-5020.616,520.3257,129.0568,0,0,0,''),
(23344,0,15,-5055.504,518.8444,129.0568,0,0,0,''),
(23344,0,16,-5102.8,514.1487,129.0568,0,0,0,''),
(23344,0,17,-5149.532,501.9677,124.2234,0,0,0,''),
(23344,0,18,-5181.648,487.4429,124.2234,0,0,0,''),
(23344,0,19,-5203.604,463.6165,124.2234,0,0,0,''),
(23344,0,20,-5222.085,438.5107,124.2234,0,0,0,''),
(23344,0,21,-5233.957,425.7707,124.2234,0,0,0,''),
(23344,0,22,-5246.468,408.2125,114.3901,0,0,0,''),
(23344,0,23,-5263.944,380.5766,82.77898,0,0,0,''),
(23344,0,24,-5281.137,345.5704,75.14012,0,0,0,''),
(23344,0,25,-5284.635,312.4421,75.14012,0,0,0,''),
(23344,0,26,-5263.003,275.3078,75.14012,0,0,0,''),
(23344,0,27,-5245.181,265.0789,73.25124,0,0,0,''),
(23344,0,28,-5215.76,265.471,73.25124,0,0,0,''),
(23344,0,29,-5205.017,235.1499,76.72346,0,0,0,''),
(23344,0,30,-5209.554,197.7829,76.72346,0,0,0,''),
(23344,0,31,-5208.264,150.2344,76.72346,0,0,0,''),
(23344,0,32,-5191.553,109.1558,93.66789,0,0,0,''),
(23344,0,33,-5182.564,85.73655,107.4735,0,0,0,''),
(23344,0,34,-5168.655,57.20128,127.9457,0,0,0,''),
(23344,0,35,-5135.729,46.99013,139.529,0,0,0,''),
(23344,0,36,-5105.781,50.70128,139.529,0,0,0,''),
(23344,0,37,-5071.623,45.81543,139.529,0,0,0,''),
(23344,0,38,-5034.686,35.1466,139.529,0,0,0,''),
(23344,0,39,-5005.947,1.385308,139.529,0,0,0,''),
(23344,0,40,-5000.307,-36.84798,139.529,0,0,0,''),
(23344,0,41,-5008.465,-64.05198,139.529,0,0,0,''),
(23344,0,42,-5035.053,-104.2141,139.529,0,0,0,''),
(23344,0,43,-5072.071,-126.9249,139.529,0,0,0,''),
(23344,0,44,-5107.338,-132.0435,139.529,0,0,0,''),
(23344,0,45,-5147.941,-120.1514,139.529,0,0,0,''),
(23344,0,46,-5172.304,-102.5539,139.529,0,0,0,''),
(23344,0,47,-5189.557,-64.99957,139.529,0,0,0,''),
(23344,0,48,-5192.558,-46.88466,139.529,0,0,0,''),
(23344,0,49,-5207.221,-7.998264,139.529,0,0,0,''),
(23344,0,50,-5208.898,16.18804,139.529,0,0,0,''),
(23344,0,51,-5210.004,47.67188,139.529,0,0,0,''),
(23344,0,52,-5229.471,71.02235,139.529,0,0,0,''),
(23344,0,53,-5259.859,84.19976,139.529,0,0,0,''),
(23344,0,54,-5280.491,98.11871,139.529,0,0,0,''),
(23344,0,55,-5304.442,119.2904,128.4734,0,0,0,''),
(23344,0,56,-5313.475,160.796,102.8068,0,0,0,''),
(23344,0,57,-5320.646,207.804,102.8068,0,0,0,''),
(23344,0,58,-5315.592,246.9388,102.8068,0,0,0,''),
(23344,0,59,-5282.777,257.4017,102.8068,0,0,0,''),
(23344,0,60,-5234.207,264.51,91.19567,0,0,0,''),
(23344,0,61,-5196.716,279.2937,75.72343,0,0,0,''),
(23344,0,62,-5181.703,301.3474,77.83456,0,0,0,''),
(23344,0,63,-5179.592,316.7443,77.83456,0,0,0,''),
(23344,0,64,-5177.286,343.4835,77.83456,0,0,0,''),
(23344,0,65,-5198.281,388.5124,82.30679,0,0,0,''),
(23344,0,66,-5190.085,423.921,103.7234,0,0,0,''),
(23344,0,67,-5161.914,456.2695,103.7234,0,0,0,''),
(23344,0,68,-5143.627,481.9556,103.7234,0,0,0,''),
(23344,0,69,-5108.331,497.8173,103.7234,0,0,0,''),
(23344,0,70,-5065.657,502.8121,103.7234,0,0,0,''),
(23344,0,71,-5026.661,514.3782,103.7234,0,0,0,''),
(23344,0,72,-4986.503,536.506,103.7234,0,0,0,''),
(23344,0,73,-4954.44,567.9017,103.7234,0,0,0,''),
(23344,0,74,-4956.34,597.6046,103.7234,0,0,0,''),
(23344,0,75,-4974.388,635.569,103.7234,0,0,0,''),
(23344,0,76,-4996.327,664.3044,96.39012,0,0,0,''),
(23344,0,77,-5021.77,663.95,91.90392,0,0,0,''),
(23344,0,78,-5041.207,664.0852,91.32059,0,0,0,''),
(23344,0,79,-5066.215,663.694,89.59836,0,1000,0,'stop flying, complete quest, SAY_CORLOK_END'),
(23344,0,80,-5080.021,661.6674,89.30286,0,0,0,''),
(23344,0,81,-5085.771,660.9174,88.55286,0,0,0,''),
(23344,0,82,-5090.771,660.1674,88.05286,0,0,0,''),
(23344,0,83,-5096.771,659.4174,87.55286,0,0,0,''),
(23344,0,84,-5097.771,659.1674,87.55286,0,0,0,''),
(23344,0,85,-5099.328,643.6407,86.50735,0,0,0,''),
(23344,0,86,-5092.37,632.1734,86.08287,0,0,0,''),
(23344,0,87,-5092.217,632.455,86.57751,0,0,0,''),
(23344,0,88,-5072.73,632.0778,86.1046,0,0,0,''),
(23344,0,89,-5072.652,632.3533,86.28184,0,1000,0,''),

-- Wing Commander Ichman
(23344,0,90,-5072.988,640.0335,86.48074,0,0,0,''),
(23345,0,1,-5091.531,631.9266,86.1046,0,0,0,''),
(23345,0,2,-5099.329,638.2125,86.36429,0,0,0,''),
(23345,0,3,-5098.52,660.3553,87.11429,0,0,0,''),
(23345,0,4,-5078.784,664.3688,89.23929,0,1000,0,'start flying + start race & failure check'),
(23345,0,5,-5070.004,664.1256,89.47318,0,0,0,''),
(23345,0,6,-5045.988,664.5428,90.34837,0,0,0,''),
(23345,0,7,-5020.852,664.3274,91.73727,0,0,0,''),
(23345,0,8,-4988.704,666.3051,92.82061,0,0,0,''),
(23345,0,9,-4951.686,663.5135,100.9675,0,0,0,''),
(23345,0,10,-4922.128,645.6582,103.7175,0,0,0,''),
(23345,0,11,-4914.606,596.9916,103.7175,0,0,0,''),
(23345,0,12,-4939.847,571.1878,103.7175,0,1,0,'start attack'),
(23345,0,13,-4969.139,547.8412,103.7175,0,0,0,''),
(23345,0,14,-4993.97,528.6561,103.7175,0,0,0,''),
(23345,0,15,-5002.941,506.9452,103.7175,0,0,0,''),
(23345,0,16,-5004.989,482.9723,103.7175,0,0,0,''),
(23345,0,17,-4994.157,457.0183,103.7175,0,0,0,''),
(23345,0,18,-4982.753,435.6739,103.7175,0,0,0,''),
(23345,0,19,-4968.979,413.1156,103.7175,0,0,0,''),
(23345,0,20,-4952.016,390.8795,103.7175,0,0,0,''),
(23345,0,21,-4918.822,380.1662,86.273,0,0,0,''),
(23345,0,22,-4891.81,384.8297,103.7175,0,0,0,''),
(23345,0,23,-4870.952,388.6259,103.7175,0,0,0,''),
(23345,0,24,-4839.091,373.6963,103.7175,0,0,0,''),
(23345,0,25,-4835.827,344.1341,103.7175,0,0,0,''),
(23345,0,26,-4847.139,297.3308,103.7175,0,0,0,''),
(23345,0,27,-4873.639,255.0166,103.7175,0,0,0,''),
(23345,0,28,-4912.098,234.3047,85.68967,0,0,0,''),
(23345,0,29,-4948.94,231.966,89.38414,0,0,0,''),
(23345,0,30,-4985.909,223.645,109.2452,0,0,0,''),
(23345,0,31,-5005.907,180.5232,103.7175,0,0,0,''),
(23345,0,32,-5002.692,144.4231,93.21744,0,0,0,''),
(23345,0,33,-5000.446,107.8185,97.6619,0,0,0,''),
(23345,0,34,-5001.402,88.90202,97.6619,0,0,0,''),
(23345,0,35,-4992.563,59.60102,97.6619,0,0,0,''),
(23345,0,36,-4973.051,35.04601,97.6619,0,0,0,''),
(23345,0,37,-4950.327,16.39974,97.6619,0,0,0,''),
(23345,0,38,-4919.713,-7.58724,97.6619,0,0,0,''),
(23345,0,39,-4895.118,-24.4617,97.6619,0,0,0,''),
(23345,0,40,-4873.843,-56.46213,97.6619,0,0,0,''),
(23345,0,41,-4878.95,-78.37022,97.6619,0,0,0,''),
(23345,0,42,-4900.977,-96.32227,124.5508,0,0,0,''),
(23345,0,43,-4938.917,-97.65365,140.8841,0,0,0,''),
(23345,0,44,-4947.491,-70.94228,139.2697,0,0,0,''),
(23345,0,45,-4962.078,-45.11686,118.7141,0,0,0,''),
(23345,0,46,-4985.609,-3.707574,104.4363,0,0,0,''),
(23345,0,47,-5007.204,21.71202,104.4363,0,0,0,''),
(23345,0,48,-5035.34,34.8622,104.4363,0,0,0,''),
(23345,0,49,-5077.724,45.16526,83.8252,0,0,0,''),
(23345,0,50,-5122.017,50.87055,85.74186,0,0,0,''),
(23345,0,51,-5163.295,41.71484,112.9919,0,0,0,''),
(23345,0,52,-5185.467,38.44889,155.7974,0,0,0,''),
(23345,0,53,-5215.355,32.12261,192.6029,0,0,0,''),
(23345,0,54,-5251.429,11.90918,192.6029,0,0,0,''),
(23345,0,55,-5270.692,35.87956,192.6029,0,0,0,''),
(23345,0,56,-5269.639,61.32357,192.6029,0,0,0,''),
(23345,0,57,-5229.232,77.38042,192.6029,0,0,0,''),
(23345,0,58,-5199.305,80.3291,192.6029,0,0,0,''),
(23345,0,59,-5162.087,96.18935,163.0474,0,0,0,''),
(23345,0,60,-5116.333,114.7767,156.1029,0,0,0,''),
(23345,0,61,-5104.885,161.9479,144.0197,0,0,0,''),
(23345,0,62,-5106.104,210.7065,141.1308,0,0,0,''),
(23345,0,63,-5100.365,254.993,151.6304,0,0,0,''),
(23345,0,64,-5094.819,294.7229,165.9231,0,0,0,''),
(23345,0,65,-5072.266,323.0579,173.471,0,0,0,''),
(23345,0,66,-5042.428,354.9113,179.3321,0,0,0,''),
(23345,0,67,-5031.137,402.7699,182.737,0,0,0,''),
(23345,0,68,-5023.528,437.5792,182.737,0,0,0,''),
(23345,0,69,-5021.058,461.7069,157.5425,0,0,0,''),
(23345,0,70,-5061.279,480.6051,134.3203,0,0,0,''),
(23345,0,71,-5101.122,489.5711,134.3203,0,0,0,''),
(23345,0,72,-5131.542,478.5534,134.3203,0,0,0,''),
(23345,0,73,-5154.209,459.8275,134.3203,0,0,0,''),
(23345,0,74,-5173.368,424.9651,134.3203,0,0,0,''),
(23345,0,75,-5176.187,388.6364,134.3203,0,0,0,''),
(23345,0,76,-5178.223,359.5666,134.3203,0,0,0,''),
(23345,0,77,-5177.092,334.0616,134.3203,0,0,0,''),
(23345,0,78,-5184.358,290.9243,134.3203,0,0,0,''),
(23345,0,79,-5200.437,273.4439,134.3203,0,0,0,''),
(23345,0,80,-5243.265,262.7923,134.3203,0,0,0,''),
(23345,0,81,-5263.354,272.4469,134.3203,0,0,0,''),
(23345,0,82,-5280.749,294.9872,134.3203,0,0,0,''),
(23345,0,83,-5285.514,322.7867,134.3203,0,0,0,''),
(23345,0,84,-5279.47,359.6388,134.3203,0,0,0,''),
(23345,0,85,-5272.3,381.9901,134.3203,0,0,0,''),
(23345,0,86,-5257.371,401.5872,134.3203,0,0,0,''),
(23345,0,87,-5234.978,422.1708,128.9314,0,0,0,''),
(23345,0,88,-5221.283,434.7353,124.7925,0,0,0,''),
(23345,0,89,-5204.375,446.8191,115.237,0,0,0,''),
(23345,0,90,-5166.356,467.1954,115.237,0,0,0,''),
(23345,0,91,-5123.749,489.6505,115.237,0,0,0,''),
(23345,0,92,-5089.289,501.8938,115.237,0,0,0,''),
(23345,0,93,-5050.138,500.7674,115.237,0,0,0,''),
(23345,0,94,-5022.075,515.1059,115.237,0,0,0,''),
(23345,0,95,-5004.622,526.2526,115.237,0,0,0,''),
(23345,0,96,-4987.251,539.7435,115.237,0,0,0,''),
(23345,0,97,-4967.14,557.4308,115.237,0,0,0,''),
(23345,0,98,-4954.635,576.6056,115.237,0,0,0,''),
(23345,0,99,-4941.903,596.954,115.237,0,0,0,''),
(23345,0,100,-4937.899,622.1005,115.237,0,0,0,''),
(23345,0,101,-4942.416,647.0491,115.237,0,0,0,''),
(23345,0,102,-4955.862,657.9393,105.1536,0,0,0,''),
(23345,0,103,-4987.663,666.5894,95.04252,0,0,0,''),
(23345,0,104,-5012.211,663.7874,93.4317,0,0,0,''),
(23345,0,105,-5029.556,664.0626,92.48726,0,0,0,''),
(23345,0,106,-5046.771,664.192,91.59837,0,0,0,''),
(23345,0,107,-5065.815,664.1027,89.51505,0,1000,0,'stop flying, complete quest, SAY_ICHMAN_END'),
(23345,0,108,-5097.221,643.8181,86.61429,0,0,0,''),
(23345,0,109,-5082.632,631.6423,86.1046,0,0,0,''),
(23345,0,110,-5065.899,639.8352,86.49668,0,0,0,''),
(23345,0,111,-5066.307,640.2136,86.49668,1.55334,1000,0,''),

-- Wing Commander Mulverick
(23346,0,1,-5078.127,632.7935,86.1046,0,0,0,''),
(23346,0,2,-5096.167,634.1729,86.26175,0,0,0,''),
(23346,0,3,-5099.563,647.3327,86.73929,0,0,0,''),
(23346,0,4,-5096.421,663.5131,87.23123,0,0,0,''),
(23346,0,5,-5078.932,664.5538,89.23929,0,1000,0,'start flying'),
(23346,0,6,-5047.585,664.4786,90.09837,0,0,0,''),
(23346,0,7,-5029.147,664.6062,91.84837,0,0,0,''),
(23346,0,8,-5001.704,665.0206,94.57059,0,0,0,''),
(23346,0,9,-4980.508,664.0265,94.57059,0,1,0,'start race & failure check'),
(23346,0,10,-4951.832,664.3872,93.03965,0,0,0,''),
(23346,0,11,-4913.112,661.8121,90.53965,0,0,0,''),
(23346,0,12,-4887.881,689.6855,86.73409,0,1,0,'start attack'),
(23346,0,13,-4885.2,719.2115,86.73409,0,0,0,''),
(23346,0,14,-4905.209,763.2064,86.73409,0,0,0,''),
(23346,0,15,-4948.712,781.429,78.37299,0,0,0,''),
(23346,0,16,-4977.905,809.7921,95.7341,0,0,0,''),
(23346,0,17,-4988.85,809.7207,89.65078,0,0,0,''),
(23346,0,18,-5011.912,818.0668,89.65078,0,0,0,''),
(23346,0,19,-5051.185,819.7384,89.65078,0,0,0,''),
(23346,0,20,-5083.584,797.4017,89.65078,0,0,0,''),
(23346,0,21,-5114.206,773.3442,72.73411,0,0,0,''),
(23346,0,22,-5143.489,764.9902,65.06745,0,0,0,''),
(23346,0,23,-5185.277,762.5067,65.06745,0,0,0,''),
(23346,0,24,-5213.917,765.2517,65.06745,0,0,0,''),
(23346,0,25,-5257.457,784.7656,65.06745,0,0,0,''),
(23346,0,26,-5278.247,777.8218,65.06745,0,0,0,''),
(23346,0,27,-5294.249,755.3405,65.06745,0,0,0,''),
(23346,0,28,-5296.898,729.1802,65.06745,0,0,0,''),
(23346,0,29,-5289.127,702.0369,65.06745,0,0,0,''),
(23346,0,30,-5273.802,666.1729,62.56745,0,0,0,''),
(23346,0,31,-5267.913,638.2552,52.5119,0,0,0,''),
(23346,0,32,-5260.284,622.6142,56.06746,0,0,0,''),
(23346,0,33,-5245.017,611.9426,66.20634,0,0,0,''),
(23346,0,34,-5200.1,604.6636,86.98518,0,0,0,''),
(23346,0,35,-5181.164,590.6979,89.29072,0,0,0,''),
(23346,0,36,-5185.517,548.6089,94.31851,0,0,0,''),
(23346,0,37,-5167.705,522.7582,94.31851,0,0,0,''),
(23346,0,38,-5137.539,504.2317,94.31851,0,0,0,''),
(23346,0,39,-5102.646,499.6689,94.31851,0,0,0,''),
(23346,0,40,-5064.482,492.9311,94.31851,0,0,0,''),
(23346,0,41,-5035.057,477.9901,104.0685,0,0,0,''),
(23346,0,42,-5001.804,454.6688,104.0685,0,0,0,''),
(23346,0,43,-4978.692,423.2803,104.0685,0,0,0,''),
(23346,0,44,-4968.225,394.6599,104.0685,0,0,0,''),
(23346,0,45,-4970.736,356.3612,97.09627,0,0,0,''),
(23346,0,46,-4981.942,320.5984,96.26293,0,0,0,''),
(23346,0,47,-5000.338,287.576,96.26293,0,0,0,''),
(23346,0,48,-5024.314,263.4618,97.12405,0,0,0,''),
(23346,0,49,-5033.601,245.4317,99.70215,0,0,0,''),
(23346,0,50,-5043.806,220.8815,109.3681,0,0,0,''),
(23346,0,51,-5055.694,189.8815,121.6043,0,0,0,''),
(23346,0,52,-5065.022,177.9086,129.7014,0,0,0,''),
(23346,0,53,-5093.455,175.0507,135.2028,0,0,0,''),
(23346,0,54,-5104.901,192.9338,143.8139,0,0,0,''),
(23346,0,55,-5108.269,222.5996,143.8139,0,0,0,''),
(23346,0,56,-5096.198,260.2229,155.9698,0,0,0,''),
(23346,0,57,-5092.704,300.505,168.0287,0,0,0,''),
(23346,0,58,-5064.827,341.2665,173.6232,0,0,0,''),
(23346,0,59,-5046.666,359.6774,175.4009,0,0,0,''),
(23346,0,60,-5017.936,369.2787,177.6509,0,0,0,''),
(23346,0,61,-4990.092,370.2358,177.6509,0,0,0,''),
(23346,0,62,-4946.644,373.1902,173.2898,0,0,0,''),
(23346,0,63,-4904.017,381.0535,154.401,0,0,0,''),
(23346,0,64,-4873.26,409.159,154.401,0,0,0,''),
(23346,0,65,-4858.19,449.1999,154.401,0,0,0,''),
(23346,0,66,-4860.118,475.3252,154.401,0,0,0,''),
(23346,0,67,-4874.003,522.4359,154.401,0,0,0,''),
(23346,0,68,-4907.657,539.0103,154.401,0,0,0,''),
(23346,0,69,-4937.984,542.9128,154.401,0,0,0,''),
(23346,0,70,-4984.786,534.6582,154.401,0,0,0,''),
(23346,0,71,-5016.347,516.8373,154.401,0,0,0,''),
(23346,0,72,-5045.822,504.1454,154.401,0,0,0,''),
(23346,0,73,-5067.003,496.7763,154.401,0,0,0,''),
(23346,0,74,-5092.522,477.6822,154.401,0,0,0,''),
(23346,0,75,-5113.307,451.8052,150.7898,0,0,0,''),
(23346,0,76,-5145.342,430.1466,148.2621,0,0,0,''),
(23346,0,77,-5159.413,401.4334,136.8176,0,0,0,''),
(23346,0,78,-5177.601,380.3644,135.0954,0,0,0,''),
(23346,0,79,-5177.559,335.6026,122.401,0,0,0,''),
(23346,0,80,-5183.724,292.3144,122.401,0,0,0,''),
(23346,0,81,-5214.583,264.1207,122.401,0,0,0,''),
(23346,0,82,-5238.495,265.3249,113.0954,0,0,0,''),
(23346,0,83,-5270.4,270.1617,113.0954,0,0,0,''),
(23346,0,84,-5295.31,259.1142,113.0954,0,0,0,''),
(23346,0,85,-5311.033,222.1083,113.0954,0,0,0,''),
(23346,0,86,-5342.249,192.5318,113.0954,0,0,0,''),
(23346,0,87,-5347.233,154.9113,113.0954,0,0,0,''),
(23346,0,88,-5320.385,112.0802,113.0954,0,0,0,''),
(23346,0,89,-5278.581,88.2244,113.0954,0,0,0,''),
(23346,0,90,-5258.171,55.24957,113.0954,0,0,0,''),
(23346,0,91,-5251.674,14.11502,113.0954,0,0,0,''),
(23346,0,92,-5249.777,-21.18479,125.651,0,0,0,''),
(23346,0,93,-5257.274,-54.0995,129.7343,0,0,0,''),
(23346,0,94,-5277.006,-99.74707,132.9566,0,0,0,''),
(23346,0,95,-5297.232,-105.6787,130.4566,0,0,0,''),
(23346,0,96,-5340.311,-104.8584,130.4566,0,0,0,''),
(23346,0,97,-5367.746,-96.76975,130.4566,0,0,0,''),
(23346,0,98,-5383.334,-62.23264,130.4566,0,0,0,''),
(23346,0,99,-5365.698,-35.27062,130.4566,0,0,0,''),
(23346,0,100,-5340.349,-18.70963,130.4566,0,0,0,''),
(23346,0,101,-5309.31,9.529188,130.4566,0,0,0,''),
(23346,0,102,-5289.217,14.57964,130.4566,0,0,0,''),
(23346,0,103,-5263.405,19.9974,130.4566,0,0,0,''),
(23346,0,104,-5232.503,13.45378,130.4566,0,0,0,''),
(23346,0,105,-5204.176,-19.43283,130.4566,0,0,0,''),
(23346,0,106,-5182.804,-62.57574,118.0121,0,0,0,''),
(23346,0,107,-5174.803,-84.48155,118.0121,0,0,0,''),
(23346,0,108,-5162.558,-103.8193,118.0121,0,0,0,''),
(23346,0,109,-5131.967,-113.7209,118.0121,0,0,0,''),
(23346,0,110,-5096.79,-121.8593,118.0121,0,0,0,''),
(23346,0,111,-5065.522,-109.8579,118.0121,0,0,0,''),
(23346,0,112,-5042.261,-85.59082,118.0121,0,0,0,''),
(23346,0,113,-5019.137,-62.09039,98.81767,0,0,0,''),
(23346,0,114,-4997.549,-36.64453,92.62322,0,0,0,''),
(23346,0,115,-4968.95,-1.824978,85.90099,0,0,0,''),
(23346,0,116,-4934.683,27.94217,70.40099,0,0,0,''),
(23346,0,117,-4902.898,45.49132,59.95655,0,0,0,''),
(23346,0,118,-4884.461,46.51064,45.45655,0,0,0,''),
(23346,0,119,-4867.73,45.39312,-0.654566,0,0,0,''),
(23346,0,120,-4854.791,31.71137,-4.710121,0,0,0,''),
(23346,0,121,-4852.745,-1.659288,1.817656,0,0,0,''),
(23346,0,122,-4861.769,-29.40929,36.5562,0,0,0,''),
(23346,0,123,-4882.453,-29.94065,74.63494,0,0,0,''),
(23346,0,124,-4910.108,-50.92372,104.1269,0,0,0,''),
(23346,0,125,-4922.273,-66.68522,140.7146,0,0,0,''),
(23346,0,126,-4949.065,-51.4962,135.4091,0,0,0,''),
(23346,0,127,-4952.385,-44.90668,116.6313,0,0,0,''),
(23346,0,128,-4964.531,-33.20681,108.0758,0,0,0,''),
(23346,0,129,-4983.418,4.737739,108.0758,0,0,0,''),
(23346,0,130,-4966.226,46.29297,108.0758,0,0,0,''),
(23346,0,131,-4946.933,79.27724,108.0758,0,0,0,''),
(23346,0,132,-4927.336,105.7801,108.0758,0,0,0,''),
(23346,0,133,-4903.621,139.424,108.0758,0,0,0,''),
(23346,0,134,-4908.835,173.5289,108.0758,0,0,0,''),
(23346,0,135,-4899.304,207.2578,108.0758,0,0,0,''),
(23346,0,136,-4894.789,241.9639,100.548,0,0,0,''),
(23346,0,137,-4876.024,262.596,100.548,0,0,0,''),
(23346,0,138,-4851.944,285.5693,100.548,0,0,0,''),
(23346,0,139,-4845.693,327.7279,100.548,0,0,0,''),
(23346,0,140,-4845.241,359.997,95.88132,0,0,0,''),
(23346,0,141,-4847.69,391.2897,95.88132,0,0,0,''),
(23346,0,142,-4859.135,419.0597,95.88132,0,0,0,''),
(23346,0,143,-4856.898,465.8593,95.88132,0,0,0,''),
(23346,0,144,-4876.195,509.1212,95.88132,0,0,0,''),
(23346,0,145,-4916.561,535.1924,95.88132,0,0,0,''),
(23346,0,146,-4941.459,566.5074,95.88132,0,0,0,''),
(23346,0,147,-4956.068,605.6051,95.88132,0,0,0,''),
(23346,0,148,-4967.994,629.4673,95.88132,0,0,0,''),
(23346,0,149,-4992.872,657.3202,95.88132,0,0,0,''),
(23346,0,150,-5019.961,663.826,95.88132,0,0,0,''),
(23346,0,151,-5049.397,663.9436,98.93688,0,0,0,''),
(23346,0,152,-5085.372,666.438,105.1035,0,0,0,''),
(23346,0,153,-5114.091,670.0516,107.3258,0,0,0,''),
(23346,0,154,-5129.29,676.2471,107.3258,0,0,0,''),
(23346,0,155,-5134.548,693.2084,107.3258,0,0,0,''),
(23346,0,156,-5124.754,712.9388,107.3258,0,0,0,''),
(23346,0,157,-5091.654,711.2447,104.5202,0,0,0,''),
(23346,0,158,-5069.821,674.8909,104.5202,0,0,0,''),
(23346,0,159,-5061.352,667.0403,104.5202,0,0,0,''),
(23346,0,160,-5019.67,666.8051,104.5202,0,0,0,''),
(23346,0,161,-5000.67,667.4341,104.5202,0,0,0,''),
(23346,0,162,-4993.282,669.9588,104.5202,0,0,0,''),
(23346,0,163,-5002.192,664.5203,97.93169,0,0,0,''),
(23346,0,164,-5017.093,664.3544,96.15392,0,0,0,''),
(23346,0,165,-5040.194,664.2045,90.40389,0,0,0,''),
(23346,0,166,-5090.223,663.8318,87.97513,0,1000,0,'stop flying, complete quest, SAY_END_MULVERICK'),
(23346,0,167,-5101.206,660.5425,87.00992,0,0,0,''),
(23346,0,168,-5102.085,639.8577,86.38492,0,0,0,''),
(23346,0,169,-5091.961,630.3558,86.1317,0,0,0,''),
(23346,0,170,-5078.229,633.2795,86.1046,0,0,0,''),
(23346,0,171,-5062.336,632.8517,86.17667,0,0,0,''),
(23346,0,172,-5060.474,640.3013,86.64524,1.50098,1000,0,''),

-- Captain Skyshatter
(23348,0,1,-5097.342,638.2053,86.37064,0,0,0,''),
(23348,0,2,-5099.037,662.449,87.04227,0,0,0,''),
(23348,0,3,-5079.384,664.2811,89.15067,0,1000,0,'start flying'),
(23348,0,4,-5059.083,664.368,90.95825,0,0,0,''),
(23348,0,5,-5034.28,664.2311,90.95825,0,0,0,''),
(23348,0,6,-5008.98,664.1523,90.95825,0,0,0,''),
(23348,0,7,-4987.987,664.8333,93.12492,0,4000,0,'SAY_MID_SKYSHATTER, start race & failure check'),
(23348,0,8,-4957.929,667.1135,89.67696,0,0,0,''),
(23348,0,9,-4923.51,679.0427,89.67696,0,0,0,''),
(23348,0,10,-4907.643,683.4996,89.67696,0,1,0,'start attack'),
(23348,0,11,-4883.626,694.9488,89.67696,0,0,0,''),
(23348,0,12,-4850.153,676.8022,89.67696,0,0,0,''),
(23348,0,13,-4864.901,632.5569,89.67696,0,0,0,''),
(23348,0,14,-4910.243,633.4714,89.67696,0,0,0,''),
(23348,0,15,-4946.938,659.1007,89.67696,0,0,0,''),
(23348,0,16,-4995.127,667.0613,94.92697,0,0,0,''),
(23348,0,17,-5029.882,666.0309,94.92698,0,0,0,''),
(23348,0,18,-5065.315,666.9732,103.6214,0,0,0,''),
(23348,0,19,-5091.274,666.2161,117.4825,0,0,0,''),
(23348,0,20,-5132.497,678.9468,117.4825,0,0,0,''),
(23348,0,21,-5160.828,695.7098,117.4825,0,0,0,''),
(23348,0,22,-5189.027,715.3826,117.4825,0,0,0,''),
(23348,0,23,-5218.202,743.7205,117.4825,0,0,0,''),
(23348,0,24,-5246.878,753.9114,117.4825,0,0,0,''),
(23348,0,25,-5291.249,736.5722,117.4825,0,0,0,''),
(23348,0,26,-5297.695,704.0183,117.4825,0,0,0,''),
(23348,0,27,-5294.14,669.929,117.4825,0,0,0,''),
(23348,0,28,-5283.434,645.6896,117.4825,0,0,0,''),
(23348,0,29,-5274.705,617.4304,117.4825,0,0,0,''),
(23348,0,30,-5275.441,576.7112,117.4825,0,0,0,''),
(23348,0,31,-5275.686,532.8275,94.37141,0,0,0,''),
(23348,0,32,-5270.501,488.1923,78.17696,0,0,0,''),
(23348,0,33,-5275.354,443.7866,67.01029,0,0,0,''),
(23348,0,34,-5282.691,395.9322,67.01029,0,0,0,''),
(23348,0,35,-5284.521,357.4507,67.01029,0,0,0,''),
(23348,0,36,-5297.132,321.8339,75.31584,0,0,0,''),
(23348,0,37,-5305.334,287.9489,67.01029,0,0,0,''),
(23348,0,38,-5324.948,244.8683,67.01029,0,0,0,''),
(23348,0,39,-5356.357,222.8803,74.17696,0,0,0,''),
(23348,0,40,-5372.028,179.2107,74.17696,0,0,0,''),
(23348,0,41,-5334.379,148.9864,74.17696,0,0,0,''),
(23348,0,42,-5315.291,119.4997,74.17696,0,0,0,''),
(23348,0,43,-5293.327,83.57639,74.17696,0,0,0,''),
(23348,0,44,-5277.183,50.7105,74.17696,0,0,0,''),
(23348,0,45,-5255.611,8.40484,74.17696,0,0,0,''),
(23348,0,46,-5224.429,-18.24121,101.3436,0,0,0,''),
(23348,0,47,-5203.018,-50.26172,113.1214,0,0,0,''),
(23348,0,48,-5177.758,-81.79276,113.1214,0,0,0,''),
(23348,0,49,-5153.055,-109.1681,113.1214,0,0,0,''),
(23348,0,50,-5112.361,-123.955,113.1214,0,0,0,''),
(23348,0,51,-5075.532,-120.1032,113.1214,0,0,0,''),
(23348,0,52,-5034.087,-103.7401,113.1214,0,0,0,''),
(23348,0,53,-4989.264,-83.96908,113.1214,0,0,0,''),
(23348,0,54,-4952.199,-82.11546,113.1214,0,0,0,''),
(23348,0,55,-4930.279,-71.91537,134.939,0,0,0,''),
(23348,0,56,-4903.256,-46.99186,119.5501,0,0,0,''),
(23348,0,57,-4892.428,-14.39844,119.5501,0,0,0,''),
(23348,0,58,-4901.102,29.4554,119.5501,0,0,0,''),
(23348,0,59,-4921.162,50.85612,119.5501,0,0,0,''),
(23348,0,60,-4960.637,51.15321,119.5501,0,0,0,''),
(23348,0,61,-5007.94,44.28179,113.9668,0,0,0,''),
(23348,0,62,-5049.16,46.34722,113.9668,0,0,0,''),
(23348,0,63,-5081.966,51.23449,113.9668,0,0,0,''),
(23348,0,64,-5109.11,52.6926,113.9668,0,0,0,''),
(23348,0,65,-5153.785,53.41374,113.9668,0,0,0,''),
(23348,0,66,-5194.33,78.2347,113.9668,0,0,0,''),
(23348,0,67,-5200.143,112.9766,113.9668,0,0,0,''),
(23348,0,68,-5210.659,155.0445,113.9668,0,0,0,''),
(23348,0,69,-5209.587,202.7329,113.9668,0,0,0,''),
(23348,0,70,-5206.819,249.7953,113.9668,0,0,0,''),
(23348,0,71,-5195.308,281.5933,113.9668,0,0,0,''),
(23348,0,72,-5175.319,312.8564,113.9668,0,0,0,''),
(23348,0,73,-5176.313,350.4632,113.9668,0,0,0,''),
(23348,0,74,-5197.359,384.4846,113.9668,0,0,0,''),
(23348,0,75,-5206.275,401.4015,113.9668,0,0,0,''),
(23348,0,76,-5210.944,427.5983,113.9668,0,0,0,''),
(23348,0,77,-5204.977,464.8688,113.9668,0,0,0,''),
(23348,0,78,-5193.151,501.4104,113.9668,0,0,0,''),
(23348,0,79,-5180.75,540.2638,113.9668,0,0,0,''),
(23348,0,80,-5174.904,567.8201,113.9668,0,0,0,''),
(23348,0,81,-5159.236,594.9044,113.9668,0,0,0,''),
(23348,0,82,-5132.958,612.1032,113.9668,0,0,0,''),
(23348,0,83,-5111.183,618.4528,115.6335,0,0,0,''),
(23348,0,84,-5082.095,643.4512,127.189,0,0,0,''),
(23348,0,85,-5051.162,666.2181,134.5779,0,0,0,''),
(23348,0,86,-5023.218,672.1176,134.5779,0,0,0,''),
(23348,0,87,-4998.408,675.9113,134.5779,0,0,0,''),
(23348,0,88,-4964.067,667.4047,134.5779,0,0,0,''),
(23348,0,89,-4945.452,649.2565,134.5779,0,0,0,''),
(23348,0,90,-4929.717,618.0428,134.5779,0,0,0,''),
(23348,0,91,-4933.029,597.6074,134.5779,0,0,0,''),
(23348,0,92,-4950.765,560.4185,134.5779,0,0,0,''),
(23348,0,93,-4986.156,534.7485,134.5779,0,0,0,''),
(23348,0,94,-5027.694,507.0074,140.5223,0,0,0,''),
(23348,0,95,-5063.99,482.6105,161.0223,0,0,0,''),
(23348,0,96,-5079.088,444.7179,188.5779,0,0,0,''),
(23348,0,97,-5081.959,401.0051,210.8873,0,0,0,''),
(23348,0,98,-5080.922,379.7276,222.7484,0,0,0,''),
(23348,0,99,-5088.472,350.8958,222.7484,0,0,0,''),
(23348,0,100,-5101.865,322.2873,222.7484,0,0,0,''),
(23348,0,101,-5108.479,280.2418,222.7484,0,0,0,''),
(23348,0,102,-5108.403,250.032,222.7484,0,0,0,''),
(23348,0,103,-5116.868,216.9851,222.7484,0,0,0,''),
(23348,0,104,-5109.27,175.0246,222.7484,0,0,0,''),
(23348,0,105,-5088.899,150.6683,222.7484,0,0,0,''),
(23348,0,106,-5060.405,142.9474,222.7484,0,0,0,''),
(23348,0,107,-5048.861,136.8668,222.7484,0,0,0,''),
(23348,0,108,-5028.831,136.0967,222.7484,0,0,0,''),
(23348,0,109,-5014.031,134.3629,222.7484,0,0,0,''),
(23348,0,110,-4976.749,133.0627,190.4706,0,0,0,''),
(23348,0,111,-4939.158,144.4543,190.4706,0,0,0,''),
(23348,0,112,-4909.588,167.5473,190.4706,0,0,0,''),
(23348,0,113,-4901.982,203.8995,160.6373,0,0,0,''),
(23348,0,114,-4923.405,237.4692,163.8317,0,0,0,''),
(23348,0,115,-4957.846,242.4023,167.1373,0,0,0,''),
(23348,0,116,-4979.879,241.2091,167.1373,0,0,0,''),
(23348,0,117,-5010.276,221.4066,167.1373,0,0,0,''),
(23348,0,118,-5046.926,191.8748,167.1373,0,0,0,''),
(23348,0,119,-5085.212,182.271,167.1373,0,0,0,''),
(23348,0,120,-5091.655,219.2357,167.1373,0,0,0,''),
(23348,0,121,-5091.76,256.4972,198.3595,0,0,0,''),
(23348,0,122,-5073.088,284.9821,198.3595,0,0,0,''),
(23348,0,123,-5056.151,305.8941,198.3595,0,0,0,''),
(23348,0,124,-5033.989,340.742,198.3595,0,0,0,''),
(23348,0,125,-5013.551,366.6549,198.3595,0,0,0,''),
(23348,0,126,-4985.13,398.5603,198.3595,0,0,0,''),
(23348,0,127,-4951.698,419.7676,198.3595,0,0,0,''),
(23348,0,128,-4927.77,437.4049,174.8595,0,0,0,''),
(23348,0,129,-4905.905,452.8599,174.8595,0,0,0,''),
(23348,0,130,-4904.514,474.8152,174.8595,0,0,0,''),
(23348,0,131,-4909.353,491.2098,174.8595,0,0,0,''),
(23348,0,132,-4906.713,508.8005,174.8595,0,0,0,''),
(23348,0,133,-4904.04,524.842,174.8595,0,0,0,''),
(23348,0,134,-4903.656,545.8247,174.8595,0,0,0,''),
(23348,0,135,-4910.834,565.4141,174.8595,0,0,0,''),
(23348,0,136,-4923.354,594.0482,147.2484,0,0,0,''),
(23348,0,137,-4936.446,624.7623,130.3039,0,0,0,''),
(23348,0,138,-4970.291,650.181,106.6095,0,0,0,''),
(23348,0,139,-4991.406,664.5876,103.2761,0,0,0,''),
(23348,0,140,-5100.779,656.943,87.00992,0,1000,0,'stop flying, complete quest, SAY_END_SKYSHATTER'),
(23348,0,141,-5099.944,642.5331,86.43362,0,0,0,''),
(23348,0,142,-5095.807,633.6784,86.23929,0,0,0,''),
(23348,0,143,-5084.066,627.6938,85.9796,0,0,0,''),
(23348,0,144,-5074.522,625.243,85.75792,0,0,0,''),
(23348,0,145,-5074.878,625.4485,85.72105,0,1000,0,''),

(23383,0,1,-4109.42,3034.16,344.168,0,5000,0,'SAY_ESCORT_START'),
(23383,0,2,-4113.27,3035.99,344.071,0,0,0,''),
(23383,0,3,-4120.02,3032.22,344.074,0,0,0,''),
(23383,0,4,-4124.41,3026.33,344.151,0,0,0,''),
(23383,0,5,-4128.82,3026.65,344.035,0,0,0,''),
(23383,0,6,-4138.91,3028.95,338.92,0,0,0,''),
(23383,0,7,-4152.59,3031.23,336.913,0,0,0,''),
(23383,0,8,-4169.81,3034.3,342.047,0,0,0,''),
(23383,0,9,-4174.63,3036.04,343.457,0,0,0,''),
(23383,0,10,-4174.4,3044.98,343.862,0,0,0,''),
(23383,0,11,-4176.63,3052.01,344.077,0,0,0,''),
(23383,0,12,-4183.66,3058.9,344.15,0,0,0,''),
(23383,0,13,-4182.92,3065.41,342.574,0,0,0,'ambush'),
(23383,0,14,-4182.06,3070.56,337.644,0,3000,0,'SAY_AMBUSH_END'),
(23383,0,15,-4181.26,3077.13,331.59,0,0,0,''),
(23383,0,16,-4179.99,3086.1,325.571,0,0,0,''),
(23383,0,17,-4178.77,3090.1,323.955,0,0,0,''),
(23383,0,18,-4177.96,3093.87,323.839,0,5000,0,'SAY_ESCORT_COMPLETE'),
(23383,0,19,-4166.25,3106.51,320.961,0,0,0,''),
(23383,0,20,-3716.813,3784.913,302.7957,0,5000,0,'SAY_ESCORT_START'),
(23383,0,21,-3713.891,3785.578,302.9066,0,0,0,''),
(23383,0,22,-3701.317,3788.437,302.1635,0,0,0,''),
(23383,0,23,-3691.435,3784.761,298.6149,0,0,0,''),
(23383,0,24,-3679.718,3780.137,295.1252,0,0,0,''),
(23383,0,25,-3678.255,3779.54,294.927,0,0,0,''),
(23383,0,26,-3665.416,3774.3,296.5848,0,0,0,''),
(23383,0,27,-3652.483,3769.122,301.5699,0,0,0,'ambush'),
(23383,0,28,-3655.307,3758.888,301.9366,0,3000,0,'SAY_AMBUSH_END'),
(23383,0,29,-3647.862,3745.505,302.1535,0,0,0,''),
(23383,0,30,-3653.096,3731.482,293.2672,0,0,0,''),
(23383,0,31,-3658.584,3718.315,283.3129,0,0,0,''),
(23383,0,32,-3660.324,3713.304,281.269,0,5000,0,'SAY_ESCORT_COMPLETE'),
(23383,0,33,-3652.71,3704.63,280.646,0,0,0,''),
(23383,0,34,-3669.901,3388.204,312.9493,0,5000,0,'SAY_ESCORT_START'),
(23383,0,35,-3672.78,3371.396,312.1327,0,0,0,''),
(23383,0,36,-3666.887,3367.723,312.2306,0,0,0,''),
(23383,0,37,-3670.773,3352.389,306.7109,0,0,0,''),
(23383,0,38,-3673.615,3342.424,304.9935,0,0,0,'ambush'),
(23383,0,39,-3679.221,3318.798,311.436,0,3000,0,'SAY_AMBUSH_END'),
(23383,0,40,-3687.093,3318.624,311.8744,0,0,0,''),
(23383,0,41,-3697.118,3315.984,312.1466,0,0,0,''),
(23383,0,42,-3703.314,3309.11,312.2234,0,0,0,''),
(23383,0,43,-3713.242,3311.646,307.328,0,0,0,''),
(23383,0,44,-3724.522,3313.627,297.02,0,0,0,''),
(23383,0,45,-3734.422,3315.625,292.3213,0,5000,0,'SAY_ESCORT_COMPLETE'),
(23383,0,46,-3730.11,3326.87,290.705,0,0,0,''),
(23784,0,1,1377.88,-6421.48,1.323,0,0,0,'SAY_ESCORT_START'),
(23784,0,2,1377.52,-6415.2,1.515,0,0,0,''),
(23784,0,3,1379.99,-6401.92,2.428,0,8000,0,'SAY_FIRE_1'),
(23784,0,4,1379.99,-6401.92,2.428,0,5000,0,'SAY_FIRE_2'),
(23784,0,5,1379.75,-6398.58,2.829,0,0,0,''),
(23784,0,6,1383.77,-6392.13,3.639,0,0,0,''),
(23784,0,7,1395.3,-6381.13,4.711,0,0,0,''),
(23784,0,8,1407.24,-6372.45,6.434,0,0,0,''),
(23784,0,9,1421.05,-6363.2,6.43,0,0,0,''),
(23784,0,10,1424.19,-6358.81,6.443,0,0,0,''),
(23784,0,11,1422.74,-6350.55,6.138,0,0,0,''),
(23784,0,12,1419.15,-6342.66,5.811,0,0,0,''),
(23784,0,13,1414.31,-6336.42,5.865,0,0,0,''),
(23784,0,14,1405.47,-6336.25,6.21,0,0,0,''),
(23784,0,15,1400.87,-6340.45,6.415,0,4000,0,'set fire'),
(23784,0,16,1400.87,-6340.45,6.415,0,15000,0,'SAY_SUPPLIES_1'),
(23784,0,17,1406,-6335.55,6.19,0,0,0,''),
(23784,0,18,1421.08,-6337.9,5.517,0,0,0,''),
(23784,0,19,1436.05,-6341.19,6.772,0,0,0,''),
(23784,0,20,1449.41,-6344.46,8.267,0,0,0,''),
(23784,0,21,1465.83,-6345.1,7.695,0,2000,0,'set fire'),
(23784,0,22,1470.89,-6347.97,7.576,0,3000,0,'set fire'),
(23784,0,23,1470.89,-6347.97,7.576,0,4000,0,'SAY_SUPPLIES_2'),
(23784,0,24,1464.28,-6345.29,7.896,0,0,0,''),
(23784,0,25,1463.02,-6339.78,7.718,0,0,0,''),
(23784,0,26,1465.49,-6335.77,7.332,0,0,0,''),
(23784,0,27,1479.17,-6325.06,7.44,0,0,0,''),
(23784,0,28,1489.4,-6315.13,8.296,0,0,0,''),
(23784,0,29,1502.83,-6311.04,6.77,0,0,0,''),
(23784,0,30,1506.4,-6317.25,7.299,0,4000,0,'set fire'),
(23784,0,31,1506.4,-6317.25,7.299,0,2000,0,'laugh'),
(23784,0,32,1506.4,-6317.25,7.299,0,10000,0,'SAY_SUPPLIES_COMPLETE'),
(23784,0,33,1506.4,-6317.25,7.299,0,5000,0,'SAY_SUPPLIES_ESCAPE'),
(23784,0,34,1511,-6295.9,6.193,0,0,0,''),
(23784,0,35,1517.06,-6275.86,5.202,0,0,0,''),
(23784,0,36,1523.78,-6258.19,4.561,0,0,0,''),
(23784,0,37,1529.62,-6244.45,5.823,0,0,0,''),
(23784,0,38,1537.66,-6224.8,6.349,0,0,0,''),
(23784,0,39,1545.3,-6214.43,6.917,0,0,0,''),
(23784,0,40,1556.08,-6203.81,6.566,0,0,0,''),
(23784,0,41,1567.2,-6194.42,7.262,0,0,0,'SAY_ARRIVE_BASE'),
(23784,0,42,1582.46,-6183.63,7.145,0,0,0,''),
(23784,0,43,1593.28,-6173.17,7.319,0,0,0,''),
(23784,0,44,1604.47,-6164.39,8.379,0,0,0,''),
(23784,0,45,1617.78,-6157.25,9.323,0,2000,0,'quest complete'),
(23784,0,46,1644.7,-6149.58,7.357,0,0,0,''),

-- Shade of the Horseman - 5 potential paths per village: fly-in, circling (repeats), bombing run, landing, and failed
-- Goldshire fly-in
(23543,0,1,-9528.89,-13.002889,73.86143,0,0,0,''),
(23543,0,2,-9523.886,-4.666667,76.19994,0,0,0,''),
(23543,0,3,-9520.465,1.032389,77.79867,0,0,0,''),
(23543,0,4,-9518.044,6.40549,75.67703,0,0,0,''),
(23543,0,5,-9512.475,16.872776,67.71999,0,0,0,''),
(23543,0,6,-9508.296,26.19363,63.52721,0,0,0,''),
(23543,0,7,-9503.8545,38.499405,61.943882,0,0,0,''),
(23543,0,8,-9495.724,56.92806,61.91611,0,0,0,''),
(23543,0,9,-9478.471,63.092773,62.277218,0,0,0,''),
(23543,0,10,-9462.028,61.116917,63.527206,0,0,0,''),
(23543,0,11,-9453.977,54.283585,65.805,0,0,0,''),
(23543,0,12,-9452.838,43.518665,71.501724,0,0,0,''),
(23543,0,13,-9453.156,38.32932,75.47117,0,0,0,''),
(23543,0,14,-9460.292,22.948893,79.69938,0,0,0,''),
(23543,0,15,-9468.031,24.063368,77.86606,0,0,0,''),
(23543,0,16,-9469.286,35.76888,78.58825,0,0,0,''),
(23543,0,17,-9467.505,41.748158,78.532715,0,0,0,''),
(23543,0,18,-9466.919,47.072647,78.11605,0,0,0,''),
(23543,0,19,-9465.989,57.74121,77.44939,0,0,0,''),
(23543,0,20,-9461.377,70.96696,79.08823,0,0,0,''),
(23543,0,21,-9460.521,81.40658, 77.28273,0,0,0,''),
(23543,0,22,-9462.997,95.15023,78.4494,0,0,0,''),
(23543,0,23,-9458.008,113.51186,74.67155,0,0,0,''),
(23543,0,24,-9448.143,119.62956,75.882545,0,0,0,''),
(23543,0,25,-9437.128,116.73294,77.80933,0,0,0,''),
(23543,0,26,-9438.458,108.43853,75.087105,0,0,0,''),
(23543,0,27,-9442.521,99.024445,76.78157,0,0,0,''),
(23543,0,28,-9452.035,91.196724,74.198265,0,0,0,''),
(23543,0,29,-9454.947,82.3852,72.86491,0,0,0,''),
(23543,0,30,-9459.013,66.9802,75.28155,0,0,0,''),
(23543,0,31,-9464.004,51.56722,75.00376,0,0,0,''),
(23543,0,32,-9464.365,41.856556,86.14274,0,0,0,''),
(23543,0,33,-9458.908,37.09961,90.89276,0,0,0,'drop start fire aura and start circling path'),
-- Goldshire repeating flying path
(23543,1,1,-9458.022,40.245117,92.30876,0,0,0,''),
(23543,1,2,-9450.923,36.35547,90.32265,0,0,0,''),
(23543,1,3,-9438.02,37.94233,80.96152,0,0,0,''),
(23543,1,4,-9433.388,58.618977,76.072624,0,0,0,''),
(23543,1,5,-9440.101,78.850044,82.10047,0,0,0,''),
(23543,1,6,-9446.246,83.248024,84.699814,0,0,0,''),
(23543,1,7,-9454.6,84.89887,84.96437,0,0,0,''),
(23543,1,8,-9463.398,85.500465,86.32915,0,0,0,''),
(23543,1,9,-9471.447,86.92722,87.25962,0,0,0,''),
(23543,1,10,-9477.656,84.96373,88.336586,0,0,0,''),
(23543,1,11,-9487.309,64.76156,89.53107,0,0,0,''),
(23543,1,12,-9485.582,57.892143,92.558815,0,0,0,''),
(23543,1,13,-9481.641,52.05431,98.25323,0,0,0,''),
(23543,1,14,-9472.576,44.70432,96.91988,0,0,0,'repeat path'),
-- Goldshire bombing run
(23543,2,1,-9516.815,69.78885,64.714806,0,0,0,''),
(23543,2,2,-9511.014,68.71951,63.932377,0,0,0,''),
(23543,2,3,-9501.435,66.57286,63.93238,0,0,0,'gain conflag aura'),
(23543,2,4,-9491.825,65.90039,63.75802,0,0,0,''),
(23543,2,5,-9481.912,65.391335,63.758015,0,0,0,''),
(23543,2,6,-9463.705,64.37109,63.758015,0,0,0,''),
(23543,2,7,-9444.268,62.11567,63.758015,0,0,0,''),
(23543,2,8,-9435.942,53.781086,73.49323,0,0,0,''),
(23543,2,9,-9430.354,41.913467,75.99323,0,0,0,''),
(23543,2,10,-9438.617,30.099121,80.02098,0,0,0,''),
(23543,2,11,-9452.91,32.343426,88.50429,0,0,0,''),
(23543,2,12,-9460.41,37.271267,94.92096,0,0,0,'drop conflag aura and start circling path'),
-- Goldshire landing to start combat
(23543,3,1,-9441.104,38.53787,89.102554,0,0,0,''),
(23543,3,2,-9444.283,57.705513,70.29694,0,0,0,''),
(23543,3,3,-9457.63,63.229115,62.90801,0,0,0,''),
(23543,3,4,-9476.708,65.72803,56.963596,0,0,0,'dismount'),
-- path 4 reserved for Goldshire leaving/failed

-- Razor Hill fly-in
(23543,5,1,143.9746,-4824.7197,19.112612,0,0,0,''),
(23543,5,2,144.75174,-4824.0903,19.112612,0,0,0,''),
(23543,5,3,147.60309,-4822.569,31.877886,0,0,0,''),
(23543,5,4,165.03798,-4808.246,29.183435,0,0,0,''),
(23543,5,5,178.37848,-4799.3027,28.600115,0,0,0,''),
(23543,5,6,197.22493,-4795.2373,26.072329,0,0,0,''),
(23543,5,7,207.34706,-4781.007,26.29455,0,0,0,''),
(23543,5,8,219.0982,-4764.345,27.100111,0,0,0,''),
(23543,5,9,232.68517,-4747.5996,27.155666,0,0,0,''),
(23543,5,10,246.49826,-4737.4277,26.433428,0,0,0,''),
(23543,5,11,256.6944,-4728.891,26.461216,0,0,0,''),
(23543,5,12,268.82504,-4720.6694,29.93344,0,0,0,''),
(23543,5,13,282.20474,-4711.1353,33.516773,0,0,0,''),
(23543,5,14,301.84875,-4699.555,36.794556,0,0,0,''),
(23543,5,15,316.8697,-4692.682,38.26678,0,0,0,''),
(23543,5,16,325.8506,-4697.7764,43.067436,0,0,0,''),
(23543,5,17,335.89154,-4702.5747,46.482716,0,0,0,''),
(23543,5,18,348.0508,-4722.7026,40.038292,0,0,0,''),
(23543,5,19,340.75363,-4737.78,31.954948,0,0,0,''),
(23543,5,20,335.77603,-4746.7314,25.566074,0,0,0,''),
(23543,5,21,323.4375,-4761.8,35.121597,0,0,0,''),
(23543,5,22,322.27725,-4778.401,43.92717,0,0,0,''),
(23543,5,23,293.87756,-4801.0884,44.427174,0,0,0,''),
(23543,5,24,282.48557,-4799.479,44.316044,0,0,0,''),
(23543,5,25,273.4689,-4793.7275,43.87159,0,0,0,''),
(23543,5,26,269.3105,-4775.4414,40.621597,0,0,0,''),
(23543,5,27,269.03336,-4760.3984,40.260494,0,0,0,''),
(23543,5,28,268.8217,-4746.8003,38.56605,0,0,0,''),
(23543,5,29,277.03488,-4722.398,38.53827,0,0,0,''),
(23543,5,30,282.9832,-4708.815,36.9827,0,0,0,''),
(23543,5,31,293.593,-4694.289,36.62161,0,0,0,''),
(23543,5,32,302.621,-4682.761,34.121635,0,0,0,''),
(23543,5,33,311.6103,-4668.7227,33.843815,0,0,0,''),
(23543,5,34,319.3367,-4664.863,33.482704,0,0,0,''),
(23543,5,35,324.803,-4673.489,36.23274,0,0,0,''),
(23543,5,36,318.41998,-4690.644,39.177162,0,0,0,''),
(23543,5,37,314.56583,-4703.768,36.843822,0,0,0,''),
(23543,5,38,315.03494,-4724.1816,27.510508,0,0,0,''),
(23543,5,39,309.16934,-4754.3247,26.843828,0,0,0,''),
(23543,5,40,312.9096,-4772.7085,34.73272,0,0,0,''),
(23543,5,41,298.57706,-4780.0493,42.677162,0,0,0,''),
(23543,5,42,274.7774,-4786.495,47.010498,0,0,0,''),
(23543,5,43,260.11996,-4779.2354,55.010494,0,0,0,''),
-- Razor Hill repeating flying path
(23543,6,1,326.08002,-4763.8613,55.324356,0,0,0,''),
(23543,6,2,310.22305,-4770.9976,55.324356,0,0,0,''),
(23543,6,3,276.6716,-4782.269,55.324356,0,0,0,''),
(23543,6,4,257.46176,-4782.1934,55.324356,0,0,0,''),
(23543,6,5,239.45589,-4765.5947,55.324356,0,0,0,''),
(23543,6,6,242.61447,-4739.0044,58.379898,0,0,0,''),
(23543,6,7,243.39415,-4733.53,58.046597,0,0,0,''),
(23543,6,8,255.81331,-4694.588,55.324356,0,0,0,''),
(23543,6,9,287.3598,-4671.0884,55.324356,0,0,0,''),
(23543,6,10,314.6122,-4687.773,55.324356,0,0,0,''),
(23543,6,11,333.2819,-4728.9194,55.324356,0,0,0,''),
-- Razor Hill bombing run
(23543,7,1,216.47032,-4744.034,34.084778,0,0,0,''),
(23543,7,2,228.07248,-4741.7603,26.14033,0,0,0,''),
(23543,7,3,245.42557,-4740.2964,19.612547,0,0,0,''),
(23543,7,4,375.64563,-4734.9976,18.064402,0,0,0,''),
(23543,7,5,383.7048,-4730.549,20.592186,0,0,0,''),
(23543,7,6,389.361,-4725.1885,23.50885,0,0,0,''),
(23543,7,7,397.0016,-4719.967,25.981066,0,0,0,''),
(23543,7,8,407.4445,-4719.2817,30.007374,0,0,0,''),
(23543,7,9,408.9253,-4724.897,31.81292,0,0,0,''),
(23543,7,10,401.2223,-4735.3647,39.713146,0,0,0,''),
(23543,7,11,388.65424,-4739.3,44.324276,0,0,0,''),
(23543,7,12,372.4656,-4743.1704,48.018696,0,0,0,''),
(23543,7,13,358.2104,-4741.0425,49.102028,0,0,0,''),
-- Razor Hill landing to start combat
(23543,8,1,292.22507,-4744.8804,63.668335,0,0,0,''),
(23543,8,2,290.93066,-4729.7827,49.723907,0,0,0,''),
(23543,8,3,270.09567,-4735.2295,33.966034,0,0,0,''),
(23543,8,4,275.1832,-4748.7017,25.67209,0,0,0,''),
(23543,8,5,288.1393,-4751.1465,22.449867,0,0,0,''),
(23543,8,6,295.06088,-4745.6196,16.199879,0,0,0,''),
(23543,8,7,291.46738,-4736.76,9.505446,0,0,0,''),
-- path 9 reserved for Razor Hill leaving/failed

-- Falconwing Square fly-in
(23543,10,1,9358.968,-6712.8184,53.869007,0,0,0,''),
(23543,10,2,9366.8955,-6718.7812,55.133724,0,0,0,''),
(23543,10,3,9368.001,-6719.613,55.31009,0,0,0,''),
(23543,10,4,9380.595,-6733.132,53.198975,0,0,0,''),
(23543,10,5,9400.262,-6758.7275,41.226746,0,0,0,''),
(23543,10,6,9421.722,-6779.7036,40.44896,0,0,0,''),
(23543,10,7,9464.911,-6780.5923,38.89339,0,0,0,''),
(23543,10,8,9482.981,-6785.661,43.14343,0,0,0,''),
(23543,10,9,9490.136,-6800.686,45.78231,0,0,0,''),
(23543,10,10,9487.449,-6815.675,40.254536,0,0,0,''),
(23543,10,11,9498.791,-6841.321,36.532314,0,0,0,''),
(23543,10,12,9526.481,-6850.4136,34.532314,0,0,0,''),
(23543,10,13,9550.522,-6837.754,33.89341,0,0,0,''),
(23543,10,14,9556.248,-6816.287,40.810097,0,0,0,''),
(23543,10,15,9552.206,-6787.99,42.03231,0,0,0,''),
(23543,10,16,9536.909,-6776.1914,40.504524,0,0,0,''),
(23543,10,17,9522.86,-6776.1816,40.504524,0,0,0,''),
(23543,10,18,9505.9795,-6781.1113,40.504524,0,0,0,''),
(23543,10,19,9486.364,-6801.3647,40.504524,0,0,0,''),
(23543,10,20,9491.923,-6829.4126,40.504524,0,0,0,''),
(23543,10,21,9510.964,-6845.8994,37.643394,0,0,0,''),
(23543,10,22,9537.286,-6846.9434,37.060062,0,0,0,''),
(23543,10,23,9554.466,-6827.153,39.11562,0,0,0,''),
(23543,10,24,9554.451,-6800.906,43.393456,0,0,0,''),
(23543,10,25,9550.833,-6791.1284,43.671238,0,0,0,''),
-- Falconwing Square repeating flying path
(23543,11,1,9513.6045,-6780.225,39.576164,0,0,0,''),
(23543,11,2,9500.165,-6787.9917,34.54836,0,0,0,''),
(23543,11,3,9493.263,-6806.903,29.243404,0,0,0,''),
(23543,11,4,9501.082,-6823.5254,26.187853,0,0,0,''),
(23543,11,5,9514.464,-6830.1953,27.215801,0,0,0,''),
(23543,11,6,9530.422,-6826.478,27.910246,0,0,0,''),
(23543,11,7,9540.124,-6813.363,29.243578,0,0,0,''),
(23543,11,8,9539.683,-6796.4653,29.60469,0,0,0,''),
(23543,11,9,9531.025,-6785.385,33.410248,0,0,0,''),
(23543,11,10,9519.258,-6780.304,39.382477,0,0,0,''),
-- Falconwing Square bombing run
-- first and last two points cut because they seem to take Shade too far away from the action
-- (23543,12,1,9553.519,-6748.7295,58.524868,0,0,0,''),
-- (23543,12,2,9545,-6759.143,43.116673,0,0,0,''),
(23543,12,1,9537.361,-6771.127,33.46585,0,0,0,''),
(23543,12,2,9530.247,-6781.988,28.584341,0,0,0,''),
(23543,12,3,9516.364,-6784.76,26.130943,0,0,0,''),
(23543,12,4,9500.437,-6788.924,26.130945,0,0,0,''),
(23543,12,5,9492.3125,-6800.029,26.130945,0,0,0,''),
(23543,12,6,9492.866,-6811.8423,26.130945,0,0,0,''),
(23543,12,7,9495.72,-6823.203,24.353159,0,0,0,''),
(23543,12,8,9508.9,-6833.459,23.936493,0,0,0,''),
(23543,12,9,9528.668,-6830.139,24.964275,0,0,0,''),
(23543,12,10,9540.255,-6817.1235,26.130945,0,0,0,''),
(23543,12,11,9538.219,-6804.21,26.130945,0,0,0,''),
(23543,12,12,9531.502,-6793.2837,26.130945,0,0,0,''),
(23543,12,13,9532.217,-6779.5728,25.174309,0,0,0,''),
(23543,12,14,9540.592,-6767.161,30.78543,0,0,0,''),
-- (23543,12,17,9549.324,-6754.8916,38.368767,0,0,0,''),
-- (23543,12,18,9557.369,-6743.6655,49.06321,0,0,0,''),
-- Falconwing Square landing to start combat
(23543,13,1,9517.885,-6804.7227,38.992825,0,0,0,''),
(23543,13,2,9503.403,-6805.651,34.909477,0,0,0,''),
(23543,13,3,9497.088,-6815.921,28.65947,0,0,0,''),
(23543,13,4,9500.1,-6823.9077,22.687254,0,0,0,''),
(23543,13,5,9513.411,-6830.0527,17.159468,0,0,0,''),
-- Falconwing Squre leaving/failed
(23543,14,1,9478.175,-6780.9233,40.604687,0,0,0,''),
(23543,14,2,9448.0625,-6780.479,36.755592,0,0,0,''),
(23543,14,3,9422.266,-6779.9126,33.311172,0,0,0,''),
(23543,14,4,9399.9,-6779.5767,30.005604,0,0,0,''),
(23543,14,5,9381.576,-6755.0083,38.811176,0,0,0,''),
(23543,14,6,9362.724,-6732.7246,41.592228,0,0,0,''),
(23543,14,7,9335.946,-6706.4033,69.779076,0,0,0,'despawn'),
/*(23543,14,8,9513.6045,-6780.225,39.576164,0,0,0,''),
(23543,14,9,9500.165,-6787.9917,34.54836,0,0,0,''),
(23543,14,10,9493.263,-6806.903,29.243404,0,0,0,''),
(23543,14,11,9501.082,-6823.5254,26.187853,0,0,0,''),
(23543,14,12,9514.464,-6830.1953,27.215801,0,0,0,''),
(23543,14,13,9530.422,-6826.478,27.910246,0,0,0,''),
(23543,14,14,9540.124,-6813.363,29.243578,0,0,0,''),
(23543,14,15,9539.683,-6796.4653,29.60469,0,0,0,''),
(23543,14,16,9531.025,-6785.385,33.410248,0,0,0,''),
(23543,14,17,9519.258,-6780.304,39.382477,0,0,0,''),*/

-- Brill fly-in
(23543,15,1,2345.1487,255.90237,64.03157,0,0,0,''),
(23543,15,2,2344.1875,255.62674,64.03157,0,0,0,''),
(23543,15,3,2331.2969,252.12674,60.345207,0,0,0,''),
(23543,15,4,2298.2327,237.8125,53.337044,0,0,0,''),
(23543,15,5,2268.9966,220.55208,55.278713,0,0,0,''),
(23543,15,6,2237.6685,208.93056,55.699127,0,0,0,''),
(23543,15,7,2220.7744,220.69098,51.115345,0,0,0,''),
(23543,15,8,2228.7969,247.97049,49.00469,0,0,0,''),
(23543,15,9,2246.382,258.68228,51.75467,0,0,0,''),
(23543,15,10,2258.5435,275.70834,55.39357,0,0,0,''),
(23543,15,11,2256.8003,298.55902,49.902645,0,0,0,''),
(23543,15,12,2270.4045,322.32986,45.921356,0,0,0,''),
(23543,15,13,2258.9307,345.01215,46.900726,0,0,0,''),
(23543,15,14,2237.698,353.34723,51.671345,0,0,0,''),
(23543,15,15,2214.033,343.08334,47.838005,0,0,0,''),
(23543,15,16,2211.8247,314.55035,51.560253,0,0,0,''),
(23543,15,17,2237.1145,293.0521,52.001656,0,0,0,''),
(23543,15,18,2257.717,279.04166,49.729984,0,0,0,''),
(23543,15,19,2269.783,264.04514,51.213852,0,0,0,''),
(23543,15,20,2297.3489,244.2743,52.868813,0,0,0,''),
(23543,15,21,2325.573,254.75694,56.619034,0,0,0,''),
(23543,15,22,2336.2432,278.63715,60.949146,0,0,0,''),
(23543,15,23,2324.9895,301.4271,56.42602,0,0,0,''),
(23543,15,24,2302.0435,314.72223,53.599422,0,0,0,''),
(23543,15,25,2282.4253,318.967,57.7979,0,0,0,''),
-- Brill repeating flying path
(23543,16,1,2273.1182,312.70312,58.101673,0,0,0,''),
(23543,16,2,2262.212,296.3889,59.626377,0,0,0,''),
(23543,16,3,2254.1008,277.33682,60.984165,0,0,0,''),
(23543,16,4,2263.8142,251.99306,61.514797,0,0,0,''),
(23543,16,5,2290.3872,238.3316,62.119976,0,0,0,''),
(23543,16,6,2320.132,244.9618,61.059525,0,0,0,''),
(23543,16,7,2339.0798,273.65973,64.23735,0,0,0,''),
(23543,16,8,2328.04,308.3507,63.126297,0,0,0,''),
(23543,16,9,2298.3403,323.4132,63.126297,0,0,0,''),
-- Brill bombing run
/* this data doesn't really make sense at all for the TBC Brill building setup, create entirely custom bombing run path below
(23543,17,1,2216.2322,283.40192,60.60691,0,0,0,''),
(23543,17,2,2239.5396,276.45377,52.41246,0,0,0,''),
(23543,17,3,2243.9995,272.8236,50.27311,0,0,0,''),
(23543,17,4,2262.9583,260.44446,45.52138,0,0,0,''),
(23543,17,5,2283.1997,245.52083,41.91029,0,0,0,''),
(23543,17,6,2304.804,229.61285,47.104725,0,0,0,''),
(23543,17,7,2332.3142,232.59029,48.286316,0,0,0,''),
(23543,17,8,2366.378,243.2109,57.048183,0,0,0,''),
(23543,17,9,2393.3013,253.63957,53.909298,0,0,0,''),
(23543,17,10,2429.8079,274.14725,60.103737,0,0,0,''),
(23543,17,11,2448.0479,298.25085,63.60374,0,0,0,''),
(23543,17,12,2452.4993,327.9196,60.379204,0,0,0,''),
(23543,17,13,2440.5276,353.00528,54.139595,0,0,0,''),
(23543,17,14,2410.7764,384.28244,62.538177,0,0,0,''),
(23543,17,15,2376.1497,396.82803,63.89259,0,0,0,''),
(23543,17,16,2328.804,388.13223,56.809254,0,0,0,''),
(23543,17,17,2299.4253,371.62674,57.14257,0,0,0,''),
(23543,17,18,2284.0295,343.61285,58.812614,0,0,0,''),*/
(23543,17,1,2234.324463,227.105118,54.241577,0,0,0,''),
(23543,17,2,2234.153320,240.489487,47.211327,0,0,0,''),
(23543,17,3,2235.416748,256.880798,42.248161,0,0,0,''),
(23543,17,4,2241.581299,263.810059,41.500320,0,0,0,''),
(23543,17,5,2249.730713,270.245728,43.034447,0,0,0,''),
(23543,17,6,2263.242920,279.816315,43.641808,0,0,0,''),
(23543,17,7,2269.666016,288.249420,43.931927,0,0,0,''),
(23543,17,8,2273.695801,294.545349,44.623772,0,0,0,''),
(23543,17,9,2277.970947,302.511108,44.598499,0,0,0,''),
(23543,17,10,2276.782227,312.956268,45.077377,0,0,0,''),
(23543,17,11,2270.265625,322.901184,45.606342,0,0,0,''),
(23543,17,12,2264.817383,327.053009,47.439075,0,0,0,''),
(23543,17,13,2258.188721,327.754486,49.632935,0,0,0,''),
(23543,17,14,2250.567627,323.681671,50.503342,0,0,0,''),
(23543,17,15,2245.687988,316.202179,52.789753,0,0,0,''),
(23543,17,16,2239.841797,307.849670,55.346718,0,0,0,''),
(23543,17,17,2234.961426,299.218323,56.688698,0,0,0,''),
(23543,17,18,2229.577637,289.802643,60.284992,0,0,0,''),
-- Brill landing to start combat
(23543,18,1,2229.6367,263.53378,57.38664,0,0,0,''),
(23543,18,2,2240.4617,272.1149,46.636642,0,0,0,''),
(23543,18,3,2251.0635,281.87854,39.053314,0,0,0,''),
(23543,18,4,2258.9932,292.59796,34.608852,0,0,0,''),
-- path 19 reserved for Brill leaving/failed

-- Azure Watch fly-in
(23543,20,1,-4086.784,-12718.129,52.912674,0,0,0,''),
(23543,20,2,-4086.299,-12717.254,52.912674,0,0,0,''),
(23543,20,3,-4088.5786,-12716.784,52.24837,0,0,0,''),
(23543,20,4,-4135.0244,-12704.223,47.836826,0,0,0,''),
(23543,20,5,-4174.161,-12677.224,59.624916,0,0,0,''),
(23543,20,6,-4194.844,-12640.737,54.60615,0,0,0,''),
(23543,20,7,-4196.282,-12596.291,59.035934,0,0,0,''),
(23543,20,8,-4203.887,-12565.525,60.485664,0,0,0,''),
(23543,20,9,-4210.0703,-12549.334,64.64713,0,0,0,''),
(23543,20,10,-4217.829,-12528.411,66.05455,0,0,0,''),
(23543,20,11,-4216.966,-12497.731,69.38791,0,0,0,''),
(23543,20,12,-4195.2266,-12471.737,71.30455,0,0,0,''),
(23543,20,13,-4170.0947,-12459.106,74.05459,0,0,0,''),
(23543,20,14,-4150.3545,-12464.034,72.47126,0,0,0,''),
(23543,20,15,-4137.88,-12476.799,70.99658,0,0,0,''),
(23543,20,16,-4124.742,-12497.759,72.07992,0,0,0,''),
(23543,20,17,-4125.0547,-12520,68.9966,0,0,0,''),
(23543,20,18,-4138.4673,-12541.72,67.66326,0,0,0,''),
(23543,20,19,-4165.264,-12554.55,67.88547,0,0,0,''),
(23543,20,20,-4190.2715,-12543.903,63.496586,0,0,0,''),
(23543,20,21,-4199.8555,-12533.329,60.16325,0,0,0,''),
(23543,20,22,-4199.8555,-12533.329,60.16325,0,0,0,''),
-- Azure Watch repeating flying path
(23543,21,1,-4205.4155,-12517.341,63.32532,0,0,0,''),
(23543,21,2,-4211.7847,-12499.882,63.325325,0,0,0,''),
(23543,21,3,-4206.111,-12480.489,67.492004,0,0,0,''),
(23543,21,4,-4187.5317,-12468.7,67.519775,0,0,0,''),
(23543,21,5,-4164.777,-12468.944,64.90866,0,0,0,''),
(23543,21,6,-4147.833,-12484.208,69.79755,0,0,0,''),
(23543,21,7,-4142.988,-12505.462,64.90866,0,0,0,''),
(23543,21,8,-4148.7715,-12519.934,63.325325,0,0,0,''),
(23543,21,9,-4164.565,-12528.206,63.325325,0,0,0,''),
(23543,21,10,-4183.68,-12531.513,63.54755,0,0,0,''),
(23543,21,11,-4201.428,-12522.581,63.325325,0,0,0,''),
-- Azure Watch bombing run
-- (23543,22,1,-4066.6611,-12500.047,76.10644,0,0,0,''),
(23543,22,1,-4083.5808,-12499.978,69.16193,0,0,0,''),
(23543,22,2,-4099.932,-12500.101,63.6064,0,0,0,''),
(23543,22,3,-4117.2354,-12500.129,62.80083,0,0,0,''),
(23543,22,4,-4133.3706,-12500.067,59.551937,0,0,0,''),
(23543,22,5,-4151.5356,-12500.165,59.0797,0,0,0,''),
(23543,22,6,-4166.6753,-12500.165,57.35749,0,0,0,''),
(23543,22,7,-4186.965,-12500.057,55.857487,0,0,0,''),
(23543,22,8,-4205.61,-12500.453,55.690804,0,0,0,''),
(23543,22,9,-4227.05,-12504.002,60.385284,0,0,0,''),
(23543,22,10,-4252.2354,-12512.66,61.246384,0,0,0,''),
(23543,22,11,-4266.674,-12533.182,62.078472,0,0,0,''),
/*(23543,22,13,-4299.0884,-12566.442,62.078472,0,0,0,''),
(23543,22,14,-4309.7334,-12597.361,62.078472,0,0,0,''),
(23543,22,15,-4293.068,-12627.314,69.27293,0,0,0,''),
(23543,22,16,-4263.1597,-12638.772,74.99516,0,0,0,''),
(23543,22,17,-4233.062,-12633.638,78.02293,0,0,0,''),
(23543,22,18,-4206.3594,-12613.681,77.63405,0,0,0,''),
(23543,22,19,-4199.997,-12583.407,68.078476,0,0,0,''),
(23543,22,20,-4200.1157,-12550.277,63.578465,0,0,0,''),*/
-- Azure Watch landing to start combat
(23543,23,1,-4100.0547,-12533.308,84.13409,0,0,0,''),
(23543,23,2,-4133.5264,-12517.641,75.68961,0,0,0,''),
(23543,23,3,-4157.6353,-12505.521,54.52299,0,0,0,''),
(23543,23,4,-4184.1816,-12484.275,46.52298,0,0,0,''),
-- path 24 reserved for Azure Watch leaving/failed

-- Kharanos fly-in
(23543,25,1,-5528.38,-589.786,433.16736,0,0,0,''),
(23543,25,2,-5528.69,-584.8607,433.2996,0,0,0,''),
(23543,25,3,-5523.0283,-570.58496,428.1887,0,0,0,''),
(23543,25,4,-5515.447,-528.35785,420.91122,0,0,0,''),
(23543,25,5,-5523.319,-500.31534,415.32803,0,0,0,''),
(23543,25,6,-5549.734,-481.21658,408.82794,0,0,0,''),
(23543,25,7,-5583.104,-481.4737,403.41144,0,0,0,''),
(23543,25,8,-5610.4253,-484.76624,407.1335,0,0,0,''),
(23543,25,9,-5633.8867,-506.7571,417.16122,0,0,0,''),
(23543,25,10,-5628.317,-535.2088,423.439,0,0,0,''),
(23543,25,11,-5611.467,-546.6129,436.29614,0,0,0,''),
(23543,25,12,-5589.7397,-546.1823,441.26837,0,0,0,''),
(23543,25,13,-5579.7466,-531.8438,438.0741,0,0,0,''),
(23543,25,14,-5588.452,-515.61957,433.65775,0,0,0,''),
(23543,25,15,-5606.755,-484.94135,420.71298,0,0,0,''),
(23543,25,16,-5603.68,-447.8284,427.8796,0,0,0,''),
(23543,25,17,-5589.1665,-438.79294,433.0567,0,0,0,''),
(23543,25,18,-5572.578,-448.159,433.3901,0,0,0,''),
(23543,25,19,-5570.1475,-459.22964,426.19547,0,0,0,''),
(23543,25,20,-5572.9087,-491.0464,429.1399,0,0,0,''),
(23543,25,21,-5581.447,-533.42694,442.529,0,0,0,''),
-- Kharanos repeating flying path
(23543,26,1,-5592.6997,-530.1878,443.79514,0,0,0,''),
(23543,26,2,-5604.026,-523.9838,440.96194,0,0,0,''),
(23543,26,3,-5615.2407,-507.67667,431.65643,0,0,0,''),
(23543,26,4,-5615.2017,-490.19092,427.37872,0,0,0,''),
(23543,26,5,-5609.4165,-468.88455,428.18433,0,0,0,''),
(23543,26,6,-5597.6304,-442.83243,437.18436,0,0,0,''),
(23543,26,7,-5577.401,-429.94644,444.62878,0,0,0,''),
(23543,26,8,-5548.4375,-430.3601,446.8788,0,0,0,''),
(23543,26,9,-5537.8687,-452.56088,448.01764,0,0,0,''),
(23543,26,10,-5557.9844,-491.91977,449.76764,0,0,0,''),
(23543,26,11,-5576.4897,-521.50275,449.76764,0,0,0,''),
(23543,26,12,-5582.849,-528.2464,445.85095,0,0,0,''),
-- Kharanos bombing run
(23543,27,1,-5638.1074,-484.21545,400.71936,0,0,0,''),
(23543,27,2,-5626.773,-483.98166,401.1465,0,0,0,''),
(23543,27,3,-5620.392,-482.93088,403.9522,0,0,0,''),
(23543,27,4,-5610.6895,-481.20172,403.9522,0,0,0,''),
(23543,27,5,-5596.0615,-482.6621,403.9522,0,0,0,''),
(23543,27,6,-5584.296,-483.10443,403.9522,0,0,0,''),
(23543,27,7,-5572.6743,-481.69672,403.9522,0,0,0,''),
(23543,27,8,-5562.312,-480.7193,403.9522,0,0,0,''),
(23543,27,9,-5566.436,-480.46457,411.18356,0,0,0,''),
(23543,27,10,-5555,-488.39795,420.26694,0,0,0,''),
(23543,27,11,-5558.6675,-500.202,431.7113,0,0,0,''),
(23543,27,12,-5567.6016,-505.57596,436.40573,0,0,0,''),
-- Kharanos landing to start combat
(23543,28,1,-5568.242,-515.7406,436.8683,0,0,0,''),
(23543,28,2,-5562.723,-502.2239,430.61838,0,0,0,''),
(23543,28,3,-5566.9165,-491.31314,425.06305,0,0,0,''),
(23543,28,4,-5572.458,-485.87982,413.61856,0,0,0,''),
(23543,28,5,-5582.303,-484.09366,404.25732,0,0,0,''),
(23543,28,6,-5593.9565,-482.44977,401.00742,0,0,0,''),
(23543,28,7,-5599.9272,-482.99463,398.39624,0,0,0,''),
(23543,28,8,-5605.137,-482.61105,397.47955,0,0,0,''),
-- path 29 reserved for Kharanos leaving/failed

-- Escape
-- sniffed
(23790,0,1,-145.6961,1334.897,48.17388,100,0,0,''),
(23790,0,2,-142.8265,1336.211,48.17387,100,8000,0,''),
(23790,0,3,-139.6775,1335.789,48.17387,100,0,0,''),
(23790,0,4,-136.364,1333.894,48.17387,100,17000,0,''),
(23790,0,5,-138.0992,1332.832,48.17387,100,0,0,''),
(23790,0,6,-139.3139,1331.377,48.17387,100,60000,0,''),
-- guessed
(23790,0,7,-135.124146,1339.056763,48.174061,100,0,0,''),
(23790,0,8,-130.729034,1339.119629,48.173988,100,0,0,''),
(23790,0,9,-112.540375,1346.102295,40.825539,100,0,0,''),
(23790,0,10,-80.070129,1369.448608,40.765926,100,0,0,''),
(23790,0,11,-80.220963,1378.210205,40.753429,100,0,0,''),
-- sniffed
(23790,0,12,-80.00238,1394.841,27.12944,100,0,0,''),
(23790,0,13,-70.83681,1409.247,27.23804,100,0,0,''),
(23790,0,14,-69.8846,1410.743,27.48376,100,0,0,''),
(23790,0,15,-67.50447,1414.484,27.24984,100,0,0,''),
(23790,0,16,-66.64658,1415.833,27.30231,100,0,0,''),
(23790,0,17,-65.23666,1418.048,27.28767,100,0,0,''),
(23790,0,18,-49.86122,1419.456,26.73495,100,0,0,''),
(23790,0,19,-24.96838,1419.364,15.42664,100,0,0,''),
(23790,0,20,-21.82492,1419.352,14.49998,100,0,0,''),
(23790,0,21,-19.67221,1419.344,13.06147,100,0,0,''),
(23790,0,22,-16.66602,1419.791,12.70261,100,0,0,''),
(23790,0,23,-11.11133,1422.047,12.07761,100,0,0,''),
(23790,0,24,-1.389323,1425.999,11.82761,100,0,0,''),
(23790,0,25,4.324544,1453.16,9.195978,100,0,0,''),
-- guessed
(23790,0,26,18.132174,1462.589844,3.926991,100,0,0,''),
(23790,0,27,39.477196,1450.521118,0.008979,100,0,0,''),
(23790,0,28,45.961079,1436.188843,0.350973,100,0,0,''),
(23790,0,29,56.151379,1419.242798,0.870952,100,0,0,''),
(23790,0,30,82.673813,1420.432129,0.768901,100,0,0,''),
(23790,0,31,86.237938,1446.306763,4.588818,100,0,0,''),
-- sniffed
(23790,0,32,86.40972,1465.93,18.13711,100,0,0,''),
(23790,0,33,86.42712,1466.93,18.13711,100,0,0,''),
(23790,0,34,86.47798,1469.854,20.05609,100,0,0,''),
(23790,0,35,86.49686,1470.938,20.05659,100,0,0,''),
(23790,0,36,86.52221,1473.874,20.07913,100,0,0,''),
(23790,0,37,86.54096,1476.046,20.07852,100,0,0,''),
(23790,0,38,86.55298,1477.437,20.07859,100,0,0,''),
(23790,0,39,86.5663,1478.979,20.07858,100,0,0,''),
(23790,0,40,86.59805,1482.655,20.07867,100,0,0,''),
(23790,0,41,86.64377,1487.949,20.07862,100,0,0,''),
(23790,0,42,86.65562,1489.32,20.07856,100,0,0,''),
(23790,0,43,86.66718,1490.659,20.07886,100,0,0,''),
(23790,0,44,86.68359,1492.56,20.28248,100,0,0,''),
(23790,0,45,86.73339,1519.45,21.23376,100,0,0,''),
-- guessed
(23790,0,46,115.147484,1532.465454,21.233761,100,0,0,''),
(23790,0,47,114.396042,1568.857422,43.523689,100,0,0,''),
(23790,0,48,120.363640,1620.080933,43.428474,100,0,0,''),
(23790,0,49,109.219406,1657.896240,42.021641,100,0,0,''),
(23790,0,50,86.464722,1674.980103,42.021641,100,0,0,''),
(23790,0,51,79.154434,1708.618286,42.021641,100,0,0,''),
(23790,0,52,97.701767,1726.481079,42.021656,100,0,0,''),
(23790,0,53,106.0898,1726.898,44.25594,100,12000,0,''),
(23790,0,54,117.4575,1726.867,42.02155,100,0,0,''),
(23790,0,55,137.6877,1719.041,42.02158,1.653,5000,0,''),

-- Post-Zul'jin
-- sniffed
(23790,1,1,129.8052,807.7782,33.37591,100,0,0,''),
(23790,1,2,128.9954,807.1915,33.37591,100,0,0,''),
(23790,1,3,122.3719,802.3923,33.37591,100,0,0,''),
(23790,1,4,121.3112,799.9839,33.37591,100,0,0,''),
(23790,1,5,119.7329,796.4,33.38746,100,0,0,''),
(23790,1,6,118.9309,794.579,33.3964,100,0,0,''),
(23790,1,7,117.9491,792.3499,33.40257,100,0,0,''),
(23790,1,8,118.0568,788.0145,33.4185,100,0,0,''),
(23790,1,9,118.4408,772.561,33.47497,100,0,0,''),
(23790,1,10,115.1441,752.7275,34.33925,100,0,0,''),
(23790,1,11,116.8422,738.0901,45.11,100,0,0,''),
(23790,1,12,105.3121,719.1982,45.11137,100,0,0,''),
(23790,1,13,107.0615,717.4769,45.11137,100,8000,0,''),
(23790,1,14,100.1625,707.4741,45.11137,100,0,0,''),
(23790,1,15,96.8226,707.6073,45.11137,100,5000,0,''),

-- Escape
-- sniffed
(23999,0,1,297.9303,1465.843,81.57809,100,7000,0,''),
(23999,0,2,302.2797,1462.846,81.57086,100,0,0,''),
(23999,0,3,306.3771,1463.949,81.58128,100,6000,0,''),
(23999,0,4,304.3857,1465.72,81.58634,0.994838,6000,0,''),
(23999,0,5,298.7304,1464.985,81.57541,100,60000,0,''),
-- guessed
(23999,0,6,305.810455,1456.020264,81.506264,100,0,0,''),
(23999,0,7,318.924622,1443.872070,74.343231,100,0,0,''),
(23999,0,8,335.608856,1422.748779,74.192307,100,0,0,''),
(23999,0,9,341.013275,1404.486328,74.441917,100,0,0,''),
(23999,0,10,335.152588,1395.553467,74.619873,100,0,0,''),
(23999,0,11,314.284271,1388.893066,57.596497,100,0,0,''),
(23999,0,12,299.649597,1384.947144,57.865318,100,0,0,''),
(23999,0,13,285.159729,1380.798828,49.321720,100,0,0,''),
(23999,0,14,246.258118,1375.042358,49.321400,100,0,0,''),
(23999,0,15,229.536514,1383.580078,44.411644,100,0,0,''),
(23999,0,16,227.194946,1440.485352,26.142105,100,0,0,''),
(23999,0,17,225.105408,1461.739990,25.916895,100,0,0,''),
(23999,0,18,208.522751,1471.140137,25.905643,100,0,0,''),
(23999,0,19,197.647491,1460.624756,20.287682,100,0,0,''),
(23999,0,20,192.763382,1426.174805,15.769067,100,0,0,''),
(23999,0,21,183.723358,1408.998413,15.064493,100,0,0,''),
-- sniffed
(23999,0,22,165.6035,1405.484,6.324402,100,0,0,''),
(23999,0,23,164.5332,1407.174,5.699402,100,0,0,''),
(23999,0,24,163.4629,1408.863,5.074402,100,0,0,''),
(23999,0,25,162.3926,1410.553,4.449402,100,0,0,''),
(23999,0,26,160.7871,1413.087,3.824402,100,0,0,''),
(23999,0,27,153.7922,1424.13,3.324402,100,0,0,''),
(23999,0,28,154.049,1444.969,3.551069,100,0,0,''),
(23999,0,29,154.2901,1471.561,20.06003,100,0,0,''),
-- guessed
(23999,0,30,154.191864,1492.069580,20.207298,100,0,0,''),
(23999,0,31,156.734192,1516.477295,21.234205,100,0,0,''),
(23999,0,32,123.011536,1532.006104,21.234205,100,0,0,''),
-- sniffed
(23999,0,33,119.9618,1564.408,41.02303,100,0,0,''),
(23999,0,34,119.8572,1568.617,43.39333,100,0,0,''),
(23999,0,35,119.5184,1583.925,43.4181,100,0,0,''),
(23999,0,36,119.266,1595.325,43.4474,100,0,0,''),
(23999,0,37,119.1767,1599.363,43.41511,100,0,0,''),
(23999,0,38,121.4332,1646.606,42.02159,100,0,0,''),
-- guessed
(23999,0,39,156.613251,1669.096436,42.021591,100,0,0,''),
(23999,0,40,161.727570,1716.291260,42.021591,100,0,0,''),
(23999,0,41,145.963959,1721.568848,42.021591,100,0,0,''),
(23999,0,42,111.525749,1714.873169,42.021591,1.42635,5000,0,''),

-- Post-Zul'jin
-- sniffed
(23999,1,1,130.8155,809.079,33.37591,100,0,0,''),
(23999,1,2,129.9751,808.537,33.37591,100,0,0,''),
(23999,1,3,123.364,804.2735,33.37591,100,0,0,''),
(23999,1,4,120.6925,802.5507,33.37591,100,0,0,''),
(23999,1,5,120.6637,800.8959,33.37591,100,0,0,''),
(23999,1,6,120.645,799.8185,33.37591,100,0,0,''),
(23999,1,7,120.3445,782.5253,33.43821,100,0,0,''),
(23999,1,8,120.1743,772.7297,33.47401,100,0,0,''),
(23999,1,9,120.1556,771.6559,33.47368,100,0,0,''),
(23999,1,10,113.2892,752.9368,33.76631,100,0,0,''),
(23999,1,11,113.6587,738.6876,45.28601,100,0,0,''),
(23999,1,12,113.3314,719.3889,45.11137,100,16000,0,''),
(23999,1,13,106.0395,720.9223,45.11136,100,0,0,''),
(23999,1,14,101.7941,706.7435,45.11137,100,0,0,''),
(23999,1,15,100.6341,697.988,45.11136,100,0,0,''),
-- guessed
(23999,1,16,100.851,695.5559,45.11136,4.1273,6000,0,'spawn Harkor''s Brew Keg'),
(23999,1,17,109.048912,687.187317,45.111408,100,0,0,''),
(23999,1,18,112.137337,677.090271,52.330826,100,0,0,''),
(23999,1,19,114.103127,671.329712,51.719406,1.55117,5000,0,''),

-- Escape
-- sniffed
(24001, 0, 1, 383.5332, 1086.308, 5.974173, 100, 1000, 0, ''),
(24001, 0, 2, 383.3186, 1088.672, 5.989234, 100, 0, 0, ''),
(24001, 0, 3, 383.3262, 1090.004, 6.320981, 100, 0, 0, ''),
(24001, 0, 4, 380.3339, 1098.337, 6.152729, 100, 0, 0, ''),
(24001, 0, 5, 398.1089, 1126.298, 6.303194, 100, 0, 0, ''),
(24001, 0, 6, 392.3712, 1146.572, 6.222425, 100, 0, 0, ''),
(24001, 0, 7, 355.8842, 1129.587, 6.525888, 100, 0, 0, ''),
(24001, 0, 8, 344.3732, 1100.115, 6.457113, 100, 12000, 0, ''),
(24001, 0, 9, 364.8193, 1103.98, 6.138184, 100, 0, 0, ''),
(24001, 0, 10, 389.1712, 1086.583, 5.902591, 100, 0, 0, ''),
(24001, 0, 11, 406.2733, 1113.202, 6.390222, 100, 0, 0, ''),
(24001, 0, 12, 389.5124, 1148.138, 6.220311, 100, 0, 0, ''),
(24001, 0, 13, 352.5295, 1118.125, 6.44824, 100, 0, 0, ''),
(24001, 0, 14, 338.2169, 1126.372, 6.455091, 100, 12000, 0, ''),
(24001, 0, 15, 345.4488, 1088.852, 7.004283, 100, 0, 0, ''),
(24001, 0, 16, 373.4771, 1097.548, 6.308249, 100, 0, 0, ''),
(24001, 0, 17, 399.9229, 1132.639, 6.309431, 100, 0, 0, ''),
(24001, 0, 18, 392.4269, 1143.645, 6.147355, 100, 0, 0, ''),
(24001, 0, 19, 347.0435, 1142.894, 6.186779, 100, 0, 0, ''),
(24001, 0, 20, 338.6937, 1126, 6.452535, 100, 0, 0, ''),
(24001, 0, 21, 369.6844, 1117.15, 5.95745, 100, 0, 0, ''),
(24001, 0, 22, 401.7724, 1098.962, 6.580225, 100, 12000, 0, ''),
(24001, 0, 23, 334.3144, 1117.262, 6.0273, 100, 0, 0, ''),
(24001, 0, 24, 313.4415, 1123.747, 9.704546, 100, 60000, 0, ''),
-- guessed
(24001, 0, 25, 309.743408, 1117.530151, 9.717999, 100, 0, 0, ''),
(24001, 0, 26, 301.905029, 1117.733276, 10.289268, 100, 0, 0, ''),
(24001, 0, 27, 283.300385, 1117.645752, 0.000099, 100, 0, 0, ''),
(24001, 0, 28, 261.491364, 1118.421631, 0.000099, 100, 0, 0, ''),
-- sniffed
(24001, 0, 29, 204.5145, 1115.355, 0.125, 100, 0, 0, ''),
(24001, 0, 30, 187.0511, 1121.262, 0.125, 100, 0, 0, ''),
(24001, 0, 31, 177.7114, 1135.33, 0.125, 100, 0, 0, ''),
(24001, 0, 32, 161.3966, 1146.505, 0.07049704, 100, 0, 0, ''),
-- guessed
(24001, 0, 33, 137.688263, 1155.374023, 0.981615, 100, 0, 0, ''),
(24001, 0, 34, 130.043289, 1159.478271, -2.338670, 100, 0, 0, ''),
(24001, 0, 35, 125.371216, 1174.883911, -6.650873, 100, 0, 0, ''),
-- sniffed
(24001, 0, 36, 121.5685, 1178.793, -12.62515, 100, 0, 0, ''),
(24001, 0, 37, 121.4629, 1179.581, -13.32066, 100, 0, 0, ''),
(24001, 0, 38, 120.666, 1185.528, -14.07066, 100, 0, 0, ''),
(24001, 0, 39, 120.002, 1190.484, -14.82066, 100, 0, 0, ''),
(24001, 0, 40, 119.4707, 1194.449, -15.44566, 100, 0, 0, ''),
(24001, 0, 41, 119.0723, 1197.423, -16.07066, 100, 0, 0, ''),
(24001, 0, 42, 118.7324, 1200.402, -16.72363, 100, 0, 0, ''),
(24001, 0, 43, 118.5059, 1202.389, -17.34863, 100, 0, 0, ''),
(24001, 0, 44, 118.2793, 1204.375, -17.97363, 100, 0, 0, ''),
(24001, 0, 45, 118.0527, 1206.361, -19.34863, 100, 0, 0, ''),
(24001, 0, 46, 117.8262, 1208.348, -21.34863, 100, 0, 0, ''),
(24001, 0, 47, 117.2598, 1213.313, -20.72363, 100, 0, 0, ''),
(24001, 0, 48, 117.0332, 1215.3, -21.59863, 100, 0, 0, ''),
(24001, 0, 49, 116.8066, 1217.286, -22.22363, 100, 0, 0, ''),
(24001, 0, 50, 116.5801, 1219.272, -22.84863, 100, 0, 0, ''),
(24001, 0, 51, 116.2402, 1222.252, -23.59863, 100, 0, 0, ''),
(24001, 0, 52, 115.7871, 1226.225, -24.22363, 100, 0, 0, ''),
(24001, 0, 53, 115.2244, 1231.232, -24.47363, 100, 0, 0, ''),
(24001, 0, 54, 115.2154, 1236.795, -21.77824, 100, 0, 0, ''),
(24001, 0, 55, 115.0737, 1243.064, -21.81769, 100, 0, 0, ''),
(24001, 0, 56, 115.1374, 1246.035, -20.19992, 100, 0, 0, ''),
(24001, 0, 57, 115.8309, 1271.141, -15.35199, 100, 0, 0, ''),
(24001, 0, 58, 129.004, 1284.024, -15.35208, 100, 0, 0, ''),
(24001, 0, 59, 128.5221, 1296.326, -15.35208, 100, 0, 0, ''),
(24001, 0, 60, 117.06, 1307.122, -15.35201, 100, 0, 0, ''),
(24001, 0, 61, 115.6787, 1310.843, -15.35201, 100, 0, 0, ''),
(24001, 0, 62, 115.8802, 1330.122, -20.30514, 100, 0, 0, ''),
(24001, 0, 63, 115.7944, 1335.341, -21.63844, 100, 0, 0, ''),
(24001, 0, 64, 115.8271, 1341.045, -21.68019, 100, 0, 0, ''),
(24001, 0, 65, 115.0957, 1347.816, -24.65599, 100, 0, 0, ''),
(24001, 0, 66, 115.8129, 1362.714, -23.36664, 100, 0, 0, ''),
(24001, 0, 67, 116.1325, 1386.015, -15.87422, 100, 0, 0, ''),
(24001, 0, 68, 118.8566, 1405.228, -6.975243, 100, 0, 0, ''),
-- guessed
(24001, 0, 69, 145.574234, 1418.649780, 3.101707, 100, 0, 0, ''),
(24001, 0, 70, 153.649857, 1430.683716, 3.302236, 100, 0, 0, ''),
(24001, 0, 71, 154.572861, 1444.352539, 3.303317, 100, 0, 0, ''),
(24001, 0, 72, 154.179489, 1471.110962, 20.054930, 100, 0, 0, ''),
(24001, 0, 73, 153.942856, 1492.816040, 20.347717, 100, 0, 0, ''),
(24001, 0, 74, 157.635666, 1517.935791, 21.234461, 100, 0, 0, ''),
(24001, 0, 75, 131.859650, 1524.410645, 21.234461, 100, 0, 0, ''),
(24001, 0, 76, 125.264290, 1531.958740, 21.234461, 100, 0, 0, ''),
(24001, 0, 77, 124.113449, 1568.925781, 43.517063, 100, 0, 0, ''),
(24001, 0, 78, 125.377708, 1621.279175, 43.441097, 100, 0, 0, ''),
(24001, 0, 79, 129.068375, 1653.187012, 42.022205, 100, 0, 0, ''),
(24001, 0, 80, 150.881958, 1663.231445, 42.022205, 100, 0, 0, ''),
(24001, 0, 81, 162.436722, 1683.081787, 42.022205, 100, 0, 0, ''),
(24001, 0, 82, 162.504486, 1714.189209, 42.022205, 100, 0, 0, ''),
(24001, 0, 83, 147.798157, 1723.008423, 42.022205, 100, 0, 0, ''),
(24001, 0, 84, 136.507858, 1721.674805, 42.022205, 100, 0, 0, ''),
(24001, 0, 85, 130.966125, 1713.293701, 42.022205, 1.69111, 5000, 0, ''),
-- Post-Zul'jin
-- sniffed
(24001,1,1,137.0035,814.2776,33.37591,100,0,0,''),
(24001,1,2,136.3934,813.4853,33.37591,100,0,0,''),
(24001,1,3,131.3571,806.944,33.37591,100,0,0,''),
(24001,1,4,122.2147,802.8359,33.37591,100,0,0,''),
(24001,1,5,120.2096,801.9349,33.37591,100,0,0,''),
(24001,1,6,120.1491,800.2522,33.37591,100,0,0,''),
(24001,1,7,119.8858,792.9299,33.40015,100,0,0,''),
(24001,1,8,119.1559,772.6306,33.47458,100,0,0,''),
(24001,1,9,119.1119,771.4086,33.47368,100,0,0,''),
(24001,1,10,119.081,770.5477,33.44409,100,0,0,''),
(24001,1,11,126.0739,752.9862,34.05475,100,0,0,''),
(24001,1,12,125.4523,737.8333,45.1387,100,0,0,''),
(24001,1,13,136.2864,715.3524,45.11137,100,0,0,''),
(24001,1,14,134.3398,714.4622,45.11137,100,14000,0,''),
(24001,1,15,134.4575,686.9156,45.11136,100,0,0,''),
(24001,1,16,130.1011,676.2486,52.19216,100,0,0,''),
(24001,1,17,125.7827,655.5018,51.67329,100,0,0,''),
(24001,1,18,113.7838,655.6854,51.68291,100,0,0,''),
(24001,1,19,109.9202,674.5815,51.95691,100,0,0,''),
(24001,1,20,105.8745,702.0614,45.11136,100,0,0,''),
(24001,1,21,83.96203,697.8445,45.11135,100,0,0,''),
(24001,1,22,83.43729,714.374,45.11136,100,0,0,''),
(24001,1,23,105.979,711.717,45.11137,100,0,0,''),
(24001,1,24,101.5918,729.3037,45.11136,100,0,0,''),
(24001,1,25,138.5485,729.1754,45.11137,100,0,0,''),
(24001,1,26,132.9379,712.7618,45.11137,100,0,0,''),
(24001,1,27,156.897,714.4436,45.11136,100,0,0,''),
(24001,1,28,157.6463,697.4852,45.11136,100,0,0,''),
(24001,1,29,132.5298,700.6038,45.11136,100,0,0,''),
(24001,1,30,106.818,700.1823,45.11136,0.856,5000,0,''), -- orientation guessed

(24024,0,1,-73.60417,1161.902,5.189967,100,0,0,''),
(24024,0,2,-75.65417,1157.752,5.364081,100,7000,0,''),
(24024,0,3,-76.48643,1156.667,5.299397,100,0,0,''),
(24024,0,4,-89.46484,1142.276,5.594042,100,0,0,''),
(24024,0,5,-84.86057,1127.99,5.594042,100,0,0,''),
(24024,0,6,-81.4426,1125.975,5.593996,100,10000,0,''),
(24024,0,7,-86.68555,1133.053,5.594041,100,0,0,''),
(24024,0,8,-79.09592,1138.717,5.458687,100,8000,0,''),
(24024,0,9,-82.01552,1138.562,5.594041,100,0,0,''),
(24024,0,10,-84.77876,1135.255,5.594041,100,0,0,''),
(24024,0,11,-84.96288,1132.281,5.594057,100,0,0,''),
(24024,0,12,-82.6097,1128.821,5.594056,100,60000,0,''),
(24175,0,1,216.3581,1467.675,25.9713,100,0,0,''),
(24175,0,2,217.2123,1467.155,25.9713,100,0,0,''),
(24175,0,3,226.0859,1461.754,25.93943,100,0,0,''),
(24175,0,4,228.0156,1435.83,26.68943,100,0,0,''),
(24175,0,5,228.1763,1433.847,27.18943,100,0,0,''),
(24175,0,6,227.731,1412.553,34.4392,100,0,0,''),
(24175,0,7,228.1711,1388.288,42.68928,100,0,0,''),
(24175,0,8,232.5426,1374.381,47.43928,100,0,0,''),
(24175,0,9,263.1866,1376.492,49.32307,100,0,0,''),
(24175,0,10,265.7423,1376.84,49.34044,100,0,0,''),
(24175,0,11,298.9646,1385.205,57.77875,100,0,0,''),
(24358,0,1,121.194,1645.62,42.021,100,0,0,''),
(24358,0,2,132.051,1642.18,42.021,100,4000,0,'SAY_AT_GONG'),
(24358,0,3,132.051,1642.18,42.021,100,1000,0,'start pounding gong'),
(24358,0,4,127.5514,1641.869,42.35881,100,0,0,''),
(24358,0,5,123.5514,1640.119,42.35881,100,0,0,''),
(24358,0,6,121.947,1639.015,42.19603,100,0,0,''),
(24358,0,7,120.8522,1637.931,42.37172,100,0,0,'SAY_OPEN_ENTRANCE'),
(24358,0,8,121.071,1622.997,43.93089,100,0,0,''),
(24358,0,9,120.7898,1609.063,43.49005,100,9000,0,'SAY_OPEN_ENTRANCE_2'),
(24358,0,10,120.7898,1609.063,43.49005,100,2000,0,'door opens, cast stealth'),
(24358,0,11,120.9933,1605.888,43.97018,100,0,0,''),
(24358,0,12,120.6967,1603.713,43.4503,100,0,0,'guardian attacker moves 1'),
(24358,0,13,120.9157,1595.822,43.96694,100,0,0,'guardian attacker moves 2'),
(24358,0,14,120.594,1587.591,43.43018,100,5000,0,'dies'),
(25208,0,1,4013.51,6390.33,29.97,0,15000,0,'Lurgglbr - after escaped from cage'),
(25208,0,2,4023.06,6379.43,29.21,0,0,0,''),
(25208,0,3,4033.61,6370.94,28.43,0,0,0,''),
(25208,0,4,4052.03,6367.42,27.37,0,0,0,''),
(25208,0,5,4061.13,6353.36,25.45,0,0,0,''),
(25208,0,6,4064.28,6330.76,25.31,0,0,0,''),
(25208,0,7,4057.94,6307.85,24.9,0,0,0,''),
(25208,0,8,4057.4,6290.12,24.43,0,0,0,''),
(25208,0,9,4065.63,6277.64,23.9,0,0,0,''),
(25208,0,10,4080.71,6280.77,26.92,0,0,0,''),
(25208,0,11,4098.9,6279,25.95,0,0,0,''),
(25208,0,12,4118,6277.81,25.72,0,0,0,''),
(25208,0,13,4129.47,6281.65,28.86,0,0,0,''),
(25208,0,14,4143.86,6282.57,29.18,0,4000,0,'Lurgglbr - outside cave'),
(25208,0,15,4159.54,6280.08,30.52,0,0,0,''),
(25208,0,16,4171.95,6291.5,22.25,0,0,0,''),
(25208,0,17,4184.86,6293.45,16.57,0,0,0,''),
(25208,0,18,4194.14,6301.28,13.31,0,0,0,''),
(25208,0,19,4210.34,6285.81,9.5,0,0,0,''),
(25208,0,20,4220.05,6272.75,7.77,0,0,0,''),
(25208,0,21,4242.45,6272.24,1.75,0,0,0,''),
(25208,0,22,4257.79,6252.91,-0.05,0,0,0,''),
(25208,0,23,4256.81,6230.74,-0.09,0,0,0,''),
(25208,0,24,4241.09,6217.87,-0.14,0,0,0,''),
(25208,0,25,4254.66,6205.16,-0.17,0,0,0,''),
(25208,0,26,4270.07,6188.42,0.059,0,15000,0,'Lurgglbr - final point'),
(25335,0,1,4138.37,5785.34,60.9668,0,14000,0,'SAY_QUEST_INTRO'),
(25335,0,2,4138.37,5785.34,60.9668,0,2000,0,'SAY_QUEST_START'),
(25335,0,3,4121.4,5791.31,62.7287,0,0,0,''),
(25335,0,4,4101.44,5799.44,67.9234,0,0,0,''),
(25335,0,5,4083.93,5805.44,71.3223,0,0,0,''),
(25335,0,6,4068.87,5807.64,73.8006,0,0,0,''),
(25335,0,7,4052.77,5802.65,75.0299,0,0,0,''),
(25335,0,8,4038.37,5795.23,75.4015,0,0,0,''),
(25335,0,9,4025.04,5789.23,75.1928,0,0,0,''),
(25335,0,10,4006.38,5787.3,73.1468,0,0,0,''),
(25335,0,11,3984.42,5778.06,73.1662,0,3000,0,'SAY_AMBUSH'),
(25335,0,12,3984.42,5778.06,73.1662,0,5000,0,'summon mobs'),
(25335,0,13,3984.42,5778.06,73.1662,0,10000,0,'SAY_AMBUSH_COMPLETE'),
(25335,0,14,3952.68,5758.44,70.4093,0,0,0,''),
(25335,0,15,3939.55,5758.72,69.3329,0,0,0,''),
(25335,0,16,3919.01,5753.34,69.2061,0,0,0,''),
(25335,0,17,3894.65,5745.7,70.2652,0,0,0,''),
(25335,0,18,3888.48,5734.52,69.0002,0,0,0,''),
(25335,0,19,3883.36,5725.31,67.5505,0,5000,0,'SAY_FINAL_BATTLE'),
(25335,0,20,3883.36,5725.31,67.5505,0,5000,0,'summon boss'),
(25335,0,21,3883.36,5725.31,67.5505,0,10000,0,'quest complete'),
(25335,0,22,3883.36,5725.31,67.5505,0,10000,0,'SAY_QUEST_END'),
(25504,0,1,2886.78,6732.96,32.9695,0,5000,0,'SAY_MOOTOO_Y_START'),
(25504,0,2,2882.26,6734.51,32.8864,0,0,0,''),
(25504,0,3,2877.37,6731.59,32.8721,0,0,0,''),
(25504,0,4,2874.18,6725.85,30.3087,0,0,0,'SAY_1_MOOTOO_Y'),
(25504,0,5,2866.36,6726.04,26.7277,0,0,0,''),
(25504,0,6,2863.97,6731.91,23.8372,0,0,0,''),
(25504,0,7,2869.29,6736.92,20.5227,0,0,0,''),
(25504,0,8,2874.16,6731.88,18.5042,0,0,0,''),
(25504,0,9,2887.05,6736.39,13.997,0,5000,0,'SAY_2_MOOTOO_Y'),
(25504,0,10,2887.05,6736.39,13.997,0,2000,0,'EMOTE_ONESHOT_POINT'),
(25504,0,11,2901.68,6741.25,14.0413,0,0,0,''),
(25504,0,12,2919.45,6746.28,13.7325,0,0,0,'SAY_3_MOOTOO_Y'),
(25504,0,13,2947.18,6738.71,12.7117,0,0,0,''),
(25504,0,14,2982.43,6748.59,10.2755,0,0,0,'SAY_4_MOOTOO_Y'),
(25504,0,15,2985.05,6776.05,8.33081,0,0,0,'SAY_5_MOOTOO_Y'),
(25504,0,16,2978.72,6801.48,5.83056,0,0,0,'SAY_6_MOOTOO_Y'),
(25504,0,17,2957.81,6818.86,4.7594,0,0,0,''),
(25504,0,18,2917.03,6820.55,5.87954,0,0,0,'SAY_4_MOOTOO_Y'),
(25504,0,19,2890.04,6825.68,4.11676,0,3000,0,'SAY_7_MOOTOO_Y'),
(25504,0,20,2850.31,6812.35,2.09411,0,0,0,'SAY_8_MOOTOO_Y'),
(25504,0,21,2813.28,6793.82,4.44574,0,0,0,''),
(25504,0,22,2807.22,6772.41,5.80334,0,2000,0,'SAY_CREDIT_MOOTOO_Y'),
(25504,0,23,2807.38,6765,6.30617,0,0,0,'SAY_1_ELDER_MOOTOO'),
(25504,0,24,2807.49,6748.29,8.25933,0,3000,0,'MOOTOO_Y_SAY_9'),
(25504,0,25,2807.49,6748.29,8.25933,0,5000,0,'SAY_2_ELDER_MOOTOO'),
(25589,0,1,4414.22,5367.3,-15.494,0,13000,0,'SAY_BONKER_START'),
(25589,0,2,4414.22,5367.3,-15.494,0,0,0,'SAY_BONKER_GO'),
(25589,0,3,4429.03,5366.66,-17.198,0,0,0,''),
(25589,0,4,4454.77,5371.56,-16.385,0,10000,0,'SAY_BONKER_LEFT'),
(25589,0,5,4467.89,5372.42,-15.236,0,0,0,''),
(25589,0,6,4481.39,5378.62,-14.997,0,0,0,''),
(25589,0,7,4484.98,5392.24,-15.31,0,0,0,''),
(25589,0,8,4473.11,5414.9,-15.272,0,0,0,''),
(25589,0,9,4461.07,5427.64,-16.163,0,0,0,''),
(25589,0,10,4441.34,5435.53,-15.367,0,0,0,''),
(25589,0,11,4427.12,5436.6,-15.149,0,0,0,''),
(25589,0,12,4408.94,5428.32,-14.629,0,0,0,''),
(25589,0,13,4396.61,5415.88,-13.552,0,0,0,''),
(25589,0,14,4392.92,5405.89,-10.506,0,0,0,''),
(25589,0,15,4390.49,5390.3,-5.628,0,0,0,''),
(25589,0,16,4393.43,5358.27,2.967,0,0,0,''),
(25589,0,17,4400.14,5345.6,4.656,0,0,0,''),
(25589,0,18,4412.08,5336.68,7.272,0,0,0,''),
(25589,0,19,4436.49,5335.23,12.415,0,0,0,''),
(25589,0,20,4454.6,5341.27,15.56,0,0,0,''),
(25589,0,21,4471.04,5352.31,18.686,0,0,0,''),
(25589,0,22,4478.23,5367.26,20.225,0,0,0,''),
(25589,0,23,4481.35,5387.54,24.537,0,0,0,''),
(25589,0,24,4483.07,5405.13,27.576,0,0,0,''),
(25589,0,25,4475.88,5414.83,29.965,0,0,0,''),
(25589,0,26,4466.6,5423.73,32.224,0,0,0,''),
(25589,0,27,4451.21,5431.03,36.189,0,0,0,''),
(25589,0,28,4428.06,5434.37,38.946,0,0,0,''),
(25589,0,29,4398.92,5443.86,44.214,0,0,0,''),
(25589,0,30,4386.82,5451.89,48.935,0,0,0,''),
(25589,0,31,4379.86,5457.21,51.371,0,0,0,''),
(25589,0,32,4372.71,5461.35,48.541,0,0,0,''),
(25589,0,33,4364.52,5465.8,48.661,0,10000,0,'SAY_BONKER_COMPLETE'),
(25589,0,34,4337.2,5472.95,46.035,0,0,0,''),
(26499,0,1,2366.18,1197.29,132.15,0,0,0,''),
(26499,0,2,2371.61,1199.01,134.727,0,0,0,''),
(26499,0,3,2376.16,1200.55,134.042,0,0,0,''),
(26499,0,4,2391.32,1203.15,134.125,0,10000,0,'SAY_ARRIVED'),
(26499,0,5,2391.32,1203.15,134.125,0,0,0,'SAY_GET_BEFORE_PLAGUE'),
(26499,0,6,2396.74,1205.99,134.125,0,0,0,'escort paused'),
(26499,0,7,2396.74,1205.99,134.125,0,8000,0,''),
(26499,0,8,2396.74,1205.99,134.125,0,5000,0,'SAY_MORE_THAN_SCOURGE'),
(26499,0,9,2412.03,1207.82,134.034,0,0,0,''),
(26499,0,10,2426.96,1212.36,134,0,0,0,''),
(26499,0,11,2438.59,1217.01,133.957,0,0,0,''),
(26499,0,12,2441.25,1215.51,133.951,0,0,0,''),
(26499,0,13,2446.16,1197.14,148.064,0,0,0,''),
(26499,0,14,2446.86,1193.56,148.076,0,0,0,'SAY_MORE_SORCERY'),
(26499,0,15,2443.58,1189.77,148.076,0,0,0,'escort paused'),
(26499,0,16,2443.58,1189.77,148.076,0,8000,0,''),
(26499,0,17,2443.58,1189.77,148.076,0,5000,0,'SAY_MOVE_ON'),
(26499,0,18,2430.99,1193.84,148.076,0,0,0,''),
(26499,0,19,2418.7,1195.07,148.076,0,0,0,''),
(26499,0,20,2410.82,1193.03,148.076,0,0,0,''),
(26499,0,21,2405.18,1177.3,148.076,0,0,0,''),
(26499,0,22,2409.68,1155.14,148.187,0,0,0,'SAY_WATCH_BACKS - escort paused'),
(26499,0,23,2409.68,1155.14,148.187,0,8000,0,''),
(26499,0,24,2409.68,1155.14,148.187,0,3000,0,'SAY_NOT_EASY'),
(26499,0,25,2413.03,1138.77,148.075,0,0,0,''),
(26499,0,26,2421.59,1122.54,148.125,0,0,0,''),
(26499,0,27,2425.38,1119.32,148.075,0,0,0,'SAY_PERSISTENT'),
(26499,0,28,2425.38,1119.32,148.075,0,8000,0,''),
(26499,0,29,2425.38,1119.32,148.075,0,0,0,'SAY_ELSE - escort paused'),
(26499,0,30,2447.38,1114.94,148.075,0,0,0,''),
(26499,0,31,2454.85,1117.05,150.007,0,0,0,''),
(26499,0,32,2459.91,1125.71,150.007,0,0,0,''),
(26499,0,33,2468.21,1124.43,150.027,0,5000,0,'SAY_TAKE_A_MOMENT'),
(26499,0,34,2468.21,1124.43,150.027,0,0,0,'SAY_PASSAGE'),
(26499,0,35,2482.7,1122.35,149.905,0,0,0,''),
(26499,0,36,2485.54,1111.68,149.907,0,0,0,''),
(26499,0,37,2487,1103.31,145.335,0,0,0,''),
(26499,0,38,2490.22,1100.45,144.86,0,0,0,''),
(26499,0,39,2496.68,1102.51,144.474,0,0,0,''),
(26499,0,40,2495.01,1115.54,143.825,0,0,0,''),
(26499,0,41,2493.21,1123.73,140.302,0,0,0,''),
(26499,0,42,2496.52,1128.8,140.01,0,0,0,''),
(26499,0,43,2500.96,1127.1,139.982,0,0,0,''),
(26499,0,44,2504.46,1120.4,139.976,0,0,0,''),
(26499,0,45,2506.48,1120.34,139.97,0,0,0,''),
(26499,0,46,2517.03,1122.5,132.064,0,0,0,''),
(26499,0,47,2523.49,1124.81,132.08,0,0,0,'encounter complete - despawn'),
(26499,0,48,2551.12,1135.61,129.797,0,0,0,''),
(26499,0,49,2562.69,1147.9,128.003,0,0,0,''),
(26499,0,50,2565.03,1168.82,127.007,0,0,0,''),
(26499,0,51,2562.41,1189.93,126.189,0,0,0,''),
(26499,0,52,2558.31,1212.63,125.739,0,0,0,''),
(26499,0,53,2551.08,1231.6,125.554,0,0,0,''),
(26499,0,54,2543.63,1250.39,126.103,0,0,0,''),
(26499,0,55,2534.27,1272.28,126.993,0,0,0,''),
(26499,0,56,2521.45,1290.46,130.194,0,0,0,''),
(26499,0,57,2517.06,1312.33,130.156,0,0,0,''),
(26499,0,58,2513.2,1324.15,131.843,0,20000,0,'SAY_REST'),
(26499,0,59,2513.2,1324.15,131.843,0,0,0,'SAY_REST_COMPLETE'),
(26499,0,60,2503.48,1347.35,132.952,0,0,0,''),
(26499,0,61,2491.94,1367.2,130.717,0,0,0,''),
(26499,0,62,2482.92,1386.12,130.029,0,0,0,''),
(26499,0,63,2471.58,1404.73,130.681,0,0,0,''),
(26499,0,64,2459.65,1418.8,130.662,0,0,0,''),
(26499,0,65,2440,1423.9,130.632,0,0,0,''),
(26499,0,66,2416.75,1419.93,130.669,0,0,0,''),
(26499,0,67,2401.42,1415.89,130.84,0,0,0,''),
(26499,0,68,2381.81,1410.02,128.147,0,0,0,''),
(26499,0,69,2367.66,1406.69,128.529,0,0,0,''),
(26499,0,70,2361.86,1405.02,128.714,0,0,0,'SAY_CRUSADER_SQUARE - escort paused'),
(26499,0,71,2341.93,1406.36,128.268,0,0,0,''),
(26499,0,72,2328.38,1413.14,127.687,0,0,0,''),
(26499,0,73,2319.29,1435.61,127.887,0,0,0,''),
(26499,0,74,2308.85,1460.5,127.84,0,0,0,''),
(26499,0,75,2301.28,1487.08,128.361,0,0,0,'SAY_FINISH_MALGANIS - escort paused'),
(26499,0,76,2301.28,1487.08,128.361,0,18000,0,'SAY_JOURNEY_BEGUN'),
(26499,0,77,2293.69,1506.81,128.737,0,18000,0,'SAY_HUNT_MALGANIS'),
(26499,0,78,2300.74,1487.23,128.362,0,0,0,''),
(26499,0,79,2308.58,1460.86,127.839,0,0,0,''),
(26499,0,80,2326.61,1420.56,127.78,0,0,0,''),
(26588,0,1,4322.89,-3693.61,263.91,0,4000,0,'SAY_ESCORT_START'),
(26588,0,2,4330.51,-3689.44,263.627,0,0,0,''),
(26588,0,3,4341.48,-3684.21,257.441,0,0,0,''),
(26588,0,4,4346.75,-3685.9,256.866,0,0,0,''),
(26588,0,5,4347.18,-3694.56,256.56,0,0,0,''),
(26588,0,6,4335.92,-3704.45,258.113,0,0,0,''),
(26588,0,7,4317.91,-3722.99,256.835,0,0,0,''),
(26588,0,8,4309.21,-3736.35,257.451,0,0,0,''),
(26588,0,9,4301.65,-3751.55,257.81,0,0,0,''),
(26588,0,10,4296.5,-3769.06,251.977,0,0,0,''),
(26588,0,11,4291.98,-3785.02,245.88,0,2000,0,'SAY_FIRST_WOLF'),
(26588,0,12,4291.98,-3785.02,245.88,0,0,0,'SAY_WOLF_ATTACK'),
(26588,0,13,4291.98,-3785.02,245.88,0,3000,0,''),
(26588,0,14,4299.54,-3807.02,237.238,0,0,0,''),
(26588,0,15,4308.17,-3835.07,226.317,0,0,0,''),
(26588,0,16,4312.53,-3847.57,222.333,0,0,0,''),
(26588,0,17,4317.51,-3861.73,214.915,0,0,0,''),
(26588,0,18,4325.01,-3882.17,208.888,0,0,0,''),
(26588,0,19,4332.63,-3893.47,203.584,0,0,0,''),
(26588,0,20,4338.52,-3899.45,199.843,0,0,0,''),
(26588,0,21,4344.69,-3911.86,197.886,0,0,0,''),
(26588,0,22,4349.63,-3922.68,195.293,0,0,0,''),
(26588,0,23,4351.97,-3934.68,191.418,0,0,0,'SAY_SECOND_WOLF'),
(26588,0,24,4351.97,-3934.68,191.418,0,3000,0,''),
(26588,0,25,4351.97,-3934.68,191.418,0,2000,0,'SAY_RESUME_ESCORT'),
(26588,0,26,4350.81,-3944.97,190.528,0,0,0,'SAY_ESCORT_COMPLETE'),
(26588,0,27,4347.95,-3958.88,193.36,0,0,0,''),
(26588,0,28,4345.96,-3988.08,187.32,0,0,0,''),
(26814,0,1,4905.26,-4758.71,27.316,0,2000,0,'open cage - SAY_ESCORT_START'),
(26814,0,2,4895.4,-4754.88,27.233,0,0,0,''),
(26814,0,3,4887.63,-4761.87,27.233,0,0,0,''),
(26814,0,4,4881.63,-4768.92,32.142,0,0,0,''),
(26814,0,5,4878.45,-4772.85,32.646,0,0,0,''),
(26814,0,6,4876.89,-4787.92,32.531,0,0,0,''),
(26814,0,7,4877.23,-4792.54,32.532,0,0,0,''),
(26814,0,8,4878.42,-4793.89,32.549,0,5000,0,'SAY_CHAMBER_1'),
(26814,0,9,4878.42,-4793.89,32.549,0,5000,0,'SAY_CHAMBER_2'),
(26814,0,10,4883.79,-4796.65,32.575,0,0,0,''),
(26814,0,11,4908.43,-4797.98,32.514,0,4000,0,'open cage'),
(26814,0,12,4908.43,-4797.98,32.514,0,3000,0,'SAY_CHAMBER_RELEASE'),
(26814,0,13,4908.43,-4797.98,32.514,0,2000,0,'SAY_THANK_YOU'),
(26814,0,14,4908.68,-4806.94,32.283,0,0,0,''),
(26814,0,15,4911.2,-4817.79,32.491,0,0,0,''),
(26814,0,16,4914.57,-4823.82,32.666,0,3000,0,''),
(26814,0,17,4914.57,-4823.82,32.666,0,7000,0,'bang gong'),
(26814,0,18,4908.56,-4820.37,32.55,0,5000,0,'SAY_CHAMBER_3'),
(26814,0,19,4908.56,-4820.37,32.55,0,0,0,'SAY_CHAMBER_4'),
(26814,0,20,4899.1,-4816.81,32.029,0,0,0,''),
(26814,0,21,4891.29,-4813.19,32.029,0,0,0,''),
(26814,0,22,4886.01,-4803.26,32.029,0,0,0,'close door'),
(26814,0,23,4883.62,-4799.12,32.556,0,1000,0,'SAY_CHAMBER_5 - set run'),
(26814,0,24,4900.58,-4806.63,32.029,0,7000,0,'SAY_CHAMBER_6'),
(26814,0,25,4900.58,-4806.63,32.029,0,6000,0,'SAY_CHAMBER_7'),
(26814,0,26,4900.58,-4806.63,32.029,0,0,0,'snake attack'),
(26814,0,27,4886.46,-4799.33,32.552,0,0,0,''),
(26814,0,28,4862.18,-4782.64,32.605,0,0,0,''),
(26814,0,29,4843.93,-4771.76,32.602,0,0,0,''),
(26814,0,30,4831.87,-4775.36,32.581,0,0,0,''),
(26814,0,31,4819.25,-4788.89,25.473,0,0,0,''),
(26814,0,32,4814.7,-4798.35,25.483,0,0,0,''),
(26814,0,33,4824.52,-4822.54,25.492,0,0,0,''),
(26814,0,34,4826.83,-4838.31,25.511,0,0,0,''),
(26814,0,35,4822.48,-4846.95,25.473,0,0,0,''),
(26814,0,36,4812.12,-4852.34,25.622,0,0,0,''),
(26814,0,37,4779.92,-4848.94,25.442,0,0,0,''),
(26814,0,38,4770.7,-4848.96,25.428,0,0,0,''),
(26814,0,39,4758.48,-4857.19,25.848,0,0,0,''),
(26814,0,40,4737.02,-4857.75,26.292,0,0,0,''),
(26814,0,41,4722.88,-4857.75,26.495,0,0,0,''),
(26814,0,42,4715.86,-4857.87,24.707,0,0,0,''),
(26814,0,43,4705.45,-4858.53,28.91,0,0,0,''),
(26814,0,44,4691.58,-4858.92,33.103,0,0,0,''),
(26814,0,45,4681.88,-4860.04,35.44,0,0,0,''),
(26814,0,46,4670.29,-4861.54,35.48,0,0,0,''),
(26814,0,47,4667.32,-4878.84,35.48,0,0,0,''),
(26814,0,48,4661.15,-4895.54,35.499,0,0,0,''),
(26814,0,49,4656.87,-4907.4,38.98,0,0,0,''),
(26814,0,50,4656.18,-4916.48,44.398,0,0,0,''),
(26814,0,51,4656.57,-4927.87,47.576,0,0,0,''),
(26814,0,52,4660.75,-4938.88,47.992,0,0,0,''),
(26814,0,53,4667.46,-4954.76,47.993,0,0,0,''),
(26814,0,54,4673.41,-4967.3,47.791,0,3000,0,'SAY_ESCORT_COMPLETE'),
(26814,0,55,4694.43,-4979.96,44.715,0,0,0,''),
(28217,0,1,5384.22,4533.26,-129.519,0,0,0,''),
(28217,0,2,5394.1,4531.19,-131.758,0,0,0,''),
(28217,0,3,5401.98,4527.3,-137.599,0,0,0,''),
(28217,0,4,5407.98,4526.48,-143.597,0,0,0,''),
(28217,0,5,5420.84,4519.58,-144.922,0,0,0,''),
(28217,0,6,5428.55,4522.23,-148.79,0,0,0,''),
(28217,0,7,5438.54,4536.08,-149.652,0,0,0,''),
(28217,0,8,5452.43,4553.94,-149.093,0,0,0,''),
(28217,0,9,5460.83,4564.37,-148.66,0,0,0,''),
(28217,0,10,5463.25,4584,-148.962,0,0,0,''),
(28217,0,11,5463.71,4603.71,-147.33,0,0,0,''),
(28217,0,12,5470.24,4609.12,-145.224,0,0,0,''),
(28217,0,13,5479.43,4609.2,-141.364,0,0,0,''),
(28217,0,14,5487.47,4615.62,-138.14,0,0,0,''),
(28217,0,15,5497.97,4634.8,-134.697,0,0,0,''),
(28217,0,16,5527.62,4648.05,-136.171,0,0,0,''),
(28217,0,17,5547.71,4651.72,-134.741,0,0,0,''),
(28217,0,18,5559.47,4652.01,-134.155,0,0,0,''),
(28217,0,19,5579.07,4652.29,-136.746,0,0,0,''),
(28217,0,20,5593.44,4643.72,-136.406,0,0,0,''),
(28217,0,21,5608.83,4630.81,-136.834,0,0,0,''),
(28217,0,22,5629.03,4607.48,-137.094,0,0,0,''),
(28217,0,23,5634.95,4600.2,-137.246,0,5000,0,'thanks and quest credit'),
(28217,0,24,5638.54,4594.92,-137.495,0,0,0,'summon'),
(28217,0,25,5638.06,4579.95,-138.029,0,0,0,''),
(28787,0,1,5913.52,5379.03,-98.8961,0,0,0,''),
(28787,0,2,5917.75,5374.52,-98.8698,0,0,0,'SAY_HELICE_EXPLOSIVES_1'),
(28787,0,3,5926.43,5372.15,-98.8845,0,0,0,''),
(28787,0,4,5929.21,5377.8,-99.0201,0,0,0,''),
(28787,0,5,5927.62,5378.56,-99.0479,0,0,0,''),
(28787,0,6,5917.62,5383.49,-106.31,0,0,0,''),
(28787,0,7,5908.99,5387.66,-106.31,0,0,0,''),
(28787,0,8,5906.29,5390.5,-106.042,0,0,0,''),
(28787,0,9,5902.42,5399.74,-99.3066,0,0,0,''),
(28787,0,10,5901.44,5404.59,-96.7596,0,0,0,''),
(28787,0,11,5897.86,5406.66,-96.0297,0,0,0,''),
(28787,0,12,5892.25,5401.29,-95.8488,0,0,0,''),
(28787,0,13,5887.42,5386.7,-95.4001,0,0,0,'SAY_HELICE_EXPLOSIVES_2'),
(28787,0,14,5883.31,5385.06,-94.4237,0,0,0,''),
(28787,0,15,5879.18,5375.9,-95.0881,0,0,0,''),
(28787,0,16,5872.61,5363.47,-97.7036,0,0,0,''),
(28787,0,17,5857.97,5354.93,-98.5861,0,0,0,''),
(28787,0,18,5848.73,5345.33,-99.429,0,0,0,''),
(28787,0,19,5842.33,5335.02,-100.421,0,0,0,''),
(28787,0,20,5832.16,5323.15,-98.7033,0,0,0,''),
(28787,0,21,5824.74,5315.71,-97.758,0,0,0,''),
(28787,0,22,5819.65,5305.41,-97.4818,0,10000,0,'SAY_HELICE_COMPLETE'),
(29434,0,1,6643.66,-1258.14,396.812,0,0,0,'SAY_ESCORT_READY'),
(29434,0,2,6669.84,-1261.13,396.362,0,0,0,''),
(29434,0,3,6672.48,-1244.1,396.644,0,0,0,''),
(29434,0,4,6665.35,-1229.89,399.214,0,0,0,''),
(29434,0,5,6656.88,-1210.86,399.819,0,0,0,''),
(29434,0,6,6658.69,-1187.53,398.761,0,0,0,''),
(29434,0,7,6664.34,-1166.37,398.633,0,0,0,''),
(29434,0,8,6667.77,-1157.03,398.136,0,0,0,''),
(29434,0,9,6670,-1145.67,398.019,0,0,0,''),
(29434,0,10,6678.49,-1120.1,397.16,0,0,0,''),
(29434,0,11,6685.05,-1100.97,396.287,0,0,0,''),
(29434,0,12,6682.75,-1087.74,396.795,0,0,0,''),
(29434,0,13,6679.6,-1073.34,404.633,0,0,0,''),
(29434,0,14,6680.32,-1066.26,405.499,0,0,0,''),
(29434,0,15,6689.71,-1053.83,407.333,0,0,0,''),
(29434,0,16,6696.24,-1043.51,411.23,0,0,0,''),
(29434,0,17,6695.09,-1032.21,414.625,0,0,0,''),
(29434,0,18,6690.72,-1016.45,414.825,0,0,0,''),
(29434,0,19,6679.98,-1009.8,414.836,0,0,0,''),
(29434,0,20,6664.82,-1009.98,414.84,0,0,0,''),
(29434,0,21,6647.98,-1010.35,418.831,0,0,0,''),
(29434,0,22,6635.37,-1010.64,423.007,0,0,0,''),
(29434,0,23,6615.76,-1001.9,426.584,0,0,0,''),
(29434,0,24,6597.33,-1002.8,429.766,0,0,0,''),
(29434,0,25,6581.18,-1009.97,433.705,0,0,0,''),
(29434,0,26,6562.83,-1016.12,433.558,0,0,0,''),
(29434,0,27,6535.39,-1024.19,433.084,0,0,0,''),
(29434,0,28,6520.09,-1030.28,433.506,0,0,0,''),
(29434,0,29,6505.7,-1028.77,436.897,0,0,0,''),
(29434,0,30,6496.5,-1027.35,437.309,0,0,0,''),
(29434,0,31,6489.65,-1026.46,434.885,0,0,0,''),
(29434,0,32,6474.28,-1024.47,434.65,0,0,0,''),
(29434,0,33,6456.69,-1022.17,432.239,0,0,0,''),
(29434,0,34,6449.76,-1021.35,431.501,0,6000,0,'SAY_ESCORT_COMPLETE'),
(29434,0,35,6418.64,-1018.39,427.91,0,0,0,'despawn'),
(29434,0,36,6639.77,-1109.59,427.193,0,0,0,''),
(29434,0,37,6641.52,-1104.35,426.97,0,0,0,''),
(29434,0,38,6659.7,-1106.49,423.005,0,0,0,''),
(29434,0,39,6670.65,-1118.34,424.474,0,0,0,''),
(29434,0,40,6666.2,-1130.1,423.113,0,0,0,''),
(29434,0,41,6642.68,-1129.11,416.779,0,0,0,''),
(29434,0,42,6628.48,-1127.42,414.923,0,0,0,''),
(29434,0,43,6619.76,-1113.34,412.185,0,0,0,''),
(29434,0,44,6622.96,-1101.69,409.846,0,0,0,''),
(29434,0,45,6640.45,-1088.53,403.227,0,0,0,''),
(29434,0,46,6659.59,-1073.82,402.945,0,0,0,''),
(29434,0,47,6671.06,-1064.83,405.381,0,0,0,'continue at wp 13'),
(31279,0,1,6717.81,3451.98,683.747,0,5000,0,'SAY_ESCORT_START_1'),
(31279,0,2,6717.81,3451.98,683.747,0,2000,0,'SAY_ESCORT_START_2'),
(31279,0,3,6718.85,3436.95,682.197,0,0,0,''),
(31279,0,4,6725.71,3432.64,682.197,0,0,0,''),
(31279,0,5,6733.12,3435.03,682.136,0,0,0,''),
(31279,0,6,6744.93,3445.79,679.032,0,0,0,''),
(31279,0,7,6760.19,3459.46,674.487,0,0,0,''),
(31279,0,8,6773.16,3469.68,673.155,0,0,0,''),
(31279,0,9,6783.85,3480.48,674.481,0,0,0,''),
(31279,0,10,6790.62,3484.06,676.671,0,0,0,''),
(31279,0,11,6805.92,3483.84,682.128,0,0,0,''),
(31279,0,12,6818.43,3483.29,686.889,0,0,0,''),
(31279,0,13,6832.83,3480.98,690.189,0,0,0,''),
(31279,0,14,6854.91,3479.89,693.181,0,0,0,''),
(31279,0,15,6873.59,3478.93,694.618,0,0,0,''),
(31279,0,16,6895.13,3478.39,698.266,0,0,0,''),
(31279,0,17,6916.83,3478.49,702.575,0,0,0,''),
(31279,0,18,6937.28,3477.34,707.257,0,0,0,''),
(31279,0,19,6959.09,3472.78,710.18,0,0,0,''),
(31279,0,20,6969.53,3470.09,710.401,0,0,0,''),
(31279,0,21,6980.07,3466.87,710.831,0,0,0,''),
(31279,0,22,7008.2,3457.3,696.672,0,0,0,''),
(31279,0,23,7020.18,3452.48,696.518,0,0,0,''),
(31279,0,24,7031.36,3445.23,696.108,0,3000,0,'SAY_KAMAROS_COMPLETE_1'),
(31279,0,25,7031.36,3445.23,696.108,0,7000,0,'SAY_KAMAROS_COMPLETE_2'),
(31279,0,26,7067.66,3420.74,694.879,0,0,0,''),
(31737,0,1,7269.77,1509.43,320.903,0,0,0,''),
(31737,0,2,7258.12,1526.6,324.304,0,0,0,''),
(31737,0,3,7260.97,1549.84,335.689,0,1000,0,'SAY_ALLIANCE_RUN'),
(31737,0,4,7264.85,1564.69,341.974,0,0,0,''),
(31737,0,5,7255.5,1579.52,351.389,0,0,0,''),
(31737,0,6,7246.57,1583.33,358.133,0,0,0,''),
(31737,0,7,7232.84,1581.03,367.501,0,0,0,'first attack'),
(31737,0,8,7223.73,1580.09,373.346,0,0,0,''),
(31737,0,9,7218.68,1586.35,377.49,0,0,0,''),
(31737,0,10,7217.37,1593.94,379.455,0,0,0,''),
(31737,0,11,7225.46,1598.87,379.647,0,0,0,''),
(31737,0,12,7237.81,1601.12,381.088,0,0,0,''),
(31737,0,13,7251.41,1609.02,383.766,0,0,0,''),
(31737,0,14,7265.52,1611.84,382.62,0,0,0,''),
(31737,0,15,7277.74,1609.8,383.899,0,0,0,''),
(31737,0,16,7290.88,1608.96,390.451,0,0,0,'second attack'),
(31737,0,17,7310.86,1615.48,400.58,0,0,0,''),
(31737,0,18,7327.59,1622.28,411.449,0,0,0,''),
(31737,0,19,7343.15,1629.88,423.033,0,0,0,''),
(31737,0,20,7347.38,1636.29,428.066,0,0,0,''),
(31737,0,21,7343.73,1644.67,430.427,0,8000,0,'SAY_ALLIANCE_BREAK'),
(31737,0,22,7343.73,1644.67,430.427,0,1000,0,'SAY_ALLIANCE_BREAK_DONE'),
(31737,0,23,7301.61,1649.02,434.578,0,0,0,''),
(31737,0,24,7291.13,1653.63,435.176,0,0,0,''),
(31737,0,25,7278.78,1657.08,434.619,0,0,0,''),
(31737,0,26,7259.07,1651.53,433.942,0,0,0,'gate attack'),
(31737,0,27,7243.21,1662.61,438.89,0,0,0,''),
(31737,0,28,7211.63,1684.33,462.316,0,0,0,'SAY_EVENT_COMPLETE'),
(31833,0,1,7504.98,1806.83,355.928,0,0,0,''),
(31833,0,2,7500.19,1817.22,355.494,0,0,0,''),
(31833,0,3,7492.7,1828.37,361.42,0,1000,0,'SAY_HORDER_RUN'),
(31833,0,4,7481.53,1836.77,370.704,0,0,0,''),
(31833,0,5,7463.6,1840.57,383.662,0,0,0,'first attack'),
(31833,0,6,7449.45,1839.82,394.694,0,0,0,''),
(31833,0,7,7432.16,1847.35,406.29,0,0,0,''),
(31833,0,8,7415.07,1845.62,419.79,0,0,0,''),
(31833,0,9,7409.83,1839.99,423.997,0,0,0,''),
(31833,0,10,7403.58,1822.6,428.435,0,0,0,'second attack'),
(31833,0,11,7398.86,1810.26,430.373,0,0,0,''),
(31833,0,12,7396.57,1789.4,432.286,0,0,0,''),
(31833,0,13,7397.82,1769.24,432.947,0,0,0,''),
(31833,0,14,7399.1,1745.27,433.108,0,8000,0,'SAY_HORDE_BREAK'),
(31833,0,15,7399.1,1745.27,433.108,0,1000,0,'SAY_HORDE_BREAK_DONE'),
(31833,0,16,7393.29,1729.91,435.058,0,0,0,''),
(31833,0,17,7385.3,1720.18,437.602,0,0,0,''),
(31833,0,18,7370.19,1715.58,442.425,0,0,0,''),
(31833,0,19,7358.27,1719.35,446.378,0,0,0,''),
(31833,0,20,7348.81,1723.01,449.727,0,0,0,''),
(31833,0,21,7333.27,1724.84,453.621,0,0,0,''),
(31833,0,22,7325.7,1725.66,456.896,0,0,0,''),
(31833,0,23,7319.81,1725.68,459.731,0,0,0,'gate attack'),
(31833,0,24,7308.11,1726.71,465.138,0,0,0,''),
(31833,0,25,7297.75,1727.79,467.98,0,0,0,''),
(31833,0,26,7288.28,1726.89,469.816,0,0,0,''),
(31833,0,27,7278.19,1722.63,472.149,0,0,0,''),
(31833,0,28,7253.08,1729.58,474.225,0,0,0,'SAY_EVENT_COMPLETE'),
(32800,0,1,6736.09,3422.16,683.457,0,5000,0,'SAY_ESCORT_START_1'),
(32800,0,2,6736.09,3422.16,683.457,0,2000,0,'SAY_ESCORT_START_2'),
(32800,0,3,6734.52,3425.64,682.517,0,0,0,''),
(32800,0,4,6733.17,3430.8,682.156,0,0,0,''),
(32800,0,5,6733.12,3435.03,682.136,0,0,0,''),
(32800,0,6,6744.93,3445.79,679.032,0,0,0,''),
(32800,0,7,6760.19,3459.46,674.487,0,0,0,''),
(32800,0,8,6773.16,3469.68,673.155,0,0,0,''),
(32800,0,9,6783.85,3480.48,674.481,0,0,0,''),
(32800,0,10,6790.62,3484.06,676.671,0,0,0,''),
(32800,0,11,6805.92,3483.84,682.128,0,0,0,''),
(32800,0,12,6818.43,3483.29,686.889,0,0,0,''),
(32800,0,13,6832.83,3480.98,690.189,0,0,0,''),
(32800,0,14,6854.91,3479.89,693.181,0,0,0,''),
(32800,0,15,6873.59,3478.93,694.618,0,0,0,''),
(32800,0,16,6895.13,3478.39,698.266,0,0,0,''),
(32800,0,17,6916.83,3478.49,702.575,0,0,0,''),
(32800,0,18,6937.28,3477.34,707.257,0,0,0,''),
(32800,0,19,6959.09,3472.78,710.18,0,0,0,''),
(32800,0,20,6969.53,3470.09,710.401,0,0,0,''),
(32800,0,21,6980.07,3466.87,710.831,0,0,0,''),
(32800,0,22,7008.2,3457.3,696.672,0,0,0,''),
(32800,0,23,7020.18,3452.48,696.518,0,0,0,''),
(32800,0,24,7031.36,3445.23,696.108,0,3000,0,'SAY_KAMAROS_COMPLETE_1'),
(32800,0,25,7031.36,3445.23,696.108,0,7000,0,'SAY_KAMAROS_COMPLETE_2'),
(32800,0,26,7067.66,3420.74,694.879,0,0,0,'');

-- EOF

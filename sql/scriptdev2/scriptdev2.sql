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
DELETE FROM scripted_areatrigger WHERE entry=4591;
INSERT INTO scripted_areatrigger VALUES (4591,'at_coilfang_waterfall');
DELETE FROM scripted_areatrigger WHERE entry=4560;
INSERT INTO scripted_areatrigger VALUES (4560,'at_legion_teleporter');
DELETE FROM scripted_areatrigger WHERE entry=3066;
INSERT INTO scripted_areatrigger VALUES (3066,'at_ravenholdt');
DELETE FROM scripted_areatrigger WHERE entry IN (4871,4872,4873);
INSERT INTO scripted_areatrigger VALUES
(4871,'at_warsong_farms'),
(4872,'at_warsong_farms'),
(4873,'at_warsong_farms');
DELETE FROM scripted_areatrigger WHERE entry IN (5046,5047);
INSERT INTO scripted_areatrigger VALUES
(5046,'at_waygate'),
(5047,'at_waygate');
DELETE FROM scripted_areatrigger WHERE entry BETWEEN 5284 AND 5287;
INSERT INTO scripted_areatrigger VALUES
(5284,'at_aldurthar_gate'),
(5285,'at_aldurthar_gate'),
(5286,'at_aldurthar_gate'),
(5287,'at_aldurthar_gate');
DELETE FROM scripted_areatrigger WHERE entry IN (4112,4113);
INSERT INTO scripted_areatrigger VALUES
(4112,'at_naxxramas'),
(4113,'at_naxxramas');
DELETE FROM scripted_areatrigger WHERE entry=5108;
INSERT INTO scripted_areatrigger VALUES (5108,'at_stormwright_shelf');
DELETE FROM scripted_areatrigger WHERE entry IN (3546,3547,3548,3549,3550,3552);
INSERT INTO scripted_areatrigger VALUES
(3546,'at_childrens_week_spot'), -- Darnassian bank
(3547,'at_childrens_week_spot'), -- Undercity - thone room
(3548,'at_childrens_week_spot'), -- Stonewrought Dam
(3549,'at_childrens_week_spot'), -- The Mor'shan Rampart
(3550,'at_childrens_week_spot'), -- Ratchet Docks
(3552,'at_childrens_week_spot'); -- Westfall Lighthouse
DELETE FROM scripted_areatrigger WHERE entry IN (2026,2046,2066,2067);
INSERT INTO scripted_areatrigger VALUES
(2026,'at_blackrock_spire'),
(2046,'at_blackrock_spire'),
(2066,'at_blackrock_spire'),
(2067,'at_blackrock_spire');
DELETE FROM scripted_areatrigger WHERE entry=5030;
INSERT INTO scripted_areatrigger VALUES (5030,'at_spearborn_encampment');
DELETE FROM scripted_areatrigger WHERE entry IN (3958,3960);
INSERT INTO scripted_areatrigger VALUES
(3958,'at_zulgurub'),
(3960,'at_zulgurub');
DELETE FROM scripted_areatrigger WHERE entry=3626;
INSERT INTO scripted_areatrigger VALUES (3626,'at_vaelastrasz');
DELETE FROM scripted_areatrigger WHERE entry=4937;
INSERT INTO scripted_areatrigger VALUES (4937,'at_sunwell_plateau');
DELETE FROM scripted_areatrigger WHERE entry=4524;
INSERT INTO scripted_areatrigger VALUES (4524,'at_shattered_halls');
DELETE FROM scripted_areatrigger WHERE entry BETWEEN 1726 AND 1740;
INSERT INTO scripted_areatrigger VALUES
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
(1740,'at_scent_larkorwi');
DELETE FROM scripted_areatrigger WHERE entry in (4288,4485);
INSERT INTO scripted_areatrigger VALUES
(4288,'at_dark_portal'),
(4485,'at_dark_portal');
DELETE FROM scripted_areatrigger WHERE entry=1966;
INSERT INTO scripted_areatrigger VALUES (1966,'at_murkdeep');
DELETE FROM scripted_areatrigger WHERE entry IN (4047,4052);
INSERT INTO scripted_areatrigger VALUES
(4047,'at_temple_ahnqiraj'),
(4052,'at_temple_ahnqiraj');
DELETE FROM scripted_areatrigger WHERE entry IN (5710,5711,5712,5714,5715,5716);
INSERT INTO scripted_areatrigger VALUES
(5710, 'at_hot_on_the_trail'),
(5711, 'at_hot_on_the_trail'),
(5712, 'at_hot_on_the_trail'),
(5714, 'at_hot_on_the_trail'),
(5715, 'at_hot_on_the_trail'),
(5716, 'at_hot_on_the_trail');
DELETE FROM scripted_areatrigger WHERE entry=3587;
INSERT INTO scripted_areatrigger VALUES (3587,'at_ancient_leaf');
DELETE FROM scripted_areatrigger WHERE entry=4479;
INSERT INTO scripted_areatrigger VALUES (4479,'at_haramad_teleport');


/* BATTLEGROUNDS */
UPDATE creature_template SET ScriptName='npc_spirit_guide' WHERE entry IN (13116, 13117);

/* WORLD BOSS */
UPDATE creature_template SET ScriptName='boss_ysondre' WHERE entry=14887;
UPDATE creature_template SET ScriptName='boss_emeriss' WHERE entry=14889;
UPDATE creature_template SET ScriptName='boss_taerar' WHERE entry=14890;
UPDATE creature_template SET ScriptName='boss_azuregos' WHERE entry=6109;
UPDATE creature_template SET ScriptName='boss_lethon' WHERE entry=14888;
UPDATE creature_template SET ScriptName='npc_spirit_shade' WHERE entry=15261;

/* GO */
UPDATE gameobject_template SET ScriptName='go_ethereum_prison' WHERE entry BETWEEN 184418 AND 184431;
UPDATE gameobject_template SET ScriptName='go_ethereum_stasis' WHERE entry BETWEEN 185465 AND 185467;
UPDATE gameobject_template SET ScriptName='go_ethereum_stasis' WHERE entry=184595;
UPDATE gameobject_template SET ScriptName='go_ethereum_stasis' WHERE entry BETWEEN 185461 AND 185464;
UPDATE gameobject_template SET ScriptName='go_jump_a_tron' WHERE entry=183146;
UPDATE gameobject_template SET ScriptName='go_mysterious_snow_mound' WHERE entry=195308;
UPDATE gameobject_template SET ScriptName='go_tele_to_dalaran_crystal' WHERE entry=191230;
UPDATE gameobject_template SET ScriptName='go_tele_to_violet_stand' WHERE entry=191229;
UPDATE gameobject_template SET ScriptName='go_andorhal_tower' WHERE entry IN (176094,176095,176096,176097);
UPDATE gameobject_template SET ScriptName='go_scourge_enclosure' WHERE entry=191548;
UPDATE gameobject_template SET ScriptName='go_veil_skith_cage' WHERE entry IN (185202,185203,185204,185205);
UPDATE gameobject_template SET ScriptName='go_lab_work_reagents' WHERE entry IN (190462, 190473, 190478, 190459);

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
UPDATE creature_template SET ScriptName='guard_dunmorogh' WHERE entry IN (727,13076);
UPDATE creature_template SET ScriptName='guard_tirisfal' WHERE entry IN (1735,1738,2210,1744,1745,5725,1743,2209,1746,1742);
UPDATE creature_template SET ScriptName='guard_silvermoon' WHERE entry=16222;
UPDATE creature_template SET ScriptName='guard_exodar' WHERE entry=16733;
UPDATE creature_template SET ScriptName='guard_shattrath' WHERE entry=19687;
UPDATE creature_template SET ScriptName='guard_shattrath_aldor' WHERE entry=18549;
UPDATE creature_template SET ScriptName='guard_shattrath_scryer' WHERE entry=18568;

/* ITEM */
UPDATE item_template SET ScriptName='item_arcane_charges' WHERE entry=34475;
UPDATE item_template SET ScriptName='item_flying_machine' WHERE entry IN (34060,34061);
UPDATE item_template SET ScriptName='item_gor_dreks_ointment' WHERE entry=30175;
UPDATE item_template SET ScriptName='item_petrov_cluster_bombs' WHERE entry=33098;
UPDATE item_template SET ScriptName='item_orb_of_draconic_energy' WHERE entry=12300;

/* NPC (usually creatures to be found in more than one specific zone) */
UPDATE creature_template SET ScriptName='npc_air_force_bots' WHERE entry IN (2614, 2615, 21974, 21993, 21996, 21997, 21999, 22001, 22002, 22003, 22063, 22065, 22066, 22068, 22069, 22070, 22071, 22078, 22079, 22080, 22086, 22087, 22088, 22090, 22124, 22125, 22126);
UPDATE creature_template SET ScriptName='npc_chicken_cluck' WHERE entry=620;
UPDATE creature_template SET ScriptName='npc_dancing_flames' WHERE entry=25305;
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
UPDATE creature_template SET ScriptName='npc_burster_worm' WHERE entry IN (16844,16857,16968,21380,21849,22038,22466,22482,23285);

/* SPELL */
UPDATE creature_template SET ScriptName='spell_dummy_npc' WHERE entry IN (
-- eastern kingdoms
1200,8888,13016,
-- kalimdor
9299,12296,12298,
-- outland
16880,16518,16847,17157,17326,17654,18879,21729,22105,24918,24922,25084,25085,
-- northrend
23678,25752,25753,25758,25792,25793,26268,26270,26421,26616,26643,26841,26924,27122,27808,28053,28054,28068,28093,28465,28600,29319,29327,29329,29330,29338,30146,30169,32149);

UPDATE gameobject_template SET ScriptName='spell_dummy_go' WHERE entry IN (181616,186949);

/* WORLD MAP SCRIPTS */
DELETE FROM world_template WHERE map IN (0, 1, 530, 571, 609);
INSERT INTO world_template VALUES
(0, 'world_map_eastern_kingdoms'),
(1, 'world_map_kalimdor'),
(530, 'world_map_outland'),
(571, 'world_map_northrend'),
(609, 'world_map_ebon_hold');

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

/* AUCHENAI CRYPTS */
UPDATE creature_template SET ScriptName='boss_exarch_maladaar' WHERE entry=18373;
UPDATE creature_template SET ScriptName='mob_stolen_soul' WHERE entry=18441;
UPDATE creature_template SET ScriptName='boss_shirrak' WHERE entry=18371;

/* SETHEKK HALLS */
UPDATE instance_template SET ScriptName='instance_sethekk_halls' WHERE map=556;
UPDATE creature_template SET ScriptName='boss_talon_king_ikiss' WHERE entry=18473;
UPDATE creature_template SET ScriptName='boss_darkweaver_syth' WHERE entry=18472;
UPDATE creature_template SET ScriptName='boss_anzu' WHERE entry=23035;
DELETE FROM scripted_event_id WHERE id=14797;
INSERT INTO scripted_event_id VALUES
(14797,'event_spell_summon_raven_god');

/* SHADOW LABYRINTH */
UPDATE instance_template SET ScriptName='instance_shadow_labyrinth' WHERE map=555;
UPDATE creature_template SET ScriptName='boss_murmur' WHERE entry=18708;
UPDATE creature_template SET ScriptName='boss_grandmaster_vorpil' WHERE entry=18732;
UPDATE creature_template SET ScriptName='boss_blackheart_the_inciter' WHERE entry=18667;
UPDATE creature_template SET ScriptName='boss_ambassador_hellmaw' WHERE entry=18731;
UPDATE creature_template SET ScriptName='npc_void_traveler' WHERE entry=19226;

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

/* AZUREMYST ISLE */
UPDATE creature_template SET ScriptName='npc_draenei_survivor' WHERE entry=16483;
UPDATE creature_template SET ScriptName='npc_magwin' WHERE entry=17312;

/* BADLANDS */

/* BARRENS */
UPDATE creature_template SET ScriptName='npc_gilthares' WHERE entry=3465;
UPDATE creature_template SET ScriptName='npc_taskmaster_fizzule' WHERE entry=7233;
UPDATE creature_template SET ScriptName='npc_twiggy_flathead' WHERE entry=6248;
DELETE FROM scripted_areatrigger WHERE entry=522;
INSERT INTO scripted_areatrigger VALUES (522,'at_twiggy_flathead');
UPDATE creature_template SET ScriptName='npc_wizzlecranks_shredder' WHERE entry=3439;
UPDATE creature_template SET ScriptName='npc_gallywix' WHERE entry=7288;

/* BLACK TEMPLE */
UPDATE instance_template SET ScriptName='instance_black_temple' WHERE map=564;
UPDATE creature_template SET ScriptName='npc_akama_shade' WHERE entry=22990; -- Akama at Shade of Akama
UPDATE creature_template SET ScriptName='npc_akama_illidan' WHERE entry=23089; -- Akama at Illidan
UPDATE creature_template SET ScriptName='mob_illidari_council' WHERE entry=23426; -- Illidari Council controller mob
UPDATE creature_template SET ScriptName='mob_blood_elf_council_voice_trigger' WHERE entry=23499; -- Voice Trigger Mob (Controls Aggro + Enrage yells)
UPDATE creature_template SET ScriptName='boss_veras_darkshadow' WHERE entry=22952; -- Rogue of Illidari Council
UPDATE creature_template SET ScriptName='boss_teron_gorefiend' WHERE entry=22871; -- Teron Gorefiend
UPDATE creature_template SET ScriptName='boss_supremus' WHERE entry=22898; -- Supremus
UPDATE creature_template SET ScriptName='boss_shade_of_akama' WHERE entry=22841; -- Shade of Akama
UPDATE creature_template SET ScriptName='boss_reliquary_of_souls' WHERE entry=22856; -- Reliquary Controller Mob
UPDATE creature_template SET ScriptName='boss_essence_of_suffering' WHERE entry=23418; -- Essence Of Suffering
UPDATE creature_template SET ScriptName='boss_essence_of_desire' WHERE entry=23419; -- Essence of Desire
UPDATE creature_template SET ScriptName='boss_essence_of_anger' WHERE entry=23420; -- Essence of Anger
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
UPDATE creature_template SET ScriptName='npc_volcano' WHERE entry=23085; -- Supremus Volcano
UPDATE creature_template SET ScriptName='molten_flame' WHERE entry=23095; -- Molten Flame in SUpremus
UPDATE creature_template SET ScriptName='mob_ashtongue_channeler' WHERE entry=23421; -- Ashtongue CHanneler in Shade of AKama
UPDATE creature_template SET ScriptName='mob_ashtongue_sorcerer' WHERE entry=23215; -- Ashtongue Sorcerer in Shade of Akama
UPDATE creature_template SET ScriptName='npc_spirit_of_olum' WHERE entry=23411;
UPDATE creature_template SET ScriptName='npc_enslaved_soul' WHERE entry=23469;

/* BLACKFATHOM DEPTHS */
UPDATE instance_template SET ScriptName='instance_blackfathom_deeps' WHERE map=48;
UPDATE gameobject_template SET ScriptName='go_fire_of_akumai' WHERE entry IN (21118,21119,21120,21121);
UPDATE gameobject_template SET ScriptName='go_fathom_stone' WHERE entry=177964;

/* BLACKROCK DEPTHS */
DELETE FROM scripted_areatrigger WHERE entry=1526;
INSERT INTO scripted_areatrigger VALUES (1526,'at_ring_of_law');
DELETE FROM scripted_areatrigger WHERE entry=1786;
INSERT INTO scripted_areatrigger VALUES (1786,'at_shadowforge_bridge');
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
DELETE FROM scripted_event_id WHERE id=4884;
INSERT INTO scripted_event_id VALUES
(4884,'event_spell_altar_emberseer');
UPDATE gameobject_template SET ScriptName='go_father_flame' WHERE entry=175245;

/* BLACKWING LAIR */
UPDATE instance_template SET ScriptName='instance_blackwing_lair' WHERE map=469;
UPDATE creature_template SET ScriptName='boss_razorgore' WHERE entry=12435;
UPDATE gameobject_template SET ScriptName='go_black_dragon_egg' WHERE entry=177807;
UPDATE creature_template SET ScriptName='boss_vaelastrasz' WHERE entry=13020;
UPDATE creature_template SET ScriptName='boss_broodlord' WHERE entry=12017;
UPDATE creature_template SET ScriptName='boss_firemaw' WHERE entry=11983;
UPDATE creature_template SET ScriptName='boss_ebonroc' WHERE entry=14601;
UPDATE creature_template SET ScriptName='boss_flamegor' WHERE entry=11981;
UPDATE creature_template SET ScriptName='boss_chromaggus' WHERE entry=14020;
UPDATE creature_template SET ScriptName='boss_victor_nefarius' WHERE entry=10162;
UPDATE creature_template SET ScriptName='boss_nefarian' WHERE entry=11583;

/* BLADE'S EDGE MOUNTAINS */
UPDATE creature_template SET ScriptName='mobs_nether_drake' WHERE entry IN (20021,21817,21820,21821,21823);
UPDATE creature_template SET ScriptName='npc_daranelle' WHERE entry=21469;
UPDATE creature_template SET ScriptName='npc_bloodmaul_stout_trigger' WHERE entry=21241;
UPDATE creature_template SET ScriptName='npc_simon_game_bunny' WHERE entry=22923;
UPDATE creature_template SET ScriptName='npc_light_orb_collector' WHERE entry IN (21926,22333);
UPDATE creature_template SET ScriptName='npc_bloodmaul_dire_wolf' WHERE entry=20058;
UPDATE creature_template SET ScriptName='npc_vimgol_visual_bunny' WHERE entry=23040;
UPDATE creature_template SET ScriptName='npc_vimgol_middle_bunny' WHERE entry=23081;
UPDATE creature_template SET ScriptName='npc_vimgol' WHERE entry=22911;
UPDATE creature_template SET ScriptName='npc_spirit_prisoner_of_bladespire' WHERE entry IN(22460);

/* BLASTED LANDS */
UPDATE creature_template SET ScriptName='npc_fallen_hero_of_horde' WHERE entry=7572;

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

/* BURNING STEPPES */
UPDATE creature_template SET ScriptName='npc_ragged_john' WHERE entry=9563;
UPDATE creature_template SET ScriptName='npc_grark_lorkrub' WHERE entry=9520;

/*  */
/* CAVERNS OF TIME */
/*  */

/* MT. HYJAL */
UPDATE instance_template SET ScriptName='instance_hyjal' WHERE map=534;
UPDATE creature_template SET ScriptName='npc_tyrande_whisperwind' WHERE entry=17948;
UPDATE creature_template SET ScriptName='npc_thrall' WHERE entry=17852;
UPDATE creature_template SET ScriptName='npc_jaina_proudmoore' WHERE entry=17772;
UPDATE creature_template SET ScriptName='boss_archimonde' WHERE entry=17968;
UPDATE creature_template SET ScriptName='npc_doomfire_spirit' WHERE entry=18104;

/* OLD HILLSBRAD */
UPDATE instance_template SET ScriptName='instance_old_hillsbrad' WHERE map=560;
UPDATE creature_template SET ScriptName='npc_erozion' WHERE entry=18723;
UPDATE creature_template SET ScriptName='npc_taretha' WHERE entry=18887;
UPDATE creature_template SET ScriptName='npc_thrall_old_hillsbrad' WHERE entry=17876;
DELETE FROM scripted_event_id WHERE id=11111;
INSERT INTO scripted_event_id VALUES
(11111,'event_go_barrel_old_hillsbrad');

/* THE CULLING OF STRATHOLME */
UPDATE instance_template SET ScriptName='instance_culling_of_stratholme' WHERE map=595;
UPDATE creature_template SET ScriptName='npc_chromie' WHERE entry IN (26527, 27915);
UPDATE creature_template SET ScriptName='spell_dummy_npc_crates_bunny' WHERE entry=27827;
UPDATE creature_template SET ScriptName='npc_spell_dummy_crusader_strike' WHERE entry IN (28167,28169);
UPDATE creature_template SET ScriptName='npc_arthas' WHERE entry=26499;
DELETE FROM scripted_areatrigger WHERE entry=5291;
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

/* THE UNDERBOG */
UPDATE creature_template SET ScriptName='mob_underbog_mushroom' WHERE entry=17990;
UPDATE creature_template SET ScriptName='boss_hungarfen' WHERE entry=17770;

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
UPDATE creature_template SET ScriptName='boss_hydross_the_unstable' WHERE entry=21216;
/* Leotheras the Blind event */
UPDATE creature_template SET ScriptName='boss_leotheras_the_blind' WHERE entry=21215;
/* Fathom-lord Karathress event */
UPDATE creature_template SET ScriptName='boss_fathomlord_karathress' WHERE entry=21214;
UPDATE creature_template SET ScriptName='boss_fathomguard_sharkkis' WHERE entry=21966;
UPDATE creature_template SET ScriptName='boss_fathomguard_tidalvess' WHERE entry=21965;
UPDATE creature_template SET ScriptName='boss_fathomguard_caribdis' WHERE entry=21964;
/* Morogrim Tidewalker event */
UPDATE creature_template SET ScriptName='boss_morogrim_tidewalker' WHERE entry=21213;
UPDATE creature_template SET ScriptName='mob_water_globule' WHERE entry=21913;
/* Lady Vashj event */
UPDATE creature_template SET ScriptName='boss_lady_vashj' WHERE entry=21212;
UPDATE creature_template SET ScriptName='mob_enchanted_elemental' WHERE entry=21958;
UPDATE gameobject_template SET ScriptName='go_shield_generator' WHERE entry IN (185051,185052,185053,185054);
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
UPDATE creature_template SET ScriptName='npc_valkyr_stalker' WHERE entry IN (34704,34720);
UPDATE creature_template SET ScriptName='npc_anubarak_spike' WHERE entry=34660;
UPDATE creature_template SET ScriptName='npc_frost_sphere' WHERE entry=34606;
UPDATE creature_template SET ScriptName='npc_nerubian_borrow' WHERE entry=34862;
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

/* DALARAN */
UPDATE creature_template SET ScriptName='npc_dalaran_guardian_mage' WHERE entry IN (29255, 29254);

/* DARKSHORE */
UPDATE creature_template SET ScriptName='npc_kerlonian' WHERE entry=11218;
UPDATE creature_template SET ScriptName='npc_prospector_remtravel' WHERE entry=2917;
UPDATE creature_template SET ScriptName='npc_threshwackonator' WHERE entry=6669;
UPDATE creature_template SET ScriptName='npc_volcor' WHERE entry=3692;
UPDATE creature_template SET ScriptName='npc_therylune' WHERE entry=3584;
UPDATE creature_template SET ScriptName='npc_rabid_bear' WHERE entry=2164;

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
DELETE FROM scripted_event_id WHERE id IN (8420,8428);
INSERT INTO scripted_event_id VALUES
(8420,'event_spells_warlock_dreadsteed'),
(8428,'event_spells_warlock_dreadsteed');

/* DRAGONBLIGHT */
UPDATE creature_template SET ScriptName='npc_destructive_ward' WHERE entry=27430;
UPDATE creature_template SET ScriptName='npc_crystalline_ice_giant' WHERE entry=26809;

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
DELETE FROM scripted_areatrigger WHERE entry=4752;
INSERT INTO scripted_areatrigger VALUES
(4752,'at_nats_landing');

/* EASTERN PLAGUELANDS */
UPDATE creature_template SET ScriptName='npc_eris_havenfire' WHERE entry=14494;

/* EBON HOLD */
UPDATE creature_template SET ScriptName='npc_death_knight_initiate' WHERE entry=28406;
UPDATE creature_template SET ScriptName='npc_unworthy_initiate_anchor' WHERE entry=29521;
UPDATE creature_template SET ScriptName='npc_unworthy_initiate' WHERE entry IN (29519,29520,29565,29566,29567);
UPDATE gameobject_template SET ScriptName='go_acherus_soul_prison' WHERE entry IN (191577,191580,191581,191582,191583,191584,191585,191586,191587,191588,191589,191590);
UPDATE creature_template SET ScriptName='npc_a_special_surprise' WHERE entry IN (29032,29061,29065,29067,29068,29070,29074,29072,29073,29071);
UPDATE creature_template SET ScriptName='npc_eye_of_acherus' WHERE entry=28511;
UPDATE creature_template SET ScriptName='npc_scarlet_ghoul' WHERE entry=28845;
UPDATE creature_template SET ScriptName='npc_highlord_darion_mograine' WHERE entry=29173;
UPDATE creature_template SET ScriptName='npc_fellow_death_knight' WHERE entry IN (29199, 29204, 29200);
UPDATE creature_template SET ScriptName='npc_lich_king_light_dawn' WHERE entry=29183;
UPDATE creature_template SET ScriptName='npc_scarlet_courier' WHERE entry=29076;

/* ELWYNN FOREST */

/* EVERSONG WOODS */
UPDATE creature_template SET ScriptName='npc_kelerun_bloodmourn' WHERE entry=17807;
UPDATE gameobject_template SET ScriptName='go_harbinger_second_trial' WHERE entry=182052;
UPDATE creature_template SET ScriptName='npc_prospector_anvilward' WHERE entry=15420;
UPDATE creature_template SET ScriptName='npc_apprentice_mirveda' WHERE entry=15402;
UPDATE creature_template SET ScriptName='npc_infused_crystal' WHERE entry=16364;

/* FELWOOD */
DELETE FROM scripted_event_id WHERE id=8328;
INSERT INTO scripted_event_id VALUES
(8328, 'npc_kroshius');
UPDATE creature_template SET ScriptName='npc_kitten' WHERE entry=9937;
UPDATE creature_template SET ScriptName='npc_corrupt_saber' WHERE entry=10042;
UPDATE creature_template SET ScriptName='npc_niby_the_almighty' WHERE entry=14469;
UPDATE creature_template SET ScriptName='npc_kroshius' WHERE entry=14467;
UPDATE creature_template SET ScriptName='npc_captured_arkonarin' WHERE entry=11016;
UPDATE creature_template SET ScriptName='npc_arei' WHERE entry=9598;

/* FERALAS */
UPDATE creature_template SET ScriptName='npc_oox22fe' WHERE entry=7807;
UPDATE creature_template SET ScriptName='npc_shay_leafrunner' WHERE entry=7774;

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
UPDATE creature_template SET ScriptName='boss_eck' WHERE entry=29932;
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
UPDATE creature_template SET ScriptName='mob_broggok_poisoncloud' WHERE entry=17662;
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
UPDATE creature_template SET ScriptName='boss_grand_warlock_nethekurse' WHERE entry=16807;
UPDATE creature_template SET ScriptName='boss_warbringer_omrogg' WHERE entry=16809;
UPDATE creature_template SET ScriptName='mob_fel_orc_convert' WHERE entry=17083;
UPDATE creature_template SET ScriptName='mob_lesser_shadow_fissure' WHERE entry=17471;
UPDATE creature_template SET ScriptName='mob_omrogg_heads' WHERE entry IN (19523,19524);
UPDATE creature_template SET ScriptName='boss_warchief_kargath_bladefist' WHERE entry=16808;
UPDATE instance_template SET ScriptName='instance_shattered_halls' WHERE map=540;

/* MAGTHERIDON'S LAIR */
UPDATE instance_template SET ScriptName='instance_magtheridons_lair' WHERE map=544;
UPDATE gameobject_template SET ScriptName='go_manticron_cube' WHERE entry=181713;
UPDATE creature_template SET ScriptName='boss_magtheridon' WHERE entry=17257;
UPDATE creature_template SET ScriptName='mob_abyssal' WHERE entry=17454;
UPDATE creature_template SET ScriptName='mob_hellfire_channeler' WHERE entry=17256;
UPDATE creature_template SET ScriptName='npc_target_trigger' WHERE entry=17474;

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

/* HILLSBRAD FOOTHILLS */

/* HINTERLANDS */
UPDATE creature_template SET ScriptName='npc_00x09hl' WHERE entry=7806;
UPDATE creature_template SET ScriptName='npc_rinji' WHERE entry=7780;

/* HOWLING FJORD */
DELETE FROM scripted_areatrigger WHERE entry IN (4778,4779);
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
UPDATE creature_template SET ScriptName='npc_coldflame' WHERE entry=36672;
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
UPDATE gameobject_template SET ScriptName='go_icc_teleporter' WHERE entry IN (202235,202242,202243,202244,202245,202246);
DELETE FROM scripted_event_id WHERE id IN (23426,23438);
INSERT INTO scripted_event_id VALUES
(23426,'event_gameobject_citadel_valve'),
(23438,'event_gameobject_citadel_valve');
DELETE FROM scripted_areatrigger WHERE entry IN (5604,5611,5612,5647,5709,5718,5732);
INSERT INTO scripted_areatrigger VALUES
(5604,'at_icecrown_citadel'),
(5611,'at_lights_hammer'),
(5612,'at_lights_hammer'),
(5647,'at_putricides_trap'),
(5709,'at_icecrown_citadel'),
(5718,'at_frozen_throne_tele'),
(5732,'at_icecrown_citadel');

/* FORGE OF SOULS */
UPDATE creature_template SET ScriptName='boss_bronjahm' WHERE entry=36497;
UPDATE creature_template SET ScriptName='npc_corrupted_soul_fragment' WHERE entry=36535;
UPDATE creature_template SET ScriptName='boss_devourer_of_souls' WHERE entry=36502;
UPDATE instance_template SET ScriptName='instance_forge_of_souls' WHERE map=632;

/* HALLS OF REFLECTION */
UPDATE instance_template SET ScriptName='instance_halls_of_reflection' WHERE map=668;
UPDATE creature_template SET ScriptName='npc_spell_aura_dummy_frostmourne_equip' WHERE entry=37226;
UPDATE creature_template SET ScriptName='npc_phantom_mage' WHERE entry=38172;
UPDATE creature_template SET ScriptName='npc_spell_summon_reflections' WHERE entry=36723;
UPDATE creature_template SET ScriptName='boss_lich_king_hor' WHERE entry=36954;
UPDATE creature_template SET ScriptName='npc_jaina_sylvanas_hor' WHERE entry IN (36955,37554);
DELETE FROM scripted_areatrigger WHERE entry IN (5697,5740,5605,5660);
INSERT INTO scripted_areatrigger VALUES
(5697,'at_frostmourne_chamber'),
(5740,'at_frostworn_general'),
(5605,'at_wrath_lich_king'),
(5660,'at_queldelar_start');
DELETE FROM scripted_event_id WHERE id=22795;
INSERT INTO scripted_event_id VALUES
(22795,'event_spell_summon_ice_wall');

/* PIT OF SARON */
UPDATE instance_template SET ScriptName='instance_pit_of_saron' WHERE map=658;
UPDATE creature_template SET ScriptName='boss_forgemaster_garfrost' WHERE entry=36494;
UPDATE creature_template SET ScriptName='boss_krick' WHERE entry=36477;
UPDATE creature_template SET ScriptName='boss_ick' WHERE entry=36476;
UPDATE creature_template SET ScriptName='npc_exploding_orb' WHERE entry=36610;
UPDATE creature_template SET ScriptName='npc_ymirjar_deathbringer' WHERE entry=36892;
UPDATE creature_template SET ScriptName='npc_collapsing_icicle' WHERE entry=36847;
UPDATE creature_template SET ScriptName='boss_tyrannus' WHERE entry=36658;
UPDATE creature_template SET ScriptName='boss_rimefang_pos' WHERE entry=36661;
DELETE FROM scripted_areatrigger WHERE entry IN (5578,5581);
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
UPDATE creature_template SET ScriptName='npc_converted_sentry' WHERE entry=24981;

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
UPDATE creature_template SET ScriptName='mob_demon_chain' WHERE entry=17248;
UPDATE creature_template SET ScriptName='npc_fiendish_portal' WHERE entry=17265;
UPDATE creature_template SET ScriptName='npc_shade_of_aran_blizzard' WHERE entry=17161;
UPDATE creature_template SET ScriptName='npc_netherspite_portal' WHERE entry IN (17367,17368,17369);
UPDATE creature_template SET ScriptName='npc_infernal_target' WHERE entry=17644;
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
DELETE FROM scripted_event_id WHERE id IN (10591,10951);
INSERT INTO scripted_event_id VALUES
(10591,'event_spell_summon_nightbane'),
(10951,'event_spell_medivh_journal');

/* LOCH MODAN */
UPDATE creature_template SET ScriptName='npc_mountaineer_pebblebitty' WHERE entry=3836;
UPDATE creature_template SET ScriptName='npc_miran' WHERE entry=1379;

/* MAGISTER'S TERRACE */
UPDATE instance_template SET ScriptName='instance_magisters_terrace' WHERE map=585;
UPDATE creature_template SET ScriptName='boss_selin_fireheart' WHERE entry=24723;
UPDATE creature_template SET ScriptName='mob_fel_crystal' WHERE entry=24722;
UPDATE creature_template SET ScriptName='boss_vexallus' WHERE entry=24744;
UPDATE creature_template SET ScriptName='mob_pure_energy' WHERE entry=24745;
UPDATE creature_template SET ScriptName='boss_priestess_delrissa' WHERE entry=24560;
UPDATE creature_template SET ScriptName='npc_kagani_nightstrike' WHERE entry=24557;
UPDATE creature_template SET ScriptName='npc_ellris_duskhallow' WHERE entry=24558;
UPDATE creature_template SET ScriptName='npc_eramas_brightblaze' WHERE entry=24554;
UPDATE creature_template SET ScriptName='npc_yazzai' WHERE entry=24561;
UPDATE creature_template SET ScriptName='npc_warlord_salaris' WHERE entry=24559;
UPDATE creature_template SET ScriptName='npc_garaxxas' WHERE entry=24555;
UPDATE creature_template SET ScriptName='npc_apoko' WHERE entry=24553;
UPDATE creature_template SET ScriptName='npc_zelfan' WHERE entry=24556;
UPDATE creature_template SET ScriptName='boss_felblood_kaelthas' WHERE entry=24664;
UPDATE creature_template SET ScriptName='mob_arcane_sphere' WHERE entry=24708;
UPDATE creature_template SET ScriptName='mob_felkael_phoenix' WHERE entry=24674;
UPDATE creature_template SET ScriptName='mob_felkael_phoenix_egg' WHERE entry=24675;
UPDATE creature_template SET ScriptName='npc_kalecgos' WHERE entry=24844;
DELETE FROM scripted_event_id WHERE id=16547;
INSERT INTO scripted_event_id VALUES
(16547,'event_go_scrying_orb');

/* MARAUDON */
UPDATE creature_template SET ScriptName='boss_noxxion' WHERE entry=13282;

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

/* MULGORE */
UPDATE creature_template SET ScriptName='npc_kyle_the_frenzied' WHERE entry=23616;

/* NAGRAND */
UPDATE creature_template SET ScriptName='mob_lump' WHERE entry=18351;
UPDATE creature_template SET ScriptName='npc_nagrand_captive' WHERE entry IN (18209,18210);
UPDATE creature_template SET ScriptName='npc_creditmarker_visit_with_ancestors' WHERE entry IN (18840,18841,18842,18843);
UPDATE creature_template SET ScriptName='npc_rethhedron' WHERE entry=22357;

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
DELETE FROM scripted_areatrigger WHERE entry=4497;
INSERT INTO scripted_areatrigger VALUES (4497,'at_commander_dawnforge');
UPDATE gameobject_template SET ScriptName='go_manaforge_control_console' WHERE entry IN (183770,183956,184311,184312);
UPDATE creature_template SET ScriptName='npc_manaforge_control_console' WHERE entry IN (20209,20417,20418,20440);
UPDATE creature_template SET ScriptName='npc_commander_dawnforge' WHERE entry=19831;
UPDATE creature_template SET ScriptName='npc_bessy' WHERE entry=20415;
UPDATE creature_template SET ScriptName='npc_maxx_a_million' WHERE entry=19589;
UPDATE creature_template SET ScriptName='npc_zeppit' WHERE entry=22484;
UPDATE creature_template SET ScriptName='npc_protectorate_demolitionist' WHERE entry=20802;
UPDATE creature_template SET ScriptName='npc_captured_vanguard' WHERE entry=20763;
UPDATE creature_template SET ScriptName='npc_drijya' WHERE entry=20281;
UPDATE creature_template SET ScriptName='npc_dimensius' WHERE entry=19554;
UPDATE creature_template SET ScriptName='npc_salhadaar' WHERE entry=20454;
UPDATE creature_template SET ScriptName='npc_energy_ball' WHERE entry=20769;

/*  */
/* THE NEXUS */
/*  */

/* EYE OF ETERNITY */
UPDATE instance_template SET ScriptName='instance_eye_of_eternity' WHERE map=616;
UPDATE creature_template SET ScriptName='boss_malygos' WHERE entry=28859;
UPDATE creature_template SET ScriptName='npc_power_spark' WHERE entry=30084;
UPDATE creature_template SET ScriptName='npc_wyrmrest_skytalon' WHERE entry=30161;
DELETE FROM scripted_event_id WHERE id=20711;
INSERT INTO scripted_event_id VALUES
(20711,'event_go_focusing_iris');

/* NEXUS */
UPDATE creature_template SET ScriptName='boss_anomalus' WHERE entry=26763;
UPDATE creature_template SET ScriptName='mob_chaotic_rift' WHERE entry=26918;
UPDATE creature_template SET ScriptName='boss_keristrasza' WHERE entry=26723;
UPDATE creature_template SET ScriptName='boss_ormorok' WHERE entry=26794;
UPDATE creature_template SET ScriptName='npc_crystal_spike_trigger' WHERE entry IN (27101, 27079);
UPDATE creature_template SET ScriptName='boss_telestra' WHERE entry=26731;
UPDATE gameobject_template SET ScriptName='go_containment_sphere' WHERE entry IN (188526, 188527, 188528);
UPDATE instance_template SET ScriptName='instance_nexus' WHERE map=576;

/* OCULUS */
UPDATE instance_template SET ScriptName='instance_oculus' WHERE map=578;
UPDATE creature_template SET ScriptName='boss_eregos' WHERE entry=27656;
UPDATE creature_template SET ScriptName='boss_urom' WHERE entry=27655;
UPDATE creature_template SET ScriptName='boss_varos' WHERE entry=27447;
UPDATE creature_template SET ScriptName='npc_azure_ring_captain' WHERE entry=28236;
UPDATE creature_template SET ScriptName='npc_arcane_beam' WHERE entry=28239;
UPDATE creature_template SET ScriptName='npc_centrifuge_core' WHERE entry=28183;
UPDATE creature_template SET ScriptName='npc_planar_anomaly' WHERE entry=30879;
UPDATE creature_template SET ScriptName='npc_oculus_drake' WHERE entry IN (27756, 27692, 27755);
DELETE FROM scripted_event_id WHERE id IN (10665,12229,18454,18455);
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
UPDATE creature_template SET ScriptName='npc_shenthul' WHERE entry=3401;
UPDATE creature_template SET ScriptName='npc_thrall_warchief' WHERE entry=4949;

/* RAGEFIRE CHASM */

/* RAZORFEN DOWNS */
UPDATE instance_template SET ScriptName='instance_razorfen_downs' WHERE map=129;
UPDATE creature_template SET ScriptName='npc_belnistrasz' WHERE entry=8516;
DELETE FROM scripted_event_id WHERE id=3130;
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
UPDATE creature_template SET ScriptName='npc_baltharus_clone' WHERE entry=39899;
UPDATE creature_template SET ScriptName='boss_halion_real' WHERE entry=39863;
UPDATE creature_template SET ScriptName='boss_halion_twilight' WHERE entry=40142;

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
DELETE FROM scripted_event_id WHERE id IN (5618, 5619, 5620, 5621, 5622, 5623);
INSERT INTO scripted_event_id VALUES
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
UPDATE creature_template SET ScriptName='npc_arugal' WHERE entry=10000;
UPDATE creature_template SET ScriptName='npc_deathstalker_vincent' WHERE entry=4444;
UPDATE creature_template SET ScriptName='mob_arugal_voidwalker' WHERE entry=4627;
UPDATE creature_template SET ScriptName='boss_arugal' WHERE entry=4275;
UPDATE creature_template SET ScriptName='npc_apothecary_hummel' WHERE entry=36296;
UPDATE creature_template SET ScriptName='npc_valentine_boss_manager' WHERE entry=36643;

/* SHADOWMOON VALLEY */
UPDATE creature_template SET ScriptName='boss_doomwalker' WHERE entry=17711;
UPDATE creature_template SET ScriptName='npc_dragonmaw_peon' WHERE entry=22252;
UPDATE creature_template SET ScriptName='mob_mature_netherwing_drake' WHERE entry=21648;
UPDATE creature_template SET ScriptName='mob_enslaved_netherwing_drake' WHERE entry=21722;
UPDATE creature_template SET ScriptName='npc_wilda' WHERE entry=21027;
UPDATE creature_template SET ScriptName='mob_torloth' WHERE entry=22076;
UPDATE creature_template SET ScriptName='npc_totem_of_spirits' WHERE entry=21071;
DELETE FROM scripted_event_id WHERE id IN (13513,13514,13515,13516);
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

/* SHATTRATH */
UPDATE creature_template SET ScriptName='npc_dirty_larry' WHERE entry=19720;
UPDATE creature_template SET ScriptName='npc_khadgars_servant' WHERE entry=19685;
UPDATE creature_template SET ScriptName='npc_salsalabim' WHERE entry=18584;

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

/* SILVERMOON */

/* SILVERPINE FOREST */
UPDATE creature_template SET ScriptName='npc_deathstalker_erland' WHERE entry=1978;
UPDATE creature_template SET ScriptName='npc_deathstalker_faerleia' WHERE entry=2058;

/* STOCKADES */

/* STONETALON MOUNTAINS */
UPDATE creature_template SET ScriptName='npc_kaya' WHERE entry=11856;

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

/* STRANGLETHORN VALE */
UPDATE creature_template SET ScriptName='mob_yenniku' WHERE entry=2530;

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
DELETE FROM scripted_areatrigger WHERE entry=4016;
INSERT INTO scripted_areatrigger VALUES (4016,'at_shade_of_eranikus');
UPDATE creature_template SET ScriptName='npc_malfurion_stormrage' WHERE entry=15362;
DELETE FROM scripted_event_id WHERE id IN (3094,3095,3097,3098,3099,3100);
INSERT INTO scripted_event_id VALUES
(3094,'event_antalarion_statue_activation'),
(3095,'event_antalarion_statue_activation'),
(3097,'event_antalarion_statue_activation'),
(3098,'event_antalarion_statue_activation'),
(3099,'event_antalarion_statue_activation'),
(3100,'event_antalarion_statue_activation');
UPDATE creature_template SET ScriptName='npc_shade_of_hakkar' WHERE entry=8440;
UPDATE gameobject_template SET ScriptName='go_eternal_flame' WHERE entry IN (148418,148419,148420,148421);
DELETE FROM scripted_event_id WHERE id=8502;
INSERT INTO scripted_event_id VALUES
(8502,'event_avatar_of_hakkar');

/* SUNWELL PLATEAU */
UPDATE instance_template SET ScriptName='instance_sunwell_plateau' WHERE map=580;
UPDATE creature_template SET ScriptName='boss_brutallus' WHERE entry=24882;
UPDATE creature_template SET ScriptName='boss_kalecgos' WHERE entry=24850;
UPDATE creature_template SET ScriptName='boss_kalecgos_humanoid' WHERE entry=24891;
UPDATE creature_template SET ScriptName='boss_sathrovarr' WHERE entry=24892;
DELETE FROM scripted_areatrigger WHERE entry=4853;
INSERT INTO scripted_areatrigger VALUES (4853,'at_madrigosa');
UPDATE creature_template SET ScriptName='boss_alythess' WHERE entry=25166;
UPDATE creature_template SET ScriptName='boss_sacrolash' WHERE entry=25165;
UPDATE creature_template SET ScriptName='npc_shadow_image' WHERE entry=25214;
UPDATE creature_template SET ScriptName='boss_muru' WHERE entry=25741;
UPDATE creature_template SET ScriptName='boss_entropius' WHERE entry=25840;
UPDATE creature_template SET ScriptName='npc_portal_target' WHERE entry=25770;
UPDATE creature_template SET ScriptName='boss_kiljaeden' WHERE entry=25315;
UPDATE creature_template SET ScriptName='npc_kiljaeden_controller' WHERE entry=25608;
UPDATE creature_template SET ScriptName='spell_dummy_npc_brutallus_cloud' WHERE entry=25703;
UPDATE creature_template SET ScriptName='boss_felmyst' WHERE entry=25038;
UPDATE creature_template SET ScriptName='npc_shield_orb' WHERE entry=25502;
UPDATE creature_template SET ScriptName='npc_power_blue_flight' WHERE entry=25653;
UPDATE creature_template SET ScriptName='npc_demonic_vapor' WHERE entry=25265;
UPDATE creature_template SET ScriptName='npc_darkness' WHERE entry=25879;
UPDATE creature_template SET ScriptName='npc_singularity' WHERE entry=25855;

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
UPDATE creature_template SET ScriptName='boss_nethermancer_sepethrea' WHERE entry=19221;
UPDATE creature_template SET ScriptName='boss_pathaleon_the_calculator' WHERE entry=19220;
UPDATE creature_template SET ScriptName='mob_nether_wraith' WHERE entry=21062;
UPDATE instance_template SET ScriptName='instance_mechanar' WHERE map=554;

/* THE BOTANICA */
UPDATE creature_template SET ScriptName='boss_high_botanist_freywinn' WHERE entry=17975;
UPDATE creature_template SET ScriptName='boss_laj' WHERE entry=17980;
UPDATE creature_template SET ScriptName='boss_warp_splinter' WHERE entry=17977;
UPDATE creature_template SET ScriptName='mob_warp_splinter_treant' WHERE entry=19949;

/* THE ARCATRAZ */
UPDATE instance_template SET ScriptName='instance_arcatraz' WHERE map=552;
UPDATE creature_template SET ScriptName='boss_harbinger_skyriss' WHERE entry=20912;
UPDATE creature_template SET ScriptName='boss_dalliah' WHERE entry=20885;
UPDATE creature_template SET ScriptName='boss_soccothrates' WHERE entry=20886;
UPDATE creature_template SET ScriptName='npc_warden_mellichar' WHERE entry=20904;
UPDATE creature_template SET ScriptName='npc_millhouse_manastorm' WHERE entry=20977;

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
UPDATE creature_template SET ScriptName='mob_phoenix_tk' WHERE entry=21362;
UPDATE creature_template SET ScriptName='mob_phoenix_egg_tk' WHERE entry=21364;

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
UPDATE creature_template SET ScriptName='npc_glob_of_viscidus' WHERE entry=15667;
UPDATE creature_template SET ScriptName='boss_huhuran' WHERE entry=15509;
UPDATE creature_template SET ScriptName='boss_veklor' WHERE entry=15276;
UPDATE creature_template SET ScriptName='boss_veknilash' WHERE entry=15275;
UPDATE creature_template SET ScriptName='boss_ouro' WHERE entry=15517;
UPDATE creature_template SET ScriptName='npc_ouro_spawner' WHERE entry=15957;
UPDATE creature_template SET ScriptName='boss_eye_of_cthun' WHERE entry=15589;
UPDATE creature_template SET ScriptName='mob_sartura_royal_guard' WHERE entry=15984;
UPDATE creature_template SET ScriptName='mob_giant_claw_tentacle' WHERE entry=15728;
UPDATE creature_template SET ScriptName='mob_anubisath_sentinel' WHERE entry=15264;
DELETE FROM scripted_areatrigger WHERE entry IN (4033,4034);
INSERT INTO scripted_areatrigger VALUES
(4033,'at_stomach_cthun'),
(4034,'at_stomach_cthun');

/* TEROKKAR FOREST */
UPDATE creature_template SET ScriptName='mob_netherweb_victim' WHERE entry=22355;
UPDATE creature_template SET ScriptName='mob_unkor_the_ruthless' WHERE entry=18262;
UPDATE creature_template SET ScriptName='npc_akuno' WHERE entry=22377;
UPDATE creature_template SET ScriptName='npc_hungry_nether_ray' WHERE entry=23439;
UPDATE creature_template SET ScriptName='npc_letoll' WHERE entry=22458;
UPDATE creature_template SET ScriptName='npc_mana_bomb_exp_trigger' WHERE entry=20767;
UPDATE gameobject_template SET ScriptName='go_mana_bomb' WHERE entry=184725;
UPDATE creature_template SET ScriptName='npc_captive_child' WHERE entry=22314;
UPDATE creature_template SET ScriptName='npc_isla_starmane' WHERE entry=18760;
UPDATE creature_template SET ScriptName="npc_skywing" WHERE entry=22424;
UPDATE creature_template SET ScriptName="npc_cenarion_sparrowhawk" WHERE entry=22972;
UPDATE creature_template SET ScriptName="npc_skyguard_prisoner" WHERE entry=23383;
UPDATE creature_template SET ScriptName='npc_avatar_of_terokk' WHERE entry=22375;
UPDATE creature_template SET ScriptName='npc_minion_of_terokk' WHERE entry=22376;

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
DELETE FROM scripted_event_id WHERE id IN (2228,2268);
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
UPDATE creature_template SET ScriptName='boss_maiden_of_grief' WHERE entry=27975;
UPDATE creature_template SET ScriptName='boss_sjonnir' WHERE entry=27978;
UPDATE creature_template SET ScriptName='npc_brann_hos' WHERE entry=28070;
UPDATE creature_template SET ScriptName='npc_dark_matter' WHERE entry=28235;
UPDATE creature_template SET ScriptName='npc_searing_gaze' WHERE entry=28265;

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
UPDATE creature_template SET ScriptName='npc_laughing_skull' WHERE entry=33990;
UPDATE creature_template SET ScriptName='npc_keeper_mimiron' WHERE entry=33412;
UPDATE creature_template SET ScriptName='npc_keeper_thorim' WHERE entry=33413;
DELETE FROM scripted_event_id WHERE id IN (9735,20907,20964,21030,21031,21032,21033,21045,21605,21606,21620);
INSERT INTO scripted_event_id VALUES
(9735, 'event_spell_saronite_barrier'), -- Vezax saronite barrier event
(20907,'event_boss_hodir'),             -- Hodir shatter chest event
(20964,'event_spell_harpoon_shot'),     -- Razorscale harpoon event
(21030,'event_go_ulduar_tower'),        -- Tower of Life destroyed event
(21031,'event_go_ulduar_tower'),        -- Tower of Storms destroyed event
(21032,'event_go_ulduar_tower'),        -- Tower of Frost destroyed event
(21033,'event_go_ulduar_tower'),        -- Tower of Flame destroyed event
(21045,'event_boss_hodir'),             -- Hodir attack start event
(21605,'event_ulduar'),                 -- Flame Leviathan shutdown event
(21606,'event_ulduar'),                 -- XT-002 Scrap repair event
(21620,'event_ulduar');                 -- Ignis construct shatter event

/* UN'GORO CRATER */
UPDATE creature_template SET ScriptName='npc_ame01' WHERE entry=9623;
UPDATE creature_template SET ScriptName='npc_ringo' WHERE entry=9999;

/* UNDERCITY */
UPDATE creature_template SET ScriptName='npc_lady_sylvanas_windrunner' WHERE entry=10181;

/*  */
/* UTGARDE KEEP */
/*  */

/* UTGARDE KEEP */
UPDATE instance_template SET ScriptName='instance_utgarde_keep' WHERE map=574;
UPDATE creature_template SET ScriptName='mob_dragonflayer_forge_master' WHERE entry=24079;
UPDATE creature_template SET ScriptName='boss_skarvald' WHERE entry=24200;
UPDATE creature_template SET ScriptName='boss_dalronn' WHERE entry=24201;
UPDATE creature_template SET ScriptName='boss_ingvar' WHERE entry=23954;
UPDATE creature_template SET ScriptName='npc_annhylde' WHERE entry=24068;
UPDATE creature_template SET ScriptName='boss_keleseth' WHERE entry=23953;
UPDATE creature_template SET ScriptName='mob_vrykul_skeleton' WHERE entry=23970;

/* UTGARDE PINNACLE */
UPDATE creature_template SET ScriptName='boss_gortok' WHERE entry=26687;
UPDATE creature_template SET ScriptName='npc_gortok_subboss' WHERE entry IN (26683,26684,26685,26686);
UPDATE creature_template SET ScriptName='boss_skadi' WHERE entry=26693;
UPDATE creature_template SET ScriptName='npc_grauf' WHERE entry=26893;
UPDATE creature_template SET ScriptName='npc_flame_breath_trigger' WHERE entry=28351;
UPDATE creature_template SET ScriptName='boss_svala' WHERE entry=29281;
UPDATE creature_template SET ScriptName='boss_ymiron' WHERE entry=26861;
UPDATE instance_template SET ScriptName='instance_pinnacle' WHERE map=575;
DELETE FROM scripted_areatrigger WHERE entry IN (4991,5140);
INSERT INTO scripted_areatrigger VALUES
(4991,'at_skadi'),
(5140,'at_svala_intro');
DELETE FROM scripted_event_id WHERE id IN (17728,20651);
INSERT INTO scripted_event_id VALUES
(17728,'event_spell_gortok_event'),
(20651,'event_achiev_kings_bane');

/* VAULT OF ARCHAVON */

/* VIOLET HOLD */
UPDATE instance_template SET ScriptName='instance_violet_hold' WHERE map=608;
UPDATE gameobject_template SET ScriptName='go_activation_crystal' WHERE entry=193611;
UPDATE creature_template SET ScriptName='npc_door_seal' WHERE entry=30896;
UPDATE creature_template SET ScriptName='npc_sinclari' WHERE entry=30658;
UPDATE creature_template SET ScriptName='npc_prison_event_controller' WHERE entry=30883;
UPDATE creature_template SET ScriptName='npc_teleportation_portal' WHERE entry IN (31011,30679,32174);
UPDATE creature_template SET ScriptName='boss_ichoron' WHERE entry IN (29313,32234);
UPDATE creature_template SET ScriptName='boss_erekem' WHERE entry IN (29315,32226);
UPDATE creature_template SET ScriptName='npc_erekem_guard' WHERE entry IN (29395,32228);

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

/* WETLANDS */
UPDATE creature_template SET ScriptName='npc_tapoke_slim_jahn' WHERE entry=4962;
UPDATE creature_template SET ScriptName='npc_mikhail' WHERE entry=4963;

/* WINTERSPRING */
UPDATE creature_template SET ScriptName='npc_ranshalla' WHERE entry=10300;
UPDATE gameobject_template SET ScriptName='go_elune_fire' WHERE entry IN (177417, 177404);

/* ZANGARMARSH */
DELETE FROM scripted_event_id WHERE id=11225;
INSERT INTO scripted_event_id VALUES (11225,'event_taxi_stormcrow');
UPDATE creature_template SET ScriptName='npc_cooshcoosh' WHERE entry=18586;
UPDATE creature_template SET ScriptName='npc_kayra_longmane' WHERE entry=17969;
UPDATE creature_template SET ScriptName='npc_fhwoor' WHERE entry=17877;

/* ZUL'AMAN */
UPDATE instance_template SET ScriptName='instance_zulaman' WHERE map=568;
UPDATE creature_template SET ScriptName='npc_harrison_jones_za' WHERE entry=24358;
UPDATE gameobject_template SET ScriptName='go_strange_gong' WHERE entry=187359;
UPDATE creature_template SET ScriptName='boss_akilzon' WHERE entry=23574;
UPDATE creature_template SET ScriptName='mob_soaring_eagle' WHERE entry=24858;
UPDATE creature_template SET ScriptName='boss_halazzi' WHERE entry=23577;
UPDATE creature_template SET ScriptName='boss_spirit_lynx' WHERE entry=24143;
UPDATE creature_template SET ScriptName='boss_janalai' WHERE entry=23578;
UPDATE creature_template SET ScriptName='boss_malacrass' WHERE entry=24239;
UPDATE creature_template SET ScriptName='boss_nalorakk' WHERE entry=23576;
UPDATE creature_template SET ScriptName='boss_zuljin' WHERE entry=23863;
UPDATE creature_template SET ScriptName='npc_feather_vortex' WHERE entry=24136;
UPDATE creature_template SET ScriptName='npc_dragonhawk_egg' WHERE entry=23817;
UPDATE creature_template SET ScriptName='npc_janalai_firebomb' WHERE entry=23920;
UPDATE creature_template SET ScriptName='npc_amanishi_hatcher' WHERE entry IN (23818,24504);
UPDATE creature_template SET ScriptName='npc_forest_frog' WHERE entry=24396;

/* ZUL'DRAK */
UPDATE creature_template SET ScriptName='npc_gurgthock' WHERE entry=30007;
UPDATE creature_template SET ScriptName='npc_ghoul_feeding_bunny' WHERE entry=28591;
UPDATE creature_template SET ScriptName='npc_decaying_ghoul' WHERE entry=28565;


/* ZUL'FARRAK */
UPDATE instance_template SET ScriptName='instance_zulfarrak' WHERE map=209;
DELETE FROM scripted_event_id WHERE id IN (2488,2609);
INSERT INTO scripted_event_id VALUES
(2488,'event_go_zulfarrak_gong'),
(2609,'event_spell_unlocking');
DELETE FROM scripted_areatrigger WHERE entry=1447;
INSERT INTO scripted_areatrigger VALUES (1447,'at_zulfarrak');
UPDATE creature_template SET ScriptName='boss_zumrah' WHERE entry=7271;

/* ZUL'GURUB */
UPDATE instance_template SET ScriptName='instance_zulgurub' WHERE map=309;
UPDATE creature_template SET ScriptName='boss_jeklik' WHERE entry=14517;
UPDATE creature_template SET ScriptName='boss_venoxis' WHERE entry=14507;
UPDATE creature_template SET ScriptName='boss_marli' WHERE entry=14510;
UPDATE creature_template SET ScriptName='boss_mandokir' WHERE entry=11382;
UPDATE creature_template SET ScriptName='mob_ohgan' WHERE entry=14988;
UPDATE creature_template SET ScriptName='boss_jindo' WHERE entry=11380;
UPDATE creature_template SET ScriptName='boss_hakkar' WHERE entry=14834;
UPDATE creature_template SET ScriptName='boss_thekal' WHERE entry=14509;
UPDATE creature_template SET ScriptName='boss_arlokk' WHERE entry=14515;
UPDATE gameobject_template SET ScriptName='go_gong_of_bethekk' WHERE entry=180526;
UPDATE creature_template SET ScriptName='boss_hazzarah' WHERE entry=15083;
UPDATE creature_template SET ScriptName='boss_renataki' WHERE entry=15084;
UPDATE creature_template SET ScriptName='mob_zealot_lorkhan' WHERE entry=11347;
UPDATE creature_template SET ScriptName='mob_zealot_zath' WHERE entry=11348;
UPDATE creature_template SET ScriptName='mob_healing_ward' WHERE entry=14987;
UPDATE creature_template SET ScriptName='npc_gurubashi_bat_rider' WHERE entry=14750;



--
-- Below contains data for table `script_texts` mainly used in C++ parts.
-- valid entries for table are between -1000000 and -1999999
--

TRUNCATE script_texts;

--
-- -1 000 000 First 100 entries are reserved for special use, do not add regular text here.
--

INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1000000,'<ScriptDev2 Text Entry Missing!>',0,0,0,0,'DEFAULT_TEXT'),
(-1000001,'%s goes into a killing frenzy!',0,2,0,0,'EMOTE_GENERIC_FRENZY_KILL'),
(-1000002,'%s goes into a frenzy!',0,2,0,0,'EMOTE_GENERIC_FRENZY'),
(-1000003,'%s becomes enraged!',0,2,0,0,'EMOTE_GENERIC_ENRAGED'),
(-1000004,'%s goes into a berserker rage!',0,2,0,0,'EMOTE_GENERIC_BERSERK'),
(-1000005,'%s goes into a frenzy!',0,3,0,0,'EMOTE_BOSS_GENERIC_FRENZY'),
(-1000006,'%s becomes enraged!',0,3,0,0,'EMOTE_BOSS_GENERIC_ENRAGED');

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
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1000100,'Come, little ones. Face me!',0,1,0,0,'azuregos SAY_TELEPORT'),

(-1000101,'Follow me, $N. I\'ll take you to the Defias hideout. But you better protect me or I am as good as dead',0,0,7,0,'defias traitor SAY_START'),
(-1000102,'The entrance is hidden here in Moonbrook. Keep your eyes peeled for thieves. They want me dead.',0,0,7,0,'defias traitor SAY_PROGRESS'),
(-1000103,'You can go tell Stoutmantle this is where the Defias Gang is holed up, $N.',0,0,7,0,'defias traitor SAY_END'),
(-1000104,'$N coming in fast! Prepare to fight!',0,0,7,0,'defias traitor SAY_AGGRO_1'),
(-1000105,'Help!',0,0,7,0,'defias traitor SAY_AGGRO_2'),

(-1000106,'Everyone ready?',0,0,1,0,'torek SAY_READY'),
(-1000107,'Ok, Lets move out!',0,0,1,0,'torek SAY_MOVE'),
(-1000108,'Prepare yourselves. Silverwing is just around the bend.',0,0,1,0,'torek SAY_PREPARE'),
(-1000109,'Silverwing is ours!',0,0,1,0,'torek SAY_WIN'),
(-1000110,'Go report that the outpost is taken. We will remain here.',0,0,1,0,'torek SAY_END'),

(-1000111,'Our house is this way, through the thicket.',0,0,7,1,'magwin SAY_START'),
(-1000112,'Help me!',0,0,7,0,'magwin SAY_AGGRO'),
(-1000113,'My poor family. Everything has been destroyed.',0,0,7,5,'magwin SAY_PROGRESS'),
(-1000114,'Father! Father! You\'re alive!',0,0,7,22,'magwin SAY_END1'),
(-1000115,'You can thank $N for getting me back here safely, father.',0,0,7,1,'magwin SAY_END2'),
(-1000116,'%s hugs her father.',0,2,7,5,'magwin EMOTE_HUG'),

(-1000117,'Thank you for agreeing to help. Now, let\'s get out of here $N.',0,0,1,0,'wounded elf SAY_ELF_START'),
(-1000118,'Over there! They\'re following us!',0,0,1,0,'wounded elf SAY_ELF_SUMMON1'),
(-1000119,'Allow me a moment to rest. The journey taxes what little strength I have.',0,0,1,16,'wounded elf SAY_ELF_RESTING'),
(-1000120,'Did you hear something?',0,0,1,0,'wounded elf SAY_ELF_SUMMON2'),
(-1000121,'Falcon Watch, at last! Now, where\'s my... Oh no! My pack, it\'s missing! Where has -',0,0,1,0,'wounded elf SAY_ELF_COMPLETE'),
(-1000122,'You won\'t keep me from getting to Falcon Watch!',0,0,1,0,'wounded elf SAY_ELF_AGGRO'),

(-1000123,'Ready when you are, $c.',0,0,0,15,'big will SAY_BIG_WILL_READY'),
(-1000124,'The Affray has begun. $n, get ready to fight!',0,0,0,0,'twiggy SAY_TWIGGY_BEGIN'),
(-1000125,'You! Enter the fray!',0,0,0,0,'twiggy SAY_TWIGGY_FRAY'),
(-1000126,'Challenger is down!',0,0,0,0,'twiggy SAY_TWIGGY_DOWN'),
(-1000127,'The Affray is over.',0,0,0,0,'twiggy SAY_TWIGGY_OVER'),

(-1000128,'We need you to send reinforcements to Manaforge Duro, Ardonis. This is not a request, it\'s an order.',0,0,0,0,'dawnforge SAY_COMMANDER_DAWNFORGE_1'),
(-1000129,'You cannot be serious! We are severely understaffed and can barely keep this manaforge functional!',0,0,0,0,'dawnforge SAY_ARCANIST_ARDONIS_1'),
(-1000130,'You will do as ordered. Manaforge Duro has come under heavy attack by mana creatures and the situation is out of control. Failure to comply will not be tolerated!',0,0,0,0,'dawnforge SAY_COMMANDER_DAWNFORGE_2'),
(-1000131,'Indeed, it is not a request.',0,0,0,0,'dawnforge SAY_PATHALEON_CULATOR_IMAGE_1'),
(-1000132,'My lord!',0,0,0,0,'dawnforge SAY_COMMANDER_DAWNFORGE_3'),
(-1000133,'Duro will be reinforced! Ultris was a complete disaster. I will NOT have that mistake repeated!',0,0,0,0,'dawnforge PATHALEON_CULATOR_IMAGE_2'),
(-1000134,'We\'ve had too many setbacks along the way: Hellfire Citadel, Fallen Sky Ridge, Firewing Point... Prince Kael\'thas will tolerate no further delays. I will tolerate nothing other than complete success!',0,0,0,0,'dawnforge PATHALEON_CULATOR_IMAGE_2_1'),
(-1000135,'I am returning to Tempest Keep. See to it that I do not have reason to return!',0,0,0,0,'dawnforge PATHALEON_CULATOR_IMAGE_2_2' ),
(-1000136,'Yes, my lord.',0,0,0,0,'dawnforge COMMANDER_DAWNFORGE_4 SAY_ARCANIST_ARDONIS_2'),
(-1000137,'See to it, Ardonis!',0,0,0,0,'dawnforge COMMANDER_DAWNFORGE_5'),

(-1000138,'Prepare to enter oblivion, meddlers. You have unleashed a god!',0,1,0,15,'Salhadaar SAY'),
(-1000139,'Avruu\'s magic is broken! I\'m free once again!',0,0,0,0,'aeranas SAY_FREE'),

(-1000140,'Let\'s go.',0,0,1,0,'lilatha SAY_START'),
(-1000141,'$N, let\'s use the antechamber to the right.',0,0,1,0,'lilatha SAY_PROGRESS1'),
(-1000142,'I can see the light at the end of the tunnel!',0,0,1,0,'lilatha SAY_PROGRESS2'),
(-1000143,'There\'s Farstrider Enclave now, $C. Not far to go... Look out! Troll ambush!!',0,0,1,0,'lilatha SAY_PROGRESS3'),
(-1000144,'Thank you for saving my life and bringing me back to safety, $N',0,0,1,0,'lilatha SAY_END1'),
(-1000145,'Captain Helios, I\'ve been rescued from the Amani Catacombs. Reporting for duty, sir!',0,0,1,0,'lilatha SAY_END2'),
(-1000146,'Liatha, get someone to look at those injuries. Thank you for bringing her back safely.',0,0,1,0,'lilatha CAPTAIN_ANSWER'),

(-1000147,'I remember well the sting of defeat at the conclusion of the Third War. I have waited far too long for my revenge. Now the shadow of the Legion falls over this world. It is only a matter of time until all of your failed creation... is undone.',11332,1,0,0,'kazzak SAY_INTRO'),
(-1000148,'The Legion will conquer all!',11333,1,0,0,'kazzak SAY_AGGRO1'),
(-1000149,'All mortals will perish!',11334,1,0,0,'kazzak SAY_AGGRO2'),
(-1000150,'All life must be eradicated!',11335,1,0,0,'kazzak SAY_SURPREME1'),
(-1000151,'I\'ll rip the flesh from your bones!',11336,1,0,0,'kazzak SAY_SURPREME2'),
(-1000152,'Kirel Narak!',11337,1,0,0,'kazzak SAY_KILL1'),
(-1000153,'Contemptible wretch!',11338,1,0,0,'kazzak SAY_KILL2'),
(-1000154,'The universe will be remade.',11339,1,0,0,'kazzak SAY_KILL3'),
(-1000155,'The Legion... will never... fall.',11340,1,0,0,'kazzak SAY_DEATH'),

(-1000156,'Bloodmaul Brew? Me favorite!',0,0,0,0,'bladespire ogre SAY_BREW_1'),

(-1000157,'Invaders, you dangle upon the precipice of oblivion! The Burning Legion comes and with it comes your end.',0,1,0,0,'kazzak SAY_RAND1'),
(-1000158,'Impudent whelps, you only delay the inevitable. Where one has fallen, ten shall rise. Such is the will of Kazzak...',0,1,0,0,'kazzak SAY_RAND2'),

(-1000159,'Do not proceed. You will be eliminated!',11344,1,0,0,'doomwalker SAY_AGGRO'),
(-1000160,'Tectonic disruption commencing.',11345,1,0,0,'doomwalker SAY_EARTHQUAKE_1'),
(-1000161,'Magnitude set. Release.',11346,1,0,0,'doomwalker SAY_EARTHQUAKE_2'),
(-1000162,'Trajectory locked.',11347,1,0,0,'doomwalker SAY_OVERRUN_1'),
(-1000163,'Engage maximum speed.',11348,1,0,0,'doomwalker SAY_OVERRUN_2'),
(-1000164,'Threat level zero.',11349,1,0,0,'doomwalker SAY_SLAY_1'),
(-1000165,'Directive accomplished.',11350,1,0,0,'doomwalker SAY_SLAY_2'),
(-1000166,'Target exterminated.',11351,1,0,0,'doomwalker SAY_SLAY_3'),
(-1000167,'System failure in five, f-o-u-r...',11352,1,0,0,'doomwalker SAY_DEATH'),

(-1000168,'Who dares awaken Aquementas?',0,1,0,0,'aquementas AGGRO_YELL_AQUE'),

(-1000169,'Muahahahaha! You fool! You\'ve released me from my banishment in the interstices between space and time!',0,1,0,0,'nether_drake SAY_NIHIL_1'),
(-1000170,'All of Draenor shall quick beneath my feet! I will destroy this world and reshape it in my image!',0,1,0,0,'nether_drake SAY_NIHIL_2'),
(-1000171,'Where shall I begin? I cannot bother myself with a worm such as yourself. There is a world to be conquered!',0,1,0,0,'nether_drake SAY_NIHIL_3'),
(-1000172,'No doubt the fools that banished me are long dead. I shall take wing survey my demense. Pray to whatever gods you hold dear that we do not meet again.',0,1,0,0,'nether_drake SAY_NIHIL_4'),
(-1000173,'NOOOOooooooo!',0,1,0,0,'nether_drake SAY_NIHIL_INTERRUPT'),

(-1000174,'Good $N, you are under the spell\'s influence. I must analyze it quickly, then we can talk.',0,0,7,0,'daranelle SAY_SPELL_INFLUENCE'),

(-1000175,'Thank you, mortal.',0,0,11,0,' SAY_JUST_EATEN'),

(-1000176,'What is this? Where am I? How... How did I... Cursed ethereals! I was on an escort mission out of Shattrath when we were ambushed! I must return to the city at once! Your name will be recorded as a friend of the Lower City, $n.',0,0,0,0,'SAY_LC'),
(-1000177,'Blessed Light! Free of my confines at last... Thank you, hero. A\'dal will be told of your heroics.',0,0,0,0,'SAY_SHAT'),
(-1000178,'At last I am released! Thank you, gentle $r. I must return to the expedition at once! They will know of your deeds. This I vow.',0,0,0,0,'SAY_CE'),
(-1000179,'I will tell the tale of your heroics to Haramad myself! All Consortium will know your name, $n. Thank you, my friend.',0,0,0,0,'SAY_CON'),
(-1000180,'The Keepers of Time thank you for your assistance. The Timeless One shall be pleased with this outcome...',0,0,0,0,'SAY_KT'),
(-1000181,'$R save me! Thank you! My people thank you too!',0,0,0,0,'SAY_SPOR'),
(-1000182,'REUSE ME',0,0,0,0,'REUSE_ME'),
(-1000183,'REUSE ME',0,0,0,0,'REUSE_ME'),

(-1000184,'Daughter!',0,0,7,5,'cowlen SAY_DAUGHTER'),

(-1000185,'There\'s no turning back now, $n. Stay close and watch my back.',0,0,0,1,'demolitionist_legoso SAY_ESCORT_START'),
(-1000186,'There it is! Do you see where the large red crystal is jutting out from the Vector Coil? That\'s where I need to plant the first set of explosives.',0,0,0,25,'demolitionist_legoso SAY_ESCORT_1'),

(-1000187,'Thank you, dear $C, you just saved my life.',0,0,0,0,'npc_redemption_target SAY_HEAL'),

(-1000188,'Deployment sucessful. Trespassers will be neutralized.',0,0,0,0,'converted_sentry SAY_CONVERTED_1'),
(-1000189,'Objective acquired. Initiating security routines.',0,0,0,0,'converted_sentry SAY_CONVERTED_2'),

(-1000190,'In Nagrand, food hunt ogre!',0,0,0,0,' SAY_LUMP_0'),
(-1000191,'You taste good with maybe a little salt and pepper.',0,0,0,0,' SAY_LUMP_1'),
(-1000192,'OK, OK! Lump give up!',0,0,0,0,' SAY_LUMP_DEFEAT'),

(-1000193,'%s looks down at the discarded necklace. In her sadness, the lady incants a glamour, which beckons forth Highborne spirits. The chamber resonates with their ancient song about the Sin\'dorei...',10896,2,1,0,'lady_sylvanas EMOTE_LAMENT_START'),

(-1000194,'I give up! Please don\'t kill me!',0,0,0,0,'unkor SAY_SUBMIT'),

(-1000195,'Thank you again, $N. I\'ll make my way to the road now. When you can, find Terenthis and let him know we escaped.',0,0,0,1,'volcor SAY_ESCAPE'),

(-1000196,'Belore...',0,0,1,0,'lady_sylvanas SAY_LAMENT_END'),
(-1000197,'%s kneels down and pick up the amulet.',0,2,1,16,'lady_sylvanas EMOTE_LAMENT_END'),

(-1000198,'Taste blade, mongrel!',0,0,0,0,'SAY_GUARD_SIL_AGGRO1'),
(-1000199,'Please tell me that you didn\'t just do what I think you just did. Please tell me that I\'m not going to have to hurt you...',0,0,0,0,'SAY_GUARD_SIL_AGGRO2'),
(-1000200,'As if we don\'t have enough problems, you go and create more!',0,0,0,0,'SAY_GUARD_SIL_AGGRO3'),

(-1000201,'I\'m saved! Thank you, doctor!',0,0,0,0,'injured_patient SAY_DOC1'),
(-1000202,'HOORAY! I AM SAVED!',0,0,0,0,'injured_patient SAY_DOC2'),
(-1000203,'Sweet, sweet embrace... take me...',0,0,0,0,'injured_patient SAY_DOC3'),

(-1000204,'%s looks up at you quizzically. Maybe you should inspect it?',0,2,0,0,'cluck EMOTE_A_HELLO'),
(-1000205,'%s looks at you unexpectadly.',0,2,0,0,'cluck EMOTE_H_HELLO'),
(-1000206,'%s starts pecking at the feed.',0,2,0,0,'cluck EMOTE_CLUCK_TEXT2'),

(-1000207,'Mmm. Me thirsty!',0,0,0,0,'bladespire ogre SAY_BREW_2'),
(-1000208,'Ohh, look! Bloodmaul Brew! Mmmm...',0,0,0,0,'bladespire ogre SAY_BREW_3'),

(-1000209,'Very well. Let\'s see what you have to show me, $N.',0,0,1,0,'anvilward SAY_ANVIL1'),
(-1000210,'What manner of trick is this, $R? If you seek to ambush me, I warn you I will not go down quietly!',0,0,1,0,'anvilward SAY_ANVIL2'),

(-1000211,'Warning! %s emergency shutdown process initiated by $N. Shutdown will complete in two minutes.',0,2,0,0,'manaforge_control EMOTE_START'),
(-1000212,'Emergency shutdown will complete in one minute.',0,2,0,0,'manaforge_control EMOTE_60'),
(-1000213,'Emergency shutdown will complete in thirty seconds.',0,2,0,0,'manaforge_control EMOTE_30'),
(-1000214,'Emergency shutdown will complete in ten seconds.',0,2,0,0,'manaforge_control EMOTE_10'),
(-1000215,'Emergency shutdown complete.',0,2,0,0,'manaforge_control EMOTE_COMPLETE'),
(-1000216,'Emergency shutdown aborted.',0,2,0,0,'manaforge_control EMOTE_ABORT'),

(-1000217,'Greetings, $N. I will guide you through the cavern. Please try and keep up.',0,4,0,0,'WHISPER_CUSTODIAN_1'),
(-1000218,'We do not know if the Caverns of Time have always been accessible to mortals. Truly, it is impossible to tell as the Timeless One is in perpetual motion, changing our timeways as he sees fit. What you see now may very well not exist tomorrow. You may wake up and have no memory of this place.',0,4,0,0,'WHISPER_CUSTODIAN_2'),
(-1000219,'It is strange, I know... Most mortals cannot actually comprehend what they see here, as often, what they see is not anchored within their own perception of reality.',0,4,0,0,'WHISPER_CUSTODIAN_3'),
(-1000220,'Follow me, please.',0,4,0,0,'WHISPER_CUSTODIAN_4'),
(-1000221,'There are only two truths to be found here: First, that time is chaotic, always in flux, and completely malleable and second, perception does not dictate reality.',0,4,0,0,'WHISPER_CUSTODIAN_5'),
(-1000222,'As custodians of time, we watch over and care for Nozdormu\'s realm. The master is away at the moment, which means that attempts are being made to dramatically alter time. The master never meddles in the affairs of mortals but instead corrects the alterations made to time by others. He is reactionary in this regard.',0,4,0,0,'WHISPER_CUSTODIAN_6'),
(-1000223,'For normal maintenance of time, the Keepers of Time are sufficient caretakers. We are able to deal with most ordinary disturbances. I speak of little things, such as rogue mages changing something in the past to elevate their status or wealth in the present.',0,4,0,0,'WHISPER_CUSTODIAN_7'),
(-1000224,'These tunnels that you see are called timeways. They are infinite in number. The ones that currently exist in your reality are what the master has deemed as \'trouble spots.\' These trouble spots may differ completely in theme but they always share a cause. That is, their existence is a result of the same temporal disturbance. Remember that should you venture inside one...',0,4,0,0,'WHISPER_CUSTODIAN_8'),
(-1000225,'This timeway is in great disarray! We have agents inside right now attempting to restore order. What information I have indicates that Thrall\'s freedom is in jeopardy. A malevolent organization known as the Infinite Dragonflight is trying to prevent his escape. I fear without outside assistance, all will be lost.',0,4,0,0,'WHISPER_CUSTODIAN_9'),
(-1000226,'We have very little information on this timeway. Sa\'at has been dispatched and is currently inside. The data we have gathered from his correspondence is that the Infinite Dragonflight are once again attempting to alter time. Could it be that the opening of the Dark Portal is being targeted for sabotage? Let us hope not...',0,4,0,0,'WHISPER_CUSTODIAN_10'),
(-1000227,'This timeway is currently collapsing. What that may hold for the past, present and future is currently unknown...',0,4,0,0,'WHISPER_CUSTODIAN_11'),
(-1000228,'The timeways are currently ranked in order from least catastrophic to most catastrophic. Note that they are all classified as catastrophic, meaning that any single one of these timeways collapsing would mean that your world would end. We only classify them in such a way so that the heroes and adventurers that are sent here know which timeway best suits their abilities.',0,4,0,0,'WHISPER_CUSTODIAN_12'),
(-1000229,'All we know of this timeway is that it leads to Mount Hyjal. The Infinite Dragonflight have gone to great lengths to prevent our involvement. We know next to nothing, mortal. Soridormi is currently attempting to break through the timeway\'s defenses but has thus far been unsuccessful. You might be our only hope of breaking through and resolving the conflict.',0,4,0,0,'WHISPER_CUSTODIAN_13'),
(-1000230,'Our time is at an end $N. I would wish you luck, if such a thing existed.',0,4,0,0,'WHISPER_CUSTODIAN_14'),

(-1000231,'Ah, $GPriest:Priestess; you came along just in time. I appreciate it.',0,0,0,20,'garments SAY_COMMON_HEALED'),
(-1000232,'Thank you! Thank you, $GPriest:Priestess;. Now I can take on those gnolls with your power to back me!',0,0,1,4,'garments SAY_DG_KEL_THANKS'),
(-1000233,'Farewell to you, and may shadow always protect you!',0,0,1,3,'garments SAY_DG_KEL_GOODBYE'),

(-1000234,'Follow me, stranger. This won\'t take long.',0,0,0,0,'SAY_KHAD_SERV_0'),
(-1000235,'Shattrath was once the draenei capital of this world. Its name means "dwelling of light."',0,4,0,0,'SAY_KHAD_SERV_1'),
(-1000236,'When the Burning Legion turned the orcs against the draenei, the fiercest battle was fought here. The draenei fought tooth and nail, but in the end the city fell.',0,4,0,0,'SAY_KHAD_SERV_2'),
(-1000237,'The city was left in ruins and darkness... until the Sha\'tar arrived.',0,4,0,0,'SAY_KHAD_SERV_3'),
(-1000238,'Let us go into the Lower City. I will warn you that as one of the only safe havens in Outland, Shattrath has attracted droves of refugees from all wars, current and past.',0,4,0,0,'SAY_KHAD_SERV_4'),
(-1000239,'The Sha\'tar, or "born from light" are the naaru that came to Outland to fight the demons of the Burning Legion.',0,4,0,0,'SAY_KHAD_SERV_5'),
(-1000240,'They were drawn to the ruins of Shattrath City where a small remnant of the draenei priesthood conducted its rites inside a ruined temple on this very spot.',0,4,0,0,'SAY_KHAD_SERV_6'),
(-1000241,'The priesthood, known as the Aldor, quickly regained its strength as word spread that the naaru had returned and reconstruction soon began. The ruined temple is now used as an infirmary for injured refugees.',0,4,0,0,'SAY_KHAD_SERV_7'),
(-1000242,'It wouldn\'t be long, however, before the city came under attack once again. This time, the attack came from Illidan\'s armies. A large regiment of blood elves had been sent by Illidan\'s ally, Kael\'thas Sunstrider, to lay waste to the city.',0,4,0,0,'SAY_KHAD_SERV_8'),
(-1000243,'As the regiment of blood elves crossed this very bridge, the Aldor\'s exarchs and vindicators lined up to defend the Terrace of Light. But then the unexpected happened.',0,4,0,0,'SAY_KHAD_SERV_9'),
(-1000244,'The blood elves laid down their weapons in front of the city\'s defenders; their leader, a blood elf elder known as Voren\'thal, stormed into the Terrace of Light and demanded to speak to A\'dal.',0,4,0,0,'SAY_KHAD_SERV_10'),
(-1000245,'As the naaru approached him, Voren\'thal kneeled before him and uttered the following words: "I\'ve seen you in a vision, naaru. My race\'s only hope for survival lies with you. My followers and I are here to serve you."',0,4,0,0,'SAY_KHAD_SERV_11'),
(-1000246,'The defection of Voren\'thal and his followers was the largest loss ever incurred by Kael\'s forces. And these weren\'t just any blood elves. Many of the best and brightest amongst Kael\'s scholars and magisters had been swayed by Voren\'thal\'s influence.',0,4,0,0,'SAY_KHAD_SERV_12'),
(-1000247,'The naaru accepted the defectors, who would become known as the Scryers; their dwelling lies in the platform above. Only those initiated with the Scryers are allowed there.',0,4,0,0,'SAY_KHAD_SERV_13'),
(-1000248,'The Aldor are followers of the Light and forgiveness and redemption are values they understand. However, they found hard to forget the deeds of the blood elves while under Kael\'s command.',0,4,0,0,'SAY_KHAD_SERV_14'),
(-1000249,'Many of the priesthood had been slain by the same magisters who now vowed to serve the naaru. They were not happy to share the city with their former enemies.',0,4,0,0,'SAY_KHAD_SERV_15'),
(-1000250,'The Aldor\'s most holy temple and its surrounding dwellings lie on the terrace above. As a holy site, only the initiated are welcome inside.',0,4,0,0,'SAY_KHAD_SERV_16'),
(-1000251,'The attacks against Shattrath continued, but the city did not fall\, as you can see. On the contrary, the naaru known as Xi\'ri led a successful incursion into Shadowmoon Valley - Illidan\'s doorstep.',0,4,0,0,'SAY_KHAD_SERV_17'),
(-1000252,'There he continues to wage war on Illidan with the assistance of the Aldor and the Scryers. The two factions have not given up on their old feuds, though.',0,4,0,0,'SAY_KHAD_SERV_18'),
(-1000253,'Such is their animosity that they vie for the honor of being sent to assist the naaru there. Each day, that decision is made here by A\'dal. The armies gather here to receive A\'dal\'s blessing before heading to Shadowmoon.',0,4,0,0,'SAY_KHAD_SERV_19'),
(-1000254,'Khadgar should be ready to see you again. Just remember that to serve the Sha\'tar you will most likely have to ally with the Aldor or the Scryers. And seeking the favor of one group will cause the others\' dislike.',0,4,0,0,'SAY_KHAD_SERV_20'),
(-1000255,'Good luck stranger, and welcome to Shattrath City.',0,4,0,0,'SAY_KHAD_SERV_21'),

(-1000256,'Thank you! Thank you, $GPriest:Priestess;. Now I can take on those murlocs with the Light on my side!',0,0,7,4,'garments SAY_ROBERTS_THANKS'),
(-1000257,'Farewell to you, and may the Light be with you always.',0,0,7,3,'garments SAY_ROBERTS_GOODBYE'),
(-1000258,'Thank you! Thank you, $GPriest:Priestess;. Now I can take on those humans with your power to back me!',0,0,1,4,'garments SAY_KORJA_THANKS'),
(-1000259,'Farewell to you, and may our ancestors be with you always!',0,0,1,3,'garments SAY_KORJA_GOODBYE'),
(-1000260,'Thank you! Thank you, $GPriest:Priestess;. Now I can take on those wendigo with the Light on my side!',0,0,7,4,'garments SAY_DOLF_THANKS'),
(-1000261,'Farewell to you, and may the Light be with you always.',0,0,7,3,'garments SAY_DOLF_GOODBYE'),
(-1000262,'Thank you! Thank you, $GPriest:Priestess;. Now I can take on those corrupt timberlings with Elune\'s power behind me!',0,0,2,4,'garments SAY_SHAYA_THANKS'),
(-1000263,'Farewell to you, and may Elune be with you always.',0,0,2,3,'garments SAY_SHAYA_GOODBYE'),

(-1000264,'Ok, $N. Follow me to the cave where I\'ll attempt to harness the power of the rune stone into these goggles.',0,0,0,1,'phizzlethorpe SAY_PROGRESS_1'),
(-1000265,'I discovered this cave on our first day here. I believe the energy in the stone can be used to our advantage.',0,0,0,1,'phizzlethorpe SAY_PROGRESS_2'),
(-1000266,'I\'ll begin drawing energy from the stone. Your job, $N, is to defend me. This place is cursed... trust me.',0,0,0,1,'phizzlethorpe SAY_PROGRESS_3'),
(-1000267,'%s begins tinkering with the goggles before the stone.',0,2,0,0,'phizzlethorpe EMOTE_PROGRESS_4'),
(-1000268,'Help!!! Get these things off me so I can get my work done!',0,0,0,0,'phizzlethorpe SAY_AGGRO'),
(-1000269,'Almost done! Just a little longer!',0,0,0,1,'phizzlethorpe SAY_PROGRESS_5'),
(-1000270,'I\'ve done it! I have harnessed the power of the stone into the goggles! Let\'s get out of here!',0,0,0,1,'phizzlethorpe SAY_PROGRESS_6'),
(-1000271,'Phew! Glad to be back from that creepy cave.',0,0,0,1,'phizzlethorpe SAY_PROGRESS_7'),
(-1000272,'%s hands one glowing goggles over to Doctor Draxlegauge.',0,2,0,0,'phizzlethorpe EMOTE_PROGRESS_8'),
(-1000273,'Doctor Draxlegauge will give you further instructions, $N. Many thanks for your help!',0,0,0,1,'phizzlethorpe SAY_PROGRESS_9'),

(-1000274,'Time to teach you a lesson in manners, little $Gboy:girl;!',0,0,0,0,'larry SAY_START'),
(-1000275,'Now I\'m gonna give you to the count of \'3\' to get out of here before I sick the dogs on you.',0,0,0,0,'larry SAY_COUNT'),
(-1000276,'1...',0,0,0,0,'larry SAY_COUNT_1'),
(-1000277,'2...',0,0,0,0,'larry SAY_COUNT_2'),
(-1000278,'Time to meet your maker!',0,0,0,0,'larry SAY_ATTACK_5'),
(-1000279,'Alright, we give up! Don\'t hurt us!',0,0,0,0,'larry SAY_GIVEUP'),

(-1000280,'A shadowy, sinister presence has invaded the Emerald Dream. Its power is poised to spill over into our world, $N. We must oppose it! That\'s why I cannot accompany you in person.',0,0,0,1,'clintar SAY_START'),
(-1000281,'The Emerald Dream will never be yours!',0,0,0,0,'clintar SAY_AGGRO_1'),
(-1000282,'Begone from this place!',0,0,0,0,'clintar SAY_AGGRO_2'),
(-1000283,'That\'s the first relic, but there are still two more. Follow me, $N.',0,0,0,0,'clintar SAY_RELIC1'),
(-1000284,'I\'ve recovered the second relic. Take a moment to rest, and then we\'ll continue to the last reliquary.',0,0,0,0,'clintar SAY_RELIC2'),
(-1000285,'We have all three of the relics, but my energy is rapidly fading. We must make our way back to Dreamwarden Lurosa! He will let you know what to do next.',0,0,0,0,'clintar SAY_RELIC3'),
(-1000286,'Lurosa, I am entrusting the Relics of Aviana to $N, who will take them to Morthis Whisperwing. I must return completely to the Emerald Dream now. Do not let $N fail!',0,0,0,1,'clintar SAY_END'),

(-1000287,'Emergency power activated! Initializing ambulanory motor! CLUCK!',0,0,0,0,'oox SAY_OOX_START'),
(-1000288,'Physical threat detected! Evasive action! CLUCK!',0,0,0,0,'oox SAY_OOX_AGGRO1'),
(-1000289,'Thread analyzed! Activating combat plan beta! CLUCK!',0,0,0,0,'oox SAY_OOX_AGGRO2'),
(-1000290,'CLUCK! Sensors detect spatial anomaly - danger imminent! CLUCK!',0,0,0,0,'oox SAY_OOX_AMBUSH'),
(-1000291,'No one challanges the Wastewander nomads - not even robotic chickens! ATTACK!',0,0,0,0,'oox SAY_OOX17_AMBUSH_REPLY'),
(-1000292,'Cloaking systems online! CLUCK! Engaging cloak for transport to Booty Bay!',0,0,0,0,'oox SAY_OOX_END'),

(-1000293,'To the house! Stay close to me, no matter what! I have my gun and ammo there!',0,0,7,0,'stilwell SAY_DS_START'),
(-1000294,'We showed that one!',0,0,7,0,'stilwell SAY_DS_DOWN_1'),
(-1000295,'One more down!',0,0,7,0,'stilwell SAY_DS_DOWN_2'),
(-1000296,'We\'ve done it! We won!',0,0,7,0,'stilwell SAY_DS_DOWN_3'),
(-1000297,'Meet me down by the orchard--I just need to put my gun away.',0,0,7,0,'stilwell SAY_DS_PROLOGUE'),

(-1000298,'Alright, alright I think I can figure out how to operate this thing...',0,0,0,393,'wizzlecrank SAY_START'),
(-1000299,'Arrrgh! This isn\'t right!',0,0,0,0,'wizzlecrank SAY_STARTUP1'),
(-1000300,'Okay, I think I\'ve got it, now. Follow me, $n!',0,0,0,1,'wizzlecrank SAY_STARTUP2'),
(-1000301,'There\'s the stolen shredder! Stop it or Lugwizzle will have our hides!',0,1,0,0,'wizzlecrank SAY_MERCENARY'),
(-1000302,'Looks like we\'re out of woods, eh? Wonder what this does...',0,0,0,0,'wizzlecrank SAY_PROGRESS_1'),
(-1000303,'Come on, don\'t break down on me now!',0,0,0,393,'wizzlecrank SAY_PROGRESS_2'),
(-1000304,'That was a close one! Well, let\'s get going, it\'s still a ways to Ratchet!',0,0,0,0,'wizzlecrank SAY_PROGRESS_3'),
(-1000305,'Hmm... I don\'t think this blinking red light is a good thing...',0,0,0,0,'wizzlecrank SAY_END'),

(-1000306,'Let\'s get to the others, and keep an eye open for those wolves cutside...',0,0,1,0,'erland SAY_START_1'),
(-1000307,'Be careful, $N. Those wolves like to hide among the trees.',0,0,1,0,'erland SAY_START_2'),
(-1000308,'A $C attacks!',0,0,1,0,'erland SAY_AGGRO_1'),
(-1000309,'Beware! I am under attack!',0,0,1,0,'erland SAY_AGGRO_2'),
(-1000310,'Oh no! A $C is upon us!',0,0,1,0,'erland SAY_AGGRO_3'),
(-1000311,'We\'re almost there!',0,0,1,0,'erland SAY_PROGRESS'),
(-1000312,'We made it! Thanks, $N. I couldn\'t have gotten without you.',0,0,1,0,'erland SAY_END'),
(-1000313,'It\'s good to see you again, Erland. What is your report?',0,0,33,1,'erland SAY_RANE'),
(-1000314,'Masses of wolves are to the east, and whoever lived at Malden\'s Orchard is gone.',0,0,1,1,'erland SAY_RANE_REPLY'),
(-1000315,'If I am excused, then I\'d like to check on Quinn...',0,0,1,1,'erland SAY_CHECK_NEXT'),
(-1000316,'Hello, Quinn. How are you faring?',0,0,1,1,'erland SAY_QUINN'),
(-1000317,'I\'ve been better. Ivar the Foul got the better of me...',0,0,33,1,'erland SAY_QUINN_REPLY'),
(-1000318,'Try to take better care of yourself, Quinn. You were lucky this time.',0,0,1,1,'erland SAY_BYE'),

(-1000319,'Let the trial begin, Bloodwrath, attack!',0,1,1,0,'kelerun SayId1'),
(-1000320,'Champion Lightrend, make me proud!',0,1,1,0,'kelerun SayId2'),
(-1000321,'Show this upstart how a real Blood Knight fights, Swiftblade!',0,1,1,0,'kelerun SayId3'),
(-1000322,'Show $n the meaning of pain, Sunstriker!',0,1,1,0,'kelerun SayId4'),

(-1000323,'Mist! I feared I would never see you again! Yes, I am well, do not worry for me. You must rest and recover your health.',0,0,7,0,'mist SAY_AT_HOME'),
(-1000324,'%s growls in acknowledgement before straightening and making her way off into the forest.',0,2,0,0,'mist EMOTE_AT_HOME'),

(-1000325,'"Threshwackonator First Mate unit prepared to follow"',0,2,0,0,'threshwackonator EMOTE_START'),
(-1000326,'YARRR! Swabie, what have ye done?! He\'s gone mad! Baton him down the hatches! Hoist the mast! ARRRR! Every man for hi\'self!',0,0,7,0,'threshwackonator SAY_AT_CLOSE'),

(-1000327,'Ok, $n, let\'s go find where I left that mysterious fossil. Follow me!',0,0,7,0,'remtravel SAY_REM_START'),
(-1000328,'Now where did I put that mysterious fossil? Ah, maybe up there...',0,0,7,0,'remtravel SAY_REM_RAMP1_1'),
(-1000329,'Hrm, nothing up here.',0,0,7,0,'remtravel SAY_REM_RAMP1_2'),
(-1000330,'No mysterious fossil here... Ah, but my copy of Green Hills of Stranglethorn. What a good book!',0,0,7,0,'remtravel SAY_REM_BOOK'),
(-1000331,'I bet you I left it in the tent!',0,0,7,0,'remtravel SAY_REM_TENT1_1'),
(-1000332,'Oh wait, that\'s Hollee\'s tent... and it\'s empty.',0,0,7,0,'remtravel SAY_REM_TENT1_2'),
(-1000333,'Interesting... I hadn\'t noticed this earlier...',0,0,7,0,'remtravel SAY_REM_MOSS'),
(-1000334,'%s inspects the ancient, mossy stone.',0,2,7,0,'remtravel EMOTE_REM_MOSS'),
(-1000335,'Oh wait! I\'m supposed to be looking for that mysterious fossil!',0,0,7,0,'remtravel SAY_REM_MOSS_PROGRESS'),
(-1000336,'Nope. didn\'t leave the fossil back here!',0,0,7,0,'remtravel SAY_REM_PROGRESS'),
(-1000337,'Ah. I remember now! I gave the mysterious fossil to Hollee! Check with her.',0,0,7,0,'remtravel SAY_REM_REMEMBER'),
(-1000338,'%s goes back to work, oblivious to everything around him.',0,2,7,0,'remtravel EMOTE_REM_END'),
(-1000339,'Something tells me this $r wants the mysterious fossil too. Help!',0,0,7,0,'remtravel SAY_REM_AGGRO'),

(-1000340,'%s howls in delight at the sight of his lunch!',0,2,0,0,'kyle EMOTE_SEE_LUNCH'),
(-1000341,'%s eats his lunch.',0,2,0,0,'kyle EMOTE_EAT_LUNCH'),
(-1000342,'%s thanks you with a special dance.',0,2,0,0,'kyle EMOTE_DANCE'),

(-1000343,'Is the way clear? Let\'s get out while we can, $N.',0,0,0,0,'kayra SAY_START'),
(-1000344,'Looks like we won\'t get away so easy. Get ready!',0,0,0,0,'kayra SAY_AMBUSH1'),
(-1000345,'Let\'s keep moving. We\'re not safe here!',0,0,0,0,'kayra SAY_PROGRESS'),
(-1000346,'Look out, $N! Enemies ahead!',0,0,0,0,'kayra SAY_AMBUSH2'),
(-1000347,'We\'re almost to the refuge! Let\'s go.',0,0,0,0,'kayra SAY_END'),

(-1000348,'Ah...the wondrous sound of kodos. I love the way they make the ground shake... inspect the beast for me.',0,0,0,0,'kodo round SAY_SMEED_HOME_1'),
(-1000349,'Hey, look out with that kodo! You had better inspect that beast before i give you credit!',0,0,0,0,'kodo round SAY_SMEED_HOME_2'),
(-1000350,'That kodo sure is a beauty. Wait a minute, where are my bifocals? Perhaps you should inspect the beast for me.',0,0,0,0,'kodo round SAY_SMEED_HOME_3'),

(-1000351,'You, there! Hand over that moonstone and nobody gets hurt!',0,1,0,0,'sprysprocket SAY_START'),
(-1000352,'%s takes the Southfury moonstone and escapes into the river. Follow her!',0,3,0,0,'sprysprocket EMOTE_START'),
(-1000353,'Just chill!',0,4,0,0,'sprysprocket SAY_WHISPER_CHILL'),
(-1000354,'Stupid grenade picked a fine time to backfire! So much for high quality goblin engineering!',0,1,0,0,'sprysprocket SAY_GRENADE_FAIL'),
(-1000355,'All right, you win! I surrender! Just don\'t hurt me!',0,1,0,0,'sprysprocket SAY_END'),

(-1000356,'Okay, okay... gimme a minute to rest now. You gone and beat me up good.',0,0,1,14,'calvin SAY_COMPLETE'),

(-1000357,'Let\'s go before they find out I\'m free!',0,0,0,1,'KAYA_SAY_START'),
(-1000358,'Look out! We\'re under attack!',0,0,0,0,'KAYA_AMBUSH'),
(-1000359,'Thank you for helping me. I know my way back from here.',0,0,0,0,'KAYA_END'),

(-1000360,'The strands of LIFE have been severed! The Dreamers must be avenged!',0,1,0,0,' ysondre SAY_AGGRO'),
(-1000361,'Come forth, ye Dreamers - and claim your vengeance!',0,1,0,0,' ysondre SAY_SUMMONDRUIDS'),

(-1000362,'Let\'s go $N. I am ready to reach Whitereach Post.',0,0,1,0,'paoka SAY_START'),
(-1000363,'Now this looks familiar. If we keep heading east, I think we can... Ahh, Wyvern on the attack!',0,0,1,0,'paoka SAY_WYVERN'),
(-1000364,'Thanks a bunch... I can find my way back to Whitereach Post from here. Be sure to talk with Motega Firemane; perhaps you can keep him from sending me home.',0,0,1,0,'paoka SAY_COMPLETE'),

(-1000365,'Be on guard... Arnak has some strange power over the Grimtotem... they will not be happy to see me escape.',0,0,1,0,'lakota SAY_LAKO_START'),
(-1000366,'Look out, the Grimtotem are upon us!',0,0,1,0,'lakota SAY_LAKO_LOOK_OUT'),
(-1000367,'Here they come.',0,0,1,0,'lakota SAY_LAKO_HERE_COME'),
(-1000368,'More Grimtotems are coming this way!',0,0,1,0,'lakota SAY_LAKO_MORE'),
(-1000369,'Finally, free at last... I must be going now, thanks for helping me escape. I can get back to Freewind Post by myself.',0,0,1,0,'lakota SAY_LAKO_END'),

(-1000370,'Stay close, $n. I\'ll need all the help I can get to break out of here. Let\'s go!',0,0,1,1,'gilthares SAY_GIL_START'),
(-1000371,'At last! Free from Northwatch Hold! I need a moment to catch my breath...',0,0,1,5,'gilthares SAY_GIL_AT_LAST'),
(-1000372,'Now i feel better. Let\'s get back to Ratchet. Come on, $n.',0,0,1,23,'gilthares SAY_GIL_PROCEED'),
(-1000373,'Looks like the Southsea Freeboters are heavily entrenched on the coast. This could get rough.',0,0,1,25,'gilthares SAY_GIL_FREEBOOTERS'),
(-1000374,'Help! $C attacking!',0,0,1,0,'gilthares SAY_GIL_AGGRO_1'),
(-1000375,'$C heading this way fast! Time for revenge!',0,0,1,0,'gilthares SAY_GIL_AGGRO_2'),
(-1000376,'$C coming right at us!',0,0,1,0,'gilthares SAY_GIL_AGGRO_3'),
(-1000377,'Get this $C off of me!',0,0,1,0,'gilthares SAY_GIL_AGGRO_4'),
(-1000378,'Almost back to Ratchet! Let\'s keep up the pace...',0,0,1,0,'gilthares SAY_GIL_ALMOST'),
(-1000379,'Ah, the sweet salt air of Ratchet.',0,0,1,0,'gilthares SAY_GIL_SWEET'),
(-1000380,'Captain Brightsun, $N here has freed me! $N, i am certain the Captain will reward your bravery.',0,0,1,66,'gilthares SAY_GIL_FREED'),

(-1000381,'I sense the tortured spirits, $n. They are this way, come quickly!',0,0,0,1,'wilda SAY_WIL_START'),
(-1000382,'Watch out!',0,0,0,0,'wilda SAY_WIL_AGGRO1'),
(-1000383,'Naga attackers! Defend yourself!',0,0,0,0,'wilda SAY_WIL_AGGRO2'),
(-1000384,'Grant me protection $n, i must break trough their foul magic!',0,0,0,0,'wilda SAY_WIL_PROGRESS1'),
(-1000385,'The naga of Coilskar are exceptionally cruel to their prisoners. It is a miracle that I survived inside that watery prison for as long as I did. Earthmother be praised.',0,0,0,0,'wilda SAY_WIL_PROGRESS2'),
(-1000386,'Now we must find the exit.',0,0,0,0,'wilda SAY_WIL_FIND_EXIT'),
(-1000387,'Lady Vashj must answer for these atrocities. She must be brought to justice!',0,0,0,0,'wilda SAY_WIL_PROGRESS4'),
(-1000388,'The tumultuous nature of the great waterways of Azeroth and Draenor are a direct result of tormented water spirits.',0,0,0,0,'wilda SAY_WIL_PROGRESS5'),
(-1000389,'It shouldn\'t be much further, $n. The exit is just up ahead.',0,0,0,1,'wilda SAY_WIL_JUST_AHEAD'),
(-1000390,'Thank you, $n. Please return to my brethren at the Altar of Damnation, near the Hand of Gul\'dan, and tell them that Wilda is safe. May the Earthmother watch over you...',0,0,0,3,'wilda SAY_WIL_END'),

(-1000391,'I\'m Thirsty.',0,0,0,0,'tooga SAY_TOOG_THIRST'),
(-1000392,'Torta must be so worried.',0,0,0,0,'tooga SAY_TOOG_WORRIED'),
(-1000393,'Torta, my love! I have returned at long last.',0,0,0,0,'tooga SAY_TOOG_POST_1'),
(-1000394,'You have any idea how long I\'ve been waiting here? And where\'s dinner? All that time gone and nothing to show for it?',0,0,0,0,'tooga SAY_TORT_POST_2'),
(-1000395,'My dearest Torta. I have been gone for so long. Finally we are reunited. At long last our love can blossom again.',0,0,0,0,'tooga SAY_TOOG_POST_3'),
(-1000396,'Enough with the rambling. I am starving! Now, get your dusty shell into that ocean and bring momma some grub.',0,0,0,0,'tooga SAY_TORT_POST_4'),
(-1000397,'Yes Torta. Whatever your heart desires...',0,0,0,0,'tooga SAY_TOOG_POST_5'),
(-1000398,'And try not to get lost this time...',0,0,0,0,'tooga SAY_TORT_POST_6'),

(-1000399,'Peace is but a fleeting dream! Let the NIGHTMARE reign!',0,1,0,0,'taerar SAY_AGGRO'),
(-1000400,'Children of Madness - I release you upon this world!',0,1,0,0,'taerar SAY_SUMMONSHADE'),

(-1000401,'Hope is a DISEASE of the soul! This land shall wither and die!',0,1,0,0,'emeriss SAY_AGGRO'),
(-1000402,'Taste your world\'s corruption!',0,1,0,0,'emeriss SAY_CASTCORRUPTION'),

(-1000403,'Rin\'ji is free!',0,0,0,0,'SAY_RIN_FREE'),
(-1000404,'Attack my sisters! The troll must not escape!',0,0,0,0,'SAY_RIN_BY_OUTRUNNER'),
(-1000405,'Rin\'ji needs help!',0,0,1,0,'SAY_RIN_HELP_1'),
(-1000406,'Rin\'ji is being attacked!',0,0,1,0,'SAY_RIN_HELP_2'),
(-1000407,'Rin\'ji can see road now, $n. Rin\'ji knows the way home.',0,0,1,0,'SAY_RIN_COMPLETE'),
(-1000408,'Rin\'ji will tell you secret now... $n, should go to the Overlook Cliffs. Rin\'ji hid something on island  there',0,0,1,0,'SAY_RIN_PROGRESS_1'),
(-1000409,'You find it, you keep it! Don\'t tell no one that Rin\'ji talked to you!',0,0,1,0,'SAY_RIN_PROGRESS_2'),

(-1000410,'Here they come! Defend yourself!',0,0,1,5,'kanati SAY_KAN_START'),

(-1000411,'Why don\'t we deal with you now, Hendel? Lady Proudmoore will speak for you back in the tower.',0,0,7,0,'hendel SAY_PROGRESS_1_TER'),
(-1000412,'Please... please... Miss Proudmore. I didn\'t mean to...',0,0,7,0,'hendel SAY_PROGRESS_2_HEN'),
(-1000413,'I apologize for taking so long to get here. I wanted Lady Proudmoore to be present also.',0,0,7,0,'hendel SAY_PROGRESS_3_TER'),
(-1000414,'We can only stay a few moments before returning to the tower. If you wish to speak to us more you may find us there.',0,0,7,0,'hendel SAY_PROGRESS_4_TER'),
(-1000415,'%s, too injured, gives up the chase.',0,2,0,0,'hendel EMOTE_SURRENDER'),

(-1000416,'Well, I\'m not sure how far I\'ll make it in this state... I\'m feeling kind of faint...',0,0,0,0,'ringo SAY_RIN_START_1'),
(-1000417,'Remember, if I faint again, the water that Spraggle gave you will revive me.',0,0,0,0,'ringo SAY_RIN_START_2'),
(-1000418,'The heat... I can\'t take it...',0,0,0,0,'ringo SAY_FAINT_1'),
(-1000419,'Maybe... you could carry me?',0,0,0,0,'ringo SAY_FAINT_2'),
(-1000420,'Uuuuuuggggghhhhh....',0,0,0,0,'ringo SAY_FAINT_3'),
(-1000421,'I\'m not feeling so well...',0,0,0,0,'ringo SAY_FAINT_4'),
(-1000422,'Where... Where am I?',0,0,0,0,'ringo SAY_WAKE_1'),
(-1000423,'I am feeling a little better now, thank you.',0,0,0,0,'ringo SAY_WAKE_2'),
(-1000424,'Yes, I must go on.',0,0,0,0,'ringo SAY_WAKE_3'),
(-1000425,'How am I feeling? Quite soaked, thank you.',0,0,0,0,'ringo SAY_WAKE_4'),
(-1000426,'Spraggle! I didn\'t think I\'d make it back!',0,0,0,0,'ringo SAY_RIN_END_1'),
(-1000427,'Ringo! You\'re okay!',0,0,0,0,'ringo SAY_SPR_END_2'),
(-1000428,'Oh... I\'m feeling faint...',0,0,0,0,'ringo SAY_RIN_END_3'),
(-1000429,'%s collapses onto the ground.',0,2,0,0,'ringo EMOTE_RIN_END_4'),
(-1000430,'%s stands up after a short pause.',0,2,0,0,'ringo EMOTE_RIN_END_5'),
(-1000431,'Ugh.',0,0,0,0,'ringo SAY_RIN_END_6'),
(-1000432,'Ringo? Wake up! Don\'t worry, I\'ll take care of you.',0,0,0,0,'ringo SAY_SPR_END_7'),
(-1000433,'%s fades away after a long pause.',0,2,0,0,'ringo EMOTE_RIN_END_8'),

(-1000434,'Liladris has been waiting for me at Maestra\'s Post, so we should make haste, $N.',0,0,0,0,'kerlonian SAY_KER_START'),
(-1000435,'%s looks very sleepy...',0,2,0,0,'kerlonian EMOTE_KER_SLEEP_1'),
(-1000436,'%s suddenly falls asleep',0,2,0,0,'kerlonian EMOTE_KER_SLEEP_2'),
(-1000437,'%s begins to drift off...',0,2,0,0,'kerlonian EMOTE_KER_SLEEP_3'),
(-1000438,'This looks like the perfect place for a nap...',0,0,0,0,'kerlonian SAY_KER_SLEEP_1'),
(-1000439,'Yaaaaawwwwwnnnn...',0,0,0,0,'kerlonian SAY_KER_SLEEP_2'),
(-1000440,'Oh, I am so tired...',0,0,0,0,'kerlonian SAY_KER_SLEEP_3'),
(-1000441,'You don\'t mind if I stop here for a moment, do you?',0,0,0,0,'kerlonian SAY_KER_SLEEP_4'),
(-1000442,'Be on the alert! The Blackwood furbolgs are numerous in the area...',0,0,0,0,'kerlonian SAY_KER_ALERT_1'),
(-1000443,'It\'s quiet... Too quiet...',0,0,0,0,'kerlonian SAY_KER_ALERT_2'),
(-1000444,'Oh, I can see Liladris from here... Tell her I\'m here, won\'t you?',0,0,0,0,'kerlonian SAY_KER_END'),
(-1000445,'%s wakes up!',0,2,0,0,'kerlonian EMOTE_KER_AWAKEN'),

(-1000446,'A-Me good. Good, A-Me. Follow... follow A-Me. Home. A-Me go home.',0,0,0,0,'ame01 SAY_AME_START'),
(-1000447,'Good... good, A-Me. A-Me good. Home. Find home.',0,0,0,0,'ame01 SAY_AME_PROGRESS'),
(-1000448,'A-Me home! A-Me good! Good A-Me. Home. Home. Home.',0,0,0,0,'ame01 SAY_AME_END'),
(-1000449,'$c, no hurt A-Me. A-Me good.',0,0,0,0,'ame01 SAY_AME_AGGRO1'),
(-1000450,'Danger. Danger! $c try hurt A-Me.',0,0,0,0,'ame01 SAY_AME_AGGRO2'),
(-1000451,'Bad, $c. $c, bad!',0,0,0,0,'ame01 SAY_AME_AGGRO3'),

(-1000452,'I noticed some fire on that island over there. A human, too. Let\'s go check it out, $n.',0,0,1,0,'ogron SAY_OGR_START'),
(-1000453,'That\'s Reethe alright. Let\'s go see what he has to say, yeah?',0,0,1,1,'ogron SAY_OGR_SPOT'),
(-1000454,'W-what do you want? Just leave me alone...',0,0,0,6,'ogron SAY_OGR_RET_WHAT'),
(-1000455,'I swear. I didn\'t steal anything from you! Here, take some of my supplies, just go away!',0,0,0,27,'ogron SAY_OGR_RET_SWEAR'),
(-1000456,'Just tell us what you know about the Shady Rest Inn, and I won\'t bash your skull in.',0,0,1,0,'ogron SAY_OGR_REPLY_RET'),
(-1000457,'I... Well, I may of taken a little thing or two from the inn... but what would an ogre care about that?',0,0,0,6,'ogron SAY_OGR_RET_TAKEN'),
(-1000458,'Look here, if you don\'t tell me about the fire--',0,0,1,0,'ogron SAY_OGR_TELL_FIRE'),
(-1000459,'Not one step closer, ogre!',0,0,0,27,'ogron SAY_OGR_RET_NOCLOSER'),
(-1000460,'And I don\'t know anything about this fire of yours...',0,0,0,0,'ogron SAY_OGR_RET_NOFIRE'),
(-1000461,'What was that? Did you hear something?',0,0,0,0,'ogron SAY_OGR_RET_HEAR'),
(-1000462,'Paval Reethe! Found you at last. And consorting with ogres now? No fear, even deserters and traitors are afforded some mercy.',0,0,0,0,'ogron SAY_OGR_CAL_FOUND'),
(-1000463,'Private, show Lieutenant Reethe some mercy.',0,0,0,29,'ogron SAY_OGR_CAL_MERCY'),
(-1000464,'Gladly, sir.',0,0,0,0,'ogron SAY_OGR_HALL_GLAD'),
(-1000465,'%s staggers backwards as the arrow lodges itself deeply in his chest.',0,2,0,0,'ogron EMOTE_OGR_RET_ARROW'),
(-1000466,'Ugh... Hallan, didn\'t think you had it in you...',0,0,0,34,'ogron SAY_OGR_RET_ARROW'),
(-1000467,'Now, let\'s clean up the rest of the trash, men!',0,0,0,0,'ogron SAY_OGR_CAL_CLEANUP'),
(-1000468,'Damn it! You\'d better not die on me, human!',0,0,1,0,'ogron SAY_OGR_NODIE'),
(-1000469,'Still with us, Reethe?',0,0,1,0,'ogron SAY_OGR_SURVIVE'),
(-1000470,'Must be your lucky day. Alright, I\'ll talk. Just leave me alone. Look, you\'re not going to believe me, but it wa... oh, Light, looks like the girl could shoot...',0,0,0,0,'ogron SAY_OGR_RET_LUCKY'),
(-1000471,'By the way, thanks for watching my back.',0,0,1,0,'ogron SAY_OGR_THANKS'),

(-1000472,'1...',0,3,0,0,'mana bomb SAY_COUNT_1'),
(-1000473,'2...',0,3,0,0,'mana bomb SAY_COUNT_2'),
(-1000474,'3...',0,3,0,0,'mana bomb SAY_COUNT_3'),
(-1000475,'4...',0,3,0,0,'mana bomb SAY_COUNT_4'),
(-1000476,'5...',0,3,0,0,'mana bomb SAY_COUNT_5'),

(-1000477,'Let us leave this place. I\'ve had enough of these madmen!',0,0,0,0,'akuno SAY_AKU_START'),
(-1000478,'You\'ll go nowhere, fool!',0,0,0,0,'akuno SAY_AKU_AMBUSH_A'),
(-1000479,'Beware! More cultists come!',0,0,0,0,'akuno SAY_AKU_AMBUSH_B'),
(-1000480,'You will not escape us so easily!',0,0,0,0,'akuno SAY_AKU_AMBUSH_B_REPLY'),
(-1000481,'I can find my way from here. My friend Mekeda will reward you for your kind actions.',0,0,0,0,'akuno SAY_AKU_COMPLETE'),

(-1000482,'Look out!',0,0,0,0,'maghar captive SAY_MAG_START'),
(-1000483,'Don\'t let them escape! Kill the strong one first!',0,0,0,0,'maghar captive SAY_MAG_NO_ESCAPE'),
(-1000484,'More of them coming! Watch out!',0,0,0,0,'maghar captive SAY_MAG_MORE'),
(-1000485,'Where do you think you\'re going? Kill them all!',0,0,0,0,'maghar captive SAY_MAG_MORE_REPLY'),
(-1000486,'Ride the lightning, filth!',0,0,0,0,'maghar captive SAY_MAG_LIGHTNING'),
(-1000487,'FROST SHOCK!!!',0,0,0,0,'maghar captive SAY_MAG_SHOCK'),
(-1000488,'It is best that we split up now, in case they send more after us. Hopefully one of us will make it back to Garrosh. Farewell stranger.',0,0,0,0,'maghar captive SAY_MAG_COMPLETE'),

(-1000489,'Show our guest around Shattrath, will you? Keep an eye out for pickpockets in the lower city.',0,0,0,0,'SAY_KHAD_START'),
(-1000490,'A newcomer to Shattrath! Make sure to drop by later. We can always use a hand with the injured.',0,0,0,0,'SAY_KHAD_INJURED'),
(-1000491,'Be mindful of what you say, this one\'s being shown around by Khadgar\'s pet.',0,0,0,0,'SAY_KHAD_MIND_YOU'),
(-1000492,'Are you joking? I\'m a Scryer, I always watch what i say. More enemies than allies in this city, it seems.',0,0,0,0,'SAY_KHAD_MIND_ALWAYS'),
(-1000493,'Light be with you, $n. And welcome to Shattrath.',0,0,0,0,'SAY_KHAD_ALDOR_GREET'),
(-1000494,'We\'re rather selective of who we befriend, $n. You think you have what it takes?',0,0,0,0,'SAY_KHAD_SCRYER_GREET'),
(-1000495,'Khadgar himself is showing you around? You must have made a good impression, $n.',0,0,0,0,'SAY_KHAD_HAGGARD'),

(-1000496,'%s lifts its head into the air, as if listening for something.',0,2,0,0,'ancestral wolf EMOTE_WOLF_LIFT_HEAD'),
(-1000497,'%s lets out a howl that rings across the mountains to the north and motions for you to follow.',0,2,0,0,'ancestral wolf EMOTE_WOLF_HOWL'),
(-1000498,'Welcome, kind spirit. What has brought you to us?',0,0,0,0,'ancestral wolf SAY_WOLF_WELCOME'),

(-1000499,'Puny $r wanna fight %s? Me beat you! Me boss here!',0,0,1,0,'morokk SAY_MOR_CHALLENGE'),
(-1000500,'Me scared! Me run now!',0,1,0,0,'morokk SAY_MOR_SCARED'),

(-1000501,'Are you sure that you are ready? If we do not have a group of your allies to aid us, we will surely fail.',0,0,1,0,'muglash SAY_MUG_START1'),
(-1000502,'This will be a though fight, $n. Follow me closely.',0,0,1,0,'muglash SAY_MUG_START2'),
(-1000503,'This is the brazier, $n. Put it out. Vorsha is a beast, worthy of praise from no one!',0,0,1,0,'muglash SAY_MUG_BRAZIER'),
(-1000504,'Now we must wait. It won\'t be long before the naga realize what we have done.',0,0,1,0,'muglash SAY_MUG_BRAZIER_WAIT'),
(-1000505,'Be on your guard, $n!',0,0,1,0,'muglash SAY_MUG_ON_GUARD'),
(-1000506,'Perhaps we will get a moment to rest. But i will not give up until we have faced off against Vorsha!',0,0,1,0,'muglash SAY_MUG_REST'),
(-1000507,'We have done it!',0,0,1,0,'muglash SAY_MUG_DONE'),
(-1000508,'You have my deepest gratitude. I thank you.',0,0,1,0,'muglash SAY_MUG_GRATITUDE'),
(-1000509,'I am going to patrol the area for a while longer and ensure that things are truly safe.',0,0,1,0,'muglash SAY_MUG_PATROL'),
(-1000510,'Please return to Zoram\'gar and report our success to the Warsong runner.',0,0,1,0,'muglash SAY_MUG_RETURN'),

(-1000511,'Aright, listen up! Form a circle around me and move out!',0,0,0,0,'letoll SAY_LE_START'),
(-1000512,'Aright, $r, just keep us safe from harm while we work. We\'ll pay you when we return.',0,0,0,0,'letoll SAY_LE_KEEP_SAFE'),
(-1000513,'The dig site is just north of here.',0,0,0,0,'letoll SAY_LE_NORTH'),
(-1000514,'We\'re here! Start diggin\'!',0,0,0,0,'letoll SAY_LE_ARRIVE'),
(-1000515,'I think there\'s somethin\' buried here, beneath the sand!',0,0,0,0,'letoll SAY_LE_BURIED'),
(-1000516,'Almost got it!',0,0,0,0,'letoll SAY_LE_ALMOST'),
(-1000517,'By brann\'s brittle bananas! What is it!? It... It looks like a drum.',0,0,0,0,'letoll SAY_LE_DRUM'),
(-1000518,'Wow... a drum.',0,0,0,0,'letoll SAY_LE_DRUM_REPLY'),
(-1000519,'This discovery will surely rock the foundation of modern archaeology.',0,0,0,0,'letoll SAY_LE_DISCOVERY'),
(-1000520,'Yea, great. Can we leave now? This desert is giving me hives.',0,0,0,0,'letoll SAY_LE_DISCOVERY_REPLY'),
(-1000521,'Have ye gone mad? You expect me to leave behind a drum without first beatin\' on it? Not this son of Ironforge! No sir!',0,0,0,0,'letoll SAY_LE_NO_LEAVE'),
(-1000522,'This reminds me of that one time where you made us search Silithus for evidence of sand gnomes.',0,0,0,0,'letoll SAY_LE_NO_LEAVE_REPLY1'),
(-1000523,'Or that time when you told us that you\'d discovered the cure for the plague of the 20th century. What is that even? 20th century?',0,0,0,0,'letoll SAY_LE_NO_LEAVE_REPLY2'),
(-1000524,'I don\'t think it can top the one time where he told us that he\'d heard that Artha\'s "cousin\'s" skeleton was frozen beneath a glacier in Winterspring. I\'ll never forgive you for that one, Letoll. I mean honestly... Artha\'s cousin?',0,0,0,0,'letoll SAY_LE_NO_LEAVE_REPLY3'),
(-1000525,'I dunno. It can\'t possibly beat the time he tried to convince us that we\'re all actually a figment of some being\'s imagination and that they only use us for their own personal amusement. That went over well during dinner with the family.',0,0,0,0,'letoll SAY_LE_NO_LEAVE_REPLY4'),
(-1000526,'Shut yer yaps! I\'m gonna bang on this drum and that\'s that!',0,0,0,0,'letoll SAY_LE_SHUT'),
(-1000527,'Say, do you guys hear that?',0,0,0,0,'letoll SAY_LE_REPLY_HEAR'),
(-1000528,'IN YOUR FACE! I told you there was somethin\' here!',0,0,0,0,'letoll SAY_LE_IN_YOUR_FACE'),
(-1000529,'Don\'t just stand there! Help him out!',0,0,0,0,'letoll SAY_LE_HELP_HIM'),
(-1000530,'%s picks up the drum.',0,2,0,0,'letoll EMOTE_LE_PICK_UP'),
(-1000531,'You\'ve been a tremendous help, $r! Let\'s get out of here before more of those things show up! I\'ll let Dwarfowitz know you did the job asked of ya\' admirably.',0,0,0,0,'letoll SAY_LE_THANKS'),

(-1000532,'At your command, my liege...',0,0,0,0,'torloth TORLOTH_DIALOGUE1'),
(-1000533,'As you desire, Lord Illidan.',0,0,0,0,'torloth TORLOTH_DIALOGUE2'),
(-1000534,'Yes, Lord Illidan, I would sacrifice to you this magnificent physique. On this day you will fall - another victim of Torloth...',0,0,0,0,'torloth TORLOTH_DIALOGUE3'),
(-1000535,'Destroy them, Torloth. Let lose their blood like a river upon this hallowed ground.',0,0,0,0,'lordillidan ILLIDAN_DIALOGUE'),
(-1000536,'What manner of fool dares stand before Illidan Stormrage? Soldiers, destroy these insects!',0,1,0,0,'lordillidan ILLIDAN_SUMMON1'),
(-1000537,'You are no challenge for the Crimson Sigil. Mind breakers, end this nonsense.',0,1,0,0,'lordillidan ILLIDAN_SUMMON2'),
(-1000538,'Run while you still can. The highlords come soon...',0,1,0,0,'lordillidan ILLIDAN_SUMMON3'),
(-1000539,'Torloth your master calls!',0,1,0,0,'lordillidan ILLIDAN_SUMMON4'),
(-1000540,'So you have defeated the Crimson Sigil. You now seek to challenge my rule? Not even Arthas could defeat me, yet you dare to even harbor such thoughts? Then I say to you, come! Come $N! The Black Temple awaits...',0,1,0,0,'lordillidan EVENT_COMPLETED'),

(-1000541,'%s jumps into the moonwell and goes underwater...',0,2,0,0,'kitten EMOTE_SAB_JUMP'),
(-1000542,'%s follows $n obediertly.',0,2,0,0,'kitten EMOTE_SAB_FOLLOW'),

(-1000543,'Why have you come here, outsider? You will only find pain! Our fate will be yours...',0,0,0,25,'restless app SAY_RAND_1'),
(-1000544,'It was ... terrible... the demon...',0,0,0,25,'restless app SAY_RAND_2'),
(-1000545,'The darkness... the corruption... they came too quickly for anyone to know...',0,0,0,25,'restless app SAY_RAND_3'),
(-1000546,'The darkness will consume all... all the living...',0,0,0,25,'restless app SAY_RAND_4'),
(-1000547,'It is too late for us, living one. Take yourself and your friend away from here before you both are... claimed...',0,0,0,25,'restless app SAY_RAND_5'),
(-1000548,'It is too late for Jarl... its hold is too strong...',0,0,0,25,'restless app SAY_RAND_6'),
(-1000549,'Go away, whoever you are! Witch Hill is mine... mine!',0,0,0,25,'restless app SAY_RAND_7'),
(-1000550,'The manor... someone else... will soon be consumed...',0,0,0,25,'restless app SAY_RAND_8'),

(-1000551,'The %s is angered by your request and attacks!',0,2,0,0,'woodlands EMOTE_AGGRO'),
(-1000552,'Breaking off a piece of its bark, the %s hands it to you before departing.',0,2,0,0,'woodlands EMOTE_CREATE'),

(-1000553,'Be ready, $N. I hear the council returning. Prepare to ambush!',0,0,0,0,'deathstalker_faerleia SAY_START'),
(-1000554,'Well done. A blow to Arugal no doubt!',0,0,0,0,'deathstalker_faerleia SAY_END'),

(-1000555,'Back... to work...',0,0,0,0,'exhausted vrykul SAY_RAND_WORK1'),
(-1000556,'You treat us worse than animals!',0,0,0,0,'exhausted vrykul SAY_RAND_WORK2'),
(-1000557,'We will have revenge...some day.',0,0,0,0,'exhausted vrykul SAY_RAND_WORK3'),
(-1000558,'Curse you! You will not treat me like a beast!',0,0,0,0,'exhausted vrykul SAY_RAND_ATTACK1'),
(-1000559,'I\'d rather die fighting than live like a slave.',0,0,0,0,'exhausted vrykul SAY_RAND_ATTACK2'),
(-1000560,'Enough! I will teach you some manners, wench!',0,0,0,0,'exhausted vrykul SAY_RAND_ATTACK3'),

(-1000561,'My wounds are grave. Forgive my slow pace but my injuries won\'t allow me to walk any faster.',0,0,0,0,'SAY_CORPORAL_KEESHAN_1'),
(-1000562,'Ah, fresh air, at last! I need a moment to rest.',0,0,0,0,'SAY_CORPORAL_KEESHAN_2'),
(-1000563,'The Blackrock infestation is thick in these parts. I will do my best to keep the pace. Let\'s go!',0,0,0,0,'SAY_CORPORAL_KEESHAN_3'),
(-1000564,'Marshal Marris, sir. Corporal Keeshan of the 12th Sabre Regiment returned from battle and reporting for duty!',0,0,0,0,'SAY_CORPORAL_KEESHAN_4'),
(-1000565,'Brave adventurer, thank you for rescuing me! I am sure Marshal Marris will reward your kind deed.',0,0,0,0,'SAY_CORPORAL_KEESHAN_5'),

(-1000566,'Stand back! Stand clear! The infernal will need to be given a wide berth!',0,0,0,0,'SAY_NIBY_1'),
(-1000567,'BOW DOWN TO THE ALMIGHTY! BOW DOWN BEFORE MY INFERNAL DESTRO... chicken?',0,0,0,0,'SAY_NIBY_2'),
(-1000568,'%s rolls on the floor laughing.',0,2,0,0,'EMOTE_IMPSY_1'),
(-1000569,'Niby, you\' re an idiot.',0,0,0,0,'SAY_IMPSY_1'),
(-1000570,'Silence, servant! Vengeance will be mine! Death to Stormwind! Death by chicken!',0,0,0,0,'SAY_NIBY_3'),

(-1000571,'Help! I\'ve only one hand to defend myself with.',0,0,0,0,'SAY_MIRAN_1'),
(-1000572,'Feel the power of the Dark Iron Dwarves!',0,0,0,0,'SAY_DARK_IRON_DWARF'),
(-1000573,'Send them on! I\'m not afraid of some scrawny beasts!',0,0,0,0,'SAY_MIRAN_2'),
(-1000574,'Ah, here at last! It\'s going to feel so good to get rid of these barrels.',0,0,0,0,'SAY_MIRAN_3'),

(-1000575,'Together we will fight our way out of here. Are you ready?',0,0,0,0,'Lurgglbr - SAY_START_1'),
(-1000576,'Then we leave.',0,0,0,0,'Lurgglbr - SAY_START_2'),
(-1000577,'This is far enough. I can make it on my own from here.',0,0,0,0,'Lurgglbr - SAY_END_1'),
(-1000578,'Thank you for helping me $r. Please tell the king I am back.',0,0,0,0,'Lurgglbr - SAY_END_2'),

(-1000579,'There! Destroy him! The Cipher must be recovered!',0,0,0,25,'spirit hunter - SAY_VENERATUS_SPAWN'),

(-1000580,'Sleep now, young one ...',0,0,0,0,'Raelorasz SAY_SLEEP'),
(-1000581,'A wonderful specimen.',0,0,0,0,'Raeloarsz SAY_SPECIMEN'),

(-1000582,'Help! Please, You must help me!',0,0,0,0,'Galen - periodic say'),
(-1000583,'Let us leave this place.',0,0,0,0,'Galen - quest accepted'),
(-1000584,'Look out! The $c attacks!',0,0,0,0,'Galen - aggro 1'),
(-1000585,'Help! I\'m under attack!',0,0,0,0,'Galen - aggro 2'),
(-1000586,'Thank you $N. I will remember you always. You can find my strongbox in my camp, north of Stonard.',0,0,0,0,'Galen - quest complete'),
(-1000587,'%s whispers to $N the secret to opening his strongbox.',0,2,0,0,'Galen - emote whisper'),
(-1000588,'%s disappears into the swamp.',0,2,0,0,'Galen - emote disapper'),

(-1000589,'Kroshius live? Kroshius crush!',0,1,0,0,'SAY_KROSHIUS_REVIVE'),

(-1000590,'Woot!',0,0,0,0,'Captive Child SAY_THANKS_1'),
(-1000591,'I think those weird bird guys were going to eat us. Gross!',0,0,0,0,'Captive Child SAY_THANKS_2'),
(-1000592,'Yay! We\'re free!',0,0,0,0,'Captive Child SAY_THANKS_3'),
(-1000593,'Gross!',0,0,0,0,'Captive Child SAY_THANKS_4'),

(-1000594,'At last... now I can rest.',0,0,0,0,'hero spirit SAY_BLESS_1'),
(-1000595,'I\'m so tired. Just let me rest for a moment.',0,0,0,0,'hero spirit SAY_BLESS_2'),
(-1000596,'I can\'t hear the screams anymore. Is this the end?',0,0,0,0,'hero spirit SAY_BLESS_3'),
(-1000597,'My nightmare, is it finally over?',0,0,0,0,'hero spirit SAY_BLESS_4'),
(-1000598,'It was awful... I dreamt I was fighting against my friends.',0,0,0,0,'hero spirit SAY_BLESS_5'),

(-1000599,'It\'s a miracle! The beast skinned itself!',0,0,0,5,'nesingwary trapper SAY_PHRASE_1'),
(-1000600,'Jackpot!',0,0,0,5,'nesingwary trapper SAY_PHRASE_2'),
(-1000601,'This is the last one i need for that set of Nesingwary steak knives!',0,0,0,5,'nesingwary trapper SAY_PHRASE_3'),
(-1000602,'Silly beasts!',0,0,0,5,'nesingwary trapper SAY_PHRASE_4'),

(-1000603,'Do not test me, scurvy dog! I\'m trained in the way of the Blood Knights!',0,0,0,0,'silvermoon harry SAY_AGGRO'),
(-1000604,'I\'ll pay! I\'ll pay! Eeeek! Please don\'t hurt me!',0,0,0,0,'silvermoon harry SAY_BEATEN'),

(-1000605,'We wait until you ready.',0,0,0,0,'rainspeaker SAY_ACCEPT'),
(-1000606,'Home time!',0,0,0,0,'rainspeaker SAY_START'),
(-1000607,'Thanks!',0,0,0,0,'rainspeaker SAY_END_1'),
(-1000608,'Oh no! Some puppy-men followed!',0,0,0,0,'rainspeaker SAY_END_2'),
(-1000609,'Dumb big-tongue lover! You not friend of Frenzyheart no more. Frenzyheart will get you good.',0,1,0,0,'rainspeaker SAY_TRACKER'),

(-1000610,'The mosswalker victim groans in pain.',0,2,0,0,'mosswalker victim EMOTE_PAIN'),

(-1000611,'Maybe you make weather better too?',0,0,0,0,'mosswalker victim SAY_RESCUE_1'),
(-1000612,'We saved. You nice, dryskin.',0,0,0,0,'mosswalker victim SAY_RESCUE_2'),
(-1000613,'You save us! Yay for you!',0,0,0,0,'mosswalker victim SAY_RESCUE_3'),
(-1000614,'Thank you! You good!',0,0,0,0,'mosswalker victim SAY_RESCUE_4'),

(-1000615,'Use my shinies...make weather good again...make undead things go away.',0,0,0,0,'mosswalker victim SAY_DIE_1'),
(-1000616,'We gave shinies to shrine... we not greedy... why this happen?',0,0,0,0,'mosswalker victim SAY_DIE_2'),
(-1000617,'I do something bad? I sorry....',0,0,0,0,'mosswalker victim SAY_DIE_3'),
(-1000618,'We not do anything... to them... I no understand.',0,0,0,0,'mosswalker victim SAY_DIE_4'),
(-1000619,'Thank...you.',0,0,0,0,'mosswalker victim SAY_DIE_5'),
(-1000620,'Please take... my shinies. All done...',0,0,0,0,'mosswalker victim SAY_DIE_6'),

(-1000621,'All systems on-line.  Prepare yourself, we leave shortly.',0,0,0,0,'maxx SAY_START'),
(-1000622,'Be careful in there and come back in one piece!',0,0,0,0,'maxx SAY_ALLEY_FAREWELL'),
(-1000623,'Proceed.',0,0,0,0,'maxx SAY_CONTINUE'),
(-1000624,'You\'re back!  Were you able to get all of the machines?',0,0,0,0,'maxx SAY_ALLEY_FINISH'),

(-1000625,'%s gathers the warp chaser\'s blood.',0,2,0,0,'zeppit EMOTE_GATHER_BLOOD'),

(-1000626,'Intiating energy collection.',0,0,0,0,'depleted golem SAY_GOLEM_CHARGE'),
(-1000627,'Energy collection complete.',0,0,0,0,'depleted golem SAY_GOLEM_COMPLETE'),

(-1000628,'%s feeds on the freshly-killed warp chaser.',0,2,0,0,'hungry ray EMOTE_FEED'),

(-1000629,'<cough> <cough> Damsel in distress over here!',0,0,0,0,'isla starmane - SAY_ISLA_PERIODIC_1'),
(-1000630,'Hello? Help?',0,0,0,0,'isla starmane - SAY_ISLA_PERIODIC_2'),
(-1000631,'Don\'t leave me in here! Cause if you do I will find you!',0,0,0,0,'isla starmane - SAY_ISLA_PERIODIC_3'),
(-1000632,'Ok, let\'s get out of here!',0,0,0,0,'isla starmane - SAY_ISLA_START'),
(-1000633,'You sure you\'re ready? Take a moment.',0,0,0,0,'isla starmane - SAY_ISLA_WAITING'),
(-1000634,'Alright, let\'s do this!',0,0,0,0,'isla starmane - SAY_ISLA_LEAVE_BUILDING'),

(-1000635,'So then we too are cursed?',0,0,0,0,'ancient vrykul SAY_VRYKUL_CURSED'),
(-1000636,'%s points to the infant.',0,2,0,0,'ancient vrykul EMOTE_VRYKUL_POINT'),
(-1000637,'%s sobs.',0,2,0,0,'ancient vrykul EMOTE_VRYKUL_SOB'),
(-1000638,'The gods have forsaken us! We must dispose of it before Ymiron is notified!',0,0,0,0,'ancient vrykul SAY_VRYKUL_DISPOSE'),
(-1000639,'NO! You cannot! I beg of you! It is our child!',0,0,0,0,'ancient vrykul SAY_VRYKUL_BEG'),
(-1000640,'Then what are we to do, wife? The others cannot find out. Should they learn of this aberration, we will all be executed.',0,0,0,0,'ancient vrykul SAY_VRYKUL_WHAT'),
(-1000641,'I... I will hide it. I will hide it until I find it a home, far away from here...',0,0,0,0,'ancient vrykul SAY_VRYKUL_HIDE'),

(-1000642,'It\'s a female.',0,5,0,0,'leopard icepaw SAY_ITS_FEMALE'),
(-1000643,'It\'s an angry male!',0,5,0,0,'leopard icepaw SAY_ITS_MALE'),

(-1000644,'Ouch! That\'s it, I quit the target business!',0,0,0,0,'SAY_LUCKY_HIT_1'),
(-1000645,'My ear! You grazed my ear!',0,0,0,0,'SAY_LUCKY_HIT_2'),
(-1000646,'Not the \'stache! Now I\'m asymmetrical!',0,0,0,5,'SAY_LUCKY_HIT_3'),
(-1000647,'Good shot!',0,0,0,4,'SAY_LUCKY_HIT_APPLE'),
(-1000648,'Stop whining. You\'ve still got your luck.',0,0,0,0,'SAY_DROSTAN_GOT_LUCKY_1'),
(-1000649,'Bah, it\'s an improvement.',0,0,0,11,'SAY_DROSTAN_GOT_LUCKY_2'),
(-1000650,'Calm down lad, it\'s just a birdshot!',0,0,0,0,'SAY_DROSTAN_HIT_BIRD_1'),
(-1000651,'The only thing hurt is your pride, lad! Buck up!',0,0,0,0,'SAY_DROSTAN_HIT_BIRD_2'),

(-1000652,'Me so hungry! YUM!',0,0,0,71,'dragonmaw peon SAY_PEON_1'),
(-1000653,'Hey... me not feel so good.',0,0,0,0,'dragonmaw peon SAY_PEON_2'),
(-1000654,'You is bad orc... baaad... or... argh!',0,0,0,0,'dragonmaw peon SAY_PEON_3'),
(-1000655,'Time for eating!?',0,0,0,71,'dragonmaw peon SAY_PEON_4'),
(-1000656,'It put the mutton in the stomach!',0,0,0,71,'dragonmaw peon SAY_PEON_5'),

(-1000657,'Let\'s get the hell out of here.',0,0,0,5,'helice SAY_HELICE_ACCEPT'),
(-1000658,'Listen up, Venture Company goons! Rule #1: Never keep the prisoner near the explosives.',0,0,0,25,'helice SAY_HELICE_EXPLOSIVES_1'),
(-1000659,'Or THIS is what you get.',0,0,0,0,'helice SAY_HELICE_EXPLODE_1'),
(-1000660,'It\'s getting a little hot over here. Shall we move on?',0,0,0,11,'helice SAY_HELICE_MOVE_ON'),
(-1000661,'Oh, look, it\'s another cartload of explosives! Let\'s help them dispose of it.',0,0,0,25,'helice SAY_HELICE_EXPLOSIVES_2'),
(-1000662,'You really shouldn\'t play with this stuff. Someone could get hurt.',0,0,0,5,'helice SAY_HELICE_EXPLODE_2'),
(-1000663,'We made it! Thank you for getting me out of that hell hole. Tell Hemet to expect me!',0,0,0,4,'helice SAY_HELICE_COMPLETE'),

(-1000664,'The Destructive Ward gains in power.',0,5,0,0,'destructive ward SAY_WARD_POWERUP'),
(-1000665,'The Destructive Ward is fully charged!',0,5,0,0,'destructive ward SAY_WARD_CHARGED'),

(-1000666,'I can sense the SHADOW on your hearts. There can be no rest for the wicked!',0,1,0,0,'lethon SAY_LETHON_AGGRO'),
(-1000667,'Your wicked souls shall feed my power!',0,1,0,0,'lethon SAY_LETHON_SHADE'),

(-1000668,'%s releases the last of its energies into the nearby runestone, successfully reactivating it.',0,2,0,0,'infused crystal SAY_DEFENSE_FINISH'),

(-1000669,'We will locate the origin of the Nightmare through the fragments you collected, $N. From there, we will pull Eranikus through a rift in the Dream. Steel yourself, $C. We are inviting the embodiment of the Nightmare into our world.',0,0,0,0,'remulos SAY_REMULOS_INTRO_1'),
(-1000670,'To Nighthaven! Keep your army close, champion. ',0,0,0,0,'remulos SAY_REMULOS_INTRO_2'),
(-1000671,'The rift will be opened there, above the Lake Elun\'ara. Prepare yourself, $N. Eranikus entry into our world will be wrought with chaos and strife.',0,0,0,0,'remulos SAY_REMULOS_INTRO_3'),
(-1000672,'He will stop at nothing to get to Malfurion\'s physical manifistation. That must not happen... We must keep the beast occupied long enough for Tyrande to arrive.',0,0,0,0,'remulos SAY_REMULOS_INTRO_4'),
(-1000673,'Defend Nightaven, hero...',0,0,0,0,'remulos SAY_REMULOS_INTRO_5'),
(-1000674,'%s has entered our world',0,3,0,0,'eranikus EMOTE_SUMMON_ERANIKUS'),
(-1000675,'Pitful predictable mortals... You know not what you have done! The master\'s will fulfilled. The Moonglade shall be destroyed and Malfurion along with it!',0,1,0,0,'eranikus SAY_ERANIKUS_SPAWN'),
(-1000676,'Fiend! Face the might of Cenarius!',0,1,0,1,'remulos SAY_REMULOS_TAUNT_1'),
(-1000677,'%s lets loose a sinister laugh.',0,2,0,0,'eranikus EMOTE_ERANIKUS_LAUGH'),
(-1000678,'You are certanly not your father, insect. Should it interest me, I would crush you with but a swipe of my claws. Turn Shan\'do Stormrage over to me and your pitiful life will be spared along with the lives of your people.',0,1,0,0,'eranikus SAY_ERANIKUS_TAUNT_2'),
(-1000679,'Who is the predictable one, beast? Surely you did not think that we would summon you on top of Malfurion? Your redemption comes, Eranikus. You will be cleansed of this madness - this corruption.',0,1,0,1,'remulos SAY_REMULOS_TAUNT_3'),
(-1000680,'My redemption? You are bold, little one. My redemption comes by the will of my god.',0,1,0,0,'eranikus SAY_ERANIKUS_TAUNT_4'),
(-1000681,'%s roars furiously.',0,2,0,0,'eranikus EMOTE_ERANIKUS_ATTACK'),
(-1000682,'Hurry, $N! We must find protective cover!',0,0,0,0,'remulos SAY_REMULOS_DEFEND_1'),
(-1000683,'Please, champion, protect our people.',0,0,0,1,'remulos SAY_REMULOS_DEFEND_2'),
(-1000684,'Rise, servants of the Nightmare! Rise and destroy this world! Let there be no survivors...',0,1,0,0,'eranikus SAY_ERANIKUS_SHADOWS'),
(-1000685,'We will battle these fiends, together! Nighthaven\'s Defenders are also among us. They will fight to the death if asked. Now, quickly, we must drive these aberations back to the Nightmare. Destroy them all!',0,0,0,1,'remulos SAY_REMULOS_DEFEND_3'),
(-1000686,'Where is your savior? How long can you hold out against my attacks?',0,1,0,0,'eranikus SAY_ERANIKUS_ATTACK_1'),
(-1000687,'Defeated my minions? Then face me, mortals!',0,1,0,0,'eranikus SAY_ERANIKUS_ATTACK_2'),
(-1000688,'Remulos, look how easy they fall before me? You can stop this, fool. Turn the druid over to me and it will all be over...',0,1,0,0,'eranikus SAY_ERANIKUS_ATTACK_3'),
(-1000689,'Elune, hear my prayers. Grant us serenity! Watch over our fallen...',0,1,0,0,'tyrande SAY_TYRANDE_APPEAR'),
(-1000690,'Tend to the injuries of the wounded, sisters!',0,0,0,0,'tyrande SAY_TYRANDE_HEAL'),
(-1000691,'Seek absolution, Eranikus. All will be forgiven...',0,1,0,0,'tyrande SAY_TYRANDE_FORGIVEN_1'),
(-1000692,'You will be forgiven, Eranikus. Elune will always love you. Break free of the bonds that command you!',0,1,0,0,'tyrande SAY_TYRANDE_FORGIVEN_2'),
(-1000693,'The grasp of the Old Gods is unmoving. He is consumed by their dark thoughts... I... I... I cannot... cannot channel much longer... Elune aide me.',0,0,0,0,'tyrande SAY_TYRANDE_FORGIVEN_3'),
(-1000694,'IT BURNS! THE PAIN.. SEARING...',0,1,0,0,'eranikus SAY_ERANIKUS_DEFEAT_1'),
(-1000695,'WHY? Why did this happen to... to me? Where were you Tyrande? Where were you when I fell from the grace of Elune?',0,1,0,0,'eranikus SAY_ERANIKUS_DEFEAT_2'),
(-1000696,'I... I feel... I feel the touch of Elune upon my being once more... She smiles upon me... Yes... I...', 0,1,0,0,'eranikus SAY_ERANIKUS_DEFEAT_3'),
(-1000697,'%s is wholly consumed by the Light of Elune. Tranquility sets in over the Moonglade',0,2,0,0,'eranikus EMOTE_ERANIKUS_REDEEM'),
(-1000698,'%s falls to one knee.',0,2,0,0,'tyrande EMOTE_TYRANDE_KNEEL'),
(-1000699,'Praise be to Elune... Eranikus is redeemed.',0,1,0,0,'tyrande SAY_TYRANDE_REDEEMED'),
(-1000700,'For so long, I was lost... The Nightmare\'s corruption had consumed me... And now, you... all of you.. you have saved me. Released me from its grasp.',0,0,0,0,'eranikus SAY_REDEEMED_1'),
(-1000701,'But... Malfurion, Cenarius, Ysera... They still fight. They need me. I must return to the Dream at once.', 0,0,0,0,'eranikus SAY_REDEEMED_2'),
(-1000702,'My lady, I am unworthy of your prayer. Truly, you are an angel of light. Please, assist me in returning to the barrow den so that I may return to the Dream. I like Malfurion, also have a love awaiting me... I must return to her... to protect her...', 0,0,0,0,'eranikus SAY_REDEEMED_3'),
(-1000703,'And heroes... I hold that which you seek. May it once more see the evil dissolved. Remulos, see to it that our champion receives the shard of the Green Flight.',0,0,0,0,'eranikus SAY_REDEEMED_4'),
(-1000704,'It will be done, Eranikus. Be well, ancient one.',0,0,0,0,'remulos SAY_REMULOS_OUTRO_1'),
(-1000705,'Let us leave Nighthave, hero. Seek me out at the grove.',0,0,0,0,'remulos SAY_REMULOS_OUTRO_2'),
(-1000706,'Your world shall suffer an unmerciful end. The Nightmare comes for you!',0,0,0,0,'eranikus SAY_ERANIKUS_KILL'),

(-1000707,'This blue light... It\'s strange. What do you think it means?',0,0,0,0,'Ranshalla SAY_ENTER_OWL_THICKET'),
(-1000708,'We\'ve found it!',0,0,0,0,'Ranshalla SAY_REACH_TORCH_1'),
(-1000709,'Please, light this while I am channeling',0,0,0,0,'Ranshalla SAY_REACH_TORCH_2'),
(-1000710,'This is the place. Let\'s light it.',0,0,0,0,'Ranshalla SAY_REACH_TORCH_3'),
(-1000711,'Let\'s find the next one.',0,0,0,0,'Ranshalla SAY_AFTER_TORCH_1'),
(-1000712,'We must continue on now.',0,0,0,0,'Ranshalla SAY_AFTER_TORCH_2'),
(-1000713,'It is time for the final step; we must activate the altar.',0,0,0,0,'Ranshalla SAY_REACH_ALTAR_1'),
(-1000714,'I will read the words carved into the stone, and you must find a way to light it.',0,0,0,0,'Ranshalla SAY_REACH_ALTAR_2'),
(-1000715,'The altar is glowing! We have done it!',0,0,0,0,'Ranshalla SAY_RANSHALLA_ALTAR_1'),
(-1000716,'What is happening? Look!',0,0,0,0,'Ranshalla SAY_RANSHALLA_ALTAR_2'),
(-1000717,'It has been many years...',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_3'),
(-1000718,'Who has disturbed the altar of the goddess?',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_4'),
(-1000719,'Please, priestesses, forgive us for our intrusion. We do not wish any harm here.',0,0,0,0,'Ranshalla SAY_RANSHALLA_ALTAR_5'),
(-1000720,'We only wish to know why the wildkin guard this area...',0,0,0,0,'Ranshalla SAY_RANSHALLA_ALTAR_6'),
(-1000721,'Enu thora\'serador. This is a sacred place.',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_7'),
(-1000722,'We will show you...',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_8'),
(-1000723,'Look above you; thara dormil dorah...',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_9'),
(-1000724,'This gem once allowed direct communication with Elune, herself.',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_10'),
(-1000725,'Through the gem, Elune channeled her infinite wisdom...',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_11'),
(-1000726,'Realizing that the gem needed to be protected, we turned to the goddess herself.',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_12'),
(-1000727,'Soon after, we began to have visions of a creature... A creature with the feathers of an owl, but the will and might of a bear...',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_13'),
(-1000728,'It was on that day that the wildkin were given to us. Fierce guardians, the goddess assigned the wildkin to protect all of her sacred places.',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_14'),
(-1000729,'Anu\'dorini Talah, Ru shallora enudoril.',0,0,0,0,'Voice of Elune SAY_VOICE_ALTAR_15'),
(-1000730,'But now, many years later, the wildkin have grown more feral, and without the guidance of the goddess, they are confused...',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_16'),
(-1000731,'Without a purpose, they wander... But many find their way back to the sacred areas that they once were sworn to protect.',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_17'),
(-1000732,'Wildkin are inherently magical; this power was bestowed upon them by the goddess.',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_18'),
(-1000733,'Know that wherever you might find them in the world, they are protecting something of importance, as they were entrusted to do so long ago.',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_19'),
(-1000734,'Please, remember what we have shown you...',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_20'),
(-1000735,'Farewell.',0,0,0,0,'Priestess of Elune SAY_PRIESTESS_ALTAR_21'),
(-1000736,'Thank you for you help, $n. I wish you well in your adventures.',0,0,0,0,'Ranshalla SAY_QUEST_END_1'),
(-1000737,'I want to stay here and reflect on what we have seen. Please see Erelas and tell him what we have learned.',0,0,0,0,'Ranshalla SAY_QUEST_END_2'),
(-1000738,'%s begins chanting a strange spell...',0,2,0,0,'Ranshalla EMOTE_CHANT_SPELL'),
(-1000739,'Remember, I need your help to properly channel. I will ask you to aid me several times in our path, so please be ready.',0,0,0,0,'Ranshalla SAY_QUEST_START'),

(-1000740,'We must act quickly or shall be lost!',0,0,0,1,'SAY_ANACHRONOS_INTRO_1'),
(-1000741,'My forces cannot overcome the Qiraji defenses. We will not be able to get close enough to place our precious barrier, dragon.',0,0,0,0,'SAY_FANDRAL_INTRO_2'),
(-1000742,'There is a way...',0,0,0,22,'SAY_MERITHRA_INTRO_3'),
(-1000743,'%s nods knowingly.',0,2,0,0,'EMOTE_ARYGOS_NOD'),
(-1000744,'Aye, Fandral, remember these words: Let not your grief guide your faith. These thoughts you hold... dark places you go, night elf.Absolution cannot be had through misguided vengeance.',0,0,0,1,'SAY_CAELESTRASZ_INTRO_4'),
(-1000745,'%s glances at her compatriots.',0,2,0,0,'EMOTE_MERITHRA_GLANCE'),
(-1000746,'We will push him back, Anachronos. This is wow. Uphold your end of this task. Let not your hands falter as you seal our fates behind the barrier.',0,0,0,1,'SAY_MERITHRA_INTRO_5'),
(-1000747,'Succumb to the endless dream, little ones. Let it comsume you!',0,1,0,22,'SAY_MERITHRA_ATTACK_1'),
(-1000748,'Anachronos, the diversion will give you an the young druid time enough to seal the gate. Do not falter. Now, let us see how they deal with chaotic magic.',0,0,0,1,'SAY_ARYGOS_ATTACK_2'),
(-1000749,'Let them feelt the wrath of the blue flight! May Malygos protect me!',0,1,0,22,'SAY_ARYGOS_ATTACK_3'),
(-1000750,'Do not forget sacrifices made on this day, night elf. We have all suffered immensely at the hands of these beasts.',0,0,0,1,'SAY_CAELESTRASZ_ATTACK_4'),
(-1000751,'Alexstrasza, give me the resolve to drive your enemies back.',0,1,0,22,'SAY_CAELESTRASZ_ATTACK_5'),
(-1000752,'NOW,STAGHELM! WE GO NOW! Prepare your magic!',0,0,0,22,'SAY_ANACHRONOS_SEAL_1'),
(-1000753,'It is done, dragon. Lead the way!',0,0,0,25,'SAY_FANDRAL_SEAL_2'),
(-1000754,'Stay close.',0,0,0,0,'SAY_ANACHRONOS_SEAL_3'),
(-1000755,'The sands of time will halt, but only for a moment! I will now conjure the barrier.',0,0,0,0,'SAY_ANACHRONOS_SEAL_4'),
(-1000756,'FINISH THE SPELL, STAGHELM! I CANNOT HOLD THE GLYPHS OF WARDING IN PLACE MUCH LONGER! CALL FORTH THE ROOTS!', 0,0,0,0,'SAY_ANACHRONOS_SEAL_5'),
(-1000757,'Ancient ones guide my hand... Wake from your slumber! WAKE AND SEAL THIS CURSED PLACE!',0,0,0,0, 'SAY_FANDRAL_SEAL_6'),
(-1000758,'%s falls to one knee - exhausted.',0,2,0,0,'EMOTE_FANDRAL_EXHAUSTED'),
(-1000759,'It... It is over, Lord Staghelm. We are victorious. Albeit the cost for this victory was great.',0,0,0,1,'SAY_ANACHRONOS_EPILOGUE_1'),
(-1000760,'There is but one duty that remains...',0,0,0,1,'SAY_ANACHRONOS_EPILOGUE_2'),
(-1000761,'Before I leave this place, I make one final offering for you, Lord Staghelm. Should a time arise in which you must gain entry to this accursed fortress, use the scepter of the shifting sands on the sacred gong. The magic holding the barrier together will dissipate an the horrors of the Ahn\'Qiraj will be unleashed upon the world once more.',0,0,0,1,'SAY_ANACHRONOS_EPILOGUE_3'),
(-1000762,'%s hands the Scepter of the Shifting Sands to $N.',0,2,0,0,'EMOTE_ANACHRONOS_SCEPTER'),
(-1000763,'After the savagery that my people have witnessed and felt, you expect me to accept another burden, dragon? Surely you are mad.',0,0,0,1,'SAY_FANDRAL_EPILOGUE_4'),
(-1000764,'I want nothing to do with Silithus, the Qiraji and least of all, any damed dragons!',0,0,0,1,'SAY_FANDRAL_EPILOGUE_5'),
(-1000765,'%s hurls the Scepter of the Shifting Sands into the barrier, shattering it.',0,2,0,0,'EMOTE_FANDRAL_SHATTER'),
(-1000766,'Lord Staghelm, where are you going? You would shatter our bond for the sake of pride?',0,0,0,1,'SAY_ANACHRONOS_EPILOGUE_6'),
(-1000767,'My son\'s soul will find no comfort in this hollow victory, dragon! I will have him back. Though it takes a millenia. I WILL have my son back!',0,0,0,1,'SAY_FANDRAL_EPILOGUE_7'),
(-1000768,'%s shakes his head in disappointment.',0,2,0,25,'EMOTE_ANACHRONOS_DISPPOINTED'),
(-1000769,'%s kneels down to pickup the fragments of the shattered scepter.',0,2,0,0,'EMOTE_ANACHRONOS_PICKUP'),
(-1000770,'And now you know all that there is to know, mortal...',0,0,0,0,'SAY_ANACHRONOS_EPILOGUE_8'),

(-1000771,'Let\'s go $N!',0,0,0,0,'Feero Ironhand SAY_QUEST_START'),
(-1000772,'It looks like we\'re in trouble. Look lively, here they come!',0,0,0,0,'Feero Ironhand SAY_FIRST_AMBUSH_START'),
(-1000773,'Assassins from that cult you found... Let\'s get moving before someone else finds us out here.',0,0,0,0,'Feero Ironhand SAY_FIRST_AMBUSH_END'),
(-1000774,'Hold! I sense an evil presence... Undead!',0,0,0,0,'Feero Ironhand SAY_SECOND_AMBUSH_START'),
(-1000775,'A $C! Slaying him would please the master. Attack!',0,0,0,0,'Forsaken Scout SAY_SCOUT_SECOND_AMBUSH'),
(-1000776,'They\'re coming out of the woodwork today. Let\'s keep moving or we may find more things that want me dead.',0,0,0,0,'Feero Ironhand SAY_SECOND_AMBUSH_END'),
(-1000777,'These three again?',0,0,0,0,'Feero Ironhand SAY_FINAL_AMBUSH_START'),
(-1000778,'Not quite so sure of yourself without the Purifier, hm?',0,0,0,0,'Balizar the Umbrage SAY_BALIZAR_FINAL_AMBUSH'),
(-1000779,'I\'ll finish you off for good this time!',0,0,0,0,'Feero Ironhand SAY_FINAL_AMBUSH_ATTACK'),
(-1000780,'Well done! I should be fine on my own from here. Remember to talk to Delgren when you return to Maestra\'s Post in Ashenvale.',0,0,0,0,'Feero Ironhand SAY_QUEST_END'),

(-1000781,'I knew Lurielle would send help! Thank you, friend, and give Lurielle my thanks as well!',0,0,0,0,'Chill Nymph SAY_FREE_1'),
(-1000782,'Where am I? What happend to me? You... you freed me?',0,0,0,0,'Chill Nymph SAY_FREE_2'),
(-1000783,'Thank you. I thought I would die without seeing my sisters again!',0,0,0,0,'Chill Nymph SAY_FREE_3'),

(-1000784,'Thanks $N. Now let\'s get out of here!',0,0,0,0,'melizza SAY_MELIZZA_START'),
(-1000785,'We made it! Thanks again! I\'m going to run ahead!',0,0,0,0,'melizza SAY_MELIZZA_FINISH'),
(-1000786,'Hey Hornizz! I\'m back! And there are some people behind me who helped me out of a jam.',0,0,0,1,'melizza SAY_MELIZZA_1'),
(-1000787,'We\'re going to have to scratch the Maraudines off our list. Too hard to work with...',0,0,0,1,'melizza SAY_MELIZZA_2'),
(-1000788,'Well, I\'m off to the Gelkis. They\'re not as dumb as the Maraudines, but they\'re more reasonable.',0,0,0,3,'melizza SAY_MELIZZA_3'),

(-1000789,'Well, now or never I suppose. Remember, once we get to the road safety, return to Terenthis to let him know we escaped.',0,0,0,0,'volcor SAY_START'),
(-1000790,'We made it, My friend. Remember to find Terenthis and let him know we\'re safe. Thank you again.',0,0,0,0,'volcor SAY_END'),
(-1000791,'Here they come.',0,0,0,0,'volcor SAY_FIRST_AMBUSH'),
(-1000792,'We can overcome these foul creatures.',0,0,0,0,'volcor SAY_AGGRO_1'),
(-1000793,'We shall earn our deaths at the very least!',0,0,0,0,'volcor SAY_AGGRO_2'),
(-1000794,'Don\'t give up! Fight, to the death!',0,0,0,0,'volcor SAY_AGGRO_3'),

(-1000795,'Ow! Ok, I\'ll get back to work, $N!',0,0,1,0,'Lazy Peon SAY_PEON_AWOKEN'),
(-1000796,'REUSE_ME',0,0,0,0,'REUSE_ME'),

(-1000797,'%s squawks and heads toward Veil Shalas. Hurry and follow!',0,2,0,0,'skywing SAY_SKYWING_START'),
(-1000798,'%s pauses briefly before the tree and then heads inside.',0,2,0,0,'skywing SAY_SKYWING_TREE_DOWN'),
(-1000799,'%s seems to be looking for something. He wants you to follow.',0,2,0,0,'skywing SAY_SKYWING_TREE_UP'),
(-1000800,'%s flies to the platform below! You\'d better jump if you want to keep up. Hurry!',0,2,0,0,'skywing SAY_SKYWING_JUMP'),
(-1000801,'%s bellows a loud squawk!',0,2,0,0,'skywing SAY_SKYWING_SUMMON'),
(-1000802,'Free at last from that horrible curse! Thank you! Please send word to Rilak the Redeemed that I am okay. My mission lies in Skettis. Terokk must be defeated!',0,0,0,0,'skywing SAY_SKYWING_END'),

(-1000803,'You do not fight alone, %n! Together, we will banish this spawn of hellfire!',0,1,0,0,'Oronok SAY_ORONOK_TOGETHER'),
(-1000804,'We will fight when you are ready.',0,0,0,0, 'Oronok SAY_ORONOK_READY'),
(-1000805,'We will set the elements free of your grasp by force!',0,1,0,0,'Oronok SAY_ORONOK_ELEMENTS'),
(-1000806,'What say the elements, Torlok? I only hear silence.',0,0,0,1,'Oronok SAY_ORONOK_EPILOGUE_1'),
(-1000807,'I hear what you hear, brother. Look behind you...',0,0,0,1,'Torlok SAY_TORLOK_EPILOGUE_2'),
(-1000808,'They are redeemed! Then we have won?',0,0,0,1,'Oronok SAY_ORONOK_EPILOGUE_3'),
(-1000809,'It is now as it should be, shaman. You have done well.',0,0,0,0,'Spirit of Earth SAY_EARTH_EPILOGUE_4'),
(-1000810,'Yes... Well enough for the elements that are here, but the cipher is known to another... The spirits of fire are in turmoil... If this force is not stopped, the world where these mortals came from will cease.',0,0,0,0,'Spirit of Fire SAY_FIRE_EPILOGUE_5'),
(-1000811,'Farewell, mortals... The earthmender knows what fire feels...',0,0,0,0, 'Spirit of Earth SAY_EARTH_EPILOGUE_6'),
(-1000812,'We leave, Torlok. I have only one request...',0,0,0,1,'Oronok SAY_ORONOK_EPILOGUE_7'),
(-1000813,'The Torn-heart men give their weapons to Earthmender Torlok.',0,2,0,0,'Torlok EMOTE_GIVE_WEAPONS'),
(-1000814,'Give these to the heroes that made this possible.',0,0,0,1,'Oronok SAY_ORONOK_EPILOGUE_8'),

(-1000815,'Be healed!',0,1,0,0,'Eris Havenfire SAY_PHASE_HEAL'),
(-1000816,'We are saved! The peasants have escaped the Scourge!',0,1,0,0,'Eris Havenfire SAY_EVENT_END'),
(-1000817,'I have failed once more...',0,1,0,0,'Eris Havenfire SAY_EVENT_FAIL_1'),
(-1000818,'I now return to whence I came, only to find myself here once more to relive the same epic tragedy.',0,0,0,0,'Eris Havenfire SAY_EVENT_FAIL_2'),
(-1000819,'The Scourge are upon us! Run! Run for your lives!',0,1,0,0,'Peasant SAY_PEASANT_APPEAR_1'),
(-1000820,'Please help us! The Prince has gone mad!',0,1,0,0,'Peasant SAY_PEASANT_APPEAR_2'),
(-1000821,'Seek sanctuary in Hearthglen! It is our only hope!',0,1,0,0,'Peasant SAY_PEASANT_APPEAR_3'),

(-1000822,'The signal has been sent. He should be arriving shortly.',0,0,0,1,'squire rowe SAY_SIGNAL_SENT'),
(-1000823,'Yawww!',0,0,0,35,'reginald windsor SAY_DISMOUNT'),
(-1000824,'I knew you would come, $N. It is good to see you again, friend.',0,0,0,1,'reginald windsor SAY_WELCOME'),

(-1000825,'On guard, friend. The lady dragon will not give in without a fight.',0,0,0,1,'reginald windsor SAY_QUEST_ACCEPT'),
(-1000826,'As was fated a lifetime ago in Karazhan, monster - I come - and with me I bring justice.',0,6,0,22,'reginald windsor SAY_GET_READY'),
(-1000827,'Seize him! Seize the worthless criminal and his allies!',0,6,0,0,'prestor SAY_GONNA_DIE'),
(-1000828,'Reginald, you know that I cannot let you pass.',0,0,0,1,'jonathan SAY_DIALOG_1'),
(-1000829,'You must do what you think is right, Marcus. We served together under Turalyon. He made us both the men that we are today. Did he err with me? Do you truly believe my intent is to cause harm to our alliance? Would I shame our heroes?',0,0,0,1,'reginald windsor SAY_DIALOG_2'),
(-1000830,'Holding me here is not the right decision, Marcus.',0,0,0,1,'reginald windsor SAY_DIALOG_3'),
(-1000831,'%s appears lost in contemplation.',0,2,0,0,'jonathan EMOTE_CONTEMPLATION'),
(-1000832,'I am ashamed, old friend. I know not what I do anymore. It is not you that would dare bring shame to the heroes of legend - it is I. It is I and the rest of these corrupt politicians. They fill our lives with empty promises, unending lies.',0,0,0,1,'jonathan SAY_DIALOG_4'),
(-1000833,'We shame our ancestors. We shame those lost to us... forgive me, Reginald.',0,0,0,1,'jonathan SAY_DIALOG_5'),
(-1000834,'Dear friend, you honor them with your vigilant watch. You are steadfast in your allegiance. I do not doubt for a moment that you would not give as great a sacrifice for your people as any of the heroes you stand under.',0,0,0,1,'reginald windsor SAY_DIALOG_6'),
(-1000835,'Now, it is time to bring her reign to an end, Marcus. Stand down, friend.',0,0,0,1,'reginald windsor SAY_DIALOG_7'),
(-1000836,'Stand down! Can you not see that heroes walk among us?',0,0,0,5,'jonathan SAY_DIALOG_8'),
(-1000837,'Move aside! Let them pass!',0,0,0,5,'jonathan SAY_DIALOG_9'),
(-1000838,'Reginald Windsor is not to be harmed! He shall pass through untouched!',0,1,0,22,'jonathan SAY_DIALOG_10'),
(-1000839,'Go, Reginald. May the light guide your hand.',0,0,0,1,'jonathan SAY_DIALOG_11'),
(-1000840,'Thank you, old friend. You have done the right thing.',0,0,0,1,'reginald windsor SAY_DIALOG_12'),
(-1000841,'Follow me, friends. To Stormwind Keep!',0,0,0,0,'reginald windsor SAY_DIALOG_13'),
(-1000842,'Light be with you, sir.',0,0,0,66,'guard SAY_1'),
(-1000843,'We are but dirt beneath your feet, sir.',0,0,0,66,'guard SAY_2'),
(-1000844,'...nerves of thorium.',0,0,0,66,'guard SAY_3'),
(-1000845,'Make way!',0,0,0,66,'guard SAY_4'),
(-1000846,'A living legend...',0,0,0,66,'guard SAY_5'),
(-1000847,'A moment I shall remember for always.',0,0,0,66,'guard SAY_6'),
(-1000848,'You are an inspiration to us all, sir.',0,0,0,66,'guard SAY_7'),
(-1000849,'Be brave, friends. The reptile will thrash wildly. It is an act of desperation. When you are ready, give me the word.',0,0,0,25,'reginald windsor SAY_BEFORE_KEEP'),
(-1000850,'Onward!',0,0,0,5,'reginald windsor SAY_GO_TO_KEEP'),
(-1000851,'Majesty, run while you still can. She is not what you think her to be...',0,0,0,1,'reginald windsor SAY_IN_KEEP_1'),
(-1000852,'To the safe hall, your majesty.',0,0,0,1,'bolvar SAY_IN_KEEP_2'),
(-1000853,'The masquerade is over, Lady Prestor. Or should I call you by your true name... Onyxia...',0,0,0,25,'reginald windsor SAY_IN_KEEP_3'),
(-1000854,'%s laughs.',0,2,0,11,'prestor EMOTE_IN_KEEP_LAUGH'),
(-1000855,'You will be incarcerated and tried for treason, Windsor. I shall watch with glee as they hand down a guilty verdict and sentence you to death by hanging...',0,0,0,1,'prestor SAY_IN_KEEP_4'),
(-1000856,'And as your limp body dangles from the rafters, I shall take pleasure in knowing that a mad man has been put to death. After all, what proof do you have? Did you expect to come in here and point your fingers at royalty and leave unscathed?',0,0,0,6,'prestor SAY_IN_KEEP_5'),
(-1000857,'You will not escape your fate, Onyxia. It has been prophesied - a vision resonating from the great halls of Karazhan. It ends now...',0,0,0,1,'reginald windsor SAY_IN_KEEP_6'),
(-1000858,'%s reaches into his pack and pulls out the encoded tablets...',0,2,0,0,'reginald windsor EMOTE_IN_KEEP_REACH'),
(-1000859,'The Dark Irons thought these tablets to be encoded. This is not any form of coding, it is the tongue of ancient dragon.',0,0,0,1,'reginald windsor SAY_IN_KEEP_7'),
(-1000860,'Listen, dragon. Let the truth resonate throughout these halls.',0,0,0,1,'reginald windsor SAY_IN_KEEP_8'),
(-1000861,'%s reads from the tablets. Unknown, unheard sounds flow through your consciousness',0,2,0,0,'reginald windsor EMOTE_IN_KEEP_READ'),
(-1000862,'%s gasps.',0,2,0,0,'bolvar EMOTE_IN_KEEP_GASP'),
(-1000863,'Curious... Windsor, in this vision, did you survive? I only ask because one thing that I can and will assure is your death. Here and now.',0,0,0,1,'onyxia SAY_IN_KEEP_9'),
(-1000864,'Dragon filth! Guards! Guards! Seize this monster!',0,1,0,22,'bolvar SAY_IN_KEEP_1'),
(-1000865,'Yesss... Guards, come to your lord\'s aid!',0,0,0,1,'onyxia SAY_IN_KEEP_10'),
(-1000866,'DO NOT LET HER ESCAPE!',0,0,0,1,'reginald windsor SAY_IN_KEEP_11'),
(-1000867,'Was this fabled, Windsor? If it was death that you came for then the prophecy has been fulfilled. May your consciousness rot in the Twisting Nether. Finish the rest of these meddlesome insects, children. Bolvar, you have been a pleasureable puppet.',0,0,0,0,'onyxia SAY_IN_KEEP_12'),
(-1000868,'You have failed him, mortalsss... Farewell!',0,1,0,0,'onyxia SAY_IN_KEEP_12'),
(-1000869,'Reginald... I... I am sorry.',0,0,0,0,'bolvar SAY_IN_KEEP_13'),
(-1000870,'Bol... Bolvar... the medallion... use...',0,0,0,0,'reginald windsor SAY_IN_KEEP_14'),
(-1000871,'%s dies.',0,2,0,0,'reginald windsor EMOTE_IN_KEEP_DIE'),
(-1000872,'%s hisses',0,2,0,0,'reginald windsor EMOTE_GUARD_TRANSFORM'),

(-1000873,'I know the way, insect. There is no need to prod me as if I were cattle.',0,0,0,1,'grark SAY_START'),
(-1000874,'Surely you do not think that you will get away with this incursion. They will come for me and you shall pay for your insolence.',0,0,0,1,'grark SAY_PAY'),
(-1000875,'RUN THEM THROUGH BROTHERS!',0,0,0,5,'grark SAY_FIRST_AMBUSH_START'),
(-1000876,'I doubt you will be so lucky the next time you encounter my brethren.',0,0,0,1,'grark SAY_FIRST_AMBUSH_END'),
(-1000877,'They come for you, fool!',0,0,0,5,'grark SAY_SEC_AMBUSH_START'),
(-1000878,'What do you think you accomplish from this, fool? Even now, the Blackrock armies make preparations to destroy your world.',0,0,0,1,'grark SAY_SEC_AMBUSH_END'),
(-1000879,'On darkest wing they fly. Prepare to meet your end!',0,0,0,5,'grark SAY_THIRD_AMBUSH_START'),
(-1000880,'The worst is yet to come!',0,0,0,1,'grark SAY_THIRD_AMBUSH_END'),
(-1000881,'%s laughs.',0,2,0,11,'grark EMOTE_LAUGH'),
(-1000882,'Time to make your final stand, Insect.',0,0,0,0,'grark SAY_LAST_STAND'),
(-1000883,'Kneel, Grark',0,0,0,1,'lexlort SAY_LEXLORT_1'),
(-1000884,'Grark Lorkrub, you have been charged and found guilty of treason against Horde. How you plead is unimportant. High Executioner Nuzrak, step forward.',0,0,0,1,'lexlort SAY_LEXLORT_2'),
(-1000885,'%s raises his massive axe over Grark.',0,2,0,27,'nuzark EMOTE_RAISE_AXE'),
(-1000886,'%s raises his hand and then lowers it.',0,2,0,0,'lexlort EMOTE_LOWER_HAND'),
(-1000887,'End him...',0,0,0,0,'lexlort SAY_LEXLORT_3'),
(-1000888,'You, soldier, report back to Kargath at once!',0,0,0,1,'lexlort SAY_LEXLORT_4'),
(-1000889,'%s submits.',0,2,0,0,'grark EMOTE_SUBMIT'),
(-1000890,'You have come to play? Then let us play!',0,0,0,0,'grark SAY_AGGRO'),

(-1000891,'Let\'s do this... Just keep me covered and I\'ll deliver the package.',0,0,0,0,'demolitionist SAY_INTRO'),
(-1000892,'I\'m under attack! I repeat, I am under attack!',0,0,0,0,'demolitionist SAY_ATTACK_1'),
(-1000893,'I need to find a new line of work.',0,0,0,0,'demolitionist SAY_ATTACK_2'),
(-1000894,'By the second sun of K\'aresh, look at this place! I can only imagine what Salhadaar is planning. Come on, let\'s keep going.',0,0,0,1,'demolitionist SAY_STAGING_GROUNDS'),
(-1000895,'With this much void waste and run off, a toxic void horror can\'t be too far behind.',0,0,0,0,'demolitionist SAY_TOXIC_HORROR'),
(-1000896,'Look there, fleshling! Salhadaar\'s conduits! He\'s keeping well fed...',0,0,0,1,'demolitionist SAY_SALHADAAR'),
(-1000897,'Alright, keep me protected while I plant this disruptor. This shouldn\'t take very long...',0,0,0,0,'demolitionist SAY_DISRUPTOR'),
(-1000898,'Protect the conduit! Stop the intruders!',0,0,0,0,'nexus stalkers SAY_PROTECT'),
(-1000899,'Done! Back up! Back up!',0,0,0,0,'demolitionist SAY_FINISH_1'),
(-1000900,'Looks like my work here is done. Report to the holo-image of Ameer over at the transporter.',0,0,0,1,'demolitionist SAY_FINISH_2'),

(-1000901,'Thanks, friend. Will you help me get out of here?',0,0,0,1,'vanguard SAY_VANGUARD_INTRO'),
(-1000902,'We\'re not too far from the Protectorate Watch Post, $N. This way!',0,0,0,1,'vanguard SAY_VANGUARD_START'),
(-1000903,'Commander! This fleshling rescued me!',0,0,0,0,'vanguard SAY_VANGUARD_FINISH'),
(-1000904,'%s salutes $N.',0,2,0,0,'vanguard EMOTE_VANGUARD_FINISH'),

(-1000905,'Ok, let\'s go!!',0,0,0,1,'therylune SAY_THERYLUNE_START'),
(-1000906,'I can make it the rest of the way. $N. THANKS!',0,0,0,1,'therylune SAY_THERYLUNE_START'),

(-1000907,'%s sniffs at the air. A tuber is near!',0,2,0,0,'domesticated felboar EMOTE_SNIFF_AIR'),
(-1000908,'%s starts to dig.',0,2,0,0,'domesticated felboar EMOTE_START_DIG'),
(-1000909,'%s squeals with glee at its discovery.',0,2,0,0,'domesticated felboar EMOTE_SQUEAL'),

(-1000910,'Shall we begin, my friend?',0,0,0,0,'anchorite truuen SAY_BEGIN'),
(-1000911,'This area is known to be full of foul Scourge. You may want to take a moment to prepare any defenses at your disposal.',0,0,0,0,'anchorite truuen SAY_FIRST_STOP'),
(-1000912,'Very well, let us continue.',0,0,0,0,'anchorite truuen SAY_CONTINUE'),
(-1000913,'Beware! We are attacked!',0,0,0,0,'anchorite truuen SAY_FIRST_ATTACK'),
(-1000914,'It must be the purity of the Mark of the Lightbringer that is drawing forth the Scourge to us. We must proceed with caution lest we overwhelmed!',0,0,0,0,'anchorite truuen SAY_PURITY'),
(-1000915,'We are beset upon again! Defend yourself!',0,0,0,0,'anchorite truuen SAY_SECOND_ATTACK'),
(-1000916,'The land truly needs to be cleansed by the Light! Let us continue on the tomb. It isn\'t far now.',0,0,0,0,'anchorite truuen SAY_CLEANSE'),
(-1000917,'Be welcome, friends!',0,0,0,0,'high priest thel\'danis SAY_WELCOME'),
(-1000918,'Thank you for coming in remembrance of me. Your efforts in recovering that symbol, while unnecessary, are certainly touching to an old man\'s heart.',0,0,0,0,'ghost of uther SAY_EPILOGUE_1'),
(-1000919,'Please, rise my friend. Keep the Blessing as a symbol of the strength of the Light and how heroes long gone might once again rise in each of us to inspire.',0,0,0,0,'ghost of uther SAY_EPILOGUE_2'),

(-1000920,'%s turns to face you.',0,2,0,0,'lich_king_wyrmskull EMOTE_LICH_KING_FACE'),
(-1000921,'Shamanism has brought you here... Its scent permeates the air. *The Lich King laughs* I was once a shaman.',14742,0,0,0,'lich_king_wyrmskull SAY_LICH_KING_1'),
(-1000922,'Shall we prepare it for you, my lord?',0,0,0,0,'valkyr_soulclaimer SAY_PREPARE'),
(-1000923,'No, minion. This one is not ready.',14743,0,0,0,'lich_king_wyrmskull SAY_LICH_KING_2'),
(-1000924,'Do you feel it, mortal? Death seeps through me, enveloping all that I touch. With just a snap of my finger your soul will languish in damnation for all eternity.',14744,0,0,0,'lich_king_wyrmskull SAY_LICH_KING_3'),
(-1000925,'But... It is not yet your time to serve the Lich King. Yes, a greater destiny awaits you. Power... You must become more powerful before you are to serve me.',14745,0,0,0,'lich_king_wyrmskull SAY_LICH_KING_4'),
(-1000926,'Now watch, val\'kyr. Observe as I apply pressure. Can you see that it is not yet ripe? Watch as it pops and falls lifeless to the floor.',14746,0,0,0,'lich_king_wyrmskull SAY_LICH_KING_5'),
(-1000927,'Persistence or stupidity? It matters not. Let this be a lesson learned, mortal!',14747,0,0,0,'lich_king_wyrmskull SAY_LICH_KING_6'),

(-1000928,'%s motions for silence.',0,3,0,25,'king_ymiron EMOTE_KING_SILENCE'),
(-1000929,'Vrykul, your king implores you listen!',0,1,0,22,'king_ymiron SAY_KING_YMIRON_SPEECH_1'),
(-1000930,'The Gods have abandonned us!',0,1,0,22,'king_ymiron SAY_KING_YMIRON_SPEECH_2'),
(-1000931,'The crowd gasps in horror.',0,2,0,0,'king_ymiron EMOTE_YMIRON_CROWD_1'),
(-1000932,'Even now, in our darkest hour, they mock us!',0,1,0,22,'king_ymiron SAY_KING_YMIRON_SPEECH_3'),
(-1000933,'Where are the titans in out time of greatest need? Our women birth abberations - disfigured runts unable to even stand on their own! Weak and ugly... Useless...',0,1,0,22,'king_ymiron SAY_KING_YMIRON_SPEECH_4'),
(-1000934,'Ymiron has toiled. Long have I sat upon my throne and thought hard of our plight. There is only one answer... One reason...',0,1,0,22,'king_ymiron SAY_KING_YMIRON_SPEECH_5'),
(-1000935,'For who but the titans themselves could bestow such a curse? What could have such power?',0,1,0,22,'king_ymiron SAY_KING_YMIRON_SPEECH_6'),
(-1000936,'And the answer is nothing... For it is the titans who have cursed us!',0,1,0,22,'king_ymiron SAY_KING_YMIRON_SPEECH_7'),
(-1000937,'The crowd clamours.',0,2,0,0,'king_ymiron EMOTE_YMIRON_CROWD_2'),
(-1000938,'On this day all Vrykul will shed their old beliefs! We denounce our old gods! All Vrykul will pledge their allegiance to Ymiron! Ymiron will protect our noble race!',0,1,0,22,'king_ymiron SAY_KING_YMIRON_SPEECH_8'),
(-1000939,'The crowd cheers.',0,2,0,0,'king_ymiron EMOTE_YMIRON_CROWD_3'),
(-1000940,'And now my first decree upon the Vrykul! All malformed infants born of Vrykul mother and father are to be destroyed upon birth! Our blood must remain pure always! Those found in violation of Ymiron\'s decree will be taken to Gjalerbron for execution!',0,1,0,22,'king_ymiron SAY_KING_YMIRON_SPEECH_9'),
(-1000941,'Vrykul must remain pure!',0,0,0,0,'king_ymiron_crowd SAY_YMIRON_CROWD_1'),
(-1000942,'Show the aberrations no mercy, Ymiron!',0,0,0,0,'king_ymiron_crowd SAY_YMIRON_CROWD_2'),
(-1000943,'Show them mercy, my king! They are of our flesh and blood!',0,0,0,0,'king_ymiron_crowd SAY_YMIRON_CROWD_3'),
(-1000944,'They weaken us! Our strength is dilluted by their very existence! Destroy them all!',0,0,0,0,'king_ymiron_crowd SAY_YMIRON_CROWD_4'),
(-1000945,'All hail our glorious king, Ymiron!',0,0,0,0,'king_ymiron_crowd SAY_YMIRON_CROWD_5'),
(-1000946,'The King is going to speak!',0,0,0,0,'king_ymiron_crowd SAY_YMIRON_CROWD_6'),
(-1000947,'Let him speak! Be silent!',0,0,0,0,'king_ymiron_crowd SAY_YMIRON_CROWD_7'),

(-1000948,'Well then, let\'s get this started. The longer we\'re here, the more damage the undead could be doing back in Hilsbrad.',0,0,0,0,'kinelory SAY_START'),
(-1000949,'All right, this is where we really have to be on our paws. Be ready!',0,0,0,0,'kinelory SAY_REACH_BOTTOM'),
(-1000950,'Attack me if you will, but you won\'t stop me from getting back to Quae.',0,0,0,0,'kinelory SAY_AGGRO_KINELORY'),
(-1000951,'You have my word that I shall find a use for your body after I\'ve killed you, Kinelory.',0,0,0,0,'jorell SAY_AGGRO_JORELL'),
(-1000952,'Watch my rear! I\'ll see what I can find in all this junk...',0,0,0,0,'kinelory SAY_WATCH_BACK'),
(-1000953,'%s begins rummaging through the apothecary\'s belongings.',0,2,0,0,'kinelory EMOTE_BELONGINGS'),
(-1000954,'I bet Quae\'ll think this is important. She\'s pretty knowledgeable about these things--no expert, but knowledgable.',0,0,0,0,'kinelory SAY_DATA_FOUND'),
(-1000955,'Okay, let\'s get out of here quick quick! Try and keep up. I\'m going to make a break for it.',0,0,0,0,'kinelory SAY_ESCAPE'),
(-1000956,'We made it! Quae, we made it!',0,0,0,0,'kinelory SAY_FINISH'),
(-1000957,'%s hands her pack to Quae.',0,2,0,0,'kinelory EMOTE_HAND_PACK'),

(-1000958,'Ok, let\'s get started.',0,0,0,0,'stinky ignatz SAY_STINKY_BEGIN'),
(-1000959,'Now let\'s look for the herb.',0,0,0,0,'stinky ignatz SAY_STINKY_FIRST_STOP'),
(-1000960,'Help! The beast is on me!',0,0,0,0,'stinky ignatz SAY_AGGRO_1'),
(-1000961,'Help! I\'m under attack!',0,0,0,0,'stinky ignatz SAY_AGGRO_2'),
(-1000962,'I can make it from here. Thanks, $N! And talk to my employer about a reward!',0,0,0,0,'stinky ignatz SAY_STINKY_END'),

(-1000963,'%s looks at you for a moment, then motions for you to follow.',0,2,0,0,'cenarion sparrowhawk EMOTE_FOLLOW'),
(-1000964,'%s surveys the ground for the buried raven stones.',0,2,0,0,'cenarion sparrowhawk EMOTE_SURVEY'),
(-1000965,'%s locates a buried raven stone.',0,2,0,0,'cenarion sparrowhawk EMOTE_LOCATE'),

(-1000966,'I WILL CRUSH YOU LIKE A GNAT!',0,1,0,0,'reth\'hedron SAY_LOW_HP'),
(-1000967,'You will regret this, mortal! Reth\'hedron will return... I will have my vengeance!',0,1,0,53,'reth\'hedron SAY_EVENT_END'),

(-1000968,'Very well. Before we head down there, take a moment to prepare yourself.',0,0,0,1,'drijya SAY_DRIJYA_START'),
(-1000969,'Let\'s proceed at a brisk pace.',0,0,0,0,'drijya SAY_DRIJYA_1'),
(-1000970,'We\'ll start at that first energy pylon, straight ahead. Remember, try to keep them off of me.',0,0,0,1,'drijya SAY_DRIJYA_2'),
(-1000971,'Keep them off me!',0,0,0,0,'drijya SAY_DRIJYA_3'),
(-1000972,'I\'m done with this pylon. On to the next.',0,0,0,1,'drijya SAY_DRIJYA_4'),
(-1000973,'Alright, pylon two down. Now for the heat mainfold.',0,0,0,1,'drijya SAY_DRIJYA_5'),
(-1000974,'That should do it. The teleporter should blow any second now!',0,0,0,5,'drijya SAY_DRIJYA_6'),
(-1000975,'Ok, let\'s get out of here!',0,0,0,1,'drijya SAY_DRIJYA_7'),
(-1000976,'Thank you, $n! I couldn\'t have done it without you. You\'ll let Gahruj know?',0,0,0,1,'drijya SAY_DRIJYA_COMPLETE'),

(-1000977,'Oh, it\'s on now! But you thought I\'d be alone too, huh?!',0,0,0,0,'tapoke slim jahn SAY_AGGRO'),
(-1000978,'Okay, okay! No need to get all violent. I\'ll talk. I\'ll talk!',0,0,0,20,'tapoke slim jahn SAY_DEFEAT'),
(-1000979,'Whoa! This is way more than what I bargained for, you\'re on your own, Slim!',0,0,0,0,'slim\'s friend SAY_FRIEND_DEFEAT'),
(-1000980,'I have a few notes from the job back at my place. I\'ll get them and then meet you back in the inn.',0,0,0,1,'tapoke slim jahn SAY_NOTES'),

(-1000981,'It is time. The rite of exorcism will now commence...',0,0,0,0,'anchorite barada SAY_EXORCISM_1'),
(-1000982,'Prepare yourself. Do not allow the ritual to be interrupted or we may lose our patient...',0,0,0,1,'anchorite barada SAY_EXORCISM_2'),
(-1000983,'Keep away. The fool is mine.',0,0,0,0,'colonel jules SAY_EXORCISM_3'),
(-1000984,'Back, foul beings of darkness! You have no power here!',0,0,0,0,'anchorite barada SAY_EXORCISM_4'),
(-1000985,'No! Not yet! This soul is ours!',0,0,0,0,'colonel jules SAY_EXORCISM_5'),
(-1000986,'Back! I cast you back... corrupter of faith! Author of pain! Do not return, or suffer the same fate as you did here today!',0,0,0,2,'anchorite barada SAY_EXORCISM_6'),
(-1000987,'I... must not...falter!',0,0,0,0,'anchorite barada SAY_EXORCISM_RANDOM_1'),
(-1000988,'Be cleansed with Light, human! Let not the demonic corruption overwhelm you.',0,0,0,0,'anchorite barada SAY_EXORCISM_RANDOM_2'),
(-1000989,'Back, foul beings of darkness! You have no power here!',0,0,0,0,'anchorite barada SAY_EXORCISM_RANDOM_3'),
(-1000990,'This is fruitless, draenei! You and your little helper cannot wrest control of this pathetic human. He is mine!',0,0,0,0,'colonel jules SAY_EXORCISM_RANDOM_4'),
(-1000991,'I see your ancestors, Anchorite! They writhe and scream in the darkness... they are with us!',0,0,0,0,'colonel jules SAY_EXORCISM_RANDOM_5'),
(-1000992,'I will tear your soul into morsels and slow roast them over demon fire!',0,0,0,0,'colonel jules SAY_EXORCISM_RANDOM_6'),

(-1000993,'It\'s on! $N, meet my fists. Fists, say hello to $N.',0,0,0,0,'dorius stonetender SAY_AGGRO_1'),
(-1000994,'I\'m about to open a can on this $N.',0,0,0,0,'dorius stonetender SAY_AGGRO_2'),

(-1000995,'Fhwoor go now, $N. Get ark, come back.',0,0,0,0,'fhwoor SAY_ESCORT_START'),
(-1000996,'Take moment... get ready.',0,0,0,0,'fhwoor SAY_PREPARE'),
(-1000997,'We go!',0,0,0,0,'fhwoor SAY_CAMP_ENTER'),
(-1000998,'Uh oh...',0,0,0,0,'fhwoor SAY_AMBUSH'),
(-1000999,'Ha ha, squishy naga!',0,0,0,0,'fhwoor SAY_AMBUSH_CLEARED'),
(-1001000,'Fhwoor do good!',0,0,0,0,'fhwoor SAY_ESCORT_COMPLETE'),

(-1001001,'We must leave before more are alerted.',0,0,0,0,'kurenai captive SAY_KUR_START'),
(-1001002,'It\'s an ambush! Defend yourself!',0,0,0,0,'kurenai captive SAY_KUR_AMBUSH_1'),
(-1001003,'We are surrounded!',0,0,0,0,'kurenai captive SAY_KUR_AMBUSH_2'),
(-1001004,'Up ahead is the road to Telaar. We will split up when we reach the fork as they will surely send more Murkblood after us. Hopefully one of us makes it back to Telaar alive.',0,0,0,1,'kurenai captive SAY_KUR_COMPLETE_1'),
(-1001005,'Farewell, stranger. Your heroics will be remembered by my people. Now, hurry to Telaar!',0,0,0,1,'kurenai captive SAY_KUR_COMPLETE_2'),

(-1001006,'Thanks for your help. Let\'s get out of here!',0,0,0,1,'skyguard prisoner SAY_ESCORT_START'),
(-1001007,'Let\'s keep moving. I don\'t like this place.',0,0,0,1,'skyguard prisoner SAY_AMBUSH_END'),
(-1001008,'Thanks again. Sergeant Doryn will be glad to hear he has one less scout to replace this week.',0,0,0,1,'skyguard prisoner SAY_ESCORT_COMPLETE'),
(-1001009,'Death to our enemies!',0,0,0,0,'skettis wing guard SAY_AMBUSH_1'),
(-1001010,'No one escapes Skettis!',0,0,0,0,'skettis wing guard SAY_AMBUSH_2'),
(-1001011,'Skettis prevails!',0,0,0,0,'skettis wing guard SAY_AMBUSH_3'),
(-1001012,'You\'ll go nowhere, Skyguard scum!',0,0,0,0,'skettis wing guard SAY_AMBUSH_4'),

(-1001013,'Right then, no time to waste. Let\'s get outa here!',0,0,0,1,'bonker togglevolt SAY_BONKER_START'),
(-1001014,'Here we go.',0,0,0,0,'bonker togglevolt SAY_BONKER_GO'),
(-1001015,'I AM NOT AN APPETIZER!',0,0,0,0,'bonker togglevolt SAY_BONKER_AGGRO'),
(-1001016,'I think it\'s up this way to the left. Let\'s go!',0,0,0,1,'bonker togglevolt SAY_BONKER_LEFT'),
(-1001017,'Ah, fresh air! I can get myself back to the airstrip from here. Be sure to tell Fizzcrank I\'m back and safe. Thanks so much, $N!',0,0,0,1,'sbonker togglevolt SAY_BONKER_COMPLETE'),

(-1001018,'On the move, men!',0,0,0,0,'kor\'kron squad leader SAY_HORDER_RUN'),
(-1001019,'Alright boys, let\'s do this!',0,0,0,0,'skybreaker squad leader SAY_ALLIANCE_RUN'),
(-1001020,'Incoming!',0,1,0,0,'squad leader SAY_AGGRO_1'),
(-1001021,'Ambush!',0,1,0,0,'squad leader SAY_AGGRO_2'),
(-1001022,'For the Horde!',0,1,0,0,'kor\'kron squad leader SAY_HORDE_AGGRO_1'),
(-1001023,'Time for some blood, men!',0,1,0,0,'kor\'kron squad leader SAY_HORDE_AGGRO_2'),
(-1001024,'Vrykul!',0,1,0,0,'kor\'kron squad leader SAY_HORDE_AGGRO_3'),
(-1001025,'Weapons out!',0,1,0,0,'kor\'kron squad leader SAY_HORDE_AGGRO_4'),
(-1001026,'Find some cover!',0,1,0,0,'skybreaker squad leader SAY_ALLIANCE_AGGRO_1'),
(-1001027,'Group up!',0,1,0,0,'skybreaker squad leader SAY_ALLIANCE_AGGRO_2'),
(-1001028,'On your feet, boys!',0,1,0,0,'skybreaker squad leader SAY_ALLIANCE_AGGRO_3'),
(-1001029,'Vrykul attack!',0,1,0,0,'skybreaker squad leader SAY_ALLIANCE_AGGRO_4'),
(-1001030,'Quickly, catch your breaths before we press for the gate!',0,0,0,0,'kor\'kron squad leader SAY_HORDE_BREAK'),
(-1001031,'On your feet, men! Move, move move!',0,0,0,0,'kor\'kron squad leader SAY_HORDE_BREAK_DONE'),
(-1001032,'Nice work! We can only rest a moment.',0,0,0,0,'skybreaker squad leader SAY_ALLIANCE_BREAK'),
(-1001033,'On your feet, boys! Move, move move!',0,0,0,0,'skybreaker squad leader SAY_ALLIANCE_BREAK_DONE'),
(-1001034,'Thanks for keeping us covered back there! We\'ll hold the gate while we wait for reinforcements.',0,0,0,1,'squad leader SAY_EVENT_COMPLETE'),
(-1001035,'Die, maggot!',0,0,0,0,'ymirheim defender SAY_DEFENDER_AGGRO_1'),
(-1001036,'Haraak foln!',0,0,0,0,'ymirheim defender SAY_DEFENDER_AGGRO_2'),
(-1001037,'I spit on you!',0,0,0,0,'ymirheim defender SAY_DEFENDER_AGGRO_3'),
(-1001038,'I will feed you to the dogs!',0,0,0,0,'ymirheim defender SAY_DEFENDER_AGGRO_4'),
(-1001039,'I will take pleasure in gutting you!',0,0,0,0,'ymirheim defender SAY_DEFENDER_AGGRO_5'),
(-1001040,'I\'ll eat your heart!',0,0,0,0,'ymirheim defender SAY_DEFENDER_AGGRO_6'),
(-1001041,'Sniveling pig!',0,0,0,0,'ymirheim defender SAY_DEFENDER_AGGRO_7'),
(-1001042,'Ugglin oo bjorr!',0,0,0,0,'ymirheim defender SAY_DEFENDER_AGGRO_8'),
(-1001043,'You come to die!',0,0,0,0,'ymirheim defender SAY_DEFENDER_AGGRO_9'),

(-1001044,'The Light\'s blessing be upon you for aiding me in my time of need, $N.',0,0,0,0,'father kamaros SAY_ESCORT_START_1'),
(-1001045,'I\'ve had my fill of this place. Let us depart.',0,0,0,1,'father kamaros SAY_ESCORT_START_2'),
(-1001046,'Face your judgment by the Light!',0,0,0,0,'father kamaros SAY_AGGRO_1'),
(-1001047,'The Argent Crusade never surrenders!',0,0,0,0,'father kamaros SAY_AGGRO_2'),
(-1001048,'You will never take me alive!',0,0,0,0,'father kamaros SAY_AGGRO_3'),
(-1001049,'I have you to thank for my life. I will return to my comrades and spread word of your bravery. Fight the Scourge with all the strength you can muster, and we will be by your side.',0,0,0,1,'father kamaros SAY_ESCORT_COMPLETE_2'),
(-1001050,'You must tell my brothers that I live.',0,0,0,1,'father kamaros SAY_ESCORT_COMPLETE_1'),

(-1001051,'Let me know when you\'re ready. I\'d prefer sooner than later... what with the slowly dying from poison and all.',0,0,0,1,'injured goblin miner SAY_ESCORT_READY'),
(-1001052,'I\'m going to bring the venom sac to Ricket... and then... you know... collapse. Thank you for helping me!',0,0,0,1,'injured goblin miner SAY_ESCORT_COMPLETE'),

(-1001053,'Alright, kid. Stay behind me and you\'ll be fine.',0,0,0,36,'harrison jones SAY_ESCORT_START'),
(-1001054,'Their ceremonial chamber, where I was to be sacrificed...',0,0,0,1,'harrison jones SAY_CHAMBER_1'),
(-1001055,'Time to put an end to all this!',0,0,0,1,'harrison jones SAY_CHAMBER_2'),
(-1001056,'You\'re free to go, miss.',0,0,0,1,'harrison jones SAY_CHAMBER_RELEASE'),
(-1001057,'Thank you!',0,0,0,71,'Adarrah SAY_THANK_YOU'),
(-1001058,'Odd. That usually does it.',0,0,0,1,'harrison jones SAY_CHAMBER_3'),
(-1001059,'Just as well, I\'ve had enough of this place.',0,0,0,1,'harrison jones SAY_CHAMBER_4'),
(-1001060,'What\'s this?',0,0,0,0,'harrison jones SAY_CHAMBER_5'),
(-1001061,'Aww, not a snake!',0,0,0,1,'harrison jones SAY_CHAMBER_6'),
(-1001062,'Listen, kid. I can handle this thing. You just watch my back!',0,0,0,1,'harrison jones SAY_CHAMBER_7'),
(-1001063,'See ya \'round, kid!',0,0,0,1,'harrison jones SAY_ESCORT_COMPLETE'),

(-1001064,'You couldn\'t have come at a better time! Let\'s get out of here.',0,0,0,0,'apothecary hanes SAY_ESCORT_START'),
(-1001065,'Yes, let us leave... but not before we leave our Alliance hosts something to remember us by!',0,0,0,0,'apothecary hanes SAY_FIRE_1'),
(-1001066,'They have limited supplies in this camp. It would be a real shame if something were to happen to them.',0,0,0,16,'apothecary hanes SAY_FIRE_2'),
(-1001067,'Ah, yes... watch it burn!',0,0,0,0,'apothecary hanes SAY_SUPPLIES_1'),
(-1001068,'We\'re almost done!',0,0,0,0,'apothecary hanes SAY_SUPPLIES_2'),
(-1001069,'Let\'s high-tail it out of here.',0,0,0,0,'apothecary hanes SAY_SUPPLIES_ESCAPE'),
(-1001070,'That\'ll teach you to mess with an apothecary, you motherless Alliance dogs!',0,1,0,22,'apothecary hanes SAY_SUPPLIES_COMPLETE'),
(-1001071,'Don\'t shoot! Apothecary coming through!',0,1,0,0,'apothecary hanes SAY_ARRIVE_BASE'),

(-1001072,'Something is wrong with the Highlord. Do something!',0,0,0,1,'scarlet cavalier SAY_CAVALIER_WORRY_1'),
(-1001073,'Hey, what is going on over there? Sir, are you alright?',0,0,0,1,'scarlet cavalier SAY_CAVALIER_WORRY_2'),
(-1001074,'What the....',0,0,0,1,'scarlet cavalier SAY_CAVALIER_WORRY_3'),
(-1001075,'Sir?',0,0,0,1,'scarlet cavalier SAY_CAVALIER_WORRY_4'),
(-1001076,'NOOOOOOOOOOOOO!',0,1,0,15,'taelan fordring SAY_SCARLET_COMPLETE_1'),
(-1001077,'I will lead us through Hearthglen to the forest\'s edge. From there, you will take me to my father.',0,0,0,1,'taelan fordring SAY_SCARLET_COMPLETE_2'),
(-1001078,'Remove your disguise, lest you feel the bite of my blade when the fury has taken control.',0,0,0,1,'taelan fordring SAY_ESCORT_START'),
(-1001079,'Halt.',0,0,0,0,'taelan fordring SAY_TAELAN_MOUNT'),
(-1001080,'%s calls for his mount.',0,2,0,22,'taelan fordring EMOTE_TAELAN_MOUNT'),
(-1001081,'It\'s not much further. The main road is just up ahead.',0,0,0,1,'taelan fordring SAY_REACH_TOWER'),
(-1001082,'You will not make it to the forest\'s edge, Fordring.',0,0,0,1,'isillien SAY_ISILLIEN_1'),
(-1001083,'Isillien!',0,1,0,25,'taelan fordring SAY_ISILLIEN_2'),
(-1001084,'This is not your fight, stranger. Protect yourself from the attacks of the Crimson Elite. I shall battle the Grand Inquisitor.',0,0,0,1,'taelan fordring SAY_ISILLIEN_3'),
(-1001085,'You disappoint me, Taelan. I had plans for you... grand plans. Alas, it was only a matter of time before your filthy bloodline would catch up with you.',0,0,0,1,'isillien SAY_ISILLIEN_4'),
(-1001086,'It is as they say: Like father, like son. You are as weak of will as Tirion... perhaps more so. I can only hope my assassins finally succeeded in ending his pitiful life.',0,0,0,1,'isillien SAY_ISILLIEN_5'),
(-1001087,'The Grand Crusader has charged me with destroying you and your newfound friends, Taelan, but know this: I do this for pleasure, not of obligation or duty.',0,0,0,1,'isillien SAY_ISILLIEN_6'),
(-1001088,'%s calls for his guardsman.',0,2,0,22,'isillien EMOTE_ISILLIEN_ATTACK'),
(-1001089,'The end is now, Fordring.',0,0,0,25,'isillien SAY_ISILLIEN_ATTACK'),
(-1001090,'Enough!',0,0,0,0,'isillien SAY_KILL_TAELAN_1'),
(-1001091,'%s laughs.',0,2,0,11,'isillien EMOTE_ISILLIEN_LAUGH'),
(-1001092,'Did you really believe that you could defeat me? Your friends are soon to join you, Taelan.',0,0,0,0,'isillien SAY_KILL_TAELAN_2'),
(-1001093,'%s turns his attention towards you.',0,2,0,0,'isillien EMOTE_ATTACK_PLAYER'),
(-1001094,'What have you done, Isillien? You once fought with honor, for the good of our people... and now... you have murdered my boy...',0,0,0,0,'tirion fordring SAY_TIRION_1'),
(-1001095,'Tragic. The elder Fordring lives on... You are too late, old man. Retreat back to your cave, hermit, unless you wish to join your son in the Twisting Nether.',0,0,0,0,'isillien SAY_TIRION_2'),
(-1001096,'May your soul burn in anguish, Isillien! Light give me strength to battle this fiend.',0,0,0,15,'tirion fordring SAY_TIRION_3'),
(-1001097,'Face me, coward. Face the faith and strength that you once embodied.',0,0,0,25,'tirion fordring SAY_TIRION_4'),
(-1001098,'Then come, hermit!',0,0,0,0,'isillien SAY_TIRION_5'),
(-1001099,'A thousand more like him exist. Ten thousand. Should one fall, another will rise to take the seat of power.',0,0,0,0,'tirion fordring SAY_EPILOG_1'),
(-1001100,'%s falls to one knee.',0,2,0,16,'tirion fordring EMOTE_FALL_KNEE'),
(-1001101,'Look what they did to my boy.',0,0,0,5,'tirion fordring SAY_EPILOG_2'),
(-1001102,'%s holds the limp body of Taelan Fordring and softly sobs.',0,2,0,0,'tirion fordring EMOTE_HOLD_TAELAN'),
(-1001103,'Too long have I sat idle, gripped in this haze... this malaise, lamenting what could have been... what should have been.',0,0,0,0,'tirion fordring SAY_EPILOG_3'),
(-1001104,'Your death will not have been in vain, Taelan. A new Order is born on this day... an Order which will dedicate itself to extinguising the evil that plagues this world. An evil that cannot hide behind politics and pleasantries.',0,0,0,0,'tirion fordring SAY_EPILOG_4'),
(-1001105,'This I promise... This I vow...',0,0,0,15,'tirion fordring SAY_EPILOG_5'),

(-1001106,'Don\'t forget to get my bell out of the chest here. And remember, if do happen to wander off, just ring it and I\'ll find you again.',0,0,0,1,'shay leafrunner SAY_ESCORT_START'),
(-1001107,'Are we taking the scenic route?',0,0,0,0,'shay leafrunner SAY_WANDER_1'),
(-1001108,'Oh, what a beautiful flower over there...',0,0,0,0,'shay leafrunner SAY_WANDER_2'),
(-1001109,'Are you sure this is the right way? Maybe we should go this way instead...',0,0,0,0,'shay leafrunner SAY_WANDER_3'),
(-1001110,'Hmmm, I wonder what\'s over this way?',0,0,0,0,'shay leafrunner SAY_WANDER_4'),
(-1001111,'This is quite an adventure!',0,0,0,0,'shay leafrunner SAY_WANDER_DONE_1'),
(-1001112,'Oh, I wandered off again. I\'m sorry.',0,0,0,0,'shay leafrunner SAY_WANDER_DONE_2'),
(-1001113,'The bell again, such a sweet sound.',0,0,0,0,'shay leafrunner SAY_WANDER_DONE_3'),
(-1001114,'%s begins to wander off.',0,2,0,0,'shay leafrunner EMOTE_WANDER'),
(-1001115,'Oh, here you are, Rockbiter! I\'m sorry, I know I\'m not supposed to wander off.',0,0,0,1,'shay leafrunner SAY_EVENT_COMPLETE_1'),
(-1001116,'I\'m so glad yer back Shay. Please, don\'t ever run off like that again! What would I tell yer parents if I lost ya?',0,0,0,1,'rockbiter SAY_EVENT_COMPLETE_2'),

(-1001117,'AHAHAHAHA... you\'ll join us soon enough!',0,1,0,0,'saronite mine slave SAY_MINER_SUICIDE_1'),
(-1001118,'I don\'t want to leave! I want to stay here!',0,1,0,0,'saronite mine slave SAY_MINER_SUICIDE_2'),
(-1001119,'I must get further underground to where he is. I must jump!',0,1,0,0,'saronite mine slave SAY_MINER_SUICIDE_3'),
(-1001120,'I won\'t leave!',0,1,0,0,'saronite mine slave SAY_MINER_SUICIDE_4'),
(-1001121,'I\'ll never return. The whole reason for my existence awaits below!',0,1,0,0,'saronite mine slave SAY_MINER_SUICIDE_5'),
(-1001122,'I\'m coming, master!',0,1,0,0,'saronite mine slave SAY_MINER_SUICIDE_6'),
(-1001123,'My life for you!',0,1,0,0,'saronite mine slave SAY_MINER_SUICIDE_7'),
(-1001124,'NO! You\'re wrong! The voices in my head are beautiful!',0,1,0,0,'saronite mine slave SAY_MINER_SUICIDE_8'),

(-1001125,'Beginning the distillation in 5 seconds.',0,0,0,0,'tipsy mcmanus SAY_DISTILLATION_START'),
(-1001126,'Add another orange! Quickly!',0,0,0,25,'tipsy mcmanus SAY_ADD_ORANGE'),
(-1001127,'Add bananas!',0,0,0,25,'tipsy mcmanus SAY_ADD_BANANAS'),
(-1001128,'Put a papaya in the still!',0,0,0,25,'tipsy mcmanus SAY_ADD_PAPAYA'),
(-1001129,'The still needs heat! Light the brazier!',0,0,0,5,'tipsy mcmanus SAY_LIGHT_BRAZIER'),
(-1001130,'Pressure\'s too high! Open the pressure valve!',0,0,0,5,'tipsy mcmanus SAY_OPEN_VALVE'),
(-1001131,'Good job! Keep your eyes open, now.',0,0,0,4,'tipsy mcmanus SAY_ACTION_COMPLETE_1'),
(-1001132,'Nicely handled! Stay on your toes!',0,0,0,4,'tipsy mcmanus SAY_ACTION_COMPLETE_2'),
(-1001133,'Well done! Be ready for anything!',0,0,0,4,'tipsy mcmanus SAY_ACTION_COMPLETE_3'),
(-1001134,'That\'ll do. Never know what it\'ll need next...',0,0,0,4,'tipsy mcmanus SAY_ACTION_COMPLETE_4'),
(-1001135,'It\'s no good! I\'m shutting it down...',0,0,0,0,'tipsy mcmanus SAY_DISTILLATION_FAIL'),
(-1001136,'We\'ve done it! Come get the cask.',0,0,0,0,'tipsy mcmanus SAY_DISTILLATION_COMPLETE'),

(-1001137,'The duel will begin in...',0,5,0,0,'death knight initiate EMOTE_DUEL_BEGIN'),
(-1001138,'3...',0,5,0,0,'death knight initiate EMOTE_DUEL_BEGIN_3'),
(-1001139,'2...',0,5,0,0,'death knight initiate EMOTE_DUEL_BEGIN_2'),
(-1001140,'1...',0,5,0,0,'death knight initiate EMOTE_DUEL_BEGIN_1'),

(-1001141,'Nope, not here...',0,0,0,0,'stinky ignatz SAY_SECOND_STOP'),
(-1001142,'There must be one around here somewhere...',0,0,0,0,'stinky ignatz SAY_THIRD_STOP_1'),
(-1001143,'Ah, there\'s one!',0,0,0,0,'stinky ignatz SAY_THIRD_STOP_2'),
(-1001144,'Come, $N! Let\'s go over there and collect it!',0,0,0,0,'stinky ignatz SAY_THIRD_STOP_3'),
(-1001145,'Ok, let\'s get out of here!',0,0,0,0,'stinky ignatz SAY_PLANT_GATHERED'),
(-1001146,'I\'m glad you\'re here! Because I need your help!!',0,0,0,0,'stinky ignatz SAY_AGGRO_3'),
(-1001147,'Look out! The $N attacks!',0,0,0,0,'stinky ignatz SAY_AGGRO_4'),

(-1001148,'I am ready, $N. Let\'s find my equipment and get out of here. I think I know where it is.',0,0,0,1,'captured arko\'narin SAY_ESCORT_START'),
(-1001149,'Oh my! Look at this... all these candles. I\'m sure they\'re used for some terrible ritual or dark summoning. We best make haste.',0,0,0,18,'captured arko\'narin SAY_FIRST_STOP'),
(-1001150,'There! Over there!',0,0,0,25,'captured arko\'narin SAY_SECOND_STOP'),
(-1001151,'You will not stop me from escaping here, $N!',0,0,0,0,'captured arko\'narin SAY_AGGRO'),
(-1001152,'All I need now is a golden lasso.',0,0,0,0,'captured arko\'narin SAY_EQUIPMENT'),
(-1001153,'DIE DEMON DOGS!',0,0,0,0,'captured arko\'narin SAY_ESCAPE'),
(-1001154,'Ah! Fresh air at last! I never thought I\'d see the day.',0,0,0,4,'captured arko\'narin SAY_FRESH_AIR'),
(-1001155,'BETRAYER!',0,1,0,0,'spirit of trey lightforge SAY_BETRAYER'),
(-1001156,'What was that?! Trey? TREY?',0,0,0,22,'captured arko\'narin SAY_TREY'),
(-1001157,'You kept me in the cell for too long, monster!',0,0,0,0,'captured arko\'narin SAY_ATTACK_TREY'),
(-1001158,'No! My friend... what\'s happened? This is all my fault...',0,0,0,18,'captured arko\'narin SAY_ESCORT_COMPLETE'),

(-1001159,'Please, help me to get through this cursed forest, $r.',0,0,0,0,'arei SAY_ESCORT_START'),
(-1001160,'This creature suffers from the effect of the fel... We must end its misery.',0,0,0,0,'arei SAY_ATTACK_IRONTREE'),
(-1001161,'The corruption of the fel has not left any of the creatures of Felwood untouched, $N. Please, be on your guard.',0,0,0,0,'arei SAY_ATTACK_TOXIC_HORROR'),
(-1001162,'I sense the taint of corruption upon this $N. Help me detroy it.',0,0,0,0,'arei SAY_EXIT_WOODS'),
(-1001163,'That I must fight against my own kind deeply saddens me.',0,0,0,0,'arei SAY_CLEAR_PATH'),
(-1001164,'I can sens it now, $N. Ashenvale lies down this path.',0,0,0,0,'arei SAY_ASHENVALE'),
(-1001165,'I feel... something strange...',0,0,0,0,'arei SAY_TRANSFORM'),
(-1001166,'$N my form has now changed! The true strength of my spirit is returing to me now... The cursed grasp of the forest is leaving me.',0,0,0,0,'arei SAY_LIFT_CURSE'),
(-1001167,'Thank you, $N. Now my spirit will finally be at peace.',0,0,0,0,'arei SAY_ESCORT_COMPLETE'),

(-1001168,'The naga torture the spirits of water. They invoke chaos and destruction!',0,0,0,0,'wilda SAY_WIL_PROGRESS_4'),
(-1001169,'The naga do not respect nature. They twist and corrupt it to meet their needs. They live to agitate the spirits.',0,0,0,0,'wilda SAY_WIL_PROGRESS_5'),

(-1001170,'Time only has meaning to mortals, insect. Dimensius is infinite!',0,1,0,0,'dimensius SAY_AGGRO'),
(-1001171,'I hunger! Feed me the power of this forge, my children!',0,1,0,0,'dimensius SAY_SUMMON'),

(-1001172,'Spare my life! I will tell you about Arelion\'s secret.',0,0,0,0,'magister_aledis SAY_ALEDIS_DEFEAT'),

(-1001173,'Are you ready, Mr. Floppy? Stay close to me and watch out for those wolves!',0,0,0,0,'emily SAY_ESCORT_START'),
(-1001174,'Um... I think one of those wolves is back...',0,0,0,0,'emily SAY_FIRST_WOLF'),
(-1001175,'He\'s going for Mr. Floppy!',0,0,0,0,'emily SAY_WOLF_ATTACK'),
(-1001176,'There\'s a big meanie attacking Mr. Floppy! Help!',0,0,0,0,'emily SAY_HELP_FLOPPY_1'),
(-1001177,'Let\'s get out of here before more wolves find us!',0,0,0,0,'emily SAY_FIRST_WOLF_DEFEAT'),
(-1001178,'Oh, no! Look, it\'s another wolf, and it\'s a biiiiiiig one!',0,0,0,0,'emily SAY_SECOND_WOLF'),
(-1001179,'He\'s gonna eat Mr. Floppy! You gotta help Mr. Floppy! You just gotta!',0,0,0,0,'emily SAY_HELP_FLOPPY_2'),
(-1001180,'Don\'t go toward the light, Mr. Floppy!',0,0,0,0,'emily SAY_FLOPPY_ALMOST_DEAD'),
(-1001181,'Mr. Floppy, you\'re ok! Thank you so much for saving Mr. Floppy!',0,0,0,0,'emily SAY_SECOND_WOLF_DEFEAT'),
(-1001182,'I think I see the camp! We\'re almost home, Mr. Floppy! Let\'s go!',0,0,0,0,'emily SAY_RESUME_ESCORT'),
(-1001183,'Thank you for helping me to get back to the camp. Go tell Walter that I\'m safe now!',0,0,0,0,'emily SAY_ESCORT_COMPLETE'),

(-1001184,'How did you find me? Did Landgren tell?',14201,0,0,0,'admiral_westwind SAY_AGGRO'),
(-1001185,'You thought I would just let you kill me?',14205,0,0,0,'admiral_westwind SAY_SPHERE'),
(-1001186,'WHAT?! No matter. Even without my sphere, I will crush you! Behold my true identity and despair!',14207,1,0,0,'admiral_westwind SAY_NO_MATTER'),
(-1001187,'Gah! I spent too much time in that weak little shell.',14426,1,0,0,'malganis_icecrown SAY_TRANSFORM'),
(-1001188,'Kirel narak! I am Mal\'Ganis. I AM ETERNAL!',14427,1,0,0,'malganis_icecrown SAY_20_HP'),
(-1001189,'ENOUGH! I waste my time here. I must gather my strength on the homeworld.',14428,1,0,0,'malganis_icecrown SAY_DEFEATED'),
(-1001190,'You\'ll never defeat the Lich King without my forces. I\'ll have my revenge... on him AND you!',14429,1,0,0,'malganis_icecrown SAY_ESCAPE'),

(-1001191,'Hey, you call yourself a body guard? Get to work and protect us...',0,0,0,0,'SAY_CORK_AMBUSH1'),
(-1001192,'Mister body guard, are you going to earn your money or what?',0,0,0,0,'SAY_CORK_AMBUSH2'),
(-1001193,'You\'re fired! <Cough...Cork clears throat.> I mean, help!',0,0,0,0,'SAY_CORK_AMBUSH3'),
(-1001194,'Blast those stupid centaurs! Sigh - well, it seems you kept your bargain. Up the road you shall find Smeed Scrabblescrew, he has your money.',0,0,0,1,'SAY_CORK_END'),
(-1001195,'Eeck! Demons hungry for the kodos!',0,0,0,0,'SAY_RIGGER_AMBUSH1'),
(-1001196,'What am I paying you for? The kodos are nearly dead!',0,0,0,0,'SAY_RIGGER_AMBUSH2'),
(-1001197,'Wow! We did it... not sure why we thought we needed the likes of you. Nevertheless, speak with Smeed Srablescrew; he will give you your earnings!',0,0,0,0,'SAY_RIGGER_END'),

(-1001198,'Cover me!',0,0,0,5,'demolitionist_legoso SAY_ESCORT_2'),
(-1001199,'It won\'t be much longer, $n. Just keep them off me while I work.',0,0,0,133,'demolitionist_legoso SAY_ESCORT_3'),
(-1001200,'That\'ll do it! Quickly, take cover!',0,0,0,5,'demolitionist_legoso SAY_ESCORT_4'),
(-1001201,'3...',0,0,0,0,'demolitionist_legoso SAY_ESCORT_COUNT_3'),
(-1001202,'2...',0,0,0,0,'demolitionist_legoso SAY_ESCORT_COUNT_2'),
(-1001203,'1...',0,0,0,0,'demolitionist_legoso SAY_ESCORT_COUNT_1'),
(-1001204,'Don\'t get too excited, hero, that was the easy part. The challenge lies ahead! Let\'s go.',0,0,0,1,'demolitionist_legoso SAY_ESCORT_CONTINUE'),
(-1001205,'What in the Nether is that?!?!',0,0,0,5,'demolitionist_legoso SAY_ESCORT_5'),
(-1001206,'Be ready for anything, $n.',0,0,0,1,'demolitionist_legoso SAY_ESCORT_6'),
(-1001207,'Blessed Light! She\'s siphoning energy right out of the Vector Coil!',0,0,0,34,'demolitionist_legoso SAY_ESCORT_7'),
(-1001208,'Cover me, we have to do this quickly. Once I blow the support on this side, it will disrupt the energy beams and she\'ll break out! I doubt very much that she\'ll be happy to see us.',0,0,0,1,'demolitionist_legoso SAY_ESCORT_8'),
(-1001209,'I\'ve almost got it! Just a little more time...',0,0,0,0,'demolitionist_legoso SAY_ESCORT_9'),
(-1001210,'Take cover and be ready for the fight of your life, $n!',0,0,0,5,'demolitionist_legoso SAY_ESCORT_10'),
(-1001211,'Holy mother of O\'ros!',0,0,0,5,'demolitionist_legoso SAY_ESCORT_AGGRO'),
(-1001212,'I... I can\'t believe it\'s over. You did it! You\'ve destroyed the blood elves and their leader!',0,0,0,1,'demolitionist_legoso SAY_ESCORT_COMPLETE_1'),
(-1001213,'Get back to Blood Watch. I\'ll see you there...',0,0,0,1,'demolitionist_legoso SAY_ESCORT_COMPLETE_2'),

(-1001214,'%s becomes unstable with brimming energy.',0,2,0,0,'living_flare EMOTE_UNSTABLE'),
(-1001215,'%s releases its energy, engulfing its surroundings in flames!',0,2,0,0,'living_flare EMOTE_BURST'),

(-1001216,'The last thing I remember is the ship falling and us getting into the pods. I\'ll go see how I can help. Thank you!',0,0,7,0,'draenei_survivor SAY_HEAL1'),
(-1001217,'Where am I? Who are you? Oh no! What happened to the ship?',0,0,7,0,'draenei_survivor SAY_HEAL2'),
(-1001218,'$C You saved me! I owe you a debt that I can never repay. I\'ll go see if I can help the others.',0,0,7,0,'draenei_survivor SAY_HEAL3'),
(-1001219,'Ugh... what is this place? Is that all that\'s left of the ship over there?',0,0,7,0,'draenei_survivor SAY_HEAL4'),
(-1001220,'Many thanks to you, $c. I''d best get to the crash site and see how I can help out. Until we meet again.',0,0,7,0,'draenei_survivor SAY_HEAL5'),
(-1001221,'Huh? What happened? Oh... my head feels like it''s going to explode! I''d best get back to the crash site.',0,0,7,0,'draenei_survivor SAY_HEAL6'),
(-1001222,'Oh, the pain...',0,0,7,0,'draenei_survivor SAY_HELP1'),
(-1001223,'Everything hurts. Please, make it stop...',0,0,7,0,'draenei_survivor SAY_HELP2'),
(-1001224,'Ughhh... I hurt. Can you help me?',0,0,7,0,'draenei_survivor SAY_HELP3'),
(-1001225,'I don''t know if I can make it. Please help me...',0,0,7,0,'draenei_survivor SAY_HELP4'),

(-1001226,'Thank you for doing this, $r. I... I think I know how to get back to my father. Please don\'t let the bad things hurt me.',0,0,0,1,'SAY_MOOTOO_Y_START'),
(-1001227,'I thought for sure that I would die like all the others!',0,0,0,1,'SAY_1_MOOTOO_Y'),
(-1001228,'I can feel my father\'s aura. He\'s definitely this way... come on, $n.',0,0,0,1,'SAY_2_MOOTOO_Y'),
(-1001229,'The mist is death... senseless, without compassion.',0,0,0,1,'SAY_3_MOOTOO_Y'),
(-1001230,'So cold...',0,0,0,1,'SAY_4_MOOTOO_Y'),
(-1001231,'What was that! Did you see that?',0,0,0,0,'SAY_5_MOOTOO_Y'),
(-1001232,'Everyone is dead... Everyone and everything...',0,0,0,1,'SAY_6_MOOTOO_Y'),
(-1001233,'I see a break in the mist up ahead. We\'re getting closer!',0,0,0,5,'SAY_7_MOOTOO_Y'),
(-1001234,'How will we overcome the mist?',0,0,0,1,'SAY_8_MOOTOO_Y'),
(-1001235,'FATHER! Father, I\'m saved!',0,0,0,1,'SAY_CREDIT_MOOTOO_Y'),
(-1001236,'Daughter! You have returned!',0,0,0,5,'SAY_1_ELDER_MOOTOO'),
(-1001237,'%s hugs Elder Mootoo.',0,2,0,5,'SAY_9_MOOTOO_Y'),
(-1001238,'Mootoo thanks you, $n.',0,0,0,1,'SAY_2_ELDER_MOOTOO'),

(-1001239,'%s is drawn to the ghost magnet...',0,2,0,0,'Magrami Spectre on spawn 1'),
(-1001240,'%s is angered!',0,2,0,0,'Magrami Spectre on spawn 2'),
(-1001241,'The Avatar of Terokk has been defeated! Death to Terokk! Death to Skettis!',0,0,7,0,'Defender Grashna on Avatar of Terokk death');

-- -1 033 000 SHADOWFANG KEEP
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1033000,'Follow me and I\'ll open the courtyard door for you.',0,0,7,1,'prisoner ashcrombe SAY_FREE_AS'),
(-1033001,'I have just the spell to get this door open. Too bad the cell doors weren\'t locked so haphazardly.',0,0,7,1,'prisoner ashcrombe SAY_OPEN_DOOR_AS'),
(-1033002,'There it is! Wide open. Good luck to you conquering what lies beyond. I must report back to the Kirin Tor at once!',0,0,7,1,'prisoner ashcrombe SAY_POST_DOOR_AS'),

(-1033003,'Free from this wretched cell at last! Let me show you to the courtyard....',0,0,1,1,'prisoner adamant SAY_FREE_AD'),
(-1033004,'You are indeed courageous for wanting to brave the horrors that lie beyond this door.',0,0,1,1,'prisoner adamant SAY_OPEN_DOOR_AD'),
(-1033005,'There we go!',0,0,1,1,'prisoner adamant SAY_POST1_DOOR_AD'),
(-1033006,'Good luck with Arugal. I must hurry back to Hadrec now.',0,0,1,1,'prisoner adamant SAY_POST2_DOOR_AD'),

(-1033007,'About time someone killed the wretch.',0,0,1,1,'prisoner adamant SAY_BOSS_DIE_AD'),
(-1033008,'For once I agree with you... scum.',0,0,7,1,'prisoner ashcrombe SAY_BOSS_DIE_AS'),

(-1033009,'I have changed my mind loyal servants, you do not need to bring the prisoner all the way to my study, I will deal with him here and now.',0,0,0,1,'arugal SAY_INTRO_1'),
(-1033010,'Vincent!  You and your pathetic ilk will find no more success in routing my sons and I than those beggardly remnants of the Kirin Tor.',0,0,0,0,'arugal SAY_INTRO_2'),
(-1033011,'If you will not serve my Master with your sword and knowledge of his enemies...',0,0,0,1,'arugal SAY_INTRO_3'),
(-1033012,'Your moldering remains will serve ME as a testament to what happens when one is foolish enough to trespass in my domain!\n',0,0,0,0,'arugal SAY_INTRO_4'),

(-1033013,'Who dares interfere with the Sons of Arugal?',0,1,0,0,'boss_arugal YELL_FENRUS'),
(-1033014,'%s vanishes.',0,2,0,0,'prisoner ashcrombe EMOTE_VANISH_AS'),
(-1033015,'%s fumbles with the rusty lock on the courtyard door.',0,2,0,432,'prisoner adamant EMOTE_UNLOCK_DOOR_AD'),
(-1033016,'Arrrgh!',0,0,0,0,'deathstalker vincent SAY_VINCENT_DIE'),
(-1033017,'You, too, shall serve!',5793,1,0,0,'boss_arugal YELL_AGGRO'),
(-1033018,'Another Falls!',5795,1,0,0,'boss_arugal YELL_KILLED_PLAYER'),
(-1033019,'Release your rage!',5797,1,0,0,'boss_arugal YELL_COMBAT'),

(-1033020,'Did they bother to tell you who I am and why I am doing this?',0,0,0,0,'hummel SAY_INTRO_1'),
(-1033021,'...or are they just using you like they do everybody else?',0,0,0,0,'hummel SAY_INTRO_2'),
(-1033022,'But what does it matter. It is time for this to end.',0,0,0,0,'hummel SAY_INTRO_3'),
(-1033023,'Baxter! Get in there and help! NOW!',0,0,0,0,'hummel SAY_CALL_BAXTER'),
(-1033024,'It is time, Frye! Attack!',0,0,0,0,'hummel SAY_CALL_FRYE'),
(-1033025,'...please don\'t think less of me.',0,0,0,0,'hummel SAY_DEATH');

-- -1 034 000 STOCKADES

-- -1 036 000 DEADMINES
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1036000,'You there! Check out that noise.',5775,6,7,0,'smite INST_SAY_ALARM1'),
(-1036001,'We\'re under attack!  A vast, ye swabs! Repel the invaders!',5777,6,7,0,'smite INST_SAY_ALARM2'),
(-1036002,'You land lubbers are tougher than I thought! I\'ll have to improvise!',5778,0,0,21,'smite SAY_PHASE_2'),
(-1036003,'D\'ah! Now you\'re making me angry!',5779,0,0,15,'smite SAY_PHASE_3');

-- -1 043 000 WAILING CAVERNS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1043000,'At last! Naralex can be awakened! Come aid me, brave adventurers!',0,6,0,0,'Disciple of Naralex - SAY_INTRO'),
(-1043001,'I must make the necessary preparations before the awakening ritual can begin. You must protect me!',0,0,0,0,'SAY_PREPARE'),
(-1043002,'These caverns were once a temple of promise for regrowth in the Barrens. Now, they are the halls of nightmares.',0,0,0,0,'Disciple of Naralex - SAY_FIRST_CORNER'),
(-1043003,'Come. We must continue. There is much to be done before we can pull Naralex from his nightmare.',0,0,0,0,'Disciple of Naralex - SAY_CONTINUE'),
(-1043004,'Within this circle of fire I must cast the spell to banish the spirits of the slain Fanglords.',0,0,0,0,'Disciple of Naralex - SAY_CIRCLE_BANISH'),
(-1043005,'The caverns have been purified. To Naralex\'s chamber we go!',0,0,0,0,'Disciple of Naralex - SAY_PURIFIED'),
(-1043006,'Beyond this corridor, Naralex lies in fitful sleep. Let us go awaken him before it is too late.',0,0,0,0,'Disciple of Naralex - SAY_NARALEX_CHAMBER'),
(-1043007,'Protect me brave souls as I delve into the Emerald Dream to rescue Naralex and put an end to this corruption!',0,1,0,0,'Disciple of Naralex - SAY_BEGIN_RITUAL'),
(-1043008,'%s begins to perform the awakening ritual on Naralex.',0,2,0,0,'Disciple of Naralex - EMOTE_RITUAL_BEGIN'),
(-1043009,'%s tosses fitfully in troubled sleep.',0,2,0,0,'Naralex - EMOTE_NARALEX_AWAKE'),
(-1043010,'%s writhes in agony. The Disciple seems to be breaking through.',0,2,0,0,'Naralex - EMOTE_BREAK_THROUGH'),
(-1043011,'%s dreams up a horrendous vision. Something stirs beneath the murky waters.',0,2,0,0,'Naralex - EMOTE_VISION'),
(-1043012,'This $N is a minion from Naralex\'s nightmare no doubt!.',0,0,0,0,'Disciple of Naralex - SAY_MUTANUS'),
(-1043013,'I AM AWAKE, AT LAST!',5789,1,0,0,'Naralex - SAY_NARALEX_AWAKE'),
(-1043014,'At last! Naralex awakes from the nightmare.',0,0,0,0,'Disciple of Naralex - SAY_AWAKE'),
(-1043015,'Ah, to be pulled from the dreaded nightmare! I thank you, my loyal Disciple, along with your brave companions.',0,0,0,0,'Naralex - SAY_NARALEX_THANKYOU'),
(-1043016,'We must go and gather with the other Disciplies. There is much work to be done before I can make another attempt to restore the Barrens. Farewell, brave souls!',0,0,0,0,'Naralex - SAY_FAREWELL'),
(-1043017,'Attacked! Help get this $N off of me!',0,0,0,0,'Disciple of Naralex - SAY_AGGRO_1'),
(-1043018,'Help!',0,0,0,0,'Disciple of Naralex - SAY_AGGRO_2'),
(-1043019,'Deal with this $N! I need to prepare to awake Naralex!',0,0,0,0,'Disciple of Naralex - SAY_AGGRO_3');

-- -1 047 000 RAZORFEN KRAUL
INSERT INTO script_texts (entry,content_default,sound,type,LANGUAGE,emote,comment) VALUES
(-1047000,'Woo hoo! Finally getting out of here. It\'s going to be rough though. Keep your eyes peeled for trouble.',0,0,0,0,'willix SAY_READY'),
(-1047001,'Up there is where Charlga Razorflank resides. Blasted old crone.',0,0,0,25,'willix SAY_1'),
(-1047002,'There\'s blueleaf tuber in this trench! It\'s like gold waiting to be mined I tell you!',0,0,0,0,'willix SAY_2'),
(-1047003,'There could be danger around every corner here.',0,0,0,0,'willix SAY_3'),
(-1047004,'I don\'t see how these foul animals live in this place... sheesh it smells!',0,0,0,0,'willix SAY_4'),
(-1047005,'I think I see a way for us to get out of this big twisted mess of a bramble.',0,0,0,0,'willix SAY_5'),
(-1047006,'Glad to be out of that wretched trench. Not much nicer up here though!',0,0,0,0,'willix SAY_6'),
(-1047007,'Finally! I\'ll be glad to get out of this place.',0,0,0,0,'willix SAY_7'),
(-1047008,'I think I\'ll rest a moment and catch my breath before heading back to Ratchet. Thanks for all the help!',0,0,0,0,'willix SAY_END'),
(-1047009,'$N heading this way fast! To arms!',0,0,0,0,'willix SAY_AGGRO_1'),
(-1047010,'Eek! $N coming right at us!',0,0,0,0,'willix SAY_AGGRO_2'),
(-1047011,'Egads! $N on me!',0,0,0,0,'willix SAY_AGGRO_3'),
(-1047012,'Help! Get this $N off of me!',0,0,0,0,'willix SAY_AGGRO_4');

-- -1 048 000 BLACKFATHOM DEEPS

-- -1 070 000 ULDAMAN
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1070000,'REUSE ME',0,0,0,0,'REUSE ME'),
(-1070001,'Who dares awaken Archaedas? Who dares the wrath of the makers!',5855,1,0,0,'archaedas SAY_AGGRO'),
(-1070002,'Awake ye servants, defend the discs!',5856,1,0,0,'archaedas SAY_AWAKE_GUARDIANS'),
(-1070003,'To my side, brothers. For the makers!',5857,1,0,0,'archaedas SAY_AWAKE_WARDERS'),
(-1070004,'Reckless mortal.',5858,1,0,0,'archaedas SAY_UNIT_SLAIN'),
(-1070005,'%s breaks free from his stone slumber!',0,2,0,0,'archaedas EMOTE_BREAK_FREE');

-- -1 090 000 GNOMEREGAN
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1090000,'With your help, I can evaluate these tunnels.',0,0,0,1,'emi shortfuse SAY_START'),
(-1090001,'Let\'s see if we can find out where these Troggs are coming from.... and put a stop to the invasion!',0,0,0,1,'emi shortfuse SAY_INTRO_1'),
(-1090002,'Such devastation... what a horrible mess...',0,0,0,5,'emi shortfuse SAY_INTRO_2'),
(-1090003,'It\'s quiet here....',0,0,0,1,'emi shortfuse SAY_INTRO_3'),
(-1090004,'...too quiet.',0,0,0,1,'emi shortfuse SAY_INTRO_4'),
(-1090005,'Look! Over there at the tunnel wall!',0,0,0,25,'emi shortfuse SAY_LOOK_1'),
(-1090006,'Trogg incursion! Defend me while I blast the hole closed!',0,0,0,5,'emi shortfuse SAY_HEAR_1'),
(-1090007,'Get this, $n off of me!',0,0,0,0,'emi shortfuse SAY_AGGRO_1'),
(-1090008,'I don\'t think one charge is going to cut it. Keep fending them off!',0,0,0,0,'emi shortfuse SAY_CHARGE_1'),
(-1090009,'The charges are set. Get back before they blow!',0,0,0,5,'emi shortfuse SAY_CHARGE_2'),
(-1090010,'Incoming blast in 10 seconds!',0,1,0,5,'emi shortfuse SAY_BLOW_1_10'),
(-1090011,'Incoming blast in 5 seconds. Clear the tunnel! Stay back!',0,1,0,5,'emi shortfuse SAY_BLOW_1_5'),
(-1090012,'FIRE IN THE HOLE!',0,1,0,25,'emi shortfuse SAY_BLOW_1'),
(-1090013,'Well done! Without your help I would have never been able to thwart that wave of troggs.',0,0,0,4,'emi shortfuse SAY_FINISH_1'),
(-1090014,'Did you hear something?',0,0,0,6,'emi shortfuse SAY_LOOK_2'),
(-1090015,'I heard something over there.',0,0,0,25,'emi shortfuse SAY_HEAR_2'),
(-1090016,'More troggs! Ward them off as I prepare the explosives!',0,0,0,0,'emi shortfuse SAY_CHARGE_3'),
(-1090017,'The final charge is set. Stand back!',0,0,0,1,'emi shortfuse SAY_CHARGE_4'),
(-1090018,'10 seconds to blast! Stand back!!!',0,1,0,5,'emi shortfuse SAY_BLOW_2_10'),
(-1090019,'5 seconds until detonation!!!!!',0,1,0,5,'emi shortfuse SAY_BLOW_2_5'),
(-1090020,'Nice work! I\'ll set off the charges to prevent any more troggs from making it to the surface.',0,0,0,1,'emi shortfuse SAY_BLOW_SOON'),
(-1090021,'FIRE IN THE HOLE!',0,1,0,0,'emi shortfuse SAY_BLOW_2'),
(-1090022,'Superb! Because of your help, my people stand a chance of re-taking our beloved city. Three cheers to you!',0,0,0,0,'emi shortfuse SAY_FINISH_2'),

(-1090023,'We come from below! You can never stop us!',0,1,0,1,'grubbis SAY_GRUBBIS_SPAWN'),

(-1090024,'Usurpers! Gnomeregan is mine!',5807,1,0,0,'thermaplugg SAY_AGGRO'),
(-1090025,'My machines are the future! They\'ll destroy you all!',5808,1,0,0,'thermaplugg  SAY_PHASE'),
(-1090026,'Explosions! MORE explosions! I\'ve got to have more explosions!',5809,1,0,0,'thermaplugg  SAY_BOMB'),
(-1090027,'...and stay dead! He got served',5810,1,0,0,'thermaplugg  SAY_SLAY'),

(-1090028,'$n attacking! Help!',0,0,0,0,'emi shortfuse SAY_AGGRO_2');

-- -1 109 000 SUNKEN TEMPLE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1109000,'The walls of the chamber tremble. Something is happening...',0,2,0,0,'malfurion stormrage EMOTE_MALFURION'),
(-1109001,'Be steadfast, champion. I know why it is that you are here and I know what it is that you seek. Eranikus will not give up the shard freely. He has been twisted... twisted by the same force that you seek to destroy.',0,0,0,0,'malfurion stormrge SAY_MALFURION1'),
(-1109002,'Are you really surprised? Is it hard to believe that the power of an Old God could reach even inside the Dream? It is true - Eranikus, Tyrant of the Dream, wages a battle against us all. The Nightmare follows in his wake of destruction.',0,0,0,0,'malfurion stormrge SAY_MALFURION2'),
(-1109003,'Understand this, Eranikus wants nothing more than to be brought to Azeroth from the Dream. Once he is out, he will stop at nothing to destroy my physical manifestation. This, however, is the only way in which you could recover the scepter shard.',0,0,0,0,'malfurion stormrge SAY_MAFLURION3'),
(-1109004,'You will bring him back into this world, champion.',0,0,0,0,'malfurion Stormrge SAY_MALFURION4'),

(-1109005,'The shield be down! Rise up Atal\'ai! Rise up!',5861,6,0,0,'jammalan SAY_JAMMALAN_INTRO'),

(-1109006,'HAKKAR LIVES!',5870,1,0,0,'avatar SAY_AVATAR_BRAZIER_1'),
(-1109007,'I TASTE THE BLOOD OF LIFE!',5868,1,0,0,'avatar SAY_AVATAR_BRAZIER_2'),
(-1109008,'I DRAW CLOSER TO YOUR WORLD!',5867,1,0,0,'avatar SAY_AVATAR_BRAZIER_3'),
(-1109009,'I AM NEAR!',5869,1,0,0,'avatar SAY_AVATAR_BRAZIER_4'),
(-1109010,'I AM HERE!',0,1,0,0,'avatar SAY_AVATAR_SPAWN');

-- -1 129 000 RAZORFEN DOWNS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1129000,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1129001,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1129002,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1129003,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1129004,'REUSE_ME',0,0,0,0,'REUSE_ME'),

(-1129005,'All right, stay close. These fiends will jump right out of the shadows at you if you let your guard down.',0,0,0,0,'belnistrasz SAY_READY'),
(-1129006,'Okay, here we go. It\'s going to take about five minutes to shut this thing down through the ritual. Once I start, keep the vermin off of me or it will be the end of us all!',0,0,0,0,'belnistrasz SAY_START_RIT'),
(-1129007,'You\'ll rue the day you crossed me, $N',0,0,0,0,'belnistrasz SAY_AGGRO_1'),
(-1129008,'Incoming $N - look sharp, friends!',0,0,0,0,'belnistrasz SAY_AGGRO_2'),
(-1129009,'Three minutes left -- I can feel the energy starting to build! Keep up the solid defense!',0,1,0,0,'belnistrasz SAY_3_MIN'),
(-1129010,'Just two minutes to go! We\'re half way there, but don\'t let your guard down!',0,1,0,0,'belnistrasz SAY_2_MIN'),
(-1129011,'One more minute! Hold on now, the ritual is about to take hold!',0,1,0,0,'belnistrasz SAY_1_MIN'),
(-1129012,'That\'s it -- we made it! The ritual is set in motion, and idol fires are about to go out for good! You truly are the heroes I thought you would be!',0,1,0,4,'belnistrasz SAY_FINISH');

-- -1 189 000 SCARLET MONASTERY
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1189000,'Ah, I have been waiting for a real challenge!',5830,1,0,0,'herod SAY_AGGRO'),
(-1189001,'Blades of Light!',5832,1,0,0,'herod SAY_WHIRLWIND'),
(-1189002,'Light, give me strength!',5833,1,0,0,'herod SAY_ENRAGE'),
(-1189003,'Hah, is that all?',5831,1,0,0,'herod SAY_KILL'),
(-1189004,'REUSE_ME',0,0,0,0,'REUSE_ME'),

(-1189005,'Infidels! They must be purified!',5835,1,0,0,'mograine SAY_MO_AGGRO'),
(-1189006,'Unworthy!',5836,1,0,0,'mograine SAY_MO_KILL'),
(-1189007,'At your side, milady!',5837,1,0,0,'mograine SAY_MO_RESSURECTED'),

(-1189008,'What, Mograine has fallen? You shall pay for this treachery!',5838,1,0,0,'whitemane SAY_WH_INTRO'),
(-1189009,'The Light has spoken!',5839,1,0,0,'whitemane SAY_WH_KILL'),
(-1189010,'Arise, my champion!',5840,1,0,0,'whitemane SAY_WH_RESSURECT'),

(-1189011,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1189012,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1189013,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1189014,'REUSE_ME',0,0,0,0,'REUSE_ME'),

(-1189015,'The monster got what he deserved.',0,0,1,0,'vishas SAY_TRIGGER_VORREL'),

(-1189016,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1189017,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1189018,'REUSE_ME',0,0,0,0,'REUSE_ME'),

(-1189019,'You will not defile these mysteries!',5842,1,0,0,'doan SAY_AGGRO'),
(-1189020,'Burn in righteous fire!',5843,1,0,0,'doan SAY_SPECIALAE'),

(-1189021,'REUSE_ME',0,0,0,0,'REUSE_ME'),

(-1189022,'It is over, your search is done! Let fate choose now, the righteous one.',11961,1,0,0,'horseman SAY_ENTRANCE'),
(-1189023,'Here\'s my body, fit and pure! Now, your blackened souls I\'ll cure!',12567,1,0,0,'horseman SAY_REJOINED'),
(-1189024,'So eager you are for my blood to spill, yet to vanquish me this my head you must kill!',11969,1,0,0,'horseman SAY_BODY_DEFEAT'),
(-1189025,'Over here, you idiot!',12569,1,0,0,'horseman SAY_LOST_HEAD'),
(-1189026,'Harken, cur! Tis you I spurn! Now, $N, feel the burn!',12573,1,0,0,'horseman SAY_CONFLAGRATION'),
(-1189027,'Soldiers arise, stand and fight! Bring victory at last to this fallen knight!',11963,1,0,0,'horseman SAY_SPROUTING_PUMPKINS'),
(-1189028,'Your body lies beaten, battered and broken. Let my curse be your own, fate has spoken.',11962,1,0,0,'horseman SAY_SLAY'),
(-1189029,'This end have I reached before. What new adventure lies in store?',11964,1,0,0,'horseman SAY_DEATH'),
(-1189030,'%s laughs.',0,2,0,0,'horseman EMOTE_LAUGH'),
(-1189031,'Horseman rise...',0,0,0,0,'horseman SAY_PLAYER1'),
(-1189032,'Your time is night...',0,0,0,0,'horseman SAY_PLAYER2'),
(-1189033,'You felt death once...',0,0,0,0,'horseman SAY_PLAYER3'),
(-1189034,'Now, know demise!',0,0,0,0,'horseman SAY_PLAYER4'),

(-1189035,'The master has fallen! Avenge him my brethren!',5834,1,0,0,'trainee SAY_TRAINEE_SPAWN');

-- -1 209 000 ZUL'FARRAK
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1209000,'How dare you enter my sanctum!',0,0,0,0,'zumrah SAY_INTRO'),
(-1209001,'Sands consume you!',5872,1,14,0,'zumrah SAY_AGGRO'),
(-1209002,'Fall!',5873,1,14,0,'zumrah SAY_KILL'),
(-1209003,'Come to me, my children!',0,0,8,0,'zumrah SAY_SUMMON');

-- -1 229 000 BLACKROCK SPIRE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1229000,'%s begins to regain its strength!',0,2,0,0,'pyroguard EMOTE_BEGIN'),
(-1229001,'%s is nearly at full strength!',0,2,0,0,'pyroguard EMOTE_NEAR'),
(-1229002,'%s regains its power and breaks free of its bonds!',0,2,0,0,'pyroguard EMOTE_FULL'),
(-1229003,'Ha! Ha! Ha! Thank you for freeing me, fools. Now let me repay you by charring the flesh from your bones.',0,1,0,0,'pyroguard SAY_FREE'),

(-1229004,'Excellent... it would appear as if the meddlesome insects have arrived just in time to feed my legion. Welcome, mortals!',0,1,0,1,'nefarius SAY_INTRO_1'),
(-1229005,'Let not even a drop of their blood remain upon the arena floor, my children. Feast on their souls!',0,1,0,1,'nefarius SAY_INTRO_2'),
(-1229006,'Foolsss...Kill the one in the dress!',0,1,0,0,'nefarius SAY_ATTACK_1'),
(-1229007,'Sire, let me join the fray! I shall tear their spines out with my bare hands!',0,1,0,1,'rend SAY_REND_JOIN'),
(-1229008,'Concentrate your attacks upon the healer!',0,1,0,0,'nefarius SAY_ATTACK_2'),
(-1229009,'Inconceivable!',0,1,0,0,'nefarius SAY_ATTACK_3'),
(-1229010,'Do not force my hand, children! I shall use your hides to line my boots.',0,1,0,0,'nefarius SAY_ATTACK_4'),
(-1229011,'Defilers!',0,1,0,0,'rend SAY_LOSE_1'),
(-1229012,'Impossible!',0,1,0,0,'rend SAY_LOSE_2'),
(-1229013,'Your efforts will prove fruitless. None shall stand in our way!',0,1,0,0,'nefarius SAY_LOSE_3'),
(-1229014,'THIS CANNOT BE!!! Rend, deal with these insects.',0,1,0,1,'nefarius SAY_LOSE_4'),
(-1229015,'With pleasure...',0,1,0,0,'rend SAY_REND_ATTACK'),
(-1229016,'The Warchief shall make quick work of you, mortals. Prepare yourselves!',0,1,0,25,'nefarius SAY_WARCHIEF'),
(-1229017,'Taste in my power!',0,1,0,0,'nefarius SAY_BUFF_GYTH'),
(-1229018,'Your victory shall be short lived. The days of both the Alliance and Horde are coming to an end. The next time we meet shall be the last.',0,1,0,1,'nefarius SAY_VICTORY'),

(-1229019,'%s is knocked off his drake!',0,2,0,0,'rend EMOTE_KNOCKED_OFF'),

(-1229020,'Intruders are destroying our eggs! Stop!!',0,1,0,0,'rookery hatcher - SAY_ROOKERY_EVENT_START');

-- -1 230 000 BLACKROCK DEPTHS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1230000,'Ah, hits the spot!',0,0,0,5,'rocknot SAY_GOT_BEER'),
(-1230001,'Come to aid the Throne!',0,1,0,0,'dagran SAY_AGGRO'),
(-1230002,'Hail to the king, baby!',0,1,0,0,'dagran SAY_SLAY'),
(-1230003,'You have challenged the Seven, and now you will die!',0,0,0,0,'doomrel SAY_DOOMREL_START_EVENT'),

(-1230004,'The Sons of Thaurissan shall watch you perish in the Ring of the Law!',0,1,0,5,'grimstone SAY_START_1'),
(-1230005,'You have been sentenced to death for crimes against the Dark Iron Nation!',0,1,0,25,'grimstone SAY_START_2'),
(-1230006,'Unleash the fury and let it be done!',0,1,0,15,'grimstone SAY_OPEN_EAST_GATE'),
(-1230007,'But your real punishment lies ahead.',0,1,0,1,'grimstone SAY_SUMMON_BOSS_1'),
(-1230008,'Haha! I bet you thought you were done!',0,1,0,153,'grimstone SAY_SUMMON_BOSS_2'),
(-1230009,'Good Riddance!',0,1,0,5,'grimstone SAY_OPEN_NORTH_GATE'),

(-1230010,'Thank you, $N! I\'m free!!!',0,0,0,0,'dughal SAY_FREE'),
(-1230011,'You locked up the wrong Marshal, $N. Prepare to be destroyed!',0,0,0,0,'windsor SAY_AGGRO_1'),
(-1230012,'I bet you\'re sorry now, aren\'t you?',0,0,0,0,'windsor SAY_AGGRO_2'),
(-1230013,'You better hold me back or $N is going to feel some prison house beatings.',0,0,0,0,'windsor SAY_AGGRO_3'),
(-1230014,'Let\'s get a move on. My gear should be in the storage area up this way...',0,0,0,0,'windsor SAY_START'),
(-1230015,'Check that cell, $N. If someone is alive in there, we need to get them out.',0,0,0,25,'windsor SAY_CELL_DUGHAL_1'),
(-1230016,'Good work! We\'re almost there, $N. This way.',0,0,0,0,'windsor SAY_CELL_DUGHAL_3'),
(-1230017,'This is it, $N. My stuff should be in that room. Cover me, I\'m going in!',0,0,0,0,'windsor SAY_EQUIPMENT_1'),
(-1230018,'Ah, there it is!',0,0,0,0,'windsor SAY_EQUIPMENT_2'),
(-1230019,'Can you feel the power, $N??? It\'s time to ROCK!',0,0,0,0,'reginald_windsor SAY__EQUIPMENT_3'),
(-1230020,'Now we just have to free Tobias and we can get out of here. This way!',0,0,0,0,'reginald_windsor SAY__EQUIPMENT_4'),
(-1230021,'Open it.',0,0,0,25,'reginald_windsor SAY_CELL_JAZ_1'),
(-1230022,'I never did like those two. Let\'s get moving.',0,0,0,0,'reginald_windsor SAY_CELL_JAZ_2'),
(-1230023,'Open it and be careful this time!',0,0,0,25,'reginald_windsor SAY_CELL_SHILL_1'),
(-1230024,'That intolerant dirtbag finally got what was coming to him. Good riddance!',0,0,0,66,'reginald_windsor SAY_CELL_SHILL_2'),
(-1230025,'Alright, let\'s go.',0,0,0,0,'reginald_windsor SAY_CELL_SHILL_3'),
(-1230026,'Open it. We need to hurry up. I can smell those Dark Irons coming a mile away and I can tell you one thing, they\'re COMING!',0,0,0,25,'reginald_windsor SAY_CELL_CREST_1'),
(-1230027,'He has to be in the last cell. Unless... they killed him.',0,0,0,0,'reginald_windsor SAY_CELL_CREST_2'),
(-1230028,'Get him out of there!',0,0,0,25,'reginald_windsor SAY_CELL_TOBIAS_1'),
(-1230029,'Excellent work, $N. Let\'s find the exit. I think I know the way. Follow me!',0,0,0,0,'reginald_windsor SAY_CELL_TOBIAS_2'),
(-1230030,'We made it!',0,0,0,4,'reginald_windsor SAY_FREE_1'),
(-1230031,'Meet me at Maxwell\'s encampment. We\'ll go over the next stages of the plan there and figure out a way to decode my tablets without the decryption ring.',0,0,0,1,'reginald_windsor SAY_FREE_2'),
(-1230032,'Thank you! I will run for safety immediately!',0,0,0,0,'tobias SAY_TOBIAS_FREE_1'),
(-1230033,'Finally!! I can leave this dump.',0,0,0,0,'tobias SAY_TOBIAS_FREE_2'),

(-1230034,'You\'ll pay for this insult, $c!',0,0,0,15,'coren direbrew SAY_AGGRO'),

(-1230035,'%s cries out an alarm!',0,2,0,0,'general_angerforge EMOTE_ALARM'),

(-1230036,'I want more ale! Give me more ale!',0,0,0,0,'rocknot SAY_MORE_BEER'),
(-1230037,'You\'ll pay for that!',0,0,0,0,'Grim Patron SAY_PISSED_PATRON_1'),
(-1230038,'No!',0,0,0,0,'Grim Patron SAY_PISSED_PATRON_2'),
(-1230039,'Oh! Now I\'m pissed!',0,0,0,0,'Grim Patron SAY_PISSED_PATRON_3'),
(-1230040,'Violence! Property damage! None shall pass!!',0,1,0,0,'Phalanx YELL_PHALANX_AGGRO'),
(-1230041,'Get away from those kegs!',0,1,0,0,'Hurley Blackbreath YELL_HURLEY_SPAWN'),
(-1230042,'You\'ll pay for that!',0,0,0,0,'Hurley Blackbreath SAY_HURLEY_AGGRO'),
(-1230043,'You can\'t hide from us. Prepare to burn!',0,1,0,0,'anvilrage guardsman SAY_GUARD_AGGRO'),
(-1230044,'That one\'s empty!!',0,0,0,60,'rocknot SAY_BARREL_1'),
(-1230045,'Ah, empty again!',0,0,0,35,'rocknot SAY_BARREL_2'),
(-1230046,'ALE!',0,0,0,60,'rocknot SAY_BARREL_3'),
(-1230047,'I\m out of here!',0,0,0,0,'rocknot SAY_ROCKNOT_DESPAWN'),
(-1230048,'HALT! There were...reports...of a...disturbance.',0,1,0,0,'Fireguard Destroyer YELL_PATROL_1'),
(-1230049,'We...are...investigating.',0,1,0,0,'Fireguard Destroyer YELL_PATROL_2'),
(-1230050,'Try the boar! It\'s my new recipe!',0,0,0,0,'Plugger Spazzring SAY_OOC_1'),
(-1230051,'Enjoy! You won\'t find better ale anywhere!',0,0,0,0,'Plugger Spazzring SAY_OOC_2'),
(-1230052,'Drink up! There\'s more where that came from!',0,0,0,0,'Plugger Spazzring SAY_OOC_3'),
(-1230053,'Have you tried the Dark Iron Ale? It\'s the best!',0,0,0,0,'Plugger Spazzring SAY_OOC_4'),
(-1230054,'What are you doing over there?',0,1,0,0,'Plugger Spazzring YELL_STOLEN_1'),
(-1230055,'Hey! Get away from that!',0,1,0,0,'Plugger Spazzring YELL_STOLEN_2'),
(-1230056,'No stealing the goods!',0,1,0,0,'Plugger Spazzring YELL_STOLEN_3'),
(-1230057,'That\'s it! You\'re going down!',0,1,0,0,'Plugger Spazzring YELL_AGRRO_1'),
(-1230058,'That\'s it! No more beer until this mess is sorted out!',0,1,0,0,'Plugger Spazzring YELL_AGRRO_2'),
(-1230059,'Hey, my pockets were picked!',0,1,0,0,'Plugger Spazzring YELL_PICKPOCKETED'),
(-1230060,'Ha! You can\'t even begin to imagine the futility of your efforts.',0,1,0,0,'Emperor Thaurissan YELL_SENATOR_1'),
(-1230061,'Is that the best you can do? Do you really expect that you could defeat someone as awe inspiring as me?',0,1,0,0,'Emperor Thaurissan YELL_SENATOR_2'),
(-1230062,'They were just getting in the way anyways.',0,1,0,0,'Emperor Thaurissan YELL_SENATOR_3'),
(-1230063,'Your efforts are utterly pointless, fools! You will never be able to defeat me!',0,1,0,0,'Emperor Thaurissan YELL_SENATOR_4'),
(-1230064,'I will crush you into little tiny pieces!',0,1,0,0,'Emperor Thaurissan YELL_AGGRO_2'),
(-1230065,'Prepare to meet your doom at the hands of Ragnaros\' most powerful servant!',0,1,0,0,'Emperor Thaurissan YELL_AGGRO_3'),
(-1230066,'Hey, Rocknot!',0,0,0,0,'Nagmara SAY_NAGMARA_1'),
(-1230067,'Let\'s go, honey.',0,0,0,0,'Nagmara SAY_NAGMARA_2'),
(-1230068,'%s kisses her lover.',0,2,0,0,'Nagmara TEXTEMOTE_NAGMARA'),
(-1230069,'%s kisses Mistress Nagmara',0,2,0,17,'Rocknot TEXTEMOTE_ROCKNOT');

-- -1 249 000 ONYXIA'S LAIR
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1249000,'How fortuitous. Usually, I must leave my lair to feed.',0,1,0,0,'onyxia SAY_AGGRO'),
(-1249001,'Learn your place mortal!',0,1,0,0,'onyxia SAY_KILL'),
(-1249002,'This meaningless exertion bores me. I\'ll incinerate you all from above!',0,1,0,254,'onyxia SAY_PHASE_2_TRANS'),
(-1249003,'It seems you\'ll need another lesson, mortals!',0,1,0,293,'onyxia SAY_PHASE_3_TRANS'),
(-1249004,'%s takes in a deep breath...',0,3,0,0,'onyxia EMOTE_BREATH');

-- -1 269 000 OPENING OF THE DARK PORTAL (BLACK MORASS)
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1269000,'Why do you persist? Surely you can see the futility of it all. It is not too late! You may still leave with your lives ...',10442,1,0,0,'temporus SAY_ENTER'),
(-1269001,'So be it ... you have been warned.',10444,1,0,0,'temporus SAY_AGGRO'),
(-1269002,'Time... sands of time is run out for you.',10443,1,0,0,'temporus SAY_BANISH'),
(-1269003,'You should have left when you had the chance.',10445,1,0,0,'temporus SAY_SLAY1'),
(-1269004,'Your days are done.',10446,1,0,0,'temporus SAY_SLAY2'),
(-1269005,'My death means ... little.',10447,1,0,0,'temporus SAY_DEATH'),

(-1269006,'Why do you aid the Magus? Just think of how many lives could be saved if the portal is never opened, if the resulting wars could be erased ...',10412,1,0,0,'chrono_lord_deja SAY_ENTER'),
(-1269007,'If you will not cease this foolish quest, then you will die!',10414,1,0,0,'chrono_lord_deja SAY_AGGRO'),
(-1269008,'You have outstayed your welcome, Timekeeper. Begone!',10413,1,0,0,'chrono_lord_deja SAY_BANISH'),
(-1269009,'I told you it was a fool\'s quest!',10415,1,0,0,'chrono_lord_deja SAY_SLAY1'),
(-1269010,'Leaving so soon?',10416,1,0,0,'chrono_lord_deja SAY_SLAY2'),
(-1269011,'Time ... is on our side.',10417,1,0,0,'chrono_lord_deja SAY_DEATH'),

(-1269012,'The time has come to shatter this clockwork universe forever! Let us no longer be slaves of the hourglass! I warn you: those who do not embrace the greater path shall become victims of its passing!',10400,1,0,0,'aeonus SAY_ENTER'),
(-1269013,'Let us see what fate lays in store...',10402,1,0,0,'aeonus SAY_AGGRO'),
(-1269014,'Your time is up, slave of the past!',10401,1,0,0,'aeonus SAY_BANISH'),
(-1269015,'One less obstacle in our way!',10403,1,0,0,'aeonus SAY_SLAY1'),
(-1269016,'No one can stop us! No one!',10404,1,0,0,'aeonus SAY_SLAY2'),
(-1269017,'It is only a matter...of time.',10405,1,0,0,'aeonus SAY_DEATH'),
(-1269018,'REUSE ME',0,0,0,0,'REUSE ME'),

(-1269019,'Stop! Do not go further, mortals. You are ill-prepared to face the forces of the Infinite Dragonflight. Come, let me help you.',0,0,0,0,'saat SAY_SAAT_WELCOME'),

(-1269020,'The time has come! Gul\'dan, order your warlocks to double their efforts! Moments from now the gateway will open, and your Horde will be released upon this ripe, unsuspecting world!',10435,1,0,0,'medivh SAY_ENTER'),
(-1269021,'What is this? Champions, coming to my aid? I sense the hand of the dark one in this. Truly this sacred event bears his blessing?',10436,1,0,0,'medivh SAY_INTRO'),
(-1269022,'Champions, my shield grows weak!',10437,1,0,0,'medivh SAY_WEAK75'),
(-1269023,'My powers must be concentrated on the portal! I do not have time to hold the shield!',10438,1,0,0,'medivh SAY_WEAK50'),
(-1269024,'The shield is nearly gone! All that I have worked for is in danger!',10439,1,0,0,'medivh SAY_WEAK25'),
(-1269025,'No... damn this feeble mortal coil...',10441,1,0,0,'medivh SAY_DEATH'),
(-1269026,'I am grateful for your aid, champions. Now, Gul\'dan\'s Horde will sweep across this world, like a locust swarm, and all my designs, all my carefully laid plans will at last fall into place.',10440,1,0,0,'medivh SAY_WIN'),
(-1269027,'Orcs of the Horde! This portalis the gateway to your new destiny! Azeroth lies before you, ripe for the taking!',0,1,0,0,'medivh SAY_ORCS_ENTER'),
(-1269028,'Gul\'dan speaks the truth! We should return at once to tell our brothers of the news! Retreat back trought the portal!',0,1,0,0,'medivh SAY_ORCS_ANSWER');

-- -1 289 000 SCHOLOMANCE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1289000,'School is in session!',0,1,0,0,'gandling SAY_GANDLING_SPAWN');

-- -1 309 000 ZUL'GURUB
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1309000,'Let the coils of hate unfurl!',8421,1,0,0,'venoxis SAY_TRANSFORM'),
(-1309001,'Ssserenity..at lassst!',0,1,0,0,'venoxis SAY_DEATH'),

(-1309002,'Lord Hir\'eek, grant me wings of vengance!',8417,1,0,0,'jeklik SAY_AGGRO'),
(-1309003,'I command you to rain fire down upon these invaders!',0,1,0,0,'jeklik SAY_RAIN_FIRE'),
(-1309004,'Finally ...death. Curse you Hakkar! Curse you!',8422,1,0,0,'jeklik SAY_DEATH'),

(-1309005,'Draw me to your web mistress Shadra. Unleash your venom!',8418,1,0,0,'marli SAY_AGGRO'),
(-1309006,'Shadra, make of me your avatar!',0,1,0,0,'marli SAY_TRANSFORM'),
(-1309007,'Aid me my brood!',0,1,0,0,'marli SAY_SPIDER_SPAWN'),
(-1309008,'Bless you mortal for this release. Hakkar controls me no longer...',8423,1,0,0,'marli SAY_DEATH'),

(-1309009,'Shirvallah, fill me with your RAGE!',8419,1,0,0,'thekal SAY_AGGRO'),
(-1309010,'Hakkar binds me no more! Peace at last!',8424,1,0,0,'thekal SAY_DEATH'),

(-1309011,'Bethekk, your priestess calls upon your might!',8416,1,0,0,'arlokk SAY_AGGRO'),
(-1309012,'Feast on $n, my pretties!',0,1,0,0,'arlokk SAY_FEAST_PANTHER'),
(-1309013,'At last, I am free of the Soulflayer!',8412,1,0,0,'arlokk SAY_DEATH'),

(-1309014,'Welcome to da great show friends! Step right up to die!',8425,1,0,0,'jindo SAY_AGGRO'),

(-1309015,'I\'ll feed your souls to Hakkar himself!',8413,1,0,0,'mandokir SAY_AGGRO'),
(-1309016,'DING!',0,1,0,0,'mandokir SAY_DING_KILL'),
(-1309017,'GRATS!',0,1,0,0,'mandokir SAY_GRATS_JINDO'),
(-1309018,'$N! I\'m watching you!',0,1,0,0,'mandokir SAY_WATCH'),
(-1309019,'Don\'t make me angry. You won\'t like it when I\'m angry.',0,4,0,0,'mandokir SAY_WATCH_WHISPER'),

(-1309020,'PRIDE HERALDS THE END OF YOUR WORLD. COME, MORTALS! FACE THE WRATH OF THE SOULFLAYER!',8414,1,0,0,'hakkar SAY_AGGRO'),
(-1309021,'Fleeing will do you no good, mortals!',0,1,0,0,'hakkar SAY_FLEEING'),
(-1309022,'You dare set foot upon Hakkari holy ground? Minions of Hakkar, destroy the infidels!',0,6,0,0,'hakkar SAY_MINION_DESTROY'),
(-1309023,'Minions of Hakkar, hear your God. The sanctity of this temple has been compromised. Invaders encroach upon holy ground! The Altar of Blood must be protected. Kill them all!',0,6,0,0,'hakkar SAY_PROTECT_ALTAR'),

(-1309024,'%s goes into a rage after seeing his raptor fall in battle!',0,2,0,0,'mandokir EMOTE_RAGE'),

(-1309025,'The brood shall not fall!',0,1,0,0,'marli SAY_TRANSFORM_BACK'),

(-1309026,'%s emits a deafening shriek!',0,2,0,0,'jeklik SAY_SHRIEK'),
(-1309027,'%s begins to cast a Great Heal!',0,2,0,0,'jeklik SAY_HEAL');

-- -1 329 000 STRATHOLME
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1329000,'Thanks to Egan',0,0,0,0,'freed_soul SAY_ZAPPED0'),
(-1329001,'Rivendare must die',0,0,0,0,'freed_soul SAY_ZAPPED1'),
(-1329002,'Who you gonna call?',0,0,0,0,'freed_soul SAY_ZAPPED2'),
(-1329003,'Don\'t cross those beams!',0,0,0,0,'freed_soul SAY_ZAPPED3'),

(-1329004,'An Ash\'ari Crystal has fallen! Stay true to the Lich King, my brethren, and attempt to resummon it.',0,6,0,0,'thuzadin acolyte SAY_ANNOUNCE_ZIGGURAT_1'),
(-1329005,'One of the Ash\'ari Crystals has been destroyed! Slay the intruders!',0,6,0,0,'thuzadin acolyte SAY_ANNOUNCE_ZIGGURAT_2'),
(-1329006,'An Ash\'ari Crystal has been toppled! Restore the ziggurat before the Necropolis is vulnerable!',0,6,0,0,'thuzadin acolyte SAY_ANNOUNCE_ZIGGURAT_3'),
(-1329007,'The Ash\'ari Crystals have been destroyed! The Slaughterhouse is vulnerable!',0,6,0,0,'baron rivendare SAY_ANNOUNCE_RIVENDARE'),

(-1329008,'Intruders at the Service Gate! Lord Rivendare must be warned!',0,6,0,0,'barthilas SAY_WARN_BARON'),
(-1329009,'Intruders! More pawns of the Argent Dawn, no doubt. I already count one of their number among my prisoners. Withdraw from my domain before she is executed!',0,6,0,0,'baron rivendare SAY_ANNOUNCE_RUN_START'),
(-1329010,'You\'re still here? Your foolishness is amusing! The Argent Dawn wench needn\'t suffer in vain. Leave at once and she shall be spared!',0,6,0,0,'baron rivendare SAY_ANNOUNCE_RUN_10_MIN'),
(-1329011,'I shall take great pleasure in taking this poor wretch\'s life! It\'s not too late, she needn\'t suffer in vain. Turn back and her death shall be merciful!',0,6,0,0,'baron rivendare SAY_ANNOUNCE_RUN_5_MIN'),
(-1329012,'May this prisoner\'s death serve as a warning. None shall defy the Scourge and live!',0,6,0,0,'baron rivendare SAY_ANNOUNCE_RUN_FAIL'),
(-1329013,'So you see fit to toy with the Lich King\'s creations? Ramstein, be sure to give the intruders a proper greeting.',0,6,0,0,'baron rivendare SAY_ANNOUNCE_RAMSTEIN'),
(-1329014,'Time to take matters into my own hands. Come. Enter my domain and challenge the might of the Scourge!',0,6,0,0,'baron rivendare SAY_UNDEAD_DEFEAT'),
(-1329015,'You did it... you\'ve slain Baron Rivendare! The Argent Dawn shall hear of your valiant deeds!',0,0,0,0,'ysida SAY_EPILOGUE'),

(-1329016,'Today you have unmade what took me years to create! For this you shall all die by my hand!',0,1,0,0,'dathrohan SAY_AGGRO'),
(-1329017,'You fools think you can defeat me so easily? Face the true might of the Nathrezim!',0,1,0,0,'dathrohan SAY_TRANSFORM'),
(-1329018,'Damn you mortals! All my plans of revenge, all my hate... all burned to ash...',0,0,0,0,'dathrohan SAY_DEATH'),
(-1329019,'Don\'t worry about me!  Slay this dreadful beast and cleanse this world of his foul taint!',0,6,0,0,'ysida YSIDA_SAY_RUN_5_MIN'),
(-1329020,'My death means nothing.... light... will... prevail!',0,6,0,0,'ysida YSIDA_SAY_RUN_FAIL'),

(-1329021,'Rivendare! I come for you!',0,1,0,0,'aurius YELL_AURIUS_AGGRO'),
(-1329022,'Argh!',0,0,0,0,'aurius SAY_AURIUS_DEATH'),

(-1329023,'Move back and hold the line! We cannot fail or all will be lost!',0,1,0,0,'YELL_BASTION_BARRICADE'),
(-1329024,'Move to the stairs and defend!',0,1,0,0,'YELL_BASTION_STAIRS'),
(-1329025,'The Scourge have broken into the Bastion! Redouble your efforts! We must not fail!',0,1,0,0,'YELL_BASTION_ENTRANCE'),
(-1329026,'They have broken into the Hall of Lights! We must stop the intruders!',0,1,0,0,'YELL_BASTION_HALL_LIGHTS'),
(-1329027,'The Scourge have broken through in all wings! May the light defeat these foul creatures! We shall fight to the last!',0,1,0,0,'YELL_BASTION_INNER_1'),
(-1329028,'This will not be the end of the Scarlet Crusade! You will not break our line!',0,1,0,0,'YELL_BASTION_INNER_2');

-- -1 349 000 MARAUDON

-- -1 389 000 RAGEFIRE CHASM

-- -1 409 000 MOLTEN CORE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1409000,'%s performs one last service for Ragnaros.',0,2,0,0,'geddon EMOTE_SERVICE'),
(-1409001,'REUSE ME',0,0,0,0,'REUSE ME'),
(-1409002,'%s refuses to die while its master is in trouble.',0,2,0,0,'core rager EMOTE_LOWHP'),

(-1409003,'Reckless mortals! None may challenge the Sons of the Living flame!',8035,1,0,0,'majordomo SAY_AGGRO'),
(-1409004,'The runes of warding have been destroyed! Hunt down the infidels, my brethren!',8039,6,0,0,'majordomo SAY_SPAWN'),
(-1409005,'Ashes to Ashes!',8037,1,0,0,'majordomo SAY_SLAY_1'),
(-1409006,'Burn mortals! Burn for this transgression!',8036,1,0,0,'majordomo SAY_SLAY_2'),
(-1409007,'Impossible! Stay your attack, mortals... I submit! I submit!',8038,1,0,0,'majordomo SAY_DEFEAT_1'),

(-1409008,'Behold Ragnaros - the Firelord! He who was ancient when this world was young! Bow before him, mortals! Bow before your ending!',8040,1,0,0,'ragnaros SAY_SUMMON_MAJ'),
(-1409009,'TOO SOON! YOU HAVE AWAKENED ME TOO SOON, EXECUTUS! WHAT IS THE MEANING OF THIS INTRUSION???',8043,1,0,0,'ragnaros SAY_ARRIVAL1_RAG'),
(-1409010,'These mortal infidels, my lord! They have invaded your sanctum and seek to steal your secrets!',8041,1,0,0,'ragnaros SAY_ARRIVAL2_MAJ'),
(-1409011,'FOOL! YOU ALLOWED THESE INSECTS TO RUN RAMPANT THROUGH THE HALLOWED CORE? AND NOW YOU LEAD THEM TO MY VERY LAIR? YOU HAVE FAILED ME, EXECUTUS! JUSTICE SHALL BE MET, INDEED!',8044,1,0,0,'ragnaros SAY_ARRIVAL3_RAG'),
(-1409012,'NOW FOR YOU, INSECTS! BOLDLY, YOU SOUGHT THE POWER OF RAGNAROS. NOW YOU SHALL SEE IT FIRSTHAND!',8045,1,0,0,'ragnaros SAY_ARRIVAL5_RAG'),

(-1409013,'COME FORTH, MY SERVANTS! DEFEND YOUR MASTER!',8049,1,0,0,'ragnaros SAY_REINFORCEMENTS1'),
(-1409014,'YOU CANNOT DEFEAT THE LIVING FLAME! COME YOU MINIONS OF FIRE! COME FORTH YOU CREATURES OF HATE! YOUR MASTER CALLS!',8050,1,0,0,'ragnaros SAY_REINFORCEMENTS2'),
(-1409015,'BY FIRE BE PURGED!',8046,1,0,0,'ragnaros SAY_HAND'),
(-1409016,'TASTE THE FLAMES OF SULFURON!',8047,1,0,0,'ragnaros SAY_WRATH'),
(-1409017,'DIE INSECT!',8051,1,0,0,'ragnaros SAY_KILL'),
(-1409018,'MY PATIENCE IS DWINDLING! COME GNATS: TO YOUR DEATH!',8048,1,0,0,'ragnaros SAY_MAGMABURST'),

(-1409019,'You think you\'ve won already? Perhaps you\'ll need another lesson in pain!',0,1,0,0,'majordomo SAY_LAST_ADD'),
(-1409020,'Brashly, you have come to wrest the secrets of the Living Flame! You will soon regret the recklessness of your quest.',0,1,0,0,'majordomo SAY_DEFEAT_2'),
(-1409021,'I go now to summon the lord whose house this is. Should you seek an audience with him, your paltry lives will surely be forfeit! Nevertheless, seek out his lair, if you dare!',0,1,0,0,'majordomo SAY_DEFEAT_3'),
(-1409022,'My flame! Please don\'t take away my flame... ',8042,1,0,0,'ragnaros SAY_ARRIVAL4_MAJ'),
(-1409023,'Very well, $n.',0,0,0,0,'majordomo SAY_SUMMON_0'),
(-1409024,'Impudent whelps! You\'ve rushed headlong to your own deaths! See now, the master stirs!',0,1,0,0,'majordomo SAY_SUMMON_1'),
(-1409025,'%s forces one of his Firesworn minions to erupt!',0,2,0,0,'Garr EMOTE_MASSIVE_ERUPTION');

-- -1 429 000 DIRE MAUL
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1429000,'The demon is loose! Quickly we must restrain him!',0,6,0,0,'highborne summoner SAY_FREE_IMMOLTHAR'),
(-1429001,'Who dares disrupt the sanctity of Eldre\'Thalas? Face me, cowards!',0,6,0,0,'prince tortheldrin SAY_KILL_IMMOLTHAR'),

(-1429002,'At last... Freed from his cursed grasp!',0,6,0,0,'old ironbark SAY_IRONBARK_REDEEM'),

(-1429003,'The king is dead - OH NOES! Summon Mizzle da Crafty! He knows what to do next!',0,1,0,0,'cho\'rush SAY_KING_DEAD');

-- -1 469 000 BLACKWING LAIR
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1469000,'None of your kind should be here! You\'ve doomed only yourselves!',8286,1,0,0,'broodlord SAY_AGGRO'),
(-1469001,'Clever Mortals but I am not so easily lured away from my sanctum!',8287,1,0,0,'broodlord SAY_LEASH'),

(-1469002,'Run! They are coming!',0,1,0,0,'vaelastrasz blackwing tech SAY_INTRO_TECH'),
(-1469003,'%s flinches as its skin shimmers.',0,2,0,0,'chromaggus EMOTE_SHIMMER'),

(-1469004,'In this world where time is your enemy, it is my greatest ally. This grand game of life that you think you play in fact plays you. To that I say...',0,1,0,1,'victor_nefarius SAY_GAMESBEGIN_1'),
(-1469005,'Let the games begin!',8280,1,0,22,'victor_nefarius SAY_GAMESBEGIN_2'),
(-1469006,'Ah...the heroes. You are persistent, aren\'t you? Your ally here attempted to match his power against mine - and paid the price. Now he shall serve me...by slaughtering you.',8279,1,0,23,'victor_nefarius SAY_NEFARIUS_CORRUPT'),

(-1469007,'Well done, my minions. The mortals\' courage begins to wane! Now, let\'s see how they contend with the true Lord of Blackrock Spire!',8288,1,0,0,'nefarian SAY_AGGRO'),
(-1469008,'Enough! Now you vermin shall feel the force of my birthright, the fury of the earth itself.',8289,1,0,0,'nefarian SAY_XHEALTH'),
(-1469009,'BURN! You wretches! BURN!',8290,1,0,0,'nefarian SAY_SHADOWFLAME'),
(-1469010,'Impossible! Rise my minions! Serve your master once more!',8291,1,0,0,'nefarian SAY_RAISE_SKELETONS'),
(-1469011,'Worthless $N! Your friends will join you soon enough!',8293,1,0,0,'nefarian SAY_SLAY'),
(-1469012,'This cannot be!  I am the master here!  You mortals are nothing to my kind!  Do you hear me? Nothing!',8292,1,0,0,'nefarian SAY_DEATH'),
(-1469013,'Mages too? You should be more careful when you play with magic...',0,1,0,0,'nefarian SAY_MAGE'),
(-1469014,'Warriors, I know you can hit harder than that! Let\'s see it!',0,1,0,0,'nefarian SAY_WARRIOR'),
(-1469015,'Druids and your silly shapeshifting. Let\'s see it in action!',0,1,0,0,'nefarian SAY_DRUID'),
(-1469016,'Priests! If you\'re going to keep healing like that, we might as well make it a little more interesting!',0,1,0,0,'nefarian SAY_PRIEST'),
(-1469017,'Paladins, I\'ve heard you have many lives. Show me.',0,1,0,0,'nefarian SAY_PALADIN'),
(-1469018,'Shamans, show me what your totems can do!',0,1,0,0,'nefarian SAY_SHAMAN'),
(-1469019,'Warlocks, you shouldn\'t be playing with magic you don\'t understand. See what happens?',0,1,0,0,'nefarian SAY_WARLOCK'),
(-1469020,'Hunters and your annoying pea-shooters!',0,1,0,0,'nefarian SAY_HUNTER'),
(-1469021,'Rogues? Stop hiding and face me!',0,1,0,0,'nefarian SAY_ROGUE'),

(-1469022,'You\'ll pay for forcing me to do this.',8275,1,0,0,'razorgore SAY_EGGS_BROKEN1'),
(-1469023,'Fools! These eggs are more precious than you know!',8276,1,0,0,'razorgore SAY_EGGS_BROKEN2'),
(-1469024,'No - not another one! I\'ll have your heads for this atrocity!',8277,1,0,0,'razorgore SAY_EGGS_BROKEN3'),
(-1469025,'If I fall into the abyss I\'ll take all of you mortals with me...',8278,1,0,0,'razorgore SAY_DEATH'),

(-1469026,'Too late, friends! Nefarius\' corruption has taken hold...I cannot...control myself.',8281,1,0,1,'vaelastrasz SAY_LINE1'),
(-1469027,'I beg you, mortals - FLEE! Flee before I lose all sense of control! The black fire rages within my heart! I MUST- release it!',8282,1,0,1,'vaelastrasz SAY_LINE2'),
(-1469028,'FLAME! DEATH! DESTRUCTION! Cower, mortals before the wrath of Lord...NO - I MUST fight this! Alexstrasza help me, I MUST fight it!',8283,1,0,1,'vaelastrasz SAY_LINE3'),
(-1469029,'Nefarius\' hate has made me stronger than ever before! You should have fled while you could, mortals! The fury of Blackrock courses through my veins!',8285,1,0,0,'vaelastrasz SAY_HALFLIFE'),
(-1469030,'Forgive me, $N! Your death only adds to my failure!',8284,1,0,0,'vaelastrasz SAY_KILLTARGET'),

(-1469031,'Death Knights, get over here!',0,1,0,0,'nefarian SAY_DEATH_KNIGHT'),

(-1469032,'Get up, little red wyrm...and destroy them!',0,1,0,1,'victor_nefarius SAY_NEFARIUS_CORRUPT_2'),

(-1469033,'%s flee as the controlling power of the orb is drained.',0,2,0,0,'razorgore EMOTE_TROOPS_FLEE'),

(-1469034,'Run! They are coming.',0,1,0,0,'blackwing technician SAY_TECHNICIAN_RUN'),

(-1469035,'Orb of Domination loses power and shuts off!',0,2,0,0,'razorgore EMOTE_ORB_SHUT_OFF');

-- -1 509 000 RUINS OF AHN'QIRAJ
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1509000,'%s senses your fear.',0,2,0,0,'moam EMOTE_AGGRO'),
(-1509001,'%s bristles with energy!',0,2,0,0,'moan EMOTE_MANA_FULL'),
(-1509028,'%s drains your mana and turns to stone.',0,2,0,0,'moam EMOTE_ENERGIZING'),

(-1509002,'%s sets eyes on $N!',0,2,0,0,'buru EMOTE_TARGET'),

(-1509003,'They come now. Try not to get yourself killed, young blood.',0,1,0,22,'andorov SAY_ANDOROV_INTRO_3'),
(-1509004,'Remember, Rajaxx, when I said I\'d kill you last?',0,1,0,0,'andorov SAY_ANDOROV_INTRO_1'),

(-1509005,'The time of our retribution is at hand! Let darkness reign in the hearts of our enemies!',8612,1,0,0,'rajaxx SAY_WAVE3'),
(-1509006,'No longer will we wait behind barred doors and walls of stone! No longer will our vengeance be denied! The dragons themselves will tremble before our wrath!',8610,1,0,0,'rajaxx SAY_WAVE4'),
(-1509007,'Fear is for the enemy! Fear and death!',8608,1,0,0,'rajaxx SAY_WAVE5'),
(-1509008,'Staghelm will whimper and beg for his life, just as his whelp of a son did! One thousand years of injustice will end this day!',8611,1,0,0,'rajaxx SAY_WAVE6'),
(-1509009,'Fandral! Your time has come! Go and hide in the Emerald Dream and pray we never find you!',8607,1,0,0,'rajaxx SAY_WAVE7'),
(-1509010,'Impudent fool! I will kill you myself!',8609,1,0,0,'rajaxx SAY_INTRO'),
(-1509011,'Attack and make them pay dearly!',8603,1,0,0,'rajaxx SAY_UNK1'),
(-1509012,'Crush them! Drive them out!',8605,1,0,0,'rajaxx SAY_UNK2'),
(-1509013,'Do not hesitate! Destroy them!',8606,1,0,0,'rajaxx SAY_UNK3'),
(-1509014,'Warriors! Captains! Continue the fight!',8613,1,0,0,'rajaxx SAY_UNK4'),
(-1509015,'You are not worth my time $N!',8614,1,0,0,'rajaxx SAY_DEAGGRO'),
(-1509016,'Breath your last!',8604,1,0,0,'rajaxx SAY_KILLS_ANDOROV'),
(-1509017,'Soon you will know the price of your meddling, mortals... The master is nearly whole... And when he rises, your world will be cease!',0,1,0,0,'rajaxx SAY_COMPLETE_QUEST'),

(-1509018,'I am rejuvinated!',8593,1,0,0,'ossirian SAY_SURPREME1'),
(-1509019,'My powers are renewed!',8595,1,0,0,'ossirian SAY_SURPREME2'),
(-1509020,'My powers return!',8596,1,0,0,'ossirian SAY_SURPREME3'),
(-1509021,'Protect the city at all costs!',8597,1,0,0,'ossirian SAY_RAND_INTRO1'),
(-1509022,'The walls have been breached!',8599,6,0,0,'ossirian SAY_RAND_INTRO2'),
(-1509023,'To your posts. Defend the city.',8600,1,0,0,'ossirian SAY_RAND_INTRO3'),
(-1509024,'Tresspassers will be terminated.',8601,1,0,0,'ossirian SAY_RAND_INTRO4'),
(-1509025,'Sands of the desert rise and block out the sun!',8598,1,0,0,'ossirian SAY_AGGRO'),
(-1509026,'You are terminated.',8602,1,0,0,'ossirian SAY_SLAY'),
(-1509027,'I...have...failed.',8594,1,0,0,'ossirian SAY_DEATH'),
-- 28 (above) = EMOTE_ENERGIZING
(-1509029,'Come get some!',0,0,0,0,'andorov SAY_ANDOROV_INTRO_4'),
(-1509030,'Kill first, ask questions later... Incoming!',0,1,0,0,'andorov SAY_ANDOROV_ATTACK_START'),
(-1509031,'I lied...',0,1,0,0,'andorov SAY_ANDOROV_INTRO_2');

-- -1 531 000 TEMPLE OF AHN'QIRAJ
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1531000,'Are you so eager to die? I would be happy to accomodate you.',8615,1,0,0,'skeram SAY_AGGRO1'),
(-1531001,'Cower mortals! The age of darkness is at hand.',8616,1,0,0,'skeram SAY_AGGRO2'),
(-1531002,'Tremble! The end is upon you.',8621,1,0,0,'skeram SAY_AGGRO3'),
(-1531003,'Let your death serve as an example!',8617,1,0,0,'skeram SAY_SLAY1'),
(-1531004,'Spineless wretches! You will drown in rivers of blood!',8619,1,0,0,'skeram SAY_SLAY2'),
(-1531005,'The screams of the dying will fill the air. A symphony of terror is about to begin!',8620,1,0,0,'skeram SAY_SLAY3'),
(-1531006,'Prepare for the return of the ancient ones!',8618,1,0,0,'skeram SAY_SPLIT'),
(-1531007,'You only delay... the inevetable.',8622,1,0,0,'skeram SAY_DEATH'),

(-1531008,'You will be judged for defiling these sacred grounds! The laws of the Ancients will not be challenged! Trespassers will be annihilated!',8646,1,0,0,'sartura SAY_AGGRO'),
(-1531009,'I sentence you to death!',8647,1,0,0,'sartura SAY_SLAY'),
(-1531010,'I serve to the last!',8648,1,0,0,'sartura SAY_DEATH'),

(-1531011,'%s is weakened!',0,2,0,0,'cthun EMOTE_WEAKENED'),

(-1531012,'The massive floating eyeball in the center of the chamber turns its gaze upon you. You stand before a god.',0,2,0,0,'eye cthun EMOTE_INTRO'),
(-1531013,'Only flesh and bone. Mortals are such easy prey...',0,1,0,0,'veklor SAY_INTRO_1'),
(-1531014,'Where are your manners, brother. Let us properly welcome our guests.',0,1,0,0,'veknilash SAY_INTRO_2'),
(-1531015,'There will be pain...',0,1,0,0,'veklor SAY_INTRO_3'),
(-1531016,'Oh so much pain...',0,1,0,0,'veknilash SAY_INTRO_4'),
(-1531017,'Come, little ones.',0,1,0,0,'veklor SAY_INTRO_5'),
(-1531018,'The feast of souls begin now...',0,1,0,0,'veknilash SAY_INTRO_6'),

(-1531019,'It\'s too late to turn away.',8623,1,0,0,'veklor SAY_AGGRO_1'),
(-1531020,'Prepare to embrace oblivion!',8626,1,0,0,'veklor SAY_AGGRO_2'),
(-1531021,'Like a fly in a web.',8624,1,0,0,'veklor SAY_AGGRO_3'),
(-1531022,'Your brash arrogance!',8628,1,0,0,'veklor SAY_AGGRO_4'),
(-1531023,'You will not escape death!',8629,1,0,0,'veklor SAY_SLAY'),
(-1531024,'My brother...NO!',8625,1,0,0,'veklor SAY_DEATH'),
(-1531025,'To decorate our halls!',8627,1,0,0,'veklor SAY_SPECIAL'),

(-1531026,'Ah, lambs to the slaughter!',8630,1,0,0,'veknilash SAY_AGGRO_1'),
(-1531027,'Let none survive!',8632,1,0,0,'veknilash SAY_AGGRO_2'),
(-1531028,'Join me brother, there is blood to be shed!',8631,1,0,0,'veknilash SAY_AGGRO_3'),
(-1531029,'Look brother, fresh blood!',8633,1,0,0,'veknilash SAY_AGGRO_4'),
(-1531030,'Your fate is sealed!',8635,1,0,0,'veknilash SAY_SLAY'),
(-1531031,'Vek\'lor, I feel your pain!',8636,1,0,0,'veknilash SAY_DEATH'),
(-1531032,'Shall be your undoing!',8634,1,0,0,'veknilash SAY_SPECIAL'),

(-1531033,'Death is close...',8580,4,0,0,'cthun SAY_WHISPER_1'),
(-1531034,'You are already dead.',8581,4,0,0,'cthun SAY_WHISPER_2'),
(-1531035,'Your courage will fail.',8582,4,0,0,'cthun SAY_WHISPER_3'),
(-1531036,'Your friends will abandon you.',8583,4,0,0,'cthun SAY_WHISPER_4'),
(-1531037,'You will betray your friends.',8584,4,0,0,'cthun SAY_WHISPER_5'),
(-1531038,'You will die.',8585,4,0,0,'cthun SAY_WHISPER_6'),
(-1531039,'You are weak.',8586,4,0,0,'cthun SAY_WHISPER_7'),
(-1531040,'Your heart will explode.',8587,4,0,0,'cthun SAY_WHISPER_8'),

(-1531041,'%s begins to slow!',0,2,0,0,'viscidus EMOTE_SLOW'),
(-1531042,'%s is freezing up!',0,2,0,0,'viscidus EMOTE_FREEZE'),
(-1531043,'%s is frozen solid!',0,2,0,0,'viscidus EMOTE_FROZEN'),
(-1531044,'%s begins to crack!',0,2,0,0,'viscidus EMOTE_CRACK'),
(-1531045,'%s looks ready to shatter!',0,2,0,0,'viscidus EMOTE_SHATTER'),
(-1531046,'%s explodes!',0,2,0,0,'viscidus EMOTE_EXPLODE');

-- -1 532 000 KARAZHAN
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1532000,'Well done Midnight!',9173,1,0,0,'attumen SAY_MIDNIGHT_KILL'),
(-1532001,'Cowards! Wretches!',9167,1,0,0,'attumen SAY_APPEAR1'),
(-1532002,'Who dares attack the steed of the Huntsman?',9298,1,0,0,'attumen SAY_APPEAR2'),
(-1532003,'Perhaps you would rather test yourselves against a more formidable opponent?!',9299,1,0,0,'attumen SAY_APPEAR3'),
(-1532004,'Come, Midnight, let\'s disperse this petty rabble!',9168,1,0,0,'attumen SAY_MOUNT'),
(-1532005,'It was... inevitable.',9169,1,0,0,'attumen SAY_KILL1'),
(-1532006,'Another trophy to add to my collection!',9300,1,0,0,'attumen SAY_KILL2'),
(-1532007,'Weapons are merely a convenience for a warrior of my skill!',9166,1,0,0,'attumen SAY_DISARMED'),
(-1532008,'I always knew... someday I would become... the hunted.',9165,1,0,0,'attumen SAY_DEATH'),
(-1532009,'Such easy sport.',9170,1,0,0,'attumen SAY_RANDOM1'),
(-1532010,'Amateurs! Do not think you can best me! I kill for a living.',9304,1,0,0,'attumen SAY_RANDOM2'),

(-1532011,'Hmm, unannounced visitors? Preparations must be made.',9211,1,0,0,'moroes SAY_AGGRO'),
(-1532012,'Now, where was I? Oh yes...',9215,1,0,0,'moroes SAY_SPECIAL_1'),
(-1532013,'You rang?',9316,1,0,0,'moroes SAY_SPECIAL_2'),
(-1532014,'One more for dinner this evening.',9214,1,0,0,'moroes SAY_KILL_1'),
(-1532015,'Time... Never enough time.',9314,1,0,0,'moroes SAY_KILL_2'),
(-1532016,'I\'ve gone and made a mess.',9315,1,0,0,'moroes SAY_KILL_3'),
(-1532017,'How terribly clumsy of me...',9213,1,0,0,'moroes SAY_DEATH'),

(-1532018,'Your behavior will not be tolerated!',9204,1,0,0,'maiden SAY_AGGRO'),
(-1532019,'Ah ah ah...',9207,1,0,0,'maiden SAY_SLAY1'),
(-1532020,'This is for the best.',9312,1,0,0,'maiden SAY_SLAY2'),
(-1532021,'Impure thoughts lead to profane actions.',9311,1,0,0,'maiden SAY_SLAY3'),
(-1532022,'Cast out your corrupt thoughts.',9313,1,0,0,'maiden SAY_REPENTANCE1'),
(-1532023,'Your impurity must be cleansed.',9208,1,0,0,'maiden SAY_REPENTANCE2'),
(-1532024,'Death comes. Will your conscience be clear?',9206,1,0,0,'maiden SAY_DEATH'),

(-1532025,'Oh at last, at last. I can go home.',9190,1,0,0,'dorothee SAY_DOROTHEE_DEATH'),
(-1532026,'Don\'t let them hurt us, Tito! Oh, you won\'t, will you?',9191,1,0,0,'dorothee SAY_DOROTHEE_SUMMON'),
(-1532027,'Tito, oh Tito, no!',9192,1,0,0,'dorothee SAY_DOROTHEE_TITO_DEATH'),
(-1532028,'Oh dear, we simply must find a way home! The old wizard could be our only hope! Strawman, Roar, Tinhead, will you... wait! Oh golly, look! We have visitors!',9195,1,0,0,'dorothee SAY_DOROTHEE_AGGRO'),

(-1532029,'Wanna fight? Huh? Do ya? C\'mon, I\'ll fight you with both claws behind my back!',9227,1,0,0,'roar SAY_ROAR_AGGRO'),
(-1532030,'You didn\'t have to go and do that.',9229,1,0,0,'roar SAY_ROAR_DEATH'),
(-1532031,'I think I\'m going to go take fourty winks.',9230,1,0,0,'roar SAY_ROAR_SLAY'),

(-1532032,'Now what should I do with you? I simply can\'t make up my mind.',9254,1,0,0,'strawman SAY_STRAWMAN_AGGRO'),
(-1532033,'Don\'t let them make a mattress... out of me.',9256,1,0,0,'strawman SAY_STRAWMAN_DEATH'),
(-1532034,'I guess I\'m not a failure after all.',9257,1,0,0,'strawman SAY_STRAWMAN_SLAY'),

(-1532035,'I could really use a heart. Say, can I have yours?',9268,1,0,0,'tinhead SAY_TINHEAD_AGGRO'),
(-1532036,'Back to being an old rustbucket.',9270,1,0,0,'tinhead SAY_TINHEAD_DEATH'),
(-1532037,'Guess I\'m not so rusty, after all.',9271,1,0,0,'tinhead SAY_TINHEAD_SLAY'),
(-1532038,'%s begins to rust.',0,2,0,0,'tinhead EMOTE_RUST'),

(-1532039,'Woe to each and every one of you my pretties! <cackles>',9179,1,0,0,'crone SAY_CRONE_AGGRO'),
(-1532040,'It will all be over soon! <cackles>',9307,1,0,0,'crone SAY_CRONE_AGGRO2'),
(-1532041,'How could you? What a cruel, cruel world!',9178,1,0,0,'crone SAY_CRONE_DEATH'),
(-1532042,'Fixed you, didn\'t I? <cackles>',9180,1,0,0,'crone SAY_CRONE_SLAY'),

(-1532043,'All the better to own you with!',9276,1,0,0,'wolf SAY_WOLF_AGGRO'),
(-1532044,'Mmmm... delicious.',9277,1,0,0,'wolf SAY_WOLF_SLAY'),
(-1532045,'Run away little girl, run away!',9278,1,0,0,'wolf SAY_WOLF_HOOD'),

(-1532046,'What devil art thou, that dost torment me thus?',9196,1,0,0,'julianne SAY_JULIANNE_AGGRO'),
(-1532047,'Where is my lord? Where is my Romulo?',9199,1,0,0,'julianne SAY_JULIANNE_ENTER'),
(-1532048,'Romulo, I come! Oh... this do I drink to thee!',9198,1,0,0,'julianne SAY_JULIANNE_DEATH01'),
(-1532049,'Where is my Lord? Where is my Romulo? Ohh, happy dagger! This is thy sheath! There rust, and let me die!',9310,1,0,0,'julianne SAY_JULIANNE_DEATH02'),
(-1532050,'Come, gentle night; and give me back my Romulo!',9200,1,0,0,'julianne SAY_JULIANNE_RESURRECT'),
(-1532051,'Parting is such sweet sorrow.',9201,1,0,0,'julianne SAY_JULIANNE_SLAY'),

(-1532052,'Wilt thou provoke me? Then have at thee, boy!',9233,1,0,0,'romulo SAY_ROMULO_AGGRO'),
(-1532053,'Thou smilest... upon the stroke that... murders me.',9235,1,0,0,'romulo SAY_ROMULO_DEATH'),
(-1532054,'This day\'s black fate on more days doth depend. This but begins the woe. Others must end.',9236,1,0,0,'romulo SAY_ROMULO_ENTER'),
(-1532055,'Thou detestable maw, thou womb of death; I enforce thy rotten jaws to open!',9237,1,0,0,'romulo SAY_ROMULO_RESURRECT'),
(-1532056,'How well my comfort is revived by this!',9238,1,0,0,'romulo SAY_ROMULO_SLAY'),

(-1532057,'The Menagerie is for guests only.',9183,1,0,0,'curator SAY_AGGRO'),
(-1532058,'Gallery rules will be strictly enforced.',9188,1,0,0,'curator SAY_SUMMON1'),
(-1532059,'This curator is equipped for gallery protection.',9309,1,0,0,'curator SAY_SUMMON2'),
(-1532060,'Your request cannot be processed.',9186,1,0,0,'curator SAY_EVOCATE'),
(-1532061,'Failure to comply will result in offensive action.',9185,1,0,0,'curator SAY_ENRAGE'),
(-1532062,'Do not touch the displays.',9187,1,0,0,'curator SAY_KILL1'),
(-1532063,'You are not a guest.',9308,1,0,0,'curator SAY_KILL2'),
(-1532064,'This Curator is no longer op... er... ation... al.',9184,1,0,0,'curator SAY_DEATH'),

(-1532065,'Your blood will anoint my circle.',9264,1,0,0,'terestian SAY_SLAY1'),
(-1532066,'The great one will be pleased.',9329,1,0,0,'terestian SAY_SLAY2'),
(-1532067,'My life, is yours. Oh great one.',9262,1,0,0,'terestian SAY_DEATH'),
(-1532068,'Ah, you\'re just in time. The rituals are about to begin.',9260,1,0,0,'terestian SAY_AGGRO'),
(-1532069,'Please, accept this humble offering, oh great one.',9263,1,0,0,'terestian SAY_SACRIFICE1'),
(-1532070,'Let the sacrifice serve his testament to my fealty.',9330,1,0,0,'terestian SAY_SACRIFICE2'),
(-1532071,'Come, you dwellers in the dark. Rally to my call!',9265,1,0,0,'terestian SAY_SUMMON1'),
(-1532072,'Gather, my pets. There is plenty for all.',9331,1,0,0,'terestian SAY_SUMMON2'),

(-1532073,'Please, no more. My son... he\'s gone mad!',9241,1,0,0,'aran SAY_AGGRO1'),
(-1532074,'I\'ll not be tortured again!',9323,1,0,0,'aran SAY_AGGRO2'),
(-1532075,'Who are you? What do you want? Stay away from me!',9324,1,0,0,'aran SAY_AGGRO3'),
(-1532076,'I\'ll show you this beaten dog still has some teeth!',9245,1,0,0,'aran SAY_FLAMEWREATH1'),
(-1532077,'Burn you hellish fiends!',9326,1,0,0,'aran SAY_FLAMEWREATH2'),
(-1532078,'I\'ll freeze you all!',9246,1,0,0,'aran SAY_BLIZZARD1'),
(-1532079,'Back to the cold dark with you!',9327,1,0,0,'aran SAY_BLIZZARD2'),
(-1532080,'Yes, yes, my son is quite powerful... but I have powers of my own!',9242,1,0,0,'aran SAY_EXPLOSION1'),
(-1532081,'I am not some simple jester! I am Nielas Aran!',9325,1,0,0,'aran SAY_EXPLOSION2'),
(-1532082,'Surely you would not deny an old man a replenishing drink? No, no I thought not.',9248,1,0,0,'aran SAY_DRINK'),
(-1532083,'I\'m not finished yet! No, I have a few more tricks up me sleeve.',9251,1,0,0,'aran SAY_ELEMENTALS'),
(-1532084,'I want this nightmare to be over!',9250,1,0,0,'aran SAY_KILL1'),
(-1532085,'Torment me no more!',9328,1,0,0,'aran SAY_KILL2'),
(-1532086,'You\'ve wasted enough of my time. Let these games be finished!',9247,1,0,0,'aran SAY_TIMEOVER'),
(-1532087,'At last... The nightmare is.. over...',9244,1,0,0,'aran SAY_DEATH'),
(-1532088,'Where did you get that?! Did HE send you?!',9249,1,0,0,'aran SAY_ATIESH'),

(-1532089,'%s cries out in withdrawal, opening gates to the warp.',0,3,0,0,'netherspite EMOTE_PHASE_PORTAL'),
(-1532090,'%s goes into a nether-fed rage!',0,3,0,0,'netherspite EMOTE_PHASE_BANISH'),

(-1532091,'Madness has brought you here to me. I shall be your undoing!',9218,1,0,0,'malchezaar SAY_AGGRO'),
(-1532092,'Simple fools! Time is the fire in which you\'ll burn!',9220,1,0,0,'malchezaar SAY_AXE_TOSS1'),
(-1532093,'I see the subtlety of conception is beyond primitives such as you.',9317,1,0,0,'malchezaar SAY_AXE_TOSS2'),
(-1532094,'Who knows what secrets hide in the dark.',9223,1,0,0,'malchezaar SAY_SPECIAL1'),
(-1532095,'The cerestial forces are mine to manipulate.',9320,1,0,0,'malchezaar SAY_SPECIAL2'),
(-1532096,'How can you hope to withstand against such overwhelming power?',9321,1,0,0,'malchezaar SAY_SPECIAL3'),
(-1532097,'Surely you did not think you could win.',9222,1,0,0,'malchezaar SAY_SLAY1'),
(-1532098,'Your greed, your foolishness has brought you to this end.',9318,1,0,0,'malchezaar SAY_SLAY2'),
(-1532099,'You are, but a plaything, unfit even to amuse.',9319,1,0,0,'malchezaar SAY_SLAY3'),
(-1532100,'All realities, all dimensions are open to me!',9224,1,0,0,'malchezaar SAY_SUMMON1'),
(-1532101,'You face not Malchezaar alone, but the legions I command!',9322,1,0,0,'malchezaar SAY_SUMMON2'),
(-1532102,'I refuse to concede defeat. I am a prince of the Eredar! I am...',9221,1,0,0,'malchezaar SAY_DEATH'),

(-1532103,'Welcome Ladies and Gentlemen, to this evening\'s presentation!',9174,1,0,0,'barnes OZ1'),
(-1532104,'Tonight we plumb the depths of the human soul as we join a lost, lonely girl trying desperately -- with the help of her loyal companions -- to find her way home!',9338,1,0,0,'barnes OZ2'),
(-1532105,'But she is pursued... by a wicked malevolent crone!',9339,1,0,0,'barnes OZ3'),
(-1532106,'Will she survive? Will she prevail? Only time will tell. And now ... on with the show!',9340,1,0,0,'barnes OZ4'),
(-1532107,'Good evening, Ladies and Gentlemen! Welcome to this evening\'s presentation!',9175,1,0,0,'barnes HOOD1'),
(-1532108,'Tonight, things are not what they seem. For tonight, your eyes may not be trusted',9335,1,0,0,'barnes HOOD2'),
(-1532109,'Take for instance, this quiet, elderly woman, waiting for a visit from her granddaughter. Surely there is nothing to fear from this sweet, grey-haired, old lady.',9336,1,0,0,'barnes HOOD3'),
(-1532110,'But don\'t let me pull the wool over your eyes. See for yourself what lies beneath those covers! And now... on with the show!',9337,1,0,0,'barnes HOOD4'),
(-1532111,'Welcome, Ladies and Gentlemen, to this evening\'s presentation!',9176,1,0,0,'barnes RAJ1'),
(-1532112,'Tonight, we explore a tale of forbidden love!',9341,1,0,0,'barnes RAJ2'),
(-1532113,'But beware, for not all love stories end happily, as you may find out. Sometimes, love pricks like a thorn.',9342,1,0,0,'barnes RAJ3'),
(-1532114,'But don\'t take it from me, see for yourself what tragedy lies ahead when the paths of star-crossed lovers meet. And now...on with the show!',9343,1,0,0,'barnes RAJ4'),
(-1532115,'Splendid, I\'m going to get the audience ready. Break a leg!',0,0,0,0,'barnes SAY_EVENT_START'),

(-1532116,'You\'ve got my attention, dragon. You\'ll find I\'m not as easily scared as the villagers below.',0,1,0,0,'image of medivh SAY_MEDIVH_1'),
(-1532117,'Your dabbling in the arcane has gone too far, Medivh. You\'ve attracted the attention of powers beyond your understanding. You must leave Karazhan at once!',0,1,0,0,'arcanagos SAY_ARCANAGOS_2'),
(-1532118,'You dare challenge me at my own dwelling? Your arrogance is astounding, even for a dragon.',0,1,0,0,'image of medivh SAY_MEDIVH_3'),
(-1532119,'A dark power seeks to use you, Medivh! If you stay, dire days will follow. You must hurry, we don\'t have much time!',0,1,0,0,'arcanagos SAY_ARCANAGOS_4'),
(-1532120,'I do not know what you speak of, dragon... but I will not be bullied by this display of insolence. I\'ll leave Karazhan when it suits me!',0,1,0,0,'image of medivh SAY_MEDIVH_5'),
(-1532121,'You leave me no alternative. I will stop you by force if you wont listen to reason.',0,1,0,0,'arcanagos SAY_ARCANAGOS_6'),
(-1532122,'%s begins to cast a spell of great power, weaving his own essence into the magic.',0,2,0,0,'image of medivh EMOTE_CAST_SPELL'),
(-1532123,'What have you done, wizard? This cannot be! I\'m burning from... within!',0,1,0,0,'arcanagos SAY_ARCANAGOS_7'),
(-1532124,'He should not have angered me. I must go... recover my strength now...',0,0,0,0,'image of medivh SAY_MEDIVH_8'),

(-1532125,'An ancient being awakens in the distance...',0,2,0,0,'nightbane EMOTE_AWAKEN'),
(-1532126,'What fools! I shall bring a quick end to your suffering!',0,1,0,0,'nightbane SAY_AGGRO'),
(-1532127,'Miserable vermin. I shall exterminate you from the air!',0,1,0,0,'nightbane SAY_AIR_PHASE'),
(-1532128,'Enough! I shall land and crush you myself!',0,1,0,0,'nightbane SAY_LAND_PHASE_1'),
(-1532129,'Insects! Let me show you my strength up close!',0,1,0,0,'nightbane SAY_LAND_PHASE_2'),
(-1532130,'%s takes a deep breath.',0,3,0,0,'nightbane EMOTE_DEEP_BREATH'),

(-1532131,'The halls of Karazhan shake, as the curse binding the doors of the Gamemaster\'s Hall is lifted.',0,2,0,0,'echo_of_medivh EMOTE_LIFT_CURSE'),
(-1532132,'%s cheats!',0,3,0,0,'echo_of_medivh EMOTE_CHEAT'),

(-1532133,'%s flaps down from the ceiling...',0,2,0,0,'shadikith_the_glider EMOTE_BAT_SPAWN'),
(-1532134,'%s howls through the halls...',0,2,0,0,'rokad_the_ravager EMOTE_DOG_SPAWN'),
(-1532135,'%s emerges from the shadows...',0,2,0,0,'hyakiss_the_lurker EMOTE_SPIDER_SPAWN');

-- -1 533 000 NAXXRAMAS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1533000,'Ahh... welcome to my parlor.',8788,1,0,0,'anubrekhan SAY_GREET'),
(-1533001,'Just a little taste...',8785,1,0,0,'anubrekhan SAY_AGGRO1'),
(-1533002,'There is no way out.',8786,1,0,0,'anubrekhan SAY_AGGRO2'),
(-1533003,'Yes, Run! It makes the blood pump faster!',8787,1,0,0,'anubrekhan SAY_AGGRO3'),
(-1533004,'I hear little hearts beating. Yesss... beating faster now. Soon the beating will stop.',8790,1,0,0,'anubrekhan SAY_TAUNT1'),
(-1533005,'Where to go? What to do? So many choices that all end in pain, end in death.',8791,1,0,0,'anubrekhan SAY_TAUNT2'),
(-1533006,'Which one shall I eat first? So difficult to choose... the all smell so delicious.',8792,1,0,0,'anubrekhan SAY_TAUNT3'),
(-1533007,'Closer now... tasty morsels. I\'ve been too long without food. Without blood to drink.',8793,1,0,0,'anubrekhan SAY_TAUNT4'),
(-1533008,'Shh... it will all be over soon.',8789,1,0,0,'anubrekhan SAY_SLAY'),

(-1533009,'Your old lives, your mortal desires, mean nothing. You are acolytes of the master now, and you will serve the cause without question! The greatest glory is to die in the master\'s service!',8799,1,0,0,'faerlina SAY_GREET'),
(-1533010,'Slay them in the master\'s name!',8794,1,0,0,'faerlina SAY_AGGRO1'),
(-1533011,'You cannot hide from me!',8795,1,0,0,'faerlina SAY_AGGRO2'),
(-1533012,'Kneel before me, worm!',8796,1,0,0,'faerlina SAY_AGGRO3'),
(-1533013,'Run while you still can!',8797,1,0,0,'faerlina SAY_AGGRO4'),
(-1533014,'You have failed!',8800,1,0,0,'faerlina SAY_SLAY1'),
(-1533015,'Pathetic wretch!',8801,1,0,0,'faerlina SAY_SLAY2'),
(-1533016,'The master... will avenge me!',8798,1,0,0,'faerlina SAY_DEATH'),

(-1533017,'Patchwerk want to play!',8909,1,0,0,'patchwerk SAY_AGGRO1'),
(-1533018,'Kel\'Thuzad make Patchwerk his Avatar of War!',8910,1,0,0,'patchwerk SAY_AGGRO2'),
(-1533019,'No more play?',8912,1,0,0,'patchwerk SAY_SLAY'),
(-1533020,'What happened to... Patch...',8911,1,0,0,'patchwerk SAY_DEATH'),

(-1533021,'%s sprays slime across the room!',0,3,0,0,'grobbulus EMOTE_SPRAY_SLIME'),

(-1533022,'%s lifts off into the air!',0,3,0,0,'sapphiron EMOTE_FLY'),

(-1533023,'Stalagg crush you!',8864,1,0,0,'stalagg SAY_STAL_AGGRO'),
(-1533024,'Stalagg kill!',8866,1,0,0,'stalagg SAY_STAL_SLAY'),
(-1533025,'Master save me...',8865,1,0,0,'stalagg SAY_STAL_DEATH'),
(-1533026,'Feed you to master!',8802,1,0,0,'feugen SAY_FEUG_AGGRO'),
(-1533027,'Feugen make master happy!',8804,1,0,0,'feugen SAY_FEUG_SLAY'),
(-1533028,'No... more... Feugen...',8803,1,0,0,'feugen SAY_FEUG_DEATH'),

(-1533029,'You are too late... I... must... OBEY!',8872,1,0,0,'thaddius SAY_GREET'),
(-1533030,'KILL!',8867,1,0,0,'thaddius SAY_AGGRO1'),
(-1533031,'EAT YOUR BONES!',8868,1,0,0,'thaddius SAY_AGGRO2'),
(-1533032,'BREAK YOU!',8869,1,0,0,'thaddius SAY_AGGRO3'),
(-1533033,'You die now!',8877,1,0,0,'thaddius SAY_SLAY'),
(-1533034,'Now YOU feel pain!',8871,1,0,0,'thaddius SAY_ELECT'),
(-1533035,'Thank... you...',8870,1,0,0,'thaddius SAY_DEATH'),
(-1533036,'REUSE_ME',0,0,0,0,'REUSE'),
(-1533037,'REUSE_ME',0,0,0,0,'REUSE'),
(-1533038,'REUSE_ME',0,0,0,0,'REUSE'),
(-1533039,'REUSE_ME',0,0,0,0,'REUSE'),

(-1533040,'Foolishly you have sought your own demise.',8807,1,0,0,'gothik SAY_SPEECH_1'),
(-1533041,'Death is the only escape.',8806,1,0,0,'gothik SAY_KILL'),
(-1533042,'I... am... undone!',8805,1,0,0,'gothik SAY_DEATH'),
(-1533043,'I have waited long enough! Now, you face the harvester of souls!',8808,1,0,0,'gothik SAY_TELEPORT'),

(-1533044,'Defend youself!',8892,1,0,0,'blaumeux SAY_BLAU_AGGRO'),
(-1533045,'Come, Zeliek, do not drive them out. Not before we\'ve had our fun.',8896,6,0,0,'blaumeux SAY_BLAU_TAUNT1'),
(-1533046,'I do hope they stay alive long enough for me to... introduce myself.',8897,6,0,0,'blaumeux SAY_BLAU_TAUNT2'),
(-1533047,'The first kill goes to me! Anyone care to wager?',8898,6,0,0,'blaumeux SAY_BLAU_TAUNT3'),
(-1533048,'Your life is mine!',8895,1,0,0,'blaumeux SAY_BLAU_SPECIAL'),
(-1533049,'Who\'s next?',8894,1,0,0,'blaumeux SAY_BLAU_SLAY'),
(-1533050,'Tou... che!',8893,1,0,0,'blaumeux SAY_BLAU_DEATH'),

(-1533051,'Come out and fight, ye wee ninny!',8899,1,0,0,'korthazz SAY_KORT_AGGRO'),
(-1533052,'To arms, ye roustabouts! We\'ve got company!',8903,6,0,0,'korthazz SAY_KORT_TAUNT1'),
(-1533053,'I heard about enough of yer sniveling. Shut yer fly trap \'afore I shut it for ye!',8904,6,0,0,'korthazz SAY_KORT_TAUNT2'),
(-1533054,'I\'m gonna enjoy killin\' these slack-jawed daffodils!',8905,6,0,0,'korthazz SAY_KORT_TAUNT3'),
(-1533055,'I like my meat extra crispy!',8902,1,0,0,'korthazz SAY_KORT_SPECIAl'),
(-1533056,'Next time, bring more friends!',8901,1,0,0,'korthazz SAY_KORT_SLAY'),
(-1533057,'What a bloody waste this is!',8900,1,0,0,'korthazz SAY_KORT_DEATH'),

(-1533058,'Flee, before it\'s too late!',8913,1,0,0,'zeliek SAY_ZELI_AGGRO'),
(-1533059,'Invaders, cease this foolish venture at once! Turn away while you still can!',8917,6,0,0,'zeliek SAY_ZELI_TAUNT1'),
(-1533060,'Perhaps they will come to their senses, and run away as fast as they can!',8918,6,0,0,'zeliek SAY_ZELI_TAUNT2'),
(-1533061,'Do not continue! Turn back while there\'s still time!',8919,6,0,0,'zeliek SAY_ZELI_TAUNT3'),
(-1533062,'I- I have no choice but to obey!',8916,1,0,0,'zeliek SAY_ZELI_SPECIAL'),
(-1533063,'Forgive me!',8915,1,0,0,'zeliek SAY_ZELI_SLAY'),
(-1533064,'It is... as it should be.',8914,1,0,0,'zeliek SAY_ZELI_DEATH'),

(-1533065,'You seek death?',14571,1,0,0,'rivendare_naxx SAY_RIVE_AGGRO1'),
(-1533066,'None shall pass!',14572,1,0,0,'rivendare_naxx SAY_RIVE_AGGRO2'),
(-1533067,'Be still!',14573,1,0,0,'rivendare_naxx SAY_RIVE_AGGRO3'),
(-1533068,'You will find no peace in death.',14574,1,0,0,'rivendare_naxx SAY_RIVE_SLAY1'),
(-1533069,'The master\'s will is done.',14575,1,0,0,'rivendare_naxx SAY_RIVE_SLAY2'),
(-1533070,'Bow to the might of the scourge!',14576,1,0,0,'rivendare_naxx SAY_RIVE_SPECIAL'),
(-1533071,'Enough prattling. Let them come! We shall grind their bones to dust.',14577,6,0,0,'rivendare_naxx SAY_RIVE_TAUNT1'),
(-1533072,'Conserve your anger! Harness your rage! You will all have outlets for your frustration soon enough.',14578,6,0,0,'rivendare_naxx SAY_RIVE_TAUNT2'),
(-1533073,'Life is meaningless. It is in death that we are truly tested.',14579,6,0,0,'rivendare_naxx SAY_RIVE_TAUNT3'),
(-1533074,'Death... will not stop me...',14580,1,0,0,'rivendare_naxx SAY_RIVE_DEATH'),

(-1533075,'Glory to the master!',8845,1,0,0,'noth SAY_AGGRO1'),
(-1533076,'Your life is forfeit!',8846,1,0,0,'noth SAY_AGGRO2'),
(-1533077,'Die, trespasser!',8847,1,0,0,'noth SAY_AGGRO3'),
(-1533078,'Rise, my soldiers! Rise and fight once more!',8851,1,0,0,'noth SAY_SUMMON'),
(-1533079,'My task is done!',8849,1,0,0,'noth SAY_SLAY1'),
(-1533080,'Breathe no more!',8850,1,0,0,'noth SAY_SLAY2'),
(-1533081,'I will serve the master... in... death!',8848,1,0,0,'noth SAY_DEATH'),

(-1533082,'%s takes in a deep breath...',0,3,0,0,'sapphiron EMOTE_BREATH'),
(-1533083,'%s resumes his attacks!',0,3,0,0,'sapphiron EMOTE_GROUND'),

(-1533084,'Your forces are nearly marshalled to strike back against your enemies, my liege.',14467,6,0,0,'kelthuzad SAY_SAPP_DIALOG1'),
(-1533085,'Soon we will eradicate the Alliance and Horde, then the rest of Azeroth will fall before the might of my army.',14768,6,0,0,'lich_king SAY_SAPP_DIALOG2_LICH'),
(-1533086,'Yes, Master. The time of their ultimate demise grows close...What is this?',14468,6,0,0,'kelthuzad SAY_SAPP_DIALOG3'),
(-1533087,'Invaders...here?! DESTROY them, Kel\'Thuzad! Naxxramas must not fall!',14769,6,0,0,'lich_king SAY_SAPP_DIALOG4_LICH'),
(-1533088,'As you command, Master!',14469,6,0,0,'kelthuzad SAY_SAPP_DIALOG5'),
(-1533089,'No!!! A curse upon you, interlopers! The armies of the Lich King will hunt you down. You will not escape your fate...',14484,6,0,0,'kelthuzad SAY_CAT_DIED'),
(-1533090,'Who dares violate the sanctity of my domain? Be warned, all who trespass here are doomed.',14463,6,0,0,'kelthuzad SAY_TAUNT1'),
(-1533091,'Fools, you think yourselves triumphant? You have only taken one step closer to the abyss! ',14464,6,0,0,'kelthuzad SAY_TAUNT2'),
(-1533092,'I grow tired of these games. Proceed, and I will banish your souls to oblivion!',14465,6,0,0,'kelthuzad SAY_TAUNT3'),
(-1533093,'You have no idea what horrors lie ahead. You have seen nothing! The frozen heart of Naxxramas awaits you!',14466,6,0,0,'kelthuzad SAY_TAUNT4'),
(-1533094,'Pray for mercy!',14475,1,0,0,'kelthuzad SAY_AGGRO1'),
(-1533095,'Scream your dying breath!',14476,1,0,0,'kelthuzad SAY_AGGRO2'),
(-1533096,'The end is upon you!',14477,1,0,0,'kelthuzad SAY_AGGRO3'),
(-1533097,'The dark void awaits you!',14478,1,0,0,'kelthuzad SAY_SLAY1'),
(-1533098,'<Kel\'Thuzad cackles maniacally!>',14479,1,0,0,'kelthuzad SAY_SLAY2'),
(-1533099,'AAAAGHHH!... Do not rejoice... your victory is a hollow one... for I shall return with powers beyond your imagining!',14480,1,0,0,'kelthuzad SAY_DEATH'),
(-1533100,'Your soul, is bound to me now!',14472,1,0,0,'kelthuzad SAY_CHAIN1'),
(-1533101,'There will be no escape!',14473,1,0,0,'kelthuzad SAY_CHAIN2'),
(-1533102,'I will freeze the blood in your veins!',14474,1,0,0,'kelthuzad SAY_FROST_BLAST'),
(-1533103,'Master! I require aid! ',14470,1,0,0,'kelthuzad SAY_REQUEST_AID'),
(-1533104,'Very well... warriors of the frozen wastes, rise up! I command you to fight, kill, and die for your master. Let none survive...',0,1,0,0,'kelthuzad SAY_ANSWER_REQUEST'),
(-1533105,'Minions, servants, soldiers of the cold dark, obey the call of Kel\'Thuzad!',14471,1,0,0,'kelthuzad SAY_SUMMON_MINIONS'),
(-1533106,'Your petty magics are no challenge to the might of the Scourge! ',14481,1,0,0,'kelthuzad SAY_SPECIAL1_MANA_DET'),
(-1533107,'Enough! I grow tired of these distractions! ',14483,1,0,0,'kelthuzad SAY_SPECIAL3_MANA_DET'),
(-1533108,'Fools, you have spread your powers too thin. Be free, my minions!',14482,1,0,0,'kelthuzad SAY_SPECIAL2_DISPELL'),

(-1533109,'You are mine now!',8825,1,0,0,'heigan SAY_AGGRO1'),
(-1533110,'I see you!',8826,1,0,0,'heigan SAY_AGGRO2'),
(-1533111,'You...are next!',8827,1,0,0,'heigan SAY_AGGRO3'),
(-1533112,'Close your eyes... sleep!',8829,1,0,0,'heigan SAY_SLAY'),
(-1533113,'The races of the world will perish. It is only a matter of time.',8830,1,0,0,'heigan SAY_TAUNT1'),
(-1533114,'I see endless suffering, I see torment, I see rage. I see... everything!',8831,1,0,0,'heigan SAY_TAUNT2'),
(-1533115,'Soon... the world will tremble!',8832,1,0,0,'heigan SAY_TAUNT3'),
(-1533116,'The end is upon you.',8833,1,0,0,'heigan SAY_CHANNELING'),
(-1533117,'Hungry worms will feast on your rotten flesh!',8834,1,0,0,'heigan SAY_TAUNT4'),
(-1533118,'Noo... o...',8828,1,0,0,'heigan SAY_DEATH'),

(-1533119,'%s spots a nearby Zombie to devour!',0,3,0,0,'gluth EMOTE_ZOMBIE'),

(-1533120,'Hah hah, I\'m just getting warmed up!',8852,1,0,0,'razuvious SAY_AGGRO1'),
(-1533121,'Stand and fight!',8853,1,0,0,'razuvious SAY_AGGRO2'),
(-1533122,'Show me what you\'ve got!',8854,1,0,0,'razuvious SAY_AGGRO3'),
(-1533123,'Sweep the leg! Do you have a problem with that?',8861,1,0,0,'razuvious SAY_SLAY1'),
(-1533124,'You should have stayed home!',8862,1,0,0,'razuvious SAY_SLAY2'),
(-1533125,'Do as I taught you!',8855,1,0,0,'razuvious SAY_COMMAND1'),
(-1533126,'Show them no mercy!',8856,1,0,0,'razuvious SAY_COMMAND2'),
(-1533127,'You disappoint me, students!',8858,1,0,0,'razuvious SAY_COMMAND3'),
(-1533128,'The time for practice is over! Show me what you\'ve learned!',8859,1,0,0,'razuvious SAY_COMMAND4'),
(-1533129,'An honorable... death...',8860,1,0,0,'razuvious SAY_DEATH'),

(-1533130,'%s summons forth Skeletal Warriors!',0,3,0,0,'noth EMOTE_WARRIOR'),
(-1533131,'%s raises more skeletons!',0,3,0,0,'noth EMOTE_SKELETON'),
(-1533132,'%s teleports to the balcony above!',0,3,0,0,'noth EMOTE_TELEPORT'),
(-1533133,'%s teleports back into the battle!',0,3,0,0,'noth EMOTE_TELEPORT_RETURN'),

(-1533134,'A Guardian of Icecrown enters the fight!',0,3,0,0,'kelthuzad EMOTE_GUARDIAN'),
(-1533135,'%s strikes!',0,3,0,0,'kelthuzad EMOTE_PHASE2'),

(-1533136,'%s teleports and begins to channel a spell!',0,3,0,0,'heigan EMOTE_TELEPORT'),
(-1533137,'%s rushes to attack once more!',0,3,0,0,'heigan EMOTE_RETURN'),

(-1533138,'%s teleports into the fray!',0,3,0,0,'gothik EMOTE_TO_FRAY'),
(-1533139,'The central gate opens!',0,3,0,0,'gothik EMOTE_GATE'),
(-1533140,'Brazenly you have disregarded powers beyond your understanding.',0,1,0,0,'gothik SAY_SPEECH_2'),
(-1533141,'You have fought hard to invade the realm of the harvester.',0,1,0,0,'gothik SAY_SPEECH_3'),
(-1533142,'Now there is only one way out - to walk the lonely path of the damned.',0,1,0,0,'gothik SAY_SPEECH_4'),

(-1533143,'An aura of necrotic energy blocks all healing!',0,3,0,0,'Loatheb EMOTE_AURA_BLOCKING'),
(-1533144,'The power of Necrotic Aura begins to wane!',0,3,0,0,'Loatheb EMOTE_AURA_WANE'),
(-1533145,'The aura fades away, allowing healing once more!',0,3,0,0,'Loatheb EMOTE_AURA_FADING'),

(-1533146,'%s spins her web into a cocoon!',0,3,0,0,'maexxna EMOTE_SPIN_WEB'),
(-1533147,'Spiderlings appear on the web!',0,3,0,0,'maexxna EMOTE_SPIDERLING'),
(-1533148,'%s sprays strands of web everywhere!',0,3,0,0,'maexxna EMOTE_SPRAY'),

(-1533149,'%s loses its link!',0,3,0,0,'tesla_coil EMOTE_LOSING_LINK'),
(-1533150,'%s overloads!',0,3,0,0,'tesla_coil EMOTE_TESLA_OVERLOAD'),
(-1533151,'The polarity has shifted!',0,3,0,0,'thaddius EMOTE_POLARITY_SHIFT'),

(-1533152,'%s decimates all nearby flesh!',0,3,0,0,'gluth EMOTE_DECIMATE'),

(-1533153,'A %s joins the fight!',0,3,0,0,'crypt_guard EMOTE_CRYPT_GUARD'),
(-1533154,'%s begins to unleash an insect swarm!',0,3,0,0,'anubrekhan EMOTE_INSECT_SWARM'),
(-1533155,'Corpse Scarabs appear from a Crypt Guard\'s corpse!',0,3,0,0,'anubrekhan EMOTE_CORPSE_SCARABS'),

(-1533156,'%s casts Unyielding Pain on everyone!',0,3,0,0,'lady_blaumeux EMOTE_UNYIELDING_PAIN'),
(-1533157,'%s casts Condemation on everyone!',0,3,0,0,'sir_zeliek EMOTE_CONDEMATION'),

(-1533158,'%s injects you with a mutagen!',0,5,0,0,'grobbulus EMOTE_INJECTION');

-- -1 534 000 THE BATTLE OF MT. HYJAL
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1534000,'I\'m in jeopardy, help me if you can!',11007,1,0,0,'jaina hyjal ATTACKED 1'),
(-1534001,'They\'ve broken through!',11049,1,0,0,'jaina hyjal ATTACKED 2'),
(-1534002,'Stay alert! Another wave approaches.',11008,1,0,0,'jaina hyjal INCOMING'),
(-1534003,'Don\'t give up! We must prevail!',11006,1,0,0,'jaina hyjal BEGIN'),
(-1534004,'Hold them back as long as possible.',11050,1,0,0,'jaina hyjal RALLY 1'),
(-1534005,'We must hold strong!',11051,1,0,0,'jaina hyjal RALLY 2'),
(-1534006,'We are lost. Fall back!',11009,1,0,0,'jaina hyjal FAILURE'),
(-1534007,'We have won valuable time. Now we must pull back!',11011,1,0,0,'jaina hyjal SUCCESS'),
(-1534008,'I did... my best.',11010,1,0,0,'jaina hyjal DEATH'),

(-1534009,'I will lie down for no one!',11031,1,0,0,'thrall hyjal ATTACKED 1'),
(-1534010,'Bring the fight to me and pay with your lives!',11061,1,0,0,'thrall hyjal ATTACKED 2'),
(-1534011,'Make ready for another wave! LOK-TAR OGAR!',11032,1,0,0,'thrall hyjal INCOMING'),
(-1534012,'Hold them back! Do not falter!',11030,1,0,0,'thrall hyjal BEGIN'),
(-1534013,'Victory or death!',11059,1,0,0,'thrall hyjal RALLY 1'),
(-1534014,'Do not give an inch of ground!',11060,1,0,0,'thrall hyjal RALLY 2'),
(-1534015,'It is over. Withdraw! We have failed.',11033,1,0,0,'thrall hyjal FAILURE'),
(-1534016,'We have played our part and done well. It is up to the others now.',11035,1,0,0,'thrall hyjal SUCCESS'),
(-1534017,'Uraaa...',11034,1,0,0,'thrall hyjal DEATH'),

(-1534018,'All of your efforts have been in vain, for the draining of the World Tree has already begun. Soon the heart of your world will beat no more.',10986,6,0,0,'archimonde SAY_PRE_EVENTS_COMPLETE'),
(-1534019,'Your resistance is insignificant.',10987,1,0,0,'archimonde SAY_AGGRO'),
(-1534020,'This world will burn!',10990,1,0,0,'archimonde SAY_DOOMFIRE1'),
(-1534021,'Manach sheek-thrish!',11041,1,0,0,'archimonde SAY_DOOMFIRE2'),
(-1534022,'A-kreesh!',10989,1,0,0,'archimonde SAY_AIR_BURST1'),
(-1534023,'Away vermin!',11043,1,0,0,'archimonde SAY_AIR_BURST2'),
(-1534024,'All creation will be devoured!',11044,1,0,0,'archimonde SAY_SLAY1'),
(-1534025,'Your soul will languish for eternity.',10991,1,0,0,'archimonde SAY_SLAY2'),
(-1534026,'I am the coming of the end!',11045,1,0,0,'archimonde SAY_SLAY3'),
(-1534027,'At last it is here. Mourn and lament the passing of all you have ever known and all that would have been! Akmin-kurai!',10993,1,0,0,'archimonde SAY_ENRAGE'),
(-1534028,'No, it cannot be! Nooo!',10992,1,0,0,'archimonde SAY_DEATH'),
(-1534029,'You are mine now.',10988,1,0,0,'archimonde SAY_SOUL_CHARGE1'),
(-1534030,'Bow to my will.',11042,1,0,0,'archimonde SAY_SOUL_CHARGE2');

-- -1 540 000 SHATTERED HALLS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1540000,'You wish to fight us all at once? This should be amusing!',10262,1,0,0,'nethekurse SAY_INTRO'),
(-1540001,'You can have that one. I no longer need him.',10263,1,0,0,'nethekurse PEON_ATTACK_1'),
(-1540002,'Yes, beat him mercilessly. His skull is a thick as an ogres.',10264,1,0,0,'nethekurse PEON_ATTACK_2'),
(-1540003,'Don\'t waste your time on that one. He\'s weak!',10265,1,0,0,'nethekurse PEON_ATTACK_3'),
(-1540004,'You want him? Very well, take him!',10266,1,0,0,'nethekurse PEON_ATTACK_4'),
(-1540005,'One pitiful wretch down. Go on, take another one.',10267,1,0,0,'nethekurse PEON_DIE_1'),
(-1540006,'Ahh, what a waste... Next!',10268,1,0,0,'nethekurse PEON_DIE_2'),
(-1540007,'I was going to kill him anyway!',10269,1,0,0,'nethekurse PEON_DIE_3'),
(-1540008,'Thank you for saving me the trouble! Now it\'s my turn to have some fun...',10270,1,0,0,'nethekurse PEON_DIE_4'),
(-1540009,'Beg for your pittyfull life!',10259,1,0,0,'nethekurse SAY_TAUNT_1'),
(-1540010,'Run covad, ruun!',10260,1,0,0,'nethekurse SAY_TAUNT_2'),
(-1540011,'Your pain amuses me.',10261,1,0,0,'nethekurse SAY_TAUNT_3'),
(-1540012,'I\'m already bored.',10271,1,0,0,'nethekurse SAY_AGGRO_1'),
(-1540013,'Come on! ... Show me a real fight.',10272,1,0,0,'nethekurse SAY_AGGRO_2'),
(-1540014,'I had more fun torturing the peons.',10273,1,0,0,'nethekurse SAY_AGGRO_3'),
(-1540015,'You Loose.',10274,1,0,0,'nethekurse SAY_SLAY_1'),
(-1540016,'Ohh! Just die.',10275,1,0,0,'nethekurse SAY_SLAY_2'),
(-1540017,'What a ... a shame.',10276,1,0,0,'nethekurse SAY_DIE'),

(-1540018,'Smash!',10306,1,0,0,'omrogg GoCombat_1'),
(-1540019,'If you nice me let you live.',10308,1,0,0,'omrogg GoCombat_2'),
(-1540020,'Me hungry!',10309,1,0,0,'omrogg GoCombat_3'),
(-1540021,'Why don\'t you let me do the talking?',10317,1,0,0,'omrogg GoCombatDelay_1'),
(-1540022,'No, we will NOT let you live!',10318,1,0,0,'omrogg GoCombatDelay_2'),
(-1540023,'You always hungry. That why we so fat!',10319,1,0,0,'omrogg GoCombatDelay_3'),
(-1540024,'You stay here. Me go kill someone else!',10303,1,0,0,'omrogg Threat_1'),
(-1540025,'What are you doing!',10315,1,0,0,'omrogg Threat_2'),
(-1540026,'Me kill someone else...',10302,1,0,0,'omrogg Threat_3'),
(-1540027,'Me not like this one...',10300,1,0,0,'omrogg Threat_4'),
(-1540028,'That\'s not funny!',10314,1,0,0,'omrogg ThreatDelay1_1'),
(-1540029,'Me get bored...',10305,1,0,0,'omrogg ThreatDelay1_2'),
(-1540030,'I\'m not done yet, idiot!',10313,1,0,0,'omrogg ThreatDelay1_3'),
(-1540031,'Hey you numbskull!',10312,1,0,0,'omrogg ThreatDelay1_4'),
(-1540032,'Ha ha ha.',10304,1,0,0,'omrogg ThreatDelay2_1'),
(-1540033,'Whhy! He almost dead!',10316,1,0,0,'omrogg ThreatDelay2_2'),
(-1540034,'H\'ey...',10307,1,0,0,'omrogg ThreatDelay2_3'),
(-1540035,'We kill his friend!',10301,1,0,0,'omrogg ThreatDelay2_4'),
(-1540036,'This one die easy!',10310,1,0,0,'omrogg Killing_1'),
(-1540037,'I\'m tired. You kill next one!',10320,1,0,0,'omrogg Killing_2'),
(-1540038,'That\'s because I do all the hard work!',10321,1,0,0,'omrogg KillingDelay_1'),
(-1540039,'This all...your fault!',10311,1,0,0,'omrogg YELL_DIE_L'),
(-1540040,'I...hate...you...',10322,1,0,0,'omrogg YELL_DIE_R'),
(-1540041,'%s enrages!',0,2,0,0,'omrogg EMOTE_ENRAGE'),

(-1540042,'Ours is the true Horde! The only Horde!',10323,1,0,0,'kargath SAY_AGGRO1'),
(-1540043,'I\'ll carve the meat from your bones!',10324,1,0,0,'kargath SAY_AGGRO2'),
(-1540044,'I am called Bladefist for a reason, as you will see!',10325,1,0,0,'kargath SAY_AGGRO3'),
(-1540045,'For the real Horde!',10326,1,0,0,'kargath SAY_SLAY1'),
(-1540046,'I am the only Warchief!',10327,1,0,0,'kargath SAY_SLAY2'),
(-1540047,'The true Horde... will.. prevail...',10328,1,0,0,'kargath SAY_DEATH'),
(-1540048,'Cowards! You\'ll never pull me into the shadows!',0,1,0,0,'kargath SAY_EVADE'),

(-1540049,'The Alliance dares to intrude this far into my fortress? Bring out the Honor Hold prisoners and call for the executioner! They\'ll pay with their lives for this trespass!',0,6,0,0,'kargath SAY_EXECUTE_ALLY'),
(-1540050,'It looks like we have a ranking officer among our captives...how amusing. Execute the green-skinned dog at once!',0,6,0,0,'kargath SAY_EXECUTE_HORDE');

-- -1 542 000 BLOOD FURNACE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1542000,'Who dares interrupt... What is this? What have you done? You ruin everything!',10164,1,0,0,'kelidan SAY_WAKE'),
(-1542001,'You mustn\'t let him loose!',10166,1,0,0,'kelidan SAY_ADD_AGGRO_1'),
(-1542002,'Ignorant whelps!',10167,1,0,0,'kelidan SAY_ADD_AGGRO_2'),
(-1542003,'You fools! He\'ll kill us all!',10168,1,0,0,'kelidan SAY_ADD_AGGRO_3'),
(-1542004,'Just as you deserve!',10169,1,0,0,'kelidan SAY_KILL_1'),
(-1542005,'Your friends will soon be joining you.',10170,1,0,0,'kelidan SAY_KILL_2'),
(-1542006,'Closer... Come closer.. and burn!',10165,1,0,0,'kelidan SAY_NOVA'),
(-1542007,'Good luck... you\'ll need it..',10171,1,0,0,'kelidan SAY_DIE'),

(-1542008,'Come intruders....',0,1,0,0,'broggok SAY_AGGRO'),

(-1542009,'My work must not be interrupted.',10286,1,0,0,'the_maker SAY_AGGRO_1'),
(-1542010,'Perhaps I can find a use for you.',10287,1,0,0,'the_maker SAY_AGGRO_2'),
(-1542011,'Anger... Hate... These are tools I can use.',10288,1,0,0,'the_maker SAY_AGGRO_3'),
(-1542012,'Let\'s see what I can make of you.',10289,1,0,0,'the_maker SAY_KILL_1'),
(-1542013,'It is pointless to resist.',10290,1,0,0,'the_maker SAY_KILL_2'),
(-1542014,'Stay away from... me.',10291,1,0,0,'the_maker SAY_DIE'),

(-1542015,'Kill them!',0,1,0,0,'broggok SAY_BROGGOK_INTRO'),

(-1542016,'How long do you beleive your pathetic sorcery can hold me?',0,6,0,0,'magtheridon SAY_MAGTHERIDON_WARN');

-- -1 543 000 HELLFIRE RAMPARTS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1543000,'Do you smell that? Fresh meat has somehow breached our citadel. Be wary of any intruders.',0,1,0,0,'gargolmar SAY_TAUNT'),
(-1543001,'Heal me! QUICKLY!',10329,1,0,0,'gargolmar SAY_HEAL'),
(-1543002,'Back off, pup!',10330,1,0,0,'gargolmar SAY_SURGE'),
(-1543003,'What have we here...?',10331,1,0,0,'gargolmar SAY_AGGRO_1'),
(-1543004,'Heh... this may hurt a little.',10332,1,0,0,'gargolmar SAY_AGGRO_2'),
(-1543005,'I\'m gonna enjoy this.',10333,1,0,0,'gargolmar SAY_AGGRO_3'),
(-1543006,'Say farewell!',10334,1,0,0,'gargolmar SAY_KILL_1'),
(-1543007,'Much too easy...',10335,1,0,0,'gargolmar SAY_KILL_2'),
(-1543008,'Hahah.. <cough> ..argh!',10336,1,0,0,'gargolmar SAY_DIE'),

(-1543009,'You dare stand against me?!',10280,1,0,0,'omor SAY_AGGRO_1'),
(-1543010,'I will not be defeated!',10279,1,0,0,'omor SAY_AGGRO_2'),
(-1543011,'Your insolence will be your death.',10281,1,0,0,'omor SAY_AGGRO_3'),
(-1543012,'Achor-she-ki! Feast my pet! Eat your fill!',10277,1,0,0,'omor SAY_SUMMON'),
(-1543013,'A-Kreesh!',10278,1,0,0,'omor SAY_CURSE'),
(-1543014,'Die, weakling!',10282,1,0,0,'omor SAY_KILL_1'),
(-1543015,'It is... not over.',10284,1,0,0,'omor SAY_DIE'),
(-1543016,'I am victorious!',10283,1,0,0,'omor SAY_WIPE'),

(-1543017,'You have faced many challenges, pity they were all in vain. Soon your people will kneel to my lord!',10292,1,0,0,'vazruden SAY_INTRO'),
(-1543018,'Your time is running out!',10294,1,0,0,'vazruden SAY_AGGRO1'),
(-1543019,'You are nothing, I answer a higher call!',10295,1,0,0,'vazruden SAY_AGGRO2'),
(-1543020,'The Dark Lord laughs at you!',10296,1,0,0,'vazruden SAY_AGGRO3'),
(-1543021,'Is there no one left to test me?',10293,1,0,0,'vazruden SAY_TAUNT'),
(-1543022,'It is over. Finished!',10297,1,0,0,'vazruden SAY_KILL1'),
(-1543023,'Your days are done!',10298,1,0,0,'vazruden SAY_KILL2'),
(-1543024,'My lord will be the end you all...',10299,1,0,0,'vazruden SAY_DEATH'),
(-1543025,'%s descends from the sky.',0,3,0,0,'vazruden EMOTE_DESCEND');

-- -1 544 000 MAGTHERIDON'S LAIR
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1544000,'Wretched, meddling insects. Release me and perhaps i will grant you a merciful death!',10247,6,0,0,'magtheridon SAY_TAUNT1'),
(-1544001,'Vermin! Leeches! Take my blood and choke on it!',10248,6,0,0,'magtheridon SAY_TAUNT2'),
(-1544002,'Illidan is an arrogant fool. I will crush him and reclaim Outland as my own.',10249,6,0,0,'magtheridon SAY_TAUNT3'),
(-1544003,'Away, you mindless parasites. My blood is my own!',10250,6,0,0,'magtheridon SAY_TAUNT4'),
(-1544004,'How long do you believe your pathetic sorcery can hold me?',10251,6,0,0,'magtheridon SAY_TAUNT5'),
(-1544005,'My blood will be the end of you!',10252,6,0,0,'magtheridon SAY_TAUNT6'),
(-1544006,'I...am...UNLEASHED!!!',10253,1,0,0,'magtheridon SAY_FREED'),
(-1544007,'Thank you for releasing me. Now...die!',10254,1,0,0,'magtheridon SAY_AGGRO'),
(-1544008,'Not again...NOT AGAIN!',10256,1,0,0,'magtheridon SAY_BANISH'),
(-1544009,'I will not be taken so easily. Let the walls of this prison tremble...and FALL!!!',10257,1,0,0,'magtheridon SAY_CHAMBER_DESTROY'),
(-1544010,'Did you think me weak? Soft? Who is the weak one now?!',10255,1,0,0,'magtheridon SAY_PLAYER_KILLED'),
(-1544011,'The Legion...will consume you...all...',10258,1,0,0,'magtheridon SAY_DEATH'),
(-1544012,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1544013,'%s begins to cast Blast Nova!',0,3,0,0,'magtheridon EMOTE_BLASTNOVA'),
(-1544014,'%s\'s bonds begin to weaken!',0,2,0,0,'magtheridon EMOTE_BEGIN'),
(-1544015,'%s breaks free!',0,2,0,0,'magtheridon EMOTE_FREED'),
(-1544016,'%s is nearly free of his bonds!',0,2,0,0,'magtheridon EMOTE_NEARLY_FREE');

-- -1 545 000 THE STEAMVAULT
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1545000,'Surge forth my pets!',10360,1,0,0,'thespia SAY_SUMMON'),
(-1545001,'The depths will consume you!',10361,1,0,0,'thespia SAY_AGGRO_1'),
(-1545002,'Meet your doom, surface dwellers!',10362,1,0,0,'thespia SAY_AGGRO_2'),
(-1545003,'You will drown in blood!',10363,1,0,0,'thespia SAY_AGGRO_3'),
(-1545004,'To the depths of oblivion with you!',10364,1,0,0,'thespia SAY_SLAY_1'),
(-1545005,'For my lady and master!',10365,1,0,0,'thespia SAY_SLAY_2'),
(-1545006,'Our matron will be.. the end of.. you..',10366,1,0,0,'thespia SAY_DEAD'),

(-1545007,'I\'m bringin\' the pain!',10367,1,0,0,'mekgineer SAY_MECHANICS'),
(-1545008,'You\'re in for a world of hurt!',10368,1,0,0,'mekgineer SAY_AGGRO_1'),
(-1545009,'Eat hot metal, scumbag!',10369,1,0,0,'mekgineer SAY_AGGRO_2'),
(-1545010,'I\'ll come over there!',10370,1,0,0,'mekgineer SAY_AGGRO_3'),
(-1545011,'I\'m bringin\' the pain!',10371,1,0,0,'mekgineer SAY_AGGRO_4'),
(-1545012,'You just got served, punk!',10372,1,0,0,'mekgineer SOUND_SLAY_1'),
(-1545013,'I own you!',10373,1,0,0,'mekgineer SOUND_SLAY_2'),
(-1545014,'Have fun dyin\', cupcake!',10374,1,0,0,'mekgineer SOUND_SLAY_3'),
(-1545015,'Mommy!',10375,1,0,0,'mekgineer SAY_DEATH'),

(-1545016,'You deem yourselves worthy simply because you bested my guards? Our work here will not be compromised!',10390,1,0,0,'kalithresh SAY_INTRO'),
(-1545017,'This is not nearly over...',10391,1,0,0,'kalithresh SAY_REGEN'),
(-1545018,'Your head will roll!',10392,1,0,0,'kalithresh SAY_AGGRO1'),
(-1545019,'I despise all of your kind!',10393,1,0,0,'kalithresh SAY_AGGRO2'),
(-1545020,'Ba\'ahntha sol\'dorei!',10394,1,0,0,'kalithresh SAY_AGGRO3'),
(-1545021,'Scram, surface filth!',10395,1,0,0,'kalithresh SAY_SLAY1'),
(-1545022,'Ah ha ha ha ha ha ha!',10396,1,0,0,'kalithresh SAY_SLAY2'),
(-1545023,'For her Excellency... for... Vashj!',10397,1,0,0,'kalithresh SAY_DEATH'),

(-1545024,'Enjoy the storm, warm bloods!',0,1,0,0,'thespia SAY_CLOUD');

-- -1 546 000 THE UNDERBOG

-- -1 547 000 THE SLAVE PENS

-- -1 548 000 SERPENTSHRINE CAVERN
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1548000,'I cannot allow you to interfere!',11289,1,0,0,'hydross SAY_AGGRO'),
(-1548001,'Better, much better.',11290,1,0,0,'hydross SAY_SWITCH_TO_CLEAN'),
(-1548002,'They have forced me to this...',11291,1,0,0,'hydross SAY_CLEAN_SLAY1'),
(-1548003,'I have no choice.',11292,1,0,0,'hydross SAY_CLEAN_SLAY2'),
(-1548004,'I am... released...',11293,1,0,0,'hydross SAY_CLEAN_DEATH'),
(-1548005,'Aaghh, the poison...',11297,1,0,0,'hydross SAY_SWITCH_TO_CORRUPT'),
(-1548006,'I will purge you from this place.',11298,1,0,0,'hydross SAY_CORRUPT_SLAY1'),
(-1548007,'You are no better than they!',11299,1,0,0,'hydross SAY_CORRUPT_SLAY2'),
(-1548008,'You are the disease, not I',11300,1,0,0,'hydross SAY_CORRUPT_DEATH'),

(-1548009,'Finally, my banishment ends!',11312,1,0,0,'leotheras SAY_AGGRO'),
(-1548010,'Be gone, trifling elf.  I am in control now!',11304,1,0,0,'leotheras SAY_SWITCH_TO_DEMON'),
(-1548011,'We all have our demons...',11305,1,0,0,'leotheras SAY_INNER_DEMONS'),
(-1548012,'I have no equal.',11306,1,0,0,'leotheras SAY_DEMON_SLAY1'),
(-1548013,'Perish, mortal.',11307,1,0,0,'leotheras SAY_DEMON_SLAY2'),
(-1548014,'Yes, YES! Ahahah!',11308,1,0,0,'leotheras SAY_DEMON_SLAY3'),
(-1548015,'Kill! KILL!',11314,1,0,0,'leotheras SAY_NIGHTELF_SLAY1'),
(-1548016,'That\'s right! Yes!',11315,1,0,0,'leotheras SAY_NIGHTELF_SLAY2'),
(-1548017,'Who\'s the master now?',11316,1,0,0,'leotheras SAY_NIGHTELF_SLAY3'),
(-1548018,'No... no! What have you done? I am the master! Do you hear me? I am... aaggh! Can\'t... contain him...',11313,1,0,0,'leotheras SAY_FINAL_FORM'),
(-1548019,'At last I am liberated. It has been too long since I have tasted true freedom!',11309,1,0,0,'leotheras SAY_FREE'),
(-1548020,'You cannot kill me! Fools, I\'ll be back! I\'ll... aarghh...',11317,1,0,0,'leotheras SAY_DEATH'),

(-1548021,'Guards, attention! We have visitors...',11277,1,0,0,'karathress SAY_AGGRO'),
(-1548022,'Your overconfidence will be your undoing! Guards, lend me your strength!',11278,1,0,0,'karathress SAY_GAIN_BLESSING'),
(-1548023,'Go on, kill them! I\'ll be the better for it!',11279,1,0,0,'karathress SAY_GAIN_ABILITY1'),
(-1548024,'I am more powerful than ever!',11280,1,0,0,'karathress SAY_GAIN_ABILITY2'),
(-1548025,'More knowledge, more power!',11281,1,0,0,'karathress SAY_GAIN_ABILITY3'),
(-1548026,'Land-dwelling scum!',11282,1,0,0,'karathress SAY_SLAY1'),
(-1548027,'Alana be\'lendor!',11283,1,0,0,'karathress SAY_SLAY2'),
(-1548028,'I am rid of you.',11284,1,0,0,'karathress SAY_SLAY3'),
(-1548029,'Her ... excellency ... awaits!',11285,1,0,0,'karathress SAY_DEATH'),

(-1548030,'Flood of the deep, take you!',11321,1,0,0,'morogrim SAY_AGGRO'),
(-1548031,'By the Tides, kill them at once!',11322,1,0,0,'morogrim SAY_SUMMON1'),
(-1548032,'Destroy them my subjects!',11323,1,0,0,'morogrim SAY_SUMMON2'),
(-1548033,'There is nowhere to hide!',11324,1,0,0,'morogrim SAY_SUMMON_BUBL1'),
(-1548034,'Soon it will be finished!',11325,1,0,0,'morogrim SAY_SUMMON_BUBL2'),
(-1548035,'It is done!',11326,1,0,0,'morogrim SAY_SLAY1'),
(-1548036,'Strugging only makes it worse.',11327,1,0,0,'morogrim SAY_SLAY2'),
(-1548037,'Only the strong survive.',11328,1,0,0,'morogrim SAY_SLAY3'),
(-1548038,'Great... currents of... Ageon.',11329,1,0,0,'morogrim SAY_DEATH'),
(-1548039,'%s sends his enemies to their watery graves!',0,3,0,0,'morogrim EMOTE_WATERY_GRAVE'),
(-1548040,'The violent earthquake has alerted nearby murlocs!',0,3,0,0,'morogrim EMOTE_EARTHQUAKE'),
(-1548041,'%s summons Watery Globules!',0,3,0,0,'morogrim EMOTE_WATERY_GLOBULES'),

(-1548042,'Water is life. It has become a rare commodity here in Outland. A commodity that we alone shall control. We are the Highborne, and the time has come at last for us to retake our rightful place in the world!',11531,1,0,0,'vashj SAY_INTRO'),
(-1548043,'I\'ll split you from stem to stern!',11532,1,0,0,'vashj SAY_AGGRO1'),
(-1548044,'Victory to Lord Illidan!',11533,1,0,0,'vashj SAY_AGGRO2'),
(-1548045,'I spit on you, surface filth!',11534,1,0,0,'vashj SAY_AGGRO3'),
(-1548046,'Death to the outsiders!',11535,1,0,0,'vashj SAY_AGGRO4'),
(-1548047,'I did not wish to lower myself by engaging your kind, but you leave me little choice!',11538,1,0,0,'vashj SAY_PHASE1'),
(-1548048,'The time is now! Leave none standing!',11539,1,0,0,'vashj SAY_PHASE2'),
(-1548049,'You may want to take cover.',11540,1,0,0,'vashj SAY_PHASE3'),
(-1548050,'Straight to the heart!',11536,1,0,0,'vashj SAY_BOWSHOT1'),
(-1548051,'Seek your mark!',11537,1,0,0,'vashj SAY_BOWSHOT2'),
(-1548052,'Your time ends now!',11541,1,0,0,'vashj SAY_SLAY1'),
(-1548053,'You have failed!',11542,1,0,0,'vashj SAY_SLAY2'),
(-1548054,'Be\'lamere an\'delay',11543,1,0,0,'vashj SAY_SLAY3'),
(-1548055,'Lord Illidan, I... I am... sorry.',11544,1,0,0,'vashj SAY_DEATH'),

(-1548056,'%s takes a deep breath!',0,3,0,0,'lurker below EMOTE_DEEP_BREATH');

-- -1 550 000 THE EYE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1550000,'Alert, you are marked for extermination!',11213,1,0,0,'voidreaver SAY_AGGRO'),
(-1550001,'Extermination, successful.',11215,1,0,0,'voidreaver SAY_SLAY1'),
(-1550002,'Imbecile life form, no longer functional.',11216,1,0,0,'voidreaver SAY_SLAY2'),
(-1550003,'Threat neutralized.',11217,1,0,0,'voidreaver SAY_SLAY3'),
(-1550004,'Systems... shutting... down...',11214,1,0,0,'voidreaver SAY_DEATH'),
(-1550005,'Alternative measure commencing...',11218,1,0,0,'voidreaver SAY_POUNDING1'),
(-1550006,'Calculating force parameters...',11219,1,0,0,'voidreaver SAY_POUNDING2'),

(-1550007,'Tal anu\'men no Sin\'dorei!',11134,1,0,0,'solarian SAY_AGGRO'),
(-1550008,'Ha ha ha! You are hopelessly outmatched!',11139,1,0,0,'solarian SAY_SUMMON1'),
(-1550009,'I will crush your delusions of grandeur!',11140,1,0,0,'solarian SAY_SUMMON2'),
(-1550010,'Your soul belongs to the Abyss!',11136,1,0,0,'solarian SAY_KILL1'),
(-1550011,'By the blood of the Highborne!',11137,1,0,0,'solarian SAY_KILL2'),
(-1550012,'For the Sunwell!',11138,1,0,0,'solarian SAY_KILL3'),
(-1550013,'The warmth of the sun... awaits.',11135,1,0,0,'solarian SAY_DEATH'),
(-1550014,'Enough of this! Now I call upon the fury of the cosmos itself.',0,1,0,0,'solarian SAY_VOIDA'),
(-1550015,'I become ONE... with the VOID!',0,1,0,0,'solarian SAY_VOIDB'),

(-1550016,'Energy. Power. My people are addicted to it... a dependence made manifest after the Sunwell was destroyed. Welcome... to the future. A pity you are too late to stop it. No one can stop me now! Selama ashal\'anore!',11256,1,0,0,'kaelthas SAY_INTRO'),
(-1550017,'Capernian will see to it that your stay here is a short one.',11257,1,0,0,'kaelthas SAY_INTRO_CAPERNIAN'),
(-1550018,'Well done, you have proven worthy to test your skills against my master engineer, Telonicus.',11258,1,0,0,'kaelthas SAY_INTRO_TELONICUS'),
(-1550019,'Let us see how your nerves hold up against the Darkener, Thaladred.',11259,1,0,0,'kaelthas SAY_INTRO_THALADRED'),
(-1550020,'You have persevered against some of my best advisors... but none can withstand the might of the Blood Hammer. Behold, Lord Sanguinar!',11260,1,0,0,'kaelthas SAY_INTRO_SANGUINAR'),
(-1550021,'As you see, I have many weapons in my arsenal...',11261,1,0,0,'kaelthas SAY_PHASE2_WEAPON'),
(-1550022,'Perhaps I underestimated you. It would be unfair to make you fight all four advisors at once, but... fair treatment was never shown to my people. I\'m just returning the favor.',11262,1,0,0,'kaelthas SAY_PHASE3_ADVANCE'),
(-1550023,'Alas, sometimes one must take matters into one\'s own hands. Balamore shanal!',11263,1,0,0,'kaelthas SAY_PHASE4_INTRO2'),
(-1550024,'I have not come this far to be stopped! The future I have planned will not be jeopardized! Now you will taste true power!!',11273,1,0,0,'kaelthas SAY_PHASE5_NUTS'),
(-1550025,'You will not prevail.',11270,1,0,0,'kaelthas SAY_SLAY1'),
(-1550026,'You gambled...and lost.',11271,1,0,0,'kaelthas SAY_SLAY2'),
(-1550027,'This was Child\'s play.',11272,1,0,0,'kaelthas SAY_SLAY3'),
(-1550028,'Obey me.',11268,1,0,0,'kaelthas SAY_MINDCONTROL1'),
(-1550029,'Bow to my will.',11269,1,0,0,'kaelthas SAY_MINDCONTROL2'),
(-1550030,'Let us see how you fare when your world is turned upside down.',11264,1,0,0,'kaelthas SAY_GRAVITYLAPSE1'),
(-1550031,'Having trouble staying grounded?',11265,1,0,0,'kaelthas SAY_GRAVITYLAPSE2'),
(-1550032,'Anara\'nel belore!',11267,1,0,0,'kaelthas SAY_SUMMON_PHOENIX1'),
(-1550033,'By the power of the sun!',11266,1,0,0,'kaelthas SAY_SUMMON_PHOENIX2'),
(-1550034,'For...Quel...thalas!',11274,1,0,0,'kaelthas SAY_DEATH'),

(-1550035,'Prepare yourselves!',11203,1,0,0,'thaladred SAY_THALADRED_AGGRO'),
(-1550036,'Forgive me, my prince! I have... failed.',11204,1,0,0,'thaladred SAY_THALADRED_DEATH'),
(-1550037,'%s sets his gaze on $N!',0,2,0,0,'thaladred EMOTE_THALADRED_GAZE'),

(-1550038,'Blood for blood!',11152,1,0,0,'sanguinar SAY_SANGUINAR_AGGRO'),
(-1550039,'NO! I ...will... not...',11153,1,0,0,'sanguinar SAY_SANGUINAR_DEATH'),

(-1550040,'The sin\'dore reign supreme!',11117,1,0,0,'capernian SAY_CAPERNIAN_AGGRO'),
(-1550041,'This is not over!',11118,1,0,0,'capernian SAY_CAPERNIAN_DEATH'),

(-1550042,'Anar\'alah belore!',11157,1,0,0,'telonicus SAY_TELONICUS_AGGRO'),
(-1550043,'More perils... await',11158,1,0,0,'telonicus SAY_TELONICUS_DEATH'),

(-1550044,'%s begins to cast Pyroblast!',0,3,0,0,'kaelthas EMOTE_PYROBLAST');

-- -1 552 000 THE ARCATRAZ
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1552000,'It is a small matter to control the mind of the weak... for I bear allegiance to powers untouched by time, unmoved by fate. No force on this world or beyond harbors the strength to bend our knee... not even the mighty Legion!',11122,1,0,0,'skyriss SAY_INTRO'),
(-1552001,'Bear witness to the agent of your demise!',11123,1,0,0,'skyriss SAY_AGGRO'),
(-1552002,'Your fate is written!',11124,1,0,0,'skyriss SAY_KILL_1'),
(-1552003,'The chaos I have sown here is but a taste...',11125,1,0,0,'skyriss SAY_KILL_2'),
(-1552004,'You will do my bidding, weakling.',11127,1,0,0,'skyriss SAY_MIND_1'),
(-1552005,'Your will is no longer your own.',11128,1,0,0,'skyriss SAY_MIND_2'),
(-1552006,'Flee in terror!',11129,1,0,0,'skyriss SAY_FEAR_1'),
(-1552007,'I will show you horrors undreamed of!',11130,1,0,0,'skyriss SAY_FEAR_2'),
(-1552008,'We span the universe, as countless as the stars!',11131,1,0,0,'skyriss SAY_IMAGE'),
(-1552009,'I am merely one of... infinite multitudes.',11126,1,0,0,'skyriss SAY_DEATH'),

(-1552010,'Where in Bonzo\'s brass buttons am I? And who are-- yaaghh, that\'s one mother of a headache!',11171,1,0,0,'millhouse SAY_INTRO_1'),
(-1552011,'\"Lowly\"? I don\'t care who you are friend, no one refers to the mighty Millhouse Manastorm as \"Lowly\"! I have no idea what goes on here, but I will gladly join your fight against this impudent imbecile! Prepare to defend yourself, cretin!',11172,1,0,0,'millhouse SAY_INTRO_2'),
(-1552012,'I just need to get some things ready first. You guys go ahead and get started. I need to summon up some water...',11173,1,0,0,'millhouse SAY_WATER'),
(-1552013,'Fantastic! Next, some protective spells. Yes! Now we\'re cookin\'',11174,1,0,0,'millhouse SAY_BUFFS'),
(-1552014,'And of course i\'ll need some mana. You guys are gonna love this, just wait.',11175,1,0,0,'millhouse SAY_DRINK'),
(-1552015,'Aaalllriiiight!! Who ordered up an extra large can of whoop-ass?',11176,1,0,0,'millhouse SAY_READY'),
(-1552016,'I didn\'t even break a sweat on that one.',11177,1,0,0,'millhouse SAY_KILL_1'),
(-1552017,'You guys, feel free to jump in anytime.',11178,1,0,0,'millhouse SAY_KILL_2'),
(-1552018,'I\'m gonna light you up, sweet cheeks!',11179,1,0,0,'millhouse SAY_PYRO'),
(-1552019,'Ice, ice, baby!',11180,1,0,0,'millhouse SAY_ICEBLOCK'),
(-1552020,'Heal me! Oh, for the love of all that is holy, HEAL me! I\'m dying!',11181,1,0,0,'millhouse SAY_LOWHP'),
(-1552021,'You\'ll be hearing from my lawyer...',11182,1,0,0,'millhouse SAY_DEATH'),
(-1552022,'Who\'s bad? Who\'s bad? That\'s right: we bad!',11183,1,0,0,'millhouse SAY_COMPLETE'),

(-1552023,'I knew the prince would be angry but, I... I have not been myself. I had to let them out! The great one speaks to me, you see. Wait--outsiders. Kael\'thas did not send you! Good... I\'ll just tell the prince you released the prisoners!',11222,1,0,0,'mellichar YELL_INTRO1'),
(-1552024,'The naaru kept some of the most dangerous beings in existence here in these cells. Let me introduce you to another...',11223,1,0,0,'mellichar YELL_INTRO2'),
(-1552025,'Yes, yes... another! Your will is mine!',11224,1,0,0,'mellichar YELL_RELEASE1'),
(-1552026,'Behold another terrifying creature of incomprehensible power!',11225,1,0,0,'mellichar YELL_RELEASE2A'),
(-1552027,'What is this? A lowly gnome? I will do better, O\'great one.',11226,1,0,0,'mellichar YELL_RELEASE2B'),
(-1552028,'Anarchy! Bedlam! Oh, you are so wise! Yes, I see it now, of course!',11227,1,0,0,'mellichar YELL_RELEASE3'),
(-1552029,'One final cell remains. Yes, O\'great one, right away!',11228,1,0,0,'mellichar YELL_RELEASE4'),
(-1552030,'Welcome, O\'great one. I am your humble servant.',11229,1,0,0,'mellichar YELL_WELCOME'),

(-1552031,'It is unwise to anger me.',11086,1,0,0,'dalliah SAY_AGGRO'),
(-1552032,'Ahh... That is much better.',11091,1,0,0,'dalliah SAY_HEAL_1'),
(-1552033,'Ahh... Just what I needed.',11092,1,0,0,'dalliah SAY_HEAL_2'),
(-1552034,'Completely ineffective. Just like someone else I know.',11087,1,0,0,'dalliah SAY_KILL_1'),
(-1552035,'You chose the wrong opponent.',11088,1,0,0,'dalliah SAY_KILL_2'),
(-1552036,'I\'ll cut you to pieces!',11090,1,0,0,'dalliah SAY_WHIRLWIND_1'),
(-1552037,'Reap the Whirlwind!',11089,1,0,0,'dalliah SAY_WHIRLWIND_2'),
(-1552038,'Now I\'m really... angry...',11093,1,0,0,'dalliah SAY_DEATH'),

(-1552039,'Have you come to kill Dalliah? Can I watch?',11237,1,0,1,'soccothrates SAY_DALLIAH_AGGRO_1'),
(-1552040,'This may be the end for you, Dalliah. What a shame that would be.',11245,1,0,1,'soccothrates SAY_DALLIAH_TAUNT_1'),
(-1552041,'Facing difficulties, Dalliah? How nice.',11244,1,0,1,'soccothrates SAY_DALLIAH_TAUNT_2'),
(-1552042,'I suggest a new strategy, you draw the attackers while I gather reinforcements. Hahaha!',11246,1,0,1,'soccothrates SAY_DALLIAH_TAUNT_3'),
(-1552043,'Finally! Well done!',11247,1,0,66,'soccothrates SAY_DALLIAH_DEAD'),
(-1552044,'On guard!',11241,1,0,0,'soccothrates SAY_CHARGE_1'),
(-1552045,'Defend yourself, for all the good it will do...',11242,1,0,0,'soccothrates SAY_CHARGE_2'),
(-1552046,'Knew this was... the only way out',11243,1,0,0,'soccothrates SAY_DEATH'),
(-1552047,'Yes, that was quite satisfying',11239,1,0,0,'soccothrates SAY_KILL'),
(-1552048,'At last, a target for my frustrations!',11238,1,0,0,'soccothrates SAY_AGGRO'),

(-1552049,'Did you call on me?',11236,1,0,397,'soccothrates SAY_INTRO_1'),
(-1552050,'Why would I call on you?',0,1,0,396,'dalliah SAY_INTRO_2'),
(-1552051,'To do your heavy lifting, most likely.',0,1,0,396,'soccothrates SAY_INTRO_3'),
(-1552052,'When I need someone to prance around like an overstuffed peacock, I''ll call on you.',0,1,0,396,'dalliah SAY_INTRO_4'),
(-1552053,'Then I\'ll commit myself to ignoring you.',0,1,0,396,'soccothrates SAY_INTRO_5'),
(-1552054,'What would you know about commitment, sheet-sah?',0,1,0,396,'dalliah SAY_INTRO_6'),
(-1552055,'You\'re the one who should be-- Wait, we have company...',0,1,0,396,'soccothrates SAY_INTRO_7');

-- -1 553 000 THE BOTANICA
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1553000,'What are you doing? These specimens are very delicate!',11144,1,0,0,'freywinn SAY_AGGRO'),
(-1553001,'Your life cycle is now concluded!',11145,1,0,0,'freywinn SAY_KILL_1'),
(-1553002,'You will feed the worms.',11146,1,0,0,'freywinn SAY_KILL_2'),
(-1553003,'Endorel aluminor!',11147,1,0,0,'freywinn SAY_TREE_1'),
(-1553004,'Nature bends to my will!',11148,1,0,0,'freywinn SAY_TREE_2'),
(-1553005,'The specimens...must be preserved.',11149,1,0,0,'freywinn SAY_DEATH'),

(-1553006,'%s emits a strange noise.',0,2,0,0,'laj EMOTE_SUMMON'),

(-1553007,'Who disturbs this sanctuary?',11230,1,0,0,'warp SAY_AGGRO'),
(-1553008,'You must die! But wait: this does not--No, no... you must die!',11231,1,0,0,'warp SAY_SLAY_1'),
(-1553009,'What am I doing? Why do I...',11232,1,0,0,'warp SAY_SLAY_2'),
(-1553010,'Children, come to me!',11233,1,0,0,'warp SAY_SUMMON_1'),
(-1553011,'Maybe this is not--No, we fight! Come to my aid.',11234,1,0,0,'warp SAY_SUMMON_2'),
(-1553012,'So... confused. Do not... belong here!',11235,1,0,0,'warp SAY_DEATH');

-- -1 554 000 THE MECHANAR
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1554000,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1554001,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1554002,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1554003,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1554004,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1554005,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1554006,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1554007,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1554008,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1554009,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1554010,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1554011,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1554012,'REUSE_ME',0,0,0,0,'REUSE_ME'),

(-1554013,'Don\'t value your life very much, do you?',11186,1,0,0,'sepethrea SAY_AGGRO'),
(-1554014,'I am not alone.',11191,1,0,0,'sepethrea SAY_SUMMON'),
(-1554015,'Think you can take the heat?',11189,1,0,0,'sepethrea SAY_DRAGONS_BREATH_1'),
(-1554016,'Anar\'endal dracon!',11190,1,0,0,'sepethrea SAY_DRAGONS_BREATH_2'),
(-1554017,'And don\'t come back!',11187,1,0,0,'sepethrea SAY_SLAY1'),
(-1554018,'En\'dala finel el\'dal',11188,1,0,0,'sepethrea SAY_SLAY2'),
(-1554019,'Anu... bala belore...alon.',11192,1,0,0,'sepethrea SAY_DEATH'),

(-1554020,'We are on a strict timetable. You will not interfere!',11193,1,0,0,'pathaleon SAY_AGGRO'),
(-1554021,'I\'m looking for a team player...',11197,1,0,0,'pathaleon SAY_DOMINATION_1'),
(-1554022,'You work for me now!',11198,1,0,0,'pathaleon SAY_DOMINATION_2'),
(-1554023,'Time to supplement my work force.',11196,1,0,0,'pathaleon SAY_SUMMON'),
(-1554024,'I prefeer to be hands-on...',11199,1,0,0,'pathaleon SAY_ENRAGE'),
(-1554025,'A minor inconvenience.',11194,1,0,0,'pathaleon SAY_SLAY_1'),
(-1554026,'Looks like you lose.',11195,1,0,0,'pathaleon SAY_SLAY_2'),
(-1554027,'The project will... continue.',11200,1,0,0,'pathaleon SAY_DEATH'),
(-1554028,'I have been waiting for you!',0,1,0,53,'pathaleon SAY_INTRO');

-- -1 555 000 SHADOW LABYRINTH
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1555000,'Infidels have invaded the sanctuary! Sniveling pests...You have yet to learn the true meaning of agony!',10473,1,0,0,'hellmaw SAY_INTRO'),
(-1555001,'Pathetic mortals! You will pay dearly!',10475,1,0,0,'hellmaw SAY_AGGRO1'),
(-1555002,'I will break you!',10476,1,0,0,'hellmaw SAY_AGGRO2'),
(-1555003,'Finally! Something to relieve the tedium!',10477,1,0,0,'hellmaw SAY_AGGRO3'),
(-1555004,'Aid me, you fools, before it\'s too late!',10474,1,0,0,'hellmaw SAY_HELP'),
(-1555005,'Do you fear death?',10478,1,0,0,'hellmaw SAY_SLAY1'),
(-1555006,'This is the part I enjoy most.',10479,1,0,0,'hellmaw SAY_SLAY2'),
(-1555007,'Do not...grow...overconfident, mortal.',10480,1,0,0,'hellmaw SAY_DEATH'),

(-1555008,'All flesh must burn.',10482,1,0,0,'blackhearth SAY_INTRO1'),
(-1555009,'All creation must be unmade!',10483,1,0,0,'blackhearth SAY_INTRO2'),
(-1555010,'Power will be yours!',10484,1,0,0,'blackhearth SAY_INTRO3'),
(-1555011,'You\'ll be sorry!',10486,1,0,0,'blackhearth SAY_AGGRO1'),
(-1555012,'Time for fun!',10487,1,0,0,'blackhearth SAY_AGGRO2'),
(-1555013,'I see dead people!',10488,1,0,0,'blackhearth SAY_AGGRO3'),
(-1555014,'No comin\' back for you!',10489,1,0,0,'blackhearth SAY_SLAY1'),
(-1555015,'Nice try!',10490,1,0,0,'blackhearth SAY_SLAY2'),
(-1555016,'Help us, hurry!',10485,1,0,0,'blackhearth SAY_HELP'),
(-1555017,'This... no... good...',10491,1,0,0,'blackhearth SAY_DEATH'),

(-1555018,'Be ready for Dark One\'s return.',10492,1,0,0,'blackhearth SAY2_INTRO1'),
(-1555019,'So we have place in new universe.',10493,1,0,0,'blackhearth SAY2_INTRO2'),
(-1555020,'Dark one promise!',10494,1,0,0,'blackhearth SAY2_INTRO3'),
(-1555021,'You\'ll be sorry!',10496,1,0,0,'blackhearth SAY2_AGGRO1'),
(-1555022,'Time to kill!',10497,1,0,0,'blackhearth SAY2_AGGRO2'),
(-1555023,'You be dead people!',10498,1,0,0,'blackhearth SAY2_AGGRO3'),
(-1555024,'Now you gone for good.',10499,1,0,0,'blackhearth SAY2_SLAY1'),
(-1555025,'You failed, haha haha',10500,1,0,0,'blackhearth SAY2_SLAY2'),
(-1555026,'Help us, hurry!',10495,1,0,0,'blackhearth SAY2_HELP'),
(-1555027,'Arrgh, aah...ahhh',10501,1,0,0,'blackhearth SAY2_DEATH'),

(-1555028,'Keep your minds focused for the days of reckoning are close at hand. Soon, the destroyer of worlds will return to make good on his promise. Soon the destruction of all that is will begin!',10522,1,0,0,'vorpil SAY_INTRO'),
(-1555029,'I\'ll make an offering of your blood!',10524,1,0,0,'vorpil SAY_AGGRO1'),
(-1555030,'You\'ll be a fine example, for the others.',10525,1,0,0,'vorpil SAY_AGGRO2'),
(-1555031,'Good, a worthy sacrifice.',10526,1,0,0,'vorpil SAY_AGGRO3'),
(-1555032,'Come to my aid, heed your master now!',10523,1,0,0,'vorpil SAY_HELP'),
(-1555033,'I serve with pride.',10527,1,0,0,'vorpil SAY_SLAY1'),
(-1555034,'Your death is for the greater cause!',10528,1,0,0,'vorpil SAY_SLAY2'),
(-1555035,'I give my life... Gladly.',10529,1,0,0,'vorpil SAY_DEATH'),

(-1555036,'%s draws energy from the air.',0,2,0,0,'murmur EMOTE_SONIC_BOOM');

-- -1 556 000 SETHEKK HALLS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1556000,'I have pets..<squawk>..of my own!',10502,1,0,0,'syth SAY_SUMMON'),
(-1556001,'Hrrmm.. Time to.. hrrm.. make my move.',10503,1,0,0,'syth SAY_AGGRO_1'),
(-1556002,'Nice pets..hrm.. Yes! <squawking>',10504,1,0,0,'syth SAY_AGGRO_2'),
(-1556003,'Nice pets have.. weapons. No so..<squawk>..nice.',10505,1,0,0,'syth SAY_AGGRO_3'),
(-1556004,'Death.. meeting life is.. <squawking>',10506,1,0,0,'syth SAY_SLAY_1'),
(-1556005,'Uhn.. Be free..<squawk>',10507,1,0,0,'syth SAY_SLAY_2'),
(-1556006,'No more life..hrm. No more pain. <squawks weakly>',10508,1,0,0,'syth SAY_DEATH'),

(-1556007,'<squawk>..Trinkets yes pretty Trinkets..<squawk>..power, great power.<squawk>..power in Trinkets..<squawk>',10557,1,0,0,'ikiss SAY_INTRO'),
(-1556008,'You make war on Ikiss?..<squawk>',10554,1,0,0,'ikiss SAY_AGGRO_1'),
(-1556009,'Ikiss cut you pretty..<squawk>..slice you. Yes!',10555,1,0,0,'ikiss SAY_AGGRO_2'),
(-1556010,'No escape for..<squawk>..for you',10556,1,0,0,'ikiss SAY_AGGRO_3'),
(-1556011,'You die..<squawk>..stay away from Trinkets',10558,1,0,0,'ikiss SAY_SLAY_1'),
(-1556012,'<squawk>',10559,1,0,0,'ikiss SAY_SLAY_2'),
(-1556013,'Ikiss will not..<squawk>..die',10560,1,0,0,'ikiss SAY_DEATH'),
(-1556015,'%s begins to channel arcane energy...',0,3,0,0,'ikiss EMOTE_ARCANE_EXP'),

(-1556016,'No! How can this be?',0,1,0,0,'anzu SAY_INTRO_1'),
(-1556017,'Pain will be the price for your insolence! You cannot stop me from claiming the Emerald Dream as my own!',0,1,0,0,'anzu SAY_INTRO_2'),
(-1556018,'Awaken, my children and assist your master!',0,1,0,0,'anzu SAY_BANISH'),
(-1556019,'Your magics shall be your undoing... ak-a-ak...',0,5,0,0,'anzu SAY_WHISPER_MAGIC_1'),
(-1556020,'%s returns to stone.',0,2,0,0,'anzu EMOTE_BIRD_STONE'),
(-1556021,'Your powers... ak-ak... turn against you...',0,5,0,0,'anzu SAY_WHISPER_MAGIC_2'),
(-1556022,'Your spells... ke-kaw... are weak magics... easy to turn against you...',0,5,0,0,'anzu SAY_WHISPER_MAGIC_3');

-- -1 557 000 MANA TOMBS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1557000,'What is this? You must forgive me, but I was not expecting company. As you can see, we are somewhat preoccupied right now. But no matter. As I am a gracious host, I will tend to you... personally.',10539,1,0,0,'shaffar SAY_INTRO'),
(-1557001,'We have not yet been properly introduced.',10541,1,0,0,'shaffar SAY_AGGRO_1'),
(-1557002,'An epic battle. How exciting!',10542,1,0,0,'shaffar SAY_AGGRO_2'),
(-1557003,'I have longed for a good adventure.',10543,1,0,0,'shaffar SAY_AGGRO_3'),
(-1557004,'It has been... entertaining.',10544,1,0,0,'shaffar SAY_SLAY_1'),
(-1557005,'And now we part company.',10545,1,0,0,'shaffar SAY_SLAY_2'),
(-1557006,'I have such fascinating things to show you.',10540,1,0,0,'shaffar SAY_SUMMON'),
(-1557007,'I must bid you... farewell.',10546,1,0,0,'shaffar SAY_DEAD'),

(-1557008,'I will feed on your soul.',10561,1,0,0,'pandemonius SAY_AGGRO_1'),
(-1557009,'So... full of life!',10562,1,0,0,'pandemonius SAY_AGGRO_2'),
(-1557010,'Do not... resist.',10563,1,0,0,'pandemonius SAY_AGGRO_3'),
(-1557011,'Yes! I am... empowered!',10564,1,0,0,'pandemonius SAY_KILL_1'),
(-1557012,'More... I must have more!',10565,1,0,0,'pandemonius SAY_KILL_2'),
(-1557013,'To the void... once... more..',10566,1,0,0,'pandemonius SAY_DEATH'),
(-1557014,'%s shifts into the void...',0,3,0,0,'pandemonius EMOTE_DARK_SHELL'),

(-1557015,'This should\'t take very long. Just watch my back as I empty these nether collectors.',0,0,0,0,'sha\'heen SAY_ESCORT_START'),
(-1557016,'Fantastic! let\'s move on, shall we?',0,0,0,0,'sha\'heen SAY_START'),
(-1557017,'Looking at these energy levels, Shaffar was set to make a killing!',0,0,0,28,'sha\'heen SAY_FIRST_STOP'),
(-1557018,'That should do it...',0,0,0,0,'sha\'heen SAY_FIRST_STOP_COMPLETE'),
(-1557019,'Hmm, now where is the next collector?',0,0,0,0,'sha\'heen SAY_COLLECTOR_SEARCH'),
(-1557020,'Ah, there it is. Follow me, fleshling.',0,0,0,0,'sha\'heen SAY_COLLECTOR_FOUND'),
(-1557021,'There can\'t be too many more of these collectors. Just keep me safe as I do my job.',0,0,0,28,'sha\'heen SAY_SECOND_STOP'),
(-1557022,'What do we have here? I thought you said the area was secure? This is now the third attack? If we make it out of here, I will definitely be deducting this from your reward. Now don\'t just stand here, destroy them so I can get to that collector.',0,0,0,0,'sha\'heen SAY_THIRD_STOP'),
(-1557023,'We\'re close to the exit. I\'ll let you rest for about thirty seconds, but then we\'re out of here.',0,0,0,0,'sha\'heen SAY_REST'),
(-1557024,'Are you ready to go?',0,0,0,0,'sha\'heen SAY_READY_GO'),
(-1557025,'Ok break time is OVER. Let\'s go!',0,0,0,0,'sha\'heen SAY_BREAK_OVER'),
(-1557026,'Shadow Lord Xiraxis yells: Bravo! Bravo! Good show... I couldn\'t convince you to work for me, could I? No, I suppose the needless slaughter of my employees might negatively impact your employment application.',0,1,0,0,'xiraxis SAY_SPAWN'),
(-1557027,'Your plan was a good one, Sha\'heen, and you would have gotten away with it if not for one thing...',0,0,0,1,'xiraxis SAY_FINAL_STOP_1'),
(-1557028,'Oh really? And what might that be?',0,0,0,1,'sha\'heen SAY_FINAL_STOP_2'),
(-1557029,'Never underestimate the other ethereal\'s greed!',0,0,0,0,'xiraxis SAY_FINAL_STOP_3'),
(-1557030,'He was right, you know. I\'ll have to take that tag-line for my own... It\'s not like he\'ll have a use for it anymore!',0,0,0,1,'sha\'heen SAY_XIRAXIS_DEAD'),
(-1557031,'Thanks and good luck!',0,0,0,1,'sha\'heen SAY_COMPLETE'),
(-1557032,'%s checks to make sure his body is intact.',0,2,0,0,'sha\'heen EMOTE_TELEPORT'),
(-1557033,'You made it! Well done, $r. Now if you\'ll excuse me, I have to get the rest of our crew inside.',0,0,0,1,'sha\'heen SAY_SPAWN'),
(-1557034,'%s expertly manipulates the control panel.',0,2,0,28,'sha\'heen EMOTE_PANEL'),
(-1557035,'Let\'s not waste any time! Take anything that isn\'t nailed down to the floor and teleport directly to Stormspire! Chop chop!',0,0,0,1,'sha\'heen SAY_ESCORT_READY');

-- -1 558 000 AUCHENAI CRYPTS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1558000,'You have defiled the resting place of our ancestors. For this offense, there can be but one punishment. It is fitting that you have come to a place of the dead... for you will soon be joining them.',10509,1,0,0,'maladaar SAY_INTRO'),
(-1558001,'Rise my fallen brothers. Take form and fight!',10512,1,0,0,'maladaar SAY_SUMMON'),
(-1558002,'You will pay with your life!',10513,1,0,0,'maladaar SAY_AGGRO_1'),
(-1558003,'There\'s no turning back now!',10514,1,0,0,'maladaar SAY_AGGRO_2'),
(-1558004,'Serve your penitence!',10515,1,0,0,'maladaar SAY_AGGRO_3'),
(-1558005,'Let your mind be clouded.',10510,1,0,0,'maladaar SAY_ROAR'),
(-1558006,'Stare into the darkness of your soul.',10511,1,0,0,'maladaar SAY_SOUL_CLEAVE'),
(-1558007,'These walls will be your doom.',10516,1,0,0,'maladaar SAY_SLAY_1'),
(-1558008,'<laugh> Now, you\'ll stay for eternity!',10517,1,0,0,'maladaar SAY_SLAY_2'),
(-1558009,'This is... where.. I belong...',10518,1,0,0,'maladaar SAY_DEATH'),

(-1558010,'%s focuses on $N',0,3,0,0,'shirrak EMOTE_FOCUS');

-- -1 560 000 ESCAPE FROM DURNHOLDE (OLD HILLSBRAD)
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1560000,'Thrall! You didn\'t really think you would escape did you? You and your allies shall answer to Blackmoore - after I\'ve had my fun!',10406,0,0,1,'skarloc SAY_ENTER'),

(-1560001,'My magical power can turn back time to before Thrall\'s death, but be careful. My power to manipulate time is limited.',0,0,0,0,'image of eronzion SAY_RESET_THRALL'),
(-1560002,'I have set back the flow of time just once more. If you fail to prevent Thrall\'s death, then all is lost.',0,0,0,0,'image of eronzion SAY_RESET_THRALL_LAST'),

(-1560003,'What\'s the meaning of this? GUARDS!',0,0,0,0,'armorer SAY_CALL_GUARDS'),
(-1560004,'All that you know... will be undone.',0,0,0,0,'infinite dragon SAY_INFINITE_AGGRO_1'),
(-1560005,'Let\'s go.',0,0,0,0,'thrall hillsbrad SAY_TH_ARMORY2'),
(-1560006,'%s startles the horse with a fierce yell!',0,2,0,0,'thrall hillsbrad EMOTE_TH_STARTLE_HORSE'),
(-1560007,'I thought I saw something go into the barn.',0,0,0,0,'tarren mill lookout SAY_LOOKOUT_BARN_1'),
(-1560008,'I didn\'t see anything.',0,0,0,0,'tarren mill lookout SAY_PROTECTOR_BARN_2'),
(-1560009,'%s tries to calm the horse down.',0,2,0,0,'thrall hillsbrad EMOTE_TH_CALM_HORSE'),
(-1560010,'Something riled that horse. Let\'s go!',0,0,0,0,'tarren mill lookout SAY_PROTECTOR_BARN_3'),
(-1560011,'Taretha isn\'t here. Let\'s head into town.',0,0,0,0,'thrall hillsbrad SAY_TH_HEAD_TOWN'),
(-1560012,'She\'s not here.',0,0,0,0,'thrall hillsbrad SAY_TH_CHURCH_ENTER'),

(-1560013,'Thrall! Come outside and face your fate!',10418,1,0,0,'epoch SAY_ENTER1'),
(-1560014,'Taretha\'s life hangs in the balance. Surely you care for her. Surely you wish to save her...',10419,1,0,0,'epoch SAY_ENTER2'),
(-1560015,'Ah, there you are. I had hoped to accomplish this with a bit of subtlety, but I suppose direct confrontation was inevitable. Your future, Thrall, must not come to pass and so...you and your troublesome friends must die!',10420,1,0,0,'epoch SAY_ENTER3'),

(-1560016,'Thrall\'s trapped himself in the chapel. He can\'t escape now.',0,0,0,0,'tarren mill lookout SAY_LOOKOUT_CHURCH'),
(-1560017,'He\'s here, stop him!',0,0,0,0,'tarren mill lookout SAY_LOOKOUT_INN'),
(-1560018,'We have all the time in the world....',0,0,0,0,'infinite dragon SAY_INFINITE_AGGRO_2'),
(-1560019,'You cannot escape us!',0,0,0,0,'infinite dragon SAY_INFINITE_AGGRO_3'),
(-1560020,'Do not think you can win!',0,0,0,0,'infinite dragon SAY_INFINITE_AGGRO_4'),

(-1560021,'REUSE_ME',0,0,0,0,'REUSE_ME'),
(-1560022,'REUSE_ME',0,0,0,0,'REUSE_ME'),

(-1560023,'Very well then. Let\'s go!',10465,0,0,0,'thrall hillsbrad SAY_TH_START_EVENT_PART1'),
(-1560024,'As long as we\'re going with a new plan, I may aswell pick up a weapon and some armor.',0,0,0,0,'thrall hillsbrad SAY_TH_ARMORY'),
(-1560025,'A rider approaches!',10466,0,0,0,'thrall hillsbrad SAY_TH_SKARLOC_MEET'),
(-1560026,'I\'ll never be chained again!',10467,1,0,0,'thrall hillsbrad SAY_TH_SKARLOC_TAUNT'),
(-1560027,'Very well. Tarren Mill lies just west of here. Since time is of the essence...',10468,0,0,0,'thrall hillsbrad SAY_TH_START_EVENT_PART2'),
(-1560028,'Let\'s ride!',10469,0,0,1,'thrall hillsbrad SAY_TH_MOUNTS_UP'),
(-1560029,'Taretha must be in the inn. Let\'s go.',0,0,0,0,'thrall hillsbrad SAY_TH_CHURCH_END'),
(-1560030,'Taretha! What foul magic is this?',0,0,0,0,'thrall hillsbrad SAY_TH_MEET_TARETHA'),
(-1560031,'Who or what was that?',10470,0,0,1,'thrall hillsbrad SAY_TH_EPOCH_WONDER'),
(-1560032,'No!',10471,0,0,5,'thrall hillsbrad SAY_TH_EPOCH_KILL_TARETHA'),
(-1560033,'Goodbye, Taretha. I will never forget your kindness.',10472,0,0,0,'thrall hillsbrad SAY_TH_EVENT_COMPLETE'),
(-1560034,'Things are looking grim...',10458,1,0,0,'thrall hillsbrad SAY_TH_RANDOM_LOW_HP1'),
(-1560035,'I will fight to the last!',10459,1,0,0,'thrall hillsbrad SAY_TH_RANDOM_LOW_HP2'),
(-1560036,'Taretha...',10460,1,0,0,'thrall hillsbrad SAY_TH_RANDOM_DIE1'),
(-1560037,'A good day...to die...',10461,1,0,0,'thrall hillsbrad SAY_TH_RANDOM_DIE2'),
(-1560038,'I have earned my freedom!',10448,0,0,0,'thrall hillsbrad SAY_TH_RANDOM_AGGRO1'),
(-1560039,'This day is long overdue. Out of my way!',10449,0,0,0,'thrall hillsbrad SAY_TH_RANDOM_AGGRO2'),
(-1560040,'I am a slave no longer!',10450,0,0,0,'thrall hillsbrad SAY_TH_RANDOM_AGGRO3'),
(-1560041,'Blackmoore has much to answer for!',10451,0,0,0,'thrall hillsbrad SAY_TH_RANDOM_AGGRO4'),
(-1560042,'You have forced my hand!',10452,0,0,0,'thrall hillsbrad SAY_TH_RANDOM_KILL1'),
(-1560043,'It should not have come to this!',10453,0,0,0,'thrall hillsbrad SAY_TH_RANDOM_KILL2'),
(-1560044,'I did not ask for this!',10454,0,0,0,'thrall hillsbrad SAY_TH_RANDOM_KILL3'),
(-1560045,'I am truly in your debt, strangers.',10455,0,0,0,'thrall hillsbrad SAY_TH_LEAVE_COMBAT1'),
(-1560046,'Thank you, strangers. You have given me hope.',10456,0,0,0,'thrall hillsbrad SAY_TH_LEAVE_COMBAT2'),
(-1560047,'I will not waste this chance. I will seek out my destiny.',10457,0,0,0,'thrall hillsbrad SAY_TH_LEAVE_COMBAT3'),

(-1560048,'I\'m free! Thank you all!',0,0,0,0,'taretha SAY_TA_FREE'),
(-1560049,'Thrall, you escaped!',0,0,0,0,'taretha SAY_TA_ESCAPED'),

(-1560050,'That\'s enough out of him.',0,0,0,0,'thrall hillsbrad SAY_TH_KILL_ARMORER'),
(-1560051,'That spell should wipe their memories of us and what just happened. All they should remember now is what reality would be like without the attempted temporal interference. Well done. Thrall will journey on to find his destiny, and Taretha...',0,0,0,0,'erozion SAY_WIPE_MEMORY'),
(-1560052,'Her fate is regrettably unavoidable.',0,0,0,0,'erozion SAY_ABOUT_TARETHA'),
(-1560053,'They call you a monster. But they\'re the monsters, not you. Farewell Thrall.',0,0,0,0,'taretha SAY_TA_FAREWELL'),

(-1560054,'I\'m glad you\'re safe, Taretha. None of this would have been possible without your friends. They made all of this happen.',0,0,0,0,'thrall hillsbrad SAY_TR_GLAD_SAFE'),
(-1560055,'Thrall, I\'ve never met these people before in my life.',0,0,0,0,'taretha SAY_TA_NEVER_MET'),
(-1560056,'Then who are these people?',0,0,0,0,'thrall hillsbrad SAY_TR_THEN_WHO'),
(-1560057,'I believe I can explain everything to you two if you give me a moment of your time.',0,0,0,0,'erozion SAY_PRE_WIPE'),
(-1560058,'You have done a great thing. Alas, the young warchief\'s memory of these events must be as they originally were ... Andormu awaits you in the master\'s lair.',0,0,0,0,'erozion SAY_AFTER_WIPE');

-- -1 564 000 BLACK TEMPLE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1564000,'You will die in the name of Lady Vashj!',11450,1,0,0,'SAY_AGGRO'),
(-1564001,'Stick around!',11451,1,0,0,'SAY_NEEDLE1'),
(-1564002,'I\'ll deal with you later!',11452,1,0,0,'SAY_NEEDLE2'),
(-1564003,'Your success was short lived!',11455,1,0,0,'SAY_SLAY1'),
(-1564004,'Time for you to go!',11456,1,0,0,'SAY_SLAY2'),
(-1564005,'Bel\'anen dal\'lorei!',11453,1,0,0,'SAY_SPECIAL1'),
(-1564006,'Blood will flow!',11454,1,0,0,'SAY_SPECIAL2'),
(-1564007,'Bal\'amer ch\'itah!',11457,1,0,0,'SAY_ENRAGE1'),
(-1564008,'My patience has ran out! Die, DIE!',11458,1,0,0,'SAY_ENRAGE2'),
(-1564009,'Lord Illidan will... crush you.',11459,1,0,0,'SAY_DEATH'),

(-1564010,'%s acquires a new target!',0,3,0,0,'supremus EMOTE_NEW_TARGET'),
(-1564011,'%s punches the ground in anger!',0,3,0,0,'supremus EMOTE_PUNCH_GROUND'),
(-1564012,'The ground begins to crack open!',0,3,0,0,'supremus EMOTE_GROUND_CRACK'),

(-1564013,'No! Not yet...',11386,1,0,0,'akama shade SAY_LOW_HEALTH'),
(-1564014,'I will not last much longer...',11385,1,0,0,'akama shade SAY_DEATH'),
(-1564015,'Come out from the shadows! I\'ve returned to lead you against our true enemy! Shed your chains and raise your weapons against your Illidari masters!',0,1,0,0,'akama shade SAY_FREE'),
(-1564016,'Hail our leader! Hail Akama!',0,1,0,0,'akama shade broken SAY_BROKEN_FREE_01'),
(-1564017,'Hail Akama!',0,1,0,0,'akama shade broken SAY_BROKEN_FREE_02'),

(-1564018,'You play, you pay.',11501,1,0,0,'shahraz SAY_TAUNT1'),
(-1564019,'I\'m not impressed.',11502,1,0,0,'shahraz SAY_TAUNT2'),
(-1564020,'Enjoying yourselves?',11503,1,0,0,'shahraz SAY_TAUNT3'),
(-1564021,'So... business or pleasure?',11504,1,0,0,'shahraz SAY_AGGRO'),
(-1564022,'You seem a little tense.',11505,1,0,0,'shahraz SAY_SPELL1'),
(-1564023,'Don\'t be shy.',11506,1,0,0,'shahraz SAY_SPELL2'),
(-1564024,'I\'m all... yours.',11507,1,0,0,'shahraz SAY_SPELL3'),
(-1564025,'Easy come, easy go.',11508,1,0,0,'shahraz SAY_SLAY1'),
(-1564026,'So much for a happy ending.',11509,1,0,0,'shahraz SAY_SLAY2'),
(-1564027,'Stop toying with my emotions!',11510,1,0,0,'shahraz SAY_ENRAGE'),
(-1564028,'I wasn\'t... finished.',11511,1,0,0,'shahraz SAY_DEATH'),

(-1564029,'Horde will... crush you.',11432,1,0,0,'bloodboil SOUND_AGGRO'),
(-1564030,'Time to feast!',11433,1,0,0,'bloodboil SAY_SLAY1'),
(-1564031,'More! I want more!',11434,1,0,0,'bloodboil SAY_SLAY2'),
(-1564032,'Drink your blood! Eat your flesh!',11435,1,0,0,'bloodboil SAY_SPECIAL1'),
(-1564033,'I hunger!',11436,1,0,0,'bloodboil SAY_SPECIAL2'),
(-1564034,'<babbling>',11437,1,0,0,'bloodboil SAY_ENRAGE1'),
(-1564035,'I\'ll rip the meat from your bones!',11438,1,0,0,'bloodboil SAY_ENRAGE2'),
(-1564036,'Aaaahrg...',11439,1,0,0,'bloodboil SAY_DEATH'),

(-1564037,'I was the first, you know. For me, the wheel of death has spun many times. <laughs> So much time has passed. I have a lot of catching up to do...',11512,1,0,0,'teron SAY_INTRO'),
(-1564038,'Vengeance is mine!',11513,1,0,0,'teron SAY_AGGRO'),
(-1564039,'I have use for you!',11514,1,0,0,'teron SAY_SLAY1'),
(-1564040,'It gets worse...',11515,1,0,0,'teron SAY_SLAY2'),
(-1564041,'What are you afraid of?',11517,1,0,0,'teron SAY_SPELL1'),
(-1564042,'Death... really isn\'t so bad.',11516,1,0,0,'teron SAY_SPELL2'),
(-1564043,'Give in!',11518,1,0,0,'teron SAY_SPECIAL1'),
(-1564044,'I have something for you...',11519,1,0,0,'teron SAY_SPECIAL2'),
(-1564045,'YOU WILL SHOW THE PROPER RESPECT!',11520,1,0,0,'teron SAY_ENRAGE'),
(-1564046,'The wheel...spins...again....',11521,1,0,0,'teron SAY_DEATH'),

(-1564047,'Pain and suffering are all that await you!',11415,1,0,0,'essence SUFF_SAY_FREED'),
(-1564048,'Don\'t leave me alone!',11416,1,0,0,'essence SUFF_SAY_AGGRO'),
(-1564049,'Look at what you make me do!',11417,1,0,0,'essence SUFF_SAY_SLAY1'),
(-1564050,'I didn\'t ask for this!',11418,1,0,0,'essence SUFF_SAY_SLAY2'),
(-1564051,'The pain is only beginning!',11419,1,0,0,'essence SUFF_SAY_SLAY3'),
(-1564052,'I don\'t want to go back!',11420,1,0,0,'essence SUFF_SAY_RECAP'),
(-1564053,'Now what do I do?',11421,1,0,0,'essence SUFF_SAY_AFTER'),
(-1564054,'REUSE_ME',0,0,0,0,'REUSE_ME'),

(-1564055,'You can have anything you desire... for a price.',11408,1,0,0,'essence DESI_SAY_FREED'),
(-1564056,'Fulfilment is at hand!',11409,1,0,0,'essence DESI_SAY_SLAY1'),
(-1564057,'Yes... you\'ll stay with us now...',11410,1,0,0,'essence DESI_SAY_SLAY2'),
(-1564058,'Your reach exceeds your grasp.',11412,1,0,0,'essence DESI_SAY_SLAY3'),
(-1564059,'Be careful what you wish for...',11411,1,0,0,'essence DESI_SAY_SPEC'),
(-1564060,'I\'ll be waiting...',11413,1,0,0,'essence DESI_SAY_RECAP'),
(-1564061,'I won\'t be far...',11414,1,0,0,'essence DESI_SAY_AFTER'),

(-1564062,'Beware: I live!',11399,1,0,0,'essence ANGER_SAY_FREED'),
(-1564063,'So... foolish.',11400,1,0,0,'essence ANGER_SAY_FREED2'),
(-1564064,'<maniacal cackle>',11401,1,0,0,'essence ANGER_SAY_SLAY1'),
(-1564065,'Enough. No more.',11402,1,0,0,'essence ANGER_SAY_SLAY2'),
(-1564066,'On your knees!',11403,1,0,0,'essence ANGER_SAY_SPEC'),
(-1564067,'Beware, coward.',11405,1,0,0,'essence ANGER_SAY_BEFORE'),
(-1564068,'I won\'t... be... ignored.',11404,1,0,0,'essence ANGER_SAY_DEATH'),

(-1564069,'You wish to test me?',11524,1,0,0,'council vera AGGRO'),
(-1564070,'I have better things to do...',11422,1,0,0,'council gath AGGRO'),
(-1564071,'Flee or die!',11482,1,0,0,'council mala AGGRO'),
(-1564072,'Common... such a crude language. Bandal!',11440,1,0,0,'council zere AGGRO'),

(-1564073,'Enough games!',11428,1,0,0,'council gath ENRAGE'),
(-1564074,'You wish to kill me? Hahaha, you first!',11530,1,0,0,'council vera ENRAGE'),
(-1564075,'For Quel\'Thalas! For the Sunwell!',11488,1,0,0,'council mala ENRAGE'),
(-1564076,'Sha\'amoor sine menoor!',11446,1,0,0,'council zere ENRAGE'),

(-1564077,'Enjoy your final moments!',11426,1,0,0,'council gath SPECIAL1'),
(-1564078,'You\'re not caught up for this!',11528,1,0,0,'council vera SPECIAL1'),
(-1564079,'No second chances!',11486,1,0,0,'council mala SPECIAL1'),
(-1564080,'Diel fin\'al',11444,1,0,0,'council zere SPECIAL1'),

(-1564081,'You are mine!',11427,1,0,0,'council gath SPECIAL2'),
(-1564082,'Anar\'alah belore!',11529,1,0,0,'council vera SPECIAL2'),
(-1564083,'I\'m full of surprises!',11487,1,0,0,'council mala SPECIAL2'),
(-1564084,'Sha\'amoor ara mashal?',11445,1,0,0,'council zere SPECIAL2'),

(-1564085,'Selama am\'oronor!',11423,1,0,0,'council gath SLAY'),
(-1564086,'Valiant effort!',11525,1,0,0,'council vera SLAY'),
(-1564087,'My work is done.',11483,1,0,0,'council mala SLAY'),
(-1564088,'Shorel\'aran.',11441,1,0,0,'council zere SLAY'),

(-1564089,'Well done!',11424,1,0,0,'council gath SLAY_COMT'),
(-1564090,'A glorious kill!',11526,1,0,0,'council vera SLAY_COMT'),
(-1564091,'As it should be!',11484,1,0,0,'council mala SLAY_COMT'),
(-1564092,'Belesa menoor!',11442,1,0,0,'council zere SLAY_COMT'),

(-1564093,'Lord Illidan... I...',11425,1,0,0,'council gath DEATH'),
(-1564094,'You got lucky!',11527,1,0,0,'council vera DEATH'),
(-1564095,'Destiny... awaits.',11485,1,0,0,'council mala DEATH'),
(-1564096,'Diel ma\'ahn... oreindel\'o',11443,1,0,0,'council zere DEATH'),

(-1564097,'Akama. Your duplicity is hardly surprising. I should have slaughtered you and your malformed brethren long ago.',11463,1,0,0,'illidan SAY_ILLIDAN_SPEECH_1'),
(-1564098,'We\'ve come to end your reign, Illidan. My people and all of Outland shall be free!',11389,1,0,25,'akama(illidan) SAY_AKAMA_SPEECH_2'),
(-1564099,'Boldly said. But I remain unconvinced.',11464,1,0,6,'illidan SAY_ILLIDAN_SPEECH_3'),
(-1564100,'The time has come! The moment is at hand!',11380,1,0,22,'akama(illidan) SAY_AKAMA_SPEECH_4'),
(-1564101,'You are not prepared!',11466,1,0,406,'illidan SAY_ILLIDAN_SPEECH_5'),
(-1564102,'Is this it, mortals? Is this all the fury you can muster?',11476,1,0,0,'illidan SAY_ILLIDAN_SPEECH_6'),
(-1564103,'Their fury pales before mine, Illidan. We have some unsettled business between us.',11491,1,0,6,'maiev SAY_MAIEV_SPEECH_7'),
(-1564104,'Maiev... How is this even possible?',11477,1,0,1,'illidan SAY_ILLIDAN_SPEECH_8'),
(-1564105,'My long hunt is finally over. Today, Justice will be done!',11492,1,0,5,'maiev SAY_MAIEV_SPEECH_9'),
(-1564106,'Feel the hatred of ten thousand years!',11470,1,0,0,'illidan SAY_FRENZY'),
(-1564107,'It is finished. You are beaten.',11496,1,0,0,'maiev SAY_MAIEV_EPILOGUE_1'),
(-1564108,'You have won... Maiev. But the huntress... is nothing without the hunt. You... are nothing... without me.',11478,1,0,0,'illidan SAY_ILLIDAN_EPILOGUE_2'),
(-1564109,'He\'s right. I feel nothing... I am... nothing.',11497,1,0,0,'maiev SAY_MAIEV_EPILOGUE_3'),
(-1564110,'Farewell, champions.',11498,1,0,0,'maiev SAY_MAIEV_EPILOGUE_4'),
(-1564111,'The Light will fill these dismal halls once again. I swear it.',11387,1,0,0,'akama(illidan) SAY_AKAMA_EPILOGUE_5'),
(-1564112,'I can feel your hatred.',11467,1,0,0,'illidan SAY_TAUNT_1'),
(-1564113,'Give in to your fear!',11468,1,0,0,'illidan SAY_TAUNT_2'),
(-1564114,'You know nothing of power!',11469,1,0,0,'illidan SAY_TAUNT_3'),
(-1564115,'Such... arrogance!',11471,1,0,0,'illidan SAY_TAUNT_4'),
(-1564116,'That is for Naisha!',11493,1,0,0,'maiev SAY_MAIEV_TAUNT_1'),
(-1564117,'Bleed as I have bled!',11494,1,0,0,'maiev SAY_MAIEV_TAUNT_2'),
(-1564118,'There shall be no prison for you this time!',11495,1,0,0,'maiev SAY_MAIEV_TRAP'),
(-1564119,'Meet your end, demon!',11500,1,0,0,'maiev SAY_MAIEV_TAUNT_4'),
(-1564120,'Be wary friends, The Betrayer meditates in the court just beyond.',11388,1,0,0,'akama(illidan) SAY_AKAMA_BEWARE'),
(-1564121,'Come, my minions. Deal with this traitor as he deserves!',11465,1,0,0,'illidan SAY_AKAMA_MINION'),
(-1564122,'I\'ll deal with these mongrels. Strike now, friends! Strike at the betrayer!',11390,1,0,22,'akama(illidan) SAY_AKAMA_LEAVE'),
(-1564123,'Who shall be next to taste my blades?!',11473,1,0,0,'illidan SAY_KILL1'),
(-1564124,'This is too easy!',11472,1,0,0,'illidan SAY_KILL2'),
(-1564125,'I will not be touched by rabble such as you!',11479,1,0,254,'illidan SAY_TAKEOFF'),
(-1564126,'Behold the flames of Azzinoth!',11480,1,0,0,'illidan SAY_SUMMONFLAMES'),
(-1564127,'Stare into the eyes of the Betrayer!',11481,1,0,0,'illidan SAY_EYE_BLAST'),
(-1564128,'Behold the power... of the demon within!',11475,1,0,0,'illidan SAY_MORPH'),
(-1564129,'You\'ve wasted too much time mortals, now you shall fall!',11474,1,0,0,'illidan SAY_ENRAGE'),

(-1564130,'Broken of the Ashtongue tribe, your leader speaks!',0,1,0,0,'akama(shade) SAY_FREE_1'),

(-1564131,'This door is all that stands between us and the Betrayer. Stand aside, friends.',0,0,0,1,'akama(illidan) SAY_OPEN_DOOR_1'),
(-1564132,'I cannot do this alone...',0,0,0,0,'akama(illidan) SAY_OPEN_DOOR_2'),
(-1564133,'You are not alone, Akama.',0,0,0,0,'spirit_Udalo SAY_OPEN_DOOR_3'),
(-1564134,'Your people will always be with you!',0,0,0,0,'spirit_Olum SAY_OPEN_DOOR_4');

-- -1 565 000 GRUUL'S LAIR
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1565000,'Gronn are the real power in outland.',11367,1,0,0,'maulgar SAY_AGGRO'),
(-1565001,'You will not defeat the hand of Gruul!',11368,1,0,0,'maulgar SAY_ENRAGE'),
(-1565002,'You won\'t kill next one so easy!',11369,1,0,0,'maulgar SAY_OGRE_DEATH1'),
(-1565003,'Pah! Does not prove anything!',11370,1,0,0,'maulgar SAY_OGRE_DEATH2'),
(-1565004,'I\'m not afraid of you.',11371,1,0,0,'maulgar SAY_OGRE_DEATH3'),
(-1565005,'Good, now you fight me!',11372,1,0,0,'maulgar SAY_OGRE_DEATH4'),
(-1565006,'You not so tough afterall!',11373,1,0,0,'maulgar SAY_SLAY1'),
(-1565007,'Aha-ha ha ha!',11374,1,0,0,'maulgar SAY_SLAY2'),
(-1565008,'Mulgar is king!',11375,1,0,0,'maulgar SAY_SLAY3'),
(-1565009,'Gruul... will crush you...',11376,1,0,0,'maulgar SAY_DEATH'),

(-1565010,'Come... and die.',11355,1,0,0,'gruul SAY_AGGRO'),
(-1565011,'Scurry',11356,1,0,0,'gruul SAY_SLAM1'),
(-1565012,'No escape',11357,1,0,0,'gruul SAY_SLAM2'),
(-1565013,'Stay',11358,1,0,0,'gruul SAY_SHATTER1'),
(-1565014,'Beg... for life',11359,1,0,0,'gruul SAY_SHATTER2'),
(-1565015,'No more',11360,1,0,0,'gruul SAY_SLAY1'),
(-1565016,'Unworthy',11361,1,0,0,'gruul SAY_SLAY2'),
(-1565017,'Die',11362,1,0,0,'gruul SAY_SLAY3'),
(-1565018,'Aaargh...',11363,1,0,0,'gruul SAY_DEATH'),
(-1565019,'%s grows in size!',0,2,0,0,'gruul EMOTE_GROW');

-- -1 568 000 ZUL'AMAN
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1568000,'Spirits of da wind be your doom!',12031,1,0,0,'janalai SAY_AGGRO'),
(-1568001,'I burn ya now!',12032,1,0,0,'janalai SAY_FIRE_BOMBS'),
(-1568002,'Where ma hatcha? Get to work on dem eggs!',12033,1,0,0,'janalai SAY_SUMMON_HATCHER'),
(-1568003,'I show you strength... in numbers.',12034,1,0,0,'janalai SAY_ALL_EGGS'),
(-1568004,'You done run outta time!',12035,1,0,0,'janalai SAY_BERSERK'),
(-1568005,'It all be over now, mon!',12036,1,0,0,'janalai SAY_SLAY_1'),
(-1568006,'Tazaga-choo!',12037,1,0,0,'janalai SAY_SLAY_2'),
(-1568007,'Zul\'jin... got a surprise for you...',12038,1,0,0,'janalai SAY_DEATH'),
(-1568008,'Come, strangers. The spirit of the dragonhawk hot be hungry for worthy souls.',12039,1,0,0,'janalai SAY_EVENT_STRANGERS'),
(-1568009,'Come, friends. Your bodies gonna feed ma hatchlings, and your souls are going to feed me with power!',12040,1,0,0,'janalai SAY_EVENT_FRIENDS'),

(-1568010,'Get da move on, guards! It be killin\' time!',12066,1,0,0,'nalorakk SAY_WAVE1_AGGRO'),
(-1568011,'Guards, go already! Who you more afraid of, dem... or me?',12067,1,0,0,'nalorakk SAY_WAVE2_STAIR1'),
(-1568012,'Ride now! Ride out dere and bring me back some heads!',12068,1,0,0,'nalorakk SAY_WAVE3_STAIR2'),
(-1568013,'I be losin\' me patience! Go on: make dem wish dey was never born!',12069,1,0,0,'nalorakk SAY_WAVE4_PLATFORM'),
(-1568014,'What could be better than servin\' da bear spirit for eternity? Come closer now. Bring your souls to me!',12078,1,0,0,'nalorakk SAY_EVENT1_SACRIFICE'),
(-1568015,'Don\'t be delayin\' your fate. Come to me now. I make your sacrifice quick.',12079,1,0,0,'nalorakk SAY_EVENT2_SACRIFICE'),
(-1568016,'You be dead soon enough!',12070,1,0,0,'nalorakk SAY_AGGRO'),
(-1568017,'I bring da pain!',12071,1,0,0,'nalorakk SAY_SURGE'),
(-1568018,'You call on da beast, you gonna get more dan you bargain for!',12072,1,0,0,'nalorakk SAY_TOBEAR'),
(-1568019,'Make way for Nalorakk!',12073,1,0,0,'nalorakk SAY_TOTROLL'),
(-1568020,'You had your chance, now it be too late!',12074,1,0,0,'nalorakk SAY_BERSERK'),
(-1568021,'Mua-ha-ha! Now whatchoo got to say?',12075,1,0,0,'nalorakk SAY_SLAY1'),
(-1568022,'Da Amani gonna rule again!',12076,1,0,0,'nalorakk SAY_SLAY2'),
(-1568023,'I... be waitin\' on da udda side....',12077,1,0,0,'nalorakk SAY_DEATH'),

(-1568024,'Da eagles gonna bear your spirits to me. Your sacrifice is not gonna be in vein!',12122,1,0,0,'akilzon SAY_EVENT1'),
(-1568025,'Your death gonna be quick, strangers. You shoulda never have come to this place...',12123,1,0,0,'akilzon SAY_EVENT2'),
(-1568026,'I be da predator! You da prey...',12013,1,0,0,'akilzon SAY_AGGRO'),
(-1568027,'Feed, me bruddahs!',12014,1,0,0,'akilzon SAY_SUMMON'),
(-1568028,'Come, and join me bruddahs!',12015,1,0,0,'akilzon SAY_SUMMON_ALT'),
(-1568029,'All you be doing is wasting my time!',12016,1,0,0,'akilzon SAY_ENRAGE'),
(-1568030,'Ya got nothin\'!',12017,1,0,0,'akilzon SAY_SLAY1'),
(-1568031,'Stop your cryin\'!',12018,1,0,0,'akilzon SAY_SLAY2'),
(-1568032,'You can\'t... kill... me spirit!',12019,1,0,0,'akilzon SAY_DEATH'),
(-1568033,'An Electrical Storm Appears!',0,2,0,0,'akilzon EMOTE_STORM'),

(-1568034,'Get on ya knees and bow.... to da fang and claw!',12020,1,0,0,'halazzi SAY_AGGRO'),
(-1568035,'I fight wit\' untamed spirit....',12021,1,0,0,'halazzi SAY_SPLIT'),
(-1568036,'Spirit, come back to me!',12022,1,0,0,'halazzi SAY_MERGE'),
(-1568037,'Me gonna carve ya now!',12023,1,0,0,'halazzi SAY_SABERLASH1'),
(-1568038,'You gonna leave in pieces!',12024,1,0,0,'halazzi SAY_SABERLASH2'),
(-1568039,'Whatch you be doing? Pissin\' yourselves...',12025,1,0,0,'halazzi SAY_BERSERK'),
(-1568040,'You cant fight the power!',12026,1,0,0,'halazzi SAY_KILL1'),
(-1568041,'You gonna fail!',12027,1,0,0,'halazzi SAY_KILL2'),
(-1568042,'Chaga... choka\'jinn.',12028,1,0,0,'halazzi SAY_DEATH'),
(-1568043,'Come, fools. Fill ma empty cages...',12029,1,0,0,'halazzi SAY_EVENT1'),
(-1568044,'I be waitin, strangers. Your deaths gonna make me stronger!',12030,1,0,0,'halazzi SAY_EVENT2'),

(-1568045,'Da shadow gonna fall on you...',12041,1,0,0,'malacrass SAY_AGGRO'),
(-1568046,'Ya don\'t kill me yet, ya don\'t get another chance!',12042,1,0,0,'malacrass SAY_ENRAGE'),
(-1568047,'Dis a nightmare ya don\' wake up from!',12043,1,0,0,'malacrass SAY_KILL1'),
(-1568048,'Azzaga choogo zinn!',12044,1,0,0,'malacrass SAY_KILL2'),
(-1568049,'Your will belong ta me now!',12045,1,0,0,'malacrass SAY_SOUL_SIPHON'),
(-1568050,'Darkness comin\' for you...',12046,1,0,0,'malacrass SAY_DRAIN_POWER'),
(-1568051,'Your soul gonna bleed!',12047,1,0,0,'malacrass SAY_SPIRIT_BOLTS'),
(-1568052,'It not gonna make no difference.',12048,1,0,0,'malacrass SAY_ADD_DIED1'),
(-1568053,'You gonna die worse dan him.',12049,1,0,0,'malacrass SAY_ADD_DIED2'),
(-1568054,'Dat no bodda me.',12050,1,0,0,'malacrass SAY_ADD_DIED3'),
(-1568055,'Dis not... da end of me...',12051,1,0,0,'malacrass SAY_DEATH'),

(-1568056,'Everybody always wanna take from us. Now we gonna start takin\' back. Anybody who get in our way...gonna drown in dey own blood! Da Amani empire be back now...seekin\' vengeance. And we gonna start wit\' you.',12090,1,0,0,'zuljin SAY_INTRO'),
(-1568057,'Nobody badduh dan me!',12091,1,0,0,'zuljin SAY_AGGRO'),
(-1568058,'Got me some new tricks... like me brudda bear....',12092,1,0,0,'zuljin SAY_BEAR_TRANSFORM'),
(-1568059,'Dere be no hidin\' from da eagle!',12093,1,0,0,'zuljin SAY_EAGLE_TRANSFORM'),
(-1568060,'Let me introduce you to me new bruddas: fang and claw!',12094,1,0,0,'zuljin SAY_LYNX_TRANSFORM'),
(-1568061,'Ya don\' have to look to da sky to see da dragonhawk!',12095,1,0,0,'zuljin SAY_DRAGONHAWK_TRANSFORM'),
(-1568062,'Fire kill you just as quick!',12096,1,0,0,'zuljin SAY_FIRE_BREATH'),
(-1568063,'You too slow! Me too strong!',12097,1,0,0,'zuljin SAY_BERSERK'),
(-1568064,'Da Amani de chuka!',12098,1,0,0,'zuljin SAY_KILL1'),
(-1568065,'Lot more gonna fall like you!',12099,1,0,0,'zuljin SAY_KILL2'),
(-1568066,'Mebbe me fall...but da Amani empire...never gonna die...',12100,1,0,0,'zuljin SAY_DEATH'),

(-1568067,'Zul\'jin got a surprise for ya...',12052,6,0,0,'zulaman SAY_INST_RELEASE'),
(-1568068,'Da spirits gonna feast today! Begin da ceremonies, sacrifice da prisoners... make room for our new guests!',12053,6,0,0,'zulaman SAY_INST_BEGIN'),
(-1568069,'Take your pick, trespassers! Any of ma priests be happy to accommodate ya.',12054,6,0,0,'zulaman SAY_INST_PROGRESS_1'),
(-1568070,'Don\'t be shy. Thousands have come before you. Ya not be alone in your service.',12055,6,0,0,'zulaman SAY_INST_PROGRESS_2'),
(-1568071,'Ya gonna fail, strangers. Many try before you, but dey only make us stronger!',12056,6,0,0,'zulaman SAY_INST_PROGRESS_3'),
(-1568072,'Your efforts was in vain, trespassers. The rituals nearly be complete.',12057,6,0,0,'zulaman SAY_INST_WARN_1'),
(-1568073,'Soon da cages gonna be empty, da sacrifices be complete, and you gonna take dere places.',12058,6,0,0,'zulaman SAY_INST_WARN_2'),
(-1568074,'Time be running low, strangers. Soon you gonna join da souls of dem ya failed to save.',12059,6,0,0,'zulaman SAY_INST_WARN_3'),
(-1568075,'Make haste, ma priests! Da rituals must not be interrupted!',12060,6,0,0,'zulaman SAY_INST_WARN_4'),
(-1568076,'Ya make a good try... but now you gonna join da ones who already fall.',12061,6,0,0,'zulaman SAY_INST_SACRIF1'),
(-1568077,'Ya not do too bad. Ya efforts [...] for a small time. Come to me now. Ya prove yourself worthy offerings.',12062,6,0,0,'zulaman SAY_INST_SACRIF2'),
(-1568078,'Watch now. Every offering gonna strengthen our ties to da spirit world. Soon, we gonna be unstoppable!',12065,6,0,0,'zulaman SAY_INST_COMPLETE'),

(-1568079,'Suit yourself. At least five of you must assist me if we\'re to get inside. Follow me.',0,1,0,0,'harrison SAY_START'),
(-1568080,'According to my calculations, if enough of us bang the gong at once the seal on these doors will break and we can enter.',0,1,0,0,'harrison SAY_AT_GONG'),
(-1568081,'I\'ve researched this site extensively and I won\'t allow any dim-witted treasure hunters to swoop in and steal what belongs to in a museum. I\'ll lead this charge.',0,1,0,0,'harrison SAY_OPEN_ENTRANCE'),

(-1568082,'%s absorbs the essence of the bear spirit!',0,2,0,0,'zuljin EMOTE_BEAR_SPIRIT'),
(-1568083,'%s absorbs the essence of the eagle spirit!',0,2,0,0,'zuljin EMOTE_EAGLE_SPIRIT'),
(-1568084,'%s absorbs the essence of the lynx spirit!',0,2,0,0,'zuljin EMOTE_LYNX_SPIRIT'),
(-1568085,'%s absorbs the essence of the dragonhawk spirit!',0,2,0,0,'zuljin EMOTE_DRAGONHAWK_SPIRIT');

-- -1 574 000 UTGARDE KEEP
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1574000,'Your blood is mine!',13221,1,0,0,'keleseth SAY_AGGRO'),
(-1574001,'Not so fast.',13222,1,0,0,'keleseth SAY_FROSTTOMB'),
(-1574002,'Aranal, lidel! Their fate shall be yours!',13224,1,0,0,'keleseth SAY_SKELETONS'),
(-1574003,'Darkness waits!',13223,1,0,0,'keleseth SAY_KILL'),
(-1574004,'I join... the night.',13225,1,0,0,'keleseth SAY_DEATH'),

(-1574005,'I\'ll paint my face with your blood!',13207,1,0,0,'ingvar SAY_AGGRO_FIRST'),
(-1574006,'I return! A second chance to carve out your skull!',13209,1,0,0,'ingvar SAY_AGGRO_SECOND'),
(-1574007,'My life for the... death god!',13213,1,0,0,'ingvar SAY_DEATH_FIRST'),
(-1574008,'No! I can do... better! I can...',13211,1,0,0,'ingvar SAY_DEATH_SECOND'),
(-1574009,'Mjul orm agn gjor!',13212,1,0,0,'ingvar SAY_KILL_FIRST'),
(-1574010,'I am a warrior born!',13214,1,0,0,'ingvar SAY_KILL_SECOND'),

(-1574011,'Dalronn! See if you can muster the nerve to join my attack!',13229,1,0,0,'skarvald SAY_SKA_AGGRO'),
(-1574012,'Not... over... yet.',13230,1,0,0,'skarvald SAY_SKA_DEATH'),
(-1574013,'A warrior\'s death.',13231,1,0,0,'skarvald SAY_SKA_DEATH_REAL'),
(-1574014,'???',13232,1,0,0,'skarvald SAY_SKA_KILL'),
(-1574015,'Pagh! What sort of necromancer lets death stop him? I knew you were worthless!',13233,1,0,0,'skarvald SAY_SKA_DAL_DIES_REPLY'),

(-1574016,'By all means, don\'t assess the situation, you halfwit! Just jump into the fray!',13199,1,0,0,'dalronn SAY_DAL_AGGRO_REPLY'),
(-1574017,'See... you... soon.',13200,1,0,0,'dalronn SAY_DAL_DEATH'),
(-1574018,'There\'s no... greater... glory.',13201,1,0,0,'dalronn SAY_DAL_DEATH_REAL'),
(-1574019,'You may serve me yet.',13202,1,0,0,'dalronn SAY_DAL_KILL'),
(-1574020,'Skarvald, you incompetent slug! Return and make yourself useful!',13203,1,0,0,'dalronn SAY_DAL_SKA_DIES_REPLY'),

(-1574021,'%s casts Frost Tomb on $N',0,3,0,0,'keleseth EMOTE_FROST_TOMB'),

(-1574022,'%s roars!',0,3,0,0,'ingvar EMOTE_ROAR'),
(-1574023,'Ingvar! Your pathetic failure will serve as a warning to all... you are damned! Arise and carry out the masters will!',13754,1,0,0,'annhylde REZZ');

-- -1 575 000 UTGARDE PINNACLE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1575000,'My liege! I have done as you asked, and now beseech you for your blessing!',13856,1,0,0,'svala SAY_INTRO_1'),
(-1575001,'Your sacrifice is a testament to your obedience. Indeed you are worthy of this charge. Arise, and forever be known as Svala Sorrowgrave!',14732,1,0,0,'svala SAY_INTRO_2_ARTHAS'),
(-1575002,'The sensation is... beyond my imagining. I am yours to command, my king.',13857,1,0,0,'svala SAY_INTRO_3'),
(-1575003,'Your first test awaits you. Destroy our uninvited guests.',14733,1,0,0,'svala SAY_INTRO_4_ARTHAS'),
(-1575004,'I will be happy to slaughter them in your name! Come, enemies of the Scourge! I will show you the might of the Lich King!',13858,1,0,0,'svala SAY_INTRO_5'),
(-1575005,'I will vanquish your soul!',13842,1,0,0,'svala SAY_AGGRO'),
(-1575006,'You were a fool to challenge the power of the Lich King!',13845,1,0,0,'svala SAY_SLAY_1'),
(-1575007,'Your will is done, my king.',13847,1,0,0,'svala SAY_SLAY_2'),
(-1575008,'Another soul for my master.',13848,1,0,0,'svala SAY_SLAY_3'),
(-1575009,'Your death approaches.',13850,1,0,0,'svala SAY_SACRIFICE_1'),
(-1575010,'Go now to my master.',13851,1,0,0,'svala SAY_SACRIFICE_2'),
(-1575011,'Your end is inevitable.',13852,1,0,0,'svala SAY_SACRIFICE_3'),
(-1575012,'Yor-guul mak!',13853,1,0,0,'svala SAY_SACRIFICE_4'),
(-1575013,'Any last words?',13854,1,0,0,'svala SAY_SACRIFICE_5'),
(-1575014,'Nooo! I did not come this far... to...',13855,1,0,0,'svala SAY_DEATH'),

(-1575015,'What this place? I will destroy you!',13464,1,0,0,'gortok SAY_AGGRO'),
(-1575016,'You die! That what master wants!',13465,1,0,0,'gortok SAY_SLAY_1'),
(-1575017,'An easy task!',13466,1,0,0,'gortok SAY_SLAY_2'),
(-1575018,' ',13467,1,0,0,'gortok SAY_DEATH'),

(-1575019,'What mongrels dare intrude here? Look alive, my brothers! A feast for the one that brings me their heads!',13497,1,0,22,'skadi SAY_AGGRO'),
(-1575020,'Sear them to the bone!',13498,1,0,0,'skadi SAY_DRAKEBREATH_1'),
(-1575021,'Go now! Leave nothing but ash in your wake!',13499,1,0,0,'skadi SAY_DRAKEBREATH_2'),
(-1575022,'Cleanse our sacred halls with flame!',13500,1,0,0,'skadi SAY_DRAKEBREATH_3'),
(-1575023,'I ask for ... to kill them, yet all I get is feeble whelps! By Ye.. SLAUGHTER THEM!',13501,1,0,0,'skadi SAY_DRAKE_HARPOON_1'),
(-1575024,'If one more harpoon touches my drake I\'ll flae my miserable heins.',13502,1,0,0,'skadi SAY_DRAKE_HARPOON_2'),
(-1575025,'Mjor Na Ul Kaval!',13503,1,0,0,'skadi SAY_KILL_1'),
(-1575026,'Not so brash now, are you?',13504,1,0,0,'skadi SAY_KILL_2'),
(-1575027,'I\'ll mount your skull from the highest tower!',13505,1,0,0,'skadi SAY_KILL_3'),
(-1575028,'ARGH! You call that... an attack? I\'ll... show... aghhhh...',13506,1,0,0,'skadi SAY_DEATH'),
(-1575029,'You motherless knaves! Your corpses will make fine morsels for my new drake!',13507,1,0,0,'skadi SAY_DRAKE_DEATH'),
(-1575030,'%s is within range of the harpoon launchers!',0,3,0,0,'skadi EMOTE_HARPOON_RANGE'),

(-1575031,'You invade my home and then dare to challenge me? I will tear the hearts from your chests and offer them as gifts to the death god! Rualg nja gaborr!',13609,1,0,0,'ymiron SAY_AGGRO'),
(-1575032,'Bjorn of the Black Storm! Honor me now with your presence!',13610,1,0,0,'ymiron SAY_SUMMON_BJORN'),
(-1575033,'Haldor of the rocky cliffs, grant me your strength!',13611,1,0,0,'ymiron SAY_SUMMON_HALDOR'),
(-1575034,'Ranulf of the screaming abyss, snuff these maggots with darkest night!',13612,1,0,0,'ymiron SAY_SUMMON_RANULF'),
(-1575035,'Tor of the Brutal Siege! Bestow your might upon me!',13613,1,0,0,'ymiron SAY_SUMMON_TORGYN'),
(-1575036,'Your death is only the beginning!',13614,1,0,0,'ymiron SAY_SLAY_1'),
(-1575037,'You have failed your people!',13615,1,0,0,'ymiron SAY_SLAY_2'),
(-1575038,'There is a reason I am king!',13616,1,0,0,'ymiron SAY_SLAY_3'),
(-1575039,'Bleed no more!',13617,1,0,0,'ymiron SAY_SLAY_4'),
(-1575040,'What... awaits me... now?',13618,1,0,0,'ymiron SAY_DEATH'),

(-1575041,'%s takes a deep breath.',0,3,0,0,'grauf EMOTE_DEEP_BREATH');

-- -1 576 000 NEXUS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1576000,'You know what they say about curiosity.',13319,1,0,0,'telestra SAY_AGGRO'),
(-1576001,'I\'ll give you more than you can handle.',13321,1,0,0,'telestra SAY_SPLIT_1'),
(-1576002,'There\'s plenty of me to go around.',13322,1,0,0,'telestra SAY_SPLIT_2'),
(-1576003,'Now to finish the job!',13323,1,0,0,'telestra SAY_MERGE'),
(-1576004,'Death becomes you!',13324,1,0,0,'telestra SAY_KILL'),
(-1576005,'Damn the... luck.',13320,1,0,0,'telestra SAY_DEATH'),

(-1576006,'Chaos beckons.',13186,1,0,0,'anomalus SAY_AGGRO'),
(-1576007,'Reality... unwoven.',13188,1,0,0,'anomalus SAY_RIFT'),
(-1576008,'Indestructible.',13189,1,0,0,'anomalus SAY_SHIELD'),
(-1576009,'Expiration... is necesarry.',13274,1,0,0,'anomalus SAY_KILL'),
(-1576010,'Of course.',13187,1,0,0,'anomalus SAY_DEATH'),

(-1576011,'Noo!',13328,1,0,0,'ormorok SAY_AGGRO'),
(-1576012,'???',13329,1,0,0,'ormorok SAY_KILL'),
(-1576013,'Baaack!',13331,1,0,0,'ormorok SAY_REFLECT'),
(-1576014,'Bleeeed!',13332,1,0,0,'ormorok SAY_ICESPIKE'),
(-1576015,'Aaggh!',13330,1,0,0,'ormorok SAY_DEATH'),

(-1576016,'Preserve? Why? There\'s no truth in it. No no no... only in the taking! I see that now!',13450,1,0,0,'keristrasza SAY_AGGRO'),
(-1576017,'Stay. Enjoy your final moments.',13451,1,0,0,'keristrasza SAY_CRYSTAL_NOVA'),
(-1576018,'Finish it! Finish it! Kill me, or I swear by the Dragonqueen you\'ll never see daylight again!',13452,1,0,0,'keristrasza SAY_ENRAGE'),
(-1576019,'Now we\'ve come to the truth!',13453,1,0,0,'keristrasza SAY_KILL'),
(-1576020,'Dragonqueen... Life-Binder... preserve... me.',13454,1,0,0,'keristrasza SAY_DEATH'),

(-1576021,'%s opens a Chaotic Rift!',0,3,0,0,'anomalus EMOTE_OPEN_RIFT'),
(-1576022,'%s shields himself and divert his power to the rifts!',0,3,0,0,'anomalus EMOTE_SHIELD');

-- -1 578 000 OCULUS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1578000,'What do we have here... those would defy the Spell-Weaver? Those without foresight or understanding. How could I make you see? Malygos is saving the world from itself! Bah! You are hardly worth my time!',13635,1,0,0,'urom SAY_SUMMON_1'),
(-1578001,'Clearly my pets failed. Perhaps another demonstration is in order.',13636,1,0,0,'urom SAY_SUMMON_2'),
(-1578002,'Still you fight. Still you cling to misguided principles. If you survive, you\'ll find me in the center ring.',13637,1,0,0,'urom SAY_SUMMON_3'),
(-1578003,'Poor blind fools!',13638,1,0,0,'urom SAY_AGGRO'),
(-1578004,'A taste... just a small taste... of the Spell-Weaver\'s power!',13639,1,0,0,'urom SAY_EXPLOSION_1'),
(-1578005,'So much unstable energy... but worth the risk to destroy you!',13640,1,0,0,'urom SAY_EXPLOSION_2'),
(-1578006,'If only you understood!',13641,1,0,0,'urom SAY_KILL_1'),
(-1578007,'Now do you see? Do you?!',13642,1,0,0,'urom SAY_KILL_2'),
(-1578008,'Unfortunate, but necessary.',13643,1,0,0,'urom SAY_KILL_3'),
(-1578009,'Everything I\'ve done... has been for Azeroth...',13644,1,0,0,'urom SAY_DEATH'),

(-1578010,'Simpletons! You cannot comprehend the forces you have set in motion. The ley line conduit will not be disrupted! Your defeat shall be absolute!',13622,6,0,0,'eregos SAY_SPAWN'),
(-1578011,'You brash interlopers are out of your element! I will ground you!',13623,1,0,0,'eregos SAY_AGGRO'),
(-1578012,'We command the arcane! It shall not be used against us.',13626,1,0,0,'eregos SAY_ARCANE_SHIELD'),
(-1578013,'It is trivial to extinguish your fire!',13627,1,0,0,'eregos SAY_FIRE_SHIELD'),
(-1578014,'No magic of nature will help you now!',13625,1,0,0,'eregos SAY_NATURE_SHIELD'),
(-1578015,'Such insolence... such arrogance... must be PUNISHED!',13624,1,0,0,'eregos SAY_FRENZY'),
(-1578016,'It\'s a long way down...',13628,1,0,0,'eregos SAY_KILL_1'),
(-1578017,'Back to the earth with you!',13629,1,0,0,'eregos SAY_KILL_2'),
(-1578018,'Enjoy the fall!',13630,1,0,0,'eregos SAY_KILL_3'),
(-1578019,'Savor this small victory, foolish little creatures. You and your dragon allies have won this battle. But we will win... the Nexus War.',13631,1,0,0,'eregos SAY_DEATH'),

(-1578020,'There will be no mercy!',13649,1,0,0,'varos SAY_AGGRO'),
(-1578021,'Blast them! Destroy them!',13650,1,0,0,'varos SAY_CALL_CAPTAIN_1'),
(-1578022,'Take no prisoners! Attack!',13651,1,0,0,'varos SAY_CALL_CAPTAIN_2'),
(-1578023,'Strike now! Obliterate them!',13652,1,0,0,'varos SAY_CALL_CAPTAIN_3'),

(-1578024,'Anomalies form as %s shifts into the Astral Plane!',0,3,0,0,'eregos EMOTE_ASTRAL_PLANE'),
(-1578025,'%s begins to cast Empowered Arcane Explosion!',0,3,0,0,'urom EMOTE_EXPLOSION'),

(-1578026,'You were warned!',13653,1,0,0,'varos SAY_KILL_1'),
(-1578027,'The Oculus is ours!',13654,1,0,0,'varos SAY_KILL_2'),
(-1578028,'They are... too strong! Underestimated their... fortitude.',13655,1,0,0,'varos SAY_DEATH'),
(-1578029,'%s calls an Azure Ring Captain!',0,3,0,0,'varos EMOTE_CAPTAIN'),

(-1578030,'%s flies away.',0,2,0,0,'drakes EMOTE_FLY_AWAY');

-- -1 580 000 SUNWELL PLATEAU
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1580000,'Aggh! No longer will I be a slave to Malygos! Challenge me and you will be destroyed!',12422,1,0,0,'kalecgos SAY_EVIL_AGGRO'),
(-1580001,'I will purge you!',12423,1,0,0,'kalecgos SAY_EVIL_SPELL1'),
(-1580002,'Your pain has only begun!',12424,1,0,0,'kalecgos SAY_EVIL_SPELL2'),
(-1580003,'In the name of Kil\'jaeden!',12425,1,0,0,'kalecgos SAY_EVIL_SLAY1'),
(-1580004,'You were warned!',12426,1,0,0,'kalecgos SAY_EVIL_SLAY2'),
(-1580005,'My awakening is complete! You shall all perish!',12427,1,0,0,'kalecgos SAY_EVIL_ENRAGE'),
(-1580006,'I need... your help... Cannot... resist him... much longer...',12428,1,0,0,'kalecgos humanoid SAY_GOOD_AGGRO'),
(-1580007,'Aaahhh! Help me, before I lose my mind!',12429,1,0,0,'kalecgos humanoid SAY_GOOD_NEAR_DEATH'),
(-1580008,'Hurry! There is not much of me left!',12430,1,0,0,'kalecgos humanoid SAY_GOOD_NEAR_DEATH2'),
(-1580009,'I am forever in your debt. Once we have triumphed over Kil\'jaeden, this entire world will be in your debt as well.',12431,1,0,0,'kalecgos humanoid SAY_GOOD_PLRWIN'),
(-1580010,'There will be no reprieve. My work here is nearly finished.',12451,1,0,0,'sathrovarr SAY_SATH_AGGRO'),
(-1580011,'I\'m... never on... the losing... side...',12452,1,0,0,'sathrovarr SAY_SATH_DEATH'),
(-1580012,'Your misery is my delight!',12453,1,0,0,'sathrovarr SAY_SATH_SPELL1'),
(-1580013,'I will watch you bleed!',12454,1,0,0,'sathrovarr SAY_SATH_SPELL2'),
(-1580014,'Pitious mortal!',12455,1,0,0,'sathrovarr SAY_SATH_SLAY1'),
(-1580015,'Haven\'t you heard? I always win!',12456,1,0,0,'sathrovarr SAY_SATH_SLAY2'),
(-1580016,'I have toyed with you long enough!',12457,1,0,0,'sathrovarr SAY_SATH_ENRAGE'),

(-1580017,'Puny lizard! Death is the only answer you\'ll find here!',12458,1,0,0,'brutallus YELL_INTRO'),
(-1580018,'Grah! Your magic is weak!',12459,1,0,0,'brutallus YELL_INTRO_BREAK_ICE'),
(-1580019,'I will crush you!',12460,1,0,0,'brutallus YELL_INTRO_CHARGE'),
(-1580020,'That was fun, but I still await a true challenge!',12461,1,0,0,'brutallus YELL_INTRO_KILL_MADRIGOSA'),
(-1580021,'Come, try your luck!',12462,1,0,0,'brutallus YELL_INTRO_TAUNT'),
(-1580022,'Ahh! More lambs to the slaughter!',12463,1,0,0,'brutallus YELL_AGGRO'),
(-1580023,'Perish, insect!',12464,1,0,0,'brutallus YELL_KILL1'),
(-1580024,'You are meat!',12465,1,0,0,'brutallus YELL_KILL2'),
(-1580025,'Too easy!',12466,1,0,0,'brutallus YELL_KILL3'),
(-1580026,'Bring the fight to me!',12467,1,0,0,'brutallus YELL_LOVE1'),
(-1580027,'Another day, another glorious battle!',12468,1,0,0,'brutallus YELL_LOVE2'),
(-1580028,'I live for this!',12469,1,0,0,'brutallus YELL_LOVE3'),
(-1580029,'So much for a real challenge... Die!',12470,1,0,0,'brutallus YELL_BERSERK'),
(-1580030,'Gah! Well done... Now... this gets... interesting...',12471,1,0,0,'brutallus YELL_DEATH'),

(-1580031,'Hold, friends! There is information to be had before this devil meets his fate!',12472,1,0,0,'madrigosa YELL_MADR_ICE_BARRIER'),
(-1580032,'Where is Anveena, demon? What has become of Kalec?',12473,1,0,293,'madrigosa YELL_MADR_INTRO'),
(-1580033,'You will tell me where they are!',12474,1,0,0,'madrigosa YELL_MADR_ICE_BLOCK'),
(-1580034,'Speak, I grow weary of asking!',12475,1,0,0,'madrigosa YELL_MADR_TRAP'),
(-1580035,'Malygos, my lord! I did my best!',12476,1,0,0,'madrigosa YELL_MADR_DEATH'),

(-1580036,'Glory to Kil\'jaeden! Death to all who oppose!',12477,1,0,0,'felmyst SAY_INTRO'),
(-1580037,'I kill for the master!',12480,1,0,0,'felmyst SAY_KILL_1'),
(-1580038,'The end has come! ',12481,1,0,0,'felmyst SAY_KILL_2'),
(-1580039,'Choke on your final breath! ',12478,1,0,0,'felmyst SAY_BREATH'),
(-1580040,'I am stronger than ever before! ',12479,1,0,0,'felmyst SAY_TAKEOFF'),
(-1580041,'No more hesitation! Your fates are written! ',12482,1,0,0,'felmyst SAY_BERSERK'),
(-1580042,'Kil\'jaeden... will... prevail... ',12483,1,0,0,'felmyst SAY_DEATH'),
(-1580043,'Madrigosa deserved a far better fate. You did what had to be done, but this battle is far from over.',12439,1,0,0,'kalecgos SAY_KALECGOS_OUTRO'),

(-1580044,'Misery...',12484,1,0,0,'sacrolash SAY_INTRO_1'),
(-1580045,'Depravity...',0,1,0,0,'alythess SAY_INTRO_2'),
(-1580046,'Confusion...',0,1,0,0,'sacrolash SAY_INTRO_3'),
(-1580047,'Hatred...',0,1,0,0,'alythess SAY_INTRO_4'),
(-1580048,'Mistrust...',0,1,0,0,'sacrolash SAY_INTRO_5'),
(-1580049,'Chaos...',0,1,0,0,'alythess SAY_INTRO_6'),
(-1580050,'These are the hallmarks...',0,1,0,0,'sacrolash SAY_INTRO_7'),
(-1580051,'These are the pillars...',0,1,0,0,'alythess SAY_INTRO_8'),

(-1580052,'Shadow to the aid of fire!',12485,1,0,0,'sacrolash SAY_SACROLASH_SHADOW_NOVA'),
(-1580053,'Alythess! Your fire burns within me!',12488,1,0,0,'sacrolash SAY_SACROLASH_EMPOWER'),
(-1580054,'Shadows, engulf!',12486,1,0,0,'sacrolash SAY_SACROLASH_KILL_1'),
(-1580055,'Ee-nok Kryul!',12487,1,0,0,'sacrolash SAY_SACROLASH_KILL_2'),
(-1580056,'I... fade.',12399,1,0,0,'sacrolash SAY_SACROLASH_DEAD'),
(-1580057,'Time is a luxury you no longer possess!',0,1,0,0,'sacrolash SAY_SACROLASH_BERSERK'),
(-1580058,'Fire to the aid of shadow!',12489,1,0,0,'alythess SAY_ALYTHESS_CANFLAGRATION'),
(-1580059,'Sacrolash!',12492,1,0,0,'alythess SAY_ALYTHESS_EMPOWER'),
(-1580060,'Fire, consume!',12490,1,0,0,'alythess SAY_ALYTHESS_KILL_1'),
(-1580061,'Ed-ir Halach!',12491,1,0,0,'alythess SAY_ALYTHESS_KILL_2'),
(-1580062,'De-ek Anur!',12494,1,0,0,'alythess SAY_ALYTHESS_DEAD'),
(-1580063,'Your luck has run its course!',12493,1,0,0,'alythess SAY_ALYTHESS_BERSERK'),

(-1580064,'All my plans have led to this!',12495,6,0,0,'kiljaeden SAY_ORDER_1'),
(-1580065,'Stay on task! Do not waste time!',12496,6,0,0,'kiljaeden SAY_ORDER_2'),
(-1580066,'I have waited long enough!',12497,6,0,0,'kiljaeden SAY_ORDER_3'),
(-1580067,'Fail me and suffer for eternity!',12498,6,0,0,'kiljaeden SAY_ORDER_4'),
(-1580068,'Drain the girl! Drain her power until there is nothing but a vacant shell!',12499,6,0,0,'kiljaeden SAY_ORDER_5'),
(-1580069,'The expendible have perished... So be it! Now I shall succeed where Sargeras could not! I will bleed this wretched world and secure my place as the true master of the Burning Legion. The end has come! Let the unraveling of this world commence!',12500,1,0,0,'kiljaeden SAY_EMERGE'),
(-1580070,'Another step towards destruction!',12501,1,0,0,'kiljaeden SAY_SLAY_1'),
(-1580071,'Anukh-Kyrie!',12502,1,0,0,'kiljaeden SAY_SLAY_2'),
(-1580072,'Who can you trust?',12503,1,0,0,'kiljaeden SAY_REFLECTION_1'),
(-1580073,'The enemy is among you.',12504,1,0,0,'kiljaeden SAY_REFLECTION_2'),
(-1580074,'Chaos!',12505,1,0,0,'kiljaeden SAY_DARKNESS_1'),
(-1580075,'Destruction!',12506,1,0,0,'kiljaeden SAY_DARKNESS_2'),
(-1580076,'Oblivion!',12507,1,0,0,'kiljaeden SAY_DARKNESS_3'),
(-1580077,'I will not be denied! This world shall fall!',12508,1,0,0,'kiljaeden SAY_PHASE_3'),
(-1580078,'Do not harbor false hope. You cannot win!',12509,1,0,0,'kiljaeden SAY_PHASE_4'),
(-1580079,'Aggghh! The powers of the Sunwell... turn... against me! What have you done? What have you done???',12510,1,0,0,'kiljaeden SAY_PHASE_5'),
(-1580080,'You are not alone. The Blue Dragonflight shall help you vanquish the Deceiver.',12438,1,0,0,'kalecgos SAY_KALECGOS_INTRO'),
(-1580081,'Anveena, you must awaken, this world needs you!',12445,1,0,0,'kalecgos SAY_KALECGOS_AWAKE_1'),
(-1580082,'I serve only the Master now.',12511,0,0,0,'anveena SAY_ANVEENA_IMPRISONED'),
(-1580083,'You must let go! You must become what you were always meant to be! The time is now, Anveena!',12446,1,0,0,'kalecgos SAY_KALECGOS_AWAKE_2'),
(-1580084,'But I\'m... lost. I cannot find my way back.',12512,0,0,0,'anveena SAY_ANVEENA_LOST'),
(-1580085,'Anveena, I love you! Focus on my voice, come back for me now! Only you can cleanse the Sunwell!',12447,1,0,0,'kalecgos SAY_KALECGOS_AWAKE_4'),
(-1580086,'Kalec... Kalec?',12513,0,0,0,'anveena SAY_ANVEENA_AWAKE'),
(-1580087,'Yes, Anveena! Let fate embrace you now!',12448,1,0,0,'kalecgos SAY_KALECGOS_AWAKE_5'),
(-1580088,'The nightmare is over, the spell is broken! Goodbye, Kalec, my love!',12514,0,0,0,'anveena SAY_ANVEENA_SACRIFICE'),
(-1580089,'Goodbye, Anveena, my love. Few will remember your name, yet this day you change the course of destiny. What was once corrupt is now pure. Heroes, do not let her sacrifice be in vain.',12450,0,0,0,'kalecgos SAY_KALECGOS_GOODBYE'),
(-1580090,'Strike now, heroes, while he is weakened! Vanquish the Deceiver!',12449,1,0,0,'kalecgos SAY_KALECGOS_ENCOURAGE'),
(-1580091,'I will channel my power into the orbs, be ready!',12440,1,0,0,'kalecgos SAY_KALECGOS_ORB_1'),
(-1580092,'I have empowered another orb! Use it quickly!',12441,1,0,0,'kalecgos SAY_KALECGOS_ORB_2'),
(-1580093,'Another orb is ready! Make haste!',12442,1,0,0,'kalecgos SAY_KALECGOS_ORB_3'),
(-1580094,'I have channeled all I can! The power is in your hands!',12443,1,0,0,'kalecgos SAY_KALECGOS_ORB_4'),

(-1580095,'Mortal heroes - your victory here today was foretold long ago. My brother\'s anguished cry of defeat will echo across the universe - bringing renewed hope to all those who still stand against the Burning Crusade.',12515,0,0,1,'velen SAY_OUTRO_1'),
(-1580096,'As the Legion\'s final defeat draws ever-nearer, stand proud in the knowledge that you have saved worlds without number from the flame.',12516,0,0,1,'velen SAY_OUTRO_2'),
(-1580097,'Just as this day marks an ending, so too does it herald a new beginning...',12517,0,0,1,'velen SAY_OUTRO_3'),
(-1580098,'The creature Entropius, whom you were forced to destroy, was once the noble naaru, M\'uru. In life, M\'uru channeled vast energies of LIGHT and HOPE. For a time, a misguided few sought to steal those energies...',12518,0,0,1,'velen SAY_OUTRO_4'),
(-1580099,'Our arrogance was unpardonable. We damned one of the most noble beings of all. We may never atone for this sin.',12524,0,0,1,'liadrin SAY_OUTRO_5'),
(-1580100,'Than fortunate it is, that I have reclaimed the noble naaru\'s spark from where it fell! Where faith dwells, hope is never lost, young blood elf.',12519,0,0,1,'velen SAY_OUTRO_6'),
(-1580101,'Can it be ?',12525,0,0,1,'liadrin SAY_OUTRO_7'),
(-1580102,'Gaz now, mortals - upon the HEART OF M\'URU! Umblemished. Bathed by the light of Creation - just as it was at the Dawn.',12520,0,0,1,'velen SAY_OUTRO_8'),
(-1580103,'In time, the light and hope held within - will rebirth more than this mere fount of power... Mayhap, they will rebirth the soul of a nation.',12521,0,0,1,'velen SAY_OUTRO_9'),
(-1580104,'Blessed ancestors! I feel it... so much love... so much grace... there are... no words... impossible to describe...',12526,0,0,1,'liadrin SAY_OUTRO_10'),
(-1580105,'Salvation, young one. It waits for us all.',12522,0,0,1,'velen SAY_OUTRO_11'),
(-1580106,'Farewell...!',12523,0,0,1,'velen SAY_OUTRO_12'),

(-1580107,'%s takes a deep breath.',0,3,0,0,'felmyst EMOTE_DEEP_BREATH');

-- -1 585 000 MAGISTER'S TERRACE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1585000,'You only waste my time!',12378,1,0,0,'selin SAY_AGGRO'),
(-1585001,'My hunger knows no bounds!',12381,1,0,0,'selin SAY_ENERGY'),
(-1585002,'Yes! I am a god!',12382,1,0,0,'selin SAY_EMPOWERED'),
(-1585003,'Enough distractions!',12388,1,0,0,'selin SAY_KILL_1'),
(-1585004,'I am invincible!',12385,1,0,0,'selin SAY_KILL_2'),
(-1585005,'No! More... I must have more!',12383,1,0,0,'selin SAY_DEATH'),
(-1585006,'%s begins to channel from the nearby Fel Crystal...',0,3,0,0,'selin EMOTE_CRYSTAL'),

(-1585007,'Drain...life!',12389,1,0,0,'vexallus SAY_AGGRO'),
(-1585008,'Un...con...tainable.',12392,1,0,0,'vexallus SAY_ENERGY'),
(-1585009,'Un...leash...',12390,1,0,0,'vexallus SAY_OVERLOAD'),
(-1585010,'Con...sume.',12393,1,0,0,'vexallus SAY_KILL'),
(-1585011,'%s discharges pure energy!',0,3,0,0,'vexallus EMOTE_DISCHARGE_ENERGY'),

(-1585012,'Annihilate them!',12395,1,0,0,'delrissa SAY_AGGRO'),
(-1585013,'Oh, the horror.',12398,1,0,0,'delrissa LackeyDeath1'),
(-1585014,'Well, aren\'t you lucky?',12400,1,0,0,'delrissa LackeyDeath2'),
(-1585015,'Now I\'m getting annoyed.',12401,1,0,0,'delrissa LackeyDeath3'),
(-1585016,'Lackies be damned! I\'ll finish you myself!',12403,1,0,0,'delrissa LackeyDeath4'),
(-1585017,'I call that a good start.',12405,1,0,0,'delrissa PlayerDeath1'),
(-1585018,'I could have sworn there were more of you.',12407,1,0,0,'delrissa PlayerDeath2'),
(-1585019,'Not really much of a group, anymore, is it?',12409,1,0,0,'delrissa PlayerDeath3'),
(-1585020,'One is such a lonely number.',12410,1,0,0,'delrissa PlayerDeath4'),
(-1585021,'It\'s been a kick, really.',12411,1,0,0,'delrissa PlayerDeath5'),
(-1585022,'Not what I had... planned...',12397,1,0,0,'delrissa SAY_DEATH'),

(-1585023,'Don\'t look so smug! I know what you\'re thinking, but Tempest Keep was merely a set back. Did you honestly believe I would trust the future to some blind, half-night elf mongrel?',12413,1,0,0,'kaelthas MT SAY_INTRO_1'),
(-1585024,'Vengeance burns!',12415,1,0,0,'kaelthas MT SAY_PHOENIX'),
(-1585025,'Felomin ashal!',12417,1,0,0,'kaelthas MT SAY_FLAMESTRIKE'),
(-1585026,'I\'ll turn your world... upside... down...',12418,1,0,0,'kaelthas MT SAY_GRAVITY_LAPSE'),
(-1585027,'Master... grant me strength.',12419,1,0,0,'kaelthas MT SAY_TIRED'),
(-1585028,'Do not... get too comfortable.',12420,1,0,0,'kaelthas MT SAY_RECAST_GRAVITY'),
(-1585029,'My demise accomplishes nothing! The Master will have you! You will drown in your own blood! This world shall burn! Aaaghh!',12421,1,0,0,'kaelthas MT SAY_DEATH'),
(-1585030,'Oh no, he was merely an instrument, a stepping stone to a much larger plan! It has all led to this, and this time, you will not interfere!',0,1,0,0,'kaelthas MT SAY_INTRO_2');

-- -1 595 000 CULLING OF STRATHOLME
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1595000,'All soldiers of Lordaeron should immediately report to the entrance of Stratholme, and await further orders from Prince Arthas.',0,6,0,0,'lordaeron crier SAY_SOLDIERS_REPORT'),
(-1595001,'Good work with the crates! Come talk to me in front of Stratholme for your next assignment!',0,4,0,0,'chromie WHISPER_CHROMIE_CRATES'),
(-1595002,'Oh, no! Adventurers, something awful has happened! A colleague of mine has been captured by the Infinite Dragonflight, and they\'re doing something horrible to him! Keeping Arthas is still your highest priority, but if you act fast you could help save a Guardian of Time!',0,4,0,0,'chromie WHISPER_CHROMIE_GUARDIAN'),
(-1595003,'Scourge forces have been spotted near the Festival Lane Gate!',0,6,0,0,'lordaeron crier SAY_SCOURGE_FESTIVAL_LANE'),
(-1595004,'Scourge forces have been spotted near the King\'s Square fountain!',0,6,0,0,'lordaeron crier SAY_SCOURGE_KINGS_SQUARE'),
(-1595005,'Scourge forces have been spotted near the Market Row Gate!',0,6,0,0,'lordaeron crier SAY_SCOURGE_MARKET_ROW'),
(-1595006,'Scourge forces have been spotted near the Town Hall!',0,6,0,0,'lordaeron crier SAY_SCOURGE_TOWN_HALL'),
(-1595007,'Scourge forces have been spotted near the Elder\'s Square Gate!',0,6,0,0,'lordaeron crier SAY_SCOURGE_ELDERS_SQUARE'),
(-1595008,'Champions, meet me at the Town Hall at once. We must take the fight to Mal\'Ganis.',14297,6,0,0,'arthas SAY_MEET_TOWN_HALL'),
(-1595009,'Follow me, I know the way through.',14298,0,0,1,'arthas SAY_FOLLOW'),
(-1595010,'Ah, you\'ve finally arrived Prince Arthas. You\'re here just in the nick of time.',0,0,0,1,'citizen SAY_ARRIVED'),
(-1595011,'Yes, I\'m glad I could get to you before the plague.',14299,0,0,0,'arthas SAY_GET_BEFORE_PLAGUE'),
(-1595012,'What is this sorcery?',14300,0,0,0,'arthas SAY_SORCERY'),
(-1595013,'There\'s no need for you to understand, Arthas. All you need to do is die.',0,0,0,1,'citizen SAY_NO_UNDERSTAND'),
(-1595014,'Mal\'Ganis appears to have more than Scourge in his arsenal. We should make haste.',14301,0,0,1,'arthas SAY_MORE_THAN_SCOURGE'),
(-1595015,'More vile sorcery! Be ready for anything!',14302,0,0,0,'arthas SAY_MORE_SORCERY'),
(-1595016,'Let\'s move on.',14303,0,0,396,'arthas SAY_MOVE_ON'),
(-1595017,'Watch your backs: they have us surrounded in this hall.',14304,0,0,1,'arthas SAY_WATCH_BACKS'),
(-1595018,'Mal\'Ganis is not making this easy.',14305,0,0,396,'arthas SAY_NOT_EASY'),
(-1595019,'They\'re very persistent.',14306,0,0,396,'arthas SAY_PERSISTENT'),
(-1595020,'What else can he put in my way?',14307,0,0,396,'arthas SAY_ELSE'),
(-1595021,'Prince Arthas Menethil, on this day, a powerful darkness has taken hold of your soul. The death you are destined to visit upon others will this day be your own.',13408,1,0,0,'chrono-lord SAY_DARKNESS'),
(-1595022,'I do what I must for Lordaeron, and neither your words nor your actions will stop me.',14309,0,0,396,'arthas SAY_DO_WHAT_MUST'),
(-1595023,'The quickest path to Mal\'Ganis lies behind that bookshelf ahead.',14308,0,0,0,'arthas SAY_QUICK_PATH'),
(-1595024,'This will only take a moment.',14310,0,0,432,'arthas SAY_TAKE_A_MOMENT'),
(-1595025,'I\'m relieved this secret passage still works.',14311,0,0,0,'arthas SAY_PASSAGE'),
(-1595026,'Let\'s move through here as quickly as possible. If the undead don\'t kill us, the fires might.',14312,0,0,396,'arthas SAY_MOVE_QUICKLY'),
(-1595027,'Rest a moment and clear your lungs, but we must move again soon.',14313,0,0,396,'arthas SAY_REST'),
(-1595028,'That\'s enough; we must move again. Mal\'Ganis awaits.',14314,0,0,396,'arthas SAY_REST_COMPLETE'),
(-1595029,'At last some good luck. Market Row has not caught fire yet. Mal\'Ganis is supposed to be in Crusaders\' Square, which is just ahead. Tell me when you\'re ready to move forward.',14315,0,0,396,'arthas SAY_CRUSADER_SQUARE'),
(-1595030,'Justice will be done.',14316,0,0,0,'arthas SAY_JUSTICE'),
(-1595031,'We\'re going to finish this right now, Mal\'Ganis. Just you... and me.',14317,0,0,5,'arthas SAY_FINISH_MALGANIS'),
(-1595032,'Your journey has just begun, young prince. Gather your forces and meet me in the arctic land of Northrend. It is there that we shall settle the score between us. It is there that your true destiny will unfold.',14412,1,0,378,'malganis SAY_JOURNEY_BEGUN'),
(-1595033,'I\'ll hunt you to the ends of the earth if I have to! Do you hear me? To the ends of the earth!',14318,0,0,0,'arthas SAY_HUNT_MALGANIS'),
(-1595034,'You performed well this day. Anything that Mal\'Ganis has left behind is yours. Take it as your reward. I must now begin plans for an expedition to Northrend.',14319,0,0,1,'arthas SAY_ESCORT_COMPLETE'),
(-1595035,'Protect your prince, soldiers of Lordaeron! I am in need of aid!',14320,0,0,0,'arthas SAY_HALF_HP'),
(-1595036,'I am being overwhelmed, assist me!',14321,0,0,0,'arthas SAY_LOW_HP'),
(-1595037,'Mal\'Ganis will pay for this.',14322,0,0,0,'arthas SAY_SLAY_1'),
(-1595038,'I can\'t afford to spare you.',14323,0,0,0,'arthas SAY_SLAY_2'),
(-1595039,'One less obstacle to deal with.',14324,0,0,0,'arthas SAY_SLAY_3'),
(-1595040,'Agh! Damn you, Mal\'Ganis! Father...Jaina...I have failed Lordaeron...',14325,0,0,0,'arthas SAY_DEATH'),
(-1595041,'My work here is finished!',0,6,0,0,'infinite corruptor SAY_CORRUPTOR_DESPAWN');

-- -1 599 000 HALLS OF STONE
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1599000,'Soft, vulnerable shells. Brief, fragile lives. You can not escape the curse of flesh!',14180,1,0,0,'sjonnir SAY_AGGRO'),
(-1599001,'???',14181,1,0,0,'sjonnir SAY_SLAY_1'),
(-1599002,'Flesh is no match for iron!',14182,1,0,0,'sjonnir SAY_SLAY_2'),
(-1599003,'Armies of iron will smother the world!',14183,1,0,0,'sjonnir SAY_SLAY_3'),
(-1599004,'Loken will not rest, until the forge is retaken. You changed nothing!',14184,1,0,0,'sjonnir SAY_DEATH'),

(-1599005,'You shouldn\'t have come...now you will die!',13487,1,0,0,'maiden SAY_AGGRO'),
(-1599006,'Why must it be this way?',13488,1,0,0,'maiden SAY_SLAY_1'),
(-1599007,'You had it coming!',13489,1,0,0,'maiden SAY_SLAY_2'),
(-1599008,'My burden grows heavier.',13490,1,0,0,'maiden SAY_SLAY_3'),
(-1599009,'This is your own fault!',13491,1,0,0,'maiden SAY_SLAY_4'),
(-1599010,'So much lost time... that you\'ll never get back!',13492,1,0,0,'maiden SAY_STUN'),
(-1599011,'I hope you all rot! I never...wanted...this.',13493,1,0,0,'maiden SAY_DEATH'),

(-1599012,'Now that\'s owning your supper!',14244,1,0,0,'brann SAY_KILL_1'),
(-1599013,'Press on, that\'s the way!',14245,1,0,0,'brann SAY_KILL_2'),
(-1599014,'Keep it up now. Plenty of death-dealing for everyone!',14246,1,0,0,'brann SAY_KILL_3'),
(-1599015,'I\'m all kinds of busted up. Might not... make it...',14257,1,0,0,'brann SAY_LOW_HEALTH'),
(-1599016,'Not yet, not... yet-',14258,1,0,0,'brann SAY_DEATH'),
(-1599017,'I\'m doing everything I can!',14260,1,0,0,'brann SAY_PLAYER_DEATH_1'),
(-1599018,'Light preserve you!',14261,1,0,0,'brann SAY_PLAYER_DEATH_2'),
(-1599019,'I hope this is all worth it!',14262,1,0,0,'brann SAY_PLAYER_DEATH_3'),
(-1599020,'Time to get some answers! Let\'s get this show on the road!',14259,1,0,0,'brann SAY_ESCORT_START'),

(-1599021,'Don\'t worry. Old Brann has got your back. Keep that metal monstrosity busy and I\'ll see if I can sweet talk this machine into helping you.',14274,1,0,0,'brann SAY_SPAWN_DWARF'),
(-1599022,'This is a wee bit trickier that before... Oh, bloody--incomin\'!',14275,1,0,0,'brann SAY_SPAWN_TROGG'),
(-1599023,'What in the name o\' Madoran did THAT do? Oh! Wait: I just about got it...',14276,1,0,0,'brann SAY_SPAWN_OOZE'),
(-1599024,'Ha, that did it. Help\'s a-coming. Take this you glow-eying brute!',14277,1,0,0,'brann SAY_SPAWN_EARTHEN'),

(-1599025,'Take a moment and relish this with me! Soon all will be revealed! Okay then, let\'s do this!',14247,1,0,0,'brann SAY_EVENT_INTRO_1'),
(-1599026,'Now keep an eye out! I\'ll have this licked in two shakes of a--',14248,1,0,0,'brann SAY_EVENT_INTRO_2'),
(-1599027,'Warning! Life form pattern not recognized. Archival processing terminated. Continued interference will result in targeted response.',13765,1,0,0,'brann SAY_EVENT_INTRO_3_ABED'),

(-1599028,'Oh, that doesn\'t sound good. We might have a complication or two...',14249,1,0,0,'brann SAY_EVENT_A_1'),
(-1599029,'Security breach in progress. Analysis of historical archives transferred to lower priority queue. Countermeasures engaged.',13756,1,0,0,'brann SAY_EVENT_A_2_KADD'),
(-1599030,'Ah, you want to play hardball, eh? That\'s just my game!',14250,1,0,0,'brann SAY_EVENT_A_3'),

(-1599031,'Couple more minutes and I\'ll--',14251,1,0,0,'brann SAY_EVENT_B_1'),
(-1599032,'Threat index threshold exceeded. Celestial archive aborted. Security level heightened.',13761,1,0,0,'brann SAY_EVENT_B_2_MARN'),
(-1599033,'Heightened? What\'s the good news?',14252,1,0,0,'brann SAY_EVENT_B_3'),

(-1599034,'So that was the problem? Now I\'m makin\' progress...',14253,1,0,0,'brann SAY_EVENT_C_1'),
(-1599035,'Critical threat index. Void analysis diverted. Initiating sanitization protocol.',13767,1,0,0,'brann SAY_EVENT_C_2_ABED'),
(-1599036,'Hang on! Nobody\'s gonna\' be sanitized as long as I have a say in it!',14254,1,0,0,'brann SAY_EVENT_C_3'),

(-1599037,'Ha! The old magic fingers finally won through! Now let\'s get down to-',14255,1,0,0,'brann SAY_EVENT_D_1'),
(-1599038,'Alert! Security fail safes deactivated. Beginning memory purge...',13768,1,0,0,'brann SAY_EVENT_D_2_ABED'),
(-1599039,'Purge? No no no no no! Where did I-- Aha, this should do the trick...',14256,1,0,0,'brann SAY_EVENT_D_3'),
(-1599040,'System online. Life form pattern recognized. Welcome Branbronzan. Query?',13769,1,0,0,'brann SAY_EVENT_D_4_ABED'),

(-1599041,'Query? What do you think I\'m here for? Tea and biscuits? Spill the beans already!',14263,1,0,0,'brann SAY_EVENT_END_01'),
(-1599042,'Tell me how that dwarfs came to be! And start at the beginning!',14264,1,0,0,'brann SAY_EVENT_END_02'),
(-1599043,'Accessing prehistoric data. Retrieved. In the beginning Earthen were created to-',13770,1,0,0,'brann SAY_EVENT_END_03_ABED'),
(-1599044,'Right, right! I know that the Earthen were made of stone to shape the deep reaches of the world but what about the anomalies? Matrix non-stabilizing and whatnot.',14265,1,0,0,'brann SAY_EVENT_END_04'),
(-1599045,'Accessing. In the early stages of its development cycle Azeroth suffered infection by parasitic, necrophotic symbiotes.',13771,1,0,0,'brann SAY_EVENT_END_05_ABED'),
(-1599046,'Necro-what? Speak bloody common will ya?',14266,1,0,0,'brann SAY_EVENT_END_06'),
(-1599047,'Designation: Old Gods. Old Gods rendered all systems, including Earthen defenseless in order to facilitate assimilation. This matrix destabilization has been termed the Curse of Flesh. Effects of destabilization increased over time.',13772,1,0,0,'brann SAY_EVENT_END_07_ABED'),
(-1599048,'Old Gods eh? So they zapped the Earthen with this Curse of Flesh. And then what?',14267,1,0,0,'brann SAY_EVENT_END_08'),
(-1599049,'Accessing. Creators arrived to extirpate symbiotic infection. Assessment revealed that Old God infestation had grown malignant. Excising parasites would result in loss of host.',13757,1,0,0,'brann SAY_EVENT_END_09_KADD'),
(-1599050,'If they killed the Old Gods Azeroth would have been destroyed.',14268,1,0,0,'brann SAY_EVENT_END_10'),
(-1599051,'Correct. Creators neutralized parasitic threat and contained it within the host. Forge of Wills and other systems were instituted to create new Earthen. Safeguards were implemented and protectors were appointed.',13758,1,0,0,'brann SAY_EVENT_END_11_KADD'),
(-1599052,'What protectors?',14269,1,0,0,'brann SAY_EVENT_END_12'),
(-1599053,'Designations: Aesir and Vanir or in common nomenclator Storm and Earth Giants. Sentinel Loken designated supreme. Dragon Aspects appointed to monitor evolution of Azeroth.',13759,1,0,0,'brann SAY_EVENT_END_13_KADD'),
(-1599054,'Aesir and Vanir. Okay. So the Forge of Wills started to make new Earthen. But what happened to the old ones?',14270,1,0,0,'brann SAY_EVENT_END_14'),
(-1599055,'Additional background is relevant to your query. Following global combat between-',13762,1,0,0,'brann SAY_EVENT_END_15_MARN'),
(-1599056,'Hold everything! The Aesir and Vanir went to war? Why?',14271,1,0,0,'brann SAY_EVENT_END_16'),
(-1599057,'Unknown. Data suggests that impetus for global combat originated with prime designate Loken who neutralized all remaining Aesir and Vanir affecting termination of conflict. Prime designate Loken then initiated stasis of several seed races including Earthen, Giant and Vrykul at designated holding facilities.',13763,1,0,0,'brann SAY_EVENT_END_17_MARN'),
(-1599058,'This Loken sounds like a nasty character. Glad we don\'t have to worry about the likes of him anymore. So if I\'m understanding you lads the original Earthen eventually woke up from this statis. And by that time this destabily-whatever had turned them into our brother dwarfs. Or at least dwarf ancestors. Hm?',14272,1,0,0,'brann SAY_EVENT_END_18'),
(-1599059,'Essentially that is correct.',13764,1,0,0,'brann SAY_EVENT_END_19_MARN'),
(-1599060,'Well now. That\'s a lot to digest. I\'m gonna need some time to take all of this in. Thank you!',14273,1,0,0,'brann SAY_EVENT_END_20'),
(-1599061,'Acknowledged Branbronzan. Session terminated.',13773,1,0,0,'brann SAY_EVENT_END_21_ABED'),

(-1599062,'Loken?! That\'s downright bothersome... We might\'ve neutralized the iron dwarves, but I\'d lay odds there\'s another machine somewhere else churnin\' out a whole mess o\' these iron vrykul!',14278,1,0,0,'brann SAY_VICTORY_SJONNIR_1'),
(-1599063,'I\'ll use the forge to make badtches o\' earthen to stand guard... But our greatest challenge still remains: find and stop Loken!',14279,1,0,0,'brann SAY_VICTORY_SJONNIR_2'),

(-1599064,'I think it\'s time to see what\'s behind the door near the entrance. I\'m going to sneak over there, nice and quiet. Meet me at the door and I\'ll get us in.',0,1,0,0,'brann SAY_ENTRANCE_MEET');

-- -1 600 000 DRAK'THARON KEEP
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1600000,'More grunts, more glands, more FOOD!',13181,1,0,0,'trollgore SAY_AGGRO'),
(-1600001,'So hungry! Must feed!',13182,1,0,0,'trollgore SAY_CONSUME'),
(-1600002,'Aaaargh...',13183,1,0,0,'trollgore SAY_DEATH'),
(-1600003,'Corpse go boom!',13184,1,0,0,'trollgore SAY_EXPLODE'),
(-1600004,'You have gone, me gonna eat you!',13185,1,0,0,'trollgore SAY_KILL'),

(-1600005,'The chill that you feel is the herald of your doom!',13173,1,0,0,'novos SAY_AGGRO'),
(-1600006,'Your efforts... are in vain.',13174,1,0,0,'novos SAY_DEATH'),
(-1600007,'Such is the fate of all who oppose the Lich King.',13175,1,0,0,'novos SAY_KILL'),
(-1600008,'Bolster my defenses! Hurry, curse you!',13176,1,0,0,'novos SAY_ADDS'),
(-1600009,'Surely you can see the futility of it all!',13177,1,0,0,'novos SAY_BUBBLE_1'),
(-1600010,'Just give up and die already!',13178,1,0,0,'novos SAY_BUBBLE_2'),
(-1600011,'%s calls for assistance.',0,3,0,0,'novos EMOTE_ASSISTANCE'),

(-1600012,'Tharon\'ja sees all! The work of mortals shall not end the eternal dynasty!',13862,1,0,0,'tharonja SAY_AGGRO'),
(-1600013,'As Tharon\'ja predicted.',13863,1,0,0,'tharonja SAY_KILL_1'),
(-1600014,'As it was written.',13864,1,0,0,'tharonja SAY_KILL_2'),
(-1600015,'Your flesh serves Tharon\'ja now!',13865,1,0,0,'tharonja SAY_FLESH_1'),
(-1600016,'Tharon\'ja has a use for your mortal shell!',13866,1,0,0,'tharonja SAY_FLESH_2'),
(-1600017,'No! A taste... all too brief!',13867,1,0,0,'tharonja SAY_SKELETON_1'),
(-1600018,'Tharon\'ja will have more!',13868,1,0,0,'tharonja SAY_SKELETON_2'),
(-1600019,'Im... impossible! Tharon\'ja is eternal! Tharon\'ja... is...',13869,1,0,0,'tharonja SAY_DEATH');

-- -1 601 000 AZJOL-NERUB
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1601000,'This kingdom belongs to the Scourge! Only the dead may enter.',14075,1,0,0,'krikthir SAY_AGGRO'),
(-1601001,'???',14076,1,0,0,'krikthir SAY_KILL_1'),
(-1601002,'You were foolish to come.',14077,1,0,0,'krikthir SAY_KILL_2'),
(-1601003,'As Anub\'Arak commands!',14078,1,0,0,'krikthir SAY_KILL_3'),
(-1601004,'We are besieged. Strike out and bring back their corpses!',14079,1,0,0,'krikthir SAY_SEND_GROUP_1'),
(-1601005,'We must hold the gate. Attack! Tear them limb from limb!',14080,1,0,0,'krikthir SAY_SEND_GROUP_2'),
(-1601006,'The gate must be protected at all costs. Rip them to shreds!',14081,1,0,0,'krikthir SAY_SEND_GROUP_3'),
(-1601007,'Keep an eye on the tunnel. We must not let anyone through!',14082,1,0,0,'krikthir SAY_PREFIGHT_1'),
(-1601008,'I hear footsteps. Be on your guard.',14083,1,0,0,'krikthir SAY_PREFIGHT_2'),
(-1601009,'I sense the living. Be ready.',14084,1,0,0,'krikthir SAY_PREFIGHT_3'),
(-1601010,'They hunger.',14085,1,0,0,'krikthir SAY_SWARM_1'),
(-1601011,'Dinner time, my pets.',14086,1,0,0,'krikthir SAY_SWARM_2'),
(-1601012,'I should be grateful. But I long ago lost the capacity.',14087,1,0,0,'krikthir SAY_DEATH'),

(-1601013,'%s moves up the tunnel!',0,3,0,0,'hadronox EMOTE_MOVE_TUNNEL'),

(-1601014,'I was king of this empire once, long ago. In life I stood as champion. In death I returned as conqueror. Now I protect the kingdom once more. Ironic, yes?',14053,1,0,0,'anubarak SAY_INTRO'),
(-1601015,'Eternal agony awaits you!',14054,1,0,0,'anubarak SAY_AGGRO'),
(-1601016,'You shall experience my torment, first-hand!',14055,1,0,0,'anubarak SAY_KILL_1'),
(-1601017,'You have made your choice.',14056,1,0,0,'anubarak SAY_KILL_2'),
(-1601018,'Soon, the Master\'s voice will call to you.',14057,1,0,0,'anubarak SAY_KILL_3'),
(-1601019,'Come forth, my brethren. Feast on their flesh!',14059,1,0,0,'anubarak SAY_SUBMERGE_1'),
(-1601020,'Auum na-l ak-k-k-k, isshhh.',14058,1,0,0,'anubarak SAY_SUBMERGE_2'),
(-1601021,'Your armor is useless against my locusts!',14060,1,0,0,'anubarak SAY_LOCUST_1'),
(-1601022,'The pestilence upon you!',14068,1,0,0,'anubarak SAY_LOCUST_2'),
(-1601023,'Uunak-hissss tik-k-k-k-k!',14067,1,0,0,'anubarak SAY_LOCUST_3'),
(-1601024,'Ahhh... RAAAAAGH! Never thought... I would be free of him...',14069,1,0,0,'anubarak SAY_DEATH'),

(-1601025,'The gate has been breached! Quickly, divert forces to deal with these invaders!',13941,1,0,0,'anub\'ar crusher SAY_AGGRO'),
(-1601026,'There\'s no time left! All remaining forces, attack the invaders!',13942,1,0,0,'anub\'ar crusher SAY_SPECIAL');

-- -1 602 000 HALLS OF LIGHTNING
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1602000,'I am the greatest of my father\'s sons! Your end has come!',14149,1,0,0,'bjarngrim SAY_AGGRO'),
(-1602001,'So ends your curse!',14153,1,0,0,'bjarngrim SAY_SLAY_1'),
(-1602002,'Flesh... is... weak!',14154,1,0,0,'bjarngrim SAY_SLAY_2'),
(-1602003,'...',14155,1,0,0,'bjarngrim SAY_SLAY_3'),
(-1602004,'How can it be...? Flesh is not... stronger!',14156,1,0,0,'bjarngrim SAY_DEATH'),
(-1602005,'Defend yourself, for all the good it will do!',14151,1,0,0,'bjarngrim SAY_BATTLE_STANCE'),
(-1602006,'%s switches to Battle Stance!',0,3,0,0,'bjarngrim EMOTE_BATTLE_STANCE'),
(-1602007,'GRAAAAAH! Behold the fury of iron and steel!',14152,1,0,0,'bjarngrim SAY_BERSEKER_STANCE'),
(-1602008,'%s switches to Berserker Stance!',0,3,0,0,'bjarngrim EMOTE_BERSEKER_STANCE'),
(-1602009,'Give me your worst!',14150,1,0,0,'bjarngrim SAY_DEFENSIVE_STANCE'),
(-1602010,'%s switches to Defensive Stance!',0,3,0,0,'bjarngrim EMOTE_DEFENSIVE_STANCE'),

(-1602011,'You wish to confront the master? You must weather the storm!',14453,1,0,0,'ionar SAY_AGGRO'),
(-1602012,'Shocking ... I know!',14456,1,0,0,'ionar SAY_SLAY_1'),
(-1602013,'You atempt the unpossible.',14457,1,0,0,'ionar SAY_SLAY_2'),
(-1602014,'Your spark of light is ... extinguish.',14458,1,0,0,'ionar SAY_SLAY_3'),
(-1602015,'Master... you have guests.',14459,1,0,0,'ionar SAY_DEATH'),
(-1602016,'The slightest spark shall be your undoing.',14454,1,0,0,'ionar SAY_SPLIT_1'),
(-1602017,'No one is safe!',14455,1,0,0,'ionar SAY_SPLIT_2'),

(-1602018,'What hope is there for you? None!',14162,1,0,0,'loken SAY_AGGRO0'),
(-1602019,'I have witnessed the rise and fall of empires. The birth and extinction of entire species. Over countless millennia the foolishness of mortals has remained beyond a constant. Your presence here confirms this.',14160,1,0,0,'loken SAY_INTRO_1'),
(-1602020,'My master has shown me the future, and you have no place in it. Azeroth will be reborn in darkness. Yogg-Saron shall be released! The Pantheon shall fall!',14161,1,0,0,'loken SAY_INTRO_2'),
(-1602021,'Only mortal...',14166,1,0,0,'loken SAY_SLAY_1'),
(-1602022,'I... am... FOREVER!',14167,1,0,0,'loken SAY_SLAY_2'),
(-1602023,'What little time you had, you wasted!',14168,1,0,0,'loken SAY_SLAY_3'),
(-1602024,'My death... heralds the end of this world.',14172,1,0,0,'loken SAY_DEATH'),
(-1602025,'You cannot hide from fate!',14163,1,0,0,'loken SAY_NOVA_1'),
(-1602026,'Come closer. I will make it quick.',14164,1,0,0,'loken SAY_NOVA_2'),
(-1602027,'Your flesh cannot hold out for long.',14165,1,0,0,'loken SAY_NOVA_3'),
(-1602028,'You stare blindly into the abyss!',14169,1,0,0,'loken SAY_75HEALTH'),
(-1602029,'Your ignorance is profound. Can you not see where this path leads?',14170,1,0,0,'loken SAY_50HEALTH'),
(-1602030,'You cross the precipice of oblivion!',14171,1,0,0,'loken SAY_25HEALTH'),
(-1602031,'%s begins to cast Lightning Nova!',0,3,0,0,'loken EMOTE_NOVA'),

(-1602032,'It is you who have destroyed my children? You... shall... pay!',13960,1,0,0,'volkhan SAY_AGGRO'),
(-1602033,'The armies of iron will conquer all!',13965, 1,0,0,'volkhan SAY_SLAY_1'),
(-1602034,'Ha, pathetic!',13966,1,0,0,'volkhan SAY_SLAY_2'),
(-1602035,'You have cost me too much work!',13967,1,0,0,'volkhan SAY_SLAY_3'),
(-1602036,'The master was right... to be concerned.',13968,1,0,0,'volkhan SAY_DEATH'),
(-1602037,'I will crush you beneath my boots!',13963,1,0,0,'volkhan SAY_STOMP_1'),
(-1602038,'All my work... undone!',13964,1,0,0,'volkhan SAY_STOMP_2'),
(-1602039,'Life from the lifelessness... death for you.',13961,1,0,0,'volkhan SAY_FORGE_1'),
(-1602040,'Nothing is wasted in the process. You will see....',13962,1,0,0,'volkhan SAY_FORGE_2'),
(-1602041,'%s runs to his anvil!',0,3,0,0,'volkhan EMOTE_TO_ANVIL'),
(-1602042,'%s prepares to shatter his Brittle Golems!',0,3,0,0,'volkhan EMOTE_SHATTER');

-- -1 603 000 ULDUAR
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1603000,'The Conservatory must be protected!',15526,1,0,0,'freya SAY_AGGRO'),
(-1603001,'Elders, grant me your strength!',15527,1,0,0,'freya SAY_AGGRO_HARD'),
(-1603002,'Eonar, your servant requires aid!',15528,1,0,0,'freya SAY_ADDS_CONSERVATOR'),
(-1603003,'Children, assist me!',15533,1,0,0,'freya SAY_ADDS_TRIO'),
(-1603004,'The swarm of the elements shall overtake you!',15534,1,0,0,'freya SAY_ADDS_LASHER'),
(-1603005,'Forgive me.',15529,1,0,0,'freya SAY_SLAY_1'),
(-1603006,'From your death springs life anew!',15530,1,0,0,'freya SAY_SLAY_2'),
(-1603007,'His hold on me dissipates. I can see clearly once more. Thank you, heroes.',15531,1,0,0,'freya SAY_DEATH'),
(-1603008,'You have strayed too far, wasted too much time!',15532,1,0,0,'freya SAY_BERSERK'),
(-1603009,'Eonar, your servant calls for your blessing!',15535,1,0,0,'freya SAY_HELP_YOGG'),

(-1603010,'Allies of Nature have appeared!',0,3,0,0,'freya EMOTE_ALLIES_NATURE'),
(-1603011,'The %s withers into the earth and begins to regenerate.',0,2,0,0,'freya EMOTE_REGEN_ALLIES'),

(-1603012,'As you wish, $N.',0,0,0,0,'keeper SAY_KEEPER_ACTIVE'),
(-1603013,'REUSE ME',0,0,0,0,'REUSE ME'),

(-1603014,'Matron, the Conservatory has been breached!',15483,1,0,0,'brightleaf SAY_AGGRO_BRIGHT'),
(-1603015,'Fertilizer.',15485,1,0,0,'brightleaf SAY_SLAY_1_BRIGHT'),
(-1603016,'Your corpse will nourish the soil!',15486,1,0,0,'brightleaf SAY_SLAY_2_BRIGHT'),
(-1603017,'Matron, one has fallen!',15487,1,0,0,'brightleaf SAY_DEATH_BRIGHT'),

(-1603018,'Mortals have no place here!',15493,1,0,0,'ironbranch SAY_AGGRO_IRON'),
(-1603019,'I return you whence you came!',15494,1,0,0,'ironbranch SAY_SLAY_1_IRON'),
(-1603020,'BEGONE!',15495,1,0,0,'ironbranch SAY_SLAY_2_IRON'),
(-1603021,'Freya! They come for you.',15496,1,0,0,'ironbranch SAY_DEATH_IRON'),

(-1603022,'This place will serve as your graveyard.',15500,1,0,0,'stonebark SAY_AGGRO_STONE'),
(-1603023,'<Angry roar>',15501,1,0,0,'stonebark SAY_SLAY_1_STONE'),
(-1603024,'Such a waste.',15502,1,0,0,'stonebark SAY_SLAY_2_STONE'),
(-1603025,'Matron, flee! They are ruthless....',15503,1,0,0,'stonebark SAY_DEATH_STONE'),

(-1603026,'Insolent whelps! Your blood will temper the weapons used to reclaim this world!',15564,1,0,0,'ignis SAY_AGGRO'),
(-1603027,'Let the inferno consume you!',15567,1,0,0,'ignis SAY_SCORCH_1'),
(-1603028,'BURN! Burn in the makers fire!',15568,1,0,0,'ignis SAY_SCORCH_2'),
(-1603029,'I will burn away your impurities!',15566,1,0,0,'ignis SAY_SLAGPOT'),
(-1603030,'Arise, soldiers of the Iron Crucible! The Makers\' will be done!',15565,1,0,0,'ignis SAY_ADDS'),
(-1603031,'More scraps for the scrapheap!',15569,1,0,0,'ignis SAY_SLAY_1'),
(-1603032,'Your bones will serve as kindling!',15570,1,0,0,'ignis SAY_SLAY_2'),
(-1603033,'Let it be finished!',15571,1,0,0,'ignis SAY_BERSERK'),
(-1603034,'I. Have. Failed.',15572,1,0,0,'ignis SAY_DEATH'),
(-1603035,'Ignis the Furnace Master begins to cast Flame Jets!',0,3,0,0,'ignis EMOTE_FLAME_JETS'),

(-1603036,'Welcome, champions! All of our attempts at grounding her have failed. We could use a hand in bring her down with these harpoon guns.',15647,0,0,0,'razorscale SAY_INTRO_WELCOME'),
(-1603037,'Give us a moment to prepare to build the turrets.',0,1,0,0,'razorscale SAY_INTRO_1'),
(-1603038,'Be on the lookout! Mole machines will be surfacing soon with those nasty Iron dwarves aboard!',0,1,0,0,'razorscale SAY_INTRO_2'),
(-1603039,'Ready to move out, keep those dwarves off of our backs!',0,1,0,0,'razorscale SAY_INTRO_3'),
(-1603040,'Move quickly! She won\'t remain grounded for long!',15648,1,0,0,'razorscale SAY_GROUNDED'),
(-1603041,'Razorscale takes a deep breath...',0,3,0,0,'razorscale EMOTE_BREATH'),
(-1603042,'Fires out! Let\'s rebuild those turrets!',0,1,0,0,'razorscale SAY_EXTINGUISH_FIRE'),
(-1603043,'Harpoon Turret is ready for use!',0,3,0,0,'razorscale EMOTE_HARPOON_READY'),
(-1603044,'Razorscale grounded permanently!',0,3,0,0,'razorscale EMOTE_GROUNDED'),

(-1603045,'New toys? For me? I promise I won\'t break them this time!',15724,1,0,0,'xt-002 SAY_AGGRO'),
(-1603046,'I... I think I broke it.',15728,1,0,0,'xt-002 SAY_SLAY_1'),
(-1603047,'I guess it doesn\'t bend that way.',15729,1,0,0,'xt-002 SAY_SLAY_2'),
(-1603048,'I\'m tired of these toys. I don\'t want to play anymore!',15730,1,0,0,'xt-002 SAY_BERSERK'),
(-1603049,'Time for a new game! My old toys will fight my new toys!',15732,1,0,0,'xt-002 SAY_ADDS'),
(-1603050,'You are bad... Toys... Very... Baaaaad!',15731,1,0,0,'xt-002 SAY_DEATH'),
(-1603051,'So tired. I will rest for just a moment!',15725,1,0,0,'xt-002 SAY_HEART_OPEN'),
(-1603052,'I\'m ready to play!',15726,1,0,0,'xt-002 SAY_HEART_CLOSE'),
(-1603053,'NO! NO! NO! NO! NO!',15727,1,0,0,'xt-002 SAY_TANCTRUM'),
(-1603054,'%s\'s heart is exposed and leaking energy.',0,3,0,0,'xt-002 EMOTE_EXPOSE_HEART'),
(-1603055,'%s consumes a scrapbot to repair himself!',0,3,0,0,'xt-002 EMOTE_REPAIR'),

(-1603056,'Whether the world\'s greatest gnats or the world\'s greatest heroes, you\'re still only mortal.',15684,1,0,0,'brundir SAY_BRUNDIR_AGGRO'),
(-1603057,'Stand still and stare into the light!',15687,1,0,0,'brundir SAY_BRUNDIR_WHIRL'),
(-1603058,'The power of the storm lives on...',15689,1,0,0,'brundir SAY_BRUNDIR_DEATH_1'),
(-1603059,'You rush headlong into the maw of madness!',15690,1,0,0,'brundir SAY_BRUNDIR_DEATH_2'),
(-1603060,'A merciful kill!',15685,1,0,0,'brundir SAY_BRUNDIR_SLAY_1'),
(-1603061,'HAH!',15686,1,0,0,'brundir SAY_BRUNDIR_SLAY_2'),
(-1603062,'This meeting of the Assembly of Iron is adjourned!',15691,1,0,0,'brundir SAY_BRUNDIR_BERSERK'),
(-1603063,'Let the storm clouds rise and rain down death from above!',15688,1,0,0,'brundir SAY_BRUNDIR_FLY'),

(-1603064,'Nothing short of total decimation will suffice!',15657,1,0,0,'molgeim SAY_MOLGEIM_AGGRO'),
(-1603065,'The legacy of storms shall not be undone...',15662,1,0,0,'molgeim SAY_MOLGEIM_DEATH_1'),
(-1603066,'What have you gained from my defeat? You are no less doomed, mortals...',15663,1,0,0,'molgeim SAY_MOLGEIM_DEATH_2'),
(-1603067,'Decipher this!',15660,1,0,0,'molgeim SAY_MOLGEIM_DEATH_RUNE'),
(-1603068,'Face the lightning surge!',15661,1,0,0,'molgeim SAY_MOLGEIM_SURGE'),
(-1603069,'The world on suffers yet another insignificant loss!',15658,1,0,0,'molgeim SAY_MOLGEIM_SLAY_1'),
(-1603070,'Death is the price of your arrogance.',15659,1,0,0,'molgeim SAY_MOLGEIM_SLAY_2'),
(-1603071,'This meeting of the Assembly of Iron is adjourned!',15664,1,0,0,'molgeim SAY_MOLGEIM_BERSERK'),

(-1603072,'You will not defeat the Assembly of Iron so easily, invaders!',15674,1,0,0,'steelbreaker SAY_STEEL_AGGRO'),
(-1603073,'My death only serves to hasten your demise.',15678,1,0,0,'steelbreaker SAY_STEEL_DEATH_1'),
(-1603074,'Impossible!',15679,1,0,0,'steelbreaker SAY_STEEL_DEATH_2'),
(-1603075,'So fragile and weak!',15675,1,0,0,'steelbreaker SAY_STEEL_SLAY_1'),
(-1603076,'Flesh... such a hindrance.',15676,1,0,0,'steelbreaker SAY_STEEL_SLAY_2'),
(-1603077,'You seek the secrets of Ulduar? Then take them!',15677,1,0,0,'steelbreaker SAY_STEEL_OVERWHELM'),
(-1603078,'This meeting of the Assembly of Iron is adjourned!',15680,1,0,0,'steelbreaker SAY_STEEL_BERSERK'),

(-1603079,'Some things are better left alone!',15473,1,0,0,'auriaya SAY_AGGRO'),
(-1603080,'There is no escape!',15475,1,0,0,'auriaya SAY_SLAY_1'),
(-1603081,'The secret dies with you!',15474,1,0,0,'auriaya SAY_SLAY_2'),
(-1603082,'You waste my time!',15477,1,0,0,'auriaya SAY_BERSERK'),
(-1603083,'Auriaya screams in agony.',15476,2,0,0,'auriaya SAY_DEATH'),
(-1603084,'Auriaya begins to cast Terrifying Screech.',0,3,0,0,'auriaya EMOTE_SCREECH'),
(-1603085,'Auriaya begins to activate the Feral Defender!',0,3,0,0,'auriaya EMOTE_DEFENDER'),

(-1603086,'You will suffer for this trespass!',15552,1,0,0,'hodir SAY_AGGRO'),
(-1603087,'Tragic. To come so far, only to fail.',15553,1,0,0,'hodir SAY_SLAY_1'),
(-1603088,'Welcome to the endless winter.',15554,1,0,0,'hodir SAY_SLAY_2'),
(-1603089,'Winds of the north consume you!',15555,1,0,0,'hodir SAY_FLASH_FREEZE'),
(-1603090,'<Furious Roar>',15556,2,0,0,'hodir SAY_FROZEN_BLOWS'),
(-1603091,'I... I am released from his grasp... at last.',15557,1,0,0,'hodir SAY_DEATH'),
(-1603092,'Enough! This ends now!',15558,1,0,0,'hodir SAY_BERSERK'),
(-1603093,'The veil of winter will protect you, champions!',15559,1,0,0,'hodir SAY_HELP_YOGG'),
(-1603094,'Hodir begins to cast Flash Freeze!',0,3,0,0,'hodir EMOTE_FLASH_FREEZE'),
(-1603095,'Hodir gains Frozen Blows!',0,3,0,0,'hodir EMOTE_FROZEN_BLOWS'),

(-1603096,'Your destruction will herald a new age of suffering!',15542,1,0,0,'vezax SAY_AGGRO'),
(-1603097,'You thought to stand before the legions of death... and survive?',15543,1,0,0,'vezax SAY_SLAY_1'),
(-1603098,'Defiance... a flaw of mortality.',15544,1,0,0,'vezax SAY_SLAY_2'),
(-1603099,'The black blood of Yogg-Saron courses through me! I. AM. UNSTOPPABLE!',15545,1,0,0,'vezaz SAY_SURGE'),
(-1603100,'Oh, what horrors await....',15546,1,0,0,'vezax SAY_DEATH'),
(-1603101,'Your defeat was inevitable!',15547,1,0,0,'vezax SAY_ENRAGE'),
(-1603102,'Behold, now! Terror, absolute!',15548,1,0,0,'vezax SAY_HARD_MODE'),
(-1603103,'A cloud of saronite vapors coalesces nearby!',0,3,0,0,'vezax EMOTE_VAPOR'),
(-1603104,'General Vezax roars and surges with dark might!',0,3,0,0,'vezax EMOTE_SURGE'),
(-1603105,'The saronite vapors mass and swirl violently, merging into a monstrous form!',0,3,0,0,'vezax EMOTE_ANIMUS'),

(-1603106,'Translocation complete. Commencing planetary analysis of Azeroth.',15405,1,0,0,'algalon SAY_INTRO_1'),
(-1603107,'Stand back, mortals. I\'m not here to fight you.',15406,1,0,0,'algalon SAY_INTRO_2'),
(-1603108,'It is in the universe\'s best interest to re-originate this planet should my analysis find systemic corruption. Do not interfere.',15407,1,0,0,'algalon SAY_INTRO_3'),

(-1603109,'See your world through my eyes. A universe so vast as to be immeasurable. Incomprehensible even to your greatest minds.',15390,1,0,0,'algalon SAY_ENGAGE'),
(-1603110,'Your actions are illogical. All possible results for this encounter have been calculated. The pantheon will receive the observer\'s message regardless outcome.',15386,1,0,0,'algalon SAY_AGGRO'),
(-1603111,'Loss of life, unavoidable.',15387,1,0,0,'algalon SAY_SLAY_1'),
(-1603112,'I do what I must.',15388,1,0,0,'algalon SAY_SLAY_2'),
(-1603113,'The stars come to my aid.',15392,1,0,0,'algalon SAY_SUMMON_STAR'),
(-1603114,'Witness the fury of cosmos!',15396,1,0,0,'algalon SAY_BIG_BANG_1'),
(-1603115,'Behold the tools of creation!',15397,1,0,0,'algalon SAY_BIG_BANG_2'),
(-1603116,'Beware!',15391,1,0,0,'algalon SAY_PHASE_2'),
(-1603117,'You are... out of time.',15394,1,0,0,'algalon SAY_BERSERK'),

(-1603118,'Analysis complete. There is partial corruption in the plane\'s life-support systems as well as complete corruption in most of the planet\'s defense mechanisms.',15398,1,0,0,'algalon SAY_DESPAWN_1'),
(-1603119,'Begin uplink: Reply Code: \'Omega\'. Planetary re-origination requested.',15399,1,0,0,'algalon SAY_DESPAWN_2'),
(-1603120,'Farewell, mortals. Your bravery is admirable, for such flawed creatures.',15400,1,0,0,'algalon SAY_DESPAWN_3'),

(-1603121,'I have seen worlds bathed in the Makers\' flames, their denizens fading without as much as a whimper. Entire planetary systems born and razed in the time that it takes your mortal hearts to beat once. Yet all throughout, my own heart devoid of emotion... of empathy. I. Have. Felt. Nothing. A million-million lives wasted. Had they all held within them your tenacity? Had they all loved life as you do?',15393,1,0,0,'algalon SAY_OUTRO_1'),
(-1603122,'Perhaps it is your imperfections... that which grants you free will... that allows you to persevere against all cosmically calculated odds. You prevail where the Titan\'s own perfect creations have failed.',15401,1,0,0,'algalon SAY_OUTRO_2'),
(-1603123,'I\'ve rearranged the reply code - your planet will be spared. I cannot be certain of my own calculations anymore.',15402,1,0,0,'algalon SAY_OUTRO_3'),
(-1603124,'I lack the strength to transmit the signal. You must... hurry... find a place of power... close to the skies.',15403,1,0,0,'algalon SAY_OUTRO_4'),
(-1603125,'Do not worry about my fate, Bronzen. If the signal is not transmitted in time, re-origination will proceed regardless. Save... your world...',15404,1,0,0,'algalon SAY_OUTRO_5'),

(-1603126,'None shall pass!',15586,1,0,0,'kologarn SAY_AGGRO'),
(-1603127,'OBLIVION!',15591,1,0,0,'kologarn SAY_SHOCKWAVE'),
(-1603128,'I will squeeze the life from you!',15592,1,0,0,'kologarn SAY_GRAB'),
(-1603129,'Just a scratch!',15589,1,0,0,'kologarn SAY_ARM_LOST_LEFT'),
(-1603130,'Only a flesh wound!',15590,1,0,0,'kologarn SAY_ARM_LOST_RIGHT'),
(-1603131,'KOL-THARISH!',15587,1,0,0,'kologarn SAY_SLAY_1'),
(-1603132,'YOU FAIL!',15588,1,0,0,'kologarn SAY_SLAY_2'),
(-1603133,'I am invincible!',15594,1,0,0,'kologarn SAY_BERSERK'),
(-1603134,'Master, they come...',15593,1,0,0,'kologarn SAY_DEATH'),
(-1603135,'The Right Arm has regrown!',0,3,0,0,'kologarn EMOTE_ARM_RIGHT'),
(-1603136,'The Left Arm has regrown!',0,3,0,0,'kologarn EMOTE_ARM_LEFT'),
(-1603137,'Kologarn casts Stone Grip!',0,3,0,0,'kologarn EMOTE_STONE_GRIP'),

(-1603138,'Interlopers! You mortals who dare to interfere with my sport will pay... Wait--you...',15733,1,0,0,'thorim SAY_AGGRO_1'),
(-1603139,'I remember you... In the mountains... But you... what is this? Where am--',15734,1,0,0,'thorim SAY_AGGRO_2'),

(-1603140,'Behold the power of the storms and despair!',15735,1,0,0,'thorim SAY_SPECIAL_1'),
(-1603141,'Do not hold back! Destroy them!',15736,1,0,0,'thorim SAY_SPECIAL_2'),
(-1603142,'Have you begun to regret your intrusion?',15737,1,0,0,'thorim SAY_SPECIAL_3'),

(-1603143,'Impertinent whelps! You dare challenge me atop my pedestal! I will crush you myself!',15738,1,0,0,'thorim SAY_JUMP'),
(-1603144,'Can\'t you at least put up a fight!?',15739,1,0,0,'thorim SAY_SLAY_1'),
(-1603145,'Pathetic!',15740,1,0,0,'thorim SAY_SLAY_2'),
(-1603146,'My patience has reached its limit!',15741,1,0,0,'thorim SAY_BERSERK'),

(-1603147,'Failures! Weaklings!',15742,1,0,0,'thorim SAY_ARENA_WIPE'),
(-1603148,'Stay your arms! I yield!',15743,1,0,0,'thorim SAY_DEFEATED'),

(-1603149,'I feel as though I am awakening from a nightmare, but the shadows in this place yet linger.',15744,1,0,0,'thorim SAY_OUTRO_1'),
(-1603150,'Sif... was Sif here? Impossible--she died by my brother\'s hand. A dark nightmare indeed....',15745,1,0,0,'thorim SAY_OUTRO_2'),
(-1603151,'I need time to reflect.... I will aid your cause if you should require it. I owe you at least that much. Farewell.',15746,1,0,0,'thorim SAY_OUTRO_3'),

(-1603152,'You! Fiend! You are not my beloved! Be gone!',15747,1,0,0,'thorim SAY_OUTRO_HARD_1'),
(-1603153,'Behold the hand behind all the evil that has befallen Ulduar! Left my kingdom in ruins, corrupted my brother and slain my wife!',15748,1,0,0,'thorim SAY_OUTRO_HARD_2'),
(-1603154,'And now it falls to you, champions, to avenge us all! The task before you is great, but I will lend you my aid as I am able. You must prevail!',15749,1,0,0,'thorim SAY_OUTRO_HARD_3'),

(-1603155,'Golganneth, lend me your strengh! Grant my mortal allies the power of thunder!',15750,1,0,0,'thorim SAY_HELP_YOGG'),

(-1603156,'Thorim, my lord, why else would these invaders have come into your sanctum but to slay you? They must be stopped!',15668,1,0,0,'thorim SAY_SIF_BEGIN'),
(-1603157,'Impossible! Lord Thorim, I will bring your foes a frigid death!',15670,1,0,0,'thorim SAY_SIF_EVENT'),
(-1603158,'These pathetic mortals are harmless, beneath my station. Dispose of them!',15669,1,0,0,'thorim SAY_SIF_DESPAWN'),

(-1603159,'Hostile entities detected. Threat assessment protocol active. Primary target engaged. Time minus thirty seconds to re-evaluation.',15506,1,0,0,'leviathan SAY_AGGRO'),
(-1603160,'Threat assessment routine modified. Current target threat level: zero. Acquiring new target.',15521,1,0,0,'leviathan SAY_SLAY'),
(-1603161,'Total systems failure. Defense protocols breached. Leviathan Unit shutting down.',15520,1,0,0,'leviathan SAY_DEATH'),
(-1603162,'Threat re-evaluated. Target assessment complete. Changing course.',15507,1,0,0,'leviathan SAY_CHANGE_1'),
(-1603163,'Pursuit objective modified. Changing course.',15508,1,0,0,'leviathan SAY_CHANGE_2'),
(-1603164,'Hostile entity stratagem predicted. Rerouting battle function. Changing course.',15509,1,0,0,'leviathan SAY_CHANGE_3'),
(-1603165,'Unauthorized entity attempting circuit overload. Activating anti-personnel countermeasures.',15516,1,0,0,'leviathan SAY_PLAYER_RIDE'),
(-1603166,'System malfunction. Diverting power to support systems.',15517,1,0,0,'leviathan SAY_OVERLOAD_1'),
(-1603167,'Combat matrix overload. Powering do-o-o-own...',15518,1,0,0,'leviathan SAY_OVERLOAD_2'),
(-1603168,'System restart required. Deactivating weapon systems.',15519,1,0,0,'leviathan SAY_OVERLOAD_3'),
(-1603169,'Orbital countermeasures enabled.',15510,1,0,0,'leviathan SAY_HARD_MODE'),
(-1603170,'\'Hodir\'s Fury\' online. Acquiring target.',15512,1,0,0,'leviathan SAY_TOWER_FROST'),
(-1603171,'\'Mimiron\'s Inferno\' online. Acquiring target.',15513,1,0,0,'leviathan SAY_TOWER_FIRE'),
(-1603172,'\'Thorim\'s Hammer\' online. Acquiring target.',15515,1,0,0,'leviathan SAY_TOWER_ENERGY'),
(-1603173,'\'Freya\'s Ward\' online. Acquiring target.',15514,1,0,0,'leviathan SAY_TOWER_NATURE'),
(-1603174,'Alert! Static defense system failure. Orbital countermeasures disabled.',15511,1,0,0,'leviathan SAY_TOWER_DOWN'),
(-1603175,'%s pursues $N',0,3,0,0,'leviathan EMOTE_PURSUE'),

(-1603176,'Oh, my! I wasn\'t expecting company! The workshop is such a mess! How embarrassing!',15611,1,0,0,'mimiron SAY_AGGRO'),
(-1603177,'Now why would you go and do something like that? Didn\'t you see the sign that said \'DO NOT PUSH THIS BUTTON!\'? How will we finish testing with the self-destruct mechanism active?',15629,1,0,0,'mimiron SAY_HARD_MODE'),
(-1603178,'Oh, my! It would seem that we are out of time, my friends!',15628,1,0,0,'mimiron SAY_BERSERK'),

(-1603179,'We haven\'t much time, friends! You\'re going to help me test out my latest and greatest creation. Now, before you change your minds, remember, that you kind of owe it to me after the mess you made with the XT-002.',15612,1,0,0,'mimiron SAY_TANK_ACTIVE'),
(-1603180,'MEDIC!',15613,1,0,0,'mimiron SAY_TANK_SLAY_1'),
(-1603181,'I can fix that... or, maybe not! Sheesh, what a mess...',15614,1,0,0,'mimiron SAY_TANK_SLAY_2'),
(-1603182,'WONDERFUL! Positively marvelous results! Hull integrity at 98.9 percent! Barely a dent! Moving right along.',15615,1,0,0,'mimiron SAY_TANK_DEATH'),

(-1603183,'Behold the VX-001 Anti-personnel Assault Cannon! You might want to take cover.',15616,1,0,0,'mimiron SAY_TORSO_ACTIVE'),
(-1603184,'Fascinating. I think they call that a \'clean kill\'.',15617,1,0,0,'mimiron SAY_TORSO_SLAY_1'),
(-1603185,'Note to self: Cannon highly effective against flesh.',15618,1,0,0,'mimiron SAY_TORSO_SLAY_2'),
(-1603186,'Thank you, friends! Your efforts have yielded some fantastic data! Now, where did I put-- oh, there it is!',15619,1,0,0,'mimiron SAY_TORSO_DEATH'),

(-1603187,'Isn\'t it beautiful? I call it the magnificent aerial command unit!',15620,1,0,0,'mimiron SAY_HEAD_ACTIVE'),
(-1603188,'Outplayed!',15621,1,0,0,'mimiron SAY_HEAD_SLAY_1'),
(-1603189,'You can do better than that!',15622,1,0,0,'mimiron SAY_HEAD_SLAY_2'),
(-1603190,'Preliminary testing phase complete. Now comes the true test!!',15623,1,0,0,'mimiron SAY_HEAD_DEATH'),

(-1603191,'Gaze upon its magnificence! Bask in its glorious, um, glory! I present you... V-07-TR-0N!',15624,1,0,0,'mimiron SAY_ROBOT_ACTIVE'),
(-1603192,'Prognosis: Negative!',15625,1,0,0,'mimiron SAY_ROBOT_SLAY_1'),
(-1603193,'You\'re not going to get up from that one, friend.',15626,1,0,0,'mimiron SAY_ROBOT_SLAY_2'),
(-1603194,'It would appear that I\'ve made a slight miscalculation. I allowed my mind to be corrupted by the fiend in the prison, overriding my primary directive. All systems seem to be functional now. Clear.',15627,1,0,1,'mimiron SAY_ROBOT_DEATH'),

(-1603195,'Combat matrix enhanced. Behold wonderous rapidity!',15630,1,0,0,'mimiron SAY_HELP_YOGG'),
(-1603196,'%s begins to cast Plasma Blast!',0,3,0,0,'mimiron EMOTE_PLASMA_BLAST'),

(-1603197,'Aaaaaaaaaaaaaaaaa... Help me!!! Please got to help me!',15771,1,0,0,'yogg SAY_SARA_INTRO_1'),
(-1603198,'What do you want from me? Leave me alone!',15772,1,0,0,'yogg SAY_SARA_INTRO_2'),
(-1603199,'The time to strike at the head of the beast will soon be upon us! Focus your anger and hatred on his minions!',15775,1,0,0,'yogg SAY_SARA_AGGRO'),
(-1603201,'Yes! YES! Show them no mercy! Give no pause to your attacks!',15773,1,0,0,'yogg SAY_SARA_HELP_1'),
(-1603202,'Let hatred and rage guide your blows!',15774,1,0,0,'yogg SAY_SARA_HELP_2'),
(-1603203,'Could they have been saved?',15779,1,0,0,'yogg SAY_SARA_SLAY_1'),
(-1603204,'Powerless to act...',15778,1,0,0,'yogg SAY_SARA_SLAY_2'),

(-1603205,'Weak-minded fools!',15780,4,0,0,'yogg SAY_WIPE_PHASE_1'),

(-1603206,'I am the lucid dream.',15754,1,0,457,'yogg SAY_PHASE_2_INTRO_1'),
(-1603207,'Tremble, mortals, before the coming of the end!',15777,1,0,0,'yogg SAY_SARA_PHASE_2_INTRO_A'),
(-1603208,'Suffocate upon your own hate!',15776,1,0,0,'yogg SAY_SARA_PHASE_2_INTRO_B'),

(-1603209,'MADNESS WILL CONSUME YOU!',15756,1,0,0,'yogg SAY_MADNESS'),
(-1603210,'Look upon the true face of death and know that your end comes soon!',15755,1,0,0,'yogg SAY_PHASE_3'),
(-1603211,'%s prepares to unleash Empowering Shadows!',0,3,0,0,'yogg EMOTE_EMPOWERING_SHADOWS'),
(-1603212,'Eternal suffering awaits!',15758,1,0,0,'yogg SAY_SLAY_2'),
(-1603213,'Your fate is sealed. The end of days is finally upon you and ALL who inhabit this miserable little seedling. Uulwi ifis halahs gag erh\'ongg w\'ssh.',15761,1,0,0,'yogg SAY_DEATH'),
(-1603214,'Your will is no longer you own...',15759,4,0,0,'yogg SAY_TO_INSANE_1'),
(-1603215,'Destroy them minion, your master commands it!',15760,4,0,0,'yogg SAY_TO_INSANE_2'),

(-1603216,'Your resilience is admirable.',15598,0,0,0,'yogg SAY_LICH_KING_1'),
(-1603217,'Arrrrrrgh!',15470,1,0,0,'yogg SAY_CHAMPION_1'),
(-1603218,'I\'m not afraid of you!',15471,0,0,0,'yogg SAY_CHAMPION_2'),
(-1603219,'I will break you as I broke him.',15599,0,0,0,'yogg SAY_LICH_KING_2'),
(-1603220,'Yrr n\'lyeth... shuul anagg!',15766,0,0,0,'yogg SAY_YOGG_V3_1'),
(-1603221,'He will learn... no king rules forever; only death is eternal!',15767,0,0,0,'yogg SAY_YOGG_V3_2'),

(-1603222,'It is done... All have been given that which must be given. I now seal the Dragon Soul forever...',15631,0,0,1,'yogg SAY_NELTHARION_1'),
(-1603223,'That terrible glow... should that be?',15784,0,0,1,'yogg SAY_YSERA'),
(-1603224,'For it to be as it must, yes.',15632,0,0,1,'yogg SAY_NELTHARION_2'),
(-1603225,'It is a weapon like no other. It must be like no other.',15610,0,0,1,'yogg SAY_MALYGOS'),
(-1603226,'His brood learned their lesson before too long, you shall soon learn yours!',15765,0,0,0,'yogg SAY_YOGG_V2'),

(-1603227,'Bad news sire.',15538,0,0,1,'yogg SAY_GARONA_1'),
(-1603228,'Gul\'dan is bringing up his warlocks by nightfall. Until then, the Blackrock clan will be trying to take the Eastern Wall.',15540,0,0,1,'yogg SAY_GARONA_3'),
(-1603229,'A thousand deaths... ',15762,0,0,0,'yogg SAY_YOGG_V1_1'),
(-1603230,'or one murder.',15763,0,0,0,'yogg SAY_YOGG_V1_2'),
(-1603231,'We will hold until the reinforcements come. As long as men with stout hearts are manning the walls and throne Stormwind will hold.',15585,0,0,1,'yogg SAY_KING_LLANE'),
(-1603232,'The orc leaders agree with your assessment.',15541,0,0,0,'yogg SAY_GARONA_4'),
(-1603233,'Your petty quarrels only make me stronger!',15764,0,0,0,'yogg SAY_YOGG_V1_3'),

(-1603234,'Portals open into Yogg-Saron\'s mind!',0,3,0,0,'yogg EMOTE_VISION_BLAST'),
(-1603235,'The illusion shatters and a path to the central chamber opens!',0,3,0,0,'yogg EMOTE_SHATTER_BLAST'),

(-1603236,'%s\'s heart is severed from his body.',0,3,0,0,'xt-002 EMOTE_KILL_HEART'),
(-1603237,'%s begins to cause the earth to quake.',0,3,0,0,'xt-002 EMOTE_EARTH_QUAKE'),
(-1603238,'%s is extinguished by the water!',0,2,0,0,'ignis EMOTE_EXTINGUISH_SCORCH'),

(-1603239,'You\'ve done it! You\'ve broken the defenses of Ulduar. In a few moments, we will be dropping in to...',15804,0,0,0,'bronzebeard radio SAY_PRE_LEVIATHAN_1'),
(-1603240,'What is that? Be careful! Something\'s headed your way!',15805,0,0,0,'bronzebeard radio SAY_PRE_LEVIATHAN_2'),
(-1603241,'Quickly! Evasive action! Evasive act--',15806,0,0,0,'bronzebeard radio SAY_PRE_LEVIATHAN_3'),

(-1603242,'%s activates Hodir\'s Fury.',0,3,0,0,'leviathan EMOTE_HODIR_FURY'),
(-1603243,'%s activates Freya\'s Ward.',0,3,0,0,'leviathan EMOTE_FREYA_WARD'),
(-1603244,'%s activates Mimiron\'s Inferno.',0,3,0,0,'leviathan EMOTE_MIMIRON_INFERNO'),
(-1603245,'%s activates Thorim\'s Hammer.',0,3,0,0,'leviathan EMOTE_THORIM_HAMMER'),

(-1603246,'I know just the place. Will you be all right?',15823,1,0,0,'brann SAY_BRANN_OUTRO'),

(-1603247,'%s surrounds itself with a crackling Runic Barrier!',0,3,0,0,'thorim EMOTE_RUNIC_BARRIER'),

(-1603248,'Self-destruct sequence initiated.',15413,1,0,0,'mimiron SAY_SELF_DESTRUCT'),
(-1603249,'This area will self-destruct in ten minutes.',15415,1,0,0,'mimiron SAY_DESTRUCT_10_MIN'),
(-1603250,'This area will self-destruct in nine minutes.',15416,1,0,0,'mimiron SAY_DESTRUCT_9_MIN'),
(-1603251,'This area will self-destruct in eight minutes.',15417,1,0,0,'mimiron SAY_DESTRUCT_8_MIN'),
(-1603252,'This area will self-destruct in seven minutes.',15418,1,0,0,'mimiron SAY_DESTRUCT_7_MIN'),
(-1603253,'This area will self-destruct in six minutes.',15419,1,0,0,'mimiron SAY_DESTRUCT_6_MIN'),
(-1603254,'This area will self-destruct in five minutes.',15420,1,0,0,'mimiron SAY_DESTRUCT_5_MIN'),
(-1603255,'This area will self-destruct in four minutes.',15421,1,0,0,'mimiron SAY_DESTRUCT_4_MIN'),
(-1603256,'This area will self-destruct in three minutes.',15422,1,0,0,'mimiron SAY_DESTRUCT_3_MIN'),
(-1603257,'This area will self-destruct in two minutes.',15423,1,0,0,'mimiron SAY_DESTRUCT_2_MIN'),
(-1603258,'This area will self-destruct in one minute.',15424,1,0,0,'mimiron SAY_DESTRUCT_1_MIN'),
(-1603259,'Self-destruct sequence finalized. Have a nice day.',15425,1,0,0,'mimiron SAY_DESTRUCT_0_MIN'),
(-1603260,'Self-destruct sequence terminated. Overide code A905.',15414,1,0,0,'mimiron SAY_SELF_DESTRUCT_END'),

(-1603261,'%s begins to boil upon touching $n!',0,2,0,0,'ominous cloud EMOTE_CLOUD_BOIL'),
(-1603262,'The monster in your nightmares.',0,1,0,457,'yogg SAY_PHASE_2_INTRO_2'),
(-1603263,'The fiend of a thousand faces.',0,1,0,457,'yogg SAY_PHASE_2_INTRO_3'),
(-1603264,'Cower before my true form.',0,1,0,457,'yogg SAY_PHASE_2_INTRO_4'),
(-1603265,'BOW DOWN BEFORE THE GOD OF DEATH!',0,1,0,0,'yogg SAY_PHASE_2_INTRO_5'),
(-1603266,'%s opens his mouth wide!',0,3,0,0,'yogg EMOTE_DEAFENING_ROAR'),
(-1603267,'The clans are united under Blackhand in this assault. They will stand together until Stormwind has fallen.',15539,0,0,1,'yogg SAY_GARONA_2');

-- -1 604 000 GUNDRAK
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1604000,'Drakkari gonna kill anybody who trespass on these lands!',14443,1,0,0,'sladran SAY_AGGRO'),
(-1604001,'Minions of the scale, heed my call!',14444,1,0,0,'sladran SAY_SUMMON_SNAKE'),
(-1604002,'Our thousand fangs gonna rend your flesh! ',14445,1,0,0,'sladran SAY_SUMMON_CONSTRICTOR'),
(-1604003,'Ye not breathin\'! Good.',14446,1,0,0,'sladran SAY_SLAY_1'),
(-1604004,'You scared now?',14447,1,0,0,'sladran SAY_SLAY_2'),
(-1604005,'I\'ll eat you next, mon!',14448,1,0,0,'sladran SAY_SLAY_3'),
(-1604006,'I sssee now... Ssscourge wasss not... our greatessst enemy...',14449,1,0,0,'sladran SAY_DEATH'),
(-1604007,'%s begins to cast Poison Nova!',0,3,0,0,'sladran EMOTE_NOVA'),

(-1604008,'%s surges forward!',0,2,0,0,'colossus EMOTE_SURGE'),
(-1604009,'%s seep into the ground.',0,2,0,0,'colossus EMOTE_SEEP'),
(-1604010,'%s begins to glow faintly.',0,2,0,0,'colossus EMOTE_GLOW'),

(-1604011,'We fought back da Scourge. What chance joo be thinkin\' JOO got?',14721,1,0,0,'moorabi SAY_AGGRO'),
(-1604012,'Da ground gonna swallow you up! ',14723,1,0,0,'moorabi SAY_QUAKE'),
(-1604013,'Get ready for somethin\'... much... BIGGAH!',14722,1,0,0,'moorabi SAY_TRANSFORM'),
(-1604014,'I crush you, cockroaches!',14725,1,0,0,'moorabi SAY_SLAY_1'),
(-1604015,'Who gonna stop me; you?',14726,1,0,0,'moorabi SAY_SLAY_2'),
(-1604016,'Not so tough now.',14727,1,0,0,'moorabi SAY_SLAY_3'),
(-1604017,'If our gods can die... den so can we...',14728,1,0,0,'moorabi SAY_DEATH'),
(-1604018,'%s begins to transform!',0,3,0,0,'moorabi EMOTE_TRANSFORM'),

(-1604019,'I\'m gonna spill your guts, mon!',14430,1,0,0,'galdarah SAY_AGGRO'),
(-1604020,'Ain\'t gonna be nottin\' left after this!',14431,1,0,0,'galdarah SAY_TRANSFORM_1'),
(-1604021,'You wanna see power? I\'m gonna show you power!',14432,1,0,0,'galdarah SAY_TRANSFORM_2'),
(-1604022,'Gut them! Impale them!',14433,1,0,0,'galdarah SAY_SUMMON_1'),
(-1604023,'Kill them all!',14434,1,0,0,'galdarah SAY_SUMMON_2'),
(-1604024,'Say hello to my BIG friend!',14435,1,0,0,'galdarah SAY_SUMMON_3'),
(-1604025,'What a rush!',14436,1,0,0,'galdarah SAY_SLAY_1'),
(-1604026,'Who needs gods, when WE ARE GODS!',14437,1,0,0,'galdarah SAY_SLAY_2'),
(-1604027,'I told ya so!',14438,1,0,0,'galdarah SAY_SLAY_3'),
(-1604028,'Even the mighty... can fall.',14439,1,0,0,'galdarah SAY_DEATH'),

(-1604029,'%s transforms into a Mammoth!',14724,2,0,0,'moorabi EMOTE_TRANSFORMED'),
(-1604030,'$N is impaled!',0,3,0,0,'EMOTE_IMPALED');


-- -1 608 000 VIOLET HOLD
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1608000,'Prison guards, we are leaving! These adventurers are taking over! Go, go, go!',0,1,0,0,'sinclari SAY_BEGIN'),
(-1608001,'I\'m locking the door. Good luck, and thank you for doing this.',0,0,0,1,'sinclari SAY_LOCK_DOOR'),

(-1608002,'Adventurers, the door is beinning to weaken!',0,1,0,0,'sinclair SAY_SEAL_75'),
(-1608003,'Only half of the door seal\'s strength remains! You must fight on!',0,1,0,0,'sinclair SAY_SEAL_50'),
(-1608004,'The door seal is about to collapse! All is lost if the Blue Dragonflight breaks through the door!',0,1,0,0,'sinclair SAY_SEAL_5'),

(-1608005,'A Portal Guardian defends the new portal!',0,3,0,0,'EMOTE_GUARDIAN_PORTAL'),
(-1608006,'An elite Blue Dragonflight squad appears from the portal!',0,3,0,0,'EMOTE_DRAGONFLIGHT_PORTAL'),
(-1608007,'A Guardian Keeper emerges from the portal!',0,3,0,0,'EMOTE_KEEPER_PORTAL'),

(-1608008,'Free to--mm--fly now. Ra-aak... Not find us--ekh-ekh! Escape!',14218,1,0,0,'erekem SAY_RELEASE_EREKEM'),
(-1608009,'I... am fury... unrestrained!',14229,1,0,0,'ichoron SAY_RELEASE_ICHORON'),
(-1608010,'Back in business! Now to execute an exit strategy.',14498,1,0,0,'xevozz SAY_RELEASE_XEVOZZ'),
(-1608011,'I am... renewed.',13995,1,0,0,'zuramat SAY_RELEASE_ZURAMAT'),

(-1608012,'Not--caww--get in way of--rrak-rrak--flee!',14219,1,0,0,'erekem SAY_AGGRO'),
(-1608013,'My---raaak--favorite! Awk awk awk! Raa-kaa!',14220,1,0,0,'erekem SAY_ADD_DIE_1'),
(-1608014,'Nasty little...A-ak,kaw! Kill! Yes,kill you!',14221,1,0,0,'erekem SAY_ADD_DIE_2'),
(-1608018,'No--kaw,kaw--flee...',14225,1,0,0,'erekem SAY_DEATH'),

(-1608019,'Stand aside, mortals!',14230,1,0,0,'ichoron SAY_AGGRO'),
(-1608020,'I will not be contained! Ngyah!!',14233,1,0,0,'ichoron SAY_SHATTERING'),
(-1608021,'Water can hold any form, take any shape... overcome any obstacle.',14232,1,0,0,'ichoron SAY_SHIELD'),
(-1608022,'I am a force of nature!',14234,1,0,0,'ichoron SAY_SLAY_1'),
(-1608023,'I shall pass!',14235,1,0,0,'ichoron SAY_SLAY_2'),
(-1608024,'You can not stop the tide!',14236,1,0,0,'ichoron SAY_SLAY_3'),
(-1608025,'I shall consume,decimate, devastate,and destroy! Yield now to the wrath of the pounding sea!',14231,1,0,0,'ichoron SAY_ENRAGE'),
(-1608026,'I... recede.',14237,1,0,0,'ichoron SAY_DEATH'),

(-1608027,'You did it! You held the Blue Dragonflight back and defeated their commander. Amazing work!',0,0,0,1,'sinclari SAY_VICTORY'),

(-1608028,'%s\'s Protective Bubble shatters!',0,3,0,0,'ichoron EMOTE_BUBBLE');

-- -1 609 000 EBON HOLD (DK START)
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1609000,'You have made a grave error, fiend!',0,0,0,0,'unworthy SAY_START_1'),
(-1609001,'I was a soldier of the Light once... Look at what I have become... ',0,0,0,0,'unworthy SAY_START_2'),
(-1609002,'You are hopelessly outmatched, $R.',0,0,0,0,'unworthy SAY_START_3'),
(-1609003,'They brand me unworthy? I will show them unmorthy!',0,0,0,0,'unworthy SAY_START_4'),
(-1609004,'You will allow me a weapon and armor, yes?',0,0,0,0,'unworthy SAY_START_5'),
(-1609005,'I will win my freedom and leave this cursed place!',0,0,0,0,'unworthy SAY_START_6'),
(-1609006,'I will dismantle this festering hellhole!',0,0,0,0,'unworthy SAY_START_7'),
(-1609007,'There can be only one survivor!',0,0,0,0,'unworthy SAY_START_8'),

(-1609008,'Let your fears consume you!',0,0,0,0,'unworthy SAY_AGGRO_1'),
(-1609009,'HAH! You can barely hold a blade! Yours will be a quick death.',0,0,0,0,'unworthy SAY_AGGRO_2'),
(-1609010,'And now you die',0,0,0,0,'unworthy SAY_AGGRO_3'),
(-1609011,'To battle!',0,0,0,0,'unworthy SAY_AGGRO_4'),
(-1609012,'There is no hope for our future...',0,0,0,0,'unworthy SAY_AGGRO_5'),
(-1609013,'Sate your hunger on cold steel, $R',0,0,0,0,'unworthy SAY_AGGRO_6'),
(-1609014,'It ends here!',0,0,0,0,'unworthy SAY_AGGRO_7'),
(-1609015,'Death is the only cure!',0,0,0,0,'unworthy SAY_AGGRO_8'),

(-1609016,'No potions!',0,0,0,0,'dk_initiate SAY_DUEL_A'),
(-1609017,'Remember this day, $n, for it is the day that you will be thoroughly owned.',0,0,0,0,'dk_initiate SAY_DUEL_B'),
(-1609018,'I\'m going to tear your heart out, cupcake!',0,0,0,0,'dk_initiate SAY_DUEL_C'),
(-1609019,'Don\'t make me laugh.',0,0,0,0,'dk_initiate SAY_DUEL_D'),
(-1609020,'Here come the tears...',0,0,0,0,'dk_initiate SAY_DUEL_E'),
(-1609021,'You have challenged death itself!',0,0,0,0,'dk_initiate SAY_DUEL_F'),
(-1609022,'The Lich King will see his true champion on this day!',0,0,0,0,'dk_initiate SAY_DUEL_G'),
(-1609023,'You\'re going down!',0,0,0,0,'dk_initiate SAY_DUEL_H'),
(-1609024,'You don\'t stand a chance, $n',0,0,0,0,'dk_initiate SAY_DUEL_I'),

(-1609025,'Come to finish the job, have you?',0,0,0,1,'special_surprise SAY_EXEC_START_1'),
(-1609026,'Come to finish the job, have ye?',0,0,0,1,'special_surprise SAY_EXEC_START_2'),
(-1609027,'Come ta finish da job, mon?',0,0,0,1,'special_surprise SAY_EXEC_START_3'),

(-1609028,'You\'ll look me in the eyes when...',0,0,0,25,'special_surprise SAY_EXEC_PROG_1'),
(-1609029,'Well this son o\' Ironforge would like...',0,0,0,25,'special_surprise SAY_EXEC_PROG_2'),
(-1609030,'Ironic, isn\'t it? To be killed...',0,0,0,25,'special_surprise SAY_EXEC_PROG_3'),
(-1609031,'If you\'d allow me just one...',0,0,0,25,'special_surprise SAY_EXEC_PROG_4'),
(-1609032,'I\'d like to stand for...',0,0,0,25,'special_surprise SAY_EXEC_PROG_5'),
(-1609033,'I want to die like an orc...',0,0,0,25,'special_surprise SAY_EXEC_PROG_6'),
(-1609034,'Dis troll gonna stand for da...',0,0,0,25,'special_surprise SAY_EXEC_PROG_7'),

(-1609035,'$N?',0,0,0,1,'special_surprise SAY_EXEC_NAME_1'),
(-1609036,'$N? Mon?',0,0,0,1,'special_surprise SAY_EXEC_NAME_2'),

(-1609037,'$N, I\'d recognize that face anywhere... What... What have they done to you, $N?',0,0,0,1,'special_surprise SAY_EXEC_RECOG_1'),
(-1609038,'$N, I\'d recognize those face tentacles anywhere... What... What have they done to you, $N?',0,0,0,1,'special_surprise SAY_EXEC_RECOG_2'),
(-1609039,'$N, I\'d recognize that face anywhere... What... What have they done to ye, $Glad:lass;?',0,0,0,1,'special_surprise SAY_EXEC_RECOG_3'),
(-1609040,'$N, I\'d recognize that decay anywhere... What... What have they done to you, $N?',0,0,0,1,'special_surprise SAY_EXEC_RECOG_4'),
(-1609041,'$N, I\'d recognize those horns anywhere... What have they done to you, $N?',0,0,0,1,'special_surprise SAY_EXEC_RECOG_5'),
(-1609042,'$N, I\'d recognize dem tusks anywhere... What... What have dey done ta you, mon?',0,0,0,1,'special_surprise SAY_EXEC_RECOG_6'),

(-1609043,'You don\'t remember me, do you? Blasted Scourge... They\'ve tried to drain you of everything that made you a righteous force of reckoning. Every last ounce of good... Everything that made you a draenei!',0,0,0,1,'special_surprise SAY_EXEC_NOREM_1'),
(-1609044,'Ye don\'t remember me, do ye? Blasted Scourge... They\'ve tried to drain ye o\' everything that made ye a righteous force o\' reckoning. Every last ounce o\' good... Everything that made you a $Gson:daughter; of Ironforge!',0,0,0,1,'special_surprise SAY_EXEC_NOREM_2'),
(-1609045,'You don\'t remember me, do you? We were humans once - long, long ago - until Lordaeron fell to the Scourge. Your transformation to a Scourge zombie came shortly after my own. Not long after that, our minds were freed by the Dark Lady.',0,0,0,1,'special_surprise SAY_EXEC_NOREM_3'),
(-1609046,'You don\'t remember me, do you? Blasted Scourge... They\'ve tried to drain you of everything that made you a pint-sized force of reckoning. Every last ounce of good... Everything that made you a gnome!',0,0,0,1,'special_surprise SAY_EXEC_NOREM_4'),
(-1609047,'You don\'t remember me, do you? Blasted Scourge...They\'ve tried to drain of everything that made you a righteous force of reckoning. Every last ounce of good...Everything that made you a human!',0,0,0,1,'special_surprise SAY_EXEC_NOREM_5'),
(-1609048,'You don\'t remember me? When you were a child your mother would leave you in my care while she served at the Temple of the Moon. I held you in my arms and fed you with honey and sheep\'s milk to calm you until she would return. You were my little angel. Blasted Scourge... What have they done to you, $N?',0,0,0,1,'special_surprise SAY_EXEC_NOREM_6'),
(-1609049,'You don\'t recognize me, do you? Blasted Scourge... They\'ve tried to drain you of everything that made you a righteous force of reckoning. Every last ounce of good... Everything that made you an orc!',0,0,0,1,'special_surprise SAY_EXEC_NOREM_7'),
(-1609050,'You don\'t remember me, do you? Blasted Scourge... They\'ve tried to drain you of everything that made you a righteous force of reckoning. Every last ounce of good... Everything that made you a tauren!',0,0,0,1,'special_surprise SAY_EXEC_NOREM_8'),
(-1609051,'You don\'t remember me, mon? Damn da Scourge! Dey gone ta drain you of everytin dat made ya a mojo masta. Every last ounce of good... Everytin\' dat made ya a troll hero, mon!',0,0,0,1,'special_surprise SAY_EXEC_NOREM_9'),

(-1609052,'A pact was made, $Gbrother:sister;! We vowed vengeance against the Lich King! For what he had done to us! We battled the Scourge as Forsaken, pushing them back into the plaguelands and freeing Tirisfal! You and I were champions of the Forsaken!',0,0,0,1,'special_surprise SAY_EXEC_THINK_1'),
(-1609053,'You must remember the splendor of life, $Gbrother:sister;. You were a champion of the Kaldorei once! This isn\'t you!',0,0,0,1,'special_surprise SAY_EXEC_THINK_2'),
(-1609054,'Think, $N. Think back. Try and remember the majestic halls of Silvermoon City, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the sin\'dorei once! This isn\'t you.',0,0,0,6,'special_surprise SAY_EXEC_THINK_3'),
(-1609055,'Think, $N. Think back. Try and remember the proud mountains of Argus, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the draenei once! This isn\'t you.',0,0,0,6,'special_surprise SAY_EXEC_THINK_4'),
(-1609056,'Think, $N. Think back. Try and remember the snow capped mountains o\' Dun Morogh! Ye were born there, $Glad:lass;. Remember the splendor o\' life, $N! Ye were a champion o\' the dwarves once! This isn\'t ye!',0,0,0,6,'special_surprise SAY_EXEC_THINK_5'),
(-1609057,'Think, $N. Think back. Try and remember Gnomeregan before those damned troggs! Remember the feel of an [arclight spanner] $Gbrother:sister;. You were a champion of gnome-kind once! This isn\'t you.',0,0,0,6,'special_surprise SAY_EXEC_THINK_6'),
(-1609058,'Think, $N. Think back. Try and remember the hills and valleys of Elwynn, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the Alliance once! This isn\'t you.',0,0,0,6,'special_surprise SAY_EXEC_THINK_7'),
(-1609059,'Think, $N. Think back. Try and remember Durotar, $Gbrother:sister;! Remember the sacrifices our heroes made so that we could be free of the blood curse. Harken back to the Valley of Trials, where we were reborn into a world without demonic influence. We found the splendor of life, $N. Together! This isn\'t you. You were a champion of the Horde once!',0,0,0,6,'special_surprise SAY_EXEC_THINK_8'),
(-1609060,'Think, $N. Think back. Try and remember the rolling plains of Mulgore, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the tauren once! This isn\'t you.',0,0,0,6,'special_surprise SAY_EXEC_THINK_9'),
(-1609061,'TINK $N. Tink back, mon! We be Darkspear, mon! Bruddas and sistas! Remember when we fought the Zalazane and done took he head and freed da Echo Isles? MON! TINK! You was a champion of da Darkspear trolls!',0,0,0,6,'special_surprise SAY_EXEC_THINK_10'),

(-1609062,'Listen to me, $N. You must fight against the Lich King\'s control. He is a monster that wants to see this world - our world - in ruin. Don\'t let him use you to accomplish his goals. You were once a hero and you can be again. Fight, damn you! Fight his control!',0,0,0,5,'special_surprise SAY_EXEC_LISTEN_1'),
(-1609063,'Listen to me, $N Ye must fight against the Lich King\'s control. He\'s a monster that wants to see this world - our world - in ruin. Don\'t let him use ye to accomplish his goals. Ye were once a hero and ye can be again. Fight, damn ye! Fight his control!',0,0,0,5,'special_surprise SAY_EXEC_LISTEN_2'),
(-1609064,'Listen to me, $N. You must fight against the Lich King\'s control. He is a monster that wants to see this world - our world - in ruin. Don\'t let him use you to accomplish his goals AGAIN. You were once a hero and you can be again. Fight, damn you! Fight his control!',0,0,0,5,'special_surprise SAY_EXEC_LISTEN_3'),
(-1609065,'Listen ta me, $Gbrudda:sista;. You must fight against da Lich King\'s control. He be a monstar dat want ta see dis world - our world - be ruined. Don\'t let he use you ta accomplish he goals. You be a hero once and you be a hero again! Fight it, mon! Fight he control!',0,0,0,5,'special_surprise SAY_EXEC_LISTEN_4'),

(-1609066,'What\'s going on in there? What\'s taking so long, $N?',0,1,0,0,'special_surprise SAY_PLAGUEFIST'),

(-1609067,'There... There\'s no more time for me. I\'m done for. Finish me off, $N. Do it or they\'ll kill us both. $N... Remember Silvermoon. This world is worth saving!',0,0,0,18,'special_surprise SAY_EXEC_TIME_1'),
(-1609068,'There... There\'s no more time for me. I\'m done for. Finish me off, $N. Do it or they\'ll kill us both. $N... Remember Argus. Don\'t let that happen to this world.',0,0,0,18,'special_surprise SAY_EXEC_TIME_2'),
(-1609069,'There... There\'s no more time for me. I\'m done for. Finish me off, $N. Do it or they\'ll kill us both $N... For KHAAAAAAAAZZZ MODAAAAAANNNNNN!!!',0,0,0,18,'special_surprise SAY_EXEC_TIME_3'),
(-1609070,'There... There\'s no more time for me. I\'m done for. Finish me off, $N. Do it or they\'ll kill us both. $N... Remember Tirisfal! This world is worth saving!',0,0,0,18,'special_surprise SAY_EXEC_TIME_4'),
(-1609071,'There... There\'s no more time for me. I\'m done for. Finish me off, $N. Do it or they\'ll kill us both. $N... Remember Gnomeregan! This world is worth saving.',0,0,0,18,'special_surprise SAY_EXEC_TIME_5'),
(-1609072,'There... There\'s no more time for me. I\'m done for. FInish me off, $N. Do it or they\'ll kill us both. $N...Remember Elwynn. This world is worth saving.',0,0,0,18,'special_surprise SAY_EXEC_TIME_6'),
(-1609073,'There... There\'s no more time for me. I\'m done for. Finish me off, $N. Do it or they\'ll kill us both. $N... Remember Teldrassil, our beloved home. This world is worth saving.',0,0,0,18,'special_surprise SAY_EXEC_TIME_7'),
(-1609074,'There... There\'s no more time for me. I\'m done for. Finish me off, $N. Do it or they\'ll kill us both. $N... For the Horde! This world is worth saving.',0,0,0,18,'special_surprise SAY_EXEC_TIME_8'),
(-1609075,'There... There\'s no more time for me. I\'m done for. Finish me off, $N. Do it or they\'ll kill us both. $N... Remember Mulgore. This world is worth saving.',0,0,0,18,'special_surprise SAY_EXEC_TIME_9'),
(-1609076,'Der... Der\'s no more time for me. I be done for. Finish me off $N. Do it or they\'ll kill us both. $N... Remember Sen\'jin Village, mon! Dis world be worth saving!',0,0,0,18,'special_surprise SAY_EXEC_TIME_10'),

(-1609077,'Do it, $N! Put me out of my misery!',0,0,0,1,'special_surprise SAY_EXEC_WAITING'),
(-1609078,'%s dies from his wounds.',0,2,0,0,'special_surprise EMOTE_DIES'),

(-1609079,'Hrm, what a strange tree. I must investigate.',0,0,0,1,'scarlet courier SAY_TREE_1'),
(-1609080,'What\'s this!? This isn\'t a tree at all! Guards! Guards!',0,0,0,5,'scarlet courier SAY_TREE_2'),

(-1609081,'%s throws rotten apple on $N.',0,2,0,0,'city guard EMOTE_APPLE'),
(-1609082,'%s throws rotten banana on $N.',0,2,0,0,'city guard EMOTE_BANANA'),
(-1609083,'%s spits on $N.',0,2,0,0,'city guard EMOTE_SPIT'),
(-1609084,'Monster!',0,0,0,14,'city guard SAY_RANDOM_1'),
(-1609085,'Murderer!',0,0,0,14,'city guard SAY_RANDOM_2'),
(-1609086,'GET A ROPE!',0,0,0,25,'city guard SAY_RANDOM_3'),
(-1609087,'How dare you set foot in our city!',0,0,0,25,'city guard SAY_RANDOM_4'),
(-1609088,'You disgust me.',0,0,0,14,'city guard SAY_RANDOM_5'),

(-1609089,'The Eye of Acherus launches towards its destination',0,3,0,0,'eye of acherus EMOTE_DESTIANTION'),
(-1609090,'The Eye of Acherus is in your control',0,3,0,0,'eye of acherus EMOTE_CONTROL'),

(-1609091,'Mommy?',0,0,0,434,'scarlet ghoul SAY_GHUL_SPAWN_1'),
(-1609092,'GIVE ME BRAINS!',0,0,0,434,'scarlet ghoul SAY_GHUL_SPAWN_2'),
(-1609093,'Must feed...',0,0,0,434,'scarlet ghoul SAY_GHUL_SPAWN_3'),
(-1609094,'So hungry...',0,0,0,434,'scarlet ghoul SAY_GHUL_SPAWN_4'),
(-1609095,'$gPoppy:Mama;!',0,0,0,434,'scarlet ghoul SAY_GHUL_SPAWN_5'),
(-1609096,'It puts the ghoul in the pit or else it gets the lash!',0,0,0,25,'gothik the harvester SAY_GOTHIK_THROW_IN_PIT'),

(-1609097,'REUSE ME',0,0,0,0,'REUSE ME'),
(-1609098,'REUSE ME',0,0,0,0,'REUSE ME'),

(-1609201,'Soldiers of the Scourge, stand ready! Prepare to unleash your fury upon the Argent Dawn!',14677,1,0,0,'Highlord Darion Mograine'),
(-1609202,'The sky weeps at the devastation of these lands! Soon, Azeroth\'s futile tears will rain down upon us!',14678,1,0,0,'Highlord Darion Mograine'),
(-1609203,'Death knights of Acherus, the death march begins!',14681,1,0,0,'Highlord Darion Mograine'),
(-1609204,'Soldiers of the Scourge, death knights of Acherus, minions of the darkness: hear the call of the Highlord!',14679,1,0,22,'Highlord Darion Mograine'),
(-1609205,'RISE!',14680,1,0,15,'Highlord Darion Mograine'),
(-1609206,'The skies turn red with the blood of the fallen! The Lich King watches over us, minions! Leave only ashes and misery in your destructive wake!',14682,1,0,25,'Highlord Darion Mograine'),
(-1609207,'Scourge armies approach!',0,1,0,0,'Korfax, Champion of the Light'),
(-1609208,'Stand fast, brothers and sisters! The Light will prevail!',14487,1,0,0,'Lord Maxwell Tyrosus'),
(-1609209,'Kneel before the Highlord!',14683,0,0,0,'Highlord Darion Mograine'),
(-1609210,'You stand no chance!',14684,0,0,0,'Highlord Darion Mograine'),
(-1609211,'The Scourge will destroy this place!',14685,0,0,0,'Highlord Darion Mograine'),
(-1609212,'Your life is forfeit.',14686,0,0,0,'Highlord Darion Mograine'),
(-1609213,'Life is meaningless without suffering.',14687,0,0,0,'Highlord Darion Mograine'),
(-1609214,'How much longer will your forces hold out?',14688,0,0,0,'Highlord Darion Mograine'),
(-1609215,'The Argent Dawn is finished!"',14689,0,0,0,'Highlord Darion Mograine'),
(-1609216,'Spare no one!',14690,0,0,0,'Highlord Darion Mograine'),
(-1609217,'What is this?! My... I cannot strike...',14691,0,0,0,'Highlord Darion Mograine'),
(-1609218,'Obey me, blade!',14692,1,0,0,'Highlord Darion Mograine'),
(-1609219,'You will do as I command! I am in control here!',14693,0,0,0,'Highlord Darion Mograine'),
(-1609220,'I can not... the blade fights me.',14694,0,0,0,'Highlord Darion Mograine'),
(-1609221,'What is happening to me?',14695,0,0,0,'Highlord Darion Mograine'),
(-1609222,'Power...wanes...',14696,0,0,0,'Highlord Darion Mograine'),
(-1609223,'Ashbringer defies me...',14697,0,0,0,'Highlord Darion Mograine'),
(-1609224,'Minions, come to my aid!',14698,0,0,0,'Highlord Darion Mograine'),
(-1609225,'You cannot win, Darion!',14584,1,0,0,'Highlord Tirion Fordring'),
(-1609226,'Bring them before the chapel!',14585,1,0,0,'Highlord Tirion Fordring'),
(-1609227,'Stand down, death knights. We have lost... The Light... This place... No hope...',14699,0,0,68,'Highlord Darion Mograine'),
(-1609228,'Have you learned nothing, boy? You have become all that your father fought against! Like that coward, Arthas, you allowed yourself to be consumed by the darkness...the hate... Feeding upon the misery of those you tortured and killed!',14586,0,0,1,'Highlord Tirion Fordring'),
(-1609229,'Your master knows what lies beneath the chapel. It is why he dares not show his face! He\'s sent you and your death knights to meet their doom, Darion.',14587,0,0,25,'Highlord Tirion Fordring'),
(-1609230,'What you are feeling right now is the anguish of a thousand lost souls! Souls that you and your master brought here! The Light will tear you apart, Darion!',14588,0,0,1,'Highlord Tirion Fordring'),
(-1609231,'Save your breath, old man. It might be the last you ever draw.',14700,0,0,25,'Highlord Darion Mograine'),
(-1609232,'My son! My dear, beautiful boy!',14493,0,0,0,'Highlord Alexandros Mograine'),
(-1609233,'Father!',14701,0,0,5,'Highlord Darion Mograine'),
(-1609234,'Argh...what...is...',14702,0,0,68,'Highlord Darion Mograine'),
(-1609235,'Father, you have returned!',14703,0,0,0,'Darion Mograine'),
(-1609236,'You have been gone a long time, father. I thought...',14704,0,0,0,'Darion Mograine'),
(-1609237,'Nothing could have kept me away from here, Darion. Not from my home and family.',14494,0,0,1,'Highlord Alexandros Mograine'),
(-1609238,'Father, I wish to join you in the war against the undead. I want to fight! I can sit idle no longer!',14705,0,0,6,'Darion Mograine'),
(-1609239,'Darion Mograine, you are barely of age to hold a sword, let alone battle the undead hordes of Lordaeron! I couldn\'t bear losing you. Even the thought...',14495,0,0,1,'Highlord Alexandros Mograine'),
(-1609240,'If I die, father, I would rather it be on my feet, standing in defiance against the undead legions! If I die, father, I die with you!',14706,0,0,6,'Darion Mograine'),
(-1609241,'My son, there will come a day when you will command the Ashbringer and, with it, mete justice across this land. I have no doubt that when that day finally comes, you will bring pride to our people and that Lordaeron will be a better place because of you. But, my son, that day is not today.',14496,0,0,1,'Highlord Alexandros Mograine'),
(-1609242,'Do not forget...',14497,0,0,6,'Highlord Alexandros Mograine'),
(-1609243,'Touching...',14803,1,0,0,'The Lich King'),
(-1609244,'You have\'ve betrayed me! You betrayed us all you monster! Face the might of Mograine!',14707,1,0,0,'Highlord Darion Mograine'),
(-1609245,'He\'s mine now...',14805,0,0,0,'The Lich King'),
(-1609246,'Pathetic...',14804,0,0,0,'The Lich King'),
(-1609247,'You\'re a damned monster, Arthas!',14589,0,0,25,'Highlord Tirion Fordring'),
(-1609248,'You were right, Fordring. I did send them in to die. Their lives are meaningless, but yours...',14806,0,0,1,'The Lich King'),
(-1609249,'How simple it was to draw the great Tirion Fordring out of hiding. You\'ve left yourself exposed, paladin. Nothing will save you...',14807,0,0,1,'The Lich King'),
(-1609250,'ATTACK!!!',14488,1,0,0,'Lord Maxwell Tyrosus'),
(-1609251,'APOCALYPSE!',14808,1,0,0,'The Lich King'),
(-1609252,'That day is not today...',14708,0,0,0,'Highlord Darion Mograine'),
(-1609253,'Tirion!',14709,1,0,0,'Highlord Darion Mograine'),
(-1609254,'ARTHAS!!!!',14591,1,0,0,'Highlord Tirion Fordring'),
(-1609255,'What is this?',14809,1,0,0,'The Lich King'),
(-1609256,'Your end.',14592,1,0,0,'Highlord Tirion Fordring'),
(-1609257,'Impossible...',14810,1,0,0,'The Lich King'),
(-1609258,'This... isn\'t... over...',14811,1,0,25,'The Lich King'),
(-1609259,'When next we meet it won\'t be on holy ground, paladin.',14812,1,0,1,'The Lich King'),
(-1609260,'Rise, Darion, and listen...',14593,0,0,0,'Highlord Tirion Fordring'),
(-1609261,'We have all been witness to a terrible tragedy. The blood of good men has been shed upon this soil! Honorable knights, slain defending their lives - our lives!',14594,0,0,0,'Highlord Tirion Fordring'),
(-1609262,'And while such things can never be forgotten, we must remain vigilant in our cause!',14595,0,0,0,'Highlord Tirion Fordring'),
(-1609263,'The Lich King must answer for what he has done and must not be allowed to cause further destruction to our world.',14596,0,0,0,'Highlord Tirion Fordring'),
(-1609264,'I make a promise to you now, brothers and sisters: The Lich King will be defeated! On this day, I call for a union.',14597,0,0,0,'Highlord Tirion Fordring'),
(-1609265,'The Argent Dawn and the Order of the Silver Hand will come together as one! We will succeed where so many before us have failed!',14598,0,0,0,'Highlord Tirion Fordring'),
(-1609266,'We will take the fight to Arthas and tear down the walls of Icecrown!',14599,0,0,15,'Highlord Tirion Fordring'),
(-1609267,'The Argent Crusade comes for you, Arthas!',14600,1,0,15,'Highlord Tirion Fordring'),
(-1609268,'So too do the Knights of the Ebon Blade... While our kind has no place in your world, we will fight to bring an end to the Lich King. This I vow!',14710,0,0,1,'Highlord Darion Mograine'),
(-1609269,'Thousands of Scourge rise up at the Highlord\'s command.',0,3,0,0,''),
(-1609270,'The army marches towards Light\'s Hope Chapel.',0,3,0,0,''),
(-1609271,'After over a hundred Defenders of the Light fall, Highlord Tirion Fordring arrives.',0,3,0,0,''),
(-1609272,'%s flee',0,2,0,0,'Orbaz'),
(-1609273,'%s kneels in defeat before Tirion Fordring.',0,3,0,0,'Highlord Darion Mograine'),
(-1609274,'%s arrives.',0,2,0,0,'Highlord Alexandros Mograine'),
(-1609275,'%s becomes a shade of his past, and walks up to his father.',0,2,0,0,'Highlord Darion Mograine'),
(-1609276,'%s hugs his father.',0,2,0,0,'Darion Mograine'),
(-1609277,'%s disappears, and the Lich King appears.',0,2,0,0,'Alexandros'),
(-1609278,'%s becomes himself again...and is now angry.',0,2,0,0,'Highlord Darion Mograine'),
(-1609279,'%s casts a spell on Tirion.',0,2,0,0,'The Lich King'),
(-1609280,'%s gasps for air.',0,2,0,0,'Highlord Tirion Fordring'),
(-1609281,'%s casts a powerful spell, killing the Defenders and knocking back the others.',0,2,0,0,'The Lich King'),
(-1609282,'%s throws the Corrupted Ashbringer to Tirion, who catches it. Tirion becomes awash with Light, and the Ashbringer is cleansed.',0,2,0,0,'Highlord Darion Mograine'),
(-1609283,'%s collapses.',0,2,0,0,'Highlord Darion Mograine'),
(-1609284,'%s charges towards the Lich King, Ashbringer in hand and strikes the Lich King.',0,2,0,0,'Highlord Tirion Fordring'),
(-1609285,'%s disappears. Tirion walks over to where Darion lay',0,2,0,0,'The Lich King'),
(-1609286,'Light washes over the chapel -- the Light of Dawn is uncovered.',0,2,0,0,''),

(-1609287,'Looks like we\'re going to have ourselves an execution.',0,0,0,25,'city guard SAY_RANDOM_6'),
(-1609288,'Traitorous dog.',0,0,0,14,'city guard SAY_RANDOM_7'),
(-1609289,'My family was wiped out by the Scourge! MONSTER!',0,0,0,25,'city guard SAY_RANDOM_8');

-- -1 615 000 OBSIDIAN SANCTUM
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1615000,'I fear nothing! Least of all you!',14111,1,0,0,'shadron SAY_SHADRON_AGGRO'),
(-1615001,'You are insignificant!',14112,1,0,0,'shadron SAY_SHADRON_SLAY_1'),
(-1615002,'Such mediocre resistance!',14113,1,0,0,'shadron SAY_SHADRON_SLAY_2'),
(-1615003,'We...are superior! How could this...be...',14118,1,0,0,'shadron SAY_SHADRON_DEATH'),
(-1615004,'You are easily bested! ',14114,1,0,0,'shadron SAY_SHADRON_BREATH'),
(-1615005,'I will take pity on you Sartharion, just this once.',14117,1,0,0,'shadron SAY_SHADRON_RESPOND'),
(-1615006,'Father tought me well!',14115,1,0,0,'shadron SAY_SHADRON_SPECIAL_1'),
(-1615007,'On your knees!',14116,1,0,0,'shadron SAY_SHADRON_SPECIAL_2'),
(-1615008,'A Shadron Disciple appears in the Twilight!',0,3,0,0,'shadron WHISPER_SHADRON_DICIPLE'),

(-1615009,'You have no place here. Your place is among the departed.',14122,1,0,0,'tenebron SAY_TENEBRON_AGGRO'),
(-1615010,'No contest.',14123,1,0,0,'tenebron SAY_TENEBRON_SLAY_1'),
(-1615011,'Typical... Just as I was having fun.',14124,1,0,0,'tenebron SAY_TENEBRON_SLAY_2'),
(-1615012,'I should not... have held back...', 14129,1,0,0,'tenebron SAY_TENEBRON_DEATH'),
(-1615013,'To darkness I condemn you...',14125,1,0,0,'tenebron SAY_TENEBRON_BREATH'),
(-1615014,'It is amusing to watch you struggle. Very well, witness how it is done.',14128,1,0,0,'tenebron SAY_TENEBRON_RESPOND'),
(-1615015,'Arrogant little creatures! To challenge powers you do not yet understand...',14126,1,0,0,'tenebron SAY_TENEBRON_SPECIAL_1'),
(-1615016,'I am no mere dragon! You will find I am much, much, more...',14127,1,0,0,'tenebron SAY_TENEBRON_SPECIAL_2'),
(-1615017,'%s begins to hatch eggs in the twilight!',0,3,0,0,'tenebron WHISPER_HATCH_EGGS'),

(-1615018,'It is my charge to watch over these eggs. I will see you burn before any harm comes to them!',14093,1,0,0,'sartharion SAY_SARTHARION_AGGRO'),
(-1615019,'This pathetic siege ends NOW!',14103,1,0,0,'sartharion SAY_SARTHARION_BERSERK'),
(-1615020,'Burn, you miserable wretches!',14098, 1,0,0,'sartharion SAY_SARTHARION_BREATH'),
(-1615021,'Shadron! Come to me, all is at risk!',14105,1,0,0,'sartharion SARTHARION_CALL_SHADRON'),
(-1615022,'Tenebron! The eggs are yours to protect as well!',14106,1,0,0,'sartharion SAY_SARTHARION_CALL_TENEBRON'),
(-1615023,'Vesperon! The clutch is in danger! Assist me!',14104,1,0,0,'sartharion SAY_SARTHARION_CALL_VESPERON'),
(-1615024,'Such is the price... of failure...',14107,1,0,0,'sartharion SAY_SARTHARION_DEATH'),
(-1615025,'Such flammable little insects....',14099,1,0,0,'sartharion SAY_SARTHARION_SPECIAL_1'),
(-1615026,'Your charred bones will litter the floor!',14100,1,0,0,'sartharion SAY_SARTHARION_SPECIAL_2'),
(-1615027,'How much heat can you take?',14101,1,0,0,'sartharion SAY_SARTHARION_SPECIAL_3'),
(-1615028,'All will be reduced to ash!',14102,1,0,0,'sartharion SAY_SARTHARION_SPECIAL_4'),
(-1615029,'You will make a fine meal for the hatchlings.',14094,1,0,0,'sartharion SAY_SARTHARION_SLAY_1'),
(-1615030,'You are the grave disadvantage.',14096,1,0,0,'sartharion SAY_SARTHARION_SLAY_2'),
(-1615031,'This is why we call you lesser beeings.',14097,1,0,0,'sartharion SAY_SARTHARION_SLAY_3'),
(-1615032,'The lava surrounding %s churns!',0,3,0,0,'sartharion WHISPER_LAVA_CHURN'),

(-1615033,'You pose no threat, lesser beings...give me your worst!',14133,1,0,0,'vesperon SAY_VESPERON_AGGRO'),
(-1615034,'The least you could do is put up a fight...',14134,1,0,0,'vesperon SAY_VESPERON_SLAY_1'),
(-1615035,'Was that the best you can do?',14135,1,0,0,'vesperon SAY_VESPERON_SLAY_2'),
(-1615036,'I still have some...fight..in...me...', 14140,1,0,0,'vesperon SAY_VESPERON_DEATH'),
(-1615037,'I will pick my teeth with your bones!',14136,1,0,0,'vesperon SAY_VESPERON_BREATH'),
(-1615038,'Father was right about you, Sartharion...You are a weakling!',14139,1,0,0,'vesperon SAY_VESPERON_RESPOND'),
(-1615039,'Aren\'t you tricky...I have a few tricks of my own...',14137,1,0,0,'vesperon SAY_VESPERON_SPECIAL_1'),
(-1615040,'Unlike, I have many talents.',14138,1,0,0,'vesperon SAY_VESPERON_SPECIAL_2'),
(-1615041,'A Vesperon Disciple appears in the Twilight!',0,3,0,0,'shadron WHISPER_VESPERON_DICIPLE'),

(-1615042,'%s begins to open a Twilight Portal!',0,3,0,0,'sartharion drake WHISPER_OPEN_PORTAL');

-- -1 616 000 EYE OF ETERNITY
INSERT INTO script_texts (entry,content_default,sound,type,LANGUAGE,emote,comment) VALUES
(-1616000,'Lesser beings, intruding here! A shame that your excess courage does not compensate for your stupidity!',14512,1,0,0,'malygos SAY_INTRO_1'),
(-1616001,'None but the blue dragonflight are welcome here! Perhaps this is the work of Alexstrasza? Well then, she has sent you to your deaths.',14513,1,0,0,'malygos SAY_INTRO_2'),
(-1616002,'What could you hope to accomplish, to storm brazenly into my domain? To employ MAGIC? Against ME? <Laughs>',14514,1,0,0,'malygos SAY_INTRO_3'),
(-1616003,'I am without limits here... the rules of your cherished reality do not apply... In this realm, I am in control...',14515,1,0,0,'malygos SAY_INTRO_4'),
(-1616004,'I give you one chance. Pledge fealty to me, and perhaps I won\'t slaughter you for your insolence!',14516,1,0,0,'malygos SAY_INTRO_5'),
(-1616005,'My patience has reached its limit, I WILL BE RID OF YOU!',14517,1,0,0,'malygos SAY_AGGRO'),
(-1616006,'Watch helplessly as your hopes are swept away...',14525,1,0,0,'malygos SAY_VORTEX'),
(-1616007,'I AM UNSTOPPABLE!',14533,1,0,0,'malygos SAY_SPARK_BUFF'),
(-1616008,'Your stupidity has finally caught up to you!',14519,1,0,0,'malygos SAY_SLAY_1_A'),
(-1616009,'More artifacts to confiscate...',14520,1,0,0,'malygos SAY_SLAY_1_B'),
(-1616010,'<Laughs> How very... naive...',14521,1,0,0,'malygos SAY_SLAY_1_C'),
(-1616011,'I had hoped to end your lives quickly, but you have proven more...resilient then I had anticipated. Nonetheless, your efforts are in vain, it is you reckless, careless mortals who are to blame for this war! I do what I must...And if it means your...extinction...THEN SO BE IT!',14522,1,0,0,'malygos SAY_END_PHASE_1'),
(-1616012,'Few have experienced the pain I will now inflict upon you!',14523,1,0,0,'malygos SAY_START_PHASE_2'),
(-1616013,'You will not succeed while I draw breath!',14518,1,0,0,'malygos SAY_DEEP_BREATH'),
(-1616014,'I will teach you IGNORANT children just how little you know of magic...',14524,1,0,0,'malygos SAY_SHELL'),
(-1616015,'Your energy will be put to good use!',14526,1,0,0,'malygos SAY_SLAY_2_A'),
(-1616016,'I am the spell-weaver! My power is infinite!',14527,1,0,0,'malygos SAY_SLAY_2_B'),
(-1616017,'Your spirit will linger here forever!',14528,1,0,0, 'malygos SAY_SLAY_2_C'),
(-1616018,'ENOUGH! If you intend to reclaim Azeroth\'s magic, then you shall have it...',14529,1,0,0,'malygos SAY_END_PHASE_2'),
(-1616019,'Now your benefactors make their appearance...But they are too late. The powers contained here are sufficient to destroy the world ten times over! What do you think they will do to you?',14530,1,0,0,'malygos SAY_INTRO_PHASE_3'),
(-1616020,'SUBMIT!',14531,1,0,0,'malygos SAY_START_PHASE_3'),
(-1616021,'Alexstrasza! Another of your brood falls!',14534,1,0,0,'malygos SAY_SLAY_3_A'),
(-1616022,'Little more then gnats!',14535,1,0,0,'malygos SAY_SLAY_3_B'),
(-1616023,'Your red allies will share your fate...',14536,1,0,1,'malygos SAY_SLAY_3_C'),
(-1616024,'The powers at work here exceed anything you could possibly imagine!',14532,1,0,0,'malygos SAY_SURGE'),
(-1616025,'Still standing? Not for long...',14537,1,0,0,'malygos SAY_SPELL_1'),
(-1616026,'Your cause is lost!',14538,1,0,0,'malygos SAY_SPELL_2'),
(-1616027,'Your fragile mind will be shattered!',14539,1,0,0,'malygos SAY_SPELL_3'),
(-1616028,'UNTHINKABLE! The mortals will destroy... e-everything... my sister... what have you-',14540,1,0,0,'malygos SAY_DEATH'),
(-1616029,'I did what I had to, brother. You gave me no alternative.',14406,1,0,1,'alextrasza SAY_OUTRO_1'),
(-1616030,'And so ends the Nexus War.',14407,1,0,1,'alextrasza SAY_OUTRO_2'),
(-1616031,'This resolution pains me deeply, but the destruction, the monumental loss of life had to end. Regardless of Malygos\' recent transgressions, I will mourn his loss. He was once a guardian, a protector. This day, one of the world\'s mightiest has fallen.',14408,1,0,1,'alextrasza SAY_OUTRO_3'),
(-1616032,'The red dragonflight will take on the burden of mending the devastation wrought on Azeroth. Return home to your people and rest. Tomorrow will bring you new challenges, and you must be ready to face them. Life...goes on.',14409,1,0,1,'alextrasza SAY_OUTRO_4'),
(-1616033,'A Power Spark forms from a nearby rift!',0,3,0,0,'malygos SAY_EMOTE_SPARK'),
(-1616034,'%s takes a deep breath.',0,3,0,0,'malygos SAY_EMOTE_BREATH');

-- -1 619 000 AHN'KAHET
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1619000,'The secrets of the deep shall remain hidden.',14033,1,0,0,'nadox SAY_AGGRO'),
(-1619001,'The young must not grow hungry...',14034,1,0,0,'nadox SAY_SUMMON_EGG_1'),
(-1619002,'Shhhad ak kereeesshh chak-k-k!',14035,1,0,0,'nadox SAY_SUMMON_EGG_2'),
(-1619003,'Sleep now, in the cold dark.',14036,1,0,0,'nadox SAY_SLAY_1'),
(-1619004,'For the Lich King!',14037,1,0,0,'nadox SAY_SLAY_2'),
(-1619005,'Perhaps we will be allies soon.',14038,1,0,0,'nadox SAY_SLAY_3'),
(-1619006,'Master, is my service complete?',14039,1,0,0,'nadox SAY_DEATH'),
(-1619007,'An Ahn\'kahar Guardian hatches!',0,3,0,0,'nadox EMOTE_HATCH'),

(-1619008,'I will feast on your remains.',14360,1,0,0,'taldaram SAY_AGGRO'),
(-1619009,'Your heartbeat is music to my ears.',14361,1,0,0,'taldaram SAY_VANISH_1'),
(-1619010,'I am nowhere. I am everywhere. I am the watcher, unseen.',14362,1,0,0,'taldaram SAY_VANISH_2'),
(-1619011,'So appetizing.',14363,1,0,0,'taldaram SAY_FEED_1'),
(-1619012,'Fresh, warm blood. It has been too long.',14364,1,0,0,'taldaram SAY_FEED_2'),
(-1619013,'Bin-dor\'el',14365,1,0,0,'taldaram SAY_SLAY_1'),
(-1619014,'I will drink no blood before it\'s time.',14366,1,0,0,'taldaram SAY_SLAY_2'),
(-1619015,'One final embrace.',14367,1,0,0,'taldaram SAY_SLAY_3'),
(-1619016,'Still I hunger, still I thirst.',14368,1,0,0,'taldaram SAY_DEATH'),

(-1619017,'These are sacred halls! Your intrusion will be met with death.',14343,1,0,0,'jedoga SAY_AGGRO'),
(-1619018,'Who among you is devoted?',14344,1,0,0,'jedoga SAY_CALL_SACRIFICE_1'),
(-1619019,'You there! Step forward!',14345,1,0,0,'jedoga SAY_CALL_SACRIFICE_2'),
(-1619020,'Yogg-Saron, grant me your power!',14346,1,0,0,'jedoga SAY_SACRIFICE_1'),
(-1619021,'Master, a gift for you!',14347,1,0,0,'jedoga SAY_SACRIFICE_2'),
(-1619022,'Glory to Yogg-Saron!',14348,1,0,0,'jedoga SAY_SLAY_1'),
(-1619023,'You are unworthy!',14349,1,0,0,'jedoga SAY_SLAY_2'),
(-1619024,'Get up! You haven\'t suffered enough.',14350,1,0,0,'jedoga SAY_SLAY_3'),
(-1619025,'Do not expect your sacrilege... to go unpunished.',14351,1,0,0,'jedoga SAY_DEATH'),
(-1619026,'The elements themselves will rise up against the civilized world! Only the faithful will be spared!',14352,1,0,0,'jedoga SAY_PREACHING_1'),
(-1619027,'Immortality can be yours. But only if you pledge yourself fully to Yogg-Saron!',14353,1,0,0,'jedoga SAY_PREACHING_2'),
(-1619028,'Here on the very borders of his domain. You will experience powers you would never have imagined! ',14354,1,0,0,'jedoga SAY_PREACHING_3'),
(-1619029,'You have traveled long and risked much to be here. Your devotion shall be rewarded.',14355,1,0,0,'jedoga SAY_PREACHING_4'),
(-1619030,'The faithful shall be exalted! But there is more work to be done. We will press on until all of Azeroth lies beneath his shadow!',14356,1,0,0,'jedoga SAY_PREACHING_5'),
(-1619031,'I have been chosen!',0,1,0,0,'jedoga SAY_VOLUNTEER_1'),
(-1619032,'I give myself to the master!',0,1,0,0,'jedoga SAY_VOLUNTEER_2'),

(-1619033,'Shgla\'yos plahf mh\'naus.',14043,1,0,0,'volazj SAY_AGGRO'),
(-1619034,'Gul\'kafh an\'shel. Yoq\'al shn ky ywaq nuul.',14044,1,0,0,'volazj SAY_INSANITY'),
(-1619035,'Ywaq puul skshgn: on\'ma yeh\'glu zuq.',14045,1,0,0,'volazj SAY_SLAY_1'),
(-1619036,'Ywaq ma phgwa\'cul hnakf.',14046,1,0,0,'volazj SAY_SLAY_2'),
(-1619037,'Ywaq maq oou; ywaq maq ssaggh. Ywaq ma shg\'fhn.',14047,1,0,0,'volazj SAY_SLAY_3'),
(-1619038,' ',14048,1,0,0,'volazj SAY_DEATH_1'),
(-1619039,'Iilth vwah, uhn\'agth fhssh za.',14049,1,0,0,'volazj SAY_DEATH_2');

-- -1 631 000 ICC: ICECROWN CITADEL
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1631001,'This is the beginning AND the end, mortals. None may enter the master\'s sanctum!',16950,1,0,0,'marrowgar SAY_INTRO'),
(-1631002,'The Scourge will wash over this world as a swarm of death and destruction!',16941,1,0,0,'marrowgar SAY_AGGRO'),
(-1631003,'BONE STORM!',16946,1,0,0,'marrowgar SAY_BONE_STORM'),
(-1631004,'Bound by bone!',16947,1,0,0,'marrowgar SAY_BONE_SPIKE_1'),
(-1631005,'Stick Around!',16948,1,0,0,'marrowgar SAY_BONE_SPIKE_2'),
(-1631006,'The only escape is death!',16949,1,0,0,'marrowgar SAY_BONE_SPIKE_3'),
(-1631007,'More bones for the offering!',16942,1,0,0,'marrowgar SAY_SLAY_1'),
(-1631008,'Languish in damnation!',16943,1,0,0,'marrowgar SAY_SLAY_2'),
(-1631009,'I see... only darkness...',16944,1,0,0,'marrowgar SAY_DEATH'),
(-1631010,'THE MASTER\'S RAGE COURSES THROUGH ME!',16945,1,0,0,'marrowgar SAY_BERSERK'),

(-1631011,'You have found your way here, because you are among the few gifted with true vision in a world cursed with blindness.',17272,1,0,0,'deathwhisper SAY_SPEECH_1'),
(-1631012,'You can see through the fog that hangs over this world like a shroud, and grasp where true power lies.',17273,1,0,0,'deathwhisper SAY_SPEECH_2'),
(-1631013,'Fix your eyes upon your crude hands: the sinew, the soft meat, the dark blood coursing within.',16878,1,0,0,'deathwhisper SAY_SPEECH_3'),
(-1631014,'It is a weakness; a crippling flaw.... A joke played by the Creators upon their own creations.',17268,1,0,0,'deathwhisper SAY_SPEECH_4'),
(-1631015,'The sooner you come to accept your condition as a defect, the sooner you will find yourselves in a position to transcend it.',17269,1,0,0,'deathwhisper SAY_SPEECH_5'),
(-1631016,'Through our Master, all things are possible. His power is without limit, and his will unbending.',17270,1,0,0,'deathwhisper SAY_SPEECH_6'),
(-1631017,'Those who oppose him will be destroyed utterly, and those who serve -- who serve wholly, unquestioningly, with utter devotion of mind and soul -- elevated to heights beyond your ken.',17271,1,0,0,'deathwhisper SAY_SPEECH_7'),
(-1631018,'What is this disturbance?! You dare trespass upon this hallowed ground? This shall be your final resting place.',16868,1,0,0,'deathwhisper SAY_AGGRO'),
(-1631019,'Enough! I see I must take matters into my own hands!',16877,1,0,0,'deathwhisper SAY_PHASE_TWO'),
(-1631020,'Take this blessing and show these intruders a taste of our master\'s power.',16873,1,0,0,'deathwhisper SAY_DARK_EMPOWERMENT'),
(-1631021,'I release you from the curse of flesh!',16874,1,0,0,'deathwhisper SAY_DARK_TRANSFORMATION'),
(-1631022,'Arise and exalt in your pure form!',16875,1,0,0,'deathwhisper SAY_ANIMATE_DEAD'),
(-1631023,'You are weak, powerless to resist my will!',16876,1,0,0,'deathwhisper SAY_DOMINATE_MIND'),
(-1631024,'This charade has gone on long enough.',16872,1,0,0,'deathwhisper SAY_BERSERK'),
(-1631025,'All part of the masters plan! Your end is... inevitable!',16871,1,0,0,'deathwhisper SAY_DEATH'),
(-1631026,'Do you yet grasp of the futility of your actions?',16869,1,0,0,'deathwhisper SAY_SLAY_1'),
(-1631027,'Embrace the darkness... Darkness eternal!',16870,1,0,0,'deathwhisper SAY_SLAY_2'),

(-1631028,'BY THE MIGHT OF THE LICH KING!',16694,1,0,0,'saurfang SAY_AGGRO'),
(-1631029,'The ground runs red with your blood!',16699,1,0,0,'saurfang SAY_FALLENCHAMPION'),
(-1631030,'Feast, my minions!',16700,1,0,0,'saurfang SAY_BLOODBEASTS'),
(-1631031,'You are nothing!',16695,1,0,0,'saurfang SAY_SLAY_1'),
(-1631032,'Your soul will find no redemption here!',16696,1,0,0,'saurfang SAY_SLAY_2'),
(-1631033,'I have become...DEATH!',16698,1,0,0,'saurfang SAY_BERSERK'),
(-1631034,'I... Am... Released.',16697,1,0,0,'saurfang SAY_DEATH'),
(-1631035,'Let\'s get a move on then! Move ou...',16974,1,0,0,'bronzebeard SAY_INTRO_ALLY_0'),
(-1631036,'For every Horde soldier that you killed, for every Alliance dog that fell, the Lich King\'s armies grew. Even now the Val\'kyr work to raise your fallen... As Scourge.',16701,1,0,0,'saurfang SAY_INTRO_ALLY_1'),
(-1631037,'Things are about to get much worse. Come, taste the power that the Lich King has bestowed upon me!',16702,1,0,0,'saurfang SAY_INTRO_ALLY_2'),
(-1631038,'A lone orc, against the might of the Alliance?',16970,1,0,0,'bronzebeard SAY_INTRO_ALLY_3'),
(-1631039,'Charge!',16971,1,0,0,'bronzebeard SAY_INTRO_ALLY_4'),
(-1631040,'Hahahaha! Dwarves...',16703,1,0,0,'saurfang SAY_INTRO_ALLY_5'),
(-1631041,'Kor\'kron, move out! Champions, watch your backs. The Scourge have been..',17103,1,0,0,'overlord SAY_INTRO_HORDE_1'),
(-1631042,'Join me, father. Join me and we will crush this world in the name of the Scourge -- for the glory of the Lich King!',16704,1,0,0,'saurfang SAY_INTRO_HORDE_2'),
(-1631043,'My boy died at the Wrath Gate. I am here only to collect his body.',17097,0,0,0,'overlord SAY_INTRO_HORDE_3'),
(-1631044,'Stubborn and old. What chance do you have? I am stronger, and more powerful than you ever were.',16705,1,0,0,'saurfang SAY_INTRO_HORDE_4'),
(-1631045,'We named him Dranosh. It means "Heart of Draenor" in orcish. I would not let the warlocks take him. My boy would be safe, hidden away by the elders of Garadar.',17098,0,0,0,'overlord SAY_INTRO_HORDE_5'),
(-1631046,'I made a promise to his mother before she died; that I would cross the Dark Portal alone - whether I lived or died, my son would be safe. Untainted...',17099,0,0,0,'overlord SAY_INTRO_HORDE_6'),
(-1631047,'Today, I fulfill that promise.',17100,0,0,0,'overlord SAY_INTRO_HORDE_7'),
(-1631048,'High Overlord Saurfang charges!',17104,2,0,0,'overlord SAY_INTRO_HORDE_8'),
(-1631049,'Pathetic old orc. Come then heroes. Come and face the might of the Scourge!',16706,1,0,0,'saurfang SAY_INTRO_HORDE_9'),
(-1631050,'%s gasps for air',16975,2,0,0,'bronzebeard SAY_OUTRO_ALLY_1'),
(-1631051,'That was Saurfang\'s boy - the Horde commander at the Wrath Gate. Such a tragic end...',16976,0,0,0,'bronzebeard SAY_OUTRO_ALLY_2'),
(-1631052,'What in the... There, in the distance!',16977,0,0,0,'bronzebeard SAY_OUTRO_ALLY_3'),
(-1631053,'Soldiers, fall in! Looks like the Horde are comin\' in to take another shot!',16978,1,0,0,'bronzebeard SAY_OUTRO_ALLY_4'),
(-1631054,'Don\'t force my hand, orc. We can\'t let you pass.',16972,0,0,0,'bronzebeard SAY_OUTRO_ALLY_5'),
(-1631055,'Behind you lies the body of my only son. Nothing will keep me from him.',17094,0,0,0,'overlord SAY_OUTRO_ALLY_6'),
(-1631056,'He... I can\'t do it. Get back on your ship and we\'ll spare your life.',16973,0,0,0,'bronzebeard SAY_OUTRO_ALLY_7'),
(-1631057,'Stand down, Muradin. Let a grieving father pass.',16690,0,0,0,'varian SAY_OUTRO_ALLY_8'),
(-1631058,'No\'ku kil zil\'nok ha tar.',17096,0,1,0,'overlord SAY_OUTRO_ALLY_9'),
(-1631059,'I will not forget this kindess. I thank you, highness.',17095,0,0,0,'overlord SAY_OUTRO_ALLY_10'),
(-1631060,'I... I was not at the Wrathgate. But the soldiers who survived told me much of what happened. Your son fought with honor. He died a hero\'s death. He deserves a hero\'s burial.',16691,0,0,0,'varian SAY_OUTRO_ALLY_11'),
(-1631061,'%s cries.',16651,2,0,0,'proudmore SAY_OUTRO_ALLY_12'),
(-1631062,'Jaina, why are you crying?',16692,0,0,0,'varian SAY_OUTRO_ALLY_13'),
(-1631063,'It was nothing, your majesty. Just... I\'m proud of my king.',16652,0,0,0,'proudmore SAY_OUTRO_ALLY_14'),
(-1631064,'Bah! Muradin, secure the deck and prepare our soldiers for an assault on the upper citadel. I\'ll send out another regiment from Stormwind.',16693,0,0,0,'varian SAY_OUTRO_ALLY_15'),
(-1631065,'Right away, yer majesty!',16979,0,0,0,'bronzebeard SAY_OUTRO_ALLY_16'),
(-1631066,'%s coughs.',17105,2,0,0,'overlord SAY_OUTRO_HORDE_1'),
(-1631067,'%s weeps over the corpse of his son.',17106,2,0,0,'overlord SAY_OUTRO_HORDE_2'),
(-1631068,'You will have a proper ceremony in Nagrand next to the pyres of your mother and ancestors.',17101,0,0,0,'overlord SAY_OUTRO_HORDE_3'),
(-1631069,'Honor, young heroes... no matter how dire the battle... Never forsake it!',17102,0,0,0,'overlord SAY_OUTRO_HORDE_4'),

(-1631070,'What? Precious? Noooooooooo!!!',16993,6,0,0,'rotface SAY_PRECIOUS_DIES'),
(-1631071,'WEEEEEE!',16986,1,0,0,'rotface SAY_AGGRO'),
(-1631072,'Icky sticky.',16991,1,0,0,'rotface SAY_SLIME_SPRAY'),
(-1631073,'I think I made an angry poo-poo. It gonna blow!',16992,1,0,0,'rotface SAY_OOZE_EXPLODE'),
(-1631074,'Great news, everyone! The slime is flowing again!',17126,1,0,0,'putricide SAY_SLIME_FLOW_1'),
(-1631075,'Good news, everyone! I\'ve fixed the poison slime pipes!',17123,1,0,0,'putricide SAY_SLIME_FLOW_2'),
(-1631076,'Daddy make toys out of you!',16987,1,0,0,'rotface SAY_SLAY_1'),
(-1631077,'I brokes-ded it...',16988,1,0,0,'rotface SAY_SLAY_2'),
(-1631078,'Sleepy Time!',16990,1,0,0,'rotface SAY_BERSERK'),
(-1631079,'Bad news daddy.',16989,1,0,0,'rotface SAY_DEATH'),
(-1631080,'Terrible news, everyone, Rotface is dead! But great news everyone, he left behind plenty of ooze for me to use! Whaa...? I\'m a poet, and I didn\'t know it? Astounding!',17146,6,0,0,'putricide SAY_ROTFACE_DEATH'),

(-1631081,'NOOOO! You kill Stinky! You pay!',16907,6,0,0,'festergut SAY_STINKY_DIES'),
(-1631082,'Fun time!',16901,1,0,0,'festergut SAY_AGGRO'),
(-1631083,'Just an ordinary gas cloud. But watch out, because that\'s no ordinary gas cloud! ',17119,1,0,0,'putricide SAY_BLIGHT'),
(-1631084,'%s farts.',16911,2,0,0,'festergut SAY_SPORE'), -- TODO Can be wrong
(-1631085,'I not feel so good...',16906,1,0,0,'festergut SAY_PUNGUENT_BLIGHT'),
(-1631086,'%s vomits',0,2,0,0,'festergut SAY_PUNGUENT_BLIGHT_EMOTE'), -- TODO Can be wrong
(-1631087,'Daddy, I did it',16902,1,0,0,'festergut SAY_SLAY_1'),
(-1631088,'Dead, dead, dead!',16903,1,0,0,'festergut SAY_SLAY_2'),
(-1631089,'Fun time over!',16905,1,0,0,'festergut SAY_BERSERK'),
(-1631090,'Da ... Ddy...',16904,1,0,0,'festergut SAY_DEATH'),
(-1631091,'Oh, Festergut. You were always my favorite. Next to Rotface. The good news is you left behind so much gas, I can practically taste it!',17124,6,0,0,'putricide SAY_FESTERGUT_DEATH'),

(-1631092,'Good news, everyone! I think I perfected a plague that will destroy all life on Azeroth!',17114,1,0,0,'putricide SAY_AGGRO'),
(-1631093,'You can\'t come in here all dirty like that! You need that nasty flesh scrubbed off first!',17125,1,0,0,'putricide SAY_AIRLOCK'),
(-1631094,'Two oozes, one room! So many delightful possibilities...',17122,1,0,0,'putricide SAY_PHASE_CHANGE'),
(-1631095,'Hmm. I don\'t feel a thing. Whaa...? Where\'d those come from?',17120,1,0,0,'putricide SAY_TRANSFORM_1'),
(-1631096,'Tastes like... Cherry! Oh! Excuse me!',17121,1,0,0,'putricide SAY_TRANSFORM_2'),
(-1631097,'Hmm... Interesting...',17115,1,0,0,'putricide SAY_SLAY_1'),
(-1631098,'That was unexpected!',17116,1,0,0,'putricide SAY_SLAY_2'),
(-1631099,'Great news, everyone!',17118,1,0,0,'putricide SAY_BERSERK'),
(-1631100,'Bad news, everyone! I don\'t think I\'m going to make it',17117,1,0,0,'putricide SAY_DEATH'),

(-1631101,'Foolish mortals. You thought us defeated so easily? The San\'layn are the Lich King\'s immortal soldiers! Now you shall face their might combined!',16795,6,0,1,'lanathel SAY_COUNCIL_INTRO_1'),
(-1631102,'Rise up, brothers, and destroy our enemies',16796,6,0,0,'lanathel SAY_COUNCIL_INTRO_2'),

(-1631103,'Such wondrous power! The Darkfallen Orb has made me INVINCIBLE!',16727,1,0,0,'keleseth SAY_KELESETH_INVOCATION'),
(-1631104,'Blood will flow!',16728,1,0,0,'keleseth SAY_KELESETH_SPECIAL'),
(-1631105,'Were you ever a threat?',16723,1,0,0,'keleseth SAY_KELESETH_SLAY_1'),
(-1631106,'Truth is found in death.',16724,1,0,0,'keleseth SAY_KELESETH_SLAY_2'),
(-1631107,'%s cackles maniacally!',16726,2,0,0,'keleseth SAY_KELESETH_BERSERK'), -- TODO Can be wrong
(-1631108,'My queen... they come...',16725,1,0,0,'keleseth SAY_KELESETH_DEATH'),

(-1631109,'Tremble before Taldaram, mortals, for the power of the orb flows through me!',16857,1,0,0,'taldaram SAY_TALDARAM_INVOCATION'),
(-1631110,'Delight in the pain!',16858,1,0,0,'taldaram SAY_TALDARAM_SPECIAL'),
(-1631111,'Worm food.',16853,1,0,0,'taldaram SAY_TALDARAM_SLAY_1'),
(-1631112,'Beg for mercy!',16854,1,0,0,'taldaram SAY_TALDARAM_SLAY_2'),
(-1631113,'%s laughs.',16856,2,0,0,'taldaram SAY_TALDARAM_BERSERK'), -- TODO Can be wrong
(-1631114,'%s gurgles and dies.',16855,2,0,0,'taldaram SAY_TALDARAM_DEATH'), -- TODO Can be wrong

(-1631115,'Naxxanar was merely a setback! With the power of the orb, Valanar will have his vengeance!',16685,1,0,0,'valanar SAY_VALANAR_INVOCATION'),
(-1631116,'My cup runneth over.',16686,1,0,0,'valanar SAY_VALANAR_SPECIAL'),
(-1631117,'Dinner... is served.',16681,1,0,0,'valanar SAY_VALANAR_SLAY_1'),
(-1631118,'Do you see NOW the power of the Darkfallen?',16682,1,0,0,'valanar SAY_VALANAR_SLAY_2'),
(-1631119,'BOW DOWN BEFORE THE SAN\'LAYN!',16684,1,0,0,'valanar SAY_VALANAR_BERSERK'),
(-1631120,'...why...?',16683,1,0,0,'valanar SAY_VALANAR_DEATH'),

(-1631121,'You have made an... unwise... decision.',16782,1,0,0,'blood_queen SAY_AGGRO'),
(-1631122,'Just a taste...',16783,1,0,0,'blood_queen SAY_BITE_1'),
(-1631123,'Know my hunger!',16784,1,0,0,'blood_queen SAY_BITE_2'),
(-1631124,'SUFFER!',16786,1,0,0,'blood_queen SAY_SHADOWS'),
(-1631125,'Can you handle this?',16787,1,0,0,'blood_queen SAY_PACT'),
(-1631126,'Yes... feed my precious one! You\'re mine now!',16790,1,0,0,'blood_queen SAY_MC'),
(-1631127,'Here it comes.',16788,1,0,0,'blood_queen SAY_AIR_PHASE'),
(-1631128,'THIS ENDS NOW!',16793,1,0,0,'blood_queen SAY_BERSERK'),
(-1631129,'But... we were getting along... so well...',16794,1,0,0,'blood_queen SAY_DEATH'),

(-1631130,'Ready your arms, my Argent Brothers. The Vrykul will protect the Frost Queen with their lives.',16819,1,0,0,'scourgebane SAY_SVALNA_EVENT_1'),
(-1631131,'Even dying here beats spending another day collecting reagents for that madman, Finklestein.',16585,1,0,0,'arnath SAY_SVALNA_EVENT_2'),
(-1631132,'Enough idle banter! Our champions have arrived - support them as we push our way through the hall!',16820,1,0,0,'scourgebane SAY_SVALNA_EVENT_3'),
(-1631133,'You may have once fought beside me, Crok, but now you are nothing more than a traitor. Come, your second death approaches!',17017,1,0,0,'svalna SAY_SVALNA_EVENT_4'),
(-1631134,'Miserable creatures, Die!',17018,1,0,0,'svalna SAY_KILLING_CRUSADERS'),
(-1631135,'Foolish Crok, you brought my reinforcements with you! Arise Argent Champions and serve the Lich King in death!',17019,1,0,0,'svalna SAY_RESSURECT'),
(-1631136,'Come Scourgebane, I\'ll show the Lich King which one of us is truly worthy of the title, champion!',17020,1,0,0,'svalna SAY_SVALNA_AGGRO'),
(-1631137,'What? They died so easily? No matter.',17022,1,0,0,'svalna SAY_KILL_CAPTAIN'),
(-1631138,'What a pitiful choice of an ally Crok.',17021,1,0,0,'svalna SAY_KILL_PLAYER'),
(-1631139,'Perhaps... you were right... Crok.',17023,1,0,0,'svalna SAY_DEATH'),

(-1631140,'Heroes, lend me your aid! I... I cannot hold them off much longer! You must heal my wounds!',17064,1,0,0,'dreamwalker SAY_AGGRO'),
(-1631141,'I have opened a portal into the Dream. Your salvation lies within, heroes.',17068,1,0,0,'dreamwalker SAY_PORTAL'),
(-1631142,'My strength is returning! Press on, heroes!',17070,1,0,0,'dreamwalker SAY_75_HEALTH'),
(-1631143,'I will not last much longer!',17069,1,0,0,'dreamwalker SAY_25_HEALTH'),
(-1631144,'Forgive me for what I do! I... cannot... stop... ONLY NIGHTMARES REMAIN!',17072,1,0,0,'dreamwalker SAY_0_HEALTH'),
(-1631145,'A tragic loss...',17066,1,0,0,'dreamwalker SAY_PLAYER_DIES'),
(-1631146,'FAILURES!',17067,1,0,0,'dreamwalker SAY_BERSERK'),
(-1631147,'I am renewed! Ysera grants me the favor to lay these foul creatures to rest!',17071,1,0,0,'dreamwalker SAY_VICTORY'),

(-1631148,'You are fools who have come to this place! The icy winds of Northrend will consume your souls!',17007,1,0,0,'sindragosa SAY_AGGRO'),
(-1631149,'Suffer, mortals, as your pathetic magic betrays you!',17014,1,0,0,'sindragosa SAY_UNCHAINED_MAGIC'),
(-1631150,'Can you feel the cold hand of death upon your heart?',17013,1,0,0,'sindragosa SAY_BLISTERING_COLD'),
(-1631151,'Aaah! It burns! What sorcery is this?!',17015,1,0,0,'sindragosa SAY_RESPIRE'),
(-1631152,'Your incursion ends here! None shall survive!',17012,1,0,0,'sindragosa SAY_TAKEOFF'),
(-1631153,'Now feel my master\'s limitless power and despair!',17016,1,0,0,'sindragosa SAY_PHASE_3'),
(-1631154,'Perish!',17008,1,0,0,'sindragosa SAY_SLAY_1'),
(-1631155,'A flaw of mortality...',17009,1,0,0,'sindragosa SAY_SLAY_2'),
(-1631156,'Enough! I tire of these games!',17011,1,0,0,'sindragosa SAY_BERSERK'),
(-1631157,'Free...at last...',17010,1,0,0,'sindragosa SAY_DEATH'),

(-1631158,'So...the Light\'s vaunted justice has finally arrived. Shall I lay down Frostmourne and throw myself at your mercy, Fordring?',17349,1,0,0,'lich_king SAY_INTRO_1'),
(-1631159,'We will grant you a swift death, Arthas. More than can be said for the thousands you\'ve tortured and slain.',17390,1,0,0,'tirion SAY_INTRO_2'),
(-1631160,'You will learn of that first hand. When my work is complete, you will beg for mercy -- and I will deny you. Your anguished cries will be testament to my unbridled power.',17350,1,0,0,'lich_king SAY_INTRO_3'),
(-1631161,'So be it. Champions, attack!',17391,1,0,0,'tirion SAY_INTRO_4'),
(-1631162,'I\'ll keep you alive to witness the end, Fordring. I would not want the Light\'s greatest champion to miss seeing this wretched world remade in my image.',17351,1,0,0,'lich_king SAY_INTRO_5'),
(-1631163,'Come then champions, feed me your rage!',17352,1,0,0,'lich_king SAY_AGGRO'),
(-1631164,'I will freeze you from within until all that remains is an icy husk!',17369,1,0,0,'lich_king SAY_REMORSELESS_WINTER'),
(-1631165,'Watch as the world around you collapses!',17370,1,0,0,'lich_king SAY_SHATTER_ARENA'),
(-1631166,'Val\'kyr, your master calls!',17373,1,0,0,'lich_king SAY_SUMMON_VALKYR'),
(-1631167,'Frostmourne hungers...',17366,1,0,0,'lich_king SAY_HARVEST_SOUL'),
(-1631168,'You have come to bring Arthas to justice? To see the Lich King destroyed?',17394,1,0,0,'terenas SAY_FM_TERENAS_AID_1'),
(-1631169,'First, you must escape Frostmourne\'s hold, or be damned as I am; trapped within this cursed blade for all eternity.',17395,1,0,0,'terenas SAY_FM_TERENAS_AID_2'),
(-1631170,'Aid me in destroying these tortured souls! Together we will loosen Frostmourne\'s hold and weaken the Lich King from within!',17396,1,0,0,'terenas SAY_FM_TERENAS_AID_3'),
(-1631171,'Argh... Frostmourne, obey me!',17367,1,0,0,'lich_king SAY_FM_PLAYER_ESCAPE'),
(-1631172,'Frostmourne feeds on the soul of your fallen ally!',17368,1,0,0,'lich_king SAY_FM_PLAYER_DEATH'),
(-1631173,'Apocalypse!',17371,1,0,0,'lich_king SAY_SPECIAL_1'),
(-1631174,'Bow down before your lord and master!',17372,1,0,0,'lich_king SAY_SPECIAL_2'),
(-1631175,'You gnats actually hurt me! Perhaps I\'ve toyed with you long enough, now taste the vengeance of the grave!',17359,1,0,0,'lich_king SAY_LAST_PHASE'),
(-1631176,'Hope wanes!',17363,1,0,0,'lich_king SAY_SLAY_1'),
(-1631177,'The end has come!',17364,1,0,0,'lich_king SAY_SLAY_2'),
(-1631178,'Face now your tragic end!',17365,1,0,0,'lich_king SAY_ENRAGE'),
(-1631179,'No question remains unanswered. No doubts linger. You are Azeroth\'s greatest champions! You overcame every challenge I laid before you. My mightiest servants have fallen before your relentless onslaught, your unbridled fury...',17353,1,0,0,'lich_king SAY_OUTRO_1'),
(-1631180,'Is it truly righteousness that drives you? I wonder',17354,1,0,0,'lich_king SAY_OUTRO_2'),
(-1631181,'You trained them well, Fordring. You delivered the greatest fighting force this world has ever known... right into my hands -- exactly as I intended. You shall be rewarded for your unwitting sacrifice.',17355,1,0,0,'lich_king SAY_OUTRO_3'),
(-1631182,'Watch now as I raise them from the dead to become masters of the Scourge. They will shroud this world in chaos and destruction. Azeroth\'s fall will come at their hands -- and you will be the first to die.',17356,1,0,0,'lich_king SAY_OUTRO_4'),
(-1631183,'I delight in the irony.',17357,1,0,0,'lich_king SAY_OUTRO_5'),
(-1631184,'LIGHT, GRANT ME ONE FINAL BLESSING. GIVE ME THE STRENGTH... TO SHATTER THESE BONDS!',17392,1,0,0,'tirion SAY_OUTRO_6'),
(-1631185,'Impossible...',17358,1,0,0,'lich_king SAY_OUTRO_7'),
(-1631186,'No more, Arthas! No more lives will be consumed by your hatred!',17393,1,0,0,'tirion SAY_OUTRO_8'),
(-1631187,'Free at last! It is over, my son. This is the moment of reckoning.',17397,1,0,0,'terenas SAY_OUTRO_9'),
(-1631188,'Rise up, champions of the Light!',17398,1,0,0,'terenas SAY_OUTRO_10'),
(-1631189,'THE LICH KING...MUST...FALL!',17389,1,0,0,'tirion SAY_OUTRO_11'),
(-1631190,'Now I stand, the lion before the lambs... and they do not fear.',17361,1,0,0,'lich_king SAY_OUTRO_12'),
(-1631191,'They cannot fear.',17362,1,0,0,'lich_king SAY_OUTRO_13'),
(-1631192,'%s dies',17374,2,0,0,'lich_king SAY_OUTRO_14'), -- TODO Can be wrong

(-1631193,'%s goes into a frenzy!',0,3,0,0,'saurfang EMOTE_FRENZY'),
(-1631194,'%s\'s Blood Beasts gain the scent of blood!',0,3,0,0,'saurfang EMOTE_SCENT'),
(-1631195,'Really... Is that all you got?',16791,1,0,0,'blood_queen SAY_SLAY_1'),
(-1631196,'Such a pity...',16792,1,0,0,'blood_queen SAY_SLAY_2'),

(-1631197,'Invocation of Blood jumps to %s!',0,3,0,0,'blood_princes EMOTE_INVOCATION'),
(-1631198,'%s begins casting Empowered Shock Vortex!',0,3,0,0,'valanar EMOTE_SHOCK_VORTEX'),
(-1631199,'%s speed toward $N!',0,3,0,0,'taldaram EMOTE_FLAMES'),

(-1631200,'Terrible news everyone. Rotface is dead, but great news everyone! He left behind plenty of ooze for me to use! What? I\'m a poet and I didn\'t know it.... Astounding!',17146,1,0,0,'festergut SAY_BLIGHT_2'),
(-1631201,'%s releases Gas Spores!',0,3,0,0,'festergut EMOTE_SPORES'),
(-1631202,'%s begins to cast Slime Spray!',0,3,0,0,'rotface EMOTE_SLIME_SPRAY'),
(-1631203,'%s grows more unstable!',0,2,0,0,'rotface EMOTE_OOZE_GROW_1'),
(-1631204,'%s is growing volatile!',0,2,0,0,'rotface EMOTE_OOZE_GROW_2'),
(-1631205,'%s can barely maintain its form!',0,2,0,0,'rotface EMOTE_OOZE_GROW_3'),
(-1631206,'%s begins to cast Unstable Experiment!',0,3,0,0,'putricide EMOTE_EXPERIMENT');

-- -1 632 000 ICC: FORGE OF SOULS
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1632000,'More souls to power the engine!',0,1,0,0,'boss_bronjahm SAY_AGGRO_1'),
(-1632001,'Finally...a captive audience!',16595,1,0,0,'boss_bronjahm SAY_AGGRO_2'),
(-1632002,'Fodder for the engine!',16596,1,0,0,'boss_bronjahm SAY_SLAY_1'),
(-1632003,'Another soul to strengthen the host!',16597,1,0,0,'boss_bronjahm SAY_SLAY_2'),
(-1632004,'My soul for you, master.',16598,1,0,0,'boss_bronjahm SAY_DEATH'),
(-1632005,'The vortex of the harvested calls to you!',16599,1,0,0,'boss_bronjahm SAY_SOULSTORM'),
(-1632006,'I will sever the soul from your body!',16600,1,0,0,'boss_bronjahm SAY_CORRUPT_SOUL'),

(-1632007,'You dare look upon the host of souls?! I SHALL DEVOUR YOU WHOLE!',16884,1,0,0,'boss_devourer SAY_MALE_1_AGGRO'),
(-1632008,'You dare look upon the host of souls?! I SHALL DEVOUR YOU WHOLE!',16890,1,0,0,'boss_devourer SAY_FEMALE_AGGRO'),
(-1632009,'Damnation!',16885,1,0,0,'boss_devourer SAY_MALE_1_SLAY_1'),
(-1632010,'Damnation!',16891,1,0,0,'boss_devourer SAY_FEMALE_SLAY_1'),
(-1632011,'Damnation!',16896,1,0,0,'boss_devourer SAY_MALE_2_SLAY_1'),
(-1632012,'Doomed for eternity!',16886,1,0,0,'boss_devourer SAY_MALE_1_SLAY_2'),
(-1632013,'Doomed for eternity!',16892,1,0,0,'boss_devourer SAY_FEMALE_SLAY_2'),
(-1632014,'Doomed for eternity!',16897,1,0,0,'boss_devourer SAY_MALE_2_SLAY_2'),
(-1632015,'The swell of souls will not be abated! You only delay the inevitable!',16887,1,0,0,'boss_devourer SAY_MALE_1_DEATH'),
(-1632016,'The swell of souls will not be abated! You only delay the inevitable!',16893,1,0,0,'boss_devourer SAY_FEMALE_DEATH'),
(-1632017,'The swell of souls will not be abated! You only delay the inevitable!',16898,1,0,0,'boss_devourer SAY_MALE_2_DEATH'),
(-1632018,'SUFFERING! ANGUISH! CHAOS! RISE AND FEED!',16888,1,0,0,'boss_devourer SAY_MALE_1_SOUL_ATTACK'),
(-1632019,'SUFFERING! ANGUISH! CHAOS! RISE AND FEED!',16894,1,0,0,'boss_devourer SAY_FEMALE_SOUL_ATTACK'),
(-1632020,'SUFFERING! ANGUISH! CHAOS! RISE AND FEED!',16899,1,0,0,'boss_devourer SAY_MALE_2_SOUL_ATTACK'),
(-1632021,'Stare into the abyss, and see your end!',16889,1,0,0,'boss_devourer SAY_MALE_1_DARK_GLARE'),
(-1632022,'Stare into the abyss, and see your end!',16895,1,0,0,'boss_devourer SAY_FEMALE_DARK_GLARE'),
(-1632023,'%s begins to cast Mirrored Soul!',0,3,0,0,'boss_devourer EMOTE_MIRRORED_SOUL'),
(-1632024,'%s begins to Unleash Souls!',0,3,0,0,'boss_devourer EMOTE_UNLEASH_SOULS'),
(-1632025,'%s begins to cast Wailing Souls!',0,3,0,0,'boss_devourer EMOTE_WAILING_SOULS');

-- -1 649 000 TRIAL OF THE CRUSADER
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1649000,'Welcome champions, you have heard the call of the argent crusade and you have boldly answered. It is here in the crusaders coliseum that you will face your greatest challenges. Those of you who survive the rigors of the coliseum will join the Argent Crusade on it\'s marsh to ice crown citadel.',16036,1,0,0,'tirion SAY_TIRION_RAID_INTRO_LONG'),
(-1649001,'Welcome to the trials of the crusader. Only the most powerful combatants of azeroth are allowed to undergo these trials. You are among the worthy few.',16053,1,0,0,'tirion SAY_RAID_TRIALS_INTRO'),

(-1649002,'Hailing from the deepest, darkest carverns of the storm peaks, Gormok the Impaler! Battle on, heroes!',16038,1,0,0,'tirion SAY_TIRION_BEAST_1'),
(-1649003,'Your beast will be no match for my champions Tirion.',16069,1,0,1,'varian SAY_VARIAN_BEAST_1'),
(-1649004,'I have seen more worthy challenges in the ring of blood, you waste our time paladin.',16026,1,0,1,'garrosh SAY_GARROSH_BEAST_1'),
(-1649005,'Steel yourselves, heroes, for the twin terrors Acidmaw and Dreadscale. Enter the arena!',16039,1,0,0,'tirion SAY_TIRION_BEAST_2'),
(-1649006,'The air freezes with the introduction of our next combatant, Icehowl! Kill or be killed, champions!',16040,1,0,0,'tirion SAY_TIRION_BEAST_3'),
(-1649007,'The monstrous menagerie has been vanquished!',16041,1,0,0,'tirion SAY_TIRION_BEAST_SLAY'),
(-1649008,'Tragic... They fought valiantly, but the beasts of Northrend triumphed. Let us observe a moment of silence for our fallen heroes.',16042,1,0,0,'tirion SAY_TIRION_BEAST_WIPE'),

(-1649009,'Grand Warlock Wilfred Fizzlebang will summon forth your next challenge. Stand by for his entry!',16043,1,0,0,'tirion SAY_TIRION_JARAXXUS_INTRO_1'),
(-1649010,'Thank you, Highlord. Now, challengers, I will begin the ritual of summoning. When I am done a fearsome doomguard will appear!',16268,1,0,2,'wilfred SAY_WILFRED_JARAXXUS_INTRO_1'),
(-1649011,'Prepare for oblivion!',16269,1,0,0,'wilfred SAY_WILFRED_JARAXXUS_INTRO_2'),
(-1649012,'Ah ha! Behold the absolute power of Wilfred Fizzlebang, master summoner! You are bound to ME, demon!',16270,1,0,5,'wilfred SAY_WILFRED_JARAXXUS_INTRO_3'),
(-1649013,'Trifling gnome, your arrogance will be your undoing!',16143,1,0,397,'jaraxxus SAY_JARAXXUS_JARAXXAS_INTRO_1'),
(-1649014,'But I\'m in charge her-',16271,1,0,5,'wilfred SAY_WILFRED_DEATH'),
(-1649015,'Quickly, heroes! Destroy the demon lord before it can open a portal to its twisted demonic realm!',16044,1,0,5,'tirion SAY_TIRION_JARAXXUS_INTRO_2'),
(-1649016,'The loss of Wilfred Fizzlebang, while unfortunate, should be a lesson to those that dare dabble in dark magic. Alas, you are victorious and must now face the next challenge.',16045,1,0,0,'tirion SAY_TIRION_JARAXXUS_EXIT_1'),
(-1649017,'Treacherous Alliance dogs! You summon a demon lord against warriors of the Horde!? Your deaths will be swift!',16021,1,0,5,'garrosh SAY_GARROSH_JARAXXUS_EXIT_1'),
(-1649018,'The Alliance doesn\'t need the help of a demon lord to deal with Horde filth. Come, pig!',16064,1,0,5,'varian SAY_VARIAN_JARAXXUS_SLAY'),
(-1649019,'Everyone, calm down! Compose yourselves! There is no conspiracy at play here. The warlock acted on his own volition - outside of influences from the Alliance. The tournament must go on!',16046,1,0,5,'tirion SAY_TIRION_JARAXXUS_EXIT_2'),

(-1649020,'The next battle will be against the Argent Crusade\'s most powerful knights! Only by defeating them will you be deemed worthy...',16047,1,0,0,'tirion SAY_TIRION_PVP_INTRO_1'),
(-1649021,'The Horde demands justice! We challenge the Alliance. Allow us to battle in place of your knights, paladin. We will show these dogs what it means to insult the Horde!',16023,1,0,5,'garrosh SAY_GARROSH_PVP_A_INTRO_1'),
(-1649022,'Our honor has been besmirched! They make wild claims and false accusations against us. I demand justice! Allow my champions to fight in place of your knights, Tirion. We challenge the Horde!',16066,1,0,5,'varian SAY_VARIAN_PVP_H_INTRO_1'),
(-1649023,'Very well, I will allow it. Fight with honor!',16048,1,0,1,'tirion SAY_TIRION_PVP_INTRO_2'),
(-1649024,'Fight for the glory of the Alliance, heroes! Honor your king and your people!',16065,1,0,5,'varian SAY_VARIAN_PVP_H_INTRO_2'),
(-1649025,'Show them no mercy, Horde champions! LOK\'TAR OGAR!',16022,1,0,1,'garrosh SAY_GARROSH_PVP_A_INTRO_2'),
(-1649026,'GLORY TO THE ALLIANCE!',16067,1,0,5,'varian SAY_VARIAN_PVP_A_WIN'),
(-1649027,'That was just a taste of what the future brings. FOR THE HORDE!',16024,1,0,1,'garrosh SAY_GARROSH_PVP_H_WIN'),
(-1649028,'A shallow and tragic victory. We are weaker as a whole from the losses suffered today. Who but the Lich King could benefit from such foolishness? Great warriors have lost their lives. And for what? The true threat looms ahead - the Lich King awaits us all in death.',16049,1,0,0,'tirion SAY_TIRION_PVP_WIN'),

(-1649029,'Only by working together will you overcome the final challenge. From the depths of Icecrown come two of the Scourge\'s most powerful lieutenants: fearsome val\'kyr, winged harbingers of the Lich King!',16050,1,0,0,'tirion SAY_TIRION_TWINS_INTRO'),
(-1649030,'Let the games begin!',16037,1,0,0,'tirion SAY_RAID_INTRO_SHORT'),
(-1649031,'Not even the lich king\'s most powerful minions could stand against the alliance. All hail our victors.',16068,1,0,1,'varian SAY_VARIAN_TWINS_A_WIN'),
(-1649032,'Do you still question the might of the Horde, paladin? We will take on all comers!',16025,1,0,0,'garrosh SAY_GARROSH_TWINS_H_WIN'),
(-1649033,'A mighty blow has been dealt to the Lich King! You have proven yourselves able bodied champions of the Argent Crusade. Together we will strike at Icecrown Citadel and destroy what remains of the Scourge! There is no challenge that we cannot face united!',16051,1,0,5,'tirion SAY_TIRION_TWINS_WIN'),

(-1649034,'You will have your challenge, Fordring.',16321,1,0,0,'lich_king SAY_LKING_ANUB_INTRO_1'),
(-1649035,'Arthas! You are hopelessly outnumbered! Lay down Frostmourne and I will grant you a just death.',16052,1,0,25,'tirion SAY_TIRION_ABUN_INTRO_1'),
(-1649036,'The Nerubians built an empire beneath the frozen wastes of Northrend. An empire that you so foolishly built your structures upon. MY EMPIRE.',16322,1,0,11,'lich_king SAY_LKING_ANUB_INTRO_2'),
(-1649037,'The souls of your fallen champions will be mine, Fordring.',16323,1,0,0,'lich_king SAY_LKING_ANUB_INTRO_3'),
(-1649038,'Ahhh, our guests have arrived, just as the master promised.',16235,1,0,0,'anubarak SAY_ANUB_ANUB_INTRO_1'),

(-1649039,'%s glares at $N and lets out a bellowing roar!',0,3,0,0,'icehowl EMOTE_MASSIVE_CRASH'),

(-1649040,'You face Jaraxxus, eredar lord of the Burning Legion!',16144,1,0,0,'jaraxxus SAY_AGGRO'),
(-1649041,'Insignificant gnat!',16145,1,0,0,'jaraxxus SAY_SLAY_1'),
(-1649042,'Banished to the Nether!',16146,1,0,0,'jaraxxus SAY_SLAY_2'),
(-1649043,'Another will take my place. Your world is doomed.',16147,1,0,0,'jaraxxus SAY_DEATH'),
(-1649044,'<SD2 Text missing>',16148,1,0,0,'jaraxxus SAY_BERSERK'), -- TODO, just some laughing
(-1649045,'Flesh from bone!',16149,1,0,0,'jaraxxus SAY_INCINERATE'),
(-1649046,'Come forth, sister! Your master calls!',16150,1,0,0,'jaraxxus SAY_MISTRESS'),
(-1649047,'Inferno!',16151,1,0,0,'jaraxxus SAY_INFERNO'),

(-1649048,'Weakling!',16017,1,0,0,'garrosh SAY_GARROSH_PVP_A_SLAY_1'),
(-1649049,'Pathetic!',16018,1,0,274,'garrosh SAY_GARROSH_PVP_A_SLAY_2'),
(-1649050,'Overpowered.',16019,1,0,25,'garrosh SAY_GARROSH_PVP_A_SLAY_3'),
(-1649051,'Lok\'tar!',16020,1,0,5,'garrosh SAY_GARROSH_PVP_A_SLAY_4'),
(-1649052,'Hah!',16060,1,0,5,'varian SAY_VARIAN_PVP_H_SLAY_1'),
(-1649053,'Hardly a challenge!',16061,1,0,274,'varian SAY_VARIAN_PVP_H_SLAY_2'),
(-1649054,'Worthless scrub.',16062,1,0,25,'varian SAY_VARIAN_PVP_H_SLAY_3'),
(-1649055,'Is this the best the Horde has to offer?',16063,1,0,6,'varian SAY_VARIAN_PVP_H_SLAY_4'),

(-1649056,'In the name of our dark master. For the Lich King. You. Will. Die.',16272,1,0,0,'twin_valkyr SAY_AGGRO'),
(-1649057,'You are finished!',16273,1,0,0,'twin_valkyr SAY_BERSERK'),
(-1649058,'Chaos!',16274,1,0,0,'twin_valkyr SAY_COLORSWITCH'),
(-1649059,'The Scourge cannot be stopped...',16275,1,0,0,'twin_valkyr SAY_DEATH'),
(-1649060,'You have been measured, and found wanting!',16276,1,0,0,'twin_valkyr SAY_SLAY_1'),
(-1649061,'Unworthy!',16277,1,0,0,'twin_valkyr SAY_SLAY_2'),
(-1649062,'Let the dark consume you!',16278,1,0,0,'twin_valkyr SAY_TO_BLACK'),
(-1649063,'Let the light consume you!',16279,1,0,0,'twin_valkyr SAY_TO_WHITE'),

(-1649064,'This place will serve as your tomb!',16234,1,0,0,'anubarak SAY_AGGRO'),
(-1649065,'F-lakkh shir!',16236,1,0,0,'anubarak SAY_SLAY_1'),
(-1649066,'Another soul to sate the host.',16237,1,0,0,'anubarak SAY_SLAY_2'),
(-1649067,'I have failed you, master...',16238,1,0,0,'anubarak SAY_DEATH'),
(-1649068,'<SD2 Text missing>',16239,1,0,0,'anubarak SAY_BERSERK'),
(-1649069,'Auum na-l ak-k-k-k, isshhh. Rise, minions. Devour...',16240,1,0,0,'anubarak SAY_SUBMERGE'),
(-1649070,'The swarm shall overtake you!',16241,1,0,0,'anubarak SAY_LEECHING_SWARM'),

(-1649071,'%s burrows into the ground!',0,3,0,0,'anubarak EMOTE_BURROW'),
(-1649072,'%s spikes pursue $N!',0,3,0,0,'anubarak EMOTE_PURSUE'),
(-1649073,'%s emerges from the ground!',0,3,0,0,'anubarak EMOTE_EMERGE'),
(-1649074,'%s unleashes a Leeching Swarm to heal himself!',0,3,0,0,'anubarak EMOTE_SWARM'),

(-1649075,'Champions, you\'re alive! Not only have you defeated every challenge of the Trial of the Crusader, but also thwarted Arthas\' plans! Your skill and cunning will prove to be a powerful weapon against the Scourge. Well done! Allow one of the Crusade\'s mages to transport you to the surface!',0,0,0,1,'tirion SAY_EPILOGUE'),

(-1649076,'As its companion perishes, %s becomes enraged!',0,3,0,0,'twin jormungars EMOTE_JORMUNGAR_ENRAGE'),
(-1649077,'%s crashes into the Coliseum wall and is stunned!',0,3,0,0,'icehowl EMOTE_WALL_CRASH');

-- -1 650 000 TRIAL OF THE CHAMPION
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1650000,'The Silver Covenant is pleased to present their contenders for this event, Highlord.',0,1,0,396,'toc herald SAY_HORDE_CHALLENGE'),
(-1650001,'Presenting the fierce Grand Champion of Orgrimmar, Mokra the Skullcrusher!',0,0,0,0,'toc herald SAY_HORDE_WARRIOR'),
(-1650002,'Coming out of the gate is Eressea Dawnsinger, skilled mage and Grand Champion of Silvermoon!',0,0,0,0,'toc herald SAY_HORDE_MAGE'),
(-1650003,'Tall in the saddle of his kodo, here is the venerable Runok Wildmane, Grand Champion of Thunder Bluff!',0,0,0,0,'toc herald SAY_HORDE_SHAMAN'),
(-1650004,'Entering the arena is the lean and dangerous Zul\'tore, Grand Champion of Sen\'jin!',0,0,0,0,'toc herald SAY_HORDE_HUNTER'),
(-1650005,'Representing the tenacity of the Forsaken, here is the Grand Champion of the Undercity, Deathstalker Visceri!',0,0,0,0,'toc herald SAY_HORDE_ROGUE'),

(-1650006,'The Sunreavers are proud to present their representatives in this trial by combat.',0,1,0,396,'toc herald SAY_ALLIANCE_CHALLENGE'),
(-1650007,'Proud and strong, give a cheer for Marshal Jacob Alerius, the Grand Champion of Stormwind!',0,0,0,0,'toc herald SAY_ALLIANCE_WARRIOR'),
(-1650008,'Here comes the small but deadly Ambrose Boltspark, Grand Champion of Gnomeregan!',0,0,0,0,'toc herald SAY_ALLIANCE_MAGE'),
(-1650009,'Coming out of the gate is Colosos, the towering Grand Champion of the Exodar!',0,0,0,0,'toc herald SAY_ALLIANCE_SHAMAN'),
(-1650010,'Entering the arena is the Grand Champion of Darnassus, the skilled sentinel Jaelyne Evensong!',0,0,0,0,'toc herald SAY_ALLIANCE_HUNTER'),
(-1650011,'The might of the dwarves is represented today by the Grand Champion of Ironforge, Lana Stouthammer!',0,0,0,0,'toc herald SAY_ALLIANCE_ROGUE'),

(-1650012,'Welcome, champions. Today, before the eyes of your leaders and peers, you will prove yourselves worthy combatants.',0,1,0,1,'tirion SAY_TIRION_WELCOME'),
(-1650013,'You will first be facing three of the Grand Champions of the Tournament! These fierce contenders have beaten out all others to reach the pinnacle of skill in the joust.',0,1,0,1,'tirion SAY_TIRION_FIRST_CHALLENGE'),
(-1650014,'Fight well, Horde! Lok\'tar Ogar!',0,1,0,22,'thrall SAY_THRALL_ALLIANCE_CHALLENGE'),
(-1650015,'Finally, a fight worth watching.',0,1,0,396,'garrosh SAY_GARROSH_ALLIANCE_CHALLENGE'),
(-1650016,'I have no taste for these games, Tirion. Still... I trust they will perform admirably.',0,1,0,1,'king varian SAY_VARIAN_HORDE_CHALLENGE'),
(-1650017,'Begin!',0,1,0,0,'tirion SAY_TIRION_CHAMPIONS_BEGIN'),
(-1650018,'The blood elves of Silvermoon cheer for $n.',0,2,0,0,'raid spectator EMOTE_BLOOD_ELVES'),
(-1650019,'The trolls of the Sen\'jin Village begin a chant to celebrate $n.',0,2,0,0,'raid spectator EMOTE_TROLLS'),
(-1650020,'The tauren of Thunder Bluff cheer for $n.',0,2,0,0,'raid spectator EMOTE_TAUREN'),
(-1650021,'The forsaken of the Undercity cheer for $n.',0,2,0,0,'raid spectator EMOTE_UNDEAD'),
(-1650022,'The orcs of Orgrimmar cheer for $n.',0,2,0,0,'raid spectator EMOTE_ORCS'),
(-1650023,'The dwarves of Ironforge begin a cheer for $n.',0,2,0,0,'raid spectator EMOTE_BLOOD_DWARVES'),
(-1650024,'The gnomes of Gnomeregan cheer for $n.',0,2,0,0,'raid spectator EMOTE_GNOMES'),
(-1650025,'The night elves of Darnassus cheer for $n.',0,2,0,0,'raid spectator EMOTE_NIGHT_ELVES'),
(-1650026,'The humans of Stormwind cheer for $n.',0,2,0,0,'raid spectator EMOTE_HUMANS'),
(-1650027,'The draenei of the Exodar cheer for $n.',0,2,0,0,'raid spectator EMOTE_DRAENEI'),

(-1650028,'Well fought! Your next challenge comes from the Crusade\'s own ranks. You will be tested against their considerable prowess.',0,1,0,0,'tirion SAY_TIRION_ARGENT_CHAMPION'),
(-1650029,'You may begin!',0,1,0,22,'tirion SAY_TIRION_ARGENT_CHAMPION_BEGIN'),
(-1650030,'Entering the arena, a paladin who is no stranger to the battlefield or tournament ground, the Grand Champion of the Argent Crusade, Eadric the Pure!',0,1,0,0,'toc herald SAY_EADRIC'),
(-1650031,'The next combatant is second to none in her passion for upholding the Light. I give you Argent Confessor Paletress!',0,1,0,0,'toc herald SAY_PALETRESS'),
(-1650032,'The Horde spectators cheer for $n.',0,2,0,0,'raid spectator EMOTE_HORDE_ARGENT_CHAMPION'),
(-1650033,'The Alliance spectators cheer for $n.',0,2,0,0,'raid spectator EMOTE_ALLIANCE_ARGENT_CHAMPION'),
(-1650034,'Are you up to the challenge? I will not hold back.',16134,0,0,397,'eadric SAY_EADRIC_INTRO'),
(-1650035,'Thank you, good herald. Your words are too kind.',16245,0,0,2,'paletress SAY_PALETRESS_INTRO_1'),
(-1650036,'May the Light give me strength to provide a worthy challenge.',16246,0,0,16,'paletress SAY_PALETRESS_INTRO_2'),

(-1650037,'Well done. You have proven yourself today-',0,1,0,0,'tirion SAY_ARGENT_CHAMPION_COMPLETE'),
(-1650038,'What\'s that, up near the rafters?',0,0,0,25,'toc herald SAY_BLACK_KNIGHT_SPAWN'),
(-1650039,'You spoiled my grand entrance, rat.',16256,0,0,0,'black knight SAY_BLACK_KNIGHT_INTRO_1'),
(-1650040,'What is the meaning of this?',0,1,0,0,'tirion SAY_TIRION_BLACK_KNIGHT_INTRO_2'),
(-1650041,'Did you honestly think an agent of the Lich King would be bested on the field of your pathetic little tournament?',16257,0,0,396,'black knight SAY_BLACK_KNIGHT_INTRO_3'),
(-1650042,'I\'ve come to finish my task.',16258,0,0,396,'black knight SAY_BLACK_KNIGHT_INTRO_4'),

(-1650043,'My congratulations, champions. Through trials both planned and unexpected, you have triumphed.',0,1,0,0,'tirion SAY_EPILOG_1'),
(-1650044,'Go now and rest; you\'ve earned it.',0,1,0,0,'tirion SAY_EPILOG_2'),
(-1650045,'You fought well.',0,1,0,66,'king varian SAY_VARIAN_EPILOG_3'),
(-1650046,'Well done, Horde!',0,1,0,66,'thrall SAY_THRALL_HORDE_EPILOG_3'),

(-1650047,'Tear him apart!',0,1,0,22,'garrosh SAY_GARROSH_OTHER_1'),
(-1650048,'Garrosh, enough.',0,1,0,396,'thrall SAY_THRALL_OTHER_2'),
(-1650049,'Admirably? Hah! I will enjoy watching your weak little champions fail, human.',0,1,0,22,'garrosh SAY_GARROSH_OTHER_3'),
(-1650050,'Don\'t just stand there; kill him!',0,1,0,22,'king varian SAY_VARIAN_OTHER_4'),
(-1650051,'I did not come here to watch animals tear at each other senselessly, Tirion.',0,1,0,1,'king varian SAY_VARIAN_OTHER_5'),

(-1650052,'Prepare yourselves!',16135,1,0,0,'eadric SAY_AGGRO'),
(-1650053,'Hammer of the Righteous!',16136,1,0,0,'eadric SAY_HAMMER'),
(-1650054,'You... You need more practice.',16137,1,0,0,'eadric SAY_KILL_1'),
(-1650055,'Nay! Nay! And I say yet again nay! Not good enough!',16138,1,0,0,'eadric SAY_KILL_2'),
(-1650056,'I yield! I submit. Excellent work. May I run away now?',16139,1,0,0,'eadric SAY_DEFEAT'),
(-1650057,'%s begins to radiate light. Shield your eyes!',0,3,0,0,'eadric EMOTE_RADIATE'),
(-1650058,'%s targets $N with the Hammer of the Righteous!',0,3,0,0,'eadric EMOTE_HAMMER'),

(-1650059,'Well then, let us begin.',16247,1,0,0,'paletress SAY_AGGRO'),
(-1650060,'Take this time to consider your past deeds.',16248,1,0,0,'paletress SAY_MEMORY'),
(-1650061,'Even the darkest memory fades when confronted.',16249,1,0,0,'paletress SAY_MEMORY_DIES'),
(-1650062,'Take your rest.',16250,1,0,0,'paletress SAY_KILL_1'),
(-1650063,'Be at ease.',16251,1,0,0,'paletress SAY_KILL_2'),
(-1650064,'Excellent work!',16252,1,0,0,'paletress SAY_DEFEAT'),

(-1650065,'This farce ends here!',16259,1,0,0,'black knight SAY_AGGRO'),
(-1650066,'My rotting flesh was just getting in the way!',16262,1,0,0,'black knight SAY_PHASE_2'),
(-1650067,'I have no need for bones to best you!',16263,1,0,0,'black knight SAY_PHASE_3'),
(-1650068,'A waste of flesh.',16260,1,0,0,'black knight SAY_KILL_1'),
(-1650069,'Pathetic.',16261,1,0,0,'black knight SAY_KILL_2'),
(-1650070,'No! I must not fail... again...',16264,1,0,0,'black knight SAY_DEATH');

-- -1 658 000 ICC: PIT OF SARON
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1658001,'Intruders have entered the masters domain. Signal the alarms!',16747,1,0,0,'tyrannus SAY_TYRANNUS_INTRO_1'),
(-1658002,'Heroes of the Alliance, attack!',16626,1,0,0,'jaina SAY_JAINA_INTRO_1'),
(-1658003,'Soldiers of the Horde, attack!',17045,1,0,0,'sylvanas SAY_SYLVANAS_INTRO_1'),
(-1658004,'Hrmph, fodder. Not even fit to labor in the quarry. Relish these final moments for soon you will be nothing more than mindless undead.',16748,1,0,0,'tyrannus SAY_TYRANNUS_INTRO_2'),
(-1658005,'Your last waking memory will be of agonizing pain.',16749,1,0,0,'tyrannus SAY_TYRANNUS_INTRO_3'),
(-1658006,'No! You monster!',16627,1,0,0,'jaina SAY_JAINA_INTRO_2'),
(-1658007,'Pathetic weaklings!',17046,1,0,1,'sylvanas SAY_SYLVANAS_INTRO_2'),
(-1658008,'Minions, destroy these interlopers!',16751,1,0,0,'tyrannus SAY_TYRANNUS_INTRO_4'),
(-1658009,'I do what I must. Please forgive me, noble soldiers.',16628,1,0,0,'jaina SAY_JAINA_INTRO_3'),
(-1658010,'You will have to make your way across this quarry on your own.',16629,0,0,1,'jaina SAY_JAINA_INTRO_4'),
(-1658011,'You will have to battle your way through this cesspit on your own.',17047,0,0,1,'sylvanas SAY_SYLVANAS_INTRO_3'),
(-1658012,'Free any Alliance slaves that you come across. We will most certainly need their assistance in battling Tyrannus. I will gather reinforcements and join you on the other side of the quarry.',16630,0,0,0,'jaina SAY_JAINA_INTRO_5'),
(-1658013,'Free any Horde slaves that you come across. We will most certainly need their assistance in battling Tyrannus. I will gather reinforcements and join you on the other side of the quarry.',17048,0,0,0,'sylvanas SAY_SYLVANAS_INTRO_4'),

(-1658014,'Tiny creatures under feet, you bring Garfrost something good to eat!',16912,1,0,0,'garfrost SAY_AGGRO'),
(-1658015,'Will save for snack. For later.',16913,1,0,0,'garfrost SAY_SLAY_1'),
(-1658016,'That one maybe not so good to eat now. Stupid Garfrost! BAD! BAD!',16914,1,0,0,'garfrost SAY_BOULDER_HIT'),
(-1658017,'Garfrost hope giant underpants clean. Save boss great shame. For later.',16915,1,0,0,'garfrost SAY_DEATH'),
(-1658018,'Axe too weak. Garfrost make better and CRUSH YOU!',16916,1,0,0,'garfrost SAY_FORGE_1'),
(-1658019,'That one maybe not so good to eat now. Stupid Garfrost! BAD! BAD!',16917,1,0,0,'garfrost SAY_FORGE_2'),
(-1658020,'Another shall take his place. You waste your time.',16752,6,0,0,'tyrannus SAY_TYRANNUS_GARFROST'),
(-1658021,'The forgemaster is dead! Get geared up men, we have a Scourgelord to kill.',0,1,0,0,'victus_or_ironskull SAY_GENERAL_GARFROST'),
(-1658022,'%s hurls a massive saronite boulder at you!',0,5,0,0,'garfrost EMOTE_THROW_SARONITE'), -- TODO emote only displayed to target
(-1658023,'%s casts Deep Freeze at $N.',0,3,0,0,'garfrost EMOTE_DEEP_FREEZE'),

(-1658024,'Our work must not be interrupted! Ick! Take care of them!',16926,1,0,0,'krick SAY_AGGRO'),
(-1658025,'Ooh...We could probably use these parts!',16927,1,0,0,'krick SAY_SLAY_1'),
(-1658026,'Arms and legs are in short supply...Thanks for your contribution!',16928,1,0,0,'krick SAY_SLAY_2'),
(-1658027,'Enough moving around! Hold still while I blow them all up!',16929,1,0,0,'krick SAY_ORDER_STOP'),
(-1658028,'Quickly! Poison them all while they\'re still close!',16930,1,0,0,'krick SAY_ORDER_BLOW'),
(-1658029,'No! That one! That one! Get that one!',16931,1,0,0,'krick SAY_TARGET_1'),
(-1658030,'I\'ve changed my mind...go get that one instead!',16932,1,0,0,'krick SAY_TARGET_2'),
(-1658031,'What are you attacking him for? The dangerous one is over there,fool!',16933,1,0,0,'krick SAY_TARGET_3'),
(-1658032,'%s begins rapidly conjuring explosive mines!',0,3,0,0,'krick EMOTE_KRICK_MINES'),
(-1658033,'%s begins to unleash a toxic poison cloud!',0,3,0,0,'ick EMOTE_ICK_POISON'),
(-1658034,'%s is chasing you!',0,5,0,0,'ick EMOTE_ICK_CHASING'), -- TODO emote type?

(-1658035,'Wait! Stop! Don\'t kill me, please! I\'ll tell you everything!',16934,1,0,431,'krick SAY_OUTRO_1'),
(-1658036,'I\'m not so naive as to believe your appeal for clemency, but I will listen.',16611,1,0,0,'jaina SAY_JAINA_KRICK_1'),
(-1658037,'Why should the Banshee Queen spare your miserable life?',17033,1,0,396,'sylvanas SAY_SYLVANAS_KRICK_1'),
(-1658038,'What you seek is in the master\'s lair, but you must destroy Tyrannus to gain entry. Within the Halls of Reflection you will find Frostmourne. It... it holds the truth.',16935,1,0,0,'krick SAY_OUTRO_2'),
(-1658039,'Frostmourne lies unguarded? Impossible!',16612,1,0,0,'jaina SAY_JAINA_KRICK_2'),
(-1658040,'Frostmourne? The Lich King is never without his blade! If you are lying to me...',17034,1,0,15,'sylvanas SAY_SYLVANAS_KRICK_2'),
(-1658041,'I swear it is true! Please, don\'t kill me!!',16936,1,0,0,'krick SAY_OUTRO_3'),
(-1658042,'Worthless gnat! Death is all that awaits you!',16753,1,0,0,'tyrannus SAY_TYRANNUS_KRICK_1'),
(-1658043,'Urg... no!!',16937,1,0,0,'krick SAY_OUTRO_4'),
(-1658044,'Do not think that I shall permit you entry into my master\'s sanctum so easily. Pursue me if you dare.',16754,1,0,0,'tyrannus SAY_TYRANNUS_KRICK_2'),
(-1658045,'What a cruel end. Come, heroes. We must see if the gnome\'s story is true. If we can separate Arthas from Frostmourne, we might have a chance at stopping him.',16613,1,0,0,'jaina SAY_JAINA_KRICK_3'),
(-1658046,'A fitting end for a traitor. Come, we must free the slaves and see what is within the Lich King\'s chamber for ourselves.',17035,1,0,396,'sylvanas SAY_SYLVANAS_KRICK_3'),

(-1658047,'Your pursuit shall be in vain, adventurers, for the Lich King has placed an army of undead at my command! Behold!',16755,6,0,0,'tyrannus SAY_TYRANNUS_AMBUSH_1'),
(-1658048,'Persistent whelps! You will not reach the entrance of my lord\'s lair! Soldiers, destroy them!',16756,6,0,0,'tyrannus SAY_TYRANNUS_AMBUSH_2'),
(-1658049,'Rimefang! Trap them within the tunnel! Bury them alive!',16757,6,0,0,'tyrannus SAY_GAUNTLET'),

(-1658050,'Alas, brave, brave adventurers, your meddling has reached its end. Do you hear the clatter of bone and steel coming up the tunnel behind you? That is the sound of your impending demise.',16758,1,0,0,'tyrannus SAY_PREFIGHT_1'),
(-1658051,'Heroes! We will hold off the undead as long as we can, even to our dying breath. Deal with the Scourgelord!',17148,1,0,0,'victus SAY_VICTUS_TRASH'),
(-1658052,'Ha, such an amusing gesture from the rabble. When I have finished with you, my master\'s blade will feast upon your souls. Die!',16759,1,0,0,'tyrannus SAY_PREFIGHT_2'),
(-1658053,'I shall not fail The Lich King! Come and meet your end!',16760,1,0,0,'tyrannus SAY_AGGRO'),
(-1658054,'Such a shameful display...',16761,1,0,0,'tyrannus SAY_SLAY_1'),
(-1658055,'Perhaps you should have stayed in the mountains!',16762,1,0,0,'tyrannus SAY_SLAY_2'),
(-1658056,'Impossible! Rimefang...Warn...',16763,1,0,0,'tyrannus SAY_DEATH'),
(-1658057,'Rimefang, destroy this fool!',16764,1,0,0,'tyrannus SAY_MARK'),
(-1658058,'Power... overwhelming!',16765,1,0,0,'tyrannus SAY_SMASH'),
(-1658059,'The frostwyrm %s gazes at $N and readies an icy attack!',0,3,0,0,'rimefang EMOTE_RIMEFANG_ICEBOLT'),
(-1658060,'%s roars and swells with dark might!',0,3,0,0,'tyrannus EMOTE_SMASH'),

(-1658061,'Brave champions, we owe you our lives, our freedom... Though it be a tiny gesture in the face of this enormous debt, I pledge that from this day forth, all will know of your deeds, and the blazing path of light you cut through the shadow of this dark citadel.',17149,1,0,0,'victus SAY_VICTUS_OUTRO_1'),
(-1658062,'This day will stand as a testament not only to your valor, but to the fact that no foe, not even the Lich King himself, can stand when Alliance and Horde set aside their differences and ---',0,1,0,0,'victus_or_ironskull SAY_GENERAL_OUTRO_2'),
(-1658063,'Heroes, to me!',16614,0,0,5,'jaina SAY_JAINA_OUTRO_1'),
(-1658064,'Take cover behind me! Quickly!',17037,0,0,5,'sylvanas SAY_SYLVANAS_OUTRO_1'),
(-1658065,'The Frost Queen is gone. We must keep moving - our objective is near.',16615,0,0,0,'jaina SAY_JAINA_OUTRO_2'),
(-1658066,'I... I could not save them... Damn you, Arthas! DAMN YOU!',16616,0,0,0,'jaina SAY_JAINA_OUTRO_3'),
(-1658067,'I thought he\'d never shut up. At last, Sindragosa silenced that long-winded fool. To the Halls of Reflection, champions! Our objective is near... I can sense it.',17036,0,0,396,'sylvanas SAY_SYLVANAS_OUTRO_2'),

(-1658068,'Heroes! We will hold off the undead as long as we can, even to our dying breath. Deal with the Scourgelord!',17150,1,0,0,'ironskull SAY_IRONSKULL_TRASH'),
(-1658069,'Brave champions, we owe you our lives, our freedom... Though it be a tiny gesture in the face of this enormous debt, I pledge that from this day forth, all will know of your deeds, and the blazing path of light you cut through the shadow of this dark citadel.',17151,1,0,0,'ironskull SAY_IRONSKULL_OUTRO_1');

-- -1 668 000 ICC: HALLS OF REFLECTION
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1668000,'The master surveyed his kingdom and found it... Lacking. His judgement was swift and without mercy: DEATH TO ALL!',16738,1,0,0,'marwyn SAY_GAUNTLET'),

(-1668001,'Your allies have arrived, Jaina, just as you promised. You will all become powerful agents of the Scourge.',17212,1,0,0,'lich king SAY_ALLY_INTRO_1'),
(-1668002,'He is too powerful, we must leave this place at once! My magic will hold him in place for only a short time! Come quickly, heroes!',16644,1,0,1,'jaina SAY_ALLY_INTRO_2'),
(-1668003,'I will not make the same mistake again, Sylvanas. This time there will be no escape. You will all serve me in death!',17213,1,0,0,'lich king SAY_HORDE_INTRO_1'),
(-1668004,'He\'s too powerful! Heroes, quickly, come to me! We must leave this place immediately! I will do what I can do hold him in place while we flee.',17058,1,0,1,'sylvanas SAY_HORDE_INTRO_2'),

(-1668005,'Death\'s cold embrace awaits.',17221,1,0,0,'Lich King - SAY_FIRST_WALL'),
(-1668006,'Succumb to the chill of the grave.',17218,1,0,0,'Lich King - SAY_SECOND_WALL'),
(-1668007,'Another dead end.',17219,1,0,0,'Lich King - SAY_THIRD_WALL'),
(-1668008,'How long can you fight it?',17220,1,0,0,'Lich King - SAY_LAST_WALL'),

(-1668009,'Halt! Do not carry that blade any further!',16675,1,0,1,'Uther - SAY_INTRO_QUELDELAR');

-- -1 724 000 RUBY SANCTUM
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1724000,'Help! I am trapped within this tree! I require aid!',17490,6,0,0,'xerestraza SAY_HELP'),
(-1724001,'Your power wanes, ancient one! Soon, you will join your friends!',17525,6,0,0,'baltharus SAY_INTRO'),
(-1724002,'Thank you! I could have not held out for much longer. A terrible thing has happened here.',17491,1,0,0,'xerestraza SAY_THANKS'),
(-1724003,'We believed that the Sanctum was well fortified, but we were not prepareted for the nature of this assault.',17492,0,0,0,'xerestraza SAY_OUTRO_1'),
(-1724004,'The Black Dragonkin materialized from thin air, and set upon us before we could react.',17493,0,0,0,'xerestraza SAY_OUTRO_2'),
(-1724005,'We did not stand a chance. As my brethren perished around me, I managed to retreat hear and bar the entrance.',17494,0,0,0,'xerestraza SAY_OUTRO_3'),
(-1724006,'They slaughtered us with cold efficiency, but the true focus of their interest seemed to be the eggs kept here in the sanctum.',17495,0,0,0,'xerestraza SAY_OUTRO_4'),
(-1724007,'The commander of the forces on the ground here is a cruel brute named Zarithrian. But I fear there are greater powers at work.',17496,0,0,0,'xerestraza SAY_OUTRO_5'),
(-1724008,'In their initial assault I caught a glimpse of their true leader, a fearsome full-grown Twilight Dragon.',17497,0,0,0,'xerestraza SAY_OUTRO_6'),
(-1724009,'I know not the extent of their plans heroes, but I know this: they cannot be allowed to succeed!',17498,0,0,0,'xerestraza SAY_OUTRO_7'),

(-1724010,'Ah, the entertainment has arrived.',17520,1,0,0,'baltharus SAY_AGGRO'),
(-1724011,'Baltharus leaves no survivors!',17521,1,0,0,'baltharus SAY_SLAY_1'),
(-1724012,'This world has enough heroes.',17522,1,0,0,'baltharus SAY_SLAY_2'),
(-1724013,'I... Didn\'t see that coming...',17523,1,0,0,'baltharus SAY_DEATH'),
(-1724014,'Twice the pain and half the fun.',17524,1,0,0,'baltharus SAY_SPLIT'),

(-1724015,'You will suffer for this intrusion!',17528,1,0,0,'saviana SAY_AGGRO'),
(-1724016,'As it should be...',17529,1,0,0,'saviana SAY_SLAY_1'),
(-1724017,'Halion will be pleased.',17530,1,0,0,'saviana SAY_SLAY_2'),
(-1724018,'Burn in the master\'s flame!',17532,1,0,0,'saviana SAY_SPECIAL'),

(-1724019,'Alexstrasza has chosen capable allies... A pity that I must END YOU!',17512,1,0,0,'zarithrian SAY_AGGRO'),
(-1724020,'You thought you stood a chance?',17513,1,0,0,'zarithrian SAY_SLAY_1'),
(-1724021,'It\'s for the best.',17514,1,0,0,'zarithrian SAY_SLAY_2'),
(-1724022,'HALION! I...',17515,1,0,0,'zarithrian SAY_DEATH'),
(-1724023,'Turn them to ash, minions!',17516,1,0,0,'zarithrian SAY_SUMMON'),

(-1724024,'Meddlesome insects! You\'re too late: The Ruby Sanctum\'s lost.',17499,6,0,0,'halion SAY_SPAWN'),
(-1724025,'Your world teeters on the brink of annihilation. You will ALL bear witness to the coming of a new age of DESTRUCTION!',17500,1,0,0,'halion SAY_AGGRO'),
(-1724026,'Another hero falls.',17501,1,0,0,'halion SAY_SLAY'),
(-1724027,'Relish this victory, mortals, for it will be your last! This world will burn with the master\'s return!',17503,1,0,0,'halion SAY_DEATH'),
(-1724028,'Not good enough.',17504,1,0,0,'halion SAY_BERSERK'),
(-1724029,'The heavens burn!',17505,1,0,0,'halion SAY_FIREBALL'),
(-1724030,'Beware the shadow!',17506,1,0,0,'halion SAY_SPHERES'),
(-1724031,'You will find only suffering within the realm of twilight! Enter if you dare!',17507,1,0,0,'halion SAY_PHASE_2'),
(-1724032,'I am the light and the darkness! Cower, mortals, before the herald of Deathwing!',17508,1,0,0,'halion SAY_PHASE_3'),
(-1724033,'The orbining spheres pulse with dark energy!',0,3,0,0,'halion EMOTE_SPHERES'),
(-1724034,'Your efforts force %s further out of the twillight realm!',0,3,0,0,'halion EMOTE_OUT_OF_TWILLIGHT'),
(-1724035,'Your efforts force %s further out of the physical realm!',0,3,0,0,'halion EMOTE_OUT_OF_PHYSICAL'),
(-1724036,'Your companions\' efforts force Halion further into the twillight realm!',0,3,0,0,'halion EMOTE_INTO_TWILLIGHT'),
(-1724037,'Your companions\' efforts force Halion further into the physical realm!',0,3,0,0,'halion EMOTE_INTO_PHYSICAL'),
(-1724038,'Without pressure in both realms %s begins to regenerate.',0,3,0,0,'halion EMOTE_REGENERATE');

-- -1 999 900 EXAMPLE TEXT
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1999900,'Let the games begin.',8280,1,0,0,'example_creature SAY_AGGRO'),
(-1999901,'I see endless suffering. I see torment. I see rage. I see everything.',8831,1,0,0,'example_creature SAY_RANDOM_0'),
(-1999902,'Muahahahaha',8818,1,0,0,'example_creature SAY_RANDOM_1'),
(-1999903,'These mortal infedels my lord, they have invaded your sanctum and seek to steal your secrets.',8041,1,0,0,'example_creature SAY_RANDOM_2'),
(-1999904,'You are already dead.',8581,1,0,0,'example_creature SAY_RANDOM_3'),
(-1999905,'Where to go? What to do? So many choices that all end in pain, end in death.',8791,1,0,0,'example_creature SAY_RANDOM_4'),
(-1999906,'$N, I sentance you to death!',8588,1,0,0,'example_creature SAY_BESERK'),
(-1999907,'The suffering has just begun!',0,1,0,0,'example_creature SAY_PHASE'),
(-1999908,'I always thought I was a good dancer.',0,0,0,0,'example_creature SAY_DANCE'),
(-1999909,'Move out Soldier!',0,0,0,0,'example_creature SAY_SALUTE'),

(-1999910,'Help $N! I\'m under attack!',0,0,0,0,'example_escort SAY_AGGRO1'),
(-1999911,'Die scum!',0,0,0,0,'example_escort SAY_AGGRO2'),
(-1999912,'Hmm a nice day for a walk alright',0,0,0,0,'example_escort SAY_WP_1'),
(-1999913,'Wild Felboar attack!',0,0,0,0,'example_escort SAY_WP_2'),
(-1999914,'Time for me to go! See ya around $N!',0,0,0,3,'example_escort SAY_WP_3'),
(-1999915,'Bye Bye!',0,0,0,3,'example_escort SAY_WP_4'),
(-1999916,'How dare you leave me like that! I hate you! =*(',0,3,0,0,'example_escort SAY_DEATH_1'),
(-1999917,'...no...how could you let me die $N',0,0,0,0,'example_escort SAY_DEATH_2'),
(-1999918,'ugh...',0,0,0,0,'example_escort SAY_DEATH_3'),
(-1999919,'Taste death!',0,0,0,0,'example_escort SAY_SPELL'),
(-1999920,'Fireworks!',0,0,0,0,'example_escort SAY_RAND_1'),
(-1999921,'Hmm, I think I could use a buff.',0,0,0,0,'example_escort SAY_RAND_2'),

(-1999922,'Normal select, guess you\'re not interested.',0,0,0,0,'example_gossip_codebox SAY_NOT_INTERESTED'),
(-1999923,'Wrong!',0,0,0,0,'example_gossip_codebox SAY_WRONG'),
(-1999924,'You\'re right, you are allowed to see my inner secrets.',0,0,0,0,'example_gossip_codebox SAY_CORRECT'),

(-1999925,'Hi!',0,0,0,0,'example_areatrigger SAY_HI');

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
(-3000110,'Why... yes, of course. I\'ve something to show you right inside this building, Mr. Anvilward.','prospector anvilward GOSSIP_ITEM_SHOW'),
(-3000111,'I am ready, Anchorite. Let us begin the exorcism.','anchorite barada GOSSIP_ITEM_EXORCISM'),
(-3000112,'I\'m ready - let\'s get out of here.','injured goblin miner GOSSIP_ITEM_ESCORT_READY'),
(-3000113,'Go on, you\'re free. Get out of here!','saronite mine slave GOSSIP_ITEM_SLAVE_FREE'),
(-3000114,'I\'m ready to start the distillation, uh, Tipsy.','tipsy mcmanus GOSSIP_ITEM_START_DISTILLATION'),
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
(-3230000,'You\'re free, Dughal! Get out of here!','dughal GOSSIP_ITEM_DUGHAL'),
(-3230001,'Get out of here, Tobias, you\'re free!','tobias GOSSIP_ITEM_TOBIAS'),
(-3230002,'Your bondage is at an end, Doom\'rel. I challenge you!','doomrel GOSSIP_ITEM_CHALLENGE'),
(-3230003,'Why don\'t you and Rocknot go find somewhere private...','nagmara GOSSIP_ITEM_NAGMARA');

-- -3 409 000 MOLTEN CORE
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3409000,'Tell me more.','majordomo_executus GOSSIP_ITEM_SUMMON_1'),
(-3409001,'What else do you have to say?','majordomo_executus GOSSIP_ITEM_SUMMON_2'),
(-3409002,'You challenged us and we have come. Where is this master you speak of?','majordomo_executus GOSSIP_ITEM_SUMMON_3');

-- -3 469 000 BLACKWING LAIR
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3469000,'I\'ve made no mistakes.','victor_nefarius GOSSIP_ITEM_NEFARIUS_1'),
(-3469001,'You have lost your mind, Nefarius. You speak in riddles.','victor_nefarius GOSSIP_ITEM_NEFARIUS_2'),
(-3469002,'Please do.','victor_nefarius GOSSIP_ITEM_NEFARIUS_3'),

(-3469003,'I cannot, Vaelastrasz! Surely something can be done to heal you!','vaelastrasz GOSSIP_ITEM_VAEL_1'),
(-3469004,'Vaelastrasz, no!!!','vaelastrasz GOSSIP_ITEM_VAEL_2');

-- -3 509 000 RUINS OF AHN'QIRAJ
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3509000,'Let\'s find out.','andorov GOSSIP_ITEM_START'),
(-3509001,'Let\'s see what you have.','andorov GOSSIP_ITEM_TRADE');

-- -3 532 000 KARAZHAN
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3532000,'Teleport me to the Guardian\'s Library','berthold GOSSIP_ITEM_TELEPORT'),
(-3532001,'I\'m not an actor.','barnes GOSSIP_ITEM_OPERA_1'),
(-3532002,'Ok, I\'ll give it a try, then.','barnes GOSSIP_ITEM_OPERA_2'),
(-3532003,'I\'ve never been more ready.','barnes GOSSIP_ITEM_OPERA_JULIANNE_WIPE'),
(-3532004,'The wolf\'s going down.','barnes GOSSIP_ITEM_OPERA_WOLF_WIPE'),
(-3532005,'What phat lewtz you have grandmother?','grandma GOSSIP_ITEM_GRANDMA'),

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
(-3532018,'Please reset the chess board, we would like to play again.','medivh GOSSIP_ITEM_RESET_BOARD');

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
(-3560000,'We are ready to get you out of here, Thrall. Let\'s go!','thrall GOSSIP_ITEM_START'),
(-3560001,'I need a pack of Incendiary Bombs.','erozion GOSSIP_ITEM_NEED_BOMBS'),
(-3560002,'Taretha cannot see you, Thrall.','thrall GOSSIP_ITEM_SKARLOC1'),
(-3560003,'The situation is rather complicated, Thrall. It would be best for you to head into the mountains now, before more of Blackmoore\'s men show up. We\'ll make sure Taretha is safe.','thrall GOSSIP_ITEM_SKARLOC2'),
(-3560004,'We\'re ready, Thrall.','thrall GOSSIP_ITEM_TARREN_2'),
(-3560005,'Strange wizard?','taretha GOSSIP_ITEM_EPOCH1'),
(-3560006,'We\'ll get you out. Taretha. Don\'t worry. I doubt the wizard would wander too far away.','taretha GOSSIP_ITEM_EPOCH2'),
(-3560007,'Tarren Mill.','thrall GOSSIP_ITEM_TARREN_1');

-- -3 564 000 BLACK TEMPLE
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3564000,'I\'m with you, Akama.','akama(shade) GOSSIP_ITEM_START_ENCOUNTER'),
(-3564001,'I\'m ready, Akama.','akama(illidan) GOSSIP_ITEM_PREPARE'),
(-3564002,'We\'re ready to face Illidan.','akama(illidan) GOSSIP_ITEM_START_EVENT');

-- -3 568 000 ZUL'AMAN
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3568000,'Thanks for the concern, but we intend to explore Zul\'Aman.','harrison jones GOSSIP_ITEM_BEGIN');

-- -3 595 000 CULLING OF STRATHOLME
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3595000,'What do you think they\'re up to?','chromie GOSSIP_ITEM_ENTRANCE_1'),
(-3595001,'You want me to do what?','chromie GOSSIP_ITEM_ENTRANCE_2'),
(-3595002,'Very well, Chromie.','chromie GOSSIP_ITEM_ENTRANCE_3'),
(-3595003,'Why have I been sent back to this particular place and time?','chromie GOSSIP_ITEM_INN_1'),
(-3595004,'What was this decision?','chromie GOSSIP_ITEM_INN_2'),
(-3595005,'So how does the Infinite Dragonflight plan to interfere?','chromie GOSSIP_ITEM_INN_3'),
(-3595006,'Chromie, you and I both know what\'s going to happen in this time stream. We\'ve seen this all before. Can you just skip us ahead to all the real action?','chromie GOSSIP_ITEM_INN_SKIP'),
(-3595007,'Yes, please!','chromie GOSSIP_ITEM_INN_TELEPORT'),
(-3595008,'Yes, my Prince. We are ready.','arthas GOSSIP_ITEM_CITY_GATES'),
(-3595009,'We\'re only doing what is best for Lordaeron, your Highness.','arthas GOSSIP_ITEM_TOWN_HALL'),
(-3595010,'Lead the way, Prince Arthas','arthas GOSSIP_ITEM_TOWN_HALL_2'),
(-3595011,'I\'m ready.','arthas GOSSIP_ITEM_EPOCH'),
(-3595012,'For Lordaeron!','arthas GOSSIP_ITEM_ESCORT'),
(-3595013,'I\'m ready to battle the dreadlord, sire.','arthas GOSSIP_ITEM_DREADLORD');

-- -3 599 000 HALLS OF STONE
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3599000,'Brann, it would be our honor!','brann GOSSIP_ITEM_ID_START'),
(-3599001,'Let\'s move Brann, enough of the history lessons!','brann GOSSIP_ITEM_ID_PROGRESS');

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
(-3603012,'We\'re ready. Begin the assault!','Brann Bronzebeard GOSSIP_ITEM_BEGIN_ASSAULT'),

(-3603013,'Lend us your aid, keeper. Together we will defeat Yogg-Saron.','Ulduar Keeper GOSSIP_ITEM_LEND_AID'),
(-3603014,'Yes.','Ulduar Keeper GOSSIP_ITEM_KEEPER_CONFIRM');

-- -3 608 000 VIOLET HOLD
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3608000,'Activate the crystals when we get in trouble, right?','sinclari GOSSIP_ITEM_INTRO'),
(-3608001,'Get your people to safety, we\'ll keep the Blue Dragonflight\'s forces at bay.','sinclari GOSSIP_ITEM_START'),
(-3608002,'I\'m not fighting, so send me in now!','sinclari GOSSIP_ITEM_TELEPORT');

-- -3 609 000 EBON HOLD (DK START)
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3609000,'I challenge you, death knight!','Death Knight Initiate GOSSIP_ITEM_ACCEPT_DUEL'),
(-3609001,'I am ready, Highlord. Let the siege of Light\'s Hope begin!','Highlord Darion Mograine GOSSIP_ITEM_READY');

-- -3 631 000 ICECROWN CITADEL
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3631000,'Teleport to the Light\'s Hammer.','GOSSIP_ITEM_TELE_LIGHTS_HAMMER'),
(-3631001,'Teleport to the Oratory of the Damned.','GOSSIP_ITEM_TELE_ORATORY_DAMNED'),
(-3631002,'Teleport to the Rampart of Skulls.','GOSSIP_ITEM_TELE_RAMPART_OF_SKULLS'),
(-3631003,'Teleport to the Deathbringer\'s Rise.','GOSSIP_ITEM_TELE_DEATHBRINGERS_RISE'),
(-3631004,'Teleport to the Upper Spire.','GOSSIP_ITEM_TELE_UPPER_SPIRE'),
(-3631005,'Teleport to the Sindragosa\'s Lair.','GOSSIP_ITEM_TELE_SINDRAGOSAS_LAIR');

-- -3 649 000 TRIAL OF CRUSADER
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3649000,'Yes. We are prepared for the challenges ahead of us.','barrett GOSSIP_ITEM_BEAST_INIT'),
(-3649001,'Bring forth the first challenge!','barrett GOSSIP_ITEM_BEAST_START'),
(-3649002,'We want another shot at those beasts!','barrett GOSSIP_ITEM_BEAST_WIPE_INIT'),
(-3649003,'What new challenge awaits us?','barrett GOSSIP_ITEM_JARAXXUS_INIT'),
(-3649004,'We\'re ready to fight the sorceror again.','barrett GOSSIP_ITEM_JARAXXUS_WIPE_INIT'),
(-3649005,'Of course!','barrett GOSSIP_ITEM_PVP_INIT'),
(-3649006,'Give the signal! We\'re ready to go!','barrett GOSSIP_ITEM_PVP_START'),
(-3649007,'That tough, huh?','barrett GOSSIP_ITEM_TWINS_INIT'),
(-3649008,'Val\'kyr? We\'re ready for them','barrett GOSSIP_ITEM_TWINS_START'),
(-3649009,'Your words of praise are appreciated, Coliseum Master.','barrett GOSSIP_ITEM_ANUB_INIT'),
(-3649010,'That is strange...','barrett GOSSIP_ITEM_ANUB_START'),
(-3649011,'We\'re ready for the next challenge.','barrett GOSSIP_ITEM_JARAXXUS_START'),
(-3649012,'You\'ll be even more amazed after we take them out!','barrett GOSSIP_ITEM_PVP_WIPE_INIT'),
(-3649013,'We\'re ready for anything!','barrett GOSSIP_ITEM_PVP_WIPE_START'),
(-3649014,'We\'re ready. This time things will be different.','barrett GOSSIP_ITEM_BEAST_WIPE_START'),
(-3649015,'Now.','barrett GOSSIP_ITEM_JARAXXUS_WIPE_START'),
(-3649016,'We\'ll just have to improve our teamwork to match the two of them.','barrett GOSSIP_ITEM_TWINS_WIPE_INIT'),
(-3649017,'Just bring them out again, then watch.','barrett GOSSIP_ITEM_TWINS_WIPE_START');

-- -3 650 000 TRIAL OF THE CHAMPION
INSERT INTO gossip_texts (entry,content_default,comment) VALUES
(-3650000,'I am ready.','herald GOSSIP_ITEM_READY'),
(-3650001,'I am ready.  However, I\'d like to skip the pageantry.','herald GOSSIP_ITEM_READY_SKIP_INTRO'),
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

DELETE FROM script_waypoint WHERE entry=467;
INSERT INTO script_waypoint VALUES
(467, 0, -10508.40, 1068.00, 55.21, 0, ''),
(467, 1, -10518.30, 1074.84, 53.96, 0, ''),
(467, 2, -10534.82, 1081.92, 49.88, 0, ''),
(467, 3, -10546.51, 1084.88, 50.13, 0, ''),
(467, 4, -10555.29, 1084.45, 45.75, 0, ''),
(467, 5, -10566.57, 1083.53, 42.10, 0, ''),
(467, 6, -10575.83, 1082.34, 39.46, 0, ''),
(467, 7, -10585.67, 1081.08, 37.77, 0, ''),
(467, 8, -10600.08, 1078.19, 36.23, 0, ''),
(467, 9, -10608.69, 1076.08, 35.88, 0, ''),
(467, 10, -10621.26, 1073.00, 35.40, 0, ''),
(467, 11, -10638.12, 1060.18, 33.61, 0, ''),
(467, 12, -10655.87, 1038.99, 33.48, 0, ''),
(467, 13, -10664.68, 1030.54, 32.70, 0, ''),
(467, 14, -10708.68, 1033.86, 33.32, 0, ''),
(467, 15, -10754.43, 1017.93, 32.79, 0, ''),
(467, 16, -10802.26, 1018.01, 32.16, 0, ''),
(467, 17, -10832.60, 1009.04, 32.71, 0, ''),
(467, 18, -10866.56, 1006.51, 31.71, 0, ''),
(467, 19, -10879.98, 1005.10, 32.84, 0, ''),
(467, 20, -10892.45, 1001.32, 34.46, 0, ''),
(467, 21, -10906.14, 997.11, 36.15, 0, ''),
(467, 22, -10922.26, 1002.23, 35.74, 0, ''),
(467, 23, -10936.32, 1023.38, 36.52, 0, ''),
(467, 24, -10933.35, 1052.61, 35.85, 0, ''),
(467, 25, -10940.25, 1077.66, 36.49, 0, ''),
(467, 26, -10957.09, 1099.33, 36.83, 0, ''),
(467, 27, -10956.53, 1119.90, 36.73, 0, ''),
(467, 28, -10939.30, 1150.75, 37.42, 0, ''),
(467, 29, -10915.14, 1202.09, 36.55, 0, ''),
(467, 30, -10892.59, 1257.03, 33.37, 0, ''),
(467, 31, -10891.93, 1306.66, 35.45, 0, ''),
(467, 32, -10896.17, 1327.86, 37.77, 0, ''),
(467, 33, -10906.03, 1368.05, 40.91, 0, ''),
(467, 34, -10910.18, 1389.33, 42.62, 0, ''),
(467, 35, -10915.42, 1417.72, 42.93, 0, ''),
(467, 36, -10926.37, 1421.18, 43.04, 0, 'walk here and say'),
(467, 37, -10952.31, 1421.74, 43.40, 0, ''),
(467, 38, -10980.04, 1411.38, 42.79, 0, ''),
(467, 39, -11006.06, 1420.47, 43.26, 0, ''),
(467, 40, -11021.98, 1450.59, 43.09, 0, ''),
(467, 41, -11025.36, 1491.59, 43.15, 0, ''),
(467, 42, -11036.09, 1508.32, 43.28, 0, ''),
(467, 43, -11060.68, 1526.72, 43.19, 0, ''),
(467, 44, -11072.75, 1527.77, 43.20, 5000, 'say and quest credit');

DELETE FROM script_waypoint WHERE entry=1978;
INSERT INTO script_waypoint VALUES
(1978, 0, 1406.32, 1083.10, 52.55, 0, ''),
(1978, 1, 1400.49, 1080.42, 52.50, 0, 'SAY_START_2'),
(1978, 2, 1388.48, 1083.10, 52.52, 0, ''),
(1978, 3, 1370.16, 1084.02, 52.30, 0, ''),
(1978, 4, 1359.02, 1080.85, 52.46, 0, ''),
(1978, 5, 1341.43, 1087.39, 52.69, 0, ''),
(1978, 6, 1321.93, 1090.51, 50.66, 0, ''),
(1978, 7, 1312.98, 1095.91, 47.49, 0, ''),
(1978, 8, 1301.09, 1102.94, 47.76, 0, ''),
(1978, 9, 1297.73, 1106.35, 50.18, 0, ''),
(1978, 10, 1295.49, 1124.32, 50.49, 0, ''),
(1978, 11, 1294.84, 1137.25, 51.75, 0, ''),
(1978, 12, 1292.89, 1158.99, 52.65, 0, ''),
(1978, 13, 1290.75, 1168.67, 52.56, 2000, 'quest complete SAY_END'),
(1978, 14, 1287.12, 1203.49, 52.66, 5000, 'SAY_RANE'),
(1978, 15, 1288.30, 1203.89, 52.68, 5000, 'SAY_RANE_REPLY'),
(1978, 16, 1288.30, 1203.89, 52.68, 5000, 'SAY_CHECK_NEXT'),
(1978, 17, 1290.72, 1207.44, 52.69, 0, ''),
(1978, 18, 1297.50, 1207.18, 53.74, 0, ''),
(1978, 19, 1301.32, 1220.90, 53.74, 0, ''),
(1978, 20, 1298.55, 1220.43, 53.74, 0, ''),
(1978, 21, 1297.38, 1212.87, 58.51, 0, ''),
(1978, 22, 1297.80, 1210.04, 58.51, 0, ''),
(1978, 23, 1305.01, 1206.10, 58.51, 0, ''),
(1978, 24, 1310.51, 1207.36, 58.51, 5000, 'SAY_QUINN'),
(1978, 25, 1312.59, 1207.21, 58.51, 5000, 'SAY_QUINN_REPLY'),
(1978, 26, 1312.59, 1207.21, 58.51, 30000, 'SAY_BYE');

DELETE FROM script_waypoint WHERE entry=2768;
INSERT INTO script_waypoint VALUES
(2768, 0, -2077.73, -2091.17, 9.49, 0, ''),
(2768, 1, -2077.99, -2105.33, 13.24, 0, ''),
(2768, 2, -2074.60, -2109.67, 14.24, 0, ''),
(2768, 3, -2076.60, -2117.46, 16.67, 0, ''),
(2768, 4, -2073.51, -2123.46, 18.42, 2000, ''),
(2768, 5, -2073.51, -2123.46, 18.42, 4000, ''),
(2768, 6, -2066.60, -2131.85, 21.56, 0, ''),
(2768, 7, -2053.85, -2143.19, 20.31, 0, ''),
(2768, 8, -2043.49, -2153.73, 20.20, 10000, ''),
(2768, 9, -2043.49, -2153.73, 20.20, 20000, ''),
(2768, 10, -2043.49, -2153.73, 20.20, 10000, ''),
(2768, 11, -2043.49, -2153.73, 20.20, 2000, ''),
(2768, 12, -2053.85, -2143.19, 20.31, 0, ''),
(2768, 13, -2066.60, -2131.85, 21.56, 0, ''),
(2768, 14, -2073.51, -2123.46, 18.42, 0, ''),
(2768, 15, -2076.60, -2117.46, 16.67, 0, ''),
(2768, 16, -2074.60, -2109.67, 14.24, 0, ''),
(2768, 17, -2077.99, -2105.33, 13.24, 0, ''),
(2768, 18, -2077.73, -2091.17, 9.49, 0, ''),
(2768, 19, -2066.41, -2086.21, 8.97, 6000, ''),
(2768, 20, -2066.41, -2086.21, 8.97, 2000, '');

DELETE FROM script_waypoint WHERE entry=2917;
INSERT INTO script_waypoint VALUES
(2917, 0, 4675.812500, 598.614563, 17.645658, 0, 'SAY_REM_START'),
(2917, 1, 4672.844238, 599.325378, 16.417622, 0, ''),
(2917, 2, 4663.449707, 607.430176, 10.494752, 0, ''),
(2917, 3, 4655.969238, 613.761353, 8.523270, 0, ''),
(2917, 4, 4640.804688, 623.999329, 8.377054, 0, ''),
(2917, 5, 4631.678711, 630.801086, 6.414999, 5000, 'SAY_REM_RAMP1_1'),
(2917, 6, 4633.533203, 632.476440, 6.509831, 0, 'ambush'),
(2917, 7, 4639.413574, 637.120789, 13.338119, 0, ''),
(2917, 8, 4642.352051, 637.668152, 13.437444, 0, ''),
(2917, 9, 4645.082031, 634.463989, 13.437208, 5000, 'SAY_REM_RAMP1_2'),
(2917, 10, 4642.345215, 637.584839, 13.435211, 0, ''),
(2917, 11, 4639.630859, 637.233765, 13.339752, 0, ''),
(2917, 12, 4633.363281, 632.462280, 6.488438, 0, ''),
(2917, 13, 4624.714844, 631.723511, 6.264030, 0, ''),
(2917, 14, 4623.525879, 629.718506, 6.201339, 5000, 'SAY_REM_BOOK'),
(2917, 15, 4623.452148, 630.369629, 6.218942, 0, 'SAY_REM_TENT1_1'),
(2917, 16, 4622.622070, 637.221558, 6.312845, 0, 'ambush'),
(2917, 17, 4619.755371, 637.386230, 6.312050, 5000, 'SAY_REM_TENT1_2'),
(2917, 18, 4620.027832, 637.367676, 6.312050, 0, ''),
(2917, 19, 4624.154785, 637.560303, 6.313898, 0, ''),
(2917, 20, 4622.967773, 634.016479, 6.294979, 0, ''),
(2917, 21, 4616.926758, 630.303284, 6.239193, 0, ''),
(2917, 22, 4614.546387, 616.983337, 5.687642, 0, ''),
(2917, 23, 4610.279297, 610.029419, 5.442539, 0, ''),
(2917, 24, 4601.149902, 604.111694, 2.054856, 0, ''),
(2917, 25, 4589.618164, 597.685730, 1.057147, 0, ''),
(2917, 26, 4577.588379, 592.145813, 1.120190, 0, 'SAY_REM_MOSS (?)'),
(2917, 27, 4569.848145, 592.177490, 1.260874, 5000, 'EMOTE_REM_MOSS (?)'),
(2917, 28, 4568.791992, 590.870911, 1.211338, 3000, 'SAY_REM_MOSS_PROGRESS (?)'),
(2917, 29, 4566.722656, 564.077881, 1.343084, 0, 'ambush'),
(2917, 30, 4568.269531, 551.958435, 5.004200, 0, ''),
(2917, 31, 4566.731934, 551.557861, 5.426314, 5000, 'SAY_REM_PROGRESS'),
(2917, 32, 4566.741699, 560.767639, 1.703257, 0, ''),
(2917, 33, 4573.916016, 582.566101, 0.749801, 0, ''),
(2917, 34, 4594.206055, 598.533020, 1.034056, 0, ''),
(2917, 35, 4601.194824, 604.283081, 2.060146, 0, ''),
(2917, 36, 4609.539551, 610.844727, 5.402220, 0, ''),
(2917, 37, 4624.800293, 618.076477, 5.851541, 0, ''),
(2917, 38, 4632.414063, 623.778442, 7.286243, 0, ''),
(2917, 39, 4645.915039, 621.983765, 8.579967, 0, ''),
(2917, 40, 4658.669922, 611.092651, 8.891747, 0, ''),
(2917, 41, 4671.924316, 599.752197, 16.01242, 5000, 'SAY_REM_REMEMBER'),
(2917, 42, 4676.976074, 600.649780, 17.82566, 5000, 'EMOTE_REM_END');

DELETE FROM script_waypoint WHERE entry=3439;
INSERT INTO script_waypoint VALUES
(3439, 0, 1105.090332, -3101.254150, 82.706, 1000, 'SAY_STARTUP1'),
(3439, 1, 1103.204468, -3104.345215, 83.113, 1000, ''),
(3439, 2, 1107.815186, -3106.495361, 82.739, 1000, ''),
(3439, 3, 1104.733276, -3100.830811, 82.747, 1000, ''),
(3439, 4, 1103.242554, -3106.270020, 83.133, 1000, ''),
(3439, 5, 1112.807373, -3106.285400, 82.320, 1000, ''),
(3439, 6, 1112.826782, -3108.908691, 82.377, 1000, ''),
(3439, 7, 1108.053955, -3115.156738, 82.894, 0, ''),
(3439, 8, 1108.355591, -3104.365234, 82.377, 5000, ''),
(3439, 9, 1100.306763, -3097.539063, 83.150, 0, 'SAY_STARTUP2'),
(3439, 10, 1100.562378, -3082.721924, 82.768, 0, ''),
(3439, 11, 1097.512939, -3069.226563, 82.206, 0, ''),
(3439, 12, 1092.964966, -3053.114746, 82.351, 0, ''),
(3439, 13, 1094.010986, -3036.958496, 82.888, 0, ''),
(3439, 14, 1095.623901, -3025.760254, 83.392, 0, ''),
(3439, 15, 1107.656494, -3013.530518, 85.653, 0, ''),
(3439, 16, 1119.647705, -3006.928223, 87.019, 0, ''),
(3439, 17, 1129.991211, -3002.410645, 91.232, 7000, 'SAY_MERCENARY'),
(3439, 18, 1133.328735, -2997.710693, 91.675, 1000, 'SAY_PROGRESS_1'),
(3439, 19, 1131.799316, -2987.948242, 91.976, 1000, ''),
(3439, 20, 1122.028687, -2993.397461, 91.536, 0, ''),
(3439, 21, 1116.614868, -2981.916748, 92.103, 0, ''),
(3439, 22, 1102.239136, -2994.245117, 92.074, 0, ''),
(3439, 23, 1096.366211, -2978.306885, 91.873, 0, ''),
(3439, 24, 1091.971558, -2985.919189, 91.730, 40000, 'SAY_PROGRESS_2');

DELETE FROM script_waypoint WHERE entry=3465;
INSERT INTO script_waypoint VALUES
(3465, 0, -2095.840820, -3650.001221, 61.716, 0, ''),
(3465, 1, -2100.193604, -3613.949219, 61.604, 0, ''),
(3465, 2, -2098.549561, -3601.557129, 59.154, 0, ''),
(3465, 3, -2093.796387, -3595.234375, 56.658, 0, ''),
(3465, 4, -2072.575928, -3578.827637, 48.844, 0, ''),
(3465, 5, -2023.858398, -3568.146240, 24.636, 0, ''),
(3465, 6, -2013.576416, -3571.499756, 22.203, 0, ''),
(3465, 7, -2009.813721, -3580.547852, 21.791, 0, ''),
(3465, 8, -2015.296021, -3597.387695, 21.760, 0, ''),
(3465, 9, -2020.677368, -3610.296143, 21.759, 0, ''),
(3465, 10, -2019.990845, -3640.155273, 21.759, 0, ''),
(3465, 11, -2016.110596, -3664.133301, 21.758, 0, ''),
(3465, 12, -1999.397095, -3679.435059, 21.316, 0, ''),
(3465, 13, -1987.455811, -3688.309326, 18.495, 0, ''),
(3465, 14, -1973.966553, -3687.666748, 14.996, 0, ''),
(3465, 15, -1949.163940, -3678.054932, 11.293, 0, ''),
(3465, 16, -1934.091187, -3682.859619, 9.897, 30000, 'SAY_GIL_AT_LAST'),
(3465, 17, -1935.383911, -3682.322021, 10.029, 1500, 'SAY_GIL_PROCEED'),
(3465, 18, -1879.039185, -3699.498047, 6.582, 7500, 'SAY_GIL_FREEBOOTERS'),
(3465, 19, -1852.728149, -3703.778809, 6.875, 0, ''),
(3465, 20, -1812.989990, -3718.500732, 10.572, 0, ''),
(3465, 21, -1788.171265, -3722.867188, 9.663, 0, ''),
(3465, 22, -1767.206665, -3739.923096, 10.082, 0, ''),
(3465, 23, -1750.194580, -3747.392090, 10.390, 0, ''),
(3465, 24, -1729.335571, -3776.665527, 11.779, 0, ''),
(3465, 25, -1715.997925, -3802.404541, 12.618, 0, ''),
(3465, 26, -1690.711548, -3829.262451, 13.905, 0, ''),
(3465, 27, -1674.700684, -3842.398682, 13.872, 0, ''),
(3465, 28, -1632.726318, -3846.109619, 14.401, 0, ''),
(3465, 29, -1592.734497, -3842.225342, 14.981, 0, ''),
(3465, 30, -1561.614746, -3839.320801, 19.118, 0, ''),
(3465, 31, -1544.567627, -3834.393311, 18.761, 0, ''),
(3465, 32, -1512.514404, -3831.715820, 22.914, 0, ''),
(3465, 33, -1486.889771, -3836.639893, 23.964, 0, ''),
(3465, 34, -1434.193604, -3852.702881, 18.843, 0, ''),
(3465, 35, -1405.794678, -3854.488037, 17.276, 0, ''),
(3465, 36, -1366.592041, -3852.383789, 19.273, 0, ''),
(3465, 37, -1337.360962, -3837.827148, 17.352, 2000, 'SAY_GIL_ALMOST'),
(3465, 38, -1299.744507, -3810.691406, 20.801, 0, ''),
(3465, 39, -1277.144409, -3782.785156, 25.918, 0, ''),
(3465, 40, -1263.686768, -3781.251953, 26.447, 0, ''),
(3465, 41, -1243.674438, -3786.328125, 25.281, 0, ''),
(3465, 42, -1221.875488, -3784.124512, 24.051, 0, ''),
(3465, 43, -1204.011230, -3775.943848, 24.437, 0, ''),
(3465, 44, -1181.706787, -3768.934082, 23.368, 0, ''),
(3465, 45, -1156.913818, -3751.559326, 21.074, 0, ''),
(3465, 46, -1138.830688, -3741.809326, 17.843, 0, ''),
(3465, 47, -1080.101196, -3738.780029, 19.805, 0, 'SAY_GIL_SWEET'),
(3465, 48, -1069.065186, -3735.006348, 19.302, 0, ''),
(3465, 49, -1061.941040, -3724.062256, 21.086, 0, ''),
(3465, 50, -1053.593262, -3697.608643, 27.320, 0, ''),
(3465, 51, -1044.110474, -3690.133301, 24.856, 0, ''),
(3465, 52, -1040.260986, -3690.739014, 25.342, 0, ''),
(3465, 53, -1028.146606, -3688.718750, 23.843, 7500, 'SAY_GIL_FREED');

DELETE FROM script_waypoint WHERE entry=3849;
INSERT INTO script_waypoint VALUES
(3849, 0, -250.923, 2116.26, 81.179, 0, 'SAY_FREE_AD'),
(3849, 1, -255.049, 2119.39, 81.179, 0, ''),
(3849, 2, -254.129, 2123.45, 81.179, 0, ''),
(3849, 3, -253.898, 2130.87, 81.179, 0, ''),
(3849, 4, -249.889, 2142.31, 86.972, 0, ''),
(3849, 5, -248.205, 2144.02, 87.013, 0, ''),
(3849, 6, -240.553, 2140.55, 87.012, 0, ''),
(3849, 7, -237.514, 2142.07, 87.012, 0, ''),
(3849, 8, -235.638, 2149.23, 90.587, 0, ''),
(3849, 9, -237.188, 2151.95, 90.624, 0, ''),
(3849, 10, -241.162, 2153.65, 90.624, 0, 'SAY_OPEN_DOOR_AD'),
(3849, 11, -241.13, 2154.56, 90.624, 2000, 'SAY_UNLOCK_DOOR_AD'),
(3849, 12, -241.13, 2154.56, 90.624, 3000, ''),
(3849, 13, -241.13, 2154.56, 90.624, 5000, 'SAY_POST1_DOOR_AD'),
(3849, 14, -241.13, 2154.56, 90.624, 0, 'SAY_POST2_DOOR_AD'),
(3849, 15, -208.764, 2141.6, 90.6257, 0, ''),
(3849, 16, -206.441, 2143.51, 90.4287, 0, ''),
(3849, 17, -203.715, 2145.85, 88.7052, 0, ''),
(3849, 18, -199.199, 2144.88, 86.501, 0, ''),
(3849, 19, -195.798, 2143.58, 86.501, 0, ''),
(3849, 20, -190.029, 2141.38, 83.2712, 0, ''),
(3849, 21, -189.353, 2138.65, 83.1102, 0, ''),
(3849, 22, -190.304, 2135.73, 81.5288, 0, ''),
(3849, 23, -207.325, 2112.43, 81.0548, 0, ''),
(3849, 24, -208.754, 2109.9, 81.0527, 0, ''),
(3849, 25, -206.248, 2108.62, 81.0555, 0, ''),
(3849, 26, -202.017, 2106.64, 78.6836, 0, ''),
(3849, 27, -200.928, 2104.49, 78.5569, 0, ''),
(3849, 28, -201.845, 2101.17, 76.9256, 0, ''),
(3849, 29, -202.844, 2100.11, 76.8911, 0, ''),
(3849, 30, -213.326, 2105.83, 76.8925, 0, ''),
(3849, 31, -226.993, 2111.47, 76.8892, 0, ''),
(3849, 32, -227.955, 2112.34, 76.8895, 0, ''),
(3849, 33, -230.05, 2106.64, 76.8895, 0, '');

DELETE FROM script_waypoint WHERE entry=3850;
INSERT INTO script_waypoint VALUES
(3850, 0, -241.817, 2122.9, 81.179, 0, 'SAY_FREE_AS'),
(3850, 1, -247.139, 2124.89, 81.179, 0, ''),
(3850, 2, -253.179, 2127.41, 81.179, 0, ''),
(3850, 3, -253.898, 2130.87, 81.179, 0, ''),
(3850, 4, -249.889, 2142.31, 86.972, 0, ''),
(3850, 5, -248.205, 2144.02, 87.013, 0, ''),
(3850, 6, -240.553, 2140.55, 87.012, 0, ''),
(3850, 7, -237.514, 2142.07, 87.012, 0, ''),
(3850, 8, -235.638, 2149.23, 90.587, 0, ''),
(3850, 9, -237.188, 2151.95, 90.624, 0, ''),
(3850, 10, -241.162, 2153.65, 90.624, 0, 'SAY_OPEN_DOOR_AS'),
(3850, 11, -241.13, 2154.56, 90.624, 5000, 'cast'),
(3850, 12, -241.13, 2154.56, 90.624, 0, ''),
(3850, 13, -241.13, 2154.56, 90.624, 5000, 'SAY_POST_DOOR_AS'),
(3850, 14, -241.13, 2154.56, 90.624, 2500, 'cast'),
(3850, 15, -241.13, 2154.56, 90.624, 0, 'SAY_VANISH_AS');

DELETE FROM script_waypoint WHERE entry=4500;
INSERT INTO script_waypoint VALUES
(4500, 0, -3125.597168, -2885.673828, 34.731, 2500, ''),
(4500, 1, -3120.257080, -2877.830322, 34.917, 0, ''),
(4500, 2, -3116.487305, -2850.670410, 34.869, 0, ''),
(4500, 3, -3093.474854, -2819.189697, 34.432, 0, ''),
(4500, 4, -3104.726318, -2802.020996, 33.954, 0, ''),
(4500, 5, -3105.906006, -2780.234375, 34.469, 0, ''),
(4500, 6, -3116.080811, -2757.902588, 34.734, 0, ''),
(4500, 7, -3125.234375, -2733.960205, 33.189, 0, '');

DELETE FROM script_waypoint WHERE entry=4962;
INSERT INTO script_waypoint VALUES
(4962, 0, -3804.438965, -828.048035, 10.093068, 0, ''),
(4962, 1, -3803.934326, -835.772400, 10.077722, 0, ''),
(4962, 2, -3792.629150, -835.670898,  9.655657, 0, ''),
(4962, 3, -3772.433838, -835.345947, 10.868981, 0, ''),
(4962, 4, -3765.937256, -840.128601, 10.885593, 0, ''),
(4962, 5, -3738.633789, -830.997498, 11.057384, 0, ''),
(4962, 6, -3690.224121, -862.261597,  9.960449, 0, '');

DELETE FROM script_waypoint WHERE entry=4983;
INSERT INTO script_waypoint VALUES
(4983, 0, -3322.649414, -3124.631836, 33.842, 0, ''),
(4983, 1, -3326.336670, -3126.833496, 34.426, 0, ''),
(4983, 2, -3336.984131, -3129.611816, 30.692, 0, ''),
(4983, 3, -3342.598389, -3132.146729, 30.422, 0, ''),
(4983, 4, -3355.827881, -3140.947998, 29.534, 0, ''),
(4983, 5, -3365.828125, -3144.284180, 35.176, 0, ''),
(4983, 6, -3368.904541, -3147.265381, 36.091, 0, ''),
(4983, 7, -3369.355957, -3169.828857, 36.325, 0, ''),
(4983, 8, -3371.443359, -3183.905029, 33.454, 0, ''),
(4983, 9, -3373.824951, -3190.861084, 34.717, 5000, 'SAY_OGR_SPOT'),
(4983, 10, -3368.529785, -3198.210205, 34.926, 0, 'SAY_OGR_RET_WHAT'),
(4983, 11, -3366.265625, -3210.867676, 33.733, 5000, 'pause'),
(4983, 12, -3368.529785, -3198.210205, 34.926, 0, ''),
(4983, 13, -3373.824951, -3190.861084, 34.717, 0, ''),
(4983, 14, -3371.443359, -3183.905029, 33.454, 0, ''),
(4983, 15, -3369.355957, -3169.828857, 36.325, 0, ''),
(4983, 16, -3368.904541, -3147.265381, 36.091, 0, ''),
(4983, 17, -3365.828125, -3144.284180, 35.176, 0, ''),
(4983, 18, -3355.827881, -3140.947998, 29.534, 0, ''),
(4983, 19, -3342.598389, -3132.146729, 30.422, 0, ''),
(4983, 20, -3336.984131, -3129.611816, 30.692, 0, ''),
(4983, 21, -3326.336670, -3126.833496, 34.426, 0, ''),
(4983, 22, -3322.649414, -3124.631836, 33.842, 0, '');

DELETE FROM script_waypoint WHERE entry = 5391;
INSERT INTO script_waypoint VALUES
(5391, 0, -9901.12, -3727.29, 22.11, 3000, ''),
(5391, 1, -9909.27, -3727.81, 23.25, 0, ''),
(5391, 2, -9935.25, -3729.02, 22.11, 0, ''),
(5391, 3, -9945.83, -3719.34, 21.68, 0, ''),
(5391, 4, -9963.41, -3710.18, 21.71, 0, ''),
(5391, 5, -9972.75, -3690.13, 21.68, 0, ''),
(5391, 6, -9989.70, -3669.67, 21.67, 0, ''),
(5391, 7, -9989.21, -3647.76, 23.00, 0, ''),
(5391, 8, -9992.27, -3633.74, 21.67, 0, ''),
(5391, 9,-10002.32, -3611.67, 22.26, 0, ''),
(5391,10, -9999.25, -3586.33, 21.85, 0, ''),
(5391,11,-10006.53, -3571.99, 21.67, 0, ''),
(5391,12,-10014.30, -3545.24, 21.67, 0, ''),
(5391,13,-10018.91, -3525.03, 21.68, 0, ''),
(5391,14,-10030.22, -3514.77, 21.67, 0, ''),
(5391,15,-10045.11, -3501.49, 21.67, 0, ''),
(5391,16,-10052.91, -3479.13, 21.67, 0, ''),
(5391,17,-10060.68, -3460.31, 21.67, 0, ''),
(5391,18,-10074.68, -3436.85, 20.97, 0, ''),
(5391,19,-10074.68, -3436.85, 20.97, 0, ''),
(5391,20,-10072.86, -3408.92, 20.43, 15000, ''),
(5391,21,-10108.01, -3406.05, 22.06, 0, '');

DELETE FROM script_waypoint WHERE entry=6182;
INSERT INTO script_waypoint VALUES
(6182, 0, -11480.684570, 1545.091187, 49.898571, 0, ''),
(6182, 1, -11466.825195, 1530.151733, 50.263611, 0, ''),
(6182, 2, -11465.213867, 1528.343750, 50.954369, 0, 'entrance hut'),
(6182, 3, -11462.990234, 1525.235596, 50.937702, 0, ''),
(6182, 4, -11461.000000, 1526.614014, 50.937702, 5000, 'pick up rifle'),
(6182, 5, -11462.990234, 1525.235596, 50.937702, 0, ''),
(6182, 6, -11465.213867, 1528.343750, 50.954369, 0, ''),
(6182, 7, -11468.353516, 1535.075562, 50.400948, 15000, 'hold, prepare for wave1'),
(6182, 8, -11468.353516, 1535.075562, 50.400948, 15000, 'hold, prepare for wave2'),
(6182, 9, -11468.353516, 1535.075562, 50.400948, 10000, 'hold, prepare for wave3'),
(6182, 10, -11467.898438, 1532.459595, 50.348885, 0, 'we are done'),
(6182, 11, -11466.064453, 1529.855225, 50.209351, 0, ''),
(6182, 12, -11462.990234, 1525.235596, 50.937702, 0, ''),
(6182, 13, -11461.000000, 1526.614014, 50.937702, 5000, 'deliver rifle'),
(6182, 14, -11462.990234, 1525.235596, 50.937702, 0, ''),
(6182, 15, -11465.213867, 1528.343750, 50.954369, 0, ''),
(6182, 16, -11470.260742, 1537.276733, 50.378487, 0, ''),
(6182, 17, -11475.581055, 1548.678833, 50.184380, 0, 'complete quest'),
(6182, 18, -11482.299805, 1557.410034, 48.624519, 0, '');

DELETE FROM script_waypoint WHERE entry=6575;
INSERT INTO script_waypoint VALUES
(6575, 0, 1945.81, -431.54, 16.36, 0, ''),
(6575, 1, 1946.21, -436.41, 16.36, 0, ''),
(6575, 2, 1950.01, -444.11, 14.63, 0, ''),
(6575, 3, 1956.08, -449.34, 13.12, 0, ''),
(6575, 4, 1966.59, -450.55, 11.27, 0, ''),
(6575, 5, 1976.09, -447.51, 11.27, 0, ''),
(6575, 6, 1983.42, -435.85, 11.27, 0, ''),
(6575, 7, 1978.17, -428.81, 11.27, 0, ''),
(6575, 8, 1973.97, -422.08, 9.04, 0, ''),
(6575, 9, 1963.84, -418.90, 6.17, 0, ''),
(6575, 10, 1961.22, -422.74, 6.17, 0, ''),
(6575, 11, 1964.80, -431.26, 6.17, 300000, '');

DELETE FROM script_waypoint WHERE entry=7780;
INSERT INTO script_waypoint VALUES
(7780, 0, 261.058868, -2757.876221, 122.553, 0, ''),
(7780, 1, 259.812195, -2758.249023, 122.555, 0, 'SAY_RIN_FREE'),
(7780, 2, 253.823441, -2758.619141, 122.562, 0, ''),
(7780, 3, 241.394791, -2769.754883, 123.309, 0, ''),
(7780, 4, 218.915588, -2783.397461, 123.355, 0, ''),
(7780, 5, 209.088196, -2789.676270, 122.001, 0, ''),
(7780, 6, 204.453568, -2792.205811, 120.620, 0, ''),
(7780, 7, 182.012604, -2809.995361, 113.887, 0, 'summon'),
(7780, 8, 164.411591, -2825.162842, 107.779, 0, ''),
(7780, 9, 149.727600, -2833.704346, 106.224, 0, ''),
(7780, 10, 142.448074, -2838.807373, 109.665, 0, ''),
(7780, 11, 133.274963, -2845.135254, 112.606, 0, ''),
(7780, 12, 111.247459, -2861.065674, 116.305, 0, ''),
(7780, 13, 96.104073, -2874.886230, 114.397, 0, 'summon'),
(7780, 14, 73.369942, -2881.184570, 117.666, 0, ''),
(7780, 15, 58.579178, -2889.151611, 116.253, 0, ''),
(7780, 16, 33.214249, -2906.343994, 115.083, 0, ''),
(7780, 17, 19.586519, -2908.712402, 117.276, 7500, 'SAY_RIN_COMPLETE'),
(7780, 18, 10.282522, -2911.607422, 118.394, 0, ''),
(7780, 19, -37.580383, -2942.730225, 117.145, 0, ''),
(7780, 20, -68.599411, -2953.694824, 116.685, 0, ''),
(7780, 21, -102.054253, -2956.965576, 116.677, 0, ''),
(7780, 22, -135.993637, -2955.743652, 115.788, 0, ''),
(7780, 23, -171.561600, -2951.417480, 115.451, 0, '');

DELETE FROM script_waypoint WHERE entry=7784;
INSERT INTO script_waypoint VALUES
(7784, 0, -8845.65, -4373.98, 43.87, 5000, 'SAY_START'),
(7784, 1, -8840.79, -4373.73, 44.24, 0, ''),
(7784, 2, -8837.43, -4373.56, 45.60, 0, ''),
(7784, 3, -8832.74, -4373.32, 45.68, 0, ''),
(7784, 4, -8829.37, -4373.14, 44.33, 0, ''),
(7784, 5, -8817.38, -4372.41, 35.58, 0, ''),
(7784, 6, -8803.47, -4371.60, 30.34, 0, ''),
(7784, 7, -8795.10, -4365.61, 26.08, 0, ''),
(7784, 8, -8766.78, -4367.13, 25.15, 0, ''),
(7784, 9, -8755.63, -4367.54, 24.63, 0, ''),
(7784, 10, -8754.42, -4365.59, 24.15, 0, ''),
(7784, 11, -8728.82, -4353.13, 20.90, 0, ''),
(7784, 12, -8706.60, -4356.55, 17.93, 0, ''),
(7784, 13, -8679.00, -4380.23, 12.64, 0, ''),
(7784, 14, -8642.96, -4393.82, 12.52, 0, ''),
(7784, 15, -8611.19, -4399.11, 9.55, 0, ''),
(7784, 16, -8554.87, -4409.32, 13.05, 0, ''),
(7784, 17, -8531.64, -4411.96, 11.20, 0, ''),
(7784, 18, -8510.40, -4414.38, 12.84, 0, ''),
(7784, 19, -8476.92, -4418.34, 9.71, 0, ''),
(7784, 20, -8435.89, -4426.74, 9.67, 0, ''),
(7784, 21, -8381.89, -4446.40, 10.23, 0, ''),
(7784, 22, -8351.15, -4447.79, 9.99, 5000, 'first ambush SAY_AMBUSH'),
(7784, 23, -8324.18, -4445.05, 9.71, 0, ''),
(7784, 24, -8138.94, -4384.78, 10.92, 0, ''),
(7784, 25, -8036.87, -4443.38, 9.65, 0, ''),
(7784, 26, -7780.92, -4761.81, 9.50, 0, ''),
(7784, 27, -7587.67, -4765.01, 8.96, 0, ''),
(7784, 28, -7497.65, -4792.86, 10.01, 0, 'second ambush SAY_AMBUSH'),
(7784, 29, -7391.54, -4774.26, 12.44, 0, ''),
(7784, 30, -7308.42, -4739.87, 12.65, 0, ''),
(7784, 31, -7016.11, -4751.12, 10.06, 0, ''),
(7784, 32, -6985.52, -4777.41, 10.26, 0, ''),
(7784, 33, -6953.02, -4786.00, 6.32, 0, ''),
(7784, 34, -6940.37, -4831.03, 0.67, 10000, 'quest complete SAY_END');

DELETE FROM script_waypoint WHERE entry=7806;
INSERT INTO script_waypoint VALUES
(7806, 0, 495.404358, -3478.350830, 114.837, 0, ''),
(7806, 1, 492.704742, -3486.112549, 108.627, 0, ''),
(7806, 2, 487.249756, -3485.764404, 107.890, 0, ''),
(7806, 3, 476.851959, -3489.875977, 99.985, 0, ''),
(7806, 4, 467.212402, -3493.355469, 99.819, 0, ''),
(7806, 5, 460.017029, -3496.984375, 104.481, 0, ''),
(7806, 6, 439.619446, -3500.730225, 110.534, 0, ''),
(7806, 7, 428.326385, -3495.874756, 118.662, 0, ''),
(7806, 8, 424.664032, -3489.381592, 121.999, 0, ''),
(7806, 9, 424.137299, -3470.952637, 124.333, 0, ''),
(7806, 10, 421.791107, -3449.242676, 119.126, 0, ''),
(7806, 11, 404.247070, -3429.376953, 117.644, 0, ''),
(7806, 12, 335.465271, -3430.717773, 116.456, 0, ''),
(7806, 13, 317.160126, -3426.708984, 116.226, 0, ''),
(7806, 14, 331.180115, -3464.002197, 117.143, 0, ''),
(7806, 15, 336.393616, -3501.877441, 118.201, 0, ''),
(7806, 16, 337.251312, -3544.764648, 117.284, 0, ''),
(7806, 17, 337.748932, -3565.415527, 116.797, 0, ''),
(7806, 18, 336.010925, -3597.363037, 118.225, 0, ''),
(7806, 19, 324.619141, -3622.884033, 119.811, 0, ''),
(7806, 20, 308.027466, -3648.600098, 123.047, 0, ''),
(7806, 21, 276.325409, -3685.738525, 128.356, 0, ''),
(7806, 22, 239.981064, -3717.330811, 131.874, 0, ''),
(7806, 23, 224.950974, -3730.169678, 132.125, 0, ''),
(7806, 24, 198.707870, -3768.292725, 129.420, 0, ''),
(7806, 25, 183.758316, -3791.068848, 128.045, 0, ''),
(7806, 26, 178.110657, -3801.575439, 128.370, 3000, 'SAY_OOX_DANGER'),
(7806, 27, 162.215225, -3827.014160, 129.424, 0, ''),
(7806, 28, 141.664734, -3864.519287, 131.419, 0, ''),
(7806, 29, 135.301697, -3880.089111, 132.120, 0, ''),
(7806, 30, 122.461151, -3910.071533, 135.605, 0, ''),
(7806, 31, 103.376175, -3937.725098, 137.342, 0, ''),
(7806, 32, 81.414474, -3958.614258, 138.469, 0, ''),
(7806, 33, 55.378139, -3982.004639, 136.520, 0, ''),
(7806, 34, 13.983131, -4013.952881, 126.903, 0, ''),
(7806, 35, -21.658007, -4048.713623, 118.068, 0, ''),
(7806, 36, -52.443058, -4081.209717, 117.477, 0, ''),
(7806, 37, -102.710854, -4116.760742, 118.666, 0, ''),
(7806, 38, -92.996193, -4135.847168, 119.310, 0, ''),
(7806, 39, -86.391273, -4153.331055, 122.502, 0, ''),
(7806, 40, -85.746086, -4163.600586, 121.892, 0, ''),
(7806, 41, -90.544006, -4183.577637, 117.587, 0, ''),
(7806, 42, -110.223564, -4205.861328, 121.878, 0, ''),
(7806, 43, -115.257607, -4211.962402, 121.878, 3000, 'SAY_OOX_DANGER'),
(7806, 44, -128.594650, -4233.343750, 117.766, 0, ''),
(7806, 45, -135.358917, -4258.120117, 117.562, 0, ''),
(7806, 46, -156.832428, -4258.961914, 120.059, 0, ''),
(7806, 47, -167.119873, -4274.102539, 117.062, 0, ''),
(7806, 48, -176.291016, -4287.594727, 118.721, 0, ''),
(7806, 49, -196.992981, -4315.815430, 117.588, 0, ''),
(7806, 50, -209.329300, -4331.671387, 115.142, 0, ''),
(7806, 51, -232.292236, -4356.015625, 108.543, 0, ''),
(7806, 52, -232.159683, -4370.904297, 102.815, 0, ''),
(7806, 53, -210.271133, -4389.896973, 84.167, 0, ''),
(7806, 54, -187.940186, -4407.532715, 70.987, 0, ''),
(7806, 55, -181.353577, -4418.771973, 64.778, 0, ''),
(7806, 56, -170.529861, -4440.438965, 58.943, 0, ''),
(7806, 57, -141.428543, -4465.323242, 45.963, 0, ''),
(7806, 58, -120.993629, -4487.088379, 32.075, 0, ''),
(7806, 59, -104.134621, -4501.837402, 25.051, 0, ''),
(7806, 60, -84.154663, -4529.436523, 11.952, 0, ''),
(7806, 61, -88.698898, -4544.626465, 9.055, 0, ''),
(7806, 62, -100.603447, -4575.034180, 11.388, 0, ''),
(7806, 63, -106.908669, -4600.407715, 11.046, 0, ''),
(7806, 64, -106.831703, -4620.503418, 11.057, 3000, 'SAY_OOX_COMPLETE');

DELETE FROM script_waypoint WHERE entry=7807;
INSERT INTO script_waypoint VALUES
(7807, 0, -4943.74, 1715.74, 62.74, 0, 'SAY_START'),
(7807, 1, -4944.93, 1706.66, 63.16, 0, ''),
(7807, 2, -4942.82, 1690.22, 64.25, 0, ''),
(7807, 3, -4946.47, 1669.62, 63.84, 0, ''),
(7807, 4, -4955.93, 1651.88, 63.00, 0, ''),
(7807, 5, -4967.58, 1643.86, 64.31, 0, ''),
(7807, 6, -4978.12, 1607.90, 64.30, 0, ''),
(7807, 7, -4975.38, 1596.16, 64.70, 0, ''),
(7807, 8, -4972.82, 1581.89, 61.75, 0, ''),
(7807, 9, -4958.65, 1581.05, 61.81, 0, ''),
(7807, 10, -4936.72, 1594.89, 65.96, 0, ''),
(7807, 11, -4885.69, 1598.10, 67.45, 4000, 'first ambush SAY_AMBUSH'),
(7807, 12, -4874.20, 1601.73, 68.54, 0, ''),
(7807, 13, -4816.64, 1594.47, 78.20, 0, ''),
(7807, 14, -4802.20, 1571.92, 87.01, 0, ''),
(7807, 15, -4746.40, 1576.11, 84.59, 0, ''),
(7807, 16, -4739.72, 1707.16, 94.04, 0, ''),
(7807, 17, -4674.03, 1840.44, 89.17, 0, ''),
(7807, 18, -4667.94, 1864.11, 85.18, 0, ''),
(7807, 19, -4668.08, 1886.39, 81.14, 0, ''),
(7807, 20, -4679.43, 1932.32, 73.76, 0, ''),
(7807, 21, -4674.17, 1946.66, 70.83, 5000, 'second ambush SAY_AMBUSH'),
(7807, 22, -4643.94, 1967.45, 65.27, 0, ''),
(7807, 23, -4595.60, 2010.75, 52.10, 0, ''),
(7807, 24, -4562.65, 2029.28, 45.41, 0, ''),
(7807, 25, -4538.56, 2032.65, 45.28, 0, ''),
(7807, 26, -4531.96, 2034.15, 48.34, 0, ''),
(7807, 27, -4507.75, 2039.32, 51.57, 0, ''),
(7807, 28, -4482.74, 2045.67, 48.15, 0, ''),
(7807, 29, -4460.87, 2051.54, 45.55, 0, ''),
(7807, 30, -4449.97, 2060.03, 45.51, 10000, 'third ambush SAY_AMBUSH'),
(7807, 31, -4448.99, 2079.05, 44.64, 0, ''),
(7807, 32, -4436.64, 2134.48, 28.83, 0, ''),
(7807, 33, -4429.25, 2170.20, 15.44, 0, ''),
(7807, 34, -4424.83, 2186.11, 11.48, 0, ''),
(7807, 35, -4416.71, 2209.76, 7.36, 0, ''),
(7807, 36, -4405.25, 2231.77, 5.94, 0, ''),
(7807, 37, -4377.61, 2265.45, 06.71, 15000, 'complete quest SAY_END');

DELETE FROM script_waypoint WHERE entry=9502;
INSERT INTO script_waypoint VALUES
(9502, 0,847.848, -230.067, -43.614, 0, ''),
(9502, 1,868.122, -223.884, -43.695, 0, 'YELL_PHALANX_AGGRO');

DELETE FROM script_waypoint WHERE entry=9503;
INSERT INTO script_waypoint VALUES
(9503, 0, 885.1852, -194.0071, -43.45835, 0,''),
(9503, 1, 885.1852, -194.0071, -43.45835, 0,''),
(9503, 2, 872.763550, -185.605621, -43.703655, 5000,'b1'),
(9503, 3, 867.923401, -188.006393, -43.703655, 5000,'b2'),
(9503, 4, 863.295898, -190.795212, -43.703655, 5000,'b3'),
(9503, 5, 856.139587, -194.652756, -43.703655, 5000,'b4'),
(9503, 6, 851.878906, -196.928131, -43.703655, 15000,'b5'),
(9503, 7, 877.035217, -187.048080, -43.703655, 0,''),
(9503, 8, 891.198000, -197.924000, -43.620400, 0,'home'),
(9503, 9, 876.9352, -189.0071, -43.45835, 0,'Nagmara escort'),
(9503, 10, 885.1852, -194.0071, -43.45835, 0,''),
(9503, 11, 869.1238, -202.8515, -43.70884, 0,''),
(9503, 12, 869.4652, -202.8777, -43.45879, 0,''),
(9503, 13, 864.2437, -210.8257, -43.45896, 0,''),
(9503, 14, 866.8236, -220.959, -43.44718, 0,''),
(9503, 15, 867.0736, -221.959, -43.44718, 0,''),
(9503, 16, 870.4187, -225.6747, -43.55664, 0,'open door'),
(9503, 17, 872.1687, -227.4247, -43.55664, 0,''),
(9503, 18, 872.9187, -228.1747, -43.55664, 0,''),
(9503, 19, 875.9187, -230.9247, -43.55664, 0,''),
(9503, 20, 876.9187, -230.1747, -43.55664, 0,''),
(9503, 21, 877.9187, -229.4247, -43.55664, 0,''),
(9503, 22, 882.3948, -225.949, -46.74049, 0,''),
(9503, 23, 885.8948, -223.699, -49.24049, 0,''),
(9503, 24, 887.6448, -222.449, -49.24049, 0,''),
(9503, 25, 885.937, -223.3513, -49.29544, 0,''),
(9503, 26, 887.437, -222.3513, -49.29544, 0,''),
(9503, 27, 888.937, -221.6013, -49.54544, 0,''),
(9503, 28, 887.687, -220.1013, -49.54544, 0,''),
(9503, 29, 886.687, -218.8513, -49.54544, 0,''),
(9503, 30, 887.5667, -220.0395, -49.70586, 0,''),
(9503, 31, 886.5667, -218.7895, -49.70586, 0,''),
(9503, 32, 886.0667, -218.2895, -49.70586, 0,''),
(9503, 33, 880.8252, -221.3895, -49.95622, 0,'stop');

DELETE FROM script_waypoint WHERE entry=9537;
INSERT INTO script_waypoint VALUES
(9537, 0, 854.9774, -150.3077, -49.671, 0, ''),
(9537, 1, 855.8161, -149.763, -49.671, 0, 'YELL_HURLEY_SPAWN'),
(9537, 2, 882.6298, -148.1658, -49.75969, 0, ''),
(9537, 3, 883.9604, -148.0866, -49.76004, 0, ''),
(9537, 4, 896.846, -147.319, -49.75753, 0, ''),
(9537, 5, 896.846, -147.319, -49.75753, 0, '');

DELETE FROM script_waypoint WHERE entry=9623;
INSERT INTO script_waypoint VALUES
(9623, 0, -6383.070801, -1964.368896, -258.709, 0, 'SAY_AME_START'),
(9623, 1, -6393.649414, -1949.572266, -261.449, 0, ''),
(9623, 2, -6397.846680, -1931.099609, -263.366, 0, ''),
(9623, 3, -6397.501953, -1921.470703, -263.876, 0, ''),
(9623, 4, -6389.630371, -1909.995361, -259.601, 0, ''),
(9623, 5, -6380.065430, -1905.452881, -255.858, 0, ''),
(9623, 6, -6373.437988, -1900.275024, -254.774, 0, ''),
(9623, 7, -6372.868652, -1893.500854, -255.678, 0, ''),
(9623, 8, -6379.730469, -1877.627808, -259.654, 0, ''),
(9623, 9, -6380.264160, -1871.139648, -260.617, 0, ''),
(9623, 10, -6373.830566, -1855.620361, -259.566, 0, ''),
(9623, 11, -6368.824707, -1847.770508, -259.246, 0, ''),
(9623, 12, -6370.902832, -1835.038940, -260.212, 0, ''),
(9623, 13, -6376.591309, -1821.592285, -260.856, 0, ''),
(9623, 14, -6381.931152, -1810.434326, -266.180, 0, ''),
(9623, 15, -6396.713867, -1807.123535, -269.329, 0, ''),
(9623, 16, -6400.266602, -1795.053589, -269.744, 0, ''),
(9623, 17, -6402.675781, -1747.514648, -272.961, 0, ''),
(9623, 18, -6396.997559, -1710.052979, -273.719, 0, ''),
(9623, 19, -6388.105957, -1676.328125, -272.133, 5000, 'SAY_AME_PROGRESS'),
(9623, 20, -6370.711914, -1638.638306, -272.031, 0, ''),
(9623, 21, -6366.709473, -1592.645996, -272.201, 0, ''),
(9623, 22, -6333.869629, -1534.598755, -270.493, 0, ''),
(9623, 23, -6305.362305, -1477.913330, -269.518, 0, ''),
(9623, 24, -6311.588867, -1419.017456, -267.622, 0, ''),
(9623, 25, -6330.014648, -1400.064331, -266.425, 0, ''),
(9623, 26, -6356.021973, -1392.607422, -267.123, 0, ''),
(9623, 27, -6370.859375, -1386.179321, -270.218, 0, ''),
(9623, 28, -6381.529785, -1369.780273, -272.110, 0, ''),
(9623, 29, -6405.381348, -1321.522827, -271.699, 0, ''),
(9623, 30, -6406.583496, -1307.574585, -271.802, 0, ''),
(9623, 31, -6386.325684, -1286.851074, -272.074, 0, ''),
(9623, 32, -6364.254883, -1264.706299, -269.075, 0, ''),
(9623, 33, -6343.636230, -1239.844360, -268.364, 0, ''),
(9623, 34, -6335.568848, -1202.449585, -271.515, 0, ''),
(9623, 35, -6325.625000, -1184.455322, -270.461, 0, ''),
(9623, 36, -6317.797363, -1177.668091, -269.792, 0, ''),
(9623, 37, -6303.024414, -1180.252686, -269.332, 0, 'SAY_AME_END'),
(9623, 38, -6301.975098, -1184.787842, -269.371, 1000, ''),
(9623, 39, -6297.575684, -1186.412964, -268.962, 5000, '');

DELETE FROM script_waypoint WHERE entry=10096;
INSERT INTO script_waypoint VALUES
(10096, 0, 604.802673, -191.081985, -54.058590, 0,'ring'),
(10096, 1, 604.072998, -222.106918, -52.743759, 0,'first gate'),
(10096, 2, 621.400391, -214.499054, -52.814453, 0,'hiding in corner'),
(10096, 3, 601.300781, -198.556992, -53.950256, 0,'ring'),
(10096, 4, 631.818359, -180.548126, -52.654770, 0,'second gate'),
(10096, 5, 627.390381, -201.075974, -52.692917, 0,'hiding in corner');

DELETE FROM script_waypoint WHERE entry=10427;
INSERT INTO script_waypoint VALUES
(10427, 0, -5185.463, -1185.927, 45.951, 0, ''),
(10427, 1, -5184.880, -1154.210, 45.035, 0, ''),
(10427, 2, -5175.880, -1126.526, 43.701, 0, ''),
(10427, 3, -5138.651, -1111.874, 44.024, 0, ''),
(10427, 4, -5134.728, -1104.796, 47.365, 0, ''),
(10427, 5, -5129.681, -1097.878, 49.449, 2500, ''),
(10427, 6, -5125.303, -1080.572, 47.033, 0, ''),
(10427, 7, -5146.668, -1053.694, 28.415, 0, ''),
(10427, 8, -5147.463, -1027.539, 13.818, 0, ''),
(10427, 9, -5139.238, -1018.889, 8.220, 0, ''),
(10427, 10, -5121.168, -1013.126, -0.619, 0, ''),
(10427, 11, -5091.919, -1014.205, -4.902, 0, ''),
(10427, 12, -5069.240, -994.299, -4.631, 0, ''),
(10427, 13, -5059.975, -944.112, -5.377, 0, ''),
(10427, 14, -5013.546, -906.184, -5.490, 0, ''),
(10427, 15, -4992.461, -920.983, -4.980, 5000, 'SAY_WYVERN'),
(10427, 16, -4976.355, -1002.997, -5.380, 0, ''),
(10427, 17, -4958.478, -1033.185, -5.433, 0, ''),
(10427, 18, -4953.353, -1052.211, -10.836, 0, ''),
(10427, 19, -4937.447, -1056.351, -22.139, 0, ''),
(10427, 20, -4908.455, -1050.433, -33.458, 0, ''),
(10427, 21, -4905.530, -1056.885, -33.722, 0, ''),
(10427, 22, -4920.830, -1073.284, -45.515, 0, ''),
(10427, 23, -4933.368, -1082.700, -50.186, 0, ''),
(10427, 24, -4935.313, -1092.353, -52.785, 0, ''),
(10427, 25, -4929.553, -1101.268, -50.637, 0, ''),
(10427, 26, -4920.679, -1100.028, -51.944, 10000, 'SAY_COMPLETE'),
(10427, 27, -4920.679, -1100.028, -51.944, 0, 'quest complete');

DELETE FROM script_waypoint WHERE entry=10638;
INSERT INTO script_waypoint VALUES
(10638, 0, -4903.521973, -1368.339844, -52.611, 5000, 'SAY_KAN_START'),
(10638, 1, -4906.004395, -1367.048096, -52.611, 0, '');

DELETE FROM script_waypoint WHERE entry=10646;
INSERT INTO script_waypoint VALUES
(10646, 0, -4792.401855, -2137.775146, 82.423, 0, ''),
(10646, 1, -4813.508301, -2141.543457, 80.774, 0, ''),
(10646, 2, -4828.630859, -2154.309814, 82.074, 0, ''),
(10646, 3, -4833.772949, -2149.182617, 81.676, 0, ''),
(10646, 4, -4846.418945, -2136.045410, 77.871, 0, ''),
(10646, 5, -4865.076660, -2116.549561, 76.483, 0, ''),
(10646, 6, -4888.434570, -2090.729248, 80.907, 0, ''),
(10646, 7, -4893.068359, -2085.468994, 82.094, 0, ''),
(10646, 8, -4907.256836, -2074.929932, 84.437, 5000, 'SAY_LAKO_LOOK_OUT'),
(10646, 9, -4899.899902, -2062.143555, 83.780, 0, ''),
(10646, 10, -4897.762207, -2056.520020, 84.184, 0, ''),
(10646, 11, -4888.331543, -2033.182495, 83.654, 0, ''),
(10646, 12, -4876.343750, -2003.916138, 90.887, 0, ''),
(10646, 13, -4872.227051, -1994.173340, 91.513, 0, ''),
(10646, 14, -4879.569336, -1976.985229, 92.185, 5000, 'SAY_LAKO_HERE_COME'),
(10646, 15, -4879.049316, -1964.349609, 92.001, 0, ''),
(10646, 16, -4874.720215, -1956.939819, 90.737, 0, ''),
(10646, 17, -4869.474609, -1952.612671, 89.206, 0, ''),
(10646, 18, -4842.466797, -1929.000732, 84.147, 0, ''),
(10646, 19, -4804.444824, -1897.302734, 89.362, 0, ''),
(10646, 20, -4798.072754, -1892.383545, 89.368, 0, ''),
(10646, 21, -4779.447754, -1882.759155, 90.169, 5000, 'SAY_LAKO_MORE'),
(10646, 22, -4762.081055, -1866.530640, 89.481, 0, ''),
(10646, 23, -4766.267090, -1861.867798, 87.847, 0, ''),
(10646, 24, -4782.929688, -1852.174683, 78.354, 0, ''),
(10646, 25, -4793.605469, -1850.961182, 77.658, 0, ''),
(10646, 26, -4803.323730, -1855.102661, 78.958, 0, ''),
(10646, 27, -4807.971680, -1854.501221, 77.743, 0, ''),
(10646, 28, -4837.212891, -1848.493408, 64.488, 0, ''),
(10646, 29, -4884.619629, -1840.401123, 56.219, 0, ''),
(10646, 30, -4889.705566, -1839.623291, 54.417, 0, ''),
(10646, 31, -4893.904297, -1843.685791, 53.012, 0, ''),
(10646, 32, -4903.142090, -1872.383545, 32.266, 0, ''),
(10646, 33, -4910.940918, -1879.864868, 29.940, 0, ''),
(10646, 34, -4920.047363, -1880.940796, 30.597, 0, ''),
(10646, 35, -4924.457031, -1881.447144, 29.292, 0, ''),
(10646, 36, -4966.120117, -1886.033081, 10.977, 0, ''),
(10646, 37, -4999.369629, -1890.847290, 4.430, 0, ''),
(10646, 38, -5007.271484, -1891.669678, 2.771, 0, ''),
(10646, 39, -5013.334473, -1879.588257, -1.947, 0, ''),
(10646, 40, -5023.328613, -1855.959961, -17.103, 0, ''),
(10646, 41, -5038.513184, -1825.986694, -35.821, 0, ''),
(10646, 42, -5048.733887, -1809.798218, -46.457, 0, ''),
(10646, 43, -5053.188965, -1791.682983, -57.186, 0, ''),
(10646, 44, -5062.093750, -1794.399780, -56.515, 0, ''),
(10646, 45, -5052.657227, -1797.044800, -54.734, 5000, 'SAY_LAKO_END');

DELETE FROM script_waypoint WHERE entry=11856;
INSERT INTO script_waypoint VALUES
(11856, 0, 113.91, -350.13, 4.55, 0, ''),
(11856, 1, 109.54, -350.08, 3.74, 0, ''),
(11856, 2, 106.95, -353.40, 3.60, 0, ''),
(11856, 3, 100.28, -338.89, 2.97, 0, ''),
(11856, 4, 110.11, -320.26, 3.47, 0, ''),
(11856, 5, 109.78, -287.80, 5.30, 0, ''),
(11856, 6, 105.02, -269.71, 4.71, 0, ''),
(11856, 7, 86.71, -251.81, 5.34, 0, ''),
(11856, 8, 64.10, -246.38, 5.91, 0, ''),
(11856, 9, -2.55, -243.58, 6.3, 0, ''),
(11856, 10, -27.78, -267.53, -1.08, 0, ''),
(11856, 11, -31.27, -283.54, -4.36, 0, ''),
(11856, 12, -28.96, -322.44, -9.19, 0, ''),
(11856, 13, -35.63, -360.03, -16.59, 0, ''),
(11856, 14, -58.30, -412.26, -30.60, 0, ''),
(11856, 15, -58.88, -474.17, -44.54, 0, ''),
(11856, 16, -45.92, -496.57, -46.26, 5000, 'AMBUSH'),
(11856, 17, -40.25, -510.07, -46.05, 0, ''),
(11856, 18, -38.88, -520.72, -46.06, 5000, 'END');

DELETE FROM script_waypoint WHERE entry=12423;
INSERT INTO script_waypoint VALUES
(12423, 0, -9509.72, -147.03, 58.74, 0, ''),
(12423, 1, -9517.07, -172.82, 58.66, 0, '');

DELETE FROM script_waypoint WHERE entry=12427;
INSERT INTO script_waypoint VALUES
(12427, 0, -5689.20, -456.44, 391.08, 0, ''),
(12427, 1, -5700.37, -450.77, 393.19, 0, '');

DELETE FROM script_waypoint WHERE entry=12428;
INSERT INTO script_waypoint VALUES
(12428, 0, 2454.09, 361.26, 31.51, 0, ''),
(12428, 1, 2472.03, 378.08, 30.98, 0, '');

DELETE FROM script_waypoint WHERE entry=12429;
INSERT INTO script_waypoint VALUES
(12429, 0, 9654.19, 909.58, 1272.11, 0, ''),
(12429, 1, 9642.53, 908.11, 1269.10, 0, '');

DELETE FROM script_waypoint WHERE entry=12430;
INSERT INTO script_waypoint VALUES
(12430, 0, 161.65, -4779.34, 14.64, 0, ''),
(12430, 1, 140.71, -4813.56, 17.04, 0, '');

DELETE FROM script_waypoint WHERE entry=12717;
INSERT INTO script_waypoint VALUES
(12717, 0, 3346.247070, 1007.879028, 3.590, 0, 'SAY_MUG_START2'),
(12717, 1, 3367.388428, 1011.505859, 3.720, 0, ''),
(12717, 2, 3418.636963, 1013.963684, 2.905, 0, ''),
(12717, 3, 3426.844971, 1015.097534, 3.449, 0, ''),
(12717, 4, 3437.025391, 1020.786194, 2.742, 0, ''),
(12717, 5, 3460.563721, 1024.256470, 1.353, 0, ''),
(12717, 6, 3479.869629, 1037.957153, 1.023, 0, ''),
(12717, 7, 3490.526367, 1043.346313, 3.338, 0, ''),
(12717, 8, 3504.282959, 1047.772339, 8.205, 0, ''),
(12717, 9, 3510.733398, 1049.790771, 12.143, 0, ''),
(12717, 10, 3514.411133, 1051.167725, 13.235, 0, ''),
(12717, 11, 3516.939697, 1052.911377, 12.918, 0, ''),
(12717, 12, 3523.635742, 1056.297485, 7.563, 0, ''),
(12717, 13, 3531.939941, 1059.863525, 6.175, 0, ''),
(12717, 14, 3535.475342, 1069.959473, 1.697, 0, ''),
(12717, 15, 3546.978027, 1093.485474, 0.680, 0, ''),
(12717, 16, 3549.729980, 1101.882446, -1.123, 0, ''),
(12717, 17, 3555.140137, 1116.985718, -4.326, 0, ''),
(12717, 18, 3571.940430, 1132.175781, -0.634, 0, ''),
(12717, 19, 3574.283203, 1137.575928, 3.684, 0, ''),
(12717, 20, 3579.312744, 1137.252319, 8.205, 0, ''),
(12717, 21, 3590.218994, 1143.646973, 8.291, 0, ''),
(12717, 22, 3595.972900, 1145.827148, 6.773, 0, ''),
(12717, 23, 3603.650391, 1146.920776, 9.763, 0, ''),
(12717, 24, 3607.081787, 1146.014282, 10.692, 5000, 'SAY_MUG_BRAZIER'),
(12717, 25, 3614.518555, 1142.629150, 10.248, 0, ''),
(12717, 26, 3616.660889, 1140.837036, 10.682, 3000, 'SAY_MUG_PATROL'),
(12717, 27, 3621.078613, 1138.109497, 10.369, 0, 'SAY_MUG_RETURN'),
(12717, 28, 3615.478516, 1145.525879, 9.614, 0, ''),
(12717, 29, 3607.188232, 1152.715942, 8.871, 0, '');

DELETE FROM script_waypoint WHERE entry=12818;
INSERT INTO script_waypoint VALUES
(12818, 0, 3347.250089, -694.700989, 159.925995, 0, ''),
(12818, 1, 3341.527039, -694.725891, 161.124542, 4000, ''),
(12818, 2, 3338.351074, -686.088138, 163.444000, 0, ''),
(12818, 3, 3352.744873, -677.721741, 162.316269, 0, ''),
(12818, 4, 3370.291016, -669.366943, 160.751358, 0, ''),
(12818, 5, 3381.479492, -659.449097, 162.545303, 0, ''),
(12818, 6, 3389.554199, -648.500000, 163.651825, 0, ''),
(12818, 7, 3396.645020, -641.508911, 164.216019, 0, ''),
(12818, 8, 3410.498535, -634.299622, 165.773453, 0, ''),
(12818, 9, 3418.461426, -631.791992, 166.477615, 0, ''),
(12818, 10, 3429.500000, -631.588745, 166.921265, 0, ''),
(12818, 11,3434.950195, -629.245483, 168.333969, 0, ''),
(12818, 12,3438.927979, -618.503235, 171.503143, 0, ''),
(12818, 13,3444.217529, -609.293640, 173.077972, 1000, 'Ambush 1'),
(12818, 14,3460.505127, -593.794189, 174.342255, 0, ''),
(12818, 15,3480.283203, -578.210327, 176.652313, 0, ''),
(12818, 16,3492.912842, -562.335449, 181.396301, 0, ''),
(12818, 17,3495.230957, -550.977600, 184.652267, 0, ''),
(12818, 18,3496.247070, -529.194214, 188.172028, 0, ''),
(12818, 19,3497.619385, -510.411499, 188.345322, 1000, 'Ambush 2'),
(12818, 20,3498.498047, -497.787506, 185.806274, 0, ''),
(12818, 21,3484.218750, -489.717529, 182.389862, 4000, '');

DELETE FROM script_waypoint WHERE entry=12858;
INSERT INTO script_waypoint VALUES
(12858, 0, 1782.63, -2241.11, 109.73, 5000, ''),
(12858, 1, 1788.88, -2240.17, 111.71, 0, ''),
(12858, 2, 1797.49, -2238.11, 112.31, 0, ''),
(12858, 3, 1803.83, -2232.77, 111.22, 0, ''),
(12858, 4, 1806.65, -2217.83, 107.36, 0, ''),
(12858, 5, 1811.81, -2208.01, 107.45, 0, ''),
(12858, 6, 1820.85, -2190.82, 100.49, 0, ''),
(12858, 7, 1829.60, -2177.49, 96.44, 0, ''),
(12858, 8, 1837.98, -2164.19, 96.71, 0, 'prepare'),
(12858, 9, 1839.99, -2149.29, 96.78, 0, ''),
(12858, 10, 1835.14, -2134.98, 96.80, 0, ''),
(12858, 11, 1823.57, -2118.27, 97.43, 0, ''),
(12858, 12, 1814.99, -2110.35, 98.38, 0, ''),
(12858, 13, 1806.60, -2103.09, 99.19, 0, ''),
(12858, 14, 1798.27, -2095.77, 100.04, 0, ''),
(12858, 15, 1783.59, -2079.92, 100.81, 0, ''),
(12858, 16, 1776.79, -2069.48, 101.77, 0, ''),
(12858, 17, 1776.82, -2054.59, 109.82, 0, ''),
(12858, 18, 1776.88, -2047.56, 109.83, 0, ''),
(12858, 19, 1776.86, -2036.55, 109.83, 0, ''),
(12858, 20, 1776.90, -2024.56, 109.83, 0, 'win'),
(12858, 21, 1776.87, -2028.31, 109.83,60000, 'stay'),
(12858, 22, 1776.90, -2028.30, 109.83, 0, '');

DELETE FROM script_waypoint WHERE entry=15420;
INSERT INTO script_waypoint VALUES
(15420, 0, 9294.78, -6682.51, 22.42, 0, ''),
(15420, 1, 9298.27, -6667.99, 22.42, 0, ''),
(15420, 2, 9309.63, -6658.84, 22.43, 0, ''),
(15420, 3, 9304.43, -6649.31, 26.46, 0, ''),
(15420, 4, 9298.83, -6648.00, 28.61, 0, ''),
(15420, 5, 9291.06, -6653.46, 31.83,2500, ''),
(15420, 6, 9289.08, -6660.17, 31.85,5000, ''),
(15420, 7, 9291.06, -6653.46, 31.83, 0, '');

DELETE FROM script_waypoint WHERE entry=16295;
INSERT INTO script_waypoint VALUES
(16295, 0, 7545.070000, -7359.870000, 162.354000, 4000, 'SAY_START'),
(16295, 1, 7550.048340, -7362.237793, 162.235657, 0, ''),
(16295, 2, 7566.976074, -7364.315430, 161.738770, 0, ''),
(16295, 3, 7578.830566, -7361.677734, 161.738770, 0, ''),
(16295, 4, 7590.969238, -7359.053711, 162.257660, 0, ''),
(16295, 5, 7598.354004, -7362.815430, 162.256683, 4000, 'SAY_PROGRESS_1'),
(16295, 6, 7605.861328, -7380.424316, 161.937073, 0, ''),
(16295, 7, 7605.295410, -7387.382813, 157.253998, 0, ''),
(16295, 8, 7606.131836, -7393.893555, 156.941925, 0, ''),
(16295, 9, 7615.207520, -7400.187012, 157.142639, 0, ''),
(16295, 10, 7618.956543, -7402.652832, 158.202042, 0, ''),
(16295, 11, 7636.850586, -7401.756836, 162.144791, 0, 'SAY_PROGRESS_2'),
(16295, 12, 7637.058105, -7404.944824, 162.206970, 4000, ''),
(16295, 13, 7636.910645, -7412.585449, 162.366425, 0, ''),
(16295, 14, 7637.607910, -7425.591797, 162.630661, 0, ''),
(16295, 15, 7637.816895, -7459.057129, 163.302704, 0, ''),
(16295, 16, 7638.859863, -7470.902344, 162.517059, 0, ''),
(16295, 17, 7641.395996, -7488.217285, 157.381287, 0, ''),
(16295, 18, 7634.455566, -7505.451660, 154.682159, 0, 'SAY_PROGRESS_3'),
(16295, 19, 7631.906738, -7516.948730, 153.597382, 0, ''),
(16295, 20, 7622.231445, -7537.037598, 151.587112, 0, ''),
(16295, 21, 7610.921875, -7550.670410, 149.639374, 0, ''),
(16295, 22, 7598.229004, -7562.551758, 145.953888, 0, ''),
(16295, 23, 7588.509277, -7577.755371, 148.294479, 0, ''),
(16295, 24, 7567.339355, -7608.456055, 146.006485, 0, ''),
(16295, 25, 7562.547852, -7617.417969, 148.097504, 0, ''),
(16295, 26, 7561.508789, -7645.064453, 151.245163, 0, ''),
(16295, 27, 7563.337402, -7654.652344, 151.227158, 0, ''),
(16295, 28, 7565.533691, -7658.296387, 151.248886, 0, ''),
(16295, 39, 7571.155762, -7659.118652, 151.244568, 0, ''),
(16295, 30, 7579.119629, -7662.213867, 151.651505, 0, 'quest complete'),
(16295, 31, 7603.768066, -7667.000488, 153.997726, 0, ''),
(16295, 32, 7603.768066, -7667.000488, 153.997726, 4000, 'SAY_END_1'),
(16295, 33, 7603.768066, -7667.000488, 153.997726, 8000, 'SAY_END_2'),
(16295, 34, 7603.768066, -7667.000488, 153.997726, 0, '');

DELETE FROM script_waypoint WHERE entry=16812;
INSERT INTO script_waypoint VALUES
(16812, 0, -10868.260, -1779.836, 90.476, 2500, 'Open door, begin walking'),
(16812, 1, -10875.585, -1779.581, 90.478, 0, ''),
(16812, 2, -10887.447, -1779.258, 90.476, 0, ''),
(16812, 3, -10894.592, -1780.668, 90.476, 0, ''),
(16812, 4, -10895.015, -1782.036, 90.476, 2500, 'Begin Speech after this'),
(16812, 5, -10894.592, -1780.668, 90.476, 0, 'Resume walking (back to spawn point now) after speech'),
(16812, 6, -10887.447, -1779.258, 90.476, 0, ''),
(16812, 7, -10875.585, -1779.581, 90.478, 0, ''),
(16812, 8, -10868.260, -1779.836, 90.476, 5000, 'close door'),
(16812, 9, -10866.799, -1780.958, 90.470, 2000, 'Summon mobs, open curtains');

DELETE FROM script_waypoint WHERE entry=16993;
INSERT INTO script_waypoint VALUES
(16993, 0, -1137.72, 4272.10, 14.04, 5000, ''),
(16993, 1, -1141.34, 4232.42, 14.63, 0, ''),
(16993, 2, -1133.47, 4220.88, 11.78, 0, ''),
(16993, 3, -1126.18, 4213.26, 13.51, 0, ''),
(16993, 4, -1100.12, 4204.32, 16.41, 0, ''),
(16993, 5, -1063.68, 4197.92, 15.51, 0, ''),
(16993, 6, -1027.28, 4194.36, 15.52, 0, ''),
(16993, 7, -995.68, 4189.59, 19.84, 0, ''),
(16993, 8, -970.90, 4188.60, 24.61, 0, ''),
(16993, 9, -961.93, 4193.34, 26.11, 15000, 'Summon 1'),
(16993, 10, -935.52, 4210.99, 31.98, 0, ''),
(16993, 11, -913.42, 4218.27, 37.29, 0, ''),
(16993, 12, -896.53, 4207.73, 43.23, 0, ''),
(16993, 13, -868.49, 4194.77, 46.75, 30000, 'Kneel and Rest Here'),
(16993, 14, -852.83, 4198.29, 47.28, 15000, 'Summon 2'),
(16993, 15, -819.85, 4200.50, 46.37, 0, ''),
(16993, 16, -791.92, 4201.96, 44.19, 0, ''),
(16993, 17, -774.42, 4202.46, 47.41, 0, ''),
(16993, 18, -762.90, 4202.17, 48.81, 0, ''),
(16993, 19, -728.25, 4195.35, 50.68, 0, ''),
(16993, 20, -713.58, 4192.07, 53.98, 0, ''),
(16993, 21, -703.09, 4189.74, 56.96, 0, ''),
(16993, 22, -693.70, 4185.43, 57.06, 0, ''),
(16993, 23, -686.38, 4159.81, 60.26, 0, ''),
(16993, 24, -679.88, 4147.04, 64.20, 0, ''),
(16993, 25, -656.74, 4147.72, 64.11, 0, ''),
(16993, 26, -652.22, 4137.50, 64.58, 0, ''),
(16993, 27, -649.99, 4136.38, 64.63, 30000, 'Quest Credit');

DELETE FROM script_waypoint WHERE entry=17077;
INSERT INTO script_waypoint VALUES
(17077, 0, -16.950142, 3801.409424, 95.064, 5000, 'EMOTE_WOLF_LIFT_HEAD'),
(17077, 1, -15.577404, 3805.170898, 94.833, 2500, ''),
(17077, 2, -20.011766, 3806.609863, 92.476, 5000, 'EMOTE_WOLF_HOWL'),
(17077, 3, -18.594666, 3816.207764, 91.482, 0, ''),
(17077, 4, -19.293468, 3838.218750, 85.012, 0, ''),
(17077, 5, -16.504408, 3871.034668, 82.327, 0, ''),
(17077, 6, 2.064510, 3898.678711, 85.623, 0, ''),
(17077, 7, 16.403864, 3921.174072, 86.024, 0, ''),
(17077, 8, 47.307926, 3932.001465, 83.302, 0, ''),
(17077, 9, 90.067230, 3942.906250, 77.000, 0, ''),
(17077, 10, 106.886024, 3944.388428, 76.502, 0, ''),
(17077, 11, 139.085480, 3941.897217, 80.617, 0, ''),
(17077, 12, 150.092346, 3942.782959, 80.399, 0, ''),
(17077, 13, 193.511475, 3950.396484, 74.366, 0, ''),
(17077, 14, 226.274948, 3958.003418, 73.257, 0, ''),
(17077, 15, 246.686981, 3963.309326, 76.376, 0, ''),
(17077, 16, 264.206177, 3977.726563, 83.704, 0, ''),
(17077, 17, 279.857422, 3986.417236, 88.245, 0, ''),
(17077, 18, 304.039642, 3998.354004, 95.649, 0, ''),
(17077, 19, 328.071503, 3995.832764, 104.434, 0, ''),
(17077, 20, 347.485229, 3990.817627, 113.608, 0, ''),
(17077, 21, 351.257202, 3954.260254, 125.747, 0, ''),
(17077, 22, 345.625977, 3932.016113, 132.358, 0, ''),
(17077, 23, 347.971893, 3908.549561, 135.520, 0, ''),
(17077, 24, 351.887878, 3891.062744, 139.957, 0, ''),
(17077, 25, 346.116852, 3864.634277, 146.647, 0, ''),
(17077, 26, 330.012360, 3839.859375, 154.148, 0, ''),
(17077, 27, 297.250610, 3811.855225, 166.893, 0, ''),
(17077, 28, 290.783112, 3800.188477, 172.130, 0, ''),
(17077, 29, 288.125427, 3782.474365, 180.825, 0, ''),
(17077, 30, 296.817841, 3771.629639, 184.961, 0, ''),
(17077, 31, 305.256256, 3765.380615, 185.360, 0, ''),
(17077, 32, 311.447906, 3757.902100, 184.312, 0, ''),
(17077, 33, 325.258026, 3730.282227, 184.076, 0, ''),
(17077, 34, 341.158630, 3717.757080, 183.904, 0, ''),
(17077, 35, 365.589020, 3717.200684, 183.902, 0, ''),
(17077, 36, 387.395081, 3731.750732, 183.645, 0, ''),
(17077, 37, 396.574127, 3732.604248, 179.831, 0, ''),
(17077, 38, 404.303192, 3737.313232, 180.151, 0, ''),
(17077, 39, 410.995972, 3742.286865, 183.364, 0, ''),
(17077, 40, 434.904541, 3761.058838, 186.219, 0, ''),
(17077, 41, 460.128815, 3774.436768, 186.348, 0, ''),
(17077, 42, 467.643951, 3788.506104, 186.446, 0, ''),
(17077, 43, 491.551666, 3815.446777, 189.848, 0, ''),
(17077, 44, 496.957855, 3836.875244, 193.078, 0, ''),
(17077, 45, 502.889191, 3855.458740, 194.834, 0, ''),
(17077, 46, 508.208466, 3863.689453, 194.024, 0, ''),
(17077, 47, 528.907593, 3887.348633, 189.762, 0, ''),
(17077, 48, 527.722229, 3890.686523, 189.240, 0, ''),
(17077, 49, 524.637329, 3891.768066, 189.149, 0, ''),
(17077, 50, 519.146057, 3886.701660, 190.128, 60000, 'SAY_WOLF_WELCOME');

DELETE FROM script_waypoint WHERE entry=17312;
INSERT INTO script_waypoint VALUES
(17312, 0, -4781.36, -11054.59, 2.475, 5000, 'SAY_START'),
(17312, 1, -4786.90, -11050.34, 3.868, 0, ''),
(17312, 2, -4830.30, -11043.16, 1.651, 0, ''),
(17312, 3, -4833.82, -11038.03, 1.607, 0, ''),
(17312, 4, -4856.62, -11034.52, 1.665, 0, ''),
(17312, 5, -4876.67, -11030.61, 1.942, 0, ''),
(17312, 6, -4881.06, -11035.62, 5.067, 0, ''),
(17312, 7, -4904.61, -11042.72, 10.689, 0, ''),
(17312, 8, -4913.10, -11050.98, 12.655, 0, ''),
(17312, 9, -4924.45, -11059.15, 14.604, 0, ''),
(17312, 10, -4939.89, -11066.83, 13.770, 0, ''),
(17312, 11, -4951.94, -11067.57, 14.174, 0, ''),
(17312, 12, -4971.94, -11064.94, 16.977, 0, ''),
(17312, 13, -4995.02, -11068.19, 21.874, 0, ''),
(17312, 14, -5017.98, -11055.18, 22.739, 0, ''),
(17312, 15, -5036.87, -11039.66, 25.646, 0, ''),
(17312, 16, -5064.00, -11026.98, 27.466, 0, ''),
(17312, 17, -5086.48, -11029.29, 28.866, 0, ''),
(17312, 18, -5108.68, -11034.53, 29.540, 0, ''),
(17312, 19, -5133.68, -11038.84, 29.169, 0, ''),
(17312, 20, -5163.79, -11042.22, 28.070, 4000, 'SAY_PROGRESS'),
(17312, 21, -5161.12, -11052.87, 31.664, 0, ''),
(17312, 22, -5160.37, -11066.50, 31.499, 0, ''),
(17312, 23, -5165.48, -11071.66, 27.989, 0, ''),
(17312, 24, -5177.59, -11076.63, 21.986, 0, ''),
(17312, 25, -5193.23, -11084.35, 20.009, 0, ''),
(17312, 26, -5207.54, -11090.39, 21.519, 0, ''),
(17312, 27, -5252.69, -11098.85, 15.572, 0, ''),
(17312, 28, -5268.33, -11105.29, 12.530, 0, ''),
(17312, 29, -5282.29, -11113.06, 7.134, 0, ''),
(17312, 30, -5302.01, -11127.28, 6.704, 0, ''),
(17312, 31, -5317.62, -11139.31, 4.684, 0, ''),
(17312, 32, -5319.09, -11140.54, 5.570, 0, ''),
(17312, 33, -5344.95, -11163.43, 6.959, 2000, 'SAY_END1'),
(17312, 34, -5344.95, -11163.43, 6.959, 1000, 'SAY_DAUGHTER'),
(17312, 35, -5355.86, -11173.78, 6.853, 2000, 'EMOTE_HUG'),
(17312, 36, -5355.86, -11173.78, 6.853, 5000, 'SAY_END2'),
(17312, 37, -5355.86, -11173.78, 6.853, 0, '');

DELETE FROM script_waypoint WHERE entry=17876;
INSERT INTO script_waypoint VALUES
(17876, 0, 2230.91, 118.765, 82.2947, 2000, 'open the prison door'),
(17876, 1, 2230.33, 114.980, 82.2946, 0, ''),
(17876, 2, 2233.36, 111.057, 82.2996, 0, ''),
(17876, 3, 2231.17, 108.486, 82.6624, 0, ''),
(17876, 4, 2220.22, 114.605, 89.4264, 0, ''),
(17876, 5, 2215.23, 115.990, 89.4549, 0, ''),
(17876, 6, 2210.00, 106.849, 89.4549, 0, ''),
(17876, 7, 2205.66, 105.234, 89.4549, 0, ''),
(17876, 8, 2192.26, 112.618, 89.4549, 2000, 'SAY_ARMORER_CALL_GUARDS'),
(17876, 9, 2185.32, 116.593, 89.4548, 2000, 'SAY_TH_ARMORER_HIT'),
(17876, 10, 2182.11, 120.328, 89.4548, 3000, 'SAY_TH_ARMORY_1'),
(17876, 11, 2182.11, 120.328, 89.4548, 5000, ''),
(17876, 12, 2182.11, 120.328, 89.4548, 3000, 'SAY_TH_ARMORY_2'),
(17876, 13, 2189.44, 113.922, 89.4549, 0, ''),
(17876, 14, 2195.63, 110.584, 89.4549, 0, ''),
(17876, 15, 2201.09, 115.115, 89.4549, 0, ''),
(17876, 16, 2204.34, 121.036, 89.4355, 0, ''),
(17876, 17, 2208.66, 129.127, 87.9560, 0, 'first ambush'),
(17876, 18, 2193.09, 137.940, 88.2164, 0, ''),
(17876, 19, 2173.39, 149.064, 87.9227, 0, ''),
(17876, 20, 2164.25, 137.965, 85.0595, 0, 'second ambush'),
(17876, 21, 2149.31, 125.645, 77.0858, 0, ''),
(17876, 22, 2142.78, 127.173, 75.5954, 0, ''),
(17876, 23, 2139.28, 133.952, 73.6386, 0, 'third ambush'),
(17876, 24, 2139.54, 155.235, 67.1269, 0, ''),
(17876, 25, 2145.38, 167.551, 64.8974, 0, 'fourth ambush'),
(17876, 26, 2134.28, 175.304, 67.9446, 0, ''),
(17876, 27, 2118.08, 187.387, 68.8141, 0, ''),
(17876, 28, 2105.88, 195.461, 65.1854, 0, ''),
(17876, 29, 2096.77, 196.939, 65.2117, 0, ''),
(17876, 30, 2083.90, 209.395, 64.8736, 0, ''),
(17876, 31, 2063.40, 229.509, 64.4883, 0, 'summon Skarloc'),
(17876, 32, 2063.40, 229.509, 64.4883, 10000, 'SAY_SKARLOC_ENTER'),
(17876, 33, 2063.40, 229.509, 64.4883, 5000, 'attack Skarloc'),
(17876, 34, 2063.40, 229.509, 64.4883, 0, 'gossip after skarloc'),
(17876, 35, 2046.70, 251.941, 62.7851, 4000, 'mount up'),
(17876, 36, 2046.70, 251.941, 62.7851, 3000, 'SAY_TH_MOUNTS_UP'),
(17876, 37, 2011.77, 278.478, 65.3388, 0, ''),
(17876, 38, 2005.08, 289.676, 66.1179, 0, ''),
(17876, 39, 2033.11, 337.450, 66.0948, 0, ''),
(17876, 40, 2070.30, 416.208, 66.0893, 0, ''),
(17876, 41, 2086.76, 469.768, 65.9182, 0, ''),
(17876, 42, 2101.70, 497.955, 61.7881, 0, ''),
(17876, 43, 2133.39, 530.933, 55.3700, 0, ''),
(17876, 44, 2157.91, 559.635, 48.5157, 0, ''),
(17876, 45, 2167.34, 586.191, 42.4394, 0, ''),
(17876, 46, 2174.17, 637.643, 33.9002, 0, ''),
(17876, 47, 2179.31, 656.053, 34.723, 0, ''),
(17876, 48, 2183.65, 670.941, 34.0318, 0, ''),
(17876, 49, 2201.50, 668.616, 36.1236, 0, ''),
(17876, 50, 2221.56, 652.747, 36.6153, 0, ''),
(17876, 51, 2238.97, 640.125, 37.2214, 0, ''),
(17876, 52, 2251.17, 620.574, 40.1473, 0, ''),
(17876, 53, 2261.98, 595.303, 41.4117, 0, ''),
(17876, 54, 2278.67, 560.172, 38.9090, 0, ''),
(17876, 55, 2336.72, 528.327, 40.9369, 0, ''),
(17876, 56, 2381.04, 519.612, 37.7312, 0, ''),
(17876, 57, 2412.20, 515.425, 39.2068, 0, ''),
(17876, 58, 2452.39, 516.174, 42.9387, 0, ''),
(17876, 59, 2467.38, 539.389, 47.4992, 0, ''),
(17876, 60, 2470.70, 554.333, 46.6668, 0, ''),
(17876, 61, 2478.07, 575.321, 55.4549, 0, ''),
(17876, 62, 2480.00, 585.408, 56.6921, 0, ''),
(17876, 63, 2482.67, 608.817, 55.6643, 0, ''),
(17876, 64, 2485.62, 626.061, 58.0132, 2000, 'dismount'),
(17876, 65, 2486.91, 626.356, 58.0761, 2000, 'EMOTE_TH_STARTLE_HORSE'),
(17876, 66, 2486.91, 626.356, 58.0761, 0, 'gossip before barn'),
(17876, 67, 2488.58, 660.940, 57.3913, 0, ''),
(17876, 68, 2502.56, 686.059, 55.6252, 0, ''),
(17876, 69, 2502.08, 694.360, 55.5083, 0, ''),
(17876, 70, 2491.46, 694.321, 55.7163, 0, 'enter barn'),
(17876, 71, 2491.10, 703.300, 55.7630, 0, ''),
(17876, 72, 2485.64, 702.992, 55.7917, 0, ''),
(17876, 73, 2479.63, 696.521, 55.7901, 0, 'spawn mobs'),
(17876, 74, 2476.24, 696.204, 55.8093, 0, 'start dialogue'),
(17876, 75, 2475.39, 695.983, 55.8146, 0, ''),
(17876, 76, 2477.75, 694.473, 55.7945, 0, ''),
(17876, 77, 2481.27, 697.747, 55.7910, 0, ''),
(17876, 78, 2486.31, 703.131, 55.7861, 0, ''),
(17876, 79, 2490.76, 703.511, 55.7662, 0, ''),
(17876, 80, 2491.30, 694.792, 55.7195, 0, 'exit barn'),
(17876, 81, 2502.08, 694.360, 55.5083, 0, ''),
(17876, 82, 2507.99, 679.298, 56.3760, 0, ''),
(17876, 83, 2524.79, 669.919, 54.9258, 0, ''),
(17876, 84, 2543.19, 665.289, 56.2957, 0, ''),
(17876, 85, 2566.49, 664.354, 54.5034, 0, ''),
(17876, 86, 2592.00, 664.611, 56.4394, 0, ''),
(17876, 87, 2614.43, 663.806, 55.3921, 2000, ''),
(17876, 88, 2616.14, 665.499, 55.1610, 0, ''),
(17876, 89, 2623.56, 666.965, 54.3983, 0, ''),
(17876, 90, 2629.99, 661.059, 54.2738, 0, ''),
(17876, 91, 2629.00, 656.982, 56.0651, 0, 'enter the church'),
(17876, 92, 2620.84, 633.007, 56.0300, 3000, 'SAY_TH_CHURCH_ENTER'),
(17876, 93, 2620.84, 633.007, 56.0300, 5000, 'church ambush'),
(17876, 94, 2620.84, 633.007, 56.0300, 0, 'SAY_TH_CHURCH_END'),
(17876, 95, 2622.99, 639.178, 56.0300, 0, ''),
(17876, 96, 2628.73, 656.693, 56.0610, 0, ''),
(17876, 97, 2630.34, 661.135, 54.2738, 0, ''),
(17876, 98, 2635.38, 672.243, 54.4508, 0, ''),
(17876, 99, 2644.13, 668.158, 55.3797, 0, ''),
(17876, 100, 2646.82, 666.740, 56.9898, 0, ''),
(17876, 101, 2658.22, 665.432, 57.1725, 0, ''),
(17876, 102, 2661.88, 674.849, 57.1725, 0, ''),
(17876, 103, 2656.23, 677.208, 57.1725, 0, ''),
(17876, 104, 2652.28, 670.270, 61.9353, 0, ''),
(17876, 105, 2650.79, 664.290, 61.9302, 0, 'inn ambush'),
(17876, 106, 2660.48, 659.409, 61.9370, 5000, 'SAY_TA_ESCAPED'),
(17876, 107, 2660.48, 659.409, 61.9370, 0, 'SAY_TH_MEET_TARETHA - gossip before epoch'),
(17876, 108, 2660.48, 659.409, 61.9370, 0, 'SAY_EPOCH_ENTER1'),
(17876, 109, 2650.62, 666.643, 61.9305, 0, ''),
(17876, 110, 2652.37, 670.561, 61.9368, 0, ''),
(17876, 111, 2656.05, 676.761, 57.1727, 0, ''),
(17876, 112, 2658.49, 677.166, 57.1727, 0, ''),
(17876, 113, 2659.28, 667.117, 57.1727, 0, ''),
(17876, 114, 2649.71, 665.387, 57.1727, 0, ''),
(17876, 115, 2634.79, 672.964, 54.4577, 0, 'outside inn'),
(17876, 116, 2635.06, 673.892, 54.4713, 18000, 'SAY_EPOCH_ENTER3'),
(17876, 117, 2635.06, 673.892, 54.4713, 0, 'fight begins'),
(17876, 118, 2635.06, 673.892, 54.4713, 0, 'fight ends'),
(17876, 119, 2634.30, 661.698, 54.4147, 0, 'run off'),
(17876, 120, 2652.21, 644.396, 56.1906, 0, '');

DELETE FROM script_waypoint WHERE entry=17969;
INSERT INTO script_waypoint VALUES
(17969, 0, -930.048950, 5288.080078, 23.848402, 0, ''),
(17969, 1, -925.677917, 5296.482910, 18.183748, 0, ''),
(17969, 2, -924.297180, 5299.016113, 17.710915, 0, ''),
(17969, 3, -928.390076, 5317.022949, 18.208593, 0, ''),
(17969, 4, -930.620972, 5329.915039, 18.773422, 0, 'SAY_AMBUSH1'),
(17969, 5, -931.490295, 5357.654785, 18.027155, 0, 'SAY_PROGRESS'),
(17969, 6, -934.777771, 5369.341797, 22.278048, 0, ''),
(17969, 7, -934.521851, 5373.407227, 22.834690, 0, ''),
(17969, 8, -937.008545, 5382.980469, 22.699078, 0, ''),
(17969, 9, -941.948059, 5404.141602, 22.669743, 0, ''),
(17969, 10, -931.244263, 5415.846680, 23.063961, 0, 'at crossroad'),
(17969, 11, -901.497925, 5420.315430, 24.213270, 0, ''),
(17969, 12, -860.311707, 5415.617676, 23.671139, 0, ''),
(17969, 13, -777.988953, 5391.982422, 23.001669, 0, ''),
(17969, 14, -750.362000, 5385.786621, 22.765791, 0, ''),
(17969, 15, -731.339417, 5382.449707, 22.517065, 0, ''),
(17969, 16, -681.235901, 5381.377930, 22.050159, 2500, 'end bridge SAY_AMBUSH2'),
(17969, 17, -637.944458, 5384.338379, 22.205647, 0, 'SAY_END'),
(17969, 18, -608.954407, 5408.715332, 21.630386, 0, ''),
(17969, 19, -598.134277, 5413.608398, 21.412275, 0, ''),
(17969, 20, -571.268982, 5420.771973, 21.184925, 0, ''),
(17969, 21, -553.099915, 5424.616211, 21.193716, 0, ''),
(17969, 22, -524.745483, 5443.945313, 20.977013, 0, ''),
(17969, 23, -502.984985, 5446.283691, 22.149435, 0, ''),
(17969, 24, -472.463959, 5449.546875, 22.561453, 0, ''),
(17969, 25, -454.533264, 5461.302246, 22.602837, 30000, 'quest complete');

DELETE FROM script_waypoint WHERE entry=18209;
INSERT INTO script_waypoint VALUES
(18209, 0, -1518.092407, 8465.188477, -4.102, 0, ''),
(18209, 1, -1516.741699, 8472.000977, -4.101, 0, ''),
(18209, 2, -1516.330444, 8473.119141, -4.102, 0, ''),
(18209, 3, -1514.117310, 8476.740234, -4.100, 0, ''),
(18209, 4, -1512.199951, 8481.147461, -4.015, 0, ''),
(18209, 5, -1514.709839, 8488.281250, -3.544, 0, ''),
(18209, 6, -1516.556274, 8495.236328, -2.463, 0, ''),
(18209, 7, -1515.730957, 8506.528320, -0.609, 7000, 'SAY_KUR_AMBUSH'),
(18209, 8, -1505.038940, 8513.247070, 0.672, 0, ''),
(18209, 9, -1476.161133, 8496.066406, 2.157, 0, ''),
(18209, 10, -1464.450684, 8492.601563, 3.529, 0, ''),
(18209, 11, -1457.568359, 8492.183594, 4.449, 0, ''),
(18209, 12, -1444.100342, 8499.031250, 6.177, 0, ''),
(18209, 13, -1426.472168, 8510.116211, 7.686, 0, ''),
(18209, 14, -1403.685303, 8524.146484, 9.680, 0, ''),
(18209, 15, -1384.890503, 8542.014648, 11.180, 0, ''),
(18209, 16, -1385.107422, 8547.194336, 11.297, 5000, 'SAY_KUR_COMPLETE'),
(18209, 17, -1387.814453, 8556.652344, 11.735, 0, ''),
(18209, 18, -1397.817749, 8574.999023, 13.204, 0, ''),
(18209, 19, -1411.961304, 8598.225586, 14.990, 0, '');

DELETE FROM script_waypoint WHERE entry=18210;
INSERT INTO script_waypoint VALUES
(18210, 0, -1581.410034, 8557.933594, 2.726, 0, ''),
(18210, 1, -1579.908447, 8553.716797, 2.559, 0, ''),
(18210, 2, -1577.829102, 8549.880859, 2.001, 0, ''),
(18210, 3, -1571.161987, 8543.494141, 2.001, 0, ''),
(18210, 4, -1563.944824, 8530.334961, 1.605, 0, ''),
(18210, 5, -1554.565552, 8518.413086, 0.364, 0, ''),
(18210, 6, -1549.239136, 8515.518555, 0.293, 0, ''),
(18210, 7, -1518.490112, 8516.771484, 0.683, 2000, 'SAY_MAG_MORE'),
(18210, 8, -1505.038940, 8513.247070, 0.672, 0, ''),
(18210, 9, -1476.161133, 8496.066406, 2.157, 0, ''),
(18210, 10, -1464.450684, 8492.601563, 3.529, 0, ''),
(18210, 11, -1457.568359, 8492.183594, 4.449, 0, ''),
(18210, 12, -1444.100342, 8499.031250, 6.177, 0, ''),
(18210, 13, -1426.472168, 8510.116211, 7.686, 0, ''),
(18210, 14, -1403.685303, 8524.146484, 9.680, 0, ''),
(18210, 15, -1384.890503, 8542.014648, 11.180, 0, ''),
(18210, 16, -1382.286133, 8539.869141, 11.139, 7500, 'SAY_MAG_COMPLETE'),
(18210, 17, -1361.224609, 8521.440430, 11.144, 0, ''),
(18210, 18, -1324.803589, 8510.688477, 13.050, 0, ''),
(18210, 19, -1312.075439, 8492.709961, 14.235, 0, '');

DELETE FROM script_waypoint WHERE entry=18887;
INSERT INTO script_waypoint VALUES
(18887, 0, 2650.06, 665.473, 61.9305, 0, ''),
(18887, 1, 2652.44, 670.761, 61.9370, 0, ''),
(18887, 2, 2655.96, 676.913, 57.1725, 0, ''),
(18887, 3, 2659.40, 677.317, 57.1725, 0, ''),
(18887, 4, 2651.75, 664.482, 57.1725, 0, ''),
(18887, 5, 2647.49, 666.595, 57.0824, 0, ''),
(18887, 6, 2644.37, 668.167, 55.4182, 0, ''),
(18887, 7, 2638.57, 671.231, 54.5200, 0, 'start dialogue - escort paused'),
(18887, 8, 2636.56, 679.894, 54.6595, 0, ''),
(18887, 9, 2640.79, 689.647, 55.3215, 0, ''),
(18887, 10, 2639.35, 706.777, 56.0667, 0, ''),
(18887, 11, 2617.70, 731.884, 55.5571, 0, '');

DELETE FROM script_waypoint WHERE entry=19589;
INSERT INTO script_waypoint VALUES
(19589, 1, 3358.22, 3728.25, 141.204, 16000, ''),
(19589, 2, 3368.05, 3715.51, 142.057, 0, ''),
(19589, 3, 3389.04, 3701.21, 144.648, 0, ''),
(19589, 4, 3419.51, 3691.41, 146.598, 0, ''),
(19589, 5, 3437.83, 3699.2,   147.235, 0, ''),
(19589, 6, 3444.85, 3700.89, 147.088, 0, ''),
(19589, 7, 3449.89, 3700.14, 148.118, 12000, 'first object'),
(19589, 8, 3443.55, 3682.09, 149.219, 0, ''),
(19589, 9, 3452.6,  3674.65,  150.226, 0, ''),
(19589, 10, 3462.6,  3659.01, 152.436, 0, ''),
(19589, 11, 3469.18, 3649.47, 153.178, 0, ''),
(19589, 12, 3475.11, 3639.41, 157.213, 0, ''),
(19589, 13, 3482.26, 3617.69, 159.126, 0, ''),
(19589, 14, 3492.7,  3606.27,  156.419, 0, ''),
(19589, 15, 3493.52, 3595.06,  156.581, 0, ''),
(19589, 16, 3490.4,  3588.45,  157.764, 0, ''),
(19589, 17, 3485.21, 3585.69, 159.979, 12000, 'second object'),
(19589, 18, 3504.68, 3594.44, 152.862, 0, ''),
(19589, 19, 3523.6,  3594.48, 145.393, 0, ''),
(19589, 20, 3537.01, 3576.71, 135.748, 0, ''),
(19589, 21, 3551.73, 3573.12, 128.013, 0, ''),
(19589, 22, 3552.12, 3614.08, 127.847, 0, ''),
(19589, 23, 3536.14, 3639.78, 126.031, 0, ''),
(19589, 24, 3522.94, 3646.47, 131.989, 0, ''),
(19589, 25, 3507.21, 3645.69, 138.1527, 0, ''),
(19589, 26, 3485.15, 3645.64, 137.755, 0, ''),
(19589, 27, 3472.18, 3633.88, 140.352, 0, ''),
(19589, 28, 3435.34, 3613.69, 140.725, 0, ''),
(19589, 29, 3417.4,  3612.4,   141.143, 12000, 'third object'),
(19589, 30, 3411.04, 3621.14, 142.454, 0, ''),
(19589, 31, 3404.47, 3636.89, 144.434, 0, ''),
(19589, 32, 3380.55, 3657.06, 144.332, 0, ''),
(19589, 33, 3375,     3676.86, 145.298, 0, ''),
(19589, 34, 3388.87, 3685.48, 146.818, 0, ''),
(19589, 35, 3393.99, 3699.4,   144.858, 0, ''),
(19589, 36, 3354.95, 3726.02, 141.428, 0, ''),
(19589, 37, 3351.40, 3722.33, 141.40, 0, 'home position');

DELETE FROM script_waypoint WHERE entry=19685;
INSERT INTO script_waypoint VALUES
(19685, 0, -1860.536987, 5416.987793, -10.480, 2500, ''),
(19685, 1, -1855.899048, 5412.805664, -12.427, 0, 'SAY_KHAD_SERV_0'),
(19685, 2, -1845.518433, 5385.352539, -12.427, 0, ''),
(19685, 3, -1815.247803, 5340.255371, -12.427, 0, ''),
(19685, 4, -1799.338379, 5312.777344, -12.427, 0, ''),
(19685, 5, -1780.491455, 5278.535156, -33.877, 2500, 'pause'),
(19685, 6, -1776.057983, 5270.247559, -38.809, 0, ''),
(19685, 7, -1772.219727, 5262.777344, -38.810, 0, ''),
(19685, 8, -1762.195557, 5261.720215, -38.850, 0, ''),
(19685, 9, -1759.242798, 5259.751465, -40.208, 0, ''),
(19685, 10, -1743.427612, 5259.661621, -40.208, 0, ''),
(19685, 11, -1744.361816, 5251.179199, -44.523, 0, ''),
(19685, 12, -1740.121582, 5240.120117, -47.740, 0, ''),
(19685, 13, -1737.636719, 5238.288086, -49.793, 0, ''),
(19685, 14, -1727.411621, 5233.874512, -50.477, 0, ''),
(19685, 15, -1707.489746, 5230.437988, -51.050, 0, ''),
(19685, 16, -1684.122925, 5223.633301, -49.415, 0, ''),
(19685, 17, -1669.973267, 5221.929688, -46.336, 0, ''),
(19685, 18, -1662.870117, 5221.712891, -44.959, 0, ''),
(19685, 19, -1657.170410, 5225.206055, -45.708, 0, ''),
(19685, 20, -1645.025635, 5238.360352, -40.212, 0, ''),
(19685, 21, -1631.657471, 5252.759766, -40.962, 0, ''),
(19685, 22, -1631.368286, 5276.543945, -41.032, 0, ''),
(19685, 23, -1621.732544, 5298.553711, -40.209, 0, ''),
(19685, 24, -1615.498169, 5298.098145, -40.209, 2500, 'pause'),
(19685, 25, -1636.979370, 5302.677734, -40.209, 0, ''),
(19685, 26, -1655.330322, 5315.736328, -40.207, 0, ''),
(19685, 27, -1656.884155, 5321.649414, -40.209, 0, ''),
(19685, 28, -1663.975586, 5335.206055, -46.526, 0, ''),
(19685, 29, -1659.141602, 5359.131836, -45.846, 0, ''),
(19685, 30, -1644.207520, 5390.886230, -45.542, 0, ''),
(19685, 31, -1646.183594, 5405.273926, -44.649, 0, ''),
(19685, 32, -1650.202637, 5414.541992, -46.324, 0, ''),
(19685, 33, -1656.052490, 5424.683594, -40.461, 0, ''),
(19685, 34, -1661.628296, 5423.929199, -40.405, 0, ''),
(19685, 35, -1664.651855, 5423.659180, -38.848, 0, ''),
(19685, 36, -1681.772339, 5425.999512, -38.809, 0, ''),
(19685, 37, -1729.785767, 5427.246094, -12.445, 0, ''),
(19685, 38, -1735.371460, 5423.663086, -12.427, 0, ''),
(19685, 39, -1741.627075, 5386.767578, -12.427, 0, ''),
(19685, 40, -1764.786133, 5363.735840, -12.427, 0, ''),
(19685, 41, -1816.372314, 5340.664063, -12.427, 0, ''),
(19685, 42, -1880.022705, 5309.796387, -12.427, 0, ''),
(19685, 43, -1887.374146, 5315.426270, -12.427, 0, ''),
(19685, 44, -1888.768066, 5324.518066, -5.146, 0, ''),
(19685, 45, -1888.399170, 5334.149902, 0.151, 0, ''),
(19685, 46, -1890.221680, 5337.659668, 0.921, 0, ''),
(19685, 47, -1897.542725, 5323.042969, 1.256, 0, ''),
(19685, 48, -1900.250244, 5319.804688, 0.831, 0, ''),
(19685, 49, -1910.039673, 5291.258789, 1.288, 0, ''),
(19685, 50, -1915.219482, 5275.572266, 2.502, 2500, 'pause'),
(19685, 51, -1927.226196, 5273.250977, 2.703, 0, ''),
(19685, 52, -1926.980225, 5278.467285, 0.109, 0, ''),
(19685, 53, -1927.665894, 5299.210449, -12.427, 0, ''),
(19685, 54, -1922.841797, 5319.263672, -12.427, 0, ''),
(19685, 55, -1925.779053, 5347.405273, -12.427, 0, ''),
(19685, 56, -1954.912476, 5384.230957, -12.427, 0, ''),
(19685, 57, -1966.727295, 5428.203613, -12.427, 0, ''),
(19685, 58, -1979.477661, 5448.415527, -12.427, 0, ''),
(19685, 59, -1977.533569, 5453.861328, -12.385, 0, ''),
(19685, 60, -1968.064087, 5455.781250, -4.343, 0, ''),
(19685, 61, -1959.223145, 5454.895020, 0.202, 0, ''),
(19685, 62, -1954.629028, 5457.011230, 0.900, 0, ''),
(19685, 63, -1967.760010, 5464.953125, 1.220, 2500, 'pause'),
(19685, 64, -1952.874023, 5462.962402, 0.956, 0, ''),
(19685, 65, -1955.339478, 5467.116699, 0.445, 0, ''),
(19685, 66, -1962.033203, 5472.804688, -4.243, 0, ''),
(19685, 67, -1968.007690, 5480.914551, -12.427, 0, ''),
(19685, 68, -1945.900146, 5515.948242, -12.427, 0, ''),
(19685, 69, -1874.867310, 5549.783691, -12.427, 0, ''),
(19685, 70, -1840.641602, 5544.234375, -12.427, 0, ''),
(19685, 71, -1838.963501, 5536.059570, -5.639, 0, ''),
(19685, 72, -1839.582275, 5525.627930, 0.193, 0, ''),
(19685, 73, -1837.931763, 5521.119629, 0.844, 0, ''),
(19685, 74, -1829.182495, 5533.433594, 1.209, 2500, 'pause'),
(19685, 75, -1848.397095, 5476.073730, 0.856, 40000, 'end');

DELETE FROM script_waypoint WHERE entry=20129;
INSERT INTO script_waypoint VALUES
(20129, 0, -8374.93,-4250.21, -204.38,5000, ''),
(20129, 1, -8374.93,-4250.21, -204.38,16000, ''),
(20129, 2, -8374.93,-4250.21, -204.38,10000, ''),
(20129, 3, -8374.93,-4250.21, -204.38,2000, ''),
(20129, 4, -8439.40,-4180.05, -209.25, 0, ''),
(20129, 5, -8437.82,-4120.84, -208.59,10000, ''),
(20129, 6, -8437.82,-4120.84, -208.59,16000, ''),
(20129, 7, -8437.82,-4120.84, -208.59,13000, ''),
(20129, 8, -8437.82,-4120.84, -208.59,18000, ''),
(20129, 9, -8437.82,-4120.84, -208.59,15000, ''),
(20129, 10, -8437.82,-4120.84, -208.59,2000, ''),
(20129, 11, -8467.26,-4198.63, -214.21, 0, ''),
(20129, 12, -8667.76,-4252.13, -209.56, 0, ''),
(20129, 13, -8703.71,-4234.58, -209.5,14000, ''),
(20129, 14, -8703.71,-4234.58, -209.5,2000, ''),
(20129, 15, -8642.81,-4304.37, -209.57, 0, ''),
(20129, 16, -8649.06,-4394.36, -208.46,6000, ''),
(20129, 17, -8649.06,-4394.36, -208.46,18000, ''),
(20129, 18, -8649.06,-4394.36, -208.46,2000, ''),
(20129, 19, -8468.72,-4437.67, -215.45, 0, ''),
(20129, 20, -8427.54,-4426, -211.13, 0, ''),
(20129, 21, -8364.83,-4393.32, -205.91, 0, ''),
(20129, 22, -8304.54,-4357.2, -208.2,18000, ''),
(20129, 23, -8304.54,-4357.2, -208.2,2000, ''),
(20129, 24, -8375.42,-4250.41, -205.14,5000, ''),
(20129, 25, -8375.42,-4250.41, -205.14,5000, '');

DELETE FROM script_waypoint WHERE entry=20415;
INSERT INTO script_waypoint VALUES
(20415, 0, 2488.77, 2184.89, 104.64, 0, ""),
(20415, 1, 2478.72, 2184.77, 98.58, 0, ""),
(20415, 2, 2473.52, 2184.71, 99.00, 0, ""),
(20415, 3, 2453.15, 2184.96, 97.09,4000, ""),
(20415, 4, 2424.18, 2184.15, 94.11, 0, ""),
(20415, 5, 2413.18, 2184.15, 93.42, 0, ""),
(20415, 6, 2402.02, 2183.90, 87.59, 0, ""),
(20415, 7, 2333.31, 2181.63, 90.03,4000, ""),
(20415, 8, 2308.73, 2184.34, 92.04, 0, ""),
(20415, 9, 2303.10, 2196.89, 94.94, 0, ""),
(20415, 10, 2304.58, 2272.23, 96.67, 0, ""),
(20415, 11, 2297.09, 2271.40, 95.16, 0, ""),
(20415, 12, 2297.68, 2266.79, 95.07,4000, ""),
(20415, 13, 2297.67, 2266.76, 95.07,4000, "");

DELETE FROM script_waypoint WHERE entry=21027;
INSERT INTO script_waypoint VALUES
(21027, 0, -2714.697266, 1326.879395, 34.306953, 0, ''),
(21027, 1, -2666.364990, 1348.222656, 34.445557, 0, ''),
(21027, 2, -2693.789307, 1336.964966, 34.445557, 0, ''),
(21027, 3, -2715.495361, 1328.054443, 34.106014, 0, ''),
(21027, 4, -2742.530762, 1314.138550, 33.606144, 0, ''),
(21027, 5, -2745.077148, 1311.108765, 33.630898, 0, ''),
(21027, 6, -2749.855225, 1302.737915, 33.475632, 0, ''),
(21027, 7, -2753.639648, 1294.059448, 33.314930, 0, ''),
(21027, 8, -2756.796387, 1285.122192, 33.391262, 0, 'spawn assassin'),
(21027, 9, -2750.042969, 1273.661987, 33.188259, 0, ''),
(21027, 10, -2740.378418, 1258.846680, 33.212521, 0, ''),
(21027, 11, -2733.629395, 1248.259766, 33.640598, 0, ''),
(21027, 12, -2727.212646, 1238.606445, 33.520847, 0, ''),
(21027, 13, -2726.377197, 1237.264526, 33.461823, 4000, 'SAY_WIL_PROGRESS1'),
(21027, 14, -2726.377197, 1237.264526, 33.461823, 4000, 'SAY_WIL_FIND_EXIT'),
(21027, 15, -2746.383301, 1266.390625, 33.191952, 0, 'spawn assassin'),
(21027, 16, -2746.383301, 1266.390625, 33.191952, 0, ''),
(21027, 17, -2758.927734, 1285.134155, 33.341728, 0, ''),
(21027, 18, -2761.845703, 1292.313599, 33.209042, 0, ''),
(21027, 19, -2758.871826, 1300.677612, 33.285332, 0, ''),
(21027, 20, -2753.928955, 1307.755859, 33.452457, 0, ''),
(21027, 21, -2738.612061, 1316.191284, 33.482975, 0, ''),
(21027, 22, -2727.897461, 1320.013916, 33.381111, 0, ''),
(21027, 23, -2709.458740, 1315.739990, 33.301838, 0, ''),
(21027, 24, -2704.658936, 1301.620361, 32.463303, 0, ''),
(21027, 25, -2704.120117, 1298.922607, 32.768162, 0, ''),
(21027, 26, -2691.798340, 1292.846436, 33.852642, 0, 'spawn assassin'),
(21027, 27, -2682.879639, 1288.853882, 32.995399, 0, ''),
(21027, 28, -2661.869141, 1279.682495, 26.686783, 0, ''),
(21027, 29, -2648.943604, 1270.272827, 24.147522, 0, ''),
(21027, 30, -2642.506836, 1262.938721, 23.512444, 0, 'spawn assassin'),
(21027, 31, -2636.984863, 1252.429077, 20.418257, 0, ''),
(21027, 32, -2648.113037, 1224.984863, 8.691818, 0, 'spawn assassin'),
(21027, 33, -2658.393311, 1200.136719, 5.492243, 0, ''),
(21027, 34, -2668.504395, 1190.450562, 3.127407, 0, ''),
(21027, 35, -2685.930420, 1174.360840, 5.163924, 0, ''),
(21027, 36, -2701.613770, 1160.026367, 5.611311, 0, ''),
(21027, 37, -2714.659668, 1149.980347, 4.342373, 0, ''),
(21027, 38, -2721.443359, 1145.002808, 1.913474, 0, ''),
(21027, 39, -2733.962158, 1143.436279, 2.620415, 0, 'spawn assassin'),
(21027, 40, -2757.876709, 1146.937500, 6.184002, 2000, 'SAY_WIL_JUST_AHEAD'),
(21027, 41, -2772.300537, 1166.052734, 6.331811, 0, ''),
(21027, 42, -2790.265381, 1189.941650, 5.207958, 0, ''),
(21027, 43, -2805.448975, 1208.663940, 5.557623, 0, 'spawn assassin'),
(21027, 44, -2820.617676, 1225.870239, 6.266103, 0, ''),
(21027, 45, -2831.926758, 1237.725830, 5.808506, 0, ''),
(21027, 46, -2842.578369, 1252.869629, 6.807481, 0, ''),
(21027, 47, -2846.344971, 1258.727295, 7.386168, 0, ''),
(21027, 48, -2847.556396, 1266.771729, 8.208790, 0, ''),
(21027, 49, -2841.654541, 1285.809204, 7.933223, 0, ''),
(21027, 50, -2841.754883, 1289.832520, 6.990304, 0, ''),
(21027, 51, -2861.973145, 1298.774536, 6.807335, 0, 'spawn assassin'),
(21027, 52, -2871.398438, 1302.348145, 6.807335, 7500, 'SAY_WIL_END');

DELETE FROM script_waypoint WHERE entry=22377;
INSERT INTO script_waypoint VALUES
(22377, 0, -2770.457520, 5418.410645, -34.538, 0, ''),
(22377, 1, -2778.180420, 5416.253906, -34.538, 0, ''),
(22377, 2, -2816.960449, 5414.944336, -34.529, 0, ''),
(22377, 3, -2827.533203, 5414.737305, -28.265, 0, ''),
(22377, 4, -2841.610596, 5413.021973, -28.261, 0, ''),
(22377, 5, -2863.605957, 5411.964355, -28.262, 1000, 'SAY_AKU_AMBUSH_A'),
(22377, 6, -2874.559570, 5413.799316, -28.260, 0, ''),
(22377, 7, -2878.775879, 5413.812012, -28.261, 0, ''),
(22377, 8, -2892.586914, 5413.478516, -18.784, 0, ''),
(22377, 9, -2896.040527, 5413.137207, -18.589, 0, ''),
(22377, 10, -2896.318848, 5409.431641, -18.450, 0, ''),
(22377, 11, -2895.997803, 5396.909668, -8.855, 0, ''),
(22377, 12, -2895.734131, 5386.623535, -9.260, 0, ''),
(22377, 13, -2895.318359, 5367.613281, -9.456, 0, ''),
(22377, 14, -2890.306641, 5353.883301, -11.280, 1000, 'SAY_AKU_AMBUSH_B'),
(22377, 15, -2880.419189, 5334.625977, -10.629, 0, ''),
(22377, 16, -2866.394043, 5314.253906, -9.678, 0, ''),
(22377, 17, -2864.753174, 5277.734375, -11.087, 0, ''),
(22377, 18, -2856.330322, 5255.902344, -11.496, 5000, 'SAY_AKU_COMPLETE');

DELETE FROM script_waypoint WHERE entry=22458;
INSERT INTO script_waypoint VALUES
(22458, 0, -3739.907959, 5393.691895, -4.213, 5000, 'SAY_LE_KEEP_SAFE'),
(22458, 1, -3733.334229, 5389.243164, -5.331, 0, ''),
(22458, 2, -3728.771729, 5385.649414, -3.704, 0, ''),
(22458, 3, -3717.267090, 5379.179199, -4.400, 0, ''),
(22458, 4, -3705.626465, 5379.261719, -7.711, 0, ''),
(22458, 5, -3688.279541, 5379.716309, -9.400, 0, ''),
(22458, 6, -3649.186523, 5389.111816, -11.917, 0, ''),
(22458, 7, -3612.791504, 5392.812500, -13.655, 0, ''),
(22458, 8, -3574.865479, 5412.704590, -16.543, 0, ''),
(22458, 9, -3564.438232, 5422.615723, -16.104, 0, ''),
(22458, 10, -3553.387695, 5444.732910, -12.184, 2500, 'arivve dig site SAY_LE_ARRIVE'),
(22458, 11, -3557.291016, 5465.319336, -9.282, 7500, 'dig 1'),
(22458, 12, -3548.102051, 5453.417969, -12.282, 10000, 'dig 2 SAY_LE_BURIED pause'),
(22458, 13, -3556.580322, 5446.475098, -11.920, 0, 'start returning'),
(22458, 14, -3564.438232, 5422.615723, -16.104, 0, ''),
(22458, 15, -3574.865479, 5412.704590, -16.543, 0, ''),
(22458, 16, -3612.791504, 5392.812500, -13.655, 0, ''),
(22458, 17, -3649.186523, 5389.111816, -11.917, 0, ''),
(22458, 18, -3688.279541, 5379.716309, -9.400, 0, ''),
(22458, 19, -3705.626465, 5379.261719, -7.711, 0, ''),
(22458, 20, -3717.267090, 5379.179199, -4.400, 0, ''),
(22458, 21, -3728.771729, 5385.649414, -3.704, 0, ''),
(22458, 22, -3733.334229, 5389.243164, -5.331, 0, ''),
(22458, 23, -3739.907959, 5393.691895, -4.213, 0, '');

DELETE FROM script_waypoint WHERE entry=22916;
INSERT INTO script_waypoint VALUES
(22916, 0, 7461.49, -3121.06, 438.210, 7000, 'SAY_START'),
(22916, 1, 7465.26, -3115.50, 439.315, 0, ''),
(22916, 2, 7470.03, -3109.29, 439.333, 0, ''),
(22916, 3, 7473.77, -3104.65, 442.366, 0, ''),
(22916, 4, 7478.67, -3098.55, 443.551, 0, ''),
(22916, 5, 7482.78, -3093.35, 441.883, 0, ''),
(22916, 6, 7486.23, -3089.19, 439.698, 0, ''),
(22916, 7, 7484.64, -3084.55, 439.566, 0, ''),
(22916, 8, 7477.09, -3084.43, 442.132, 0, ''),
(22916, 9, 7470.66, -3084.86, 443.194, 0, ''),
(22916, 10, 7456.51, -3085.83, 438.863, 0, ''),
(22916, 11, 7446.00, -3085.59, 438.210, 0, ''),
(22916, 12, 7444.60, -3084.10, 438.323, 0, ''),
(22916, 13, 7445.58, -3080.92, 439.374, 5000, 'collect 1'),
(22916, 14, 7446.18, -3085.36, 438.210, 5000, 'SAY_RELIC1'),
(22916, 15, 7453.90, -3086.69, 439.454, 0, ''),
(22916, 16, 7459.41, -3085.50, 439.158, 0, ''),
(22916, 17, 7465.90, -3085.01, 442.329, 0, ''),
(22916, 18, 7472.80, -3084.81, 443.085, 0, ''),
(22916, 19, 7480.58, -3084.56, 440.642, 0, ''),
(22916, 20, 7484.59, -3084.71, 439.568, 0, ''),
(22916, 21, 7491.81, -3090.55, 440.052, 0, ''),
(22916, 22, 7497.13, -3095.34, 437.505, 0, ''),
(22916, 23, 7496.61, -3113.62, 434.554, 0, ''),
(22916, 24, 7501.79, -3123.79, 435.347, 0, ''),
(22916, 25, 7506.60, -3130.78, 434.179, 0, ''),
(22916, 26, 7504.53, -3133.46, 435.579, 5000, 'collect 2'),
(22916, 27, 7505.20, -3130.03, 434.151, 15000, 'SAY_RELIC2'),
(22916, 28, 7502.04, -3124.44, 435.298, 0, ''),
(22916, 29, 7495.90, -3113.93, 434.538, 0, ''),
(22916, 30, 7488.79, -3111.10, 434.310, 0, ''),
(22916, 31, 7477.81, -3105.37, 430.541, 0, 'summon'),
(22916, 32, 7471.49, -3092.55, 429.006, 0, ''),
(22916, 33, 7472.35, -3062.72, 428.341, 0, ''),
(22916, 34, 7472.26, -3054.92, 427.150, 0, ''),
(22916, 35, 7475.03, -3053.39, 428.672, 5000, 'collect 3'),
(22916, 36, 7472.40, -3057.21, 426.870, 5000, 'SAY_RELIC3'),
(22916, 37, 7472.39, -3062.86, 428.301, 0, ''),
(22916, 38, 7470.24, -3087.69, 429.045, 0, ''),
(22916, 39, 7475.24, -3099.03, 429.917, 0, ''),
(22916, 40, 7484.24, -3109.85, 432.719, 0, ''),
(22916, 41, 7489.10, -3111.31, 434.400, 0, ''),
(22916, 42, 7497.02, -3108.54, 434.798, 0, ''),
(22916, 43, 7497.75, -3097.70, 437.031, 0, ''),
(22916, 44, 7492.53, -3090.12, 440.041, 0, ''),
(22916, 45, 7490.43, -3085.44, 439.807, 0, ''),
(22916, 46, 7501.02, -3069.70, 441.875, 0, ''),
(22916, 47, 7509.15, -3064.67, 445.012, 0, ''),
(22916, 48, 7515.78, -3060.16, 445.727, 0, ''),
(22916, 49, 7516.46, -3058.11, 445.682, 10000, 'quest credit');

DELETE FROM script_waypoint WHERE entry=23002;
INSERT INTO script_waypoint VALUES
(23002, 0, 3687.11, -3960.69, 31.8726, 0, ''),
(23002, 1, 3676.28, -3953.76, 29.9396, 0, ''),
(23002, 2, 3658.54, -3952.15, 30.0414, 0, ''),
(23002, 3, 3628.91, -3956.90, 29.4050, 0, ''),
(23002, 4, 3602.54, -3968.16, 31.5110, 0, ''),
(23002, 5, 3564.96, -3978.00, 30.3622, 0, ''),
(23002, 6, 3542.47, -3981.81, 29.1465, 0, ''),
(23002, 7, 3511.34, -3981.25, 30.2822, 0, ''),
(23002, 8, 3473.45, -3992.67, 30.2861, 0, ''),
(23002, 9, 3439.10, -4006.73, 29.2737, 0, ''),
(23002, 10, 3415.66, -4026.24, 25.2498, 0, ''),
(23002, 11, 3380.88, -4045.38, 26.3114, 0, ''),
(23002, 12, 3355.23, -4051.42, 25.5665, 0, ''),
(23002, 13, 3312.00, -4055.65, 28.3297, 0, ''),
(23002, 14, 3286.34, -4079.27, 28.2464, 0, ''),
(23002, 15, 3260.68, -4087.29, 31.4043, 0, ''),
(23002, 16, 3236.83, -4087.65, 32.6894, 0, ''),
(23002, 17, 3215.06, -4082.10, 32.4181, 0, ''),
(23002, 18, 3203.59, -4082.47, 32.7436, 0, ''),
(23002, 19, 3166.41, -4062.09, 33.2357, 0, ''),
(23002, 20, 3147.51, -4055.33, 33.5683, 0, ''),
(23002, 21, 3125.41, -4050.01, 34.6100, 0, ''),
(23002, 22, 3121.16, -4045.07, 36.5481, 0, ''),
(23002, 23, 3101.54, -4023.78, 33.7169, 0, ''),
(23002, 24, 3094.16, -4016.89, 33.8487, 0, ''),
(23002, 25, 3079.57, -4011.01, 35.7546, 0, ''),
(23002, 26, 3058.83, -4001.71, 34.3039, 0, ''),
(23002, 27, 3037.83, -3986.60, 33.4216, 0, ''),
(23002, 28, 3016.93, -3970.83, 33.3743, 0, ''),
(23002, 29, 2998.05, -3954.89, 33.2338, 0, ''),
(23002, 30, 2969.35, -3929.27, 33.4831, 0, ''),
(23002, 31, 2941.23, -3909.56, 31.3506, 0, ''),
(23002, 32, 2911.42, -3895.07, 32.0950, 0, ''),
(23002, 33, 2892.44, -3875.52, 30.8123, 0, ''),
(23002, 34, 2870.52, -3858.97, 32.1977, 0, ''),
(23002, 35, 2865.84, -3836.99, 32.1108, 0, ''),
(23002, 36, 2850.52, -3814.52, 32.8635, 0, ''),
(23002, 37, 2836.63, -3796.94, 33.1473, 0, ''),
(23002, 38, 2820.73, -3780.22, 28.6916, 0, ''),
(23002, 39, 2795.82, -3770.13, 30.1327, 0, ''),
(23002, 40, 2773.15, -3765.54, 30.2947, 0, ''),
(23002, 41, 2742.31, -3761.65, 30.1218, 0, ''),
(23002, 42, 2708.43, -3748.46, 21.2468, 0, ''),
(23002, 43, 2661.45, -3741.11, 21.9603, 0, ''),
(23002, 44, 2623.89, -3735.29, 25.8979, 0, ''),
(23002, 45, 2585.93, -3728.85, 28.5146, 0, ''),
(23002, 46, 2554.93, -3730.10, 26.6795, 0, ''),
(23002, 47, 2538.68, -3721.28, 28.1589, 0, ''),
(23002, 48, 2508.54, -3708.71, 29.6718, 0, ''),
(23002, 49, 2474.69, -3710.37, 31.0805, 0, ''),
(23002, 50, 2456.40, -3698.83, 31.6187, 0, ''),
(23002, 51, 2430.54, -3701.87, 31.0494, 0, ''),
(23002, 52, 2390.13, -3681.76, 29.5484, 0, ''),
(23002, 53, 2357.06, -3673.96, 29.8845, 0, ''),
(23002, 54, 2330.15, -3672.73, 31.1314, 0, ''),
(23002, 55, 2302.77, -3665.22, 29.4110, 0, ''),
(23002, 56, 2279.24, -3659.46, 29.6247, 0, ''),
(23002, 57, 2254.65, -3661.12, 29.6984, 0, ''),
(23002, 58, 2223.32, -3654.92, 31.0149, 0, ''),
(23002, 59, 2194.29, -3645.40, 32.0417, 0, ''),
(23002, 60, 2153.05, -3650.82, 31.2292, 0, ''),
(23002, 61, 2114.15, -3639.96, 31.7371, 0, ''),
(23002, 62, 2093.68, -3646.65, 31.3745, 0, ''),
(23002, 63, 2069.86, -3670.59, 30.6172, 0, ''),
(23002, 64, 2024.40, -3677.64, 29.7682, 0, ''),
(23002, 65, 1988.61, -3680.02, 31.8937, 0, ''),
(23002, 66, 1962.68, -3692.17, 32.7811, 0, ''),
(23002, 67, 1931.94, -3708.48, 31.3641, 0, ''),
(23002, 68, 1893.36, -3710.02, 33.0193, 0, ''),
(23002, 69, 1865.73, -3718.35, 32.1664, 0, ''),
(23002, 70, 1839.74, -3732.92, 32.5322, 0, ''),
(23002, 71, 1805.08, -3757.76, 32.6295, 0, ''),
(23002, 72, 1780.24, -3775.53, 30.5931, 0, ''),
(23002, 73, 1753.28, -3786.79, 30.7445, 0, ''),
(23002, 74, 1731.09, -3796.64, 36.8866, 0, '');

DELETE FROM script_waypoint WHERE entry=24358;
INSERT INTO script_waypoint VALUES
(24358, 0, 121.193970, 1645.619385, 42.021, 0, ''),
(24358, 1, 132.051468, 1642.176025, 42.021, 5000, 'SAY_AT_GONG'),
(24358, 2, 120.670631, 1636.346802, 42.415, 0, ''),
(24358, 3, 120.536003, 1611.654663, 43.473, 10000, 'SAY_OPEN_ENTRANCE'),
(24358, 4, 120.536003, 1611.654663, 43.473, 0, '');

DELETE FROM script_waypoint WHERE entry=28070;
INSERT INTO script_waypoint VALUES
(28070, 0, 1053.789795, 476.639343, 207.744, 0, ''),
(28070, 1, 1032.293945, 467.623444, 207.736, 0, ''),
(28070, 2, 1017.908752, 454.765656, 207.719, 0, ''),
(28070, 3, 1004.810120, 441.305115, 207.373, 0, ''),
(28070, 4, 988.694214, 424.422485, 207.425, 0, ''),
(28070, 5, 984.816345, 422.177917, 205.994, 0, ''),
(28070, 6, 977.204468, 420.026917, 205.994, 0, ''),
(28070, 7, 962.388123, 421.983307, 205.994, 0, ''),
(28070, 8, 950.419556, 416.515198, 205.994, 0, ''),
(28070, 9, 943.972290, 403.071228, 205.994, 0, ''),
(28070, 10, 947.921936, 387.683563, 205.994, 0, ''),
(28070, 11, 946.554749, 383.270782, 205.994, 0, ''),
(28070, 12, 944.654724, 380.630859, 207.286, 0, ''),
(28070, 13, 941.101563, 377.373413, 207.421, 0, 'reach tribunal, set pause'),
(28070, 14, 935.217896, 370.557343, 207.421, 0, ''),
(28070, 15, 928.035950, 363.026733, 204.018, 0, ''),
(28070, 16, 909.287292, 344.392792, 203.706, 0, ''),
(28070, 17, 897.946838, 333.634735, 203.706, 0, 'reach panel'),
(28070, 18, 918.914429, 351.312866, 203.706, 0, 'reach floor disc (end event begin)'),
(28070, 19, 928.070068, 363.296326, 204.091, 0, 'stealth'),
(28070, 20, 934.817627, 370.136261, 207.421, 0, ''),
(28070, 21, 941.501465, 377.254456, 207.421, 0, '');

DELETE FROM script_waypoint WHERE entry=28217;
INSERT INTO script_waypoint VALUES
(28217, 0, 5384.218262, 4533.261230, -129.518799, 0, ''),
(28217, 1, 5394.103027, 4531.190918, -131.758179, 0, ''),
(28217, 2, 5401.982910, 4527.303711, -137.599258, 0, ''),
(28217, 3, 5407.979492, 4526.484375, -143.597122, 0, ''),
(28217, 4, 5420.837402, 4519.582520, -144.921677, 0, ''),
(28217, 5, 5428.551758, 4522.227051, -148.790253, 0, ''),
(28217, 6, 5438.542480, 4536.080566, -149.651520, 0, ''),
(28217, 7, 5452.433105, 4553.935059, -149.093414, 0, ''),
(28217, 8, 5460.834961, 4564.371582, -148.660049, 0, ''),
(28217, 9, 5463.245605, 4584.000000, -148.961945, 0, ''),
(28217,10, 5463.708984, 4603.705566, -147.329636, 0, ''),
(28217,11, 5470.239258, 4609.115234, -145.223984, 0, ''),
(28217,12, 5479.432617, 4609.195313, -141.364014, 0, ''),
(28217,13, 5487.466309, 4615.625000, -138.139740, 0, ''),
(28217,14, 5497.967773, 4634.802734, -134.696869, 0, ''),
(28217,15, 5527.621582, 4648.053711, -136.170990, 0, ''),
(28217,16, 5547.706055, 4651.724121, -134.740707, 0, ''),
(28217,17, 5559.466309, 4652.008301, -134.154831, 0, ''),
(28217,18, 5579.070313, 4652.293945, -136.745895, 0, ''),
(28217,19, 5593.437500, 4643.722168, -136.405670, 0, ''),
(28217,20, 5608.825684, 4630.810547, -136.833588, 0, ''),
(28217,21, 5629.032227, 4607.479492, -137.093552, 0, ''),
(28217,22, 5634.952148, 4600.204102, -137.246063, 5000, 'thanks and quest credit'),
(28217,23, 5638.541504, 4594.924805, -137.495117, 0, 'summon'),
(28217,24, 5638.061523, 4579.945801, -138.029465, 0, '');

DELETE FROM script_waypoint WHERE entry=28787;
INSERT INTO script_waypoint (entry, pointid, location_x, location_y, location_z, waittime, point_comment) VALUES
(28787, 1, 5913.516113, 5379.034668, -98.896118, 0, ''),
(28787, 2, 5917.750977, 5374.519043, -98.869781, 0, 'SAY_HELICE_EXPLOSIVES_1'),
(28787, 3, 5926.428711, 5372.145020, -98.884453, 0, ''),
(28787, 4, 5929.214844, 5377.803223, -99.020065, 0, ''),
(28787, 5, 5927.621582, 5378.564941, -99.047890, 0, ''),
(28787, 6, 5917.622070, 5383.494629, -106.310204, 0, ''),
(28787, 7, 5908.991211, 5387.655762, -106.310204, 0, ''),
(28787, 8, 5906.287109, 5390.496582, -106.041801, 0, ''),
(28787, 9, 5902.415039, 5399.741211, -99.306595, 0, ''),
(28787, 10, 5901.444336, 5404.593262, -96.759636, 0, ''),
(28787, 11, 5897.860352, 5406.656250, -96.029709, 0, ''),
(28787, 12, 5892.254395, 5401.291504, -95.848808, 0, ''),
(28787, 13, 5887.421875, 5386.701172, -95.400146, 0, 'SAY_HELICE_EXPLOSIVES_2'),
(28787, 14, 5883.308105, 5385.057617, -94.423698, 0, ''),
(28787, 15, 5879.180664, 5375.897461, -95.088066, 0, ''),
(28787, 16, 5872.613281, 5363.473633, -97.703575, 0, ''),
(28787, 17, 5857.971191, 5354.929688, -98.586090, 0, ''),
(28787, 18, 5848.729004, 5345.326660, -99.428978, 0, ''),
(28787, 19, 5842.330566, 5335.018555, -100.421455, 0, ''),
(28787, 20, 5832.164551, 5323.145020, -98.703285, 0, ''),
(28787, 21, 5824.738770, 5315.712891, -97.758018, 0, ''),
(28787, 22, 5819.650879, 5305.409668, -97.481796, 10000, 'SAY_HELICE_COMPLETE');

DELETE FROM script_waypoint WHERE entry=30658;
INSERT INTO script_waypoint VALUES
(30658, 0, 1830.504517, 799.356506, 44.341801, 5000, 'use activation'),
(30658, 1, 1832.461792, 800.431396, 44.311745, 10000, 'SAY_BEGIN call back guards'),
(30658, 2, 1824.786987, 803.828369, 44.363434, 3000, 'SAY_LOCK_DOOR'),
(30658, 3, 1824.786987, 803.828369, 44.363434, 0, 'close door'),
(30658, 4, 1817.315674, 804.060608, 44.363998, 0, 'escort paused - allow teleport inside'),
(30658, 5, 1826.889648, 803.929993, 44.363239, 30000, 'SAY_VICTORY');

DELETE FROM script_waypoint WHERE entry = 349;
INSERT INTO script_waypoint VALUES
(349, 01,  -8769.591797, -2185.733643, 141.974564, 0, ''),
(349, 02,  -8776.540039, -2193.782959, 140.960159, 0, ''),
(349, 03,  -8783.289063, -2194.818604, 140.461731, 0, ''),
(349, 04,  -8792.520508, -2188.802002, 142.077728, 0, ''),
(349, 05,  -8807.547852, -2186.100830, 141.504135, 0, ''),
(349, 06,  -8818,        -2184.8,      139.153,    0, ''),
(349, 07,  -8825.805664, -2188.840576, 138.458832, 0, ''),
(349, 08,  -8827.522461, -2199.805664, 139.621933, 0, ''),
(349, 09,  -8821.140625, -2212.642334, 143.126419, 0, ''),
(349, 10,  -8809.175781, -2230.456299, 143.438431, 0, ''),
(349, 11,  -8797.040039, -2240.718262, 146.548203, 0, ''),
(349, 12,  -8795.242188, -2251.809814, 146.808044, 0, ''),
(349, 13,  -8780.159180, -2258.615967, 148.553772, 0, ''),
(349, 14,  -8762.650391, -2259.559326, 151.144241, 0, ''),
(349, 15,  -8754.357422, -2253.735352, 152.243073, 0, ''),
(349, 16,  -8741.869141, -2250.997070, 154.485718, 0, ''),
(349, 17,  -8733.218750, -2251.010742, 154.360031, 0, ''),
(349, 18,  -8717.474609, -2245.044678, 154.68614,  0, ''),
(349, 19,  -8712.240234, -2246.826172, 154.709473, 0, ''),
(349, 20,  -8693.840820, -2240.410889, 152.909714, 0, ''),
(349, 21,  -8681.818359, -2245.332764, 155.517838, 0, ''),
(349, 22,  -8669.86,     -2252.77,     154.854,    0, ''),
(349, 23,  -8670.56,     -2264.69,     156.978,    0, ''),
(349, 24,  -8676.557617, -2269.204346, 155.411316, 0, ''),
(349, 25,  -8673.340820, -2288.650146, 157.054123, 0, ''),
(349, 26,  -8677.760742, -2302.563965, 155.916580, 16000, 'Corp. Keeshan - Short Break Outside'),
(349, 27,  -8682.462891, -2321.688232, 155.916946, 0, ''),
(349, 28,  -8690.402344, -2331.779297, 155.970505, 0, ''),
(349, 29,  -8715.1,      -2353.95,     156.188,    0, ''),
(349, 30,  -8748.042969, -2370.701904, 157.988342, 0, ''),
(349, 31,  -8780.900391, -2421.370361, 156.108871, 0, ''),
(349, 32,  -8792.009766, -2453.379883, 142.746002, 0, ''),
(349, 33,  -8804.780273, -2472.429932, 134.192001, 0, ''),
(349, 34,  -8841.348633, -2503.626221, 132.276138, 0, ''),
(349, 35,  -8867.565430, -2529.892822, 134.738586, 0, ''),
(349, 36,  -8870.67,     -2542.08,     131.044,    0, ''),
(349, 37,  -8922.05,     -2585.31,     132.446,    0, ''),
(349, 38,  -8949.08,     -2596.87,     132.537,    0, ''),
(349, 39,  -8993.460938, -2604.042725, 130.756210, 0, ''),
(349, 40,  -9006.709961, -2598.469971, 127.966003, 0, ''),
(349, 41,  -9038.96,     -2572.71,     124.748,    0, ''),
(349, 42,  -9046.92,     -2560.64,     124.447,    0, ''),
(349, 43,  -9066.693359, -2546.633301, 123.110138, 0, ''),
(349, 44,  -9077.54,     -2541.67,     121.17,     0, ''),
(349, 45,  -9125.320313, -2490.059326, 116.057274, 0, ''),
(349, 46,  -9145.063477, -2442.239990, 108.231689, 0, ''),
(349, 47,  -9158.197266, -2425.363281, 105.500038, 0, ''),
(349, 48,  -9151.922852, -2393.671631, 100.856010, 0, ''),
(349, 49,  -9165.193359, -2376.031738, 94.821518,  0, ''),
(349, 50,  -9187.099609, -2360.520020, 89.923103,  0, ''),
(349, 51,  -9235.443359, -2305.239014, 77.925316,  0, ''),
(349, 52,  -9264.73,     -2292.92,     70.0089,    0, ''),
(349, 53,  -9277.468750, -2296.188721, 68.089630,  2500, 'Corp. Keeshan - quest-finish'),
(349, 54,  -9277.468750, -2296.188721, 68.089630,  0, 'Corp. Keeshan - Say Goodbye');

DELETE FROM script_waypoint WHERE entry=1379;
INSERT INTO script_waypoint VALUES
(1379,01,-5751.12,-3441.01,301.743,0,''),
(1379,02,-5738.58,-3485.14,302.410,0,''),
(1379,03,-5721.62,-3507.85,304.011,0,''),
(1379,04,-5710.21,-3527.97,304.708,0,''),
(1379,05,-5706.92,-3542.89,304.871,0,''),
(1379,06,-5701.53,-3551.24,305.962,0,''),
(1379,07,-5699.53,-3555.69,306.505,0,''),
(1379,08,-5690.56,-3571.98,309.035,0,''),
(1379,09,-5678.61,-3587.17,310.607,0,''),
(1379,10,-5677.05,-3594.35,311.527,0,''),
(1379,11,-5674.39,-3605.19,312.239,0,''),
(1379,12,-5674.45,-3614.39,312.337,0,''),
(1379,13,-5673.05,-3630.56,311.105,0,''),
(1379,14,-5680.34,-3645.44,315.185,0,''),
(1379,15,-5684.46,-3650.05,314.687,0,''),
(1379,16,-5693.9,-3674.14,313.03,0,''),
(1379,17,-5701.43,-3712.54,313.959,0,''),
(1379,18,-5698.79,-3720.88,316.943,0,''),
(1379,19,-5699.95,-3733.63,318.597,0,'Protecting the Shipment - Ambush'),
(1379,20,-5698.61,-3754.74,322.047,0,''),
(1379,21,-5688.68,-3769,323.957,0,''),
(1379,22,-5688.14,-3782.65,322.667,0,''),
(1379,23,-5699.23,-3792.65,322.448,30000,'Protecting the Shipment - End'),
(1379,24,-5700.80,-3792.78,322.588,0,'');

DELETE FROM script_waypoint WHERE entry = 25208;
INSERT INTO script_waypoint VALUES
(25208,0,4013.51,6390.33,29.970,15000,'Lurgglbr - after escaped from cage'),
(25208,1,4023.06,6379.43,29.210,0,''),
(25208,2,4033.61,6370.94,28.430,0,''),
(25208,3,4052.03,6367.42,27.370,0,''),
(25208,4,4061.13,6353.36,25.450,0,''),
(25208,5,4064.28,6330.76,25.310,0,''),
(25208,6,4057.94,6307.85,24.900,0,''),
(25208,7,4057.40,6290.12,24.430,0,''),
(25208,8,4065.63,6277.64,23.900,0,''),
(25208,9,4080.71,6280.77,26.920,0,''),
(25208,10,4098.90,6279.00,25.950,0,''),
(25208,11,4118.00,6277.81,25.720,0,''),
(25208,12,4129.47,6281.65,28.860,0,''),
(25208,13,4143.86,6282.57,29.180,4000,'Lurgglbr - outside cave'),
(25208,14,4159.54,6280.08,30.520,0,''),
(25208,15,4171.95,6291.50,22.250,0,''),
(25208,16,4184.86,6293.45,16.570,0,''),
(25208,17,4194.14,6301.28,13.310,0,''),
(25208,18,4210.34,6285.81,09.500,0,''),
(25208,19,4220.05,6272.75,07.770,0,''),
(25208,20,4242.45,6272.24,01.750,0,''),
(25208,21,4257.79,6252.91,-0.050,0,''),
(25208,22,4256.81,6230.74,-0.090,0,''),
(25208,23,4241.09,6217.87,-0.140,0,''),
(25208,24,4254.66,6205.16,-0.170,0,''),
(25208,25,4270.07,6188.42,0.059,15000,'Lurgglbr - final point');

DELETE FROM script_waypoint WHERE entry=7998;
INSERT INTO script_waypoint VALUES
(7998, 01, -510.1305, -132.6899, -152.5,    0, ''),
(7998, 02, -511.0994, -129.74,   -153.8453, 0, ''),
(7998, 03, -511.7897, -127.4764, -155.5505, 0, ''),
(7998, 04, -512.9688, -124.926,  -156.1146, 5000, ''),
(7998, 05, -513.9719, -120.2358, -156.1161, 0, ''),
(7998, 06, -514.3875, -115.1896, -156.1165, 0, ''),
(7998, 07, -514.3039, -111.4777, -155.5205, 0, ''),
(7998, 08, -514.8401, -107.6633, -154.8925, 0, ''),
(7998, 09, -518.9943, -101.4161, -154.6482, 27000, ''),
(7998, 10, -526.9984, -98.14884, -155.6253, 0, ''),
(7998, 11, -534.5686, -105.4101, -155.989,  30000, ''),
(7998, 12, -535.5336, -104.6947, -155.9713, 0, ''),
(7998, 13, -541.6304, -98.6583,  -155.8584, 25000, ''),
(7998, 14, -535.0923, -99.91748, -155.9742, 0, ''),
(7998, 15, -519.0099, -101.5097, -154.6766, 3000, ''),
(7998, 16, -504.4659, -97.84802, -150.9554, 30000, ''),
(7998, 17, -506.9069, -89.14736, -151.083,  23000, ''),
(7998, 18, -512.7576, -101.9025, -153.198, 0, ''),
(7998, 19, -519.9883, -124.8479, -156.128, 86400000, 'this npc should not reset on wp end');

DELETE FROM script_waypoint WHERE entry = 18760;
INSERT INTO script_waypoint (entry, pointid, location_x, location_y, location_z, waittime, point_comment) VALUES
(18760, 01, -2267.07, 3091.46, 13.8271, 0, ''),
(18760, 02, -2270.92, 3094.19, 13.8271, 0, ''),
(18760, 03, -2279.08, 3100.04, 13.8271, 0, ''),
(18760, 04, -2290.05, 3105.07, 13.8271, 0, ''),
(18760, 05, -2297.64, 3112.32, 13.8271, 0, ''),
(18760, 06, -2303.89, 3118.22, 13.8231, 10000, 'building exit'),
(18760, 07, -2307.77, 3123.47, 13.7257, 0, ''),
(18760, 08, -2310.67, 3126.2,  12.5841, 0, ''),
(18760, 09, -2311.48, 3126.98, 12.2769, 0, ''),
(18760, 10, -2316.91, 3132.13, 11.9261, 0, ''),
(18760, 11, -2320.43, 3135.54, 11.7436, 0, ''),
(18760, 12, -2327.38, 3139.36, 10.9431, 0, ''),
(18760, 13, -2332.02, 3142.05, 9.81277, 0, ''),
(18760, 14, -2338.21, 3145.32, 9.31001, 0, ''),
(18760, 15, -2343.1,  3148.91, 8.84879, 0, ''),
(18760, 16, -2347.76, 3153.15, 7.71049, 0, ''),
(18760, 17, -2351.04, 3156.12, 6.66476, 0, ''),
(18760, 18, -2355.15, 3163.18, 5.11997, 0, ''),
(18760, 19, -2359.01, 3169.83, 3.64343, 0, ''),
(18760, 20, -2364.85, 3176.81, 2.32802, 0, ''),
(18760, 21, -2368.77, 3181.69, 1.53285, 0, ''),
(18760, 22, -2371.76, 3185.11, 0.979932, 0, ''),
(18760, 23, -2371.85, 3191.89, -0.293048, 0, ''),
(18760, 24, -2370.99, 3199.6, -1.10504, 0, 'turn left 1'),
(18760, 25, -2376.24, 3205.54, -1.04152, 0, ''),
(18760, 26, -2380.99, 3211.61, -1.16891, 0, ''),
(18760, 27, -2384.04, 3218.4, -1.15279, 0, ''),
(18760, 28, -2385.41, 3226.22, -1.23403, 0, ''),
(18760, 29, -2386.02, 3233.89, -1.31723, 0, ''),
(18760, 30, -2384.7, 3239.82, -1.51903, 0, ''),
(18760, 31, -2382.98, 3247.94, -1.39163, 0, ''),
(18760, 32, -2379.68, 3254.22, -1.25927, 0, ''),
(18760, 33, -2375.27, 3259.69, -1.22925, 0, ''),
(18760, 34, -2369.62, 3264.55, -1.1879, 0, ''),
(18760, 35, -2364.01, 3268.32, -1.48348, 0, ''),
(18760, 36, -2356.61, 3272.31, -1.5505, 0, ''),
(18760, 37, -2352.3, 3274.63, -1.35693, 0, ''),
(18760, 38, -2348.54, 3278.04, -1.04161, 0, 'turn left 2'),
(18760, 39, -2347.56, 3282.41, -0.75979, 0, ''),
(18760, 40, -2348.29, 3288.91, -0.63215, 0, ''),
(18760, 41, -2349.68, 3298.84, -1.07864, 0, ''),
(18760, 42, -2351.15, 3308.52, -1.38864, 0, ''),
(18760, 43, -2352.2, 3317.14, -1.59873, 0, ''),
(18760, 44, -2351.59, 3325.51, -1.92624, 0, ''),
(18760, 45, -2350.88, 3333.38, -2.32506, 0, ''),
(18760, 46, -2350.05, 3342.68, -2.51886, 0, ''),
(18760, 47, -2350.12, 3347.32, -2.57528, 0, ''),
(18760, 48, -2348.72, 3353.7, -2.72689, 0, ''),
(18760, 49, -2346.53, 3360.85, -2.9756, 0, ''),
(18760, 50, -2344.83, 3365.46, -3.3311, 0, ''),
(18760, 51, -2342.68, 3368.91, -3.74526, 0, ''),
(18760, 52, -2340.25, 3371.44, -4.10499, 0, ''),
(18760, 53, -2337.4, 3373.47, -4.44362, 0, ''),
(18760, 54, -2332.68, 3376.02, -5.19648, 0, ''),
(18760, 55, -2326.69, 3379.64, -6.24757, 0, ''),
(18760, 56, -2321.2, 3383.98, -7.28247, 0, ''),
(18760, 57, -2317.81, 3387.78, -8.40093, 0, ''),
(18760, 58, -2315.3, 3392.47, -9.63431, 0, ''),
(18760, 59, -2314.69, 3396.6, -10.2031, 0, ''),
(18760, 60, -2315.48, 3402.35, -10.8211, 0, 'gate'),
(18760, 61, -2317.55, 3409.27, -11.3309, 5000, 'Firewing point exit'),
(18760, 62, -2320.69, 3412.99, -11.5207, 0, ''),
(18760, 63, -2326.88, 3417.89, -11.6105, 0, ''),
(18760, 64, -2332.73, 3421.74, -11.5659, 0, ''),
(18760, 65, -2337.23, 3424.89, -11.496, 0, ''),
(18760, 66, -2339.57, 3429.17, -11.3782, 0, ''),
(18760, 67, -2341.66, 3435.86, -11.3746, 5000, 'Wave and transform'),
(18760, 68, -2342.15, 3443.94, -11.2562, 2000, 'final destination');

DELETE FROM script_waypoint WHERE entry=3678;
INSERT INTO script_waypoint VALUES
(3678, 0, -134.925, 125.468, -78.16, 0, ''),
(3678, 1, -125.684, 132.937, -78.42, 0, ''),
(3678, 2, -113.812, 139.295, -80.98, 0, ''),
(3678, 3, -109.854, 157.538, -80.20, 0, ''),
(3678, 4, -108.640, 175.207, -79.74, 0, ''),
(3678, 5, -108.668, 195.457, -80.64, 0, ''),
(3678, 6, -111.007, 219.007, -86.58, 0, ''),
(3678, 7, -102.408, 232.821, -91.52, 0, 'first corner SAY_FIRST_CORNER'),
(3678, 8, -92.434, 227.742, -90.75, 0, ''),
(3678, 9, -82.456, 224.853, -93.57, 0, ''),
(3678, 10, -67.789, 208.073, -93.34, 0, ''),
(3678, 11, -43.343, 205.295, -96.37, 0, ''),
(3678, 12, -34.676, 221.394, -95.82, 0, ''),
(3678, 13, -32.582, 238.573, -93.51, 0, ''),
(3678, 14, -42.149, 258.672, -92.88, 0, ''),
(3678, 15, -55.257, 274.696, -92.83, 0, 'circle of flames SAY_CIRCLE_BANISH'),
(3678, 16, -48.604, 287.584, -92.46, 0, ''),
(3678, 17, -47.236, 296.093, -90.88, 0, ''),
(3678, 18, -35.618, 309.067, -89.73, 0, ''),
(3678, 19, -23.573, 311.376, -88.60, 0, ''),
(3678, 20, -8.692, 302.389, -87.43, 0, ''),
(3678, 21, -1.237, 293.268, -85.55, 0, ''),
(3678, 22, 10.398, 279.294, -85.86, 0, ''),
(3678, 23, 23.108, 264.693, -86.69, 0, ''),
(3678, 24, 31.996, 251.436, -87.62, 0, ''),
(3678, 25, 43.374, 233.073, -87.61, 0, ''),
(3678, 26, 54.438, 212.048, -89.50, 3000, 'chamber entrance SAY_NARALEX_CHAMBER'),
(3678, 27, 78.794, 208.895, -92.84, 0, ''),
(3678, 28, 88.392, 225.231, -94.46, 0, ''),
(3678, 29, 98.758, 233.938, -95.84, 0, ''),
(3678, 30, 107.248, 233.054, -95.98, 0, ''),
(3678, 31, 112.825, 233.907, -96.39, 0, ''),
(3678, 32, 114.634, 236.969, -96.04, 1000, 'naralex SAY_BEGIN_RITUAL'),
(3678, 33, 127.385, 252.279, -90.07, 0, ''),
(3678, 34, 121.595, 264.488, -91.55, 0, ''),
(3678, 35, 115.472, 264.253, -91.50, 0, ''),
(3678, 36, 99.988, 252.790, -91.51, 0, ''),
(3678, 37, 96.347, 245.038, -90.34, 0, ''),
(3678, 38, 82.201, 216.273, -86.10, 0, ''),
(3678, 39, 75.112, 206.494, -84.80, 0, ''),
(3678, 40, 27.174, 201.064, -72.31, 0, ''),
(3678, 41, -41.114, 204.149, -78.94, 0, '');

DELETE FROM script_waypoint WHERE entry=5644;
INSERT INTO script_waypoint (entry, pointid, location_x, location_y, location_z, waittime, point_comment) VALUES
(5644, 1, -339.679, 1752.04, 139.482, 0, ''),
(5644, 2, -328.957, 1734.95, 139.327, 0, ''),
(5644, 3, -338.29, 1731.36, 139.327, 0, ''),
(5644, 4, -350.747, 1731.12, 139.338, 0, ''),
(5644, 5, -365.064, 1739.04, 139.376, 0, ''),
(5644, 6, -371.105, 1746.03, 139.374, 0, ''),
(5644, 7, -383.141, 1738.62, 138.93, 0, ''),
(5644, 8, -390.445, 1733.98, 136.353, 0, ''),
(5644, 9, -401.368, 1726.77, 131.071, 0, ''),
(5644, 10, -416.016, 1721.19, 129.807, 0, ''),
(5644, 11, -437.139, 1709.82, 126.342, 0, ''),
(5644, 12, -455.83, 1695.61, 119.305, 0, ''),
(5644, 13, -459.862, 1687.92, 116.059, 0, ''),
(5644, 14, -463.565, 1679.1, 111.653, 0, ''),
(5644, 15, -461.485, 1670.94, 109.033, 0, ''),
(5644, 16, -471.786, 1647.34, 102.862, 0, ''),
(5644, 17, -477.146, 1625.69, 98.342, 0, ''),
(5644, 18, -475.815, 1615.815, 97.07, 0, ''),
(5644, 19, -474.329, 1590.01, 94.4982, 0, '');

DELETE FROM script_waypoint WHERE entry=4508;
INSERT INTO script_waypoint VALUES
(4508, 0, 2194.38, 1791.65, 65.48, 5000, ''),
(4508, 1, 2188.56, 1805.87, 64.45, 0, ''),
(4508, 2, 2186.2, 1836.278, 59.859, 5000, 'SAY_WILLIX_1'),
(4508, 3, 2163.27, 1851.67, 56.73, 0, ''),
(4508, 4, 2140.22, 1845.02, 48.32, 0, ''),
(4508, 5, 2131.5, 1804.29, 46.85, 0, ''),
(4508, 6, 2096.18, 1789.03, 51.13, 3000, 'SAY_WILLIX_2'),
(4508, 7, 2074.46, 1780.09, 55.64, 0, ''),
(4508, 8, 2055.12, 1768.67, 58.46, 0, ''),
(4508, 9, 2037.83, 1748.62, 60.27, 5000, 'SAY_WILLIX_3'),
(4508, 10, 2037.51, 1728.94, 60.85, 0, ''),
(4508, 11, 2044.7, 1711.71, 59.71, 0, ''),
(4508, 12, 2067.66, 1701.84, 57.77, 0, ''),
(4508, 13, 2078.91, 1704.54, 56.77, 0, ''),
(4508, 14, 2097.65, 1715.24, 54.74, 3000, 'SAY_WILLIX_4'),
(4508, 15, 2106.44, 1720.98, 54.41, 0, ''),
(4508, 16, 2123.96, 1732.56, 52.27, 0, ''),
(4508, 17, 2153.82, 1728.73, 51.92, 0, ''),
(4508, 18, 2163.49, 1706.33, 54.42, 0, ''),
(4508, 19, 2158.75, 1695.98, 55.70, 0, ''),
(4508, 20, 2142.6, 1680.72, 58.24, 0, ''),
(4508, 21, 2118.31, 1671.54, 59.21, 0, ''),
(4508, 22, 2086.02, 1672.04, 61.24, 0, ''),
(4508, 23, 2068.81, 1658.93, 61.24, 0, ''),
(4508, 24, 2062.82, 1633.31, 64.35, 0, ''),
(4508, 25, 2060.92, 1600.11, 62.41, 3000, 'SAY_WILLIX_5'),
(4508, 26, 2063.05, 1589.16, 63.26, 0, ''),
(4508, 27, 2063.67, 1577.22, 65.89, 0, ''),
(4508, 28, 2057.94, 1560.68, 68.40, 0, ''),
(4508, 29, 2052.56, 1548.05, 73.35, 0, ''),
(4508, 30, 2045.22, 1543.4, 76.65, 0, ''),
(4508, 31, 2034.35, 1543.01, 79.70, 0, ''),
(4508, 32, 2029.95, 1542.94, 80.79, 0, ''),
(4508, 33, 2021.34, 1538.67, 80.8, 0, 'SAY_WILLIX_6'),
(4508, 34, 2012.45, 1549.48, 79.93, 0, ''),
(4508, 35, 2008.05, 1554.92, 80.44, 0, ''),
(4508, 36, 2006.54, 1562.72, 81.11, 0, ''),
(4508, 37, 2003.8, 1576.43, 81.57, 0, ''),
(4508, 38, 2000.57, 1590.06, 80.62, 0, ''),
(4508, 39, 1998.96, 1596.87, 80.22, 0, ''),
(4508, 40, 1991.19, 1600.82, 79.39, 0, ''),
(4508, 41, 1980.71, 1601.44, 79.77, 0, ''),
(4508, 42, 1967.22, 1600.18, 80.62, 0, ''),
(4508, 43, 1956.43, 1596.97, 81.75, 0, ''),
(4508, 44, 1954.87, 1592.02, 82.18, 3000, 'SAY_WILLIX_7'),
(4508, 45, 1948.35, 1571.35, 80.96, 30000, 'SAY_WILLIX_END'),
(4508, 46, 1947.02, 1566.42, 81.80, 30000, '');

DELETE FROM script_waypoint WHERE entry = 8516;
INSERT INTO script_waypoint VALUES
(8516, 1,2603.18, 725.259, 54.6927, 0, ''),
(8516, 2,2587.13, 734.392, 55.231, 0, ''),
(8516, 3,2570.69, 753.572, 54.5855, 0, ''),
(8516, 4,2558.51, 747.66, 54.4482, 0, ''),
(8516, 5,2544.23, 772.924, 47.9255, 0, ''),
(8516, 6,2530.08, 797.475, 45.97, 0, ''),
(8516, 7,2521.83, 799.127, 44.3061, 0, ''),
(8516, 8,2502.61, 789.222, 39.5074, 0, ''),
(8516, 9,2495.25, 789.406, 39.499, 0, ''),
(8516, 10,2488.07, 802.455, 42.9834, 0, ''),
(8516, 11,2486.64, 826.649, 43.6363, 0, ''),
(8516, 12,2492.64, 835.166, 45.1427, 0, ''),
(8516, 13,2505.02, 847.564, 47.6487, 0, ''),
(8516, 14,2538.96, 877.362, 47.6781, 0, ''),
(8516, 15,2546.07, 885.672, 47.6789, 0, ''),
(8516, 16,2548.02, 897.584, 47.7277, 0, ''),
(8516, 17,2544.29, 909.116, 46.2506, 0, ''),
(8516, 18,2523.60, 920.306, 45.8717, 0, ''),
(8516, 19,2522.69, 933.546, 47.5769, 0, ''),
(8516, 20,2531.63, 959.893, 49.4111, 0, ''),
(8516, 21,2540.23, 973.338, 50.1241, 0, ''),
(8516, 22,2547.21, 977.489, 49.9759, 0, ''),
(8516, 23,2558.75, 969.243, 50.7353, 0, ''),
(8516, 24,2575.60, 950.138, 52.8460, 0, ''),
(8516, 25,2575.60, 950.138, 52.8460, 0, '');

DELETE FROM `script_waypoint` WHERE entry=29173;
INSERT INTO `script_waypoint` VALUES
(29173, 0, 2411.322, -5152.227, 83.777, 0,''),
(29173, 1, 2386.443, -5177.385, 74.049, 0,''),
(29173, 2, 2357.140, -5209.571, 79.642, 0,'SAY_LIGHT_OF_DAWN_STAND_1'),
(29173, 3, 2342.683, -5232.791, 85.259, 0,'SAY_LIGHT_OF_DAWN_STAND_2'),
(29173, 4, 2281.354, -5278.533, 82.227, 0,'Start battle'),
(29173, 5, 2280.302, -5284.489, 82.657, 600000,'Go in front of the chapel for outro');

DELETE FROM script_waypoint WHERE entry=11832;
INSERT INTO script_waypoint VALUES
(11832, 0, 7848.385645, -2216.356670, 470.888333, 15000, 'SAY_REMULOS_INTRO_1'),
(11832, 1, 7848.385645, -2216.356670, 470.888333, 5000, 'SAY_REMULOS_INTRO_2'),
(11832, 2, 7829.785645, -2244.836670, 463.853333, 0, ''),
(11832, 3, 7819.010742, -2304.344238, 455.956726, 0, ''),
(11832, 4, 7931.099121, -2314.350830, 473.054047, 0, ''),
(11832, 5, 7943.553223, -2324.688721, 477.676819, 0, ''),
(11832, 6, 7952.017578, -2351.135010, 485.234924, 0, ''),
(11832, 7, 7963.672852, -2412.990967, 488.953369, 0, ''),
(11832, 8, 7975.178223, -2551.602051, 490.079926, 0, ''),
(11832, 9, 7948.046875, -2570.828613, 489.750732, 0, ''),
(11832, 10, 7947.161133, -2583.396729, 490.066284, 0, ''),
(11832, 11, 7951.086426, -2596.215088, 489.831268, 0, ''),
(11832, 12, 7948.267090, -2610.062988, 492.340424, 0, ''),
(11832, 13, 7928.521973, -2625.954346, 492.447540, 0, 'escort paused - SAY_REMULOS_INTRO_3'),
(11832, 14, 7948.267090, -2610.062988, 492.340424, 0, ''),
(11832, 15, 7952.318848, -2594.118408, 490.070374, 0, ''),
(11832, 16, 7913.988770, -2567.002686, 488.330566, 0, ''),
(11832, 17, 7835.454102, -2571.099121, 489.289246, 0, 'escort paused - SAY_REMULOS_DEFEND_2'),
(11832, 18, 7897.283691, -2560.652344, 487.461304, 0, 'escort paused');

DELETE FROM script_waypoint WHERE entry=10300;
INSERT INTO script_waypoint VALUES
(10300, 1, 5728.81, -4801.15, 778.18, 0, ''),
(10300, 2, 5730.22, -4818.34, 777.11, 0, ''),
(10300, 3, 5728.12, -4835.76, 778.15, 1000, 'SAY_ENTER_OWL_THICKET'),
(10300, 4, 5718.85, -4865.62, 787.56, 0, ''),
(10300, 5, 5697.13, -4909.12, 801.53, 0, ''),
(10300, 6, 5684.20, -4913.75, 801.60, 0, ''),
(10300, 7, 5674.67, -4915.78, 802.13, 0, ''),
(10300, 8, 5665.61, -4919.22, 804.85, 0, ''),
(10300, 9, 5638.22, -4897.58, 804.97, 0, ''),
(10300, 10, 5632.67, -4892.05, 805.44, 0, 'Cavern 1 - EMOTE_CHANT_SPELL'),
(10300, 11, 5664.58, -4921.84, 804.91, 0, ''),
(10300, 12, 5684.21, -4943.81, 802.80, 0, ''),
(10300, 13, 5724.92, -4983.69, 808.25, 0, ''),
(10300, 14, 5753.39, -4990.73, 809.84, 0, ''),
(10300, 15, 5765.62, -4994.89, 809.42, 0, 'Cavern 2 - EMOTE_CHANT_SPELL'),
(10300, 16, 5724.94, -4983.58, 808.29, 0, ''),
(10300, 17, 5699.61, -4989.82, 808.03, 0, ''),
(10300, 18, 5686.80, -5012.17, 807.27, 0, ''),
(10300, 19, 5691.43, -5037.43, 807.73, 0, ''),
(10300, 20, 5694.24, -5054.64, 808.85, 0, 'Cavern 3 - EMOTE_CHANT_SPELL'),
(10300, 21, 5686.88, -5012.18, 807.23, 0, ''),
(10300, 22, 5664.94, -5001.12, 807.78, 0, ''),
(10300, 23, 5647.12, -5002.84, 807.54, 0, ''),
(10300, 24, 5629.23, -5014.88, 807.94, 0, ''),
(10300, 25, 5611.26, -5025.62, 808.36, 0, 'Cavern 4 - EMOTE_CHANT_SPELL'),
(10300, 26, 5647.13, -5002.85, 807.57, 0, ''),
(10300, 27, 5641.12, -4973.22, 809.39, 0, ''),
(10300, 28, 5622.97, -4953.58, 811.12, 0, ''),
(10300, 29, 5601.52, -4939.49, 820.77, 0, ''),
(10300, 30, 5571.87, -4936.22, 831.35, 0, ''),
(10300, 31, 5543.23, -4933.67, 838.33, 0, ''),
(10300, 32, 5520.86, -4942.05, 843.02, 0, ''),
(10300, 33, 5509.15, -4946.31, 849.36, 0, ''),
(10300, 34, 5498.45, -4950.08, 849.98, 0, ''),
(10300, 35, 5485.78, -4963.40, 850.43, 0, ''),
(10300, 36, 5467.92, -4980.67, 851.89, 0, 'Cavern 5 - EMOTE_CHANT_SPELL'),
(10300, 37, 5498.68, -4950.45, 849.96, 0, ''),
(10300, 38, 5518.68, -4921.94, 844.65, 0, ''),
(10300, 39, 5517.66, -4920.82, 845.12, 0, 'SAY_REACH_ALTAR_1'),
(10300, 40, 5518.38, -4913.47, 845.57, 0, ''),
(10300, 41, 5511.31, -4913.82, 847.17, 5000, 'light the spotlights'),
(10300, 42, 5511.31, -4913.82, 847.17, 0, 'start altar cinematic - SAY_RANSHALLA_ALTAR_2'),
(10300, 43, 5510.36, -4921.17, 846.33, 0, ''),
(10300, 44, 5517.66, -4920.82, 845.12, 0, 'escort paused');

DELETE FROM script_waypoint WHERE entry=4484;
INSERT INTO script_waypoint VALUES
(4484, 0, 3178.57, 188.52, 4.27, 0, 'SAY_QUEST_START'),
(4484, 1, 3189.82, 198.56, 5.62, 0, ''),
(4484, 2, 3215.21, 185.78, 6.43, 0, ''),
(4484, 3, 3224.05, 183.08, 6.74, 0, ''),
(4484, 4, 3228.11, 194.97, 7.51, 0, ''),
(4484, 5, 3225.33, 201.78, 7.25, 0, ''),
(4484, 6, 3233.33, 226.88, 10.18, 0, ''),
(4484, 7, 3274.12, 225.83, 10.72, 0, ''),
(4484, 8, 3321.63, 209.82, 12.36, 0, ''),
(4484, 9, 3369.66, 226.21, 11.69, 0, ''),
(4484, 10, 3402.35, 227.20, 9.48, 0, ''),
(4484, 11, 3441.92, 224.75, 10.85, 0, ''),
(4484, 12, 3453.87, 220.31, 12.52, 0, ''),
(4484, 13, 3472.51, 213.68, 13.26, 0, ''),
(4484, 14, 3515.49, 212.96, 9.76, 5000, 'SAY_FIRST_AMBUSH_START'),
(4484, 15, 3516.21, 212.84, 9.52, 20000, 'SAY_FIRST_AMBUSH_END'),
(4484, 16, 3548.22, 217.12, 7.34, 0, ''),
(4484, 17, 3567.57, 219.43, 5.22, 0, ''),
(4484, 18, 3659.85, 209.68, 2.27, 0, ''),
(4484, 19, 3734.90, 177.64, 6.75, 0, ''),
(4484, 20, 3760.24, 162.51, 7.49, 5000, 'SAY_SECOND_AMBUSH_START'),
(4484, 21, 3761.58, 161.14, 7.37, 20000, 'SAY_SECOND_AMBUSH_END'),
(4484, 22, 3801.17, 129.87, 9.38, 0, ''),
(4484, 23, 3815.53, 118.53, 10.14, 0, ''),
(4484, 24, 3894.58, 44.88, 15.49, 0, ''),
(4484, 25, 3972.83, 0.42, 17.34, 0, ''),
(4484, 26, 4026.41, -7.63, 16.77, 0, ''),
(4484, 27, 4086.24, 12.32, 16.12, 0, ''),
(4484, 28, 4158.79, 50.67, 25.86, 0, ''),
(4484, 29, 4223.48, 99.52, 35.47, 5000, 'SAY_FINAL_AMBUSH_START'),
(4484, 30, 4224.28, 100.02, 35.49, 10000, 'SAY_QUEST_END'),
(4484, 31, 4243.45, 117.44, 38.83, 0, ''),
(4484, 32, 4264.18, 134.22, 42.96, 0, '');

DELETE FROM script_waypoint WHERE entry=12277;
INSERT INTO script_waypoint VALUES
(12277, 1, -1154.87, 2708.16, 111.123, 1000, 'SAY_MELIZZA_START'),
(12277, 2, -1162.62, 2712.86, 111.549, 0, ''),
(12277, 3, -1183.37, 2709.45, 111.601, 0, ''),
(12277, 4, -1245.09, 2676.43, 111.572, 0, ''),
(12277, 5, -1260.54, 2672.48, 111.55, 0, ''),
(12277, 6, -1272.71, 2666.38, 111.555, 0, ''),
(12277, 7, -1342.95, 2580.82, 111.557, 0, ''),
(12277, 8, -1362.24, 2561.74, 110.848, 0, ''),
(12277, 9, -1376.56, 2514.06, 95.6146, 0, ''),
(12277, 10, -1379.06, 2510.88, 93.3256, 0, ''),
(12277, 11, -1383.14, 2489.17, 89.009, 0, ''),
(12277, 12, -1395.34, 2426.15, 88.6607, 0, 'SAY_MELIZZA_FINISH'),
(12277, 13, -1366.23, 2317.17, 91.8086, 0, ''),
(12277, 14, -1353.81, 2213.52, 90.726, 0, ''),
(12277, 15, -1354.19, 2208.28, 88.7386, 0, ''),
(12277, 16, -1354.59, 2193.77, 77.6702, 0, ''),
(12277, 17, -1367.62, 2160.64, 67.1482, 0, ''),
(12277, 18, -1379.44, 2132.77, 64.1326, 0, ''),
(12277, 19, -1404.81, 2088.68, 61.8162, 0, 'SAY_MELIZZA_1'),
(12277, 20, -1417.15, 2082.65, 62.4112, 0, ''),
(12277, 21, -1423.28, 2074.19, 62.2046, 0, ''),
(12277, 22, -1432.99, 2070.56, 61.7811, 0, ''),
(12277, 23, -1469.27, 2078.68, 63.1141, 0, ''),
(12277, 24, -1507.21, 2115.12, 62.3578, 0, '');

DELETE FROM script_waypoint WHERE entry=3692;
INSERT INTO script_waypoint VALUES
(3692, 1, 4608.43, -6.32, 69.74, 1000, 'stand up'),
(3692, 2, 4608.43, -6.32, 69.74, 4000, 'SAY_START'),
(3692, 3, 4604.54, -5.17, 69.51, 0, ''),
(3692, 4, 4604.26, -2.02, 69.42, 0, ''),
(3692, 5, 4607.75, 3.79, 70.13, 1000, 'first ambush'),
(3692, 6, 4607.75, 3.79, 70.13, 0, 'SAY_FIRST_AMBUSH'),
(3692, 7, 4619.77, 27.47, 70.40, 0, ''),
(3692, 8, 4626.28, 42.46, 68.75, 0, ''),
(3692, 9, 4633.13, 51.17, 67.40, 0, ''),
(3692, 10, 4639.67, 79.03, 61.74, 0, ''),
(3692, 11, 4647.54, 94.25, 59.92, 0, 'second ambush'),
(3692, 12, 4682.08, 113.47, 54.83, 0, ''),
(3692, 13, 4705.28, 137.81, 53.36, 0, 'last ambush'),
(3692, 14, 4730.30, 158.76, 52.33, 0, ''),
(3692, 15, 4756.47, 195.65, 53.61, 10000, 'SAY_END'),
(3692, 16, 4608.43, -6.32, 69.74, 1000, 'bow'),
(3692, 17, 4608.43, -6.32, 69.74, 4000, 'SAY_ESCAPE'),
(3692, 18, 4608.43, -6.32, 69.74, 4000, 'SPELL_MOONSTALKER_FORM'),
(3692, 19, 4604.54, -5.17, 69.51, 0, ''),
(3692, 20, 4604.26, -2.02, 69.42, 0, ''),
(3692, 21, 4607.75, 3.79, 70.13, 0, ''),
(3692, 22, 4607.75, 3.79, 70.13, 0, ''),
(3692, 23, 4619.77, 27.47, 70.40, 0, ''),
(3692, 24, 4640.33, 33.74, 68.22, 0, 'quest complete');

DELETE FROM script_waypoint WHERE entry=22424;
INSERT INTO script_waypoint VALUES
(22424, 1, -3620.54, 4164.57, 1.81, 0, 'SKYWING_START'),
(22424, 2, -3624.78, 4149.65, 7.44, 0, ''),
(22424, 3, -3630.30, 4124.84, 21.28, 0, ''),
(22424, 4, -3629.14, 4093.65, 44.35, 0, ''),
(22424, 5, -3626.34, 4080.29, 52.39, 0, ''),
(22424, 6, -3619.35, 4063.86, 60.86, 3000, 'SAY_SKYWING_TREE_DOWN'),
(22424, 7, -3615.09, 4054.17, 62.46, 0, ''),
(22424, 8, -3611.39, 4046.60, 65.07, 0, ''),
(22424, 9, -3606.68, 4040.50, 66.00, 0, ''),
(22424, 10, -3600.88, 4038.69, 67.14, 0, ''),
(22424, 11, -3597.88, 4033.84, 68.53, 0, ''),
(22424, 12, -3602.19, 4027.89, 69.36, 0, ''),
(22424, 13, -3609.85, 4028.37, 70.78, 0, ''),
(22424, 14, -3613.01, 4031.10, 72.14, 0, ''),
(22424, 15, -3613.18, 4035.63, 73.52, 0, ''),
(22424, 16, -3609.84, 4039.73, 75.25, 0, ''),
(22424, 17, -3604.55, 4040.12, 77.01, 0, ''),
(22424, 18, -3600.61, 4036.03, 78.84, 0, ''),
(22424, 19, -3602.63, 4029.99, 81.01, 0, ''),
(22424, 20, -3608.87, 4028.64, 83.27, 0, ''),
(22424, 21, -3612.53, 4032.74, 85.24, 0, ''),
(22424, 22, -3611.08, 4038.13, 87.31, 0, ''),
(22424, 23, -3605.09, 4039.35, 89.55, 0, ''),
(22424, 24, -3601.87, 4035.44, 91.64, 0, ''),
(22424, 25, -3603.08, 4030.58, 93.66, 0, ''),
(22424, 26, -3608.47, 4029.23, 95.91, 0, ''),
(22424, 27, -3611.68, 4033.35, 98.09, 0, ''),
(22424, 28, -3609.51, 4038.25, 100.45, 0, ''),
(22424, 29, -3604.54, 4038.01, 102.72, 0, ''),
(22424, 30, -3602.40, 4033.48, 105.12, 0, ''),
(22424, 31, -3606.17, 4029.69, 107.63, 0, ''),
(22424, 32, -3609.93, 4031.26, 109.53, 0, ''),
(22424, 33, -3609.38, 4035.86, 110.67, 0, ''),
(22424, 34, -3603.58, 4043.03, 112.89, 0, ''),
(22424, 35, -3600.99, 4046.49, 111.81, 0, ''),
(22424, 36, -3602.32, 4051.77, 111.81, 3000, 'SAY_SKYWING_TREE_UP'),
(22424, 37, -3609.55, 4055.95, 112.00, 0, ''),
(22424, 38, -3620.93, 4043.77, 111.99, 0, ''),
(22424, 39, -3622.44, 4038.95, 111.99, 0, ''),
(22424, 40, -3621.64, 4025.39, 111.99, 0, ''),
(22424, 41, -3609.62, 4015.20, 111.99, 0, ''),
(22424, 42, -3598.37, 4017.72, 111.99, 0, ''),
(22424, 43, -3590.21, 4026.62, 111.99, 0, ''),
(22424, 44, -3586.55, 4034.13, 112.00, 0, ''),
(22424, 45, -3580.39, 4033.46, 112.00, 0, ''),
(22424, 46, -3568.83, 4032.53, 107.16, 0, ''),
(22424, 47, -3554.81, 4031.23, 105.31, 0, ''),
(22424, 48, -3544.39, 4030.10, 106.58, 0, ''),
(22424, 49, -3531.91, 4029.25, 111.70, 0, ''),
(22424, 50, -3523.50, 4030.24, 112.47, 0, ''),
(22424, 51, -3517.48, 4037.42, 112.66, 0, ''),
(22424, 52, -3510.40, 4040.77, 112.92, 0, ''),
(22424, 53, -3503.83, 4041.35, 113.17, 0, ''),
(22424, 54, -3498.31, 4040.65, 113.11, 0, ''),
(22424, 55, -3494.05, 4031.67, 113.11, 0, ''),
(22424, 56, -3487.71, 4025.58, 113.12, 0, ''),
(22424, 57, -3500.42, 4012.93, 113.11, 0, ''),
(22424, 58, -3510.86, 4010.15, 113.10, 0, ''),
(22424, 59, -3518.07, 4008.62, 112.97, 0, ''),
(22424, 60, -3524.74, 4014.55, 112.41, 2000, 'SAY_SKYWING_JUMP'),
(22424, 61, -3537.81, 4008.59, 92.53, 0, ''),
(22424, 62, -3546.25, 4008.81, 92.79, 0, ''),
(22424, 63, -3552.07, 4006.48, 92.84, 0, ''),
(22424, 64, -3556.29, 4000.14, 92.92, 0, ''),
(22424, 65, -3556.16, 3991.24, 92.92, 0, ''),
(22424, 66, -3551.48, 3984.28, 92.91, 0, ''),
(22424, 67, -3542.90, 3981.64, 92.91, 0, ''),
(22424, 68, -3534.82, 3983.98, 92.92, 0, ''),
(22424, 69, -3530.58, 3989.91, 92.85, 0, ''),
(22424, 70, -3529.85, 3998.77, 92.59, 0, ''),
(22424, 71, -3534.15, 4008.45, 92.34, 0, ''),
(22424, 72, -3532.87, 4012.97, 91.64, 0, ''),
(22424, 73, -3530.57, 4023.42, 86.82, 0, ''),
(22424, 74, -3528.24, 4033.91, 85.69, 0, ''),
(22424, 75, -3526.22, 4043.75, 87.26, 0, ''),
(22424, 76, -3523.84, 4054.29, 92.42, 0, ''),
(22424, 77, -3522.44, 4059.06, 92.92, 0, ''),
(22424, 78, -3514.26, 4060.72, 92.92, 0, ''),
(22424, 79, -3507.76, 4065.21, 92.92, 0, ''),
(22424, 80, -3503.24, 4076.63, 92.92, 0, 'SAY_SKYWING_SUMMON'),
(22424, 81, -3504.23, 4080.47, 92.92, 7000, 'SPELL_TRANSFORM'),
(22424, 82, -3504.23, 4080.47, 92.92, 20000, 'SAY_SKYWING_END');

DELETE FROM script_waypoint WHERE entry=17804;
INSERT INTO script_waypoint VALUES
(17804, 0, -9054.86, 443.58, 93.05, 0, ''),
(17804, 1, -9079.33, 424.49, 92.52, 0, ''),
(17804, 2, -9086.21, 419.02, 92.32, 3000, ''),
(17804, 3, -9086.21, 419.02, 92.32, 1000, ''),
(17804, 4, -9079.33, 424.49, 92.52, 0, ''),
(17804, 5, -9054.38, 436.30, 93.05, 0, ''),
(17804, 6, -9042.23, 434.24, 93.37, 5000, 'SAY_SIGNAL_SENT');

DELETE FROM script_waypoint WHERE entry=12580;
INSERT INTO script_waypoint VALUES
(12580, 0, -8997.63, 486.402, 96.622, 0, ''),
(12580, 1, -8971.08, 507.541, 96.349, 0, 'SAY_DIALOG_1'),
(12580, 2, -8953.17, 518.537, 96.355, 0, ''),
(12580, 3, -8936.33, 501.777, 94.066, 0, ''),
(12580, 4, -8922.52, 498.45, 93.869, 0, ''),
(12580, 5, -8907.64, 509.941, 93.840, 0, ''),
(12580, 6, -8925.26, 542.51, 94.274, 0, ''),
(12580, 7, -8832.28, 622.285, 93.686, 0, ''),
(12580, 8, -8824.8, 621.713, 94.084, 0, ''),
(12580, 9, -8796.46, 590.922, 97.466, 0, ''),
(12580, 10, -8769.85, 607.883, 97.118, 0, ''),
(12580, 11, -8737.14, 574.741, 97.398, 0, 'reset jonathan'),
(12580, 12, -8746.27, 563.446, 97.399, 0, ''),
(12580, 13, -8745.5, 557.877, 97.704, 0, ''),
(12580, 14, -8730.95, 541.477, 101.12, 0, ''),
(12580, 15, -8713.16, 520.692, 97.227, 0, ''),
(12580, 16, -8677.09, 549.614, 97.438, 0, ''),
(12580, 17, -8655.72, 552.732, 96.941, 0, ''),
(12580, 18, -8641.68, 540.516, 98.972, 0, ''),
(12580, 19, -8620.08, 520.120, 102.812, 0, ''),
(12580, 20, -8591.09, 492.553, 104.032, 0, ''),
(12580, 21, -8562.45, 463.583, 104.517, 0, ''),
(12580, 22, -8548.63, 467.38, 104.517, 0, 'SAY_WINDSOR_BEFORE_KEEP'),
(12580, 23, -8487.77, 391.44, 108.386, 0, ''),
(12580, 24, -8455.95, 351.225, 120.88, 0, ''),
(12580, 25, -8446.87, 339.904, 121.33, 0, 'SAY_WINDSOR_KEEP_1'),
(12580, 26, -8446.87, 339.904, 121.33, 10000, '');

DELETE FROM script_waypoint WHERE entry=9520;
INSERT INTO script_waypoint VALUES
(9520, 1, -7699.62, -1444.29, 139.87, 4000, 'SAY_START'),
(9520, 2, -7670.67, -1458.25, 140.74, 0, ''),
(9520, 3, -7675.26, -1465.58, 140.74, 0, ''),
(9520, 4, -7685.84, -1472.66, 140.75, 0, ''),
(9520, 5, -7700.08, -1473.41, 140.79, 0, ''),
(9520, 6, -7712.55, -1470.19, 140.79, 0, ''),
(9520, 7, -7717.27, -1481.70, 140.72, 5000, 'SAY_PAY'),
(9520, 8, -7726.23, -1500.78, 132.99, 0, ''),
(9520, 9, -7744.61, -1531.61, 132.69, 0, ''),
(9520, 10, -7763.08, -1536.22, 131.93, 0, ''),
(9520, 11, -7815.32, -1522.61, 134.16, 0, ''),
(9520, 12, -7850.26, -1516.87, 138.17, 0, 'SAY_FIRST_AMBUSH_START'),
(9520, 13, -7850.26, -1516.87, 138.17, 3000, 'SAY_FIRST_AMBUSH_END'),
(9520, 14, -7881.01, -1508.49, 142.37, 0, ''),
(9520, 15, -7888.91, -1458.09, 144.79, 0, ''),
(9520, 16, -7889.18, -1430.21, 145.31, 0, ''),
(9520, 17, -7900.53, -1427.01, 150.26, 0, ''),
(9520, 18, -7904.15, -1429.91, 150.27, 0, ''),
(9520, 19, -7921.48, -1425.47, 140.54, 0, ''),
(9520, 20, -7941.43, -1413.10, 134.35, 0, ''),
(9520, 21, -7964.85, -1367.45, 132.99, 0, ''),
(9520, 22, -7989.95, -1319.121, 133.71, 0, ''),
(9520, 23, -8010.43, -1270.23, 133.42, 0, ''),
(9520, 24, -8025.62, -1243.78, 133.91, 0, 'SAY_SEC_AMBUSH_START'),
(9520, 25, -8025.62, -1243.78, 133.91, 3000, 'SAY_SEC_AMBUSH_END'),
(9520, 26, -8015.22, -1196.98, 146.76, 0, ''),
(9520, 27, -7994.68, -1151.38, 160.70, 0, ''),
(9520, 28, -7970.91, -1132.81, 170.16, 0, 'summon Searscale Drakes'),
(9520, 29, -7927.59, -1122.79, 185.86, 0, ''),
(9520, 30, -7897.67, -1126.67, 194.32, 0, 'SAY_THIRD_AMBUSH_START'),
(9520, 31, -7897.67, -1126.67, 194.32, 3000, 'SAY_THIRD_AMBUSH_END'),
(9520, 32, -7864.11, -1135.98, 203.29, 0, ''),
(9520, 33, -7837.31, -1137.73, 209.63, 0, ''),
(9520, 34, -7808.72, -1134.90, 214.84, 0, ''),
(9520, 35, -7786.85, -1127.24, 214.84, 0, ''),
(9520, 36, -7746.58, -1125.16, 215.08, 5000, 'EMOTE_LAUGH'),
(9520, 37, -7746.41, -1103.62, 215.62, 0, ''),
(9520, 38, -7740.25, -1090.51, 216.69, 0, ''),
(9520, 39, -7730.97, -1085.55, 217.12, 0, ''),
(9520, 40, -7697.89, -1089.43, 217.62, 0, ''),
(9520, 41, -7679.30, -1059.15, 220.09, 0, ''),
(9520, 42, -7661.39, -1038.24, 226.24, 0, ''),
(9520, 43, -7634.49, -1020.96, 234.30, 0, ''),
(9520, 44, -7596.22, -1013.16, 244.03, 0, ''),
(9520, 45, -7556.53, -1021.74, 253.21, 0, 'SAY_LAST_STAND');

DELETE FROM script_waypoint WHERE entry=9023;
INSERT INTO script_waypoint VALUES
(9023, 1, 316.336, -225.528, -77.7258, 2000, 'SAY_WINDSOR_START'),
(9023, 2, 322.96, -207.13, -77.87, 0, ''),
(9023, 3, 281.05, -172.16, -75.12, 0, ''),
(9023, 4, 272.19, -139.14, -70.61, 0, ''),
(9023, 5, 283.62, -116.09, -70.21, 0, ''),
(9023, 6, 296.18, -94.30, -74.08, 0, ''),
(9023, 7, 294.57, -93.11, -74.08, 0, 'escort paused - SAY_WINDSOR_CELL_DUGHAL_1'),
(9023, 8, 294.57, -93.11, -74.08, 10000, ''),
(9023, 9, 294.57, -93.11, -74.08, 3000, 'SAY_WINDSOR_CELL_DUGHAL_3'),
(9023, 10, 314.31, -74.31, -76.09, 0, ''),
(9023, 11, 360.22, -62.93, -66.77, 0, ''),
(9023, 12, 383.38, -69.40, -63.25, 0, ''),
(9023, 13, 389.99, -67.86, -62.57, 0, ''),
(9023, 14, 400.98, -72.01, -62.31, 0, 'SAY_WINDSOR_EQUIPMENT_1'),
(9023, 15, 404.22, -62.30, -63.50, 2000, ''),
(9023, 16, 404.22, -62.30, -63.50, 1500, 'open supply door'),
(9023, 17, 407.65, -51.86, -63.96, 0, ''),
(9023, 18, 403.61, -51.71, -63.92, 1000, 'SAY_WINDSOR_EQUIPMENT_2'),
(9023, 19, 403.61, -51.71, -63.92, 2000, ''),
(9023, 20, 403.61, -51.71, -63.92, 1000, 'open supply crate'),
(9023, 21, 403.61, -51.71, -63.92, 1000, 'update entry to Reginald Windsor'),
(9023, 22, 403.61, -52.71, -63.92, 4000, 'SAY_WINDSOR_EQUIPMENT_3'),
(9023, 23, 403.61, -52.71, -63.92, 4000, 'SAY_WINDSOR_EQUIPMENT_4'),
(9023, 24, 406.33, -54.87, -63.95, 0, ''),
(9023, 25, 403.86, -73.88, -62.02, 0, ''),
(9023, 26, 428.80, -81.34, -64.91, 0, ''),
(9023, 27, 557.03, -119.71, -61.83, 0, ''),
(9023, 28, 573.40, -124.39, -65.07, 0, ''),
(9023, 29, 593.91, -130.29, -69.25, 0, ''),
(9023, 30, 593.21, -132.16, -69.25, 0, 'escort paused - SAY_WINDSOR_CELL_JAZ_1'),
(9023, 31, 593.21, -132.16, -69.25, 1000, ''),
(9023, 32, 593.21, -132.16, -69.25, 3000, 'SAY_WINDSOR_CELL_JAZ_2'),
(9023, 33, 622.81, -135.55, -71.92, 0, ''),
(9023, 34, 634.68, -151.29, -70.32, 0, ''),
(9023, 35, 635.06, -153.25, -70.32, 0, 'escort paused - SAY_WINDSOR_CELL_SHILL_1'),
(9023, 36, 635.06, -153.25, -70.32, 3000, ''),
(9023, 37, 635.06, -153.25, -70.32, 5000, 'SAY_WINDSOR_CELL_SHILL_2'),
(9023, 38, 635.06, -153.25, -70.32, 2000, 'SAY_WINDSOR_CELL_SHILL_3'),
(9023, 39, 655.25, -172.39, -73.72, 0, ''),
(9023, 40, 654.79, -226.30, -83.06, 0, ''),
(9023, 41, 622.85, -268.85, -83.96, 0, ''),
(9023, 42, 579.45, -275.56, -80.44, 0, ''),
(9023, 43, 561.19, -266.85, -75.59, 0, ''),
(9023, 44, 547.91, -253.92, -70.34, 0, ''),
(9023, 45, 549.20, -252.40, -70.34, 0, 'escort paused - SAY_WINDSOR_CELL_CREST_1'),
(9023, 46, 549.20, -252.40, -70.34, 1000, ''),
(9023, 47, 549.20, -252.40, -70.34, 4000, 'SAY_WINDSOR_CELL_CREST_2'),
(9023, 48, 555.33, -269.16, -74.40, 0, ''),
(9023, 49, 554.31, -270.88, -74.40, 0, 'escort paused - SAY_WINDSOR_CELL_TOBIAS_1'),
(9023, 50, 554.31, -270.88, -74.40, 10000, ''),
(9023, 51, 554.31, -270.88, -74.40, 4000, 'SAY_WINDSOR_CELL_TOBIAS_2'),
(9023, 52, 536.10, -249.60, -67.47, 0, ''),
(9023, 53, 520.94, -216.65, -59.28, 0, ''),
(9023, 54, 505.99, -148.74, -62.17, 0, ''),
(9023, 55, 484.21, -56.24, -62.43, 0, ''),
(9023, 56, 470.39, -6.01, -70.10, 0, ''),
(9023, 57, 452.45, 29.85, -70.37, 1500, 'SAY_WINDSOR_FREE_1'),
(9023, 58, 452.45, 29.85, -70.37, 15000, 'SAY_WINDSOR_FREE_2');

DELETE FROM script_waypoint WHERE entry=17225;
INSERT INTO script_waypoint VALUES
(17225, 0, -11033.51, -1784.65, 182.284, 3000, ''),
(17225, 1, -11107.57, -1873.36, 136.878, 0, ''),
(17225, 2, -11118.71, -1883.65, 132.441, 0, ''),
(17225, 3, -11132.92, -1888.12, 128.969, 0, ''),
(17225, 4, -11150.31, -1890.54, 126.557, 0, ''),
(17225, 5, -11160.64, -1891.63, 124.793, 0, ''),
(17225, 6, -11171.52, -1889.45, 123.417, 0, ''),
(17225, 7, -11183.46, -1884.09, 119.754, 0, ''),
(17225, 8, -11196.25, -1874.01, 115.227, 0, ''),
(17225, 9, -11205.59, -1859.66, 110.216, 0, ''),
(17225, 10, -11236.53, -1818.03, 97.3972, 0, ''),
(17225, 11, -11253.11, -1794.48, 93.3101, 0, ''),
(17225, 12, -11254.86, -1787.13, 92.5174, 0, ''),
(17225, 13, -11253.32, -1777.08, 91.7739, 0, ''),
(17225, 14, -11247.48, -1770.27, 92.4183, 0, ''),
(17225, 15, -11238.61, -1766.51, 94.6417, 0, ''),
(17225, 16, -11227.56, -1767.22, 100.256, 0, ''),
(17225, 17, -11218.41, -1770.55, 107.859, 0, ''),
(17225, 18, -11204.81, -1781.77, 110.383, 0, ''),
(17225, 19, -11195.77, -1801.07, 110.833, 0, ''),
(17225, 20, -11195.81, -1824.66, 113.936, 0, ''),
(17225, 21, -11197.11, -1860.01, 117.945, 0, ''),
(17225, 22, -11194.60, -1884.23, 121.401, 0, ''),
(17225, 23, -11184.21, -1894.78, 120.326, 0, ''),
(17225, 24, -11176.91, -1899.84, 119.844, 0, ''),
(17225, 25, -11168.13, -1901.77, 118.958, 0, ''),
(17225, 26, -11154.91, -1901.66, 117.218, 0, ''),
(17225, 27, -11143.15, -1901.22, 115.885, 0, ''),
(17225, 28, -11131.19, -1897.59, 113.722, 0, ''),
(17225, 29, -11121.31, -1890.25, 111.643, 0, ''),
(17225, 30, -11118.22, -1883.83, 110.595, 3000, ''),
(17225, 31, -11118.45, -1883.68, 91.473, 0, 'start combat');

DELETE FROM script_waypoint WHERE entry=20802;
INSERT INTO script_waypoint VALUES
(20802, 0, 4017.864, 2325.038, 114.029, 3000, 'SAY_INTRO'),
(20802, 1, 4006.373, 2324.593, 111.455, 0, ''),
(20802, 2, 3998.391, 2326.364, 113.164, 0, ''),
(20802, 3, 3982.309, 2330.261, 113.846, 7000, 'SAY_STAGING_GROUNDS'),
(20802, 4, 3950.646, 2329.249, 113.924, 0, 'SAY_TOXIC_HORROR'),
(20802, 5, 3939.229, 2330.994, 112.197, 0, ''),
(20802, 6, 3927.858, 2333.644, 111.330, 0, ''),
(20802, 7, 3917.851, 2337.696, 113.493, 0, ''),
(20802, 8, 3907.743, 2343.336, 114.062, 0, ''),
(20802, 9, 3878.760, 2378.611, 114.037, 8000, 'SAY_SALHADAAR'),
(20802, 10, 3863.153, 2355.876, 114.987, 0, ''),
(20802, 11, 3861.241, 2344.893, 115.201, 0, ''),
(20802, 12, 3872.463, 2323.114, 114.671, 0, 'escort paused - SAY_DISRUPTOR'),
(20802, 13, 3863.740, 2349.790, 115.382, 0, 'SAY_FINISH_2');

DELETE FROM script_waypoint WHERE entry=20763;
INSERT INTO script_waypoint VALUES
(20763, 0, 4084.092, 2297.254, 110.277, 0, ''),
(20763, 1, 4107.174, 2294.916, 106.625, 0, ''),
(20763, 2, 4154.129, 2296.789, 102.331, 0, ''),
(20763, 3, 4166.021, 2302.819, 103.422, 0, ''),
(20763, 4, 4195.039, 2301.094, 113.786, 0, ''),
(20763, 5, 4205.246, 2297.116, 117.992, 0, ''),
(20763, 6, 4230.429, 2294.642, 127.307, 0, ''),
(20763, 7, 4238.981, 2293.579, 129.332, 0, ''),
(20763, 8, 4250.184, 2293.272, 129.009, 0, ''),
(20763, 9, 4262.810, 2290.768, 126.485, 0, ''),
(20763, 10, 4265.845, 2278.562, 128.235, 0, ''),
(20763, 11, 4265.609, 2265.734, 128.452, 0, ''),
(20763, 12, 4258.838, 2245.354, 132.804, 0, ''),
(20763, 13, 4247.976, 2221.211, 137.668, 0, ''),
(20763, 14, 4247.973, 2213.876, 137.721, 0, ''),
(20763, 15, 4249.876, 2204.265, 137.121, 4000, ''),
(20763, 16, 4249.876, 2204.265, 137.121, 0, 'SAY_VANGUARD_FINISH'),
(20763, 17, 4252.455, 2170.885, 137.677, 3000, 'EMOTE_VANGUARD_FINISH'),
(20763, 18, 4252.455, 2170.885, 137.677, 5000, '');

DELETE FROM script_waypoint WHERE entry=23089;
INSERT INTO script_waypoint VALUES
(23089, 0, 660.22, 305.74, 271.688, 0, 'escort paused - GOSSIP_ITEM_PREPARE'),
(23089, 1, 675.10, 343.30, 271.688, 0, ''),
(23089, 2, 694.01, 374.84, 271.687, 0, ''),
(23089, 3, 706.22, 375.75, 274.888, 0, ''),
(23089, 4, 720.48, 370.38, 281.300, 0, ''),
(23089, 5, 733.30, 357.66, 292.477, 0, ''),
(23089, 6, 740.40, 344.39, 300.920, 0, ''),
(23089, 7, 747.54, 329.03, 308.509, 0, ''),
(23089, 8, 748.24, 318.78, 311.781, 0, ''),
(23089, 9, 752.41, 304.31, 312.077, 0, 'escort paused - SAY_AKAMA_OPEN_DOOR_1'),
(23089, 10, 770.27, 304.89, 312.35, 0, ''),
(23089, 11, 780.18, 305.26, 319.71, 0, ''),
(23089, 12, 791.45, 289.27, 319.80, 0, ''),
(23089, 13, 790.41, 262.70, 341.42, 0, ''),
(23089, 14, 782.88, 250.20, 341.60, 0, ''),
(23089, 15, 765.35, 241.40, 353.62, 0, ''),
(23089, 16, 750.61, 235.63, 353.02, 0, 'escort paused - GOSSIP_ITEM_START_EVENT'),
(23089, 17, 748.87, 304.93, 352.99, 0, 'escort paused - SAY_ILLIDAN_SPEECH_1'),
(23089, 18, 737.92, 368.15, 352.99, 0, ''),
(23089, 19, 749.64, 378.69, 352.99, 0, ''),
(23089, 20, 766.49, 371.79, 353.63, 0, ''),
(23089, 21, 784.98, 361.89, 341.41, 0, ''),
(23089, 22, 791.44, 347.10, 341.41, 0, ''),
(23089, 23, 794.80, 319.47, 319.75, 0, ''),
(23089, 24, 794.34, 304.34, 319.75, 0, 'escort paused - fight illidari elites'),
(23089, 25, 794.80, 319.47, 319.75, 0, ''),
(23089, 26, 791.44, 347.10, 341.41, 0, ''),
(23089, 27, 784.98, 361.89, 341.41, 0, ''),
(23089, 28, 766.49, 371.79, 353.63, 0, ''),
(23089, 29, 749.64, 378.69, 352.99, 0, ''),
(23089, 30, 737.92, 368.15, 352.99, 0, 'escort paused');

DELETE FROM script_waypoint WHERE entry=3584;
INSERT INTO script_waypoint VALUES
(3584, 0, 4520.4, 420.235, 33.5284, 2000, ''),
(3584, 1, 4512.26, 408.881, 32.9308, 0, ''),
(3584, 2, 4507.94, 396.47, 32.9476, 0, ''),
(3584, 3, 4507.53, 383.781, 32.995, 0, ''),
(3584, 4, 4512.1, 374.02, 33.166, 0, ''),
(3584, 5, 4519.75, 373.241, 33.1574, 0, ''),
(3584, 6, 4592.41, 369.127, 31.4893, 0, ''),
(3584, 7, 4598.55, 364.801, 31.4947, 0, ''),
(3584, 8, 4602.76, 357.649, 32.9265, 0, ''),
(3584, 9, 4597.88, 352.629, 34.0317, 0, ''),
(3584, 10, 4590.23, 350.9, 36.2977, 0, ''),
(3584, 11, 4581.5, 348.254, 38.3878, 0, ''),
(3584, 12, 4572.05, 348.059, 42.3539, 0, ''),
(3584, 13, 4564.75, 344.041, 44.2463, 0, ''),
(3584, 14, 4556.63, 341.003, 47.6755, 0, ''),
(3584, 15, 4554.38, 334.968, 48.8003, 0, ''),
(3584, 16, 4557.63, 329.783, 49.9532, 0, ''),
(3584, 17, 4563.32, 316.829, 53.2409, 0, ''),
(3584, 18, 4566.09, 303.127, 55.0396, 0, ''),
(3584, 19, 4561.65, 295.456, 57.0984, 4000, 'SAY_THERYLUNE_FINISH'),
(3584, 20, 4551.03, 293.333, 57.1534, 2000, '');

DELETE FROM script_waypoint WHERE entry=17238;
INSERT INTO script_waypoint VALUES
(17238, 0, 954.21, -1433.72, 63.00, 0, ''),
(17238, 1, 972.70, -1438.85, 65.56, 0, ''),
(17238, 2, 984.79, -1444.15, 64.13, 0, ''),
(17238, 3, 999.00, -1451.74, 61.20, 0, ''),
(17238, 4, 1030.94, -1470.39, 63.49, 25000, 'SAY_FIRST_STOP'),
(17238, 5, 1030.94, -1470.39, 63.49, 3000, 'SAY_CONTINUE'),
(17238, 6, 1036.50, -1484.25, 64.60, 0, ''),
(17238, 7, 1039.11, -1501.22, 65.32, 0, ''),
(17238, 8, 1038.44, -1522.18, 64.55, 0, ''),
(17238, 9, 1037.19, -1543.15, 62.33, 0, ''),
(17238, 10, 1036.79, -1563.88, 61.93, 5000, 'SAY_FIRST_ATTACK'),
(17238, 11, 1036.79, -1563.88, 61.93, 5000, 'SAY_PURITY'),
(17238, 12, 1035.61, -1587.64, 61.66, 0, ''),
(17238, 13, 1035.43, -1612.97, 61.54, 0, ''),
(17238, 14, 1035.36, -1630.66, 61.53, 0, ''),
(17238, 15, 1038.85, -1653.02, 60.35, 0, ''),
(17238, 16, 1042.27, -1669.36, 60.75, 0, ''),
(17238, 17, 1050.41, -1687.22, 60.52, 0, ''),
(17238, 18, 1061.15, -1704.45, 60.59, 0, ''),
(17238, 19, 1073.51, -1716.99, 60.65, 0, ''),
(17238, 20, 1084.20, -1727.24, 60.95, 0, ''),
(17238, 21, 1100.71, -1739.89, 60.64, 5000, 'SAY_SECOND_ATTACK'),
(17238, 22, 1100.71, -1739.89, 60.64, 0, 'SAY_CLEANSE'),
(17238, 23, 1117.03, -1749.01, 60.87, 0, ''),
(17238, 24, 1123.58, -1762.29, 62.40, 0, ''),
(17238, 25, 1123.36, -1769.29, 62.83, 0, ''),
(17238, 26, 1115.78, -1779.59, 62.09, 0, ''),
(17238, 27, 1109.56, -1789.78, 61.03, 0, ''),
(17238, 28, 1094.81, -1797.62, 61.22, 0, ''),
(17238, 29, 1079.30, -1801.58, 64.95, 0, ''),
(17238, 30, 1060.24, -1803.40, 70.36, 0, ''),
(17238, 31, 1047.69, -1804.49, 73.92, 0, ''),
(17238, 32, 1032.59, -1805.99, 76.13, 0, ''),
(17238, 33, 1013.60, -1812.36, 77.32, 0, ''),
(17238, 34, 1007.01, -1814.38, 80.48, 0, ''),
(17238, 35, 999.93, -1816.39, 80.48, 2000, 'SAY_WELCOME'),
(17238, 36, 984.72, -1822.05, 80.48, 0, ''),
(17238, 37, 977.77, -1824.80, 80.79, 0, ''),
(17238, 38, 975.33, -1824.91, 81.24, 12000, 'event complete'),
(17238, 39, 975.33, -1824.91, 81.24, 10000, 'SAY_EPILOGUE_1'),
(17238, 40, 975.33, -1824.91, 81.24, 8000, 'SAY_EPILOGUE_2'),
(17238, 41, 975.33, -1824.91, 81.24, 30000, '');

DELETE FROM script_waypoint WHERE entry=2713;
INSERT INTO script_waypoint VALUES
(2713, 0, -1416.91, -3044.12, 36.21, 0, ''),
(2713, 1, -1408.43, -3051.35, 37.79, 0, ''),
(2713, 2, -1399.45, -3069.20, 31.25, 0, ''),
(2713, 3, -1400.28, -3083.14, 27.06, 0, ''),
(2713, 4, -1405.30, -3096.72, 26.36, 0, ''),
(2713, 5, -1406.12, -3105.95, 24.82, 0, ''),
(2713, 6, -1417.41, -3106.80, 16.61, 0, ''),
(2713, 7, -1433.06, -3101.55, 12.56, 0, ''),
(2713, 8, -1439.86, -3086.36, 12.29, 0, ''),
(2713, 9, -1450.48, -3065.16, 12.58, 5000, 'SAY_REACH_BOTTOM'),
(2713, 10, -1456.15, -3055.53, 12.54, 0, ''),
(2713, 11, -1459.41, -3035.16, 12.11, 0, ''),
(2713, 12, -1472.47, -3034.18, 12.44, 0, ''),
(2713, 13, -1495.57, -3034.48, 12.55, 0, ''),
(2713, 14, -1524.91, -3035.47, 13.15, 0, ''),
(2713, 15, -1549.05, -3037.77, 12.98, 0, ''),
(2713, 16, -1555.69, -3028.02, 13.64, 3000, 'SAY_WATCH_BACK'),
(2713, 17, -1555.69, -3028.02, 13.64, 5000, 'SAY_DATA_FOUND'),
(2713, 18, -1555.69, -3028.02, 13.64, 2000, 'SAY_ESCAPE'),
(2713, 19, -1551.19, -3037.78, 12.96, 0, ''),
(2713, 20, -1584.60, -3048.77, 13.67, 0, ''),
(2713, 21, -1602.14, -3042.82, 15.12, 0, ''),
(2713, 22, -1610.68, -3027.42, 17.22, 0, ''),
(2713, 23, -1601.65, -3007.97, 24.65, 0, ''),
(2713, 24, -1581.05, -2992.32, 30.85, 0, ''),
(2713, 25, -1559.95, -2979.51, 34.30, 0, ''),
(2713, 26, -1536.51, -2969.78, 32.64, 0, ''),
(2713, 27, -1511.81, -2961.09, 29.12, 0, ''),
(2713, 28, -1484.83, -2960.87, 32.54, 0, ''),
(2713, 29, -1458.23, -2966.80, 40.52 , 0, ''),
(2713, 30, -1440.20, -2971.20, 43.15, 0, ''),
(2713, 31, -1427.85, -2989.15, 38.09, 0, ''),
(2713, 32, -1420.27, -3008.91, 35.01, 0, ''),
(2713, 33, -1427.58, -3032.53, 32.31, 5000, 'SAY_FINISH'),
(2713, 34, -1427.40, -3035.17, 32.26, 0, '');

DELETE FROM script_waypoint WHERE entry=4880;
INSERT INTO script_waypoint VALUES
(4880, 0, -2670.221, -3446.189, 34.085, 0, ''),
(4880, 1, -2683.958, -3451.094, 34.707, 0, ''),
(4880, 2, -2703.241, -3454.822, 33.395, 0, ''),
(4880, 3, -2721.615, -3457.408, 33.626, 0, ''),
(4880, 4, -2739.977, -3459.843, 33.329, 0, ''),
(4880, 5, -2756.240, -3460.516, 32.037, 5000, 'SAY_STINKY_FIRST_STOP'),
(4880, 6, -2764.517, -3472.714, 33.750, 0, ''),
(4880, 7, -2773.679, -3482.913, 32.840, 0, ''),
(4880, 8, -2781.394, -3490.613, 32.598, 0, ''),
(4880, 9, -2788.308, -3492.904, 30.761, 0, ''),
(4880, 10, -2794.578, -3489.185, 31.119, 5000, 'SAY_SECOND_STOP'),
(4880, 11, -2789.427, -3498.043, 31.050, 0, ''),
(4880, 12, -2786.968, -3508.168, 31.983, 0, ''),
(4880, 13, -2786.770, -3519.953, 31.079, 0, ''),
(4880, 14, -2789.359, -3525.025, 31.831, 0, ''),
(4880, 15, -2797.950, -3523.693, 31.697, 0, ''),
(4880, 16, -2812.971, -3519.838, 29.864, 0, ''),
(4880, 17, -2818.331, -3521.396, 30.563, 0, ''),
(4880, 18, -2824.771, -3528.728, 32.399, 0, ''),
(4880, 19, -2830.697, -3539.875, 32.505, 0, ''),
(4880, 20, -2836.235, -3549.962, 31.180, 0, ''),
(4880, 21, -2837.576, -3561.052, 30.740, 0, ''),
(4880, 22, -2834.445, -3568.264, 30.751, 0, ''),
(4880, 23, -2827.351, -3569.807, 31.316, 0, ''),
(4880, 24, -2817.380, -3566.961, 30.947, 5000, 'SAY_THIRD_STOP_1'),
(4880, 25, -2817.380, -3566.961, 30.947, 2000, 'SAY_THIRD_STOP_2'),
(4880, 26, -2817.380, -3566.961, 30.947, 0, 'SAY_THIRD_STOP_3'),
(4880, 27, -2818.815, -3579.415, 28.525, 0, ''),
(4880, 28, -2820.205, -3590.640, 30.269, 0, ''),
(4880, 29, -2820.849, -3593.938, 31.150, 3000, ''),
(4880, 30, -2820.849, -3593.938, 31.150, 3000, 'SAY_PLANT_GATHERED'),
(4880, 31, -2834.209, -3592.041, 33.790, 0, ''),
(4880, 32, -2840.306, -3586.207, 36.288, 0, ''),
(4880, 33, -2847.491, -3576.416, 37.660, 0, ''),
(4880, 34, -2855.718, -3565.184, 39.390, 0, ''),
(4880, 35, -2861.785, -3552.902, 41.243, 0, ''),
(4880, 36, -2869.542, -3545.579, 40.701, 0, ''),
(4880, 37, -2877.784, -3538.372, 37.274, 0, ''),
(4880, 38, -2882.677, -3534.165, 34.844, 0, ''),
(4880, 39, -2888.567, -3534.117, 34.298, 4000, 'SAY_STINKY_END'),
(4880, 40, -2888.567, -3534.117, 34.298, 0, '');

DELETE FROM script_waypoint WHERE entry=20281;
INSERT INTO script_waypoint VALUES
(20281, 0, 3096.416, 2801.408, 118.149, 7000, 'SAY_DRIJYA_START'),
(20281, 1, 3096.516, 2801.065, 118.128, 0, 'SAY_DRIJYA_1'),
(20281, 2, 3099.995, 2796.665, 118.118, 0, ''),
(20281, 3, 3098.759, 2786.174, 117.125, 0, ''),
(20281, 4, 3087.792, 2754.602, 115.441, 0, ''),
(20281, 5, 3080.718, 2730.793, 115.930, 9000, 'SAY_DRIJYA_2'),
(20281, 6, 3060.235, 2731.306, 115.122, 0, ''),
(20281, 7, 3050.9  , 2727.823, 114.1269, 0, ''),
(20281, 8, 3050.9  , 2727.823, 114.1269, 8000, 'SAY_DRIJYA_4'),
(20281, 9, 3055.008, 2724.972, 113.687, 0, ''),
(20281, 10, 3053.777, 2718.427, 113.684, 0, ''),
(20281, 11, 3028.622, 2693.375, 114.670, 0, ''),
(20281, 12, 3022.430, 2695.297, 113.406, 0, ''),
(20281, 13, 3022.430, 2695.297, 113.406, 8000, 'SAY_DRIJYA_5'),
(20281, 14, 3025.463, 2700.755, 113.514, 0, ''),
(20281, 15, 3011.336, 2716.782, 113.691, 0, ''),
(20281, 16, 3010.882, 2726.991, 114.239, 0, ''),
(20281, 17, 3009.178, 2729.083, 114.324, 0, ''),
(20281, 18, 3009.178, 2729.083, 114.324, 15000, 'SAY_DRIJYA_6'),
(20281, 19, 3009.178, 2729.083, 114.324, 6000, 'SPELL_EXPLOSION_VISUAL'),
(20281, 20, 3009.178, 2729.083, 114.324, 8000, 'SAY_DRIJYA_7'),
(20281, 21, 3033.888, 2736.437, 114.369, 0, ''),
(20281, 22, 3071.492, 2741.502, 116.462, 0, ''),
(20281, 23, 3087.792, 2754.602, 115.441, 0, ''),
(20281, 24, 3094.505, 2770.198, 115.744, 0, ''),
(20281, 25, 3103.510, 2784.362, 116.857, 0, ''),
(20281, 26, 3099.995, 2796.665, 118.118, 0, ''),
(20281, 27, 3096.290, 2801.027, 118.096, 0, 'SAY_DRIJYA_COMPLETE');

DELETE FROM script_waypoint WHERE entry=8284;
INSERT INTO script_waypoint VALUES
(8284, 0, -7007.209, -1749.160, 234.182, 3000, 'stand up'),
(8284, 1, -7007.324, -1729.849, 234.162, 0, ''),
(8284, 2, -7006.394, -1726.522, 234.099, 0, ''),
(8284, 3, -7003.256, -1726.903, 234.594, 0, ''),
(8284, 4, -6994.778, -1733.571, 238.281, 0, ''),
(8284, 5, -6987.904, -1735.935, 240.727, 0, ''),
(8284, 6, -6978.704, -1736.991, 241.809, 0, ''),
(8284, 7, -6964.261, -1740.251, 241.713, 0, ''),
(8284, 8, -6946.701, -1746.284, 241.667, 0, ''),
(8284, 9, -6938.751, -1749.381, 240.744, 0, ''),
(8284, 10, -6927.004, -1768.782, 240.744, 0, ''),
(8284, 11, -6909.453, -1791.258, 240.744, 0, ''),
(8284, 12, -6898.225, -1804.870, 240.744, 0, ''),
(8284, 13, -6881.280, -1821.788, 240.744, 0, ''),
(8284, 14, -6867.653, -1832.672, 240.706, 0, ''),
(8284, 15, -6850.184, -1839.254, 243.006, 0, ''),
(8284, 16, -6829.381, -1847.635, 244.190, 0, ''),
(8284, 17, -6804.618, -1857.535, 244.209, 0, ''),
(8284, 18, -6776.421, -1868.879, 244.142, 0, ''),
(8284, 19, -6753.471, -1876.906, 244.170, 10000, 'stop'),
(8284, 20, -6753.471, -1876.906, 244.170, 0, 'ambush'),
(8284, 21, -6731.033, -1884.944, 244.144, 0, ''),
(8284, 22, -6705.738, -1896.779, 244.144, 0, ''),
(8284, 23, -6678.956, -1909.607, 244.369, 0, ''),
(8284, 24, -6654.263, -1916.758, 244.145, 0, ''),
(8284, 25, -6620.604, -1917.608, 244.149, 0, ''),
(8284, 26, -6575.958, -1922.408, 244.149, 0, ''),
(8284, 27, -6554.811, -1929.883, 244.162, 0, ''),
(8284, 28, -6521.856, -1947.322, 244.151, 0, ''),
(8284, 29, -6493.320, -1962.654, 244.151, 0, ''),
(8284, 30, -6463.350, -1975.537, 244.213, 0, ''),
(8284, 31, -6435.428, -1983.847, 244.548, 0, ''),
(8284, 32, -6418.380, -1985.778, 246.554, 0, ''),
(8284, 33, -6389.783, -1986.544, 246.771, 30000, 'quest complete');

DELETE FROM script_waypoint WHERE entry=17877;
INSERT INTO script_waypoint VALUES
(17877, 0, 231.403, 8479.940, 17.928, 3000, ''),
(17877, 1, 214.645, 8469.645, 23.121, 0, ''),
(17877, 2, 208.538, 8463.481, 24.738, 0, ''),
(17877, 3, 196.524, 8446.077, 24.814, 0, ''),
(17877, 4, 188.186, 8431.674, 22.625, 0, ''),
(17877, 5, 181.196, 8420.152, 23.730, 0, ''),
(17877, 6, 171.919, 8406.290, 21.844, 0, ''),
(17877, 7, 166.613, 8396.479, 23.585, 0, ''),
(17877, 8, 167.237, 8386.686, 21.546, 0, ''),
(17877, 9, 169.401, 8372.670, 19.599, 0, ''),
(17877, 10, 174.148, 8342.325, 20.409, 0, ''),
(17877, 11, 173.195, 8324.177, 21.126, 0, ''),
(17877, 12, 172.415, 8310.290, 21.702, 0, ''),
(17877, 13, 173.233, 8298.755, 19.564, 0, ''),
(17877, 14, 173.984, 8287.925, 18.839, 0, ''),
(17877, 15, 189.984, 8266.263, 18.500, 0, ''),
(17877, 16, 204.057, 8256.019, 19.701, 0, ''),
(17877, 17, 212.950, 8248.737, 21.583, 0, ''),
(17877, 18, 223.152, 8240.160, 20.001, 0, ''),
(17877, 19, 230.730, 8232.994, 18.990, 0, ''),
(17877, 20, 238.261, 8223.804, 20.720, 0, ''),
(17877, 21, 247.651, 8214.208, 19.146, 0, ''),
(17877, 22, 259.231, 8207.796, 19.278, 0, ''),
(17877, 23, 272.360, 8204.755, 19.980, 0, ''),
(17877, 24, 282.211, 8202.087, 22.090, 20000, 'SAY_PREPARE'),
(17877, 25, 282.211, 8202.087, 22.090, 0, 'SAY_CAMP_ENTER'),
(17877, 26, 296.006, 8191.644, 21.680, 0, ''),
(17877, 27, 304.472, 8188.048, 20.707, 0, ''),
(17877, 28, 317.574, 8182.044, 18.296, 0, ''),
(17877, 29, 340.046, 8178.776, 17.937, 0, ''),
(17877, 30, 353.799, 8181.222, 18.557, 0, ''),
(17877, 31, 368.231, 8186.324, 22.450, 0, ''),
(17877, 32, 375.737, 8187.030, 23.916, 0, ''),
(17877, 33, 390.067, 8186.638, 21.190, 0, ''),
(17877, 34, 398.699, 8181.824, 18.648, 0, ''),
(17877, 35, 412.325, 8172.612, 17.927, 0, ''),
(17877, 36, 424.541, 8161.957, 19.575, 0, ''),
(17877, 37, 436.900, 8157.407, 22.115, 0, ''),
(17877, 38, 444.548, 8155.414, 23.553, 0, ''),
(17877, 39, 457.201, 8154.233, 23.429, 0, ''),
(17877, 40, 470.989, 8154.142, 21.650, 0, ''),
(17877, 41, 483.435, 8154.151, 20.706, 0, ''),
(17877, 42, 507.558, 8157.515, 21.729, 0, ''),
(17877, 43, 528.036, 8162.028, 22.795, 0, ''),
(17877, 44, 542.402, 8161.099, 22.914, 0, ''),
(17877, 45, 557.286, 8160.273, 23.708, 13000, ''),
(17877, 46, 557.286, 8160.273, 23.708, 0, 'take the Ark'),
(17877, 47, 539.767, 8144.839, 22.217, 0, ''),
(17877, 48, 531.296, 8139.475, 22.146, 0, ''),
(17877, 49, 509.056, 8139.262, 20.705, 0, ''),
(17877, 50, 499.975, 8136.228, 20.408, 0, ''),
(17877, 51, 485.511, 8129.389, 22.010, 0, ''),
(17877, 52, 474.371, 8128.534, 22.657, 0, ''),
(17877, 53, 460.708, 8130.115, 20.946, 0, ''),
(17877, 54, 449.248, 8129.271, 21.033, 0, ''),
(17877, 55, 433.670, 8125.064, 18.440, 0, ''),
(17877, 56, 412.822, 8121.581, 17.603, 0, ''),
(17877, 57, 391.150, 8117.812, 17.736, 0, ''),
(17877, 58, 379.024, 8114.185, 17.889, 0, ''),
(17877, 59, 365.110, 8106.992, 18.220, 0, ''),
(17877, 60, 352.531, 8108.944, 17.932, 0, ''),
(17877, 61, 340.894, 8120.636, 17.374, 0, ''),
(17877, 62, 328.480, 8134.929, 18.112, 0, ''),
(17877, 63, 317.573, 8143.246, 20.604, 0, ''),
(17877, 64, 311.146, 8146.796, 21.097, 0, ''),
(17877, 65, 299.359, 8152.583, 18.676, 0, ''),
(17877, 66, 276.115, 8160.440, 17.735, 0, ''),
(17877, 67, 262.704, 8170.509, 17.478, 0, ''),
(17877, 68, 243.755, 8177.747, 17.744, 0, ''),
(17877, 69, 233.496, 8178.426, 17.528, 0, ''),
(17877, 70, 219.874, 8182.550, 19.637, 0, 'SAY_AMBUSH - escort paused'),
(17877, 71, 219.874, 8182.550, 19.637, 20000, 'SAY_AMBUSH_CLEARED'),
(17877, 72, 210.978, 8193.978, 20.777, 0, ''),
(17877, 73, 203.699, 8213.042, 22.768, 0, ''),
(17877, 74, 199.246, 8225.537, 24.847, 0, ''),
(17877, 75, 195.064, 8239.906, 22.640, 0, ''),
(17877, 76, 193.198, 8253.617, 20.083, 0, ''),
(17877, 77, 189.151, 8264.834, 18.714, 0, ''),
(17877, 78, 178.814, 8281.036, 19.070, 0, ''),
(17877, 79, 173.952, 8293.241, 18.533, 0, ''),
(17877, 80, 174.399, 8305.458, 21.006, 0, ''),
(17877, 81, 175.124, 8319.509, 21.626, 0, ''),
(17877, 82, 175.690, 8339.654, 20.375, 0, ''),
(17877, 83, 172.754, 8362.673, 19.181, 0, ''),
(17877, 84, 176.465, 8379.798, 18.445, 0, ''),
(17877, 85, 186.433, 8393.126, 18.933, 0, ''),
(17877, 86, 199.438, 8407.825, 18.763, 0, ''),
(17877, 87, 211.874, 8422.383, 18.785, 0, ''),
(17877, 88, 219.900, 8436.264, 21.927, 0, ''),
(17877, 89, 225.062, 8450.565, 22.832, 0, ''),
(17877, 90, 226.942, 8464.410, 19.822, 0, ''),
(17877, 91, 231.403, 8479.940, 17.928, 0, ''),
(17877, 92, 247.625, 8483.801, 22.464, 13000, ''),
(17877, 93, 231.403, 8479.940, 17.928, 10000, 'SAY_ESCORT_COMPLETE');

DELETE FROM script_waypoint WHERE entry=23383;
INSERT INTO script_waypoint VALUES
(23383, 0, -4109.424, 3034.155, 344.168, 5000, 'SAY_ESCORT_START'),
(23383, 1, -4113.265, 3035.989, 344.071, 0, ''),
(23383, 2, -4120.018, 3032.223, 344.074, 0, ''),
(23383, 3, -4124.412, 3026.332, 344.151, 0, ''),
(23383, 4, -4128.823, 3026.645, 344.035, 0, ''),
(23383, 5, -4138.909, 3028.952, 338.920, 0, ''),
(23383, 6, -4152.592, 3031.234, 336.913, 0, ''),
(23383, 7, -4169.812, 3034.305, 342.047, 0, ''),
(23383, 8, -4174.631, 3036.044, 343.457, 0, ''),
(23383, 9, -4174.399, 3044.983, 343.862, 0, ''),
(23383, 10, -4176.635, 3052.014, 344.077, 0, ''),
(23383, 11, -4183.662, 3058.895, 344.150, 0, ''),
(23383, 12, -4182.916, 3065.411, 342.574, 0, ''),
(23383, 13, -4182.055, 3070.558, 337.644, 5000, 'ambush'),
(23383, 14, -4182.055, 3070.558, 337.644, 5000, 'SAY_AMBUSH_END'),
(23383, 15, -4181.256, 3077.131, 331.590, 0, ''),
(23383, 16, -4179.994, 3086.101, 325.571, 0, ''),
(23383, 17, -4178.770, 3090.101, 323.955, 0, ''),
(23383, 18, -4177.965, 3093.867, 323.839, 5000, 'SAY_ESCORT_COMPLETE'),
(23383, 19, -4166.252, 3106.508, 320.961, 0, '');

DELETE FROM script_waypoint WHERE entry=25589;
INSERT INTO script_waypoint VALUES
(25589, 0, 4414.220, 5367.299, -15.494, 13000, 'SAY_BONKER_START'),
(25589, 1, 4414.220, 5367.299, -15.494, 0, 'SAY_BONKER_GO'),
(25589, 2, 4429.033, 5366.662, -17.198, 0, ''),
(25589, 3, 4454.772, 5371.562, -16.385, 10000, 'SAY_BONKER_LEFT'),
(25589, 4, 4467.889, 5372.425, -15.236, 0, ''),
(25589, 5, 4481.388, 5378.616, -14.997, 0, ''),
(25589, 6, 4484.985, 5392.241, -15.310, 0, ''),
(25589, 7, 4473.114, 5414.899, -15.272, 0, ''),
(25589, 8, 4461.070, 5427.644, -16.163, 0, ''),
(25589, 9, 4441.339, 5435.530, -15.367, 0, ''),
(25589, 10, 4427.119, 5436.604, -15.149 , 0, ''),
(25589, 11, 4408.939, 5428.320, -14.629, 0, ''),
(25589, 12, 4396.607, 5415.876, -13.552, 0, ''),
(25589, 13, 4392.921, 5405.893, -10.506, 0, ''),
(25589, 14, 4390.492, 5390.298, -5.628, 0, ''),
(25589, 15, 4393.429, 5358.273, 2.967, 0, ''),
(25589, 16, 4400.138, 5345.599, 4.656, 0, ''),
(25589, 17, 4412.080, 5336.678, 7.272, 0, ''),
(25589, 18, 4436.494, 5335.233, 12.415, 0, ''),
(25589, 19, 4454.602, 5341.273, 15.560, 0, ''),
(25589, 20, 4471.045, 5352.314, 18.686, 0, ''),
(25589, 21, 4478.235, 5367.257, 20.225, 0, ''),
(25589, 22, 4481.352, 5387.544, 24.537, 0, ''),
(25589, 23, 4483.067, 5405.131, 27.576, 0, ''),
(25589, 24, 4475.878, 5414.829, 29.965, 0, ''),
(25589, 25, 4466.598, 5423.731, 32.224, 0, ''),
(25589, 26, 4451.211, 5431.026, 36.189, 0, ''),
(25589, 27, 4428.056, 5434.374, 38.946, 0, ''),
(25589, 28, 4398.915, 5443.864, 44.214, 0, ''),
(25589, 29, 4386.822, 5451.893, 48.935, 0, ''),
(25589, 30, 4379.861, 5457.215, 51.371, 0, ''),
(25589, 31, 4372.712, 5461.347, 48.541, 0, ''),
(25589, 32, 4364.523, 5465.798, 48.661, 10000, 'SAY_BONKER_COMPLETE'),
(25589, 33, 4337.198, 5472.948, 46.035, 0, '');

DELETE FROM script_waypoint WHERE entry=31737;
INSERT INTO script_waypoint VALUES
(31737, 0, 7269.769, 1509.434, 320.903, 0, ''),
(31737, 1, 7258.117, 1526.602, 324.304, 0, ''),
(31737, 2, 7260.972, 1549.837, 335.689, 1000, 'SAY_ALLIANCE_RUN'),
(31737, 3, 7264.854, 1564.689, 341.974, 0, ''),
(31737, 4, 7255.504, 1579.524, 351.389, 0, ''),
(31737, 5, 7246.569, 1583.333, 358.133, 0, ''),
(31737, 6, 7232.839, 1581.032, 367.501, 0, 'first attack'),
(31737, 7, 7223.732, 1580.088, 373.346, 0, ''),
(31737, 8, 7218.684, 1586.349, 377.490, 0, ''),
(31737, 9, 7217.367, 1593.943, 379.455, 0, ''),
(31737, 10, 7225.456, 1598.870, 379.647, 0, ''),
(31737, 11, 7237.810, 1601.123, 381.088, 0, ''),
(31737, 12, 7251.413, 1609.023, 383.766, 0, ''),
(31737, 13, 7265.517, 1611.843, 382.620, 0, ''),
(31737, 14, 7277.738, 1609.804, 383.899, 0, ''),
(31737, 15, 7290.876, 1608.956, 390.451, 0, 'second attack'),
(31737, 16, 7310.857, 1615.485, 400.580, 0, ''),
(31737, 17, 7327.588, 1622.280, 411.449, 0, ''),
(31737, 18, 7343.151, 1629.884, 423.033, 0, ''),
(31737, 19, 7347.384, 1636.286, 428.066, 0, ''),
(31737, 20, 7343.727, 1644.666, 430.427, 8000, 'SAY_ALLIANCE_BREAK'),
(31737, 21, 7343.727, 1644.666, 430.427, 1000, 'SAY_ALLIANCE_BREAK_DONE'),
(31737, 22, 7301.614, 1649.022, 434.578, 0, ''),
(31737, 23, 7291.128, 1653.633, 435.176, 0, ''),
(31737, 24, 7278.780, 1657.080, 434.619, 0, ''),
(31737, 25, 7259.066, 1651.533, 433.942, 0, 'gate attack'),
(31737, 26, 7243.214, 1662.610, 438.890, 0, ''),
(31737, 27, 7211.633, 1684.327, 462.316, 0, 'SAY_EVENT_COMPLETE');

DELETE FROM script_waypoint WHERE entry=31833;
INSERT INTO script_waypoint VALUES
(31833, 0, 7504.983, 1806.833, 355.928, 0, ''),
(31833, 1, 7500.186, 1817.217, 355.494, 0, ''),
(31833, 2, 7492.701, 1828.367, 361.420, 1000, 'SAY_HORDER_RUN'),
(31833, 3, 7481.528, 1836.774, 370.704, 0, ''),
(31833, 4, 7463.597, 1840.573, 383.662, 0, 'first attack'),
(31833, 5, 7449.448, 1839.822, 394.694, 0, ''),
(31833, 6, 7432.161, 1847.350, 406.290, 0, ''),
(31833, 7, 7415.067, 1845.623, 419.790, 0, ''),
(31833, 8, 7409.832, 1839.991, 423.997, 0, ''),
(31833, 9, 7403.585, 1822.599, 428.435, 0, 'second attack'),
(31833, 10, 7398.860, 1810.257, 430.373, 0, ''),
(31833, 11, 7396.572, 1789.399, 432.286, 0, ''),
(31833, 12, 7397.816, 1769.238, 432.947, 0, ''),
(31833, 13, 7399.105, 1745.266, 433.108, 8000, 'SAY_HORDE_BREAK'),
(31833, 14, 7399.105, 1745.266, 433.108, 1000, 'SAY_HORDE_BREAK_DONE'),
(31833, 15, 7393.293, 1729.907, 435.058, 0, ''),
(31833, 16, 7385.299, 1720.183, 437.602, 0, ''),
(31833, 17, 7370.189, 1715.580, 442.425, 0, ''),
(31833, 18, 7358.270, 1719.352, 446.378, 0, ''),
(31833, 19, 7348.808, 1723.011, 449.727, 0, ''),
(31833, 20, 7333.273, 1724.842, 453.621, 0, ''),
(31833, 21, 7325.701, 1725.662, 456.896, 0, ''),
(31833, 22, 7319.808, 1725.676, 459.731, 0, 'gate attack'),
(31833, 23, 7308.107, 1726.708, 465.138, 0, ''),
(31833, 24, 7297.754, 1727.792, 467.980, 0, ''),
(31833, 25, 7288.278, 1726.889, 469.816, 0, ''),
(31833, 26, 7278.187, 1722.632, 472.149, 0, ''),
(31833, 27, 7253.084, 1729.579, 474.225, 0, 'SAY_EVENT_COMPLETE');

DELETE FROM script_waypoint WHERE entry=31279;
INSERT INTO script_waypoint VALUES
(31279, 0, 6717.810, 3451.979, 683.747, 5000, 'SAY_ESCORT_START_1'),
(31279, 1, 6717.810, 3451.979, 683.747, 2000, 'SAY_ESCORT_START_2'),
(31279, 2, 6718.854, 3436.952, 682.197, 0, ''),
(31279, 3, 6725.714, 3432.644, 682.197, 0, ''),
(31279, 4, 6733.117, 3435.033, 682.136, 0, ''),
(31279, 5, 6744.931, 3445.788, 679.032, 0, ''),
(31279, 6, 6760.190, 3459.459, 674.487, 0, ''),
(31279, 7, 6773.156, 3469.683, 673.155, 0, ''),
(31279, 8, 6783.855, 3480.482, 674.481, 0, ''),
(31279, 9, 6790.618, 3484.064, 676.671, 0, ''),
(31279, 10, 6805.924, 3483.840, 682.128, 0, ''),
(31279, 11, 6818.427, 3483.294, 686.889, 0, ''),
(31279, 12, 6832.831, 3480.982, 690.189, 0, ''),
(31279, 13, 6854.910, 3479.888, 693.181, 0, ''),
(31279, 14, 6873.589, 3478.932, 694.618, 0, ''),
(31279, 15, 6895.129, 3478.388, 698.266, 0, ''),
(31279, 16, 6916.835, 3478.487, 702.575, 0, ''),
(31279, 17, 6937.283, 3477.337, 707.257, 0, ''),
(31279, 18, 6959.092, 3472.777, 710.180, 0, ''),
(31279, 19, 6969.530, 3470.091, 710.401, 0, ''),
(31279, 20, 6980.068, 3466.872, 710.831, 0, ''),
(31279, 21, 7008.199, 3457.296, 696.672, 0, ''),
(31279, 22, 7020.182, 3452.484, 696.518, 0, ''),
(31279, 23, 7031.362, 3445.230, 696.108, 3000, 'SAY_KAMAROS_COMPLETE_1'),
(31279, 24, 7031.362, 3445.230, 696.108, 7000, 'SAY_KAMAROS_COMPLETE_2'),
(31279, 25, 7067.656, 3420.741, 694.879, 0, '');

DELETE FROM script_waypoint WHERE entry=32800;
INSERT INTO script_waypoint VALUES
(32800, 0, 6736.090, 3422.160, 683.457, 5000, 'SAY_ESCORT_START_1'),
(32800, 1, 6736.090, 3422.160, 683.457, 2000, 'SAY_ESCORT_START_2'),
(32800, 2, 6734.518, 3425.644, 682.517, 0, ''),
(32800, 3, 6733.167, 3430.796, 682.156, 0, ''),
(32800, 4, 6733.117, 3435.033, 682.136, 0, ''),
(32800, 5, 6744.931, 3445.788, 679.032, 0, ''),
(32800, 6, 6760.190, 3459.459, 674.487, 0, ''),
(32800, 7, 6773.156, 3469.683, 673.155, 0, ''),
(32800, 8, 6783.855, 3480.482, 674.481, 0, ''),
(32800, 9, 6790.618, 3484.064, 676.671, 0, ''),
(32800, 10, 6805.924, 3483.840, 682.128, 0, ''),
(32800, 11, 6818.427, 3483.294, 686.889, 0, ''),
(32800, 12, 6832.831, 3480.982, 690.189, 0, ''),
(32800, 13, 6854.910, 3479.888, 693.181, 0, ''),
(32800, 14, 6873.589, 3478.932, 694.618, 0, ''),
(32800, 15, 6895.129, 3478.388, 698.266, 0, ''),
(32800, 16, 6916.835, 3478.487, 702.575, 0, ''),
(32800, 17, 6937.283, 3477.337, 707.257, 0, ''),
(32800, 18, 6959.092, 3472.777, 710.180, 0, ''),
(32800, 19, 6969.530, 3470.091, 710.401, 0, ''),
(32800, 20, 6980.068, 3466.872, 710.831, 0, ''),
(32800, 21, 7008.199, 3457.296, 696.672, 0, ''),
(32800, 22, 7020.182, 3452.484, 696.518, 0, ''),
(32800, 23, 7031.362, 3445.230, 696.108, 3000, 'SAY_KAMAROS_COMPLETE_1'),
(32800, 24, 7031.362, 3445.230, 696.108, 7000, 'SAY_KAMAROS_COMPLETE_2'),
(32800, 25, 7067.656, 3420.741, 694.879, 0, '');

DELETE FROM script_waypoint WHERE entry=29434;
INSERT INTO script_waypoint VALUES
(29434, 0, 6643.662, -1258.140, 396.812, 0, 'SAY_ESCORT_READY'),
(29434, 1, 6669.843, -1261.131, 396.362, 0, ''),
(29434, 2, 6672.479, -1244.102, 396.644, 0, ''),
(29434, 3, 6665.353, -1229.893, 399.214, 0, ''),
(29434, 4, 6656.884, -1210.856, 399.819, 0, ''),
(29434, 5, 6658.687, -1187.532, 398.761, 0, ''),
(29434, 6, 6664.340, -1166.372, 398.633, 0, ''),
(29434, 7, 6667.770, -1157.029, 398.136, 0, ''),
(29434, 8, 6670.005, -1145.671, 398.019, 0, ''),
(29434, 9, 6678.494, -1120.105, 397.160, 0, ''),
(29434, 10, 6685.051, -1100.975, 396.287, 0, ''),
(29434, 11, 6682.745, -1087.736, 396.795, 0, ''),
(29434, 12, 6679.602, -1073.343, 404.633, 0, ''),
(29434, 13, 6680.316, -1066.258, 405.499, 0, ''),
(29434, 14, 6689.714, -1053.830, 407.333, 0, ''),
(29434, 15, 6696.244, -1043.514, 411.230, 0, ''),
(29434, 16, 6695.093, -1032.211, 414.625, 0, ''),
(29434, 17, 6690.720, -1016.449, 414.825, 0, ''),
(29434, 18, 6679.976, -1009.805, 414.836, 0, ''),
(29434, 19, 6664.816, -1009.983, 414.840, 0, ''),
(29434, 20, 6647.982, -1010.354, 418.831, 0, ''),
(29434, 21, 6635.366, -1010.637, 423.007, 0, ''),
(29434, 22, 6615.762, -1001.898, 426.584, 0, ''),
(29434, 23, 6597.334, -1002.802, 429.766, 0, ''),
(29434, 24, 6581.178, -1009.971, 433.705, 0, ''),
(29434, 25, 6562.826, -1016.122, 433.558, 0, ''),
(29434, 26, 6535.386, -1024.189, 433.084, 0, ''),
(29434, 27, 6520.094, -1030.279, 433.506, 0, ''),
(29434, 28, 6505.704, -1028.766, 436.897, 0, ''),
(29434, 29, 6496.504, -1027.350, 437.309, 0, ''),
(29434, 30, 6489.653, -1026.457, 434.885, 0, ''),
(29434, 31, 6474.284, -1024.466, 434.650, 0, ''),
(29434, 32, 6456.688, -1022.172, 432.239, 0, ''),
(29434, 33, 6449.764, -1021.355, 431.501, 6000, 'SAY_ESCORT_COMPLETE'),
(29434, 34, 6418.638, -1018.385, 427.910, 0, 'despawn'),
(29434, 35, 6639.769, -1109.591, 427.193, 0, ''),
(29434, 36, 6641.524, -1104.348, 426.970, 0, ''),
(29434, 37, 6659.703, -1106.495, 423.005, 0, ''),
(29434, 38, 6670.649, -1118.345, 424.474, 0, ''),
(29434, 39, 6666.202, -1130.105, 423.113, 0, ''),
(29434, 40, 6642.683, -1129.107, 416.779, 0, ''),
(29434, 41, 6628.478, -1127.415, 414.923, 0, ''),
(29434, 42, 6619.763, -1113.337, 412.185, 0, ''),
(29434, 43, 6622.960, -1101.692, 409.846, 0, ''),
(29434, 44, 6640.454, -1088.525, 403.227, 0, ''),
(29434, 45, 6659.586, -1073.823, 402.945, 0, ''),
(29434, 46, 6671.060, -1064.829, 405.381, 0, 'continue at wp 13');

DELETE FROM script_waypoint WHERE entry=26814;
INSERT INTO script_waypoint VALUES
(26814, 0, 4905.259, -4758.709, 27.316, 2000, 'open cage - SAY_ESCORT_START'),
(26814, 1, 4895.403, -4754.880, 27.233, 0, ''),
(26814, 2, 4887.629, -4761.870, 27.233, 0, ''),
(26814, 3, 4881.628, -4768.923, 32.142, 0, ''),
(26814, 4, 4878.448, -4772.853, 32.646, 0, ''),
(26814, 5, 4876.892, -4787.923, 32.531, 0, ''),
(26814, 6, 4877.230, -4792.542, 32.532, 0, ''),
(26814, 7, 4878.416, -4793.893, 32.549, 5000, 'SAY_CHAMBER_1'),
(26814, 8, 4878.416, -4793.893, 32.549, 5000, 'SAY_CHAMBER_2'),
(26814, 9, 4883.791, -4796.650, 32.575, 0, ''),
(26814, 10, 4908.433, -4797.975, 32.514, 4000, 'open cage'),
(26814, 11, 4908.433, -4797.975, 32.514, 3000, 'SAY_CHAMBER_RELEASE'),
(26814, 12, 4908.433, -4797.975, 32.514, 2000, 'SAY_THANK_YOU'),
(26814, 13, 4908.678, -4806.945, 32.283, 0, ''),
(26814, 14, 4911.196, -4817.785, 32.491, 0, ''),
(26814, 15, 4914.571, -4823.823, 32.666, 3000, ''),
(26814, 16, 4914.571, -4823.823, 32.666, 7000, 'bang gong'),
(26814, 17, 4908.558, -4820.374, 32.550, 5000, 'SAY_CHAMBER_3'),
(26814, 18, 4908.558, -4820.374, 32.550, 0, 'SAY_CHAMBER_4'),
(26814, 19, 4899.099, -4816.810, 32.029, 0, ''),
(26814, 20, 4891.287, -4813.185, 32.029, 0, ''),
(26814, 21, 4886.007, -4803.263, 32.029, 0, 'close door'),
(26814, 22, 4883.618, -4799.119, 32.556, 1000, 'SAY_CHAMBER_5 - set run'),
(26814, 23, 4900.580, -4806.635, 32.029, 7000, 'SAY_CHAMBER_6'),
(26814, 24, 4900.580, -4806.635, 32.029, 6000, 'SAY_CHAMBER_7'),
(26814, 25, 4900.580, -4806.635, 32.029, 0, 'snake attack'),
(26814, 26, 4886.463, -4799.330, 32.552, 0, ''),
(26814, 27, 4862.184, -4782.641, 32.605, 0, ''),
(26814, 28, 4843.930, -4771.764, 32.602, 0, ''),
(26814, 29, 4831.872, -4775.357, 32.581, 0, ''),
(26814, 30, 4819.254, -4788.892, 25.473, 0, ''),
(26814, 31, 4814.696, -4798.355, 25.483, 0, ''),
(26814, 32, 4824.520, -4822.539, 25.492, 0, ''),
(26814, 33, 4826.834, -4838.310, 25.511, 0, ''),
(26814, 34, 4822.480, -4846.951, 25.473, 0, ''),
(26814, 35, 4812.121, -4852.343, 25.622, 0, ''),
(26814, 36, 4779.916, -4848.937, 25.442, 0, ''),
(26814, 37, 4770.701, -4848.962, 25.428, 0, ''),
(26814, 38, 4758.476, -4857.186, 25.848, 0, ''),
(26814, 39, 4737.023, -4857.752, 26.292, 0, ''),
(26814, 40, 4722.875, -4857.749, 26.495, 0, ''),
(26814, 41, 4715.862, -4857.869, 24.707, 0, ''),
(26814, 42, 4705.447, -4858.532, 28.910, 0, ''),
(26814, 43, 4691.578, -4858.917, 33.103, 0, ''),
(26814, 44, 4681.879, -4860.041, 35.440, 0, ''),
(26814, 45, 4670.293, -4861.545, 35.480, 0, ''),
(26814, 46, 4667.317, -4878.836, 35.480, 0, ''),
(26814, 47, 4661.148, -4895.541, 35.499, 0, ''),
(26814, 48, 4656.874, -4907.395, 38.980, 0, ''),
(26814, 49, 4656.184, -4916.478, 44.398, 0, ''),
(26814, 50, 4656.566, -4927.874, 47.576, 0, ''),
(26814, 51, 4660.753, -4938.885, 47.992, 0, ''),
(26814, 52, 4667.464, -4954.763, 47.993, 0, ''),
(26814, 53, 4673.411, -4967.304, 47.791, 3000, 'SAY_ESCORT_COMPLETE'),
(26814, 54, 4694.427, -4979.960, 44.715, 0, '');

DELETE FROM script_waypoint WHERE entry=23784;
INSERT INTO script_waypoint VALUES
(23784, 0, 1377.875, -6421.482, 1.323, 0, 'SAY_ESCORT_START'),
(23784, 1, 1377.523, -6415.196, 1.515, 0, ''),
(23784, 2, 1379.988, -6401.920, 2.428, 8000, 'SAY_FIRE_1'),
(23784, 3, 1379.988, -6401.920, 2.428, 5000, 'SAY_FIRE_2'),
(23784, 4, 1379.749, -6398.577, 2.829, 0, ''),
(23784, 5, 1383.767, -6392.131, 3.639, 0, ''),
(23784, 6, 1395.301, -6381.135, 4.711, 0, ''),
(23784, 7, 1407.236, -6372.452, 6.434, 0, ''),
(23784, 8, 1421.052, -6363.196, 6.430, 0, ''),
(23784, 9, 1424.191, -6358.807, 6.443, 0, ''),
(23784, 10, 1422.745, -6350.552, 6.138, 0, ''),
(23784, 11, 1419.152, -6342.663, 5.811, 0, ''),
(23784, 12, 1414.308, -6336.418, 5.865, 0, ''),
(23784, 13, 1405.468, -6336.249, 6.210, 0, ''),
(23784, 14, 1400.868, -6340.454, 6.415, 4000, 'set fire'),
(23784, 15, 1400.868, -6340.454, 6.415, 15000, 'SAY_SUPPLIES_1'),
(23784, 16, 1406.004, -6335.554, 6.190, 0, ''),
(23784, 17, 1421.080, -6337.905, 5.517, 0, ''),
(23784, 18, 1436.049, -6341.191, 6.772, 0, ''),
(23784, 19, 1449.407, -6344.460, 8.267, 0, ''),
(23784, 20, 1465.833, -6345.101, 7.695, 2000, 'set fire'),
(23784, 21, 1470.890, -6347.974, 7.576, 3000, 'set fire'),
(23784, 22, 1470.890, -6347.974, 7.576, 4000, 'SAY_SUPPLIES_2'),
(23784, 23, 1464.277, -6345.285, 7.896, 0, ''),
(23784, 24, 1463.023, -6339.777, 7.718, 0, ''),
(23784, 25, 1465.487, -6335.771, 7.332, 0, ''),
(23784, 26, 1479.166, -6325.064, 7.440, 0, ''),
(23784, 27, 1489.401, -6315.133, 8.296, 0, ''),
(23784, 28, 1502.828, -6311.045, 6.770, 0, ''),
(23784, 29, 1506.398, -6317.246, 7.299, 4000, 'set fire'),
(23784, 30, 1506.398, -6317.246, 7.299, 2000, 'laugh'),
(23784, 31, 1506.398, -6317.246, 7.299, 10000, 'SAY_SUPPLIES_COMPLETE'),
(23784, 32, 1506.398, -6317.246, 7.299, 5000, 'SAY_SUPPLIES_ESCAPE'),
(23784, 33, 1511.000, -6295.903, 6.193, 0, ''),
(23784, 34, 1517.061, -6275.862, 5.202, 0, ''),
(23784, 35, 1523.781, -6258.195, 4.561, 0, ''),
(23784, 36, 1529.622, -6244.452, 5.823, 0, ''),
(23784, 37, 1537.658, -6224.802, 6.349, 0, ''),
(23784, 38, 1545.301, -6214.430, 6.917, 0, ''),
(23784, 39, 1556.078, -6203.805, 6.566, 0, ''),
(23784, 40, 1567.203, -6194.417, 7.262, 0, 'SAY_ARRIVE_BASE'),
(23784, 41, 1582.464, -6183.626, 7.145, 0, ''),
(23784, 42, 1593.279, -6173.173, 7.319, 0, ''),
(23784, 43, 1604.470, -6164.387, 8.379, 0, ''),
(23784, 44, 1617.776, -6157.249, 9.323, 2000, 'quest complete'),
(23784, 45, 1644.696, -6149.582, 7.357, 0, '');

DELETE FROM script_waypoint WHERE entry=1842;
INSERT INTO script_waypoint VALUES
(1842, 0, 2941.748, -1391.816, 167.237, 0, 'SAY_ESCORT_START'),
(1842, 1, 2940.561, -1393.641, 165.943, 0, ''),
(1842, 2, 2932.194, -1410.657, 165.943, 0, ''),
(1842, 3, 2921.808, -1405.087, 165.943, 0, ''),
(1842, 4, 2916.479, -1402.582, 165.943, 0, ''),
(1842, 5, 2918.523, -1398.121, 165.943, 0, ''),
(1842, 6, 2922.801, -1389.494, 160.842, 0, ''),
(1842, 7, 2924.931, -1385.645, 160.842, 0, ''),
(1842, 8, 2930.931, -1388.654, 160.842, 0, ''),
(1842, 9, 2946.701, -1396.646, 160.842, 0, ''),
(1842, 10, 2948.721, -1392.789, 160.842, 0, ''),
(1842, 11, 2951.979, -1386.616, 155.948, 0, ''),
(1842, 12, 2953.836, -1383.326, 155.948, 0, ''),
(1842, 13, 2951.192, -1381.740, 155.948, 0, ''),
(1842, 14, 2946.675, -1379.287, 152.020, 0, ''),
(1842, 15, 2942.795, -1377.661, 152.020, 0, ''),
(1842, 16, 2935.488, -1392.522, 152.020, 0, ''),
(1842, 17, 2921.167, -1384.796, 152.020, 0, ''),
(1842, 18, 2915.331, -1395.354, 152.020, 0, ''),
(1842, 19, 2926.250, -1401.263, 152.028, 0, ''),
(1842, 20, 2930.321, -1403.479, 150.521, 0, ''),
(1842, 21, 2933.936, -1405.357, 150.521, 0, ''),
(1842, 22, 2929.221, -1415.786, 150.504, 0, ''),
(1842, 23, 2921.173, -1431.680, 150.781, 0, ''),
(1842, 24, 2917.470, -1438.781, 150.781, 0, ''),
(1842, 25, 2913.048, -1453.524, 148.098, 0, 'SAY_TAELAN_MOUNT'),
(1842, 26, 2913.832, -1474.930, 146.224, 0, ''),
(1842, 27, 2906.815, -1487.061, 146.224, 0, ''),
(1842, 28, 2900.644, -1496.575, 146.306, 0, ''),
(1842, 29, 2885.249, -1501.585, 146.020, 0, ''),
(1842, 30, 2863.877, -1500.380, 146.681, 0, ''),
(1842, 31, 2846.509, -1487.183, 146.332, 0, ''),
(1842, 32, 2823.752, -1490.987, 145.782, 0, ''),
(1842, 33, 2800.984, -1510.907, 145.049, 0, ''),
(1842, 34, 2789.488, -1525.215, 143.729, 0, ''),
(1842, 35, 2776.964, -1542.305, 139.435, 0, ''),
(1842, 36, 2762.032, -1561.804, 133.763, 0, ''),
(1842, 37, 2758.741, -1569.599, 131.514, 0, ''),
(1842, 38, 2765.488, -1588.793, 129.721, 0, ''),
(1842, 39, 2779.613, -1613.120, 129.132, 0, ''),
(1842, 40, 2757.654, -1638.032, 128.236, 0, ''),
(1842, 41, 2741.308, -1659.790, 126.457, 0, ''),
(1842, 42, 2729.797, -1677.571, 126.499, 0, ''),
(1842, 43, 2716.778, -1694.648, 126.301, 0, ''),
(1842, 44, 2706.658, -1709.474, 123.420, 0, ''),
(1842, 45, 2699.506, -1720.572, 120.265, 0, ''),
(1842, 46, 2691.977, -1738.466, 114.994, 0, ''),
(1842, 47, 2690.514, -1757.045, 108.764, 0, ''),
(1842, 48, 2691.953, -1780.309, 99.890, 0, ''),
(1842, 49, 2689.344, -1803.264, 89.130, 0, ''),
(1842, 50, 2697.849, -1820.550, 80.681, 0, ''),
(1842, 51, 2701.934, -1836.706, 73.700, 0, ''),
(1842, 52, 2698.088, -1853.866, 68.999, 0, ''),
(1842, 53, 2693.657, -1870.237, 66.882, 0, ''),
(1842, 54, 2682.347, -1885.251, 66.009, 0, ''),
(1842, 55, 2668.229, -1900.796, 66.256, 0, 'SAY_REACH_TOWER - escort paused');

DELETE FROM script_waypoint WHERE entry=1840;
INSERT INTO script_waypoint VALUES
(1840, 0, 2689.677, -1937.474, 72.14, 0, ''),
(1840, 1, 2683.112, -1926.823, 72.14, 0, ''),
(1840, 2, 2678.725, -1919.416, 68.86, 0, 'escort paused');

DELETE FROM script_waypoint WHERE entry=12126;
INSERT INTO script_waypoint VALUES
(12126, 0, 2631.229, -1917.927, 72.59, 0, ''),
(12126, 1, 2643.529, -1914.072, 71.00, 0, ''),
(12126, 2, 2653.827, -1907.536, 69.34, 0, 'escort paused');

DELETE FROM script_waypoint WHERE entry=11016;
INSERT INTO script_waypoint VALUES
(11016, 0, 5004.985, -440.237, 319.059, 4000, 'SAY_ESCORT_START'),
(11016, 1, 4992.224, -449.964, 317.057, 0, ''),
(11016, 2, 4988.549, -457.438, 316.289, 0, ''),
(11016, 3, 4989.978, -464.297, 316.846, 0, ''),
(11016, 4, 4994.038, -467.754, 318.055, 0, ''),
(11016, 5, 5002.307, -466.318, 319.965, 0, ''),
(11016, 6, 5011.801, -462.889, 321.501, 0, ''),
(11016, 7, 5020.533, -460.797, 321.970, 0, ''),
(11016, 8, 5026.836, -463.171, 321.345, 0, ''),
(11016, 9, 5028.663, -476.805, 318.726, 0, ''),
(11016, 10, 5029.503, -487.131, 318.179, 0, ''),
(11016, 11, 5031.178, -497.678, 316.533, 0, ''),
(11016, 12, 5032.720, -504.748, 314.744, 0, ''),
(11016, 13, 5034.997, -513.138, 314.372, 0, ''),
(11016, 14, 5037.493, -521.733, 313.221, 6000, 'SAY_FIRST_STOP'),
(11016, 15, 5049.055, -519.546, 313.221, 0, ''),
(11016, 16, 5059.170, -522.930, 313.221, 0, ''),
(11016, 17, 5062.755, -529.933, 313.221, 0, ''),
(11016, 18, 5063.896, -538.827, 313.221, 0, ''),
(11016, 19, 5062.223, -545.635, 313.221, 0, ''),
(11016, 20, 5061.690, -552.333, 313.101, 0, ''),
(11016, 21, 5060.333, -560.349, 310.873, 0, ''),
(11016, 22, 5055.621, -565.541, 308.737, 0, ''),
(11016, 23, 5049.803, -567.604, 306.537, 0, ''),
(11016, 24, 5043.011, -564.946, 303.682, 0, ''),
(11016, 25, 5038.221, -559.823, 301.463, 0, ''),
(11016, 26, 5039.456, -548.675, 297.824, 0, ''),
(11016, 27, 5043.437, -538.807, 297.801, 0, ''),
(11016, 28, 5056.397, -528.954, 297.801, 0, ''),
(11016, 29, 5064.397, -521.904, 297.801, 0, ''),
(11016, 30, 5067.616, -512.999, 297.196, 0, ''),
(11016, 31, 5065.990, -505.329, 297.214, 0, ''),
(11016, 32, 5062.238, -499.086, 297.448, 0, ''),
(11016, 33, 5065.087, -492.069, 298.054, 0, ''),
(11016, 34, 5071.195, -491.173, 297.666, 5000, 'SAY_SECOND_STOP'),
(11016, 35, 5087.474, -496.478, 296.677, 0, ''),
(11016, 36, 5095.552, -508.639, 296.677, 0, ''),
(11016, 37, 5104.300, -521.014, 296.677, 0, ''),
(11016, 38, 5110.132, -532.123, 296.677, 4000, 'open equipment chest'),
(11016, 39, 5110.132, -532.123, 296.677, 4000, 'cast SPELL_STRENGHT_ARKONARIN'),
(11016, 40, 5110.132, -532.123, 296.677, 4000, 'SAY_EQUIPMENT'),
(11016, 41, 5110.132, -532.123, 296.677, 0, 'SAY_ESCAPE'),
(11016, 42, 5099.748, -510.823, 296.677, 0, ''),
(11016, 43, 5091.944, -497.516, 296.677, 0, ''),
(11016, 44, 5079.375, -486.811, 297.638, 0, ''),
(11016, 45, 5069.212, -488.770, 298.082, 0, ''),
(11016, 46, 5064.242, -496.051, 297.275, 0, ''),
(11016, 47, 5065.084, -505.239, 297.361, 0, ''),
(11016, 48, 5067.818, -515.245, 297.125, 0, ''),
(11016, 49, 5064.617, -521.170, 297.801, 0, ''),
(11016, 50, 5053.221, -530.739, 297.801, 0, ''),
(11016, 51, 5045.725, -538.311, 297.801, 0, ''),
(11016, 52, 5039.695, -548.112, 297.801, 0, ''),
(11016, 53, 5038.778, -557.588, 300.787, 0, ''),
(11016, 54, 5042.014, -566.749, 303.838, 0, ''),
(11016, 55, 5050.555, -568.149, 306.782, 0, ''),
(11016, 56, 5056.979, -564.674, 309.342, 0, ''),
(11016, 57, 5060.791, -556.801, 311.936, 0, ''),
(11016, 58, 5059.581, -551.626, 313.221, 0, ''),
(11016, 59, 5062.826, -541.994, 313.221, 0, ''),
(11016, 60, 5063.554, -531.288, 313.221, 0, ''),
(11016, 61, 5057.934, -523.088, 313.221, 0, ''),
(11016, 62, 5049.471, -519.360, 313.221, 0, ''),
(11016, 63, 5040.789, -519.809, 313.221, 0, ''),
(11016, 64, 5034.299, -515.361, 313.948, 0, ''),
(11016, 65, 5032.001, -505.532, 314.663, 0, ''),
(11016, 66, 5029.915, -495.645, 316.821, 0, ''),
(11016, 67, 5028.871, -487.000, 318.179, 0, ''),
(11016, 68, 5028.109, -475.531, 318.839, 0, ''),
(11016, 69, 5027.759, -465.442, 320.643, 0, ''),
(11016, 70, 5019.955, -460.892, 321.969, 0, ''),
(11016, 71, 5009.426, -464.793, 321.248, 0, ''),
(11016, 72, 4999.567, -468.062, 319.426, 0, ''),
(11016, 73, 4992.034, -468.128, 317.894, 0, ''),
(11016, 74, 4988.168, -461.293, 316.369, 0, ''),
(11016, 75, 4990.624, -447.459, 317.104, 0, ''),
(11016, 76, 4993.475, -438.643, 318.272, 0, ''),
(11016, 77, 4995.451, -430.178, 318.462, 0, ''),
(11016, 78, 4993.564, -422.876, 318.864, 0, ''),
(11016, 79, 4985.401, -420.864, 320.205, 0, ''),
(11016, 80, 4976.515, -426.168, 323.112, 0, ''),
(11016, 81, 4969.832, -429.755, 325.029, 0, ''),
(11016, 82, 4960.702, -425.440, 325.834, 0, ''),
(11016, 83, 4955.447, -418.765, 327.433, 0, ''),
(11016, 84, 4949.702, -408.796, 328.004, 0, ''),
(11016, 85, 4940.017, -403.222, 329.956, 0, ''),
(11016, 86, 4934.982, -401.475, 330.898, 0, ''),
(11016, 87, 4928.693, -399.302, 331.744, 0, ''),
(11016, 88, 4926.935, -398.436, 333.079, 0, ''),
(11016, 89, 4916.163, -393.822, 333.729, 0, ''),
(11016, 90, 4908.393, -396.217, 333.217, 0, ''),
(11016, 91, 4905.610, -396.535, 335.050, 0, ''),
(11016, 92, 4897.876, -395.245, 337.346, 0, ''),
(11016, 93, 4895.206, -388.203, 339.295, 0, ''),
(11016, 94, 4896.945, -382.429, 341.040, 0, ''),
(11016, 95, 4901.885, -378.799, 342.771, 0, ''),
(11016, 96, 4908.087, -380.635, 344.597, 0, ''),
(11016, 97, 4911.910, -385.818, 346.491, 0, ''),
(11016, 98, 4910.104, -393.444, 348.798, 0, ''),
(11016, 99, 4903.500, -396.947, 350.812, 0, ''),
(11016, 100, 4898.083, -394.226, 351.821, 0, ''),
(11016, 101, 4891.333, -393.436, 351.801, 0, ''),
(11016, 102, 4881.203, -395.211, 351.590, 0, ''),
(11016, 103, 4877.843, -395.536, 349.713, 0, ''),
(11016, 104, 4873.972, -394.919, 349.844, 5000, 'SAY_FRESH_AIR'),
(11016, 105, 4873.972, -394.919, 349.844, 3000, 'SAY_BETRAYER'),
(11016, 106, 4873.972, -394.919, 349.844, 2000, 'SAY_TREY'),
(11016, 107, 4873.972, -394.919, 349.844, 0, 'SAY_ATTACK_TREY'),
(11016, 108, 4873.972, -394.919, 349.844, 5000, 'SAY_ESCORT_COMPLETE'),
(11016, 109, 4873.972, -394.919, 349.844, 1000, ''),
(11016, 110, 4863.016, -394.521, 350.650, 0, ''),
(11016, 111, 4848.696, -397.612, 351.215, 0, '');

DELETE FROM script_waypoint WHERE entry=9598;
INSERT INTO script_waypoint VALUES
(9598, 0, 6004.265, -1180.494, 376.377, 0, 'SAY_ESCORT_START'),
(9598, 1, 6002.512, -1157.294, 381.407, 0, ''),
(9598, 2, 6029.228, -1139.720, 383.127, 0, ''),
(9598, 3, 6042.479, -1128.963, 386.582, 0, ''),
(9598, 4, 6062.820, -1115.522, 386.850, 0, ''),
(9598, 5, 6089.188, -1111.907, 383.105, 0, ''),
(9598, 6, 6104.390, -1114.561, 380.490, 0, ''),
(9598, 7, 6115.080, -1128.572, 375.779, 0, ''),
(9598, 8, 6119.352, -1147.314, 372.518, 0, ''),
(9598, 9, 6119.003, -1176.082, 371.072, 0, ''),
(9598, 10, 6120.982, -1198.408, 373.432, 0, ''),
(9598, 11, 6123.521, -1226.636, 374.119, 0, ''),
(9598, 12, 6127.737, -1246.035, 373.574, 0, ''),
(9598, 13, 6133.433, -1253.642, 369.100, 0, ''),
(9598, 14, 6150.787, -1269.151, 369.240, 0, ''),
(9598, 15, 6155.805, -1275.029, 373.627, 0, ''),
(9598, 16, 6163.544, -1307.130, 376.234, 0, ''),
(9598, 17, 6174.800, -1340.885, 379.039, 0, ''),
(9598, 18, 6191.144, -1371.260, 378.453, 0, ''),
(9598, 19, 6215.652, -1397.517, 376.012, 0, ''),
(9598, 20, 6243.784, -1407.675, 371.594, 0, ''),
(9598, 21, 6280.775, -1408.259, 370.554, 0, ''),
(9598, 22, 6325.360, -1406.688, 370.082, 0, ''),
(9598, 23, 6355.000, -1404.337, 370.646, 0, ''),
(9598, 24, 6374.679, -1399.176, 372.105, 0, ''),
(9598, 25, 6395.803, -1367.057, 374.910, 0, ''),
(9598, 26, 6408.569, -1333.487, 376.616, 0, ''),
(9598, 27, 6409.075, -1312.168, 379.598, 0, ''),
(9598, 28, 6418.689, -1277.697, 381.638, 0, ''),
(9598, 29, 6441.689, -1247.316, 387.246, 0, ''),
(9598, 30, 6462.136, -1226.316, 397.610, 0, ''),
(9598, 31, 6478.021, -1216.260, 408.284, 0, ''),
(9598, 32, 6499.632, -1217.087, 419.461, 0, ''),
(9598, 33, 6523.165, -1220.780, 430.549, 0, ''),
(9598, 34, 6542.716, -1216.997, 437.788, 0, ''),
(9598, 35, 6557.279, -1211.125, 441.452, 0, ''),
(9598, 36, 6574.568, -1204.589, 443.216, 0, 'SAY_EXIT_IRONTREE');

DELETE FROM script_waypoint WHERE entry=26588;
INSERT INTO script_waypoint VALUES
(26588, 0, 4322.890, -3693.610, 263.910, 4000, 'SAY_ESCORT_START'),
(26588, 1, 4330.509, -3689.442, 263.627, 0, ''),
(26588, 2, 4341.477, -3684.207, 257.441, 0, ''),
(26588, 3, 4346.749, -3685.898, 256.866, 0, ''),
(26588, 4, 4347.176, -3694.563, 256.560, 0, ''),
(26588, 5, 4335.924, -3704.452, 258.113, 0, ''),
(26588, 6, 4317.913, -3722.990, 256.835, 0, ''),
(26588, 7, 4309.215, -3736.347, 257.451, 0, ''),
(26588, 8, 4301.650, -3751.553, 257.810, 0, ''),
(26588, 9, 4296.501, -3769.056, 251.977, 0, ''),
(26588, 10, 4291.985, -3785.022, 245.880, 2000, 'SAY_FIRST_WOLF'),
(26588, 11, 4291.985, -3785.022, 245.880, 0, 'SAY_WOLF_ATTACK'),
(26588, 12, 4291.985, -3785.022, 245.880, 3000, ''),
(26588, 13, 4299.542, -3807.021, 237.238, 0, ''),
(26588, 14, 4308.171, -3835.070, 226.317, 0, ''),
(26588, 15, 4312.530, -3847.574, 222.333, 0, ''),
(26588, 16, 4317.506, -3861.733, 214.915, 0, ''),
(26588, 17, 4325.013, -3882.172, 208.888, 0, ''),
(26588, 18, 4332.627, -3893.466, 203.584, 0, ''),
(26588, 19, 4338.521, -3899.447, 199.843, 0, ''),
(26588, 20, 4344.693, -3911.864, 197.886, 0, ''),
(26588, 21, 4349.635, -3922.679, 195.293, 0, ''),
(26588, 22, 4351.970, -3934.677, 191.418, 0, 'SAY_SECOND_WOLF'),
(26588, 23, 4351.970, -3934.677, 191.418, 3000, ''),
(26588, 24, 4351.970, -3934.677, 191.418, 2000, 'SAY_RESUME_ESCORT'),
(26588, 25, 4350.807, -3944.965, 190.528, 0, 'SAY_ESCORT_COMPLETE'),
(26588, 26, 4347.947, -3958.875, 193.360, 0, ''),
(26588, 27, 4345.956, -3988.083, 187.320, 0, '');

DELETE FROM script_waypoint WHERE entry=26499;
INSERT INTO script_waypoint VALUES
(26499, 0, 2366.184, 1197.285, 132.150, 0, ''),
(26499, 1, 2371.608, 1199.006, 134.727, 0, ''),
(26499, 2, 2376.157, 1200.552, 134.042, 0, ''),
(26499, 3, 2391.321, 1203.153, 134.125, 10000, 'SAY_ARRIVED'),
(26499, 4, 2391.321, 1203.153, 134.125, 0, 'SAY_GET_BEFORE_PLAGUE'),
(26499, 5, 2396.739, 1205.993, 134.125, 0, 'escort paused'),
(26499, 6, 2396.739, 1205.993, 134.125, 8000, ''),
(26499, 7, 2396.739, 1205.993, 134.125, 5000, 'SAY_MORE_THAN_SCOURGE'),
(26499, 8, 2412.033, 1207.823, 134.034, 0, ''),
(26499, 9, 2426.958, 1212.363, 134.000, 0, ''),
(26499, 10, 2438.589, 1217.005, 133.957, 0, ''),
(26499, 11, 2441.247, 1215.506, 133.951, 0, ''),
(26499, 12, 2446.155, 1197.135, 148.064, 0, ''),
(26499, 13, 2446.861, 1193.559, 148.076, 0, 'SAY_MORE_SORCERY'),
(26499, 14, 2443.582, 1189.773, 148.076, 0, 'escort paused'),
(26499, 15, 2443.582, 1189.773, 148.076, 8000, ''),
(26499, 16, 2443.582, 1189.773, 148.076, 5000, 'SAY_MOVE_ON'),
(26499, 17, 2430.986, 1193.844, 148.076, 0, ''),
(26499, 18, 2418.701, 1195.074, 148.076, 0, ''),
(26499, 19, 2410.825, 1193.033, 148.076, 0, ''),
(26499, 20, 2405.178, 1177.300, 148.076, 0, ''),
(26499, 21, 2409.676, 1155.144, 148.187, 0, 'SAY_WATCH_BACKS - escort paused'),
(26499, 22, 2409.676, 1155.144, 148.187, 8000, ''),
(26499, 23, 2409.676, 1155.144, 148.187, 3000, 'SAY_NOT_EASY'),
(26499, 24, 2413.030, 1138.769, 148.075, 0, ''),
(26499, 25, 2421.589, 1122.539, 148.125, 0, ''),
(26499, 26, 2425.375, 1119.325, 148.075, 0, 'SAY_PERSISTENT'),
(26499, 27, 2425.375, 1119.325, 148.075, 8000, ''),
(26499, 28, 2425.375, 1119.325, 148.075, 0, 'SAY_ELSE - escort paused'),
(26499, 29, 2447.376, 1114.935, 148.075, 0, ''),
(26499, 30, 2454.853, 1117.053, 150.007, 0, ''),
(26499, 31, 2459.909, 1125.710, 150.007, 0, ''),
(26499, 32, 2468.208, 1124.426, 150.027, 5000, 'SAY_TAKE_A_MOMENT'),
(26499, 33, 2468.208, 1124.426, 150.027, 0, 'SAY_PASSAGE'),
(26499, 34, 2482.697, 1122.354, 149.905, 0, ''),
(26499, 35, 2485.536, 1111.682, 149.907, 0, ''),
(26499, 36, 2486.997, 1103.307, 145.335, 0, ''),
(26499, 37, 2490.222, 1100.452, 144.860, 0, ''),
(26499, 38, 2496.676, 1102.510, 144.474, 0, ''),
(26499, 39, 2495.006, 1115.535, 143.825, 0, ''),
(26499, 40, 2493.206, 1123.732, 140.302, 0, ''),
(26499, 41, 2496.522, 1128.798, 140.010, 0, ''),
(26499, 42, 2500.956, 1127.101, 139.982, 0, ''),
(26499, 43, 2504.459, 1120.400, 139.976, 0, ''),
(26499, 44, 2506.478, 1120.344, 139.970, 0, ''),
(26499, 45, 2517.028, 1122.504, 132.064, 0, ''),
(26499, 46, 2523.487, 1124.808, 132.080, 0, 'encounter complete - despawn'),
(26499, 47, 2551.116, 1135.607, 129.797, 0, ''),
(26499, 48, 2562.692, 1147.900, 128.003, 0, ''),
(26499, 49, 2565.026, 1168.818, 127.007, 0, ''),
(26499, 50, 2562.405, 1189.934, 126.189, 0, ''),
(26499, 51, 2558.311, 1212.633, 125.739, 0, ''),
(26499, 52, 2551.082, 1231.603, 125.554, 0, ''),
(26499, 53, 2543.631, 1250.385, 126.103, 0, ''),
(26499, 54, 2534.270, 1272.281, 126.993, 0, ''),
(26499, 55, 2521.446, 1290.463, 130.194, 0, ''),
(26499, 56, 2517.060, 1312.327, 130.156, 0, ''),
(26499, 57, 2513.198, 1324.149, 131.843, 20000, 'SAY_REST'),
(26499, 58, 2513.198, 1324.149, 131.843, 0, 'SAY_REST_COMPLETE'),
(26499, 59, 2503.484, 1347.347, 132.952, 0, ''),
(26499, 60, 2491.935, 1367.205, 130.717, 0, ''),
(26499, 61, 2482.922, 1386.118, 130.029, 0, ''),
(26499, 62, 2471.576, 1404.726, 130.681, 0, ''),
(26499, 63, 2459.646, 1418.801, 130.662, 0, ''),
(26499, 64, 2440.002, 1423.901, 130.632, 0, ''),
(26499, 65, 2416.750, 1419.929, 130.669, 0, ''),
(26499, 66, 2401.423, 1415.888, 130.840, 0, ''),
(26499, 67, 2381.814, 1410.022, 128.147, 0, ''),
(26499, 68, 2367.663, 1406.689, 128.529, 0, ''),
(26499, 69, 2361.863, 1405.020, 128.714, 0, 'SAY_CRUSADER_SQUARE - escort paused'),
(26499, 70, 2341.932, 1406.359, 128.268, 0, ''),
(26499, 71, 2328.375, 1413.144, 127.687, 0, ''),
(26499, 72, 2319.288, 1435.609, 127.887, 0, ''),
(26499, 73, 2308.846, 1460.503, 127.840, 0, ''),
(26499, 74, 2301.277, 1487.081, 128.361, 0, 'SAY_FINISH_MALGANIS - escort paused'),
(26499, 75, 2301.277, 1487.081, 128.361, 18000, 'SAY_JOURNEY_BEGUN'),
(26499, 76, 2293.693, 1506.805, 128.737, 18000, 'SAY_HUNT_MALGANIS'),
(26499, 77, 2300.743, 1487.231, 128.362, 0, ''),
(26499, 78, 2308.582, 1460.863, 127.839, 0, ''),
(26499, 79, 2326.608, 1420.555, 127.780, 0, '');

DELETE FROM script_waypoint WHERE entry=19671;
INSERT INTO script_waypoint VALUES
(19671, 0, -362.876, -71.333, -0.960, 5000, 'SAY_ESCORT_START'),
(19671, 1, -372.647, -88.728, -0.958, 0, ''),
(19671, 2, -373.163, -114.959, -0.958, 0, ''),
(19671, 3, -373.137, -138.439, -0.958, 0, ''),
(19671, 4, -373.687, -152.964, -0.958, 0, ''),
(19671, 5, -379.091, -158.089, -0.958, 0, ''),
(19671, 6, -381.149, -164.796, -0.958, 0, ''),
(19671, 7, -375.292, -164.111, 0.715, 0, 'SAY_FIRST_STOP - escort paused'),
(19671, 8, -375.292, -164.111, 0.715, 1000, 'SAY_FIRST_STOP_COMPLETE'),
(19671, 9, -381.149, -164.796, -0.958, 0, ''),
(19671, 10, -374.292, -193.614, -0.967, 0, ''),
(19671, 11, -370.799, -217.796, -0.959, 0, ''),
(19671, 12, -344.132, -222.647, -0.959, 0, ''),
(19671, 13, -310.880, -219.357, -0.945, 9000, 'SAY_COLLECTOR_SEARCH'),
(19671, 14, -299.879, -201.809, -1.181, 0, ''),
(19671, 15, -285.874, -187.791, -0.694, 0, ''),
(19671, 16, -271.884, -164.856, -1.912, 0, ''),
(19671, 17, -260.729, -159.094, -1.190, 0, 'SAY_SECOND_STOP - escort paused'),
(19671, 18, -244.372, -163.136, -0.924, 0, ''),
(19671, 19, -236.428, -185.952, -0.953, 0, ''),
(19671, 20, -210.659, -206.710, 0.906, 0, ''),
(19671, 21, -193.375, -219.378, -0.518, 0, ''),
(19671, 22, -171.121, -223.043, -0.955, 0, ''),
(19671, 23, -148.658, -221.031, -0.770, 0, ''),
(19671, 24, -128.150, -215.657, -0.187, 0, ''),
(19671, 25, -93.429, -219.264, -1.320, 0, ''),
(19671, 26, -72.886, -222.278, -0.184, 0, 'SAY_THIRD_STOP - escort paused'),
(19671, 27, -42.343, -224.004, -0.268, 0, ''),
(19671, 28, -15.603, -223.780, 0.793, 0, ''),
(19671, 29, -12.975, -223.157, 1.811, 42000, 'SAY_REST'),
(19671, 30, -12.975, -223.157, 1.811, 5000, 'SAY_BREAK_OVER'),
(19671, 31, -14.898, -222.906, 0.786, 0, ''),
(19671, 32, -44.684, -221.592, -0.146, 0, ''),
(19671, 33, -59.746, -211.847, 0.216, 0, ''),
(19671, 34, -63.100, -195.288, -0.615, 0, ''),
(19671, 35, -63.218, -191.959, -1.725, 0, ''),
(19671, 36, -67.063, -160.281, -0.955, 0, ''),
(19671, 37, -67.888, -128.704, -1.226, 0, ''),
(19671, 38, -68.226, -107.766, -0.289, 0, ''),
(19671, 39, -68.251, -105.624, -1.631, 0, ''),
(19671, 40, -66.342, -88.147, -1.167, 0, 'SAY_XIRAXIS_SPAWN - escort paused'),
(19671, 41, -67.831, -78.571, -2.114, 0, ''),
(19671, 42, -67.692, -76.665, -0.941, 0, ''),
(19671, 43, -67.624, -56.025, -0.844, 0, 'quest complete'),
(19671, 44, -64.493, -15.776, -0.943, 0, '');

DELETE FROM script_waypoint WHERE entry=17982;
INSERT INTO script_waypoint VALUES
(17982, 0, -1778.691, -11063.521, 77.140, 0, ''),
(17982, 1, -1786.793, -11051.682, 77.624, 0, ''),
(17982, 2, -1793.498, -11045.093, 76.965, 0, ''),
(17982, 3, -1793.670, -11038.277, 76.308, 0, ''),
(17982, 4, -1809.280, -11025.772, 68.239, 0, ''),
(17982, 5, -1828.214, -11007.534, 67.414, 0, ''),
(17982, 6, -1848.783, -10995.200, 65.269, 0, ''),
(17982, 7, -1875.192, -10981.417, 61.316, 0, ''),
(17982, 8, -1893.577, -10962.064, 61.258, 0, ''),
(17982, 9, -1912.960, -10935.167, 61.436, 0, ''),
(17982, 10, -1927.932, -10909.572, 62.588, 0, ''),
(17982, 11, -1936.610, -10866.243, 66.684, 0, ''),
(17982, 12, -1939.888, -10854.035, 69.186, 0, ''),
(17982, 13, -1936.704, -10839.362, 73.829, 0, ''),
(17982, 14, -1936.740, -10817.189, 81.554, 0, ''),
(17982, 15, -1942.172, -10803.520, 86.075, 0, ''),
(17982, 16, -1962.055, -10791.647, 88.658, 0, ''),
(17982, 17, -1973.293, -10780.165, 88.985, 0, ''),
(17982, 18, -1976.268, -10763.559, 90.655, 0, ''),
(17982, 19, -1964.070, -10745.006, 95.730, 0, ''),
(17982, 20, -1953.445, -10728.167, 104.113, 0, ''),
(17982, 21, -1950.743, -10710.538, 111.087, 0, ''),
(17982, 22, -1956.330, -10689.001, 110.965, 5000, 'SAY_ESCORT_LEGOSO_1'),
(17982, 23, -1955.620, -10658.600, 110.883, 1000, 'escort paused, SAY_ESCORT_LEGOSO_2, SAY_ESCORT_LEGOSO_3'),
(17982, 24, -1976.113, -10688.756, 112.680, 0, 'SAY_ESCORT_LEGOSO_4'),
(17982, 25, -1985.777, -10699.355, 115.460, 1000, 'escort paused, SAY_ESCORT_COUNT_3'),
(17982, 26, -2000.165, -10676.817, 118.616, 0, ''),
(17982, 27, -2016.455, -10657.827, 125.049, 0, ''),
(17982, 28, -2033.140, -10641.366, 137.672, 0, ''),
(17982, 29, -2039.429, -10628.161, 144.266, 0, ''),
(17982, 30, -2032.072, -10609.500, 148.143, 0, ''),
(17982, 31, -2017.386, -10608.134, 153.232, 0, ''),
(17982, 32, -1989.028, -10613.335, 162.852, 0, ''),
(17982, 33, -1975.880, -10616.800, 163.837, 5000, 'SAY_ESCORT_LEGOSO_5, SAY_ESCORT_LEGOSO_6'),
(17982, 34, -1957.047, -10596.522, 168.726, 0, ''),
(17982, 35, -1944.290, -10580.700, 174.467, 13000, 'SAY_ESCORT_LEGOSO_7, SAY_ESCORT_LEGOSO_8'),
(17982, 36, -1936.099, -10581.618, 176.737, 0, ''),
(17982, 37, -1915.010, -10581.500, 178.105, 1000, 'escort paused, SAY_ESCORT_LEGOSO_9, SAY_ESCORT_LEGOSO_10'),
(17982, 38, -1952.080, -10560.300, 177.657, 4000, 'SAY_ESCORT_COUNT_3, SAY_ESCORT_COUNT_2, SAY_ESCORT_COUNT_1'),
(17982, 39, -1952.080, -10560.300, 177.657, 1000, 'escort paused, SAY_LEGOSO_AGGRO');

DELETE FROM script_waypoint WHERE entry=25504;
INSERT INTO script_waypoint VALUES
(25504, 1, 2886.78, 6732.96, 32.9695, 5000, 'SAY_MOOTOO_Y_START'),
(25504, 2, 2882.26, 6734.51, 32.8864, 0, ''),
(25504, 3, 2877.37, 6731.59, 32.8721, 0, ''),
(25504, 4, 2874.18, 6725.85, 30.3087, 0, 'SAY_1_MOOTOO_Y'),
(25504, 5, 2866.36, 6726.04, 26.7277, 0, ''),
(25504, 6, 2863.97, 6731.91, 23.8372, 0, ''),
(25504, 7, 2869.29, 6736.92, 20.5227, 0, ''),
(25504, 8, 2874.16, 6731.88, 18.5042, 0, ''),
(25504, 9, 2887.05, 6736.39, 13.997, 5000, 'SAY_2_MOOTOO_Y'),
(25504, 10, 2887.05, 6736.39, 13.997, 2000, 'EMOTE_ONESHOT_POINT'),
(25504, 11, 2901.68, 6741.25, 14.0413, 0, ''),
(25504, 12, 2919.45, 6746.28, 13.7325, 0, 'SAY_3_MOOTOO_Y'),
(25504, 13, 2947.18, 6738.71, 12.7117, 0, ''),
(25504, 14, 2982.43, 6748.59, 10.2755, 0, 'SAY_4_MOOTOO_Y'),
(25504, 15, 2985.05, 6776.05, 8.33081, 0, 'SAY_5_MOOTOO_Y'),
(25504, 16, 2978.72, 6801.48, 5.83056, 0, 'SAY_6_MOOTOO_Y'),
(25504, 17, 2957.81, 6818.86, 4.7594, 0, ''),
(25504, 18, 2917.03, 6820.55, 5.87954, 0, 'SAY_4_MOOTOO_Y'),
(25504, 19, 2890.04, 6825.68, 4.11676, 3000, 'SAY_7_MOOTOO_Y'),
(25504, 20, 2850.31, 6812.35, 2.09411, 0, 'SAY_8_MOOTOO_Y'),
(25504, 21, 2813.28, 6793.82, 4.44574, 0, ''),
(25504, 22, 2807.22, 6772.41, 5.80334, 2000, 'SAY_CREDIT_MOOTOO_Y'),
(25504, 23, 2807.38, 6765,    6.30617, 0, 'SAY_1_ELDER_MOOTOO'),
(25504, 24, 2807.49, 6748.29, 8.25933, 3000, 'MOOTOO_Y_SAY_9'),
(25504, 25, 2807.49, 6748.29, 8.25933, 5000, 'SAY_2_ELDER_MOOTOO');

-- EOF

DROP TABLE IF EXISTS spell_scripts;
CREATE TABLE spell_scripts(
Id INT NOT NULL COMMENT 'Spell ID',
ScriptName CHAR(64) NOT NULL COMMENT 'Core Spell Script Name',
PRIMARY KEY(Id)
);

-- Classic
INSERT INTO spell_scripts(Id, ScriptName) VALUES
(720,'spell_entangle_fankriss'),
(731,'spell_entangle_fankriss'),
(785,'spell_true_fulfillment'),
(794,'spell_initialize_images'),
(802,'spell_mutate_bug'),
(1121,'spell_entangle_fankriss'),
(3730,'spell_initialize_image'),
(7054,'spell_forsaken_skill'),
(13278,'spell_gdr_channel'),
(13493,'spell_gdr_periodic'),
(16380,'spell_greater_invisibility_mob'),
(17244,'spell_anastari_possess'),
(19832,'spell_possess_razorgore'),
(19872,'spell_calm_dragonkin'),
(19873,'spell_destroy_egg'),
(20038,'spell_explosion_razorgore'),
(21651,'spell_opening_capping'),
(25684,'spell_summon_mana_fiends_moam'),
(25937,'spell_viscidus_freeze'),
(26009,'spell_cthun_periodic_rotate'),
(26084,'spell_aq_whirlwind'),
(26136,'spell_cthun_periodic_rotate'),
(26137,'spell_cthun_rotate_trigger'),
(26152,'spell_cthun_periodic_eye_trigger'),
(26180,'spell_huhuran_wyvern_string'),
(26398,'spell_cthun_hook_tentacle'),
(26584,'spell_summon_toxic_slime'),
(26591,'spell_teleport_image'),
(26619,'spell_periodic_scarab_trigger'),
(26686,'spell_aq_whirlwind'),
(28282,'spell_ashbringer_item');

-- TBC
INSERT INTO spell_scripts(Id, ScriptName) VALUES
(32811,'spell_greater_invisibility_mob'),
(29969,'spell_summon_blizzard'),
(29970,'spell_dispel_blizzard'),
(29979,'spell_massive_magnetic_pull'),
(30004,'spell_flame_wreath'),
(29770,'spell_midnight_mount'),
(29883,'spell_blink_arcane_anomaly'),
(29907,'spell_astral_bite'),
(29690,'spell_inebriate_removal'),
(30115,'spell_sacrifice'),
(31702,'spell_someone_grab_me'),
(31703,'spell_magnetic_pull'),
(31704,'spell_levitate'),
(30166,'spell_shadow_grasp_magtheridon'),
(30410,'spell_shadow_grasp_cube'),
(30658,'spell_quake_magtheridon'),
(30571,'spell_quake_magtheridon_knockback'),
(30425,'spell_portal_attunement'),
(30469,'spell_nether_beam'),
(38546,'spell_face_random_target'),
(30659,'spell_fel_infusion'),
(35139,'spell_throw_booms_doom'),
(35150,'spell_nether_charge_passive'),
(37670,'spell_nether_charge_timer'),
(33676,'spell_incite_chaos'),
(30843,'spell_enfeeble'),
(41624,'spell_enfeeble_removal'),
(32264,'spell_shirrak_inhibit_magic'),
(33332,'spell_suppression_blast'),
(32830,'spell_auchenai_possess'),
(36557,'spell_cursed_scarab_periodic'),
(36560,'spell_cursed_scarab_despawn_periodic'),
(34874,'spell_despawn_underbog_mushrooms'),
(36374,'spell_summon_smoke_beacon'),
(37591,'spell_inebriate_removal'),
(35754,'spell_soccothrates_charge'),
(38028,'spell_watery_grave'),
(34231,'spell_increased_holy_light_healing'),
(33793,'spell_vazruden_liquid_fire_script'),
(33794,'spell_vazruden_liquid_fire_script'),
(30926,'spell_vazruden_liquid_fire_script'),
(36921,'spell_vazruden_liquid_fire_script'),
(30700,'spell_face_highest_threat'),
(36153,'spell_soulbind'),
(36460,'spell_ultra_deconsolodation_zapper'),
(37431,'spell_lurker_spout_initial'),
(37429,'spell_lurker_spout_turning'),
(37430,'spell_lurker_spout_turning'),
(37851,'spell_tag_for_single_use'),
(33985,'spell_tag_for_single_use'),
(37896,'spell_to_infinity_and_above'),
(38606,'spell_exorcism_feather'),
(38640,'spell_koi_koi_death'),
(38915,'spell_mental_interference'),
(40186,'spell_summon_blossom_move_target'),
(40251,'spell_shadow_of_death'),
(41289,'spell_ethereal_ring_signal_flare'),
(41999,'spell_shadow_of_death_remove'),
(38358,'spell_tidal_surge_caribdis'),
(37676,'spell_insidious_whisper'),
(37750,'spell_clear_consuming_madness'),
(37640,'spell_leotheras_whirlwind_aura'),
(37641,'spell_leotheras_whirlwind_proc'),
(33572,'spell_gronn_lords_grasp'),
(38511,'spell_vashj_persuasion'),
(33040,'spell_wota_main_cast_and_jump'),
(33049,'spell_wota_main_cast_and_jump'),
(33045,'spell_wota_dot'),
(33048,'spell_wota_search'),
(33229,'spell_wota_remove'),
(42783,'spell_wrath_of_the_astromancer'),
(45043,'spell_power_circle'),
(44006,'spell_teleport_self_akilzon'),
(34630,'spell_scrap_reaver_spell'),
(43681,'spell_inactive'),
(34800,'spell_getting_sleepy_aura'),
(43364,'spell_getting_sleepy_aura');

-- Wotlk
INSERT INTO spell_scripts(Id, ScriptName) VALUES
(47028,'spell_taunka_face_me'),
(47431,'spell_capture_jormungar_spawn'),
(47435,'spell_scrape_corrosive_spit'),
(47447,'spell_corrosive_spit'),
(48268,'spell_container_of_rats'),
(48397,'spell_drop_off_villager'),
(49099,'spell_army_of_the_dead'),
(55931,'spell_conjure_flame_sphere'),
(57283,'spell_remove_mushroom_power');

-- Hunter
INSERT INTO spell_scripts(Id, ScriptName) VALUES
(34026,'spell_kill_command'),
(34477,'spell_stacking_rules_override');

-- Priest
INSERT INTO spell_scripts(Id, ScriptName) VALUES
(10060,'spell_stacking_rules_override');

-- Warlock
INSERT INTO spell_scripts(Id, ScriptName) VALUES
(980,'spell_curse_of_agony'),
(1014,'spell_curse_of_agony'),
(6217,'spell_curse_of_agony'),
(11711,'spell_curse_of_agony'),
(11712,'spell_curse_of_agony'),
(11713,'spell_curse_of_agony'),
(27218,'spell_curse_of_agony'),
(1454,'spell_life_tap'),
(1455,'spell_life_tap'),
(1456,'spell_life_tap'),
(11687,'spell_life_tap'),
(11688,'spell_life_tap'),
(11689,'spell_life_tap'),
(27222,'spell_life_tap'),
(57946,'spell_life_tap'),
(35696,'spell_demonic_knowledge'),
(30108,'spell_unstable_affliction'),
(30404,'spell_unstable_affliction'),
(30405,'spell_unstable_affliction'),
(32379,'spell_shadow_word_death'),
(32996,'spell_shadow_word_death');

-- Pet Scaling
INSERT INTO spell_scripts(Id, ScriptName) VALUES
(34902,'spell_hunter_pet_scaling_1'),
(34903,'spell_hunter_pet_scaling_2'),
(34904,'spell_hunter_pet_scaling_3'),

(34947,'spell_warlock_pet_scaling_1'),
(34956,'spell_warlock_pet_scaling_2'),
(34957,'spell_warlock_pet_scaling_3'),
(34958,'spell_warlock_pet_scaling_4'),

(35657,'spell_mage_pet_scaling_1'),
(35658,'spell_mage_pet_scaling_2'),
(35659,'spell_mage_pet_scaling_3'),
(35660,'spell_mage_pet_scaling_4'),

(35661,'spell_priest_pet_scaling_1'),
(35662,'spell_priest_pet_scaling_2'),
(35663,'spell_priest_pet_scaling_3'),
(35664,'spell_priest_pet_scaling_4'),

(35665,'spell_elemental_pet_scaling_1'),
(35666,'spell_elemental_pet_scaling_2'),
(35667,'spell_elemental_pet_scaling_3'),
(35668,'spell_elemental_pet_scaling_4'),

(35669,'spell_druid_pet_scaling_1'),
(35670,'spell_druid_pet_scaling_2'),
(35671,'spell_druid_pet_scaling_3'),
(35672,'spell_druid_pet_scaling_4'),

(35674,'spell_enhancement_pet_scaling_1'),
(35675,'spell_enhancement_pet_scaling_2'),
(35676,'spell_enhancement_pet_scaling_3'),

(36186,'spell_infernal_pet_scaling_1'),
(36188,'spell_infernal_pet_scaling_2'),
(36189,'spell_infernal_pet_scaling_3'),
(36190,'spell_infernal_pet_scaling_4');

-- Warrior
INSERT INTO spell_scripts(Id, ScriptName) VALUES
(5308,'spell_warrior_execute'),
(20658,'spell_warrior_execute'),
(20660,'spell_warrior_execute'),
(20661,'spell_warrior_execute'),
(20662,'spell_warrior_execute'),
(25234,'spell_warrior_execute'),
(25236,'spell_warrior_execute'),
(47470,'spell_warrior_execute'),
(47471,'spell_warrior_execute'),
(20647,'spell_warrior_execute_damage');

-- Mage
INSERT INTO spell_scripts(Id, ScriptName) VALUES
(11213,'spell_arcane_concentration'),
(12574,'spell_arcane_concentration'),
(12575,'spell_arcane_concentration'),
(12576,'spell_arcane_concentration'),
(12577,'spell_arcane_concentration');

-- Battleground and Outdoor PvP
-- Alterac Valley
INSERT INTO spell_scripts(Id, ScriptName) VALUES
-- Arathi basin
(23936,'spell_battleground_banner_trigger'),
(23932,'spell_battleground_banner_trigger'),
(23938,'spell_battleground_banner_trigger'),
(23935,'spell_battleground_banner_trigger'),
(23937,'spell_battleground_banner_trigger'),
-- Alterac Valley
(24677,'spell_battleground_banner_trigger'),
-- Zangarmarsh
(32433,'spell_outdoor_pvp_banner_trigger'),
(32438,'spell_outdoor_pvp_banner_trigger'),
-- Isle of Conquest
(35092,'spell_battleground_banner_trigger'),
(65826,'spell_battleground_banner_trigger'),
(65825,'spell_battleground_banner_trigger'),
(66686,'spell_battleground_banner_trigger'),
(66687,'spell_battleground_banner_trigger');

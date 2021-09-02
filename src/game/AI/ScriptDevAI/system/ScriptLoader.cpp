/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
* This program is free software licensed under GPL version 2
* Please see the included DOCS/LICENSE.TXT for more information */

#include "AI/ScriptDevAI/include/sc_common.h"

// battlegrounds
extern void AddSC_battleground();

// custom

// examples
extern void AddSC_example_creature();
extern void AddSC_example_escort();
extern void AddSC_example_gossip_codebox();
extern void AddSC_example_misc();

// world
extern void AddSC_areatrigger_scripts();
extern void AddSC_bosses_emerald_dragons();
extern void AddSC_generic_creature();
extern void AddSC_go_scripts();
extern void AddSC_guards();
extern void AddSC_item_scripts();
extern void AddSC_npc_professions();
extern void AddSC_npcs_special();
extern void AddSC_quests_scripts();
extern void AddSC_spell_scripts();
extern void AddSC_world_map_scripts();
extern void AddSC_boss_highlord_kruul();
extern void AddSC_war_effort();
extern void AddSC_scourge_invasion();
extern void AddSC_suns_reach_reclamation();
extern void AddSC_world_map_ebon_hold();
extern void AddSC_shade_of_the_horseman();
extern void AddSC_childrens_week_tbc();
extern void AddSC_midsummer_festival();
extern void AddSC_hallows_end();
extern void AddSC_brewfest();

// eastern kingdoms
extern void AddSC_blackrock_depths();                       // blackrock_depths
extern void AddSC_boss_ambassador_flamelash();
extern void AddSC_boss_coren_direbrew();
extern void AddSC_boss_draganthaurissan();
extern void AddSC_boss_general_angerforge();
extern void AddSC_boss_high_interrogator_gerstahn();
extern void AddSC_instance_blackrock_depths();
extern void AddSC_boss_overlordwyrmthalak();                // blackrock_spire
extern void AddSC_boss_pyroguard_emberseer();
extern void AddSC_boss_gyth();
extern void AddSC_instance_blackrock_spire();
extern void AddSC_boss_razorgore();                         // blackwing_lair
extern void AddSC_boss_vaelastrasz();
extern void AddSC_boss_broodlord();
extern void AddSC_boss_firemaw();
extern void AddSC_boss_ebonroc();
extern void AddSC_boss_flamegor();
extern void AddSC_boss_chromaggus();
extern void AddSC_boss_nefarian();
extern void AddSC_boss_victor_nefarius();
extern void AddSC_instance_blackwing_lair();
extern void AddSC_boss_mr_smite();                          // deadmines
extern void AddSC_deadmines();
extern void AddSC_instance_deadmines();
extern void AddSC_gnomeregan();                             // gnomeregan
extern void AddSC_boss_thermaplugg();
extern void AddSC_instance_gnomeregan();
extern void AddSC_boss_attumen();                           // karazhan
extern void AddSC_boss_curator();
extern void AddSC_boss_maiden_of_virtue();
extern void AddSC_boss_shade_of_aran();
extern void AddSC_boss_netherspite();
extern void AddSC_boss_nightbane();
extern void AddSC_boss_prince_malchezaar();
extern void AddSC_boss_terestian_illhoof();
extern void AddSC_boss_moroes();
extern void AddSC_bosses_opera();
extern void AddSC_chess_event();
extern void AddSC_instance_karazhan();
extern void AddSC_karazhan();
extern void AddSC_boss_felblood_kaelthas();                 // magisters_terrace
extern void AddSC_boss_selin_fireheart();
extern void AddSC_boss_vexallus();
extern void AddSC_boss_priestess_delrissa();
extern void AddSC_instance_magisters_terrace();
extern void AddSC_magisters_terrace();
extern void AddSC_boss_lucifron();                          // molten_core
extern void AddSC_boss_magmadar();
extern void AddSC_boss_gehennas();
extern void AddSC_boss_garr();
extern void AddSC_boss_baron_geddon();
extern void AddSC_boss_shazzrah();
extern void AddSC_boss_golemagg();
extern void AddSC_boss_sulfuron();
extern void AddSC_boss_majordomo();
extern void AddSC_boss_ragnaros();
extern void AddSC_instance_molten_core();
extern void AddSC_molten_core();
extern void AddSC_ebon_hold();                              // scarlet_enclave
extern void AddSC_boss_arcanist_doan();                     // scarlet_monastery
extern void AddSC_boss_herod();
extern void AddSC_boss_mograine_and_whitemane();
extern void AddSC_boss_headless_horseman();
extern void AddSC_instance_scarlet_monastery();
extern void AddSC_boss_darkmaster_gandling();               // scholomance
extern void AddSC_boss_jandicebarov();
extern void AddSC_instance_scholomance();
extern void AddSC_scholomance();
extern void AddSC_boss_hummel();                            // shadowfang_keep
extern void AddSC_shadowfang_keep();
extern void AddSC_instance_shadowfang_keep();
extern void AddSC_boss_maleki_the_pallid();                 // stratholme
extern void AddSC_boss_cannon_master_willey();
extern void AddSC_boss_baroness_anastari();
extern void AddSC_boss_dathrohan_balnazzar();
extern void AddSC_boss_order_of_silver_hand();
extern void AddSC_instance_stratholme();
extern void AddSC_stratholme();
extern void AddSC_instance_sunken_temple();                 // sunken_temple
extern void AddSC_sunken_temple();
extern void AddSC_boss_brutallus();                         // sunwell_plateau
extern void AddSC_boss_eredar_twins();
extern void AddSC_boss_felmyst();
extern void AddSC_boss_kalecgos();
extern void AddSC_boss_kiljaeden();
extern void AddSC_boss_muru();
extern void AddSC_instance_sunwell_plateau();
extern void AddSC_sunwell_plateau();
extern void AddSC_boss_archaedas();                         // uldaman
extern void AddSC_instance_uldaman();
extern void AddSC_uldaman();
extern void AddSC_boss_akilzon();                           // zulaman
extern void AddSC_boss_halazzi();
extern void AddSC_boss_janalai();
extern void AddSC_boss_malacrass();
extern void AddSC_boss_nalorakk();
extern void AddSC_instance_zulaman();
extern void AddSC_zulaman();
extern void AddSC_boss_zuljin();
extern void AddSC_boss_arlokk();                            // zulgurub
extern void AddSC_boss_hakkar();
extern void AddSC_boss_hazzarah();
extern void AddSC_boss_jeklik();
extern void AddSC_boss_jindo();
extern void AddSC_boss_mandokir();
extern void AddSC_boss_marli();
extern void AddSC_boss_ouro();
extern void AddSC_boss_renataki();
extern void AddSC_boss_thekal();
extern void AddSC_boss_venoxis();
extern void AddSC_instance_zulgurub();
extern void AddSC_zulgurub();

extern void AddSC_alterac_mountains();
extern void AddSC_arathi_highlands();
extern void AddSC_blasted_lands();
extern void AddSC_boss_kazzakAI();
extern void AddSC_burning_steppes();
extern void AddSC_dun_morogh();
extern void AddSC_eastern_plaguelands();
extern void AddSC_elwynn_forest();
extern void AddSC_eversong_woods();
extern void AddSC_ghostlands();
extern void AddSC_hinterlands();
extern void AddSC_ironforge();
extern void AddSC_isle_of_queldanas();
extern void AddSC_loch_modan();
extern void AddSC_redridge_mountains();
extern void AddSC_searing_gorge();
extern void AddSC_silvermoon_city();
extern void AddSC_silverpine_forest();
extern void AddSC_stormwind_city();
extern void AddSC_stranglethorn_vale();
extern void AddSC_swamp_of_sorrows();
extern void AddSC_tirisfal_glades();
extern void AddSC_undercity();
extern void AddSC_western_plaguelands();
extern void AddSC_westfall();
extern void AddSC_wetlands();
extern void AddSC_world_eastern_kingdoms();

// kalimdor
extern void AddSC_instance_blackfathom_deeps();             // blackfathom_deeps
extern void AddSC_boss_aeonus();                            // COT, dark_portal
extern void AddSC_boss_chrono_lord_deja();
extern void AddSC_boss_temporus();
extern void AddSC_dark_portal();
extern void AddSC_instance_dark_portal();
extern void AddSC_hyjal();                                  // COT, hyjal
extern void AddSC_boss_archimonde();
extern void AddSC_boss_anetheron();
extern void AddSC_boss_azgalor();
extern void AddSC_boss_kazrogal();
extern void AddSC_boss_rage_winterchill();
extern void AddSC_instance_mount_hyjal();
extern void AddSC_instance_old_hillsbrad();                 // COT, old_hillsbrad
extern void AddSC_old_hillsbrad();
extern void AddSC_culling_of_stratholme();                  // COT, culling_of_stratholme
extern void AddSC_instance_culling_of_stratholme();
extern void AddSC_dire_maul();                              // dire_maul
extern void AddSC_instance_dire_maul();
extern void AddSC_instance_maraudon();                      // maraudon
extern void AddSC_boss_noxxion();
extern void AddSC_boss_onyxia();                            // onyxias_lair
extern void AddSC_instance_onyxias_lair();
extern void AddSC_instance_razorfen_downs();                // razorfen_downs
extern void AddSC_razorfen_downs();
extern void AddSC_instance_razorfen_kraul();                // razorfen_kraul
extern void AddSC_razorfen_kraul();
extern void AddSC_boss_ayamiss();                           // ruins_of_ahnqiraj
extern void AddSC_boss_buru();
extern void AddSC_boss_kurinnaxx();
extern void AddSC_boss_ossirian();
extern void AddSC_boss_moam();
extern void AddSC_boss_rajaxx();
extern void AddSC_ruins_of_ahnqiraj();
extern void AddSC_instance_ruins_of_ahnqiraj();
extern void AddSC_boss_cthun();                             // temple_of_ahnqiraj
extern void AddSC_boss_fankriss();
extern void AddSC_boss_huhuran();
extern void AddSC_bug_trio();
extern void AddSC_boss_sartura();
extern void AddSC_boss_skeram();
extern void AddSC_boss_twinemperors();
extern void AddSC_boss_viscidus();
extern void AddSC_mob_anubisath_sentinel();
extern void AddSC_instance_temple_of_ahnqiraj();
extern void AddSC_instance_wailing_caverns();               // wailing_caverns
extern void AddSC_wailing_caverns();
extern void AddSC_boss_zumrah();                            // zulfarrak
extern void AddSC_instance_zulfarrak();
extern void AddSC_zulfarrak();

extern void AddSC_ashenvale();
extern void AddSC_azshara();
extern void AddSC_azuremyst_isle();
extern void AddSC_bloodmyst_isle();
extern void AddSC_boss_azuregos();
extern void AddSC_darkshore();
extern void AddSC_desolace();
extern void AddSC_durotar();
extern void AddSC_dustwallow_marsh();
extern void AddSC_felwood();
extern void AddSC_feralas();
extern void AddSC_moonglade();
extern void AddSC_mulgore();
extern void AddSC_orgrimmar();
extern void AddSC_silithus();
extern void AddSC_stonetalon_mountains();
extern void AddSC_tanaris();
extern void AddSC_teldrassil();
extern void AddSC_the_barrens();
extern void AddSC_thousand_needles();
extern void AddSC_thunder_bluff();
extern void AddSC_ungoro_crater();
extern void AddSC_winterspring();
extern void AddSC_world_kalimdor();
extern void AddSC_world_northrend();

// northrend
extern void AddSC_boss_amanitar();                          // azjol-nerub, ahnkahet
extern void AddSC_boss_jedoga();
extern void AddSC_boss_nadox();
extern void AddSC_boss_taldaram();
extern void AddSC_boss_volazj();
extern void AddSC_instance_ahnkahet();
extern void AddSC_boss_anubarak();                          // azjol-nerub, azjol-nerub
extern void AddSC_boss_hadronox();
extern void AddSC_boss_krikthir();
extern void AddSC_instance_azjol_nerub();
extern void AddSC_boss_argent_challenge();                  // CC, trial_of_the_champion
extern void AddSC_boss_black_knight();
extern void AddSC_boss_grand_champions();
extern void AddSC_instance_trial_of_the_champion();
extern void AddSC_trial_of_the_champion();
extern void AddSC_boss_anubarak_trial();                    // CC, trial_of_the_crusader
extern void AddSC_boss_faction_champions();
extern void AddSC_boss_jaraxxus();
extern void AddSC_instance_trial_of_the_crusader();
extern void AddSC_northrend_beasts();
extern void AddSC_trial_of_the_crusader();
extern void AddSC_twin_valkyr();
extern void AddSC_boss_novos();                             // draktharon_keep
extern void AddSC_boss_tharonja();
extern void AddSC_boss_trollgore();
extern void AddSC_instance_draktharon_keep();
extern void AddSC_boss_colossus();                          // gundrak
extern void AddSC_boss_eck();
extern void AddSC_boss_galdarah();
extern void AddSC_boss_moorabi();
extern void AddSC_boss_sladran();
extern void AddSC_instance_gundrak();
extern void AddSC_boss_bronjahm();                          // ICC, forge_of_souls
extern void AddSC_boss_devourer_of_souls();
extern void AddSC_instance_forge_of_souls();
extern void AddSC_boss_lich_king();                         // ICC, halls_of_reflection
extern void AddSC_halls_of_reflection();
extern void AddSC_instance_halls_of_reflection();
extern void AddSC_boss_garfrost();                          // ICC, pit_of_saron
extern void AddSC_boss_krick_and_ick();
extern void AddSC_boss_tyrannus();
extern void AddSC_instance_pit_of_saron();
extern void AddSC_pit_of_saron();
extern void AddSC_blood_prince_council();                   // ICC, icecrown_citadel
extern void AddSC_boss_blood_queen_lanathel();
extern void AddSC_boss_deathbringer_saurfang();
extern void AddSC_boss_festergut();
extern void AddSC_boss_lady_deathwhisper();
extern void AddSC_boss_lord_marrowgar();
extern void AddSC_boss_professor_putricide();
extern void AddSC_boss_rotface();
extern void AddSC_boss_sindragosa();
extern void AddSC_boss_the_lich_king();
extern void AddSC_boss_valithria_dreamwalker();
extern void AddSC_gunship_battle();
extern void AddSC_icecrown_citadel();
extern void AddSC_instance_icecrown_citadel();
extern void AddSC_boss_anubrekhan();                        // naxxramas
extern void AddSC_boss_four_horsemen();
extern void AddSC_boss_faerlina();
extern void AddSC_boss_gluth();
extern void AddSC_boss_gothik();
extern void AddSC_boss_grobbulus();
extern void AddSC_boss_kelthuzad();
extern void AddSC_boss_loatheb();
extern void AddSC_boss_maexxna();
extern void AddSC_boss_noth();
extern void AddSC_boss_heigan();
extern void AddSC_boss_patchwerk();
extern void AddSC_boss_razuvious();
extern void AddSC_boss_sapphiron();
extern void AddSC_boss_thaddius();
extern void AddSC_instance_naxxramas();
extern void AddSC_boss_malygos();                           // nexus, eye_of_eternity
extern void AddSC_instance_eye_of_eternity();
extern void AddSC_boss_anomalus();                          // nexus, nexus
extern void AddSC_boss_keristrasza();
extern void AddSC_boss_ormorok();
extern void AddSC_boss_telestra();
extern void AddSC_instance_nexus();
extern void AddSC_boss_eregos();                            // nexus, oculus
extern void AddSC_boss_urom();
extern void AddSC_boss_varos();
extern void AddSC_instance_oculus();
extern void AddSC_oculus();
extern void AddSC_boss_sartharion();                        // obsidian_sanctum
extern void AddSC_instance_obsidian_sanctum();
extern void AddSC_boss_baltharus();                         // ruby_sanctum
extern void AddSC_boss_halion();
extern void AddSC_boss_saviana();
extern void AddSC_boss_zarithrian();
extern void AddSC_instance_ruby_sanctum();
extern void AddSC_ruby_sanctum();
extern void AddSC_boss_bjarngrim();                         // ulduar, halls_of_lightning
extern void AddSC_boss_ionar();
extern void AddSC_boss_loken();
extern void AddSC_boss_volkhan();
extern void AddSC_instance_halls_of_lightning();
extern void AddSC_boss_sjonnir();                           // ulduar, halls_of_stone
extern void AddSC_halls_of_stone();
extern void AddSC_instance_halls_of_stone();
extern void AddSC_boss_assembly_of_iron();                  // ulduar, ulduar
extern void AddSC_boss_algalon();
extern void AddSC_boss_auriaya();
extern void AddSC_boss_flame_leviathan();
extern void AddSC_boss_freya();
extern void AddSC_boss_general_vezax();
extern void AddSC_boss_hodir();
extern void AddSC_boss_ignis();
extern void AddSC_boss_kologarn();
extern void AddSC_boss_mimiron();
extern void AddSC_boss_razorscale();
extern void AddSC_boss_thorim();
extern void AddSC_boss_xt_002();
extern void AddSC_boss_yogg_saron();
extern void AddSC_instance_ulduar();
extern void AddSC_ulduar();
extern void AddSC_boss_ingvar();                            // utgarde_keep, utgarde_keep
extern void AddSC_boss_keleseth();
extern void AddSC_instance_utgarde_keep();
extern void AddSC_utgarde_keep();
extern void AddSC_boss_gortok();                           // utgarde_keep, utgarde_pinnacle
extern void AddSC_boss_skadi();
extern void AddSC_boss_svala();
extern void AddSC_boss_ymiron();
extern void AddSC_instance_pinnacle();
extern void AddSC_boss_archavon();                          // vault_of_archavon
extern void AddSC_boss_emalon();
extern void AddSC_boss_koralon();
extern void AddSC_boss_toravon();
extern void AddSC_instance_vault_of_archavon();
extern void AddSC_boss_ichoron();                           // violet_hold
extern void AddSC_boss_xevozz();
extern void AddSC_instance_violet_hold();
extern void AddSC_violet_hold();

extern void AddSC_borean_tundra();
extern void AddSC_dalaran();
extern void AddSC_dragonblight();
extern void AddSC_grizzly_hills();
extern void AddSC_howling_fjord();
extern void AddSC_icecrown();
extern void AddSC_sholazar_basin();
extern void AddSC_storm_peaks();
extern void AddSC_wintergrasp();
extern void AddSC_zuldrak();

// outland
extern void AddSC_boss_exarch_maladaar();                   // auchindoun, auchenai_crypts
extern void AddSC_boss_shirrak();
extern void AddSC_boss_nexusprince_shaffar();               // auchindoun, mana_tombs
extern void AddSC_boss_pandemonius();
extern void AddSC_mana_tombs();
extern void AddSC_boss_yor();
extern void AddSC_boss_anzu();                              // auchindoun, sethekk_halls
extern void AddSC_boss_darkweaver_syth();
extern void AddSC_boss_talon_king_ikiss();
extern void AddSC_instance_sethekk_halls();
extern void AddSC_boss_ambassador_hellmaw();                // auchindoun, shadow_labyrinth
extern void AddSC_boss_blackheart_the_inciter();
extern void AddSC_boss_grandmaster_vorpil();
extern void AddSC_boss_murmur();
extern void AddSC_instance_shadow_labyrinth();
extern void AddSC_black_temple();                           // black_temple
extern void AddSC_boss_illidan();
extern void AddSC_boss_shade_of_akama();
extern void AddSC_boss_supremus();
extern void AddSC_boss_gurtogg_bloodboil();
extern void AddSC_boss_mother_shahraz();
extern void AddSC_boss_reliquary_of_souls();
extern void AddSC_boss_teron_gorefiend();
extern void AddSC_boss_najentus();
extern void AddSC_boss_illidari_council();
extern void AddSC_instance_black_temple();
extern void AddSC_boss_fathomlord_karathress();             // CR, serpent_shrine
extern void AddSC_boss_hydross_the_unstable();
extern void AddSC_boss_lady_vashj();
extern void AddSC_boss_leotheras_the_blind();
extern void AddSC_boss_morogrim_tidewalker();
extern void AddSC_boss_the_lurker_below();
extern void AddSC_instance_serpentshrine_cavern();
extern void AddSC_boss_ahune();                             // CR, slave_pens
extern void AddSC_instance_slave_pens();
extern void AddSC_boss_hydromancer_thespia();               // CR, steam_vault
extern void AddSC_boss_mekgineer_steamrigger();
extern void AddSC_boss_warlord_kalithresh();
extern void AddSC_instance_steam_vault();
extern void AddSC_boss_hungarfen();                         // CR, Underbog
extern void AddSC_boss_swamplord_muselek();
extern void AddSC_boss_black_stalker();
extern void AddSC_instance_underbog();
extern void AddSC_boss_gruul();                             // gruuls_lair
extern void AddSC_boss_high_king_maulgar();
extern void AddSC_instance_gruuls_lair();
extern void AddSC_boss_broggok();                           // HC, blood_furnace
extern void AddSC_boss_kelidan_the_breaker();
extern void AddSC_boss_the_maker();
extern void AddSC_instance_blood_furnace();
extern void AddSC_boss_nazan_and_vazruden();                // HC, hellfire_ramparts
extern void AddSC_boss_omor_the_unscarred();
extern void AddSC_boss_watchkeeper_gargolmar();
extern void AddSC_instance_ramparts();
extern void AddSC_boss_magtheridon();                       // HC, magtheridons_lair
extern void AddSC_instance_magtheridons_lair();
extern void AddSC_boss_grand_warlock_nethekurse();          // HC, shattered_halls
extern void AddSC_boss_warbringer_omrogg();
extern void AddSC_boss_warchief_kargath_bladefist();
extern void AddSC_instance_shattered_halls();
extern void AddSC_arcatraz();                               // TK, arcatraz
extern void AddSC_boss_dalliah();
extern void AddSC_boss_harbinger_skyriss();
extern void AddSC_boss_soccothrates();
extern void AddSC_instance_arcatraz();
extern void AddSC_boss_high_botanist_freywinn();            // TK, botanica
extern void AddSC_boss_thorngrin();
extern void AddSC_boss_laj();
extern void AddSC_boss_warp_splinter();
extern void AddSC_boss_alar();                              // TK, the_eye
extern void AddSC_boss_high_astromancer_solarian();
extern void AddSC_boss_kaelthas();
extern void AddSC_boss_void_reaver();
extern void AddSC_instance_the_eye();
extern void AddSC_boss_mechano_lord_capacitus();
extern void AddSC_boss_nethermancer_sepethrea();            // TK, the_mechanar
extern void AddSC_boss_pathaleon_the_calculator();
extern void AddSC_instance_mechanar();

extern void AddSC_blades_edge_mountains();
extern void AddSC_bashir_landing();
extern void AddSC_shartuul_transporter();
extern void AddSC_boss_doomlordkazzak();
extern void AddSC_boss_doomwalker();
extern void AddSC_hellfire_peninsula();
extern void AddSC_nagrand();
extern void AddSC_netherstorm();
extern void AddSC_shadowmoon_valley();
extern void AddSC_shattrath_city();
extern void AddSC_terokkar_forest();
extern void AddSC_boss_terokk();
extern void AddSC_OutlandWorldScript();
extern void AddSC_zangarmarsh();

void AddScripts()
{
    // battlegrounds
    AddSC_battleground();

    // custom

    // examples
    AddSC_example_creature();
    AddSC_example_escort();
    AddSC_example_gossip_codebox();
    AddSC_example_misc();

    // world
    AddSC_areatrigger_scripts();
    AddSC_bosses_emerald_dragons();
    AddSC_generic_creature();
    AddSC_go_scripts();
    AddSC_guards();
    AddSC_item_scripts();
    AddSC_npc_professions();
    AddSC_npcs_special();
    AddSC_quests_scripts();
    AddSC_spell_scripts();
    AddSC_world_map_scripts();
    AddSC_boss_highlord_kruul();
    AddSC_war_effort();
    AddSC_scourge_invasion();
    AddSC_suns_reach_reclamation();
    AddSC_world_map_ebon_hold();
    AddSC_shade_of_the_horseman();
    AddSC_childrens_week_tbc();
    AddSC_midsummer_festival();
    AddSC_hallows_end();
    AddSC_brewfest();

    // eastern kingdoms
    AddSC_blackrock_depths();                               // blackrock_depths
    AddSC_boss_ambassador_flamelash();
    AddSC_boss_coren_direbrew();
    AddSC_boss_draganthaurissan();
    AddSC_boss_general_angerforge();
    AddSC_boss_high_interrogator_gerstahn();
    AddSC_instance_blackrock_depths();
    AddSC_boss_overlordwyrmthalak();                        // blackrock_spire
    AddSC_boss_pyroguard_emberseer();
    AddSC_boss_gyth();
    AddSC_instance_blackrock_spire();
    AddSC_boss_razorgore();                                 // blackwing_lair
    AddSC_boss_vaelastrasz();
    AddSC_boss_broodlord();
    AddSC_boss_firemaw();
    AddSC_boss_ebonroc();
    AddSC_boss_flamegor();
    AddSC_boss_chromaggus();
    AddSC_boss_nefarian();
    AddSC_boss_victor_nefarius();
    AddSC_instance_blackwing_lair();
    AddSC_deadmines();                                      // deadmines
    AddSC_boss_mr_smite();
    AddSC_instance_deadmines();
    AddSC_gnomeregan();                                     // gnomeregan
    AddSC_boss_thermaplugg();
    AddSC_instance_gnomeregan();
    AddSC_boss_attumen();                                   // karazhan
    AddSC_boss_curator();
    AddSC_boss_maiden_of_virtue();
    AddSC_boss_shade_of_aran();
    AddSC_boss_netherspite();
    AddSC_boss_nightbane();
    AddSC_boss_prince_malchezaar();
    AddSC_boss_terestian_illhoof();
    AddSC_boss_moroes();
    AddSC_bosses_opera();
    AddSC_chess_event();
    AddSC_instance_karazhan();
    AddSC_karazhan();
    AddSC_boss_felblood_kaelthas();                         // magisters_terrace
    AddSC_boss_selin_fireheart();
    AddSC_boss_vexallus();
    AddSC_boss_priestess_delrissa();
    AddSC_instance_magisters_terrace();
    AddSC_magisters_terrace();
    AddSC_boss_lucifron();                                  // molten_core
    AddSC_boss_magmadar();
    AddSC_boss_gehennas();
    AddSC_boss_garr();
    AddSC_boss_baron_geddon();
    AddSC_boss_shazzrah();
    AddSC_boss_golemagg();
    AddSC_boss_sulfuron();
    AddSC_boss_majordomo();
    AddSC_boss_ragnaros();
    AddSC_instance_molten_core();
    AddSC_molten_core();
    AddSC_ebon_hold();                                      // scarlet_enclave
    AddSC_boss_arcanist_doan();                             // scarlet_monastery
    AddSC_boss_herod();
    AddSC_boss_mograine_and_whitemane();
    AddSC_boss_headless_horseman();
    AddSC_instance_scarlet_monastery();
    AddSC_boss_darkmaster_gandling();                       // scholomance
    AddSC_boss_jandicebarov();
    AddSC_instance_scholomance();
    AddSC_scholomance();
    AddSC_boss_hummel();                                    // shadowfang_keep
    AddSC_shadowfang_keep();
    AddSC_instance_shadowfang_keep();
    AddSC_boss_maleki_the_pallid();                         // stratholme
    AddSC_boss_cannon_master_willey();
    AddSC_boss_baroness_anastari();
    AddSC_boss_dathrohan_balnazzar();
    AddSC_boss_order_of_silver_hand();
    AddSC_instance_stratholme();
    AddSC_stratholme();
    AddSC_instance_sunken_temple();                         // sunken_temple
    AddSC_sunken_temple();
    AddSC_boss_brutallus();                                 // sunwell_plateau
    AddSC_boss_eredar_twins();
    AddSC_boss_felmyst();
    AddSC_boss_kalecgos();
    AddSC_boss_kiljaeden();
    AddSC_boss_muru();
    AddSC_instance_sunwell_plateau();
    AddSC_sunwell_plateau();
    AddSC_boss_archaedas();                                 // uldaman
    AddSC_instance_uldaman();
    AddSC_uldaman();
    AddSC_boss_akilzon();                                   // zulaman
    AddSC_boss_halazzi();
    AddSC_boss_janalai();
    AddSC_boss_malacrass();
    AddSC_boss_nalorakk();
    AddSC_instance_zulaman();
    AddSC_zulaman();
    AddSC_boss_zuljin();
    AddSC_boss_arlokk();                                    // zulgurub
    AddSC_boss_hakkar();
    AddSC_boss_hazzarah();
    AddSC_boss_jeklik();
    AddSC_boss_jindo();
    AddSC_boss_mandokir();
    AddSC_boss_marli();
    AddSC_boss_ouro();
    AddSC_boss_renataki();
    AddSC_boss_thekal();
    AddSC_boss_venoxis();
    AddSC_instance_zulgurub();
    AddSC_zulgurub();

    AddSC_alterac_mountains();
    AddSC_arathi_highlands();
    AddSC_blasted_lands();
    AddSC_boss_kazzakAI();
    AddSC_burning_steppes();
    AddSC_dun_morogh();
    AddSC_eastern_plaguelands();
    AddSC_elwynn_forest();
    AddSC_eversong_woods();
    AddSC_ghostlands();
    AddSC_hinterlands();
    AddSC_ironforge();
    AddSC_isle_of_queldanas();
    AddSC_loch_modan();
    AddSC_redridge_mountains();
    AddSC_searing_gorge();
    AddSC_silvermoon_city();
    AddSC_silverpine_forest();
    AddSC_stormwind_city();
    AddSC_stranglethorn_vale();
    AddSC_swamp_of_sorrows();
    AddSC_tirisfal_glades();
    AddSC_undercity();
    AddSC_western_plaguelands();
    AddSC_westfall();
    AddSC_wetlands();
    AddSC_world_eastern_kingdoms();

    // kalimdor
    AddSC_instance_blackfathom_deeps();                     // blackfathom deeps
    AddSC_boss_aeonus();                                    // CoT, dark_portal
    AddSC_boss_chrono_lord_deja();
    AddSC_boss_temporus();
    AddSC_dark_portal();
    AddSC_instance_dark_portal();
    AddSC_hyjal();                                          // CoT, hyjal
    AddSC_boss_archimonde();
    AddSC_boss_anetheron();
    AddSC_boss_azgalor();
    AddSC_boss_kazrogal();
    AddSC_boss_rage_winterchill();
    AddSC_instance_mount_hyjal();
    AddSC_instance_old_hillsbrad();                         // CoT, old_hillsbrand
    AddSC_old_hillsbrad();
    AddSC_culling_of_stratholme();                          // CoT, culling_of_stratholme
    AddSC_instance_culling_of_stratholme();
    AddSC_dire_maul();                                      // dire_maul
    AddSC_instance_dire_maul();
    AddSC_instance_maraudon();                              // maraudon
    AddSC_boss_noxxion();
    AddSC_boss_onyxia();                                    // onyxias_lair
    AddSC_instance_onyxias_lair();
    AddSC_instance_razorfen_downs();                        // razorfen_downs
    AddSC_razorfen_downs();
    AddSC_instance_razorfen_kraul();                        // razorfen_kraul
    AddSC_razorfen_kraul();
    AddSC_boss_ayamiss();                                   // ruins_of_ahnqiraj
    AddSC_boss_buru();
    AddSC_boss_kurinnaxx();
    AddSC_boss_ossirian();
    AddSC_boss_moam();
    AddSC_boss_rajaxx();
    AddSC_ruins_of_ahnqiraj();
    AddSC_instance_ruins_of_ahnqiraj();
    AddSC_boss_cthun();                                     // temple_of_ahnqiraj
    AddSC_boss_fankriss();
    AddSC_boss_huhuran();
    AddSC_bug_trio();
    AddSC_boss_sartura();
    AddSC_boss_skeram();
    AddSC_boss_twinemperors();
    AddSC_boss_viscidus();
    AddSC_mob_anubisath_sentinel();
    AddSC_instance_temple_of_ahnqiraj();
    AddSC_instance_wailing_caverns();                       // wailing_caverns
    AddSC_wailing_caverns();
    AddSC_boss_zumrah();                                    // zulfarrak
    AddSC_zulfarrak();
    AddSC_instance_zulfarrak();

    AddSC_ashenvale();
    AddSC_azshara();
    AddSC_azuremyst_isle();
    AddSC_bloodmyst_isle();
    AddSC_boss_azuregos();
    AddSC_darkshore();
    AddSC_desolace();
    AddSC_durotar();
    AddSC_dustwallow_marsh();
    AddSC_felwood();
    AddSC_feralas();
    AddSC_moonglade();
    AddSC_mulgore();
    AddSC_orgrimmar();
    AddSC_silithus();
    AddSC_stonetalon_mountains();
    AddSC_tanaris();
    AddSC_teldrassil();
    AddSC_the_barrens();
    AddSC_thousand_needles();
    AddSC_thunder_bluff();
    AddSC_ungoro_crater();
    AddSC_winterspring();
    AddSC_world_kalimdor();
    AddSC_world_northrend();

    // northrend
    AddSC_boss_amanitar();                                  // azjol-nerub, ahnkahet
    AddSC_boss_jedoga();
    AddSC_boss_nadox();
    AddSC_boss_taldaram();
    AddSC_boss_volazj();
    AddSC_instance_ahnkahet();
    AddSC_boss_anubarak();                                  // azjol-nerub, azjol-nerub
    AddSC_boss_hadronox();
    AddSC_boss_krikthir();
    AddSC_instance_azjol_nerub();
    AddSC_boss_argent_challenge();                          // CC, trial_of_the_champion
    AddSC_boss_black_knight();
    AddSC_boss_grand_champions();
    AddSC_instance_trial_of_the_champion();
    AddSC_trial_of_the_champion();
    AddSC_boss_anubarak_trial();                            // CC, trial_of_the_crusader
    AddSC_boss_faction_champions();
    AddSC_boss_jaraxxus();
    AddSC_instance_trial_of_the_crusader();
    AddSC_northrend_beasts();
    AddSC_trial_of_the_crusader();
    AddSC_twin_valkyr();
    AddSC_boss_novos();                                     // draktharon_keep
    AddSC_boss_tharonja();
    AddSC_boss_trollgore();
    AddSC_instance_draktharon_keep();
    AddSC_boss_colossus();                                  // gundrak
    AddSC_boss_eck();
    AddSC_boss_galdarah();
    AddSC_boss_moorabi();
    AddSC_boss_sladran();
    AddSC_instance_gundrak();
    AddSC_boss_bronjahm();                                  // ICC, FH, forge_of_souls
    AddSC_boss_devourer_of_souls();
    AddSC_instance_forge_of_souls();
    AddSC_boss_lich_king();                                 // ICC, FH, halls_of_reflection
    AddSC_halls_of_reflection();
    AddSC_instance_halls_of_reflection();
    AddSC_boss_garfrost();                                  // ICC, FH, pit_of_saron
    AddSC_boss_krick_and_ick();
    AddSC_boss_tyrannus();
    AddSC_instance_pit_of_saron();
    AddSC_pit_of_saron();
    AddSC_blood_prince_council();                           // ICC, icecrown_citadel
    AddSC_boss_blood_queen_lanathel();
    AddSC_boss_deathbringer_saurfang();
    AddSC_boss_festergut();
    AddSC_boss_lady_deathwhisper();
    AddSC_boss_lord_marrowgar();
    AddSC_boss_professor_putricide();
    AddSC_boss_rotface();
    AddSC_boss_sindragosa();
    AddSC_boss_the_lich_king();
    AddSC_boss_valithria_dreamwalker();
    AddSC_gunship_battle();
    AddSC_icecrown_citadel();
    AddSC_instance_icecrown_citadel();
    AddSC_boss_anubrekhan();                                // naxxramas
    AddSC_boss_four_horsemen();
    AddSC_boss_faerlina();
    AddSC_boss_gluth();
    AddSC_boss_gothik();
    AddSC_boss_grobbulus();
    AddSC_boss_kelthuzad();
    AddSC_boss_loatheb();
    AddSC_boss_maexxna();
    AddSC_boss_noth();
    AddSC_boss_heigan();
    AddSC_boss_patchwerk();
    AddSC_boss_razuvious();
    AddSC_boss_sapphiron();
    AddSC_boss_thaddius();
    AddSC_instance_naxxramas();
    AddSC_boss_malygos();                                   // nexus, eye_of_eternity
    AddSC_instance_eye_of_eternity();
    AddSC_boss_anomalus();                                  // nexus, nexus
    AddSC_boss_keristrasza();
    AddSC_boss_ormorok();
    AddSC_boss_telestra();
    AddSC_instance_nexus();
    AddSC_boss_eregos();                                    // nexus, oculus
    AddSC_boss_urom();
    AddSC_boss_varos();
    AddSC_instance_oculus();
    AddSC_oculus();
    AddSC_boss_sartharion();                                // obsidian_sanctum
    AddSC_instance_obsidian_sanctum();
    AddSC_boss_baltharus();                                 // ruby_sanctum
    AddSC_boss_halion();
    AddSC_boss_saviana();
    AddSC_boss_zarithrian();
    AddSC_instance_ruby_sanctum();
    AddSC_ruby_sanctum();
    AddSC_boss_bjarngrim();                                 // ulduar, halls_of_lightning
    AddSC_boss_ionar();
    AddSC_boss_loken();
    AddSC_boss_volkhan();
    AddSC_instance_halls_of_lightning();
    AddSC_boss_sjonnir();                                   // ulduar, halls_of_stone
    AddSC_halls_of_stone();
    AddSC_instance_halls_of_stone();
    AddSC_boss_assembly_of_iron();                          // ulduar, ulduar
    AddSC_boss_algalon();
    AddSC_boss_auriaya();
    AddSC_boss_flame_leviathan();
    AddSC_boss_freya();
    AddSC_boss_general_vezax();
    AddSC_boss_hodir();
    AddSC_boss_ignis();
    AddSC_boss_kologarn();
    AddSC_boss_mimiron();
    AddSC_boss_razorscale();
    AddSC_boss_thorim();
    AddSC_boss_xt_002();
    AddSC_boss_yogg_saron();
    AddSC_instance_ulduar();
    AddSC_ulduar();
    AddSC_boss_ingvar();                                    // UK, utgarde_keep
    AddSC_boss_keleseth();
    AddSC_instance_utgarde_keep();
    AddSC_utgarde_keep();
    AddSC_boss_gortok();                                    // UK, utgarde_pinnacle
    AddSC_boss_skadi();
    AddSC_boss_svala();
    AddSC_boss_ymiron();
    AddSC_instance_pinnacle();
    AddSC_boss_archavon();                                  // vault_of_archavon
    AddSC_boss_emalon();
    AddSC_boss_koralon();
    AddSC_boss_toravon();
    AddSC_instance_vault_of_archavon();
    AddSC_boss_ichoron();                                   // violet_hold
    AddSC_boss_xevozz();
    AddSC_instance_violet_hold();
    AddSC_violet_hold();

    AddSC_borean_tundra();
    AddSC_dalaran();
    AddSC_dragonblight();
    AddSC_grizzly_hills();
    AddSC_howling_fjord();
    AddSC_icecrown();
    AddSC_sholazar_basin();
    AddSC_storm_peaks();
    AddSC_wintergrasp();
    AddSC_zuldrak();

    // outland
    AddSC_boss_exarch_maladaar();                           // auchindoun, auchenai_crypts
    AddSC_boss_shirrak();
    AddSC_boss_nexusprince_shaffar();                       // auchindoun, mana_tombs
    AddSC_boss_pandemonius();
    AddSC_mana_tombs();
    AddSC_boss_yor();
    AddSC_boss_anzu();                                      // auchindoun, sethekk_halls
    AddSC_boss_darkweaver_syth();
    AddSC_boss_talon_king_ikiss();
    AddSC_instance_sethekk_halls();
    AddSC_boss_ambassador_hellmaw();                        // auchindoun, shadow_labyrinth
    AddSC_boss_blackheart_the_inciter();
    AddSC_boss_grandmaster_vorpil();
    AddSC_boss_murmur();
    AddSC_instance_shadow_labyrinth();
    AddSC_black_temple();                                   // black_temple
    AddSC_boss_illidan();
    AddSC_boss_shade_of_akama();
    AddSC_boss_supremus();
    AddSC_boss_gurtogg_bloodboil();
    AddSC_boss_mother_shahraz();
    AddSC_boss_reliquary_of_souls();
    AddSC_boss_teron_gorefiend();
    AddSC_boss_najentus();
    AddSC_boss_illidari_council();
    AddSC_instance_black_temple();
    AddSC_boss_fathomlord_karathress();                     // CR, serpent_shrine
    AddSC_boss_hydross_the_unstable();
    AddSC_boss_lady_vashj();
    AddSC_boss_leotheras_the_blind();
    AddSC_boss_morogrim_tidewalker();
    AddSC_boss_the_lurker_below();
    AddSC_instance_serpentshrine_cavern();
    AddSC_boss_ahune();                                     // CR, slave_pens
    AddSC_instance_slave_pens();
    AddSC_boss_hydromancer_thespia();                       // CR, steam_vault
    AddSC_boss_mekgineer_steamrigger();
    AddSC_boss_warlord_kalithresh();
    AddSC_instance_steam_vault();
    AddSC_boss_hungarfen();                                 // CR, Underbog
    AddSC_boss_swamplord_muselek();
    AddSC_boss_black_stalker();
    AddSC_instance_underbog();
    AddSC_boss_gruul();                                     // gruuls_lair
    AddSC_boss_high_king_maulgar();
    AddSC_instance_gruuls_lair();
    AddSC_boss_broggok();                                   // HC, blood_furnace
    AddSC_boss_kelidan_the_breaker();
    AddSC_boss_the_maker();
    AddSC_instance_blood_furnace();
    AddSC_boss_nazan_and_vazruden();                        // HC, hellfire_ramparts
    AddSC_boss_omor_the_unscarred();
    AddSC_boss_watchkeeper_gargolmar();
    AddSC_instance_ramparts();
    AddSC_boss_magtheridon();                               // HC, magtheridons_lair
    AddSC_instance_magtheridons_lair();
    AddSC_boss_grand_warlock_nethekurse();                  // HC, shattered_halls
    AddSC_boss_warbringer_omrogg();
    AddSC_boss_warchief_kargath_bladefist();
    AddSC_instance_shattered_halls();
    AddSC_arcatraz();                                       // TK, arcatraz
    AddSC_boss_dalliah();
    AddSC_boss_harbinger_skyriss();
    AddSC_boss_soccothrates();
    AddSC_instance_arcatraz();
    AddSC_boss_high_botanist_freywinn();                    // TK, botanica
    AddSC_boss_thorngrin();
    AddSC_boss_laj();
    AddSC_boss_warp_splinter();
    AddSC_boss_alar();                                      // TK, the_eye
    AddSC_boss_high_astromancer_solarian();
    AddSC_boss_kaelthas();
    AddSC_boss_void_reaver();
    AddSC_instance_the_eye();
    AddSC_boss_mechano_lord_capacitus();                    // TK, the_mechanar
    AddSC_boss_nethermancer_sepethrea();
    AddSC_boss_pathaleon_the_calculator();
    AddSC_instance_mechanar();

    AddSC_blades_edge_mountains();
    AddSC_bashir_landing();
    AddSC_shartuul_transporter();
    AddSC_boss_doomlordkazzak();
    AddSC_boss_doomwalker();
    AddSC_hellfire_peninsula();
    AddSC_nagrand();
    AddSC_netherstorm();
    AddSC_shadowmoon_valley();
    AddSC_shattrath_city();
    AddSC_terokkar_forest();
    AddSC_boss_terokk();
    AddSC_OutlandWorldScript();
    AddSC_zangarmarsh();
}

/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss_Illidan_Stormrage
SD%Complete: 100
SDComment:
SDCategory: Black Temple
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "black_temple.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "AI/ScriptDevAI/base/escort_ai.h"

// #define FAST_TIMERS
// #define NO_SHADOWFIEND
// #define NO_SHEAR

enum
{
    /************* Quotes and Sounds ***********************/
    // Intro yells and gossip
    SAY_AKAMA_COUNCIL_DEAD          = -1564140,
    SAY_AKAMA_BEWARE                = -1564120,
    SAY_AKAMA_OPEN_DOOR_1           = -1564131,
    SAY_AKAMA_OPEN_DOOR_2           = -1564132,
    SAY_UDALO_OPEN_DOOR_3           = -1564133,
    SAY_OLUM_OPEN_DOOR_4            = -1564134,
    SAY_AKAMA_OPEN_DOOR_5           = -1564136,

    // Gossip for when a player clicks Akama
    GOSSIP_ITEM_PREPARE             = -3564001,
    GOSSIP_ITEM_START_EVENT         = -3564002,
    TEXT_ID_AKAMA_ILLIDAN_PREPARE   = 10465,                // ToDo: fix text id - this entry is wrong -> "The time has come to face Illidan, $N. Are you ready?"
    TEXT_ID_AKAMA_ILLIDAN_START     = 10835,

    // Event speech
    SAY_ILLIDAN_SPEECH_1            = -1564097,
    SAY_AKAMA_SPEECH_2              = -1564098,
    SAY_ILLIDAN_SPEECH_3            = -1564099,
    SAY_AKAMA_SPEECH_4              = -1564100,
    SAY_ILLIDAN_SPEECH_5            = -1564101,             // aggro
    SAY_ILLIDAN_MINION              = -1564121,
    SAY_AKAMA_LEAVE                 = -1564122,
    SAY_ILLIDAN_SPEECH_6            = -1564102,
    SAY_MAIEV_SPEECH_7              = -1564103,
    SAY_ILLIDAN_SPEECH_8            = -1564104,
    SAY_MAIEV_SPEECH_9              = -1564105,
    SAY_MAIEV_TRAP                  = -1564118,
    SAY_MAIEV_FALL                  = -1564141, // unused atm

    // Epilogue speech
    SAY_MAIEV_EPILOGUE_1            = -1564107,
    SAY_ILLIDAN_EPILOGUE_2          = -1564108,
    SAY_MAIEV_EPILOGUE_3            = -1564109,
    SAY_MAIEV_EPILOGUE_4            = -1564110,
    SAY_AKAMA_EPILOGUE_5            = -1564111,

    // Combat yells
    SAY_KILL1                       = -1564123,
    SAY_KILL2                       = -1564124,
    SAY_TAKEOFF                     = -1564125, // phase 2 start
    SAY_SUMMONFLAMES                = -1564126,
    SAY_EYE_BLAST                   = -1564127,
    SAY_MORPH                       = -1564128,
    SAY_FRENZY                      = -1564106,
    SAY_BERSERK                     = -1564129,

    // Note: this yells may not be used. Need additional research
    SAY_TAUNT_1                     = -1564112,
    SAY_TAUNT_2                     = -1564113,
    SAY_TAUNT_3                     = -1564114,
    SAY_TAUNT_4                     = -1564115,

    SAY_MAIEV_TAUNT_1               = -1564116,
    SAY_MAIEV_TAUNT_2               = -1564117,
    SAY_MAIEV_TAUNT_3               = -1564119,


    /************** Spells *************/
    // Normal Form
    SPELL_SHEAR                     = 41032,                // Reduces Max. Health by 60% for 7 seconds. Can stack 19 times. 1.5 second cast
    SPELL_FLAME_CRASH               = 40832,                // Summons an invis/unselect passive mob that has an uiAura of flame in a circle around him.
    SPELL_DRAW_SOUL                 = 40904,                // 5k Shadow Damage in front of him. Heals Illidan for 100k health (script effect)
    SPELL_PARASITIC_SHADOWFIEND_BOSS= 41917,                // DoT of 3k Shadow every 2 seconds. Lasts 10 seconds. (Script effect: Summon 2 parasites once the debuff has ticked off)
    SPELL_SUMMON_PARASITICS         = 41915,                // Summons 2 Parasitic Shadowfiends on the target. Handled in core.
    SPELL_AGONIZING_FLAMES          = 40834,                // triggers 40932
    SPELL_FRENZY                    = 40683,                // Increases damage by 50% and attack speed by 30%. 20 seconds, PHASE 5 ONLY

    // Flying (Phase 2)
    SPELL_THROW_GLAIVE              = 39635,                // triggers 41466 - Throws the first glaive on the ground
    SPELL_THROW_GLAIVE_VISUAL       = 39849,                // triggers 41466 - Throws the second glaive on the ground
    SPELL_GLAIVE_RETURNS            = 39873,                // Glaive flies back to Illidan
    SPELL_FIREBALL                  = 40598,                // 2.5k-3.5k damage in 10 yard radius. 2 second cast time.
    SPELL_DARK_BARRAGE              = 40585,                // 10 second channeled spell, 3k shadow damage per second.
    SPELL_EYE_BLAST_DUMMY           = 39908,                // This does the blue beam channel - targets 23070

    // Demon Form
    SPELL_DEMON_TRANSFORM_1         = 40511,                // start transform animation - spell sequence: 40398, 40506, 40510 - handled in core
    SPELL_DEMON_TRANSFORM_2         = 40398,                // Second uiPhase of animations (kneel)
    SPELL_DEMON_TRANSFORM_3         = 40510,                // Final uiPhase of animations (stand up and roar)
    SPELL_DEMON_FORM                = 40506,                // Transforms into Demon Illidan. Has an Aura of Dread on him.
    SPELL_SHADOW_BLAST              = 41078,                // 8k - 11k Shadow Damage. Targets highest threat. Has a splash effect, damaging anyone in 20 yards of the target.
    SPELL_FLAME_BURST               = 41126,                // triggers 41131
    SPELL_SUMMON_SHADOW_DEMONS      = 41117,                // summons 23375

    // Other Illidan spells
    SPELL_KNEEL_INTRO               = 39656,                // Before beginning encounter, this is how he appears (talking to Wilson).
    SPELL_SUMMMON_MAIEV             = 40403,                // summons 23197
    SPELL_TELEPORT_MAIEV            = 41221,
    SPELL_SHADOW_PRISON             = 40647,                // Illidan casts this spell to immobilize entire raid when he summons Maiev.
    SPELL_CAGE_TRAP                 = 40693,                // Cast by Illidan on Maiev - teleports Maiev for the trap
    SPELL_DEATH                     = 41218,                // This spell doesn't do anything except stun Illidan and set him on his knees.
    SPELL_BERSERK                   = 45078,                // Damage increased by 500%, attack speed by 150%
    SPELL_PASSIVE_HIT               = 43689,                // applied on spawn, provides 20% hit.
    SPELL_EMOTE_TALK_QUESTION       = 41616,                // used by illidan during P5 transition


    /************** Non-Illidan Spells *************/
    // Akama
    SPELL_AKAMA_DOOR_FAIL           = 41271,                // Akama's first door attempt
    SPELL_AKAMA_DOOR_CHANNEL        = 41268,                // Akama's channel spell on the door before the Temple Summit
    SPELL_DEATHSWORN_DOOR_CHANNEL   = 41269,                // Olum and Udalo's channel spell on the door before the Temple Summit
    SPELL_HEALING_POTION            = 40535,                // Akama uses this to heal himself to full.
    SPELL_CHAIN_LIGHTNING           = 40536,
    SPELL_AKAMA_DESPAWN             = 41242,                // Akama despawn spell at end of outro

    // Door Trigger
    SPELL_ARCANE_EXPL_VISUAL        = 35426,

    // Maiev
    SPELL_SHADOW_STRIKE             = 40685,
    SPELL_THROW_DAGGER              = 41152,
    SPELL_CAGE_TRAP_SUMMON          = 40694,                // summons npc 23304 and go 185916
    SPELL_TELEPORT_VISUAL           = 41236,
    SPELL_MAIEV_DOWN                = 40409,                // cast when death prevention kicks in on maiev

    // Misc Summoned
    SPELL_FLAME_CRASH_EFFECT        = 40836,                // Firey blue ring of circle that the other flame crash summons
    SPELL_EYE_BLAST_TRIGGER         = 40017,                // This summons Demon Form every few seconds and deals ~20k damage in its radius
    // SPELL_DEMON_FIRE             = 40029,                // Blue fire trail left by Eye Blast. Deals 2k per second if players stand on it.
    SPELL_BLAZE_EFFECT              = 40610,                // Green flame on the ground, triggers damage (5k) every few seconds
    SPELL_INSTANT_BLADE_BIRTH       = 40031,                // Makes the blades appear instantly with no fade in

    // Blade of Azzinoth
    SPELL_RANGE_MARKER              = 41997,                // Dummy effect used by the Blade of Azzinoth to check the range of the Azzinoth flame - needs core support
    SPELL_SUMMON_TEAR_AZZINOTH      = 39855,                // Summons 22997
    SPELL_AZZINOTH_CHANNEL          = 39857,                // Glaives cast it on Flames

    // Flame of Azzinoth
    SPELL_FLAME_BLAST               = 40631,                // Flames of Azzinoth use this. Frontal cone AoE 7k-9k damage.
    SPELL_CHARGE                    = 42003,                // Flames of Azzinoth charges whoever is too far from them. They enrage after this
    SPELL_UNCAGED_WRATH             = 39869,
    SPELL_BLAZE                     = 40637,                // summons 23259

    // Parasitic Shadowfiend
    SPELL_PARASITIC_SHADOWFIEND_P   = 41913,
    SPELL_PARASITIC_SHADOWFIEND_ADD = 41914,
    SPELL_SHADOWFORM_PARASITE       = 34429,

    // Shadow Demon
    SPELL_SHADOW_DEMON_PASSIVE      = 41079,                // Adds the "shadowform" uiAura to Shadow Demons.
    SPELL_CONSUME_SOUL              = 41080,                // Once the Shadow Demons reach their target, they use this to kill them
    SPELL_PARALYZE                  = 41083,                // Shadow Demons cast this on their target

    // Cage spells
    SPELL_CAGE_TRAP_PERIODIC        = 40760,                // first periodic which triggers visuals and dmg taken spell
    SPELL_CAGE_TRAP_DUMMY           = 40761,                // cast on illidan when trap sprung
    SPELL_CAGED                     = 40695,                // Caged Trap triggers will cast this on Illidan if he is within 3 yards

    // Suicide spell
    SPELL_QUIET_SUICIDE             = 3617,


    /************** Creature Summons **************/
    /*NPC_ILLIDARI_ELITE              = 23226,                // attacks Akama on the stairs
    NPC_FLAME_CRASH                 = 23336,                // has aura 40836
    NPC_PARASITIC_SHADOWFIEND       = 23498,                // has aura 41913 (in c_t_a)
    NPC_BLADE_OF_AZZINOTH           = 22996,                // has aura 41997 and summons 22997 on spawn
    NPC_FLAME_OF_AZZINOTH           = 22997,
    NPC_ILLIDAN_TARGET              = 23070,                // the eye blast target - has aura 40017
    NPC_DEMON_FIRE                  = 23069,                // has aura 40029 (in EventAI)
    NPC_BLAZE                       = 23259,                // has aura 40610
    NPC_SHADOW_DEMON                = 23375,
    NPC_CAGE_TRAP_DISTURB_TRIGGER   = 23304,*/

    NPC_CAGE_TRAP_TRIGGER_1         = 23292,
    NPC_CAGE_TRAP_TRIGGER_2         = 23293,
    NPC_CAGE_TRAP_TRIGGER_3         = 23294,
    NPC_CAGE_TRAP_TRIGGER_4         = 23295,
    NPC_CAGE_TRAP_TRIGGER_5         = 23296,
    NPC_CAGE_TRAP_TRIGGER_6         = 23297,
    NPC_CAGE_TRAP_TRIGGER_7         = 23298,
    NPC_CAGE_TRAP_TRIGGER_8         = 23299,

    GO_CAGE_TRAP                    = 185916,

    /************** Others **************/
    EQUIP_ID_MAIN_HAND              = 32837,
    EQUIP_ID_OFF_HAND               = 32838,

    MAX_ILLIDARI_ELITES             = 10,
    MAX_CAGE_SPELLS                 = 8,
    MAX_FLAME_AZZINOTH              = 2,

    DUMMY_EMOTE_ID_1                = 1,
    DUMMY_EMOTE_ID_2                = 2,
    DUMMY_EMOTE_ID_3                = 3,
    DUMMY_EMOTE_ID_4                = 4,
    DUMMY_EMOTE_ID_5                = 7,

    POINT_ILLIDAN_FLIGHT            = 0,
    POINT_ILLIDAN_FLIGHT_RANDOM     = 1,
    POINT_ILLIDAN_LANDING           = 2,

    POINT_AKAMA_ILLIDAN_CLOSE       = 1,
    POINT_AKAMA_COUNCIL_DEAD_END    = 2,
    POINT_AKAMA_DOOR_STOP           = 8,
    POINT_AKAMA_ILLIDAN_STOP        = 5,
    POINT_AKAMA_ILLDARI             = 7,
    POINT_AKAMA_BACK_UP             = 6,

    PATH_ID_AKAMA_COUNCIL_DEAD      = 1,
    PATH_ID_AKAMA_COUNCIL_PRE_DOOR  = 2,
    PATH_ID_AKAMA_COUNCIL_POST_DOOR = 3,
    PATH_ID_AKAMA_ILLIDAN           = 4,
    PATH_ID_AKAMA_FIGHT_ILLIDARI    = 5,
    PATH_ID_AKAMA_BACK_UP           = 6,

    SOUND_KIT_AKAMA_CHANNEL_DOOR    = 11717,
    SOUND_KIT_ILLIDAN_AGGRO         = 11725,
    SOUND_KIT_ILLIDAN_90            = 11726,
    SOUND_KIT_ILLIDAN_TAKEOFF       = 11727,
    SOUND_KIT_ILLIDAN_P3            = 11728,
    SOUND_KIT_ILLIDAN_P5            = 11729,
};

static const uint32 aCagedSummonSpells[MAX_CAGE_SPELLS] = { 40696, 40697, 40698, 40699, 40700, 40701, 40702, 40703 };
static const uint32 aCagedVisualSpells[MAX_CAGE_SPELLS] = { 40704, 40707, 40708, 40709, 40710, 40711, 40712, 40713 };

static const DialogueEntry aIntroDialogue[] =
{
    {SAY_AKAMA_COUNCIL_DEAD,    NPC_AKAMA,              0},
    {DUMMY_EMOTE_ID_1,          0,                      5000},
    {SAY_AKAMA_OPEN_DOOR_1,     NPC_AKAMA,              3500},
    {SPELL_AKAMA_DOOR_FAIL,     0,                      10000},
    {SAY_AKAMA_OPEN_DOOR_2,     NPC_AKAMA,              6000},
    {NPC_SPIRIT_OF_OLUM,        0,                      1500},
    {SAY_UDALO_OPEN_DOOR_3,     NPC_SPIRIT_OF_UDALO,    6000},
    {SAY_OLUM_OPEN_DOOR_4,      NPC_SPIRIT_OF_OLUM,     8000},
    {SPELL_AKAMA_DOOR_CHANNEL,  0,                      11000},
    {GO_ILLIDAN_GATE,           0,                      6500},
    {SAY_AKAMA_OPEN_DOOR_5,     NPC_AKAMA,              8000},
    {EMOTE_ONESHOT_SALUTE,      0,                      3000},
    {NPC_SPIRIT_OF_UDALO,       0,                      0},
    {0, 0, 0},
};

static const DialogueEntry aEventDialogue[] =
{
    // Akama intro
    {NPC_AKAMA,                 0,                      2000},
    {SAY_ILLIDAN_SPEECH_1,      NPC_ILLIDAN_STORMRAGE,  3000},
    {EMOTE_ONESHOT_QUESTION,    0,                      3000},
    {DUMMY_EMOTE_ID_1,          0,                      3000},
    {DUMMY_EMOTE_ID_2,          0,                      3000},
    {SAY_AKAMA_SPEECH_2,        NPC_AKAMA,              10000},
    {SAY_ILLIDAN_SPEECH_3,      NPC_ILLIDAN_STORMRAGE,  3000},
    {DUMMY_EMOTE_ID_3,          0,                      4000},
    {SAY_AKAMA_SPEECH_4,        NPC_AKAMA,              4000},
    {SAY_ILLIDAN_SPEECH_5,      NPC_ILLIDAN_STORMRAGE,  1000},
    {EQUIP_ID_MAIN_HAND,        0,                      4000},
    {NPC_ILLIDAN_STORMRAGE,     0,                      0},
    // Akama leaves fight
    {SAY_ILLIDAN_MINION,        NPC_ILLIDAN_STORMRAGE,  8000},
    {SAY_AKAMA_LEAVE,           NPC_AKAMA,              0},
    // Maiev cutscene
    {DUMMY_EMOTE_ID_4,          0,                      1500},
    {SAY_ILLIDAN_SPEECH_6,      NPC_ILLIDAN_STORMRAGE,  7500},
    {SPELL_SUMMMON_MAIEV,       0,                      1000},
    {SAY_MAIEV_SPEECH_7,        NPC_MAIEV_SHADOWSONG,   2500},
    {EMOTE_ONESHOT_EXCLAMATION, 0,                      6000},
    {SAY_ILLIDAN_SPEECH_8,      NPC_ILLIDAN_STORMRAGE,  4500},
    {EMOTE_STATE_READY1H,       0,                      2000},
    {SAY_MAIEV_SPEECH_9,        NPC_MAIEV_SHADOWSONG,   2500},
    {EMOTE_ONESHOT_YES,         0,                      2500},
    {EMOTE_ONESHOT_ROAR,        0,                      2500},
    {DUMMY_EMOTE_ID_5,          0,                      1000},
    {NPC_MAIEV_SHADOWSONG,      0,                      0},
    {0, 0, 0},
};

static const DialogueEntry aEpilogueDialogue[] =
{
    {SAY_MAIEV_EPILOGUE_1,      NPC_MAIEV_SHADOWSONG,   6000},
    {SAY_ILLIDAN_EPILOGUE_2,    NPC_ILLIDAN_STORMRAGE,  18000},
    {NPC_ILLIDAN_STORMRAGE,     0,                      2000},
    {SAY_MAIEV_EPILOGUE_3,      NPC_MAIEV_SHADOWSONG,   13000},
    {SAY_MAIEV_EPILOGUE_4,      NPC_MAIEV_SHADOWSONG,   2000},
    {SPELL_TELEPORT_VISUAL,     0,                      0},
    {0, 0, 0},
};

/*** Phase Names ***/
enum IllidanPhase
{
    PHASE_1_AKAMA,
    PHASE_2_FLIGHT,
    PHASE_3_NORMAL,
    PHASE_4_DEMON,
    PHASE_5_MAIEV,
    PHASE_TRANSITION,
};

enum PhaseTransition
{
    PHASETRANSITION_NONE,
    PHASETRANSITION_LIFTOFF,
    PHASETRANSITION_LAND,
    PHASETRANSITION_MAIEV,
};

struct Locations
{
    float fX, fY, fZ;
};

static const Locations aCenterLoc[] =
{
    {676.648f, 304.7607f, 354.1909f},           // Landing location
};

static const Locations aIllidariElitesPos[MAX_ILLIDARI_ELITES] =
{
    {743.9686f, 289.6447f, 311.1807f},
    {753.8425f, 286.562f, 310.9353f},
    {745.2552f, 322.1574f, 310.4596f},
    {745.3237f, 283.986f, 309.2765f},
    {750.0472f, 282.3274f, 309.4353f},
    {747.0576f, 326.4268f, 309.0688f},
    {751.0878f, 327.6505f, 309.4576f},
    {748.8422f, 288.062f, 310.9782f},
    {750.0322f, 323.6064f, 310.2757f},
    {754.0332f, 325.8136f, 310.3195f},
};

static const Locations illidanFlightPos[] =
{
    { 658.8175f, 266.0992f, 370.0833f},
    { 705.2382f, 273.1146f, 370.0833f},
    { 657.8459f, 343.9125f, 370.0833f},
    { 705.9218f, 336.1461f, 370.0815f},
};

static const Locations aEyeBlastPos[] =
{
    // spawn
    {642.7164f, 305.2436f, 353.5596f},
    {652.105f,  259.5127f, 353.0794f},
    {710.8815f, 306.4028f, 353.5962f},
    {708.8524f, 342.2623f, 353.0794f},
};

enum IllidanActions
{
    ILLIDAN_ACTION_BERSERK,
    ILLIDAN_ACTION_AKAMA,
    ILLIDAN_ACTION_PHASE_2,
    ILLIDAN_ACTION_PHASE_3,
    ILLIDAN_ACTION_PHASE_5,
    ILLIDAN_ACTION_TRANSFORM,
    ILLIDAN_ACTION_TRAP,
    ILLIDAN_ACTION_ENRAGE,
    ILLIDAN_ACTION_FLAME_BURST,
    ILLIDAN_ACTION_SHADOW_DEMON,
    ILLIDAN_ACTION_SHADOW_BLAST,
    ILLIDAN_ACTION_AGONISING_FLAMES,
    ILLIDAN_ACTION_EYE_BLAST,
    ILLIDAN_ACTION_DARK_BARRAGE,
    ILLIDAN_ACTION_FIREBALL,
    ILLIDAN_ACTION_SHADOW_FIEND,
    ILLIDAN_ACTION_FLAME_CRASH,
    ILLIDAN_ACTION_SHEAR,
    ILLIDAN_ACTION_DRAW_SOUL,
    ILLIDAN_ACTIONS_MAX,
    ILLIDAN_ACTION_PHASE_TRANSITION,
};

/*######
## boss_illidan_stormrage
######*/

struct boss_illidan_stormrageAI : public CombatAI, private DialogueHelper
{
    boss_illidan_stormrageAI(Creature* creature) : CombatAI(creature, ILLIDAN_ACTIONS_MAX),
        DialogueHelper(aEventDialogue), m_instance(static_cast<instance_black_temple*>(creature->GetInstanceData()))
    {
        //TODO: Review timers
        AddCombatAction(ILLIDAN_ACTION_BERSERK, 1500000u); // 25 minutes
        AddTimerlessCombatAction(ILLIDAN_ACTION_AKAMA, true);
        AddTimerlessCombatAction(ILLIDAN_ACTION_PHASE_2, true);
        AddTimerlessCombatAction(ILLIDAN_ACTION_PHASE_3, false);
        AddTimerlessCombatAction(ILLIDAN_ACTION_PHASE_5, true);
        AddCombatAction(ILLIDAN_ACTION_TRANSFORM, true);
        AddCombatAction(ILLIDAN_ACTION_ENRAGE, true);
        AddCombatAction(ILLIDAN_ACTION_TRAP, true);
        AddCombatAction(ILLIDAN_ACTION_SHADOW_DEMON, true);
        AddCombatAction(ILLIDAN_ACTION_FLAME_BURST, true);
        AddCombatAction(ILLIDAN_ACTION_SHADOW_BLAST, true);
        AddCombatAction(ILLIDAN_ACTION_AGONISING_FLAMES, true);
        AddCombatAction(ILLIDAN_ACTION_EYE_BLAST, true);
        AddCombatAction(ILLIDAN_ACTION_DARK_BARRAGE, true);
        AddCombatAction(ILLIDAN_ACTION_FIREBALL, true);
        AddCombatAction(ILLIDAN_ACTION_SHADOW_FIEND, true);
        AddCombatAction(ILLIDAN_ACTION_FLAME_CRASH, true);
        AddCombatAction(ILLIDAN_ACTION_SHEAR, true);
        AddCombatAction(ILLIDAN_ACTION_DRAW_SOUL, true);
        AddCustomAction(ILLIDAN_ACTION_PHASE_TRANSITION, true, [&]() { HandlePhaseTransition(); });

        SetDeathPrevention(true);
        if (m_instance)
        {
            m_creature->GetCombatManager().SetLeashingCheck([](Unit* unit, float /*x*/, float /*y*/, float z)
            {
                return static_cast<ScriptedInstance*>(unit->GetInstanceData())->GetPlayerInMap(true, false) == nullptr || z < 352.0f;
            });
        }
        InitializeDialogueHelper(m_instance);
        AddOnKillText(SAY_KILL1, SAY_KILL2);
        Reset();
    }

    instance_black_temple* m_instance;

    IllidanPhase m_phase;

    float m_targetMoveX, m_targetMoveY, m_targetMoveZ;

    uint8 m_flameAzzinothKilled;

    IllidanPhase m_prevPhase;                             // store the previous phase in transition
    PhaseTransition m_currentTransition;                  // store the current transition between phases
    uint32 m_phaseTransitionStage;
    uint8 m_curEyeBlastLoc;
    ObjectGuid m_curEyeBlastTarget;
    uint32 m_flameBlasts;

    GuidList m_bladesGuidList;

    GuidVector m_cageTrapVisualGuids;

    void Reset() override
    {
        CombatAI::Reset();

        m_phase               = PHASE_1_AKAMA;
        m_currentTransition   = PHASETRANSITION_NONE;
        m_flameAzzinothKilled = 0;
        
        SetMeleeEnabled(true);
        SetRangedMode(false, 0.f, TYPE_NONE);

        m_bladesGuidList.clear();

        // Reset boss
        m_creature->SetHover(false);
        m_creature->SetLevitate(false);
        m_creature->SetImmobilizedState(false);
        SetCombatMovement(true);
        m_creature->SetWalk(false, true);
        SetCombatScriptStatus(false);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        m_creature->SetSheath(SHEATH_STATE_UNARMED);
        m_creature->HandleEmoteState(0);
        DoCastSpellIfCan(nullptr, SPELL_KNEEL_INTRO, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_PASSIVE_HIT, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    uint32 GetInitialActionTimer(IllidanActions id)
    {
        switch (id)
        {
            case ILLIDAN_ACTION_TRANSFORM: return 60000;
            case ILLIDAN_ACTION_ENRAGE: return 40000;
            case ILLIDAN_ACTION_TRAP: return 30000;
            case ILLIDAN_ACTION_SHADOW_DEMON: return 20000;
            case ILLIDAN_ACTION_FLAME_BURST: return 7000;
            case ILLIDAN_ACTION_SHADOW_BLAST: return urand(1000, 2000);
            case ILLIDAN_ACTION_AGONISING_FLAMES: return 25000;
            case ILLIDAN_ACTION_EYE_BLAST: return 10000;
            case ILLIDAN_ACTION_DARK_BARRAGE: return 77000;
            case ILLIDAN_ACTION_FIREBALL: return 0;
            case ILLIDAN_ACTION_SHADOW_FIEND: return 25000;
            case ILLIDAN_ACTION_FLAME_CRASH: return urand(25000, 30000);
            case ILLIDAN_ACTION_SHEAR: return 10000;
            case ILLIDAN_ACTION_DRAW_SOUL: return 32000;
            default: return 0;
        }
    }

    void GetAIInformation(ChatHandler& reader) override
    {
        reader.PSendSysMessage("Boss Illidan, current uiPhase = %u", m_phase);
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_ILLIDAN, IN_PROGRESS);
        m_creature->PlayMusic(SOUND_KIT_ILLIDAN_AGGRO);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_ILLIDAN, FAIL);
    }

    void JustRespawned() override
    {
        CombatAI::JustRespawned();
        DisableDialogue();
    }

    void CorpseRemoved(uint32& respawnDelay) override
    {
        // Respawn after 10 seconds
        if (m_instance->GetData(TYPE_ILLIDAN) == FAIL)
            respawnDelay = 10;
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance->GetData(TYPE_ILLIDAN) != FAIL) // akama died
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

            if (m_instance)
                m_instance->SetData(TYPE_ILLIDAN, DONE);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        if (eventType == AI_EVENT_CUSTOM_A) // encounter wipe
        {
            m_creature->ForcedDespawn();
            if (Creature* akama = m_instance->GetSingleCreatureFromStorage(NPC_AKAMA))
                akama->ForcedDespawn();
            if (Creature* akama = m_instance->GetSingleCreatureFromStorage(NPC_MAIEV_SHADOWSONG))
                akama->ForcedDespawn();
            DespawnGuids(m_instance->GetIllidanSpawns());
        }
        else if (eventType == AI_EVENT_CUSTOM_B) // Demon Transform 1 aura
        {
            if (miscValue == 1)
                m_creature->CastSpell(nullptr, SPELL_DEMON_TRANSFORM_2, TRIGGERED_OLD_TRIGGERED);
            else if (miscValue == 2)
            {
                if (m_phase != PHASE_4_DEMON)
                    DoScriptText(SAY_MORPH, m_creature);
            }
        }
        else if (eventType == AI_EVENT_CUSTOM_C) // Demon Transform 2 aura
        {
            if (m_phase != PHASE_4_DEMON)
            {
                if (miscValue == 2)
                    m_creature->CastSpell(nullptr, SPELL_DEMON_FORM, TRIGGERED_OLD_TRIGGERED);
                else if (miscValue == 3)
                    m_creature->CastSpell(nullptr, SPELL_DEMON_TRANSFORM_3, TRIGGERED_OLD_TRIGGERED);
            }
            else
            {
                if (miscValue == 1)
                    m_creature->RemoveAurasDueToSpell(SPELL_DEMON_FORM);
                else if (miscValue == 2)
                    m_creature->CastSpell(nullptr, SPELL_DEMON_TRANSFORM_3, TRIGGERED_OLD_TRIGGERED);
                else if (miscValue == 3)
                    SetEquipmentSlots(true);
            }
        }
        else if (eventType == AI_EVENT_CUSTOM_D) // Demon Transform 3 aura end
        {
            m_creature->RemoveAurasDueToSpell(SPELL_DEMON_TRANSFORM_2);
            if (m_phase != PHASE_4_DEMON)
            {
                m_prevPhase = m_phase;
                m_phase = PHASE_4_DEMON;
            }
            else
            {
                m_phase = m_prevPhase;
                m_creature->CastSpell(nullptr, SPELL_PASSIVE_HIT, TRIGGERED_OLD_TRIGGERED);
            }
            SetCombatScriptStatus(false);
            SetCombatMovement(true);
            SetMeleeEnabled(true);
            SetDeathPrevention(true);
            PreparePhaseTimers();
            DoStartMovement(m_creature->GetVictim());
            // Phase 4 Transition End
            if (Creature* maiev = m_instance->GetSingleCreatureFromStorage(NPC_MAIEV_SHADOWSONG))
                SendAIEvent(AI_EVENT_CUSTOM_A, maiev, maiev, m_phase);
        }
        else if (eventType == AI_EVENT_CUSTOM_E) // End of Eye Blast
        {
            m_creature->SetImmobilizedState(false);
            if (GetActionReadyStatus(ILLIDAN_ACTION_PHASE_3))
                return;
            int32 randVal = urand(1, 2);
            if (randVal == 2)
                randVal = -1;
            m_curEyeBlastLoc = (m_curEyeBlastLoc + randVal + 4) % 4; // make sure he only goes left or right
            SetCombatScriptStatus(true);
            m_creature->GetMotionMaster()->MovePoint(POINT_ILLIDAN_FLIGHT_RANDOM, illidanFlightPos[m_curEyeBlastLoc].fX, illidanFlightPos[m_curEyeBlastLoc].fY, illidanFlightPos[m_curEyeBlastLoc].fZ, FORCED_MOVEMENT_RUN);
        }
        else if (eventType == AI_EVENT_CUSTOM_F)
        {
            m_creature->CastSpell(nullptr, SPELL_CAGED, TRIGGERED_OLD_TRIGGERED);

            // Cast the visual effects
            for (uint32 aCagedSummonSpell : aCagedSummonSpells)
                m_creature->CastSpell(nullptr, aCagedSummonSpell, TRIGGERED_OLD_TRIGGERED);

            for (uint32 i = 0; i < 8; ++i)
                if (Creature* trigger = m_creature->GetMap()->GetCreature(m_cageTrapVisualGuids[i]))
                    trigger->CastSpell(nullptr, aCagedVisualSpells[i], TRIGGERED_NONE);

            m_cageTrapVisualGuids.clear();
        }
    }

    void JustPreventedDeath(Unit* attacker) override
    {
        if (m_phase == PHASE_4_DEMON)
            HandlePhaseBehaviour();
        else
        {
            m_creature->InterruptNonMeleeSpells(true);
            m_creature->StopMoving();
            m_creature->RemoveAllAurasOnDeath();
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->ClearAllReactives();

            DoCastSpellIfCan(nullptr, SPELL_DEATH);
            DoCastSpellIfCan(m_creature, SPELL_TELEPORT_MAIEV, CAST_TRIGGERED);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveIdle();

            // Signal Maiev to start the outro dialogue
            if (m_instance)
            {
                // DespawnGuids(m_instance->GetIllidanSpawns());
                if (Creature* maiev = m_instance->GetSingleCreatureFromStorage(NPC_MAIEV_SHADOWSONG))
                    maiev->AI()->KilledUnit(m_creature);
            }
        }
    }

    void JustDidDialogueStep(int32 entry) override
    {
        switch (entry)
        {
            case NPC_AKAMA:
                m_creature->RemoveAurasDueToSpell(SPELL_KNEEL_INTRO);
                break;
            case EMOTE_ONESHOT_QUESTION:
            case DUMMY_EMOTE_ID_1:
            case DUMMY_EMOTE_ID_2:
            case DUMMY_EMOTE_ID_3:
                //TODO: move to SD2 SQL if emotes can be added
                m_creature->HandleEmote(EMOTE_ONESHOT_QUESTION);
                break;
            case EQUIP_ID_MAIN_HAND:
                m_creature->SetSheath(SHEATH_STATE_MELEE);
                break;
            case NPC_ILLIDAN_STORMRAGE:
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                m_creature->SetInCombatWithZone();
                AttackClosestEnemy();
                PreparePhaseTimers(); // Phase 1 start
                if (m_instance)
                {
                    if (Creature* akama = m_instance->GetSingleCreatureFromStorage(NPC_AKAMA))
                    {
                        akama->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                        akama->AI()->AttackStart(m_creature);
                    }
                }
                break;
            case SAY_AKAMA_LEAVE:
                if (m_instance)
                    // Remove Akama from threat list and allow him to fight the Illidari elites
                    if (Creature* akama = m_instance->GetSingleCreatureFromStorage(NPC_AKAMA))
                        akama->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, akama, akama);
                break;
            case SAY_ILLIDAN_SPEECH_6:
                m_creature->CastSpell(nullptr, SPELL_EMOTE_TALK_QUESTION, TRIGGERED_NONE);
                break;
            case SPELL_SUMMMON_MAIEV:
                DoCastSpellIfCan(nullptr, SPELL_SUMMMON_MAIEV);
                break;
            case EMOTE_ONESHOT_EXCLAMATION:
                if (m_instance)
                    if (Creature* maiev = m_instance->GetSingleCreatureFromStorage(NPC_MAIEV_SHADOWSONG))
                        maiev->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                break;
            case EMOTE_STATE_READY1H:
                m_creature->HandleEmote(EMOTE_STATE_READY1H);
                break;
            case EMOTE_ONESHOT_YES:
            case EMOTE_ONESHOT_ROAR:
                if (m_instance)
                    if (Creature* maiev = m_instance->GetSingleCreatureFromStorage(NPC_MAIEV_SHADOWSONG))
                        maiev->HandleEmote(uint32(entry));
                break;
            case DUMMY_EMOTE_ID_5:
                // Resume combat and attack Maiev
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                SetCombatScriptStatus(false);
                SetCombatMovement(true);
                SetMeleeEnabled(true);
                if (Unit* victim = m_creature->GetVictim())
                {
                    m_creature->SetTarget(victim);
                    DoStartMovement(victim);
                }
                m_phase = PHASE_5_MAIEV;
                PreparePhaseTimers();
                break;
            case NPC_MAIEV_SHADOWSONG:
                if (m_instance)
                    if (Creature* maiev = m_instance->GetSingleCreatureFromStorage(NPC_MAIEV_SHADOWSONG))
                        maiev->AI()->AttackStart(m_creature);
                break;
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_FLAME_CRASH:
                summoned->SetCanEnterCombat(false);
                summoned->AI()->SetCombatMovement(false);
                summoned->CastSpell(nullptr, SPELL_INSTANT_BLADE_BIRTH, TRIGGERED_NONE);
                summoned->AI()->DoCastSpellIfCan(nullptr, SPELL_FLAME_CRASH_EFFECT, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
                break;
            case NPC_BLADE_OF_AZZINOTH:
                summoned->CastSpell(nullptr, SPELL_INSTANT_BLADE_BIRTH, TRIGGERED_NONE);
                summoned->CastSpell(nullptr, SPELL_RANGE_MARKER, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(nullptr, SPELL_SUMMON_TEAR_AZZINOTH, TRIGGERED_OLD_TRIGGERED);
                m_bladesGuidList.push_back(summoned->GetObjectGuid());
                break;
            case NPC_ILLIDAN_TARGET:
                summoned->AI()->SetCombatMovement(false);
                summoned->AI()->SetReactState(REACT_PASSIVE);
                summoned->CastSpell(nullptr, SPELL_EYE_BLAST_TRIGGER, TRIGGERED_OLD_TRIGGERED);
                summoned->SetCanEnterCombat(false);
                DoCastSpellIfCan(summoned, SPELL_EYE_BLAST_DUMMY);
                break;
            case NPC_SHADOW_DEMON:
                break;
            case NPC_MAIEV_SHADOWSONG:
                summoned->SetFacingToObject(m_creature);
                m_creature->SetFacingToObject(summoned);
                m_creature->SetTarget(summoned);
                break;
            case NPC_CAGE_TRAP_TRIGGER_1:
            case NPC_CAGE_TRAP_TRIGGER_2:
            case NPC_CAGE_TRAP_TRIGGER_3:
            case NPC_CAGE_TRAP_TRIGGER_4:
            case NPC_CAGE_TRAP_TRIGGER_5:
            case NPC_CAGE_TRAP_TRIGGER_6:
            case NPC_CAGE_TRAP_TRIGGER_7:
            case NPC_CAGE_TRAP_TRIGGER_8:
                m_cageTrapVisualGuids.push_back(summoned->GetObjectGuid());
                break;
        }
    }

    void HandlePhaseBehaviour()
    {
        switch (m_phase)
        {
            case PHASE_5_MAIEV:
            {
                DisableCombatAction(ILLIDAN_ACTION_ENRAGE);
                DisableCombatAction(ILLIDAN_ACTION_TRAP);
                // [[fallthrough]]
            }
            case PHASE_3_NORMAL:
            {
                m_flameBlasts = 0;
                DoCastSpellIfCan(nullptr, SPELL_DEMON_TRANSFORM_1);
                AddMainSpell(SPELL_SHADOW_BLAST);
                SetRangedMode(true, 80.f, TYPE_FULL_CASTER);
                SetCombatScriptStatus(true);
                SetMeleeEnabled(false);
                SetCombatMovement(false);
                SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                m_creature->RemoveAurasDueToSpell(SPELL_PASSIVE_HIT);
                DoResetThreat();
                break;
            }
            case PHASE_4_DEMON:
            {
                DoCastSpellIfCan(nullptr, SPELL_DEMON_TRANSFORM_1, CAST_INTERRUPT_PREVIOUS);
                SetRangedMode(false, 0.f, TYPE_NONE);
                SetCombatScriptStatus(true);
                SetMeleeEnabled(false);
                SetCombatMovement(false);
                DoResetThreat();
                break;
            }
            case PHASE_2_FLIGHT:
            {
                m_currentTransition = PHASETRANSITION_LAND;
                m_phaseTransitionStage = 0;
                SetCombatScriptStatus(true);
                ResetTimer(ILLIDAN_ACTION_PHASE_TRANSITION, 1000u);
                break;
            }
            case PHASE_1_AKAMA:
            {
                //Do transition movement/summons
                m_currentTransition = PHASETRANSITION_LIFTOFF;
                m_phaseTransitionStage = 0;
                SetCombatScriptStatus(true);
                ResetTimer(ILLIDAN_ACTION_PHASE_TRANSITION, 1000u);
                break;
            }
            default: break;
        }
    }

    void PreparePhaseTimers() // called on entering specific phase
    {
        switch (m_phase)
        {
            case PHASE_1_AKAMA:
            {
                ResetCombatAction(ILLIDAN_ACTION_SHADOW_FIEND, GetInitialActionTimer(ILLIDAN_ACTION_SHADOW_FIEND));
                ResetCombatAction(ILLIDAN_ACTION_FLAME_CRASH, GetInitialActionTimer(ILLIDAN_ACTION_FLAME_CRASH));
                ResetCombatAction(ILLIDAN_ACTION_SHEAR, GetInitialActionTimer(ILLIDAN_ACTION_SHEAR));
                ResetCombatAction(ILLIDAN_ACTION_DRAW_SOUL, GetInitialActionTimer(ILLIDAN_ACTION_DRAW_SOUL));
                break;
            }
            case PHASE_2_FLIGHT:
            {
                DisableCombatAction(ILLIDAN_ACTION_DRAW_SOUL);
                DisableCombatAction(ILLIDAN_ACTION_FLAME_CRASH);
                DisableCombatAction(ILLIDAN_ACTION_SHEAR);
                DisableCombatAction(ILLIDAN_ACTION_SHADOW_FIEND);
                ResetCombatAction(ILLIDAN_ACTION_EYE_BLAST, GetInitialActionTimer(ILLIDAN_ACTION_EYE_BLAST));
                ResetCombatAction(ILLIDAN_ACTION_DARK_BARRAGE, GetInitialActionTimer(ILLIDAN_ACTION_DARK_BARRAGE));
                ResetCombatAction(ILLIDAN_ACTION_FIREBALL, GetInitialActionTimer(ILLIDAN_ACTION_FIREBALL));
                break;
            }
            case PHASE_4_DEMON:
            {
                DisableCombatAction(ILLIDAN_ACTION_DRAW_SOUL);
                DisableCombatAction(ILLIDAN_ACTION_FLAME_CRASH);
                DisableCombatAction(ILLIDAN_ACTION_SHEAR);
                DisableCombatAction(ILLIDAN_ACTION_SHADOW_FIEND);
                DisableCombatAction(ILLIDAN_ACTION_AGONISING_FLAMES);
                ResetCombatAction(ILLIDAN_ACTION_FLAME_BURST, GetInitialActionTimer(ILLIDAN_ACTION_FLAME_BURST));
                ResetCombatAction(ILLIDAN_ACTION_SHADOW_BLAST, GetInitialActionTimer(ILLIDAN_ACTION_SHADOW_BLAST));
                ResetCombatAction(ILLIDAN_ACTION_SHADOW_DEMON, GetInitialActionTimer(ILLIDAN_ACTION_SHADOW_DEMON));
                ResetCombatAction(ILLIDAN_ACTION_TRANSFORM, 48000u);
                break;
            }
            case PHASE_5_MAIEV:
                ResetCombatAction(ILLIDAN_ACTION_ENRAGE, GetInitialActionTimer(ILLIDAN_ACTION_ENRAGE));
                ResetCombatAction(ILLIDAN_ACTION_TRAP, GetInitialActionTimer(ILLIDAN_ACTION_TRAP));
                // [fallthrough]
            case PHASE_3_NORMAL:
            {
                // phase 2 cleanup
                DisableCombatAction(ILLIDAN_ACTION_EYE_BLAST);
                DisableCombatAction(ILLIDAN_ACTION_DARK_BARRAGE);
                DisableCombatAction(ILLIDAN_ACTION_FIREBALL);
                // phase 4 cleanup
                DisableCombatAction(ILLIDAN_ACTION_FLAME_BURST);
                DisableCombatAction(ILLIDAN_ACTION_SHADOW_BLAST);
                DisableCombatAction(ILLIDAN_ACTION_SHADOW_DEMON);
                // phase 3 abilities
                ResetCombatAction(ILLIDAN_ACTION_DRAW_SOUL, GetInitialActionTimer(ILLIDAN_ACTION_DRAW_SOUL));
                ResetCombatAction(ILLIDAN_ACTION_FLAME_CRASH, GetInitialActionTimer(ILLIDAN_ACTION_FLAME_CRASH));
                ResetCombatAction(ILLIDAN_ACTION_SHEAR, GetInitialActionTimer(ILLIDAN_ACTION_SHEAR));
                ResetCombatAction(ILLIDAN_ACTION_SHADOW_FIEND, GetInitialActionTimer(ILLIDAN_ACTION_SHADOW_FIEND));
                ResetCombatAction(ILLIDAN_ACTION_AGONISING_FLAMES, GetInitialActionTimer(ILLIDAN_ACTION_AGONISING_FLAMES));
                ResetCombatAction(ILLIDAN_ACTION_TRANSFORM, GetInitialActionTimer(ILLIDAN_ACTION_TRANSFORM));
                break;
            }
            default: break;
        }
    }

    void HandlePhaseTransition()
    {
        uint32 nextTimer = 0;
        switch (m_currentTransition)
        {
            case PHASETRANSITION_LIFTOFF:
            {
                switch (m_phaseTransitionStage)
                {
                    case 0:
                    {
                        m_creature->RemoveAllAuras();
                        m_creature->GetMotionMaster()->Clear(false, true);
                        m_creature->GetMotionMaster()->MoveIdle();
                        m_creature->HandleEmote(EMOTE_ONESHOT_LIFTOFF);
                        m_creature->SetLevitate(true);
                        m_creature->SetHover(true);
                        m_creature->SetFacingTo(0.659198f);
                        SetCombatMovement(false);
                        SetMeleeEnabled(false);
                        m_creature->SetTarget(nullptr);
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                        nextTimer = 3500;
                        break;
                    }
                    case 1:
                    {
                        DoScriptText(SAY_TAKEOFF, m_creature);
                        nextTimer = 3000;
                        break;
                    }
                    case 2: // picks closest trigger out of the 3 and flies towards it
                    {
                        GuidVector& lowerTriggers = m_instance->GetIllidanTriggersLower();
                        Creature* closestTrigger = nullptr;
                        for (ObjectGuid guid : lowerTriggers)
                        {
                            if (Creature* trigger = m_creature->GetMap()->GetCreature(guid))
                            {
                                if (closestTrigger)
                                {
                                    if (closestTrigger->GetDistance(m_creature, true, DIST_CALC_NONE) > trigger->GetDistance(m_creature, true, DIST_CALC_NONE))
                                        closestTrigger = trigger;
                                }
                                else
                                    closestTrigger = trigger;
                            }
                        }
                        float x, y, z;
                        closestTrigger->GetPosition(x, y, z);
                        m_creature->GetMotionMaster()->MovePoint(POINT_ILLIDAN_FLIGHT, x, y, z, FORCED_MOVEMENT_RUN);
                        break;
                    }
                    case 3:
                    {
                        if (m_instance)
                        {
                            // Need to provide explicit glaive targets
                            GuidVector targets;
                            m_instance->GetGlaiveTargetGuidVector(targets);
                            Creature* glaive1 = m_creature->GetMap()->GetCreature(targets[0]);
                            if (!glaive1)
                                break;
                            // Summon both blades and remove them from equipment
                            DoCastSpellIfCan(glaive1, SPELL_THROW_GLAIVE_VISUAL);
                            nextTimer = 2000;
                        }
                        break;
                    }
                    case 4:
                    {
                        if (m_instance)
                        {
                            // Need to provide explicit glaive targets
                            GuidVector targets;
                            m_instance->GetGlaiveTargetGuidVector(targets);
                            Creature* glaive2 = m_creature->GetMap()->GetCreature(targets[1]);
                            if (!glaive2)
                                break;
                            DoCastSpellIfCan(glaive2, SPELL_THROW_GLAIVE, CAST_TRIGGERED);
                            SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                            m_currentTransition = PHASETRANSITION_NONE;
                            m_phase = PHASE_2_FLIGHT;
                            //uint32 firstEyeBlastPos = 0;
                            //uint32 secondEyeBlastPos = 0;
                            //for (uint32 i = 1; i < 4; ++i)
                            //{
                            //    if (m_creature->GetDistance(illidanFlightPos[firstEyeBlastPos].fX, illidanFlightPos[firstEyeBlastPos].fY, illidanFlightPos[firstEyeBlastPos].fZ, DIST_CALC_NONE)
                            //        > m_creature->GetDistance(illidanFlightPos[i].fX, illidanFlightPos[i].fY, illidanFlightPos[i].fZ, DIST_CALC_NONE))
                            //    {
                            //        if (m_creature->GetDistance(illidanFlightPos[secondEyeBlastPos].fX, illidanFlightPos[secondEyeBlastPos].fY, illidanFlightPos[secondEyeBlastPos].fZ, DIST_CALC_NONE)
                            //            > m_creature->GetDistance(illidanFlightPos[firstEyeBlastPos].fX, illidanFlightPos[firstEyeBlastPos].fY, illidanFlightPos[firstEyeBlastPos].fZ, DIST_CALC_NONE))
                            //        {
                            //            secondEyeBlastPos = firstEyeBlastPos;
                            //        }
                            //        firstEyeBlastPos = i;
                            //    }
                            //    else if (m_creature->GetDistance(illidanFlightPos[secondEyeBlastPos].fX, illidanFlightPos[secondEyeBlastPos].fY, illidanFlightPos[secondEyeBlastPos].fZ, DIST_CALC_NONE)
                            //        > m_creature->GetDistance(illidanFlightPos[i].fX, illidanFlightPos[i].fY, illidanFlightPos[i].fZ, DIST_CALC_NONE))
                            //    {
                            //        secondEyeBlastPos = i;
                            //    }
                            //}
                            // m_curEyeBlastLoc = urand(0, 1) ? firstEyeBlastPos : secondEyeBlastPos;
                            m_curEyeBlastLoc = urand(0, 3);
                            m_creature->GetMotionMaster()->MovePoint(POINT_ILLIDAN_FLIGHT_RANDOM, illidanFlightPos[m_curEyeBlastLoc].fX, illidanFlightPos[m_curEyeBlastLoc].fY, illidanFlightPos[m_curEyeBlastLoc].fZ, FORCED_MOVEMENT_RUN);
                            PreparePhaseTimers();
                        }
                        break;
                    }
                }
                ++m_phaseTransitionStage;
                break;
            }
            case PHASETRANSITION_LAND:
            {
                switch (m_phaseTransitionStage)
                {
                    case 0:
                    {
                        m_creature->SetImmobilizedState(false);
                        m_creature->SetTarget(nullptr);
                        m_creature->GetMotionMaster()->MovePoint(POINT_ILLIDAN_LANDING, aCenterLoc[0].fX, aCenterLoc[0].fY, aCenterLoc[0].fZ, FORCED_MOVEMENT_RUN);
                        break;
                    }
                    case 1:
                    {
                        // Despawn the blades
                        for (GuidList::const_iterator itr = m_bladesGuidList.begin(); itr != m_bladesGuidList.end(); ++itr)
                        {
                            if (Creature* blade = m_creature->GetMap()->GetCreature(*itr))
                            {
                                blade->CastSpell(m_creature, SPELL_GLAIVE_RETURNS, TRIGGERED_OLD_TRIGGERED);
                                blade->ForcedDespawn(500);
                            }
                        }
                        nextTimer = 500;
                        break;
                    }
                    case 2:
                    {
                        m_creature->PlaySpellVisual(7668);
                        nextTimer = 500;
                        break;
                    }
                    case 3:
                    {
                        // Set the equipment and land
                        SetEquipmentSlots(true);
                        nextTimer = 3000;
                        break;
                    }
                    case 4:
                    {
                        m_creature->SetLevitate(false);
                        m_creature->SetHover(false);
                        m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
                        nextTimer = 3000;
                        break;
                    }
                    case 5:
                    {
                        // Start phase 3
                        DoResetThreat();
                        m_phase = PHASE_3_NORMAL;
                        m_currentTransition = PHASETRANSITION_NONE;

                        SetCombatScriptStatus(false);
                        SetCombatMovement(true);
                        SetMeleeEnabled(true);
                        m_creature->SetImmobilizedState(false);
                        if (m_creature->GetVictim())
                        {
                            m_creature->SetTarget(m_creature->GetVictim());
                            DoStartMovement(m_creature->GetVictim());
                        }
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                        nextTimer = 0;
                        PreparePhaseTimers();
                        break;
                    }
                }
                ++m_phaseTransitionStage;
                break;
            }
            case PHASETRANSITION_MAIEV:
            {
                //TODO
                break;
            }
            default: break;
        }
        if (nextTimer)
            ResetTimer(ILLIDAN_ACTION_PHASE_TRANSITION, nextTimer);
    }

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType == POINT_MOTION_TYPE)
        {
            if (pointId == POINT_ILLIDAN_FLIGHT)
            {
                DoScriptText(SAY_SUMMONFLAMES, m_creature);
                m_creature->SetFacingTo(4.670939f); // sets facing to world trigger in sniff - maybe change to this?
                ResetTimer(ILLIDAN_ACTION_PHASE_TRANSITION, 1200u);
            }
            else if (pointId == POINT_ILLIDAN_FLIGHT_RANDOM)
            {
                SetCombatScriptStatus(false);
                m_creature->SetImmobilizedState(true);
            }
            else if (pointId == POINT_ILLIDAN_LANDING)
            {
                m_creature->SetFacingTo(6.230825f);
                ResetTimer(ILLIDAN_ACTION_PHASE_TRANSITION, 1500u);
            }
        }
    }

    void SummonedMovementInform(Creature* summoned, uint32 moveType, uint32 pointId) override
    {
        if (summoned->GetEntry() == NPC_ILLIDAN_TARGET)
        {
            switch (summoned->GetMotionMaster()->GetPathId())
            {
                case 1: if (pointId == 13) summoned->ForcedDespawn(); break;
                case 2: if (pointId == 12) summoned->ForcedDespawn(); break;
                case 3: if (pointId == 13) summoned->ForcedDespawn(); break;
                case 4: if (pointId == 14) summoned->ForcedDespawn(); break;
            }
        }
    }

    // Wrapper to start the combat dialogue
    void DoStartCombatEvent() { StartNextDialogueText(NPC_AKAMA); }

    // Wrapper to land Illidan when both flames are killed
    void DoInformFlameKilled()
    {
        // Land Illidan if both Flames are killed
        ++m_flameAzzinothKilled;

        if (m_flameAzzinothKilled == MAX_FLAME_AZZINOTH)
        {
            SetActionReadyStatus(ILLIDAN_ACTION_PHASE_3, true);
        }
    }

    // Wrapper to handle the Eye Blast cast
    bool DoCastEyeBlastIfCan()
    {
        if (!CanExecuteCombatAction())
            return false;

        DoScriptText(SAY_EYE_BLAST, m_creature);

        // Set spawn and target loc
        m_creature->SummonCreature(NPC_ILLIDAN_TARGET, aEyeBlastPos[m_curEyeBlastLoc].fX, aEyeBlastPos[m_curEyeBlastLoc].fY, aEyeBlastPos[m_curEyeBlastLoc].fZ, 0, TEMPSPAWN_TIMED_DESPAWN, 15000, true, true, m_curEyeBlastLoc + 1);
        return true;
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ILLIDAN_ACTION_BERSERK:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    DisableCombatAction(action);
                }
                return;
            }
            case ILLIDAN_ACTION_AKAMA:
            {
                if (m_creature->GetHealthPercent() > 90.0f)
                    return;
                m_creature->PlayMusic(SOUND_KIT_ILLIDAN_90);
                StartNextDialogueText(SAY_ILLIDAN_MINION);
                SetActionReadyStatus(action, false);
                return;
            }
            case ILLIDAN_ACTION_PHASE_2:
            {
                if (m_creature->GetHealthPercent() > 65.0f)
                    return;
                m_creature->PlayMusic(SOUND_KIT_ILLIDAN_TAKEOFF);
                HandlePhaseBehaviour(); // Phase 2 transition start
                SetActionReadyStatus(action, false);
                return;
            }
            case ILLIDAN_ACTION_PHASE_3:
            {
                m_creature->PlayMusic(SOUND_KIT_ILLIDAN_P3);
                HandlePhaseBehaviour(); // Phase 3 transition start
                SetActionReadyStatus(action, false);
                return;
            }
            case ILLIDAN_ACTION_PHASE_5:
            {
                if (m_creature->GetHealthPercent() > 30.0f)
                    return;
                if (m_phase == PHASE_4_DEMON) // return to normal form first
                {
                    HandlePhaseBehaviour();
                    return;
                }
                if (DoCastSpellIfCan(nullptr, SPELL_SHADOW_PRISON) == CAST_OK) // Phase 5 transition start
                {
                    SetMeleeEnabled(false);
                    m_creature->SetTarget(nullptr);
                    m_creature->PlayMusic(SOUND_KIT_ILLIDAN_P5);
                    StartNextDialogueText(DUMMY_EMOTE_ID_4);
                    SetCombatScriptStatus(true);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

                    SetCombatMovement(false);
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MoveIdle();

                    m_phase = PHASE_TRANSITION;
                    SetActionReadyStatus(action, false);
                }
                return;
            }
            case ILLIDAN_ACTION_TRANSFORM:
            {
                if (m_phase == PHASE_4_DEMON && m_flameBlasts < 3)
                    return;

                HandlePhaseBehaviour(); // Phase 4 transition start
                return;
            }
            case ILLIDAN_ACTION_TRAP:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_CAGE_TRAP) == CAST_OK)
                    ResetCombatAction(action, 45000u);
                return;
            }
            case ILLIDAN_ACTION_ENRAGE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_FRENZY) == CAST_OK)
                {
                    DoScriptText(SAY_FRENZY, m_creature);
                    DisableCombatAction(action);
                }
                return;
            }
            case ILLIDAN_ACTION_SHADOW_DEMON:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_SHADOW_DEMONS) == CAST_OK)
                    DisableCombatAction(action);
                return;
            }
            case ILLIDAN_ACTION_FLAME_BURST:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_FLAME_BURST) == CAST_OK)
                {
                    ResetCombatAction(action, 19500);
                    ++m_flameBlasts;
                }
                return;
            }
            case ILLIDAN_ACTION_SHADOW_BLAST:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOW_BLAST) == CAST_OK)
                    ResetCombatAction(action, 2500u);
                return;
            }
            case ILLIDAN_ACTION_AGONISING_FLAMES:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_AGONIZING_FLAMES) == CAST_OK)
                    ResetCombatAction(action, 24000u);
                return;
            }
            case ILLIDAN_ACTION_EYE_BLAST:
            {
                if (DoCastEyeBlastIfCan())
                {
                    ResetCombatAction(action, urand(37000, 65000));
                    ResetCombatAction(ILLIDAN_ACTION_FIREBALL, 15000u);     // Check this
                }
                return;
            }
            case ILLIDAN_ACTION_DARK_BARRAGE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_DARK_BARRAGE) == CAST_OK)
                        ResetCombatAction(action, urand(43000, 60000));
                return;
            }
            case ILLIDAN_ACTION_FIREBALL:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_FIREBALL) == CAST_OK)
                        ResetCombatAction(action, urand(2500, 3000));
                return;
            }
#ifndef NO_SHADOWFIEND
            case ILLIDAN_ACTION_SHADOW_FIEND:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_PARASITIC_SHADOWFIEND_BOSS, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_PARASITIC_SHADOWFIEND_BOSS) == CAST_OK)
                        ResetCombatAction(action, 25000u);
                return;
            }
#endif // !NO_SHEAR
            case ILLIDAN_ACTION_FLAME_CRASH:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FLAME_CRASH) == CAST_OK)
                    ResetCombatAction(action, urand(26000, 35000));
                return;
            }
#ifndef NO_SHEAR
            case ILLIDAN_ACTION_SHEAR:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHEAR) == CAST_OK)
                    ResetCombatAction(action, urand(12000, 15000));
                return;
            }
#endif // !NO_SHEAR
            case ILLIDAN_ACTION_DRAW_SOUL:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_DRAW_SOUL) == CAST_OK)
                    ResetCombatAction(action, 32000u);
                return;
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        DialogueUpdate(diff);
        CombatAI::UpdateAI(diff);
    }
};

/*######
## npc_akama_illidan
######*/

enum AkamaStages
{
    AKAMA_STAGE_COUNCIL = 0,
    AKAMA_STAGE_ILLIDAN = 1,
};

enum AkamaActions
{
    AKAMA_ACTION_HEAL,
    AKAMA_ACTION_CHAIN_LIGHTNING,
    AKAMA_ACTION_MAX,
    AKAMA_SUMMON_ILLIDARI,
    AKAMA_OUTRO_DELAY,
    AKAMA_OUTRO_ACTIONS,
};

struct npc_akama_illidanAI : public CombatAI, private DialogueHelper
{
    npc_akama_illidanAI(Creature* creature) : CombatAI(creature, AKAMA_ACTION_MAX),
        DialogueHelper(aIntroDialogue), m_instance(static_cast<instance_black_temple*>(creature->GetInstanceData())), m_akamaStage(AKAMA_STAGE_ILLIDAN), m_outroStage(0)
    {
        AddCombatAction(AKAMA_ACTION_CHAIN_LIGHTNING, true);
        AddCombatAction(AKAMA_ACTION_HEAL, 0u);
        AddCustomAction(AKAMA_SUMMON_ILLIDARI, true, [&]()
        {
            for (const Locations& aIllidariElitesPo : aIllidariElitesPos)
                m_creature->SummonCreature(NPC_ILLIDARI_ELITE, aIllidariElitesPo.fX, aIllidariElitesPo.fY, aIllidariElitesPo.fZ, 0, TEMPSPAWN_DEAD_DESPAWN, 0);

            ResetTimer(AKAMA_SUMMON_ILLIDARI, urand(35000, 50000));
        });
        AddCustomAction(AKAMA_OUTRO_DELAY, true, [&]()
        {
            m_creature->SetImmobilizedState(false);
            m_creature->GetMotionMaster()->MoveWaypoint(PATH_ID_AKAMA_BACK_UP);
        });
        AddCustomAction(AKAMA_OUTRO_ACTIONS, true, [&]()
        {
            HandleOutro();
        });
        InitializeDialogueHelper(m_instance);
        m_creature->SetNoThreatState(true);
        SetDeathPrevention(true);
    }

    instance_black_temple* m_instance;

    uint32 m_akamaStage;

    bool m_bFightMinions;
    bool m_bIsIntroFinished;

    GuidVector m_summons;

    uint32 m_outroStage;

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A) // council died
        {
            m_creature->NearTeleportTo(609.772f, 308.456f, 271.826f, 2.373648f);
            m_creature->SetWalk(false);
            m_creature->GetMotionMaster()->MoveWaypoint(PATH_ID_AKAMA_COUNCIL_DEAD);
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            m_akamaStage = AKAMA_STAGE_COUNCIL;
        }
        else if (eventType == AI_EVENT_CUSTOM_B) // go fight illidari downstairs
        {
            SetCombatScriptStatus(true);
            m_creature->CombatStop(true);
            SetReactState(REACT_PASSIVE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            m_creature->GetMotionMaster()->MoveWaypoint(PATH_ID_AKAMA_FIGHT_ILLIDARI);
        }
        else if (eventType == AI_EVENT_CUSTOM_C)
        {
            DespawnGuids(m_summons);
            m_creature->CombatStop(true);
            ResetTimer(AKAMA_OUTRO_DELAY, 2000u);
        }
    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        m_creature->SetImmobilizedState(false);
        m_bFightMinions = false;
        m_bIsIntroFinished = false;
        SetCombatScriptStatus(false);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        SetReactState(REACT_AGGRESSIVE);
        m_instance->DoTeleportAkamaIfCan();
    }

    void JustPreventedDeath(Unit* /*killer*/) override
    {
        if (Creature* illidan = m_instance->GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
            illidan->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, illidan);
    }

    void CorpseRemoved(uint32& respawnDelay) override
    {
        // Respawn after 10 seconds
        if (m_instance->GetData(TYPE_ILLIDAN) == FAIL)
            respawnDelay = 10;
        else if (m_instance->GetData(TYPE_ILLIDAN) == DONE)
            respawnDelay = 720000;
    }

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType == WAYPOINT_MOTION_TYPE)
        {
            switch (m_creature->GetMotionMaster()->GetPathId())
            {
                case PATH_ID_AKAMA_COUNCIL_DEAD:
                {
                    if (pointId == POINT_AKAMA_COUNCIL_DEAD_END)
                    {
                        DoScriptText(SAY_AKAMA_COUNCIL_DEAD, m_creature);
                        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        m_creature->GetMotionMaster()->Clear(false, true);
                        m_creature->GetMotionMaster()->MoveIdle();
                    }
                    break;
                }
                case PATH_ID_AKAMA_COUNCIL_PRE_DOOR:
                {
                    if (pointId == POINT_AKAMA_DOOR_STOP)
                    {
                        m_creature->GetMotionMaster()->Clear(false, true);
                        m_creature->GetMotionMaster()->MoveIdle();
                        if (m_instance)
                        {
                            if (Creature* trigger = m_instance->GetSingleCreatureFromStorage(NPC_ILLIDAN_DOOR_TRIGGER))
                                m_creature->SetFacingToObject(trigger);
                        }
                        StartNextDialogueText(DUMMY_EMOTE_ID_1);
                    }
                    break;
                }
                case PATH_ID_AKAMA_COUNCIL_POST_DOOR:
                {
                    if (pointId == POINT_AKAMA_ILLIDAN_STOP)
                    {
                        m_creature->GetMotionMaster()->Clear(false, true);
                        m_creature->GetMotionMaster()->MoveIdle();
                        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        if (m_instance)
                        {
                            if (Creature* illidan = m_instance->GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
                                m_creature->SetFacingToObject(illidan);
                        }
                        DoScriptText(SAY_AKAMA_BEWARE, m_creature);
                        m_akamaStage = AKAMA_STAGE_ILLIDAN;
                    }
                    break;
                }
                case PATH_ID_AKAMA_ILLIDAN:
                {
                    if (m_instance)
                    {
                        m_creature->GetMotionMaster()->Clear(false, true);
                        m_creature->GetMotionMaster()->MoveIdle();
                        if (Creature* illidan = m_instance->GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
                        {
                            if (boss_illidan_stormrageAI* illidanAI = dynamic_cast<boss_illidan_stormrageAI*>(illidan->AI()))
                                illidanAI->DoStartCombatEvent();

                            m_creature->SetFacingToObject(illidan);
                        }
                    }
                    break;
                }
                case PATH_ID_AKAMA_FIGHT_ILLIDARI:
                {
                    if (pointId == POINT_AKAMA_ILLDARI)
                    {
                        m_creature->GetMotionMaster()->Clear(false, true);
                        m_creature->GetMotionMaster()->MoveIdle();
                        m_bFightMinions = true;
                        ResetTimer(AKAMA_SUMMON_ILLIDARI, 2000);
                        ResetCombatAction(AKAMA_ACTION_CHAIN_LIGHTNING, urand(5000, 10000));
                        SetCombatScriptStatus(false);
                        SetReactState(REACT_AGGRESSIVE);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                        m_creature->SetImmobilizedState(true);
                    }
                    break;
                }
                case PATH_ID_AKAMA_BACK_UP:
                {
                    if (pointId == POINT_AKAMA_BACK_UP)
                    {
                        m_creature->GetMotionMaster()->Clear(false, true);
                        m_creature->GetMotionMaster()->MoveIdle();
                        if (m_instance)
                        {
                            // Move to a close point to Illidan
                            if (Creature* illidan = m_instance->GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
                            {
                                float fX, fY, fZ;
                                illidan->GetContactPoint(m_creature, fX, fY, fZ);
                                m_creature->GetMotionMaster()->MovePoint(POINT_AKAMA_ILLIDAN_CLOSE, fX, fY, fZ, FORCED_MOVEMENT_RUN);
                            }
                        }
                    }
                    break;
                }
            }
        }
        else if (moveType == POINT_MOTION_TYPE && pointId == POINT_AKAMA_ILLIDAN_CLOSE)
            HandleOutro();
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case SPELL_AKAMA_DOOR_FAIL:
                DoCastSpellIfCan(m_creature, SPELL_AKAMA_DOOR_FAIL);
                break;
            case NPC_SPIRIT_OF_OLUM:
                m_creature->SummonCreature(NPC_SPIRIT_OF_OLUM,  751.6437f, 297.2233f, 312.2083f, 6.03f, TEMPSPAWN_TIMED_DESPAWN, 50000);
                m_creature->SummonCreature(NPC_SPIRIT_OF_UDALO, 751.4565f, 311.0107f, 312.19f, 0.0f, TEMPSPAWN_TIMED_DESPAWN, 50000);
                break; 
            case SPELL_AKAMA_DOOR_CHANNEL:
                DoCastSpellIfCan(m_creature, SPELL_AKAMA_DOOR_CHANNEL);
                if (m_instance)
                {
                    if (Creature* olum = m_instance->GetSingleCreatureFromStorage(NPC_SPIRIT_OF_OLUM))
                        olum->CastSpell(nullptr, SPELL_DEATHSWORN_DOOR_CHANNEL, TRIGGERED_OLD_TRIGGERED);
                    if (Creature* udalo = m_instance->GetSingleCreatureFromStorage(NPC_SPIRIT_OF_UDALO))
                        udalo->CastSpell(nullptr, SPELL_DEATHSWORN_DOOR_CHANNEL, TRIGGERED_OLD_TRIGGERED);
                }
                m_creature->PlayMusic(SOUND_KIT_AKAMA_CHANNEL_DOOR);
                break;
            case GO_ILLIDAN_GATE:
                if (m_instance)
                {
                    if (Creature* trigger = m_instance->GetSingleCreatureFromStorage(NPC_ILLIDAN_DOOR_TRIGGER))
                        trigger->CastSpell(trigger, SPELL_ARCANE_EXPL_VISUAL, TRIGGERED_OLD_TRIGGERED);

                    m_instance->DoUseDoorOrButton(GO_ILLIDAN_GATE);
                }
                break;
            case EMOTE_ONESHOT_SALUTE:
                if (m_instance)
                {
                    if (Creature* olum = m_instance->GetSingleCreatureFromStorage(NPC_SPIRIT_OF_OLUM))
                        olum->HandleEmote(EMOTE_ONESHOT_SALUTE);
                    if (Creature* udalo = m_instance->GetSingleCreatureFromStorage(NPC_SPIRIT_OF_UDALO))
                        udalo->HandleEmote(EMOTE_ONESHOT_SALUTE);
                }
                break;
            case NPC_SPIRIT_OF_UDALO:
                m_creature->GetMotionMaster()->MoveWaypoint(PATH_ID_AKAMA_COUNCIL_POST_DOOR);
                break;
            case DUMMY_EMOTE_ID_1:
                break;
        }
    }

    void HandleOutro()
    {
        uint32 timer = 0;
        switch (m_outroStage)
        {
            case 0:
                if (m_instance)
                {
                    if (Creature* illidan = m_instance->GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
                        m_creature->SetFacingToObject(illidan);
                }
                DoScriptText(SAY_AKAMA_EPILOGUE_5, m_creature);
                timer = 3500;
                break;
            case 1:
                m_creature->HandleEmote(EMOTE_ONESHOT_SALUTE);
                timer = 2500;
                break;
            case 2:
                m_creature->HandleEmote(EMOTE_STATE_READY1H);
                timer = 2500;
                break;
            case 3:
                m_creature->CastSpell(nullptr, SPELL_AKAMA_DESPAWN, TRIGGERED_NONE);
                m_creature->ForcedDespawn(1000); // move to spell script when they are nicer
                break;
        }
        ++m_outroStage;
        if (timer)
            ResetTimer(AKAMA_OUTRO_ACTIONS, timer);
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_ILLIDARI_ELITE:
                summoned->AI()->AttackStart(m_creature);
                break;
            case NPC_SPIRIT_OF_OLUM:
            case NPC_SPIRIT_OF_UDALO:
                summoned->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
        }
        m_summons.push_back(summoned->GetObjectGuid());
    }

    // Wrapper to handle event resume
    void DoResumeEvent()
    {
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        if (m_akamaStage == AKAMA_STAGE_COUNCIL)
            m_creature->GetMotionMaster()->MoveWaypoint(PATH_ID_AKAMA_COUNCIL_PRE_DOOR);
        else
        {
            m_creature->SetWalk(false);
            m_creature->GetMotionMaster()->MoveWaypoint(PATH_ID_AKAMA_ILLIDAN);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case AKAMA_ACTION_HEAL:
                if (m_creature->GetHealthPercent() < 10.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_HEALING_POTION) == CAST_OK)
                        ResetCombatAction(action, 30000);
                }
                break;
            case AKAMA_ACTION_CHAIN_LIGHTNING:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CHAIN_LIGHTNING) == CAST_OK)
                        ResetCombatAction(action, urand(2000, 4000));
                break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        DialogueUpdate(diff);

        CombatAI::UpdateAI(diff);
    }
};

bool GossipHello_npc_akama_illidan(Player* player, Creature* creature)
{
    // Before climbing the stairs
    if (creature->GetPositionZ() < 300.0f)
    {
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_PREPARE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(TEXT_ID_AKAMA_ILLIDAN_PREPARE, creature->GetObjectGuid());
    }
    // Before starting combat
    else
    {
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START_EVENT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(TEXT_ID_AKAMA_ILLIDAN_START, creature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_akama_illidan(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1 || action == GOSSIP_ACTION_INFO_DEF + 2)
    {
        player->CLOSE_GOSSIP_MENU();

        if (npc_akama_illidanAI* pAkamaAI = dynamic_cast<npc_akama_illidanAI*>(creature->AI()))
            pAkamaAI->DoResumeEvent();
    }

    return true;
}

/*######
## boss_maiev
######*/

enum MaievActions
{
    MAIEV_ACTION_SHADOW_STRIKE,
    MAIEV_ACTION_THROW_DAGGER,
    MAIEV_ACTION_TAUNT,
    MAIEV_ACTION_MAX,
    MAIEV_ACTION_TRAP,
};

struct boss_maievAI : public CombatAI, private DialogueHelper
{
    boss_maievAI(Creature* creature) : CombatAI(creature, MAIEV_ACTION_MAX), DialogueHelper(aEpilogueDialogue), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_outro(false)
    {
        AddCombatAction(MAIEV_ACTION_SHADOW_STRIKE, 4000, 8000);
        AddCombatAction(MAIEV_ACTION_THROW_DAGGER, 6000, 10000);
        AddCombatAction(MAIEV_ACTION_TAUNT, 40000, 60000);
        AddCustomAction(MAIEV_ACTION_TRAP, true, [&]()
        {
            // Yell only the first time
            if (!m_hasYelledTrap)
            {
                DoScriptText(SAY_MAIEV_TRAP, m_creature);
                m_hasYelledTrap = true;
            }
            SetCombatScriptStatus(false);
            SetMeleeEnabled(true);
            SetCombatMovement(true);
            DoCastSpellIfCan(nullptr, SPELL_CAGE_TRAP_SUMMON);
        });
        InitializeDialogueHelper(m_instance);

        // Not sure if this is correct, but she seems to ignore all the shadow damage inflicted
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, true);
        SetDeathPrevention(true);
        SetReactState(REACT_DEFENSIVE);
        m_creature->SetNoThreatState(true);
    }

    ScriptedInstance* m_instance;

    bool m_hasYelledTrap;
    bool m_outro;

    void Reset() override
    {
        m_hasYelledTrap = false;
        CombatAI::Reset();
    }

    uint32 GetSubsequentActionTimer(MaievActions id)
    {
        switch (id)
        {
            case MAIEV_ACTION_SHADOW_STRIKE: return urand(12000, 16000);
            case MAIEV_ACTION_THROW_DAGGER: return urand(6000, 10000);
            case MAIEV_ACTION_TAUNT: return urand(40000, 60000);
            default: return 0;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        if (eventType == AI_EVENT_CUSTOM_A) // illidan transitioned
        {
            if (miscValue == PHASE_4_DEMON)
            {
                m_attackDistance = 20.f;
                m_moveFurther = true;
                DoStartMovement(m_creature->GetVictim());
                ResetCombatAction(MAIEV_ACTION_THROW_DAGGER, urand(4000, 8000));
                DisableCombatAction(MAIEV_ACTION_SHADOW_STRIKE);
            }
            else
            {
                m_attackDistance = 0.f;
                m_moveFurther = false;
                DoStartMovement(m_creature->GetVictim());
                ResetCombatAction(MAIEV_ACTION_SHADOW_STRIKE, urand(4000, 8000));
                DisableCombatAction(MAIEV_ACTION_THROW_DAGGER);
            }
        }
    }

    void KilledUnit(Unit* victim) override
    {
        // Dummy function - used to start the epilogue
        if (victim->GetEntry() == NPC_ILLIDAN_STORMRAGE)
        {
            m_outro = true;
            SetCombatScriptStatus(true);
            SetMeleeEnabled(false);
            SetCombatMovement(false);
            StartNextDialogueText(SAY_MAIEV_EPILOGUE_1);
        }
    }

    void JustPreventedDeath(Unit* attacker) override
    {
        if (m_outro)
            return;
        m_creature->CastSpell(nullptr, SPELL_MAIEV_DOWN, TRIGGERED_OLD_TRIGGERED);
        SetDeathPrevention(true);
    }

    // Custom evade - don't allow her to return to home position
    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(true);

        m_creature->SetLootRecipient(nullptr);

        Reset();
    }

    void JustDidDialogueStep(int32 entry) override
    {
        switch (entry)
        {
            case NPC_ILLIDAN_STORMRAGE:
                if (m_instance)
                {
                    if (Creature* illidan = m_instance->GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
                        illidan->CastSpell(nullptr, SPELL_QUIET_SUICIDE, TRIGGERED_OLD_TRIGGERED);
                    if (Creature* akama = m_instance->GetSingleCreatureFromStorage(NPC_AKAMA))
                        SendAIEvent(AI_EVENT_CUSTOM_C, m_creature, akama);
                }
                break;
            case SPELL_TELEPORT_VISUAL:
                m_creature->CastSpell(nullptr, SPELL_TELEPORT_VISUAL, TRIGGERED_NONE);
                m_creature->ForcedDespawn(500);
                break;
        }
    }

    void SpellHit(Unit* /*caster*/, SpellEntry const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_CAGE_TRAP)
        {
            SetCombatScriptStatus(true);
            SetMeleeEnabled(false);
            SetCombatMovement(false, true);
            m_creature->SetTarget(nullptr);
            ResetTimer(MAIEV_ACTION_TRAP, 2500);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MAIEV_ACTION_SHADOW_STRIKE:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOW_STRIKE) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(MaievActions(action)));
                return;
            }
            case MAIEV_ACTION_THROW_DAGGER:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_THROW_DAGGER) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(MaievActions(action)));
                return;
            }
            case MAIEV_ACTION_TAUNT:
            {
                switch (urand(0, 2))
                {
                    case 0: DoScriptText(SAY_MAIEV_TAUNT_1, m_creature); break;
                    case 1: DoScriptText(SAY_MAIEV_TAUNT_2, m_creature); break;
                    case 2: DoScriptText(SAY_MAIEV_TAUNT_3, m_creature); break;
                }
                ResetCombatAction(action, GetSubsequentActionTimer(MaievActions(action)));
                return;
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        DialogueUpdate(diff);
        CombatAI::UpdateAI(diff);
    }
};

/*######
## npc_cage_trap_trigger
######*/

struct npc_cage_trap_triggerAI : public ScriptedAI
{
    npc_cage_trap_triggerAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    bool m_active;

    void Reset() override
    {
        m_active = false;
    }

    void AttackStart(Unit* /*who*/) override { }

    void CorpseRemoved(uint32& delay) override
    {
        if (GameObject* trap = GetClosestGameObjectWithEntry(m_creature, GO_CAGE_TRAP, 3.f))
        {
            trap->SetLootState(GO_JUST_DEACTIVATED);
            trap->Delete();
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
#ifndef PRENERF_2_1
        // post 2.3
        if (!m_active && who->GetEntry() == NPC_ILLIDAN_STORMRAGE && m_creature->IsWithinDistInMap(who, 3.0f))
        {
            if (static_cast<boss_illidan_stormrageAI*>(who->AI())->m_phase != PHASE_4_DEMON && !static_cast<boss_illidan_stormrageAI*>(who->AI())->GetCombatScriptStatus())
            {
                m_creature->CastSpell(nullptr, SPELL_CAGE_TRAP_DUMMY, TRIGGERED_OLD_TRIGGERED);

                m_active = true;
                m_creature->ForcedDespawn(15000);
            }
        }
#endif
    }

    void UpdateAI(const uint32 /*diff*/) override { }
};

/*######
## npc_flame_of_azzinoth
######*/

enum FlameActions
{
    FLAME_ACTION_WRATH_CHECK,
    FLAME_ACTION_FLAME_BLAST,
    FLAME_ACTION_SUMMON_BLAZE,
    FLAME_ACTION_CHARGE,
    FLAME_ACTION_MAX,
    FLAME_ACTION_ATTACK,
};

struct npc_flame_of_azzinothAI : public CombatAI
{
    npc_flame_of_azzinothAI(Creature* creature) : CombatAI(creature, FLAME_ACTION_MAX), m_instance(static_cast<instance_black_temple*>(creature->GetInstanceData()))
    {
        AddCombatAction(FLAME_ACTION_WRATH_CHECK, 3000u);
        AddCombatAction(FLAME_ACTION_FLAME_BLAST, 10000u);
        AddCombatAction(FLAME_ACTION_SUMMON_BLAZE, true);
        AddCombatAction(FLAME_ACTION_CHARGE, 10000u);
        AddCustomAction(FLAME_ACTION_ATTACK, 2000u, [&]()
        {
            SetReactState(REACT_AGGRESSIVE);
            m_creature->SetInCombatWithZone();
        });
        SetReactState(REACT_DEFENSIVE);
    }

    instance_black_temple* m_instance;

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_BLAZE)
        {
            summoned->CastSpell(nullptr, SPELL_BLAZE_EFFECT, TRIGGERED_NONE);
            summoned->AI()->SetCombatMovement(false);
        }
    }

    Unit* DoPickChargeTarget()
    {
        ThreatList const& threatlist = m_creature->getThreatManager().getThreatList();
        if (threatlist.empty())
            return nullptr;

        GuidVector blades;
        m_instance->GetCreatureGuidVectorFromStorage(NPC_BLADE_OF_AZZINOTH, blades);
        if (blades.size() < 2) // safeguard
            return nullptr;
        Unit* first = m_creature->GetMap()->GetCreature(blades[0]);
        Unit* second = m_creature->GetMap()->GetCreature(blades[1]);
        for (ThreatList::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
        {
            if (Unit* target = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid()))
            {
                float x, y, z;
                target->GetPosition(x, y, z);
                if ((!first || first->GetDistance(x, y, z, DIST_CALC_COMBAT_REACH) > 30.f) && (!second || second->GetDistance(x, y, z, DIST_CALC_COMBAT_REACH) > 30.f))
                    return target;
            }
        }
        return nullptr;
    }

    void Enrage()
    {
        if (DoCastSpellIfCan(nullptr, SPELL_UNCAGED_WRATH, CAST_TRIGGERED) == CAST_OK)
        {
            DisableCombatAction(FLAME_ACTION_WRATH_CHECK);
            if (Unit* spawner = m_creature->GetSpawner())
                spawner->InterruptSpell(CURRENT_CHANNELED_SPELL); // interrupts link
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case FLAME_ACTION_WRATH_CHECK:
            {
                /* - aura based version for when stuff works properly
                if (!m_creature->HasAura(SPELL_RANGE_MARKER))
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_UNCAGED_WRATH, CAST_TRIGGERED) == CAST_OK)
                    {
                        DisableCombatAction(FLAME_ACTION_WRATH_CHECK);
                        if (Unit* spawner = m_creature->GetSpawner())
                            spawner->InterruptSpell(CURRENT_CHANNELED_SPELL); // interrupts link
                    }
                }
                */
                // distance based version
                if (Unit* spawner = m_creature->GetSpawner())
                {
                    float x, y, z;
                    m_creature->GetPosition(x, y, z);
                    if (spawner->GetDistance(x, y, z, DIST_CALC_COMBAT_REACH) > 30.f)                    
                        Enrage();                   
                }
                else ResetCombatAction(action, 1000);
                return;
            }
            case FLAME_ACTION_FLAME_BLAST:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_FLAME_BLAST) == CAST_OK)
                {
                    ResetCombatAction(action, 10000);
                    ResetCombatAction(FLAME_ACTION_SUMMON_BLAZE, 1000);
                }
                return;
            }
            case FLAME_ACTION_SUMMON_BLAZE:
            {
                if (Unit* target = m_creature->GetVictim())
                    if (target->CastSpell(nullptr, SPELL_BLAZE, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid()) == SPELL_CAST_OK)
                        DisableCombatAction(FLAME_ACTION_SUMMON_BLAZE);
                return;
            }
            case FLAME_ACTION_CHARGE:
            {
                if (Unit* target = DoPickChargeTarget())
                    if (DoCastSpellIfCan(target, SPELL_CHARGE) == CAST_OK)
                        ResetCombatAction(action, 5000);
                return;
            }
        }
    }
};

/*######
## npc_shadow_demon
######*/

struct npc_shadow_demonAI : public ScriptedAI
{
    npc_shadow_demonAI(Creature* creature) : ScriptedAI(creature)
    {
        SetReactState(REACT_AGGRESSIVE);
        m_creature->SetCorpseDelay(5);
        m_creature->SetWalk(false);
        Reset();
    }

    ObjectGuid m_targetGuid;

    void Reset() override {}

    void AttackStart(Unit* who) override
    {
        // Function used to set target only - the npc doesn't really attack
        m_targetGuid = who->GetObjectGuid();
    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        m_creature->CastSpell(nullptr, SPELL_SHADOW_DEMON_PASSIVE, TRIGGERED_OLD_TRIGGERED);
        FindNewTarget();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Player* player = m_creature->GetMap()->GetPlayer(m_targetGuid))
            player->RemoveAurasByCasterSpell(SPELL_PARALYZE, m_creature->GetObjectGuid());
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && m_creature->IsAlive()) // Channel ended for any reason
            m_targetGuid = ObjectGuid(); // find new target on next AI update
    }

    void FindNewTarget()
    {
        Unit* illidan = m_creature->GetSpawner();
        if (illidan && illidan->GetTypeId() == TYPEID_UNIT)
        {
            if (Unit* target = static_cast<Creature*>(illidan)->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_PARALYZE, SELECT_FLAG_PLAYER))
            {
                // Dummy attack function - used only to set the target
                AttackStart(target);
                m_creature->CastSpell(target, SPELL_PARALYZE, TRIGGERED_OLD_TRIGGERED);

                // Move towards target (which is stunned)
                float x, y, z;
                target->GetContactPoint(m_creature, x, y, z);
                m_creature->GetMotionMaster()->MovePoint(1, x, y, z);
            }
        }
    }

    void MovementInform(uint32 movementType, uint32 pointId) override
    {
        if (movementType != POINT_MOTION_TYPE || !pointId)
            return;

        if (!m_creature->IsAlive())
        {
            sLog.outCustomLog("Why did shadow demon movement trigger?");
            // sLog.traceLog();
            return;
        }

        if (Player* player = m_creature->GetMap()->GetPlayer(m_targetGuid))
            DoCastSpellIfCan(player, SPELL_CONSUME_SOUL); // will find new target on channel interrupt
    }

    void UpdateAI(const uint32 /*diff*/) override
    {
        if (!m_targetGuid)
            FindNewTarget();
    }
};

/*######
## npc_blade_of_azzinoth
######*/

struct npc_blade_of_azzinothAI : public ScriptedAI
{
    npc_blade_of_azzinothAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        Reset();
    }

    ScriptedInstance* m_instance;

    void Reset() override {}

    // Do-Nothing-But-Stand-There
    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }

    void JustSummoned(Creature* summoned) override
    {
        // Summon a Flame pear each blade
        if (summoned->GetEntry() == NPC_FLAME_OF_AZZINOTH)
            DoCastSpellIfCan(summoned, SPELL_AZZINOTH_CHANNEL);
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        // Inform Illidan when a flame is killed
        if (summoned->GetEntry() == NPC_FLAME_OF_AZZINOTH)
        {
            if (!m_instance)
                return;

            // For some reason it doesn't work with Spell Hit for SPELL_GLAIVE_RETURNS script effect, so we need to inform him manually
            if (Creature* illidan = m_instance->GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
            {
                if (boss_illidan_stormrageAI* illidanAI = dynamic_cast<boss_illidan_stormrageAI*>(illidan->AI()))
                    illidanAI->DoInformFlameKilled();
            }
        }
    }

    void UpdateAI(const uint32 /*diff*/) override { }
};

struct npc_parasitic_shadowfiendAI : public ScriptedAI
{
    npc_parasitic_shadowfiendAI(Creature* creature) : ScriptedAI(creature, 0), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        SetReactState(REACT_PASSIVE);
        AddCustomAction(1, 2000u, [&]()
        {
            if (!m_instance)
                return;

            if (Creature* illidan = m_instance->GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
            {
                if (!illidan->IsInCombat())
                {
                    m_creature->ForcedDespawn();
                    return;
                }
                SetReactState(REACT_AGGRESSIVE);
                m_params.skip.guid = m_creature->GetSpawnerGuid();
                if (Unit* target = illidan->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_SKIP_CUSTOM, m_params))
                {
                    m_creature->AI()->AttackStart(target);
                    m_creature->AddThreat(target, 100000.f);
                }
                m_creature->SetInCombatWithZone();
            }
            else m_creature->ForcedDespawn();
        });
    }

    ScriptedInstance* m_instance;
    SelectAttackingTargetParams m_params;

    void Reset() override {}

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        DoCastSpellIfCan(nullptr, SPELL_SHADOWFORM_PARASITE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_PARASITIC_SHADOWFIEND_P, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        m_creature->SetCorpseDelay(1);
    }
};

bool GOUse_go_cage_trap(Player* player, GameObject* go)
{
    Creature* trapTrigger = GetClosestCreatureWithEntry(go, NPC_CAGE_TRAP_DISTURB_TRIGGER, 3.f);
    if (trapTrigger)
    {
        // pre 2.3
#ifdef PRENERF_2_1
        Creature* illidan = GetClosestCreatureWithEntry(go, NPC_ILLIDAN_STORMRAGE, 5.f);
        if (illidan && static_cast<boss_illidan_stormrageAI*>(illidan->AI())->m_phase != PHASE_4_DEMON && !static_cast<boss_illidan_stormrageAI*>(illidan->AI())->GetCombatScriptStatus())
        {
            trapTrigger->CastSpell(nullptr, SPELL_CAGE_TRAP_DUMMY, TRIGGERED_OLD_TRIGGERED);
            trapTrigger->ForcedDespawn(15000);
        }
#endif
    }
    return true;
}

struct ParasiticShadowfiendAura : public SpellScript, public AuraScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_1) // on hit despawn
        {
            Unit* target = spell->GetUnitTarget();
            if (target->IsCreature())
                static_cast<Creature*>(target)->ForcedDespawn();
        }
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            if (aura->GetRemoveMode() != AURA_REMOVE_BY_DISPEL)
                aura->GetTarget()->CastSpell(nullptr, SPELL_SUMMON_PARASITICS, TRIGGERED_OLD_TRIGGERED);
    }
};

struct ShadowPrison : public SpellScript
{
    void OnRadiusCalculate(Spell* /*spell*/, SpellEffectIndex /*effIdx*/, bool /*targetB*/, float& radius) const override
    {
        radius = 100.f;
    }

    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->IsControlledByPlayer() || (target->IsCreature() && target->GetEntry() == NPC_AKAMA))
            return true;
        return false;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_2 && spell->GetUnitTarget())
        {
            spell->GetUnitTarget()->RemoveAurasDueToSpell(SPELL_PARASITIC_SHADOWFIEND_BOSS, nullptr, AURA_REMOVE_BY_DISPEL);
            spell->GetUnitTarget()->RemoveAurasDueToSpell(SPELL_PARASITIC_SHADOWFIEND_ADD, nullptr, AURA_REMOVE_BY_DISPEL);
        }
    }
};

void AddSC_boss_illidan()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_illidan_stormrage";
    pNewScript->GetAI = &GetNewAIInstance<boss_illidan_stormrageAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_akama_illidan";
    pNewScript->GetAI = &GetNewAIInstance<npc_akama_illidanAI>;
    pNewScript->pGossipHello = &GossipHello_npc_akama_illidan;
    pNewScript->pGossipSelect = &GossipSelect_npc_akama_illidan;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_maiev_shadowsong";
    pNewScript->GetAI = &GetNewAIInstance<boss_maievAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_flame_of_azzinoth";
    pNewScript->GetAI = &GetNewAIInstance<npc_flame_of_azzinothAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_blade_of_azzinoth";
    pNewScript->GetAI = &GetNewAIInstance<npc_blade_of_azzinothAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_cage_trap_trigger";
    pNewScript->GetAI = &GetNewAIInstance<npc_cage_trap_triggerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_shadow_demon";
    pNewScript->GetAI = &GetNewAIInstance<npc_shadow_demonAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_parasitic_shadowfiend";
    pNewScript->GetAI = &GetNewAIInstance<npc_parasitic_shadowfiendAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_cage_trap";
    pNewScript->pGOUse = &GOUse_go_cage_trap;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ParasiticShadowfiendAura>("spell_parasitic_shadowfiend");
    RegisterSpellScript<ShadowPrison>("spell_shadow_prison");
}

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
SD%Complete: 90
SDComment: Movement during flight phase NYI. Some other fine details may need adjustments.
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "black_temple.h"
#include "escort_ai.h"

enum
{
    /************* Quotes and Sounds ***********************/
    // Intro yells and gossip
    SAY_AKAMA_BEWARE                = -1564120,
    SAY_AKAMA_OPEN_DOOR_1           = -1564131,
    SAY_AKAMA_OPEN_DOOR_2           = -1564132,
    SAY_UDALO_OPEN_DOOR_3           = -1564133,
    SAY_OLUM_OPEN_DOOR_4            = -1564134,

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

    // Epilogue speech
    SAY_MAIEV_EPILOGUE_1            = -1564107,
    SAY_ILLIDAN_EPILOGUE_2          = -1564108,
    SAY_MAIEV_EPILOGUE_3            = -1564109,
    SAY_MAIEV_EPILOGUE_4            = -1564110,
    SAY_AKAMA_EPILOGUE_5            = -1564111,

    // Combat yells
    SAY_KILL1                       = -1564123,
    SAY_KILL2                       = -1564124,
    SAY_TAKEOFF                     = -1564125,
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
    SPELL_PARASITIC_SHADOWFIEND     = 41917,                // DoT of 3k Shadow every 2 seconds. Lasts 10 seconds. (Script effect: Summon 2 parasites once the debuff has ticked off)
    // SPELL_SUMMON_PARASITICS       = 41915,               // Summons 2 Parasitic Shadowfiends on the target. Handled in core.
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
    SPELL_DEATH                     = 41220,                // This spell doesn't do anything except stun Illidan and set him on his knees.
    SPELL_BERSERK                   = 45078,                // Damage increased by 500%, attack speed by 150%


    /************** Non-Illidan Spells *************/
    // Akama
    SPELL_AKAMA_DOOR_FAIL           = 41271,                // Akama's first door attempt
    SPELL_AKAMA_DOOR_CHANNEL        = 41268,                // Akama's channel spell on the door before the Temple Summit
    SPELL_DEATHSWORN_DOOR_CHANNEL   = 41269,                // Olum and Udalo's channel spell on the door before the Temple Summit
    SPELL_HEALING_POTION            = 40535,                // Akama uses this to heal himself to full.
    SPELL_CHAIN_LIGHTNING           = 40536,

    // Maiev
    SPELL_SHADOW_STRIKE             = 40685,
    SPELL_THROW_DAGGER              = 41152,
    SPELL_CAGE_TRAP_SUMMON          = 40694,                // summons npc 23304 and go 185916
    SPELL_TELEPORT_VISUAL           = 41236,

    // Misc Summoned
    SPELL_FLAME_CRASH_EFFECT        = 40836,                // Firey blue ring of circle that the other flame crash summons
    SPELL_EYE_BLAST_TRIGGER         = 40017,                // This summons Demon Form every few seconds and deals ~20k damage in its radius
    // SPELL_DEMON_FIRE             = 40029,                // Blue fire trail left by Eye Blast. Deals 2k per second if players stand on it.
    SPELL_BLAZE_EFFECT              = 40610,                // Green flame on the ground, triggers damage (5k) every few seconds

    // Blade of Azzinoth
    SPELL_RANGE_MARKER              = 41997,                // Dummy effect used by the Blade of Azzinoth to check the range of the Azzinoth flame - needs core support
    SPELL_SUMMON_TEAR_AZZINOTH      = 39855,                // Summons 22997
    SPELL_AZZINOTH_CHANNEL          = 39857,                // Glaives cast it on Flames

    // Flame of Azzinoth
    SPELL_FLAME_BLAST               = 40631,                // Flames of Azzinoth use this. Frontal cone AoE 7k-9k damage.
    SPELL_CHARGE                    = 42003,                // Flames of Azzinoth charges whoever is too far from them. They enrage after this
    SPELL_UNCAGED_WRATH             = 39869,
    SPELL_BLAZE                     = 40637,                // summons 23259

    // Shadow Demon
    SPELL_SHADOW_DEMON_PASSIVE      = 41079,                // Adds the "shadowform" uiAura to Shadow Demons.
    SPELL_CONSUME_SOUL              = 41080,                // Once the Shadow Demons reach their target, they use this to kill them
    SPELL_PARALYZE                  = 41083,                // Shadow Demons cast this on their target

    // Cage spells
    SPELL_CAGE_TRAP_PERIODIC        = 40760,                // purpose unk
    SPELL_CAGE_TRAP_DUMMY           = 40761,                // purpose unk
    SPELL_CAGED                     = 40695,                // Caged Trap triggers will cast this on Illidan if he is within 3 yards


    /************** Creature Summons **************/
    NPC_ILLIDARI_ELITE              = 23226,                // attacks Akama on the stairs
    NPC_FLAME_CRASH                 = 23336,                // has aura 40836
    // NPC_PARASITIC_SHADOWFIEND    = 23498,                // has aura 41913 (in c_t_a)
    NPC_BLADE_OF_AZZINOTH           = 22996,                // has aura 41997 and summons 22997 on spawn
    NPC_FLAME_OF_AZZINOTH           = 22997,
    NPC_ILLIDAN_TARGET              = 23070,                // the eye blast target - has aura 40017
    // NPC_DEMON_FIRE               = 23069,                // has aura 40029 (in EventAI)
    NPC_BLAZE                       = 23259,                // has aura 40610
    NPC_SHADOW_DEMON                = 23375,
    // NPC_CAGE_TRAP_DISTURB_TRIGGER = 23304,

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
};

static const uint32 aCagedSummonSpells[MAX_CAGE_SPELLS] = { 40696, 40697, 40698, 40699, 40700, 40701, 40702, 40703 };
static const uint32 aCagedVisualSpells[MAX_CAGE_SPELLS] = { 40704, 40707, 40708, 40709, 40710, 40711, 40712, 40713 };

static const DialogueEntry aIntroDialogue[] =
{
    {SAY_AKAMA_OPEN_DOOR_1,     NPC_AKAMA,              4000},
    {SPELL_AKAMA_DOOR_FAIL,     0,                      9000},
    {SAY_AKAMA_OPEN_DOOR_2,     NPC_AKAMA,              6000},
    {NPC_SPIRIT_OF_OLUM,        0,                      2000},
    {SAY_UDALO_OPEN_DOOR_3,     NPC_SPIRIT_OF_UDALO,    2000},
    {SAY_OLUM_OPEN_DOOR_4,      NPC_SPIRIT_OF_OLUM,     4000},
    {SPELL_AKAMA_DOOR_CHANNEL,  0,                      11000},
    {GO_ILLIDAN_GATE,           0,                      4000},
    {NPC_SPIRIT_OF_UDALO,       0,                      0},
    {0, 0, 0},
};

static const DialogueEntry aEventDialogue[] =
{
    // Akama intro
    {NPC_AKAMA,                 0,                      1000},
    {SAY_ILLIDAN_SPEECH_1,      NPC_ILLIDAN_STORMRAGE,  3000},
    {EMOTE_ONESHOT_QUESTION,    0,                      3000},
    {DUMMY_EMOTE_ID_1,          0,                      3000},
    {DUMMY_EMOTE_ID_2,          0,                      3000},
    {SAY_AKAMA_SPEECH_2,        NPC_AKAMA,              10000},
    {SAY_ILLIDAN_SPEECH_3,      NPC_ILLIDAN_STORMRAGE,  3000},
    {DUMMY_EMOTE_ID_3,          0,                      4000},
    {SAY_AKAMA_SPEECH_4,        NPC_AKAMA,              4000},
    {EQUIP_ID_MAIN_HAND,        0,                      1000},
    {SAY_ILLIDAN_SPEECH_5,      NPC_ILLIDAN_STORMRAGE,  4000},
    {NPC_ILLIDAN_STORMRAGE,     0,                      0},
    // Akama leaves fight
    {SAY_ILLIDAN_MINION,        NPC_ILLIDAN_STORMRAGE,  8000},
    {SAY_AKAMA_LEAVE,           NPC_AKAMA,              0},
    // Maiev cutscene
    {SAY_ILLIDAN_SPEECH_6,      NPC_ILLIDAN_STORMRAGE,  7000},
    {SPELL_SUMMMON_MAIEV,       0,                      1000},
    {SAY_MAIEV_SPEECH_7,        NPC_MAIEV_SHADOWSONG,   2000},
    {EMOTE_ONESHOT_EXCLAMATION, 0,                      6000},
    {SAY_ILLIDAN_SPEECH_8,      NPC_ILLIDAN_STORMRAGE,  7000},
    {SAY_MAIEV_SPEECH_9,        NPC_MAIEV_SHADOWSONG,   2000},
    {EMOTE_ONESHOT_YES,         0,                      5000},
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
enum Phase
{
    PHASE_AKAMA             = 1,
    PHASE_BLADES            = 2,
    PHASE_DUAL_NORMAL       = 3,
    PHASE_DUAL_DEMON        = 4,
    PHASE_MAIEV             = 5,
    PHASE_TRANSITION        = 6,
};

struct Locations
{
    float fX, fY, fZ;
};

static const Locations aCenterLoc[] =
{
    {705.012f, 305.721f, 354.723f},             // front location
    {676.740f, 305.297f, 353.192f},             // center location
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

static const Locations aEyeBlastPos[] =
{
    // spawn
    {650.600f, 258.124f, 352.996f},             // back left
    {651.867f, 353.212f, 352.996f},             // back right
    {710.010f, 266.950f, 352.996f},             // front left
    {711.003f, 343.562f, 352.996f},             // front right
    // target - left
    {742.212f, 338.333f, 352.996f},             // front right
    {674.559f, 375.761f, 352.996f},             // back right
    // target - right
    {741.545f, 270.640f, 352.996f},             // front left
    {671.943f, 235.718f, 352.996f},             // back left
    // center back
    {639.511f, 305.852f, 353.264f}
};

/*######
## boss_illidan_stormrage
######*/

struct boss_illidan_stormrageAI : public ScriptedAI, private DialogueHelper
{
    boss_illidan_stormrageAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aEventDialogue)
    {
        m_pInstance = (instance_black_temple*)pCreature->GetInstanceData();
        InitializeDialogueHelper(m_pInstance);
        Reset();
    }

    instance_black_temple* m_pInstance;

    Phase m_uiPhase;
    uint32 m_uiBerserkTimer;

    bool m_bHasSummonedElites;
    float m_fTargetMoveX, m_fTargetMoveY, m_fTargetMoveZ;

    uint32 m_uiShearTimer;
    uint32 m_uiDrawSoulTimer;
    uint32 m_uiFlameCrashTimer;
    uint32 m_uiShadowFiendTimer;

    uint32 m_uiFireballTimer;
    uint32 m_uiEyeBlastTimer;
    uint32 m_uiDarkBarrageTimer;
    uint32 m_uiSummonBladesTimer;                           // Animate summoning the Blades of Azzinoth in Phase 2
    uint32 m_uiCenterMoveTimer;
    uint32 m_uiLandTimer;                                   // This is used at the end of uiPhase 2 to signal Illidan landing after Flames are dead
    uint8 m_uiLandStage;
    uint8 m_uiFlameAzzinothKilled;

    uint32 m_uiAgonizingFlamesTimer;
    uint32 m_uiTransformTimer;

    uint32 m_uiShadowBlastTimer;
    uint32 m_uiFlameBurstTimer;
    uint32 m_uiShadowDemonTimer;
    Phase m_uiPrevPhase;                                    // store the previous phase in transition

    uint32 m_uiEnrageTimer;
    uint32 m_uiTrapTimer;

    GuidList m_lBladesGuidList;

    void Reset() override
    {
        m_uiPhase               = PHASE_AKAMA;
        m_uiBerserkTimer        = 25 * MINUTE * IN_MILLISECONDS;

        m_bHasSummonedElites    = false;

        m_uiShearTimer          = urand(10000, 15000);
        m_uiFlameCrashTimer     = 30000;
        m_uiShadowFiendTimer    = 25000;
        m_uiDrawSoulTimer       = 35000;

        m_uiFlameAzzinothKilled = 0;
        m_uiSummonBladesTimer   = 0;
        m_uiCenterMoveTimer     = 0;
        m_uiFireballTimer       = 5000;
        m_uiDarkBarrageTimer    = 45000;
        m_uiEyeBlastTimer       = 15000;
        m_uiLandTimer           = 0;
        m_uiLandStage           = 0;

        m_uiAgonizingFlamesTimer = 35000;
        m_uiTransformTimer      = 0;

        m_uiShadowBlastTimer    = urand(1000, 2000);
        m_uiFlameBurstTimer     = 10000;
        m_uiShadowDemonTimer    = 30000;

        m_uiEnrageTimer         = 40000;
        m_uiTrapTimer           = urand(30000, 40000);

        m_lBladesGuidList.clear();

        // Reset boss
        m_creature->SetLevitate(false);
        SetCombatMovement(true);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
    }

    void GetAIInformation(ChatHandler& reader) override
    {
        reader.PSendSysMessage("Boss Illidan, current uiPhase = %u", m_uiPhase);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ILLIDAN, IN_PROGRESS);
    }

    // Do not attack using LoS function. The attack is triggered in script
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ILLIDAN, FAIL);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ILLIDAN, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(urand(0, 1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage) override
    {
        if (uiDamage < m_creature->GetHealth())
            return;

        // Make sure it won't die by accident
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
        {
            uiDamage = 0;
            return;
        };

        uiDamage = 0;
        m_creature->InterruptNonMeleeSpells(true);
        m_creature->SetHealth(1);
        m_creature->StopMoving();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->ClearAllReactives();

        DoCastSpellIfCan(m_creature, SPELL_DEATH, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_TELEPORT_MAIEV, CAST_TRIGGERED);
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();

        // Signal Maiev to start the outro dialogue
        if (m_pInstance)
        {
            if (Creature* pMaiev = m_pInstance->GetSingleCreatureFromStorage(NPC_MAIEV_SHADOWSONG))
                pMaiev->AI()->KilledUnit(m_creature);
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case NPC_AKAMA:
                if (m_pInstance)
                {
                    if (Creature* pAkama = m_pInstance->GetSingleCreatureFromStorage(NPC_AKAMA))
                        m_creature->SetFacingToObject(pAkama);
                }
                m_creature->RemoveAurasDueToSpell(SPELL_KNEEL_INTRO);
                break;
            case EMOTE_ONESHOT_QUESTION:
            case DUMMY_EMOTE_ID_1:
            case DUMMY_EMOTE_ID_2:
            case DUMMY_EMOTE_ID_3:
                m_creature->HandleEmote(EMOTE_ONESHOT_QUESTION);
                break;
            case EQUIP_ID_MAIN_HAND:
                SetEquipmentSlots(false, EQUIP_ID_MAIN_HAND, EQUIP_ID_OFF_HAND, EQUIP_NO_CHANGE);
                break;
            case NPC_ILLIDAN_STORMRAGE:
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                m_creature->SetInCombatWithZone();
                if (m_pInstance)
                {
                    if (Creature* pAkama = m_pInstance->GetSingleCreatureFromStorage(NPC_AKAMA))
                    {
                        pAkama->AI()->AttackStart(m_creature);
                        AttackStart(pAkama);
                    }
                }
                break;
            case SAY_AKAMA_LEAVE:
                DoResetThreat();
                if (m_pInstance)
                {
                    // Remove Akama from threat list and allow him to fight the Illidari elites
                    if (Creature* pAkama = m_pInstance->GetSingleCreatureFromStorage(NPC_AKAMA))
                    {
                        pAkama->AI()->EnterEvadeMode();
                        m_creature->getThreatManager().modifyThreatPercent(pAkama, -101);
                    }
                }
                break;
            case SPELL_SUMMMON_MAIEV:
                DoCastSpellIfCan(m_creature, SPELL_SUMMMON_MAIEV);
                break;
            case EMOTE_ONESHOT_EXCLAMATION:
                if (m_pInstance)
                {
                    if (Creature* pMaiev = m_pInstance->GetSingleCreatureFromStorage(NPC_MAIEV_SHADOWSONG))
                        pMaiev->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                }
                break;
            case EMOTE_ONESHOT_YES:
                if (m_pInstance)
                {
                    if (Creature* pMaiev = m_pInstance->GetSingleCreatureFromStorage(NPC_MAIEV_SHADOWSONG))
                        pMaiev->HandleEmote(EMOTE_ONESHOT_YES);
                }
                break;
            case NPC_MAIEV_SHADOWSONG:
                // Resume combat and attack Maiev
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->SetTargetGuid(m_creature->getVictim()->GetObjectGuid());
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                if (m_pInstance)
                {
                    if (Creature* pMaiev = m_pInstance->GetSingleCreatureFromStorage(NPC_MAIEV_SHADOWSONG))
                        pMaiev->AI()->AttackStart(m_creature);
                }
                m_uiPhase = PHASE_MAIEV;
                m_uiTransformTimer = 60000;
                break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_FLAME_CRASH:
                pSummoned->CastSpell(pSummoned, SPELL_FLAME_CRASH_EFFECT, false);
                break;
            case NPC_BLADE_OF_AZZINOTH:
                pSummoned->CastSpell(pSummoned, SPELL_RANGE_MARKER, true);
                pSummoned->CastSpell(pSummoned, SPELL_SUMMON_TEAR_AZZINOTH, true);
                m_lBladesGuidList.push_back(pSummoned->GetObjectGuid());
                break;
            case NPC_ILLIDAN_TARGET:
                pSummoned->SetWalk(false);
                pSummoned->CastSpell(pSummoned, SPELL_EYE_BLAST_TRIGGER, true);
                pSummoned->GetMotionMaster()->MovePoint(0, m_fTargetMoveX, m_fTargetMoveY, m_fTargetMoveZ);
                DoCastSpellIfCan(pSummoned, SPELL_EYE_BLAST_DUMMY, CAST_TRIGGERED);
                break;
            case NPC_SHADOW_DEMON:
                pSummoned->CastSpell(pSummoned, SPELL_SHADOW_DEMON_PASSIVE, true);
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_PARALYZE, SELECT_FLAG_PLAYER))
                {
                    // Dummy attack function - used only to set the target
                    pSummoned->AI()->AttackStart(pTarget);
                    pSummoned->CastSpell(pTarget, SPELL_PARALYZE, true);

                    // Move towards target (which is stunned)
                    float fX, fY, fZ;
                    pTarget->GetContactPoint(pSummoned, fX, fY, fZ);
                    pSummoned->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                }
                break;
            case NPC_MAIEV_SHADOWSONG:
                pSummoned->SetFacingToObject(m_creature);
                m_creature->SetTargetGuid(pSummoned->GetObjectGuid());
                break;
        }
    }

    // Wrapper to start the combat dialogue
    void DoStartCombatEvent() { StartNextDialogueText(NPC_AKAMA); }

    // Wrapper to land Illidan when both flames are killed
    void DoInformFlameKilled()
    {
        // Land Illidan if both Flames are killed
        ++m_uiFlameAzzinothKilled;

        if (m_uiFlameAzzinothKilled == MAX_FLAME_AZZINOTH)
        {
            m_uiLandTimer = 5000;
            m_uiPhase = PHASE_TRANSITION;
            m_creature->InterruptNonMeleeSpells(false);
        }
    }

    // Wrapper to handle the Eye Blast cast
    bool DoCastEyeBlastIfCan()
    {
        if (m_creature->IsNonMeleeSpellCasted(false))
            return false;

        DoScriptText(SAY_EYE_BLAST, m_creature);

        // Set spawn and target loc
        uint8 uiSpawnLoc = urand(0, 3);
        uint8 uiTargetLoc = 0;
        switch (uiSpawnLoc)
        {
            case 0: uiTargetLoc = urand(4, 5); break;
            case 1: uiTargetLoc = urand(6, 7); break;
            case 2: uiTargetLoc = urand(0, 1) ? 5 : 8; break;
            case 3: uiTargetLoc = urand(7, 8); break;
        }

        m_fTargetMoveX = aEyeBlastPos[uiTargetLoc].fX;
        m_fTargetMoveY = aEyeBlastPos[uiTargetLoc].fY;
        m_fTargetMoveZ = aEyeBlastPos[uiTargetLoc].fZ;
        m_creature->SummonCreature(NPC_ILLIDAN_TARGET, aEyeBlastPos[uiSpawnLoc].fX, aEyeBlastPos[uiSpawnLoc].fY, aEyeBlastPos[uiSpawnLoc].fZ, 0, TEMPSUMMON_TIMED_DESPAWN, 15000);

        return true;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Make Akama evade combat at 85%
        if (!m_bHasSummonedElites && m_creature->GetHealthPercent() < 85.0f)
        {
            StartNextDialogueText(SAY_ILLIDAN_MINION);
            m_bHasSummonedElites = true;
        }

        // Phase 1 to 2 transition
        if (m_uiPhase == PHASE_AKAMA && m_creature->GetHealthPercent() < 65.0f)
        {
            DoScriptText(SAY_TAKEOFF, m_creature);
            m_uiSummonBladesTimer = 10000;
            m_uiCenterMoveTimer   = 2000;
            m_uiPhase = PHASE_BLADES;

            m_creature->RemoveAllAuras();
            m_creature->SetLevitate(true);
            SetCombatMovement(false);
            m_creature->GetMotionMaster()->Clear();
            m_creature->HandleEmote(EMOTE_ONESHOT_LIFTOFF);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            return;
        }

        // Summon Maiev at 30% hp
        if (m_uiPhase == PHASE_DUAL_NORMAL && m_creature->GetHealthPercent() <= 30.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SHADOW_PRISON, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
            {
                StartNextDialogueText(SAY_ILLIDAN_SPEECH_6);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                SetCombatMovement(false);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveIdle();

                m_uiPhase = PHASE_TRANSITION;
                m_uiTransformTimer = 0;
            }
            return;
        }

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        switch (m_uiPhase)
        {
            case PHASE_MAIEV:

                // Phase 5 spell only
                if (m_uiEnrageTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
                    {
                        DoScriptText(SAY_FRENZY, m_creature);
                        m_uiEnrageTimer = 40000;
                    }
                }
                else
                    m_uiEnrageTimer -= uiDiff;

                if (m_uiTrapTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_CAGE_TRAP) == CAST_OK)
                        m_uiTrapTimer = urand(40000, 50000);
                }
                else
                    m_uiTrapTimer -= uiDiff;

                // no break;
            case PHASE_DUAL_NORMAL:

                // Phase 3 and 5 spells
                if (m_uiAgonizingFlamesTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_AGONIZING_FLAMES) == CAST_OK)
                        m_uiAgonizingFlamesTimer = 60000;
                }
                else
                    m_uiAgonizingFlamesTimer -= uiDiff;

                if (m_uiTransformTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_DEMON_TRANSFORM_1) == CAST_OK)
                    {
                        DoScriptText(SAY_MORPH, m_creature);

                        m_uiPrevPhase = m_uiPhase;
                        m_uiPhase = PHASE_TRANSITION;
                        m_uiTransformTimer   = 12500;
                        m_uiFlameBurstTimer  = 10000;
                        m_uiShadowDemonTimer = 30000;

                        SetCombatMovement(false);
                        m_creature->GetMotionMaster()->Clear();
                        m_creature->GetMotionMaster()->MoveIdle();
                    }
                }
                else
                    m_uiTransformTimer -= uiDiff;

                // no break;
            case PHASE_AKAMA:

                if (m_uiShearTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHEAR) == CAST_OK)
                        m_uiShearTimer = urand(10000, 15000);
                }
                else
                    m_uiShearTimer -= uiDiff;

                if (m_uiFlameCrashTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FLAME_CRASH) == CAST_OK)
                        m_uiFlameCrashTimer = urand(25000, 35000);
                }
                else
                    m_uiFlameCrashTimer -= uiDiff;

                if (m_uiShadowFiendTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_PARASITIC_SHADOWFIEND, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_PARASITIC_SHADOWFIEND) == CAST_OK)
                            m_uiShadowFiendTimer = 40000;
                    }
                }
                else
                    m_uiShadowFiendTimer -= uiDiff;

                if (m_uiDrawSoulTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_DRAW_SOUL) == CAST_OK)
                        m_uiDrawSoulTimer = 35000;
                }
                else
                    m_uiDrawSoulTimer -= uiDiff;

                DoMeleeAttackIfReady();

                break;
            case PHASE_BLADES:

                if (m_uiCenterMoveTimer)
                {
                    if (m_uiCenterMoveTimer <= uiDiff)
                    {
                        // The movement is not very clear - it may be possible that he is moving around the center during this phase
                        // ToDo: this requires additional resarch. For now bring him near home position
                        m_creature->GetMotionMaster()->MovePoint(0, aCenterLoc[0].fX, aCenterLoc[0].fY, aCenterLoc[0].fZ);
                        m_uiCenterMoveTimer = 0;
                    }
                    else
                        m_uiCenterMoveTimer -= uiDiff;
                }

                if (m_uiSummonBladesTimer)
                {
                    if (m_uiSummonBladesTimer <= uiDiff)
                    {
                        if (m_pInstance)
                        {
                            // Need to provide explicit glaive targets
                            GuidVector vTargetsVect;
                            m_pInstance->GetGlaiveTargetGuidVector(vTargetsVect);

                            Creature* pGlaive1 = m_creature->GetMap()->GetCreature(vTargetsVect[0]);
                            Creature* pGlaive2 = m_creature->GetMap()->GetCreature(vTargetsVect[1]);
                            if (!pGlaive1 || !pGlaive2)
                                return;

                            // Summon both blades and remove them from equipment
                            if (DoCastSpellIfCan(pGlaive1, SPELL_THROW_GLAIVE_VISUAL) == CAST_OK)
                            {
                                DoCastSpellIfCan(pGlaive2, SPELL_THROW_GLAIVE, CAST_TRIGGERED);
                                SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                                m_uiSummonBladesTimer = 0;
                            }
                        }
                    }
                    else
                        m_uiSummonBladesTimer -= uiDiff;

                    // no other spells during takeoff
                    return;
                }

                if (m_uiFireballTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_FIREBALL) == CAST_OK)
                            m_uiFireballTimer = urand(2000, 3000);
                    }
                }
                else
                    m_uiFireballTimer -= uiDiff;

                if (m_uiDarkBarrageTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_DARK_BARRAGE) == CAST_OK)
                            m_uiDarkBarrageTimer = 45000;
                    }
                }
                else
                    m_uiDarkBarrageTimer -= uiDiff;

                if (m_uiEyeBlastTimer < uiDiff)
                {
                    if (DoCastEyeBlastIfCan())
                    {
                        m_uiEyeBlastTimer = urand(35000, 45000);
                        m_uiFireballTimer = 15000;
                    }
                }
                else
                    m_uiEyeBlastTimer -= uiDiff;

                break;
            case PHASE_DUAL_DEMON:

                // Handle phase transition at 30%
                if (m_uiPrevPhase == PHASE_DUAL_NORMAL && m_creature->GetHealthPercent() <= 30.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_DEMON_TRANSFORM_1) == CAST_OK)
                    {
                        m_uiTransformTimer = 12500;
                        m_uiPhase = PHASE_TRANSITION;

                        SetCombatMovement(true);
                        m_creature->GetMotionMaster()->Clear();
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    }
                }

                if (m_uiTransformTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_DEMON_TRANSFORM_1) == CAST_OK)
                    {
                        m_uiTransformTimer = 12500;
                        m_uiPhase = PHASE_TRANSITION;

                        SetCombatMovement(true);
                        m_creature->GetMotionMaster()->Clear();
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    }
                }
                else
                    m_uiTransformTimer -= uiDiff;

                if (m_uiShadowDemonTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_SHADOW_DEMONS) == CAST_OK)
                        m_uiShadowDemonTimer = 60000;
                }
                else
                    m_uiShadowDemonTimer -= uiDiff;

                if (m_uiShadowBlastTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_SHADOW_BLAST) == CAST_OK)
                            m_uiShadowBlastTimer = urand(2000, 3000);
                    }
                }
                else
                    m_uiShadowBlastTimer -= uiDiff;

                if (m_uiFlameBurstTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FLAME_BURST) == CAST_OK)
                        m_uiFlameBurstTimer = 20000;
                }
                else
                    m_uiFlameBurstTimer -= uiDiff;

                break;
            case PHASE_TRANSITION:

                if (m_uiLandTimer)
                {
                    if (m_uiLandTimer <= uiDiff)
                    {
                        switch (m_uiLandStage)
                        {
                            case 0:
                                // Despawn the blades
                                for (GuidList::const_iterator itr = m_lBladesGuidList.begin(); itr != m_lBladesGuidList.end(); ++itr)
                                {
                                    if (Creature* pBlade = m_creature->GetMap()->GetCreature(*itr))
                                    {
                                        pBlade->CastSpell(m_creature, SPELL_GLAIVE_RETURNS, true);
                                        pBlade->ForcedDespawn(500);
                                    }
                                }
                                m_uiLandTimer = 5000;
                                break;
                            case 1:
                                // Set the equipment and land
                                SetEquipmentSlots(false, EQUIP_ID_MAIN_HAND, EQUIP_ID_OFF_HAND, EQUIP_NO_CHANGE);

                                m_creature->SetLevitate(false);
                                m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
                                m_uiLandTimer = 2000;
                                break;
                            case 2:
                                // Start phase 3
                                DoResetThreat();
                                m_uiPhase = PHASE_DUAL_NORMAL;

                                SetCombatMovement(true);
                                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                m_uiTransformTimer = 64000;
                                m_uiLandTimer = 0;
                                break;
                        }
                        ++m_uiLandStage;
                    }
                    else
                        m_uiLandTimer -= uiDiff;
                }

                if (m_uiTransformTimer)
                {
                    if (m_uiTransformTimer <= uiDiff)
                    {
                        // Drop the transform time from the spell timers
                        if (m_creature->HasAura(SPELL_DEMON_FORM))
                        {
                            DoResetThreat();
                            m_uiPhase = PHASE_DUAL_DEMON;
                            m_uiShadowDemonTimer = 17000;
                            m_uiFlameBurstTimer = 7000;
                            m_uiTransformTimer = 47000;
                        }
                        else
                        {
                            m_uiPhase = m_uiPrevPhase;
                            m_uiEnrageTimer = 40000;
                            m_uiTransformTimer = 60000;
                            m_uiTrapTimer = urand(30000, 40000);
                        }
                    }
                    else
                        m_uiTransformTimer -= uiDiff;
                }

                break;
        }
    }
};

/*######
## npc_akama_illidan
######*/

struct npc_akama_illidanAI : public npc_escortAI, private DialogueHelper
{
    npc_akama_illidanAI(Creature* pCreature) : npc_escortAI(pCreature),
        DialogueHelper(aIntroDialogue)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        InitializeDialogueHelper(m_pInstance);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiSummonMinionTimer;
    uint32 m_uiHealDelayTimer;
    uint32 m_uiChainLightningTimer;

    bool m_bFightMinions;
    bool m_bIsIntroFinished;

    void Reset() override
    {
        m_uiSummonMinionTimer   = 2000;
        m_uiHealDelayTimer      = 0;
        m_uiChainLightningTimer = urand(5000, 10000);

        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_bFightMinions         = false;
            m_bIsIntroFinished      = false;
        }
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        // Star the event
        if (pWho->GetTypeId() == TYPEID_PLAYER && pWho->IsWithinDistInMap(m_creature, 70.0f) && pWho->IsWithinLOSInMap(m_creature))
            Start(true);
    }

    void AttackStart(Unit* pWho) override
    {
        // Don't attack Illidan again
        if (m_bIsIntroFinished && pWho->GetEntry() == NPC_ILLIDAN_STORMRAGE)
            return;

        npc_escortAI::AttackStart(pWho);
    }

    void EnterEvadeMode() override
    {
        // Called first when evading from Illidan
        if (!m_bIsIntroFinished)
        {
            SetEscortPaused(false);
            m_bIsIntroFinished = true;
        }

        // Go back to epilogue position
        if (m_pInstance && m_pInstance->GetData(TYPE_ILLIDAN) == DONE)
        {
            SetEscortPaused(false);
            m_bFightMinions = false;
        }

        npc_escortAI::EnterEvadeMode();
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 0:
                SetEscortPaused(true);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
            case 9:
                SetEscortPaused(true);
                if (m_pInstance)
                {
                    if (Creature* pTrigger = m_pInstance->GetSingleCreatureFromStorage(NPC_ILLIDAN_DOOR_TRIGGER))
                        m_creature->SetFacingToObject(pTrigger);
                }
                StartNextDialogueText(SAY_AKAMA_OPEN_DOOR_1);
                break;
            case 16:
                SetEscortPaused(true);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                if (m_pInstance)
                {
                    if (Creature* pIllidan = m_pInstance->GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
                        m_creature->SetFacingToObject(pIllidan);
                }
                break;
            case 17:
                SetEscortPaused(true);
                if (m_pInstance)
                {
                    if (Creature* pIllidan = m_pInstance->GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
                    {
                        if (boss_illidan_stormrageAI* pIllidanAI = dynamic_cast<boss_illidan_stormrageAI*>(pIllidan->AI()))
                            pIllidanAI->DoStartCombatEvent();

                        m_creature->SetFacingToObject(pIllidan);
                    }
                }
                break;
            case 24:
                SetEscortPaused(true);
                m_bFightMinions = true;
                break;
            case 30:
                SetEscortPaused(true);
                if (m_pInstance)
                {
                    // Move to a close point to Illidan
                    if (Creature* pIllidan = m_pInstance->GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
                    {
                        float fX, fY, fZ;
                        pIllidan->GetContactPoint(m_creature, fX, fY, fZ);
                        m_creature->GetMotionMaster()->MovePoint(100, fX, fY, fZ);
                    }
                }
                break;
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        npc_escortAI::MovementInform(uiMoveType, uiPointId);

        if (uiMoveType != POINT_MOTION_TYPE || uiPointId != 100)
            return;

        // Do outro
        if (m_pInstance)
        {
            if (Creature* pIllidan = m_pInstance->GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
                m_creature->SetFacingToObject(pIllidan);
        }

        DoScriptText(SAY_AKAMA_EPILOGUE_5, m_creature);
        m_creature->ForcedDespawn(10000);
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case SPELL_AKAMA_DOOR_FAIL:
                DoCastSpellIfCan(m_creature, SPELL_AKAMA_DOOR_FAIL);
                break;
            case NPC_SPIRIT_OF_OLUM:
                m_creature->SummonCreature(NPC_SPIRIT_OF_OLUM, 751.64f, 297.22f, 312.21f, 6.03f, TEMPSUMMON_TIMED_DESPAWN, 25000);
                m_creature->SummonCreature(NPC_SPIRIT_OF_UDALO, 751.47f, 311.01f, 312.19f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 25000);
                break;
            case SPELL_AKAMA_DOOR_CHANNEL:
                DoCastSpellIfCan(m_creature, SPELL_AKAMA_DOOR_CHANNEL);
                if (m_pInstance)
                {
                    if (Creature* pOlum = m_pInstance->GetSingleCreatureFromStorage(NPC_SPIRIT_OF_OLUM))
                        pOlum->CastSpell(pOlum, SPELL_DEATHSWORN_DOOR_CHANNEL, true);
                    if (Creature* pUdalo = m_pInstance->GetSingleCreatureFromStorage(NPC_SPIRIT_OF_UDALO))
                        pUdalo->CastSpell(pUdalo, SPELL_DEATHSWORN_DOOR_CHANNEL, true);
                }
                break;
            case GO_ILLIDAN_GATE:
                if (m_pInstance)
                    m_pInstance->DoUseDoorOrButton(GO_ILLIDAN_GATE);
                break;
            case NPC_SPIRIT_OF_UDALO:
                SetEscortPaused(false);
                break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_ILLIDARI_ELITE:
                pSummoned->AI()->AttackStart(m_creature);
                break;
            case NPC_SPIRIT_OF_OLUM:
            case NPC_SPIRIT_OF_UDALO:
                pSummoned->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
        }
    }

    // Wrapper to handle event resume
    void DoResumeEvent()
    {
        SetEscortPaused(false);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (m_bFightMinions)
        {
            if (m_uiSummonMinionTimer < uiDiff)
            {
                for (uint8 i = 0; i < MAX_ILLIDARI_ELITES; ++i)
                    m_creature->SummonCreature(NPC_ILLIDARI_ELITE, aIllidariElitesPos[i].fX, aIllidariElitesPos[i].fY, aIllidariElitesPos[i].fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0);

                m_uiSummonMinionTimer = urand(35000, 50000);
            }
            else
                m_uiSummonMinionTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiHealDelayTimer)
        {
            if (m_uiHealDelayTimer <= uiDiff)
                m_uiHealDelayTimer = 0;
            else
                m_uiHealDelayTimer -= uiDiff;
        }

        if (m_bFightMinions)
        {
            if (m_uiChainLightningTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHAIN_LIGHTNING) == CAST_OK)
                    m_uiChainLightningTimer = urand(4000, 8000);
            }
            else
                m_uiChainLightningTimer -= uiDiff;
        }

        if (m_creature->GetHealthPercent() < 10.0f && !m_uiHealDelayTimer)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_HEALING_POTION) == CAST_OK)
                m_uiHealDelayTimer = 30000;
        }

        DoMeleeAttackIfReady();
    }
};

bool GossipHello_npc_akama_illidan(Player* pPlayer, Creature* pCreature)
{
    // Before climbing the stairs
    if (pCreature->GetPositionZ() < 300.0f)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_PREPARE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_AKAMA_ILLIDAN_PREPARE, pCreature->GetObjectGuid());
    }
    // Before starting combat
    else
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START_EVENT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_AKAMA_ILLIDAN_START, pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_akama_illidan(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1 || GOSSIP_ACTION_INFO_DEF + 2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        if (npc_akama_illidanAI* pAkamaAI = dynamic_cast<npc_akama_illidanAI*>(pCreature->AI()))
            pAkamaAI->DoResumeEvent();
    }

    return true;
}

/*######
## boss_maiev
######*/

struct boss_maievAI : public ScriptedAI, private DialogueHelper
{
    boss_maievAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aEpilogueDialogue)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        InitializeDialogueHelper(m_pInstance);
        Reset();
    };

    ScriptedInstance* m_pInstance;

    uint32 m_uiTauntTimer;
    uint32 m_uiShadowStriketimer;
    uint32 m_uiThrowDaggerTimer;

    bool m_bHasYelledTrap;

    void Reset() override
    {
        m_uiTauntTimer        = urand(40000, 60000);
        m_uiShadowStriketimer = urand(4000, 8000);
        m_uiThrowDaggerTimer  = urand(6000, 10000);
        m_bHasYelledTrap      = false;

        // Not sure if this is correct, but she seems to ignore all the shadow damage inflicted
        m_creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, true);
    }

    void KilledUnit(Unit* pVictim) override
    {
        // Dummy function - used to start the epilogue
        if (pVictim->GetEntry() == NPC_ILLIDAN_STORMRAGE)
            StartNextDialogueText(SAY_MAIEV_EPILOGUE_1);
    }

    // Custom evade - don't allow her to return to home position
    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(true);

        m_creature->SetLootRecipient(NULL);

        Reset();
    }

    // Attack only by script
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case NPC_ILLIDAN_STORMRAGE:
                if (m_pInstance)
                {
                    if (Creature* pIllidan = m_pInstance->GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
                        pIllidan->DealDamage(pIllidan, pIllidan->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                }
                break;
            case SPELL_TELEPORT_VISUAL:
                if (DoCastSpellIfCan(m_creature, SPELL_TELEPORT_VISUAL) == CAST_OK)
                    m_creature->ForcedDespawn(1000);
                break;
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_CAGE_TRAP)
        {
            // Yell only the first time
            if (!m_bHasYelledTrap)
            {
                DoScriptText(SAY_MAIEV_TRAP, m_creature);
                m_bHasYelledTrap = true;
            }
            DoCastSpellIfCan(m_creature, SPELL_CAGE_TRAP_SUMMON, CAST_TRIGGERED);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiTauntTimer < uiDiff)
        {
            switch (urand(0, 2))
            {
                case 0: DoScriptText(SAY_MAIEV_TAUNT_1, m_creature); break;
                case 1: DoScriptText(SAY_MAIEV_TAUNT_2, m_creature); break;
                case 2: DoScriptText(SAY_MAIEV_TAUNT_3, m_creature); break;
            }
            m_uiTauntTimer = urand(40000, 60000);
        }
        else
            m_uiTauntTimer -= uiDiff;

        if (m_uiShadowStriketimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_STRIKE) == CAST_OK)
                m_uiShadowStriketimer = urand(12000, 16000);
        }
        else
            m_uiShadowStriketimer -= uiDiff;

        if (m_uiThrowDaggerTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_THROW_DAGGER) == CAST_OK)
                m_uiThrowDaggerTimer = urand(6000, 10000);
        }
        else
            m_uiThrowDaggerTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_cage_trap_trigger
######*/

struct npc_cage_trap_triggerAI : public ScriptedAI
{
    npc_cage_trap_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_bActive;

    void Reset() override
    {
        m_bActive = false;
    }

    void AttackStart(Unit* /*pWho*/) override { }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bActive && pWho->GetEntry() == NPC_ILLIDAN_STORMRAGE && m_creature->IsWithinDistInMap(pWho, 3.0f))
        {
            pWho->CastSpell(pWho, SPELL_CAGED, true);

            // Cast the visual effects
            for (uint8 i = 0; i < MAX_CAGE_SPELLS; ++i)
                DoCastSpellIfCan(m_creature, aCagedSummonSpells[i], CAST_TRIGGERED);

            for (uint8 i = 0; i < MAX_CAGE_SPELLS; ++i)
                DoCastSpellIfCan(m_creature, aCagedVisualSpells[i], CAST_TRIGGERED);

            if (GameObject* pCageTrap = GetClosestGameObjectWithEntry(m_creature, GO_CAGE_TRAP, 5.0f))
                pCageTrap->Use(m_creature);

            m_bActive = true;
            m_creature->ForcedDespawn(15000);
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

/*######
## npc_flame_of_azzinoth
######*/

struct npc_flame_of_azzinothAI : public ScriptedAI
{
    npc_flame_of_azzinothAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiFlameBlastTimer;
    uint32 m_uiSummonBlazeTimer;
    uint32 m_uiChargeTimer;
    uint32 m_uiWrathCheckTimer;

    void Reset() override
    {
        m_uiFlameBlastTimer  = 10000;
        m_uiSummonBlazeTimer = 0;
        m_uiChargeTimer      = 5000;
        m_uiWrathCheckTimer  = 1000;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_BLAZE)
            pSummoned->CastSpell(pSummoned, SPELL_BLAZE_EFFECT, false);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFlameBlastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FLAME_BLAST) == CAST_OK)
            {
                m_uiFlameBlastTimer = 10000;
                m_uiSummonBlazeTimer = 3000;
            }
        }
        else
            m_uiFlameBlastTimer -= uiDiff;

        if (m_uiSummonBlazeTimer)
        {
            if (m_uiSummonBlazeTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BLAZE) == CAST_OK)
                    m_uiSummonBlazeTimer = 0;
            }
            else
                m_uiSummonBlazeTimer -= uiDiff;
        }

        // Workaround for broken aura 41997; the creature should enrage if not within dist of 30 from summoner
        // This should be done by checking if aura 41997 is removed from self, when getting out of range
        if (m_uiWrathCheckTimer)
        {
            if (m_uiWrathCheckTimer <= uiDiff)
            {
                if (GetClosestCreatureWithEntry(m_creature, NPC_BLADE_OF_AZZINOTH, 30.0f))
                    m_uiWrathCheckTimer = 1000;
                else
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_UNCAGED_WRATH, CAST_TRIGGERED) == CAST_OK)
                        m_uiWrathCheckTimer = 0;
                }
            }
            else
                m_uiWrathCheckTimer -= uiDiff;
        }

        // Try to find a suitable target to charge
        if (m_uiChargeTimer < uiDiff)
        {
            std::vector<Unit*> suitableTargets;
            ThreatList const& threatList = m_creature->getThreatManager().getThreatList();

            for (ThreatList::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
            {
                if (Unit* pTarget = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid()))
                {
                    if (pTarget->GetTypeId() == TYPEID_PLAYER && !pTarget->IsWithinDist(m_creature, 30.0f))
                        suitableTargets.push_back(pTarget);
                }
            }

            if (suitableTargets.empty())
                m_uiChargeTimer = 3000;
            else
            {
                Unit* pTarget = suitableTargets[urand(0, suitableTargets.size() - 1)];

                if (pTarget)
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_CHARGE) == CAST_OK)
                        m_uiChargeTimer = urand(5000, 10000);
                }
            }
        }
        else
            m_uiChargeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_shadow_demon
######*/

struct npc_shadow_demonAI : public ScriptedAI
{
    npc_shadow_demonAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_targetGuid;

    void Reset() override {}

    void AttackStart(Unit* pWho) override
    {
        // Function used to set target only - the npc doesn't really attack
        m_targetGuid = pWho->GetObjectGuid();
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_targetGuid))
            pPlayer->RemoveAurasDueToSpell(SPELL_PARALYZE);
    }

    void MovementInform(uint32 uiMovementType, uint32 uiPointId) override
    {
        if (uiMovementType != POINT_MOTION_TYPE || !uiPointId)
            return;

        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_targetGuid))
        {
            if (DoCastSpellIfCan(pPlayer, SPELL_CONSUME_SOUL) == CAST_OK)
                m_creature->ForcedDespawn(1000);
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

/*######
## npc_blade_of_azzinoth
######*/

struct npc_blade_of_azzinothAI : public ScriptedAI
{
    npc_blade_of_azzinothAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() override {}

    // Do-Nothing-But-Stand-There
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustSummoned(Creature* pSummoned) override
    {
        // Summon a Flame pear each blade
        if (pSummoned->GetEntry() == NPC_FLAME_OF_AZZINOTH)
        {
            DoCastSpellIfCan(pSummoned, SPELL_AZZINOTH_CHANNEL, CAST_TRIGGERED);
            pSummoned->SetInCombatWithZone();
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        // Inform Illidan when a flame is killed
        if (pSummoned->GetEntry() == NPC_FLAME_OF_AZZINOTH)
        {
            if (!m_pInstance)
                return;

            // For some reason it doesn't work with Spell Hit for SPELL_GLAIVE_RETURNS script effect, so we need to inform him manually
            if (Creature* pIllidan = m_pInstance->GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
            {
                if (boss_illidan_stormrageAI* pIllidanAI = dynamic_cast<boss_illidan_stormrageAI*>(pIllidan->AI()))
                    pIllidanAI->DoInformFlameKilled();
            }
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_boss_illidan_stormrage(Creature* pCreature)
{
    return new boss_illidan_stormrageAI(pCreature);
}

CreatureAI* GetAI_npc_akama_illidan(Creature* pCreature)
{
    return new npc_akama_illidanAI(pCreature);
}

CreatureAI* GetAI_boss_maiev(Creature* pCreature)
{
    return new boss_maievAI(pCreature);
}

CreatureAI* GetAI_mob_flame_of_azzinoth(Creature* pCreature)
{
    return new npc_flame_of_azzinothAI(pCreature);
}

CreatureAI* GetAI_npc_cage_trap_trigger(Creature* pCreature)
{
    return new npc_cage_trap_triggerAI(pCreature);
}

CreatureAI* GetAI_npc_shadow_demon(Creature* pCreature)
{
    return new npc_shadow_demonAI(pCreature);
}

CreatureAI* GetAI_npc_blade_of_azzinoth(Creature* pCreature)
{
    return new npc_blade_of_azzinothAI(pCreature);
}

void AddSC_boss_illidan()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_illidan_stormrage";
    pNewScript->GetAI = &GetAI_boss_illidan_stormrage;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_akama_illidan";
    pNewScript->GetAI = &GetAI_npc_akama_illidan;
    pNewScript->pGossipHello = &GossipHello_npc_akama_illidan;
    pNewScript->pGossipSelect = &GossipSelect_npc_akama_illidan;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_maiev_shadowsong";
    pNewScript->GetAI = &GetAI_boss_maiev;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_flame_of_azzinoth";
    pNewScript->GetAI = &GetAI_mob_flame_of_azzinoth;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_blade_of_azzinoth";
    pNewScript->GetAI = &GetAI_npc_blade_of_azzinoth;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_cage_trap_trigger";
    pNewScript->GetAI = &GetAI_npc_cage_trap_trigger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_shadow_demon";
    pNewScript->GetAI = &GetAI_npc_shadow_demon;
    pNewScript->RegisterSelf();
}

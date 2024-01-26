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
SDName: Boss_Kaelthas
SD%Complete: 80
SDComment: Timers; Transition phase is incomplete, some spells are unk.
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "the_eye.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/SpellAuras.h"

//#define FAST_TIMERS
//#define FAST_TRANSITION_TIMERS

enum
{
    // ***** Event yells ********
    // kael'thas Speech
    SAY_INTRO                           = 20195, // differs in wotlk+
    SAY_INTRO_CAPERNIAN                 = 19004,
    SAY_INTRO_TELONICUS                 = 19005,
    SAY_INTRO_THALADRED                 = 19006,
    SAY_INTRO_SANGUINAR                 = 19003,
    SAY_PHASE2_WEAPON                   = 19053,
    SAY_PHASE3_ADVANCE                  = 19060,
    SAY_PHASE4_INTRO2                   = 19195,
    SAY_PHASE5_NUTS                     = 20208,
    SAY_SLAY1                           = 20204,
    SAY_SLAY2                           = 20205,
    SAY_SLAY3                           = 20206,
    SAY_MINDCONTROL1                    = 20202,
    SAY_MINDCONTROL2                    = 18259,
    SAY_GRAVITYLAPSE1                   = 20198,
    SAY_GRAVITYLAPSE2                   = 20199,
    SAY_SUMMON_PHOENIX1                 = 20201,
    SAY_SUMMON_PHOENIX2                 = 20200,
    SAY_DEATH                           = 20207,
    EMOTE_PYROBLAST                     = 20775,

    // Thaladred the Darkener speech
    SAY_THALADRED_AGGRO                 = 20905,
    SAY_THALADRED_DEATH                 = 20904,
    EMOTE_THALADRED_GAZE                = 11074,

    // Lord Sanguinar speech
    SAY_SANGUINAR_AGGRO                 = 20859,
    SAY_SANGUINAR_DEATH                 = 20858,

    // Grand Astromancer Capernian speech
    SAY_CAPERNIAN_AGGRO                 = 20847,
    SAY_CAPERNIAN_DEATH                 = 20848,

    // Master Engineer Telonicus speech
    SAY_TELONICUS_AGGRO                 = 20860,
    SAY_TELONICUS_DEATH                 = 20861,

    // ***** Kaelthas spells ********
    // Phase 2 spells
    SPELL_KAEL_PHASE_2                  = 36709,            // not sure if this is used in the right way
    SPELL_SUMMON_WEAPONS                = 36976,
    SPELL_SUMMON_WEAPONA                = 36958,
    SPELL_SUMMON_WEAPONB                = 36959,
    SPELL_SUMMON_WEAPONC                = 36960,
    SPELL_SUMMON_WEAPOND                = 36961,
    SPELL_SUMMON_WEAPONE                = 36962,
    SPELL_SUMMON_WEAPONF                = 36963,
    SPELL_SUMMON_WEAPONG                = 36964,
    SPELL_RESURRECTION                  = 36450,

    // Phase 4 spells
    SPELL_FIREBALL                      = 36805,
    SPELL_PYROBLAST                     = 36819,
    SPELL_FLAME_STRIKE                  = 36735,            // summons 21369
    SPELL_FLAME_STRIKE_DUMMY            = 36730,
    SPELL_ARCANE_DISRUPTION             = 36834,
    SPELL_SHOCK_BARRIER                 = 36815,
    SPELL_PHOENIX_ANIMATION             = 36723,            // summons 21362
    SPELL_MIND_CONTROL                  = 36797,            // applies 36798
    SPELL_MIND_CONTROL2                 = 36798,

    // Phase 4 to 5 transition
    SPELL_NETHERBEAM_EXPLODE            = 36089,
    SPELL_NETHERBEAM                    = 36090,
    SPELL_KAEL_GAIN_POWER               = 36091,
    SPELL_EXPLODE                       = 36092,
    SPELL_KAEL_STUN                     = 36185,
    SPELL_KAEL_FULL_POWER               = 36187,

    SPELL_PURE_NETHER_BEAM1             = 36196,            // random spells used for visuals on kael during stunned sequence
    SPELL_PURE_NETHER_BEAM2             = 36197,
    SPELL_PURE_NETHER_BEAM3             = 36198,

    SPELL_PURE_NETHER_BEAM4             = 36201,            // searching spells for pure nether beam
    SPELL_PURE_NETHER_BEAM5             = 36290,
    SPELL_PURE_NETHER_BEAM6             = 36291,

    SPELL_NETHERBEAM_GLOW1              = 36364,            // all 3 used by kael during ascention sequence
    SPELL_NETHERBEAM_GLOW2              = 36370,
    SPELL_NETHERBEAM_GLOW3              = 36371,

    SPELL_EXPLODE_1                     = 36354,            // used by specific guids at select intervals
    SPELL_EXPLODE_2                     = 36373,
    SPELL_EXPLODE_3                     = 36375,
    SPELL_EXPLODE_4                     = 36376,

    SPELL_FLOATING_DROWNED              = 36550,            // guesswork based on video confirmation, now on retail one spell handles all - changed visual


    // Phase 5 spells
    SPELL_GRAVITY_LAPSE                 = 35941,
    SPELL_GRAVITY_LAPSE_KNOCKBACK       = 34480,            // cast by players - damage effect
    SPELL_GRAVITY_LAPSE_AURA            = 39432,            // cast by players - fly effect
    SPELL_NETHER_BEAM                   = 35869,            // triggers 35873 on target
    SPELL_NETHER_BEAM_DAMAGE            = 35873,
    SPELL_NETHER_VAPOR_SUMMON           = 35865,            // script effect - casts all 4 summon spells
    SPELL_NETHER_VAPOR_PERIODIC_DAMAGE  = 35858,            // triggers 35859 damage portion
    SPELL_NETHER_VAPOR_PERIODIC_SCRIPT  = 35879,            // triggers 35880, cast at spawn
    SPELL_NETHER_VAPOR_SUMMON_1         = 35861,            // 4 spells to summon at 4 directions from caster
    SPELL_NETHER_VAPOR_SUMMON_2         = 35862,
    SPELL_NETHER_VAPOR_SUMMON_3         = 35863,
    SPELL_NETHER_VAPOR_SUMMON_4         = 35864,
    SPELL_NETHER_VAPOR_LIGHTNING        = 45960,
    SPELL_ASTRAL_STORM                  = 45959,
    SPELL_REMOVE_WEAPONS                = 39497,            // spells to delete items from player

    // ***** Advisors spells ********
    // Thaladred the Darkener spells
    SPELL_PSYCHIC_BLOW                  = 36966,
    SPELL_SILENCE                       = 30225,
    SPELL_REND                          = 36965,
    SPELL_TRIGGER_CREATURE_SPECIAL      = 40373,            // guesswork, modelled after buru - using spell based on fact gaze executes during spell list tick

    SPELL_SET_THALADRED_REZZED          = 2006402,

    // Lord Sanguinar spells
    SPELL_THRASH                        = 8876,
    SPELL_BELLOWING_ROAR                = 44863,

    // Grand Astromancer Capernian spells
    SPELL_CAPERNIAN_FIREBALL            = 36971,
    SPELL_CONFLAGRATION                 = 37018,
    SPELL_ARCANE_BURST                  = 36970,

    // Master Engineer Telonicus spells
    SPELL_BOMB                          = 37036,
    SPELL_REMOTE_TOY                    = 37027,
    SPELL_SHOOT                         = 16496,            // Only used when at max range, 25-30yd

    // ***** Other spells ********
    // Utilities
    SPELL_PERMANENT_FEIGN_DEATH         = 29266,            // placed upon advisors on fake death
    SPELL_BANISH                        = 40370,

    // ***** Creature Entries ********
    NPC_FLAME_STRIKE_TRIGGER            = 21369,
    NPC_PHOENIX                         = 21362,
    NPC_PHOENIX_EGG                     = 21364,
    NPC_NETHER_VAPOR                    = 21002,

    // ***** Other ********
    PHASE_0_NOT_BEGUN                   = 0,
    PHASE_1_ADVISOR                     = 1,
    PHASE_2_WEAPON                      = 2,
    PHASE_3_ADVISOR_ALL                 = 3,
    PHASE_4_SOLO                        = 4,
    PHASE_TRANSITION                    = 5,
    PHASE_5_GROUND                      = 6,
    PHASE_5_GRAVITY                     = 7,

    POINT_ID_CENTER                     = 1,
    POINT_ID_AIR                        = 2,
    POINT_ID_LAND                       = 3,

    SOUND_PHASE_TRANSITION1             = 27,
    SOUND_PHASE_TRANSITION2             = 1136,
    SOUND_PHASE_TRANSITION3             = 6477,
    SOUND_PHASE_TRANSITION4             = 44,

    VISUAL_PHASE_TRANSITION1            = 7234,
    VISUAL_PHASE_TRANSITION2            = 7235,

    MAX_WEAPONS                         = 7,
    MAX_MIND_CONTROL                    = 3,

    SPELL_SET_PHASE_4                   = 1962201,
    SPELL_SET_PHASE_5_INITIAL           = 1962202,
    SPELL_SET_PHASE_5_GRAVITY           = 1962203,
    SPELL_SET_PHASE_5_GROUND            = 1962204,
};

static const uint32 m_spellSummonWeapon[MAX_WEAPONS] =
{
    SPELL_SUMMON_WEAPONA, SPELL_SUMMON_WEAPONB, SPELL_SUMMON_WEAPONC, SPELL_SUMMON_WEAPOND,
    SPELL_SUMMON_WEAPONE, SPELL_SUMMON_WEAPONF, SPELL_SUMMON_WEAPONG
};

// teleport spells for gravity lapse event
static const uint32 m_spellGravityLapseTeleport[] =
{
    35966, 35967, 35968, 35969, 35970, 35971, 35972, 35973, 35974, 35975, 35976, 35977, 35978, 35979, 35980,
    35981, 35982, 35983, 35984, 35985, 35986, 35987, 35988, 35989, 35990
};

static const float aCenterPos[3] = { 796.641f, -0.588817f, 48.72847f};
static const float flightPos[3] = { 795.007f, -0.471827f, 75.f };
static const float landPos[3] = { 792.419f, -0.504778f, 50.0505f };

enum TriggerData
{
    TRIGGERS_FIRST_STAGE    = 6,
    TRIGGERS_EXPLOSION      = 6,
    TRIGGERS_SECOND_STAGE   = 13,
    TRIGGERS_TOTAL          = 25,

    TRIGGERS_GROUP_ONE = 6,
    TRIGGERS_GROUP_TWO = 4,
    TRIGGERS_GROUP_THREE = 3,
};

//                                              0      1      2      3      4      5      6      7      8      9     10     11     12
static const uint32 triggerSpellIds[13] = { 36201, 36201, 36201, 36290, 36290, 36290, 36290, 36290, 36290, 36290, 36291, 36291, 36291};

// determines to which cycle which trigger belongs
static const uint32 groupOneIndices[TRIGGERS_GROUP_ONE]      = { 0, 1, 4, 7, 8, 9};
static const uint32 groupTwoIndices[TRIGGERS_GROUP_TWO]      = { 2, 4, 9, 10};
static const uint32 groupThreeIndices[TRIGGERS_GROUP_THREE]  = { 5, 11, 12};

/*######
## boss_kaelthas
######*/

enum KaelThasActions
{
    KAEL_ACTION_PYROBLAST_SEQUENCE,
    KAEL_ACTION_PHASE_5,
    KAEL_ACTION_MAX,
    KAEL_PHASE_ONE,
    KAEL_PHASE_TWO,
    KAEL_PHASE_THREE,
    KAEL_PHASE_TRANSITION,
    KAEL_PHASE_TRANSITION_GROWTH,
    KAEL_PHASE_TRANSITION_NETHER_BEAM,
    KAEL_WEAPON_ATTACK,
    KAEL_ACTION_GRAVITY_LAPSE_END,
    KAEL_RESPAWN_ADVISORS,
};

struct boss_kaelthasAI : public CombatAI
{
    boss_kaelthasAI(Creature* creature) : CombatAI(creature, KAEL_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddOnKillText(SAY_SLAY1, SAY_SLAY2, SAY_SLAY3);
        AddTimerlessCombatAction(KAEL_ACTION_PHASE_5, true);
        AddCombatAction(KAEL_ACTION_PYROBLAST_SEQUENCE, true);
#ifdef FAST_TIMERS
        AddCustomAction(KAEL_PHASE_ONE, 5000u, [&]() { HandlePhaseOne(); }, TIMER_COMBAT_COMBAT);
#else
        AddCustomAction(KAEL_PHASE_ONE, 23000u, [&]() { HandlePhaseOne(); }, TIMER_COMBAT_COMBAT);
#endif
        AddCustomAction(KAEL_WEAPON_ATTACK, true, [&]() { HandleWeaponAttack(); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(KAEL_PHASE_TWO, true, [&]() { HandlePhaseTwoEnd(); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(KAEL_PHASE_THREE, true, [&]() { HandlePhaseThree(); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(KAEL_PHASE_TRANSITION, true, [&]() { HandlePhaseTransition(); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(KAEL_PHASE_TRANSITION_GROWTH, true, [&]() { HandlePhaseTransitionGrowth(); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(KAEL_PHASE_TRANSITION_NETHER_BEAM, true, [&]() { HandlePhaseTransitionNetherBeam(); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(KAEL_ACTION_GRAVITY_LAPSE_END, true, [&]() { HandleGravityLapseEnd(); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(KAEL_RESPAWN_ADVISORS, 1000u, [&]() { RespawnAdvisors(); }, TIMER_ALWAYS);
    }

    ScriptedInstance* m_instance;

    uint32 m_pyroblastCounter;

    uint8 m_uiGravityIndex;

    uint8 m_uiPhase;
    uint8 m_uiPhaseSubphase;

    uint32 m_phaseTransitionStage;

    uint32 m_phaseTransitionGrowthStage;
    uint32 m_phaseTransitionPureNetherBeamStage;

    GuidVector m_worldTriggersFirstStage;
    GuidVector m_worldTriggersExplosions;
    GuidVector m_worldTriggersSecondStage;

    GuidVector m_netherVapor;
    GuidList m_summonedGuidList;
    GuidVector m_weapons;

    GuidSet m_charmTargets;

    uint32 m_advisorsAlive;

    bool m_actionReadyStatus[KAEL_ACTION_MAX];

    bool m_rangeMode;

    void Reset() override
    {
        CombatAI::Reset();
        // Phases
        m_uiPhase                   = PHASE_0_NOT_BEGUN;
        m_uiPhaseSubphase           = 0;

        // Spells
        m_uiGravityIndex            = 0;

        m_phaseTransitionGrowthStage = 0;

        m_phaseTransitionPureNetherBeamStage = 0;

        m_phaseTransitionStage = 0;

        m_advisorsAlive = 4;

        SetRangedMode(true, 35.f, TYPE_PROXIMITY);

        SetCombatMovement(true);
        DoDespawnSummons();

        m_rangeMode = true;
        SetMeleeEnabled(true);

        ResetSize();

        SetCombatScriptStatus(false);

        m_charmTargets.clear();

        m_creature->SetSpellList(m_creature->GetCreatureInfo()->SpellList);
    }

    void ResetSize()
    {
        // reset size and speed from phase 5
        m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.f);
        m_creature->UpdateModelData(); // also resets speed
        m_creature->UpdateSpeed(MOVE_RUN, true);
    }

    void RespawnAdvisors()
    {
        if (m_instance)
            for (unsigned int aAdvisor : aAdvisors)
                if (Creature* add = m_instance->GetSingleCreatureFromStorage(aAdvisor))
                    add->Respawn();
    }

    void GetAIInformation(ChatHandler& reader) override
    {
        reader.PSendSysMessage("Kael'thas is currently in phase %u", uint32(m_uiPhase));
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_B)
        {
            DoDespawnSummons();
        }
    }

    void DoDespawnSummons()
    {
        for (GuidList::const_iterator itr = m_summonedGuidList.begin(); itr != m_summonedGuidList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
            {
                if (pTemp->GetEntry() == NPC_PHOENIX)
                    m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pTemp);
                pTemp->ForcedDespawn();
            }
        }

        m_netherVapor.clear();
        m_summonedGuidList.clear();
    }

    // Custom Move in LoS function
    void MoveInLineOfSight(Unit* who) override
    {
        if (m_uiPhase == PHASE_0_NOT_BEGUN && who->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED) &&
                m_creature->IsWithinDistInMap(who, m_creature->GetAttackDistance(who)) && m_creature->IsWithinLOSInMap(who) && m_creature->CanAttackOnSight(who))
        {
            if (who->IsPlayer() && static_cast<Player*>(who)->IsGameMaster())
                return;
            DoBroadcastText(SAY_INTRO, m_creature);
            m_creature->CastSpell(nullptr, SPELL_REMOVE_WEAPONS, TRIGGERED_OLD_TRIGGERED);
            m_uiPhase = PHASE_1_ADVISOR;

            // Set the player in combat with the boss
            SetCombatMovement(false);
            SetCombatScriptStatus(true);
            m_creature->SetInCombatWithZone();

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

            if (m_instance)
                m_instance->SetData(TYPE_KAELTHAS, IN_PROGRESS);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);
        m_creature->CastSpell(nullptr, SPELL_REMOVE_WEAPONS, TRIGGERED_OLD_TRIGGERED);

        if (m_instance)
            m_instance->SetData(TYPE_KAELTHAS, DONE);

        DoDespawnSummons();
        ResetSize();
    }

    void EnterEvadeMode() override
    {
        m_creature->CastSpell(nullptr, SPELL_REMOVE_WEAPONS, TRIGGERED_OLD_TRIGGERED);

        if (m_instance)
            m_instance->SetData(TYPE_KAELTHAS, FAIL);

        m_creature->SetRespawnDelay(30, true);
        m_creature->ForcedDespawn();
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_FLAME_STRIKE_TRIGGER:
            {
                summoned->AI()->SetCombatMovement(false);
                summoned->AI()->SetMeleeEnabled(false);
                DoCastSpellIfCan(summoned, SPELL_FLAME_STRIKE_DUMMY, CAST_FORCE_TARGET_SELF | CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
                break;
            }
            case NPC_NETHER_VAPOR:
            {
                m_netherVapor.push_back(summoned->GetObjectGuid());
                break;
            }
            default: // Weapons
            {
                m_weapons.push_back(summoned->GetObjectGuid());
                ResetTimer(KAEL_WEAPON_ATTACK, 2000);
                summoned->AI()->SetReactState(REACT_PASSIVE);
                summoned->SetCorpseDelay(60);
                break;
            }
        }
            
        m_summonedGuidList.push_back(summoned->GetObjectGuid());
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spellInfo) override
    {
        // Handle summon weapons event
        if (spellInfo->Id == SPELL_SUMMON_WEAPONS)
        {
            for (unsigned int i : m_spellSummonWeapon)
                DoCastSpellIfCan(m_creature, i, CAST_TRIGGERED);

            m_uiPhase      = PHASE_2_WEAPON;
            uint32 timer = 120000;
#ifdef FAST_TIMERS
            timer = 10000;
#elif defined(PRENERF_2_0_3)
            timer = 90000; // very early pre 2.1 - 90s (90000), later 120s (120000);
#endif
            ResetTimer(KAEL_PHASE_TWO, timer);
        }
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spellInfo) override
    {
        // Handle gravity lapse teleport - each player hit has his own teleport spell
        if (spellInfo->Id == SPELL_GRAVITY_LAPSE && target->IsPlayer())
        {
            DoCastSpellIfCan(target, m_spellGravityLapseTeleport[m_uiGravityIndex], CAST_TRIGGERED);
            target->CastSpell(nullptr, SPELL_GRAVITY_LAPSE_KNOCKBACK, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(nullptr, SPELL_GRAVITY_LAPSE_AURA, TRIGGERED_OLD_TRIGGERED);
            ++m_uiGravityIndex;
        }

        if (spellInfo->Id == SPELL_MIND_CONTROL)
            m_charmTargets.insert(target->GetObjectGuid());
    }

    void OnSpellInterrupt(SpellEntry const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_PYROBLAST)
            DisableCombatAction(KAEL_ACTION_PYROBLAST_SEQUENCE);
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE || !pointId)
            return;

        switch (pointId)
        {
            case POINT_ID_CENTER:
            {
                // ToDo: also start channeling to the giant crystals nearby
                DoBroadcastText(SAY_PHASE5_NUTS, m_creature);
                m_creature->SetFacingTo(3.176499f);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                ResetTimer(KAEL_PHASE_TRANSITION, 2000);

                if (m_worldTriggersFirstStage.empty())
                {
                    GuidVector triggers;
                    m_instance->GetCreatureGuidVectorFromStorage(NPC_WORLD_TRIGGER_LARGE, triggers);

                    if (triggers.size() < TRIGGERS_TOTAL)
                        sLog.outErrorDb("Kaelthas Sunstrider script is missing World Trigger Large NPCs, current count %lu, expected count %u", triggers.size(), TRIGGERS_TOTAL);

                    std::sort(triggers.begin(), triggers.end(), [](const ObjectGuid& a, const ObjectGuid& b) -> bool
                    {
                        return a.GetCounter() < b.GetCounter();
                    }); // sort based on guid, first 6 are first stage triggers, rest is second stage

                    // first 6
                    for (uint32 i = 0; i < TRIGGERS_FIRST_STAGE; ++i)
                        m_worldTriggersFirstStage.push_back(triggers[i]);
                    // second 6
                    for (uint32 i = TRIGGERS_FIRST_STAGE; i < TRIGGERS_FIRST_STAGE + TRIGGERS_EXPLOSION; ++i)
                        m_worldTriggersExplosions.push_back(triggers[i]);
                    // last 13
                    for (uint32 i = TRIGGERS_FIRST_STAGE + TRIGGERS_EXPLOSION; i < TRIGGERS_TOTAL; ++i)
                        m_worldTriggersSecondStage.push_back(triggers[i]);
                }
                break;
            }
            case POINT_ID_LAND:
            {
                m_creature->SetLevitate(false);
                m_creature->SetHover(false);
                SetCombatScriptStatus(false);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                m_creature->RemoveAurasDueToSpell(SPELL_KAEL_FULL_POWER);
                m_uiPhase = PHASE_5_GROUND;
                SetCombatMovement(true);
                m_creature->SetSpellList(SPELL_SET_PHASE_5_INITIAL);
                if (Unit* victim = m_creature->GetVictim())
                {
                    m_creature->SetTarget(victim);
                    DoStartMovement(victim);
                }
                break;
            }
        }
    }

    void AdvisorDefeated(uint32 uiEntry)
    {
        if (m_uiPhase != PHASE_1_ADVISOR)
            return;

        // Handle phase 1 end
        if (uiEntry == NPC_TELONICUS)
            ResetTimer(KAEL_PHASE_ONE, 3000);
        else
            ResetTimer(KAEL_PHASE_ONE, 1000);
    }

    void AdvisorDied()
    {
        --m_advisorsAlive;
        if (m_advisorsAlive == 0)
            ReduceTimer(KAEL_PHASE_THREE, 3000);
    }

    void HandleGravityLapseEnd()
    {
        SetCombatMovement(true);
        SetMeleeEnabled(true);
        DespawnGuids(m_netherVapor);
        m_creature->SetSpellList(SPELL_SET_PHASE_5_GROUND);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_SHOCK_BARRIER:
                if (m_uiPhase != PHASE_4_SOLO)
                    break;
                ResetCombatAction(KAEL_ACTION_PYROBLAST_SEQUENCE, 1);
                break;
            case SPELL_MIND_CONTROL:
                DoBroadcastText(urand(0, 1) ? SAY_MINDCONTROL1 : SAY_MINDCONTROL2, m_creature);
                break;
            case SPELL_PHOENIX_ANIMATION:
                DoBroadcastText(urand(0, 1) ? SAY_SUMMON_PHOENIX1 : SAY_SUMMON_PHOENIX2, m_creature);
                break;
            case SPELL_GRAVITY_LAPSE:
            {
                m_creature->CastSpell(nullptr, SPELL_SHOCK_BARRIER, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_COOLDOWNS);
                DoBroadcastText(urand(0, 1) ? SAY_GRAVITYLAPSE1 : SAY_GRAVITYLAPSE2, m_creature);
                m_creature->CastSpell(nullptr, SPELL_BANISH, TRIGGERED_OLD_TRIGGERED);
                SetMeleeEnabled(false);
                m_creature->SetTarget(nullptr);
                SetCombatMovement(false);
                m_uiGravityIndex = 0;
                ResetTimer(KAEL_ACTION_GRAVITY_LAPSE_END, 35000);
                m_creature->SetSpellList(SPELL_SET_PHASE_5_GRAVITY);
                break;
            }                
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KAEL_ACTION_PHASE_5:
            {
                if (m_creature->GetHealthPercent() < 50.0f)
                {
                    // ToDo: should he cast something here?
                    m_creature->InterruptNonMeleeSpells(false);
                    m_creature->SetTarget(nullptr);
                    SetCombatScriptStatus(true);
                    SetCombatMovement(false);

                    m_creature->GetMotionMaster()->MovePoint(POINT_ID_CENTER, aCenterPos[0], aCenterPos[1], aCenterPos[2], FORCED_MOVEMENT_RUN);

                    ResetTimer(KAEL_PHASE_TRANSITION, 30000);
                    m_uiPhase = PHASE_TRANSITION;
                    DisableCombatAction(action);
                    return;
                }
                break;
            }
            case KAEL_ACTION_PYROBLAST_SEQUENCE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_PYROBLAST) == CAST_OK)
                {
                    DoBroadcastText(EMOTE_PYROBLAST, m_creature);
                    if (m_pyroblastCounter < 2)
                        m_pyroblastCounter++;
                    else
                        DisableCombatAction(action);
                    return;
                }
                break;
            }
        }
    }

    void HandlePhaseOne()
    {
        if (!m_instance)
            return;

        uint32 timer = 0;
        switch (m_uiPhaseSubphase)
        {
            case 0:
                DoBroadcastText(SAY_INTRO_THALADRED, m_creature);
#ifdef FAST_TIMERS
                timer = 1000;
#else
                timer = 7000;
#endif
                break;
            case 1:
                if (Creature* advisor = m_instance->GetSingleCreatureFromStorage(NPC_THALADRED))
                {
                    advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                    advisor->SetInCombatWithZone();
                    advisor->AI()->AttackClosestEnemy();
                }
                break;
            case 2:
                DoBroadcastText(SAY_INTRO_SANGUINAR, m_creature);
#ifdef FAST_TIMERS
                timer = 1000;
#else
                timer = 12500;
#endif
                break;
            case 3:
                if (Creature* advisor = m_instance->GetSingleCreatureFromStorage(NPC_SANGUINAR))
                {
                    advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                    advisor->SetInCombatWithZone();
                    advisor->AI()->AttackClosestEnemy();
                }
                break;
            case 4:
                DoBroadcastText(SAY_INTRO_CAPERNIAN, m_creature);
#ifdef FAST_TIMERS
                timer = 1000;
#else
                timer = 7000;
#endif
                break;
            case 5:
                if (Creature* advisor = m_instance->GetSingleCreatureFromStorage(NPC_CAPERNIAN))
                {
                    advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                    advisor->SetInCombatWithZone();
                    advisor->AI()->AttackClosestEnemy();
                }
                break;
            case 6:
                DoBroadcastText(SAY_INTRO_TELONICUS, m_creature);
#ifdef FAST_TIMERS
                timer = 1000;
#else
                timer = 8400;
#endif
                break;
            case 7:
                if (Creature* advisor = m_instance->GetSingleCreatureFromStorage(NPC_TELONICUS))
                {
                    advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                    advisor->SetInCombatWithZone();
                    advisor->AI()->AttackClosestEnemy();
                }
                break;
            case 8:
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_WEAPONS) == CAST_OK)
                    DoBroadcastText(SAY_PHASE2_WEAPON, m_creature);
                break;
        }

        ++m_uiPhaseSubphase;
        if (timer)
            ResetTimer(KAEL_PHASE_ONE, timer);
    }

    void HandleWeaponAttack()
    {
        for (ObjectGuid& guid : m_weapons)
        {
            if (Creature* weapon = m_creature->GetMap()->GetCreature(guid))
            {
                weapon->AI()->SetReactState(REACT_AGGRESSIVE);
                weapon->SetInCombatWithZone();
                weapon->AI()->AttackClosestEnemy();
            }
        }
    }

    void HandlePhaseTwoEnd()
    {
        DoBroadcastText(SAY_PHASE3_ADVANCE, m_creature);
        m_uiPhase = PHASE_3_ADVISOR_ALL;
        m_uiPhaseSubphase = 0;
#ifdef FAST_TIMERS
        ResetTimer(KAEL_PHASE_THREE, 1000);
#else
        ResetTimer(KAEL_PHASE_THREE, 6000);
#endif
    }

    void HandlePhaseThree()
    {
        uint32 timer = 0;
        switch (m_uiPhaseSubphase)
        {
            case 0:
            {
                // Rezzurect all advisors
                if (DoCastSpellIfCan(m_creature, SPELL_RESURRECTION) == CAST_OK)
                {
                    m_uiPhaseSubphase = 1;
#ifdef FAST_TIMERS
                    timer = 10000;
#elif defined(PRENERF_2_0_3)
                    timer = 120000;
#else
                    timer = 180000;
#endif
                }
                break;
            }
            case 1:
            {
                DoBroadcastText(SAY_PHASE4_INTRO2, m_creature);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                DoResetThreat();
                SetCombatScriptStatus(false);
                SetCombatMovement(true, true);
                m_uiPhase = PHASE_4_SOLO;
                m_uiPhaseSubphase = 0;
                AddInitialCooldowns();
                break;
            }
        }
        if (timer)
            ResetTimer(KAEL_PHASE_THREE, timer);
    }

    void HandlePhaseTransition()
    {
        uint32 timer = 0;
        switch (m_phaseTransitionStage)
        {
            case 0:
            {
                m_creature->CastSpell(nullptr, SPELL_KAEL_GAIN_POWER, TRIGGERED_NONE);
                m_creature->PlayDirectSound(SOUND_PHASE_TRANSITION1);
                m_creature->PlayDirectSound(SOUND_PHASE_TRANSITION2);

                timer = 1000;
                break;
            }
            case 1:
            {
                m_creature->SetLevitate(true);
                m_creature->SetHover(true);
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_AIR, flightPos[0], flightPos[1], flightPos[2], FORCED_MOVEMENT_WALK);
                timer = 16000;
                ResetTimer(KAEL_PHASE_TRANSITION_GROWTH, 500);
                break;
            }
            case 2:
            {
                m_creature->RemoveAurasDueToSpell(SPELL_KAEL_GAIN_POWER);
                m_creature->RemoveAurasDueToSpell(SPELL_NETHERBEAM_GLOW3);
                m_creature->CastSpell(nullptr, SPELL_EXPLODE, TRIGGERED_NONE);
                m_creature->RemoveAurasDueToSpell(SPELL_NETHERBEAM_EXPLODE);
                m_creature->RemoveAurasDueToSpell(SPELL_NETHERBEAM);
                m_instance->DoUseDoorOrButton(GO_KAEL_STATUE_RIGHT);
                m_instance->DoUseDoorOrButton(GO_BRIDGE_WINDOW);
                m_instance->DoUseDoorOrButton(GO_KAEL_STATUE_LEFT);
#ifdef FAST_TRANSITION_TIMERS
                timer = 500;
#else
                timer = 13500;
                ResetTimer(KAEL_PHASE_TRANSITION_NETHER_BEAM, 4000);
#endif
                break;
            }
            case 3:
            {
                m_creature->RemoveAurasDueToSpell(SPELL_KAEL_STUN);
                m_creature->RemoveAurasDueToSpell(SPELL_FLOATING_DROWNED);
                m_creature->SetOrientation(3.228787f);
                m_creature->CastSpell(nullptr, SPELL_KAEL_FULL_POWER, TRIGGERED_NONE);
#ifdef FAST_TRANSITION_TIMERS
                timer = 2000;
#else
                timer = 3500;
#endif
                break;
            }
            case 4:
            {
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_LAND, landPos[0], landPos[1], landPos[2], FORCED_MOVEMENT_WALK);
                break;
            }
        }
        ++m_phaseTransitionStage;
        if (timer)
            ResetTimer(KAEL_PHASE_TRANSITION, timer);
    }

    void HandlePhaseTransitionGrowth()
    {
        static uint32 timers[] = { 3600,400,2000,400,4500,4000,4000,4000,0 };
        switch (m_phaseTransitionGrowthStage)
        {
            case 0:
            {
                if (Creature* trigger = m_creature->GetMap()->GetCreature(m_worldTriggersFirstStage[0]))
                    trigger->CastSpell(nullptr, SPELL_NETHERBEAM_EXPLODE, TRIGGERED_NONE);
                if (Creature* trigger = m_creature->GetMap()->GetCreature(m_worldTriggersFirstStage[1]))
                    trigger->CastSpell(nullptr, SPELL_NETHERBEAM, TRIGGERED_NONE);
                break;
            }
            case 1:
            {
                if (Creature* trigger = m_creature->GetMap()->GetCreature(m_worldTriggersFirstStage[2]))
                    trigger->CastSpell(nullptr, SPELL_NETHERBEAM, TRIGGERED_NONE);
                break;
            }
            case 2:
            {
                if (Creature* trigger = m_creature->GetMap()->GetCreature(m_worldTriggersFirstStage[3]))
                    trigger->CastSpell(nullptr, SPELL_NETHERBEAM_EXPLODE, TRIGGERED_NONE);
                break;
            }
            case 3:
            {
                if (Creature* trigger = m_creature->GetMap()->GetCreature(m_worldTriggersFirstStage[4]))
                    trigger->CastSpell(nullptr, SPELL_NETHERBEAM, TRIGGERED_NONE);
                break;
            }
            case 4:
            {
                if (Creature* trigger = m_creature->GetMap()->GetCreature(m_worldTriggersFirstStage[5]))
                    trigger->CastSpell(nullptr, SPELL_NETHERBEAM_EXPLODE, TRIGGERED_NONE);
                break;
            }
            case 5: // explosions
            case 6:
            case 7:
            case 8:
            {
                uint32 spellId;
                switch (m_phaseTransitionGrowthStage)
                {
                    case 5: spellId = SPELL_EXPLODE_4; break;
                    case 6: spellId = SPELL_EXPLODE_2; break;
                    case 7: spellId = SPELL_EXPLODE_3; break;
                    case 8: spellId = SPELL_EXPLODE_1; break;
                }

                for (ObjectGuid& guid : m_worldTriggersExplosions)
                    if (Creature* trigger = m_creature->GetMap()->GetCreature(guid))
                        trigger->CastSpell(nullptr, spellId, TRIGGERED_NONE);
                break;
            }
        }
        uint32 timer = timers[m_phaseTransitionGrowthStage];
        if (timer)
            ResetTimer(KAEL_PHASE_TRANSITION_GROWTH, timer);
        ++m_phaseTransitionGrowthStage;
    }

    void HandlePhaseTransitionNetherBeam()
    {
        switch (m_phaseTransitionPureNetherBeamStage % 3)
        {
            case 0:
                for (uint32 i = 0; i < TRIGGERS_GROUP_ONE; ++i)
                    if (urand(0, 1)) // 50% chance per trigger
                        if (Creature* trigger = m_creature->GetMap()->GetCreature(m_worldTriggersSecondStage[groupOneIndices[i]]))
                            trigger->CastSpell(nullptr, triggerSpellIds[groupOneIndices[i]], TRIGGERED_NONE);
                break;
            case 1:
                for (uint32 i = 0; i < TRIGGERS_GROUP_TWO; ++i)
                    if (urand(0, 1)) // 50% chance per trigger
                        if (Creature* trigger = m_creature->GetMap()->GetCreature(m_worldTriggersSecondStage[groupTwoIndices[i]]))
                            trigger->CastSpell(nullptr, triggerSpellIds[groupTwoIndices[i]], TRIGGERED_NONE);
                break;
            case 2:
                for (uint32 i = 0; i < TRIGGERS_GROUP_THREE; ++i)
                    if (urand(0, 1)) // 50% chance per trigger
                        if (Creature* trigger = m_creature->GetMap()->GetCreature(m_worldTriggersSecondStage[groupThreeIndices[i]]))
                            trigger->CastSpell(nullptr, triggerSpellIds[groupThreeIndices[i]], TRIGGERED_NONE);
                break;
        }

        ++m_phaseTransitionPureNetherBeamStage;
        if (m_phaseTransitionPureNetherBeamStage < 30) // 30 pure nether beam cycles on retail
            ResetTimer(KAEL_PHASE_TRANSITION_NETHER_BEAM, 400);
    }
};

// 36709 - Kael Phase Two
struct KaelPhaseTwo : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (boss_kaelthasAI* kaelAI = dynamic_cast<boss_kaelthasAI*>(target->AI()))
            kaelAI->AdvisorDefeated(caster->GetEntry());
    }
};

/*######
## advisor_base_ai
######*/

enum AdvisorActions
{
    ADVISOR_ACTION_MAX,
    ADVISOR_START_ATTACK = 30,
};

struct advisor_base_ai : public CombatAI
{
    advisor_base_ai(Creature* creature, uint32 combatActions) : CombatAI(creature, combatActions), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddCustomAction(ADVISOR_START_ATTACK, true, [&]() { HandleStartAttack(); }, TIMER_COMBAT_COMBAT);
    }

    ScriptedInstance* m_instance;

    bool m_resurrected;

    void Reset() override
    {
        CombatAI::Reset();
        m_resurrected = false;

        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

        SetCombatMovement(true);
        SetCombatScriptStatus(false);
        SetDeathPrevention(true);

        m_creature->SetSpellList(m_creature->GetEntry() * 100 + 1);
    }

    void HandleStartAttack()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        SetCombatScriptStatus(false);
        SetMeleeEnabled(true);
        SetCombatMovement(true);
        SetCombatScriptStatus(false);
        SetReactState(REACT_AGGRESSIVE);
        DoResetThreat();
        DoStartMovement(m_creature->GetVictim());

        SetDeathPrevention(false);
        m_resurrected = true;
    }

    virtual void HandleSpellSwitch() = 0;

    void JustReachedHome() override
    {
        // Reset Kael if needed
        if (m_instance)
        {
            if (Creature* kael = m_instance->GetSingleCreatureFromStorage(NPC_KAELTHAS))
                kael->AI()->EnterEvadeMode();

            m_instance->SetData(TYPE_KAELTHAS, FAIL);
        }
    }

    void JustPreventedDeath(Unit* killer) override
    {
        m_creature->InterruptNonMeleeSpells(true);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        m_creature->ClearAllReactives();
        SetMeleeEnabled(false);
        SetCombatMovement(false);
        SetCombatScriptStatus(true);

        m_creature->CastSpell(nullptr, SPELL_PERMANENT_FEIGN_DEATH, TRIGGERED_NONE);

        if (m_instance)
        {
            if (Creature* kael = m_instance->GetSingleCreatureFromStorage(NPC_KAELTHAS))
                if (boss_kaelthasAI* kaelAI = dynamic_cast<boss_kaelthasAI*>(kael->AI()))
                    kaelAI->AdvisorDefeated(m_creature->GetEntry());
        }
    }

    void SpellHit(Unit* caster, const SpellEntry* spellInfo) override
    {
        // Remove fake death
        if (spellInfo->Id == SPELL_RESURRECTION && caster->GetEntry() == NPC_KAELTHAS)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_PERMANENT_FEIGN_DEATH);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            ResetTimer(ADVISOR_START_ATTACK, 2000);
        }
    }

    void JustDied(Unit* killer) override
    {
        if (m_instance)
        {
            if (Creature* kael = m_instance->GetSingleCreatureFromStorage(NPC_KAELTHAS))
                if (boss_kaelthasAI* kaelAI = dynamic_cast<boss_kaelthasAI*>(kael->AI()))
                    kaelAI->AdvisorDied();
        }
    }
};

/*######
## boss_thaladred_the_darkener
######*/

struct boss_thaladred_the_darkenerAI : public advisor_base_ai
{
    boss_thaladred_the_darkenerAI(Creature* creature) : advisor_base_ai(creature, ADVISOR_ACTION_MAX) { Reset(); }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_THALADRED_AGGRO, m_creature);
    }

    void JustDied(Unit* killer) override
    {
        advisor_base_ai::JustDied(killer);
        DoBroadcastText(SAY_THALADRED_DEATH, m_creature);
    }

    void SpellHit(Unit* caster, const SpellEntry* spellInfo) override
    {
        advisor_base_ai::SpellHit(caster, spellInfo);
        if (spellInfo->Id == SPELL_TRIGGER_CREATURE_SPECIAL)
        {
            SelectAttackingTargetParams parameters;
            parameters.range.minRange = 0.f;
            parameters.range.maxRange = 200.f;
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_RANGE_RANGE, parameters))
            {
                // seems to be part of spell list loop
                DoResetThreat();
                m_creature->AddThreat(target, 1000000.f);
                DoBroadcastText(EMOTE_THALADRED_GAZE, m_creature, target);
            }
        }
    }

    void HandleSpellSwitch() override
    {
        m_creature->SetSpellList(SPELL_SET_THALADRED_REZZED);
    }
};

/*######
## boss_lord_sanguinar
######*/

struct boss_lord_sanguinarAI : public advisor_base_ai
{
    boss_lord_sanguinarAI(Creature* creature) : advisor_base_ai(creature, ADVISOR_ACTION_MAX)
    {
    }

    void Reset() override
    {
        advisor_base_ai::Reset();

        DoCastSpellIfCan(nullptr, SPELL_THRASH, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_SANGUINAR_AGGRO, m_creature);
    }

    void JustDied(Unit* killer) override
    {
        advisor_base_ai::JustDied(killer);
        DoBroadcastText(SAY_SANGUINAR_DEATH, m_creature);
    }

    void HandleSpellSwitch() override
    {
        AddInitialCooldowns();
    }
};

/*######
## boss_grand_astromancer_capernian
######*/

struct boss_grand_astromancer_capernianAI : public advisor_base_ai
{
    boss_grand_astromancer_capernianAI(Creature* creature) : advisor_base_ai(creature, ADVISOR_ACTION_MAX)
    {
        SetRangedMode(true, 30.f, TYPE_FULL_CASTER);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_CAPERNIAN_AGGRO, m_creature);
    }

    void JustDied(Unit* killer) override
    {
        advisor_base_ai::JustDied(killer);
        DoBroadcastText(SAY_CAPERNIAN_DEATH, m_creature);
    }

    void HandleSpellSwitch() override
    {
        AddInitialCooldowns();
    }
};

/*######
## boss_master_engineer_telonicus
######*/

struct boss_master_engineer_telonicusAI : public advisor_base_ai
{
    boss_master_engineer_telonicusAI(Creature* creature) : advisor_base_ai(creature, ADVISOR_ACTION_MAX)
    {
        m_creature->SetModifierValue(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_PCT, 1.f); // sniff evidence seems to point out that telonicus isnt subject to 50% offhand dmg modifier
        // TODO: Maybe true for all mobs?
        SetRangedMode(true, 25.f, TYPE_PROXIMITY);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_TELONICUS_AGGRO, m_creature);
    }

    void JustDied(Unit* killer) override
    {
        advisor_base_ai::JustDied(killer);
        DoBroadcastText(SAY_TELONICUS_DEATH, m_creature);
    }

    void HandleSpellSwitch() override
    {
        AddInitialCooldowns();
    }
};

struct npc_nether_vaporAI : public ScriptedAI
{
    npc_nether_vaporAI(Creature* creature) : ScriptedAI(creature)
    {
        SetReactState(REACT_PASSIVE);
        AddCustomAction(0, urand(10000, 50000), [&]()
        {
            if (Unit* spawner = m_creature->GetSpawner())
            {
                float angle = spawner->GetAngle(m_creature);
                float distance = sqrt(m_creature->GetDistance(spawner, true, DIST_CALC_NONE));
                angle += frand(-1.f, 1.f);
                MapManager::NormalizeOrientation(angle);
                distance = frand(0, distance);
                float x, y, z;
                spawner->GetNearPoint(m_creature, x, y, z, 1.f, distance, angle);
                m_creature->GetMotionMaster()->MovePoint(1, x, y, z);
            }
        });
    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        DoCastSpellIfCan(nullptr, SPELL_NETHER_VAPOR_PERIODIC_DAMAGE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_NETHER_VAPOR_PERIODIC_SCRIPT, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_NETHER_VAPOR_LIGHTNING, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }
};

// 45960 - Nether Vapor Lightning
struct NetherVaporLightning : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        Position pos = target->GetPosition();
        pos.x += frand(-10.f, 10.f);
        pos.y += frand(-10.f, 10.f);
        pos.z += frand(-10.f, 10.f);
        aura->GetTarget()->CastSpell(pos.x, pos.y, pos.z, SPELL_ASTRAL_STORM, TRIGGERED_OLD_TRIGGERED);
    }
};

// 35861 - Summon Nether Vapor
struct NetherVaporSummon : public SpellScript
{
    void OnDestTarget(Spell* spell) const override
    {
        spell->m_targets.m_destPos.z += 5.f;
    }
};

// 35865 - Summon Nether Vapor
struct NetherVaporSummonParent : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        spell->GetCaster()->CastSpell(nullptr, 35861, TRIGGERED_NONE);
        spell->GetCaster()->CastSpell(nullptr, 35862, TRIGGERED_NONE);
        spell->GetCaster()->CastSpell(nullptr, 35863, TRIGGERED_NONE);
        spell->GetCaster()->CastSpell(nullptr, 35864, TRIGGERED_NONE);
    }
};

// 39497 - Remove Enchanted Weapons
struct RemoveWeapons : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer())
            return;

        target->CastSpell(nullptr, 39498, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);
        target->CastSpell(nullptr, 39499, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);
        target->CastSpell(nullptr, 39500, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);
        target->CastSpell(nullptr, 39501, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);
        target->CastSpell(nullptr, 39502, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);
        target->CastSpell(nullptr, 39503, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);
        target->CastSpell(nullptr, 39504, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);
    }
};

// 34480, 44226, 49887 - Gravity Lapse
struct GravityLapseKnockup : public AuraScript
{
    void OnPeriodicTickEnd(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target->IsAboveGround(4.f)) // knock up player if he is too close to the ground
            target->CastSpell(nullptr, 35938, TRIGGERED_OLD_TRIGGERED);
    }
};

// 36815 - Shock Barrier
struct ShockBarrier : public AuraScript
{
    void OnAbsorb(Aura* /*aura*/, int32& currentAbsorb, int32& remainingDamage, uint32& reflectedSpellId, int32& reflectDamage, bool& /*preventedDeath*/, bool& /*dropCharge*/, DamageEffectType /*damageType*/) const override
    {
        reflectedSpellId = 36822;
        if (remainingDamage < currentAbsorb)
            reflectDamage = remainingDamage / 100;
        else
            reflectDamage = currentAbsorb / 100;
    }
};

// 36797 - Mind Control
struct MindControlKaelthas : public SpellScript, public AuraScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (spell->GetCaster()->GetVictim() == target)
            return false;
        return true;
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->GetTarget()->CastSpell(nullptr, SPELL_MIND_CONTROL2, TRIGGERED_OLD_TRIGGERED);
        else
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_MIND_CONTROL2);
    }
};

// 35869 - Nether Beam
struct NetherBeam : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        ThreatList const& threatlist = caster->getThreatManager().getThreatList();
        if (threatlist.empty())
            return;

        std::vector<Unit*> suitableUnits;
        caster->SelectAttackingTargets(suitableUnits, ATTACKING_TARGET_ALL_SUITABLE, 0, SPELL_NETHER_BEAM_DAMAGE, SELECT_FLAG_PLAYER);

        if (!suitableUnits.empty())
        {
            while (suitableUnits.size() > 5)
                suitableUnits.erase(suitableUnits.begin() + urand(0, suitableUnits.size() - 1));

            for (Unit* beamTarget : suitableUnits)
                caster->CastSpell(beamTarget, SPELL_NETHER_BEAM_DAMAGE, TRIGGERED_NONE);
        }
    }
};

// 36819 - Pyroblast
struct PyroblastKael : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (spell->GetCaster()->GetVictim() && target != spell->GetCaster()->GetVictim())
            return false;
        return true;
    }
};

// 36730 - Flame Strike
struct FlameStrikeKael : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        aura->GetTarget()->CastSpell(nullptr, 36731, TRIGGERED_OLD_TRIGGERED, nullptr, aura);
        if (aura->GetTarget()->IsCreature())
            static_cast<Creature*>(aura->GetTarget())->ForcedDespawn(10000);
    }
};

// 36089 - Netherbeam, 36090 - Netherbeam
struct NetherbeamSpeedKael : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        float speed = target->GetBaseRunSpeed(); // fetch current base speed
        target->ApplyModPositiveFloatValue(OBJECT_FIELD_SCALE_X, float(aura->GetModifier()->m_amount) / 100, apply);
        target->UpdateModelData(); // resets speed
        target->SetBaseRunSpeed(speed + (1.f / 7.f));
        target->UpdateSpeed(MOVE_RUN, true); // sends speed packet
    }
};

// 36091 - Kael Gaining Power
struct KaelGainingPower : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& /*data*/) const override
    {
        Unit* target = aura->GetTarget();
        switch (aura->GetAuraTicks())
        {
            case 1:
                target->CastSpell(nullptr, SPELL_NETHERBEAM_GLOW1, TRIGGERED_OLD_TRIGGERED);
                target->PlayDirectSound(27);
                target->PlayDirectSound(1136);
                break;
            case 2:
                target->RemoveAurasDueToSpell(SPELL_NETHERBEAM_GLOW1);
                target->CastSpell(nullptr, SPELL_NETHERBEAM_GLOW2, TRIGGERED_OLD_TRIGGERED);
                target->PlayDirectSound(27);
                target->PlayDirectSound(1136);
                break;
            case 3:
                target->RemoveAurasDueToSpell(SPELL_NETHERBEAM_GLOW2);
                target->CastSpell(nullptr, SPELL_NETHERBEAM_GLOW3, TRIGGERED_OLD_TRIGGERED);
                target->PlayDirectSound(27);
                target->PlayDirectSound(1136);
                break;
            case 4:
                if (target->GetTypeId() == TYPEID_UNIT && target->AI())
                    target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, static_cast<Creature*>(target));
                break;
        }
    }
};

// 36092 - Kael Explodes
struct KaelExplodes : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        caster->CastSpell(nullptr, 36185, TRIGGERED_NONE);
        caster->CastSpell(nullptr, 36550, TRIGGERED_NONE);
        caster->PlayDirectSound(3320);
        caster->PlayDirectSound(10845);
        caster->PlayDirectSound(6539);
    }
};

// 36196, 36197, 36198 - Pure Nether Beam
struct PureNetherBeam : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (!spell->GetUnitTarget())
            return;

        spell->GetUnitTarget()->PlaySpellVisual(7234);
        spell->GetUnitTarget()->PlaySpellVisual(7235);
    }
};

// 36201, 36290, 36291 - Pure Nether Beam
struct PureNetherBeamParent : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (!spell->GetUnitTarget())
            return;

        uint32 spellId;
        switch (urand(0, 2))
        {
            case 0: spellId = 36196; break;
            case 1: spellId = 36197; break;
            case 2: spellId = 36198; break;
        }

        spell->GetCaster()->PlayDirectSound(6477);
        spell->GetCaster()->PlayDirectSound(44);

        spell->GetUnitTarget()->CastSpell(nullptr, spellId, TRIGGERED_NONE); // also triggered in sniff, only SMSG_SPELL_GO
    }
};

// 37027 - Remote Toy
struct RemoteToy : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        if (urand(0, 4) == 0) // 20% chance to apply trigger spell
            data.spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(37029); // Remote Toy
    }
};

void AddSC_boss_kaelthas()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_kaelthas";
    pNewScript->GetAI = &GetNewAIInstance<boss_kaelthasAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_thaladred_the_darkener";
    pNewScript->GetAI = &GetNewAIInstance<boss_thaladred_the_darkenerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_lord_sanguinar";
    pNewScript->GetAI = &GetNewAIInstance<boss_lord_sanguinarAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_grand_astromancer_capernian";
    pNewScript->GetAI = &GetNewAIInstance<boss_grand_astromancer_capernianAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_master_engineer_telonicus";
    pNewScript->GetAI = &GetNewAIInstance<boss_master_engineer_telonicusAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_nether_vapor";
    pNewScript->GetAI = &GetNewAIInstance<npc_nether_vaporAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<KaelPhaseTwo>("spell_kael_phase_two");
    RegisterSpellScript<NetherVaporLightning>("spell_nether_vapor_lightning");
    RegisterSpellScript<NetherVaporSummon>("spell_nether_vapor_summon");
    RegisterSpellScript<NetherVaporSummonParent>("spell_nether_vapor_summon_parent");
    RegisterSpellScript<RemoveWeapons>("spell_remove_weapons");
    RegisterSpellScript<GravityLapseKnockup>("spell_gravity_lapse_knockup");
    RegisterSpellScript<ShockBarrier>("spell_shock_barrier");
    RegisterSpellScript<MindControlKaelthas>("spell_mind_control_kaelthas");
    RegisterSpellScript<NetherBeam>("spell_nether_beam");
    RegisterSpellScript<PyroblastKael>("spell_pyroblast_kael");
    RegisterSpellScript<FlameStrikeKael>("spell_flame_strike_kael");
    RegisterSpellScript<NetherbeamSpeedKael>("spell_netherbeam_speed_kael");
    RegisterSpellScript<KaelGainingPower>("spell_kael_gaining_power");
    RegisterSpellScript<KaelExplodes>("spell_kael_explodes");
    RegisterSpellScript<PureNetherBeam>("spell_pure_nether_beam");
    RegisterSpellScript<PureNetherBeamParent>("spell_pure_nether_beam_parent");
    RegisterSpellScript<RemoteToy>("spell_remote_toy_trigger");
}

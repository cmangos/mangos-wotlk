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
//#define NO_PHOENIX

enum
{
    // ***** Event yells ********
    // kael'thas Speech
    SAY_INTRO                           = -1550016,
    SAY_INTRO_CAPERNIAN                 = -1550017,
    SAY_INTRO_TELONICUS                 = -1550018,
    SAY_INTRO_THALADRED                 = -1550019,
    SAY_INTRO_SANGUINAR                 = -1550020,
    SAY_PHASE2_WEAPON                   = -1550021,
    SAY_PHASE3_ADVANCE                  = -1550022,
    SAY_PHASE4_INTRO2                   = -1550023,
    SAY_PHASE5_NUTS                     = -1550024,
    SAY_SLAY1                           = -1550025,
    SAY_SLAY2                           = -1550026,
    SAY_SLAY3                           = -1550027,
    SAY_MINDCONTROL1                    = -1550028,
    SAY_MINDCONTROL2                    = -1550029,
    SAY_GRAVITYLAPSE1                   = -1550030,
    SAY_GRAVITYLAPSE2                   = -1550031,
    SAY_SUMMON_PHOENIX1                 = -1550032,
    SAY_SUMMON_PHOENIX2                 = -1550033,
    SAY_DEATH                           = -1550034,
    EMOTE_PYROBLAST                     = -1550044,

    // Thaladred the Darkener speech
    SAY_THALADRED_AGGRO                 = -1550035,
    SAY_THALADRED_DEATH                 = -1550036,
    EMOTE_THALADRED_GAZE                = -1550037,

    // Lord Sanguinar speech
    SAY_SANGUINAR_AGGRO                 = -1550038,
    SAY_SANGUINAR_DEATH                 = -1550039,

    // Grand Astromancer Capernian speech
    SAY_CAPERNIAN_AGGRO                 = -1550040,
    SAY_CAPERNIAN_DEATH                 = -1550041,

    // Master Engineer Telonicus speech
    SAY_TELONICUS_AGGRO                 = -1550042,
    SAY_TELONICUS_DEATH                 = -1550043,

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
    SPELL_SHOOT                         = 16496,        // Only used when at max range, 25-30yd

    // ***** Other spells ********
    // Utilities
    SPELL_PERMANENT_FEIGN_DEATH         = 29266,        // placed upon advisors on fake death
    SPELL_BANISH                        = 40370,        // not used, should despawn phoenixes when gravity lapse executes

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
    PHASE_5_GRAVITY                     = 5,
    PHASE_TRANSITION                    = 6,

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
};

static const uint32 m_auiSpellSummonWeapon[MAX_WEAPONS] =
{
    SPELL_SUMMON_WEAPONA, SPELL_SUMMON_WEAPONB, SPELL_SUMMON_WEAPONC, SPELL_SUMMON_WEAPOND,
    SPELL_SUMMON_WEAPONE, SPELL_SUMMON_WEAPONF, SPELL_SUMMON_WEAPONG
};

// teleport spells for gravity lapse event
static const uint32 m_auiSpellGravityLapseTeleport[] =
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
    KAEL_ACTION_NETHER_VAPOR,
    KAEL_ACTION_NETHER_BEAM,
    KAEL_ACTION_PHASE_5,
    KAEL_ACTION_PHOENIX,
    KAEL_ACTION_FLAMESTRIKE,
    KAEL_ACTION_ARCANE_DISRUPTION,
    KAEL_ACTION_SHOCK_BARRIER,
    KAEL_ACTION_GRAVITY_LAPSE,
    KAEL_ACTION_MIND_CONTROL,
    KAEL_ACTION_PYROBLAST_INIT,
    KAEL_ACTION_MELEE_MODE,
    KAEL_ACTION_FIREBALL,
    KAEL_ACTION_MAX,
};

struct boss_kaelthasAI : public ScriptedAI
{
    boss_kaelthasAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        Reset();
    }

    ScriptedInstance* m_instance;

    uint32 m_uiFireballTimer;
    uint32 m_uiArcaneDisruptionTimer;
    uint32 m_uiPhoenixTimer;
    uint32 m_uiFlameStrikeTimer;

    uint32 m_pyroblastCounter;
    uint32 m_uiShockBarrierTimer;
    uint32 m_uiMindControlTimer;
    uint32 m_uiExplodeTimer;

    uint32 m_uiGravityLapseTimer;
    uint32 m_uiGravityExpireTimer;
    uint32 m_uiNetherBeamTimer;
    uint32 m_uiNetherVaporTimer;
    uint8 m_uiGravityIndex;

    uint32 m_uiPhaseTimer;
    uint8 m_uiPhase;
    uint8 m_uiPhaseSubphase;

    uint32 m_phaseTransitionTimer;
    uint32 m_phaseTransitionStage;

    uint32 m_phaseTransitionGrowthTimer;
    uint32 m_phaseTransitionGrowthStage;
    uint32 m_phaseTransitionPureNetherBeamTimer;
    uint32 m_phaseTransitionPureNetherBeamStage;

    GuidVector m_worldTriggersFirstStage;
    GuidVector m_worldTriggersExplosions;
    GuidVector m_worldTriggersSecondStage;

    GuidVector m_netherVapor;
    GuidList m_lSummonedGuidList;
    GuidVector m_weapons;
    uint32 m_weaponAttackTimer;

    GuidSet m_charmTargets;

    bool m_actionReadyStatus[KAEL_ACTION_MAX];

    bool m_rangeMode;

    void Reset() override
    {
        // Phases
        m_uiPhase                   = PHASE_0_NOT_BEGUN;
#ifdef FAST_TIMERS
        m_uiPhaseSubphase = 0;
        m_uiPhaseTimer = 5000;
#else
        m_uiPhaseSubphase = 0;
        m_uiPhaseTimer = 23000;
#endif

        // Spells
        m_uiFireballTimer           = 0;
        m_uiArcaneDisruptionTimer   = 45000;
        m_uiPhoenixTimer            = 50000;
        m_uiFlameStrikeTimer        = 30000;

        m_uiShockBarrierTimer       = 60000;
        m_uiMindControlTimer        = 40000;
        m_uiExplodeTimer            = 0;

        m_uiGravityLapseTimer       = 12000;
        m_uiGravityExpireTimer      = 0;
        m_uiNetherBeamTimer         = 8000;
        m_uiNetherVaporTimer        = 10000;
        m_uiGravityIndex            = 0;

        m_phaseTransitionGrowthTimer = 0;
        m_phaseTransitionGrowthStage = 0;

        m_phaseTransitionPureNetherBeamStage = 0;
        m_phaseTransitionPureNetherBeamTimer = 0;

        m_phaseTransitionTimer = 0;
        m_phaseTransitionStage = 0;

        m_attackDistance = 30.0f;
        m_attackAngle = 0.0f;

        SetCombatMovement(true);
        DoDespawnSummons();

        m_rangeMode = true;
        SetMeleeEnabled(true);

        for (uint32 i = 0; i < KAEL_ACTION_MAX; ++i)
            m_actionReadyStatus[i] = false;

        m_actionReadyStatus[KAEL_ACTION_PHASE_5] = true;
        m_actionReadyStatus[KAEL_ACTION_FIREBALL] = true;
        m_actionReadyStatus[KAEL_ACTION_MELEE_MODE] = true;

        ResetSize();

        SetCombatScriptStatus(false);

        m_charmTargets.clear();
    }

    void ResetSize()
    {
        // reset size and speed from phase 5
        m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.f);
        m_creature->UpdateModelData(); // also resets speed
        m_creature->UpdateSpeed(MOVE_RUN, true);
    }

    void JustRespawned()
    {
        ScriptedAI::JustRespawned();
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
        for (GuidList::const_iterator itr = m_lSummonedGuidList.begin(); itr != m_lSummonedGuidList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
            {
                if (pTemp->GetEntry() == NPC_PHOENIX)
                    m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pTemp);
                pTemp->ForcedDespawn();
            }
        }

        m_netherVapor.clear();
        m_lSummonedGuidList.clear();
    }

    // Custom Move in LoS function
    void MoveInLineOfSight(Unit* who) override
    {
        if (m_uiPhase == PHASE_0_NOT_BEGUN && who->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED) &&
                m_creature->IsWithinDistInMap(who, m_creature->GetAttackDistance(who)) && m_creature->IsWithinLOSInMap(who) && m_creature->CanAttackOnSight(who))
        {
            if (who->IsPlayer() && static_cast<Player*>(who)->IsGameMaster())
                return;
            DoScriptText(SAY_INTRO, m_creature);
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

    void KilledUnit(Unit* /*unit*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        m_creature->CastSpell(nullptr, SPELL_REMOVE_WEAPONS, TRIGGERED_OLD_TRIGGERED);

        if (m_instance)
            m_instance->SetData(TYPE_KAELTHAS, DONE);

        DoDespawnSummons();
        ResetSize();
    }

    void JustReachedHome() override
    {
        m_creature->CastSpell(nullptr, SPELL_REMOVE_WEAPONS, TRIGGERED_OLD_TRIGGERED);

        if (m_instance)
            m_instance->SetData(TYPE_KAELTHAS, FAIL);
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_FLAME_STRIKE_TRIGGER:
            {
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
                m_weaponAttackTimer = 2000;
                summoned->AI()->SetReactState(REACT_PASSIVE);
                summoned->SetCorpseDelay(60);
                break;
            }
        }
            
        m_lSummonedGuidList.push_back(summoned->GetObjectGuid());
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spellInfo) override
    {
        // Handle summon weapons event
        if (spellInfo->Id == SPELL_SUMMON_WEAPONS)
        {
            for (unsigned int i : m_auiSpellSummonWeapon)
                DoCastSpellIfCan(m_creature, i, CAST_TRIGGERED);

            m_uiPhase      = PHASE_2_WEAPON;
#ifdef FAST_TIMERS
            m_uiPhaseTimer = 10000;
#else
            m_uiPhaseTimer = 120000;
#endif
#ifdef PRENERF_2_0_3
            m_uiPhaseTimer = 90000; // very early pre 2.1 - 90s (90000), later 120s (120000);
#endif
        }
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spellInfo) override
    {
        // Handle gravity lapse teleport - each player hit has his own teleport spell
        if (spellInfo->Id == SPELL_GRAVITY_LAPSE && target->GetTypeId() == TYPEID_PLAYER)
        {
            DoCastSpellIfCan(target, m_auiSpellGravityLapseTeleport[m_uiGravityIndex], CAST_TRIGGERED);
            target->CastSpell(target, SPELL_GRAVITY_LAPSE_KNOCKBACK, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(target, SPELL_GRAVITY_LAPSE_AURA, TRIGGERED_OLD_TRIGGERED);
            ++m_uiGravityIndex;
        }

        if (spellInfo->Id == SPELL_MIND_CONTROL)
            m_charmTargets.insert(target->GetObjectGuid());
    }

    void OnSpellInterrupt(SpellEntry const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_PYROBLAST)
            m_actionReadyStatus[KAEL_ACTION_PYROBLAST_SEQUENCE] = false;
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
                DoScriptText(SAY_PHASE5_NUTS, m_creature);
                m_creature->SetFacingTo(3.176499f);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                m_phaseTransitionTimer = 2000;

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
                m_uiPhase = PHASE_5_GRAVITY;
                SetCombatMovement(true);
                m_uiArcaneDisruptionTimer = 70000;
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
            m_uiPhaseTimer = 3000;
        else
            m_uiPhaseTimer = 1000;
    }

    void ExecuteActions()
    {
        // cant use generic condition - TODO: Rewrite gravity lapse into own phase
        if (!m_unit->CanReactInCombat() || m_unit->hasUnitState(UNIT_STAT_PROPELLED | UNIT_STAT_RETREATING | UNIT_STAT_CHANNELING) || m_unit->IsNonMeleeSpellCasted(false))
            return;

        for (uint32 i = 0; i < KAEL_ACTION_MAX; ++i)
        {
            if (m_actionReadyStatus[i])
            {
                if (GetCombatScriptStatus())
                {
                    switch (i)
                    {
                        case KAEL_ACTION_NETHER_BEAM:
                        case KAEL_ACTION_SHOCK_BARRIER:
                        case KAEL_ACTION_NETHER_VAPOR:
                            break;
                        default: continue;
                    }
                }

                switch (i)
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

                            m_uiPhase = PHASE_TRANSITION;
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                    case KAEL_ACTION_PHOENIX:
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_PHOENIX_ANIMATION) == CAST_OK)
                        {
                            DoScriptText(urand(0, 1) ? SAY_SUMMON_PHOENIX1 : SAY_SUMMON_PHOENIX2, m_creature);
                            m_uiPhoenixTimer = 60000;
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                    case KAEL_ACTION_FLAMESTRIKE:
                    {
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(target, SPELL_FLAME_STRIKE) == CAST_OK)
                            {
                                m_uiFlameStrikeTimer = 30000;
                                m_actionReadyStatus[i] = false;
                                return;
                            }
                        }
                        break;
                    }
                    case KAEL_ACTION_ARCANE_DISRUPTION:
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_ARCANE_DISRUPTION) == CAST_OK)
                        {
                            m_uiArcaneDisruptionTimer = 60000;
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                    case KAEL_ACTION_SHOCK_BARRIER:
                    {
                        if (m_uiPhase != PHASE_5_GRAVITY)
                            break;
                        if (DoCastSpellIfCan(m_creature, SPELL_SHOCK_BARRIER, CAST_AURA_NOT_PRESENT) == CAST_OK)
                        {
                            m_uiShockBarrierTimer = 10000;
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                    case KAEL_ACTION_MIND_CONTROL:
                    {
                        if (m_uiPhase != PHASE_4_SOLO)
                            break;

                        if (m_creature->getThreatManager().getThreatList().size() == 1)
                            break;

                        DoCastSpellIfCan(nullptr, SPELL_MIND_CONTROL);
                        DoScriptText(urand(0, 1) ? SAY_MINDCONTROL1 : SAY_MINDCONTROL2, m_creature);
                        m_uiMindControlTimer = 30000;
                        m_actionReadyStatus[i] = false;
                        return;
                    }
                    case KAEL_ACTION_PYROBLAST_INIT:
                    {
                        if (m_uiPhase != PHASE_4_SOLO)
                            break;
                        if (DoCastSpellIfCan(m_creature, SPELL_SHOCK_BARRIER) == CAST_OK)
                        {
                            m_pyroblastCounter = 0;
                            m_uiShockBarrierTimer = 60000;
                            m_actionReadyStatus[i] = false;
                            m_actionReadyStatus[KAEL_ACTION_PYROBLAST_SEQUENCE] = true;
                            return;
                        }
                        break;
                    }
                    case KAEL_ACTION_PYROBLAST_SEQUENCE:
                    {
                        if (DoCastSpellIfCan(nullptr, SPELL_PYROBLAST) == CAST_OK)
                        {
                            DoScriptText(EMOTE_PYROBLAST, m_creature);
                            if (m_pyroblastCounter < 2)
                                m_pyroblastCounter++;
                            else
                                m_actionReadyStatus[KAEL_ACTION_PYROBLAST_SEQUENCE] = false;
                            return;
                        }
                        break;
                    }
                    case KAEL_ACTION_GRAVITY_LAPSE:
                    {
                        if (m_uiPhase != PHASE_5_GRAVITY)
                            break;
                        DoCastSpellIfCan(m_creature, SPELL_SHOCK_BARRIER);
                        if (DoCastSpellIfCan(m_creature, SPELL_GRAVITY_LAPSE) == CAST_OK)
                        {
                            DoScriptText(urand(0, 1) ? SAY_GRAVITYLAPSE1 : SAY_GRAVITYLAPSE2, m_creature);
                            m_creature->CastSpell(m_creature, SPELL_BANISH, TRIGGERED_OLD_TRIGGERED);
                            SetMeleeEnabled(false);
                            m_creature->SetTarget(nullptr);
                            SetCombatScriptStatus(true);
                            SetCombatMovement(false);
                            m_uiGravityIndex = 0;
                            m_uiNetherBeamTimer = 8000;
                            m_uiNetherVaporTimer = 5000;
                            m_uiGravityExpireTimer = 34000;
                            m_uiGravityLapseTimer = 90000;
                            m_uiShockBarrierTimer = 5000;
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                    case KAEL_ACTION_NETHER_BEAM:
                    {
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(m_creature, SPELL_NETHER_BEAM) == CAST_OK)
                            {
                                m_uiNetherBeamTimer = urand(2000, 4000);
                                m_actionReadyStatus[i] = false;
                            }
                        }
                        break;
                    }
                    case KAEL_ACTION_NETHER_VAPOR:
                    {
                        if (m_uiPhase != PHASE_5_GRAVITY)
                            break;
                        if (DoCastSpellIfCan(m_creature, SPELL_NETHER_VAPOR_SUMMON) == CAST_OK)
                        {
                            m_uiNetherVaporTimer = 0;
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                    case KAEL_ACTION_MELEE_MODE:
                    {
                        if (m_rangeMode && m_creature->IsWithinCombatDist(m_creature->GetVictim(), 8.f))
                        {
                            m_rangeMode = false;
                            m_attackDistance = 0.0f;
                            //m_creature->SetSheath(SHEATH_STATE_MELEE);
                            m_creature->MeleeAttackStart(m_creature->GetVictim());
                            DoStartMovement(m_creature->GetVictim());
                        }
                        break;
                    }
                    case KAEL_ACTION_FIREBALL: // TODO: make fireball have CD in melee mode
                    {
                        if (m_rangeMode && !m_creature->IsSpellReady(SPELL_FIREBALL))
                        {
                            m_attackDistance = 0.0f;                            
                            m_rangeMode = false;
                            DoStartMovement(m_creature->GetVictim());
                            return;
                        }
                        else if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FIREBALL) == CAST_OK)
                        {
                            m_uiFireballTimer = m_rangeMode ? 0 : urand(3000, 10000);
                            if (m_uiFireballTimer > 0)
                                m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    }
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        switch (m_uiPhase)
        {
            // ***** Advisors phase ********
            case PHASE_1_ADVISOR:
            {
                if (!m_uiPhaseTimer)
                    return;

                if (m_uiPhaseTimer <= uiDiff)
                {
                    if (!m_instance)
                        return;

                    switch (m_uiPhaseSubphase)
                    {
                        case 0:
                            DoScriptText(SAY_INTRO_THALADRED, m_creature);
#ifdef FAST_TIMERS
                            m_uiPhaseTimer = 1000;
#else
                            m_uiPhaseTimer = 7000;
#endif
                            break;
                        case 1:
                            if (Creature* advisor = m_instance->GetSingleCreatureFromStorage(NPC_THALADRED))
                            {
                                advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                                advisor->SetInCombatWithZone();
                                advisor->AI()->AttackClosestEnemy();
                            }
                            m_uiPhaseTimer = 0;
                            break;
                        case 2:
                            DoScriptText(SAY_INTRO_SANGUINAR, m_creature);
#ifdef FAST_TIMERS
                            m_uiPhaseTimer = 1000;
#else
                            m_uiPhaseTimer = 12500;
#endif
                            break;
                        case 3:
                            if (Creature* advisor = m_instance->GetSingleCreatureFromStorage(NPC_SANGUINAR))
                            {
                                advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                                advisor->SetInCombatWithZone();
                                advisor->AI()->AttackClosestEnemy();
                            }
                            m_uiPhaseTimer = 0;
                            break;
                        case 4:
                            DoScriptText(SAY_INTRO_CAPERNIAN, m_creature);
#ifdef FAST_TIMERS
                            m_uiPhaseTimer = 1000;
#else
                            m_uiPhaseTimer = 7000;
#endif
                            break;
                        case 5:
                            if (Creature* advisor = m_instance->GetSingleCreatureFromStorage(NPC_CAPERNIAN))
                            {
                                advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                                advisor->SetInCombatWithZone();
                                advisor->AI()->AttackClosestEnemy();
                            }
                            m_uiPhaseTimer = 0;
                            break;
                        case 6:
                            DoScriptText(SAY_INTRO_TELONICUS, m_creature);
#ifdef FAST_TIMERS
                            m_uiPhaseTimer = 1000;
#else
                            m_uiPhaseTimer = 8400;
#endif
                            break;
                        case 7:
                            if (Creature* advisor = m_instance->GetSingleCreatureFromStorage(NPC_TELONICUS))
                            {
                                advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                                advisor->SetInCombatWithZone();
                                advisor->AI()->AttackClosestEnemy();
                            }
                            m_uiPhaseTimer = 0;
                            break;
                        case 8:
                            if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_WEAPONS) == CAST_OK)
                                DoScriptText(SAY_PHASE2_WEAPON, m_creature);
                            m_uiPhaseTimer = 0;
                            break;
                    }

                    ++m_uiPhaseSubphase;
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                break;
            }

            // ***** Weapons phase ********
            case PHASE_2_WEAPON:
            {
                if (m_weaponAttackTimer) // attack delay for weapons
                {
                    if (m_weaponAttackTimer <= uiDiff)
                    {
                        m_weaponAttackTimer = 0;
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
                    else
                        m_weaponAttackTimer -= uiDiff;
                }

                if (m_uiPhaseTimer <= uiDiff) // Start text and set delay for rezzurect
                {
                    DoScriptText(SAY_PHASE3_ADVANCE, m_creature);
                    m_uiPhase = PHASE_3_ADVISOR_ALL;
                    m_uiPhaseSubphase = 0;
#ifdef FAST_TIMERS
                    m_uiPhaseTimer = 1000;
#else
                    m_uiPhaseTimer = 6000;
#endif
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                break;
            }

            // ***** All advisors phase ********
            case PHASE_3_ADVISOR_ALL:
            {
                if (m_uiPhaseTimer <= uiDiff)
                {
                    switch (m_uiPhaseSubphase)
                    {
                        case 0:
                        {
                            // Rezzurect all advisors
                            if (DoCastSpellIfCan(m_creature, SPELL_RESURRECTION) == CAST_OK)
                            {
                                m_uiPhaseSubphase = 1;
#ifdef FAST_TIMERS
                                m_uiPhaseTimer = 10000;
#else
                                m_uiPhaseTimer = 180000;
#endif
#ifdef PRENERF_2_0_3
                                m_uiPhaseTimer = 120000;
#endif
                            }
                            break;
                        }
                        case 1:
                        {
                            DoScriptText(SAY_PHASE4_INTRO2, m_creature);
                            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                            DoResetThreat();
                            SetCombatScriptStatus(false);
                            SetCombatMovement(true, true);
                            m_uiPhase = PHASE_4_SOLO;
                            m_uiPhaseTimer = 30000;
                            m_uiPhaseSubphase = 0;
                            break;
                        }
                    }
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                break;
            }

            // ***** Solo phases ********
            case PHASE_4_SOLO:
            case PHASE_5_GRAVITY:
            {
                if (m_uiGravityExpireTimer)
                {
                    // Switch to the other spells after gravity lapse expired
                    if (m_uiGravityExpireTimer <= uiDiff)
                    {
                        SetCombatScriptStatus(false);
                        SetCombatMovement(true);
                        SetMeleeEnabled(true);
                        m_uiGravityExpireTimer = 0;
                        for (ObjectGuid guid : m_netherVapor)
                            if (Creature* vapor = m_creature->GetMap()->GetCreature(guid))
                                vapor->ForcedDespawn();
                        m_netherVapor.clear();

                        // make sure these dont occur in the rest of the phase
                        m_actionReadyStatus[KAEL_ACTION_SHOCK_BARRIER] = false;
                        m_actionReadyStatus[KAEL_ACTION_NETHER_BEAM] = false;
                    }
                    else
                        m_uiGravityExpireTimer -= uiDiff;
                }

                if (!m_creature->SelectHostileTarget())
                    return;

                if (m_uiGravityExpireTimer)
                {
                    if (m_uiNetherBeamTimer <= uiDiff)
                    {
                        m_uiNetherBeamTimer = 0;
                        m_actionReadyStatus[KAEL_ACTION_NETHER_BEAM] = true;
                    }
                    else
                        m_uiNetherBeamTimer -= uiDiff;

                    if (!m_actionReadyStatus[KAEL_ACTION_SHOCK_BARRIER])
                    {
                        if (m_uiShockBarrierTimer <= uiDiff)
                        {
                            m_uiShockBarrierTimer = 0;
                            m_actionReadyStatus[KAEL_ACTION_SHOCK_BARRIER] = true;
                        }
                        else
                            m_uiShockBarrierTimer -= uiDiff;
                    }

                    if (!m_actionReadyStatus[KAEL_ACTION_NETHER_VAPOR])
                    {
                        if (m_uiNetherVaporTimer)
                        {
                            if (m_uiNetherVaporTimer <= uiDiff)
                            {
                                m_uiNetherVaporTimer = 0;
                                m_actionReadyStatus[KAEL_ACTION_NETHER_VAPOR] = true;
                            }
                            else
                                m_uiNetherVaporTimer -= uiDiff;
                        }
                    }
                }

                // ***** Phase 4 specific actions ********
                if (m_uiPhase == PHASE_4_SOLO)
                {
                    if (!m_actionReadyStatus[KAEL_ACTION_PYROBLAST_INIT])
                    {
                        if (m_uiShockBarrierTimer <= uiDiff)
                        {
                            m_uiShockBarrierTimer = 0;
                            m_actionReadyStatus[KAEL_ACTION_PYROBLAST_INIT] = true;
                        }
                        else
                            m_uiShockBarrierTimer -= uiDiff;
                    }

                    if (!m_actionReadyStatus[KAEL_ACTION_MIND_CONTROL])
                    {
                        if (m_uiMindControlTimer <= uiDiff)
                        {
                            m_uiMindControlTimer = 0;
                            m_actionReadyStatus[KAEL_ACTION_MIND_CONTROL] = true;
                        }
                        else
                            m_uiMindControlTimer -= uiDiff;
                    }
                }
                // ***** Phase 5 specific actions ********
                else if (m_uiPhase == PHASE_5_GRAVITY)
                {
                    if (!m_actionReadyStatus[KAEL_ACTION_GRAVITY_LAPSE])
                    {
                        if (m_uiGravityLapseTimer <= uiDiff)
                        {
                            m_uiGravityLapseTimer = 0;
                            m_actionReadyStatus[KAEL_ACTION_GRAVITY_LAPSE] = true;
                        }
                        else
                            m_uiGravityLapseTimer -= uiDiff;
                    }
                }

                if (!m_actionReadyStatus[KAEL_ACTION_ARCANE_DISRUPTION])
                {
                    if (m_uiArcaneDisruptionTimer <= uiDiff)
                    {
                        m_uiArcaneDisruptionTimer = 0;
                        m_actionReadyStatus[KAEL_ACTION_ARCANE_DISRUPTION] = true;
                    }
                    else
                        m_uiArcaneDisruptionTimer -= uiDiff;
                }


                if (!m_actionReadyStatus[KAEL_ACTION_FLAMESTRIKE])
                {
                    if (m_uiFlameStrikeTimer <= uiDiff)
                    {
                        m_uiFlameStrikeTimer = 0;
                        m_actionReadyStatus[KAEL_ACTION_FLAMESTRIKE] = true;
                    }
                    else
                        m_uiFlameStrikeTimer -= uiDiff;
                }

#ifndef NO_PHOENIX
                if (!m_actionReadyStatus[KAEL_ACTION_PHOENIX])
                {
                    if (m_uiPhoenixTimer <= uiDiff)
                    {
                        m_uiPhoenixTimer = 0;
                        m_actionReadyStatus[KAEL_ACTION_PHOENIX] = true;
                    }
                    else
                        m_uiPhoenixTimer -= uiDiff;
                }
#endif

                if (!m_actionReadyStatus[KAEL_ACTION_FIREBALL])
                {
                    if (m_uiFireballTimer <= uiDiff)
                    {
                        m_uiFireballTimer = 0;
                        m_actionReadyStatus[KAEL_ACTION_FIREBALL] = true;
                    }
                    else
                        m_uiFireballTimer -= uiDiff;
                }

                ExecuteActions();

                DoMeleeAttackIfReady();      
                break;
            }
            // ***** Phase transition ********
            case PHASE_TRANSITION:
            {
                // Handle phase 4 to 5 transition
                if (m_phaseTransitionTimer)
                {
                    if (m_phaseTransitionTimer <= uiDiff)
                    {
                        switch (m_phaseTransitionStage)
                        {
                            case 0:
                            {
                                m_creature->CastSpell(nullptr, SPELL_KAEL_GAIN_POWER, TRIGGERED_NONE);
                                m_creature->PlayDirectSound(SOUND_PHASE_TRANSITION1);
                                m_creature->PlayDirectSound(SOUND_PHASE_TRANSITION2);
                                m_uiPhaseTimer = 0;

                                m_phaseTransitionTimer = 1000;
                                m_phaseTransitionStage = 1;
                                break;
                            }
                            case 1:
                            {
                                m_creature->SetLevitate(true);
                                m_creature->SetHover(true);
                                m_creature->GetMotionMaster()->MovePoint(POINT_ID_AIR, flightPos[0], flightPos[1], flightPos[2], FORCED_MOVEMENT_WALK);
                                m_phaseTransitionTimer = 0;
                                m_phaseTransitionStage = 2;
                                m_phaseTransitionTimer = 16000;
                                m_phaseTransitionGrowthTimer = 500;
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
                                m_phaseTransitionTimer = 500;
#else
                                m_phaseTransitionTimer = 13500;
                                m_phaseTransitionPureNetherBeamTimer = 4000; // start pure nether beams
#endif
                                m_phaseTransitionStage = 3;
                                break;
                            }
                            case 3:
                            {
                                m_creature->RemoveAurasDueToSpell(SPELL_KAEL_STUN);
                                m_creature->RemoveAurasDueToSpell(SPELL_FLOATING_DROWNED);
                                m_creature->SetOrientation(3.228787f);
                                m_creature->CastSpell(nullptr, SPELL_KAEL_FULL_POWER, TRIGGERED_NONE);
#ifdef FAST_TRANSITION_TIMERS
                                m_phaseTransitionTimer = 2000;
#else
                                m_phaseTransitionTimer = 3500;
#endif
                                m_phaseTransitionStage = 4;
                                break;
                            }
                            case 4:
                            {
                                m_creature->GetMotionMaster()->MovePoint(POINT_ID_LAND, landPos[0], landPos[1], landPos[2], FORCED_MOVEMENT_WALK);
                                m_phaseTransitionTimer = 0;
                                break;
                            }
                        }
                    }
                    else
                        m_phaseTransitionTimer -= uiDiff;
                }

                if (m_phaseTransitionGrowthTimer)
                {
                    if (m_phaseTransitionGrowthTimer <= uiDiff)
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
                        m_phaseTransitionGrowthTimer = timers[m_phaseTransitionGrowthStage];
                        ++m_phaseTransitionGrowthStage;
                    }
                    else m_phaseTransitionGrowthTimer -= uiDiff;
                }

                if (m_phaseTransitionPureNetherBeamTimer)
                {
                    if (m_phaseTransitionPureNetherBeamTimer <= uiDiff)
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
                            m_phaseTransitionPureNetherBeamTimer = std::max(0, 400 - int32(uiDiff)); // need to roll over timer to achieve blizzlike behaviour
                        else
                            m_phaseTransitionPureNetherBeamTimer = 0;
                    }
                    else m_phaseTransitionPureNetherBeamTimer -= uiDiff;
                }
                break;
            }
        }
    }
};

bool EffectDummyCreature_kael_phase_2(Unit* caster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* creatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_KAEL_PHASE_2 && uiEffIndex == EFFECT_INDEX_0)
    {
        if (boss_kaelthasAI* pKaelAI = dynamic_cast<boss_kaelthasAI*>(creatureTarget->AI()))
            pKaelAI->AdvisorDefeated(caster->GetEntry());

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## advisor_base_ai
######*/

struct advisor_base_ai : public ScriptedAI
{
    advisor_base_ai(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        Reset();
    }

    ScriptedInstance* m_instance;

    bool m_bFakeDeath;
    bool m_bCanFakeDeath;

    uint32 m_attackTimer;

    void Reset() override
    {
        m_bCanFakeDeath = true;
        m_bFakeDeath    = false;

        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

        m_attackTimer = 0;

        SetCombatMovement(true);
        SetCombatScriptStatus(false);

        ResetTimers();
    }

    virtual void ResetTimers()
    {

    }

    void JustReachedHome() override
    {
        // Reset Kael if needed
        if (m_instance)
        {
            if (Creature* pKael = m_instance->GetSingleCreatureFromStorage(NPC_KAELTHAS))
                pKael->AI()->EnterEvadeMode();

            m_instance->SetData(TYPE_KAELTHAS, FAIL);
        }
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        // Allow fake death only in the first phase
        if (!m_bCanFakeDeath)
            return;

        if (damage < m_creature->GetHealth())
            return;

        // Make sure it won't die by accident
        if (m_bFakeDeath)
        {
            damage = std::min(damage, m_creature->GetHealth() - 1);
            return;
        }

        damage = std::min(damage, m_creature->GetHealth() - 1);
        m_bFakeDeath = true;

        m_creature->InterruptNonMeleeSpells(true);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        m_creature->ClearAllReactives();
        m_creature->MeleeAttackStop(m_creature->GetVictim());
        SetCombatMovement(false);
        SetCombatScriptStatus(true);

        DoCastSpellIfCan(m_creature, SPELL_PERMANENT_FEIGN_DEATH);

        if (m_instance)
        {
            if (Creature* kael = m_instance->GetSingleCreatureFromStorage(NPC_KAELTHAS))
                if (boss_kaelthasAI* pKaelAI = dynamic_cast<boss_kaelthasAI*>(kael->AI()))
                    pKaelAI->AdvisorDefeated(m_creature->GetEntry());
        }
    }

    void SpellHit(Unit* caster, const SpellEntry* spellInfo) override
    {
        // Remove fake death
        if (spellInfo->Id == SPELL_RESURRECTION && caster->GetEntry() == NPC_KAELTHAS)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_PERMANENT_FEIGN_DEATH);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            m_bCanFakeDeath = false;
            m_attackTimer = 2000;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_attackTimer)
        {
            if (m_attackTimer <= uiDiff)
            {
                m_attackTimer = 0;
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                m_bFakeDeath = false;
                SetCombatScriptStatus(false);
                m_creature->MeleeAttackStart(m_creature->GetVictim());
                SetCombatMovement(true);
                SetCombatScriptStatus(false);
                SetReactState(REACT_AGGRESSIVE);
                ResetTimers();
                DoResetThreat();
                DoStartMovement(m_creature->GetVictim());
            }
            else
                m_attackTimer -= uiDiff;

            return;
        }
    }
};

/*######
## boss_thaladred_the_darkener
######*/

struct boss_thaladred_the_darkenerAI : public advisor_base_ai
{
    boss_thaladred_the_darkenerAI(Creature* creature) : advisor_base_ai(creature) { Reset(); }

    uint32 m_uiGazeTimer;
    uint32 m_uiRendTimer;
    uint32 m_uiSilenceTimer;
    uint32 m_uiPsychicBlowTimer;

    void Reset() override
    {
        m_uiGazeTimer        = 0;
        m_uiRendTimer        = urand(4000, 8000);
        m_uiSilenceTimer     = 5000;
        m_uiPsychicBlowTimer = 25000;

        advisor_base_ai::Reset();
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_THALADRED_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_THALADRED_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        advisor_base_ai::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Don't use abilities during fake death
        if (m_bFakeDeath)
            return;

        if (m_uiGazeTimer <= uiDiff)
        {
            SelectAttackingTargetParams parameters;
            parameters.range.minRange = 0.f;
            parameters.range.maxRange = 200.f;
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_RANGE_RANGE, parameters))
            {
                DoResetThreat();
                m_creature->AddThreat(target, 1000000.f);
                DoScriptText(EMOTE_THALADRED_GAZE, m_creature, target);
            }
            m_uiGazeTimer = 10000;
        }
        else
            m_uiGazeTimer -= uiDiff;

        if (m_uiRendTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_REND) == CAST_OK)
                m_uiRendTimer = urand(7000, 12000);
        }
        else
            m_uiRendTimer -= uiDiff;

        if (m_uiSilenceTimer <= uiDiff)
        {
            if (m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, nullptr, SELECT_FLAG_IN_MELEE_RANGE))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SILENCE) == CAST_OK)
                    m_uiSilenceTimer = urand(7000, 13000);
            }
        }
        else
            m_uiSilenceTimer -= uiDiff;

        if (m_uiPsychicBlowTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_PSYCHIC_BLOW) == CAST_OK)
                m_uiPsychicBlowTimer = urand(20000, 25000);
        }
        else
            m_uiPsychicBlowTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_lord_sanguinar
######*/

struct boss_lord_sanguinarAI : public advisor_base_ai
{
    boss_lord_sanguinarAI(Creature* creature) : advisor_base_ai(creature)
    { 
        m_paramsBellowingRoar.range.minRange = 0;
        m_paramsBellowingRoar.range.maxRange = 35;
        Reset();
    }

    uint32 m_uiFearTimer;
    bool m_fearActive;
    SelectAttackingTargetParams m_paramsBellowingRoar;

    void Reset() override
    {
        m_uiFearTimer = 10000;
        m_fearActive = false;

        DoCastSpellIfCan(m_creature, SPELL_THRASH, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

        advisor_base_ai::Reset();
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_SANGUINAR_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_SANGUINAR_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        advisor_base_ai::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Don't use abilities during fake death
        if (m_bFakeDeath)
            return;

        if (!m_fearActive)
        {
            if (m_uiFearTimer <= uiDiff)
            {
                m_uiFearTimer = 0;
                m_fearActive = true;
            }
            else
                m_uiFearTimer -= uiDiff;
        }

        if (m_creature->IsNonMeleeSpellCasted(false) || !CanExecuteCombatAction())
            return;

        if (m_fearActive)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_RANGE_AOE_RANGE, m_paramsBellowingRoar))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BELLOWING_ROAR) == CAST_OK)
                {
                    m_uiFearTimer = 30000;
                    m_fearActive = false;
                }
            }
        }

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_grand_astromancer_capernian
######*/

struct boss_grand_astromancer_capernianAI : public advisor_base_ai
{
    boss_grand_astromancer_capernianAI(Creature* creature) : advisor_base_ai(creature) { Reset(); }

    uint32 m_uiFireballTimer;
    uint32 m_uiConflagrationTimer;
    uint32 m_uiArcaneExplosionTimer;

    void Reset() override
    {
        m_attackDistance = 30.0f;

        advisor_base_ai::Reset();
    }

    void ResetTimers() override
    {
        m_uiFireballTimer = 0;
        m_uiConflagrationTimer = 20000;
        m_uiArcaneExplosionTimer = 5000;
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_CAPERNIAN_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_CAPERNIAN_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        advisor_base_ai::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Don't use abilities during fake death
        if (m_bFakeDeath)
            return;

        if (m_uiConflagrationTimer <= uiDiff)
        {
            m_uiConflagrationTimer = 0;
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_CONFLAGRATION, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(target, SPELL_CONFLAGRATION) == CAST_OK)
                {
                    m_uiConflagrationTimer = urand(16000, 18000);
                    return;
                }
            }
        }
        else
            m_uiConflagrationTimer -= uiDiff;

        if (m_uiArcaneExplosionTimer <= uiDiff)
        {
            m_uiArcaneExplosionTimer = 0;
            if (m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, nullptr, SELECT_FLAG_IN_MELEE_RANGE))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ARCANE_BURST) == CAST_OK)
                {
                    m_uiArcaneExplosionTimer = urand(4000, 6000);
                    return;
                }
            }
        }
        else
            m_uiArcaneExplosionTimer -= uiDiff;

        if (m_uiFireballTimer <= uiDiff)
        {
            m_uiFireballTimer = 0;
            if (!m_creature->IsSpellReady(SPELL_CAPERNIAN_FIREBALL))
            {
                m_attackDistance = 0.0f;
                m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim(), m_attackDistance, m_attackAngle, m_moveFurther);
            }
            else if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CAPERNIAN_FIREBALL) == CAST_OK)
            {
                m_uiFireballTimer = 2000;
                if (m_attackDistance == 0.0f)
                {
                    m_attackDistance = 30.0f;
                    m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim(), m_attackDistance, m_attackAngle, m_moveFurther);
                }
                return;
            }
        }
        else
            m_uiFireballTimer -= uiDiff;

        // Do NOT deal any melee damage.
    }
};

/*######
## boss_master_engineer_telonicus
######*/

/*
Telonicus is supposed to be a hybrid caster
chases in melee only if approached
when ranged, either uses shoot ability when out of range of bomb, or bomb exclusively
*/

struct boss_master_engineer_telonicusAI : public advisor_base_ai
{
    boss_master_engineer_telonicusAI(Creature* creature) : advisor_base_ai(creature)
    {
        m_creature->SetModifierValue(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_PCT, 1.f); // sniff evidence seems to point out that telonicus isnt subject to 50% offhand dmg modifier
        // TODO: Maybe true for all mobs?
        Reset();
    }

    uint32 m_uiBombTimer;
    uint32 m_uiRemoteToyTimer;

    void Reset() override
    {
        m_uiBombTimer       = 4000;
        m_uiRemoteToyTimer  = 5000;

        advisor_base_ai::Reset();
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_TELONICUS_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_TELONICUS_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        advisor_base_ai::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Don't use abilities during fake death
        if (m_bFakeDeath)
            return;

        if (m_uiBombTimer <= uiDiff)
        {
            m_uiBombTimer = 0;
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_BOMB) == CAST_OK)
                m_uiBombTimer = urand(4000, 5000);
        }
        else
            m_uiBombTimer -= uiDiff;

        if (m_uiRemoteToyTimer <= uiDiff)
        {
            m_uiRemoteToyTimer = 0;
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_REMOTE_TOY, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_AURA))
            {
                if (DoCastSpellIfCan(target, SPELL_REMOTE_TOY) == CAST_OK)
                    m_uiRemoteToyTimer = urand(10000, 15000);
            }
        }
        else
            m_uiRemoteToyTimer -= uiDiff;

        DoMeleeAttackIfReady();
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

    void Reset() override {}

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        DoCastSpellIfCan(nullptr, SPELL_NETHER_VAPOR_PERIODIC_DAMAGE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_NETHER_VAPOR_PERIODIC_SCRIPT, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_NETHER_VAPOR_LIGHTNING, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }
};

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

struct NetherVaporSummon : public SpellScript
{
    void OnDestTarget(Spell* spell) const override
    {
        spell->m_targets.m_destPos.z += 5.f;
    }
};

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

struct GravityLapseKnockup : public AuraScript
{
    void OnPeriodicTickEnd(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target->IsAboveGround(4.f)) // knock up player if he is too close to the ground
            target->CastSpell(nullptr, 35938, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_kaelthas()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_kaelthas";
    pNewScript->GetAI = &GetNewAIInstance<boss_kaelthasAI>;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_kael_phase_2;
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

    RegisterSpellScript<NetherVaporLightning>("spell_nether_vapor_lightning");
    RegisterSpellScript<NetherVaporSummon>("spell_nether_vapor_summon");
    RegisterSpellScript<NetherVaporSummonParent>("spell_nether_vapor_summon_parent");
    RegisterSpellScript<RemoveWeapons>("spell_remove_weapons");
    RegisterSpellScript<GravityLapseKnockup>("spell_gravity_lapse_knockup");
}

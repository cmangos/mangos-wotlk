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
SDName: Boss_KelThuzad
SD%Complete: 75
SDComment: Timers need major overhault. Find good data.
SDCategory: Naxxramas
EndScriptData */

// some not answered questions:
// - will intro mobs, not sent to center, despawn when phase 2 start?
// - what happens if raid fail, can they start the event as soon after as they want?

#include "AI/ScriptDevAI/base/BossAI.h"
#include "AI/ScriptDevAI/include/sc_common.h"
#include "naxxramas.h"

enum
{
    SAY_SUMMON_MINIONS                  = 12999,         // start of phase 1

    EMOTE_PHASE2                        = 32803,         // start of phase 2
    SAY_AGGRO1                          = 12995,
    SAY_AGGRO2                          = 12996,
    SAY_AGGRO3                          = 12997,

    SAY_SLAY1                           = 13021,
    SAY_SLAY2                           = 13022,

    SAY_DEATH                           = 13019,

    SAY_CHAIN1                          = 13017,
    SAY_CHAIN2                          = 13018,
    SAY_FROST_BLAST                     = 13020,

    SAY_REQUEST_AID                     = 12998,         // start of phase 3
    SAY_ANSWER_REQUEST                  = 12994,         // lich king answer

    SAY_SPECIAL1_MANA_DET               = 13492,

    EMOTE_GUARDIAN                      = 32804,         // at each guardian summon

    // spells to be casted
    SPELL_FROST_BOLT                    = 28478,
    SPELL_FROST_BOLT_H                  = 55802,
    SPELL_FROST_BOLT_NOVA               = 28479,
    SPELL_FROST_BOLT_NOVA_H             = 55807,

    SPELL_CHAINS_OF_KELTHUZAD           = 28408,            // 3.x, heroic only
    SPELL_CHAINS_OF_KELTHUZAD_SCALE     = 28409,
    SPELL_CHAINS_OF_KELTHUZAD_TARGET    = 28410,

    SPELL_MANA_DETONATION               = 27819,
    SPELL_SHADOW_FISSURE                = 27810,
    SPELL_FROST_BLAST                   = 27808,
    SPELL_FROST_BLAST_DAMAGE            = 29879,            // Frost Blast: damage spell

    SPELL_CHANNEL_VISUAL                = 29423,

    MAX_SOLDIER_COUNT                   = 71,
    MAX_ABOMINATION_COUNT               = 8,
    MAX_BANSHEE_COUNT                   = 8,

    // Call forth one of the add to the center of the room
    // Soldier of the Frozen Waste (triggers 28415)
    SPELL_SUMMON_PERIODIC_A_1           = 29410,            // Every 5 secs
    SPELL_SUMMON_PERIODIC_A_2           = 29391,            // Every 4 secs
    SPELL_SUMMON_PERIODIC_A_3           = 28425,            // Every 3 secs
    SPELL_SUMMON_PERIODIC_A_4           = 29392,            // Every 2 secs
    SPELL_SUMMON_PERIODIC_A_5           = 29409,            // Every 1 secs
    // Unstoppable Abomination (triggers 28416)
    SPELL_SUMMON_PERIODIC_B_1           = 28426,            // Every 30 secs
    SPELL_SUMMON_PERIODIC_B_2           = 29393,            // Every 25 secs
    SPELL_SUMMON_PERIODIC_B_3           = 29394,            // Every 20 secs
    SPELL_SUMMON_PERIODIC_B_4           = 29398,            // Every 15 secs
    SPELL_SUMMON_PERIODIC_B_5           = 29411,            // Every 10 secs
    // Soul Weaver (triggers 28417)
    SPELL_SUMMON_PERIODIC_C_1           = 29399,            // Every 40 secs
    SPELL_SUMMON_PERIODIC_C_2           = 29400,            // Every 35 secs
    SPELL_SUMMON_PERIODIC_C_3           = 28427,            // Every 30 secs
    SPELL_SUMMON_PERIODIC_C_4           = 29401,            // Every 20 secs
    SPELL_SUMMON_PERIODIC_C_5           = 29412,            // Every 15 secs

    SPELL_SUMMON_TYPE_A                 = 28421,            // Summon 1 Soldier of the Frozen Waste
    SPELL_SUMMON_TYPE_B                 = 28422,            // Summon 1 Unstoppable Abomination
    SPELL_SUMMON_TYPE_C                 = 28423,            // Summon 1 Soul Weaver
    SPELL_SUMMON_TYPE_D                 = 28454,            // Summon 1 Guardian of Icecrown
    SPELL_SUMMON_PERIODIC_D             = 28453,            // One Guardian of Icecrown summoned every 10 secs
    SPELL_GUARDIAN_INIT                 = 29898,            // Cast at start of phase 3 by Kel'Thuzad (start of phase 2 in sniffs but this way we avoid running a timer needlessly)

    MAX_CONTROLLED_TARGETS              = 2,                // 2 targets are mind controlled by Chains of Kel'Thuzad in 25

    ACHIEV_REQ_KILLED_ABOMINATIONS      = 18,
};

static const uint32 phaseOneAdds[] = {NPC_SOLDIER_FROZEN, NPC_UNSTOPPABLE_ABOM, NPC_SOUL_WEAVER};

// List which spell must be used at which point in phase 1 to summon a given NPC type
static const std::vector<std::vector<uint32>> soulWeaverSpellsTimers
{
        {1,   SPELL_SUMMON_PERIODIC_C_1},   // 1 for 40 sec
        {42,  SPELL_SUMMON_PERIODIC_C_4},   // 1 for 20 sec
        {63,  SPELL_SUMMON_PERIODIC_C_3},   // 3 for 90 sec
        {154, SPELL_SUMMON_PERIODIC_C_5},   // 9 for 135 sec
        {290, 0}                            // Total: 14 Soul Weavers
};

static const std::vector<std::vector<uint32>> unstoppableAbominationSpellsTimers
{
        {15,  SPELL_SUMMON_PERIODIC_B_1},   // 2 for 60 sec
        {76,  SPELL_SUMMON_PERIODIC_B_2},   // 2 for 50 sec
        {127, SPELL_SUMMON_PERIODIC_B_3},   // 5 for 100 sec
        {228, SPELL_SUMMON_PERIODIC_B_4},   // 3 for 45 sec
        {259, SPELL_SUMMON_PERIODIC_B_5},   // 2 for 20 sec
        {291, 0}                            // Total: 14 Unstoppable Abominations
};

static const std::vector<std::vector<uint32>> soldierFrozenWasteSpellsTimers
{
        {7,   SPELL_SUMMON_PERIODIC_A_1},   // 13 for 65 secs
        {73,  SPELL_SUMMON_PERIODIC_A_2},   // 15 for 60 secs
        {134, SPELL_SUMMON_PERIODIC_A_3},   // 18 for 54 secs
        {189, SPELL_SUMMON_PERIODIC_A_4},   // 27 for 54 secs
        {244, SPELL_SUMMON_PERIODIC_A_5},   // 47 for 47 secs
        {292, 0}                            // Total: 120 Soldiers of the Frozen Wastes
};

// List all spells used to summon a given NPC type during phase 1
static const std::map<uint32, std::vector<uint32>> summoningSpells
{
        {NPC_SOUL_WEAVER,      {SPELL_SUMMON_PERIODIC_C_1, SPELL_SUMMON_PERIODIC_C_2, SPELL_SUMMON_PERIODIC_C_3, SPELL_SUMMON_PERIODIC_C_4, SPELL_SUMMON_PERIODIC_C_5}},
        {NPC_UNSTOPPABLE_ABOM, {SPELL_SUMMON_PERIODIC_B_1, SPELL_SUMMON_PERIODIC_B_2, SPELL_SUMMON_PERIODIC_B_3, SPELL_SUMMON_PERIODIC_B_4, SPELL_SUMMON_PERIODIC_B_5}},
        {NPC_SOLDIER_FROZEN,   {SPELL_SUMMON_PERIODIC_A_1, SPELL_SUMMON_PERIODIC_A_2, SPELL_SUMMON_PERIODIC_A_3, SPELL_SUMMON_PERIODIC_A_4, SPELL_SUMMON_PERIODIC_A_5}}
};

static float M_F_ANGLE = 0.2f;                              // to adjust for map rotation
static float M_F_HEIGHT = 2.0f;                             // adjust for height difference
static float M_F_RANGE = 55.0f;                             // ~ range from center of chamber to center of alcove

enum Phase
{
    PHASE_INTRO,
    PHASE_NORMAL,
    PHASE_GUARDIANS,
};

enum KelThuzadActions
{
    KELTHUZAD_SUMMON_GUARDIAN,
    KELTHUZAD_PHASE_GUARDIANS,
    KELTHUZAD_ACTIONS_MAX,
    KELTHUZAD_COMBAT_PHASE,
    KELTHUZAD_SUMMON_INTRO,
    KELTHUZAD_SUMMON_SOLDIER,
    KELTHUZAD_SUMMON_WEAVER,
    KELTHUZAD_SUMMON_ABO,
};

struct boss_kelthuzadAI : public BossAI
{
    boss_kelthuzadAI(Creature* creature) : BossAI(creature, KELTHUZAD_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_naxxramas*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_KELTHUZAD);
        AddOnKillText(SAY_SLAY1, SAY_SLAY2);
        AddOnDeathText(SAY_DEATH);
        AddOnAggroText(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3);
        AddCombatAction(KELTHUZAD_SUMMON_GUARDIAN, true);
        AddTimerlessCombatAction(KELTHUZAD_PHASE_GUARDIANS, false);
        AddCustomAction(KELTHUZAD_COMBAT_PHASE, true, [&]()
        {
            SetCombatScriptStatus(false);
            m_uiPhase = PHASE_NORMAL;
            DespawnIntroCreatures();

            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING | UNIT_FLAG_UNINTERACTIBLE);
            SetCombatMovement(true);
            m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());

            AddInitialCooldowns();

            DoScriptText(EMOTE_PHASE2, m_creature);
        });
        AddCustomAction(KELTHUZAD_SUMMON_SOLDIER, true, [&]()
        {
            if (m_uiSoldierCount >= MAX_SOLDIER_COUNT || m_uiPhase != PHASE_INTRO)
            {
                DisableTimer(KELTHUZAD_SUMMON_SOLDIER);
                return;
            }
            SummonMob(NPC_SOLDIER_FROZEN);
            ResetTimer(KELTHUZAD_SUMMON_SOLDIER, 3s);
        });
        AddCustomAction(KELTHUZAD_SUMMON_WEAVER, true, [&]()
        {
            if (m_uiBansheeCount >= MAX_BANSHEE_COUNT || m_uiPhase != PHASE_INTRO)
            {
                DisableTimer(KELTHUZAD_SUMMON_WEAVER);
                return;
            }
            SummonMob(NPC_SOUL_WEAVER);
            ResetTimer(KELTHUZAD_SUMMON_WEAVER, 25s);
        });
        AddCustomAction(KELTHUZAD_SUMMON_ABO, true, [&]()
        {
            if (m_uiAbominationCount >= MAX_ABOMINATION_COUNT || m_uiPhase != PHASE_INTRO)
            {
                DisableTimer(KELTHUZAD_SUMMON_ABO);
                return;
            }
            SummonMob(NPC_UNSTOPPABLE_ABOM);
            ResetTimer(KELTHUZAD_SUMMON_ABO, 3s);
        });
        m_uiGuardiansCountMax = m_isRegularMode ? 2 : 4;
    }

    instance_naxxramas* m_instance;
    bool m_isRegularMode;

    uint32 m_uiGuardiansCount;
    uint32 m_uiGuardiansCountMax;

    uint8  m_uiPhase;
    uint32 m_uiSoldierCount;
    uint32 m_uiBansheeCount;
    uint32 m_uiAbominationCount;
    uint32 m_uiIntroPackCount;
    uint32 m_uiKilledAbomination;

    GuidVector m_introMobsSet;
    GuidSet m_lAddsSet;

    void Reset() override
    {
        BossAI::Reset();
        m_uiGuardiansCount      = 0;
        m_uiIntroPackCount      = 0;

        m_uiSoldierCount        = 0;
        m_uiBansheeCount        = 0;
        m_uiAbominationCount    = 0;
        m_uiKilledAbomination   = 0;
        m_uiPhase               = PHASE_INTRO;

        // it may be some spell should be used instead, to control the intro phase
        SetCombatScriptStatus(true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING | UNIT_FLAG_UNINTERACTIBLE);
        m_creature->CastStop();
        SetCombatMovement(false);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() != TYPEID_PLAYER)
            return;

        if (urand(0, 1))
            DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        DespawnAdds();

        if (m_instance)
            m_instance->SetData(TYPE_KELTHUZAD, DONE);
    }

    void JustReachedHome() override
    {
        DespawnIntroCreatures();
        DespawnAdds();

        if (m_instance)
            m_instance->SetData(TYPE_KELTHUZAD, NOT_STARTED);
    }

    void Aggro(Unit* enemy) override
    {
        ResetTimer(KELTHUZAD_COMBAT_PHASE, 3min + 48s);
        DoScriptText(SAY_SUMMON_MINIONS, m_creature);
        DoCastSpellIfCan(nullptr, SPELL_CHANNEL_VISUAL);
        AddCustomAction(KELTHUZAD_SUMMON_INTRO, 2s, [&]()
        {
            SummonIntroCreatures(m_uiIntroPackCount);
            ++m_uiIntroPackCount;
            if (m_uiIntroPackCount < 8)
                ResetTimer(KELTHUZAD_SUMMON_INTRO, 2s);
        });
        ResetTimer(KELTHUZAD_SUMMON_SOLDIER, 5s);
        ResetTimer(KELTHUZAD_SUMMON_WEAVER, 5s);
        ResetTimer(KELTHUZAD_SUMMON_ABO, 5s);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (m_instance && m_instance->GetData(TYPE_KELTHUZAD) != IN_PROGRESS)
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void DespawnIntroCreatures()
    {
        DespawnGuids(m_introMobsSet);
    }

    void DespawnAdds()
    {
        if (m_instance)
        {
            for (auto itr : m_lAddsSet)
            {
                if (Creature* creature = m_instance->instance->GetCreature(itr))
                {
                    if (creature->IsAlive())
                    {
                        creature->AI()->EnterEvadeMode();
                        creature->ForcedDespawn(15000);
                    }
                }
            }
        }

        m_lAddsSet.clear();
    }

    float GetLocationAngle(uint32 uiId) const
    {
        switch (uiId)
        {
            case 1: return M_PI_F - M_F_ANGLE;              // south
            case 2: return M_PI_F / 2 * 3 - M_F_ANGLE;      // east
            case 3: return M_PI_F / 2 - M_F_ANGLE;          // west
            case 4: return M_PI_F / 4 - M_F_ANGLE;          // north-west
            case 5: return M_PI_F / 4 * 7 - M_F_ANGLE;      // north-east
            case 6: return M_PI_F / 4 * 5 - M_F_ANGLE;      // south-east
            case 7: return M_PI_F / 4 * 3 - M_F_ANGLE;      // south-west
        }

        return M_F_ANGLE;
    }

    void SummonIntroCreatures(uint32 packId)
    {
        if (!m_instance)
            return;

        float fAngle = GetLocationAngle(packId + 1);

        float fX, fY, fZ;
        m_instance->GetChamberCenterCoords(fX, fY, fZ);

        fX += M_F_RANGE * cos(fAngle);
        fY += M_F_RANGE * sin(fAngle);
        fZ += M_F_HEIGHT;

        MaNGOS::NormalizeMapCoord(fX);
        MaNGOS::NormalizeMapCoord(fY);

        uint32 uiNpcEntry = NPC_SOUL_WEAVER;

        for (uint8 uiI = 0; uiI < 14; ++uiI)
        {
            if (uiI > 0)
            {
                if (uiI < 4)
                    uiNpcEntry = NPC_UNSTOPPABLE_ABOM;
                else
                    uiNpcEntry = NPC_SOLDIER_FROZEN;
            }

            float fNewX, fNewY, fNewZ;
            m_creature->GetRandomPoint(fX, fY, fZ, 12.0f, fNewX, fNewY, fNewZ);

            m_creature->SummonCreature(uiNpcEntry, fNewX, fNewY, fNewZ, fAngle + M_PI_F, TEMPSPAWN_CORPSE_DESPAWN, 5000);
        }
    }

    void SummonMob(uint32 uiType)
    {
        if (!m_instance)
            return;

        float fAngle = GetLocationAngle(urand(1, 7));

        float fX, fY, fZ;
        m_instance->GetChamberCenterCoords(fX, fY, fZ);

        fX += M_F_RANGE * cos(fAngle);
        fY += M_F_RANGE * sin(fAngle);
        fZ += M_F_HEIGHT;

        MaNGOS::NormalizeMapCoord(fX);
        MaNGOS::NormalizeMapCoord(fY);

        m_creature->SummonCreature(uiType, fX, fY, fZ, 0.0f, TEMPSPAWN_CORPSE_DESPAWN, 5000);
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_GUARDIAN:
            {
                DoScriptText(EMOTE_GUARDIAN, m_creature);

                m_lAddsSet.insert(summoned->GetObjectGuid());
                ++m_uiGuardiansCount;

                summoned->SetInCombatWithZone();
                break;
            }
            case NPC_SOLDIER_FROZEN:
            case NPC_UNSTOPPABLE_ABOM:
            case NPC_SOUL_WEAVER:
            {
                if (m_uiIntroPackCount < 7)
                    m_introMobsSet.push_back(summoned->GetObjectGuid());
                else
                {
                    m_lAddsSet.insert(summoned->GetObjectGuid());

                    if (m_instance)
                    {
                        float fX, fY, fZ;
                        m_instance->GetChamberCenterCoords(fX, fY, fZ);
                        summoned->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                    }
                }

                break;
            }
        }
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_GUARDIAN:
            case NPC_SOLDIER_FROZEN:
            case NPC_SOUL_WEAVER:
                m_lAddsSet.erase(summoned->GetObjectGuid());
                break;
            case NPC_UNSTOPPABLE_ABOM:
                m_lAddsSet.erase(summoned->GetObjectGuid());

                ++m_uiKilledAbomination;
                if (m_uiKilledAbomination >= ACHIEV_REQ_KILLED_ABOMINATIONS)
                    m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_GET_ENOUGH, true);

                break;
        }
    }

    void SummonedMovementInform(Creature* summoned, uint32 motionType, uint32 pointId) override
    {
        if (motionType == POINT_MOTION_TYPE && pointId == 0)
            summoned->SetInCombatWithZone();
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* target) override
    {
        switch (spellInfo->Id)
        {
			case SPELL_CHAINS_OF_KELTHUZAD: DoScriptText(urand(0, 1) ? SAY_CHAIN1 : SAY_CHAIN2, m_creature); break;
            case SPELL_FROST_BLAST: if (urand(0, 1)) DoScriptText(SAY_FROST_BLAST, m_creature); break;
            case SPELL_MANA_DETONATION: if (urand(0, 1)) DoScriptText(SAY_SPECIAL1_MANA_DET, m_creature); break;
        }
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case KELTHUZAD_SUMMON_GUARDIAN: return 5s;
            default: return 0s;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KELTHUZAD_PHASE_GUARDIANS:
            {
                if (m_creature->GetHealthPercent() < 45.0f)
                {
                    ResetCombatAction(KELTHUZAD_SUMMON_GUARDIAN, GetSubsequentActionTimer(KELTHUZAD_SUMMON_GUARDIAN));
                    DoScriptText(SAY_REQUEST_AID, m_creature);
                    DisableCombatAction(action);
                }
                return;
            }
            case KELTHUZAD_SUMMON_GUARDIAN:
            {
                if (m_uiGuardiansCount >= m_uiGuardiansCountMax)
                {
                    DisableCombatAction(action);
                    return;
                }
                SummonMob(NPC_GUARDIAN);
                break;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

// Summon one add (which type depends on spell)
struct TriggerKTAdd : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /* effIdx */) const override
    {
        if (Unit* unitTarget = spell->GetUnitTarget())
        {
            switch (spell->m_spellInfo->Id)
            {
                case 28415:
                    unitTarget->CastSpell(unitTarget, SPELL_SUMMON_TYPE_A, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, spell->GetCaster()->GetObjectGuid(), nullptr);
                    break;
                case 28416:
                    unitTarget->CastSpell(unitTarget, SPELL_SUMMON_TYPE_B, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, spell->GetCaster()->GetObjectGuid(), nullptr);
                    break;
                case 28417:
                    unitTarget->CastSpell(unitTarget, SPELL_SUMMON_TYPE_C, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, spell->GetCaster()->GetObjectGuid(), nullptr);
                    break;
                case 28455:
                    unitTarget->CastSpell(unitTarget, SPELL_SUMMON_TYPE_D, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, spell->GetCaster()->GetObjectGuid(), nullptr);
                    break;
                default:
                    break;
            }
        }
    }
};

// Select four random players plus the main tank and mind control them all. Evil.
struct ChainsKelThuzad : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /* effIdx */) const override
    {
        Unit* caster = spell->GetCaster();

        SelectAttackingTargetParams chainsParams;
        chainsParams.range.minRange = 0;
        chainsParams.range.maxRange = 100;

        std::vector<Unit*> targets;
        // Skip the main tank because we will add it later
        caster->SelectAttackingTargets(targets, ATTACKING_TARGET_ALL_SUITABLE, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_SKIP_TANK, chainsParams);

        // Prevent the mind control to happen if all remaining players would be targeted
        if (targets.size() <= MAX_CONTROLLED_TARGETS)
            return;

        std::shuffle(targets.begin(), targets.end(), *GetRandomGenerator());
        targets.resize(MAX_CONTROLLED_TARGETS);

        DoScriptText(urand(0, 1) ? SAY_CHAIN1 : SAY_CHAIN2, caster);

        // Mind control the random targets
        for (auto& target : targets)
        {
            caster->CastSpell(target, SPELL_CHAINS_OF_KELTHUZAD_TARGET, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(nullptr, SPELL_CHAINS_OF_KELTHUZAD_SCALE, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

// Do damage onto the player equal to 26% of his/her full hit points on every tick
struct FrostBlast : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& /* data */) const override
    {
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;

        if (Unit* target = aura->GetTarget())
        {
            int32 basePointsDamage = target->GetMaxHealth() * 26 / 100;
            caster->CastCustomSpell(target, SPELL_FROST_BLAST_DAMAGE, &basePointsDamage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

void AddSC_boss_kelthuzad()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_kelthuzad";
    pNewScript->GetAI = &GetNewAIInstance<boss_kelthuzadAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<TriggerKTAdd>("spell_trigger_KT_add");
    RegisterSpellScript<ChainsKelThuzad>("spell_chains_kel_thuzad");
    RegisterSpellScript<FrostBlast>("spell_kel_thuzad_frost_blast");
}

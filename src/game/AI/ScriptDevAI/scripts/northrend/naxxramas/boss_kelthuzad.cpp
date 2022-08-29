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
    SPELL_CHAINS_OF_KELTHUZAD_TARGET    = 28410,

    SPELL_MANA_DETONATION               = 27819,
    SPELL_SHADOW_FISSURE                = 27810,
    SPELL_FROST_BLAST                   = 27808,

    SPELL_CHANNEL_VISUAL                = 29423,

    MAX_SOLDIER_COUNT                   = 71,
    MAX_ABOMINATION_COUNT               = 8,
    MAX_BANSHEE_COUNT                   = 8,

    ACHIEV_REQ_KILLED_ABOMINATIONS      = 18,
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
    KELTHUZAD_FROSTBOLT,
    KELTHUZAD_FROSTBOLT_NOVA,
    KELTHUZAD_CHAINS,
    KELTHUZAD_SHADOW_FISSURE,
    KELTHUZAD_MANA_DETONATION,
    KELTHUZAD_FROST_BLAST,
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
        AddCombatAction(KELTHUZAD_FROSTBOLT, 1s, 60s);
        AddCombatAction(KELTHUZAD_FROSTBOLT_NOVA, 15s);
        AddCombatAction(KELTHUZAD_CHAINS, 30s, 60s);
        AddCombatAction(KELTHUZAD_MANA_DETONATION, 20s);
        AddCombatAction(KELTHUZAD_SHADOW_FISSURE, 25s);
        AddCombatAction(KELTHUZAD_FROST_BLAST, 30s, 60s);
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

            DoScriptText(EMOTE_PHASE2, m_creature);

            BossAI::Aggro();
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
        Reset();
    }

    instance_naxxramas* m_instance;
    bool m_isRegularMode;

    uint32 m_uiGuardiansCount;
    uint32 m_uiGuardiansCountMax;

    uint8  m_uiPhase;
    uint32 m_uiSoldierCount;
    uint32 m_uiBansheeCount;
    uint32 m_uiAbominationCount;
    uint32 m_uiSummonIntroTimer;
    uint32 m_uiIntroPackCount;
    uint32 m_uiKilledAbomination;

    GuidSet m_lIntroMobsSet;
    GuidSet m_lAddsSet;

    void Reset() override
    {
        m_uiGuardiansCount      = 0;
        m_uiSummonIntroTimer    = 0;
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

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        if (urand(0, 1))
            DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
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

    void Aggro(Unit* pWho) override
    {
        if (m_uiPhase != PHASE_INTRO)
            return;
        ResetTimer(KELTHUZAD_COMBAT_PHASE, 3min + 48s);
        DoScriptText(SAY_SUMMON_MINIONS, m_creature);
        DoCastSpellIfCan(nullptr, SPELL_CHANNEL_VISUAL);
        AddCustomAction(KELTHUZAD_SUMMON_INTRO, 2s, [&](){
            SummonIntroCreatures(m_uiIntroPackCount);
            ++m_uiIntroPackCount;
            if (m_uiIntroPackCount < 8)
                ResetTimer(KELTHUZAD_SUMMON_INTRO, 2s);
        });
        ResetTimer(KELTHUZAD_SUMMON_SOLDIER, 5s);
        ResetTimer(KELTHUZAD_SUMMON_WEAVER, 5s);
        ResetTimer(KELTHUZAD_SUMMON_ABO, 5s);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_instance && m_instance->GetData(TYPE_KELTHUZAD) != IN_PROGRESS)
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void DespawnIntroCreatures()
    {
        if (m_instance)
        {
            for (auto itr : m_lIntroMobsSet)
            {
                if (Creature* pCreature = m_instance->instance->GetCreature(itr))
                    pCreature->ForcedDespawn();
            }
        }

        m_lIntroMobsSet.clear();
    }

    void DespawnAdds()
    {
        if (m_instance)
        {
            for (auto itr : m_lAddsSet)
            {
                if (Creature* pCreature = m_instance->instance->GetCreature(itr))
                {
                    if (pCreature->IsAlive())
                    {
                        pCreature->AI()->EnterEvadeMode();
                        pCreature->ForcedDespawn(15000);
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

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_GUARDIAN:
            {
                DoScriptText(EMOTE_GUARDIAN, m_creature);

                m_lAddsSet.insert(pSummoned->GetObjectGuid());
                ++m_uiGuardiansCount;

                pSummoned->SetInCombatWithZone();
                break;
            }
            case NPC_SOLDIER_FROZEN:
            case NPC_UNSTOPPABLE_ABOM:
            case NPC_SOUL_WEAVER:
            {
                if (m_uiIntroPackCount < 7)
                    m_lIntroMobsSet.insert(pSummoned->GetObjectGuid());
                else
                {
                    m_lAddsSet.insert(pSummoned->GetObjectGuid());

                    if (m_instance)
                    {
                        float fX, fY, fZ;
                        m_instance->GetChamberCenterCoords(fX, fY, fZ);
                        pSummoned->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                    }
                }

                break;
            }
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_GUARDIAN:
            case NPC_SOLDIER_FROZEN:
            case NPC_SOUL_WEAVER:
                m_lAddsSet.erase(pSummoned->GetObjectGuid());
                break;
            case NPC_UNSTOPPABLE_ABOM:
                m_lAddsSet.erase(pSummoned->GetObjectGuid());

                ++m_uiKilledAbomination;
                if (m_uiKilledAbomination >= ACHIEV_REQ_KILLED_ABOMINATIONS)
                    m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_GET_ENOUGH, true);

                break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType == POINT_MOTION_TYPE && uiPointId == 0)
            pSummoned->SetInCombatWithZone();
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case KELTHUZAD_FROSTBOLT: return RandomTimer(1s, 60s);
            case KELTHUZAD_FROSTBOLT_NOVA: return 15s;
            case KELTHUZAD_MANA_DETONATION: return 20s;
            case KELTHUZAD_SHADOW_FISSURE: return 25s;
            case KELTHUZAD_FROST_BLAST: return RandomTimer(30s, 60s);
            case KELTHUZAD_CHAINS: return RandomTimer(30s, 60s);
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
            case KELTHUZAD_FROSTBOLT:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_FROST_BOLT : SPELL_FROST_BOLT_H) == CAST_OK)
                    break;
                return;
            }
            case KELTHUZAD_FROSTBOLT_NOVA:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_FROST_BOLT_NOVA : SPELL_FROST_BOLT_NOVA_H) == CAST_OK)
                    break;
                return;
            }
            case KELTHUZAD_MANA_DETONATION:
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_MANA_DETONATION, SELECT_FLAG_PLAYER | SELECT_FLAG_POWER_MANA))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_MANA_DETONATION) == CAST_OK)
                    {
                        if (urand(0, 1))
                            DoScriptText(SAY_SPECIAL1_MANA_DET, m_creature);

                        break;
                    }
                }
                return;
            }
            case KELTHUZAD_SHADOW_FISSURE:
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_SHADOW_FISSURE) == CAST_OK)
                        break;
                }
                return;
            }
            case KELTHUZAD_FROST_BLAST:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FROST_BLAST) == CAST_OK)
                {
                    if (urand(0, 1))
                        DoScriptText(SAY_FROST_BLAST, m_creature);

                    break;
                }
                return;
            }
            case KELTHUZAD_CHAINS:
            {
                if (m_isRegularMode)
                {
                    DisableCombatAction(action);
                    return;
                }
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CHAINS_OF_KELTHUZAD) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_CHAIN1 : SAY_CHAIN2, m_creature);

                    break;
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

void AddSC_boss_kelthuzad()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_kelthuzad";
    pNewScript->GetAI = &GetNewAIInstance<boss_kelthuzadAI>;
    pNewScript->RegisterSelf();
}

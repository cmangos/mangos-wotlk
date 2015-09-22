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
SDName: Boss_Zuljin
SD%Complete: 90
SDComment: Timers should be improved.
SDCategory: Zul'Aman
EndScriptData */

#include "precompiled.h"
#include "zulaman.h"

enum
{
    SAY_INTRO                       = -1568056,
    SAY_AGGRO                       = -1568057,
    SAY_BEAR_TRANSFORM              = -1568058,
    SAY_EAGLE_TRANSFORM             = -1568059,
    SAY_LYNX_TRANSFORM              = -1568060,
    SAY_DRAGONHAWK_TRANSFORM        = -1568061,
    SAY_FIRE_BREATH                 = -1568062,
    SAY_BERSERK                     = -1568053,
    SAY_KILL1                       = -1568064,
    SAY_KILL2                       = -1568065,
    SAY_DEATH                       = -1568066,

    EMOTE_BEAR_SPIRIT               = -1568082,
    EMOTE_EAGLE_SPIRIT              = -1568083,
    EMOTE_LYNX_SPIRIT               = -1568084,
    EMOTE_DRAGONHAWK_SPIRIT         = -1568085,

    // Troll Form
    SPELL_WHIRLWIND                 = 17207,
    SPELL_GRIEVOUS_THROW            = 43093,                // removes debuff after full healed

    // Bear Form
    SPELL_CREEPING_PARALYSIS        = 43095,                // should cast on the whole raid
    SPELL_OVERPOWER                 = 43456,                // use after melee attack dodged

    // Eagle Form
    SPELL_ENERGY_STORM              = 43983,                // enemy area aura, trigger 42577 on vortexes which cast 43137 on targets
    SPELL_SUMMON_CYCLONE            = 43112,                // summon four feather vortex
    NPC_FEATHER_VORTEX              = 24136,                // ToDo: script via ACID
    SPELL_CYCLONE_VISUAL            = 43119,                // trigger 43147 visual
    SPELL_CYCLONE_PASSIVE           = 43120,                // trigger 43121 (4y aoe) every second
    SPELL_CYCLONE                   = 43121,

    // Lynx Form
    SPELL_CLAW_RAGE                 = 42583,                // Charges a random target and applies dummy effect 43149 on it
    SPELL_CLAW_RAGE_TRIGGER         = 43149,
    SPELL_LYNX_RUSH                 = 43152,                // Charges 9 targets in a row - Dummy effect should apply 43153
    SPELL_LYNX_RUSH_CHARGE          = 43153,

    // Dragonhawk Form
    SPELL_FLAME_WHIRL               = 43213,                // trigger two spells
    SPELL_FLAME_BREATH              = 43215,
    SPELL_SUMMON_PILLAR             = 43216,                // summon 24187
    NPC_COLUMN_OF_FIRE              = 24187,
    SPELL_PILLAR_TRIGGER            = 43218,                // trigger 43217

    // Cosmetic
    SPELL_SPIRIT_DRAINED            = 42520,
    SPELL_SPIRIT_DRAIN              = 42542,

    // Transforms
    SPELL_SHAPE_OF_THE_BEAR         = 42594,
    SPELL_SHAPE_OF_THE_EAGLE        = 42606,
    SPELL_SHAPE_OF_THE_LYNX         = 42607,
    SPELL_SHAPE_OF_THE_DRAGONHAWK   = 42608,

    SPELL_BERSERK                   = 45078,                // Berserk timer or existance is unk

    MAX_VORTEXES                    = 4,
    MAX_LYNX_RUSH                   = 10,
    POINT_ID_CENTER                 = 0,

    PHASE_BEAR                      = 0,
    PHASE_EAGLE                     = 1,
    PHASE_LYNX                      = 2,
    PHASE_DRAGONHAWK                = 3,
    PHASE_TROLL                     = 4,
};

struct BossPhase
{
    uint32 uiSpiritSpellId;
    int32 iYellId, iEmoteId;
    uint32 uiSpiritId;
    uint8 uiPhase;
};

static const BossPhase aZuljinPhases[] =
{
    {SPELL_SHAPE_OF_THE_BEAR,       SAY_BEAR_TRANSFORM,       EMOTE_BEAR_SPIRIT,       NPC_BEAR_SPIRIT,       PHASE_BEAR},
    {SPELL_SHAPE_OF_THE_EAGLE,      SAY_EAGLE_TRANSFORM,      EMOTE_EAGLE_SPIRIT,      NPC_EAGLE_SPIRIT,      PHASE_EAGLE},
    {SPELL_SHAPE_OF_THE_LYNX,       SAY_LYNX_TRANSFORM,       EMOTE_LYNX_SPIRIT,       NPC_LYNX_SPIRIT,       PHASE_LYNX},
    {SPELL_SHAPE_OF_THE_DRAGONHAWK, SAY_DRAGONHAWK_TRANSFORM, EMOTE_DRAGONHAWK_SPIRIT, NPC_DRAGONHAWK_SPIRIT, PHASE_DRAGONHAWK}
};

// coords for going for changing form
static const float fZuljinMoveLoc[3] = {120.148811f, 703.713684f, 45.111477f};

/*######
## boss_zuljin
######*/

struct boss_zuljinAI : public ScriptedAI
{
    boss_zuljinAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bHasTaunted = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint8 m_uiPhase;
    uint8 m_uiHealthCheck;

    uint32 m_uiWhirlwindTimer;
    uint32 m_uiGrievousThrowTimer;

    uint32 m_uiParalysisTimer;
    uint32 m_uiOverpowerTimer;

    uint32 m_uiClawRageTimer;
    uint32 m_uiLynxRushTimer;
    uint8 m_uiLynxRushCount;

    uint32 m_uiFlameWhirlTimer;
    uint32 m_uiFlameBreathTimer;
    uint32 m_uiPillarOfFireTimer;

    bool m_bHasTaunted;
    bool m_bIsInTransition;
    uint32 m_uiTransformTimer;

    GuidList m_lSummonsList;

    void Reset() override
    {
        m_uiHealthCheck         = 80;
        m_uiPhase               = PHASE_TROLL;

        m_uiWhirlwindTimer      = 7000;
        m_uiGrievousThrowTimer  = 8000;

        m_uiParalysisTimer      = 7000;
        m_uiOverpowerTimer      = 5000;

        m_uiClawRageTimer       = 5000;
        m_uiLynxRushTimer       = 15000;
        m_uiLynxRushCount       = 0;

        m_uiFlameWhirlTimer     = 7000;
        m_uiFlameBreathTimer    = 15000;
        m_uiPillarOfFireTimer   = 7000;

        m_bIsInTransition       = false;

        SetCombatMovement(true);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ZULJIN, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ZULJIN, FAIL);

        // Despawn all feather vortexes
        DoDespawnVortexes();

        // Reset all spirits
        for (uint8 i = 0; i < MAX_VORTEXES; ++i)
        {
            if (Creature* pSpirit = m_pInstance->GetSingleCreatureFromStorage(aZuljinPhases[i].uiSpiritId))
            {
                pSpirit->SetStandState(UNIT_STAND_STATE_STAND);
                pSpirit->AI()->EnterEvadeMode();
            }
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_ZULJIN, DONE);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasTaunted && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 60.0f))
        {
            DoScriptText(SAY_INTRO, m_creature);
            m_bHasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    // Function to handle the Feather Vortexes despawn on phase change
    void DoDespawnVortexes()
    {
        for (GuidList::const_iterator itr = m_lSummonsList.begin(); itr != m_lSummonsList.end(); ++itr)
        {
            if (Creature* pVortex = m_creature->GetMap()->GetCreature(*itr))
                pVortex->ForcedDespawn();
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_FEATHER_VORTEX:
                pSummoned->CastSpell(pSummoned, SPELL_CYCLONE_VISUAL, true);
                pSummoned->CastSpell(pSummoned, SPELL_CYCLONE_PASSIVE, true);
                m_lSummonsList.push_back(pSummoned->GetObjectGuid());

                // Attack random target
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pSummoned->GetMotionMaster()->MoveFollow(pTarget, 0, 0);
                break;
            case NPC_COLUMN_OF_FIRE:
                pSummoned->CastSpell(pSummoned, SPELL_PILLAR_TRIGGER, true);
                break;
        }
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || uiPointId != POINT_ID_CENTER)
            return;

        // increment phase
        if (m_uiPhase == PHASE_TROLL)
            m_uiPhase = PHASE_BEAR;
        else
            ++m_uiPhase;

        // drain the spirit
        if (Creature* pSpirit = m_pInstance->GetSingleCreatureFromStorage(aZuljinPhases[m_uiPhase].uiSpiritId))
            pSpirit->CastSpell(m_creature, SPELL_SPIRIT_DRAIN, false);
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_SPIRIT_DRAIN)
        {
            DoCastSpellIfCan(m_creature, aZuljinPhases[m_uiPhase].uiSpiritSpellId, CAST_INTERRUPT_PREVIOUS);
            DoScriptText(aZuljinPhases[m_uiPhase].iYellId, m_creature);
            DoScriptText(aZuljinPhases[m_uiPhase].iEmoteId, m_creature);

            // in eagle phase we don't move
            if (m_uiPhase != PHASE_EAGLE)
            {
                SetCombatMovement(true);
                if (m_creature->getVictim())
                {
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                }
            }
            // In Eagle phase we just cast Energy storm and summon 4 Feather cyclones; Boss doesn't move in this phase
            else
            {
                DoCastSpellIfCan(m_creature, SPELL_ENERGY_STORM, CAST_TRIGGERED);

                // summon 4 vortexes
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_CYCLONE, CAST_TRIGGERED);
            }

            m_bIsInTransition = false;
        }
    }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        if (pSpellEntry->Id == SPELL_CLAW_RAGE && pTarget->GetTypeId() == TYPEID_PLAYER)
        {
            DoCastSpellIfCan(m_creature, SPELL_CLAW_RAGE_TRIGGER, CAST_TRIGGERED);
            m_uiLynxRushTimer += 8000;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_bIsInTransition)
            return;

        if (m_creature->GetHealthPercent() < m_uiHealthCheck)
        {
            m_uiHealthCheck -= 20;
            m_bIsInTransition = true;

            SetCombatMovement(false);
            m_creature->GetMotionMaster()->MovePoint(POINT_ID_CENTER, fZuljinMoveLoc[0], fZuljinMoveLoc[1], fZuljinMoveLoc[2]);

            // Despawn vortexes and remvoe the energy storm after eagle phase is complete
            if (m_uiPhase == PHASE_EAGLE)
            {
                m_creature->RemoveAurasDueToSpell(SPELL_ENERGY_STORM);
                DoDespawnVortexes();
            }

            // Reset threat
            DoResetThreat();

            // don't do this after troll phase
            if (m_uiPhase != PHASE_TROLL)
            {
                if (m_creature->HasAura(aZuljinPhases[m_uiPhase].uiSpiritSpellId))
                    m_creature->RemoveAurasDueToSpell(aZuljinPhases[m_uiPhase].uiSpiritSpellId);

                // drain spirit
                if (Creature* pSpirit = m_pInstance->GetSingleCreatureFromStorage(aZuljinPhases[m_uiPhase].uiSpiritId))
                {
                    pSpirit->InterruptNonMeleeSpells(false);
                    pSpirit->CastSpell(m_creature, SPELL_SPIRIT_DRAINED, false);
                    pSpirit->SetStandState(UNIT_STAND_STATE_DEAD);
                }
            }
        }

        switch (m_uiPhase)
        {
            case PHASE_TROLL:

                if (m_uiWhirlwindTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_WHIRLWIND) == CAST_OK)
                        m_uiWhirlwindTimer = urand(15000, 20000);
                }
                else
                    m_uiWhirlwindTimer -= uiDiff;

                if (m_uiGrievousThrowTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_GRIEVOUS_THROW) == CAST_OK)
                            m_uiGrievousThrowTimer = 10000;
                    }
                }
                else
                    m_uiGrievousThrowTimer -= uiDiff;

                break;
            case PHASE_BEAR:

                if (m_uiParalysisTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_CREEPING_PARALYSIS) == CAST_OK)
                        m_uiParalysisTimer = 27000;
                }
                else
                    m_uiParalysisTimer -= uiDiff;

                if (m_uiOverpowerTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_OVERPOWER) == CAST_OK)
                        m_uiOverpowerTimer = urand(12000, 16000);
                }
                else
                    m_uiOverpowerTimer -= uiDiff;

                break;
            case PHASE_EAGLE:
                // Nothing here; Spells casted just once at the beginning of the phase;
                break;
            case PHASE_LYNX:

                // Don't apply Claw Rage during Lynx Rush
                if (!m_uiLynxRushCount)
                {
                    if (m_uiClawRageTimer < uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_CLAW_RAGE, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_CLAW_RAGE) == CAST_OK)
                                m_uiClawRageTimer = urand(15000, 20000);
                        }
                    }
                    else
                        m_uiClawRageTimer -= uiDiff;
                }

                if (m_uiLynxRushTimer < uiDiff)
                {
                    if (!m_uiLynxRushCount)
                        DoCastSpellIfCan(m_creature, SPELL_LYNX_RUSH);
                    else
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            DoCastSpellIfCan(pTarget, SPELL_LYNX_RUSH_CHARGE);
                    }

                    ++m_uiLynxRushCount;

                    if (m_uiLynxRushCount == MAX_LYNX_RUSH)
                    {
                        m_uiLynxRushTimer = urand(20000, 25000);
                        m_uiLynxRushCount = 0;
                    }
                    else
                        m_uiLynxRushTimer = 400;
                }
                else
                    m_uiLynxRushTimer -= uiDiff;

                break;
            case PHASE_DRAGONHAWK:

                if (m_uiFlameWhirlTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FLAME_WHIRL) == CAST_OK)
                        m_uiFlameWhirlTimer = 15000;
                }
                else
                    m_uiFlameWhirlTimer -= uiDiff;

                if (m_uiPillarOfFireTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_PILLAR) == CAST_OK)
                        m_uiPillarOfFireTimer = urand(17000, 22000);
                }
                else
                    m_uiPillarOfFireTimer -= uiDiff;

                if (m_uiFlameBreathTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FLAME_BREATH) == CAST_OK)
                        m_uiFlameBreathTimer = 15000;
                }
                else
                    m_uiFlameBreathTimer -= uiDiff;

                break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_zuljin(Creature* pCreature)
{
    return new boss_zuljinAI(pCreature);
}

/*######
## npc_feather_vortex
######*/

struct npc_feather_vortexAI : public ScriptedAI
{
    npc_feather_vortexAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() override { }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        if (pSpellEntry->Id == SPELL_CYCLONE && pTarget->GetTypeId() == TYPEID_PLAYER && m_pInstance)
        {
            if (Creature* pZuljin = m_pInstance->GetSingleCreatureFromStorage(NPC_ZULJIN))
            {
                // Change target on player hit
                if (Unit* pTarget = pZuljin->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    m_creature->GetMotionMaster()->MoveFollow(pTarget, 0, 0);
            }
        }
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_feather_vortex(Creature* pCreature)
{
    return new npc_feather_vortexAI(pCreature);
}

void AddSC_boss_zuljin()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_zuljin";
    pNewScript->GetAI = &GetAI_boss_zuljin;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_feather_vortex";
    pNewScript->GetAI = &GetAI_npc_feather_vortex;
    pNewScript->RegisterSelf();
}

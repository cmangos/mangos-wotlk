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
SDName: Boss_Novos
SD%Complete: 90%
SDComment: Summon Timers are vague
SDCategory: Drak'Tharon Keep
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "draktharon_keep.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_AGGRO                       = -1600005,
    SAY_DEATH                       = -1600006,
    SAY_KILL                        = -1600007,
    SAY_ADDS                        = -1600008,
    SAY_BUBBLE_1                    = -1600009,
    SAY_BUBBLE_2                    = -1600010,

    EMOTE_ASSISTANCE                = -1600011,

    SPELL_ARCANE_FIELD              = 47346,
    SPELL_IMMUNITY                  = 34098,
    SPELL_SUMMON_MINIONS_H          = 59910,
    SPELL_FROSTBOLT                 = 49037,
    SPELL_FROSTBOLT_H               = 59855,
    SPELL_ARCANE_BLAST              = 49198,
    SPELL_ARCANE_BLAST_H            = 59909,
    SPELL_BLIZZARD                  = 49034,
    SPELL_BLIZZARD_H                = 59854,
    SPELL_TOUCH_OF_MISERY           = 50090,                // TODO - purpose of this spell (triggers SPELL_WRATH_OF_MISERY) unknown
    SPELL_WRATH_OF_MISERY           = 50089,
    SPELL_WRATH_OF_MISERY_H         = 59856,

    // SPELL_SUMMON_CRYSTAL_HANDLER    = 49179,             // Spell seems to be unused, perhaps only server-side, and especially no suitable positioned caster are found for this spell
    SPELL_SUMMON_FETID_TROLL_CORPSE = 49103,
    SPELL_SUMMON_HULKING_CORPSE     = 49104,
    SPELL_SUMMON_RISON_SHADOWCASTER = 49105,

    // Spells 'Crystal Handler Death' 47336, 55801, 55803, 55805 (defined in instance script)
};

// The Crystal Handlers are summoned around the two entrances of the room
static const float aHandlerSummonPos[2][3] =
{
    { -342.894836f, -727.016846f, 28.581081f},
    { -410.644653f, -731.826904f, 28.580412f}
};

/*######
## boss_novos
######*/

enum Phases
{
    PHASE_IDLE      = 0,
    PHASE_SHIELDED  = 1,
    PHASE_WAITING   = 2,
    PHASE_NORMAL    = 3,
};

struct boss_novosAI : public Scripted_NoMovementAI
{
    boss_novosAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_draktharon_keep*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_draktharon_keep* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSummonHandlerTimer;                          // TODO the summoning timers are weak
    uint32 m_uiSummonShadowcasterTimer;
    uint32 m_uiSummonFetidTrollTimer;
    uint32 m_uiSummonHulkingCorpseTimer;
    uint32 m_uiPhaseTimer;
    uint32 m_uiArcaneBlastTimer;
    uint32 m_uiBlizzardTimer;
    uint32 m_uiWrathTimer;

    uint8 m_uiSummonedHandlers;
    uint8 m_uiLostCrystals;
    Phases m_uiPhase;

    void Reset() override
    {
        m_uiSummonHandlerTimer = 25000;
        m_uiSummonShadowcasterTimer = 3000;
        m_uiSummonFetidTrollTimer = 10000;
        m_uiSummonHulkingCorpseTimer = 30000;
        m_uiPhaseTimer = 3000;
        m_uiArcaneBlastTimer = urand(6000, 8000);
        m_uiBlizzardTimer = urand(8000, 12000);
        m_uiWrathTimer = urand(12000, 15000);

        m_uiSummonedHandlers = 0;
        m_uiLostCrystals = 0;
        m_uiPhase = PHASE_IDLE;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
    }

    void LostOneCrystal()
    {
        ++m_uiLostCrystals;

        DoScriptText(urand(0, 1) ? SAY_BUBBLE_1 : SAY_BUBBLE_2, m_creature);

        if (m_uiLostCrystals == MAX_CRYSTALS)               // Enter Phase 2
            m_uiPhase = PHASE_WAITING;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // An Add reached the ground, if its z-pos is near the z pos of Novos
        if (pWho->GetEntry() == NPC_HULKING_CORPSE || pWho->GetEntry() == NPC_FETID_TROLL_CORPSE || pWho->GetEntry() == NPC_RISON_SHADOWCASTER)
        {
            // Add reached ground, and the failure has not yet been reported
            if (pWho->GetPositionZ() < m_creature->GetPositionZ() + 1.5f && m_pInstance && m_pInstance->GetData(TYPE_NOVOS) == IN_PROGRESS)
                m_pInstance->SetData(TYPE_NOVOS, SPECIAL);
            return;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_BLAST : SPELL_ARCANE_BLAST_H, CAST_TRIGGERED);

        DoCastSpellIfCan(m_creature, SPELL_IMMUNITY, CAST_TRIGGERED);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

        DoCastSpellIfCan(m_creature, SPELL_ARCANE_FIELD);
        m_uiPhase = PHASE_SHIELDED;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_NOVOS, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_NOVOS, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NOVOS, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_CRYSTAL_HANDLER:
            case NPC_ROTTED_TROLL_CORPSE:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pSummoned->AI()->AttackStart(pTarget);
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_CRYSTAL_HANDLER)
        {
            uint8 uiIndex = 0;
            if (m_pInstance)
            {
                if (Creature* pTarget = m_pInstance->GetNextCrystalTarget(pSummoned, uiIndex))
                    pSummoned->CastSpell(pTarget, aCrystalHandlerDeathSpells[uiIndex], TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        switch (m_uiPhase)
        {
            case PHASE_IDLE:
                // no actions during this phase
                return;
            case PHASE_SHIELDED:                            // Event Phase, only summoning of mobs
                if (m_uiSummonHandlerTimer < uiDiff)
                {
                    float fX, fY, fZ;
                    ++m_uiSummonedHandlers;
                    m_creature->GetRandomPoint(aHandlerSummonPos[m_uiSummonedHandlers % 2][0], aHandlerSummonPos[m_uiSummonedHandlers % 2][1], aHandlerSummonPos[m_uiSummonedHandlers % 2][2], 10.0f, fX, fY, fZ);
                    m_creature->SummonCreature(NPC_CRYSTAL_HANDLER, fX, fY, fZ, 0.0f, TEMPSPAWN_DEAD_DESPAWN, 0);

                    DoScriptText(SAY_ADDS, m_creature);
                    DoScriptText(EMOTE_ASSISTANCE, m_creature);

                    m_uiSummonHandlerTimer = 40000;
                }
                else
                    m_uiSummonHandlerTimer -= uiDiff;

                if (m_uiSummonShadowcasterTimer < uiDiff)
                {
                    if (m_pInstance)
                        if (Creature* pSummoner = m_pInstance->GetSummonDummy())
                            pSummoner->CastSpell(pSummoner, SPELL_SUMMON_RISON_SHADOWCASTER, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
                    m_uiSummonShadowcasterTimer = 25000;
                }
                else
                    m_uiSummonShadowcasterTimer -= uiDiff;

                if (m_uiSummonFetidTrollTimer < uiDiff)
                {
                    if (m_pInstance)
                        if (Creature* pSummoner = m_pInstance->GetSummonDummy())
                            pSummoner->CastSpell(pSummoner, SPELL_SUMMON_FETID_TROLL_CORPSE, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
                    m_uiSummonFetidTrollTimer = 5000;
                }
                else
                    m_uiSummonFetidTrollTimer -= uiDiff;

                if (m_uiSummonHulkingCorpseTimer < uiDiff)
                {
                    if (m_pInstance)
                        if (Creature* pSummoner = m_pInstance->GetSummonDummy())
                            pSummoner->CastSpell(pSummoner, SPELL_SUMMON_HULKING_CORPSE, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
                    m_uiSummonHulkingCorpseTimer = 30000;
                }
                else
                    m_uiSummonHulkingCorpseTimer -= uiDiff;

                break;

            case PHASE_WAITING:                             // Short delay between last destroyed crystal and entering combat
                if (m_uiPhaseTimer < uiDiff)
                {
                    m_uiPhase = PHASE_NORMAL;
                    // Remove Immunity and Shield Aura
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                    m_creature->RemoveAllAuras();

                    if (!m_bIsRegularMode)
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_MINIONS_H, CAST_INTERRUPT_PREVIOUS);
                    else
                        m_creature->InterruptNonMeleeSpells(true);
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                break;

            case PHASE_NORMAL:                              // Normal Phase, attack enemies
                if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
                    return;

                if (m_uiArcaneBlastTimer < uiDiff)
                {
                    // TODO - might be possible that this spell is only casted, when there is an enemy in range
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_BLAST : SPELL_ARCANE_BLAST_H) == CAST_OK)
                        m_uiArcaneBlastTimer = urand(7000, 9000);
                }
                else
                    m_uiArcaneBlastTimer -= uiDiff;

                if (m_uiBlizzardTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_BLIZZARD : SPELL_BLIZZARD_H) == CAST_OK)
                            m_uiBlizzardTimer = urand(9000, 13500);
                }
                else
                    m_uiBlizzardTimer -= uiDiff;

                if (m_uiWrathTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_WRATH_OF_MISERY : SPELL_WRATH_OF_MISERY_H) == CAST_OK)
                            m_uiWrathTimer = urand(12500, 17200);
                }
                else
                    m_uiWrathTimer -= uiDiff;

                if (!m_creature->IsNonMeleeSpellCasted(true))       // TODO Use this additional check, because might want to change the random target to be a target that is in LoS (which then is expensive)
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_FROSTBOLT : SPELL_FROSTBOLT_H);

                break;
        }
    }
};

UnitAI* GetAI_boss_novos(Creature* pCreature)
{
    return new boss_novosAI(pCreature);
}

// Small helper script to handle summoned adds for Novos
struct npc_crystal_channel_targetAI : public ScriptedAI
{
    npc_crystal_channel_targetAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_draktharon_keep*)pCreature->GetInstanceData();
    }

    instance_draktharon_keep* m_pInstance;

    void Reset() override {}
    void MoveInLineOfSight(Unit* /*pWho*/) override {}
    void AttackStart(Unit* /*pWho*/) override {}
    void UpdateAI(const uint32 /*uiDiff*/) override {}

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_HULKING_CORPSE || pSummoned->GetEntry() == NPC_FETID_TROLL_CORPSE || pSummoned->GetEntry() == NPC_RISON_SHADOWCASTER)
        {
            // Let them move down the stairs
            float fX, fY, fZ;

            // The end of the stairs is approximately at 1/3 of the way between summoning-position and novos, height of Novos
            if (Creature* pNovos = m_pInstance->GetSingleCreatureFromStorage(NPC_NOVOS))
            {
                m_creature->GetRandomPoint(0.70 * pNovos->GetPositionX() + 0.30 * pSummoned->GetPositionX(), 0.70 * pNovos->GetPositionY() + 0.30 * pSummoned->GetPositionY(), pNovos->GetPositionZ() + 1.5f, 4.0f, fX, fY, fZ);
                pSummoned->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
            }
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiPointId != 1 || uiMotionType != POINT_MOTION_TYPE || (pSummoned->GetEntry() != NPC_HULKING_CORPSE && pSummoned->GetEntry() != NPC_FETID_TROLL_CORPSE && pSummoned->GetEntry() != NPC_RISON_SHADOWCASTER))
            return;

        if (!pSummoned->IsInCombat() && m_pInstance)
        {
            if (Creature* pNovos = m_pInstance->GetSingleCreatureFromStorage(NPC_NOVOS))
            {
                if (Unit* pTarget = pNovos->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pSummoned->AI()->AttackStart(pTarget);
            }
        }
    }
};

UnitAI* GetAI_npc_crystal_channel_target(Creature* pCreature)
{
    return new npc_crystal_channel_targetAI(pCreature);
}

// Handling of the dummy auras of Crystal Handler Death spells, on remove the Crystal needs to be opened
bool EffectAuraDummy_npc_crystal_channel_target(const Aura* pAura, bool bApply)
{
    for (uint8 i = 0; i < MAX_CRYSTALS; ++i)
    {
        if (pAura->GetId() == aCrystalHandlerDeathSpells[i])
        {
            if (pAura->GetEffIndex() == EFFECT_INDEX_0 && !bApply)
            {
                if (Creature* pCreature = (Creature*)pAura->GetTarget())
                {
                    if (instance_draktharon_keep* pInstance = (instance_draktharon_keep*)pCreature->GetInstanceData())
                    {
                        if (pInstance->GetData(TYPE_NOVOS) == NOT_STARTED || pInstance->GetData(TYPE_NOVOS) == FAIL)
                            return true;

                        pInstance->DoHandleCrystal(i);

                        // Inform Novos about removed
                        if (Creature* pNovos = pInstance->GetSingleCreatureFromStorage(NPC_NOVOS))
                            if (boss_novosAI* pNovosAI = dynamic_cast<boss_novosAI*>(pNovos->AI()))
                                pNovosAI->LostOneCrystal();
                    }
                }
            }

            return true;
        }
    }

    return false;
}

void AddSC_boss_novos()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_novos";
    pNewScript->GetAI = &GetAI_boss_novos;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_crystal_channel_target";
    pNewScript->GetAI = &GetAI_npc_crystal_channel_target;
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_npc_crystal_channel_target;
    pNewScript->RegisterSelf();
}

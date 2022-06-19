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
SDName: boss_black_knight
SD%Complete: 100
SDComment:
SDCategory: Crusader Coliseum, Trial of the Champion
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "trial_of_the_champion.h"

enum
{
    SAY_PHASE_2                     = -1650066,
    SAY_PHASE_3                     = -1650067,
    SAY_KILL_1                      = -1650068,
    SAY_KILL_2                      = -1650069,
    SAY_DEATH                       = -1650070,

    // generic spells
    SPELL_KILL_CREDIT               = 68663,
    SPELL_FEIGN_DEATH               = 67691,                // triggers 67693
    SPELL_CLEAR_ALL_DEBUFFS         = 34098,
    SPELL_FULL_HEAL                 = 17683,
    SPELL_BLACK_KNIGHT_RES          = 67693,
    SPELL_RAISE_DEAD_ARELAS         = 67705,
    SPELL_RAISE_DEAD_JAEREN         = 67715,

    // phase 1
    SPELL_DEATHS_RESPITE            = 67745,
    SPELL_ICY_TOUCH                 = 67718,
    SPELL_OBLITERATE                = 67725,
    SPELL_PLAGUE_STRIKE             = 67724,

    // phase 2
    SPELL_ARMY_OF_THE_DEAD          = 67761,
    SPELL_DESECRATION               = 67778,
    SPELL_GHOUL_EXPLODE             = 67751,                // triggers 67729

    // phase 3
    SPELL_DEATHS_BITE               = 67808,
    SPELL_MARKED_FOR_DEATH          = 67823,

    // ghoul spells
    SPELL_CLAW                      = 67774,
    SPELL_EXPLODE                   = 67729,
    SPELL_LEAP                      = 67749,

    // risen zombies
    NPC_RISEN_JAEREN                = 35545,
    NPC_RISEN_ARELAS                = 35564,
    NPC_RISEN_CHAMPION              = 35590,

    // transform models
    MODEL_ID_SKELETON               = 29846,
    MODEL_ID_GHOST                  = 21300,

    // equipment id
    EQUIP_ID_SWORD                  = 40343,

    // phases
    PHASE_DEATH_KNIGHT              = 1,
    PHASE_SKELETON                  = 2,
    PHASE_GHOST                     = 3,
    PHASE_TRANSITION                = 4,
};

/*######
## boss_black_knight
######*/

struct boss_black_knightAI : public ScriptedAI
{
    boss_black_knightAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_trial_of_the_champion*)pCreature->GetInstanceData();
        Reset();
    }

    instance_trial_of_the_champion* m_pInstance;

    uint8 m_uiPhase;
    uint8 m_uiNextPhase;

    uint32 m_uiDeathsRespiteTimer;
    uint32 m_uiIcyTouchTimer;
    uint32 m_uiObliterateTimer;
    uint32 m_uiPlagueStrikeTimer;

    uint32 m_uiDesecrationTimer;
    uint32 m_uiGhoulExplodeTimer;

    uint32 m_uiDeathsBiteTimer;
    uint32 m_uiMarkedDeathTimer;

    ObjectGuid m_ghoulGuid;

    void Reset() override
    {
        m_uiPhase               = PHASE_DEATH_KNIGHT;

        m_uiDeathsRespiteTimer  = 10000;
        m_uiIcyTouchTimer       = urand(5000, 10000);
        m_uiObliterateTimer     = urand(10000, 15000);
        m_uiPlagueStrikeTimer   = 5000;

        m_uiDesecrationTimer    = 6000;
        m_uiGhoulExplodeTimer   = 10000;

        m_uiMarkedDeathTimer    = 0;
        m_uiDeathsBiteTimer     = 7000;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->SetDisplayId(m_creature->GetNativeDisplayId());
        SetEquipmentSlots(true);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            DoCastSpellIfCan(m_creature, m_pInstance->GetPlayerTeam() == ALLIANCE ? SPELL_RAISE_DEAD_ARELAS : SPELL_RAISE_DEAD_JAEREN);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BLACK_KNIGHT, FAIL);

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BLACK_KNIGHT, DONE);

        DoScriptText(SAY_DEATH, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_KILL_CREDIT, CAST_TRIGGERED);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // no aggro during the intro
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER))
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->AI()->AttackStart(pTarget);

        if (pSummoned->GetEntry() == NPC_RISEN_JAEREN || pSummoned->GetEntry() == NPC_RISEN_ARELAS)
            m_ghoulGuid = pSummoned->GetObjectGuid();
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (m_uiPhase == PHASE_GHOST)
            return;

        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (m_uiPhase == PHASE_TRANSITION)
                return;

            // start transition phase
            if (m_uiPhase == PHASE_DEATH_KNIGHT)
            {
                m_uiNextPhase = PHASE_SKELETON;

                // ghould explodes at the end of the round
                if (Creature* pGhoul = m_creature->GetMap()->GetCreature(m_ghoulGuid))
                {
                    pGhoul->InterruptNonMeleeSpells(true);
                    pGhoul->CastSpell(pGhoul, SPELL_EXPLODE, TRIGGERED_NONE);
                }
            }
            else if (m_uiPhase == PHASE_SKELETON)
                m_uiNextPhase = PHASE_GHOST;

            m_creature->InterruptNonMeleeSpells(true);
            m_creature->SetHealth(0);
            m_creature->StopMoving();
            m_creature->ClearComboPointHolders();
            m_creature->RemoveAllAurasOnDeath();
            m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
            m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->ClearAllReactives();
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

            DoCastSpellIfCan(m_creature, SPELL_FEIGN_DEATH, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_CLEAR_ALL_DEBUFFS, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_FULL_HEAL, CAST_TRIGGERED);
            m_uiPhase = PHASE_TRANSITION;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // finish transition
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());
            DoResetThreat();

            m_uiPhase = m_uiNextPhase;

            if (m_uiPhase == PHASE_SKELETON)
            {
                DoScriptText(SAY_PHASE_2, m_creature);
                m_creature->SetDisplayId(MODEL_ID_SKELETON);

                DoCastSpellIfCan(m_creature, SPELL_ARMY_OF_THE_DEAD);
            }
            else if (m_uiPhase == PHASE_GHOST)
            {
                DoScriptText(SAY_PHASE_3, m_creature);
                m_creature->SetDisplayId(MODEL_ID_GHOST);

                SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        switch (m_uiPhase)
        {
            case PHASE_DEATH_KNIGHT:

                if (m_uiDeathsRespiteTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_DEATHS_RESPITE) == CAST_OK)
                            m_uiDeathsRespiteTimer = 20000;
                    }
                }
                else
                    m_uiDeathsRespiteTimer -= uiDiff;

                if (m_uiIcyTouchTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_ICY_TOUCH, SELECT_FLAG_IN_MELEE_RANGE | SELECT_FLAG_IN_LOS))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_ICY_TOUCH) == CAST_OK)
                            m_uiIcyTouchTimer = urand(10000, 15000);
                    }
                }
                else
                    m_uiIcyTouchTimer -= uiDiff;

                if (m_uiObliterateTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_OBLITERATE) == CAST_OK)
                        m_uiObliterateTimer = urand(18000, 25000);
                }
                else
                    m_uiObliterateTimer -= uiDiff;

                if (m_uiPlagueStrikeTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_PLAGUE_STRIKE) == CAST_OK)
                        m_uiPlagueStrikeTimer = 10000;
                }
                else
                    m_uiPlagueStrikeTimer -= uiDiff;

                break;
            case PHASE_SKELETON:

                if (m_uiDesecrationTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DESECRATION) == CAST_OK)
                        m_uiDesecrationTimer = 6000;
                }
                else
                    m_uiDesecrationTimer -= uiDiff;

                if (m_uiGhoulExplodeTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_GHOUL_EXPLODE) == CAST_OK)
                        m_uiGhoulExplodeTimer = 12000;
                }
                else
                    m_uiGhoulExplodeTimer -= uiDiff;

                break;
            case PHASE_GHOST:

                if (m_uiDeathsBiteTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_DEATHS_BITE) == CAST_OK)
                        m_uiDeathsBiteTimer = 2000;
                }
                else
                    m_uiDeathsBiteTimer -= uiDiff;

                if (m_uiMarkedDeathTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_MARKED_FOR_DEATH) == CAST_OK)
                            m_uiMarkedDeathTimer = urand(10000, 15000);
                    }
                }
                else
                    m_uiMarkedDeathTimer -= uiDiff;
                break;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_black_knight(Creature* pCreature)
{
    return new boss_black_knightAI(pCreature);
}

bool EffectDummyCreature_black_knight_res(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_BLACK_KNIGHT_RES && uiEffIndex == EFFECT_INDEX_0)
    {
        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);
        return true;
    }

    return false;
}

/*######
## npc_black_knight_ghoul
######*/

struct npc_black_knight_ghoulAI : public ScriptedAI
{
    npc_black_knight_ghoulAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_trial_of_the_champion*)pCreature->GetInstanceData();
        Reset();
    }

    instance_trial_of_the_champion* m_pInstance;

    uint32 m_uiClawTimer;

    bool m_bExploded;

    void Reset() override
    {
        m_uiClawTimer = urand(3000, 6000);

        m_bExploded   = false;
    }

    void Aggro(Unit* pWho) override
    {
        if (m_creature->GetEntry() == NPC_RISEN_ARELAS || m_creature->GetEntry() == NPC_RISEN_JAEREN)
            DoCastSpellIfCan(pWho, SPELL_LEAP);
    }

    void SpellHitTarget(Unit* pUnit, const SpellEntry* pSpellEntry) override
    {
        if (pUnit->GetTypeId() != TYPEID_PLAYER)
            return;

        // set achiev failed
        if (pSpellEntry->Id == SPELL_EXPLODE && m_pInstance)
            m_pInstance->SetHadWorseAchievFailed();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // explode on low health
        if (!m_bExploded && m_creature->GetHealthPercent() < 15.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_EXPLODE, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                m_bExploded = true;
        }

        if (m_uiClawTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLAW) == CAST_OK)
                m_uiClawTimer = urand(7000, 14000);
        }
        else
            m_uiClawTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_black_knight_ghoul(Creature* pCreature)
{
    return new npc_black_knight_ghoulAI(pCreature);
}

/*######
## npc_black_knight_gryphon
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_black_knight_gryphonAI : public ScriptedAI
{
    npc_black_knight_gryphonAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_black_knight_gryphon(Creature* pCreature)
{
    return new npc_black_knight_gryphonAI(pCreature);
}

void AddSC_boss_black_knight()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_black_knight";
    pNewScript->GetAI = &GetAI_boss_black_knight;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_black_knight_res;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_black_knight_ghoul";
    pNewScript->GetAI = &GetAI_npc_black_knight_ghoul;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_black_knight_gryphon";
    pNewScript->GetAI = &GetAI_npc_black_knight_gryphon;
    pNewScript->RegisterSelf();
}

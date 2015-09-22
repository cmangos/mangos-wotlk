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
SDName: Boss_Reliquary_of_Souls
SD%Complete: 90
SDComment: Persistent Area Auras for each Essence (Aura of Suffering, Aura of Desire, Aura of Anger) requires core support.
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "black_temple.h"

enum
{
    // Sound'n'speech
    // Suffering
    SUFF_SAY_FREED                  = -1564047,
    SUFF_SAY_AGGRO                  = -1564048,
    SUFF_SAY_SLAY1                  = -1564049,
    SUFF_SAY_SLAY2                  = -1564050,
    SUFF_SAY_FRENZY                 = -1564051,
    SUFF_SAY_RECAP                  = -1564052,
    SUFF_SAY_AFTER                  = -1564053,
    EMOTE_BOSS_GENERIC_ENRAGED      = -1000006,

    // Desire
    DESI_SAY_FREED                  = -1564055,
    DESI_SAY_SLAY1                  = -1564056,
    DESI_SAY_SLAY2                  = -1564057,
    DESI_SAY_SLAY3                  = -1564058,
    DESI_SAY_SPEC                   = -1564059,
    DESI_SAY_RECAP                  = -1564060,
    DESI_SAY_AFTER                  = -1564061,

    // Anger
    ANGER_SAY_FREED                 = -1564062,
    ANGER_SAY_FREED2                = -1564063,
    ANGER_SAY_SLAY1                 = -1564064,
    ANGER_SAY_SLAY2                 = -1564065,
    ANGER_SAY_SPEC                  = -1564066,
    ANGER_SAY_BEFORE                = -1564067,
    ANGER_SAY_DEATH                 = -1564068,

    // Spells
    // Suffering
    SPELL_AURA_OF_SUFFERING         = 41292,
    SPELL_AURA_OF_SUFFERING_ARMOR   = 42017,
    SPELL_SUFFERING_PASSIVE         = 41296,
    SPELL_SUFFERING_PASSIVE_2       = 41623,
    SPELL_FRENZY                    = 41305,
    SPELL_SOUL_DRAIN                = 41303,

    // Desire
    SPELL_AURA_OF_DESIRE            = 41350,
    SPELL_RUNE_SHIELD               = 41431,
    SPELL_DEADEN                    = 41410,
    SPELL_SPIRIT_SHOCK              = 41426,

    // Anger
    SPELL_AURA_OF_ANGER             = 41337,
    SPELL_SEETHE                    = 41364,
    SPELL_SOUL_SCREAM               = 41545,
    SPELL_SPITE                     = 41376,                // triggers 41377 after 2 seconds

    // Generic
    SPELL_SUMMON_ESSENCE_SUFFERING  = 41488,
    SPELL_SUMMON_ESSENCE_DESIRE     = 41493,
    SPELL_SUMMON_ESSENCE_ANGER      = 41496,
    SPELL_SUMMON_ENSLAVED_SOUL      = 41537,

    // Soul spells
    SPELL_ENSLAVED_SOUL_PASSIVE     = 41535,
    SPELL_SOUL_RELEASE              = 41542,

    // Summons
    NPC_ESSENCE_SUFFERING           = 23418,
    NPC_ESSENCE_DESIRE              = 23419,
    NPC_ESSENCE_ANGER               = 23420,
    NPC_ENSLAVED_SOUL               = 23469,

    // Phases
    PHASE_0_NOT_BEGUN               = 0,
    PHASE_1_SUFFERING               = 1,
    PHASE_2_DESIRE                  = 2,
    PHASE_3_ANGER                   = 3,

    MAX_ENSLAVED_SOULS              = 36,
};

/*######
## boss_reliquary_of_souls
######*/

struct boss_reliquary_of_soulsAI : public Scripted_NoMovementAI
{
    boss_reliquary_of_soulsAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint8 m_uiPhase;
    uint8 m_uiSoulSummonedCount;
    uint8 m_uiSoulDeathCount;

    uint32 m_uiSummonEssenceTimer;
    uint32 m_uiSummonSoulTimer;
    uint32 m_uiAnimationTimer;
    uint32 m_uiAnimResetTimer;

    void Reset() override
    {
        m_uiPhase               = PHASE_0_NOT_BEGUN;
        m_uiSoulDeathCount      = 0;
        m_uiSoulSummonedCount   = 0;

        m_uiSummonSoulTimer     = 1000;
        m_uiAnimationTimer      = 0;
        m_uiAnimResetTimer      = 0;
        m_uiSummonEssenceTimer  = 0;

        // Reset animation
        m_creature->HandleEmote(EMOTE_STATE_NONE);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RELIQUIARY, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RELIQUIARY, FAIL);
    }

    void AttackStart(Unit* /*pWho*/) override { }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_uiPhase == PHASE_0_NOT_BEGUN && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster() &&
                m_creature->IsWithinDistInMap(pWho, m_creature->GetAttackDistance(pWho)) && m_creature->IsWithinLOSInMap(pWho))
        {
            // Start phase 1
            m_uiPhase = PHASE_1_SUFFERING;
            m_uiSummonEssenceTimer = 7000;
            m_uiAnimationTimer = 4000;

            // Set the player in combat with the boss
            pWho->SetInCombatWith(m_creature);
            m_creature->AddThreat(pWho);

            // Start animation
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

            if (m_pInstance)
                m_pInstance->SetData(TYPE_RELIQUIARY, IN_PROGRESS);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_ESSENCE_SUFFERING:
                DoScriptText(SUFF_SAY_FREED, pSummoned);
                break;
            case NPC_ESSENCE_DESIRE:
                DoScriptText(DESI_SAY_FREED, pSummoned);
                break;
            case NPC_ESSENCE_ANGER:
                DoScriptText(ANGER_SAY_FREED, pSummoned);
                break;
        }

        // All summons are set in combat
        pSummoned->SetInCombatWithZone();
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        // Self kill when the Essence of Anger is killed
        if (pSummoned->GetEntry() == NPC_ESSENCE_ANGER)
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // Switch to next phase when the essence gets back
        switch (pSummoned->GetEntry())
        {
            case NPC_ESSENCE_SUFFERING:
                DoScriptText(SUFF_SAY_AFTER, pSummoned);
                m_uiPhase = PHASE_2_DESIRE;;
                break;
            case NPC_ESSENCE_DESIRE:
                DoScriptText(DESI_SAY_AFTER, pSummoned);
                m_uiPhase = PHASE_3_ANGER;
                break;
        }

        // Despawn and set animation
        pSummoned->ForcedDespawn();

        m_uiSoulDeathCount = 0;
        m_uiSoulSummonedCount = 0;
        m_uiAnimResetTimer = 2000;
        // Reset animation
        m_creature->HandleEmote(EMOTE_ONESHOT_EMERGE);
    }

    // Wrapper to count the dead spirits
    void DoNotifySouldDead()
    {
        ++m_uiSoulDeathCount;

        // Prepare to summon the essence
        if (m_uiSoulDeathCount == MAX_ENSLAVED_SOULS)
        {
            m_uiSummonEssenceTimer = 7000;
            m_uiAnimationTimer     = 4000;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Animation for opening the Reliquary
        if (m_uiAnimationTimer)
        {
            if (m_uiAnimationTimer <= uiDiff)
            {
                m_creature->HandleEmote(EMOTE_ONESHOT_SUBMERGE);
                m_uiAnimationTimer = 0;
            }
            else
                m_uiAnimationTimer -= uiDiff;
        }

        // Animation for reset Reliquary
        if (m_uiAnimResetTimer)
        {
            if (m_uiAnimResetTimer <= uiDiff)
            {
                // Reset animation
                m_creature->HandleEmote(EMOTE_STATE_NONE);
                m_uiAnimResetTimer = 0;
            }
            else
                m_uiAnimResetTimer -= uiDiff;
        }

        // Summon the Essence on timer
        if (m_uiSummonEssenceTimer)
        {
            if (m_uiSummonEssenceTimer <= uiDiff)
            {
                uint32 uiSpellId = 0;
                switch (m_uiPhase)
                {
                    case PHASE_1_SUFFERING: uiSpellId = SPELL_SUMMON_ESSENCE_SUFFERING; break;
                    case PHASE_2_DESIRE:    uiSpellId = SPELL_SUMMON_ESSENCE_DESIRE;    break;
                    case PHASE_3_ANGER:     uiSpellId = SPELL_SUMMON_ESSENCE_ANGER;     break;
                }

                if (DoCastSpellIfCan(m_creature, uiSpellId) == CAST_OK)
                {
                    m_creature->HandleEmote(EMOTE_STATE_SUBMERGED);
                    m_uiSummonEssenceTimer = 0;
                }
            }
            else
                m_uiSummonEssenceTimer -= uiDiff;
        }

        // Summon Enslaved souls between the essence
        switch (m_uiPhase)
        {
            case PHASE_2_DESIRE:
            case PHASE_3_ANGER:

                if (m_uiSoulSummonedCount < MAX_ENSLAVED_SOULS)
                {
                    if (m_uiSummonSoulTimer < uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_ENSLAVED_SOUL) == CAST_OK)
                        {
                            ++m_uiSoulSummonedCount;
                            m_uiSummonSoulTimer = 500;
                        }
                    }
                    else
                        m_uiSummonSoulTimer -= uiDiff;
                }

                break;
        }
    }
};

/*######
## essence_base_AI
######*/

struct essence_base_AI : public ScriptedAI
{
    essence_base_AI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsPhaseFinished = false;
    }

    ScriptedInstance* m_pInstance;

    bool m_bIsPhaseFinished;

    virtual void OnPhaseFinished() {}

    void JustReachedHome() override
    {
        // Reset encounter and despawn Essence
        if (m_pInstance)
        {
            if (Creature* pReliquary = m_pInstance->GetSingleCreatureFromStorage(NPC_RELIQUARY_OF_SOULS))
                pReliquary->AI()->EnterEvadeMode();
        }

        m_creature->ForcedDespawn();
    }

    void DamageTaken(Unit* /*pKiller*/, uint32& uiDamage) override
    {
        if (uiDamage < m_creature->GetHealth())
            return;

        // Prevent glitch if in fake death
        if (m_bIsPhaseFinished)
        {
            uiDamage = 0;
            return;
        }

        uiDamage = 0;

        m_creature->InterruptNonMeleeSpells(true);
        m_creature->SetHealth(0);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->ClearAllReactives();
        m_creature->GetMotionMaster()->Clear();

        if (!m_pInstance)
            return;

        // Move to home position
        if (Creature* pReliquary = m_pInstance->GetSingleCreatureFromStorage(NPC_RELIQUARY_OF_SOULS))
            m_creature->GetMotionMaster()->MovePoint(1, pReliquary->GetPositionX(), pReliquary->GetPositionY(), pReliquary->GetPositionZ());

        m_bIsPhaseFinished = true;

        OnPhaseFinished();
    }
};

/*######
## boss_essence_of_suffering
######*/

struct boss_essence_of_sufferingAI : public essence_base_AI
{
    boss_essence_of_sufferingAI(Creature* pCreature) : essence_base_AI(pCreature) { Reset(); }

    uint32 m_uiEnrageTimer;
    uint32 m_uiSoulDrainTimer;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_AURA_OF_SUFFERING, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_SUFFERING_PASSIVE, CAST_TRIGGERED);

        m_uiEnrageTimer     = 45000;
        m_uiSoulDrainTimer  = 20000;
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SUFF_SAY_SLAY1 : SUFF_SAY_SLAY2, m_creature);
    }

    void OnPhaseFinished()
    {
        DoScriptText(SUFF_SAY_RECAP, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEnrageTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
            {
                DoScriptText(EMOTE_BOSS_GENERIC_ENRAGED, m_creature);
                DoScriptText(SUFF_SAY_FRENZY, m_creature);
                m_uiEnrageTimer = 45000;
            }
        }
        else
            m_uiEnrageTimer -= uiDiff;

        if (m_uiSoulDrainTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SOUL_DRAIN) == CAST_OK)
                m_uiSoulDrainTimer = urand(45000, 60000);
        }
        else
            m_uiSoulDrainTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_essence_of_desire
######*/

struct boss_essence_of_desireAI : public essence_base_AI
{
    boss_essence_of_desireAI(Creature* pCreature) : essence_base_AI(pCreature) { Reset(); }

    uint32 m_uiRuneShieldTimer;
    uint32 m_uiDeadenTimer;
    uint32 m_uiSoulShockTimer;

    void Reset() override
    {
        m_uiRuneShieldTimer = urand(10000, 15000);
        m_uiDeadenTimer     = 15000;
        m_uiSoulShockTimer  = urand(5000, 10000);

        DoCastSpellIfCan(m_creature, SPELL_AURA_OF_DESIRE);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(DESI_SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(DESI_SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(DESI_SAY_SLAY3, m_creature); break;
        }
    }

    void OnPhaseFinished()
    {
        DoScriptText(DESI_SAY_RECAP, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiRuneShieldTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_RUNE_SHIELD) == CAST_OK)
                m_uiRuneShieldTimer = 15000;
        }
        else
            m_uiRuneShieldTimer -= uiDiff;

        if (m_uiDeadenTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEADEN) == CAST_OK)
            {
                DoScriptText(DESI_SAY_SPEC, m_creature);
                m_uiDeadenTimer = 30000;
            }
        }
        else
            m_uiDeadenTimer -= uiDiff;

        if (m_uiSoulShockTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SPIRIT_SHOCK) == CAST_OK)
                m_uiSoulShockTimer = urand(5000, 10000);
        }
        else
            m_uiSoulShockTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_essence_of_anger
######*/

struct boss_essence_of_angerAI : public ScriptedAI
{
    boss_essence_of_angerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiSeetheTimer;
    uint32 m_uiSoulScreamTimer;
    uint32 m_uiSpiteTimer;

    void Reset() override
    {
        m_uiSeetheTimer      = 5000;
        m_uiSoulScreamTimer  = 10000;
        m_uiSpiteTimer       = 20000;

        DoCastSpellIfCan(m_creature, SPELL_AURA_OF_ANGER);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? ANGER_SAY_SLAY1 : ANGER_SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(ANGER_SAY_DEATH, m_creature);
    }

    void JustReachedHome() override
    {
        // Reset encounter and despawn Essence
        if (m_pInstance)
        {
            if (Creature* pReliquary = m_pInstance->GetSingleCreatureFromStorage(NPC_RELIQUARY_OF_SOULS))
                pReliquary->AI()->EnterEvadeMode();
        }

        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSeetheTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SEETHE) == CAST_OK)
                m_uiSeetheTimer = urand(20000, 30000);
        }
        else
            m_uiSeetheTimer -= uiDiff;

        if (m_uiSoulScreamTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SOUL_SCREAM) == CAST_OK)
                m_uiSoulScreamTimer = 10000;
        }
        else
            m_uiSoulScreamTimer -= uiDiff;

        if (m_uiSpiteTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SPITE) == CAST_OK)
            {
                DoScriptText(ANGER_SAY_BEFORE, m_creature);
                m_uiSpiteTimer = 20000;
            }
        }
        else
            m_uiSpiteTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_enslaved_soul
######*/

struct npc_enslaved_soulAI : public ScriptedAI
{
    npc_enslaved_soulAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_ENSLAVED_SOUL_PASSIVE);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_SOUL_RELEASE, CAST_TRIGGERED);

        // Notify the main boss about the spirit death. Needs to be done here, because the spirit is summoned with triggered spell
        if (m_pInstance)
        {
            if (Creature* pReliquary = m_pInstance->GetSingleCreatureFromStorage(NPC_RELIQUARY_OF_SOULS))
            {
                if (boss_reliquary_of_soulsAI* pBossAI = dynamic_cast<boss_reliquary_of_soulsAI*>(pReliquary->AI()))
                    pBossAI->DoNotifySouldDead();
            }
        }
    }

    void JustReachedHome() override
    {
        // Reset encounter and despawn the spirit
        if (m_pInstance)
        {
            if (Creature* pReliquary = m_pInstance->GetSingleCreatureFromStorage(NPC_RELIQUARY_OF_SOULS))
                pReliquary->AI()->EnterEvadeMode();
        }

        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 /*uiDiff*/) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_reliquary_of_souls(Creature* pCreature)
{
    return new boss_reliquary_of_soulsAI(pCreature);
}

CreatureAI* GetAI_boss_essence_of_suffering(Creature* pCreature)
{
    return new boss_essence_of_sufferingAI(pCreature);
}

CreatureAI* GetAI_boss_essence_of_desire(Creature* pCreature)
{
    return new boss_essence_of_desireAI(pCreature);
}

CreatureAI* GetAI_boss_essence_of_anger(Creature* pCreature)
{
    return new boss_essence_of_angerAI(pCreature);
}

CreatureAI* GetAI_npc_enslaved_soul(Creature* pCreature)
{
    return new npc_enslaved_soulAI(pCreature);
}

void AddSC_boss_reliquary_of_souls()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_reliquary_of_souls";
    pNewScript->GetAI = &GetAI_boss_reliquary_of_souls;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_essence_of_suffering";
    pNewScript->GetAI = &GetAI_boss_essence_of_suffering;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_essence_of_desire";
    pNewScript->GetAI = &GetAI_boss_essence_of_desire;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_essence_of_anger";
    pNewScript->GetAI = &GetAI_boss_essence_of_anger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_enslaved_soul";
    pNewScript->GetAI = &GetAI_npc_enslaved_soul;
    pNewScript->RegisterSelf();
}

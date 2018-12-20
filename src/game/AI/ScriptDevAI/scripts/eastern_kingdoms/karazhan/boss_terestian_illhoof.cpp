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
SDName: Boss_Terestian_Illhoof
SD%Complete: 100
SDComment:
SDCategory: Karazhan
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "karazhan.h"
#include "AI/ScriptDevAI/base/TimerAI.h"

enum
{
    SAY_SLAY1                   = -1532065,
    SAY_SLAY2                   = -1532066,
    SAY_DEATH                   = -1532067,
    SAY_AGGRO                   = -1532068,
    SAY_SACRIFICE1              = -1532069,
    SAY_SACRIFICE2              = -1532070,
    SAY_SUMMON1                 = -1532071,
    SAY_SUMMON2                 = -1532072,
    SAY_KILREK_DEATH            = -1532136,

    // spells
    SPELL_SUMMON_DEMONCHAINS    = 30120,                    // Summons demonic chains that maintain the ritual of sacrifice.
    SPELL_SHADOW_BOLT           = 30055,                    // Hurls a bolt of dark magic at an enemy, inflicting Shadow damage.
    SPELL_SACRIFICE             = 30115,                    // Teleports and adds the debuff
    SPELL_BERSERK               = 32965,                    // Increases attack speed by 75%. Periodically casts Shadow Bolt Volley.
    SPELL_SUMMON_IMP            = 30066,                    // Summons Kil'rek
    SPELL_FIENDISH_PORTAL       = 30171,                    // Opens portal and summons Fiendish Portal, 2 sec cast
    SPELL_FIENDISH_PORTAL_1     = 30179,                    // Opens portal and summons Fiendish Portal, instant cast

    // Chains spells
    SPELL_DEMON_CHAINS          = 30206,                    // Instant - Visual Effect

    // Portal spells
    SPELL_SUMMON_FIENDISH_IMP   = 30184,

    // Kilrek
    SPELL_BROKEN_PACT           = 30065,                    // All damage taken increased by 25%, TODO: Sends AI event for Imp Timer

    // summoned npcs
    NPC_DEMONCHAINS             = 17248,
    NPC_FIENDISHIMP             = 17267,
    NPC_PORTAL                  = 17265,
    NPC_KILREK                  = 17229
};

enum IllhoofActions
{
    ILLHOOF_ACTION_SUMMON_KILREK,
    ILLHOOF_ACTION_SACRIFICE,
    ILLHOOF_ACTION_SHADOWBOLT,
    ILLHOOF_ACTION_SUMMON,
    ILLHOOF_ACTION_BERSERK,
    ILLHOOF_ACTION_MAX,
};

struct boss_terestianAI : public ScriptedAI, public CombatTimerAI
{
    boss_terestianAI(Creature* pCreature) : ScriptedAI(pCreature), CombatTimerAI(ILLHOOF_ACTION_MAX)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        AddCombatAction(ILLHOOF_ACTION_SUMMON_KILREK, 0);
        AddCombatAction(ILLHOOF_ACTION_SACRIFICE, 0);
        AddCombatAction(ILLHOOF_ACTION_SHADOWBOLT, 0);
        AddCombatAction(ILLHOOF_ACTION_SUMMON, 0);
        AddCombatAction(ILLHOOF_ACTION_BERSERK, 0);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    ObjectGuid m_kilrekGuid;
    ObjectGuid m_sacrificeGuid;
    
    bool m_bSummonedPortals;

    void Reset() override
    {
        for (uint32 i = 0; i < ILLHOOF_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(ILLHOOF_ACTION_SUMMON_KILREK, GetInitialActionTimer(ILLHOOF_ACTION_SUMMON_KILREK));
        ResetTimer(ILLHOOF_ACTION_SACRIFICE, GetInitialActionTimer(ILLHOOF_ACTION_SACRIFICE));
        ResetTimer(ILLHOOF_ACTION_SHADOWBOLT, GetInitialActionTimer(ILLHOOF_ACTION_SHADOWBOLT));
        ResetTimer(ILLHOOF_ACTION_SUMMON, GetInitialActionTimer(ILLHOOF_ACTION_SUMMON));
        ResetTimer(ILLHOOF_ACTION_BERSERK, GetInitialActionTimer(ILLHOOF_ACTION_BERSERK));

        DisableCombatAction(ILLHOOF_ACTION_SUMMON_KILREK);

        m_bSummonedPortals = false;
    }

    uint32 GetInitialActionTimer(uint32 id)
    {
        switch (id)
        {
            case ILLHOOF_ACTION_SUMMON_KILREK: return 0;
            case ILLHOOF_ACTION_SACRIFICE: return urand(30000, 35000);
            case ILLHOOF_ACTION_SHADOWBOLT: return urand(5000, 7000);
            case ILLHOOF_ACTION_SUMMON: return 10000;
            case ILLHOOF_ACTION_BERSERK: return 10 * MINUTE * IN_MILLISECONDS;
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(uint32 id)
    {
        switch (id)
        {
            case ILLHOOF_ACTION_SUMMON_KILREK: return 30000;
            case ILLHOOF_ACTION_SACRIFICE: return urand(40000, 50000);
            case ILLHOOF_ACTION_SHADOWBOLT: return urand(6000, 16000);
            default: return 0; // never occurs but for compiler
        }
    }

    void ExecuteActions() override
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < ILLHOOF_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case ILLHOOF_ACTION_SUMMON_KILREK:
                    {
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_IMP);
                        SetActionReadyStatus(i, false); // Reset when imp dies
                        continue;
                    }
                    case ILLHOOF_ACTION_SACRIFICE:
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_SACRIFICE, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_SACRIFICE) == CAST_OK)
                            {
                                DoCastSpellIfCan(m_creature, SPELL_SUMMON_DEMONCHAINS, CAST_TRIGGERED);
                                DoScriptText(urand(0, 1) ? SAY_SACRIFICE1 : SAY_SACRIFICE2, m_creature);
                                m_sacrificeGuid = pTarget->GetObjectGuid();
                            }
                        }
                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case ILLHOOF_ACTION_SHADOWBOLT:
                    {
                        DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT);
                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case ILLHOOF_ACTION_SUMMON:
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_FIENDISH_PORTAL) == CAST_OK)
                            DoScriptText(urand(0, 1) ? SAY_SUMMON1 : SAY_SUMMON2, m_creature);
                        SetActionReadyStatus(i, false); // once per fight
                        continue;
                    }
                    case ILLHOOF_ACTION_BERSERK:
                    {
                        DoCastSpellIfCan(m_creature, SPELL_BERSERK);
                        SetActionReadyStatus(i, false); // once per fight
                        continue;
                    }
                }
            }
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_TERESTIAN, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustReachedHome() override
    {
        Creature* kilrek = m_creature->GetMap()->GetAnyTypeCreature(m_kilrekGuid);
        if (!kilrek || !kilrek->isAlive())
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_IMP);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_TERESTIAN, FAIL);
    }

    void JustRespawned() override
    {
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_IMP);
        Reset();
    }
    
    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_PORTAL:
                if (!m_bSummonedPortals)
                {
                    m_bSummonedPortals = true;
                    DoCastSpellIfCan(m_creature, SPELL_FIENDISH_PORTAL_1, CAST_TRIGGERED);
                }
                break;
            case NPC_KILREK:
                m_kilrekGuid = pSummoned->GetObjectGuid();
                if (m_creature->isInCombat())
                    pSummoned->SetInCombatWithZone();
                break;
            case NPC_DEMONCHAINS:
                pSummoned->CastSpell(pSummoned, SPELL_DEMON_CHAINS, TRIGGERED_NONE);
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_KILREK:
                DoScriptText(SAY_KILREK_DEATH, pSummoned, pSummoned);
                pSummoned->CastSpell(m_creature, SPELL_BROKEN_PACT, TRIGGERED_OLD_TRIGGERED);
                ResetTimer(ILLHOOF_ACTION_SUMMON_KILREK, GetSubsequentActionTimer(ILLHOOF_ACTION_SUMMON_KILREK));
                break;
            case NPC_DEMONCHAINS:
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_sacrificeGuid))
                    pPlayer->RemoveAurasDueToSpell(SPELL_SACRIFICE);
                break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_TERESTIAN, DONE);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        UpdateTimers(uiDiff, m_creature->isInCombat());
        ExecuteActions();

        DoMeleeAttackIfReady();
    }
};

struct npc_fiendish_portalAI : public ScriptedAI
{
    npc_fiendish_portalAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiSummonTimer;

    void Reset() override
    {
        m_uiSummonTimer = 5000;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->ApplySpellImmune(nullptr, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
        pSummoned->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiSummonTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_FIENDISH_IMP) == CAST_OK)
                m_uiSummonTimer = 5000;
        }
        else
            m_uiSummonTimer -= uiDiff;
    }
};

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct mob_demon_chainAI : public Scripted_NoMovementAI
{
    mob_demon_chainAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_boss_terestian_illhoof(Creature* pCreature)
{
    return new boss_terestianAI(pCreature);
}

UnitAI* GetAI_npc_fiendish_portal(Creature* pCreature)
{
    return new npc_fiendish_portalAI(pCreature);
}

UnitAI* GetAI_mob_demon_chain(Creature* pCreature)
{
    return new mob_demon_chainAI(pCreature);
}

void AddSC_boss_terestian_illhoof()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_terestian_illhoof";
    pNewScript->GetAI = &GetAI_boss_terestian_illhoof;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fiendish_portal";
    pNewScript->GetAI = &GetAI_npc_fiendish_portal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_demon_chain";
    pNewScript->GetAI = &GetAI_mob_demon_chain;
    pNewScript->RegisterSelf();
}

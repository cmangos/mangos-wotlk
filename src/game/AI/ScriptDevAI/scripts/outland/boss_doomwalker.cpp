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
SDName: Boss_Doomwalker
SD%Complete: 100
SDComment:
SDCategory: Shadowmoon Valley
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"

enum
{
    NPC_DREADLORD               = 21166,
    NPC_ILLIDARI_RAVAGER        = 22857,
    NPC_SHADOWHOOF_ASSASSIN     = 22858,
    NPC_SHADOWHOOF_SUMMONER     = 22859,
    NPC_ILLIDARI_SUCCUBUS       = 22860,

    SAY_AGGRO                   = -1000159,
    SAY_EARTHQUAKE_1            = -1000160,
    SAY_EARTHQUAKE_2            = -1000161,
    SAY_OVERRUN_1               = -1000162,
    SAY_OVERRUN_2               = -1000163,
    SAY_SLAY_1                  = -1000164,
    SAY_SLAY_2                  = -1000165,
    SAY_SLAY_3                  = -1000166,
    SAY_DEATH                   = -1000167,

    EMOTE_FRENZY                = -1000002,

    SPELL_EARTHQUAKE            = 32686,
    SPELL_CRUSH_ARMOR           = 33661,
    SPELL_LIGHTNING_WRATH       = 33665,
    SPELL_OVERRUN               = 32636,
    SPELL_OVERRUN_TRIGGER_SPAWN = 32632,
    SPELL_OVERRUN_DAMAGE        = 32637,
    SPELL_ENRAGE                = 33653,
    SPELL_MARK_OF_DEATH_PLAYER  = 37128,
    SPELL_MARK_OF_DEATH_AURA    = 37125,        // triggers 37131 on target if it has aura 37128

    SPELL_EARTHQUAKE_VISUAL     = 41966,        // used in OOC sequence for RP

    COUNT_OVERRUN               = 5,
    POINT_OVERRUN               = 1,
};

struct boss_doomwalkerAI : public ScriptedAI
{
    boss_doomwalkerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiChainTimer;
    uint32 m_uiOverrunTimer;
    uint32 m_uiQuakeTimer;
    uint32 m_uiArmorTimer;

    uint32 m_overrunTargets;
    uint32 m_overrunExecTimer;
    GuidVector m_overrunCandidates;
    SelectAttackingTargetParams m_overrunParams;

    bool m_bHasEnrage;

    void Reset() override
    {
        m_uiArmorTimer     = urand(5000, 13000);
        m_uiChainTimer     = urand(10000, 30000);
        m_uiQuakeTimer     = urand(25000, 35000);
        m_uiOverrunTimer   = urand(30000, 45000);

        m_bHasEnrage       = false;

        m_overrunTargets = 0;
        m_overrunCandidates.clear();

        m_overrunParams.range.minRange = 0;
        m_overrunParams.range.maxRange = 30;

        m_creature->RemoveAurasDueToSpell(SPELL_MARK_OF_DEATH_AURA);

        m_creature->SetWalk(true);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        pVictim->CastSpell(pVictim, SPELL_MARK_OF_DEATH_PLAYER, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());

        if (urand(0, 4))
            return;

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void MovementInform(uint32 movementType, uint32 data) override
    {
        if (movementType == POINT_MOTION_TYPE && data == POINT_OVERRUN)
        {
            if (Unit* victim = m_creature->GetVictim())
            {
                m_creature->MeleeAttackStart(victim);
                m_creature->SetTarget(victim);
                DoStartMovement(victim);
            }
            SetCombatScriptStatus(false);
            m_creature->RemoveAurasDueToSpell(SPELL_OVERRUN);
            DoResetThreat();
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        m_creature->MeleeAttackStop(m_creature->GetVictim());
        m_creature->SetTarget(nullptr);
        SetCombatScriptStatus(true);
        float x, y, z;
        summoned->GetNearPoint(m_creature, x, y, z, 0.f, 0.f, summoned->GetAngle(m_creature));
        m_creature->GetMotionMaster()->MovePoint(POINT_OVERRUN, x, y, z, FORCED_MOVEMENT_RUN);
        m_overrunExecTimer = 250;
    }

    void JustRespawned() override
    {
        Reset();

        std::list<Creature*> npcList;
        GetCreatureListWithEntryInGrid(npcList, m_creature, NPC_DREADLORD, 200.0f);
        GetCreatureListWithEntryInGrid(npcList, m_creature, NPC_ILLIDARI_RAVAGER, 200.0f);
        GetCreatureListWithEntryInGrid(npcList, m_creature, NPC_SHADOWHOOF_ASSASSIN, 200.0f);
        GetCreatureListWithEntryInGrid(npcList, m_creature, NPC_SHADOWHOOF_SUMMONER, 200.0f);
        GetCreatureListWithEntryInGrid(npcList, m_creature, NPC_ILLIDARI_SUCCUBUS, 200.0f);

        for (Creature* creature : npcList)
            creature->Suicide();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        m_creature->RemoveAurasDueToSpell(SPELL_MARK_OF_DEATH_AURA);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_MARK_OF_DEATH_AURA, CAST_TRIGGERED);
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (GetCombatScriptStatus())
        {
            if (m_overrunExecTimer <= uiDiff)
            {
                m_creature->CastSpell(nullptr, SPELL_OVERRUN_DAMAGE, TRIGGERED_NONE);
                m_overrunExecTimer = 250;
            }
            else
                m_overrunExecTimer -= uiDiff;
            return;
        }

        // Spell Enrage, when hp <= 20% gain enrage
        if (m_creature->GetHealthPercent() <= 20.0f && !m_bHasEnrage)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
            {
                DoScriptText(EMOTE_FRENZY, m_creature);
                m_bHasEnrage = true;
            }
        }

        // Spell Overrun
        if (m_uiOverrunTimer < uiDiff)
        {
            std::vector<Unit*> targets;
            m_creature->SelectAttackingTargets(targets, ATTACKING_TARGET_ALL_SUITABLE, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_RANGE_AOE_RANGE, m_overrunParams);
            while (targets.size() > COUNT_OVERRUN)
                targets.erase(targets.begin() + urand(0, targets.size() - 1));

            if (!targets.empty() && DoCastSpellIfCan(nullptr, SPELL_OVERRUN) == CAST_OK)
            {
                float angle = m_creature->GetAngle(targets[0]);
                m_creature->SetFacingTo(angle);
                m_creature->SetOrientation(angle);
                m_creature->CastSpell(nullptr, SPELL_OVERRUN_TRIGGER_SPAWN, TRIGGERED_OLD_TRIGGERED); // shouldnt be sent to client
                DoScriptText(urand(0, 1) ? SAY_OVERRUN_1 : SAY_OVERRUN_2, m_creature);
                m_uiOverrunTimer = urand(25000, 40000);
            }
        }
        else
            m_uiOverrunTimer -= uiDiff;

        // Spell Earthquake
        if (m_uiQuakeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_EARTHQUAKE) == CAST_OK)
            {
                DoScriptText(urand(0, 1) ? SAY_EARTHQUAKE_1 : SAY_EARTHQUAKE_2, m_creature);
                m_uiQuakeTimer = urand(30000, 55000);
            }
        }
        else
            m_uiQuakeTimer -= uiDiff;

        // Spell Chain Lightning
        if (m_uiChainTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER);
            if (!pTarget)
                pTarget = m_creature->GetVictim();

            if (pTarget)
            {
                if (DoCastSpellIfCan(pTarget, SPELL_LIGHTNING_WRATH) == CAST_OK)
                    m_uiChainTimer = urand(7000, 27000);
            }
        }
        else
            m_uiChainTimer -= uiDiff;

        // Spell Sunder Armor
        if (m_uiArmorTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CRUSH_ARMOR) == CAST_OK)
                m_uiArmorTimer = urand(10000, 25000);
        }
        else
            m_uiArmorTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_doomwalker(Creature* pCreature)
{
    return new boss_doomwalkerAI(pCreature);
}

void AddSC_boss_doomwalker()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_doomwalker";
    pNewScript->GetAI = &GetAI_boss_doomwalker;
    pNewScript->RegisterSelf();
}

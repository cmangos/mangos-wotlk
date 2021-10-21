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
SDName: Boss_NexusPrince_Shaffar
SD%Complete: 100
SDComment: ToDo: move the Ethereal Beacon script to eventAI
SDCategory: Auchindoun, Mana Tombs
EndScriptData */

/* ContentData
boss_nexusprince_shaffar
mob_ethereal_beacon
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/TimerAI.h"

enum
{
    SPELL_C_C_D               = 35465,
    SPELL_PURPLE_BANISH_STATE = 32566,

    FACTION_HOSTILE = 14,

    NPC_YOR = 22930,

    SPELL_STOMP = 36405,
    SPELL_DOUBLE_BREATH = 38361,

    NPC_STASIS_PRISONER_DUNGEON = 22927,
};

enum YorActions
{
    YOR_ACTION_STOMP,
    YOR_ACTION_DOUBLE_BREATH,
    YOR_COMBAT_ACTION_MAX,
    YOR_ATTACK,
};

enum PrisonerActions
{
    PRISONER_ATTACK,
    PRISONER_TALK,
    PRISONER_CAST,
    PRISONER_CREDIT,
};

struct boss_yorAI : public ScriptedAI
{
    boss_yorAI(Creature* creature) : ScriptedAI(creature, YOR_COMBAT_ACTION_MAX)
    {
        AddCustomAction(YOR_ATTACK, true, [&]
        {
            m_creature->SetImmuneToNPC(false);
            m_creature->SetImmuneToPlayer(false);
            m_creature->setFaction(FACTION_HOSTILE);
            if (Player* player = m_creature->GetMap()->GetPlayer(m_playerGuid))
                AttackStart(player);
        });
        AddCombatAction(YOR_ACTION_STOMP, 0u);
        AddCombatAction(YOR_ACTION_DOUBLE_BREATH, 0u);
        JustRespawned();
    }

    void JustRespawned() override
    {
        DoCastSpellIfCan(nullptr, SPELL_C_C_D, (CAST_AURA_NOT_PRESENT | CAST_TRIGGERED));
        DoCastSpellIfCan(nullptr, SPELL_PURPLE_BANISH_STATE, (CAST_AURA_NOT_PRESENT | CAST_TRIGGERED));
        if (m_stasisGuid)
            if (GameObject* stasis = m_creature->GetMap()->GetGameObject(m_stasisGuid))
                stasis->ResetDoorOrButton();
    }

    ObjectGuid m_playerGuid;
    ObjectGuid m_stasisGuid;

    void StartEvent(Player* player, GameObject* go)
    {
        m_playerGuid = player->GetObjectGuid();
        if (go)
            m_stasisGuid = go->GetObjectGuid();
        m_creature->RemoveAurasDueToSpell(SPELL_C_C_D);
        m_creature->RemoveAurasDueToSpell(SPELL_PURPLE_BANISH_STATE);
        m_creature->UpdateEntry(NPC_YOR);
        ResetTimer(YOR_ATTACK, 2000);
    }

    void Reset() override
    {
        for (uint32 i = 0; i < YOR_COMBAT_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(YOR_ACTION_STOMP,         GetInitialActionTimer(YOR_ACTION_STOMP));
        ResetTimer(YOR_ACTION_DOUBLE_BREATH, GetInitialActionTimer(YOR_ACTION_DOUBLE_BREATH));
    }

    uint32 GetInitialActionTimer(YorActions id)
    {
        switch (id)
        {
            case YOR_ACTION_STOMP: return 15000;
            case YOR_ACTION_DOUBLE_BREATH: return 8000;
            default: return 0;
        }
    }

    uint32 GetSubsequentActionTimer(YorActions id)
    {
        switch (id)
        {
            case YOR_ACTION_STOMP: return 20000;
            case YOR_ACTION_DOUBLE_BREATH: return urand(9000, 14000);
            default: return 0;
        }
    }

    void ExecuteActions() override
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < YOR_COMBAT_ACTION_MAX; ++i)
        {
            if (!GetActionReadyStatus(i))
                continue;

            switch (i)
            {
                case YOR_ACTION_STOMP:
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, SPELL_STOMP, SELECT_FLAG_USE_EFFECT_RADIUS))
                    {
                        if (DoCastSpellIfCan(nullptr, SPELL_STOMP) == CAST_OK)
                        {
                            SetActionReadyStatus(i, false);
                            ResetTimer(i, GetSubsequentActionTimer(YorActions(i)));
                            return;
                        }
                    }
                    continue;
                case YOR_ACTION_DOUBLE_BREATH:
                    if (DoCastSpellIfCan(nullptr, SPELL_DOUBLE_BREATH) == CAST_OK)
                    {
                        SetActionReadyStatus(i, false);
                        ResetTimer(i, GetSubsequentActionTimer(YorActions(i)));
                        return;
                    }
                    continue;
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff, m_creature->IsInCombat());

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        ExecuteActions();

        DoMeleeAttackIfReady();
    }
};

bool GOUse_go_stasis_chamber_shaffar(Player* player, GameObject* go)
{
    if (Creature* prisoner = GetClosestCreatureWithEntry(go, NPC_STASIS_PRISONER_DUNGEON, 2.f))
    {
        boss_yorAI* ai = static_cast<boss_yorAI*>(prisoner->AI());
        ai->StartEvent(player, go);
    }

    return false;
}

enum
{
    SPELL_BLUE_BANISH_STATE = 39650,

    SAY_RELEASED_1 = -1557036,
    SAY_RELEASED_2 = -1557037,

    NPC_AMBASSADOR_PAXIVI = 22928,

    SPELL_QID_10977 = 39660,
};

struct npc_ethereum_prisoner_dungeonAI : public ScriptedAI
{
    npc_ethereum_prisoner_dungeonAI(Creature* creature) : ScriptedAI(creature, 0)
    {
        AddCustomAction(PRISONER_CREDIT, true, [&]
        {
            if (Player* player = m_creature->GetMap()->GetPlayer(m_playerGuid))
                player->CastSpell(nullptr, SPELL_QID_10977, TRIGGERED_NONE);
        });
        AddCustomAction(PRISONER_TALK, true, [&]
        {
            if (Player* player = m_creature->GetMap()->GetPlayer(m_playerGuid))
                DoScriptText(SAY_RELEASED_2, m_creature, player);
            m_creature->ForcedDespawn(600000);
        });
        JustRespawned();
    }

    void JustRespawned() override
    {
        DoCastSpellIfCan(nullptr, SPELL_C_C_D, (CAST_AURA_NOT_PRESENT | CAST_TRIGGERED));
        DoCastSpellIfCan(nullptr, SPELL_BLUE_BANISH_STATE, (CAST_AURA_NOT_PRESENT | CAST_TRIGGERED));
        if (m_stasisGuid)
            if (GameObject* stasis = m_creature->GetMap()->GetGameObject(m_stasisGuid))
                stasis->ResetDoorOrButton();
    }

    ObjectGuid m_playerGuid;
    ObjectGuid m_stasisGuid;

    void StartEvent(Player* player, GameObject* go)
    {
        m_playerGuid = player->GetObjectGuid();
        if (go)
            m_stasisGuid = go->GetObjectGuid();
        m_creature->RemoveAurasDueToSpell(SPELL_C_C_D);
        m_creature->RemoveAurasDueToSpell(SPELL_BLUE_BANISH_STATE);
        m_creature->UpdateEntry(NPC_AMBASSADOR_PAXIVI);
        float angle = m_unit->GetAngle(player);
        m_creature->SetFacingTo(angle);
        m_creature->SetOrientation(angle);
        DoScriptText(SAY_RELEASED_1, m_creature, player);
        ResetTimer(PRISONER_CREDIT, 500);
        ResetTimer(PRISONER_TALK, 8500);
    }

    void Reset() override
    {

    }

    void ExecuteActions() override {}

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff, m_creature->IsInCombat());
    }
};

// guid scripting, if there ever was any
UnitAI* GetAInpc_ethereum_prisoner_dungeon(Creature* creature)
{
    if (creature->GetPositionX() > 0)
        return new npc_ethereum_prisoner_dungeonAI(creature);
    else
        return new boss_yorAI(creature);
}

bool GOUse_go_stasis_chamber_mana_tombs(Player* player, GameObject* go)
{
    if (Creature* prisoner = GetClosestCreatureWithEntry(go, NPC_STASIS_PRISONER_DUNGEON, 2.f))
    {
        npc_ethereum_prisoner_dungeonAI* ai = static_cast<npc_ethereum_prisoner_dungeonAI*>(prisoner->AI());
        ai->StartEvent(player, go);
    }

    return false;
}

void AddSC_boss_yor()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "go_stasis_chamber_mana_tombs";
    pNewScript->pGOUse = &GOUse_go_stasis_chamber_mana_tombs;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ethereum_prisoner_dungeon";
    pNewScript->GetAI = &GetAInpc_ethereum_prisoner_dungeon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_stasis_chamber_shaffar";
    pNewScript->pGOUse = &GOUse_go_stasis_chamber_shaffar;
    pNewScript->RegisterSelf();
}
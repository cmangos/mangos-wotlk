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
SDName: Hyjal
SD%Complete: 80
SDComment:
SDCategory: Caverns of Time, Mount Hyjal
EndScriptData */

/* ContentData
npc_jaina_proudmoore
npc_thrall
npc_tyrande_whisperwind
npc_building_trigger
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "hyjalAI.h"
#include "Spells/Scripts/SpellScript.h"

enum JainaActions
{
    JAINA_20 = HYJAL_AI_ACTION_MAX,
    JAINA_BLIZZARD,
    JAINA_PYROBLAST,
    JAINA_SUMMON_ELEMENTALS,
    JAINA_ACTION_MAX,
};

struct npc_jaina_proudmooreAI : public hyjalAI
{
    npc_jaina_proudmooreAI(Creature* creature) : hyjalAI(creature, JAINA_ACTION_MAX)
    {
        AddTimerlessCombatAction(JAINA_20, true);
        AddCombatAction(JAINA_BLIZZARD, 15000, 35000);
        AddCombatAction(JAINA_PYROBLAST, 2000, 9000);
        AddCombatAction(JAINA_SUMMON_ELEMENTALS, 15000, 45000);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case JAINA_20:
                if (m_creature->GetHealthPercent() <= 20.f)
                {
                    SetActionReadyStatus(action, false);
                    DoScriptText(SAY_CALL_FOR_HELP_EMOTE, m_creature);
                    DoCallForHelp(30.f);
                }
                break;
            case JAINA_BLIZZARD:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (DoCastSpellIfCan(target, SPELL_BLIZZARD) == CAST_OK)
                        ResetCombatAction(action, urand(15000, 35000));
                break;
            case JAINA_PYROBLAST:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (DoCastSpellIfCan(target, SPELL_PYROBLAST) == CAST_OK)
                        ResetCombatAction(action, urand(2000, 9000));
                break;
            case JAINA_SUMMON_ELEMENTALS:
                if (m_creature->CountGuardiansWithEntry(NPC_WATER_ELEMENTAL) == 0)
                    if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_ELEMENTALS) == CAST_OK)
                        ResetCombatAction(action, urand(15000, 45000));
                break;
            default: hyjalAI::ExecuteAction(action); break;
        }
    }
};

bool GossipSelect_npc_jaina_proudmoore(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 action)
{
    if (instance_mount_hyjal* instance = (instance_mount_hyjal*)creature->GetInstanceData())
    {
        if (hyjalAI* jainaAI = dynamic_cast<hyjalAI*>(creature->AI()))
        {
            switch (action)
            {
                case 100:
                    if (!jainaAI->IsEventStarted())
                    {
                        instance->StartEvent(JAINA_FIRST_BOSS);
                        jainaAI->EventStarted();
                        player->PrepareGossipMenu(creature, 7556);
                        player->SendPreparedGossip(creature);
                    }
                    break;
                case 101:
                    if (!jainaAI->IsEventStarted())
                    {
                        instance->StartEvent(JAINA_SECOND_BOSS);
                        jainaAI->EventStarted();
                        player->PrepareGossipMenu(creature, 7689);
                        player->SendPreparedGossip(creature);
                    }
                    break;
                case 102:
                    if (instance->GetData(TYPE_AZGALOR) != DONE) // Jaina has the same gossip menu when spawned in orc base, but nothing should happen when selecting her gossip menu options
                    {
                        creature->SetActiveObjectState(true); // Set active object to prevent issues if players go out of range after talking to her (could lead to ores not spawning)
                        instance->StartEvent(JAINA_WIN);
                        creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP); // ToDo: Jaina should not lose gossip flag, but also should not stop movement when a player opens her gossip window. Currently we have no way to achieve this.
                    }
                    player->CLOSE_GOSSIP_MENU();
                    break;
            }
        }
    }
    return false;
}

enum ThrallActions
{
    THRALL_FERAL_SPIRITS = HYJAL_AI_ACTION_MAX,
    THRALL_CHAIN_LIGHTNING,
    THRALL_ACTION_MAX,
};

struct npc_thrallAI : public hyjalAI
{
    npc_thrallAI(Creature* creature) : hyjalAI(creature, THRALL_ACTION_MAX)
    {
        AddCombatAction(THRALL_FERAL_SPIRITS, 60000u);
        AddCombatAction(THRALL_CHAIN_LIGHTNING, 6000, 8000);
    }

    GuidVector m_feralSpirits;

    void JustSummoned(Creature* creature)
    {
        if (creature->GetEntry() == NPC_FERAL_SPIRIT)
            m_feralSpirits.push_back(creature->GetObjectGuid());
    }

    bool CanUseFeralSpiritsAgain()
    {
        uint32 count = 0;

        for (auto spiritGuid : m_feralSpirits)
            if (Creature* spirit = m_creature->GetMap()->GetAnyTypeCreature(spiritGuid))
                if (spirit->IsAlive())
                    count++;

        if (count == 0)
            m_feralSpirits.clear();

        return count == 0;
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case THRALL_CHAIN_LIGHTNING:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (DoCastSpellIfCan(target, SPELL_CHAIN_LIGHTNING) == CAST_OK)
                        ResetCombatAction(action, urand(13000, 19000));
                break;
            case THRALL_FERAL_SPIRITS:
                if (CanUseFeralSpiritsAgain())
                    if (DoCastSpellIfCan(nullptr, SPELL_FERAL_SPIRIT) == CAST_OK)
                        ResetCombatAction(action, urand(15000, 45000));
                break;
            default: hyjalAI::ExecuteAction(action); break;
        }
    }
};

bool GossipSelect_npc_thrall(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 action)
{
    if (instance_mount_hyjal* instance = (instance_mount_hyjal*)creature->GetInstanceData())
    {
        if (hyjalAI* thrallAI = dynamic_cast<hyjalAI*>(creature->AI()))
        {
            switch (action)
            {
            case 100:
                if (!thrallAI->IsEventStarted())
                {
                    instance->StartEvent(THRALL_FIRST_BOSS);
                    thrallAI->EventStarted();
                    player->PrepareGossipMenu(creature, 7584);
                    player->SendPreparedGossip(creature);
                }
                break;
            case 101:
                if (!thrallAI->IsEventStarted())
                {
                    instance->StartEvent(THRALL_SECOND_BOSS);
                    thrallAI->EventStarted();
                    player->PrepareGossipMenu(creature, 7701);
                    player->SendPreparedGossip(creature);
                }
                break;
            case 102:
                creature->SetActiveObjectState(true); // Set active object to prevent issues if players go out of range after talking to him (could lead to ores not spawning)
                instance->StartEvent(THRALL_WIN);
                creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP); // ToDo: Thrall should not lose gossip flag, but nothing should happen when choosing his options
                player->CLOSE_GOSSIP_MENU();
                break;
            }
        }
    }
    return true;
}

struct npc_building_triggerAI : public ScriptedAI
{
    npc_building_triggerAI(Creature* creature) : ScriptedAI(creature){}

    void Reset() override 
    {
        m_creature->AI()->SetReactState(REACT_PASSIVE);
        SetDeathPrevention(true);
        DoCastSpellIfCan(nullptr, SPELL_SUPER_INVIS, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    ObjectGuid m_firstAttackerGuid;

    void DamageTaken(Unit* dealer, uint32& /*damage*/, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        if (dealer && (dealer->GetEntry() == NPC_GHOUL || dealer->GetEntry() == NPC_GARGO))
        {
            if (!m_firstAttackerGuid)
            {
                m_firstAttackerGuid = dealer->GetObjectGuid();
                dealer->setFaction(FACTION_SPAR);
                m_creature->setFaction(FACTION_SPAR_BUDDY);

                // only 1 attacker allowed at a time
                if (instance_mount_hyjal* instance = static_cast<instance_mount_hyjal*>(m_creature->GetInstanceData()))
                {
                    for (uint32 i = 0; i < MAX_BASE; i++)
                    {
                        for (auto overrunSpawnGuid : instance->GetOverrunSpawns(i))
                        {
                            if (Creature* overrunSpawn = m_creature->GetMap()->GetAnyTypeCreature(overrunSpawnGuid))
                            {
                                if (overrunSpawn->GetObjectGuid() == m_firstAttackerGuid)
                                    continue;
                                if (!overrunSpawn->HasTarget(m_creature->GetObjectGuid()) || !overrunSpawn->HasTarget())
                                    continue;
                                overrunSpawn->AI()->EnterEvadeMode();
                            }
                        }
                    }
                }
            }
        }
    }
};

enum TyrandeActions
{
    TYRANDE_STARFALL = HYJAL_AI_ACTION_MAX,
    TYRANDE_TRUESHOT_AURA,
    TYRANDE_ACTION_MAX,
};

struct npc_tyrande_whisperwindAI : public hyjalAI
{
    npc_tyrande_whisperwindAI(Creature* creature) : hyjalAI(creature, TYRANDE_ACTION_MAX)
    {
        AddCombatAction(TYRANDE_STARFALL, 60000, 70000);
        AddCombatAction(TYRANDE_TRUESHOT_AURA, 4000u);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case TYRANDE_STARFALL:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_STARFALL) == CAST_OK)
                    ResetCombatAction(action, urand(60000, 70000));
                break;
            case TYRANDE_TRUESHOT_AURA:
                if (DoCastSpellIfCan(nullptr, SPELL_TRUESHOT_AURA) == CAST_OK)
                    ResetCombatAction(action, 4000u);
                break;
            default: hyjalAI::ExecuteAction(action); break;
        }
    }
};

struct RaiseDeadHyjal : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        if (strict)
        {
            float radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(spell->m_spellInfo->rangeIndex));
            UnitList tempUnitList;
            GameObjectList tempGOList;
            return spell->CheckScriptTargeting(EFFECT_INDEX_0, 1, radius, TARGET_LOCATION_SCRIPT_NEAR_CASTER, tempUnitList, tempGOList);
        }
        return SPELL_CAST_OK;
    }
};

void AddSC_hyjal()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_jaina_proudmoore";
    pNewScript->GetAI = &GetNewAIInstance<npc_jaina_proudmooreAI>;
    pNewScript->pGossipSelect = &GossipSelect_npc_jaina_proudmoore;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_thrall";
    pNewScript->GetAI = &GetNewAIInstance<npc_thrallAI>;
    pNewScript->pGossipSelect = &GossipSelect_npc_thrall;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tyrande_whisperwind";
    pNewScript->GetAI = &GetNewAIInstance<npc_tyrande_whisperwindAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_building_trigger";
    pNewScript->GetAI = &GetNewAIInstance<npc_building_triggerAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<RaiseDeadHyjal>("spell_raise_dead_hyjal");
}

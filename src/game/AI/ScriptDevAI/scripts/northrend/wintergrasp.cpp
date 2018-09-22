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
SDName: Wintergrasp
SD%Complete: 0
SDComment: Wintergrasp battlefield related scripts
SDCategory: Wintergrasp
EndScriptData */

/* ContentData
npc_spirit_guide_wintergrasp
go_vehicle_teleporter
event_go_tower_destroy
EndContentData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "Battlefield/BattlefieldWG.h"

enum
{
    // Wintergrasp spirit healer (copy from battleground)
    SPELL_SPIRIT_HEAL_CHANNEL       = 22011,                // Spirit Heal Channel

    SPELL_SPIRIT_HEAL               = 22012,                // Spirit Heal
    SPELL_SPIRIT_HEAL_MANA          = 44535,                // in battlegrounds player get this no-mana-cost-buff

    SPELL_WAITING_TO_RESURRECT      = 2584,                 // players who cancel this aura don't want a resurrection


    // vehicle teleporter
    MAX_WINTERGRASP_VEHICLES        = 4,

    SPELL_TELEPORT_VEHICLE          = 49759,                // teleport all vehicles in range

    NPC_WORLD_TRIGGER               = 23472,                // used as teleport target
};

static const uint32 aWintergraspVehicles[MAX_WINTERGRASP_VEHICLES] = { NPC_WINTERGRASP_CATAPULT, NPC_WINTERGRASP_DEMOLISHER, NPC_WINTERGRASP_SIEGE_ENGINE_A, NPC_WINTERGRASP_SIEGE_ENGINE_H };

/*###############
## npc_spirit_guide_wintergrasp
################*/

struct npc_spirit_guide_wintergraspAI : public ScriptedAI
{
    npc_spirit_guide_wintergraspAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    Battlefield* m_battlefield;

    uint32 m_uiHealChannelTimer;

    void Reset() override
    {
        m_battlefield = nullptr;
        m_uiHealChannelTimer = 1000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiHealChannelTimer < uiDiff)
        {
            m_uiHealChannelTimer = 1000;

            if (!m_battlefield)
            {
                // attempt to initialize the opvp
                OutdoorPvP* outdoorPvP = sOutdoorPvPMgr.GetScript(m_creature->GetZoneId());
                if (outdoorPvP && outdoorPvP->IsBattlefield())
                    m_battlefield = (Battlefield*)outdoorPvP;
                else
                    return;
            }

            if (m_battlefield->GetBattlefieldStatus() == BF_STATUS_COOLDOWN)
                return;

            // auto cast the whole time this spell
            if (!m_creature->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                m_creature->CastSpell(m_creature, SPELL_SPIRIT_HEAL_CHANNEL, TRIGGERED_NONE);
        }
        else
            m_uiHealChannelTimer -= uiDiff;
    }

    void SpellHitTarget(Unit* pUnit, const SpellEntry* pSpellEntry) override
    {
        if (pSpellEntry->Id == SPELL_SPIRIT_HEAL && pUnit->GetTypeId() == TYPEID_PLAYER
                && pUnit->HasAura(SPELL_WAITING_TO_RESURRECT))
            pUnit->CastSpell(pUnit, SPELL_SPIRIT_HEAL_MANA, TRIGGERED_OLD_TRIGGERED);
    }
};

UnitAI* GetAI_npc_spirit_guide_wintergrasp(Creature* pCreature)
{
    return new npc_spirit_guide_wintergraspAI(pCreature);
}

/*###############
## go_vehicle_teleporter
################*/

struct go_vehicle_teleporter : public GameObjectAI
{
    go_vehicle_teleporter(GameObject* go) : GameObjectAI(go)
    {
        m_battlefield = nullptr;
        m_gameobject = go;
        m_uiTeleportTimer = 1000;
    }

    Battlefield* m_battlefield;
    GameObject* m_gameobject;

    uint32 m_uiTeleportTimer;

    // Return the teleport destination target
    Unit* GetTeleportTarget(Creature* source)
    {
        // The destination trigger is between 64 and 69 away; no more than 70
        CreatureList lTriggersInRange;
        GetCreatureListWithEntryInGrid(lTriggersInRange, m_gameobject, NPC_WORLD_TRIGGER, 70.0f);

        if (lTriggersInRange.empty())
            return nullptr;

        lTriggersInRange.sort(ObjectDistanceOrderReversed(source));
        return lTriggersInRange.front();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiTeleportTimer < uiDiff)
        {
            m_uiTeleportTimer = 1000;

            if (!m_gameobject)
                return;

            if (!m_battlefield)
            {
                // attempt to initialize the opvp
                OutdoorPvP* outdoorPvP = sOutdoorPvPMgr.GetScript(m_gameobject->GetZoneId());
                if (outdoorPvP && outdoorPvP->IsBattlefield())
                    m_battlefield = (Battlefield*)outdoorPvP;
                else
                    return;
            }

            if (m_battlefield->GetBattlefieldStatus() == BF_STATUS_COOLDOWN)
                return;

            for (uint8 i = 0; i < MAX_WINTERGRASP_VEHICLES; ++i)
            {
                // Note: the object has a very large range; might need to be corrected in DB
                if (Creature* vehicle = GetClosestCreatureWithEntry(m_gameobject, aWintergraspVehicles[i], 0.1f))
                {
                    if (vehicle->HasAura(SPELL_TELEPORT_VEHICLE))
                        continue;

                    // validate vehicle ownership against the flag it carries
                    if ((m_battlefield->GetDefender() == ALLIANCE && vehicle->HasAura(SPELL_ALLIANCE_FLAG)) ||
                        (m_battlefield->GetDefender() == HORDE && vehicle->HasAura(SPELL_HORDE_FLAG)))
                    {
                        // teleport to the trigger which is furthest away
                        if (Unit* target = GetTeleportTarget(vehicle))
                            vehicle->CastSpell(target, SPELL_TELEPORT_VEHICLE, TRIGGERED_OLD_TRIGGERED);
                    }
                }
            }
        }
        else
            m_uiTeleportTimer -= uiDiff;
    }
};


GameObjectAI* GetAI_go_vehicle_teleporter(GameObject* go)
{
    return new go_vehicle_teleporter(go);
}

/*###############
## event_go_tower_destroy
################*/

bool ProcessEventId_event_go_tower_destroy(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (bIsStart && pSource->GetTypeId() == TYPEID_GAMEOBJECT)
    {
        // despawn nearby flags and cannons
        GameObject* tower = (GameObject*)pSource;
        if (!pSource)
            return false;

        CreatureList lCannonsInRange;
        GetCreatureListWithEntryInGrid(lCannonsInRange, tower, NPC_WINTERGRASP_TOWER_CANNON, 50.0f);
        for (auto cannon : lCannonsInRange)
            cannon->ForcedDespawn();

        GameObjectList lAllianceBanners;
        GetGameObjectListWithEntryInGrid(lAllianceBanners, tower, GO_WINTERGRASP_ALLIANCE_BANNER, 50.0f);
        for (auto banner : lAllianceBanners)
            banner->SetLootState(GO_JUST_DEACTIVATED);

        GameObjectList lHordeBanners;
        GetGameObjectListWithEntryInGrid(lHordeBanners, tower, GO_WINTERGRASP_HORDE_BANNER, 50.0f);
        for (auto banner : lHordeBanners)
            banner->SetLootState(GO_JUST_DEACTIVATED);

    }

    // always return false to allow battlefield script to process
    return false;
}


void AddSC_wintergrasp()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_spirit_guide_wintergrasp";
    pNewScript->GetAI = &GetAI_npc_spirit_guide_wintergrasp;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_vehicle_teleporter";
    pNewScript->GetGameObjectAI = &GetAI_go_vehicle_teleporter;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_go_tower_destroy";
    pNewScript->pProcessEventId = &ProcessEventId_event_go_tower_destroy;
    pNewScript->RegisterSelf();
}

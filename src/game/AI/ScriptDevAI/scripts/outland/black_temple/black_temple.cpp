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
SDName: Instance_Black_Temple
SD%Complete: 100
SDComment: Instance Data Scripts and functions to acquire mobs and set encounter status for use in various Black Temple Scripts
SDCategory: Black Temple
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "black_temple.h"

/* Black Temple encounters:
0 - High Warlord Naj'entus event
1 - Supremus Event
2 - Shade of Akama Event
3 - Teron Gorefiend Event
4 - Gurtogg Bloodboil Event
5 - Reliquary Of Souls Event
6 - Mother Shahraz Event
7 - Illidari Council Event
8 - Illidan Stormrage Event
*/

instance_black_temple::instance_black_temple(Map* pMap) : ScriptedInstance(pMap), m_akamaIllidanSequence(false)
{
    Initialize();
};

void instance_black_temple::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_black_temple::OnPlayerEnter(Player* /*pPlayer*/)
{
    DoTeleportAkamaIfCan();
}

void instance_black_temple::OnPlayerResurrect(Player* player)
{
    if (GetData(TYPE_RELIQUIARY) == IN_PROGRESS)
        if (Creature* trigger = GetSingleCreatureFromStorage(NPC_RELIQUARY_COMBAT_TRIGGER))
            trigger->EngageInCombatWith(player);
}

bool instance_black_temple::IsEncounterInProgress() const
{
    for (uint32 i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            return true;
    }

    return false;
}

void instance_black_temple::OnCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_SPIRIT_OF_OLUM:
        case NPC_SPIRIT_OF_UDALO:
            // Use only the summoned versions
            if (!creature->IsTemporarySummon())
                break;
        case NPC_AKAMA:
        case NPC_ILLIDAN_STORMRAGE:
        case NPC_MAIEV_SHADOWSONG:
        case NPC_AKAMA_SHADE:
        case NPC_SHADE_OF_AKAMA:
        case NPC_RELIQUARY_OF_SOULS:
        case NPC_GATHIOS:
        case NPC_ZEREVOR:
        case NPC_LADY_MALANDE:
        case NPC_VERAS:
        case NPC_ILLIDARI_COUNCIL:
        case NPC_COUNCIL_VOICE:
        case NPC_ILLIDAN_DOOR_TRIGGER:
        case NPC_BLACK_TEMPLE_TRIGGER:
        case NPC_TERON_GOREFIEND:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;
        case NPC_SHADOWY_CONSTRUCT:
            m_shadowyConstructs.push_back(creature->GetObjectGuid());
            break;
        case NPC_ASH_CHANNELER:
            m_channelersGuidVector.push_back(creature->GetObjectGuid());
            break;
        case NPC_CREATURE_GENERATOR:
            m_creatureGeneratorGuidVector.push_back(creature->GetObjectGuid());
            break;
        case NPC_GLAIVE_TARGET:
            m_vGlaiveTargetGuidVector.push_back(creature->GetObjectGuid());
            break;
        case NPC_ASHTONGUE_BATTLELORD:
        case NPC_ASHTONGUE_MYSTIC:
        case NPC_ASHTONGUE_STORMCALLER:
        case NPC_ASHTONGUE_PRIMALIST:
        case NPC_STORM_FURY:
        case NPC_ASHTONGUE_FERAL_SPIRIT:
        case NPC_ASHTONGUE_STALKER:
            m_ashtongue.push_back(creature->GetObjectGuid());
            if (m_auiEncounter[TYPE_SHADE] == DONE)
                creature->setFaction(FACTION_ASHTONGUE_FRIENDLY);
            break;
        case NPC_VENGEFUL_SPIRIT:
            if (Creature* teron = GetSingleCreatureFromStorage(NPC_TERON_GOREFIEND))
                teron->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, creature, teron);
            break;
        case NPC_SKY_STALKER:
        case NPC_WIND_REAVER:
            if (creature->GetPositionZ() > 160.0f)
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            break;
        case NPC_RELIQUARY_COMBAT_TRIGGER:
            creature->GetCombatManager().SetLeashingDisable(true);
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;
        case NPC_ANGERED_SOUL_FRAGMENT:
        case NPC_SUFFERING_SOUL_FRAGMENT:
        case NPC_HUNGERING_SOUL_FRAGMENT:
            m_soulFragments.push_back(creature->GetObjectGuid());
            break;
        case NPC_ENSLAVED_SOUL:
            m_enslavedSouls.push_back(creature->GetObjectGuid());
            break;
        case NPC_WORLD_TRIGGER:
        {
            float z = creature->GetPositionZ();
            if (z > 355.f && z < 365.f)
                m_illidanTriggersLower.push_back(creature->GetObjectGuid());
            else if (z > 365.f)
                m_illidanTriggersUpper.push_back(creature->GetObjectGuid());
            break;
        }
        case NPC_BLADE_OF_AZZINOTH:
            GuidVector& blades = m_npcEntryGuidCollection[creature->GetEntry()];
            if (blades.size() == 2) // new attempt
                blades.clear();
            blades.push_back(creature->GetObjectGuid());
            break;
    }

    switch (creature->GetEntry())
    {
        case NPC_FLAME_CRASH:
        case NPC_BLADE_OF_AZZINOTH:
        case NPC_ILLIDAN_TARGET:
        case NPC_SHADOW_DEMON:
        case NPC_ILLIDARI_ELITE:
        case NPC_PARASITIC_SHADOWFIEND:
        case NPC_FLAME_OF_AZZINOTH:
        case NPC_DEMON_FIRE:
        case NPC_BLAZE:
        case NPC_CAGE_TRAP_DISTURB_TRIGGER:
            m_illidanSpawns.push_back(creature->GetObjectGuid());
            break;
    }
}

void instance_black_temple::OnCreatureRespawn(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_ASHTONGUE_BATTLELORD:
        case NPC_ASHTONGUE_MYSTIC:
        case NPC_ASHTONGUE_STORMCALLER:
        case NPC_ASHTONGUE_PRIMALIST:
        case NPC_STORM_FURY:
        case NPC_ASHTONGUE_FERAL_SPIRIT:
        case NPC_ASHTONGUE_STALKER:
            if (m_auiEncounter[TYPE_SHADE] == DONE)
                creature->setFaction(FACTION_ASHTONGUE_FRIENDLY);
            break;
        case NPC_ANGERED_SOUL_FRAGMENT:
        case NPC_SUFFERING_SOUL_FRAGMENT:
        case NPC_HUNGERING_SOUL_FRAGMENT:
            if (GetData(NPC_RELIQUARY_OF_SOULS) == DONE)
            {
                creature->SetRespawnDelay(time(nullptr) + 7 * DAY);
                creature->ForcedDespawn();
            }
            break;
    }
}

void instance_black_temple::OnObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case GO_NAJENTUS_GATE:                              // Gate past Naj'entus (at the entrance to Supermoose's courtyards)
            if (m_auiEncounter[TYPE_NAJENTUS] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SUPREMUS_DOORS:                             // Main Temple Doors - right past Supermoose (Supremus)
            if (m_auiEncounter[TYPE_SUPREMUS] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SHADE_OF_AKAMA:                             // Door close during encounter
        case GO_GOREFIEND_DOOR:                             // Door close during encounter
            break;
        case GO_GURTOGG_DOOR:                               // Door opens after encounter
            if (m_auiEncounter[TYPE_BLOODBOIL] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_PRE_SHAHRAZ_DOOR:                           // Door leading to Mother Shahraz
            if (m_auiEncounter[TYPE_SHADE] == DONE && m_auiEncounter[TYPE_GOREFIEND] == DONE && m_auiEncounter[TYPE_BLOODBOIL] == DONE && m_auiEncounter[TYPE_RELIQUIARY] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_POST_SHAHRAZ_DOOR:                          // Door after shahraz
            if (m_auiEncounter[TYPE_SHAHRAZ] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ILLIDAN_GATE:                               // Gate leading to Temple Summit
            if (m_auiEncounter[TYPE_ILLIDAN] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_PRE_COUNCIL_DOOR:                           // Door leading to the Council (grand promenade)
        case GO_COUNCIL_DOOR:                               // Door leading to the Council (inside)
        case GO_ILLIDAN_DOOR_R:                             // Right door at Temple Summit
        case GO_ILLIDAN_DOOR_L:                             // Left door at Temple Summit
            break;
        case GO_IMPALING_SPINE:
            m_impalingSpines.push_back(go->GetObjectGuid());
            break;

        default:
            return;
    }
    m_goEntryGuidStore[go->GetEntry()] = go->GetObjectGuid();
}

void instance_black_temple::OnCreatureEvade(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_RELIQUARY_COMBAT_TRIGGER:
        {
            if (Creature* reliquary = GetSingleCreatureFromStorage(NPC_RELIQUARY_OF_SOULS))
                reliquary->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, reliquary, reliquary);
            break;
        }
        default: break;
    }
}

void instance_black_temple::OnCreatureDeath(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_ANGERED_SOUL_FRAGMENT:
        case NPC_SUFFERING_SOUL_FRAGMENT:
        case NPC_HUNGERING_SOUL_FRAGMENT:
            if (GetData(TYPE_RELIQUIARY) == DONE)
            {
                creature->SetRespawnDelay(time(nullptr) + 7 * DAY);
                creature->SaveRespawnTime();
            }
            break;
    }
}

void instance_black_temple::SetData(uint32 type, uint32 data)
{
    switch (type)
    {
        case TYPE_NAJENTUS:
            m_auiEncounter[type] = data;
            if (data == DONE)
            {
                DoUseDoorOrButton(GO_NAJENTUS_GATE);
                if (Creature* trigger = GetSingleCreatureFromStorage(NPC_BLACK_TEMPLE_TRIGGER))
                    DoScriptText(EMOTE_OPEN_NAJENTUS_DOOR, trigger);
            }
            break;
        case TYPE_SUPREMUS:
            m_auiEncounter[type] = data;
            DoUseDoorOrButton(GO_NAJENTUS_GATE);
            if (data == DONE)
                DoUseDoorOrButton(GO_SUPREMUS_DOORS);
            break;
        case TYPE_SHADE:
            m_auiEncounter[type] = data;
            // combat door
            DoUseDoorOrButton(GO_SHADE_OF_AKAMA);
            if (data == FAIL)
                // Despawn channelers on fail
                for (ObjectGuid guid : m_channelersGuidVector)
                    if (Creature* channeler = instance->GetCreature(guid))
                        channeler->ForcedDespawn();
            if (data != IN_PROGRESS)
                for (ObjectGuid guid : m_creatureGeneratorGuidVector) // notify generators to stop spawning
                    if (Creature* generator = instance->GetCreature(guid))
                        generator->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, generator, generator);
            if (data == DONE)
            {
                DoOpenPreMotherDoor();
                for (ObjectGuid guid : m_ashtongue)
                    if (Creature* ashtongue = instance->GetCreature(guid))
                        ashtongue->setFaction(FACTION_ASHTONGUE_FRIENDLY);
            }
            break;
        case TYPE_GOREFIEND:
            m_auiEncounter[type] = data;
            DoUseDoorOrButton(GO_GOREFIEND_DOOR);
            if (data == DONE)
                DoOpenPreMotherDoor();
            break;
        case TYPE_BLOODBOIL:
            m_auiEncounter[type] = data;
            if (data == DONE)
            {
                DoOpenPreMotherDoor();
                DoUseDoorOrButton(GO_GURTOGG_DOOR);
            }
            break;
        case TYPE_RELIQUIARY:
            m_auiEncounter[type] = data;
            if (data == IN_PROGRESS)
            {
                if (Creature* trigger = GetSingleCreatureFromStorage(NPC_RELIQUARY_COMBAT_TRIGGER))
                {
                    trigger->SetActiveObjectState(true);
                    trigger->SetInCombatWithZone(false);
                    if (!trigger->IsInCombat())
                    {
                        SetData(TYPE_RELIQUIARY, FAIL);
                        return;
                    }
                }
            }
            if (data == DONE || data == FAIL)
            {
                if (Creature* trigger = GetSingleCreatureFromStorage(NPC_RELIQUARY_COMBAT_TRIGGER))
                {
                    trigger->CombatStop();
                    trigger->SetActiveObjectState(false);
                }
            }
            if (data == DONE)
            {
                for (ObjectGuid guid : m_soulFragments)
                {
                    if (Creature* soul = instance->GetCreature(guid))
                    {
                        soul->SetRespawnDelay(time(nullptr) + 7 * DAY);
                        soul->ForcedDespawn();
                    }
                }
                DoOpenPreMotherDoor();
            }
            if (data == FAIL)
            {
                for (ObjectGuid guid : m_soulFragments)
                {
                    if (Creature* soul = instance->GetCreature(guid))
                    {
                        soul->SetRespawnDelay(15);
                        soul->Respawn();
                    }
                }
            }
            break;
        case TYPE_SHAHRAZ:
            if (data == DONE)
                DoUseDoorOrButton(GO_POST_SHAHRAZ_DOOR);
            m_auiEncounter[type] = data;
            break;
        case TYPE_COUNCIL:
            // Don't set the same data twice
            if (m_auiEncounter[type] == data)
                return;
            m_auiEncounter[type] = data;
            DoUseDoorOrButton(GO_COUNCIL_DOOR);
            if (data == DONE)
                DoTeleportAkamaIfCan();
            break;
        case TYPE_ILLIDAN:
            if (data == IN_PROGRESS && GetData(TYPE_COUNCIL) != DONE)
                BanPlayersIfNoGm("Player engaged Illidan without killing council and Gamemaster being present in instance.");
            DoUseDoorOrButton(GO_ILLIDAN_DOOR_R);
            DoUseDoorOrButton(GO_ILLIDAN_DOOR_L);
            m_auiEncounter[type] = data;
            if (data == FAIL)
            {
                // Cleanup encounter
                if (Creature* illidan = GetSingleCreatureFromStorage(NPC_ILLIDAN_STORMRAGE))
                    illidan->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, illidan, illidan);
            }
            break;
        default:
            script_error_log("Instance Black Temple: ERROR SetData = %u for type %u does not exist/not implemented.", type, data);
            return;
    }

    if (data == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                   << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_black_temple::GetData(uint32 type) const
{
    if (type < MAX_ENCOUNTER)
        return m_auiEncounter[type];

    return 0;
}

void instance_black_temple::DoOpenPreMotherDoor()
{
    if (GetData(TYPE_SHADE) == DONE && GetData(TYPE_GOREFIEND) == DONE && GetData(TYPE_BLOODBOIL) == DONE && GetData(TYPE_RELIQUIARY) == DONE)
    {
        DoUseDoorOrButton(GO_PRE_SHAHRAZ_DOOR);
        if (Creature* trigger = GetSingleCreatureFromStorage(NPC_BLACK_TEMPLE_TRIGGER))
            DoScriptText(EMOTE_OPEN_MOTHER_DOOR, trigger);
    }
}

void instance_black_temple::DoTeleportAkamaIfCan()
{
    if (GetData(TYPE_COUNCIL) != DONE || m_akamaIllidanSequence || GetData(TYPE_ILLIDAN) == DONE)
        return;

    Creature* akama = GetSingleCreatureFromStorage(NPC_AKAMA, true);
    if (!akama) // will happen on him being loaded
        return;

    m_akamaIllidanSequence = true;
    akama->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, akama, akama);
}

void instance_black_temple::DespawnImpalingSpines()
{
    for (ObjectGuid guid : m_impalingSpines)
        if (GameObject* go = instance->GetGameObject(guid))
            go->Delete();

    m_impalingSpines.clear();
}

void instance_black_temple::RespawnChannelers()
{
    for (ObjectGuid guid : m_channelersGuidVector)
        if (Creature* channeler = instance->GetCreature(guid))
            channeler->Respawn();
}

void instance_black_temple::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7] >> m_auiEncounter[8];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)            // Do not load an encounter as "In Progress" - reset it instead.
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_black_temple(Map* map)
{
    return new instance_black_temple(map);
}

void AddSC_instance_black_temple()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_black_temple";
    pNewScript->GetInstanceData = &GetInstanceData_instance_black_temple;
    pNewScript->RegisterSelf();
}

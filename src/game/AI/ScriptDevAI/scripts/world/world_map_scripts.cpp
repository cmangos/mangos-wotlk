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
SDName: world_map_scripts
SD%Complete: 100
SDComment: Quest support: 1126, 4740, 8868, 11538
SDCategory: World Map Scripts
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "world_map_scripts.h"
#include "World/WorldState.h"
#include "World/WorldStateDefines.h"
#include "Entities/TemporarySpawn.h"
#include <array>
#include <ctime>
#include <cstring>
#include "AI/ScriptDevAI/scripts/world/scourge_invasion.h"
#include "World/World.h"
#include "brewfest.h"

enum
{
    SAY_DUROTAR_FROM_OG_ARRIVAL   = -1020001,
    SAY_TIRISFAL_FROM_UC_ARRIVAL  = -1020002,
    SAY_ST_FROM_GROMGOL_ARRIVAL   = -1020003,
    SAY_WK_DEPARTURE              = -1020004,
    SAY_WK_ARRIVAL                = -1020005,
    SAY_UC_FROM_VL_ARRIVAL        = -1020006,
    SAY_OG_FROM_BT_ARRIVAL        = -1020007,
    SAY_OG_FROM_TB_ARRIVAL        = -1020008,

    EVENT_UC_FROM_GROMGOL_ARRIVAL = 15312,
    EVENT_GROMGOL_FROM_UC_ARRIVAL = 15314,
    EVENT_OG_FROM_UC_ARRIVAL      = 15318,
    EVENT_UC_FROM_OG_ARRIVAL      = 15320,
    EVENT_OG_FROM_GROMGOL_ARRIVAL = 15322,
    EVENT_GROMGOL_FROM_OG_ARRIVAL = 15324,
    EVENT_WK_DEPARTURE            = 15430,
    EVENT_WK_ARRIVAL              = 15431,
    EVENT_VL_FROM_UC_ARRIVAL      = 19126,
    EVENT_UC_FROM_VL_ARRIVAL      = 19127,
    EVENT_OG_FROM_BT_ARRIVAL      = 19137,
    EVENT_BT_FROM_OG_ARRIVAL      = 19139,
    EVENT_OG_FROM_TB_ARRIVAL      = 21868,
    EVENT_TB_FROM_OG_ARRIVAL      = 21870,

    RELAY_UC_FROM_GROMGOL_ARRIVAL = 21000,
    RELAY_GROMGOL_FROM_UC_ARRIVAL = 21001,
    RELAY_OG_FROM_UC_ARRIVAL      = 21002,
    RELAY_UC_FROM_OG_ARRIVAL      = 21003,
    RELAY_OG_FROM_GROMGOL_ARRIVAL = 21004,
    RELAY_GROMGOL_FROM_OG_ARRIVAL = 21005,
    RELAY_VL_FROM_UC_ARRIVAL      = 21006,
    RELAY_UC_FROM_VL_ARRIVAL      = 21007,
    RELAY_OG_FROM_BT_ARRIVAL      = 21008,
    RELAY_BT_FROM_OG_ARRIVAL      = 21009,
    RELAY_OG_FROM_TB_ARRIVAL      = 21010,
    RELAY_TB_FROM_OG_ARRIVAL      = 21011,

    SOUND_ZEPPELIN_HORN           = 11804,
};

bool ProcessEventTransports(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    sWorldState.HandleConditionStateChange(pSource->GetEntry(), uiEventId);

    WorldObject* transport = (WorldObject*)pSource;
    uint32 entry = 0;
    int32 text_entry = 0;
    uint32 relay_entry = 0;
    switch (uiEventId)
    {
        case EVENT_UC_FROM_GROMGOL_ARRIVAL:         // UC arrival from gromgol
            entry = NPC_HINDENBURG;
            text_entry = SAY_ST_FROM_GROMGOL_ARRIVAL;
            relay_entry = RELAY_UC_FROM_GROMGOL_ARRIVAL;
            break;
        case EVENT_GROMGOL_FROM_UC_ARRIVAL:         // gromgol arrival from UC
            entry = NPC_SQUIBBY_OVERSPECK;
            text_entry = SAY_TIRISFAL_FROM_UC_ARRIVAL;
            relay_entry = RELAY_GROMGOL_FROM_UC_ARRIVAL;
            break;
        case EVENT_OG_FROM_UC_ARRIVAL:              // OG arrival from UC
            entry = NPC_FREZZA;
            text_entry = SAY_TIRISFAL_FROM_UC_ARRIVAL;
            relay_entry = RELAY_OG_FROM_UC_ARRIVAL;
            break;
        case EVENT_UC_FROM_OG_ARRIVAL:              // UC arrival from OG
            entry = NPC_ZAPETTA;
            text_entry = SAY_DUROTAR_FROM_OG_ARRIVAL;
            relay_entry = RELAY_UC_FROM_OG_ARRIVAL;
            break;
        case EVENT_OG_FROM_GROMGOL_ARRIVAL:         // OG arrival from gromgol
            entry = NPC_SNURK_BUCKSQUICK;
            text_entry = SAY_ST_FROM_GROMGOL_ARRIVAL;
            relay_entry = RELAY_OG_FROM_GROMGOL_ARRIVAL;
            break;
        case EVENT_GROMGOL_FROM_OG_ARRIVAL:         // gromgol arrival from OG
            entry = NPC_NEZRAZ;
            text_entry = SAY_DUROTAR_FROM_OG_ARRIVAL;
            relay_entry = RELAY_GROMGOL_FROM_OG_ARRIVAL;
            break;
        case EVENT_WK_ARRIVAL:                      // WestGuard Keep arrival
            entry = NPC_HARROWMEISER;
            text_entry = SAY_WK_ARRIVAL;
            break;
        case EVENT_WK_DEPARTURE:                    // WestGuard Keep departure
            entry = NPC_HARROWMEISER;
            text_entry = SAY_WK_DEPARTURE;
            break;
        case EVENT_VL_FROM_UC_ARRIVAL:              // Vengance Landing arrival from UC
            entry = NPC_DRENK_SPANNERSPARK;
            text_entry = SAY_TIRISFAL_FROM_UC_ARRIVAL;
            relay_entry = RELAY_VL_FROM_UC_ARRIVAL;
            break;
        case EVENT_UC_FROM_VL_ARRIVAL:              // UC arrival from Vengance Landing
            entry = NPC_MEEFI_FARTHROTTLE;
            text_entry = SAY_UC_FROM_VL_ARRIVAL;
            relay_entry = RELAY_UC_FROM_VL_ARRIVAL;
            break;
        case EVENT_OG_FROM_BT_ARRIVAL:              // OG arrival from BT
            entry = NPC_GREEB_RAMROCKET;
            text_entry = SAY_OG_FROM_BT_ARRIVAL;
            relay_entry = RELAY_OG_FROM_BT_ARRIVAL;
            break;
        case EVENT_BT_FROM_OG_ARRIVAL:              // BT arrival from OG
            entry = NPC_NARGO_SCREWBORE;
            text_entry = SAY_DUROTAR_FROM_OG_ARRIVAL;
            relay_entry = RELAY_BT_FROM_OG_ARRIVAL;
            break;
        case EVENT_OG_FROM_TB_ARRIVAL:              // OG arrival from TB
            entry = NPC_ZELLI_HOTNOZZLE;
            text_entry = SAY_OG_FROM_TB_ARRIVAL;
            relay_entry = RELAY_OG_FROM_TB_ARRIVAL;
            break;
        case EVENT_TB_FROM_OG_ARRIVAL:              // TB arrival from OG
            entry = NPC_KRENDLE_BIGPOCKETS;
            text_entry = SAY_DUROTAR_FROM_OG_ARRIVAL;
            relay_entry = RELAY_TB_FROM_OG_ARRIVAL;
            break;
    }
    if (entry)
        if (Creature* zeppelinMaster = static_cast<ScriptedInstance*>(transport->GetMap()->GetInstanceData())->GetSingleCreatureFromStorage(entry, true))
        {
            zeppelinMaster->PlayDistanceSound(SOUND_ZEPPELIN_HORN);
            DoScriptText(text_entry, zeppelinMaster);
            zeppelinMaster->GetMap()->ScriptsStart(SCRIPT_TYPE_RELAY, relay_entry, zeppelinMaster, zeppelinMaster);
        }
    return true;
}

void ShadeOfTheHorsemanData::Reset()
{
    memset(m_shadeOfTheHorsemanAttackPhases, 0, sizeof(m_shadeOfTheHorsemanAttackPhases));
}

uint32 ShadeOfTheHorsemanData::GetDataAttackPhase(uint32 id) const
{
    return m_shadeOfTheHorsemanAttackPhases[id];
}

void ShadeOfTheHorsemanData::SetDataAttackPhase(uint32 id, uint32 value)
{
    m_shadeOfTheHorsemanAttackPhases[id] = value;
}

bool ShadeOfTheHorsemanData::IsConditionFulfilled(uint32 conditionId, uint32 areaId) const
{
    uint32 checkAreaId = 0;
    switch (conditionId)
    {
        case INSTANCE_CONDITION_ID_FIRE_BRIGADE_PRACTICE_GOLDSHIRE:
            checkAreaId = AREAID_GOLDSHIRE;
            break;
        case INSTANCE_CONDITION_ID_FIRE_BRIGADE_PRACTICE_KHARANOS:
            checkAreaId = AREAID_KHARANOS;
            break;
        case INSTANCE_CONDITION_ID_FIRE_BRIGADE_PRACTICE_AZURE_WATCH:
            checkAreaId = AREAID_AZURE_WATCH;
            break;
        case INSTANCE_CONDITION_ID_FIRE_TRAINING_BRILL:
            checkAreaId = AREAID_BRILL;
            break;
        case INSTANCE_CONDITION_ID_FIRE_TRAINING_RAZOR_HILL:
            checkAreaId = AREAID_RAZOR_HILL;
            break;
        case INSTANCE_CONDITION_ID_FIRE_TRAINING_FALCONWING:
            checkAreaId = AREAID_FALCONWING_SQUARE;
            break;
    }
    
    if (checkAreaId)
        return m_shadeOfTheHorsemanAttackPhases[conditionId - INSTANCE_CONDITION_ID_FIRE_BRIGADE_PRACTICE_GOLDSHIRE] != SHADE_PHASE_SPAWNED && areaId == checkAreaId;

    switch (conditionId)
    {
        case INSTANCE_CONDITION_ID_STOP_THE_FIRES_ALLIANCE:
            return (m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_GOLDSHIRE] == SHADE_PHASE_SPAWNED && areaId == AREAID_GOLDSHIRE) ||
                   (m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_KHARANOS] == SHADE_PHASE_SPAWNED && areaId == AREAID_KHARANOS) ||
                   (m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_AZURE_WATCH] == SHADE_PHASE_SPAWNED && areaId == AREAID_AZURE_WATCH);
        case INSTANCE_CONDITION_ID_STOP_THE_FIRES_HORDE:
            return (m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_BRILL] == SHADE_PHASE_SPAWNED && areaId == AREAID_BRILL) ||
                   (m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_RAZOR_HILL] == SHADE_PHASE_SPAWNED && areaId == AREAID_RAZOR_HILL) ||
                   (m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_FALCONWING_SQUARE] == SHADE_PHASE_SPAWNED && areaId == AREAID_FALCONWING_SQUARE);
        case INSTANCE_CONDITION_ID_THE_HEADLESS_HORSEMAN_ALLIANCE:
            return (m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_GOLDSHIRE] == SHADE_PHASE_VICTORY && areaId == AREAID_GOLDSHIRE) ||
                   (m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_KHARANOS] == SHADE_PHASE_VICTORY && areaId == AREAID_KHARANOS) ||
                   (m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_AZURE_WATCH] == SHADE_PHASE_VICTORY && areaId == AREAID_AZURE_WATCH);
        case INSTANCE_CONDITION_ID_THE_HEADLESS_HORSEMAN_HORDE:
            return (m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_BRILL] == SHADE_PHASE_VICTORY && areaId == AREAID_BRILL) ||
                   (m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_RAZOR_HILL] == SHADE_PHASE_VICTORY && areaId == AREAID_RAZOR_HILL) ||
                   (m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_FALCONWING_SQUARE] == SHADE_PHASE_VICTORY && areaId == AREAID_FALCONWING_SQUARE);
        case INSTANCE_CONDITION_ID_LET_THE_FIRES_COME_ALLIANCE:
            return ((m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_GOLDSHIRE] == SHADE_PHASE_ALL_CLEAR || m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_GOLDSHIRE] == SHADE_PHASE_VICTORY) && areaId == AREAID_GOLDSHIRE) || 
                   ((m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_KHARANOS] == SHADE_PHASE_ALL_CLEAR || m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_GOLDSHIRE] == SHADE_PHASE_VICTORY) && areaId == AREAID_KHARANOS) ||
                   ((m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_AZURE_WATCH] == SHADE_PHASE_ALL_CLEAR || m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_GOLDSHIRE] == SHADE_PHASE_VICTORY) && areaId == AREAID_AZURE_WATCH);
        case INSTANCE_CONDITION_ID_LET_THE_FIRES_COME_HORDE:
            return ((m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_BRILL] == SHADE_PHASE_ALL_CLEAR || m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_BRILL] == SHADE_PHASE_VICTORY) && areaId == AREAID_BRILL) ||
                   ((m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_RAZOR_HILL] == SHADE_PHASE_ALL_CLEAR || m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_RAZOR_HILL] == SHADE_PHASE_VICTORY) && areaId == AREAID_RAZOR_HILL) ||
                   ((m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_FALCONWING_SQUARE] == SHADE_PHASE_ALL_CLEAR || m_shadeOfTheHorsemanAttackPhases[SHADE_VILLAGE_FALCONWING_SQUARE] == SHADE_PHASE_VICTORY) && areaId == AREAID_FALCONWING_SQUARE);
        default: return false;
    }
}

uint32 ShadeOfTheHorsemanData::HandleGetData(uint32 id) const
{
    return m_shadeOfTheHorsemanAttackPhases[id - TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE];
}

void ShadeOfTheHorsemanData::HandleSetData(uint32 id, uint32 value)
{
    m_shadeOfTheHorsemanAttackPhases[id - TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE] = value;
}

uint32 ShadeOfTheHorsemanData::GetTypeFromZoneId(uint32 zoneId)
{
    switch (zoneId)
    {
        case ZONEID_ELWYNN_FOREST: return SHADE_VILLAGE_GOLDSHIRE;
        case ZONEID_DUN_MOROGH: return SHADE_VILLAGE_KHARANOS;
        case ZONEID_AZUREMYST_ISLE: return SHADE_VILLAGE_AZURE_WATCH;
        case ZONEID_TIRISFAL_GLADES: return SHADE_VILLAGE_BRILL;
        case ZONEID_DUROTAR: return SHADE_VILLAGE_RAZOR_HILL;
        case ZONEID_EVERSONG_WOODS: return SHADE_VILLAGE_FALCONWING_SQUARE;
        default: return 0;
    }
}

void AddSC_world_map_scripts()
{    
    Script* pNewScript = new Script;
    pNewScript->Name = "event_transports";
    pNewScript->pProcessEventId = &ProcessEventTransports;
    pNewScript->RegisterSelf();
}

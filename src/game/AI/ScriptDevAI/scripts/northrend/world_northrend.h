/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_WORLD_NORTHREND_H
#define DEF_WORLD_NORTHREND_H

#include "Common.h"
#include <map>

 /* *********************************************************
  *                     NORTHREND
  */
struct world_map_northrend : public ScriptedMap
{
    public:
        world_map_northrend(Map* pMap) : ScriptedMap(pMap) {}

        void OnCreatureCreate(Creature* creature) override;

        void SetData(uint32 /*uiType*/, uint32 /*uiData*/) override;

        void SetDalaranCooldownTime(ObjectGuid playerGuid);
        bool IsDalaranCooldownForPlayer(ObjectGuid playerGuid) const;

    private:
        std::map<ObjectGuid, TimePoint> m_dalaranAreatriggerCooldown;
};

#endif
/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
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
#ifndef __BATTLEGROUNDDS_H
#define __BATTLEGROUNDDS_H

enum
{
    BG_DS_CREATURE_WATER_SPOUT        = 28567,
    BG_DS_CREATURE_WORLD_TRIGGER      = 23472,

    // BG_DS_GO_DALARA_SEWERS_DOOR_1  = 192642,
    // BG_DS_GO_DALARA_SEWERS_DOOR_2  = 192643,
    BG_DS_GO_WATERFALL_COLLISION      = 194395,
    BG_DS_GO_WATERFALL_ANIM           = 191877,
    // BG_DS_GO_SHADOW_SIGHT_1        = 184663,
    // BG_DS_GO_SHADOW_SIGHT_2        = 184664,

    BG_DS_SPELL_FLUSH                 = 57405,    // Triggers 61698; Visual and target selector for the starting knockback from the pipe
    BG_DS_SPELL_WATER_SPOUT           = 58873,    // Knockback effect of the central waterfall

    BG_DS_SPELL_DEMONIC_CIRCLE        = 48018,    // Demonic Circle Summon

    BG_DS_SPELL_TRACTOR_BEAM          = 28266,
};

class BattleGround;

class BattleGroundDS : public BattleGround
{
        friend class BattleGroundMgr;

    public:
        BattleGroundDS();

        /* inherited from BattlegroundClass */
        virtual void StartingEventOpenDoors() override;
        virtual void Reset() override;

        bool HandleAreaTrigger(Player* /*player*/, uint32 /*triggerId*/) override;
        bool HandlePlayerUnderMap(Player* /*player*/) override;

        void HandleCreatureCreate(Creature* /*creature*/) override;
        void HandleGameObjectCreate(GameObject* /*go*/) override;

        void Update(uint32 /*diff*/) override;

    private:
        uint32 m_uiFlushTimer;
        uint32 m_uiWaterfallTimer;
        uint32 m_uiWaterfallSpellTimer;
        uint8 m_uiWaterfallStage;

        GuidList m_lGateTriggersGuids;

        ObjectGuid m_waterfallTriggerGuid;
        ObjectGuid m_waterfallCollisionGuid;
        ObjectGuid m_waterfallAnimGuid;
        ObjectGuid m_worldTriggerGuid;
};
#endif

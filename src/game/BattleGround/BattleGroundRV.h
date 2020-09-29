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
#ifndef __BATTLEGROUNDRV_H
#define __BATTLEGROUNDRV_H

enum
{
    // BG_RV_GO_ARENA_ELEVATOR_1      = 194582,
    // BG_RV_GO_ARENA_ELEVATOR_2      = 194586,
    // BG_RV_GO_SHADOW_SIGHT_1        = 184663,
    // BG_RV_GO_SHADOW_SIGHT_2        = 184664,

    // BG_RV_GO_ORG_ARENA_FIRE_1      = 192704,       // fires removed as of 3.2.0
    // BG_RV_GO_ORG_ARENA_FIRE_2      = 192705,
    // BG_RV_GO_ORG_ARENA_FIREDOOR_1  = 192387,
    // BG_RV_GO_ORG_ARENA_FIREDOOR_2  = 192388,

    // animations
    BG_RV_GO_ORG_ARENA_PULLEY_1       = 192389,       // arena
    BG_RV_GO_ORG_ARENA_PULLEY_2       = 192390,       // ivory
    BG_RV_GO_ORG_ARENA_GEAR_1         = 192393,       // lightning
    BG_RV_GO_ORG_ARENA_GEAR_2         = 192394,       // axe

    // collisions
    BG_RV_GO_ORG_ARENA_COLLISION_1    = 194580,       // axe
    BG_RV_GO_ORG_ARENA_COLLISION_2    = 194579,       // arena
    BG_RV_GO_ORG_ARENA_COLLISION_3    = 194581,       // lightning
    BG_RV_GO_ORG_ARENA_COLLISION_4    = 194578,       // ivory

    // transports
    BG_RV_GO_ORG_ARENA_PILAR_1        = 194583,       // axe
    BG_RV_GO_ORG_ARENA_PILAR_2        = 194584,       // arena
    BG_RV_GO_ORG_ARENA_PILAR_3        = 194585,       // lightning
    BG_RV_GO_ORG_ARENA_PILAR_4        = 194587        // ivory
};

class BattleGround;

class BattleGroundRV : public BattleGround
{
        friend class BattleGroundMgr;

    public:
        BattleGroundRV();

        /* inherited from BattlegroundClass */
        void StartingEventOpenDoors() override;

        bool HandleAreaTrigger(Player* /*player*/, uint32 /*triggerId*/) override;

        bool HandlePlayerUnderMap(Player* /*player*/) override;

        void HandleGameObjectCreate(GameObject* /*go*/) override;

        void Update(uint32 /*diff*/) override;

    private:
        void DoSwitchPillars();

        uint32 m_uiPillarTimer;

        GuidList m_lPillarsGuids;
        GuidList m_lCollisionsGuids;
        GuidList m_lAnimationsGuids;
};
#endif

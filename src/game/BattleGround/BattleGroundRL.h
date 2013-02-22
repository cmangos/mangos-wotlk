/*
 * This file is part of the Continued-MaNGOS Project
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
#ifndef __BATTLEGROUNDRL_H
#define __BATTLEGROUNDRL_H

class BattleGround;

class BattleGroundRLScore : public BattleGroundScore
{
    public:
        BattleGroundRLScore() {};
        virtual ~BattleGroundRLScore() {};
        // TODO fix me
};

class BattleGroundRL : public BattleGround
{
        friend class BattleGroundMgr;

    public:
        BattleGroundRL();

        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player* plr) override;
        virtual void FillInitialWorldStates(WorldPacket& d, uint32& count) override;
        virtual void StartingEventOpenDoors() override;

        void RemovePlayer(Player* plr, ObjectGuid guid) override;
        void HandleAreaTrigger(Player* source, uint32 trigger) override;
        void HandleKillPlayer(Player* player, Player* killer) override;
        bool HandlePlayerUnderMap(Player* plr) override;
};
#endif

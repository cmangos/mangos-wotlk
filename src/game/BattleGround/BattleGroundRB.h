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

#ifndef __BATTLEGROUNDABG_H
#define __BATTLEGROUNDABG_H

class BattleGround;

class BattleGroundABGScore : public BattleGroundScore
{
    public:
        BattleGroundABGScore() {};
        virtual ~BattleGroundABGScore() {};
};

class BattleGroundRB : public BattleGround
{
        friend class BattleGroundMgr;

    public:
        BattleGroundRB();

        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player* plr) override;

        /* Scorekeeping */
        void UpdatePlayerScore(Player* source, uint32 type, uint32 value) override;

    private:
};
#endif

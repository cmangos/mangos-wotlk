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

#ifndef MANGOS_TOTEMAI_H
#define MANGOS_TOTEMAI_H

#include "AI/BaseAI/CreatureAI.h"
#include "AI/EventAI/CreatureEventAI.h"
#include "Entities/ObjectGuid.h"
#include "Timer.h"

class Creature;
class Totem;

class TotemAI : public CreatureEventAI
{
    public:
        explicit TotemAI(Creature* c);
        static int Permissible(const Creature*);

        virtual void MoveInLineOfSight(Unit*) override;
        virtual void AttackStart(Unit*) override;
        virtual void EnterEvadeMode() override;
        virtual bool IsVisible(Unit*) const override;

        virtual void UpdateAI(const uint32) override;

    protected:
        Totem& getTotem() const;

    private:
        ObjectGuid i_victimGuid;
};
#endif

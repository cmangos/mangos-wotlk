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

#ifndef GAMEOBJECT_DEFINES_H
#define GAMEOBJECT_DEFINES_H

// client side GO show states
enum GOState
{
    GO_STATE_ACTIVE             = 0,                        // show in world as used and not reset (closed door open)
    GO_STATE_READY              = 1,                        // show in world as ready (closed door close)
    GO_STATE_ACTIVE_ALTERNATIVE = 2                         // show in world as used in alt way and not reset (closed door open by cannon fire)
};

enum GameObjectDestructibleState
{
    GO_DESTRUCTIBLE_INTACT      = 0,
    GO_DESTRUCTIBLE_DAMAGED     = 1,
    GO_DESTRUCTIBLE_DESTROYED   = 2,
    GO_DESTRUCTIBLE_REBUILDING  = 3
};

#define MAX_GO_STATE              3

#endif
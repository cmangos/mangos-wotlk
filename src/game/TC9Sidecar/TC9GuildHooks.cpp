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

#include "TC9GuildHooks.h"
#include "Globals/ObjectAccessor.h"
#include "Entities/Player.h"

void ToCloud9GuildHooks::OnGuildMemberAdded(uint64 guild, uint64 character)
{
    Player *player = ObjectAccessor::FindPlayer(ObjectGuid(character));
    if (!player)
        return;

    player->SetInGuild(guild);
}

void ToCloud9GuildHooks::OnGuildMemberRemoved(uint64 /*guild*/, uint64 character)
{
    Player *player = ObjectAccessor::FindPlayer(ObjectGuid(character));
    if (!player)
        return;

    player->SetInGuild(0);
}

void ToCloud9GuildHooks::OnGuildMemberLeft(uint64 /*guild*/, uint64 character)
{
    Player *player = ObjectAccessor::FindPlayer(ObjectGuid(character));
    if (!player)
        return;

    player->SetInGuild(0);
}

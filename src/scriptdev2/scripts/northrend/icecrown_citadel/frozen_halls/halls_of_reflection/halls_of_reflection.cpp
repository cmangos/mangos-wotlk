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
SDName: halls_of_reflection.cpp
SD%Complete: 30
SDComment: Support for the first encounters only
SDCategory: Halls of Reflection
EndScriptData */

#include "precompiled.h"
#include "halls_of_reflection.h"

/*######
## at_frostmourne_chamber
######*/

static const float afFalricSpawnLoc[4] = { 5276.583f, 2037.450f, 709.402f, 5.532f };
static const float afMarwybSpawnLoc[4] = { 5342.232f, 1975.696f, 709.402f, 2.391f };

bool AreaTrigger_at_frostmourne_chamber(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pAt->id == AREATRIGGER_FROSTMOURNE_ALTAR)
    {
        if (pPlayer->isGameMaster() || !pPlayer->isAlive())
            return false;

        instance_halls_of_reflection* pInstance = (instance_halls_of_reflection*)pPlayer->GetInstanceData();
        if (!pInstance)
            return false;

        // reset event only after fail and if intro is complete or Maryn isn't complete
        if (pInstance->GetData(TYPE_FROSTMOURNE_INTRO) != DONE || pInstance->GetData(TYPE_MARWYN) == DONE)
            return false;

        // reset on Falric fail
        if (pInstance->GetData(TYPE_FALRIC) == FAIL)
        {
            pPlayer->SummonCreature(NPC_FALRIC, afFalricSpawnLoc[0], afFalricSpawnLoc[1], afFalricSpawnLoc[2], afFalricSpawnLoc[3], TEMPSUMMON_DEAD_DESPAWN, 0);
            pPlayer->SummonCreature(NPC_MARWYN, afMarwybSpawnLoc[0], afMarwybSpawnLoc[1], afMarwybSpawnLoc[2], afMarwybSpawnLoc[3], TEMPSUMMON_DEAD_DESPAWN, 0);

            pInstance->SetData(TYPE_FALRIC, SPECIAL);
        }
        // reset on Marwyn fail
        else if (pInstance->GetData(TYPE_FALRIC) == DONE && pInstance->GetData(TYPE_MARWYN) == FAIL)
        {
            pPlayer->SummonCreature(NPC_MARWYN, afMarwybSpawnLoc[0], afMarwybSpawnLoc[1], afMarwybSpawnLoc[2], afMarwybSpawnLoc[3], TEMPSUMMON_DEAD_DESPAWN, 0);

            pInstance->SetData(TYPE_MARWYN, SPECIAL);
        }
    }

    return false;
}

enum
{
    SPELL_FROSTMOURNE_EQUIP         = 72729,
};

/*######
## spell_aura_dummy_frostmourne_equip
######*/

bool EffectAuraDummy_spell_aura_dummy_frostmourne_equip(const Aura* pAura, bool bApply)
{
    // ### Workaround alert! ###
    // This is required in order to forced despawn the Frostmourne Gameobject
    // To be removed and replaced with DBscripts once proper despawning of Door type GOs is supported
    if (pAura->GetId() == SPELL_FROSTMOURNE_EQUIP && pAura->GetEffIndex() == EFFECT_INDEX_0 && bApply)
    {
        if (Creature* pTarget = (Creature*)pAura->GetTarget())
        {
            ScriptedInstance* pInstance = (ScriptedInstance*)pTarget->GetInstanceData();
            if (!pInstance)
                return true;

            if (GameObject* pGo = pInstance->GetSingleGameObjectFromStorage(GO_FROSTMOURNE))
                pGo->Delete();
        }
    }
    return true;
}

void AddSC_halls_of_reflection()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "at_frostmourne_chamber";
    pNewScript->pAreaTrigger = &AreaTrigger_at_frostmourne_chamber;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spell_aura_dummy_frostmourne_equip";
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_spell_aura_dummy_frostmourne_equip;
    pNewScript->RegisterSelf();
}

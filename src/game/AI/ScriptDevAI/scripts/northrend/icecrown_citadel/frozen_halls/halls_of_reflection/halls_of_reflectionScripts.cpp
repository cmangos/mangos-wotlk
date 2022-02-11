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
SD%Complete: 90
SDComment: Scripts for the first encounter and the mini-boss. Quests 24480 and 24561.
SDCategory: Halls of Reflection
EndScriptData */

/* ContentData
at_frostmourne_chamber
spell_aura_dummy_frostmourne_equip
at_frostworn_general
spell_summon_reflections
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "halls_of_reflection.h"
#include "Entities/TemporarySpawn.h"
#include "Spells/SpellAuras.h"

/*######
## at_frostmourne_chamber
######*/

enum
{
    SAY_MARWYN_GAUNTLET_START               = -1668000,         // ToDo: research how is this text used
};

static const float afFalricSpawnLoc[4] = { 5276.583f, 2037.450f, 709.402f, 5.532f };
static const float afMarwybSpawnLoc[4] = { 5342.232f, 1975.696f, 709.402f, 2.391f };

bool AreaTrigger_at_frostmourne_chamber(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pAt->id == AREATRIGGER_FROSTMOURNE_ALTAR)
    {
        if (pPlayer->IsGameMaster() || !pPlayer->IsAlive())
            return false;

        instance_halls_of_reflection* pInstance = (instance_halls_of_reflection*)pPlayer->GetInstanceData();
        if (!pInstance)
            return false;

        // reset event only after fail and if intro is complete or Maryn isn't complete
        if (pInstance->GetData(TYPE_FROSTMOURNE_INTRO) != DONE || pInstance->GetData(TYPE_MARWYN) == DONE || pInstance->GetData(TYPE_QUEL_DELAR) == IN_PROGRESS)
            return false;

        // reset on Falric fail
        if (pInstance->GetData(TYPE_FALRIC) == FAIL)
        {
            pPlayer->SummonCreature(NPC_FALRIC, afFalricSpawnLoc[0], afFalricSpawnLoc[1], afFalricSpawnLoc[2], afFalricSpawnLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0);
            pPlayer->SummonCreature(NPC_MARWYN, afMarwybSpawnLoc[0], afMarwybSpawnLoc[1], afMarwybSpawnLoc[2], afMarwybSpawnLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0);
        }
        // reset on Marwyn fail
        else if (pInstance->GetData(TYPE_FALRIC) == DONE && pInstance->GetData(TYPE_MARWYN) == FAIL)
        {
            if (Creature* pMarwyn = pPlayer->SummonCreature(NPC_MARWYN, afMarwybSpawnLoc[0], afMarwybSpawnLoc[1], afMarwybSpawnLoc[2], afMarwybSpawnLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0))
                DoScriptText(SAY_MARWYN_GAUNTLET_START, pMarwyn);
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

/*######
## at_frostworn_general
######*/

enum
{
    SPELL_FROZEN_POSITION                   = 69609,                    // in c_t_a
    SPELL_HALLS_OF_REFLECTION_CLONE_NAME    = 69837,
    SPELL_HALLS_OF_REFLECTION_CLONE         = 69828,
    SPELL_HALLS_OF_REFLECTION_GHOST         = 69861,
};

bool AreaTrigger_at_frostworn_general(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pAt->id == AREATRIGGER_FROSTWORN_GENERAL)
    {
        if (pPlayer->IsGameMaster() || !pPlayer->IsAlive())
            return false;

        instance_halls_of_reflection* pInstance = (instance_halls_of_reflection*)pPlayer->GetInstanceData();
        if (!pInstance)
            return false;

        if (pInstance->GetData(TYPE_MARWYN) != DONE || pInstance->GetData(TYPE_FROSTWORN_GENERAL) == IN_PROGRESS || pInstance->GetData(TYPE_FROSTWORN_GENERAL) == DONE)
            return false;

        // Get the dongeon traps list to get summon location
        GuidList lDugeonTrapsGuids;
        pInstance->GetDungeonTrapsGUIDList(lDugeonTrapsGuids);

        if (lDugeonTrapsGuids.empty())
        {
            script_error_log("instance_halls_of_reflection: Error: couldn't find any dungeon trap stalker.");
            return true;
        }

        // summon a reflection for each player in the list
        Map::PlayerList const& allPlayers = pInstance->instance->GetPlayers();
        if (allPlayers.isEmpty())
            return false;

        for (const auto& allPlayer : allPlayers)
        {
            Player* pPlayerTarget = allPlayer.getSource();
            if (!pPlayerTarget)
                continue;

            // Spawn a reflection using the player as summoner and the dungeon trap for location
            GuidList::iterator iter = lDugeonTrapsGuids.begin();
            advance(iter, urand(0, lDugeonTrapsGuids.size() - 1));

            Creature* pSpawnCreature = pPlayerTarget->GetMap()->GetCreature(*iter);
            if (!pSpawnCreature)
                return false;

            // spawn a spiritual reflection
            // ToDo: research what is the difference between the two entries
            if (Creature* pReflection = pPlayerTarget->SummonCreature(urand(0, 1) ? NPC_SPIRITUAL_REFLECTION_1 : NPC_SPIRITUAL_REFLECTION_2, pSpawnCreature->GetPositionX(), pSpawnCreature->GetPositionY(), pSpawnCreature->GetPositionZ(), pSpawnCreature->GetOrientation(), TEMPSPAWN_DEAD_DESPAWN, 0))
            {
                pPlayerTarget->CastSpell(pReflection, SPELL_HALLS_OF_REFLECTION_CLONE_NAME, TRIGGERED_OLD_TRIGGERED);
                pPlayerTarget->CastSpell(pReflection, SPELL_HALLS_OF_REFLECTION_CLONE, TRIGGERED_OLD_TRIGGERED);
                pReflection->CastSpell(pReflection, SPELL_HALLS_OF_REFLECTION_GHOST, TRIGGERED_OLD_TRIGGERED);
            }

            lDugeonTrapsGuids.remove(*iter);
        }

        pInstance->SetData(TYPE_FROSTWORN_GENERAL, IN_PROGRESS);
    }

    return false;
}

/*######
## spell_summon_reflections
######*/

enum
{
    SPELL_SUMMON_REFLECTIONS    = 69223,
    SPELL_JUMPT_TO_TARGET       = 69886,
};

bool EffectDummyCreature_spell_summon_reflections(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_SUMMON_REFLECTIONS && uiEffIndex == EFFECT_INDEX_0)
    {
        instance_halls_of_reflection* pInstance = (instance_halls_of_reflection*)pCreatureTarget->GetInstanceData();
        if (!pInstance)
            return true;

        GuidList lReflectionsGuids;
        pInstance->GetReflectionsGUIDList(lReflectionsGuids);

        if (lReflectionsGuids.empty())
        {
            script_error_log("instance_halls_of_reflection: Error: couldn't find any spiritual reflection.");
            return true;
        }

        // Set all reflections to attack owner
        for (GuidList::const_iterator itr = lReflectionsGuids.begin(); itr != lReflectionsGuids.end(); ++itr)
        {
            if (Creature* pCreature = pCreatureTarget->GetMap()->GetCreature(*itr))
            {
                if (pCreature->IsTemporarySummon())
                {
                    if (Player* pSummoner = pCreature->GetMap()->GetPlayer(pCreature->GetSpawnerGuid()))
                    {
                        pCreature->SetLevitate(false);
                        pCreature->AI()->AttackStart(pSummoner);
                        pCreature->RemoveAurasDueToSpell(SPELL_FROZEN_POSITION);
                        pCreature->CastSpell(pSummoner, SPELL_JUMPT_TO_TARGET, TRIGGERED_OLD_TRIGGERED);
                    }
                }
            }
        }
    }

    return true;
};

/*######
## at_queldelar_start
######*/

enum
{
    SPELL_QUELDELARS_WILL           = 70698,
};

bool AreaTrigger_at_queldelar_start(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pAt->id == AREATRIGGER_QUELDELAR_START)
    {
        if (pPlayer->IsGameMaster() || !pPlayer->IsAlive())
            return false;

        if (!pPlayer->HasAura(SPELL_QUELDELAR_COMPULSION))
            return false;

        instance_halls_of_reflection* pInstance = static_cast<instance_halls_of_reflection*>(pPlayer->GetInstanceData());
        if (!pInstance)
            return false;

        if (pInstance->GetData(TYPE_QUEL_DELAR) != SPECIAL)
            return false;

        // start event
        if (Creature* pUther = pInstance->GetSingleCreatureFromStorage(NPC_UTHER))
            pUther->GetMotionMaster()->MoveWaypoint();

        pPlayer->CastSpell(pPlayer, SPELL_QUELDELARS_WILL, TRIGGERED_OLD_TRIGGERED);

        pInstance->SetData(TYPE_QUEL_DELAR, IN_PROGRESS);
    }

    return true;
};

/*######
## spell_gunship_cannon_fire_aura - 70017
######*/

struct spell_gunship_cannon_fire_aura : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& /*data*/) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        uint32 spellId;

        // handle gunship fire at the end of Lich King event
        switch (target->GetEntry())
        {
            case 22515: spellId = 70021; break;
            case 37593: spellId = 70246; break;
            default: return;
        }

        target->CastSpell(target, spellId, TRIGGERED_NONE);
    }
};

/*######
## spell_halls_of_reflection_clone - 69828
######*/

struct spell_halls_of_reflection_clone : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        // run code for eff1 and eff2
        if (effIdx == EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster)
            return;

        // trigger spell 69891 and 69892
        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        target->CastSpell(caster, spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_start_halls_of_reflection_quest - 72900
######*/

struct spell_start_halls_of_reflection_quest : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer())
            return;

        Player* targetPlayer = static_cast<Player*>(target);
        target->CastSpell(target, targetPlayer->GetTeam() == ALLIANCE ? 71351 : 71542, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_halls_of_reflection()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "at_frostmourne_chamber";
    pNewScript->pAreaTrigger = &AreaTrigger_at_frostmourne_chamber;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spell_aura_dummy_frostmourne_equip";
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_spell_aura_dummy_frostmourne_equip;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_frostworn_general";
    pNewScript->pAreaTrigger = &AreaTrigger_at_frostworn_general;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spell_summon_reflections";
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_spell_summon_reflections;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_queldelar_start";
    pNewScript->pAreaTrigger = &AreaTrigger_at_queldelar_start;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_gunship_cannon_fire_aura>("spell_gunship_cannon_fire_aura");
    RegisterSpellScript<spell_halls_of_reflection_clone>("spell_halls_of_reflection_clone");
    RegisterSpellScript<spell_start_halls_of_reflection_quest>("spell_start_halls_of_reflection_quest");
}

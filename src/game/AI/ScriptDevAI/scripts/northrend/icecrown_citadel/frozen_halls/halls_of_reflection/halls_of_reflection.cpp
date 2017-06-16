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
npc_phantom_mage
at_frostworn_general
spell_summon_reflections
EndContentData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "halls_of_reflection.h"
#include "Entities/TemporarySummon.h"

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
        if (pPlayer->isGameMaster() || !pPlayer->isAlive())
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
            pPlayer->SummonCreature(NPC_FALRIC, afFalricSpawnLoc[0], afFalricSpawnLoc[1], afFalricSpawnLoc[2], afFalricSpawnLoc[3], TEMPSUMMON_DEAD_DESPAWN, 0);
            pPlayer->SummonCreature(NPC_MARWYN, afMarwybSpawnLoc[0], afMarwybSpawnLoc[1], afMarwybSpawnLoc[2], afMarwybSpawnLoc[3], TEMPSUMMON_DEAD_DESPAWN, 0);
        }
        // reset on Marwyn fail
        else if (pInstance->GetData(TYPE_FALRIC) == DONE && pInstance->GetData(TYPE_MARWYN) == FAIL)
        {
            if (Creature* pMarwyn = pPlayer->SummonCreature(NPC_MARWYN, afMarwybSpawnLoc[0], afMarwybSpawnLoc[1], afMarwybSpawnLoc[2], afMarwybSpawnLoc[3], TEMPSUMMON_DEAD_DESPAWN, 0))
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
## npc_phantom_mage
######*/

enum
{
    SPELL_CHAIN_OF_ICE          = 72171,
    SPELL_FIREBALL              = 72163,
    SPELL_FLAMESTRIKE           = 72169,
    SPELL_FROSTBOLT             = 72166,
    SPELL_HALLUCINATION         = 72342,
};

struct npc_phantom_mageAI : public ScriptedAI
{
    npc_phantom_mageAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_bHasHallucination;

    uint32 m_uiHallucinationTimer;
    uint32 m_uiChainsIceTimer;
    uint32 m_uiFireballTimer;
    uint32 m_uiFrostboltTimer;
    uint32 m_uiFlamestrikeTimer;

    void Reset() override
    {
        m_bHasHallucination     = false;

        m_uiHallucinationTimer  = 0;
        m_uiChainsIceTimer      = urand(4000, 7000);
        m_uiFlamestrikeTimer    = urand(6000, 9000);
        m_uiFireballTimer       = urand(2000, 5000);
        m_uiFrostboltTimer      = urand(3000, 5000);
    }

    void EnterEvadeMode() override
    {
        if (m_uiHallucinationTimer)
            return;

        ScriptedAI::EnterEvadeMode();
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->AI()->AttackStart(m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiHallucinationTimer)
        {
            if (m_uiHallucinationTimer <= uiDiff)
                m_uiHallucinationTimer = 0;
            else
                m_uiHallucinationTimer -= uiDiff;

            // no other actions during Hallucination
            return;
        }

        if (!m_bHasHallucination && m_creature->GetHealthPercent() <= 50.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_HALLUCINATION) == CAST_OK)
            {
                m_uiHallucinationTimer = 1000;
                m_bHasHallucination = true;
            }
        }

        if (m_uiChainsIceTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_CHAIN_OF_ICE) == CAST_OK)
                    m_uiChainsIceTimer = urand(8000, 13000);
            }
        }
        else
            m_uiChainsIceTimer -= uiDiff;

        if (m_uiFlamestrikeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_FLAMESTRIKE) == CAST_OK)
                    m_uiFlamestrikeTimer = urand(7000, 12000);
            }
        }
        else
            m_uiFlamestrikeTimer -= uiDiff;

        if (m_uiFireballTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIREBALL) == CAST_OK)
                m_uiFireballTimer = urand(4000, 8000);
        }
        else
            m_uiFireballTimer -= uiDiff;

        if (m_uiFrostboltTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROSTBOLT) == CAST_OK)
                m_uiFrostboltTimer = urand(3000, 6000);
        }
        else
            m_uiFrostboltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_phantom_mage(Creature* pCreature)
{
    return new npc_phantom_mageAI(pCreature);
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
        if (pPlayer->isGameMaster() || !pPlayer->isAlive())
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

        for (Map::PlayerList::const_iterator itr = allPlayers.begin(); itr != allPlayers.end(); ++itr)
        {
            Player* pPlayerTarget = itr->getSource();
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
            if (Creature* pReflection = pPlayerTarget->SummonCreature(urand(0, 1) ? NPC_SPIRITUAL_REFLECTION_1 : NPC_SPIRITUAL_REFLECTION_2, pSpawnCreature->GetPositionX(), pSpawnCreature->GetPositionY(), pSpawnCreature->GetPositionZ(), pSpawnCreature->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0))
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
                    TemporarySummon* pTemporary = (TemporarySummon*)pCreature;

                    if (Player* pSummoner = pCreature->GetMap()->GetPlayer(pTemporary->GetSummonerGuid()))
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
        if (pPlayer->isGameMaster() || !pPlayer->isAlive())
            return false;

        if (!pPlayer->HasAura(SPELL_QUELDELAR_COMPULSION))
            return false;

        instance_halls_of_reflection* pInstance = (instance_halls_of_reflection*)pPlayer->GetInstanceData();
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

    pNewScript = new Script;
    pNewScript->Name = "npc_phantom_mage";
    pNewScript->GetAI = &GetAI_npc_phantom_mage;
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
}

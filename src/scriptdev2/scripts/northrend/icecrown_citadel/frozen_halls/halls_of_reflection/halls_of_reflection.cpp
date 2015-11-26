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

/* ContentData
at_frostmourne_chamber
spell_aura_dummy_frostmourne_equip
npc_phantom_mage
EndContentData */

#include "precompiled.h"
#include "halls_of_reflection.h"

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
        if (pInstance->GetData(TYPE_FROSTMOURNE_INTRO) != DONE || pInstance->GetData(TYPE_MARWYN) == DONE)
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
}

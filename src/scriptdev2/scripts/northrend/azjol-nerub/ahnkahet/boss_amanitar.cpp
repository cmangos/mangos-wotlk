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
SDName: Boss_Amanitar
SD%Complete: 80
SDComment: Mushrooms summoning may need improvements;
SDCategory: Ahn'kahet
EndScriptData */

#include "precompiled.h"
#include "ahnkahet.h"

enum
{
    SPELL_BASH                      = 57094,
    SPELL_VENOM_BOLT_VOLLEY         = 57088,
    SPELL_ENTANGLING_ROOTS          = 57095,
    SPELL_MINI                      = 57055,
    SPELL_REMOVE_MUSHROOM_POWER     = 57283,                // purpose unk - this spell may remove the Mini aura from all players

    // Mushroom entries
    NPC_HEALTHY_MUSHROOM            = 30391,
    NPC_POISONOUS_MUSHROOM          = 30435,

    // Mushroom spells
    SPELL_POISON_CLOUD              = 57061,
    SPELL_POTENT_FUNGUS             = 56648,
    SPELL_POISON_MUSHROOM_VISUAL    = 56741,
    SPELL_POWER_MUSHROOM_VISUAL     = 56740,
    SPELL_MUSHROOM_FORM             = 31690,
};

static const float aMushroomPos[3] = {362.8f, -869.16f, -75.03f};

/*######
## boss_amanitar
######*/

struct boss_amanitarAI : public ScriptedAI
{
    boss_amanitarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiBashTimer;
    uint32 m_uiVenomBoltTimer;
    uint32 m_uiRootsTimer;
    uint32 m_uiMiniTimer;
    uint32 m_uiMushroomTimer;

    void Reset() override
    {
        m_uiBashTimer       = urand(7000, 10000);
        m_uiVenomBoltTimer  = urand(10000, 15000);
        m_uiRootsTimer      = 20000;
        m_uiMiniTimer       = urand(20000, 25000);
        m_uiMushroomTimer   = urand(10000, 20000);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoSummonMushrooms(true);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_AMANITAR, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AMANITAR, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AMANITAR, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_POISONOUS_MUSHROOM)
            pSummoned->CastSpell(pSummoned, SPELL_POISON_MUSHROOM_VISUAL, true);
        else if (pSummoned->GetEntry() == NPC_HEALTHY_MUSHROOM)
            pSummoned->CastSpell(pSummoned, SPELL_POWER_MUSHROOM_VISUAL, true);

        // ToDo: research if the mushrooms should have a grow effect!
        pSummoned->CastSpell(pSummoned, SPELL_MUSHROOM_FORM, true);
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_POISONOUS_MUSHROOM)
            pSummoned->CastSpell(pSummoned, SPELL_POISON_CLOUD, true);
        else if (pSummoned->GetEntry() == NPC_HEALTHY_MUSHROOM)
            pSummoned->CastSpell(pSummoned, SPELL_POTENT_FUNGUS, true);
    }

    void DoSummonMushrooms(bool bIsFirstSummon)
    {
        // This implementation may not be 100% accurate;
        // On aggro boss summons about 20 mushrooms; On timer it summons about 5 mushrooms per turn
        // There is a 33% chance that the mushroom will be healthy
        // The summon position is based on the center of the area coords

        float fX, fY, fZ;
        uint32 uiMaxMushrooms = bIsFirstSummon ? 20 : 5;

        for (uint8 i = 0; i < uiMaxMushrooms; ++i)
        {
            uint32 uiMushroomEntry = roll_chance_i(33) ? NPC_HEALTHY_MUSHROOM : NPC_POISONOUS_MUSHROOM;
            m_creature->GetRandomPoint(aMushroomPos[0], aMushroomPos[1], aMushroomPos[2], 30.0f, fX, fY, fZ);
            m_creature->SummonCreature(uiMushroomEntry, fX, fY, fZ, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_BASH) == CAST_OK)
                m_uiBashTimer = urand(8000, 13000);
        }
        else
            m_uiBashTimer -= uiDiff;

        if (m_uiVenomBoltTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_VENOM_BOLT_VOLLEY) == CAST_OK)
                m_uiVenomBoltTimer = urand(15000, 20000);
        }
        else
            m_uiVenomBoltTimer -= uiDiff;

        if (m_uiRootsTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_ENTANGLING_ROOTS) == CAST_OK)
                    m_uiRootsTimer = urand(20000, 25000);
            }
        }
        else
            m_uiRootsTimer -= uiDiff;

        if (m_uiMiniTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MINI) == CAST_OK)
                m_uiMiniTimer = 30000;
        }
        else
            m_uiMiniTimer -= uiDiff;

        if (m_uiMushroomTimer < uiDiff)
        {
            DoSummonMushrooms(false);
            m_uiMushroomTimer = urand(10000, 20000);
        }
        else
            m_uiMushroomTimer -= uiDiff;

        // ToDo: Research if he requires out of combat area evade check

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_amanitar(Creature* pCreature)
{
    return new boss_amanitarAI(pCreature);
}

/*######
## npc_amanitar_mushroom
######*/

struct npc_amanitar_mushroomAI : public Scripted_NoMovementAI
{
    npc_amanitar_mushroomAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_amanitar_mushroom(Creature* pCreature)
{
    return new npc_amanitar_mushroomAI(pCreature);
}

void AddSC_boss_amanitar()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_amanitar";
    pNewScript->GetAI = &GetAI_boss_amanitar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_amanitar_mushroom";
    pNewScript->GetAI = &GetAI_npc_amanitar_mushroom;
    pNewScript->RegisterSelf();
}

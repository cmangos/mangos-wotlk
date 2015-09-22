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
SDName: boss_festergut
SD%Complete: 90%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
    SPELL_BERSERK               = 47008,

    // Gastric Bloat
    SPELL_GASTRIC_BLOAT         = 72214, // proc aura, ~8 sec cooldown, cooldown for Creature requires implementation in core

    // Inhale Blight
    SPELL_INHALE_BLIGHT         = 69165,
    SPELL_INHALED_BLIGHT_10     = 69166,
    SPELL_INHALED_BLIGHT_25     = 71912,

    // Pungent Blight
    SPELL_PUNGENT_BLIGHT        = 69195,

    // Gaseous Blight
    SPELL_GASEUS_BLIGHT_DUMMY   = 69125, // gas is spread into the room on aggro
    // periodic auras spells
    SPELL_GASEOUS_BLIGHT_1      = 69157,
    SPELL_GASEOUS_BLIGHT_2      = 69162,
    SPELL_GASEOUS_BLIGHT_3      = 69164,

    // visual gas dummy auras
    SPELL_GASEOUS_BLIGHT_DUMMY1 = 69126,
    SPELL_GASEOUS_BLIGHT_DUMMY2 = 69152,
    SPELL_GASEOUS_BLIGHT_DUMMY3 = 69154,

    // Inoculent
    SPELL_REMOVE_INOCULENT      = 69298,

    // Gas Spore
    SPELL_GAS_SPORE             = 69278,

    // Vile Gas
    SPELL_VILE_GAS_SUMMON       = 72288,
    SPELL_VILE_GAS              = 71307
};

enum
{
    SAY_AGGRO                   = -1631082,
    SAY_BLIGHT                  = -1631083,
    SAY_SPORE                   = -1631084,
    SAY_PUNGUENT_BLIGHT         = -1631085,
    SAY_PUNGUENT_BLIGHT_EMOTE   = -1631086,
    SAY_SLAY_1                  = -1631087,
    SAY_SLAY_2                  = -1631088,
    SAY_BERSERK                 = -1631089,
    SAY_DEATH                   = -1631090,
    SAY_FESTERGUT_DEATH         = -1631091,
};

struct boss_festergutAI : public ScriptedAI
{
    boss_festergutAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetMap()->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiBerserkTimer;
    uint32 m_uiGastricBloatTimer;
    uint32 m_uiInhaleBlightTimer;
    uint32 m_uiGasSporeTimer;
    uint32 m_uiVileGasTimer;

    void Reset() override
    {
        m_uiBerserkTimer = 5 * MINUTE * IN_MILLISECONDS;
        m_uiGastricBloatTimer = 10000;
        m_uiInhaleBlightTimer = 30000;
        m_uiGasSporeTimer = 20000;
        m_uiVileGasTimer = 10000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        DoCastSpellIfCan(m_creature, SPELL_GASTRIC_BLOAT, CAST_TRIGGERED); // not working as intended currently
        DoCastSpellIfCan(m_creature, SPELL_GASEOUS_BLIGHT_1, CAST_TRIGGERED); // DoT aura
        DoCastSpellIfCan(m_creature, SPELL_GASEUS_BLIGHT_DUMMY, CAST_TRIGGERED); // visual cast on dummy npc

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FESTERGUT, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FESTERGUT, FAIL);

        DoCastSpellIfCan(m_creature, SPELL_REMOVE_INOCULENT, CAST_TRIGGERED);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FESTERGUT, DONE);

        DoScriptText(SAY_DEATH, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_REMOVE_INOCULENT, CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Berserk
        if (m_uiBerserkTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
            {
                DoScriptText(SAY_BERSERK, m_creature);
                m_uiBerserkTimer = 5 * MINUTE * IN_MILLISECONDS;
            }
        }
        else
            m_uiBerserkTimer -= uiDiff;

        // Inhale Blight and Pungent Blight
        if (m_uiInhaleBlightTimer <= uiDiff)
        {
            SpellAuraHolder* holder = m_creature->GetSpellAuraHolder(SPELL_INHALED_BLIGHT_10);

            if (!holder)
                holder = m_creature->GetSpellAuraHolder(SPELL_INHALED_BLIGHT_25);

            // inhale the gas or if already have 3 stacks - release it
            if (holder && holder->GetStackAmount() >= 3)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_PUNGENT_BLIGHT) == CAST_OK)
                {
                    DoScriptText(SAY_PUNGUENT_BLIGHT_EMOTE, m_creature);
                    DoScriptText(SAY_PUNGUENT_BLIGHT, m_creature);
                    m_uiInhaleBlightTimer = 35000;
                }
            }
            else if (DoCastSpellIfCan(m_creature, SPELL_INHALE_BLIGHT) == CAST_OK)
            {
                if (m_pInstance)
                {
                    if (Creature* pProfessor = m_pInstance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                        DoScriptText(SAY_BLIGHT, pProfessor);
                }
                m_uiInhaleBlightTimer = 30000;
            }
        }
        else
            m_uiInhaleBlightTimer -= uiDiff;

        // Gas Spore
        if (m_uiGasSporeTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_GAS_SPORE) == CAST_OK)
            {
                DoScriptText(SAY_SPORE, m_creature);
                m_uiGasSporeTimer = 40000;
            }
        }
        else
            m_uiGasSporeTimer -= uiDiff;

        // Vile Gas
        if (m_uiVileGasTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_VILE_GAS_SUMMON, CAST_TRIGGERED) == CAST_OK)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_VILE_GAS) == CAST_OK)
                    m_uiVileGasTimer = 30000;
            }
        }
        else
            m_uiVileGasTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_festergut(Creature* pCreature)
{
    return new boss_festergutAI(pCreature);
}

void AddSC_boss_festergut()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_festergut";
    pNewScript->GetAI = &GetAI_boss_festergut;
    pNewScript->RegisterSelf();
}

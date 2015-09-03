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
SDName: Oculus
SD%Complete: 80
SDComment: Make use of the passenger boarding wrappers when supported by the core.
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"
#include "oculus.h"
#include "TemporarySummon.h"

enum
{
    EMOTE_FLY_AWAY                          = -1578030,

    SPELL_RIDE_RUBY_DRAKE_QUE               = 49463,
    SPELL_RIDE_EMERAL_DRAKE_QUE             = 49427,
    SPELL_RIDE_AMBER_DRAKE_QUE              = 49459,

    SPELL_DRAKE_FLAG_VISUAL                 = 53797,
    SPELL_PARACHUTE                         = 50550,                // triggers 50553
    SPELL_FLIGHT                            = 50296,
    SPELL_SOAR                              = 50325,
    SPELL_EVASIVE_AURA                      = 50248,
};

/*######
## npc_oculus_drake
######*/

struct npc_oculus_drakeAI : public ScriptedAI
{
    npc_oculus_drakeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        if (m_creature->IsTemporarySummon())
        {
            TemporarySummon* pTemporary = (TemporarySummon*)m_creature;

            uint32 uiMountSpell = 0;
            switch (m_creature->GetEntry())
            {
                case NPC_RUBY_DRAKE:    uiMountSpell = SPELL_RIDE_RUBY_DRAKE_QUE;   break;
                case NPC_AMBER_DRAKE:   uiMountSpell = SPELL_RIDE_AMBER_DRAKE_QUE;  break;
                case NPC_EMERALD_DRAKE: uiMountSpell = SPELL_RIDE_EMERAL_DRAKE_QUE; break;
            }

            // Force player to mount
            if (Player* pSummoner = m_creature->GetMap()->GetPlayer(pTemporary->GetSummonerGuid()))
            {
                pSummoner->CastSpell(pSummoner, uiMountSpell, true);

                // The dragon moves near the player after spawn
                float fX, fY, fZ;
                pSummoner->GetContactPoint(m_creature, fX, fY, fZ);
                m_creature->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
            }
        }

        SetCombatMovement(false);
        Reset();
    }

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_SOAR, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

        // Another aura for the Ruby drake
        if (m_creature->GetEntry() == NPC_RUBY_DRAKE)
            DoCastSpellIfCan(m_creature, SPELL_EVASIVE_AURA, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        // Handle player parachute
        if (m_creature->IsTemporarySummon())
        {
            TemporarySummon* pTemporary = (TemporarySummon*)m_creature;
            if (Player* pSummoner = m_creature->GetMap()->GetPlayer(pTemporary->GetSummonerGuid()))
            {
                pSummoner->RemoveAurasDueToSpell(SPELL_DRAKE_FLAG_VISUAL);
                pSummoner->CastSpell(pSummoner, SPELL_PARACHUTE, true);
            }
        }
    }

    // TODO: Temporary workaround - please remove when the boarding wrappers are implemented in core
    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pCaster->GetTypeId() != TYPEID_PLAYER)
            return;

        if (pSpell->Id == 49464 || pSpell->Id == 49346 || pSpell->Id == 49460)
            DoCastSpellIfCan(m_creature, SPELL_FLIGHT, CAST_TRIGGERED);
    }

    // TODO: Enable the wrappers below, when they will be properly supported by the core
    /*
    void PassengerBoarded(Unit* pPassenger, uint8 uiSeat) override
    {
        if (pPassenger->GetTypeId() != TYPEID_PLAYER)
            return;

        // Set vehicle auras
        DoCastSpellIfCan(m_creature, SPELL_FLIGHT, CAST_TRIGGERED);

        // Set passenger auras
        pPassenger->CastSpell(pPassenger, SPELL_DRAKE_FLAG_VISUAL, true);
    }

    void PassengerUnBoarded(Unit* pPassenger) override
    {
        pPassenger->RemoveAurasDueToSpell(SPELL_DRAKE_FLAG_VISUAL);
        pPassenger->CastSpell(pPassenger, SPELL_PARACHUTE, true);

        DoScriptText(EMOTE_FLY_AWAY, m_creature);

        // The dragon runs away and despawns
        float fX, fY, fZ;
        m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, 20, frand(0, 2 * M_PI_F));
        m_creature->GetMotionMaster()->MovePoint(0, fX, fY, fZ + 20.0f);
        m_creature->ForcedDespawn(5000);
    }
    */
};

CreatureAI* GetAI_npc_oculus_drake(Creature* pCreature)
{
    return new npc_oculus_drakeAI(pCreature);
}

void AddSC_oculus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_oculus_drake";
    pNewScript->GetAI = &GetAI_npc_oculus_drake;
    pNewScript->RegisterSelf();
}

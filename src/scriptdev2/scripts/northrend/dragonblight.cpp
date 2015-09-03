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
SDName: Dragonblight
SD%Complete: 100
SDComment: Quest support: 12075, 12166, 12261.
SDCategory: Dragonblight
EndScriptData */

/* ContentData
npc_destructive_ward
npc_crystalline_ice_giant
EndContentData */

#include "precompiled.h"

/*######
# npc_destructive_ward
#####*/

enum
{
    SAY_WARD_POWERUP                    = -1000664,
    SAY_WARD_CHARGED                    = -1000665,

    SPELL_DESTRUCTIVE_PULSE             = 48733,
    SPELL_DESTRUCTIVE_BARRAGE           = 48734,
    SPELL_DESTRUCTIVE_WARD_POWERUP      = 48735,

    SPELL_SUMMON_SMOLDERING_SKELETON    = 48715,
    SPELL_SUMMON_SMOLDERING_CONSTRUCT   = 48718,
    SPELL_DESTRUCTIVE_WARD_KILL_CREDIT  = 52409,

    MAX_STACK                           = 1,
};

// Script is based on real event from you-know-where.
// Some sources show the event in a bit different way, for unknown reason.
// Devs decided to add it in the below way, until more details can be obtained.

// It will be only two power-up's, where other sources has a different count (2-4 stacks has been observed)
// Probably caused by either a change in a patch (bugfix?) or the powerup has a condition (some
// sources suggest this, but without any explanation about what this might be)

struct npc_destructive_wardAI : public Scripted_NoMovementAI
{
    npc_destructive_wardAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_uiPowerTimer = 30000;
        m_uiStack = 0;
        m_uiSummonTimer = 2000;
        m_bCanPulse = false;
        m_bFirst = true;
        Reset();
    }

    uint32 m_uiPowerTimer;
    uint32 m_uiStack;
    uint32 m_uiSummonTimer;
    bool m_bFirst;
    bool m_bCanPulse;

    void Reset() override { }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->AI()->AttackStart(m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_bCanPulse)
        {
            if (DoCastSpellIfCan(m_creature, m_uiStack > MAX_STACK ? SPELL_DESTRUCTIVE_BARRAGE : SPELL_DESTRUCTIVE_PULSE) == CAST_OK)
                m_bCanPulse = false;
        }

        if (m_uiSummonTimer)
        {
            if (m_uiSummonTimer <= uiDiff)
            {
                if (m_bFirst)
                    m_uiSummonTimer = 25000;
                else
                    m_uiSummonTimer = 0;

                switch (m_uiStack)
                {
                    case 0:
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOLDERING_SKELETON, CAST_TRIGGERED);
                        break;
                    case 1:
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOLDERING_CONSTRUCT, CAST_TRIGGERED);

                        if (m_bFirst)
                            break;

                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOLDERING_CONSTRUCT, CAST_TRIGGERED);
                        break;
                    case 2:
                        if (m_bFirst)
                            break;

                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOLDERING_SKELETON, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOLDERING_SKELETON, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOLDERING_CONSTRUCT, CAST_TRIGGERED);
                        break;
                }

                m_bFirst = !m_bFirst;
            }
            else
                m_uiSummonTimer -= uiDiff;
        }

        if (!m_uiPowerTimer)
            return;

        if (m_uiPowerTimer <= uiDiff)
        {
            if (m_uiStack > MAX_STACK)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DESTRUCTIVE_WARD_KILL_CREDIT) == CAST_OK)
                {
                    DoScriptText(SAY_WARD_CHARGED, m_creature, m_creature->GetOwner());
                    m_uiPowerTimer = 0;
                    m_uiSummonTimer = 0;
                    m_bCanPulse = true;
                }
            }
            else if (DoCastSpellIfCan(m_creature, SPELL_DESTRUCTIVE_WARD_POWERUP) == CAST_OK)
            {
                DoScriptText(SAY_WARD_POWERUP, m_creature, m_creature->GetOwner());

                m_uiPowerTimer = 30000;
                m_uiSummonTimer = 2000;

                m_bFirst = true;
                m_bCanPulse = true;                         // pulse right after each charge

                ++m_uiStack;
            }
        }
        else
            m_uiPowerTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_destructive_ward(Creature* pCreature)
{
    return new npc_destructive_wardAI(pCreature);
}

/*######
## npc_crystalline_ice_giant
######*/

enum
{
    SPELL_FEIGN_DEATH_PERMANENT     = 31261,
    ITEM_ID_SAMPLE_ROCKFLESH        = 36765,
    NPC_CRYSTALLINE_GIANT           = 26809,
};

bool NpcSpellClick_npc_crystalline_ice_giant(Player* pPlayer, Creature* pClickedCreature, uint32 /*uiSpellId*/)
{
    if (pClickedCreature->GetEntry() == NPC_CRYSTALLINE_GIANT && pClickedCreature->HasAura(SPELL_FEIGN_DEATH_PERMANENT))
    {
        if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(ITEM_ID_SAMPLE_ROCKFLESH, 1))
        {
            pPlayer->SendNewItem(pItem, 1, true, false);
            pClickedCreature->ForcedDespawn();

            // always return true when handled special npc spell click
            return true;
        }
    }

    return true;
}

void AddSC_dragonblight()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_destructive_ward";
    pNewScript->GetAI = &GetAI_npc_destructive_ward;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_crystalline_ice_giant";
    pNewScript->pNpcSpellClick = &NpcSpellClick_npc_crystalline_ice_giant;
    pNewScript->RegisterSelf();
}

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
SDName: Boss_Silver_Hand_Bosses
SD%Complete: 80
SDComment: Timers; Not sure if we need to respawn dead npcs on evade; May need additional adjustments / research
SDCategory: Stratholme
EndScriptData */

#include "precompiled.h"
#include "stratholme.h"

/*#####
# Additional:
# Although this is a working solution, the correct would be in addition to check if Aurius is dead.
# Once player extinguish the eternal flame (cast spell 31497->start event 11206) Aurius should become hostile.
# Once Aurius is defeated, he should be the one summoning the ghosts.
#####*/

enum
{
    // Gregor
    SPELL_HAMMER_JUSTICE        = 13005,
    SPELL_HAMMER_WRATH          = 32772,
    SPELL_HOLY_SHOCK            = 32771,
    // Cathela
    SPELL_HOLY_SHIELD           = 32777,
    SPELL_REDOUBT               = 32776,
    // Aelmar
    SPELL_JUDGEMENT             = 32778,
    // Vicar
    SPELL_BLESSING              = 32770,
    SPELL_HOLY_LIGHT            = 32769,

    TARGET_TYPE_RANDOM          = 0,
    TARGET_TYPE_VICTIM          = 1,
    TARGET_TYPE_SELF            = 2,
    TARGET_TYPE_FRIENDLY        = 3,
};

struct SilverHandAbilityStruct
{
    uint32 m_uiCreatureEntry, m_uiSpellId;
    uint8 m_uiTargetType;
    uint32 m_uiInitialTimer, m_uiCooldown;
};

static SilverHandAbilityStruct m_aSilverHandAbility[8] =
{
    {NPC_GREGOR_THE_JUSTICIAR,  SPELL_HAMMER_JUSTICE,   TARGET_TYPE_RANDOM,     2000,  15000},
    {NPC_GREGOR_THE_JUSTICIAR,  SPELL_HAMMER_WRATH,     TARGET_TYPE_RANDOM,     10000, 15000},
    {NPC_GREGOR_THE_JUSTICIAR,  SPELL_HOLY_SHOCK,       TARGET_TYPE_RANDOM,     4000,  7000},
    {NPC_CATHELA_THE_SEEKER,    SPELL_HOLY_SHIELD,      TARGET_TYPE_SELF,       1000,  60000},
    {NPC_CATHELA_THE_SEEKER,    SPELL_REDOUBT,          TARGET_TYPE_SELF,       5000,  15000},
    {NPC_AELMAR_THE_VANQUISHER, SPELL_JUDGEMENT,        TARGET_TYPE_VICTIM,     4000,  9000},
    {NPC_VICAR_HYERONIMUS,      SPELL_BLESSING,         TARGET_TYPE_FRIENDLY,   2000,  13000},
    {NPC_VICAR_HYERONIMUS,      SPELL_HOLY_LIGHT,       TARGET_TYPE_FRIENDLY,   5000,  9000},
};
struct boss_silver_hand_bossesAI : public ScriptedAI
{
    boss_silver_hand_bossesAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        for (uint8 i = 0; i < countof(m_aSilverHandAbility); ++i)
        {
            if (m_aSilverHandAbility[i].m_uiCreatureEntry == m_creature->GetEntry())
                m_mSpellTimers[i] = m_aSilverHandAbility[i].m_uiInitialTimer;
        }
        Reset();
    }

    instance_stratholme* m_pInstance;

    UNORDERED_MAP<uint8, uint32> m_mSpellTimers;

    void Reset() override
    {
        for (UNORDERED_MAP<uint8, uint32>::iterator itr = m_mSpellTimers.begin(); itr != m_mSpellTimers.end(); ++itr)
            itr->second = m_aSilverHandAbility[itr->first].m_uiInitialTimer;
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
        {
            // Set data to special when each paladin dies
            m_pInstance->SetData(TYPE_TRUE_MASTERS, SPECIAL);

            // For the last one which dies, give the quest credit
            if (m_pInstance->GetData(TYPE_TRUE_MASTERS) == DONE)
            {
                if (pKiller->GetTypeId() == TYPEID_PLAYER)
                {
                    if (Creature* pCredit = m_pInstance->GetSingleCreatureFromStorage(NPC_PALADIN_QUEST_CREDIT))
                        ((Player*)pKiller)->KilledMonsterCredit(NPC_PALADIN_QUEST_CREDIT, pCredit->GetObjectGuid());
                }
            }
        }
    }

    bool CanUseSpecialAbility(uint32 uiIndex)
    {
        Unit* pTarget = NULL;

        switch (m_aSilverHandAbility[uiIndex].m_uiTargetType)
        {
            case TARGET_TYPE_SELF:
                pTarget = m_creature;
                break;
            case TARGET_TYPE_VICTIM:
                pTarget = m_creature->getVictim();
                break;
            case TARGET_TYPE_RANDOM:
                pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, m_aSilverHandAbility[uiIndex].m_uiSpellId, SELECT_FLAG_IN_LOS);
                break;
            case TARGET_TYPE_FRIENDLY:
                pTarget = DoSelectLowestHpFriendly(10.0f);
                break;
        }

        if (pTarget)
        {
            if (DoCastSpellIfCan(pTarget, m_aSilverHandAbility[uiIndex].m_uiSpellId) == CAST_OK)
                return true;
        }

        return false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        for (UNORDERED_MAP<uint8, uint32>::iterator itr = m_mSpellTimers.begin(); itr != m_mSpellTimers.end(); ++itr)
        {
            if (itr->second < uiDiff)
            {
                if (CanUseSpecialAbility(itr->first))
                {
                    itr->second = m_aSilverHandAbility[itr->first].m_uiCooldown;
                    break;
                }
            }
            else
                itr->second -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_silver_hand_bossesAI(Creature* pCreature)
{
    return new boss_silver_hand_bossesAI(pCreature);
}

void AddSC_boss_order_of_silver_hand()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_silver_hand_bosses";
    pNewScript->GetAI = &GetAI_boss_silver_hand_bossesAI;
    pNewScript->RegisterSelf();
}

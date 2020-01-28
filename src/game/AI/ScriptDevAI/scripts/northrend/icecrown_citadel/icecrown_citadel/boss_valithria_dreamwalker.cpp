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
SDName: boss_valithria
SD%Complete: 5%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"

enum SvalnaTexts                                            // TODO Maybe need own file?
{
    SAY_SVALNA_EVENT_1          = -1631130,
    SAY_SVALNA_EVENT_2          = -1631131,
    SAY_SVALNA_EVENT_3          = -1631132,
    SAY_SVALNA_EVENT_4          = -1631133,
    SAY_KILLING_CRUSADERS       = -1631134,
    SAY_RESSURECT               = -1631135,
    SAY_SVALNA_AGGRO            = -1631136,
    SAY_KILL_CAPTAIN            = -1631137,
    SAY_KILL_PLAYER             = -1631138,
    SAY_DEATH                   = -1631139,
};

enum
{
    SAY_AGGRO                   = -1631140,
    SAY_PORTAL                  = -1631141,
    SAY_75_HEALTH               = -1631142,
    SAY_25_HEALTH               = -1631143,
    SAY_0_HEALTH                = -1631144,
    SAY_PLAYER_DIES             = -1631145,
    SAY_BERSERK                 = -1631146,
    SAY_VICTORY                 = -1631147,
};

// Valithria
enum
{
    SPELL_NIGHTMARE_PORTAL_PRE      = 71977,
    SPELL_NIGHTMARE_PORTAL          = 71987,
    SPELL_TWISTED_NIGHTMARES        = 71941,
    SPELL_TWISTED_NIGHTMARES_DOT    = 71940,
    SPELL_NIGHTMARE_CLOUD           = 71970, // Nightmare Clouds cast this on dreaming Valithria - then she deals this dmg to Real Valithria(?)
    SPELL_NIGHTMARE_CLOUD_VISUAL    = 71939,

    SPELL_DREAM_PORTAL_PRE          = 71301,
    SPELL_DREAM_PORTAL              = 71305,
    SPELL_EMERALD_VIGOR             = 70873,
    SPELL_DREAM_CLOUD_VISUAL        = 70876,

    SPELL_DREAM_STATE               = 70766,

    SPELL_DREAMWALKER_RAGE          = 71189,
    SPELL_IMMUNITY                  = 72724,
    SPELL_CORRUPTION                = 70904,
    SPELL_DREAM_SLIP                = 71196,
    SPELL_ICE_SPIKE                 = 70702,

    SPELL_CLEAR_DREAMS_NIGHTMARES   = 75863, // script effect removes Emerald Vigor and Nightmare auras

    // summons
    // TODO:
    // these spells should be applied to dummy npcs in gates
    // they should apply these auras once the encounter has started
    // but how to apply this, which spell on which npc and when?
    // how to handle summon timers speedup?
    SPELL_SUMMON_TIMER_SUPPRESSER   = 70912,
    SPELL_SUMMON_TIMER_SKELETON     = 70913,
    SPELL_SUMMON_TIMER_ZOMBIE       = 70914,
    SPELL_SUMMON_TIMER_ABOMIN       = 70915,
    SPELL_SUMMON_TIMER_ARCHMAGE     = 70916,

    // entries
    NPC_NIGHTMARE_PORTAL_PRE        = 38429,
    NPC_NIGHTMARE_PORTAL            = 38430,
    NPC_NIGHTMARE_CLOUD             = 38421,
    NPC_DREAM_PORTAL_PRE            = 38186,
    NPC_DREAM_PORTAL                = 37945,
    NPC_DREAM_CLOUD                 = 37985,

    // Achievements
    SPELL_ACHIEVEMENT_CREDIT        = 72706,

    // other
    SUMMON_TYPES_NUMBER             = 4
};

struct boss_valithria_dreamwalkerAI : public ScriptedAI
{
    boss_valithria_dreamwalkerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    void Reset() override
    {
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VALITHRIA, FAIL);
    }

    // actually, when summoned creature kills a player
    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(SAY_PLAYER_DIES, m_creature, pVictim);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_0_HEALTH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VALITHRIA, FAIL);
    }

    void UpdateAI(const uint32 /*uiDiff*/) override
    {
    }
};

UnitAI* GetAI_boss_valithria_dreamwalker(Creature* pCreature)
{
    return new boss_valithria_dreamwalkerAI(pCreature);
};

void AddSC_boss_valithria_dreamwalker()
{
    Script* pNewscript = new Script;
    pNewscript->Name = "boss_valithria_dreamwalker";
    pNewscript->GetAI = &GetAI_boss_valithria_dreamwalker;
    pNewscript->RegisterSelf();
}

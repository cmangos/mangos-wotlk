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
SDName: Boss_Moroes
SD%Complete: 95
SDComment: Timers.
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "karazhan.h"

enum
{
    MAX_ACTIVE_GUESTS   = 4,
    MAX_GUESTS          = 6,

    SAY_AGGRO           = -1532011,
    SAY_SPECIAL_1       = -1532012,
    SAY_SPECIAL_2       = -1532013,
    SAY_KILL_1          = -1532014,
    SAY_KILL_2          = -1532015,
    SAY_KILL_3          = -1532016,
    SAY_DEATH           = -1532017,

    SPELL_VANISH        = 29448,
    SPELL_GARROTE       = 37066,
    SPELL_BLIND         = 34694,
    SPELL_GOUGE         = 29425,
    SPELL_FRENZY        = 37023
};

static const float afLocations[MAX_ACTIVE_GUESTS][4] =
{
    { -10991.0f, -1884.33f, 81.73f, 0.614315f},
    { -10989.4f, -1885.88f, 81.73f, 0.904913f},
    { -10978.1f, -1887.07f, 81.73f, 2.035550f},
    { -10975.9f, -1885.81f, 81.73f, 2.253890f}
};

static const uint32 auiGuests[MAX_GUESTS] =
{
    NPC_LADY_KEIRA_BERRYBUCK,
    NPC_LADY_CATRIONA_VON_INDI,
    NPC_LORD_CRISPIN_FERENCE,
    NPC_BARON_RAFE_DREUGER,
    NPC_BARONESS_DOROTHEA_MILLSTIPE,
    NPC_LORD_ROBIN_DARIS
};

struct boss_moroesAI : public ScriptedAI
{
    boss_moroesAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance  = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    std::vector<uint32> m_vGuestsEntryList;

    uint32 m_uiVanishTimer;
    uint32 m_uiBlindTimer;
    uint32 m_uiGougeTimer;
    uint32 m_uiWaitTimer;

    bool m_bEnrage;

    void Reset() override
    {
        m_uiVanishTimer     = 30000;
        m_uiBlindTimer      = 35000;
        m_uiGougeTimer      = 23000;
        m_uiWaitTimer       = 0;

        m_bEnrage           = false;

        DoSpawnGuests();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MOROES, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void JustReachedHome() override
    {
        DoRemoveGarroteAura();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MOROES, FAIL);
    }

    void JustDied(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        DoRemoveGarroteAura();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MOROES, DONE);
    }

    void EnterEvadeMode() override
    {
        // Don't evade during vanish phase
        if (m_uiWaitTimer)
            return;

        ScriptedAI::EnterEvadeMode();
    }

    void DoSpawnGuests()
    {
        // not if m_creature are dead, so avoid
        if (!m_creature->isAlive())
            return;

        // it's empty, so first time
        if (m_vGuestsEntryList.empty())
        {
            // pre-allocate size for speed
            m_vGuestsEntryList.resize(MAX_GUESTS);

            // fill vector array with entries from creature array
            for (uint8 i = 0; i < MAX_GUESTS; ++i)
                m_vGuestsEntryList[i] = auiGuests[i];

            std::random_shuffle(m_vGuestsEntryList.begin(), m_vGuestsEntryList.end());

            // Summon the 4 entries
            for (uint8 i = 0; i < MAX_ACTIVE_GUESTS; ++i)
                m_creature->SummonCreature(m_vGuestsEntryList[i], afLocations[i][0], afLocations[i][1], afLocations[i][2], afLocations[i][3], TEMPSUMMON_CORPSE_DESPAWN, 0);
        }
        // Resummon the killed adds
        else
        {
            if (!m_pInstance)
                return;

            for (uint8 i = 0; i < MAX_ACTIVE_GUESTS; ++i)
            {
                // If we already have the creature on the map, then don't summon it
                if (m_pInstance->GetSingleCreatureFromStorage(m_vGuestsEntryList[i], true))
                    continue;

                m_creature->SummonCreature(m_vGuestsEntryList[i], afLocations[i][0], afLocations[i][1], afLocations[i][2], afLocations[i][3], TEMPSUMMON_CORPSE_DESPAWN, 0);
            }
        }
    }

    // Wrapper to remove the Garrote aura on death and on evade - ToDo: maybe find a better way for this!
    void DoRemoveGarroteAura()
    {
        // remove aura from spell Garrote when Moroes dies
        Map* pMap = m_creature->GetMap();
        if (pMap->IsDungeon())
        {
            Map::PlayerList const& PlayerList = pMap->GetPlayers();

            if (PlayerList.isEmpty())
                return;

            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (i->getSource()->isAlive() && i->getSource()->HasAura(SPELL_GARROTE))
                    i->getSource()->RemoveAurasDueToSpell(SPELL_GARROTE);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Note: because the Vanish spell adds invisibility effect on the target, the timers won't be decreased during the vanish phase
        if (m_uiWaitTimer)
        {
            if (m_uiWaitTimer <= uiDiff)
            {
                // It's not very clear how to handle this spell
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    DoScriptText(urand(0, 1) ? SAY_SPECIAL_1 : SAY_SPECIAL_2, m_creature);
                    DoResetThreat();
                    AttackStart(pTarget);
                    pTarget->CastSpell(pTarget, SPELL_GARROTE, true);
                }
                m_uiWaitTimer = 0;
            }
            else
                m_uiWaitTimer -= uiDiff;

            // Don't user other abilities in vanish
            return;
        }

        if (!m_bEnrage && m_creature->GetHealthPercent() < 30.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
                m_bEnrage = true;
        }

        // No other spells are cast after enrage
        if (!m_bEnrage)
        {
            if (m_uiVanishTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_VANISH) == CAST_OK)
                {
                    m_uiVanishTimer = 30000;
                    m_uiWaitTimer   = 1000;
                }
            }
            else
                m_uiVanishTimer -= uiDiff;

            // Gouge highest aggro, and attack second highest
            if (m_uiGougeTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_GOUGE) == CAST_OK)
                    m_uiGougeTimer = 40000;
            }
            else
                m_uiGougeTimer -= uiDiff;

            if (m_uiBlindTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_BLIND, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_BLIND) == CAST_OK)
                        m_uiBlindTimer = 40000;
                }
            }
            else
                m_uiBlindTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_moroes(Creature* pCreature)
{
    return new boss_moroesAI(pCreature);
}

void AddSC_boss_moroes()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_moroes";
    pNewScript->GetAI = &GetAI_boss_moroes;
    pNewScript->RegisterSelf();
}

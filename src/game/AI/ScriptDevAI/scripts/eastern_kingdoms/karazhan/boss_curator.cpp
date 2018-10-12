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
SDName: Boss_Curator
SD%Complete: 90%
SDComment:
SDCategory: Karazhan
EndScriptData */

/* Pre-nerf Changes
Add Astral Armor ability - present whenever Curator is not evocating or enraged.

Patches
Removed sometime prior to 2.1.
*/

#include "AI/ScriptDevAI/include/precompiled.h"
#include "karazhan.h"
#include "Spells/Spell.h"
#include "AI/ScriptDevAI/base/TimerAI.h"

enum
{
    SAY_AGGRO                   = -1532057,
    SAY_SUMMON1                 = -1532058,
    SAY_SUMMON2                 = -1532059,
    SAY_EVOCATE                 = -1532060,
    SAY_ENRAGE                  = -1532061,
    SAY_KILL1                   = -1532062,
    SAY_KILL2                   = -1532063,
    SAY_DEATH                   = -1532064,

    // Flare
    NPC_ASTRAL_FLARE_1          = 17096,
    NPC_ASTRAL_FLARE_2          = 19781,
    NPC_ASTRAL_FLARE_3          = 19782,
    NPC_ASTRAL_FLARE_4          = 19783,

    SPELL_ASTRAL_FLARE_NE       = 30236,
    SPELL_ASTRAL_FLARE_NW       = 30239,
    SPELL_ASTRAL_FLARE_SE       = 30240,
    SPELL_ASTRAL_FLARE_SW       = 30241,

    SPELL_ASTRAL_FLARE_PASSIVE  = 30234,
    SPELL_ASTRAL_FLARE_VISUAL   = 30237,

    // The Curator
    SPELL_HATEFUL_BOLT          = 30383,
    SPELL_EVOCATION             = 30254,
    SPELL_ARCANE_INFUSION       = 30403,
    SPELL_BERSERK               = 26662,

    SPELL_ASTRAL_ARMOR          = 29476,

    CURATOR_PHASE_1 = 0,
    CURATOR_PHASE_2 = 1,
};

static uint32 astralFlareSpells[]{ SPELL_ASTRAL_FLARE_NE, SPELL_ASTRAL_FLARE_NW, SPELL_ASTRAL_FLARE_SE, SPELL_ASTRAL_FLARE_SW };

enum CuratorActions
{
    CURATOR_ACTION_BERSERK,
    CURATOR_ACTION_ARCANE_INFUSION,
    CURATOR_ACTION_FLARE,
    CURATOR_ACTION_HATEFUL_BOLT,
    CURATOR_ACTION_MAX,
};

struct boss_curatorAI : public ScriptedAI, public CombatTimerAI
{
    boss_curatorAI(Creature* pCreature) : ScriptedAI(pCreature), CombatTimerAI(CURATOR_ACTION_MAX)
    {
        m_pInstance  = (ScriptedInstance*)pCreature->GetInstanceData();
        AddCombatAction(CURATOR_ACTION_BERSERK, 0);
        AddCombatAction(CURATOR_ACTION_FLARE, 0);
        AddCombatAction(CURATOR_ACTION_HATEFUL_BOLT, 0);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    GuidVector m_sparkGuids;
    uint8 m_phase;

    void Reset() override
    {
        for (uint32 i = 0; i < CURATOR_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(CURATOR_ACTION_BERSERK, GetInitialActionTimer(CURATOR_ACTION_BERSERK));
        ResetTimer(CURATOR_ACTION_FLARE, GetInitialActionTimer(CURATOR_ACTION_FLARE));
        ResetTimer(CURATOR_ACTION_HATEFUL_BOLT, GetInitialActionTimer(CURATOR_ACTION_HATEFUL_BOLT));
        SetActionReadyStatus(CURATOR_ACTION_ARCANE_INFUSION, true);

        m_creature->ApplySpellImmune(nullptr, IMMUNITY_EFFECT, SPELL_EFFECT_POWER_DRAIN, true);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_EFFECT, SPELL_EFFECT_POWER_BURN, true);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_DISPEL, DISPEL_POISON, true);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ARCANE, true);

        m_phase = CURATOR_PHASE_1;
    }

    uint32 GetInitialActionTimer(uint32 id)
    {
        switch (id)
        {
            case CURATOR_ACTION_BERSERK: return 10 * MINUTE * IN_MILLISECONDS;
            case CURATOR_ACTION_FLARE: return 10000;
            case CURATOR_ACTION_HATEFUL_BOLT: return 15000;
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(uint32 id)
    {
        switch (id)
        {
            case CURATOR_ACTION_FLARE: return 10000;
            case CURATOR_ACTION_HATEFUL_BOLT: return GetActionReadyStatus(CURATOR_ACTION_ARCANE_INFUSION) ? 15000 : 7000;
            default: return 0; // never occurs but for compiler
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_CURATOR, DONE);

        m_sparkGuids.clear();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_CURATOR, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CURATOR, FAIL);

        for (ObjectGuid guid : m_sparkGuids)
            if (Creature* spark = m_creature->GetMap()->GetCreature(guid))
                spark->ForcedDespawn();

        m_sparkGuids.clear();
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_ASTRAL_FLARE_1:
            case NPC_ASTRAL_FLARE_2:
            case NPC_ASTRAL_FLARE_3:
            case NPC_ASTRAL_FLARE_4:
            {
                switch (urand(0, 3))
                {
                    case 0: DoScriptText(SAY_SUMMON1, m_creature); break;
                    case 1: DoScriptText(SAY_SUMMON2, m_creature); break;
                }

                // Flare start with aggro on it's target, should be immune to arcane
                pSummoned->AI()->DoCastSpellIfCan(pSummoned, SPELL_ASTRAL_FLARE_PASSIVE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
                pSummoned->AI()->DoCastSpellIfCan(pSummoned, SPELL_ASTRAL_FLARE_VISUAL, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
                pSummoned->AI()->SetMeleeEnabled(false);

                pSummoned->SetInCombatWithZone();
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    pSummoned->AddThreat(pTarget, 100000.f);
                m_sparkGuids.push_back(pSummoned->GetObjectGuid());
                break;
            }
        }
    }

    void ExecuteActions() override
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < CURATOR_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case CURATOR_ACTION_BERSERK:
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                        {
                            // ScriptText needs confirmation
                            DoScriptText(SAY_ENRAGE, m_creature);
                            SetActionReadyStatus(i, false); // once per fight
                        }
                        continue;
                    }
                    case CURATOR_ACTION_ARCANE_INFUSION:
                    {
                        if (m_creature->GetHealthPercent() < 15.0f)
                        {
                            if (DoCastSpellIfCan(m_creature, SPELL_ARCANE_INFUSION) == CAST_OK)
                            {
                                DoScriptText(SAY_ENRAGE, m_creature);
                                SetActionReadyStatus(i, false); // once per fight
                                m_phase = CURATOR_PHASE_2;
                            }
                        }
                        continue;
                    }
                    case CURATOR_ACTION_FLARE:
                    {
                        if (m_phase != CURATOR_PHASE_1)
                            continue;

                        // summon Astral Flare
                        uint32 randomSpell = astralFlareSpells[urand(0, 3)];
                        DoCastSpellIfCan(nullptr, randomSpell);
                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(randomSpell);
                        // supposed to happen right away not on next oom
                        if (m_creature->GetPower((Powers)spellInfo->powerType) < Spell::CalculatePowerCost(spellInfo, m_creature))
                        {
                            if (DoCastSpellIfCan(m_creature, SPELL_EVOCATION) == CAST_OK)
                            {
                                DoScriptText(SAY_EVOCATE, m_creature);
                                // m_creature->RemoveAurasDueToSpell(SPELL_ASTRAL_ARMOR);
                            }
                        }
                        continue;
                    }
                    case CURATOR_ACTION_HATEFUL_BOLT:
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 1, nullptr, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_HATEFUL_BOLT) == CAST_OK)
                            {
                                ResetTimer(i, GetSubsequentActionTimer(i));
                                SetActionReadyStatus(i, false);
                                return;
                            }
                        }
                        continue;
                    }
                }
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        UpdateTimers(diff, m_creature->isInCombat());
        ExecuteActions();

        // if (!m_creature->HasAura(SPELL_ASTRAL_ARMOR))
        //     m_creature->CastSpell(m_creature, SPELL_ASTRAL_ARMOR, TRIGGERED_NONE);

	    DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_curator(Creature* pCreature)
{
    return new boss_curatorAI(pCreature);
}

void AddSC_boss_curator()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_curator";
    pNewScript->GetAI = &GetAI_boss_curator;
    pNewScript->RegisterSelf();
}

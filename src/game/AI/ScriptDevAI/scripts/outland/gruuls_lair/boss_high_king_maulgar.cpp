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
SDName: Boss_High_King_Maulgar
SD%Complete: 80
SDComment: Verify that the script is working properly
SDCategory: Gruul's Lair
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "gruuls_lair.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                   = -1565000,
    SAY_ENRAGE                  = -1565001,
    SAY_OGRE_DEATH1             = -1565002,
    SAY_OGRE_DEATH2             = -1565003,
    SAY_OGRE_DEATH3             = -1565004,
    SAY_OGRE_DEATH4             = -1565005,
    SAY_SLAY1                   = 20072,
    SAY_SLAY2                   = 20073,
    SAY_SLAY3                   = 20074,
    SAY_DEATH                   = -1565009,

    // High King Maulgar Spells
    SPELL_ARCING_SMASH          = 39144,
    SPELL_MIGHTY_BLOW           = 33230,
    SPELL_WHIRLWIND             = 33238,
    SPELL_FLURRY                = 33232,
    SPELL_CHARGE                = 26561,
    SPELL_FEAR                  = 16508,

    // Olm the Summoner Spells
    SPELL_DARK_DECAY            = 33129,
    SPELL_DEATH_COIL            = 33130,
    SPELL_SUMMON_WILD_FELHUNTER = 33131,

    // Kiggler the Crazed Spells
    SPELL_GREATER_POLYMORPH     = 33173,
    SPELL_LIGHTNING_BOLT        = 36152,
    SPELL_ARCANE_SHOCK          = 33175,
    SPELL_ARCANE_EXPLOSION      = 33237,

    // Blindeye the Seer Spells
    SPELL_GREATER_PW_SHIELD     = 33147,
    SPELL_HEAL                  = 33144,
    SPELL_PRAYEROFHEALING       = 33152,

    // Krosh Firehand Spells
    SPELL_GREATER_FIREBALL      = 33051,
    SPELL_SPELLSHIELD           = 33054,
    SPELL_BLAST_WAVE            = 33061,
};

enum MaulgarActions
{
    MAULGAR_PHASE_2,
    MAULGAR_ARCING_SMASH,
    MAULGAR_MIGHTY_BLOW,
    MAULGAR_WHIRLWIND,
    MAULGAR_CHARGE,
    MAULGAR_FEAR,
    MAULGAR_ACTION_MAX,
};

// High King Maulgar AI
struct boss_high_king_maulgarAI : public CombatAI
{
    boss_high_king_maulgarAI(Creature* creature) : CombatAI(creature, MAULGAR_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(MAULGAR_PHASE_2, true);
        AddCombatAction(MAULGAR_ARCING_SMASH, 8000, 14000);
        AddCombatAction(MAULGAR_MIGHTY_BLOW, 15000, 25000);
        AddCombatAction(MAULGAR_WHIRLWIND, 30000u);
        AddCombatAction(MAULGAR_CHARGE, true);
        AddCombatAction(MAULGAR_FEAR, true);
        AddOnKillText(SAY_SLAY1, SAY_SLAY2, SAY_SLAY3);
    }

    ScriptedInstance* m_instance;

    uint32 m_uiCouncilDeathCount;

    void Reset() override
    {
        CombatAI::Reset();
        m_uiCouncilDeathCount = 0;
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MAULGAR_EVENT, FAIL);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        // Set data to Special on Death
        if (m_instance)
            m_instance->SetData(TYPE_MAULGAR_EVENT, SPECIAL);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_MAULGAR_EVENT, IN_PROGRESS);
    }

    void EventCouncilDeath()
    {
        switch (++m_uiCouncilDeathCount)
        {
            case 1: DoScriptText(SAY_OGRE_DEATH1, m_creature); break;
            case 2: DoScriptText(SAY_OGRE_DEATH2, m_creature); break;
            case 3: DoScriptText(SAY_OGRE_DEATH3, m_creature); break;
            case 4: DoScriptText(SAY_OGRE_DEATH4, m_creature); break;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MAULGAR_PHASE_2:
                if (m_creature->GetHealthPercent() < 50.0f)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_FLURRY) == CAST_OK)
                    {
                        DoScriptText(SAY_ENRAGE, m_creature);
                        SetActionReadyStatus(action, false);
                        ResetCombatAction(MAULGAR_FEAR, urand(10000, 25000));
                        ResetCombatAction(MAULGAR_CHARGE, 2000);
                    }
                }
                break;
            case MAULGAR_ARCING_SMASH:
                if (DoCastSpellIfCan(nullptr, SPELL_ARCING_SMASH) == CAST_OK)
                    ResetCombatAction(action, urand(8000, 12000));
                break;
            case MAULGAR_MIGHTY_BLOW:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MIGHTY_BLOW) == CAST_OK)
                    ResetCombatAction(action, urand(20000, 35000));
                break;
            case MAULGAR_WHIRLWIND:
                if (DoCastSpellIfCan(nullptr, SPELL_WHIRLWIND) == CAST_OK)
                    ResetCombatAction(action, urand(30000, 40000));
                break;
            case MAULGAR_CHARGE:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_CHARGE, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_CHARGE) == CAST_OK)
                        ResetCombatAction(action, urand(14000, 20000));
                break;
            case MAULGAR_FEAR:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FEAR) == CAST_OK)
                    ResetCombatAction(action, urand(20000, 35000));
                break;
        }
    }
};

// Base AI for every council member
struct Council_Base_AI : public CombatAI
{
    Council_Base_AI(Creature* creature, uint32 actions) : CombatAI(creature, actions), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())) { }

    ScriptedInstance* m_instance;

    void JustDied(Unit* /*pVictim*/) override
    {
        if (!m_instance)
            return;

        Creature* pMaulgar = m_instance->GetSingleCreatureFromStorage(NPC_MAULGAR);
        if (pMaulgar && pMaulgar->IsAlive())
        {
            if (boss_high_king_maulgarAI* pMaulgarAI = dynamic_cast<boss_high_king_maulgarAI*>(pMaulgar->AI()))
                pMaulgarAI->EventCouncilDeath();
        }

        // Set data to Special on Death
        m_instance->SetData(TYPE_MAULGAR_EVENT, SPECIAL);
    }
};

enum OlmActions
{
    OLM_DARK_DECAY,
    OLM_DEATH_COIL,
    OLM_SUMMON,
    OLM_ACTION_MAX,
};

// Olm The Summoner AI
struct boss_olm_the_summonerAI : public Council_Base_AI
{
    boss_olm_the_summonerAI(Creature* creature) : Council_Base_AI(creature, OLM_ACTION_MAX)
    {
        AddCombatAction(OLM_DARK_DECAY, 18000u);
        AddCombatAction(OLM_DEATH_COIL, 14000u);
        AddCombatAction(OLM_SUMMON, 10000u);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case OLM_DARK_DECAY:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DARK_DECAY) == CAST_OK)
                    ResetCombatAction(action, 5000);
                break;
            case OLM_DEATH_COIL:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DEATH_COIL) == CAST_OK)
                    ResetCombatAction(action, urand(8000, 13000));
                break;
            case OLM_SUMMON:
                if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_WILD_FELHUNTER) == CAST_OK)
                    ResetCombatAction(action, urand(25000, 35000));
                break;
        }
    }
};

enum KigglerActions
{
    KIGGLER_ACTION_GREATER_POLYMORPH,
    KIGGLER_ACTION_ARCANE_EXPLOSION,
    KIGGLER_ACTION_ARCANE_SHOCK,
    KIGGLER_ACTION_LIGHTNING_BOLT,
    KIGGLER_ACTION_MAX,
};

// Kiggler The Crazed AI
struct boss_kiggler_the_crazedAI : public Council_Base_AI
{
    boss_kiggler_the_crazedAI(Creature* creature) : Council_Base_AI(creature, KIGGLER_ACTION_MAX)
    {
        AddCombatAction(KIGGLER_ACTION_GREATER_POLYMORPH, 5000u);
        AddCombatAction(KIGGLER_ACTION_ARCANE_EXPLOSION, 30000u);
        AddCombatAction(KIGGLER_ACTION_ARCANE_SHOCK, 20000u);
        AddCombatAction(KIGGLER_ACTION_LIGHTNING_BOLT, 0u);
        AddMainSpell(SPELL_LIGHTNING_BOLT);
        SetRangedMode(true, 35.f, TYPE_PROXIMITY);
    }

    void SpellHitTarget(Unit* pVictim, const SpellEntry* pSpell) override
    {
        // Spell currently not supported by core. Knock back effect should lower threat
        // Workaround in script:
        if (pSpell->Id == SPELL_ARCANE_EXPLOSION)
        {
            if (!pVictim || pVictim->GetTypeId() != TYPEID_PLAYER)
                return;

            if (m_creature->getThreatManager().getThreat(pVictim))
                m_creature->getThreatManager().modifyThreatPercent(pVictim, -75);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KIGGLER_ACTION_GREATER_POLYMORPH:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_GREATER_POLYMORPH) == CAST_OK)
                    ResetCombatAction(action, 11000);
                return;
            case KIGGLER_ACTION_ARCANE_EXPLOSION:
                if (m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, SPELL_ARCANE_EXPLOSION, SELECT_FLAG_PLAYER | SELECT_FLAG_USE_EFFECT_RADIUS))
                    if (DoCastSpellIfCan(nullptr, SPELL_ARCANE_EXPLOSION) == CAST_OK)
                        ResetCombatAction(action, 30000);
                return;
            case KIGGLER_ACTION_ARCANE_SHOCK:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ARCANE_SHOCK) == CAST_OK)
                    ResetCombatAction(action, urand(15000, 20000));
                return;
            case KIGGLER_ACTION_LIGHTNING_BOLT:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_LIGHTNING_BOLT) == CAST_OK)
                    ResetCombatAction(action, GetCurrentRangedMode() ? urand(2000, 3000) : urand(5000, 8000));
            break;
        }
    }
};

enum BlindeyeActions
{
    BLINDEYE_GREATER_POWER_WORD_SHIELD,
    BLINDEYE_HEAL,
    BLINDEYE_ACTION_MAX,
};

// Blindeye The Seer AI
struct boss_blindeye_the_seerAI : public Council_Base_AI
{
    boss_blindeye_the_seerAI(Creature* creature) : Council_Base_AI(creature, BLINDEYE_ACTION_MAX)
    {
        AddCombatAction(BLINDEYE_GREATER_POWER_WORD_SHIELD, 30000u);
        AddCombatAction(BLINDEYE_HEAL, 25000, 40000);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case BLINDEYE_GREATER_POWER_WORD_SHIELD:
                if (DoCastSpellIfCan(nullptr, SPELL_GREATER_PW_SHIELD) == CAST_OK)
                {
                    DoCastSpellIfCan(nullptr, SPELL_PRAYEROFHEALING);
                    ResetCombatAction(action, urand(30000, 40000));
                }
                break;
            case BLINDEYE_HEAL:
                if (Unit* target = DoSelectLowestHpFriendly(50.0f))
                    if (DoCastSpellIfCan(target, SPELL_HEAL) == CAST_OK)
                        ResetCombatAction(action, urand(15000, 40000));
                break;
        }
    }
};

enum KroshActions
{
    KROSH_BLAST_WAVE,
    KROSH_SPELL_SHIELD,
    KROSH_GREATER_FIREBALL,
    KROSH_ACTION_MAX,
};

// Krosh Firehand AI
struct boss_krosh_firehandAI : public Council_Base_AI
{
    boss_krosh_firehandAI(Creature* creature) : Council_Base_AI(creature, KROSH_ACTION_MAX)
    {
        AddCombatAction(KROSH_BLAST_WAVE, 4000u);
        AddCombatAction(KROSH_SPELL_SHIELD, 1000u);
        AddCombatAction(KROSH_GREATER_FIREBALL, 12000u);
        SetRangedMode(true, 30.f, TYPE_FULL_CASTER);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KROSH_BLAST_WAVE:
                if (m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_USE_EFFECT_RADIUS))
                    if (DoCastSpellIfCan(nullptr, SPELL_BLAST_WAVE) == CAST_OK)
                        ResetCombatAction(action, 6000);
                break;
            case KROSH_SPELL_SHIELD:
                if (DoCastSpellIfCan(nullptr, SPELL_SPELLSHIELD) == CAST_OK)
                    ResetCombatAction(action, 30000);
                break;
            case KROSH_GREATER_FIREBALL:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_GREATER_FIREBALL) == CAST_OK)
                    ResetCombatAction(action, 3200);
                break;
        }
    }
};

void AddSC_boss_high_king_maulgar()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_high_king_maulgar";
    pNewScript->GetAI = &GetNewAIInstance<boss_high_king_maulgarAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_kiggler_the_crazed";
    pNewScript->GetAI = &GetNewAIInstance<boss_kiggler_the_crazedAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_blindeye_the_seer";
    pNewScript->GetAI = &GetNewAIInstance<boss_blindeye_the_seerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_olm_the_summoner";
    pNewScript->GetAI = &GetNewAIInstance<boss_olm_the_summonerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_krosh_firehand";
    pNewScript->GetAI = &GetNewAIInstance<boss_krosh_firehandAI>;
    pNewScript->RegisterSelf();
}

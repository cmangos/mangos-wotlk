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
SDName: Boss_Halazzi
SD%Complete: 90
SDComment: A few details and timers need check.
SDCategory: Zul'Aman
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "zulaman.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                       = -1568034,
    SAY_SPLIT                       = -1568035,
    SAY_MERGE                       = -1568036,
    SAY_SABERLASH1                  = -1568037,
    SAY_SABERLASH2                  = -1568038,
    SAY_BERSERK                     = -1568039,
    SAY_KILL1                       = -1568040,
    SAY_KILL2                       = -1568041,
    SAY_DEATH                       = -1568042,
    SAY_EVENT1                      = -1568043,
    SAY_EVENT2                      = -1568044,

    // generic spells
    SPELL_BERSERK                   = 45078,
    SPELL_TRANSFORM_TO_ORIGINAL     = 43311,
    // SPELL_DUAL_WIELD              = 42459,            // spell not confirmed
    // SPELL_TRANSFIGURE             = 44054,            // purpose unk

    // Phase single spells
    SPELL_SABER_LASH                = 43267,
    SPELL_FRENZY                    = 43139,

    // Phase switch spells
    SPELL_HALAZZI_TRANSFORM_SUMMON  = 43143,            // summons 24143
    SPELL_TRANSFIGURE_TO_TROLL      = 43142,            // triggers 43573
    SPELL_TRANSFIGURE_TRANSFORM     = 43573,

    SPELL_TRANSFORM_TO_LYNX_75      = 43145,
    SPELL_TRANSFORM_TO_LYNX_50      = 43271,
    SPELL_TRANSFORM_TO_LYNX_25      = 43272,
    SPELL_HALAZZI_TRANSFORM_DUMMY   = 43615,
    // SPELL_HALAZZI_TRANSFORM_VISUAL= 43293,

    // Phase spirits spells
    SPELL_FLAMESHOCK                = 43303,
    SPELL_EARTHSHOCK                = 43305,
    SPELL_LIGHTNING_TOTEM           = 43302,            // summons 24224

	// Totem spell
	SPELL_LIGHTNING					= 43301,

    NPC_HALAZZI_TROLL               = 24144,            // dummy creature - used to update stats
    NPC_SPIRIT_LYNX                 = 24143,
};

enum HalazziPhase
{
    PHASE_SINGLE        = 0,
    PHASE_TOTEM         = 1,
    PHASE_FINAL         = 2
};

enum HalazziActions
{
    HALAZZI_ACTION_BERSERK,
    HALAZZI_ACTION_TRANSFORM,
    HALAZZI_ACTION_FRENZY,
    HALAZZI_ACTION_SABER_LASH,
    HALAZZI_ACTION_SHOCK,
    HALAZZI_ACTION_TOTEM,
    HALAZZI_ACTION_MAX,
};

struct boss_halazziAI : public CombatAI
{
    boss_halazziAI(Creature* creature) : CombatAI(creature, HALAZZI_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddCombatAction(HALAZZI_ACTION_BERSERK, uint32(10 * MINUTE * IN_MILLISECONDS));
        AddTimerlessCombatAction(HALAZZI_ACTION_TRANSFORM, true);
        AddCombatAction(HALAZZI_ACTION_FRENZY, 16000u);
        AddCombatAction(HALAZZI_ACTION_SABER_LASH, 20000u);
        AddCombatAction(HALAZZI_ACTION_SHOCK, true);
        AddCombatAction(HALAZZI_ACTION_TOTEM, true);
        m_creature->GetCombatManager().SetLeashingCheck([](Unit*, float x, float y, float z)
            {
                return x < 307.f || y < 1055.f;
            });
        AddOnKillText(SAY_KILL1, SAY_KILL2);
        Reset();
    }

    ScriptedInstance* m_instance;

    HalazziPhase m_phase;

    uint32 m_phaseCounter;

    ObjectGuid m_spiritLynxGuid;
    GuidVector m_spawns;

    void Reset() override
    {
        CombatAI::Reset();
        m_phase           = PHASE_SINGLE;
        m_phaseCounter    = 3;
        SetDeathPrevention(true);
        DespawnGuids(m_spawns);
    }

    void EnterEvadeMode() override
    {
        // Transform back on evade
        if (DoCastSpellIfCan(nullptr, SPELL_TRANSFORM_TO_ORIGINAL) == CAST_OK)
            m_creature->UpdateEntry(NPC_HALAZZI);

        ScriptedAI::EnterEvadeMode();
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_HALAZZI, FAIL);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_HALAZZI, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_HALAZZI, DONE);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_SPIRIT_LYNX)
        {
            m_spiritLynxGuid = summoned->GetObjectGuid();
            summoned->SetInCombatWithZone();
            summoned->AI()->AttackClosestEnemy();
            summoned->CastSpell(m_creature, SPELL_HALAZZI_TRANSFORM_DUMMY, TRIGGERED_OLD_TRIGGERED);
        }

        m_spawns.push_back(summoned->GetObjectGuid());
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spell) override
    {
        if (spell->Id == SPELL_TRANSFIGURE_TRANSFORM)
        {
            DoCastSpellIfCan(m_creature, SPELL_HALAZZI_TRANSFORM_SUMMON, CAST_TRIGGERED);
            m_creature->UpdateEntry(NPC_HALAZZI_TROLL);

            m_phase = PHASE_TOTEM;
            HandlePhaseTransition();
        }
    }

    // Wrapper to handle the phase transform
    void DoReuniteSpirits()
    {
        uint32 spellId = 0;

        // Each health level has it's own spell - but they all do the same thing
        switch (m_phaseCounter)
        {
            case 3: spellId = SPELL_TRANSFORM_TO_LYNX_75; break;
            case 2: spellId = SPELL_TRANSFORM_TO_LYNX_50; break;
            case 1: spellId = SPELL_TRANSFORM_TO_LYNX_25; break;
        }

        if (DoCastSpellIfCan(nullptr, spellId) == CAST_OK)
        {
            DoScriptText(SAY_MERGE, m_creature);
            // Update stats back to the original Halazzi
            m_creature->UpdateEntry(NPC_HALAZZI);

            // Despawn the Lynx
            if (Creature* lynx = m_creature->GetMap()->GetCreature(m_spiritLynxGuid))
                lynx->ForcedDespawn();

            // Set the proper health level - workaround for missing server side spell 43538
            m_creature->SetHealth(m_creature->GetMaxHealth() / 4 * m_phaseCounter);
            --m_phaseCounter;

            m_phase = m_phaseCounter > 0 ? PHASE_SINGLE : PHASE_FINAL;
            HandlePhaseTransition();
        }
    }

    void HandlePhaseTransition()
    {
        switch (m_phase)
        {
            case PHASE_TOTEM:
                DisableCombatAction(HALAZZI_ACTION_FRENZY);
                DisableCombatAction(HALAZZI_ACTION_SABER_LASH);
            case PHASE_FINAL:
                ResetCombatAction(HALAZZI_ACTION_SHOCK, 10000);
                ResetCombatAction(HALAZZI_ACTION_TOTEM, 12000);
                if (m_phase == PHASE_TOTEM)
                    break;
                SetDeathPrevention(false);
                //[[fallthrough]]
            case PHASE_SINGLE:
                ResetCombatAction(HALAZZI_ACTION_FRENZY, 16000);
                ResetCombatAction(HALAZZI_ACTION_SABER_LASH, 20000);
                if (m_phase == PHASE_FINAL)
                    break;
                DisableCombatAction(HALAZZI_ACTION_SHOCK);
                DisableCombatAction(HALAZZI_ACTION_TOTEM);
                break;
        }
        SetActionReadyStatus(HALAZZI_ACTION_TRANSFORM, true);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case HALAZZI_ACTION_BERSERK:
                if (DoCastSpellIfCan(nullptr, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    DisableCombatAction(action);
                }
                return;
            case HALAZZI_ACTION_TRANSFORM:
                if (m_phase != PHASE_TOTEM)
                {
                    // Split boss at 75%, 50% and 25%
                    if (m_creature->GetHealthPercent() <= float(25 * m_phaseCounter))
                    {
                        if (DoCastSpellIfCan(nullptr, SPELL_TRANSFIGURE_TO_TROLL) == CAST_OK)
                        {
                            DoScriptText(SAY_SPLIT, m_creature);
                            SetActionReadyStatus(HALAZZI_ACTION_TRANSFORM, false);
                        }
                    }
                }
                else if (m_creature->GetHealthPercent() < 20.0f)
                    DoReuniteSpirits();
                return;
            case HALAZZI_ACTION_FRENZY:
                if (DoCastSpellIfCan(nullptr, SPELL_FRENZY) == CAST_OK)
                    ResetCombatAction(action, 16000);
                return;
            case HALAZZI_ACTION_SABER_LASH:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SABER_LASH) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_SABERLASH1 : SAY_SABERLASH2, m_creature);
                    ResetCombatAction(action, 20000);
                }
                return;
            case HALAZZI_ACTION_TOTEM:
                if (DoCastSpellIfCan(nullptr, SPELL_LIGHTNING_TOTEM) == CAST_OK)
                    ResetCombatAction(action, 20000);
                return;
            case HALAZZI_ACTION_SHOCK:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, urand(0, 1) ? SPELL_EARTHSHOCK : SPELL_FLAMESHOCK) == CAST_OK)
                        ResetCombatAction(action, urand(10000, 14000));
                return;
        }
    }
};

enum
{
    SPELL_LYNX_FRENZY       = 43290,
    SPELL_SHRED_ARMOR       = 43243
};

enum LynxActions
{
    LYNX_ACTION_UNIFICATION,
    LYNX_ACTION_FRENZY,
    LYNX_ACTION_SHRED_ARMOR,
    LYNX_ACTION_MAX,
};

struct boss_spirit_lynxAI : public CombatAI
{
    boss_spirit_lynxAI(Creature* creature) : CombatAI(creature, LYNX_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(LYNX_ACTION_UNIFICATION, true);
        AddCombatAction(LYNX_ACTION_FRENZY, 10000, 20000);
        AddCombatAction(LYNX_ACTION_SHRED_ARMOR, 4000u);
        SetDeathPrevention(true);
        Reset();
    }

    ScriptedInstance* m_instance;

    void KilledUnit(Unit* victim) override
    {
        if (!m_instance)
            return;

        if (Creature* halazzi = m_instance->GetSingleCreatureFromStorage(NPC_HALAZZI))
            halazzi->AI()->KilledUnit(victim);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case LYNX_ACTION_UNIFICATION:
                // Unite spirits at 10% health
                if (m_creature->GetHealthPercent() < 10.0f && m_instance)
                {
                    if (Creature* halazzi = m_instance->GetSingleCreatureFromStorage(NPC_HALAZZI))
                    {
                        if (boss_halazziAI* bossAI = dynamic_cast<boss_halazziAI*>(halazzi->AI()))
                            bossAI->DoReuniteSpirits();
                    }
                }
                return;
            case LYNX_ACTION_FRENZY:
                DoCastSpellIfCan(nullptr, SPELL_LYNX_FRENZY);
                ResetCombatAction(action, urand(20000, 30000));
                return;
            case LYNX_ACTION_SHRED_ARMOR:
                DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHRED_ARMOR);
                ResetCombatAction(action, 4000);
                return;
        }
    }
};

struct npc_corrupted_lightning_totemAI : public ScriptedAI
{
	npc_corrupted_lightning_totemAI(Creature* creature) : ScriptedAI(creature)
	{
		SetCombatMovement(false);
		SetMeleeEnabled(false);
	}

	void Reset() override { }

	void JustRespawned() override
	{
		m_creature->SetInCombatWithZone();
        SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(SPELL_LIGHTNING);
        m_creature->AddCooldown(*spellInfo);
	}

	void UpdateAI(const uint32 diff) override
	{
		if (m_creature->IsSpellReady(SPELL_LIGHTNING))
			if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_LIGHTNING, SELECT_FLAG_PLAYER))
				DoCastSpellIfCan(target, SPELL_LIGHTNING);
	}
};

void AddSC_boss_halazzi()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_halazzi";
    pNewScript->GetAI = &GetNewAIInstance<boss_halazziAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_spirit_lynx";
    pNewScript->GetAI = &GetNewAIInstance<boss_spirit_lynxAI>;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "npc_corrupted_lightning_totem";
	pNewScript->GetAI = &GetNewAIInstance<npc_corrupted_lightning_totemAI>;
	pNewScript->RegisterSelf();
}

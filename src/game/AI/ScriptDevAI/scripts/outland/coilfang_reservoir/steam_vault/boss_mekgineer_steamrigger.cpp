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
SDName: Boss_Mekgineer_Steamrigger
SD%Complete: 80
SDComment: Enrage on heroic NYI
SDCategory: Coilfang Resevoir, The Steamvault
EndScriptData */

/* ContentData
boss_mekgineer_steamrigger
mob_steamrigger_mechanic
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "steam_vault.h"

enum
{
    SAY_MECHANICS               = 14602,
    SAY_AGGRO_1                 = 17716,
    SAY_AGGRO_2                 = 17717,
    SAY_AGGRO_3                 = 17718,
    SAY_AGGRO_4                 = 17719,
    SAY_SLAY_1                  = 17720,
    SAY_SLAY_2                  = 17721,
    SAY_SLAY_3                  = 17722,
    SAY_DEATH                   = 17723,

    SPELL_SUPER_SHRINK_RAY      = 31485,
    SPELL_SAW_BLADE             = 31486,
    SPELL_ELECTRIFIED_NET       = 35107,
    SPELL_BERSERK               = 26662,

    SPELL_INVIS_AND_STEALTH_DET = 18950,

    NPC_STEAMRIGGER_MECHANIC    = 17951,

    SPELL_SUMMON_GNOME_1        = 31528,
    SPELL_SUMMON_GNOME_2        = 31529,
    SPELL_SUMMON_GNOME_3        = 31530,
    SPELL_SUMMON_GNOMES         = 31531,

    // Mechanic spells
    SPELL_DISPEL_MAGIC          = 17201,
    SPELL_REPAIR                = 31532,
    SPELL_REPAIR_H              = 37936,
};

static const uint32 gnomeSpells[] = { SPELL_SUMMON_GNOME_1, SPELL_SUMMON_GNOME_2, SPELL_SUMMON_GNOME_3 };

enum MekgineerActions
{
    MEKGINEER_SUMMON_GNOMES,
    MEKGINEER_ACTION_MAX,
    MEKGINEER_SPAWN_GNOME
};

struct boss_mekgineer_steamriggerAI : public CombatAI
{
    boss_mekgineer_steamriggerAI(Creature* creature) : CombatAI(creature, MEKGINEER_ACTION_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3);
        AddTimerlessCombatAction(MEKGINEER_SUMMON_GNOMES, true);
        if (!m_isRegularMode)
            AddCustomAction(MEKGINEER_SPAWN_GNOME, 20000u, [&]()
            {
                m_creature->CastSpell(nullptr, gnomeSpells[urand(0, 2)], TRIGGERED_OLD_TRIGGERED);
            }, TIMER_COMBAT_COMBAT);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    uint8 m_mechanicPhaseCount;

    GuidVector m_spawns;

    void Reset() override
    {
        CombatAI::Reset();
        DoCastSpellIfCan(nullptr, SPELL_INVIS_AND_STEALTH_DET, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        m_mechanicPhaseCount  = 1;

        DespawnGuids(m_spawns);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MEKGINEER_STEAMRIGGER, FAIL);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_MEKGINEER_STEAMRIGGER, DONE);
    }

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoBroadcastText(SAY_AGGRO_1, m_creature); break;
            case 1: DoBroadcastText(SAY_AGGRO_2, m_creature); break;
            case 2: DoBroadcastText(SAY_AGGRO_3, m_creature); break;
        }

        if (m_instance)
            m_instance->SetData(TYPE_MEKGINEER_STEAMRIGGER, IN_PROGRESS);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_STEAMRIGGER_MECHANIC)
        {
            summoned->GetMotionMaster()->MoveFollow(m_creature, 0, 0);
            m_spawns.push_back(summoned->GetObjectGuid());
        }
    }

    void ExecuteAction(uint32 action) override
    {
        if (action == MEKGINEER_SUMMON_GNOMES)
        {
            if (m_creature->GetHealthPercent() < (100 - 25 * m_mechanicPhaseCount))
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_GNOMES) == CAST_OK)
                {
                    DoBroadcastText(SAY_MECHANICS, m_creature);
                    ++m_mechanicPhaseCount;
                }
            }
        }
    }
};

struct mob_steamrigger_mechanicAI : public CombatAI
{
    mob_steamrigger_mechanicAI(Creature* creature) : CombatAI(creature, 0),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetReactState(REACT_DEFENSIVE);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    void MoveInLineOfSight(Unit* who) override
    {
        // Don't attack players unless attacked
        if (who->GetEntry() == NPC_STEAMRIGGER)
        {
            if (m_instance->GetData(TYPE_MEKGINEER_STEAMRIGGER) == IN_PROGRESS)
            {
                // Channel the repair spell on Steamrigger
                // This will also stop creature movement and will allow them to continue to follow the boss after channeling is finished or the boss is out of range
                if (m_creature->IsWithinDistInMap(who, 2 * INTERACTION_DISTANCE))
                {
                    DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_REPAIR : SPELL_REPAIR_H);
                    SetReactState(REACT_AGGRESSIVE);
                }
            }
        }
        ScriptedAI::MoveInLineOfSight(who);
    }
};

// 31531 - Summon Gnomes
struct SummonGnomes : public AuraScript
{
    void OnPeriodicTickEnd(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        target->CastSpell(nullptr, SPELL_SUMMON_GNOME_1, TRIGGERED_OLD_TRIGGERED);
        target->CastSpell(nullptr, SPELL_SUMMON_GNOME_2, TRIGGERED_OLD_TRIGGERED);
        target->CastSpell(nullptr, SPELL_SUMMON_GNOME_3, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_mekgineer_steamrigger()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_mekgineer_steamrigger";
    pNewScript->GetAI = &GetNewAIInstance<boss_mekgineer_steamriggerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_steamrigger_mechanic";
    pNewScript->GetAI = &GetNewAIInstance<mob_steamrigger_mechanicAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SummonGnomes>("spell_summon_gnomes");
}

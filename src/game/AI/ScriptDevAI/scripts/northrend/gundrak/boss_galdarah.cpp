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
SDName: Boss_Galdarah
SD%Complete: 80%
SDComment: achievements need to be implemented, channeling before engage is missing
SDCategory: Gundrak
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "gundrak.h"
#include "AI/ScriptDevAI/base/BossAI.h"

enum
{
    SAY_AGGRO                  = 32534,
    SAY_TRANSFORM_1            = 32535,
    SAY_TRANSFORM_2            = 32536,
    SAY_SUMMON_1               = 32537,
    SAY_SUMMON_2               = 32538,
    SAY_SUMMON_3               = 32539,
    SAY_SLAY_1                 = 32540,
    SAY_SLAY_2                 = 32541,
    SAY_SLAY_3                 = 32542,
    SAY_DEATH                  = 32543,

    EMOTE_IMPALED              = 30718,

    NPC_RHINO_SPIRIT           = 29791,
    SPELL_STAMPEDE_RHINO       = 55220,
    SPELL_STAMPEDE_RHINO_H     = 59823,
    SPELL_STAMPEDE_EFFECT      = 55219,                 // spell cast on boss; possibly used to lock a target
    SPELL_STAMPEDE_PROC        = 55221,                 // proc creature despawn

    // troll form spells
    SPELL_STAMPEDE             = 55218,
    SPELL_WHIRLING_SLASH       = 55250,
    SPELL_WHIRLING_SLASH_H     = 59824,
    SPELL_RHINO_TRANSFORM      = 55297,
    SPELL_PUNCTURE             = 55276,
    SPELL_PUNCTURE_H           = 59826,

    // rhino form spells
    SPELL_TROLL_TRANSFORM      = 55299,
    SPELL_ENRAGE               = 55285,
    SPELL_ENRAGE_H             = 59828,
    SPELL_IMPALING_CHARGE      = 54956,
    SPELL_IMPALING_CHARGE_H    = 59827,
    SPELL_IMPALING_CHARGE_CONTROL_VEHICLE = 54958,
    SPELL_RECENTLY_IMPALED_MARKER = 60592,
    SPELL_STOMP                = 55292,
    SPELL_STOMP_H              = 59829,

    SPELLSET_TROLL_NHC         = 2930600,
    SPELLSET_RHINO_NHC         = 2930601,
    SPELLSET_TROLL_HC          = 3136800,
    SPELLSET_RHINO_HC          = 3136801,
};

/*######
## boss_galdarah
######*/

enum GaldarahActions
{
    GALDARAH_PHASE_CHANGE,
    GALDARAH_ACTIONS_MAX,
};

struct boss_galdarahAI : public BossAI
{
    boss_galdarahAI(Creature* creature) : BossAI(creature, GALDARAH_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_gundrak*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_GALDARAH);
        AddOnAggroText(SAY_AGGRO);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3);
        AddOnDeathText(SAY_DEATH);
        AddCombatAction(GALDARAH_PHASE_CHANGE, true);
    }

    instance_gundrak* m_instance;
    bool m_isRegularMode;
    bool m_isTrollPhase;

    uint8 m_abilityCount;

    void Reset() override
    {
        BossAI::Reset();
        m_isTrollPhase = true;
        m_abilityCount = 0;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType != AI_EVENT_CUSTOM_EVENTAI_A)
            return;
        if (m_creature->GetObjectGuid() != sender->GetObjectGuid())
            return;
        if (m_abilityCount < 2)
        {
            ++m_abilityCount;
            return;
        }
        ResetCombatAction(GALDARAH_PHASE_CHANGE, 7s);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() != NPC_RHINO_SPIRIT)
            return;
        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, m_isRegularMode ? SPELL_STAMPEDE_RHINO : SPELL_STAMPEDE_RHINO_H, SELECT_FLAG_PLAYER))
        {
            summoned->CastSpell(m_creature, SPELL_STAMPEDE_EFFECT, TRIGGERED_OLD_TRIGGERED);
            summoned->CastSpell(nullptr, SPELL_STAMPEDE_PROC, TRIGGERED_OLD_TRIGGERED);
            summoned->CastSpell(target, m_isRegularMode ? SPELL_STAMPEDE_RHINO : SPELL_STAMPEDE_RHINO_H, TRIGGERED_NONE, nullptr, nullptr, m_creature->GetObjectGuid());
        }
    }

    void DoPhaseSwitch()
    {
        if (!m_isTrollPhase)
            m_creature->RemoveAurasDueToSpell(SPELL_RHINO_TRANSFORM);

        m_isTrollPhase = !m_isTrollPhase;

        if (m_isTrollPhase)
        {
            DoCastSpellIfCan(nullptr, SPELL_TROLL_TRANSFORM);
            m_creature->SetSpellList(m_isRegularMode ? SPELLSET_TROLL_NHC : SPELLSET_TROLL_HC);
        }
        else
        {
            DoBroadcastText(urand(0, 1) ? SAY_TRANSFORM_1 : SAY_TRANSFORM_2, m_creature);
            DoCastSpellIfCan(nullptr, SPELL_RHINO_TRANSFORM);
            m_creature->SetSpellList(m_isRegularMode ? SPELLSET_RHINO_NHC : SPELLSET_RHINO_HC);
        }
        m_abilityCount = 0;
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case GALDARAH_PHASE_CHANGE:
            {
                DoPhaseSwitch();
                DisableCombatAction(action);
                return;
            }
        }
    }
};

struct ImpalingCharge : SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo /*missInfo*/) const override
    {
        Unit* rhino = spell->GetCaster();
        Unit* player = spell->GetUnitTarget();
        if (!rhino || !player)
            return;
        player->CastSpell(rhino, SPELL_IMPALING_CHARGE_CONTROL_VEHICLE, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
        // Store the player guid in order to count it for the achievement
        if (instance_gundrak* instance = dynamic_cast<instance_gundrak*>(rhino->GetInstanceData()))
            instance->SetData(TYPE_ACHIEV_SHARE_LOVE, player->GetGUIDLow());
    }
};

void AddSC_boss_galdarah()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_galdarah";
    pNewScript->GetAI = &GetNewAIInstance<boss_galdarahAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ImpalingCharge>("spell_impaling_charge");
}

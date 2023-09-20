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
SDName: Boss_Doomwalker
SD%Complete: 100
SDComment:
SDCategory: Shadowmoon Valley
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    NPC_DREADLORD               = 21166,
    NPC_ILLIDARI_RAVAGER        = 22857,
    NPC_SHADOWHOOF_ASSASSIN     = 22858,
    NPC_SHADOWHOOF_SUMMONER     = 22859,
    NPC_ILLIDARI_SUCCUBUS       = 22860,

    SAY_AGGRO                   = 20941,
    SAY_EARTHQUAKE_1            = 20942,
    SAY_EARTHQUAKE_2            = 20943,
    SAY_OVERRUN_1               = 20944,
    SAY_OVERRUN_2               = 20945,
    SAY_SLAY_1                  = 20946,
    SAY_SLAY_2                  = 20947,
    SAY_SLAY_3                  = 20948,
    SAY_DEATH                   = 20949,

    EMOTE_FRENZY                = 1191,

    SPELL_EARTHQUAKE            = 32686,
    SPELL_CRUSH_ARMOR           = 33661,
    SPELL_LIGHTNING_WRATH       = 33665,
    SPELL_OVERRUN               = 32636,
    SPELL_OVERRUN_TRIGGER_SPAWN = 32632,
    SPELL_OVERRUN_DAMAGE        = 32637,
    SPELL_ENRAGE                = 33653,
    SPELL_MARK_OF_DEATH_PLAYER  = 37128,
    SPELL_MARK_OF_DEATH_AURA    = 37125,        // triggers 37131 on target if it has aura 37128

    SPELL_EARTHQUAKE_VISUAL     = 41966,        // used in OOC sequence for RP

    COUNT_OVERRUN               = 5,
    POINT_OVERRUN               = 1,
};

enum DoomwalkerActions
{
    DOOMWALKER_ACTION_MAX,
    DOOMWALKER_OVERRUN_SPELL,
};

struct boss_doomwalkerAI : public CombatAI
{
    boss_doomwalkerAI(Creature* creature) : CombatAI(creature, DOOMWALKER_ACTION_MAX)
    {
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3);
        AddCustomAction(DOOMWALKER_OVERRUN_SPELL, true, [&]() { HandleOverrunDamage(); }, TIMER_COMBAT_COMBAT);
    }

    uint32 m_uiChainTimer;
    uint32 m_uiOverrunTimer;
    uint32 m_uiQuakeTimer;
    uint32 m_uiArmorTimer;

    uint32 m_overrunTargets;
    uint32 m_overrunExecTimer;
    GuidVector m_overrunCandidates;
    SelectAttackingTargetParams m_overrunParams;

    void Reset() override
    {
        CombatAI::Reset();
        m_overrunTargets = 0;

        m_overrunParams.range.minRange = 0;
        m_overrunParams.range.maxRange = 30;

        m_creature->RemoveAurasDueToSpell(SPELL_MARK_OF_DEATH_AURA);

        SetCombatMovement(true);
        SetMeleeEnabled(true);
        SetCombatScriptStatus(false);
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);
        if (!victim->IsPlayer())
            return;

        victim->CastSpell(nullptr, SPELL_MARK_OF_DEATH_PLAYER, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
    }

    void MovementInform(uint32 movementType, uint32 data) override
    {
        if (movementType == POINT_MOTION_TYPE && data == POINT_OVERRUN)
            HandleOverrun();
    }

    void JustSummoned(Creature* summoned) override
    {
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        m_creature->SetTarget(nullptr);
        SetCombatScriptStatus(true);
        float x, y, z;
        summoned->GetNearPoint(m_creature, x, y, z, 0.f, 0.f, summoned->GetAngle(m_creature));
        m_creature->GetMotionMaster()->MovePoint(POINT_OVERRUN, x, y, z, FORCED_MOVEMENT_RUN);
        ResetTimer(DOOMWALKER_OVERRUN_SPELL, 250);
    }

    void JustRespawned() override
    {
        CombatAI::JustRespawned();

        std::list<Creature*> npcList;
        GetCreatureListWithEntryInGrid(npcList, m_creature, NPC_DREADLORD, 200.0f);
        GetCreatureListWithEntryInGrid(npcList, m_creature, NPC_ILLIDARI_RAVAGER, 200.0f);
        GetCreatureListWithEntryInGrid(npcList, m_creature, NPC_SHADOWHOOF_ASSASSIN, 200.0f);
        GetCreatureListWithEntryInGrid(npcList, m_creature, NPC_SHADOWHOOF_SUMMONER, 200.0f);
        GetCreatureListWithEntryInGrid(npcList, m_creature, NPC_ILLIDARI_SUCCUBUS, 200.0f);

        for (Creature* creature : npcList)
            creature->Suicide();
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);
        m_creature->RemoveAurasDueToSpell(SPELL_MARK_OF_DEATH_AURA);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoCastSpellIfCan(nullptr, SPELL_MARK_OF_DEATH_AURA, CAST_TRIGGERED);
        DoBroadcastText(SAY_AGGRO, m_creature);
    }

    void HandleOverrun()
    {
        auto fail = [&]()
        {
            SetCombatMovement(true, true);
            SetMeleeEnabled(true);
            SetCombatScriptStatus(false);
            m_creature->RemoveAurasDueToSpell(SPELL_OVERRUN);
            DoResetThreat();
            DisableTimer(DOOMWALKER_OVERRUN_SPELL);
            AttackClosestEnemy();
        };
        if (m_overrunTargets == 0)
        {
            fail();
            return;
        }

        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_RANGE_AOE_RANGE, m_overrunParams))
        {
            float angle = m_creature->GetAngle(target);
            m_creature->SetFacingTo(angle);
            m_creature->SetOrientation(angle);
            m_creature->CastSpell(nullptr, SPELL_OVERRUN_TRIGGER_SPAWN, TRIGGERED_OLD_TRIGGERED); // shouldnt be sent to client
            DoScriptText(urand(0, 1) ? SAY_OVERRUN_1 : SAY_OVERRUN_2, m_creature);
            --m_overrunTargets;
        }
        else
            fail();
    }

    void HandleOverrunDamage()
    {
        m_creature->CastSpell(nullptr, SPELL_OVERRUN_DAMAGE, TRIGGERED_NONE);
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* target) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_OVERRUN:
                m_overrunTargets = COUNT_OVERRUN;
                HandleOverrun();
                break;
            case SPELL_ENRAGE:
                DoScriptText(EMOTE_FRENZY, m_creature);
                break;
            case SPELL_EARTHQUAKE:
                DoScriptText(urand(0, 1) ? SAY_EARTHQUAKE_1 : SAY_EARTHQUAKE_2, m_creature);
                break;
        }
    }
};

// 32686 - Earthquake
struct EarthquakeDoomwalker : public AuraScript
{
    void OnPeriodicTrigger(Aura* /*aura*/, PeriodicTriggerData& data) const override
    {
        if (urand(0, 1))
            data.spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(13360);
    }
};

// 37127 - Mark of Death
struct MarkofDeath : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply && aura->GetTarget()->HasAura(37128))
            aura->GetTarget()->CastSpell(nullptr, 37131, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_doomwalker()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_doomwalker";
    pNewScript->GetAI = &GetNewAIInstance<boss_doomwalkerAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<EarthquakeDoomwalker>("spell_earthquake_doomwalker");
    RegisterSpellScript<MarkofDeath>("spell_mark_of_death");
}

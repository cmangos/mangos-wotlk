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
SDName: boss_krick_and_ick
SD%Complete: 100
SDComment:
SDCategory: Pit of Saron
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "pit_of_saron.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/SpellAuras.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SAY_AGGRO                           = -1658024,
    SAY_SLAY_1                          = -1658025,
    SAY_SLAY_2                          = -1658026,
    SAY_ORDER_STOP                      = -1658027,
    SAY_ORDER_BLOW                      = -1658028,
    SAY_TARGET_1                        = -1658029,
    SAY_TARGET_2                        = -1658030,
    SAY_TARGET_3                        = -1658031,
    SAY_OUTRO_1                         = -1658035,

    EMOTE_KRICK_MINES                   = -1658032,
    EMOTE_ICK_POISON                    = -1658033,
    EMOTE_ICK_CHASING                   = -1658034,

    // ick spells
    SPELL_POISON_NOVA                   = 68989,
    SPELL_MIGHTY_KICK                   = 69021,
    SPELL_PURSUIT                       = 68987,
    SPELL_EXPLOSIVE_BARRAGE_ICK         = 69263,

    // krick spells
    SPELL_TOXIC_WASTE                   = 69024,
    SPELL_SHADOW_BOLT                   = 69028,
    SPELL_EXPLOSIVE_BARRAGE_KRICK       = 69012,                // Triggers 69015 every 2 sec
    SPELL_KRICK_KILL_CREDIT             = 71308,

    NPC_EXPLODING_ORB                   = 36610,                // summoned by 69015; handled in EAI

    // exploding orb spells
    // SPELL_EXPLOSIVE_BARRAGE_SUMMON   = 69015,
    SPELL_EXPLODING_ORB_VISUAL          = 69017,
    SPELL_AUTO_GROW_AND_SPEED_BOOST     = 69020,                // triggers 69019 at 15 stacks
    // SPELL_EXPLOSIVE_BARRAGE_DMG      = 69019,
    // SPELL_HASTY_GROW                 = 44851,                // Orb explodes after the 15th stack

    MAX_HASTY_GROW_STACKS               = 15,
};

static const EventNpcLocations aKrickSummonData[1]
{
    {NPC_SYLVANAS_PART1, NPC_JAINA_PART1, 777.2274f, 119.5521f, 510.0363f, 6.05f, 1},
};

static const float afTyrannusTeleLoc[4] = { 841.01f, 196.245f, 573.964f, 3.75118f };

/*######
## boss_ick
######*/

enum IckActions
{
    ICK_MIGHTY_BLOW,
    ICK_POISON_NOVA,
    ICK_PURSUIT,
    ICK_EXPLOSIVE_BARRAGE,
    ICK_ACTION_MAX,
};

struct boss_ickAI : public CombatAI
{
    boss_ickAI(Creature* creature) : CombatAI(creature, ICK_ACTION_MAX), m_instance(static_cast<instance_pit_of_saron*>(creature->GetInstanceData()))
    {
        AddCombatAction(ICK_MIGHTY_BLOW, 10000u);
        AddCombatAction(ICK_POISON_NOVA, 60000u);
        AddCombatAction(ICK_PURSUIT, 10000u);
        AddCombatAction(ICK_EXPLOSIVE_BARRAGE, 50000u);
    }

    instance_pit_of_saron* m_instance;

    void Aggro(Unit* who) override
    {
        if (m_instance)
        {
            m_instance->SetData(TYPE_KRICK, IN_PROGRESS);

            // Say aggro and also put Krick in combat
            if (Creature* pKrick = m_instance->GetSingleCreatureFromStorage(NPC_KRICK))
            {
                DoScriptText(SAY_AGGRO, pKrick);
                pKrick->AI()->AttackStart(who);
            }
        }
    }

    void KilledUnit(Unit* victim)
    {
        CombatAI::KilledUnit(victim);

        if (m_instance)
        {
            if (Creature* pKrick = m_instance->GetSingleCreatureFromStorage(NPC_KRICK))
                DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, pKrick);
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_instance)
        {
            m_instance->SetData(TYPE_KRICK, DONE);

            // despawn the spawn at the entrance first
            if (Creature* pCreature = m_instance->GetSingleCreatureFromStorage(m_instance->GetPlayerTeam() == HORDE ? NPC_SYLVANAS_PART1 : NPC_JAINA_PART1))
                pCreature->ForcedDespawn();
            if (Creature* pCreature = m_instance->GetSingleCreatureFromStorage(m_instance->GetPlayerTeam() == HORDE ? NPC_KILARA : NPC_ELANDRA))
                pCreature->ForcedDespawn();
            if (Creature* pCreature = m_instance->GetSingleCreatureFromStorage(m_instance->GetPlayerTeam() == HORDE ? NPC_LORALEN : NPC_KORELN))
                pCreature->ForcedDespawn();

            // summon Jaina / Sylvanas
            if (Creature* pCreature = m_creature->SummonCreature(m_instance->GetPlayerTeam() == HORDE ? aKrickSummonData[0].uiEntryHorde : aKrickSummonData[0].uiEntryAlliance,
                aKrickSummonData[0].fX, aKrickSummonData[0].fY, aKrickSummonData[0].fZ, aKrickSummonData[0].fO, TEMPSPAWN_TIMED_DESPAWN, 10 * MINUTE * IN_MILLISECONDS))
                pCreature->GetMotionMaster()->MoveWaypoint(aKrickSummonData[0].pathId, 0, 1000);

            // move Krick into position
            if (Creature* pKrick = m_instance->GetSingleCreatureFromStorage(NPC_KRICK))
            {
                pKrick->AI()->EnterEvadeMode();
                pKrick->SetImmuneToNPC(true);
                pKrick->SetImmuneToPlayer(true);
                pKrick->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            }

            // teleport Tyrannus for event
            if (Creature* pTyrannus = m_instance->GetSingleCreatureFromStorage(NPC_TYRANNUS_INTRO))
            {
                pTyrannus->GetMotionMaster()->MoveIdle();
                pTyrannus->NearTeleportTo(afTyrannusTeleLoc[0], afTyrannusTeleLoc[1], afTyrannusTeleLoc[2], afTyrannusTeleLoc[3]);
            }
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_KRICK, FAIL);
    }

    void ExecuteAction(uint32 action) override
    {
        if (!m_instance)
            return;

        switch (action)
        {
            case ICK_MIGHTY_BLOW:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MIGHTY_KICK) == CAST_OK)
                    ResetCombatAction(action, 30000);
                break;
            case ICK_POISON_NOVA:
                if (DoCastSpellIfCan(m_creature, SPELL_POISON_NOVA) == CAST_OK)
                {
                    if (Creature* pKrick = m_instance->GetSingleCreatureFromStorage(NPC_KRICK))
                    {
                        DoScriptText(SAY_ORDER_BLOW, pKrick);
                        DoScriptText(EMOTE_ICK_POISON, pKrick);
                    }

                    ResetCombatAction(action, 60000);
                }
                break;
            case ICK_PURSUIT:
                if (DoCastSpellIfCan(m_creature, SPELL_PURSUIT) == CAST_OK)
                {
                    if (Creature* pKrick = m_instance->GetSingleCreatureFromStorage(NPC_KRICK))
                    {
                        switch (urand(0, 2))
                        {
                            case 0: DoScriptText(SAY_TARGET_1, pKrick); break;
                            case 1: DoScriptText(SAY_TARGET_2, pKrick); break;
                            case 2: DoScriptText(SAY_TARGET_3, pKrick); break;
                        }
                    }

                    ResetCombatAction(action, 60000);
                }
                break;
            case ICK_EXPLOSIVE_BARRAGE:
                if (DoCastSpellIfCan(m_creature, SPELL_EXPLOSIVE_BARRAGE_ICK) == CAST_OK)
                {
                    if (Creature* pKrick = m_instance->GetSingleCreatureFromStorage(NPC_KRICK))
                    {
                        pKrick->InterruptNonMeleeSpells(false);
                        pKrick->CastSpell(pKrick, SPELL_EXPLOSIVE_BARRAGE_KRICK, TRIGGERED_OLD_TRIGGERED);

                        DoScriptText(SAY_ORDER_STOP, pKrick);
                        DoScriptText(EMOTE_KRICK_MINES, pKrick);
                    }

                    ResetCombatAction(action, 60000);
                }
                break;
        }
    }
};

/*######
## boss_krick
######*/

enum KrickActions
{
    KRICK_SHADOW_BOLT,
    KRICK_TOXIC_WASTE,
    KRICK_ACTION_MAX,
};

struct boss_krickAI : public CombatAI
{
    boss_krickAI(Creature* creature) : CombatAI(creature, KRICK_ACTION_MAX)
    {
        AddCombatAction(KRICK_SHADOW_BOLT, 8000u);
        AddCombatAction(KRICK_TOXIC_WASTE, 5000u);
    }

    void KilledUnit(Unit* victim)
    {
        CombatAI::KilledUnit(victim);

        if (victim != m_creature)
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* killer) override
    {
        DoCastSpellIfCan(m_creature, SPELL_KRICK_KILL_CREDIT, CAST_TRIGGERED);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_EXPLODING_ORB)
        {
            summoned->AI()->SetReactState(REACT_PASSIVE);
            summoned->SetCanEnterCombat(false);
            summoned->CastSpell(summoned, SPELL_EXPLODING_ORB_VISUAL, TRIGGERED_OLD_TRIGGERED);
            summoned->CastSpell(summoned, SPELL_AUTO_GROW_AND_SPEED_BOOST, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KRICK_SHADOW_BOLT:
                if (Unit* spawner = m_creature->GetSpawner())
                {
                    Creature* ick = static_cast<Creature*>(spawner);
                    if (Unit* target = ick->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(target, SPELL_SHADOW_BOLT) == CAST_OK)
                            ResetCombatAction(action, 30000);
                    }
                }
                break;
            case KRICK_TOXIC_WASTE:
                if (DoCastSpellIfCan(m_creature, SPELL_TOXIC_WASTE) == CAST_OK)
                    ResetCombatAction(action, 5000);
                break;
        }
    }

};

/*######
## spell_explosive_barrage_aura - 69012
######*/

struct spell_explosive_barrage_aura : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& /*data*/) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsCreature())
            return;

        // Summon an Exploding Orb for each player in combat with the caster
        ThreatList const& threatList = target->getThreatManager().getThreatList();
        for (auto itr : threatList)
        {
            if (Unit* expectedTarget = target->GetMap()->GetUnit(itr->getUnitGuid()))
            {
                if (expectedTarget->IsPlayer())
                    target->CastSpell(expectedTarget, 69015, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }
};

/*######
## spell_pursuit - 68987
######*/

struct spell_pursuit : public SpellScript, public AuraScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_1)
            return;

        Unit* target = spell->GetUnitTarget();
        Unit* caster = spell->GetAffectiveCaster();
        if (!target || !caster)
            return;

        caster->AddThreat(target, spell->m_spellInfo->CalculateSimpleValue(effIdx));
        caster->FixateTarget(target);
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;

        if (!apply)
            caster->AI()->DoResetThreat();
    }
};

void AddSC_boss_krick_and_ick()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_ick";
    pNewScript->GetAI = &GetNewAIInstance<boss_ickAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_krick";
    pNewScript->GetAI = &GetNewAIInstance<boss_krickAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_explosive_barrage_aura>("spell_explosive_barrage_aura");
    RegisterSpellScript<spell_pursuit>("spell_pursuit");
}

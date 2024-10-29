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
SDName: boss_auriaya
SD%Complete: 100%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ulduar.h"
#include "AI/ScriptDevAI/base/BossAI.h"

enum
{
    SAY_AGGRO                           = 34341,
    SAY_SLAY_1                          = 34355,
    SAY_SLAY_2                          = 34354,
    SAY_BERSERK                         = 34358,
    EMOTE_DEFENDER                      = 34162,

    // Auriaya
    SPELL_BERSERK                       = 47008,
    SPELL_GUARDIAN_SWARM                = 64396,            // triggers 64397
    SPELL_SENTINEL_BLAST                = 64389,            // triggers 64392
    SPELL_SENTINEL_BLAST_H              = 64678,            // triggers 64679
    SPELL_SONIC_SCREECH                 = 64422,
    SPELL_SONIC_SCREECH_H               = 64688,
    SPELL_TERRIFYING_SCREECH            = 64386,
    SPELL_ACTIVATE_FERAL_DEFENDER       = 64449,            // triggers 64447
    SPELL_ACTIVATE_FERAL_DEFENDER_TRIGG = 64448,

    // Feral Defender spells
    SPELL_FERAL_ESSENCE                 = 64455,
    SPELL_FERAL_ESSENCE_REMOVAL         = 64456,            // remove 1 stack of 64455
    SPELL_FERAL_POUNCE                  = 64478,
    SPELL_FERAL_POUNCE_H                = 64669,
    SPELL_FERAL_RUSH                    = 64489,            // triggers 64496
    SPELL_FERAL_RUSH_H                  = 64673,            // triggers 64674
    SPELL_SEEPING_FERAL_ESSENCE_SUMMON  = 64457,
    SPELL_FEIGN_DEATH                   = 64461,            // related to the feral defender feign death
    SPELL_FULL_HEAL                     = 64460,            // on feign death remove

    // Seeping Feral Essence
    SPELL_SEEPING_FERAL_ESSENCE         = 64458,
    SPELL_SEEPING_FERAL_ESSENCE_H       = 64676,

    NPC_SEEPING_FERAL_ESSENCE           = 34098,            // summoned by the feral defender on feign death
    // NPC_GUARDIAN_SWARN               = 34034,            // summoned by spell
    NPC_FERAL_DEFENDER_STALKER          = 34096,

    SOUND_DEATH                         = 15476,
};

/*######
## boss_auriaya
######*/

enum AuriayaActions
{
    AURIAYA_DEFENDER,
    AURIAYA_ACTIONS_MAX,
};

struct boss_auriayaAI : public BossAI
{
    boss_auriayaAI(Creature* creature) : BossAI(creature, AURIAYA_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_ulduar*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_AURIAYA);
        AddOnAggroText(SAY_AGGRO);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
        AddCombatAction(AURIAYA_DEFENDER, 1min);
    }

    instance_ulduar* m_instance;
    bool m_isRegularMode;

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        m_creature->PlayDirectSound(SOUND_DEATH);
    }

    void JustSummoned(Creature* summoned) override
    {
        // Summon the feral defender
        if (summoned->GetEntry() == NPC_FERAL_DEFENDER_STALKER)
            DoCastSpellIfCan(summoned, SPELL_ACTIVATE_FERAL_DEFENDER, CAST_INTERRUPT_PREVIOUS);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case AURIAYA_DEFENDER:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ACTIVATE_FERAL_DEFENDER_TRIGG) == CAST_OK)
                    DisableCombatAction(action);
                return;
            }
            default:
                return;
        }
    }
};

/*######
## boss_feral_defender
######*/

enum FeralDefenderActions
{
    FERAL_DEFENDER_FERAL_RUSH,
    FERAL_DEFENDER_ACTIONS_MAX,
    FERAL_DEFENDER_REVIVE,
    FERAL_DEFENDER_RESTART_COMBAT,
};

struct boss_feral_defenderAI : public BossAI
{
    boss_feral_defenderAI(Creature* creature) : BossAI(creature, FERAL_DEFENDER_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_ulduar*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        m_maxFeralRush = m_isRegularMode ? 6 : 10;
        AddCombatAction(FERAL_DEFENDER_FERAL_RUSH, 3s, 5s);
        AddCustomAction(FERAL_DEFENDER_REVIVE, true, [&]()
        {
            DoCastSpellIfCan(m_creature, SPELL_FULL_HEAL, CAST_TRIGGERED);
            ResetTimer(FERAL_DEFENDER_RESTART_COMBAT, 3s);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        });
        AddCustomAction(FERAL_DEFENDER_RESTART_COMBAT, true, [&]()
        {
            DoResetThreat();
            m_creature->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->SetImmobilizedState(false, true);
            SetCombatScriptStatus(false);
            if (Creature* seepingFeralStalker = m_creature->GetMap()->GetCreature(m_seepingGuid))
                seepingFeralStalker->ForcedDespawn();

            m_creature->AI()->SpellListChanged();
            ResetTimer(FERAL_DEFENDER_FERAL_RUSH, 1s);
        });
        SetDeathPrevention(true);
    }

    instance_ulduar* m_instance;
    bool m_isRegularMode;

    uint8 m_feralRushCount;
    uint8 m_maxFeralRush;
    uint8 m_deathCount;

    ObjectGuid m_seepingGuid;

    void Reset() override
    {
        m_feralRushCount      = 0;
        m_deathCount          = 0;

        DoCastSpellIfCan(m_creature, SPELL_FERAL_ESSENCE);
    }

    void JustPreventedDeath(Unit* attacker) override
    {
        if (m_deathCount >= 7)
            SetDeathPrevention(false);
        DoCastSpellIfCan(m_creature, SPELL_FERAL_ESSENCE_REMOVAL, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_SEEPING_FERAL_ESSENCE_SUMMON, CAST_TRIGGERED);
        DoLimitedFakeDeath(SPELL_FEIGN_DEATH);
        ++m_deathCount;
        ResetTimer(FERAL_DEFENDER_REVIVE, 30s);
        SetCombatScriptStatus(true);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        // Set achiev criteria to true
        if (m_instance)
            m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_NINE_LIVES, true);
    }

    void JustSummoned(Creature* summoned) override
    {
        // Cast seeping feral essence on the summoned
        if (summoned->GetEntry() == NPC_SEEPING_FERAL_ESSENCE)
        {
            m_seepingGuid = summoned->GetObjectGuid();
            summoned->SetImmobilizedState(true);
            summoned->CastSpell(summoned, m_isRegularMode ? SPELL_SEEPING_FERAL_ESSENCE : SPELL_SEEPING_FERAL_ESSENCE_H, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
        }
    }

    void DoLimitedFakeDeath(uint32 spellId)
    {
        m_creature->InterruptNonMeleeSpells(false);
        m_creature->InterruptMoving();
        m_creature->ClearComboPointHolders();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        m_creature->ClearAllReactives();
        m_creature->SetTarget(nullptr);
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetImmobilizedState(true, true);

        if (spellId)
            DoCastSpellIfCan(nullptr, spellId, CAST_INTERRUPT_PREVIOUS);
        ResetDeathPrevented();
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case FERAL_DEFENDER_FERAL_RUSH:
            {
                if (DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_FERAL_RUSH : SPELL_FERAL_RUSH_H) == CAST_OK)
                {
                    ++m_feralRushCount;
                    if (m_feralRushCount < m_maxFeralRush)
                        ResetCombatAction(action, 400ms);
                    else
                    {
                        m_feralRushCount = 0;
                        ResetCombatAction(action, 12s);
                    }
                }
                return;
            }
        }
    };
};

// 64386 - Terrifying Screech
struct TerrifyingScreech : public SpellScript
{
    void OnSuccessfulFinish(Spell* spell) const override
    {
        Creature* caster = dynamic_cast<Creature*>(spell->GetCaster());
        if (!caster || !caster->AI())
            return;
        caster->AI()->DoCastSpellIfCan(nullptr, caster->GetMap()->IsRegularDifficulty() ? SPELL_SENTINEL_BLAST : SPELL_SENTINEL_BLAST_H);
    }
};

// 64456 - Feral Essence Application Removal
struct FeralEssanceRemoval : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget)
            return;

        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_0);
        unitTarget->RemoveAuraHolderFromStack(spellId);
    }
};

void AddSC_boss_auriaya()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_auriaya";
    pNewScript->GetAI = &GetNewAIInstance<boss_auriayaAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_feral_defender";
    pNewScript->GetAI = &GetNewAIInstance<boss_feral_defenderAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<TerrifyingScreech>("spell_terrifying_screech");
    RegisterSpellScript<FeralEssanceRemoval>("spell_feral_essence_removal");
}

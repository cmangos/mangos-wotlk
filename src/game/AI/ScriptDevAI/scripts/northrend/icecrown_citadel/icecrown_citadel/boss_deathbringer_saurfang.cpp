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
SDName: boss_deathbringer_saurfang
SD%Complete: 100%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_AGGRO                   = -1631028,
    SAY_FALLENCHAMPION          = -1631029,
    SAY_BLOODBEASTS             = -1631030,
    SAY_SLAY_1                  = -1631031,
    SAY_SLAY_2                  = -1631032,
    SAY_BERSERK                 = -1631033,
    SAY_DEATH                   = -1631034,
    EMOTE_FRENZY                = -1631193,
    EMOTE_SCENT                 = -1631194,

    // intro event related
    SPELL_GRIP_OF_AGONY         = 70572,
    SPELL_VEHICLE_HARDCODED     = 46598,

    // aggro spells
    SPELL_BLOOD_LINK            = 72178,
    SPELL_MARK_FALLEN_DAMAGE    = 72256,            // procs 72255 on Saurfang melee attack
    SPELL_RUNE_OF_BLOOD_PROC    = 72408,            // procs 72409 on Saurfang melee attack

    // combat spells
    SPELL_BLOOD_POWER           = 72371,            // triggered by 72195
    // SPELL_BLOOD_POWER_SCALE   = 72370,           // purpose unk
    // SPELL_ZERO_POWER          = 72242,           // included in creature_template_addon

    SPELL_MARK_FALLEN_CHAMPION  = 72254,            // triggers 72293 which procs 72260 on target death
    SPELL_RUNE_OF_BLOOD         = 72410,
    SPELL_BLOOD_NOVA            = 72378,
    SPELL_BOILING_BLOOD         = 72385,

    SPELL_CALL_BLOOD_BEAST_1    = 72172,            // summons 38508
    SPELL_CALL_BLOOD_BEAST_2    = 72173,
    SPELL_CALL_BLOOD_BEAST_3    = 72356,
    SPELL_CALL_BLOOD_BEAST_4    = 72357,
    SPELL_CALL_BLOOD_BEAST_5    = 72358,
    SPELL_SCENT_OF_BLOOD        = 72769,            // triggers 72771 on the blood beasts

    SPELL_BERSERK               = 26662,
    SPELL_FRENZY                = 72737,

    // evade / death spells
    SPELL_REMOVE_MARKS          = 72257,
    SPELL_ACHIEVEMENT           = 72928,
    SPELL_FEIGN_DEATH           = 70628,

    // Summoned spells
    SPELL_RESISTANT_SKIN        = 72723,
    SPELL_BLOOD_LINK_BEAST      = 72176,

    FACTION_ID_UNDEAD           = 974,
    FACTION_ID_FRIENDLY         = 35,
};

enum DeathbringerActions
{
    DEATHBRINGER_BERSERK,
    DEATHBRINGER_FRENZY,
    DEATHBRINGER_RUNE_OF_BLOOD,
    DEATHBRINGER_BOILING_BLOOD,
    DEATHBRINGER_BLOOD_NOVA,
    DEATHBRINGER_BLOOD_BEAST,
    DEATHBRINGER_SCENT_OF_BLOOD,
    DEATHBRINGER_MARK_FALLEN_CHAMPION,
    DEATHBRINGER_ACTION_MAX,
    DEATHBRINGER_DEATH
};

struct boss_deathbringer_saurfangAI : public CombatAI
{
    boss_deathbringer_saurfangAI(Creature* creature) : CombatAI(creature, DEATHBRINGER_ACTION_MAX), m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData()))
    {
        if (m_instance)
        {
            m_bIsHeroicMode = m_instance->IsHeroicDifficulty();
            m_bIs25ManMode = m_instance->Is25ManDifficulty();
        }

        // common actions
        AddCombatAction(DEATHBRINGER_BERSERK, uint32(m_bIsHeroicMode ? 6 : 8 * MINUTE * IN_MILLISECONDS));
        AddCombatAction(DEATHBRINGER_RUNE_OF_BLOOD, 25000u);
        AddCombatAction(DEATHBRINGER_BOILING_BLOOD, 19000u);
        AddCombatAction(DEATHBRINGER_BLOOD_NOVA, 20000u);
        AddCombatAction(DEATHBRINGER_BLOOD_BEAST, 40000u);

        if (m_bIsHeroicMode)
            AddCombatAction(DEATHBRINGER_SCENT_OF_BLOOD, 47000u);

        AddTimerlessCombatAction(DEATHBRINGER_FRENZY, true);
        AddTimerlessCombatAction(DEATHBRINGER_MARK_FALLEN_CHAMPION, true);

        AddCustomAction(DEATHBRINGER_DEATH, true, [&]() { HandleDeath(); });

        m_bIsFakingDeath = false;
        Reset();
    }

    instance_icecrown_citadel* m_instance;

    uint8 m_uiAchievSpellCount;

    bool m_bIs25ManMode;
    bool m_bIsHeroicMode;
    bool m_bIsFakingDeath;

    void Reset() override
    {
        CombatAI::Reset();

        m_uiAchievSpellCount = 0;
        m_creature->SetPower(m_creature->GetPowerType(), 0);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        DoCastSpellIfCan(m_creature, SPELL_BLOOD_LINK, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_MARK_FALLEN_DAMAGE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_RUNE_OF_BLOOD_PROC, CAST_TRIGGERED);

        if (m_instance)
            m_instance->SetData(TYPE_DEATHBRINGER_SAURFANG, IN_PROGRESS);
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        if (urand(0, 1))
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void EnterEvadeMode() override
    {
        if (m_bIsFakingDeath)
            return;

        CombatAI::EnterEvadeMode();
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (m_bIsFakingDeath)
        {
            uiDamage = 0;
            return;
        }

        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;
            m_bIsFakingDeath = true;

            // yell death and change flags
            DoScriptText(SAY_DEATH, m_creature);

            ClearCombatOnlyRoot();
            m_creature->RemoveAllAurasOnEvade();
            m_creature->CombatStopWithPets(true);

            DoCastSpellIfCan(m_creature, SPELL_REMOVE_MARKS, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_ACHIEVEMENT, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_FEIGN_DEATH, CAST_TRIGGERED);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->SetImmuneToPlayer(true);

            if (m_instance)
                m_instance->SetData(TYPE_DEATHBRINGER_SAURFANG, DONE);

            // change faction and start outro
            ResetTimer(DEATHBRINGER_DEATH, 5 * IN_MILLISECONDS);

            // stop combat and movement
            SetCombatScriptStatus(true);
            m_creature->SetTarget(nullptr);
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveIdle();
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_DEATHBRINGER_SAURFANG, FAIL);

        DoCastSpellIfCan(m_creature, SPELL_REMOVE_MARKS, CAST_TRIGGERED);
    }

    void JustSummoned(Creature* summoned) override
    {
        summoned->CastSpell(summoned, SPELL_RESISTANT_SKIN, TRIGGERED_OLD_TRIGGERED);
        summoned->CastSpell(summoned, SPELL_BLOOD_LINK_BEAST, TRIGGERED_OLD_TRIGGERED);

        // Note: the summoned should be activated only after 2-3 seconds after summon - can be done in eventAI
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            summoned->AI()->AttackStart(pTarget);
    }

    void HandleDeath()
    {
        // inform creatures to start outro event
        SendAIEventAround(AI_EVENT_CUSTOM_EVENTAI_C, m_creature, 0, 30.0f);

        m_creature->SetFactionTemporary(FACTION_ID_FRIENDLY, TEMPFACTION_NONE);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case DEATHBRINGER_BERSERK:
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    DisableCombatAction(action);
                }
                break;
            case DEATHBRINGER_FRENZY:
                if (m_creature->GetHealthPercent() <= 30.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
                    {
                        DoScriptText(EMOTE_FRENZY, m_creature);
                        DisableCombatAction(action);
                    }
                }
                break;
            case DEATHBRINGER_RUNE_OF_BLOOD:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_RUNE_OF_BLOOD) == CAST_OK)
                    ResetCombatAction(action, 25000);
                break;
            case DEATHBRINGER_BOILING_BLOOD:
                if (DoCastSpellIfCan(m_creature, SPELL_BOILING_BLOOD) == CAST_OK)
                    ResetCombatAction(action, 15000);
                break;
            case DEATHBRINGER_BLOOD_NOVA:
                if (DoCastSpellIfCan(m_creature, SPELL_BLOOD_NOVA) == CAST_OK)
                    ResetCombatAction(action, 20000);
                break;
            case DEATHBRINGER_BLOOD_BEAST:
                DoScriptText(SAY_BLOODBEASTS, m_creature);

                DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_1, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_2, CAST_TRIGGERED);

                if (m_bIs25ManMode)
                {
                    DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_3, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_4, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_5, CAST_TRIGGERED);
                }

                ResetCombatAction(action, 40000);

                if (m_bIsHeroicMode)
                    ResetCombatAction(DEATHBRINGER_SCENT_OF_BLOOD, 7000);
                break;
            case DEATHBRINGER_SCENT_OF_BLOOD:
                if (DoCastSpellIfCan(m_creature, SPELL_SCENT_OF_BLOOD) == CAST_OK)
                {
                    DoScriptText(EMOTE_SCENT, m_creature);
                    ResetCombatAction(action, 40000);
                }
                break;
            case DEATHBRINGER_MARK_FALLEN_CHAMPION:
                if (m_creature->GetPower(m_creature->GetPowerType()) == 100)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_MARK_FALLEN_CHAMPION) == CAST_OK)
                    {
                        DoScriptText(SAY_FALLENCHAMPION, m_creature);
                        m_creature->RemoveAurasDueToSpell(SPELL_BLOOD_POWER);
                        m_creature->SetPower(m_creature->GetPowerType(), 0);

                        // check for achievement fail
                        ++m_uiAchievSpellCount;

                        if (m_instance)
                        {
                            if (m_uiAchievSpellCount == (m_bIs25ManMode ? 5 : 3))
                                m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_MADE_A_MESS, false);
                        }
                    }
                }
                break;
        }
    }
};

/*######
## spell_grip_of_agony_aura - 70572
######*/

struct spell_grip_of_agony_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        target->SetLevitate(apply);
        target->GetMotionMaster()->Clear(false, true);

        // on apply move up with 10
        if (apply)
            target->GetMotionMaster()->MovePoint(0, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ() + 10);
        // on remove fall to the ground
        else
            target->GetMotionMaster()->MoveFall();
    }
};

/*######
## spell_mark_fallen_champion - 72254
######*/

struct spell_mark_fallen_champion : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster || !target->IsPlayer())
            return;

        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        if (!target->HasAura(spellId))
            caster->CastSpell(target, spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_blood_link - 72202
######*/

struct spell_blood_link : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->CastSpell(target, 72195, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_deathbringer_saurfang()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_deathbringer_saurfang";
    pNewScript->GetAI = &GetNewAIInstance<boss_deathbringer_saurfangAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_grip_of_agony_aura>("spell_grip_of_agony_aura");
    RegisterSpellScript<spell_mark_fallen_champion>("spell_mark_fallen_champion");
    RegisterSpellScript<spell_blood_link>("spell_blood_link");
}

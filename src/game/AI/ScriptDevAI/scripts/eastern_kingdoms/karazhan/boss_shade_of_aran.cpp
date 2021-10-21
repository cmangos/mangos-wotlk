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
SDName: Boss_Shade_of_Aran
SD%Complete: 95
SDComment: When drinking mana, it should remove all negative damage auras and should sit. Timers may need adjustments.
SDCategory: Karazhan
EndScriptData */

/* Pre-nerf Changes
Add Dragon's Breath ability - used shortly after Flame Wreath dissipates (random target in melee range).
Patch_2.1.0 - Shade of Aran will no longer cast Dragon's Breath.
*/

#include "AI/ScriptDevAI/include/sc_common.h"
#include "karazhan.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Globals/ObjectMgr.h"

enum
{
    SAY_AGGRO1                  = -1532073,
    SAY_AGGRO2                  = -1532074,
    SAY_AGGRO3                  = -1532075,
    SAY_FLAMEWREATH1            = -1532076,
    SAY_FLAMEWREATH2            = -1532077,
    SAY_BLIZZARD1               = -1532078,
    SAY_BLIZZARD2               = -1532079,
    SAY_EXPLOSION1              = -1532080,
    SAY_EXPLOSION2              = -1532081,
    SAY_DRINK                   = -1532082,                 // Low Mana / AoE Pyroblast
    SAY_ELEMENTALS              = -1532083,
    SAY_KILL1                   = -1532084,
    SAY_KILL2                   = -1532085,
    SAY_TIMEOVER                = -1532086,
    SAY_DEATH                   = -1532087,
    SAY_ATIESH                  = -1532088,                 // Atiesh is equipped by a raid member
    SAY_EXPLOSION_EMOTE         = -1532139,

    // basic spells
    SPELL_FROSTBOLT             = 29954,
    SPELL_FIREBALL              = 29953,
    SPELL_ARCANE_MISSILES       = 29955,
    SPELL_DRAGONS_BREATH        = 29964,                   // not used since 2.1.0
    SPELL_CHAINS_OF_ICE         = 29991,
    SPELL_COUNTERSPELL          = 29961,
    // SPELL_COMBUSTION          = 29977,                   // spell not confirmed
    // SPELL_PRESENCE_OF_MIND    = 29976,                   // spell not confirmed
    // SPELL_WATER_BREAK         = 39177,                   // purpose unk

    // low mana spells
    SPELL_MASS_POLYMORPH        = 29963,
    SPELL_CONJURE_WATER         = 29975,
    SD_SPELL_DRINK              = 30024,
    SPELL_MANA_POTION           = 32453,
    SPELL_PYROBLAST             = 29978,
    SPELL_DISPEL_BLIZZARD       = 29970,

    // super spells
    SPELL_FLAME_WREATH          = 30004,                // triggers 29946 on targets
    SPELL_SUMMON_BLIZZARD       = 29969,                // script target on npc 17161 - triggers spell 29952 on target
    SPELL_BLINK_CENTER          = 29967,
    SPELL_MASSIVE_MAGNETIC_PULL = 29979,                // triggers 30010 on target
    SPELL_MASS_SLOW             = 30035,
    SPELL_ARCANE_EXPLOSION      = 29973,

    // summon elemental spells
    SPELL_SUMMON_WATER_ELEM_1   = 29962,
    SPELL_SUMMON_WATER_ELEM_2   = 37051,
    SPELL_SUMMON_WATER_ELEM_3   = 37052,
    SPELL_SUMMON_WATER_ELEM_4   = 37053,

    // Creatures
    NPC_WATER_ELEMENTAL         = 17167,
    NPC_SHADOW_OF_ARAN          = 18254,

    MAX_SHADOWS_OF_ARAN         = 5,                    // this is not confirmed

    NORMAL_SPELL_COUNT          = 3,

    ITEM_ATIESH_1               = 22589,
    ITEM_ATIESH_2               = 22632,
    ITEM_ATIESH_3               = 22631,
    ITEM_ATIESH_4               = 22630,
};

static std::set<uint32> atieshStaves = { ITEM_ATIESH_1, ITEM_ATIESH_2, ITEM_ATIESH_3, ITEM_ATIESH_4 };

enum SuperSpells
{
    SUPER_FLAME_WREATH  = 0,
    SUPER_BLIZZARD      = 1,
    SUPER_ARCANE_EXPL   = 2,
};

enum AranActions // order based on priority
{
    ARAN_ACTION_DRINK,
    ARAN_ACTION_POTION,
    ARAN_ACTION_ELEMENTALS,
    ARAN_ACTION_BERSERK,
    ARAN_ACTION_DRAGONS_BREATH,
    ARAN_ACTION_SUPERSPELL,
    ARAN_ACTION_SECONDARY_SPELL,
    ARAN_ACTION_PRIMARY_SPELL,
    ARAN_ACTION_MAX,
    ARAN_DRINKING_STAGES,
};

struct boss_aranAI : public CombatAI
{
    boss_aranAI(Creature* creature) : CombatAI(creature, ARAN_ACTION_MAX), m_instance(static_cast<instance_karazhan*>(creature->GetInstanceData())), m_atiesh(false)
    {
        AddTimerlessCombatAction(ARAN_ACTION_DRINK, true);
        AddTimerlessCombatAction(ARAN_ACTION_POTION, true);
        AddTimerlessCombatAction(ARAN_ACTION_ELEMENTALS, true);
        AddCombatAction(ARAN_ACTION_BERSERK, uint32(12 * MINUTE * IN_MILLISECONDS));
        AddCombatAction(ARAN_ACTION_DRAGONS_BREATH, true);
        AddCombatAction(ARAN_ACTION_SUPERSPELL, 35000u);
        AddCombatAction(ARAN_ACTION_SECONDARY_SPELL, 5000u);
        AddCombatAction(ARAN_ACTION_PRIMARY_SPELL, 0u);
        AddCustomAction(ARAN_DRINKING_STAGES, true, [&]() {HandleDrinking(); });
        SetRangedMode(true, 100.f, TYPE_FULL_CASTER);
        AddMainSpell(SPELL_FROSTBOLT);
        AddMainSpell(SPELL_FIREBALL);
        AddMainSpell(SPELL_ARCANE_MISSILES);
    }

    instance_karazhan* m_instance;

    uint8 m_uiLastSuperSpell;

    uint8 m_uiManaRecoveryStage;

    bool m_bDrinkInterrupted;
    bool m_atiesh;

    std::vector<uint32> m_choiceVector;

    void Reset() override
    {
        CombatAI::Reset();
        m_uiLastSuperSpell = urand(SUPER_FLAME_WREATH, SUPER_ARCANE_EXPL);

        m_uiManaRecoveryStage   = 0;

        m_bDrinkInterrupted     = false;

        SetCombatMovement(true);

        for (ObjectGuid guid : m_instance->GetAranTeleportNPCs())
            if (Creature* teleport = m_creature->GetMap()->GetCreature(guid))
            {
                if (teleport->GetCreatureInfo()->Entry == NPC_SHADOW_OF_ARAN) // avoid case on spawn
                {
                    teleport->ResetEntry();
                    teleport->AI()->EnterEvadeMode();
                    teleport->AIM_Initialize();
                }
            }
    }

    uint32 GetNormalSpellId(uint32 index) const
    {
        switch (index)
        {
            default:
            case 0: return SPELL_ARCANE_MISSILES;
            case 1: return SPELL_FIREBALL;
            case 2: return SPELL_FROSTBOLT;
        }
    }

    static uint32 GetNormalSpellCooldown(uint32 spellId)
    {
        switch (spellId)
        {
            default:
            case SPELL_ARCANE_MISSILES: return 7000;
            case SPELL_FIREBALL:        return 3000;
            case SPELL_FROSTBOLT:       return 3000;
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        CombatAI::MoveInLineOfSight(who);
        if (!m_atiesh && who->IsPlayer())
        {
            Player* player = static_cast<Player*>(who);
            if (Item* weapon = player->GetWeaponForAttack(BASE_ATTACK))
            {
                if (atieshStaves.find(weapon->GetEntry()) != atieshStaves.end())
                {
                    m_atiesh = true;
                    DoScriptText(SAY_ATIESH, m_creature, who);
                }
            }
        }
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void JustDied(Unit* /*victim*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        // Remove the summoned elementals - which are considered guardians
        m_creature->RemoveGuardians();

        if (m_instance)
            m_instance->SetData(TYPE_ARAN, DONE);
    }

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        if (m_instance)
            m_instance->SetData(TYPE_ARAN, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        // Remove the summoned elementals - which are considered guardians
        m_creature->RemoveGuardians();

        if (m_instance)
            m_instance->SetData(TYPE_ARAN, FAIL);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MANA_POTION) == CAST_OK)
            {
                m_creature->RemoveAurasDueToSpell(SD_SPELL_DRINK);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                ResetTimer(ARAN_DRINKING_STAGES, 1000);
                m_uiManaRecoveryStage = 2;
                m_bDrinkInterrupted = true;
            }
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_WATER_ELEMENTAL:
                summoned->SetInCombatWithZone();
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                {
                    summoned->AddThreat(target, 100000.f);
                    summoned->AI()->AttackStart(target);
                }
                break;
        }
    }

    void HandleDrinking()
    {
        uint32 timer = 0;
        switch (m_uiManaRecoveryStage)
        {
            case 0:
                DoCastSpellIfCan(nullptr, SPELL_CONJURE_WATER);
                timer = 2000;
                break;
            case 1:
                DoCastSpellIfCan(nullptr, SD_SPELL_DRINK);
                m_creature->SetStandState(UNIT_STAND_STATE_SIT);
                timer = 5000;
                break;
            case 2:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                timer = 1000;
                break;
            case 3:
                if (DoCastSpellIfCan(nullptr, SPELL_PYROBLAST) == CAST_OK)
                {
                    SetMeleeEnabled(true);
                    m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                    SetCombatMovement(true);
                    SetCombatScriptStatus(false);
                }
                break;
        }
        ++m_uiManaRecoveryStage;
        if (timer)
            ResetTimer(ARAN_DRINKING_STAGES, timer);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ARAN_ACTION_DRINK:
                if (m_creature->GetPowerType() == POWER_MANA && (m_creature->GetPower(POWER_MANA) * 100 / m_creature->GetMaxPower(POWER_MANA)) < 20)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_MASS_POLYMORPH) == CAST_OK)
                    {
                        m_creature->CastSpell(nullptr, SPELL_DISPEL_BLIZZARD, TRIGGERED_OLD_TRIGGERED);
                        DoScriptText(SAY_DRINK, m_creature);
                        SetCombatMovement(false);
                        SetCombatScriptStatus(true);
                        SetMeleeEnabled(false);
                        m_creature->SetTarget(nullptr);
                        m_uiManaRecoveryStage = 0;
                        ResetTimer(ARAN_DRINKING_STAGES, 2000);
                        m_bDrinkInterrupted = false;
                        SetActionReadyStatus(action, false);
                    }
                }
                return;
            case ARAN_ACTION_POTION:
            {
                if (m_creature->GetPowerPercent() < 3.f) // always drink when low
                    DoCastSpellIfCan(m_creature, SPELL_MANA_POTION);
                break;
            }
            case ARAN_ACTION_ELEMENTALS:
            {
                if (m_creature->GetHealthPercent() > 40.0f)
                    return;

                DoCastSpellIfCan(nullptr, SPELL_SUMMON_WATER_ELEM_1, CAST_TRIGGERED);
                DoCastSpellIfCan(nullptr, SPELL_SUMMON_WATER_ELEM_2, CAST_TRIGGERED);
                DoCastSpellIfCan(nullptr, SPELL_SUMMON_WATER_ELEM_3, CAST_TRIGGERED);
                DoCastSpellIfCan(nullptr, SPELL_SUMMON_WATER_ELEM_4, CAST_TRIGGERED);

                DoScriptText(SAY_ELEMENTALS, m_creature);

                SetActionReadyStatus(action, false);
                return;
            }
            case ARAN_ACTION_BERSERK:
            {
                for (ObjectGuid guid : m_instance->GetAranTeleportNPCs())
                    if (Creature* teleport = m_creature->GetMap()->GetCreature(guid))
                    {
                        teleport->UpdateEntry(NPC_SHADOW_OF_ARAN);
                        teleport->AIM_Initialize();
                        teleport->SetInCombatWithZone();
                    }

                DoScriptText(SAY_TIMEOVER, m_creature);
                DisableCombatAction(action);
                return;
            }

            case ARAN_ACTION_DRAGONS_BREATH:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), (SELECT_FLAG_PLAYER | SELECT_FLAG_IN_MELEE_RANGE)))
                {
                    DoCastSpellIfCan(target, SPELL_DRAGONS_BREATH, CAST_TRIGGERED);
                    DisableCombatAction(action);
                    DelayCombatAction(ARAN_ACTION_SUPERSPELL, 6000); // Duration
                }
                return;
            }
            case ARAN_ACTION_SUPERSPELL:
            {
                uint8 uiAvailableSpell = urand(SUPER_FLAME_WREATH, SUPER_ARCANE_EXPL);

                // randomize so it won't be the same spell twice in a row
                while (uiAvailableSpell == m_uiLastSuperSpell)
                    uiAvailableSpell = urand(SUPER_FLAME_WREATH, SUPER_ARCANE_EXPL);

                m_uiLastSuperSpell = uiAvailableSpell;

                DoCastSpellIfCan(m_creature, SPELL_BLINK_CENTER, CAST_TRIGGERED);

                switch (m_uiLastSuperSpell)
                {
                    case SUPER_ARCANE_EXPL:
                        DoCastSpellIfCan(m_creature, SPELL_MASSIVE_MAGNETIC_PULL, CAST_TRIGGERED);
                        DoScriptText(urand(0, 1) ? SAY_EXPLOSION1 : SAY_EXPLOSION2, m_creature);
                        DoCastSpellIfCan(m_creature, SPELL_MASS_SLOW, CAST_TRIGGERED);
                        if (DoCastSpellIfCan(m_creature, SPELL_ARCANE_EXPLOSION) == CAST_OK)
                            DoScriptText(SAY_EXPLOSION_EMOTE, m_creature);
                        break;
                    case SUPER_FLAME_WREATH:
                        if (DoCastSpellIfCan(m_creature, SPELL_FLAME_WREATH) == CAST_OK)
                        {
                            DoScriptText(urand(0, 1) ? SAY_FLAMEWREATH1 : SAY_FLAMEWREATH2, m_creature);
#ifdef PRENERF_2_0_3
                            ResetCombatAction(ARAN_ACTION_DRAGONS_BREATH, 27000);
#endif
                        }
                        break;
                    case SUPER_BLIZZARD:
                        if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_BLIZZARD) == CAST_OK)
                            DoScriptText(urand(0, 1) ? SAY_BLIZZARD1 : SAY_BLIZZARD2, m_creature);
                        break;
                }
                ResetCombatAction(action, 30000);
                return;
            }
            case ARAN_ACTION_SECONDARY_SPELL:
            {
                CanCastResult spellResult = CAST_FAIL_OTHER;

                switch (urand(0, 1))
                {
                    case 0:
                        spellResult = DoCastSpellIfCan(nullptr, SPELL_COUNTERSPELL);
                        break;
                    case 1:
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                            spellResult = DoCastSpellIfCan(target, SPELL_CHAINS_OF_ICE);
                        break;
                }
                if (spellResult == CAST_OK)
                    ResetCombatAction(action, urand(5000, 20000));
                return;
            }
            case ARAN_ACTION_PRIMARY_SPELL:
            {
                Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER);
                if (!target)
                    break;

                m_choiceVector.clear();

                for (uint32 i = 0; i < NORMAL_SPELL_COUNT; ++i)
                {
                    uint32 spellId = GetNormalSpellId(i);
                    if (m_creature->IsSpellReady(spellId))
                        m_choiceVector.push_back(i);
                }

                if (m_choiceVector.size() != 0)
                {
                    uint32 currentSpellIndex = urand(0, m_choiceVector.size() - 1);
                    uint32 currentSpellId = GetNormalSpellId(currentSpellIndex);
                    if (DoCastSpellIfCan(target, currentSpellId) == CAST_OK)
                        ResetCombatAction(action, 2000);
                }
            }
        }
    }
};

struct SummonBlizzard : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* target = spell->GetUnitTarget())
            target->CastSpell(nullptr, 29952, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, spell->GetCaster()->GetObjectGuid());
    }
};

struct DispelBlizzard : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* target = spell->GetUnitTarget())
            target->RemoveAurasDueToSpell(29952);
    }
};

struct MassiveMagneticPull : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* target = spell->GetUnitTarget())
            spell->GetCaster()->CastSpell(target, 30010, TRIGGERED_OLD_TRIGGERED);
    }
};

struct FlameWreath : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* target = spell->GetUnitTarget())
            if (target->IsPlayer())
                spell->GetCaster()->CastSpell(target, 29946, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_shade_of_aran()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_shade_of_aran";
    pNewScript->GetAI = &GetNewAIInstance<boss_aranAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SummonBlizzard>("spell_summon_blizzard");
    RegisterSpellScript<DispelBlizzard>("spell_dispel_blizzard");
    RegisterSpellScript<MassiveMagneticPull>("spell_massive_magnetic_pull");
    RegisterSpellScript<FlameWreath>("spell_flame_wreath");

    sObjectMgr.AddCreatureCooldown(NPC_SHADOW_OF_ARAN, SPELL_FROSTBOLT, boss_aranAI::GetNormalSpellCooldown(SPELL_FROSTBOLT), boss_aranAI::GetNormalSpellCooldown(SPELL_FROSTBOLT));
    sObjectMgr.AddCreatureCooldown(NPC_SHADOW_OF_ARAN, SPELL_FIREBALL, boss_aranAI::GetNormalSpellCooldown(SPELL_FIREBALL), boss_aranAI::GetNormalSpellCooldown(SPELL_FIREBALL));
    sObjectMgr.AddCreatureCooldown(NPC_SHADOW_OF_ARAN, SPELL_ARCANE_MISSILES, boss_aranAI::GetNormalSpellCooldown(SPELL_ARCANE_MISSILES), boss_aranAI::GetNormalSpellCooldown(SPELL_ARCANE_MISSILES));
}

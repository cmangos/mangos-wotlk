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

Patches
2.1.0 - Shade of Aran will no longer cast Dragon's Breath.
*/

#include "AI/ScriptDevAI/include/precompiled.h"
#include "karazhan.h"

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
};

enum SuperSpells
{
    SUPER_FLAME_WREATH  = 0,
    SUPER_BLIZZARD      = 1,
    SUPER_ARCANE_EXPL   = 2,
};

enum AranActions // order based on priority
{
    ARAN_ACTION_DRINK,
    ARAN_ACTION_ELEMENTALS,
    ARAN_ACTION_BERSERK,
    ARAN_ACTION_DRAGONS_BREATH,
    ARAN_ACTION_SUPERSPELL,
    ARAN_ACTION_SECONDARY_SPELL,
    ARAN_ACTION_PRIMARY_SPELL,
    ARAN_ACTION_MAX,
};

struct boss_aranAI : public ScriptedAI
{
    boss_aranAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_karazhan*)pCreature->GetInstanceData();
        Reset();
    }

    instance_karazhan* m_pInstance;

    uint32 m_uiSecondarySpellTimer;
    uint32 m_uiNormalCastTimer;
    uint32 m_uiSuperCastTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiDragonsBreathTimer;

    uint8 m_uiLastSuperSpell;
    uint32 m_normalSpellCooldown[NORMAL_SPELL_COUNT];

    uint32 m_uiManaRecoveryTimer;
    uint8 m_uiManaRecoveryStage;

    bool m_bElementalsSpawned;
    bool m_bIsDrinking;
    bool m_bDrinkInterrupted;

    bool m_actionReadyStatus[ARAN_ACTION_MAX];

    std::vector<uint32> m_choiceVector;

    void Reset() override
    {
        m_uiLastSuperSpell = urand(SUPER_FLAME_WREATH, SUPER_ARCANE_EXPL);

        m_uiSecondarySpellTimer = 5000;
        m_uiNormalCastTimer     = 0;
        m_uiSuperCastTimer      = 35000;
        m_uiManaRecoveryTimer   = 0;
        m_uiDragonsBreathTimer  = 0;
        m_uiManaRecoveryStage   = 0;
        m_uiBerserkTimer        = 12 * MINUTE * IN_MILLISECONDS;

        m_bElementalsSpawned    = false;
        m_bIsDrinking           = false;
        m_bDrinkInterrupted     = false;

        m_attackDistance        = 100.f;

        for (bool& m_actionReadyStatu : m_actionReadyStatus)
            m_actionReadyStatu = false;

        m_actionReadyStatus[ARAN_ACTION_PRIMARY_SPELL] = true;

        for (unsigned int& i : m_normalSpellCooldown)
            i = 0;

        SetCombatMovement(true);

        for (ObjectGuid guid : m_pInstance->GetAranTeleportNPCs())
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

    uint32 GetNormalSpellCooldown(uint32 spellId) const
    {
        switch (spellId)
        {
            default:
            case SPELL_ARCANE_MISSILES: return 7000;
            case SPELL_FIREBALL:        return 3000;
            case SPELL_FROSTBOLT:       return 3000;
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void JustDied(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        // Remove the summoned elementals - which are considered guardians
        m_creature->RemoveGuardians();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ARAN, DONE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ARAN, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        // Remove the summoned elementals - which are considered guardians
        m_creature->RemoveGuardians();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ARAN, FAIL);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MANA_POTION) == CAST_OK)
            {
                m_creature->RemoveAurasDueToSpell(SD_SPELL_DRINK);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                m_uiManaRecoveryTimer = 1000;
                m_uiManaRecoveryStage = 2;
                m_bDrinkInterrupted = true;
            }
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_WATER_ELEMENTAL:
                pSummoned->SetInCombatWithZone();
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    pSummoned->AddThreat(pTarget, 100000.f);
                break;
        }
    }

    void UpdateActions()
    {
        for (uint32 i = 0; i < ARAN_ACTION_MAX; ++i)
        {
            if (!m_actionReadyStatus[i])
            {
                switch (i)
                {
                    // conditional actions
                    case ARAN_ACTION_DRINK:
                        if (!m_bIsDrinking && m_creature->GetPowerType() == POWER_MANA && (m_creature->GetPower(POWER_MANA) * 100 / m_creature->GetMaxPower(POWER_MANA)) < 20)
                            m_actionReadyStatus[i] = true;
                        break;
                    case ARAN_ACTION_ELEMENTALS:
                        if (!m_bElementalsSpawned && m_creature->GetHealthPercent() < 40.0f)
                            m_actionReadyStatus[i] = true;
                        break;
                    // timed actions
                    case ARAN_ACTION_DRAGONS_BREATH:
                        break;
                    case ARAN_ACTION_SUPERSPELL:
                        break;
                    case ARAN_ACTION_SECONDARY_SPELL:
                        break;
                    case ARAN_ACTION_PRIMARY_SPELL:
                        break;
                }
            }
        }
    }

    void ExecuteActions()
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < ARAN_ACTION_MAX; ++i)
        {
            if (m_actionReadyStatus[i])
            {
                switch (i)
                {
                    case ARAN_ACTION_DRINK:
                        if (DoCastSpellIfCan(m_creature, SPELL_MASS_POLYMORPH) == CAST_OK)
                        {
                            m_creature->CastSpell(nullptr, SPELL_DISPEL_BLIZZARD, TRIGGERED_OLD_TRIGGERED);
                            DoScriptText(SAY_DRINK, m_creature);
                            SetCombatMovement(false);
                            SetCombatScriptStatus(true);
                            SetMeleeEnabled(false);
                            m_creature->SetTarget(nullptr);
                            m_uiManaRecoveryStage = 0;
                            m_uiManaRecoveryTimer = 2000;
                            m_bDrinkInterrupted = false;
                            m_bIsDrinking = true;
                            m_actionReadyStatus[i] = false;
                            return; // successful end of action
                        }
                        break; // unsuccessful end of action - try again next cycle
                    case ARAN_ACTION_ELEMENTALS:
                    {
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_WATER_ELEM_1, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_WATER_ELEM_2, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_WATER_ELEM_3, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_WATER_ELEM_4, CAST_TRIGGERED);

                        DoScriptText(SAY_ELEMENTALS, m_creature);

                        m_bElementalsSpawned = true;
                        m_actionReadyStatus[i] = false;
                        return;
                    }
                    case ARAN_ACTION_BERSERK:
                    {
                        for (ObjectGuid guid : m_pInstance->GetAranTeleportNPCs())
                            if (Creature* teleport = m_creature->GetMap()->GetCreature(guid))
                            {
                                teleport->UpdateEntry(NPC_SHADOW_OF_ARAN);
                                teleport->AIM_Initialize();
                                teleport->SetInCombatWithZone();
                            }

                        DoScriptText(SAY_TIMEOVER, m_creature);
                        m_uiBerserkTimer = 0;
                        m_actionReadyStatus[i] = false;
                        return;
                    }
                    case ARAN_ACTION_DRAGONS_BREATH:
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER))
                        {
                            DoCastSpellIfCan(pTarget, SPELL_DRAGONS_BREATH, CAST_TRIGGERED);
                            m_uiDragonsBreathTimer = 0;
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
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
                                    m_uiDragonsBreathTimer = 27000;
                                }
                                break;
                            case SUPER_BLIZZARD:
                                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_BLIZZARD) == CAST_OK)
                                    DoScriptText(urand(0, 1) ? SAY_BLIZZARD1 : SAY_BLIZZARD2, m_creature);
                                break;
                        }
                        m_uiSuperCastTimer = 30000;
                        m_actionReadyStatus[i] = false;
                        return;
                    }
                    case ARAN_ACTION_SECONDARY_SPELL:
                    {
                        CanCastResult spellResult = CAST_FAIL_OTHER;

                        switch (urand(0, 1))
                        {
                            case 0:
                                spellResult = DoCastSpellIfCan(m_creature, SPELL_COUNTERSPELL);
                                break;
                            case 1:
                                if (Unit* pUnit = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                                    spellResult = DoCastSpellIfCan(pUnit, SPELL_CHAINS_OF_ICE);
                                break;
                        }
                        if (spellResult == CAST_OK)
                            m_uiSecondarySpellTimer = urand(5000, 20000);
                        else
                            break;
                        m_actionReadyStatus[i] = false;
                        return;
                    }
                    case ARAN_ACTION_PRIMARY_SPELL:
                    {
                        Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER);
                        if (!pTarget)
                            break;

                        m_choiceVector.clear();

                        for (uint32 i = 0; i < NORMAL_SPELL_COUNT; ++i)
                        {
                            uint32 spellId = GetNormalSpellId(i);
                            if (m_normalSpellCooldown[i] == 0 && m_creature->IsSpellReady(spellId))
                                m_choiceVector.push_back(i);
                        }

                        if (m_choiceVector.size() == 0)
                        {
                            m_attackDistance = 0; // go into melee range
                            DoStartMovement(m_creature->getVictim());
                            return;
                        }
                        else
                        {
                            uint32 currentSpellIndex = urand(0, m_choiceVector.size() - 1);
                            uint32 currentSpellId = GetNormalSpellId(currentSpellIndex);
                            DoCastSpellIfCan(pTarget, currentSpellId);
                            m_normalSpellCooldown[currentSpellIndex] = GetNormalSpellCooldown(currentSpellId);
                            m_attackDistance = 100.f;
                            DoStartMovement(m_creature->getVictim());
                            return;
                        }
                    }
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_bIsDrinking)
        {
            // Do the mana recovery process
            if (m_uiManaRecoveryTimer < uiDiff)
            {
                switch (m_uiManaRecoveryStage)
                {
                    case 0:
                        if (DoCastSpellIfCan(m_creature, SPELL_CONJURE_WATER) == CAST_OK)
                        {
                            m_uiManaRecoveryTimer = 2000;
                            ++m_uiManaRecoveryStage;
                        }
                        break;
                    case 1:
                        if (DoCastSpellIfCan(m_creature, SD_SPELL_DRINK) == CAST_OK)
                        {
                            m_creature->SetStandState(UNIT_STAND_STATE_SIT);
                            m_uiManaRecoveryTimer = 5000;
                            ++m_uiManaRecoveryStage;
                        }
                        break;
                    case 2:
                        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        m_uiManaRecoveryTimer = 1000;
                        ++m_uiManaRecoveryStage;
                        break;
                    case 3:
                        if (DoCastSpellIfCan(m_creature, SPELL_PYROBLAST) == CAST_OK)
                        {
                            SetCombatMovement(true);
                            SetCombatScriptStatus(false);
                            SetMeleeEnabled(true);
                            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                            m_uiManaRecoveryTimer = 2000;
                            m_bIsDrinking = false;
                        }
                        break;
                }
            }
            else
                m_uiManaRecoveryTimer -= uiDiff;

            // no other spells during mana recovery
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSuperCastTimer < uiDiff)
        {
            m_uiSuperCastTimer = 0;
            m_actionReadyStatus[ARAN_ACTION_SUPERSPELL] = true;
        }
        else
            m_uiSuperCastTimer -= uiDiff;

        //// Normal spell casts
        //if (m_uiNormalCastTimer < uiDiff)
        //{
        //    m_uiNormalCastTimer = 0;
        //    m_actionReadyStatus[ARAN_ACTION_PRIMARY_SPELL] = true;
        //}
        //else
        //    m_uiNormalCastTimer -= uiDiff;

        // Secondary spells
        if (m_uiSecondarySpellTimer < uiDiff)
        {
            m_uiSecondarySpellTimer = 0;
            m_actionReadyStatus[ARAN_ACTION_SECONDARY_SPELL] = true;
        }
        else
            m_uiSecondarySpellTimer -= uiDiff;

        if (m_uiDragonsBreathTimer)
        {
            if (m_uiDragonsBreathTimer <= uiDiff)
            {
                m_uiDragonsBreathTimer = 0;
                m_actionReadyStatus[ARAN_ACTION_DRAGONS_BREATH] = true;
            }
            else
                m_uiDragonsBreathTimer -= uiDiff;
        }

        // Berserk timer - the summons position is guesswork
        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                m_uiBerserkTimer = 0;
                m_actionReadyStatus[ARAN_ACTION_BERSERK] = true;
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        for (unsigned int& i : m_normalSpellCooldown)
        {
            if (i)
            {
                if (i <= uiDiff)
                    i = 0;
                else
                    i -= uiDiff;
            }
        }

        UpdateActions();
        ExecuteActions();

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_aran(Creature* pCreature)
{
    return new boss_aranAI(pCreature);
}

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_shade_of_aran_blizzardAI : public ScriptedAI
{
    npc_shade_of_aran_blizzardAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_shade_of_aran_blizzard(Creature* pCreature)
{
    return new npc_shade_of_aran_blizzardAI(pCreature);
}

void AddSC_boss_shade_of_aran()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_shade_of_aran";
    pNewScript->GetAI = &GetAI_boss_aran;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shade_of_aran_blizzard";
    pNewScript->GetAI = &GetAI_npc_shade_of_aran_blizzard;
    pNewScript->RegisterSelf();
}

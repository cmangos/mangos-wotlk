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
SDName: Boss_Malacrass
SD%Complete: 80
SDComment: Contain adds and adds selection; Stolen abilities timers need improvement
SDCategory: Zul'Aman
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "zulaman.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                   = -1568045,
    SAY_ENRAGE                  = -1568046,
    SAY_KILL1                   = -1568047,
    SAY_KILL2                   = -1568048,
    SAY_SOUL_SIPHON             = -1568049,
    SAY_DRAIN_POWER             = -1568050,
    SAY_SPIRIT_BOLTS            = -1568051,
    SAY_ADD_DIED1               = -1568052,
    SAY_ADD_DIED2               = -1568053,
    SAY_ADD_DIED3               = -1568054,
    SAY_DEATH                   = -1568055,

    /* Notes about the event:
     * The boss casts siphon soul right after he finishes the spirit bolts channel, which takes 10 sec
     * The siphon soul is a channeled spell for 30 sec during which the boss uses some class abilities of the target
     * Basically the boss casts a dummy spell which chooses a random target on which it casts the actuall channel spell
     * The drain power spell acts as a enrage timer. It's cast each 30 seconds after the boss' health is below 80%
     */
    SPELL_SPIRIT_BOLTS          = 43383,
    SPELL_SIPHON_SOUL_DUMMY     = 43498,
    SPELL_SIPHON_SOUL           = 43501,
    SPELL_DRAIN_POWER           = 44131,

    // for various powers he uses after using soul drain
    // Death Knight
    SPELL_DK_DEATH_AND_DECAY    = 61603,
    SPELL_DK_PLAGUE_STRIKE      = 61600,
    SPELL_DK_MARK_OF_BLOOD      = 61606,

    // Druid
    SPELL_DR_THORNS             = 43420,
    SPELL_DR_LIFEBLOOM          = 43421,
    SPELL_DR_MOONFIRE           = 43545,

    // Hunter
    SPELL_HU_EXPLOSIVE_TRAP     = 43444,
    SPELL_HU_FREEZING_TRAP      = 43447,
    SPELL_HU_SNAKE_TRAP         = 43449,

    // Mage
    SPELL_MG_FROST_NOVA         = 43426,
    SPELL_MG_ICE_LANCE          = 43427,
    SPELL_MG_FROSTBOLT          = 43428,

    // Paladin
    SPELL_PA_CONSECRATION       = 43429,
    SPELL_PA_AVENGING_WRATH     = 43430,
    SPELL_PA_HOLY_LIGHT         = 43451,

    // Priest
    SPELL_PR_HEAL               = 41372,
    // SPELL_PR_MIND_BLAST         = 41374, - unconfirmed
    // SPELL_PR_SW_DEATH           = 41375, - unconfirmed
    SPELL_PR_PSYCHIC_SCREAM     = 43432,
    SPELL_PR_MIND_CONTROL       = 43550,

    // Rogue
    SPELL_RO_WOUND_POISON       = 39665,
    SPELL_RO_BLIND              = 43433,
    SPELL_RO_SLICE_DICE         = 43547,

    // Shaman
    SPELL_SH_CHAIN_LIGHT        = 43435,
    SPELL_SH_FIRE_NOVA          = 43436,
    SPELL_SH_HEALING_WAVE       = 43548,

    // Warlock
    SPELL_WL_CURSE_OF_DOOM      = 43439,
    SPELL_WL_RAIN_OF_FIRE       = 43440,
    SPELL_WL_UNSTABLE_AFFL      = 35183,

    // Warrior
    SPELL_WR_MORTAL_STRIKE      = 43441,
    SPELL_WR_WHIRLWIND          = 43442,
    SPELL_WR_SPELL_REFLECT      = 43443,

    // misc
    TARGET_TYPE_RANDOM          = 0,
    TARGET_TYPE_VICTIM          = 1,
    TARGET_TYPE_SELF            = 2,
    TARGET_TYPE_FRIENDLY        = 3,
    TARGET_TYPE_RANDOM_NOT_MAIN = 4,

    MAX_ACTIVE_ADDS             = 4
};

// Adds positions
static const float m_aAddPositions[MAX_ACTIVE_ADDS][4] =
{
    {128.4845f, 923.0428f, 33.97255f, 1.58825f},
    {122.6053f, 923.2454f, 33.97256f, 1.570796f},
    {111.6928f, 923.1531f, 33.97258f, 1.570796f},
    {105.403f,  923.3421f, 33.97259f, 1.553343f},
};

// Each position is a random of two spawns
static const uint32 aSpawnEntries[MAX_ACTIVE_ADDS][2] =
{
    {NPC_ALYSON,    NPC_THURG},
    {NPC_SLITHER,   NPC_RADAAN},
    {NPC_GAZAKROTH, NPC_FENSTALKER},
    {NPC_DARKHEART, NPC_KORAGG},
};

struct PlayerAbilityStruct
{
    uint32 m_uiSpellId;
    uint8 m_uiTargetType;
    uint32 m_uiInitialTimer, m_uiCooldown;
};

// Classes are in the same order as they are in DBC
static PlayerAbilityStruct aMalacrassStolenAbility[][4] =
{
    {},
    {
        // 1 warrior
        {SPELL_WR_SPELL_REFLECT,    TARGET_TYPE_SELF,     7000,  15000},
        {SPELL_WR_WHIRLWIND,        TARGET_TYPE_SELF,     7000,  15000},
        {SPELL_WR_MORTAL_STRIKE,    TARGET_TYPE_VICTIM,   6000,  15000}
    },
    {
        // 2 paladin
        {SPELL_PA_CONSECRATION,     TARGET_TYPE_SELF,     13000, 30000},
        {SPELL_PA_HOLY_LIGHT,       TARGET_TYPE_FRIENDLY, 10000, 12000},
        {SPELL_PA_AVENGING_WRATH,   TARGET_TYPE_SELF,     13000, 30000}
    },
    {
        // 3 hunter
        {SPELL_HU_EXPLOSIVE_TRAP,   TARGET_TYPE_SELF,     12000, 30000},
        {SPELL_HU_FREEZING_TRAP,    TARGET_TYPE_SELF,     3000,  30000},
        {SPELL_HU_SNAKE_TRAP,       TARGET_TYPE_SELF,     21000, 30000}
    },
    {
        // 4 rogue
        {SPELL_RO_WOUND_POISON,     TARGET_TYPE_VICTIM,   6000,  15000},
        {SPELL_RO_SLICE_DICE,       TARGET_TYPE_SELF,     0,     30000},
        {SPELL_RO_BLIND,            TARGET_TYPE_RANDOM_NOT_MAIN,   7000, 15000}
    },
    {
        // 5 priest
        {SPELL_PR_MIND_CONTROL,     TARGET_TYPE_SELF,     11000, 12000},
        {SPELL_PR_HEAL,             TARGET_TYPE_FRIENDLY, 10000, 12000},
        {SPELL_PR_PSYCHIC_SCREAM,   TARGET_TYPE_RANDOM,   12000, 12000}
    },
    {
        // 6 death knight
        {SPELL_DK_DEATH_AND_DECAY,  TARGET_TYPE_RANDOM,   25000, 30000},
        {SPELL_DK_PLAGUE_STRIKE,    TARGET_TYPE_VICTIM,   5000,  17000},
        {SPELL_DK_MARK_OF_BLOOD,    TARGET_TYPE_RANDOM,   14000, 30000}
    },
    {
        // 7 shaman
        {SPELL_SH_FIRE_NOVA,        TARGET_TYPE_SELF,     12000, 30000},
        {SPELL_SH_CHAIN_LIGHT,      TARGET_TYPE_RANDOM,   11000, 12000},
        {SPELL_SH_HEALING_WAVE,     TARGET_TYPE_FRIENDLY, 10000, 12000},
    },
    {
        // 8 mage
        {SPELL_MG_FROST_NOVA,       TARGET_TYPE_SELF,     2000,  10000},
        {SPELL_MG_ICE_LANCE,        TARGET_TYPE_RANDOM,   2000,  10000},
        {SPELL_MG_FROSTBOLT,        TARGET_TYPE_RANDOM,   2000,  4000},
    },
    {
        // 9 warlock
        {SPELL_WL_CURSE_OF_DOOM,    TARGET_TYPE_RANDOM,   11000, 11000},
        {SPELL_WL_RAIN_OF_FIRE,     TARGET_TYPE_RANDOM,   6000,  5000},
        {SPELL_WL_UNSTABLE_AFFL,    TARGET_TYPE_RANDOM,   10000, 11000}
    },
    {
        // 10 unused - no class in DBC here
    },
    {
        // 11 druid
        {SPELL_DR_LIFEBLOOM,        TARGET_TYPE_FRIENDLY, 10000, 12000},
        {SPELL_DR_THORNS,           TARGET_TYPE_SELF,     0,     40000},
        {SPELL_DR_MOONFIRE,         TARGET_TYPE_RANDOM,   4000,  4000}
    }
};

enum MalacrassActions
{
    MALACRASS_SPIRIT_BOLTS,
    MALACRASS_SIPHON_SOUL,
    MALACRASS_DRAIN_POWER_ENABLE,
    MALACRASS_DRAIN_POWER,
    MALACRASS_PLAYER_ABILITY_1,
    MALACRASS_PLAYER_ABILITY_2,
    MALACRASS_PLAYER_ABILITY_3,
    MALACRASS_PLAYER_ABILITY_4,
    MALACRASS_ACTION_MAX,
};

struct boss_malacrassAI : public CombatAI
{
    boss_malacrassAI(Creature* creature) : CombatAI(creature, MALACRASS_ACTION_MAX), m_instance(static_cast<instance_zulaman*>(creature->GetInstanceData()))
    {
        AddCombatAction(MALACRASS_SPIRIT_BOLTS, 30000u);
        AddTimerlessCombatAction(MALACRASS_DRAIN_POWER_ENABLE, true);
        AddCombatAction(MALACRASS_DRAIN_POWER, true);
        AddCombatAction(MALACRASS_SIPHON_SOUL, true);
        AddCombatAction(MALACRASS_PLAYER_ABILITY_1, true);
        AddCombatAction(MALACRASS_PLAYER_ABILITY_2, true);
        AddCombatAction(MALACRASS_PLAYER_ABILITY_3, true);
        AddCombatAction(MALACRASS_PLAYER_ABILITY_4, true);
        m_creature->GetCombatManager().SetLeashingCheck([](Unit*, float x, float y, float z)
        {
            return y > 1025.f;
        });
        AddOnKillText(SAY_KILL1, SAY_KILL2);
        Reset();
    }

    instance_zulaman* m_instance;

    uint8 m_playerClass;

    void Reset() override
    {
        CombatAI::Reset();
        m_playerClass         = 0;

        DoInitializeAdds();

        m_creature->ApplySpellImmune(nullptr, IMMUNITY_MECHANIC, MECHANIC_DISARM, true);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MALACRASS, FAIL);
    }

    void CorpseRemoved(uint32& respawnDelay) override
    {
        // Respawn after 30 seconds
        if (m_instance->GetData(TYPE_MALACRASS) == FAIL)
            respawnDelay = 30;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        if (eventType == AI_EVENT_CUSTOM_A) // malacrass kill or wipe
        {
            auto& addsList = m_instance->GetMalacrassAddsEntryList();
            m_creature->ForcedDespawn();
            for (uint8 i = 0; i < MAX_ACTIVE_ADDS; ++i)
                if (Creature* add = m_instance->GetSingleCreatureFromStorage(addsList[i], true))
                    add->ForcedDespawn();
        }
    }

    void DoInitializeAdds()
    {
        // not if m_creature are dead, so avoid
        if (!m_creature->IsAlive())
            return;

        auto& addsList = m_instance->GetMalacrassAddsEntryList();

        // it's empty, so first time
        if (addsList.empty())
        {
            addsList.resize(MAX_ACTIVE_ADDS);

            for (uint8 i = 0; i < MAX_ACTIVE_ADDS; ++i)
            {
                uint8 addVersion = urand(0, 1);
                addsList[i] = aSpawnEntries[i][addVersion];
                Creature* creature = m_creature->SummonCreature(aSpawnEntries[i][addVersion], m_aAddPositions[i][0], m_aAddPositions[i][1], m_aAddPositions[i][2], m_aAddPositions[i][3], TEMPSPAWN_DEAD_DESPAWN, 0);
                creature->SetCorpseDelay(5);
            }
        }
        // Resummon the killed adds
        else
        {
            if (!m_instance)
                return;

            for (uint8 i = 0; i < MAX_ACTIVE_ADDS; ++i)
            {
                // If we already have the creature on the map, then don't summon it
                if (m_instance->GetSingleCreatureFromStorage(addsList[i], true))
                    continue;

                m_creature->SummonCreature(addsList[i], m_aAddPositions[i][0], m_aAddPositions[i][1], m_aAddPositions[i][2], m_aAddPositions[i][3], TEMPSPAWN_DEAD_DESPAWN, 0);
            }
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_MALACRASS, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_MALACRASS, DONE);
    }

    void SummonedCreatureJustDied(Creature* /*summoned*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_ADD_DIED1, m_creature); break;
            case 1: DoScriptText(SAY_ADD_DIED2, m_creature); break;
            case 2: DoScriptText(SAY_ADD_DIED3, m_creature); break;
        }
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spellInfo) override
    {
        // Set the player's class when hit with soul siphon
        if (target->GetTypeId() == TYPEID_PLAYER && spellInfo->Id == SPELL_SIPHON_SOUL)
        {
            m_playerClass = target->getClass();

            if (m_playerClass == CLASS_WARRIOR || m_playerClass == CLASS_ROGUE)
                m_creature->ApplySpellImmune(nullptr, IMMUNITY_MECHANIC, MECHANIC_DISARM, false);

            for (uint8 i = 0; i < 3; ++i)
            {
                switch (aMalacrassStolenAbility[m_playerClass][i].m_uiSpellId)
                {
                    case SPELL_HU_EXPLOSIVE_TRAP:
                    case SPELL_HU_FREEZING_TRAP:
                    case SPELL_HU_SNAKE_TRAP:
                        ResetCombatAction(MALACRASS_PLAYER_ABILITY_1 + i, urand(1000, 25000));
                        break;
                    default:
                        ResetCombatAction(MALACRASS_PLAYER_ABILITY_1 + i, aMalacrassStolenAbility[m_playerClass][i].m_uiInitialTimer);
                        break;
                }
            }
        }
    }

    bool CanUseSpecialAbility(uint32 spellIndex)
    {
        Unit* target = nullptr;
        bool requireTarget = true;

        switch (aMalacrassStolenAbility[m_playerClass][spellIndex].m_uiTargetType)
        {
            case TARGET_TYPE_SELF:
                target = nullptr;
                requireTarget = false;
                break;
            case TARGET_TYPE_VICTIM:
                target = m_creature->GetVictim();
                break;
            case TARGET_TYPE_RANDOM:
                target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER);
                break;
            case TARGET_TYPE_FRIENDLY:
                target = DoSelectLowestHpFriendly(50.0f);
                break;
            case TARGET_TYPE_RANDOM_NOT_MAIN:
                target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_SKIP_TANK);
                break;
        }

        if (!requireTarget || target)
        {
            if (DoCastSpellIfCan(target, aMalacrassStolenAbility[m_playerClass][spellIndex].m_uiSpellId) == CAST_OK)
                return true;
        }

        return false;
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MALACRASS_SPIRIT_BOLTS:
                if (DoCastSpellIfCan(nullptr, SPELL_SPIRIT_BOLTS) == CAST_OK)
                {
                    DoScriptText(SAY_SPIRIT_BOLTS, m_creature);
                    DisableCombatAction(MALACRASS_PLAYER_ABILITY_1);
                    DisableCombatAction(MALACRASS_PLAYER_ABILITY_2);
                    DisableCombatAction(MALACRASS_PLAYER_ABILITY_3);
                    DisableCombatAction(MALACRASS_PLAYER_ABILITY_4);
                    DisableCombatAction(action);
                    ResetCombatAction(MALACRASS_SIPHON_SOUL, 0);
                    if (m_playerClass == CLASS_WARRIOR || m_playerClass == CLASS_ROGUE)
                        m_creature->ApplySpellImmune(nullptr, IMMUNITY_MECHANIC, MECHANIC_DISARM, true);
                }
                break;
            case MALACRASS_DRAIN_POWER_ENABLE:
                if (m_creature->GetHealthPercent() < 80.0f && DoCastSpellIfCan(nullptr, SPELL_DRAIN_POWER) == CAST_OK)
                {
                    SetActionReadyStatus(action, false);
                    DoScriptText(SAY_DRAIN_POWER, m_creature);
                    ResetCombatAction(MALACRASS_DRAIN_POWER, 30000);
                }
                break;
            case MALACRASS_DRAIN_POWER:
                if (DoCastSpellIfCan(nullptr, SPELL_DRAIN_POWER) == CAST_OK)
                {
                    DoScriptText(SAY_DRAIN_POWER, m_creature);
                    ResetCombatAction(action, 30000);
                }
                break;
            case MALACRASS_SIPHON_SOUL:
                if (DoCastSpellIfCan(nullptr, SPELL_SIPHON_SOUL_DUMMY) == CAST_OK)
                {
                    DoScriptText(SAY_SOUL_SIPHON, m_creature);
                    DisableCombatAction(action);
                    ResetCombatAction(MALACRASS_SPIRIT_BOLTS, 30000);
                }
                break;
            case MALACRASS_PLAYER_ABILITY_1:
            case MALACRASS_PLAYER_ABILITY_2:
            case MALACRASS_PLAYER_ABILITY_3:
            case MALACRASS_PLAYER_ABILITY_4:
                if (CanUseSpecialAbility(action - MALACRASS_PLAYER_ABILITY_1))
                    ResetCombatAction(action, aMalacrassStolenAbility[m_playerClass][action - MALACRASS_PLAYER_ABILITY_1].m_uiCooldown);
                break;
        }
    }
};

void AddSC_boss_malacrass()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_malacrass";
    pNewScript->GetAI = &GetNewAIInstance<boss_malacrassAI>;
    pNewScript->RegisterSelf();
}

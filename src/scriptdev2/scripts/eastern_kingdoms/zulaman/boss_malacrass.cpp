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

#include "precompiled.h"
#include "zulaman.h"

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
    SPELL_MG_FIREBALL           = 41383,
    SPELL_MG_FROST_NOVA         = 43426,
    SPELL_MG_ICE_LANCE          = 43427,
    SPELL_MG_FROSTBOLT          = 43428,

    // Paladin
    SPELL_PA_CONSECRATION       = 43429,
    SPELL_PA_AVENGING_WRATH     = 43430,
    SPELL_PA_HOLY_LIGHT         = 43451,

    // Priest
    SPELL_PR_HEAL               = 41372,
    SPELL_PR_MIND_BLAST         = 41374,
    SPELL_PR_SW_DEATH           = 41375,
    SPELL_PR_PSYCHIC_SCREAM     = 43432,
    SPELL_PR_MIND_CONTROL       = 43550,
    SPELL_PR_PAIN_SUPP          = 44416,

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

    MAX_ACTIVE_ADDS             = 4
};

// Adds positions
static const float m_aAddPositions[MAX_ACTIVE_ADDS][4] =
{
    {128.279f, 921.279f, 33.889f, 1.527f},
    {123.261f, 921.279f, 33.889f, 1.527f},
    {112.084f, 921.279f, 33.889f, 1.527f},
    {106.473f, 921.279f, 33.889f, 1.527f},
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
static PlayerAbilityStruct m_aMalacrassStolenAbility[][4] =
{
    {
        // 0* shadow priest - exception: it seems that the priest has two specs. We use this slot for the shadow priest
        {SPELL_PR_MIND_CONTROL,     TARGET_TYPE_RANDOM,   15000, 30000},
        {SPELL_PR_MIND_BLAST,       TARGET_TYPE_RANDOM,   23000, 30000},
        {SPELL_PR_SW_DEATH,         TARGET_TYPE_RANDOM,   5000,  16000}
    },
    {
        // 1 warrior
        {SPELL_WR_SPELL_REFLECT,    TARGET_TYPE_SELF,     2000,  30000},
        {SPELL_WR_WHIRLWIND,        TARGET_TYPE_SELF,     10000, 30000},
        {SPELL_WR_MORTAL_STRIKE,    TARGET_TYPE_VICTIM,   6000,  15000}
    },
    {
        // 2 paladin
        {SPELL_PA_CONSECRATION,     TARGET_TYPE_SELF,     10000, 30000},
        {SPELL_PA_HOLY_LIGHT,       TARGET_TYPE_FRIENDLY, 17000, 30000},
        {SPELL_PA_AVENGING_WRATH,   TARGET_TYPE_SELF,     0,     30000}
    },
    {
        // 3 hunter
        {SPELL_HU_EXPLOSIVE_TRAP,   TARGET_TYPE_SELF,     12000, 30000},
        {SPELL_HU_FREEZING_TRAP,    TARGET_TYPE_SELF,     3000,  30000},
        {SPELL_HU_SNAKE_TRAP,       TARGET_TYPE_SELF,     21000, 30000}
    },
    {
        // 4 rogue
        {SPELL_RO_WOUND_POISON,     TARGET_TYPE_VICTIM,   3000,  17000},
        {SPELL_RO_SLICE_DICE,       TARGET_TYPE_SELF,     17000, 30000},
        {SPELL_RO_BLIND,            TARGET_TYPE_RANDOM,   12000, 30000}
    },
    {
        // 5 priest
        {SPELL_PR_PAIN_SUPP,        TARGET_TYPE_FRIENDLY, 24000, 30000},
        {SPELL_PR_HEAL,             TARGET_TYPE_FRIENDLY, 16000, 30000},
        {SPELL_PR_PSYCHIC_SCREAM,   TARGET_TYPE_RANDOM,   8000,  30000}
    },
    {
        // 6 death knight
        {SPELL_DK_DEATH_AND_DECAY,  TARGET_TYPE_RANDOM,   25000, 30000},
        {SPELL_DK_PLAGUE_STRIKE,    TARGET_TYPE_VICTIM,   5000,  17000},
        {SPELL_DK_MARK_OF_BLOOD,    TARGET_TYPE_RANDOM,   14000, 30000}
    },
    {
        // 7 shaman
        {SPELL_SH_FIRE_NOVA,        TARGET_TYPE_SELF,     25000, 30000},
        {SPELL_SH_HEALING_WAVE,     TARGET_TYPE_FRIENDLY, 15000, 30000},
        {SPELL_SH_CHAIN_LIGHT,      TARGET_TYPE_RANDOM,   4000,  16000}
    },
    {
        // 8 mage
        {SPELL_MG_FIREBALL,         TARGET_TYPE_RANDOM,   8000,  30000},
        {SPELL_MG_FROSTBOLT,        TARGET_TYPE_RANDOM,   25000, 30000},
        {SPELL_MG_ICE_LANCE,        TARGET_TYPE_RANDOM,   2000,  18000},
        {SPELL_MG_FROST_NOVA,       TARGET_TYPE_SELF,     17000, 30000}
    },
    {
        // 9 warlock
        {SPELL_WL_CURSE_OF_DOOM,    TARGET_TYPE_RANDOM,   0,     30000},
        {SPELL_WL_RAIN_OF_FIRE,     TARGET_TYPE_RANDOM,   16000, 30000},
        {SPELL_WL_UNSTABLE_AFFL,    TARGET_TYPE_RANDOM,   8000,  13000}
    },
    {
        // 10 unused - no class in DBC here
    },
    {
        // 11 druid
        {SPELL_DR_LIFEBLOOM,        TARGET_TYPE_FRIENDLY, 15000, 30000},
        {SPELL_DR_THORNS,           TARGET_TYPE_SELF,     0,     30000},
        {SPELL_DR_MOONFIRE,         TARGET_TYPE_RANDOM,   8000,  13000}
    }
};

struct boss_malacrassAI : public ScriptedAI
{
    boss_malacrassAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiSpiritBoltsTimer;
    uint32 m_uiDrainPowerTimer;
    uint32 m_uiSiphonSoulTimer;
    uint32 m_uiPlayerAbilityTimer;
    uint8 m_uiPlayerClass;

    bool m_bCanUsePlayerSpell;

    std::vector<uint32> m_vAddsEntryList;
    std::vector<uint32> m_vPlayerSpellTimer;

    void Reset() override
    {
        m_uiSpiritBoltsTimer    = 30000;
        m_uiDrainPowerTimer     = 0;
        m_uiSiphonSoulTimer     = 40000;
        m_uiPlayerAbilityTimer  = 10000;
        m_uiPlayerClass         = 0;

        m_bCanUsePlayerSpell    = false;

        DoInitializeAdds();
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALACRASS, FAIL);
    }

    void DoInitializeAdds()
    {
        // not if m_creature are dead, so avoid
        if (!m_creature->isAlive())
            return;

        // it's empty, so first time
        if (m_vAddsEntryList.empty())
        {
            m_vAddsEntryList.resize(MAX_ACTIVE_ADDS);

            for (uint8 i = 0; i < MAX_ACTIVE_ADDS; ++i)
            {
                uint8 uiAddVersion = urand(0, 1);
                m_vAddsEntryList[i] = aSpawnEntries[i][uiAddVersion];
                m_creature->SummonCreature(aSpawnEntries[i][uiAddVersion], m_aAddPositions[i][0], m_aAddPositions[i][1], m_aAddPositions[i][2], m_aAddPositions[i][3], TEMPSUMMON_CORPSE_DESPAWN, 0);
            }
        }
        // Resummon the killed adds
        else
        {
            if (!m_pInstance)
                return;

            for (uint8 i = 0; i < MAX_ACTIVE_ADDS; ++i)
            {
                // If we already have the creature on the map, then don't summon it
                if (m_pInstance->GetSingleCreatureFromStorage(m_vAddsEntryList[i], true))
                    continue;

                m_creature->SummonCreature(m_vAddsEntryList[i], m_aAddPositions[i][0], m_aAddPositions[i][1], m_aAddPositions[i][2], m_aAddPositions[i][3], TEMPSUMMON_CORPSE_DESPAWN, 0);
            }
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALACRASS, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(urand(0, 1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALACRASS, DONE);
    }

    void SummonedCreatureJustDied(Creature* /*pSummoned*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_ADD_DIED1, m_creature); break;
            case 1: DoScriptText(SAY_ADD_DIED2, m_creature); break;
            case 2: DoScriptText(SAY_ADD_DIED3, m_creature); break;
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        // Set the player's class when hit with soul siphon
        if (pTarget->GetTypeId() == TYPEID_PLAYER && pSpell->Id == SPELL_SIPHON_SOUL)
        {
            m_uiPlayerClass = ((Player*)pTarget)->getClass();
            m_bCanUsePlayerSpell = true;

            // In case the player it's priest we can choose either a holy priest or a shadow priest
            if (m_uiPlayerClass == CLASS_PRIEST)
                m_uiPlayerClass = urand(0, 1) ? CLASS_PRIEST : 0;

            // Init the spell timers
            uint8 m_uiMaxSpells = m_uiPlayerClass == CLASS_MAGE ? 4 : 3;

            m_vPlayerSpellTimer.clear();
            m_vPlayerSpellTimer.reserve(m_uiMaxSpells);
            for (uint8 i = 0; i < m_uiMaxSpells; ++i)
                m_vPlayerSpellTimer.push_back(m_aMalacrassStolenAbility[m_uiPlayerClass][i].m_uiInitialTimer);
        }
    }

    bool CanUseSpecialAbility(uint32 uiSpellIndex)
    {
        Unit* pTarget = NULL;

        switch (m_aMalacrassStolenAbility[m_uiPlayerClass][uiSpellIndex].m_uiTargetType)
        {
            case TARGET_TYPE_SELF:
                pTarget = m_creature;
                break;
            case TARGET_TYPE_VICTIM:
                pTarget = m_creature->getVictim();
                break;
            case TARGET_TYPE_RANDOM:
                pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                break;
            case TARGET_TYPE_FRIENDLY:
                pTarget = DoSelectLowestHpFriendly(50.0f);
                break;
        }

        if (pTarget)
        {
            if (DoCastSpellIfCan(pTarget, m_aMalacrassStolenAbility[m_uiPlayerClass][uiSpellIndex].m_uiSpellId, CAST_TRIGGERED) == CAST_OK)
                return true;
        }

        return false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Acts as an enrage timer
        if (m_creature->GetHealthPercent() < 80.0f)
        {
            if (m_uiDrainPowerTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DRAIN_POWER) == CAST_OK)
                {
                    DoScriptText(SAY_DRAIN_POWER, m_creature);
                    m_uiDrainPowerTimer = 30000;
                }
            }
            else
                m_uiDrainPowerTimer -= uiDiff;
        }

        if (m_uiSpiritBoltsTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SPIRIT_BOLTS) == CAST_OK)
            {
                DoScriptText(SAY_SPIRIT_BOLTS, m_creature);
                m_bCanUsePlayerSpell = false;
                m_uiSpiritBoltsTimer = 40000;
            }
        }
        else
            m_uiSpiritBoltsTimer -= uiDiff;

        if (m_uiSiphonSoulTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SIPHON_SOUL_DUMMY) == CAST_OK)
            {
                DoScriptText(SAY_SOUL_SIPHON, m_creature);
                m_uiSiphonSoulTimer = 40000;
            }
        }
        else
            m_uiSiphonSoulTimer -= uiDiff;

        // Use abilities only during the siphon soul phases
        if (m_bCanUsePlayerSpell)
        {
            // Loop through all abilities
            for (uint8 i = 0; i < m_vPlayerSpellTimer.size(); ++i)
            {
                if (m_vPlayerSpellTimer[i] < uiDiff)
                {
                    if (CanUseSpecialAbility(i))
                        m_vPlayerSpellTimer[i] = m_aMalacrassStolenAbility[m_uiPlayerClass][i].m_uiCooldown;
                }
                else
                    m_vPlayerSpellTimer[i] -= uiDiff;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_malacrass(Creature* pCreature)
{
    return new boss_malacrassAI(pCreature);
}

void AddSC_boss_malacrass()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_malacrass";
    pNewScript->GetAI = &GetAI_boss_malacrass;
    pNewScript->RegisterSelf();
}

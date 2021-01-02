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
SDName: faction_champions
SD%Complete: 90
SDComment: AI might require some tweaks and improvements
SDCategory: Crusader Coliseum
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "trial_of_the_crusader.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_GARROSH_PVP_A_SLAY_1            = -1649048,
    SAY_GARROSH_PVP_A_SLAY_2            = -1649049,
    SAY_GARROSH_PVP_A_SLAY_3            = -1649050,
    SAY_GARROSH_PVP_A_SLAY_4            = -1649051,

    SAY_VARIAN_PVP_H_SLAY_1             = -1649052,
    SAY_VARIAN_PVP_H_SLAY_2             = -1649053,
    SAY_VARIAN_PVP_H_SLAY_3             = -1649054,
    SAY_VARIAN_PVP_H_SLAY_4             = -1649055,

    SPELL_PVP_TRINKET                   = 65547,

    // AI type defines
    AI_TYPE_MELEE                       = 1,
    AI_TYPE_RANGED                      = 2,
    AI_TYPE_HEALER                      = 3,

    // misc
    TARGET_TYPE_RANDOM                  = 0,
    TARGET_TYPE_VICTIM                  = 1,
    TARGET_TYPE_SELF                    = 2,
    TARGET_TYPE_FRIENDLY                = 3,

    CRUSADER_AIEVENT_THROW_RADIUS       = 30,
    CRUSADER_HEALTH_STEPS               = 3,


    // druid restoration spells
    SPELL_THORNS                    = 66068,                // aggro spell
    SPELL_BARKSKIN                  = 65860,
    SPELL_NATURES_GRASP             = 66071,
    SPELL_TRANQUILITY               = 66086,
    SPELL_LIFEBLOOM                 = 66093,
    SPELL_NOURISH                   = 66066,
    SPELL_REGROWTH                  = 66067,
    SPELL_REJUVENATION              = 66065,

    MAX_DRUID_RESTO_SPELLS          = 7,


    // paladin holy spells
    SPELL_CLEANSE                   = 66116,                // event spell
    SPELL_DIVINE_SHIELD             = 66010,
    SPELL_FLASH_OF_LIGHT            = 66113,
    SPELL_FLASH_HEAL                = 66104,
    SPELL_HOLY_LIGHT                = 66112,
    SPELL_HAND_OF_PROTECTION        = 66009,
    SPELL_HAND_OF_FREEDOM           = 68757,
    SPELL_HOLY_SHOCK                = 66114,
    SPELL_HAMMER_OF_JUSTICE         = 66613,

    MAX_PALADIN_HOLY_SPELLS         = 8,


    // priest discipline spells
    SPELL_DISPEL_MAGIC              = 65546,
    // SPELL_FLASH_HEAL             = 66104,                // already defined
    SPELL_PENANCE                   = 66097,                // triggers 66098
    SPELL_POWER_WORD_SHIELD         = 66099,
    SPELL_RENEW                     = 66177,
    SPELL_MANA_BURN                 = 66100,
    SPELL_PSYCHIC_SCREAM            = 65543,

    MAX_PRIEST_DISC_SPELLS          = 7,


    // shaman restoration spells
    SPELL_CLEANSE_SPIRIT            = 66056,
    SPELL_EARTH_SHIELD              = 66063,                // triggers 66064
    SPELL_HEX                       = 66054,
    SPELL_LESSER_HEALING_WAVE       = 66055,
    SPELL_RIPTIDE                   = 66053,
    SPELL_HEROISM                   = 65983,
    SPELL_BLOODLUST                 = 65980,                // replace heroism for horde crusaders
    SPELL_EARTH_SHOCK               = 65973,

    MAX_SHAMAN_RESTO_SPELLS         = 7,


    // druid balance spells
    // SPELL_BARKSKIN               = 65860,                // already defined
    SPELL_CYCLONE                   = 65859,
    SPELL_ENTANGLING_ROOTS          = 65857,
    SPELL_FAERIE_FIRE               = 65863,
    SPELL_FORCE_OF_NATURE           = 65861,
    SPELL_INSECT_SWARM              = 65855,
    SPELL_MOONFIRE                  = 65856,
    SPELL_STARFIRE                  = 65854,
    SPELL_WRATH                     = 65862,

    MAX_DRUID_BALANCE_SPELLS        = 9,


    // hunter spells
    SPELL_CALL_PET                  = 67777,                // aggro spell
    SPELL_AIMED_SHOT                = 65883,
    SPELL_DETERRENCE                = 65871,
    SPELL_DISENGAGE                 = 65869,                // triggers 65870
    SPELL_EXPLOSIVE_SHOT            = 65866,
    SPELL_FROST_TRAP                = 65880,
    SPELL_SHOOT                     = 65868,
    SPELL_STEADY_SHOT               = 65867,
    SPELL_WING_CLIP                 = 66207,
    SPELL_WYVERN_STING              = 65877,                // triggers 65878 on remove

    MAX_HUNTER_SPELLS               = 9,


    // mage spells
    SPELL_ARCANE_BARRAGE            = 65799,
    SPELL_ARCANE_BLAST              = 65791,
    SPELL_ARCANE_EXPLOSION          = 65800,
    SPELL_BLINK                     = 65793,
    SPELL_COUNTERSPELL              = 65790,
    SPELL_FROST_NOVA                = 65792,
    SPELL_FROSTBOLT                 = 65807,
    SPELL_ICE_BLOCK                 = 65802,
    SPELL_POLYMORPH                 = 65801,

    MAX_MAGE_SPELLS                 = 9,


    // priest shadow spells
    SPELL_DISPERSION                = 65544,
    SPELL_MIND_BLAST                = 65492,
    SPELL_MIND_FLAY                 = 65488,
    SPELL_PSYCHIC_HORROR            = 65545,
    // SPELL_PSYCHIC_SCREAM         = 65543,                // already defined
    SPELL_SHADOW_WORD_PAIN          = 65541,
    SPELL_SILENCE                   = 65542,
    SPELL_VAMPIRIC_TOUCH            = 65490,
    SPELL_DISPEL                    = 65546,                // event spell

    MAX_PRIEST_DAMAGE_SPELLS        = 8,


    // warlock spells
    SPELL_SUMMON_FELHUNTER          = 67514,                // aggro spell
    SPELL_CORRUPTION                = 65810,
    SPELL_CURSE_OF_AGONY            = 65814,
    SPELL_CURSE_OF_EXHAUSTION       = 65815,
    SPELL_DEATH_COIL_WARLOCK        = 65820,
    SPELL_FEAR                      = 65809,
    SPELL_HELLFIRE                  = 65816,
    SPELL_SEARING_PAIN              = 65819,
    SPELL_SHADOW_BOLT               = 65821,
    SPELL_UNSTABLE_AFFLICTION       = 65812,

    MAX_WARLOCK_SPELLS              = 9,


    // death knight spells
    SPELL_CHAINS_OF_ICE             = 66020,
    SPELL_DEATH_COIL                = 66019,
    SPELL_DEATH_GRIP                = 66017,
    SPELL_FROST_STRIKE              = 66047,
    SPELL_ICEBOUND_FORTITUDE        = 66023,
    SPELL_ICY_TOUCH                 = 66021,                // triggers 67767
    SPELL_STRANGULATE               = 66018,

    MAX_DEATH_KNIGHT_SPELLS         = 7,


    // warrior spells
    SPELL_BLADESTORM                = 65947,
    SPELL_CHARGE                    = 65927,
    SPELL_DISARM                    = 65935,
    SPELL_INTIMIDATING_SHOUT        = 65931,
    SPELL_MORTAL_STRIKE             = 65926,
    SPELL_OVERPOWER                 = 65924,
    SPELL_RETALIATION               = 65932,
    SPELL_SHATTERING_THROW          = 65940,
    SPELL_SUNDER_ARMOR              = 65936,

    MAX_WARRIOR_SPELLS              = 9,


    // paladin retribution spells
    SPELL_SEAL_OF_COMMAND           = 66004,                // aggro spell
    SPELL_AVENGING_WRATH            = 66011,
    SPELL_CRUSADER_STRIKE           = 66003,
    SPELL_DIVINE_STORM              = 66006,
    SPELL_HAMMER_OF_JUSTICE_RETRI   = 66007,
    SPELL_JUDGEMENT_OF_COMMAND      = 66005,
    SPELL_REPENTANCE                = 66008,

    MAX_PALADIN_DAMAGE_SPELLS       = 6,


    // shaman enhancement spells
    //SPELL_EARTH_SHOCK             = 65973,                // already defined
    //SPELL_HEROISM                 = 65983,                // already defined
    SPELL_LAVA_LASH                 = 65974,
    SPELL_MAELSTROM_WEAPON          = 65986,
    SPELL_STORMSTRIKE               = 65970,
    SPELL_WINDFURY                  = 65976,

    MAX_SHAMAN_DAMAGE_SPELLS        = 6,


    // rogue spells
    SPELL_BLADE_FURRY               = 65956,
    SPELL_BLIND                     = 65960,
    SPELL_CLOAK_OF_SHADOWS          = 65961,
    SPELL_EVISCERATE                = 65957,
    SPELL_FAN_OF_KNIVES             = 65955,
    SPELL_HEMORRHAGE                = 65954,
    SPELL_SHADOWSTEP                = 66178,
    SPELL_WOUND_POISON              = 65962,

    MAX_ROGUE_SPELLS                = 8,
};

struct CrusaderAbilityStruct
{
    uint32 m_uiSpellId, m_uiTargetType;
    uint32 m_uiInitialTimer, m_uiCooldown, m_uiMinHealth;
    SelectFlags m_selectFlag;
};

/*######
## trial_crusader_common
######*/

struct trial_crusader_commonAI : public ScriptedAI
{
    trial_crusader_commonAI(Creature* pCreature, CrusaderAbilityStruct const* pAbilityArray, uint32 uiMaxAbilities) : ScriptedAI(pCreature),
        m_pAbilityArray(pAbilityArray),
        m_uiMaxAbilities(uiMaxAbilities)
    {
        m_pInstance = static_cast<instance_trial_of_the_crusader*>(pCreature->GetInstanceData());
        m_uiSpellTimer.resize(m_uiMaxAbilities);
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;
    CrusaderAbilityStruct const* m_pAbilityArray;

    uint32 m_uiResetThreatTimer;
    uint32 m_uiIsCCTimer;
    uint32 m_uiTrinketTimer;
    uint32 m_uiTrinketCooldownTimer;

    uint32 m_uiThrowAIEventStep;

    uint8 m_uiAIType;

    uint32 m_uiAbilityTimer;
    uint32 m_uiMaxAbilities;
    std::vector<uint32> m_uiSpellTimer;

    void Reset() override
    {
        // NOTE:
        // These guys does not follow normal threat system rules
        // For later development, some alternative threat system should be made
        // We do not know what this system is based upon, but one theory is class (healers=high threat, dps=medium, etc)
        // We reset their threat frequently as an alternative until such a system exist
        m_uiResetThreatTimer        = urand(5000, 15000);
        m_uiIsCCTimer               = 2000;
        m_uiTrinketCooldownTimer    = 0;

        m_uiThrowAIEventStep        = 0;

        m_uiAbilityTimer            = 2000;

        for (uint8 i = 0; i < m_uiMaxAbilities; ++i)
            m_uiSpellTimer[i] = m_pAbilityArray[i].m_uiInitialTimer;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER))
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_FACTION_CHAMPIONS) != IN_PROGRESS)
            {
                m_pInstance->SetData(TYPE_FACTION_CHAMPIONS, IN_PROGRESS);
                m_pInstance->DoSetCrusadersInCombat(pWho);
            }
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (!m_pInstance)
            return;

        Creature* pSpeaker = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetPlayerTeam() == ALLIANCE ? NPC_GARROSH : NPC_VARIAN);
        if (!pSpeaker)
            return;

        switch (urand(0, 3))
        {
            case 0: DoScriptText(m_pInstance->GetPlayerTeam() == ALLIANCE ? SAY_GARROSH_PVP_A_SLAY_1 : SAY_VARIAN_PVP_H_SLAY_1, pSpeaker); break;
            case 1: DoScriptText(m_pInstance->GetPlayerTeam() == ALLIANCE ? SAY_GARROSH_PVP_A_SLAY_2 : SAY_VARIAN_PVP_H_SLAY_2, pSpeaker); break;
            case 2: DoScriptText(m_pInstance->GetPlayerTeam() == ALLIANCE ? SAY_GARROSH_PVP_A_SLAY_3 : SAY_VARIAN_PVP_H_SLAY_3, pSpeaker); break;
            case 3: DoScriptText(m_pInstance->GetPlayerTeam() == ALLIANCE ? SAY_GARROSH_PVP_A_SLAY_4 : SAY_VARIAN_PVP_H_SLAY_4, pSpeaker); break;
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_FACTION_CHAMPIONS) != FAIL)
                m_pInstance->SetData(TYPE_FACTION_CHAMPIONS, FAIL);
        }
    }

    void DamageTaken(Unit* pDealer, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        uint32 uiStep = m_uiThrowAIEventStep != 100 ? m_uiThrowAIEventStep : 0;
        if (uiStep < CRUSADER_HEALTH_STEPS)
        {
            // Throw at 90%, 50% and 10% health
            float fHealthSteps[CRUSADER_HEALTH_STEPS] = { 90.0f, 50.0f, 10.0f };
            float fNewHealthPercent = (m_creature->GetHealth() - uiDamage) * 100.0f / m_creature->GetMaxHealth();
            AIEventType sendEvent[CRUSADER_HEALTH_STEPS] = { AI_EVENT_LOST_SOME_HEALTH, AI_EVENT_LOST_HEALTH, AI_EVENT_CRITICAL_HEALTH };

            if (fNewHealthPercent > fHealthSteps[uiStep])
                return;                                         // Not reached the next mark

            // search for highest reached mark (with actual event attached)
            for (uint32 i = CRUSADER_HEALTH_STEPS - 1; i > uiStep; --i)
            {
                if (fNewHealthPercent < fHealthSteps[i])
                {
                    uiStep = i;
                    break;
                }
            }

            // send event around and to self
            SendAIEventAround(sendEvent[uiStep], pDealer, 0, CRUSADER_AIEVENT_THROW_RADIUS);
            SendAIEvent(sendEvent[uiStep], pDealer, m_creature);
            m_uiThrowAIEventStep = uiStep + 1;
        }
    }

    void HealedBy(Unit* pHealer, uint32& uiHealedAmount) override
    {
        if (m_uiThrowAIEventStep == 100)
            return;

        if (m_creature->GetHealth() + uiHealedAmount >= m_creature->GetMaxHealth())
        {
            SendAIEventAround(AI_EVENT_GOT_FULL_HEALTH, pHealer, 0, CRUSADER_AIEVENT_THROW_RADIUS);
            m_uiThrowAIEventStep = 100;
        }
    }

    void JustDied(Unit* pKiller) override
    {
        SendAIEventAround(AI_EVENT_JUST_DIED, pKiller, 0, CRUSADER_AIEVENT_THROW_RADIUS);
    }

    bool CanUseSpecialAbility(uint32 uiSpellId, uint32 uiTargetType, SelectFlags selectFlag, uint32 uiMaxHpPct)
    {
        Unit* pTarget = nullptr;

        switch (uiTargetType)
        {
            case TARGET_TYPE_SELF:
                pTarget = m_creature;
                break;
            case TARGET_TYPE_VICTIM:
                pTarget = m_creature->GetVictim();
                break;
            case TARGET_TYPE_RANDOM:
                pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(uiSpellId), selectFlag);
                break;
            case TARGET_TYPE_FRIENDLY:
                // calculate HP deficit compared to caster health; this might not give very accurate results
                pTarget = DoSelectLowestHpFriendly(40.0f, uint32(m_creature->GetMaxHealth() * (100 - uiMaxHpPct) * 0.01f));
                break;
        }

        if (pTarget)
        {
            if (DoCastSpellIfCan(pTarget, uiSpellId) == CAST_OK)
                return true;
        }

        return false;
    }

    // Return true to handle shared timers and MeleeAttack
    virtual bool UpdateCrusaderAI(const uint32 /*uiDiff*/) { return true; }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Call specific virtual function
        if (!UpdateCrusaderAI(uiDiff))
            return;

        if (m_uiAbilityTimer < uiDiff)
        {
            uint8 uiIndex = urand(0, m_uiMaxAbilities - 1);
            uint32 uiMinHealth = m_pAbilityArray[uiIndex].m_uiMinHealth;
            uint8 uiTargetType = m_pAbilityArray[uiIndex].m_uiTargetType;

            SelectFlags spellSelectFlag = m_pAbilityArray[uiIndex].m_selectFlag;

            // check timers and health condition
            // only cast spells that have timers expired
            // also check for health percentage for self cast spells
            if (m_uiSpellTimer[uiIndex] || (uiTargetType == TARGET_TYPE_SELF && uiMinHealth && m_creature->GetHealthPercent() > uiMinHealth))
            {
                m_uiAbilityTimer = 2000;
                return;
            }
            uint32 uiSpellId = m_pAbilityArray[uiIndex].m_uiSpellId;

            // special case for heroism / bloodlust
            if (uiSpellId == SPELL_HEROISM && m_pInstance && m_pInstance->GetPlayerTeam() == ALLIANCE)
                uiSpellId = SPELL_BLOODLUST;

            if (CanUseSpecialAbility(uiSpellId, uiTargetType, spellSelectFlag, uiMinHealth))
            {
                m_uiSpellTimer[uiIndex] = m_pAbilityArray[uiIndex].m_uiCooldown;
                m_uiAbilityTimer = urand(2000, 6000);
            }
            else
                m_uiAbilityTimer = 2000;
        }
        else
            m_uiAbilityTimer -= uiDiff;

        // spell cooldown
        for (uint8 i = 0; i < m_uiMaxAbilities; ++i)
        {
            if (m_uiSpellTimer[i])
            {
                if (m_uiSpellTimer[i] <= uiDiff)
                    m_uiSpellTimer[i] = 0;
                else
                    m_uiSpellTimer[i] -= uiDiff;
            }
        }

        // Change target
        if (m_uiResetThreatTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                DoResetThreat();
                AttackStart(pTarget);
                m_uiResetThreatTimer = urand(5000, 15000);
            }
        }
        else
            m_uiResetThreatTimer -= uiDiff;

        // CC check for PVP trinket
        if (m_uiIsCCTimer < uiDiff)
        {
            if (m_creature->isFrozen() || m_creature->IsCrowdControlled())
            {
                // Pvp trinket only in heroic mode
                if (m_pInstance && m_pInstance->IsHeroicDifficulty() && !m_uiTrinketCooldownTimer)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_PVP_TRINKET, CAST_TRIGGERED) == CAST_OK)
                        m_uiTrinketCooldownTimer = 120000;
                }

                SendAIEventAround(AI_EVENT_GOT_CCED, nullptr, 0, CRUSADER_AIEVENT_THROW_RADIUS);
                SendAIEvent(AI_EVENT_GOT_CCED, nullptr, m_creature);
                m_uiIsCCTimer = 5000;
            }
            else
                m_uiIsCCTimer = 2000;
        }
        else
            m_uiIsCCTimer -= uiDiff;

        // trinket cooldown
        if (m_uiTrinketCooldownTimer)
        {
            if (m_uiTrinketCooldownTimer <= uiDiff)
                m_uiTrinketCooldownTimer = 0;
            else
                m_uiTrinketCooldownTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

/*######
## CRUSADER HEALERS
######*/

/*######
## boss_crusader_druid_resto
######*/

static const CrusaderAbilityStruct m_aDruidHealerAbilities[MAX_DRUID_RESTO_SPELLS] =
{
    {SPELL_BARKSKIN,        TARGET_TYPE_SELF,       5000,  60000,  50},
    {SPELL_NATURES_GRASP,   TARGET_TYPE_SELF,       2000,  60000,  0},
    {SPELL_TRANQUILITY,     TARGET_TYPE_SELF,       2000,  600000, 30},
    {SPELL_LIFEBLOOM,       TARGET_TYPE_FRIENDLY,   2000,  2000,   0},
    {SPELL_NOURISH,         TARGET_TYPE_FRIENDLY,   1500,  1500,   0},
    {SPELL_REGROWTH,        TARGET_TYPE_FRIENDLY,   2000,  2000,   0},
    {SPELL_REJUVENATION,    TARGET_TYPE_FRIENDLY,   2000,  2000,   0},
};

struct boss_crusader_druid_restoAI : public trial_crusader_commonAI
{
    boss_crusader_druid_restoAI(Creature* pCreature) : trial_crusader_commonAI(pCreature, m_aDruidHealerAbilities, MAX_DRUID_RESTO_SPELLS) { Reset(); }

    void Reset() override
    {
        m_uiAIType              = AI_TYPE_HEALER;
        m_attackDistance = 15.0f;

        trial_crusader_commonAI::Reset();
    }

    void Aggro(Unit* pWho) override
    {
        DoCastSpellIfCan(m_creature, SPELL_THORNS);

        trial_crusader_commonAI::Aggro(pWho);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        uint8 uiIndex = 99;
        switch (eventType)
        {
            case AI_EVENT_CRITICAL_HEALTH:
                uiIndex = urand(0, 1) ? 5 : 6;
                break;
            case AI_EVENT_LOST_HEALTH:
                uiIndex = urand(0, 1) ? 4 : 5;
                break;
            case AI_EVENT_LOST_SOME_HEALTH:
                uiIndex = urand(0, 1) ? 3 : 4;
                break;
            default:
                break;
        }

        if (uiIndex > MAX_DRUID_RESTO_SPELLS - 1)
            return;

        if (!m_uiSpellTimer[uiIndex])
        {
            if (DoCastSpellIfCan(pSender, m_aDruidHealerAbilities[uiIndex].m_uiSpellId) == CAST_OK)
                m_uiSpellTimer[uiIndex] = m_aDruidHealerAbilities[uiIndex].m_uiCooldown;
        }
    }
};

/*######
## boss_crusader_paladin_holy
######*/

static const CrusaderAbilityStruct m_aPaladinHealerAbilities[MAX_PALADIN_HOLY_SPELLS] =
{
    {SPELL_DIVINE_SHIELD,       TARGET_TYPE_SELF,       0,     300000, 25},
    {SPELL_FLASH_OF_LIGHT,      TARGET_TYPE_FRIENDLY,   2000,  2000,   0},
    {SPELL_HOLY_SHOCK,          TARGET_TYPE_FRIENDLY,   2000,  6000,   0},
    {SPELL_FLASH_HEAL,          TARGET_TYPE_FRIENDLY,   2000,  2000,   0},
    {SPELL_HOLY_LIGHT,          TARGET_TYPE_FRIENDLY,   1500,  1500,   0},
    {SPELL_HAND_OF_FREEDOM,     TARGET_TYPE_FRIENDLY,   2000,  25000,  30},
    {SPELL_HAND_OF_PROTECTION,  TARGET_TYPE_FRIENDLY,   0,     300000, 10},
    {SPELL_HAMMER_OF_JUSTICE,   TARGET_TYPE_RANDOM,     15000, 40000,  0},
};

struct boss_crusader_paladin_holyAI : public trial_crusader_commonAI
{
    boss_crusader_paladin_holyAI(Creature* pCreature) : trial_crusader_commonAI(pCreature, m_aPaladinHealerAbilities, MAX_PALADIN_HOLY_SPELLS) { Reset(); }

    void Reset() override
    {
        m_uiAIType              = AI_TYPE_HEALER;
        m_attackDistance = 15.0f;

        trial_crusader_commonAI::Reset();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        uint8 uiIndex = 99;
        uint32 uiSpellEntry = 0;
        uint32 uiSpellTimer = 0;

        switch (eventType)
        {
            case AI_EVENT_CRITICAL_HEALTH:
                uiIndex = urand(0, 1) ? 3 : 4;
                break;
            case AI_EVENT_LOST_HEALTH:
                uiIndex = urand(0, 1) ? 2 : 3;
                break;
            case AI_EVENT_LOST_SOME_HEALTH:
                uiIndex = urand(0, 1) ? 1 : 2;
                break;
            case AI_EVENT_GOT_CCED:
                uiSpellEntry = SPELL_CLEANSE;
                break;
            default:
                break;
        }

        if (!uiSpellEntry)
        {
            if (uiIndex > MAX_PALADIN_HOLY_SPELLS - 1)
                return;
            uiSpellEntry = m_aPaladinHealerAbilities[uiIndex].m_uiSpellId;
            uiSpellTimer = m_aPaladinHealerAbilities[uiIndex].m_uiCooldown;
        }

        if (!m_uiSpellTimer[uiIndex])
        {
            if (DoCastSpellIfCan(pSender, uiSpellEntry) == CAST_OK)
            {
                if (uiSpellTimer)
                    m_uiSpellTimer[uiIndex] = uiSpellTimer;
            }
        }
    }
};

/*######
## boss_crusader_priest_disc
######*/

static const CrusaderAbilityStruct m_aPriestHealerAbilities[MAX_PRIEST_DISC_SPELLS] =
{
    {SPELL_PSYCHIC_SCREAM,      TARGET_TYPE_SELF,       10000, 30000,  0},
    {SPELL_PENANCE,             TARGET_TYPE_SELF,       2000,  10000,  0},
    {SPELL_RENEW,               TARGET_TYPE_FRIENDLY,   1500,  1500,   0},
    {SPELL_FLASH_HEAL,          TARGET_TYPE_FRIENDLY,   1500,  1500,   0},
    {SPELL_DISPEL_MAGIC,        TARGET_TYPE_FRIENDLY,   2000,  2000,   0},
    {SPELL_POWER_WORD_SHIELD,   TARGET_TYPE_FRIENDLY,   1500,  15000,  70},
    {SPELL_MANA_BURN,           TARGET_TYPE_RANDOM,     2000,  7000,   0, SELECT_FLAG_POWER_MANA},
};

struct boss_crusader_priest_discAI : public trial_crusader_commonAI
{
    boss_crusader_priest_discAI(Creature* pCreature) : trial_crusader_commonAI(pCreature, m_aPriestHealerAbilities, MAX_PRIEST_DISC_SPELLS) { Reset(); }

    void Reset() override
    {
        m_uiAIType              = AI_TYPE_HEALER;
        m_attackDistance = 15.0f;

        trial_crusader_commonAI::Reset();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        uint8 uiIndex = 99;
        uint32 uiSpellEntry = 0;
        uint32 uiSpellTimer = 0;

        switch (eventType)
        {
            case AI_EVENT_CRITICAL_HEALTH:
                uiIndex = urand(0, 1) ? 2 : 3;
                break;
            case AI_EVENT_LOST_HEALTH:
                uiIndex = 2;
                break;
            case AI_EVENT_LOST_SOME_HEALTH:
                uiIndex = 1;
                break;
            case AI_EVENT_GOT_CCED:
                uiSpellEntry = SPELL_DISPEL_MAGIC;
                break;
            default:
                break;
        }

        if (!uiSpellEntry)
        {
            if (uiIndex > MAX_PRIEST_DISC_SPELLS - 1)
                return;
            uiSpellEntry = m_aPriestHealerAbilities[uiIndex].m_uiSpellId;
            uiSpellTimer = m_aPriestHealerAbilities[uiIndex].m_uiCooldown;
        }

        if (!m_uiSpellTimer[uiIndex])
        {
            if (DoCastSpellIfCan(pSender, uiSpellEntry) == CAST_OK)
            {
                if (uiSpellTimer)
                    m_uiSpellTimer[uiIndex] = uiSpellTimer;
            }
        }
    }
};

/*######
## boss_crusader_shaman_resto
######*/

static const CrusaderAbilityStruct m_aShamanHealerAbilities[MAX_SHAMAN_RESTO_SPELLS] =
{
    {SPELL_HEROISM,             TARGET_TYPE_SELF,       60000, 300000, 0},
    {SPELL_LESSER_HEALING_WAVE, TARGET_TYPE_FRIENDLY,   1500,  1500,   0},
    {SPELL_RIPTIDE,             TARGET_TYPE_FRIENDLY,   1500,  6000,   0},
    {SPELL_EARTH_SHIELD,        TARGET_TYPE_FRIENDLY,   1500,  1500,   0},
    {SPELL_CLEANSE_SPIRIT,      TARGET_TYPE_FRIENDLY,   1500,  1500,   0},
    {SPELL_HEX,                 TARGET_TYPE_RANDOM,     2000,  45000,  0},
    {SPELL_EARTH_SHOCK,         TARGET_TYPE_RANDOM,     2000,  6000,   0},
};

struct boss_crusader_shaman_restoAI : public trial_crusader_commonAI
{
    boss_crusader_shaman_restoAI(Creature* pCreature) : trial_crusader_commonAI(pCreature, m_aShamanHealerAbilities, MAX_SHAMAN_RESTO_SPELLS) { Reset(); }

    void Reset() override
    {
        m_uiAIType              = AI_TYPE_HEALER;
        m_attackDistance = 15.0f;

        trial_crusader_commonAI::Reset();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        uint8 uiIndex = 99;
        uint32 uiSpellEntry = 0;
        uint32 uiSpellTimer = 0;

        switch (eventType)
        {
            case AI_EVENT_CRITICAL_HEALTH:
                uiIndex = 1;
                break;
            case AI_EVENT_LOST_HEALTH:
                uiIndex = 2;
                break;
            case AI_EVENT_LOST_SOME_HEALTH:
                uiIndex = 3;
                break;
            case AI_EVENT_GOT_CCED:
                uiSpellEntry = SPELL_CLEANSE_SPIRIT;
                break;
            default:
                break;
        }

        if (!uiSpellEntry)
        {
            if (uiIndex > MAX_SHAMAN_RESTO_SPELLS - 1)
                return;
            uiSpellEntry = m_aShamanHealerAbilities[uiIndex].m_uiSpellId;
            uiSpellTimer = m_aShamanHealerAbilities[uiIndex].m_uiCooldown;
        }

        if (!m_uiSpellTimer[uiIndex])
        {
            if (DoCastSpellIfCan(pSender, uiSpellEntry) == CAST_OK)
            {
                if (uiSpellTimer)
                    m_uiSpellTimer[uiIndex] = uiSpellTimer;
            }
        }
    }
};

/*######
## CRUSADERS RANGED
######*/

/*######
## boss_crusader_druid_balance
######*/

static const CrusaderAbilityStruct m_aDruidDamageAbilities[MAX_DRUID_BALANCE_SPELLS] =
{
    {SPELL_BARKSKIN,            TARGET_TYPE_SELF,   5000,  60000,  50},
    {SPELL_MOONFIRE,            TARGET_TYPE_VICTIM, 2000,  30000,  0},
    {SPELL_STARFIRE,            TARGET_TYPE_VICTIM, 2000,  20000,  0},
    {SPELL_FAERIE_FIRE,         TARGET_TYPE_VICTIM, 2000,  40000,  0},
    {SPELL_INSECT_SWARM,        TARGET_TYPE_VICTIM, 2000,  15000,  0},
    {SPELL_WRATH,               TARGET_TYPE_VICTIM, 2000,  5000,   0},
    {SPELL_FORCE_OF_NATURE,     TARGET_TYPE_RANDOM, 1500,  180000, 0},
    {SPELL_CYCLONE,             TARGET_TYPE_RANDOM, 5000,  6000,   0},
    {SPELL_ENTANGLING_ROOTS,    TARGET_TYPE_RANDOM, 5000,  10000,  0},
};

struct boss_crusader_druid_balanceAI : public trial_crusader_commonAI
{
    boss_crusader_druid_balanceAI(Creature* pCreature) : trial_crusader_commonAI(pCreature, m_aDruidDamageAbilities, MAX_DRUID_BALANCE_SPELLS) { Reset(); }

    void Reset() override
    {
        m_uiAIType              = AI_TYPE_RANGED;
        m_attackDistance = 15.0f;

        trial_crusader_commonAI::Reset();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        uint8 uiIndex = 99;
        switch (eventType)
        {
            case AI_EVENT_JUST_DIED:
            case AI_EVENT_CRITICAL_HEALTH:
                uiIndex = urand(0, 1) ? 7 : 8;
                break;
            default:
                break;
        }

        if (uiIndex > MAX_DRUID_BALANCE_SPELLS - 1)
            return;

        if (!m_uiSpellTimer[uiIndex])
        {
            if (DoCastSpellIfCan(pInvoker, m_aDruidDamageAbilities[uiIndex].m_uiSpellId) == CAST_OK)
                m_uiSpellTimer[uiIndex] = m_aDruidDamageAbilities[uiIndex].m_uiCooldown;
        }
    }
};

/*######
## boss_crusader_hunter
######*/

static const CrusaderAbilityStruct m_aHunterAbilities[MAX_HUNTER_SPELLS] =
{
    {SPELL_DETERRENCE,      TARGET_TYPE_SELF,   2000,  90000,  30},
    {SPELL_DISENGAGE,       TARGET_TYPE_SELF,   1000,  30000,  50},
    {SPELL_FROST_TRAP,      TARGET_TYPE_SELF,   1500,  30000,  0},
    {SPELL_SHOOT,           TARGET_TYPE_VICTIM, 2000,  3000,   0},
    {SPELL_STEADY_SHOT,     TARGET_TYPE_VICTIM, 1500,  5000,   0},
    {SPELL_EXPLOSIVE_SHOT,  TARGET_TYPE_VICTIM, 1500,  6000,   0},
    {SPELL_AIMED_SHOT,      TARGET_TYPE_VICTIM, 1000,  10000,  0},
    {SPELL_WYVERN_STING,    TARGET_TYPE_RANDOM, 1500,  60000,  0},
    {SPELL_WING_CLIP,       TARGET_TYPE_RANDOM, 1500,  6000,   0, SELECT_FLAG_IN_MELEE_RANGE},
};

struct boss_crusader_hunterAI : public trial_crusader_commonAI
{
    boss_crusader_hunterAI(Creature* pCreature) : trial_crusader_commonAI(pCreature, m_aHunterAbilities, MAX_HUNTER_SPELLS) { Reset(); }

    void Reset() override
    {
        m_uiAIType              = AI_TYPE_RANGED;
        m_attackDistance = 15.0f;

        trial_crusader_commonAI::Reset();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            DoCastSpellIfCan(m_creature, SPELL_CALL_PET);
        else
        {
            uint8 uiIndex = 99;
            switch (eventType)
            {
                case AI_EVENT_CRITICAL_HEALTH:
                    switch (urand(0, 2))
                    {
                        case 0: uiIndex = 1; break;
                        case 1: uiIndex = 2; break;
                        case 2: uiIndex = 7; break;
                    }
                    break;
                default:
                    break;
            }

            if (uiIndex > MAX_HUNTER_SPELLS - 1)
                return;

            if (!m_uiSpellTimer[uiIndex])
            {
                if (DoCastSpellIfCan(pInvoker, m_aHunterAbilities[uiIndex].m_uiSpellId) == CAST_OK)
                    m_uiSpellTimer[uiIndex] = m_aHunterAbilities[uiIndex].m_uiCooldown;
            }
        }
    }
};

/*######
## boss_crusader_mage
######*/

static const CrusaderAbilityStruct m_aMageAbilities[MAX_MAGE_SPELLS] =
{
    {SPELL_ARCANE_EXPLOSION,    TARGET_TYPE_SELF,   2000,  20000,  0},
    {SPELL_BLINK,               TARGET_TYPE_SELF,   1000,  15000,  0},
    {SPELL_FROST_NOVA,          TARGET_TYPE_SELF,   2000,  25000,  0},
    {SPELL_ICE_BLOCK,           TARGET_TYPE_SELF,   2000,  300000, 20},
    {SPELL_ARCANE_BARRAGE,      TARGET_TYPE_VICTIM, 2000,  30000,  0},
    {SPELL_ARCANE_BLAST,        TARGET_TYPE_VICTIM, 2500,  20000,  0},
    {SPELL_FROSTBOLT,           TARGET_TYPE_VICTIM, 2000,  15000,  0},
    {SPELL_COUNTERSPELL,        TARGET_TYPE_RANDOM, 1000,  24000,  0, SELECT_FLAG_POWER_MANA},
    {SPELL_POLYMORPH,           TARGET_TYPE_RANDOM, 2000,  15000,  0},
};

struct boss_crusader_mageAI : public trial_crusader_commonAI
{
    boss_crusader_mageAI(Creature* pCreature) : trial_crusader_commonAI(pCreature, m_aMageAbilities, MAX_MAGE_SPELLS) { Reset(); }

    void Reset() override
    {
        m_uiAIType              = AI_TYPE_RANGED;
        m_attackDistance = 15.0f;

        trial_crusader_commonAI::Reset();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        uint8 uiIndex = 99;
        switch (eventType)
        {
            case AI_EVENT_CRITICAL_HEALTH:
                uiIndex = 7;
                break;
            case AI_EVENT_GOT_CCED:
                uiIndex = 1;
                break;
            default:
                break;
        }

        if (uiIndex > MAX_HUNTER_SPELLS - 1)
            return;

        if (!m_uiSpellTimer[uiIndex])
        {
            if (DoCastSpellIfCan(pInvoker, m_aMageAbilities[uiIndex].m_uiSpellId) == CAST_OK)
                m_uiSpellTimer[uiIndex] = m_aMageAbilities[uiIndex].m_uiCooldown;
        }
    }
};

/*######
## boss_crusader_priest_shadow
######*/

static const CrusaderAbilityStruct m_aPriestDamageAbilities[MAX_PRIEST_DAMAGE_SPELLS] =
{
    {SPELL_DISPERSION,          TARGET_TYPE_SELF,     6000,  180000, 50},
    {SPELL_PSYCHIC_SCREAM,      TARGET_TYPE_SELF,     1500,  30000,  0},
    {SPELL_MIND_BLAST,          TARGET_TYPE_VICTIM,   1500,  8000,   0},
    {SPELL_MIND_FLAY,           TARGET_TYPE_VICTIM,   2000,  15000,  0},
    {SPELL_PSYCHIC_HORROR,      TARGET_TYPE_VICTIM,   2000,  120000, 30},
    {SPELL_SHADOW_WORD_PAIN,    TARGET_TYPE_VICTIM,   2000,  20000,  0},
    {SPELL_VAMPIRIC_TOUCH,      TARGET_TYPE_VICTIM,   2000,  2000,   0},
    {SPELL_SILENCE,             TARGET_TYPE_RANDOM,   2000,  45000,  0, SELECT_FLAG_POWER_MANA},
};

struct boss_crusader_priest_shadowAI : public trial_crusader_commonAI
{
    boss_crusader_priest_shadowAI(Creature* pCreature) : trial_crusader_commonAI(pCreature, m_aPriestDamageAbilities, MAX_PRIEST_DAMAGE_SPELLS) { Reset(); }

    void Reset() override
    {
        m_uiAIType              = AI_TYPE_RANGED;
        m_attackDistance = 15.0f;

        trial_crusader_commonAI::Reset();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        uint8 uiIndex = 99;
        uint32 uiSpellEntry = 0;
        uint32 uiSpellTimer = 0;

        switch (eventType)
        {
            case AI_EVENT_CRITICAL_HEALTH:
                uiIndex = 1;
                break;
            case AI_EVENT_GOT_CCED:
                uiSpellEntry = SPELL_DISPEL;
                break;
            default:
                break;
        }

        if (!uiSpellEntry)
        {
            if (uiIndex > MAX_PRIEST_DAMAGE_SPELLS - 1)
                return;
            uiSpellEntry = m_aPriestDamageAbilities[uiIndex].m_uiSpellId;
            uiSpellTimer = m_aPriestDamageAbilities[uiIndex].m_uiCooldown;
        }

        if (!m_uiSpellTimer[uiIndex])
        {
            if (DoCastSpellIfCan(pSender, uiSpellEntry) == CAST_OK)
            {
                if (uiSpellTimer)
                    m_uiSpellTimer[uiIndex] = uiSpellTimer;
            }
        }
    }
};

/*######
## boss_crusader_warlock
######*/

static const CrusaderAbilityStruct m_aWarlockAbilities[MAX_WARLOCK_SPELLS] =
{
    {SPELL_HELLFIRE,                TARGET_TYPE_SELF,     1500,  30000,  0},
    {SPELL_SHADOW_BOLT,             TARGET_TYPE_VICTIM,   2000,  15000,  0},
    {SPELL_UNSTABLE_AFFLICTION,     TARGET_TYPE_VICTIM,   2000,  20000,  0},
    {SPELL_CORRUPTION,              TARGET_TYPE_VICTIM,   2000,  20000,  0},
    {SPELL_CURSE_OF_AGONY,          TARGET_TYPE_VICTIM,   2000,  30000,  0},
    {SPELL_CURSE_OF_EXHAUSTION,     TARGET_TYPE_VICTIM,   2000,  20000,  0},
    {SPELL_SEARING_PAIN,            TARGET_TYPE_VICTIM,   2000,  15000,  0},
    {SPELL_DEATH_COIL_WARLOCK,      TARGET_TYPE_VICTIM,   2000,  120000, 50},
    {SPELL_FEAR,                    TARGET_TYPE_RANDOM,   2000,  30000,  0},
};

struct boss_crusader_warlockAI : public trial_crusader_commonAI
{
    boss_crusader_warlockAI(Creature* pCreature) : trial_crusader_commonAI(pCreature, m_aWarlockAbilities, MAX_WARLOCK_SPELLS) { Reset(); }

    void Reset() override
    {
        m_uiAIType              = AI_TYPE_RANGED;
        m_attackDistance = 15.0f;

        trial_crusader_commonAI::Reset();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_FELHUNTER);
        else
        {
            uint8 uiIndex = 99;
            switch (eventType)
            {
                case AI_EVENT_JUST_DIED:
                case AI_EVENT_CRITICAL_HEALTH:
                    uiIndex = 8;
                    break;
                default:
                    break;
            }

            if (uiIndex > MAX_WARLOCK_SPELLS - 1)
                return;

            if (!m_uiSpellTimer[uiIndex])
            {
                if (DoCastSpellIfCan(pSender, m_aWarlockAbilities[uiIndex].m_uiSpellId) == CAST_OK)
                    m_uiSpellTimer[uiIndex] = m_aWarlockAbilities[uiIndex].m_uiCooldown;
            }
        }
    }
};

/*######
## MELEE CRUSADERS
######*/

/*######
## boss_crusader_death_knight
######*/

static const CrusaderAbilityStruct m_aDeathKnightAbilities[MAX_DEATH_KNIGHT_SPELLS] =
{
    {SPELL_ICEBOUND_FORTITUDE,  TARGET_TYPE_SELF,     10000, 60000,  0},
    {SPELL_CHAINS_OF_ICE,       TARGET_TYPE_VICTIM,   1500,  20000,  0},
    {SPELL_FROST_STRIKE,        TARGET_TYPE_VICTIM,   1500,  6000,   0},
    {SPELL_ICY_TOUCH,           TARGET_TYPE_RANDOM,   2000,  8000,   0},
    {SPELL_DEATH_COIL,          TARGET_TYPE_RANDOM,   2000,  8000,   0},
    {SPELL_DEATH_GRIP,          TARGET_TYPE_RANDOM,   1000,  35000,  0},
    {SPELL_STRANGULATE,         TARGET_TYPE_RANDOM,   40000, 120000, 0},
};

struct boss_crusader_death_knightAI : public trial_crusader_commonAI
{
    boss_crusader_death_knightAI(Creature* pCreature) : trial_crusader_commonAI(pCreature, m_aDeathKnightAbilities, MAX_DEATH_KNIGHT_SPELLS) { Reset(); }

    void Reset() override
    {
        trial_crusader_commonAI::Reset();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        uint8 uiIndex = 99;

        switch (eventType)
        {
            case AI_EVENT_CRITICAL_HEALTH:
                uiIndex = urand(0, 1) ? 5 : 6;
                break;

            default:
                break;
        }

        if (uiIndex > MAX_DEATH_KNIGHT_SPELLS - 1)
            return;

        if (!m_uiSpellTimer[uiIndex])
        {
            if (DoCastSpellIfCan(pInvoker, m_aDeathKnightAbilities[uiIndex].m_uiSpellId) == CAST_OK)
                m_uiSpellTimer[uiIndex] = m_aDeathKnightAbilities[uiIndex].m_uiCooldown;
        }
    }
};

/*######
## boss_crusader_warrior
######*/

static const CrusaderAbilityStruct m_aWarriorAbilities[MAX_WARRIOR_SPELLS] =
{
    {SPELL_BLADESTORM,          TARGET_TYPE_SELF,     10000, 90000,  0},
    {SPELL_RETALIATION,         TARGET_TYPE_SELF,     30000, 300000, 0},
    {SPELL_SUNDER_ARMOR,        TARGET_TYPE_VICTIM,   1500,  5000,   0},
    {SPELL_DISARM,              TARGET_TYPE_VICTIM,   20000, 60000,  0},
    {SPELL_INTIMIDATING_SHOUT,  TARGET_TYPE_VICTIM,   10000, 30000,  0},
    {SPELL_MORTAL_STRIKE,       TARGET_TYPE_VICTIM,   2000,  10000,  0},
    {SPELL_OVERPOWER,           TARGET_TYPE_VICTIM,   2000,  6000,   0},
    {SPELL_SHATTERING_THROW,    TARGET_TYPE_RANDOM,   40000, 300000, 0},
    {SPELL_CHARGE,              TARGET_TYPE_RANDOM,   1000,  15000,  0, SELECT_FLAG_NOT_IN_MELEE_RANGE},
};

struct boss_crusader_warriorAI : public trial_crusader_commonAI
{
    boss_crusader_warriorAI(Creature* pCreature) : trial_crusader_commonAI(pCreature, m_aWarriorAbilities, MAX_WARRIOR_SPELLS) { Reset(); }

    void Reset() override { trial_crusader_commonAI::Reset(); }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        uint8 uiIndex = 99;
        switch (eventType)
        {
            case AI_EVENT_CRITICAL_HEALTH:
            case AI_EVENT_LOST_HEALTH:
                uiIndex = 8;
                break;
            default:
                break;
        }

        if (uiIndex > MAX_WARRIOR_SPELLS - 1)
            return;

        if (!m_uiSpellTimer[uiIndex])
        {
            if (DoCastSpellIfCan(pInvoker, m_aWarriorAbilities[uiIndex].m_uiSpellId) == CAST_OK)
                m_uiSpellTimer[uiIndex] = m_aWarriorAbilities[uiIndex].m_uiCooldown;
        }
    }
};

/*######
## boss_crusader_paladin_retri
######*/

static const CrusaderAbilityStruct m_aPaladinDamageAbilities[MAX_PALADIN_DAMAGE_SPELLS] =
{
    {SPELL_AVENGING_WRATH,          TARGET_TYPE_SELF,     2000,  180000, 50},
    {SPELL_DIVINE_STORM,            TARGET_TYPE_SELF,     10000, 20000,  0},
    {SPELL_CRUSADER_STRIKE,         TARGET_TYPE_VICTIM,   2000,  15000,  0},
    {SPELL_HAMMER_OF_JUSTICE_RETRI, TARGET_TYPE_RANDOM,   10000, 40000,  0},
    {SPELL_JUDGEMENT_OF_COMMAND,    TARGET_TYPE_RANDOM,   5000,  20000,  0},
    {SPELL_REPENTANCE,              TARGET_TYPE_RANDOM,   30000, 60000,  0},
};

struct boss_crusader_paladin_retriAI : public trial_crusader_commonAI
{
    boss_crusader_paladin_retriAI(Creature* pCreature) : trial_crusader_commonAI(pCreature, m_aPaladinDamageAbilities, MAX_PALADIN_DAMAGE_SPELLS) { Reset(); }

    void Reset() override { trial_crusader_commonAI::Reset(); }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            DoCastSpellIfCan(m_creature, SPELL_SEAL_OF_COMMAND);
        else
        {
            uint8 uiIndex = 99;
            switch (eventType)
            {
                case AI_EVENT_CRITICAL_HEALTH:
                case AI_EVENT_LOST_HEALTH:
                    uiIndex = urand(0, 1) ? 3 : 5;
                    break;
                default:
                    break;
            }

            if (uiIndex > MAX_PALADIN_DAMAGE_SPELLS - 1)
                return;

            if (!m_uiSpellTimer[uiIndex])
            {
                if (DoCastSpellIfCan(pInvoker, m_aPaladinDamageAbilities[uiIndex].m_uiSpellId) == CAST_OK)
                    m_uiSpellTimer[uiIndex] = m_aPaladinDamageAbilities[uiIndex].m_uiCooldown;
            }
        }
    }
};

/*######
## boss_crusader_shaman_enha
######*/

static const CrusaderAbilityStruct m_aShamanDamageAbilities[MAX_SHAMAN_DAMAGE_SPELLS] =
{
    {SPELL_HEROISM,            TARGET_TYPE_SELF,     60000, 300000, 0},
    {SPELL_MAELSTROM_WEAPON,   TARGET_TYPE_SELF,     30000, 40000,  0},
    {SPELL_WINDFURY,           TARGET_TYPE_SELF,     10000, 10000,  0},
    {SPELL_EARTH_SHOCK,        TARGET_TYPE_VICTIM,   2000,  8000,   0},
    {SPELL_LAVA_LASH,          TARGET_TYPE_VICTIM,   2000,  10000,  0},
    {SPELL_STORMSTRIKE,        TARGET_TYPE_VICTIM,   7000,  15000,  0},
};

struct boss_crusader_shaman_enhaAI : public trial_crusader_commonAI
{
    boss_crusader_shaman_enhaAI(Creature* pCreature) : trial_crusader_commonAI(pCreature, m_aShamanDamageAbilities, MAX_SHAMAN_DAMAGE_SPELLS) { Reset(); }

    void Reset() override { trial_crusader_commonAI::Reset(); }
};

/*######
## boss_crusader_rogue
######*/

static const CrusaderAbilityStruct m_aRogueAbilities[MAX_ROGUE_SPELLS] =
{
    {SPELL_BLADE_FURRY,       TARGET_TYPE_SELF,     40000, 120000, 0},
    {SPELL_CLOAK_OF_SHADOWS,  TARGET_TYPE_SELF,     2000,  90000,  50},
    {SPELL_FAN_OF_KNIVES,     TARGET_TYPE_SELF,     1500,  15000,  0},
    {SPELL_SHADOWSTEP,        TARGET_TYPE_SELF,     8000,  30000,  0},
    {SPELL_EVISCERATE,        TARGET_TYPE_VICTIM,   3000,  20000,  0},
    {SPELL_HEMORRHAGE,        TARGET_TYPE_VICTIM,   2000,  15000,  0},
    {SPELL_WOUND_POISON,      TARGET_TYPE_VICTIM,   5000,  18000,  0},
    {SPELL_BLIND,             TARGET_TYPE_RANDOM,   8000,  120000, 30},
};

struct boss_crusader_rogueAI : public trial_crusader_commonAI
{
    boss_crusader_rogueAI(Creature* pCreature) : trial_crusader_commonAI(pCreature, m_aRogueAbilities, MAX_ROGUE_SPELLS) { Reset(); }

    void Reset() override { trial_crusader_commonAI::Reset(); }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        uint8 uiIndex = 99;
        switch (eventType)
        {
            case AI_EVENT_CRITICAL_HEALTH:
            case AI_EVENT_LOST_HEALTH:
                uiIndex = 7;
                break;
            default:
                break;
        }

        if (uiIndex > MAX_ROGUE_SPELLS - 1)
            return;

        if (!m_uiSpellTimer[uiIndex])
        {
            if (DoCastSpellIfCan(pInvoker, m_aRogueAbilities[uiIndex].m_uiSpellId) == CAST_OK)
                m_uiSpellTimer[uiIndex] = m_aRogueAbilities[uiIndex].m_uiCooldown;
        }
    }
};

/*######
## spell_disengage - 65869
######*/

struct spell_disengage : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->CastSpell(target, 65870, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_faction_champions()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_crusader_death_knight";
    pNewScript->GetAI = &GetNewAIInstance<boss_crusader_death_knightAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crusader_druid_balance";
    pNewScript->GetAI = &GetNewAIInstance<boss_crusader_druid_balanceAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crusader_druid_resto";
    pNewScript->GetAI = &GetNewAIInstance<boss_crusader_druid_restoAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crusader_hunter";
    pNewScript->GetAI = &GetNewAIInstance<boss_crusader_hunterAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crusader_mage";
    pNewScript->GetAI = &GetNewAIInstance<boss_crusader_mageAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crusader_paladin_holy";
    pNewScript->GetAI = &GetNewAIInstance<boss_crusader_paladin_holyAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crusader_paladin_retri";
    pNewScript->GetAI = &GetNewAIInstance<boss_crusader_paladin_retriAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crusader_priest_disc";
    pNewScript->GetAI = &GetNewAIInstance<boss_crusader_priest_discAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crusader_priest_shadow";
    pNewScript->GetAI = &GetNewAIInstance<boss_crusader_priest_shadowAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crusader_rogue";
    pNewScript->GetAI = &GetNewAIInstance<boss_crusader_rogueAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crusader_shaman_enha";
    pNewScript->GetAI = &GetNewAIInstance<boss_crusader_shaman_enhaAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crusader_shaman_resto";
    pNewScript->GetAI = &GetNewAIInstance<boss_crusader_shaman_restoAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crusader_warlock";
    pNewScript->GetAI = &GetNewAIInstance<boss_crusader_warlockAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crusader_warrior";
    pNewScript->GetAI = &GetNewAIInstance<boss_crusader_warriorAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_disengage>("spell_disengage");
}

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
SDName: Boss_Priestess_Delrissa
SD%Complete: 90
SDComment: Script handles Delrissa and her companions AI. They need special PvP-like behavior. Timers need adjustments
SDCategory: Magister's Terrace
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "magisters_terrace.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                   = -1585012,
    SAY_DEATH                   = -1585022,

    SPELL_HEALING_POTION        = 15503,
    SPELL_DISPEL_MAGIC          = 27609,
    SPELL_MEDALLION             = 46227,
    SPELL_FLASH_HEAL            = 17843,
    SPELL_SHADOW_WORD_PAIN      = 14032,
    SPELL_SHADOW_WORD_PAIN_H    = 15654,
    SPELL_SCREAM                = 27610,
    SPELL_SHIELD                = 44291,            // maybe 44175?
    SPELL_SHIELD_H              = 46193,
    SPELL_RENEW                 = 44174,
    SPELL_RENEW_H               = 46192,

    SPELL_PERMANENT_FEIGN_DEATH = 29266,
    SPELL_SUICIDE               = 3617,

    MAX_COMPANIONS              = 8,
};

static const int32 aPlayerDeath[] = { -1585017, -1585018, -1585019, -1585020, -1585021};
static const uint32 aDelrissaLackeys[MAX_COMPANIONS] = {NPC_KAGANI, NPC_ELLRYS, NPC_ERAMAS, NPC_YAZZAI, NPC_SALARIS, NPC_GARAXXAS, NPC_APOKO, NPC_ZELFAN};

static const float aLackeyLocations[MAX_DELRISSA_ADDS][4] =
{
    {123.77f,  17.6007f, -19.921f, 4.98f},
    {131.731f, 15.0827f, -19.921f, 4.98f},
    {121.563f, 15.6213f, -19.921f, 4.98f},
    {129.988f, 17.2355f, -19.921f, 4.98f},
};

/*######
## boss_priestess_delrissa
######*/

enum DelrissaActions
{
    DELRISSA_HEAL,
    DELRISSA_RENEW,
    DELRISSA_SHIELD,
    DELRISSA_SHADOW_WORD_PAIN,
    DELRISSA_DISPEL,
    DELRISSA_SCREAM,
    DELRISSA_MEDALLION,
    DELRISSA_ACTION_MAX,
};

struct boss_priestess_delrissaAI : public CombatAI
{
    boss_priestess_delrissaAI(Creature* creature) : CombatAI(creature, DELRISSA_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddCombatAction(DELRISSA_HEAL, 15000u);
        AddCombatAction(DELRISSA_RENEW, 10000u);
        AddCombatAction(DELRISSA_SHIELD, 2000u);
        AddCombatAction(DELRISSA_SHADOW_WORD_PAIN, 5000u);
        AddCombatAction(DELRISSA_DISPEL, 7500u);
        AddCombatAction(DELRISSA_SCREAM, 9000u);
        if (!m_isRegularMode)
            AddCombatAction(DELRISSA_MEDALLION, 1000, 2000);
        else
            AddCombatAction(DELRISSA_MEDALLION, true);
        SetDeathPrevention(true);
        Reset();
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    std::vector<uint32> m_vuiLackeyEnties;

    uint8 m_playersKilled;
    uint32 m_summonsKilled;

    void Reset() override
    {
        CombatAI::Reset();
        m_playersKilled = 0;
        m_summonsKilled = 0;

        DoInitializeCompanions();
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_DELRISSA, FAIL);
    }

    void Aggro(Unit* who) override
    {
        if (who->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_DELRISSA, IN_PROGRESS);
    }

    void JustPreventedDeath(Unit* attacker) override
    {
        DoFakeDeath(SPELL_PERMANENT_FEIGN_DEATH);
        if (m_summonsKilled >= 4)
            m_creature->CastSpell(nullptr, SPELL_SUICIDE, TRIGGERED_OLD_TRIGGERED);
    }

    // Summon four random adds to help during the fight
    void DoInitializeCompanions()
    {
        // can be called if creature are dead, so avoid
        if (!m_creature->isAlive())
            return;

        // it's empty, so first time
        if (m_vuiLackeyEnties.empty())
        {
            // pre-allocate size for speed
            m_vuiLackeyEnties.resize(MAX_COMPANIONS);

            // fill vector array with entries from creature array
            for (uint8 i = 0; i < MAX_COMPANIONS; ++i)
                m_vuiLackeyEnties[i] = aDelrissaLackeys[i];

            std::random_shuffle(m_vuiLackeyEnties.begin(), m_vuiLackeyEnties.end());

            // Summon the 4 entries
            for (uint8 i = 0; i < MAX_DELRISSA_ADDS; ++i)
                m_creature->SummonCreature(m_vuiLackeyEnties[i], aLackeyLocations[i][0], aLackeyLocations[i][1], aLackeyLocations[i][2], aLackeyLocations[i][3], TEMPSPAWN_CORPSE_DESPAWN, 0);
        }
        // Resummon the killed adds
        else
        {
            if (!m_instance)
                return;

            for (uint8 i = 0; i < MAX_DELRISSA_ADDS; ++i)
            {
                // If we already have the creature on the map, then don't summon it
                if (m_instance->GetSingleCreatureFromStorage(m_vuiLackeyEnties[i], true))
                    continue;

                m_creature->SummonCreature(m_vuiLackeyEnties[i], aLackeyLocations[i][0], aLackeyLocations[i][1], aLackeyLocations[i][2], aLackeyLocations[i][3], TEMPSPAWN_CORPSE_DESPAWN, 0);
            }
        }
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(aPlayerDeath[m_playersKilled], m_creature);
        ++m_playersKilled;

        // reset counter
        if (m_playersKilled == 5)
            m_playersKilled = 0;
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        ++m_summonsKilled;
        if (m_summonsKilled >= 4)
            m_creature->CastSpell(nullptr, SPELL_SUICIDE, TRIGGERED_OLD_TRIGGERED);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (!m_instance)
            return;

        // Remove lootable flag if the lackeys are not killed
        if (m_instance->GetData(TYPE_DELRISSA) == SPECIAL)
            m_instance->SetData(TYPE_DELRISSA, DONE);
        else
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
    }

    virtual void ExecuteActions() override
    {
        if (!m_creature->isAlive() || m_creature->GetCombatManager().IsEvadingHome() || (m_unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED) && m_unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED)) ||
            m_unit->hasUnitState(UNIT_STAT_PROPELLED | UNIT_STAT_RETREATING) || m_unit->IsNonMeleeSpellCasted(false))
            return; // custom condition due to her medallion - TODO: resolve globally

        for (uint32 i = 0; i < GetCombatActionCount(); ++i)
        {
            // can be changed on any action - prevent all additional ones
            if (GetCombatScriptStatus())
                return;

            if (GetActionReadyStatus(i))
                ExecuteAction(i);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case DELRISSA_HEAL:
                if (Unit* target = DoSelectLowestHpFriendly(50.0f))
                    if (DoCastSpellIfCan(target, SPELL_FLASH_HEAL) == CAST_OK)
                        ResetCombatAction(action, urand(15000, 20000));
                break;
            case DELRISSA_RENEW:
                if (Unit* target = DoSelectLowestHpFriendly(50.0f))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_RENEW : SPELL_RENEW_H) == CAST_OK)
                        ResetCombatAction(action, urand(5000, 10000));
                break;
            case DELRISSA_SHIELD:
                if (Unit* target = DoSelectLowestHpFriendly(50.0f))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_SHIELD : SPELL_SHIELD_H) == CAST_OK)
                        ResetCombatAction(action, urand(30000, 35000));
                break;
            case DELRISSA_DISPEL:
            {
                Unit* target = nullptr;
                std::list<Creature*> lTempList = DoFindFriendlyCC(50.0f);

                if (!lTempList.empty())
                    target = *(lTempList.begin());
                else
                    target = DoSelectLowestHpFriendly(50.0f);

                if (target)
                    if (DoCastSpellIfCan(target, SPELL_DISPEL_MAGIC) == CAST_OK)
                        ResetCombatAction(action, urand(12000, 15000));
                break;
            }
            case DELRISSA_MEDALLION:
                if (m_creature->isFrozen() || m_creature->IsCrowdControlled())
                    if (DoCastSpellIfCan(nullptr, SPELL_MEDALLION) == CAST_OK)
                        DisableCombatAction(action);
                break;
            case DELRISSA_SHADOW_WORD_PAIN:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_SHADOW_WORD_PAIN : SPELL_SHADOW_WORD_PAIN_H) == CAST_OK)
                        ResetCombatAction(action, 10000);
                break;
            case DELRISSA_SCREAM:
                if (DoCastSpellIfCan(nullptr, SPELL_SCREAM) == CAST_OK)
                    ResetCombatAction(action, urand(15000, 20000));
                break;
        }
    }
};

UnitAI* GetAI_boss_priestess_delrissa(Creature* creature)
{
    return new boss_priestess_delrissaAI(creature);
}

/*######
## priestess_companion_common
######*/

enum CompanionActions
{
    COMPANION_POTION,
    COMPANION_RETARGET,
    COMPANION_MEDALLION,
    COMPANION_ACTION_MAX,
};

struct priestess_companion_commonAI : public CombatAI
{
    priestess_companion_commonAI(Creature* creature, uint32 actions) : CombatAI(creature, actions),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddTimerlessCombatAction(COMPANION_POTION, true);
        AddCombatAction(COMPANION_RETARGET, 5000, 15000);
        if (!m_isRegularMode)
            AddCombatAction(COMPANION_MEDALLION, 1000, 2000);
        else
            AddCombatAction(COMPANION_MEDALLION, true);
        Reset();
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    uint32 m_uiResetThreatTimer;
    uint32 m_uiMedallionTimer;

    void Reset() override
    {
        CombatAI::Reset();

        // These guys does not follow normal threat system rules
        // For later development, some alternative threat system should be made
        // We do not know what this system is based upon, but one theory is class (healers=high threat, dps=medium, etc)
        // We reset their threat frequently as an alternative until such a system exist
    }

    void KilledUnit(Unit* victim) override
    {
        if (!m_instance)
            return;

        if (Creature* delrissa = m_instance->GetSingleCreatureFromStorage(NPC_DELRISSA))
            delrissa->AI()->KilledUnit(victim);
    }

    virtual void ExecuteActions() override
    {
        if (!m_creature->isAlive() || m_creature->GetCombatManager().IsEvadingHome() || (m_unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED) && m_unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED)) ||
            m_unit->hasUnitState(UNIT_STAT_PROPELLED | UNIT_STAT_RETREATING) || m_unit->IsNonMeleeSpellCasted(false))
            return; // custom condition due to medallion - TODO: resolve globally

        for (uint32 i = 0; i < GetCombatActionCount(); ++i)
        {
            // can be changed on any action - prevent all additional ones
            if (GetCombatScriptStatus())
                return;

            if (GetActionReadyStatus(i))
                ExecuteAction(i);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case COMPANION_POTION:
                if (m_creature->GetHealthPercent() > 25.0f)
                    return;
                if (DoCastSpellIfCan(nullptr, SPELL_HEALING_POTION) == CAST_OK)
                    SetActionReadyStatus(action, false);
                break;
            case COMPANION_RETARGET:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER))
                {
                    DoResetThreat();
                    AttackStart(target);
                    ResetCombatAction(action, urand(5000, 15000));
                }
                break;
            case COMPANION_MEDALLION:
                if (m_creature->isFrozen() || m_creature->IsCrowdControlled())
                    if (DoCastSpellIfCan(nullptr, SPELL_MEDALLION) == CAST_OK)
                        DisableCombatAction(action);
                break;
        }
    }
};

enum
{
    SPELL_KIDNEY_SHOT       = 27615,
    SPELL_GOUGE             = 12540,
    SPELL_KICK              = 27613,
    SPELL_VANISH            = 44290,
    SPELL_BACKSTAB          = 15657,
    SPELL_BACKSTAB_H        = 15582,
    SPELL_EVISCERATE        = 27611,
    SPELL_EVISCERATE_H      = 46189,
};

/*######
## npc_kagani_nightstrike - Rogue
######*/

enum KaganiActions
{
    KAGANI_GOUGE = COMPANION_ACTION_MAX,
    KAGANI_KICK,
    KAGANI_VANISH,
    KAGANI_EVISCERATE,
    KAGANI_ACTION_MAX,
    KAGANI_VANISH_END,
};

struct npc_kagani_nightstrikeAI : public priestess_companion_commonAI
{
    npc_kagani_nightstrikeAI(Creature* creature) : priestess_companion_commonAI(creature, KAGANI_ACTION_MAX)
    {
        AddCombatAction(KAGANI_GOUGE, 5500u);
        AddCombatAction(KAGANI_KICK, 7000u);
        AddCombatAction(KAGANI_VANISH, 2000u);
        AddCombatAction(KAGANI_EVISCERATE, 6000u);
        AddCustomAction(KAGANI_VANISH_END, true, [&]()
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BACKSTAB, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KIDNEY_SHOT, CAST_TRIGGERED);
        });
        Reset();
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KAGANI_GOUGE:
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_GOUGE) == CAST_OK)
                    ResetCombatAction(action, 5500);
                break;
            case KAGANI_KICK:
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_KICK) == CAST_OK)
                    ResetCombatAction(action, 7000);
                break;
            case KAGANI_VANISH:
                if (DoCastSpellIfCan(nullptr, SPELL_VANISH) == CAST_OK)
                {
                    // Prefer targets with mana
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_POWER_MANA | SELECT_FLAG_PLAYER))
                    {
                        DoResetThreat();
                        AttackStart(target);
                    }

                    ResetCombatAction(action, 30000);
                    ResetTimer(KAGANI_VANISH_END, 10000);
                }
                break;
            case KAGANI_EVISCERATE:
                if (DoCastSpellIfCan(m_creature->getVictim(), m_isRegularMode ? SPELL_EVISCERATE : SPELL_EVISCERATE_H) == CAST_OK)
                    ResetCombatAction(action, 4000);
                break;
        }
        priestess_companion_commonAI::ExecuteAction(action);
    }
};

UnitAI* GetAI_npc_kagani_nightstrike(Creature* creature)
{
    return new npc_kagani_nightstrikeAI(creature);
}

enum
{
    SPELL_IMMOLATE              = 44267,
    SPELL_IMMOLATE_H            = 46191,
    SPELL_SHADOW_BOLT           = 12471,
    SPELL_SHADOW_BOLT_H         = 15232,
    SPELL_SEED_OF_CORRUPTION    = 44141,
    SPELL_CURSE_OF_AGONY        = 14875,
    SPELL_CURSE_OF_AGONY_H      = 46190,
    SPELL_FEAR                  = 38595,
    SPELL_DEATH_COIL            = 44142,
    SPELL_SUMMON_IMP            = 44163,

    NPC_FIZZLE                  = 24656,
};

/*######
## npc_ellris_duskhallow - Warlock
######*/

enum EllrisActions
{
    ELLRIS_IMMOLATE = COMPANION_ACTION_MAX,
    ELLRIS_SHADOWBOLT,
    ELLRIS_SEED_OF_CORRUPTION,
    ELLRIS_CURSE_OF_AGONY,
    ELLRIS_FEAR,
    ELLRIS_DEATH_COIL,
    ELLRIS_ACTION_MAX,
};

struct npc_ellris_duskhallowAI : public priestess_companion_commonAI
{
    npc_ellris_duskhallowAI(Creature* creature) : priestess_companion_commonAI(creature, ELLRIS_ACTION_MAX)
    {
        AddCombatAction(ELLRIS_IMMOLATE, 6000u);
        AddCombatAction(ELLRIS_SHADOWBOLT, 3000u);
        AddCombatAction(ELLRIS_SEED_OF_CORRUPTION, 2000u);
        AddCombatAction(ELLRIS_CURSE_OF_AGONY, 1000u);
        AddCombatAction(ELLRIS_FEAR, 10000u);
        AddCombatAction(ELLRIS_DEATH_COIL, 8000u);
        Reset();
    }

    void Reset() override
    {
        priestess_companion_commonAI::Reset();

        // Check if we already have an imp summoned
        if (!GetClosestCreatureWithEntry(m_creature, NPC_FIZZLE, 50.0f))
            DoCastSpellIfCan(nullptr, SPELL_SUMMON_IMP);

        m_attackDistance = 20.0f;
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ELLRIS_IMMOLATE:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_IMMOLATE : SPELL_IMMOLATE_H) == CAST_OK)
                        ResetCombatAction(action, 6000);
                break;
            case ELLRIS_SHADOWBOLT:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT_H) == CAST_OK)
                        ResetCombatAction(action, 5000);
                break;
            case ELLRIS_SEED_OF_CORRUPTION:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_SEED_OF_CORRUPTION) == CAST_OK)
                        ResetCombatAction(action, 10000);
                break;
            case ELLRIS_CURSE_OF_AGONY:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_CURSE_OF_AGONY : SPELL_CURSE_OF_AGONY_H) == CAST_OK)
                        ResetCombatAction(action, 13000);
                break;
            case ELLRIS_FEAR:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_FEAR) == CAST_OK)
                        ResetCombatAction(action, 10000);
                break;
            case ELLRIS_DEATH_COIL:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_DEATH_COIL) == CAST_OK)
                        ResetCombatAction(action, urand(8000, 13000));
                break;
        }
        priestess_companion_commonAI::ExecuteAction(action);
    }
};

UnitAI* GetAI_npc_ellris_duskhallow(Creature* creature)
{
    return new npc_ellris_duskhallowAI(creature);
}

enum
{
    SPELL_KNOCKDOWN     = 11428,
    SPELL_KNOCKDOWN_H   = 46183,
    SPELL_SNAP_KICK     = 46182,
    SPELL_DUAL_WEILD    = 42459,
};

/*######
## npc_eramas_brightblaze - Monk
######*/

enum EramasActions
{
    ERAMAS_KNOCKDOWN = COMPANION_ACTION_MAX,
    ERAMAS_KICK,
    ERAMAS_ACTION_MAX,
};

struct npc_eramas_brightblazeAI : public priestess_companion_commonAI
{
    npc_eramas_brightblazeAI(Creature* creature) : priestess_companion_commonAI(creature, ERAMAS_ACTION_MAX)
    {
        AddCombatAction(ERAMAS_KNOCKDOWN, 6000u);
        AddCombatAction(ERAMAS_KICK, 4500u);
    }

    void Reset() override
    {
        priestess_companion_commonAI::Reset();
        DoCastSpellIfCan(nullptr, SPELL_DUAL_WEILD, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ERAMAS_KNOCKDOWN:
                if (DoCastSpellIfCan(m_creature->getVictim(), m_isRegularMode ? SPELL_KNOCKDOWN : SPELL_KNOCKDOWN_H) == CAST_OK)
                    ResetCombatAction(action, 6000u);
                break;
            case ERAMAS_KICK:
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SNAP_KICK) == CAST_OK)
                    ResetCombatAction(action, 4500u);
                break;
        }
        priestess_companion_commonAI::ExecuteAction(action);
    }
};

UnitAI* GetAI_npc_eramas_brightblaze(Creature* creature)
{
    return new npc_eramas_brightblazeAI(creature);
}

enum
{
    SPELL_POLYMORPH         = 13323,
    SPELL_ICE_BLOCK         = 27619,
    SPELL_BLIZZARD          = 44178,
    SPELL_BLIZZARD_H        = 46195,
    SPELL_ICE_LANCE         = 44176,
    SPELL_ICE_LANCE_H       = 46194,
    SPELL_CONE_OF_COLD      = 12611,
    SPELL_CONE_OF_COLD_H    = 38384,
    SPELL_FROSTBOLT         = 15043,
    SPELL_FROSTBOLT_H       = 15530,
    SPELL_BLINK             = 14514
};

/*######
## npc_yazzai - Mage
######*/

enum YazzaiActions
{
    YAZZAI_POLYMORPH = COMPANION_ACTION_MAX,
    YAZZAI_ICE_BLOCK,
    YAZZAI_BLIZZARD,
    YAZZAI_ICE_LANCE,
    YAZZAI_CONE_OF_COLD,
    YAZZAI_FROSTBOLT,
    YAZZAI_BLINK,
    YAZZAI_ACTION_MAX,
};

struct npc_yazzaiAI : public priestess_companion_commonAI
{
    npc_yazzaiAI(Creature* creature) : priestess_companion_commonAI(creature, YAZZAI_ACTION_MAX)
    {
        AddCombatAction(YAZZAI_POLYMORPH, 1000u);
        AddTimerlessCombatAction(YAZZAI_ICE_BLOCK, true);
        AddCombatAction(YAZZAI_BLIZZARD, 8000u);
        AddCombatAction(YAZZAI_ICE_LANCE, 12000u);
        AddCombatAction(YAZZAI_CONE_OF_COLD, 10000u);
        AddCombatAction(YAZZAI_FROSTBOLT, 3000u);
        AddCombatAction(YAZZAI_BLINK, 8000u);
        Reset();
    }

    void Reset() override
    {
        priestess_companion_commonAI::Reset();

        m_attackDistance = 20.0f;
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case YAZZAI_POLYMORPH:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_POLYMORPH) == CAST_OK)
                        ResetCombatAction(action, 20000);
                break;
            case YAZZAI_ICE_BLOCK:
                if (m_creature->GetHealthPercent() < 35.0f)
                    if (DoCastSpellIfCan(nullptr, SPELL_ICE_BLOCK) == CAST_OK)
                        SetActionReadyStatus(action, false);
                break;
            case YAZZAI_BLIZZARD:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_BLIZZARD : SPELL_BLIZZARD_H) == CAST_OK)
                        ResetCombatAction(action, urand(8000, 15000));
                break;
            case YAZZAI_ICE_LANCE:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_ICE_LANCE : SPELL_ICE_LANCE_H) == CAST_OK)
                        ResetCombatAction(action, 12000);
                break;
            case YAZZAI_CONE_OF_COLD:
                if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_CONE_OF_COLD : SPELL_CONE_OF_COLD_H) == CAST_OK)
                    ResetCombatAction(action, 10000);
                break;
            case YAZZAI_FROSTBOLT:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_FROSTBOLT : SPELL_FROSTBOLT_H) == CAST_OK)
                        ResetCombatAction(action, 8000);
                break;
            case YAZZAI_BLINK:
                // if anybody is in melee range than escape by blink
                if (m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_IN_MELEE_RANGE))
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_BLINK) == CAST_OK)
                        ResetCombatAction(action, 8000);
                }
                else
                    ResetCombatAction(action, 2000);
                break;
        }
        priestess_companion_commonAI::ExecuteAction(action);
    }
};

UnitAI* GetAI_npc_yazzai(Creature* creature)
{
    return new npc_yazzaiAI(creature);
}

enum
{
    SPELL_INTERCEPT_STUN        = 27577,
    SPELL_DISARM                = 27581,
    SPELL_PIERCING_HOWL         = 23600,
    SPELL_FRIGHTENING_SHOUT     = 19134,
    SPELL_HAMSTRING             = 27584,
    SPELL_BATTLE_SHOUT          = 27578,
    SPELL_MORTAL_STRIKE         = 44268
};

/*######
## npc_warlord_salaris - Warrior
######*/

enum
{
    SALARIS_INTERCEPT_STUN = COMPANION_ACTION_MAX,
    SALARIS_DISARM,
    SALARIS_PIERCING_HOWL,
    SALARIS_FRIGHTENING_SHOUT,
    SALARIS_HAMSTRING,
    SALARIS_BATTLE_SHOUT,
    SALARIS_MORTAL_STRIKE,
    SALARIS_ACTION_MAX,
};

struct npc_warlord_salarisAI : public priestess_companion_commonAI
{
    npc_warlord_salarisAI(Creature* creature) : priestess_companion_commonAI(creature, SALARIS_ACTION_MAX)
    {
        AddCombatAction(SALARIS_INTERCEPT_STUN, 500u);
        AddCombatAction(SALARIS_DISARM, 6000u);
        AddCombatAction(SALARIS_PIERCING_HOWL, 10000u);
        AddCombatAction(SALARIS_FRIGHTENING_SHOUT, 18000u);
        AddCombatAction(SALARIS_HAMSTRING, 4500u);
        AddCombatAction(SALARIS_MORTAL_STRIKE, 8000u);
        Reset();
    }

    void Aggro(Unit* /*who*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_BATTLE_SHOUT);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SALARIS_INTERCEPT_STUN:
                // if nobody is in melee range than try to use Intercept
                if (!m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_IN_MELEE_RANGE))
                {
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_INTERCEPT_STUN, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_IN_MELEE_RANGE | SELECT_FLAG_IN_LOS))
                        if (DoCastSpellIfCan(target, SPELL_INTERCEPT_STUN) == CAST_OK)
                            ResetCombatAction(action, 10000);
                }
                else
                    ResetCombatAction(action, 2000);
                break;
            case SALARIS_DISARM:
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_DISARM) == CAST_OK)
                    ResetCombatAction(action, 6000);
                break;
            case SALARIS_PIERCING_HOWL:
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_PIERCING_HOWL) == CAST_OK)
                    ResetCombatAction(action, 10000);
                break;
            case SALARIS_FRIGHTENING_SHOUT:
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FRIGHTENING_SHOUT) == CAST_OK)
                    ResetCombatAction(action, 18000);
                break;
            case SALARIS_HAMSTRING:
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_HAMSTRING) == CAST_OK)
                    ResetCombatAction(action, 4500);
                break;
            case SALARIS_MORTAL_STRIKE:
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE) == CAST_OK)
                    ResetCombatAction(action, 4500);
                break;
        }
        priestess_companion_commonAI::ExecuteAction(action);
    }
};

UnitAI* GetAI_npc_warlord_salaris(Creature* creature)
{
    return new npc_warlord_salarisAI(creature);
}

enum
{
    SPELL_AIMED_SHOT            = 44271,
    SPELL_SHOOT                 = 15620,
    SPELL_SHOOT_H               = 22907,
    SPELL_CONCUSSIVE_SHOT       = 27634,
    SPELL_MULTI_SHOT            = 31942,
    SPELL_MULTI_SHOT_H          = 44285,
    SPELL_WING_CLIP             = 44286,
    SPELL_FREEZING_TRAP         = 44136,

    NPC_SLIVER                  = 24552
};

/*######
## npc_garaxxas - Hunter
######*/

enum GaraxxasActions
{
    GARAXXAS_AIMED_SHOT = COMPANION_ACTION_MAX,
    GARAXXAS_SHOOT,
    GARAXXAS_CONCUSSIVE_SHOT,
    GARAXXAS_MULTI_SHOT,
    GARAXXAS_WING_CLIP,
    GARAXXAS_FREEZING_TRAP,
    GARAXXAS_ACTION_MAX,
};

struct npc_garaxxasAI : public priestess_companion_commonAI
{
    npc_garaxxasAI(Creature* creature) : priestess_companion_commonAI(creature, GARAXXAS_ACTION_MAX)
    {
        AddCombatAction(GARAXXAS_AIMED_SHOT, 6000u);
        AddCombatAction(GARAXXAS_SHOOT, 2500u);
        AddCombatAction(GARAXXAS_CONCUSSIVE_SHOT, 8000u);
        AddCombatAction(GARAXXAS_MULTI_SHOT, 10000u);
        AddCombatAction(GARAXXAS_WING_CLIP, 4000u);
        AddCombatAction(GARAXXAS_FREEZING_TRAP, 15000u);
        Reset();
    }

    void Reset() override
    {
        priestess_companion_commonAI::Reset();

        // Check if the pet was killed
        if (!GetClosestCreatureWithEntry(m_creature, NPC_SLIVER, 50.0f))
            m_creature->SummonCreature(NPC_SLIVER, 0, 0, 0, 0, TEMPSPAWN_CORPSE_DESPAWN, 0);
    }

    void ExecuteAction(uint32 action) override // TODO: Check ranged mode
    {
        switch (action)
        {
            case GARAXXAS_AIMED_SHOT:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_AIMED_SHOT) == CAST_OK)
                        ResetCombatAction(action, 6000);
                break;
            case GARAXXAS_SHOOT:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_SHOOT : SPELL_SHOOT_H) == CAST_OK)
                        ResetCombatAction(action, 2500);
                break;
            case GARAXXAS_CONCUSSIVE_SHOT:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_CONCUSSIVE_SHOT) == CAST_OK)
                        ResetCombatAction(action, 8000);
                break;
            case GARAXXAS_MULTI_SHOT:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_MULTI_SHOT : SPELL_MULTI_SHOT_H) == CAST_OK)
                        ResetCombatAction(action, 10000);
                break;
            case GARAXXAS_WING_CLIP: // melee
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_WING_CLIP) == CAST_OK)
                    ResetCombatAction(action, 4000);
                break;
            case GARAXXAS_FREEZING_TRAP: // melee
                if (DoCastSpellIfCan(nullptr, SPELL_FREEZING_TRAP) == CAST_OK)
                    ResetCombatAction(action, urand(15000, 30000));
                break;
        }
        priestess_companion_commonAI::ExecuteAction(action);
    }
};

UnitAI* GetAI_npc_garaxxas(Creature* creature)
{
    return new npc_garaxxasAI(creature);
}

enum
{
    SPELL_WAR_STOMP             = 46026,
    SPELL_PURGE                 = 27626,
    SPELL_LESSER_HEALING_WAVE   = 44256,
    SPELL_LESSER_HEALING_WAVE_H = 46181,
    SPELL_FROST_SHOCK           = 21401,
    SPELL_FROST_SHOCK_H         = 46180,
    SPELL_WINDFURY_TOTEM        = 27621,
    SPELL_FIRE_NOVA_TOTEM       = 44257,
    SPELL_EARTHBIND_TOTEM       = 15786
};

/*######
## npc_apoko - Shaman
######*/

enum ApokoActions
{
    APOKO_TOTEM = COMPANION_ACTION_MAX,
    APOKO_WAR_STOMP,
    APOKO_PURGE,
    APOKO_HEALING_WAVE,
    APOKO_FROST_SHOCK,
    APOKO_ACTION_MAX,
};

struct npc_apokoAI : public priestess_companion_commonAI
{
    npc_apokoAI(Creature* creature) : priestess_companion_commonAI(creature, APOKO_ACTION_MAX)
    {
        AddCombatAction(APOKO_TOTEM, 0u);
        AddCombatAction(APOKO_WAR_STOMP, 10000u);
        AddCombatAction(APOKO_PURGE, 8000u);
        AddCombatAction(APOKO_HEALING_WAVE, 5000u);
        AddCombatAction(APOKO_FROST_SHOCK, 7000u);
        Reset();
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case APOKO_TOTEM:
                // It's not very clear how exactly these spells should be cast
                switch (urand(0, 2))
                {
                    case 0: DoCastSpellIfCan(nullptr, SPELL_WINDFURY_TOTEM);  break;
                    case 1: DoCastSpellIfCan(nullptr, SPELL_FIRE_NOVA_TOTEM); break;
                    case 2: DoCastSpellIfCan(nullptr, SPELL_EARTHBIND_TOTEM); break;
                }
                ResetCombatAction(action, urand(2000, 6000));
                break;
            case APOKO_WAR_STOMP:
                if (DoCastSpellIfCan(nullptr, SPELL_WAR_STOMP) == CAST_OK)
                    ResetCombatAction(action, 10000);
                break;
            case APOKO_PURGE:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_PURGE) == CAST_OK)
                        ResetCombatAction(action, 15000);
                break;
            case APOKO_HEALING_WAVE:
                if (Unit* target = DoSelectLowestHpFriendly(50.0f))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_LESSER_HEALING_WAVE : SPELL_LESSER_HEALING_WAVE_H) == CAST_OK)
                        ResetCombatAction(action, 5000);
                break;
            case APOKO_FROST_SHOCK:
                if (DoCastSpellIfCan(m_creature->getVictim(), m_isRegularMode ? SPELL_FROST_SHOCK : SPELL_FROST_SHOCK_H) == CAST_OK)
                    ResetCombatAction(action, 7000);
                break;
        }
        priestess_companion_commonAI::ExecuteAction(action);
    }
};

UnitAI* GetAI_npc_apoko(Creature* creature)
{
    return new npc_apokoAI(creature);
}

enum
{
    SPELL_GOBLIN_DRAGON_GUN     = 44272,
    SPELL_GOBLIN_DRAGON_GUN_H   = 46185,
    SPELL_ROCKET_LAUNCH         = 44137,
    SPELL_ROCKET_LAUNCH_H       = 46187,
    SPELL_RECOMBOBULATE         = 44274,
    SPELL_HIGH_EXPLOSIVE_SHEEP  = 44276,
    SPELL_FEL_IRON_BOMB         = 46024,
    SPELL_FEL_IRON_BOMB_H       = 46184,
};

/*######
## npc_zelfan - Engineer
######*/

enum ZelfanActions
{
    ZELFAN_GOBLIN_DRAGON_GUN = COMPANION_ACTION_MAX,
    ZELFAN_ROCKET_LAUNCH,
    ZELFAN_RECOMBOBULATE,
    ZELFAN_HIGH_EXPLOSIVE_SHEEP,
    ZELFAN_FEL_IRON_BOMB,
    ZELFAN_ACTIONS_MAX,
};

struct npc_zelfanAI : public priestess_companion_commonAI
{
    npc_zelfanAI(Creature* creature) : priestess_companion_commonAI(creature, ZELFAN_ACTIONS_MAX)
    {
        AddCombatAction(ZELFAN_GOBLIN_DRAGON_GUN, 20000u);
        AddCombatAction(ZELFAN_ROCKET_LAUNCH, 7000u);
        AddCombatAction(ZELFAN_RECOMBOBULATE, 4000u);
        AddCombatAction(ZELFAN_HIGH_EXPLOSIVE_SHEEP, 1200u);
        AddCombatAction(ZELFAN_FEL_IRON_BOMB, 15000u);
        Reset();
    }

    void JustSummoned(Creature* summoned) override
    {
        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, nullptr, SELECT_FLAG_PLAYER))
            summoned->AI()->AttackStart(m_creature->getVictim());
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ZELFAN_GOBLIN_DRAGON_GUN:
                if (DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_GOBLIN_DRAGON_GUN : SPELL_GOBLIN_DRAGON_GUN_H) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 20000));
                break;
            case ZELFAN_ROCKET_LAUNCH:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_ROCKET_LAUNCH : SPELL_ROCKET_LAUNCH_H) == CAST_OK)
                        ResetCombatAction(action, 9000);
                break;
            case ZELFAN_RECOMBOBULATE:
            {
                // Note: this should be casted only on Polyformed targets
                Unit* target = nullptr;
                std::list<Creature*> lTempList = DoFindFriendlyCC(50.0f);

                if (!lTempList.empty())
                    target = *(lTempList.begin());
                else
                    target = DoSelectLowestHpFriendly(50.0f);

                if (target)
                    if (DoCastSpellIfCan(target, SPELL_RECOMBOBULATE) == CAST_OK)
                        ResetCombatAction(action, 2000);
                break;
            }
            case ZELFAN_HIGH_EXPLOSIVE_SHEEP:
                if (DoCastSpellIfCan(nullptr, SPELL_HIGH_EXPLOSIVE_SHEEP) == CAST_OK)
                    ResetCombatAction(action, 65000);
                break;
            case ZELFAN_FEL_IRON_BOMB:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_FEL_IRON_BOMB : SPELL_FEL_IRON_BOMB_H) == CAST_OK)
                        ResetCombatAction(action, 15000);
                break;
        }
        priestess_companion_commonAI::ExecuteAction(action);
    }
};

UnitAI* GetAI_npc_zelfan(Creature* creature)
{
    return new npc_zelfanAI(creature);
}

void AddSC_boss_priestess_delrissa()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_priestess_delrissa";
    pNewScript->GetAI = &GetAI_boss_priestess_delrissa;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_kagani_nightstrike";
    pNewScript->GetAI = &GetAI_npc_kagani_nightstrike;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ellris_duskhallow";
    pNewScript->GetAI = &GetAI_npc_ellris_duskhallow;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_eramas_brightblaze";
    pNewScript->GetAI = &GetAI_npc_eramas_brightblaze;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_yazzai";
    pNewScript->GetAI = &GetAI_npc_yazzai;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_warlord_salaris";
    pNewScript->GetAI = &GetAI_npc_warlord_salaris;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_garaxxas";
    pNewScript->GetAI = &GetAI_npc_garaxxas;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_apoko";
    pNewScript->GetAI = &GetAI_npc_apoko;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_zelfan";
    pNewScript->GetAI = &GetAI_npc_zelfan;
    pNewScript->RegisterSelf();
}

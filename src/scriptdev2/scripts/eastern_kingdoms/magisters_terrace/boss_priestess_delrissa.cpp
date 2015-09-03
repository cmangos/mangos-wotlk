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

#include "precompiled.h"
#include "magisters_terrace.h"

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

struct boss_priestess_delrissaAI : public ScriptedAI
{
    boss_priestess_delrissaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    std::vector<uint32> m_vuiLackeyEnties;

    uint32 m_uiHealTimer;
    uint32 m_uiRenewTimer;
    uint32 m_uiShieldTimer;
    uint32 m_uiSWPainTimer;
    uint32 m_uiDispelTimer;
    uint32 m_uiScreamTimer;
    uint32 m_uiMedallionTimer;
    uint8 m_uiPlayersKilled;

    void Reset() override
    {
        m_uiHealTimer       = 15000;
        m_uiRenewTimer      = 10000;
        m_uiShieldTimer     = 2000;
        m_uiSWPainTimer     = 5000;
        m_uiDispelTimer     = 7500;
        m_uiScreamTimer     = 9000;
        m_uiPlayersKilled   = 0;
        m_uiMedallionTimer  = urand(1000, 2000);

        DoInitializeCompanions();
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DELRISSA, FAIL);
    }

    void Aggro(Unit* pWho) override
    {
        if (pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_DELRISSA, IN_PROGRESS);
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
                m_creature->SummonCreature(m_vuiLackeyEnties[i], aLackeyLocations[i][0], aLackeyLocations[i][1], aLackeyLocations[i][2], aLackeyLocations[i][3], TEMPSUMMON_CORPSE_DESPAWN, 0);
        }
        // Resummon the killed adds
        else
        {
            if (!m_pInstance)
                return;

            for (uint8 i = 0; i < MAX_DELRISSA_ADDS; ++i)
            {
                // If we already have the creature on the map, then don't summon it
                if (m_pInstance->GetSingleCreatureFromStorage(m_vuiLackeyEnties[i], true))
                    continue;

                m_creature->SummonCreature(m_vuiLackeyEnties[i], aLackeyLocations[i][0], aLackeyLocations[i][1], aLackeyLocations[i][2], aLackeyLocations[i][3], TEMPSUMMON_CORPSE_DESPAWN, 0);
            }
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(aPlayerDeath[m_uiPlayersKilled], m_creature);
        ++m_uiPlayersKilled;

        // reset counter
        if (m_uiPlayersKilled == 5)
            m_uiPlayersKilled = 0;
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (!m_pInstance)
            return;

        // Remove lootable flag if the lackeys are not killed
        if (m_pInstance->GetData(TYPE_DELRISSA) == SPECIAL)
            m_pInstance->SetData(TYPE_DELRISSA, DONE);
        else
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiHealTimer < uiDiff)
        {
            if (Unit* pTarget = DoSelectLowestHpFriendly(50.0f))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_FLASH_HEAL) == CAST_OK)
                    m_uiHealTimer = urand(15000, 20000);
            }
        }
        else
            m_uiHealTimer -= uiDiff;

        if (m_uiRenewTimer < uiDiff)
        {
            if (Unit* pTarget = DoSelectLowestHpFriendly(50.0f))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_RENEW : SPELL_RENEW_H) == CAST_OK)
                    m_uiRenewTimer = urand(5000, 10000);
            }
        }
        else
            m_uiRenewTimer -= uiDiff;

        if (m_uiShieldTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SHIELD : SPELL_SHIELD_H) == CAST_OK)
                m_uiShieldTimer = urand(30000, 35000);
        }
        else
            m_uiShieldTimer -= uiDiff;

        if (m_uiDispelTimer < uiDiff)
        {
            Unit* pTarget = NULL;
            std::list<Creature*> lTempList = DoFindFriendlyCC(50.0f);

            if (!lTempList.empty())
                pTarget = *(lTempList.begin());
            else
                pTarget = DoSelectLowestHpFriendly(50.0f);

            if (pTarget)
            {
                if (DoCastSpellIfCan(pTarget, SPELL_DISPEL_MAGIC) == CAST_OK)
                    m_uiDispelTimer = urand(12000, 15000);
            }
        }
        else
            m_uiDispelTimer -= uiDiff;

        // Use the Medallion if CC - only on heroic. Not sure how many times they are allowed to use it.
        if (!m_bIsRegularMode && m_uiMedallionTimer)
        {
            if (m_creature->isFrozen() || m_creature->hasUnitState(UNIT_STAT_CAN_NOT_REACT))
            {
                if (m_uiMedallionTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_MEDALLION, CAST_TRIGGERED) == CAST_OK)
                        m_uiMedallionTimer = 0;
                }
                else
                    m_uiMedallionTimer -= uiDiff;
            }
        }

        if (m_uiSWPainTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SHADOW_WORD_PAIN : SPELL_SHADOW_WORD_PAIN_H) == CAST_OK)
                    m_uiSWPainTimer = 10000;
            }
        }
        else
            m_uiSWPainTimer -= uiDiff;

        if (m_uiScreamTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SCREAM) == CAST_OK)
                m_uiScreamTimer = urand(15000, 20000);
        }
        else
            m_uiScreamTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_priestess_delrissa(Creature* pCreature)
{
    return new boss_priestess_delrissaAI(pCreature);
}

/*######
## priestess_companion_common
######*/

struct priestess_companion_commonAI : public ScriptedAI
{
    priestess_companion_commonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiResetThreatTimer;
    uint32 m_uiMedallionTimer;
    bool m_bUsedPotion;

    void Reset() override
    {
        m_bUsedPotion = false;

        // These guys does not follow normal threat system rules
        // For later development, some alternative threat system should be made
        // We do not know what this system is based upon, but one theory is class (healers=high threat, dps=medium, etc)
        // We reset their threat frequently as an alternative until such a system exist
        m_uiResetThreatTimer = urand(5000, 15000);
        m_uiMedallionTimer   = urand(1000, 2000);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (!m_pInstance)
            return;

        if (Creature* pDelrissa = m_pInstance->GetSingleCreatureFromStorage(NPC_DELRISSA))
            pDelrissa->AI()->KilledUnit(pVictim);
    }

    // Return true to handle shared timers and MeleeAttack
    virtual bool UpdateCompanionAI(const uint32 /*uiDiff*/) { return true; }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Call specific virtual function
        if (!UpdateCompanionAI(uiDiff))
            return;

        if (!m_bUsedPotion && m_creature->GetHealthPercent() < 25.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_HEALING_POTION) == CAST_OK)
                m_bUsedPotion = true;
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

        // Use the Medallion if CC - only on heroic. Not sure how many times they are allowed to use it.
        if (!m_bIsRegularMode && m_uiMedallionTimer)
        {
            if (m_creature->isFrozen() || m_creature->hasUnitState(UNIT_STAT_CAN_NOT_REACT))
            {
                if (m_uiMedallionTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_MEDALLION, CAST_TRIGGERED) == CAST_OK)
                        m_uiMedallionTimer = 0;
                }
                else
                    m_uiMedallionTimer -= uiDiff;
            }
        }

        DoMeleeAttackIfReady();
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

struct npc_kagani_nightstrikeAI : public priestess_companion_commonAI
{
    npc_kagani_nightstrikeAI(Creature* pCreature) : priestess_companion_commonAI(pCreature) { Reset(); }

    uint32 m_uiGougeTimer;
    uint32 m_uiKickTimer;
    uint32 m_uiVanishTimer;
    uint32 m_uiEviscerateTimer;
    uint32 m_uiVanishEndTimer;

    void Reset() override
    {
        m_uiGougeTimer      = 5500;
        m_uiKickTimer       = 7000;
        m_uiVanishTimer     = 2000;
        m_uiEviscerateTimer = 6000;
        m_uiVanishEndTimer  = 0;

        priestess_companion_commonAI::Reset();
    }

    void EnterEvadeMode() override
    {
        if (m_uiVanishEndTimer)
            return;

        ScriptedAI::EnterEvadeMode();
    }

    bool UpdateCompanionAI(const uint32 uiDiff)
    {
        if (m_uiVanishEndTimer)
        {
            if (m_uiVanishEndTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_BACKSTAB, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_KIDNEY_SHOT, CAST_TRIGGERED);
                m_uiVanishEndTimer = 0;
            }
            else
                m_uiVanishEndTimer -= uiDiff;

            return false;
        }

        if (m_uiVanishTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_VANISH) == CAST_OK)
            {
                // Prefer targets with mana
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_POWER_MANA))
                {
                    DoResetThreat();
                    AttackStart(pTarget);
                }

                m_uiVanishTimer    = 30000;
                m_uiVanishEndTimer = 10000;
            }
        }
        else
            m_uiVanishTimer -= uiDiff;

        if (m_uiGougeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_GOUGE) == CAST_OK)
                m_uiGougeTimer = 5500;
        }
        else
            m_uiGougeTimer -= uiDiff;

        if (m_uiKickTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_KICK) == CAST_OK)
                m_uiKickTimer = 7000;
        }
        else
            m_uiKickTimer -= uiDiff;

        if (m_uiEviscerateTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_EVISCERATE : SPELL_EVISCERATE_H) == CAST_OK)
                m_uiEviscerateTimer = 4000;
        }
        else
            m_uiEviscerateTimer -= uiDiff;

        return true;
    }
};

CreatureAI* GetAI_npc_kagani_nightstrike(Creature* pCreature)
{
    return new npc_kagani_nightstrikeAI(pCreature);
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

struct npc_ellris_duskhallowAI : public priestess_companion_commonAI
{
    npc_ellris_duskhallowAI(Creature* pCreature) : priestess_companion_commonAI(pCreature) { Reset(); }

    uint32 m_uiImmolateTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiSeedCorruptionTimer;
    uint32 m_uiCurseAgonyTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiDeathCoilTimer;

    void Reset() override
    {
        m_uiImmolateTimer       = 6000;
        m_uiShadowBoltTimer     = 3000;
        m_uiSeedCorruptionTimer = 2000;
        m_uiCurseAgonyTimer     = 1000;
        m_uiFearTimer           = 10000;
        m_uiDeathCoilTimer      = 8000;

        priestess_companion_commonAI::Reset();

        // Check if we already have an imp summoned
        if (!GetClosestCreatureWithEntry(m_creature, NPC_FIZZLE, 50.0f))
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_IMP);
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho, 20.0f);
        }
    }

    bool UpdateCompanionAI(const uint32 uiDiff)
    {
        if (m_uiImmolateTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ?  SPELL_IMMOLATE : SPELL_IMMOLATE_H) == CAST_OK)
                    m_uiImmolateTimer = 6000;
            }
        }
        else
            m_uiImmolateTimer -= uiDiff;

        if (m_uiShadowBoltTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT_H) == CAST_OK)
                    m_uiShadowBoltTimer = 5000;
            }
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        if (m_uiSeedCorruptionTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_SEED_OF_CORRUPTION) == CAST_OK)
                    m_uiSeedCorruptionTimer = 10000;
            }
        }
        else
            m_uiSeedCorruptionTimer -= uiDiff;

        if (m_uiCurseAgonyTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_CURSE_OF_AGONY : SPELL_CURSE_OF_AGONY_H) == CAST_OK)
                    m_uiCurseAgonyTimer = 13000;
            }
        }
        else
            m_uiCurseAgonyTimer -= uiDiff;

        if (m_uiFearTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_FEAR) == CAST_OK)
                    m_uiFearTimer = 10000;
            }
        }
        else
            m_uiFearTimer -= uiDiff;

        if (m_uiDeathCoilTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_DEATH_COIL) == CAST_OK)
                    m_uiDeathCoilTimer = urand(8000, 13000);
            }
        }
        else
            m_uiDeathCoilTimer -= uiDiff;

        return true;
    }
};

CreatureAI* GetAI_npc_ellris_duskhallow(Creature* pCreature)
{
    return new npc_ellris_duskhallowAI(pCreature);
}

enum
{
    SPELL_KNOCKDOWN     = 11428,
    SPELL_KNOCKDOWN_H   = 46183,
    SPELL_SNAP_KICK     = 46182
};

/*######
## npc_eramas_brightblaze - Monk
######*/

struct npc_eramas_brightblazeAI : public priestess_companion_commonAI
{
    npc_eramas_brightblazeAI(Creature* pCreature) : priestess_companion_commonAI(pCreature) { Reset(); }

    uint32 m_uiKnockdownTimer;
    uint32 m_uiSnapKickTimer;

    void Reset() override
    {
        m_uiKnockdownTimer = 6000;
        m_uiSnapKickTimer  = 4500;

        priestess_companion_commonAI::Reset();
    }

    bool UpdateCompanionAI(const uint32 uiDiff)
    {
        if (m_uiKnockdownTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_KNOCKDOWN : SPELL_KNOCKDOWN_H) == CAST_OK)
                m_uiKnockdownTimer = 6000;
        }
        else
            m_uiKnockdownTimer -= uiDiff;

        if (m_uiSnapKickTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SNAP_KICK) == CAST_OK)
                m_uiSnapKickTimer  = 4500;
        }
        else
            m_uiSnapKickTimer -= uiDiff;

        return true;
    }
};

CreatureAI* GetAI_npc_eramas_brightblaze(Creature* pCreature)
{
    return new npc_eramas_brightblazeAI(pCreature);
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

struct npc_yazzaiAI : public priestess_companion_commonAI
{
    npc_yazzaiAI(Creature* pCreature) : priestess_companion_commonAI(pCreature) { Reset(); }

    bool m_bHasIceBlocked;

    uint32 m_uiPolymorphTimer;
    uint32 m_uiIceBlockTimer;
    uint32 m_uiWait_Timer;
    uint32 m_uiBlizzardTimer;
    uint32 m_uiIceLanceTimer;
    uint32 m_uiConeColdTimer;
    uint32 m_uiFrostboltTimer;
    uint32 m_uiBlinkTimer;

    void Reset() override
    {
        m_bHasIceBlocked    = false;

        m_uiPolymorphTimer  = 1000;
        m_uiIceBlockTimer   = 20000;
        m_uiWait_Timer      = 10000;
        m_uiBlizzardTimer   = 8000;
        m_uiIceLanceTimer   = 12000;
        m_uiConeColdTimer   = 10000;
        m_uiFrostboltTimer  = 3000;
        m_uiBlinkTimer      = 8000;

        priestess_companion_commonAI::Reset();
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho, 20.0f);
        }
    }

    bool UpdateCompanionAI(const uint32 uiDiff)
    {
        if (m_uiPolymorphTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_POLYMORPH) == CAST_OK)
                    m_uiPolymorphTimer = 20000;
            }
        }
        else
            m_uiPolymorphTimer -= uiDiff;

        if (m_creature->GetHealthPercent() < 35.0f && !m_bHasIceBlocked)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ICE_BLOCK) == CAST_OK)
                m_bHasIceBlocked = true;
        }

        if (m_uiBlizzardTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_BLIZZARD : SPELL_BLIZZARD_H) == CAST_OK)
                    m_uiBlizzardTimer = urand(8000, 15000);
            }
        }
        else
            m_uiBlizzardTimer -= uiDiff;

        if (m_uiIceLanceTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_ICE_LANCE : SPELL_ICE_LANCE_H) == CAST_OK)
                    m_uiIceLanceTimer = 12000;
            }
        }
        else
            m_uiIceLanceTimer -= uiDiff;

        if (m_uiConeColdTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_CONE_OF_COLD : SPELL_CONE_OF_COLD_H) == CAST_OK)
                m_uiConeColdTimer = 10000;
        }
        else
            m_uiConeColdTimer -= uiDiff;

        if (m_uiFrostboltTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_FROSTBOLT : SPELL_FROSTBOLT_H) == CAST_OK)
                    m_uiFrostboltTimer = 8000;
            }
        }
        else
            m_uiFrostboltTimer -= uiDiff;

        if (m_uiBlinkTimer < uiDiff)
        {
            // if anybody is in melee range than escape by blink
            if (m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_IN_MELEE_RANGE))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BLINK) == CAST_OK)
                    m_uiBlinkTimer = 8000;
            }
            else
                m_uiBlinkTimer = 2000;
        }
        else
            m_uiBlinkTimer -= uiDiff;

        return true;
    }
};

CreatureAI* GetAI_npc_yazzai(Creature* pCreature)
{
    return new npc_yazzaiAI(pCreature);
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

struct npc_warlord_salarisAI : public priestess_companion_commonAI
{
    npc_warlord_salarisAI(Creature* pCreature) : priestess_companion_commonAI(pCreature) { Reset(); }

    uint32 m_uiInterceptStunTimer;
    uint32 m_uiDisarmTimer;
    uint32 m_uiPiercingHowlTimer;
    uint32 m_uiFrighteningShoutTimer;
    uint32 m_uiHamstringTimer;
    uint32 m_uiMortalStrikeTimer;

    void Reset() override
    {
        m_uiInterceptStunTimer      = 500;
        m_uiDisarmTimer             = 6000;
        m_uiPiercingHowlTimer       = 10000;
        m_uiFrighteningShoutTimer   = 18000;
        m_uiHamstringTimer          = 4500;
        m_uiMortalStrikeTimer       = 8000;

        priestess_companion_commonAI::Reset();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_BATTLE_SHOUT);
    }

    bool UpdateCompanionAI(const uint32 uiDiff)
    {
        if (m_uiInterceptStunTimer < uiDiff)
        {
            // if nobody is in melee range than try to use Intercept
            if (!m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_IN_MELEE_RANGE))
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_INTERCEPT_STUN, SELECT_FLAG_NOT_IN_MELEE_RANGE | SELECT_FLAG_IN_LOS))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_INTERCEPT_STUN) == CAST_OK)
                        m_uiInterceptStunTimer = 10000;
                }
            }
            else
                m_uiInterceptStunTimer = 2000;
        }
        else
            m_uiInterceptStunTimer -= uiDiff;

        if (m_uiDisarmTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_DISARM) == CAST_OK)
                m_uiDisarmTimer = 6000;
        }
        else
            m_uiDisarmTimer -= uiDiff;

        if (m_uiHamstringTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_HAMSTRING) == CAST_OK)
                m_uiHamstringTimer = 4500;
        }
        else
            m_uiHamstringTimer -= uiDiff;

        if (m_uiMortalStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE) == CAST_OK)
                m_uiMortalStrikeTimer = 4500;
        }
        else
            m_uiMortalStrikeTimer -= uiDiff;

        if (m_uiPiercingHowlTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_PIERCING_HOWL) == CAST_OK)
                m_uiPiercingHowlTimer = 10000;
        }
        else
            m_uiPiercingHowlTimer -= uiDiff;

        if (m_uiFrighteningShoutTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FRIGHTENING_SHOUT) == CAST_OK)
                m_uiFrighteningShoutTimer = 18000;
        }
        else
            m_uiFrighteningShoutTimer -= uiDiff;

        return true;
    }
};

CreatureAI* GetAI_npc_warlord_salaris(Creature* pCreature)
{
    return new npc_warlord_salarisAI(pCreature);
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

struct npc_garaxxasAI : public priestess_companion_commonAI
{
    npc_garaxxasAI(Creature* pCreature) : priestess_companion_commonAI(pCreature) { Reset(); }

    uint32 m_uiAimedShotTimer;
    uint32 m_uiShootTimer;
    uint32 m_uiConcussiveShotTimer;
    uint32 m_uiMultiShotTimer;
    uint32 m_uiWingClipTimer;
    uint32 m_uiFreezingTrapTimer;

    void Reset() override
    {
        m_uiAimedShotTimer      = 6000;
        m_uiShootTimer          = 2500;
        m_uiConcussiveShotTimer = 8000;
        m_uiMultiShotTimer      = 10000;
        m_uiWingClipTimer       = 4000;
        m_uiFreezingTrapTimer   = 15000;

        priestess_companion_commonAI::Reset();

        // Check if the pet was killed
        if (!GetClosestCreatureWithEntry(m_creature, NPC_SLIVER, 50.0f))
            m_creature->SummonCreature(NPC_SLIVER, 0, 0, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho, 20.0f);
        }
    }

    bool UpdateCompanionAI(const uint32 uiDiff)
    {
        if (m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
        {
            if (m_uiWingClipTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_WING_CLIP) == CAST_OK)
                    m_uiWingClipTimer = 4000;
            }
            else
                m_uiWingClipTimer -= uiDiff;

            if (m_uiFreezingTrapTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FREEZING_TRAP) == CAST_OK)
                    m_uiFreezingTrapTimer = urand(15000, 30000);
            }
            else
                m_uiFreezingTrapTimer -= uiDiff;
        }
        else
        {
            if (m_uiConcussiveShotTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_CONCUSSIVE_SHOT) == CAST_OK)
                        m_uiConcussiveShotTimer = 8000;
                }
            }
            else
                m_uiConcussiveShotTimer -= uiDiff;

            if (m_uiMultiShotTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_MULTI_SHOT : SPELL_MULTI_SHOT_H) == CAST_OK)
                        m_uiMultiShotTimer = 10000;
                }
            }
            else
                m_uiMultiShotTimer -= uiDiff;

            if (m_uiAimedShotTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_AIMED_SHOT) == CAST_OK)
                        m_uiAimedShotTimer = 6000;
                }
            }
            else
                m_uiAimedShotTimer -= uiDiff;

            if (m_uiShootTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SHOOT : SPELL_SHOOT_H) == CAST_OK)
                        m_uiShootTimer = 2500;
                }
            }
            else
                m_uiShootTimer -= uiDiff;
        }

        return true;
    }
};

CreatureAI* GetAI_npc_garaxxas(Creature* pCreature)
{
    return new npc_garaxxasAI(pCreature);
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

struct npc_apokoAI : public priestess_companion_commonAI
{
    npc_apokoAI(Creature* pCreature) : priestess_companion_commonAI(pCreature) { Reset(); }

    uint32 m_uiTotemTimer;
    uint32 m_uiWarStompTimer;
    uint32 m_uiPurgeTimer;
    uint32 m_uiHealingWaveTimer;
    uint32 m_uiFrostShockTimer;

    void Reset() override
    {
        m_uiTotemTimer       = 0;
        m_uiWarStompTimer    = 10000;
        m_uiPurgeTimer       = 8000;
        m_uiHealingWaveTimer = 5000;
        m_uiFrostShockTimer  = 7000;

        priestess_companion_commonAI::Reset();
    }

    bool UpdateCompanionAI(const uint32 uiDiff)
    {
        if (m_uiTotemTimer < uiDiff)
        {
            // It's not very clear how exactly these spells should be cast
            switch (urand(0, 2))
            {
                case 0: DoCastSpellIfCan(m_creature, SPELL_WINDFURY_TOTEM);  break;
                case 1: DoCastSpellIfCan(m_creature, SPELL_FIRE_NOVA_TOTEM); break;
                case 2: DoCastSpellIfCan(m_creature, SPELL_EARTHBIND_TOTEM); break;
            }
            m_uiTotemTimer = urand(2000, 6000);
        }
        else
            m_uiTotemTimer -= uiDiff;

        if (m_uiWarStompTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WAR_STOMP) == CAST_OK)
                m_uiWarStompTimer = 10000;
        }
        else
            m_uiWarStompTimer -= uiDiff;

        if (m_uiPurgeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_PURGE) == CAST_OK)
                    m_uiPurgeTimer = 15000;
            }
        }
        else
            m_uiPurgeTimer -= uiDiff;

        if (m_uiFrostShockTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FROST_SHOCK : SPELL_FROST_SHOCK_H) == CAST_OK)
                m_uiFrostShockTimer = 7000;
        }
        else
            m_uiFrostShockTimer -= uiDiff;

        if (m_uiHealingWaveTimer < uiDiff)
        {
            if (Unit* pTarget = DoSelectLowestHpFriendly(50.0f))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_LESSER_HEALING_WAVE : SPELL_LESSER_HEALING_WAVE_H) == CAST_OK)
                    m_uiHealingWaveTimer = 5000;
            }
        }
        else
            m_uiHealingWaveTimer -= uiDiff;

        return true;
    }
};

CreatureAI* GetAI_npc_apoko(Creature* pCreature)
{
    return new npc_apokoAI(pCreature);
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

struct npc_zelfanAI : public priestess_companion_commonAI
{
    npc_zelfanAI(Creature* pCreature) : priestess_companion_commonAI(pCreature) { Reset(); }

    uint32 m_uiGoblinDragonGunTimer;
    uint32 m_uiRocketLaunchTimer;
    uint32 m_uiRecombobulateTimer;
    uint32 m_uiHighExplosiveSheepTimer;
    uint32 m_uiFelIronBombTimer;

    void Reset() override
    {
        m_uiGoblinDragonGunTimer    = 20000;
        m_uiRocketLaunchTimer       = 7000;
        m_uiRecombobulateTimer      = 4000;
        m_uiHighExplosiveSheepTimer = 10000;
        m_uiFelIronBombTimer        = 15000;

        priestess_companion_commonAI::Reset();
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (m_creature->getVictim())
            pSummoned->AI()->AttackStart(m_creature->getVictim());
    }

    bool UpdateCompanionAI(const uint32 uiDiff)
    {
        if (m_uiGoblinDragonGunTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_GOBLIN_DRAGON_GUN : SPELL_GOBLIN_DRAGON_GUN_H) == CAST_OK)
                m_uiGoblinDragonGunTimer = urand(10000, 20000);
        }
        else
            m_uiGoblinDragonGunTimer -= uiDiff;

        if (m_uiRocketLaunchTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_ROCKET_LAUNCH : SPELL_ROCKET_LAUNCH_H) == CAST_OK)
                    m_uiRocketLaunchTimer = 9000;
            }
        }
        else
            m_uiRocketLaunchTimer -= uiDiff;

        if (m_uiFelIronBombTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_FEL_IRON_BOMB : SPELL_FEL_IRON_BOMB_H) == CAST_OK)
                    m_uiFelIronBombTimer = 15000;
            }
        }
        else
            m_uiFelIronBombTimer -= uiDiff;

        if (m_uiRecombobulateTimer < uiDiff)
        {
            // Note: this should be casted only on Polyformed targets
            Unit* pTarget = NULL;
            std::list<Creature*> lTempList = DoFindFriendlyCC(50.0f);

            if (!lTempList.empty())
                pTarget = *(lTempList.begin());
            else
                pTarget = DoSelectLowestHpFriendly(50.0f);

            if (pTarget)
            {
                if (DoCastSpellIfCan(pTarget, SPELL_RECOMBOBULATE) == CAST_OK)
                    m_uiRecombobulateTimer = 2000;
            }
        }
        else
            m_uiRecombobulateTimer -= uiDiff;

        if (m_uiHighExplosiveSheepTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_HIGH_EXPLOSIVE_SHEEP) == CAST_OK)
                m_uiHighExplosiveSheepTimer = 65000;
        }
        else
            m_uiHighExplosiveSheepTimer -= uiDiff;

        return true;
    }
};

CreatureAI* GetAI_npc_zelfan(Creature* pCreature)
{
    return new npc_zelfanAI(pCreature);
}

void AddSC_boss_priestess_delrissa()
{
    Script* pNewScript;

    pNewScript = new Script;
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

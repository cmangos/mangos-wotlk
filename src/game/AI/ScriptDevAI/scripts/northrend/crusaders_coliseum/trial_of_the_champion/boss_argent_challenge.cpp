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
SDName: argent_challenge
SD%Complete: 90
SDComment: Achievement NYI.
SDCategory: Crusader Coliseum, Trial of the Champion
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "trial_of_the_champion.h"

enum
{
    FACTION_CHAMPION_FRIENDLY           = 35,
};

/*######
## argent_companion_common
######*/

struct argent_champion_commonAI : public ScriptedAI
{
    argent_champion_commonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_trial_of_the_champion*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bDefeated = false;
        Reset();
    }

    instance_trial_of_the_champion* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bDefeated;

    void Reset() override { }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ARGENT_CHAMPION, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_ARGENT_CHAMPION) != DONE)
            m_pInstance->SetData(TYPE_ARGENT_CHAMPION, FAIL);
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (m_bDefeated)
                return;

            if (m_pInstance)
                m_pInstance->SetData(TYPE_ARGENT_CHAMPION, DONE);

            // Handle event completion
            DoHandleEventEnd();

            m_creature->SetFactionTemporary(FACTION_CHAMPION_FRIENDLY, TEMPFACTION_NONE);
            EnterEvadeMode();

            m_bDefeated = true;
        }
    }

    // Function that handles personalized event completion
    virtual void DoHandleEventEnd() {}
};

enum
{
    SAY_EADRIC_AGGRO                = -1650052,
    SAY_EADRIC_HAMMER               = -1650053,
    SAY_EADRIC_KILL_1               = -1650054,
    SAY_EADRIC_KILL_2               = -1650055,
    SAY_EADRIC_DEFEAT               = -1650056,
    EMOTE_EADRIC_RADIANCE           = -1650057,
    EMOTE_EADRIC_HAMMER             = -1650058,

    SPELL_KILL_CREDIT_EADRIC        = 68575,
    SPELL_EADRIC_ACHIEVEMENT        = 68197,                        // required for achiev 3803

    SPELL_HAMMER_OF_JUSTICE         = 66863,
    SPELL_HAMMER_OF_RIGHTEOUS       = 66867,
    SPELL_RADIANCE                  = 66935,
    SPELL_VENGEANCE                 = 66865,
};

/*######
## boss_eadric
######*/

struct boss_eadricAI : public argent_champion_commonAI
{
    boss_eadricAI(Creature* pCreature) : argent_champion_commonAI(pCreature) { Reset(); }

    uint32 m_uiHammerTimer;
    uint32 m_uiRadianceTimer;

    void Reset() override
    {
        argent_champion_commonAI::Reset();

        m_uiHammerTimer = urand(30000, 35000);
        m_uiRadianceTimer = urand(10000, 15000);
    }

    void Aggro(Unit* pWho) override
    {
        DoScriptText(SAY_EADRIC_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_VENGEANCE, CAST_TRIGGERED);

        argent_champion_commonAI::Aggro(pWho);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_EADRIC_KILL_1 : SAY_EADRIC_KILL_2, m_creature);
    }

    void DoHandleEventEnd()
    {
        DoScriptText(SAY_EADRIC_DEFEAT, m_creature);

        // ToDo: implement the mechanics for this achiev
        //DoCastSpellIfCan(m_creature, SPELL_EADRIC_ACHIEVEMENT, CAST_TRIGGERED);
        m_creature->CastSpell(m_creature, SPELL_KILL_CREDIT_EADRIC, TRIGGERED_OLD_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiHammerTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_HAMMER_OF_RIGHTEOUS) == CAST_OK)
                {
                    DoCastSpellIfCan(m_creature, SPELL_HAMMER_OF_JUSTICE, CAST_TRIGGERED);

                    DoScriptText(EMOTE_EADRIC_HAMMER, m_creature, pTarget);
                    m_uiHammerTimer = 35000;
                }
            }
        }
        else
            m_uiHammerTimer -= uiDiff;

        if (m_uiRadianceTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_RADIANCE) == CAST_OK)
            {
                DoScriptText(EMOTE_EADRIC_RADIANCE, m_creature);
                m_uiRadianceTimer = urand(30000, 35000);
            }
        }
        else
            m_uiRadianceTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_eadric(Creature* pCreature)
{
    return new boss_eadricAI(pCreature);
}

enum
{
    SAY_PALETRESS_AGGRO             = -1650059,
    SAY_PALETRESS_MEMORY            = -1650060,
    SAY_PALETRESS_MEMORY_DIES       = -1650061,
    SAY_PALETRESS_KILL_1            = -1650062,
    SAY_PALETRESS_KILL_2            = -1650063,
    SAY_PALETRESS_DEFEAT            = -1650064,

    SPELL_KILL_CREDIT_PALETRESS     = 68574,
    SPELL_CONFESSOR_ACHIEVEMENT     = 68206,                        // required for achiev 3802

    SPELL_CONFESS                   = 66547,
    SPELL_CONFESS_AURA              = 66680,
    SPELL_SUMMON_MEMORY             = 66545,
    SPELL_REFLECTIVE_SHIELD         = 66515,

    SPELL_HOLY_FIRE                 = 66538,
    SPELL_HOLY_NOVA                 = 66546,
    SPELL_HOLY_SMITE                = 66536,
    SPELL_RENEW                     = 66537,

    SPELL_MEMORY_SPAWN_EFFECT       = 66675,
    SPELL_SHADOWFORM                = 41408,
};

/*######
## boss_paletress
######*/

struct boss_paletressAI : public argent_champion_commonAI
{
    boss_paletressAI(Creature* pCreature) : argent_champion_commonAI(pCreature) { Reset(); }

    uint32 m_uiHolySmiteTimer;
    uint32 m_uiHolyFireTimer;
    uint32 m_uiHolyNovaTimer;
    uint32 m_uiRenewTimer;

    bool m_bSummonedMemory;

    void Reset() override
    {
        argent_champion_commonAI::Reset();

        m_uiHolySmiteTimer  = 0;
        m_uiHolyFireTimer   = urand(7000, 12000);
        m_uiHolyNovaTimer   = urand(20000, 25000);
        m_uiRenewTimer      = urand(5000, 9000);

        m_bSummonedMemory   = false;
    }

    void Aggro(Unit* pWho) override
    {
        DoScriptText(SAY_PALETRESS_AGGRO, m_creature);

        argent_champion_commonAI::Aggro(pWho);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_PALETRESS_KILL_1 : SAY_PALETRESS_KILL_2, m_creature);
    }

    void DoHandleEventEnd()
    {
        DoScriptText(SAY_PALETRESS_DEFEAT, m_creature);

        m_creature->CastSpell(m_creature, SPELL_KILL_CREDIT_PALETRESS, TRIGGERED_OLD_TRIGGERED);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->CastSpell(pSummoned, SPELL_SHADOWFORM, TRIGGERED_OLD_TRIGGERED);
        pSummoned->CastSpell(pSummoned, SPELL_MEMORY_SPAWN_EFFECT, TRIGGERED_OLD_TRIGGERED);
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        DoScriptText(SAY_PALETRESS_MEMORY_DIES, m_creature);
        pSummoned->CastSpell(pSummoned, SPELL_CONFESSOR_ACHIEVEMENT, TRIGGERED_OLD_TRIGGERED);
        m_creature->RemoveAurasDueToSpell(SPELL_REFLECTIVE_SHIELD);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_bSummonedMemory && m_creature->GetHealthPercent() <= 25.0f)
        {
            DoScriptText(SAY_PALETRESS_MEMORY, m_creature);

            DoCastSpellIfCan(m_creature, SPELL_CONFESS, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_CONFESS_AURA, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_MEMORY, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_REFLECTIVE_SHIELD, CAST_TRIGGERED);
            m_bSummonedMemory = true;
        }

        if (m_uiHolySmiteTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_HOLY_SMITE))
                    m_uiHolySmiteTimer = urand(1000, 2000);
            }
        }
        else
            m_uiHolySmiteTimer -= uiDiff;

        if (m_uiHolyFireTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_HOLY_FIRE))
                    m_uiHolyFireTimer = 25000;
            }
        }
        else
            m_uiHolyFireTimer -= uiDiff;

        if (m_uiHolyNovaTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_HOLY_NOVA))
                m_uiHolyNovaTimer = urand(30000, 40000);
        }
        else
            m_uiHolyNovaTimer -= uiDiff;

        if (m_uiRenewTimer < uiDiff)
        {
            if (Unit* pTarget = DoSelectLowestHpFriendly(60.0f))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_RENEW) == CAST_OK)
                    m_uiRenewTimer = 20000;
            }
        }
        else
            m_uiRenewTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_paletress(Creature* pCreature)
{
    return new boss_paletressAI(pCreature);
}

void AddSC_boss_argent_challenge()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_eadric";
    pNewScript->GetAI = &GetAI_boss_eadric;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_paletress";
    pNewScript->GetAI = &GetAI_boss_paletress;
    pNewScript->RegisterSelf();
}

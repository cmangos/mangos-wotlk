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
SDName: Boss_Exarch_Maladaar
SD%Complete: 95
SDComment: Most of event implemented, possibly make some better code for switching his dark side in to better "images" of player.
SDCategory: Auchindoun, Auchenai Crypts
EndScriptData */

/* ContentData
mob_stolen_soul
boss_exarch_maladaar
mob_avatar_of_martyred
EndContentData */

#include "precompiled.h"

enum
{
    SPELL_STOLEN_SOUL_DISPEL = 33326,

    SPELL_MOONFIRE          = 37328,
    SPELL_FIREBALL          = 37329,
    SPELL_MIND_FLAY         = 37330,
    SPELL_HEMORRHAGE        = 37331,
    SPELL_FROSTSHOCK        = 37332,
    SPELL_CURSE_OF_AGONY    = 37334,
    SPELL_MORTAL_STRIKE     = 37335,
    SPELL_FREEZING_TRAP     = 37368,
    SPELL_HAMMER_OF_JUSTICE = 37369,
    SPELL_PLAGUE_STRIKE     = 58339
};

struct mob_stolen_soulAI : public ScriptedAI
{
    mob_stolen_soulAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint8 m_uiStolenClass;
    uint32 m_uiSpellTimer;

    ObjectGuid m_targetGuid;

    void Reset() override
    {
        m_uiSpellTimer = 1000;
    }

    void SetSoulInfo(Unit* pTarget)
    {
        m_uiStolenClass = pTarget->getClass();
        m_targetGuid = pTarget->GetObjectGuid();
        m_creature->SetDisplayId(pTarget->GetDisplayId());
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (Unit* pTarget = m_creature->GetMap()->GetUnit(m_targetGuid))
            DoCastSpellIfCan(pTarget, SPELL_STOLEN_SOUL_DISPEL, CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpellTimer < uiDiff)
        {
            switch (m_uiStolenClass)
            {
                case CLASS_WARRIOR:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
                    m_uiSpellTimer = 6000;
                    break;
                case CLASS_PALADIN:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_HAMMER_OF_JUSTICE);
                    m_uiSpellTimer = 6000;
                    break;
                case CLASS_HUNTER:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_FREEZING_TRAP);
                    m_uiSpellTimer = 20000;
                    break;
                case CLASS_ROGUE:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_HEMORRHAGE);
                    m_uiSpellTimer = 10000;
                    break;
                case CLASS_PRIEST:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIND_FLAY);
                    m_uiSpellTimer = 5000;
                    break;
                case CLASS_SHAMAN:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROSTSHOCK);
                    m_uiSpellTimer = 8000;
                    break;
                case CLASS_MAGE:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIREBALL);
                    m_uiSpellTimer = 5000;
                    break;
                case CLASS_WARLOCK:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_AGONY);
                    m_uiSpellTimer = 20000;
                    break;
                case CLASS_DRUID:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_MOONFIRE);
                    m_uiSpellTimer = 10000;
                    break;
                case CLASS_DEATH_KNIGHT:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_PLAGUE_STRIKE);
                    m_uiSpellTimer = 10000;
                    break;
            }
        }
        else
            m_uiSpellTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_stolen_soul(Creature* pCreature)
{
    return new mob_stolen_soulAI(pCreature);
}

enum
{
    SAY_INTRO                = -1558000,
    SAY_SUMMON               = -1558001,
    SAY_AGGRO_1              = -1558002,
    SAY_AGGRO_2              = -1558003,
    SAY_AGGRO_3              = -1558004,
    SAY_ROAR                 = -1558005,
    SAY_SOUL_CLEAVE          = -1558006,
    SAY_SLAY_1               = -1558007,
    SAY_SLAY_2               = -1558008,
    SAY_DEATH                = -1558009,

    SPELL_RIBBON_OF_SOULS    = 32422,
    SPELL_SOUL_SCREAM        = 32421,
    SPELL_STOLEN_SOUL        = 32346,
    SPELL_STOLEN_SOUL_VISUAL = 32395,
    SPELL_SUMMON_AVATAR      = 32424,

    NPC_STOLEN_SOUL          = 18441,
    NPC_DORE                 = 19412
};

struct boss_exarch_maladaarAI : public ScriptedAI
{
    boss_exarch_maladaarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bHasTaunted = false;
        Reset();
    }

    ObjectGuid m_targetGuid;

    uint32 m_uiFearTimer;
    uint32 m_uiRibbonOfSoulsTimer;
    uint32 m_uiStolenSoulTimer;

    bool m_bHasTaunted;
    bool m_bHasSummonedAvatar;

    void Reset() override
    {
        m_targetGuid.Clear();

        m_uiFearTimer          = urand(11000, 29000);
        m_uiRibbonOfSoulsTimer = urand(4000, 8000);
        m_uiStolenSoulTimer    = urand(19000, 31000);

        m_bHasSummonedAvatar = false;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasTaunted && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 150.0f) && m_creature->IsWithinLOSInMap(pWho))
        {
            DoScriptText(SAY_INTRO, m_creature);
            m_bHasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO_2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO_3, m_creature); break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_STOLEN_SOUL)
        {
            // SPELL_STOLEN_SOUL_VISUAL has shapeshift effect, but not implemented feature in mangos for this spell.
            pSummoned->CastSpell(pSummoned, SPELL_STOLEN_SOUL_VISUAL, false);

            if (Player* pTarget = m_creature->GetMap()->GetPlayer(m_targetGuid))
            {
                if (mob_stolen_soulAI* pSoulAI = dynamic_cast<mob_stolen_soulAI*>(pSummoned->AI()))
                    pSoulAI->SetSoulInfo(pTarget);

                pSummoned->AI()->AttackStart(pTarget);
            }
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        if (urand(0, 1))
            return;

        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        // When Exarch Maladaar is defeated D'ore appear.
        DoSpawnCreature(NPC_DORE, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 600000);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_STOLEN_SOUL && pTarget->GetTypeId() == TYPEID_PLAYER)
            DoSpawnCreature(NPC_STOLEN_SOUL, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 10000);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bHasSummonedAvatar && m_creature->GetHealthPercent() < 25.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_AVATAR) == CAST_OK)
            {
                DoScriptText(SAY_SUMMON, m_creature);
                m_bHasSummonedAvatar = true;
                m_uiStolenSoulTimer = urand(15000, 30000);
            }
        }

        if (m_uiStolenSoulTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_STOLEN_SOUL, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_STOLEN_SOUL) == CAST_OK)
                {
                    if (urand(0, 1))
                        DoScriptText(urand(0, 1) ? SAY_ROAR : SAY_SOUL_CLEAVE, m_creature);

                    m_targetGuid = pTarget->GetObjectGuid();

                    m_uiStolenSoulTimer = urand(35000, 67000);
                }
            }
        }
        else
            m_uiStolenSoulTimer -= uiDiff;

        if (m_uiRibbonOfSoulsTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_RIBBON_OF_SOULS) == CAST_OK)
                    m_uiRibbonOfSoulsTimer = urand(4000, 18000);
            }
        }
        else
            m_uiRibbonOfSoulsTimer -= uiDiff;

        if (m_uiFearTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SOUL_SCREAM) == CAST_OK)
                m_uiFearTimer = urand(13000, 30000);
        }
        else
            m_uiFearTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_exarch_maladaar(Creature* pCreature)
{
    return new boss_exarch_maladaarAI(pCreature);
}

void AddSC_boss_exarch_maladaar()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_exarch_maladaar";
    pNewScript->GetAI = &GetAI_boss_exarch_maladaar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_stolen_soul";
    pNewScript->GetAI = &GetAI_mob_stolen_soul;
    pNewScript->RegisterSelf();
}

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
SDName: Boss_Magtheridon
SD%Complete: 80
SDComment: Phase 3 transition requires additional research. The Manticron cubes require additional core support. Timers need to be revised.
SDCategory: Hellfire Citadel, Magtheridon's lair
EndScriptData */

#include "precompiled.h"
#include "magtheridons_lair.h"

enum
{
    // yells
    SAY_AGGRO_1                 = -1544006,
    SAY_AGGRO_2                 = -1544007,
    SAY_BANISH                  = -1544008,
    SAY_CHAMBER_DESTROY         = -1544009,
    SAY_PLAYER_KILLED           = -1544010,
    SAY_DEATH                   = -1544011,

    EMOTE_GENERIC_ENRAGED       = -1000003,
    EMOTE_BLASTNOVA             = -1544013,
    EMOTE_FREED                 = -1544015,

    // Maghteridon spells
    SPELL_SHADOW_CAGE_DUMMY     = 30205,                    // dummy aura - in creature_template_addon
    SPELL_BLASTNOVA             = 30616,
    SPELL_CLEAVE                = 30619,
    // SPELL_QUAKE               = 30657,                   // spell may be related but probably used in the recent versions of the script
    // SPELL_QUAKE_TRIGGER       = 30576,                   // spell removed from DBC - triggers 30571
    SPELL_QUAKE_KNOCKBACK       = 30571,
    SPELL_BLAZE                 = 30541,                    // triggers 30542
    SPELL_BERSERK               = 27680,

    // phase 3 spells
    SPELL_CAMERA_SHAKE          = 36455,
    SPELL_DEBRIS_KNOCKDOWN      = 36449,
    SPELL_QUAKE_EFFECT          = 30572,                    // sets the debris during phase 3 - triggers 30632
    SPELL_DEBRIS_DAMAGE         = 30631,
    SPELL_DEBRIS_VISUAL         = 30632,

    // Cube spells
    SPELL_SHADOW_CAGE           = 30168,
    SPELL_SHADOW_GRASP_VISUAL   = 30166,
    SPELL_SHADOW_GRASP          = 30410,
    SPELL_MIND_EXHAUSTION       = 44032,

    // Hellfire channeler spells
    SPELL_SHADOW_GRASP_DUMMY    = 30207,                    // dummy spell - cast on OOC timer
    SPELL_SHADOW_BOLT_VOLLEY    = 30510,
    SPELL_DARK_MENDING          = 30528,
    SPELL_FEAR                  = 30530,                    // 39176
    SPELL_BURNING_ABYSSAL       = 30511,
    SPELL_SOUL_TRANSFER         = 30531,

    // Abyss spells
    SPELL_FIRE_BLAST            = 37110,

    // summons
    // NPC_MAGS_ROOM             = 17516,
    NPC_BURNING_ABYSS           = 17454,
    NPC_RAID_TRIGGER            = 17376,

    MAX_QUAKE_COUNT             = 7,
};

/*######
## boss_magtheridon
######*/

struct boss_magtheridonAI : public ScriptedAI
{
    boss_magtheridonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiBerserkTimer;
    uint32 m_uiQuakeTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiBlastNovaTimer;
    uint32 m_uiBlazeTimer;
    uint32 m_uiDebrisTimer;
    uint32 m_uiTransitionTimer;
    uint8 m_uiTransitionCount;
    uint8 m_uiQuakeCount;

    bool m_bIsPhase3;

    void Reset() override
    {
        m_uiBerserkTimer    = 20 * MINUTE * IN_MILLISECONDS;
        m_uiQuakeTimer      = 30000;
        m_uiBlazeTimer      = urand(10000, 15000);
        m_uiBlastNovaTimer  = 60000;
        m_uiCleaveTimer     = 15000;
        m_uiTransitionTimer = 0;
        m_uiTransitionCount = 0;
        m_uiDebrisTimer     = urand(20000, 30000);

        m_bIsPhase3         = false;

        SetCombatMovement(true);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(EMOTE_FREED, m_creature);
        DoScriptText(urand(0, 1) ? SAY_AGGRO_1 : SAY_AGGRO_2, m_creature);

        m_creature->RemoveAurasDueToSpell(SPELL_SHADOW_CAGE_DUMMY);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_PLAYER_KILLED, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAGTHERIDON_EVENT, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAGTHERIDON_EVENT, FAIL);
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        // When banished by the cubes
        if (pSpell->Id == SPELL_SHADOW_CAGE)
            DoScriptText(SAY_BANISH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(EMOTE_GENERIC_ENRAGED, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        // Transition to phase 3
        if (m_uiTransitionTimer)
        {
            if (m_uiTransitionTimer <= uiDiff)
            {
                switch (m_uiTransitionCount)
                {
                    case 0:
                        // Shake the room
                        if (DoCastSpellIfCan(m_creature, SPELL_CAMERA_SHAKE) == CAST_OK)
                        {
                            if (m_pInstance)
                                m_pInstance->SetData(TYPE_MAGTHERIDON_EVENT, SPECIAL);

                            m_uiTransitionTimer = 8000;
                        }
                        break;
                    case 1:
                        if (DoCastSpellIfCan(m_creature, SPELL_DEBRIS_KNOCKDOWN) == CAST_OK)
                            m_uiTransitionTimer = 0;
                        break;
                }

                ++m_uiTransitionCount;
            }
            else
                m_uiTransitionTimer -= uiDiff;

            // Workaround for missing spell: no other spells during transition
            if (!m_uiTransitionCount)
                return;
        }

        if (m_uiQuakeTimer < uiDiff)
        {
            // Workaround for missing spell
            // Note: this won't really stun the boss, but it won't allow him to use other spells
            if (!m_uiQuakeCount)
            {
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveIdle();
            }

            if (m_uiQuakeCount < MAX_QUAKE_COUNT)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_QUAKE_KNOCKBACK) == CAST_OK)
                {
                    m_uiQuakeTimer = 1000;
                    ++m_uiQuakeCount;
                }
            }
            else
            {
                SetCombatMovement(true);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

                m_uiQuakeTimer = 43000;
                m_uiQuakeCount = 0;
            }
        }
        else
            m_uiQuakeTimer -= uiDiff;

        // don't use other spells during quake
        if (m_uiQuakeCount)
            return;

        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = 10000;
        }
        else
            m_uiCleaveTimer -= uiDiff;

        if (m_uiBlastNovaTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLASTNOVA) == CAST_OK)
            {
                DoScriptText(EMOTE_BLASTNOVA, m_creature);
                m_uiBlastNovaTimer = 60000;
            }
        }
        else
            m_uiBlastNovaTimer -= uiDiff;

        if (m_uiBlazeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLAZE) == CAST_OK)
                m_uiBlazeTimer = urand(10000, 15000);
        }
        else
            m_uiBlazeTimer -= uiDiff;

        // Transition to phase 3
        if (!m_bIsPhase3 && m_creature->GetHealthPercent() < 30.0f)
        {
            // ToDo: maybe there is a spell here - requires additional research
            DoScriptText(SAY_CHAMBER_DESTROY, m_creature);
            m_uiTransitionTimer = 5000;
            m_bIsPhase3 = true;
        }

        // Debris fall in phase 3
        if (m_bIsPhase3)
        {
            if (m_uiDebrisTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_QUAKE_EFFECT) == CAST_OK)
                    m_uiDebrisTimer = urand(20000, 30000);
            }
            else
                m_uiDebrisTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

/*######
## mob_hellfire_channeler
######*/

struct mob_hellfire_channelerAI : public ScriptedAI
{
    mob_hellfire_channelerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiShadowGraspTimer;
    uint32 m_uiShadowBoltVolleyTimer;
    uint32 m_uiDarkMendingTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiInfernalTimer;

    void Reset() override
    {
        m_uiShadowGraspTimer        = 10000;
        m_uiShadowBoltVolleyTimer   = urand(8000, 10000);
        m_uiDarkMendingTimer        = 10000;
        m_uiFearTimer               = urand(15000, 20000);
        m_uiInfernalTimer           = urand(10000, 50000);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        m_creature->InterruptNonMeleeSpells(false);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHANNELER_EVENT, IN_PROGRESS);
    }

    // Don't attack on LoS check
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_SOUL_TRANSFER, CAST_TRIGGERED);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHANNELER_EVENT, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (m_creature->getVictim())
            pSummoned->AI()->AttackStart(m_creature->getVictim());
    }

    void SummonedCreatureDespawn(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_BURNING_ABYSS)
            m_uiInfernalTimer = urand(10000, 15000);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Channel spell on Magtheridon, on OOC timer
        if (m_uiShadowGraspTimer)
        {
            if (m_uiShadowGraspTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SHADOW_GRASP_DUMMY) == CAST_OK)
                    m_uiShadowGraspTimer = 0;
            }
            else
                m_uiShadowGraspTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiShadowBoltVolleyTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SHADOW_BOLT_VOLLEY) == CAST_OK)
                m_uiShadowBoltVolleyTimer = urand(10000, 20000);
        }
        else
            m_uiShadowBoltVolleyTimer -= uiDiff;

        if (m_uiDarkMendingTimer < uiDiff)
        {
            if (Unit* pTarget = DoSelectLowestHpFriendly(30.0f))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_DARK_MENDING) == CAST_OK)
                    m_uiDarkMendingTimer = urand(10000, 20000);
            }
        }
        else
            m_uiDarkMendingTimer -= uiDiff;

        if (m_uiFearTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_FEAR) == CAST_OK)
                    m_uiFearTimer = urand(25000, 40000);
            }
        }
        else
            m_uiFearTimer -= uiDiff;

        if (m_uiInfernalTimer)
        {
            if (m_uiInfernalTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_BURNING_ABYSSAL) == CAST_OK)
                        m_uiInfernalTimer = 0;
                }
            }
            else
                m_uiInfernalTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

/*######
## go_manticron_cube
######*/

bool GOUse_go_manticron_cube(Player* pPlayer, GameObject* pGo)
{
    // if exhausted or already channeling return
    if (pPlayer->HasAura(SPELL_MIND_EXHAUSTION) || pPlayer->HasAura(SPELL_SHADOW_GRASP))
        return true;

    if (ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_MAGTHERIDON_EVENT) != IN_PROGRESS)
            return true;

        if (Creature* pMagtheridon = pInstance->GetSingleCreatureFromStorage(NPC_MAGTHERIDON))
        {
            if (!pMagtheridon->isAlive())
                return true;

            // visual is cast by cube
            if (Creature* pTrigger = GetClosestCreatureWithEntry(pGo, NPC_RAID_TRIGGER, 5.0f))
                pTrigger->CastSpell(pTrigger, SPELL_SHADOW_GRASP_VISUAL, false);

            // the real spell is cast by player
            pPlayer->CastSpell(pPlayer, SPELL_SHADOW_GRASP, false, NULL, NULL, pGo->GetObjectGuid());
        }
    }

    return true;
}

// TODO Remove this 'script' when combat and persistent area auras can be proper prevented from core-side
struct npc_target_triggerAI : public Scripted_NoMovementAI
{
    npc_target_triggerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) {Reset();}

    uint32 m_uiDebrisTimer;

    void Reset() override
    {
        m_uiDebrisTimer = 0;
    }

    void AttackStart(Unit* /*pWho*/) override {}
    void MoveInLineOfSight(Unit* /*pWho*/) override {}

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        // Workaround for missing core support for this type of dummy aura
        if (pSpell->Id == SPELL_QUAKE_EFFECT)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DEBRIS_VISUAL) == CAST_OK)
                m_uiDebrisTimer = 5000;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Cast debris damage after 5 seconds (on visual removal)
        if (m_uiDebrisTimer)
        {
            if (m_uiDebrisTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DEBRIS_DAMAGE) == CAST_OK)
                    m_uiDebrisTimer = 0;
            }
            else
                m_uiDebrisTimer -= uiDiff;
        }
    }
};

// ToDo: move this script to eventAI
struct mob_abyssalAI : public ScriptedAI
{
    mob_abyssalAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiFireBlastTimer;
    uint32 m_uiDespawnTimer;

    void Reset() override
    {
        m_uiDespawnTimer   = 60000;
        m_uiFireBlastTimer = 6000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiDespawnTimer < uiDiff)
        {
            m_creature->ForcedDespawn();
            m_uiDespawnTimer = 10000;
        }
        else
            m_uiDespawnTimer -= uiDiff;

        if (m_uiFireBlastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIRE_BLAST) == CAST_OK)
                m_uiFireBlastTimer = urand(5000, 15000);
        }
        else
            m_uiFireBlastTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_magtheridon(Creature* pCreature)
{
    return new boss_magtheridonAI(pCreature);
}

CreatureAI* GetAI_mob_hellfire_channeler(Creature* pCreature)
{
    return new mob_hellfire_channelerAI(pCreature);
}

CreatureAI* GetAI_npc_target_triggerAI(Creature* pCreature)
{
    return new npc_target_triggerAI(pCreature);
}

CreatureAI* GetAI_mob_abyssalAI(Creature* pCreature)
{
    return new mob_abyssalAI(pCreature);
}

void AddSC_boss_magtheridon()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_magtheridon";
    pNewScript->GetAI = &GetAI_boss_magtheridon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_hellfire_channeler";
    pNewScript->GetAI = &GetAI_mob_hellfire_channeler;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_manticron_cube";
    pNewScript->pGOUse = &GOUse_go_manticron_cube;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_target_trigger";
    pNewScript->GetAI = &GetAI_npc_target_triggerAI;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_abyssal";
    pNewScript->GetAI = &GetAI_mob_abyssalAI;
    pNewScript->RegisterSelf();
}

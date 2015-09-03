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
SDName: Bosses_Opera
SD%Complete: 95
SDComment: Oz, Hood, and RAJ event implemented. Spell timers may need adjustments.
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "karazhan.h"

/***********************************/
/*** OPERA WIZARD OF OZ EVENT *****/
/*********************************/

enum
{
    SAY_DOROTHEE_DEATH          = -1532025,
    SAY_DOROTHEE_SUMMON         = -1532026,
    SAY_DOROTHEE_TITO_DEATH     = -1532027,
    SAY_DOROTHEE_AGGRO          = -1532028,

    SAY_ROAR_AGGRO              = -1532029,
    SAY_ROAR_DEATH              = -1532030,
    SAY_ROAR_SLAY               = -1532031,

    SAY_STRAWMAN_AGGRO          = -1532032,
    SAY_STRAWMAN_DEATH          = -1532033,
    SAY_STRAWMAN_SLAY           = -1532034,

    SAY_TINHEAD_AGGRO           = -1532035,
    SAY_TINHEAD_DEATH           = -1532036,
    SAY_TINHEAD_SLAY            = -1532037,
    EMOTE_RUST                  = -1532038,

    SAY_CRONE_AGGRO             = -1532039,
    SAY_CRONE_AGGRO2            = -1532040,
    SAY_CRONE_DEATH             = -1532041,
    SAY_CRONE_SLAY              = -1532042,

    /**** Spells ****/
    // Dorothee
    SPELL_WATERBOLT             = 31012,
    SPELL_SCREAM                = 31013,
    SPELL_SUMMONTITO            = 31014,

    // Strawman
    SPELL_BRAIN_BASH            = 31046,
    SPELL_BRAIN_WIPE            = 31069,
    SPELL_CONFLAG_PROC          = 31073,            // procs 31075 on fire damage

    // Tinhead
    SPELL_CLEAVE                = 31043,
    SPELL_RUST                  = 31086,

    // Roar
    SPELL_MANGLE                = 31041,
    SPELL_SHRED                 = 31042,
    SPELL_FRIGHTENED_SCREAM     = 31013,

    // Crone
    SPELL_CHAIN_LIGHTNING       = 32337,

    // Cyclone
    SPELL_CYCLONE               = 32334,
    SPELL_CYCLONE_VISUAL        = 32332,

    /** Creature Entries **/
    NPC_TITO                    = 17548,
    NPC_CYCLONE                 = 18412,
};

struct boss_dorotheeAI : public ScriptedAI
{
    boss_dorotheeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiAggroTimer;
    uint32 m_uiIntroTimer;

    uint32 m_uiWaterBoltTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiSummonTitoTimer;

    bool m_bTitoDied;

    void Reset() override
    {
        m_uiIntroTimer      = 2000;
        m_uiAggroTimer      = 12000;

        m_uiWaterBoltTimer  = 5000;
        m_uiFearTimer       = 15000;
        m_uiSummonTitoTimer = 47500;

        m_bTitoDied         = false;
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DOROTHEE_DEATH, m_creature);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // Allow a short delay before attacking
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (m_creature->getVictim())
            pSummoned->AI()->AttackStart(m_creature->getVictim());
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_TITO)
        {
            DoScriptText(SAY_DOROTHEE_TITO_DEATH, m_creature);
            m_bTitoDied = true;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiIntroTimer)
        {
            if (m_uiIntroTimer <= uiDiff)
            {
                DoScriptText(SAY_DOROTHEE_AGGRO, m_creature);
                m_uiIntroTimer = 0;
            }
            else
                m_uiIntroTimer -= uiDiff;
        }

        if (m_uiAggroTimer)
        {
            if (m_uiAggroTimer <= uiDiff)
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                m_creature->SetInCombatWithZone();
                m_uiAggroTimer = 0;
            }
            else
                m_uiAggroTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiWaterBoltTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_WATERBOLT) == CAST_OK)
                    m_uiWaterBoltTimer = m_bTitoDied ? 1500 : 5000;
            }
        }
        else
            m_uiWaterBoltTimer -= uiDiff;

        if (m_uiFearTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SCREAM) == CAST_OK)
                m_uiFearTimer = 30000;
        }
        else
            m_uiFearTimer -= uiDiff;

        if (m_uiSummonTitoTimer)
        {
            if (m_uiSummonTitoTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMONTITO) == CAST_OK)
                {
                    DoScriptText(SAY_DOROTHEE_SUMMON, m_creature);
                    m_uiSummonTitoTimer = 0;
                }
            }
            else
                m_uiSummonTitoTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

struct boss_strawmanAI : public ScriptedAI
{
    boss_strawmanAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiAggroTimer;
    uint32 m_uiBrainBashTimer;
    uint32 m_uiBrainWipeTimer;

    void Reset() override
    {
        m_uiAggroTimer     = 27000;
        m_uiBrainBashTimer = 5000;
        m_uiBrainWipeTimer = 7000;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_CONFLAG_PROC);
        DoScriptText(SAY_STRAWMAN_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_STRAWMAN_DEATH, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_STRAWMAN_SLAY, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiAggroTimer)
        {
            if (m_uiAggroTimer <= uiDiff)
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                m_creature->SetInCombatWithZone();
                m_uiAggroTimer = 0;
            }
            else
                m_uiAggroTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBrainBashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_BRAIN_BASH) == CAST_OK)
                m_uiBrainBashTimer = 15000;
        }
        else
            m_uiBrainBashTimer -= uiDiff;

        if (m_uiBrainWipeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_BRAIN_WIPE) == CAST_OK)
                    m_uiBrainWipeTimer = 20000;
            }
        }
        else
            m_uiBrainWipeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct boss_tinheadAI : public ScriptedAI
{
    boss_tinheadAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiAggroTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiRustTimer;

    void Reset() override
    {
        m_uiAggroTimer  = 37000;
        m_uiCleaveTimer = 5000;
        m_uiRustTimer   = 30000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_TINHEAD_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_TINHEAD_DEATH, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_TINHEAD_SLAY, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiAggroTimer)
        {
            if (m_uiAggroTimer <= uiDiff)
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                m_creature->SetInCombatWithZone();
                m_uiAggroTimer = 0;
            }
            else
                m_uiAggroTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = 5000;
        }
        else
            m_uiCleaveTimer -= uiDiff;

        if (m_uiRustTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_RUST) == CAST_OK)
            {
                DoScriptText(EMOTE_RUST, m_creature);
                m_uiRustTimer = 6000;
            }
        }
        else
            m_uiRustTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct boss_roarAI : public ScriptedAI
{
    boss_roarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiAggroTimer;
    uint32 m_uiMangleTimer;
    uint32 m_uiShredTimer;
    uint32 m_uiScreamTimer;

    void Reset() override
    {
        m_uiAggroTimer  = 17000;
        m_uiMangleTimer = 5000;
        m_uiShredTimer  = 10000;
        m_uiScreamTimer = 15000;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_ROAR_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_ROAR_DEATH, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_ROAR_SLAY, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiAggroTimer)
        {
            if (m_uiAggroTimer <= uiDiff)
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                m_creature->SetInCombatWithZone();
                m_uiAggroTimer = 0;
            }
            else
                m_uiAggroTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiMangleTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MANGLE) == CAST_OK)
                m_uiMangleTimer = urand(5000, 8000);
        }
        else
            m_uiMangleTimer -= uiDiff;

        if (m_uiShredTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHRED) == CAST_OK)
                m_uiShredTimer = urand(10000, 15000);
        }
        else
            m_uiShredTimer -= uiDiff;

        if (m_uiScreamTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FRIGHTENED_SCREAM) == CAST_OK)
                m_uiScreamTimer = urand(20000, 30000);
        }
        else
            m_uiScreamTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

static const float afCycloneSpawnLoc[4] = { -10907.68f, -1778.651f, 90.56018f, 0.61f};

struct boss_croneAI : public ScriptedAI
{
    boss_croneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiChainLightningTimer;

    void Reset() override
    {
        m_uiChainLightningTimer = 10000;
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_CRONE_AGGRO : SAY_CRONE_AGGRO2, m_creature);

        // spawn the cyclone on aggro
        m_creature->SummonCreature(NPC_CYCLONE, afCycloneSpawnLoc[0], afCycloneSpawnLoc[1], afCycloneSpawnLoc[2], afCycloneSpawnLoc[3], TEMPSUMMON_DEAD_DESPAWN, 0);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_CRONE_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_OPERA, DONE);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->CastSpell(pSummoned, SPELL_CYCLONE, true);
        pSummoned->CastSpell(pSummoned, SPELL_CYCLONE_VISUAL, true);
        pSummoned->GetMotionMaster()->MoveRandomAroundPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 15.0f);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiChainLightningTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_CHAIN_LIGHTNING) == CAST_OK)
                    m_uiChainLightningTimer = 15000;
            }
        }
        else
            m_uiChainLightningTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dorothee(Creature* pCreature)
{
    return new boss_dorotheeAI(pCreature);
}

CreatureAI* GetAI_boss_strawman(Creature* pCreature)
{
    return new boss_strawmanAI(pCreature);
}

CreatureAI* GetAI_boss_tinhead(Creature* pCreature)
{
    return new boss_tinheadAI(pCreature);
}

CreatureAI* GetAI_boss_roar(Creature* pCreature)
{
    return new boss_roarAI(pCreature);
}

CreatureAI* GetAI_boss_crone(Creature* pCreature)
{
    return new boss_croneAI(pCreature);
}

/**************************************/
/**** Opera Red Riding Hood Event ****/
/************************************/

enum
{
    /**** Yells for the Wolf ****/
    SAY_WOLF_AGGRO                  = -1532043,
    SAY_WOLF_SLAY                   = -1532044,
    SAY_WOLF_HOOD                   = -1532045,
    SOUND_WOLF_DEATH                = 9275,                // Only sound on death, no text.

    /**** Spells For The Wolf ****/
    SPELL_PICK_RED_RIDING_HOOD      = 30769,               // targeting spell - triggers 30768
    SPELL_TERRIFYING_HOWL           = 30752,
    SPELL_WIDE_SWIPE                = 30761,

    GOSSIP_ITEM_GRANDMA             = -3532005,
    TEXT_ID_GRANDMA                 = 8990,

    /**** The Wolf's Entry ****/
    NPC_BIG_BAD_WOLF                = 17521
};

bool GossipHello_npc_grandmother(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GRANDMA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_GRANDMA, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_grandmother(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        if (Creature* pBigBadWolf = pCreature->SummonCreature(NPC_BIG_BAD_WOLF, 0, 0, 0, 0, TEMPSUMMON_DEAD_DESPAWN, 0))
            pBigBadWolf->AI()->AttackStart(pPlayer);

        pCreature->ForcedDespawn();
    }

    return true;
}

struct boss_bigbadwolfAI : public ScriptedAI
{
    boss_bigbadwolfAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiRedRidingHoodTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiSwipeTimer;

    void Reset() override
    {
        m_uiRedRidingHoodTimer = 30000;
        m_uiFearTimer          = urand(25000, 35000);
        m_uiSwipeTimer         = 5000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_WOLF_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoPlaySoundToSet(m_creature, SOUND_WOLF_DEATH);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_OPERA, DONE);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiRedRidingHoodTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_PICK_RED_RIDING_HOOD) == CAST_OK)
            {
                DoScriptText(SAY_WOLF_HOOD, m_creature);
                m_uiRedRidingHoodTimer = 30000;
            }
        }
        else
            m_uiRedRidingHoodTimer -= uiDiff;

        if (m_uiFearTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TERRIFYING_HOWL) == CAST_OK)
                m_uiFearTimer = 24000;
        }
        else
            m_uiFearTimer -= uiDiff;

        if (m_uiSwipeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WIDE_SWIPE) == CAST_OK)
                m_uiSwipeTimer = urand(25000, 30000);
        }
        else
            m_uiSwipeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_bigbadwolf(Creature* pCreature)
{
    return new boss_bigbadwolfAI(pCreature);
}

/**********************************************/
/******** Opera Romeo and Juliet Event *******/
/********************************************/

enum
{
    /**** Speech *****/
    SAY_JULIANNE_AGGRO              = -1532046,
    SAY_JULIANNE_ENTER              = -1532047,
    SAY_JULIANNE_DEATH01            = -1532048,
    SAY_JULIANNE_DEATH02            = -1532049,
    SAY_JULIANNE_RESURRECT          = -1532050,
    SAY_JULIANNE_SLAY               = -1532051,

    SAY_ROMULO_AGGRO                = -1532052,
    SAY_ROMULO_DEATH                = -1532053,
    SAY_ROMULO_ENTER                = -1532054,
    SAY_ROMULO_RESURRECT            = -1532055,
    SAY_ROMULO_SLAY                 = -1532056,

    /***** Spells For Julianne *****/
    SPELL_BLINDING_PASSION          = 30890,
    SPELL_DEVOTION                  = 30887,
    SPELL_ETERNAL_AFFECTION         = 30878,
    SPELL_POWERFUL_ATTRACTION       = 30889,
    SPELL_DRINK_POISON              = 30907,

    /***** Spells For Romulo ****/
    SPELL_BACKWARD_LUNGE            = 30815,
    SPELL_DARING                    = 30841,
    SPELL_DEADLY_SWATHE             = 30817,
    SPELL_POISON_THRUST             = 30822,

    /**** Other Misc. Spells ****/
    SPELL_FULL_HEALTH               = 43979,                // res effect on Julianne
    SPELL_UNDYING_LOVE              = 30951,                // res effect on Romulo
};

enum OperaPhase
{
    PHASE_JULIANNE      = 0,
    PHASE_ROMULO        = 1,
    PHASE_BOTH          = 2,
};

static const float afRomuloSpawnLoc[4] = { -10893.62f, -1760.78f, 90.55f, 4.76f};

struct boss_julianneAI : public ScriptedAI
{
    boss_julianneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    OperaPhase m_Phase;

    uint32 m_uiBlindingPassionTimer;
    uint32 m_uiDevotionTimer;
    uint32 m_uiEternalAffectionTimer;
    uint32 m_uiPowerfulAttractionTimer;
    uint32 m_uiSummonRomuloTimer;
    uint32 m_uiResurrectSelfTimer;

    bool m_bIsFakingDeath;

    void Reset() override
    {
        m_Phase                     = PHASE_JULIANNE;

        m_uiBlindingPassionTimer    = 30000;
        m_uiDevotionTimer           = 15000;
        m_uiEternalAffectionTimer   = 25000;
        m_uiPowerfulAttractionTimer = 5000;
        m_uiSummonRomuloTimer       = 0;
        m_uiResurrectSelfTimer      = 0;

        m_bIsFakingDeath            = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_JULIANNE_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& uiDamage) override
    {
        if (uiDamage < m_creature->GetHealth())
            return;

        uiDamage = 0;

        if (m_bIsFakingDeath)
            return;

        if (m_Phase == PHASE_JULIANNE)
        {
            // Prepare fake death
            if (DoCastSpellIfCan(m_creature, SPELL_DRINK_POISON, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
            {
                m_Phase               = PHASE_BOTH;
                m_bIsFakingDeath      = true;
                m_uiSummonRomuloTimer = 12000;
            }
        }
        else if (m_Phase == PHASE_BOTH)
        {
            // set fake death and allow 10 sec timer to kill Romulos
            DoScriptText(SAY_JULIANNE_DEATH02, m_creature);
            DoSetFakeDeath();
            m_uiResurrectSelfTimer = 10000;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_OPERA, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(SAY_JULIANNE_SLAY, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (m_creature->getVictim())
            pSummoned->AI()->AttackStart(m_creature->getVictim());
    }

    // Wrapper to set fake death
    void DoSetFakeDeath()
    {
        m_bIsFakingDeath = true;

        m_creature->InterruptNonMeleeSpells(false);
        m_creature->SetHealth(1);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->ClearAllReactives();
        m_creature->SetTargetGuid(ObjectGuid());
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
    }

    // Wrapper to remove fake death
    void DoRemoveFakeDeath()
    {
        m_bIsFakingDeath = false;

        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->GetMotionMaster()->Clear();
        DoStartMovement(m_creature->getVictim());
    }

    // Wrapper to start phase 3
    void DoHandleRomuloResurrect()
    {
        if (DoCastSpellIfCan(m_creature, SPELL_UNDYING_LOVE) == CAST_OK)
        {
            DoCastSpellIfCan(m_creature, SPELL_FULL_HEALTH, CAST_TRIGGERED);
            DoScriptText(SAY_JULIANNE_RESURRECT, m_creature);
            DoRemoveFakeDeath();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // spawn Romulo on timer after fake death
        if (m_uiSummonRomuloTimer)
        {
            if (m_uiSummonRomuloTimer <= uiDiff)
            {
                m_creature->SummonCreature(NPC_ROMULO, afRomuloSpawnLoc[0], afRomuloSpawnLoc[1], afRomuloSpawnLoc[2], afRomuloSpawnLoc[3], TEMPSUMMON_DEAD_DESPAWN, 0);
                m_uiSummonRomuloTimer = 0;
            }
            else
                m_uiSummonRomuloTimer -= uiDiff;
        }

        if (m_uiResurrectSelfTimer)
        {
            if (m_uiResurrectSelfTimer <= uiDiff)
            {
                if (m_pInstance)
                {
                    if (Creature* pRomulo = m_pInstance->GetSingleCreatureFromStorage(NPC_ROMULO))
                    {
                        // if Romulos is dead, then self kill
                        if (pRomulo->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                        {
                            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                            pRomulo->DealDamage(pRomulo, pRomulo->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                        }
                        else
                        {
                            DoRemoveFakeDeath();
                            DoCastSpellIfCan(m_creature, SPELL_FULL_HEALTH, CAST_TRIGGERED);
                        }
                    }
                }
                m_uiResurrectSelfTimer = 0;
            }
            else
                m_uiResurrectSelfTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // don't use spells during transition
        if (m_bIsFakingDeath)
            return;

        if (m_uiBlindingPassionTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_BLINDING_PASSION) == CAST_OK)
                    m_uiBlindingPassionTimer = urand(30000, 45000);
            }
        }
        else
            m_uiBlindingPassionTimer -= uiDiff;

        if (m_uiDevotionTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DEVOTION) == CAST_OK)
                m_uiDevotionTimer = urand(15000, 45000);
        }
        else
            m_uiDevotionTimer -= uiDiff;

        if (m_uiPowerfulAttractionTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_POWERFUL_ATTRACTION) == CAST_OK)
                    m_uiPowerfulAttractionTimer = urand(5000, 30000);
            }
        }
        else
            m_uiPowerfulAttractionTimer -= uiDiff;

        if (m_uiEternalAffectionTimer < uiDiff)
        {
            if (Unit* pTarget = DoSelectLowestHpFriendly(30.0f))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_ETERNAL_AFFECTION) == CAST_OK)
                    m_uiEternalAffectionTimer = urand(45000, 60000);
            }
        }
        else
            m_uiEternalAffectionTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool EffectDummyCreature_spell_drink_poison(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_DRINK_POISON && uiEffIndex == EFFECT_INDEX_0)
    {
        // Set fake death on poison
        if (boss_julianneAI* pJulianneAI = dynamic_cast<boss_julianneAI*>(pCreatureTarget->AI()))
            pJulianneAI->DoSetFakeDeath();

        DoScriptText(SAY_JULIANNE_DEATH01, pCreatureTarget);

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

struct boss_romuloAI : public ScriptedAI
{
    boss_romuloAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    OperaPhase m_Phase;

    uint32 m_uiBackwardLungeTimer;
    uint32 m_uiDaringTimer;
    uint32 m_uiDeadlySwatheTimer;
    uint32 m_uiPoisonThrustTimer;
    uint32 m_uiResurrectTimer;
    uint32 m_uiResurrectSelfTimer;

    bool m_bIsFakingDeath;

    void Reset() override
    {
        m_Phase                 = PHASE_ROMULO;

        m_uiBackwardLungeTimer  = 15000;
        m_uiDaringTimer         = 20000;
        m_uiDeadlySwatheTimer   = 25000;
        m_uiPoisonThrustTimer   = 10000;
        m_uiResurrectTimer      = 0;
        m_uiResurrectSelfTimer  = 0;

        m_bIsFakingDeath        = false;
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& uiDamage) override
    {
        if (uiDamage < m_creature->GetHealth())
            return;

        uiDamage = 0;

        if (m_Phase == PHASE_ROMULO)
        {
            DoScriptText(SAY_ROMULO_DEATH, m_creature);
            DoSetFakeDeath();
            m_Phase             = PHASE_BOTH;
            m_uiResurrectTimer  = 10000;
        }
        else if (m_Phase == PHASE_BOTH)
        {
            // set fake death and allow 10 sec timer to kill Julianne
            DoSetFakeDeath();
            m_uiResurrectSelfTimer = 10000;
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_ROMULO_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_OPERA, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(SAY_ROMULO_SLAY, m_creature);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        // remove fake death on res
        if (pSpell->Id == SPELL_UNDYING_LOVE && pCaster->GetEntry() == NPC_JULIANNE)
            DoRemoveFakeDeath();
    }

    // Wrapper to set fake death
    void DoSetFakeDeath()
    {
        m_bIsFakingDeath = true;

        m_creature->InterruptNonMeleeSpells(false);
        m_creature->SetHealth(1);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->ClearAllReactives();
        m_creature->SetTargetGuid(ObjectGuid());
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
    }

    // Wrapper to remove fake death
    void DoRemoveFakeDeath()
    {
        m_bIsFakingDeath = false;

        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->GetMotionMaster()->Clear();
        DoStartMovement(m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Resurrect both of them at the beginning of phase 3
        if (m_uiResurrectTimer)
        {
            if (m_uiResurrectTimer <= uiDiff)
            {
                if (m_pInstance)
                {
                    if (Creature* pJulianne = m_pInstance->GetSingleCreatureFromStorage(NPC_JULIANNE))
                    {
                        if (boss_julianneAI* pJulianneAI = dynamic_cast<boss_julianneAI*>(pJulianne->AI()))
                            pJulianneAI->DoHandleRomuloResurrect();
                    }
                }
                m_uiResurrectTimer = 0;
            }
            else
                m_uiResurrectTimer -= uiDiff;
        }

        if (m_uiResurrectSelfTimer)
        {
            if (m_uiResurrectSelfTimer <= uiDiff)
            {
                if (m_pInstance)
                {
                    if (Creature* pJulianne = m_pInstance->GetSingleCreatureFromStorage(NPC_JULIANNE))
                    {
                        // if Julianne is dead, then self kill
                        if (pJulianne->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                        {
                            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                            pJulianne->DealDamage(pJulianne, pJulianne->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                        }
                        else
                        {
                            DoRemoveFakeDeath();
                            DoScriptText(SAY_ROMULO_RESURRECT, m_creature);
                            DoCastSpellIfCan(m_creature, SPELL_FULL_HEALTH, CAST_TRIGGERED);
                        }
                    }
                }
                m_uiResurrectSelfTimer = 0;
            }
            else
                m_uiResurrectSelfTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // don't use spells on fake death
        if (m_bIsFakingDeath)
            return;

        if (m_uiBackwardLungeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BACKWARD_LUNGE) == CAST_OK)
                m_uiBackwardLungeTimer = urand(15000, 30000);
        }
        else
            m_uiBackwardLungeTimer -= uiDiff;

        if (m_uiDaringTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DARING) == CAST_OK)
                m_uiDaringTimer = urand(20000, 40000);
        }
        else
            m_uiDaringTimer -= uiDiff;

        if (m_uiDeadlySwatheTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_DEADLY_SWATHE) == CAST_OK)
                    m_uiDeadlySwatheTimer = urand(15000, 25000);
            }
        }
        else
            m_uiDeadlySwatheTimer -= uiDiff;

        if (m_uiPoisonThrustTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_POISON_THRUST) == CAST_OK)
                m_uiPoisonThrustTimer = urand(10000, 20000);
        }
        else
            m_uiPoisonThrustTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_julianne(Creature* pCreature)
{
    return new boss_julianneAI(pCreature);
}

CreatureAI* GetAI_boss_romulo(Creature* pCreature)
{
    return new boss_romuloAI(pCreature);
}

void AddSC_bosses_opera()
{
    Script* pNewScript;

    // Oz
    pNewScript = new Script;
    pNewScript->Name = "boss_dorothee";
    pNewScript->GetAI = &GetAI_boss_dorothee;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_strawman";
    pNewScript->GetAI = &GetAI_boss_strawman;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_tinhead";
    pNewScript->GetAI = &GetAI_boss_tinhead;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_roar";
    pNewScript->GetAI = &GetAI_boss_roar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crone";
    pNewScript->GetAI = &GetAI_boss_crone;
    pNewScript->RegisterSelf();

    // Hood
    pNewScript = new Script;
    pNewScript->Name = "npc_grandmother";
    pNewScript->pGossipHello = &GossipHello_npc_grandmother;
    pNewScript->pGossipSelect = &GossipSelect_npc_grandmother;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_bigbadwolf";
    pNewScript->GetAI = &GetAI_boss_bigbadwolf;
    pNewScript->RegisterSelf();

    // Romeo And Juliet
    pNewScript = new Script;
    pNewScript->Name = "boss_julianne";
    pNewScript->GetAI = &GetAI_boss_julianne;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_spell_drink_poison;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_romulo";
    pNewScript->GetAI = &GetAI_boss_romulo;
    pNewScript->RegisterSelf();
}

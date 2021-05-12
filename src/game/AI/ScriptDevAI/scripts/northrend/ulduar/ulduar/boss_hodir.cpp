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
SDName: boss_hodir
SD%Complete: 90%
SDComment: Achievements NYI.
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ulduar.h"
#include "Entities/TemporarySpawn.h"

enum
{
    SAY_AGGRO                           = -1603086,
    SAY_SLAY_1                          = -1603087,
    SAY_SLAY_2                          = -1603088,
    SAY_FLASH_FREEZE                    = -1603089,
    SAY_FROZEN_BLOWS                    = -1603090,
    SAY_EPILOGUE                        = -1603091,
    SAY_BERSERK                         = -1603092,

    EMOTE_FLASH_FREEZE                  = -1603094,
    EMOTE_FROZEN_BLOWS                  = -1603095,

    // spells
    SPELL_BERSERK                       = 26662,
    SPELL_HODIR_CREDIT                  = 64899,                // kill credit spell; added in spell_template
    SPELL_SHATTER_CHEST                 = 65272,                // hard mode timer until chest is shattered; triggers 62501 which will send event 20907 if completed
    SPELL_FROZEN_BLOWS                  = 62478,
    SPELL_FROZEN_BLOWS_H                = 63512,
    SPELL_FREEZE                        = 62469,
    SPELL_BITTING_COLD                  = 62038,                // triggers 62039 and 62188
    SPELL_ICICLE_AURA                   = 62227,                // periodic targeting aura; triggers the spell which summons npc 33169
    SPELL_ICICLE_SNOWPACK               = 62476,                // cast right before Flash Freeze; triggers the spell which summons npc 33173
    SPELL_ICICLE_SNOWPACK_H             = 62477,
    SPELL_FLASH_FREEZE                  = 61968,                // main spell; sends event 20896

    // icicle spells
    SPELL_ICICLE                        = 62236,
    SPELL_ICICLE_DUMMY                  = 62453,

    // snowpacked icicle spells
    // SPELL_ICICLE_ICE_SHARDS          = 62460,                // triggers the spell which summons npc 33174 and go 194173

    // snowpacked icicle target spells
    SPELL_SAFE_AREA                     = 65705,                // grant immunity from flash freeze

    // flash freeze related spells
    // SPELL_FLASH_FREEZE_VISUAL        = 62148,                // cast by npc 30298 (handled by event 20896)
    // SPELL_FLASH_FREEZE_SUMMON        = 61970,                // cast by all Flash Freeze targets; summons 32926
    // SPELL_FLASH_FREEZE_SUMMON_NPC    = 61989,                // used to flash freeze all npc targets before the encounter; summons 32938
    // SPELL_FLASH_FREEZE_STUN          = 64175,                // use and purpose unk
    // SPELL_FLASH_FREEZE_FRIENDLY      = 64176,                // use and purpose unk

    // flash freeze spells
    SPELL_FLASH_FREEZE_AURA             = 61969,                // stuns the summoner
    SPELL_FLASH_FREEZE_KILL             = 62226,                // kill frozen targets

    // flash freeze npc spells
    SPELL_FLASH_FREEZE_AURA_NPC         = 61990,                // stuns the summoner (npc)
    SPELL_FLASH_FREEZE_INITIAL          = 62878,                // trigger aggro on Hodir if damaged; sends event 21045

    // npcs
    NPC_ICICLE                          = 33169,
    // NPC_SNOWPACKED_ICICLE            = 33173,                // entry used to generate npc 33173 and go 194173; handled in eventAI
    // NPC_SNOWPACKED_ICICLE_TARGET     = 33174,                // entry used to handle safe area aura from Flash Freeze; handled in eventAI
    NPC_FLASH_FREEZE                    = 32926,                // entry used during the encounter
    NPC_FLASH_FREEZE_NPC                = 32938,                // entry which stuns the friendly npcs before the actual fight

    // GO_SNOWDRIFT                     = 194173,
    EVENT_ID_ATTACK_START               = 21045,
    EVENT_ID_SHATTER_CHEST              = 20907,
    FACTION_ID_FRIENDLY                 = 35,
};

/*######
## boss_hodir
######*/

struct boss_hodirAI : public ScriptedAI
{
    boss_hodirAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bEventFinished = false;
        m_uiEpilogueTimer = 0;
        m_uiEpilogueStage = 0;
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bEventFinished;

    uint32 m_uiEpilogueTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiFlashFreezeTimer;
    uint32 m_uiFrozenBlowsTimer;
    uint32 m_uiFreezeTimer;
    uint8 m_uiEpilogueStage;

    void Reset() override
    {
        m_uiBerserkTimer     = 8 * MINUTE * IN_MILLISECONDS;
        m_uiFlashFreezeTimer = 50000;
        m_uiFrozenBlowsTimer = 70000;
        m_uiFreezeTimer      = urand(25000, 30000);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_HODIR, IN_PROGRESS);
            m_pInstance->SetData(TYPE_HODIR_HARD, DONE);
        }

        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_BITTING_COLD, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_ICICLE_AURA, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(m_creature, SPELL_SHATTER_CHEST, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void AttackStart(Unit* pWho) override
    {
        // don't attack again after being defeated
        if (m_bEventFinished)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HODIR, FAIL);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);

        if (m_creature->IsAlive() && !m_bEventFinished)
            m_creature->GetMotionMaster()->MoveTargetedHome();

        m_creature->SetLootRecipient(nullptr);

        Reset();
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (!m_bEventFinished)
            {
                // Inform the faction helpers that the fight is over
                ThreatList const& threatList = m_creature->getThreatManager().getThreatList();
                for (auto itr : threatList)
                {
                    // only check creatures
                    if (!itr->getUnitGuid().IsCreature())
                        continue;

                    if (Creature* pTarget = m_creature->GetMap()->GetCreature(itr->getUnitGuid()))
                        pTarget->AI()->EnterEvadeMode();
                }

                m_uiEpilogueTimer = 10000;
                m_creature->CastSpell(m_creature, SPELL_HODIR_CREDIT, TRIGGERED_OLD_TRIGGERED);
                m_creature->SetFactionTemporary(FACTION_ID_FRIENDLY, TEMPFACTION_NONE);
                m_bEventFinished = true;
                EnterEvadeMode();
            }
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_ICICLE)
        {
            pSummoned->CastSpell(pSummoned, SPELL_ICICLE_DUMMY, TRIGGERED_NONE);
            pSummoned->CastSpell(pSummoned, SPELL_ICICLE, TRIGGERED_OLD_TRIGGERED);
            pSummoned->ForcedDespawn(5000);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiEpilogueTimer)
        {
            if (m_uiEpilogueTimer <= uiDiff)
            {
                switch (m_uiEpilogueStage)
                {
                    case 0:
                        if (m_pInstance)
                            m_pInstance->SetData(TYPE_HODIR, DONE);

                        DoScriptText(SAY_EPILOGUE, m_creature);
                        m_uiEpilogueTimer = 10000;
                        break;
                    case 1:
                        if (DoCastSpellIfCan(m_creature, SPELL_TELEPORT) == CAST_OK)
                        {
                            m_creature->ForcedDespawn(2000);
                            m_uiEpilogueTimer = 0;
                        }
                        break;
                }
                ++m_uiEpilogueStage;
            }
            else
                m_uiEpilogueTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        if (m_uiFlashFreezeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FLASH_FREEZE) == CAST_OK)
            {
                DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ICICLE_SNOWPACK : SPELL_ICICLE_SNOWPACK_H, CAST_TRIGGERED);
                DoScriptText(EMOTE_FLASH_FREEZE, m_creature);
                DoScriptText(SAY_FLASH_FREEZE, m_creature);
                m_uiFlashFreezeTimer = 50000;
            }
        }
        else
            m_uiFlashFreezeTimer -= uiDiff;

        if (m_uiFrozenBlowsTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FROZEN_BLOWS : SPELL_FROZEN_BLOWS_H) == CAST_OK)
            {
                DoScriptText(SAY_FROZEN_BLOWS, m_creature);
                DoScriptText(EMOTE_FROZEN_BLOWS, m_creature);
                m_uiFrozenBlowsTimer = 60000;
            }
        }
        else
            m_uiFrozenBlowsTimer -= uiDiff;

        if (m_uiFreezeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_FREEZE) == CAST_OK)
                    m_uiFreezeTimer = 15000;
            }
        }
        else
            m_uiFreezeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_hodir(Creature* pCreature)
{
    return new boss_hodirAI(pCreature);
}

/*######
## npc_flash_freeze
######*/

struct npc_flash_freezeAI : public Scripted_NoMovementAI
{
    npc_flash_freezeAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_pInstance;

    bool m_bFreezeInit;

    void Reset() override
    {
        m_bFreezeInit = false;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustDied(Unit* /*pKiller*/) override
    {
        // On Flash Freeze death, the owner should attack Hodir
        if (m_creature->GetEntry() == NPC_FLASH_FREEZE_NPC && m_creature->IsTemporarySummon() && m_pInstance)
        {
            if (Creature* pHodir = m_pInstance->GetSingleCreatureFromStorage(NPC_HODIR))
            {
                // ignore if event already completed
                if (pHodir->GetFaction() == FACTION_ID_FRIENDLY)
                    return;

                if (Creature* pSummoner = m_creature->GetMap()->GetCreature(m_creature->GetSpawnerGuid()))
                    pSummoner->AI()->AttackStart(pHodir);
            }
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/) override
    {
        // Flash Freeze npcs should be always be summoned
        if (!m_creature->IsTemporarySummon())
            return;

        // do the freezing on the first update tick
        if (!m_bFreezeInit)
        {
            // Flash Freeze npc will always stun or kill the summoner
            if (m_creature->GetEntry() == NPC_FLASH_FREEZE_NPC)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FLASH_FREEZE_AURA_NPC) == CAST_OK)
                    DoCastSpellIfCan(m_creature, SPELL_FLASH_FREEZE_INITIAL, CAST_TRIGGERED);
            }
            else if (m_creature->GetEntry() == NPC_FLASH_FREEZE)
            {
                if (Unit* pSummoner = m_creature->GetMap()->GetUnit(m_creature->GetSpawnerGuid()))
                {
                    // kill frozen players
                    if (pSummoner->HasAura(SPELL_FREEZE))
                        DoCastSpellIfCan(pSummoner, SPELL_FLASH_FREEZE_KILL);
                    else
                        DoCastSpellIfCan(m_creature, SPELL_FLASH_FREEZE_AURA);

                    if (pSummoner->GetTypeId() == TYPEID_PLAYER && m_pInstance)
                        m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_CHEESE_FREEZE, false);
                }
            }

            m_bFreezeInit = true;
        }
    }
};

UnitAI* GetAI_npc_flash_freeze(Creature* pCreature)
{
    return new npc_flash_freezeAI(pCreature);
}

/*######
## event_boss_hodir
######*/

bool ProcessEventId_event_boss_hodir(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (pSource->GetTypeId() == TYPEID_UNIT)
    {
        instance_ulduar* pInstance = (instance_ulduar*)((Creature*)pSource)->GetInstanceData();
        if (!pInstance)
            return true;

        if (uiEventId == EVENT_ID_SHATTER_CHEST)
        {
            // Mark hard mode as failed and despawn the Rare cache
            pInstance->SetData(TYPE_HODIR_HARD, FAIL);

            if (GameObject* pChest = pInstance->GetSingleGameObjectFromStorage(pInstance->instance->IsRegularDifficulty() ? GO_CACHE_OF_RARE_WINTER_10 : GO_CACHE_OF_RARE_WINTER_25))
                pChest->SetLootState(GO_JUST_DEACTIVATED);
        }
        else if (uiEventId == EVENT_ID_ATTACK_START)
        {
            // Start encounter
            if (Creature* pHodir = pInstance->GetSingleCreatureFromStorage(NPC_HODIR))
            {
                // ignore if event already completed
                if (pHodir->GetFaction() == FACTION_ID_FRIENDLY)
                    return true;

                pHodir->SetInCombatWithZone();
            }
        }

        return true;
    }

    return false;
}

/*######
## npc_icicle_target
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_icicle_targetAI : public Scripted_NoMovementAI
{
    npc_icicle_targetAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_SAFE_AREA);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_icicle_target(Creature* pCreature)
{
    return new npc_icicle_targetAI(pCreature);
}

void AddSC_boss_hodir()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_hodir";
    pNewScript->GetAI = GetAI_boss_hodir;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_flash_freeze";
    pNewScript->GetAI = GetAI_npc_flash_freeze;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_boss_hodir";
    pNewScript->pProcessEventId = &ProcessEventId_event_boss_hodir;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_icicle_target";
    pNewScript->GetAI = GetAI_npc_icicle_target;
    pNewScript->RegisterSelf();
}

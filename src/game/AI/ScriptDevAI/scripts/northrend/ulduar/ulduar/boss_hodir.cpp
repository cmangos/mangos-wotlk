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

#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "AI/ScriptDevAI/include/sc_common.h"
#include "ulduar.h"
#include "AI/ScriptDevAI/base/BossAI.h"

enum
{
    SAY_AGGRO                           = 34336,
    SAY_SLAY_1                          = 34337,
    SAY_SLAY_2                          = 34338,
    SAY_EPILOGUE                        = 33484,
    SAY_BERSERK                         = 34340,

    SAY_FLASH_FREEZE                    = 34339,
    EMOTE_FLASH_FREEZE                  = 33314,

    // spells
    SPELL_BERSERK                       = 26662,
    SPELL_HODIR_CREDIT                  = 64899,                // kill credit spell; added in spell_template
    SPELL_SHATTER_CHEST                 = 65272,                // hard mode timer until chest is shattered; triggers 62501 which will send event 20907 if completed
    SPELL_SHATTER_CHEST_TRIGGERED       = 62501,
    SPELL_FROZEN_BLOWS                  = 62478,
    SPELL_FROZEN_BLOWS_H                = 63512,
    SPELL_FREEZE                        = 62469,
    SPELL_BITTING_COLD                  = 62038,                // triggers 62039 and 62188
    SPELL_BITING_COLD_AURA              = 62039,
    SPELL_BITING_COLD_STACK             = 62188,
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
    SPELL_AURA_SAFE_AREA                = 62464,

    // flash freeze related spells
    // SPELL_FLASH_FREEZE_VISUAL        = 62148,                // cast by npc 30298 (handled by event 20896)
    SPELL_FLASH_FREEZE_SUMMON           = 61970,                // cast by all Flash Freeze targets; summons 32926
    // SPELL_FLASH_FREEZE_SUMMON_NPC    = 61989,                // used to flash freeze all npc targets before the encounter; summons 32938
    // SPELL_FLASH_FREEZE_STUN          = 64175,                // use and purpose unk
    // SPELL_FLASH_FREEZE_FRIENDLY      = 64176,                // use and purpose unk

    // flash freeze spells
    SPELL_FLASH_FREEZE_AURA             = 61969,                // stuns the summoner
    SPELL_FLASH_FREEZE_KILL             = 62226,                // kill frozen targets

    // flash freeze npc spells
    SPELL_FLASH_FREEZE_AURA_NPC         = 61990,                // stuns the summoner (npc)
    SPELL_FLASH_FREEZE_INITIAL          = 62878,                // trigger aggro on Hodir if damaged; sends event 21045

    // npc spells
    SPELL_TOASTY_FIRE                   = 62821,

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

enum HodirActions
{
    HODIR_AGGRO_SPELLS,
    HODIR_FLASH_FREEZE,
    HODIR_FREEZE,
    HODIR_BERSERK,
    HODIR_ACTIONS_MAX,
    HODIR_EPILOGUE,
};

struct boss_hodirAI : public BossAI
{
    boss_hodirAI(Creature* creature) : BossAI(creature, HODIR_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_ulduar*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())

    {
        SetDataType(TYPE_HODIR);
        AddOnAggroText(SAY_AGGRO);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
        AddCombatAction(HODIR_BERSERK, 8min);
        AddCombatAction(HODIR_FREEZE, 25s, 30s);
        AddCombatAction(HODIR_FLASH_FREEZE, 50s);
        AddTimerlessCombatAction(HODIR_AGGRO_SPELLS, true);
        AddCustomAction(HODIR_EPILOGUE, true, [&]()
        {
            switch (m_epilogueStage)
            {
                case 0:
                    if (m_instance)
                        m_instance->SetData(TYPE_HODIR, DONE);

                    DoBroadcastText(SAY_EPILOGUE, m_creature);
                    ResetTimer(HODIR_EPILOGUE, 10s);
                    break;
                case 1:
                    if (DoCastSpellIfCan(m_creature, SPELL_TELEPORT) == CAST_OK)
                    {
                        m_creature->ForcedDespawn(2000);
                    }
                    break;
            }
            ++m_epilogueStage;
        });
        SetDeathPrevention(true);
        m_eventFinished = false;
        m_epilogueStage = 0;
        Reset();
    }

    instance_ulduar* m_instance;
    bool m_isRegularMode;
    bool m_eventFinished;
    uint8 m_epilogueStage;

    void Aggro(Unit* /*pWho*/) override
    {
        BossAI::Aggro();
        if (m_instance)
            m_instance->SetData(TYPE_HODIR_HARD, DONE);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);

        if (m_creature->IsAlive() && !m_eventFinished)
            m_creature->GetMotionMaster()->MoveTargetedHome();

        m_creature->SetLootRecipient(nullptr);
    }

    void JustPreventedDeath(Unit* attacker) override
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

        ResetTimer(HODIR_EPILOGUE, 10s);
        m_creature->CastSpell(m_creature, SPELL_HODIR_CREDIT, TRIGGERED_OLD_TRIGGERED);
        m_creature->SetFactionTemporary(FACTION_ID_FRIENDLY, TEMPFACTION_NONE);
        m_eventFinished = true;
        SetCombatScriptStatus(true);
        m_creature->SetCanEnterCombat(false);
        m_creature->SetImmuneToNPC(true);
        EnterEvadeMode();
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

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case HODIR_AGGRO_SPELLS:
            {
                DoCastSpellIfCan(m_creature, SPELL_BITTING_COLD, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_ICICLE_AURA, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
                DoCastSpellIfCan(m_creature, SPELL_SHATTER_CHEST, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
                DisableCombatAction(action);
                return;
            }
            case HODIR_FREEZE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (DoCastSpellIfCan(nullptr, SPELL_FREEZE, CAST_TRIGGERED) == CAST_OK)
                        ResetCombatAction(action, 15s);
                return;
            }
            case HODIR_FLASH_FREEZE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_FLASH_FREEZE, CAST_FORCE_CAST) == CAST_OK)
                {
                    DoBroadcastText(SAY_FLASH_FREEZE, m_creature);
                    DoBroadcastText(EMOTE_FLASH_FREEZE, m_creature);
                    DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_ICICLE_SNOWPACK : SPELL_ICICLE_SNOWPACK_H, CAST_TRIGGERED);
                    ResetCombatAction(action, 50s);
                }
                return;
            }
            case HODIR_BERSERK:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoBroadcastText(SAY_BERSERK, m_creature);
                    DisableCombatAction(action);
                }
                return;
            }
        }
    }
};

/*######
## npc_flash_freeze
######*/

struct npc_flash_freezeAI : public Scripted_NoMovementAI
{
    npc_flash_freezeAI(Creature* creature) : Scripted_NoMovementAI(creature)
    {
        AddCustomAction(0, true, [&]()
        {
            if (m_creature->GetHealthPercent() >=99.9f)
                m_creature->CombatStop();
        });
        m_instance = (instance_ulduar*)creature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_instance;

    bool m_bFreezeInit;

    void Reset() override
    {
        m_bFreezeInit = false;
    }

    void Aggro(Unit* who) override
    {
        ResetIfNotStarted(0, 5s);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustDied(Unit* /*pKiller*/) override
    {
        // On Flash Freeze death, the owner should attack Hodir
        if (m_creature->GetEntry() == NPC_FLASH_FREEZE_NPC && m_creature->IsTemporarySummon() && m_instance)
        {
            if (Creature* pHodir = m_instance->GetSingleCreatureFromStorage(NPC_HODIR))
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

                    if (pSummoner->GetTypeId() == TYPEID_PLAYER && m_instance)
                        m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_CHEESE_FREEZE, false);
                }
            }

            m_bFreezeInit = true;
        }
    }
};

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
    npc_icicle_targetAI(Creature* creature) : Scripted_NoMovementAI(creature) { Reset(); }

    void Reset() override
    {
        SetAIImmobilizedState(true);
        DoCastSpellIfCan(m_creature, SPELL_SAFE_AREA);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

// 61968 Flash Freeze
struct FlashFreeze : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;
        if (aura->GetAuraTicks() == 1 && !target->HasAura(SPELL_AURA_SAFE_AREA))
            target->CastSpell(nullptr, SPELL_FLASH_FREEZE_SUMMON, TRIGGERED_INSTANT_CAST |
                TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_CASTER_AURA_STATE | TRIGGERED_IGNORE_GCD, nullptr, aura);
    }
};

// 65272 Shatter Chest
struct ShatterChest : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;
        target->CastSpell(target, SPELL_SHATTER_CHEST_TRIGGERED, TRIGGERED_OLD_TRIGGERED, nullptr, aura);
    }
};

// 62038 Biting Cold
struct BitingCold : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const
    {
        Player* target = dynamic_cast<Player*>(aura->GetTarget());
        if (!target)
            return;
        if (target->IsMoving())
            target->RemoveAuraHolderFromStack(SPELL_BITING_COLD_AURA);
        else if (aura->GetAuraTicks() % 3 && !target->HasAura(SPELL_TOASTY_FIRE))
            target->CastSpell(target, SPELL_BITING_COLD_AURA, TRIGGERED_OLD_TRIGGERED, nullptr, aura);
        return;
    }
};

// 62039 Biting Cold
struct BitingColdDamage : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;
        target->CastSpell(target, SPELL_BITING_COLD_STACK, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_hodir()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_hodir";
    pNewScript->GetAI = &GetNewAIInstance<boss_hodirAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_flash_freeze";
    pNewScript->GetAI = &GetNewAIInstance<npc_flash_freezeAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_boss_hodir";
    pNewScript->pProcessEventId = &ProcessEventId_event_boss_hodir;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_icicle_target";
    pNewScript->GetAI = &GetNewAIInstance<npc_icicle_targetAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<FlashFreeze>("spell_flash_freeze");
    RegisterSpellScript<ShatterChest>("spell_shatter_chest");
    RegisterSpellScript<BitingCold>("spell_biting_cold");
    RegisterSpellScript<BitingColdDamage>("spell_biting_cold_damage");
}

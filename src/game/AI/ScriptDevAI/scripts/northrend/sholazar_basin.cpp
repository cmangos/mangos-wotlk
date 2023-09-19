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
SDName: Sholazar_Basin
SD%Complete: 100
SDComment: Quest support: 12570, 12644, 12688
SDCategory: Sholazar Basin
EndScriptData */

/* ContentData
npc_helice
npc_injured_rainspeaker
npc_tipsy_mcmanus
npc_wants_fruit_credit
go_quest_still_at_it_credit
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"

/*######
## npc_helice
######*/

enum
{
    QUEST_ENGINEERING_DISASTER          = 12688,

    SAY_HELICE_ACCEPT                   = -1000657,
    SAY_HELICE_EXPLOSIVES_1             = -1000658,
    SAY_HELICE_EXPLODE_1                = -1000659,
    SAY_HELICE_MOVE_ON                  = -1000660,
    SAY_HELICE_EXPLOSIVES_2             = -1000661,
    SAY_HELICE_EXPLODE_2                = -1000662,
    SAY_HELICE_COMPLETE                 = -1000663,

    SPELL_DETONATE_EXPLOSIVES_1         = 52369,            // first "barrel"
    SPELL_DETONATE_EXPLOSIVES_2         = 52371,            // second "barrel"
};

struct npc_heliceAI : public npc_escortAI
{
    npc_heliceAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_uiExplodeTimer = 5000;
        m_uiExplodePhase = 0;
        m_bFirstBarrel = true;
        Reset();
    }

    uint32 m_uiExplodeTimer;
    uint32 m_uiExplodePhase;
    bool m_bFirstBarrel;

    void Reset() override
    {
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 2:
            {
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_HELICE_EXPLOSIVES_1, m_creature, pPlayer);
                    SetEscortPaused(true);
                }
                break;
            }
            case 13:
            {
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_HELICE_EXPLOSIVES_2, m_creature, pPlayer);
                    SetEscortPaused(true);
                }
                break;
            }
            case 22:
            {
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_HELICE_COMPLETE, m_creature, pPlayer);
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ENGINEERING_DISASTER, m_creature);
                }
                break;
            }
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
        {
            if (!HasEscortState(STATE_ESCORT_ESCORTING))
                return;

            if (HasEscortState(STATE_ESCORT_PAUSED))
            {
                if (m_uiExplodeTimer < uiDiff)
                {
                    if (m_bFirstBarrel)
                    {
                        switch (m_uiExplodePhase)
                        {
                            case 0:
                                DoCastSpellIfCan(m_creature, SPELL_DETONATE_EXPLOSIVES_1);

                                if (Player* pPlayer = GetPlayerForEscort())
                                    DoScriptText(SAY_HELICE_EXPLODE_1, m_creature, pPlayer);

                                m_uiExplodeTimer = 2500;
                                ++m_uiExplodePhase;
                                break;
                            case 1:
                                if (Player* pPlayer = GetPlayerForEscort())
                                    DoScriptText(SAY_HELICE_MOVE_ON, m_creature, pPlayer);

                                m_uiExplodeTimer = 2500;
                                ++m_uiExplodePhase;
                                break;
                            case 2:
                                SetEscortPaused(false);
                                m_uiExplodePhase = 0;
                                m_uiExplodeTimer = 5000;
                                m_bFirstBarrel = false;
                                break;
                        }
                    }
                    else
                    {
                        switch (m_uiExplodePhase)
                        {
                            case 0:
                                DoCastSpellIfCan(m_creature, SPELL_DETONATE_EXPLOSIVES_2);

                                if (Player* pPlayer = GetPlayerForEscort())
                                    DoScriptText(SAY_HELICE_EXPLODE_2, m_creature, pPlayer);

                                m_uiExplodeTimer = 2500;
                                ++m_uiExplodePhase;
                                break;
                            case 1:
                                SetEscortPaused(false);
                                m_uiExplodePhase = 0;
                                m_uiExplodeTimer = 5000;
                                m_bFirstBarrel = true;
                                break;
                        }
                    }
                }
                else
                    m_uiExplodeTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_helice(Creature* pCreature)
{
    return new npc_heliceAI(pCreature);
}

bool QuestAccept_npc_helice(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ENGINEERING_DISASTER)
    {
        DoScriptText(SAY_HELICE_ACCEPT, pCreature, pPlayer);

        if (npc_heliceAI* pEscortAI = dynamic_cast<npc_heliceAI*>(pCreature->AI()))
        {
            pEscortAI->Start(false, pPlayer, pQuest);
            pCreature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);
        }
    }

    return false;
}

/*######
## npc_injured_rainspeaker
######*/

enum
{
    QUEST_FORTUNATE_MISUNDERSTAND       = 12570,

    GOSSIP_ITEM_READY                   = -3000103,

    SAY_ACCEPT                          = -1000605,
    SAY_START                           = -1000606,
    SAY_END_1                           = -1000607,
    SAY_END_2                           = -1000608,
    SAY_TRACKER                         = -1000609,         // not used in escort (aggro text for trackers? something for vekjik?)

    NPC_FRENZYHEART_TRACKER             = 28077,

    SPELL_ORACLE_ESCORT_START           = 51341,            // unknown purpose
    SPELL_FEIGN_DEATH                   = 51329,
    SPELL_ORACLE_INTRO                  = 51448,
};

struct npc_injured_rainspeakerAI : public npc_escortAI
{
    npc_injured_rainspeakerAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() override { }

    void JustStartedEscort() override
    {
        if (Player* pPlayer = GetPlayerForEscort())
        {
            DoScriptText(SAY_START, m_creature, pPlayer);
            DoCastSpellIfCan(m_creature, SPELL_ORACLE_ESCORT_START);
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 23:
            {
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_END_1, m_creature, pPlayer);
                    DoCastSpellIfCan(m_creature, SPELL_ORACLE_INTRO);
                }
                break;
            }
            case 24:
            {
                DoScriptText(SAY_END_2, m_creature);

                // location behind
                float fAngle = m_creature->GetOrientation();
                fAngle += M_PI_F;

                float fX, fY, fZ;
                m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0.0f, 15.0f, fAngle);

                m_creature->SummonCreature(NPC_FRENZYHEART_TRACKER, fX, fY, fZ, m_creature->GetOrientation(), TEMPSPAWN_TIMED_DESPAWN, 30000);
                break;
            }
        }
    }

    void UpdateEscortAI(const uint32 /*uiDiff*/) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_injured_rainspeaker(Creature* pCreature)
{
    return new npc_injured_rainspeakerAI(pCreature);
}

bool QuestAccept_npc_injured_rainspeaker(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_FORTUNATE_MISUNDERSTAND)
    {
        pCreature->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
        DoScriptText(SAY_ACCEPT, pCreature, pPlayer);

        // Workaround, GossipHello/GossipSelect doesn't work well when object already has gossip from database
        if (npc_injured_rainspeakerAI* pEscortAI = dynamic_cast<npc_injured_rainspeakerAI*>(pCreature->AI()))
        {
            pEscortAI->Start(true, pPlayer, pQuest);
            pCreature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);
        }
    }

    return false;
}

/*
bool GossipHello_npc_injured_rainspeaker(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_FORTUNATE_MISUNDERSTAND) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_READY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
        return true;
    }

    return false;
}

bool GossipSelect_npc_injured_rainspeaker(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        if (npc_injured_rainspeakerAI* pEscortAI = dynamic_cast<npc_injured_rainspeakerAI*>(pCreature->AI()))
            pEscortAI->Start(true, pPlayer);
    }

    return false;
}
*/

/*######
## npc_tipsy_mcmanus
######*/

enum
{
    SAY_DISTILLATION_START          = -1001125,
    SAY_ADD_ORANGE                  = -1001126,
    SAY_ADD_BANANAS                 = -1001127,
    SAY_ADD_PAPAYA                  = -1001128,
    SAY_LIGHT_BRAZIER               = -1001129,
    SAY_OPEN_VALVE                  = -1001130,
    SAY_ACTION_COMPLETE_1           = -1001131,
    SAY_ACTION_COMPLETE_2           = -1001132,
    SAY_ACTION_COMPLETE_3           = -1001133,
    SAY_ACTION_COMPLETE_4           = -1001134,
    SAY_DISTILLATION_FAIL           = -1001135,
    SAY_DISTILLATION_COMPLETE       = -1001136,

    GOSSIP_ITEM_TIPSY_MCMANUS       = -3000114,
    TEXT_ID_READY                   = 13288,
    QUEST_ID_STILL_AT_IT            = 12644,

    SPELL_TOSS_ORANGE               = 51931,
    SPELL_TOSS_BANANA               = 51932,
    SPELL_TOSS_PAPAYA               = 51933,

    NPC_WANTS_ORANGE_TRIGGER        = 28535,
    NPC_WANTS_PAPAYA_TRIGGER        = 28536,
    NPC_WANTS_BANANA_TRIGGER        = 28537,
    // NPC_STEAMING_VALVE_TRIGGER   = 28539,
    // NPC_WANTS_FIRE_TRIGGER       = 28540,
    NPC_TIPSY_MCMANUS               = 28566,

    GO_THUNDERBREW_JUNGLE_PUNCH     = 190643,
    GO_PRESSURE_VALVE               = 190635,
    GO_BRAZIER                      = 190636,
};

struct StillAtItData
{
    int32 iText;
    uint32 uiOwnerEntry;
};

static const StillAtItData aStillAtItFruits[3] =
{
    {SAY_ADD_ORANGE,     NPC_WANTS_ORANGE_TRIGGER},
    {SAY_ADD_BANANAS,    NPC_WANTS_BANANA_TRIGGER},
    {SAY_ADD_PAPAYA,     NPC_WANTS_PAPAYA_TRIGGER},
};

static const StillAtItData aStillAtItMachines[2] =
{
    {SAY_LIGHT_BRAZIER,  GO_BRAZIER},
    {SAY_OPEN_VALVE,     GO_PRESSURE_VALVE},
};

struct npc_tipsy_mcmanusAI : public ScriptedAI
{
    npc_tipsy_mcmanusAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint8 m_uiTaskIndex;
    uint32 m_uiTaskOwnerEntry;
    uint32 m_uiTaskTimer;
    uint32 m_uiActionTimer;

    void Reset() override
    {
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        m_uiTaskIndex       = 0;
        m_uiTaskOwnerEntry  = 0;
        m_uiTaskTimer       = 0;
        m_uiActionTimer     = 0;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (pInvoker->GetTypeId() != TYPEID_PLAYER)
            return;

        // start event
        if (eventType == AI_EVENT_START_EVENT)
        {
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            DoScriptText(SAY_DISTILLATION_START, m_creature);
            m_uiTaskTimer = 5000;
        }
        // check fruit tasks
        else if (eventType == AI_EVENT_CUSTOM_A)
        {
            for (auto aStillAtItFruit : aStillAtItFruits)
            {
                if (aStillAtItFruit.uiOwnerEntry == uiMiscValue)
                    DoCheckDistillationTask(uiMiscValue);
            }
        }
        // check machine tasks
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            for (auto aStillAtItMachine : aStillAtItMachines)
            {
                if (aStillAtItMachine.uiOwnerEntry == uiMiscValue)
                    DoCheckDistillationTask(uiMiscValue);
            }
        }
    }

    // wrapper to complete a distillation task
    void DoCheckDistillationTask(uint32 uiOwnerEntry)
    {
        // check if the given entry matches the expected one
        if (uiOwnerEntry == m_uiTaskOwnerEntry)
        {
            switch (urand(0, 3))
            {
                case 0: DoScriptText(SAY_ACTION_COMPLETE_1, m_creature); break;
                case 1: DoScriptText(SAY_ACTION_COMPLETE_2, m_creature); break;
                case 2: DoScriptText(SAY_ACTION_COMPLETE_3, m_creature); break;
                case 3: DoScriptText(SAY_ACTION_COMPLETE_4, m_creature); break;
            }

            m_uiTaskTimer = 6000;
            m_uiActionTimer = 0;
        }
        // reset if failed
        else
        {
            DoScriptText(SAY_DISTILLATION_FAIL, m_creature);
            EnterEvadeMode();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiTaskTimer)
        {
            if (m_uiTaskTimer <= uiDiff)
            {
                switch (m_uiTaskIndex)
                {
                    // fruit tasks
                    case 0:
                    case 1:
                    case 3:
                    case 6:
                    case 7:
                    {
                        uint8 uiIndex = urand(0, 2);
                        DoScriptText(aStillAtItFruits[uiIndex].iText, m_creature);
                        m_uiTaskOwnerEntry = aStillAtItFruits[uiIndex].uiOwnerEntry;

                        m_uiTaskTimer = 0;
                        m_uiActionTimer = 5000;
                        break;
                    }
                    // valve or fire task
                    case 2:
                    case 4:
                    case 5:
                    case 8:
                    {
                        uint8 uiIndex = urand(0, 1);
                        DoScriptText(aStillAtItMachines[uiIndex].iText, m_creature);
                        m_uiTaskOwnerEntry = aStillAtItMachines[uiIndex].uiOwnerEntry;

                        m_uiTaskTimer = 0;
                        m_uiActionTimer = 5000;
                        break;
                    }
                    // complete event
                    case 9:
                        DoScriptText(SAY_DISTILLATION_COMPLETE, m_creature);
                        m_creature->GetMotionMaster()->MoveWaypoint();
                        if (GameObject* pPunch = GetClosestGameObjectWithEntry(m_creature, GO_THUNDERBREW_JUNGLE_PUNCH, 10.0f))
                        {
                            pPunch->SetRespawnTime(30);
                            pPunch->Refresh();
                        }
                        m_uiTaskTimer = 20000;
                        break;
                    case 10:
                        EnterEvadeMode();
                        m_uiTaskTimer = 0;
                        break;
                }
                ++m_uiTaskIndex;
            }
            else
                m_uiTaskTimer -= uiDiff;
        }

        // timer delay to allow player to complete the task
        if (m_uiActionTimer)
        {
            if (m_uiActionTimer <= uiDiff)
            {
                DoScriptText(SAY_DISTILLATION_FAIL, m_creature);
                EnterEvadeMode();
                m_uiActionTimer = 0;
            }
            else
                m_uiActionTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_tipsy_mcmanus(Creature* pCreature)
{
    return new npc_tipsy_mcmanusAI(pCreature);
}

bool GossipHello_npc_tipsy_mcmanus(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_ID_STILL_AT_IT) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TIPSY_MCMANUS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_READY, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_tipsy_mcmanus(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction != GOSSIP_ACTION_INFO_DEF + 1)
        return false;

    pPlayer->CLOSE_GOSSIP_MENU();
    pCreature->AI()->SendAIEvent(AI_EVENT_START_EVENT, pPlayer, pCreature);
    return true;
}

/*######
## npc_wants_fruit_credit
######*/

bool EffectDummyCreature_npc_wants_fruit_credit(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if ((uiSpellId == SPELL_TOSS_ORANGE || uiSpellId == SPELL_TOSS_BANANA || uiSpellId == SPELL_TOSS_PAPAYA) && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCaster->GetTypeId() == TYPEID_PLAYER && ((Player*)pCaster)->GetQuestStatus(QUEST_ID_STILL_AT_IT) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* pTipsyMcmanus = GetClosestCreatureWithEntry(pCaster, NPC_TIPSY_MCMANUS, 2 * INTERACTION_DISTANCE))
            {
                pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pTipsyMcmanus, pCreatureTarget->GetEntry());
                return true;
            }
        }
    }
    return false;
}

/*######
## go_quest_still_at_it_credit
######*/

bool GOUse_go_quest_still_at_it_credit(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_ID_STILL_AT_IT) != QUEST_STATUS_INCOMPLETE)
        return true;

    if (Creature* pTipsyMcmanus = GetClosestCreatureWithEntry(pPlayer, NPC_TIPSY_MCMANUS, 2 * INTERACTION_DISTANCE))
        pTipsyMcmanus->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, pPlayer, pTipsyMcmanus, pGo->GetEntry());

    return false;
}

// 51330 - Shoot RJR
struct ShootRJR : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        if (!spell->m_targets.getUnitTargetGuid().IsCreatureOrVehicle() || spell->m_targets.getUnitTargetGuid().GetEntry() != 28054) // Lucky Wilhelm
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void OnSpellCastResultOverride(SpellCastResult& result, uint32& param1, uint32& param2) const override
    {
        if (result == SPELL_FAILED_BAD_TARGETS)
        {
            result = SPELL_FAILED_CUSTOM_ERROR;
            param1 = SPELL_FAILED_CUSTOM_ERROR_27;
        }
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        // guessed chances
        if (roll_chance_i(75))
            caster->CastSpell(target, roll_chance_i(25) ? 51366 : 51332, TRIGGERED_OLD_TRIGGERED);
        else
            caster->CastSpell(target, 51331, TRIGGERED_OLD_TRIGGERED);
    }
};

// 45472 - Parachute
struct ParachutePeriodicDummy : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        if (aura->GetTarget()->IsFalling())
        {
            aura->GetTarget()->CastSpell(nullptr, aura->GetAmount(), TRIGGERED_OLD_TRIGGERED);
            aura->GetTarget()->RemoveAurasDueToSpell(aura->GetId());
        }
    }
};

enum DrostanQuest
{
    QUEST_THE_GREAT_HUNTERS_CHALLENGE   = 12592,
};

// 52546 - Initiate Kill Check
struct InitiateKillCheck : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Player* killer = nullptr;
        if (spell->GetUnitTarget()->IsPlayer())
            killer = static_cast<Player*>(spell->GetUnitTarget());
        if (spell->GetUnitTarget()->IsPlayerControlled())
            killer = const_cast<Player*>(spell->GetUnitTarget()->GetControllingPlayer());
        if (!killer)
            return;

        uint32 questProgress = killer->GetReqKillOrCastCurrentCount(QUEST_THE_GREAT_HUNTERS_CHALLENGE, NPC_TIPSY_MCMANUS); // yes this is correct
        switch (questProgress)
        {
            case 6:
            case 11:
            case 21:
            case 28:
            case 35:
            case 41:
            case 49:
            case 56:
                // summon drostan
            {
                spell->GetUnitTarget()->CastSpell(nullptr, 52556, TRIGGERED_OLD_TRIGGERED); // spell 52559 should be used (replace when fixed) 
                break;
            }
        }
    }
};

// 52556 - Summon Drostan
struct SummonDrostan : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        Player* caster = dynamic_cast<Player*>(spell->GetCaster());
        if (!caster)
            return;

        uint32 questProgress = caster->GetReqKillOrCastCurrentCount(QUEST_THE_GREAT_HUNTERS_CHALLENGE, NPC_TIPSY_MCMANUS);
        switch (questProgress)
        {
            case 6:
                spell->GetCaster()->CastSpell(nullptr, 52585, TRIGGERED_OLD_TRIGGERED);
                break;
            case 11:
                spell->GetCaster()->CastSpell(nullptr, 52725, TRIGGERED_OLD_TRIGGERED);
                break;
            case 21:
                spell->GetCaster()->CastSpell(nullptr, 52726, TRIGGERED_OLD_TRIGGERED);
                break;
            case 28:
                spell->GetCaster()->CastSpell(nullptr, 52727, TRIGGERED_OLD_TRIGGERED);
                break;
            case 35:
                spell->GetCaster()->CastSpell(nullptr, 52728, TRIGGERED_OLD_TRIGGERED);
                break;
            case 41:
                spell->GetCaster()->CastSpell(nullptr, 52729, TRIGGERED_OLD_TRIGGERED);
                break;
            case 49:
                spell->GetCaster()->CastSpell(nullptr, 52730, TRIGGERED_OLD_TRIGGERED);
                break;
            case 56:
                spell->GetCaster()->CastSpell(nullptr, 52731, TRIGGERED_OLD_TRIGGERED);
                break;
        }
    }
};

// 51962 - Offer Jungle Punch
struct OfferJunglePunch : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        // can be cast only on these targets
        if (target && target->GetEntry() != 27986 && target->GetEntry() != 28047 && target->GetEntry() != 28568)
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }
};

// 51759 - Summon and Mount Stampy
struct SummonAndMountStampy : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->SelectLevel(spell->GetCaster()->GetLevel());
    }
};

// 52218 - Vic's Flying Machine
struct VicsFlyingMachine : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->SelectLevel(spell->GetCaster()->GetLevel());
    }
};

// 51186 - Summon Goregek the Bristlepine Hunter
struct SummonGoregekTheBristlepineHunter : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->SelectLevel(spell->GetCaster()->GetLevel());
    }
};

// 51188 - Summon Dajik the Wasp Hunter
struct SummonDajikTheWaspHunter : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->SelectLevel(spell->GetCaster()->GetLevel());
    }
};

// 51189 - Summon Zepik the Gorloc Hunter
struct SummonZepikTheGorlocHunter : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->SelectLevel(spell->GetCaster()->GetLevel());
        summon->SetDefaultGossipMenuId(summon->GetCreatureInfo()->GossipMenuId);
    }
};

// 51257 - Summon Possessed Crocolisk
struct SummonPossessedCrocolisk : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->SelectLevel(spell->GetCaster()->GetLevel());
    }
};

void AddSC_sholazar_basin()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_helice";
    pNewScript->GetAI = &GetAI_npc_helice;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_helice;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_injured_rainspeaker";
    pNewScript->GetAI = &GetAI_npc_injured_rainspeaker;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_injured_rainspeaker;
    // pNewScript->pGossipHello = &GossipHello_npc_injured_rainspeaker;
    // pNewScript->pGossipSelect = &GossipSelect_npc_injured_rainspeaker;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tipsy_mcmanus";
    pNewScript->GetAI = &GetAI_npc_tipsy_mcmanus;
    pNewScript->pGossipHello = &GossipHello_npc_tipsy_mcmanus;
    pNewScript->pGossipSelect = &GossipSelect_npc_tipsy_mcmanus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wants_fruit_credit";
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_wants_fruit_credit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_quest_still_at_it_credit";
    pNewScript->pGOUse = &GOUse_go_quest_still_at_it_credit;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ShootRJR>("spell_shoot_rjr");
    RegisterSpellScript<ParachutePeriodicDummy>("spell_parachute_periodic_dummy");
    RegisterSpellScript<InitiateKillCheck>("spell_initiate_kill_check");
    RegisterSpellScript<SummonDrostan>("spell_summon_drostan");
    RegisterSpellScript<OfferJunglePunch>("spell_offer_jungle_punch");
    RegisterSpellScript<SummonAndMountStampy>("spell_summon_and_mount_stampy");
    RegisterSpellScript<VicsFlyingMachine>("spell_vics_flying_machine");
    RegisterSpellScript<SummonGoregekTheBristlepineHunter>("spell_summon_goregek_the_bristlepine_hunter");
    RegisterSpellScript<SummonDajikTheWaspHunter>("spell_summon_dajik_the_wasp_hunter");
    RegisterSpellScript<SummonZepikTheGorlocHunter>("spell_summon_zepik_the_gorloc_hunter");
    RegisterSpellScript<SummonPossessedCrocolisk>("spell_summon_possessed_crocolisk");
}

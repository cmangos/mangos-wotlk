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
SDName: Borean_Tundra
SD%Complete: 100
SDComment: Quest support: 11570, 11590, 11608, 11664, 11673, 11728, 11865, 11881, 11889, 11897, 11919, 11940.
SDCategory: Borean Tundra
EndScriptData */

/* ContentData
npc_nesingwary_trapper
npc_sinkhole_kill_credit
npc_lurgglbr
npc_beryl_sorcerer
npc_captured_beryl_sorcerer
npc_nexus_drake_hatchling
npc_scourged_flamespitter
npc_bonker_togglevolt
npc_jenny
npc_mootoo_the_younger
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "TemporarySummon.h"
#include "follower_ai.h"

/*######
## npc_nesingwary_trapper
######*/

enum
{
    NPC_NESINGWARY_TRAPPER  = 25835,
    GO_QUALITY_FUR          = 187983,

    SAY_PHRASE_1            = -1000599,
    SAY_PHRASE_2            = -1000600,
    SAY_PHRASE_3            = -1000601,
    SAY_PHRASE_4            = -1000602
};

struct npc_nesingwary_trapperAI : public ScriptedAI
{
    npc_nesingwary_trapperAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint8 m_uiPhase;
    uint32 m_uiPhaseTimer;
    ObjectGuid m_playerGuid;
    ObjectGuid m_trapGuid;

    void Reset() override
    {
        m_uiPhase = 0;
        m_uiPhaseTimer = 0;
        m_playerGuid.Clear();
        m_trapGuid.Clear();
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_uiPhase && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 20.0f))
        {
            m_uiPhase = 1;
            m_uiPhaseTimer = 1000;
            m_playerGuid = pWho->GetObjectGuid();

            if (m_creature->IsTemporarySummon())
            {
                // Get the summoner trap
                if (GameObject* pTrap = m_creature->GetMap()->GetGameObject(((TemporarySummon*)m_creature)->GetSummonerGuid()))
                    m_trapGuid = pTrap->GetObjectGuid();
            }
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || !uiPointId)
            return;

        if (GameObject* pTrap = m_creature->GetMap()->GetGameObject(m_trapGuid))
        {
            // respawn the Quality Fur
            if (GameObject* pGoFur = GetClosestGameObjectWithEntry(pTrap, GO_QUALITY_FUR, INTERACTION_DISTANCE))
            {
                if (!pGoFur->isSpawned())
                {
                    pGoFur->SetRespawnTime(10);
                    pGoFur->Refresh();
                }
            }
        }

        m_uiPhaseTimer = 2000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->getVictim() && m_uiPhaseTimer)
        {
            if (m_uiPhaseTimer <= uiDiff)
            {
                switch (m_uiPhase)
                {
                    case 1:
                        if (GameObject* pTrap = m_creature->GetMap()->GetGameObject(m_trapGuid))
                        {
                            float fX, fY, fZ;
                            pTrap->GetContactPoint(m_creature, fX, fY, fZ);

                            m_creature->SetWalk(false);
                            m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                        }
                        m_uiPhaseTimer = 0;
                        break;
                    case 2:
                        switch (urand(0, 3))
                        {
                            case 0: DoScriptText(SAY_PHRASE_1, m_creature); break;
                            case 1: DoScriptText(SAY_PHRASE_2, m_creature); break;
                            case 2: DoScriptText(SAY_PHRASE_3, m_creature); break;
                            case 3: DoScriptText(SAY_PHRASE_4, m_creature); break;
                        }
                        m_creature->HandleEmote(EMOTE_ONESHOT_LOOT);
                        m_uiPhaseTimer = 3000;
                        break;
                    case 3:
                        if (GameObject* pTrap = m_creature->GetMap()->GetGameObject(m_trapGuid))
                        {
                            pTrap->Use(m_creature);

                            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                            {
                                if (pPlayer->isAlive())
                                    pPlayer->KilledMonsterCredit(m_creature->GetEntry());
                            }
                        }
                        m_uiPhaseTimer = 0;
                        break;
                }
                ++m_uiPhase;
            }
            else
                m_uiPhaseTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_nesingwary_trapper(Creature* pCreature)
{
    return new npc_nesingwary_trapperAI(pCreature);
}

/*#####
# npc_oil_stained_wolf
#####*/

enum
{
    SPELL_THROW_WOLF_BAIT           = 53326,
    SPELL_PLACE_WOLF_BAIT           = 46072,                // doesn't appear to be used for anything
    SPELL_HAS_EATEN                 = 46073,
    SPELL_SUMMON_DROPPINGS          = 46075,

    FACTION_MONSTER                 = 634,

    POINT_DEST                      = 1
};

struct npc_oil_stained_wolfAI : public ScriptedAI
{
    npc_oil_stained_wolfAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_bCanCrapInPublic;
    uint32 m_uiPooTimer;

    void Reset() override
    {
        m_bCanCrapInPublic = false;
        m_uiPooTimer = 0;
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_DEST)
        {
            DoCastSpellIfCan(m_creature, SPELL_HAS_EATEN);
            m_uiPooTimer = 4000;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_uiPooTimer)
            {
                if (m_uiPooTimer <= uiDiff)
                {
                    if (m_bCanCrapInPublic)
                    {
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_DROPPINGS);
                        m_creature->GetMotionMaster()->Clear();
                        Reset();
                    }
                    else
                    {
                        m_creature->HandleEmote(EMOTE_ONESHOT_BATTLEROAR);
                        m_bCanCrapInPublic = true;
                        m_uiPooTimer = 3000;
                    }
                }
                else
                    m_uiPooTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_oil_stained_wolf(Creature* pCreature)
{
    return new npc_oil_stained_wolfAI(pCreature);
}

bool EffectDummyCreature_npc_oil_stained_wolf(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_THROW_WOLF_BAIT)
    {
        if (uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->getFaction() != FACTION_MONSTER && !pCreatureTarget->HasAura(SPELL_HAS_EATEN))
        {
            pCreatureTarget->SetFactionTemporary(FACTION_MONSTER);
            pCreatureTarget->SetWalk(false);

            pCreatureTarget->GetMotionMaster()->MoveIdle();

            float fX, fY, fZ;
            pCaster->GetContactPoint(pCreatureTarget, fX, fY, fZ, CONTACT_DISTANCE);
            pCreatureTarget->GetMotionMaster()->MovePoint(POINT_DEST, fX, fY, fZ);
            return true;
        }
    }

    return false;
}

bool EffectAuraDummy_npc_oil_stained_wolf(const Aura* pAura, bool bApply)
{
    if (pAura->GetId() == SPELL_HAS_EATEN)
    {
        if (pAura->GetEffIndex() != EFFECT_INDEX_0)
            return false;

        if (bApply)
        {
            pAura->GetTarget()->HandleEmote(EMOTE_ONESHOT_CUSTOMSPELL01);
        }
        else
        {
            Creature* pCreature = (Creature*)pAura->GetTarget();
            pCreature->setFaction(pCreature->GetCreatureInfo()->FactionAlliance);
        }

        return true;
    }

    return false;
}

/*#####
# npc_sinkhole_kill_credit
#####*/

enum
{
    SPELL_SUMMON_EXPLOSIVES_CART_FIRE   = 46799,
    SPELL_SUMMON_SCOURGE_BURROWER       = 46800,
    SPELL_COSMETIC_HUGE_EXPLOSION       = 46225,
    SPELL_CANNON_FIRE                   = 42445,
};

struct npc_sinkhole_kill_creditAI : public ScriptedAI
{
    npc_sinkhole_kill_creditAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_cartGuid;
    ObjectGuid m_wormGuid;
    uint32 m_uiCartTimer;
    uint32 m_uiCartPhase;

    void Reset() override
    {
        m_cartGuid.Clear();
        m_wormGuid.Clear();
        m_uiCartTimer = 2000;
        m_uiCartPhase = 0;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        m_wormGuid = pSummoned->GetObjectGuid();
    }

    void JustSummoned(GameObject* pGo) override
    {
        // Go is not really needed, but ok to use as a check point so only one "event" can be processed at a time
        if (m_cartGuid)
            return;

        // Expecting summoned from mangos dummy effect 46797
        m_cartGuid = pGo->GetObjectGuid();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_cartGuid)
        {
            if (m_uiCartTimer <= uiDiff)
            {
                switch (m_uiCartPhase)
                {
                    case 0:
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_EXPLOSIVES_CART_FIRE);
                        m_uiCartTimer = 4000;
                        break;
                    case 1:
                        // Unclear if these should be in a dummy effect or not.
                        // The order of spells are correct though.
                        DoCastSpellIfCan(m_creature, SPELL_COSMETIC_HUGE_EXPLOSION, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_CANNON_FIRE, CAST_TRIGGERED);
                        break;
                    case 2:
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SCOURGE_BURROWER);
                        m_uiCartTimer = 2000;
                        break;
                    case 3:
                        if (Creature* pWorm = m_creature->GetMap()->GetCreature(m_wormGuid))
                        {
                            pWorm->SetDeathState(JUST_DIED);
                            pWorm->SetHealth(0);
                        }
                        m_uiCartTimer = 10000;
                        break;
                    case 4:
                        if (Creature* pWorm = m_creature->GetMap()->GetCreature(m_wormGuid))
                            pWorm->RemoveCorpse();

                        Reset();
                        return;
                }

                ++m_uiCartPhase;
            }
            else
                m_uiCartTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_sinkhole_kill_credit(Creature* pCreature)
{
    return new npc_sinkhole_kill_creditAI(pCreature);
}

/*######
## npc_lurgglbr
######*/

enum
{
    QUEST_ESCAPE_FROM_WINTERFIN_CAVERNS = 11570,
    GO_CAGE                             = 187369,

    SAY_START_1                         = -1000575,
    SAY_START_2                         = -1000576,
    SAY_END_1                           = -1000577,
    SAY_END_2                           = -1000578
};

struct npc_lurgglbrAI : public npc_escortAI
{
    npc_lurgglbrAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_uiSayTimer = 0;
        m_uiSpeech = 0;
        Reset();
    }

    uint32 m_uiSayTimer;
    uint8 m_uiSpeech;

    void Reset() override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiSayTimer = 0;
            m_uiSpeech = 0;
        }
    }

    void JustStartedEscort() override
    {
        if (GameObject* pCage = GetClosestGameObjectWithEntry(m_creature, GO_CAGE, INTERACTION_DISTANCE))
        {
            if (pCage->GetGoState() == GO_STATE_READY)
                pCage->Use(m_creature);
        }
    }

    void WaypointStart(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 1:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_START_2, m_creature, pPlayer);

                // Cage actually closes here, however it's normally determined by GO template and auto close time

                break;
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 0:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    m_creature->SetFacingToObject(pPlayer);
                    DoScriptText(SAY_START_1, m_creature, pPlayer);
                }
                break;
            case 25:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_END_1, m_creature, pPlayer);
                    m_uiSayTimer = 3000;
                }
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_uiSayTimer)
            {
                if (m_uiSayTimer <= uiDiff)
                {
                    Player* pPlayer = GetPlayerForEscort();

                    if (!pPlayer)
                    {
                        m_uiSayTimer = 0;
                        return;
                    }

                    m_creature->SetFacingToObject(pPlayer);

                    switch (m_uiSpeech)
                    {
                        case 0:
                            DoScriptText(SAY_END_2, m_creature, pPlayer);
                            m_uiSayTimer = 3000;
                            break;
                        case 1:
                            pPlayer->GroupEventHappens(QUEST_ESCAPE_FROM_WINTERFIN_CAVERNS, m_creature);
                            m_uiSayTimer = 0;
                            break;
                    }

                    ++m_uiSpeech;
                }
                else
                    m_uiSayTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_lurgglbr(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPE_FROM_WINTERFIN_CAVERNS)
    {
        if (npc_lurgglbrAI* pEscortAI = dynamic_cast<npc_lurgglbrAI*>(pCreature->AI()))
        {
            pCreature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);
            pEscortAI->Start(false, pPlayer, pQuest);
        }
    }
    return true;
}

CreatureAI* GetAI_npc_lurgglbr(Creature* pCreature)
{
    return new npc_lurgglbrAI(pCreature);
}

/*#####
# npc_beryl_sorcerer
#####*/

enum
{
    SPELL_ARCANE_CHAINS                 = 45611,
    SPELL_ARCANE_CHAINS_CHANNEL         = 45630,
    SPELL_SUMMON_CHAINS_CHARACTER       = 45625,                // triggers 45626
    // SPELL_ENSLAVED_ARCANE_CHAINS     = 45632,                // chain visual - purpose unk, probably used on quest end

    NPC_BERYL_SORCERER                  = 25316,
    NPC_CAPTURED_BERYL_SORCERER         = 25474,
};

bool EffectAuraDummy_npc_beryl_sorcerer(const Aura* pAura, bool bApply)
{
    if (pAura->GetId() == SPELL_ARCANE_CHAINS)
    {
        if (pAura->GetEffIndex() != EFFECT_INDEX_0 || !bApply)
            return false;

        Creature* pCreature = (Creature*)pAura->GetTarget();
        Unit* pCaster = pAura->GetCaster();
        if (!pCreature || !pCaster || pCaster->GetTypeId() != TYPEID_PLAYER || pCreature->GetEntry() != NPC_BERYL_SORCERER)
            return false;

        // only for wounded creatures
        if (pCreature->GetHealthPercent() > 30.0f)
            return false;

        // spawn the captured sorcerer, apply dummy aura on the summoned and despawn
        pCaster->CastSpell(pCreature, SPELL_SUMMON_CHAINS_CHARACTER, TRIGGERED_OLD_TRIGGERED);
        pCaster->CastSpell(pCaster, SPELL_ARCANE_CHAINS_CHANNEL, TRIGGERED_OLD_TRIGGERED);
        pCreature->ForcedDespawn();
        return true;
    }

    return false;
}

/*#####
# npc_captured_beryl_sorcerer
#####*/

bool EffectAuraDummy_npc_captured_beryl_sorcerer(const Aura* pAura, bool bApply)
{
    if (pAura->GetId() == SPELL_ARCANE_CHAINS_CHANNEL)
    {
        if (pAura->GetEffIndex() != EFFECT_INDEX_0 || !bApply)
            return false;

        Creature* pCreature = (Creature*)pAura->GetTarget();
        Unit* pCaster = pAura->GetCaster();
        if (!pCreature || !pCaster || pCaster->GetTypeId() != TYPEID_PLAYER || pCreature->GetEntry() != NPC_CAPTURED_BERYL_SORCERER)
            return false;

        // follow the caster
        ((Player*)pCaster)->KilledMonsterCredit(NPC_CAPTURED_BERYL_SORCERER);
        pCreature->GetMotionMaster()->MoveFollow(pCaster, pCreature->GetDistance(pCaster), M_PI_F - pCreature->GetAngle(pCaster));
        return true;
    }

    return false;
}

/*######
## npc_nexus_drake_hatchling
######*/

enum
{
    // combat spells
    SPELL_INTANGIBLE_PRESENCE           = 36513,
    SPELL_NETHERBREATH                  = 36631,

    // quest start spells
    SPELL_DRAKE_HARPOON                 = 46607,                    // initial spell
    SPELL_RED_DRAGONBLOOD               = 46620,                    // applied by aura 46607
    SPELL_CAPTURE_TRIGGER               = 46673,                    // notify the drake that it was captured; triggered by aura 46620 expire
    SPELL_SUBDUED                       = 46675,                    // visual spell; triggered by spell 46673
    SPELL_DRAKE_HATCHLING_SUBDUED       = 46691,                    // inform player that drake has been captured; triggered by spell 46673
    SPELL_DRAKE_VOMIT_PERIODIC          = 46678,                    // visual spell; triggered by spell 46673

    // quest completion spells
    SPELL_DRAKE_TURN_IN                 = 46696,                    // notify the drake that quest is finised
    SPELL_STRIP_AURAS                   = 46693,                    // remove all quest auras
    SPELL_DRAKE_COMPLETION_PING         = 46702,
    SPELL_RAELORASZ_FIREBALL            = 46704,
    SPELL_COMPLETE_IMMOLATION           = 46703,

    NPC_RAELORASZ                       = 26117,                    // quest giver / taker
    NPC_NEXUS_DRAKE_HATCHLING           = 26127,
    NPC_COLDARRA_DRAKE_HUNT_INVISMAN    = 26175,                    // quest credit

    QUEST_DRAKE_HUNT                    = 11919,
    QUEST_DRAKE_HUNT_DAILY              = 11940,

    FACTION_FRIENDLY                    = 35,
};

struct npc_nexus_drake_hatchlingAI : public FollowerAI
{
    npc_nexus_drake_hatchlingAI(Creature* pCreature) : FollowerAI(pCreature) { Reset(); }

    uint32 m_uiNetherbreathTimer;
    uint32 m_uiPresenceTimer;
    uint32 m_uiSubduedTimer;

    void Reset() override
    {
        m_uiNetherbreathTimer = urand(2000, 4000);
        m_uiPresenceTimer     = urand(15000, 17000);
        m_uiSubduedTimer      = 0;
    }

    void EnterEvadeMode() override
    {
        // force check for evading when the faction is changed
        if (m_uiSubduedTimer)
            return;

        FollowerAI::EnterEvadeMode();
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        FollowerAI::MoveInLineOfSight(pWho);

        if (!m_creature->HasAura(SPELL_SUBDUED) || m_creature->getVictim())
            return;

        if (pWho->GetEntry() == NPC_COLDARRA_DRAKE_HUNT_INVISMAN && m_creature->IsWithinDistInMap(pWho, 20.0f))
        {
            Player* pPlayer = GetLeaderForFollower();
            if (!pPlayer || !pPlayer->HasAura(SPELL_DRAKE_HATCHLING_SUBDUED))
                return;

            pWho->CastSpell(pPlayer, SPELL_STRIP_AURAS, TRIGGERED_OLD_TRIGGERED);

            // give kill credit, mark the follow as completed and start the final event
            pPlayer->KilledMonsterCredit(NPC_COLDARRA_DRAKE_HUNT_INVISMAN);
            pPlayer->CastSpell(m_creature, SPELL_DRAKE_TURN_IN, TRIGGERED_OLD_TRIGGERED);
            SetFollowComplete(true);
        }
    }

    void JustRespawned() override
    {
        // reset stand state if required
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);

        FollowerAI::JustRespawned();
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // start following
        if (eventType == AI_EVENT_START_EVENT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            StartFollow((Player*)pInvoker);
            m_uiSubduedTimer = 3 * MINUTE * IN_MILLISECONDS;
        }
        // timeout; quest failed
        else if (eventType == AI_EVENT_CUSTOM_A)
        {
            // check if the quest isn't already completed
            if (!HasFollowState(STATE_FOLLOW_COMPLETE))
            {
                // force reset
                JustRespawned();
                ScriptedAI::EnterEvadeMode();
            }
        }
    }

    void UpdateFollowerAI(const uint32 uiDiff)
    {
        if (m_uiSubduedTimer)
        {
            if (m_uiSubduedTimer <= uiDiff)
                m_uiSubduedTimer = 0;
            else
                m_uiSubduedTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiNetherbreathTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_NETHERBREATH) == CAST_OK)
                m_uiNetherbreathTimer = urand(17000, 20000);
        }
        else
            m_uiNetherbreathTimer -= uiDiff;

        if (m_uiPresenceTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_INTANGIBLE_PRESENCE) == CAST_OK)
                m_uiPresenceTimer = urand(18000, 20000);
        }
        else
            m_uiPresenceTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_nexus_drake_hatchling(Creature* pCreature)
{
    return new npc_nexus_drake_hatchlingAI(pCreature);
}

bool EffectAuraDummy_npc_nexus_drake_hatchling(const Aura* pAura, bool bApply)
{
    if (pAura->GetId() == SPELL_DRAKE_HARPOON)
    {
        if (pAura->GetEffIndex() != EFFECT_INDEX_0 || !bApply)
            return false;

        Creature* pCreature = (Creature*)pAura->GetTarget();
        Unit* pCaster = pAura->GetCaster();
        if (!pCreature || !pCaster || pCaster->GetTypeId() != TYPEID_PLAYER || pCreature->GetEntry() != NPC_NEXUS_DRAKE_HATCHLING)
            return false;

        // check if drake is already doing the quest
        if (pCreature->HasAura(SPELL_RED_DRAGONBLOOD) || pCreature->HasAura(SPELL_SUBDUED))
            return false;

        pCaster->CastSpell(pCreature, SPELL_RED_DRAGONBLOOD, TRIGGERED_OLD_TRIGGERED);
        return true;
    }
    else if (pAura->GetId() == SPELL_RED_DRAGONBLOOD && pAura->GetEffIndex() == EFFECT_INDEX_0)
    {
        Creature* pCreature = (Creature*)pAura->GetTarget();
        Unit* pCaster = pAura->GetCaster();
        if (!pCreature || !pCaster || pCaster->GetTypeId() != TYPEID_PLAYER || pCreature->GetEntry() != NPC_NEXUS_DRAKE_HATCHLING)
            return false;

        // start attacking on apply and capture on aura expire
        if (bApply)
            pCreature->AI()->AttackStart(pCaster);
        else
            pCaster->CastSpell(pCreature, SPELL_CAPTURE_TRIGGER, TRIGGERED_OLD_TRIGGERED);

        return true;
    }
    else if (pAura->GetId() == SPELL_SUBDUED && pAura->GetEffIndex() == EFFECT_INDEX_0 && !bApply)
    {
        Creature* pCreature = (Creature*)pAura->GetTarget();
        if (!pCreature || pCreature->GetEntry() != NPC_NEXUS_DRAKE_HATCHLING)
            return false;

        // aura expired - evade
        pCreature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCreature, pCreature);
        return true;
    }

    return false;
}

bool EffectDummyCreature_npc_nexus_drake_hatchling(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_CAPTURE_TRIGGER && uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->GetEntry() == NPC_NEXUS_DRAKE_HATCHLING)
    {
        if (pCaster->GetTypeId() != TYPEID_PLAYER)
            return true;

        if (pCaster->HasAura(SPELL_DRAKE_HATCHLING_SUBDUED) || pCreatureTarget->HasAura(SPELL_SUBDUED))
            return true;

        Player* pPlayer = (Player*)pCaster;
        if (!pPlayer)
            return true;

        // check the quest
        if (pPlayer->GetQuestStatus(QUEST_DRAKE_HUNT) != QUEST_STATUS_INCOMPLETE && pPlayer->GetQuestStatus(QUEST_DRAKE_HUNT_DAILY) != QUEST_STATUS_INCOMPLETE)
            return true;

        // evade and set friendly and start following
        pCreatureTarget->SetFactionTemporary(FACTION_FRIENDLY, TEMPFACTION_RESTORE_REACH_HOME | TEMPFACTION_RESTORE_RESPAWN);
        pCreatureTarget->DeleteThreatList();
        pCreatureTarget->CombatStop(true);
        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_START_EVENT, pCaster, pCreatureTarget);

        // cast visual spells
        pCreatureTarget->CastSpell(pCreatureTarget, SPELL_DRAKE_VOMIT_PERIODIC, TRIGGERED_OLD_TRIGGERED);
        pCreatureTarget->CastSpell(pCreatureTarget, SPELL_SUBDUED, TRIGGERED_OLD_TRIGGERED);
        pCreatureTarget->CastSpell(pCaster, SPELL_DRAKE_HATCHLING_SUBDUED, TRIGGERED_OLD_TRIGGERED);

        return true;
    }
    else if (uiSpellId == SPELL_DRAKE_TURN_IN && uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->GetEntry() == NPC_NEXUS_DRAKE_HATCHLING)
    {
        if (Creature* pRaelorasz = GetClosestCreatureWithEntry(pCreatureTarget, NPC_RAELORASZ, 30.0f))
        {
            // Inform Raelorasz and move in front of him
            pCreatureTarget->CastSpell(pRaelorasz, SPELL_DRAKE_COMPLETION_PING, TRIGGERED_OLD_TRIGGERED);

            float fX, fY, fZ;
            pRaelorasz->GetContactPoint(pCreatureTarget, fX, fY, fZ, CONTACT_DISTANCE);
            pCreatureTarget->GetMotionMaster()->Clear(true, true);
            pCreatureTarget->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
            return true;
        }
    }
    else if (uiSpellId == SPELL_RAELORASZ_FIREBALL && uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->GetEntry() == NPC_NEXUS_DRAKE_HATCHLING)
    {
        pCreatureTarget->CastSpell(pCreatureTarget, SPELL_COMPLETE_IMMOLATION, TRIGGERED_OLD_TRIGGERED);
        pCreatureTarget->SetStandState(UNIT_STAND_STATE_DEAD);
        pCreatureTarget->ForcedDespawn(10000);

        return true;
    }

    return false;
}

/*#####
# npc_scourged_flamespitter
#####*/

enum
{
    SPELL_REINFORCED_NET                = 46361,
    SPELL_NET                           = 47021,

    SPELL_INCINERATE_COSMETIC           = 45863,
    SPELL_INCINERATE                    = 32707,

    NPC_FLAMESPITTER                    = 25582,
};

struct npc_scourged_flamespitterAI : public ScriptedAI
{
    npc_scourged_flamespitterAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiIncinerateTimer;
    uint32 m_uiNetExpireTimer;

    void Reset() override
    {
        m_uiIncinerateTimer = urand(1000, 2000);
        m_uiNetExpireTimer = 0;
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, false))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho, 10.0f);
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        if (DoCastSpellIfCan(m_creature, SPELL_NET) == CAST_OK)
            m_uiNetExpireTimer = 20000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiNetExpireTimer)
        {
            if (m_uiNetExpireTimer <= uiDiff)
            {
                // evade when the net root has expired
                if (!m_creature->getVictim())
                    EnterEvadeMode();

                m_uiNetExpireTimer = 0;
            }
            else
                m_uiNetExpireTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            // incinerate visual on OOC timer, unless creature is rooted
            if (!m_uiNetExpireTimer)
            {
                if (m_uiIncinerateTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_INCINERATE_COSMETIC) == CAST_OK)
                        m_uiIncinerateTimer = urand(3000, 5000);
                }
                else
                    m_uiIncinerateTimer -= uiDiff;
            }

            return;
        }

        if (m_uiIncinerateTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_INCINERATE) == CAST_OK)
                m_uiIncinerateTimer = urand(3000, 5000);
        }
        else
            m_uiIncinerateTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_scourged_flamespitter(Creature* pCreature)
{
    return new npc_scourged_flamespitterAI(pCreature);
}

bool EffectAuraDummy_npc_scourged_flamespitter(const Aura* pAura, bool bApply)
{
    if (pAura->GetId() == SPELL_REINFORCED_NET && pAura->GetEffIndex() == EFFECT_INDEX_0 && bApply)
    {
        Creature* pCreature = (Creature*)pAura->GetTarget();
        Unit* pCaster = pAura->GetCaster();
        if (!pCreature || !pCaster || pCaster->GetTypeId() != TYPEID_PLAYER || pCreature->GetEntry() != NPC_FLAMESPITTER)
            return false;

        // move the flamespitter to the ground level
        pCreature->GetMotionMaster()->Clear();
        pCreature->SetWalk(false);

        float fGroundZ = pCreature->GetMap()->GetHeight(pCreature->GetPhaseMask(), pCreature->GetPositionX(), pCreature->GetPositionY(), pCreature->GetPositionZ());
        pCreature->GetMotionMaster()->MovePoint(1, pCreature->GetPositionX(), pCreature->GetPositionY(), fGroundZ);
        return true;
    }

    return false;
}

/*#####
## npc_bonker_togglevolt
#####*/

enum
{
    SAY_BONKER_START            = -1001013,
    SAY_BONKER_GO               = -1001014,
    SAY_BONKER_AGGRO            = -1001015,
    SAY_BONKER_LEFT             = -1001016,
    SAY_BONKER_COMPLETE         = -1001017,

    QUEST_ID_GET_ME_OUTA_HERE   = 11673,
};

struct npc_bonker_togglevoltAI : public npc_escortAI
{
    npc_bonker_togglevoltAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() override { }

    void Aggro(Unit* /*pWho*/) override
    {
        if (urand(0, 1))
            DoScriptText(SAY_BONKER_AGGRO, m_creature);
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 0:
                DoScriptText(SAY_BONKER_START, m_creature);
                break;
            case 1:
                DoScriptText(SAY_BONKER_GO, m_creature);
                // WORKAROUND ALERT - temp ignore pathfinding until we pass the pool
                // creature cannont find a proper swimming path in this area, so ignore pathfinding for the moment
                m_creature->addUnitState(UNIT_STAT_IGNORE_PATHFINDING);
                break;
            case 3:
                DoScriptText(SAY_BONKER_LEFT, m_creature);
                // WORKAROUND END - resume pathfinding
                m_creature->clearUnitState(UNIT_STAT_IGNORE_PATHFINDING);
                break;
            case 32:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    pPlayer->GroupEventHappens(QUEST_ID_GET_ME_OUTA_HERE, m_creature);
                    DoScriptText(SAY_BONKER_COMPLETE, m_creature, pPlayer);
                }
                break;
        }
    }
};

CreatureAI* GetAI_npc_bonker_togglevolt(Creature* pCreature)
{
    return new npc_bonker_togglevoltAI(pCreature);
}

bool QuestAccept_npc_bonker_togglevolt(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_GET_ME_OUTA_HERE)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

/*######
## npc_jenny
######*/

enum
{
    SPELL_CREATES_CARRIED       = 46340,
    SPELL_DROP_CRATE            = 46342,
    SPELL_JENNY_CREDIT          = 46358,

    NPC_FEZZIX                  = 25849,

    QUEST_ID_LOADER_UP          = 11881,
};

struct npc_jennyAI : public FollowerAI
{
    npc_jennyAI(Creature* pCreature) : FollowerAI(pCreature)
    {
        m_bFollowStarted = false;
        m_bEventComplete = false;
        Reset();
    }

    bool m_bEventComplete;
    bool m_bFollowStarted;

    uint32 m_uiDropDelayTimer;

    void Reset() override
    {
        m_uiDropDelayTimer = 0;
    }

    void AttackedBy(Unit* pAttacker) override
    {
        if (!m_uiDropDelayTimer)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DROP_CRATE) == CAST_OK)
            {
                m_creature->RemoveAuraHolderFromStack(SPELL_CREATES_CARRIED);
                m_uiDropDelayTimer = 10000;

                // check if all crates are dropped
                if (!m_creature->HasAura(SPELL_CREATES_CARRIED))
                {
                    FollowerAI::JustDied(pAttacker);
                    m_creature->ForcedDespawn();
                }
            }
        }
    }

    void AttackStart(Unit* pWho) override { }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_bEventComplete)
            return;

        if (pWho->GetEntry() == NPC_FEZZIX && m_creature->IsWithinDistInMap(pWho, 10.0f))
        {
            if (DoCastSpellIfCan(m_creature, SPELL_JENNY_CREDIT) == CAST_OK)
            {
                SetFollowComplete(true);

                float fX, fY, fZ;
                pWho->GetContactPoint(m_creature, fX, fY, fZ);
                m_creature->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                m_creature->ForcedDespawn(15000);

                m_bEventComplete = true;
            }
        }
    }

    void UpdateFollowerAI(const uint32 uiDiff)
    {
        if (!m_bFollowStarted)
        {
            if (Player* pSummoner = m_creature->GetCharmerOrOwnerPlayerOrPlayerItself())
            {
                StartFollow(pSummoner, pSummoner->getFaction(), GetQuestTemplateStore(QUEST_ID_LOADER_UP));

                if (DoCastSpellIfCan(m_creature, SPELL_CREATES_CARRIED) == CAST_OK)
                    m_bFollowStarted = true;
            }
        }

        if (m_uiDropDelayTimer)
        {
            if (m_uiDropDelayTimer <= uiDiff)
                m_uiDropDelayTimer = 0;
            else
                m_uiDropDelayTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }
};

CreatureAI* GetAI_npc_jenny(Creature* pCreature)
{
    return new npc_jennyAI(pCreature);
}

/*######
## npc_seaforium_depth_charge
######*/

enum
{
    SPELL_SEAFORIUM_DEPTH_EXPLOSION         = 45502,
};

// Note: Creature is summoned as a TotemAI in the core. This script is required in order to tweak the creature's behavior
struct npc_seaforium_depth_chargeAI : public Scripted_NoMovementAI
{
    npc_seaforium_depth_chargeAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint32 m_uiExplosionTimer;

    void Reset() override
    {
        m_uiExplosionTimer = 10000;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiExplosionTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SEAFORIUM_DEPTH_EXPLOSION) == CAST_OK)
            {
                m_creature->ForcedDespawn(1000);
                m_uiExplosionTimer = 10000;
            }
        }
        else
            m_uiExplosionTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_seaforium_depth_charge(Creature* pCreature)
{
    return new npc_seaforium_depth_chargeAI(pCreature);
}

/*######
## npc_mootoo_the_younger
######*/

enum
{
    SAY_MOOTOO_Y_START      = -1001226,
    SAY_1_MOOTOO_Y          = -1001227,
    SAY_2_MOOTOO_Y          = -1001228,
    SAY_3_MOOTOO_Y          = -1001229,
    SAY_4_MOOTOO_Y          = -1001230,
    SAY_5_MOOTOO_Y          = -1001231,
    SAY_6_MOOTOO_Y          = -1001232,
    SAY_7_MOOTOO_Y          = -1001233,
    SAY_8_MOOTOO_Y          = -1001234,
    SAY_9_MOOTOO_Y          = -1001237,
    SAY_CREDIT_MOOTOO_Y     = -1001235,
    SAY_1_ELDER_MOOTOO      = -1001236,
    SAY_2_ELDER_MOOTOO      = -1001238,

    NPC_ELDER_MOOTOO        = 25503,

    QUEST_ESCAPING_THE_MIST = 11664
};

struct npc_mootoo_the_youngerAI : public npc_escortAI
{
    npc_mootoo_the_youngerAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
            m_creature->SetStandState(UNIT_STAND_STATE_SIT);
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 1:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_MOOTOO_Y_START, m_creature, pPlayer);
                break;
            case 4:
                DoScriptText(SAY_1_MOOTOO_Y, m_creature);
                break;
            case 9:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_2_MOOTOO_Y, m_creature, pPlayer);
                break;
            case 10:
                m_creature->HandleEmote(EMOTE_ONESHOT_POINT);
                break;
            case 12:
                DoScriptText(SAY_3_MOOTOO_Y, m_creature);
                break;
            case 14:
                DoScriptText(SAY_4_MOOTOO_Y, m_creature);
                break;
            case 15:
                DoScriptText(SAY_5_MOOTOO_Y, m_creature);
                break;
            case 16:
                DoScriptText(SAY_6_MOOTOO_Y, m_creature);
                break;
            case 18:
                DoScriptText(SAY_4_MOOTOO_Y, m_creature);
                break;
            case 19:
                DoScriptText(SAY_7_MOOTOO_Y, m_creature);
                break;
            case 20:
                DoScriptText(SAY_8_MOOTOO_Y, m_creature);
                break;
            case 22:
                DoScriptText(SAY_CREDIT_MOOTOO_Y, m_creature);
                SetRun();
                break;
            case 23:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_ESCAPING_THE_MIST, m_creature);
                if (Creature* pFather = GetClosestCreatureWithEntry(m_creature, NPC_ELDER_MOOTOO, 30.0f))
                    DoScriptText(SAY_1_ELDER_MOOTOO, pFather);
                break;
            case 24:
                DoScriptText(SAY_9_MOOTOO_Y, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                    if (Creature* pFather = GetClosestCreatureWithEntry(m_creature, NPC_ELDER_MOOTOO, 30.0f))
                        DoScriptText(SAY_2_ELDER_MOOTOO, pFather, pPlayer);
                break;
            case 25:
                if (Player* pPlayer = GetPlayerForEscort())
                    if (Creature* pFather = GetClosestCreatureWithEntry(m_creature, NPC_ELDER_MOOTOO, 30.0f))
                        pFather->HandleEmote(EMOTE_ONESHOT_BOW);
                SetEscortPaused(true);
                m_creature->ForcedDespawn(10000);
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));
    }
};

bool QuestAccept_npc_mootoo_the_younger(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPING_THE_MIST)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

CreatureAI* GetAI_npc_mootoo_the_youngerAI(Creature* pCreature)
{
    return new npc_mootoo_the_youngerAI(pCreature);
}

void AddSC_borean_tundra()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_nesingwary_trapper";
    pNewScript->GetAI = &GetAI_npc_nesingwary_trapper;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_oil_stained_wolf";
    pNewScript->GetAI = &GetAI_npc_oil_stained_wolf;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_oil_stained_wolf;
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_npc_oil_stained_wolf;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_sinkhole_kill_credit";
    pNewScript->GetAI = &GetAI_npc_sinkhole_kill_credit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lurgglbr";
    pNewScript->GetAI = &GetAI_npc_lurgglbr;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_lurgglbr;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_beryl_sorcerer";
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_npc_beryl_sorcerer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_captured_beryl_sorcerer";
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_npc_captured_beryl_sorcerer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_nexus_drake_hatchling";
    pNewScript->GetAI = &GetAI_npc_nexus_drake_hatchling;
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_npc_nexus_drake_hatchling;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_nexus_drake_hatchling;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_scourged_flamespitter";
    pNewScript->GetAI = &GetAI_npc_scourged_flamespitter;
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_npc_scourged_flamespitter;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_bonker_togglevolt";
    pNewScript->GetAI = &GetAI_npc_bonker_togglevolt;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_bonker_togglevolt;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_jenny";
    pNewScript->GetAI = &GetAI_npc_jenny;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_seaforium_depth_charge";
    pNewScript->GetAI = &GetAI_npc_seaforium_depth_charge;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mootoo_the_younger";
    pNewScript->GetAI = &GetAI_npc_mootoo_the_youngerAI;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_mootoo_the_younger;
    pNewScript->RegisterSelf();
}
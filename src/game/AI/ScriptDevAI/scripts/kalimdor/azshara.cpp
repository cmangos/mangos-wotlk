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
SDName: Azshara
SD%Complete: 100%
SDComment: Quest support: 2744, 3141, 3602, 8729, 9364, 10994
SDCategory: Azshara
EndScriptData */

/* ContentData
npc_rizzle_sprysprocket
npc_depth_charge
go_southfury_moonstone
mobs_spitelashes
npc_loramus_thalipedes
npc_felhound_tracker
event_arcanite_buoy
go_lightning
boss_maws
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "AI/ScriptDevAI/base/pet_ai.h"

/*#####
## npc_rizzle_sprysprocket
#####*/

enum
{
    SAY_START                   = -1000351,
    EMOTE_START                 = -1000352,
    SAY_WHISPER_CHILL           = -1000353,
    SAY_GRENADE_FAIL            = -1000354,
    SAY_END                     = -1000355,

    QUEST_MOONSTONE             = 10994,
    NPC_RIZZLE                  = 23002,
    NPC_DEPTH_CHARGE            = 23025,

    SPELL_SUMMON_RIZZLE         = 39866,
    SPELL_BLACKJACK             = 39865,                    // stuns player
    SPELL_ESCAPE                = 39871,                    // teleports to water
    SPELL_SWIM_SPEED            = 40596,

    SPELL_FROST_TRAP            = 39902,                    // not used?

    SPELL_PERIODIC_GRENADE      = 40553,                    // cannot tell who are supposed to have this aura
    SPELL_FROST_GRENADE         = 40525,                    // triggered by periodic grenade

    SPELL_SUMMON_DEPTH_CHARGE   = 39907,                    // summons the bomb creature
    SPELL_TRAP                  = 39899,                    // knockback

    SPELL_PERIODIC_CHECK        = 39888,
    SPELL_SURRENDER             = 39889,                    // should be triggered by periodic check, if player comes in certain distance with quest incomplete

    SPELL_GIVE_MOONSTONE        = 39886
};

#define GOSSIP_ITEM_MOONSTONE   "Hand over the Southfury moonstone and I'll let you go."

struct npc_rizzle_sprysprocketAI : public npc_escortAI
{
    npc_rizzle_sprysprocketAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        pCreature->SetActiveObjectState(true);
        m_bIsIntro = true;
        m_uiIntroPhase = 0;
        m_uiIntroTimer = 0;
        m_uiDepthChargeTimer = 10000;
        Reset();
    }

    bool m_bIsIntro;
    uint8 m_uiIntroPhase;
    uint32 m_uiIntroTimer;
    uint32 m_uiDepthChargeTimer;

    void MoveInLineOfSight(Unit* pUnit) override
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING) && pUnit->GetTypeId() == TYPEID_PLAYER)
        {
            if (!HasEscortState(STATE_ESCORT_PAUSED) && m_creature->IsWithinDistInMap(pUnit, INTERACTION_DISTANCE) && m_creature->IsWithinLOSInMap(pUnit))
            {
                if (((Player*)pUnit)->GetQuestStatus(QUEST_MOONSTONE) == QUEST_STATUS_INCOMPLETE)
                    m_creature->CastSpell(m_creature, SPELL_SURRENDER, TRIGGERED_OLD_TRIGGERED);
            }
        }

        npc_escortAI::MoveInLineOfSight(pUnit);
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 1:
                m_creature->CastSpell(m_creature, SPELL_PERIODIC_CHECK, TRIGGERED_OLD_TRIGGERED);
                break;
        }
    }

    void Reset() override { }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_SURRENDER)
        {
            SetEscortPaused(true);
            DoScriptText(SAY_END, m_creature);
            m_creature->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
    }

    // this may be wrong (and doesn't work)
    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        if (pTarget->GetTypeId() == TYPEID_PLAYER && pSpell->Id == SPELL_FROST_GRENADE)
            DoScriptText(SAY_WHISPER_CHILL, m_creature, pTarget);
    }

    // this may be wrong
    void JustSummoned(Creature* /*pSummoned*/) override
    {
        // pSummoned->CastSpell(pSummoned,SPELL_PERIODIC_GRENADE,TRIGGERED_NONE,NULL,NULL,m_creature->GetObjectGuid());
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (m_bIsIntro)
        {
            if (m_uiIntroTimer < uiDiff)
                m_uiIntroTimer = 1500;
            else
            {
                m_uiIntroTimer -= uiDiff;
                return;
            }

            switch (m_uiIntroPhase)
            {
                case 0:
                    DoScriptText(SAY_START, m_creature);
                    DoScriptText(EMOTE_START, m_creature);
                    break;
                case 1:
                    // teleports to water _before_ we Start()
                    m_creature->CastSpell(m_creature, SPELL_ESCAPE, TRIGGERED_NONE);
                    break;
                case 2:
                    m_creature->CastSpell(m_creature, SPELL_SWIM_SPEED, TRIGGERED_NONE);
                    m_bIsIntro = false;
                    Start(true);
                    break;
            }

            ++m_uiIntroPhase;
            return;
        }

        if (m_uiDepthChargeTimer < uiDiff)
        {
            if (!HasEscortState(STATE_ESCORT_PAUSED))
                m_creature->CastSpell(m_creature, SPELL_SUMMON_DEPTH_CHARGE, TRIGGERED_NONE);

            m_uiDepthChargeTimer = urand(10000, 15000);
        }
        else
            m_uiDepthChargeTimer -= uiDiff;
    }
};

UnitAI* GetAI_npc_rizzle_sprysprocket(Creature* pCreature)
{
    return new npc_rizzle_sprysprocketAI(pCreature);
}

bool GossipHello_npc_rizzle_sprysprocket(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_MOONSTONE) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MOONSTONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_rizzle_sprysprocket(Player* pPlayer, Creature* /*pCreature*/, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, SPELL_GIVE_MOONSTONE, TRIGGERED_NONE);
    }

    return true;
}

struct npc_depth_chargeAI : public ScriptedAI
{
    npc_depth_chargeAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void MoveInLineOfSight(Unit* pUnit) override
    {
        if (pUnit->GetTypeId() != TYPEID_PLAYER)
            return;

        if (m_creature->IsWithinDistInMap(pUnit, INTERACTION_DISTANCE) && m_creature->IsWithinLOSInMap(pUnit))
            m_creature->CastSpell(pUnit, SPELL_TRAP, TRIGGERED_NONE);
    }

    void Reset() override { }
};

UnitAI* GetAI_npc_depth_charge(Creature* pCreature)
{
    return new npc_depth_chargeAI(pCreature);
}

/*######
## go_southfury_moonstone
######*/

bool GOUse_go_southfury_moonstone(Player* pPlayer, GameObject* /*pGo*/)
{
    // implicitTarget=48 not implemented as of writing this code, and manual summon may be just ok for our purpose
    // pPlayer->CastSpell(pPlayer,SPELL_SUMMON_RIZZLE,TRIGGERED_NONE);

    if (Creature* pCreature = pPlayer->SummonCreature(NPC_RIZZLE, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSPAWN_DEAD_DESPAWN, 0))
        pCreature->CastSpell(pPlayer, SPELL_BLACKJACK, TRIGGERED_NONE);

    return false;
}

/*######
## mobs_spitelashes
######*/
enum
{
    // quest related
    SPELL_POLYMORPH_BACKFIRE    = 28406,        // summons npc 16479
    QUEST_FRAGMENTED_MAGIC      = 9364,

    // npc spells
    SPELL_DISARM                = 6713,         // warrior
    SPELL_SCREECH               = 3589,         // screamer
    SPELL_FROST_SHOCK           = 12548,        // serpent guard
    SPELL_RENEW                 = 11640,        // siren
    SPELL_SHOOT                 = 6660,
    SPELL_FROST_SHOT            = 12551,
    SPELL_FROST_NOVA            = 11831,
    SPELL_STRIKE                = 11976,        // myrmidon

    NPC_SPITELASH_WARRIOR       = 6190,
    NPC_SPITELASH_SCREAMER      = 6193,
    NPC_SPITELASH_GUARD         = 6194,
    NPC_SPITELASH_SIREN         = 6195,
    NPC_SPITELASH_MYRMIDON      = 6196,

    TARGET_TYPE_RANDOM          = 0,
    TARGET_TYPE_VICTIM          = 1,
    TARGET_TYPE_SELF            = 2,
    TARGET_TYPE_FRIENDLY        = 3,
};

struct SpitelashAbilityStruct
{
    uint32 m_uiCreatureEntry, m_uiSpellId;
    uint8 m_uiTargetType;
    uint32 m_uiInitialTimer, m_uiCooldown;
};

static SpitelashAbilityStruct m_aSpitelashAbility[8] =
{
    {NPC_SPITELASH_WARRIOR,     SPELL_DISARM,       TARGET_TYPE_VICTIM,     4000,  10000},
    {NPC_SPITELASH_SCREAMER,    SPELL_SCREECH,      TARGET_TYPE_SELF,       7000,  15000},
    {NPC_SPITELASH_GUARD,       SPELL_FROST_SHOCK,  TARGET_TYPE_VICTIM,     7000,  13000},
    {NPC_SPITELASH_SIREN,       SPELL_RENEW,        TARGET_TYPE_FRIENDLY,   4000,  7000},
    {NPC_SPITELASH_SIREN,       SPELL_SHOOT,        TARGET_TYPE_RANDOM,     3000,  9000},
    {NPC_SPITELASH_SIREN,       SPELL_FROST_SHOT,   TARGET_TYPE_RANDOM,     7000,  10000},
    {NPC_SPITELASH_SIREN,       SPELL_FROST_NOVA,   TARGET_TYPE_SELF,       10000, 15000},
    {NPC_SPITELASH_MYRMIDON,    SPELL_STRIKE,       TARGET_TYPE_VICTIM,     3000,  7000}
};

struct mobs_spitelashesAI : public ScriptedAI
{
    mobs_spitelashesAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        for (uint8 i = 0; i < countof(m_aSpitelashAbility); ++i)
        {
            if (m_aSpitelashAbility[i].m_uiCreatureEntry == m_creature->GetEntry())
                m_mSpellTimers[i] = m_aSpitelashAbility[i].m_uiInitialTimer;
        }

        Reset();
    }

    uint32 m_uiMorphTimer;

    std::unordered_map<uint8, uint32> m_mSpellTimers;

    void Reset() override
    {
        m_uiMorphTimer = 0;

        for (auto& m_mSpellTimer : m_mSpellTimers)
            m_mSpellTimer.second = m_aSpitelashAbility[m_mSpellTimer.first].m_uiInitialTimer;
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        // If already hit by the polymorph return
        if (m_uiMorphTimer)
            return;

        // Creature get polymorphed into a sheep and after 5 secs despawns
        if (pCaster->GetTypeId() == TYPEID_PLAYER && ((Player*)pCaster)->GetQuestStatus(QUEST_FRAGMENTED_MAGIC) == QUEST_STATUS_INCOMPLETE &&
                (pSpell->Id == 118 || pSpell->Id == 12824 || pSpell->Id == 12825 || pSpell->Id == 12826))
            m_uiMorphTimer = 5000;
    }

    bool CanUseSpecialAbility(uint32 uiIndex)
    {
        Unit* pTarget = nullptr;

        switch (m_aSpitelashAbility[uiIndex].m_uiTargetType)
        {
            case TARGET_TYPE_SELF:
                pTarget = m_creature;
                break;
            case TARGET_TYPE_VICTIM:
                pTarget = m_creature->GetVictim();
                break;
            case TARGET_TYPE_RANDOM:
                pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, m_aSpitelashAbility[uiIndex].m_uiSpellId, SELECT_FLAG_IN_LOS);
                break;
            case TARGET_TYPE_FRIENDLY:
                pTarget = DoSelectLowestHpFriendly(10.0f);
                break;
        }

        if (pTarget)
        {
            if (DoCastSpellIfCan(pTarget, m_aSpitelashAbility[uiIndex].m_uiSpellId) == CAST_OK)
                return true;
        }

        return false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiMorphTimer)
        {
            if (m_uiMorphTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_POLYMORPH_BACKFIRE, CAST_TRIGGERED) == CAST_OK)
                {
                    m_uiMorphTimer = 0;
                    m_creature->ForcedDespawn();
                }
            }
            else
                m_uiMorphTimer -= uiDiff;
        }

        for (auto& m_mSpellTimer : m_mSpellTimers)
        {
            if (m_mSpellTimer.second < uiDiff)
            {
                if (CanUseSpecialAbility(m_mSpellTimer.first))
                {
                    m_mSpellTimer.second = m_aSpitelashAbility[m_mSpellTimer.first].m_uiCooldown;
                    break;
                }
            }
            else
                m_mSpellTimer.second -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_mobs_spitelashes(Creature* pCreature)
{
    return new mobs_spitelashesAI(pCreature);
}

/*######
## npc_loramus_thalipedes
######*/

bool GossipHello_npc_loramus_thalipedes(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(2744) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Can you help me?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    if (pPlayer->GetQuestStatus(3141) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Tell me your story", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_loramus_thalipedes(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(2744);
            break;

        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Please continue", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
            pPlayer->SEND_GOSSIP_MENU(1813, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+21:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I do not understand", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 22);
            pPlayer->SEND_GOSSIP_MENU(1814, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+22:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Indeed", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 23);
            pPlayer->SEND_GOSSIP_MENU(1815, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+23:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I will do this with or your help, Loramus", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 24);
            pPlayer->SEND_GOSSIP_MENU(1816, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+24:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Yes", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 25);
            pPlayer->SEND_GOSSIP_MENU(1817, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+25:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(3141);
            break;
    }
    return true;
}

/*######
## npc_felhound_tracker
######*/

enum
{
    QUEST_AZSHARITE     = 3602,

    GO_AZSHARITE_1      = 152620,
    GO_AZSHARITE_2      = 152621,
    GO_AZSHARITE_3      = 152622,
    GO_AZSHARITE_4      = 152631,

    SOUND_GROWL         = 1249
};

static const uint32 aGOList[] = {GO_AZSHARITE_1, GO_AZSHARITE_2, GO_AZSHARITE_3, GO_AZSHARITE_4};

struct npc_felhound_trackerAI : public ScriptedPetAI
{
    npc_felhound_trackerAI(Creature* pCreature) : ScriptedPetAI(pCreature) { Reset(); }

    bool m_bIsMovementActive;
    uint32 m_uiWaitTimer;

    void Reset() override
    {
        m_bIsMovementActive  = false;
        m_uiWaitTimer        = 0;
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        m_creature->GetMotionMaster()->MoveIdle();
        m_bIsMovementActive  = false;
        m_uiWaitTimer = 20000;
    }

    void ReceiveEmote(Player* pPlayer, uint32 uiTextEmote)
    {
        // Only react if player is on the quest
        if (pPlayer->GetQuestStatus(QUEST_AZSHARITE) != QUEST_STATUS_INCOMPLETE)
            return;

        if (uiTextEmote == TEXTEMOTE_ROAR)
        {
            m_uiWaitTimer = 0;
            m_bIsMovementActive = false;
            DoFindNewCrystal(pPlayer);
        }
    }

    // Function to search for new tubber in range
    void DoFindNewCrystal(Player* /*pMaster*/)
    {
        GameObjectList lCrystalsInRange;
        for (unsigned int i : aGOList)
        {
            GetGameObjectListWithEntryInGrid(lCrystalsInRange, m_creature, i, 40.0f);
            // If a crystal was found in range, stop the search here, else try with another GO
            if (!lCrystalsInRange.empty())
                break;
        }

        if (lCrystalsInRange.empty())   // Definely no GO found
        {
            m_creature->PlayDirectSound(SOUND_GROWL);
            return;
        }
        lCrystalsInRange.sort(ObjectDistanceOrder(m_creature));
        GameObject* pNearestCrystal = nullptr;

        // Always need to find new ones
        for (GameObjectList::const_iterator itr = lCrystalsInRange.begin(); itr != lCrystalsInRange.end(); ++itr)
        {
            if ((*itr)->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND))
            {
                pNearestCrystal = *itr;
                break;
            }
        }

        if (!pNearestCrystal)
        {
            m_creature->PlayDirectSound(SOUND_GROWL);
            return;
        }
        float fX, fY, fZ;
        pNearestCrystal->GetContactPoint(m_creature, fX, fY, fZ, 3.0f);
        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
        m_bIsMovementActive = true;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_bIsMovementActive)
            return;

        if (m_uiWaitTimer)
        {
            if (m_uiWaitTimer < uiDiff)
            {
                m_uiWaitTimer = 0;
                m_bIsMovementActive = false;
            }
            else
            {
                m_uiWaitTimer -= uiDiff;
                return;
            }
        }
        ScriptedPetAI::UpdateAI(uiDiff);
    }
};

UnitAI* GetAI_npc_felhound_tracker(Creature* pCreature)
{
    return new npc_felhound_trackerAI(pCreature);
}

/*######
## event_arcanite_buoy
######*/

enum
{
    EVENT_ARCANITE_BUOY     = 9542,
    NPC_MAWS                = 15571,
    GO_THEATRIC_LIGHTNING   = 183356,
};

struct SummonLocation
{
    float m_fX, m_fY, m_fZ;
};

static const SummonLocation aMawsSpawn = { 3561.73f, -6647.2f, -7.5f };

bool ProcessEventId_arcanite_buoy(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (uiEventId == EVENT_ARCANITE_BUOY)
    {
        if (pSource->GetTypeId() == TYPEID_PLAYER)
        {
            // Do not summon it twice
            if (Creature* temp = GetClosestCreatureWithEntry((Player*)pSource, NPC_MAWS, 200.0f))
                return false;

            // Summon Maws
            if (Creature* maws = ((Player*)pSource)->SummonCreature(NPC_MAWS, aMawsSpawn.m_fX, aMawsSpawn.m_fY, aMawsSpawn.m_fZ, 1.6f, TEMPSPAWN_MANUAL_DESPAWN, 0))
            {
                maws->SetWalk(false);
                maws->GetMotionMaster()->MoveWaypoint();

                // Respawn visual lightning GO
                if (GameObject* lightning = GetClosestGameObjectWithEntry((Player*)pSource, GO_THEATRIC_LIGHTNING, 20.0f))
                {
                    if (!lightning->IsSpawned())
                    {
                        lightning->SetRespawnTime(9000);
                        lightning->Refresh();
                    }
                }
            }
        }
        return true;
    }
    return false;
}

/*###############
## go_lightning
################*/

struct go_ai_lightning : public GameObjectAI
{
    go_ai_lightning(GameObject* go) : GameObjectAI(go), m_uilightningTimer(urand(2, 5) * IN_MILLISECONDS) {}

    uint32 m_uilightningTimer;

    // Custom anim is played on a random timer as long as GO is spawned in world
    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_go->IsSpawned())
            return;

        if (m_uilightningTimer <= uiDiff)
        {
            m_go->SendGameObjectCustomAnim(m_go->GetObjectGuid());
            m_uilightningTimer = urand(2, 5) * IN_MILLISECONDS;
        }
        else
            m_uilightningTimer -= uiDiff;
    }
};

GameObjectAI* GetAI_go_lightning(GameObject* go)
{
    return new go_ai_lightning(go);
}

/*###############
## boss_maws
################*/

enum
{
    SPELL_FRENZY                = 19812,
    SPELL_DARK_WATER            = 25743,
    SPELL_RAMPAGE               = 25744,

    EMOTE_GENERIC_FRENZY_KILL   = -1000001,
//    EMOTE_MAWS_KILL             = -1010000      // World emote that was removed in WotLK patch 3.2
};

struct boss_mawsAI : public ScriptedAI
{
    boss_mawsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 uiFrenzyTimer;
    uint32 uiDarkWaterTimer;
    uint32 uiRampageTimer;
    uint32 uiDespawnTimer;

    void Reset() override
    {
        uiFrenzyTimer           = 25 * IN_MILLISECONDS;
        uiDarkWaterTimer        = 15 * IN_MILLISECONDS;
        uiRampageTimer          = urand(20, 120) * IN_MILLISECONDS;
        uiDespawnTimer          = 2.5 * HOUR * IN_MILLISECONDS;
    }

    void JustDied(Unit* /*pKiller*/)
    {
        // Despawn visual lightning GO
        if (GameObject* lightning = GetClosestGameObjectWithEntry(m_creature, GO_THEATRIC_LIGHTNING, 200.0f))
            lightning->SetLootState(GO_JUST_DEACTIVATED);
//        DoScriptText(EMOTE_MAWS_KILL, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Despawn timer handled in UpdateAI because creature must perform actions on despawn that cannot be done in OnDespawn() (creature is already despawned)
        if (uiDespawnTimer < uiDiff)
        {
            // Despawn visual lightning GO
            if (GameObject* lightning = GetClosestGameObjectWithEntry(m_creature, GO_THEATRIC_LIGHTNING, 200.0f))
                lightning->SetLootState(GO_JUST_DEACTIVATED);
//            DoScriptText(EMOTE_MAWS_KILL, m_creature);
            m_creature->ForcedDespawn();
        }
        else
            uiDespawnTimer -= uiDiff;

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (uiFrenzyTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_FRENZY);
            DoScriptText(EMOTE_GENERIC_FRENZY_KILL, m_creature);
            uiFrenzyTimer = (m_creature->GetHealthPercent() < 20.0f ? 15 : 25) * IN_MILLISECONDS;
        }
        else
            uiFrenzyTimer -= uiDiff;

        if (uiRampageTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_RAMPAGE);
            uiRampageTimer = (m_creature->GetHealthPercent() < 20.0f ? 12 : urand(20, 120)) * IN_MILLISECONDS;
        }
        else
            uiRampageTimer -= uiDiff;

        if (m_creature->GetHealthPercent() < 20.0f)
        {
            if (uiDarkWaterTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_DARK_WATER);
                uiDarkWaterTimer = 15 * IN_MILLISECONDS;
            }
            else
                uiDarkWaterTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_maws(Creature* pCreature)
{
    return new boss_mawsAI(pCreature);
}

void AddSC_azshara()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_rizzle_sprysprocket";
    pNewScript->GetAI = &GetAI_npc_rizzle_sprysprocket;
    pNewScript->pGossipHello = &GossipHello_npc_rizzle_sprysprocket;
    pNewScript->pGossipSelect = &GossipSelect_npc_rizzle_sprysprocket;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_depth_charge";
    pNewScript->GetAI = &GetAI_npc_depth_charge;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_southfury_moonstone";
    pNewScript->pGOUse = &GOUse_go_southfury_moonstone;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mobs_spitelashes";
    pNewScript->GetAI = &GetAI_mobs_spitelashes;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_loramus_thalipedes";
    pNewScript->pGossipHello =  &GossipHello_npc_loramus_thalipedes;
    pNewScript->pGossipSelect = &GossipSelect_npc_loramus_thalipedes;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_felhound_tracker";
    pNewScript->GetAI = &GetAI_npc_felhound_tracker;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_arcanite_buoy";
    pNewScript->pProcessEventId = &ProcessEventId_arcanite_buoy;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_lightning";
    pNewScript->GetGameObjectAI = &GetAI_go_lightning;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_maws";
    pNewScript->GetAI = &GetAI_boss_maws;
    pNewScript->RegisterSelf();
}

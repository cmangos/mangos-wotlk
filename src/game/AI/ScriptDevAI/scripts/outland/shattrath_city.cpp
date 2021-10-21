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
SDName: Shattrath_City
SD%Complete: 100
SDComment: Quest support: 10004, 10231.
SDCategory: Shattrath City
EndScriptData */

/* ContentData
npc_dirty_larry
npc_ishanah
npc_khadgars_servant
npc_salsalabim
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "World/WorldState.h"
#include "AI/ScriptDevAI/base/TimerAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SAY_START               = -1000274,
    EMOTE_KNUCKLES          = -1001242,
    SAY_COUNT               = -1000275,
    SAY_COUNT_1             = -1000276,
    SAY_COUNT_2             = -1000277,
    SAY_ATTACK              = -1000278,
    SAY_GIVEUP              = -1000279,
    QUEST_WHAT_BOOK         = 10231,
    ENTRY_CREEPJACK         = 19726,
    ENTRY_MALONE            = 19725,
    GOSSIP_ITEM_BOOK        = -3000105,
};

struct npc_dirty_larryAI : public ScriptedAI
{
    npc_dirty_larryAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiNpcFlags = pCreature->GetUInt32Value(UNIT_NPC_FLAGS);
        Reset();
    }

    uint32 m_uiNpcFlags;

    ObjectGuid m_creepjackGuid;
    ObjectGuid m_maloneGuid;
    ObjectGuid m_playerGuid;

    bool bEvent;
    bool bActiveAttack;

    uint32 m_uiSayTimer;
    uint32 m_uiStep;

    void Reset() override
    {
        m_creature->SetUInt32Value(UNIT_NPC_FLAGS, m_uiNpcFlags);

        m_playerGuid.Clear();
        m_creepjackGuid.Clear();
        m_maloneGuid.Clear();

        bEvent = false;
        bActiveAttack = false;

        m_uiSayTimer = 1000;
        m_uiStep = 0;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
    }

    void StartEvent(Player* pPlayer)
    {
        m_playerGuid = pPlayer->GetObjectGuid();

        m_creature->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);

        m_creature->SetFacingToObject(pPlayer);

        if (Creature* pCreepjack = GetClosestCreatureWithEntry(m_creature, ENTRY_CREEPJACK, 20.0f))
        {
            m_creepjackGuid = pCreepjack->GetObjectGuid();
            pCreepjack->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        }

        if (Creature* pMalone = GetClosestCreatureWithEntry(m_creature, ENTRY_MALONE, 20.0f))
        {
            m_maloneGuid = pMalone->GetObjectGuid();
            pMalone->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        }

        bEvent = true;
    }

    uint32 NextStep(uint32 uiStep)
    {
        Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

        if (!pPlayer)
        {
            if (Creature* pCreepjack = m_creature->GetMap()->GetCreature(m_creepjackGuid))
            {
                if (!pCreepjack->GetCombatManager().IsInEvadeMode() && pCreepjack->IsAlive())
                    pCreepjack->AI()->EnterEvadeMode();

                pCreepjack->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            }

            if (Creature* pMalone = m_creature->GetMap()->GetCreature(m_maloneGuid))
            {
                if (!pMalone->GetCombatManager().IsInEvadeMode() && pMalone->IsAlive())
                    pMalone->AI()->EnterEvadeMode();

                pMalone->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            }

            EnterEvadeMode();
            return 0;
        }

        switch (uiStep)
        {
            case 1:
                if (Creature* pCreepjack = m_creature->GetMap()->GetCreature(m_creepjackGuid))
                    pCreepjack->SetStandState(UNIT_STAND_STATE_STAND);

                if (Creature* pMalone = m_creature->GetMap()->GetCreature(m_maloneGuid))
                    pMalone->SetStandState(UNIT_STAND_STATE_STAND);

                DoScriptText(SAY_START, m_creature, pPlayer);
                return 2000;
            case 2:
                if (Creature* pMalone = m_creature->GetMap()->GetCreature(m_maloneGuid))
                {
                    pMalone->SetFacingToObject(pPlayer);
                    DoScriptText(EMOTE_KNUCKLES, pMalone, pPlayer);
                }
                if (Creature* pCreepjack = m_creature->GetMap()->GetCreature(m_creepjackGuid))
                {
                    pCreepjack->SetFacingToObject(pPlayer);
                    DoScriptText(EMOTE_KNUCKLES, pCreepjack, pPlayer);
                }
                return 2000;
            case 3: DoScriptText(SAY_COUNT, m_creature, pPlayer);   return 5000;
            case 4: DoScriptText(SAY_COUNT_1, m_creature, pPlayer); return 2000;
            case 5: DoScriptText(SAY_COUNT_2, m_creature, pPlayer); return 2000;
            case 6: DoScriptText(SAY_ATTACK, m_creature, pPlayer);  return 2000;
            case 7:
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                SetReactState(REACT_AGGRESSIVE);

                if (pPlayer->IsAlive())
                {
                    if (!m_creature->IsInCombat())
                        AttackStart(pPlayer);

                    if (Creature* pMalone = m_creature->GetMap()->GetCreature(m_maloneGuid))
                    {
                        pMalone->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                        if (!pMalone->IsInCombat())
                            pMalone->AI()->AttackStart(pPlayer);
                    }

                    if (Creature* pCreepjack = m_creature->GetMap()->GetCreature(m_creepjackGuid))
                    {
                        pCreepjack->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                        if (!pCreepjack->IsInCombat())
                            pCreepjack->AI()->AttackStart(pPlayer);
                    }
                }

                bActiveAttack = true;
                return 500;
            case 8:
                DoScriptText(SAY_GIVEUP, m_creature, pPlayer);
                return 4000;
            case 9:
                m_creature->GetMotionMaster()->MoveTargetedHome();
                EnterEvadeMode();
                return 0;
            default:
                return 0;
        }
    }

    void AttackedBy(Unit* pAttacker) override
    {
        if (m_creature->GetVictim())
            return;

        if (!bActiveAttack)
            return;

        AttackStart(pAttacker);
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        if (damage < m_creature->GetHealth())
            return;

        damage = std::min(damage, m_creature->GetHealth() - 1);

        // damage will kill, this is pretty much the same as 1%HP left
        if (bEvent)
        {
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            SetReactState(REACT_PASSIVE);
            m_creature->CombatStop();

            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_WHAT_BOOK, m_creature);

            if (Creature* pCreepjack = m_creature->GetMap()->GetCreature(m_creepjackGuid))
            {
                if (!pCreepjack->GetCombatManager().IsInEvadeMode() && pCreepjack->IsAlive())
                    pCreepjack->AI()->EnterEvadeMode();

                pCreepjack->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            }
            if (Creature* pMalone = m_creature->GetMap()->GetCreature(m_maloneGuid))
            {
                if (!pMalone->GetCombatManager().IsInEvadeMode() && pMalone->IsAlive())
                    pMalone->AI()->EnterEvadeMode();

                pMalone->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            }

            bActiveAttack = false;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (bEvent && !bActiveAttack)
        {
            if (m_uiSayTimer < diff)
                m_uiSayTimer = NextStep(++m_uiStep);
            else
                m_uiSayTimer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

bool GossipHello_npc_dirty_larry(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_WHAT_BOOK) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BOOK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_dirty_larry(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (npc_dirty_larryAI* pLarryAI = dynamic_cast<npc_dirty_larryAI*>(pCreature->AI()))
            pLarryAI->StartEvent(pPlayer);

        pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}

UnitAI* GetAI_npc_dirty_larry(Creature* pCreature)
{
    return new npc_dirty_larryAI(pCreature);
}

/*######
## npc_khadgars_servant
######*/

enum
{
    SAY_KHAD_START          = -1000489,
    SAY_KHAD_SERV_0         = -1000234,

    SAY_KHAD_SERV_1         = -1000235,
    SAY_KHAD_SERV_2         = -1000236,
    SAY_KHAD_SERV_3         = -1000237,
    SAY_KHAD_SERV_4         = -1000238,

    SAY_KHAD_SERV_5         = -1000239,
    SAY_KHAD_SERV_6         = -1000240,
    SAY_KHAD_SERV_7         = -1000241,

    SAY_KHAD_SERV_8         = -1000242,
    SAY_KHAD_SERV_9         = -1000243,
    SAY_KHAD_SERV_10        = -1000244,
    SAY_KHAD_SERV_11        = -1000245,

    SAY_KHAD_SERV_12        = -1000246,
    SAY_KHAD_SERV_13        = -1000247,

    SAY_KHAD_SERV_14        = -1000248,
    SAY_KHAD_SERV_15        = -1000249,
    SAY_KHAD_SERV_16        = -1000250,
    SAY_KHAD_SERV_17        = -1000251,

    SAY_KHAD_SERV_18        = -1000252,
    SAY_KHAD_SERV_19        = -1000253,
    SAY_KHAD_SERV_20        = -1000254,
    SAY_KHAD_SERV_21        = -1000255,

    SAY_KHAD_INJURED        = -1000490,
    SAY_KHAD_MIND_YOU       = -1000491,
    SAY_KHAD_MIND_ALWAYS    = -1000492,
    SAY_KHAD_ALDOR_GREET    = -1000493,
    SAY_KHAD_SCRYER_GREET   = -1000494,
    SAY_KHAD_HAGGARD        = -1000495,

    NPC_KHADGAR             = 18166,
    NPC_SHANIR              = 18597,
    NPC_IZZARD              = 18622,
    NPC_ADYRIA              = 18596,
    NPC_ANCHORITE           = 19142,
    NPC_ARCANIST            = 18547,
    NPC_HAGGARD             = 19684,

    QUEST_CITY_LIGHT        = 10211
};

struct npc_khadgars_servantAI : public npc_escortAI
{
    npc_khadgars_servantAI(Creature* creature) : npc_escortAI(creature), m_startPhase(0)
    {
        AddCustomAction(0, 2000u, [=]() { HandleStart(); });
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    uint32 m_uiPointId;
    uint32 m_uiTalkTimer;
    uint32 m_uiTalkCount;
    uint32 m_uiRandomTalkCooldown;

    uint32 m_startPhase;

    void Reset() override
    {
        m_uiTalkTimer = 2500;
        m_uiTalkCount = 0;
        m_uiPointId = 0;
        m_uiRandomTalkCooldown = 0;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_uiRandomTalkCooldown && pWho->GetTypeId() == TYPEID_UNIT && m_creature->IsWithinDistInMap(pWho, 10.0f))
        {
            switch (pWho->GetEntry())
            {
                case NPC_HAGGARD:
                    if (Player* pPlayer = GetPlayerForEscort())
                        DoScriptText(SAY_KHAD_HAGGARD, pWho, pPlayer);
                    m_uiRandomTalkCooldown = 7500;
                    break;
                case NPC_ANCHORITE:
                    if (Player* pPlayer = GetPlayerForEscort())
                        DoScriptText(SAY_KHAD_ALDOR_GREET, pWho, pPlayer);
                    m_uiRandomTalkCooldown = 7500;
                    break;
                case NPC_ARCANIST:
                    if (Player* pPlayer = GetPlayerForEscort())
                        DoScriptText(SAY_KHAD_SCRYER_GREET, pWho, pPlayer);
                    m_uiRandomTalkCooldown = 7500;
                    break;
            }
        }
    }

    void HandleStart()
    {
        uint32 timer = 0;
        switch (m_startPhase)
        {
            case 0: // khadgar talks
                if (Creature* pKhadgar = GetClosestCreatureWithEntry(m_creature, NPC_KHADGAR, 10.0f))
                    DoScriptText(SAY_KHAD_START, pKhadgar);
                timer = 3000;
                break;
            case 1: // servant talks
                DoScriptText(SAY_KHAD_SERV_0, m_creature);
                timer = 4000;
                break;
            case 2: // wps start
                if (m_creature->GetOwner() && m_creature->GetOwner()->GetTypeId() == TYPEID_PLAYER)
                    Start(false, static_cast<Player*>(m_creature->GetOwner()));
                else
                    script_error_log("npc_khadgars_servant can not obtain owner or owner is not a player.");
                break;
        }
        ++m_startPhase;
        if (timer)
            ResetTimer(0, timer);
    }

    void WaypointReached(uint32 uiPointId) override
    {
        m_uiPointId = uiPointId;

        switch (uiPointId)
        {
            case 1:
                break;
            case 7:
            case 26:
            case 52:
            case 65:
            case 76:
            case 77:
                SetEscortPaused(true);
                break;
            case 36:
                if (Creature* pIzzard = GetClosestCreatureWithEntry(m_creature, NPC_IZZARD, 10.0f))
                    DoScriptText(SAY_KHAD_MIND_YOU, pIzzard);
                break;
            case 37:
                if (Creature* pAdyria = GetClosestCreatureWithEntry(m_creature, NPC_ADYRIA, 10.0f))
                    DoScriptText(SAY_KHAD_MIND_ALWAYS, pAdyria);
                break;
        }
    }

    void UpdateEscortAI(const uint32 diff) override
    {
        UpdateTimers(diff, m_creature->IsInCombat());

        if (m_uiRandomTalkCooldown)
        {
            if (m_uiRandomTalkCooldown <= diff)
                m_uiRandomTalkCooldown = 0;
            else
                m_uiRandomTalkCooldown -= diff;
        }

        if (HasEscortState(STATE_ESCORT_PAUSED))
        {
            if (m_uiTalkTimer <= diff)
            {
                ++m_uiTalkCount;
                m_uiTalkTimer = 7500;

                Player* pPlayer = GetPlayerForEscort();

                if (!pPlayer)
                    return;

                switch (m_uiPointId)
                {
                    case 7:                                 // to lower city
                    {
                        switch (m_uiTalkCount)
                        {
                            case 1:
                                DoScriptText(SAY_KHAD_SERV_1, m_creature, pPlayer);
                                break;
                            case 2:
                                DoScriptText(SAY_KHAD_SERV_2, m_creature, pPlayer);
                                break;
                            case 3:
                                DoScriptText(SAY_KHAD_SERV_3, m_creature, pPlayer);
                                break;
                            case 4:
                                DoScriptText(SAY_KHAD_SERV_4, m_creature, pPlayer);
                                SetEscortPaused(false);
                                break;
                        }
                        break;
                    }
                    case 26:                                // in lower city
                    {
                        switch (m_uiTalkCount)
                        {
                            case 5:
                                if (Creature* pShanir = GetClosestCreatureWithEntry(m_creature, NPC_SHANIR, 15.0f))
                                    DoScriptText(SAY_KHAD_INJURED, pShanir, pPlayer);

                                DoScriptText(SAY_KHAD_SERV_5, m_creature, pPlayer);
                                break;
                            case 6:
                                DoScriptText(SAY_KHAD_SERV_6, m_creature, pPlayer);
                                break;
                            case 7:
                                DoScriptText(SAY_KHAD_SERV_7, m_creature, pPlayer);
                                SetEscortPaused(false);
                                break;
                        }
                        break;
                    }
                    case 52:                                // outside
                    {
                        switch (m_uiTalkCount)
                        {
                            case 8:
                                DoScriptText(SAY_KHAD_SERV_8, m_creature, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_KHAD_SERV_9, m_creature, pPlayer);
                                break;
                            case 10:
                                DoScriptText(SAY_KHAD_SERV_10, m_creature, pPlayer);
                                break;
                            case 11:
                                DoScriptText(SAY_KHAD_SERV_11, m_creature, pPlayer);
                                SetEscortPaused(false);
                                break;
                        }
                        break;
                    }
                    case 65:                                // scryer
                    {
                        switch (m_uiTalkCount)
                        {
                            case 12:
                                DoScriptText(SAY_KHAD_SERV_12, m_creature, pPlayer);
                                break;
                            case 13:
                                DoScriptText(SAY_KHAD_SERV_13, m_creature, pPlayer);
                                SetEscortPaused(false);
                                break;
                        }
                        break;
                    }
                    case 76:                                // aldor
                    {
                        switch (m_uiTalkCount)
                        {
                            case 14:
                                DoScriptText(SAY_KHAD_SERV_14, m_creature, pPlayer);
                                break;
                            case 15:
                                DoScriptText(SAY_KHAD_SERV_15, m_creature, pPlayer);
                                break;
                            case 16:
                                DoScriptText(SAY_KHAD_SERV_16, m_creature, pPlayer);
                                break;
                            case 17:
                                DoScriptText(SAY_KHAD_SERV_17, m_creature, pPlayer);
                                SetEscortPaused(false);
                                break;
                        }
                        break;
                    }
                    case 77:                                // a'dal
                    {
                        switch (m_uiTalkCount)
                        {
                            case 18:
                                DoScriptText(SAY_KHAD_SERV_18, m_creature, pPlayer);
                                break;
                            case 19:
                                DoScriptText(SAY_KHAD_SERV_19, m_creature, pPlayer);
                                break;
                            case 20:
                                DoScriptText(SAY_KHAD_SERV_20, m_creature, pPlayer);
                                break;
                            case 21:
                                DoScriptText(SAY_KHAD_SERV_21, m_creature, pPlayer);
                                pPlayer->AreaExploredOrEventHappens(QUEST_CITY_LIGHT);
                                SetEscortPaused(false);
                                break;
                        }
                        break;
                    }
                }
            }
            else
                m_uiTalkTimer -= diff;
        }
    }
};

/*######
# npc_salsalabim
######*/

enum
{
    FACTION_HOSTILE_SA              = 90,
    QUEST_10004                     = 10004,

    SPELL_MAGNETIC_PULL             = 31705,

    SAY_SAL_AGGRO                   = -1015104
};

struct npc_salsalabimAI : public ScriptedAI
{
    npc_salsalabimAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiMagneticPullTimer;

    void Reset() override
    {
        m_uiMagneticPullTimer = 15000;
    }

    void DamageTaken(Unit* pDoneBy, uint32& damage, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        if (pDoneBy && pDoneBy->GetTypeId() == TYPEID_PLAYER)
        {
            if (m_creature->GetHealthPercent() < 20.0f)
            {
                ((Player*)pDoneBy)->RewardPlayerAndGroupAtEventExplored(QUEST_10004, m_creature);
                damage = std::min(damage, m_creature->GetHealth() - 1);
                EnterEvadeMode();
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiMagneticPullTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MAGNETIC_PULL);
            m_uiMagneticPullTimer = 15000;
        }
        else
            m_uiMagneticPullTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_salsalabim(Creature* pCreature)
{
    return new npc_salsalabimAI(pCreature);
}

bool GossipSelect_npc_salsalabim(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == 1)
    {
       pCreature->SetFactionTemporary(FACTION_HOSTILE_SA, TEMPFACTION_RESTORE_REACH_HOME);
       pCreature->AI()->AttackStart(pPlayer);
       DoScriptText(SAY_SAL_AGGRO, pCreature, pPlayer);
       pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}

enum
{
    QUEST_KAELTHAS_AND_THE_VERDANT_SPHERE   = 11007,
    QUEST_TRIAL_OF_THE_NAARU_MAGTHERIDON    = 10888,
    QUEST_CUDGEL_OF_KARDESH                 = 10901,
    TITLE_CHAMPION_OF_THE_NAARU             = 53,

    SCRIPT_RELAY_ID = 10061,
};

bool QuestRewarded_npc_adal(Player* player, Creature* creature, Quest const* quest)
{
    switch (quest->GetQuestId())
    {
        case QUEST_TRIAL_OF_THE_NAARU_MAGTHERIDON:
            if (player->GetQuestStatus(QUEST_CUDGEL_OF_KARDESH) == QUEST_STATUS_COMPLETE)
                player->SetTitle(TITLE_CHAMPION_OF_THE_NAARU);
            break;
        case QUEST_KAELTHAS_AND_THE_VERDANT_SPHERE:
            sWorldState.HandleExternalEvent(CUSTOM_EVENT_ADALS_SONG_OF_BATTLE, 0);
            player->GetMap()->ScriptsStart(sRelayScripts, SCRIPT_RELAY_ID, creature, player, Map::SCRIPT_EXEC_PARAM_UNIQUE_BY_SOURCE); // only once active per adal
            return true; // handled
    }

    return false; // unhandled
}

enum
{
    SPELL_DEMON_BROILED_SURPRISE    = 43753,
};

struct DemonBroiledSurprise : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool strict) const
    {
        if (strict)
        {
            float radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(spell->m_spellInfo->rangeIndex));
            UnitList tempUnitList;
            GameObjectList tempGOList;
            return spell->CheckScriptTargeting(EFFECT_INDEX_1, 1, radius, TARGET_ENUM_UNITS_SCRIPT_AOE_AT_SRC_LOC, tempUnitList, tempGOList);
        }
        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        spell->GetCaster()->CastSpell(nullptr, SPELL_DEMON_BROILED_SURPRISE, TRIGGERED_NONE);
    }
};

void AddSC_shattrath_city()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_dirty_larry";
    pNewScript->GetAI = &GetAI_npc_dirty_larry;
    pNewScript->pGossipHello = &GossipHello_npc_dirty_larry;
    pNewScript->pGossipSelect = &GossipSelect_npc_dirty_larry;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_khadgars_servant";
    pNewScript->GetAI = &GetNewAIInstance<npc_khadgars_servantAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_salsalabim";
    pNewScript->GetAI = &GetAI_npc_salsalabim;
    pNewScript->pGossipSelect = &GossipSelect_npc_salsalabim;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_adal";
    pNewScript->pQuestRewardedNPC = &QuestRewarded_npc_adal;
    pNewScript->RegisterSelf();

    RegisterSpellScript<DemonBroiledSurprise>("spell_demon_broiled_surprise");
}

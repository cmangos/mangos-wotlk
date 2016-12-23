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
SDName: Blades_Edge_Mountains
SD%Complete: 90
SDComment: Quest support: 10503, 10504, 10506, 10512, 10545, 10556, 10609, 10674, 10859, 10998, 11058, 11080. (npc_daranelle needs bit more work before consider complete)
SDCategory: Blade's Edge Mountains
EndScriptData */

/* ContentData
mobs_nether_drake
npc_daranelle
npc_bloodmaul_stout_trigger
npc_simon_game_bunny
npc_light_orb_collector
npc_bloodmaul_dire_wolf
EndContentData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "TemporarySummon.h"

/*######
## mobs_nether_drake
######*/

enum
{
    SAY_NIHIL_1                 = -1000169,
    SAY_NIHIL_2                 = -1000170,
    SAY_NIHIL_3                 = -1000171,
    SAY_NIHIL_4                 = -1000172,
    SAY_NIHIL_INTERRUPT         = -1000173,

    MAX_ENTRIES                 = 4,

    NPC_PROTO                   = 21821,
    NPC_ADOLESCENT              = 21817,
    NPC_MATURE                  = 21820,
    NPC_NIHIL                   = 21823,

    SPELL_T_PHASE_MODULATOR     = 37573,

    SPELL_ARCANE_BLAST          = 38881,
    SPELL_MANA_BURN             = 38884,
    SPELL_INTANGIBLE_PRESENCE   = 36513,
};

static const uint32 aNetherDrakeEntries[MAX_ENTRIES] = {NPC_PROTO, NPC_ADOLESCENT, NPC_MATURE, NPC_NIHIL};

struct mobs_nether_drakeAI : public ScriptedAI
{
    mobs_nether_drakeAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_bIsNihil;
    uint32 m_uiNihilSpeechTimer;
    uint32 m_uiNihilSpeechPhase;

    uint32 m_uiArcaneBlastTimer;
    uint32 m_uiManaBurnTimer;
    uint32 m_uiIntangiblePresenceTimer;

    void Reset() override
    {
        m_bIsNihil                  = false;
        m_uiNihilSpeechTimer        = 3000;
        m_uiNihilSpeechPhase        = 0;

        m_uiArcaneBlastTimer        = 7500;
        m_uiManaBurnTimer           = 10000;
        m_uiIntangiblePresenceTimer = 15000;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    // in case creature was not summoned (not expected)
    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE)
            return;

        if (uiPointId)
            m_creature->ForcedDespawn();
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_T_PHASE_MODULATOR && pCaster->GetTypeId() == TYPEID_PLAYER)
        {
            // we are nihil, so say before transform
            if (m_creature->GetEntry() == NPC_NIHIL)
            {
                DoScriptText(SAY_NIHIL_INTERRUPT, m_creature);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_bIsNihil = false;
            }

            // choose a new entry
            uint8 uiIndex = urand(0, MAX_ENTRIES - 1);

            // If we choose the same entry, try again
            while (aNetherDrakeEntries[uiIndex] == m_creature->GetEntry())
                uiIndex = urand(0, MAX_ENTRIES - 1);

            if (m_creature->UpdateEntry(aNetherDrakeEntries[uiIndex]))
            {
                // Nihil does only dialogue
                if (aNetherDrakeEntries[uiIndex] == NPC_NIHIL)
                {
                    EnterEvadeMode();
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_bIsNihil = true;
                }
                else
                    AttackStart(pCaster);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_bIsNihil)
        {
            if (m_uiNihilSpeechTimer < uiDiff)
            {
                switch (m_uiNihilSpeechPhase)
                {
                    case 0:
                        DoScriptText(SAY_NIHIL_1, m_creature);
                        break;
                    case 1:
                        DoScriptText(SAY_NIHIL_2, m_creature);
                        break;
                    case 2:
                        DoScriptText(SAY_NIHIL_3, m_creature);
                        break;
                    case 3:
                        DoScriptText(SAY_NIHIL_4, m_creature);
                        break;
                    case 4:
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        // take off to location above
                        m_creature->SetLevitate(true);
                        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
                        m_creature->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX() + 50.0f, m_creature->GetPositionY(), m_creature->GetPositionZ() + 50.0f);
                        break;
                }
                ++m_uiNihilSpeechPhase;
                m_uiNihilSpeechTimer = 5000;
            }
            else
                m_uiNihilSpeechTimer -= uiDiff;

            // anything below here is not interesting for Nihil, so skip it
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiIntangiblePresenceTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_INTANGIBLE_PRESENCE) == CAST_OK)
                m_uiIntangiblePresenceTimer = urand(15000, 30000);
        }
        else
            m_uiIntangiblePresenceTimer -= uiDiff;

        if (m_uiManaBurnTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_MANA_BURN, SELECT_FLAG_POWER_MANA))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_MANA_BURN) == CAST_OK)
                    m_uiManaBurnTimer = urand(8000, 16000);
            }
        }
        else
            m_uiManaBurnTimer -= uiDiff;

        if (m_uiArcaneBlastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANE_BLAST) == CAST_OK)
                m_uiArcaneBlastTimer = urand(2500, 7500);
        }
        else
            m_uiArcaneBlastTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mobs_nether_drake(Creature* pCreature)
{
    return new mobs_nether_drakeAI(pCreature);
}

/*######
## npc_daranelle
######*/

enum
{
    SAY_SPELL_INFLUENCE     = -1000174,
    NPC_KALIRI_AURA_DISPEL  = 21511,
    SPELL_LASHHAN_CHANNEL   = 36904
};

struct npc_daranelleAI : public ScriptedAI
{
    npc_daranelleAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() override { }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER)
        {
            if (pWho->HasAura(SPELL_LASHHAN_CHANNEL, EFFECT_INDEX_0) && m_creature->IsWithinDistInMap(pWho, 10.0f))
            {
                DoScriptText(SAY_SPELL_INFLUENCE, m_creature, pWho);

                // TODO: Move the below to updateAI and run if this statement == true
                ((Player*)pWho)->KilledMonsterCredit(NPC_KALIRI_AURA_DISPEL, m_creature->GetObjectGuid());
                pWho->RemoveAurasDueToSpell(SPELL_LASHHAN_CHANNEL);
            }
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }
};

CreatureAI* GetAI_npc_daranelle(Creature* pCreature)
{
    return new npc_daranelleAI(pCreature);
}

/*######
## npc_bloodmaul_stout_trigger
######*/

enum
{
    SAY_BREW_1                  = -1000156,
    SAY_BREW_2                  = -1000207,
    SAY_BREW_3                  = -1000208,

    SPELL_INTOXICATION          = 35240,
    //SPELL_INTOXICATION_VISUAL   = 35777,

    GO_TANKARD                  = 184315,
};

static const uint32 aOgreEntries[] = {19995, 19998, 20334, 20723, 20726, 20730, 20731, 20732, 21296};

struct npc_bloodmaul_stout_triggerAI : public ScriptedAI
{
    npc_bloodmaul_stout_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiStartTimer;
    bool m_bHasValidOgre;

    ObjectGuid m_selectedOgreGuid;

    void Reset() override
    {
        m_uiStartTimer = 1000;
        m_bHasValidOgre = false;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_bHasValidOgre && pWho->GetObjectGuid() == m_selectedOgreGuid && m_creature->IsWithinDistInMap(pWho, 3.5f))
        {
            // Handle interaction and run home with EAI
            m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_A, m_creature, (Creature*)pWho);

            // Give kill credit to the summoner player
            if (m_creature->IsTemporarySummon())
            {
                TemporarySummon* pTemporary = (TemporarySummon*)m_creature;

                if (Player* pSummoner = m_creature->GetMap()->GetPlayer(pTemporary->GetSummonerGuid()))
                    pSummoner->KilledMonsterCredit(m_creature->GetEntry(), m_creature->GetObjectGuid());
            }

            if (GameObject* tankard = GetClosestGameObjectWithEntry(m_creature, GO_TANKARD, 2.0f))
            {
                tankard->AddObjectToRemoveList();
                m_creature->ForcedDespawn(2000);
            }

            m_bHasValidOgre = false;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiStartTimer)
        {
            if (m_uiStartTimer <= uiDiff)
            {
                // get all the ogres in range
                std::list<Creature*> lOgreList;
                for (uint8 i = 0; i < countof(aOgreEntries); ++i)
                    GetCreatureListWithEntryInGrid(lOgreList, m_creature,  aOgreEntries[i], 30.0f);

                if (lOgreList.empty())
                {
                    m_uiStartTimer = 5000;
                    return;
                }

                // sort by distance and get only the closest
                lOgreList.sort(ObjectDistanceOrder(m_creature));

                std::list<Creature*>::const_iterator ogreItr = lOgreList.begin();
                Creature* pOgre = NULL;

                do
                {
                    if ((*ogreItr)->isAlive() && !(*ogreItr)->HasAura(SPELL_INTOXICATION))
                        pOgre = *ogreItr;

                    ++ogreItr;
                }
                while (!pOgre && ogreItr != lOgreList.end());

                if (!pOgre)
                {
                    m_uiStartTimer = 5000;
                    return;
                }

                // Move ogre to the point
                float fX, fY, fZ;
                pOgre->GetMotionMaster()->MoveIdle();
                pOgre->SetWalk(false, true);
                m_creature->GetContactPoint(pOgre, fX, fY, fZ);
                pOgre->GetMotionMaster()->MovePoint(0, fX, fY, fZ);

                switch (urand(0, 2))
                {
                    case 0: DoScriptText(SAY_BREW_1, pOgre); break;
                    case 1: DoScriptText(SAY_BREW_2, pOgre); break;
                    case 2: DoScriptText(SAY_BREW_3, pOgre); break;
                }

                m_selectedOgreGuid = pOgre->GetObjectGuid();
                m_uiStartTimer = 0;
                m_bHasValidOgre = true;
            }
            else
                m_uiStartTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_bloodmaul_stout_trigger(Creature* pCreature)
{
    return new npc_bloodmaul_stout_triggerAI(pCreature);
}

/*######
## npc_simon_game_bunny
######*/

enum
{
    // sounds
    SOUND_ID_BLUE                   = 11588,
    SOUND_ID_GREEN                  = 11589,
    SOUND_ID_RED                    = 11590,
    SOUND_ID_YELLOW                 = 11591,
    SOUND_ID_DISABLE_NODE           = 11758,

    // generic spells
    SPELL_SIMON_GAME_START          = 39993,        // aura used to prepare the AI game
    SPELL_PRE_EVENT_TIMER           = 40041,        // aura used to handle the color sequence

    // stage prepare spells (summons the colored auras)
    SPELL_PRE_GAME_BLUE_AURA        = 40176,
    SPELL_PRE_GAME_GREEN_AURA       = 40177,
    SPELL_PRE_GAME_RED_AURA         = 40178,
    SPELL_PRE_GAME_YELLOW_AURA      = 40179,

    // stage prepare spells large
    SPELL_PRE_GAME_YELLOW_LARGE     = 41110,
    SPELL_PRE_GAME_RED_LARGE        = 41111,
    SPELL_PRE_GAME_GREEN_LARGE      = 41112,
    SPELL_PRE_GAME_BLUE_LARGE       = 41113,

    // visual spells which define which buttons are pressed
    SPELL_BUTTON_PUSH_BLUE          = 40244,
    SPELL_BUTTON_PUSH_GREEN         = 40245,
    SPELL_BUTTON_PUSH_RED           = 40246,
    SPELL_BUTTON_PUSH_YELLOW        = 40247,

    // allow the clusters to be used and despawns the visual auras
    SPELL_GAME_START_RED            = 40169,
    SPELL_GAME_START_BLUE           = 40170,
    SPELL_GAME_START_GREEN          = 40171,
    SPELL_GAME_START_YELLOW         = 40172,

    // locks the clusters after a stage is completed
    SPELL_GAME_END_BLUE             = 40283,
    SPELL_GAME_END_GREEN            = 40284,
    SPELL_GAME_END_RED              = 40285,
    SPELL_GAME_END_YELLOW           = 40286,

    // other spells
    // SPELL_SWITCHED_ON_OFF        = 40512,            // decharger lock (not used)
    // SPELL_SWITCHED_ON_OFF_2      = 40499,            // decharger unlock (not used)
    SPELL_SWITCHED_ON               = 40494,            // apexis lock spell
    SPELL_SWITCHED_OFF              = 40495,            // apexis unlock spell

    // misc visual spells
    SPELL_VISUAL_LEVEL_START        = 40436,            // on Player game begin
    SPELL_VISUAL_GAME_FAILED        = 40437,            // on Player game fail
    SPELL_VISUAL_GAME_START         = 40387,            // on AI game begin
    SPELL_VISUAL_GAME_TICK          = 40391,            // game tick (sound)
    SPELL_VISUAL_GAME_TICK_LARGE    = 42019,            // game tick large (sound)

    // spells used by the player on GO press
    SPELL_INTROSPECTION_GREEN       = 40055,
    SPELL_INTROSPECTION_BLUE        = 40165,
    SPELL_INTROSPECTION_RED         = 40166,
    SPELL_INTROSPECTION_YELLOW      = 40167,

    // button press results
    SPELL_SIMON_BUTTON_PRESSED      = 39999,
    SPELL_GOOD_PRESS                = 40063,
    SPELL_BAD_PRESS                 = 41241,            // single player punishment
    SPELL_SIMON_GROUP_REWARD        = 41952,            // group punishment

    // quest rewards
    SPELL_APEXIS_VIBRATIONS         = 40310,            // quest complete spell
    SPELL_APEXIS_EMANATIONS         = 40311,            // quest complete spell
    SPELL_APEXIS_ENLIGHTENMENT      = 40312,            // quest complete spell

    // other
    NPC_SIMON_GAME_BUNNY            = 22923,

    GO_APEXIS_RELIC                 = 185890,
    GO_APEXIS_MONUMENT              = 185944,

    QUEST_AN_APEXIS_RELIC           = 11058,
    QUEST_RELICS_EMANATION          = 11080,

    // colors
    COLOR_IDX_BLUE                  = 0,
    COLOR_IDX_GREEN                 = 1,
    COLOR_IDX_RED                   = 2,
    COLOR_IDX_YELLOW                = 3,

    // phases
    PHASE_LEVEL_PREPARE             = 1,
    PHASE_AI_GAME                   = 2,
    PHASE_PLAYER_PREPARE            = 3,
    PHASE_PLAYER_GAME               = 4,
    PHASE_LEVEL_FINISHED            = 5,

    MAX_SIMON_LEVELS                = 8,                // counts the max levels of the game
    MAX_SIMON_FAIL_TIMER            = 5,                // counts the delay in which the player is allowed to click
};

struct SimonGame
{
    uint8 m_uiColor;
    uint32 m_uiVisual, m_uiIntrospection, m_uiSoundId;
};

static const SimonGame aApexisGameData[4] =
{
    {COLOR_IDX_BLUE,    SPELL_BUTTON_PUSH_BLUE,     SPELL_INTROSPECTION_BLUE,   SOUND_ID_BLUE},
    {COLOR_IDX_GREEN,   SPELL_BUTTON_PUSH_GREEN,    SPELL_INTROSPECTION_GREEN,  SOUND_ID_GREEN},
    {COLOR_IDX_RED,     SPELL_BUTTON_PUSH_RED,      SPELL_INTROSPECTION_RED,    SOUND_ID_RED},
    {COLOR_IDX_YELLOW,  SPELL_BUTTON_PUSH_YELLOW,   SPELL_INTROSPECTION_YELLOW, SOUND_ID_YELLOW}
};

struct npc_simon_game_bunnyAI : public ScriptedAI
{
    npc_simon_game_bunnyAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint8 m_uiGamePhase;

    uint32 m_uiLevelCount;
    uint32 m_uiLevelStage;
    uint32 m_uiPlayerStage;

    std::vector<uint8> m_vColors;
    bool m_bIsLargeEvent;
    bool m_bIsEventStarted;

    ObjectGuid m_masterPlayerGuid;

    void Reset() override
    {
        m_uiGamePhase  = PHASE_LEVEL_PREPARE;
        m_bIsEventStarted = false;

        m_uiLevelCount = 0;
        m_uiLevelStage = 0;
        m_uiPlayerStage = 0;
    }

    void GetAIInformation(ChatHandler& reader) override
    {
        reader.PSendSysMessage("Simon Game Bunny, current game phase = %u, current level = %u", uint32(m_uiGamePhase), m_uiLevelCount);
    }

    // Prepare levels
    void DoPrepareLevel()
    {
        // this visual is cast only after the first level
        if (m_uiLevelCount)
            DoCastSpellIfCan(m_creature, SPELL_VISUAL_GAME_START, CAST_TRIGGERED);
        // this part is done only on the first tick
        else
        {
            // lock apexis
            DoCastSpellIfCan(m_creature, SPELL_SWITCHED_ON, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_PRE_EVENT_TIMER, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

            // Get original summoner
            if (m_creature->IsTemporarySummon())
                m_masterPlayerGuid = ((TemporarySummon*)m_creature)->GetSummonerGuid();

            // Get closest apexis
            if (GameObject* pGo = GetClosestGameObjectWithEntry(m_creature, GO_APEXIS_RELIC, 5.0f))
                m_bIsLargeEvent = false;
            else if (GameObject* pGo = GetClosestGameObjectWithEntry(m_creature, GO_APEXIS_MONUMENT, 17.0f))
                m_bIsLargeEvent = true;
        }

        // prepare the buttons and summon the visual auras
        DoCastSpellIfCan(m_creature, m_bIsLargeEvent ? SPELL_PRE_GAME_BLUE_LARGE : SPELL_PRE_GAME_BLUE_AURA, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, m_bIsLargeEvent ? SPELL_PRE_GAME_GREEN_LARGE : SPELL_PRE_GAME_GREEN_AURA, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, m_bIsLargeEvent ? SPELL_PRE_GAME_RED_LARGE : SPELL_PRE_GAME_RED_AURA, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, m_bIsLargeEvent ? SPELL_PRE_GAME_YELLOW_LARGE : SPELL_PRE_GAME_YELLOW_AURA, CAST_TRIGGERED);

        m_vColors.clear();
        ++m_uiLevelCount;
    }

    // Setup the color sequence
    void DoSetupLevel()
    {
        uint8 uiIndex = urand(COLOR_IDX_BLUE, COLOR_IDX_YELLOW);
        m_vColors.push_back(uiIndex);

        DoCastSpellIfCan(m_creature, aApexisGameData[uiIndex].m_uiVisual, CAST_TRIGGERED);
        DoPlaySoundToSet(m_creature, aApexisGameData[uiIndex].m_uiSoundId);
    }

    // Setup the player level - called at the beginning at each player level
    void DoSetupPlayerLevel()
    {
        // allow the buttons to be used and despawn the visual auras
        DoCastSpellIfCan(m_creature, SPELL_GAME_START_RED, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_START_BLUE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_START_GREEN, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_START_YELLOW, CAST_TRIGGERED);
    }

    // Complete level - called when one level is completed succesfully
    void DoCompleteLevel()
    {
        // lock the buttons
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_RED, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_BLUE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_GREEN, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_YELLOW, CAST_TRIGGERED);

        // Complete game if all the levels
        if (m_uiLevelCount == MAX_SIMON_LEVELS)
            DoCompleteGame();
    }

    // Complete event - called when the game has been completed succesfully
    void DoCompleteGame()
    {
        // ToDo: not sure if the quest reward spells are implemented right. They all give the same buff but with a different duration
        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_masterPlayerGuid))
        {
            if (Group* pGroup = pPlayer->GetGroup())
            {
                for (GroupReference* pRef = pGroup->GetFirstMember(); pRef != NULL; pRef = pRef->next())
                {
                    if (Player* pMember = pRef->getSource())
                    {
                        // distance check - they need to be close to the Apexis
                        if (!pMember->IsWithinDistInMap(m_creature, 20.0f))
                            continue;

                        // on group event cast Enlightment on daily quest and Emanations on normal quest
                        if (pMember->GetQuestStatus(QUEST_AN_APEXIS_RELIC) == QUEST_STATUS_INCOMPLETE)
                            DoCastSpellIfCan(pMember, SPELL_APEXIS_EMANATIONS, CAST_TRIGGERED);
                        else if (pMember->GetQuestStatus(QUEST_RELICS_EMANATION) == QUEST_STATUS_INCOMPLETE)
                            DoCastSpellIfCan(pMember, SPELL_APEXIS_ENLIGHTENMENT, CAST_TRIGGERED);
                    }
                }
            }
            else
            {
                // solo event - cast Emanations on daily quest and vibrations on normal quest
                if (pPlayer->GetQuestStatus(QUEST_AN_APEXIS_RELIC) == QUEST_STATUS_INCOMPLETE)
                    DoCastSpellIfCan(pPlayer, SPELL_APEXIS_VIBRATIONS, CAST_TRIGGERED);
                else if (pPlayer->GetQuestStatus(QUEST_RELICS_EMANATION) == QUEST_STATUS_INCOMPLETE)
                    DoCastSpellIfCan(pPlayer, SPELL_APEXIS_EMANATIONS, CAST_TRIGGERED);
            }
        }

        // cleanup event after quest is finished
        DoCastSpellIfCan(m_creature, SPELL_SWITCHED_OFF, CAST_TRIGGERED);
        DoPlaySoundToSet(m_creature, SOUND_ID_DISABLE_NODE);
        m_creature->ForcedDespawn();
    }

    // Cleanup event - called when event fails
    void DoCleanupGame()
    {
        // lock the buttons
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_RED, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_BLUE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_GREEN, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_YELLOW, CAST_TRIGGERED);

        //  unlock apexis and despawn
        DoCastSpellIfCan(m_creature, SPELL_SWITCHED_OFF, CAST_TRIGGERED);
        DoPlaySoundToSet(m_creature, SOUND_ID_DISABLE_NODE);
        m_creature->ForcedDespawn();
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        switch (m_uiGamePhase)
        {
            case PHASE_LEVEL_PREPARE:
                // delay before each level - handled by big timer aura
                if (eventType == AI_EVENT_CUSTOM_A)
                    m_uiGamePhase = PHASE_AI_GAME;
                break;
            case PHASE_AI_GAME:
                // AI game - handled by small timer aura
                if (eventType == AI_EVENT_CUSTOM_B)
                {
                    // Move to next phase if the level is setup
                    if (m_uiLevelStage == m_uiLevelCount)
                    {
                        m_uiGamePhase = PHASE_PLAYER_PREPARE;
                        m_uiLevelStage = 0;
                        return;
                    }

                    DoSetupLevel();
                    ++m_uiLevelStage;
                }
                break;
            case PHASE_PLAYER_PREPARE:
                // Player prepare - handled by small timer aura
                if (eventType == AI_EVENT_CUSTOM_B)
                {
                    DoCastSpellIfCan(m_creature, SPELL_VISUAL_LEVEL_START, CAST_TRIGGERED);
                    DoSetupPlayerLevel();

                    m_uiGamePhase = PHASE_PLAYER_GAME;
                    m_uiPlayerStage = 0;
                }
                break;
            case PHASE_PLAYER_GAME:
                // Player game - listen to the player moves
                if (eventType == AI_EVENT_CUSTOM_C)
                {
                    // good button pressed
                    if (uiMiscValue == aApexisGameData[m_vColors[m_uiLevelStage]].m_uiIntrospection)
                    {
                        DoCastSpellIfCan(m_creature, SPELL_GOOD_PRESS, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, aApexisGameData[m_vColors[m_uiLevelStage]].m_uiVisual, CAST_TRIGGERED);

                        DoPlaySoundToSet(m_creature, aApexisGameData[m_vColors[m_uiLevelStage]].m_uiSoundId);

                        // increase the level stage and reset the event counter
                        ++m_uiLevelStage;
                        m_uiPlayerStage = 0;

                        // if all buttons were pressed succesfully, then move to next level
                        if (m_uiLevelStage == m_vColors.size())
                        {
                            DoCompleteLevel();

                            m_uiLevelStage = 0;
                            m_uiGamePhase = PHASE_LEVEL_FINISHED;
                        }
                        // cast tick sound
                        else
                            DoCastSpellIfCan(pInvoker, m_bIsLargeEvent ? SPELL_VISUAL_GAME_TICK_LARGE : SPELL_VISUAL_GAME_TICK, CAST_TRIGGERED);
                    }
                    // bad button pressed
                    else
                    {
                        DoCastSpellIfCan(pInvoker, m_bIsLargeEvent ? SPELL_SIMON_GROUP_REWARD : SPELL_BAD_PRESS, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_VISUAL_GAME_FAILED, CAST_TRIGGERED);
                        DoCleanupGame();
                    }
                }
                // AI ticks which handle the player timeout
                else if (eventType == AI_EVENT_CUSTOM_B)
                {
                    // if it takes too much time, the event will fail
                    if (m_uiPlayerStage == MAX_SIMON_FAIL_TIMER)
                    {
                        DoCastSpellIfCan(m_creature, SPELL_VISUAL_GAME_FAILED, CAST_TRIGGERED);
                        DoCleanupGame();
                    }

                    // Not sure if this is right, but we need to keep the buttons unlocked on every tick
                    DoSetupPlayerLevel();
                    ++m_uiPlayerStage;
                }
                break;
            case PHASE_LEVEL_FINISHED:
                // small delay until the next level
                if (eventType == AI_EVENT_CUSTOM_A)
                {
                    DoPrepareLevel();
                    m_uiGamePhase = PHASE_LEVEL_PREPARE;
                }
                break;
        }
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 /*uiDiff*/) override
    {
        // Start game on first update tick - don't wait for dummy auras
        if (!m_bIsEventStarted)
        {
            DoPrepareLevel();
            m_bIsEventStarted = true;
        }
    }
};

CreatureAI* GetAI_npc_simon_game_bunny(Creature* pCreature)
{
    return new npc_simon_game_bunnyAI(pCreature);
}

bool EffectDummyCreature_npc_simon_game_bunny(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (pCreatureTarget->GetEntry() != NPC_SIMON_GAME_BUNNY)
        return false;

    if (uiSpellId == SPELL_SIMON_GAME_START && uiEffIndex == EFFECT_INDEX_0)
    {
        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);
        return true;
    }
    else if (uiSpellId == SPELL_PRE_EVENT_TIMER && uiEffIndex == EFFECT_INDEX_0)
    {
        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, pCaster, pCreatureTarget);
        return true;
    }

    return false;
}

bool EffectScriptEffectCreature_npc_simon_game_bunny(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid originalCasterGuid)
{
    if ((uiSpellId == SPELL_INTROSPECTION_BLUE || uiSpellId == SPELL_INTROSPECTION_GREEN || uiSpellId == SPELL_INTROSPECTION_RED ||
            uiSpellId == SPELL_INTROSPECTION_YELLOW) && uiEffIndex == EFFECT_INDEX_1)
    {
        if (pCreatureTarget->GetEntry() == NPC_SIMON_GAME_BUNNY && pCaster->GetTypeId() == TYPEID_PLAYER && originalCasterGuid.IsGameObject())
            pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_C, pCaster, pCreatureTarget, uiSpellId);

        return true;
    }

    return false;
}

/*######
## npc_light_orb_collector
######*/

enum
{
    NPC_LIGHT_ORB_MINI          = 20771,
    NPC_KILL_CREDIT_TRIGGER     = 21929,

    MAX_PULL_DISTANCE           = 20,
};

struct npc_light_orb_collectorAI : public ScriptedAI
{
    npc_light_orb_collectorAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_selectedOrbGuid;
    bool m_bOrbPulled;

    uint32 m_uiStartTimer;

    void Reset() override
    {
        m_bOrbPulled    = false;
        m_uiStartTimer  = 0;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (pWho->GetTypeId() != TYPEID_UNIT || pWho->GetEntry() != NPC_LIGHT_ORB_MINI)
            return;

        // Select an nearby orb to collect
        if (!m_uiStartTimer && !m_bOrbPulled)
        {
            if (m_creature->GetDistance(pWho) <= MAX_PULL_DISTANCE)
            {
                m_selectedOrbGuid = pWho->GetObjectGuid();
                m_uiStartTimer = 2000;
            }
        }
        else if (m_bOrbPulled && pWho->GetObjectGuid() == m_selectedOrbGuid && m_creature->IsWithinDistInMap(pWho, 3.5f))
        {
            // Despawn the collected orb if close enough
            ((Creature*)pWho)->ForcedDespawn();

            // Give kill credit to the player
            if (m_creature->IsTemporarySummon())
            {
                TemporarySummon* pTemporary = (TemporarySummon*)m_creature;

                if (Player* pSummoner = m_creature->GetMap()->GetPlayer(pTemporary->GetSummonerGuid()))
                    pSummoner->KilledMonsterCredit(NPC_KILL_CREDIT_TRIGGER, m_creature->GetObjectGuid());
            }

            // Despawn collector
            m_creature->ForcedDespawn();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiStartTimer)
        {
            // Start collecting after some delay
            if (m_uiStartTimer <= uiDiff)
            {
                Creature* pSelectedOrb = m_creature->GetMap()->GetCreature(m_selectedOrbGuid);
                if (!pSelectedOrb)
                    return;

                // Orb is pulled fast
                pSelectedOrb->SetWalk(false);

                // Move orb to the collector
                float fX, fY, fZ;;
                pSelectedOrb->GetMotionMaster()->MoveIdle();
                m_creature->GetContactPoint(pSelectedOrb, fX, fY, fZ);
                pSelectedOrb->GetMotionMaster()->MovePoint(0, fX, fY, fZ);

                m_bOrbPulled = true;
                m_uiStartTimer = 0;
            }
            else
                m_uiStartTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_light_orb_collector(Creature* pCreature)
{
    return new npc_light_orb_collectorAI(pCreature);
}


/*######
## Grimoire business AIs
######*/

enum
{
    NPC_VIMGOL_THE_VILE         = 22911,
    NPC_VIMGOL_VISUAL_BUNNY     = 23040,
    NPC_VIMGOL_MIDDLE_BUNNY     = 23081,

    SPELL_VIMGOL_POP_TEST_A     = 39834,
    SPELL_VIMGOL_POP_TEST_B     = 39851,
    SPELL_VIMGOL_POP_TEST_C     = 39852,
    SPELL_VIMGOL_POP_TEST_D     = 39853,
    SPELL_VIMGOL_POP_TEST_E     = 39854,

    SPELL_PENTAGRAM_BEAM        = 39921,
    SPELL_UNHOLY_GROWTH         = 40545,
    SPELL_SUMMONED_DEMON        = 7741,
    SPELL_INTERRUPT_UNHOLY_GROWTH = 40547,
    SPELL_SUMMON_GRIMOIRE       = 39862,
    SPELL_SHADOWBOLT_VOLLEY     = 40070,
};

struct npc_vimgol_AI : public ScriptedAI
{
    bool m_uiEnrage;
    uint32 m_uiVolleyTimer;
    uint32 m_uiCastTimer;

    npc_vimgol_AI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset() override
    {
        m_uiEnrage = false;
        m_uiVolleyTimer = 5000;
        m_uiCastTimer = 0;  
    }

    void MovementInform(uint32 uiMovementType, uint32 uiData) override 
    {        
        m_creature->GetMotionMaster()->Clear();
        m_creature->CastSpell(m_creature, SPELL_UNHOLY_GROWTH, TRIGGERED_NONE);
        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
    }

    void JustDied(Unit* pKiller) override
    {
        DoCast(m_creature, SPELL_SUMMON_GRIMOIRE, true);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_uiEnrage)
        {
            if (m_creature->GetHealthPercent() <= 50.0f)
            {
                m_uiEnrage = true;

                if (Creature* pMiddleBunny = GetClosestCreatureWithEntry(m_creature, NPC_VIMGOL_MIDDLE_BUNNY, 60.0f))
                {
                    float x = pMiddleBunny->GetPositionX(), y = pMiddleBunny->GetPositionY(), z = pMiddleBunny->GetPositionZ();
                    m_creature->UpdateAllowedPositionZ(x, y, z);
                    m_creature->GetMotionMaster()->MovePoint(1, x, y, z);
                }
            }
        }

        if (m_uiVolleyTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_SHADOWBOLT_VOLLEY);
            m_uiVolleyTimer = 5000;
        }
        else
            m_uiVolleyTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_vimgol(Creature* pCreature)
{
    return new npc_vimgol_AI(pCreature);
}

struct npc_vimgol_visual_bunnyAI : public ScriptedAI
{
    npc_vimgol_visual_bunnyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pMap = (ScriptedMap*)pCreature->GetInstanceData();

        GuidVector bunnyGuids;

        if (m_pMap)
        {
            m_pMap->GetCreatureGuidVectorFromStorage(m_creature->GetEntry(), bunnyGuids);

            for (auto it = bunnyGuids.begin(); it != bunnyGuids.end(); ++it)
                if ((*it) == m_creature->GetObjectGuid())
                    m_uiBunnyId = std::distance(bunnyGuids.begin(), it);
        }

        Reset();
    }

    ScriptedMap* m_pMap;
    
    uint8 m_uiBunnyId;
    uint32 m_uiCastTimer;

    void Reset() override
    {
        m_uiCastTimer = 0;
    }

    uint32 GetScriptData() override
    {
        return m_uiBunnyId;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiCastTimer <= uiDiff)
        {
            switch (m_uiBunnyId)
            {
                case 0:
                    DoCastSpellIfCan(m_creature, SPELL_VIMGOL_POP_TEST_A);
                    break;
                case 1:
                    DoCastSpellIfCan(m_creature, SPELL_VIMGOL_POP_TEST_B);
                    break;
                case 2:
                    DoCastSpellIfCan(m_creature, SPELL_VIMGOL_POP_TEST_C);
                    break;
                case 3:
                    DoCastSpellIfCan(m_creature, SPELL_VIMGOL_POP_TEST_D);
                    break;
                case 4:
                    DoCastSpellIfCan(m_creature, SPELL_VIMGOL_POP_TEST_E);
                    break;
            }

            m_uiCastTimer = 4000;
        }
        else
            m_uiCastTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_vimgol_visual_bunny(Creature* pCreature)
{
    return new npc_vimgol_visual_bunnyAI(pCreature);
}

struct npc_vimgol_middle_bunnyAI : public ScriptedAI
{
    npc_vimgol_middle_bunnyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pMap = (ScriptedMap*)pCreature->GetInstanceData();

        if (m_pMap)
            m_pMap->GetCreatureGuidVectorFromStorage(NPC_VIMGOL_VISUAL_BUNNY, m_uiBunnyGuids);

        Reset();
    }

    ScriptedMap* m_pMap;
    GuidVector m_uiBunnyGuids;

    bool m_uiSpawned;
    bool m_uiActiveCircles[5];
    uint32 m_uiScanTimer;
    uint8 m_uiBeamTimer;
    uint8 m_uiFireballSkipCounter;

    void Reset() override
    {
        m_uiSpawned = false;

        for (int i = 0; i < 5; i++)
            m_uiActiveCircles[i] = false;

        m_uiScanTimer = 0;
        m_uiBeamTimer = 5;
        m_uiFireballSkipCounter = 0;
    }

    uint8 playersInsideCircles()
    {
        uint32 tmpAuras[5] = {
            SPELL_VIMGOL_POP_TEST_A, SPELL_VIMGOL_POP_TEST_B, SPELL_VIMGOL_POP_TEST_C,
            SPELL_VIMGOL_POP_TEST_D, SPELL_VIMGOL_POP_TEST_E
        };
        uint8 tmpCounter = 0;

        if (m_uiBunnyGuids.size() < 5 && m_pMap)
        {
            m_uiBunnyGuids.clear();
            m_pMap->GetCreatureGuidVectorFromStorage(NPC_VIMGOL_VISUAL_BUNNY, m_uiBunnyGuids);
        }

        for (int i = 0; i < 5; i++)
            m_uiActiveCircles[i] = false;

        std::list<Player*> playerList;
        GetPlayerListWithEntryInWorld(playerList, m_creature, 30);
        for (auto itr = playerList.begin(); itr != playerList.end(); ++itr)
        {
            if (!(*itr)->HasAura(SPELL_VIMGOL_POP_TEST_A) && !(*itr)->HasAura(SPELL_VIMGOL_POP_TEST_B) && !(*itr)->HasAura(SPELL_VIMGOL_POP_TEST_C) &&
                !(*itr)->HasAura(SPELL_VIMGOL_POP_TEST_D) && !(*itr)->HasAura(SPELL_VIMGOL_POP_TEST_E))
                continue;

            for (auto it = m_uiBunnyGuids.begin(); it != m_uiBunnyGuids.end(); ++it)
            {
                for (int i = 0; i < 5; ++i)
                {
                    if (!(*itr)->GetAura(tmpAuras[i], SpellEffectIndex(0)))
                        continue;

                    if ((*it) != (*itr)->GetAura(tmpAuras[i], SpellEffectIndex(0))->GetCasterGuid())
                        continue;

                    m_uiActiveCircles[std::distance(m_uiBunnyGuids.begin(), it)] = true;
                }
            }
        }

        for (int i = 0; i < 5; i++)
            if (m_uiActiveCircles[i])
                ++tmpCounter;

        return tmpCounter;
    }

    void CastBunnySpell(Creature* pTarget, uint32 uSpell)
    {
        if (!uSpell)
            return;

        std::list<Creature*> creatureList;
        GetCreatureListWithEntryInGrid(creatureList, m_creature, NPC_VIMGOL_VISUAL_BUNNY, 200.0f);
        for (auto& bunny : creatureList)
            for (auto it = m_uiBunnyGuids.begin(); it != m_uiBunnyGuids.end(); ++it)
                if ((*it) == bunny->GetObjectGuid())
                    if (m_uiActiveCircles[std::distance(m_uiBunnyGuids.begin(), it)])
                        bunny->CastSpell(pTarget ? pTarget : bunny, uSpell, TRIGGERED_OLD_TRIGGERED);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->CastSpell(pSummoned, SPELL_SUMMONED_DEMON, TRIGGERED_OLD_TRIGGERED);
        CastBunnySpell(nullptr, SPELL_PENTAGRAM_BEAM);
        m_uiSpawned = true;

        for (int i = 0; i < 5; i++)
            m_uiActiveCircles[i] = false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiScanTimer <= uiDiff)
        {
            m_uiScanTimer = 1000;

            if (m_uiSpawned)
            {
                if (Creature* pVimgol = GetClosestCreatureWithEntry(m_creature, NPC_VIMGOL_THE_VILE, 40.0f, true))
                {
                    m_uiFireballSkipCounter++;
                    if (m_uiFireballSkipCounter == 3)
                    {
                        CastBunnySpell(pVimgol, SPELL_INTERRUPT_UNHOLY_GROWTH);
                        m_uiFireballSkipCounter = 0;
                    }
                    if (playersInsideCircles() == 5)
                        pVimgol->InterruptSpell(CURRENT_GENERIC_SPELL);
                }
                else
                {
                    m_uiSpawned = false;
                    m_uiScanTimer = 30000;
                }
            }
            else
            {
                if (playersInsideCircles() == 5)
                    m_creature->SummonCreature(NPC_VIMGOL_THE_VILE, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
                if (m_uiBeamTimer == 0)
                {
                    CastBunnySpell(nullptr, SPELL_PENTAGRAM_BEAM);
                    m_uiBeamTimer = 5;
                }
                m_uiBeamTimer--;
            }
        }
        else
            m_uiScanTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_vimgol_middle_bunny(Creature* pCreature)
{
    return new npc_vimgol_middle_bunnyAI(pCreature);
}

/*######
## npc_bloodmaul_dire_wolf
######*/

enum
{
    SPELL_REND                          = 13443,
    SPELL_RINAS_DIMINUTION_POWDER       = 36310,                // Note: spell also throws event id 13584; requires more research on the purpose of this event

    NPC_DIRE_WOLF_TRIGGER               = 21176,
    NPC_BLOODMAUL_DIRE_WOLF             = 20058,

    FACTION_FRIENDLY                    = 35,
};

struct npc_bloodmaul_dire_wolfAI : public ScriptedAI
{
    npc_bloodmaul_dire_wolfAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiUnfriendlyTimer;
    uint32 m_uiRendTimer;

    void Reset() override
    {
        m_uiUnfriendlyTimer = 0;
        m_uiRendTimer       = urand(3000, 6000);
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            m_uiUnfriendlyTimer = 60000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Reset npc on timer
        if (m_uiUnfriendlyTimer)
        {
            if (m_uiUnfriendlyTimer <= uiDiff)
                EnterEvadeMode();
            else
                m_uiUnfriendlyTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiRendTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_REND) == CAST_OK)
                m_uiRendTimer = urand(8000, 13000);
        }
        else
            m_uiRendTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_bloodmaul_dire_wolf(Creature* pCreature)
{
    return new npc_bloodmaul_dire_wolfAI(pCreature);
}

bool EffectScriptEffectCreature_spell_diminution_powder(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_RINAS_DIMINUTION_POWDER && uiEffIndex == EFFECT_INDEX_1 && pCreatureTarget->GetEntry() == NPC_BLOODMAUL_DIRE_WOLF)
    {
        // Check if creature already has aura. ToDo: check for Hibernating creatures
        if (pCreatureTarget->HasAura(SPELL_RINAS_DIMINUTION_POWDER))
            return true;

        // give kill credit, change to friendly and inform the creautre about the reset timer
        ((Player*)pCaster)->KilledMonsterCredit(NPC_DIRE_WOLF_TRIGGER);
        pCreatureTarget->SetFactionTemporary(FACTION_FRIENDLY, TEMPFACTION_RESTORE_REACH_HOME);

        // Note: we can't remove all auras because it will also remove the current aura; so currently we only remove periodic damage auras
        // This might be wrong and we might need to change this to something like "RemoveAllAurasExceptId(...)"
        pCreatureTarget->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_DAMAGE);
        pCreatureTarget->DeleteThreatList();
        pCreatureTarget->CombatStop(true);

        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);
        return true;
    }

    return false;
}

void AddSC_blades_edge_mountains()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "mobs_nether_drake";
    pNewScript->GetAI = &GetAI_mobs_nether_drake;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_daranelle";
    pNewScript->GetAI = &GetAI_npc_daranelle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_bloodmaul_stout_trigger";
    pNewScript->GetAI = &GetAI_npc_bloodmaul_stout_trigger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_simon_game_bunny";
    pNewScript->GetAI = &GetAI_npc_simon_game_bunny;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_simon_game_bunny;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_npc_simon_game_bunny;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_light_orb_collector";
    pNewScript->GetAI = &GetAI_npc_light_orb_collector;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_bloodmaul_dire_wolf";
    pNewScript->GetAI = &GetAI_npc_bloodmaul_dire_wolf;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_spell_diminution_powder;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_vimgol_visual_bunny";
    pNewScript->GetAI = &GetAI_npc_vimgol_visual_bunny;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_vimgol_middle_bunny";
    pNewScript->GetAI = &GetAI_npc_vimgol_middle_bunny;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_vimgol";
    pNewScript->GetAI = &GetAI_npc_vimgol;
    pNewScript->RegisterSelf();
}

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
npc_frequency_scanner
npc_fel_cannon
npc_warp_gate
npc_obelisk_trigger
npc_vimgol
npc_vimgol_visual_bunny
npc_vimgol_middle_bunny
npc_bird_spirit
npc_soulgrinder
npc_supplicant
npc_spirit_prisoner_of_bladespire
npc_evergrove_druid
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/scripts/outland/world_outland.h"
#include "Entities/TemporarySpawn.h"
#include "Reputation/ReputationMgr.h"
#include "AI/ScriptDevAI/base/TimerAI.h"
#include "Spells/Spell.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

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

    SPELL_RESISTANCES           = 36630,
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

        DoCastSpellIfCan(nullptr, SPELL_RESISTANCES, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING))
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
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);
                m_bIsNihil = false;
            }

            // choose a new entry
            uint8 uiIndex = urand(0, MAX_ENTRIES - 1);

            // If we choose the same entry, try again
            while (aNetherDrakeEntries[uiIndex] == m_creature->GetEntry())
                uiIndex = urand(0, MAX_ENTRIES - 1);

            m_creature->CastSpell(nullptr, 35426, TRIGGERED_OLD_TRIGGERED); // arcane explosion visual
            if (m_creature->UpdateEntry(aNetherDrakeEntries[uiIndex]))
            {
                // Nihil does only dialogue
                if (aNetherDrakeEntries[uiIndex] == NPC_NIHIL)
                {
                    EnterEvadeMode();
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);
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
                        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
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

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
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
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_MANA_BURN, (SELECT_FLAG_PLAYER | SELECT_FLAG_POWER_MANA)))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_MANA_BURN) == CAST_OK)
                    m_uiManaBurnTimer = urand(8000, 16000);
            }
        }
        else
            m_uiManaBurnTimer -= uiDiff;

        if (m_uiArcaneBlastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ARCANE_BLAST) == CAST_OK)
                m_uiArcaneBlastTimer = urand(2500, 7500);
        }
        else
            m_uiArcaneBlastTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_mobs_nether_drake(Creature* pCreature)
{
    return new mobs_nether_drakeAI(pCreature);
}

/*######
## npc_daranelle
######*/

enum
{
    SAY_SPELL_INFLUENCE         = -1000174,
    SPELL_LASHHAN_CHANNEL       = 36904,
    SPELL_DISPELLING_ANALYSIS   = 37028
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

                m_creature->CastSpell(pWho, SPELL_DISPELLING_ANALYSIS, TRIGGERED_NONE);
            }
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }
};

UnitAI* GetAI_npc_daranelle(Creature* pCreature)
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
                if (Player* pSummoner = m_creature->GetMap()->GetPlayer(m_creature->GetSpawnerGuid()))
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
                CreatureList lOgreList;
                for (unsigned int aOgreEntrie : aOgreEntries)
                    GetCreatureListWithEntryInGrid(lOgreList, m_creature, aOgreEntrie, 30.0f);

                if (lOgreList.empty())
                {
                    m_uiStartTimer = 5000;
                    return;
                }

                // sort by distance and get only the closest
                lOgreList.sort(ObjectDistanceOrder(m_creature));

                CreatureList::const_iterator ogreItr = lOgreList.begin();
                Creature* pOgre = nullptr;

                do
                {
                    if ((*ogreItr)->IsAlive() && !(*ogreItr)->HasAura(SPELL_INTOXICATION))
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

UnitAI* GetAI_npc_bloodmaul_stout_trigger(Creature* pCreature)
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
    NPC_SIMON_GAME_BUNNY_LARGE      = 23378,

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
    PHASE_INACTIVE                  = 0,
    PHASE_LEVEL_PREPARE             = 1,
    PHASE_AI_GAME                   = 2,
    PHASE_PLAYER_PREPARE            = 3,
    PHASE_PLAYER_GAME               = 4,
    PHASE_LEVEL_FINISHED            = 5,

    SIMON_BIG_LEVEL_SUMMON          = 6,
    SIMON_LEVEL_VIBRATIONS          = 6,
    SIMON_LEVEL_EMANATIONS          = 8,
    SIMON_LEVEL_ENLIGHTENMENT       = 10,               // Also end of the game
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
    npc_simon_game_bunnyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsLargeEvent = m_creature->GetEntry() == NPC_SIMON_GAME_BUNNY_LARGE;
        Reset();
    }

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
        m_uiGamePhase  = PHASE_INACTIVE;
        m_bIsEventStarted = false;

        m_uiLevelCount = 0;
        m_uiLevelStage = 0;
        m_uiPlayerStage = 0;

        m_creature->RemoveAllAurasOnDeath(); // cleans up all auras
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
        }

        // prepare the buttons and summon the visual auras
        DoCastSpellIfCan(nullptr, m_bIsLargeEvent ? SPELL_PRE_GAME_BLUE_LARGE : SPELL_PRE_GAME_BLUE_AURA, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, m_bIsLargeEvent ? SPELL_PRE_GAME_GREEN_LARGE : SPELL_PRE_GAME_GREEN_AURA, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, m_bIsLargeEvent ? SPELL_PRE_GAME_RED_LARGE : SPELL_PRE_GAME_RED_AURA, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, m_bIsLargeEvent ? SPELL_PRE_GAME_YELLOW_LARGE : SPELL_PRE_GAME_YELLOW_AURA, CAST_TRIGGERED);

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
        DoCastSpellIfCan(nullptr, SPELL_GAME_START_RED, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_GAME_START_BLUE, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_GAME_START_GREEN, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_GAME_START_YELLOW, CAST_TRIGGERED);
    }

    // Complete level - called when one level is completed succesfully
    void DoCompleteLevel()
    {
        // lock the buttons
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_RED, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_BLUE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_GREEN, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_YELLOW, CAST_TRIGGERED);

        if (m_bIsLargeEvent)
        {
            if (m_uiLevelCount == SIMON_BIG_LEVEL_SUMMON)
            {
                BuffPlayers(SPELL_APEXIS_VIBRATIONS);
                if (Player* player = m_creature->GetMap()->GetPlayer(m_masterPlayerGuid))
                    player->CastSpell(player, SPELL_SIMON_GROUP_REWARD, TRIGGERED_OLD_TRIGGERED);
                DoCompleteGame();
            }
        }
        else
        {
            // Complete game if all the levels
            switch (m_uiLevelCount)
            {
                case SIMON_LEVEL_VIBRATIONS:
                    BuffPlayers(SPELL_APEXIS_VIBRATIONS);
                    break;
                case SIMON_LEVEL_EMANATIONS:
                    BuffPlayers(SPELL_APEXIS_EMANATIONS);
                    break;
                case SIMON_LEVEL_ENLIGHTENMENT:
                    BuffPlayers(SPELL_APEXIS_ENLIGHTENMENT);
                    DoCompleteGame();
                    break;
                default: break;
            }
        }
    }

    void BuffPlayers(uint32 buffId)
    {
        if (m_bIsLargeEvent)
        {
            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_masterPlayerGuid))
            {
                if (Group* pGroup = pPlayer->GetGroup())
                {
                    for (GroupReference* pRef = pGroup->GetFirstMember(); pRef != nullptr; pRef = pRef->next())
                    {
                        if (Player* pMember = pRef->getSource())
                        {
                            // distance check - they need to be close to the Apexis
                            if (!pMember->IsWithinDistInMap(m_creature, 20.0f))
                                continue;

                            DoCastSpellIfCan(pMember, buffId, CAST_TRIGGERED);
                        }
                    }
                }
                else
                    DoCastSpellIfCan(pPlayer, buffId, CAST_TRIGGERED);
            }
        }
        else
        {
            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_masterPlayerGuid))
                DoCastSpellIfCan(pPlayer, buffId, CAST_TRIGGERED);
        }
    }

    // Complete event - called when the game has been completed succesfully
    void DoCompleteGame()
    {
        // cleanup event after quest is finished
        DoCastSpellIfCan(m_creature, SPELL_SWITCHED_OFF, CAST_TRIGGERED);
        DoPlaySoundToSet(m_creature, SOUND_ID_DISABLE_NODE);
        Reset();
    }

    // Cleanup event - called when event fails
    void DoCleanupGame()
    {
        if (m_bIsLargeEvent)
            if (Player* player = m_creature->GetMap()->GetPlayer(m_masterPlayerGuid))
                player->CastSpell(player, SPELL_SIMON_GROUP_REWARD, TRIGGERED_OLD_TRIGGERED);

        // lock the buttons
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_RED, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_BLUE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_GREEN, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_GAME_END_YELLOW, CAST_TRIGGERED);

        //  unlock apexis and despawn
        DoCastSpellIfCan(m_creature, SPELL_SWITCHED_OFF, CAST_TRIGGERED);
        DoPlaySoundToSet(m_creature, SOUND_ID_DISABLE_NODE);
        Reset();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        switch (m_uiGamePhase)
        {
            case PHASE_INACTIVE:
                if (eventType == AI_EVENT_CUSTOM_A && !m_bIsEventStarted)
                {
                    m_uiGamePhase = PHASE_LEVEL_PREPARE;
                    m_masterPlayerGuid = pInvoker->GetObjectGuid();
                    m_uiLevelCount = 0;
                    DoPrepareLevel();
                    m_bIsEventStarted = true;
                    break;
                }
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
                    m_uiPlayerStage = 0;
                    if (uiMiscValue == aApexisGameData[m_vColors[m_uiLevelStage]].m_uiIntrospection)
                    {
                        DoCastSpellIfCan(m_creature, SPELL_GOOD_PRESS, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, aApexisGameData[m_vColors[m_uiLevelStage]].m_uiVisual, CAST_TRIGGERED);

                        DoPlaySoundToSet(m_creature, aApexisGameData[m_vColors[m_uiLevelStage]].m_uiSoundId);

                        // increase the level stage and reset the event counter
                        ++m_uiLevelStage;

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
                        DoCastSpellIfCan(pInvoker, SPELL_BAD_PRESS, CAST_TRIGGERED);
                        if (!m_bIsLargeEvent && !pInvoker->IsAlive()) // if player got killed on small event
                        {
                            DoCastSpellIfCan(m_creature, SPELL_VISUAL_GAME_FAILED, CAST_TRIGGERED);
                            DoCleanupGame();
                        }
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
                        return;
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

    }
};

UnitAI* GetAI_npc_simon_game_bunny(Creature* pCreature)
{
    return new npc_simon_game_bunnyAI(pCreature);
}

bool EffectDummyCreature_npc_simon_game_bunny(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (pCreatureTarget->GetEntry() != NPC_SIMON_GAME_BUNNY && pCreatureTarget->GetEntry() != NPC_SIMON_GAME_BUNNY_LARGE)
        return false;

    if (uiSpellId == SPELL_PRE_EVENT_TIMER && uiEffIndex == EFFECT_INDEX_0)
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
        if ((pCreatureTarget->GetEntry() == NPC_SIMON_GAME_BUNNY || pCreatureTarget->GetEntry() == NPC_SIMON_GAME_BUNNY_LARGE)
                && pCaster->GetTypeId() == TYPEID_PLAYER && originalCasterGuid.IsGameObject())
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
                if (Player* pSummoner = m_creature->GetMap()->GetPlayer(m_creature->GetSpawnerGuid()))
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
                float fX, fY, fZ;
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

UnitAI* GetAI_npc_light_orb_collector(Creature* pCreature)
{
    return new npc_light_orb_collectorAI(pCreature);
}

/*######
## npc_obelisk_trigger
######*/

enum
{
    NPC_TRIGGER = 20736,
    NPC_DOOMCRYER = 19963,

    SPELL_GREEN_BEAM = 35846
};

static const uint32 aObeliskEntries[] = { 185193, 185195, 185196, 185197, 185198 };

struct npc_obelisk_triggerAI : public ScriptedAI
{
    npc_obelisk_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    // Active count (updates every 1sec)
    uint8 m_uiActiveObelisk;

    // Timers
    uint32 m_uiActivateTimer;
    uint32 m_uiCheckTimer;
    uint32 m_uiBeamTimer;
    uint32 m_uiResetTimer;

    // Bool checks to prevent spam
    bool m_uiSpawnBoss;
    bool m_uiMarkForReset;

    void Reset() override
    {
        m_uiActiveObelisk = 0;
        m_uiCheckTimer = 1000;

        m_uiActivateTimer = 30 * IN_MILLISECONDS;
        m_uiBeamTimer = 5 * IN_MILLISECONDS;
        m_uiResetTimer= 120 * IN_MILLISECONDS;

        m_uiSpawnBoss = false;
        m_uiMarkForReset = false;

        std::list<GameObject*> lObelisk;
        for (uint8 i = 0; i < countof(aObeliskEntries); ++i)
            GetGameObjectListWithEntryInGrid(lObelisk, m_creature, aObeliskEntries[i], 100.0f);

        for (std::list<GameObject*>::iterator itr = lObelisk.begin(); itr != lObelisk.end(); ++itr)
        {
            if ((*itr)->GetGoState() == GO_STATE_ACTIVE && (*itr)->GetLootState() == GO_ACTIVATED)
            {
                (*itr)->ResetDoorOrButton();
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiCheckTimer)
        {
            if (m_uiCheckTimer <= uiDiff && m_uiActiveObelisk < 5)
            {
                m_uiActiveObelisk = 0;

                std::list<GameObject*> lObelisk;
                for (uint8 i = 0; i < countof(aObeliskEntries); ++i)
                    GetGameObjectListWithEntryInGrid(lObelisk, m_creature, aObeliskEntries[i], 100.0f);

                for (std::list<GameObject*>::iterator itr = lObelisk.begin(); itr != lObelisk.end(); ++itr)
                {
                    if ((*itr)->GetGoState() == GO_STATE_ACTIVE && (*itr)->GetLootState() == GO_ACTIVATED)
                        ++m_uiActiveObelisk;
                }

                m_uiCheckTimer = 1000;
            }
            else
                m_uiCheckTimer -= uiDiff;
        }
        
        if (m_uiActiveObelisk == 5)
        {
            if (m_uiActivateTimer)
            {
                if (m_uiActivateTimer <= uiDiff && !m_uiSpawnBoss)
                {
                    Creature* m_uiDoomcryer = m_creature->SummonCreature(NPC_DOOMCRYER, m_creature->GetPositionX(), m_creature->GetPositionY(), 283.65f, 0.32f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);

                    if (m_uiDoomcryer)
                        m_uiDoomcryer->GetMotionMaster()->MovePoint(1, 2882.60f, 4818.73f, 282.0f);

                    std::list<Creature*> lBunny;
                    GetCreatureListWithEntryInGrid(lBunny, m_creature, NPC_TRIGGER, 100.0f);
                    for (std::list<Creature*>::iterator itr = lBunny.begin(); itr != lBunny.end(); ++itr)
                        (*itr)->RemoveAllAuras();

                    m_uiSpawnBoss = true;
                    m_uiMarkForReset = true;
                }
                else
                {
                    if (m_uiBeamTimer && !m_uiSpawnBoss)
                    {
                        if (m_uiBeamTimer <= uiDiff)
                        {
                            std::list<Creature*> lBunny;
                            GetCreatureListWithEntryInGrid(lBunny, m_creature, NPC_TRIGGER, 100.0f);
                            for (std::list<Creature*>::iterator itr = lBunny.begin(); itr != lBunny.end(); ++itr)
                            {
                                std::list<GameObject*> lObelisk;
                                for (uint8 i = 0; i < countof(aObeliskEntries); ++i)
                                    GetGameObjectListWithEntryInGrid(lObelisk, (*itr), aObeliskEntries[i], 1.0f);

                                if (!(*itr)->HasAura(SPELL_GREEN_BEAM) && lObelisk.begin() != lObelisk.end())
                                {
                                    (*itr)->CastSpell(m_creature, SPELL_GREEN_BEAM, TRIGGERED_OLD_TRIGGERED);
                                    break;
                                }
                            }

                            m_uiBeamTimer = 5 * IN_MILLISECONDS;

                        }
                        else
                            m_uiBeamTimer -= uiDiff;
                    }

                    m_uiActivateTimer -= uiDiff;
                }
            }
        }

        if (m_uiMarkForReset)
        {
            if (m_uiResetTimer <= uiDiff)
            {
                Reset();
            }
            else
                m_uiResetTimer -= uiDiff;

        }

    }
};

UnitAI* GetAI_obelisk_triggerAI(Creature* pCreature)
{
    return new npc_obelisk_triggerAI(pCreature);
}

/*######
## npc_vimgol
######*/

enum
{
    NPC_VIMGOL_THE_VILE         = 22911,
    // NPC_VIMGOL_VISUAL_BUNNY     = 23040,
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

    void MovementInform(uint32 /*uiMovementType*/, uint32 /*uiData*/) override
    {
        m_creature->GetMotionMaster()->Clear();
        m_creature->CastSpell(m_creature, SPELL_UNHOLY_GROWTH, TRIGGERED_NONE);
        m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        m_creature->CastSpell(m_creature, SPELL_SUMMON_GRIMOIRE, TRIGGERED_OLD_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
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

UnitAI* GetAI_npc_vimgol(Creature* pCreature)
{
    return new npc_vimgol_AI(pCreature);
}

/*######
## npc_vimgol_visual_bunny
######*/

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

UnitAI* GetAI_npc_vimgol_visual_bunny(Creature* pCreature)
{
    return new npc_vimgol_visual_bunnyAI(pCreature);
}

/*######
## npc_vimgol_middle_bunny
######*/

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

        for (bool& m_uiActiveCircle : m_uiActiveCircles)
            m_uiActiveCircle = false;

        m_uiScanTimer = 0;
        m_uiBeamTimer = 5;
        m_uiFireballSkipCounter = 0;
    }

    uint8 playersInsideCircles()
    {
        uint32 tmpAuras[5] =
        {
            SPELL_VIMGOL_POP_TEST_A, SPELL_VIMGOL_POP_TEST_B, SPELL_VIMGOL_POP_TEST_C,
            SPELL_VIMGOL_POP_TEST_D, SPELL_VIMGOL_POP_TEST_E
        };
        uint8 tmpCounter = 0;

        if (m_uiBunnyGuids.size() < 5 && m_pMap)
        {
            m_uiBunnyGuids.clear();
            m_pMap->GetCreatureGuidVectorFromStorage(NPC_VIMGOL_VISUAL_BUNNY, m_uiBunnyGuids);
        }

        for (bool& m_uiActiveCircle : m_uiActiveCircles)
            m_uiActiveCircle = false;

        PlayerList playerList;
        GetPlayerListWithEntryInWorld(playerList, m_creature, 30);
        for (auto& itr : playerList)
        {
            if (!itr->HasAura(SPELL_VIMGOL_POP_TEST_A) && !itr->HasAura(SPELL_VIMGOL_POP_TEST_B) && !itr->HasAura(SPELL_VIMGOL_POP_TEST_C) &&
                    !itr->HasAura(SPELL_VIMGOL_POP_TEST_D) && !itr->HasAura(SPELL_VIMGOL_POP_TEST_E))
                continue;

            for (auto it = m_uiBunnyGuids.begin(); it != m_uiBunnyGuids.end(); ++it)
            {
                for (unsigned int tmpAura : tmpAuras)
                {
                    if (!itr->GetAura(tmpAura, SpellEffectIndex(0)))
                        continue;

                    if ((*it) != itr->GetAura(tmpAura, SpellEffectIndex(0))->GetCasterGuid())
                        continue;

                    m_uiActiveCircles[std::distance(m_uiBunnyGuids.begin(), it)] = true;
                }
            }
        }

        for (bool m_uiActiveCircle : m_uiActiveCircles)
            if (m_uiActiveCircle)
                ++tmpCounter;

        return tmpCounter;
    }

    void CastBunnySpell(Creature* pTarget, uint32 uSpell)
    {
        if (!uSpell)
            return;

        CreatureList creatureList;
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

        for (bool& m_uiActiveCircle : m_uiActiveCircles)
            m_uiActiveCircle = false;
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
                    m_creature->SummonCreature(NPC_VIMGOL_THE_VILE, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 90000);
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

UnitAI* GetAI_npc_vimgol_middle_bunny(Creature* pCreature)
{
    return new npc_vimgol_middle_bunnyAI(pCreature);
}

/*######
## npc_bird_spirit
######*/

enum
{
    NPC_TASKMASTER = 22160,
};

struct npc_bird_spiritAI : public ScriptedAI
{
    npc_bird_spiritAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint8 m_uiPhase;
    uint32 m_uiTimer;
    ObjectGuid m_taskmasterGuid;

    void Reset() override
    {
        m_uiTimer = 2000;
        m_uiPhase = 0;
    }

    void JustRespawned() override
    {
        Creature* taskmaster = GetClosestCreatureWithEntry(m_creature, NPC_TASKMASTER, 45.f);
        if (taskmaster)
        {
            m_creature->SetWalk(false, true);
            m_taskmasterGuid = taskmaster->GetObjectGuid();
        }
    }

    void MovementInform(uint32 /*uiMovementType*/, uint32 uiData) override
    {
        switch (uiData)
        {
            case 1: m_uiTimer = 2000; break;
            case 2: m_uiTimer = 2000; break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiTimer)
        {
            if (m_uiTimer <= uiDiff)
            {
                switch (m_uiPhase)
                {
                    case 0:
                    {
                        if (Creature* taskM = m_creature->GetMap()->GetCreature(m_taskmasterGuid))
                            m_creature->GetMotionMaster()->MovePoint(1, taskM->GetPositionX(), taskM->GetPositionY(), taskM->GetPositionZ());
                        m_uiTimer = 0;
                        m_uiPhase++;
                        break;
                    }
                    case 1:
                    {
                        m_creature->SetSelectionGuid(m_creature->GetSpawnerGuid());
                        if (Unit* summoner = m_creature->GetMap()->GetUnit(m_creature->GetSpawnerGuid()))
                            m_creature->GetMotionMaster()->MovePoint(2, summoner->GetPositionX(), summoner->GetPositionY(), summoner->GetPositionZ());
                        m_uiTimer = 0;
                        m_uiPhase++;
                        break;
                    }
                    case 2:
                    {
                        TemporarySpawn* summon = (TemporarySpawn*)m_creature;
                        summon->UnSummon();
                        m_uiTimer = 0;
                        m_uiPhase++;
                        break;
                    }
                }
            }
            else
                m_uiTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_bird_spirit(Creature* pCreature)
{
    return new npc_bird_spiritAI(pCreature);
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
        m_uiRendTimer = urand(8000, 10000);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
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

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiRendTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_REND) == CAST_OK)
                m_uiRendTimer = urand(34000, 36000);
        }
        else
            m_uiRendTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_bloodmaul_dire_wolf(Creature* pCreature)
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
        pCreatureTarget->CombatStop(true);

        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);
        return true;
    }

    return false;
}

/*######
## mobs_grishna_arrakoa
######*/

struct Prophecy
{
    uint32 creature; // Dummy quest credit creature
    int32 text; // Text whisper id
    float x; // Positions for tempsummon
    float y;
    float z;
    float o;
};

static const std::unordered_map<uint32, Prophecy> prophecies =
{
    {4613,{ (uint32)22798, (int32)-1015012, 3779.987061f, 6729.603027f, 180.498413f, 5.71490f }},
    {4615,{ (uint32)22799, (int32)-1015013, 3629.285889f, 6542.140137f, 155.004669f, 2.56267f }},
    {4616,{ (uint32)22800, (int32)-1015014, 3736.950439f, 6640.749023f, 133.674530f, 3.33629f }},
    {4617,{ (uint32)22801, (int32)-1015015, 3572.574219f, 6669.196289f, 128.455444f, 5.62290f }}
};

enum
{
    UNDERSTAND_RAVENSPEECH_SPELL    = 37466,
    UNDERSTAND_RAVENSPEECH_AURA     = 37642,
    QUEST_WHISPERS_OF_THE_RAVEN_GOD = 10607,
    // NPC_WHISPER_RAVEN_GOD_TEMPLATE  = 21851,
    NPC_VISION_RAVEN_GOD_TEMPLATE   = 21861,
};

bool AreaTrigger_at_raven_prophecy(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (/*pPlayer->IsGameMaster() ||*/ pPlayer->IsAlive() &&
                                       pPlayer->HasAura(UNDERSTAND_RAVENSPEECH_AURA) &&
        pPlayer->GetQuestStatus(QUEST_WHISPERS_OF_THE_RAVEN_GOD) == QUEST_STATUS_INCOMPLETE)
    {
        auto prophecyIterator = prophecies.find(pAt->id);
        if (prophecyIterator != prophecies.end())
        {
            Prophecy prophecy = prophecyIterator->second;
            //The quest required IDs are negative when they are game object so we must negate the game object ID
            if (pPlayer->GetReqKillOrCastCurrentCount(QUEST_WHISPERS_OF_THE_RAVEN_GOD, prophecy.creature) == 0)
            {
                if (Creature* whisper = ((ScriptedMap*)pPlayer->GetInstanceData())->GetSingleCreatureFromStorage(NPC_WHISPER_RAVEN_GOD_TEMPLATE))
                {
                    if (pPlayer->SummonCreature(NPC_VISION_RAVEN_GOD_TEMPLATE, prophecy.x, prophecy.y, prophecy.z, prophecy.o, TEMPSPAWN_TIMED_DESPAWN, 6000))
                    {
                        DoScriptText(prophecy.text, whisper, pPlayer);

                        //giving credit for the game object automatically negates the id, so we don't have to negate it 
                        pPlayer->KilledMonsterCredit(prophecy.creature);
                    }
                }
            }
        }
    }
    return false;
}

/*######
## npc_frequency_scanner
######*/

enum
{
    SPELL_OSCILLATION_FIELD             = 37408,
    SPELL_OSCILLATING_FREQUENCY_SCANNER = 37407,
    SPELL_SUMMON_TOP_BUNNY_CASTER       = 37392,
    SPELL_SUMMON_WYRM_FROM_BEYOND       = 37503,
    SPELL_SUMMON_SINGING_RIDGE_VOID_STORM = 37510,
    SPELL_SUMMON_AURA_GENERATOR_000     = 37373,
    SPELL_TOP_BUNNY_BEAM                = 37418,

    NPC_VOID_STORM          = 21798,
    NPC_TOP_BUNNY           = 21759,
    // NPC_OSCILLATING_FREQUENCY_SCANNER_MASTER_BUNNY = 21760,
    // NPC_WYRM_FROM_BEYOND    = 21796,

    MODEL_WYRM_FROM_BEYOND  = 20476,
};

// This is a first attempt to implement GO type 30 behaviour
struct go_aura_generator_000AI : public GameObjectAI
{
    go_aura_generator_000AI(GameObject* go) : GameObjectAI(go), m_auraSearchTimer(1000), m_spellInfo(sSpellTemplate.LookupEntry<SpellEntry>(SPELL_OSCILLATING_FREQUENCY_SCANNER)) {}

    uint32 m_auraSearchTimer;
    ObjectGuid m_player;
    SpellEntry const* m_spellInfo;

    void UpdateAI(const uint32 diff) override
    {
        if (m_auraSearchTimer <= diff)
        {
            m_auraSearchTimer = 1000;
            if (Player* player = m_go->GetMap()->GetPlayer(m_player))
            {
                float x, y, z;
                m_go->GetPosition(x, y, z);
                auto bounds = player->GetSpellAuraHolderBounds(m_spellInfo->Id);
                SpellAuraHolder* myHolder = nullptr;
                for (auto itr = bounds.first; itr != bounds.second; ++itr)
                {
                    SpellAuraHolder* holder = (*itr).second;
                    if (holder->GetCasterGuid() == m_go->GetObjectGuid())
                    {
                        myHolder = holder;
                        break;
                    }
                }
                bool isCloseEnough = player->GetDistance(x, y, z, DIST_CALC_COMBAT_REACH) < GetSpellRadius(sSpellRadiusStore.LookupEntry(m_spellInfo->EffectRadiusIndex[EFFECT_INDEX_0]));
                if (!myHolder)
                {
                    if (isCloseEnough)
                    {
                        myHolder = CreateSpellAuraHolder(m_spellInfo, player, m_go);
                        GameObjectAura* Aur = new GameObjectAura(m_spellInfo, EFFECT_INDEX_0, nullptr, nullptr, myHolder, player, m_go);
                        myHolder->AddAura(Aur, EFFECT_INDEX_0);
                        if (!player->AddSpellAuraHolder(myHolder))
                            delete myHolder;
                    }
                }
                else if (!isCloseEnough)
                    player->RemoveSpellAuraHolder(myHolder);
            }
        }
        else m_auraSearchTimer -= diff;
    }
};

struct npc_frequency_scanner : public ScriptedAI
{
    npc_frequency_scanner(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiOscillationFieldTimer;
    uint32 m_uiAttackOwnerTimer;

    ObjectGuid m_guidWyrm;

    bool m_bAttack;

    void Reset() override
    {
        m_uiOscillationFieldTimer = 0;
        m_uiAttackOwnerTimer = 0;
        m_bAttack = false;
        SetReactState(REACT_PASSIVE);
    }

    void JustRespawned() override
    {
        m_creature->CastSpell(nullptr, SPELL_SUMMON_TOP_BUNNY_CASTER, TRIGGERED_NONE);
        m_creature->CastSpell(nullptr, SPELL_SUMMON_AURA_GENERATOR_000, TRIGGERED_NONE);
    }

    void JustSummoned(Creature* creature) override
    {
        if (creature->GetEntry() == NPC_TOP_BUNNY)
            creature->CastSpell(nullptr, SPELL_TOP_BUNNY_BEAM, TRIGGERED_NONE);
    }

    void JustSummoned(GameObject* go) override
    {
        static_cast<go_aura_generator_000AI*>(go->AI())->m_player = m_creature->GetSpawnerGuid();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiOscillationFieldTimer <= uiDiff)
        {
            m_uiOscillationFieldTimer = 2000;
            m_creature->CastSpell(m_creature, SPELL_OSCILLATION_FIELD, TRIGGERED_NONE);
        }
        else
            m_uiOscillationFieldTimer -= uiDiff;
    }
};

UnitAI* GetAI_npc_frequency_scanner(Creature* creature)
{
    return new npc_frequency_scanner(creature);
}

GameObjectAI* GetGOAI_go_aura_generator_000(GameObject* go)
{
    return new go_aura_generator_000AI(go);
}

/*######
## npc_fel_cannon
######*/

enum
{
    SPELL_DEATHS_DOOR_FEL_CANNON = 39219,
    SPELL_ANTI_DEMON_FLAME_THROWER = 39222,
    SPELL_ARTILLERY_ON_THE_WARP_GATE = 39221,
    SPELL_FEL_CANNON_BLAST = 36242,
    SPELL_UNSTABLE_FEL_IMP_TRANSFORM = 39227, // cast in acid
    SPELL_UNSTABLE_EXPLOSION = 39266, // cast in acid

    SPELL_VOID_HOUND_TRANSFORM      = 39275, // TODO: Script should also spawn void hounds

    SPELL_GO_SMALL_FIRE = 49910, // serverside spells for spawning GOs - TODO: Remove and substitute with pre-spawned gos
    SPELL_GO_SMOKE = 49911,
    SPELL_GO_BIG_FIRE = 49912,

    SPELL_EXPLOSION = 30934,

    NPC_DEATHS_DOOR_FEL_CANNON = 22443,
    // NPC_DEATHS_DOOR_NORTH_WARP_GATE = 22471,
    // NPC_DEATHS_DOOR_SOUTH_WARP_GATE = 22472,
    NPC_UNSTABLE_FEL_IMP = 22474,
    NPC_DEATHS_DOOR_FEL_CANNON_TARGET_BUNNY = 22495,
    NPC_WARP_GATE_SHIELD = 23116, // not sure about this one
    NPC_DEATHS_DOOR_EXPLOSION_BUNNY = 22502,
    NPC_NORTH_WARP_GATE_CREDIT = 22503,
    NPC_SOUTH_WARP_GATE_CREDIT = 22504,

    GO_SMALL_FIRE = 185317,
    GO_SMOKE = 185318,
    GO_BIG_FIRE = 185319,

    NPC_EXPLOSION_BUNNY = 22502,
};

struct npc_fel_cannon : public Scripted_NoMovementAI
{
    npc_fel_cannon(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint32 m_uiCannonBlastTimer;

    bool m_bMCed;
    
    void Reset() override
    {
        m_uiCannonBlastTimer = 1000;
        m_bMCed = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    bool CanHandleCharm() override { return true; }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_DEATHS_DOOR_FEL_CANNON)
        {
            m_bMCed = true;
            if (Creature* target = GetClosestCreatureWithEntry(m_creature, NPC_DEATHS_DOOR_FEL_CANNON_TARGET_BUNNY, 100.f))
            {
                m_creature->SetFacingToObject(target);
                m_creature->FixateTarget(target);
            }

            m_creature->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);

            m_creature->GetCombatManager().SetLeashingDisable(true);
        }
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override {} // disable changing facing of any kind

    void EnterEvadeMode() override
    {
        if (!m_bMCed)
        {
            Scripted_NoMovementAI::EnterEvadeMode();
            m_creature->FixateTarget(nullptr);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_bMCed && !m_creature->HasCharmer())
        {            
            Scripted_NoMovementAI::EnterEvadeMode();
            m_creature->FixateTarget(nullptr);
            return;
        }

        if (!m_bMCed)
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
                return;

            if (m_uiCannonBlastTimer <= uiDiff)
            {
                m_uiCannonBlastTimer = 2500;
                m_creature->CastSpell(m_creature->GetVictim(), SPELL_FEL_CANNON_BLAST, TRIGGERED_NONE);
            }
            else
                m_uiCannonBlastTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_fel_cannon(Creature* pCreature)
{
    return new npc_fel_cannon(pCreature);
}

/*######
## npc_warp_gate
######*/

static float impSpawns[2][4] = 
{
    { 2188.340f, 5476.629f, 155.069f, 5.259f }, // north
    { 1981.730f, 5315.390f, 156.600f, 0.262f} // south
};

static float fireSpawns[14][3] =
{
    { 2199.255f,5474.860f,153.578f},
    { 2185.102f,5470.938f,164.374f },
    { 2185.104f,5484.197f,154.817f },
    { 2188.340f, 5476.629f, 155.069f },
    { 2194.513f,5481.483f,164.991f },
    { 2178.581f,5477.184f, 157.264f},
    { 2188.340f, 5476.629f, 155.069f},
    { 1982.273f,5331.832f,153.952f },
    { 1987.307f,5307.395f,169.677f },
    { 1980.868f,8325.251f,169.290f },
    { 1981.730f, 5315.390f, 156.600f },
    { 1989.396f,5322.008f,155.384f },
    { 1973.351f,5299.629f,155.395f},
    { 1981.730f, 5315.390f, 156.600f }
};

struct npc_warp_gate : public Scripted_NoMovementAI
{
    npc_warp_gate(Creature* pCreature) : Scripted_NoMovementAI(pCreature), m_resetTimer(0) { Reset(); }

    uint32 m_uiHitCounter;
    uint32 m_uiSpawnImpTimer;
    uint32 m_resetTimer;

    ObjectGuid m_guidFelCannon;
    ObjectGuid m_guidSmoke;

    std::vector<ObjectGuid> m_vImpGuids;
    
    void Reset() override
    {
        m_uiHitCounter = 0;
        m_uiSpawnImpTimer = 0;

        for (ObjectGuid& guid : m_vImpGuids)
            if (Creature* imp = m_creature->GetMap()->GetCreature(guid))
                imp->ForcedDespawn(100);

        m_vImpGuids.clear();

        m_guidSmoke = ObjectGuid();

        if (m_creature->IsAlive())
        {
            float x, y, z, ori;
            m_creature->GetRespawnCoord(x, y, z, &ori);
            m_creature->SetOrientation(ori);
            m_creature->SetFacingTo(ori);
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_ARTILLERY_ON_THE_WARP_GATE)
        {
            if (!m_uiHitCounter)
            {
                if (m_resetTimer)
                {
                    m_resetTimer = 0;
                    m_creature->GetInstanceData()->SetData(m_creature->GetEntry() == NPC_DEATHS_DOOR_NORTH_WARP_GATE ? TYPE_DEATHS_DOOR_NORTH : TYPE_DEATHS_DOOR_SOUTH, 0);
                }
                m_guidFelCannon = pCaster->GetObjectGuid();
            }
            
            uint32 spellId;

            switch (m_uiHitCounter)
            {
                case 0:
                case 1:
                case 2:
                case 4:
                case 5:
                {
                    spellId = SPELL_GO_SMALL_FIRE;
                    break;
                }
                case 3:
                {
                    spellId = SPELL_GO_SMOKE;
                    break;
                }
                case 6:
                {
                    spellId = SPELL_GO_BIG_FIRE;
                    if (GameObject* smoke = m_creature->GetMap()->GetGameObject(m_guidSmoke))
                        smoke->AddObjectToRemoveList();
                    if (Creature* cannon = m_creature->GetMap()->GetCreature(m_guidFelCannon))
                    {
                        if (Player* player = dynamic_cast<Player*>(cannon->GetCharmer()))
                        {
                            player->RewardPlayerAndGroupAtEventCredit(m_creature->GetEntry() == NPC_DEATHS_DOOR_NORTH_WARP_GATE ? NPC_NORTH_WARP_GATE_CREDIT : NPC_SOUTH_WARP_GATE_CREDIT, m_creature);
                        }
                    }
                    if (Creature* explosionBunny = GetClosestCreatureWithEntry(m_creature, NPC_EXPLOSION_BUNNY, 30.f))
                        explosionBunny->CastSpell(nullptr, SPELL_EXPLOSION, TRIGGERED_NONE);
                    break;
                }
            }
            uint32 i = m_creature->GetEntry() - NPC_DEATHS_DOOR_NORTH_WARP_GATE;
            m_creature->CastSpell(fireSpawns[i * 7 + m_uiHitCounter][0], fireSpawns[i * 7 + m_uiHitCounter][1], fireSpawns[i * 7 + m_uiHitCounter][2], spellId, TRIGGERED_OLD_TRIGGERED);

            m_uiHitCounter++;
            m_creature->GetInstanceData()->SetData(m_creature->GetEntry() == NPC_DEATHS_DOOR_NORTH_WARP_GATE ? TYPE_DEATHS_DOOR_NORTH : TYPE_DEATHS_DOOR_SOUTH, m_uiHitCounter);
            if (m_uiHitCounter == 7)
            {
                m_resetTimer = 30000;
                Reset();
            }
        }
    }

    void JustSummoned(GameObject* pGo) override
    {
        if (pGo->GetEntry() == GO_SMOKE)
            m_guidSmoke = pGo->GetObjectGuid();
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_UNSTABLE_FEL_IMP)
        {
            m_vImpGuids.push_back(pSummoned->GetObjectGuid());
            pSummoned->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pSummoned->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            if (Creature* cannon = m_creature->GetMap()->GetCreature(m_guidFelCannon))
            {
                pSummoned->AI()->AttackStart(cannon);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiHitCounter)
        {
            Creature* cannon = m_creature->GetMap()->GetCreature(m_guidFelCannon);
            if (!cannon || !cannon->HasCharmer())
            {
                Reset();
                return;
            }

            if (m_uiSpawnImpTimer <= uiDiff)
            {
                uint32 i = m_creature->GetEntry() - NPC_DEATHS_DOOR_NORTH_WARP_GATE;
                if (m_creature->SummonCreature(NPC_UNSTABLE_FEL_IMP, impSpawns[i][0], impSpawns[i][1], impSpawns[i][2], impSpawns[i][3], TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 20000, true, true))
                    m_uiSpawnImpTimer = 3000;
            }
            else
                m_uiSpawnImpTimer -= uiDiff;
        }

        if (m_resetTimer)
        {
            if (m_resetTimer <= uiDiff)
            {
                m_creature->GetInstanceData()->SetData(m_creature->GetEntry() == NPC_DEATHS_DOOR_NORTH_WARP_GATE ? TYPE_DEATHS_DOOR_NORTH : TYPE_DEATHS_DOOR_SOUTH, 0);
                m_resetTimer = 0;
            }
            else m_resetTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_warp_gate(Creature* pCreature)
{
    return new npc_warp_gate(pCreature);
}

/*######
## npc_soulgrinder
######*/

enum
{
    NPC_SOULGRINDER = 23019,
    NPC_OGRE_SPIRIT = 22912,
    NPC_SKULLOC_SOULGRINDER = 22910,
    NPC_SOULGRINDER_BUNNY = 23037,

    SPELL_SOULGRINDER_RITUAL_VISUAL_BUNNY = 39936,
    SPELL_BEAM = 39920,
    SPELL_SHADOWFORM_1 = 39943,
    SPELL_SHADOWFORM_2 = 39944,
    SPELL_SHADOWFORM_3 = 39946,
    SPELL_SHADOWFORM_4 = 39947,
    SPELL_SHADOWFORM_TEST = 39951,

    SPELL_VIMGOL_CANTATION                      = 40328, // item spell that triggers event
    SPELL_SOULGRINDER_RITUAL_VISUAL_IN_PROGRESS = 39918, // added in creature_template_addon
    SPELL_SCARE_SOULGRINDER_GHOST = 39914,

    SPELL_SOULGRINDER_GHOST_TRANSFORM   = 39916,
    SPELL_SPIRIT_PARTICLES_PURPLE       = 28126,
    SPELL_SOULGRINDER_GHOST_SPAWN_IN    = 17321,

    SAY_SKULLOC_SOULGRINDER = -1015016,
    SAY_FAIL_QUEST          = -1015017,
};

bool ProcessEventId_Soulgrinder(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (GetClosestCreatureWithEntry((WorldObject*)pSource, NPC_SOULGRINDER, 20))
    {
        return false;
    }
    Player* player = (Player*)pSource;
    player->SummonCreature(NPC_SOULGRINDER, 3535.111f, 5590.628f, 0.3859383f, 0.7853982f, TEMPSPAWN_TIMED_OR_CORPSE_DESPAWN, 265000);
    return true;
}

struct npc_soulgrinderAI : public ScriptedAI
{
    npc_soulgrinderAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetReactState(REACT_PASSIVE);
        m_uiTimer = 7000; // need to be set once, without regard for combat
        m_uiPhase = 0;
        m_uiOgreCounter = 0;
        m_ogreAttackTimer = 0;
        Reset();
    }

    uint8 m_uiPhase;
    uint32 m_uiTimer;
    uint8 m_uiOgreCounter;
    uint32 m_ogreAttackTimer;
    GuidVector m_ogreSpirits;
    GuidVector m_soulgrinderDummies;
    ObjectGuid m_skullocSoulgrinder;

    void Reset() override
    {

    }

    void JustRespawned() override
    {
        // 26 spawns that are spawned immediately with this NPC
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3574.525f, 5624.78f,  -9.746364f, 5.707227f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3558.613f, 5543.979f, -2.24089f, 0.9948376f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3542.293f, 5546.184f, -2.307737f, 5.096361f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3507.783f, 5607.564f, -2.115761f, 2.146755f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3525.318f, 5624.862f, -5.340961f, 0.2792527f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3573.157f, 5592.371f, -4.614707f, 2.984513f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3542.814f, 5622.923f, -3.886532f, 0.1919862f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3574.056f, 5574.628f, -5.577652f, 0.2792527f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3559.653f, 5559.587f, -2.850421f, 3.01942f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3574.718f, 5609.495f, -3.285291f, 1.32645f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3541.38f,  5559.73f,  -2.665792f, 2.827433f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3542.222f, 5573.395f, -2.64414f, 4.276057f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3557.254f, 5591.203f, -2.993953f, 3.01942f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3558.495f, 5577.132f, -3.076451f, 3.892084f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3557.081f, 5612.431f, -4.562363f, 1.518436f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3542.203f, 5608.064f, -3.378048f, 0.1570796f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3558.009f, 5626.f   , -5.146327f, 3.857178f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3524.67f,  5608.019f, -3.321486f, 2.042035f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3510.031f, 5575.792f, -1.80589f, 4.904375f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3508.348f, 5589.707f, -0.8961017f, 1.623156f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3507.988f, 5561.777f,  1.993673f, 2.80998f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3509.509f, 5625.191f, -4.461998f, 0.06981317f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3525.131f, 5573.122f, -1.622976f, 6.126106f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3493.974f, 5594.064f, -0.03712566f, 2.670354f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3526.678f, 5560.159f, -0.8016807f, 0.9424778f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_OGRE_SPIRIT, 3574.228f, 5559.057f, -6.009155f, 6.021386f, TEMPSPAWN_CORPSE_DESPAWN, 1000);
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_OGRE_SPIRIT:
                m_ogreSpirits.push_back(summoned->GetObjectGuid());
                break;
            case NPC_SKULLOC_SOULGRINDER:
                m_skullocSoulgrinder = summoned->GetObjectGuid();
                break;
            case NPC_SOULGRINDER_BUNNY:
                m_soulgrinderDummies.push_back(summoned->GetObjectGuid());
                break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        for (ObjectGuid& guid : m_soulgrinderDummies)
            if (Creature* dummy = m_creature->GetMap()->GetCreature(guid))
                dummy->ForcedDespawn();

        for (ObjectGuid& guid : m_ogreSpirits)
            if (Creature* spirit = m_creature->GetMap()->GetCreature(guid))
                spirit->ForcedDespawn();

        if (Player* summoner = m_creature->GetMap()->GetPlayer(m_creature->GetSpawnerGuid()))
            summoner->FailQuest(11000);

        if (Creature* gronn = m_creature->GetMap()->GetCreature(m_skullocSoulgrinder))
        {
            DoScriptText(SAY_FAIL_QUEST, gronn);
            gronn->ForcedDespawn();
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SKULLOC_SOULGRINDER)
        {
            for (ObjectGuid& guid : m_soulgrinderDummies)
                if (Creature* dummy = m_creature->GetMap()->GetCreature(guid))
                    dummy->ForcedDespawn();

            m_creature->ForcedDespawn();
        }
    }

    void SummonedCreatureDespawn(Creature* pSummoned) override
    {
        if (pSummoned == m_creature)
        {
            for (ObjectGuid& guid : m_soulgrinderDummies)
                if (Creature* dummy = m_creature->GetMap()->GetCreature(guid))
                    dummy->ForcedDespawn();

            if (Creature* gronn = m_creature->GetMap()->GetCreature(m_skullocSoulgrinder))
                if (gronn->IsAlive())
                    gronn->ForcedDespawn();
        }
    }

    void SpawnCreature()
    {
        if (Creature* ogre = m_creature->GetMap()->GetCreature(m_ogreSpirits[m_uiOgreCounter]))
        {
            ogre->AI()->SetReactState(REACT_DEFENSIVE);
            ogre->CastSpell(ogre, SPELL_SOULGRINDER_GHOST_TRANSFORM, TRIGGERED_NONE);
            ogre->CastSpell(ogre, SPELL_SPIRIT_PARTICLES_PURPLE, TRIGGERED_NONE);
            ogre->CastSpell(ogre, SPELL_SOULGRINDER_GHOST_SPAWN_IN, TRIGGERED_NONE);
            ogre->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            ogre->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            ogre->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            m_ogreAttackTimer = urand(2000,4000);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_ogreAttackTimer) // Spirits attack 2 seconds after being "spawned in"
        {
            if (m_ogreAttackTimer <= uiDiff)
            {
                m_ogreAttackTimer = 0;
                if (Creature* ogre = m_creature->GetMap()->GetCreature(m_ogreSpirits[m_uiOgreCounter - 1]))
                {
                    ogre->AI()->AttackStart(m_creature);
                    ogre->AI()->SetReactState(REACT_AGGRESSIVE);
                }
            }
            else
                m_ogreAttackTimer -= uiDiff;
        }

        if (m_uiTimer)
        {
            if (m_uiTimer <= uiDiff)
            {
                switch (m_uiPhase)
                {
                    case 0:
                    {
                        SpawnCreature();
                        m_uiTimer = 6000;
                        m_uiOgreCounter++;
                        if (m_uiOgreCounter == 2)
                        {
                            Creature * bunny = m_creature->SummonCreature(NPC_SOULGRINDER_BUNNY, 3491.224f, 5529.023f, 17.14335f, 6.195919f, TEMPSPAWN_MANUAL_DESPAWN, 0);
                            Creature * gronn = m_creature->SummonCreature(NPC_SKULLOC_SOULGRINDER, 3478.563f, 5550.74f, 7.838801f, 0.3665192f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 300000);
                            bunny->CastSpell(m_creature, SPELL_BEAM, TRIGGERED_NONE);
                            gronn->GetMotionMaster()->MoveIdle();
                            gronn->CastSpell(gronn, SPELL_SHADOWFORM_1, TRIGGERED_NONE);
                        }
                        else if (m_uiOgreCounter == 8)
                        {
                            Creature * bunny = m_creature->SummonCreature(NPC_SOULGRINDER_BUNNY, 3464.954f, 5564.684f, 17.99977f, 5.67232f, TEMPSPAWN_MANUAL_DESPAWN, 0);
                            bunny->CastSpell(m_creature, SPELL_BEAM, TRIGGERED_NONE);
                            if (Creature* gronn = m_creature->GetMap()->GetCreature(m_skullocSoulgrinder))
                            {
                                gronn->CastSpell(gronn, SPELL_SHADOWFORM_2, TRIGGERED_NONE);
                                gronn->CastSpell(gronn, SPELL_SHADOWFORM_TEST, TRIGGERED_NONE);
                            }
                        }
                        else if (m_uiOgreCounter == 14)
                        {
                            Creature * bunny = m_creature->SummonCreature(NPC_SOULGRINDER_BUNNY, 3515.02f, 5524.39f, 16.8927f, 3.560477f, TEMPSPAWN_MANUAL_DESPAWN, 0);
                            bunny->CastSpell(m_creature, SPELL_BEAM, TRIGGERED_NONE);
                            if (Creature* gronn = m_creature->GetMap()->GetCreature(m_skullocSoulgrinder))
                            {
                                gronn->CastSpell(gronn, SPELL_SHADOWFORM_3, TRIGGERED_NONE);
                                gronn->CastSpell(gronn, SPELL_SHADOWFORM_TEST, TRIGGERED_NONE);
                            }
                        }
                        else if (m_uiOgreCounter == 20)
                        {
                            Creature * bunny = m_creature->SummonCreature(NPC_SOULGRINDER_BUNNY, 3468.82f, 5581.41f, 17.5205f, 0.0174533f, TEMPSPAWN_MANUAL_DESPAWN, 0);
                            bunny->CastSpell(m_creature, SPELL_BEAM, TRIGGERED_NONE);
                            if (Creature* gronn = m_creature->GetMap()->GetCreature(m_skullocSoulgrinder))
                            {
                                gronn->RemoveAurasDueToSpell(SPELL_SHADOWFORM_1);
                                gronn->RemoveAurasDueToSpell(SPELL_SHADOWFORM_2);
                                gronn->RemoveAurasDueToSpell(SPELL_SHADOWFORM_3);
                                gronn->CastSpell(gronn, SPELL_SHADOWFORM_4, TRIGGERED_NONE);
                                gronn->CastSpell(gronn, SPELL_SHADOWFORM_TEST, TRIGGERED_NONE);
                            }
                        }
                        else if (m_uiOgreCounter == 25)
                        {
                            for (ObjectGuid& guid : m_soulgrinderDummies)
                                if (Creature* dummy = m_creature->GetMap()->GetCreature(guid))
                                    dummy->InterruptSpell(CURRENT_CHANNELED_SPELL);

                            for (ObjectGuid& guid : m_ogreSpirits)
                                if (Creature* spirit = m_creature->GetMap()->GetCreature(guid))
                                    spirit->ForcedDespawn();
                            
                            m_creature->CastSpell(nullptr, SPELL_SCARE_SOULGRINDER_GHOST, TRIGGERED_NONE);
                            if (Creature* gronn = m_creature->GetMap()->GetCreature(m_skullocSoulgrinder))
                            {
                                gronn->RemoveAurasDueToSpell(SPELL_SHADOWFORM_4);
                                gronn->CastSpell(gronn, SPELL_SHADOWFORM_4, TRIGGERED_NONE);
                                gronn->CastSpell(gronn, SPELL_SHADOWFORM_TEST, TRIGGERED_NONE);
                                m_creature->CastSpell(gronn, SPELL_BEAM, TRIGGERED_NONE);
                            }
                            m_uiTimer = 4000;
                            m_uiPhase++;
                        }
                        return;
                    }
                    case 1:
                    {
                        if (Creature* gronn = m_creature->GetMap()->GetCreature(m_skullocSoulgrinder))
                        {
                            DoScriptText(SAY_SKULLOC_SOULGRINDER, gronn);
                            gronn->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                            gronn->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        }
                        m_uiTimer = 6000;
                        m_uiPhase++;
                        return;
                    }
                    case 2:
                    {
                        if (Creature* gronn = m_creature->GetMap()->GetCreature(m_skullocSoulgrinder))
                        {
                            gronn->RemoveAurasDueToSpell(SPELL_SHADOWFORM_4);
                            m_creature->InterruptSpell(CURRENT_CHANNELED_SPELL);
                        }
                        m_uiTimer = 0;
                    }
                }
            }
            else
                m_uiTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_soulgrinder(Creature* pCreature)
{
    return new npc_soulgrinderAI(pCreature);
}

/*######
## npc_supplicant
######*/

enum
{
    QUEST_INTO_THE_SOULGRINDER = 11000,

    NPC_BLOODMAUL_SUPPLICANT = 23052,
    NPC_BLADESPIRE_SUPPLICANT = 23053,
    NPC_OGRE_DRUM_BUNNY = 23056,

    MUG_EQUIP = 2703,
    DRUMMER_EQUIP = 5301,

    SAY_MOGDORG_1 = -1001181,

    SAY_SUPPLICANT_LOS_1 = -1001186,
    SAY_SUPPLICANT_LOS_2 = -1001187,
    SAY_SUPPLICANT_LOS_3 = -1001188,
    SAY_SUPPLICANT_LOS_4 = -1001235,
    SAY_SUPPLICANT_LOS_5 = -1001236,
    SAY_EMOTE_RESPONSE_KISS = -1001184,
    SAY_EMOTE_RESPONSE_POINT = -1001185,
    SAY_EMOTE_RESPONSE_ANGRY = -1001234,
    SAY_EMOTE_RESPONSE_FART_1 = -1001232,
    SAY_EMOTE_RESPONSE_FART_2 = -1001233,

    SPELL_OGRE_FORCE_REACTION = 39960,
    SPELL_BLOODMAUL_INTOXICATION_VISUAL = 35777,

    RANDOM_SOUND_1 = 11627,
    RANDOM_SOUND_2 = 11628,
    RANDOM_SOUND_3 = 11629,
    RANDOM_SOUND_4 = 11630,
    CHEER_SOUND = 396,
    ROAR_SOUND = 402,

    DBSCRIPT_SPAWN_OGRES = 10104,

    PHASE_KNEEL_TO_PLAYER = 1,
    PHASE_UNKNEEL_TO_PLAYER = 2,
    PHASE_TURN_TO_OGRE = 3,
    PHASE_MASS_KNEEL_TO_PLAYER = 4,
    PHASE_DRUMMER = 5
};

bool QuestRewarded_into_the_soulgrinder(Player* player, Creature* pCreature, Quest const* pQuest)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

    if (!pInstance)
        return true;

    if (pQuest->GetQuestId() == QUEST_INTO_THE_SOULGRINDER)
    {
        DoScriptText(SAY_MOGDORG_1, pCreature, player);
        player->CastSpell(player, SPELL_OGRE_FORCE_REACTION, TRIGGERED_OLD_TRIGGERED);
        player->GetMap()->ScriptsStart(sRelayScripts, DBSCRIPT_SPAWN_OGRES, player, pCreature);
    }

    return true;
}

struct npc_supplicantAI : public ScriptedAI
{
    npc_supplicantAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_summonerGuid = m_creature->GetSpawnerGuid();
        m_last = static_cast<uint32>(-1);
        Reset();
    }

    ObjectGuid m_summonerGuid; // player who summoned this

    uint32 m_timer;
    uint32 m_randomStuffTimer;
    uint32 m_drinkTimer;
    uint32 m_randomSoundTimer;

    uint8 m_phase;

    GuidVector m_supplicants;
    int32 m_last;

    void Reset() override
    {
        m_timer = 0;
        m_randomStuffTimer = 0;
        m_drinkTimer = 0;
        m_randomSoundTimer = 0;

        m_phase = PHASE_MASS_KNEEL_TO_PLAYER;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (pSender != m_creature) // Sender should always be the creature itself
            return;

        if (eventType == AI_EVENT_CUSTOM_A) // Start doing random dance/turn/roar etc.
        {
            CreatureList list;
            GetCreatureListWithEntryInGrid(list, m_creature, NPC_BLOODMAUL_SUPPLICANT, 10.f); // guesswork
            GetCreatureListWithEntryInGrid(list, m_creature, NPC_BLADESPIRE_SUPPLICANT, 10.f);

            for (Creature* supplicant : list)
                m_supplicants.push_back(supplicant->GetObjectGuid());

            m_phase = PHASE_TURN_TO_OGRE;
            m_timer = 1;
            m_randomStuffTimer = 1;
        }
        else if (eventType == AI_EVENT_CUSTOM_B) // Initial kneel
        {
            if (Unit* summoner = ((TemporarySpawn*)m_creature)->GetSpawner())
                m_creature->SetFacingToObject(summoner);
        }
        else if (eventType == AI_EVENT_CUSTOM_C) // Drummers
        {
            m_phase = PHASE_DRUMMER;
            m_timer = 1;
            // Equip should change after the initial kneel
            m_creature->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, DRUMMER_EQUIP);
        }
        else if (eventType == AI_EVENT_CUSTOM_D) // Ogres making random sounds
        {
            m_randomSoundTimer = urand(4000, 15000);
        }
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (pWho->GetObjectGuid() != m_summonerGuid)
            return;

        if (m_phase == PHASE_MASS_KNEEL_TO_PLAYER || m_phase == PHASE_KNEEL_TO_PLAYER || m_phase == PHASE_UNKNEEL_TO_PLAYER || m_phase == PHASE_DRUMMER)
            return;

        if (m_creature->IsWithinDistInMap(pWho, 7.0f))
        {
            ResetOgreState(true);

            m_creature->SetFacingToObject(pWho);

            if (urand(0, 100) <= 5)
            {
                switch (urand(0, 4))
                {
                case 0:
                    DoScriptText(SAY_SUPPLICANT_LOS_1, m_creature, pWho);
                    break;
                case 1:
                    DoScriptText(SAY_SUPPLICANT_LOS_2, m_creature, pWho);
                    break;
                case 2:
                    DoScriptText(SAY_SUPPLICANT_LOS_3, m_creature, pWho);
                    break;
                case 3:
                    DoScriptText(SAY_SUPPLICANT_LOS_4, m_creature, pWho);
                    break;
                case 4:
                    DoScriptText(SAY_SUPPLICANT_LOS_5, m_creature, pWho);
                    break;
                default:
                    m_creature->MonsterSay("Error", 0);
                    break;
                }
            }

            m_phase = PHASE_KNEEL_TO_PLAYER;
            m_timer = 1500;
        }
    }

    void ReceiveEmote(Player* pPlayer, uint32 uiEmote) override
    {
        if (pPlayer->GetObjectGuid() != m_summonerGuid)
            return;

        switch (uiEmote)
        {
        case TEXTEMOTE_DANCE:
            m_creature->HandleEmote(EMOTE_ONESHOT_DANCE);
            break;
        case TEXTEMOTE_WAVE:
            m_creature->HandleEmote(EMOTE_ONESHOT_WAVE);
            break;
        case TEXTEMOTE_ROAR:
            m_creature->HandleEmote(EMOTE_ONESHOT_ROAR);
            break;
        case TEXTEMOTE_POINT:
            DoScriptText(SAY_EMOTE_RESPONSE_POINT, m_creature, pPlayer);
            break;
        case TEXTEMOTE_KISS:
            DoScriptText(SAY_EMOTE_RESPONSE_KISS, m_creature, pPlayer);
            break;
        case TEXTEMOTE_ANGRY:
            DoScriptText(SAY_EMOTE_RESPONSE_ANGRY, m_creature, pPlayer);
            break;
        case TEXTEMOTE_FART:
            DoScriptText(SAY_EMOTE_RESPONSE_FART_1, m_creature, pPlayer);
            DoScriptText(SAY_EMOTE_RESPONSE_FART_2, m_creature, pPlayer);
            break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_timer)
        {
            if (m_timer <= uiDiff)
            {
                switch (m_phase)
                {
                case PHASE_KNEEL_TO_PLAYER:
                {
                    m_randomStuffTimer = 0;
                    m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);

                    m_phase = PHASE_UNKNEEL_TO_PLAYER;
                    m_timer = 15000;
                    break;
                }
                case PHASE_UNKNEEL_TO_PLAYER:
                {
                    m_creature->SetStandState(UNIT_STAND_STATE_STAND);

                    m_phase = PHASE_TURN_TO_OGRE;
                    m_timer = 1200;
                    m_randomStuffTimer = 5000;

                    break;
                }
                case PHASE_TURN_TO_OGRE:
                {
                    m_creature->HandleEmoteState(EMOTE_STATE_NONE); // Stop dancing

                    // turn to random ogre
                    if (m_supplicants.size() > 1) // safeguard
                    {
                        uint32 random = urand(0, m_supplicants.size() - 1);
                        if (random == m_last)
                            m_last = (m_last + 1) % (m_supplicants.size() - 1);
                        if (Creature* supplicant = m_creature->GetMap()->GetCreature(m_supplicants[random]))
                            m_creature->SetFacingToObject(supplicant);
                        m_last = random;
                    }
                    else if (m_supplicants.size() == 1 && m_last == -1) // only need to do this once
                    {
                        if (Creature* supplicant = m_creature->GetMap()->GetCreature(m_supplicants[0]))
                            m_creature->SetFacingToObject(supplicant);
                        m_last = 0;
                    }

                    m_timer = urand(3600, 45000);
                    break;
                }
                case PHASE_DRUMMER:
                {
                    if (Creature* drumBunny = GetClosestCreatureWithEntry(m_creature, NPC_OGRE_DRUM_BUNNY, 10))
                    {
                        m_creature->SetFacingToObject(drumBunny);
                        m_creature->HandleEmote(EMOTE_ONESHOT_CUSTOMSPELL01);
                    }

                    m_timer = urand(3600, 7300);
                    break;
                }
                }
            }
            else
                m_timer -= uiDiff;

            if (m_randomStuffTimer)
            {
                if (m_randomStuffTimer <= uiDiff)
                {
                    ResetOgreState(false);

                    // Do random stuff
                    switch (urand(0, 4))
                    {
                    case 0:
                        m_creature->HandleEmoteState(EMOTE_STATE_DANCE);
                        break;
                    case 1:
                        m_creature->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, MUG_EQUIP);
                        m_creature->CastSpell(m_creature, SPELL_BLOODMAUL_INTOXICATION_VISUAL, TRIGGERED_NONE);
                        m_drinkTimer = 1;
                        break;
                    case 2:
                        m_creature->HandleEmote(EMOTE_ONESHOT_ROAR);
                        if (urand(0, 100) <= 20)
                            m_creature->PlayDistanceSound(ROAR_SOUND);
                        break;
                    case 3:
                        m_creature->HandleEmote(EMOTE_ONESHOT_CHEER);
                        if (urand(0, 100) <= 20)
                            m_creature->PlayDistanceSound(CHEER_SOUND);
                        break;
                    case 4:
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        break;
                    default:
                        m_creature->MonsterSay("Error", 0);
                        break;
                    }

                    m_randomStuffTimer = urand(4800, 16000);
                }
                else
                    m_randomStuffTimer -= uiDiff;
            }

            if (m_drinkTimer)
            {
                if (m_drinkTimer <= uiDiff)
                {
                    m_creature->HandleEmote(EMOTE_ONESHOT_EAT_NOSHEATHE);
                    m_drinkTimer = 1200;
                }
                else
                    m_drinkTimer -= uiDiff;
            }

            if (m_randomSoundTimer)
            {
                if (m_randomSoundTimer <= uiDiff)
                {
                    switch (urand(1, 4))
                    {
                    case 1:
                        m_creature->PlayDirectSound(RANDOM_SOUND_1);
                        break;
                    case 2:
                        m_creature->PlayDirectSound(RANDOM_SOUND_2);
                        break;
                    case 3:
                        m_creature->PlayDirectSound(RANDOM_SOUND_3);
                        break;
                    case 4:
                        m_creature->PlayDirectSound(RANDOM_SOUND_4);
                        break;
                    }
                    m_randomSoundTimer = urand(10000, 37000);
                }
                else
                    m_randomSoundTimer -= uiDiff;
            }
        }
    }

    void ResetOgreState(bool stopRandomStuff)
    {
        if (stopRandomStuff)
            m_randomStuffTimer = 0;

        m_creature->HandleEmoteState(EMOTE_STATE_NONE); // Stop dancing
        m_drinkTimer = 0; // Stop drinking
        m_creature->LoadEquipment(m_creature->GetCreatureInfo()->EquipmentTemplateId, true); // Set default equipment
        m_creature->RemoveAurasDueToSpell(SPELL_BLOODMAUL_INTOXICATION_VISUAL); // Remove intoxication aura
    }
};

UnitAI* GetAI_npc_supplicant(Creature* pCreature)
{
    return new npc_supplicantAI(pCreature);
}

/*######
## npc_spirit_prisoner_of_bladespire
######*/

enum
{
    POINT_PLAYER_POSITION = 1,
};

struct npc_spirit_prisoner_of_bladespire : public ScriptedAI
{
    npc_spirit_prisoner_of_bladespire(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override {}

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            if (Unit* spawner = m_creature->GetSpawner())
                m_creature->GetMotionMaster()->MovePoint(POINT_PLAYER_POSITION, spawner->GetPositionX(), spawner->GetPositionY(), spawner->GetPositionZ());
        }
    }

    void MovementInform(uint32 uiMovementType, uint32 uiData) override
    {
        if (uiMovementType == POINT_MOTION_TYPE && uiData == POINT_PLAYER_POSITION)
            m_creature->ForcedDespawn();
    }
};

UnitAI* GetAI_npc_spirit_prisoner_of_bladespire(Creature* pCreature)
{
    return new npc_spirit_prisoner_of_bladespire(pCreature);
}

/*######
## npc_deadsoul_orb
######*/

enum
{
    WAYPOINT_TRIGGER_1 = 20851,
    WAYPOINT_TRIGGER_2 = 20852,
    WAYPOINT_TRIGGER_3 = 20853,
    WAYPOINT_TRIGGER_4 = 20855,
    WAYPOINT_TRIGGER_5 = 20856,

    ORB_TRIGGER_01 = 20666,
};

struct npc_deadsoul_orb : public ScriptedAI
{
    npc_deadsoul_orb(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_creature->SetActiveObjectState(true); // Need to be active since the area is so large they might get unloaded on the way to the destination
        nextTrigger = WAYPOINT_TRIGGER_1;
        pointCount = 1;
        MoveToNextTrigger();
    }

    uint32 nextTrigger;
    uint8 pointCount;

    void Reset() override { }

    void MovementInform(uint32 uiMovementType, uint32 uiData) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        switch (uiData)
        {
            case 1:
            {
                nextTrigger = WAYPOINT_TRIGGER_2;
                pointCount++;
                MoveToNextTrigger();
                break;
            }
            case 2:
            {
                nextTrigger = WAYPOINT_TRIGGER_3;
                pointCount++;
                MoveToNextTrigger();
                break;
            }
            case 3:
            {
                nextTrigger = WAYPOINT_TRIGGER_4;
                pointCount++;
                MoveToNextTrigger();
                break;
            }
            case 4:
            {
                nextTrigger = WAYPOINT_TRIGGER_5;
                pointCount++;
                MoveToNextTrigger();
                break;
            }
            case 5:
            {
                pointCount++;
                m_creature->SetWalk(false);
                MoveToNextTrigger();
                break;
            }
            case 6:
            {
                if (Creature* orbTrigger = GetClosestCreatureWithEntry(m_creature, ORB_TRIGGER_01, 15))
                {
                    m_creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_A, m_creature, orbTrigger);
                }
                m_creature->ForcedDespawn();
            }
        }
    }

    void MoveToNextTrigger()
    {
        if (pointCount == 6)
            m_creature->GetMotionMaster()->MovePoint(pointCount, 2809.716f, 5250.526f, 274.4666f);
        else
        {
            if (Creature* waypointTrigger = GetClosestCreatureWithEntry(m_creature, nextTrigger, 95))
            {
                m_creature->GetMotionMaster()->MovePoint(pointCount, waypointTrigger->GetPositionX(), waypointTrigger->GetPositionY(), waypointTrigger->GetPositionZ());
            }
            else
            {
                m_creature->ForcedDespawn(3000);
            }
        }
    }
};

UnitAI* GetAI_npc_deadsoul_orb(Creature* pCreature)
{
    return new npc_deadsoul_orb(pCreature);
}

/*######
## npc_evergrove_druid
######*/

enum
{
    SPELL_DRUID_SIGNAL = 38782,
    SPELL_EVERGROVE_DRUID_TRANSFORM_CROW = 38776,
    SPELL_EVERGROVE_DRUID_TRANSFORM_DRUID = 39158,

    DBSCRIPT_LANDING_SCRIPT = 10074,
    DBSCRIPT_FLY_OFF_SCRIPT = 10075
};

struct npc_evergrove_druidAI : public ScriptedAI
{
    npc_evergrove_druidAI(Creature* pCreature) : ScriptedAI(pCreature), returnTimer(0), landingDone(false), alreadySummoned(false)
    {
    }

    ObjectGuid m_summonerGuid;
    uint32 returnTimer;
    bool landingDone;
    bool alreadySummoned;

    void Reset() override {}

    void JustRespawned() override
    {
        landingDone = false;
        alreadySummoned = false;
        returnTimer = 0;
    }

    void SpellHit(Unit* caster, const SpellEntry* spell) override
    {
        if (alreadySummoned)
            return;

        if (caster->GetTypeId() != TYPEID_PLAYER)
            return;

        if (spell->Id == SPELL_DRUID_SIGNAL)
        {
            alreadySummoned = true;
            m_summonerGuid = caster->GetObjectGuid();
            m_creature->CastSpell(m_creature, SPELL_EVERGROVE_DRUID_TRANSFORM_CROW, TRIGGERED_NONE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->GetMotionMaster()->MoveFollow(caster, 1.f, 0.f);
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (landingDone)
            return;

        if (who->GetTypeId() != TYPEID_PLAYER)
            return;

        if (who->GetObjectGuid() != m_summonerGuid)
            return;

        if (m_creature->IsWithinDistInMap(who, 5.0f))
        {
            landingDone = true;
            returnTimer = 120000;

            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetLevitate(true);
            m_creature->SetHover(false);
            m_creature->SetCanFly(false);
            m_creature->GetMap()->ScriptsStart(sRelayScripts, DBSCRIPT_LANDING_SCRIPT, m_creature, who);
        }
    }

    void ReturnToSpawn(Player* questAccepter = nullptr)
    {
        if (!m_summonerGuid)
            return;

        if (questAccepter) // Only return to spawn if it's the original player accepting a quest
            if (questAccepter->GetObjectGuid() != m_summonerGuid)
                return;

        returnTimer = 0;
        Unit* summoned = m_creature->GetMap()->GetUnit(m_summonerGuid);
        m_creature->GetMap()->ScriptsStart(sRelayScripts, DBSCRIPT_FLY_OFF_SCRIPT, m_creature, summoned);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (sender != m_creature) // Sender should always be creature itself
            return;

        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_creature->SetLevitate(false);
            m_creature->SetHover(true);
            m_creature->SetCanFly(true);
        }
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            m_creature->GetMotionMaster()->MoveTargetedHome();
        }
    }

    void JustReachedHome() override
    {
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (returnTimer)
        {
            if (returnTimer <= uiDiff)
            {
                ReturnToSpawn();
            }
            else
                returnTimer -= uiDiff;
        }
    }
};

bool QuestAccept_npc_evergrove_druid(Player* player, Creature* creature, const Quest* /*quest*/)
{
    // As soon as the player has accepted a quest, return to spawn again
    if (npc_evergrove_druidAI* druidAI = dynamic_cast<npc_evergrove_druidAI*>(creature->AI()))
        druidAI->ReturnToSpawn(player);

    return true;
}

UnitAI* GetAI_npc_evergrove_druidAI(Creature* creature)
{
    return new npc_evergrove_druidAI(creature);
}

/*######
## npc_apexis_flayer
######*/

enum
{
    NPC_NETHERSTORM_TRIGGER = 19656,

    FLAYER_SPELL_SPECIAL_UNARMED = 33334,
    FLAYER_SPELL_REND = 13443,
    FLAYER_SPELL_SHRED_ARMOR = 40770,
};

enum FlayerActions
{
    FLAYER_ACTION_REND,
    FLAYER_ACTION_SHRED_ARMOR,
    FLAYER_ACTION_MAX,
};

struct npc_apexis_flayerAI : public ScriptedAI
{
    npc_apexis_flayerAI(Creature* creature) : ScriptedAI(creature, FLAYER_ACTION_MAX)
    {
        AddCombatAction(FLAYER_ACTION_REND, 0u);
        AddCombatAction(FLAYER_ACTION_SHRED_ARMOR, 0u);
        Reset();
    }

    uint8 strikeCount;
    uint32 idleTimer;
    uint32 crystalTimer;
    uint32 rendTimer;
    uint32 shredArmorTimer;

    void Reset() override
    {
        for (uint32 i = 0; i < FLAYER_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(FLAYER_ACTION_REND, GetInitialActionTimer(FLAYER_ACTION_REND));
        ResetTimer(FLAYER_ACTION_SHRED_ARMOR, GetInitialActionTimer(FLAYER_ACTION_SHRED_ARMOR));

        idleTimer = urand(15000, 20000);
        crystalTimer = 0;
        strikeCount = 0;
    }

    uint32 GetInitialActionTimer(uint32 id) // Timers are copied from the EventAI script
    {
        switch (id)
        {
            case FLAYER_ACTION_REND: return urand(8300, 13300);
            case FLAYER_ACTION_SHRED_ARMOR: return urand(4200, 6200);
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(uint32 id) // Timers are copied from the EventAI script
    {
        switch (id)
        {
            case FLAYER_ACTION_REND: return urand(22000, 26000);
            case FLAYER_ACTION_SHRED_ARMOR: return urand(28100, 31600);
            default: return 0; // never occurs but for compiler
        }
    }

    void ExecuteActions() override
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < FLAYER_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case FLAYER_ACTION_REND:
                    {
                        if (Unit* target = m_creature->GetVictim())
                        {
                            if (DoCastSpellIfCan(target, FLAYER_SPELL_REND) == CAST_OK)
                            {
                                ResetTimer(i, GetSubsequentActionTimer(i));
                                SetActionReadyStatus(i, false);
                                return;
                            }
                        }
                        continue;
                    }
                    case FLAYER_ACTION_SHRED_ARMOR:
                    {
                        if (Unit* target = m_creature->GetVictim())
                        {
                            if (DoCastSpellIfCan(target, FLAYER_SPELL_SHRED_ARMOR) == CAST_OK)
                            {
                                ResetTimer(i, GetSubsequentActionTimer(i));
                                SetActionReadyStatus(i, false);
                                return;
                            }
                        }
                        continue;
                    }
                }
            }
        }
    }


    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
        {
            if (idleTimer)
            {
                if (idleTimer <= diff)
                {
                    MoveToCrystal();
                    idleTimer = 0;
                }
                else
                    idleTimer -= diff;
            }

            if (crystalTimer)
            {
                if (crystalTimer <= diff)
                {
                    CrystalStrike();
                }
                else
                    crystalTimer -= diff;
            }
        }
        else
        {
            UpdateTimers(diff, m_creature->IsInCombat());
            ExecuteActions();

            DoMeleeAttackIfReady();
        }
    }

    void MoveToCrystal()
    {
        if (Creature* triggerNPC = GetClosestCreatureWithEntry(m_creature, NPC_NETHERSTORM_TRIGGER, 15))
        {
            float fX, fY, fZ;
            m_creature->GetContactPoint(triggerNPC, fX, fY, fZ);
            m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
        }
    }

    void CrystalStrike()
    {
        switch (strikeCount)
        {
            case 0:
                m_creature->CastSpell(m_creature, FLAYER_SPELL_SPECIAL_UNARMED, TRIGGERED_NONE);
                crystalTimer = 2400;
                strikeCount++;
                break;
            case 1:
                m_creature->CastSpell(m_creature, FLAYER_SPELL_SPECIAL_UNARMED, TRIGGERED_NONE);
                crystalTimer = 2400;
                strikeCount++;
                break;
            case 2:
                m_creature->CastSpell(m_creature, FLAYER_SPELL_SPECIAL_UNARMED, TRIGGERED_NONE);
                crystalTimer = 2400;
                strikeCount++;
                break;
            case 3:
                m_creature->CastSpell(m_creature, FLAYER_SPELL_SPECIAL_UNARMED, TRIGGERED_NONE);
                crystalTimer = 2400;
                strikeCount++;
                break;
            case 4:
                float respX, respY, respZ, respO, wander_distance;
                m_creature->GetRespawnCoord(respX, respY, respZ, &respO, &wander_distance);
                m_creature->GetMotionMaster()->MoveRandomAroundPoint(respX, respY, respZ, wander_distance);
                crystalTimer = 0;
                strikeCount = 0;
                idleTimer = urand(15000, 20000);
                break;
            default:
                break;
        }
    }

    void MovementInform(uint32 /*movementType*/, uint32 data) override
    {
        switch (data)
        {
            case 1:
            {
                m_creature->GetMotionMaster()->MoveIdle();
                crystalTimer = 1;
                break;
            }
        }
    }
};

UnitAI* GetAI_npc_apexis_flayerAI(Creature* creature)
{
    return new npc_apexis_flayerAI(creature);
}

enum
{
    NPC_LEAFBEARD = 21326,
};

struct ExorcismFeather : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        if (ObjectGuid target = spell->m_targets.getUnitTargetGuid()) // can be cast only on this target
            if (target.GetEntry() != NPC_LEAFBEARD)
                return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }
};

struct KoiKoiDeath : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsCreature())
            return;

        Creature* leafbeard = static_cast<Creature*>(target);
        leafbeard->SetFactionTemporary(FACTION_FRIENDLY, TEMPFACTION_RESTORE_RESPAWN);
        leafbeard->RemoveGuardians();
        leafbeard->CombatStopWithPets(true);
        leafbeard->GetMotionMaster()->MoveIdle();
        leafbeard->ForcedDespawn(12000);
    }
};

enum
{
    NPC_NETHER_DRAKE_EGG_BUNNY = 21814,
};

struct go_nether_drake_egg_trapAI : public GameObjectAI
{
    go_nether_drake_egg_trapAI(GameObject* go) : GameObjectAI(go) { }

    void JustDespawned() override
    {
        if (Creature* bunny = GetClosestCreatureWithEntry(m_go, NPC_NETHER_DRAKE_EGG_BUNNY, 5.f))
            bunny->ForcedDespawn();
    }
};

enum
{
    SPELL_ZEPHYRIUM_CHARGED = 37108,
};

struct Soaring : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            aura->GetTarget()->CastSpell(nullptr, SPELL_ZEPHYRIUM_CHARGED, TRIGGERED_NONE);
    }
};

struct CoaxMarmot : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
        {
            Unit* target = aura->GetTarget();
            if (target->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* pPlayer = static_cast<Player*>(target);
            if (pPlayer->GetMover() != target)
            {
                if (Creature* mover = static_cast<Creature*>(pPlayer->GetMover())) // this spell uses DoSummonPossesed so remove this on removal
                {
                    pPlayer->BreakCharmOutgoing();
                    mover->ForcedDespawn();
                }
            }
        }
    }
};

void AddSC_blades_edge_mountains()
{
    Script* pNewScript = new Script;
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
    pNewScript->Name = "npc_obelisk_trigger";
    pNewScript->GetAI = &GetAI_obelisk_triggerAI;
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

    pNewScript = new Script;
    pNewScript->Name = "npc_bird_spirit";
    pNewScript->GetAI = &GetAI_npc_bird_spirit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_bloodmaul_dire_wolf";
    pNewScript->GetAI = &GetAI_npc_bloodmaul_dire_wolf;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_spell_diminution_powder;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mobs_grishna_arrakoa";
    pNewScript->pAreaTrigger = &AreaTrigger_at_raven_prophecy;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_frequency_scanner";
    pNewScript->GetAI = &GetAI_npc_frequency_scanner;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_aura_generator_000";
    pNewScript->GetGameObjectAI = &GetGOAI_go_aura_generator_000;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fel_cannon";
    pNewScript->GetAI = &GetAI_npc_fel_cannon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_warp_gate";
    pNewScript->GetAI = &GetAI_npc_warp_gate;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_into_the_soulgrinder";
    pNewScript->pProcessEventId = &ProcessEventId_Soulgrinder;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_soulgrinder";
    pNewScript->GetAI = &GetAI_npc_soulgrinder;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mogdorg_the_wizened";
    pNewScript->pQuestRewardedNPC = &QuestRewarded_into_the_soulgrinder;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_supplicant";
    pNewScript->GetAI = &GetAI_npc_supplicant;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spirit_prisoner_of_bladespire";
    pNewScript->GetAI = &GetAI_npc_spirit_prisoner_of_bladespire;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_deadsoul_orb";
    pNewScript->GetAI = &GetAI_npc_deadsoul_orb;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_evergrove_druid";
    pNewScript->GetAI = &GetAI_npc_evergrove_druidAI;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_evergrove_druid;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_apexis_flayer";
    pNewScript->GetAI = &GetAI_npc_apexis_flayerAI;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_nether_drake_egg_trap";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_nether_drake_egg_trapAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ExorcismFeather>("spell_exorcism_feather");
    RegisterSpellScript<KoiKoiDeath>("spell_koi_koi_death");
    RegisterSpellScript<Soaring>("spell_soaring");
    RegisterSpellScript<CoaxMarmot>("spell_coax_marmot");
}

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
SDName: chess_event
SD%Complete: 80
SDComment: Chess AI could use some improvements.
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "karazhan.h"

enum
{
    // texts
    EMOTE_LIFT_CURSE                = -1532131,
    EMOTE_CHEAT                     = -1532132,

    SOUND_ID_GAME_BEGIN             = 10338,
    SOUND_ID_LOSE_PAWN_PLAYER_1     = 10339,
    SOUND_ID_LOSE_PAWN_PLAYER_2     = 10340,
    SOUND_ID_LOSE_PAWN_PLAYER_3     = 10341,
    SOUND_ID_LOSE_PAWN_MEDIVH_1     = 10342,
    SOUND_ID_LOSE_PAWN_MEDIVH_2     = 10343,
    SOUND_ID_LOSE_PAWN_MEDIVH_3     = 10344,
    SOUND_ID_LOSE_ROOK_PLAYER       = 10345,
    SOUND_ID_LOSE_ROOK_MEDIVH       = 10346,
    SOUND_ID_LOSE_BISHOP_PLAYER     = 10347,
    SOUND_ID_LOSE_BISHOP_MEDIVH     = 10348,
    SOUND_ID_LOSE_KNIGHT_PLAYER     = 10349,
    SOUND_ID_LOSE_KNIGHT_MEDIVH     = 10350,
    SOUND_ID_LOSE_QUEEN_PLAYER      = 10351,
    SOUND_ID_LOSE_QUEEN_MEDIVH      = 10352,
    SOUND_ID_CHECK_PLAYER           = 10353,
    SOUND_ID_CHECK_MEDIVH           = 10354,
    SOUND_ID_WIN_PLAYER             = 10355,
    SOUND_ID_WIN_MEDIVH             = 10356,
    SOUND_ID_CHEAT_1                = 10357,
    SOUND_ID_CHEAT_2                = 10358,
    SOUND_ID_CHEAT_3                = 10359,

    // movement spells
    SPELL_MOVE_GENERIC              = 30012,                    // spell which sends the signal to move - handled in core
    SPELL_MOVE_1                    = 32312,                    // spell which selects AI move square (for short range pieces)
    SPELL_MOVE_2                    = 37388,                    // spell which selects AI move square (for long range pieces)
    // SPELL_MOVE_PAWN              = 37146,                    // individual move spells (used only by controlled npcs)
    // SPELL_MOVE_KNIGHT            = 37144,
    // SPELL_MOVE_QUEEN             = 37148,
    // SPELL_MOVE_ROCK              = 37151,
    // SPELL_MOVE_BISHOP            = 37152,
    // SPELL_MOVE_KING              = 37153,

    // additional movement spells
    SPELL_CHANGE_FACING             = 30284,                    // spell which sends the initial facing request - handled in core
    SPELL_FACE_SQUARE               = 30270,                    // change facing - finalize facing update

    SPELL_MOVE_TO_SQUARE            = 30253,                    // spell which sends the move response from the square to the piece
    SPELL_MOVE_COOLDOWN             = 30543,                    // add some cooldown to movement
    SPELL_MOVE_MARKER               = 32261,                    // white beam visual - used to mark the movement as complete
    SPELL_DISABLE_SQUARE            = 32745,                    // used by the White / Black triggers on the squares when a chess piece moves into place
    SPELL_IS_SQUARE_USED            = 39400,                    // cast when a chess piece moves to another square
    // SPELL_SQUARED_OCCUPIED       = 39399,                    // triggered by 39400; used to check if the square is occupied (if hits a target); Missing in 2.4.3

    // generic spells
    SPELL_IN_GAME                   = 30532,                    // teleport player near the entrance
    SPELL_CONTROL_PIECE             = 30019,                    // control a chess piece
    SPELL_RECENTLY_IN_GAME          = 30529,                    // debuff on player after chess piece uncharm

    SPELL_CHESS_AI_ATTACK_TIMER     = 32226,                    // melee action timer - triggers 32225
    SPELL_ACTION_MELEE              = 32225,                    // handle melee attacks
    SPELL_MELEE_DAMAGE              = 32247,                    // melee damage spell - used by all chess pieces
    // SPELL_AI_SNAPSHOT_TIMER      = 37440,                    // used to trigger spell 32260; purpose and usage unk
    // SPELL_DISABLE_SQUARE_SELF    = 32260,                    // used when a piece moves to another square
    // SPELL_AI_ACTION_TIMER        = 37504,                    // handle some kind of event check. Cast by npc 17459. Currently the way it works is unk
    // SPELL_DISABLE_SQUARE         = 30271,                    // not used
    // SPELL_FIND_ENEMY             = 32303,                    // not used
    // SPELL_MOVE_NEAR_UNIT         = 30417,                    // not used
    // SPELL_GET_EMPTY_SQUARE       = 30418,                    // not used
    // SPELL_FACE_NEARBY_ENEMY      = 37787,                    // not used
    // SPELL_POST_MOVE_FACING       = 38011,                    // not used

    // melee action spells
    SPELL_MELEE_FOOTMAN             = 32227,
    SPELL_MELEE_WATER_ELEM          = 37142,
    SPELL_MELEE_CHARGER             = 37143,
    SPELL_MELEE_CLERIC              = 37147,
    SPELL_MELEE_CONJURER            = 37149,
    SPELL_MELEE_KING_LLANE          = 37150,
    SPELL_MELEE_GRUNT               = 32228,
    SPELL_MELEE_DAEMON              = 37220,
    SPELL_MELEE_NECROLYTE           = 37337,
    SPELL_MELEE_WOLF                = 37339,
    SPELL_MELEE_WARLOCK             = 37345,
    SPELL_MELEE_WARCHIEF_BLACKHAND  = 37348,

    // cheat spells
    SPELL_HAND_OF_MEDIVH_HORDE      = 39338,                    // triggers 39339
    SPELL_HAND_OF_MEDIVH_ALLIANCE   = 39342,                    // triggers 39339
    SPELL_FURY_OF_MEDIVH_HORDE      = 39341,                    // triggers 39343
    SPELL_FURY_OF_MEDIVH_ALLIANCE   = 39344,                    // triggers 39345
    SPELL_FURY_OF_MEDIVH_AURA       = 39383,
    // SPELL_FULL_HEAL_HORDE        = 39334,                    // spells are not confirmed (probably removed after 2.4.3)
    // SPELL_FULL_HEAL_ALLIANCE     = 39335,

    // spells used by the chess npcs
    SPELL_HEROISM                   = 37471,                    // human king
    SPELL_SWEEP                     = 37474,
    SPELL_BLOODLUST                 = 37472,                    // orc king
    SPELL_CLEAVE                    = 37476,
    SPELL_HEROIC_BLOW               = 37406,                    // human pawn
    SPELL_SHIELD_BLOCK              = 37414,
    SPELL_VICIOUS_STRIKE            = 37413,                    // orc pawn
    SPELL_WEAPON_DEFLECTION         = 37416,
    SPELL_SMASH                     = 37453,                    // human knight
    SPELL_STOMP                     = 37498,
    SPELL_BITE                      = 37454,                    // orc knight
    SPELL_HOWL                      = 37502,
    SPELL_ELEMENTAL_BLAST           = 37462,                    // human queen
    SPELL_RAIN_OF_FIRE              = 37465,
    SPELL_FIREBALL                  = 37463,                    // orc queen
    // SPELL_POISON_CLOUD           = 37469,
    SPELL_POISON_CLOUD_ACTION       = 37775,                    // triggers 37469 - acts as a target selector spell for orc queen
    SPELL_HEALING                   = 37455,                    // human bishop
    SPELL_HOLY_LANCE                = 37459,
    // SPELL_SHADOW_MEND            = 37456,                    // orc bishop
    SPELL_SHADOW_MEND_ACTION        = 37824,                    // triggers 37456 - acts as a target selector spell for orc bishop
    SPELL_SHADOW_SPEAR              = 37461,
    SPELL_GEYSER                    = 37427,                    // human rook
    SPELL_WATER_SHIELD              = 37432,
    SPELL_HELLFIRE                  = 37428,                    // orc rook
    SPELL_FIRE_SHIELD               = 37434,

    // spells used to transform side trigger when npc dies
    SPELL_TRANSFORM_FOOTMAN         = 39350,
    SPELL_TRANSFORM_CHARGER         = 39352,
    SPELL_TRANSFORM_CLERIC          = 39353,
    SPELL_TRANSFORM_WATER_ELEM      = 39354,
    SPELL_TRANSFORM_CONJURER        = 39355,
    SPELL_TRANSFORM_KING_LLANE      = 39356,
    SPELL_TRANSFORM_GRUNT           = 39357,
    SPELL_TRANSFORM_WOLF            = 39358,
    SPELL_TRANSFORM_NECROLYTE       = 39359,
    SPELL_TRANSFORM_DAEMON          = 39360,
    SPELL_TRANSFORM_WARLOCK         = 39361,
    SPELL_TRANSFORM_BLACKHAND       = 39362,

    // generic npcs
    // NPC_SQUARE_OUTSIDE_B         = 17316,                    // used to check the interior of the board
    // NPC_SQUARE_OUTSIDE_W         = 17317,                    // not used in our script; keep for reference only
    NPC_FURY_MEDIVH_VISUAL          = 22521,                    // has aura 39383

    // gossip texts
    GOSSIP_ITEM_ORC_GRUNT           = -3532006,
    GOSSIP_ITEM_ORC_WOLF            = -3532007,
    GOSSIP_ITEM_SUMMONED_DEAMON     = -3532008,
    GOSSIP_ITEM_ORC_WARLOCK         = -3532009,
    GOSSIP_ITEM_ORC_NECROLYTE       = -3532010,
    GOSSIP_ITEM_WARCHIEF_BLACKHAND  = -3532011,
    GOSSIP_ITEM_HUMAN_FOOTMAN       = -3532012,
    GOSSIP_ITEM_HUMAN_CHARGER       = -3532013,
    GOSSIP_ITEM_WATER_ELEMENTAL     = -3532014,
    GOSSIP_ITEM_HUMAN_CONJURER      = -3532015,
    GOSSIP_ITEM_HUMAN_CLERIC        = -3532016,
    GOSSIP_ITEM_KING_LLANE          = -3532017,
    GOSSIP_ITEM_RESET_BOARD         = -3532018,

    // gossip menu
    GOSSIP_MENU_ID_GRUNT            = 10425,
    GOSSIP_MENU_ID_WOLF             = 10439,
    GOSSIP_MENU_ID_WARLOCK          = 10440,
    GOSSIP_MENU_ID_NECROLYTE        = 10434,
    GOSSIP_MENU_ID_DEAMON           = 10426,
    GOSSIP_MENU_ID_BLACKHAND        = 10442,
    GOSSIP_MENU_ID_FOOTMAN          = 8952,
    GOSSIP_MENU_ID_CHARGER          = 10414,
    GOSSIP_MENU_ID_CONJURER         = 10417,
    GOSSIP_MENU_ID_CLERIC           = 10416,
    GOSSIP_MENU_ID_ELEMENTAL        = 10413,
    GOSSIP_MENU_ID_LLANE            = 10418,
    GOSSIP_MENU_ID_MEDIVH           = 10506,
    GOSSIP_MENU_ID_MEDIVH_BEATEN    = 10718,

    // misc
    TARGET_TYPE_RANDOM              = 1,
    TARGET_TYPE_FRIENDLY            = 2,
};

/*######
## npc_echo_of_medivh
######*/

struct npc_echo_of_medivhAI : public ScriptedAI
{
    npc_echo_of_medivhAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_karazhan*)pCreature->GetInstanceData();
        Reset();
    }

    instance_karazhan* m_pInstance;

    uint32 m_uiCheatTimer;

    void Reset() override
    {
        m_uiCheatTimer = 90000;
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_FURY_MEDIVH_VISUAL)
            pSummoned->CastSpell(pSummoned, SPELL_FURY_OF_MEDIVH_AURA, true);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_CHESS) != IN_PROGRESS)
            return;

        if (m_uiCheatTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, urand(0, 1) ? (m_pInstance->GetPlayerTeam() == ALLIANCE ? SPELL_HAND_OF_MEDIVH_HORDE : SPELL_HAND_OF_MEDIVH_ALLIANCE) :
                                 (m_pInstance->GetPlayerTeam() == ALLIANCE ? SPELL_FURY_OF_MEDIVH_ALLIANCE : SPELL_FURY_OF_MEDIVH_HORDE));

            switch (urand(0, 2))
        {
                case 0: DoPlaySoundToSet(m_creature, SOUND_ID_CHEAT_1); break;
                case 1: DoPlaySoundToSet(m_creature, SOUND_ID_CHEAT_2); break;
                case 2: DoPlaySoundToSet(m_creature, SOUND_ID_CHEAT_3); break;
            }

            DoScriptText(EMOTE_CHEAT, m_creature);
            m_uiCheatTimer = 90000;
        }
        else
            m_uiCheatTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_echo_of_medivh(Creature* pCreature)
{
    return new npc_echo_of_medivhAI(pCreature);
}

bool GossipHello_npc_echo_of_medivh(Player* pPlayer, Creature* pCreature)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_CHESS) != DONE && pInstance->GetData(TYPE_CHESS) != SPECIAL)
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_MEDIVH, pCreature->GetObjectGuid());
        else
        {
            if (pInstance->GetData(TYPE_CHESS) == SPECIAL)
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RESET_BOARD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_MEDIVH_BEATEN, pCreature->GetObjectGuid());
        }

        return true;
    }

    return false;
}

bool GossipSelect_npc_echo_of_medivh(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        // reset the board
        if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
            pInstance->SetData(TYPE_CHESS, DONE);

        pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}

/*######
## npc_chess_piece_generic
######*/

struct npc_chess_piece_genericAI : public ScriptedAI
{
    npc_chess_piece_genericAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_karazhan*)pCreature->GetInstanceData();
        Reset();
    }

    instance_karazhan* m_pInstance;

    ObjectGuid m_currentSquareGuid;

    uint32 m_uiMoveTimer;
    uint32 m_uiMoveCommandTimer;
    uint32 m_uiSpellCommandTimer;

    bool m_bIsPrimarySpell;
    float m_fCurrentOrientation;

    void Reset() override
    {
        m_uiMoveTimer = 0;
        m_uiMoveCommandTimer = 1000;
        m_uiSpellCommandTimer = m_creature->HasAura(SPELL_CONTROL_PIECE) ? 0 : 1000;
        m_bIsPrimarySpell = true;

        // cancel move timer for player faction npcs or for friendly games
        if (m_pInstance)
        {
            if ((m_pInstance->GetPlayerTeam() == ALLIANCE && m_creature->getFaction() == FACTION_ID_CHESS_ALLIANCE) ||
                    (m_pInstance->GetPlayerTeam() == HORDE && m_creature->getFaction() == FACTION_ID_CHESS_HORDE) ||
                    m_pInstance->GetData(TYPE_CHESS) == DONE)
                m_uiMoveCommandTimer = 0;
        }
    }

    // no default attacking or evading
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void EnterEvadeMode() override { }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (Creature* pSquare = m_creature->GetMap()->GetCreature(m_currentSquareGuid))
            pSquare->RemoveAllAuras();

        // ToDo: remove corpse after 10 sec
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // handle move event
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            // clear the current square
            if (Creature* pSquare = m_creature->GetMap()->GetCreature(m_currentSquareGuid))
                pSquare->RemoveAllAuras();

            m_currentSquareGuid = pInvoker->GetObjectGuid();
            m_uiMoveTimer = 2000;
        }
        // handle encounter start event
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            // reset the variables
            Reset();
            m_currentSquareGuid = pInvoker->GetObjectGuid();

            // ToDo: enable this when the scope of the spell is clear
            //if (Creature* pStalker = m_pInstance->GetSingleCreatureFromStorage(NPC_WAITING_ROOM_STALKER))
            //    pStalker->CastSpell(pStalker, SPELL_AI_ACTION_TIMER, true);

            //DoCastSpellIfCan(m_creature, SPELL_AI_SNAPSHOT_TIMER, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_CHESS_AI_ATTACK_TIMER, CAST_TRIGGERED);

            pInvoker->CastSpell(pInvoker, SPELL_DISABLE_SQUARE, true);
            pInvoker->CastSpell(pInvoker, SPELL_IS_SQUARE_USED, true);
        }
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // update facing
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 5.0f))
            DoCastSpellIfCan(pTarget, SPELL_CHANGE_FACING);
        else
            m_creature->SetFacingTo(m_fCurrentOrientation);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        // do a soft reset when the piece is controlled
        if (pCaster->GetTypeId() == TYPEID_PLAYER && pSpell->Id == SPELL_CONTROL_PIECE)
            Reset();
    }

    // Function which returns a random target by type and range
    Unit* GetTargetByType(uint8 uiType, float fRange, float fArc = M_PI_F)
    {
        if (!m_pInstance)
            return NULL;

        uint32 uiTeam = m_creature->getFaction() == FACTION_ID_CHESS_ALLIANCE ? FACTION_ID_CHESS_HORDE : FACTION_ID_CHESS_ALLIANCE;

        // get friendly list for this type
        if (uiType == TARGET_TYPE_FRIENDLY)
            uiTeam = m_creature->getFaction();

        // Get the list of enemies
        GuidList lTempList;
        std::vector<Creature*> vTargets;
        vTargets.reserve(lTempList.size());

        m_pInstance->GetChessPiecesByFaction(lTempList, uiTeam);
        for (GuidList::const_iterator itr = lTempList.begin(); itr != lTempList.end(); ++itr)
        {
            Creature* pTemp = m_creature->GetMap()->GetCreature(*itr);
            if (pTemp && pTemp->isAlive())
            {
                // check for specified range targets and angle; Note: to be checked if the angle is right
                if (fRange && !m_creature->isInFrontInMap(pTemp, fRange, fArc))
                    continue;

                // skip friendly targets which are at full HP
                if (uiType == TARGET_TYPE_FRIENDLY && pTemp->GetHealth() == pTemp->GetMaxHealth())
                    continue;

                vTargets.push_back(pTemp);
            }
        }

        if (vTargets.empty())
            return NULL;

        return vTargets[urand(0, vTargets.size() - 1)];
    }

    // Function to get a square as close as possible to the enemy
    Unit* GetMovementSquare()
    {
        if (!m_pInstance)
            return NULL;

        // define distance based on the spell radius
        // this will replace the targeting sysmte of spells SPELL_MOVE_1 and SPELL_MOVE_2
        float fRadius = 10.0f;
        std::list<Creature*> lSquaresList;

        // some pieces have special distance
        switch (m_creature->GetEntry())
        {
            case NPC_HUMAN_CONJURER:
            case NPC_ORC_WARLOCK:
            case NPC_HUMAN_CHARGER:
            case NPC_ORC_WOLF:
                fRadius = 15.0f;
                break;
        }

        // get all available squares for movement
        GetCreatureListWithEntryInGrid(lSquaresList, m_creature, NPC_SQUARE_BLACK, fRadius);
        GetCreatureListWithEntryInGrid(lSquaresList, m_creature, NPC_SQUARE_WHITE, fRadius);

        if (lSquaresList.empty())
            return NULL;

        // Get the list of enemies
        GuidList lTempList;
        std::list<Creature*> lEnemies;

        m_pInstance->GetChessPiecesByFaction(lTempList, m_creature->getFaction() == FACTION_ID_CHESS_ALLIANCE ? FACTION_ID_CHESS_HORDE : FACTION_ID_CHESS_ALLIANCE);
        for (GuidList::const_iterator itr = lTempList.begin(); itr != lTempList.end(); ++itr)
        {
            Creature* pTemp = m_creature->GetMap()->GetCreature(*itr);
            if (pTemp && pTemp->isAlive())
                lEnemies.push_back(pTemp);
        }

        if (lEnemies.empty())
            return NULL;

        // Sort the enemies by distance and the squares compared to the distance to the closest enemy
        lEnemies.sort(ObjectDistanceOrder(m_creature));
        lSquaresList.sort(ObjectDistanceOrder(lEnemies.front()));

        return lSquaresList.front();
    }

    virtual uint32 DoCastPrimarySpell() { return 5000; }
    virtual uint32 DoCastSecondarySpell() { return 5000; }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_CHESS) != IN_PROGRESS)
            return;

        // issue move command
        if (m_uiMoveCommandTimer)
        {
            if (m_uiMoveCommandTimer <= uiDiff)
            {
                // just update facing if some enemy is near
                if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 5.0f))
                    DoCastSpellIfCan(pTarget, SPELL_CHANGE_FACING);
                else
                {
                    // the npc doesn't have a 100% chance to move; also there should be some GCD check in core for this part
                    if (roll_chance_i(15))
                    {
                        // Note: in a normal case the target would be chosen using the spells above
                        // However, because the core doesn't support special targeting, we'll provide explicit target
                        //uint32 uiMoveSpell = SPELL_MOVE_1;
                        //switch (m_creature->GetEntry())
                        //{
                        //    case NPC_HUMAN_CONJURER:
                        //    case NPC_ORC_WARLOCK:
                        //    case NPC_HUMAN_CHARGER:
                        //    case NPC_ORC_WOLF:
                        //        uiMoveSpell = SPELL_MOVE_2;
                        //        break;
                        //}
                        //DoCastSpellIfCan(m_creature, uiMoveSpell, CAST_TRIGGERED);

                        // workaround which provides specific move target
                        if (Unit* pTarget = GetMovementSquare())
                            DoCastSpellIfCan(pTarget, SPELL_MOVE_GENERIC, CAST_TRIGGERED | CAST_INTERRUPT_PREVIOUS);

                        m_fCurrentOrientation = m_creature->GetOrientation();
                    }
                }

                m_uiMoveCommandTimer = 5000;
            }
            else
                m_uiMoveCommandTimer -= uiDiff;
        }

        // issue spell command
        if (m_uiSpellCommandTimer)
        {
            if (m_uiSpellCommandTimer <= uiDiff)
            {
                // alternate the spells and also reset the timer
                m_uiSpellCommandTimer = m_bIsPrimarySpell ? DoCastPrimarySpell() : DoCastSecondarySpell();
                m_bIsPrimarySpell = !m_bIsPrimarySpell;
            }
            else
                m_uiSpellCommandTimer -= uiDiff;
        }

        // finish move timer
        if (m_uiMoveTimer)
        {
            if (m_uiMoveTimer <= uiDiff)
            {
                if (Creature* pSquare = m_creature->GetMap()->GetCreature(m_currentSquareGuid))
                {
                    DoCastSpellIfCan(pSquare, SPELL_MOVE_MARKER, CAST_TRIGGERED);
                    m_creature->GetMotionMaster()->MovePoint(1, pSquare->GetPositionX(), pSquare->GetPositionY(), pSquare->GetPositionZ());
                }
                m_uiMoveTimer = 0;
            }
            else
                m_uiMoveTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }
};

bool GossipSelect_npc_chess_generic(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        // start event when used on the king
        if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
        {
            // teleport at the entrance and control the chess piece
            pPlayer->CastSpell(pPlayer, SPELL_IN_GAME, true);
            pPlayer->CastSpell(pCreature, SPELL_CONTROL_PIECE, true);

            if (pInstance->GetData(TYPE_CHESS) == NOT_STARTED)
                pInstance->SetData(TYPE_CHESS, IN_PROGRESS);
            else if (pInstance->GetData(TYPE_CHESS) == DONE)
                pInstance->SetData(TYPE_CHESS, SPECIAL);
        }

        pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}

bool EffectDummyCreature_npc_chess_generic(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // movement perform spell
    if (uiSpellId == SPELL_MOVE_TO_SQUARE && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCaster->GetTypeId() == TYPEID_UNIT)
        {
            pCaster->CastSpell(pCaster, SPELL_DISABLE_SQUARE, true);
            pCaster->CastSpell(pCaster, SPELL_IS_SQUARE_USED, true);

            pCreatureTarget->CastSpell(pCreatureTarget, SPELL_MOVE_COOLDOWN, true);
            pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);
        }

        return true;
    }
    // generic melee tick
    else if (uiSpellId == SPELL_ACTION_MELEE && uiEffIndex == EFFECT_INDEX_0)
    {
        uint32 uiMeleeSpell = 0;

        switch (pCreatureTarget->GetEntry())
        {
            case NPC_KING_LLANE:            uiMeleeSpell = SPELL_MELEE_KING_LLANE;          break;
            case NPC_HUMAN_CHARGER:         uiMeleeSpell = SPELL_MELEE_CHARGER;             break;
            case NPC_HUMAN_CLERIC:          uiMeleeSpell = SPELL_MELEE_CLERIC;              break;
            case NPC_HUMAN_CONJURER:        uiMeleeSpell = SPELL_MELEE_CONJURER;            break;
            case NPC_HUMAN_FOOTMAN:         uiMeleeSpell = SPELL_MELEE_FOOTMAN;             break;
            case NPC_CONJURED_WATER_ELEMENTAL: uiMeleeSpell = SPELL_MELEE_WATER_ELEM;       break;
            case NPC_WARCHIEF_BLACKHAND:    uiMeleeSpell = SPELL_MELEE_WARCHIEF_BLACKHAND;  break;
            case NPC_ORC_GRUNT:             uiMeleeSpell = SPELL_MELEE_GRUNT;               break;
            case NPC_ORC_NECROLYTE:         uiMeleeSpell = SPELL_MELEE_NECROLYTE;           break;
            case NPC_ORC_WARLOCK:           uiMeleeSpell = SPELL_MELEE_WARLOCK;             break;
            case NPC_ORC_WOLF:              uiMeleeSpell = SPELL_MELEE_WOLF;                break;
            case NPC_SUMMONED_DAEMON:       uiMeleeSpell = SPELL_MELEE_DAEMON;              break;
        }

        pCreatureTarget->CastSpell(pCreatureTarget, uiMeleeSpell, true);
        return true;
    }
    // square facing
    else if (uiSpellId == SPELL_FACE_SQUARE && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCaster->GetTypeId() == TYPEID_UNIT)
            pCreatureTarget->SetFacingToObject(pCaster);

        return true;
    }

    return false;
}

/*######
## npc_king_llane
######*/

struct npc_king_llaneAI : public npc_chess_piece_genericAI
{
    npc_king_llaneAI(Creature* pCreature) : npc_chess_piece_genericAI(pCreature)
    {
        m_bIsAttacked = false;
        Reset();
    }

    bool m_bIsAttacked;

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) override
    {
        if (!uiDamage || !m_bIsAttacked || !m_pInstance || pDoneBy->GetTypeId() != TYPEID_UNIT)
            return;

        if (Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_ECHO_MEDIVH))
        {
            if (m_pInstance->GetPlayerTeam() == ALLIANCE)
                DoPlaySoundToSet(pMedivh, SOUND_ID_CHECK_PLAYER);
            else
                DoPlaySoundToSet(pMedivh, SOUND_ID_CHECK_MEDIVH);
        }

        m_bIsAttacked = true;
    }

    void JustDied(Unit* pKiller) override
    {
        npc_chess_piece_genericAI::JustDied(pKiller);

        if (!m_pInstance)
            return;

        Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_ECHO_MEDIVH);
        if (!pMedivh)
            return;

        if (m_pInstance->GetData(TYPE_CHESS) == SPECIAL)
            m_pInstance->SetData(TYPE_CHESS, DONE);
        else
        {
            if (m_pInstance->GetPlayerTeam() == HORDE)
            {
                DoPlaySoundToSet(pMedivh, SOUND_ID_WIN_PLAYER);
                DoScriptText(EMOTE_LIFT_CURSE, pMedivh);

                m_pInstance->SetData(TYPE_CHESS, DONE);
            }
            else
            {
                DoPlaySoundToSet(pMedivh, SOUND_ID_WIN_MEDIVH);
                m_pInstance->SetData(TYPE_CHESS, FAIL);
            }
        }

        m_pInstance->DoMoveChessPieceToSides(SPELL_TRANSFORM_KING_LLANE, FACTION_ID_CHESS_ALLIANCE, true);
    }

    uint32 DoCastPrimarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 20.0f))
        {
            DoCastSpellIfCan(m_creature, SPELL_HEROISM);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_HEROISM);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }

    uint32 DoCastSecondarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 10.0f))
        {
            DoCastSpellIfCan(m_creature, SPELL_SWEEP);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_SWEEP);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }
};

CreatureAI* GetAI_npc_king_llane(Creature* pCreature)
{
    return new npc_king_llaneAI(pCreature);
}

bool GossipHello_npc_king_llane(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(SPELL_RECENTLY_IN_GAME) || pCreature->HasAura(SPELL_CONTROL_PIECE))
        return true;

    if (instance_karazhan* pInstance = (instance_karazhan*)pCreature->GetInstanceData())
    {
        if ((pInstance->GetData(TYPE_CHESS) != DONE && pPlayer->GetTeam() == ALLIANCE) || pInstance->IsFriendlyGameReady())
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KING_LLANE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_LLANE, pCreature->GetObjectGuid());
    return true;
}

/*######
## npc_warchief_blackhand
######*/

struct npc_warchief_blackhandAI : public npc_chess_piece_genericAI
{
    npc_warchief_blackhandAI(Creature* pCreature) : npc_chess_piece_genericAI(pCreature)
    {
        m_bIsAttacked = false;
        Reset();
    }

    bool m_bIsAttacked;

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) override
    {
        if (!uiDamage || !m_bIsAttacked || !m_pInstance || pDoneBy->GetTypeId() != TYPEID_UNIT)
            return;

        if (Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_ECHO_MEDIVH))
        {
            if (m_pInstance->GetPlayerTeam() == HORDE)
                DoPlaySoundToSet(pMedivh, SOUND_ID_CHECK_PLAYER);
            else
                DoPlaySoundToSet(pMedivh, SOUND_ID_CHECK_MEDIVH);
        }

        m_bIsAttacked = true;
    }

    void JustDied(Unit* pKiller) override
    {
        npc_chess_piece_genericAI::JustDied(pKiller);

        if (!m_pInstance)
            return;

        Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_ECHO_MEDIVH);
        if (!pMedivh)
            return;

        if (m_pInstance->GetData(TYPE_CHESS) == SPECIAL)
            m_pInstance->SetData(TYPE_CHESS, DONE);
        else
        {
            if (m_pInstance->GetPlayerTeam() == ALLIANCE)
            {
                DoPlaySoundToSet(pMedivh, SOUND_ID_WIN_PLAYER);
                DoScriptText(EMOTE_LIFT_CURSE, pMedivh);

                m_pInstance->SetData(TYPE_CHESS, DONE);
            }
            else
            {
                DoPlaySoundToSet(pMedivh, SOUND_ID_WIN_MEDIVH);
                m_pInstance->SetData(TYPE_CHESS, FAIL);
            }
        }

        m_pInstance->DoMoveChessPieceToSides(SPELL_TRANSFORM_BLACKHAND, FACTION_ID_CHESS_HORDE, true);
    }

    uint32 DoCastPrimarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 20.0f))
        {
            DoCastSpellIfCan(m_creature, SPELL_BLOODLUST);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_BLOODLUST);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }

    uint32 DoCastSecondarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 10.0f))
        {
            DoCastSpellIfCan(m_creature, SPELL_CLEAVE);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_CLEAVE);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }
};

CreatureAI* GetAI_npc_warchief_blackhand(Creature* pCreature)
{
    return new npc_warchief_blackhandAI(pCreature);
}

bool GossipHello_npc_warchief_blackhand(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(SPELL_RECENTLY_IN_GAME) || pCreature->HasAura(SPELL_CONTROL_PIECE))
        return true;

    if (instance_karazhan* pInstance = (instance_karazhan*)pCreature->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_CHESS) != DONE && pPlayer->GetTeam() == HORDE || pInstance->IsFriendlyGameReady())
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_WARCHIEF_BLACKHAND, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_BLACKHAND, pCreature->GetObjectGuid());
    return true;
}

/*######
## npc_human_conjurer
######*/

struct npc_human_conjurerAI : public npc_chess_piece_genericAI
{
    npc_human_conjurerAI(Creature* pCreature) : npc_chess_piece_genericAI(pCreature) { Reset(); }

    void JustDied(Unit* pKiller) override
    {
        npc_chess_piece_genericAI::JustDied(pKiller);

        if (!m_pInstance)
            return;

        Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_ECHO_MEDIVH);
        if (!pMedivh)
            return;

        if (m_pInstance->GetPlayerTeam() == ALLIANCE)
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_QUEEN_PLAYER);
        else
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_QUEEN_MEDIVH);

        m_pInstance->DoMoveChessPieceToSides(SPELL_TRANSFORM_CONJURER, FACTION_ID_CHESS_ALLIANCE);
    }

    uint32 DoCastPrimarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 20.0f))
        {
            DoCastSpellIfCan(pTarget, SPELL_ELEMENTAL_BLAST);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_ELEMENTAL_BLAST);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }

    uint32 DoCastSecondarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 25.0f))
        {
            DoCastSpellIfCan(pTarget, SPELL_RAIN_OF_FIRE);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_RAIN_OF_FIRE);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }
};

CreatureAI* GetAI_npc_human_conjurer(Creature* pCreature)
{
    return new npc_human_conjurerAI(pCreature);
}

bool GossipHello_npc_human_conjurer(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(SPELL_RECENTLY_IN_GAME) || pCreature->HasAura(SPELL_CONTROL_PIECE))
        return true;

    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if ((pInstance->GetData(TYPE_CHESS) == IN_PROGRESS && pPlayer->GetTeam() == ALLIANCE) || pInstance->GetData(TYPE_CHESS) == SPECIAL)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_HUMAN_CONJURER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_CONJURER, pCreature->GetObjectGuid());
    return true;
}

/*######
## npc_orc_warlock
######*/

struct npc_orc_warlockAI : public npc_chess_piece_genericAI
{
    npc_orc_warlockAI(Creature* pCreature) : npc_chess_piece_genericAI(pCreature) { Reset(); }

    void JustDied(Unit* pKiller) override
    {
        npc_chess_piece_genericAI::JustDied(pKiller);

        if (!m_pInstance)
            return;

        Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_ECHO_MEDIVH);
        if (!pMedivh)
            return;

        if (m_pInstance->GetPlayerTeam() == HORDE)
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_QUEEN_PLAYER);
        else
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_QUEEN_MEDIVH);

        m_pInstance->DoMoveChessPieceToSides(SPELL_TRANSFORM_WARLOCK, FACTION_ID_CHESS_HORDE);
    }

    uint32 DoCastPrimarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 20.0f))
        {
            DoCastSpellIfCan(pTarget, SPELL_FIREBALL);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_FIREBALL);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }

    uint32 DoCastSecondarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 25.0f))
        {
            DoCastSpellIfCan(pTarget, SPELL_POISON_CLOUD_ACTION);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_POISON_CLOUD_ACTION);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }
};

CreatureAI* GetAI_npc_orc_warlock(Creature* pCreature)
{
    return new npc_orc_warlockAI(pCreature);
}

bool GossipHello_npc_orc_warlock(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(SPELL_RECENTLY_IN_GAME) || pCreature->HasAura(SPELL_CONTROL_PIECE))
        return true;

    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if ((pInstance->GetData(TYPE_CHESS) == IN_PROGRESS && pPlayer->GetTeam() == HORDE) || pInstance->GetData(TYPE_CHESS) == SPECIAL)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ORC_WARLOCK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_WARLOCK, pCreature->GetObjectGuid());
    return true;
}

/*######
## npc_human_footman
######*/

struct npc_human_footmanAI : public npc_chess_piece_genericAI
{
    npc_human_footmanAI(Creature* pCreature) : npc_chess_piece_genericAI(pCreature) { Reset(); }

    void JustDied(Unit* pKiller) override
    {
        npc_chess_piece_genericAI::JustDied(pKiller);

        if (!m_pInstance)
            return;

        Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_ECHO_MEDIVH);
        if (!pMedivh)
            return;

        if (m_pInstance->GetPlayerTeam() == ALLIANCE)
        {
            switch (urand(0, 2))
            {
                case 0: DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_PAWN_PLAYER_1); break;
                case 1: DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_PAWN_PLAYER_2); break;
                case 2: DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_PAWN_PLAYER_3); break;
            }
        }
        else
        {
            switch (urand(0, 2))
            {
                case 0: DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_PAWN_MEDIVH_1); break;
                case 1: DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_PAWN_MEDIVH_2); break;
                case 2: DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_PAWN_MEDIVH_3); break;
            }
        }

        m_pInstance->DoMoveChessPieceToSides(SPELL_TRANSFORM_FOOTMAN, FACTION_ID_CHESS_ALLIANCE);
    }

    uint32 DoCastPrimarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 8.0f, M_PI_F / 12))
        {
            DoCastSpellIfCan(m_creature, SPELL_HEROIC_BLOW);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_HEROIC_BLOW);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }

    uint32 DoCastSecondarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 8.0f))
        {
            DoCastSpellIfCan(m_creature, SPELL_SHIELD_BLOCK);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_SHIELD_BLOCK);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }
};

CreatureAI* GetAI_npc_human_footman(Creature* pCreature)
{
    return new npc_human_footmanAI(pCreature);
}

bool GossipHello_npc_human_footman(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(SPELL_RECENTLY_IN_GAME) || pCreature->HasAura(SPELL_CONTROL_PIECE))
        return true;

    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if ((pInstance->GetData(TYPE_CHESS) == IN_PROGRESS && pPlayer->GetTeam() == ALLIANCE) || pInstance->GetData(TYPE_CHESS) == SPECIAL)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_HUMAN_FOOTMAN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_FOOTMAN, pCreature->GetObjectGuid());
    return true;
}

/*######
## npc_orc_grunt
######*/

struct npc_orc_gruntAI : public npc_chess_piece_genericAI
{
    npc_orc_gruntAI(Creature* pCreature) : npc_chess_piece_genericAI(pCreature) { Reset(); }

    void JustDied(Unit* pKiller) override
    {
        npc_chess_piece_genericAI::JustDied(pKiller);

        if (!m_pInstance)
            return;

        Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_ECHO_MEDIVH);
        if (!pMedivh)
            return;

        if (m_pInstance->GetPlayerTeam() == HORDE)
        {
            switch (urand(0, 2))
            {
                case 0: DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_PAWN_PLAYER_1); break;
                case 1: DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_PAWN_PLAYER_2); break;
                case 2: DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_PAWN_PLAYER_3); break;
            }
        }
        else
        {
            switch (urand(0, 2))
            {
                case 0: DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_PAWN_MEDIVH_1); break;
                case 1: DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_PAWN_MEDIVH_2); break;
                case 2: DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_PAWN_MEDIVH_3); break;
            }
        }

        m_pInstance->DoMoveChessPieceToSides(SPELL_TRANSFORM_GRUNT, FACTION_ID_CHESS_HORDE);
    }

    uint32 DoCastPrimarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 8.0f, M_PI_F / 12))
        {
            DoCastSpellIfCan(m_creature, SPELL_VICIOUS_STRIKE);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_VICIOUS_STRIKE);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }

    uint32 DoCastSecondarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 8.0f))
        {
            DoCastSpellIfCan(m_creature, SPELL_WEAPON_DEFLECTION);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_WEAPON_DEFLECTION);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }
};

CreatureAI* GetAI_npc_orc_grunt(Creature* pCreature)
{
    return new npc_orc_gruntAI(pCreature);
}

bool GossipHello_npc_orc_grunt(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(SPELL_RECENTLY_IN_GAME) || pCreature->HasAura(SPELL_CONTROL_PIECE))
        return true;

    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if ((pInstance->GetData(TYPE_CHESS) == IN_PROGRESS && pPlayer->GetTeam() == HORDE) || pInstance->GetData(TYPE_CHESS) == SPECIAL)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ORC_GRUNT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_GRUNT, pCreature->GetObjectGuid());
    return true;
}

/*######
## npc_water_elemental
######*/

struct npc_water_elementalAI : public npc_chess_piece_genericAI
{
    npc_water_elementalAI(Creature* pCreature) : npc_chess_piece_genericAI(pCreature) { Reset(); }

    void JustDied(Unit* pKiller) override
    {
        npc_chess_piece_genericAI::JustDied(pKiller);

        if (!m_pInstance)
            return;

        Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_ECHO_MEDIVH);
        if (!pMedivh)
            return;

        if (m_pInstance->GetPlayerTeam() == ALLIANCE)
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_ROOK_PLAYER);
        else
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_ROOK_MEDIVH);

        m_pInstance->DoMoveChessPieceToSides(SPELL_TRANSFORM_WATER_ELEM, FACTION_ID_CHESS_ALLIANCE);
    }

    uint32 DoCastPrimarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 9.0f))
        {
            DoCastSpellIfCan(m_creature, SPELL_GEYSER);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_GEYSER);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }

    uint32 DoCastSecondarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 9.0f))
        {
            DoCastSpellIfCan(m_creature, SPELL_WATER_SHIELD);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_WATER_SHIELD);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }
};

CreatureAI* GetAI_npc_water_elemental(Creature* pCreature)
{
    return new npc_water_elementalAI(pCreature);
}

bool GossipHello_npc_water_elemental(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(SPELL_RECENTLY_IN_GAME) || pCreature->HasAura(SPELL_CONTROL_PIECE))
        return true;

    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if ((pInstance->GetData(TYPE_CHESS) == IN_PROGRESS && pPlayer->GetTeam() == ALLIANCE) || pInstance->GetData(TYPE_CHESS) == SPECIAL)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_WATER_ELEMENTAL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_ELEMENTAL, pCreature->GetObjectGuid());
    return true;
}

/*######
## npc_summoned_daemon
######*/

struct npc_summoned_daemonAI : public npc_chess_piece_genericAI
{
    npc_summoned_daemonAI(Creature* pCreature) : npc_chess_piece_genericAI(pCreature) { Reset(); }

    void JustDied(Unit* pKiller) override
    {
        npc_chess_piece_genericAI::JustDied(pKiller);

        if (!m_pInstance)
            return;

        Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_ECHO_MEDIVH);
        if (!pMedivh)
            return;

        if (m_pInstance->GetPlayerTeam() == HORDE)
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_ROOK_PLAYER);
        else
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_ROOK_MEDIVH);

        m_pInstance->DoMoveChessPieceToSides(SPELL_TRANSFORM_DAEMON, FACTION_ID_CHESS_HORDE);
    }

    uint32 DoCastPrimarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 9.0f))
        {
            DoCastSpellIfCan(m_creature, SPELL_HELLFIRE);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_HELLFIRE);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }

    uint32 DoCastSecondarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 9.0f))
        {
            DoCastSpellIfCan(m_creature, SPELL_FIRE_SHIELD);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_FIRE_SHIELD);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }
};

CreatureAI* GetAI_npc_summoned_daemon(Creature* pCreature)
{
    return new npc_summoned_daemonAI(pCreature);
}

bool GossipHello_npc_summoned_daemon(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(SPELL_RECENTLY_IN_GAME) || pCreature->HasAura(SPELL_CONTROL_PIECE))
        return true;

    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if ((pInstance->GetData(TYPE_CHESS) == IN_PROGRESS && pPlayer->GetTeam() == HORDE) || pInstance->GetData(TYPE_CHESS) == SPECIAL)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SUMMONED_DEAMON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_DEAMON, pCreature->GetObjectGuid());
    return true;
}

/*######
## npc_human_charger
######*/

struct npc_human_chargerAI : public npc_chess_piece_genericAI
{
    npc_human_chargerAI(Creature* pCreature) : npc_chess_piece_genericAI(pCreature) { Reset(); }

    void JustDied(Unit* pKiller) override
    {
        npc_chess_piece_genericAI::JustDied(pKiller);

        if (!m_pInstance)
            return;

        Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_ECHO_MEDIVH);
        if (!pMedivh)
            return;

        if (m_pInstance->GetPlayerTeam() == ALLIANCE)
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_KNIGHT_PLAYER);
        else
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_KNIGHT_MEDIVH);

        m_pInstance->DoMoveChessPieceToSides(SPELL_TRANSFORM_CHARGER, FACTION_ID_CHESS_ALLIANCE);
    }

    uint32 DoCastPrimarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 8.0f, M_PI_F / 12))
        {
            DoCastSpellIfCan(m_creature, SPELL_SMASH);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_SMASH);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }

    uint32 DoCastSecondarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 10.0f, M_PI_F / 12))
        {
            DoCastSpellIfCan(m_creature, SPELL_STOMP);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_STOMP);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }
};

CreatureAI* GetAI_npc_human_charger(Creature* pCreature)
{
    return new npc_human_chargerAI(pCreature);
}

bool GossipHello_npc_human_charger(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(SPELL_RECENTLY_IN_GAME) || pCreature->HasAura(SPELL_CONTROL_PIECE))
        return true;

    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if ((pInstance->GetData(TYPE_CHESS) == IN_PROGRESS && pPlayer->GetTeam() == ALLIANCE) || pInstance->GetData(TYPE_CHESS) == SPECIAL)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_HUMAN_CHARGER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_CHARGER, pCreature->GetObjectGuid());
    return true;
}

/*######
## npc_orc_wolf
######*/

struct npc_orc_wolfAI : public npc_chess_piece_genericAI
{
    npc_orc_wolfAI(Creature* pCreature) : npc_chess_piece_genericAI(pCreature) { Reset(); }

    void JustDied(Unit* pKiller) override
    {
        npc_chess_piece_genericAI::JustDied(pKiller);

        if (!m_pInstance)
            return;

        Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_ECHO_MEDIVH);
        if (!pMedivh)
            return;

        if (m_pInstance->GetPlayerTeam() == HORDE)
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_KNIGHT_PLAYER);
        else
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_KNIGHT_MEDIVH);

        m_pInstance->DoMoveChessPieceToSides(SPELL_TRANSFORM_WOLF, FACTION_ID_CHESS_HORDE);
    }

    uint32 DoCastPrimarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 8.0f, M_PI_F / 12))
        {
            DoCastSpellIfCan(m_creature, SPELL_BITE);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_BITE);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }

    uint32 DoCastSecondarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 10.0f, M_PI_F / 12))
        {
            DoCastSpellIfCan(m_creature, SPELL_HOWL);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_HOWL);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }
};

CreatureAI* GetAI_npc_orc_wolf(Creature* pCreature)
{
    return new npc_orc_wolfAI(pCreature);
}

bool GossipHello_npc_orc_wolf(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(SPELL_RECENTLY_IN_GAME) || pCreature->HasAura(SPELL_CONTROL_PIECE))
        return true;

    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if ((pInstance->GetData(TYPE_CHESS) == IN_PROGRESS && pPlayer->GetTeam() == HORDE) || pInstance->GetData(TYPE_CHESS) == SPECIAL)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ORC_WOLF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_WOLF, pCreature->GetObjectGuid());
    return true;
}

/*######
## npc_human_cleric
######*/

struct npc_human_clericAI : public npc_chess_piece_genericAI
{
    npc_human_clericAI(Creature* pCreature) : npc_chess_piece_genericAI(pCreature) { Reset(); }

    void JustDied(Unit* pKiller) override
    {
        npc_chess_piece_genericAI::JustDied(pKiller);

        if (!m_pInstance)
            return;

        Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_ECHO_MEDIVH);
        if (!pMedivh)
            return;

        if (m_pInstance->GetPlayerTeam() == ALLIANCE)
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_BISHOP_PLAYER);
        else
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_BISHOP_MEDIVH);

        m_pInstance->DoMoveChessPieceToSides(SPELL_TRANSFORM_CLERIC, FACTION_ID_CHESS_ALLIANCE);
    }

    uint32 DoCastPrimarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_FRIENDLY, 25.0f))
        {
            DoCastSpellIfCan(pTarget, SPELL_HEALING);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_HEALING);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }

    uint32 DoCastSecondarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 18.0f, M_PI_F / 12))
        {
            DoCastSpellIfCan(m_creature, SPELL_HOLY_LANCE);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_HOLY_LANCE);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }
};

CreatureAI* GetAI_npc_human_cleric(Creature* pCreature)
{
    return new npc_human_clericAI(pCreature);
}

bool GossipHello_npc_human_cleric(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(SPELL_RECENTLY_IN_GAME) || pCreature->HasAura(SPELL_CONTROL_PIECE))
        return true;

    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if ((pInstance->GetData(TYPE_CHESS) == IN_PROGRESS && pPlayer->GetTeam() == ALLIANCE) || pInstance->GetData(TYPE_CHESS) == SPECIAL)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_HUMAN_CLERIC, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_CLERIC, pCreature->GetObjectGuid());
    return true;
}

/*######
## npc_orc_necrolyte
######*/

struct npc_orc_necrolyteAI : public npc_chess_piece_genericAI
{
    npc_orc_necrolyteAI(Creature* pCreature) : npc_chess_piece_genericAI(pCreature) { Reset(); }

    void JustDied(Unit* pKiller) override
    {
        npc_chess_piece_genericAI::JustDied(pKiller);

        if (!m_pInstance)
            return;

        Creature* pMedivh = m_pInstance->GetSingleCreatureFromStorage(NPC_ECHO_MEDIVH);
        if (!pMedivh)
            return;

        if (m_pInstance->GetPlayerTeam() == HORDE)
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_BISHOP_PLAYER);
        else
            DoPlaySoundToSet(pMedivh, SOUND_ID_LOSE_BISHOP_MEDIVH);

        m_pInstance->DoMoveChessPieceToSides(SPELL_TRANSFORM_NECROLYTE, FACTION_ID_CHESS_HORDE);
    }

    uint32 DoCastPrimarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_FRIENDLY, 25.0f))
        {
            DoCastSpellIfCan(pTarget, SPELL_SHADOW_MEND_ACTION);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_SHADOW_MEND_ACTION);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }

    uint32 DoCastSecondarySpell() override
    {
        if (Unit* pTarget = GetTargetByType(TARGET_TYPE_RANDOM, 18.0f, M_PI_F / 12))
        {
            DoCastSpellIfCan(m_creature, SPELL_SHADOW_SPEAR);

            // reset timer based on spell values
            const SpellEntry* pSpell = GetSpellStore()->LookupEntry(SPELL_SHADOW_SPEAR);
            return pSpell->RecoveryTime ? pSpell->RecoveryTime : pSpell->CategoryRecoveryTime;
        }

        return 5000;
    }
};

CreatureAI* GetAI_npc_orc_necrolyte(Creature* pCreature)
{
    return new npc_orc_necrolyteAI(pCreature);
}

bool GossipHello_npc_orc_necrolyte(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(SPELL_RECENTLY_IN_GAME) || pCreature->HasAura(SPELL_CONTROL_PIECE))
        return true;

    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if ((pInstance->GetData(TYPE_CHESS) == IN_PROGRESS && pPlayer->GetTeam() == HORDE) || pInstance->GetData(TYPE_CHESS) == SPECIAL)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ORC_NECROLYTE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ID_NECROLYTE, pCreature->GetObjectGuid());
    return true;
}

void AddSC_chess_event()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_echo_of_medivh";
    pNewScript->GetAI = GetAI_npc_echo_of_medivh;
    pNewScript->pGossipHello = GossipHello_npc_echo_of_medivh;
    pNewScript->pGossipSelect = GossipSelect_npc_echo_of_medivh;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_king_llane";
    pNewScript->GetAI = GetAI_npc_king_llane;
    pNewScript->pGossipHello = GossipHello_npc_king_llane;
    pNewScript->pGossipSelect = GossipSelect_npc_chess_generic;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_chess_generic;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_warchief_blackhand";
    pNewScript->GetAI = GetAI_npc_warchief_blackhand;
    pNewScript->pGossipHello = GossipHello_npc_warchief_blackhand;
    pNewScript->pGossipSelect = GossipSelect_npc_chess_generic;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_chess_generic;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_human_conjurer";
    pNewScript->GetAI = GetAI_npc_human_conjurer;
    pNewScript->pGossipHello = GossipHello_npc_human_conjurer;
    pNewScript->pGossipSelect = GossipSelect_npc_chess_generic;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_chess_generic;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_orc_warlock";
    pNewScript->GetAI = GetAI_npc_orc_warlock;
    pNewScript->pGossipHello = GossipHello_npc_orc_warlock;
    pNewScript->pGossipSelect = GossipSelect_npc_chess_generic;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_chess_generic;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_human_footman";
    pNewScript->GetAI = GetAI_npc_human_footman;
    pNewScript->pGossipHello = GossipHello_npc_human_footman;
    pNewScript->pGossipSelect = GossipSelect_npc_chess_generic;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_chess_generic;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_orc_grunt";
    pNewScript->GetAI = GetAI_npc_orc_grunt;
    pNewScript->pGossipHello = GossipHello_npc_orc_grunt;
    pNewScript->pGossipSelect = GossipSelect_npc_chess_generic;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_chess_generic;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_water_elemental";
    pNewScript->GetAI = GetAI_npc_water_elemental;
    pNewScript->pGossipHello = GossipHello_npc_water_elemental;
    pNewScript->pGossipSelect = GossipSelect_npc_chess_generic;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_chess_generic;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_summoned_daemon";
    pNewScript->GetAI = GetAI_npc_summoned_daemon;
    pNewScript->pGossipHello = GossipHello_npc_summoned_daemon;
    pNewScript->pGossipSelect = GossipSelect_npc_chess_generic;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_chess_generic;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_human_charger";
    pNewScript->GetAI = GetAI_npc_human_charger;
    pNewScript->pGossipHello = GossipHello_npc_human_charger;
    pNewScript->pGossipSelect = GossipSelect_npc_chess_generic;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_chess_generic;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_orc_wolf";
    pNewScript->GetAI = GetAI_npc_orc_wolf;
    pNewScript->pGossipHello = GossipHello_npc_orc_wolf;
    pNewScript->pGossipSelect = GossipSelect_npc_chess_generic;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_chess_generic;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_human_cleric";
    pNewScript->GetAI = GetAI_npc_human_cleric;
    pNewScript->pGossipHello = GossipHello_npc_human_cleric;
    pNewScript->pGossipSelect = GossipSelect_npc_chess_generic;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_chess_generic;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_orc_necrolyte";
    pNewScript->GetAI = GetAI_npc_orc_necrolyte;
    pNewScript->pGossipHello = GossipHello_npc_orc_necrolyte;
    pNewScript->pGossipSelect = GossipSelect_npc_chess_generic;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_chess_generic;
    pNewScript->RegisterSelf();
}

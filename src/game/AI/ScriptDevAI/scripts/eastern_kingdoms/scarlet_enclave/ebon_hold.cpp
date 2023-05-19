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
SDName: Ebon_Hold
SD%Complete: 95
SDComment: Quest support: 12619, 12641, 12687, 12698, 12733, 12739(and 12742 to 12750), 12754, 12801
SDCategory: Ebon Hold
EndScriptData */

/* ContentData
npc_a_special_surprise
npc_death_knight_initiate
npc_eye_of_acherus
npc_scarlet_ghoul
npc_highlord_darion_mograine
npc_scarlet_courier
spell_emblazon_runeblade
spell_death_knight_initiate_visual
spell_siphon_of_acherus
spell_recall_eye_of_acherus
spell_summon_ghouls_scarlet_crusade
go_plague_cauldron
spell_devour_humanoid
spell_portal_to_capital_city
spell_acherus_deathcharger
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "world_map_ebon_hold.h"
#include "AI/ScriptDevAI/base/pet_ai.h"
#include "Entities/TemporarySpawn.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

/*######
## npc_a_special_surprise
######*/

enum SpecialSurprise
{
    SAY_EXEC_START_1            = -1609025,                 // speech for all
    SAY_EXEC_START_2            = -1609026,
    SAY_EXEC_START_3            = -1609027,

    SAY_EXEC_PROG_1             = -1609028,
    SAY_EXEC_PROG_2             = -1609029,
    SAY_EXEC_PROG_3             = -1609030,
    SAY_EXEC_PROG_4             = -1609031,
    SAY_EXEC_PROG_5             = -1609032,
    SAY_EXEC_PROG_6             = -1609033,
    SAY_EXEC_PROG_7             = -1609034,

    SAY_EXEC_NAME_1             = -1609035,
    SAY_EXEC_NAME_2             = -1609036,

    SAY_EXEC_RECOG_1            = -1609037,
    SAY_EXEC_RECOG_2            = -1609038,
    SAY_EXEC_RECOG_3            = -1609039,
    SAY_EXEC_RECOG_4            = -1609040,
    SAY_EXEC_RECOG_5            = -1609041,
    SAY_EXEC_RECOG_6            = -1609042,

    SAY_EXEC_NOREM_1            = -1609043,
    SAY_EXEC_NOREM_2            = -1609044,
    SAY_EXEC_NOREM_3            = -1609045,
    SAY_EXEC_NOREM_4            = -1609046,
    SAY_EXEC_NOREM_5            = -1609047,
    SAY_EXEC_NOREM_6            = -1609048,
    SAY_EXEC_NOREM_7            = -1609049,
    SAY_EXEC_NOREM_8            = -1609050,
    SAY_EXEC_NOREM_9            = -1609051,
    SAY_EXEC_NOREM_10           = -1609015,

    SAY_EXEC_THINK_1            = -1609052,
    SAY_EXEC_THINK_2            = -1609053,
    SAY_EXEC_THINK_3            = -1609054,
    SAY_EXEC_THINK_4            = -1609055,
    SAY_EXEC_THINK_5            = -1609056,
    SAY_EXEC_THINK_6            = -1609057,
    SAY_EXEC_THINK_7            = -1609058,
    SAY_EXEC_THINK_8            = -1609059,
    SAY_EXEC_THINK_9            = -1609060,
    SAY_EXEC_THINK_10           = -1609061,

    SAY_EXEC_LISTEN_1           = -1609062,
    SAY_EXEC_LISTEN_2           = -1609063,
    SAY_EXEC_LISTEN_3           = -1609064,
    SAY_EXEC_LISTEN_4           = -1609065,

    SAY_PLAGUEFIST              = -1609066,

    SAY_EXEC_TIME_1             = -1609067,
    SAY_EXEC_TIME_2             = -1609068,
    SAY_EXEC_TIME_3             = -1609069,
    SAY_EXEC_TIME_4             = -1609070,
    SAY_EXEC_TIME_5             = -1609071,
    SAY_EXEC_TIME_6             = -1609072,
    SAY_EXEC_TIME_7             = -1609073,
    SAY_EXEC_TIME_8             = -1609074,
    SAY_EXEC_TIME_9             = -1609075,
    SAY_EXEC_TIME_10            = -1609076,

    SAY_EXEC_WAITING            = -1609077,
    EMOTE_DIES                  = -1609078,

    // creature entries
    NPC_PRISONER_TAUREN         = 29032,
    NPC_PRISONER_HUMAN          = 29061,
    NPC_PRISONER_NIGHT_ELF      = 29065,
    NPC_PRISONER_DWARF          = 29067,
    NPC_PRISONER_GNOME          = 29068,
    NPC_PRISONER_DRAENEI        = 29070,
    NPC_PRISONER_UNDEAD         = 29071,
    NPC_PRISONER_ORC            = 29072,
    NPC_PRISONER_TROLL          = 29073,
    NPC_PRISONER_BLOOD_ELF      = 29074,

    NPC_PLAGUEFIST              = 29053,

    // event data
    DATA_NPC_STAND              = 1,
    DATA_NPC_TALK               = 2,
    DATA_NPC_EMOTE              = 3,
    DATA_NPC_EXCLAMATION        = 4,
    DATA_NPC_CRY                = 5,
    DATA_NPC_EXECUTION          = 6,
};

static const DialogueEntry aSurpriseDialogue[] =
{
    // Tauren
    {NPC_PRISONER_TAUREN,   0,                      2000},
    {SAY_EXEC_START_1,      NPC_PRISONER_TAUREN,    4000},
    {DATA_NPC_STAND,        0,                      1000},
    {SAY_EXEC_PROG_1,       NPC_PRISONER_TAUREN,    2000},
    {SAY_EXEC_NAME_1,       NPC_PRISONER_TAUREN,    4000},
    {SAY_EXEC_RECOG_5,      NPC_PRISONER_TAUREN,    6000},
    {SAY_EXEC_NOREM_8,      NPC_PRISONER_TAUREN,    5000},
    {DATA_NPC_TALK,         0,                      3000},
    {SAY_EXEC_THINK_9,      NPC_PRISONER_TAUREN,    5000},
    {DATA_NPC_EMOTE,        0,                      5000},
    {SAY_EXEC_LISTEN_1,     NPC_PRISONER_TAUREN,    5000},
    {DATA_NPC_TALK,         0,                      5000},
    {DATA_NPC_EXCLAMATION,  0,                      4000},
    {SAY_PLAGUEFIST,        NPC_PLAGUEFIST,         4000},
    {SAY_EXEC_TIME_9,       NPC_PRISONER_TAUREN,    3000},
    {DATA_NPC_CRY,          0,                      3000},
    {DATA_NPC_EXECUTION,    0,                      1000},
    {SAY_EXEC_WAITING,      NPC_PRISONER_TAUREN,    0},

    // Human
    {NPC_PRISONER_HUMAN,    0,                      2000},
    {SAY_EXEC_START_1,      NPC_PRISONER_HUMAN,     4000},
    {DATA_NPC_STAND,        0,                      1000},
    {SAY_EXEC_PROG_5,       NPC_PRISONER_HUMAN,     2000},
    {SAY_EXEC_NAME_1,       NPC_PRISONER_HUMAN,     4000},
    {SAY_EXEC_RECOG_1,      NPC_PRISONER_HUMAN,     6000},
    {SAY_EXEC_NOREM_5,      NPC_PRISONER_HUMAN,     5000},
    {DATA_NPC_TALK,         0,                      3000},
    {SAY_EXEC_THINK_7,      NPC_PRISONER_HUMAN,     5000},
    {DATA_NPC_EMOTE,        0,                      5000},
    {SAY_EXEC_LISTEN_1,     NPC_PRISONER_HUMAN,     5000},
    {DATA_NPC_TALK,         0,                      5000},
    {DATA_NPC_EXCLAMATION,  0,                      4000},
    {SAY_PLAGUEFIST,        NPC_PLAGUEFIST,         4000},
    {SAY_EXEC_TIME_6,       NPC_PRISONER_HUMAN,     3000},
    {DATA_NPC_CRY,          0,                      3000},
    {DATA_NPC_EXECUTION,    0,                      1000},
    {SAY_EXEC_WAITING,      NPC_PRISONER_HUMAN,     0},

    // Night Elf
    {NPC_PRISONER_NIGHT_ELF, 0,                     2000},
    {SAY_EXEC_START_1,      NPC_PRISONER_NIGHT_ELF, 4000},
    {DATA_NPC_STAND,        0,                      1000},
    {SAY_EXEC_PROG_1,       NPC_PRISONER_NIGHT_ELF, 2000},
    {SAY_EXEC_NAME_1,       NPC_PRISONER_NIGHT_ELF, 4000},
    {SAY_EXEC_RECOG_1,      NPC_PRISONER_NIGHT_ELF, 6000},
    {SAY_EXEC_NOREM_6,      NPC_PRISONER_NIGHT_ELF, 5000},
    {DATA_NPC_TALK,         0,                      3000},
    {SAY_EXEC_THINK_2,      NPC_PRISONER_NIGHT_ELF, 5000},
    {DATA_NPC_EMOTE,        0,                      5000},
    {SAY_EXEC_LISTEN_1,     NPC_PRISONER_NIGHT_ELF, 5000},
    {DATA_NPC_TALK,         0,                      5000},
    {DATA_NPC_EXCLAMATION,  0,                      4000},
    {SAY_PLAGUEFIST,        NPC_PLAGUEFIST,         4000},
    {SAY_EXEC_TIME_7,       NPC_PRISONER_NIGHT_ELF, 3000},
    {DATA_NPC_CRY,          0,                      3000},
    {DATA_NPC_EXECUTION,    0,                      1000},
    {SAY_EXEC_WAITING,      NPC_PRISONER_NIGHT_ELF, 0},

    // Dwarf
    {NPC_PRISONER_DWARF,    0,                      2000},
    {SAY_EXEC_START_2,      NPC_PRISONER_DWARF,     4000},
    {DATA_NPC_STAND,        0,                      1000},
    {SAY_EXEC_PROG_2,       NPC_PRISONER_DWARF,     2000},
    {SAY_EXEC_NAME_1,       NPC_PRISONER_DWARF,     4000},
    {SAY_EXEC_RECOG_3,      NPC_PRISONER_DWARF,     6000},
    {SAY_EXEC_NOREM_2,      NPC_PRISONER_DWARF,     5000},
    {DATA_NPC_TALK,         0,                      3000},
    {SAY_EXEC_THINK_5,      NPC_PRISONER_DWARF,     5000},
    {DATA_NPC_EMOTE,        0,                      5000},
    {SAY_EXEC_LISTEN_2,     NPC_PRISONER_DWARF,     5000},
    {DATA_NPC_TALK,         0,                      5000},
    {DATA_NPC_EXCLAMATION,  0,                      4000},
    {SAY_PLAGUEFIST,        NPC_PLAGUEFIST,         4000},
    {SAY_EXEC_TIME_3,       NPC_PRISONER_DWARF,     3000},
    {DATA_NPC_CRY,          0,                      3000},
    {DATA_NPC_EXECUTION,    0,                      1000},
    {SAY_EXEC_WAITING,      NPC_PRISONER_DWARF,     0},

    // Gnome
    {NPC_PRISONER_GNOME,    0,                      2000},
    {SAY_EXEC_START_1,      NPC_PRISONER_GNOME,     4000},
    {DATA_NPC_STAND,        0,                      1000},
    {SAY_EXEC_PROG_4,       NPC_PRISONER_GNOME,     2000},
    {SAY_EXEC_NAME_1,       NPC_PRISONER_GNOME,     4000},
    {SAY_EXEC_RECOG_1,      NPC_PRISONER_GNOME,     6000},
    {SAY_EXEC_NOREM_4,      NPC_PRISONER_GNOME,     5000},
    {DATA_NPC_TALK,         0,                      3000},
    {SAY_EXEC_THINK_6,      NPC_PRISONER_GNOME,     5000},
    {DATA_NPC_EMOTE,        0,                      5000},
    {SAY_EXEC_LISTEN_1,     NPC_PRISONER_GNOME,     5000},
    {DATA_NPC_TALK,         0,                      5000},
    {DATA_NPC_EXCLAMATION,  0,                      4000},
    {SAY_PLAGUEFIST,        NPC_PLAGUEFIST,         4000},
    {SAY_EXEC_TIME_5,       NPC_PRISONER_GNOME,     3000},
    {DATA_NPC_CRY,          0,                      3000},
    {DATA_NPC_EXECUTION,    0,                      1000},
    {SAY_EXEC_WAITING,      NPC_PRISONER_GNOME,     0},

    // Draenei
    {NPC_PRISONER_DRAENEI,  0,                      2000},
    {SAY_EXEC_START_1,      NPC_PRISONER_DRAENEI,   4000},
    {DATA_NPC_STAND,        0,                      1000},
    {SAY_EXEC_PROG_1,       NPC_PRISONER_DRAENEI,   2000},
    {SAY_EXEC_NAME_1,       NPC_PRISONER_DRAENEI,   4000},
    {SAY_EXEC_RECOG_2,      NPC_PRISONER_DRAENEI,   6000},
    {SAY_EXEC_NOREM_1,      NPC_PRISONER_DRAENEI,   5000},
    {DATA_NPC_TALK,         0,                      3000},
    {SAY_EXEC_THINK_4,      NPC_PRISONER_DRAENEI,   5000},
    {DATA_NPC_EMOTE,        0,                      5000},
    {SAY_EXEC_LISTEN_1,     NPC_PRISONER_DRAENEI,   5000},
    {DATA_NPC_TALK,         0,                      5000},
    {DATA_NPC_EXCLAMATION,  0,                      4000},
    {SAY_PLAGUEFIST,        NPC_PLAGUEFIST,         4000},
    {SAY_EXEC_TIME_2,       NPC_PRISONER_DRAENEI,   3000},
    {DATA_NPC_CRY,          0,                      3000},
    {DATA_NPC_EXECUTION,    0,                      1000},
    {SAY_EXEC_WAITING,      NPC_PRISONER_DRAENEI,   0},

    // Undead
    {NPC_PRISONER_UNDEAD,   0,                      2000},
    {SAY_EXEC_START_1,      NPC_PRISONER_UNDEAD,    4000},
    {DATA_NPC_STAND,        0,                      1000},
    {SAY_EXEC_PROG_3,       NPC_PRISONER_UNDEAD,    2000},
    {SAY_EXEC_NAME_1,       NPC_PRISONER_UNDEAD,    4000},
    {SAY_EXEC_RECOG_4,      NPC_PRISONER_UNDEAD,    6000},
    {SAY_EXEC_NOREM_3,      NPC_PRISONER_UNDEAD,    5000},
    {DATA_NPC_TALK,         0,                      3000},
    {SAY_EXEC_THINK_1,      NPC_PRISONER_UNDEAD,    5000},
    {DATA_NPC_EMOTE,        0,                      5000},
    {SAY_EXEC_LISTEN_3,     NPC_PRISONER_UNDEAD,    5000},
    {DATA_NPC_TALK,         0,                      5000},
    {DATA_NPC_EXCLAMATION,  0,                      4000},
    {SAY_PLAGUEFIST,        NPC_PLAGUEFIST,         4000},
    {SAY_EXEC_TIME_4,       NPC_PRISONER_UNDEAD,    3000},
    {DATA_NPC_CRY,          0,                      3000},
    {DATA_NPC_EXECUTION,    0,                      1000},
    {SAY_EXEC_WAITING,      NPC_PRISONER_UNDEAD,    0},

    // Orc
    {NPC_PRISONER_ORC,      0,                      2000},
    {SAY_EXEC_START_1,      NPC_PRISONER_ORC,       4000},
    {DATA_NPC_STAND,        0,                      1000},
    {SAY_EXEC_PROG_6,       NPC_PRISONER_ORC,       2000},
    {SAY_EXEC_NAME_1,       NPC_PRISONER_ORC,       4000},
    {SAY_EXEC_RECOG_1,      NPC_PRISONER_ORC,       6000},
    {SAY_EXEC_NOREM_7,      NPC_PRISONER_ORC,       5000},
    {DATA_NPC_TALK,         0,                      3000},
    {SAY_EXEC_THINK_8,      NPC_PRISONER_ORC,       5000},
    {DATA_NPC_EMOTE,        0,                      5000},
    {SAY_EXEC_LISTEN_1,     NPC_PRISONER_ORC,       5000},
    {DATA_NPC_TALK,         0,                      5000},
    {DATA_NPC_EXCLAMATION,  0,                      4000},
    {SAY_PLAGUEFIST,        NPC_PLAGUEFIST,         4000},
    {SAY_EXEC_TIME_8,       NPC_PRISONER_ORC,       3000},
    {DATA_NPC_CRY,          0,                      3000},
    {DATA_NPC_EXECUTION,    0,                      1000},
    {SAY_EXEC_WAITING,      NPC_PRISONER_ORC,       0},

    // Troll
    {NPC_PRISONER_TROLL,    0,                      2000},
    {SAY_EXEC_START_3,      NPC_PRISONER_TROLL,     4000},
    {DATA_NPC_STAND,        0,                      1000},
    {SAY_EXEC_PROG_7,       NPC_PRISONER_TROLL,     2000},
    {SAY_EXEC_NAME_2,       NPC_PRISONER_TROLL,     4000},
    {SAY_EXEC_RECOG_6,      NPC_PRISONER_TROLL,     6000},
    {SAY_EXEC_NOREM_9,      NPC_PRISONER_TROLL,     5000},
    {DATA_NPC_TALK,         0,                      3000},
    {SAY_EXEC_THINK_10,     NPC_PRISONER_TROLL,     5000},
    {DATA_NPC_EMOTE,        0,                      5000},
    {SAY_EXEC_LISTEN_4,     NPC_PRISONER_TROLL,     5000},
    {DATA_NPC_TALK,         0,                      5000},
    {DATA_NPC_EXCLAMATION,  0,                      4000},
    {SAY_PLAGUEFIST,        NPC_PLAGUEFIST,         4000},
    {SAY_EXEC_TIME_10,      NPC_PRISONER_TROLL,     3000},
    {DATA_NPC_CRY,          0,                      3000},
    {DATA_NPC_EXECUTION,    0,                      1000},
    {SAY_EXEC_WAITING,      NPC_PRISONER_TROLL,     0},

    // Blood Elf
    {NPC_PRISONER_BLOOD_ELF, 0,                     2000},
    {SAY_EXEC_START_1,      NPC_PRISONER_BLOOD_ELF, 4000},
    {DATA_NPC_STAND,        0,                      1000},
    {SAY_EXEC_PROG_1,       NPC_PRISONER_BLOOD_ELF, 2000},
    {SAY_EXEC_NAME_1,       NPC_PRISONER_BLOOD_ELF, 4000},
    {SAY_EXEC_RECOG_1,      NPC_PRISONER_BLOOD_ELF, 6000},
    {SAY_EXEC_NOREM_10,     NPC_PRISONER_BLOOD_ELF, 5000},
    {DATA_NPC_TALK,         0,                      3000},
    {SAY_EXEC_THINK_3,      NPC_PRISONER_BLOOD_ELF, 5000},
    {DATA_NPC_EMOTE,        0,                      5000},
    {SAY_EXEC_LISTEN_1,     NPC_PRISONER_BLOOD_ELF, 5000},
    {DATA_NPC_TALK,         0,                      5000},
    {DATA_NPC_EXCLAMATION,  0,                      4000},
    {SAY_PLAGUEFIST,        NPC_PLAGUEFIST,         4000},
    {SAY_EXEC_TIME_1,       NPC_PRISONER_BLOOD_ELF, 3000},
    {DATA_NPC_CRY,          0,                      3000},
    {DATA_NPC_EXECUTION,    0,                      1000},
    {SAY_EXEC_WAITING,      NPC_PRISONER_BLOOD_ELF, 0},

    {0, 0, 0},
};

struct npc_a_special_surpriseAI : public ScriptedAI, private DialogueHelper
{
    npc_a_special_surpriseAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aSurpriseDialogue)
    {
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    uint32 m_uiCryTimer;
    ObjectGuid m_playerGuid;

    void Reset() override
    {
        m_uiCryTimer = urand(15000, 30000);
        m_playerGuid.Clear();
    }

    // Helper function to check if quest is available
    bool MeetQuestCondition(Player* pPlayer) const
    {
        switch (m_creature->GetEntry())
        {
            case NPC_PRISONER_HUMAN:     return pPlayer->GetQuestStatus(12742) == QUEST_STATUS_INCOMPLETE;       // Ellen Stanbridge
            case NPC_PRISONER_ORC:       return pPlayer->GetQuestStatus(12748) == QUEST_STATUS_INCOMPLETE;       // Kug Ironjaw
            case NPC_PRISONER_DWARF:     return pPlayer->GetQuestStatus(12744) == QUEST_STATUS_INCOMPLETE;       // Donovan Pulfrost
            case NPC_PRISONER_NIGHT_ELF: return pPlayer->GetQuestStatus(12743) == QUEST_STATUS_INCOMPLETE;       // Yazmina Oakenthorn
            case NPC_PRISONER_UNDEAD:    return pPlayer->GetQuestStatus(12750) == QUEST_STATUS_INCOMPLETE;       // Antoine Brack
            case NPC_PRISONER_TAUREN:    return pPlayer->GetQuestStatus(12739) == QUEST_STATUS_INCOMPLETE;       // Malar Bravehorn
            case NPC_PRISONER_GNOME:     return pPlayer->GetQuestStatus(12745) == QUEST_STATUS_INCOMPLETE;       // Goby Blastenheimer
            case NPC_PRISONER_TROLL:     return pPlayer->GetQuestStatus(12749) == QUEST_STATUS_INCOMPLETE;       // Iggy Darktusk
            case NPC_PRISONER_BLOOD_ELF: return pPlayer->GetQuestStatus(12747) == QUEST_STATUS_INCOMPLETE;       // Lady Eonys
            case NPC_PRISONER_DRAENEI:   return pPlayer->GetQuestStatus(12746) == QUEST_STATUS_INCOMPLETE;       // Valok the Righteous
        }

        return false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (m_playerGuid || !who->IsPlayer() || !who->IsWithinDist(m_creature, INTERACTION_DISTANCE))
            return;

        // start dialogue on approach
        if (MeetQuestCondition(static_cast<Player*>(who)))
        {
            StartNextDialogueText(m_creature->GetEntry());
            m_playerGuid = who->GetObjectGuid();
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case DATA_NPC_STAND:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
            case DATA_NPC_TALK:
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case DATA_NPC_EMOTE:
                m_creature->HandleEmote(EMOTE_ONESHOT_NO);
                break;
            case DATA_NPC_EXCLAMATION:
                m_creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                break;
            case DATA_NPC_CRY:
                m_creature->HandleEmote(EMOTE_ONESHOT_CRY);
                break;
            case DATA_NPC_EXECUTION:
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_creature->SetImmuneToPlayer(false);
                break;
        }
    }

    Creature* GetSpeakerByEntry(uint32 uiEntry) override
    {
        switch (uiEntry)
        {
            case NPC_PRISONER_TAUREN:
            case NPC_PRISONER_HUMAN:
            case NPC_PRISONER_NIGHT_ELF:
            case NPC_PRISONER_DWARF:
            case NPC_PRISONER_GNOME:
            case NPC_PRISONER_DRAENEI:
            case NPC_PRISONER_UNDEAD:
            case NPC_PRISONER_ORC:
            case NPC_PRISONER_TROLL:
            case NPC_PRISONER_BLOOD_ELF:
                return m_creature;
            case NPC_PLAGUEFIST:
                return GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f);
        }

        return nullptr;
    }

    Unit* GetDialogueTarget() override
    {
        return m_creature->GetMap()->GetPlayer(m_playerGuid);
    }

    void JustDied(Unit* killer) override
    {
        DoScriptText(EMOTE_DIES, m_creature);

        // despawn in 2 seconds; respawn after 30 seconds
        m_creature->ForcedDespawn(2000);
        m_creature->SetRespawnDelay(30);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (m_uiCryTimer)
        {
            if (m_uiCryTimer <= uiDiff)
            {
                m_creature->HandleEmote(EMOTE_ONESHOT_CRY);
                m_uiCryTimer = urand(15000, 30000);
            }
            else
                m_uiCryTimer -= uiDiff;
        }
    }
};

/*######
## npc_death_knight_initiate
######*/

enum
{
    SAY_DUEL_A                  = -1609016,
    SAY_DUEL_B                  = -1609017,
    SAY_DUEL_C                  = -1609018,
    SAY_DUEL_D                  = -1609019,
    SAY_DUEL_E                  = -1609020,
    SAY_DUEL_F                  = -1609021,
    SAY_DUEL_G                  = -1609022,
    SAY_DUEL_H                  = -1609023,
    SAY_DUEL_I                  = -1609024,

    EMOTE_DUEL_BEGIN            = -1001137,
    EMOTE_DUEL_BEGIN_3          = -1001138,
    EMOTE_DUEL_BEGIN_2          = -1001139,
    EMOTE_DUEL_BEGIN_1          = -1001140,

    GOSSIP_ITEM_ACCEPT_DUEL     = -3609000,
    GOSSIP_TEXT_ID_DUEL         = 13433,

    SPELL_DUEL                  = 52996,
    SPELL_DUEL_TRIGGERED        = 52990,
    SPELL_DUEL_VICTORY          = 52994,
    SPELL_DUEL_FLAG             = 52991,

    // generic DK spells. used in many scripts here
    SPELL_BLOOD_STRIKE          = 52374,
    SPELL_DEATH_COIL            = 52375,
    SPELL_ICY_TOUCH             = 52372,
    SPELL_PLAGUE_STRIKE         = 52373,

    GO_DUEL_FLAG                = 191126,

    NPC_DEATH_KNIGHT_INITIATE   = 28406,

    QUEST_DEATH_CHALLENGE       = 12733,
    FACTION_HOSTILE             = 2068
};

static const DialogueEntry aInitiateDialogue[] =
{
    {GO_DUEL_FLAG,              0,                         5000},
    {EMOTE_DUEL_BEGIN,          NPC_DEATH_KNIGHT_INITIATE, 1000},
    {EMOTE_DUEL_BEGIN_3,        NPC_DEATH_KNIGHT_INITIATE, 1000},
    {EMOTE_DUEL_BEGIN_2,        NPC_DEATH_KNIGHT_INITIATE, 1000},
    {EMOTE_DUEL_BEGIN_1,        NPC_DEATH_KNIGHT_INITIATE, 1000},
    {NPC_DEATH_KNIGHT_INITIATE, 0,                         0},
    {0, 0, 0},
};

int32 m_auiRandomSay[] =
{
    SAY_DUEL_A, SAY_DUEL_B, SAY_DUEL_C, SAY_DUEL_D, SAY_DUEL_E, SAY_DUEL_F, SAY_DUEL_G, SAY_DUEL_H, SAY_DUEL_I
};

enum InitiateActions
{
    INITIATE_BLOOD_STRIKE,
    INITIATE_DEATH_COIL,
    INITIATE_ICY_TOUCH,
    INITIATE_PLAGUE_STRIKE,
    INITIATE_ACTION_MAX,
};

struct npc_death_knight_initiateAI : public CombatAI, private DialogueHelper
{
    npc_death_knight_initiateAI(Creature* creature) : CombatAI(creature, INITIATE_ACTION_MAX),
        DialogueHelper(aInitiateDialogue)
    {
        AddCombatAction(INITIATE_BLOOD_STRIKE, 4000u);
        AddCombatAction(INITIATE_DEATH_COIL, 6000u);
        AddCombatAction(INITIATE_ICY_TOUCH, 2000u);
        AddCombatAction(INITIATE_PLAGUE_STRIKE, 5000u);

        Reset();
    }

    ObjectGuid m_duelerGuid;

    bool m_bIsDuelComplete;

    void Reset() override
    {
        m_creature->SetImmuneToPlayer(true);
        m_duelerGuid.Clear();

        m_bIsDuelComplete = false;

        CombatAI::Reset();
    }

    void JustReachedHome() override
    {
        // reset encounter
        if (GameObject* pFlag = GetClosestGameObjectWithEntry(m_creature, GO_DUEL_FLAG, 30.0f))
            pFlag->SetLootState(GO_JUST_DEACTIVATED);

        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // start duel
        if (eventType == AI_EVENT_START_EVENT && pInvoker->IsPlayer())
        {
            StartNextDialogueText(GO_DUEL_FLAG);
            m_duelerGuid = pInvoker->GetObjectGuid();
        }
    }

    void EnterEvadeMode() override
    {
        // evade only when duel isn't complete
        if (!m_bIsDuelComplete)
            CombatAI::EnterEvadeMode();
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        // no damage check unless in duel with a player
        if (m_duelerGuid.IsEmpty())
            return;

        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (!m_bIsDuelComplete)
            {
                // complete duel and evade (without home movemnet)
                m_bIsDuelComplete = true;
                ClearCombatOnlyRoot();
                m_creature->RemoveAllAurasOnEvade();
                m_creature->CombatStopWithPets(true);
                m_creature->SetLootRecipient(nullptr);

                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_duelerGuid))
                {
                    DoCastSpellIfCan(pPlayer, SPELL_DUEL_VICTORY, CAST_TRIGGERED);
                    m_creature->SetFacingToObject(pPlayer);
                }

                // remove duel flag
                if (GameObject* pFlag = GetClosestGameObjectWithEntry(m_creature, GO_DUEL_FLAG, 30.0f))
                    pFlag->SetLootState(GO_JUST_DEACTIVATED);

                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_BEG);
                m_creature->ForcedDespawn(10000);
            }
        }
    }

    Creature* GetSpeakerByEntry(uint32 entry) override
    {
        if (entry == NPC_DEATH_KNIGHT_INITIATE)
            return m_creature;

        return nullptr;
    }

    Unit* GetDialogueTarget() override
    {
        return m_creature->GetMap()->GetPlayer(m_duelerGuid);
    }

    void JustDidDialogueStep(int32 entry) override
    {
        if (entry == NPC_DEATH_KNIGHT_INITIATE)
        {
            m_creature->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_COMBAT_STOP | TEMPFACTION_RESTORE_RESPAWN);
            m_creature->SetImmuneToPlayer(false);

            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_duelerGuid))
                AttackStart(pPlayer);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case INITIATE_BLOOD_STRIKE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_BLOOD_STRIKE) == CAST_OK)
                    ResetCombatAction(action, 9000);
                break;
            case INITIATE_DEATH_COIL:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DEATH_COIL) == CAST_OK)
                    ResetCombatAction(action, 8000);
                break;
            case INITIATE_ICY_TOUCH:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ICY_TOUCH) == CAST_OK)
                    ResetCombatAction(action, 8000);
                break;
            case INITIATE_PLAGUE_STRIKE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_PLAGUE_STRIKE) == CAST_OK)
                    ResetCombatAction(action, 8000);
                break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        DialogueUpdate(diff);

        CombatAI::UpdateAI(diff);
    }
};

bool GossipHello_npc_death_knight_initiate(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_DEATH_CHALLENGE) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ACCEPT_DUEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_DUEL, pCreature->GetObjectGuid());
        return true;
    }
    return false;
}

bool GossipSelect_npc_death_knight_initiate(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        pCreature->SetFacingToObject(pPlayer);

        DoScriptText(m_auiRandomSay[urand(0, countof(m_auiRandomSay) - 1)], pCreature, pPlayer);

        pCreature->CastSpell(pPlayer, SPELL_DUEL, TRIGGERED_OLD_TRIGGERED);
        pCreature->CastSpell(pPlayer, SPELL_DUEL_FLAG, TRIGGERED_OLD_TRIGGERED);
    }
    return true;
}

bool EffectDummyCreature_npc_death_knight_initiate(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_DUEL_TRIGGERED && uiEffIndex == EFFECT_INDEX_0)
    {
        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_START_EVENT, pCaster, pCreatureTarget);
        return true;
    }

    return false;
}

/*######
## npc_eye_of_acherus
######*/

enum
{
    SPELL_EYE_CONTROL       = 51852,                        // apply phase aura: 2; summon creature 28511 and apply player control aura
    SPELL_EYE_VISUAL        = 51892,                        // apply visual aura
    SPELL_EYE_FLIGHT        = 51890,                        // apply fly aura and change mounted speed; cast by the eye of acherus
    SPELL_EYE_FLIGHT_BOOST  = 51923,                        // apply fly aura and increase speed aura
    SPELL_RECALL_EYE        = 52694,

    EMOTE_DESTIANTION       = -1609089,
    EMOTE_CONTROL           = -1609090,

    POINT_EYE_START_POS     = 0,
    POINT_EYE_DESTINATION   = 1,

    START_POINT_PAUSE_TIME  = 5000
};

// movement destination coords
static const float aEyeDestination[3] = { 1758.007f, -5876.785f, 166.8667f };
static const float aEyeStartPos[3] = { 2361.21f, -5660.45f, 503.8283f };

struct npc_eye_of_acherusAI : public ScriptedAI
{
    npc_eye_of_acherusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->SetDisplayId(26320);                // HACK remove when correct modelid will be taken by core

        m_isFinished = false;
        m_reachPoint = false;
        m_flightTimer = START_POINT_PAUSE_TIME;
        m_phase = 0;
        Reset();
    }

    bool m_isFinished;
    bool m_reachPoint;
    uint32 m_flightTimer;
    uint32 m_phase;

    void Reset() override {}

    void JustDied(Unit* /*pKiller*/) override
    {
        // recall the eye when it dies - need to remove phase and control aura from player
        DoCastSpellIfCan(m_creature, SPELL_RECALL_EYE, CAST_TRIGGERED);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (m_isFinished || m_reachPoint || uiType != POINT_MOTION_TYPE)
            return;

        switch (uiPointId)
        {
            case POINT_EYE_START_POS:
            case POINT_EYE_DESTINATION:
                m_reachPoint = true;
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_isFinished)
            return;

        switch (m_phase)
        {
            case 0:         // initialization and move to start position
            {
                if (m_creature->GetBeneficiaryPlayer())
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_EYE_VISUAL) == CAST_OK)
                        m_creature->GetMotionMaster()->MovePoint(POINT_EYE_DESTINATION, aEyeStartPos[0], aEyeStartPos[1], aEyeStartPos[2]);
                }
                else
                {
                    // Failsafe: creature isn't controlled -> despawn
                    m_creature->ForcedDespawn();
                    m_isFinished = true;
                }

                ++m_phase;
                break;
            }
            case 1:         // wait for start position reached, then wait 5 sec before the journey to the end point
            {
                if (!m_reachPoint)
                    return;

                if (m_flightTimer < uiDiff)
                {
                    // Apply flight aura and start moving to position
                    if (DoCastSpellIfCan(m_creature, SPELL_EYE_FLIGHT_BOOST) == CAST_OK)
                    {
                        if (Player* player = m_creature->GetBeneficiaryPlayer())
                            DoScriptText(EMOTE_DESTIANTION, m_creature, player);

                        // move to the destination position
                        Position tgtPos = Position(aEyeDestination[0], aEyeDestination[1], aEyeDestination[2], 0);
                        m_creature->GetMotionMaster()->MovePoint(POINT_EYE_DESTINATION, tgtPos, FORCED_MOVEMENT_NONE, m_creature->GetSpawner()->GetSpeed(MOVE_RUN));

                        m_reachPoint = false;
                        ++m_phase;
                    }
                }
                else
                    m_flightTimer -= uiDiff;

                break;
            }
            case 2:         // wait to reach end point then set fly mode by applying SPELL_EYE_FLIGHT
            {
                if (!m_reachPoint)
                    return;

                if (DoCastSpellIfCan(m_creature, SPELL_EYE_FLIGHT) == CAST_OK)
                {
                    if (Player* pPlayer = m_creature->GetBeneficiaryPlayer())
                        DoScriptText(EMOTE_CONTROL, m_creature, pPlayer);

                    m_isFinished = true;
                    ++m_phase;
                }
                break;
            }
            default:
                m_isFinished = true;
                break;
        }
    }
};

/*######
## npc_scarlet_ghoul
######*/

enum
{
    SAY_GHUL_SPAWN_1            = -1609091,
    SAY_GHUL_SPAWN_2            = -1609092,
    SAY_GHUL_SPAWN_3            = -1609093,
    SAY_GHUL_SPAWN_4            = -1609094,
    SAY_GHUL_SPAWN_5            = -1609095,
    SAY_GOTHIK_THROW_IN_PIT_1   = -1609096,
    SAY_GOTHIK_THROW_IN_PIT_2   = -1609097,

    SPELL_GHOUL_SUMMONED        = 52500,                    // dummy aura applied on owner player
    SPELL_GOTHIK_GHOUL_PING     = 52514,                    // aoe ping cast by Gothik using spell 52513; targets creature 28845
    SPELL_GHOUL_CREDIT          = 52517,                    // kill credit id: 28845
    SPELL_GHOULPLOSION          = 52519,                    // cast by Gothik on the Scarlet Ghoul before destroying the creature; triggers 52555 from ghoul target to ghoul master

    NPC_GOTHIK                  = 28658,
};

static const float aPitPosition[3] = { 2369.276f, -5778.689f, 151.367f};

struct npc_scarlet_ghoulAI : public ScriptedPetAI
{
    npc_scarlet_ghoulAI(Creature* pCreature) : ScriptedPetAI(pCreature)
    {
        // apply aura on the player master
        if (DoCastSpellIfCan(m_creature, SPELL_GHOUL_SUMMONED) == CAST_OK)
        {
            m_bGotHit       = false;
            m_bIsJumping    = false;
            m_bDidInitText  = false;

            SetReactState(REACT_DEFENSIVE);
        }
        Reset();
    }

    ObjectGuid m_gothikGuid;

    bool m_bGotHit;
    bool m_bIsJumping;
    bool m_bDidInitText;

    void Reset() override {}

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType == EFFECT_MOTION_TYPE && uiPointId == 1)
        {
            // make Gothik despawn the ghoul; notification to player follows in spell chain
            if (Creature* pGothik = m_creature->GetMap()->GetCreature(m_gothikGuid))
                pGothik->CastSpell(m_creature, SPELL_GHOULPLOSION, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            // first spell hit - apply kill credit
            if (!m_bGotHit)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_GHOUL_CREDIT) == CAST_OK)
                    m_bGotHit = true;
            }
            // following spell hit - jump and despawn
            else
            {
                m_gothikGuid = pInvoker->GetObjectGuid();

                // make caster yell if possible
                world_map_ebon_hold* pInstance = static_cast<world_map_ebon_hold*>(m_creature->GetInstanceData());
                if (pInstance && pInstance->CanAndToggleGothikYell())
                    DoScriptText(urand(0, 1) ? SAY_GOTHIK_THROW_IN_PIT_1 : SAY_GOTHIK_THROW_IN_PIT_2, pInvoker);

                // jump to the pit
                float fX, fY, fZ;
                m_creature->GetRandomPoint(aPitPosition[0], aPitPosition[1], aPitPosition[2], 10.0f, fX, fY, fZ);
                m_creature->GetMotionMaster()->MoveJump(fX, fY, fZ, 24.21229f, 6.0f, 1);

                m_bIsJumping = true;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_bDidInitText)
        {
            Unit* pOwner = m_creature->GetMaster();
            DoScriptText(SAY_GHUL_SPAWN_1 - urand(0, 4), m_creature, pOwner);

            m_bDidInitText = true;
        }

        if (m_bIsJumping)
            return;

        ScriptedPetAI::UpdateAI(uiDiff);
    }
};

bool EffectDummyCreature_npc_scarlet_ghoul(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_GOTHIK_GHOUL_PING && uiEffIndex == EFFECT_INDEX_0)
    {
        // inform creature AI that was hit by spell
        pCaster->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);
        return true;
    }

    return false;
}

/*######
## npc_highlord_darion_mograine
######*/

enum LightOfDawn
{
    // yells
    SAY_LIGHT_OF_DAWN_INTRO_1           = -1609201,         // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN_INTRO_2           = -1609202,

    SAY_LIGHT_OF_DAWN_PREPARE_1         = -1609203,
    SAY_LIGHT_OF_DAWN_PREPARE_2         = -1609204,
    SAY_LIGHT_OF_DAWN_PREPARE_3         = -1609205,
    SAY_LIGHT_OF_DAWN_PREPARE_4         = -1609206,

    SAY_LIGHT_OF_DAWN_STAND_1           = -1609207,         // Korfax
    SAY_LIGHT_OF_DAWN_STAND_2           = -1609208,         // Lord Maxwell Tyrosus

    SAY_LIGHT_OF_DAWN_BATTLE_1          = -1609209,         // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN_BATTLE_2          = -1609210,
    SAY_LIGHT_OF_DAWN_BATTLE_3          = -1609211,
    SAY_LIGHT_OF_DAWN_BATTLE_4          = -1609212,
    SAY_LIGHT_OF_DAWN_BATTLE_5          = -1609213,
    SAY_LIGHT_OF_DAWN_BATTLE_6          = -1609214,
    SAY_LIGHT_OF_DAWN_BATTLE_7          = -1609215,
    SAY_LIGHT_OF_DAWN_BATTLE_8          = -1609216,
    SAY_LIGHT_OF_DAWN_BATTLE_9          = -1609224,

    SAY_LIGHT_OF_DAWN_BATTLE_10         = -1609217,         // Battle end yells
    SAY_LIGHT_OF_DAWN_BATTLE_11         = -1609218,
    SAY_LIGHT_OF_DAWN_BATTLE_12         = -1609219,
    SAY_LIGHT_OF_DAWN_BATTLE_13         = -1609220,
    SAY_LIGHT_OF_DAWN_BATTLE_14         = -1609221,
    SAY_LIGHT_OF_DAWN_BATTLE_15         = -1609222,
    SAY_LIGHT_OF_DAWN_BATTLE_16         = -1609223,

    SAY_LIGHT_OF_DAWN_OUTRO_1           = -1609225,         // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN_OUTRO_2           = -1609226,
    SAY_LIGHT_OF_DAWN_OUTRO_3           = -1609227,         // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN_OUTRO_4           = -1609228,         // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN_OUTRO_5           = -1609229,
    SAY_LIGHT_OF_DAWN_OUTRO_6           = -1609230,
    SAY_LIGHT_OF_DAWN_OUTRO_7           = -1609231,         // Highlord Darion Mograine

    SAY_LIGHT_OF_DAWN_VISION_1          = -1609232,         // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN_VISION_2          = -1609233,         // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN_VISION_3          = -1609234,
    SAY_LIGHT_OF_DAWN_VISION_4          = -1609235,         // Darion Mograine
    SAY_LIGHT_OF_DAWN_VISION_5          = -1609236,
    SAY_LIGHT_OF_DAWN_VISION_6          = -1609237,         // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN_VISION_7          = -1609238,         // Darion Mograine
    SAY_LIGHT_OF_DAWN_VISION_8          = -1609239,         // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN_VISION_9          = -1609240,         // Darion Mograine
    SAY_LIGHT_OF_DAWN_VISION_10         = -1609241,         // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN_VISION_11         = -1609242,

    SAY_LIGHT_OF_DAWN_KING_VISIT_1      = -1609243,         // The Lich King
    SAY_LIGHT_OF_DAWN_KING_VISIT_2      = -1609245,
    SAY_LIGHT_OF_DAWN_KING_VISIT_3      = -1609244,         // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN_KING_VISIT_4      = -1609246,         // The Lich King
    SAY_LIGHT_OF_DAWN_KING_VISIT_5      = -1609247,         // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN_KING_VISIT_6      = -1609248,         // The Lich King
    SAY_LIGHT_OF_DAWN_KING_VISIT_7      = -1609249,
    SAY_LIGHT_OF_DAWN_KING_VISIT_8      = -1609250,         // Lord Maxwell Tyrosus
    SAY_LIGHT_OF_DAWN_KING_VISIT_9      = -1609251,         // The Lich King
    SAY_LIGHT_OF_DAWN_KING_VISIT_10     = -1609252,         // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN_KING_VISIT_11     = -1609253,
    SAY_LIGHT_OF_DAWN_KING_VISIT_12     = -1609254,         // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN_KING_VISIT_13     = -1609255,         // The Lich King
    SAY_LIGHT_OF_DAWN_KING_VISIT_14     = -1609256,         // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN_KING_VISIT_15     = -1609257,         // The Lich King
    SAY_LIGHT_OF_DAWN_KING_VISIT_16     = -1609258,
    SAY_LIGHT_OF_DAWN_KING_VISIT_17     = -1609259,

    SAY_LIGHT_OF_DAWN_EPILOGUE_1        = -1609260,         // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN_EPILOGUE_2        = -1609261,
    SAY_LIGHT_OF_DAWN_EPILOGUE_3        = -1609262,
    SAY_LIGHT_OF_DAWN_EPILOGUE_4        = -1609263,
    SAY_LIGHT_OF_DAWN_EPILOGUE_5        = -1609264,
    SAY_LIGHT_OF_DAWN_EPILOGUE_6        = -1609265,
    SAY_LIGHT_OF_DAWN_EPILOGUE_7        = -1609266,
    SAY_LIGHT_OF_DAWN_EPILOGUE_8        = -1609267,
    SAY_LIGHT_OF_DAWN_EPILOGUE_9        = -1609268,         // Highlord Darion Mograine

    // Emotes - Note: some of the emote texts are not used in 3.x
    // EMOTE_LIGHT_OF_DAWN_ARMY_RISE    = -1609269,         // Emotes
    // EMOTE_LIGHT_OF_DAWN_ARMY_MARCH   = -1609270,
    // EMOTE_LIGHT_OF_DAWN_TIRION       = -1609271,
    EMOTE_LIGHT_OF_DAWN_FLEE            = -1609272,
    EMOTE_LIGHT_OF_DAWN_KNEEL           = -1609273,
    // EMOTE_LIGHT_OF_DAWN_ALEXANDROS   = -1609274,
    // EMOTE_LIGHT_OF_DAWN_SHADE        = -1609275,
    EMOTE_LIGHT_OF_DAWN_HUG             = -1609276,
    // EMOTE_LIGHT_OF_DAWN_LICH_KING    = -1609277,
    // EMOTE_LIGHT_OF_DAWN_ANGRY        = -1609278,
    // EMOTE_LIGHT_OF_DAWN_CAST_SPELL   = -1609279,
    EMOTE_LIGHT_OF_DAWN_GRASP           = -1609280,
    // EMOTE_LIGHT_OF_DAWN_POWERFULL    = -1609281,
    // EMOTE_LIGHT_OF_DAWN_ASHBRINGER   = -1609282,
    EMOTE_LIGHT_OF_DAWN_COLAPSE         = -1609283,
    // EMOTE_LIGHT_OF_DAWN_CHARGE       = -1609284,
    // EMOTE_LIGHT_OF_DAWN_KING_LEAVE   = -1609285,
    // EMOTE_LIGHT_OF_DAWN_LIGHT        = -1609286,

    // sound ids
    SOUND_ID_LK_LAUGH                   = 14820,
    SOUND_ID_TIRION_ROAR                = 12300,

    // Spells
    // Highlord Darion Mograine combat spalls
    SPELL_HERO_AGGRO_AURA               = 53627,
    SPELL_SCOURGE_AGGRO_AURA            = 53624,
    SPELL_ANTI_MAGIC_ZONE_DARION        = 52893,
    SPELL_DEATH_STRIKE                  = 53639,
    SPELL_DEATH_EMBRACE                 = 53635,
    SPELL_ICY_TOUCH_DARION              = 49723,
    SPELL_PLAGUE_STRIKE_KNIGHTS         = 50688,
    SPELL_THE_MIGHT_OF_MOGRAINE         = 53642,            // on players when battle begins
    SPELL_UNHOLY_BLIGHT                 = 53640,

    // SPELL_BIRTH                      = 53603,            // ground shake - handled in EAI
    SPELL_THE_LIGHT_OF_DAWN_DUMMY       = 53658,            // light globe
    SPELL_THE_LIGHT_OF_DAWN_DAMAGE_LOSS = 53645,            // cast by the scourge units
    // SPELL_ALEXANDROS_MOGRAINE_SPAWN  = 53667,            // spawn effect for Alexandros
    SPELL_MOGRAINE_CHARGE               = 53679,            // charge to the Lich King
    SPELL_ASHBRINGER                    = 53701,            // throw Ashbringer to Tirion
    SPELL_THE_LIGHT_OF_DAWN_CREDIT      = 53606,            // quest credit

    // Lich King spells
    SPELL_APOCALYPSE                    = 53210,            // knocks back all enemies
    SPELL_TELEPORT_VISUAL               = 52233,            // on leave
    SPELL_SOUL_FEAST_ALEX               = 53677,            // on Alexandros
    SPELL_SOUL_FEAST_TIRION             = 53685,            // on Tirion
    // SPELL_ICEBOUND_VISAGE            = 53274,            // ice effect - handled in EAI
    SPELL_REBUKE                        = 53680,            // knockback

    // Highlord Tirion Fordring
    EQUIP_HIGHLORD_TIRION_FORDRING      = 13262,
    SPELL_LAY_ON_HANDS                  = 53778,            // heal effect
    SPELL_REBIRTH_OF_THE_ASHBRINGER     = 53702,            // globe sphere
    SPELL_TIRION_CHARGE                 = 53705,            // on the lich king

    // others
    QUEST_ID_LIGHT_OF_DAWN              = 12801,

    GOSSIP_ITEM_READY                   = -3609001,
    GOSSIP_TEXT_ID_READY                = 13485,

    DATA_PREPARE_DIALOGUE               = 1,
    DATA_SUMMON_CHAMPIONS               = 2,
    DATA_SUMMON_DEFENDERS               = 3,

    DATA_SUMMON_ARMY_1                  = 4,
    DATA_SUMMON_ARMY_2                  = 5,
    DATA_SUMMON_ARMY_3                  = 6,
    DATA_SUMMON_ARMY_4                  = 7,
    DATA_UNDEAD_ARMY_MARCH              = 8,

    DATA_LICH_KING_POINT                = 9,
    DATA_LICH_KING_MOVE                 = 10,
    DATA_LICH_KING_LAUGH                = 11,
    DATA_RESET_CHAMPIONS                = 12,
    DATA_LICH_KING_FACE_TIRION          = 13,
    DATA_LICH_KING_DEFEAT               = 14,
    DATA_TIRION_CONCLUSION              = 15,
    DATA_DARION_EXCLAMATION             = 16,
};

static const DialogueEntry aDarionDialogue[] =
{
    // preparation yells
    {DATA_PREPARE_DIALOGUE,             0,                                  40000},
    {DATA_SUMMON_CHAMPIONS,             0,                                  5000},
    {DATA_SUMMON_DEFENDERS,             0,                                  70000},
    {SAY_LIGHT_OF_DAWN_INTRO_1,         NPC_HIGHLORD_DARION_MOGRAINE,       60000},
    {SAY_LIGHT_OF_DAWN_INTRO_2,         NPC_HIGHLORD_DARION_MOGRAINE,       0},

    // before battle yells
    {SAY_LIGHT_OF_DAWN_PREPARE_1,       NPC_HIGHLORD_DARION_MOGRAINE,       5000},
    {SAY_LIGHT_OF_DAWN_PREPARE_2,       NPC_HIGHLORD_DARION_MOGRAINE,       8000},
    {SAY_LIGHT_OF_DAWN_PREPARE_3,       NPC_HIGHLORD_DARION_MOGRAINE,       6000},
    {DATA_SUMMON_ARMY_1,                0,                                  5000},
    {DATA_SUMMON_ARMY_2,                0,                                  6000},
    {DATA_SUMMON_ARMY_3,                0,                                  5000},
    {DATA_SUMMON_ARMY_4,                0,                                  15000},
    {SAY_LIGHT_OF_DAWN_PREPARE_4,       NPC_HIGHLORD_DARION_MOGRAINE,       5000},
    {DATA_UNDEAD_ARMY_MARCH,            0,                                  5000},
    {SAY_LIGHT_OF_DAWN_STAND_1,         NPC_KORFAX_CHAMPION_OF_THE_LIGHT,   3000},
    {SAY_LIGHT_OF_DAWN_STAND_2,         NPC_LORD_MAXWELL_TYROSUS,           0},

    // after battle event
    {SAY_LIGHT_OF_DAWN_OUTRO_3,         NPC_HIGHLORD_DARION_MOGRAINE,       19000},
    {SAY_LIGHT_OF_DAWN_OUTRO_4,         NPC_HIGHLORD_TIRION_FORDRING,       21000},
    {SAY_LIGHT_OF_DAWN_OUTRO_5,         NPC_HIGHLORD_TIRION_FORDRING,       13000},
    {SAY_LIGHT_OF_DAWN_OUTRO_6,         NPC_HIGHLORD_TIRION_FORDRING,       13000},
    {SAY_LIGHT_OF_DAWN_OUTRO_7,         NPC_HIGHLORD_DARION_MOGRAINE,       6000},
    {NPC_HIGHLORD_ALEXANDROS_MOGRAINE,  0,                                  3000},
    {SAY_LIGHT_OF_DAWN_VISION_1,        NPC_HIGHLORD_ALEXANDROS_MOGRAINE,   1500},
    {SAY_LIGHT_OF_DAWN_VISION_2,        NPC_HIGHLORD_DARION_MOGRAINE,       3000},
    {SAY_LIGHT_OF_DAWN_VISION_3,        NPC_HIGHLORD_DARION_MOGRAINE,       2000},
    {SAY_LIGHT_OF_DAWN_VISION_4,        NPC_DARION_MOGRAINE,                5000},
    {EMOTE_LIGHT_OF_DAWN_HUG,           NPC_DARION_MOGRAINE,                4000},
    {SAY_LIGHT_OF_DAWN_VISION_5,        NPC_DARION_MOGRAINE,                5000},
    {SAY_LIGHT_OF_DAWN_VISION_6,        NPC_HIGHLORD_ALEXANDROS_MOGRAINE,   8000},
    {SAY_LIGHT_OF_DAWN_VISION_7,        NPC_DARION_MOGRAINE,                8000},
    {SAY_LIGHT_OF_DAWN_VISION_8,        NPC_HIGHLORD_ALEXANDROS_MOGRAINE,   15000},
    {SAY_LIGHT_OF_DAWN_VISION_9,        NPC_DARION_MOGRAINE,                11000},
    {SAY_LIGHT_OF_DAWN_VISION_10,       NPC_HIGHLORD_ALEXANDROS_MOGRAINE,   29000},
    {SAY_LIGHT_OF_DAWN_VISION_11,       NPC_HIGHLORD_ALEXANDROS_MOGRAINE,   4000},
    {NPC_THE_LICH_KING,                 0,                                  2000},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_2,    NPC_THE_LICH_KING,                  6000},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_3,    NPC_HIGHLORD_DARION_MOGRAINE,       2000},
    {DATA_LICH_KING_MOVE,               0,                                  1000},
    {DATA_LICH_KING_POINT,              0,                                  5000},
    {SPELL_MOGRAINE_CHARGE,             0,                                  1500},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_4,    NPC_THE_LICH_KING,                  4000},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_5,    NPC_HIGHLORD_TIRION_FORDRING,       5000},
    {DATA_LICH_KING_LAUGH,              0,                                  3000},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_6,    NPC_THE_LICH_KING,                  15000},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_7,    NPC_THE_LICH_KING,                  17000},
    {EMOTE_LIGHT_OF_DAWN_GRASP,         NPC_HIGHLORD_TIRION_FORDRING,       1000},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_8,    NPC_LORD_MAXWELL_TYROSUS,           500},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_9,    NPC_THE_LICH_KING,                  3000},
    {DATA_RESET_CHAMPIONS,              0,                                  7000},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_10,   NPC_HIGHLORD_DARION_MOGRAINE,       3000},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_11,   NPC_HIGHLORD_DARION_MOGRAINE,       1000},
    {SPELL_ASHBRINGER,                  0,                                  5000},
    {SPELL_REBIRTH_OF_THE_ASHBRINGER,   0,                                  1000},
    {EMOTE_LIGHT_OF_DAWN_COLAPSE,       NPC_HIGHLORD_DARION_MOGRAINE,       2000},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_12,   NPC_HIGHLORD_TIRION_FORDRING,       4000},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_13,   NPC_THE_LICH_KING,                  3000},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_14,   NPC_HIGHLORD_TIRION_FORDRING,       2000},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_15,   NPC_THE_LICH_KING,                  4000},
    {DATA_LICH_KING_FACE_TIRION,        0,                                  1000},
    {DATA_LICH_KING_DEFEAT,             0,                                  2000},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_16,   NPC_THE_LICH_KING,                  9000},
    {SAY_LIGHT_OF_DAWN_KING_VISIT_17,   NPC_THE_LICH_KING,                  10000},
    {SPELL_TELEPORT_VISUAL,             0,                                  10000},
    {SPELL_LAY_ON_HANDS,                0,                                  5000},
    {SAY_LIGHT_OF_DAWN_EPILOGUE_1,      NPC_HIGHLORD_TIRION_FORDRING,       4000},
    {DATA_TIRION_CONCLUSION,            0,                                  8000},
    {SAY_LIGHT_OF_DAWN_EPILOGUE_2,      NPC_HIGHLORD_TIRION_FORDRING,       15000},
    {SAY_LIGHT_OF_DAWN_EPILOGUE_3,      NPC_HIGHLORD_TIRION_FORDRING,       7000},
    {SAY_LIGHT_OF_DAWN_EPILOGUE_4,      NPC_HIGHLORD_TIRION_FORDRING,       10000},
    {SAY_LIGHT_OF_DAWN_EPILOGUE_5,      NPC_HIGHLORD_TIRION_FORDRING,       11000},
    {SAY_LIGHT_OF_DAWN_EPILOGUE_6,      NPC_HIGHLORD_TIRION_FORDRING,       10000},
    {SAY_LIGHT_OF_DAWN_EPILOGUE_7,      NPC_HIGHLORD_TIRION_FORDRING,       8000},
    {SAY_LIGHT_OF_DAWN_EPILOGUE_8,      NPC_HIGHLORD_TIRION_FORDRING,       10000},
    {SAY_LIGHT_OF_DAWN_EPILOGUE_9,      NPC_HIGHLORD_DARION_MOGRAINE,       6000},
    {DATA_DARION_EXCLAMATION,           0,                                  4000},
    {QUEST_ID_LIGHT_OF_DAWN,            0,                                  0},

    {0, 0, 0},
};

enum DarionActions
{
    DARION_ANTI_MAGIC_ZONE,
    DARION_DEATH_STRIKE,
    DARION_DEATH_EMBRACE,
    DARION_ICY_TOUCH,
    DARION_UNHOLY_BLIGHT,
    DARION_COMBAT_YELL,
    DARION_DEFEAT_YELL,
    DARION_ACTION_MAX,
};

struct npc_highlord_darion_mograineAI : public CombatAI, private DialogueHelper
{
    npc_highlord_darion_mograineAI(Creature* creature) : CombatAI(creature, DARION_ACTION_MAX), m_instance(static_cast<world_map_ebon_hold*>(creature->GetInstanceData())),
        DialogueHelper(aDarionDialogue)
    {
        AddCombatAction(DARION_ANTI_MAGIC_ZONE, 1000u);
        AddCombatAction(DARION_DEATH_STRIKE, 1000u);
        AddCombatAction(DARION_DEATH_EMBRACE, 5000u);
        AddCombatAction(DARION_ICY_TOUCH, 5000u);
        AddCombatAction(DARION_UNHOLY_BLIGHT, 6000u);
        AddCombatAction(DARION_COMBAT_YELL, 15000u);
        AddCombatAction(DARION_DEFEAT_YELL, true);

        InitializeDialogueHelper(m_instance);
    }

    world_map_ebon_hold* m_instance;

    GuidList m_lOutroGuardsGuids;

    void GetAIInformation(ChatHandler& reader) override
    {
        ScriptedAI::GetAIInformation(reader);

        if (m_instance)
            reader.PSendSysMessage("Current state for TYPE_BATTLE: %u", m_instance->GetData(TYPE_BATTLE));
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_LEONID_BARTHALOMEW_THE_REVERED:
                pSummoned->GetMotionMaster()->MoveWaypoint();
                break;
            case NPC_HIGHLORD_TIRION_FORDRING:
                // decrease Darion's damage
                DoCastSpellIfCan(m_creature, SPELL_THE_LIGHT_OF_DAWN_DAMAGE_LOSS, CAST_TRIGGERED);

                DoScriptText(SAY_LIGHT_OF_DAWN_OUTRO_1, pSummoned);
                pSummoned->SetWalk(false);
                pSummoned->GetMotionMaster()->MoveWaypoint();

                DisableCombatAction(DARION_COMBAT_YELL);
                ResetCombatAction(DARION_DEFEAT_YELL, 1000);
                break;
            case NPC_DARION_MOGRAINE:
                pSummoned->SetWalk(false);
                pSummoned->GetMotionMaster()->MoveWaypoint(0, 0, 5000);
                break;
            case NPC_THE_LICH_KING:
                DoScriptText(SAY_LIGHT_OF_DAWN_KING_VISIT_1, pSummoned);
                pSummoned->AI()->SetReactState(REACT_PASSIVE);
                pSummoned->SetCanEnterCombat(false);
                break;
        }
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId)
    {
        if (uiMotionType != WAYPOINT_MOTION_TYPE || !m_instance)
            return;

        // Start end dialogue
        if (uiPointId == 1 && m_instance->GetData(TYPE_BATTLE) == BATTLE_STATE_DIALOGUE)
        {
            // battle finished - remove light of dawn aura
            StartNextDialogueText(SAY_LIGHT_OF_DAWN_OUTRO_3);
            DoScriptText(EMOTE_LIGHT_OF_DAWN_KNEEL, m_creature);

            m_creature->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN_DUMMY);
            m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
        }
    }

    // Method to send out quest credit
    void DoSendQuestCredit()
    {
        Map::PlayerList const& PlayerList = m_creature->GetMap()->GetPlayers();

        for (const auto& itr : PlayerList)
        {
            Player* pPlayer = itr.getSource();
            if (pPlayer && pPlayer->GetQuestStatus(QUEST_ID_LIGHT_OF_DAWN) == QUEST_STATUS_INCOMPLETE && pPlayer->IsAlive() && pPlayer->IsWithinDistInMap(m_creature, 100.0f))
                pPlayer->CastSpell(pPlayer, SPELL_THE_LIGHT_OF_DAWN_CREDIT, TRIGGERED_OLD_TRIGGERED);
        }
    }

    // Start event conclusion
    void DoStartConclusion()
    {
        if (!m_instance)
            return;

        m_instance->SetData(TYPE_BATTLE, BATTLE_STATE_DIALOGUE);

        m_creature->SetImmuneToNPC(true);
        m_creature->SetImmuneToPlayer(true);
        m_creature->SetWalk(true);
        EnterEvadeMode();
        m_creature->Unmount();

        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MoveWaypoint(1);
        DoCastSpellIfCan(m_creature, SPELL_THE_LIGHT_OF_DAWN_DUMMY, CAST_TRIGGERED);

        // death knights are defeated; move them to the chapel
        if (Creature* pKoltira = m_instance->GetSingleCreatureFromStorage(NPC_KOLTIRA_DEATHWEAVER))
        {
            pKoltira->SetImmuneToNPC(true);
            pKoltira->SetImmuneToPlayer(true);
            pKoltira->AI()->EnterEvadeMode();
            pKoltira->Unmount();
            pKoltira->GetMotionMaster()->MoveWaypoint(1);
            pKoltira->CastSpell(pKoltira, SPELL_THE_LIGHT_OF_DAWN_DUMMY, TRIGGERED_OLD_TRIGGERED);
        }
        if (Creature* pThassarian = m_instance->GetSingleCreatureFromStorage(NPC_THASSARIAN))
        {
            pThassarian->SetImmuneToNPC(true);
            pThassarian->SetImmuneToPlayer(true);
            pThassarian->AI()->EnterEvadeMode();
            pThassarian->Unmount();
            pThassarian->GetMotionMaster()->MoveWaypoint(1);
            pThassarian->CastSpell(pThassarian, SPELL_THE_LIGHT_OF_DAWN_DUMMY, TRIGGERED_OLD_TRIGGERED);
        }
        // Orbaz flees -> despawn in WP script
        if (Creature* pOrbaz = m_instance->GetSingleCreatureFromStorage(NPC_ORBAZ_BLOODBANE))
        {
            DoScriptText(EMOTE_LIGHT_OF_DAWN_FLEE, pOrbaz);

            pOrbaz->SetImmuneToNPC(true);
            pOrbaz->SetImmuneToPlayer(true);
            pOrbaz->AI()->EnterEvadeMode();
            pOrbaz->GetMotionMaster()->MoveWaypoint(1);
        }

        // make light champs evade and go to outro position
        for (auto& entry : aLightChampions)
        {
            if (Creature* pChampion = m_instance->GetSingleCreatureFromStorage(entry))
            {
                pChampion->SetImmuneToNPC(true);
                pChampion->SetImmuneToPlayer(true);
                pChampion->AI()->EnterEvadeMode();
                pChampion->SetWalk(false);
                pChampion->GetMotionMaster()->Clear(false, true);
                pChampion->GetMotionMaster()->MoveWaypoint(1);
            }
        }

        // spawn soldiers for outro
        for (auto& i : aGuardsOutroSpawnLoc)
        {
            if (Creature* pGuard = m_creature->SummonCreature(NPC_DEFENDER_OF_THE_LIGHT, i.m_fX, i.m_fY, i.m_fZ, i.m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0))
            {
                pGuard->SetImmuneToNPC(true);
                pGuard->SetImmuneToPlayer(true);
                m_lOutroGuardsGuids.push_back(pGuard->GetObjectGuid());
            }
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        switch (eventType)
        {
            case AI_EVENT_CUSTOM_A:
                StartNextDialogueText(DATA_PREPARE_DIALOGUE);
                break;
            case AI_EVENT_CUSTOM_B:
                StartNextDialogueText(SAY_LIGHT_OF_DAWN_PREPARE_1);
                m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                break;
            case AI_EVENT_CUSTOM_C:
                m_creature->SummonCreature(NPC_HIGHLORD_TIRION_FORDRING, aEventLocations[0].m_fX, aEventLocations[0].m_fY, aEventLocations[0].m_fZ, aEventLocations[0].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0, true);
                break;
            case AI_EVENT_CUSTOM_D:
                DoScriptText(SAY_LIGHT_OF_DAWN_OUTRO_2, pSender);
                DoStartConclusion();
                break;

            default:
                break;
        }
    }

    void JustDidDialogueStep(int32 entry) override
    {
        if (!m_instance)
            return;

        switch (entry)
        {
            case DATA_SUMMON_CHAMPIONS:
                for (const auto& i : aLightChampionsSpawnLoc)
                    m_creature->SummonCreature(i.m_uiEntry, i.m_fX, i.m_fY, i.m_fZ, i.m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0);
                break;
            case DATA_SUMMON_DEFENDERS:
                for (const auto& i : aGuardsDefendSpawnLoc)
                    m_creature->SummonCreature(i.m_uiEntry, i.m_fX, i.m_fY, i.m_fZ, i.m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0);
                break;


            case SAY_LIGHT_OF_DAWN_PREPARE_3:
                m_instance->DoUpdateBattleZoneLightData(LIGHT_ID_BATTLE, 20);
                m_instance->DoUpdateBattleWeatherData(WEATHER_ID_BLOOD_RAIN, 1);

                for (const auto& i : aArmyLocations1)
                    m_creature->SummonCreature(i.m_uiEntry, i.m_fX, i.m_fY, i.m_fZ, i.m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0);
                break;
            case DATA_SUMMON_ARMY_1:
                for (const auto& i : aArmyLocations2)
                    m_creature->SummonCreature(i.m_uiEntry, i.m_fX, i.m_fY, i.m_fZ, i.m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0);
                break;
            case DATA_SUMMON_ARMY_2:
                for (const auto& i : aArmyLocations3)
                    m_creature->SummonCreature(i.m_uiEntry, i.m_fX, i.m_fY, i.m_fZ, i.m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0);
                break;
            case DATA_SUMMON_ARMY_3:
                for (const auto& i : aArmyLocations4)
                    m_creature->SummonCreature(i.m_uiEntry, i.m_fX, i.m_fY, i.m_fZ, i.m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0);
                break;
            case DATA_SUMMON_ARMY_4:
                for (const auto& i : aArmyLocations5)
                    m_creature->SummonCreature(i.m_uiEntry, i.m_fX, i.m_fY, i.m_fZ, i.m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0);
                break;
            case SAY_LIGHT_OF_DAWN_PREPARE_4:
                // send AI event for emote 53 (OneShotBattleRoar) or emote 434 (OneShotOnmicastGhoul)
                // start movement for the Flesh Behemoth
                SendAIEventAround(AI_EVENT_CUSTOM_EVENTAI_A, m_creature, 2000, 100.0f);
                break;
            case DATA_UNDEAD_ARMY_MARCH:
                DoCastSpellIfCan(m_creature, SPELL_THE_MIGHT_OF_MOGRAINE, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_HERO_AGGRO_AURA, CAST_TRIGGERED);

                m_creature->SetImmuneToNPC(false);
                m_creature->SetImmuneToPlayer(false);
                m_creature->SetWalk(false);
                m_creature->GetMotionMaster()->MoveWaypoint(0, 0, 2000);

                // inform the army to start WP movement; the Warriors play sound 11948
                SendAIEventAround(AI_EVENT_CUSTOM_EVENTAI_B, m_creature, 0, 100.0f);
                // inform the fellow Death Knights to start movement
                SendAIEventAround(AI_EVENT_CUSTOM_EVENTAI_C, m_creature, 3000, 50.0f);
                break;


            case SAY_LIGHT_OF_DAWN_OUTRO_7:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
            case NPC_HIGHLORD_ALEXANDROS_MOGRAINE:
                m_creature->SummonCreature(NPC_HIGHLORD_ALEXANDROS_MOGRAINE, aEventLocations[1].m_fX, aEventLocations[1].m_fY, aEventLocations[1].m_fZ, aEventLocations[1].m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0);
                break;
            case SAY_LIGHT_OF_DAWN_VISION_1:
                if (Creature* pAlexandros = m_instance->GetSingleCreatureFromStorage(NPC_HIGHLORD_ALEXANDROS_MOGRAINE))
                    m_creature->SetFacingToObject(pAlexandros);
                break;
            case SAY_LIGHT_OF_DAWN_VISION_3:
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_creature->SummonCreature(NPC_DARION_MOGRAINE, aEventLocations[2].m_fX, aEventLocations[2].m_fY, aEventLocations[2].m_fZ, aEventLocations[2].m_fO, TEMPSPAWN_TIMED_DESPAWN, 1 * MINUTE * IN_MILLISECONDS);
                break;
            case SAY_LIGHT_OF_DAWN_VISION_8:
                if (Creature* pTirion = m_instance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                {
                    pTirion->SetWalk(true);
                    pTirion->GetMotionMaster()->Clear(false, true);
                    pTirion->GetMotionMaster()->MoveWaypoint(1);
                }
                break;
            case NPC_THE_LICH_KING:
                m_creature->SummonCreature(NPC_THE_LICH_KING, aEventLocations[3].m_fX, aEventLocations[3].m_fY, aEventLocations[3].m_fZ, aEventLocations[3].m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0);
                break;
            case SAY_LIGHT_OF_DAWN_KING_VISIT_2:
                if (Creature* pLichKing = m_instance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                    pLichKing->CastSpell(pLichKing, SPELL_SOUL_FEAST_ALEX, TRIGGERED_NONE);
                break;
            case SAY_LIGHT_OF_DAWN_KING_VISIT_3:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
            case DATA_LICH_KING_POINT:
                if (Creature* pLichKing = m_instance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                    pLichKing->HandleEmote(EMOTE_ONESHOT_POINT);
                break;
            case DATA_LICH_KING_MOVE:
                if (Creature* pLichKing = m_instance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                    pLichKing->GetMotionMaster()->MoveWaypoint();
                break;
            case SPELL_MOGRAINE_CHARGE:
                DoCastSpellIfCan(m_creature, SPELL_MOGRAINE_CHARGE, CAST_TRIGGERED);
                break;
            case SAY_LIGHT_OF_DAWN_KING_VISIT_4:
                if (Creature* pLichKing = m_instance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                    pLichKing->CastSpell(m_creature, SPELL_REBUKE, TRIGGERED_OLD_TRIGGERED);
                break;
            case SAY_LIGHT_OF_DAWN_KING_VISIT_5:
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case DATA_LICH_KING_LAUGH:
                if (Creature* pLichKing = m_instance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                {
                    pLichKing->PlayDirectSound(SOUND_ID_LK_LAUGH);
                    pLichKing->HandleEmote(EMOTE_ONESHOT_LAUGH);
                }
                break;
            case EMOTE_LIGHT_OF_DAWN_GRASP:
                if (Creature* pLichKing = m_instance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                    pLichKing->CastSpell(pLichKing, SPELL_SOUL_FEAST_TIRION, TRIGGERED_OLD_TRIGGERED);
                break;
            case SAY_LIGHT_OF_DAWN_KING_VISIT_8:
                if (Creature* pLichKing = m_instance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                    pLichKing->SetImmuneToNPC(false);

                for (auto& entry : aLightChampions)
                {
                    if (Creature* pChampion = m_instance->GetSingleCreatureFromStorage(entry))
                    {
                        pChampion->SetWalk(false);
                        pChampion->GetMotionMaster()->Clear(false, true);
                        pChampion->GetMotionMaster()->MoveWaypoint(2);
                        pChampion->SetImmuneToNPC(false);
                    }
                }

                for (const auto& guid : m_lOutroGuardsGuids)
                {
                    if (Creature* pGuard = m_creature->GetMap()->GetCreature(guid))
                    {
                        pGuard->SetWalk(false);
                        pGuard->GetMotionMaster()->Clear(false, true);
                        pGuard->GetMotionMaster()->MoveWaypoint(urand(0, 2));
                        pGuard->SetImmuneToNPC(false);
                    }
                }
                break;
            case SAY_LIGHT_OF_DAWN_KING_VISIT_9:
                if (Creature* pLichKing = m_instance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                    pLichKing->CastSpell(pLichKing, SPELL_APOCALYPSE, TRIGGERED_OLD_TRIGGERED);
                break;
            case DATA_RESET_CHAMPIONS:
                for (auto& entry : aLightChampions)
                {
                    if (Creature* pChampion = m_instance->GetSingleCreatureFromStorage(entry))
                    {
                        pChampion->GetMotionMaster()->Clear(false, true);
                        pChampion->GetMotionMaster()->MoveWaypoint(1);
                        pChampion->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                    }
                }

                for (const auto& guid : m_lOutroGuardsGuids)
                {
                    if (Creature* pGuard = m_creature->GetMap()->GetCreature(guid))
                    {
                        pGuard->GetMotionMaster()->Clear(false, true);
                        pGuard->GetMotionMaster()->MoveIdle();
                        pGuard->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                        pGuard->SetStandState(UNIT_STAND_STATE_DEAD);
                    }
                }
                break;
            case SAY_LIGHT_OF_DAWN_KING_VISIT_11:
                if (Creature* pTirion = m_instance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                    m_creature->SetFacingToObject(pTirion);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
            case SPELL_ASHBRINGER:
                DoCastSpellIfCan(m_creature, SPELL_ASHBRINGER, CAST_TRIGGERED);
                SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                break;
            case SPELL_REBIRTH_OF_THE_ASHBRINGER:
                if (Creature* pLichKing = m_instance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                    pLichKing->InterruptNonMeleeSpells(false);

                if (Creature* pTirion = m_instance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                {
                    pTirion->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, EQUIP_HIGHLORD_TIRION_FORDRING);
                    pTirion->CastSpell(pTirion, SPELL_REBIRTH_OF_THE_ASHBRINGER, TRIGGERED_OLD_TRIGGERED);
                    pTirion->SetStandState(UNIT_STAND_STATE_STAND);
                    pTirion->HandleEmote(EMOTE_ONESHOT_ROAR);
                    pTirion->PlayDirectSound(SOUND_ID_TIRION_ROAR);
                }

                m_instance->DoRespawnGameObject(GO_LIGHT_OF_DAWN, 7 * MINUTE);
                break;
            case EMOTE_LIGHT_OF_DAWN_COLAPSE:
                if (Creature* pTirion = m_instance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                    pTirion->RemoveAurasDueToSpell(SPELL_REBIRTH_OF_THE_ASHBRINGER);

                m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                break;
            case SAY_LIGHT_OF_DAWN_KING_VISIT_15:
                if (Creature* pTirion = m_instance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                    pTirion->CastSpell(pTirion, SPELL_TIRION_CHARGE, TRIGGERED_OLD_TRIGGERED);
                break;
            case DATA_LICH_KING_FACE_TIRION:
                if (Creature* pLichKing = m_instance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                    if (Creature* pTirion = m_instance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                        pLichKing->SetFacingToObject(pTirion);
                break;
            case DATA_LICH_KING_DEFEAT:
                if (Creature* pLichKing = m_instance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                    pLichKing->HandleEmote(EMOTE_ONESHOT_SHOUT);
                break;
            case SPELL_TELEPORT_VISUAL:
                if (Creature* pLichKing = m_instance->GetSingleCreatureFromStorage(NPC_THE_LICH_KING))
                {
                    pLichKing->CastSpell(pLichKing, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
                    pLichKing->ForcedDespawn(2000);
                }

                if (Creature* pTirion = m_instance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                {
                    float fX, fY, fZ;
                    m_creature->GetContactPoint(pTirion, fX, fY, fZ, CONTACT_DISTANCE * 3);
                    pTirion->GetMotionMaster()->Clear(false, true);
                    pTirion->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                }
                break;
            case SPELL_LAY_ON_HANDS:
                if (Creature* pTirion = m_instance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                    pTirion->CastSpell(pTirion, SPELL_LAY_ON_HANDS, TRIGGERED_OLD_TRIGGERED);

                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case DATA_TIRION_CONCLUSION:
                if (Creature* pTirion = m_instance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                {
                    pTirion->SetWalk(true);
                    pTirion->GetMotionMaster()->Clear(false, true);
                    pTirion->GetMotionMaster()->MoveWaypoint(2);
                }
                break;
            case SAY_LIGHT_OF_DAWN_EPILOGUE_2:
                if (Creature* pTirion = m_instance->GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                    pTirion->SetFacingToObject(m_creature);
                break;
            case SAY_LIGHT_OF_DAWN_EPILOGUE_9:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
            case DATA_DARION_EXCLAMATION:
                m_creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                break;


                // Event finished; wait for players to complete quest
            case QUEST_ID_LIGHT_OF_DAWN:
                m_instance->SetData(TYPE_BATTLE, BATTLE_STATE_WAIT_QUEST);

                m_instance->DoUpdateBattleZoneLightData(LIGHT_ID_DEFAULT, 5);
                m_instance->DoUpdateBattleWeatherData(WEATHER_ID_FOG, 0);

                DoSendQuestCredit();
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                break;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case DARION_ANTI_MAGIC_ZONE:
                if (DoCastSpellIfCan(m_creature, SPELL_ANTI_MAGIC_ZONE_DARION) == CAST_OK)
                    ResetCombatAction(action, urand(35000, 40000));
                break;
            case DARION_DEATH_STRIKE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DEATH_STRIKE) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 15000));
                break;
            case DARION_DEATH_EMBRACE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DEATH_EMBRACE) == CAST_OK)
                    ResetCombatAction(action, urand(50000, 60000));
                break;
            case DARION_ICY_TOUCH:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ICY_TOUCH_DARION) == CAST_OK)
                    ResetCombatAction(action, 25000);
                break;
            case DARION_UNHOLY_BLIGHT:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_UNHOLY_BLIGHT) == CAST_OK)
                    ResetCombatAction(action, urand(65000, 75000));
                break;
            case DARION_COMBAT_YELL:
                switch (urand(0, 8))
                {
                    case 0: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_1, m_creature); break;
                    case 1: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_2, m_creature); break;
                    case 2: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_3, m_creature); break;
                    case 3: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_4, m_creature); break;
                    case 4: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_5, m_creature); break;
                    case 5: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_6, m_creature); break;
                    case 6: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_7, m_creature); break;
                    case 7: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_8, m_creature); break;
                    case 8: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_9, m_creature); break;
                }

                // make sure that darion always stays in the area
                if (!m_creature->IsWithinDist2d(aEventLocations[1].m_fX, aEventLocations[1].m_fY, 75.0f))
                    m_creature->GetMotionMaster()->MovePoint(0, aEventLocations[1].m_fX, aEventLocations[1].m_fY, aEventLocations[1].m_fZ);

                ResetCombatAction(action, urand(15000, 20000));
                break;
            case DARION_DEFEAT_YELL:
                switch (urand(0, 6))
                {
                    case 0: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_10, m_creature); break;
                    case 1: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_11, m_creature); break;
                    case 2: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_12, m_creature); break;
                    case 3: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_13, m_creature); break;
                    case 4: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_14, m_creature); break;
                    case 5: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_15, m_creature); break;
                    case 6: DoScriptText(SAY_LIGHT_OF_DAWN_BATTLE_16, m_creature); break;
                }

                // make sure that darion always stays in the area
                if (!m_creature->IsWithinDist2d(aEventLocations[1].m_fX, aEventLocations[1].m_fY, 75.0f))
                    m_creature->GetMotionMaster()->MovePoint(0, aEventLocations[1].m_fX, aEventLocations[1].m_fY, aEventLocations[1].m_fZ);

                ResetCombatAction(action, urand(5000, 7000));
                break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        DialogueUpdate(diff);

        CombatAI::UpdateAI(diff);
    }
};

bool GossipHello_npc_highlord_darion_mograine(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    // Only allow start battle after reset
    if (world_map_ebon_hold* pInstance = static_cast<world_map_ebon_hold*>(pCreature->GetInstanceData()))
    {
        if (pPlayer->GetQuestStatus(QUEST_ID_LIGHT_OF_DAWN) == QUEST_STATUS_INCOMPLETE && pInstance->GetData(TYPE_BATTLE) == BATTLE_STATE_NOT_STARTED)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_READY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_READY, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_highlord_darion_mograine(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction ==  GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (world_map_ebon_hold* pInstance = static_cast<world_map_ebon_hold*>(pCreature->GetInstanceData()))
        {
            // set data to special in order to start the event
            pInstance->SetData(TYPE_BATTLE, BATTLE_STATE_PREPARE);
            pCreature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pPlayer, pCreature);

            pPlayer->CLOSE_GOSSIP_MENU();

            return true;
        }
    }
    pPlayer->CLOSE_GOSSIP_MENU();

    return false;
}

/*######
## npc_scarlet_courier
######*/

enum
{
    SAY_TREE_1          = -1609079,
    SAY_TREE_2          = -1609080,

    GO_TREE             = 191144,
};

struct npc_scarlet_courierAI : public ScriptedAI
{
    npc_scarlet_courierAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiInitTimer;
    uint32 m_uiCombatTimer;
    uint8 m_uiCombatStage;

    void Reset() override
    {
        m_uiInitTimer   = 2000;
        m_uiCombatTimer = 0;
        m_uiCombatStage = 0;
    }

    void AttackedBy(Unit* /*pAttacker*/) override
    {
        m_creature->Unmount();
    }

    void JustReachedHome() override
    {
        m_creature->ForcedDespawn();
        DoDespawnTree();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoDespawnTree();
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || !uiPointId)
            return;

        m_uiCombatTimer = 5000;
    }

    // Wrapper function that despawns the tree
    void DoDespawnTree()
    {
        if (GameObject* pTree = GetClosestGameObjectWithEntry(m_creature, GO_TREE, 30.0f))
            pTree->SetLootState(GO_JUST_DEACTIVATED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // walk to the tree
        if (m_uiInitTimer)
        {
            if (m_uiInitTimer <= uiDiff)
            {
                DoScriptText(SAY_TREE_1, m_creature);

                float fX, fY, fZ;
                if (GameObject* pTree = GetClosestGameObjectWithEntry(m_creature, GO_TREE, 30.0f))
                {
                    pTree->GetContactPoint(m_creature, fX, fY, fZ);
                    m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                }

                m_uiInitTimer = 0;
            }
            else
                m_uiInitTimer -= uiDiff;
        }

        // despawn tree and start combat
        if (m_uiCombatTimer)
        {
            if (m_uiCombatTimer <= uiDiff)
            {
                switch (m_uiCombatStage)
                {
                    case 0:
                        DoScriptText(SAY_TREE_2, m_creature);
                        m_creature->Unmount();
                        DoDespawnTree();

                        m_uiCombatTimer = 3000;
                        break;
                    case 1:
                        if (m_creature->IsTemporarySummon())
                        {
                            if (Player* pSummoner = m_creature->GetMap()->GetPlayer(m_creature->GetSpawnerGuid()))
                                m_creature->AI()->AttackStart(pSummoner);
                        }

                        m_uiCombatTimer = 0;
                        break;
                }
                ++m_uiCombatStage;
            }
            else
                m_uiCombatTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

/*######
## spell_emblazon_runeblade - 51770
######*/

struct spell_emblazon_runeblade : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        if (!caster)
            return;

        uint32 uiSpell = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        caster->CastSpell(caster, uiSpell, TRIGGERED_NONE);
    }
};

/*######
## spell_emblazon_runeblade_aura - 51769
######*/

struct spell_emblazon_runeblade_aura : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        // override target; the real caster of the triggered spell is the player
        data.caster = aura->GetCaster();
        data.target = nullptr;
    }
};

/*######
## spell_death_knight_initiate_visual - 51519
######*/

struct spell_death_knight_initiate_visual : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget)
            return;

        uint32 spellId;

        bool isMale = unitTarget->getGender() == GENDER_MALE;
        switch (unitTarget->getRace())
        {
            case RACE_HUMAN:    spellId = isMale ? 51520 : 51534; break;
            case RACE_DWARF:    spellId = isMale ? 51538 : 51537; break;
            case RACE_NIGHTELF: spellId = isMale ? 51535 : 51536; break;
            case RACE_GNOME:    spellId = isMale ? 51539 : 51540; break;
            case RACE_DRAENEI:  spellId = isMale ? 51541 : 51542; break;
            case RACE_ORC:      spellId = isMale ? 51543 : 51544; break;
            case RACE_UNDEAD:   spellId = isMale ? 51549 : 51550; break;
            case RACE_TAUREN:   spellId = isMale ? 51547 : 51548; break;
            case RACE_TROLL:    spellId = isMale ? 51546 : 51545; break;
            case RACE_BLOODELF: spellId = isMale ? 51551 : 51552; break;
            default:
                return;
        }

        unitTarget->CastSpell(unitTarget, spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_siphon_of_acherus_aura - 51859
######*/

struct spell_siphon_of_acherus_aura : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        // override target; the real caster of the triggered spell is the player
        data.caster = aura->GetCaster();
        data.target = nullptr;
    }
};

/*######
## spell_siphon_of_acherus - 51858
######*/

struct spell_siphon_of_acherus : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!caster || !target)
            return;

        uint32 spellId;

        switch (target->GetEntry())
        {
            case 28525: spellId = 51973; break;     // New Avalon Forge
            case 28542: spellId = 51979; break;     // Scarlet Hold
            case 28543: spellId = 51976; break;     // New Avalon Town Hall
            case 28544: spellId = 51981; break;     // Chapel of the Crimson Flame
            default:
                return;
        }

        target->CastSpell(caster, spellId, TRIGGERED_NONE);
    }
};

/*######
## spell_siphon_of_acherus_credit - 51973, 51976, 51979, 51981
######*/

struct spell_siphon_of_acherus_credit : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        uint32 triggerSpellId;

        switch (spell->m_spellInfo->Id)
        {
            case 51973: triggerSpellId = 51974; break;  // New Avalon Forge
            case 51979: triggerSpellId = 51980; break;  // Scarlet Hold
            case 51976: triggerSpellId = 51977; break;  // New Avalon Town Hall
            case 51981: triggerSpellId = 51982; break;  // Chapel of the Crimson Flame
            default:
                return;
        }

        target->CastSpell(target, triggerSpellId, TRIGGERED_NONE);
    }
};

/*######
## spell_recall_eye_of_acherus - 52694
######*/

struct spell_recall_eye_of_acherus : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        if (!caster)
            return;

        Unit* charmer = caster->GetCharmer();
        if (!charmer || charmer->GetTypeId() != TYPEID_PLAYER)
            return;

        charmer->RemoveAurasDueToSpell(SPELL_EYE_CONTROL);
        charmer->RemoveAurasDueToSpell(SPELL_EYE_FLIGHT_BOOST);
    }
};

/*######
## spell_summon_ghouls_scarlet_crusade - 51904
######*/

struct spell_summon_ghouls_scarlet_crusade : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        // cast Summon Ghouls On Scarlet Crusade
        target->CastSpell(target, 51900, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_ghoulplosion - 52519
######*/

struct spell_ghoulplosion : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        // action handled by effect 2; effect 0 is for visual
        if (effIdx != EFFECT_INDEX_2)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        // cast Dispel Scarlet Ghoul Credit Counter on ghoul owner
        target->CastSpell(target, 52555, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_dispel_scarlet_ghoul_credit - 52555
######*/

struct spell_dispel_scarlet_ghoul_credit : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster || !caster->IsCreature())
            return;

        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        // remove ghoul counter aura - 52500
        target->RemoveAurasByCasterSpell(spellId, caster->GetObjectGuid());

        // unsummon pet
        Creature* ghoul = static_cast<Creature*>(caster);
        if (ghoul->IsPet())
            (static_cast<Pet*>(caster))->Unsummon(PET_SAVE_AS_DELETED);
    }
};

/*######
## spell_gift_of_the_harvester - 52479
######*/

struct spell_gift_of_the_harvester : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster || !caster->IsPlayer())
            return;

        // summon ghoul using spell 52490
        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        // Each ghoul casts 52500 onto player, so use number of auras as check
        Unit::SpellAuraHolderConstBounds bounds = caster->GetSpellAuraHolderBounds(52500);
        uint32 summonedGhouls = std::distance(bounds.first, bounds.second);

        // randomly summon a ghoul pet (creature id 28845) or a hostile ghost (creature id 28846)
        caster->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), urand(0, 2) || summonedGhouls >= 5 ? 52505 : spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## go_plague_cauldron
######*/

struct go_plague_cauldron : public GameObjectAI
{
    go_plague_cauldron(GameObject* go) : GameObjectAI(go)
    {
        go->GetVisibilityData().SetInvisibilityMask(10, true);
        go->GetVisibilityData().AddInvisibilityValue(10, 1000);
    }
};

/*######
## spell_devour_humanoid - 53110
######*/

struct spell_devour_humanoid : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!caster || !target || !target->IsCreature())
            return;

        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);
        target->CastSpell(caster, spellId, TRIGGERED_OLD_TRIGGERED);

        Creature* creatureTarget = static_cast<Creature*>(target);
        creatureTarget->ForcedDespawn(8000);
    }
};

/*######
## spell_portal_to_capital_city - 58418, 58420
######*/

struct spell_portal_to_capital_city : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer())
            return;

        Player* playerTarget = static_cast<Player*>(target);

        // get quest id and spell id
        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_0);
        uint32 questId = spell->m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_1);

        // check for quest complete, but not rewarded
        if (playerTarget->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE && !playerTarget->GetQuestRewardStatus(questId))
            target->CastSpell(target, spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_acherus_deathcharger - 48778
######*/

struct spell_acherus_deathcharger : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        if (Unit* caster = spell->GetCaster())
        {
            if (caster->HasAura(53081))
            {
                auto& list = spell->GetTargetList();
                for (auto& target : list)
                    if (target.targetGUID == caster->GetObjectGuid())
                        target.effectHitMask &= ~((1 << EFFECT_INDEX_1) | (1 << EFFECT_INDEX_0));
            }
        }
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_2 && spell->GetCaster()->HasAura(53081))
            spell->GetCaster()->CastSpell(nullptr, 58819, TRIGGERED_INSTANT_CAST);
    }
};

/*######
## spell_skeletal_gryphon_escape - 52588
######*/

struct spell_skeletal_gryphon_escape : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer() || !target->IsBoarded())
            return;

        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);
        target->RemoveAurasDueToSpell(spellId);
    }
};

/*######
## spell_death_gate - 52751
######*/

struct spell_death_gate : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer() || target->getClass() != CLASS_DEATH_KNIGHT)
            return;

        // cast spell 53822 to teleport player to Ebon Hold
        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);
        target->CastSpell(target, spellId, TRIGGERED_NONE);
    }
};

struct EyeOfAcherus : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0 && !apply)
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_EYE_FLIGHT_BOOST);
    }
};

void AddSC_ebon_hold()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_a_special_surprise";
    pNewScript->GetAI = &GetNewAIInstance<npc_a_special_surpriseAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_death_knight_initiate";
    pNewScript->GetAI = &GetNewAIInstance<npc_death_knight_initiateAI>;
    pNewScript->pGossipHello = &GossipHello_npc_death_knight_initiate;
    pNewScript->pGossipSelect = &GossipSelect_npc_death_knight_initiate;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_death_knight_initiate;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_eye_of_acherus";
    pNewScript->GetAI = &GetNewAIInstance<npc_eye_of_acherusAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_scarlet_ghoul";
    pNewScript->GetAI = &GetNewAIInstance<npc_scarlet_ghoulAI>;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_scarlet_ghoul;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_highlord_darion_mograine";
    pNewScript->GetAI = &GetNewAIInstance<npc_highlord_darion_mograineAI>;
    pNewScript->pGossipHello =  &GossipHello_npc_highlord_darion_mograine;
    pNewScript->pGossipSelect = &GossipSelect_npc_highlord_darion_mograine;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_scarlet_courier";
    pNewScript->GetAI = &GetNewAIInstance<npc_scarlet_courierAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_plague_cauldron";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_plague_cauldron>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_emblazon_runeblade>("spell_emblazon_runeblade");
    RegisterSpellScript<spell_emblazon_runeblade_aura>("spell_emblazon_runeblade_aura");
    RegisterSpellScript<spell_death_knight_initiate_visual>("spell_death_knight_initiate_visual");
    RegisterSpellScript<spell_siphon_of_acherus_aura>("spell_siphon_of_acherus_aura");
    RegisterSpellScript<spell_siphon_of_acherus>("spell_siphon_of_acherus");
    RegisterSpellScript<spell_siphon_of_acherus_credit>("spell_siphon_of_acherus_credit");
    RegisterSpellScript<spell_recall_eye_of_acherus>("spell_recall_eye_of_acherus");
    RegisterSpellScript<spell_summon_ghouls_scarlet_crusade>("spell_summon_ghouls_scarlet_crusade");
    RegisterSpellScript<spell_ghoulplosion>("spell_ghoulplosion");
    RegisterSpellScript<spell_dispel_scarlet_ghoul_credit>("spell_dispel_scarlet_ghoul_credit");
    RegisterSpellScript<spell_gift_of_the_harvester>("spell_gift_of_the_harvester");
    RegisterSpellScript<spell_devour_humanoid>("spell_devour_humanoid");
    RegisterSpellScript<spell_portal_to_capital_city>("spell_portal_to_capital_city");
    RegisterSpellScript<spell_acherus_deathcharger>("spell_acherus_deathcharger");
    RegisterSpellScript<spell_skeletal_gryphon_escape>("spell_skeletal_gryphon_escape");
    RegisterSpellScript<spell_death_gate>("spell_death_gate");
    RegisterSpellScript<EyeOfAcherus>("spell_eye_of_acherus");
}

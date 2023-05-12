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
SDName: Shadowmoon_Valley
SD%Complete: 100
SDComment: Quest support: 10451, 10458, 10480, 10481, 10514, 10540, 10588, 10707, 10781, 10804, 10854, 10985, 11020, 11064, 11067, 11068, 11069, 11070, 11071.
SDCategory: Shadowmoon Valley
EndScriptData */

/* ContentData
mob_mature_netherwing_drake
mob_enslaved_netherwing_drake
npc_dragonmaw_peon
npc_wilda
mob_torloth
npc_lord_illidan_stormrage
npc_totem_of_spirits
event_spell_soul_captured_credit
go_crystal_prison
npc_spawned_oronok_tornheart
npc_domesticated_felboar
npc_veneratus_spawn_node
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "AI/ScriptDevAI/base/pet_ai.h"
#include "Entities/TemporarySpawn.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

/*#####
# mob_mature_netherwing_drake
#####*/

enum
{
    SAY_JUST_EATEN              = -1000175,

    SPELL_PLACE_CARCASS         = 38439,
    SPELL_JUST_EATEN            = 38502,
    SPELL_NETHER_BREATH         = 38467,
    SPELL_INTANGIBLE_PRESENCE   = 36513,

    QUEST_KINDNESS              = 10804,
    NPC_EVENT_PINGER            = 22131,

    GO_FLAYER_CARCASS           = 185155,
};

struct mob_mature_netherwing_drakeAI : public ScriptedAI
{
    mob_mature_netherwing_drakeAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_playerGuid;

    uint32 m_uiEatTimer;
    uint32 m_uiCreditTimer;
    uint32 m_uiNetherbreathTimer;
    uint32 m_uiIntangiblePresenceTimer;

    void Reset() override
    {
        m_playerGuid.Clear();

        m_uiEatTimer    = 0;
        m_uiCreditTimer = 0;
        m_uiNetherbreathTimer = 5000;
        m_uiIntangiblePresenceTimer = 12000;
    }

    void SpellHit(Unit* pCaster, SpellEntry const* pSpell) override
    {
        if (m_uiEatTimer || m_uiCreditTimer)
            return;

        if (pCaster->GetTypeId() == TYPEID_PLAYER && pSpell->Id == SPELL_PLACE_CARCASS && !m_creature->HasAura(SPELL_JUST_EATEN))
        {
            m_playerGuid = pCaster->GetObjectGuid();
            m_uiEatTimer = 5000;
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE)
            return;

        if (uiPointId)
        {
            m_uiCreditTimer = 7000;
            m_creature->SetLevitate(false);
            m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
            m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
        }
    }

    void JustReachedHome() override
    {
        m_creature->GetMotionMaster()->Clear();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiEatTimer)
        {
            if (m_uiEatTimer <= uiDiff)
            {
                if (GameObject* pGo = GetClosestGameObjectWithEntry(m_creature, GO_FLAYER_CARCASS, 80.0f))
                {
                    if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
                        m_creature->GetMotionMaster()->MovementExpired();

                    m_creature->GetMotionMaster()->MoveIdle();

                    float fX, fY, fZ;
                    pGo->GetContactPoint(m_creature, fX, fY, fZ, CONTACT_DISTANCE);

                    m_creature->SetWalk(false);
                    m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                }
                m_uiEatTimer = 0;
            }
            else
                m_uiEatTimer -= uiDiff;

            return;
        }
        if (m_uiCreditTimer)
        {
            if (m_uiCreditTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_JUST_EATEN);
                DoScriptText(SAY_JUST_EATEN, m_creature);

                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                    pPlayer->KilledMonsterCredit(NPC_EVENT_PINGER, m_creature->GetObjectGuid());

                Reset();
                m_creature->SetLevitate(true);
                m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
                m_creature->GetMotionMaster()->Clear();
                m_uiCreditTimer = 0;
            }
            else
                m_uiCreditTimer -= uiDiff;

            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiNetherbreathTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_NETHER_BREATH);
            m_uiNetherbreathTimer = urand(9000, 13000);
        }
        else
            m_uiNetherbreathTimer -= uiDiff;

        if (m_uiIntangiblePresenceTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_INTANGIBLE_PRESENCE);
            m_uiIntangiblePresenceTimer = urand(12000, 16000);
        }
        else
            m_uiIntangiblePresenceTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_mob_mature_netherwing_drake(Creature* pCreature)
{
    return new mob_mature_netherwing_drakeAI(pCreature);
}

/*###
# mob_enslaved_netherwing_drake
####*/

enum
{
    FACTION_FRIENDLY                = 1840,                 // Not sure if this is correct, it was taken off of Mordenai.

    SPELL_HIT_FORCE_OF_NELTHARAKU   = 38762,
    SPELL_FORCE_OF_NELTHARAKU       = 38775,

    EMOTE_ON_HIT_FORCE              = -1015001,

    QUEST_FORCE_OF_NELT             = 10854,
    NPC_DRAGONMAW_SUBJUGATOR        = 21718,
    NPC_ESCAPE_DUMMY                = 21348
};

struct mob_enslaved_netherwing_drakeAI : public ScriptedAI
{
    mob_enslaved_netherwing_drakeAI(Creature* creature) : ScriptedAI(creature), m_uiFlyTimer(0), m_disabled(false)
    {
        Reset();
    }

    ObjectGuid m_playerGuid;
    uint32 m_uiFlyTimer;
    bool m_disabled;

    void Reset() override
    {
        m_creature->SetCanFly(false);
        m_creature->SetHover(false);
    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        m_disabled = false;
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_HIT_FORCE_OF_NELTHARAKU && !m_disabled)
        {
            if (Player* pPlayer = pCaster->GetBeneficiaryPlayer())
            {
                m_uiFlyTimer = 2500;
                m_disabled = true;
                m_playerGuid = pPlayer->GetObjectGuid();

                DoScriptText(EMOTE_ON_HIT_FORCE, m_creature);

                m_creature->SetFactionTemporary(FACTION_FRIENDLY, TEMPFACTION_RESTORE_RESPAWN);

                if (Creature* pDragonmaw = GetClosestCreatureWithEntry(m_creature, NPC_DRAGONMAW_SUBJUGATOR, 20.0f))
                    AttackStart(pDragonmaw);
            }
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE)
            return;

        if (uiPointId)
            m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
        {
            if (m_uiFlyTimer)
            {
                if (m_uiFlyTimer <= uiDiff)
                {
                    if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                    {
                        DoCastSpellIfCan(pPlayer, SPELL_FORCE_OF_NELTHARAKU, CAST_TRIGGERED);
                        m_playerGuid.Clear();

                        float fX, fY, fZ;

                        // Get an escape position
                        if (Creature* pEscapeDummy = GetClosestCreatureWithEntry(m_creature, NPC_ESCAPE_DUMMY, 50.0f))
                            pEscapeDummy->GetPosition(fX, fY, fZ);
                        else
                        {
                            m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 20.0f, fX, fY, fZ);
                            fZ += 25;
                        }
                        m_creature->SetCanFly(true);
                        m_creature->SetHover(true);
                        m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                    }
                    m_uiFlyTimer = 0;
                }
                else
                    m_uiFlyTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_mob_enslaved_netherwing_drake(Creature* pCreature)
{
    return new mob_enslaved_netherwing_drakeAI(pCreature);
}

/*#####
# npc_dragonmaw_peon
#####*/

enum
{
    SAY_PEON_1                      = -1000652,
    SAY_PEON_2                      = -1000653,
    SAY_PEON_3                      = -1000654,
    SAY_PEON_4                      = -1000655,
    SAY_PEON_5                      = -1000656,

    SPELL_SERVING_MUTTON            = 40468,
    NPC_DRAGONMAW_KILL_CREDIT       = 23209,
    EQUIP_ID_MUTTON                 = 2202,
    POINT_DEST                      = 1
};

struct npc_dragonmaw_peonAI : public ScriptedAI
{
    npc_dragonmaw_peonAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_playerGuid;
    uint32 m_uiPoisonTimer;
    uint32 m_uiMoveTimer;
    uint32 m_uiEatTimer;

    void Reset() override
    {
        m_playerGuid.Clear();
        m_uiPoisonTimer = 0;
        m_uiMoveTimer = 0;
        m_uiEatTimer = 0;

        SetEquipmentSlots(true);
    }

    bool SetPlayerTarget(ObjectGuid playerGuid)
    {
        // Check if event already started
        if (m_playerGuid)
            return false;

        m_playerGuid = playerGuid;
        m_uiMoveTimer = 500;
        return true;
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_DEST)
        {
            m_uiEatTimer = 2000;
            m_uiPoisonTimer = 3000;

            switch (urand(0, 4))
            {
                case 0: DoScriptText(SAY_PEON_1, m_creature); break;
                case 1: DoScriptText(SAY_PEON_2, m_creature); break;
                case 2: DoScriptText(SAY_PEON_3, m_creature); break;
                case 3: DoScriptText(SAY_PEON_4, m_creature); break;
                case 4: DoScriptText(SAY_PEON_5, m_creature); break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiMoveTimer)
        {
            if (m_uiMoveTimer <= uiDiff)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                {
                    GameObject* pMutton = pPlayer->GetGameObject(SPELL_SERVING_MUTTON);

                    // Workaround for broken function GetGameObject
                    if (!pMutton)
                    {
                        const SpellEntry* pSpell = GetSpellStore()->LookupEntry<SpellEntry>(SPELL_SERVING_MUTTON);

                        uint32 uiGameobjectEntry = pSpell->EffectMiscValue[EFFECT_INDEX_0];

                        // this can fail, but very low chance
                        pMutton = GetClosestGameObjectWithEntry(pPlayer, uiGameobjectEntry, 2 * INTERACTION_DISTANCE);
                    }

                    if (pMutton)
                    {
                        float fX, fY, fZ;
                        pMutton->GetContactPoint(m_creature, fX, fY, fZ, CONTACT_DISTANCE);

                        m_creature->SetWalk(false);
                        m_creature->GetMotionMaster()->MovePoint(POINT_DEST, fX, fY, fZ);
                    }
                }

                m_uiMoveTimer = 0;
            }
            else
                m_uiMoveTimer -= uiDiff;
        }
        else if (m_uiEatTimer)
        {
            if (m_uiEatTimer <= uiDiff)
            {
                SetEquipmentSlots(false, EQUIP_ID_MUTTON, EQUIP_UNEQUIP);
                m_creature->HandleEmote(EMOTE_ONESHOT_EAT_NOSHEATHE);
                m_uiEatTimer = 0;
            }
            else
                m_uiEatTimer -= uiDiff;
        }
        else if (m_uiPoisonTimer)
        {
            if (m_uiPoisonTimer <= uiDiff)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                    pPlayer->KilledMonsterCredit(NPC_DRAGONMAW_KILL_CREDIT, m_creature->GetObjectGuid());

                m_uiPoisonTimer = 0;

                // dies
                m_creature->SetDeathState(JUST_DIED);
                m_creature->SetHealth(0);
            }
            else
                m_uiPoisonTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget())
            return;

        DoMeleeAttackIfReady();
    }
};

bool EffectDummyCreature_npc_dragonmaw_peon(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiEffIndex != EFFECT_INDEX_1 || uiSpellId != SPELL_SERVING_MUTTON || pCaster->GetTypeId() != TYPEID_PLAYER)
        return false;

    npc_dragonmaw_peonAI* pPeonAI = dynamic_cast<npc_dragonmaw_peonAI*>(pCreatureTarget->AI());

    if (!pPeonAI)
        return false;

    if (pPeonAI->SetPlayerTarget(pCaster->GetObjectGuid()))
    {
        pCreatureTarget->HandleEmote(EMOTE_ONESHOT_NONE);
        return true;
    }

    return false;
}

/*######
# npc_wilda
######*/

enum
{
    SAY_WIL_START               = -1000381,
    SAY_WIL_AGGRO_1             = -1000382,
    SAY_WIL_AGGRO_2             = -1000383,
    SAY_WIL_FREE_SPIRITS        = -1000384,
    SAY_WIL_FIND_EXIT           = -1000386,
    SAY_WIL_PROGRESS_1          = -1000385,
    SAY_WIL_PROGRESS_2          = -1000387,
    SAY_WIL_PROGRESS_3          = -1000388,
    SAY_WIL_PROGRESS_4          = -1001168,
    SAY_WIL_PROGRESS_5          = -1001169,
    SAY_WIL_JUST_AHEAD          = -1000389,
    SAY_WIL_END                 = -1000390,

    SPELL_CHAIN_LIGHTNING       = 16006,
    SPELL_EARTHBING_TOTEM       = 15786,
    SPELL_FROST_SHOCK           = 12548,
    SPELL_HEALING_WAVE          = 12491,
    SPELL_WATER_BUBBLE          = 35929,
    SPELL_BREAK_WATER_PRISON    = 35933,

    QUEST_ESCAPE_COILSCAR       = 10451,
    NPC_COILSKAR_ASSASSIN       = 21044,
    NPC_CAPTURED_WATER_SPIRIT   = 21029,
};

struct npc_wildaAI : public npc_escortAI
{
    npc_wildaAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        // the creature is floating in a prison; no quest available first;
        // the floating prison setup and quest flag restore is handled by DB
        m_creature->SetLevitate(true);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

        Reset();
    }

    uint32 m_uiHealingTimer;
    uint32 m_uiShockTimer;
    uint32 m_uiLightningTimer;

    void Reset() override
    {
        m_uiHealingTimer = 0;
        m_uiShockTimer = 1000;
        m_uiLightningTimer = 2000;

        m_attackDistance = 10.0f;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (roll_chance_i(30))
            DoCastSpellIfCan(m_creature, SPELL_EARTHBING_TOTEM);
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 9:
            case 27:
            case 31:
            case 33:
            case 40:
            case 44:
            case 52:
                DoSpawnAssassin();
                break;
            case 14:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_WIL_FREE_SPIRITS, m_creature, pPlayer);
                DoCastSpellIfCan(m_creature, SPELL_BREAK_WATER_PRISON);
                break;
            case 15:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_WIL_FIND_EXIT, m_creature, pPlayer);
                DoFreeSpirits();
                break;
            case 16:
                DoSpawnAssassin(2);
                break;
            case 41:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_WIL_JUST_AHEAD, m_creature, pPlayer);
                break;
            case 53:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoDespawnSpirits();
                    m_creature->SetFacingToObject(pPlayer);
                    DoScriptText(SAY_WIL_END, m_creature, pPlayer);
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ESCAPE_COILSCAR, m_creature);
                }
                break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_COILSKAR_ASSASSIN)
        {
            if (Player* pPlayer = GetPlayerForEscort())
                pSummoned->AI()->AttackStart(pPlayer);
        }
    }

    // wrapper to spawn assassin and do text
    void DoSpawnAssassin(uint8 uiCount = 1)
    {
        // unknown where they actually appear
        float fX, fY, fZ;
        for (uint8 i = 0; i < uiCount; ++i)
        {
            m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 10.0f, fX, fY, fZ);
            m_creature->SummonCreature(NPC_COILSKAR_ASSASSIN, fX, fY, fZ, 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 10000);
        }

        // random chance to yell
        if (roll_chance_i(20))
            return;

        // random text when assassin is summoned
        switch (urand(0, 6))
        {
            case 0: DoScriptText(SAY_WIL_PROGRESS_1, m_creature); break;
            case 1: DoScriptText(SAY_WIL_PROGRESS_2, m_creature); break;
            case 2: DoScriptText(SAY_WIL_PROGRESS_3, m_creature); break;
            case 3: DoScriptText(SAY_WIL_PROGRESS_4, m_creature); break;
            case 4: DoScriptText(SAY_WIL_PROGRESS_5, m_creature); break;
            case 5: DoScriptText(SAY_WIL_AGGRO_1, m_creature); break;
            case 6: DoScriptText(SAY_WIL_AGGRO_2, m_creature); break;
        }
    }

    // free the water spirits
    void DoFreeSpirits()
    {
        CreatureList lSpiritsInRange;
        GetCreatureListWithEntryInGrid(lSpiritsInRange, m_creature, NPC_CAPTURED_WATER_SPIRIT, 50.0f);

        if (lSpiritsInRange.empty())
            return;

        // all spirits follow
        for (Creature* spirit : lSpiritsInRange)
        {
            spirit->RemoveAurasDueToSpell(SPELL_WATER_BUBBLE);
            spirit->StopMoving();
            spirit->GetMotionMaster()->MoveFollow(m_creature, m_creature->GetDistance(spirit) * 0.25f, M_PI_F / 2 + m_creature->GetAngle(spirit), true);
            spirit->SetFactionTemporary(FACTION_ESCORT_N_FRIEND_ACTIVE, TEMPFACTION_RESTORE_RESPAWN);
            spirit->SetLevitate(false);
        }
    }

    void DoDespawnSpirits()
    {
        CreatureList lSpiritsInRange;
        GetCreatureListWithEntryInGrid(lSpiritsInRange, m_creature, NPC_CAPTURED_WATER_SPIRIT, 50.0f);

        if (lSpiritsInRange.empty())
            return;

        // all spirits follow
        for (CreatureList::const_iterator itr = lSpiritsInRange.begin(); itr != lSpiritsInRange.end(); ++itr)
        {
            (*itr)->ForcedDespawn(6000);
            (*itr)->SetLevitate(true);
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiLightningTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CHAIN_LIGHTNING) == CAST_OK)
                m_uiLightningTimer = 4000;
        }
        else
            m_uiLightningTimer -= uiDiff;

        if (m_uiShockTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FROST_SHOCK) == CAST_OK)
                m_uiShockTimer = 10000;
        }
        else
            m_uiShockTimer -= uiDiff;

        if (m_creature->GetHealthPercent() <= 30.0f)
        {
            if (m_uiHealingTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_HEALING_WAVE);
                m_uiHealingTimer = 15000;
            }
            else
                m_uiHealingTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_wilda(Creature* pCreature)
{
    return new npc_wildaAI(pCreature);
}

bool QuestAccept_npc_wilda(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPE_COILSCAR)
    {
        DoScriptText(SAY_WIL_START, pCreature, pPlayer);
        pCreature->SetFactionTemporary(FACTION_ESCORT_N_FRIEND_ACTIVE, TEMPFACTION_RESTORE_RESPAWN);
        pCreature->SetLevitate(false);

        if (npc_wildaAI* pEscortAI = dynamic_cast<npc_wildaAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }
    return true;
}

/*#####
# Quest: Battle of the Crimson Watch
#####*/

enum
{
    QUEST_BATTLE_OF_THE_CRIMSON_WATCH = 10781,

    EVENT_COOLDOWN = 30000,

    SAY_EVENT_START = -1015103,

    SAY_TORLOTH_DIALOGUE1 = -1000532,
    SAY_TORLOTH_DIALOGUE2 = -1000533,
    SAY_TORLOTH_DIALOGUE3 = -1000534,
    SAY_ILLIDAN_DIALOGUE = -1000535,
    SAY_ILLIDAN_SUMMON1 = -1000536,
    SAY_ILLIDAN_SUMMON2 = -1000537,
    SAY_ILLIDAN_SUMMON3 = -1000538,
    SAY_ILLIDAN_SUMMON4 = -1000539,
    SAY_EVENT_COMPLETED = -1000540,

    MODEL_ID_FELGUARD = 18654,
    MODEL_ID_DREADLORD = 19991,

    NPC_ILLIDARI_SOLDIER = 22075,
    NPC_ILLIDARI_MIND_BREAKER = 22074,
    NPC_ILLIDARI_HIGHLORD = 19797,
    NPC_TORLOTH_THE_MAGNIFICENT = 22076,
    NPC_LORD_ILLIDAN = 22083,
    NPC_MARCUS_AURALION = 22073,
};

enum CinematicCreature
{
    LORD_ILLIDAN = 1,
    TORLOTH = 0
};

const float EVENT_AREA_RADIUS = 65.0;

struct TorlothCinematic
{
    int32  iTextId;
    uint32 uiCreature;
    uint32 uiTimer;
};

static TorlothCinematic TorlothAnim[] =
{
    {SAY_TORLOTH_DIALOGUE1, TORLOTH, 2000},
    {SAY_ILLIDAN_DIALOGUE, LORD_ILLIDAN, 7000},
    {SAY_TORLOTH_DIALOGUE2, TORLOTH, 3000},
    {0, TORLOTH, 1000},                                 // Torloth stand
    {SAY_TORLOTH_DIALOGUE3, TORLOTH, 7000},
    {0, TORLOTH, 0},                                    // Torloth attacks
};

struct Location
{
    float fLocX;
    float fLocY;
    float fLocZ;
    float fOrient;
};

static Location SpawnLocation[] =
{
    { -4615.8556f, 1342.2532f, 139.9f, 1.612f},             // Illidari Soldier
    { -4598.9365f, 1377.3182f, 139.9f, 3.917f},             // Illidari Soldier
    { -4598.4697f, 1360.8999f, 139.9f, 2.427f},             // Illidari Soldier
    { -4589.3599f, 1369.1061f, 139.9f, 3.165f},             // Illidari Soldier
    { -4608.3477f, 1386.0076f, 139.9f, 4.108f},             // Illidari Soldier
    { -4633.1889f, 1359.8033f, 139.9f, 0.949f},             // Illidari Soldier
    { -4623.5791f, 1351.4574f, 139.9f, 0.971f},             // Illidari Soldier
    { -4607.2988f, 1351.6099f, 139.9f, 2.416f},             // Illidari Soldier
    { -4633.7764f, 1376.0417f, 139.9f, 5.608f},             // Illidari Soldier
    { -4600.2461f, 1369.1240f, 139.9f, 3.056f},             // Illidari Mind Breaker
    { -4631.7808f, 1367.9459f, 139.9f, 0.020f},             // Illidari Mind Breaker
    { -4600.2461f, 1369.1240f, 139.9f, 3.056f},             // Illidari Highlord
    { -4631.7808f, 1367.9459f, 139.9f, 0.020f},             // Illidari Highlord
    { -4615.5586f, 1353.0031f, 139.9f, 1.540f},             // Illidari Highlord
    { -4616.4736f, 1384.2170f, 139.9f, 4.971f},             // Illidari Highlord
    { -4627.1240f, 1378.8752f, 139.9f, 2.544f}              // Torloth The Magnificent
};

struct WaveData
{
    uint8  uiSpawnCount;
    uint8  uiUsedSpawnPoint;
    uint32 uiCreatureId;
    uint32 uiSpawnTimer;
    uint32 uiYellTimer;
    int32  iTextId;
};

static WaveData WavesInfo[] =
{
    // Illidari Soldier
    {9, 0, NPC_ILLIDARI_SOLDIER, 10000, 7000, SAY_ILLIDAN_SUMMON1},
    // Illidari Mind Breaker
    {2, 9, NPC_ILLIDARI_MIND_BREAKER, 10000, 7000, SAY_ILLIDAN_SUMMON2},
    // Illidari Highlord
    {4, 11, NPC_ILLIDARI_HIGHLORD, 10000, 7000, SAY_ILLIDAN_SUMMON3},
    // Torloth The Magnificent
    {1, 15, NPC_TORLOTH_THE_MAGNIFICENT, 10000, 7000, SAY_ILLIDAN_SUMMON4}
};

/*######
# mob_torloth
#####*/

enum
{
    SPELL_CLEAVE = 15284,
    SPELL_SHADOWFURY = 39082,
    SPELL_SPELL_REFLECTION = 33961,

    SPELL_SPIRIT_PARTICLES = 37119,
    SPELL_SHADOWFORM = 37816,

    FACTION_HOSTILE = 14,
};

enum TorlothActions
{
    TORLOTH_CLEAVE,
    TORLOTH_SHADOWFURY,
    TORLOTH_SPELL_REFLECTION,
    TORLOTH_ACTION_MAX,
    TORLOTH_ANIMATIONS,
};

struct mob_torlothAI : public CombatAI
{
    mob_torlothAI(Creature* creature) : CombatAI(creature, TORLOTH_ACTION_MAX)
    {
        AddCombatAction(TORLOTH_CLEAVE, 10000u);
        AddCombatAction(TORLOTH_SHADOWFURY, 18000u);
        AddCombatAction(TORLOTH_SPELL_REFLECTION, 25000u);
        AddCustomAction(TORLOTH_ANIMATIONS, 4000u, [&]() {HandleAnimation(); });
    }

    ObjectGuid m_lordIllidanGuid;
    ObjectGuid m_playerGuid;

    uint8 m_uiAnimationCount;

    void Reset() override
    {
        CombatAI::Reset();
        m_lordIllidanGuid.Clear();
        m_playerGuid.Clear();

        m_uiAnimationCount = 0;

        // make him not attackable for the time of animation
        SetCombatMovement(false);
    }

    void FailEvent()
    {
        m_creature->ForcedDespawn();
        if (Unit* spawner = m_creature->GetSpawner()) // get controller of event
            SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, spawner); // fail
    }

    void JustRespawned() override
    {
        CombatAI::JustRespawned();
        DoCastSpellIfCan(nullptr, SPELL_SPIRIT_PARTICLES, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_SHADOWFORM, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void EnterEvadeMode() override
    {
        CombatAI::EnterEvadeMode(); // cleanup
        FailEvent();
    }

    void HandleAnimation()
    {
        Creature* creature = m_creature;

        if (TorlothAnim[m_uiAnimationCount].uiCreature == LORD_ILLIDAN)
        {
            creature = m_creature->GetMap()->GetCreature(m_lordIllidanGuid);

            if (!creature)
            {
                m_creature->ForcedDespawn();
                return;
            }
        }

        if (TorlothAnim[m_uiAnimationCount].iTextId)
            DoScriptText(TorlothAnim[m_uiAnimationCount].iTextId, creature);

        switch (m_uiAnimationCount)
        {
            case 0:
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case 3:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                if (Player* target = m_creature->GetMap()->GetPlayer(m_playerGuid))
                    m_creature->SetFacingToObject(target);
                break;
            case 5:
            {
                SetCombatMovement(true);
                m_creature->setFaction(FACTION_HOSTILE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                if (Player* target = m_creature->GetMap()->GetPlayer(m_playerGuid))
                    AttackStart(target);
                else
                    FailEvent();
                break;
            }
        }

        if (uint32 timer = TorlothAnim[m_uiAnimationCount].uiTimer)
            ResetTimer(TORLOTH_ANIMATIONS, timer);
        ++m_uiAnimationCount;
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Player* player = m_creature->GetMap()->GetPlayer(m_playerGuid))
        {
            player->RewardPlayerAndGroupAtEventExplored(QUEST_BATTLE_OF_THE_CRIMSON_WATCH, m_creature);

            if (Creature* lordIllidan = m_creature->GetMap()->GetCreature(m_lordIllidanGuid))
            {
                DoScriptText(SAY_EVENT_COMPLETED, lordIllidan, player);
                lordIllidan->AI()->EnterEvadeMode();
            }
        }
        if (Unit* spawner = m_creature->GetSpawner()) // get controller of event
            SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, spawner); // success
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case TORLOTH_CLEAVE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                    ResetCombatAction(action, 15000);
                break;
            case TORLOTH_SHADOWFURY:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOWFURY) == CAST_OK)
                    ResetCombatAction(action, 20000);
                break;
            case TORLOTH_SPELL_REFLECTION:
                if (DoCastSpellIfCan(nullptr, SPELL_SPELL_REFLECTION) == CAST_OK)
                    ResetCombatAction(action, 30000);
                break;
        }
    }
};

/*#####
# npc_lord_illidan_stormrage
#####*/

struct npc_lord_illidan_stormrageAI : public Scripted_NoMovementAI
{
    npc_lord_illidan_stormrageAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) {}

    GuidVector m_spawns;
    ObjectGuid m_playerGuid;
    uint32 m_uiWaveTimer;
    uint32 m_uiAnnounceTimer;
    uint32 m_uiCheckTimer;
    uint8  m_uiMobCount;
    uint8  m_uiWaveCount;

    bool m_bEventStarted;
    bool m_bWaveAnnounced;

    void Reset() override
    {
        m_playerGuid.Clear();

        m_uiWaveTimer = 10000;
        m_uiAnnounceTimer = 7000;
        m_uiCheckTimer = 2000;

        m_uiMobCount = 0;
        m_uiWaveCount = 0;

        m_bEventStarted = false;
        m_bWaveAnnounced = false;

        m_spawns.clear();
    }

    void StartEvent(Player* player)
    {
        m_bEventStarted = true;
        m_playerGuid = player->GetObjectGuid();
        if (Creature* marcus = GetClosestCreatureWithEntry(m_creature, NPC_MARCUS_AURALION, 20.f))
            DoScriptText(SAY_EVENT_START, marcus, player);
    }

    void FailEvent()
    {
        DespawnGuids(m_spawns);
        Reset();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A) // failed - sent by torloth
            FailEvent();
        else if (eventType == AI_EVENT_CUSTOM_B) // success
            Reset();
    }

    void SummonWave()
    {
        uint8 uiCount = WavesInfo[m_uiWaveCount].uiSpawnCount;
        uint8 uiLocIndex = WavesInfo[m_uiWaveCount].uiUsedSpawnPoint;
        uint8 uiFelguardCount = 0;
        uint8 uiDreadlordCount = 0;

        for (uint8 i = 0; i < uiCount; ++i)
        {
            float fLocX = SpawnLocation[uiLocIndex + i].fLocX;
            float fLocY = SpawnLocation[uiLocIndex + i].fLocY;
            float fLocZ = SpawnLocation[uiLocIndex + i].fLocZ;
            float fOrient = SpawnLocation[uiLocIndex + i].fOrient;

            if (Creature* pSpawn = m_creature->SummonCreature(WavesInfo[m_uiWaveCount].uiCreatureId, fLocX, fLocY, fLocZ, fOrient, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000))
            {
                if (m_uiWaveCount)                          // only in first wave
                    continue;

                if (!urand(0, 2) && uiFelguardCount < 2)
                {
                    pSpawn->SetDisplayId(MODEL_ID_FELGUARD);
                    ++uiFelguardCount;
                }
                else if (uiDreadlordCount < 3)
                {
                    pSpawn->SetDisplayId(MODEL_ID_DREADLORD);
                    ++uiDreadlordCount;
                }
                else if (uiFelguardCount < 2)
                {
                    pSpawn->SetDisplayId(MODEL_ID_FELGUARD);
                    ++uiFelguardCount;
                }
            }
        }

        ++m_uiWaveCount;
        m_uiWaveTimer = WavesInfo[m_uiWaveCount].uiSpawnTimer;
        m_uiAnnounceTimer = WavesInfo[m_uiWaveCount].uiYellTimer;
    }

    void JustSummoned(Creature* summoned) override
    {
        // increment mob count
        ++m_uiMobCount;

        if (!m_playerGuid)
            return;

        if (summoned->GetEntry() == NPC_TORLOTH_THE_MAGNIFICENT)
        {
            if (mob_torlothAI* pTorlothAI = dynamic_cast<mob_torlothAI*>(summoned->AI()))
            {
                pTorlothAI->m_lordIllidanGuid = m_creature->GetObjectGuid();
                pTorlothAI->m_playerGuid = m_playerGuid;
            }
        }
        else
        {
            if (Player* target = m_creature->GetMap()->GetPlayer(m_playerGuid))
                summoned->AI()->AttackStart(target);
        }
    }

    void SummonedCreatureJustDied(Creature* creature) override
    {
        if (creature->GetEntry() != NPC_TORLOTH_THE_MAGNIFICENT)
        {
            // decrement mob count
            --m_uiMobCount;

            if (!m_uiMobCount)
                m_bWaveAnnounced = false;
        }
    }

    void CheckEventFail()
    {
        Player* player = m_creature->GetMap()->GetPlayer(m_playerGuid);

        if (!player)
            return;

        if (!player->CheckForGroup([&](Player const* player) -> bool
            {
                if (!player->IsAlive())
                    return false;

                if (player->GetQuestStatus(QUEST_BATTLE_OF_THE_CRIMSON_WATCH) != QUEST_STATUS_INCOMPLETE)
                    return false;

                if (!player->IsWithinDistInMap(m_creature, EVENT_AREA_RADIUS))
                    return false;

                return true;
            }))
        {
            player->FailQuestForGroup(QUEST_BATTLE_OF_THE_CRIMSON_WATCH);
            FailEvent();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_playerGuid || !m_bEventStarted)
            return;

        if (!m_uiMobCount && m_uiWaveCount < 4)
        {
            if (!m_bWaveAnnounced && m_uiAnnounceTimer < uiDiff)
            {
                DoScriptText(WavesInfo[m_uiWaveCount].iTextId, m_creature);
                m_bWaveAnnounced = true;
            }
            else
                m_uiAnnounceTimer -= uiDiff;

            if (m_uiWaveTimer < uiDiff)
                SummonWave();
            else
                m_uiWaveTimer -= uiDiff;
        }

        if (m_uiCheckTimer < uiDiff)
        {
            CheckEventFail();
            m_uiCheckTimer = 2000;
        }
        else
            m_uiCheckTimer -= uiDiff;
    }
};

/*#####
# go_crystal_prison : GameObject that begins the event and hands out quest
######*/
bool GOQuestAccept_GO_crystal_prison(Player* player, GameObject* /*go*/, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_BATTLE_OF_THE_CRIMSON_WATCH)
        if (Creature* lordIllidan = GetClosestCreatureWithEntry(player, NPC_LORD_ILLIDAN, 50.0))
            if (npc_lord_illidan_stormrageAI* illidanAI = dynamic_cast<npc_lord_illidan_stormrageAI*>(lordIllidan->AI()))
                if (!illidanAI->m_bEventStarted)
                    illidanAI->StartEvent(player);

    return true;
}

/*######
## npc_totem_of_spirits
######*/

enum
{
    QUEST_SPIRITS_FIRE_AND_EARTH        = 10458,
    QUEST_SPIRITS_WATER                 = 10480,
    QUEST_SPIRITS_AIR                   = 10481,

    // quest 10458, 10480, 10481
    SPELL_ELEMENTAL_SIEVE               = 36035,
    SPELL_CALL_TO_THE_SPIRITS           = 36206,

    SPELL_EARTH_CAPTURED                = 36025,            // dummies (having visual effects)
    SPELL_FIERY_CAPTURED                = 36115,
    SPELL_WATER_CAPTURED                = 36170,
    SPELL_AIR_CAPTURED                  = 36181,

    SPELL_EARTH_CAPTURED_CREDIT         = 36108,            // event 13513
    SPELL_FIERY_CAPTURED_CREDIT         = 36117,            // event 13514
    SPELL_WATER_CAPTURED_CREDIT         = 36171,            // event 13515
    SPELL_AIR_CAPTURED_CREDIT           = 36182,            // event 13516

    NPC_TOTEM_OF_SPIRITS                = 21071,
    NPC_EARTH_SPIRIT                    = 21050,            // to be killed
    NPC_FIERY_SPIRIT                    = 21061,
    NPC_WATER_SPIRIT                    = 21059,
    NPC_AIR_SPIRIT                      = 21060,

    NPC_EARTHEN_SOUL                    = 21073,            // invisible souls summoned by the totem
    NPC_FIERY_SOUL                      = 21097,
    NPC_WATERY_SOUL                     = 21109,
    NPC_AIRY_SOUL                       = 21116,

    NPC_CREDIT_MARKER_EARTH             = 21092,            // quest objective npc's
    NPC_CREDIT_MARKER_FIERY             = 21094,
    NPC_CREDIT_MARKER_WATER             = 21095,
    NPC_CREDIT_MARKER_AIR               = 21096,

    EVENT_EARTH                         = 13513,            // credit events
    EVENT_FIERY                         = 13514,
    EVENT_WATER                         = 13515,
    EVENT_AIR                           = 13516,
};

struct npc_totem_of_spiritsAI : public ScriptedAI
{
    npc_totem_of_spiritsAI(Creature* creature) : ScriptedAI(creature)
    {
        SetReactState(REACT_PASSIVE);
        Reset();
        m_uiElementalSieveTimer = 2500; // needs to be cast non-stop without interference from evade and some such
    }

    uint32 m_uiElementalSieveTimer;

    void Reset() override {}

    void SummonedMovementInform(Creature* summoned, uint32 /*motionType*/, uint32 /*data*/) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_EARTHEN_SOUL:
                summoned->CastSpell(m_creature, SPELL_EARTH_CAPTURED, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_FIERY_SOUL:
                summoned->CastSpell(m_creature, SPELL_FIERY_CAPTURED, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_WATERY_SOUL:
                summoned->CastSpell(m_creature, SPELL_WATER_CAPTURED, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_AIRY_SOUL:
                summoned->CastSpell(m_creature, SPELL_AIR_CAPTURED, TRIGGERED_OLD_TRIGGERED);
                break;
        }

        // Despawn the spirit soul after it's captured
        summoned->ForcedDespawn(1000);
    }

    void JustSummoned(Creature* summoned) override
    {
        // After summoning the spirit soul, make it move towards the totem
        summoned->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 4);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_uiElementalSieveTimer <= diff)
        {
            m_creature->CastSpell(nullptr, SPELL_ELEMENTAL_SIEVE, TRIGGERED_OLD_TRIGGERED);
            m_uiElementalSieveTimer = 2500;
        }
        else
            m_uiElementalSieveTimer -= diff;
    }
};

bool EffectDummyCreature_npc_totem_of_spirits(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiEffIndex != EFFECT_INDEX_0)
        return false;

    switch (uiSpellId)
    {
        case SPELL_EARTH_CAPTURED:
        {
            pCreatureTarget->CastSpell(pCreatureTarget, SPELL_EARTH_CAPTURED_CREDIT, TRIGGERED_OLD_TRIGGERED);
            return true;
        }
        case SPELL_FIERY_CAPTURED:
        {
            pCreatureTarget->CastSpell(pCreatureTarget, SPELL_FIERY_CAPTURED_CREDIT, TRIGGERED_OLD_TRIGGERED);
            return true;
        }
        case SPELL_WATER_CAPTURED:
        {
            pCreatureTarget->CastSpell(pCreatureTarget, SPELL_WATER_CAPTURED_CREDIT, TRIGGERED_OLD_TRIGGERED);
            return true;
        }
        case SPELL_AIR_CAPTURED:
        {
            pCreatureTarget->CastSpell(pCreatureTarget, SPELL_AIR_CAPTURED_CREDIT, TRIGGERED_OLD_TRIGGERED);
            return true;
        }
    }

    return false;
}

bool ProcessEventId_event_spell_soul_captured_credit(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (bIsStart && pSource->GetTypeId() == TYPEID_UNIT)
    {
        Player* pOwner = (Player*)((Creature*)pSource)->GetOwner();

        if (!pOwner)
            return true;

        switch (uiEventId)
        {
            case EVENT_EARTH:
                pOwner->KilledMonsterCredit(NPC_CREDIT_MARKER_EARTH);
                return true;
            case EVENT_FIERY:
                pOwner->KilledMonsterCredit(NPC_CREDIT_MARKER_FIERY);
                return true;
            case EVENT_WATER:
                pOwner->KilledMonsterCredit(NPC_CREDIT_MARKER_WATER);
                return true;
            case EVENT_AIR:
                pOwner->KilledMonsterCredit(NPC_CREDIT_MARKER_AIR);
                return true;
        }
    }

    return false;
}

/*#####
#npc_shadowlord_deathwail
#####*/

struct WaveCoords
{
    float xCoord, yCoord, zCoord;
};

const static WaveCoords WaveSpawnCoords[3][3] =
{
    {
        /////////////GROUP 1//////////// 0
        {-3220.212f, 257.0669f, 139.0887f},
        {-3214.421f, 253.1379f, 139.1302f},
        {-3218.280f, 248.6299f, 139.1302f},
    },
    {
        /////////////GROUP 2//////////// 1
        {-3256.545f, 260.2362f, 137.1539f},
        {-3253.961f, 257.7454f, 137.1894f},
        {-3258.832f, 256.8369f, 137.1468f},
    },
    {
        /////////////GROUP 3//////////// 2
        {-3219.986f, 259.6718f, 139.0960f},
        {-3214.978f, 256.4007f, 139.1302f},
        {-3217.333f, 252.0130f, 139.1302f},
    }
};

const static WaveCoords* WaveGroupOneSpawnCoords	= WaveSpawnCoords[0];
const static WaveCoords* WaveGroupTwoSpawnCoords	= WaveSpawnCoords[1];
const static WaveCoords* WaveGroupThreeSpawnCoords	= WaveSpawnCoords[2];

const static WaveCoords DeathwailDescentCoords[] =
{
    {-3245.675f, 298.3688f, 171.9848f},
    {-3251.500f, 301.2324f, 167.3459f},
    {-3258.824f, 302.1381f, 162.9848f},
    {-3267.146f, 297.6195f, 157.9292f},
    {-3266.891f, 283.9230f, 155.4848f},
    {-3261.760f, 278.8767f, 150.5682f},
    {-3255.110f, 278.1188f, 147.8738f},
    {-3248.274f, 278.3419f, 143.6453f},
    {-3245.108f, 287.5689f, 142.7843f},
    {-3248.166f, 289.8274f, 139.0618f},
};

enum
{
    SAY_BEGIN_DESCENT			= -1015002,
    SAY_HEART_RETRIEVED			= -1015003,
    SAY_DEAD					= -1015004,

    NPC_SHADOWLORD_DEATHWAIL	= 22006,
    NPC_HOF_VISUAL_TRIGGER		= 22058,
    NPC_SHADOWMOON_SOULSTEALER	= 22061,
    NPC_DEATHWAIL_VISUAL_TRIG	= 22096,
    NPC_SHADOWMOON_RETAINER		= 22102,
    NPC_FELFIRE_SUMMONER		= 22121,

    GOBJECT_HEART_OF_FURY		= 185125,

    SPELL_SHADOW_BOLT			= 12471,
    SPELL_SHADOW_BOLT_VOLLEY	= 15245,
    SPELL_FEAR					= 27641,
    SPELL_FEL_FIREBALL			= 38312,
    SPELL_SUMMON_FEL_FIRE		= 38375,

    FEAR_CD					= 20000,
    FEL_FIREBALL_CD			= 6000,
    SHADOW_BOLT_CD			= 4000,
    SHADOW_BOLT_VOLLEY_CD	= 10000,
    PLAYER_CHECK_CD			= 2000,
    RETAINER_WAVE_CD		= 100000,

    MAX_PLAYER_DISTANCE		= 100,

    RETAINER_DESPAWN_TIME	= 180000,
    DEATHWAIL_DESPAWN_TIME	= 300000
};

struct npc_shadowlord_deathwailAI : public ScriptedAI
{
    npc_shadowlord_deathwailAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    const static uint8 WaveCoordOffset = 11;

    ObjectGuid m_playerGuid;

    CreatureList m_lSoulstealers;

    Creature* m_cHOFVisualTrigger;
    Creature* m_cDeathwailTrigger;

    uint32 m_uiFelFireballTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiShadowBoltVolleyTimer;
    uint32 m_uiPlayerCheckTimer;
    uint32 m_uiPeriodicWaveTimer;
    uint32 m_uiDeathwailDespawnTimer;

    bool m_bEventInProgress = false;
    bool m_bDeathwailGrounded;

    void Reset() override
    {
        if (m_bEventInProgress && !m_bDeathwailGrounded)
            return;

        m_playerGuid = ObjectGuid();
        m_lSoulstealers.clear();
        m_cHOFVisualTrigger = nullptr;
        m_cDeathwailTrigger = nullptr;

        m_uiFelFireballTimer		= FEL_FIREBALL_CD;
        m_uiFearTimer				= 8000 + urand(0, 5000);
        m_uiShadowBoltTimer			= SHADOW_BOLT_CD;
        m_uiShadowBoltVolleyTimer	= SHADOW_BOLT_VOLLEY_CD;
        m_uiPlayerCheckTimer		= PLAYER_CHECK_CD;
        m_uiPeriodicWaveTimer		= RETAINER_WAVE_CD;
        m_uiDeathwailDespawnTimer	= DEATHWAIL_DESPAWN_TIME;
    }

    void JustRespawned() override
    {
        m_bDeathwailGrounded = false;
        m_bEventInProgress = false;
        SetReactState(REACT_PASSIVE);
        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
        m_creature->SetLevitate(true);
        SetDeathPrevention(true);
        Reset();
    }

    void DoReleaseSoulstealersAndReset()
    {
        for (Creature* soulstealer : m_lSoulstealers)
            if (soulstealer)
            {
                soulstealer->CombatStop();
                soulstealer->SetActiveObjectState(false);
            }

        m_bEventInProgress = false;

        m_creature->SetActiveObjectState(false);
        Reset();
    }

    void DoSummonWave(bool both = false)
    {
        if (both)
        {
            // Wave starting indoors
            m_creature->SummonCreature(NPC_SHADOWMOON_RETAINER, WaveGroupOneSpawnCoords[0].xCoord, WaveGroupOneSpawnCoords[0].yCoord, WaveGroupOneSpawnCoords[0].zCoord, 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, RETAINER_DESPAWN_TIME, true, true, 1);
            m_creature->SummonCreature(NPC_SHADOWMOON_RETAINER, WaveGroupOneSpawnCoords[1].xCoord, WaveGroupOneSpawnCoords[1].yCoord, WaveGroupOneSpawnCoords[1].zCoord, 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, RETAINER_DESPAWN_TIME, true, true, 2);
            m_creature->SummonCreature(NPC_SHADOWMOON_RETAINER, WaveGroupOneSpawnCoords[2].xCoord, WaveGroupOneSpawnCoords[2].yCoord, WaveGroupOneSpawnCoords[2].zCoord, 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, RETAINER_DESPAWN_TIME, true, true, 3);

            // Wave starting outdoors
            m_creature->SummonCreature(NPC_SHADOWMOON_RETAINER, WaveGroupTwoSpawnCoords[0].xCoord, WaveGroupTwoSpawnCoords[0].yCoord, WaveGroupTwoSpawnCoords[0].zCoord, 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, RETAINER_DESPAWN_TIME, true, true, 4);
            m_creature->SummonCreature(NPC_SHADOWMOON_RETAINER, WaveGroupTwoSpawnCoords[1].xCoord, WaveGroupTwoSpawnCoords[1].yCoord, WaveGroupTwoSpawnCoords[1].zCoord, 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, RETAINER_DESPAWN_TIME, true, true, 5);
            m_creature->SummonCreature(NPC_SHADOWMOON_RETAINER, WaveGroupTwoSpawnCoords[2].xCoord, WaveGroupTwoSpawnCoords[2].yCoord, WaveGroupTwoSpawnCoords[2].zCoord, 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, RETAINER_DESPAWN_TIME, true, true, 6);
        }
        else
        {
            m_creature->SummonCreature(NPC_SHADOWMOON_RETAINER, WaveGroupThreeSpawnCoords[0].xCoord, WaveGroupThreeSpawnCoords[0].yCoord, WaveGroupThreeSpawnCoords[0].zCoord, 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, RETAINER_DESPAWN_TIME, true, true, 1);
            m_creature->SummonCreature(NPC_SHADOWMOON_RETAINER, WaveGroupThreeSpawnCoords[1].xCoord, WaveGroupThreeSpawnCoords[1].yCoord, WaveGroupThreeSpawnCoords[1].zCoord, 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, RETAINER_DESPAWN_TIME, true, true, 2);
            m_creature->SummonCreature(NPC_SHADOWMOON_RETAINER, WaveGroupThreeSpawnCoords[2].xCoord, WaveGroupThreeSpawnCoords[2].yCoord, WaveGroupThreeSpawnCoords[2].zCoord, 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, RETAINER_DESPAWN_TIME, true, true, 3);
        }
    }

    void DoBeginDescent()
    {
        m_bDeathwailGrounded = true;
        m_creature->GetMotionMaster()->MovePoint(0, DeathwailDescentCoords[0].xCoord, DeathwailDescentCoords[0].yCoord, DeathwailDescentCoords[0].zCoord);
        DoScriptText(SAY_BEGIN_DESCENT, m_creature);
        SetDeathPrevention(false);
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE)
            return;

        switch (pointId)
        {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
                m_creature->GetMotionMaster()->MovePoint(pointId + 1, DeathwailDescentCoords[pointId + 1].xCoord, DeathwailDescentCoords[pointId + 1].yCoord, DeathwailDescentCoords[pointId + 1].zCoord);
                break;
            case 9:
                DoScriptText(SAY_HEART_RETRIEVED, m_creature);
                SetReactState(REACT_AGGRESSIVE);
                m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
                m_creature->SetLevitate(false);

                if (GameObject* goHoF = GetClosestGameObjectWithEntry(m_creature, GOBJECT_HEART_OF_FURY, 30.0f))
                {
                    goHoF->SetRespawnTime(5 * MINUTE);
                    goHoF->Refresh();
                }
                if (!m_cHOFVisualTrigger)
                    m_cHOFVisualTrigger = GetClosestCreatureWithEntry(m_creature, NPC_HOF_VISUAL_TRIGGER, 175.0f);
                if (m_cHOFVisualTrigger)
                    m_cHOFVisualTrigger->ForcedDespawn();

                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveIdle();
                break;
        }
    }

    // true = event started normally
    // false = something's wrong, ignore
    bool SoulstealerEnteredCombat(Creature* /*unit*/, Unit* attacker)
    {
        if (!m_bEventInProgress)
        {
            // Begin event
            m_bEventInProgress = true;
            m_creature->SetActiveObjectState(true);

            m_cHOFVisualTrigger = GetClosestCreatureWithEntry(m_creature, NPC_HOF_VISUAL_TRIGGER, 175.0f);
            m_cDeathwailTrigger = GetClosestCreatureWithEntry(m_creature, NPC_DEATHWAIL_VISUAL_TRIG, 175.0f);

            CreatureList lOtherChannelers;
            GetCreatureListWithEntryInGrid(lOtherChannelers, m_creature, NPC_SHADOWMOON_SOULSTEALER, 175.0f);

            for (auto& lOtherChanneler : lOtherChannelers)
                if (lOtherChanneler->IsAlive())
                {
                    lOtherChanneler->AI()->AttackStart(attacker);
                    lOtherChanneler->SetActiveObjectState(true);

                    // agro on party members
                    if (Player* player = m_creature->GetMap()->GetPlayer(attacker->GetObjectGuid()))
                        if (Group* group = player->GetGroup())
                            for (GroupReference* ref = group->GetFirstMember(); ref != nullptr; ref = ref->next())
                            {
                                Player* member = ref->getSource();
                                if (member && member->IsAlive() && m_cHOFVisualTrigger && m_cHOFVisualTrigger->IsWithinDistInMap(member, MAX_PLAYER_DISTANCE))
                                    lOtherChanneler->AI()->AttackStart(member);
                            }

                    m_lSoulstealers.push_back(lOtherChanneler);
                }

            m_playerGuid = attacker->GetObjectGuid();
            DoSummonWave();
        }
        else if (m_bDeathwailGrounded)
            return false; // possbile edge case

        return true;
    }

    void SoulstealerDied(Creature* unit)
    {
        m_lSoulstealers.remove(unit);

        if (m_bEventInProgress && !m_bDeathwailGrounded && m_lSoulstealers.empty())
            DoBeginDescent();
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEAD, m_creature);
        m_bEventInProgress = false;
        m_creature->SetActiveObjectState(false);
    }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        if (pSpellEntry->Id == SPELL_FEL_FIREBALL && pTarget->GetEntry() == NPC_DEATHWAIL_VISUAL_TRIG)
            if (Creature* summoner = GetClosestCreatureWithEntry(m_creature, NPC_FELFIRE_SUMMONER, 175.0f))
            {
                summoner->CastSpell(pTarget, SPELL_SUMMON_FEL_FIRE, TRIGGERED_NONE);
                summoner->ForcedDespawn();
            }
    }

    /* Reset if:
     * Players run away (100 dist)
     * All players die
     * All players dead/dropped agro */
    bool IsPlayerOrGroupInRangeAndActiveInEvent()
    {
        for (Creature* soulstealer : m_lSoulstealers)
        {
            ThreatList const& threatList = soulstealer->getThreatManager().getThreatList();

            if (threatList.empty())
                return false;
        }

        if (Player* player = m_creature->GetMap()->GetPlayer(m_playerGuid)) // can find player
        {
            if (Group* group = player->GetGroup()) // player in group
            {
                for (GroupReference* ref = group->GetFirstMember(); ref != nullptr; ref = ref->next())
                {
                    Player* member = ref->getSource(); // any member gettable, alive & in range
                    if (member && member->IsAlive() && m_cHOFVisualTrigger && m_cHOFVisualTrigger->IsWithinDistInMap(member, MAX_PLAYER_DISTANCE))
                        return true;
                }
            }
            else // player alone
            {
                if (player->IsAlive() && m_cHOFVisualTrigger && m_cHOFVisualTrigger->IsWithinDistInMap(player, MAX_PLAYER_DISTANCE))
                    return true;
            }
        }

        return false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_bEventInProgress)
            return;

        if (m_bDeathwailGrounded)
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            {
                if (m_uiDeathwailDespawnTimer < uiDiff)
                {
                    m_creature->ForcedDespawn();
                    m_creature->SetRespawnTime(60);
                }
                else
                    m_uiDeathwailDespawnTimer -= uiDiff;

                return;
            }

            if (m_uiShadowBoltTimer < uiDiff)
            {

                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOW_BOLT) == CAST_OK)
                    m_uiShadowBoltTimer = SHADOW_BOLT_CD + urand(0, SHADOW_BOLT_CD);
            }
            else
                m_uiShadowBoltTimer -= uiDiff;

            if (m_uiShadowBoltVolleyTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOW_BOLT_VOLLEY) == CAST_OK)
                    m_uiShadowBoltVolleyTimer = SHADOW_BOLT_VOLLEY_CD + urand(0, SHADOW_BOLT_VOLLEY_CD);
            }
            else
                m_uiShadowBoltVolleyTimer -= uiDiff;

            if (m_uiFearTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FEAR) == CAST_OK)
                    m_uiFearTimer = FEAR_CD + urand(0, FEAR_CD);
            }
            else
                m_uiFearTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
        else
        {
            // Check if event should reset
            if (m_playerGuid)
            {
                if (m_uiPlayerCheckTimer < uiDiff)
                {
                    if (!IsPlayerOrGroupInRangeAndActiveInEvent())
                    {
                        DoReleaseSoulstealersAndReset();
                        return;
                    }

                    m_uiPlayerCheckTimer = PLAYER_CHECK_CD;
                }
                else
                    m_uiPlayerCheckTimer -= uiDiff;
            }
            else
            {
                DoReleaseSoulstealersAndReset();
                return;
            }

            if (m_uiFelFireballTimer < uiDiff)
            {
                if (m_cDeathwailTrigger)
                {
                    if (m_creature->CastSpell(m_cDeathwailTrigger, SPELL_FEL_FIREBALL, TRIGGERED_NONE) == SPELL_CAST_OK)
                    {
                        // TODO: creature summoning should be handled by spell, not here?
                        m_creature->SummonCreature(NPC_FELFIRE_SUMMONER,
                                                    m_cDeathwailTrigger->GetPositionX(),
                                                    m_cDeathwailTrigger->GetPositionY(),
                                                    m_cDeathwailTrigger->GetPositionZ(), 0.0f, TEMPSPAWN_TIMED_DESPAWN, 6000, true);
                    }
                }

                m_uiFelFireballTimer = FEL_FIREBALL_CD;
            }
            else
                m_uiFelFireballTimer -= uiDiff;

            if (m_uiPeriodicWaveTimer < uiDiff)
            {
                DoSummonWave();
                m_uiPeriodicWaveTimer = RETAINER_WAVE_CD;
            }
            else
                m_uiPeriodicWaveTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_shadowlord_deathwail(Creature* pCreature)
{
    return new npc_shadowlord_deathwailAI(pCreature);
}

/*#####
#mob_shadowmoon_soulstealer
#####*/

struct mob_shadowmoon_soulstealerAI : public ScriptedAI
{
    mob_shadowmoon_soulstealerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    Creature* m_cDeathwail;

    bool m_bSixtyTriggered;
    bool m_bTwentyTriggered;

    void Reset() override
    {
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        m_cDeathwail = nullptr;
        m_bSixtyTriggered = false;
        m_bTwentyTriggered = false;
    }

    void JustRespawned() override
    {
        m_creature->GetCombatManager().SetLeashingDisable(false);
        m_creature->SetActiveObjectState(false);
        m_creature->AI()->SetReactState(REACT_DEFENSIVE);

        Reset();
    }

    void EnterEvadeMode() override
    {
        m_creature->GetCombatManager().SetLeashingDisable(false);
        m_creature->AI()->SetReactState(REACT_DEFENSIVE);
        Reset();
    }

    void Aggro(Unit* who) override
    {
        m_cDeathwail = GetClosestCreatureWithEntry(m_creature, NPC_SHADOWLORD_DEATHWAIL, 200.0f);
        bool exitCombat = false;

        if (m_cDeathwail)
        {
            if (npc_shadowlord_deathwailAI* DeathwailAI = dynamic_cast<npc_shadowlord_deathwailAI*>(m_cDeathwail->AI()))
            {
                if (!who->GetObjectGuid().IsPlayer()) // not attacked by player
                {
                    if (Unit* attackerOwner = m_creature->GetMap()->GetPlayer(who->GetOwnerGuid()))
                        who = attackerOwner;
                    else // not attacked by player, nor NPC owned by player... just ignore
                        exitCombat = true;
                }
                if (!exitCombat && DeathwailAI->SoulstealerEnteredCombat(m_creature, who))
                {
                    m_creature->GetCombatManager().SetLeashingDisable(true);
                    m_creature->AI()->SetReactState(REACT_PASSIVE);
                }
                else
                    exitCombat = true;
            }
            else
                exitCombat = true;
        }
        else
            exitCombat = true;

        if (exitCombat)
            m_creature->CombatStop();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_cDeathwail)
            if (npc_shadowlord_deathwailAI* DeathwailAI = dynamic_cast<npc_shadowlord_deathwailAI*>(m_cDeathwail->AI()))
                DeathwailAI->SoulstealerDied(m_creature);
    }

    void UpdateAI(const uint32 /*uiDiff*/) override
    {
        if (!m_creature->SelectHostileTarget())
            return;

        if (!m_cDeathwail)
            return;

        if (!m_bSixtyTriggered)
        {
            if (m_creature->GetHealthPercent() <= 60.0f)
            {
                if (npc_shadowlord_deathwailAI* DeathwailAI = dynamic_cast<npc_shadowlord_deathwailAI*>(m_cDeathwail->AI()))
                {
                    DeathwailAI->DoSummonWave(true);
                    m_bSixtyTriggered = true;
                }
            }
        }
        else if (!m_bTwentyTriggered)
        {
            if (m_creature->GetHealthPercent() <= 20.0f)
            {
                if (npc_shadowlord_deathwailAI* DeathwailAI = dynamic_cast<npc_shadowlord_deathwailAI*>(m_cDeathwail->AI()))
                {
                    DeathwailAI->DoSummonWave(true);
                    m_bTwentyTriggered = true;
                }
            }
        }
    }
};

/*#####
#npc_spawned_oronok_tornheart
#####*/

enum
{
    // texts
    SAY_ORONOK_TOGETHER             = -1000803,
    SAY_ORONOK_READY                = -1000804,
    SAY_ORONOK_ELEMENTS             = -1000805,
    SAY_ORONOK_EPILOGUE_1           = -1000806,
    SAY_TORLOK_EPILOGUE_2           = -1000807,
    SAY_ORONOK_EPILOGUE_3           = -1000808,
    SAY_EARTH_EPILOGUE_4            = -1000809,
    SAY_FIRE_EPILOGUE_5             = -1000810,
    SAY_EARTH_EPILOGUE_6            = -1000811,
    SAY_ORONOK_EPILOGUE_7           = -1000812,
    EMOTE_GIVE_WEAPONS              = -1000813,
    SAY_ORONOK_EPILOGUE_8           = -1000814,

    GOSSIP_ITEM_FIGHT               = -3000109,
    GOSSIP_TEXT_ID_ORONOK           = 10421,

    // spells - some are already defined above
    // SPELL_CHAIN_LIGHTNING         = 16006,
    SPELL_EARTHBIND_TOTEM           = 15786,
    // SPELL_FROST_SHOCK             = 12548,
    // SPELL_HEALING_WAVE            = 12491,
    SPELL_ORONOK_SPEED_INCREASE     = 37246,
    SPELL_ELEMENTAL_SPAWN_IN        = 25035,

    // npcs
    NPC_ORONOK_TORN_HEART           = 21685,
    NPC_GROMTOR_SON_OF_ORONOK       = 21687,
    NPC_BORAK_SON_OF_ORONOK         = 21686,
    NPC_CYRUKH_THE_FIRELORD         = 21181,
    // NPC_EARTH_SPIRIT              = 21050,
    NPC_REDEEMED_SPIRIT_OF_EARTH    = 21739,
    NPC_REDEEMED_SPIRIT_OF_FIRE     = 21740,
    NPC_REDEEMED_SPIRIT_OF_AIR      = 21738,
    NPC_REDEEMED_SPIRIT_OF_WATER    = 21741,
    NPC_EARTHMENDER_TORLOK          = 21024,

    GO_MARK_OF_KAELTHAS             = 185170,

    QUEST_CIPHER_OF_DAMNATION       = 10588,

    POINT_ID_ATTACK_READY           = 1,
    POINT_ID_ELEMENTS               = 2,
    POINT_ID_EPILOGUE               = 3,
    WAYPOINT_ID_DESPAWN             = 5,
    WAYPOINT_ID_INTRO_FINAL         = 31,

    EVENT_STARTED_MOVING            = 1,
    EVENT_GIVE_WEAPONS              = 2,
    EVENT_MOUNT                     = 3,
    EVENT_HOVER_AND_MOVE            = 4,

    MOUNT_ID_ORONOK                 = 17721,

    FACTION_ORONOK_COMBAT           = 495,
    FACTION_ORONOK_FRIENDLY         = 35,

    PATH_ID_OUTRO                   = 2,
};

static const DialogueEntry aOutroDialogue[] =
{
    {QUEST_CIPHER_OF_DAMNATION,     0,                              32000},
    {NPC_CYRUKH_THE_FIRELORD,       0,                              0},
    {EVENT_STARTED_MOVING,          0,                              1000},
    {NPC_EARTHMENDER_TORLOK,        0,                              1000},
    {SAY_ORONOK_EPILOGUE_1,         NPC_ORONOK_TORN_HEART,          5000},
    {SAY_TORLOK_EPILOGUE_2,         NPC_EARTHMENDER_TORLOK,         5000},
    {NPC_REDEEMED_SPIRIT_OF_EARTH,  0,                              5000},
    {SAY_ORONOK_EPILOGUE_3,         NPC_ORONOK_TORN_HEART,          5000},
    {SAY_EARTH_EPILOGUE_4,          NPC_REDEEMED_SPIRIT_OF_EARTH,   5000},
    {SAY_FIRE_EPILOGUE_5,           NPC_REDEEMED_SPIRIT_OF_FIRE,    14000},
    {SAY_EARTH_EPILOGUE_6,          NPC_REDEEMED_SPIRIT_OF_EARTH,   6000},
    {SAY_ORONOK_EPILOGUE_7,         NPC_ORONOK_TORN_HEART,          6000},
    {SAY_ORONOK_EPILOGUE_8,         NPC_ORONOK_TORN_HEART,          1000},
    {EVENT_GIVE_WEAPONS,            0,                              8500},
    {EVENT_MOUNT,                   0,                              1000},
    {EVENT_HOVER_AND_MOVE,          0,                              1000},
    {NPC_ORONOK_TORN_HEART,         0,                              0},
    {0, 0, 0},
};

const static Position aDamnationLocations[] =
{
    { -3587.229f, 1892.889f, 47.32373f, 2.199115f}, // 0 air spirit summon loc
    { -3598.681f, 1888.016f, 47.32373f, 1.692969f}, // 1 earth spirit summon loc
    { -3605.315f, 1884.477f, 47.32373f, 1.308997f}, // 2 fire spirit summon loc
    { -3591.871f, 1886.822f, 47.32373f, 1.850049f}, // 3 water spirit summon loc
    { -3595.36f, 1869.78f, 47.24f},            // 4 fight ready move loc
    { -3635.90f, 1860.94f, 52.93f},            // 5 elementals move loc
    { -3599.71f, 1897.94f, 47.24f}             // 6 epilogue move loc
};

// TODO: Add formations and more shaking animations during intro
struct npc_spawned_oronok_tornheartAI : public ScriptedAI, private DialogueHelper
{
    npc_spawned_oronok_tornheartAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aOutroDialogue)
    {
        Reset();
        StartNextDialogueText(QUEST_CIPHER_OF_DAMNATION);
    }

    uint32 m_uiLightningTimer;
    uint32 m_uiTotemTimer;
    uint32 m_uiFrostTimer;
    uint32 m_uiHealTimer;

    ObjectGuid m_torlokGuid;
    ObjectGuid m_fireSpiritGuid;
    ObjectGuid m_earthSpiritGuid;
    ObjectGuid m_borakGuid;
    ObjectGuid m_gromtorGuid;
    ObjectGuid m_cyrukhGuid;

    Creature* m_borak;
    Creature* m_gromtor;

    bool m_bHasAttackStart;

    void Reset() override
    {
        m_uiLightningTimer  = 15000;
        m_uiTotemTimer      = 10000;
        m_uiFrostTimer      = 20000;
        m_uiHealTimer       = 8000;

        m_bHasAttackStart   = false;
    }

    void JustRespawned() override
    {
        m_creature->Mount(MOUNT_ID_ORONOK);
        m_creature->SetLevitate(true);
        m_creature->SetHover(true);
        m_creature->CastSpell(nullptr, SPELL_ORONOK_SPEED_INCREASE, TRIGGERED_OLD_TRIGGERED);
        Creature* borak = m_creature->SummonCreature(NPC_BORAK_SON_OF_ORONOK, -3419.458f, 1383.739f, 228.1865f, 5.532694f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 300000, true, true);
        borak->Mount(MOUNT_ID_ORONOK);
        borak->SetLevitate(true);
        borak->SetHover(true);
        borak->CastSpell(nullptr, SPELL_ORONOK_SPEED_INCREASE, TRIGGERED_OLD_TRIGGERED);
        borak->GetMotionMaster()->MoveFollow(m_creature, 5.0f, -M_PI_F / 2, true);
        Creature* gromtor = m_creature->SummonCreature(NPC_GROMTOR_SON_OF_ORONOK, -3419.458f, 1383.739f, 228.1865f, 5.532694f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 300000, true, true);
        gromtor->Mount(MOUNT_ID_ORONOK);
        gromtor->SetLevitate(true);
        gromtor->SetHover(true);
        gromtor->CastSpell(nullptr, SPELL_ORONOK_SPEED_INCREASE, TRIGGERED_OLD_TRIGGERED);
        gromtor->GetMotionMaster()->MoveFollow(m_creature, 5.0f, M_PI_F / 2, true);
        gromtor->CastSpell(nullptr, SPELL_ORONOK_SPEED_INCREASE, TRIGGERED_OLD_TRIGGERED);
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case NPC_CYRUKH_THE_FIRELORD:
            {
                Creature* cyrukh = m_creature->SummonCreature(NPC_CYRUKH_THE_FIRELORD, -3600.073f, 1799.803f, 39.70634f, 1.448623f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 300000, true, true);
                cyrukh->AI()->SetReactState(REACT_DEFENSIVE);
                break;
            }
            case NPC_EARTHMENDER_TORLOK:
                if (Creature* pTorlok = GetClosestCreatureWithEntry(m_creature, NPC_EARTHMENDER_TORLOK, 25.0f))
                {
                    m_torlokGuid = pTorlok->GetObjectGuid();
                    m_creature->SetFacingToObject(pTorlok);
                }
                break;
            case NPC_REDEEMED_SPIRIT_OF_EARTH:
            {
                m_creature->SetFacingTo(4.9f);
                Creature* elemental = m_creature->SummonCreature(NPC_REDEEMED_SPIRIT_OF_AIR, aDamnationLocations[0].x, aDamnationLocations[0].y, aDamnationLocations[0].z, aDamnationLocations[0].o, TEMPSPAWN_TIMED_DESPAWN, 32000);
                elemental->CastSpell(nullptr, SPELL_ELEMENTAL_SPAWN_IN, TRIGGERED_NONE);
                elemental = m_creature->SummonCreature(NPC_REDEEMED_SPIRIT_OF_EARTH, aDamnationLocations[1].x, aDamnationLocations[1].y, aDamnationLocations[1].z, aDamnationLocations[1].o, TEMPSPAWN_TIMED_DESPAWN, 32000);
                elemental->CastSpell(nullptr, SPELL_ELEMENTAL_SPAWN_IN, TRIGGERED_NONE);
                elemental = m_creature->SummonCreature(NPC_REDEEMED_SPIRIT_OF_FIRE, aDamnationLocations[2].x, aDamnationLocations[2].y, aDamnationLocations[2].z, aDamnationLocations[2].o, TEMPSPAWN_TIMED_DESPAWN, 32000);
                elemental->CastSpell(nullptr, SPELL_ELEMENTAL_SPAWN_IN, TRIGGERED_NONE);
                elemental = m_creature->SummonCreature(NPC_REDEEMED_SPIRIT_OF_WATER, aDamnationLocations[3].x, aDamnationLocations[3].y, aDamnationLocations[3].z, aDamnationLocations[3].o, TEMPSPAWN_TIMED_DESPAWN, 32000);
                elemental->CastSpell(nullptr, SPELL_ELEMENTAL_SPAWN_IN, TRIGGERED_NONE);
                break;
            }
            case SAY_ORONOK_EPILOGUE_7:
                if (Creature* pTorlok = m_creature->GetMap()->GetCreature(m_torlokGuid))
                    m_creature->SetFacingToObject(pTorlok);
                DoScriptText(EMOTE_GIVE_WEAPONS, m_creature);
                break;
            case EVENT_GIVE_WEAPONS:
                m_creature->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 0);
                if (Creature* pBorak = GetSpeakerByEntry(NPC_BORAK_SON_OF_ORONOK))
                {
                    pBorak->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 0);
                    pBorak->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 0);
                }
                if (Creature* pGromtor = GetSpeakerByEntry(NPC_GROMTOR_SON_OF_ORONOK))
                {
                    pGromtor->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 0);
                    pGromtor->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 0);
                }
                break;
            case EVENT_MOUNT:
                m_creature->Mount(MOUNT_ID_ORONOK);
                if (Creature* pBorak = GetSpeakerByEntry(NPC_BORAK_SON_OF_ORONOK))
                    pBorak->Mount(MOUNT_ID_ORONOK);
                if (Creature* pGromtor = GetSpeakerByEntry(NPC_GROMTOR_SON_OF_ORONOK))
                    pGromtor->Mount(MOUNT_ID_ORONOK);
                break;
            case EVENT_HOVER_AND_MOVE:
                m_creature->Mount(MOUNT_ID_ORONOK);
                m_creature->SetLevitate(true);
                m_creature->SetHover(true);
                m_creature->StopMoving();
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveWaypoint(PATH_ID_OUTRO);
                if (Creature* pBorak = GetSpeakerByEntry(NPC_BORAK_SON_OF_ORONOK))
                {
                    pBorak->Mount(MOUNT_ID_ORONOK);
                    pBorak->SetLevitate(true);
                    pBorak->SetHover(true);
                    pBorak->GetMotionMaster()->MoveFollow(m_creature, 5.0f, -M_PI_F / 2, true);
                }
                if (Creature* pGromtor = GetSpeakerByEntry(NPC_GROMTOR_SON_OF_ORONOK))
                {
                    pGromtor->Mount(MOUNT_ID_ORONOK);
                    pGromtor->SetLevitate(true);
                    pGromtor->SetHover(true);
                    pGromtor->GetMotionMaster()->MoveFollow(m_creature, 5.0f, M_PI_F / 2, true);
                }
                break;
            case NPC_ORONOK_TORN_HEART:
                if (GameObject* pMark = GetClosestGameObjectWithEntry(m_creature, GO_MARK_OF_KAELTHAS, 30.0f))
                {
                    pMark->SetRespawnTime(5 * MINUTE);
                    pMark->Refresh();
                }
                break;
        }
    }

    Creature* GetSpeakerByEntry(uint32 uiEntry) override
    {
        switch (uiEntry)
        {
            case NPC_ORONOK_TORN_HEART:         return m_creature;
            case NPC_EARTHMENDER_TORLOK:        return m_creature->GetMap()->GetCreature(m_torlokGuid);
            case NPC_REDEEMED_SPIRIT_OF_EARTH:  return m_creature->GetMap()->GetCreature(m_earthSpiritGuid);
            case NPC_REDEEMED_SPIRIT_OF_FIRE:   return m_creature->GetMap()->GetCreature(m_fireSpiritGuid);
            case NPC_GROMTOR_SON_OF_ORONOK:     return m_creature->GetMap()->GetCreature(m_gromtorGuid);
            case NPC_BORAK_SON_OF_ORONOK:       return m_creature->GetMap()->GetCreature(m_borakGuid);
            case NPC_CYRUKH_THE_FIRELORD:       return m_creature->GetMap()->GetCreature(m_cyrukhGuid);
            default:                            return nullptr;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_REDEEMED_SPIRIT_OF_FIRE:    m_fireSpiritGuid = pSummoned->GetObjectGuid();  break;
            case NPC_REDEEMED_SPIRIT_OF_EARTH:   m_earthSpiritGuid = pSummoned->GetObjectGuid(); break;
            case NPC_GROMTOR_SON_OF_ORONOK:      m_gromtorGuid = pSummoned->GetObjectGuid();     break;
            case NPC_BORAK_SON_OF_ORONOK:        m_borakGuid = pSummoned->GetObjectGuid();       break;
            case NPC_CYRUKH_THE_FIRELORD:        m_cyrukhGuid = pSummoned->GetObjectGuid();      break;
        }
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(true);

        m_creature->SetLootRecipient(nullptr);

        Reset();

        if (!m_creature->IsAlive())
            return;

        if (Creature* pCyrukh = GetSpeakerByEntry(NPC_CYRUKH_THE_FIRELORD))
        {
            if (!pCyrukh->IsAlive())
            {
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_EPILOGUE, aDamnationLocations[6].x, aDamnationLocations[6].y, aDamnationLocations[6].z);
                m_creature->setFaction(FACTION_ORONOK_FRIENDLY);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                if (Creature* borak = GetSpeakerByEntry(NPC_BORAK_SON_OF_ORONOK))
                {
                    borak->setFaction(FACTION_ORONOK_FRIENDLY);
                    borak->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                    borak->GetMotionMaster()->MoveFollow(m_creature, 5.0f, -M_PI_F / 2, true);
                }
                if (Creature* gromtor = GetSpeakerByEntry(NPC_GROMTOR_SON_OF_ORONOK))
                {
                    gromtor->setFaction(FACTION_ORONOK_FRIENDLY);
                    gromtor->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                    gromtor->GetMotionMaster()->MoveFollow(m_creature, 5.0f, M_PI_F / 2, true);
                }
            }
        }
        else
        {
            script_error_log("Npc %u couldn't be found or something really bad happened. Epilogue event for quest %u will stop.", NPC_CYRUKH_THE_FIRELORD, QUEST_CIPHER_OF_DAMNATION);
            m_creature->GetMotionMaster()->MoveTargetedHome();
        }
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        switch (motionType)
        {
            case POINT_MOTION_TYPE: PointMovementInform(motionType, pointId); break;
            case WAYPOINT_MOTION_TYPE: WaypointMotionType(motionType, pointId); break;
            default: break;
        }
    }

    void WaypointMotionType(uint32 /*motionType*/, uint32 pointId)
    {
        switch (pointId)
        {
            case WAYPOINT_ID_DESPAWN:
            {
                if (m_creature->GetMotionMaster()->GetPathId() == 2)
                {
                    if (Creature* borak = GetSpeakerByEntry(NPC_BORAK_SON_OF_ORONOK))
                        borak->ForcedDespawn();
                    if (Creature* gromtor = GetSpeakerByEntry(NPC_GROMTOR_SON_OF_ORONOK))
                        gromtor->ForcedDespawn();
                    m_creature->ForcedDespawn();
                }
                break;
            }
            case WAYPOINT_ID_INTRO_FINAL:
            {
                // Set them in motion
                m_creature->SetWalk(false);
                m_creature->Mount(0);
                m_creature->SetLevitate(false);
                m_creature->SetHover(false);
                m_creature->Unmount();
                m_creature->StopMoving();
                m_creature->RemoveAurasDueToSpell(SPELL_ORONOK_SPEED_INCREASE);
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_ATTACK_READY, aDamnationLocations[4].x, aDamnationLocations[4].y, aDamnationLocations[4].z);
                if (Creature* borak = GetSpeakerByEntry(NPC_BORAK_SON_OF_ORONOK))
                {
                    borak->Mount(0);
                    borak->SetLevitate(false);
                    borak->SetHover(false);
                    borak->Unmount();
                    borak->GetMotionMaster()->MoveFollow(m_creature, 5.0f, -M_PI_F / 2, true);
                    borak->RemoveAurasDueToSpell(SPELL_ORONOK_SPEED_INCREASE);
                    m_borak = borak;
                }
                if (Creature* gromtor = GetSpeakerByEntry(NPC_GROMTOR_SON_OF_ORONOK))
                {
                    gromtor->Mount(0);
                    gromtor->SetLevitate(false);
                    gromtor->SetHover(false);
                    gromtor->Unmount();
                    gromtor->GetMotionMaster()->MoveFollow(m_creature, 5.0f, M_PI_F / 2, true);
                    gromtor->RemoveAurasDueToSpell(SPELL_ORONOK_SPEED_INCREASE);
                    m_gromtor = gromtor;
                }
                if (Unit* summoner = m_creature->GetMap()->GetUnit(m_creature->GetSpawnerGuid()))
                    DoScriptText(SAY_ORONOK_TOGETHER, m_creature, summoner);
                break;
            }
        }
    }

    void PointMovementInform(uint32 /*motionType*/, uint32 pointId)
    {
        switch (pointId)
        {
            case POINT_ID_ATTACK_READY:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                DoScriptText(SAY_ORONOK_READY, m_creature);
                break;
            case POINT_ID_ELEMENTS:
                // Cyrukh starts to attack
                m_creature->setFaction(FACTION_ORONOK_COMBAT);
                if (Creature* borak = GetSpeakerByEntry(NPC_BORAK_SON_OF_ORONOK))
                    borak->setFaction(FACTION_ORONOK_COMBAT);
                if (Creature* gromtor = GetSpeakerByEntry(NPC_GROMTOR_SON_OF_ORONOK))
                    gromtor->setFaction(FACTION_ORONOK_COMBAT);
                if (Creature* pCyrukh = GetSpeakerByEntry(NPC_CYRUKH_THE_FIRELORD))
                {
                    pCyrukh->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                    if (m_borak)
                        printf("%p\n", m_borak);
                    if (m_gromtor)
                        printf("%p\n", m_gromtor);
                    pCyrukh->AI()->AttackStart(m_creature);
                    AttackStart(pCyrukh);
                    m_bHasAttackStart = true;
                }
                break;
            case POINT_ID_EPILOGUE:
                StartNextDialogueText(NPC_EARTHMENDER_TORLOK);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiLightningTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_CHAIN_LIGHTNING) == CAST_OK)
                    m_uiLightningTimer = urand(9000, 15000);
            }
        }
        else
            m_uiLightningTimer -= uiDiff;

        if (m_uiTotemTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_EARTHBIND_TOTEM) == CAST_OK)
                m_uiTotemTimer = urand(40000, 60000);
        }
        else
            m_uiTotemTimer -= uiDiff;

        if (m_uiFrostTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FROST_SHOCK) == CAST_OK)
                m_uiFrostTimer = urand(14000, 18000);
        }
        else
            m_uiFrostTimer -= uiDiff;

        if (m_uiHealTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_HEALING_WAVE) == CAST_OK)
                m_uiHealTimer = urand(6000, 10000);
        }
        else
            m_uiHealTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_spawned_oronok_tornheart(Creature* pCreature)
{
    return new npc_spawned_oronok_tornheartAI(pCreature);
}

bool GossipHello_npc_spawned_oronok_tornheart(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_CIPHER_OF_DAMNATION) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_ORONOK, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_spawned_oronok_tornheart(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        // Note: this movement expects MMaps.
        DoScriptText(SAY_ORONOK_ELEMENTS, pCreature);
        pCreature->GetMotionMaster()->MovePoint(POINT_ID_ELEMENTS, aDamnationLocations[5].x, aDamnationLocations[5].y, aDamnationLocations[5].z);
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}

/*######
## npc_domesticated_felboar
######*/

enum
{
    EMOTE_SNIFF_AIR             = -1000907,
    EMOTE_START_DIG             = -1000908,
    EMOTE_SQUEAL                = -1000909,

    SPELL_SHADOWMOON_TUBER      = 36462,
    SPELL_SPECIAL_UNARMED       = 33334,
    SPELL_TUBER_WHISTLE         = 36652,

    NPC_DOMESTICATED_FELBOAR    = 21195,
    NPC_SHADOWMOON_TUBER_NODE   = 21347,
    GO_SHADOWMOON_TUBER_MOUND   = 184701,
};

struct npc_domesticated_felboarAI : public ScriptedAI
{
    npc_domesticated_felboarAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiTuberTimer;
    uint8 m_uiTuberStage;

    void Reset() override
    {
        m_uiTuberTimer = 0;
        m_uiTuberStage = 0;
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        if (DoCastSpellIfCan(m_creature, SPELL_SPECIAL_UNARMED) == CAST_OK)
        {
            DoScriptText(EMOTE_START_DIG, m_creature);
            m_uiTuberTimer = 2000;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_START_EVENT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            DoScriptText(EMOTE_SNIFF_AIR, m_creature);

            float fX, fY, fZ;
            m_creature->SetWalk(false);
            m_creature->GetMotionMaster()->MoveIdle();
            pSender->GetContactPoint(m_creature, fX, fY, fZ);
            m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiTuberTimer)
        {
            if (m_uiTuberTimer <= uiDiff)
            {
                switch (m_uiTuberStage)
                {
                    case 0:
                        if (DoCastSpellIfCan(m_creature, SPELL_SPECIAL_UNARMED) == CAST_OK)
                            m_uiTuberTimer = 2000;
                        break;
                    case 1:
                        if (DoCastSpellIfCan(m_creature, SPELL_SHADOWMOON_TUBER) == CAST_OK)
                        {
                            // Despawn current tuber
                            if (GameObject* pTuber = GetClosestGameObjectWithEntry(m_creature, GO_SHADOWMOON_TUBER_MOUND, 3.0f))
                                pTuber->SetLootState(GO_JUST_DEACTIVATED);

                            DoScriptText(EMOTE_SQUEAL, m_creature);
                            m_uiTuberTimer = 2000;
                        }
                        break;
                    case 2:
                        EnterEvadeMode();
                        break;
                }
                ++m_uiTuberStage;
            }
            else
                m_uiTuberTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_domesticated_felboar(Creature* pCreature)
{
    return new npc_domesticated_felboarAI(pCreature);
}

bool EffectDummyCreature_npc_shadowmoon_tuber_node(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_TUBER_WHISTLE && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCreatureTarget->GetEntry() == NPC_SHADOWMOON_TUBER_NODE)
        {
            // Check if tuber mound exists or it's spawned
            GameObject* pTuber = GetClosestGameObjectWithEntry(pCreatureTarget, GO_SHADOWMOON_TUBER_MOUND, 1.0f);
            if (!pTuber || !pTuber->IsSpawned())
                return true;

            // Call nearby felboar
            if (Creature* pBoar = GetClosestCreatureWithEntry(pCreatureTarget, NPC_DOMESTICATED_FELBOAR, 40.0f))
                pCreatureTarget->AI()->SendAIEvent(AI_EVENT_START_EVENT, pCaster, pBoar);
        }

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## npc_veneratus_spawn_node
######*/

enum
{
    SAY_VENERATUS_SPAWN         = -1000579,

    NPC_VENERATUS               = 20427,
    NPC_SPIRIT_HUNTER           = 21332,
};

struct npc_veneratus_spawn_nodeAI : public Scripted_NoMovementAI
{
    npc_veneratus_spawn_nodeAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // Check for the spirit hunter in order to spawn Veneratus; this will replace missing spells 36614 (dummy periodic spell) and 36616 (summon spell)
        if (pWho->GetEntry() == NPC_SPIRIT_HUNTER && m_creature->IsWithinDistInMap(pWho, 40.0f) && m_creature->IsWithinLOSInMap(pWho))
        {
            DoScriptText(SAY_VENERATUS_SPAWN, pWho);
            m_creature->SummonCreature(NPC_VENERATUS, 0, 0, 0, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
            m_creature->ForcedDespawn();
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_veneratus_spawn_node(Creature* pCreature)
{
    return new npc_veneratus_spawn_nodeAI(pCreature);
}

/*######
## npc_disobedient_dragonmaw_peon
######*/

enum
{
    SAY_IDLE1   = -1001285,
    SAY_IDLE2   = -1001286,
    SAY_BOOTERANG1 = -1001287,
    SAY_BOOTERANG2 = -1001288,
    SAY_BOOTERANG3 = -1001289,
    SAY_BOOTERANG4 = -1001290,
    SAY_BOOTERANG5 = -1001291,
    SAY_BOOTERANG6 = -1001292,

    SPELL_BOOTERANG                 = 40742,
    SPELL_LAZY_AND_GOOD_FOR_NOTHING = 40732,
    SPELL_DEFIANT_AND_ENRAGED       = 40735,
    SPELL_PEON_CLEAR_ALL            = 40762,
    SPELL_KICK                      = 34802,
    SPELL_SUNDER_ARMOR              = 15572,

    NPC_PEON                = 22252,
    NPC_PEON_WORK_NODE      = 23308,
    NPC_DISOBEDIENT_PEON    = 23311,

    FACTION_DISOBEDIENT     = 14,
    FACTION_WHACKED         = 62,

    EMOTE_WORKING           = 233,

    POINT_ARRIVED           = 1,
};

struct npc_disobedient_dragonmaw_peonAI : public ScriptedAI
{
    npc_disobedient_dragonmaw_peonAI(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }

    uint32 m_angryTimer;
    uint32 m_booterangTimer;
    uint32 m_kickTimer;
    uint32 m_sunderarmorTimer;
    ObjectGuid m_lastPlayerGuid;

    void Reset() override
    {
        if (m_lastPlayerGuid.IsEmpty())
        {
            if (m_creature->HasAura(SPELL_LAZY_AND_GOOD_FOR_NOTHING))
                m_angryTimer = 0;
            else
                m_angryTimer = urand(6000, 10000);
            m_booterangTimer = 0;
            m_kickTimer = urand(5000, 10000);
            m_sunderarmorTimer = urand(3000, 9000);
        }
    }

    void MovementInform(uint32 movementType, uint32 data) override
    {
        if (movementType == POINT_MOTION_TYPE)
        {
            if (data == POINT_ARRIVED)
            {
                Creature* node = GetClosestCreatureWithEntry(m_creature, NPC_PEON_WORK_NODE, 10.f);
                if (node)
                {
                    float angle = m_creature->GetAngle(node);
                    m_creature->SetOrientation(angle);
                    m_creature->SetFacingTo(angle);
                }
                if (Player* player = m_creature->GetMap()->GetPlayer(m_lastPlayerGuid))
                    player->RewardPlayerAndGroupAtEventCredit(NPC_DISOBEDIENT_PEON, m_creature);

                m_lastPlayerGuid = ObjectGuid();
                m_creature->UpdateEntry(NPC_PEON);
                m_creature->HandleEmote(EMOTE_WORKING);
                m_creature->ForcedDespawn(90000);
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveIdle();
            }
        }
    }

    void SpellHit(Unit* caster, const SpellEntry* spell) override
    {
        if (spell->Id == SPELL_BOOTERANG)
            HandleBooterang(caster);
    }

    void HandleAngry()
    {
        DoCastSpellIfCan(nullptr, SPELL_DEFIANT_AND_ENRAGED);
        switch (urand(0, 4))
        {
            case 0: DoScriptText(SAY_IDLE1, m_creature); break;
            case 1: DoScriptText(SAY_IDLE2, m_creature); break;
            default: break;
        }
    }

    void HandleBooterang(Unit* caster)
    {
        if (caster->GetTypeId() != TYPEID_PLAYER && m_lastPlayerGuid.IsEmpty())
            return;

        Player* player = static_cast<Player*>(caster);
        m_lastPlayerGuid = player->GetObjectGuid();

        switch (urand(0, 5))
        {
            case 0: DoScriptText(SAY_BOOTERANG1, m_creature, player); break;
            case 1: DoScriptText(SAY_BOOTERANG2, m_creature, player); break;
            case 2: DoScriptText(SAY_BOOTERANG3, m_creature, player); break;
            case 3: DoScriptText(SAY_BOOTERANG4, m_creature, player); break;
            case 4: DoScriptText(SAY_BOOTERANG5, m_creature, player); break;
            case 5: DoScriptText(SAY_BOOTERANG6, m_creature, player); break;
        }
        float angle = m_creature->GetAngle(player);
        m_creature->SetOrientation(angle);
        m_creature->SetFacingTo(angle);

        DoCastSpellIfCan(nullptr, SPELL_PEON_CLEAR_ALL); // clears combat and removes aura
        m_creature->setFaction(FACTION_WHACKED);

        m_booterangTimer = 3000;
        m_angryTimer = 0;
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->IsInCombat())
        {
            if (m_angryTimer)
            {
                if (m_angryTimer <= diff)
                {
                    HandleAngry();
                    m_angryTimer = urand(6000, 10000);
                }
                else m_angryTimer -= diff;
            }

            if (m_booterangTimer)
            {
                if (m_booterangTimer <= diff)
                {
                    float x, y, z;
                    Creature* node = GetClosestCreatureWithEntry(m_creature, NPC_PEON_WORK_NODE, 60.f);
                    if (node)
                        node->GetNearPoint(m_creature, x, y, z, m_creature->GetObjectBoundingRadius(), 5.f, node->GetAngle(m_creature));
                    else // failsafe
                        m_creature->GetPosition(x, y, z);
                    m_creature->GetMotionMaster()->MovePoint(POINT_ARRIVED, x, y, z);
                    m_booterangTimer = 0;
                }
                else m_booterangTimer -= diff;
            }
        }
        else
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
                return;

            if (m_kickTimer)
            {
                if (m_kickTimer <= diff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_KICK) == CAST_OK)
                        m_kickTimer = urand(12000, 24000);
                }
                else m_kickTimer -= diff;
            }
            if (m_sunderarmorTimer)
            {
                if (m_sunderarmorTimer <= diff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SUNDER_ARMOR) == CAST_OK)
                        m_sunderarmorTimer = urand(12000, 18000);
                }
                else m_sunderarmorTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }

        ScriptedAI::UpdateAI(diff);
    }
};

UnitAI* GetAI_npc_disobedient_dragonmaw_peon(Creature* pCreature)
{
    return new npc_disobedient_dragonmaw_peonAI(pCreature);
}

/*######
## Deadliest Trap Ever Laid commander NPC AIs
######*/

enum
{
    SAY_EVENT_ACCEPT_ARCUS = -1001293,
    SAY_EVENT_START_ARCUS  = -1001294,
    SAY_EVENT_END_ARCUS    = -1001295,

    SAY_EVENT_ACCEPT_HOBB  = -1001296,
    SAY_EVENT_START_HOBB   = -1001297,
    SAY_EVENT_END_HOBB     = -1001298,

    QUEST_DEADLIEST_TRAP_SCRYERS = 11097,
    QUEST_DEADLIEST_TRAP_ALDOR   = 11101,

    // Combat spells
    SPELL_AIMED_SHOT    = 38370,
    SPELL_SHOOT         = 41440,
    SPELL_MULTI_SHOT    = 41448,

    // Emotes
    // EMOTE_STATE_READYRIFLE
    // EMOTE_ONESHOT_READYBOW

    NPC_SANCTUM_DEFENDER = 23435,
    NPC_ALTAR_DEFENDER   = 23453,

    NPC_SCRYERS_DRAGONMAW_SKYBREAKER = 23440,
    NPC_ALDOR_DRAGONMAW_SKYBREAKER   = 23441,

    COMMANDER_HOBB          = 0,
    COMMANDER_ARCUS         = 1,
    COMMANDER_COUNT         = 2,
    DEFENDER_SPAWN_COUNT    = 10, // per commander

    DRAGONMAW_SPAWN_COUNT   = 9, // per commander

    // Paths
    LAST_POINT_HOBB         = 2,
    LAST_POINT_ARCUS        = 6,

    POINT_HOME              = 1,

    DRAGONMAW_KILL_COUNT    = 30,
};

enum CommanderActions : uint32
{
    COMMANDER_COMBAT_ACTION_AIMED_SHOT,
    COMMANDER_COMBAT_ACTION_MULTI_SHOT,
    COMMANDER_COMBAT_ACTION_SHOOT,
    COMMANDER_COMBAT_ACTION_MAX,
    COMMANDER_ACTION_START_QUEST_FLAGS,
    COMMANDER_ACTION_START_QUEST_TEXT,
    COMMANDER_ACTION_START_QUEST_MOVEMENT,
    COMMANDER_ACTION_POST_MOVEMENT_TEXT,
    COMMANDER_ACTION_POST_MOVEMENT_FACE_DIRECTION, // Only for aldor
    COMMANDER_ACTION_POST_MOVEMENT_START_EVENT,
    COMMANDER_ACTION_WIN_RETURN,
    COMMANDER_ACTION_FAIL_EVENT,
};

struct SpawnData
{
    float x, y, z, ori;
};

static SpawnData defenderSpawns[COMMANDER_COUNT][DEFENDER_SPAWN_COUNT] =
{
    { // scryer
        { -4084.224f,1066.301f,31.44092f,5.427974f },
        { -4077.014f,1070.703f,31.12169f,5.375614f },
        { -4070.818f,1074.806f,31.39684f,5.393067f },
        { -4063.517f,1079.07f, 32.32865f,5.462881f },
        { -4086.24f, 1060.344f,31.0915f, 5.305801f },
        { -4078.464f,1065.285f,31.17392f,5.358161f },
        { -4071.446f,1070.082f,30.65687f,5.358161f },
        { -4064.422f,1074.611f,31.13296f,5.323254f },
        { -4056.84f, 1079.573f,32.50325f,5.5676f   },
        { -4074.189f,1076.387f,31.9625f, 5.358161f }
    },
    { // aldor
        { -3094.992f,662.7219f,-12.76196f,2.583087f },
        { -3091.117f,678.4016f,-16.35974f,3.036873f },
        { -3088.423f,684.4808f,-17.56973f,3.141593f },
        { -3093.32f, 673.4081f,-15.20303f,2.879793f },
        { -3085.618f,692.2532f,-17.65626f,3.089233f },
        { -3096.634f,667.7632f,-13.79022f,2.775074f },
        { -3083.601f,687.3881f,-17.03627f,3.089233f },
        { -3086.168f,680.5683f,-15.94252f,2.984513f },
        { -3088.57f, 674.317f, -14.89533f,2.75762f  },
        { -3091.114f,668.1656f,-14.13708f,2.740167f }
    }
};

static SpawnData dragonmawSpawns[COMMANDER_COUNT][DRAGONMAW_SPAWN_COUNT] =
{ // orientations are not sniffed
    { // scryer
        { -4063.342f, 928.9622f, 82.92392f, 0.f},
        { -4101.632f, 942.9062f, 58.77553f, 0.f},
        { -3996.584f, 984.6102f, 39.35212f, 0.f},
        { -4059.524f, 924.002f,  65.44735f, 0.f},
        { -4037.429f, 946.7924f, 83.17754f, 0.f},
        { -4088.804f, 925.6664f, 67.03867f, 0.f},
        { -4037.43f,  934.6457f, 40.03371f, 0.f},
        { -4079.224f, 931.5303f, 76.49546f, 0.f},
        { -4108.912f, 938.1812f, 94.50812f, 0.f},
    },
    { // aldor
        { -3277.004f, 696.5723f, 35.61721f, 0.f},
        { -2953.928f, 533.5043f, 92.35249f, 0.f},
        { -3276.309f, 709.3771f, 25.12147f, 0.f},
        { -2992.797f, 545.8665f, 70.36189f, 0.f},
        { -3287.957f, 725.9796f, 51.60571f, 0.f},
        { -3293.602f, 759.0432f, 44.21932f, 0.f},
        { -3286.902f, 732.0245f, 27.88107f, 0.f},
        { -3016.981f, 534.4969f, 21.01351f, 0.f},
        { -3269.654f, 687.1747f, 33.11995f, 0.f},
    }
};

struct DeadliestScriptInfo
{
    uint32 dragonmawNpc;
    uint32 defenderNpc;
    uint32 lastPointInPath;
    int32 startText;
    int32 midText;
    int32 winText;
    int32 deathText;
    uint32 questId;
};

static DeadliestScriptInfo deadliestScriptInfo[COMMANDER_COUNT] =
{
    // TODO: Scryers on death text not confirmed
    { NPC_SCRYERS_DRAGONMAW_SKYBREAKER, NPC_SANCTUM_DEFENDER, LAST_POINT_HOBB, SAY_EVENT_ACCEPT_HOBB, SAY_EVENT_START_HOBB, SAY_EVENT_END_HOBB, SAY_EVENT_ACCEPT_ARCUS, QUEST_DEADLIEST_TRAP_SCRYERS },
    { NPC_ALDOR_DRAGONMAW_SKYBREAKER,   NPC_ALTAR_DEFENDER,   LAST_POINT_ARCUS, SAY_EVENT_ACCEPT_ARCUS, SAY_EVENT_START_ARCUS, SAY_EVENT_END_ARCUS, SAY_EVENT_ACCEPT_ARCUS, QUEST_DEADLIEST_TRAP_ALDOR }
};

struct npc_commanderAI : public CombatAI
{
    npc_commanderAI(Creature* creature, uint8 commanderId) : CombatAI(creature, COMMANDER_COMBAT_ACTION_MAX),
        m_defenderSpawns(DEFENDER_SPAWN_COUNT),
        m_dragonmawSpawns(DRAGONMAW_SPAWN_COUNT),
        m_killCounter(0),
        m_commanderId(commanderId)
    {
        m_attackDistance = 30.f;
        m_meleeEnabled = false;
        AddCombatAction(COMMANDER_COMBAT_ACTION_AIMED_SHOT, GetInitialCombatActionTimer(COMMANDER_COMBAT_ACTION_AIMED_SHOT));
        AddCombatAction(COMMANDER_COMBAT_ACTION_MULTI_SHOT, GetInitialCombatActionTimer(COMMANDER_COMBAT_ACTION_MULTI_SHOT));
        AddCombatAction(COMMANDER_COMBAT_ACTION_SHOOT, GetInitialCombatActionTimer(COMMANDER_COMBAT_ACTION_SHOOT));

        AddCustomAction(COMMANDER_ACTION_START_QUEST_FLAGS, true, [&]() { m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER); });
        AddCustomAction(COMMANDER_ACTION_START_QUEST_TEXT, true, [&]()
        { DoScriptText(deadliestScriptInfo[m_commanderId].startText, m_creature, m_creature->GetMap()->GetPlayer(m_startingPlayer)); });
        AddCustomAction(COMMANDER_ACTION_START_QUEST_MOVEMENT, true, [&]() { m_creature->GetMotionMaster()->MoveWaypoint(); });
        AddCustomAction(COMMANDER_ACTION_POST_MOVEMENT_TEXT, true, [&]()
        { DoScriptText(deadliestScriptInfo[m_commanderId].midText, m_creature, m_creature->GetMap()->GetPlayer(m_startingPlayer)); });
        AddCustomAction(COMMANDER_ACTION_POST_MOVEMENT_START_EVENT, true, [&]() { StartAttackingEvent(); });
        AddCustomAction(COMMANDER_ACTION_WIN_RETURN, true, [&]()
        {
            float x, y, z, ori;
            m_creature->GetRespawnCoord(x, y, z, &ori);
            m_creature->GetMotionMaster()->MovePoint(POINT_HOME, x, y, z);
        });
        AddCustomAction(COMMANDER_ACTION_FAIL_EVENT, true, [&]()
        {
            m_creature->ForcedDespawn();
            FailEvent();
        });
    }

    GuidVector m_defenderSpawns;
    GuidVector m_dragonmawSpawns;
    ObjectGuid m_startingPlayer;

    uint8 m_killCounter;
    uint8 m_commanderId;

    void GetAIInformation(ChatHandler& reader) override
    {
        ScriptedAI::GetAIInformation(reader);
        CombatActions::GetAIInformation(reader);
        reader.PSendSysMessage("Defender Spawn Count: %lu", m_defenderSpawns.size());
        reader.PSendSysMessage("Dragonmaw Spawn Count: %lu", m_dragonmawSpawns.size());
        reader.PSendSysMessage("Starting player: %lu", m_startingPlayer.GetRawValue());
        reader.PSendSysMessage("Kill counter: %u", m_killCounter);
    }

    uint32 GetInitialCombatActionTimer(uint32 index)
    {
        switch (index)
        {
            case COMMANDER_COMBAT_ACTION_AIMED_SHOT: return 5000;
            case COMMANDER_COMBAT_ACTION_MULTI_SHOT: return 10000;
            case COMMANDER_COMBAT_ACTION_SHOOT:      return 0;
        }
        return 0;
    }

    uint32 GetRepeatingCombatActionTimer(uint32 index)
    {
        switch (index)
        {
            case COMMANDER_COMBAT_ACTION_AIMED_SHOT: return 10000;
            case COMMANDER_COMBAT_ACTION_MULTI_SHOT: return 15000;
            case COMMANDER_COMBAT_ACTION_SHOOT:      return 3000;
        }
        return 0;
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case COMMANDER_COMBAT_ACTION_AIMED_SHOT:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_AIMED_SHOT) == CAST_OK)
                    ResetCombatAction(action, GetRepeatingCombatActionTimer(action));
                return;
            case COMMANDER_COMBAT_ACTION_MULTI_SHOT:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MULTI_SHOT) == CAST_OK)
                    ResetCombatAction(action, GetRepeatingCombatActionTimer(action));
                return;
            case COMMANDER_COMBAT_ACTION_SHOOT:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHOOT) == CAST_OK)
                    ResetCombatAction(action, GetRepeatingCombatActionTimer(action));
                return;
        }
    }

    void SpawnDefenders()
    {
        for (uint32 i = 0; i < DEFENDER_SPAWN_COUNT; ++i)
        {
            Creature* defender = m_creature->SummonCreature(deadliestScriptInfo[m_commanderId].defenderNpc, defenderSpawns[m_commanderId][i].x,
                    defenderSpawns[m_commanderId][i].y, defenderSpawns[m_commanderId][i].z, defenderSpawns[m_commanderId][i].ori, TEMPSPAWN_CORPSE_DESPAWN, 1000, true);
            defender->HandleEmote(EMOTE_STATE_READYRIFLE);
            m_defenderSpawns[i] = defender->GetObjectGuid();
        }
    }

    void DespawnDefenders()
    {
        for (ObjectGuid& guid : m_defenderSpawns)
            if (Creature* defender = m_creature->GetMap()->GetCreature(guid))
                defender->ForcedDespawn();
    }

    void SpawnDragonmaw()
    {
        for (uint32 i = 0; i < DRAGONMAW_SPAWN_COUNT; ++i)
            SpawnSpecificDragonmaw(i);
    }

    void SpawnSpecificDragonmaw(uint32 i)
    {
        Creature* dragonmaw = m_creature->SummonCreature(deadliestScriptInfo[m_commanderId].dragonmawNpc, dragonmawSpawns[m_commanderId][i].x,
            dragonmawSpawns[m_commanderId][i].y, dragonmawSpawns[m_commanderId][i].z, dragonmawSpawns[m_commanderId][i].ori, TEMPSPAWN_CORPSE_DESPAWN, 1000, true, true, i);
        m_dragonmawSpawns[i] = dragonmaw->GetObjectGuid();
    }

    void DespawnDragonmaw()
    {
        for (ObjectGuid& guid : m_dragonmawSpawns)
            if (Creature* dragonmaw = m_creature->GetMap()->GetCreature(guid))
                dragonmaw->ForcedDespawn();
    }

    void StartEvent(Player* player)
    {
        m_startingPlayer = player->GetObjectGuid();
        SpawnDefenders();
        m_killCounter = 0;
        m_creature->SetWalk(false);
        ResetTimer(COMMANDER_ACTION_START_QUEST_FLAGS, 1000);
        ResetTimer(COMMANDER_ACTION_START_QUEST_TEXT, 2000);
        ResetTimer(COMMANDER_ACTION_START_QUEST_MOVEMENT, 4500);
        m_creature->SetActiveObjectState(true);
    }

    virtual void FinishedWaypointMovement()
    {
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetSheath(SHEATH_STATE_RANGED);
        ResetTimer(COMMANDER_ACTION_FAIL_EVENT, 900000);
    }

    virtual void StartAttackingEvent()
    {
        SpawnDragonmaw();
    }

    void WinEvent()
    {
        if (Player* player = m_creature->GetMap()->GetPlayer(m_startingPlayer))
            player->RewardPlayerAndGroupAtEventExplored(deadliestScriptInfo[m_commanderId].questId, m_creature);

        DoScriptText(deadliestScriptInfo[m_commanderId].winText, m_creature, m_creature->GetMap()->GetPlayer(m_startingPlayer));

        if (m_creature->IsInCombat())
            EnterEvadeMode();
        ResetTimer(COMMANDER_ACTION_WIN_RETURN, 3000);
        ResetEvent();
    }

    void FailEvent()
    {
        DoScriptText(deadliestScriptInfo[m_commanderId].deathText, m_creature, m_creature->GetMap()->GetPlayer(m_startingPlayer));
        ResetEvent();
    }

    void ResetEvent()
    {
        DespawnDefenders();
        DespawnDragonmaw();
        m_startingPlayer = ObjectGuid();
        m_creature->SetSheath(SHEATH_STATE_MELEE);
        DisableTimer(COMMANDER_ACTION_FAIL_EVENT);
        m_creature->HandleEmote(0);
        m_creature->SetActiveObjectState(false);
    }

    void JustDied(Unit* /*killer*/) override
    {
        FailEvent();
    }

    void MovementInform(uint32 movementType, uint32 data) override
    {
        switch (movementType)
        {
            case WAYPOINT_MOTION_TYPE:
                if (deadliestScriptInfo[m_commanderId].lastPointInPath == data)
                    FinishedWaypointMovement();
                break;
            case POINT_MOTION_TYPE:
                if (data == POINT_HOME)
                {
                    float x, y, z, ori;
                    m_creature->GetRespawnCoord(x, y, z, &ori);
                    m_creature->SetOrientation(ori); m_creature->SetFacingTo(ori);
                    m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                }
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        if (summoned->GetEntry() == deadliestScriptInfo[m_commanderId].dragonmawNpc)
        {
            ++m_killCounter;
            if (m_killCounter >= DRAGONMAW_KILL_COUNT)
            {
                WinEvent();
                return;
            }
            ObjectGuid const& guid = summoned->GetObjectGuid();
            for (uint32 i = 0; i < m_dragonmawSpawns.size(); ++i)
            {
                if (guid == m_dragonmawSpawns[i])
                {
                    SpawnSpecificDragonmaw(i);
                    break;
                }
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff, m_creature->IsInCombat());

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        ExecuteActions();
    }
};

struct npc_commander_hobbAI : public npc_commanderAI
{
    npc_commander_hobbAI(Creature* creature) : npc_commanderAI(creature, COMMANDER_HOBB) {}

    void FinishedWaypointMovement() override
    {
        npc_commanderAI::FinishedWaypointMovement();
        ResetTimer(COMMANDER_ACTION_POST_MOVEMENT_TEXT, 1000);
        ResetTimer(COMMANDER_ACTION_POST_MOVEMENT_START_EVENT, 1000);
    }

    void StartAttackingEvent() override
    {
        m_creature->HandleEmote(EMOTE_ONESHOT_READYBOW);
        m_creature->SetOrientation(5.340707f); m_creature->SetFacingTo(5.340707f);
        npc_commanderAI::StartAttackingEvent();
    }
};

struct npc_commander_arcusAI : public npc_commanderAI
{
    npc_commander_arcusAI(Creature* creature) : npc_commanderAI(creature, COMMANDER_ARCUS)
    {
        AddCustomAction(COMMANDER_ACTION_POST_MOVEMENT_FACE_DIRECTION, true, [&]() { m_creature->SetOrientation(2.949606f); m_creature->SetFacingTo(2.949606f); });
    }

    void FinishedWaypointMovement() override
    {
        npc_commanderAI::FinishedWaypointMovement();
        ResetTimer(COMMANDER_ACTION_POST_MOVEMENT_TEXT, 1000);
        ResetTimer(COMMANDER_ACTION_POST_MOVEMENT_FACE_DIRECTION, 5000);
        ResetTimer(COMMANDER_ACTION_POST_MOVEMENT_START_EVENT, 6000);
    }

    void StartAttackingEvent() override
    {
        m_creature->HandleEmote(EMOTE_STATE_READYRIFLE);
        npc_commanderAI::StartAttackingEvent();
    }
};

bool QuestAccept_npc_commander(Player* player, Creature* questgiver, Quest const* quest)
{
    switch (quest->GetQuestId())
    {
        case QUEST_DEADLIEST_TRAP_SCRYERS:
        case QUEST_DEADLIEST_TRAP_ALDOR:
            if (npc_commanderAI* ai = static_cast<npc_commanderAI*>(questgiver->AI()))
                ai->StartEvent(player);
            return true;
    }

    return false;
}

/*######
# Dragonmaw Racer NPC AIs (Netherwing faction questline)
######*/

enum
{
    QUEST_MUCKJAW                   = 11064,
    QUEST_TROPE                     = 11067,
    QUEST_CORLOK                    = 11068,
    QUEST_ICHMAN                    = 11069,
    QUEST_MULVERICK                 = 11070,
    QUEST_SKYSHATTER                = 11071,

    SAY_START_MUCKJAW               = -1001299,
    SAY_END_MUCKJAW                 = -1001300,
    SAY_START_TROPE                 = -1001301,
    SAY_END_TROPE                   = -1001302,
    SAY_START_CORLOK                = -1001303,
    SAY_END_CORLOK                  = -1001304,
    SAY_START_ICHMAN                = -1001305,
    SAY_END_ICHMAN                  = -1001306,
    SAY_START_MULVERICK             = -1001307,
    SAY_END_MULVERICK               = -1001308,
    SAY_START_SKYSHATTER            = -1001309,
    SAY_MID_SKYSHATTER              = -1001310,
    SAY_END_SKYSHATTER              = -1001311,

    // purpose of aggro check/burst unknown, something to do with failing quest?

    SPELL_AGGRO_CHECK_MUCKJAW       = 40847,
    SPELL_AGGRO_CHECK_TROPE         = 40984,
    SPELL_AGGRO_CHECK_CORLOK        = 40992,
    SPELL_AGGRO_CHECK_ICHMAN        = 41006,
    SPELL_AGGRO_CHECK_MULVERICK     = 41014,
    SPELL_AGGRO_CHECK_SKYSHATTER    = 41020,

    SPELL_AGGRO_BURST_MUCKJAW       = 40850,
    SPELL_AGGRO_BURST_TROPE         = 40986,
    SPELL_AGGRO_BURST_CORLOK        = 40994,
    SPELL_AGGRO_BURST_ICHMAN        = 41008,
    SPELL_AGGRO_BURST_MULVERICK     = 41016,
    SPELL_AGGRO_BURST_SKYSHATTER    = 41023,

    SPELL_ATTACK_TROPE              = 40909,
    SPELL_ATTACK_CORLOK             = 40894,
    SPELL_ATTACK_ICHMAN             = 40928,
    SPELL_ATTACK_MULVERICK          = 40930,
    SPELL_ATTACK_SKYSHATTER         = 40945,

    // responses of target NPCs handled in ACID
    NPC_TARGET_TROPE                = 23357,
    NPC_TARGET_CORLOK               = 23358,
    NPC_TARGET_ICHMAN               = 23359,
    NPC_TARGET_MULVERICK            = 23360,
    NPC_TARGET_SKYSHATTER           = 23361,

    RACER_MUCKJAW                   = 0,
    RACER_TROPE                     = 1,
    RACER_CORLOK                    = 2,
    RACER_ICHMAN                    = 3,
    RACER_MULVERICK                 = 4,
    RACER_SKYSHATTER                = 5,

    RACER_COUNT                     = 6
};

struct DragonmawRacerScriptInfo
{
    uint32 questId;
    int32 startText;
    int32 attackSpellId;
    uint32 targetNpc;
    int32 checkSpellId;
    int32 burstSpellId;
    int32 winText;
};

static DragonmawRacerScriptInfo dragonmawRacesScriptInfo[RACER_COUNT] =
{
    { QUEST_MUCKJAW, SAY_START_MUCKJAW, 0, 0, SPELL_AGGRO_CHECK_MUCKJAW, SPELL_AGGRO_BURST_MUCKJAW, SAY_END_MUCKJAW }, // doesn't attack and has no target NPC
    { QUEST_TROPE, SAY_START_TROPE, SPELL_ATTACK_TROPE, NPC_TARGET_TROPE, SPELL_AGGRO_CHECK_TROPE, SPELL_AGGRO_BURST_TROPE, SAY_END_TROPE },
    { QUEST_CORLOK, SAY_START_CORLOK, SPELL_ATTACK_CORLOK, NPC_TARGET_CORLOK, SPELL_AGGRO_CHECK_CORLOK, SPELL_AGGRO_BURST_CORLOK, SAY_END_CORLOK },
    { QUEST_ICHMAN, SAY_START_ICHMAN, SPELL_ATTACK_ICHMAN, NPC_TARGET_ICHMAN, SPELL_AGGRO_CHECK_ICHMAN, SPELL_AGGRO_BURST_ICHMAN, SAY_END_ICHMAN },
    { QUEST_MULVERICK, SAY_START_MULVERICK, SPELL_ATTACK_MULVERICK, NPC_TARGET_MULVERICK, SPELL_AGGRO_CHECK_MULVERICK, SPELL_AGGRO_BURST_MULVERICK, SAY_END_MULVERICK },
    { QUEST_SKYSHATTER, SAY_START_SKYSHATTER, SPELL_ATTACK_SKYSHATTER, NPC_TARGET_SKYSHATTER, SPELL_AGGRO_CHECK_SKYSHATTER, SPELL_AGGRO_BURST_SKYSHATTER, SAY_END_SKYSHATTER }
};

struct npc_dragonmaw_racerAI : public ScriptedAI
{
    npc_dragonmaw_racerAI(Creature* creature, uint8 racerId) : ScriptedAI(creature), m_racerId(racerId) { Reset(); }

    uint8 m_racerId;
    uint32 m_uiAttackTimer;
    uint32 m_questId;
    ObjectGuid m_playerRacerGuid;

    void Reset() override
    {
        m_creature->SetHover(false);
        m_creature->SetLevitate(false);
        m_creature->SetWalk(true);
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_uiAttackTimer = 0;
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_uiAttackTimer)
        {
            if (m_uiAttackTimer <= diff)
            {
                AttackPlayer();
            }
            else
                m_uiAttackTimer -= diff;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        DoCastSpellIfCan(pSummoned, dragonmawRacesScriptInfo[m_racerId].attackSpellId);
    }

    void Takeoff()
    {
        m_creature->SetHover(true);
        m_creature->SetLevitate(true);
    }

    void StartRace()
    {
        m_creature->SetWalk(false);

        if (dragonmawRacesScriptInfo[m_racerId].checkSpellId)
            DoCastSpellIfCan(m_creature, dragonmawRacesScriptInfo[m_racerId].checkSpellId); // begins failure checking
    }

    void FailRace()
    {
        m_creature->RemoveAurasDueToSpell(dragonmawRacesScriptInfo[m_racerId].burstSpellId);
        if (Player* player = m_creature->GetMap()->GetPlayer(m_playerRacerGuid))
            player->FailQuest(m_questId);
        m_creature->ForcedDespawn();
    }

    void StartAttack()
    {
        if (dragonmawRacesScriptInfo[m_racerId].attackSpellId)
            m_uiAttackTimer = urand(1000,3000);

        if (dragonmawRacesScriptInfo[m_racerId].burstSpellId)
            DoCastSpellIfCan(m_creature, dragonmawRacesScriptInfo[m_racerId].burstSpellId);
    }

    virtual void AttackPlayer()
    {
        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerRacerGuid))
        {
            float fX, fY, fZ;
            m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 15.0f, fX, fY, fZ);
            m_creature->SummonCreature(dragonmawRacesScriptInfo[m_racerId].targetNpc, fX, fY, fZ, 0, TEMPSPAWN_TIMED_DESPAWN, 6000);
            m_creature->GetRandomPoint(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), 10.0f, fX, fY, fZ);
            m_creature->SummonCreature(dragonmawRacesScriptInfo[m_racerId].targetNpc, fX, fY, fZ, 0, TEMPSPAWN_TIMED_DESPAWN, 6000);
        }
        m_uiAttackTimer = 4000;
    }

    void FinishRace()
    {
        m_creature->SetHover(false);
        m_creature->SetLevitate(false);
        m_creature->SetWalk(true);
        m_uiAttackTimer = 0;
        m_creature->RemoveAurasDueToSpell(dragonmawRacesScriptInfo[m_racerId].burstSpellId);
        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerRacerGuid))
        {
            DoScriptText(dragonmawRacesScriptInfo[m_racerId].winText, m_creature, pPlayer);
            pPlayer->RewardPlayerAndGroupAtEventExplored(dragonmawRacesScriptInfo[m_racerId].questId, m_creature);
        }
    }
};

struct npc_dragonmaw_racer_muckjawAI : public npc_dragonmaw_racerAI
{
    npc_dragonmaw_racer_muckjawAI(Creature* creature) : npc_dragonmaw_racerAI(creature, RACER_MUCKJAW) {}

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType != PATH_MOTION_TYPE)
            return;
        switch (pointId)
        {
            case 4:
                npc_dragonmaw_racerAI::Takeoff();
                break;
            case 7:
                npc_dragonmaw_racerAI::StartRace();
                break;
            case 9:
                npc_dragonmaw_racerAI::StartAttack();
                break;
            case 35:
                npc_dragonmaw_racerAI::FinishRace();
                break;
            case 37:
                npc_dragonmaw_racerAI::Reset();
                break;
        }
    }
};

struct npc_dragonmaw_racer_tropeAI : public npc_dragonmaw_racerAI
{
    npc_dragonmaw_racer_tropeAI(Creature* creature) : npc_dragonmaw_racerAI(creature, RACER_TROPE) {}

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType != PATH_MOTION_TYPE)
            return;
        switch (pointId)
        {
            case 5:
                npc_dragonmaw_racerAI::Takeoff();
                break;
            case 7:
                npc_dragonmaw_racerAI::StartRace();
                break;
            case 10:
                npc_dragonmaw_racerAI::StartAttack();
                break;
            case 53:
                npc_dragonmaw_racerAI::FinishRace();
                break;
            case 60:
                npc_dragonmaw_racerAI::Reset();
                break;
        }
    }
};

struct npc_dragonmaw_racer_corlokAI : public npc_dragonmaw_racerAI
{
    npc_dragonmaw_racer_corlokAI(Creature* creature) : npc_dragonmaw_racerAI(creature, RACER_CORLOK) {}

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType != PATH_MOTION_TYPE)
            return;
        switch (pointId)
        {
            case 6:
                npc_dragonmaw_racerAI::Takeoff();
                break;
            case 9:
                npc_dragonmaw_racerAI::StartRace();
                break;
            case 12:
                npc_dragonmaw_racerAI::StartAttack();
                break;
            case 79:
                npc_dragonmaw_racerAI::FinishRace();
                break;
            case 89:
                npc_dragonmaw_racerAI::Reset();
                break;
        }
    }
};

struct npc_dragonmaw_racer_ichmanAI : public npc_dragonmaw_racerAI
{
    npc_dragonmaw_racer_ichmanAI(Creature* creature) : npc_dragonmaw_racerAI(creature, RACER_ICHMAN) {}

    void MovementInform(uint32 /*moveType*/, uint32 pointId) override
    {
        switch (pointId)
        {
            case 4:
                npc_dragonmaw_racerAI::Takeoff();
                npc_dragonmaw_racerAI::StartRace();
                break;
            case 12:
                npc_dragonmaw_racerAI::StartAttack();
                break;
            case 107:
                npc_dragonmaw_racerAI::FinishRace();
                break;
            case 111:
                npc_dragonmaw_racerAI::Reset();
                break;
        }
    }
};

struct npc_dragonmaw_racer_mulverickAI : public npc_dragonmaw_racerAI
{
    npc_dragonmaw_racer_mulverickAI(Creature* creature) : npc_dragonmaw_racerAI(creature, RACER_MULVERICK) {}

    void MovementInform(uint32 /*moveType*/, uint32 pointId) override
    {
        switch (pointId)
        {
            case 5:
                npc_dragonmaw_racerAI::Takeoff();
                break;
            case 9:
                npc_dragonmaw_racerAI::StartRace();
                break;
            case 12:
                npc_dragonmaw_racerAI::StartAttack();
                break;
            case 166:
                npc_dragonmaw_racerAI::FinishRace();
                break;
            case 172:
                npc_dragonmaw_racerAI::Reset();
                break;
        }
    }

    void AttackPlayer() override
    {
        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerRacerGuid))
        {
            float fX, fY, fZ;
            m_creature->GetRandomPoint(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), 10.0f, fX, fY, fZ);
            m_creature->SummonCreature(dragonmawRacesScriptInfo[m_racerId].targetNpc, fX, fY, fZ, 0, TEMPSPAWN_TIMED_DESPAWN, 6000);
        }
        m_uiAttackTimer = urand(1000,4000);
    }
};

struct npc_dragonmaw_racer_skyshatterAI : public npc_dragonmaw_racerAI
{
    npc_dragonmaw_racer_skyshatterAI(Creature* creature) : npc_dragonmaw_racerAI(creature, RACER_SKYSHATTER) {}

    void MovementInform(uint32 /*moveType*/, uint32 pointId) override
    {
        switch (pointId)
        {
            case 3:
                npc_dragonmaw_racerAI::Takeoff();
                break;
            case 7:
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerRacerGuid))
                    DoScriptText(SAY_MID_SKYSHATTER, m_creature, pPlayer);
                npc_dragonmaw_racerAI::StartRace();
                break;
            case 10:
                npc_dragonmaw_racerAI::StartAttack();
                break;
            case 140:
                npc_dragonmaw_racerAI::FinishRace();
                break;
            case 145:
                npc_dragonmaw_racerAI::Reset();
                break;
        }
    }

    void AttackPlayer() override
    {
        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerRacerGuid))
        {
            float fX, fY, fZ;
            m_creature->GetRandomPoint(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), 5.0f, fX, fY, fZ);
            m_creature->SummonCreature(dragonmawRacesScriptInfo[m_racerId].targetNpc, fX, fY, fZ, 0, TEMPSPAWN_TIMED_DESPAWN, 6000);
        }
        m_uiAttackTimer = urand(1000, 2000);
    }
};

bool QuestAccept_npc_dragonmaw_racer(Player* player, Creature* questgiver, Quest const* quest)
{
    switch (quest->GetQuestId())
    {
        case QUEST_MUCKJAW:
        case QUEST_TROPE:
        case QUEST_CORLOK:
        case QUEST_ICHMAN:
        case QUEST_MULVERICK:
        case QUEST_SKYSHATTER:
            if (npc_dragonmaw_racerAI* ai = static_cast<npc_dragonmaw_racerAI*>(questgiver->AI()))
            {
                DoScriptText(dragonmawRacesScriptInfo[ai->m_racerId].startText, questgiver, player);
                ai->m_playerRacerGuid = player->GetObjectGuid();
                ai->m_questId = quest->GetQuestId();
                questgiver->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
                questgiver->GetMotionMaster()->MovePath(0, PATH_FROM_EXTERNAL);
            }
            return true;
    }

    return false;
}

struct DragonmawKnockdownTheAggroCheck : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (!spell->GetUnitTarget()->IsPlayer() || spell->GetCaster()->IsPlayer())
            return;

        if (npc_dragonmaw_racerAI* ai = static_cast<npc_dragonmaw_racerAI*>(spell->GetCaster()->AI()))
            if (((Player*)spell->GetUnitTarget())->GetObjectGuid() == ai->m_playerRacerGuid)
                spell->SetScriptValue(1); // found player within 100 yds still - race may continue
    }

    void OnSuccessfulFinish(Spell* spell) const override
    {
        if (spell->GetCaster()->IsPlayer())
            return;

        if (!spell->GetScriptValue()) // lost race - player got too far away
            if (npc_dragonmaw_racerAI* ai = static_cast<npc_dragonmaw_racerAI*>(spell->GetCaster()->AI()))
                ai->FailRace();
    }
};

/*######
## mob_bt_battle_fighter
######*/

enum {
    NPC_DOOMWALKER            = 17711,
    NPC_FORMATION_MARKER      = 19179,
    NPC_ILLIDARI_RAVAGER      = 22857,
    NPC_SHADOWHOOF_ASSASSIN   = 22858,
    NPC_ILLIDARI_SUCCUBUS     = 22860,
    NPC_LIGHTSWORN_VINDICATOR = 22861,
    NPC_ANCHORITE_CAALEN      = 22862,
    NPC_SEASONED_MAGISTER     = 22863,
    NPC_FYRA_DAWNSTAR         = 22864,
    NPC_BT_BATTLE_SENOR       = 22934,
    NPC_SHADOWLORD            = 22988,

    SAY_VINDICATOR_ON_AGGRO_1 = -1015062,
    SAY_MAG_ON_AGGRO_1        = -1015063,
    SAY_MAG_ON_AGGRO_2        = -1015064,
    SAY_MAG_ON_AGGRO_3        = -1015065,
    SAY_CAALEN_FORWARD        = -1015032,
    SAY_FYRA_ONWARD	          = -1015031,

    ILLIDARI_ATTACK_INTERVAL  = 150000,
    REINFORCE_INTERVAL        = 30000,

    // Spells
    SPELL_BATTLE_FLOW_REGULATOR        = 39699, // The BT Battle Sensor NPC casts this spell/aura, presumably using it to control the constant demon/aldor/scryer fight outside of BT, no clue how it works

    SPELL_RAVAGER_CLEAVE               = 15496,
    SPELL_RAVAGER_CUTDOWN              = 32009,
    SPELL_RAVAGER_DEMORALIZING_SHOUT   = 16244,

    SPELL_ASSASSIN_DEBILITATING_STRIKE = 37577,
    SPELL_ASSASSIN_SINISTER_STRIKE     = 14873,

    SPELL_SUCCUBUS_LASH_OF_PAIN        = 32202,
    SPELL_SUCCUBUS_SEDUCTION           = 31865,

    SPELL_VINDICATOR_EXORCISM          = 33632,
    SPELL_VINDICATOR_HAMMER            = 13005,
    SPELL_VINDICATOR_HOLY_LIGHT        = 13952,
    SPELL_VINDICATOR_SEAL_OF_SAC       = 13903,

    SPELL_CAALEN_HOLY_SMITE            = 20696,
    SPELL_CAALEN_PRAYER_OF_HEALING     = 35943,

    SPELL_MAGISTER_FIREBALL            = 9053,

    SPELL_SHADOWLORD_INFERNO           = 39941,
    SPELL_SHADOWLORD_CARRION_SWARM     = 39942,
    SPELL_SHADOWLORD_SLEEP             = 12098,
};

enum
{
    ILLIDARI_CLEAVE,
    ILLIDARI_CUTDOWN,
    ILLIDARI_DEMORALIZING_SHOUT,
    ILLIDARI_ACTION_MAX,
};

enum
{
    ASSASSIN_DEBILITATING_STRIKE,
    ASSASSIN_SINISTER_STRIKE,
    ASSASSIN_ACTION_MAX,
};

enum
{
    SUCCUBUS_LASH_OF_PAIN,
    SUCCUBUS_SEDUCTION,
    SUCCUBUS_ACTION_MAX,
};

enum
{
    VINDICATOR_EXORCISM,
    VINDICATOR_HAMMER,
    VINDICATOR_HOLY_LIGHT,
    VINDICATOR_SEAL_OF_SAC,
    VINDICATOR_ACTION_MAX,
};

enum
{
    CAALEN_HOLY_SMITE,
    CAALEN_PRAYER_OF_HEALING,
    CAALEN_ACTION_MAX,
};

enum
{
    MAGISTER_FIREBALL, // FLEE AT 15%
    MAGISTER_ACTION_MAX,
};

enum
{
    SHADOWLORD_INFERNO,
    SHADOWLORD_CARRION_SWARM,
    SHADOWLORD_SLEEP,
    SHADOWLORD_ACTION_MAX,
};

struct mob_bt_battle_fighterAI : public ScriptedAI
{
    mob_bt_battle_fighterAI(Creature* pCreature) : ScriptedAI(pCreature, VINDICATOR_ACTION_MAX)
    {
        switch (m_creature->GetEntry())
        {
            case NPC_ILLIDARI_RAVAGER:
            {
                AddCombatAction(ILLIDARI_CLEAVE, 0u);
                AddCombatAction(ILLIDARI_CUTDOWN, 0u);
                AddCombatAction(ILLIDARI_DEMORALIZING_SHOUT, 0u);
                break;
            }
            case NPC_SHADOWHOOF_ASSASSIN:
            {
                AddCombatAction(ASSASSIN_DEBILITATING_STRIKE, 0u);
                AddCombatAction(ASSASSIN_SINISTER_STRIKE, 0u);
                break;
            }
            case NPC_ILLIDARI_SUCCUBUS:
            {
                AddCombatAction(SUCCUBUS_LASH_OF_PAIN, 0u);
                AddCombatAction(SUCCUBUS_SEDUCTION, 0u);
                break;
            }
            case NPC_LIGHTSWORN_VINDICATOR:
            {
                AddCombatAction(VINDICATOR_EXORCISM, 0u);
                AddCombatAction(VINDICATOR_HAMMER, 0u);
                AddCombatAction(VINDICATOR_HOLY_LIGHT, 0u);
                AddCombatAction(VINDICATOR_SEAL_OF_SAC, 0u);
                break;
            }
            case NPC_ANCHORITE_CAALEN:
            {
                AddCombatAction(CAALEN_HOLY_SMITE, 0u);
                AddCombatAction(CAALEN_PRAYER_OF_HEALING, 0u);
                break;
            }
            case NPC_SEASONED_MAGISTER:
            {
                AddCombatAction(MAGISTER_FIREBALL, 0u);
                break;
            }
            case NPC_SHADOWLORD:
            {
                AddCombatAction(SHADOWLORD_INFERNO, 0u);
                AddCombatAction(SHADOWLORD_CARRION_SWARM, 0u);
                AddCombatAction(SHADOWLORD_SLEEP, 0u);

                float m_fMidPoint = -3558.0f;
                bool left_side = (m_creature->GetPositionX() < m_fMidPoint);
                m_uiPathId = left_side ? 0 : 1;

                break;
            }
        }

        Reset();
    }

    uint8 m_uiPathId = 0; // only used for the Shadowlords
    uint8 m_uiLastWaypoint = 0;
    bool m_bIsWaypointing = true;

    void Reset() override
    {
        ScriptedAI::Reset();

        switch (m_creature->GetEntry())
        {
            case NPC_ILLIDARI_RAVAGER:
            {
                for (uint32 i = 0; i < ILLIDARI_ACTION_MAX; ++i)
                    SetActionReadyStatus(i, false);

                ResetTimer(ILLIDARI_CLEAVE, GetInitialActionTimer(ILLIDARI_CLEAVE));
                ResetTimer(ILLIDARI_CUTDOWN, GetInitialActionTimer(ILLIDARI_CUTDOWN));
                ResetTimer(ILLIDARI_DEMORALIZING_SHOUT, GetInitialActionTimer(ILLIDARI_DEMORALIZING_SHOUT));

                break;
            }
            case NPC_SHADOWHOOF_ASSASSIN:
            {
                for (uint32 i = 0; i < ASSASSIN_ACTION_MAX; ++i)
                    SetActionReadyStatus(i, false);

                ResetTimer(ASSASSIN_DEBILITATING_STRIKE, GetInitialActionTimer(ASSASSIN_DEBILITATING_STRIKE));
                ResetTimer(ASSASSIN_SINISTER_STRIKE, GetInitialActionTimer(ASSASSIN_SINISTER_STRIKE));

                break;
            }
            case NPC_ILLIDARI_SUCCUBUS:
            {
                for (uint32 i = 0; i < SUCCUBUS_ACTION_MAX; ++i)
                    SetActionReadyStatus(i, false);

                ResetTimer(SUCCUBUS_LASH_OF_PAIN, GetInitialActionTimer(SUCCUBUS_LASH_OF_PAIN));
                ResetTimer(SUCCUBUS_SEDUCTION, GetInitialActionTimer(SUCCUBUS_SEDUCTION));

                break;
            }
            case NPC_LIGHTSWORN_VINDICATOR:
            {
                for (uint32 i = 0; i < VINDICATOR_ACTION_MAX; ++i)
                    SetActionReadyStatus(i, false);

                ResetTimer(VINDICATOR_EXORCISM, GetInitialActionTimer(VINDICATOR_EXORCISM));
                ResetTimer(VINDICATOR_HAMMER, GetInitialActionTimer(VINDICATOR_HAMMER));
                ResetTimer(VINDICATOR_HOLY_LIGHT, GetInitialActionTimer(VINDICATOR_HOLY_LIGHT));
                ResetTimer(VINDICATOR_SEAL_OF_SAC, GetInitialActionTimer(VINDICATOR_SEAL_OF_SAC));

                break;
            }
            case NPC_ANCHORITE_CAALEN:
            {
                for (uint32 i = 0; i < CAALEN_ACTION_MAX; ++i)
                    SetActionReadyStatus(i, false);

                ResetTimer(CAALEN_HOLY_SMITE, GetInitialActionTimer(CAALEN_HOLY_SMITE));
                ResetTimer(CAALEN_PRAYER_OF_HEALING, GetInitialActionTimer(CAALEN_PRAYER_OF_HEALING));

                break;
            }
            case NPC_SEASONED_MAGISTER:
            {
                for (uint32 i = 0; i < MAGISTER_ACTION_MAX; ++i)
                    SetActionReadyStatus(i, false);

                ResetTimer(MAGISTER_FIREBALL, GetInitialActionTimer(MAGISTER_FIREBALL));
                SetCombatMovement(true);

                break;
            }
            case NPC_SHADOWLORD:
            {
                for (uint32 i = 0; i < SHADOWLORD_ACTION_MAX; ++i)
                    SetActionReadyStatus(i, false);

                ResetTimer(SHADOWLORD_INFERNO, GetInitialActionTimer(SHADOWLORD_INFERNO));
                ResetTimer(SHADOWLORD_CARRION_SWARM, GetInitialActionTimer(SHADOWLORD_CARRION_SWARM));
                ResetTimer(SHADOWLORD_SLEEP, GetInitialActionTimer(SHADOWLORD_SLEEP));

                break;
            }
        }

        SetCombatScriptStatus(false);
        SetCombatMovement(true);
    }

    uint32 GetInitialActionTimer(const uint32 action) const
    {
        switch (m_creature->GetEntry())
        {
            case NPC_ILLIDARI_RAVAGER:
            {
                switch (action)
                {
                    case ILLIDARI_CLEAVE: return urand(4000, 7000);
                    case ILLIDARI_CUTDOWN: return 6000;
                    case ILLIDARI_DEMORALIZING_SHOUT: return 8000;
                    default: return 0;
                }
            }
            case NPC_SHADOWHOOF_ASSASSIN:
            {
                switch (action)
                {
                    case ASSASSIN_DEBILITATING_STRIKE: return urand(3500, 4000);
                    case ASSASSIN_SINISTER_STRIKE: return urand(6000, 8000);
                    default: return 0;
                }
            }
            case NPC_ILLIDARI_SUCCUBUS:
            {
                switch (action)
                {
                    case SUCCUBUS_LASH_OF_PAIN: return urand(2000, 4000);
                    case SUCCUBUS_SEDUCTION: return urand(5000, 14000);
                    default: return 0;
                }
            }
            case NPC_LIGHTSWORN_VINDICATOR:
            {
                switch (action)
                {
                    case VINDICATOR_EXORCISM: return urand(3000, 12000);
                    case VINDICATOR_HAMMER: return urand(5000, 15000);
                    case VINDICATOR_HOLY_LIGHT: return urand(2500, 4000);
                    case VINDICATOR_SEAL_OF_SAC: return urand(1000, 4000);
                    default: return 0;
                }
            }
            case NPC_ANCHORITE_CAALEN:
            {
                switch (action)
                {
                    case CAALEN_HOLY_SMITE: return urand(3000, 9000);
                    case CAALEN_PRAYER_OF_HEALING: return urand(9000, 12000);
                    default: return 0;
                }
            }
            case NPC_SEASONED_MAGISTER:
            {
                switch (action)
                {
                    case MAGISTER_FIREBALL: return urand(0, 1000);
                    default: return 0;
                }
            }
            case NPC_SHADOWLORD:
            {
                switch (action)
                {
                    case SHADOWLORD_INFERNO: return urand(5000, 10000);
                    case SHADOWLORD_CARRION_SWARM: return urand(0, 2000);
                    case SHADOWLORD_SLEEP: return urand(1500, 5000);
                    default: return 0;
                }
            }
        }
        return 0;
    }

    uint32 GetSubsequentActionTimer(const uint32 action) const
    {
        switch (m_creature->GetEntry())
        {
            case NPC_ILLIDARI_RAVAGER:
            {
                switch (action)
                {
                    case ILLIDARI_CLEAVE: return urand(9500, 12000);
                    case ILLIDARI_CUTDOWN: return urand(14000, 16000);
                    case ILLIDARI_DEMORALIZING_SHOUT: return urand(15000, 18000);
                    default: return 0;
                }
            }
            case NPC_SHADOWHOOF_ASSASSIN:
            {
                switch (action)
                {
                    case ASSASSIN_DEBILITATING_STRIKE: return urand(10000, 12000);
                    case ASSASSIN_SINISTER_STRIKE: return urand(11000, 18000);
                    default: return 0;
                }
            }
            case NPC_ILLIDARI_SUCCUBUS:
            {
                switch (action)
                {
                    case SUCCUBUS_LASH_OF_PAIN: return urand(4000, 7000);
                    case SUCCUBUS_SEDUCTION: return urand(12000, 18000);
                    default: return 0;
                }
            }
            case NPC_LIGHTSWORN_VINDICATOR:
            {
                switch (action)
                {
                    case VINDICATOR_EXORCISM: return urand(15000, 28000);
                    case VINDICATOR_HAMMER: return urand(18000, 24000);
                    case VINDICATOR_HOLY_LIGHT: return urand(12000, 18000);
                    case VINDICATOR_SEAL_OF_SAC: return urand(30000, 45000);
                    default: return 0;
                }
            }
            case NPC_ANCHORITE_CAALEN:
            {
                switch (action)
                {
                    case CAALEN_HOLY_SMITE: return urand(8000, 13000);
                    case CAALEN_PRAYER_OF_HEALING: return urand(12000, 15000);
                    default: return 0;
                }
            }
            case NPC_SEASONED_MAGISTER:
            {
                switch (action)
                {
                    case MAGISTER_FIREBALL: return urand(3400, 4800);
                    default: return 0;
                }
            }
            case NPC_SHADOWLORD:
            {
                switch (action)
                {
                    case SHADOWLORD_INFERNO: return urand(15000, 25000);
                    case SHADOWLORD_CARRION_SWARM: return urand(10000, 12000);
                    case SHADOWLORD_SLEEP: return urand(10000, 16000);
                    default: return 0;
                }
            }
        }
        return 0;
    }

    void ExecuteActions()
    {
        if (!CanExecuteCombatAction())
            return;

        switch (m_creature->GetEntry())
        {
            case NPC_ILLIDARI_RAVAGER:		ExecuteRavagerActions(); break;
            case NPC_SHADOWHOOF_ASSASSIN:   ExecuteAssassinActions(); break;
            case NPC_ILLIDARI_SUCCUBUS:		ExecuteSuccubusActions(); break;
            case NPC_LIGHTSWORN_VINDICATOR:	ExecuteVindicatorActions(); break;
            case NPC_ANCHORITE_CAALEN:		ExecuteCaalenActions(); break;
            case NPC_SEASONED_MAGISTER:		ExecuteMagisterActions(); break;
            case NPC_SHADOWLORD:			ExecuteShadowlordActions(); break;
        }
    }

    void ExecuteRavagerActions()
    {
        for (uint32 i = 0; i < ILLIDARI_ACTION_MAX; ++i)
            if (GetActionReadyStatus(i))
                switch (i)
                {
                    case ILLIDARI_CLEAVE:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_RAVAGER_CLEAVE) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                    case ILLIDARI_CUTDOWN:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_RAVAGER_CUTDOWN) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                    case ILLIDARI_DEMORALIZING_SHOUT:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_RAVAGER_DEMORALIZING_SHOUT) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                }
    }

    void ExecuteAssassinActions()
    {
        for (uint32 i = 0; i < ASSASSIN_ACTION_MAX; ++i)
            if (GetActionReadyStatus(i))
                switch (i)
                {
                    case ASSASSIN_DEBILITATING_STRIKE:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ASSASSIN_DEBILITATING_STRIKE) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                    case ASSASSIN_SINISTER_STRIKE:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ASSASSIN_SINISTER_STRIKE) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                }
    }

    void ExecuteSuccubusActions()
    {
        for (uint32 i = 0; i < SUCCUBUS_ACTION_MAX; ++i)
            if (GetActionReadyStatus(i))
                switch (i)
                {
                    case SUCCUBUS_LASH_OF_PAIN:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SUCCUBUS_LASH_OF_PAIN) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                    case SUCCUBUS_SEDUCTION:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SUCCUBUS_SEDUCTION) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                }
    }

    void ExecuteVindicatorActions()
    {
        for (uint32 i = 0; i < VINDICATOR_ACTION_MAX; ++i)
            if (GetActionReadyStatus(i))
                switch (i)
                {
                    case VINDICATOR_EXORCISM:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_VINDICATOR_EXORCISM) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                    case VINDICATOR_HAMMER:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_VINDICATOR_HAMMER) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                    case VINDICATOR_HOLY_LIGHT:
                    {
                        if (Unit* pUnit = DoSelectLowestHpFriendly(20.0f, 1))
                            DoCastSpellIfCan(pUnit, SPELL_VINDICATOR_HOLY_LIGHT);

                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        break;
                    }
                    case VINDICATOR_SEAL_OF_SAC:
                    {
                        if (Unit* pUnit = DoSelectLowestHpFriendly(20.0f, 1))
                        {
                            DoCastSpellIfCan(pUnit, SPELL_VINDICATOR_SEAL_OF_SAC);

                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                }
    }

    void ExecuteCaalenActions()
    {
        for (uint32 i = 0; i < CAALEN_ACTION_MAX; ++i)
            if (GetActionReadyStatus(i))
                switch (i)
                {
                    case CAALEN_HOLY_SMITE:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CAALEN_HOLY_SMITE) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                    case CAALEN_PRAYER_OF_HEALING:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CAALEN_PRAYER_OF_HEALING) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                }
    }

    void ExecuteMagisterActions()
    {
        for (uint32 i = 0; i < MAGISTER_ACTION_MAX; ++i)
            if (GetActionReadyStatus(i))
                switch (i)
                {
                    case MAGISTER_FIREBALL:
                    {
                        CanCastResult canCast = DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MAGISTER_FIREBALL);

                        switch (canCast)
                        {
                        case CAST_OK:
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);

                            break;
                        }
                        case CAST_FAIL_NOT_IN_LOS:
                        case CAST_FAIL_TOO_CLOSE:
                        case CAST_FAIL_TOO_FAR:
                        case CAST_FAIL_POWER:
                        {
                            SetCombatMovement(true);
                            break;
                        }
                        default: break;
                        }

                        break;
                    }
                }
    }

    void ExecuteShadowlordActions()
    {
        for (uint32 i = 0; i < SHADOWLORD_ACTION_MAX; ++i)
            if (GetActionReadyStatus(i))
                switch (i)
                {
                    case SHADOWLORD_INFERNO:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOWLORD_INFERNO) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                    case SHADOWLORD_CARRION_SWARM:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOWLORD_CARRION_SWARM) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                    case SHADOWLORD_SLEEP:
                    {
                        // Pretty much guessing how this spell should be used
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOWLORD_SLEEP) == CAST_OK)
                        {
                            m_creature->getThreatManager().modifyThreatPercent(m_creature->GetVictim(), -90);

                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        break;
                    }
                }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        UpdateTimers(uiDiff, m_creature->IsInCombat());

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        ExecuteActions();

        DoMeleeAttackIfReady();
    }

    void Aggro(Unit* who) override
    {
        switch (m_creature->GetEntry())
        {
            case NPC_LIGHTSWORN_VINDICATOR:
            {
                if (!urand(0, 9))
                {
                    DoScriptText(SAY_VINDICATOR_ON_AGGRO_1, m_creature, who); // missing texts?
                }

                break;
            }
            case NPC_SEASONED_MAGISTER:
            {
                if (!urand(0, 9))
                {
                    if (urand(0, 2))
                        DoScriptText(urand(0, 1) ? SAY_MAG_ON_AGGRO_1 : SAY_MAG_ON_AGGRO_2, m_creature, who);
                    else
                        DoScriptText(SAY_MAG_ON_AGGRO_3, m_creature, who);
                }

                SetCombatMovement(false);
                break;
            }
        }
    }

    /*void JustRespawned() override
    {

    }*/

    void JustReachedHome() override
    {
        if (!m_bIsWaypointing)
            return;

        switch (m_creature->GetEntry())
        {
            case NPC_ILLIDARI_RAVAGER:
            case NPC_SHADOWHOOF_ASSASSIN:
            case NPC_ILLIDARI_SUCCUBUS:
            {
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveWaypoint(0, 1, 1000);
                m_creature->GetMotionMaster()->SetNextWaypoint(m_uiLastWaypoint + 1);
                break;
            }
            case NPC_LIGHTSWORN_VINDICATOR:
            case NPC_ANCHORITE_CAALEN:
            case NPC_SEASONED_MAGISTER:
            case NPC_FYRA_DAWNSTAR:
            {
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveWaypoint(0, 2, 1000);
                m_creature->GetMotionMaster()->SetNextWaypoint(m_uiLastWaypoint + 1);
                break;
            }
            case NPC_SHADOWLORD:
            {
                if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
                {
                    m_creature->GetMotionMaster()->Clear(false, true);
                    m_creature->GetMotionMaster()->MoveWaypoint(m_uiPathId, 2, 1000);
                    m_creature->GetMotionMaster()->SetNextWaypoint(m_uiLastWaypoint + 1);
                }
                break;
            }
        }
    }

    void MovementInform(uint32 movementType, uint32 data) override
    {
        if (movementType == WAYPOINT_MOTION_TYPE)
            m_uiLastWaypoint = data;
    }
};

UnitAI* GetAI_mob_bt_battle_fighter(Creature* pCreature)
{
    return new mob_bt_battle_fighterAI(pCreature);
}

/*######
## npc_bt_battle_sensor
######*/

struct FormationMap
{
    std::map<ObjectGuid, ObjectGuid> markerToOccupant;
    std::map<ObjectGuid, ObjectGuid> occupantToMarker;

    void clearMaps()
    {
        markerToOccupant.clear();
        occupantToMarker.clear();
    }

    void registerFormationMarker(ObjectGuid marker)
    {
        markerToOccupant[marker] = ObjectGuid();
    }

    // Returns the marker claimed
    ObjectGuid claimFormationMarker(ObjectGuid occupant)
    {
        if (occupant.IsEmpty())
            return ObjectGuid();

        if (occupantToMarker.count(occupant))
            return occupantToMarker[occupant];

        std::vector<ObjectGuid> openMarkers;

        for (std::map<ObjectGuid, ObjectGuid>::iterator itr = markerToOccupant.begin(); itr != markerToOccupant.end(); ++itr)
            if (itr->second.IsEmpty())
                openMarkers.push_back(itr->first);

        ObjectGuid marker;

        if (openMarkers.size() > 0)
        {
            marker = openMarkers[urand(0, openMarkers.size() - 1)];

            markerToOccupant[marker] = occupant;
            occupantToMarker[occupant] = marker;

            return marker;
        }
        else
            return marker;
    }

    bool releaseFormationMarker(ObjectGuid occupant)
    {
        if (!occupantToMarker.count(occupant))
            return false;

        markerToOccupant[occupantToMarker[occupant]] = ObjectGuid();
        occupantToMarker.erase(occupant);
        return true;
    }

    ObjectGuid getRandomOccupant()
    {
        if (occupantToMarker.empty())
            return ObjectGuid();

        uint32 chance = occupantToMarker.size();
        for (auto itr = occupantToMarker.begin(); itr != occupantToMarker.end(); ++itr)
        {
            if (urand(0, chance--) == 0)
                return itr->first;
        }

        return ObjectGuid();
    }

    void EngageInBlackTempleAssault(int numberTroops, int point, Map* map)
    {
        int count = 0;

        GuidVector guids;

        for (auto itr = occupantToMarker.begin(); itr != occupantToMarker.end(); ++itr)
        {
            if (Creature* occupant = map->GetCreature(itr->first))
            {
                if (occupant->IsInCombat())
                    continue;

                occupant->GetMotionMaster()->Clear(false, true);
                occupant->GetMotionMaster()->MoveWaypoint(0, 2);
                occupant->GetMotionMaster()->SetNextWaypoint(point);
                occupant->SetWalk(false);

                if (mob_bt_battle_fighterAI* occupantAI = dynamic_cast<mob_bt_battle_fighterAI*>(occupant->AI()))
                    occupantAI->m_bIsWaypointing = true;

                guids.push_back(itr->first);

                ++count;
            }

            if (count >= numberTroops)
                break;
        }

        for (ObjectGuid guid : guids)
            releaseFormationMarker(guid);
    }
};

struct npc_bt_battle_sensor : public ScriptedAI
{
    npc_bt_battle_sensor(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }

    enum IllidariAttackGroup {
        NONE = 0,
        RAVAGERS_1 = 1,
        RAVAGERS_2 = 2,
        RAVAGERS_3 = 4,
        RAVAGERS_4 = 8,
        SHADOWHOOF_1 = 16,
        SHADOWHOOF_2 = 32,
        SHADOWHOOF_3 = 64
    };

    FormationMap m_forwardFormationMap;
    FormationMap m_rearFormationMapAldor;
    FormationMap m_rearFormationMapScryer;

    ObjectGuid m_caalenGuid;
    ObjectGuid m_fyraGuid;

    ObjectGuid m_ravagerLeader1;
    ObjectGuid m_ravagerLeader2;
    ObjectGuid m_ravagerLeader3;
    ObjectGuid m_ravagerLeader4;
    ObjectGuid m_ShadowhoofLeader1;
    ObjectGuid m_ShadowhoofLeader2;
    ObjectGuid m_ShadowhoofLeader3;

    uint8 m_attackReadyMask;

    GuidList m_lIllidariInBattle;
    GuidList m_lGoodGuysInBattle;

    float m_fIllidariAttackTimer;
    float m_fAldorScryerReinforceTimer;
    float m_fMidPoint = -3558.0f;

    uint8 m_uiMinAttackGroupsReady = 5; // do not send an attack, if fewer than this many groups are ready
    uint8 m_uiMaxNumTroopsForward = 8;
    uint8 m_uiNumLightswornForward;
    uint8 m_uiNumMagisterForward;

    void Reset() override
    {
        m_forwardFormationMap.clearMaps();
        m_rearFormationMapAldor.clearMaps();
        m_rearFormationMapScryer.clearMaps();

        m_caalenGuid.Clear();
        m_fyraGuid.Clear();

        m_ravagerLeader1.Clear();
        m_ravagerLeader2.Clear();
        m_ravagerLeader3.Clear();
        m_ravagerLeader4.Clear();
        m_ShadowhoofLeader1.Clear();
        m_ShadowhoofLeader2.Clear();
        m_ShadowhoofLeader3.Clear();

        m_attackReadyMask = 0;

        m_lIllidariInBattle.clear();
        m_lGoodGuysInBattle.clear();

        m_fIllidariAttackTimer = ILLIDARI_ATTACK_INTERVAL;
        m_fAldorScryerReinforceTimer = REINFORCE_INTERVAL;

        m_uiNumLightswornForward = 0;
        m_uiNumMagisterForward = 0;
    }

    /*void SpellHit(Unit* caster, const SpellEntry* spell) override
    {
        if (spell->Id == SPELL_BATTLE_FLOW_REGULATOR)
        {
        }
    }*/

    void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        const ObjectGuid senderGuid = sender->GetObjectGuid();

        switch (sender->GetEntry())
        {
            case NPC_ILLIDARI_RAVAGER:
                /* Expected events:
                * A: Ravager group 1 ready
                * B: Ravager group 2 ready
                * C: Ravager group 3 ready
                * D: Ravager group 4 ready
                * E: Left Ravager leader spawned
                * F: Right Ravager leader spawned
                * Custom_A: Attack */
                switch (eventType)
                {
                    case AI_EVENT_JUST_DIED:
                        m_fIllidariAttackTimer *= 0.75f;
                        break;
                    case AI_EVENT_CUSTOM_EVENTAI_A:
                        if (Creature* leader = m_creature->GetMap()->GetCreature(m_ravagerLeader1))
                        {
                            leader->GetMotionMaster()->Clear(false, true);
                            leader->GetMotionMaster()->MoveIdle();
                            leader->SetWalk(false);

                            if (mob_bt_battle_fighterAI* occupantAI = dynamic_cast<mob_bt_battle_fighterAI*>(leader->AI()))
                                occupantAI->m_bIsWaypointing = false;
                        }

                        m_attackReadyMask += RAVAGERS_1;
                        break;
                    case AI_EVENT_CUSTOM_EVENTAI_B:
                        if (Creature* leader = m_creature->GetMap()->GetCreature(m_ravagerLeader2))
                        {
                            leader->GetMotionMaster()->Clear(false, true);
                            leader->GetMotionMaster()->MoveIdle();
                            leader->SetWalk(false);

                            if (mob_bt_battle_fighterAI* occupantAI = dynamic_cast<mob_bt_battle_fighterAI*>(leader->AI()))
                                occupantAI->m_bIsWaypointing = false;
                        }

                        m_attackReadyMask += RAVAGERS_2;
                        break;
                    case AI_EVENT_CUSTOM_EVENTAI_C:
                        if (Creature* leader = m_creature->GetMap()->GetCreature(m_ravagerLeader3))
                        {
                            leader->GetMotionMaster()->Clear(false, true);
                            leader->GetMotionMaster()->MoveIdle();
                            leader->SetWalk(false);

                            if (mob_bt_battle_fighterAI* occupantAI = dynamic_cast<mob_bt_battle_fighterAI*>(leader->AI()))
                                occupantAI->m_bIsWaypointing = false;
                        }

                        m_attackReadyMask += RAVAGERS_3;
                        break;
                    case AI_EVENT_CUSTOM_EVENTAI_D:
                        if (Creature* leader = m_creature->GetMap()->GetCreature(m_ravagerLeader4))
                        {
                            leader->GetMotionMaster()->Clear(false, true);
                            leader->GetMotionMaster()->MoveIdle();
                            leader->SetWalk(false);

                            if (mob_bt_battle_fighterAI* occupantAI = dynamic_cast<mob_bt_battle_fighterAI*>(leader->AI()))
                                occupantAI->m_bIsWaypointing = false;
                        }

                        m_attackReadyMask += RAVAGERS_4;
                        break;
                    case AI_EVENT_CUSTOM_EVENTAI_E:
                    {
                        bool left_side = (sender->GetPositionX() < m_fMidPoint);

                        if (left_side)
                            m_ravagerLeader1 = senderGuid;
                        else
                            m_ravagerLeader2 = senderGuid;
                        break;
                    }
                    case AI_EVENT_CUSTOM_EVENTAI_F:
                    {
                        bool left_side = (sender->GetPositionX() < m_fMidPoint);

                        if (left_side)
                            m_ravagerLeader3 = senderGuid;
                        else
                            m_ravagerLeader4 = senderGuid;
                        break;
                    }
                    case AI_EVENT_CUSTOM_A:
                    {
                        IllidariAttackNearestEnemy(sender);
                        break;
                    }
                    case AI_EVENT_CUSTOM_B:
                    {
                        if (Creature* senderCreature = dynamic_cast<Creature*>(sender))
                            senderCreature->GetMotionMaster()->MoveRandomAroundPoint(sender->GetPositionX(), sender->GetPositionY(), sender->GetPositionZ(), 15.0f);
                        if (mob_bt_battle_fighterAI* senderAI = dynamic_cast<mob_bt_battle_fighterAI*>(sender->AI()))
                            senderAI->m_bIsWaypointing = false;
                        break;
                    }
                    default: break;
                }

                break;
            case NPC_SHADOWHOOF_ASSASSIN:
                /* Expected events:
                * Died: died
                * A: Shadowhoof Group 1 ready
                * B: Shadowhoof Group 2 ready
                * C: Shadowhoof Group 3 ready
                * D: Shadowhoof Group 1 leader spawned
                * E: Shadowhoof Group 2 leader spawned
                * F: Shadowhoof Group 3 leader spawned
                * Custom_A: Attack */
                switch (eventType)
                {
                    case AI_EVENT_JUST_DIED:
                        m_fIllidariAttackTimer *= 0.85f;
                        break;
                    case AI_EVENT_CUSTOM_EVENTAI_A:
                        if (Creature* leader = m_creature->GetMap()->GetCreature(m_ShadowhoofLeader1))
                        {
                            leader->GetMotionMaster()->Clear(false, true);
                            leader->GetMotionMaster()->MoveIdle();
                            leader->SetWalk(false);

                            if (mob_bt_battle_fighterAI* occupantAI = dynamic_cast<mob_bt_battle_fighterAI*>(leader->AI()))
                                occupantAI->m_bIsWaypointing = false;
                        }

                        m_attackReadyMask += SHADOWHOOF_1;
                        break;
                    case AI_EVENT_CUSTOM_EVENTAI_B:
                        if (Creature* leader = m_creature->GetMap()->GetCreature(m_ShadowhoofLeader2))
                        {
                            leader->GetMotionMaster()->Clear(false, true);
                            leader->GetMotionMaster()->MoveIdle();
                            leader->SetWalk(false);

                            if (mob_bt_battle_fighterAI* occupantAI = dynamic_cast<mob_bt_battle_fighterAI*>(leader->AI()))
                                occupantAI->m_bIsWaypointing = false;
                        }

                        m_attackReadyMask += SHADOWHOOF_2;
                        break;
                    case AI_EVENT_CUSTOM_EVENTAI_C:
                        if (Creature* leader = m_creature->GetMap()->GetCreature(m_ShadowhoofLeader3))
                        {
                            leader->GetMotionMaster()->Clear(false, true);
                            leader->GetMotionMaster()->MoveIdle();
                            leader->SetWalk(false);

                            if (mob_bt_battle_fighterAI* occupantAI = dynamic_cast<mob_bt_battle_fighterAI*>(leader->AI()))
                                occupantAI->m_bIsWaypointing = false;
                        }

                        m_attackReadyMask += SHADOWHOOF_3;
                        break;
                    case AI_EVENT_CUSTOM_EVENTAI_D:
                    {
                        m_ShadowhoofLeader1 = senderGuid;
                        break;
                    }
                    case AI_EVENT_CUSTOM_EVENTAI_E:
                    {
                        m_ShadowhoofLeader2 = senderGuid;
                        break;
                    }
                    case AI_EVENT_CUSTOM_EVENTAI_F:
                    {
                        m_ShadowhoofLeader3 = senderGuid;
                        break;
                    }
                    case AI_EVENT_CUSTOM_A:
                    {
                        IllidariAttackNearestEnemy(sender);
                        break;
                    }
                    case AI_EVENT_CUSTOM_B:
                    {
                        if (Creature* senderCreature = dynamic_cast<Creature*>(sender))
                            senderCreature->GetMotionMaster()->MoveRandomAroundPoint(sender->GetPositionX(), sender->GetPositionY(), sender->GetPositionZ(), 15.0f);
                        if (mob_bt_battle_fighterAI* senderAI = dynamic_cast<mob_bt_battle_fighterAI*>(sender->AI()))
                            senderAI->m_bIsWaypointing = false;
                        break;
                    }
                    default: break;
                }

                break;
            case NPC_LIGHTSWORN_VINDICATOR:
                /* Expected events:
                * Died: died
                * A: Aldor/Scryer guys need formation assignment
                * B: Begin running around if nobody to fight */
                switch (eventType)
                {
                    case AI_EVENT_JUST_DIED:
                    {
                        if (m_forwardFormationMap.releaseFormationMarker(senderGuid))
                            --m_uiNumLightswornForward;
                        else
                            m_rearFormationMapAldor.releaseFormationMarker(senderGuid);

                        if (Creature* senderCreature = dynamic_cast<Creature*>(sender))
                            senderCreature->SetActiveObjectState(true);

                        break;
                    }
                    case AI_EVENT_CUSTOM_EVENTAI_A:
                    {
                        ObjectGuid marker = GetAvailableMarkerFromMap(m_rearFormationMapAldor, senderGuid);

                        if (!marker.IsEmpty())
                            if (Creature* markerCreature = m_creature->GetMap()->GetCreature(marker))
                            {
                                sender->GetMotionMaster()->Clear(false, true);
                                sender->GetMotionMaster()->MoveIdle();
                                sender->GetMotionMaster()->MovePoint(0, markerCreature->GetPositionX(), markerCreature->GetPositionY(), markerCreature->GetPositionZ());

                                if (Creature* senderCreature = dynamic_cast<Creature*>(sender))
                                {
                                    senderCreature->SetWalk(true);
                                    senderCreature->SetActiveObjectState(false);
                                }

                                if (mob_bt_battle_fighterAI* senderAI = dynamic_cast<mob_bt_battle_fighterAI*>(sender->AI()))
                                    senderAI->m_bIsWaypointing = false;



                                return;
                            }

                        sender->GetMotionMaster()->Clear(false, true);
                        sender->GetMotionMaster()->MoveIdle();
                        break;
                    }
                    case AI_EVENT_CUSTOM_EVENTAI_B:
                    {
                        Creature* doomwalker = GetClosestCreatureWithEntry(m_creature, NPC_DOOMWALKER, 200.0f);
                        bool doomwalkerDead = doomwalker ? false : true;

                        if (urand(0, 1) || doomwalkerDead)
                        {
                            sender->GetMotionMaster()->Clear(false, true);
                            sender->GetMotionMaster()->MoveWaypoint(0, 3);
                            sender->GetMotionMaster()->SetNextWaypoint(urand(0, 8));

                            if (Creature* senderAI = dynamic_cast<Creature*>(sender))
                                senderAI->ForcedDespawn(90000);
                        }
                        break;
                    }
                    default: break;
                }

                break;
            case NPC_SEASONED_MAGISTER:
                switch (eventType)
                {
                    case AI_EVENT_JUST_DIED:
                    {
                        if (m_forwardFormationMap.releaseFormationMarker(senderGuid))
                            --m_uiNumMagisterForward;
                        else
                            m_rearFormationMapScryer.releaseFormationMarker(senderGuid);

                        if (Creature* senderCreature = dynamic_cast<Creature*>(sender))
                            senderCreature->SetActiveObjectState(true);

                        break;
                    }
                    case AI_EVENT_CUSTOM_EVENTAI_A:
                    {
                        ObjectGuid marker = GetAvailableMarkerFromMap(m_rearFormationMapScryer, senderGuid);

                        if (!marker.IsEmpty())
                            if (Creature* markerCreature = m_creature->GetMap()->GetCreature(marker))
                            {
                                sender->GetMotionMaster()->Clear(false, true);
                                sender->GetMotionMaster()->MoveIdle();
                                sender->GetMotionMaster()->MovePoint(0, markerCreature->GetPositionX(), markerCreature->GetPositionY(), markerCreature->GetPositionZ());

                                if (Creature* senderCreature = dynamic_cast<Creature*>(sender))
                                {
                                    senderCreature->SetWalk(true);
                                    senderCreature->SetActiveObjectState(false);
                                }

                                if (mob_bt_battle_fighterAI* senderAI = dynamic_cast<mob_bt_battle_fighterAI*>(sender->AI()))
                                    senderAI->m_bIsWaypointing = false;

                                return;
                            }

                        sender->GetMotionMaster()->Clear(false, true);
                        sender->GetMotionMaster()->MoveIdle();
                        break;
                    }
                    case AI_EVENT_CUSTOM_EVENTAI_B:
                    {
                        Creature* doomwalker = GetClosestCreatureWithEntry(m_creature, NPC_DOOMWALKER, 200.0f);
                        bool doomwalkerDead = doomwalker ? false : true;

                        if (urand(0, 1) || doomwalkerDead)
                        {
                            sender->GetMotionMaster()->Clear(false, true);
                            sender->GetMotionMaster()->MoveWaypoint(0, 3);
                            sender->GetMotionMaster()->SetNextWaypoint(urand(0, 8));

                            if (Creature* senderAI = dynamic_cast<Creature*>(sender))
                                senderAI->ForcedDespawn(90000);
                        }
                        break;
                    }
                    default: break;
                }

                break;
            case NPC_ANCHORITE_CAALEN:
                switch (eventType)
                {
                    case AI_EVENT_JUST_DIED:
                    {
                        m_caalenGuid.Clear();

                        if (Creature* senderCreature = dynamic_cast<Creature*>(sender))
                            senderCreature->SetActiveObjectState(true);

                        break;
                    }
                    case AI_EVENT_CUSTOM_EVENTAI_A:
                    {
                        sender->GetMotionMaster()->Clear(false, true);
                        sender->GetMotionMaster()->MoveIdle();

                        if (Creature* senderCreature = dynamic_cast<Creature*>(sender))
                            senderCreature->SetActiveObjectState(false);

                        m_caalenGuid = senderGuid;
                        break;
                    }
                    default: break;
                }

                break;
            case NPC_FYRA_DAWNSTAR:
                switch (eventType)
                {
                    case AI_EVENT_JUST_DIED:
                    {
                        m_fyraGuid.Clear();

                        if (Creature* senderCreature = dynamic_cast<Creature*>(sender))
                            senderCreature->SetActiveObjectState(true);

                        break;
                    }
                    case AI_EVENT_CUSTOM_EVENTAI_A:
                    {
                        sender->GetMotionMaster()->Clear(false, true);
                        sender->GetMotionMaster()->MoveIdle();

                        if (Creature* senderCreature = dynamic_cast<Creature*>(sender))
                            senderCreature->SetActiveObjectState(false);

                        m_fyraGuid = senderGuid;
                        break;
                    }
                    case AI_EVENT_CUSTOM_EVENTAI_B:
                        sender->GetMotionMaster()->Clear(false, true);
                        sender->GetMotionMaster()->MoveWaypoint(0, 3);
                        sender->GetMotionMaster()->SetNextWaypoint(0);

                        if (Creature* senderAI = dynamic_cast<Creature*>(sender))
                            senderAI->ForcedDespawn(90000);
                        break;
                    default: break;
                }

                break;
            case NPC_BT_BATTLE_SENOR:
                /* Expected events:
                * A: Fyra/Caalen yells
                * B: CHAAAARGE */
                if (eventType == AI_EVENT_CUSTOM_EVENTAI_A)
                {
                    if (urand(0, 1))
                    {
                        if (Creature* caalen = m_creature->GetMap()->GetCreature(m_caalenGuid))
                            DoScriptText(SAY_CAALEN_FORWARD, caalen);
                    }
                    else
                        if (Creature* fyra = m_creature->GetMap()->GetCreature(m_fyraGuid))
                            DoScriptText(SAY_FYRA_ONWARD, fyra);
                }
                else if (eventType == AI_EVENT_CUSTOM_EVENTAI_B)
                {
                    m_rearFormationMapScryer.EngageInBlackTempleAssault(3, 0, m_creature->GetMap());
                    m_rearFormationMapScryer.EngageInBlackTempleAssault(3, 9, m_creature->GetMap());
                    m_rearFormationMapScryer.EngageInBlackTempleAssault(2, 0, m_creature->GetMap());

                    m_rearFormationMapAldor.EngageInBlackTempleAssault(2, 0, m_creature->GetMap());
                    m_rearFormationMapAldor.EngageInBlackTempleAssault(2, 6, m_creature->GetMap());
                    m_rearFormationMapAldor.EngageInBlackTempleAssault(4, 13, m_creature->GetMap());

                    m_forwardFormationMap.EngageInBlackTempleAssault(4, 3, m_creature->GetMap());
                    m_forwardFormationMap.EngageInBlackTempleAssault(m_uiMaxNumTroopsForward, 12, m_creature->GetMap());

                    if (Creature* caalen = m_creature->GetMap()->GetCreature(m_caalenGuid))
                    {
                        caalen->GetMotionMaster()->Clear(false, true);
                        caalen->GetMotionMaster()->MoveWaypoint(0, 2);
                        caalen->GetMotionMaster()->SetNextWaypoint(0);
                        caalen->SetWalk(false);

                        if (mob_bt_battle_fighterAI* caalenAI = dynamic_cast<mob_bt_battle_fighterAI*>(caalen->AI()))
                            caalenAI->m_bIsWaypointing = true;
                    }
                    if (Creature* fyra = m_creature->GetMap()->GetCreature(m_fyraGuid))
                    {
                        fyra->GetMotionMaster()->Clear(false, true);
                        fyra->GetMotionMaster()->MoveWaypoint(0, 2);
                        fyra->GetMotionMaster()->SetNextWaypoint(0);
                        fyra->SetWalk(false);

                        if (mob_bt_battle_fighterAI* fyraAI = dynamic_cast<mob_bt_battle_fighterAI*>(fyra->AI()))
                            fyraAI->m_bIsWaypointing = false;
                    }

                    m_uiNumLightswornForward = 0;
                    m_uiNumMagisterForward = 0;
                }

                break;
        }
    }

    void IllidariAttackNearestEnemy(Unit* attacker)
    {
        Creature* nearestLightsworn;
        Creature* nearestMagister;

        nearestLightsworn = GetClosestCreatureWithEntry(attacker, NPC_LIGHTSWORN_VINDICATOR, 55.0f);
        nearestMagister = GetClosestCreatureWithEntry(attacker, NPC_SEASONED_MAGISTER, 55.0f);

        if (!nearestLightsworn && !nearestMagister)
            return;

        float distLightsworn = ((nearestLightsworn == nullptr) ? 9999.0f : attacker->GetDistance(nearestLightsworn));

        if (nearestMagister)
        {
            float distMagister = attacker->GetDistance(nearestMagister);
            nearestLightsworn = (distLightsworn < distMagister) ? nearestLightsworn : nearestMagister;
        }

        attacker->AI()->AttackStart(nearestLightsworn);
    }

    ObjectGuid GetAvailableMarkerFromMap(FormationMap& map, ObjectGuid occupant)
    {
        ObjectGuid marker = map.claimFormationMarker(occupant);

        if (!marker.IsEmpty())
            return marker;
        else
        {
            FindFormationMarkers();
            return map.claimFormationMarker(occupant);
        }
    }

    bool FindFormationMarkers()
    {
        m_rearFormationMapScryer.clearMaps();
        m_rearFormationMapAldor.clearMaps();
        std::list<Creature*> lFormationMarkers;

        GetCreatureListWithEntryInGrid(lFormationMarkers, m_creature, NPC_FORMATION_MARKER, 300.0f);

        for (std::list<Creature*>::iterator itr = lFormationMarkers.begin(); itr != lFormationMarkers.end(); ++itr)
            if ((*itr)->GetPositionY() < 540.0f)
                m_forwardFormationMap.registerFormationMarker((*itr)->GetObjectGuid());
            else
            {
                bool left_side = ((*itr)->GetPositionX() < m_fMidPoint);
                bool CaalenFyra = ((*itr)->GetPositionY() > 582.5f);

                if (left_side)
                {
                    if (CaalenFyra)
                        m_fyraGuid = (*itr)->GetObjectGuid();
                    else
                        m_rearFormationMapScryer.registerFormationMarker((*itr)->GetObjectGuid());
                }
                else
                {
                    if (CaalenFyra)
                        m_caalenGuid = (*itr)->GetObjectGuid();
                    else
                        m_rearFormationMapAldor.registerFormationMarker((*itr)->GetObjectGuid());
                }
            }

        if (lFormationMarkers.size() > 0)
            return true;

        return false;
    }

    void ReinforceAldorScryerFrontline()
    {
        while ((m_uiNumLightswornForward + m_uiNumMagisterForward) < m_uiMaxNumTroopsForward)
        {
            ObjectGuid fighter;

            if (urand(0, m_uiMaxNumTroopsForward + m_uiNumLightswornForward - m_uiNumMagisterForward) < m_uiMaxNumTroopsForward)
                fighter = m_rearFormationMapAldor.getRandomOccupant();
            else
                fighter = m_rearFormationMapScryer.getRandomOccupant();

            if (fighter.IsEmpty())
                return;

            ObjectGuid marker = GetAvailableMarkerFromMap(m_forwardFormationMap, fighter);

            if (!marker.IsEmpty())
                if (Creature* fighterCreature = m_creature->GetMap()->GetCreature(fighter))
                    if (!fighterCreature->IsInCombat())
                        if (Creature* markerCreature = m_creature->GetMap()->GetCreature(marker))
                        {
                            fighterCreature->GetMotionMaster()->Clear(false, true);
                            fighterCreature->SetWalk(false);
                            fighterCreature->GetMotionMaster()->MovePoint(0, markerCreature->GetPositionX(), markerCreature->GetPositionY(), markerCreature->GetPositionZ());

                            if (fighterCreature->GetEntry() == NPC_LIGHTSWORN_VINDICATOR)
                            {
                                ++m_uiNumLightswornForward;
                                m_rearFormationMapAldor.releaseFormationMarker(fighter);
                            }
                            else
                            {
                                ++m_uiNumMagisterForward;
                                m_rearFormationMapScryer.releaseFormationMarker(fighter);
                            }
                        }

            fighter.Clear();
            marker.Clear();
        }
    }

    void SendIllidariAttack()
    {
        IllidariAttackGroup attackGroup = ChooseIllidariAttack();
        ObjectGuid leaderGuid;
        uint32 waypoint = 0;

        switch (attackGroup)
        {
            case NONE:
                break;
            case RAVAGERS_1:
                leaderGuid = m_ravagerLeader1;
                m_ravagerLeader1.Clear();
                waypoint = 5;
                break;
            case RAVAGERS_2:
                leaderGuid = m_ravagerLeader2;
                m_ravagerLeader2.Clear();
                waypoint = 5;
                break;
            case RAVAGERS_3:
                leaderGuid = m_ravagerLeader3;
                m_ravagerLeader3.Clear();
                waypoint = 3;
                break;
            case RAVAGERS_4:
                leaderGuid = m_ravagerLeader4;
                m_ravagerLeader4.Clear();
                waypoint = 3;
                break;
            case SHADOWHOOF_1:
                leaderGuid = m_ShadowhoofLeader1;
                m_ShadowhoofLeader1.Clear();
                waypoint = 5;
                break;
            case SHADOWHOOF_2:
                leaderGuid = m_ShadowhoofLeader2;
                m_ShadowhoofLeader2.Clear();
                waypoint = 3;
                break;
            case SHADOWHOOF_3:
                leaderGuid = m_ShadowhoofLeader3;
                m_ShadowhoofLeader3.Clear();
                waypoint = 4;
                break;
        }

        if (Creature* leader = m_creature->GetMap()->GetCreature(leaderGuid))
        {
            leader->GetMotionMaster()->Clear(false, true);
            leader->GetMotionMaster()->MoveWaypoint(0, 1);
            leader->GetMotionMaster()->SetNextWaypoint(waypoint);
            leader->ForcedDespawn(600000);
            m_attackReadyMask -= attackGroup;

            if (mob_bt_battle_fighterAI* leaderAI = dynamic_cast<mob_bt_battle_fighterAI*>(leader->AI()))
                leaderAI->m_bIsWaypointing = true;
        }
    }

    // Determine how many waves are ready
    int countSetBits(uint8 n)
    {
        // base case
        if (n == 0)
            return 0;
        else
            // if last bit set add 1 else add 0
            return (n & 1) + countSetBits(n >> 1);
    }

    /* Most of the time send ravager group 3 or 4
    *  rest of the time send one of the others randomly */
    IllidariAttackGroup ChooseIllidariAttack()
    {
        if (countSetBits(m_attackReadyMask) < m_uiMinAttackGroupsReady)
            return NONE;
        else
        {
            uint8 rav3or4_mask = m_attackReadyMask & (RAVAGERS_3 | RAVAGERS_4);
            uint8 everythingElse_mask = m_attackReadyMask & ~(RAVAGERS_3 | RAVAGERS_4);

            if (rav3or4_mask && !everythingElse_mask)
            {
                return ChooseIllidariRavagers3or4(rav3or4_mask);
            }
            else if (!rav3or4_mask && everythingElse_mask)
            {
                return ChooseIllidariNotRavagers3or4(everythingElse_mask);
            }
            else
            {
                if (urand(0, 9) >= 3)
                    return ChooseIllidariRavagers3or4(rav3or4_mask);
                else
                    return ChooseIllidariNotRavagers3or4(everythingElse_mask);
            }
        }

        return NONE;
    }

    IllidariAttackGroup ChooseIllidariRavagers3or4(uint8 readyMask)
    {
        switch (readyMask)
        {
            case RAVAGERS_3 | RAVAGERS_4:
                if (urand(0, 1))
                    return RAVAGERS_3;
                else
                    return RAVAGERS_4;
            case RAVAGERS_3:
                return RAVAGERS_3;
            case RAVAGERS_4:
                return RAVAGERS_4;
            default:
                return NONE;
        }
    }

    IllidariAttackGroup ChooseIllidariNotRavagers3or4(uint8 readyMask)
    {
        if (urand(0, 5) <= 2)
        {
            readyMask = readyMask & (RAVAGERS_1 | RAVAGERS_2);
            switch (readyMask)
            {
                case RAVAGERS_1 | RAVAGERS_2:
                    if (urand(0, 1))
                        return RAVAGERS_1;
                    else
                        return RAVAGERS_2;
                case RAVAGERS_1:
                    return RAVAGERS_1;
                case RAVAGERS_2:
                    return RAVAGERS_2;
                default:
                    return NONE;
            }
        }
        else
        {
            readyMask = readyMask & (SHADOWHOOF_1 | SHADOWHOOF_2 | SHADOWHOOF_3);
            switch (readyMask)
            {
                case SHADOWHOOF_1 | SHADOWHOOF_2 | SHADOWHOOF_3:
                    if (!urand(0, 2))
                        return SHADOWHOOF_1;
                    else if (urand(0, 1))
                        return SHADOWHOOF_2;
                    else
                        return SHADOWHOOF_3;
                case SHADOWHOOF_1 | SHADOWHOOF_2:
                    return SHADOWHOOF_2;
                case SHADOWHOOF_1 | SHADOWHOOF_3:
                    return SHADOWHOOF_1;
                case SHADOWHOOF_2 | SHADOWHOOF_3:
                    return SHADOWHOOF_3;
                case SHADOWHOOF_1:
                    return SHADOWHOOF_1;
                case SHADOWHOOF_2:
                    return SHADOWHOOF_2;
                case SHADOWHOOF_3:
                    return SHADOWHOOF_3;
                default:
                    return NONE;
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_fIllidariAttackTimer < diff)
        {
            SendIllidariAttack();

            m_fIllidariAttackTimer = ILLIDARI_ATTACK_INTERVAL;
        }
        else
            m_fIllidariAttackTimer -= diff;

        if (m_fAldorScryerReinforceTimer < diff)
        {
            ReinforceAldorScryerFrontline();

            m_fAldorScryerReinforceTimer = REINFORCE_INTERVAL;
        }
        else
            m_fAldorScryerReinforceTimer -= diff;
    }
};

UnitAI* GetAI_npc_bt_battle_sensor(Creature* pCreature)
{
    return new npc_bt_battle_sensor(pCreature);
}

struct TagGreaterFelfireDiemetradon : public SpellScript, public AuraScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        if (!target || !target->IsCreature() || static_cast<Creature*>(target)->HasBeenHitBySpell(spell->m_spellInfo->Id))
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void OnHit(Spell* spell, SpellMissInfo /*missInfo*/) const override
    {
        Unit* target = spell->m_targets.getUnitTarget(); // no need to check for creature, done above
        if (target)
            static_cast<Creature*>(target)->RegisterHitBySpell(spell->m_spellInfo->Id);
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            static_cast<Creature*>(aura->GetTarget())->UnregisterHitBySpell(aura->GetSpellProto()->Id);
    }
};

struct DragonmawIllusionBase : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
        {
            aura->GetTarget()->CastSpell(nullptr, 40216, TRIGGERED_OLD_TRIGGERED);
            aura->GetTarget()->CastSpell(nullptr, 42016, TRIGGERED_OLD_TRIGGERED);
        }
        else
        {
            aura->GetTarget()->RemoveAurasDueToSpell(40216);
            aura->GetTarget()->RemoveAurasDueToSpell(42016);
        }
    }
};

struct DragonmawIllusionTransform : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->GetTarget()->OverrideMountDisplayId(16314);
        else
            aura->GetTarget()->OverrideMountDisplayId(0);
    }
};

struct CallingRider : public SpellScript
{
    void OnDestTarget(Spell* spell) const override
    {
        spell->m_targets.m_destPos.z += 25.f;
    }
};

void AddSC_shadowmoon_valley()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "mob_mature_netherwing_drake";
    pNewScript->GetAI = &GetAI_mob_mature_netherwing_drake;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_enslaved_netherwing_drake";
    pNewScript->GetAI = &GetAI_mob_enslaved_netherwing_drake;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dragonmaw_peon";
    pNewScript->GetAI = &GetNewAIInstance<npc_dragonmaw_peonAI>;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_dragonmaw_peon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wilda";
    pNewScript->GetAI = &GetAI_npc_wilda;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_wilda;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lord_illidan_stormrage";
    pNewScript->GetAI = &GetNewAIInstance<npc_lord_illidan_stormrageAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_torloth";
    pNewScript->GetAI = &GetNewAIInstance<mob_torlothAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_totem_of_spirits";
    pNewScript->GetAI = &GetNewAIInstance<npc_totem_of_spiritsAI>;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_totem_of_spirits;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shadowlord_deathwail";
    pNewScript->GetAI = &GetAI_npc_shadowlord_deathwail;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_shadowmoon_soulstealer";
    pNewScript->GetAI = &GetNewAIInstance<mob_shadowmoon_soulstealerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_soul_captured_credit";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_soul_captured_credit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_crystal_prison";
    pNewScript->pQuestAcceptGO = &GOQuestAccept_GO_crystal_prison;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spawned_oronok_tornheart";
    pNewScript->GetAI = &GetAI_npc_spawned_oronok_tornheart;
    pNewScript->pGossipHello =  &GossipHello_npc_spawned_oronok_tornheart;
    pNewScript->pGossipSelect = &GossipSelect_npc_spawned_oronok_tornheart;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_domesticated_felboar";
    pNewScript->GetAI = &GetAI_npc_domesticated_felboar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shadowmoon_tuber_node";
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_shadowmoon_tuber_node;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_veneratus_spawn_node";
    pNewScript->GetAI = &GetAI_npc_veneratus_spawn_node;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_disobedient_dragonmaw_peon";
    pNewScript->GetAI = &GetAI_npc_disobedient_dragonmaw_peon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_commander_hobb";
    pNewScript->GetAI = &GetNewAIInstance<npc_commander_hobbAI>;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_commander;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_commander_arcus";
    pNewScript->GetAI = &GetNewAIInstance<npc_commander_arcusAI>;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_commander;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dragonmaw_racer_muckjaw";
    pNewScript->GetAI = &GetNewAIInstance<npc_dragonmaw_racer_muckjawAI>;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_dragonmaw_racer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dragonmaw_racer_trope";
    pNewScript->GetAI = &GetNewAIInstance<npc_dragonmaw_racer_tropeAI>;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_dragonmaw_racer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dragonmaw_racer_corlok";
    pNewScript->GetAI = &GetNewAIInstance<npc_dragonmaw_racer_corlokAI>;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_dragonmaw_racer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dragonmaw_racer_ichman";
    pNewScript->GetAI = &GetNewAIInstance<npc_dragonmaw_racer_ichmanAI>;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_dragonmaw_racer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dragonmaw_racer_mulverick";
    pNewScript->GetAI = &GetNewAIInstance<npc_dragonmaw_racer_mulverickAI>;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_dragonmaw_racer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dragonmaw_racer_skyshatter";
    pNewScript->GetAI = &GetNewAIInstance<npc_dragonmaw_racer_skyshatterAI>;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_dragonmaw_racer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_bt_battle_fighter";
    pNewScript->GetAI = &GetAI_mob_bt_battle_fighter;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_bt_battle_sensor";
    pNewScript->GetAI = &GetAI_npc_bt_battle_sensor;
    pNewScript->RegisterSelf();

    RegisterSpellScript<DragonmawKnockdownTheAggroCheck>("spell_dragonmaw_knockdown_the_aggro_check");
    RegisterSpellScript<TagGreaterFelfireDiemetradon>("spell_tag_for_single_use");
    RegisterSpellScript<DragonmawIllusionBase>("spell_dragonmaw_illusion_base");
    RegisterSpellScript<DragonmawIllusionTransform>("spell_dragonmaw_illusion_transform");
    RegisterSpellScript<CallingRider>("spell_calling_rider");
}

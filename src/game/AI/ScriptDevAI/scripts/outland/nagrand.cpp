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
SDName: Nagrand
SD%Complete: 90
SDComment: Quest support: 9868, 9879, 9918, 10085, 10646, 11090.
SDCategory: Nagrand
EndScriptData */

/* ContentData
mob_lump
npc_nagrand_captive
npc_creditmarker_visit_with_ancestors
npc_rethhedron
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "AI/ScriptDevAI/scripts/outland/world_outland.h"
#include "Spells/Scripts/SpellScript.h"

/*######
## mob_lump
######*/

enum
{
    SAY_LUMP_AGGRO_1            = -1000190,
    SAY_LUMP_AGGRO_2            = -1000191,
    SAY_LUMP_DEFEAT             = -1000192,

    SPELL_VISUAL_SLEEP          = 16093,
    SPELL_SPEAR_THROW           = 32248,

    FACTION_FRIENDLY            = 35
};

struct mob_lumpAI : public ScriptedAI
{
    mob_lumpAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bReset = false;
        Reset();
    }

    uint32 m_uiResetTimer;
    uint32 m_uiSpearThrowTimer;
    bool m_bReset;

    void Reset() override
    {
        m_uiResetTimer = MINUTE * IN_MILLISECONDS;
        m_uiSpearThrowTimer = 2000;
    }

    void AttackedBy(Unit* pAttacker) override
    {
        if (m_creature->GetVictim())
            return;

        if (!m_creature->CanAttackNow(pAttacker))
            return;

        AttackStart(pAttacker);
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        if (m_creature->GetHealth() < damage || (m_creature->GetHealth() - damage) * 100 / m_creature->GetMaxHealth() < 30)
        {
            damage = std::min(damage, m_creature->GetHealth() - 1);

            m_creature->RemoveAllAuras();
            m_creature->CombatStop(true);

            // should get unit_flags UNIT_FLAG_IMMUNE_TO_PLAYER | UNIT_FLAG_IMMUNE_TO_NPC at faction change, but unclear why/for what reason, skipped (no flags expected as default)
            m_creature->SetFactionTemporary(FACTION_FRIENDLY, TEMPFACTION_RESTORE_REACH_HOME);

            m_creature->SetStandState(UNIT_STAND_STATE_SIT);
            DoScriptText(SAY_LUMP_DEFEAT, m_creature);

            m_bReset = true;
        }
    }

    void Aggro(Unit* pWho) override
    {
        if (m_creature->HasAura(SPELL_VISUAL_SLEEP, EFFECT_INDEX_0))
            m_creature->RemoveAurasDueToSpell(SPELL_VISUAL_SLEEP);

        if (!m_creature->IsStandState())
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

        DoScriptText(urand(0, 1) ? SAY_LUMP_AGGRO_1 : SAY_LUMP_AGGRO_2, m_creature, pWho);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Check if we waiting for a reset
        if (m_bReset)
        {
            if (m_uiResetTimer < uiDiff)
            {
                EnterEvadeMode();
                m_bReset = false;
            }
            else
                m_uiResetTimer -= uiDiff;
        }

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // SpearThrow Timer
        if (m_uiSpearThrowTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SPEAR_THROW);
            m_uiSpearThrowTimer = 20000;
        }
        else
            m_uiSpearThrowTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_mob_lump(Creature* pCreature)
{
    return new mob_lumpAI(pCreature);
}

/*######
## npc_nagrand_captive
#####*/

enum
{
    SAY_MAG_START               = -1000482,
    SAY_MAG_MORE                = -1000484,
    SAY_MAG_LIGHTNING           = -1000486,
    SAY_MAG_SHOCK               = -1000487,
    SAY_MAG_COMPLETE            = -1000488,

    SAY_KUR_START               = -1001001,
    SAY_KUR_AMBUSH_1            = -1001002,
    SAY_KUR_AMBUSH_2            = -1001003,
    SAY_KUR_UP_AHEAD            = -1001004,
    SAY_KUR_COMPLETE            = -1001005,

    SAY_MURK_BRUTE_NO_ESCAPE	= -1000483,
    SAY_MURK_BRUTE_WHERE		= -1000485,

    SPELL_CHAIN_LIGHTNING       = 16006,
    SPELL_EARTHBIND_TOTEM       = 15786,
    SPELL_FROST_SHOCK           = 12548,
    SPELL_HEALING_WAVE          = 12491,

    QUEST_TOTEM_KARDASH_H       = 9868,
    QUEST_TOTEM_KARDASH_A       = 9879,

    NPC_KURENAI_CAPTIVE         = 18209,
    NPC_MAGHAR_CAPTIVE          = 18210,

    NPC_MURK_RAIDER             = 18203,
    NPC_MURK_BRUTE              = 18211,
    NPC_MURK_SCAVENGER          = 18207,
    NPC_MURK_PUTRIFIER          = 18202,

    FACTION_TROLL_FROSTMANE		= 33,
    FACTION_UNKWN				= 10
};

struct npc_nagrand_captiveAI : public npc_escortAI
{
    npc_nagrand_captiveAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    uint32 m_uiChainLightningTimer;
    uint32 m_uiEarthbindTotemTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiFrostShockTimer;

    void Reset() override
    {
        m_uiChainLightningTimer = 1000;
        m_uiEarthbindTotemTimer = 0;
        m_uiHealTimer = 0;
        m_uiFrostShockTimer = 6000;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            if (m_creature->GetEntry() == NPC_MAGHAR_CAPTIVE)
            {
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                m_creature->SetFactionTemporary(FACTION_TROLL_FROSTMANE, TEMPFACTION_RESTORE_RESPAWN);

                if (Creature *summoned = m_creature->SummonCreature(NPC_MURK_BRUTE, -1574.611f, 8547.862f, 2.084420f, 2.32f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 10000))
                    DoScriptText(SAY_MURK_BRUTE_NO_ESCAPE, summoned);
                m_creature->SummonCreature(NPC_MURK_RAIDER, -1576.695f, 8546.507f, 2.0844190f, 1.902409f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 10000);

                DoScriptText(SAY_MAG_START, m_creature);
            }
            else if (m_creature->GetEntry() == NPC_KURENAI_CAPTIVE)
            {
                m_creature->SetFactionTemporary(FACTION_UNKWN, TEMPFACTION_RESTORE_RESPAWN);

                if (Creature *summoned = m_creature->SummonCreature(NPC_MURK_BRUTE, -1530.8f, 8455.717f, -4.019422f, 0.084f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 10000))
                    DoScriptText(SAY_MURK_BRUTE_NO_ESCAPE, summoned);
                m_creature->SummonCreature(NPC_MURK_RAIDER, -1514.549f, 8477.511f, -4.015618f, 4.43f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 10000);

                DoScriptText(SAY_KUR_AMBUSH_1, m_creature);
            }

            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));
        }
        else if (eventType == AI_EVENT_JUST_DIED) // Sent by brutes when they die to add the questgiver flag
        {
            if (!HasEscortState(STATE_ESCORT_ESCORTING))
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        }
        else if (eventType == AI_EVENT_CUSTOM_EVENTAI_A) // sent by brutes 5 minutes after they die to remove the questgiver flag if nobody took up the quest
        {
            if (!HasEscortState(STATE_ESCORT_ESCORTING) && m_creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
                m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        if (m_creature->GetEntry() == NPC_MAGHAR_CAPTIVE)
        {
            switch (uiPointId)
            {
                case 5:
                    DoScriptText(SAY_MAG_MORE, m_creature);
                    break;
                case 6:
                    if (Creature *summoned = m_creature->SummonCreature(NPC_MURK_BRUTE, -1496.662f, 8508.388f, 1.015174f, 2.56f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 10000))
                        DoScriptText(SAY_MURK_BRUTE_WHERE, summoned);
                    m_creature->SummonCreature(NPC_MURK_PUTRIFIER, -1494.623f, 8505.492f, 1.173438f, 2.63f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 10000);
                    m_creature->SummonCreature(NPC_MURK_SCAVENGER, -1497.349f, 8505.020f, 1.107700f, 2.56f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 10000);
                    break;
                case 11:
                    DoScriptText(SAY_MAG_COMPLETE, m_creature);

                    if (Player* pPlayer = GetPlayerForEscort())
                        pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_TOTEM_KARDASH_H, m_creature);

                    SetRun();
                    break;
            }
        }
        else if (m_creature->GetEntry() == NPC_KURENAI_CAPTIVE)
        {
            switch (uiPointId)
            {
                case 2:
                    DoScriptText(SAY_KUR_START, m_creature);
                    break;
                case 8:
                    DoScriptText(SAY_KUR_AMBUSH_2, m_creature);
                    break;
                case 9:
                    if (Creature *summoned = m_creature->SummonCreature(NPC_MURK_BRUTE, -1417.57f, 8516.55f, 8.593721f, 3.76f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 10000))
                        DoScriptText(SAY_MURK_BRUTE_WHERE, summoned);
                    m_creature->SummonCreature(NPC_MURK_PUTRIFIER, -1411.089f, 8507.651f, 8.976571f, 3.21f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 10000);
                    m_creature->SummonCreature(NPC_MURK_SCAVENGER, -1440.539f, 8490.212f, 6.207497f, 1.03f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 10000);
                    break;
                case 10:
                    m_creature->SetFacingTo(0.61f);
                    DoScriptText(SAY_KUR_UP_AHEAD, m_creature);					
                    break;
                case 13:
                    DoScriptText(SAY_KUR_COMPLETE, m_creature);

                    if (Player* pPlayer = GetPlayerForEscort())
                        pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_TOTEM_KARDASH_A, m_creature);

                    SetRun();
                    break;
            }
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->IsTotem())
            return;
        
        pSummoned->AI()->AttackStart(m_creature);
    }

    void JustRespawned() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        //m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);

        if (m_creature->GetEntry() == NPC_MAGHAR_CAPTIVE)
            m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
    }

    void SpellHitTarget(Unit* /*pTarget*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_CHAIN_LIGHTNING)
        {
            if (urand(0, 9))
                return;

            DoScriptText(SAY_MAG_LIGHTNING, m_creature);
        }
        else if (pSpell->Id == SAY_MAG_SHOCK)
        {
            if (urand(0, 5))
                return;

            DoScriptText(SAY_MAG_SHOCK, m_creature);
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiChainLightningTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CHAIN_LIGHTNING) == CAST_OK)
                m_uiChainLightningTimer = urand(7000, 14000);
        }
        else
            m_uiChainLightningTimer -= uiDiff;

        if (m_creature->GetHealthPercent() < 30.0f)
        {
            if (m_uiHealTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_HEALING_WAVE) == CAST_OK)
                    m_uiHealTimer = 5000;
            }
            else
                m_uiHealTimer -= uiDiff;
        }
        else if (m_creature->GetHealthPercent() < 85.0f)
        {
            if (m_uiEarthbindTotemTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_EARTHBIND_TOTEM) == CAST_OK)
                    m_uiEarthbindTotemTimer = urand(10000, 14000);
            }
            else
                m_uiEarthbindTotemTimer -= uiDiff;
        }

        if (m_uiFrostShockTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FROST_SHOCK) == CAST_OK)
                m_uiFrostShockTimer = urand(7500, 15000);
        }
        else
            m_uiFrostShockTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_nagrand_captive(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_TOTEM_KARDASH_H || pQuest->GetQuestId() == QUEST_TOTEM_KARDASH_A)
    {
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        //pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        pCreature->SetStandState(UNIT_STAND_STATE_STAND);

        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

UnitAI* GetAI_npc_nagrand_captive(Creature* pCreature)
{
    return new npc_nagrand_captiveAI(pCreature);
}

/*######
## npc_creditmarker_visist_with_ancestors (Quest 10085)
######*/

enum
{
    QUEST_VISIT_WITH_ANCESTORS  = 10085
};

struct npc_creditmarker_visit_with_ancestorsAI : public ScriptedAI
{
    npc_creditmarker_visit_with_ancestorsAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override {}

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 30.0f))
        {
            if (((Player*)pWho)->GetQuestStatus(QUEST_VISIT_WITH_ANCESTORS) == QUEST_STATUS_INCOMPLETE)
            {
                uint32 creditMarkerId = m_creature->GetEntry();
                if ((creditMarkerId >= 18840) && (creditMarkerId <= 18843))
                {
                    // 18840: Sunspring, 18841: Laughing, 18842: Garadar, 18843: Bleeding
                    if (!((Player*)pWho)->GetReqKillOrCastCurrentCount(QUEST_VISIT_WITH_ANCESTORS, creditMarkerId))
                        ((Player*)pWho)->KilledMonsterCredit(creditMarkerId, m_creature->GetObjectGuid());
                }
            }
        }
    }
};

UnitAI* GetAI_npc_creditmarker_visit_with_ancestors(Creature* pCreature)
{
    return new npc_creditmarker_visit_with_ancestorsAI(pCreature);
}

/*######
## npc_rethhedron
######*/

enum
{
    SAY_LOW_HP                      = -1000966,
    SAY_EVENT_END                   = -1000967,

    SPELL_CRIPPLE                   = 41281,
    SPELL_SHADOW_BOLT               = 41280,
    SPELL_ABYSSAL_TOSS              = 41283,                // Casts toss at summoned npc
    SPELL_ABYSSAL_IMPACT            = 41284,
    // SPELL_GROUND_AIR_PULSE       = 41270,                // spell purpose unk
    // SPELL_AGGRO_CHECK            = 41285,                // spell purpose unk
    // SPELL_AGGRO_BURST            = 41286,                // spell purpose unk

    SPELL_COSMETIC_LEGION_RING      = 41339,
    SPELL_QUEST_COMPLETE            = 41340,

    NPC_SPELLBINDER                 = 22342,
    NPC_RETHHEDRONS_TARGET          = 23416,                // Summoned through missing serveride 41288

    PATH_ID_OUTRO                   = 1,

    POINT_ID_PORTAL_YELL            = 1,
    POINT_ID_PORTAL_FRONT           = 2,
    POINT_ID_PORTAL_FINAL           = 3,

    SOUND_ID_ABYSSAL_TOSS           = 10667,                // Played after abyssal toss
};

struct npc_rethhedronAI : public ScriptedAI
{
    npc_rethhedronAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->GetCombatManager().SetLeashingDisable(true);
        Reset();
    }

    uint32 m_uiCrippleTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiAbyssalTossTimer;
    uint32 m_uiDelayTimer;

    bool m_bLowHpYell;
    bool m_bEventFinished;

    void Reset() override
    {
        m_uiCrippleTimer     = urand(5000, 9000);
        m_uiShadowBoltTimer  = urand(1000, 3000);
        m_uiAbyssalTossTimer = 0;

        m_attackDistance = 30.0f;

        m_bLowHpYell        = false;

        m_attackDistance = 60.0f;
    }

    void JustRespawned() override
    {
        SetReactState(REACT_AGGRESSIVE);
        SetCombatMovement(true);
        SetCombatScriptStatus(false);
        m_bEventFinished = false;
        m_meleeEnabled = true;
        ScriptedAI::JustRespawned();
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        damage = std::min(m_creature->GetHealth() - 1, damage);

        // go to epilog at 10% health
        if (!m_bEventFinished && m_creature->GetHealth() - damage < m_creature->GetMaxHealth() * 0.1f)
        {
            SetReactState(REACT_PASSIVE);
            SetCombatMovement(false);
            SetCombatScriptStatus(true);
            m_meleeEnabled = false;
            m_creature->MeleeAttackStop(m_creature->GetVictim());
            m_creature->SetTarget(nullptr);
            m_creature->InterruptNonMeleeSpells(false);
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveWaypoint(PATH_ID_OUTRO);
            m_bEventFinished = true;
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != WAYPOINT_MOTION_TYPE || m_creature->GetMotionMaster()->GetPathId() != PATH_ID_OUTRO)
            return;

        switch (uiPointId)
        {
            case POINT_ID_PORTAL_YELL:
                DoScriptText(SAY_EVENT_END, m_creature);
                break;
            case POINT_ID_PORTAL_FINAL:
                DoCastSpellIfCan(m_creature, SPELL_QUEST_COMPLETE, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_COSMETIC_LEGION_RING, CAST_TRIGGERED);
                m_creature->ForcedDespawn(2000);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_bEventFinished)
            return;

        if (m_uiCrippleTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CRIPPLE) == CAST_OK)
                m_uiCrippleTimer = urand(20000, 30000);
        }
        else
            m_uiCrippleTimer -= uiDiff;

        if (m_uiShadowBoltTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOW_BOLT) == CAST_OK)
                m_uiShadowBoltTimer = urand(3000, 5000);
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        if (m_uiAbyssalTossTimer < uiDiff)
        {
            if (Unit* victim = m_creature->GetVictim())
            {
                if (!m_creature->CanReachWithMeleeAttack(victim))
                {
                    Creature* rethedronTarget = m_creature->SummonCreature(NPC_RETHHEDRONS_TARGET, victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ(), 0, TEMPSPAWN_TIMED_OR_CORPSE_DESPAWN, 30000);;
                    if (DoCastSpellIfCan(rethedronTarget, SPELL_ABYSSAL_TOSS) == CAST_OK)
                    {
                        m_creature->PlayDistanceSound(SOUND_ID_ABYSSAL_TOSS);
                        m_uiAbyssalTossTimer = urand(500, 2000);
                    }
                }
            }
        }
        else
            m_uiAbyssalTossTimer -= uiDiff;

        if (!m_bLowHpYell && m_creature->GetHealthPercent() < 40.0f)
        {
            DoScriptText(SAY_LOW_HP, m_creature);
            m_bLowHpYell = true;
        }

        DoMeleeAttackIfReady();
    }
};

enum
{
    GOSSIP_IN_BATTLE = 7700,

    QUEST_RING_OF_BLOOD_FINAL_CHALLENGE = 9977,
};

bool QuestAccept_npc_gurthock(Player* /*player*/, Creature* creature, const Quest* quest)
{
    if (quest->GetQuestId() == QUEST_RING_OF_BLOOD_FINAL_CHALLENGE)
    {
        if (InstanceData* data = creature->GetInstanceData())
            data->SetData(TYPE_MOGOR, 1);
    }
    else
        creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    return true;
}

bool GossipHello_npc_gurthock(Player* player, Creature* creature)
{
    if (creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
        return false;
    player->PrepareGossipMenu(creature, GOSSIP_IN_BATTLE);
    player->SendPreparedGossip(creature);
    return true;
}

enum
{
    NPC_WILD_SPARROWHAWK = 22979,

    SPELL_CAPTURED_SPARROWHAWK = 39812,
};

struct SparrowhawkNet : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        if (spell->m_targets.getUnitTargetGuid().GetEntry() != NPC_WILD_SPARROWHAWK)
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_1 || !spell->GetUnitTarget())
            return;

        spell->GetUnitTarget()->CastSpell(spell->GetCaster(), SPELL_CAPTURED_SPARROWHAWK, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_nagrand()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "mob_lump";
    pNewScript->GetAI = &GetAI_mob_lump;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_nagrand_captive";
    pNewScript->GetAI = &GetAI_npc_nagrand_captive;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_nagrand_captive;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_creditmarker_visit_with_ancestors";
    pNewScript->GetAI = &GetAI_npc_creditmarker_visit_with_ancestors;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rethhedron";
    pNewScript->GetAI = &GetNewAIInstance<npc_rethhedronAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_gurthock";
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_gurthock;
    pNewScript->pGossipHello = &GossipHello_npc_gurthock;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SparrowhawkNet>("spell_sparrowhawk_net");
}

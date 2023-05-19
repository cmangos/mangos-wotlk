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
SDName: Grizzly_Hills
SD%Complete:
SDComment: Quest support: 11984, 12027, 12082, 12138, 12198
SDCategory: Grizzly Hills
EndScriptData */

/* ContentData
npc_depleted_war_golem
npc_harrison_jones
spell_eagle_eyes
spell_escape_from_silverbrook_credit_master
spell_tag_troll
spell_out_cold
spell_assemble_cage
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "AI/ScriptDevAI/base/pet_ai.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "Entities/Vehicle.h"

/*######
## npc_depleted_war_golem
######*/

enum
{
    SAY_GOLEM_CHARGE            = -1000626,
    SAY_GOLEM_COMPLETE          = -1000627,

    NPC_LIGHTNING_SENTRY        = 26407,

    SPELL_CHARGE_GOLEM          = 47799,
    SPELL_GOLEM_CHARGE_CREDIT   = 47797,
};

struct npc_depleted_war_golemAI : public ScriptedPetAI
{
    npc_depleted_war_golemAI(Creature* pCreature) : ScriptedPetAI(pCreature) { Reset(); }

    void Reset() override { }

    void OwnerKilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_UNIT && pVictim->GetEntry() == NPC_LIGHTNING_SENTRY)
        {
            // Is distance expected?
            if (m_creature->IsWithinDistInMap(pVictim, 10.0f))
                m_creature->CastSpell(m_creature, SPELL_CHARGE_GOLEM, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

UnitAI* GetAI_npc_depleted_war_golem(Creature* pCreature)
{
    return new npc_depleted_war_golemAI(pCreature);
}

bool EffectAuraDummy_npc_depleted_war_golem(const Aura* pAura, bool bApply)
{
    if (pAura->GetId() != SPELL_CHARGE_GOLEM)
        return true;

    Creature* pCreature = (Creature*)pAura->GetTarget();

    if (!pCreature)
        return true;

    if (pAura->GetEffIndex() == EFFECT_INDEX_0)
    {
        if (bApply)
        {
            DoScriptText(SAY_GOLEM_CHARGE, pCreature);
            pCreature->addUnitState(UNIT_STAT_STUNNED);
        }
        else
        {
            DoScriptText(SAY_GOLEM_COMPLETE, pCreature);
            pCreature->clearUnitState(UNIT_STAT_STUNNED);

            // targets master
            pCreature->CastSpell(pCreature, SPELL_GOLEM_CHARGE_CREDIT, TRIGGERED_OLD_TRIGGERED);
        }
    }

    return true;
}

/*######
## npc_harrison_jones
######*/

enum
{
    // yells
    SAY_HARRISON_ESCORT_START           = -1001053,
    SAY_HARRISON_CHAMBER_1              = -1001054,
    SAY_HARRISON_CHAMBER_2              = -1001055,
    SAY_HARRISON_CHAMBER_RELEASE        = -1001056,
    SAY_ADARRAH_THANK_YOU               = -1001057,
    SAY_HARRISON_CHAMBER_3              = -1001058,
    SAY_HARRISON_CHAMBER_4              = -1001059,
    SAY_HARRISON_CHAMBER_5              = -1001060,
    SAY_HARRISON_CHAMBER_6              = -1001061,
    SAY_HARRISON_CHAMBER_7              = -1001062,
    SAY_HARRISON_ESCORT_COMPELTE        = -1001063,

    // quest
    QUEST_ID_DUN_DA_DUN_TAH             = 12082,

    // npcs
    NPC_ADARRAH                         = 24405,
    NPC_TECAHUNA                        = 26865,
    NPC_MUMMY_EFFECT_BUNNY              = 26867,
    NPC_ANCIENT_DRAKKARI_KING           = 26871,

    // spells
    SPELL_BUNNY_IMMOLATION              = 48150,
    SPELL_GONG_EFFECT                   = 47730,
    SPELL_TECAHUNA_SPIRIT_BEAM          = 47601,
    SPELL_SUMMON_DRAKKARI_KING          = 47602,

    // objects
    GO_HARRISON_CAGE                    = 188465,
    GO_ADARRAH_CAGE                     = 188487,
    GO_FIRE_DOOR                        = 188480,
};

struct npc_harrison_jonesAI : public npc_escortAI
{
    npc_harrison_jonesAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_uiActivateMummiesTimer = 0;
        Reset();
    }

    ObjectGuid m_tecahunaGuid;
    ObjectGuid m_adarrahGuid;

    uint32 m_uiActivateMummiesTimer;

    GuidList m_lImmolationBunnyGuids;

    void Reset() override { }

    void JustDied(Unit* pKiller) override
    {
        DoCleanChamberRoom();

        npc_escortAI::JustDied(pKiller);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            DoScriptText(SAY_HARRISON_ESCORT_START, m_creature);
            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));

            if (GameObject* pCage = GetClosestGameObjectWithEntry(m_creature, GO_HARRISON_CAGE, 5.0f))
                pCage->Use(m_creature);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_TECAHUNA)
        {
            m_tecahunaGuid = pSummoned->GetObjectGuid();

            // sort the mummies based on the distance
            CreatureList lBunniesInRange;
            GetCreatureListWithEntryInGrid(lBunniesInRange, m_creature, NPC_MUMMY_EFFECT_BUNNY, 50.0f);

            lBunniesInRange.sort(ObjectDistanceOrder(pSummoned));

            for (CreatureList::const_iterator itr = lBunniesInRange.begin(); itr != lBunniesInRange.end(); ++itr)
                m_lImmolationBunnyGuids.push_back((*itr)->GetObjectGuid());
        }
        else if (pSummoned->GetEntry() == NPC_ANCIENT_DRAKKARI_KING)
            pSummoned->AI()->AttackStart(m_creature);
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_TECAHUNA)
        {
            SetEscortPaused(false);
            DoCleanChamberRoom();
        }
    }

    void DoCleanChamberRoom()
    {
        // open door
        if (GameObject* pDoor = GetClosestGameObjectWithEntry(m_creature, GO_FIRE_DOOR, 50.0f))
            pDoor->ResetDoorOrButton();

        // clear auras
        CreatureList lBunniesInRange;
        GetCreatureListWithEntryInGrid(lBunniesInRange, m_creature, NPC_MUMMY_EFFECT_BUNNY, 50.0f);

        for (CreatureList::const_iterator itr = lBunniesInRange.begin(); itr != lBunniesInRange.end(); ++itr)
            (*itr)->RemoveAurasDueToSpell(SPELL_BUNNY_IMMOLATION);

        m_uiActivateMummiesTimer = 0;
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 8:
                DoScriptText(SAY_HARRISON_CHAMBER_1, m_creature);
                break;
            case 9:
                DoScriptText(SAY_HARRISON_CHAMBER_2, m_creature);
                break;
            case 11:
                m_creature->HandleEmote(EMOTE_ONESHOT_USESTANDING);
                break;
            case 12:
                DoScriptText(SAY_HARRISON_CHAMBER_RELEASE, m_creature);
                if (GameObject* pCage = GetClosestGameObjectWithEntry(m_creature, GO_ADARRAH_CAGE, 5.0f))
                    pCage->Use(m_creature);
                break;
            case 13:
                if (Creature* pAdarrah = GetClosestCreatureWithEntry(m_creature, NPC_ADARRAH, 5.0f))
                {
                    DoScriptText(SAY_ADARRAH_THANK_YOU, pAdarrah);
                    m_adarrahGuid = pAdarrah->GetObjectGuid();
                }
                break;
            case 14:
                if (Creature* pAdarrah = m_creature->GetMap()->GetCreature(m_adarrahGuid))
                {
                    pAdarrah->SetWalk(false);
                    pAdarrah->GetMotionMaster()->MovePoint(0, 4878.416f, -4793.893f, 32.549f);
                    pAdarrah->ForcedDespawn(5000);
                }
                break;
            case 16:
                m_creature->SetFacingTo(0.2f);
                m_creature->HandleEmote(EMOTE_ONESHOT_KNEEL);
                break;
            case 17:
            {
                // set mummies in fire
                CreatureList lBunniesInRange;
                GetCreatureListWithEntryInGrid(lBunniesInRange, m_creature, NPC_MUMMY_EFFECT_BUNNY, 50.0f);

                for (CreatureList::const_iterator itr = lBunniesInRange.begin(); itr != lBunniesInRange.end(); ++itr)
                    (*itr)->CastSpell((*itr), SPELL_BUNNY_IMMOLATION, TRIGGERED_OLD_TRIGGERED);

                m_creature->SetFacingTo(5.0f);
                DoCastSpellIfCan(m_creature, SPELL_GONG_EFFECT);
                break;
            }
            case 18:
                DoScriptText(SAY_HARRISON_CHAMBER_3, m_creature);
                break;
            case 19:
                DoScriptText(SAY_HARRISON_CHAMBER_4, m_creature);
                break;
            case 22:
                // close door
                if (GameObject* pDoor = GetClosestGameObjectWithEntry(m_creature, GO_FIRE_DOOR, 10.0f))
                    pDoor->Use(m_creature);
                break;
            case 23:
                DoScriptText(SAY_HARRISON_CHAMBER_5, m_creature);
                SetRun();
                // summon snake
                m_creature->SummonCreature(NPC_TECAHUNA, 4907.077f, -4819.035f, 32.55f, 2.32f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;
            case 24:
                DoScriptText(SAY_HARRISON_CHAMBER_6, m_creature);
                break;
            case 25:
                DoScriptText(SAY_HARRISON_CHAMBER_7, m_creature);
                break;
            case 26:
                // attack snake
                if (Creature* pTecahuna = m_creature->GetMap()->GetCreature(m_tecahunaGuid))
                    AttackStart(pTecahuna);
                SetEscortPaused(true);
                m_uiActivateMummiesTimer = 10000;
                break;
            case 54:
                DoScriptText(SAY_HARRISON_ESCORT_COMPELTE, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ID_DUN_DA_DUN_TAH, m_creature);
                    m_creature->SetFacingToObject(pPlayer);
                }
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // special script for snake fight
        if (m_uiActivateMummiesTimer)
        {
            if (m_uiActivateMummiesTimer <= uiDiff)
            {
                if (Creature* pTecahuna = m_creature->GetMap()->GetCreature(m_tecahunaGuid))
                {
                    // activate 2 mummies at each turn
                    for (uint8 i = 0; i < 2; ++i)
                    {
                        if (Creature* pBunny = m_creature->GetMap()->GetCreature(m_lImmolationBunnyGuids.front()))
                        {
                            pTecahuna->CastSpell(pBunny, SPELL_TECAHUNA_SPIRIT_BEAM, TRIGGERED_OLD_TRIGGERED);
                            pBunny->CastSpell(pBunny, SPELL_SUMMON_DRAKKARI_KING, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
                            pBunny->RemoveAurasDueToSpell(SPELL_BUNNY_IMMOLATION);
                            m_lImmolationBunnyGuids.remove(m_lImmolationBunnyGuids.front());
                        }
                    }
                }

                // set timer based on the remaining mummies
                if (m_lImmolationBunnyGuids.empty())
                    m_uiActivateMummiesTimer = 0;
                else
                    m_uiActivateMummiesTimer = urand(5000, 10000);
            }
            else
                m_uiActivateMummiesTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_harrison_jones(Creature* pCreature)
{
    return new npc_harrison_jonesAI(pCreature);
}

bool QuestAccept_npc_harrison_jones(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_DUN_DA_DUN_TAH)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

/*######
## npc_emily
######*/

enum
{
    SAY_ESCORT_START                    = -1001173,
    SAY_FIRST_WOLF                      = -1001174,
    SAY_FIRST_WOLF_ATTACK               = -1001175,
    SAY_HELP_FLOPPY_1                   = -1001176,
    SAY_FIRST_WOLF_DEFEAT               = -1001177,
    SAY_SECOND_WOLF                     = -1001178,
    SAY_HELP_FLOPPY_2                   = -1001179,
    SAY_FLOPPY_ALMOST_DEAD              = -1001180,
    SAY_SECOND_WOLF_DEFEAT              = -1001181,
    SAY_RESUME_ESCORT                   = -1001182,
    SAY_ESCORT_COMPLETE                 = -1001183,

    SPELL_FLOPPY_BECOMES_LUNCH          = 47184,

    NPC_HUNGRY_WORG                     = 26586,
    NPC_RAVENOUS_WORG                   = 26590,
    NPC_MR_FLOPPY                       = 26589,

    QUEST_ID_MR_FLOPPY_ADVENTURE        = 12027,
};

struct npc_emilyAI : public npc_escortAI
{
    npc_emilyAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    ObjectGuid m_floppyGuid;

    void Reset() override { }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));

            if (Creature* pFloppy = GetClosestCreatureWithEntry(m_creature, NPC_MR_FLOPPY, 10.0f))
                m_floppyGuid = pFloppy->GetObjectGuid();
        }
        else if (eventType == AI_EVENT_JUST_DIED && pSender->GetEntry() == NPC_MR_FLOPPY)
        {
            npc_escortAI::JustDied(m_creature);
            m_creature->ForcedDespawn();
        }
        else if (eventType == AI_EVENT_CRITICAL_HEALTH && pSender->GetEntry() == NPC_MR_FLOPPY)
            DoScriptText(SAY_FLOPPY_ALMOST_DEAD, m_creature);
        else if (eventType == AI_EVENT_LOST_SOME_HEALTH && pSender->GetEntry() == NPC_MR_FLOPPY)
            DoScriptText(urand(0, 1) ? SAY_HELP_FLOPPY_1 : SAY_HELP_FLOPPY_2, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_RAVENOUS_WORG:
            case NPC_HUNGRY_WORG:
                if (Creature* pFloppy = m_creature->GetMap()->GetCreature(m_floppyGuid))
                {
                    float fX, fY, fZ;
                    pFloppy->GetContactPoint(pSummoned, fX, fY, fZ);
                    pSummoned->SetWalk(false);
                    pSummoned->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                }
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_RAVENOUS_WORG:
                DoScriptText(SAY_SECOND_WOLF_DEFEAT, m_creature);
                SetEscortPaused(false);
                // resume follow after vehicle unboard
                if (Creature* pFloppy = m_creature->GetMap()->GetCreature(m_floppyGuid))
                    pFloppy->GetMotionMaster()->MoveFollow(m_creature, pFloppy->GetDistance(m_creature), M_PI_F - pFloppy->GetAngle(m_creature));
                break;
            case NPC_HUNGRY_WORG:
                DoScriptText(SAY_FIRST_WOLF_DEFEAT, m_creature);
                SetEscortPaused(false);
                break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || !uiPointId)
            return;

        switch (pSummoned->GetEntry())
        {
            case NPC_RAVENOUS_WORG:
                // board the ravenous worg vehicle
                if (Creature* pFloppy = m_creature->GetMap()->GetCreature(m_floppyGuid))
                    pFloppy->CastSpell(pSummoned, SPELL_FLOPPY_BECOMES_LUNCH, TRIGGERED_OLD_TRIGGERED);
            // no break;
            case NPC_HUNGRY_WORG:
                if (Creature* pFloppy = m_creature->GetMap()->GetCreature(m_floppyGuid))
                    pSummoned->AI()->AttackStart(pFloppy);
                break;
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 1:
                DoScriptText(SAY_ESCORT_START, m_creature);
                break;
            case 11:
                DoScriptText(SAY_FIRST_WOLF, m_creature);
                m_creature->SummonCreature(NPC_HUNGRY_WORG, 4305.514f, -3799.008f, 237.034f, 2.20f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;
            case 12:
                SetEscortPaused(true);
                DoScriptText(SAY_FIRST_WOLF_ATTACK, m_creature);
                break;
            case 23:
                SetEscortPaused(true);
                DoScriptText(SAY_SECOND_WOLF, m_creature);
                m_creature->SummonCreature(NPC_RAVENOUS_WORG, 4339.643f, -3948.972f, 194.904f, 0.90f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;
            case 25:
                DoScriptText(SAY_RESUME_ESCORT, m_creature);
                SetRun();
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ID_MR_FLOPPY_ADVENTURE, m_creature);
                break;
            case 26:
                DoScriptText(SAY_ESCORT_COMPLETE, m_creature);
                break;
            case 28:
                if (Creature* pFloppy = m_creature->GetMap()->GetCreature(m_floppyGuid))
                    pFloppy->ForcedDespawn();
                SetEscortPaused(true);
                m_creature->ForcedDespawn(1000);
                break;
        }
    }
};

UnitAI* GetAI_npc_emily(Creature* pCreature)
{
    return new npc_emilyAI(pCreature);
}

bool QuestAccept_npc_emily(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_MR_FLOPPY_ADVENTURE)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

/*######
## spell_eagle_eyes - 49546
######*/

struct spell_eagle_eyes : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        // Eagle Eyes can be cast only on this target
        if (!target || target->GetEntry() != 26369)
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }
};

/*######
## spell_escape_from_silverbrook_credit_master - 50471
######*/

struct spell_escape_from_silverbrook_credit_master : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (Unit* passenger = caster->GetVehicleInfo()->GetPassenger(1))
            passenger->CastSpell(nullptr, 50473, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_tag_troll - 47031
######*/

enum
{
    SAY_BUDD_TAG_TROLL          = -1020009,

    SPELL_BUDDS_ATTENTION       = 47014,
    SPELL_BUDDS_SNEAK           = 47032,
    SPELL_TAG_TROLL             = 47031,
    SPELL_OUT_COLD              = 47035,

    // server side spells; implemented in AI script
    // SPELL_PERIODIC_TRIGGER   = 47019,            // probably used to handle the texts
    // SPELL_CANCEL_ATTENTION   = 47040,
    // SPELL_TROLL_CHECK        = 47036,
    // SPELL_FORCE_REACTION     = 47047,

    NPC_DRAKKARI_SHAMAN         = 26447,
    NPC_DRAKKARI_WARRIOR        = 26425,
};

struct spell_tag_troll : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return SPELL_FAILED_BAD_TARGETS;

        if (caster->IsInCombat())
            return SPELL_FAILED_AFFECTING_COMBAT;

        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsCreature())
            return;

        Creature* troll = GetClosestCreatureWithEntry(target, NPC_DRAKKARI_SHAMAN, 50.0f);
        if (!troll)
            troll = GetClosestCreatureWithEntry(target, NPC_DRAKKARI_WARRIOR, 50.0f);
        if (!troll)
            return;

        target->CastSpell(target, SPELL_BUDDS_SNEAK, TRIGGERED_OLD_TRIGGERED);

        if (Unit* owner = target->GetOwner())
            owner->RemoveAurasDueToSpell(SPELL_BUDDS_ATTENTION);

        Creature* budd = static_cast<Creature*>(target);
        budd->AI()->SetReactState(REACT_PASSIVE);
        budd->SetWalk(false);
        budd->GetMotionMaster()->MoveChase(troll, 0.f, 0.f, false, true, false);

        // inform troll about the tag
        budd->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_B, budd, troll);
    }
};

/*######
## spell_out_cold - 47035
######*/

struct spell_out_cold : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_1)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsCreature())
            return;

        DoScriptText(SAY_BUDD_TAG_TROLL, target);
        target->RemoveAurasDueToSpell(SPELL_BUDDS_SNEAK);

        // run away and despawn
        Creature* budd = static_cast<Creature*>(target);

        float fX, fY, fZ;
        budd->GetRandomPoint(budd->GetPositionX(), budd->GetPositionY(), budd->GetPositionZ(), 50.0f, fX, fY, fZ);
        budd->SetWalk(false);
        budd->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
        budd->ForcedDespawn(5000);
    }
};


/*######
## spell_assemble_cage - 47042
######*/

struct spell_assemble_cage : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        if (!target)
            return SPELL_FAILED_BAD_TARGETS;

        if (!target->HasAura(47035))
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsCreature())
            return;

        target->CastSpell(target, 47045, TRIGGERED_OLD_TRIGGERED);

        target->AI()->ClearCombatOnlyRoot();
        target->RemoveAllAurasOnEvade();
        target->CombatStopWithPets(true);
        target->SetImmuneToNPC(true);
        target->SetImmuneToPlayer(true);

        Creature* troll = static_cast<Creature*>(target);
        troll->ForcedDespawn(10000);
    }
};

// 52812 - Molten Fury
struct MoltenFuryFlamebringer : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->GetUnitTarget() || !spell->GetUnitTarget()->IsStunned())
            return;

        spell->SetDamage(spell->GetDamage() * 2);
    }
};

void AddSC_grizzly_hills()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_depleted_war_golem";
    pNewScript->GetAI = &GetAI_npc_depleted_war_golem;
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_npc_depleted_war_golem;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_harrison_jones";
    pNewScript->GetAI = &GetAI_npc_harrison_jones;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_harrison_jones;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_emily";
    pNewScript->GetAI = &GetAI_npc_emily;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_emily;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_eagle_eyes>("spell_eagle_eyes");
    RegisterSpellScript<spell_escape_from_silverbrook_credit_master>("spell_escape_from_silverbrook_credit_master");
    RegisterSpellScript<spell_tag_troll>("spell_tag_troll");
    RegisterSpellScript<spell_out_cold>("spell_out_cold");
    RegisterSpellScript<spell_assemble_cage>("spell_assemble_cage");
    RegisterSpellScript<MoltenFuryFlamebringer>("spell_molten_fury_flamebringer");
}

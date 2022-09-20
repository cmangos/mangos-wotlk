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
SDName: Zuldrak
SD%Complete: 100
SDComment: Quest support: 12652, 12934.
SDCategory: Zuldrak
EndScriptData */

/* ContentData
npc_gurgthock
npc_ghoul_feeding_bunny
npc_decaying_ghoul
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Entities/TemporarySpawn.h"
#include "Entities/Vehicle.h"
#include "AI/EventAI/CreatureEventAI.h"

/*######
## npc_gurgthock
######*/

enum
{
    QUEST_FROM_BEYOND = 12934,

    NPC_AZBARIN       = 30026,
    NPC_DUKE_SINGEN   = 30019,
    NPC_ERATHIUS      = 30025,
    NPC_GARGORAL      = 30024
};

static float m_afSpawnLocation[] = {5768.71f, -2969.29f, 273.816f};
static uint32 m_auiBosses[] = {NPC_AZBARIN, NPC_DUKE_SINGEN, NPC_ERATHIUS, NPC_GARGORAL};

struct npc_gurgthockAI : public ScriptedAI
{
    npc_gurgthockAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_playerGuid;

    void SetPlayer(Player* pPlayer)
    {
        m_playerGuid = pPlayer->GetObjectGuid();
    }

    void Reset() override
    {
        m_playerGuid.Clear();
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        uint32 uiEntry = pSummoned->GetEntry();
        for (unsigned int m_auiBosse : m_auiBosses)
        {
            if (uiEntry == m_auiBosse)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_FROM_BEYOND, m_creature);

                m_playerGuid.Clear();
                return;
            }
        }
    }
};

bool QuestAccept_npc_gurgthock(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_FROM_BEYOND)
    {
        pCreature->SummonCreature(m_auiBosses[urand(0, 3)], m_afSpawnLocation[0], m_afSpawnLocation[1], m_afSpawnLocation[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_CORPSE_DESPAWN, 600000);

        if (npc_gurgthockAI* pGurthockAI = dynamic_cast<npc_gurgthockAI*>(pCreature->AI()))
            pGurthockAI->SetPlayer(pPlayer);
    }
    return true;
}

UnitAI* GetAI_npc_gurgthock(Creature* pCreature)
{
    return new npc_gurgthockAI(pCreature);
}

/*######
## npc_ghoul_feeding_bunny
######*/

enum
{
    SPELL_ATTRACT_GHOUL                     = 52037,                // script target on npc 28565
    SPELL_GHOUL_KILL_CREDIT                 = 52030,
    // SPELL_GHOUL_KILL_CREDIT_EFFECT       = 52038,                // triggers 52039; purpose unk - same effect as 52030 but with different target

    NPC_DECAYING_GHOUL                      = 28565,
};

struct npc_ghoul_feeding_bunnyAI : public ScriptedAI
{
    npc_ghoul_feeding_bunnyAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiAttractTimer;

    void Reset() override
    {
        m_uiAttractTimer = 1000;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && pInvoker->GetEntry() == NPC_DECAYING_GHOUL)
        {
            // Give kill credit to the summoner player
            if (m_creature->IsTemporarySummon())
            {
                if (Player* pSummoner = m_creature->GetMap()->GetPlayer(m_creature->GetSpawnerGuid()))
                    DoCastSpellIfCan(pSummoner, SPELL_GHOUL_KILL_CREDIT, CAST_TRIGGERED);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiAttractTimer)
        {
            if (m_uiAttractTimer <= uiDiff)
            {
                // try to target a nearby ghoul
                if (DoCastSpellIfCan(m_creature, SPELL_ATTRACT_GHOUL) == CAST_OK)
                    m_uiAttractTimer = 0;
                else
                    m_uiAttractTimer = 5000;
            }
            else
                m_uiAttractTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_ghoul_feeding_bunny(Creature* pCreature)
{
    return new npc_ghoul_feeding_bunnyAI(pCreature);
}

/*######
## npc_decaying_ghoul
######*/

enum
{
    SPELL_BIRTH                             = 26047,
    SPELL_FLESH_ROT                         = 28913,

    NPC_GHOUL_FEEDING_BUNNY                 = 28591,
};

struct npc_decaying_ghoulAI : public ScriptedAI
{
    npc_decaying_ghoulAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bSpawnAnim = false;
        Reset();
    }

    uint32 m_uiFleshRotTimer;
    bool m_bSpawnAnim;
    ObjectGuid m_feedingBunnyGuid;

    void Reset() override
    {
        m_uiFleshRotTimer = urand(1000, 3000);
    }

    void JustRespawned() override
    {
        DoCastSpellIfCan(m_creature, SPELL_BIRTH);
        m_creature->HandleEmote(EMOTE_STATE_NONE);
        m_feedingBunnyGuid.Clear();
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // handle the animation and despawn
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->HandleEmote(EMOTE_STATE_EAT_NO_SHEATHE);
        m_creature->ForcedDespawn(10000);

        // send AI event for the quest credit
        if (Creature* pBunny = m_creature->GetMap()->GetCreature(m_feedingBunnyGuid))
            SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pBunny);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && pInvoker->GetEntry() == NPC_GHOUL_FEEDING_BUNNY)
        {
            // check if the ghoul has already a feeding bunny set
            if (m_feedingBunnyGuid)
                return;

            // move the ghoul to the feeding target
            float fX, fY, fZ;
            m_creature->SetWalk(false);
            m_creature->GetMotionMaster()->Clear();
            pInvoker->GetContactPoint(m_creature, fX, fY, fZ);

            m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
            m_feedingBunnyGuid = pInvoker->GetObjectGuid();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // cast birth animation
        if (!m_bSpawnAnim)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BIRTH) == CAST_OK)
                m_bSpawnAnim = true;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiFleshRotTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FLESH_ROT) == CAST_OK)
                m_uiFleshRotTimer = urand(7000, 15000);
        }
        else
            m_uiFleshRotTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_decaying_ghoul(Creature* pCreature)
{
    return new npc_decaying_ghoulAI(pCreature);
}

bool EffectDummyCreature_npc_decaying_ghoul(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_ATTRACT_GHOUL && uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->GetEntry() == NPC_DECAYING_GHOUL)
    {
        if (pCaster->GetEntry() != NPC_GHOUL_FEEDING_BUNNY)
            return true;

        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);
        return true;
    }

    return false;
}

// 55430 - Gymer's Buddy
struct GymersBuddy : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        // this is required because effect 1 happens before effect 0 and at time of visibility update gymer needs to be in second phase in order for control not to malfunction
        if (target)
            target->SetPhaseMask(256, false);
    }
};

// 55421 - Gymer's Throw
struct GymersThrow : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (VehicleInfo* vehicle = caster->GetVehicleInfo())
        {
            if (Unit* vargul = vehicle->GetPassenger(1)) // gets vargul held in hand
            {
                vargul->RemoveAurasDueToSpell(43671); // removes from vehicle
                caster->CastSpell(vargul, 55569, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);
            }
        }
    }
};

enum
{
    QUEST_DARK_HORIZON = 12664,
    QUEST_REUNITED      = 12663,
    NPC_OVERLORD_DRAKURU = 28717,
};

bool AreaTrigger_at_overlord_drakuru(Player* player, AreaTriggerEntry const* at)
{
    if (!player->IsActiveQuest(QUEST_DARK_HORIZON) && !player->IsActiveQuest(QUEST_REUNITED))
        return false;

    int32 textId = 0;
    uint32 entry = NPC_OVERLORD_DRAKURU;
    switch (at->id)
    {
        case 5056: textId = 28691; break;
        case 5057: textId = 28700; break;
        case 5058: textId = 28701; break;
        case 5059: textId = 28708; break;
        case 5060: textId = 28709; break;
        case 5095: textId = 29281; break;
        case 5096: textId = 29291; break;
        case 5097: textId = 29292; break;
        case 5098: textId = 29293; break;
    }

    Creature* overlord = GetClosestCreatureWithEntry(player, entry, 50.f);
    if (overlord && textId)
        DoBroadcastText(textId, overlord, player);

    return true;
}

// 55368 - Summon Stefan
struct SummonStefan : public SpellScript
{
    void OnDestTarget(Spell* spell) const override
    {
        spell->m_targets.m_destPos.z += 20.f;
    }
};

// 52244 - Charm Geist
struct CharmGeist : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_0 || !apply)
            return;

        static_cast<Creature*>(aura->GetTarget())->SetSpellList(2875000);
    }
};

// 52245 - Harvest Blight Crystal
struct HarvestBlightCrystal : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_0)
        {
            spell->GetCaster()->CastSpell(nullptr, 52247, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

// 52247 - Target Crystal
struct TargetCrystal : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        if (spell->GetCaster()->AI())
            spell->GetCaster()->AI()->SetFollowMovement(false);
    }
};

// 52390 - Charm Drakuru Servant
struct CharmDrakuruServant : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!aura->GetTarget()->IsCreature())
            return;

        if (apply)
        {
            static_cast<Creature*>(aura->GetTarget())->UpdateEntry(28805, nullptr, nullptr, false, false);
            DoBroadcastText(28884, aura->GetTarget(), aura->GetCaster());
        }
    }
};

// 52590 - Kill credit
struct KillCreditDrakkariSkullcrusher : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        if (!spell->GetCaster()->IsPlayer())
            return;

        Player* player = static_cast<Player*>(spell->GetCaster());
        spell->SetScriptValue(player->GetReqKillOrCastCurrentCount(12690, 29099));
    }

    void OnSuccessfulFinish(Spell* spell) const override
    {
        if (!spell->GetCaster()->IsPlayer())
            return;

        Player* player = static_cast<Player*>(spell->GetCaster());
        uint32 count = player->GetReqKillOrCastCurrentCount(12690, 29099);
        if (count == spell->GetScriptValue()) // nothing changed, prevent spam at 60
            return;

        if (count % 20 == 0) // at 20, 40, 60 - spawn chieftain
            spell->GetCaster()->CastSpell(nullptr, 52616, TRIGGERED_OLD_TRIGGERED);
    }
};

// 52089 - Drakuramas Teleport Script 01
struct DrakuramasTeleportScript01 : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!aura->GetTarget()->IsPlayer())
            return;

        if (apply)
        {
            aura->GetTarget()->CastSpell(nullptr, 52091, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

// 52239 - Drakuramas Teleport Script 02
struct DrakuramasTeleportScript02 : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!aura->GetTarget()->IsPlayer())
            return;

        if (apply)
        {
            aura->GetTarget()->CastSpell(nullptr, 52240, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

// 52676 - Drakuramas Teleport Script 03
struct DrakuramasTeleportScript03 : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!aura->GetTarget()->IsPlayer())
            return;

        if (apply)
        {
            aura->GetTarget()->CastSpell(nullptr, 52677, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

// 54089 - Drop Disguise
struct DropDisguise : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!aura->GetTarget()->IsPlayer())
            return;

        if (apply)
        {
            aura->GetTarget()->CastSpell(nullptr, 52010, TRIGGERED_OLD_TRIGGERED);
            DoBroadcastText(28552, aura->GetTarget(), aura->GetTarget());
        }
    }
};

// 52010 - Scourge Disguise Expiring
struct ScourgeDisguiseExpiring : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!aura->GetTarget()->IsPlayer())
            return;

        if (!apply && aura->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
        {
            aura->GetTarget()->RemoveAurasDueToSpell(51966);
            DoBroadcastText(29886, aura->GetTarget(), aura->GetTarget());
        }
    }
};

// 41928 - Argent Stand Unit: Ride Gargoyle
struct ArgentStandUnitRideGargoyle : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
        {
            if (Unit* caster = aura->GetCaster())
            {
                if (caster->HasAura(53031)) // Crusader Parachute
                    caster->CastSpell(nullptr, 53039, TRIGGERED_OLD_TRIGGERED);
                caster->GetMotionMaster()->MoveFall();
            }
        }
    }
};

// 53031 - Crusader Parachute
struct CrusaderParachute : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        if (spell->m_targets.getUnitTargetGuid().GetEntry() != 28028 && spell->m_targets.getUnitTargetGuid().GetEntry() != 28029)
            return SPELL_FAILED_BAD_IMPLICIT_TARGETS;
        return SPELL_CAST_OK;
    }
};

// 53039 - Deploy Parachute
struct DeployParachute : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        // Crusader Parachute
        spell->GetCaster()->RemoveAurasDueToSpell(53031);
    }
};

struct ArgentStandUnit : public CreatureEventAI
{
    ArgentStandUnit(Creature* creature) : CreatureEventAI(creature) {}

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType == EFFECT_MOTION_TYPE && pointId == EVENT_FALL)
        {
            if (!m_creature->HasAura(53039))
                m_creature->FallSuicide();
        }
    }
};

// 51060 - Have Withered Batwing
struct HaveWitheredBatwing : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!aura->GetTarget()->IsPlayer())
            return;

        if (apply)
        {
            aura->GetCaster()->CastSpell(aura->GetTarget(), 51226, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

// 51068 - Have Muddy Mire Maggot
struct HaveMuddyMireMaggot : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!aura->GetTarget()->IsPlayer())
            return;

        if (apply)
        {
            aura->GetTarget()->CastSpell(aura->GetTarget(), 51227, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

// 51088 - Have Amberseed
struct HaveAmberseed : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!aura->GetTarget()->IsPlayer())
            return;

        if (apply)
        {
            aura->GetCaster()->CastSpell(aura->GetTarget(), 51228, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

// 51094 - Have Chilled Serpent Mucus
struct HaveChilledSerpentMucus : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!aura->GetTarget()->IsPlayer())
            return;

        if (apply)
        {
            aura->GetCaster()->CastSpell(aura->GetTarget(), 51229, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

// 50664 - Summon Crusader LeCraft
struct SummonCrusaderLeCraft : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->GetMotionMaster()->MoveFollow(spell->GetCaster(), PET_FOLLOW_DIST, PET_FOLLOW_ANGLE, true);
    }
};

// 50678 - Summon Crusader Lamoof
struct SummonCrusaderLamoof : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->GetMotionMaster()->MoveFollow(spell->GetCaster(), PET_FOLLOW_DIST, PET_FOLLOW_ANGLE, true);
    }
};

// 50692 - Summon Crusader Josephine
struct SummonCrusaderJosephine : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->GetMotionMaster()->MoveFollow(spell->GetCaster(), PET_FOLLOW_DIST, PET_FOLLOW_ANGLE, true);
    }
};

// 54496 Summon Plague Spray
struct SummonPlagueSpray : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_A, summon, spell->GetCaster());
    }
};

// 51173 - A Tangled Skein: Encasing Webs - Effect
struct ATangledSkeinEncasingWebsEffect : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_0)
        {
            spell->GetUnitTarget()->CastSpell(nullptr, 51168, TRIGGERED_OLD_TRIGGERED);
            spell->GetUnitTarget()->GetMotionMaster()->MoveFall();
        }
    }
};

void AddSC_zuldrak()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_gurgthock";
    pNewScript->GetAI = &GetAI_npc_gurgthock;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_gurgthock;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ghoul_feeding_bunny";
    pNewScript->GetAI = &GetAI_npc_ghoul_feeding_bunny;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_decaying_ghoul";
    pNewScript->GetAI = &GetAI_npc_decaying_ghoul;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_decaying_ghoul;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_overlord_drakuru";
    pNewScript->pAreaTrigger = &AreaTrigger_at_overlord_drakuru;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_argent_stand_unit";
    pNewScript->GetAI = &GetNewAIInstance<ArgentStandUnit>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<GymersBuddy>("spell_gymers_buddy");
    RegisterSpellScript<GymersThrow>("spell_gymers_throw");
    RegisterSpellScript<SummonStefan>("spell_summon_stefan");
    RegisterSpellScript<CharmGeist>("spell_charm_geist");
    RegisterSpellScript<HarvestBlightCrystal>("spell_harvest_blight_crystal");
    RegisterSpellScript<TargetCrystal>("spell_target_crystal");
    RegisterSpellScript<CharmDrakuruServant>("spell_charm_drakuru_servant");
    RegisterSpellScript<KillCreditDrakkariSkullcrusher>("spell_kill_credit_drakkari_skullcrusher");
    RegisterSpellScript<DrakuramasTeleportScript01>("spell_drakuramas_teleport_script_01");
    RegisterSpellScript<DrakuramasTeleportScript02>("spell_drakuramas_teleport_script_02");
    RegisterSpellScript<DrakuramasTeleportScript03>("spell_drakuramas_teleport_script_03");
    RegisterSpellScript<DropDisguise>("spell_drop_disguise");
    RegisterSpellScript<ScourgeDisguiseExpiring>("spell_scourge_disguise_expiring");
    RegisterSpellScript<ArgentStandUnitRideGargoyle>("spell_argent_stand_unit_ride_gargoyle");
    RegisterSpellScript<CrusaderParachute>("spell_crusader_parachute");
    RegisterSpellScript<DeployParachute>("spell_deploy_parachute");
    RegisterSpellScript<HaveWitheredBatwing>("spell_have_withered_batwing");
    RegisterSpellScript<HaveMuddyMireMaggot>("spell_have_muddy_mire_maggot");
    RegisterSpellScript<HaveAmberseed>("spell_have_amberseed");
    RegisterSpellScript<HaveChilledSerpentMucus>("spell_have_chilled_serpent_mucus");
    RegisterSpellScript<SummonCrusaderLeCraft>("spell_summon_crusader_lecraft");
    RegisterSpellScript<SummonCrusaderLamoof>("spell_summon_crusader_lamoof");
    RegisterSpellScript<SummonCrusaderJosephine>("spell_summon_crusader_josephine");
    RegisterSpellScript<SummonPlagueSpray>("spell_summon_plague_spray");
    RegisterSpellScript<ATangledSkeinEncasingWebsEffect>("spell_a_tangled_skein_encasing_webs_effect");
}

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
npc_ghoul_feeding_bunny
npc_decaying_ghoul
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Entities/TemporarySpawn.h"
#include "Entities/Vehicle.h"
#include "AI/EventAI/CreatureEventAI.h"

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

std::vector<uint32> easyAuras = { 51018, 51055, 51057, 51059, 51062, 51064, 51067, 51069, 51072, 51077, 51079 };
std::vector<uint32> mediumAuras = { 51081, 51083, 51085, 51087, 51091 };
std::vector<uint32> hardAuras = { 51093, 51095, 51097, 51100, 51102, 53150, 53153, 53158 };

enum AlchemistsApprentice
{
    QUEST_TROLL_PATROL_ALCHEMISTS_APPRENTICE = 12541,
    SPELL_KILL_CREDIT_ALCHEMISTS_APPRENTICE  = 51111,
};

// 51015 - Random Ingredient Easy Aura
// 51154 - Random Ingredient Medium Aura
// 51157 - Random Ingredient Hard Aura
struct RandomIngredientAura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* caster = aura->GetCaster();
        Unit* target = aura->GetTarget();

        uint32 spellId = 0;
        switch (aura->GetId())
        {
            case 51015: // easy
                spellId = easyAuras[urand(0, easyAuras.size() - 1)];
                break;
            case 51154: // medium
                spellId = mediumAuras[urand(0, mediumAuras.size() - 1)];
                break;
            case 51157: // hard
                spellId = hardAuras[urand(0, hardAuras.size() - 1)];
                break;
        }

        SpellCastArgs args;
        args.SetTarget(target).SetScriptValue(aura->GetScriptValue());

        caster->CastSpell(args, spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

// 51046 - Pot Check
struct PotCheck : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target->IsPlayer())
            return;

        Player* player = static_cast<Player*>(target);

        if (!player->IsCurrentQuest(QUEST_TROLL_PATROL_ALCHEMISTS_APPRENTICE, 1))
            return;

        uint32 foundAuraId = 0;
        uint32 stage = 0;
        Unit* alchemist = nullptr;
        auto helper = [&](std::vector<uint32>& auras)
        {
            for (uint32 auraId : auras)
            {
                if (Aura* aura = target->GetAura(auraId, EFFECT_INDEX_0))
                {
                    alchemist = aura->GetCaster();
                    stage = aura->GetScriptValue();
                    foundAuraId = auraId;
                    break;
                }
            }
        };

        helper(easyAuras);
        if (!foundAuraId)
            helper(mediumAuras);
        if (!foundAuraId)
            helper(hardAuras);

        if (!foundAuraId)
            return;

        uint32 itemId = 0;
        switch (foundAuraId)
        {
            case 51018: itemId = 38338; break; // Knotroot
            case 51055: itemId = 38341; break; // Pickled Eagle Egg
            case 51057: itemId = 38337; break; // Speckled Guano
            case 51059: itemId = 38339; break; // Withered Batwing
            case 51062: itemId = 38381; break; // Seasoned Slider Cider
            case 51064: itemId = 38384; break; // Pulverized Gargoyle Teeth
            case 51067: itemId = 38386; break; // Muddy Mire Maggot
            case 51069: itemId = 38393; break; // Spiky Spider Egg
            case 51072: itemId = 38396; break; // Hairy Herring Head
            case 51077: itemId = 38397; break; // Putrid Pirate Perspiration
            case 51079: itemId = 38398; break; // Icecrown Bottled Water
            case 51081: itemId = 38369; break; // Wasp's Wings
            case 51083: itemId = 38343; break; // Prismatic Mojo
            case 51085: itemId = 38370; break; // Raptor Claw
            case 51087: itemId = 38340; break; // Amberseed
            case 51091: itemId = 38344; break; // Shrunken Dragon's Claw
            case 51093: itemId = 38346; break; // Chilled Serpent Mucus
            case 51095: itemId = 38336; break; // Crystallized Hogsnot
            case 51097: itemId = 38379; break; // Crushed Basilisk Crystals
            case 51100: itemId = 38342; break; // Trollbane
            case 51102: itemId = 38345; break; // Frozen Spider Ichor
            case 53150: itemId = 39668; break; // Abomination Guts
            case 53153: itemId = 39669; break; // Ghoul Drool
            case 53158: itemId = 39670; break; // Blight Crystal
        }

        if (!player->HasItemCount(itemId, 1))
        {
            // fail quest
            switch (urand(0, 2))
            {
                case 0: DoBroadcastText(27686, alchemist, target); break;
                case 1: DoBroadcastText(27687, alchemist, target); break;
                case 2: DoBroadcastText(27688, alchemist, target); break;
            }
            player->FailQuest(QUEST_TROLL_PATROL_ALCHEMISTS_APPRENTICE);
            return;
        }

        player->RemoveAurasDueToSpell(foundAuraId);
        player->DestroyItemCount(itemId, 1, true);

        if (stage == 5) // finished quest
        {
            alchemist->CastSpell(player, SPELL_KILL_CREDIT_ALCHEMISTS_APPRENTICE, TRIGGERED_OLD_TRIGGERED);
            return;
        }

        switch (urand(0, 17))
        {
            case 0: DoBroadcastText(27630, alchemist, target); break;
            case 1: DoBroadcastText(27631, alchemist, target); break;
            case 2: DoBroadcastText(27632, alchemist, target); break;
            case 3: DoBroadcastText(27633, alchemist, target); break;
            case 4: DoBroadcastText(27634, alchemist, target); break;
            case 5: DoBroadcastText(27635, alchemist, target); break;
            case 6: DoBroadcastText(27636, alchemist, target); break;
            case 7: DoBroadcastText(27637, alchemist, target); break;
            case 8: DoBroadcastText(27677, alchemist, target); break;
            case 9: DoBroadcastText(27678, alchemist, target); break;
            case 10: DoBroadcastText(27679, alchemist, target); break;
            case 11: DoBroadcastText(27680, alchemist, target); break;
            case 12: DoBroadcastText(27681, alchemist, target); break;
            case 13: DoBroadcastText(27682, alchemist, target); break;
            case 14: DoBroadcastText(27683, alchemist, target); break;
            case 15: DoBroadcastText(27684, alchemist, target); break;
            case 16: DoBroadcastText(27685, alchemist, target); break;
            case 17: DoBroadcastText(27690, alchemist, target); break;
        }

        uint32 spellId = 0;
        switch (urand(0, 2))
        {
            case 0: spellId = 51015; break;
            case 1: spellId = 51154; break;
            case 2: spellId = 51157; break;
        }

        SpellCastArgs args;
        args.SetTarget(player).SetScriptValue(stage + 1);

        alchemist->CastSpell(args, spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

// 51018 - Fetch ...
struct FetchAlchemistsApprentice : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            DoBroadcastText(aura->GetAmount(), aura->GetCaster(), aura->GetTarget());
        if (!apply && aura->GetRemoveMode() != AURA_REMOVE_BY_DEFAULT)
        {
            // fail quest due to expiration
            DoScriptText(27688, aura->GetCaster(), aura->GetTarget());
            if (aura->GetTarget()->IsPlayer())
                static_cast<Player*>(aura->GetTarget())->FailQuest(QUEST_TROLL_PATROL_ALCHEMISTS_APPRENTICE);
            return;
        }
    }
};

// 51511 - Toss Ice Boulder
struct TossIceBoulder : public SpellScript
{
    void OnRadiusCalculate(Spell* spell, SpellEffectIndex effIdx, bool targetB, float& radius) const override
    {
        if (effIdx == EFFECT_INDEX_0 && !targetB) // radius index supplied is for targetB not targetA
            radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(spell->m_spellInfo->rangeIndex));
    }
};

// 51671 - Ghostly
struct Ghostly : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        spell->GetCaster()->CastSpell(nullptr, 51717, TRIGGERED_OLD_TRIGGERED);
    }
};

// 55882 - Orb of Storms
// 55872 - Orb of Flame
// 55886 - Boulder
// 55888 - Orb of Water
struct RandomRadiusPicker : public SpellScript
{
    void OnRadiusCalculate(Spell* spell, SpellEffectIndex effIdx, bool targetB, float& radius) const override
    {
        if (effIdx == EFFECT_INDEX_0)
        {
            if (targetB)
            {
                SpellRangeEntry const* entry = sSpellRangeStore.LookupEntry(spell->m_spellInfo->rangeIndex);
                radius = frand(GetSpellMinRange(entry), GetSpellMaxRange(entry));
            }
            else
                radius = 100.f; // guesswork
        }
    }
};

// 62108 - Tails Up : Summon Female Frost Leopard 
struct TailsUpSummonFemaleFrostLeopard : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        if (summon->AI())
            summon->AI()->RequestFollow(spell->GetCaster());
    }
};

// 62116 - Tails Up: Summon Female Icepaw Bear
struct TailsUpSummonFemaleIcepawBear : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        if (summon->AI())
            summon->AI()->RequestFollow(spell->GetCaster());
    }
};

void AddSC_zuldrak()
{
    Script* pNewScript = new Script;
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
    RegisterSpellScript<RandomIngredientAura>("spell_random_ingredient_aura");
    RegisterSpellScript<PotCheck>("spell_pot_check");
    RegisterSpellScript<FetchAlchemistsApprentice>("spell_fetch_alchemists_apprentice");
    RegisterSpellScript<TossIceBoulder>("spell_toss_ice_boulder");
    RegisterSpellScript<Ghostly>("spell_ghostly");
    RegisterSpellScript<RandomRadiusPicker>("spell_random_radius_picker");
    RegisterSpellScript<TailsUpSummonFemaleFrostLeopard>("spell_tails_up_summon_female_frost_leopard");
    RegisterSpellScript<TailsUpSummonFemaleIcepawBear>("spell_tails_up_summon_female_icepaw_bear");
}

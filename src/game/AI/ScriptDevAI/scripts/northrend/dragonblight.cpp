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
SDName: Dragonblight
SD%Complete: 100
SDComment: Quest support: 11983, 12075, 12166, 12261.
SDCategory: Dragonblight
EndScriptData */

/* ContentData
npc_destructive_ward
npc_crystalline_ice_giant
spell_Taunka_Face_Me
spell_container_of_rats
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

/*######
# npc_destructive_ward
#####*/

enum
{
    SAY_WARD_POWERUP                    = -1000664,
    SAY_WARD_CHARGED                    = -1000665,

    SPELL_DESTRUCTIVE_PULSE             = 48733,
    SPELL_DESTRUCTIVE_BARRAGE           = 48734,
    SPELL_DESTRUCTIVE_WARD_POWERUP      = 48735,

    SPELL_SUMMON_SMOLDERING_SKELETON    = 48715,
    SPELL_SUMMON_SMOLDERING_CONSTRUCT   = 48718,
    SPELL_DESTRUCTIVE_WARD_KILL_CREDIT  = 52409,

    MAX_STACK                           = 1,
};

// Script is based on real event from you-know-where.
// Some sources show the event in a bit different way, for unknown reason.
// Devs decided to add it in the below way, until more details can be obtained.

// It will be only two power-up's, where other sources has a different count (2-4 stacks has been observed)
// Probably caused by either a change in a patch (bugfix?) or the powerup has a condition (some
// sources suggest this, but without any explanation about what this might be)

struct npc_destructive_wardAI : public Scripted_NoMovementAI
{
    npc_destructive_wardAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        SetReactState(REACT_PASSIVE);
        m_uiPowerTimer = 30000;
        m_uiStack = 0;
        m_uiSummonTimer = 2000;
        m_bCanPulse = false;
        m_bFirst = true;
        Reset();
    }

    uint32 m_uiPowerTimer;
    uint32 m_uiStack;
    uint32 m_uiSummonTimer;
    bool m_bFirst;
    bool m_bCanPulse;

    void Reset() override { }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->AI()->AttackStart(m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_bCanPulse)
        {
            if (DoCastSpellIfCan(m_creature, m_uiStack > MAX_STACK ? SPELL_DESTRUCTIVE_BARRAGE : SPELL_DESTRUCTIVE_PULSE) == CAST_OK)
                m_bCanPulse = false;
        }

        if (m_uiSummonTimer)
        {
            if (m_uiSummonTimer <= uiDiff)
            {
                if (m_bFirst)
                    m_uiSummonTimer = 25000;
                else
                    m_uiSummonTimer = 0;

                switch (m_uiStack)
                {
                    case 0:
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOLDERING_SKELETON, CAST_TRIGGERED);
                        break;
                    case 1:
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOLDERING_CONSTRUCT, CAST_TRIGGERED);

                        if (m_bFirst)
                            break;

                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOLDERING_CONSTRUCT, CAST_TRIGGERED);
                        break;
                    case 2:
                        if (m_bFirst)
                            break;

                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOLDERING_SKELETON, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOLDERING_SKELETON, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SMOLDERING_CONSTRUCT, CAST_TRIGGERED);
                        break;
                }

                m_bFirst = !m_bFirst;
            }
            else
                m_uiSummonTimer -= uiDiff;
        }

        if (!m_uiPowerTimer)
            return;

        if (m_uiPowerTimer <= uiDiff)
        {
            if (m_uiStack > MAX_STACK)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DESTRUCTIVE_WARD_KILL_CREDIT) == CAST_OK)
                {
                    DoScriptText(SAY_WARD_CHARGED, m_creature, m_creature->GetOwner());
                    m_uiPowerTimer = 0;
                    m_uiSummonTimer = 0;
                    m_bCanPulse = true;
                }
            }
            else if (DoCastSpellIfCan(m_creature, SPELL_DESTRUCTIVE_WARD_POWERUP) == CAST_OK)
            {
                DoScriptText(SAY_WARD_POWERUP, m_creature, m_creature->GetOwner());

                m_uiPowerTimer = 30000;
                m_uiSummonTimer = 2000;

                m_bFirst = true;
                m_bCanPulse = true;                         // pulse right after each charge

                ++m_uiStack;
            }
        }
        else
            m_uiPowerTimer -= uiDiff;
    }
};

UnitAI* GetAI_npc_destructive_ward(Creature* pCreature)
{
    return new npc_destructive_wardAI(pCreature);
}

/*######
## npc_crystalline_ice_giant
######*/

enum
{
    SPELL_FEIGN_DEATH_PERMANENT     = 31261,
    ITEM_ID_SAMPLE_ROCKFLESH        = 36765,
    NPC_CRYSTALLINE_GIANT           = 26809,
};

bool NpcSpellClick_npc_crystalline_ice_giant(Player* pPlayer, Creature* pClickedCreature, uint32 /*uiSpellId*/)
{
    if (pClickedCreature->GetEntry() == NPC_CRYSTALLINE_GIANT && pClickedCreature->HasAura(SPELL_FEIGN_DEATH_PERMANENT))
    {
        if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(ITEM_ID_SAMPLE_ROCKFLESH, 1))
        {
            pPlayer->SendNewItem(pItem, 1, true, false);
            pClickedCreature->ForcedDespawn();

            // always return true when handled special npc spell click
            return true;
        }
    }

    return true;
}

/*######
## spell_taunka_face_me
######*/

struct TaunkaFaceMeSpellScript : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer())
            return;

        Player* player = static_cast<Player*>(target);
        spell->GetCaster()->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        player->PrepareGossipMenu(spell->GetCaster(), 9302);
        player->SendPreparedGossip(spell->GetCaster());
    }
};

/*######
## spell_capture_jormungar_spawn
######*/

struct CaptureJormungarSpawnSpellScript : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        // Capture Jormungar Spawn can be cast only on this target
        if (!target || target->GetEntry() != 26359)
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }
};

/*######
## spell_scrape_corrosive_spit
######*/

struct ScrapeCorrosiveSpit : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool/* strict*/) const override
    {
        if (!spell->GetCaster()->HasAura(47447))
            return SPELL_FAILED_CASTER_AURASTATE;
        return SPELL_CAST_OK;
    }
};

/*######
## spell_container_of_rats
######*/

struct ContainerOfRatsSpellScript : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        if (!target || target->IsAlive())
            return SPELL_FAILED_BAD_TARGETS;
        // Container of Rats can be cast only on these targets
        if (target->GetEntry() != 27202 && target->GetEntry() != 27203 && target->GetEntry() != 27206 && target->GetEntry() != 27207 && target->GetEntry() != 27210)
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }
};

/*######
## spell_drop_off_villager
######*/

struct DropOffVillager : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool/* strict*/) const override
    {
        if (!spell->GetCaster()->HasAura(43671))
            return SPELL_FAILED_NOT_READY;
        return SPELL_CAST_OK;
    }
};

/*######
## spell_army_of_the_dead
######*/

struct ArmyOfTheDead : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        data.caster = aura->GetCaster();
        data.target = aura->GetTarget();
    }
};

enum
{
    SPELL_UNDIGESTIBLE = 47430,
};

struct CorrosiveSpit : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        if (Unit* target = spell->m_targets.getUnitTarget())
        {
            if (target->HasAura(SPELL_UNDIGESTIBLE))
            {
                auto& list = spell->GetTargetList();
                for (auto& target : list)
                    target.effectHitMask &= ~(1 << EFFECT_INDEX_1);
            }
        }
    }
};

/*######
## go_scrying_orb
######*/

struct go_scrying_orb : public GameObjectAI
{
    go_scrying_orb(GameObject* go) : GameObjectAI(go)
    {
        go->GetVisibilityData().SetInvisibilityMask(1, true);
        go->GetVisibilityData().AddInvisibilityValue(1, 1000);
    }
};

/*######
## spell_twisting_blade
######*/

struct TwistingBlade : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        if (target->GetEntry() != 26316 && target->GetEntry() != 26575 && target->GetEntry() != 26577 && target->GetEntry() != 26578)
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }
};

/*######
## spell_ley_line_focus_item
######*/

struct spell_ley_line_focus_item : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        data.caster = aura->GetCaster();
        data.target = nullptr;
    }
};

struct spell_ley_line_focus_item_trigger : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* target = spell->GetUnitTarget();
        Unit* caster = spell->GetCaster();
        switch (spell->m_spellInfo->Id)
        {
            case 50546: target->CastSpell(caster, 47390, TRIGGERED_OLD_TRIGGERED); break;
            case 50547: target->CastSpell(caster, 47472, TRIGGERED_OLD_TRIGGERED); break;
            case 50548: target->CastSpell(caster, 47635, TRIGGERED_OLD_TRIGGERED); break;
        }
    }
};

enum
{
    SPELL_LIQUID_FIRE                   = 46770,
    SPELL_LIQUID_FIRE_AURA              = 47972,

    NPC_ELK                             = 26616,
    NPC_GRIZZLY                         = 26643,

    NPC_ELK_BUNNY                       = 27111,
    NPC_GRIZZLY_BUNNY                   = 27112,
};

// 46770 - Liquid Fire of Elune
struct LiquidFireOfElune : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        if (!target || target->HasAura(SPELL_LIQUID_FIRE_AURA))
            return SPELL_FAILED_BAD_TARGETS;

        if (target->GetEntry() != NPC_ELK && target->GetEntry() != NPC_GRIZZLY)
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Creature* target = dynamic_cast<Creature*>(spell->GetUnitTarget());
        if (!target)
            return;

        if (!spell->GetCaster()->IsPlayer())
            return;

        if (target->HasAura(SPELL_LIQUID_FIRE_AURA))
            return;

        if (target->GetEntry() == NPC_ELK)
        {
            target->CastSpell(nullptr, SPELL_LIQUID_FIRE_AURA, TRIGGERED_OLD_TRIGGERED);
            static_cast<Player*>(spell->GetCaster())->KilledMonsterCredit(NPC_ELK_BUNNY);
        }
        else if (target->GetEntry() == NPC_GRIZZLY)
        {
            target->CastSpell(nullptr, SPELL_LIQUID_FIRE_AURA, TRIGGERED_OLD_TRIGGERED);
            static_cast<Player*>(spell->GetCaster())->KilledMonsterCredit(NPC_GRIZZLY_BUNNY);
        }
    }
};

struct HourglassOfEternity : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        if (spell->GetCaster()->HasAura(50867)) // Hourglass of Eternity Nearby
            return SPELL_FAILED_FIZZLE;

        return SPELL_CAST_OK;
    }
};

// 48363 - Rescue Villager
struct RescueVillager : public SpellScript
{
    void OnSpellCastResultOverride(SpellCastResult& result, uint32& param1, uint32& param2) const override
    {
        if (result == SPELL_FAILED_OUT_OF_RANGE || result == SPELL_FAILED_BAD_TARGETS)
        {
            result = SPELL_FAILED_CUSTOM_ERROR;
            param1 = SPELL_FAILED_CUSTOM_ERROR_35;
        }
    }
};

// 49109 - Drop off Gnome
struct DropOffGnome : public SpellScript
{
    void OnSpellCastResultOverride(SpellCastResult& result, uint32& param1, uint32& param2) const override
    {
        if (result == SPELL_FAILED_OUT_OF_RANGE || result == SPELL_FAILED_BAD_TARGETS)
        {
            result = SPELL_FAILED_CUSTOM_ERROR;
            param1 = SPELL_FAILED_CUSTOM_ERROR_5;
        }
    }
};

void AddSC_dragonblight()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_destructive_ward";
    pNewScript->GetAI = &GetAI_npc_destructive_ward;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_crystalline_ice_giant";
    pNewScript->pNpcSpellClick = &NpcSpellClick_npc_crystalline_ice_giant;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_scrying_orb";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_scrying_orb>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<TaunkaFaceMeSpellScript>("spell_taunka_face_me");
    RegisterSpellScript<CaptureJormungarSpawnSpellScript>("spell_capture_jormungar_spawn");
    RegisterSpellScript<ScrapeCorrosiveSpit>("spell_scrape_corrosive_spit");
    RegisterSpellScript<ContainerOfRatsSpellScript>("spell_container_of_rats");
    RegisterSpellScript<DropOffVillager>("spell_drop_off_villager");
    RegisterSpellScript<TwistingBlade>("spell_twisting_blade");
    RegisterSpellScript<ArmyOfTheDead>("spell_army_of_the_dead");
    RegisterSpellScript<CorrosiveSpit>("spell_corrosive_spit");
    RegisterSpellScript<spell_ley_line_focus_item>("spell_ley_line_focus_ring");
    RegisterSpellScript<spell_ley_line_focus_item_trigger>("spell_ley_line_focus_item_trigger");
    RegisterSpellScript<LiquidFireOfElune>("spell_liquid_fire_of_elune");
    RegisterSpellScript<HourglassOfEternity>("spell_hourglass_of_eternity");
    RegisterSpellScript<RescueVillager>("spell_rescue_villager");
    RegisterSpellScript<DropOffGnome>("spell_drop_off_gnome");
}

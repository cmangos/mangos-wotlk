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
SDName: Storm_Peaks
SD%Complete: 100
SDComment: Quest support: 12832, 12977.
SDCategory: Storm Peaks
EndScriptData */

/* ContentData
npc_floating_spirit
npc_restless_frostborn
npc_injured_miner
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"

/*######
## npc_floating_spirit
######*/

enum
{
    SPELL_BLOW_HODIRS_HORN              = 55983,
    SPELL_SUMMON_FROST_GIANG_SPIRIT     = 55986,
    SPELL_SUMMON_FROST_WARRIOR_SPIRIT   = 55991,
    SPELL_SUMMON_FROST_GHOST_SPIRIT     = 55992,

    NPC_FROST_GIANT_GHOST_KC            = 30138,
    NPC_FROST_DWARF_GHOST_KC            = 30139,

    NPC_NIFFELEM_FOREFATHER             = 29974,
    NPC_FROSTBORN_WARRIOR               = 30135,
    NPC_FROSTBORN_GHOST                 = 30144,
};

struct npc_floating_spiritAI : public ScriptedAI
{
    npc_floating_spiritAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override
    {
        // Simple animation for the floating spirit
        m_creature->SetLevitate(true);
        m_creature->ForcedDespawn(5000);

        m_creature->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 50.0f);
    }
};

UnitAI* GetAI_npc_floating_spirit(Creature* pCreature)
{
    return new npc_floating_spiritAI(pCreature);
}

/*######
## npc_restless_frostborn
######*/

// 55983 - Blow Hodir's Horn
struct BlowHodirsHorn : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        if (!target || (target->GetEntry() != NPC_NIFFELEM_FOREFATHER && target->GetEntry() != NPC_FROSTBORN_WARRIOR && target->GetEntry() != NPC_FROSTBORN_GHOST))
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        uint32 uiCredit = 0;
        uint32 uiSpawnSpell = 0;
        switch (target->GetEntry())
        {
            case NPC_NIFFELEM_FOREFATHER:
                uiCredit = NPC_FROST_GIANT_GHOST_KC;
                uiSpawnSpell = SPELL_SUMMON_FROST_GIANG_SPIRIT;
                break;
            case NPC_FROSTBORN_WARRIOR:
                uiCredit = NPC_FROST_DWARF_GHOST_KC;
                uiSpawnSpell = SPELL_SUMMON_FROST_WARRIOR_SPIRIT;
                break;
            case NPC_FROSTBORN_GHOST:
                uiCredit = NPC_FROST_DWARF_GHOST_KC;
                uiSpawnSpell = SPELL_SUMMON_FROST_GHOST_SPIRIT;
                break;
        }

        // spawn the spirit and give the credit; spirit animation is handled by the script above
        caster->CastSpell(nullptr, uiSpawnSpell, TRIGGERED_OLD_TRIGGERED);
        static_cast<Player*>(caster)->KilledMonsterCredit(uiCredit);
    }
};

/*######
## npc_injured_miner
######*/

enum
{
    // yells
    SAY_MINER_READY                     = -1001051,
    SAY_MINER_COMPLETE                  = -1001052,

    // gossip
    GOSSIP_ITEM_ID_READY                = -3000112,
    TEXT_ID_POISONED                    = 13650,
    TEXT_ID_READY                       = 13651,

    // misc
    SPELL_FEIGN_DEATH                   = 51329,
    QUEST_ID_BITTER_DEPARTURE           = 12832,
};

struct npc_injured_minerAI : public npc_escortAI
{
    npc_injured_minerAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() override { }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            Start(true, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));
            SetEscortPaused(true);

            // set alternative waypoints if required
            if (m_creature->GetPositionX() > 6650.0f)
                SetCurrentWaypoint(8);
            else if (m_creature->GetPositionX() > 6635.0f)
                SetCurrentWaypoint(36);

            DoScriptText(SAY_MINER_READY, m_creature);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            m_creature->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
            m_creature->SetFactionTemporary(FACTION_ESCORT_N_FRIEND_ACTIVE, TEMPFACTION_RESTORE_RESPAWN);
        }
        else if (eventType == AI_EVENT_CUSTOM_A && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            SetEscortPaused(false);
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 34:
                DoScriptText(SAY_MINER_COMPLETE, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ID_BITTER_DEPARTURE, m_creature);
                    m_creature->SetFacingToObject(pPlayer);
                }
                break;
            case 35:
                m_creature->ForcedDespawn();
                break;
            case 47:
                // merge with the other wp path
                SetEscortPaused(true);
                SetCurrentWaypoint(14);
                SetEscortPaused(false);
                break;
        }
    }
};

UnitAI* GetAI_npc_injured_miner(Creature* pCreature)
{
    return new npc_injured_minerAI(pCreature);
}

bool GossipHello_npc_injured_miner(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (!pCreature->HasAura(SPELL_FEIGN_DEATH))
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_READY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_READY, pCreature->GetObjectGuid());
        return true;
    }

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_POISONED, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_injured_miner(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pPlayer, pCreature);
    }

    return true;
}

bool QuestAccept_npc_injured_miner(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_BITTER_DEPARTURE)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

// 54997 - Cast Net
struct CastNetStormforgedPursuer : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const
    {
        if (ObjectGuid target = spell->m_targets.getUnitTargetGuid()) // can be cast only on this target
            if (target.GetEntry() != 29696) // Stormforged Pursuer
                return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }
};

/*######
## npc_ethereal_frostworg
######*/

enum
{    // misc
    SPELL_SUMMON_INFILTRATOR                 = 56325,
    NPC_STORMFORGED_INFILTRATOR              = 30222,

    FROSTWOLF_MOVE_NORMAL = 1,
    FROSTWOLF_MOVE_FINAL = 2,

};

enum FrostwolfActions
{
    FROSTWOLF_CHANGE_DIRECTION,
    FROSTWOLF_RESUME_SEARCH,
    FROSTWOLF_FIND_INFILTRATOR,
    FROSTWOLF_SUMMON_INFILTRATOR,
};

static const uint32 ethereal_frostworg_emotes[4][2] =
{
    {30909, 30910},
    {30911, 30912},
    {30913, 30914},
    {30915, 30915}
};

struct npc_ethereal_frostworgAI : public ScriptedAI
{
    npc_ethereal_frostworgAI(Creature* creature) : ScriptedAI(creature)
    {
        AddCustomAction(FROSTWOLF_FIND_INFILTRATOR, 45s, [&]()
        {
            DisableTimer(FROSTWOLF_CHANGE_DIRECTION);
            DisableTimer(FROSTWOLF_RESUME_SEARCH);
            MoveToNewPoint(FROSTWOLF_MOVE_FINAL);
        }, TIMER_COMBAT_OOC);
        AddCustomAction(FROSTWOLF_CHANGE_DIRECTION, 15s, [&]()
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveIdle();
            ResetTimer(FROSTWOLF_RESUME_SEARCH, 3s);
            ResetTimer(FROSTWOLF_CHANGE_DIRECTION, 15s);
        }, TIMER_COMBAT_OOC);
        AddCustomAction(FROSTWOLF_RESUME_SEARCH, 0s, [&]()
        {
            switch (m_emoteCounter)
            {
                case 0:
                case 1:
                case 2:
                {
                    if (!m_creature->IsInCombat())
                        DoBroadcastText(ethereal_frostworg_emotes[m_emoteCounter][urand(0,1)], m_creature);
                    break;
                }
                case 3: break;
                default: m_creature->ForcedDespawn(); break;
            }
            ++m_emoteCounter;
            
            MoveToNewPoint(FROSTWOLF_MOVE_NORMAL);
        }, TIMER_COMBAT_OOC);
    }

    uint32 m_emoteCounter = 0;
    bool m_summoned = false;

    void MoveToNewPoint(uint32 type)
    {
        float dist = type == 1 ? 50.f : 20.f;
        float x, y, z = 0.f;
        while (abs(z - m_creature->GetPositionZ()) > 5)
            m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), dist, x, y, z, 10.f);
        m_creature->GetMotionMaster()->MovePoint(type, Position(x, y, z), FORCED_MOVEMENT_RUN);
    }

    void Reset() override
    {
        MoveToNewPoint(FROSTWOLF_MOVE_NORMAL);
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_STORMFORGED_INFILTRATOR)
            m_creature->ForcedDespawn();
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (urand(0,2))
        {
            case 0: DoBroadcastText(30916, summoned); break;
            case 1: DoBroadcastText(30917, summoned); break;
            case 2: DoBroadcastText(30918, summoned); break;
        }
    }

    void MovementInform(uint32 movementType, uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case FROSTWOLF_MOVE_NORMAL:
            {
                MoveToNewPoint(FROSTWOLF_MOVE_NORMAL);
                break;
            }
            case FROSTWOLF_MOVE_FINAL:
            {
                if (m_summoned)
                    return;
                m_summoned = true;
                m_creature->GetMotionMaster()->Clear(true, true);
                AddCustomAction(FROSTWOLF_SUMMON_INFILTRATOR, 2s, [&]() { DoCastSpellIfCan(nullptr, SPELL_SUMMON_INFILTRATOR); });
                DoBroadcastText(ethereal_frostworg_emotes[3][0], m_creature);
                break;
            }
            default: break;
        }
    }
};

/*######
## go_fjorns_anvil
######*/

struct go_fjorns_anvil : public GameObjectAI
{
    go_fjorns_anvil(GameObject* go) : GameObjectAI(go)
    {
        go->GetVisibilityData().SetInvisibilityMask(7, true);
        go->GetVisibilityData().AddInvisibilityValue(7, 50);
    }
};

/*######
## go_hodirs_helm
######*/

struct go_hodirs_helm : public GameObjectAI
{
    go_hodirs_helm(GameObject* go) : GameObjectAI(go)
    {
        go->GetVisibilityData().SetInvisibilityMask(9, true);
        go->GetVisibilityData().AddInvisibilityValue(9, 1000);
    }
};

/*######
## go_hodirs_horn
######*/

struct go_hodirs_horn : public GameObjectAI
{
    go_hodirs_horn(GameObject* go) : GameObjectAI(go)
    {
        go->GetVisibilityData().SetInvisibilityMask(8, true);
        go->GetVisibilityData().AddInvisibilityValue(8, 1000);
    }
};

/*######
## go_krolmir
######*/

struct go_krolmir : public GameObjectAI
{
    go_krolmir(GameObject* go) : GameObjectAI(go)
    {
        go->GetVisibilityData().SetInvisibilityMask(7, true);
        go->GetVisibilityData().AddInvisibilityValue(7, 100);
    }
};

/*######
## go_hodirs_spear
######*/

struct go_hodirs_spear : public GameObjectAI
{
    go_hodirs_spear(GameObject* go) : GameObjectAI(go)
    {
        go->GetVisibilityData().SetInvisibilityMask(5, true);
        go->GetVisibilityData().AddInvisibilityValue(5, 1000);
    }
};

/*######
## go_falling_rocks
######*/

struct go_falling_rocks : public GameObjectAI
{
    go_falling_rocks(GameObject* go) : GameObjectAI(go)
    {
        go->GetVisibilityData().SetInvisibilityMask(7, true);
        go->GetVisibilityData().AddInvisibilityValue(7, 1000);
    }
};

enum
{
    // quest 12981
    SPELL_THROW_ICE                     = 56099,
    SPELL_FROZEN_IRON_SCRAP             = 56101,
    NPC_SMOLDERING_SCRAP_BUNNY          = 30169,
    GO_SMOLDERING_SCRAP                 = 192124,
};

// 56099 - Throw Ice
struct ThrowIce : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        return target->GetEntry() == NPC_SMOLDERING_SCRAP_BUNNY;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (target == nullptr)
            return;

        if (GameObject* scrap = GetClosestGameObjectWithEntry(target, GO_SMOLDERING_SCRAP, 5.0f))
        {
            if (scrap->GetRespawnTime() != 0)
                return;

            target->CastSpell(nullptr, SPELL_FROZEN_IRON_SCRAP, TRIGGERED_OLD_TRIGGERED);
            scrap->SetLootState(GO_JUST_DEACTIVATED);
            static_cast<Creature*>(target)->ForcedDespawn(1000);
        }
    }
};

// 54798 - FLAMING Arrow Triggered Effect
struct FArrowTEff : public AuraScript
{
    enum
    {
        NPC_GIANT       = 29351,    // Niffelem Frost Giant 29351
        NPC_FROSTWORG   = 29358,    // Frostworg 29358
    };

    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* caster = aura->GetCaster();
        Unit* target = aura->GetTarget();

        if (target->HasAura(54690))
            return;
        else
        {
            if (target->GetEntry() == NPC_FROSTWORG)
            {
                target->CastSpell(nullptr, 54683, TRIGGERED_OLD_TRIGGERED); // 54683 Ablaze
                target->CastSpell(nullptr, 54690, TRIGGERED_OLD_TRIGGERED); // 54690 Cosmetic - Immolation (Whole Body) 6 Sec
                target->CastSpell(caster, 58183, TRIGGERED_OLD_TRIGGERED);  // 58183 Frostworg Kill Credit 02
            }
            else if (target->GetEntry() == NPC_GIANT)
            {
                target->CastSpell(nullptr, 54683, TRIGGERED_OLD_TRIGGERED); // 54683 Ablaze
                target->CastSpell(nullptr, 54690, TRIGGERED_OLD_TRIGGERED); // 54690 Cosmetic - Immolation (Whole Body) 6 Sec
                target->CastSpell(caster, 58184, TRIGGERED_OLD_TRIGGERED);  // 58184 Frost Giant Kill Credit 02
            }
        }
    }
};

// 54952 - Initial Proto-Drake
struct InitialProtoDrake : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        Unit* caster = spell->GetCaster();
        summon->SetFactionTemporary(spell->GetCaster()->GetFaction());
    }
};

// 55028 - Summon Freed Proto-Drake
struct SummonFreedProtoDrake : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        spell->GetCaster()->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
    }
};

// 56750 - Gather Snow
struct GatherSnow : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();

        if (GameObject* snow = GetClosestGameObjectWithEntry(target, 192075, 10.0f))
        {
            if (snow->GetRespawnTime() != 0)
                return;

            snow->SetLootState(GO_JUST_DEACTIVATED);
        }
    }
};

// 56753 - Throw Snowball
struct ThrowSnowball : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        // Throw Snowball can be cast only on this target
        if (!target || target->GetEntry() != 30120)
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }
};

// 56278 - Read Pronouncement
struct ReadPronouncement : public AuraScript
{
    enum
    {
        SPELL_SEE_INVIS_TIER_3  = 56773,

        SAY_1                   = 30906,
        SAY_2                   = 30907,
    };

    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* caster = aura->GetCaster();

        if (caster->HasAura(SPELL_SEE_INVIS_TIER_3))
            return;
        else
        {
            static_cast<Player*>(caster)->KilledMonsterCredit(30210);
            DoBroadcastText(SAY_1, caster, caster);
            DoBroadcastText(SAY_2, caster, caster);
            caster->CastSpell(caster, SPELL_SEE_INVIS_TIER_3, TRIGGERED_OLD_TRIGGERED); // 56773 See Invis Tier 3
        }
    }
};

// 56671 - Spear of Hodir
struct SpearOfHodir : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        // Throw Snowball can be cast only on this target
        if (!target || target->GetEntry() != 30275)
            return SPELL_FAILED_BAD_TARGETS;
        if (target->IsInCombat())
            return SPELL_FAILED_TARGET_IN_COMBAT;

        return SPELL_CAST_OK;
    }
};

// 56689 - Grip
struct Grip : public AuraScript
{
    enum
    {
        SPELL_FIGHT_WYRM            = 56673,
        SPELL_LOW_HEALTH_TRIGGER    = 60596,
    };

    void OnHolderInit(SpellAuraHolder* holder, WorldObject* /*caster*/) const override
    {
        holder->PresetAuraStacks(50);
    }
    void OnPeriodicTickEnd(Aura* aura) const override
    {
        if (!aura)
            return;

        Unit* caster = aura->GetCaster();
        if (!caster)
            return;

        SpellAuraHolder* auraHolder = caster->GetSpellAuraHolder(56689);
        if (!auraHolder)
            return;

        uint8 stacks = auraHolder->GetStackAmount();

        if (stacks > 1)
        {
            caster->RemoveAuraStack(56689, -1);
        }

        if (stacks <= 1)
        {
            caster->RemoveAurasDueToSpell(SPELL_FIGHT_WYRM);
        }

        // Phase 2 switch
        if (caster->GetHealthPercent() <= 25.0f)
        {
            // caster->RemoveAurasDueToSpell(SPELL_FIGHT_WYRM);
            caster->AI()->SendAIEventAround(AI_EVENT_CUSTOM_EVENTAI_A, caster,0,5);
        }
    }
};

// 60533 - Grab On
struct GrabOn : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();

        // adds 10 stacks
        target->RemoveAuraStack(56689, 10);
    }
};

// 60776 - Claw Swipe
struct ClawSwipe : public SpellScript
{
    enum
    {
        SPELL_GRIP          = 56689,
        SPELL_DODGW_CLAWS   = 56704,

        SAY_1               = 32763,
        SAY_2               = 32794,
        SAY_3               = 32784,
    };

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();

        if (target->HasAura(SPELL_DODGW_CLAWS))
        {
            DoBroadcastText(SAY_2, caster, target, 5);
        }
        else
        {
            // removes 5 stacks
            DoBroadcastText(SAY_1, caster, target, 5);
            caster->RemoveAuraStack(SPELL_GRIP, -5);
        }

        SpellAuraHolder* auraHolder = caster->GetSpellAuraHolder(SPELL_GRIP);
        
        if (!auraHolder)
            return;

        uint8 stacks = auraHolder->GetStackAmount();

        if (stacks <= 20)
        {
            DoBroadcastText(SAY_3, caster, target, 5);
        }
    }
};

// 60587 - Fatal Strike
struct FatalStrike : public SpellScript
{
    enum
    {
        SPELL_PRY_JAWS_OPEN     = 56706,
        SPELL_FSTRIKE           = 60881,

        SAY_MISS                = 32795,
    };

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        if (!caster)
            return;

        Unit* master = caster->GetMaster();
        if (!master)
            return;

        if (SpellAuraHolder* holder = caster->GetSpellAuraHolder(SPELL_PRY_JAWS_OPEN))
        {
            if (roll_chance_i(holder->GetStackAmount() * 5))
            {
                caster->CastSpell(caster, SPELL_FSTRIKE, TRIGGERED_OLD_TRIGGERED);
            }
            else
            {
                DoBroadcastText(SAY_MISS, caster, master, 5);
            }
        }
    }
};

// 60864 - Jaws of Death
struct JawsOfDeath : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        spell->SetDamage(target->GetMaxHealth() * 0.03f); // damage = 3% master HP instead of flat 3
    }
};

// 61319 - Jokkum Scriptcast
struct JokkumScriptcast : public SpellScript, public AuraScript
{
    void OnRadiusCalculate(Spell* /*spell*/, SpellEffectIndex /*effIdx*/, bool /*targetB*/, float& radius) const override
    {
        radius = 50.f;
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();

            target->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
            target->Unmount();
    }
};


void AddSC_storm_peaks()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_floating_spirit";
    pNewScript->GetAI = &GetAI_npc_floating_spirit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_injured_miner";
    pNewScript->GetAI = &GetAI_npc_injured_miner;
    pNewScript->pGossipHello = &GossipHello_npc_injured_miner;
    pNewScript->pGossipSelect = &GossipSelect_npc_injured_miner;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_injured_miner;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ethereal_frostworg";
    pNewScript->GetAI = &GetNewAIInstance<npc_ethereal_frostworgAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_falling_rocks";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_falling_rocks>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_fjorns_anvil";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_fjorns_anvil>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_hodirs_helm";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_hodirs_helm>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_hodirs_horn";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_hodirs_horn>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_hodirs_spear";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_hodirs_spear>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_krolmir";
    pNewScript->GetGameObjectAI = &GetNewAIInstance<go_krolmir>;
    pNewScript->RegisterSelf();
    
    RegisterSpellScript<BlowHodirsHorn>("spell_blow_hodirs_horn");
    RegisterSpellScript<CastNetStormforgedPursuer>("spell_cast_net_stormforged_pursuer");
    RegisterSpellScript<ThrowIce>("spell_throw_ice");
    RegisterSpellScript<FArrowTEff>("spell_flaming_arrow_triggered_effect");
    RegisterSpellScript<SummonFreedProtoDrake>("spell_summon_freed_protodrake");
    RegisterSpellScript<GatherSnow>("spell_gather_snow");
    RegisterSpellScript<ThrowSnowball>("spell_throw_snowball");
    RegisterSpellScript<ReadPronouncement>("spell_read_pronouncement");
    RegisterSpellScript<Grip>("spell_grip");
    RegisterSpellScript<GrabOn>("spell_grab_on");
    RegisterSpellScript<ClawSwipe>("spell_claw_swipe");
    RegisterSpellScript<SpearOfHodir>("spell_spear_of_hodir");
    RegisterSpellScript<FatalStrike>("spell_fatal_strike");
    RegisterSpellScript<JawsOfDeath>("spell_jaws_of_death");
    RegisterSpellScript<JokkumScriptcast>("spell_jokkum_scriptcast");
    RegisterSpellScript<InitialProtoDrake>("spell_initial_proto_drake");
}

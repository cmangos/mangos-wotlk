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
SDName: Boss_Brutallus
SD%Complete: 90
SDComment: Intro may need some adjustments
SDCategory: Sunwell Plateau
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "sunwell_plateau.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    YELL_INTRO                      = -1580017,
    YELL_INTRO_BREAK_ICE            = -1580018,
    YELL_INTRO_CHARGE               = -1580019,
    YELL_INTRO_KILL_MADRIGOSA       = -1580020,
    YELL_INTRO_TAUNT                = -1580021,

    YELL_MADR_ICE_BARRIER           = -1580031,
    YELL_MADR_INTRO                 = -1580032,
    YELL_MADR_ICE_BLOCK             = -1580033,
    YELL_MADR_TRAP                  = -1580034,
    YELL_MADR_DEATH                 = -1580035,

    YELL_AGGRO                      = -1580022,
    YELL_KILL1                      = 25629,
    YELL_KILL2                      = 25630,
    YELL_KILL3                      = 25631,
    YELL_LOVE1                      = -1580026,
    YELL_LOVE2                      = -1580027,
    YELL_LOVE3                      = -1580028,
    YELL_BERSERK                    = -1580029,
    YELL_DEATH                      = -1580030,

    SPELL_METEOR_SLASH              = 45150,
    SPELL_BURN                      = 45141,
    SPELL_BURN_DAMAGE               = 46394,
    SPELL_STOMP                     = 45185,
    SPELL_BERSERK                   = 26662,
    SPELL_SUMMON_DEATH_CLOUD        = 45884,                // Summoned on death

    SPELL_DUAL_WEILD_PASSIVE        = 42459,
    SPELL_TAUNT_HIT_CHANCE          = 45210,

    // Epilogue spells
    SPELL_BRUTALLUS_DEATH_CLOUD     = 45212,
    SPELL_FELBLAZE_PREVIZUAL        = 44885,
    SPELL_SUMMON_FELBLAZE           = 45069,

    NPC_BRUTALLUS_DEATH_CLOUD       = 25703,

    // spells used during the intro event
    SPELL_FROST_BLAST               = 45203,                // Madrigosa's spells
    SPELL_FREEZE                    = 46609,                // Activates the ice barrier - script effect for 46610
    SPELL_FREEZE_USE_GO             = 46610,
    SPELL_FROSTBOLT                 = 44843,
    SPELL_FROST_BREATH              = 45065,
    SPELL_ENCAPSULATE               = 44883,
    SPELL_FEL_FIREBALL              = 44844,                // Brutallus' spells
    SPELL_CLEAR_DEBUFFS             = 34098,
    SPELL_FLAME_RING                = 44874,                // this spell should have a fire explosion when removed
    SPELL_CHARGE                    = 44884,
    SPELL_BREAK_ICE                 = 46637,                // Break the ice, open the door - dummy spell for 46638 and 47030
    SPELL_ARCANE_EXPLOSION_VISUAL   = 35426,
    SPELL_FULL_HEAL                 = 17683,

    SPELL_PERMANENT_FEIGN_DEATH     = 29266,

    SPELL_SELF_STUN                 = 45066,

    POINT_MOVE_GROUND               = 1,
    POINT_MOVE_ICE_BLOCK            = 2,
    POINT_MOVE_ENCAPSULATE          = 3,
    POINT_MOVE_FEL_FIREBALL         = 4,
};

static const DialogueEntry aIntroDialogue[] =
{
    {NPC_MADRIGOSA,             0,              6000},
    {YELL_MADR_ICE_BARRIER,     NPC_MADRIGOSA,  7000},
    {YELL_MADR_INTRO,           NPC_MADRIGOSA,  7000},
    {YELL_INTRO,                NPC_BRUTALLUS,  6000},
    {SPELL_FROST_BREATH,        0,              6000},
    {POINT_MOVE_ICE_BLOCK,      0,              5000},
    {YELL_MADR_ICE_BLOCK,       NPC_MADRIGOSA,  5000},
    {SPELL_FLAME_RING,          0,              7000},
    {YELL_INTRO_BREAK_ICE,      NPC_BRUTALLUS,  1000},
    {SPELL_FEL_FIREBALL,        0,              4000},
    {POINT_MOVE_GROUND,         0,              5000},
    {YELL_MADR_TRAP,            NPC_MADRIGOSA,  3000},
    {POINT_MOVE_ENCAPSULATE,    0,              7000},
    {YELL_INTRO_CHARGE,         NPC_BRUTALLUS,  2000},
    {SPELL_CHARGE,              NPC_BRUTALLUS,  5000},
    {SPELL_SELF_STUN,           0,              5000},
    {YELL_INTRO_KILL_MADRIGOSA, NPC_BRUTALLUS,  8000},
    {YELL_INTRO_TAUNT,          NPC_BRUTALLUS,  0},
    {0, 0, 0},
};

/*######
## boss_brutallus
######*/

enum BrutallusActions
{
    BRUTALLUS_BERSERK,
    BRUTALLUS_SLASH,
    BRUTALLUS_STOMP,
    BRUTALLUS_BURN,
    BRUTALLUS_LOVE,
    BRUTALLUS_ACTION_MAX,
    BRUTALLUS_MADRIGOSA_FROSTBOLT,
    BRUTALLUS_MADRIGOSA_SPAWN_MOVE,
};

struct boss_brutallusAI : public CombatAI, private DialogueHelper
{
    boss_brutallusAI(Creature* creature) : CombatAI(creature, BRUTALLUS_ACTION_MAX), m_instance(static_cast<instance_sunwell_plateau*>(creature->GetInstanceData())),
        DialogueHelper(aIntroDialogue), m_bIsIntroInProgress(false)
    {
        InitializeDialogueHelper(m_instance);
        AddCombatAction(BRUTALLUS_BERSERK, uint32(6 * MINUTE * IN_MILLISECONDS));
        AddCombatAction(BRUTALLUS_SLASH, 11000u);
        AddCombatAction(BRUTALLUS_STOMP, 30000u);
        AddCombatAction(BRUTALLUS_BURN, 20000u);
        AddCombatAction(BRUTALLUS_LOVE, 10000, 17000);
        AddCustomAction(BRUTALLUS_MADRIGOSA_FROSTBOLT, true, [&]() { HandleMadrigosaFrostbolt(); });
        AddCustomAction(BRUTALLUS_MADRIGOSA_SPAWN_MOVE, true, [&]() { HandleMadrigosaSpawnMove(); });
        AddOnKillText(YELL_KILL1, YELL_KILL2, YELL_KILL3);
        m_creature->SetCorpseAccelerationDelay(30000);
        AddUnreachabilityCheck();
    }

    instance_sunwell_plateau* m_instance;

    bool m_bIsIntroInProgress;

    void Reset() override
    {
        CombatAI::Reset();

        DoCastSpellIfCan(nullptr, SPELL_DUAL_WEILD_PASSIVE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_TAUNT_HIT_CHANCE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* who) override
    {
        // Don't aggro when attacking Madrigosa
        if (who->GetEntry() == NPC_MADRIGOSA)
            return;

        DoScriptText(YELL_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_BRUTALLUS, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(YELL_DEATH, m_creature);
        DoCastSpellIfCan(nullptr, SPELL_SUMMON_DEATH_CLOUD, CAST_TRIGGERED);

        if (m_instance)
            m_instance->SetData(TYPE_BRUTALLUS, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_BRUTALLUS, FAIL);
    }

    void GetAIInformation(ChatHandler& reader) override
    {
        if (m_instance)
        {
            if (m_instance->GetData(TYPE_BRUTALLUS) == SPECIAL)
                reader.PSendSysMessage("Brutallus intro event is currently %s", m_bIsIntroInProgress ? "in progress" : "completed");
            else
                reader.PSendSysMessage("Brutallus intro event is currently %s", m_instance->GetData(TYPE_BRUTALLUS) == NOT_STARTED ? "not started" : "completed");

            if (m_instance->GetData(TYPE_BRUTALLUS) != NOT_STARTED)
            {
                if (Creature* madrigosa = m_instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA, true))
                    reader.PSendSysMessage("Madrigosa guid is %s and has %u health.", madrigosa->GetGuidStr().c_str(), madrigosa->GetHealth());
            }
        }
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        // Error log if Madrigosa dies
        if (summoned->GetEntry() == NPC_MADRIGOSA)
            script_error_log("Npc %u, %s, died unexpectedly. Felmyst won't be summoned anymore.", summoned->GetEntry(), summoned->GetName());
    }

    void SummonedCreatureDespawn(Creature* summoned) override
    {
        // Yell of Madrigosa on death
        if (summoned->GetEntry() == NPC_MADRIGOSA)
            summoned->CastSpell(nullptr, SPELL_SUMMON_FELBLAZE, TRIGGERED_OLD_TRIGGERED);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_MADRIGOSA)
        {
            summoned->SetWalk(false);
            summoned->SetLevitate(false);
            summoned->SetHover(true);
            static_cast<CreatureAI*>(summoned->AI())->SetDeathPrevention(true);
            ResetTimer(BRUTALLUS_MADRIGOSA_SPAWN_MOVE, 2000);
        }
        else if (summoned->GetEntry() == NPC_BRUTALLUS_DEATH_CLOUD)
            summoned->CastSpell(nullptr, SPELL_BRUTALLUS_DEATH_CLOUD, TRIGGERED_OLD_TRIGGERED);
    }

    void SummonedMovementInform(Creature* summoned, uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE || summoned->GetEntry() != NPC_MADRIGOSA)
            return;

        if (pointId == POINT_MOVE_GROUND)
        {
            summoned->SetLevitate(false);
            summoned->SetHover(false);
        }
        else if (pointId == POINT_MOVE_FEL_FIREBALL)
        {
            summoned->HandleEmote(EMOTE_ONESHOT_LAND);
            summoned->GetMotionMaster()->MoveFall();
            summoned->SetLevitate(false);
            summoned->SetHover(false);
        }
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spellInfo) override
    {
        // Fake death Madrigosa when charged
        if (target->GetEntry() == NPC_MADRIGOSA && spellInfo->Id == SPELL_CHARGE)
        {
            DoScriptText(YELL_MADR_DEATH, target);
            target->InterruptNonMeleeSpells(true);
            target->StopMoving();
            target->CombatStop();
            target->ClearComboPointHolders();
            target->RemoveAllAurasOnDeath();
            target->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
            target->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
            target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            target->ClearAllReactives();
            target->GetMotionMaster()->Clear();
            target->GetMotionMaster()->MoveIdle();
            target->GetMotionMaster()->MoveFall();
            target->CastSpell(nullptr, SPELL_SELF_STUN, TRIGGERED_OLD_TRIGGERED);

            // Brutallus evades
            EnterEvadeMode();
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        if (!m_instance)
            return;

        switch (iEntry)
        {
            case NPC_MADRIGOSA:
                m_creature->SummonCreature(NPC_MADRIGOSA, 1470.362f, 738.1818f, 64.16677f, 4.625123f, TEMPSPAWN_DEAD_DESPAWN, 0);
                m_bIsIntroInProgress = true;
                break;
            case YELL_MADR_ICE_BARRIER:
                if (Creature* madrigosa = m_instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                    madrigosa->CastSpell(madrigosa, SPELL_FREEZE, TRIGGERED_NONE);
                break;
            case YELL_MADR_INTRO:
                if (Creature* madrigosa = m_instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                    madrigosa->GetMotionMaster()->MovePoint(POINT_MOVE_GROUND, aMadrigosaLoc[0]);
                break;
            case YELL_INTRO:
                if (Creature* madrigosa = m_instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                {
                    madrigosa->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                    m_creature->AI()->AttackStart(madrigosa);
                    // do not cast spells during RP
                    DisableCombatAction(BRUTALLUS_SLASH);
                    DisableCombatAction(BRUTALLUS_STOMP);
                    DisableCombatAction(BRUTALLUS_BURN);
                    DisableCombatAction(BRUTALLUS_LOVE);
                }
                break;
            case SPELL_FROST_BREATH:
                if (Creature* madrigosa = m_instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                {
                    madrigosa->CastSpell(m_creature, SPELL_FROST_BREATH, TRIGGERED_NONE);
                    madrigosa->GetMotionMaster()->MoveIdle();
                }
                break;
            case POINT_MOVE_ICE_BLOCK:
                SetMeleeEnabled(false);
                if (Creature* madrigosa = m_instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                {
                    madrigosa->AI()->SetMeleeEnabled(false);
                    madrigosa->HandleEmote(EMOTE_ONESHOT_LIFTOFF);
                    madrigosa->SetHover(true);
                    madrigosa->GetMotionMaster()->MovePoint(POINT_MOVE_ICE_BLOCK, aMadrigosaLoc[1].x, aMadrigosaLoc[1].y, aMadrigosaLoc[1].z);
                }
                // Temporary! This will make Brutallus not follow Madrigosa through the air until mmaps are implemented
                m_creature->SetImmobilizedState(true);
                break;
            case YELL_MADR_ICE_BLOCK:
                if (Creature* madrigosa = m_instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                    madrigosa->CastSpell(m_creature, SPELL_FROST_BLAST, TRIGGERED_OLD_TRIGGERED);
                ResetTimer(BRUTALLUS_MADRIGOSA_FROSTBOLT, 2000);
                break;
            case SPELL_FLAME_RING:
                DoCastSpellIfCan(nullptr, SPELL_CLEAR_DEBUFFS, CAST_TRIGGERED);
                DoCastSpellIfCan(nullptr, SPELL_FLAME_RING, CAST_TRIGGERED);
                break;
            case YELL_INTRO_BREAK_ICE:
                m_creature->RemoveAurasDueToSpell(SPELL_FLAME_RING);
                break;
            case SPELL_FEL_FIREBALL:
                // Spell has script target Madrigosa
                DoCastSpellIfCan(m_creature, SPELL_FEL_FIREBALL);
                break;
            case POINT_MOVE_GROUND:
                if (Creature* madrigosa = m_instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                    madrigosa->GetMotionMaster()->MovePoint(POINT_MOVE_FEL_FIREBALL, aMadrigosaLoc[2]);
                DisableTimer(BRUTALLUS_MADRIGOSA_FROSTBOLT);
                break;
            case YELL_MADR_TRAP:
                m_creature->SetImmobilizedState(false);
                m_creature->SetHover(true);
                m_creature->SetLevitate(true);
                SetCombatMovement(false);
                if (Creature* madrigosa = m_instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                {
                    madrigosa->CastSpell(m_creature, SPELL_ENCAPSULATE, TRIGGERED_OLD_TRIGGERED);
                    // Need to remove the fire aura after 4 sec so Madrigosa won't die so soon
                    madrigosa->RemoveAurasDueToSpell(SPELL_FEL_FIREBALL);
                }
                break;
            case POINT_MOVE_ENCAPSULATE:
                m_creature->SetIgnoreMMAP(true);
                m_creature->GetMotionMaster()->MovePoint(POINT_MOVE_ENCAPSULATE, Position(1463.13f, 578.7533f, 44.51308f, 0.f), FORCED_MOVEMENT_RUN);
                break;
            case YELL_INTRO_CHARGE:
                m_creature->RemoveAurasDueToSpell(SPELL_ENCAPSULATE);
                m_creature->GetMotionMaster()->MoveFall();
                break;
            case SPELL_CHARGE:
                m_creature->SetIgnoreMMAP(false);
                m_creature->SetHover(false);
                m_creature->SetLevitate(false);
                SetMeleeEnabled(true);
                SetCombatMovement(true, true);
                DoCastSpellIfCan(nullptr, SPELL_CHARGE);
                break;
            case SPELL_SELF_STUN:
                if (Creature* madrigosa = m_instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                {
                    madrigosa->RemoveAurasDueToSpell(SPELL_SELF_STUN);
                    madrigosa->CastSpell(nullptr, SPELL_PERMANENT_FEIGN_DEATH, TRIGGERED_OLD_TRIGGERED);
                }
                break;
            case YELL_INTRO_KILL_MADRIGOSA:
                DoCastSpellIfCan(nullptr, SPELL_FULL_HEAL);
                // Face the players
                if (GameObject* pIceWall = m_instance->GetSingleGameObjectFromStorage(GO_ICE_BARRIER))
                    m_creature->SetFacingToObject(pIceWall);

                if (Creature* madrigosa = m_instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                {
                    madrigosa->RemoveAurasDueToSpell(SPELL_SELF_STUN);
                    madrigosa->CastSpell(nullptr, SPELL_PERMANENT_FEIGN_DEATH, TRIGGERED_OLD_TRIGGERED);
                }
                break;
            case YELL_INTRO_TAUNT:
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                DoCastSpellIfCan(m_creature, SPELL_BREAK_ICE);
                m_bIsIntroInProgress = false;
                break;
        }
    }

    // Wrapper to start the dialogue text
    void DoStartIntro()
    {
        m_instance->StartBrutallusIntro();
        StartNextDialogueText(NPC_MADRIGOSA);
    }

    void HandleMadrigosaFrostbolt()
    {
        if (Creature* madrigosa = m_instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
            madrigosa->CastSpell(m_creature, SPELL_FROSTBOLT, TRIGGERED_OLD_TRIGGERED);
        ResetTimer(BRUTALLUS_MADRIGOSA_FROSTBOLT, urand(1000, 2000));
    }

    void HandleMadrigosaSpawnMove()
    {
        if (Creature* madrigosa = m_instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
        {
            madrigosa->SetIgnoreMMAP(true);
            madrigosa->GetMotionMaster()->MovePoint(0, aMadrigosaLoc[1].x, aMadrigosaLoc[1].y, aMadrigosaLoc[1].z);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case BRUTALLUS_BERSERK:
                if (DoCastSpellIfCan(nullptr, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(YELL_BERSERK, m_creature);
                    DisableCombatAction(action);
                }
                break;
            case BRUTALLUS_SLASH:
                if (DoCastSpellIfCan(nullptr, SPELL_METEOR_SLASH) == CAST_OK)
                    ResetCombatAction(action, 11000);
                break;
            case BRUTALLUS_STOMP:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_STOMP) == CAST_OK)
                    ResetCombatAction(action, 30000);
                break;
            case BRUTALLUS_BURN:
                // different spell in choice due to requiring no aura check
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_BURN_DAMAGE, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_AURA | SELECT_FLAG_SKIP_TANK))
                    if (DoCastSpellIfCan(target, SPELL_BURN) == CAST_OK)
                        ResetCombatAction(action, 20000);
                break;
            case BRUTALLUS_LOVE:
                switch (urand(0, 4))
                {
                    case 0: DoScriptText(YELL_LOVE1, m_creature); break;
                    case 1: DoScriptText(YELL_LOVE2, m_creature); break;
                    case 2: DoScriptText(YELL_LOVE3, m_creature); break;
                    default: break;
                }
                ResetCombatAction(action, urand(15000, 23000));
                break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        // Update only the intro related stuff
        if (m_instance && m_instance->IsBrutallusIntroStarted() && m_bIsIntroInProgress)
            DialogueUpdate(diff); // Dialogue updates outside of combat too

        CombatAI::UpdateAI(diff);
    }
};

/*######
## at_madrigosa
######*/

bool AreaTrigger_at_madrigosa(Player* player, AreaTriggerEntry const* /*at*/)
{
    if (player->IsGameMaster())
        return false;

    if (instance_sunwell_plateau* instance = static_cast<instance_sunwell_plateau*>(player->GetInstanceData()))
        if (Creature* brutallus = instance->GetSingleCreatureFromStorage(NPC_BRUTALLUS))
            if (brutallus->IsAlive() && !instance->IsBrutallusIntroStarted())
                if (boss_brutallusAI* bossAI = dynamic_cast<boss_brutallusAI*>(brutallus->AI()))
                    bossAI->DoStartIntro();

    return false;
}

struct SpellBurnBrutallus : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex eff) const override
    {
        if (target->HasAura(SPELL_BURN_DAMAGE) || target->IsImmuneToSchool(sSpellTemplate.LookupEntry<SpellEntry>(SPELL_BURN_DAMAGE), (1 << eff)))
            return false;
        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        spell->GetUnitTarget()->CastSpell(nullptr, SPELL_BURN_DAMAGE, TRIGGERED_OLD_TRIGGERED);
    }
};

struct SpellBurnDamageBrutallus : public AuraScript
{
    void OnPeriodicCalculateAmount(Aura* aura, uint32& amount) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0 && aura->GetAuraTicks() % 11 == 0)
            aura->GetModifier()-> m_amount *= 2;

        amount = aura->GetModifier()->m_amount;
    }
};

struct StompBrutallus : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_2)
            return;
        spell->GetUnitTarget()->RemoveAurasDueToSpell(SPELL_BURN_DAMAGE);
    }
};

struct DeathCloudBrutallus : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        // On Aura removal start Felmyst summon visuals
        if (aura->GetEffIndex() == EFFECT_INDEX_0 && !apply)
        {
            if (ScriptedInstance* instance = static_cast<ScriptedInstance*>(aura->GetTarget()->GetInstanceData()))
            {
                if (Creature* madrigosa = instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                {
                    // Set respawn pos to current pos
                    madrigosa->SetRespawnCoord(madrigosa->GetPositionX(), madrigosa->GetPositionY(), madrigosa->GetPositionZ(), madrigosa->GetOrientation());

                    madrigosa->CastSpell(nullptr, SPELL_FELBLAZE_PREVIZUAL, TRIGGERED_OLD_TRIGGERED);
                    madrigosa->ForcedDespawn(10000);
                }
            }
        }
    }
};

struct FreezeMadrigosa : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        spell->GetUnitTarget()->CastSpell(nullptr, SPELL_FREEZE_USE_GO, TRIGGERED_OLD_TRIGGERED);
    }
};

struct spell_break_ice : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->GetTarget()->CastSpell(nullptr, 46638, TRIGGERED_OLD_TRIGGERED);
        else
            aura->GetTarget()->CastSpell(nullptr, 47030, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_brutallus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_brutallus";
    pNewScript->GetAI = &GetNewAIInstance<boss_brutallusAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_madrigosa";
    pNewScript->pAreaTrigger = &AreaTrigger_at_madrigosa;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SpellBurnBrutallus>("spell_burn_brutallus");
    RegisterSpellScript<SpellBurnDamageBrutallus>("spell_burn_damage_brutallus");
    RegisterSpellScript<StompBrutallus>("spell_stomp_brutallus");
    RegisterSpellScript<DeathCloudBrutallus>("spell_death_cloud_brutallus");
    RegisterSpellScript<FreezeMadrigosa>("spell_freeze_madrigosa");
    RegisterSpellScript<spell_break_ice>("spell_break_ice");
}

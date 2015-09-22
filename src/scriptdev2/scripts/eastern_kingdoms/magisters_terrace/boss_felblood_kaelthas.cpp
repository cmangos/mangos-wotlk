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
SDName: Boss_Felblood_Kaelthas
SD%Complete: 90
SDComment: Minor adjustments required; Timers.
SDCategory: Magisters' Terrace
EndScriptData */

#include "precompiled.h"
#include "magisters_terrace.h"

enum
{
    SAY_INTRO_1                 = -1585023,                 // This yell should be done when the room is cleared. For now, set it as a movelineofsight yell.
    SAY_INTRO_2                 = -1585030,
    SAY_PHOENIX                 = -1585024,
    SAY_FLAMESTRIKE             = -1585025,
    SAY_GRAVITY_LAPSE           = -1585026,
    SAY_TIRED                   = -1585027,
    SAY_RECAST_GRAVITY          = -1585028,
    SAY_DEATH                   = -1585029,

    // Phase 1 spells
    SPELL_FIREBALL              = 44189,                    // Deals 2700-3300 damage at current target
    SPELL_FIREBALL_H            = 46164,                    //       4950-6050
    SPELL_PHOENIX               = 44194,                    // Summons a phoenix
    SPELL_FLAME_STRIKE          = 44192,                    // Summons the trigger + animation (projectile)
    SPELL_SHOCK_BARRIER         = 46165,                    // Heroic only; 10k damage shield, followed by Pyroblast
    SPELL_PYROBLAST             = 36819,                    // Heroic only; 45-55k fire damage

    // Phase 2 spells
    SPELL_GRAVITY_LAPSE         = 44224,                    // Cast at the beginning of every Gravity Lapse
    SPELL_GRAVITY_LAPSE_VISUAL  = 44251,                    // Channeled; blue beam animation to every enemy in range - when removed the Gravity Lapse auras are removed from players
    SPELL_TELEPORT_CENTER       = 44218,                    // Teleport the boss in the center. Requires DB entry in spell_target_position.
    SPELL_GRAVITY_LAPSE_FLY     = 44227,                    // Hastens flyspeed and allows flying for 1 minute. Requires aura stacking exception for 44226.
    SPELL_GRAVITY_LAPSE_DOT     = 44226,                    // Knocks up in the air and applies a 300 DPS DoT.
    SPELL_ARCANE_SPHERE_SUMMON  = 44265,                    // Summons 1 arcane sphere
    SPELL_POWER_FEEDBACK        = 44233,                    // Stuns him, making him take 50% more damage for 10 seconds. Cast after Gravity Lapse

    // Summoned spells
    SPELL_ARCANE_SPHERE_PASSIVE = 44263,                    // Passive auras on Arcane Spheres
    SPELL_FLAME_STRIKE_DUMMY    = 44191,                    // Flamestrike indicator before the damage
    SPELL_EMBER_BLAST           = 44199,                    // On Phoenix death
    SPELL_PHOENIX_BURN          = 44197,                    // A spell Phoenix uses to damage everything around
    SPELL_REBIRTH_DMG           = 44196,                    // DMG if a Phoenix rebirth happen

    // Summoned creatures
    NPC_FLAME_STRIKE_TRIGGER    = 24666,
    NPC_PHOENIX                 = 24674,
    NPC_PHOENIX_EGG             = 24675,
    NPC_ARCANE_SPHERE           = 24708,

    MAX_ARCANE_SPHERES          = 3,
};

static const DialogueEntry aIntroDialogue[] =
{
    {SAY_INTRO_1,         NPC_KAELTHAS, 16000},
    {EMOTE_ONESHOT_LAUGH, 0,            2000},
    {EMOTE_STATE_TALK,    0,            2000},
    {SAY_INTRO_2,         NPC_KAELTHAS, 16000},
    {NPC_PHOENIX,         0,            0},
    {SAY_DEATH,           NPC_KAELTHAS, 4000},
    {EMOTE_ONESHOT_POINT, 0,            5000},
    {EMOTE_ONESHOT_ROAR,  0,            3000},
    {NPC_PHOENIX_EGG,     0,            0},
    {0, 0, 0},
};

// Spells used to teleport players for Gravity Lapse
static const uint32 aGravityLapseSpells[] = {44219, 44220, 44221, 44222, 44223};

/*######
## boss_felblood_kaelthas
######*/

struct boss_felblood_kaelthasAI : public ScriptedAI, private DialogueHelper
{
    boss_felblood_kaelthasAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aIntroDialogue)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        InitializeDialogueHelper(m_pInstance);
        m_bHasTaunted = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiFireballTimer;
    uint32 m_uiPhoenixTimer;
    uint32 m_uiFlameStrikeTimer;

    // Heroic only
    uint32 m_uiShockBarrierTimer;
    uint32 m_uiPyroblastTimer;

    uint32 m_uiGravityLapseTimer;
    uint32 m_uiGravityLapseStage;
    uint8 m_uiGravityIndex;

    bool m_bIsFirstPhase;
    bool m_bFirstGravityLapse;
    bool m_bHasTaunted;

    void Reset() override
    {
        m_uiFireballTimer       = 0;
        m_uiPhoenixTimer        = 10000;
        m_uiFlameStrikeTimer    = 25000;

        m_uiPyroblastTimer      = 0;
        m_uiShockBarrierTimer   = 60000;

        m_uiGravityLapseTimer   = 1000;
        m_uiGravityLapseStage   = 0;
        m_uiGravityIndex        = 0;

        m_bFirstGravityLapse    = true;
        m_bIsFirstPhase         = true;

        SetCombatMovement(true);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KAELTHAS, DONE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KAELTHAS, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KAELTHAS, FAIL);
    }

    // Boss has an interesting speech before killed, so we need to fake death (without stand state) and allow him to finish his theatre
    void DamageTaken(Unit* /*pKiller*/, uint32& uiDamage) override
    {
        if (uiDamage < m_creature->GetHealth())
            return;

        // Make sure it won't die by accident
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
        {
            uiDamage = 0;
            return;
        }

        uiDamage = 0;
        RemoveGravityLapse();
        StartNextDialogueText(SAY_DEATH);
        m_creature->HandleEmote(EMOTE_STATE_TALK);

        m_creature->InterruptNonMeleeSpells(true);
        m_creature->SetHealth(1);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->ClearAllReactives();
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasTaunted && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster() &&
                m_creature->IsWithinDistInMap(pWho, 40.0) && m_creature->IsWithinLOSInMap(pWho))
        {
            StartNextDialogueText(SAY_INTRO_1);
            m_creature->HandleEmote(EMOTE_STATE_TALK);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_bHasTaunted = true;
        }

        // Allow him to finish intro
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE))
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case EMOTE_ONESHOT_LAUGH:
                m_creature->HandleEmote(EMOTE_ONESHOT_LAUGH);
                break;
            case EMOTE_STATE_TALK:
                m_creature->HandleEmote(EMOTE_STATE_TALK);
                break;
            case NPC_PHOENIX:
                m_creature->HandleEmote(EMOTE_STATE_NONE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                break;
            case EMOTE_ONESHOT_POINT:
                m_creature->HandleEmote(EMOTE_ONESHOT_POINT);
                break;
            case EMOTE_ONESHOT_ROAR:
                m_creature->HandleEmote(EMOTE_ONESHOT_ROAR);
                break;
            case NPC_PHOENIX_EGG:
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                break;
        }
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho, 20.0f);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_FLAME_STRIKE_TRIGGER)
            pSummoned->CastSpell(pSummoned, SPELL_FLAME_STRIKE_DUMMY, false, NULL, NULL, m_creature->GetObjectGuid());
        else
        {
            // Attack or follow target
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (pSummoned->GetEntry() == NPC_ARCANE_SPHERE)
                    pSummoned->GetMotionMaster()->MoveFollow(pTarget, 0, 0);
                else
                    pSummoned->AI()->AttackStart(pTarget);
            }
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        // Handle Gravity Lapse on targets
        if (pSpell->Id == SPELL_GRAVITY_LAPSE && pTarget->GetTypeId() == TYPEID_PLAYER)
        {
            DoCastSpellIfCan(pTarget, aGravityLapseSpells[m_uiGravityIndex], CAST_TRIGGERED);
            pTarget->CastSpell(pTarget, SPELL_GRAVITY_LAPSE_FLY, true, 0, 0, m_creature->GetObjectGuid());
            pTarget->CastSpell(pTarget, SPELL_GRAVITY_LAPSE_DOT, true, 0, 0, m_creature->GetObjectGuid());
            ++m_uiGravityIndex;
        }
    }

    // Wrapper to remove Gravity Lapse - this should be removed on aura 44251 expires
    void RemoveGravityLapse()
    {
        GuidVector vGuids;
        m_creature->FillGuidsListFromThreatList(vGuids);

        for (GuidVector::const_iterator itr = vGuids.begin(); itr != vGuids.end(); ++itr)
        {
            Unit* pUnit = m_creature->GetMap()->GetUnit(*itr);

            if (pUnit && pUnit->GetTypeId() == TYPEID_PLAYER)
            {
                pUnit->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_FLY);
                pUnit->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_DOT);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Don't use spells during the epilogue
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
            return;

        if (m_bIsFirstPhase)
        {
            // *Heroic mode only:
            if (!m_bIsRegularMode)
            {
                if (m_uiShockBarrierTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SHOCK_BARRIER) == CAST_OK)
                    {
                        m_uiPyroblastTimer = 1000;
                        m_uiShockBarrierTimer = 60000;
                    }
                }
                else
                    m_uiShockBarrierTimer -= uiDiff;

                if (m_uiPyroblastTimer)
                {
                    if (m_uiPyroblastTimer <= uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_PYROBLAST) == CAST_OK)
                            m_uiPyroblastTimer = 0;
                    }
                    else
                        m_uiPyroblastTimer -= uiDiff;
                }
            }

            if (m_uiFireballTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_FIREBALL : SPELL_FIREBALL_H) == CAST_OK)
                        m_uiFireballTimer = urand(2000, 4000);
                }
            }
            else
                m_uiFireballTimer -= uiDiff;

            if (m_uiPhoenixTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_PHOENIX) == CAST_OK)
                {
                    DoScriptText(SAY_PHOENIX, m_creature);
                    m_uiPhoenixTimer = 45000;
                }
            }
            else
                m_uiPhoenixTimer -= uiDiff;

            if (m_uiFlameStrikeTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_FLAME_STRIKE) == CAST_OK)
                    {
                        DoScriptText(SAY_FLAMESTRIKE, m_creature);
                        m_uiFlameStrikeTimer = urand(15000, 25000);
                    }
                }
            }
            else
                m_uiFlameStrikeTimer -= uiDiff;

            // Below 50%
            if (m_creature->GetHealthPercent() < 50.0f)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_TELEPORT_CENTER, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                {
                    SetCombatMovement(false);
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MoveIdle();

                    m_bIsFirstPhase = false;
                }
            }

            DoMeleeAttackIfReady();
        }
        else
        {
            if (m_uiGravityLapseTimer < uiDiff)
            {
                switch (m_uiGravityLapseStage)
                {
                    case 0:
                        // Cast Gravity Lapse on Players
                        if (DoCastSpellIfCan(m_creature, SPELL_GRAVITY_LAPSE) == CAST_OK)
                        {
                            if (m_bFirstGravityLapse)
                            {
                                DoScriptText(SAY_GRAVITY_LAPSE, m_creature);
                                m_bFirstGravityLapse = false;
                            }
                            else
                                DoScriptText(SAY_RECAST_GRAVITY, m_creature);

                            m_uiGravityLapseTimer = 2000;
                            m_uiGravityIndex = 0;
                            ++m_uiGravityLapseStage;
                        }
                        break;
                    case 1:
                        // Summon spheres and apply the Gravity Lapse visual - upon visual expire, the gravity lapse is removed
                        if (DoCastSpellIfCan(m_creature, SPELL_GRAVITY_LAPSE_VISUAL) == CAST_OK)
                        {
                            for (uint8 i = 0; i < MAX_ARCANE_SPHERES; ++i)
                                DoCastSpellIfCan(m_creature, SPELL_ARCANE_SPHERE_SUMMON, CAST_TRIGGERED);

                            m_uiGravityLapseTimer = 30000;
                            ++m_uiGravityLapseStage;
                        }
                        break;
                    case 2:
                        // Cast Power Feedback and stay stunned for 10 secs - also break the statues if they are not broken
                        if (DoCastSpellIfCan(m_creature, SPELL_POWER_FEEDBACK) == CAST_OK)
                        {
                            DoScriptText(SAY_TIRED, m_creature);
                            RemoveGravityLapse();
                            m_uiGravityLapseTimer = 10000;
                            m_uiGravityLapseStage = 0;
                        }
                        break;
                }
            }
            else
                m_uiGravityLapseTimer -= uiDiff;
        }
    }
};

/*######
## mob_felkael_phoenix
######*/

struct mob_felkael_phoenixAI : public ScriptedAI
{
    mob_felkael_phoenixAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiBurnTimer;

    bool m_bFakeDeath;

    void Reset() override
    {
        m_uiBurnTimer = 2000;
        m_bFakeDeath = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_PHOENIX_BURN);
    }

    void EnterEvadeMode() override
    {
        // Don't evade during ember blast
        if (m_bFakeDeath)
            return;

        ScriptedAI::EnterEvadeMode();
    }

    void DamageTaken(Unit* /*pKiller*/, uint32& uiDamage) override
    {
        if (uiDamage < m_creature->GetHealth())
            return;

        // Prevent glitch if in fake death
        if (m_bFakeDeath)
        {
            uiDamage = 0;
            return;
        }

        // prevent death
        uiDamage = 0;
        DoSetFakeDeath();
    }

    void DoSetFakeDeath()
    {
        m_bFakeDeath = true;

        m_creature->InterruptNonMeleeSpells(false);
        m_creature->SetHealth(1);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->ClearAllReactives();
        m_creature->SetTargetGuid(ObjectGuid());
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        // Spawn egg and make invisible
        DoCastSpellIfCan(m_creature, SPELL_EMBER_BLAST, CAST_TRIGGERED);
        m_creature->SummonCreature(NPC_PHOENIX_EGG, 0, 0, 0, 0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 10000);
    }

    void SummonedCreatureDespawn(Creature* /*pSummoned*/) override
    {
        m_creature->RemoveAurasDueToSpell(SPELL_EMBER_BLAST);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        // Remove fake death on egg despawn after 10 secs
        if (DoCastSpellIfCan(m_creature, SPELL_REBIRTH_DMG) == CAST_OK)
        {
            m_creature->SetHealth(m_creature->GetMaxHealth());
            m_creature->GetMotionMaster()->Clear();
            DoStartMovement(m_creature->getVictim());
            m_bFakeDeath = false;

            DoCastSpellIfCan(m_creature, SPELL_PHOENIX_BURN, CAST_TRIGGERED);
        }
    }

    void SummonedCreatureJustDied(Creature* /*pSummoned*/) override
    {
        // Self kill if the egg is killed
        if (m_bFakeDeath)
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bFakeDeath)
            return;

        // ToDo: research if this is correct and how can this be done by spell
        if (m_uiBurnTimer < uiDiff)
        {
            // spell Burn should possible do this, but it doesn't, so do this for now.
            uint32 uiDmg = urand(1650, 2050);
            if (uiDmg > m_creature->GetHealth())
                DoSetFakeDeath();
            else
                m_creature->DealDamage(m_creature, uiDmg, 0, DOT, SPELL_SCHOOL_MASK_FIRE, NULL, false);

            m_uiBurnTimer = 2000;
        }
        else
            m_uiBurnTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## mob_felkael_phoenix_egg
######*/

// TODO Remove this 'script' when combat movement can be proper prevented from core-side
struct mob_felkael_phoenix_eggAI : public Scripted_NoMovementAI
{
    mob_felkael_phoenix_eggAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override {}
    void MoveInLineOfSight(Unit* /*pWho*/) override {}
    void AttackStart(Unit* /*pWho*/) override {}
    void UpdateAI(const uint32 /*uiDiff*/) override {}
};

/*######
## mob_arcane_sphere
######*/

struct mob_arcane_sphereAI : public ScriptedAI
{
    mob_arcane_sphereAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiDespawnTimer;
    uint32 m_uiChangeTargetTimer;

    void Reset() override
    {
        m_uiDespawnTimer      = 30000;
        m_uiChangeTargetTimer = urand(6000, 12000);

        DoCastSpellIfCan(m_creature, SPELL_ARCANE_SPHERE_PASSIVE);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Should despawn when aura 44251 expires
        if (m_uiDespawnTimer < uiDiff)
        {
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            m_uiDespawnTimer = 0;
        }
        else
            m_uiDespawnTimer -= uiDiff;

        if (m_uiChangeTargetTimer < uiDiff)
        {
            if (!m_pInstance)
                return;

            // Follow the target - do not attack
            if (Creature* pKael = m_pInstance->GetSingleCreatureFromStorage(NPC_KAELTHAS))
            {
                if (Unit* pTarget = pKael->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    m_creature->GetMotionMaster()->MoveFollow(pTarget, 0, 0);
            }

            m_uiChangeTargetTimer = urand(5000, 15000);
        }
        else
            m_uiChangeTargetTimer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_felblood_kaelthas(Creature* pCreature)
{
    return new boss_felblood_kaelthasAI(pCreature);
}

CreatureAI* GetAI_mob_arcane_sphere(Creature* pCreature)
{
    return new mob_arcane_sphereAI(pCreature);
}

CreatureAI* GetAI_mob_felkael_phoenix(Creature* pCreature)
{
    return new mob_felkael_phoenixAI(pCreature);
}

CreatureAI* GetAI_mob_felkael_phoenix_egg(Creature* pCreature)
{
    return new mob_felkael_phoenix_eggAI(pCreature);
}

void AddSC_boss_felblood_kaelthas()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_felblood_kaelthas";
    pNewScript->GetAI = &GetAI_boss_felblood_kaelthas;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_arcane_sphere";
    pNewScript->GetAI = &GetAI_mob_arcane_sphere;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_felkael_phoenix";
    pNewScript->GetAI = &GetAI_mob_felkael_phoenix;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_felkael_phoenix_egg";
    pNewScript->GetAI = &GetAI_mob_felkael_phoenix_egg;
    pNewScript->RegisterSelf();
}

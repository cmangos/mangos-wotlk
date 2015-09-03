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

#include "precompiled.h"
#include "sunwell_plateau.h"

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
    YELL_KILL1                      = -1580023,
    YELL_KILL2                      = -1580024,
    YELL_KILL3                      = -1580025,
    YELL_LOVE1                      = -1580026,
    YELL_LOVE2                      = -1580027,
    YELL_LOVE3                      = -1580028,
    YELL_BERSERK                    = -1580029,
    YELL_DEATH                      = -1580030,

    SPELL_METEOR_SLASH              = 45150,
    SPELL_BURN                      = 45141,
    SPELL_STOMP                     = 45185,
    SPELL_BERSERK                   = 26662,
    SPELL_SUMMON_DEATH_CLOUD        = 45884,                // Summoned on death

    // Epilogue spells
    SPELL_BRUTALLUS_DEATH_CLOUD     = 45212,
    SPELL_FELBLAZE_PREVIZUAL        = 44885,
    SPELL_SUMMON_FELBLAZE           = 45069,

    NPC_BRUTALLUS_DEATH_CLOUD       = 25703,

    // spells used during the intro event
    SPELL_FROST_BLAST               = 45203,                // Madrigosa's spells
    SPELL_FREEZE                    = 46609,                // Activates the ice barrier - script effect for 46610
    SPELL_FROSTBOLT                 = 44843,
    SPELL_FROST_BREATH              = 45065,
    SPELL_ENCAPSULATE               = 44883,
    SPELL_FEL_FIREBALL              = 44844,                // Brutallus' spells
    SPELL_CLEAR_DEBUFFS             = 34098,
    SPELL_FLAME_RING                = 44874,                // this spell should have a fire explosion when removed
    SPELL_CHARGE                    = 44884,
    SPELL_BREAK_ICE                 = 46637,                // Break the ice, open the door - dummy spell for 46638 and 47030

    POINT_MOVE_GROUND               = 1,
    POINT_MOVE_ICE_BLOCK            = 2,
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
    {YELL_MADR_TRAP,            NPC_MADRIGOSA,  14000},
    {YELL_INTRO_CHARGE,         NPC_BRUTALLUS,  10000},
    {YELL_INTRO_KILL_MADRIGOSA, NPC_BRUTALLUS,  8000},
    {YELL_INTRO_TAUNT,          NPC_BRUTALLUS,  0},
    {0, 0, 0},
};

/*######
## boss_brutallus
######*/

struct boss_brutallusAI : public ScriptedAI, private DialogueHelper
{
    boss_brutallusAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aIntroDialogue)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        InitializeDialogueHelper(m_pInstance);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiSlashTimer;
    uint32 m_uiBurnTimer;
    uint32 m_uiStompTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiLoveTimer;

    uint32 m_uiMadrigosaSpellTimer;

    bool m_bCanDoMeleeAttack;
    bool m_bIsIntroInProgress;

    void Reset() override
    {
        m_uiSlashTimer      = 11000;
        m_uiStompTimer      = 30000;
        m_uiBurnTimer       = 20000;
        m_uiBerserkTimer    = 6 * MINUTE * IN_MILLISECONDS;
        m_uiLoveTimer       = urand(10000, 17000);

        m_uiMadrigosaSpellTimer = 0;

        m_bCanDoMeleeAttack = true;
        m_bIsIntroInProgress = false;
    }

    void Aggro(Unit* pWho) override
    {
        // Don't aggro when attacking Madrigosa
        if (pWho->GetEntry() == NPC_MADRIGOSA)
            return;

        DoScriptText(YELL_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BRUTALLUS, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim) override
    {
        // Don't yell for Madrigosa
        if (pVictim->GetEntry() == NPC_MADRIGOSA)
            return;

        switch (urand(0, 2))
        {
            case 0: DoScriptText(YELL_KILL1, m_creature); break;
            case 1: DoScriptText(YELL_KILL2, m_creature); break;
            case 2: DoScriptText(YELL_KILL3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(YELL_DEATH, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_DEATH_CLOUD, CAST_TRIGGERED);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BRUTALLUS, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
        {
            // When evade from the fight with Madrigosa skip this
            if (m_pInstance->GetData(TYPE_BRUTALLUS) == SPECIAL)
                return;

            m_pInstance->SetData(TYPE_BRUTALLUS, FAIL);
        }
    }

    void GetAIInformation(ChatHandler& reader) override
    {
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_BRUTALLUS) == SPECIAL)
                reader.PSendSysMessage("Brutallus intro event is currently %s", m_bIsIntroInProgress ? "in progress" : "completed");
            else
                reader.PSendSysMessage("Brutallus intro event is currently %s", m_pInstance->GetData(TYPE_BRUTALLUS) == NOT_STARTED ? "not started" : "completed");

            if (m_pInstance->GetData(TYPE_BRUTALLUS) != NOT_STARTED)
            {
                if (Creature* pMadrigosa = m_pInstance->GetSingleCreatureFromStorage(NPC_MADRIGOSA, true))
                    reader.PSendSysMessage("Madrigosa guid is %s and has %u health.", pMadrigosa->GetObjectGuid().GetString().c_str(), pMadrigosa->GetHealth());
            }
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        // Error log if Madrigosa dies
        if (pSummoned->GetEntry() == NPC_MADRIGOSA)
            script_error_log("Npc %u, %s, died unexpectedly. Felmyst won't be summoned anymore.", pSummoned->GetEntry(), pSummoned->GetName());
    }

    void SummonedCreatureDespawn(Creature* pSummoned) override
    {
        // Yell of Madrigosa on death
        if (pSummoned->GetEntry() == NPC_MADRIGOSA)
            pSummoned->CastSpell(pSummoned, SPELL_SUMMON_FELBLAZE, true);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_MADRIGOSA)
        {
            pSummoned->SetWalk(false);
            pSummoned->SetLevitate(true);
            pSummoned->GetMotionMaster()->MovePoint(0, aMadrigosaLoc[1].m_fX, aMadrigosaLoc[1].m_fY, aMadrigosaLoc[1].m_fZ, false);
        }
        else if (pSummoned->GetEntry() == NPC_BRUTALLUS_DEATH_CLOUD)
            pSummoned->CastSpell(pSummoned, SPELL_BRUTALLUS_DEATH_CLOUD, true);
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || pSummoned->GetEntry() != NPC_MADRIGOSA)
            return;

        if (uiPointId == POINT_MOVE_GROUND)
            pSummoned->SetLevitate(false);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        // Fake death Madrigosa when charged
        if (pTarget->GetEntry() == NPC_MADRIGOSA && pSpell->Id == SPELL_CHARGE)
        {
            DoScriptText(YELL_MADR_DEATH, pTarget);
            pTarget->InterruptNonMeleeSpells(true);
            pTarget->SetHealth(0);
            pTarget->StopMoving();
            pTarget->ClearComboPointHolders();
            pTarget->RemoveAllAurasOnDeath();
            pTarget->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
            pTarget->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
            pTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pTarget->ClearAllReactives();
            pTarget->GetMotionMaster()->Clear();
            pTarget->GetMotionMaster()->MoveIdle();
            pTarget->SetStandState(UNIT_STAND_STATE_DEAD);

            // Brutallus evades
            EnterEvadeMode();
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        if (!m_pInstance)
            return;

        switch (iEntry)
        {
            case NPC_MADRIGOSA:
                if (Creature* pTrigger = m_pInstance->GetSingleCreatureFromStorage(NPC_FLIGHT_TRIGGER_LEFT))
                    m_creature->SummonCreature(NPC_MADRIGOSA, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ(), 0, TEMPSUMMON_DEAD_DESPAWN, 0);
                m_bIsIntroInProgress = true;
                break;
            case YELL_MADR_ICE_BARRIER:
                if (Creature* pMadrigosa = m_pInstance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                    pMadrigosa->CastSpell(pMadrigosa, SPELL_FREEZE, false);
                break;
            case YELL_MADR_INTRO:
                if (Creature* pMadrigosa = m_pInstance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                    pMadrigosa->GetMotionMaster()->MovePoint(POINT_MOVE_GROUND, aMadrigosaLoc[0].m_fX, aMadrigosaLoc[0].m_fY, aMadrigosaLoc[0].m_fZ);
                break;
            case YELL_INTRO:
                if (Creature* pMadrigosa = m_pInstance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                    m_creature->AI()->AttackStart(pMadrigosa);
                break;
            case SPELL_FROST_BREATH:
                if (Creature* pMadrigosa = m_pInstance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                {
                    pMadrigosa->CastSpell(m_creature, SPELL_FROST_BREATH, false);
                    pMadrigosa->GetMotionMaster()->MoveIdle();
                }
                break;
            case POINT_MOVE_ICE_BLOCK:
                m_bCanDoMeleeAttack = false;
                if (Creature* pMadrigosa = m_pInstance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                {
                    pMadrigosa->GetMotionMaster()->MovePoint(POINT_MOVE_ICE_BLOCK, aMadrigosaLoc[1].m_fX, aMadrigosaLoc[1].m_fY, aMadrigosaLoc[1].m_fZ);
                    pMadrigosa->HandleEmote(EMOTE_ONESHOT_LIFTOFF);
                    pMadrigosa->SetLevitate(true);
                }
                // Temporary! This will make Brutallus not follow Madrigosa through the air until mmaps are implemented
                m_creature->GetMotionMaster()->MoveIdle();
                break;
            case YELL_MADR_ICE_BLOCK:
                if (Creature* pMadrigosa = m_pInstance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                    pMadrigosa->CastSpell(m_creature, SPELL_FROST_BLAST, true);
                m_uiMadrigosaSpellTimer = 2000;
                break;
            case SPELL_FLAME_RING:
                DoCastSpellIfCan(m_creature, SPELL_CLEAR_DEBUFFS, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_FLAME_RING, CAST_TRIGGERED);
                break;
            case YELL_INTRO_BREAK_ICE:
                m_creature->RemoveAurasDueToSpell(SPELL_FLAME_RING);
                break;
            case SPELL_FEL_FIREBALL:
                // Spell has script target Madrigosa
                DoCastSpellIfCan(m_creature, SPELL_FEL_FIREBALL);
                break;
            case POINT_MOVE_GROUND:
                if (Creature* pMadrigosa = m_pInstance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                    pMadrigosa->GetMotionMaster()->MovePoint(POINT_MOVE_GROUND, aMadrigosaLoc[0].m_fX, aMadrigosaLoc[0].m_fY, aMadrigosaLoc[0].m_fZ);
                m_uiMadrigosaSpellTimer = 0;
                break;
            case YELL_MADR_TRAP:
                if (Creature* pMadrigosa = m_pInstance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                {
                    pMadrigosa->CastSpell(m_creature, SPELL_ENCAPSULATE, true);
                    // Need to remove the fire aura after 4 sec so Madrigosa won't die so soon
                    pMadrigosa->RemoveAurasDueToSpell(SPELL_FEL_FIREBALL);
                }
                break;
            case YELL_INTRO_CHARGE:
                m_bCanDoMeleeAttack = true;
                if (m_creature->getVictim())
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                DoCastSpellIfCan(m_creature, SPELL_CHARGE);
                break;
            case YELL_INTRO_KILL_MADRIGOSA:
                // Face the players
                if (GameObject* pIceWall = m_pInstance->GetSingleGameObjectFromStorage(GO_ICE_BARRIER))
                    m_creature->SetFacingToObject(pIceWall);
                break;
            case YELL_INTRO_TAUNT:
                DoCastSpellIfCan(m_creature, SPELL_BREAK_ICE);
                m_bIsIntroInProgress = false;
                break;
        }
    }

    // Wrapper to start the dialogue text
    void DoStartIntro()
    {
        StartNextDialogueText(NPC_MADRIGOSA);
    }

    // Wrapper to keep all the intro event stuff together
    void UpdateIntroEvent(const uint32 uiDiff)
    {
        // Dialogue updates outside of combat too
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiMadrigosaSpellTimer)
        {
            if (m_uiMadrigosaSpellTimer <= uiDiff)
            {
                if (Creature* pMadrigosa = m_pInstance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                    pMadrigosa->CastSpell(m_creature, SPELL_FROSTBOLT, true);
                m_uiMadrigosaSpellTimer = urand(1000, 2000);
            }
            else
                m_uiMadrigosaSpellTimer -= uiDiff;
        }

        // We need to limit the melee attacks for the intro event
        if (m_bCanDoMeleeAttack)
            DoMeleeAttackIfReady();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Update only the intro related stuff
        if (m_pInstance && m_pInstance->GetData(TYPE_BRUTALLUS) == SPECIAL)
        {
            UpdateIntroEvent(uiDiff);
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiLoveTimer < uiDiff)
        {
            switch (urand(0, 2))
            {
                case 0: DoScriptText(YELL_LOVE1, m_creature); break;
                case 1: DoScriptText(YELL_LOVE2, m_creature); break;
                case 2: DoScriptText(YELL_LOVE3, m_creature); break;
            }
            m_uiLoveTimer = urand(15000, 23000);
        }
        else
            m_uiLoveTimer -= uiDiff;

        if (m_uiSlashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_METEOR_SLASH) == CAST_OK)
                m_uiSlashTimer = 11000;
        }
        else
            m_uiSlashTimer -= uiDiff;

        if (m_uiStompTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_STOMP) == CAST_OK)
                m_uiStompTimer = 30000;
        }
        else
            m_uiStompTimer -= uiDiff;

        if (m_uiBurnTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_BURN, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_BURN) == CAST_OK)
                    m_uiBurnTimer = 20000;
            }
        }
        else
            m_uiBurnTimer -= uiDiff;

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(YELL_BERSERK, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_brutallus(Creature* pCreature)
{
    return new boss_brutallusAI(pCreature);
}

/*######
## spell_aura_dummy_npc_brutallus_cloud
######*/

bool EffectAuraDummy_spell_aura_dummy_npc_brutallus_cloud(const Aura* pAura, bool bApply)
{
    // On Aura removal start Felmyst summon visuals
    if (pAura->GetId() == SPELL_BRUTALLUS_DEATH_CLOUD && pAura->GetEffIndex() == EFFECT_INDEX_0 && !bApply)
    {
        if (Creature* pTarget = (Creature*)pAura->GetTarget())
        {
            if (ScriptedInstance* pInstance = (ScriptedInstance*)pTarget->GetInstanceData())
            {
                if (Creature* pMadrigosa = pInstance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
                {
                    // Set respawn pos to current pos
                    pMadrigosa->SetRespawnCoord(pMadrigosa->GetPositionX(), pMadrigosa->GetPositionY(), pMadrigosa->GetPositionZ(), pMadrigosa->GetOrientation());

                    pMadrigosa->CastSpell(pMadrigosa, SPELL_FELBLAZE_PREVIZUAL, true);
                    pMadrigosa->ForcedDespawn(10000);
                }
            }
        }
    }
    return true;
}

/*######
## at_madrigosa
######*/

bool AreaTrigger_at_madrigosa(Player* pPlayer, AreaTriggerEntry const* /*pAt*/)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData())
    {
        // this simply set encounter state, and trigger ice barrier become active
        // bosses can start pre-event based on this new state
        if (pInstance->GetData(TYPE_BRUTALLUS) == NOT_STARTED)
        {
            pInstance->SetData(TYPE_BRUTALLUS, SPECIAL);

            // Start the intro event
            if (Creature* pBrutallus = pInstance->GetSingleCreatureFromStorage(NPC_BRUTALLUS))
            {
                if (boss_brutallusAI* pBossAI = dynamic_cast<boss_brutallusAI*>(pBrutallus->AI()))
                    pBossAI->DoStartIntro();
            }
        }
    }

    return false;
}

void AddSC_boss_brutallus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_brutallus";
    pNewScript->GetAI = &GetAI_boss_brutallus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "spell_dummy_npc_brutallus_cloud";
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_spell_aura_dummy_npc_brutallus_cloud;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_madrigosa";
    pNewScript->pAreaTrigger = &AreaTrigger_at_madrigosa;
    pNewScript->RegisterSelf();
}

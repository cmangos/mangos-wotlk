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
SDName: Bosses_Opera
SD%Complete: 95
SDComment: Oz, Hood, and RAJ event implemented. Spell timers may need adjustments.
Romulo's Poisoned Thrust will now be used less frequently. - Patch_2.1.0
SDCategory: Karazhan
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "karazhan.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

/***********************************/
/*** OPERA WIZARD OF OZ EVENT *****/
/*********************************/

enum
{
    SAY_DOROTHEE_DEATH          = -1532025,
    SAY_DOROTHEE_SUMMON         = -1532026,
    SAY_DOROTHEE_TITO_DEATH     = -1532027,
    SAY_DOROTHEE_AGGRO          = -1532028,

    SAY_ROAR_AGGRO              = -1532029,
    SAY_ROAR_DEATH              = -1532030,
    SAY_ROAR_SLAY               = -1532031,

    SAY_STRAWMAN_AGGRO          = -1532032,
    SAY_STRAWMAN_DEATH          = -1532033,
    SAY_STRAWMAN_SLAY           = -1532034,

    SAY_TINHEAD_AGGRO           = -1532035,
    SAY_TINHEAD_DEATH           = -1532036,
    SAY_TINHEAD_SLAY            = -1532037,
    EMOTE_RUST                  = -1532038,

    SAY_CRONE_INTRO             = -1532039,
    SAY_CRONE_INTRO2            = -1532040,
    SAY_CRONE_DEATH             = -1532041,
    SAY_CRONE_SLAY              = -1532042,

    /**** Spells ****/
    // Dorothee
    SPELL_WATERBOLT             = 31012,
    SPELL_FRIGHTENEDSCREAM      = 31013,
    SPELL_SUMMONTITO            = 31014,

    // Strawman
    SPELL_BRAIN_BASH            = 31046,
    SPELL_BRAIN_WIPE            = 31069,
    SPELL_CONFLAG_PROC          = 31073,            // procs 31075 on fire damage

    // Tinhead
    SPELL_CLEAVE                = 31043,
    SPELL_RUST                  = 31086,

    // Roar
    SPELL_MANGLE                = 31041,
    SPELL_SHRED                 = 31042,
    SPELL_FRIGHTENED_SCREAM     = 31013,

    // Crone
    SPELL_CHAIN_LIGHTNING       = 32337,
    SPELL_FIERY_BROOM_PROC      = 32339,            // passive spell causing periodic damage

    // Cyclone
    SPELL_CYCLONE               = 32334,
    SPELL_CYCLONE_VISUAL        = 32332,

    /** Creature Entries **/
    NPC_TITO                    = 17548,
    NPC_CYCLONE                 = 18412,
};

enum DorotheeActions // order based on priority
{
    DOROTHEE_ACTION_SUMMONTITO,
    DOROTHEE_ACTION_FRIGHTENEDSCREAM,
    DOROTHEE_ACTION_WATERBOLT,
    DOROTHEE_ACTION_MAX,
    DOROTHEE_INTRO,
    DOROTHEE_AGGRO,
};

struct boss_dorotheeAI : public CombatAI
{
    boss_dorotheeAI(Creature* creature) : CombatAI(creature, DOROTHEE_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        SetReactState(REACT_PASSIVE);
        AddCombatAction(DOROTHEE_ACTION_SUMMONTITO, GetInitialActionTimer(DOROTHEE_ACTION_SUMMONTITO));
        AddCombatAction(DOROTHEE_ACTION_FRIGHTENEDSCREAM, GetInitialActionTimer(DOROTHEE_ACTION_FRIGHTENEDSCREAM));
        AddCombatAction(DOROTHEE_ACTION_WATERBOLT, GetInitialActionTimer(DOROTHEE_ACTION_WATERBOLT));
        AddCustomAction(DOROTHEE_INTRO, 2000u, [&]() { HandleIntro(); });
        AddCustomAction(DOROTHEE_AGGRO, 12000u, [&]() { HandleAggro(); });
        SetRangedMode(true, 40.f, TYPE_FULL_CASTER);
    }

    ScriptedInstance* m_instance;

    void Reset() override
    {
        CombatAI::Reset();

        SetCombatMovement(true);
        SetCombatScriptStatus(false);
    }

    uint32 GetInitialActionTimer(DorotheeActions id)
    {
        switch (id)
        {
            case DOROTHEE_ACTION_SUMMONTITO: return 36500;
            case DOROTHEE_ACTION_FRIGHTENEDSCREAM: return urand(12000, 15000);
            case DOROTHEE_ACTION_WATERBOLT: return 0;
            default: return 0;
        }
    }

    uint32 GetSubsequentActionTimer(DorotheeActions id)
    {
        switch (id)
        {
            case DOROTHEE_ACTION_FRIGHTENEDSCREAM: return urand(18000, 30000);
            case DOROTHEE_ACTION_WATERBOLT: return 2400;
            default: return 0;
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DOROTHEE_DEATH, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (m_creature->GetVictim())
            pSummoned->AI()->AttackStart(m_creature->GetVictim());
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_TITO)
            DoScriptText(SAY_DOROTHEE_TITO_DEATH, m_creature);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case DOROTHEE_ACTION_WATERBOLT:
            {
                if (DoCastSpellIfCan(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER), SPELL_WATERBOLT) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(DorotheeActions(action)));
                break;
            }
            case DOROTHEE_ACTION_FRIGHTENEDSCREAM:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, SPELL_FRIGHTENEDSCREAM, SELECT_FLAG_PLAYER | SELECT_FLAG_USE_EFFECT_RADIUS))
                    if (DoCastSpellIfCan(nullptr, SPELL_FRIGHTENEDSCREAM) == CAST_OK)
                        ResetCombatAction(action, GetSubsequentActionTimer(DorotheeActions(action)));
                break;
            }
            case DOROTHEE_ACTION_SUMMONTITO:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMONTITO) == CAST_OK)
                {
                    DoScriptText(SAY_DOROTHEE_SUMMON, m_creature);
                    DisableCombatAction(action);
                }
                break;
            }
        }
    };

    void HandleIntro()
    {
        DoScriptText(SAY_DOROTHEE_AGGRO, m_creature);
    }

    void HandleAggro()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        SetReactState(REACT_AGGRESSIVE);
        m_creature->SetInCombatWithZone();
        AttackClosestEnemy();
        if (!m_creature->IsInCombat())
            JustReachedHome();
    }
};

struct boss_strawmanAI : public ScriptedAI
{
    boss_strawmanAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    ScriptedInstance* m_instance;

    uint32 m_uiAggroTimer;
    uint32 m_uiBrainBashTimer;
    uint32 m_uiBrainWipeTimer;

    void Reset() override
    {
        m_uiAggroTimer     = 27000;
        m_uiBrainBashTimer = 5000;
        m_uiBrainWipeTimer = 7000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_CONFLAG_PROC);
        DoScriptText(SAY_STRAWMAN_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_STRAWMAN_DEATH, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_STRAWMAN_SLAY, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiAggroTimer)
        {
            if (m_uiAggroTimer <= uiDiff)
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                SetReactState(REACT_AGGRESSIVE);
                m_creature->SetInCombatWithZone();
                AttackClosestEnemy();
                if (!m_creature->IsInCombat())
                    JustReachedHome();
                m_uiAggroTimer = 0;
            }
            else
                m_uiAggroTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiBrainBashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_BRAIN_BASH) == CAST_OK)
                m_uiBrainBashTimer = 15000;
        }
        else
            m_uiBrainBashTimer -= uiDiff;

        if (m_uiBrainWipeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_BRAIN_WIPE) == CAST_OK)
                    m_uiBrainWipeTimer = 20000;
            }
        }
        else
            m_uiBrainWipeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct boss_tinheadAI : public ScriptedAI
{
    boss_tinheadAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    ScriptedInstance* m_instance;

    uint32 m_uiAggroTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiRustTimer;

    void Reset() override
    {
        m_uiAggroTimer  = 37000;
        m_uiCleaveTimer = 5000;
        m_uiRustTimer   = 6000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_TINHEAD_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_TINHEAD_DEATH, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_TINHEAD_SLAY, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiAggroTimer)
        {
            if (m_uiAggroTimer <= uiDiff)
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                SetReactState(REACT_AGGRESSIVE);
                m_creature->SetInCombatWithZone();
                AttackClosestEnemy();
                if (!m_creature->IsInCombat())
                    JustReachedHome();
                m_uiAggroTimer = 0;
            }
            else
                m_uiAggroTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = 5000;
        }
        else
            m_uiCleaveTimer -= uiDiff;

        if (m_uiRustTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_RUST) == CAST_OK)
            {
                DoScriptText(EMOTE_RUST, m_creature);
                m_uiRustTimer = 6000;
            }
        }
        else
            m_uiRustTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct boss_roarAI : public ScriptedAI
{
    boss_roarAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    ScriptedInstance* m_instance;

    uint32 m_uiAggroTimer;
    uint32 m_uiMangleTimer;
    uint32 m_uiShredTimer;
    uint32 m_uiScreamTimer;

    void Reset() override
    {
        m_uiAggroTimer  = 17000;
        m_uiMangleTimer = 5000;
        m_uiShredTimer  = 10000;
        m_uiScreamTimer = 15000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_ROAR_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_ROAR_DEATH, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_ROAR_SLAY, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiAggroTimer)
        {
            if (m_uiAggroTimer <= uiDiff)
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                SetReactState(REACT_AGGRESSIVE);
                m_creature->SetInCombatWithZone();
                AttackClosestEnemy();
                if (!m_creature->IsInCombat())
                    JustReachedHome();
                m_uiAggroTimer = 0;
            }
            else
                m_uiAggroTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiMangleTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MANGLE) == CAST_OK)
                m_uiMangleTimer = urand(5000, 8000);
        }
        else
            m_uiMangleTimer -= uiDiff;

        if (m_uiShredTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHRED) == CAST_OK)
                m_uiShredTimer = urand(10000, 15000);
        }
        else
            m_uiShredTimer -= uiDiff;

        if (m_uiScreamTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FRIGHTENED_SCREAM) == CAST_OK)
                m_uiScreamTimer = urand(20000, 30000);
        }
        else
            m_uiScreamTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

static const float afCycloneSpawnLoc1[4] = { -10908.86f, -1773.627f, 90.55865f, 5.833215f };
static const float afCycloneSpawnLoc2[4] = { -10910.79f, -1771.201f, 90.56122f, 0.3642556f };
static const float afCycloneSpawnLoc3[4] = { -10907.68f, -1778.651f, 90.56018f, 2.44127f };

struct boss_croneAI : public ScriptedAI
{
    boss_croneAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        m_creature->CastSpell(m_creature, SPELL_FIERY_BROOM_PROC, TRIGGERED_OLD_TRIGGERED);
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    ScriptedInstance* m_instance;

    uint32 m_uiIntroTimer;
    uint32 m_uiChainLightningTimer;
    uint32 m_uiAggroTimer;

    void Reset() override
    {
        m_uiIntroTimer = 6000;
        m_uiChainLightningTimer = 10000;
        m_uiAggroTimer = 9000;
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        // spawn the cyclones on aggro
        if (Creature *cMainCyclone = m_creature->SummonCreature(NPC_CYCLONE, afCycloneSpawnLoc1[0], afCycloneSpawnLoc1[1], afCycloneSpawnLoc1[2], afCycloneSpawnLoc1[3], TEMPSPAWN_DEAD_DESPAWN, 0))
        {
            if (Creature *cFollowCyclone = m_creature->SummonCreature(NPC_CYCLONE, afCycloneSpawnLoc2[0], afCycloneSpawnLoc2[1], afCycloneSpawnLoc2[2], afCycloneSpawnLoc2[3], TEMPSPAWN_DEAD_DESPAWN, 0))
                cFollowCyclone->GetMotionMaster()->MoveFollow(cMainCyclone, cFollowCyclone->GetDistance(cMainCyclone), cFollowCyclone->GetAngle(cMainCyclone));
            if (Creature *cFollowCyclone = m_creature->SummonCreature(NPC_CYCLONE, afCycloneSpawnLoc3[0], afCycloneSpawnLoc3[1], afCycloneSpawnLoc3[2], afCycloneSpawnLoc3[3], TEMPSPAWN_DEAD_DESPAWN, 0))
                cFollowCyclone->GetMotionMaster()->MoveFollow(cMainCyclone, cFollowCyclone->GetDistance(cMainCyclone), cFollowCyclone->GetAngle(cMainCyclone));
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_CRONE_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_OPERA, DONE);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->CastSpell(pSummoned, SPELL_CYCLONE, TRIGGERED_OLD_TRIGGERED);
        pSummoned->CastSpell(pSummoned, SPELL_CYCLONE_VISUAL, TRIGGERED_OLD_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiIntroTimer)
        {
            if (m_uiIntroTimer <= uiDiff)
            {
                DoScriptText(urand(0, 1) ? SAY_CRONE_INTRO : SAY_CRONE_INTRO2, m_creature); // TODO: should be said at player who started event
                m_uiIntroTimer = 0;
            }
            else
                m_uiIntroTimer -= uiDiff;
        }

        if (m_uiAggroTimer)
        {
            if (m_uiAggroTimer <= uiDiff)
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                SetReactState(REACT_AGGRESSIVE);
                m_creature->SetInCombatWithZone();
                AttackClosestEnemy();
                if (!m_creature->IsInCombat())
                    JustReachedHome();
                m_uiAggroTimer = 0;
            }
            else
                m_uiAggroTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiChainLightningTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_CHAIN_LIGHTNING) == CAST_OK)
                    m_uiChainLightningTimer = 15000;
            }
        }
        else
            m_uiChainLightningTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/**************************************/
/**** Opera Red Riding Hood Event ****/
/************************************/

enum
{
    /**** Yells for the Wolf ****/
    SAY_WOLF_AGGRO                  = -1532043,
    SAY_WOLF_SLAY                   = -1532044,
    SAY_WOLF_HOOD                   = -1532045,
    SOUND_WOLF_DEATH                = 9275,                // Only sound on death, no text.

    /**** Spells For The Wolf ****/
    SPELL_PICK_RED_RIDING_HOOD      = 30769,               // targeting spell - triggers 30768
    SPELL_RED_RIDING_HOOD           = 30756,
    SPELL_TERRIFYING_HOWL           = 30752,
    SPELL_WIDE_SWIPE                = 30761,

    GOSSIP_ITEM_GRANDMA_FIRST       = -3532020,
    GOSSIP_ITEM_GRANDMA_SECOND      = -3532021,
    GOSSIP_ITEM_GRANDMA_THIRD       = -3532005,

    TEXT_ID_GRANDMA_FIRST           = 9009,
    TEXT_ID_GRANDMA_SECOND          = 9010,
    TEXT_ID_GRANDMA_THIRD           = 9011,

    GOSSIP_MENU_ID_GRANDMA_FIRST    = 7441,
    GOSSIP_MENU_ID_GRANDMA_SECOND   = 7442,
    GOSSIP_MENU_ID_GRANDMA_THIRD    = 7443,

    TEXT_ID_GRANDMA                 = 8990, // Unk purpose

    /**** The Wolf's Entry ****/
    NPC_BIG_BAD_WOLF                = 17521
};

bool GossipHello_npc_grandmother(Player* pPlayer, Creature* creature)
{
    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GRANDMA_FIRST, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    pPlayer->SET_GOSSIP_MENU_ID(GOSSIP_MENU_ID_GRANDMA_FIRST);
    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_GRANDMA_FIRST, creature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_grandmother(Player* pPlayer, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GRANDMA_SECOND, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SET_GOSSIP_MENU_ID(GOSSIP_MENU_ID_GRANDMA_SECOND);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_GRANDMA_SECOND, creature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GRANDMA_THIRD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SET_GOSSIP_MENU_ID(GOSSIP_MENU_ID_GRANDMA_THIRD);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_GRANDMA_THIRD, creature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            if (Creature* bigBadWolf = creature->SummonCreature(NPC_BIG_BAD_WOLF, 0, 0, 0, 0, TEMPSPAWN_DEAD_DESPAWN, 0))
                bigBadWolf->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pPlayer, bigBadWolf);
            creature->ForcedDespawn();
            break;
    }

    return true;
}

struct npc_grandmotherAI : public ScriptedAI
{
    npc_grandmotherAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        Reset();
    }

    ScriptedInstance* m_instance;

    void Reset() override
    {

    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_instance)
        {
            if (!m_instance->GetPlayerInMap(true, false))
            {
                if (m_instance)
                    m_instance->SetData(TYPE_OPERA, FAIL);

                m_creature->ForcedDespawn();
            }
        }
    }
};

struct boss_bigbadwolfAI : public ScriptedAI
{
    boss_bigbadwolfAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        Reset();
    }

    ScriptedInstance* m_instance;

    uint32 m_uiRedRidingHoodTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiSwipeTimer;

    Player* m_originalTarget;
    float m_originalThreat;

    bool m_isFixating;
    bool m_startFixate;

    void Reset() override
    {
        m_uiRedRidingHoodTimer = 4000;
        m_uiFearTimer          = urand(25000, 35000);
        m_uiSwipeTimer         = 5000;

        m_originalTarget = nullptr;
        m_originalThreat = 0;

        m_isFixating = false;
        m_startFixate = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_WOLF_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoPlaySoundToSet(m_creature, SOUND_WOLF_DEATH);

        if (m_instance)
            m_instance->SetData(TYPE_OPERA, DONE);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            AttackStart(invoker);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_isFixating && !m_startFixate)
        {
            Map::PlayerList const& pPlayers = m_creature->GetMap()->GetPlayers();

            for (Map::PlayerList::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
            {
                if (Player* pPlayer = itr->getSource())
                {
                    if (pPlayer->HasAura(SPELL_RED_RIDING_HOOD) || pPlayer->HasAura(30753))
                    {
                        m_creature->FixateTarget(pPlayer);
                        DoScriptText(SAY_WOLF_HOOD, m_creature);
                        m_startFixate = true;

                        // Apply chasing timer
                        m_uiRedRidingHoodTimer = 20000;
                        break;
                    }
                }
            }
        }

        if (m_uiRedRidingHoodTimer < uiDiff)
        {
            if (!m_isFixating || (m_isFixating && !m_startFixate))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_PICK_RED_RIDING_HOOD) == CAST_OK)
                {
                    // Apply fixate
                    m_isFixating = true;
                }
            }
            else
            {
                m_creature->FixateTarget(nullptr);

                m_uiRedRidingHoodTimer = 30000;
                m_isFixating = false;
                m_startFixate = false;
            }
        }
        else
            m_uiRedRidingHoodTimer -= uiDiff;

        if (m_uiFearTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TERRIFYING_HOWL) == CAST_OK)
                m_uiFearTimer = 24000;
        }
        else
            m_uiFearTimer -= uiDiff;

        if (m_uiSwipeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WIDE_SWIPE) == CAST_OK)
                m_uiSwipeTimer = urand(25000, 30000);
        }
        else
            m_uiSwipeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/**********************************************/
/******** Opera Romeo and Juliet Event *******/
/********************************************/

enum
{
    /**** Speech *****/
    SAY_JULIANNE_AGGRO              = -1532046,
    SAY_JULIANNE_ENTER              = -1532047,
    SAY_JULIANNE_DEATH01            = -1532048,
    SAY_JULIANNE_DEATH02            = -1532049,
    SAY_JULIANNE_RESURRECT          = -1532050,
    SAY_JULIANNE_SLAY               = -1532051,

    SAY_ROMULO_AGGRO                = -1532052,
    SAY_ROMULO_DEATH                = -1532053,
    SAY_ROMULO_ENTER                = -1532054,
    SAY_ROMULO_RESURRECT            = -1532055,
    SAY_ROMULO_SLAY                 = -1532056,

    /***** Spells For Julianne *****/
    SPELL_BLINDING_PASSION          = 30890,
    SPELL_DEVOTION                  = 30887,
    SPELL_ETERNAL_AFFECTION         = 30878,
    SPELL_POWERFUL_ATTRACTION       = 30889,
    SPELL_DRINK_POISON              = 30907,

    /***** Spells For Romulo ****/
    SPELL_BACKWARD_LUNGE            = 30815,
    SPELL_DARING                    = 30841,
    SPELL_DEADLY_SWATHE             = 30817,
    SPELL_POISON_THRUST             = 30822,

    /**** Other Misc. Spells ****/
    SPELL_FULL_HEALTH               = 43979,                // res effect on Julianne
    SPELL_UNDYING_LOVE              = 30951,                // res effect on Romulo
    SPELL_SUICIDE_WHILE_DEAD        = 30966,                // suicide spell for Julianne and Romulo
};

enum OperaPhase
{
    PHASE_JULIANNE      = 0,
    PHASE_ROMULO        = 1,
    PHASE_BOTH          = 2,
};

static const float afRomuloSpawnLoc[4] = { -10893.62f, -1760.78f, 90.55f, 4.76f};

struct boss_julianneAI : public ScriptedAI
{
    boss_julianneAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        Reset();
    }

    ScriptedInstance* m_instance;

    OperaPhase m_Phase;

    uint32 m_uiBlindingPassionTimer;
    uint32 m_uiDevotionTimer;
    uint32 m_uiEternalAffectionTimer;
    uint32 m_uiPowerfulAttractionTimer;
    uint32 m_uiSummonRomuloTimer;
    uint32 m_uiResurrectSelfTimer;
    uint32 m_uiAggroTimer;

    bool m_bIsFakingDeath;

    void Reset() override
    {
        m_Phase                     = PHASE_JULIANNE;

        m_uiBlindingPassionTimer    = 30000;
        m_uiDevotionTimer           = 15000;
        m_uiEternalAffectionTimer   = 25000;
        m_uiPowerfulAttractionTimer = 5000;
        m_uiSummonRomuloTimer       = 0;
        m_uiResurrectSelfTimer      = 0;

        m_bIsFakingDeath            = false;

        m_uiAggroTimer = 9000;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_JULIANNE_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        if (damage < m_creature->GetHealth())
            return;

        damage = std::min(damage, m_creature->GetHealth() - 1);

        if (m_bIsFakingDeath)
            return;

        if (m_Phase == PHASE_JULIANNE)
        {
            // Prepare fake death
            if (DoCastSpellIfCan(m_creature, SPELL_DRINK_POISON, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
            {
                m_Phase               = PHASE_BOTH;
                m_bIsFakingDeath      = true;
                m_uiSummonRomuloTimer = 12000;
            }
        }
        else if (m_Phase == PHASE_BOTH)
        {
            // set fake death and allow 10 sec timer to kill Romulos
            DoScriptText(SAY_JULIANNE_DEATH02, m_creature);
            DoSetFakeDeath();
            m_uiResurrectSelfTimer = 10000;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

        if (m_instance)
            m_instance->SetData(TYPE_OPERA, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(SAY_JULIANNE_SLAY, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (m_creature->GetVictim())
            pSummoned->AI()->AttackStart(m_creature->GetVictim());
    }

    // Wrapper to set fake death
    void DoSetFakeDeath()
    {
        m_bIsFakingDeath = true;

        m_creature->InterruptNonMeleeSpells(false);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        m_creature->ClearAllReactives();
        m_creature->SetTarget(nullptr);
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
        SetCombatMovement(false);
    }

    // Wrapper to remove fake death
    void DoRemoveFakeDeath()
    {
        m_bIsFakingDeath = false;

        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        DoResetThreat();
        SetCombatMovement(true);
        DoStartMovement(m_creature->GetVictim());
    }

    // Wrapper to start phase 3
    void DoHandleRomuloResurrect()
    {
        m_creature->CastSpell(nullptr, SPELL_FULL_HEALTH, TRIGGERED_NONE);
        DoRemoveFakeDeath();
        DoCastSpellIfCan(m_creature, SPELL_UNDYING_LOVE);
        DoScriptText(SAY_JULIANNE_RESURRECT, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // spawn Romulo on timer after fake death
        if (m_uiSummonRomuloTimer)
        {
            if (m_uiSummonRomuloTimer <= uiDiff)
            {
                m_creature->SummonCreature(NPC_ROMULO, afRomuloSpawnLoc[0], afRomuloSpawnLoc[1], afRomuloSpawnLoc[2], afRomuloSpawnLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0);
                m_uiSummonRomuloTimer = 0;
            }
            else
                m_uiSummonRomuloTimer -= uiDiff;
        }

        if (m_uiResurrectSelfTimer)
        {
            if (m_uiResurrectSelfTimer <= uiDiff)
            {
                if (m_instance)
                {
                    if (Creature* pRomulo = m_instance->GetSingleCreatureFromStorage(NPC_ROMULO))
                    {
                        // if Romulos is dead, then self kill
                        if (pRomulo->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE))
                        {
                            m_creature->CastSpell(nullptr, SPELL_SUICIDE_WHILE_DEAD, TRIGGERED_OLD_TRIGGERED);
                            pRomulo->CastSpell(nullptr, SPELL_SUICIDE_WHILE_DEAD, TRIGGERED_OLD_TRIGGERED);
                        }
                        else
                        {
                            DoRemoveFakeDeath();
                            DoCastSpellIfCan(m_creature, SPELL_FULL_HEALTH, CAST_TRIGGERED);
                        }
                    }
                }
                m_uiResurrectSelfTimer = 0;
            }
            else
                m_uiResurrectSelfTimer -= uiDiff;
        }

        if (m_uiAggroTimer)
        {
            if (m_uiAggroTimer <= uiDiff)
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                SetReactState(REACT_AGGRESSIVE);
                m_creature->SetInCombatWithZone();
                AttackClosestEnemy();
                if (!m_creature->IsInCombat())
                    JustReachedHome();
                m_uiAggroTimer = 0;
            }
            else
                m_uiAggroTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // don't use spells during transition
        if (m_bIsFakingDeath)
            return;

        if (m_uiBlindingPassionTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_BLINDING_PASSION) == CAST_OK)
                    m_uiBlindingPassionTimer = urand(30000, 45000);
            }
        }
        else
            m_uiBlindingPassionTimer -= uiDiff;

        if (m_uiDevotionTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DEVOTION) == CAST_OK)
                m_uiDevotionTimer = urand(15000, 45000);
        }
        else
            m_uiDevotionTimer -= uiDiff;

        if (m_uiPowerfulAttractionTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_POWERFUL_ATTRACTION) == CAST_OK)
                    m_uiPowerfulAttractionTimer = urand(5000, 30000);
            }
        }
        else
            m_uiPowerfulAttractionTimer -= uiDiff;

        if (m_uiEternalAffectionTimer < uiDiff)
        {
            if (Unit* pTarget = DoSelectLowestHpFriendly(30.0f))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_ETERNAL_AFFECTION) == CAST_OK)
                    m_uiEternalAffectionTimer = urand(45000, 60000);
            }
        }
        else
            m_uiEternalAffectionTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool EffectDummyCreature_spell_drink_poison(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* creatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_DRINK_POISON && uiEffIndex == EFFECT_INDEX_0)
    {
        // Set fake death on poison
        if (boss_julianneAI* pJulianneAI = dynamic_cast<boss_julianneAI*>(creatureTarget->AI()))
            pJulianneAI->DoSetFakeDeath();

        DoScriptText(SAY_JULIANNE_DEATH01, creatureTarget);

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

struct boss_romuloAI : public ScriptedAI
{
    boss_romuloAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        Reset();
    }

    ScriptedInstance* m_instance;

    OperaPhase m_Phase;

    uint32 m_uiBackwardLungeTimer;
    uint32 m_uiDaringTimer;
    uint32 m_uiDeadlySwatheTimer;
    uint32 m_uiPoisonThrustTimer;
    uint32 m_uiResurrectTimer;
    uint32 m_uiResurrectSelfTimer;

    bool m_bIsFakingDeath;

    void Reset() override
    {
        m_Phase                 = PHASE_ROMULO;

        m_uiBackwardLungeTimer  = 15000;
        m_uiDaringTimer         = 20000;
        m_uiDeadlySwatheTimer   = 25000;
        m_uiPoisonThrustTimer   = 10000;
        m_uiResurrectTimer      = 0;
        m_uiResurrectSelfTimer  = 0;

        m_bIsFakingDeath        = false;
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_OPERA, FAIL);

        m_creature->ForcedDespawn();
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        if (damage < m_creature->GetHealth())
            return;

        damage = std::min(damage, m_creature->GetHealth() - 1);

        if (m_bIsFakingDeath)
            return;

        if (m_Phase == PHASE_ROMULO)
        {
            DoScriptText(SAY_ROMULO_DEATH, m_creature);
            DoSetFakeDeath();
            m_Phase             = PHASE_BOTH;
            m_uiResurrectTimer  = 10000;
        }
        else if (m_Phase == PHASE_BOTH)
        {
            // set fake death and allow 10 sec timer to kill Julianne
            DoSetFakeDeath();
            m_uiResurrectSelfTimer = 10000;
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_ROMULO_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

        if (m_instance)
            m_instance->SetData(TYPE_OPERA, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(SAY_ROMULO_SLAY, m_creature);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        // remove fake death on res
        if (pSpell->Id == SPELL_UNDYING_LOVE && pCaster->GetEntry() == NPC_JULIANNE)
            DoRemoveFakeDeath();
    }

    // Wrapper to set fake death
    void DoSetFakeDeath()
    {
        m_bIsFakingDeath = true;

        m_creature->InterruptNonMeleeSpells(false);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        m_creature->ClearAllReactives();
        m_creature->SetTarget(nullptr);
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
        SetCombatMovement(false);
    }

    // Wrapper to remove fake death
    void DoRemoveFakeDeath()
    {
        m_bIsFakingDeath = false;

        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        DoResetThreat();
        SetCombatMovement(true);
        DoStartMovement(m_creature->GetVictim());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Resurrect both of them at the beginning of phase 3
        if (m_uiResurrectTimer)
        {
            if (m_uiResurrectTimer <= uiDiff)
            {
                if (m_instance)
                {
                    if (Creature* pJulianne = m_instance->GetSingleCreatureFromStorage(NPC_JULIANNE))
                    {
                        if (boss_julianneAI* pJulianneAI = dynamic_cast<boss_julianneAI*>(pJulianne->AI()))
                            pJulianneAI->DoHandleRomuloResurrect();
                    }
                }
                m_uiResurrectTimer = 0;
            }
            else
                m_uiResurrectTimer -= uiDiff;
        }

        if (m_uiResurrectSelfTimer)
        {
            if (m_uiResurrectSelfTimer <= uiDiff)
            {
                if (m_instance)
                {
                    if (Creature* pJulianne = m_instance->GetSingleCreatureFromStorage(NPC_JULIANNE))
                    {
                        // if Julianne is dead, then self kill
                        if (pJulianne->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE))
                        {
                            m_creature->CastSpell(nullptr, SPELL_SUICIDE_WHILE_DEAD, TRIGGERED_OLD_TRIGGERED);
                            pJulianne->CastSpell(nullptr, SPELL_SUICIDE_WHILE_DEAD, TRIGGERED_OLD_TRIGGERED);
                        }
                        else
                        {
                            DoRemoveFakeDeath();
                            DoScriptText(SAY_ROMULO_RESURRECT, m_creature);
                            DoCastSpellIfCan(m_creature, SPELL_FULL_HEALTH, CAST_TRIGGERED);
                        }
                    }
                }
                m_uiResurrectSelfTimer = 0;
            }
            else
                m_uiResurrectSelfTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // don't use spells on fake death
        if (m_bIsFakingDeath)
            return;

        if (m_uiBackwardLungeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BACKWARD_LUNGE) == CAST_OK)
                m_uiBackwardLungeTimer = urand(15000, 30000);
        }
        else
            m_uiBackwardLungeTimer -= uiDiff;

        if (m_uiDaringTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DARING) == CAST_OK)
                m_uiDaringTimer = urand(20000, 40000);
        }
        else
            m_uiDaringTimer -= uiDiff;

        if (m_uiDeadlySwatheTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DEADLY_SWATHE) == CAST_OK)
                m_uiDeadlySwatheTimer = urand(15000, 25000);
        }
        else
            m_uiDeadlySwatheTimer -= uiDiff;

        if (m_uiPoisonThrustTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_POISON_THRUST) == CAST_OK)
#ifdef PRENERF_2_0_3
                m_uiPoisonThrustTimer = urand(8000, 16000);
#else
                m_uiPoisonThrustTimer = urand(10000, 20000);
#endif
        }
        else
            m_uiPoisonThrustTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

void AddSC_bosses_opera()
{
    // Oz
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_dorothee";
    pNewScript->GetAI = &GetNewAIInstance<boss_dorotheeAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_strawman";
    pNewScript->GetAI = &GetNewAIInstance<boss_strawmanAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_tinhead";
    pNewScript->GetAI = &GetNewAIInstance<boss_tinheadAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_roar";
    pNewScript->GetAI = &GetNewAIInstance<boss_roarAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_crone";
    pNewScript->GetAI = &GetNewAIInstance<boss_croneAI>;
    pNewScript->RegisterSelf();

    // Hood
    pNewScript = new Script;
    pNewScript->Name = "npc_grandmother";
    pNewScript->GetAI = &GetNewAIInstance<npc_grandmotherAI>;
    pNewScript->pGossipHello = &GossipHello_npc_grandmother;
    pNewScript->pGossipSelect = &GossipSelect_npc_grandmother;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_bigbadwolf";
    pNewScript->GetAI = &GetNewAIInstance<boss_bigbadwolfAI>;
    pNewScript->RegisterSelf();

    // Romeo And Juliet
    pNewScript = new Script;
    pNewScript->Name = "boss_julianne";
    pNewScript->GetAI = &GetNewAIInstance<boss_julianneAI>;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_spell_drink_poison;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_romulo";
    pNewScript->GetAI = &GetNewAIInstance<boss_romuloAI>;
    pNewScript->RegisterSelf();
}
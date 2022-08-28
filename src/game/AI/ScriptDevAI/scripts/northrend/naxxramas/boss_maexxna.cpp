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
SDName: Boss_Maexxna
SD%Complete: 90
SDComment: Web Wrap works incorrectly: The way it's supposed to work is Maexxna throws the player into one of several directions against the wall.
    The player is then supposed to drop down from the wall and when the player stops moving, the Web Wrap NPC spawns on the player location.
    While thrown the Web Wrap visual already starts running on the player.
SDCategory: Naxxramas
EndScriptData */

#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "AI/ScriptDevAI/base/TimerAI.h"
#include "AI/ScriptDevAI/include/sc_common.h"
#include "Entities/Unit.h"
#include "naxxramas.h"

enum
{
    // Maexxna Spells

    // Web Wrap spells
    SPELL_WEB_WRAP_200          = 28618,
    SPELL_WEB_WRAP_300          = 28619,
    SPELL_WEB_WRAP_400          = 28620,
    SPELL_WEB_WRAP_500          = 28621,
    SPELL_WEBWRAP_STUN          = 28622,                    // Triggered by spells 28618 - 28621
    SPELL_WEB_WRAP_SUMMON       = 28627,
    SPELL_CLEAR_WEB_WRAP_TARGET = 28628,
    SPELL_CLEAR_WEB_WRAP_SELF   = 28629,
    SPELL_SELF_STUN             = 29826,                    // Spell Id unsure
    SPELL_KILL_WEBWRAP          = 29108,

    NPC_INVISIBLE_MAN           = 17286,                    // Handle the summoning of the players and Web Wrap NPCs

    MAX_PLAYERS_WEB_WRAP_H      = 2,
    MAX_PLAYERS_WEB_WRAP        = 1,

    TIMER_40_SEC                = 40000,                    // Used by all main abilities

    EMOTE_SPIN_WEB              = 32303,
    EMOTE_SPIDERLING            = 32305,
    EMOTE_SPRAY                 = 32304,
    EMOTE_BOSS_GENERIC_FRENZY   = 1191,

    SPELL_DOUBLE_ATTACK         = 19818,

    SPELL_WEB_WRAP_INIT         = 28617,
    SPELL_WEBWRAP               = 28622,
    //SPELL_WEBWRAP_2             = 28673,                    // purpose unknown

    SPELL_WEBSPRAY              = 29484,
    SPELL_WEBSPRAY_H            = 54125,

    SPELL_POISONSHOCK           = 28741,
    SPELL_POISONSHOCK_H         = 54122,

    SPELL_NECROTICPOISON_H      = 28776,
    SPELL_NECROTICPOISON        = 54121,

    SPELL_FRENZY                = 54123,
    SPELL_FRENZY_H              = 54124,

    NPC_WEB_WRAP                = 16486,
    NPC_SPIDERLING              = 17055,

    MAX_SPIDERLINGS             = 8,
    MAX_WEB_WRAP_POSITIONS      = 3,
};

/*###################
#   npc_web_wrap
###################*/
struct npc_web_wrapAI : public ScriptedAI
{
    npc_web_wrapAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_victimGuid;
    uint32 m_uiWebWrapTimer;

    void Reset() override
    {
        m_uiWebWrapTimer = 0;
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override {}
    void AttackStart(Unit* /*pWho*/) override {}

    void SetVictim(Unit* pVictim)
    {
        if (pVictim)
        {
            // Vanilla spell 28618, 28619, 28620, 28621 had effect SPELL_EFFECT_PULL_TOWARDS with EffectMiscValue = 200, 300, 400 and 500
            // All these spells trigger 28622 after 1 or 2 seconds
            // the EffectMiscValue may have been based on the distance between the victim and the target

            // NOTE: This implementation may not be 100% correct, but it gets very close to the expected result

            float fDist = m_creature->GetDistance(pVictim, false);
            // Switch the speed multiplier based on the distance from the web wrap
            uint32 uiEffectMiscValue = 500;
            if (fDist < 25.0f)
                uiEffectMiscValue = 200;
            else if (fDist < 50.0f)
                uiEffectMiscValue = 300;
            else if (fDist < 75.0f)
                uiEffectMiscValue = 400;

            // Note: normally we should use the Knockback effect to handle this, but because this doesn't behave as expected we'll just use Jump Movement
            // pVictim->KnockBackFrom(m_creature, -fDist, uiEffectMiscValue * 0.1f);

            float fSpeed = fDist * (uiEffectMiscValue * 0.01f);
            pVictim->GetMotionMaster()->MoveJump(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), fSpeed, 0.0f);

            m_victimGuid = pVictim->GetObjectGuid();
            m_uiWebWrapTimer = uiEffectMiscValue == 200 ? 1000 : 2000;
            m_creature->SetInCombatWithVictim(pVictim);
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_victimGuid)
        {
            if (Player* pVictim = m_creature->GetMap()->GetPlayer(m_victimGuid))
            {
                if (pVictim->IsAlive())
                    pVictim->RemoveAurasDueToSpell(SPELL_WEBWRAP);
                pVictim->RestoreDisplayId();
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiWebWrapTimer)
        {
            // Finally the player gets web wrapped and he should change the display id until the creature is killed
            if (m_uiWebWrapTimer <= uiDiff)
            {
                if (Player* pVictim = m_creature->GetMap()->GetPlayer(m_victimGuid))
                {
                    pVictim->SetDisplayId(15435);
                    pVictim->CastSpell(pVictim, SPELL_WEBWRAP, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
                }

                m_uiWebWrapTimer = 0;
            }
            else
                m_uiWebWrapTimer -= uiDiff;
        }
        if (m_victimGuid)
        {
            if (Player* pVictim = m_creature->GetMap()->GetPlayer(m_victimGuid))
            {
                if (!pVictim->IsAlive())
                {
                    pVictim->RemoveAurasDueToSpell(SPELL_WEBWRAP);
                    pVictim->RestoreDisplayId();
                }
            }
        }
    }
};

/*###################
#   boss_maexxna
###################*/

enum MaexxnaActions
{
    MAEXXNA_WEBWRAP,
    MAEXXNA_WEBSPRAY,
    MAEXXNA_POISON_SHOCK,
    MAEXXNA_NECROTIC_POISON,
    MAEXXNA_SUMMON_SPIDERLING,
    MAEXXNA_ENRAGE_HP_CHECK,
    MAEXXNA_ACTION_MAX,
};

static const float aWebWrapLoc[MAX_WEB_WRAP_POSITIONS][3] =
{
    {3546.796f, -3869.082f, 296.450f},
    {3531.271f, -3847.424f, 299.450f},
    {3497.067f, -3843.384f, 302.384f}
};

struct boss_maexxnaAI : public BossAI
{
    boss_maexxnaAI(Creature* creature) : BossAI(creature, MAEXXNA_ACTION_MAX),
    m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
    m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddTimerlessCombatAction(MAEXXNA_ENRAGE_HP_CHECK, true);         // Soft enrage à 30%
        AddCombatAction(MAEXXNA_WEBWRAP, 20s);
        AddCombatAction(MAEXXNA_WEBSPRAY, 40s);
        AddCombatAction(MAEXXNA_POISON_SHOCK, 10s, 20s);
        AddCombatAction(MAEXXNA_NECROTIC_POISON, 20s, 30s);
        AddCombatAction(MAEXXNA_SUMMON_SPIDERLING, 30s);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    CreatureList m_summoningTriggers;
    SelectAttackingTargetParams m_webWrapParams;

    void Reset() override
    {
        CombatAI::Reset();

        m_webWrapParams.range.minRange = 0;
        m_webWrapParams.range.maxRange = 100;

        DoCastSpellIfCan(m_creature, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case MAEXXNA_WEBWRAP: //fall through
            case MAEXXNA_WEBSPRAY: return 40s;
            case MAEXXNA_POISON_SHOCK: return RandomTimer(10s, 20s);
            case MAEXXNA_NECROTIC_POISON: return RandomTimer(10s, 30s);
            case MAEXXNA_SUMMON_SPIDERLING: return 30s;
            default: return 0s;
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MAEXXNA, IN_PROGRESS);
        else
            return;

        // Fill the list of summoning NPCs for the Web Wrap ability
        GetCreatureListWithEntryInGrid(m_summoningTriggers, m_creature, NPC_INVISIBLE_MAN, 100.0f);
    }

    void JustReachedHome() override
    {
        BossAI::JustReachedHome();
        DoCastSpellIfCan(m_creature, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_SPIDERLING)
            summoned->SetInCombatWithZone();
        if (summoned->GetEntry() == NPC_WEB_WRAP)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, uint32(0), SELECT_FLAG_PLAYER))
            {
                if (npc_web_wrapAI* pWebAI = dynamic_cast<npc_web_wrapAI*>(summoned->AI()))
                    pWebAI->SetVictim(pTarget);
            }
        }
    }

    void SummonedJustReachedHome(Creature* summoned) override
    {
        summoned->ForcedDespawn();
    }

    void SummonSpiderlings()
    {
        for (uint8 i = 0; i < MAX_SPIDERLINGS; ++i)
            m_creature->SummonCreature(NPC_SPIDERLING, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
    }

    bool DoCastWebWrap()
    {
        // If we can't select a player for web wrap then skip the summoning
        if (!m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, uint32(0), SELECT_FLAG_PLAYER))
            return false;

        uint8 uiPos1 = urand(0, MAX_WEB_WRAP_POSITIONS - 1);
        m_creature->SummonCreature(NPC_WEB_WRAP, aWebWrapLoc[uiPos1][0], aWebWrapLoc[uiPos1][1], aWebWrapLoc[uiPos1][2], 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);

        // Summon a second web wrap on heroic
        if (!m_isRegularMode)
        {
            uint8 uiPos2 = (uiPos1 + urand(1, MAX_WEB_WRAP_POSITIONS - 1)) % MAX_WEB_WRAP_POSITIONS;
            m_creature->SummonCreature(NPC_WEB_WRAP, aWebWrapLoc[uiPos2][0], aWebWrapLoc[uiPos2][1], aWebWrapLoc[uiPos2][2], 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
        }

        return true;
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MAEXXNA_ENRAGE_HP_CHECK:
            {
                if (m_creature->GetHealthPercent() < 30.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
                    {
                        DoBroadcastText(EMOTE_BOSS_GENERIC_FRENZY, m_creature);
                        DisableCombatAction(action);
                    }
                }
                return;
            }
            case MAEXXNA_WEBWRAP:
            {
                if (DoCastWebWrap())
                {
                    DoBroadcastText(EMOTE_SPIN_WEB, m_creature);
                    break;
                }
                return;
            }
            case MAEXXNA_WEBSPRAY:
            {
                if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_WEBSPRAY : SPELL_WEBSPRAY_H) == CAST_OK)
                {
                    DoBroadcastText(EMOTE_SPRAY, m_creature);
                    break;
                }
                return;
            }
            case MAEXXNA_POISON_SHOCK:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_POISONSHOCK : SPELL_POISONSHOCK_H) == CAST_OK)
                    break;
                return;
            }
            case MAEXXNA_NECROTIC_POISON:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_NECROTICPOISON : SPELL_NECROTICPOISON_H) == CAST_OK)
                    break;
                return;
            }
            case MAEXXNA_SUMMON_SPIDERLING:
            {
                SummonSpiderlings();
                DoBroadcastText(EMOTE_SPIDERLING, m_creature);
                break;
            }
            default:
                return;
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

void AddSC_boss_maexxna()
{
    Script* newScript = new Script;
    newScript->Name = "boss_maexxna";
    newScript->GetAI = &GetNewAIInstance<boss_maexxnaAI>;
    newScript->RegisterSelf();

    newScript = new Script;
    newScript->Name = "npc_web_wrap";
    newScript->GetAI = &GetNewAIInstance<npc_web_wrapAI>;
    newScript->RegisterSelf();
}

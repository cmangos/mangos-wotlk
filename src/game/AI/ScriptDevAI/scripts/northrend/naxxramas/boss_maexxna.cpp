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

#include "AI/ScriptDevAI/base/BossAI.h"
#include "AI/ScriptDevAI/include/sc_common.h"
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
    SPELL_WEBWRAP_AURA          = 28622,

    SPELL_WEBWRAP               = 28673,
    SPELL_WEBWRAP_H             = 54127,

    SPELL_WEBSPRAY              = 29484,
    SPELL_WEBSPRAY_H            = 54125,

    SPELL_POISONSHOCK           = 28741,
    SPELL_POISONSHOCK_H         = 54122,

    SPELL_NECROTICPOISON_H      = 28776,
    SPELL_NECROTICPOISON        = 54121,

    SPELL_SUMMON_MAEXXNA_SPIDERLINGS   = 54130,
    SPELL_SUMMON_MAEXXNA_SPIDERLINGS_H = 29434,

    SPELL_FRENZY                = 54123,
    SPELL_FRENZY_H              = 54124,

    NPC_WEB_WRAP                = 16486,
    NPC_SPIDERLING              = 17055,

    MAX_WEB_WRAP_POSITIONS      = 3,
};

/*###################
#   npc_web_wrap
###################*/
struct npc_web_wrapAI : public ScriptedAI
{
    npc_web_wrapAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_victimGuid;

    void Reset() override
    {

    }

    void MoveInLineOfSight(Unit* /*who*/) override {}
    void AttackStart(Unit* /*who*/) override {}

    void SetVictim(Unit* pVictim)
    {
        if (pVictim)
        {
            m_victimGuid = pVictim->GetObjectGuid();
            m_creature->SetInCombatWithVictim(pVictim);
        }
    }

    void JustRespawned() override
    {
        // spawner is wrapped player
        if (Unit* spawner = m_creature->GetSpawner())
            SetVictim(spawner);
        else // something went terribly wrong - cleanup
            m_creature->ForcedDespawn(1);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_victimGuid)
        {
            if (Player* victim = m_creature->GetMap()->GetPlayer(m_victimGuid))
            {
                if (victim->IsAlive())
                    victim->RemoveAurasDueToSpell(SPELL_WEBWRAP);
                victim->RestoreDisplayId();
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_victimGuid)
        {
            if (Player* victim = m_creature->GetMap()->GetPlayer(m_victimGuid))
            {
                if (!victim->IsAlive())
                {
                    victim->RemoveAurasDueToSpell(SPELL_WEBWRAP);
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
        SetDataType(TYPE_MAEXXNA);
        AddTimerlessCombatAction(MAEXXNA_ENRAGE_HP_CHECK, true);         // Soft enrage à 30%
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    void Reset() override
    {
        CombatAI::Reset();

        DoCastSpellIfCan(nullptr, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_SPIDERLING)
            summoned->SetInCombatWithZone();
    }

    void SummonedJustReachedHome(Creature* summoned) override
    {
        summoned->ForcedDespawn();
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* target) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_WEBWRAP: case SPELL_WEBWRAP_H: DoBroadcastText(EMOTE_SPIN_WEB, m_creature); break;
            case SPELL_WEBSPRAY: case SPELL_WEBSPRAY_H: DoBroadcastText(EMOTE_SPRAY, m_creature); break;
            case SPELL_SUMMON_MAEXXNA_SPIDERLINGS: case SPELL_SUMMON_MAEXXNA_SPIDERLINGS_H: DoBroadcastText(EMOTE_SPIDERLING, m_creature); break;
            default: break;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MAEXXNA_ENRAGE_HP_CHECK:
            {
                if (m_creature->GetHealthPercent() < 30.0f)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_FRENZY) == CAST_OK)
                    {
                        DoBroadcastText(EMOTE_BOSS_GENERIC_FRENZY, m_creature);
                        DisableCombatAction(action);
                    }
                }
                return;
            }
        }
    }
};

struct WebWrapMaexxna : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        uint32 targetCount = spell->m_spellInfo->Id == SPELL_WEBWRAP ? MAX_PLAYERS_WEB_WRAP : MAX_PLAYERS_WEB_WRAP_H;
        std::vector<uint32> targetSpells{ 28617, 29280, 29281, 29282, 29283, 29285, 29287 };
        std::vector<Unit*> unitList;
        spell->GetCaster()->SelectAttackingTargets(unitList, ATTACKING_TARGET_ALL_SUITABLE, 1, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_SKIP_TANK);
        std::shuffle(unitList.begin(), unitList.end(), *GetRandomGenerator());
        unitList.resize(targetCount);
        for (Unit* target : unitList)
        {
            uint32 spellId = targetSpells[urand(0, targetSpells.size() - 1)];
            targetSpells.erase(std::remove(targetSpells.begin(), targetSpells.end(), spellId), targetSpells.end());
            target->CastSpell(nullptr, spellId, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct WebWrapMessage : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        float dist = spell->GetCaster()->GetDistance(spell->GetUnitTarget(), true, DIST_CALC_NONE);
        uint32 spellId = 0;
        if (dist <= 20.f)
            spellId = SPELL_WEB_WRAP_200;
        else if (dist <= 30.f)
            spellId = SPELL_WEB_WRAP_300;
        else if (dist <= 40.f)
            spellId = SPELL_WEB_WRAP_400;
        else
            spellId = SPELL_WEB_WRAP_500;

        if (spellId)
            spell->GetCaster()->CastSpell(spell->GetUnitTarget(), spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

struct WebWrapDamage : public AuraScript
{
    void OnPeriodicTickEnd(Aura* aura) const override
    {
        if (aura->GetAuraTicks() == 2)
            aura->GetTarget()->CastSpell(nullptr, SPELL_WEB_WRAP_SUMMON, TRIGGERED_OLD_TRIGGERED);
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

    RegisterSpellScript<WebWrapMaexxna>("spell_web_wrap_maexxna");
    RegisterSpellScript<WebWrapMessage>("spell_web_wrap_message");
    RegisterSpellScript<WebWrapDamage>("spell_web_wrap_damage");
}

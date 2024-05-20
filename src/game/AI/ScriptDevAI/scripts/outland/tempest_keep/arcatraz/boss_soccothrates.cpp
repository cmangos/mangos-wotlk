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
SDName: boss_soccothrates
SD%Complete: 80
SDComment: Spell Felfire Line Up and Wrath-Scryer's Felfire npc are summoning are NYI and they need additional research.
SDCategory: Tempest Keep, The Arcatraz
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "arcatraz.h"
#include "Spells/Scripts/SpellScript.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    // Intro yells
    SAY_SOCCOTHRATES_INTRO_1        = 20051,
    SAY_DALLIAH_INTRO_2             = 20055,
    SAY_SOCCOTHRATES_INTRO_3        = 20052,
    SAY_DALLIAH_INTRO_4             = 20056,
    SAY_SOCCOTHRATES_INTRO_5        = 20053,
    SAY_DALLIAH_INTRO_6             = 20057,
    SAY_SOCCOTHRATES_INTRO_7        = 20054,

    SAY_AGGRO                       = 19967,
    SAY_KILL                        = 19969,
    SAY_DEATH                       = 19973,
    SAY_CHARGE_1                    = 19971,
    SAY_CHARGE_2                    = 19972,

    SPELL_IMMOLATION                = 36051,
    SPELL_IMMOLATION_H              = 39007,
    SPELL_KNOCK_AWAY                = 36512,
    SPELL_FELFIRE_LINE_UP           = 35770,                // dummy spellInfo - moves prespawned NPCs into a line
    SPELL_FELFIRE                   = 35769,
    SPELL_CHARGE_TARGETING          = 36038,                // summons 21030 on target
    SPELL_CHARGE                    = 35754,                // script target on 21030; also dummy effect area effect target on 20978 - makes the target cast 35769
    SPELL_FELFIRE_SHOCK             = 35759,
    SPELL_FELFIRE_SHOCK_H           = 39006,

    NPC_WRATH_SCRYER_CHARGE_TARGET  = 21030,
};

static const DialogueEntry aIntroDialogue[] =
{
    {SAY_SOCCOTHRATES_INTRO_1,  NPC_SOCCOTHRATES,   3000},
    {SAY_DALLIAH_INTRO_2,       NPC_DALLIAH,        2000},
    {SAY_SOCCOTHRATES_INTRO_3,  NPC_SOCCOTHRATES,   4000},
    {SAY_DALLIAH_INTRO_4,       NPC_DALLIAH,        5000},
    {SAY_SOCCOTHRATES_INTRO_5,  NPC_SOCCOTHRATES,   3000},
    {SAY_DALLIAH_INTRO_6,       NPC_DALLIAH,        3000},
    {SAY_SOCCOTHRATES_INTRO_7,  NPC_SOCCOTHRATES,   0},
    {0, 0, 0},
};

enum SoccothratesActions
{
    SOCCOTHRATES_ACTION_MAX,
    SOCCOTHRATES_FELFIRE_LINEUP,
};

struct boss_soccothratesAI : public CombatAI, private DialogueHelper
{
    boss_soccothratesAI(Creature* creature) : CombatAI(creature, SOCCOTHRATES_ACTION_MAX),
        DialogueHelper(aIntroDialogue),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty()),
        m_hasYelledIntro(false)
    {
        AddOnKillText(SAY_KILL);
        InitializeDialogueHelper(m_instance);
        AddCustomAction(SOCCOTHRATES_FELFIRE_LINEUP, true, [&]() { HandleFelfireLineup(); });
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    float m_x, m_y, m_z; // last charge target location
    uint8 m_lineUpCounter;

    bool m_hasYelledIntro;

    void Reset() override
    {
        CombatAI::Reset();

        DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_IMMOLATION : SPELL_IMMOLATION_H);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_AGGRO, m_creature);

        GuidVector felfireVector; // at aggro felfire mobs always teleport to respawn location
        m_instance->GetCreatureGuidVectorFromStorage(NPC_WRATH_SCRYER_FELFIRE, felfireVector);
        for (ObjectGuid& guid : felfireVector)
        {
            if (Creature* creature = m_creature->GetMap()->GetCreature(guid))
            {
                float x, y, z, ori;
                creature->GetRespawnCoord(x, y, z, &ori);
                creature->NearTeleportTo(x, y, z, ori);
            }
        }

        if (m_instance)
            m_instance->SetData(TYPE_SOCCOTHRATES, IN_PROGRESS);

        m_creature->SetCombatStartPosition(Position(aSoccotharesStartPos[0], aSoccotharesStartPos[1], aSoccotharesStartPos[2]));
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_hasYelledIntro && who->IsPlayer() && !static_cast<Player*>(who)->IsGameMaster() && m_creature->IsWithinDistInMap(who, 75.0f) && m_creature->IsWithinLOSInMap(who))
        {
            StartNextDialogueText(SAY_SOCCOTHRATES_INTRO_1);
            m_hasYelledIntro = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void JustDied(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_SOCCOTHRATES, DONE);
    }

    void EnterEvadeMode() override
    {
        CombatAI::EnterEvadeMode();

        if (m_instance)
            m_instance->SetData(TYPE_SOCCOTHRATES, FAIL);
    }

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType != POINT_MOTION_TYPE)
            return;

        // Adjust orientation
        if (pointId)
            m_creature->SetFacingTo(aSoccotharesStartPos[3]);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_WRATH_SCRYER_CHARGE_TARGET)
        {
            summoned->GetPosition(m_x, m_y, m_z);
            m_lineUpCounter = 1;
        }
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spellInfo) override
    {
        if (spellInfo->Id == SPELL_FELFIRE_LINE_UP)
        {
            // need to get even points between caster and target to reposition felfire evenly
            float sX = m_creature->GetPositionX(), sY = m_creature->GetPositionY(); // source coords
            float tX = m_x, tY = m_y; // target coords
            float felfireDistX = (tX - sX) / 7, felfireDistY = (tY - sY) / 7;
            float fX = sX + (felfireDistX * m_lineUpCounter), fY = sY + (felfireDistY * m_lineUpCounter);
            target->NearTeleportTo(fX, fY, m_creature->GetPositionZ(), m_creature->GetOrientation());
            m_lineUpCounter++;
        }
        else if (spellInfo->Id == SPELL_CHARGE && target->GetEntry() == NPC_WRATH_SCRYER_CHARGE_TARGET)
            SetCombatMovement(true);
    }

    void JustDidDialogueStep(int32 textEntry) override
    {
        // Move each of them to their places
        if (textEntry == SAY_SOCCOTHRATES_INTRO_7)
        {
            m_creature->GetMotionMaster()->MovePoint(1, aSoccotharesStartPos[0], aSoccotharesStartPos[1], aSoccotharesStartPos[2]);

            if (m_instance)
            {
                if (Creature* dalliah = m_instance->GetSingleCreatureFromStorage(NPC_DALLIAH))
                    dalliah->GetMotionMaster()->MovePoint(1, aDalliahStartPos[0], aDalliahStartPos[1], aDalliahStartPos[2]);
            }
        }
    }

    void HandleFelfireLineup()
    {
        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
        {
            if (DoCastSpellIfCan(target, SPELL_CHARGE_TARGETING) == CAST_OK)
            {
                m_creature->CastSpell(nullptr, SPELL_FELFIRE_LINE_UP, TRIGGERED_OLD_TRIGGERED);
                DoBroadcastText(urand(0, 1) ? SAY_CHARGE_1 : SAY_CHARGE_2, m_creature, target);
            }
        }
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        if (spellInfo->Id == SPELL_KNOCK_AWAY)
            ResetTimer(SOCCOTHRATES_FELFIRE_LINEUP, 2000);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);
        CombatAI::UpdateAI(uiDiff);
    }
};

// 35754 - Charge
struct SoccothratesCharge : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_2)
            return;

        if (Unit* target = spell->GetUnitTarget())
            target->CastSpell(nullptr, SPELL_FELFIRE, TRIGGERED_NONE);
    }
};

// 36512 - Knock Away
struct KnockAwaySoccothrates : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
        {
            Unit* target = aura->GetTarget();
            if (target->AI())
            {
                target->AI()->SetCombatMovement(false); // prevents interrupting charge
                // Note: this spellInfo will also light up the Wrath-Scryer's Felfire npcs
                target->AI()->DoCastSpellIfCan(nullptr, SPELL_CHARGE);
            }
        }
    }
};

void AddSC_boss_soccothrates()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_soccothrates";
    pNewScript->GetAI = &GetNewAIInstance<boss_soccothratesAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SoccothratesCharge>("spell_soccothrates_charge");
    RegisterSpellScript<KnockAwaySoccothrates>("spell_soccothrates_knock_away");
}

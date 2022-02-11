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
SDName: Boss_Moroes
SD%Complete: 95
SDComment: Timers.
SDCategory: Karazhan
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "karazhan.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    MAX_ACTIVE_GUESTS   = 4,
    MAX_GUESTS          = 6,

    SAY_AGGRO           = -1532011,
    SAY_SPECIAL_1       = -1532012,
    SAY_SPECIAL_2       = -1532013,
    SAY_KILL_1          = -1532014,
    SAY_KILL_2          = -1532015,
    SAY_KILL_3          = -1532016,
    SAY_DEATH           = -1532017,
    SAY_FRENZY          = -1000002,

    SPELL_VANISH        = 29448,
    SPELL_GARROTE       = 37066,
    SPELL_BLIND         = 34694,
    SPELL_GOUGE         = 29425,
    SPELL_TAUNT         = 37017,
    SPELL_FRENZY        = 37023
};

static const float guestLocations[MAX_ACTIVE_GUESTS][4] =
{
    { -10987.086f, -1884.002f, 81.73f, 1.352331f},
    { -10989.889f, -1882.592f, 81.73f, 0.932928f},
    { -10979.159f, -1885.326f, 81.73f, 2.017563f},
    { -10976.789f, -1883.946f, 81.73f, 2.328581f}
};

static const uint32 guests[MAX_GUESTS] =
{
    NPC_LADY_KEIRA_BERRYBUCK,
    NPC_LADY_CATRIONA_VON_INDI,
    NPC_LORD_CRISPIN_FERENCE,
    NPC_BARON_RAFE_DREUGER,
    NPC_BARONESS_DOROTHEA_MILLSTIPE,
    NPC_LORD_ROBIN_DARIS
};

enum MoroesActions
{
    MOROES_ACTION_VANISH,
    MOROES_ACTION_BLIND,
    MOROES_ACTION_GOUGE,
    MOROES_ACTION_ENRAGE,
    MOROES_ACTION_MAX,
    MOROES_ACTION_GAROTTE,
};

struct boss_moroesAI : public CombatAI
{
    boss_moroesAI(Creature* creature) : CombatAI(creature, MOROES_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddCombatAction(MOROES_ACTION_VANISH, 40000u);
        AddCombatAction(MOROES_ACTION_BLIND, 30000u);
        AddCombatAction(MOROES_ACTION_GOUGE, 21000u);
        AddTimerlessCombatAction(MOROES_ACTION_ENRAGE, true);
        AddCustomAction(MOROES_ACTION_GAROTTE, true, [&]()
        {
            if (m_creature->GetVictim())
                m_creature->GetVictim()->CastSpell(nullptr, SPELL_GARROTE, TRIGGERED_OLD_TRIGGERED);
        });
        m_creature->GetCombatManager().SetLeashingCheck([](Unit*, float x, float y, float) -> bool
        {
            return x < -11030.f || x > -10943.f || y < -1955.f || y > -1860.f;
        });
        AddOnKillText(SAY_KILL_1, SAY_KILL_2, SAY_KILL_3);
        Reset();
    }

    ScriptedInstance* m_instance;

    std::vector<uint32> m_vGuestsEntryList;

    void Reset() override
    {
        CombatAI::Reset();

        SetCombatScriptStatus(false);
        SetCombatMovement(true);

        DoSpawnGuests();
    }

    uint32 GetSubsequentActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case MOROES_ACTION_VANISH: return 42000;
            case MOROES_ACTION_BLIND: return urand(30000, 35000);
            case MOROES_ACTION_GOUGE: return urand(30000, 35000);
            default: return 0; // never occurs but for compiler
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            DoScriptText(urand(0, 1) ? SAY_SPECIAL_1 : SAY_SPECIAL_2, m_creature);
            m_creature->RemoveAurasDueToSpell(SPELL_TAUNT);
            SetMeleeEnabled(true);
            SetCombatScriptStatus(false);
            m_attackAngle = 0.f;
            if (m_creature->IsInCombat()) // can happen on evade
                DoStartMovement(m_creature->GetVictim());
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_MOROES, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MOROES, FAIL);
    }

    void JustDied(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        DoRemoveGarroteAura();

        if (m_instance)
            m_instance->SetData(TYPE_MOROES, DONE);
    }

    void DoSpawnGuests()
    {
        // not if m_creature are dead, so avoid
        if (!m_creature->IsAlive())
            return;

        // it's empty, so first time
        if (m_vGuestsEntryList.empty())
        {
            // pre-allocate size for speed
            m_vGuestsEntryList.resize(MAX_GUESTS);

            // fill vector array with entries from creature array
            for (uint8 i = 0; i < MAX_GUESTS; ++i)
                m_vGuestsEntryList[i] = guests[i];

            std::shuffle(m_vGuestsEntryList.begin(), m_vGuestsEntryList.end(), *GetRandomGenerator());

            // Summon the 4 entries
            for (uint8 i = 0; i < MAX_ACTIVE_GUESTS; ++i)
                m_creature->SummonCreature(m_vGuestsEntryList[i], guestLocations[i][0], guestLocations[i][1], guestLocations[i][2], guestLocations[i][3], TEMPSPAWN_CORPSE_DESPAWN, 0);
        }
        // Resummon the killed adds
        else
        {
            if (!m_instance)
                return;

            for (uint8 i = 0; i < MAX_ACTIVE_GUESTS; ++i)
            {
                // If we already have the creature on the map, then don't summon it
                if (Creature* add = m_instance->GetSingleCreatureFromStorage(m_vGuestsEntryList[i], true))
                {
                    if (add->IsInCombat())
                        add->AI()->EnterEvadeMode();
                    continue;
                }

                m_creature->SummonCreature(m_vGuestsEntryList[i], guestLocations[i][0], guestLocations[i][1], guestLocations[i][2], guestLocations[i][3], TEMPSPAWN_CORPSE_DESPAWN, 0);
            }
        }
    }

    // Wrapper to remove the Garrote aura on death and on evade - ToDo: maybe find a better way for this!
    void DoRemoveGarroteAura()
    {
        // remove aura from spell Garrote when Moroes dies
        Map* pMap = m_creature->GetMap();
        if (pMap->IsDungeon())
        {
            Map::PlayerList const& PlayerList = pMap->GetPlayers();

            if (PlayerList.isEmpty())
                return;

            for (const auto& i : PlayerList)
            {
                if (i.getSource()->IsAlive() && i.getSource()->HasAura(SPELL_GARROTE))
                    i.getSource()->RemoveAurasDueToSpell(SPELL_GARROTE);
            }
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MOROES_ACTION_VANISH:
            {
                Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_GARROTE, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_AURA);
                if (!target) // if no target without garrote found - select any random
                    target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER);
                if (!target)
                    break;
                target->CastSpell(nullptr, SPELL_TAUNT, TRIGGERED_OLD_TRIGGERED); // TODO: Needs to send both packets
                m_creature->SelectHostileTarget(); // apply taunt before vanish
                m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_STALKED); // hunters mark
                DoCastSpellIfCan(nullptr, SPELL_VANISH);
                SetCombatScriptStatus(true);
                SetMeleeEnabled(false);
                m_attackAngle = M_PI_F;
                DoStartMovement(m_creature->GetVictim());
                ResetCombatAction(action, GetSubsequentActionTimer(action));
                ResetTimer(MOROES_ACTION_GAROTTE, 9500);
                break;
            }
            case MOROES_ACTION_BLIND:
            {
                Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, SPELL_GARROTE, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_AURA | SELECT_FLAG_SKIP_TANK);
                if (!target) // if no target without garrote found - select any random
                    target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER);
                if (!target)
                    break;

                if (DoCastSpellIfCan(target, SPELL_BLIND) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case MOROES_ACTION_GOUGE:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_GOUGE) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case MOROES_ACTION_ENRAGE:
            {
                if (m_creature->GetHealthPercent() < 30.0f)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_FRENZY) == CAST_OK)
                    {
                        DoScriptText(SAY_FRENZY, m_creature);
                        SetActionReadyStatus(action, false);
                    }
                }
                break;
            }
        }
    }
};

struct MoroesVanish : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            aura->GetTarget()->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, aura->GetTarget(), aura->GetTarget());
    }
};

void AddSC_boss_moroes()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_moroes";
    pNewScript->GetAI = &GetNewAIInstance<boss_moroesAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<MoroesVanish>("spell_moroes_vanish");
}

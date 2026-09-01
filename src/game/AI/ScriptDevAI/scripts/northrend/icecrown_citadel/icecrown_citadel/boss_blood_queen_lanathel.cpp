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
SDName: boss_blood_queen_lanathel
SD%Complete: 90%
SDComment: Encounter mechanics and four raid difficulties implemented; Blood Infusion quest credit remains NYI.
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"

enum
{
    SAY_AGGRO                   = 38048,
    SAY_BITE_1                  = 38053,
    SAY_BITE_2                  = 38058,
    SAY_SHADOWS                 = 38061,
    SAY_PACT                    = 38062,
    SAY_AIR_PHASE               = 38063,
    SAY_BERSERK                 = 38068,
    SAY_DEATH                   = 38069,
    SAY_SLAY_1                  = 38066,
    SAY_SLAY_2                  = 38067,

    // all phases
    SPELL_BERSERK                   = 26662,
    SPELL_SHROUD_OF_SORROW          = 70986,

    // ground phase
    SPELL_BLOOD_MIRROR              = 70837,            // triggers 70445 and other similar spells
    SPELL_SWARMING_SHADOWS          = 71861,            // triggers 71264 and 71267
    SPELL_PACT_OF_THE_DARKFALLEN    = 71336,            // triggers 71340
    SPELL_VAMPIRIC_BITE             = 71837,            // triggers 71726 and 70946
    SPELL_TWILIGHT_BLOODBOLT        = 71445,            // triggers 72313, 71446 and 71818
    SPELL_DELIRIOUS_SLASH           = 72261,            // heroic only - triggers 71623 and 72264
    SPELL_PRESENCE_OF_DARKFALLEN    = 70994,            // heroic only - triggers 71958, 71959 and 71952
    SPELL_THIRST_QUENCHED           = 72154,            // related to quest 24756

    SPELL_ESSENCE_10_NORMAL         = 70879,
    SPELL_ESSENCE_25_NORMAL         = 71525,
    SPELL_ESSENCE_10_HEROIC         = 71530,
    SPELL_ESSENCE_25_HEROIC         = 71531,
    SPELL_FRENZIED_10               = 70877,
    SPELL_FRENZIED_25               = 71474,
    SPELL_UNCONTROLLABLE_FRENZY     = 70923,
    SPELL_BLOOD_MIRROR_DAMAGE       = 70821,
    SPELL_BLOOD_MIRROR_VISUAL       = 71510,
    SPELL_BLOOD_MIRROR_DUMMY        = 70838,

    // air phase
    SPELL_INCITE_TERROR             = 73070,
    SPELL_BLOODBOLT_WHIRL           = 71772,

    // others
    // NPC_SWARMING_SHADOWS          = 38163,            // has aura 71267 (or 71277?)

    // encounter phases
    PHASE_GROUND                    = 1,
    PHASE_RUNNING                   = 2,
    PHASE_AIR                       = 3,
    PHASE_FLYING                    = 4,

    // movement points
    POINT_CENTER_GROUND             = 1,
    POINT_CENTER_AIR                = 2
};

static uint32 GetEssenceSpell(const Unit* unit)
{
    switch (unit->GetMap()->GetDifficulty())
    {
        case RAID_DIFFICULTY_25MAN_NORMAL: return SPELL_ESSENCE_25_NORMAL;
        case RAID_DIFFICULTY_10MAN_HEROIC: return SPELL_ESSENCE_10_HEROIC;
        case RAID_DIFFICULTY_25MAN_HEROIC: return SPELL_ESSENCE_25_HEROIC;
        default: return SPELL_ESSENCE_10_NORMAL;
    }
}

static uint32 GetFrenziedSpell(const Unit* unit)
{
    return unit->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ||
        unit->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC ? SPELL_FRENZIED_25 : SPELL_FRENZIED_10;
}

static bool IsVampire(const Unit* unit)
{
    return unit->HasAura(SPELL_ESSENCE_10_NORMAL) || unit->HasAura(SPELL_ESSENCE_25_NORMAL) ||
        unit->HasAura(SPELL_ESSENCE_10_HEROIC) || unit->HasAura(SPELL_ESSENCE_25_HEROIC) ||
        unit->HasAura(SPELL_FRENZIED_10) || unit->HasAura(SPELL_FRENZIED_25);
}

static const float aQueenPosition[2][3] =
{
    {4595.64f, 2769.19f, 400.13f},
    {4595.90f, 2769.31f, 421.83f},
};

struct boss_blood_queen_lanathelAI : public ScriptedAI
{
    boss_blood_queen_lanathelAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint8 m_uiPhase;
    uint32 m_uiPhaseTimer;

    uint32 m_uiBloodMirrorTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiVampiricBiteTimer;
    uint32 m_uiBloodboltTimer;
    uint32 m_uiPactDarkfallenTimer;
    uint32 m_uiSwarmingShadowsTimer;
    uint32 m_uiDeliriousSlashTimer;

    ObjectGuid m_offTankGuid;

    void RemoveEncounterAuras()
    {
        if (!m_creature->GetMap())
            return;

        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
        {
            Player* player = playerRef.getSource();
            if (!player)
                continue;

            uint32 const auras[] = { SPELL_ESSENCE_10_NORMAL, SPELL_ESSENCE_25_NORMAL, SPELL_ESSENCE_10_HEROIC,
                SPELL_ESSENCE_25_HEROIC, SPELL_FRENZIED_10, SPELL_FRENZIED_25, SPELL_UNCONTROLLABLE_FRENZY,
                SPELL_BLOOD_MIRROR_DAMAGE, SPELL_BLOOD_MIRROR_VISUAL, SPELL_BLOOD_MIRROR_DUMMY,
                SPELL_PACT_OF_THE_DARKFALLEN };
            for (uint32 aura : auras)
                player->RemoveAurasDueToSpell(aura);
        }
    }

    std::vector<Player*> GetEligiblePlayers(bool excludeTank = false, bool excludeVampires = false) const
    {
        std::vector<Player*> players;
        Unit* tank = m_creature->GetVictim();
        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
        {
            Player* player = playerRef.getSource();
            if (!player || !player->IsAlive() || !m_creature->CanAttack(player) || !m_creature->IsWithinDistInMap(player, 200.0f))
                continue;
            if (excludeTank && player == tank)
                continue;
            if (excludeVampires && IsVampire(player))
                continue;
            players.push_back(player);
        }
        return players;
    }

    void Reset() override
    {
        m_uiPhase                = PHASE_GROUND;
        m_uiPhaseTimer           = 120000;                  // 2 min

        m_uiEnrageTimer          = 330000;                  // 5 min and 30 secs
        m_uiBloodMirrorTimer     = 0;
        m_uiDeliriousSlashTimer  = 20000;
        m_uiVampiricBiteTimer    = 15000;
        m_uiBloodboltTimer       = urand(15000, 20000);
        m_uiPactDarkfallenTimer  = 15000;
        m_uiSwarmingShadowsTimer = 30000;

        m_offTankGuid.Clear();
        RemoveEncounterAuras();

        m_creature->SetAnimTier(AnimTier::Ground);
    }

    void JustReachedHome() override
    {
        RemoveEncounterAuras();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_QUEEN_LANATHEL, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoBroadcastText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoBroadcastText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_SHROUD_OF_SORROW, CAST_TRIGGERED);
        RemoveEncounterAuras();

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_QUEEN_LANATHEL, IN_PROGRESS);

            if (m_pInstance->IsHeroicDifficulty())
                DoCastSpellIfCan(m_creature, SPELL_PRESENCE_OF_DARKFALLEN, CAST_TRIGGERED);
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);
        RemoveEncounterAuras();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_QUEEN_LANATHEL, DONE);
    }

    void MovementInform(uint32 uiMovementType, uint32 uiPointId) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_CENTER_GROUND)
        {
            if (m_uiPhase == PHASE_RUNNING)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_INCITE_TERROR) == CAST_OK)
                {
                    m_uiPhase = PHASE_FLYING;

                    m_creature->SetLevitate(true);
                    m_creature->SetAnimTier(AnimTier::Hover);

                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(POINT_CENTER_AIR, aQueenPosition[1][0], aQueenPosition[1][1], aQueenPosition[1][2]);
                }
            }
            else if (m_uiPhase == PHASE_FLYING)
            {
                m_uiPhase      = PHASE_GROUND;
                m_uiPhaseTimer = 120000;
                SetCombatMovement(true);

                m_creature->SetLevitate(false);
                m_creature->SetAnimTier(AnimTier::Ground);

                m_creature->GetMotionMaster()->Clear();
                if (m_creature->GetVictim())
                    m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());
            }
        }
        else if (uiPointId == POINT_CENTER_AIR)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLOODBOLT_WHIRL) == CAST_OK)
            {
                DoBroadcastText(SAY_AIR_PHASE, m_creature);
                m_uiPhase      = PHASE_AIR;
                m_uiPhaseTimer = 7000;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiEnrageTimer)
        {
            if (m_uiEnrageTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoBroadcastText(SAY_BERSERK, m_creature);
                    m_uiEnrageTimer = 0;
                }
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        switch (m_uiPhase)
        {
            case PHASE_GROUND:
            {
                // Air phase change timer
                if (m_uiPhaseTimer < uiDiff)
                {
                    SetCombatMovement(false);
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(POINT_CENTER_GROUND, aQueenPosition[0][0], aQueenPosition[0][1], aQueenPosition[0][2]);

                    m_uiPhase      = PHASE_RUNNING;
                    m_uiPhaseTimer = 0;
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                // Only one bite per fight
                if (m_uiVampiricBiteTimer)
                {
                    if (m_uiVampiricBiteTimer <= uiDiff)
                    {
                        std::vector<Player*> targets = GetEligiblePlayers(true, true);
                        if (targets.empty())
                            targets = GetEligiblePlayers(false, true);
                        Player* target = targets.empty() ? nullptr : targets[urand(0, targets.size() - 1)];
                        if (target && DoCastSpellIfCan(target, 71726) == CAST_OK)
                        {
                            target->CastSpell(target, GetEssenceSpell(target), TRIGGERED_OLD_TRIGGERED);
                            DoBroadcastText(urand(0, 1) ? SAY_BITE_1 : SAY_BITE_2, m_creature);
                            m_uiVampiricBiteTimer = 0;
                        }
                    }
                    else
                        m_uiVampiricBiteTimer -= uiDiff;
                }

                if (m_uiBloodMirrorTimer < uiDiff)
                {
                    std::vector<Player*> targets = GetEligiblePlayers(true, false);
                    if (!targets.empty() && m_creature->GetVictim())
                    {
                        std::sort(targets.begin(), targets.end(), [this](Player* left, Player* right)
                        {
                            return m_creature->GetVictim()->GetDistance(left) < m_creature->GetVictim()->GetDistance(right);
                        });
                        Player* offTank = targets.front();
                        if (m_offTankGuid != offTank->GetObjectGuid())
                        {
                            m_offTankGuid = offTank->GetObjectGuid();
                            offTank->CastSpell(m_creature->GetVictim(), SPELL_BLOOD_MIRROR_DAMAGE, TRIGGERED_OLD_TRIGGERED);
                            m_creature->GetVictim()->CastSpell(offTank, SPELL_BLOOD_MIRROR_DUMMY, TRIGGERED_OLD_TRIGGERED);
                            m_creature->CastSpell(m_creature->GetVictim(), SPELL_BLOOD_MIRROR_VISUAL, TRIGGERED_OLD_TRIGGERED);
                        }
                    }
                    m_uiBloodMirrorTimer = 2500;
                }
                else
                    m_uiBloodMirrorTimer -= uiDiff;

                if (m_uiBloodboltTimer < uiDiff)
                {
                    std::vector<Player*> targets = GetEligiblePlayers(true, false);
                    uint32 count = m_pInstance && m_pInstance->Is25ManDifficulty() ? 4 : 2;
                    while (count-- && !targets.empty())
                    {
                        uint32 index = urand(0, targets.size() - 1);
                        m_creature->CastSpell(targets[index], 71446, TRIGGERED_OLD_TRIGGERED);
                        targets.erase(targets.begin() + index);
                    }
                    m_uiBloodboltTimer = urand(10000, 15000);
                }
                else
                    m_uiBloodboltTimer -= uiDiff;

                if (m_uiPactDarkfallenTimer < uiDiff)
                {
                    std::vector<Player*> targets = GetEligiblePlayers(true, false);
                    uint32 count = m_pInstance && m_pInstance->Is25ManDifficulty() ? 3 : 2;
                    if (targets.size() >= count)
                    {
                        while (count--)
                        {
                            uint32 index = urand(0, targets.size() - 1);
                            m_creature->CastSpell(targets[index], 71340, TRIGGERED_OLD_TRIGGERED);
                            targets.erase(targets.begin() + index);
                        }
                        DoBroadcastText(SAY_PACT, m_creature);
                    }
                    m_uiPactDarkfallenTimer = 30500;
                }
                else
                    m_uiPactDarkfallenTimer -= uiDiff;

                if (m_uiSwarmingShadowsTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SWARMING_SHADOWS) == CAST_OK)
                    {
                        DoBroadcastText(SAY_SHADOWS, m_creature);
                        m_uiSwarmingShadowsTimer = urand(30000, 35000);
                    }
                }
                else
                    m_uiSwarmingShadowsTimer -= uiDiff;

                // Heroic spells
                if (m_pInstance && m_pInstance->IsHeroicDifficulty())
                {
                    if (m_uiDeliriousSlashTimer < uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_DELIRIOUS_SLASH) == CAST_OK)
                            m_uiDeliriousSlashTimer = 15000;
                    }
                    else
                        m_uiDeliriousSlashTimer -= uiDiff;
                }

                DoMeleeAttackIfReady();

                break;
            }
            case PHASE_RUNNING:
            case PHASE_FLYING:
            {
                // Nothing here. Wait for arriving at the point
                break;
            }
            case PHASE_AIR:
            {
                if (m_uiPhaseTimer < uiDiff)
                {
                    m_uiPhase      = PHASE_FLYING;
                    m_uiPhaseTimer = 0;

                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(POINT_CENTER_GROUND, aQueenPosition[0][0], aQueenPosition[0][1], aQueenPosition[0][2]);
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                break;
            }
        }
    }
};

// Player bite: consume Bloodthirst, make the bitten target a vampire, and keep
// the biter from being mind controlled when the bite succeeds.
struct spell_blood_queen_vampiric_bite : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        return target && !IsVampire(target) ? SPELL_CAST_OK : SPELL_FAILED_BAD_TARGETS;
    }

    void OnHit(Spell* spell, SpellMissInfo missInfo) const override
    {
        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (!caster || !target || missInfo != SPELL_MISS_NONE)
            return;

        caster->RemoveAurasDueToSpell(GetFrenziedSpell(caster), nullptr, AURA_REMOVE_BY_DEFAULT);
        caster->CastSpell(caster, GetEssenceSpell(caster), TRIGGERED_OLD_TRIGGERED);
        target->CastSpell(target, GetEssenceSpell(target), TRIGGERED_OLD_TRIGGERED);
        target->CastSpell(target, 71952, TRIGGERED_OLD_TRIGGERED);
    }
};

struct spell_blood_queen_frenzied_bloodthirst : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
        {
            Unit* target = aura->GetTarget();
            if (target && !target->HasAura(GetEssenceSpell(target)))
                target->CastSpell(target, SPELL_UNCONTROLLABLE_FRENZY, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

UnitAI* GetAI_boss_blood_queen_lanathel(Creature* pCreature)
{
    return new boss_blood_queen_lanathelAI(pCreature);
}

void AddSC_boss_blood_queen_lanathel()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_blood_queen_lanathel";
    pNewScript->GetAI = &GetAI_boss_blood_queen_lanathel;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_blood_queen_vampiric_bite>("spell_blood_queen_vampiric_bite");
    RegisterSpellScript<spell_blood_queen_frenzied_bloodthirst>("spell_blood_queen_frenzied_bloodthirst");
}

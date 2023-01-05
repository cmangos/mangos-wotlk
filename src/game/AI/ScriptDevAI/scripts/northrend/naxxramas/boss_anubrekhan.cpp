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
SDName: Boss_Anubrekhan
SD%Complete: 95
SDComment: Intro text usage is not very clear. Requires additional research.
SDCategory: Naxxramas
EndScriptData */

#include "AI/ScriptDevAI/base/BossAI.h"
#include "AI/ScriptDevAI/include/sc_common.h"
#include "naxxramas.h"
enum
{
    SAY_GREET                   = 13004,
    SAY_AGGRO1                  = 13000,
    SAY_AGGRO2                  = 13002,
    SAY_AGGRO3                  = 13003,
    SAY_TAUNT1                  = 13006,
    SAY_TAUNT2                  = 13007,
    SAY_TAUNT3                  = 13008,
    SAY_TAUNT4                  = 13009,
    SAY_SLAY                    = 13005,

    EMOTE_CRYPT_GUARD           = 29887,
    EMOTE_INSECT_SWARM          = 13443,
    EMOTE_CORPSE_SCARABS        = 32796,

    SPELL_DOUBLE_ATTACK         = 18943,
    SPELL_IMPALE                = 28783,                    // May be wrong spell id. Causes more dmg than I expect
    SPELL_IMPALE_H              = 56090,
    SPELL_LOCUSTSWARM           = 28785,                    // This is a self buff that triggers the dmg debuff
    SPELL_LOCUSTSWARM_H         = 54021,

    SPELL_SUMMON_GUARD          = 29508,                    // Summons 1 crypt guard at targeted location

    SPELL_SUMMON_CORPSE_SCARABS_5   = 29105,                // This spawns 5 corpse scarabs ontop of us (most likely the pPlayer casts this on death)
    SPELL_SUMMON_CORPSE_SCARABS_10  = 28864,                // This is used by the crypt guards when they die

    // NPC_CRYPT_GUARD             = 16573,

    SPELLSET_10N                = 1595601,
    SPELLSET_25N                = 2924901,
};

enum AnubRekhanActions
{
    ANUBREKHAN_ACTIONS_MAX,
    ANUBREKHAN_SUMMON,
};

struct boss_anubrekhanAI : public BossAI
{
    boss_anubrekhanAI(Creature* creature) : BossAI(creature, ANUBREKHAN_ACTIONS_MAX),
        m_instance(static_cast<instance_naxxramas*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty()), m_hasTaunted(false)
    {
        SetDataType(TYPE_ANUB_REKHAN);
        AddOnAggroText(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3);
        AddOnKillText(SAY_SLAY);
        AddCustomAction(ANUBREKHAN_SUMMON, true, [&]()
        {
            DoCastSpellIfCan(nullptr, SPELL_SUMMON_GUARD);
        });
    }

    instance_naxxramas* m_instance;
    bool m_isRegularMode;
    bool m_hasTaunted;

    void Reset() override
    {
        m_creature->SetSpellList(m_isRegularMode ? SPELLSET_10N : SPELLSET_25N);
        DoCastSpellIfCan(nullptr, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit *who) override
    {
        BossAI::Aggro(who);
        if (m_isRegularMode)
            ResetTimer(ANUBREKHAN_SUMMON, 20s);
    }

    void KilledUnit(Unit* victim) override
    {
        BossAI::KilledUnit(victim);
        if (!victim)
            return;

        // Force the player to spawn corpse scarabs via spell
        if (victim->IsPlayer())
            victim->CastSpell(nullptr, SPELL_SUMMON_CORPSE_SCARABS_5, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_hasTaunted && who->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(who, 110.0f) && m_creature->IsWithinLOSInMap(who))
        {
            switch (urand(0,4))
            {
                case 0: DoBroadcastText(SAY_GREET, m_creature); break;
                case 1: DoBroadcastText(SAY_TAUNT1, m_creature); break;
                case 2: DoBroadcastText(SAY_TAUNT2, m_creature); break;
                case 3: DoBroadcastText(SAY_TAUNT3, m_creature); break;
                case 4: DoBroadcastText(SAY_TAUNT4, m_creature); break;
            }
            m_hasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_CRYPT_GUARD)
            DoBroadcastText(EMOTE_CRYPT_GUARD, summoned);

        summoned->AI()->AttackClosestEnemy();
    }

    void SummonedCreatureDespawn(Creature* summoned) override
    {
        // If creature despawns on out of combat, skip this
        if (!m_creature->IsInCombat())
            return;

        if (summoned && summoned->GetEntry() == NPC_CRYPT_GUARD)
        {
            summoned->CastSpell(summoned, SPELL_SUMMON_CORPSE_SCARABS_10, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
            DoBroadcastText(EMOTE_CORPSE_SCARABS, summoned);
        }
    }
};

void AddSC_boss_anubrekhan()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_anubrekhan";
    pNewScript->GetAI = &GetNewAIInstance<boss_anubrekhanAI>;
    pNewScript->RegisterSelf();
}

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
SDName: boss_krick_and_ick
SD%Complete: 95
SDComment: Timers may need adjustments.
SDCategory: Pit of Saron
EndScriptData */

#include "precompiled.h"
#include "pit_of_saron.h"

enum
{
    SAY_AGGRO                           = -1658024,
    SAY_SLAY_1                          = -1658025,
    SAY_SLAY_2                          = -1658026,
    SAY_ORDER_STOP                      = -1658027,
    SAY_ORDER_BLOW                      = -1658028,
    SAY_TARGET_1                        = -1658029,
    SAY_TARGET_2                        = -1658030,
    SAY_TARGET_3                        = -1658031,
    SAY_OUTRO_1                         = -1658035,

    EMOTE_KRICK_MINES                   = -1658032,
    EMOTE_ICK_POISON                    = -1658033,
    EMOTE_ICK_CHASING                   = -1658034,

    // ick spells
    SPELL_POISON_NOVA                   = 68989,
    SPELL_MIGHTY_KICK                   = 69021,
    SPELL_PURSUIT                       = 68987,
    SPELL_EXPLOSIVE_BARRAGE_ICK         = 69263,

    // krick spells
    SPELL_TOXIC_WASTE                   = 69024,
    SPELL_SHADOW_BOLT                   = 69028,
    SPELL_EXPLOSIVE_BARRAGE_KRICK       = 69012,                // Triggers 69015 every 2 sec

    NPC_EXPLODING_ORB                   = 36610,

    // exploding orb spells
    // SPELL_EXPLOSIVE_BARRAGE_SUMMON   = 69015,
    SPELL_EXPLODING_ORB_VISUAL          = 69017,
    SPELL_AUTO_GROW_AND_SPEED_BOOST     = 69020,
    SPELL_EXPLOSIVE_BARRAGE_DMG         = 69019,
    SPELL_HASTY_GROW                    = 44851,                // Orb explodes after the 15th stack

    MAX_HASTY_GROW_STACKS               = 15,
};

static const float afOutroNpcSpawnLoc[4] = {777.2274f, 119.5521f, 510.0363f, 6.05f};
static const float afTyrannusTeleLoc[4] = {841.01f, 196.245f, 573.964f, 4.46f};

/*######
## boss_ick
######*/

struct boss_ickAI : public ScriptedAI
{
    boss_ickAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pit_of_saron*)pCreature->GetInstanceData();
        m_uiMountTimer = 1000;
        Reset();
    }

    instance_pit_of_saron* m_pInstance;
    uint32 m_uiMountTimer;

    uint32 m_uiPoisonNovaTimer;
    uint32 m_uiPursueTimer;
    uint32 m_uiMightKickTimer;
    uint32 m_uiToxicWasteTimer;
    uint32 m_uiShadowboltTimer;
    uint32 m_uiExplosivBarrageTimer;
    uint32 m_uiCooldownTimer;

    void Reset() override
    {
        m_uiPoisonNovaTimer      = urand(20000, 25000);
        m_uiPursueTimer          = 20000;
        m_uiMightKickTimer       = 1000;
        m_uiToxicWasteTimer      = urand(3000, 5000);
        m_uiShadowboltTimer      = urand(5000, 7000);
        m_uiExplosivBarrageTimer = urand(30000, 35000);
        m_uiCooldownTimer        = 0;
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_KRICK, IN_PROGRESS);

            // Say aggro and also put Krick in combat
            if (Creature* pKrick = m_pInstance->GetSingleCreatureFromStorage(NPC_KRICK))
            {
                DoScriptText(SAY_AGGRO, pKrick);
                pKrick->AI()->AttackStart(pWho);
            }
        }
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        if (m_pInstance)
        {
            if (Creature* pKrick = m_pInstance->GetSingleCreatureFromStorage(NPC_KRICK))
                DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, pKrick);
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_KRICK, DONE);

            if (Creature* pKrick = m_pInstance->GetSingleCreatureFromStorage(NPC_KRICK))
            {
                DoScriptText(SAY_OUTRO_1, pKrick);
                pKrick->AI()->EnterEvadeMode();

                // Summon Jaina or Sylvanas for epilogue
                pKrick->SummonCreature(m_pInstance->GetPlayerTeam() == HORDE ? NPC_SYLVANAS_PART1 : NPC_JAINA_PART1,
                                       afOutroNpcSpawnLoc[0], afOutroNpcSpawnLoc[1], afOutroNpcSpawnLoc[2], afOutroNpcSpawnLoc[3], TEMPSUMMON_TIMED_DESPAWN, 2 * MINUTE * IN_MILLISECONDS);
            }

            if (Creature* pTyrannus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS_INTRO))
                pTyrannus->NearTeleportTo(afTyrannusTeleLoc[0], afTyrannusTeleLoc[1], afTyrannusTeleLoc[2], afTyrannusTeleLoc[3]);
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KRICK, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_pInstance)
            return;

        // He needs to be mounted manually, not by vehicle_accessories
        if (m_uiMountTimer)
        {
            if (m_uiMountTimer <= uiDiff)
            {
                if (Creature* pKrick = m_pInstance->GetSingleCreatureFromStorage(NPC_KRICK))
                    pKrick->CastSpell(m_creature, SPELL_RIDE_VEHICLE_HARDCODED, true);

                m_uiMountTimer = 0;
            }
            else
                m_uiMountTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cooldown timer - we need to block all Krick spell during some events
        if (m_uiCooldownTimer)
        {
            if (m_uiCooldownTimer <= uiDiff)
                m_uiCooldownTimer = 0;
            else
                m_uiCooldownTimer -= uiDiff;

            return;
        }

        if (m_uiPoisonNovaTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_POISON_NOVA) == CAST_OK)
            {
                if (Creature* pKrick = m_pInstance->GetSingleCreatureFromStorage(NPC_KRICK))
                {
                    DoScriptText(SAY_ORDER_BLOW, pKrick);
                    DoScriptText(EMOTE_ICK_POISON, pKrick);
                }

                m_uiCooldownTimer = 5000;
                m_uiPoisonNovaTimer = urand(20000, 25000);
            }
        }
        else
            m_uiPoisonNovaTimer -= uiDiff;

        if (m_uiPursueTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_PURSUIT) == CAST_OK)
            {
                if (Creature* pKrick = m_pInstance->GetSingleCreatureFromStorage(NPC_KRICK))
                {
                    switch (urand(0, 2))
                    {
                        case 0: DoScriptText(SAY_TARGET_1, pKrick); break;
                        case 1: DoScriptText(SAY_TARGET_2, pKrick); break;
                        case 2: DoScriptText(SAY_TARGET_3, pKrick); break;
                    }
                }

                m_uiCooldownTimer = 17000;
                m_uiPursueTimer = urand(50000, 70000);
            }
        }
        else
            m_uiPursueTimer -= uiDiff;

        if (m_uiMightKickTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIGHTY_KICK) == CAST_OK)
                m_uiMightKickTimer = 10000;
        }
        else
            m_uiMightKickTimer -= uiDiff;

        if (m_uiToxicWasteTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (Creature* pKrick = m_pInstance->GetSingleCreatureFromStorage(NPC_KRICK))
                    pKrick->CastSpell(pTarget, SPELL_TOXIC_WASTE, true);

                m_uiToxicWasteTimer = urand(3000, 5000);
            }
        }
        else
            m_uiToxicWasteTimer -= uiDiff;

        if (m_uiShadowboltTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (Creature* pKrick = m_pInstance->GetSingleCreatureFromStorage(NPC_KRICK))
                    pKrick->CastSpell(pTarget, SPELL_SHADOW_BOLT, true);

                m_uiShadowboltTimer = urand(4000, 8000);
            }
        }
        else
            m_uiShadowboltTimer -= uiDiff;

        if (m_uiExplosivBarrageTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_EXPLOSIVE_BARRAGE_ICK) == CAST_OK)
            {
                if (Creature* pKrick = m_pInstance->GetSingleCreatureFromStorage(NPC_KRICK))
                {
                    pKrick->CastSpell(pKrick, SPELL_EXPLOSIVE_BARRAGE_KRICK, true);

                    DoScriptText(SAY_ORDER_STOP, pKrick);
                    DoScriptText(EMOTE_KRICK_MINES, pKrick);
                }

                m_uiCooldownTimer = 20000;
                m_uiExplosivBarrageTimer = urand(25000, 30000);
            }
        }
        else
            m_uiExplosivBarrageTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ick(Creature* pCreature)
{
    return new boss_ickAI(pCreature);
}

/*######
## boss_krick
######*/

struct boss_krickAI : public ScriptedAI
{
    boss_krickAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() override { }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(true);

        m_creature->SetLootRecipient(NULL);

        Reset();

        // Don't handle movement. Boss is on vehicle so he doesn't have to go anywhere. On epilogue he needs to stay in place
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_SYLVANAS_PART1:
            case NPC_JAINA_PART1:
            {
                float fX, fY, fZ;
                pSummoned->SetWalk(false);
                m_creature->GetContactPoint(pSummoned, fX, fY, fZ, 2 * INTERACTION_DISTANCE);
                pSummoned->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                break;
            }
            case NPC_EXPLODING_ORB:
                pSummoned->CastSpell(pSummoned, SPELL_EXPLODING_ORB_VISUAL, true);
                pSummoned->CastSpell(pSummoned, SPELL_AUTO_GROW_AND_SPEED_BOOST, true);
                break;
        }
    }

    void SummonedMovementInform(Creature* /*pSummoned*/, uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
            return;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KRICK, SPECIAL);
    }

    void UpdateAI(const uint32 /*uiDiff*/) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }
};

CreatureAI* GetAI_boss_krick(Creature* pCreature)
{
    return new boss_krickAI(pCreature);
}

/*######
## npc_exploding_orb
######*/

struct npc_exploding_orbAI : public Scripted_NoMovementAI
{
    npc_exploding_orbAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint8 m_uiGrowCount;

    void Reset() override
    {
        m_uiGrowCount = 0;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_HASTY_GROW)
        {
            ++m_uiGrowCount;

            if (m_uiGrowCount == MAX_HASTY_GROW_STACKS)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_EXPLOSIVE_BARRAGE_DMG) == CAST_OK)
                {
                    m_creature->RemoveAllAuras();
                    m_creature->ForcedDespawn(1000);
                }
            }
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_exploding_orb(Creature* pCreature)
{
    return new npc_exploding_orbAI(pCreature);
}

void AddSC_boss_krick_and_ick()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_ick";
    pNewScript->GetAI = &GetAI_boss_ick;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_krick";
    pNewScript->GetAI = &GetAI_boss_krick;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_exploding_orb";
    pNewScript->GetAI = &GetAI_npc_exploding_orb;
    pNewScript->RegisterSelf();
}

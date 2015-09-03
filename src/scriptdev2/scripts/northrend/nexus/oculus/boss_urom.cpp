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
SDName: boss_urom
SD%Complete: 90
SDComment: Small adjustments may be required.
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"
#include "oculus.h"

enum
{
    SAY_SUMMON_1                    = -1578000,
    SAY_SUMMON_2                    = -1578001,
    SAY_SUMMON_3                    = -1578002,
    SAY_AGGRO                       = -1578003,
    SAY_EXPLOSION_1                 = -1578004,
    SAY_EXPLOSION_2                 = -1578005,
    SAY_KILL_1                      = -1578006,
    SAY_KILL_2                      = -1578007,
    SAY_KILL_3                      = -1578008,
    SAY_DEATH                       = -1578009,
    EMOTE_EXPLOSION                 = -1578025,

    // spells
    SPELL_ARCANE_SHIELD             = 53813,                // This spell id may be wrong. Needs research!
    SPELL_ARCANE_EXPLOSION          = 51110,
    SPELL_ARCANE_EXPLOSION_H        = 59377,
    SPELL_FROSTBOMB                 = 51103,
    SPELL_TIME_BOMB                 = 51121,
    SPELL_TIME_BOMB_H               = 59376,
    SPELL_SUMMON_MENAGERIE_1        = 50476,
    SPELL_SUMMON_MENAGERIE_2        = 50495,
    SPELL_SUMMON_MENAGERIE_3        = 50496,
    SPELL_TELEPORT                  = 51112,

    // npcs
    NPC_PHANTASMAL_CLOUDSCRAPER     = 27645,
    NPC_PHANTASMAL_MAMMOTH          = 27642,
    NPC_PHANTASMAL_WOLF             = 27644,

    NPC_PHANTASMAL_AIR              = 27650,
    NPC_PHANTASMAL_FIRE             = 27651,
    NPC_PHANTASMAL_WATER            = 27653,

    NPC_PHANTASMAL_MURLOC           = 27649,
    NPC_PHANTASMAL_NAGAL            = 27648,
    NPC_PHANTASMAL_OGRE             = 27647,

    MAX_PLATFORMS                   = 3,
};

static uint32 uiTrashPacks[MAX_PLATFORMS][MAX_PLATFORMS] =
{
    {NPC_PHANTASMAL_CLOUDSCRAPER,   NPC_PHANTASMAL_MAMMOTH, NPC_PHANTASMAL_WOLF},
    {NPC_PHANTASMAL_AIR,            NPC_PHANTASMAL_FIRE,    NPC_PHANTASMAL_WATER},
    {NPC_PHANTASMAL_MURLOC,         NPC_PHANTASMAL_NAGAL,   NPC_PHANTASMAL_OGRE},
};

struct boss_uromAI : public ScriptedAI
{
    boss_uromAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        // Randomize the trash mobs packs
        for (uint8 i = 0; i < MAX_PLATFORMS; ++i)
            m_vuiTrashPacksIds.push_back(i);

        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsTeleporting;
    bool m_bIsPlatformPhase;
    uint8 m_uiPlatformPhase;
    uint32 m_uiExplosionExpireTimer;
    uint32 m_uiArcaneShieldTimer;
    uint32 m_uiExplosionTimer;
    uint32 m_uiTeleportTimer;
    uint32 m_uiFrostBombTimer;
    uint32 m_uiTimeBombTimer;

    float m_fX, m_fY, m_fZ;

    ObjectGuid m_attackTarget;

    std::vector<uint32> m_vuiTrashPacksIds;

    void Reset() override
    {
        m_bIsPlatformPhase       = true;
        m_uiPlatformPhase        = 0;
        m_uiExplosionTimer       = 0;
        m_uiExplosionExpireTimer = 0;
        m_uiTeleportTimer        = 20000;
        m_uiFrostBombTimer       = 5000;
        m_uiTimeBombTimer        = urand(10000, 15000);

        ResetPlatformVariables();

        std::random_shuffle(m_vuiTrashPacksIds.begin(), m_vuiTrashPacksIds.end());
    }

    void ResetPlatformVariables()
    {
        m_bIsTeleporting        = false;
        m_uiArcaneShieldTimer   = 1000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_UROM, IN_PROGRESS);
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_uiPlatformPhase < MAX_PLATFORMS)
        {
            if (m_bIsTeleporting)
                return;

            // Summon the trash mobs pack
            m_bIsTeleporting = true;
            m_attackTarget = pWho->GetObjectGuid();
            m_creature->InterruptNonMeleeSpells(false);
            DoSpawnTrashPack();

            // teleport to next platform and spawn adds
            switch (m_uiPlatformPhase)
            {
                case 0:
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_MENAGERIE_1) == CAST_OK)
                        DoScriptText(SAY_SUMMON_1, m_creature);
                    break;
                case 1:
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_MENAGERIE_2) == CAST_OK)
                        DoScriptText(SAY_SUMMON_2, m_creature);
                    break;
                case 2:
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_MENAGERIE_3) == CAST_OK)
                        DoScriptText(SAY_SUMMON_3, m_creature);
                    break;
            }
        }
        // Boss has teleported in the central ring - start normal combat
        else if (m_bIsPlatformPhase)
        {
            DoScriptText(SAY_AGGRO, m_creature);
            m_creature->InterruptNonMeleeSpells(false);
            m_bIsPlatformPhase = false;

            ScriptedAI::AttackStart(pWho);
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_DEATH_SPELL, CAST_TRIGGERED);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_UROM, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_UROM, FAIL);
    }

    void EnterEvadeMode() override
    {
        // Don't evade while casting explosion
        if (m_uiExplosionExpireTimer)
            return;

        if (m_bIsPlatformPhase)
        {
            m_creature->RemoveAllAurasOnEvade();
            m_creature->DeleteThreatList();
            m_creature->CombatStop(true);

            m_creature->SetLootRecipient(NULL);

            ResetPlatformVariables();
        }
        else
        {
            // Teleport to home position, in order to override the movemaps
            m_creature->NearTeleportTo(aOculusBossSpawnLocs[0][0], aOculusBossSpawnLocs[0][1], aOculusBossSpawnLocs[0][2], aOculusBossSpawnLocs[0][3]);

            ScriptedAI::EnterEvadeMode();
        }
    }

    void JustSummoned(Creature* pSummon) override
    {
        if (Unit* pTarget = m_creature->GetMap()->GetUnit(m_attackTarget))
            pSummon->AI()->AttackStart(pTarget);
    }

    void DoSpawnTrashPack()
    {
        float fX, fY, fZ;

        // Summon the 3 mobs contained in the pack
        for (uint8 i = 0; i < MAX_PLATFORMS; ++i)
        {
            m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, 10.0f, M_PI_F / 2 * i);
            m_creature->SummonCreature(uiTrashPacks[m_vuiTrashPacksIds[m_uiPlatformPhase]][i], fX, fY, fZ, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 30000);
        }

        // Summon a fourth mob, which can be random
        uint32 uiEntry = uiTrashPacks[m_vuiTrashPacksIds[m_uiPlatformPhase]][urand(0, 2)];
        m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, 10.0f, M_PI_F / 2 * 3);
        m_creature->SummonCreature(uiEntry, fX, fY, fZ, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 30000);
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        switch (pSpell->Id)
        {
            case SPELL_SUMMON_MENAGERIE_3:
            case SPELL_SUMMON_MENAGERIE_2:
            case SPELL_SUMMON_MENAGERIE_1:
                EnterEvadeMode();
                ++m_uiPlatformPhase;
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Set the Arcane Shield on out of combat timer
        if (m_uiArcaneShieldTimer)
        {
            if (m_uiArcaneShieldTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ARCANE_SHIELD) == CAST_OK)
                    m_uiArcaneShieldTimer = 0;
            }
            else
                m_uiArcaneShieldTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Don't use any combat abilities during the platform transition
        if (m_bIsPlatformPhase)
            return;

        if (m_uiExplosionTimer)
        {
            if (m_uiExplosionTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_EXPLOSION : SPELL_ARCANE_EXPLOSION_H) == CAST_OK)
                {
                    DoScriptText(EMOTE_EXPLOSION, m_creature);
                    m_uiExplosionTimer = 0;
                }
            }
            else
                m_uiExplosionTimer -= uiDiff;
        }

        if (m_uiExplosionExpireTimer)
        {
            if (m_uiExplosionExpireTimer <= uiDiff)
            {
                // Teleport to the original location
                m_creature->NearTeleportTo(m_fX, m_fY, m_fZ, 0);

                // Resume combat movement
                SetCombatMovement(true);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                m_uiExplosionExpireTimer = 0;
            }
            else
                m_uiExplosionExpireTimer -= uiDiff;

            // Don't decrease timers during the explosion event
            return;
        }

        if (m_uiTeleportTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TELEPORT) == CAST_OK)
            {
                DoScriptText(urand(0, 1) ? SAY_EXPLOSION_1 : SAY_EXPLOSION_2, m_creature);

                // Store the original position - boss needs to be teleported back
                m_creature->GetPosition(m_fX, m_fY, m_fZ);

                // Stop movement until he casts the arcane explosion
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->MoveIdle();
                m_uiTeleportTimer = 20000;
                m_uiExplosionExpireTimer = m_bIsRegularMode ? 9500 : 7500;
                m_uiExplosionTimer = 1000;
            }
        }
        else
            m_uiTeleportTimer -= uiDiff;

        if (m_uiFrostBombTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROSTBOMB) == CAST_OK)
                m_uiFrostBombTimer = urand(4000, 6000);
        }
        else
            m_uiFrostBombTimer -= uiDiff;

        if (m_uiTimeBombTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_TIME_BOMB : SPELL_TIME_BOMB_H) == CAST_OK)
                    m_uiTimeBombTimer = urand(10000, 15000);
            }
        }
        else
            m_uiTimeBombTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_urom(Creature* pCreature)
{
    return new boss_uromAI(pCreature);
}

void AddSC_boss_urom()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_urom";
    pNewScript->GetAI = &GetAI_boss_urom;
    pNewScript->RegisterSelf();
}

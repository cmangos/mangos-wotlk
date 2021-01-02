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
SDName: Boss Volkhan
SD%Complete: 80%
SDComment: The dummy spells need more research and should be handled in core
SDCategory: Halls of Lightning
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "halls_of_lightning.h"

enum
{
    SAY_AGGRO                               = -1602032,
    SAY_SLAY_1                              = -1602033,
    SAY_SLAY_2                              = -1602034,
    SAY_SLAY_3                              = -1602035,
    SAY_DEATH                               = -1602036,
    SAY_STOMP_1                             = -1602037,
    SAY_STOMP_2                             = -1602038,
    SAY_FORGE_1                             = -1602039,
    SAY_FORGE_2                             = -1602040,
    EMOTE_TO_ANVIL                          = -1602041,
    EMOTE_SHATTER                           = -1602042,

    SPELL_HEAT                              = 52387,
    SPELL_HEAT_H                            = 59528,
    SPELL_SHATTERING_STOMP                  = 52237,
    SPELL_SHATTERING_STOMP_H                = 59529,

    // unclear how "directions" of spells must be. Last, summoning GO, what is it for? Script depend on:
    SPELL_TEMPER                            = 52238,        // TARGET_SCRIPT boss->anvil
    SPELL_TEMPER_DUMMY                      = 52654,        // TARGET_SCRIPT anvil->boss
    // SPELL_TEMPER_VISUAL                   = 52661,       // summons GO

    SPELL_SUMMON_MOLTEN_GOLEM               = 52405,

    // Molten Golem
    SPELL_BLAST_WAVE                        = 23113,
    SPELL_IMMOLATION_STRIKE                 = 52433,
    SPELL_IMMOLATION_STRIKE_H               = 59530,
    SPELL_SHATTER                           = 52429,
    SPELL_SHATTER_H                         = 59527,
    SPELL_COOL_DOWN                         = 52441,

    NPC_MOLTEN_GOLEM                        = 28695,
    NPC_BRITTLE_GOLEM                       = 28681,

    MAX_GOLEM                               = 2,
    MAX_ACHIEV_GOLEMS                       = 4
};

/*######
## Boss Volkhan
######*/

struct boss_volkhanAI : public ScriptedAI
{
    boss_volkhanAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_halls_of_lightning*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_halls_of_lightning* m_pInstance;

    GuidList m_lGolemGUIDList;

    bool m_bIsRegularMode;
    bool m_bHasShattered;

    uint32 m_uiShatterTimer;
    uint32 m_uiHeatTimer;
    uint32 m_uiTemperTimer;

    void Reset() override
    {
        m_bHasShattered = false;

        m_uiShatterTimer = 3000;
        m_uiHeatTimer = 30000;
        m_uiTemperTimer = 10000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLKHAN, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        DespawnGolems();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLKHAN, DONE);
    }

    void JustReachedHome() override
    {
        DespawnGolems();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLKHAN, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void DespawnGolems()
    {
        if (m_lGolemGUIDList.empty())
            return;

        for (GuidList::const_iterator itr = m_lGolemGUIDList.begin(); itr != m_lGolemGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
            {
                if (pTemp->IsAlive())
                    pTemp->ForcedDespawn();
            }
        }
    }

    void ShatterGolems()
    {
        if (m_lGolemGUIDList.empty())
            return;

        uint8 m_uiBrittleGolemsCount = 0;

        for (GuidList::const_iterator itr = m_lGolemGUIDList.begin(); itr != m_lGolemGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
            {
                // only shatter brittle golems
                if (pTemp->GetEntry() == NPC_BRITTLE_GOLEM)
                {
                    pTemp->CastSpell(pTemp, m_bIsRegularMode ? SPELL_SHATTER : SPELL_SHATTER_H, TRIGGERED_OLD_TRIGGERED);
                    ++m_uiBrittleGolemsCount;
                }
            }
        }

        // If shattered more than 4 golems mark achiev as failed
        if (m_uiBrittleGolemsCount > MAX_ACHIEV_GOLEMS)
        {
            if (m_pInstance)
                m_pInstance->SetData(TYPE_VOLKHAN, SPECIAL);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_MOLTEN_GOLEM)
        {
            m_lGolemGUIDList.push_back(pSummoned->GetObjectGuid());
            pSummoned->CastSpell(pSummoned, SPELL_COOL_DOWN, TRIGGERED_OLD_TRIGGERED);

            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        DoCastSpellIfCan(m_creature, SPELL_TEMPER);
        SetCombatMovement(true);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // he shatters only one time, at 25%
        if (m_creature->GetHealthPercent() <= 25.0f && !m_bHasShattered)
        {
            // should he stomp even if he has no brittle golem to shatter? <-yes!
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SHATTERING_STOMP : SPELL_SHATTERING_STOMP_H) == CAST_OK)
            {
                DoScriptText(urand(0, 1) ? SAY_STOMP_1 : SAY_STOMP_2, m_creature);
                DoScriptText(EMOTE_SHATTER, m_creature);
                m_uiShatterTimer = 3000;
                m_bHasShattered = true;
            }
        }

        // Shatter Golems 3 seconds after Shattering Stomp
        if (m_uiShatterTimer)
        {
            if (m_uiShatterTimer <= uiDiff)
            {
                ShatterGolems();
                m_uiShatterTimer = 0;
            }
            else
                m_uiShatterTimer -= uiDiff;
        }

        // Summon Golems only when over 25% hp
        if (m_creature->GetHealthPercent() > 25.0f)
        {
            if (m_uiTemperTimer < uiDiff)
            {
                DoScriptText(EMOTE_TO_ANVIL, m_creature);
                DoScriptText(urand(0, 1) ? SAY_FORGE_1 : SAY_FORGE_2, m_creature);
                SetCombatMovement(false);

                if (m_pInstance)
                {
                    if (Creature* pAnvil = m_pInstance->GetSingleCreatureFromStorage(NPC_VOLKHAN_ANVIL))
                    {
                        float fX, fY, fZ;
                        pAnvil->GetContactPoint(m_creature, fX, fY, fZ, INTERACTION_DISTANCE);
                        m_creature->SetWalk(false);
                        m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                    }
                    else
                        script_error_log("Npc %u couldn't be found or something really bad happened.", NPC_VOLKHAN_ANVIL);
                }
                m_uiTemperTimer = 30000;
            }
            else
                m_uiTemperTimer -= uiDiff;
        }

        if (m_uiHeatTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_HEAT : SPELL_HEAT_H) == CAST_OK)
                m_uiHeatTimer = urand(10000, 15000);
            else
                m_uiHeatTimer = urand(3000, 5000);
        }
        else
            m_uiHeatTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool EffectDummyCreature_boss_volkhan(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_TEMPER_DUMMY && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCaster->GetEntry() != NPC_VOLKHAN_ANVIL || pCreatureTarget->GetEntry() != NPC_VOLKHAN)
            return true;

        for (uint8 i = 0; i < MAX_GOLEM; ++i)
            pCreatureTarget->CastSpell(pCaster, SPELL_SUMMON_MOLTEN_GOLEM, TRIGGERED_OLD_TRIGGERED);

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## npc_volkhan_anvil
######*/

bool EffectDummyCreature_npc_volkhan_anvil(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_TEMPER && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCaster->GetEntry() != NPC_VOLKHAN || pCreatureTarget->GetEntry() != NPC_VOLKHAN_ANVIL)
            return true;

        pCreatureTarget->CastSpell(pCaster, SPELL_TEMPER_DUMMY, TRIGGERED_NONE);
        // ToDo: research how the visual spell is used

        if (pCaster->GetVictim())
        {
            pCaster->GetMotionMaster()->Clear();
            pCaster->GetMotionMaster()->MoveChase(pCaster->GetVictim());
        }

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## mob_molten_golem
######*/

struct mob_molten_golemAI : public ScriptedAI
{
    mob_molten_golemAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_halls_of_lightning*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_halls_of_lightning* m_pInstance;

    bool m_bIsRegularMode;

    uint32 m_uiBlastTimer;
    uint32 m_uiImmolationTimer;

    void Reset() override
    {
        m_uiBlastTimer = 20000;
        m_uiImmolationTimer = 5000;
    }

    void EnterEvadeMode() override
    {
        // Evade but keep the current location
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(true);

        m_creature->SetLootRecipient(nullptr);

        // Update creature to Brittle Golem
        // Note: the npc has the proper flags in DB and won't engate in combat anymore
        m_creature->UpdateEntry(NPC_BRITTLE_GOLEM);
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        // Transform intro Brittle when damaged to 0 HP
        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (m_creature->IsNonMeleeSpellCasted(false))
                m_creature->InterruptNonMeleeSpells(false);

            EnterEvadeMode();
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        // This is the dummy effect of the spells - Note: should be handled as a dummy effect in core
        if (pSpell->Id == SPELL_SHATTER || pSpell->Id == SPELL_SHATTER_H)
        {
            if (m_creature->GetEntry() == NPC_BRITTLE_GOLEM)
                m_creature->Suicide();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target or if we are frozen
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiBlastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLAST_WAVE) == CAST_OK)
                m_uiBlastTimer = 20000;
        }
        else
            m_uiBlastTimer -= uiDiff;

        if (m_uiImmolationTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_IMMOLATION_STRIKE : SPELL_IMMOLATION_STRIKE_H) == CAST_OK)
                m_uiImmolationTimer = 5000;
        }
        else
            m_uiImmolationTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_volkhan()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_volkhan";
    pNewScript->GetAI = &GetNewAIInstance<boss_volkhanAI>;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_boss_volkhan;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_volkhan_anvil";
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_volkhan_anvil;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_molten_golem";
    pNewScript->GetAI = &GetNewAIInstance<mob_molten_golemAI>;
    pNewScript->RegisterSelf();
}

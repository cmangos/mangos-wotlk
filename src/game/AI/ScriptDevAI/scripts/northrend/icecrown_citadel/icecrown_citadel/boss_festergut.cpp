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
SDName: boss_festergut
SD%Complete: 90%
SDComment: Achievement NYI.
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"
#include "Spells/SpellAuras.h"

enum
{
    // generic spels
    SPELL_BERSERK               = 47008,
    SPELL_REMOVE_INOCULENT      = 69298,

    SPELL_GASTRIC_BLOAT         = 72214,            // procs 72219 on damage done
    SPELL_GAS_SPORE             = 69278,            // should trigger 69291 on surviving targets
    SPELL_VILE_GAS              = 71307,            // triggers 69240
    //SPELL_INOCULATED            = 69291,            // spell cast on players when they survive the gas spore. Requires core support

    // Blight spells
    SPELL_INHALE_BLIGHT         = 69165,
    SPELL_INHALED_BLIGHT_10     = 69166,            // 10 man aura version
    SPELL_INHALED_BLIGHT_25     = 71912,            // 25 man aura version

    // Pungent Blight
    SPELL_PUNGENT_BLIGHT        = 69195,            // reset the gas cloud

    // Gaseous Blight
    SPELL_GASEOUS_BLIGHT_INIT   = 69125,            // initial spell cast by Putricide

    // periodic auras spells
    SPELL_GASEOUS_BLIGHT_1      = 69157,            // full level gas
    SPELL_GASEOUS_BLIGHT_2      = 69162,            // medium level gas
    SPELL_GASEOUS_BLIGHT_3      = 69164,            // lower level gas

    // visual gas dummy auras
    SPELL_GASEOUS_BLIGHT_DUMMY1 = 69126,            // full level gas visual aura
    SPELL_GASEOUS_BLIGHT_DUMMY2 = 69152,            // middle level gas visual aura
    SPELL_GASEOUS_BLIGHT_DUMMY3 = 69154,            // lower level gas visual aura

    // heroic spells
    SPELL_MALLEABLE_GOO_SUMMON  = 72299,            // spawns creature 38556
    SPELL_MALLEABLE_GOO         = 72296,
    //SPELL_MALLEABLE_GOO_VISUAL  = 75845,          // ToDo: research if this is really used

    // npcs
    NPC_MALLEABLE_OOZE_STALKER  = 38556,

    // yells
    SAY_AGGRO                   = -1631082,
    SAY_BLIGHT                  = -1631083,
    SAY_BLIGHT_ROTFACE_DEAD     = -1631200,
    //SAY_SPORE                   = -1631084,
    SAY_PUNGUENT_BLIGHT         = -1631085,
    //SAY_PUNGUENT_BLIGHT_EMOTE   = -1631086,
    SAY_SLAY_1                  = -1631087,
    SAY_SLAY_2                  = -1631088,
    SAY_BERSERK                 = -1631089,
    SAY_DEATH                   = -1631090,
    SAY_PUTRICIDE_AGGRO         = -1631080,
    EMOTE_SPORES                = -1631201,
};

static const float afBalconyLocation[3] = {4324.82f, 3166.03f, 389.3831f};

/*######
## boss_festergut
######*/

struct boss_festergutAI : public ScriptedAI
{
    boss_festergutAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetMap()->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiBerserkTimer;
    uint32 m_uiInhaleBlightTimer;
    uint32 m_uiGasSporeTimer;
    uint32 m_uiGaseousBlightTimer;
    uint32 m_uiVileGasTimer;
    uint32 m_uiMalleableGooTimer;

    uint8 m_uiGaseousBlightStage;

    void Reset() override
    {
        m_uiBerserkTimer        = 5 * MINUTE * IN_MILLISECONDS;
        m_uiInhaleBlightTimer   = 34000;
        m_uiGasSporeTimer       = 20000;
        m_uiGaseousBlightTimer  = 9000;
        m_uiVileGasTimer        = 10000;
        m_uiMalleableGooTimer   = 15000;
        m_uiGaseousBlightStage  = 0;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_GASTRIC_BLOAT, CAST_TRIGGERED);

        // set encounter in progress and get professor to the balcony
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_FESTERGUT, IN_PROGRESS);

            if (Creature* pPutricide = m_pInstance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
            {
                pPutricide->SetWalk(false);
                pPutricide->GetMotionMaster()->MovePoint(101, afBalconyLocation[0], afBalconyLocation[1], afBalconyLocation[2]);

                // heroic aggro text
                if (m_pInstance->IsHeroicDifficulty() && m_pInstance->GetData(TYPE_ROTFACE) == DONE)
                    DoScriptText(SAY_PUTRICIDE_AGGRO, pPutricide);
            }
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_FESTERGUT, FAIL);

            // reset gas stalker and putricide
            if (Creature* pStalker = m_pInstance->GetSingleCreatureFromStorage(NPC_GAS_STALKER))
                pStalker->RemoveAllAurasOnEvade();

            if (Creature* pPutricide = m_pInstance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                pPutricide->AI()->EnterEvadeMode();
        }

        DoCastSpellIfCan(m_creature, SPELL_REMOVE_INOCULENT, CAST_TRIGGERED);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_FESTERGUT, DONE);

            // reset gas stalker and putricide
            if (Creature* pStalker = m_pInstance->GetSingleCreatureFromStorage(NPC_GAS_STALKER))
                pStalker->RemoveAllAurasOnEvade();

            // ToDo: research if there is any event/yell happening on boss death
            if (Creature* pPutricide = m_pInstance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                pPutricide->AI()->EnterEvadeMode();
        }

        DoScriptText(SAY_DEATH, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_REMOVE_INOCULENT, CAST_TRIGGERED);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_MALLEABLE_OOZE_STALKER)
        {
            if (!m_pInstance)
                return;

            if (Creature* pPutricide = m_pInstance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                pPutricide->CastSpell(pSummoned, SPELL_MALLEABLE_GOO, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_pInstance)
            return;

        // Berserk
        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        if (m_uiGaseousBlightTimer)
        {
            if (m_uiGaseousBlightTimer <= uiDiff)
            {
                // two stage event; first trigger all the puddle stalkers around then set the room in gas
                switch (m_uiGaseousBlightStage)
                {
                    case 0:
                        if (Creature* pProfessor = m_pInstance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                        {
                            pProfessor->HandleEmote(EMOTE_ONESHOT_TALK_NOSHEATHE);
                            pProfessor->CastSpell(pProfessor, SPELL_GASEOUS_BLIGHT_INIT, TRIGGERED_OLD_TRIGGERED);
                            DoScriptText((m_pInstance->GetData(TYPE_ROTFACE) == DONE && m_pInstance->IsHeroicDifficulty()) ? SAY_BLIGHT_ROTFACE_DEAD : SAY_BLIGHT, pProfessor);
                        }
                        m_uiGaseousBlightTimer = 1000;
                        break;
                    case 1:
                        if (DoCastSpellIfCan(m_creature, SPELL_GASEOUS_BLIGHT_1) == CAST_OK)
                            m_uiGaseousBlightTimer = 0;
                        break;
                }
                ++m_uiGaseousBlightStage;
            }
            else
                m_uiGaseousBlightTimer -= uiDiff;
        }

        // Inhale Blight and Pungent Blight
        if (m_uiInhaleBlightTimer < uiDiff)
        {
            SpellAuraHolder* pHolder = m_creature->GetSpellAuraHolder(m_pInstance->Is25ManDifficulty() ? SPELL_INHALED_BLIGHT_25 : SPELL_INHALED_BLIGHT_10);

            // inhale the gas or if already have 3 stacks - release it
            if (pHolder && pHolder->GetStackAmount() >= 3)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_PUNGENT_BLIGHT) == CAST_OK)
                {
                    DoScriptText(SAY_PUNGUENT_BLIGHT, m_creature);
                    m_uiInhaleBlightTimer = 38000;
                }
            }
            else
            {
                if (DoCastSpellIfCan(m_creature, SPELL_INHALE_BLIGHT) == CAST_OK)
                    m_uiInhaleBlightTimer = 36000;
            }
        }
        else
            m_uiInhaleBlightTimer -= uiDiff;

        // Gas Spore
        if (m_uiGasSporeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_GAS_SPORE) == CAST_OK)
            {
                DoScriptText(EMOTE_SPORES, m_creature);
                m_uiGasSporeTimer = 40000;
            }
        }
        else
            m_uiGasSporeTimer -= uiDiff;

        // Vile Gas
        if (m_uiVileGasTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_VILE_GAS) == CAST_OK)
                m_uiVileGasTimer = 30000;
        }
        else
            m_uiVileGasTimer -= uiDiff;

        // Heroic spells
        if (m_pInstance->IsHeroicDifficulty())
        {
            if (m_uiMalleableGooTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_MALLEABLE_GOO_SUMMON) == CAST_OK)
                    m_uiMalleableGooTimer = 15000;
            }
            else
                m_uiMalleableGooTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_festergut(Creature* pCreature)
{
    return new boss_festergutAI(pCreature);
}

bool EffectScriptEffectCreature_spell_inhale_blight(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_INHALE_BLIGHT && uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->GetEntry() == NPC_FESTERGUT)
    {
        instance_icecrown_citadel* pInstance = (instance_icecrown_citadel*)pCreatureTarget->GetInstanceData();
        if (!pInstance)
            return false;

        Creature* pGasStalker = pInstance->GetSingleCreatureFromStorage(NPC_GAS_STALKER);
        if (!pGasStalker)
            return false;

        if (pCreatureTarget->HasAura(SPELL_GASEOUS_BLIGHT_1))
        {
            pGasStalker->RemoveAllAurasOnEvade();
            pCreatureTarget->CastSpell(pCreatureTarget, SPELL_GASEOUS_BLIGHT_2, TRIGGERED_OLD_TRIGGERED);
        }
        else if (pCreatureTarget->HasAura(SPELL_GASEOUS_BLIGHT_2))
        {
            pGasStalker->RemoveAllAurasOnEvade();
            pCreatureTarget->CastSpell(pCreatureTarget, SPELL_GASEOUS_BLIGHT_3, TRIGGERED_OLD_TRIGGERED);
        }
        else if (pCreatureTarget->HasAura(SPELL_GASEOUS_BLIGHT_3))
            pGasStalker->RemoveAllAurasOnEvade();

        return true;
    }

    return false;
}

/*######
## npc_orange_gas_stalker
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_orange_gas_stalkerAI : public Scripted_NoMovementAI
{
    npc_orange_gas_stalkerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_orange_gas_stalker(Creature* pCreature)
{
    return new npc_orange_gas_stalkerAI(pCreature);
}

void AddSC_boss_festergut()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_festergut";
    pNewScript->GetAI = &GetAI_boss_festergut;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_spell_inhale_blight;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_orange_gas_stalker";
    pNewScript->GetAI = GetAI_npc_orange_gas_stalker;
    pNewScript->RegisterSelf();
}

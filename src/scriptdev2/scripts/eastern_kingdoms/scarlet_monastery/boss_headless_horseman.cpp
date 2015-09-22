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
SDName: boss_headless_horseman
SD%Complete: 90
SDComment: Intro and epilog are handled by DB. Script might require some fine-tune.
SDCategory: Scarlet Monastery
EndScriptData */

#include "precompiled.h"
#include "TemporarySummon.h"

enum
{
    // horseman yells
    SAY_REJOINED                = -1189023,
    SAY_BODY_DEFEAT             = -1189024,
    SAY_LOST_HEAD               = -1189025,
    SAY_CONFLAGRATION           = -1189026,
    SAY_SPROUTING_PUMPKINS      = -1189027,
    SAY_SLAY                    = -1189028,
    SAY_DEATH                   = -1189029,

    // event start yells - handled by dbscripts
    // SAY_ENTRANCE             = -1189022,
    // EMOTE_LAUGH              = -1189030,
    // SAY_PLAYER1              = -1189031,
    // SAY_PLAYER2              = -1189032,
    // SAY_PLAYER3              = -1189033,
    // SAY_PLAYER4              = -1189034,

    // normal phase spells
    SPELL_BODY_HEAD_VISUAL      = 42413,            // head visual
    SPELL_JACK_LANTERNED        = 44185,            // on killed player
    SPELL_HORSEMAN_CLEAVE       = 42587,
    SPELL_CONFLAGRATION         = 42380,            // triggers 42381
    SPELL_SUMMON_PUMPKIN        = 52236,            // triggers 42394
    SPELL_HORSEMAN_SUMMON       = 42394,            // triggered spell - used to do the text
    SPELL_CONFLAGRATION_SOUND   = 48149,
    SPELL_BODY_STAGE_1          = 42547,            // phase control spells
    SPELL_BODY_STAGE_2          = 42548,
    SPELL_BODY_STAGE_3          = 42549,

    // headless body spells
    SPELL_SEND_HEAD             = 42399,            // send event 15394 - toss head
    SPELL_WHIRLWIND             = 43116,            // triggers 43118
    SPELL_BODY_REGEN_PROC       = 42556,            // procs 42587; also adds immunity
    SPELL_BODY_REGEN            = 42403,            // change model to headless
    SPELL_BODY_REGEN_CONFUSE    = 43105,            // confuse spell

    // head spells
    SPELL_HEAD_VISUAL           = 44241,
    SPELL_HEAL_BODY             = 43306,            // heal body to 100% on rejoin
    SPELL_REQUEST_BODY          = 43101,
    SPELL_HORSEMAN_HEAD_LANDS   = 42400,            // head land visual
    // SPELL_HEAD_INVISIBLE     = 44312,            // purpose unk
    // SPELL_HEADS_BREATH       = 43207,            // purpose unk

    // pumpkin spells
    SPELL_PUMPKIN_LIFE_CYCLE    = 42280,            // visual root aura
    SPELL_PUMPKIN_AURA          = 42294,            // visual green aura
    SPELL_SPROUTING             = 42281,            // sprout delay
    SPELL_PUMPKIN_DEATH         = 42291,            // visual on sprout
    SPELL_SPROUT_BODY           = 42285,            // visual moving aura
    SPELL_SQUASH_SOUL           = 42514,

    // event end spells
    SPELL_HEAD_IS_DEAD          = 42428,            // send event 15407; triggers 42566
    SPELL_BODY_DEAD             = 42429,            // send event 15331
    SPELL_BODY_LEAVE_COMBAT     = 43805,            // send event 15407; trigger spell 42556

    // spells used for the intro or epilog (handled by DB)
    // SPELL_LAUGH                 = 43881,         // play sound 11965
    // SPELL_LAUGH_MANIACAL        = 43885,         // play sound 11975
    // SPELL_LAUGH_LOW             = 43894,         // play sound 11976
    // SPELL_RHYME_SHAKE_MEDIUM    = 42909,         // shake effect on event start
    // SPELL_RHYME_SHAKE_SMALL     = 42910,
    // SPELL_WISP_ESCAPE_MISSILE   = 43034,
    // SPELL_WISP_FLIGHT_MISSILE   = 42821,         // triggers 42818
    // SPELL_WISP_INVISIBLE        = 42823,
    // SPELL_ON_KILL_PROC          = 43877,         // procs 13567 - use unk
    // SPELL_ENRAGE_VISUAL         = 42438,         // use unk

    // creatures
    NPC_HEADLESS_HORSEMAN       = 23682,
    NPC_HEAD_OF_HORSEMAN        = 23775,
    NPC_PULSING_PUMPKIN         = 23694,            // summoned by spell 42277
    // NPC_HORSEMAN_WISP_INV    = 24034,            // probably used for the epilog
};

enum HorsemanPhase
{
    PHASE_HORSEMAN              = 1,
    PHASE_CONFLAGRATION         = 2,
    PHASE_PUMPKINS              = 3,
    PHASE_HEAD_TOSS             = 4,
};

struct boss_headless_horsemanAI : public ScriptedAI
{
    boss_headless_horsemanAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->SetWalk(false);
        m_creature->SetLevitate(true);

        m_bHorsemanLanded = false;
        Reset();
    }

    bool m_bHorsemanLanded;
    bool m_bHeadRequested;

    HorsemanPhase m_fightPhase;

    ObjectGuid m_headGuid;

    uint32 m_uiCleaveTimer;
    uint32 m_uiConflagrationTimer;
    uint32 m_uiPumpkinTimer;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_BODY_STAGE_1, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(m_creature, SPELL_BODY_HEAD_VISUAL, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

        m_fightPhase            = PHASE_HORSEMAN;
        m_uiCleaveTimer         = 3000;
        m_uiConflagrationTimer  = urand(20000, 25000);
        m_uiPumpkinTimer        = urand(35000, 40000);
        m_bHeadRequested        = false;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_bHorsemanLanded)
            ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
        {
            DoCastSpellIfCan(pVictim, SPELL_JACK_LANTERNED, CAST_TRIGGERED);
            DoScriptText(SAY_SLAY, m_creature);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_HEAD_OF_HORSEMAN)
            m_headGuid = pSummoned->GetObjectGuid();
        else if (pSummoned->GetEntry() == NPC_PULSING_PUMPKIN)
        {
            pSummoned->CastSpell(pSummoned, SPELL_SPROUTING, false);
            pSummoned->CastSpell(pSummoned, SPELL_PUMPKIN_AURA, true);
            pSummoned->CastSpell(pSummoned, SPELL_PUMPKIN_LIFE_CYCLE, true);
            pSummoned->AI()->AttackStart(m_creature->getVictim());
        }
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage) override
    {
        if (m_fightPhase != PHASE_HEAD_TOSS && uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;
            DoTossHead();
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_HORSEMAN_SUMMON)
            DoScriptText(SAY_SPROUTING_PUMPKINS, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void JustReachedHome() override
    {
        // cleanup
        m_creature->ForcedDespawn();
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        // allow attacking
        if (uiType == WAYPOINT_MOTION_TYPE && uiPointId == 15)
        {
            m_bHorsemanLanded = true;
            m_creature->SetLevitate(false);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // rejoin head on request
        if (eventType == AI_EVENT_CUSTOM_A && pInvoker->GetEntry() == NPC_HEAD_OF_HORSEMAN)
        {
            DoRejoinHead();
            pInvoker->CastSpell(m_creature, SPELL_SEND_HEAD, true);
        }
    }

    // function to handle toss head phase
    void DoTossHead()
    {
        // in the first transition; spawn the head
        if (m_creature->HasAura(SPELL_BODY_STAGE_1))
        {
            float fX, fY, fZ;
            m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 15.0f, fX, fY, fZ);
            m_creature->SummonCreature(NPC_HEAD_OF_HORSEMAN, fX, fY, fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
        }

        // make head available
        if (Creature* pHead = m_creature->GetMap()->GetCreature(m_headGuid))
            DoCastSpellIfCan(pHead, SPELL_SEND_HEAD, CAST_TRIGGERED);

        // only from second transition we start whirlwind
        if (m_creature->HasAura(SPELL_BODY_STAGE_2) || m_creature->HasAura(SPELL_BODY_STAGE_3))
            DoCastSpellIfCan(m_creature, SPELL_WHIRLWIND, CAST_TRIGGERED);

        // remove head visual and set transition phase auras
        m_creature->RemoveAurasDueToSpell(SPELL_HEAD_VISUAL);
        DoCastSpellIfCan(m_creature, SPELL_BODY_REGEN_PROC, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_BODY_REGEN, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_BODY_REGEN_CONFUSE, CAST_TRIGGERED);

        m_fightPhase = PHASE_HEAD_TOSS;
        m_bHeadRequested = false;
    }

    // function to handle the head rejoin
    void DoRejoinHead()
    {
        // remove transition auras and set the head visual
        m_creature->RemoveAurasDueToSpell(SPELL_BODY_REGEN_CONFUSE);
        m_creature->RemoveAurasDueToSpell(SPELL_BODY_REGEN);
        m_creature->RemoveAurasDueToSpell(SPELL_BODY_REGEN_PROC);
        m_creature->RemoveAurasDueToSpell(SPELL_WHIRLWIND);

        DoScriptText(SAY_REJOINED, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_BODY_HEAD_VISUAL, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

        // switch from phase 1 to phase 2
        if (m_creature->HasAura(SPELL_BODY_STAGE_1))
        {
            m_fightPhase = PHASE_CONFLAGRATION;
            m_creature->RemoveAurasDueToSpell(SPELL_BODY_STAGE_1);
            DoCastSpellIfCan(m_creature, SPELL_BODY_STAGE_2, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        }
        // switch from phase 2 to phase 3 or repeat phase 3
        else if (m_creature->HasAura(SPELL_BODY_STAGE_2) || m_creature->HasAura(SPELL_BODY_STAGE_3))
        {
            m_fightPhase = PHASE_PUMPKINS;
            m_creature->RemoveAurasDueToSpell(SPELL_BODY_STAGE_2);
            DoCastSpellIfCan(m_creature, SPELL_BODY_STAGE_3, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_PUMPKIN, CAST_TRIGGERED);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (m_fightPhase)
        {
            case PHASE_PUMPKINS:

                if (m_uiPumpkinTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_PUMPKIN) == CAST_OK)
                        m_uiPumpkinTimer = urand(35000, 40000);
                }
                else
                    m_uiPumpkinTimer -= uiDiff;

                // no break;
            case PHASE_CONFLAGRATION:

                // conflagration not happening during pumpkin phase
                if (m_fightPhase != PHASE_PUMPKINS)
                {
                    if (m_uiConflagrationTimer < uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_CONFLAGRATION) == CAST_OK)
                            {
                                DoScriptText(SAY_CONFLAGRATION, m_creature);
                                m_uiConflagrationTimer = urand(15000, 20000);
                            }
                        }
                    }
                    else
                        m_uiConflagrationTimer -= uiDiff;
                }

                // no break;
            case PHASE_HORSEMAN:

                // cleave - all phases
                if (m_uiCleaveTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_HORSEMAN_CLEAVE) == CAST_OK)
                        m_uiCleaveTimer = 5000;
                }
                else
                    m_uiCleaveTimer -= uiDiff;

                DoMeleeAttackIfReady();
                break;
            case PHASE_HEAD_TOSS:
                // rejoin head by force at 100% hp
                if (!m_bHeadRequested && m_creature->GetHealthPercent() == 100.0f)
                {
                    if (Creature* pHead = m_creature->GetMap()->GetCreature(m_headGuid))
                        SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, pHead);

                    m_bHeadRequested = true;
                }
                break;
        }
    }
};

CreatureAI* GetAI_boss_headless_horseman(Creature* pCreature)
{
    return new boss_headless_horsemanAI(pCreature);
}

bool EffectScriptEffectCreature_boss_headless_horseman(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_REQUEST_BODY && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCreatureTarget->GetEntry() == NPC_HEADLESS_HORSEMAN)
            pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);

        return true;
    }

    return false;
}

struct boss_head_of_horsemanAI : public ScriptedAI
{
    boss_head_of_horsemanAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiHeadPhase = 1;
        Reset();
    }

    uint8 m_uiHeadPhase;

    void Reset() override { }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_HEAD_IS_DEAD, CAST_TRIGGERED);

        // end the event
        if (m_creature->IsTemporarySummon())
        {
            TemporarySummon* pTemporary = (TemporarySummon*)m_creature;

            if (Unit* pHorseman = m_creature->GetMap()->GetUnit(pTemporary->GetSummonerGuid()))
            {
                pHorseman->CastSpell(pHorseman, SPELL_BODY_LEAVE_COMBAT, true);
                pHorseman->CastSpell(pHorseman, SPELL_BODY_DEAD, true);
            }
        }
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& /*uiDamage*/) override
    {
        // allow him to die the last phase
        if (m_uiHeadPhase >= 3)
            return;

        // rejoin and switch to next phase
        if (m_creature->GetHealthPercent() < float(100 - m_uiHeadPhase * 33.3f))
        {
            DoRejoinHead(false);
            ++m_uiHeadPhase;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (pInvoker->GetEntry() != NPC_HEADLESS_HORSEMAN)
            return;

        // toss head
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            // make visible
            DoScriptText(SAY_LOST_HEAD, m_creature);
            DoCastSpellIfCan(m_creature, SPELL_HORSEMAN_HEAD_LANDS, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_HEAD_VISUAL, CAST_TRIGGERED);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            // run around the graveyard
            m_creature->SetWalk(false);
            m_creature->GetMotionMaster()->MoveRandomAroundPoint(pInvoker->GetPositionX(), pInvoker->GetPositionY(), pInvoker->GetPositionZ(), 40.0f);
        }
        // rejoin head by force - body healed
        else if (eventType == AI_EVENT_CUSTOM_B)
            DoRejoinHead(true);
    }

    // rejoin the head with the body
    void DoRejoinHead(bool bForced)
    {
        // script targets on body
        m_creature->RemoveAllAurasOnEvade();
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        DoCastSpellIfCan(m_creature, SPELL_REQUEST_BODY, CAST_TRIGGERED);

        // heal body only if head is not requested by force (Horseman healed)
        if (!bForced)
            DoCastSpellIfCan(m_creature, SPELL_HEAL_BODY, CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override { }
};

CreatureAI* GetAI_boss_head_of_horseman(Creature* pCreature)
{
    return new boss_head_of_horsemanAI(pCreature);
}

bool EffectScriptEffectCreature_boss_head_of_horseman(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if (uiSpellId == SPELL_SEND_HEAD && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCreatureTarget->GetEntry() == NPC_HEAD_OF_HORSEMAN)
            pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);

        return true;
    }

    return false;
}

void AddSC_boss_headless_horseman()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_headless_horseman";
    pNewScript->GetAI = GetAI_boss_headless_horseman;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_boss_headless_horseman;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_head_of_horseman";
    pNewScript->GetAI = GetAI_boss_head_of_horseman;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_boss_head_of_horseman;
    pNewScript->RegisterSelf();
}

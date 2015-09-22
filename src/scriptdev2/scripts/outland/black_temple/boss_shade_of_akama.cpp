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
SDName: Boss_Shade_of_Akama
SD%Complete: 90
SDComment: Some adjustments may be required once the Shade Soul Channel stacking is fixed in core. Epilogue positions need more work.
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "black_temple.h"

enum
{
    // yells
    SAY_DEATH                       = -1564013,
    SAY_LOW_HEALTH                  = -1564014,
    // Ending cinematic text
    SAY_FREE_1                      = -1564130,
    SAY_FREE_2                      = -1564015,
    SAY_BROKEN_FREE_01              = -1564016,
    SAY_BROKEN_FREE_02              = -1564017,

    // gossip
    GOSSIP_ITEM_START_ENCOUNTER     = -3564000,
    TEXT_ID_AKAMA                   = 10866,

    // Akama spells
    SPELL_STEALTH                   = 34189,
    SPELL_DESTRUCTIVE_POISON        = 40874,
    SPELL_CHAIN_LIGHTNING           = 39945,                // old spell was 42024 -> probably wrong
    SPELL_AKAMA_SOUL_CHANNEL        = 40447,                // channeled during the event
    SPELL_AKAMA_SOUL_RETRIEVE       = 40902,                // used for the epilogue

    // Other spells
    SPELL_SUMMON_DEFENDER           = 40474,
    SPELL_SUMMON_SORCERER           = 40476,
    // SPELL_VERTEX_SHADE_BLACK      = 39833,               // used by the shade - in c_t_a
    SPELL_SHADE_SOUL_CHANNEL        = 40401,                // channel spell, used to banish the shade
    SPELL_SUMMON_SHADE_TRIGGER      = 40955,

    // npcs
    NPC_ASH_SORCERER                = 23215,
    NPC_ASH_DEFENDER                = 23216,
    NPC_ASH_ELEMENTAL               = 23523,
    NPC_ASH_ROGUE                   = 23318,
    NPC_ASH_SPIRITBIND              = 23524,
    NPC_ASH_BROKEN                  = 23319,

    // akama's phases
    PHASE_CHANNEL                   = 1,
    PHASE_COMBAT                    = 2,
    PHASE_EPILOGUE                  = 3,

    MAX_CHANNELERS                  = 6,
};

static const uint32 auiRandSpawnEntry[] =
{
    NPC_ASH_ELEMENTAL,
    NPC_ASH_ROGUE,
    NPC_ASH_SPIRITBIND
};

static const DialogueEntry aOutroDialogue[] =
{
    {SPELL_AKAMA_SOUL_RETRIEVE, 0,                  18000},
    {EMOTE_ONESHOT_ROAR,        0,                  2000},
    {SAY_FREE_1,                NPC_AKAMA_SHADE,    5000},
    {SAY_FREE_2,                NPC_AKAMA_SHADE,    20000},
    {SAY_BROKEN_FREE_01,        0,                  2000},
    {EMOTE_STATE_KNEEL,         0,                  5000},
    {SAY_BROKEN_FREE_02,        0,                  0},
    {0, 0, 0},
};

struct Location
{
    float m_fX, m_fY, m_fZ;
};

static const Location afAkamaWP[] =
{
    {516.885193f, 400.836060f, 112.784f},
    {469.597443f, 402.264404f, 118.537f}
};

static const Location afBrokenSpawnLoc[] =
{
    {541.375916f, 401.439575f, 112.784f},       // The place where Akama channels
    {534.130005f, 352.394531f, 112.784f},       // Behind a 'pillar' which is behind the east alcove
};

/*######
## npc_akama
######*/

struct npc_akamaAI : public ScriptedAI, private DialogueHelper
{
    npc_akamaAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aOutroDialogue)
    {
        m_pInstance = (instance_black_temple*)pCreature->GetInstanceData();
        InitializeDialogueHelper(m_pInstance);
        Reset();
    }

    instance_black_temple* m_pInstance;

    uint8 m_uiPhase;

    uint32 m_uiDestructivePoisonTimer;
    uint32 m_uiLightningBoltTimer;

    uint32 m_uiSummonPackTimer;
    uint32 m_uiSummonDefenderTimer;
    uint32 m_uiSummonSorcererTimer;

    uint8 m_uiChannelersDead;

    GuidList m_lBrokenGUIDList;
    GuidList m_lSorcerersGUIDList;

    bool m_bHasYelledOnce;

    void Reset() override
    {
        SetCombatMovement(false);

        m_uiPhase                   = 0;

        m_uiDestructivePoisonTimer  = 15000;
        m_uiLightningBoltTimer      = 10000;

        m_uiSummonPackTimer         = 5000;
        m_uiSummonDefenderTimer     = 10000;
        m_uiSummonSorcererTimer     = 10000;

        m_uiChannelersDead          = 0;

        m_bHasYelledOnce            = false;

        m_lBrokenGUIDList.clear();
        m_lSorcerersGUIDList.clear();

        DoCastSpellIfCan(m_creature, SPELL_STEALTH);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void AttackedBy(Unit* pAttacker) override
    {
        // When the Shade starts to attack Akama, switch to melee phase
        if (m_uiPhase == PHASE_CHANNEL && pAttacker->GetEntry() == NPC_SHADE_OF_AKAMA)
        {
            m_creature->InterruptNonMeleeSpells(false);
            AttackStart(pAttacker);
            m_uiPhase = PHASE_COMBAT;

            // despawn all sorcerers at this point
            for (GuidList::const_iterator itr = m_lSorcerersGUIDList.begin(); itr != m_lSorcerersGUIDList.end(); ++itr)
            {
                if (Creature* pSorcerer = m_creature->GetMap()->GetCreature(*itr))
                    pSorcerer->ForcedDespawn();
            }
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
        // Note: this is called from the Shade, Channeler and Sorcerer script
        // If the function is changed in the future, please review this.
        switch (pVictim->GetEntry())
        {
            case NPC_SHADE_OF_AKAMA:
                m_uiPhase = PHASE_EPILOGUE;

                m_creature->GetMotionMaster()->MovePoint(PHASE_EPILOGUE, afAkamaWP[1].m_fX, afAkamaWP[1].m_fY, afAkamaWP[1].m_fZ);
                break;
            case NPC_ASH_SORCERER:
                // Decrease the sorcerer counter
                m_lSorcerersGUIDList.remove(pVictim->GetObjectGuid());
                break;
            case NPC_ASH_CHANNELER:

                ++m_uiChannelersDead;

                // Move the shade to Akama when all channelers are dead
                // Note: the boss should be already slowly moving, but this isn't possible because of the missing stack for the speed debuff
                if (m_uiChannelersDead == MAX_CHANNELERS)
                {
                    if (m_pInstance)
                    {
                        if (Creature* pShade = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADE_OF_AKAMA))
                        {
                            float fX, fY, fZ;
                            m_creature->GetContactPoint(pShade, fX, fY, fZ);
                            pShade->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                        }
                    }
                }
                break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        m_creature->SetCorpseDelay(30);

        if (m_pInstance)
        {
            // Reset the shade
            if (Creature* pShade = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADE_OF_AKAMA))
                pShade->AI()->EnterEvadeMode();
        }
    }

    void CorpseRemoved(uint32& uiRespawnDelay) override
    {
        // Resapwn after 5 min
        uiRespawnDelay = 5 * MINUTE;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_ASH_SORCERER:
            {
                pSummoned->SetWalk(false);
                m_lSorcerersGUIDList.push_back(pSummoned->GetObjectGuid());

                float fX, fY, fZ;
                if (m_pInstance)
                {
                    if (Creature* pShade = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADE_OF_AKAMA))
                    {
                        pShade->GetNearPoint(pShade, fX, fY, fZ, 0, 20.0f, pShade->GetAngle(pSummoned));
                        pSummoned->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                    }
                }
                break;
            }
            case NPC_ASH_BROKEN:
            {
                float fX, fY, fZ;
                m_lBrokenGUIDList.push_back(pSummoned->GetObjectGuid());

                m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0, 30.0f, m_creature->GetAngle(pSummoned));
                pSummoned->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                break;
            }
            case NPC_ASH_DEFENDER:
                pSummoned->AI()->AttackStart(m_creature);
                break;
            default:
                pSummoned->SetInCombatWithZone();
                break;
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !m_pInstance)
            return;

        switch (uiPointId)
        {
            case PHASE_CHANNEL:
                if (DoCastSpellIfCan(m_creature, SPELL_AKAMA_SOUL_CHANNEL) == CAST_OK)
                {
                    m_uiPhase = PHASE_CHANNEL;

                    GuidList m_lChannelersList;
                    m_pInstance->GetChannelersGuidList(m_lChannelersList);

                    for (GuidList::const_iterator itr = m_lChannelersList.begin(); itr != m_lChannelersList.end(); ++itr)
                    {
                        if (Creature* pChanneler = m_creature->GetMap()->GetCreature(*itr))
                            pChanneler->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                }
                break;
            case PHASE_EPILOGUE:
                // Start epilogue here
                if (Creature* pShade = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADE_OF_AKAMA))
                    m_creature->SetFacingToObject(pShade);

                StartNextDialogueText(SPELL_AKAMA_SOUL_RETRIEVE);
                break;
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case SPELL_AKAMA_SOUL_RETRIEVE:
                DoCastSpellIfCan(m_creature, SPELL_AKAMA_SOUL_RETRIEVE);
                break;
            case EMOTE_ONESHOT_ROAR:
                m_creature->HandleEmote(EMOTE_ONESHOT_ROAR);
                break;
            case SAY_FREE_1:
                DoSummonBrokenAshtongue();
                break;
            case SAY_BROKEN_FREE_01:
                if (Creature* pBroken = GetClosestCreatureWithEntry(m_creature, NPC_ASH_BROKEN, 35.0f))
                    DoScriptText(SAY_BROKEN_FREE_01, pBroken);
                break;
            case EMOTE_STATE_KNEEL:
                for (GuidList::const_iterator itr = m_lBrokenGUIDList.begin(); itr != m_lBrokenGUIDList.end(); ++itr)
                {
                    if (Creature* pBroken = m_creature->GetMap()->GetCreature(*itr))
                        pBroken->HandleEmote(EMOTE_STATE_KNEEL);
                }
                break;
            case SAY_BROKEN_FREE_02:
                for (GuidList::const_iterator itr = m_lBrokenGUIDList.begin(); itr != m_lBrokenGUIDList.end(); ++itr)
                {
                    if (Creature* pBroken = m_creature->GetMap()->GetCreature(*itr))
                        DoScriptText(SAY_BROKEN_FREE_02, pBroken);
                }
                break;
        }
    }

    // Wrapper to start the Akama event
    void DoStartEvent()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SHADE, IN_PROGRESS);

        m_creature->RemoveAurasDueToSpell(SPELL_STEALTH);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->GetMotionMaster()->MovePoint(PHASE_CHANNEL, afAkamaWP[0].m_fX, afAkamaWP[0].m_fY, afAkamaWP[0].m_fZ);
    }

    // Wrapper to summon ashtongue mobs
    void DoSummonAshtongue(uint32 uiSpellId = 0)
    {
        if (!m_pInstance)
            return;

        GuidVector vGeneratorsVect;
        m_pInstance->GetGeneratorGuidVector(vGeneratorsVect);
        Creature* pGenerator = m_creature->GetMap()->GetCreature(vGeneratorsVect[urand(0, 1)]);
        if (!pGenerator)
            return;

        // Summon mobs by spell
        if (uiSpellId)
            pGenerator->CastSpell(pGenerator, uiSpellId, true, NULL, NULL, m_creature->GetObjectGuid());
        // Summon ashtongue pack
        else
        {
            float fX, fY, fZ;
            for (uint8 i = 0; i < countof(auiRandSpawnEntry); ++i)
            {
                pGenerator->GetRandomPoint(pGenerator->GetPositionX(), pGenerator->GetPositionY(), pGenerator->GetPositionZ(), 5.0f, fX, fY, fZ);
                m_creature->SummonCreature(auiRandSpawnEntry[i], fX, fY, fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
            }
        }
    }

    // Wrapper to summon the npcs for the epilogue
    void DoSummonBrokenAshtongue()
    {
        if (!m_pInstance)
            return;

        float fX, fY, fZ;

        // Spawn 4 Broken in the center and behind the column
        for (uint8 i = 0; i < countof(afBrokenSpawnLoc); ++i)
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                fX = afBrokenSpawnLoc[i].m_fX;
                fY = afBrokenSpawnLoc[i].m_fY + (j * 7);
                fZ = afBrokenSpawnLoc[i].m_fZ;

                m_creature->SummonCreature(NPC_ASH_BROKEN, fX, fY, fZ, 0, TEMPSUMMON_TIMED_DESPAWN, 10 * MINUTE * IN_MILLISECONDS);
            }
        }

        GuidVector vGeneratorsVect;
        m_pInstance->GetGeneratorGuidVector(vGeneratorsVect);

        // Spawn 4 Broken at each generator
        for (uint8 i = 0; i < vGeneratorsVect.size(); ++i)
        {
            if (Creature* pGenerator = m_creature->GetMap()->GetCreature(vGeneratorsVect[i]))
            {
                for (uint8 j = 0; j < 4; ++j)
                {
                    pGenerator->GetRandomPoint(pGenerator->GetPositionX(), pGenerator->GetPositionY(), pGenerator->GetPositionZ(), 10.0f, fX, fY, fZ);
                    m_creature->SummonCreature(NPC_ASH_BROKEN, fX, fY, fZ, 0, TEMPSUMMON_TIMED_DESPAWN, 10 * MINUTE * IN_MILLISECONDS);
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        switch (m_uiPhase)
        {
            case PHASE_CHANNEL:

                if (m_uiSummonDefenderTimer < uiDiff)
                {
                    DoSummonAshtongue(SPELL_SUMMON_DEFENDER);
                    m_uiSummonDefenderTimer = 15000;
                }
                else
                    m_uiSummonDefenderTimer -= uiDiff;

                if (m_lSorcerersGUIDList.size() <= m_uiChannelersDead)
                {
                    if (m_uiSummonSorcererTimer < uiDiff)
                    {
                        DoSummonAshtongue(SPELL_SUMMON_SORCERER);
                        m_uiSummonSorcererTimer = urand(20000, 30000);
                    }
                    else
                        m_uiSummonSorcererTimer -= uiDiff;
                }

                if (m_uiSummonPackTimer < uiDiff)
                {
                    DoSummonAshtongue();
                    m_uiSummonPackTimer = 35000;
                }
                else
                    m_uiSummonPackTimer -= uiDiff;

                break;
            case PHASE_COMBAT:

                if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                    return;

                if (!m_bHasYelledOnce && m_creature->GetHealthPercent() < 15.0f)
                {
                    DoScriptText(SAY_LOW_HEALTH, m_creature);
                    m_bHasYelledOnce = true;
                }

                if (m_uiDestructivePoisonTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_DESTRUCTIVE_POISON) == CAST_OK)
                        m_uiDestructivePoisonTimer = 15000;
                }
                else
                    m_uiDestructivePoisonTimer -= uiDiff;

                if (m_uiLightningBoltTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHAIN_LIGHTNING) == CAST_OK)
                        m_uiLightningBoltTimer = 10000;
                }
                else
                    m_uiLightningBoltTimer -= uiDiff;

                DoMeleeAttackIfReady();

                break;
            case PHASE_EPILOGUE:
                DialogueUpdate(uiDiff);
                break;
        }
    }
};

bool GossipHello_npc_akama(Player* pPlayer, Creature* pCreature)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_SHADE) != DONE)
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START_ENCOUNTER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_AKAMA, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_akama(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)             // Fight time
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        if (npc_akamaAI* pAkamaAI = dynamic_cast<npc_akamaAI*>(pCreature->AI()))
            pAkamaAI->DoStartEvent();
    }

    return true;
}

/*######
## boss_shade_of_akama
######*/

struct boss_shade_of_akamaAI : public ScriptedAI
{
    boss_shade_of_akamaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() override
    {
        SetCombatMovement(false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SHADE, FAIL);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetEntry() == NPC_AKAMA)
            EnterEvadeMode();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SHADE_TRIGGER, CAST_TRIGGERED);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_SHADE, DONE);

            // Inform Akama that the Shade is dead
            if (Creature* pAkama = m_pInstance->GetSingleCreatureFromStorage(NPC_AKAMA_SHADE))
                pAkama->AI()->KilledUnit(m_creature);
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId || !m_pInstance)
            return;

        // Set in combat with Akama
        if (Creature* pAkama = m_pInstance->GetSingleCreatureFromStorage(NPC_AKAMA_SHADE))
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            // Shade should move to Akama, not the other way around
            AttackStart(pAkama);

            // Crazy amount of threat
            m_creature->AddThreat(pAkama, 10000000.0f);
            pAkama->AddThreat(m_creature, 10000000.0f);
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

/*######
## mob_ashtongue_channeler
######*/

struct mob_ashtongue_channelerAI : public ScriptedAI
{
    mob_ashtongue_channelerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiBanishTimer;

    void Reset() override
    {
        m_uiBanishTimer = 5000;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (!m_pInstance)
            return;

        // Inform Akama that one channeler is dead
        if (Creature* pAkama = m_pInstance->GetSingleCreatureFromStorage(NPC_AKAMA_SHADE))
            pAkama->AI()->KilledUnit(m_creature);
    }

    void AttackStart(Unit* /*pWho*/) override {}
    void MoveInLineOfSight(Unit* /*pWho*/) override {}

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiBanishTimer)
        {
            if (m_uiBanishTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SHADE_SOUL_CHANNEL))
                    m_uiBanishTimer = 0;
            }
            else
                m_uiBanishTimer -= uiDiff;
        }
    }
};

/*######
## mob_ashtongue_sorcerer
######*/

struct mob_ashtongue_sorcererAI : public ScriptedAI
{
    mob_ashtongue_sorcererAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() override {}

    void AttackStart(Unit* /*pWho*/) override {}
    void MoveInLineOfSight(Unit* /*pWho*/) override {}

    void JustDied(Unit* /*pKiller*/) override
    {
        if (!m_pInstance)
            return;

        // Inform Akama that one sorcerer is dead
        if (Creature* pAkama = m_pInstance->GetSingleCreatureFromStorage(NPC_AKAMA_SHADE))
            pAkama->AI()->KilledUnit(m_creature);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // Channel on the Shade when reached the calculated point
        if (DoCastSpellIfCan(m_creature, SPELL_SHADE_SOUL_CHANNEL) == CAST_OK)
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveIdle();
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/) override {}
};

CreatureAI* GetAI_npc_akama_shade(Creature* pCreature)
{
    return new npc_akamaAI(pCreature);
}

CreatureAI* GetAI_boss_shade_of_akama(Creature* pCreature)
{
    return new boss_shade_of_akamaAI(pCreature);
}

CreatureAI* GetAI_mob_ashtongue_channeler(Creature* pCreature)
{
    return new mob_ashtongue_channelerAI(pCreature);
}

CreatureAI* GetAI_mob_ashtongue_sorcerer(Creature* pCreature)
{
    return new mob_ashtongue_sorcererAI(pCreature);
}

void AddSC_boss_shade_of_akama()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_akama_shade";
    pNewScript->GetAI = &GetAI_npc_akama_shade;
    pNewScript->pGossipHello = &GossipHello_npc_akama;
    pNewScript->pGossipSelect = &GossipSelect_npc_akama;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_shade_of_akama";
    pNewScript->GetAI = &GetAI_boss_shade_of_akama;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_ashtongue_channeler";
    pNewScript->GetAI = &GetAI_mob_ashtongue_channeler;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_ashtongue_sorcerer";
    pNewScript->GetAI = &GetAI_mob_ashtongue_sorcerer;
    pNewScript->RegisterSelf();
}

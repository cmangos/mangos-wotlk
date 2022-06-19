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
SDName: boss_freya
SD%Complete: 95%
SDComment: Elders and some of the Nature Allies handled in ACID. Script might require some minor improvements.
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ulduar.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_AGGRO                           = -1603000,
    SAY_AGGRO_HARD                      = -1603001,
    SAY_ADDS_CONSERVATOR                = -1603002,
    SAY_ADDS_TRIO                       = -1603003,
    SAY_ADDS_LASHER                     = -1603004,
    SAY_SLAY_1                          = -1603005,
    SAY_SLAY_2                          = -1603006,
    SAY_EPILOGUE                        = -1603007,
    SAY_BERSERK                         = -1603008,

    EMOTE_ALLIES_NATURE                 = -1603010,
    EMOTE_REGEN_ALLIES                  = -1603011,

    SAY_AGGRO_BRIGHT                    = -1603014,
    SAY_SLAY_1_BRIGHT                   = -1603015,
    SAY_SLAY_2_BRIGHT                   = -1603016,
    SAY_DEATH_BRIGHT                    = -1603017,

    SAY_AGGRO_IRON                      = -1603018,
    SAY_SLAY_1_IRON                     = -1603019,
    SAY_SLAY_2_IRON                     = -1603020,
    SAY_DEATH_IRON                      = -1603021,

    SAY_AGGRO_STONE                     = -1603022,
    SAY_SLAY_1_STONE                    = -1603023,
    SAY_SLAY_2_STONE                    = -1603024,
    SAY_DEATH_STONE                     = -1603025,

    // general spells
    SPELL_FREYA_CREDIT                  = 65074,                // kill credit spell; added in spell_template
    SPELL_DEFORESTATION_CREDIT          = 65015,                // used for achievs 2985 and 2984
    SPELL_BERSERK                       = 47008,
    // SPELL_FREYA_DUMMY_YELLOW         = 63292,                // dummy spells used to light up the crystals; used in dbscripts_on_creature_move
    // SPELL_FREYA_DUMMY_BLUE           = 63294,
    // SPELL_FREYA_DUMMY_GREEN          = 63295,

    // combat spells
    SPELL_ATTUNED_TO_NATURE             = 62519,
    SPELL_TOUCH_OF_EONAR                = 62528,
    SPELL_TOUCH_OF_EONAR_H              = 62892,
    SPELL_SUNBEAM                       = 62623,
    SPELL_SUNBEAM_H                     = 62872,

    // summon creature spells
    SPELL_SUMMON_ALLIES_OF_NATURE       = 62678,                // triggers random of 62685, 62686 or 62688
    SPELL_SUMMON_ALLIES_OF_NATURE_H     = 62873,                // spell needs to be confirmed; identical to normal mode version
    SPELL_SUMMON_WAVE_1                 = 62685,                // summon npc 33203
    SPELL_SUMMON_WAVE_3                 = 62686,                // summon npcs 33202, 32916 and 32919
    SPELL_SUMMON_WAVE_10                = 62688,                // summon 10 * npc 32918
    SPELL_LIFEBINDERS_GIFT_SUMMON       = 62572,
    SPELL_NATURE_BOMB_SUMMON            = 64604,

    // summon loot spells
    SPELL_SUMMON_CHEST_0                = 62950,                // summon loot chest spells, depending on the number of elders alive
    SPELL_SUMMON_CHEST_1                = 62952,
    SPELL_SUMMON_CHEST_2                = 62953,
    SPELL_SUMMON_CHEST_3                = 62954,
    SPELL_SUMMON_CHEST_0_H              = 62955,
    SPELL_SUMMON_CHEST_1_H              = 62956,
    SPELL_SUMMON_CHEST_2_H              = 62957,
    SPELL_SUMMON_CHEST_3_H              = 62958,

    // hard mode spells
    SPELL_FULL_HEAL                     = 43978,
    SPELL_DRAINED_OF_POWER              = 62467,                // stun effect for each elder alive after finish channeling

    SPELL_BRIGHTLEAF_ESSENCE_CHANNEL    = 62485,                // brightleaf
    SPELL_BRIGHTLEAF_ESSENCE_CHANNEL_H  = 65587,
    SPELL_UNSTABLE_SUN_BEAM             = 62450,
    SPELL_UNSTABLE_SUN_BEAM_H           = 62868,

    SPELL_IRONBRANCH_ESSENCE_CHANNEL    = 62484,                // ironbrach
    SPELL_IRONBRANCH_ESSENCE_CHANNEL_H  = 65588,
    SPELL_IRON_ROOTS                    = 62439,
    SPELL_IRON_ROOTS_H                  = 62862,

    SPELL_STONEBARK_ESSEMCE_CHANNEL     = 62483,                // stonebark
    SPELL_STONEBARK_ESSEMCE_CHANNEL_H   = 65589,
    SPELL_GROUND_TREMOR                 = 62437,
    SPELL_GROUND_TREMOR_H               = 62859,

    // summons spells
    SPELL_SPORE_SUMMON_PERIODIC         = 62566,                // triggers 62582, 62591, 62592, 62593; cast by 33203
    SPELL_HEALTHY_SPORE_VISUAL          = 62538,                // cast by npc 33215
    SPELL_POTENT_PHEROMONES             = 62541,                // cast by npc 33215

    // sun beam spells; handled by eventAI
    // SPELL_UNSTABLE_SUN_BEAM_VISUAL   = 62216,                // cast by npc 33170
    // SPELL_UNSTABLE_ENERGY            = 62451,                // cast by npc 33170
    // SPELL_UNSTABLE_ENERGY_H          = 62865,

    // iron roots spells
    SPELL_STRENGTHEN_IRON_ROOTS         = 62440,                // remove stun and damange aura from summoner
    SPELL_STRENGTHEN_IRON_ROOTS_H       = 63601,
    SPELL_IRON_ROOTS_REMOVE             = 62282,                // same as above, but for the Elder version
    SPELL_IRON_ROOTS_REMOVE_H           = 63598,

    // nature bomb spells
    SPELL_NATURE_BOMB_GO                = 64600,                // spawns go 194902
    SPELL_NATURE_BOMB                   = 64587,
    SPELL_NATURE_BOMB_H                 = 64650,

    // allies of nature spells
    SPELL_ATTUNED_10_STACKS             = 62525,                // remove 10 stacks of 62519
    SPELL_ATTUNED_2_STACKS              = 62524,                // remove 2 stacks of 62519
    SPELL_ATTUNED_25_STACKS             = 62521,                // remove 25 stacks of 62519

    // three allies spells
    SPELL_FEIGN_DEATH                   = 65985,
    SPELL_CLEAR_DEBUFFS                 = 34098,
    SPELL_TIDAL_WAVE                    = 62652,                // triggers 62653 or 62935
    SPELL_TIDAL_WAVE_VISUAL             = 62655,
    SPELL_HARDENED_BARK                 = 62664,
    SPELL_HARDENED_BARK_H               = 64191,
    SPELL_LIGHTNING_LASH                = 62648,
    SPELL_LIGHTNING_LASH_H              = 62939,
    SPELL_STORMBOLT                     = 62649,
    SPELL_STORMBOLT_H                   = 62938,

    // eonar's gift spells
    SPELL_LIFEBINDERS_GIFT              = 62584,
    SPELL_LIFEBINDERS_GIFT_H            = 64185,
    SPELL_LIFEBINDERS_GIFT_VISUAL       = 62579,
    SPELL_AUTO_GROW                     = 62559,                // cast by npcs 33228 and 33215
    SPELL_PHEROMONES                    = 62619,

    // allies of nature summons
    NPC_DETONATING_LASHER               = 32918,                // has auras 64481 and 28819
    NPC_ANCIENT_CONSERVATOR             = 33203,
    NPC_WATER_SPIRIT                    = 33202,
    NPC_STORM_LASHER                    = 32919,
    NPC_SNAPLASHER                      = 32916,

    // other summons
    NPC_NATURE_BOMB                     = 34129,
    // NPC_SUN_BEAM                     = 33170,                // handled in eventAI
    // NPC_UNSTABLE_SUN_BEAM            = 33050,                // handled in eventAI
    NPC_IRON_ROOTS                      = 33088,
    NPC_STRENGHENED_IRON_ROOTS          = 33168,
    NPC_EONARS_GIFT                     = 33228,
    // NPC_HEALTHY_SPORE                = 33215,

    // other
    // GO_NATURE_BOMB                   = 194902,
    MIN_ATTUNED_NATURE_STACKS           = 25,
    MAX_ALLIES_SPELLS                   = 3,
    MAX_ALLIES_WAVES                    = 6,
    MIN_DEFORESTATION_COUNT             = 2,
};

static const uint32 aAlliesSpawnSpells[MAX_ALLIES_SPELLS] = {SPELL_SUMMON_WAVE_1, SPELL_SUMMON_WAVE_3, SPELL_SUMMON_WAVE_10};

/*######
## boss_freya
######*/

struct boss_freyaAI : public ScriptedAI
{
    boss_freyaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        // init the Allies of Nature spells
        spawnSpellsVector.reserve(MAX_ALLIES_SPELLS);
        for (unsigned int aAlliesSpawnSpell : aAlliesSpawnSpells)
            spawnSpellsVector.push_back(aAlliesSpawnSpell);

        m_bEventFinished = false;
        m_uiEpilogueTimer = 0;
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bEventFinished;
    bool m_bDeforestationComplete;

    uint32 m_uiEpilogueTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiDrainEldersTimer;

    uint32 m_uiAlliesNatureTimer;
    uint8 m_uiAlliesWaveCount;

    uint32 m_uiSunbeamTimer;
    uint32 m_uiNatureBombTimer;
    uint32 m_uiLifebindersGiftTimer;
    uint32 m_uiThreeAlliesTimer;

    uint32 m_uiUnstableEnergyTimer;
    uint32 m_uiIronRootsTimer;
    uint32 m_uiGroundTremorTimer;

    uint32 m_uiDeforestationTimer;
    uint8 m_uiWaterSpiritDeadCount;
    uint8 m_uiSnaplasherDeadCount;
    uint8 m_uiStormlasherDeadCount;

    ObjectGuid m_waterSpiritGuid;
    ObjectGuid m_stormLasherGuid;
    ObjectGuid m_snaplasherGuid;

    std::vector<uint32> spawnSpellsVector;

    void Reset() override
    {
        m_uiBerserkTimer            = 10 * MINUTE * IN_MILLISECONDS;

        m_uiAlliesNatureTimer       = 10000;
        m_uiAlliesWaveCount         = 0;
        m_uiNatureBombTimer         = 65000;
        m_uiSunbeamTimer            = 20000;
        m_uiLifebindersGiftTimer    = 25000;
        m_uiThreeAlliesTimer        = 0;

        m_uiUnstableEnergyTimer     = 0;
        m_uiIronRootsTimer          = 0;
        m_uiGroundTremorTimer       = 0;
        m_uiDrainEldersTimer        = 0;

        m_uiDeforestationTimer      = 0;
        m_uiWaterSpiritDeadCount    = 0;
        m_uiSnaplasherDeadCount     = 0;
        m_uiStormlasherDeadCount    = 0;

        // make the spawn spells random
        std::shuffle(spawnSpellsVector.begin(), spawnSpellsVector.end(), *GetRandomGenerator());

        m_bDeforestationComplete    = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        // don't attack again after being defeated
        if (m_bEventFinished)
            return;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FREYA, IN_PROGRESS);

        DoCastSpellIfCan(m_creature, SPELL_ATTUNED_TO_NATURE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_TOUCH_OF_EONAR : SPELL_TOUCH_OF_EONAR_H, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

        FetchElders();
    }

    void AttackStart(Unit* pWho) override
    {
        // don't attack again after being defeated
        if (m_bEventFinished)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // don't attack again after being defeated
        if (m_bEventFinished)
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_FREYA, FAIL);

            // reset elders
            if (Creature* pElder = m_pInstance->GetSingleCreatureFromStorage(NPC_ELDER_BRIGHTLEAF))
            {
                if (pElder->IsAlive())
                {
                    pElder->AI()->EnterEvadeMode();
                    pElder->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                }
            }
            if (Creature* pElder = m_pInstance->GetSingleCreatureFromStorage(NPC_ELDER_IRONBRACH))
            {
                if (pElder->IsAlive())
                {
                    pElder->AI()->EnterEvadeMode();
                    pElder->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                }
            }
            if (Creature* pElder = m_pInstance->GetSingleCreatureFromStorage(NPC_ELDER_STONEBARK))
            {
                if (pElder->IsAlive())
                {
                    pElder->AI()->EnterEvadeMode();
                    pElder->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                }
            }
        }
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);

        if (m_creature->IsAlive() && !m_bEventFinished)
            m_creature->GetMotionMaster()->MoveTargetedHome();

        m_creature->SetLootRecipient(nullptr);

        Reset();
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (!m_bEventFinished)
            {
                if (m_pInstance)
                {
                    m_pInstance->SetData(TYPE_FREYA, DONE);

                    // spawn chest loot
                    switch (m_pInstance->GetData(TYPE_FREYA_HARD))
                    {
                        case 0: DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SUMMON_CHEST_0 : SPELL_SUMMON_CHEST_0_H, CAST_TRIGGERED); break;
                        case 1: DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SUMMON_CHEST_1 : SPELL_SUMMON_CHEST_1_H, CAST_TRIGGERED); break;
                        case 2: DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SUMMON_CHEST_2 : SPELL_SUMMON_CHEST_2_H, CAST_TRIGGERED); break;
                        case 3: DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SUMMON_CHEST_3 : SPELL_SUMMON_CHEST_3_H, CAST_TRIGGERED); break;
                    }

                    // check aura stacks for achiev
                    if (SpellAuraHolder* pNatureAura = m_creature->GetSpellAuraHolder(SPELL_ATTUNED_TO_NATURE))
                    {
                        if (pNatureAura && pNatureAura->GetStackAmount() >= MIN_ATTUNED_NATURE_STACKS)
                            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_BACK_NATURE, true);
                    }
                }

                DoScriptText(SAY_EPILOGUE, m_creature);
                m_creature->CastSpell(m_creature, SPELL_FREYA_CREDIT, TRIGGERED_OLD_TRIGGERED);

                m_uiEpilogueTimer = 10000;
                m_bEventFinished = true;
                EnterEvadeMode();
            }
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_EONARS_GIFT:
                pSummoned->CastSpell(pSummoned, SPELL_LIFEBINDERS_GIFT_VISUAL, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_AUTO_GROW, TRIGGERED_OLD_TRIGGERED);
                pSummoned->CastSpell(pSummoned, SPELL_PHEROMONES, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_DETONATING_LASHER:
            case NPC_ANCIENT_CONSERVATOR:
                pSummoned->AI()->AttackStart(m_creature->GetVictim());
                break;
            case NPC_WATER_SPIRIT:
                m_waterSpiritGuid = pSummoned->GetObjectGuid();
                pSummoned->AI()->AttackStart(m_creature->GetVictim());
                break;
            case NPC_STORM_LASHER:
                m_stormLasherGuid = pSummoned->GetObjectGuid();
                pSummoned->AI()->AttackStart(m_creature->GetVictim());
                break;
            case NPC_SNAPLASHER:
                m_snaplasherGuid = pSummoned->GetObjectGuid();
                pSummoned->AI()->AttackStart(m_creature->GetVictim());
                break;
            case NPC_NATURE_BOMB:
                pSummoned->CastSpell(pSummoned, SPELL_NATURE_BOMB_GO, TRIGGERED_OLD_TRIGGERED);
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_DETONATING_LASHER:
                pSummoned->CastSpell(m_creature, SPELL_ATTUNED_2_STACKS, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_ANCIENT_CONSERVATOR:
                pSummoned->CastSpell(m_creature, SPELL_ATTUNED_25_STACKS, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_WATER_SPIRIT:
            case NPC_STORM_LASHER:
            case NPC_SNAPLASHER:
                pSummoned->CastSpell(m_creature, SPELL_ATTUNED_10_STACKS, TRIGGERED_OLD_TRIGGERED);

                // start achiev timer and reset the counter
                if (!m_uiDeforestationTimer && !m_bDeforestationComplete)
                {
                    m_uiDeforestationTimer      = 10000;
                    m_uiWaterSpiritDeadCount    = 0;
                    m_uiSnaplasherDeadCount     = 0;
                    m_uiStormlasherDeadCount    = 0;
                }
                break;
        }

        // Deforestation achiev
        if (m_uiDeforestationTimer)
        {
            switch (pSummoned->GetEntry())
            {
                case NPC_WATER_SPIRIT:
                    ++m_uiWaterSpiritDeadCount;
                    break;
                case NPC_STORM_LASHER:
                    ++m_uiStormlasherDeadCount;
                    break;
                case NPC_SNAPLASHER:
                    ++m_uiSnaplasherDeadCount;
                    break;
            }

            // give achiev credit
            if (m_uiWaterSpiritDeadCount >= MIN_DEFORESTATION_COUNT && m_uiStormlasherDeadCount >= MIN_DEFORESTATION_COUNT && m_uiSnaplasherDeadCount >= MIN_DEFORESTATION_COUNT)
            {
                if (m_pInstance)
                {
                    if (Creature* pTrigger = m_pInstance->GetSingleCreatureFromStorage(NPC_FREYA_ACHIEV_TRIGGER))
                        pTrigger->CastSpell(pTrigger, SPELL_DEFORESTATION_CREDIT, TRIGGERED_OLD_TRIGGERED);

                    m_bDeforestationComplete = true;
                    m_uiDeforestationTimer = 0;
                }
            }
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        // handle Allies of Nature spawn
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            // adjust the index to the size of the vector
            uint8 uiIndex = m_uiAlliesWaveCount;
            if (uiIndex >= MAX_ALLIES_SPELLS)
                uiIndex = m_uiAlliesWaveCount - MAX_ALLIES_SPELLS;

            switch (spawnSpellsVector[uiIndex])
            {
                case SPELL_SUMMON_WAVE_1:  DoScriptText(SAY_ADDS_CONSERVATOR, m_creature); break;
                case SPELL_SUMMON_WAVE_3:  DoScriptText(SAY_ADDS_TRIO, m_creature);        break;
                case SPELL_SUMMON_WAVE_10: DoScriptText(SAY_ADDS_LASHER, m_creature);      break;
            }

            DoCastSpellIfCan(m_creature, spawnSpellsVector[uiIndex], CAST_TRIGGERED);

            ++m_uiAlliesWaveCount;

            // re-shuffle the spells
            if (m_uiAlliesWaveCount == MAX_ALLIES_SPELLS)
            {
                uint32 uiLastSpell = spawnSpellsVector[MAX_ALLIES_SPELLS - 1];
                std::shuffle(spawnSpellsVector.begin(), spawnSpellsVector.end(), *GetRandomGenerator());

                // make sure we won't repeat the last spell
                while (spawnSpellsVector[0] == uiLastSpell)
                    std::shuffle(spawnSpellsVector.begin(), spawnSpellsVector.end(), *GetRandomGenerator());
            }
        }
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            if (!m_uiThreeAlliesTimer)
                m_uiThreeAlliesTimer = 12000;
        }
    }

    // check for all elders alive
    void FetchElders()
    {
        if (!m_pInstance)
            return;

        uint8 uiEldersAlive = 0;

        if (Creature* pElder = m_pInstance->GetSingleCreatureFromStorage(NPC_ELDER_BRIGHTLEAF))
        {
            if (pElder->IsAlive())
            {
                pElder->CastSpell(pElder, m_bIsRegularMode ? SPELL_BRIGHTLEAF_ESSENCE_CHANNEL : SPELL_BRIGHTLEAF_ESSENCE_CHANNEL_H, TRIGGERED_NONE, nullptr, nullptr, m_creature->GetObjectGuid());
                pElder->CastSpell(pElder, SPELL_FULL_HEAL, TRIGGERED_OLD_TRIGGERED);

                m_uiUnstableEnergyTimer = 25000;
                ++uiEldersAlive;
            }
        }
        if (Creature* pElder = m_pInstance->GetSingleCreatureFromStorage(NPC_ELDER_IRONBRACH))
        {
            if (pElder->IsAlive())
            {
                pElder->CastSpell(pElder, m_bIsRegularMode ? SPELL_IRONBRANCH_ESSENCE_CHANNEL : SPELL_IRONBRANCH_ESSENCE_CHANNEL_H, TRIGGERED_NONE, nullptr, nullptr, m_creature->GetObjectGuid());
                pElder->CastSpell(pElder, SPELL_FULL_HEAL, TRIGGERED_OLD_TRIGGERED);

                m_uiIronRootsTimer = 60000;
                ++uiEldersAlive;
            }
        }
        if (Creature* pElder = m_pInstance->GetSingleCreatureFromStorage(NPC_ELDER_STONEBARK))
        {
            if (pElder->IsAlive())
            {
                pElder->CastSpell(pElder, m_bIsRegularMode ? SPELL_STONEBARK_ESSEMCE_CHANNEL : SPELL_STONEBARK_ESSEMCE_CHANNEL_H, TRIGGERED_NONE, nullptr, nullptr, m_creature->GetObjectGuid());
                pElder->CastSpell(pElder, SPELL_FULL_HEAL, TRIGGERED_OLD_TRIGGERED);

                m_uiGroundTremorTimer = 10000;
                ++uiEldersAlive;
            }
        }

        // store the info about the elders alive
        m_pInstance->SetData(TYPE_FREYA_HARD, uiEldersAlive);

        if (uiEldersAlive)
        {
            DoScriptText(SAY_AGGRO_HARD, m_creature);
            m_uiDrainEldersTimer = 5000;
        }
        else
            DoScriptText(SAY_AGGRO, m_creature);
    }

    // Function that will drain elders after aggro
    void DoDrainElders()
    {
        if (!m_pInstance)
            return;

        if (Creature* pElder = m_pInstance->GetSingleCreatureFromStorage(NPC_ELDER_BRIGHTLEAF))
        {
            if (pElder->IsAlive())
            {
                pElder->CastSpell(pElder, SPELL_DRAINED_OF_POWER, TRIGGERED_OLD_TRIGGERED);
                pElder->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            }
        }
        if (Creature* pElder = m_pInstance->GetSingleCreatureFromStorage(NPC_ELDER_IRONBRACH))
        {
            if (pElder->IsAlive())
            {
                pElder->CastSpell(pElder, SPELL_DRAINED_OF_POWER, TRIGGERED_OLD_TRIGGERED);
                pElder->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            }
        }
        if (Creature* pElder = m_pInstance->GetSingleCreatureFromStorage(NPC_ELDER_STONEBARK))
        {
            if (pElder->IsAlive())
            {
                pElder->CastSpell(pElder, SPELL_DRAINED_OF_POWER, TRIGGERED_OLD_TRIGGERED);
                pElder->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiEpilogueTimer)
        {
            if (m_uiEpilogueTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_TELEPORT, CAST_TRIGGERED) == CAST_OK)
                {
                    m_creature->ForcedDespawn(2000);
                    m_uiEpilogueTimer = 0;
                }
            }
            else
                m_uiEpilogueTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

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

        // Deforestation achiev timer
        if (m_uiDeforestationTimer)
        {
            if (m_uiDeforestationTimer <= uiDiff)
                m_uiDeforestationTimer = 0;
            else
                m_uiDeforestationTimer -= uiDiff;
        }

        // Drain elders after hard mode aggro
        if (m_uiDrainEldersTimer)
        {
            if (m_uiDrainEldersTimer <= uiDiff)
            {
                DoDrainElders();
                m_uiDrainEldersTimer = 0;
            }
            else
                m_uiDrainEldersTimer -= uiDiff;
        }

        if (m_uiThreeAlliesTimer)
        {
            if (m_uiThreeAlliesTimer <= uiDiff)
            {
                Creature* pSpirit = m_creature->GetMap()->GetCreature(m_waterSpiritGuid);
                Creature* pStormLasher = m_creature->GetMap()->GetCreature(m_stormLasherGuid);
                Creature* pSnapLasher = m_creature->GetMap()->GetCreature(m_snaplasherGuid);
                if (!pSpirit || !pStormLasher || !pSnapLasher)
                    return;

                if (pSpirit->HasAura(SPELL_FEIGN_DEATH) && pStormLasher->HasAura(SPELL_FEIGN_DEATH) && pSnapLasher->HasAura(SPELL_FEIGN_DEATH))
                {
                    // TODO: Check if this shouldnt be a suicide
                    pSpirit->Suicide();
                    pStormLasher->Suicide();
                    pSnapLasher->Suicide();
                }
                else
                {
                    pSpirit->CastSpell(pSpirit, SPELL_FULL_HEAL, TRIGGERED_OLD_TRIGGERED);
                    pStormLasher->CastSpell(pStormLasher, SPELL_FULL_HEAL, TRIGGERED_OLD_TRIGGERED);
                    pSnapLasher->CastSpell(pSnapLasher, SPELL_FULL_HEAL, TRIGGERED_OLD_TRIGGERED);
                }

                m_uiThreeAlliesTimer = 0;
            }
            else
                m_uiThreeAlliesTimer -= uiDiff;
        }

        if (m_uiAlliesWaveCount < MAX_ALLIES_WAVES)
        {
            if (m_uiAlliesNatureTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SUMMON_ALLIES_OF_NATURE : SPELL_SUMMON_ALLIES_OF_NATURE_H) == CAST_OK)
                {
                    DoScriptText(EMOTE_ALLIES_NATURE, m_creature);
                    m_uiAlliesNatureTimer = 60000;
                }
            }
            else
                m_uiAlliesNatureTimer -= uiDiff;
        }
        else
        {
            if (m_uiNatureBombTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_NATURE_BOMB_SUMMON) == CAST_OK)
                    m_uiNatureBombTimer = 15000;
            }
            else
                m_uiNatureBombTimer -= uiDiff;
        }

        if (m_uiSunbeamTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SUNBEAM : SPELL_SUNBEAM_H) == CAST_OK)
                    m_uiSunbeamTimer = 15000;
            }
        }
        else
            m_uiSunbeamTimer -= uiDiff;

        if (m_uiLifebindersGiftTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_LIFEBINDERS_GIFT_SUMMON) == CAST_OK)
                m_uiLifebindersGiftTimer = 40000;
        }
        else
            m_uiLifebindersGiftTimer -= uiDiff;

        // Brightleaf ability
        if (m_uiUnstableEnergyTimer)
        {
            if (m_uiUnstableEnergyTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_UNSTABLE_SUN_BEAM : SPELL_UNSTABLE_SUN_BEAM_H) == CAST_OK)
                    m_uiUnstableEnergyTimer = 25000;
            }
            else
                m_uiUnstableEnergyTimer -= uiDiff;
        }

        // Ironbranch ability
        if (m_uiIronRootsTimer)
        {
            if (m_uiIronRootsTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_IRON_ROOTS : SPELL_IRON_ROOTS_H) == CAST_OK)
                    m_uiIronRootsTimer = 60000;
            }
            else
                m_uiIronRootsTimer -= uiDiff;
        }

        // Stonebark ability
        if (m_uiGroundTremorTimer)
        {
            if (m_uiGroundTremorTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_GROUND_TREMOR : SPELL_GROUND_TREMOR_H) == CAST_OK)
                    m_uiGroundTremorTimer = 30000;
            }
            else
                m_uiGroundTremorTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_freya(Creature* pCreature)
{
    return new boss_freyaAI(pCreature);
}

bool EffectScriptEffectCreature_boss_freya(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if ((uiSpellId == SPELL_SUMMON_ALLIES_OF_NATURE || uiSpellId == SPELL_SUMMON_ALLIES_OF_NATURE_H) && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCreatureTarget->GetEntry() == NPC_FREYA)
            pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget);

        return true;
    }

    return false;
}

/*######
## three_nature_allies
######*/

struct three_nature_alliesAI : public ScriptedAI
{
    three_nature_alliesAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsFakeDeath;

    void Reset() override
    {
        m_bIsFakeDeath = false;
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (pDoneBy->GetEntry() == NPC_FREYA)
            return;

        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (m_bIsFakeDeath)
                return;

            if (m_pInstance)
            {
                if (Creature* pFreya = m_pInstance->GetSingleCreatureFromStorage(NPC_FREYA))
                    SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, pFreya);
            }

            DoCastSpellIfCan(m_creature, SPELL_CLEAR_DEBUFFS, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_FEIGN_DEATH, CAST_TRIGGERED);

            m_creature->SetHealth(1);
            m_creature->ClearComboPointHolders();
            m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
            m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->ClearAllReactives();
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

            DoScriptText(EMOTE_REGEN_ALLIES, m_creature);
            m_bIsFakeDeath = true;
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_FULL_HEAL)
        {
            m_bIsFakeDeath = false;
            DoResetThreat();
            m_creature->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        }
    }
};

/*######
## npc_water_spirit
######*/

struct npc_water_spiritAI : public three_nature_alliesAI
{
    npc_water_spiritAI(Creature* pCreature) : three_nature_alliesAI(pCreature) { Reset(); }

    uint32 m_uiTidalWaveTimer;

    void Reset() override
    {
        m_uiTidalWaveTimer = 10000;
        three_nature_alliesAI::Reset();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiTidalWaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TIDAL_WAVE) == CAST_OK)
            {
                DoCastSpellIfCan(m_creature, SPELL_TIDAL_WAVE_VISUAL, CAST_TRIGGERED);
                m_uiTidalWaveTimer = 10000;
            }
        }
        else
            m_uiTidalWaveTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_water_spirit(Creature* pCreature)
{
    return new npc_water_spiritAI(pCreature);
}

/*######
## npc_snaplasher
######*/

struct npc_snaplasherAI : public three_nature_alliesAI
{
    npc_snaplasherAI(Creature* pCreature) : three_nature_alliesAI(pCreature) { Reset(); }

    void Reset() override
    {
        three_nature_alliesAI::Reset();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_HARDENED_BARK : SPELL_HARDENED_BARK_H);
    }
};

UnitAI* GetAI_npc_snaplasher(Creature* pCreature)
{
    return new npc_snaplasherAI(pCreature);
}

/*######
## npc_storm_lasher
######*/

struct npc_storm_lasherAI : public three_nature_alliesAI
{
    npc_storm_lasherAI(Creature* pCreature) : three_nature_alliesAI(pCreature) { Reset(); }

    uint32 m_uiLightningLashTimer;
    uint32 m_uiStormBoltTimer;

    void Reset() override
    {
        m_uiLightningLashTimer = urand(5000, 10000);
        m_uiStormBoltTimer = 5000;
        three_nature_alliesAI::Reset();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiLightningLashTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_LIGHTNING_LASH : SPELL_LIGHTNING_LASH_H) == CAST_OK)
                    m_uiLightningLashTimer = urand(5000, 10000);
            }
        }
        else
            m_uiLightningLashTimer -= uiDiff;

        if (m_uiStormBoltTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_STORMBOLT : SPELL_STORMBOLT_H) == CAST_OK)
                    m_uiStormBoltTimer = 5000;
            }
        }
        else
            m_uiStormBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_storm_lasher(Creature* pCreature)
{
    return new npc_storm_lasherAI(pCreature);
}

/*######
## npc_eonars_gift
######*/

struct npc_eonars_giftAI : public Scripted_NoMovementAI
{
    npc_eonars_giftAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 m_uiGiftTimer;

    void Reset() override
    {
        m_uiGiftTimer = 10000;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiGiftTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_LIFEBINDERS_GIFT : SPELL_LIFEBINDERS_GIFT_H) == CAST_OK)
                m_uiGiftTimer = 10000;
        }
        else
            m_uiGiftTimer -= uiDiff;
    }
};

UnitAI* GetAI_npc_eonars_gift(Creature* pCreature)
{
    return new npc_eonars_giftAI(pCreature);
}

/*######
## npc_nature_bomb
######*/

struct npc_nature_bombAI : public Scripted_NoMovementAI
{
    npc_nature_bombAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 m_uiNatureBombTimer;

    ObjectGuid m_natureBombGuid;

    void Reset() override
    {
        m_uiNatureBombTimer = 10000;
    }

    void JustSummoned(GameObject* pGo) override
    {
        m_natureBombGuid = pGo->GetObjectGuid();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiNatureBombTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_NATURE_BOMB : SPELL_NATURE_BOMB_H) == CAST_OK)
            {
                if (GameObject* pBomb = m_creature->GetMap()->GetGameObject(m_natureBombGuid))
                    pBomb->Use(m_creature);

                m_creature->ForcedDespawn(2000);
                m_uiNatureBombTimer = 10000;
            }
        }
        else
            m_uiNatureBombTimer -= uiDiff;
    }
};

UnitAI* GetAI_npc_nature_bomb(Creature* pCreature)
{
    return new npc_nature_bombAI(pCreature);
}

/*######
## npc_iron_roots
######*/

struct npc_iron_rootsAI : public Scripted_NoMovementAI
{
    npc_iron_rootsAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (!m_creature->IsTemporarySummon())
            return;

        if (m_creature->GetEntry() == NPC_IRON_ROOTS)
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_IRON_ROOTS_REMOVE : SPELL_IRON_ROOTS_REMOVE_H, CAST_TRIGGERED);
        else if (m_creature->GetEntry() == NPC_STRENGHENED_IRON_ROOTS)
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_STRENGTHEN_IRON_ROOTS : SPELL_STRENGTHEN_IRON_ROOTS_H, CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_iron_roots(Creature* pCreature)
{
    return new npc_iron_rootsAI(pCreature);
}

/*######
## npc_healthy_spore
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_healthy_sporeAI : public Scripted_NoMovementAI
{
    npc_healthy_sporeAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_AUTO_GROW, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(m_creature, SPELL_POTENT_PHEROMONES, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(m_creature, SPELL_HEALTHY_SPORE_VISUAL, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        m_creature->ForcedDespawn(25000);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_npc_healthy_spore(Creature* pCreature)
{
    return new npc_healthy_sporeAI(pCreature);
}

void AddSC_boss_freya()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_freya";
    pNewScript->GetAI = GetAI_boss_freya;
    pNewScript->pEffectScriptEffectNPC = &EffectScriptEffectCreature_boss_freya;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_water_spirit";
    pNewScript->GetAI = GetAI_npc_water_spirit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_snaplasher";
    pNewScript->GetAI = GetAI_npc_snaplasher;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_storm_lasher";
    pNewScript->GetAI = GetAI_npc_storm_lasher;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_eonars_gift";
    pNewScript->GetAI = GetAI_npc_eonars_gift;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_nature_bomb";
    pNewScript->GetAI = GetAI_npc_nature_bomb;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_iron_roots";
    pNewScript->GetAI = GetAI_npc_iron_roots;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_healthy_spore";
    pNewScript->GetAI = GetAI_npc_healthy_spore;
    pNewScript->RegisterSelf();
}

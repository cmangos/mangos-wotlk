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
SDName: Spell_Scripts
SD%Complete: 100
SDComment: Spell scripts for dummy effects (if script need access/interaction with parts of other AI or instance, add it in related source files)
SDCategory: Spell
EndScriptData */

/* ContentData
spell 21014
spell 21050
spell 26275
spell 29528
spell 29866
spell 34665
spell 37136
spell 39246
spell 43340
spell 44935
spell 45109
spell 45111
spell 46023
spell 47575
spell 50630
spell 50706
spell 51331
spell 51332
spell 51366
spell 52090
spell 52419
spell 56099
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Spells/SpellAuras.h"
#include "Spells/Scripts/SpellScript.h"
#include "Grids/Cell.h"
#include "Grids/CellImpl.h"
#include "Grids/GridNotifiers.h"
#include "Grids/GridNotifiersImpl.h"
#include "Grids/CellImpl.h"
#include "OutdoorPvP/OutdoorPvP.h"

/* When you make a spell effect:
- always check spell id and effect index
- always return true when the spell is handled by script
*/

enum
{
    // quest 9452
    SPELL_CAST_FISHING_NET      = 29866,
    GO_RED_SNAPPER              = 181616,
    NPC_ANGRY_MURLOC            = 17102,
    ITEM_RED_SNAPPER            = 23614,
    // SPELL_SUMMON_TEST           = 49214                  // ! Just wrong spell name? It summon correct creature (17102)but does not appear to be used.

    // quest 11472
    SPELL_ANUNIAQS_NET          = 21014,
    GO_TASTY_REEF_FISH          = 186949,
    NPC_REEF_SHARK              = 24637,
    ITEM_TASTY_REEF_FISH        = 34127,
};

bool EffectDummyGameObj_spell_dummy_go(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, GameObject* pGOTarget, ObjectGuid /*originalCasterGuid*/)
{
    switch (uiSpellId)
    {
        case SPELL_ANUNIAQS_NET:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pGOTarget->GetRespawnTime() != 0 || pGOTarget->GetEntry() != GO_TASTY_REEF_FISH || pCaster->GetTypeId() != TYPEID_PLAYER)
                    return true;

                if (urand(0, 3))
                {
                    if (Item* pItem = ((Player*)pCaster)->StoreNewItemInInventorySlot(ITEM_TASTY_REEF_FISH, 1))
                        ((Player*)pCaster)->SendNewItem(pItem, 1, true, false);
                }
                else
                {
                    if (Creature* pShark = pCaster->SummonCreature(NPC_REEF_SHARK, pGOTarget->GetPositionX(), pGOTarget->GetPositionY(), pGOTarget->GetPositionZ(), 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, 30000))
                        pShark->AI()->AttackStart(pCaster);
                }

                pGOTarget->SetLootState(GO_JUST_DEACTIVATED);
                return true;
            }
            return true;
        }
        case SPELL_CAST_FISHING_NET:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pGOTarget->GetRespawnTime() != 0 || pGOTarget->GetEntry() != GO_RED_SNAPPER || pCaster->GetTypeId() != TYPEID_PLAYER)
                    return true;

                if (urand(0, 2))
                {
                    if (Creature* pMurloc = pCaster->SummonCreature(NPC_ANGRY_MURLOC, pCaster->GetPositionX(), pCaster->GetPositionY() + 20.0f, pCaster->GetPositionZ(), 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, 10000))
                        pMurloc->AI()->AttackStart(pCaster);
                }
                else
                {
                    if (Item* pItem = ((Player*)pCaster)->StoreNewItemInInventorySlot(ITEM_RED_SNAPPER, 1))
                        ((Player*)pCaster)->SendNewItem(pItem, 1, true, false);
                }

                pGOTarget->SetLootState(GO_JUST_DEACTIVATED);
                return true;
            }
            return true;
        }
    }

    return false;
}

enum
{
    // quest 9629
    SPELL_TAG_MURLOC                    = 30877,
    SPELL_TAG_MURLOC_PROC               = 30875,
    NPC_BLACKSILT_MURLOC                = 17326,
    NPC_TAGGED_MURLOC                   = 17654,

    // quest 9447
    SPELL_HEALING_SALVE                 = 29314,
    SPELL_HEALING_SALVE_DUMMY           = 29319,
    NPC_MAGHAR_GRUNT                    = 16846,

    // quest 10190
    SPELL_RECHARGING_BATTERY            = 34219,
    NPC_DRAINED_PHASE_HUNTER            = 19595,

    // target hulking helboar
    SPELL_ADMINISTER_ANTIDOTE           = 34665,
    NPC_HELBOAR                         = 16880,
    NPC_DREADTUSK                       = 16992,

    // quest 12906/13422
    SPELL_DISCIPLINING_ROD              = 56033,
    SAY_RAND_WORK1                      = -1000555,
    SAY_RAND_WORK2                      = -1000556,
    SAY_RAND_WORK3                      = -1000557,
    SAY_RAND_ATTACK1                    = -1000558,
    SAY_RAND_ATTACK2                    = -1000559,
    SAY_RAND_ATTACK3                    = -1000560,

    // quest 11515
    SPELL_FEL_SIPHON_DUMMY              = 44936,
    NPC_FELBLOOD_INITIATE               = 24918,
    NPC_EMACIATED_FELBLOOD              = 24955,

    // target nestlewood owlkin
    SPELL_INOCULATE_OWLKIN              = 29528,
    NPC_OWLKIN                          = 16518,
    NPC_OWLKIN_INOC                     = 16534,

    // for quest 12516
    SPELL_MODIFIED_MOJO                 = 50706,

    NPC_PROPHET_OF_SSERATUS             = 28068,
    NPC_WEAK_PROPHET_OF_SSERATUS        = 28151,

    // for quest 12459
    SPELL_SEEDS_OF_NATURES_WRATH        = 49587,

    NPC_REANIMATED_FROSTWYRM            = 26841,
    NPC_TURGID                          = 27808,
    NPC_DEATHGAZE                       = 27122,

    NPC_WEAK_REANIMATED_FROSTWYRM       = 27821,
    NPC_WEAK_TURGID                     = 27809,
    NPC_WEAK_DEATHGAZE                  = 27807,

    // for quest 11730
    SPELL_ULTRASONIC_SCREWDRIVER        = 46023,
    SPELL_REPROGRAM_KILL_CREDIT         = 46027,

    NPC_COLLECT_A_TRON                  = 25793,
    SPELL_SUMMON_COLLECT_A_TRON         = 46034,

    NPC_DEFENDO_TANK                    = 25758,
    SPELL_SUMMON_DEFENDO_TANK           = 46058,

    NPC_SCAVENGE_A8                     = 25752,
    SPELL_SUMMON_SCAVENGE_A8            = 46063,

    NPC_SCAVENGE_B6                     = 25792,
    SPELL_SUMMON_SCAVENGE_B6            = 46066,

    NPC_SENTRY_BOT                      = 25753,
    SPELL_SUMMON_SENTRY_BOT             = 46068,

    // target woodlands walker
    SPELL_STRENGTH_ANCIENTS             = 47575,
    SPELL_CREATE_BARK_WALKERS           = 47550,
    FACTION_HOSTILE                     = 16,

    EMOTE_AGGRO                         = -1000551,
    EMOTE_CREATE                        = -1000552,

    SAY_SPECIMEN                        = -1000581,
    NPC_NEXUS_DRAKE_HATCHLING           = 26127,
    SPELL_RAELORASZ_FIREBALL            = 46704,

    // Quest "Disrupt the Greengill Coast" (11541)
    SPELL_ORB_OF_MURLOC_CONTROL         = 45109,
    SPELL_GREENGILL_SLAVE_FREED         = 45110,
    SPELL_ENRAGE                        = 45111,
    NPC_FREED_GREENGILL_SLAVE           = 25085,
    NPC_DARKSPINE_MYRMIDON              = 25060,
    NPC_DARKSPINE_SIREN                 = 25073,

    // quest 14107
    SPELL_BLESSING_OF_PEACE             = 66719,
    NPC_FALLEN_HERO_SPIRIT              = 32149,
    NPC_FALLEN_HERO_SPIRIT_PROXY        = 35055,
    SAY_BLESS_1                         = -1000594,
    SAY_BLESS_2                         = -1000595,
    SAY_BLESS_3                         = -1000596,
    SAY_BLESS_4                         = -1000597,
    SAY_BLESS_5                         = -1000598,

    // quest 12813, by item 40587
    SPELL_DARKMENDER_TINCTURE           = 52741,
    SPELL_SUMMON_CORRUPTED_SCARLET      = 54415,
    NPC_CORPSES_RISE_CREDIT_BUNNY       = 29398,

    // quest 12659, item 38731
    SPELL_AHUNAES_KNIFE                 = 52090,
    NPC_SCALPS_KILL_CREDIT_BUNNY        = 28622,

    // quest 13549
    SPELL_TAILS_UP_GENDER_MASTER        = 62110,
    SPELL_TAILS_UP_AURA                 = 62109,
    SPELL_FORCE_LEOPARD_SUMMON          = 62117,
    SPELL_FORCE_BEAR_SUMMON             = 62118,
    NPC_FROST_LEOPARD                   = 29327,
    NPC_ICEPAW_BEAR                     = 29319,
    NPC_LEOPARD_KILL_CREDIT             = 33005,
    NPC_BEAR_KILL_CREDIT                = 33006,
    SAY_ITS_FEMALE                      = -1000642,
    SAY_ITS_MALE                        = -1000643,

    // quest 9849, item 24501
    SPELL_THROW_GORDAWG_BOULDER         = 32001,
    NPC_MINION_OF_GUROK                 = 18181,

    // quest 12589
    SPELL_HIT_APPLE                     = 51331,
    SPELL_MISS_APPLE                    = 51332,
    SPELL_MISS_APPLE_HIT_BIRD           = 51366,
    SPELL_APPLE_FALLS_TO_GROUND         = 51371,
    NPC_APPLE                           = 28053,
    NPC_LUCKY_WILHELM                   = 28054,
    NPC_DROSTAN                         = 28328,
    SAY_LUCKY_HIT_1                     = -1000644,
    SAY_LUCKY_HIT_2                     = -1000645,
    SAY_LUCKY_HIT_3                     = -1000646,
    SAY_LUCKY_HIT_APPLE                 = -1000647,
    SAY_DROSTAN_GOT_LUCKY_1             = -1000648,
    SAY_DROSTAN_GOT_LUCKY_2             = -1000649,
    SAY_DROSTAN_HIT_BIRD_1              = -1000650,
    SAY_DROSTAN_HIT_BIRD_2              = -1000651,

    // quest 11314, item 33606
    SPELL_LURIELLES_PENDANT             = 43340,
    NPC_CHILL_NYMPH                     = 23678,
    NPC_LURIELLE                        = 24117,
    FACTION_FRIENDLY                    = 35,
    SAY_FREE_1                          = -1000781,
    SAY_FREE_2                          = -1000782,
    SAY_FREE_3                          = -1000783,

    // npcs that are only interactable while dead
    SPELL_SHROUD_OF_DEATH               = 10848,
    SPELL_SPIRIT_PARTICLES              = 17327,
    NPC_FRANCLORN_FORGEWRIGHT           = 8888,
    NPC_GAERIYAN                        = 9299,

    //  for quest 10584
    SPELL_PROTOVOLTAIC_MAGNETO_COLLECTOR = 37136,
    NPC_ENCASED_ELECTROMENTAL           = 21731,

    // quest 6661
    SPELL_MELODIOUS_RAPTURE             = 21050,
    SPELL_MELODIOUS_RAPTURE_VISUAL      = 21051,
    NPC_DEEPRUN_RAT                     = 13016,
    NPC_ENTHRALLED_DEEPRUN_RAT          = 13017,

    // quest 12981
    SPELL_THROW_ICE                     = 56099,
    SPELL_FROZEN_IRON_SCRAP             = 56101,
    NPC_SMOLDERING_SCRAP_BUNNY          = 30169,
    GO_SMOLDERING_SCRAP                 = 192124,
};

bool EffectAuraDummy_spell_aura_dummy_npc(const Aura* pAura, bool bApply)
{
    switch (pAura->GetId())
    {
        case SPELL_BLESSING_OF_PEACE:
        {
            Creature* pCreature = (Creature*)pAura->GetTarget();

            if (!pCreature || pCreature->GetEntry() != NPC_FALLEN_HERO_SPIRIT)
                return true;

            if (pAura->GetEffIndex() != EFFECT_INDEX_0)
                return true;

            if (bApply)
            {
                switch (urand(0, 4))
                {
                    case 0: DoScriptText(SAY_BLESS_1, pCreature); break;
                    case 1: DoScriptText(SAY_BLESS_2, pCreature); break;
                    case 2: DoScriptText(SAY_BLESS_3, pCreature); break;
                    case 3: DoScriptText(SAY_BLESS_4, pCreature); break;
                    case 4: DoScriptText(SAY_BLESS_5, pCreature); break;
                }
            }
            else
            {
                if (Player* pPlayer = (Player*)pAura->GetCaster())
                {
                    pPlayer->KilledMonsterCredit(NPC_FALLEN_HERO_SPIRIT_PROXY, pCreature->GetObjectGuid());
                    pCreature->ForcedDespawn();
                }
            }

            return true;
        }
        case SPELL_HEALING_SALVE:
        {
            if (pAura->GetEffIndex() != EFFECT_INDEX_0)
                return true;

            if (bApply)
            {
                if (Unit* pCaster = pAura->GetCaster())
                    pCaster->CastSpell(pAura->GetTarget(), SPELL_HEALING_SALVE_DUMMY, TRIGGERED_OLD_TRIGGERED);
            }

            return true;
        }
        case SPELL_HEALING_SALVE_DUMMY:
        {
            if (pAura->GetEffIndex() != EFFECT_INDEX_0)
                return true;

            if (!bApply)
            {
                Creature* pCreature = (Creature*)pAura->GetTarget();

                pCreature->UpdateEntry(NPC_MAGHAR_GRUNT);

                if (pCreature->getStandState() == UNIT_STAND_STATE_KNEEL)
                    pCreature->SetStandState(UNIT_STAND_STATE_STAND);

                pCreature->ForcedDespawn(60 * IN_MILLISECONDS);
            }

            return true;
        }
        case SPELL_RECHARGING_BATTERY:
        {
            if (pAura->GetEffIndex() != EFFECT_INDEX_0)
                return true;

            if (!bApply)
            {
                if (pAura->GetTarget()->HasAuraState(AURA_STATE_HEALTHLESS_35_PERCENT))
                    ((Creature*)pAura->GetTarget())->UpdateEntry(NPC_DRAINED_PHASE_HUNTER);
            }

            return true;
        }
        case SPELL_TAG_MURLOC:
        {
            Creature* pCreature = (Creature*)pAura->GetTarget();

            if (pAura->GetEffIndex() != EFFECT_INDEX_0)
                return true;

            if (bApply)
            {
                if (pCreature->GetEntry() == NPC_BLACKSILT_MURLOC)
                {
                    if (Unit* pCaster = pAura->GetCaster())
                        pCaster->CastSpell(pCreature, SPELL_TAG_MURLOC_PROC, TRIGGERED_OLD_TRIGGERED);
                }
            }
            else
            {
                if (pCreature->GetEntry() == NPC_TAGGED_MURLOC)
                    pCreature->ForcedDespawn();
            }

            return true;
        }
        case SPELL_RAELORASZ_FIREBALL:
        {
            if (pAura->GetEffIndex() != EFFECT_INDEX_0)
                return true;

            if (Unit* pCaster = pAura->GetCaster())
                DoScriptText(SAY_SPECIMEN, pCaster);

            Unit* pTarget = pAura->GetTarget();
            if (pTarget->GetTypeId() == TYPEID_UNIT)
            {
                Creature* pCreature = (Creature*)pTarget;

                if (pCreature->GetEntry() == NPC_NEXUS_DRAKE_HATCHLING)
                {
                    pCreature->SetStandState(UNIT_STAND_STATE_SLEEP);
                    pCreature->ForcedDespawn(3000);
                }
            }
            return true;
        }
        case SPELL_ENRAGE:
        {
            if (!bApply || pAura->GetTarget()->GetTypeId() != TYPEID_UNIT)
                return false;

            Creature* pTarget = (Creature*)pAura->GetTarget();

            if (Creature* pCreature = GetClosestCreatureWithEntry(pTarget, NPC_DARKSPINE_MYRMIDON, 50.0f))
            {
                pTarget->AI()->AttackStart(pCreature);
                return true;
            }

            if (Creature* pCreature = GetClosestCreatureWithEntry(pTarget, NPC_DARKSPINE_SIREN, 50.0f))
            {
                pTarget->AI()->AttackStart(pCreature);
                return true;
            }

            return false;
        }
        case SPELL_SHROUD_OF_DEATH:
        case SPELL_SPIRIT_PARTICLES:
        {
            Creature* pCreature = (Creature*)pAura->GetTarget();

            if (!pCreature || (pCreature->GetEntry() != NPC_FRANCLORN_FORGEWRIGHT && pCreature->GetEntry() != NPC_GAERIYAN && pCreature->GetEntry()))
                return false;

            if (bApply)
                pCreature->m_AuraFlags |= UNIT_AURAFLAG_ALIVE_INVISIBLE;
            else
                pCreature->m_AuraFlags &= ~UNIT_AURAFLAG_ALIVE_INVISIBLE;

            return false;
        }
        case SPELL_PROTOVOLTAIC_MAGNETO_COLLECTOR:
        {
            if (pAura->GetEffIndex() != EFFECT_INDEX_0)
                return true;

            Unit* pTarget = pAura->GetTarget();
            if (bApply && pTarget->GetTypeId() == TYPEID_UNIT)
                ((Creature*)pTarget)->UpdateEntry(NPC_ENCASED_ELECTROMENTAL);
            return true;
        }
    }

    return false;
}

bool EffectDummyCreature_spell_dummy_npc(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    switch (uiSpellId)
    {
        case SPELL_ADMINISTER_ANTIDOTE:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCreatureTarget->GetEntry() != NPC_HELBOAR)
                    return true;

                // possible needs check for quest state, to not have any effect when quest really complete

                pCreatureTarget->UpdateEntry(NPC_DREADTUSK);
                return true;
            }
            return true;
        }
        case SPELL_DARKMENDER_TINCTURE:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCaster->GetTypeId() != TYPEID_PLAYER)
                    return true;

                // TODO: find/fix visual for effect, no related spells found doing this

                pCreatureTarget->CastSpell(pCreatureTarget, SPELL_SUMMON_CORRUPTED_SCARLET, TRIGGERED_OLD_TRIGGERED);

                ((Player*)pCaster)->KilledMonsterCredit(NPC_CORPSES_RISE_CREDIT_BUNNY);

                pCreatureTarget->ForcedDespawn();
                return true;
            }
            return true;
        }
        case SPELL_DISCIPLINING_ROD:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCreatureTarget->getStandState() == UNIT_STAND_STATE_STAND)
                    return true;

                switch (urand(1, 2))
                {
                    case 1:
                    {
                        switch (urand(1, 3))
                        {
                            case 1: DoScriptText(SAY_RAND_ATTACK1, pCreatureTarget); break;
                            case 2: DoScriptText(SAY_RAND_ATTACK2, pCreatureTarget); break;
                            case 3: DoScriptText(SAY_RAND_ATTACK3, pCreatureTarget); break;
                        }

                        pCreatureTarget->SetStandState(UNIT_STAND_STATE_STAND);
                        pCreatureTarget->AI()->AttackStart(pCaster);
                        break;
                    }
                    case 2:
                    {
                        switch (urand(1, 3))
                        {
                            case 1: DoScriptText(SAY_RAND_WORK1, pCreatureTarget); break;
                            case 2: DoScriptText(SAY_RAND_WORK2, pCreatureTarget); break;
                            case 3: DoScriptText(SAY_RAND_WORK3, pCreatureTarget); break;
                        }

                        pCreatureTarget->SetStandState(UNIT_STAND_STATE_STAND);
                        pCreatureTarget->HandleEmote(EMOTE_STATE_WORK);
                        break;
                    }
                }

                return true;
            }
            return true;
        }
        case SPELL_INOCULATE_OWLKIN:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCreatureTarget->GetEntry() != NPC_OWLKIN)
                    return true;

                pCreatureTarget->UpdateEntry(NPC_OWLKIN_INOC);
                pCreatureTarget->AIM_Initialize();
                ((Player*)pCaster)->KilledMonsterCredit(NPC_OWLKIN_INOC);

                // set despawn timer, since we want to remove creature after a short time
                pCreatureTarget->ForcedDespawn(15000);

                return true;
            }
            return true;
        }
        case SPELL_MODIFIED_MOJO:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCreatureTarget->GetEntry() != NPC_PROPHET_OF_SSERATUS)
                    return true;

                // Apparently done before updateEntry, so need to make a way to handle that
                // "Mmm, more mojo"
                // "%s drinks the Mojo"
                // "NOOOOOOOOOOOOooooooo...............!"

                pCreatureTarget->UpdateEntry(NPC_WEAK_PROPHET_OF_SSERATUS);
                return true;
            }
            return true;
        }
        case SPELL_FEL_SIPHON_DUMMY:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCreatureTarget->GetEntry() != NPC_FELBLOOD_INITIATE)
                    return true;

                pCreatureTarget->UpdateEntry(NPC_EMACIATED_FELBLOOD);
                return true;
            }
            return true;
        }
        case SPELL_SEEDS_OF_NATURES_WRATH:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                uint32 uiNewEntry = 0;

                switch (pCreatureTarget->GetEntry())
                {
                    case NPC_REANIMATED_FROSTWYRM:  uiNewEntry = NPC_WEAK_REANIMATED_FROSTWYRM; break;
                    case NPC_TURGID:                uiNewEntry = NPC_WEAK_TURGID; break;
                    case NPC_DEATHGAZE:             uiNewEntry = NPC_WEAK_DEATHGAZE; break;
                }

                if (uiNewEntry)
                    pCreatureTarget->UpdateEntry(uiNewEntry);

                return true;
            }
            return true;
        }
        case SPELL_STRENGTH_ANCIENTS:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCaster->GetTypeId() == TYPEID_PLAYER)
                {
                    if (urand(0, 1))
                    {
                        DoScriptText(EMOTE_AGGRO, pCreatureTarget);
                        pCreatureTarget->setFaction(FACTION_HOSTILE);
                        pCreatureTarget->AI()->AttackStart(pCaster);
                    }
                    else
                    {
                        DoScriptText(EMOTE_CREATE, pCreatureTarget);
                        pCaster->CastSpell(pCaster, SPELL_CREATE_BARK_WALKERS, TRIGGERED_OLD_TRIGGERED);
                        pCreatureTarget->ForcedDespawn(5000);
                    }
                }
                return true;
            }
            return true;
        }
        case SPELL_TAG_MURLOC_PROC:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCreatureTarget->GetEntry() == NPC_BLACKSILT_MURLOC)
                    pCreatureTarget->UpdateEntry(NPC_TAGGED_MURLOC);
            }
            return true;
        }
        case SPELL_ULTRASONIC_SCREWDRIVER:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCreatureTarget->IsCorpse())
                {
                    uint32 newSpellId = 0;

                    switch (pCreatureTarget->GetEntry())
                    {
                        case NPC_COLLECT_A_TRON:    newSpellId = SPELL_SUMMON_COLLECT_A_TRON; break;
                        case NPC_DEFENDO_TANK:      newSpellId = SPELL_SUMMON_DEFENDO_TANK; break;
                        case NPC_SCAVENGE_A8:       newSpellId = SPELL_SUMMON_SCAVENGE_A8; break;
                        case NPC_SCAVENGE_B6:       newSpellId = SPELL_SUMMON_SCAVENGE_B6; break;
                        case NPC_SENTRY_BOT:        newSpellId = SPELL_SUMMON_SENTRY_BOT; break;
                    }

                    if (const SpellEntry* pSpell = GetSpellStore()->LookupEntry<SpellEntry>(newSpellId))
                    {
                        pCaster->CastSpell(pCreatureTarget, pSpell->Id, TRIGGERED_OLD_TRIGGERED);

                        if (Pet* pPet = pCaster->FindGuardianWithEntry(pSpell->EffectMiscValue[uiEffIndex]))
                            pPet->CastSpell(pCaster, SPELL_REPROGRAM_KILL_CREDIT, TRIGGERED_OLD_TRIGGERED);

                        pCreatureTarget->ForcedDespawn();
                    }
                }
                return true;
            }
            return true;
        }
        case SPELL_ORB_OF_MURLOC_CONTROL:
        {
            pCreatureTarget->CastSpell(pCaster, SPELL_GREENGILL_SLAVE_FREED, TRIGGERED_OLD_TRIGGERED);

            // Freed Greengill Slave
            pCreatureTarget->UpdateEntry(NPC_FREED_GREENGILL_SLAVE);

            pCreatureTarget->CastSpell(pCreatureTarget, SPELL_ENRAGE, TRIGGERED_OLD_TRIGGERED);

            return true;
        }
        case SPELL_AHUNAES_KNIFE:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCaster->GetTypeId() != TYPEID_PLAYER)
                    return true;

                ((Player*)pCaster)->KilledMonsterCredit(NPC_SCALPS_KILL_CREDIT_BUNNY);
                pCreatureTarget->ForcedDespawn();
                return true;
            }
            return true;
        }
        case SPELL_TAILS_UP_GENDER_MASTER:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                const bool isMale = urand(0, 1) != 0;
                Player* pPlayer = pCreatureTarget->GetLootRecipient();

                if (isMale)
                    DoScriptText(SAY_ITS_MALE, pCreatureTarget, pPlayer);
                else
                    DoScriptText(SAY_ITS_FEMALE, pCreatureTarget, pPlayer);

                switch (pCreatureTarget->GetEntry())
                {
                    case NPC_FROST_LEOPARD:
                    {
                        if (isMale)
                        {
                            pCreatureTarget->CastSpell(pCreatureTarget, SPELL_TAILS_UP_AURA, TRIGGERED_OLD_TRIGGERED);
                            pCreatureTarget->RemoveAurasDueToSpell(62248);
                            pCreatureTarget->AI()->AttackStart(pPlayer);
                        }
                        else
                        {
                            pPlayer->KilledMonsterCredit(NPC_LEOPARD_KILL_CREDIT, pCreatureTarget->GetObjectGuid());
                            pCreatureTarget->CastSpell(pPlayer, SPELL_FORCE_LEOPARD_SUMMON, TRIGGERED_OLD_TRIGGERED);
                            pCreatureTarget->ForcedDespawn();
                        }

                        break;
                    }
                    case NPC_ICEPAW_BEAR:
                    {
                        if (isMale)
                        {
                            pCreatureTarget->CastSpell(pCreatureTarget, SPELL_TAILS_UP_AURA, TRIGGERED_OLD_TRIGGERED);
                            pCreatureTarget->RemoveAurasDueToSpell(62248);
                            pCreatureTarget->AI()->AttackStart(pPlayer);
                        }
                        else
                        {
                            pPlayer->KilledMonsterCredit(NPC_BEAR_KILL_CREDIT, pCreatureTarget->GetObjectGuid());
                            pCreatureTarget->CastSpell(pPlayer, SPELL_FORCE_BEAR_SUMMON, TRIGGERED_OLD_TRIGGERED);
                            pCreatureTarget->ForcedDespawn();
                        }

                        break;
                    }
                }
                return true;
            }
            return true;
        }
        case SPELL_THROW_GORDAWG_BOULDER:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                for (int i = 0; i < 3; ++i)
                {
                    if (irand(i, 2))                        // 2-3 summons
                        pCreatureTarget->SummonCreature(NPC_MINION_OF_GUROK, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSPAWN_CORPSE_DESPAWN, 5000);
                }

                pCreatureTarget->CastSpell(nullptr, 3617, TRIGGERED_OLD_TRIGGERED); // suicide spell
                return true;
            }
            return true;
        }
        case SPELL_HIT_APPLE:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCaster->GetTypeId() == TYPEID_PLAYER)
                    ((Player*)pCaster)->KilledMonsterCredit(pCreatureTarget->GetEntry(), pCreatureTarget->GetObjectGuid());

                pCreatureTarget->CastSpell(pCreatureTarget, SPELL_APPLE_FALLS_TO_GROUND, TRIGGERED_NONE);

                if (Creature* pLuckyWilhelm = GetClosestCreatureWithEntry(pCreatureTarget, NPC_LUCKY_WILHELM, 2 * INTERACTION_DISTANCE))
                    DoScriptText(SAY_LUCKY_HIT_APPLE, pLuckyWilhelm);
            }
            return true;
        }
        case SPELL_MISS_APPLE:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                switch (urand(1, 3))
                {
                    case 1: DoScriptText(SAY_LUCKY_HIT_1, pCreatureTarget); break;
                    case 2: DoScriptText(SAY_LUCKY_HIT_2, pCreatureTarget); break;
                    case 3: DoScriptText(SAY_LUCKY_HIT_3, pCreatureTarget); break;
                }

                if (Creature* pDrostan = GetClosestCreatureWithEntry(pCreatureTarget, NPC_DROSTAN, 4 * INTERACTION_DISTANCE))
                    DoScriptText(urand(0, 1) ? SAY_DROSTAN_GOT_LUCKY_1 : SAY_DROSTAN_GOT_LUCKY_2, pDrostan);
            }
            return true;
        }
        case SPELL_MISS_APPLE_HIT_BIRD:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (Creature* pDrostan = GetClosestCreatureWithEntry(pCreatureTarget, NPC_DROSTAN, 5 * INTERACTION_DISTANCE))
                    DoScriptText(urand(0, 1) ? SAY_DROSTAN_HIT_BIRD_1 : SAY_DROSTAN_HIT_BIRD_2, pDrostan);

                pCreatureTarget->Suicide();
            }
            return true;
        }
        case SPELL_LURIELLES_PENDANT:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCreatureTarget->GetEntry() != NPC_CHILL_NYMPH || pCaster->GetTypeId() != TYPEID_PLAYER)
                    return true;

                switch (urand(0, 2))
                {
                    case 0: DoScriptText(SAY_FREE_1, pCreatureTarget); break;
                    case 1: DoScriptText(SAY_FREE_2, pCreatureTarget); break;
                    case 2: DoScriptText(SAY_FREE_3, pCreatureTarget); break;
                }

                ((Player*)pCaster)->KilledMonsterCredit(NPC_LURIELLE);
                pCreatureTarget->SetFactionTemporary(FACTION_FRIENDLY, TEMPFACTION_RESTORE_RESPAWN);
                pCreatureTarget->DeleteThreatList();
                pCreatureTarget->AttackStop(true);
                pCreatureTarget->GetMotionMaster()->MoveFleeing(pCaster, 7);
                pCreatureTarget->ForcedDespawn(7 * IN_MILLISECONDS);
            }
            return true;
        }
        case SPELL_MELODIOUS_RAPTURE:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCaster->GetTypeId() != TYPEID_PLAYER && pCreatureTarget->GetEntry() != NPC_DEEPRUN_RAT)
                    return true;

                pCreatureTarget->UpdateEntry(NPC_ENTHRALLED_DEEPRUN_RAT);
                pCreatureTarget->CastSpell(pCreatureTarget, SPELL_MELODIOUS_RAPTURE_VISUAL, TRIGGERED_NONE);
                pCreatureTarget->GetMotionMaster()->MoveFollow(pCaster, frand(0.5f, 3.0f), frand(M_PI_F * 0.8f, M_PI_F * 1.2f));

                ((Player*)pCaster)->KilledMonsterCredit(NPC_ENTHRALLED_DEEPRUN_RAT);
            }
            return true;
        }
        case SPELL_THROW_ICE:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCreatureTarget->GetEntry() != NPC_SMOLDERING_SCRAP_BUNNY)
                    return true;

                if (GameObject* pScrap = GetClosestGameObjectWithEntry(pCreatureTarget, GO_SMOLDERING_SCRAP, 5.0f))
                {
                    if (pScrap->GetRespawnTime() != 0)
                        return true;

                    pCreatureTarget->CastSpell(pCreatureTarget, SPELL_FROZEN_IRON_SCRAP, TRIGGERED_OLD_TRIGGERED);
                    pScrap->SetLootState(GO_JUST_DEACTIVATED);
                    pCreatureTarget->ForcedDespawn(1000);
                }
            }
            return true;
        }
    }

    return false;
}

struct GreaterInvisibilityMob : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->ForcePeriodicity(1 * IN_MILLISECONDS); // tick every second
    }

    void OnPeriodicTickEnd(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target->IsCreature())
            return;

        Creature* invisible = static_cast<Creature*>(target);
        std::list<Unit*> nearbyTargets;
        MaNGOS::AnyUnitInObjectRangeCheck u_check(invisible, float(invisible->GetDetectionRange()));
        MaNGOS::UnitListSearcher<MaNGOS::AnyUnitInObjectRangeCheck> searcher(nearbyTargets, u_check);
        Cell::VisitWorldObjects(invisible, searcher, invisible->GetDetectionRange());
        for (Unit* nearby : nearbyTargets)
        {
            if (invisible->CanAttackOnSight(nearby) && invisible->IsWithinLOSInMap(nearby, true))
            {
                invisible->AI()->AttackStart(nearby);
                return;
            }
        }
    }
};

struct InebriateRemoval : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target->IsPlayer())
            return;

        SpellEffectIndex effIdx;
        SpellEffectIndex effIdxInebriate;
        switch (aura->GetSpellProto()->Id)
        {
            case 29690: effIdx = EFFECT_INDEX_1; effIdxInebriate = EFFECT_INDEX_2; break;
            case 37591: effIdx = EFFECT_INDEX_0; effIdxInebriate = EFFECT_INDEX_1; break;
            default: return;
        }
        Player* player = static_cast<Player*>(target);
        if (!apply && aura->GetEffIndex() == effIdx)
            player->SetDrunkValue(uint16(std::max(int32(player->GetDrunkValue()) - player->CalculateSpellEffectValue(player, aura->GetSpellProto(), effIdxInebriate) * 256, 0)));
    }
};

struct AstralBite : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (Unit* caster = spell->GetCaster())
            caster->getThreatManager().modifyAllThreatPercent(-100);
    }
};

struct FelInfusion : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(1);
        spell->SetFilteringScheme(EFFECT_INDEX_0, true, SCHEME_CLOSEST);
    }

    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (!target->IsInCombat())
            return false;
        return true;
    }
};

enum
{
    SPELL_POSSESS       = 32830,
    SPELL_POSSESS_BUFF  = 32831,
    SPELL_POSSESS_INVIS = 32832,
    SPELL_KNOCKDOWN     = 13360,
};

struct AuchenaiPossess : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
        {
            Unit* caster = aura->GetCaster();
            if (caster)
                caster->CastSpell(nullptr, SPELL_POSSESS_INVIS, TRIGGERED_OLD_TRIGGERED);
            aura->GetTarget()->CastSpell(nullptr, SPELL_POSSESS_BUFF, TRIGGERED_OLD_TRIGGERED);
            aura->ForcePeriodicity(1000);
        }
        else
        {
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_POSSESS_BUFF);
            aura->GetTarget()->CastSpell(aura->GetTarget(), SPELL_KNOCKDOWN, TRIGGERED_OLD_TRIGGERED);
            if (Unit* caster = aura->GetCaster())
                if (caster->IsCreature())
                    static_cast<Creature*>(caster)->ForcedDespawn();
        }
    }

    void OnPeriodicTickEnd(Aura* aura) const override
    {
        if (aura->GetTarget()->GetHealthPercent() < 50.f)
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_POSSESS);
    }
};

struct GettingSleepyAura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
            aura->GetTarget()->CastSpell(nullptr, 34801, TRIGGERED_OLD_TRIGGERED); // Sleep
    }
};

struct AllergiesAura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->ForcePeriodicity(10 * IN_MILLISECONDS);
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        if (urand(0, 2) > 0)
            aura->GetTarget()->CastSpell(nullptr, 31428, TRIGGERED_OLD_TRIGGERED); // Sneeze
    }
};

enum
{
    SPELL_USE_CORPSE = 33985,
};

struct RaiseDead : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(1);
    }

    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (!target->IsCreature() || static_cast<Creature*>(target)->HasBeenHitBySpell(SPELL_USE_CORPSE))
            return false;

        return true;
    }
};

struct UseCorpse : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsCreature())
            return;

        static_cast<Creature*>(target)->RegisterHitBySpell(SPELL_USE_CORPSE);
    }
};

struct SplitDamage : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (spell->m_spellInfo->Effect[effIdx] != SPELL_EFFECT_SCHOOL_DAMAGE)
            return;

        uint32 count = 0;
        auto& targetList = spell->GetTargetList();
        for (Spell::TargetList::const_iterator ihit = targetList.begin(); ihit != targetList.end(); ++ihit)
            if (ihit->effectHitMask & (1 << effIdx))
                ++count;

        spell->SetDamage(spell->GetDamage() / count); // divide to all targets
    }
};

struct TKDive : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (spell->m_spellInfo->Effect[effIdx] != SPELL_EFFECT_SCHOOL_DAMAGE)
            return;

        Unit* target = spell->GetUnitTarget();
        spell->GetCaster()->AddThreat(target, 1000000.f);
    }
};

struct CurseOfPain : public AuraScript
{
    void OnPeriodicTickEnd(Aura* aura) const override
    {
        if (aura->GetTarget()->GetHealthPercent() < 50.f)
            aura->GetTarget()->RemoveAurasDueToSpell(aura->GetId());
    }
};

enum SeedOfCorruptionNpc
{
    SPELL_SEED_OF_CORRUPTION_PROC_DEFAULT   = 32865,
    SPELL_SEED_OF_CORRUPTION_NPC_24558      = 44141,
    SPELL_SEED_OF_CORRUPTION_PROC_NPC_24558 = 43991,
};

struct spell_seed_of_corruption_npc : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_1)
            return SPELL_AURA_PROC_OK;
        Modifier* mod = procData.triggeredByAura->GetModifier();
        // if damage is more than need deal finish spell
        if (mod->m_amount <= (int32)procData.damage)
        {
            // remember guid before aura delete
            ObjectGuid casterGuid = procData.triggeredByAura->GetCasterGuid();

            int32 basePoints = 2000; // guesswork, need to fill for all spells that use this because its not in spell data

            // Remove aura (before cast for prevent infinite loop handlers)
            procData.victim->RemoveAurasByCasterSpell(procData.triggeredByAura->GetId(), procData.triggeredByAura->GetCasterGuid());

            // Cast finish spell (triggeredByAura already not exist!)
            uint32 triggered_spell_id = 0;
            switch (aura->GetSpellProto()->Id)
            {
                case SPELL_SEED_OF_CORRUPTION_NPC_24558: triggered_spell_id = SPELL_SEED_OF_CORRUPTION_PROC_NPC_24558; break;
                default: triggered_spell_id = SPELL_SEED_OF_CORRUPTION_PROC_DEFAULT; break;
            }
            if (Unit* caster = procData.triggeredByAura->GetCaster())
                caster->CastCustomSpell(procData.victim, triggered_spell_id, &basePoints, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);

            return SPELL_AURA_PROC_OK;              // no hidden cooldown
        }

        // Damage counting
        mod->m_amount -= procData.damage;
        return SPELL_AURA_PROC_OK;
    }
};

// PX-238 Winter Wondervolt TRAP
struct WondervoltTrap : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_0)
        {
            if (Unit* target = spell->GetUnitTarget())
            {
                if (WorldObject* source = spell->GetCastingObject())
                    if (!source->IsWithinDist(target, 1.0f))
                        return;

                if (spell->GetUnitTarget()->getGender() == GENDER_MALE)
                {
                    target->RemoveAurasDueToSpell(26157);
                    target->RemoveAurasDueToSpell(26273);
                    target->CastSpell(target, urand(0, 1) ? 26157 : 26273, TRIGGERED_OLD_TRIGGERED);
                }
                else
                {
                    target->RemoveAurasDueToSpell(26272);
                    target->RemoveAurasDueToSpell(26274);
                    target->CastSpell(target, urand(0, 1) ? 26272 : 26274, TRIGGERED_OLD_TRIGGERED);
                }
            }
            return;
        }
    }
};

// wotlk section

struct deflection : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool/* strict*/) const override
    {
        if (spell->GetCaster()->GetHealthPercent() > 35.f)
            return SPELL_FAILED_FIZZLE;
        return SPELL_CAST_OK;
    }
};

/*######
## spell_eject_all_passengers - 50630
######*/

struct spell_eject_all_passengers : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        if (!caster)
            return;

        caster->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
    }
};

enum SpellVisualKitFoodOrDrink
{
    SPELL_VISUAL_KIT_FOOD = 406,
    SPELL_VISUAL_KIT_DRINK = 438
};

struct FoodAnimation : public AuraScript
{
    void OnHeartbeat(Aura* aura) const override
    {
        // standing up cancels immediately but doing /sleep or /kneel interrupts on next tick
        if (!aura->GetTarget()->IsSitState())
        {
            aura->GetTarget()->RemoveAurasDueToSpell(aura->GetId());
            return;
        }

        aura->GetTarget()->PlaySpellVisual(SPELL_VISUAL_KIT_FOOD);
    }
};

struct DrinkAnimation : public AuraScript
{
    void OnHeartbeat(Aura* aura) const override
    {
        // standing up cancels immediately but doing /sleep or /kneel interrupts on next tick
        if (!aura->GetTarget()->IsSitState())
        {
            aura->GetTarget()->RemoveAurasDueToSpell(aura->GetId());
            return;
        }

        aura->GetTarget()->PlaySpellVisual(SPELL_VISUAL_KIT_DRINK);
    }
};

struct Drink : public DrinkAnimation
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply || aura->GetEffIndex() != EFFECT_INDEX_0)
            return;

        if (!aura->GetTarget()->IsPlayer())
            return;

        if (aura->GetTarget()->GetMap()->IsBattleArena())
            return;

        if (Aura* periodicAura = aura->GetHolder()->GetAuraByEffectIndex((SpellEffectIndex)(aura->GetEffIndex() + 1)))
            aura->GetModifier()->m_amount = periodicAura->GetModifier()->m_amount;
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_1)
            return;

        if (!aura->GetTarget()->IsPlayer())
            return;

        if (!aura->GetTarget()->GetMap()->IsBattleArena())
            return;

        Aura* regenAura = aura->GetHolder()->m_auras[EFFECT_INDEX_0];
        if (!regenAura)
            return;

        // **********************************************
        // This feature used only in arenas
        // **********************************************
        // Here need increase mana regen per tick (6 second rule)
        // on 1 tick -   0  (handled in 2 second)
        // on 2 tick - 166% (handled in 4 second)
        // on 3 tick - 133% (handled in 6 second)

        int32 resultingAmount = 0;
        // Apply bonus for 1 - 4 tick
        switch (aura->GetAuraTicks())
        {
            case 1:   // 0%
                resultingAmount = aura->GetAmount() * 5 / 3;
                break;
            case 2:   // 166%
                resultingAmount = aura->GetAmount() * 4 / 3;
                break;
            default:  // 100% - normal regen
                resultingAmount = aura->GetAmount();
                // No need to update after 4th tick
                aura->ForcePeriodicity(0);
                break;
        }

        regenAura->GetModifier()->m_amount = resultingAmount;
        ((Player*)aura->GetTarget())->UpdateManaRegen();
    }
};

struct spell_effect_summon_no_follow_movement : public SpellScript
{
    void OnSummon(Spell* /*spell*/, Creature* summon) const override
    {
        summon->AI()->SetFollowMovement(false);
    }
};

struct SpellHasteHealerTrinket : public AuraScript
{
    bool OnCheckProc(Aura* /*aura*/, ProcExecutionData& data) const override
    {
        // should only proc off of direct heals or HoT applications
        if (data.spell && (data.isHeal || IsSpellHaveAura(data.spellInfo, SPELL_AURA_PERIODIC_HEAL)))
            return true;

        return false;
    }
};

struct IncreasedHealingDoneDummy : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const
    {
        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_HEALING_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*victim*/, int32& advertisedBenefit, float& /*totalMod*/) const override
    {
        advertisedBenefit += aura->GetModifier()->m_amount;
    }
};

struct spell_scourge_strike : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->IsPlayer() || (target->IsPlayerControlled()))
            return false;

        return true;
    }
};

enum
{
    SPELL_THISTLEFUR_DEATH = 8603,
    SPELL_RIVERPAW_DEATH   = 8655,
    SPELL_STROMGARDE_DEATH = 8894,
    SPELL_CRUSHRIDGE_DEATH = 9144,

    SAY_RAGE_FALLEN        = 1151,
};

struct TribalDeath : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        uint32 entry = 0;
        switch (spell->m_spellInfo->Id)
        {
            case SPELL_THISTLEFUR_DEATH: entry = 3925; break; // Thistlefur Avenger
            case SPELL_RIVERPAW_DEATH: entry = 0; break; // Unk
            case SPELL_STROMGARDE_DEATH: entry = 2585; break; // Stromgarde Vindicator
            case SPELL_CRUSHRIDGE_DEATH: entry = 2287; break; // Crushridge Warmonger
        }
        if (target->GetEntry() != entry)
            return false;
        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        uint32 spellId = 0;
        switch (spell->m_spellInfo->Id)
        {
            case SPELL_THISTLEFUR_DEATH: spellId = 8602; break;
            case SPELL_RIVERPAW_DEATH: spellId = 0; break; // Unk
            case SPELL_STROMGARDE_DEATH: spellId = 8602; break;
            case SPELL_CRUSHRIDGE_DEATH: spellId = 8269; break;
        }
        Unit* target = spell->GetUnitTarget();
        Unit* caster = spell->GetCaster();
        target->CastSpell(nullptr, spellId, TRIGGERED_OLD_TRIGGERED);
        if (!target->IsInCombat())
            if (Unit* killer = target->GetMap()->GetUnit(static_cast<Creature*>(target)->GetKillerGuid()))
                target->AI()->AttackStart(killer);

        if (spell->m_spellInfo->Id == SPELL_CRUSHRIDGE_DEATH)
            DoBroadcastText(SAY_RAGE_FALLEN, target, caster);
    }
};

struct RetaliationCreature : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        if (!spell->m_targets.getUnitTarget() || !spell->GetCaster()->HasInArc(spell->m_targets.getUnitTarget()))
            return SPELL_FAILED_CASTER_AURASTATE;

        return SPELL_CAST_OK;
    }
};

struct HateToHalf : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        spell->GetCaster()->getThreatManager().modifyThreatPercent(spell->GetUnitTarget(), -50);
    }
};

struct HateToZero : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        spell->GetCaster()->getThreatManager().modifyThreatPercent(spell->GetUnitTarget(), -100);
    }
};

struct PreventSpellIfSameAuraOnCaster : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        if (spell->GetCaster()->HasAura(spell->m_spellInfo->Id))
            return SPELL_FAILED_CASTER_AURASTATE;

        return SPELL_CAST_OK;
    }
};

struct InstillLordValthalaksSpirit : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        spell->GetCaster()->AddCreature(spell->m_spellInfo->Id, summon);
    }
};

struct Stoned : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (apply)
        {
            if (target->GetTypeId() != TYPEID_UNIT)
                return;

            if (target->GetEntry() == 25507)
                return;

            target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_UNINTERACTIBLE);
            target->addUnitState(UNIT_STAT_ROOT);
        }
        else
        {
            if (target->GetTypeId() != TYPEID_UNIT)
                return;

            if (target->GetEntry() == 25507)
                return;

            // see dummy effect of spell 10254 for removal of flags etc
            target->CastSpell(nullptr, 10254, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct BirthNoVisualInstantSpawn : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        spell->GetCaster()->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DO_NOT_FADE_IN);
    }
};

struct SleepVisualFlavor : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (apply)
            target->SetStandState(UNIT_STAND_STATE_SLEEP);
        else
            target->SetStandState(UNIT_STAND_STATE_STAND);
    }
};

enum spell_call_of_the_falcon
{
    YELL_KILL_FALCONER          = 17624, // Kill $n!
    NPC_BLOODWARDER_FALCONER    = 17994,
    NPC_BLOODFALCON             = 18155,
    SPELL_CALL_OF_THE_FALCON    = 34853,
};

struct CallOfTheFalcon : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
        {
            DoBroadcastText(YELL_KILL_FALCONER, aura->GetCaster(), aura->GetTarget());
            aura->GetTarget()->CastSpell(nullptr, SPELL_CALL_OF_THE_FALCON, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

// 36435 - Forget                                                               // Unlearn Armorsmith specialization
struct ForgetArmorsmith : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Player* player = static_cast<Player*>(spell->GetUnitTarget());
        player->removeSpell(36122);   // Earthforged Leggings
        player->removeSpell(36129);   // Heavy Earthforged Breastplate
        player->removeSpell(36130);   // Stormforged Hauberk
        player->removeSpell(34533);   // Breastplate of Kings
        player->removeSpell(34529);   // Nether Chain Shirt
        player->removeSpell(34534);   // Bulwark of Kings
        player->removeSpell(36257);   // Bulwark of the Ancient Kings
        player->removeSpell(36256);   // Embrace of the Twisting Nether
        player->removeSpell(34530);   // Twisting Nether Chain Shirt
        player->removeSpell(36124);   // Windforged Leggings
    }
};

// 36436 - Forget                                                               // Unlearn Weaponsmith specialization
struct ForgetWeaponsmith : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Player* player = static_cast<Player*>(spell->GetUnitTarget());
        player->removeSpell(36125);   // Light Earthforged Blade
        player->removeSpell(36128);   // Light Emberforged Hammer
        player->removeSpell(36126);   // Light Skyforged Axe
    }
};

// 36438 - Forget                                                               // Unlearn Swordsmith specialization
struct ForgetSwordsmith : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Player* player = static_cast<Player*>(spell->GetUnitTarget());
        player->removeSpell(36258);   // Blazefury
        player->removeSpell(34537);   // Blazeguard
        player->removeSpell(34535);   // Fireguard
        player->removeSpell(36131);   // Windforged Rapier
        player->removeSpell(36133);   // Stoneforged Claymore
        player->removeSpell(34538);   // Lionheart Blade
        player->removeSpell(34540);   // Lionheart Champion
        player->removeSpell(36259);   // Lionheart Executioner
    }
};

// 36439 - Forget                                                               // Unlearn Axesmith specialization
struct ForgetAxesmith : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Player* player = static_cast<Player*>(spell->GetUnitTarget());
        player->removeSpell(36260);   // Wicked Edge of the Planes
        player->removeSpell(34562);   // Black Planar Edge
        player->removeSpell(34541);   // The Planar Edge
        player->removeSpell(36134);   // Stormforged Axe
        player->removeSpell(36135);   // Skyforged Great Axe
        player->removeSpell(36261);   // Bloodmoon
        player->removeSpell(34543);   // Lunar Crescent
        player->removeSpell(34544);   // Mooncleaver
    }
};

// 36441 - Forget                                                               // Unlearn Hammersmith specialization
struct ForgetHammersmith : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Player* player = static_cast<Player*>(spell->GetUnitTarget());
        player->removeSpell(36262);   // Dragonstrike
        player->removeSpell(34546);   // Dragonmaw
        player->removeSpell(34545);   // Drakefist Hammer
        player->removeSpell(36136);   // Lavaforged Warhammer
        player->removeSpell(34547);   // Thunder
        player->removeSpell(34567);   // Deep Thunder
        player->removeSpell(36263);   // Stormherald
        player->removeSpell(36137);   // Great Earthforged Hammer
    }
};

struct GameobjectCallForHelpOnUsage : public SpellScript
{
    void OnSuccessfulStart(Spell* spell) const
    {
        UnitList targets;
        MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck check(spell->GetCaster(), 12.f);
        MaNGOS::UnitListSearcher<MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck> searcher(targets, check);
        Cell::VisitAllObjects(spell->GetCaster(), searcher, 12.f);
        for (Unit* attacker : targets)
        {
            if (attacker->IsCreature() && static_cast<Creature*>(attacker)->IsCritter())
                continue;

            if (!spell->GetCaster()->IsEnemy(attacker))
                continue;

            if (attacker->AI())
                attacker->AI()->AttackStart(spell->GetCaster());
        }
    }
};

struct Submerged : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget)
            return;

        unitTarget->SetStandState(UNIT_STAND_STATE_CUSTOM);
        unitTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
    }
};

struct Stand : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget)
            return;

        unitTarget->SetStandState(UNIT_STAND_STATE_STAND);
        unitTarget->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
    }
};

void AddSC_spell_scripts()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "spell_dummy_go";
    pNewScript->pEffectDummyGO = &EffectDummyGameObj_spell_dummy_go;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "spell_dummy_npc";
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_spell_dummy_npc;
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_spell_aura_dummy_npc;
    pNewScript->RegisterSelf();

    RegisterSpellScript<GreaterInvisibilityMob>("spell_greater_invisibility_mob");
    RegisterSpellScript<InebriateRemoval>("spell_inebriate_removal");
    RegisterSpellScript<AstralBite>("spell_astral_bite");
    RegisterSpellScript<FelInfusion>("spell_fel_infusion");
    RegisterSpellScript<AuchenaiPossess>("spell_auchenai_possess");
    RegisterSpellScript<GettingSleepyAura>("spell_getting_sleepy_aura");
    RegisterSpellScript<AllergiesAura>("spell_allergies");
    RegisterSpellScript<UseCorpse>("spell_use_corpse");
    RegisterSpellScript<RaiseDead>("spell_raise_dead");
    RegisterSpellScript<SplitDamage>("spell_split_damage");
    RegisterSpellScript<TKDive>("spell_tk_dive");
    RegisterSpellScript<CurseOfPain>("spell_curse_of_pain");
    RegisterSpellScript<spell_seed_of_corruption_npc>("spell_seed_of_corruption_npc");
    RegisterSpellScript<deflection>("spell_deflection");
    RegisterSpellScript<spell_eject_all_passengers>("spell_eject_all_passengers");
    RegisterSpellScript<WondervoltTrap>("spell_wondervolt_trap");
    RegisterSpellScript<FoodAnimation>("spell_food_animation");
    RegisterSpellScript<DrinkAnimation>("spell_drink_animation");
    RegisterSpellScript<Drink>("spell_drink");
    RegisterSpellScript<spell_effect_summon_no_follow_movement>("spell_effect_summon_no_follow_movement");
    RegisterSpellScript<SpellHasteHealerTrinket>("spell_spell_haste_healer_trinket");
    RegisterSpellScript<IncreasedHealingDoneDummy>("spell_increased_healing_done_dummy");
    RegisterSpellScript<spell_scourge_strike>("spell_scourge_strike");
    RegisterSpellScript<TribalDeath>("spell_tribal_death");
    RegisterSpellScript<PreventSpellIfSameAuraOnCaster>("spell_prevent_spell_if_same_aura_on_caster");
    RegisterSpellScript<InstillLordValthalaksSpirit>("spell_instill_lord_valthalaks_spirit");
    RegisterSpellScript<RetaliationCreature>("spell_retaliation_creature");
    RegisterSpellScript<HateToHalf>("spell_hate_to_half");
    RegisterSpellScript<HateToZero>("spell_hate_to_zero");
    RegisterSpellScript<Stoned>("spell_stoned");
    RegisterSpellScript<BirthNoVisualInstantSpawn>("spell_birth_no_visual_instant_spawn");
    RegisterSpellScript<SleepVisualFlavor>("spell_sleep_visual_flavor");
    RegisterSpellScript<CallOfTheFalcon>("spell_call_of_the_falcon");
    RegisterSpellScript<GameobjectCallForHelpOnUsage>("spell_gameobject_call_for_help_on_usage");
    RegisterSpellScript<ForgetArmorsmith>("spell_forget_36435");
    RegisterSpellScript<ForgetWeaponsmith>("spell_forget_36436");
    RegisterSpellScript<ForgetSwordsmith>("spell_forget_36438");
    RegisterSpellScript<ForgetAxesmith>("spell_forget_36439");
    RegisterSpellScript<ForgetHammersmith>("spell_forget_36441");
    RegisterSpellScript<Submerged>("spell_submerged");
    RegisterSpellScript<Stand>("spell_stand");
}

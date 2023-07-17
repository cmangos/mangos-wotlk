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
SDName: Guards
SD%Complete: 100
SDComment: CombatAI should be organized better for future. Quest support 13188 / 13189.
SDCategory: Guards
EndScriptData */

/* ContentData
guard_azuremyst
guard_bluffwatcher
guard_contested
guard_darnassus
guard_dunmorogh
guard_durotar
guard_elwynnforest
guard_eversong
guard_exodar
guard_ironforge
guard_mulgore
guard_orgrimmar
guard_shattrath
guard_shattrath_aldor
guard_shattrath_scryer
guard_silvermoon
guard_stormwind
guard_teldrassil
guard_tirisfal
guard_undercity
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/guard_ai.h"
#include "GameEvents/GameEventMgr.h"

struct guard_hallows_end_helperAI : public guardAI
{
    guard_hallows_end_helperAI(Creature* creature) : guardAI(creature)
    {
        Reset();
    }

    GuidList lPracticeFires;
    uint32 extinguishedCount;
    uint32 checkExtinguishedTimer;
    uint32 relitCount;
    uint32 relightTimer;
    bool relightingFires;
    bool isSpawnedForHallowsEnd;
    Position spawnPos = m_creature->GetRespawnPosition();

    void Reset() override
    {
        isSpawnedForHallowsEnd = false;
        GameEventCreatureData const* eventData = sGameEventMgr.GetCreatureUpdateDataForActiveEvent(m_creature->GetDbGuid());
        if (eventData)
        {
            relightingFires = false;
            checkExtinguishedTimer = 1000;
            extinguishedCount = 0;
            relightTimer = 0;
            relitCount = 0;
            m_creature->SetWalk(false);
            m_creature->SetActiveObjectState(true);
            SetReactState(REACT_DEFENSIVE);
            isSpawnedForHallowsEnd = true;
        }
    }

    void MovementInform(uint32 motionType, uint32 point) override
    {
        if (motionType != POINT_MOTION_TYPE)
            return;

        if (point)
        {
            m_creature->GetMotionMaster()->MoveIdle();
            relightTimer = 5000;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (isSpawnedForHallowsEnd)
        {
            if (relightingFires)
            {
                if (relightTimer)
                {
                    if (relightTimer < diff)
                    {
                        relightTimer = 3000;

                        if (DoCastSpellIfCan(nullptr, SPELL_START_FIRE_TARGET_TEST_GUARD, CAST_TRIGGERED) == CAST_OK)
                            relitCount++;

                        if (relitCount == lPracticeFires.size())
                        {
                            relitCount = 0;
                            relightingFires = false;
                            checkExtinguishedTimer = 3000;
                        }
                    }
                    else
                        relightTimer -= diff;
                }
            }
            else
            {
                if (checkExtinguishedTimer)
                {
                    if (checkExtinguishedTimer < diff)
                    {
                        // go home if necessary
                        if (m_creature->GetPositionX() != spawnPos.x || m_creature->GetPositionY() != spawnPos.y || m_creature->GetPositionZ() != spawnPos.z)
                        {
                            m_creature->GetMotionMaster()->MovePoint(0, Position(spawnPos.x, spawnPos.y, spawnPos.z, spawnPos.o));
                            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        }

                        checkExtinguishedTimer = 3000;
                        extinguishedCount = 0;
                        if (lPracticeFires.size() < 3)
                        {
                            std::list<Creature*> lPracticeFireCreatures;
                            GetCreatureListWithEntryInGrid(lPracticeFireCreatures, m_creature, NPC_HEADLESS_HORSEMAN_FIRE, 65.f);
                            for (auto& fire : lPracticeFireCreatures)
                                lPracticeFires.push_back(fire->GetObjectGuid());
                        }
                        for (ObjectGuid fireGuid : lPracticeFires)
                            if (Creature* fire = m_creature->GetMap()->GetCreature(fireGuid))
                                if (!fire->HasAura(SPELL_FIRE))
                                    extinguishedCount++;

                        if (extinguishedCount == lPracticeFires.size())
                        {
                            DoScriptText(SAY_GUARD_RELIGHT_FIRE, m_creature);
                            relightingFires = true;
                            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                            switch (m_creature->GetEntry())
                            {
                                case NPC_DEATHGUARD_FLORENCE:
                                    m_creature->GetMotionMaster()->MovePoint(1, Position(2242.474f, 474.683f, 36.0789f, 3.351452f));
                                    break;
                                case NPC_RAZOR_HILL_GRUNT:
                                    m_creature->GetMotionMaster()->MovePoint(1, Position(277.6265f, -4563.144f, 24.1798f, 5.736432f));
                                    break;
                                case NPC_IRONFORGE_MOUNTAINEER:
                                    m_creature->GetMotionMaster()->MovePoint(1, Position(-5755.2036f, -504.95849f, 397.72644f, 4.769157f));
                                    break;
                                case NPC_STORMWIND_GUARD:
                                    m_creature->GetMotionMaster()->MovePoint(1, Position(-9331.7294f, 50.873886f, 60.544579f, 0.261875f));
                                    break;
                                default: // no movement required, already stationed close enough (should all have to move in TBC?)
                                    relightTimer = 5000;
                                    break;
                            }
                        }
                    }
                    else
                        checkExtinguishedTimer -= diff;
                }
            }
        }
        guardAI::UpdateAI(diff);
    }
};

bool GossipHello_guard_hallows_end_helper(Player* player, Creature* creature)
{
    GameEventCreatureData const* eventData = sGameEventMgr.GetCreatureUpdateDataForActiveEvent(creature->GetDbGuid());
    if (eventData)
    {
        uint32 textId = 0;
        switch (creature->GetEntry())
        {
            case NPC_RAZOR_HILL_GRUNT:
                player->SEND_GOSSIP_MENU(TEXT_ID_RAZOR_HILL_GRUNT, creature->GetObjectGuid());
                break;
            case NPC_DEATHGUARD_FLORENCE:
                player->SEND_GOSSIP_MENU(TEXT_ID_DEATHGUARD_FLORENCE, creature->GetObjectGuid());
                break;
            case NPC_STORMWIND_GUARD:
                player->SEND_GOSSIP_MENU(TEXT_ID_STORMWIND_GUARD, creature->GetObjectGuid());
                break;
            case NPC_IRONFORGE_MOUNTAINEER:
                player->SEND_GOSSIP_MENU(TEXT_ID_IRONFORGE_MOUNTAINEER, creature->GetObjectGuid());
                break;
            case NPC_SILVERMOON_GUARDIAN:
                player->SEND_GOSSIP_MENU(TEXT_ID_SILVERMOON_GUARDIAN, creature->GetObjectGuid());
                break;
            case NPC_AZUREMYST_PEACEKEEPER:
                player->SEND_GOSSIP_MENU(TEXT_ID_AZUREMYST_PEACEKEEPER, creature->GetObjectGuid());
                break;
        }
    }
    else
    {
        player->PrepareGossipMenu(creature, player->GetDefaultGossipMenuForSource(creature));
        player->SendPreparedGossip(creature);
    }
    return true;
}

/*******************************************************
 * guard_shattrath_aldor
 *******************************************************/

struct guard_shattrath_aldorAI : public guardAI
{
    guard_shattrath_aldorAI(Creature* pCreature) : guardAI(pCreature) { Reset(); }

    uint32 m_uiExile_Timer;
    uint32 m_uiBanish_Timer;
    ObjectGuid m_playerGuid;
    bool m_bCanTeleport;

    void Reset() override
    {
        m_uiBanish_Timer = 5000;
        m_uiExile_Timer = 8500;
        m_playerGuid.Clear();
        m_bCanTeleport = false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_bCanTeleport)
        {
            if (m_uiExile_Timer < uiDiff)
            {
                if (Player* pTarget = m_creature->GetMap()->GetPlayer(m_playerGuid))
                {
                    pTarget->CastSpell(pTarget, SPELL_EXILE, TRIGGERED_OLD_TRIGGERED);
                    pTarget->CastSpell(pTarget, SPELL_BANISH_TELEPORT, TRIGGERED_OLD_TRIGGERED);
                }

                m_playerGuid.Clear();
                m_uiExile_Timer = 8500;
                m_bCanTeleport = false;
            }
            else
                m_uiExile_Timer -= uiDiff;
        }
        else if (m_uiBanish_Timer < uiDiff)
        {
            Unit* pVictim = m_creature->GetVictim();

            if (pVictim && pVictim->GetTypeId() == TYPEID_PLAYER)
            {
                DoCastSpellIfCan(pVictim, SPELL_BANISHED_SHATTRATH_A);
                m_uiBanish_Timer = 9000;
                m_playerGuid = pVictim->GetObjectGuid();
                m_bCanTeleport = true;
            }
        }
        else
            m_uiBanish_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*******************************************************
 * guard_shattrath_scryer
 *******************************************************/

struct guard_shattrath_scryerAI : public guardAI
{
    guard_shattrath_scryerAI(Creature* pCreature) : guardAI(pCreature) { Reset(); }

    uint32 m_uiExile_Timer;
    uint32 m_uiBanish_Timer;
    ObjectGuid m_playerGuid;
    bool m_bCanTeleport;

    void Reset() override
    {
        m_uiBanish_Timer = 5000;
        m_uiExile_Timer = 8500;
        m_playerGuid.Clear();
        m_bCanTeleport = false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_bCanTeleport)
        {
            if (m_uiExile_Timer < uiDiff)
            {
                if (Player* pTarget = m_creature->GetMap()->GetPlayer(m_playerGuid))
                {
                    pTarget->CastSpell(pTarget, SPELL_EXILE, TRIGGERED_OLD_TRIGGERED);
                    pTarget->CastSpell(pTarget, SPELL_BANISH_TELEPORT, TRIGGERED_OLD_TRIGGERED);
                }

                m_playerGuid.Clear();
                m_uiExile_Timer = 8500;
                m_bCanTeleport = false;
            }
            else
                m_uiExile_Timer -= uiDiff;
        }
        else if (m_uiBanish_Timer < uiDiff)
        {
            Unit* pVictim = m_creature->GetVictim();

            if (pVictim && pVictim->GetTypeId() == TYPEID_PLAYER)
            {
                DoCastSpellIfCan(pVictim, SPELL_BANISHED_SHATTRATH_S);
                m_uiBanish_Timer = 9000;
                m_playerGuid = pVictim->GetObjectGuid();
                m_bCanTeleport = true;
            }
        }
        else
            m_uiBanish_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

enum
{
    SPELL_WINDSOR_INSPIRATION_EFFECT = 20275,
    MAX_GUARD_SALUTES                = 7,
};

static const int32 aGuardSalute[MAX_GUARD_SALUTES] = { -1000842, -1000843, -1000844, -1000845, -1000846, -1000847, -1000848};

struct guardAI_stormwind : public guardAI
{
    guardAI_stormwind(Creature* creature) : guardAI(creature)
    {
        Reset();
    }

    uint32 m_saluteWaitTimer;

    void Reset() override
    {
        m_saluteWaitTimer = 0;
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spell) override
    {
        if (spell->Id == SPELL_WINDSOR_INSPIRATION_EFFECT && !m_saluteWaitTimer)
        {
            DoScriptText(aGuardSalute[urand(0, MAX_GUARD_SALUTES - 1)], m_creature);
            m_saluteWaitTimer = 2 * MINUTE * IN_MILLISECONDS;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_saluteWaitTimer)
        {
            if (m_saluteWaitTimer < diff)
                m_saluteWaitTimer = 0;
            else
                m_saluteWaitTimer -= diff;
        }

        guardAI::UpdateAI(diff);
    }

    void ReceiveEmote(Player* player, uint32 textEmote) override
    {
        if (player->GetTeam() == ALLIANCE)
            DoReplyToTextEmote(textEmote);
    }
};

// 43783 - Spawn Guard
struct SpawnGuard : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        float pointX = 0;
        float pointY = 0;
        float pointZ = 0;

        uint32 guardEntry = 0;
        uint32 counter = 0;

        bool foundPosition = false;

        switch (caster->GetAreaId())
        {
            case 9:
            case 24: guardEntry = 1642;     break;      // Northshire
            case 42: guardEntry = 10038;    break;      // Darkshire
            case 69: guardEntry = 10037;    break;      // Lakeshire
            case 87: guardEntry = 1423;     break;      // Goldshire
            case 108: guardEntry = 8096;    break;      // Sentinel Hill
            case 131: guardEntry = 727;     break;      // Kharanos
            case 132: guardEntry = 853;     break;      // Coldridge Valley
            case 144: guardEntry = 8055;    break;      // Thelsamar
            case 152:                                   // The Bulwark
            case 154:                                   // Deathknell
            case 159: guardEntry = 7980;    break;      // Brill
            case 188: guardEntry = 4844;    break;      // Shadowglen
            case 186: guardEntry = 3571;    break;      // Dolanaar
            case 221:                                   // Camp Narache
            case 222:                                   // Bloodhoof Village
            {
                switch (rand() % 4)
                {
                    case 0: guardEntry = 3210; break;
                    case 1: guardEntry = 3211; break;
                    case 2: guardEntry = 3213; break;
                    case 3: guardEntry = 3214; break;
                }
                break;
            }
            case 228: guardEntry = 7489;    break;      // Sulpcher
            case 271: guardEntry = 2386;    break;      // Southshore
            case 272: guardEntry = 2405;    break;      // Tarren Mill
            case 320: guardEntry = 10696;   break;      // Refuge Pointe
            case 321: guardEntry = 2621;    break;      // Hammerfall
            case 340: guardEntry = 8155;    break;      // Kargath
            case 362: guardEntry = 5953;    break;      // Razor Hill
            case 363: guardEntry = 5952;    break;      // Valley of Trials
            case 367: guardEntry = 8017;    break;      // Sen'jin Village
            case 380: guardEntry = 3501;    break;      // Crossroads
            case 415: guardEntry = 6087;    break;      // Astranaar
            case 431: guardEntry = 12903;   break;      // Splintertree Post
            case 442: guardEntry = 6086;    break;      // Auberdine
            case 460: guardEntry = 7730;    break;      // Sun Rock Retreat
            case 484: guardEntry = 9525;    break;      // Freewind Post
            case 513: guardEntry = 4979;    break;      // Threamore
            case 597: guardEntry = 8154;    break;      // Ghost Walker Post
            case 608: guardEntry = 8151;    break;      // Nijel's Point
            case 1099: guardEntry = 8147;   break;      // Camp Mojache
            case 1116: guardEntry = 7939;   break;      // Feathermoon Stronghold
            case 1497: guardEntry = 5624;   break;      // Undercity
            case 1519: guardEntry = 68;     break;      // Stormwind City
            case 1537: guardEntry = 5595;   break;      // Ironforge
            case 1637: guardEntry = 3296;   break;      // Orgrimmar
            case 1638: guardEntry = 3084;   break;      // Thunder Bluff
            case 1657: guardEntry = 4262;   break;      // Darnassus
            case 2408: guardEntry = 12338;  break;      // Shadowprey Village
            case 2897: guardEntry = 12903;  break;      // Zoram'gar Outpost
            case 3462:                                  // Fairbreeze Village
            case 3487:                                  // Silvermoon City
            case 3488: guardEntry = 16222;  break;      // Tranquillien
            case 3527: guardEntry = 16921;  break;      // Crash Site
            case 3557: guardEntry = 16733;  break;      // Exodar
            case 3576: guardEntry = 18038;  break;      // Azure Watch
            case 3584: guardEntry = 17549;  break;      // Blood Watch
            case 3665: guardEntry = 16222;  break;      // Falconwing Square
        }

        caster->GetRandomPoint(caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), 30.0f, pointX, pointY, pointZ, 20.0f);


        while (counter < 100)
        {
            foundPosition = caster->GetMap()->GetReachableRandomPosition(caster, pointX, pointY, pointZ, 20.0f);

            if (foundPosition)
                break;

            counter++;
        }

        // Spawn Guards only if we have random position.
        if (foundPosition && guardEntry != 0)
        {
            Creature* guard = caster->SummonCreature(guardEntry, pointX, pointY, pointZ, caster->GetOrientation(), TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30 * IN_MILLISECONDS);

            if (guard)
                guard->AI()->AttackStart(caster->getAttackerForHelper());
        }
    }
};

/*******************************************************
 * quests 13188 / 13189
 *******************************************************/

enum
{
    SPELL_RETURN_ORGRIMMAR          = 58552,
    SPELL_RETURN_STORMWIND          = 58533,

    SPELL_TOSS_APPLE                = 58509,
    SPELL_TOSS_BANANA               = 58513,
    SPELL_SPIT                      = 58520,

    EMOTE_APPLE                     = -1609081,
    EMOTE_BANANA                    = -1609082,
    EMOTE_SPIT                      = -1609083,
    SAY_RANDOM_1                    = -1609084,
    SAY_RANDOM_2                    = -1609085,
    SAY_RANDOM_3                    = -1609086,
    SAY_RANDOM_4                    = -1609087,
    SAY_RANDOM_5                    = -1609088,
    SAY_RANDOM_6                    = -1609287,
    SAY_RANDOM_7                    = -1609288,
    SAY_RANDOM_8                    = -1609289,
};

bool EffectDummyCreature_npc_city_guard(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // check spell ids; creature ids are defined in script target
    if ((uiSpellId == SPELL_RETURN_ORGRIMMAR || uiSpellId == SPELL_RETURN_STORMWIND) && uiEffIndex == EFFECT_INDEX_0)
    {
        // random action
        switch (urand(0, 10))
        {
            case 0:
                pCreatureTarget->CastSpell(pCaster, SPELL_TOSS_APPLE, TRIGGERED_OLD_TRIGGERED);
                DoScriptText(EMOTE_APPLE, pCreatureTarget, pCaster);
                break;
            case 1:
                pCreatureTarget->CastSpell(pCaster, SPELL_TOSS_BANANA, TRIGGERED_OLD_TRIGGERED);
                DoScriptText(EMOTE_BANANA, pCreatureTarget, pCaster);
                break;
            case 2:
                pCreatureTarget->CastSpell(pCaster, SPELL_SPIT, TRIGGERED_OLD_TRIGGERED);
                DoScriptText(EMOTE_SPIT, pCreatureTarget, pCaster);
                break;
            case 3: DoScriptText(SAY_RANDOM_1, pCreatureTarget, pCaster); break;
            case 4: DoScriptText(SAY_RANDOM_2, pCreatureTarget, pCaster); break;
            case 5: DoScriptText(SAY_RANDOM_3, pCreatureTarget, pCaster); break;
            case 6: DoScriptText(SAY_RANDOM_4, pCreatureTarget, pCaster); break;
            case 7: DoScriptText(SAY_RANDOM_5, pCreatureTarget, pCaster); break;
            case 8: DoScriptText(SAY_RANDOM_6, pCreatureTarget, pCaster); break;
            case 9: DoScriptText(SAY_RANDOM_7, pCreatureTarget, pCaster); break;
            case 10: DoScriptText(SAY_RANDOM_8, pCreatureTarget, pCaster); break;
        }

        // return true as we don't need further script handling in DB
        return true;
    }

    return false;
}

void AddSC_guards()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "guard_azuremyst";
    pNewScript->GetAI = &GetNewAIInstance<guard_hallows_end_helperAI>;
    pNewScript->pGossipHello = &GossipHello_guard_hallows_end_helper;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_bluffwatcher";
    pNewScript->GetAI = &GetNewAIInstance<guardAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_contested";
    pNewScript->GetAI = &GetNewAIInstance<guardAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_darnassus";
    pNewScript->GetAI = &GetNewAIInstance<guardAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_dunmorogh";
    pNewScript->pGossipHello = &GossipHello_guard_hallows_end_helper;
    pNewScript->GetAI = &GetNewAIInstance<guard_hallows_end_helperAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_durotar";
    pNewScript->pGossipHello = &GossipHello_guard_hallows_end_helper;
    pNewScript->GetAI = &GetNewAIInstance<guard_hallows_end_helperAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_elwynnforest";
    pNewScript->pGossipHello = &GossipHello_guard_hallows_end_helper;
    pNewScript->GetAI = &GetNewAIInstance<guard_hallows_end_helperAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_eversong";
    pNewScript->pGossipHello = &GossipHello_guard_hallows_end_helper;
    pNewScript->GetAI = &GetNewAIInstance<guard_hallows_end_helperAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_exodar";
    pNewScript->GetAI = &GetNewAIInstance<guardAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_ironforge";
    pNewScript->GetAI = &GetNewAIInstance<guardAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_mulgore";
    pNewScript->GetAI = &GetNewAIInstance<guardAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_orgrimmar";
    pNewScript->GetAI = &GetNewAIInstance<guardAI_orgrimmar>;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_city_guard;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_shattrath";
    pNewScript->GetAI = &GetNewAIInstance<guardAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_shattrath_aldor";
    pNewScript->GetAI = &GetNewAIInstance<guard_shattrath_aldorAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_shattrath_scryer";
    pNewScript->GetAI = &GetNewAIInstance<guard_shattrath_scryerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_silvermoon";
    pNewScript->GetAI = &GetNewAIInstance<guardAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_stormwind";
    pNewScript->GetAI = &GetNewAIInstance<guardAI_stormwind>;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_city_guard;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_teldrassil";
    pNewScript->GetAI = &GetNewAIInstance<guardAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_tirisfal";
    pNewScript->GetAI = &GetNewAIInstance<guardAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_undercity";
    pNewScript->GetAI = &GetNewAIInstance<guardAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "guard_hallows_end_helper";
    pNewScript->GetAI = &GetNewAIInstance<guard_hallows_end_helperAI>;
    pNewScript->pGossipHello = &GossipHello_guard_hallows_end_helper;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SpawnGuard>("spell_spawn_guard");
}

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
SDName: boss_deathbringer_saurfang
SD%Complete: 100%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_AGGRO                   = -1631028,
    SAY_FALLENCHAMPION          = -1631029,
    SAY_BLOODBEASTS             = -1631030,
    SAY_SLAY_1                  = -1631031,
    SAY_SLAY_2                  = -1631032,
    SAY_BERSERK                 = -1631033,
    SAY_DEATH                   = -1631034,
    EMOTE_FRENZY                = -1631193,
    EMOTE_SCENT                 = -1631194,

    // intro event related
    SPELL_GRIP_OF_AGONY         = 70572,
    SPELL_VEHICLE_HARDCODED     = 46598,

    // aggro spells
    SPELL_BLOOD_LINK            = 72178,
    SPELL_MARK_FALLEN_DAMAGE    = 72256,            // procs 72255 on Saurfang melee attack
    SPELL_RUNE_OF_BLOOD_PROC    = 72408,            // procs 72409 on Saurfang melee attack

    // combat spells
    SPELL_BLOOD_POWER           = 72371,            // triggered by 72195
    // SPELL_BLOOD_POWER_SCALE   = 72370,           // purpose unk
    // SPELL_ZERO_POWER          = 72242,           // included in creature_template_addon

    SPELL_MARK_FALLEN_CHAMPION  = 72254,            // triggers 72293 which procs 72260 on target death
    SPELL_RUNE_OF_BLOOD         = 72410,
    SPELL_BLOOD_NOVA            = 72378,
    SPELL_BOILING_BLOOD         = 72385,

    SPELL_CALL_BLOOD_BEAST_1    = 72172,            // summons 38508
    SPELL_CALL_BLOOD_BEAST_2    = 72173,
    SPELL_CALL_BLOOD_BEAST_3    = 72356,
    SPELL_CALL_BLOOD_BEAST_4    = 72357,
    SPELL_CALL_BLOOD_BEAST_5    = 72358,
    SPELL_SCENT_OF_BLOOD        = 72769,            // triggers 72771 on the blood beasts

    SPELL_BERSERK               = 26662,
    SPELL_FRENZY                = 72737,

    // evade / death spells
    SPELL_REMOVE_MARKS          = 72257,
    SPELL_ACHIEVEMENT           = 72928,
    SPELL_FEIGN_DEATH           = 70628,

    // Summoned spells
    SPELL_RESISTANT_SKIN        = 72723,
    SPELL_BLOOD_LINK_BEAST      = 72176,

    NPC_BLOOD_BEAST             = 38508,

    FACTION_ID_UNDEAD           = 974,
    FACTION_ID_FRIENDLY         = 35,

    // Deathbringer's Rise Alliance introduction.  These use a separate text
    // range from the existing combat text above.
    SAY_INTRO_ALLIANCE_1         = -1631300,
    SAY_INTRO_ALLIANCE_2         = -1631301,
    SAY_INTRO_ALLIANCE_3         = -1631302,
    SAY_INTRO_ALLIANCE_4         = -1631303,
    SAY_INTRO_ALLIANCE_5         = -1631304,
    SAY_INTRO_ALLIANCE_6         = -1631305,
    SAY_INTRO_ALLIANCE_7         = -1631306,

    // CMaNGOS DB relay scripts which drive the faction-specific post-kill
    // cinematics.  The faction leaders use ScriptDevAI for the repaired
    // introduction, so their former EventAI relay action must be forwarded
    // here explicitly.
    RELAY_SAURFANG_OUTRO_ALLIANCE = 20519,
    RELAY_SAURFANG_OUTRO_HORDE    = 20520,
};

enum SaurfangIntroActions
{
    SAURFANG_INTRO_BOSS_REPLY_1 = 1,
    SAURFANG_INTRO_BOSS_REPLY_2,
    SAURFANG_INTRO_MURADIN_CHALLENGE,
    SAURFANG_INTRO_MURADIN_CHARGE,
    SAURFANG_INTRO_GRIP,
    SAURFANG_INTRO_FINAL_WORD,
    SAURFANG_INTRO_BEGIN_COMBAT,
};

float const aSaurfangIntroFirstStep[3] = { -541.318f, 2211.365f, 539.292f };
float const aSaurfangIntroCharge[3] = { -509.651f, 2211.377f, 539.287f };
float const aSaurfangCombatPosition[3] = { -496.354f, 2211.330f, 541.114f };

enum DeathbringerActions
{
    DEATHBRINGER_BERSERK,
    DEATHBRINGER_FRENZY,
    DEATHBRINGER_RUNE_OF_BLOOD,
    DEATHBRINGER_BOILING_BLOOD,
    DEATHBRINGER_BLOOD_NOVA,
    DEATHBRINGER_BLOOD_BEAST,
    DEATHBRINGER_SCENT_OF_BLOOD,
    DEATHBRINGER_MARK_FALLEN_CHAMPION,
    DEATHBRINGER_ACTION_MAX,
    DEATHBRINGER_DEATH
};

struct boss_deathbringer_saurfangAI : public CombatAI
{
    boss_deathbringer_saurfangAI(Creature* creature) : CombatAI(creature, DEATHBRINGER_ACTION_MAX), m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData()))
    {
        if (m_instance)
        {
            m_bIsHeroicMode = m_instance->IsHeroicDifficulty();
            m_bIs25ManMode = m_instance->Is25ManDifficulty();
        }

        // common actions
        AddCombatAction(DEATHBRINGER_BERSERK, uint32((m_bIsHeroicMode ? 6 : 8) * MINUTE * IN_MILLISECONDS));
        AddCombatAction(DEATHBRINGER_RUNE_OF_BLOOD, 20000u);
        AddCombatAction(DEATHBRINGER_BOILING_BLOOD, 15500u);
        AddCombatAction(DEATHBRINGER_BLOOD_NOVA, 17000u);
        AddCombatAction(DEATHBRINGER_BLOOD_BEAST, 30000u);

        if (m_bIsHeroicMode)
            AddCombatAction(DEATHBRINGER_SCENT_OF_BLOOD, 47000u);

        AddTimerlessCombatAction(DEATHBRINGER_FRENZY, true);
        AddTimerlessCombatAction(DEATHBRINGER_MARK_FALLEN_CHAMPION, true);

        AddCustomAction(DEATHBRINGER_DEATH, true, [&]() { HandleDeath(); });

        m_bIsFakingDeath = false;
        Reset();
    }

    instance_icecrown_citadel* m_instance;

    uint8 m_uiAchievSpellCount;

    bool m_bIs25ManMode;
    bool m_bIsHeroicMode;
    bool m_bIsFakingDeath;
    GuidList m_bloodBeastGuids;

    void CleanupBloodBeasts()
    {
        for (GuidList::const_iterator itr = m_bloodBeastGuids.begin(); itr != m_bloodBeastGuids.end(); ++itr)
            if (Creature* beast = m_creature->GetMap()->GetCreature(*itr))
                beast->ForcedDespawn();
        m_bloodBeastGuids.clear();
    }

    void Reset() override
    {
        CombatAI::Reset();

        m_uiAchievSpellCount = 0;
        m_creature->SetPower(m_creature->GetPowerType(), 0);
        CleanupBloodBeasts();
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        DoCastSpellIfCan(m_creature, SPELL_BLOOD_LINK, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_MARK_FALLEN_DAMAGE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_RUNE_OF_BLOOD_PROC, CAST_TRIGGERED);

        if (m_instance)
            m_instance->SetData(TYPE_DEATHBRINGER_SAURFANG, IN_PROGRESS);
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        if (urand(0, 1))
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void EnterEvadeMode() override
    {
        if (m_bIsFakingDeath)
            return;

        CombatAI::EnterEvadeMode();
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* spellInfo) override
    {
        if (m_bIsFakingDeath)
        {
            uiDamage = 0;
            return;
        }

        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;
            m_bIsFakingDeath = true;

            // yell death and change flags
            DoScriptText(SAY_DEATH, m_creature);

            ClearCombatOnlyRoot();
            m_creature->RemoveAllAurasOnEvade();
            m_creature->CombatStopWithPets(true);

            DoCastSpellIfCan(m_creature, SPELL_REMOVE_MARKS, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_ACHIEVEMENT, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_FEIGN_DEATH, CAST_TRIGGERED);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->SetImmuneToPlayer(true);

            if (m_instance)
                m_instance->SetData(TYPE_DEATHBRINGER_SAURFANG, DONE);

            // Blood Beasts are encounter summons and must not survive the
            // boss completion or retain combat on the raid afterward.
            CleanupBloodBeasts();

            // The intro holds the faction leader aloft with Grip of Agony.
            // The spell aura does not naturally expire, so explicitly remove
            // it when the encounter completes and let its aura handler return
            // the leader to the ground.
            if (Creature* leader = GetClosestCreatureWithEntry(m_creature, NPC_MURADIN_BRONZEBEARD, 100.0f))
                leader->RemoveAurasDueToSpell(SPELL_GRIP_OF_AGONY);
            else if (Creature* leader = GetClosestCreatureWithEntry(m_creature, NPC_OVERLORD_SAURFANG, 100.0f))
                leader->RemoveAurasDueToSpell(SPELL_GRIP_OF_AGONY);

            // change faction and start outro
            ResetTimer(DEATHBRINGER_DEATH, 5 * IN_MILLISECONDS);

            // stop combat and movement
            SetCombatScriptStatus(true);
            m_creature->SetTarget(nullptr);
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveIdle();
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_DEATHBRINGER_SAURFANG, FAIL);

        DoCastSpellIfCan(m_creature, SPELL_REMOVE_MARKS, CAST_TRIGGERED);
        CleanupBloodBeasts();
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_BLOOD_BEAST)
            m_bloodBeastGuids.push_back(summoned->GetObjectGuid());

        summoned->CastSpell(summoned, SPELL_RESISTANT_SKIN, TRIGGERED_OLD_TRIGGERED);
        summoned->CastSpell(summoned, SPELL_BLOOD_LINK_BEAST, TRIGGERED_OLD_TRIGGERED);

        // Note: the summoned should be activated only after 2-3 seconds after summon - can be done in eventAI
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
            summoned->AI()->AttackStart(pTarget);
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_BLOOD_BEAST)
            m_bloodBeastGuids.remove(summoned->GetObjectGuid());
    }

    void HandleDeath()
    {
        // Inform the faction leader and every suspended escort to start the
        // outro.  The stock 30-yard radius did not reach the guards spread
        // across Deathbringer's Rise, leaving them levitating indefinitely.
        SendAIEventAround(AI_EVENT_CUSTOM_EVENTAI_C, m_creature, 0, 150.0f);

        m_creature->SetFactionTemporary(FACTION_ID_FRIENDLY, TEMPFACTION_NONE);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case DEATHBRINGER_BERSERK:
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    DisableCombatAction(action);
                }
                break;
            case DEATHBRINGER_FRENZY:
                if (m_creature->GetHealthPercent() <= 30.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
                    {
                        DoScriptText(EMOTE_FRENZY, m_creature);
                        DisableCombatAction(action);
                    }
                }
                break;
            case DEATHBRINGER_RUNE_OF_BLOOD:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_RUNE_OF_BLOOD) == CAST_OK)
                    ResetCombatAction(action, urand(20000, 25000));
                break;
            case DEATHBRINGER_BOILING_BLOOD:
                if (DoCastSpellIfCan(m_creature, SPELL_BOILING_BLOOD) == CAST_OK)
                    ResetCombatAction(action, urand(15000, 20000));
                break;
            case DEATHBRINGER_BLOOD_NOVA:
                if (DoCastSpellIfCan(m_creature, SPELL_BLOOD_NOVA) == CAST_OK)
                    ResetCombatAction(action, urand(20000, 25000));
                break;
            case DEATHBRINGER_BLOOD_BEAST:
                DoScriptText(SAY_BLOODBEASTS, m_creature);

                DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_1, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_2, CAST_TRIGGERED);

                if (m_bIs25ManMode)
                {
                    DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_3, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_4, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_CALL_BLOOD_BEAST_5, CAST_TRIGGERED);
                }

                ResetCombatAction(action, 40000);

                if (m_bIsHeroicMode)
                    ResetCombatAction(DEATHBRINGER_SCENT_OF_BLOOD, 7000);
                break;
            case DEATHBRINGER_SCENT_OF_BLOOD:
                if (DoCastSpellIfCan(m_creature, SPELL_SCENT_OF_BLOOD) == CAST_OK)
                {
                    DoScriptText(EMOTE_SCENT, m_creature);
                    ResetCombatAction(action, 40000);
                }
                break;
            case DEATHBRINGER_MARK_FALLEN_CHAMPION:
                if (m_creature->GetPower(m_creature->GetPowerType()) == 100)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_MARK_FALLEN_CHAMPION) == CAST_OK)
                    {
                        DoScriptText(SAY_FALLENCHAMPION, m_creature);
                        m_creature->RemoveAurasDueToSpell(SPELL_BLOOD_POWER);
                        m_creature->SetPower(m_creature->GetPowerType(), 0);

                        // check for achievement fail
                        ++m_uiAchievSpellCount;

                        if (m_instance)
                        {
                            if (m_uiAchievSpellCount == (m_bIs25ManMode ? 5 : 3))
                                m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_MADE_A_MESS, false);
                        }
                    }
                }
                break;
        }
    }
};

// The faction leader at Deathbringer's Rise is the retail start control for
// this encounter.  CMaNGOS already spawned the leader and guards after
// Gunship, but had no script binding for the gossip or the introduction.
struct npc_saurfang_intro_leaderAI : public ScriptedAI
{
    npc_saurfang_intro_leaderAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData()))
    {
        AddCustomAction(SAURFANG_INTRO_BOSS_REPLY_1, true, [&]() { SayByEntry(NPC_DEATHBRINGER_SAURFANG, SAY_INTRO_ALLIANCE_2); });
        AddCustomAction(SAURFANG_INTRO_BOSS_REPLY_2, true, [&]() { SayByEntry(NPC_DEATHBRINGER_SAURFANG, SAY_INTRO_ALLIANCE_3); });
        AddCustomAction(SAURFANG_INTRO_MURADIN_CHALLENGE, true, [&]()
        {
            DoScriptText(SAY_INTRO_ALLIANCE_4, m_creature);
            m_creature->SetWalk(true);
            m_creature->GetMotionMaster()->MovePoint(1, aSaurfangIntroFirstStep[0], aSaurfangIntroFirstStep[1], aSaurfangIntroFirstStep[2]);
        });
        AddCustomAction(SAURFANG_INTRO_MURADIN_CHARGE, true, [&]()
        {
            DoScriptText(SAY_INTRO_ALLIANCE_5, m_creature);
            m_creature->SetWalk(false);
            m_creature->GetMotionMaster()->MovePoint(2, aSaurfangIntroCharge[0], aSaurfangIntroCharge[1], aSaurfangIntroCharge[2]);
        });
        AddCustomAction(SAURFANG_INTRO_GRIP, true, [&]()
        {
            if (Creature* boss = GetBoss())
            {
                DoScriptText(SAY_INTRO_ALLIANCE_7, boss);
                boss->CastSpell(m_creature, SPELL_GRIP_OF_AGONY, TRIGGERED_OLD_TRIGGERED);
            }
        });
        AddCustomAction(SAURFANG_INTRO_FINAL_WORD, true, [&]() { SayByEntry(NPC_DEATHBRINGER_SAURFANG, SAY_INTRO_ALLIANCE_6); });
        AddCustomAction(SAURFANG_INTRO_BEGIN_COMBAT, true, [&]()
        {
            if (Creature* boss = GetBoss())
            {
                // The intro's boss spawn begins neutral and immune.  The
                // final scene handoff must change both its reaction flags and
                // faction before Aggro; otherwise it can attack a player
                // while still displaying as a friendly green unit.
                boss->SetFactionTemporary(FACTION_ID_UNDEAD, TEMPFACTION_NONE);
                boss->SetImmuneToPlayer(false);
                boss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                if (Player* player = m_instance->GetPlayerInMap(true))
                    boss->AI()->AttackStart(player);
            }
        });
    }

    instance_icecrown_citadel* m_instance;
    bool m_started = false;

    void Reset() override
    {
        m_started = false;
        SetReactState(REACT_PASSIVE);
        // Both faction leaders are scene actors, including High Overlord
        // Saurfang while he retrieves his son during the Alliance outro.
        // Keep them selectable for the pre-fight gossip, but never valid
        // player combat targets.
        m_creature->SetImmuneToPlayer(true);

        if (m_instance && m_instance->GetData(TYPE_DEATHBRINGER_SAURFANG) == DONE)
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        else
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType != AI_EVENT_CUSTOM_EVENTAI_C || !sender || sender->GetEntry() != NPC_DEATHBRINGER_SAURFANG)
            return;

        // This is the ScriptDevAI equivalent of the stock EventAI actions
        // 3718704 / 3720004.  Removing Grip lands the leader; the relay then
        // performs the complete Alliance or Horde body-recovery cinematic,
        // including guard cleanup and the replacement post-event party.
        m_creature->RemoveAurasDueToSpell(SPELL_GRIP_OF_AGONY);

        uint32 relayId = m_creature->GetEntry() == NPC_MURADIN_BRONZEBEARD
            ? RELAY_SAURFANG_OUTRO_ALLIANCE
            : RELAY_SAURFANG_OUTRO_HORDE;
        m_creature->GetMap()->ScriptsStart(SCRIPT_TYPE_RELAY, relayId, m_creature, m_creature);
    }

    Creature* GetBoss() const
    {
        return GetClosestCreatureWithEntry(m_creature, NPC_DEATHBRINGER_SAURFANG, 120.0f);
    }

    void SayByEntry(uint32 entry, int32 text)
    {
        if (Creature* creature = GetClosestCreatureWithEntry(m_creature, entry, 120.0f))
            DoScriptText(text, creature);
    }

    void StartEvent()
    {
        if (m_started || !m_instance || m_instance->GetData(TYPE_DEATHBRINGER_SAURFANG) == DONE)
            return;

        Creature* boss = GetBoss();
        if (!boss)
            return;

        m_started = true;
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_instance->DoUseOpenableObject(GO_SAURFANG_DOOR, true);

        // The boss moves down from the throne while the faction leader begins
        // the scene, exactly as in the original encounter.
        boss->SetImmuneToPlayer(true);
        boss->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        boss->SetWalk(false);
        boss->GetMotionMaster()->MovePoint(3, aSaurfangCombatPosition[0], aSaurfangCombatPosition[1], aSaurfangCombatPosition[2]);

        DoScriptText(SAY_INTRO_ALLIANCE_1, m_creature);
        ResetTimer(SAURFANG_INTRO_BOSS_REPLY_1, 2500);
        ResetTimer(SAURFANG_INTRO_BOSS_REPLY_2, 20000);
        ResetTimer(SAURFANG_INTRO_MURADIN_CHALLENGE, 29500);
        ResetTimer(SAURFANG_INTRO_MURADIN_CHARGE, 34500);
        ResetTimer(SAURFANG_INTRO_GRIP, 36500);
        ResetTimer(SAURFANG_INTRO_FINAL_WORD, 38500);
        ResetTimer(SAURFANG_INTRO_BEGIN_COMBAT, 43000);
    }
};

bool GossipHello_npc_saurfang_intro_leader(Player* player, Creature* creature)
{
    if (instance_icecrown_citadel* instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData()))
    {
        if (instance->GetData(TYPE_DEATHBRINGER_SAURFANG) != DONE && instance->GetData(TYPE_DEATHBRINGER_SAURFANG) != IN_PROGRESS)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "We are ready. Begin the assault.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
            return true;
        }
    }

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_saurfang_intro_leader(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
{
    player->CLOSE_GOSSIP_MENU();
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
        static_cast<npc_saurfang_intro_leaderAI*>(creature->AI())->StartEvent();
    return true;
}

/*######
## spell_grip_of_agony_aura - 70572
######*/

struct spell_grip_of_agony_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        target->SetLevitate(apply);
        target->GetMotionMaster()->Clear(false, true);

        // on apply move up with 10
        if (apply)
            target->GetMotionMaster()->MovePoint(0, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ() + 10);
        // on remove fall to the ground
        else
            target->GetMotionMaster()->MoveFall();
    }
};

/*######
## spell_mark_fallen_champion - 72254
######*/

struct spell_mark_fallen_champion : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster || !target->IsPlayer())
            return;

        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        if (!target->HasAura(spellId))
            caster->CastSpell(target, spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_blood_link - 72202
######*/

struct spell_blood_link : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->CastSpell(target, 72195, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_deathbringer_saurfang()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_deathbringer_saurfang";
    pNewScript->GetAI = &GetNewAIInstance<boss_deathbringer_saurfangAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_saurfang_intro_leader";
    pNewScript->GetAI = &GetNewAIInstance<npc_saurfang_intro_leaderAI>;
    pNewScript->pGossipHello = &GossipHello_npc_saurfang_intro_leader;
    pNewScript->pGossipSelect = &GossipSelect_npc_saurfang_intro_leader;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_grip_of_agony_aura>("spell_grip_of_agony_aura");
    RegisterSpellScript<spell_mark_fallen_champion>("spell_mark_fallen_champion");
    RegisterSpellScript<spell_blood_link>("spell_blood_link");
}

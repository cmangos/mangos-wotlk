/*
   Name    : PlayerbotWarriorAI.cpp
   Complete: maybe around 37%
   Author  : Natsukawa
   Version : 0.39
 */
#include "PlayerbotWarriorAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;
PlayerbotWarriorAI::PlayerbotWarriorAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    BATTLE_STANCE           = m_ai->initSpell(BATTLE_STANCE_1); //ARMS
    CHARGE                  = m_ai->initSpell(CHARGE_1); //ARMS
    OVERPOWER               = m_ai->initSpell(OVERPOWER_1); // ARMS
    HEROIC_STRIKE           = m_ai->initSpell(HEROIC_STRIKE_1); //ARMS
    REND                    = m_ai->initSpell(REND_1); //ARMS
    THUNDER_CLAP            = m_ai->initSpell(THUNDER_CLAP_1);  //ARMS
    HAMSTRING               = m_ai->initSpell(HAMSTRING_1);  //ARMS
    MOCKING_BLOW            = m_ai->initSpell(MOCKING_BLOW_1);  //ARMS
    RETALIATION             = m_ai->initSpell(RETALIATION_1);  //ARMS
    SWEEPING_STRIKES        = m_ai->initSpell(SWEEPING_STRIKES_1); //ARMS
    MORTAL_STRIKE           = m_ai->initSpell(MORTAL_STRIKE_1);  //ARMS
    BLADESTORM              = m_ai->initSpell(BLADESTORM_1);  //ARMS
    HEROIC_THROW            = m_ai->initSpell(HEROIC_THROW_1);  //ARMS
    SHATTERING_THROW        = m_ai->initSpell(SHATTERING_THROW_1);  //ARMS
    BLOODRAGE               = m_ai->initSpell(BLOODRAGE_1); //PROTECTION
    DEFENSIVE_STANCE        = m_ai->initSpell(DEFENSIVE_STANCE_1); //PROTECTION
    DEVASTATE               = m_ai->initSpell(DEVASTATE_1); //PROTECTION
    SUNDER_ARMOR            = m_ai->initSpell(SUNDER_ARMOR_1); //PROTECTION
    TAUNT                   = m_ai->initSpell(TAUNT_1); //PROTECTION
    SHIELD_BASH             = m_ai->initSpell(SHIELD_BASH_1); //PROTECTION
    REVENGE                 = m_ai->initSpell(REVENGE_1); //PROTECTION
    SHIELD_BLOCK            = m_ai->initSpell(SHIELD_BLOCK_1); //PROTECTION
    DISARM                  = m_ai->initSpell(DISARM_1); //PROTECTION
    SHIELD_WALL             = m_ai->initSpell(SHIELD_WALL_1); //PROTECTION
    SHIELD_SLAM             = m_ai->initSpell(SHIELD_SLAM_1); //PROTECTION
    VIGILANCE               = m_ai->initSpell(VIGILANCE_1); //PROTECTION
    DEVASTATE               = m_ai->initSpell(DEVASTATE_1); //PROTECTION
    SHOCKWAVE               = m_ai->initSpell(SHOCKWAVE_1); //PROTECTION
    CONCUSSION_BLOW         = m_ai->initSpell(CONCUSSION_BLOW_1); //PROTECTION
    SPELL_REFLECTION        = m_ai->initSpell(SPELL_REFLECTION_1); //PROTECTION
    LAST_STAND              = m_ai->initSpell(LAST_STAND_1); //PROTECTION
    BATTLE_SHOUT            = m_ai->initSpell(BATTLE_SHOUT_1); //FURY
    DEMORALIZING_SHOUT      = m_ai->initSpell(DEMORALIZING_SHOUT_1); //FURY
    CLEAVE                  = m_ai->initSpell(CLEAVE_1); //FURY
    INTIMIDATING_SHOUT      = m_ai->initSpell(INTIMIDATING_SHOUT_1); //FURY
    EXECUTE                 = m_ai->initSpell(EXECUTE_1); //FURY
    CHALLENGING_SHOUT       = m_ai->initSpell(CHALLENGING_SHOUT_1); //FURY
    SLAM                    = m_ai->initSpell(SLAM_1); //FURY
    BERSERKER_STANCE        = m_ai->initSpell(BERSERKER_STANCE_1); //FURY
    INTERCEPT               = m_ai->initSpell(INTERCEPT_1); //FURY
    DEATH_WISH              = m_ai->initSpell(DEATH_WISH_1); //FURY
    BERSERKER_RAGE          = m_ai->initSpell(BERSERKER_RAGE_1); //FURY
    WHIRLWIND               = m_ai->initSpell(WHIRLWIND_1); //FURY
    PUMMEL                  = m_ai->initSpell(PUMMEL_1); //FURY
    BLOODTHIRST             = m_ai->initSpell(BLOODTHIRST_1); //FURY
    RECKLESSNESS            = m_ai->initSpell(RECKLESSNESS_1); //FURY
    RAMPAGE                 = 0; // passive
    HEROIC_FURY             = m_ai->initSpell(HEROIC_FURY_1); //FURY
    COMMANDING_SHOUT        = m_ai->initSpell(COMMANDING_SHOUT_1); //FURY
    ENRAGED_REGENERATION    = m_ai->initSpell(ENRAGED_REGENERATION_1); //FURY
    PIERCING_HOWL           = m_ai->initSpell(PIERCING_HOWL_1); //FURY

    RECENTLY_BANDAGED       = 11196; // first aid check

    // racial
    GIFT_OF_THE_NAARU       = m_ai->initSpell(GIFT_OF_THE_NAARU_WARRIOR); // draenei
    STONEFORM               = m_ai->initSpell(STONEFORM_ALL); // dwarf
    ESCAPE_ARTIST           = m_ai->initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF   = m_ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    SHADOWMELD              = m_ai->initSpell(SHADOWMELD_ALL); // night elf
    BLOOD_FURY              = m_ai->initSpell(BLOOD_FURY_MELEE_CLASSES); // orc
    WAR_STOMP               = m_ai->initSpell(WAR_STOMP_ALL); // tauren
    BERSERKING              = m_ai->initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN    = m_ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead

    //Procs
    SLAM_PROC               = m_ai->initSpell(SLAM_PROC_1);
    BLOODSURGE              = m_ai->initSpell(BLOODSURGE_1);
    TASTE_FOR_BLOOD         = m_ai->initSpell(TASTE_FOR_BLOOD_1);
    SUDDEN_DEATH            = m_ai->initSpell(SUDDEN_DEATH_1);
}
PlayerbotWarriorAI::~PlayerbotWarriorAI() {}

CombatManeuverReturns PlayerbotWarriorAI::DoFirstCombatManeuver(Unit *pTarget)
{
    if (!m_ai)  return RETURN_NO_ACTION_ERROR;
    if (!m_bot) return RETURN_NO_ACTION_ERROR;

    PlayerbotAI::CombatOrderType co = m_ai->GetCombatOrder();
    float fTargetDist = m_bot->GetCombatDistance(pTarget);

    if (DEFENSIVE_STANCE && (co & PlayerbotAI::ORDERS_TANK))
    {
        if (!m_bot->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0) && m_ai->CastSpell(DEFENSIVE_STANCE))
            return RETURN_CONTINUE;
        else if (TAUNT > 0 && m_bot->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0) && m_ai->CastSpell(TAUNT, *pTarget))
            return RETURN_FINISHED_FIRST_MOVES;
    }

    if (BERSERKER_STANCE)
    {
        if (!m_bot->HasAura(BERSERKER_STANCE, EFFECT_INDEX_0) && m_ai->CastSpell(BERSERKER_STANCE))
            return RETURN_CONTINUE;
        if (BLOODRAGE > 0 && m_bot->HasAura(BERSERKER_STANCE, EFFECT_INDEX_0) && m_ai->GetRageAmount() <= 10)
            return m_ai->CastSpell(BLOODRAGE) ? RETURN_FINISHED_FIRST_MOVES : RETURN_NO_ACTION_ERROR;
        if (INTERCEPT > 0 && m_bot->HasAura(BERSERKER_STANCE, EFFECT_INDEX_0))
        {
            if (fTargetDist < 8.0f)
                return RETURN_NO_ACTION_OK;
            else if (fTargetDist > 25.0f)
                return RETURN_CONTINUE; // wait to come into range
            else if (INTERCEPT > 0 && m_ai->CastSpell(INTERCEPT, *pTarget))
            {
                float x, y, z;
                pTarget->GetContactPoint(m_bot, x, y, z, 3.666666f);
                m_bot->Relocate(x, y, z);
                return RETURN_FINISHED_FIRST_MOVES;
            }
        }
    }

    if (BATTLE_STANCE)
    {
        if (!m_bot->HasAura(BATTLE_STANCE, EFFECT_INDEX_0) && m_ai->CastSpell(BATTLE_STANCE))
            return RETURN_CONTINUE;
        if (CHARGE > 0 && m_bot->HasAura(BATTLE_STANCE, EFFECT_INDEX_0))
        {
            if (fTargetDist < 8.0f)
                return RETURN_NO_ACTION_OK;
            if (fTargetDist > 25.0f)
                return RETURN_CONTINUE; // wait to come into range
            else if (CHARGE > 0 && m_ai->CastSpell(CHARGE, *pTarget))
            {
                float x, y, z;
                pTarget->GetContactPoint(m_bot, x, y, z, 3.666666f);
                m_bot->Relocate(x, y, z);
                return RETURN_FINISHED_FIRST_MOVES;
            }
        }
    }

    return RETURN_NO_ACTION_OK;
}

//Buff and rebuff shouts
void PlayerbotWarriorAI::CheckShouts()
{
    if (!m_ai)  return;
    if (!m_bot) return;

    uint32 spec = m_bot->GetSpec();

    if ((spec == WARRIOR_SPEC_ARMS || spec == WARRIOR_SPEC_FURY) && BATTLE_SHOUT > 0 && m_ai->GetRageAmount() >= 10 && !m_bot->HasAura(BATTLE_SHOUT, EFFECT_INDEX_0))
        m_ai->CastSpell(BATTLE_SHOUT);
    else if (spec == WARRIOR_SPEC_PROTECTION && COMMANDING_SHOUT > 0 && m_ai->GetRageAmount() >= 10 && !m_bot->HasAura(COMMANDING_SHOUT, EFFECT_INDEX_0))
        m_ai->CastSpell(COMMANDING_SHOUT);
    //Buff battle shout if bot doesn't have commanding yet
    else if (spec == WARRIOR_SPEC_PROTECTION && COMMANDING_SHOUT == 0 && BATTLE_SHOUT > 0 && m_ai->GetRageAmount() >= 10 && !m_bot->HasAura(BATTLE_SHOUT, EFFECT_INDEX_0))
        m_ai->CastSpell(BATTLE_SHOUT);
}

CombatManeuverReturns PlayerbotWarriorAI::DoNextCombatManeuver(Unit *pTarget)
{
    if (!m_ai)  return RETURN_NO_ACTION_ERROR;
    if (!m_bot) return RETURN_NO_ACTION_ERROR;

    //switch (m_ai->GetScenarioType())
    //{
    //    case PlayerbotAI::SCENARIO_DUEL:
    //        if (HEROIC_STRIKE > 0)
    //            m_ai->CastSpell(HEROIC_STRIKE);
    //        return;
    //    default:
    //        break;
    //}
    // ------- Non Duel combat ----------

    // Damage Attacks

    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();
    float fTargetDist = m_bot->GetCombatDistance(pTarget);
    PlayerbotAI::CombatOrderType co = m_ai->GetCombatOrder();
    uint32 spec = m_bot->GetSpec();

    //If we have devastate it will replace SA in our rotation
    //uint32 SUNDER = (DEVASTATE > 0 ? DEVASTATE : SUNDER_ARMOR);

    //Used to determine if this bot is highest on threat
    Unit *newTarget = m_ai->FindAttacker((PlayerbotAI::ATTACKERINFOTYPE) (PlayerbotAI::AIT_VICTIMSELF | PlayerbotAI::AIT_HIGHESTTHREAT), m_bot);

    // do shouts, berserker rage, etc...
    if (BERSERKER_RAGE > 0 && !m_bot->HasAura(BERSERKER_RAGE, EFFECT_INDEX_0))
        m_ai->CastSpell(BERSERKER_RAGE);
    else if (BLOODRAGE > 0 && m_ai->GetRageAmount() <= 10)
        m_ai->CastSpell(BLOODRAGE);

    CheckShouts();
    switch (spec)
    {
        case WARRIOR_SPEC_ARMS:
            if (EXECUTE > 0 && (pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.20) && m_ai->GetRageAmount() >= 15)
            {
                m_ai->CastSpell (EXECUTE, *pTarget);
                return RETURN_CONTINUE;
            }
            //Haven't found a way to make sudden death work yet, either wrong spell or it needs an effect index(probably)
            else if (EXECUTE > 0 && m_bot->HasAura(SUDDEN_DEATH))
            {
                m_ai->CastSpell (EXECUTE, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (REND > 0 && m_ai->GetRageAmount() >= 10 && !pTarget->HasAura(REND, EFFECT_INDEX_0))
            {
                m_ai->CastSpell(REND, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (MORTAL_STRIKE > 0 && m_ai->GetRageAmount() >= 30 && !m_bot->HasSpellCooldown(MORTAL_STRIKE))
            {
                m_ai->CastSpell(MORTAL_STRIKE, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (SHATTERING_THROW > 0 && !pTarget->HasAura(SHATTERING_THROW, EFFECT_INDEX_0) && !m_bot->HasSpellCooldown(SHATTERING_THROW))
            {
                m_ai->CastSpell(SHATTERING_THROW, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (BLADESTORM > 0 && m_ai->GetRageAmount() >= 25 && !m_bot->HasSpellCooldown(BLADESTORM) /*&& m_ai->GetAttackerCount() >= 3*/)
            {
                m_ai->CastSpell(BLADESTORM, *pTarget);
                return RETURN_CONTINUE;
            }
            // No way to tell if overpower is active (yet), however taste for blood works
            else if (OVERPOWER > 0 && m_ai->GetRageAmount() >= 5 && m_bot->HasAura(TASTE_FOR_BLOOD))
            {
                m_ai->CastSpell(OVERPOWER, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (HEROIC_STRIKE > 0 && m_ai->GetRageAmount() >= 60)
            {
                m_ai->CastSpell(HEROIC_STRIKE, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (SLAM > 0 && m_ai->GetRageAmount() >= 15)
            {
                m_ai->CastSpell(SLAM, *pTarget);
                m_ai->SetIgnoreUpdateTime(1.5); // TODO: SetIgnoreUpdateTime takes a uin8 - how will 1.5 work as a value?
                return RETURN_CONTINUE;
            }

        case WARRIOR_SPEC_FURY:
            if (EXECUTE > 0 && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.20 && m_ai->GetRageAmount() >= 15)
            {
                m_ai->CastSpell (EXECUTE, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (BLOODTHIRST > 0 && m_ai->GetRageAmount() >= 20 && !m_bot->HasSpellCooldown(BLOODTHIRST))
            {
                m_ai->CastSpell(BLOODTHIRST, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (WHIRLWIND > 0 && m_ai->GetRageAmount() >= 25 && !m_bot->HasSpellCooldown(WHIRLWIND))
            {
                m_ai->CastSpell(WHIRLWIND, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (SLAM > 0 && m_ai->GetRageAmount() >= 15 && m_bot->HasAura(BLOODSURGE, EFFECT_INDEX_0))
            {
                m_ai->CastSpell(SLAM, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (HEROIC_STRIKE > 0 && m_ai->GetRageAmount() >= 60)
            {
                m_ai->CastSpell(HEROIC_STRIKE, *pTarget);
                return RETURN_CONTINUE;
            }

        case WARRIOR_SPEC_PROTECTION:
            if (m_ai->GetCombatOrder() == PlayerbotAI::ORDERS_TANK && !newTarget && TAUNT > 0 && !m_bot->HasSpellCooldown(TAUNT))
            {
                m_ai->CastSpell(TAUNT, *pTarget);
                return RETURN_CONTINUE;
            }
            // No way to tell if revenge is active (yet)
            /*else if (REVENGE > 0 && m_ai->GetRageAmount() >= 5)
               {
                m_ai->CastSpell(REVENGE, *pTarget);
                return RETURN_CONTINUE;
               }*/
            else if (REND > 0 && m_ai->GetRageAmount() >= 10 && !pTarget->HasAura(REND, EFFECT_INDEX_0))
            {
                m_ai->CastSpell(REND, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (THUNDER_CLAP > 0 && m_ai->GetRageAmount() >= 20 && !pTarget->HasAura(THUNDER_CLAP))
            {
                m_ai->CastSpell(THUNDER_CLAP, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (DEMORALIZING_SHOUT > 0 && m_ai->GetRageAmount() >= 10 && !pTarget->HasAura(DEMORALIZING_SHOUT, EFFECT_INDEX_0))
            {
                m_ai->CastSpell(DEMORALIZING_SHOUT, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (CONCUSSION_BLOW > 0 && m_ai->GetRageAmount() >= 15 && !m_bot->HasSpellCooldown(CONCUSSION_BLOW))
            {
                m_ai->CastSpell(CONCUSSION_BLOW, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (SHOCKWAVE > 0 && m_ai->GetRageAmount() >= 15  && !m_bot->HasSpellCooldown(SHOCKWAVE))
            {
                m_ai->CastSpell(SHOCKWAVE, *pTarget);
                return RETURN_CONTINUE;
            }
            else if (SHIELD_SLAM > 0 && m_ai->GetRageAmount() >= 20 && !m_bot->HasSpellCooldown(SHIELD_SLAM))
            {
                m_ai->CastSpell(SHIELD_SLAM, *pTarget);
                return RETURN_CONTINUE;
            }
            //else if (SUNDER > 0 && m_ai->GetRageAmount() >= 15 && !pTarget->HasAura(SUNDER_ARMOR))
            //{
            //	m_ai->CastSpell(SUNDER, *pTarget);
            //    return RETURN_CONTINUE;
            //}
            //Devastate seems to be broken in current build
            //else if (DEVASTATE > 0 && m_ai->GetRageAmount() >= 15)
            //{
            //	 m_ai->CastSpell(DEVASTATE, *pTarget);
            //    return RETURN_CONTINUE;
            //}
            else if (HEROIC_STRIKE > 0 && m_ai->GetRageAmount() >= 60)
            {
                m_ai->CastSpell(HEROIC_STRIKE, *pTarget);
                return RETURN_CONTINUE;
            }


            /*case WarriorSpellPreventing:
                out << "Case Prevent";
                if (SHIELD_BASH > 0 && m_ai->GetRageAmount() >= 10 && m_ai->CastSpell(SHIELD_BASH, *pTarget))
                    out << " > Shield Bash";
                else if (PUMMEL > 0 && m_ai->GetRageAmount() >= 10 && m_ai->CastSpell(PUMMEL, *pTarget))
                    out << " > Pummel";
                else if (SPELL_REFLECTION > 0 && m_ai->GetRageAmount() >= 15 && !m_bot->HasAura(SPELL_REFLECTION, EFFECT_INDEX_0) && m_ai->CastSpell(SPELL_REFLECTION, *m_bot))
                    out << " > Spell Reflection";
                else
                    out << " > NONE";
                break;

               case WarriorBattle:
                out << "Case Battle";
                else if (LAST_STAND > 0 && !m_bot->HasAura(LAST_STAND, EFFECT_INDEX_0) && m_bot->GetHealth() < m_bot->GetMaxHealth() * 0.5 && m_ai->CastSpell(LAST_STAND, *m_bot))
                    out << " > Last Stand!";
                else if (DEATH_WISH > 0 && m_ai->GetRageAmount() >= 10 && !m_bot->HasAura(DEATH_WISH, EFFECT_INDEX_0) && m_ai->CastSpell(DEATH_WISH, *m_bot))
                    out << " > Death Wish";
                else if (RETALIATION > 0 && pVictim == m_bot && m_ai->GetAttackerCount() >= 2 && !m_bot->HasAura(RETALIATION, EFFECT_INDEX_0) && m_ai->CastSpell(RETALIATION, *m_bot))
                    out << " > Retaliation";
                else if (SWEEPING_STRIKES > 0 && m_ai->GetRageAmount() >= 30 && m_ai->GetAttackerCount() >= 2 && !m_bot->HasAura(SWEEPING_STRIKES, EFFECT_INDEX_0) && m_ai->CastSpell(SWEEPING_STRIKES, *m_bot))
                    out << " > Sweeping Strikes!";
                else if (INTIMIDATING_SHOUT > 0 && m_ai->GetRageAmount() >= 25 && m_ai->GetAttackerCount() > 5 && m_ai->CastSpell(INTIMIDATING_SHOUT, *pTarget))
                    out << " > Intimidating Shout";
                else if (ENRAGED_REGENERATION > 0 && m_ai->GetRageAmount() >= 15 && !m_bot->HasAura(BERSERKER_RAGE, EFFECT_INDEX_0) && !m_bot->HasAura(ENRAGED_REGENERATION, EFFECT_INDEX_0) && m_bot->GetHealth() < m_bot->GetMaxHealth() * 0.5 && m_ai->CastSpell(ENRAGED_REGENERATION, *m_bot))
                    out << " > Enraged Regeneration";
                else if (HAMSTRING > 0 && m_ai->GetRageAmount() >= 10 && !pTarget->HasAura(HAMSTRING, EFFECT_INDEX_0) && m_ai->CastSpell(HAMSTRING, *pTarget))
                    out << " > Hamstring";
                else if (CHALLENGING_SHOUT > 0 && m_ai->GetRageAmount() >= 5 && pVictim != m_bot && m_ai->GetHealthPercent() > 25 && !pTarget->HasAura(MOCKING_BLOW, EFFECT_INDEX_0) && !pTarget->HasAura(CHALLENGING_SHOUT, EFFECT_INDEX_0) && m_ai->CastSpell(CHALLENGING_SHOUT, *pTarget))
                    out << " > Challenging Shout";
                else if (CLEAVE > 0 && m_ai->GetRageAmount() >= 20 && m_ai->CastSpell(CLEAVE, *pTarget))
                    out << " > Cleave";

                else if (PIERCING_HOWL > 0 && m_ai->GetRageAmount() >= 10 && m_ai->GetAttackerCount() >= 3 && !pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0) && !pTarget->HasAura(PIERCING_HOWL, EFFECT_INDEX_0) && !pTarget->HasAura(SHOCKWAVE, EFFECT_INDEX_0) && !pTarget->HasAura(CONCUSSION_BLOW, EFFECT_INDEX_0) && m_ai->CastSpell(PIERCING_HOWL, *pTarget))
                    out << " > Piercing Howl";
                else if (MOCKING_BLOW > 0 && m_ai->GetRageAmount() >= 10 && pVictim != m_bot && m_ai->GetHealthPercent() > 25 && !pTarget->HasAura(MOCKING_BLOW, EFFECT_INDEX_0) && !pTarget->HasAura(CHALLENGING_SHOUT, EFFECT_INDEX_0) && m_ai->CastSpell(MOCKING_BLOW, *pTarget))
                    out << " > Mocking Blow";
                else if (HEROIC_THROW > 0 && m_ai->CastSpell(HEROIC_THROW, *pTarget))
                    out << " > Heroic Throw";
                else if (m_bot->getRace() == RACE_TAUREN && !pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0) && !pTarget->HasAura(PIERCING_HOWL, EFFECT_INDEX_0) && !pTarget->HasAura(SHOCKWAVE, EFFECT_INDEX_0) && !pTarget->HasAura(CONCUSSION_BLOW, EFFECT_INDEX_0) && m_ai->CastSpell(WAR_STOMP, *pTarget))
                    out << " > War Stomp";
                else if (m_bot->getRace() == RACE_HUMAN && m_bot->hasUnitState(UNIT_STAT_STUNNED) || m_bot->HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) || m_bot->HasAuraType(SPELL_AURA_MOD_CHARM) && m_ai->CastSpell(EVERY_MAN_FOR_HIMSELF, *m_bot))
                    out << " > Every Man for Himself";
                else if (m_bot->getRace() == RACE_UNDEAD && m_bot->HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot->HasAuraType(SPELL_AURA_MOD_CHARM) && m_ai->CastSpell(WILL_OF_THE_FORSAKEN, *m_bot))
                    out << " > Will of the Forsaken";
                else if (m_bot->getRace() == RACE_DWARF && m_bot->HasAuraState(AURA_STATE_DEADLY_POISON) && m_ai->CastSpell(STONEFORM, *m_bot))
                    out << " > Stoneform";
                else if (m_bot->getRace() == RACE_GNOME && m_bot->hasUnitState(UNIT_STAT_STUNNED) || m_bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) && m_ai->CastSpell(ESCAPE_ARTIST, *m_bot))
                    out << " > Escape Artist";
                else if (m_bot->getRace() == RACE_NIGHTELF && pVictim == m_bot && m_ai->GetHealthPercent() < 25 && !m_bot->HasAura(SHADOWMELD, EFFECT_INDEX_0) && m_ai->CastSpell(SHADOWMELD, *m_bot))
                    out << " > Shadowmeld";
                else if (m_bot->getRace() == RACE_ORC && !m_bot->HasAura(BLOOD_FURY, EFFECT_INDEX_0) && m_ai->CastSpell(BLOOD_FURY, *m_bot))
                    out << " > Blood Fury";
                else if (m_bot->getRace() == RACE_TROLL && !m_bot->HasAura(BERSERKING, EFFECT_INDEX_0) && m_ai->CastSpell(BERSERKING, *m_bot))
                    out << " > Berserking";
                else if (m_bot->getRace() == RACE_DRAENEI && m_ai->GetHealthPercent() < 25 && !m_bot->HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0) && m_ai->CastSpell(GIFT_OF_THE_NAARU, *m_bot))
                    out << " > Gift of the Naaru";
                else
                    out << " > NONE";
                break;

               case WarriorDefensive:
                out << "Case Defensive";
                if (DISARM > 0 && m_ai->GetRageAmount() >= 15 && !pTarget->HasAura(DISARM, EFFECT_INDEX_0) && m_ai->CastSpell(DISARM, *pTarget))
                    out << " > Disarm";
                else if (SHIELD_BLOCK > 0 && !m_bot->HasAura(SHIELD_BLOCK, EFFECT_INDEX_0) && m_ai->CastSpell(SHIELD_BLOCK, *m_bot))
                    out << " > Shield Block";
                else if (SHIELD_WALL > 0 && !m_bot->HasAura(SHIELD_WALL, EFFECT_INDEX_0) && m_ai->CastSpell(SHIELD_WALL, *m_bot))
                    out << " > Shield Wall";
                else
                    out << " > NONE";
                break;*/

    }

    return RETURN_NO_ACTION_UNKNOWN;
}

void PlayerbotWarriorAI::DoNonCombatActions()
{
    if (!m_ai)  return;
    if (!m_bot) return;

    uint32 spec = m_bot->GetSpec();
    if (!m_bot)
        return;

    //Stance Check
    if (spec == WARRIOR_SPEC_ARMS && !m_bot->HasAura(BATTLE_STANCE, EFFECT_INDEX_0))
        m_ai->CastSpell(BATTLE_STANCE);
    else if (spec == WARRIOR_SPEC_FURY && !m_bot->HasAura(BERSERKER_STANCE, EFFECT_INDEX_0))
        m_ai->CastSpell(BERSERKER_STANCE);
    else if (spec == WARRIOR_SPEC_PROTECTION && !m_bot->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0))
        m_ai->CastSpell(DEFENSIVE_STANCE);

    // buff master with VIGILANCE
    if (VIGILANCE > 0)
        (!GetMaster()->HasAura(VIGILANCE, EFFECT_INDEX_0) && m_ai->CastSpell(VIGILANCE, *GetMaster()));

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = m_ai->FindFood();
    Item* fItem = m_ai->FindBandage();

    if (pItem != NULL && m_ai->GetHealthPercent() < 30)
    {
        m_ai->TellMaster("I could use some food.");
        m_ai->UseItem(pItem);
        return;
    }
    else if (pItem == NULL && fItem != NULL && !m_bot->HasAura(RECENTLY_BANDAGED, EFFECT_INDEX_0) && m_ai->GetHealthPercent() < 70)
    {
        m_ai->TellMaster("I could use first aid.");
        m_ai->UseItem(fItem);
        return;
    }
    else if (pItem == NULL && fItem == NULL && m_bot->getRace() == RACE_DRAENEI && !m_bot->HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0) && m_ai->GetHealthPercent() < 70)
    {
        m_ai->TellMaster("I'm casting gift of the naaru.");
        m_ai->CastSpell(GIFT_OF_THE_NAARU, *m_bot);
        return;
    }
} // end DoNonCombatActions

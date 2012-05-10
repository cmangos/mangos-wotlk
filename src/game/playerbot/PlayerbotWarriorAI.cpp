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
    BATTLE_STANCE           = ai->initSpell(BATTLE_STANCE_1); //ARMS
    CHARGE                  = ai->initSpell(CHARGE_1); //ARMS
    OVERPOWER               = ai->initSpell(OVERPOWER_1); // ARMS
    HEROIC_STRIKE           = ai->initSpell(HEROIC_STRIKE_1); //ARMS
    REND                    = ai->initSpell(REND_1); //ARMS
    THUNDER_CLAP            = ai->initSpell(THUNDER_CLAP_1);  //ARMS
    HAMSTRING               = ai->initSpell(HAMSTRING_1);  //ARMS
    MOCKING_BLOW            = ai->initSpell(MOCKING_BLOW_1);  //ARMS
    RETALIATION             = ai->initSpell(RETALIATION_1);  //ARMS
    SWEEPING_STRIKES        = ai->initSpell(SWEEPING_STRIKES_1); //ARMS
    MORTAL_STRIKE           = ai->initSpell(MORTAL_STRIKE_1);  //ARMS
    BLADESTORM              = ai->initSpell(BLADESTORM_1);  //ARMS
    HEROIC_THROW            = ai->initSpell(HEROIC_THROW_1);  //ARMS
    SHATTERING_THROW        = ai->initSpell(SHATTERING_THROW_1);  //ARMS
    BLOODRAGE               = ai->initSpell(BLOODRAGE_1); //PROTECTION
    DEFENSIVE_STANCE        = ai->initSpell(DEFENSIVE_STANCE_1); //PROTECTION
    DEVASTATE               = ai->initSpell(DEVASTATE_1); //PROTECTION
    SUNDER_ARMOR            = ai->initSpell(SUNDER_ARMOR_1); //PROTECTION
    TAUNT                   = ai->initSpell(TAUNT_1); //PROTECTION
    SHIELD_BASH             = ai->initSpell(SHIELD_BASH_1); //PROTECTION
    REVENGE                 = ai->initSpell(REVENGE_1); //PROTECTION
    SHIELD_BLOCK            = ai->initSpell(SHIELD_BLOCK_1); //PROTECTION
    DISARM                  = ai->initSpell(DISARM_1); //PROTECTION
    SHIELD_WALL             = ai->initSpell(SHIELD_WALL_1); //PROTECTION
    SHIELD_SLAM             = ai->initSpell(SHIELD_SLAM_1); //PROTECTION
    VIGILANCE               = ai->initSpell(VIGILANCE_1); //PROTECTION
    DEVASTATE               = ai->initSpell(DEVASTATE_1); //PROTECTION
    SHOCKWAVE               = ai->initSpell(SHOCKWAVE_1); //PROTECTION
    CONCUSSION_BLOW         = ai->initSpell(CONCUSSION_BLOW_1); //PROTECTION
    SPELL_REFLECTION        = ai->initSpell(SPELL_REFLECTION_1); //PROTECTION
    LAST_STAND              = ai->initSpell(LAST_STAND_1); //PROTECTION
    BATTLE_SHOUT            = ai->initSpell(BATTLE_SHOUT_1); //FURY
    DEMORALIZING_SHOUT      = ai->initSpell(DEMORALIZING_SHOUT_1); //FURY
    CLEAVE                  = ai->initSpell(CLEAVE_1); //FURY
    INTIMIDATING_SHOUT      = ai->initSpell(INTIMIDATING_SHOUT_1); //FURY
    EXECUTE                 = ai->initSpell(EXECUTE_1); //FURY
    CHALLENGING_SHOUT       = ai->initSpell(CHALLENGING_SHOUT_1); //FURY
    SLAM                    = ai->initSpell(SLAM_1); //FURY
    BERSERKER_STANCE        = ai->initSpell(BERSERKER_STANCE_1); //FURY
    INTERCEPT               = ai->initSpell(INTERCEPT_1); //FURY
    DEATH_WISH              = ai->initSpell(DEATH_WISH_1); //FURY
    BERSERKER_RAGE          = ai->initSpell(BERSERKER_RAGE_1); //FURY
    WHIRLWIND               = ai->initSpell(WHIRLWIND_1); //FURY
    PUMMEL                  = ai->initSpell(PUMMEL_1); //FURY
    BLOODTHIRST             = ai->initSpell(BLOODTHIRST_1); //FURY
    RECKLESSNESS            = ai->initSpell(RECKLESSNESS_1); //FURY
    RAMPAGE                 = 0; // passive
    HEROIC_FURY             = ai->initSpell(HEROIC_FURY_1); //FURY
    COMMANDING_SHOUT        = ai->initSpell(COMMANDING_SHOUT_1); //FURY
    ENRAGED_REGENERATION    = ai->initSpell(ENRAGED_REGENERATION_1); //FURY
    PIERCING_HOWL           = ai->initSpell(PIERCING_HOWL_1); //FURY

    RECENTLY_BANDAGED       = 11196; // first aid check

    // racial
    GIFT_OF_THE_NAARU       = ai->initSpell(GIFT_OF_THE_NAARU_WARRIOR); // draenei
    STONEFORM               = ai->initSpell(STONEFORM_ALL); // dwarf
    ESCAPE_ARTIST           = ai->initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF   = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    SHADOWMELD              = ai->initSpell(SHADOWMELD_ALL); // night elf
    BLOOD_FURY              = ai->initSpell(BLOOD_FURY_MELEE_CLASSES); // orc
    WAR_STOMP               = ai->initSpell(WAR_STOMP_ALL); // tauren
    BERSERKING              = ai->initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN    = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead

	//Procs
	SLAM_PROC				= ai->initSpell(SLAM_PROC_1);
	BLOODSURGE				= ai->initSpell(BLOODSURGE_1);
	TASTE_FOR_BLOOD			= ai->initSpell(TASTE_FOR_BLOOD_1);
	SUDDEN_DEATH			= ai->initSpell(SUDDEN_DEATH_1);
}
PlayerbotWarriorAI::~PlayerbotWarriorAI() {}

bool PlayerbotWarriorAI::DoFirstCombatManeuver(Unit *pTarget)
{
    Player *m_bot = GetPlayerBot();
    PlayerbotAI *ai = GetAI();
    PlayerbotAI::CombatOrderType co = ai->GetCombatOrder();
    float fTargetDist = m_bot->GetCombatDistance(pTarget);

    /*if ((co & PlayerbotAI::ORDERS_TANK) && DEFENSIVE_STANCE > 0 && !m_bot->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0) && ai->CastSpell(DEFENSIVE_STANCE))
    {
        return true;
    }
    else if ((co & PlayerbotAI::ORDERS_TANK) && TAUNT > 0 && m_bot->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0) && ai->CastSpell(TAUNT, *pTarget))
    {
        return false;
    }
	else if (BERSERKER_STANCE > 0 && !m_bot->HasAura(BERSERKER_STANCE, EFFECT_INDEX_0) && ai->CastSpell(BERSERKER_STANCE))
	{
		return true;
	}
	else if (BLOODRAGE > 0 && m_bot->HasAura(BERSERKER_STANCE, EFFECT_INDEX_0) && ai->GetRageAmount() <= 10)
	{
		ai->CastSpell(BLOODRAGE);
		return false;
	}
	else if (BERSERKER_STANCE > 0 && INTERCEPT > 0 && m_bot->HasAura(BERSERKER_STANCE, EFFECT_INDEX_0))
	{
		if (fTargetDist < 8.0f)
            return false;
        else if (fTargetDist > 25.0f)
            return true;
        else if (INTERCEPT > 0 && ai->CastSpell(INTERCEPT, *pTarget))
        {
            float x, y, z;
            pTarget->GetContactPoint(m_bot, x, y, z, 3.666666f);
            m_bot->Relocate(x, y, z);
            return false;
        }
    }
    else if (BATTLE_STANCE > 0 && !m_bot->HasAura(BATTLE_STANCE, EFFECT_INDEX_0) && ai->CastSpell(BATTLE_STANCE))
    {
        return true;
    }
    else if (BATTLE_STANCE > 0 && CHARGE > 0 && m_bot->HasAura(BATTLE_STANCE, EFFECT_INDEX_0))
    {
        if (fTargetDist < 8.0f)
            return false;
        else if (fTargetDist > 25.0f)
            return true;
        else if (CHARGE > 0 && ai->CastSpell(CHARGE, *pTarget))
        {
            float x, y, z;
            pTarget->GetContactPoint(m_bot, x, y, z, 3.666666f);
            m_bot->Relocate(x, y, z);
            return false;
        }
    }*/

    return false;
}

//Buff and rebuff shouts
void PlayerbotWarriorAI::CheckShouts()
{
	PlayerbotAI* ai = GetAI();
	Player *m_bot = GetPlayerBot();
	uint32 spec = m_bot->GetSpec();

	if ((spec == WARRIOR_SPEC_ARMS || spec == WARRIOR_SPEC_FURY) && BATTLE_SHOUT > 0 && ai->GetRageAmount() >= 10 && !m_bot->HasAura(BATTLE_SHOUT, EFFECT_INDEX_0))
		ai->CastSpell(BATTLE_SHOUT);
	else if (spec == WARRIOR_SPEC_PROTECTION && COMMANDING_SHOUT > 0 && ai->GetRageAmount() >= 10 && !m_bot->HasAura(COMMANDING_SHOUT, EFFECT_INDEX_0))
		ai->CastSpell(COMMANDING_SHOUT);
	//Buff battle shout if bot doesn't have commanding yet
	else if (spec == WARRIOR_SPEC_PROTECTION && COMMANDING_SHOUT == 0 && BATTLE_SHOUT > 0 && ai->GetRageAmount() >= 10 && !m_bot->HasAura(BATTLE_SHOUT, EFFECT_INDEX_0))
		ai->CastSpell(BATTLE_SHOUT);
}

void PlayerbotWarriorAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    //switch (ai->GetScenarioType())
    //{
    //    case PlayerbotAI::SCENARIO_DUEL:
    //        if (HEROIC_STRIKE > 0)
    //            ai->CastSpell(HEROIC_STRIKE);
    //        return;
    //}
    // ------- Non Duel combat ----------

    // Damage Attacks

    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();
    float fTargetDist = m_bot->GetCombatDistance(pTarget);
    PlayerbotAI::CombatOrderType co = ai->GetCombatOrder();
	uint32 spec = m_bot->GetSpec();

	//If we have devastate it will replace SA in our rotation
	uint32 SUNDER = (DEVASTATE > 0 ? DEVASTATE : SUNDER_ARMOR);

	//Used to determine if this bot is highest on threat
	Unit *newTarget = ai->FindAttacker((PlayerbotAI::ATTACKERINFOTYPE) (PlayerbotAI::AIT_VICTIMSELF | PlayerbotAI::AIT_HIGHESTTHREAT), m_bot);

    // do shouts, berserker rage, etc...
    if (BERSERKER_RAGE > 0 && !m_bot->HasAura(BERSERKER_RAGE, EFFECT_INDEX_0))
    {
		 ai->CastSpell(BERSERKER_RAGE);
    }
	else if (BLOODRAGE > 0 && ai->GetRageAmount() <= 10)
	{
		ai->CastSpell(BLOODRAGE);
	}

	CheckShouts();
    switch (spec)
    {
		case WARRIOR_SPEC_ARMS:
			
			if (EXECUTE > 0 && (pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.20) && ai->GetRageAmount() >= 15)
            {
                ai->CastSpell (EXECUTE, *pTarget);
                break;
            }
			//Haven't found a way to make sudden death work yet, either wrong spell or it needs an effect index(probably)
			else if (EXECUTE > 0 && m_bot->HasAura(SUDDEN_DEATH))
			{
				ai->CastSpell (EXECUTE, *pTarget);
                break;
            }
			else if (REND > 0 && ai->GetRageAmount() >= 10 && !pTarget->HasAura(REND, EFFECT_INDEX_0))
			{
				 ai->CastSpell(REND, *pTarget);
				 break;
			}
			else if (MORTAL_STRIKE > 0 && ai->GetRageAmount() >= 30 && !m_bot->HasSpellCooldown(MORTAL_STRIKE))
			{
				 ai->CastSpell(MORTAL_STRIKE, *pTarget);
				 break;
			}
			else if (SHATTERING_THROW > 0 && !pTarget->HasAura(SHATTERING_THROW, EFFECT_INDEX_0) && !m_bot->HasSpellCooldown(SHATTERING_THROW))
			{
				 ai->CastSpell(SHATTERING_THROW, *pTarget);
				 break;
			}
			else if (BLADESTORM > 0 && ai->GetRageAmount() >= 25 && !m_bot->HasSpellCooldown(BLADESTORM)/*&& ai->GetAttackerCount() >= 3*/)
			{
				ai->CastSpell(BLADESTORM, *pTarget);
				break;
			}
			//No way to tell if overpoweris active(yet), however taste for blood works 
			else if (OVERPOWER > 0 && ai->GetRageAmount() >= 5 && m_bot->HasAura(TASTE_FOR_BLOOD))
			{
				 ai->CastSpell(OVERPOWER, *pTarget);
				 break;
			}
			else if (HEROIC_STRIKE > 0 && ai->GetRageAmount() >= 60)
			{
				 ai->CastSpell(HEROIC_STRIKE, *pTarget);
				 break;
			}
			else if (SLAM > 0 && ai->GetRageAmount() >= 15)
			{
				 ai->CastSpell(SLAM, *pTarget);
				 ai->SetIgnoreUpdateTime(1.5);
				 break;
			}

		case WARRIOR_SPEC_FURY:

			if (EXECUTE > 0 && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.20 && ai->GetRageAmount() >= 15)
            {
                ai->CastSpell (EXECUTE, *pTarget);
                break;
            }
			else if (BLOODTHIRST > 0 && ai->GetRageAmount() >= 20 && !m_bot->HasSpellCooldown(BLOODTHIRST))
			{
				ai->CastSpell(BLOODTHIRST, *pTarget);
				break;
			}
			else if (WHIRLWIND > 0 && ai->GetRageAmount() >= 25 && !m_bot->HasSpellCooldown(WHIRLWIND))
			{
				ai->CastSpell(WHIRLWIND, *pTarget);
				break;
			}
			else if (SLAM > 0 && ai->GetRageAmount() >= 15 && m_bot->HasAura(BLOODSURGE, EFFECT_INDEX_0))
			{
				ai->CastSpell(SLAM, *pTarget);
				break;
			}
			else if (HEROIC_STRIKE > 0 && ai->GetRageAmount() >= 60)
			{
				 ai->CastSpell(HEROIC_STRIKE, *pTarget);
				 break;
			}

		case WARRIOR_SPEC_PROTECTION:

			if (ai->GetCombatOrder() == PlayerbotAI::ORDERS_TANK && !newTarget && TAUNT > 0 && !m_bot->HasSpellCooldown(TAUNT))
			{
				ai->CastSpell(TAUNT, *pTarget);
				break;
			}
			//No way to tell if revenge is active(yet)
			/*else if (REVENGE > 0 && ai->GetRageAmount() >= 5)
			{
				ai->CastSpell(REVENGE, *pTarget);
				break;
			}*/
			else if (REND > 0 && ai->GetRageAmount() >= 10 && !pTarget->HasAura(REND, EFFECT_INDEX_0))
			{
				ai->CastSpell(REND, *pTarget);
				break;
			}
			else if (THUNDER_CLAP > 0 && ai->GetRageAmount() >= 20 && !pTarget->HasAura(THUNDER_CLAP))
			{
				 ai->CastSpell(THUNDER_CLAP, *pTarget);
					 break;
			}
			else if (DEMORALIZING_SHOUT > 0 && ai->GetRageAmount() >= 10 && !pTarget->HasAura(DEMORALIZING_SHOUT, EFFECT_INDEX_0))
			{
				 ai->CastSpell(DEMORALIZING_SHOUT, *pTarget);
				 break;
			}
			else if (CONCUSSION_BLOW > 0 && ai->GetRageAmount() >= 15 && !m_bot->HasSpellCooldown(CONCUSSION_BLOW))
			{
				 ai->CastSpell(CONCUSSION_BLOW, *pTarget);
				 break;
			}
			else if (SHOCKWAVE > 0 && ai->GetRageAmount() >= 15  && !m_bot->HasSpellCooldown(SHOCKWAVE))
			{
				 ai->CastSpell(SHOCKWAVE, *pTarget);
				 break;
			}
			else if (SHIELD_SLAM > 0 && ai->GetRageAmount() >= 20 && !m_bot->HasSpellCooldown(SHIELD_SLAM))
			{
				 ai->CastSpell(SHIELD_SLAM, *pTarget);
				 break;
			}
			//else if (SUNDER > 0 && ai->GetRageAmount() >= 15 && !pTarget->HasAura(SUNDER_ARMOR))
			//{
			//	ai->CastSpell(SUNDER, *pTarget);
			//	break;
			//}
			//Devastate seems to be broken in current build
			//else if (DEVASTATE > 0 && ai->GetRageAmount() >= 15)
			//{
			//	 ai->CastSpell(DEVASTATE, *pTarget);
			//	 break;
			//}
			else if (HEROIC_STRIKE > 0 && ai->GetRageAmount() >= 60)
			{
				ai->CastSpell(HEROIC_STRIKE, *pTarget);
				break;
			}


        /*case WarriorSpellPreventing:
            out << "Case Prevent";
            if (SHIELD_BASH > 0 && ai->GetRageAmount() >= 10 && ai->CastSpell(SHIELD_BASH, *pTarget))
                out << " > Shield Bash";
            else if (PUMMEL > 0 && ai->GetRageAmount() >= 10 && ai->CastSpell(PUMMEL, *pTarget))
                out << " > Pummel";
            else if (SPELL_REFLECTION > 0 && ai->GetRageAmount() >= 15 && !m_bot->HasAura(SPELL_REFLECTION, EFFECT_INDEX_0) && ai->CastSpell(SPELL_REFLECTION, *m_bot))
                out << " > Spell Reflection";
            else
                out << " > NONE";
            break;

        case WarriorBattle:
            out << "Case Battle";
            else if (LAST_STAND > 0 && !m_bot->HasAura(LAST_STAND, EFFECT_INDEX_0) && m_bot->GetHealth() < m_bot->GetMaxHealth() * 0.5 && ai->CastSpell(LAST_STAND, *m_bot))
                out << " > Last Stand!";
            else if (DEATH_WISH > 0 && ai->GetRageAmount() >= 10 && !m_bot->HasAura(DEATH_WISH, EFFECT_INDEX_0) && ai->CastSpell(DEATH_WISH, *m_bot))
                out << " > Death Wish";
            else if (RETALIATION > 0 && pVictim == m_bot && ai->GetAttackerCount() >= 2 && !m_bot->HasAura(RETALIATION, EFFECT_INDEX_0) && ai->CastSpell(RETALIATION, *m_bot))
                out << " > Retaliation";
            else if (SWEEPING_STRIKES > 0 && ai->GetRageAmount() >= 30 && ai->GetAttackerCount() >= 2 && !m_bot->HasAura(SWEEPING_STRIKES, EFFECT_INDEX_0) && ai->CastSpell(SWEEPING_STRIKES, *m_bot))
                out << " > Sweeping Strikes!";
            else if (INTIMIDATING_SHOUT > 0 && ai->GetRageAmount() >= 25 && ai->GetAttackerCount() > 5 && ai->CastSpell(INTIMIDATING_SHOUT, *pTarget))
                out << " > Intimidating Shout";
            else if (ENRAGED_REGENERATION > 0 && ai->GetRageAmount() >= 15 && !m_bot->HasAura(BERSERKER_RAGE, EFFECT_INDEX_0) && !m_bot->HasAura(ENRAGED_REGENERATION, EFFECT_INDEX_0) && m_bot->GetHealth() < m_bot->GetMaxHealth() * 0.5 && ai->CastSpell(ENRAGED_REGENERATION, *m_bot))
                out << " > Enraged Regeneration";
            else if (HAMSTRING > 0 && ai->GetRageAmount() >= 10 && !pTarget->HasAura(HAMSTRING, EFFECT_INDEX_0) && ai->CastSpell(HAMSTRING, *pTarget))
                out << " > Hamstring";
            else if (CHALLENGING_SHOUT > 0 && ai->GetRageAmount() >= 5 && pVictim != m_bot && ai->GetHealthPercent() > 25 && !pTarget->HasAura(MOCKING_BLOW, EFFECT_INDEX_0) && !pTarget->HasAura(CHALLENGING_SHOUT, EFFECT_INDEX_0) && ai->CastSpell(CHALLENGING_SHOUT, *pTarget))
                out << " > Challenging Shout";
            else if (CLEAVE > 0 && ai->GetRageAmount() >= 20 && ai->CastSpell(CLEAVE, *pTarget))
                out << " > Cleave";

            else if (PIERCING_HOWL > 0 && ai->GetRageAmount() >= 10 && ai->GetAttackerCount() >= 3 && !pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0) && !pTarget->HasAura(PIERCING_HOWL, EFFECT_INDEX_0) && !pTarget->HasAura(SHOCKWAVE, EFFECT_INDEX_0) && !pTarget->HasAura(CONCUSSION_BLOW, EFFECT_INDEX_0) && ai->CastSpell(PIERCING_HOWL, *pTarget))
                out << " > Piercing Howl";
            else if (MOCKING_BLOW > 0 && ai->GetRageAmount() >= 10 && pVictim != m_bot && ai->GetHealthPercent() > 25 && !pTarget->HasAura(MOCKING_BLOW, EFFECT_INDEX_0) && !pTarget->HasAura(CHALLENGING_SHOUT, EFFECT_INDEX_0) && ai->CastSpell(MOCKING_BLOW, *pTarget))
                out << " > Mocking Blow";
            else if (HEROIC_THROW > 0 && ai->CastSpell(HEROIC_THROW, *pTarget))
                out << " > Heroic Throw";
            else if (m_bot->getRace() == RACE_TAUREN && !pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0) && !pTarget->HasAura(PIERCING_HOWL, EFFECT_INDEX_0) && !pTarget->HasAura(SHOCKWAVE, EFFECT_INDEX_0) && !pTarget->HasAura(CONCUSSION_BLOW, EFFECT_INDEX_0) && ai->CastSpell(WAR_STOMP, *pTarget))
                out << " > War Stomp";
            else if (m_bot->getRace() == RACE_HUMAN && m_bot->hasUnitState(UNIT_STAT_STUNNED) || m_bot->HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) || m_bot->HasAuraType(SPELL_AURA_MOD_CHARM) && ai->CastSpell(EVERY_MAN_FOR_HIMSELF, *m_bot))
                out << " > Every Man for Himself";
            else if (m_bot->getRace() == RACE_UNDEAD && m_bot->HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot->HasAuraType(SPELL_AURA_MOD_CHARM) && ai->CastSpell(WILL_OF_THE_FORSAKEN, *m_bot))
                out << " > Will of the Forsaken";
            else if (m_bot->getRace() == RACE_DWARF && m_bot->HasAuraState(AURA_STATE_DEADLY_POISON) && ai->CastSpell(STONEFORM, *m_bot))
                out << " > Stoneform";
            else if (m_bot->getRace() == RACE_GNOME && m_bot->hasUnitState(UNIT_STAT_STUNNED) || m_bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) && ai->CastSpell(ESCAPE_ARTIST, *m_bot))
                out << " > Escape Artist";
            else if (m_bot->getRace() == RACE_NIGHTELF && pVictim == m_bot && ai->GetHealthPercent() < 25 && !m_bot->HasAura(SHADOWMELD, EFFECT_INDEX_0) && ai->CastSpell(SHADOWMELD, *m_bot))
                out << " > Shadowmeld";
            else if (m_bot->getRace() == RACE_ORC && !m_bot->HasAura(BLOOD_FURY, EFFECT_INDEX_0) && ai->CastSpell(BLOOD_FURY, *m_bot))
                out << " > Blood Fury";
            else if (m_bot->getRace() == RACE_TROLL && !m_bot->HasAura(BERSERKING, EFFECT_INDEX_0) && ai->CastSpell(BERSERKING, *m_bot))
                out << " > Berserking";
            else if (m_bot->getRace() == RACE_DRAENEI && ai->GetHealthPercent() < 25 && !m_bot->HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0) && ai->CastSpell(GIFT_OF_THE_NAARU, *m_bot))
                out << " > Gift of the Naaru";
            else
                out << " > NONE";
            break;

        case WarriorDefensive:
            out << "Case Defensive";
            if (DISARM > 0 && ai->GetRageAmount() >= 15 && !pTarget->HasAura(DISARM, EFFECT_INDEX_0) && ai->CastSpell(DISARM, *pTarget))
                out << " > Disarm";
            else if (SHIELD_BLOCK > 0 && !m_bot->HasAura(SHIELD_BLOCK, EFFECT_INDEX_0) && ai->CastSpell(SHIELD_BLOCK, *m_bot))
                out << " > Shield Block";
            else if (SHIELD_WALL > 0 && !m_bot->HasAura(SHIELD_WALL, EFFECT_INDEX_0) && ai->CastSpell(SHIELD_WALL, *m_bot))
                out << " > Shield Wall";
            else
                out << " > NONE";
            break;*/

    }

}

void PlayerbotWarriorAI::DoNonCombatActions()
{
    PlayerbotAI *ai = GetAI();
    Player * m_bot = GetPlayerBot();
	uint32 spec = m_bot->GetSpec();
    if (!m_bot)
        return;

	//Stance Check
	if (spec == WARRIOR_SPEC_ARMS && !m_bot->HasAura(BATTLE_STANCE, EFFECT_INDEX_0))
		ai->CastSpell(BATTLE_STANCE);
	else if (spec == WARRIOR_SPEC_FURY && !m_bot->HasAura(BERSERKER_STANCE, EFFECT_INDEX_0))
		ai->CastSpell(BERSERKER_STANCE);
	else if (spec == WARRIOR_SPEC_PROTECTION && !m_bot->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0))
		ai->CastSpell(DEFENSIVE_STANCE);

    // buff master with VIGILANCE
    if (VIGILANCE > 0)
        (!GetMaster()->HasAura(VIGILANCE, EFFECT_INDEX_0) && ai->CastSpell(VIGILANCE, *GetMaster()));

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = ai->FindFood();
    Item* fItem = ai->FindBandage();

    if (pItem != NULL && ai->GetHealthPercent() < 30)
    {
        ai->TellMaster("I could use some food.");
        ai->UseItem(pItem);
        return;
    }
    else if (pItem == NULL && fItem != NULL && !m_bot->HasAura(RECENTLY_BANDAGED, EFFECT_INDEX_0) && ai->GetHealthPercent() < 70)
    {
        ai->TellMaster("I could use first aid.");
        ai->UseItem(fItem);
        return;
    }
    else if (pItem == NULL && fItem == NULL && m_bot->getRace() == RACE_DRAENEI && !m_bot->HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0) && ai->GetHealthPercent() < 70)
    {
        ai->TellMaster("I'm casting gift of the naaru.");
        ai->CastSpell(GIFT_OF_THE_NAARU, *m_bot);
        return;
    }
} // end DoNonCombatActions

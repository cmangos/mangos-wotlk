
#include "PlayerbotMageAI.h"

class PlayerbotAI;

PlayerbotMageAI::PlayerbotMageAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai)
{
    ARCANE_MISSILES         = ai->getSpellId("arcane missiles"); //ARCANE
    ARCANE_EXPLOSION        = ai->getSpellId("arcane explosion");
    COUNTERSPELL            = ai->getSpellId("counterspell");
    SLOW                    = ai->getSpellId("slow");
    ARCANE_BARRAGE          = ai->getSpellId("arcane barrage");
    ARCANE_BLAST            = ai->getSpellId("arcane blast");
	ARCANE_POWER            = ai->getSpellId("arcane power");
    DAMPEN_MAGIC            = ai->getSpellId("dampen magic");
	AMPLIFY_MAGIC           = ai->getSpellId("amplify magic");
	MAGE_ARMOR              = ai->getSpellId("mage armor");
	MIRROR_IMAGE            = ai->getSpellId("mirror image");
	ARCANE_INTELLECT        = ai->getSpellId("arcane intellect");
    ARCANE_BRILLIANCE       = ai->getSpellId("arcane brilliance");
    DALARAN_INTELLECT       = ai->getSpellId("dalaran intellect");
    DALARAN_BRILLIANCE      = ai->getSpellId("dalaran brilliance");
    MANA_SHIELD             = ai->getSpellId("mana shield");
    CONJURE_WATER           = ai->getSpellId("conjure water");
    CONJURE_FOOD            = ai->getSpellId("conjure food");
    FIREBALL                = ai->getSpellId("fireball"); //FIRE
    FIRE_BLAST              = ai->getSpellId("fire blast");
    FLAMESTRIKE             = ai->getSpellId("flamestrike");
    SCORCH                  = ai->getSpellId("scorch");
    PYROBLAST               = ai->getSpellId("pyroblast");
    BLAST_WAVE              = ai->getSpellId("blast wave");
    COMBUSTION              = ai->getSpellId("combustion");
    DRAGONS_BREATH          = ai->getSpellId("dragon's breath");
    LIVING_BOMB             = ai->getSpellId("living bomb");
    FROSTFIRE_BOLT          = ai->getSpellId("frostfire bolt");
	FIRE_WARD               = ai->getSpellId("fire ward");
	MOLTEN_ARMOR            = ai->getSpellId("molten armor");
    ICY_VEINS               = ai->getSpellId("icy veins"); //FROST
    DEEP_FREEZE             = ai->getSpellId("deep freeze");
    FROSTBOLT               = ai->getSpellId("frostbolt");
    FROST_NOVA              = ai->getSpellId("frost nova");
    BLIZZARD                = ai->getSpellId("blizzard");
    CONE_OF_COLD            = ai->getSpellId("cone of cold");
    ICE_BARRIER             = ai->getSpellId("ice barrier");
    SUMMON_WATER_ELEMENTAL  = ai->getSpellId("summon water elemental");
	FROST_WARD              = ai->getSpellId("frost ward"); 
    ICE_LANCE               = ai->getSpellId("ice lance");
    FROST_ARMOR             = ai->getSpellId("frost armor");
    ICE_ARMOR               = ai->getSpellId("ice armor");
	ICE_BLOCK               = ai->getSpellId("ice block");
	COLD_SNAP               = ai->getSpellId("cold snap");

	RECENTLY_BANDAGED       = 11196; // first aid check

	// racial
	ARCANE_TORRENT          = ai->getSpellId("arcane torrent"); // blood elf
	GIFT_OF_THE_NAARU       = ai->getSpellId("gift of the naaru"); // draenei
	ESCAPE_ARTIST           = ai->getSpellId("escape artist"); // gnome
	EVERY_MAN_FOR_HIMSELF   = ai->getSpellId("every man for himself"); // human
	BERSERKING              = ai->getSpellId("berserking"); // troll
	WILL_OF_THE_FORSAKEN    = ai->getSpellId("will of the forsaken"); // undead
}

PlayerbotMageAI::~PlayerbotMageAI() {}

void PlayerbotMageAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            if (FIREBALL > 0)
                ai->CastSpell(FIREBALL);
            return;
    }

    // ------- Non Duel combat ----------

	//ai->SetMovementOrder( PlayerbotAI::MOVEMENT_FOLLOW, GetMaster() ); // dont want to melee mob

    // Damage Spells (primitive example)
    ai->SetInFront( pTarget );
    Player *m_bot = GetPlayerBot();
	Unit* pVictim = pTarget->getVictim();
	float dist = m_bot->GetDistance( pTarget );

    switch (SpellSequence)
    {
        case SPELL_FROST:
            if (ICY_VEINS > 0 && !m_bot->HasAura(ICY_VEINS, 0) && LastSpellFrost < 1 && ai->GetManaPercent() >= 3)
            {
                ai->CastSpell(ICY_VEINS, *m_bot);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            else if (FROSTBOLT > 0 && LastSpellFrost < 2 && !pTarget->HasAura(FROSTBOLT, 0) && ai->GetManaPercent() >= 16)
            {
                ai->CastSpell(FROSTBOLT, *pTarget);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
			else if (FROST_WARD > 0 && LastSpellFrost < 3 && !m_bot->HasAura(FROST_WARD, 0) && ai->GetManaPercent() >= 19)
            {
                ai->CastSpell(FROST_WARD, *m_bot);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            else if (FROST_NOVA > 0 && LastSpellFrost < 4 && dist <= ATTACK_DISTANCE && !pTarget->HasAura(FROST_NOVA, 0) && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(FROST_NOVA, *pTarget);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
			else if (ICE_LANCE > 0 && LastSpellFrost < 5 && ai->GetManaPercent() >= 7)
            {
                ai->CastSpell(ICE_LANCE, *pTarget);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            else if (BLIZZARD > 0 && LastSpellFrost < 6 && ai->GetAttackerCount()>=5 && ai->GetManaPercent() >= 89)
            {
                ai->CastSpell(BLIZZARD, *pTarget);
				ai->SetIgnoreUpdateTime(8);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            else if (CONE_OF_COLD > 0 && LastSpellFrost < 7 && dist <= ATTACK_DISTANCE && !pTarget->HasAura(CONE_OF_COLD, 0) && ai->GetManaPercent() >= 35)
            {
                ai->CastSpell(CONE_OF_COLD, *pTarget);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
			else if (DEEP_FREEZE > 0 && LastSpellFrost < 8 && pTarget->HasAura(AURA_STATE_FROZEN, 0) && !pTarget->HasAura(DEEP_FREEZE, 0) && ai->GetManaPercent() >= 9)
            {
                ai->CastSpell(DEEP_FREEZE, *pTarget);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            else if (ICE_BARRIER > 0 && LastSpellFrost < 9 && pVictim == m_bot && !m_bot->HasAura(ICE_BARRIER, 0) && ai->GetHealthPercent() < 50 && ai->GetManaPercent() >= 30)
            {
                ai->CastSpell(ICE_BARRIER, *m_bot);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            else if (SUMMON_WATER_ELEMENTAL > 0 && LastSpellFrost < 10 && ai->GetManaPercent() >= 16)
            {
                ai->CastSpell(SUMMON_WATER_ELEMENTAL);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
			else if (ICE_BLOCK > 0 && LastSpellFrost < 11 && pVictim == m_bot && !m_bot->HasAura(ICE_BLOCK, 0) && ai->GetHealthPercent() < 30)
            {
                ai->CastSpell(ICE_BLOCK, *m_bot);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
			else if (COLD_SNAP > 0 && LastSpellFrost < 12)
            {
                ai->CastSpell(COLD_SNAP, *m_bot);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            LastSpellFrost = 0;
            //SpellSequence = SPELL_FIRE;
            //break;

        case SPELL_FIRE:
			if (FIRE_WARD > 0 && !m_bot->HasAura(FIRE_WARD, 0) && LastSpellFire < 1 && ai->GetManaPercent() >= 3)
            {
                ai->CastSpell(FIRE_WARD, *m_bot);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
			else if (COMBUSTION > 0 && !m_bot->HasAura(COMBUSTION, 0) && LastSpellFire < 2)
            {
                ai->CastSpell(COMBUSTION, *m_bot);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (FIREBALL > 0 && LastSpellFire < 3 && ai->GetManaPercent() >= 23)
            {
                ai->CastSpell(FIREBALL, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (FIRE_BLAST > 0 && LastSpellFire < 4 && ai->GetManaPercent() >= 25)
            {
                ai->CastSpell(FIRE_BLAST, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (FLAMESTRIKE > 0 && LastSpellFire < 5 && ai->GetManaPercent() >= 35)
            {
                ai->CastSpell(FLAMESTRIKE, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (SCORCH > 0 && LastSpellFire < 6 && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(SCORCH, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (PYROBLAST > 0 && LastSpellFire < 7 && !pTarget->HasAura(PYROBLAST, 0) && ai->GetManaPercent() >= 27)
            {
                ai->CastSpell(PYROBLAST, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (BLAST_WAVE > 0 && LastSpellFire < 8 && ai->GetAttackerCount()>=3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 34)
            {
                ai->CastSpell(BLAST_WAVE, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (DRAGONS_BREATH > 0 && LastSpellFire < 9 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 37)
            {
                ai->CastSpell(DRAGONS_BREATH, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (LIVING_BOMB > 0 && LastSpellFire < 10 && !pTarget->HasAura(LIVING_BOMB, 0) && ai->GetManaPercent() >= 27)
            {
                ai->CastSpell(LIVING_BOMB, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (FROSTFIRE_BOLT > 0 && LastSpellFire < 11 && !pTarget->HasAura(FROSTFIRE_BOLT, 0) && ai->GetManaPercent() >= 14)
            {
                ai->CastSpell(FROSTFIRE_BOLT, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            LastSpellFire = 0;
            //SpellSequence = SPELL_ARCANE;
            //break;

        case SPELL_ARCANE:
            if (ARCANE_POWER > 0 && LastSpellArcane < 1 && ai->GetManaPercent() >= 37)
            {
                ai->CastSpell(ARCANE_POWER, *pTarget);
                SpellSequence = SPELL_FROST;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
			else if (ARCANE_MISSILES > 0 && LastSpellArcane < 2 && ai->GetManaPercent() >= 37)
            {
                ai->CastSpell(ARCANE_MISSILES, *pTarget);
				ai->SetIgnoreUpdateTime(3);
                SpellSequence = SPELL_FROST;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
            else if (ARCANE_EXPLOSION > 0 && LastSpellArcane < 3 && ai->GetAttackerCount()>=3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 27)
            {
                ai->CastSpell(ARCANE_EXPLOSION, *pTarget);
                SpellSequence = SPELL_FROST;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
            else if (COUNTERSPELL > 0 && pTarget->IsNonMeleeSpellCasted(true) && LastSpellArcane < 4 && ai->GetManaPercent() >= 9)
            {
                ai->CastSpell(COUNTERSPELL, *pTarget);
                SpellSequence = SPELL_FROST;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
            else if (SLOW > 0 && LastSpellArcane < 5 && !pTarget->HasAura(SLOW, 0) && ai->GetManaPercent() >= 12)
            {
                ai->CastSpell(SLOW, *pTarget);
                SpellSequence = SPELL_FROST;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
            else if (ARCANE_BARRAGE > 0 && LastSpellArcane < 6 && ai->GetManaPercent() >= 27)
            {
                ai->CastSpell(ARCANE_BARRAGE, *pTarget);
                SpellSequence = SPELL_FROST;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
            else if (ARCANE_BLAST > 0 && LastSpellArcane < 7 && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(ARCANE_BLAST, *pTarget);
                SpellSequence = SPELL_FROST;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
            else if (MIRROR_IMAGE > 0 && LastSpellArcane < 8 && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(MIRROR_IMAGE);
                SpellSequence = SPELL_FROST;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
			else if (MANA_SHIELD > 0 && LastSpellArcane < 9 && ai->GetHealthPercent() < 70 && pVictim == m_bot && !m_bot->HasAura(MANA_SHIELD, 0) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(MANA_SHIELD, *m_bot);
                SpellSequence = SPELL_FROST;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
            else
            {
                LastSpellArcane = 0;
                SpellSequence = SPELL_FROST;
            }
    }
} // end DoNextCombatManeuver

void PlayerbotMageAI::DoNonCombatActions()
{
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    SpellSequence = SPELL_FROST;
	PlayerbotAI* ai = GetAI();

    // buff master
	if (DALARAN_BRILLIANCE > 0)
        (!GetMaster()->HasAura(DALARAN_BRILLIANCE, 0) && ai->GetManaPercent() >= 81 && ai->CastSpell (DALARAN_BRILLIANCE, *GetMaster()));
	else if (ARCANE_BRILLIANCE > 0)
        (!GetMaster()->HasAura(ARCANE_BRILLIANCE, 0) && !GetMaster()->HasAura(DALARAN_BRILLIANCE, 0) && ai->GetManaPercent() >= 97 && ai->CastSpell (ARCANE_BRILLIANCE, *GetMaster()));

    // buff myself
    if (DALARAN_INTELLECT > 0)
        (!m_bot->HasAura(DALARAN_INTELLECT, 0) && !m_bot->HasAura(DALARAN_BRILLIANCE, 0) && !m_bot->HasAura(ARCANE_BRILLIANCE, 0) && ai->GetManaPercent() >= 31 && ai->CastSpell (DALARAN_INTELLECT, *m_bot));
    else if (ARCANE_INTELLECT > 0)
        (!m_bot->HasAura(ARCANE_INTELLECT, 0) && !m_bot->HasAura(DALARAN_BRILLIANCE, 0) && !m_bot->HasAura(ARCANE_BRILLIANCE, 0) && !m_bot->HasAura(DALARAN_INTELLECT, 0) && ai->GetManaPercent() >= 37 && ai->CastSpell (ARCANE_INTELLECT, *m_bot));

    if (MOLTEN_ARMOR > 0)
        (!m_bot->HasAura(MOLTEN_ARMOR, 0) && !m_bot->HasAura(MAGE_ARMOR, 0) && ai->GetManaPercent() >= 31 && ai->CastSpell (MOLTEN_ARMOR, *m_bot));
	else if (MAGE_ARMOR > 0)
        (!m_bot->HasAura(MAGE_ARMOR, 0) && !m_bot->HasAura(MOLTEN_ARMOR, 0) && ai->GetManaPercent() >= 31 && ai->CastSpell (MAGE_ARMOR, *m_bot));
	else if (ICE_ARMOR > 0)
        (!m_bot->HasAura(ICE_ARMOR, 0) && !m_bot->HasAura(MOLTEN_ARMOR, 0) && !m_bot->HasAura(MAGE_ARMOR, 0) && ai->GetManaPercent() >= 34 && ai->CastSpell (ICE_ARMOR, *m_bot));
    else if (FROST_ARMOR > 0)
        (!m_bot->HasAura(FROST_ARMOR, 0) && !m_bot->HasAura(MOLTEN_ARMOR, 0) && !m_bot->HasAura(MAGE_ARMOR, 0) && !m_bot->HasAura(ICE_ARMOR, 0) && ai->GetManaPercent() >= 34 && ai->CastSpell (FROST_ARMOR, *m_bot));

	// buff master's group
    if (GetMaster()->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(uint64 (itr->guid));
            if( !tPlayer || !tPlayer->isAlive() )
                continue;
			// buff
			(!tPlayer->HasAura(ARCANE_INTELLECT,0) && !tPlayer->HasAura(DALARAN_BRILLIANCE, 0) && !tPlayer->HasAura(ARCANE_BRILLIANCE, 0) && !tPlayer->HasAura(DALARAN_INTELLECT,0) && ai->GetManaPercent() >= 37 && ai->CastSpell (ARCANE_INTELLECT, *tPlayer));
			(!tPlayer->HasAura(DALARAN_INTELLECT,0) && !tPlayer->HasAura(DALARAN_BRILLIANCE, 0) && !tPlayer->HasAura(ARCANE_BRILLIANCE, 0) && ai->GetManaPercent() >= 31 && ai->CastSpell (DALARAN_INTELLECT, *tPlayer));
			(!tPlayer->HasAura(DAMPEN_MAGIC,0) && !tPlayer->HasAura(AMPLIFY_MAGIC,0) && ai->GetManaPercent() >= 32 && ai->CastSpell (DAMPEN_MAGIC, *tPlayer));
			(!tPlayer->HasAura(AMPLIFY_MAGIC,0) && !tPlayer->HasAura(DAMPEN_MAGIC,0) && ai->GetManaPercent() >= 32 && ai->CastSpell (AMPLIFY_MAGIC, *tPlayer));
        }
    }

    // conjure food & water
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = ai->FindDrink();
	Item* fItem = ai->FindBandage();

    if (pItem == NULL && CONJURE_WATER && ai->GetBaseManaPercent() >= 48)
    {
        ai->TellMaster("I'm conjuring some water.");
        ai->CastSpell(CONJURE_WATER, *m_bot);
		ai->SetIgnoreUpdateTime(3);
        return;
    }
    else if (pItem != NULL && ai->GetManaPercent() < 30)
    {
        ai->TellMaster("I could use a drink.");
        ai->UseItem(*pItem);
        ai->SetIgnoreUpdateTime(30);
        return;
    }

    pItem = ai->FindFood();

    if (pItem == NULL && CONJURE_FOOD && ai->GetBaseManaPercent() >= 48)
    {
        ai->TellMaster("I'm conjuring some food.");
        ai->CastSpell(CONJURE_FOOD, *m_bot);
		ai->SetIgnoreUpdateTime(3);
    }

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = ai->FindFood();

    if (pItem != NULL && ai->GetHealthPercent() < 30)
    {
        ai->TellMaster("I could use some food.");
        ai->UseItem(*pItem);
        ai->SetIgnoreUpdateTime(30);
        return;
    }
	else if (pItem == NULL && fItem != NULL && !m_bot->HasAura(RECENTLY_BANDAGED, 0) && ai->GetHealthPercent() < 70)
    {
        ai->TellMaster("I could use first aid.");
        ai->UseItem(*fItem);
        ai->SetIgnoreUpdateTime(8);
        return;
    }

} // end DoNonCombatActions

void PlayerbotMageAI::BuffPlayer(Player* target)
{
    GetAI()->CastSpell(ARCANE_INTELLECT, *target);
}


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
    MIRROR_IMAGE            = ai->getSpellId("mirror image"); //ARCANE
    FIREBALL                = ai->getSpellId("fireball"); //FIRE
    FIREBLAST               = ai->getSpellId("fire blast");
    IMPACT                  = ai->getSpellId("impact");
    FLAMESTRIKE             = ai->getSpellId("flamestrike");
    SCORCH                  = ai->getSpellId("scorch");
    PYROBLAST               = ai->getSpellId("pyroblast");
    BLAST_WAVE              = ai->getSpellId("blast_wave");
    COMBUSTION              = ai->getSpellId("combustion");
    DRAGONS_BREATH          = ai->getSpellId("dragon's breath");
    LIVING_BOMB             = ai->getSpellId("living bomb");
    FROSTFIRE_BOLT          = ai->getSpellId("frostfire bolt"); //FIRE
    CHILLED                 = ai->getSpellId("chilled"); //FROST
    FROSTBITE               = ai->getSpellId("frostbite");
    DEEP_FREEZE             = ai->getSpellId("deep freeze");
    FROSTBOLT               = ai->getSpellId("frostbolt");
    FROST_NOVA              = ai->getSpellId("frost nova");
    BLIZZARD                = ai->getSpellId("blizzard");
    ICY_VEINS               = ai->getSpellId("icy veins");
    CONE_OF_COLD            = ai->getSpellId("cone of cold");
    ICE_BARRIER             = ai->getSpellId("ice barrier");
    SUMMON_WATER_ELEMENTAL  = ai->getSpellId("summon water elemental");
    ICE_LANCE               = ai->getSpellId("ice lance"); //FROST
    FROST_ARMOR             = ai->getSpellId("frost armor");
    ICE_ARMOR               = ai->getSpellId("ice armor");
    MAGE_ARMOR              = ai->getSpellId("mage armor");
    ARCANE_INTELLECT        = ai->getSpellId("arcane intellect");
    ARCANE_BRILLIANCE       = ai->getSpellId("arcane brilliance");
    DALARAN_INTELLECT       = ai->getSpellId("dalaran intellect");
    DALARAN_BRILLIANCE      = ai->getSpellId("dalaran brilliance");
    MANA_SHIELD             = ai->getSpellId("mana shield");
    CONJURE_WATER           = ai->getSpellId("conjure water");
    CONJURE_FOOD            = ai->getSpellId("conjure food");
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

	ai->SetMovementOrder( PlayerbotAI::MOVEMENT_FOLLOW, GetMaster() ); // dont want to melee mob

    // Damage Spells (primitive example)
    ai->SetInFront( pTarget );
    Player *m_bot = GetPlayerBot();

    switch (SpellSequence)
    {
        case SPELL_FROST:
            if (CHILLED > 0 && LastSpellFrost < 1)
            {
                ai->CastSpell(CHILLED, *pTarget);
                SpellSequence = SPELL_FROST;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            else if (FROSTBITE > 0 && LastSpellFrost < 2)
            {
                ai->CastSpell(FROSTBITE, *pTarget);
                SpellSequence = SPELL_FROST;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            else if (DEEP_FREEZE > 0 && LastSpellFrost < 3 && ai->GetManaPercent() >= 9)
            {
                ai->CastSpell(DEEP_FREEZE, *pTarget);
                SpellSequence = SPELL_FROST;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            else if (ICE_LANCE > 0 && LastSpellFrost < 4 && ai->GetManaPercent() >= 7)
            {
                ai->CastSpell(ICE_LANCE, *pTarget);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            else if (FROSTBOLT > 0 && LastSpellFrost < 5 && ai->GetManaPercent() >= 13)
            {
                ai->CastSpell(FROSTBOLT, *pTarget);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            else if (FROST_NOVA > 0 && LastSpellFrost < 6 && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(FROST_NOVA, *pTarget);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            /*else if (BLIZZARD > 0 && LastSpellFrost < 7 && ai->GetManaPercent() >= 74)
            {
                ai->CastSpell(BLIZZARD, *pTarget);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            */
            else if (ICY_VEINS > 0 && LastSpellFrost < 8 && ai->GetManaPercent() >= 3)
            {
                ai->CastSpell(ICY_VEINS);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            else if (CONE_OF_COLD > 0 && LastSpellFrost < 9 && ai->GetManaPercent() >= 35)
            {
                ai->CastSpell(CONE_OF_COLD, *pTarget);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            else if (ICE_BARRIER > 0 && LastSpellFrost < 7 && ai->GetManaPercent() >= 30)
            {
                ai->CastSpell(ICE_BARRIER);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            else if (SUMMON_WATER_ELEMENTAL > 0 && LastSpellFrost < 11 && ai->GetManaPercent() >= 16)
            {
                ai->CastSpell(SUMMON_WATER_ELEMENTAL);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost = LastSpellFrost +1;
                break;
            }
            LastSpellFrost = 0;
            //SpellSequence = SPELL_FIRE;
            //break;

        case SPELL_FIRE:
            if (FIREBALL > 0 && LastSpellFire < 1 && ai->GetManaPercent() >= 23)
            {
                ai->CastSpell(FIREBALL, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (FIREBLAST > 0 && LastSpellFire < 2 && ai->GetManaPercent() >= 25)
            {
                ai->CastSpell(FIREBLAST, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (IMPACT > 0 && LastSpellFire < 3)
            {
                ai->CastSpell(IMPACT, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (FLAMESTRIKE > 0 && LastSpellFire < 4 && ai->GetManaPercent() >= 35)
            {
                ai->CastSpell(FLAMESTRIKE, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (SCORCH > 0 && LastSpellFire < 5 && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(SCORCH, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (PYROBLAST > 0 && LastSpellFire < 6 && ai->GetManaPercent() >= 27)
            {
                ai->CastSpell(PYROBLAST, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (BLAST_WAVE > 0 && LastSpellFire < 7 && ai->GetManaPercent() >= 34)
            {
                ai->CastSpell(BLAST_WAVE);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (COMBUSTION > 0 && LastSpellFire < 8)
            {
                ai->CastSpell(COMBUSTION);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (DRAGONS_BREATH > 0 && LastSpellFire < 9&& ai->GetManaPercent() >= 37)
            {
                ai->CastSpell(DRAGONS_BREATH);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (LIVING_BOMB > 0 && LastSpellFire < 10 && ai->GetManaPercent() >= 27)
            {
                ai->CastSpell(LIVING_BOMB, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellFire = LastSpellFire +1;
                break;
            }
            else if (FROSTFIRE_BOLT > 0 && LastSpellFire < 11 && ai->GetManaPercent() >= 14)
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
            if (ARCANE_MISSILES > 0 && LastSpellArcane < 2 && ai->GetManaPercent() >= 37)
            {
                ai->CastSpell(ARCANE_MISSILES, *pTarget);
                SpellSequence = SPELL_ARCANE;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
            else if (ARCANE_EXPLOSION > 0 && LastSpellArcane < 4 && ai->GetManaPercent() >= 27)
            {
                ai->CastSpell(ARCANE_EXPLOSION);
                SpellSequence = SPELL_FROST;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
            else if (COUNTERSPELL > 0 && LastSpellArcane < 5 && ai->GetManaPercent() >= 9)
            {
                ai->CastSpell(COUNTERSPELL, *pTarget);
                SpellSequence = SPELL_FROST;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
            else if (SLOW > 0 && LastSpellArcane < 6 && ai->GetManaPercent() >= 12)
            {
                ai->CastSpell(SLOW, *pTarget);
                SpellSequence = SPELL_FROST;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
            else if (ARCANE_BARRAGE > 0 && LastSpellArcane < 7 && ai->GetManaPercent() >= 27)
            {
                ai->CastSpell(ARCANE_BARRAGE, *pTarget);
                SpellSequence = SPELL_FROST;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
            else if (ARCANE_BLAST > 0 && LastSpellArcane < 8 && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(ARCANE_BLAST, *pTarget);
                SpellSequence = SPELL_FROST;
                LastSpellArcane = LastSpellArcane +1;
                break;
            }
            else if (MIRROR_IMAGE > 0 && LastSpellArcane < 9 && ai->GetManaPercent() >= 10)
            {
                ai->CastSpell(MIRROR_IMAGE);
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

    // buff myself
    if (DALARAN_INTELLECT > 0)
        (!m_bot->HasAura(DALARAN_INTELLECT, 0) && GetAI()->CastSpell (DALARAN_INTELLECT, *m_bot));
    else if (ARCANE_INTELLECT > 0)
        (!m_bot->HasAura(ARCANE_INTELLECT, 0) && GetAI()->CastSpell (ARCANE_INTELLECT, *m_bot));

    if (ICE_ARMOR > 0)
        (!m_bot->HasAura(ICE_ARMOR, 0) && GetAI()->CastSpell (ICE_ARMOR, *m_bot));
    else if (FROST_ARMOR > 0)
        (!m_bot->HasAura(FROST_ARMOR, 0) && GetAI()->CastSpell (FROST_ARMOR, *m_bot));

    // buff master
    if (DALARAN_INTELLECT > 0)
        (!GetMaster()->HasAura(DALARAN_INTELLECT, 0) && GetAI()->CastSpell (DALARAN_INTELLECT, *(GetMaster())) );
    else if (ARCANE_INTELLECT > 0)
        (!GetMaster()->HasAura(ARCANE_INTELLECT, 0) && GetAI()->CastSpell (ARCANE_INTELLECT, *(GetMaster())) );

    // conjure food & water
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = GetAI()->FindDrink();

    if (pItem == NULL && GetAI()->GetBaseManaPercent() >= 48)
    {
        GetAI()->TellMaster("I'm conjuring some water.");
        GetAI()->CastSpell(CONJURE_WATER, *m_bot);
        return;
    }
    else if (pItem != NULL && GetAI()->GetManaPercent() < 15)
    {
        GetAI()->TellMaster("I could use a drink.");
        GetAI()->UseItem(*pItem);
        GetAI()->SetIgnoreUpdateTime(30);
        return;
    }

    pItem = GetAI()->FindFood();

    if (pItem == NULL && GetAI()->GetBaseManaPercent() >= 48)
    {
        GetAI()->TellMaster("I'm conjuring some food.");
        GetAI()->CastSpell(CONJURE_FOOD, *m_bot);
    }

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = GetAI()->FindFood();

    if (pItem != NULL && GetAI()->GetHealthPercent() < 15)
    {
        GetAI()->TellMaster("I could use some food.");
        GetAI()->UseItem(*pItem);
        GetAI()->SetIgnoreUpdateTime(30);
        return;
    }

/*
    // buff and heal master's group
    if (GetMaster()->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = GetMaster()->GetObjPlayer(itr->guid);

            // first rezz em
            if (tPlayer->isDead())
            {
                std::string msg = "rezzing ";
                msg += tPlayer->GetName();
                GetPlayerBot()->Say(msg, LANG_UNIVERSAL);
                GetAI()->CastSpell(REZZ, *tPlayer);
                // rez is only 10 sec, but give time for lag
                GetAI()->SetIgnoreUpdateTime(15);
            }
            else
            {
                // buff and heal
                (!tPlayer->HasAura(FORTITUDE,0) && GetAI()->CastSpell (FORTITUDE, *tPlayer));
                (HealTarget(*tPlayer, tPlayer->GetHealth()*100 / tPlayer->GetMaxHealth()));
            }
        }
    }
*/
} // end DoNonCombatActions

void PlayerbotMageAI::BuffPlayer(Player* target)
{
    GetAI()->CastSpell(ARCANE_INTELLECT, *target);
}

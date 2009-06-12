
#include "PlayerbotWarlockAI.h"

class PlayerbotAI;
PlayerbotWarlockAI::PlayerbotWarlockAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai)
{
    SHADOW_BOLT           = ai->getSpellId("shadow bolt");
    IMMOLATE              = ai->getSpellId("immolate");
    INCINERATE            = ai->getSpellId("incinerate");
    SEARING_PAIN          = ai->getSpellId("searing pain");
    CONFLAGRATE           = ai->getSpellId("conflagrate");
    SOUL_FIRE             = ai->getSpellId("soul fire");
    SHADOWFURY            = ai->getSpellId("shadowfury");
    CORRUPTION            = ai->getSpellId("corruption"); //affliction
    CURSE_OF_WEAKNESS     = ai->getSpellId("curse of weakness"); //CURSE
    CHAOS_BOLT            = ai->getSpellId("chaos bolt");
    CURSE_OF_AGONY        = ai->getSpellId("curse of agony"); //CURSE
    DEMON_SKIN            = ai->getSpellId("demon skin"); //DEMONOLOGY
    DEMON_ARMOR           = ai->getSpellId("demon armor");//DEMONOLOGY
    FEL_ARMOR             = ai->getSpellId("fel armor");//DEMONOLOGY
    SHADOW_WARD           = ai->getSpellId("shadow ward");//DEMONOLOGY
    SOULSHATTER           = ai->getSpellId("soulshatter"); //DEMONOLOGY
    DRAIN_SOUL            = ai->getSpellId("drain soul"); //affliction
    CURSE_OF_RECKLESSNESS = ai->getSpellId("curse of recklessness"); //CURSE
    DRAIN_LIFE            = ai->getSpellId("drain life"); //affliction
    CURSE_OF_TONGUES      = ai->getSpellId("curse of tongues"); //CURSE
    SIPHON_LIFE           = ai->getSpellId("siphon life"); //affliction
    UNSTABLE_AFFLICTION   = ai->getSpellId("unstable affliction"); //affliction
    HAUNT                 = ai->getSpellId("haunt"); //affliction
    ATROCITY              = ai->getSpellId("atrocity"); //affliction
    SEED_OF_CORRUPTION    = ai->getSpellId("seed of corruption"); //affliction
    SUMMON_IMP            = ai->getSpellId("summon imp"); //DEMONOLOGY
    SUMMON_VOIDWALKER     = ai->getSpellId("summon voidwalker"); //DEMONOLOGY
    SUMMON_SUCCUBUS       = ai->getSpellId("summon succubus"); //DEMONOLOGY
    SUMMON_FELHUNTER      = ai->getSpellId("summon fellhunter"); //DEMONOLOGY
    SUMMON_FELGUARD       = ai->getSpellId("summon fellguard"); //DEMONOLOGY
}

PlayerbotWarlockAI::~PlayerbotWarlockAI() {}

void PlayerbotWarlockAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            if (SHADOW_BOLT > 0)
                ai->CastSpell(SHADOW_BOLT);
            return;
    }

    // ------- Non Duel combat ----------

    ai->Follow(*GetMaster()); // dont want to melee mob

    // Damage Spells
    ai->SetInFront( pTarget );
    Player *m_bot = GetPlayerBot();

    switch (SpellSequence)
    {
        case SPELL_CURSES:
            if (CURSE_OF_WEAKNESS > 0 && LastSpellCurse < 1 && ai->GetManaPercent() >= 14)
            {
                if (!ai->HasAura(CURSE_OF_WEAKNESS, *pTarget))
                {
                    ai->CastSpell(CURSE_OF_WEAKNESS, *pTarget);
                    SpellSequence = SPELL_AFFLICTION;
                    LastSpellCurse = LastSpellCurse +1;
                    break;
                 }
            }
            else if (CURSE_OF_AGONY > 0 && LastSpellCurse < 6 && ai->GetManaPercent() >= 14)
            {
                if (!ai->HasAura(CURSE_OF_AGONY, *pTarget))
                {
                    ai->CastSpell(CURSE_OF_AGONY, *pTarget);
                    SpellSequence = SPELL_AFFLICTION;
                    LastSpellCurse = LastSpellCurse +1;
                    break;
                }
            }
            else if (CURSE_OF_RECKLESSNESS > 0 && LastSpellCurse < 12 && ai->GetManaPercent() >= 8)
            {
                if (!ai->HasAura(CURSE_OF_RECKLESSNESS, *pTarget))
                {
                    ai->CastSpell(CURSE_OF_RECKLESSNESS, *pTarget);
                    SpellSequence = SPELL_AFFLICTION;
                    LastSpellCurse = LastSpellCurse +1;
                    break;
                }
            }
            else if (CURSE_OF_TONGUES > 0 && LastSpellCurse < 18 && ai->GetManaPercent() >= 4)
            {
                if (!ai->HasAura(CURSE_OF_TONGUES, *pTarget))
                {
                    ai->CastSpell(CURSE_OF_TONGUES, *pTarget);
                    SpellSequence = SPELL_AFFLICTION;
                    LastSpellCurse = LastSpellCurse +1;
                    break;
                 }
            }
            else if (ai->HasAura(CURSE_OF_WEAKNESS, *pTarget) || ai->HasAura(CURSE_OF_AGONY, *pTarget) || ai->HasAura(CURSE_OF_RECKLESSNESS, *pTarget) || ai->HasAura(CURSE_OF_TONGUES, *pTarget))
            {
                SpellSequence = SPELL_AFFLICTION;
                LastSpellCurse = LastSpellCurse +1;
                break;
            }
            LastSpellCurse = 0;
            //SpellSequence = SPELL_AFFLICTION;
            //break;

        case SPELL_AFFLICTION:
            if (CORRUPTION > 0 && LastSpellAffliction < 1 && ai->GetManaPercent() >= 19)
            {
                ai->CastSpell(CORRUPTION, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
            else if (DRAIN_SOUL > 0 && LastSpellAffliction < 2 && ai->GetManaPercent() >= 19)
            {
                ai->CastSpell(DRAIN_SOUL, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
            else if (DRAIN_LIFE > 0 && LastSpellAffliction < 3 && ai->GetManaPercent() >= 23)
            {
                ai->CastSpell(DRAIN_LIFE, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
            else if (SIPHON_LIFE > 0 && LastSpellAffliction < 4 && ai->GetManaPercent() >= 22)
            {
                ai->CastSpell(SIPHON_LIFE, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
            else if (UNSTABLE_AFFLICTION > 0 && LastSpellAffliction < 5 && ai->GetManaPercent() >= 20)
            {
                ai->CastSpell(UNSTABLE_AFFLICTION, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
            else if (HAUNT > 0 && LastSpellAffliction < 6 && ai->GetManaPercent() >= 12)
            {
                ai->CastSpell(HAUNT, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
            else if (ATROCITY > 0 && LastSpellAffliction < 7 && ai->GetManaPercent() >= 21)
            {
                ai->CastSpell(ATROCITY, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
            else if (SEED_OF_CORRUPTION > 0 && LastSpellAffliction < 8 && ai->GetManaPercent() >= 34)
            {
                ai->CastSpell(SEED_OF_CORRUPTION, *pTarget);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction = LastSpellAffliction +1;
                break;
            }
            LastSpellDestruction = 0;
            //SpellSequence = SPELL_DESTRUCTION;
            //break;

          case SPELL_DESTRUCTION:
              if (SHADOW_BOLT > 0 && LastSpellDestruction < 1 && ai->GetManaPercent() >= 23)
              {
                  ai->CastSpell(SHADOW_BOLT, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else if (IMMOLATE > 0 && LastSpellDestruction < 2 && ai->GetManaPercent() >= 23)
              {
                  ai->CastSpell(IMMOLATE, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else if (INCINERATE > 0 && LastSpellDestruction < 3 && ai->GetManaPercent() >= 19)
              {
                  ai->CastSpell(INCINERATE, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else if (SEARING_PAIN > 0 && LastSpellDestruction < 4 && ai->GetManaPercent() >= 11)
              {
                  ai->CastSpell(SEARING_PAIN, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else if (CONFLAGRATE > 0 && LastSpellDestruction < 5 && ai->GetManaPercent() >= 16)
              {
                  ai->CastSpell(CONFLAGRATE, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else if (SOUL_FIRE > 0 && LastSpellDestruction < 6 && ai->GetManaPercent() >= 13)
              {
                  ai->CastSpell(SOUL_FIRE, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else if (SHADOWFURY > 0 && LastSpellDestruction < 7 && ai->GetManaPercent() >= 37)
              {
                  ai->CastSpell(SHADOWFURY, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else if (CHAOS_BOLT > 0 && LastSpellDestruction < 8 && ai->GetManaPercent() >= 9)
              {
                  ai->CastSpell(CHAOS_BOLT, *pTarget);
                  SpellSequence = SPELL_CURSES;
                  LastSpellDestruction = LastSpellDestruction + 1;
                  break;
              }
              else
              {
                  LastSpellDestruction = 0;
                  SpellSequence = SPELL_CURSES;
              }
    }
} // end DoNextCombatManeuver

void PlayerbotWarlockAI::DoNonCombatActions()
{
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    SpellSequence = SPELL_CURSES;

    // buff myself  DEMON_SKIN, DEMON_ARMOR, SHADOW_WARD, FEL_ARMOR

    if (FEL_ARMOR > 0)
        (!m_bot->HasAura(FEL_ARMOR, 0) && GetAI()->CastSpell(FEL_ARMOR, *m_bot));
    else if (DEMON_ARMOR > 0)
        (!m_bot->HasAura(DEMON_ARMOR, 0) && GetAI()->CastSpell(DEMON_ARMOR, *m_bot));
    else if (DEMON_SKIN > 0)
        (!m_bot->HasAura(DEMON_SKIN, 0) && GetAI()->CastSpell(DEMON_SKIN, *m_bot));

    // mana check
    if (m_bot->getStandState() != PLAYER_STATE_NONE)
        m_bot->SetStandState(PLAYER_STATE_NONE);

    Item* pItem = GetAI()->FindDrink();

    if (pItem != NULL && GetAI()->GetManaPercent() < 15)
    {
        GetAI()->TellMaster("I could use a drink.");
        GetAI()->UseItem(*pItem);
        GetAI()->SetIgnoreUpdateTime(30);
        return;
    }

    // hp check
    if (m_bot->getStandState() != PLAYER_STATE_NONE)
        m_bot->SetStandState(PLAYER_STATE_NONE);

    pItem = GetAI()->FindFood();

    if (pItem != NULL && GetAI()->GetHealthPercent() < 15)
    {
        GetAI()->TellMaster("I could use some food.");
        GetAI()->UseItem(*pItem);
        GetAI()->SetIgnoreUpdateTime(30);
        return;
    }


} // end DoNonCombatActions

//void PlayerbotWarlockAI::BuffPlayer(Player* target)
//{
//    GetAI()->CastSpell(ARCANE_INTELLECT, *target);
//}

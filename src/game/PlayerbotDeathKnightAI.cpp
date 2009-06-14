// a simple DK class by rrtn :)

#include "PlayerbotDeathKnightAI.h"

class PlayerbotAI;
PlayerbotDeathKnightAI::PlayerbotDeathKnightAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai) {
    
    PLAGUE_STRIKE     = ai->getSpellId("plague strike"); // Unholy
    DEATH_GRIP        = ai->getSpellId("death grip");
    DEATH_COIL        = ai->getSpellId("death coil");
    DEATH_STRIKE      = ai->getSpellId("death strike");
    UNHOLY_BLIGHT     = ai->getSpellId("unholy blight");
    SCOURGE_STRIKE    = ai->getSpellId("scourge strike");
    ICY_TOUCH         = ai->getSpellId("icy touch"); // Frost
    OBLITERATE        = ai->getSpellId("obliterate");
    HOWLING_BLAST     = ai->getSpellId("howling blast");
    FROST_STRIKE      = ai->getSpellId("frost strike");
    FROSTFEVER        = ai->getSpellId("frostfever");
    RUNE_STRIKE       = ai->getSpellId("rune strike");
    ICY_CLUTCH        = ai->getSpellId("icy clutch");
    BLOOD_STRIKE      = ai->getSpellId("blood strike"); // Blood
    PESTILENCE        = ai->getSpellId("pestilence");
    STRANGULATE       = ai->getSpellId("strangulate");
    BLOOD_BOIL        = ai->getSpellId("blood boil");
    HEART_STRIKE      = ai->getSpellId("heart strike");
    MARK_OF_BLOOD     = ai->getSpellId("mark of blood"); // buffs
    KILLING_MACHINE   = ai->getSpellId("killing machine");
    HORN_OF_WINTER    = ai->getSpellId("horn of winter");
    UNHOLY_PRESENCE   = 48265; // presence
    FROST_PRESENCE    = 48263;
    BLOOD_PRESENCE    = 48266;
}

PlayerbotDeathKnightAI::~PlayerbotDeathKnightAI() {}

void PlayerbotDeathKnightAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            ai->CastSpell(PLAGUE_STRIKE);
            return;
    }

    // ------- Non Duel combat ----------

    //ai->Follow(*GetMaster()); // dont want to melee mob

    // DK Attcks: Unholy, Frost & Blood

    // damage spells
    ai->SetInFront( pTarget );
    Player *m_bot = GetPlayerBot();

    switch (SpellSequence)
    {
        case SPELL_DK_UNHOLY:
            if(PLAGUE_STRIKE > 0 && LastSpellUnholyDK < 1)
            {
                ai->CastSpell(PLAGUE_STRIKE, *pTarget);
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK +1;
                break;
            }
            else if (DEATH_GRIP > 0 && LastSpellUnholyDK < 2)
            {
                ai->CastSpell(DEATH_GRIP);
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK +1;
                break;
            }
            else if (DEATH_COIL > 0 && LastSpellUnholyDK <3 && ai->GetRunicPower() >= 40)
            {
                ai->CastSpell(DEATH_COIL);
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK +1;
                break;
            }
            else if (DEATH_STRIKE > 0 && LastSpellUnholyDK < 4)
            {
                ai->CastSpell(DEATH_STRIKE);
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK +1;
                break;
            }
            else if (UNHOLY_BLIGHT > 0 && LastSpellUnholyDK <5 && ai->GetRunicPower() >= 40)
            {
                ai->CastSpell(UNHOLY_BLIGHT);
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK +1;
                break;
            }
            else if (SCOURGE_STRIKE > 0 && LastSpellUnholyDK < 6)
            {
                ai->CastSpell(SCOURGE_STRIKE);
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK +1;
                break;
            }
            else if (LastSpellUnholyDK > 7)
            {
                LastSpellUnholyDK = 0;
                SpellSequence = SPELL_DK_FROST;
                break;
            }
            LastSpellUnholyDK = 0;

        case SPELL_DK_FROST:
            if (KILLING_MACHINE > 0)
                (!m_bot->HasAura(KILLING_MACHINE, 0) && GetAI()->CastSpell (KILLING_MACHINE, *m_bot));

            if (ICY_TOUCH > 0 && LastSpellFrostDK < 1)
            {
                ai->CastSpell(ICY_TOUCH, *pTarget);
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK +1;
                break;
            }
            else if (OBLITERATE > 0 && LastSpellFrostDK < 2)
            {
                ai->CastSpell(OBLITERATE);
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK +1;
                break;
            }
            else if (FROST_STRIKE > 0 && LastSpellFrostDK < 3 && ai->GetRunicPower() >= 32)
            {
                ai->CastSpell(FROST_STRIKE);
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK +1;
                break;
            }
            else if (HOWLING_BLAST > 0 && LastSpellFrostDK < 4)
            {
                ai->CastSpell(HOWLING_BLAST);
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK +1;
                break;
            }
            else if (RUNE_STRIKE > 0 && LastSpellFrostDK < 5 && ai->GetRunicPower() >= 20)
            {
                ai->CastSpell(RUNE_STRIKE);
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK +1;
                break;
            }
            else if (FROSTFEVER > 0 && LastSpellFrostDK < 6)
            {
                ai->CastSpell(FROSTFEVER);
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK +1;
                break;
            }
            else if (ICY_CLUTCH > 0 && LastSpellFrostDK < 7)
            {
                ai->CastSpell(ICY_CLUTCH, *pTarget);
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK +1;
                break;
            }
            else if (LastSpellFrostDK > 8)
            {
                LastSpellFrostDK = 0;
                SpellSequence = SPELL_DK_BLOOD;
                break;
            }
            LastSpellFrostDK = 0;

        case SPELL_DK_BLOOD:
            if (MARK_OF_BLOOD > 0)
                (!m_bot->HasAura(MARK_OF_BLOOD, 0) && GetAI()->CastSpell (MARK_OF_BLOOD, *m_bot));

            if (BLOOD_STRIKE > 0 && LastSpellBloodDK < 1)
            {
                ai->CastSpell(BLOOD_STRIKE, *pTarget);
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK =  LastSpellBloodDK +1;
                break;
            }
            else if (PESTILENCE > 0 && LastSpellBloodDK < 2)
            {
                ai->CastSpell(PESTILENCE);
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK +1;
                break;
            }
            else if (STRANGULATE > 0 && LastSpellBloodDK < 3)
            {
                ai->CastSpell(STRANGULATE);
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK +1;
                break;
            }
            else if (BLOOD_BOIL > 0 && LastSpellBloodDK < 4)
            {
                ai->CastSpell(BLOOD_BOIL);
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK +1;
                break;
            }
            else if (HEART_STRIKE > 0 && LastSpellFrostDK < 5)
            {
                ai->CastSpell(HEART_STRIKE);
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK +1;
                break;
            }
            else if (LastSpellBloodDK > 6)
            {
                LastSpellBloodDK = 0;
                SpellSequence = SPELL_DK_UNHOLY;
                break;
            }
            else
            {
                LastSpellBloodDK = 0;
                SpellSequence = SPELL_DK_UNHOLY;
            }
    }
} // end DoNextCombatManeuver

void PlayerbotDeathKnightAI::DoNonCombatActions()
{
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    SpellSequence = SPELL_DK_UNHOLY;

    // buff myself (Unholy DK) UNHOLY_PRESENCE, HORN_OF_WINTER
    if (UNHOLY_PRESENCE > 0)
        (!m_bot->HasAura(UNHOLY_PRESENCE, 0) && GetAI()->CastSpell (UNHOLY_PRESENCE, *m_bot));

    if (HORN_OF_WINTER > 0)
        (!m_bot->HasAura(HORN_OF_WINTER, 0) && GetAI()->CastSpell (HORN_OF_WINTER, *m_bot));

    // buff master with HORN_OF_WINTER too
    if (HORN_OF_WINTER> 0)
        (!GetMaster()->HasAura(HORN_OF_WINTER,0) && GetAI()->CastSpell (HORN_OF_WINTER, *(GetMaster())) );

    // hp check
    if (m_bot->getStandState() != PLAYER_STATE_NONE)
        m_bot->SetStandState(PLAYER_STATE_NONE);

    Item* pItem = GetAI()->FindFood();

    if (pItem != NULL && GetAI()->GetHealthPercent() < 15)
    {
        GetAI()->TellMaster("I could use some food.");
        GetAI()->UseItem(*pItem);
        GetAI()->SetIgnoreUpdateTime(30);
        return;
    }

} // end DoNonCombatActions

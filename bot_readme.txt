What it is:
===========

Playerbot lets you add another character from your account as a bot that you can control and which will hopefully help you. Only characters from your account can be used, so you can have a maximum of 9 bots at one time.

This was taken from the Trinity site, and modified slightly by me to get some of the kinks out. I reworked the priest class and also added a mage class and a warrior class, which are still in crude form. Any class can be used as a bot - just don't expect much in the way of spells or abilities until someone writes the code for them.

Bots will only use abilities that they have - for example, a priest will only use the renew spell if it has been trained. Also, bot's equipment will lose durability like any other character. So every so often you'll need to log in and repair and train your bot.

For Mangos 7800+

Commands:
=========

/s .bot add BOTNAME (add character to world)
/s .bot remove BOTNAME
/s .bot co|combatorder BOTNAME COMBATORDER [TARGET]
/invite BOTNAME (bot will auto accept invite)
/t BOTNAME attack (bot will attack selected target, similar to the way a pet can attack)
/t BOTNAME follow (orders bot to follow player; will also revive bot if dead or teleport bot if far away)
/t BOTNAME stay
/t BOTNAME assist (you'll need to be attacking something and the bot only does melee atm)
/t BOTNAME spells (replies with all spells known to bot)
/t BOTNAME cast <SPELLID>
/t BOTNAME cast <SPELL SUBSTRING>
/t BOTNAME use <ITEM LINK>
/t BOTNAME equip <ITEM LINK>
/t BOTNAME reset (will reset states, orders and loot list)
/t BOTNAME report (bot reports all items needed to finish quests)
/t BOTNAME free (bot shows free empty space in its inventory)

Shortcuts:
c = cast
e = equip
u = use

Combat Orders explained:
========================

  There are primary and secondary commands which can be combined. In this way it is
  possible to define a bot to assist the main tank and also protect the healer, making
  combat management much easier.
  The commands assist and protect require a target parameter or a friendly player
  selected by bots master.
  Available Combat Orders:
    tank        pri     try to bind all targets involved in combat by gaining highest threat
    assist      pri     do damage on selected targets attacker without getting highest threat
    heal        pri     concentrate on healing - no offensive spells, try to keep threat low
    protect     sec     if target of protect get's directly attacked gain higher threat on attacker
    reset       -       clear out assist and protect targets and set combat order to nothing
  Examples:
    .bot co TheTank tank
    .bot co MyHealer heal
    .bot co TheBrutal assist TheTank
    .bot co TheBrutal protect MyHealer

Trading with bots:
==================

To trade items/money with your bot simply initiate a trade and the bot will tell you how much money and items are available. To request an item simple whisper the bot and shift click the link of the item you would like. You can link multiple items on the same line. You can also request money in the following manner when the trade window is open:
/w BOTNAME 10g <-- request that the bot give you 10 gold
/w BOTNAME 6g500s25c <-- request 6 gold, 500 silver, and 25 cooper

A bot is also able to show an item in its 'Will not be traded' slot. The item can be either
in its bags or be equipped and even be soulbound. By this you can cast spells/enchantments
on soulbound items ('nt' stands for 'not trading').
/w BOTNAME nt [Powerful Soulbound Item]

More Information:
=================

If specifying a spell substring, the spell chosen will be in priority of exact name match, highest spell rank, and spell using no reagents. Case does not matter. Here's some examples:
/t BOTNAME c greater heal
/t BOTNAME cast pain
/w BOTNAME c poly
/w BOTNAME cast fort
/t BOTNAME cast <SPELLID>
- OR -
/w BOTNAME c <SPELLID>

Also all commands can be broadcast to the party. For example:
/p follow
/p spells

To use or equip items for your bot say:
/w BOTNAME use <ITEMLINK1> <ITEMLINK2>
/w BOTNAME equip <ITEMLINK1> <ITEMLINK2>
- OR -
/w BOTNAME u <ITEMLINK1> <ITEMLINK2>
/w BOTNAME e <ITEMLINK1> <ITEMLINK2>

If you inspect your bot, your bot will tell you what items you have in your inventory that you can equip. To create a link in the chat window, hold the shift key and press the left mouse button when clicking the link.


Changes from Trinity to Mangos:
===============================

I added the following in SharedDefines.h.

enum SpellCategory
{
	SPELL_CATEGORY_FOOD             = 11,
	SPELL_CATEGORY_DRINK            = 59
};

I also had to add the following to Player.h:

enum PlayerStateType
{
	PLAYER_STATE_NONE              = 0,
	PLAYER_STATE_SIT               = 1
};


Configuration variables:
========================
Also see src/mangosd/mangosd.conf.dist for configuration variables!

    PlayerbotAI.DebugWhisper
        Enable debug output by whispering master
        Default: 0 - off
                 1 - on

    PlayerbotAI.FollowDistanceMin
    PlayerbotAI.FollowDistanceMax
        Min. and max. follow distance for bots
        Default: 0.5 / 1.0


Some Problems:
==============

The bots don't always face in the right direction. Sometimes when a bot makes the kill, the corpse is not lootable. The mage bot sometimes get stuck when he begins to cast a spell (but this is corrected the next time he enters combat).

What it is:
===========

Playerbot lets you add another character from your account as a bot that you can control and which will hopefully help you. Only characters from your account can be used, so you can have a maximum of 9 bots at one time.

This was taken from the Trinity site, and modified slightly by me to get some of the kinks out. I reworked the priest class and also added a mage class and a warrior class, which are still in crude form. Any class can be used as a bot - just don't expect much in the way of spells or abilities until someone writes the code for them.

Bots will only use abilities that they have - for example, a priest will only use the renew spell if it has been trained. Also, bot's equipment will lose durability like any other character. So every so often you'll need to log in and repair and train your bot.

For MaNGOS 10816+

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
/t BOTNAME cast <SPELLID | (part of) SPELLNAME | SPELLLINK>
/t BOTNAME use <ITEM LINK>
/t BOTNAME equip <ITEM LINK>
/t BOTNAME reset (will reset states, orders and loot list)
/t BOTNAME report (bot reports all items needed to finish quests)
/t BOTNAME stats (bot shows available money, free inventory space and estimated item repair costs)
/t BOTNAME survey (bot shows all available gameobjects, within a local perimeter around the bot)
/t BOTNAME find <GAMEOBJECT LINK> (bot will travel to the gameobject location and then wait)
/t BOTNAME get <GAMEOBJECT LINK> (bot will fetch the selected gameobject and then return to the player)
/t BOTNAME quests (List bot's current quests)
/t BOTNAME drop <QUESTLINK> (Drop a quest)
/t BOTNAME orders (Shows bot's combat orders)
/t BOTNAME pet spells (Shows spells known to bot's pet. Autocast spells will be shown in green)
/t BOTNAME pet cast <SPELLID | (part of) SPELLNAME | SPELLLINK>
/t BOTNAME pet toggle <SPELLID | (part of) SPELLNAME | SPELLLINK> (Toggle autocast for a given spell)
/t BOTNAME pet state (Shows current react mode of bot's pet)
/t BOTNAME pet react <(a)ggressive | (d)efensive | (p)assive> (Set bot's pet reaction mode)
/t BOTNAME collect (shows collect subcommand options and current collect status)
/t BOTNAME collect <subcommand(s)> (subcommands can be alone or together [none combat loot objects profession quest])
/t BOTNAME sell <ITEM LINK> (bot will add item to it's m_itemIds, for later sale)
/t BOTNAME auction (bot will display all it's active owned auctions. Auction info will include an <AUCTION LINK> )
/t BOTNAME auction add <ITEM LINK> (bot will add item to it's m_itemIds, for later auction)
/t BOTNAME auction remove <AUCTION LINK> (bot will add auctionid to it's m_auctions, for later auction cancellation)
/t BOTNAME repair <ITEM LINK> (bot will seek out armourer and repair selected items specified by <ITEM LINK> )
/t BOTNAME repair all (bot(s) will seek out armourer and repair all damaged items equipped, or in bags )
/t BOTNAME talent (Lists bot(s) active talents [TALENT LINK] & glyphs [GLYPH LINK], unspent talent points & cost to reset all talents)
/t BOTNAME talent learn [TALENT LINK] .. (Learn selected talent from bot client 'inspect' dialog -> 'talent' tab or from talent command (shift click icon or link))
/t BOTNAME talent reset (Resets all talents)
/t BOTNAME bank  (Lists bot(s) bank balance)
/t BOTNAME bank deposit [Item Link][Item Link] .. (Deposit item(s) in bank)
/t BOTNAME bank withdraw [Item Link][Item Link] ..  (Withdraw item(s) from bank. ([Item Link] from bank))

Shortcuts:
c = cast
e = equip
u = use

Gameobject interaction with bots:
=================================

  The bot(s) can interact with gameobjects. This is particularly useful, in order to complete 'gather' type
  quests (e.g Milly's harvest in Northshire). The bot(s) can also harvest 'ore deposits' and 'herbs'

  Four commands have been introduced 'collect', 'survey, 'find' & 'get', to facilitate this feature.

  The 'collect' command provides a way to have a bot automatically get loot or objects in the world.
  The command by itself gives you a display of what settings the bots have. Defaults are stored in
  the playerbot.conf. The subcommands do the actual work:
    combat - bot will loot after combat for the options that are set
    loot - bot will grab all loot available from corpse
    quest - bot will grab all quest items on corpse
    profession - bot will grab any profession related item that the bot has from corpse
    skin - if you have skinning, and corpse has been looted, bot will skin corpse. Most of
        the time you may want this option and the loot option enabled, but if you are grabbing
        the stuff as you are going around, the bot will skin afterwards.
    objects - bot will collect things that are specified by the survey and get <shift-click> commands
    none - removes any collect options that have been set

  The 'survey' command provides the means for bot(s) to detect gameobjects in the world. It can be used to detect
  available gameobjects local to a single bot, or more effectively (wider area) those for a party of bots.

  Suggestion: setup the 'survey' command as an assigned macro button, on the client (e.g /p survey). You can
  then quickly refresh the gameobject list.

  Gameobject list <GAMEOBJECT LINK> (Currently bots can only interact with ore, herb and needed quest items)
  ---------------

  [Copper Vein][Silverleaf][Earthroot][Milly's Harvest][Battered Chest][Food Crate]

  Then, use the 'find' or 'get' commands to interect with the gameobject.

  Using the gameobject list information, it is possible to locate and/or fetch each of the gameobjects. To select
  a <GAMEOBJECT LINK>, hold down the shift key and click on the relevant link with your mouse.

Creature interaction with bots:
===============================

  The bot(s) can now interact directly with creatures. This will enable bot(s) to independently contact NPCs, without
  the need for player's selection. This opens up new possiblities for bot/NPC commerce.

  Each bot will maintain it's own item list (m_itemIds - This is a list of paired data). The first component (UNIT_NPC_FLAG)
  dictates what is to done with the second component (itemid).

  Each bot will also maintain it's own active auction list (m_auctions - This is a list of paired data). The first component
  (ActionFlags) dictates the action to be performed on the second component (auctionid).

  While(m_itemIds not empty)
  {
      bot will scan for surrounding creatures (findNearbyCreature()) who can service m_itemIds
      if(found)
          Search is carried out for all itemids that can be processed by this creature.
          if(processed successfully)
              This instance of the data pair (npcflag,itemid) will be removed from m_itemIds
              While(m_auctions not empty)
                  process contained auctionids according to ActionFlag & then update m_auctions
  }

  Please note that bot(s) m_itemIds & m_auctions will be emptied, when bot(s) are dismissed.

Repair with bots:
=================

  bot(s) can now repair <all or selected> items, either equipped or in bags. If the bot(s) is a member of a guild, then they pay
  else the bot(s) pays. If the bot(s) cannot pay for the repair, they remain damaged.

  The new 'stats' command provides useful information to help in the repair decision.

  First:  Money available to bot(s)
  Second: Free inventory slots for bot(s)
  Third:  Estimated (excludes NPC reputation discount) item damage cost for bot(s).

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

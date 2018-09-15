ALTER TABLE db_version CHANGE COLUMN required_13980_01_mangos_string required_13981_01_mangos_string bit;

DELETE FROM mangos_string WHERE entry BETWEEN 616 AND 635;
DELETE FROM mangos_string WHERE entry BETWEEN 639 AND 646;
INSERT INTO mangos_string VALUES
(616,'The battle for the Strand of the Ancients begins in 2 minutes.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(617,'The battle for the Strand of the Ancients begins in 1 minute.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(618,'The battle for the Strand of the Ancients begins in 30 seconds. Prepare yourselves!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(619,'Let the battle for the Strand of the Ancients begin!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(620,'Round 1 - Finished!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(621,'Round 2 of the Battle for the Strand of the Ancients begins in 1 minute.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(622,'Round 2 begins in 30 seconds. Prepare yourselves!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(623,'The battle for the Strand of the Ancients has ended!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(624,'The Gate of the Green Emerald is under attack!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(625,'The Gate of the Green Emerald has been destroyed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(626,'The Gate of the Purple Amethyst under attack!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(627,'The Gate of the Purple Amethyst has been destroyed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(628,'The Gate of the Blue Sapphire is under attack!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(629,'The Gate of the Blue Sapphire has been destroyed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(630,'The Gate of the Red Sun is under attack!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(631,'The Gate of the Red Sun has been destroyed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(632,'The Gate of the Yellow Moon is under attack!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(633,'The Gate of the Yellow Moon has been destroyed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(634,'The relic chamber is under attack!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(635,'The chamber has been breached! The titan relic is vulnerable!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(639,'The Alliance captured the titan portal!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(640,'The Horde captured the titan portal!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(641,'The Eastern Graveyard has been captured by the Horde!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(642,'The Western Graveyard has been captured by the Horde!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(643,'The Southern Graveyard has been captured by the Horde!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(644,'The Eastern Graveyard has been captured by the Alliance!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(645,'The Western Graveyard has been captured by the Alliance!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(646,'The Southern Graveyard has been captured by the Alliance!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

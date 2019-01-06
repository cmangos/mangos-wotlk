ALTER TABLE db_version CHANGE COLUMN required_13988_01_mangos_seal_of_righteousness_final required_13989_01_mangos_string bit;

-- Isle of conquest
DELETE FROM mangos_string WHERE entry BETWEEN 647 AND 649;
DELETE FROM mangos_string WHERE entry BETWEEN 688 AND 699;
DELETE FROM mangos_string WHERE entry BETWEEN 754 AND 758;
DELETE FROM mangos_string WHERE entry=783;
INSERT INTO mangos_string VALUES
(647,'The battle will begin in 2 minutes.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(648,'The battle will begin in 1 minute.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(649,'The battle will begin in 30 seconds.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(688,'The battle will begin in 15 seconds.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(689,'The battle has begun!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(690,'The west gate of the Alliance keep has been destroyed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(691,'The east gate of the Alliance keep has been destroyed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(692,'The front gate of the Alliance keep has been destroyed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(693,'The west gate of the Horde keep has been destroyed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(694,'The east gate of the Horde keep has been destroyed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(695,'The front gate of the Horde keep has been destroyed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(696,'alliance keep',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(697,'horde keep',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(698,'workshop',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(699,'docks',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(754,'oil refinery',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(755,'quarry',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(756,'hangar',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(757,'$n has assaulted the %s!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(758,'$n has defended the %s!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(783,'The %s has taken the %s!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
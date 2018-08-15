REVOKE ALL PRIVILEGES ON * . * FROM 'mangos'@'localhost';

REVOKE ALL PRIVILEGES ON `wotlkmangos` . * FROM 'mangos'@'localhost';

REVOKE GRANT OPTION ON `wotlkmangos` . * FROM 'mangos'@'localhost';

REVOKE ALL PRIVILEGES ON `wotlkcharacters` . * FROM 'mangos'@'localhost';

REVOKE GRANT OPTION ON `wotlkcharacters` . * FROM 'mangos'@'localhost';

REVOKE ALL PRIVILEGES ON `wotlkrealmd` . * FROM 'mangos'@'localhost';

REVOKE GRANT OPTION ON `wotlkrealmd` . * FROM 'mangos'@'localhost';

DELETE FROM `user` WHERE CONVERT( User USING utf8 ) = CONVERT( 'mangos' USING utf8 ) AND CONVERT( Host USING utf8 ) = CONVERT( 'localhost' USING utf8 ) ;

DROP DATABASE IF EXISTS `wotlkmangos` ;

DROP DATABASE IF EXISTS `wotlkcharacters` ;

DROP DATABASE IF EXISTS `wotlkrealmd` ;

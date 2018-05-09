REVOKE ALL PRIVILEGES ON * . * FROM 'mangos'@'localhost';

REVOKE ALL PRIVILEGES ON `mangos` . * FROM 'mangos'@'localhost';

REVOKE ALL PRIVILEGES ON `characters` . * FROM 'mangos'@'localhost';

REVOKE ALL PRIVILEGES ON `realmd` . * FROM 'mangos'@'localhost';

DELETE FROM `mysql`.`user` WHERE CONVERT( User USING utf8 ) = CONVERT( 'mangos' USING utf8 ) AND CONVERT( Host USING utf8 ) = CONVERT( 'localhost' USING utf8 ) ;

DROP DATABASE IF EXISTS `mangos` ;

DROP DATABASE IF EXISTS `characters` ;

DROP DATABASE IF EXISTS `realmd` ;

DROP USER IF EXISTS `mangos`;

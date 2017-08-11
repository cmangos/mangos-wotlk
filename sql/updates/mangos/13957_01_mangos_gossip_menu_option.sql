ALTER TABLE db_version CHANGE COLUMN required_13956_01_mangos_spell_chain_totems_typos required_13957_01_mangos_gossip_menu_option bit;

DELETE FROM gossip_menu_option WHERE menu_id=0 AND id=16;
INSERT INTO `gossip_menu_option` VALUES('0','16','0','GOSSIP_OPTION_BOT','99','1','0','0','0','0','0',NULL,'0');

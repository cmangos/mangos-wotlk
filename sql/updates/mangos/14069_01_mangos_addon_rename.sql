ALTER TABLE db_version CHANGE COLUMN required_14068_01_mangos_gameobject_data_split required_14069_01_mangos_addon_rename bit;

ALTER TABLE creature_template_addon CHANGE b2_0_sheath sheath_state tinyint(3) unsigned NOT NULL DEFAULT '0';
ALTER TABLE creature_addon CHANGE b2_0_sheath sheath_state tinyint(3) unsigned NOT NULL DEFAULT '0';
ALTER TABLE creature_template_addon CHANGE b2_1_pvp_state pvp_flags tinyint(3) unsigned NOT NULL DEFAULT '0';
ALTER TABLE creature_addon CHANGE b2_1_pvp_state pvp_flags tinyint(3) unsigned NOT NULL DEFAULT '0';



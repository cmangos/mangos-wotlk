ALTER TABLE db_version CHANGE COLUMN required_s2397_01_mangos_broadcast_text bit required_s2398_01_mangos_world_safe_locs_facing bit;

CREATE TABLE IF NOT EXISTS `world_safe_locs_facing` (
  `id` int(10) unsigned NOT NULL,
  `orientation` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

ALTER TABLE `battleground_template`
	DROP COLUMN `AllianceStartO`,
	DROP COLUMN `HordeStartO`;

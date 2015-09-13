ALTER TABLE `pvpstats_players`
  CHANGE COLUMN `player_guid` `character_guid` INT(10) UNSIGNED NOT NULL AFTER `battleground_id`;

ALTER TABLE playerbotai_db_version CHANGE COLUMN required_5_playerbotai_auto_follow required_6_playerbotai_combatorders bit;

ALTER TABLE `playerbot_saved_data` DROP COLUMN `bot_secondary_order`;
ALTER TABLE playerbot_saved_data CHANGE COLUMN `bot_primary_order` `combat_order` int(11);

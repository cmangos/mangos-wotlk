ALTER TABLE db_version CHANGE COLUMN required_12951_01_mangos_item_template required_12952_01_mangos_instance_template bit;

ALTER TABLE instance_template
ADD COLUMN mountAllowed tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER ScriptName;

UPDATE instance_template SET mountAllowed = 1 WHERE map IN
(
    36, 209, 269, 309, 509, 534, 560, 564, 568, 578, 580, 595, 603, 615, 616, 631, 658, 724
);

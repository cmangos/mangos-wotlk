-- quest_template.RewMailTemplateId should be removed

-- -- The columns RewMailTemplateId and RewMailDelaySecs of table quest_template should be dropped.
-- -- quest_template.quest_end_script with SCRIPT_COMMAND_SEND_MAIL can handle this behaviour
-- -- Output limited to 20 lines

-- Related commit [12843] as of Feb. 2015
SELECT * FROM quest_template WHERE RewMailTemplateId != 0 LIMIT 20;

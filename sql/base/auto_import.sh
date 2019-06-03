USERNAME=""
read -e -p "MySQL User: " -i "root" USERNAME
PASSWORD=""
read -s -p "MySQL Password: " PASSWORD
export MYSQL_PWD="$PASSWORD"
echo ""
echo ""
read -e -p "mangos database name?: " -i "wotlkmangos" DB
MYSQL_COMMAND="mysql -u$USERNAME $DB"
$MYSQL_COMMAND < "mangos.sql"
echo 'applying original_data...'
cat dbc/original_data/*.sql | $MYSQL_COMMAND
echo 'applying cmangos_fixes...'
cat dbc/cmangos_fixes/*.sql | $MYSQL_COMMAND
echo "Completed!"
echo ""
read -e -p "characters database name?: " -i "wotlkcharacters" DB
MYSQL_COMMAND="mysql -u$USERNAME $DB"
$MYSQL_COMMAND < "characters.sql"
echo "Completed!"
echo ""
read -e -p "realmd database name?: " -i "wotlkrealmd" DB
MYSQL_COMMAND="mysql -u$USERNAME $DB"
$MYSQL_COMMAND < "realmd.sql"
echo "Completed!"

#!/bin/bash

MYSQL_HOST="localhost"
MYSQL_USER="root"
MYSQL_PASS="shit"
MYSQL_DB="trade_history"

for i in `seq 0 99`
do
    echo "create table user_deal_history_$i"
    mysql -h$MYSQL_HOST -u$MYSQL_USER -p$MYSQL_PASS $MYSQL_DB -e "CREATE TABLE user_deal_history_$i LIKE user_deal_history_example;"
done

#for i in `seq 0 99`
#do
#    echo "update table deal_history_$i"
#    mysql -h$MYSQL_HOST -u$MYSQL_USER -p$MYSQL_PASS $MYSQL_DB -e "ALTER TABLE deal_history_$i ADD COLUMN user_id INT UNSIGNED NOT NULL DEFAULT 0 AFTER time;"
#done

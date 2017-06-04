#!/bin/bash

MYSQL_HOST="localhost"
MYSQL_USER="root"
MYSQL_PASS="shit"
MYSQL_DB="trade_history"

for i in `seq 0 99`
do
    echo "update table order_history_$i"
    mysql -h$MYSQL_HOST -u$MYSQL_USER -p$MYSQL_PASS $MYSQL_DB -e "ALTER TABLE order_history_$i CHANGE deal_fee deal_fee DECIMAL(30,16) NOT NULL;"
done

for i in `seq 0 99`
do
    echo "update table order_detail_$i"
    mysql -h$MYSQL_HOST -u$MYSQL_USER -p$MYSQL_PASS $MYSQL_DB -e "ALTER TABLE order_detail_$i CHANGE deal_fee deal_fee DECIMAL(30,16) NOT NULL;"
done

for i in `seq 0 99`
do
    echo "update table deal_history_$i"
    mysql -h$MYSQL_HOST -u$MYSQL_USER -p$MYSQL_PASS $MYSQL_DB -e "ALTER TABLE deal_history_$i ADD COLUMN deal_id BIGINT NOT NULL DEFAULT 0 AFTER time;"
    mysql -h$MYSQL_HOST -u$MYSQL_USER -p$MYSQL_PASS $MYSQL_DB -e "ALTER TABLE deal_history_$i CHANGE fee fee DECIMAL(30,16) NOT NULL;"
    mysql -h$MYSQL_HOST -u$MYSQL_USER -p$MYSQL_PASS $MYSQL_DB -e "ALTER TABLE deal_history_$i ADD COLUMN deal_fee DECIMAL(30,16) NOT NULL DEFAULT 0 AFTER fee;"
done

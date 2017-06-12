#!/bin/bash

MYSQL_HOST="localhost"
MYSQL_USER="root"
MYSQL_PASS="shit"
MYSQL_DB="trade_history"

for i in `seq 0 99`
do
    echo "update table balance_history_$i"
    mysql -h$MYSQL_HOST -u$MYSQL_USER -p$MYSQL_PASS $MYSQL_DB -e "ALTER TABLE balance_history_$i CHANGE asset asset VARCHAR(30) NOT NULL;"
done

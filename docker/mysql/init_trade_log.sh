#!/bin/sh

MYSQL_HOST="localhost"
MYSQL_USER="user"
MYSQL_PASS="pass"
MYSQL_DB="trade_log"

for i in `seq 0 99`
do
    echo "create table slice_balance_example$i"
    mysql -h$MYSQL_HOST -u$MYSQL_USER -p$MYSQL_PASS $MYSQL_DB -e "CREATE TABLE slice_balance_$i LIKE slice_balance_example;"
done

for i in `seq 0 99`
do
    echo "create table slice_order_$i"
    mysql -h$MYSQL_HOST -u$MYSQL_USER -p$MYSQL_PASS $MYSQL_DB -e "CREATE TABLE slice_order_$i LIKE slice_order_example;"
done

for i in `seq 0 99`
do
    echo "create table operlog_$i"
    mysql -h$MYSQL_HOST -u$MYSQL_USER -p$MYSQL_PASS $MYSQL_DB -e "CREATE TABLE operlog_$i LIKE operlog_example;"
done


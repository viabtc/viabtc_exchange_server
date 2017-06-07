#!/bin/bash

MYSQL_HOST="localhost"
MYSQL_USER="root"
MYSQL_PASS="shit"
MYSQL_DB="trade_history"

for i in `seq 0 99`
do
    echo "update table deal_history_$i"
    mysql -h$MYSQL_HOST -u$MYSQL_USER -p$MYSQL_PASS $MYSQL_DB -e "ALTER TABLE deal_history_$i ADD COLUMN user_id INT UNSIGNED NOT NULL DEFAULT 0 AFTER time;"
done

CREATE_SQL='CREATE TABLE `user_deal_history_example` (
    `id`            BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    `time`          DOUBLE NOT NULL,
    `user_id`       INT UNSIGNED NOT NULL,
    `market`        VARCHAR(30) NOT NULL,
    `deal_id`       BIGINT UNSIGNED NOT NULL,
    `order_id`      BIGINT UNSIGNED NOT NULL,
    `deal_order_id` BIGINT UNSIGNED NOT NULL,
    `side`          TINYINT UNSIGNED NOT NULL,
    `role`          TINYINT UNSIGNED NOT NULL,
    `price`         DECIMAL(30,8) NOT NULL,
    `amount`        DECIMAL(30,8) NOT NULL,
    `deal`          DECIMAL(30,16) NOT NULL,
    `fee`           DECIMAL(30,16) NOT NULL,
    `deal_fee`      DECIMAL(30,16) NOT NULL,
    INDEX `idx_user_market` (`user_id`, `market`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;'

mysql -h$MYSQL_HOST -u$MYSQL_USER -p$MYSQL_PASS $MYSQL_DB -e "$CREATE_SQL"

for i in `seq 0 99`
do
    echo "create table user_deal_history_$i"
    mysql -h$MYSQL_HOST -u$MYSQL_USER -p$MYSQL_PASS $MYSQL_DB -e "CREATE TABLE user_deal_history_$i LIKE user_deal_history_example;"
done

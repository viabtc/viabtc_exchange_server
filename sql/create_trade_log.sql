CREATE TABLE `slice_balance_example` (
    `id`            INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    `user_id`       INT UNSIGNED NOT NULL,
    `asset`         VARCHAR(30) NOT NULL,
    `t`             TINYINT UNSIGNED NOT NULL,
    `balance`       DECIMAL(30,10) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `slice_order_example` (
    `id`            BIGINT UNSIGNED NOT NULL PRIMARY KEY,
    `t`             TINYINT UNSIGNED NOT NULL,
    `side`          TINYINT UNSIGNED NOT NULL,
    `create_time`   DOUBLE NOT NULL,
    `update_time`   DOUBLE NOT NULL,
    `user_id`       INT UNSIGNED NOT NULL,
    `market`        VARCHAR(30) NOT NULL,
    `price`         DECIMAL(30,10) NOT NULL,
    `amount`        DECIMAL(30,10) NOT NULL,
    `fee`           DECIMAL(30,10) NOT NULL,
    `left`          DECIMAL(30,10) NOT NULL,
    `freeze`        DECIMAL(30,10) NOT NULL,
    `deal_stock`    DECIMAL(30,10) NOT NULL,
    `deal_money`    DECIMAL(30,10) NOT NULL,
    `deal_fee`      DECIMAL(30,10) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `slice_history` (
    `id`            INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    `time`          BIGINT UNSIGNED NOT NULL,
    `end_date`      DATE NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `oper_log_example` (
    `id`            BIGINT UNSIGNED NOT NULL PRIMARY KEY,
    `t`             VARCHAR(30) NOT NULL,
    `detail`        TEXT
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

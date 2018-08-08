CREATE TABLE `balance_history_example` (
    `id`            BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    `time`          DOUBLE NOT NULL,
    `user_id`       INT UNSIGNED NOT NULL,
    `asset`         VARCHAR(30) NOT NULL,
    `business`      VARCHAR(30) NOT NULL,
    `change`        DECIMAL(30,8) NOT NULL,
    `balance`       DECIMAL(30,16) NOT NULL,
    `detail`        TEXT NOT NULL,
    INDEX `idx_user_asset` (`user_id`, `asset`),
    INDEX `idx_user_asset_business` (`user_id`, `asset`, `business`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- split by user_id
CREATE TABLE `order_history_example` (
    `id`            BIGINT UNSIGNED NOT NULL PRIMARY KEY,
    `create_time`   DOUBLE NOT NULL,
    `finish_time`   DOUBLE NOT NULL,
    `user_id`       INT UNSIGNED NOT NULL,
    `market`        VARCHAR(30) NOT NULL,
    `source`        VARCHAR(30) NOT NULL,
    `t`             TINYINT UNSIGNED NOT NULL,
    `side`          TINYINT UNSIGNED NOT NULL,
    `price`         DECIMAL(30,8) NOT NULL,
    `amount`        DECIMAL(30,8) NOT NULL,
    `taker_fee`     DECIMAL(30,4) NOT NULL,
    `maker_fee`     DECIMAL(30,4) NOT NULL,
    `deal_stock`    DECIMAL(30,8) NOT NULL,
    `deal_money`    DECIMAL(30,16) NOT NULL,
    `deal_fee`      DECIMAL(30,16) NOT NULL,
    INDEX `idx_user_market` (`user_id`, `market`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- split by id, aka orer_id
CREATE TABLE `order_detail_example` (
    `id`            BIGINT UNSIGNED NOT NULL PRIMARY KEY,
    `create_time`   DOUBLE NOT NULL,
    `finish_time`   DOUBLE NOT NULL,
    `user_id`       INT UNSIGNED NOT NULL,
    `market`        VARCHAR(30) NOT NULL,
    `source`        VARCHAR(30) NOT NULL,
    `t`             TINYINT UNSIGNED NOT NULL,
    `side`          TINYINT UNSIGNED NOT NULL,
    `price`         DECIMAL(30,8) NOT NULL,
    `amount`        DECIMAL(30,8) NOT NULL,
    `taker_fee`     DECIMAL(30,4) NOT NULL,
    `maker_fee`     DECIMAL(30,4) NOT NULL,
    `deal_stock`    DECIMAL(30,8) NOT NULL,
    `deal_money`    DECIMAL(30,16) NOT NULL,
    `deal_fee`      DECIMAL(30,16) NOT NULL,
    INDEX `idx_user_market` (`user_id`, `market`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- split by order_id
CREATE TABLE `deal_history_example` (
    `id`            BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    `time`          DOUBLE NOT NULL,
    `user_id`       INT UNSIGNED NOT NULL,
    `deal_id`       BIGINT UNSIGNED NOT NULL,
    `order_id`      BIGINT UNSIGNED NOT NULL,
    `deal_order_id` BIGINT UNSIGNED NOT NULL,
    `role`          TINYINT UNSIGNED NOT NULL,
    `price`         DECIMAL(30,8) NOT NULL,
    `amount`        DECIMAL(30,8) NOT NULL,
    `deal`          DECIMAL(30,16) NOT NULL,
    `fee`           DECIMAL(30,16) NOT NULL,
    `deal_fee`      DECIMAL(30,16) NOT NULL,
    INDEX `idx_order_id` (`order_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- split by user_id
CREATE TABLE `user_deal_history_example` (
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
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

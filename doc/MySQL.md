#MySQL

## Terminology
The terms to be used during the tables description without further explanations.
* `Exchange` - (usually "the Exchange") the set of the software providing assets exchange functionality and
use the databases to perstist the interanal state.
* `Asset` - in the scope of the Exchange the asset means nothing more than a name of the crypto currency.
* `Money` - an asset an user usually "owns" (or borrow) and may "sell" (or put "ask" limit order) in exchange of the
another asset called "stock".
* `Stock` - in the simplest case, an asset to be suggested by the market for the exchange to "money". Stock
is the currency an user may "buy" (or put "bid" limit order) in exchange of the "money" he owns.
* `Market` - a 6 or 7 characters long string concatenation of "Stock" and "Money".
* `TBI` - "to be investigated" - there's no knowledge yet
* `TBC` - "to be confirmed" - there's an assumtion not confirmed yet
* `user_id` - an ID of exchange user sometimes referred as "exchange id" meaning "an id of the exchange user".

## Databases

Excahnge works with 2 separate independed database which can and should run separately as
different instances run on different hosts. 
The one reason to have them separated is to protect each other from exhausting disk space.

Usual maintenance tasks are: to clean (possible backup) "operlog_yyyymmdd" tables and binary logs if
they are turned on.

## Maintenance

The *simnet* script to clean up all operlog tables besides the most recent (active) one:
```
#bin/sh
MYSQL="mysql -uUSER -pPWD -h127.0.0.1  -P3316"
echo 'show tables' | $MYSQL trade_log|grep operlog|grep -v example |head -n -1|while read f;do echo "drop table $f;";done|$MYSQL trade_log
```

The *simnet* script to clean up binary logs (if they are turned on) older than 3 days
```
#bin/sh
MYSQL="mysql -uexchange -pexchange -h127.0.0.1  -P3316"
echo 'PURGE BINARY LOGS BEFORE DATE(NOW() - INTERVAL 3 DAY) + INTERVAL 0 SECOND;' | $MYSQL
```

### Database trade_history

This database is created with five empty tables: `balance_history_example`, `order_history_example`,
`order_detail_example`, `deal_history_example`, `user_deal_history_example`. The original .sql
scripts to create these tables reside in `sql` directory and theier maintained copies used to build the
mysql docker in the `docker/mysql` directory.

Before the very first launch of the exchange each of 5 tables must be cloned 100 other tables with with
the `_example` part replace to `_NN` where NN are numbers from _0_ to _99_. Further the data will be stripped
over the tables according to record's id reminder of the division by 100.

#### Table balance_history_NN

This table has a history of  user's balances. User's ID is a key to be divided by 100 and the remainder
to use for selection one of 100 tables. For ex. balances of an user with ID *1121516550* will be put in
`balance_history_50` table.

It has a some note about the reason why the balance has got its value but generally does not contain
the exact information about the reason causes balance to be changed

Most important fields:

An ID of balance owner
```
  `user_id` int(10) unsigned NOT NULL,
```

A name of asset to keep the balance of. Something like "BTC", "ETC", etc.
```
  `asset` varchar(30) NOT NULL,
```

A vague info about the reason of the balance change. The usual values are "deposit", "withdraw" and "trade", but
others are possible.
```
  `business` varchar(30) NOT NULL,
```

The difference between the recorderd balance amount and the previous one
```
  `change` decimal(30,8) NOT NULL,
```

The last recorder at the time balance
```
  `balance` decimal(30,16) NOT NULL,
```

An arbitrary detail info passed alongside with the balance change operation request

```
  `detail` text NOT NULL,
```

#### Table order_history_NN

A time the order has been opened and either completed or canceled
```
  `create_time` double NOT NULL,
  `finish_time` double NOT NULL,
```

An id of the user who created the order

```
  `user_id` int(10) unsigned NOT NULL,
```

A "market" (concatenation of assests to be exchnaged) name
```
  `market` varchar(30) NOT NULL,
```
An arbitrary string used during the creation of the order. Typically contain a 
name of the software used.
```
  `source` varchar(30) NOT NULL,
```
TBI:
```
  `t` tinyint(3) unsigned NOT NULL,
```
1 - for "buy" and 2 for "sell" orders
```
  `side` tinyint(3) unsigned NOT NULL,
```

TBC: Price is not 0 for closed or put by limit orders. TBC: price is expressed in "money" currency
```
  `price` decimal(30,8) NOT NULL,
```

The total amount of the assets (i.e. price * quantity) used for the order
```
  `amount` decimal(30,8) NOT NULL,
```

The fees
```
  `taker_fee` decimal(30,4) NOT NULL,
  `maker_fee` decimal(30,4) NOT NULL,
```

TBC: an amounts of the deal is the amount of the order minus the fee(s) expressed in stock amd money 
currency
```
  `deal_stock` decimal(30,8) NOT NULL,
  `deal_money` decimal(30,16) NOT NULL,
```
TBC: the total fee of taker's and maker's fees
```
  `deal_fee` decimal(30,16) NOT NULL,
```

#### Table order_detail_NN

The structure of this table is absolutely identical to the structure of `order_history_NN` but the content
is only subset of `order_history_NN` and contains some pretty small amount of data. 
TBI: why the orders put in this table?

### Table deal_history_NN
The columnst of this table is the subset of the columns of user_deal_history_NN
The data of this table is the subset of the data of user_deal_history_NN

```
  `time` double NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `deal_id` bigint(20) unsigned NOT NULL,
  `order_id` bigint(20) unsigned NOT NULL,
  `deal_order_id` bigint(20) unsigned NOT NULL,
  `role` tinyint(3) unsigned NOT NULL,
  `price` decimal(30,8) NOT NULL,
  `amount` decimal(30,8) NOT NULL,
  `deal` decimal(30,16) NOT NULL,
  `fee` decimal(30,16) NOT NULL,
  `deal_fee` decimal(30,16) NOT NULL,
```
TBI: why the orders put in this table?

### Table user_deal_history_NN
These  fields are common with  deal_history_NN table
```
  `time` double NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `deal_id` bigint(20) unsigned NOT NULL,
  `order_id` bigint(20) unsigned NOT NULL,
  `deal_order_id` bigint(20) unsigned NOT NULL,
  `role` tinyint(3) unsigned NOT NULL,
  `price` decimal(30,8) NOT NULL,
  `amount` decimal(30,8) NOT NULL,
  `deal` decimal(30,16) NOT NULL,
  `fee` decimal(30,16) NOT NULL,
  `deal_fee` decimal(30,16) NOT NULL,
```
```
  `market` varchar(30) NOT NULL,
  `side` tinyint(3) unsigned NOT NULL,
```
TBI: why the orders put in this table?
### Database trade_log

This database is created with four empty tables: `operlog_example`, `slice_balance_example`,
`slice_history`, `slice_order_example`. The original .sql
scripts to create these tables reside in `sql` directory and theier maintained copies used to build the
mysql docker in the `docker/mysql` directory.

Unlike the tables in `trade_history` database are not stripped in any way this the reason
why *_example tables are not cloned before the first run. Instead they cloned by the Exchange itself
on demand - usually daily or hourly.

From this naming it is supposed the database is "write only" in mean of the Exchange never reads rows 
from it and its purpose is to "slice" the data contained in "trade_history" database for sake of
possible data maining tasks.  

#### Table slice_history
This is the only table in the database that has a fixed name and it maps timestamps used for table names of the
other tables to the IDs of the operations, orders and deals and vice versa.

```
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `time` bigint(20) NOT NULL,
  `end_oper_id` bigint(20) unsigned NOT NULL,
  `end_order_id` bigint(20) unsigned NOT NULL,
  `end_deals_id` bigint(20) unsigned NOT NULL,
```


#### Table operlog_yyyymmdd

This table is nothing more than a log of all json-rpc calls run against `matchengine`. The column "details" 
contains serialized json-rpc request in the raw form like `{"method": "limit_order", "params": [1121516550, "BTCDASH", 1, "0.01315719", "0.06258553", "0.0", "0.0", "depthflooder"]}`

```
  `id` bigint(20) unsigned NOT NULL,
  `time` double NOT NULL,
  `detail` text,
```

Because of the tables of this kind contains all json-rpc interacton they are most growing tables in the both databases. 
It is write-only and are not used by the Exchnage (TBC) thus the all tables but the last one may be deleted without
the harm.

#### Table slice_balance_NNNNNNNNNN

NNNNNNNNNN stands for unix time in seconds and is updated by each hour.

Keeps the current (most recent) balances of the unique triplet {user_ud, asset, t} where the
role of `t` still is unknown (TBI)

```
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` int(10) unsigned NOT NULL,
  `asset` varchar(30) NOT NULL,
  `t` tinyint(3) unsigned NOT NULL,
  `balance` decimal(30,16) NOT NULL,
```

#### Table slice_order_NNNNNNNNNN
NNNNNNNNNN stands for unix time in seconds and is updated by each hour.

*The interisting fact* about this table is it is the only that contains `freeze` column. 

TBI `t` is always 1 in this table
```
  `t` tinyint(3) unsigned NOT NULL,
```

```
  `side` tinyint(3) unsigned NOT NULL,
  `create_time` double NOT NULL,
  `update_time` double NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `market` varchar(30) NOT NULL,
  `price` decimal(30,8) NOT NULL,
  `amount` decimal(30,8) NOT NULL,
  `taker_fee` decimal(30,4) NOT NULL,
  `maker_fee` decimal(30,4) NOT NULL,
  `left` decimal(30,8) NOT NULL,
  `freeze` decimal(30,8) NOT NULL,
  `deal_stock` decimal(30,8) NOT NULL,
  `deal_money` decimal(30,16) NOT NULL,
  `deal_fee` decimal(30,12) NOT NULL,
```

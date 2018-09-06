#!/bin/bash

#buy 2 bitcoins at 8000 CNY
./cli 127.0.0.1 7316 201 '[1, "BTCBCH", 2, "3", "8000", "0.002", "0.001","api.v1"]'

#query my pending order list
./cli 127.0.0.1 7316 203 '[1, "BTCBCH", 0,10]'

#sell 1 bitcoins at 8010 CNY
./cli 127.0.0.1 7316 201 '[1, "BTCBCH", 1, "1", "8010", "0.002", "0.001","api.v1"]'

#query my pending order list
./cli 127.0.0.1 7316 203 '[1, "BTCBCH", 0,10]'


#query all pending list on sell direction
./cli 127.0.0.1 7316 205 '[ "BTCBCH", 1, 0,10]'

#query all pending list on buy direction
./cli 127.0.0.1 7316 205 '[ "BTCBCH", 2, 0,10]'


#query market depth
./cli 127.0.0.1 7316 206 '[ "BTCBCH",10,"0"]'

#query pending order detail
./cli 127.0.0.1 7316 207 '[ "BTCBCH",5]'


#./cli 127.0.0.1 7316 201 '[2, "BTCBCH", 2, "1",  "7000", "0.002", "0.001"]'

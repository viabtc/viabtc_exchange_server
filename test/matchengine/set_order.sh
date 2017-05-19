#!/bin/bash

#buy 2 bitcoins at 8000 CNY
./cli.exe 127.0.0.1 7316 201 '[1, "BTCCNY", 2, "3", "8000", "0.002", "0.001","api.v1"]'

#query my pending order list
./cli.exe 127.0.0.1 7316 203 '[1, "BTCCNY", 0,10]'

#sell 1 bitcoins at 8010 CNY
./cli.exe 127.0.0.1 7316 201 '[1, "BTCCNY", 1, "1", "8010", "0.002", "0.001","api.v1"]'

#query my pending order list
./cli.exe 127.0.0.1 7316 203 '[1, "BTCCNY", 0,10]'


#query all pending list on sell direction
./cli.exe 127.0.0.1 7316 205 '[ "BTCCNY", 1, 0,10]'

#query all pending list on buy direction
./cli.exe 127.0.0.1 7316 205 '[ "BTCCNY", 2, 0,10]'


#query market depth
./cli.exe 127.0.0.1 7316 206 '[ "BTCCNY",10,0]'

#query pending order detail
./cli.exe 127.0.0.1 7316 207 '[ "BTCCNY",order_id]'


#./cli.exe 127.0.0.1 7316 201 '[2, "BTCCNY", 2, "1",  "7000", "0.002", "0.001"]'

#!/bin/bash

#buy 3 bitcoins at 8000 CNY
#./cli 127.0.0.1 7316 201 '[1, "BTCCNY", 2, "3", "8000", "0.002", "0.001","api.v1"]'

#query my pending order list
#./cli 127.0.0.1 7316 203 '[1, "BTCCNY", 0,10]'

#sell 3 bitcoins at 8010 CNY
./cli 127.0.0.1 7316 201 '[2, "BTCCNY", 1, "3", "7999", "0.002", "0.001","api.v1"]'


#query all pending list on sell direction
#./cli 127.0.0.1 7316 205 '[ "BTCCNY", 1, 0,10]'

#query all pending list on buy direction
#./cli 127.0.0.1 7316 205 '[ "BTCCNY", 2, 0,10]'


#query market depth
#./cli 127.0.0.1 7316 206 '[ "BTCCNY",10,0]'

#query pending order detail
#./cli 127.0.0.1 7316 207 '[ "BTCCNY",order_id]'



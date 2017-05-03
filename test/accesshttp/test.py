#!/usr/bin/python
# -*- coding: utf-8 -*-

import json
import time
import requests

def call(method, *params):
    data = {'method': method, 'params': params, 'id': int(time.time() * 1000)}
    r = requests.post('http://127.0.0.1:8080/', data=json.dumps(data))
    print r.text

#call("balance.update", 2, "CNY", "deposit", 100, "10000")
#call("balance.query", 1)
#call("balance.query", 2)
#call("balance.history", 1, "BTC", "", 0, 0, 0, 10)
#call("trade.finished_order", 1, "BTCCNY", 0, 0, 0, 10)
#call("trade.order_deals", "BTCCNY", 10, 0)
#call("trade.order_book", "BTCCNY", 1, 0, 100)
#call("trade.order_book", "BTCCNY", 2, 0, 100)
#call("trade.put_limit", 2, "BTCCNY", 2, "1", "7000", "0.001", "0.001")
#call("trade.put_limit", 1, "BTCCNY", 1, "1", "7000", "0.001", "0.001")
#call("trade.order_book", "BTCCNY", 1, 0, 100)
#call("trade.order_book", "BTCCNY", 2, 0, 100)
#call("trade.finished_order_detail", 11)
#call("trade.finished_order_detail", 12)
call("market.deals", "BTCCNY", 10, 5)

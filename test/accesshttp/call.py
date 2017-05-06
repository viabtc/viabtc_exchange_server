#!/usr/bin/python
# -*- coding: utf-8 -*-

import json
import time
import requests

def call(method, *params):
    data = {'method': method, 'params': params, 'id': int(time.time() * 1000)}
    r = requests.post('http://127.0.0.1:8080/', data=json.dumps(data))
    print r.text


#!/usr/bin/python
# -*- coding: utf-8 -*-

import json
import requests

data = {"country_code": "86", "mobile": "13554818887", "login_password": "123456"}
headers = {"content-type": "application/json"}
r = requests.post("http://120.77.243.61/exchange/v1/user/sign/in/mobile", data=json.dumps(data), headers=headers);
print r.text


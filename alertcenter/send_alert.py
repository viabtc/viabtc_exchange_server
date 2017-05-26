#!/usr/bin/python
# -*- coding: utf-8 -*-

import time
import boto3
from redis.sentinel import Sentinel

ALERT_EMAILS = ['yang@haipo.me']
REDIS_SENTINEL_LIST = [("192.168.0.62", 26379), ("192.168.0.63", 26379), ("192.168.0.64", 26379)]

def send_email(to_address, subject, content):
    ses = boto3.client('ses')
    r = ses.send_email(Source = '"viabtc-exchange-alert" <alert@mail.viabtc.cn>',
            Destination = {'ToAddresses': [to_address]},
            Message = {'Subject': { 'Data': subject, 'Charset': 'utf-8'},
                'Body': {'Text': {'Data': content, 'Charset': 'utf-8'}}})

def main():
    last_send = None
    sentinel = Sentinel(REDIS_SENTINEL_LIST)
    redis_master = sentinel.master_for("mymaster", socket_timeout=120)
    while True:
        r = redis_master.blpop('alert:message', 60)
        if not r:
            continue

        current_timestamp = int(time.time())
        if last_send and current_timestamp - last_send < 60:
            continue
        last_send = current_timestamp
        message = r[1]
        for email in ALERT_EMAILS:
            send_email(email, "viabtc server error", message)

if __name__ == '__main__':
    main()


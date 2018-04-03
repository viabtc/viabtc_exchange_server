#!/bin/bash

V="-v/tmp/bitlum/exchange/kafka/logs:/var/log/kafka"

docker stop exchange_kafka
docker rm exchange_kafka
if [ -z $SH ];then
docker run --name=exchange_kafka -it $V $@ bitlum/exchange-kafka
else
docker run --name=exchange_kafka -it --entrypoint=/bin/sh $V $@ bitlum/exchange-kafka
fi

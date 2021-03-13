#!/bin/bash

V="-v/tmp/bitlum/exchange/mysql:/var/lib/mysql"

docker stop exchange_mysql
docker rm exchange_mysql
if [ -z $SH ];then
docker run --name=exchange_mysql $V $@ bitlum/exchange-mysql
else
docker run --name=exchange_mysql -it --entrypoint=/bin/sh $V $@ bitlum/exchange-mysql
fi

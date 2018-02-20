#!/bin/bash

if [ $# -lt 1 ]; then
	N=1
else
	N=$1
	shift
fi

V="-v/tmp/bitlum/exchange/redis/$N/lib:/var/lib/redis"

docker stop exchange_redis_$N
docker rm exchange_redis_$N
if [ -z $SH ];then
docker run --name=exchange_redis_$N -it $V $@ bitlum/exchange-redis-$N
else
docker run --name=exchange_redis_$N -it --entrypoint=/bin/sh $V $@ bitlum/exchange-redis-$N
fi

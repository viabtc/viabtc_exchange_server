#!/bin/sh

sed -i.bak "s/^port .*$/port ${PORT}/" /etc/redis/sentinel.conf
sed -i.bak "s/^\(sentinel monitor [^ ]*\).*$/\1 ${REDIS_HOST} ${REDIS_PORT} ${QUORUM}/" /etc/redis/sentinel.conf
rm /etc/redis/sentinel.conf.bak
chown redis:redis /etc/redis/sentinel.conf
cat /etc/redis/sentinel.conf
echo $@
exec $@

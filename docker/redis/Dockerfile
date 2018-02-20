FROM alpine:3.7

RUN addgroup -S redis && adduser -S -G redis redis
RUN apk add --no-cache redis

ARG PORT=6379
COPY redis.conf /etc/redis/redis.conf
COPY entrypoint.sh /docker-entrypoint.sh
RUN	sed -i.bak "s/^port .*$/port ${PORT}/" /etc/redis/redis.conf && rm /etc/redis/redis.conf.bak
RUN chown redis:redis /etc/redis/redis.conf


EXPOSE ${PORT}

ENTRYPOINT ["/docker-entrypoint.sh"]
CMD ["redis-server", "/etc/redis/redis.conf"]

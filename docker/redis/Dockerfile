FROM alpine:3.7

RUN addgroup -S redis && adduser -S -G redis redis
RUN apk add --no-cache redis

COPY redis.conf /etc/redis/redis.conf

ENV PORT=6379
ENV SLAVEOF=

CMD ["redis-server", "/etc/redis/redis.conf"]

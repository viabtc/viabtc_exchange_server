FROM alpine:3.7

RUN addgroup -S redis && adduser -S -G redis redis
RUN apk add --no-cache redis

COPY sentinel.conf /etc/redis/sentinel.conf
COPY entrypoint.sh /docker-entrypoint.sh

ENV PORT=26379
ENV REDIS_PORT=6379
ENV REDIS_HOST=redis
ENV QUORUM=2

ENTRYPOINT ["/docker-entrypoint.sh"]
CMD ["redis-server", "/etc/redis/sentinel.conf", "--sentinel"]

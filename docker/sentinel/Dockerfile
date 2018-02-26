FROM alpine:3.7

RUN addgroup -S redis && adduser -S -G redis redis
RUN apk add --no-cache redis


COPY sentinel.conf /etc/redis/sentinel.conf

ENV PORT=26379
ENV REDIS_PORT=6379
ENV REDIS_HOST=redis

CMD ["redis-server", "/etc/redis/sentinel.conf", "--sentinel"]

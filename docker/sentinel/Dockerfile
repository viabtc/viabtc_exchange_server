FROM alpine:3.7

RUN addgroup -S redis && adduser -S -G redis redis
RUN apk add --no-cache redis

ARG PORT=26379
ARG REDIS_PORT=6379
ARG REDIS_HOST=redis
ARG REDIS_QUORUM=1

COPY sentinel.conf /etc/redis/sentinel.conf

RUN	sed -i.bak "s/^port .*$/port ${PORT}/" /etc/redis/sentinel.conf && \
    cat /etc/redis/sentinel.conf && \
  	sed -i.bak "s/^\(sentinel monitor [^ ]*\).*$/\1 ${REDIS_HOST} ${REDIS_PORT} ${REDIS_QUORUM}/" /etc/redis/sentinel.conf && \
    rm /etc/redis/sentinel.conf.bak && \
    cat /etc/redis/sentinel.conf
RUN chown redis:redis /etc/redis/sentinel.conf

EXPOSE ${PORT}

CMD ["redis-server", "/etc/redis/sentinel.conf", "--sentinel"]

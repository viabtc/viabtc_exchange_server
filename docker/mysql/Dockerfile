FROM alpine:3.7
RUN apk --no-cache add mariadb mariadb-client

VOLUME /var/lib/mysql
EXPOSE 3306

COPY my.cnf /etc/mysql/my.cnf
RUN chown -R mysql.mysql /etc/mysql

RUN mkdir /run/mysqld
RUN chown -R mysql.mysql /run/mysqld

COPY entrypoint.sh docker-entrypoint.sh

RUN mkdir /docker-entrypoint-initdb.d
COPY init_trade_history.sh /docker-entrypoint-initdb.d
COPY create_trade_history.sql /docker-entrypoint-initdb.d
COPY create_trade_log.sql /docker-entrypoint-initdb.d

ENTRYPOINT ["/docker-entrypoint.sh"]
CMD ["mysqld"]

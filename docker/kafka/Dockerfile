FROM alpine:3.7

# Default to UTF-8 file.encoding
ENV LANG C.UTF-8

# add a simple script that can auto-detect the appropriate JAVA_HOME value
# based on whether the JDK or only the JRE is installed
RUN { \
		echo '#!/bin/sh'; \
		echo 'set -e'; \
		echo; \
		echo 'dirname "$(dirname "$(readlink -f "$(which javac || which java)")")"'; \
	} > /usr/local/bin/docker-java-home \
	&& chmod +x /usr/local/bin/docker-java-home
ENV JAVA_HOME /usr/lib/jvm/java-1.8-openjdk/jre
ENV PATH $PATH:/usr/lib/jvm/java-1.8-openjdk/jre/bin:/usr/lib/jvm/java-1.8-openjdk/bin

ENV JAVA_VERSION 8u151
ENV JAVA_ALPINE_VERSION 8.171.11-r0

RUN set -x \
	&& apk add --no-cache \
		openjdk8-jre="$JAVA_ALPINE_VERSION" \
	&& [ "$JAVA_HOME" = "$(docker-java-home)" ]

RUN apk --no-cache add wget

RUN mkdir -p /opt/kafka
RUN mkdir -p /etc/kafka
RUN mkdir -p /var/log/kafka
RUN mkdir -p /var/lib/kafka

RUN wget http://mirror.linux-ia64.org/apache/kafka/1.0.0/kafka_2.11-1.0.0.tgz && \
    tar -xvf kafka_2.11-1.0.0.tgz -C /opt/kafka/ && \
    rm kafka_2.11-1.0.0.tgz

COPY server.properties /etc/kafka/server.properties

RUN apk --no-cache add bash

EXPOSE 9092
CMD cd /opt/kafka/kafka_2.11-1.0.0 && \
   (nohup bin/zookeeper-server-start.sh config/zookeeper.properties &) && \
   sleep 3 && \
   bin/kafka-server-start.sh /etc/kafka/server.properties

FROM ubuntu:16.04 AS BUILDSTEP
LABEL BUILDSTEP NNNN
MAINTAINER Sergey Dolin <sergey@s4y.solutions>

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
      jq \
      netcat \
      inetutils-ping \
      vim \
      git \
      wget \
      make \
      cmake \
      gcc \
      g++ \
      python \
      libev-dev \
      libmpdec-dev \
      libjansson-dev \
      libssl-dev \
      libgnutls-dev \
      libmysqlclient-dev \
      libhttp-parser-dev \
      libcurl4-openssl-dev \
			libldap2-dev \
			libkrb5-dev \
			libalberta-dev  \
			libgss-dev \
			libidn11-dev \
			librtmp-dev &&\
    rm -rf /var/lib/apt/lists/* 

RUN git clone https://github.com/edenhill/librdkafka.git && cd librdkafka && ./configure && make && make install && cd / && rm -rf librdkafka

RUN mkdir viabtc_exchange_server
COPY depends viabtc_exchange_server/depends
COPY network viabtc_exchange_server/network
COPY utils viabtc_exchange_server/utils
COPY alertcenter viabtc_exchange_server/alertcenter
COPY matchengine viabtc_exchange_server/matchengine
COPY marketprice viabtc_exchange_server/marketprice
COPY readhistory viabtc_exchange_server/readhistory
COPY accesshttp viabtc_exchange_server/accesshttp
COPY accessws viabtc_exchange_server/accessws
COPY docker/exchange/makefile.inc.ubuntu viabtc_exchange_server/makefile.inc

RUN cd viabtc_exchange_server/depends/hiredis && make && mv libhiredis.* /usr/lib && cd .. && mv hiredis /usr/include 
RUN cd viabtc_exchange_server/network && make
RUN cd viabtc_exchange_server/utils && make
RUN cd viabtc_exchange_server/alertcenter && make
RUN cd viabtc_exchange_server/matchengine && make
COPY docker/exchange/makefile.marketprice.ubuntu viabtc_exchange_server/marketprice/makefile
RUN cd viabtc_exchange_server/marketprice && make
RUN cd viabtc_exchange_server/readhistory && make 
RUN cd viabtc_exchange_server/accesshttp && make
COPY docker/exchange/makefile.accessws.ubuntu viabtc_exchange_server/accessws/makefile
RUN cd viabtc_exchange_server/accessws && make

FROM ubuntu:16.04

RUN apt-get update \
    && apt-get upgrade -y \
    && apt-get install -y \
		  libldap-2.4 \
      libgss3 \
      librtmp1 \
      libmysqlclient20 \
      jq \
      netcat \
    && rm -rf /var/lib/apt/lists/* 

RUN mkdir /viabtc_exchange_server
# keep .exe for better grep of ps output
COPY --from=BUILDSTEP /viabtc_exchange_server/alertcenter/alertcenter /viabtc_exchange_server/alertcenter.exe
COPY docker/exchange/alertcenter.json /viabtc_exchange_server

COPY --from=BUILDSTEP /viabtc_exchange_server/matchengine/matchengine /viabtc_exchange_server/matchengine.exe
COPY docker/exchange/matchengine.json /viabtc_exchange_server

COPY --from=BUILDSTEP /viabtc_exchange_server/marketprice/marketprice /viabtc_exchange_server/marketprice.exe
COPY docker/exchange/marketprice.json /viabtc_exchange_server

COPY --from=BUILDSTEP /viabtc_exchange_server/readhistory/readhistory /viabtc_exchange_server/readhistory.exe
COPY docker/exchange/readhistory.json /viabtc_exchange_server

COPY --from=BUILDSTEP /viabtc_exchange_server/accesshttp/accesshttp /viabtc_exchange_server/accesshttp.exe
COPY docker/exchange/accesshttp.json /viabtc_exchange_server

COPY --from=BUILDSTEP /viabtc_exchange_server/accessws/accessws /viabtc_exchange_server/accessws.exe
COPY docker/exchange/accessws.json /viabtc_exchange_server

COPY docker/exchange/entrypoint.sh /docker-entrypoint.sh

ENTRYPOINT [ "/docker-entrypoint.sh" ]

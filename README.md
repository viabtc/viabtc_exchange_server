# ViaBTC Exchange Server

ViaBTC Exchange Server is a trading backend with high-speed performance, designed for cryptocurrency exchanges. It can support up to 10000 trades every second and real-time user/market data notification through websocket.

## Architecture

![Architecture](https://user-images.githubusercontent.com/1209350/32476113-5ffc622a-c3b0-11e7-9755-924f17bcc167.jpeg)

For this project, it is marked as Server in this picture.

## Code structure

**Required systems**

* MySQL: For saving operation log, user balance history, order history and trade history.

* Redis: A redis sentinel group is for saving market data.

* Kafka: A message system.

**Base library**

* network: An event base and high performance network programming library, easily supporting [1000K TCP connections](http://www.kegel.com/c10k.html). Include TCP/UDP/UNIX SOCKET server and client implementation, a simple timer, state machine, thread pool. 

* utils: Some basic library, including log, config parse, some data structure and http/websocket/rpc server implementation.

**Modules**

* matchengine: This is the most important part for it records user balance and executes user order. It is in memory database, saves operation log in MySQL and redoes the operation log when start. It also writes user history into MySQL, push balance, orders and deals message to kafka.

* marketprice: Reads message(s) from kafka, and generates k line data.

* readhistory: Reads history data from MySQL.

* accesshttp: Supports a simple HTTP interface and hides complexity for upper layer.

* accwssws: A websocket server that supports query and pushes for user and market data. By the way, you need nginx in front to support wss.

* alertcenter: A simple server that writes FATAL level log to redis list so we can send alert emails.

## API Reference

[HTTP Protocol](https://github.com/Bringer-of-Light/viabtc_exchange_server/wiki/HTTP-Protocol) and [Websocket Protocol](https://github.com/Bringer-of-Light/viabtc_exchange_server/wiki/WebSocket-Protocol) documents are available in english.

[Python3 API realisation](https://github.com/grumpydevelop/viabtc_exchange_server_tools/blob/master/api/api_exchange.py)


## Websocket authorization

The websocket protocol has an authorization method (`server.auth`) which is used to authorize the websocket connection to subscribe to user specific events (trade and balance events).

To accomodate this method your exchange frontend will need to supply an internal endpoint which takes an authorization token from the HTTP header named `Authorization` and validates that token and returns the user_id.

The internal authorization endpoint is defined by the `auth_url` setting in the config file (`accessws/config.json`).

Example response: `{"code": 0, "message": null, "data": {"user_id": 1}}`


## Compile and Install

**Operating system**

Ubuntu 14.04 or Ubuntu 16.04. Not yet tested on other systems.

**Requirements**

You MUST use the depends/hiredis to install the hiredis library. Or it may not be compatible.

**Compilation**

Compile network and utils first. The rest all are independent.

**Deployment**

One single instance is given for matchengine, marketprice and alertcenter, while readhistory, accesshttp and accwssws can have multiple instances to work with loadbalancing.

Please do not install every instance on the same machine.

Every process runs in deamon and starts with a watchdog process. It will automatically restart within 1s when crashed.

(out of date)The best practice of deploying the instance is in the following directory structure:

```
matchengine
|---bin
|   |---matchengine.exe
|---log
|   |---matchengine.log
|---conf
|   |---config.json
|---shell
|   |---restart.sh
|   |---check_alive.sh
```

**Using Docker**
```
git clone 
cd viabtc_exchange_server/docker
docker-compose up [opitional: service name]
```
docker integration is merged from [ohld: testnet-exchange/viabtc_exchange_server](https://github.com/testnet-exchange/viabtc_exchange_server).


**Manual compile on Ubuntu 14.04.5X64**
```
#make a new folder and cd into it before you run these cmds

apt update 
apt install -y wget vim psmisc git
apt install -y libev-dev libmpdec-dev  libmysqlclient-dev libssl-dev
apt install -y build-essential autoconf libtool python 

# clear
rm -rf /var/lib/apt/lists/* 

#install jansson
git clone https://github.com/akheron/jansson
cd jansson
autoreconf -i
./configure
make
make install
cd ..

# install kafka
wget --no-check-certificate https://codeload.github.com/edenhill/librdkafka/tar.gz/v0.11.3 -O  librdkafka.tar.gz 
tar xzvf librdkafka.tar.gz 
rm -rf librdkafka.tar.gz

cd librdkafka-* 
./configure --prefix=/usr/local 
sed -i "s/WITH_LDS=/#WITH_LDS=/g" Makefile.config 
make 
make install
cd ../

# install curl
wget --no-check-certificate https://codeload.github.com/curl/curl/tar.gz/curl-7_45_0 -O curl-7.45.0.tar.gz
tar xzvf curl-7.45.0.tar.gz
rm -rf curl-7.45.0.tar.gz
mv curl-* curl
cd curl
./buildconf
./configure --prefix=/usr/local --disable-ldap --disable-ldaps

#./configure --prefix=/usr/local --disable-shared --enable-static --without-libidn --without-librtmp --without-gnutls --without-nss --without-libssh2 --without-zlib --without-winidn --disable-rtsp --disable-ldap --disable-ldaps --disable-ipv6 --without-ssl

make
make install
cd ../

# install liblz4
apt update  
apt install -y liblz4-dev 

# download viabtc
git clone https://github.com/Bringer-of-Light/viabtc_exchange_server.git
mv viabtc_exchange_server viabtc

cd viabtc
make -C depends/hiredis clean
make -C network clean
make -C utils clean
make -C accesshttp clean
make -C accessws clean
make -C matchengine clean
make -C marketprice clean
make -C alertcenter clean
make -C readhistory clean

make -C depends/hiredis
make -C depends/hiredis install
make -C network
make -C utils
make -C accesshttp
make -C accessws
make -C matchengine
make -C marketprice
make -C alertcenter
make -C readhistory
cd ..

# copy all exe file into bin
mkdir bin
cp -f viabtc/accesshttp/accesshttp.exe bin
cp -f viabtc/accessws/accessws.exe bin
cp -f viabtc/matchengine/matchengine.exe bin
cp -f viabtc/marketprice/marketprice.exe bin
cp -f viabtc/alertcenter/alertcenter.exe bin
cp -f viabtc/readhistory/readhistory.exe bin
ll
```

## Warning
1. Using source repo of viabtc won't success when manual compiling, many makefile in this repo has been modified.
2. This repo add some new features to json rpc API "balance.update", see the wiki for detail.

## Donation

* BTC: 14x3GrEoMLituT6vF2wcEbqMAxCvt2724s
* BCC: 1364ZurPv8uTgnFr1uqowJDFF15aNFemkf
* ETH: 0xA2913166AE0689C07fCB5C423559239bB2814b6D

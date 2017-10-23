# ViaBTC Exchange Server

ViaBTC Exchange Server is a trading backend with high-speed performance, designed for cryptocurrency exchanges. It can support up to 10000 trades every second and real-time user/market data notification though websocket.

## Architecture

![Architecture](https://user-images.githubusercontent.com/1209350/30948274-4e463044-a441-11e7-881b-b50ab903ed65.jpg)

For this project, it is marked as Server in this picture.

## code structure

**Require system**

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

## Compile and Install

**operating system**

Ubuntu 14.04 or Ubuntu 16.04. Not yet tested on other systems.

**Requirements**

See [requirements](https://github.com/viabtc/viabtc_exchange_server/wiki/requirements). Install the mentioned system or library.

You MUST use the depends/hiredis to install the hiredis library. Or it may not be compatible.

**Compile order**

Compile network and utils first. The rest all are independent.

**Deploy**

One signal instance is given for matchengine, marketprice and alertcenter, while readhistory, accesshttp and accwsswshave multi instance works with loadbalance.

Please do not install every instance on the same machine.

Every process runs in deamon and starts with a watchdog process. It will automatically restart within 1s when crashed.

The best practice of deploying the instance is in the following directory structure:

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

## API

[HTTP Protocl](https://github.com/viabtc/viabtc_exchange_server/wiki/HTTP-Protocol) and [Websocket Protocl](https://github.com/viabtc/viabtc_exchange_server/wiki/WebSocket-Protocol) documents are available in Chinese. Should time permit, we will have it translated into English in the future.

## Donation

* BTC: 14x3GrEoMLituT6vF2wcEbqMAxCvt2724s
* BCC: 1364ZurPv8uTgnFr1uqowJDFF15aNFemkf
* ETH: 0xA2913166AE0689C07fCB5C423559239bB2814b6D

## Paid service

If you need help deploying this system, we can provide paid technical support.

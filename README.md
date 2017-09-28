# ViaBTC Exchange Server

ViaBTC Exchange Server is a high performance trade backend for exchanges like digital currency exchange. It support up to 10000 trades every second. It support push user data and market data in real time thought websocket.

## Architecture

![Architecture](https://user-images.githubusercontent.com/1209350/30948274-4e463044-a441-11e7-881b-b50ab903ed65.jpg)

This project is the server part in this picture.

## code structure

**Require system**

* MySQL: save oper log, save user balance history, order history, trade history.

* Redis: a redis sentinel group, save market data.

* Kafka: a message system. 

**Base library**

* network: a event base and high performance network programming library, easily supoort [1000K TCP connections](http://www.kegel.com/c10k.html). Include TCP/UDP/UNIX SOCKET server and client implementation, a simple timer, state machine, thread pool. 

* utils: some basic library. Include log, config parse, some data structure, http/websocket/rpc server implementation.

**Modules**

* matchengine: the most important part, it reord user balance, execute user order. It is a in memory database, save oper log in MySQL, redo the oper log when start. It also write user history into MySQL, push balance and order and deal message to kafka.

* marketprice: read message from kafka, generage k line data.

* readhistory: read history data from MySQL.

* accesshttp: support a simple HTTP interface and hide complexity for upper layer.

* accwssws: a websocket server, support user data and market data query and push. You need nginx in front to support wss.

* alertcenter: a simple server write FATAL level log to redis list, so we can send alert emails.

## Compile and Install

**operating system**

Ubuntu 14.04 or Ubuntu 16.04. Not test on other system

**Requirements**

See [requirements](https://github.com/viabtc/viabtc_exchange_server/wiki/requirements). Install those system or library.

You MUST use the depends/hiredis to install the hiredis library. Or it may not compatibility.

**Compile order**

Compile network and utils first. The rest all are independent.

**Deploy**

The matchengine and marketprice and alertcenter is a signal instance, the readhistory and accesshttp and accwssws can have multi instance work with loadbalance.

Every instance better not install on same machine.

Every process runing in deamon and start with a watchdog process, if crash, it will autoly restart in 1s. 

The best practice of deploy the instance is in following directory structure:

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

There are [HTTP Protocl](https://github.com/viabtc/viabtc_exchange_server/wiki/HTTP-Protocol) and [Websocket Protocl](https://github.com/viabtc/viabtc_exchange_server/wiki/WebSocket-Protocol) documents. But it is in Chinese, I may translate it into English in the future.

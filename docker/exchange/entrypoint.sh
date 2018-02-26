#!/bin/bash
help () {
	echo "Available daemons are: "
	echo "  accesshttp"
	echo "  accessws"
	echo "  alertcenter"
	echo "  marketprice"
	echo "  matchengine"
	echo "  readhistory"
}

echo "@=$@"

if [ $# -lt 1 ];then
	help
	exit 0
fi

SERVICE=$1


case $SERVICE in 
	alertcenter)
		DAEMON=alertcenter
		;;
	matchengine)
		DAEMON=matchengine
		;;
	marketprice)
		DAEMON=marketprice
		;;
	readhistory)
		DAEMON=readhistory
		;;
	accesshttp)
		DAEMON=accesshttp
		;;
	accessws)
		DAEMON=accessws
		;;
	--help)
		help
		;;
	-h)
		help
		;;
  *)
		exec $@
		;;
esac

if [ $DAEMON ]; then 
	cd /viabtc_exchange_server
	echo "=== $DAEMON is starting"
	redis=`jq .redis.addr[0] < ${DAEMON}.json 2>/dev/null`
	test -z $redis || test $redis = 'null' && redis=''
	if [ -z $redis ];then
		redis=`jq .redis.addr < ${DAEMON}.json 2>/dev/null`
	fi
	test -z $redis || test $redis = 'null' && redis=''
	if [ $redis ];then
		echo "=== $DAEMON contains redis config: $redis"
		redis=`echo $redis|sed -e 's/"//g'`
		echo "=== $DAEMON: sentinel addr found: $redis"
		host=`echo $redis | cut -f1 -d':'`
		port=`echo $redis | cut -f2 -d':'`
		nc -z $host $port
		result=$?
	  while [ $result -ne 0 ]; do
			echo "=== $DAEMON: is waiting for sentinel on host=$host, port=$port ..."
			sleep 10
		  nc -z $host $port
		  result=$?
		done
		echo "=== $DAEMON: sentinel is up"
  fi
	kafka=`jq .brokers[0] < ${DAEMON}.json 2>/dev/null`
	test -z $kafka || test $kafka = 'null' && kafka=''
	if [ -z $kafka ];then
		kafka=`jq .brokers < ${DAEMON}.json 2>/dev/null`
	fi
	test -z $kafka || test $kafka = 'null' && kafka=''
	if [ -z $kafka ];then
	  kafka=`jq .deals.brokers[0] < ${DAEMON}.json 2>/dev/null`
	fi
	test -z $kafka || test $kafka = 'null' && kafka=''
	if [ -z $kafka ];then
	  kafka=`jq .deals.brokers < ${DAEMON}.json 2>/dev/null`
	fi
	test -z $kafka || test $kafka = 'null' && kafka=''
	if [ $kafka ];then
		echo "=== $DAEMON contains kafka config: $kafka"
		kafka=`echo $kafka|sed -e 's/"//g'`
		echo "=== $DAEMON: kafka addr found: $kafka"
		host=`echo $kafka | cut -f1 -d':'`
		port=`echo $kafka | cut -f2 -d':'`
		nc -z $host $port
		result=$?
	  while [ $result -ne 0 ]; do
			echo "=== $DAEMON: is waiting for kafka on host=$host, port=$port ..."
			sleep 10
		  nc -z $host $port
			result=$?
		done
		echo "=== $DAEMON: kafka is up"
  fi
	host=`jq .db_history.host < ${DAEMON}.json 2>/dev/null`
	test -z $host || test "$host" = 'null' && host=''
	if [ "$host" ];then
		port=`jq .db_history.port < ${DAEMON}.json 2>/dev/null`
		test -z $port || test $port = 'null' && port=''
		test $port || port=3306
		echo "=== $DAEMON contains db_history mysql host: $host and port: $port"
		host=`echo $host|sed -e 's/"//g'`
		port=`echo $port|sed -e 's/"//g'`
		nc -z $host $port
		result=$?
	  while [ $result -ne 0 ]; do
			echo "=== $DAEMON: is waiting for db_history on host=$host, port=$port ..."
			sleep 10
		  nc -z $host $port
			result=$?
		done
		echo "=== $DAEMON: db_history is up"
  fi
	host=`jq .db_log.host < ${DAEMON}.json 2>/dev/null`
	test -z $host || test "$host" = 'null' && host=''
	if [ "$host" ];then
		port=`jq .db_log.port < ${DAEMON}.json 2>/dev/null`
		test -z $port || test $port = 'null' && port=''
		test $port || port=3306
		echo "=== $DAEMON contains db_log mysql host: $host and port: $port"
		host=`echo $host|sed -e 's/"//g'`
		port=`echo $port|sed -e 's/"//g'`
		nc -z $host $port
		result=$?
	  while [ $result -ne 0 ]; do
			echo "=== $DAEMON: is waiting for db_log on host=$host, port=$port ..."
			sleep 10
		  nc -z $host $port
			result=$?
		done
		echo "=== $DAEMON: db_log is up"
  fi
	echo "=== $DAEMON: ready to exec"
	./$DAEMON.exe ${DAEMON}.json
	NEW=1
	while true; do
		sleep 5
		ps aux|grep ${DAEMON}.exe|grep -v grep  > /dev/null
		RESULT=$?
		if [ $RESULT -ne 0 ];then
			# some of services forks with _worker suffix
			ps aux|grep ${DAEMON}_worker |grep -v grep  > /dev/null
			RESULT=$?
			if [ $RESULT -ne 0 ];then
				echo "=== $DAEMON has died, exitting docker...";
				break;
			else
				if [ $NEW -eq 1 ]; then
					echo "Microservice $DAEMON(worker) has started";
				  NEW=0
			  fi
			fi
		else
			if [ $NEW -eq 1 ]; then
				echo "=== $DAEMON(exe) has started";
				NEW=0
			fi
		fi
  done
	exit 100
elif [ $BASH ]; then
	while true;do sleep 3600;done
else
	exit 101
fi

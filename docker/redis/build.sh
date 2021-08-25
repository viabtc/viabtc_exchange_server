#!/bin/sh
WD=`dirname $0`
if [ $# -lt 1 ]; then
	N=1
else
	N=$1
	shift
fi
cd $WD && docker build -t bitlum/exchange-redis-$N .

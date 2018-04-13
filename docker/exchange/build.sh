#!/bin/sh
WD=`dirname $0`/../..
cd $WD && docker build -t bitlum/exchange . -f docker/exchange/Dockerfile

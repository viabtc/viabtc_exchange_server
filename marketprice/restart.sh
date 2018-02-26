#!/bin/bash

killall -s SIGQUIT marketprice
sleep 1
./marketprice config.json

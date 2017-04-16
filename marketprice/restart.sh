#!/bin/bash

killall -s SIGQUIT marketprice.exe
sleep 1
./marketprice.exe config.json

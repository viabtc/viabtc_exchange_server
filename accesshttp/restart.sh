#!/bin/bash

killall -s SIGQUIT accesshttp.exe
sleep 1
./accesshttp.exe config.json

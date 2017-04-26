#!/bin/bash

killall -s SIGQUIT accessws.exe
sleep 1
./accessws.exe config.json

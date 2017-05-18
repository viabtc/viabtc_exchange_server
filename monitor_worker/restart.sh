#!/bin/bash

killall -s SIGQUIT monitor_worker.exe
sleep 1
./monitor_worker.exe config.json

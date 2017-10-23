#!/bin/bash

killall -s SIGQUIT readhistory.exe
sleep 1
./readhistory.exe config.json

#!/bin/bash

killall -s SIGQUIT readhistory
sleep 1
./readhistory config.json

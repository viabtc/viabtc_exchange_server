#!/bin/bash

killall -s SIGQUIT accessws
sleep 1
./accessws config.json

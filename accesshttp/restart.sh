#!/bin/bash

killall -s SIGQUIT accesshttp
sleep 1
./accesshttp config.json

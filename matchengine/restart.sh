#!/bin/bash

killall -s SIGQUIT matchengine.exe
sleep 1
./matchengine.exe config.json

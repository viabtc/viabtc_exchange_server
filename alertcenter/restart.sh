#!/bin/bash

killall -s SIGQUIT alertcenter.exe
sleep 1
./alertcenter.exe config.json

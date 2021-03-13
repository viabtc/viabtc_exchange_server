#!/bin/bash

killall -s SIGQUIT matchengine
sleep 1
./matchengine config.json

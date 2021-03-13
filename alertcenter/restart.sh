#!/bin/bash

killall -s SIGQUIT alertcenter
sleep 1
./alertcenter config.json

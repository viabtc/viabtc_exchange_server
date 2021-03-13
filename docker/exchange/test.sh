#!/bin/bash
docker stop exchange_test
docker rm exchange_test
docker run --name=exchange_test -it  bitlum/exchange $@

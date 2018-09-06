#!/bin/bash

#./cli 127.0.0.1 7316 102 '[1, 1, "BTC", "deposit", 1, "100"]'

./cli 127.0.0.1 7316 101 '[1, "BCH" ]'
./cli 127.0.0.1 7316 102 '[1, "BCH", "deposit", '$RANDOM', "100000", {}]'
./cli 127.0.0.1 7316 101 '[1, "BCH" ]'
./cli 127.0.0.1 7316 102 '[1, "BCH", "withdraw", '$RANDOM', "-100000", {}]'
./cli 127.0.0.1 7316 101 '[1, "BCH" ]'

# freeze
echo "freeze"
./cli 127.0.0.1 7316 102 '[1, "BCH", "deposit", '$RANDOM', "100", {}]'
./cli 127.0.0.1 7316 106 '[1, "BCH", "100"]'
./cli 127.0.0.1 7316 101 '[1, "BCH"]'

# unfreeze
echo "unfreeze"
./cli 127.0.0.1 7316 106 '[1, "BCH", "-100"]'
./cli 127.0.0.1 7316 101 '[1, "BCH"]'

./cli 127.0.0.1 7424 103 '[1, "BCH" ,"",0,0,0,10]'

# freeze
echo "freeze 2"
./cli 127.0.0.1 7316 106 '[1, "BCH", "100"]'
./cli 127.0.0.1 7316 101 '[1, "BCH"]'

# unfreeze and update
echo "update balance after unfreeze"
./cli 127.0.0.1 7316 107 '[1, "BCH", "withdraw", '$RANDOM', "-100", {}]'
./cli 127.0.0.1 7316 101 '[1, "BCH"]'

#./cli 127.0.0.1 7316 102 '[2, 1, "BTC", "deposit", 1, "100"]'
#./cli 127.0.0.1 7316 102 '[2, 1, "CNY", "deposit", 1, "10000"]'

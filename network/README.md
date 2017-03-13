## network is a asynchronous network programing library base on libev and in C.

network includes the following modules:

- `nw_buf`   : buf manager
- `nw_evt`   : main loop
- `nw_sock`  : socket releated
- `nw_ses`   : network session manager
- `nw_timer` : timer, call a function after specify time, repeat or not repeat
- `nw_svr`   : server implement, one server can bind multi address in different sock type
- `nw_clt`   : client implement, auto reconnect
- `nw_state` : state machine with timeout
- `nw_job`   : thread pool

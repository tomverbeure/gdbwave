set logging on
set logging overwrite on
dir ../test_data/sw_semihosting
set verbose on
set trace-commands on
set remotelogfile gdb_rsp.log
set pagination off
target extended-remote localhost:3333
br main
#c
#br 35
#c


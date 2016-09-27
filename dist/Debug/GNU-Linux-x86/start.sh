#!/bin/sh
cd /root/wizeds/eds/dist/Debug/GNU-Linux-x86
export LD_LIBRARY_PATH=/root/wizeds/eds/API/C/lib/:/root/wizeds/eds/API/C/edsapi/backend/9.1/
pid=`ps -ef | grep eds | grep -v grep`
killall -9 eds
ulimit -c unlimited
nohup ./eds --conf=conf/eds.ini > /dev/null 2>&1 2>&1 &

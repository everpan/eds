#!/bin/bash
lines=`ps -ef |grep -v grep| grep eds|grep -c "conf="`
echo $lines
if [[ $lines -ne 1 ]]; 
then
echo log:`date "+%Y-%m-%d %H:%M:%S"` 
`/data/eds_server/dist/Debug/GNU-Linux-x86/start.sh`
fi

#!/bin/bash

Log=/home/ubuntu/Server/Server_v1_log.txt
DATE=`date +%Y%m%d-%H%M%S`

mv -f "$Log" "$Log_`date +%Y-%m-%d-%H%M`"

echo "******************************" >> $Log
echo "  server is starting  "

Cnt=`ps -ex|grep "Server_v1"|grep -v grep|wc -l`
PROCESS=`ps -ex|grep "Server_v1"|grep -v grep|awk '{print $1}'`

if [ $Cnt -ne 0 ]
then
   echo "$DATE : server(PID : $PROCESS) is already started" >> $Log
else
   ./Server_v1&
   echo "$DATE : server implementation complete" >> $Log
fi

echo "******************************" >> $Log


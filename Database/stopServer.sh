#!/bin/bash

Log=/home/ubuntu/Server/Server_v1_log.txt
DATE=`date +%Y%m%d-%H%M%S`

echo "*****************************" >> $Log
echo "  server wil be terminated  "

Cnt=`ps -ex|grep "Server_v1"|grep -v grep|wc -l`
PROCESS=`ps -ex|grep "Server_v1"|grep -v grep|awk '{print $1}'`

if [ $Cnt -ne 0 ]
then
   kill -9 $PROCESS
   echo "$DATE : server(PID : $PROCESS) is terminated" >> $Log
else
   echo "$DATE : server is not playing" >> $Log
fi

echo "******************************" >> $Log

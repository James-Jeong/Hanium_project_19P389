#!/bin/sh

Log=/home/pi/Documents/Hanium_Project_1/server_log.out
DATE=`date +%Y%m%d-%H%M%S`

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" >> $Log
echo "  TCP server wil be terminated  "

Cnt=`ps -ex|grep "TCPserver"|grep -v grep|wc -l`
PROCESS=`ps -ex|grep "TCPserver"|grep -v grep|awk '{print $1}'`

if [ $Cnt -ne 0 ]
then
   kill -9 $PROCESS
   echo "$DATE : TCP server(PID : $PROCESS) is terminated" >> $Log
else
   echo "$DATE : TCP server is not playing" >> $Log
fi

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" >> $Log
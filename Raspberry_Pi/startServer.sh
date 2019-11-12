#!/bin/sh

Log=/home/pi/Documents/Hanium_Project_1/server_log.out
DATE=`date +%Y%m%d-%H%M%S`

mv -f "$Log" "$Log_`date +%Y-%m-%d-%H%M`"

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" >> $Log
echo "  TCP server is starting  "

Cnt=`ps -ex|grep "TCPserver"|grep -v grep|wc -l`
PROCESS=`ps -ex|grep "TCPserver"|grep -v grep|awk '{print $1}'`

if [ $Cnt -ne 0 ]
then
   echo "$DATE : TCP server(PID : $PROCESS) is already started" >> $Log
else
   ./TCPserver >> $Log &
   echo "$DATE : TCP server implementation complete" >> $Log
fi

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" >> $Log

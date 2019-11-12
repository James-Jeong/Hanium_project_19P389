#!/bin/bash

Log=/home/ubuntu/Server/monitor.out
DATE=`date +%Y%m%d-%H%M%S`
NORMAL_SLEEP=60
PROB_SLEEP=5

#Log File Rename
mv -f "$Log" "${Log}_`date +%Y-%m-%d-%H%M`"

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" >> $Log
echo "$DATE Database Server Start!"   >> $Log
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" >> $Log

while [ 1 ]
do
        DATE=`date +%Y%m%d-%H%M%S`

        Cnt=`ps -ex|grep "Server_v1"|grep -v grep|wc -l`

        if [ $Cnt \< 1 ]
        then
                PROCESS=`ps -ex|grep "Server_v1"|grep -v grep|awk '{print $1}'`
                if [ "$PROCESS" != "" ]
                then
                        echo "$DATE : terminate server" >> $Log
                        kill -9 $PROCESS
                        wait
                fi

                echo "$DATE : server restart" >> $Log

                ./startServer.sh &

                echo "$DATE : server restart complete" >> $Log

                sleep $PROB_SLEEP

                continue

        else
                echo "$DATE : Server is working normally!" >> $Log
        fi


        sleep $NORMAL_SLEEP

done

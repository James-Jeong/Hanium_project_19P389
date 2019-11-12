#!/bin/bash

Log=/home/pi/Documents/Hanium_Project_1/monitor.out
DATE=`date +%Y%m%d-%H%M%S`
NORMAL_SLEEP=60     #정상시 대기 시간
PROB_SLEEP=5     #장애발생시 대기 시간(장애시 곧바로 시작하기 위해)

#Log File Rename
mv -f "$Log" "${Log}_`date +%Y-%m-%d-%H%M`"

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" >> $Log
echo "$DATE TCP server Monitor Start!"   >> $Log
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" >> $Log

while [ 1 ]
do
        DATE=`date +%Y%m%d-%H%M%S`

        Cnt=`ps -ex|grep "TCPserver"|grep -v grep|wc -l`

        if [ $Cnt \< 1 ]
        then
                PROCESS=`ps -ex|grep "TCPserver"|grep -v grep|awk '{print $1}'`
                if [ "$PROCESS" != "" ]
                then
                        echo "$DATE : terminate server" >> $Log
                        kill -9 $PROCESS
                        wait
                fi

                echo "$DATE : TCP server restart" >> $Log
                 # 여기서 서버를 재기동한다.
                ./startServer.sh &

                echo "$DATE : TCP server restart complete" >> $Log

                sleep $PROB_SLEEP

                continue

        else
                echo "$DATE : Server is working normally!" >> $Log
        fi


        sleep $NORMAL_SLEEP

done
#!/bin/bash

CUR_SECONDS=`date +%S`
CUR_MINUTES=`date +%M`
CUR_HOURS=`date +%H`
CUR_DAYS=`date +%j`
CUR_YEARS=`date +%Y`
ADJUSTED_YEARS=$(echo "(${CUR_YEARS}-1970)" | bc)
NUM_LEAPYEARS=$(echo "(${ADJUSTED_YEARS} + 1)/4" | bc)
LEAP_ADJUST=$(echo "(${CUR_YEARS}-2001)/400 - (${CUR_YEARS}-2001)/100" | bc)
NUM_LEAPYEARS=$(echo "${NUM_LEAPYEARS} + ${LEAP_ADJUST}" | bc)
TOTAL_DAYS=$(echo "(${CUR_DAYS}-1+${NUM_LEAPYEARS})" | bc)
TOTAL_SECONDS=`echo "((((${ADJUSTED_YEARS}*365)+${TOTAL_DAYS})*24+${CUR_HOURS})*60+${CUR_MINUTES})*60+${CUR_SECONDS}" | bc`
echo ${TOTAL_SECONDS}

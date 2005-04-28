#!/bin/bash
# This script will convert an rfc 822 datetime to an integer number of seconds
# since Jan 1, 1970. The date is given as the first (and only) parameter.
# The output goes to stdout.

# Note: The time zone is currently ignored.

# Here's the format accepted (note that the year has been modified to accept 4
# digits).  See an example by running "date -R"

# 5.  DATE AND TIME SPECIFICATION
#
#     5.1.  SYNTAX
#
#     date-time   =  [ day "," ] date time        ; dd mm yy
#                                                 ;  hh:mm:ss zzz
#
#     day         =  "Mon"  / "Tue" /  "Wed"  / "Thu"
#                 /  "Fri"  / "Sat" /  "Sun"
#
#     date        =  1*2DIGIT month 4DIGIT        ; day month year
#                                                 ;  e.g. 20 Jun 82
#
#     month       =  "Jan"  /  "Feb" /  "Mar"  /  "Apr"
#                 /  "May"  /  "Jun" /  "Jul"  /  "Aug"
#                 /  "Sep"  /  "Oct" /  "Nov"  /  "Dec"
#
#     time        =  hour zone                    ; ANSI and Military
#
#     hour        =  2DIGIT ":" 2DIGIT [":" 2DIGIT]
#                                                 ; 00:00:00 - 23:59:59
#
#     zone        =  "UT"  / "GMT"                ; Universal Time
#                                                 ; North American : UT
#                 /  "EST" / "EDT"                ;  Eastern:  - 5/ - 4
#                 /  "CST" / "CDT"                ;  Central:  - 6/ - 5
#                 /  "MST" / "MDT"                ;  Mountain: - 7/ - 6
#                 /  "PST" / "PDT"                ;  Pacific:  - 8/ - 7
#                 /  1ALPHA                       ; Military: Z = UT;
#                                                 ;  A:-1; (J not used)
#                                                 ;  M:-12; N:+1; Y:+12
#                 / ( ("+" / "-") 4DIGIT )        ; Local differential
#                                                 ;  hours+min. (HHMM)


DATE_TEXT="${1}"

# First, replace any spaces or tabs with a single space.
DATE_TEXT=`echo "${DATE_TEXT}" | sed 's/\s/ /g' | sed 's/\s\+/ /g'`

# Now, we need to get rid of all optional fields that serve no purpose (DOW).
DATE_TEXT=`echo "${DATE_TEXT}" | sed -n 's/.*\(Mon,\|Tue,\|Wed,\|Thu,\|Fri,\|Sat,\|Sun,\)\(.*\)/\2/p'`

# Remove any leading spaces.
DATE_TEXT=`echo "${DATE_TEXT}" | sed -n 's/^\s\+\(.*\)$/\1/p'`


DAY=`echo "${DATE_TEXT}" | cut -f1 -d' '`
MONTH=`echo "${DATE_TEXT}" | cut -f2 -d' '`
YEAR=`echo "${DATE_TEXT}" | cut -f3 -d' '`
TIME=`echo "${DATE_TEXT}" | cut -f4 -d' '`
TIMEZONE=`echo "${DATE_TEXT}" | cut -f5 -d' '`


# Find out if it is a leap year.
LEAP_YEAR=0
TEMP=`echo "${YEAR} % 4" | bc`
if [ "${TEMP}" = "0" ]; then
	# Potentially a leap year.
	TEMP=`echo "${YEAR} % 100" | bc`
	if [ "${TEMP}" = "0" ]; then
		# Still could be a leap year (every 400 years)
		TEMP=`echo "${YEAR} % 400" | bc`
		if [ "${TEMP}" = "0" ]; then
			LEAP_YEAR=1
		fi
	else
		LEAP_YEAR=1
	fi
fi
unset TEMP

#DAYS_PER_MONTH=(31 28 31 30  31  30  31  31  30  31  30  31)
DAYS_PRIOR_TO_MONTH=(0 31 59 90 120 151 181 212 243 273 304 334)

# Replace the month with the corresponding number.
MONTH_INDEX=`echo "${MONTH}" | \
		sed -n \
			-e "s/Jan/0/p" \
			-e "s/Feb/1/p" \
			-e "s/Mar/2/p" \
			-e "s/Apr/3/p" \
			-e "s/May/4/p" \
			-e "s/Jun/5/p" \
			-e "s/Jul/6/p" \
			-e "s/Aug/7/p" \
			-e "s/Sep/8/p" \
			-e "s/Oct/9/p" \
			-e "s/Nov/10/p" \
			-e "s/Dec/11/p"`

# Calculate the current day of the year.
DAY_OF_YEAR=`echo "${DAYS_PRIOR_TO_MONTH[${MONTH_INDEX}]}+${DAY}" | bc`

if [ ${MONTH_INDEX} -ge 2 ]; then
  DAY_OF_YEAR=`echo "${DAY_OF_YEAR}+${LEAP_YEAR}" | bc`
fi 

HOURS=`echo ${TIME} | cut -f1 -d':'`
MINUTES=`echo ${TIME} | cut -f2 -d':'`
SECONDS=`echo ${TIME} | cut -f3 -d':'`

ADJUSTED_YEARS=$(echo "(${YEAR}-1970)" | bc)
# A bad approximation.  This shoule be fixed sometime in the next 100 years.
NUM_LEAP_YEARS=$(echo "((${ADJUSTED_YEARS} + 1)/4) + (${YEAR}-2001)/400 - (${YEAR}-2001)/100" | bc)
TOTAL_DAYS=$(echo "(${DAY_OF_YEAR}-1+${NUM_LEAP_YEARS})" | bc)

# Seconds are optional, so leave them out for now, and add them in later (if
# they were given).
TOTAL_SECONDS=`echo "((((${ADJUSTED_YEARS}*365)+${TOTAL_DAYS})*24+${HOURS})*60+${MINUTES})*60" | bc`
if [ ! -z "${SECONDS}" ]; then
  TOTAL_SECONDS=`echo "${TOTAL_SECONDS}+${SECONDS}" | bc`
fi
echo ${TOTAL_SECONDS}


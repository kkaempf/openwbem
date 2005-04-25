#!/bin/bash

############################################################################
# Usage: remote_build.sh MACHINE_NAME CONFIG_FILE_NAME
#
# This script will:
# 1. Store it's pid in the file ${LOG_DIRECTORY}/${MACHINE_NAME}.pid
# 2. Create a temporary script on the build machine MACHINE_NAME for getting an
#    initial checkout of OW, so that it can use it for the remainder of the
#    build process. 
# 3. Execute this bootstrap script on the remote machine.

# 4. Execute the build script found in the ${CONFIG_FILE_NAME} on MACHINE_NAME
# 5. Echo the return value, so that it can be logged.
# 6. Remove the pid file
# 7. Mirror the return value in our own return value (even though it will
#    likely be ignored). 
############################################################################

echo "Remote build params: $*"

# Abort on single errors.
set -e

# Enable error reporting on undefined vars.
set -u

MESSAGE_PREFIX_RELEASE="OW_BUILD_MESSAGE_RELEASE:"
RESULTS_PREFIX_RELEASE="OW_BUILD_RESULTS_RELEASE:"
MESSAGE_PREFIX_DEBUG="OW_BUILD_MESSAGE_DEBUG:"
RESULTS_PREFIX_DEBUG="OW_BUILD_RESULTS_DEBUG:"
REMOTE_RESULT=0

if [ $# -lt 2 ]
then
	echo "$MESSAGE_PREFIX_RELEASE} Remote build executed without machine name or config file."
	echo "$RESULTS_PREFIX_RELEASE} 1"
	exit 1
fi

MACHINE_NAME=$1
CONFIG_FILE_NAME=$2
PID_FILE=${LOG_DIRECTORY}/${MACHINE_NAME}.pid

shift 2
SAVED_COMMAND_LINE="$@"

# Execute the term handler, so if any problems occur, this script will kill all children.
. term_handler.sh
. common_functions.sh

############################################################################
#
# 1. Create the PID file.
#
############################################################################
echo $$ > $PID_FILE

build_history_entry "begin"

BUILD_FINISHED=0

function remove_pid_file()
{
	build_history_entry "end"
	if [ ${BUILD_FINISHED} -ne 1 ]
	then
		echo "${MESSAGE_PREFIX_RELEASE} Unexpected exit occurred"
		echo "${RESULTS_PREFIX_RELEASE} 2"
		rm -f ${PID_FILE}
		exit 2
	else
		rm -f ${PID_FILE}
	fi
}
trap remove_pid_file EXIT

# Execute the config file so that some settings can be obtained before the remote execution.
# Note: This will change the OW_SOURCE_DIR, so it can't be used after this line.
. ${OW_SOURCE_DIR}/build/automated/data/${CONFIG_FILE_NAME}

############################################################################
#
# 2. Create the temporary script.
#
############################################################################
TEMP_SCRIPT_NAME=/tmp/bootstrap_script_$$.sh

build_history_entry "begin:bootstrap"
echo -e "\n<a name=\"BOOTSTRAP\">\n"

# Create a script... This will check out OW so that the config file will exist on that machine.
# NOTE: This is a self-destructive script, it will remove itself after use.
echo "#!/bin/bash" > $TEMP_SCRIPT_NAME
if [ ! -z "$OW_REMOTE_PATH" ]; then
	echo "export PATH=$OW_REMOTE_PATH" >> $TEMP_SCRIPT_NAME
fi
echo "set -e" >> $TEMP_SCRIPT_NAME
echo "if [ -e $OW_WORK_DIRECTORY ]; then rm -rf $OW_WORK_DIRECTORY; fi" >> $TEMP_SCRIPT_NAME
echo "mkdir -p $OW_WORK_DIRECTORY" >> $TEMP_SCRIPT_NAME
echo "mkdir -p $LOCAL_BUILD_DIR" >> $TEMP_SCRIPT_NAME
echo "cd $LOCAL_BUILD_DIR" >> $TEMP_SCRIPT_NAME
echo "export CVS_RSH=ssh" >> $TEMP_SCRIPT_NAME
echo "if [ -e openwbem ]; then rm -rf openwbem; fi" >> $TEMP_SCRIPT_NAME
echo "scp -r $HOSTNAME:$OW_SOURCE_DIR ." >> $TEMP_SCRIPT_NAME
echo "rm \$0" >> $TEMP_SCRIPT_NAME
chmod a+x $TEMP_SCRIPT_NAME

# A hard-coded (ICK) path to the bootstrap script.
INITIAL_BOOTSTRAP=$OW_USER_HOME/initial_bootstrap.sh

# Copy the temporary script to the build machine.
scp ${TEMP_SCRIPT_NAME} ${OW_BUILD_USER}@${MACHINE_NAME}:${INITIAL_BOOTSTRAP}
rm $TEMP_SCRIPT_NAME

############################################################################
#
# 3. Execute the bootstrap.
#
############################################################################
# Execute the temporary script in bash (wherever in the path bash can be found)
ssh ${OW_BUILD_USER}@${MACHINE_NAME} bash -x ${INITIAL_BOOTSTRAP}

build_history_entry "end:bootstrap"

# Now, there should be a clean copy of openwbem in $OW_WORK_DIRECTORY on
# MACHINE_NAME.

############################################################################
#
# 4. Execute the build script.
#
############################################################################

# This function will check the log (which also happens to be a combination of
# std/stdout for the current script) for a kill request from the remote
# machine.  If such a request is found, it will return 0, and set the
# REMOTE_RESULT variable. 
function check_for_kill_request()
{
	echo "Checking for kill request".
	local LOG_FILE=${LOG_DIRECTORY}/${MACHINE_NAME}.log
	local KILL_REQUEST_FOUND_TEXT="Found kill request."
	local NUMBER_OF_LINES=100

	local TEMP_FILE=`mktemp /tmp/log_tailing_${MACHINE_NAME}.XXXXXX`

	tail -n $NUMBER_OF_LINES ${LOG_FILE} | egrep -v "(grep.*kill)|(local.*kill)" > $TEMP_FILE

	# Trim the file, if needed, to get rid of prior kill requests.
	if grep "$KILL_REQUEST_FOUND_TEXT" $TEMP_FILE >/dev/null 2>&1; then
		local SPLIT_LINE=`grep -n "$KILL_REQUEST_FOUND_TEXT" $TEMP_FILE | tail -1 | cut -f1 -d':'`
		local TEMP_FILE2=`mktemp $TEMP_FILE.XXXXXX`
		local NUM_LINES=`cat $TEMP_FILE | wc -l`
		local SKIP_LINES=`echo $NUM_LINES-$SPLIT_LINE-1 | bc`
		tail -$SKIP_LINES $TEMP_FILE > $TEMP_FILE2
		mv -f $TEMP_FILE2 $TEMP_FILE		
	fi

	local RETURN_VALUE=1

	if cat $TEMP_FILE | tail -n $NUMBER_OF_LINES  | grep '^KILL_ME_PLEASE'
	then
		echo "$KILL_REQUEST_FOUND_TEXT"
		REMOTE_RESULT=$(tail -n ${NUMBER_OF_LINES} ${TEMP_FILE} | grep '^KILL_ME_PLEASE' | tail -n1 | cut -f2- -d':')
		RETURN_VALUE=0
	fi
	
	rm -f $TEMP_FILE

	return $RETURN_VALUE
}

BUILD_METHOD=
function execute_build()
{
	build_history_entry "begin:build:$BUILD_METHOD"
	# Disable the auto-exit on errors, since the build may fail, and we
	# want to have real results from both builds.
	set +e
	echo "Command line to execute on remote:"
	echo "ssh ${OW_BUILD_USER}@${MACHINE_NAME} bash -x ${OW_BUILD_SCRIPT} ${OW_SOURCE_DIR}/build/automated/data/${CONFIG_FILE_NAME} ${BUILD_METHOD} $SAVED_COMMAND_LINE"
	# We're safe to execute their chosen build script now.
	ssh ${OW_BUILD_USER}@${MACHINE_NAME} ${USABLE_SHELL} -x ${OW_BUILD_SCRIPT} ${OW_SOURCE_DIR}/build/automated/data/${CONFIG_FILE_NAME} ${BUILD_METHOD} $SAVED_COMMAND_LINE &
	local killed=0

	while ps $! 2>/dev/null >/dev/null
	do
		sleep 60 || true
		if ps $! 2>/dev/null >/dev/null && check_for_kill_request
		then
			echo "The remote process reqested a kill"
			kill -term $!
			killed=1
		fi
	done

	echo "Waiting for the process to terminate."
	wait $!
	RESULTS=$?
	# If the remote process was killed (by request), use the value specified in
	# the results instead of the return value (which will show an error).
	if [ "x$killed" != "x0" ]
	then
		local LOG_FILE=${LOG_DIRECTORY}/${MACHINE_NAME}.log
		RESULTS=${REMOTE_RESULT}
		RESULTS=$(echo $RESULTS)
	fi
	echo "Remote execution finished."
	set -e
	build_history_entry "end:build:$BUILD_METHOD:$RESULTS"
}

if build_release; then
	echo -e "\n<a name=\"RELEASE\">\n"
	# Perform the release build
	BUILD_METHOD=release
	execute_build 
	RELEASE_RESULTS=$RESULTS

	############################################################################
	#
	# 5.release Output the return value.
	#
	############################################################################
	if [ "$RELEASE_RESULTS" != "0" ]
	then
  	echo "${MESSAGE_PREFIX_RELEASE} Remote build failed."
		echo "${RESULTS_PREFIX_RELEASE} ${RELEASE_RESULTS}"
	else
	  echo "${MESSAGE_PREFIX_RELEASE} Remote build was successful."
		echo "${RESULTS_PREFIX_RELEASE} ${RELEASE_RESULTS}"
	fi
else
	echo "Release not built."
fi

echo -e "\n<a name=\"DEBUG\">\n"
echo "==============================================================================="
echo "=OW_DEBUG_BEGINS=============================================================="
echo "==============================================================================="

if build_debug ; then
	# Build the debug build.
	BUILD_METHOD=debug
	execute_build 
	DEBUG_RESULTS=$RESULTS


	############################################################################
	#
	# 5.debug Output the return value.
	#
	############################################################################
	if [ "$DEBUG_RESULTS" != "0" ]
	then
	  echo "${MESSAGE_PREFIX_DEBUG} Remote build failed."
	  echo "${RESULTS_PREFIX_DEBUG} ${DEBUG_RESULTS}"
	else
	  echo "${MESSAGE_PREFIX_DEBUG} Remote build was successful."
	  echo "${RESULTS_PREFIX_DEBUG} ${DEBUG_RESULTS}"
	fi
else
	echo "Debug not built."
fi

# Set a flag so that the automatic error reporting won't happen.
BUILD_FINISHED=1


############################################################################
#
# 6. Remove the PID file.
#
############################################################################
# This was commented out, because it is removed automatically (in the exit handler).
# remove_pid_file;


############################################################################
#
# 7. Mirror the return value.
#
############################################################################
# If a non-zero result was given for either release or debug, mirror it.
if build_release && [ ! -z "${RELEASE_RESULTS}" ] && [ "${RELEASE_RESULTS}" != "0" ] ; then
	exit ${RELEASE_RESULTS}
fi
if build_debug   && [ ! -z "${DEBUG_RESULTS}" ] && [ "${DEBUG_RESULTS}" != "0" ] ; then
	exit ${DEBUG_RESULTS}
fi
# If no results were given for either release or debug, return 99 (a magic number).
if build_release && [ -z "${RELEASE_RESULTS}" ] ; then
	exit 99
fi
if build_debug   && [ -z "${DEBUG_RESULTS}" ] ; then 
	exit 99
fi

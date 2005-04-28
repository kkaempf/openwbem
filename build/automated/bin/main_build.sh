#!/bin/bash

# This script is ONLY to be executed from the cron_build.sh script.
set -u
set -a

# don't exit immediately now if it fails.
set +e

# Initialize the build variables.
RVAL=0

OW_BUILD_NUM=`date '+%y%m%d%H%M'`

OW_MAJOR_VERSION=`sed -n -e 's/OPENWBEM_MAJOR_VERSION=\(.*\)/\1/p' < $OW_SOURCE_DIR/configure.in`
OW_MINOR_VERSION=`sed -n -e 's/OPENWBEM_MINOR_VERSION=\(.*\)/\1/p' < $OW_SOURCE_DIR/configure.in`
OW_MICRO_VERSION=`sed -n -e 's/OPENWBEM_MICRO_VERSION=\(.*\)/\1/p' < $OW_SOURCE_DIR/configure.in`
OW_VERSION="$OW_MAJOR_VERSION.$OW_MINOR_VERSION.$OW_MICRO_VERSION"

if [ "x$BRANCH" != "xHEAD" ]; then
	TAG_TEXT="$BRANCH."
fi

if [ "$OFFICIAL_BUILD" != "1" ]; then
	OW_BUILD_ID=`printf "%s.unofficial.%s%s" "$OW_VERSION" "$TAG_TEXT" "$OW_BUILD_NUM"`
else
	OW_BUILD_ID=`printf "%s.%s" "$OW_VERSION" "$OW_BUILD_NUM"`
fi

LOG="/tmp/ow_buildlog-$OW_BUILD_ID-$$"

# Variables to control the locations of where to put the packages
PACKAGE_LOCATION_RELEASE=$OW_WORK_DIRECTORY/release_packages
PACKAGE_LOCATION_DEBUG=$OW_WORK_DIRECTORY/debug_packages

# Set up a term handler, so if any problems occur, this script will kill all children.
. term_handler.sh
. common_functions.sh

MACHINE_NAME=master

build_history_entry "begin"

# Close stdout, stderr, and replace them with a file (will be copied to jerry upon completion or failure).
main_build_output_file=`mktemp /tmp/ow_main_build_log.txt.XXXXXX`
exec 1>&-
exec 2>&-
exec 1>$main_build_output_file
exec 2>&1
chmod a+r $main_build_output_file


REMOTE_BUILD_SCRIPT=remote_build.sh

BRANCH_TEXT=
BRANCH_SUFFIX=

[ "$CVS_BRANCH" != "HEAD" ] || BRANCH_TEXT=" (branch $CVS_BRANCH)"
[ "$CVS_BRANCH" != "HEAD" ] || BRANCH_SUFFIX=".$CVS_BRANCH"

function start_build_on_machine()
{
	if [ $# -eq 3 ]
	then
		BUILD_MACHINE_NAME="$1"
		BUILD_CONFIG_FILE_NAME="$2"
		TARGET_PACKAGE_DIR="$3"
		BUILD_LOG_FILE=${LOG_DIRECTORY}/${BUILD_MACHINE_NAME}.log

		mkdir -p $PACKAGE_LOCATION_RELEASE/$TARGET_PACKAGE_DIR
		mkdir -p $PACKAGE_LOCATION_DEBUG/$TARGET_PACKAGE_DIR

		# EVIL HACK!!! Windows needs to be handled separately, so as to
		# not cause problems with the current machine, if it is also
		# the linux build box. 
		if [ "$TARGET_PACKAGE_DIR" = "win32" ]
		then
			build_release && export OW_PACKAGE_DESTINATION_RELEASE="$HOSTNAME:$PACKAGE_LOCATION_RELEASE/$TARGET_PACKAGE_DIR"
			build_debug && export OW_PACKAGE_DESTINATION_DEBUG="$HOSTNAME:$PACKAGE_LOCATION_DEBUG/$TARGET_PACKAGE_DIR"
			WIN_BUILD_MACHINE=${BUILD_MACHINE_NAME}
			ow_win_builder.sh ${BUILD_MACHINE_NAME} >& ${BUILD_LOG_FILE} &
			build_release && unset OW_PACKAGE_DESTINATION_RELEASE
			build_debug && unset OW_PACKAGE_DESTINATION_DEBUG
		else
			BUILD_TYPE_VARS=
			build_release && BUILD_TYPE_VARS="$BUILD_TYPE_VARS OW_PACKAGE_DESTINATION_RELEASE=$HOSTNAME:$PACKAGE_LOCATION_RELEASE/$TARGET_PACKAGE_DIR"
			build_debug && BUILD_TYPE_VARS="$BUILD_TYPE_VARS OW_PACKAGE_DESTINATION_DEBUG=$HOSTNAME:$PACKAGE_LOCATION_DEBUG/$TARGET_PACKAGE_DIR"

			# Execute the remote building script.
			# NOTE: The parameters passed to this script in the form
			# A=B will be used as environment variables. 
			${REMOTE_BUILD_SCRIPT} ${BUILD_MACHINE_NAME} ${BUILD_CONFIG_FILE_NAME} \
				$BUILD_TYPE_VARS \
				"OW_BUILD_ID=$OW_BUILD_ID" \
				"OW_MAJOR_VERSION=$OW_MAJOR_VERSION" \
				"OW_MINOR_VERSION=$OW_MINOR_VERSION" \
				"OW_MICRO_VERSION=$OW_MICRO_VERSION" \
				"OW_VERSION=$OW_VERSION" \
				"OW_BUILD_NUM=$OW_BUILD_NUM" \
				"CVS_BRANCH=$CVS_BRANCH" \
				"CVS_BRANCH_FLAG=$CVS_BRANCH_FLAG" \
				"CVS_BRANCH_DATE=\"$CVS_BRANCH_DATE\"" \
				>& ${BUILD_LOG_FILE} &
			unset BUILD_TYPE_VARS
		fi


	else
		# Return a non-zero value.
		false
	fi
}

# Generate a sequence of numbers that can be used in a for loop, so that
# resulting code can be simplified (and less error prone) when iterating 
# over values in arrays.
generate_numbers_for_loop()
{
	local SIZE=$1
	local index=0
	local results=""
	while [ $index -lt $SIZE ]
	do
		results="$results $index"
		let "index=$index+1"
	done
	echo $results
}

# Make some arrays for use in the remainder of this script.
declare -a BUILD_MACHINES
declare -a CONFIG_FILES
declare -a PACKAGE_OUTPUT_DIRECTORIES
FAILED_MACHINES=
declare -a EXTRA_ERRORS
declare -a FINAL_RESULT_ARRAY

BUILD_ERROR_BEFORE_END=1

function handle_logs_for_error()
{
	# Clear out any spaces...
	FAILED_MACHINES=`echo $FAILED_MACHINES`
	
	EXTRA_SUBJECT_TEXT=""

	if [ "$BUILD_ERROR_BEFORE_END" = "1" ]
	then
		EXTRA_ERRORS[${#EXTRA_ERRORS[@]}]="The build was interrupted (or killed)."
		EXTRA_SUBJECT_TEXT=" (interrupted)"
	fi

	rm -f $LOG-merged

	function send_error_log_on_errors()
	{
		echo "Oops... Something went wrong while creating an error message (script error)$BRANCH_TEXT.  Sending the log anyway." >> $LOG-merged
		
		if [ -d ${LOG_DIRECTORY} ]; then
			for x in ${LOG_DIRECTORY}/*; do
				echo "==============================" >> $LOG-merged
				echo "File $x:" >> $LOG-merged
				echo "==============================" >> $LOG-merged
				tail -n $MAX_OUTPUT_LINES $x >> $LOG-merged
			done
		else
			echo "The log directory does not exist.  Check for logs ($OW_BUILD_ID) on jerry:/data/ow/devbuilds" >> $LOG-merged
		fi

		if [ "$OFFICIAL_BUILD" = "1" ]; then
			cat $LOG-merged | mail -s"ow build $OW_BUILD_ID ($FAILED_MACHINES) unsuccessful (interruption/failure) :~(" $RESULTS_EMAIL_ADDRESS
			rm $LOG-merged
			scp $main_build_output_file $FINAL_DATA_DESTINATION/$OW_BUILD_ID.master.log
			rm -f $main_build_output_file
		fi
		build_history_entry "end:error"
	}
	trap send_error_log_on_errors EXIT

	echo "###########################################################################" >> $LOG-merged
	echo "OW Build$BRANCH_TEXT failed on $FAILED_MACHINES" >> $LOG-merged
	echo "###########################################################################" >> $LOG-merged
	build_release && echo "The release build was attempted." >> $LOG-merged
	build_debug && echo "The debug build was attempted." >> $LOG-merged
	echo "This build was attempted on: ${BUILD_MACHINES[@]}" >> $LOG-merged
	echo "The results were:" >> $LOG-merged
	for index in `generate_numbers_for_loop ${#BUILD_MACHINES[@]}`
	do
		if [ $index -lt ${#FINAL_RESULT_ARRAY[@]} ]
		then
			echo "${BUILD_MACHINES[$index]}: ${FINAL_RESULT_ARRAY[$index]}" >> $LOG-merged
		else
			echo "${BUILD_MACHINES[$index]}: UNKNOWN" >> $LOG-merged
		fi
	done
	
	if [ "${#EXTRA_ERRORS[@]}" != "0" ]; then
		echo "Additional errors were found:" >> $LOG-merged
		for index in `generate_numbers_for_loop ${#EXTRA_ERRORS[@]}`
		do
			echo "ERROR: ${EXTRA_ERRORS[$index]}" >> $LOG-merged
		done
	fi

	if [ "$OFFICIAL_BUILD" = "1" ]; then
		# Put an entry in the email about the location of logs.
		echo >> $LOG-merged
		echo "===========================================================================" >> $LOG-merged
		echo "The complete build logs can be found on $FINAL_DATA_DESTINATION" >> $LOG-merged
		
		# Copy them to a common location.
		for index in `generate_numbers_for_loop ${#BUILD_MACHINES[@]}`
		do
			machine=${BUILD_MACHINES[$index]}
			echo "${PACKAGE_OUTPUT_DIRECTORIES[$index]}: $OW_BUILD_ID.$machine.log" >> $LOG-merged
			scp ${LOG_DIRECTORY}/${machine}.log $FINAL_DATA_DESTINATION/$OW_BUILD_ID.$machine.log
		done

		if [ -e ${LOG_DIRECTORY}/owdocs.log ]
		then
			scp ${LOG_DIRECTORY}/owdocs.log $FINAL_DATA_DESTINATION/$OW_BUILD_ID.documentation.log
		fi
	fi
	
	for machine in $FAILED_MACHINES; do
		echo >> $LOG-merged
		echo "===========================================================================" >> $LOG-merged
		echo "  Log from $machine:" >> $LOG-merged
		echo "===========================================================================" >> $LOG-merged
	  	# Since the log was already merged (debug/release), split it,
		# and get it's component lines from each. 

		total_lines=`cat ${LOG_DIRECTORY}/${machine}.log | wc -l`
		split_line=`cat ${LOG_DIRECTORY}/${machine}.log | tr '[\0]' '[!]' | grep -n OW_DEBUG_BEGINS 2>/dev/null | cut -f1 -d':' | tail -1`


		if [ ! -z "$split_line" ]
		then
			echo "=========================" >> $LOG-merged
			echo "Release log ($machine): " >> $LOG-merged
			echo "=========================" >> $LOG-merged
			head -n $split_line ${LOG_DIRECTORY}/${machine}.log | tail -n $MAX_OUTPUT_LINES >> $LOG-merged
			echo "=========================" >> $LOG-merged
			echo "Debug log ($machine): " >> $LOG-merged
			echo "=========================" >> $LOG-merged
			debug_lines=`echo $total_lines-$split_line | bc`
			tail -n $debug_lines ${LOG_DIRECTORY}/${machine}.log | tail -n $MAX_OUTPUT_LINES >> $LOG-merged
		else
			# In this case, it doesn't appear to have the split.  Just dump $MAX_OUTPUT_LINES lines.
			tail -n $MAX_OUTPUT_LINES ${LOG_DIRECTORY}/${machine}.log >> $LOG-merged
		fi
		echo >> $LOG-merged
		echo >> $LOG-merged
		echo "===========================================================================" >> $LOG-merged
	done

	if [ "$OFFICIAL_BUILD" = "1" ]; then

		cat $LOG-merged | mail -s"ow build $OW_BUILD_ID ($FAILED_MACHINES) unsuccessful${EXTRA_SUBJECT_TEXT} :~(" $RESULTS_EMAIL_ADDRESS
		rm $LOG-merged
		rm -rf ${LOG_DIRECTORY}
	fi
	trap 'echo "Main build terminating with errors"' EXIT
	build_history_entry "end:error"
	if [ "$OFFICIAL_BUILD" = "1" ]; then
		scp $main_build_output_file $FINAL_DATA_DESTINATION/$OW_BUILD_ID.master.log
		rm -f $main_build_output_file
	fi
	exit 71
} # handle_logs_for_error()

# Use this as the exit handler.
trap handle_logs_for_error EXIT


###########################################################################
# Go through the build machine list and create an array for the machines and
# config files.   
# Note: This skips empty lines, and lines with a leading #
###########################################################################
BUILD_MACHINE_LIST_FILE=$PATH_TO_BUILD_SYSTEM/data/build_machine_list

# This will make sure the build machine list only contains the machines which
# were listed in the list for the particular branch.
OLD_DIR=`pwd`
cd $PATH_TO_BUILD_SYSTEM/data
cvs update $CVS_BRANCH_FLAG build_machine_list
cd $OLD_DIR
for machine_iter in `egrep -v '^[ \t]*#|^$' $BUILD_MACHINE_LIST_FILE | cut -f1 -d' '`
do
	config_line=`egrep "^[ \t]*$machine_iter" $BUILD_MACHINE_LIST_FILE | head -n1`
	temp_config_file=`echo $config_line | cut -f2 -d' '`
	temp_iso_directory=`echo $config_line | cut -f3 -d' '`
	let local_index=${#BUILD_MACHINES[@]}
	BUILD_MACHINES[$local_index]=$machine_iter
	CONFIG_FILES[$local_index]=$temp_config_file
	PACKAGE_OUTPUT_DIRECTORIES[$local_index]=$temp_iso_directory
	unset config_line temp_config_file temp_iso_directory local_index 
done

###########################################################################
# Start up the builds.
###########################################################################
for cur_index in `generate_numbers_for_loop ${#BUILD_MACHINES[@]}`
do
  start_build_on_machine "${BUILD_MACHINES[$cur_index]}" "${CONFIG_FILES[$cur_index]}" "${PACKAGE_OUTPUT_DIRECTORIES[$cur_index]}" 
done

###########################################################################
# Wait for the builds to finish
###########################################################################
let "num_running=1"
while [ $num_running -gt 0 ]
do
  # Wait at least 10 seconds before checking for completion.
  sleep 10 || true

  let "num_running=0"
  for cur_index in `generate_numbers_for_loop ${#BUILD_MACHINES[@]}`
  do
    temp_machine=${BUILD_MACHINES[$cur_index]}

    # If the PID file still exists, then the build hasn't finished.
    if [ -e "${LOG_DIRECTORY}/${temp_machine}.pid" ]
    then
      if ps `cat "${LOG_DIRECTORY}/${temp_machine}.pid"` 2>/dev/null >/dev/null
      then
        let "num_running+=1"
      else
        EXTRA_ERRORS[${#EXTRA_ERRORS[@]}]="$temp_machine quit without removing PID file."
        rm -f "${LOG_DIRECTORY}/${temp_machine}.pid"
      fi
    fi
  done
done
unset num_running cur_index


# arg1=log file, arg2=machine name arg3=build mode
get_results()
{
	[ $# -eq 3 ] || return 1
	
	local log_file=$1
	local machine_name=$2
	local build_mode=`echo $3 | tr '[a-z]' '[A-Z]'`
	local build_mode_lower=`echo $build_mode | tr '[A-Z]' '[a-z]'`
	
	local temp_result_line=`cat $log_file | tr '[\0]' '[!]' | grep "^OW_BUILD_RESULTS_${build_mode}:" | tail -n1`

	local result="NO_RESULTS_GIVEN"
	
	# Check to see if there is a result line
	if [ ! -z "$temp_result_line" ]
	then
		# Grab the data from the right side of the ':'
		local temp_result=`echo "$temp_result_line" | cut -f2 -d':'`
		# Strip any whitespace (left and right)
		temp_result=`echo $temp_result`

		if [ ! -z "$temp_result" ]
		then
			result=$temp_result
		fi
	fi

	[ "$result" != "NO_RESULTS_GIVEN" ] || EXTRA_ERRORS[${#EXTRA_ERRORS[@]}]="$machine_name ($build_mode_lower)--No results."

	local output_directory=`cat $log_file | tr '[\0]' '[!]' | egrep "^.*OW_OUTPUT_DIRECTORY_${build_mode}=.*\$" 2>/dev/null | sed "s,OW_OUTPUT_DIRECTORY_${build_mode}=\(.*\),\1,g" | tail -n1`
	# Only set an output missing result if the return value was good (0),
	# anything else should show its own error. 
	if [ -z "$output_directory" -a "$result" = "0" ]
	then
		result="NO_OUTPUT_DIRECTORY"
		EXTRA_ERRORS[${#EXTRA_ERRORS[@]}]="$machine_name ($build_mode_lower)--No output directory supplied."
	fi

	echo $result

	return 0
}


###########################################################################
# Collect the build results.
###########################################################################
FINAL_RESULTS=0
for cur_index in `generate_numbers_for_loop ${#BUILD_MACHINES[@]}`
do
	temp_machine=${BUILD_MACHINES[$cur_index]}

	# These numbers are too high for any normal process to return (0-127).
	# As such, they are easily recognizable.
	debug_result="NO_RESULTS_GIVEN"
	release_result="NO_RESULTS_GIVEN"
	combined_result="${release_result}.${debug_result}"

	if build_release; then
		release_result=`get_results ${LOG_DIRECTORY}/${temp_machine}.log ${temp_machine} RELEASE`
	else
		release_result=0
	fi
	[ "${release_result}" = "0" ] || FINAL_RESULTS=$release_result

	if build_debug; then
		debug_result=`get_results ${LOG_DIRECTORY}/${temp_machine}.log ${temp_machine} DEBUG`
	else
		debug_result=0
	fi

	[ "${debug_result}" = "0" ] || FINAL_RESULTS=$debug_result

	combined_result="${release_result}.${debug_result}"

	[ "${combined_result}" = "0.0" ] || FAILED_MACHINES="${FAILED_MACHINES} ${temp_machine}"
	FINAL_RESULT_ARRAY[$cur_index]=$combined_result

done

function copy_destination_files()
{
	# Arg=index.
	local  "local_index=$1"
	
	if [ $local_index -lt ${#BUILD_MACHINES[@]} ]
	then
		local machine_name=${BUILD_MACHINES[$local_index]}

		MODES_TO_BUILD=
		build_release && MODES_TO_BUILD="$MODES_TO_BUILD RELEASE"
		build_debug && MODES_TO_BUILD="$MODES_TO_BUILD DEBUG"

		if [ "x$MODES_TO_BUILD" != "x" ]; then
			for mode in $MODES_TO_BUILD
			do
				output_directory=`cat ${LOG_DIRECTORY}/${machine_name}.log | tr '[\0]' '[!]' | grep ^OW_OUTPUT_DIRECTORY_${mode} 2>/dev/null | sed "s,OW_OUTPUT_DIRECTORY_${mode}=\(.*\),\1,g" | tail -n1`
	
				# The error reporting for this has already been done, so just check to see if the output is not "none".
				if [ ! -z "$output_directory" ]
				then		
					if [ "$output_directory" != "none" ]
					then
						# Copy the files to the local directory.
						eval mkdir -p \$PACKAGE_LOCATION_${mode}/${PACKAGE_OUTPUT_DIRECTORIES[$local_index]}
						eval scp -r "$machine_name:$output_directory/*" \$PACKAGE_LOCATION_${mode}/${PACKAGE_OUTPUT_DIRECTORIES[$local_index]}
					fi
				     	
				fi
			done
		fi
	fi		
}



if [ "$FINAL_RESULTS" = "0" ]
then
	#
	#
	# Copy all of the platform-specific files to the CD building directory.
	#
	#
	mkdir -p $PACKAGE_LOCATION_RELEASE
	mkdir -p $PACKAGE_LOCATION_DEBUG

	for cur_index in `generate_numbers_for_loop ${#BUILD_MACHINES[@]}`
	do
		copy_destination_files $cur_index
		
		# FIXME! Do something if the files couldn't copy.
		TEMP_RESULTS=$?
	done



fi

if [ "$FINAL_RESULTS" = "0" ]
then
	PACKAGE_LOCATIONS=
	
	build_release && PACKAGE_LOCATIONS="$PACKAGE_LOCATIONS $PACKAGE_LOCATION_RELEASE"
	build_debug && PACKAGE_LOCATIONS="$PACKAGE_LOCATIONS $PACKAGE_LOCATION_DEBUG"

fi


# If they all built, 
if [ "$FINAL_RESULTS" = "0" ]; then
	if [ "$OFFICIAL_BUILD" = "1" ]; then

		EMAIL_SUBJECT="ow build $OW_BUILD_ID successful :)"
		
		# Copy build logs over...
		for machine in ${BUILD_MACHINES[@]}; do
			scp ${LOG_DIRECTORY}/${machine}.log $FINAL_DATA_DESTINATION/$OW_BUILD_ID.$machine.log
		done

		if [ -e ${LOG_DIRECTORY}/owdocs.log ]
		then
			scp ${LOG_DIRECTORY}/owdocs.log $FINAL_DATA_DESTINATION/$OW_BUILD_ID.documentation.log
		fi
		# End build log copy

		# TODO: copy the packages somewhere


		if [ "x$FAILED_MACHINES" != "x" ]
		then
			echo > $LOG-merged
			build_release && echo "The release build was attempted." >> $LOG-merged
			build_debug && echo "The debug build was attempted." >> $LOG-merged
			echo "This build was attempted on ${BUILD_MACHINES[@]}" >> $LOG-merged
			echo "The following failed, but were ignored: $FAILED_MACHINES" >> $LOG-merged
			echo >> $LOG-merged
			EMAIL_SUBJECT="ow build $OW_BUILD_ID completed with errors (ignored)"
		else
			echo "This build was completed on ${BUILD_MACHINES[@]}" > $LOG-merged
		fi
		# Put an entry in the email about the location of logs.
		echo >> $LOG-merged
		echo "===========================================================================" >> $LOG-merged
		echo "The complete build logs can be found on $FINAL_DATA_DESTINATION" >> $LOG-merged
		
		# Copy them to a common location.
		for index in `generate_numbers_for_loop ${#BUILD_MACHINES[@]}`
		do
			machine=${BUILD_MACHINES[$index]}
			echo "${PACKAGE_OUTPUT_DIRECTORIES[$index]}: $OW_BUILD_ID.$machine.log" >> $LOG-merged
			scp ${LOG_DIRECTORY}/${machine}.log $FINAL_DATA_DESTINATION/$OW_BUILD_ID.$machine.log
		done

		if [ -e ${LOG_DIRECTORY}/owdocs.log ]
		then
			scp ${LOG_DIRECTORY}/owdocs.log $FINAL_DATA_DESTINATION/$OW_BUILD_ID.documentation.log
		fi
		echo "===========================================================================" >> $LOG-merged
		echo >> $LOG-merged

		if [ "${#EXTRA_ERRORS[@]}" != "0" ]; then
			echo "Additional errors were found:" >> $LOG-merged
			for index in `generate_numbers_for_loop ${#EXTRA_ERRORS[@]}`
			do
				echo "ERROR: ${EXTRA_ERRORS[$index]}" >> $LOG-merged
			done
			echo "===========================================================================" >> $LOG-merged
			echo >> $LOG-merged
			EMAIL_SUBJECT="ow build $OW_BUILD_ID completed with errors (ignored)"
		fi

		mail -s"$EMAIL_SUBJECT" $RESULTS_EMAIL_ADDRESS < $LOG-merged
		rm $LOG-merged

		rm -rf ${LOG_DIRECTORY}
	fi

	BUILD_ERROR_BEFORE_END=0
	trap - EXIT
else
	# Print Send the emails, save the logs, etc.
	BUILD_ERROR_BEFORE_END=0
	handle_logs_for_error
fi

build_history_entry "end:success"


if [ "$OFFICIAL_BUILD" = "1" ]; then
	 scp $main_build_output_file $FINAL_DATA_DESTINATION/$OW_BUILD_ID.master.log
	 rm $main_build_output_file
fi

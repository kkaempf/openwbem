#!/bin/sh
#
### BEGIN INIT INFO
# Provides: @PACKAGE_PREFIX@owcimomd
# Required-Start: $network
# Required-Stop: $network
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Description: @PACKAGE_PREFIX@owcimomd
#       Start/Stop the OpenWBEM CIMOM Daemon
### END INIT INFO
#
#
# chkconfig: 2345 36 64
# description: OpenWBEM CIMOM Daemon
# processname: owcimomd

NAME=owcimomd
SBINDIR=@sbindir@
#SBINDIR=/opt/vintela/vmx/sbin
DAEMON=$SBINDIR/$NAME
OPTIONS=
DESCRIPTIVE=
PRODUCT_ABBREV=@product_abbreviation@
PIDFILE=@PIDFILE_DIR@/@PACKAGE_PREFIX@$NAME.pid
OSNAME=`uname | tr '[:lower:]' '[:upper:]'`
FUNCTION_FILE=
FUNCTION_FILE_REQUIRED=0
EXECUTED_FROM_CRON=${CRON_IS_RUNNING_@product_abbreviation@:=0}
CRON_EXECUTION_REQUIRED=0
# HPUX uses this to say if the client should start.
RUN_@product_abbreviation@=1
# MacOSX uses this to say if the client should start.
@product_abbreviation@=-YES-
# The parameter to 'echo' that suppresses the newline.
ECHO_NO_NEWLINE=-n
OW_STATUS_RUNNING=0
USER=INVALID_USER
QUIET_OPT=0
STARTUP_DIR=`pwd`

CheckRootUser()
{
	if [ -n "$EUID" -a "$EUID" != "0" ] || [ -n "$UID" -a "$UID" != "0" ]; then
		if id | grep 'uid=0' >/dev/null 2>/dev/null; then
			USER=root
		fi
	else
		USER=root
	fi
}

PathToExecutable()
{
	if [ $# -eq 1 ]; then
		PATH_TO_EXECUTABLE=
		if [ -f "/$1" ]; then
			PATH_TO_EXECUTABLE="`dirname \"/$1\"`"
		elif [ -f "./$1" ]; then
			PATH_TO_EXECUTABLE="$STARTUP_DIR/`dirname \"$1\"`"
		elif [ -f "$STARTUP_DIR/`dirname \"$1\"`/`basename \"$1\"`" ]; then
			PATH_TO_EXECUTABLE="$STARTUP_DIR/`dirname \"$1\"`"
		elif [ -f "`dirname \"$1\"`/`basename \"$1\"`" ]; then
			PATH_TO_EXECUTABLE="`dirname \"$1\"`"
		elif which "$1" >/dev/null 2>/dev/null; then
			temp="`which \"$1\"`"
			# which malfunctions on OSX, returning 0 when something was not found.
			# Verify the results.
			if [ -f "$temp" ]; then
				PATH_TO_EXECUTABLE="`dirname \"$temp\"`"
			fi
			unset temp
		fi
		if [ -n "$PATH_TO_EXECUTABLE" ]; then
			FULL_PATH_TO_EXECUTABLE="$PATH_TO_EXECUTABLE/`basename \"$1\"`"
			unset PATH_TO_EXECUTABLE
			if [ -f "$FULL_PATH_TO_EXECUTABLE" ] ; then
				echo $FULL_PATH_TO_EXECUTABLE
				unset FULL_PATH_TO_EXECUTABLE
				return 0
			fi
			unset FULL_PATH_TO_EXECUTABLE
		else
			unset PATH_TO_EXECUTABLE
		fi
	fi
	return 1
}

OW_GetPlatformSettings()
{
	case $OSNAME in
		LINUX)
			FUNCTION_FILE=/etc/rc.d/init.d/functions
			;;
		HPUX)
			FUNCTION_FILE=/etc/rc.config.d/@PACKAGE_PREFIX@$NAME
			# The echo command on HPUX doesn't support -n.
			ECHO_NO_NEWLINE=
			;;
		DARWIN)
			FUNCTION_FILE=/etc/rc.common
			FUNCTION_FILE_REQUIRED=1
			CRON_EXECUTION_REQUIRED=1
			;;
		AIX)
			# The echo command on AIX doesn't support -n.
			ECHO_NO_NEWLINE=
			;;
		SUNOS)
			# The echo command on Solaris doesn't support -n.
			ECHO_NO_NEWLINE=
			;;
		*)
			;;
	esac
}

DisplayOutput()
{
	if [ $EXECUTED_FROM_CRON -eq 1 ]; then
		echo "`date`: " "$@"
		return 0
	else
		if [ "$USER" = "root" ] && [ "x$OSNAME" = "xDARWIN" ]; then
			if [ $# -gt 0 ] && [ "x$1" = "x$ECHO_NO_NEWLINE" ]; then
				shift
			fi
			ConsoleMessage "$@"
			return 0;
		fi
	fi
	echo "$@"
}

OW_FatalError()
{
	DisplayOutput "$@" >&2
	exit 1
}

OW_IsNumeric()
{
	if [ $# -lt 1 ]; then
		return 1
	fi
	for parameter in "$@"; do
		if [ "x$parameter" != "x" ]; then
			local_extracted_number=`echo "$parameter" | sed 's/[^0-9]//g'`
			if [ "x$local_extracted_number" != "x$parameter" ]; then
				return 1
			fi
		else
			return 1
		fi
	done
	return 0
}

# This function exists because some Linux platforms have added broken
# shells which cannot properly handle SIGnnnn names for signals.  This
# will try various forms of the named signal, trying to make one work.
OW_SendSignal()
{
	if [ $# -ne 2 ]; then
		echo "Incorrect number of arguments ($#).  Need a signal name and a PID" >&2
		return 1
	fi

	signal=$1
	pid_to_signal=$2

	if OW_IsNumeric "$pid_to_signal" && [ $pid_to_signal -gt 0 ]; then
		:
	else
		echo "Not sending $signal: Invalid pid specified: \"$pid_to_signal\"" >&2
		return 1
	fi

	signal_replacement_list=""

	case $signal in
		SIGTERM | TERM)
			signal_replacement_list="SIGTERM TERM 15"
			;;
		SIGKILL | KILL)
			signal_replacement_list="SIGKILL KILL 9"
			;;
		SIGHUP  | HUP)
			signal_replacement_list="SUGHUP HUP 1"
			;;
		[0-9] | [0-9][0-9])
			signal_replacement_list="$signal"
			;;
		*)
			OW_FatalError "Invalid signal: $signal"
			;;
	esac
	for new_signal in $signal_replacement_list; do
		if kill -$new_signal $pid_to_signal >/dev/null 2>&1; then
			return 0
		elif kill -s $new_signal $pid_to_signal >/dev/null 2>&1; then
			return 0
		fi
	done
	echo "Failed to send signal $signal to PID $pid_to_signal."
	return 1
}

OW_TestRunning()
{
	if OW_IsNumeric $1 && [ $1 -gt 0 ]; then
		if kill -0 $1 >/dev/null 2>&1; then
			return 0
		fi
	fi
	return 1
}

# Make a temporary file while avoiding race conditions and potential symlink attacks.
# Unlike the mktemp command, this will not be guaranteed to create a unique
# name (although it usually will be, unless a name is forced); it only
# guarantees that if it returns a success code, the file (or directory) was
# created and will be empty (overwrites can and will happen).
#
# NOTE: The umask for the system is currently ignored (a side effect of
# securely creating the file).  Until some method of inverting a umask to make
# it usable for a chmod command, the file will only be readable by UID 0.
OW_MakeTemp()
{
	# Local variables...
	make_temp_use_given_name=0
	make_temp_directory=0
	make_temp_try_count=1
	make_temp_successful=0
	make_temp_dest_dir=${TMPDIR:-/tmp}
	make_temp_file_pattern=""
	make_temp_mask=`umask`
	make_temp_perms=""
	make_temp_flags=$-
	umask 077

	# Do not bail on errors!
	set +e

	# Parse the given options.
	while [ $# -gt 0 ]; do
		case "$1" in
			-d)
				make_temp_directory=1
				;;
			-f)
				make_temp_use_given_name=1
				;;
			-r)
				make_temp_try_count=$2
				shift
				;;
			-p)
				make_temp_perms="$2"
				shift
				;;
			*)
				make_temp_file_pattern="$1"
				;;
		esac
		shift
	done


	make_temp_base=`basename "$make_temp_file_pattern"`
	make_temp_chosen_dir=`dirname "$make_temp_file_pattern"`

	# Retry as many times as requested until there is a successful creation.
	while [ $make_temp_try_count -gt 0 ] && [ $make_temp_successful -eq 0 ]; do
		make_temp_try_count=`expr $make_temp_try_count - 1`
		make_temp_target_suffix="${RANDOM}.${RANDOM}.${RANDOM}.${make_temp_try_count}.$$"
		make_temp_temp_dir="${make_temp_dest_dir}/temp_dir.${make_temp_base}.${make_temp_target_suffix}"
		make_temp_created_file=""
		rm -rf $make_temp_temp_dir
		mkdir $make_temp_temp_dir >/dev/null 2>&1
		if [ $? -eq 0 ]; then
			# Made a directory.
			if [ $make_temp_directory -eq 1 ]; then
				make_temp_created_file="${make_temp_dest_dir}/${make_temp_base}.${make_temp_target_suffix}"
				rm -rf "$make_temp_created_file"
				mkdir -p "`dirname \"$make_temp_created_file\"`"
				mv -f "$make_temp_temp_dir" "$make_temp_created_file"
				if [ $? -eq 0 ]; then
					:
				else
					echo "OW_MakeTemp: Unable to make directory.  A running attack is possible." >&2
					# Clean up some before aborting.
					rm -rf "$make_temp_temp_dir"
					break
				fi
			else
				make_temp_created_file="$make_temp_temp_dir/${make_temp_base}.${make_temp_target_suffix}"
				# Solaris doesn't support the "set -o noclobber" functionality..
				# This isn't as secure as the noclobber method, but broken
				# shells must be dealt with.
				if [ -f "$make_temp_created_file" ]; then
					# Clean up some before trying again.
					rm -rf "$make_temp_temp_dir"
					continue
				else
					# Make a zero-byte file.
					cat <<EOF > "$make_temp_created_file"
EOF
				fi
			fi

			# At this point, we are guaranteed to have a unique (and safe) file
			# and/or directory which has a umask set for the current users use.
			# That is, of course, unless an attack is running with the UID the
			# script is currently running with.

			# If file permissions were specified, set them.
			if [ "x$make_temp_perms" != "x" ]; then
				chmod "$make_temp_perms" "$make_temp_created_file"
				if [ $? -eq 0 ]; then
					:
				else
					echo "OW_MakeTemp: Unable to set chosen permissions ($make_temp_perms) for temp file." >&2
					# Clean up some before aborting the loop.
					rm -rf $make_temp_created_file
					rm -rf $make_temp_temp_dir
					break
				fi
			fi

			# Move the created file into the temp directory now...

			if [ $make_temp_use_given_name -eq 1 ]; then
				# Rename the file to be exactly what they specified.
				make_temp_destfile="${make_temp_chosen_dir}/${make_temp_base}"
				rm -rf "$make_temp_destfile"
				mkdir -p "`dirname \"$make_temp_destfile\"`"
				mv -f "$make_temp_created_file" "$make_temp_destfile"
				if [ $? -eq 0 ]; then
					make_temp_created_file="$make_temp_destfile"
				else
					echo "OW_MakeTemp: Unable to create target: $make_temp_destfile" 2>&1
					make_temp_created_file=""
				fi
				unset make_temp_destfile
			elif [ $make_temp_directory -eq 0 ] ; then
				make_temp_destfile="$make_temp_chosen_dir/`basename $make_temp_created_file`"
				rm -rf "$make_temp_destfile"
				mkdir -p "$make_temp_chosen_dir"
				mv -f "$make_temp_created_file" "$make_temp_chosen_dir"
				if [ $? -eq 0 ]; then
					make_temp_created_file="$make_temp_destfile"
				else
					echo "OW_MakeTemp: Unable to create target: ${make_temp_chosen_dir}/`basename ${make_temp_created_file}`" 2>&1
					make_temp_created_file=""
				fi
				unset make_temp_destfile
			else
				# An exact match was not requested, so we'll just use what we have.
				# Things are always safer this way.
				:
			fi

			if [ "x$make_temp_created_file" != "x" ] ; then
				make_temp_successful=1
				echo "$make_temp_created_file"
				rm -rf "$make_temp_temp_dir"
			else
				# Something above failed, so we'll just clean up, and retry if possible.
				rm -rf "$make_temp_temp_dir"
			fi

		else
			# FIXME! What to do upon failure?
			:
		fi
	done

	# Clean up.
	set -$make_temp_flags
	umask $make_temp_mask
	unset make_temp_flags
	unset make_temp_mask
	unset make_temp_base
	unset make_temp_temp_dir
	unset make_temp_use_given_name
	unset make_temp_directory
	unset make_temp_try_count
	unset make_temp_dest_dir
	unset make_temp_chosen_dir
	unset make_temp_created_file
	unset make_temp_file_pattern
	unset make_temp_perms
	unset make_temp_target_suffix

	if [ $make_temp_successful -eq 0 ]; then
		echo "OW_MakeTemp: Failed to create desired file or directory."
		unset make_temp_successful
		return 1
	else
		unset make_temp_successful
		return 0
	fi
}

# Safely create a file with the name <arg1>.
OW_SafeCreateFile()
{
	OW_MakeTemp -f "$1" >/dev/null
	if [ $? -eq 0 ]; then
		:
	else
		echo "Unable to create \"$1\": A running local symlink (or other) attack is possible." >&2
		return 1
	fi
}

# This is mainly for OSX, which will fail for installation (using hdiutil) if
# the install script is detached.  This is NOT to be used recursively, in
# parallel or in any other case where there could be multiple cron entries
# installed at the same time.
#
# A --pattern option may be specified, which will use the supplied argument as
# a tag in creating the cron entry.
OW_CreateCronEntry()
{
	results=1

	local_cron_pattern="GENERIC"

	if [ $# -gt 1 ]; then
		if [ "x$1" = "x--pattern" ]; then
			local_cron_pattern="$2"
			shift 2
		fi
	fi

	if temp_file=`OW_MakeTemp /tmp/root_cronXXXXXX`; then
		if crontab -l -u root >/dev/null 2>&1; then
			[ $QUIET_OPT -eq 1 ] || echo "Adding a cron entry with tag \"${local_cron_pattern}\""
			if crontab -l -u root 2>/dev/null | sed "/# BEGIN_VINTELA_INSTALLER_TAG_${local_cron_pattern}/,/# END_VINTELA_INSTALLER_TAG_${local_cron_pattern}/d" > $temp_file; then
				:
			else
				[ $QUIET_OPT -eq 1 ] || echo "Filtered crontab retrieval failed: \"${local_cron_pattern}\""
				rm $temp_file
				return 1
			fi
		else
			[ $QUIET_OPT -eq 1 ] || echo "Creating new crontab with tag \"${local_cron_pattern}\""
		fi

		# Create a crontab entry...
		echo "# BEGIN_VINTELA_INSTALLER_TAG_${local_cron_pattern}" >> $temp_file
		echo 'CRON_IS_RUNNING_@product_abbreviation@=1' >> $temp_file
		echo "*/1 * * * *  " "$@" >> $temp_file
		echo "# END_VINTELA_INSTALLER_TAG_${local_cron_pattern}" >> $temp_file

		if crontab -u root $temp_file; then
			results=$?
			[ $QUIET_OPT -eq 1 ] || echo "Crontab entry added with tag \"${local_cron_pattern}\""
		else
			results=$?
			echo "Failed to install new crontab."
		fi
		rm -f $temp_file
	fi
	unset local_cron_pattern
	return $results
}

# This reverses the operations done in the CreateCronEntry function.
OW_RemoveCronEntry()
{
	results=1

	local_cron_pattern="GENERIC"

	if [ $# -gt 1 ]; then
		if [ "x$1" = "x--pattern" ]; then
			local_cron_pattern="$2"
			shift 2
		fi
	fi

	# Remove any existing crontab entry.
	if temp_file=`OW_MakeTemp /tmp/root_cronXXXXXX`; then
		if crontab -l -u root >/dev/null 2>&1; then
			if crontab -l -u root 2>/dev/null | grep "BEGIN_VINTELA_INSTALLER_TAG_${local_cron_pattern}" >/dev/null 2>&1; then
				if crontab -l -u root 2>/dev/null | sed "/# BEGIN_VINTELA_INSTALLER_TAG_${local_cron_pattern}/,/# END_VINTELA_INSTALLER_TAG_${local_cron_pattern}/d" > $temp_file ; then
					[ $QUIET_OPT -eq 1 ] || echo "Removing crontab entry with tag \"${local_cron_pattern}\""
					if crontab -u root $temp_file ; then
						results=0
						[ $QUIET_OPT -eq 1 ] || echo "Crontab entry removed: \"${local_cron_pattern}\""
					else
						results=$?
						echo "Failed to install new crontab."
					fi
				else
					echo "Failed to filter tag from crontab: \"${local_cron_pattern}\"."
				fi
			else
				[ $QUIET_OPT -eq 1 ] || echo "Non existent crontab entry: \"${local_cron_pattern}\"."
			fi
		else
			[ $QUIET_OPT -eq 1 ] || echo "Cannot remove crontab entry: No crontab installed"
		fi
		rm -f $temp_file
	fi
	unset local_cron_pattern
	return $results
}

OW_CimomdEnabled()
{
	if [ $RUN_@product_abbreviation@ -ne 0 ] && [ "${@product_abbreviation@:=-NO-}" = "-YES-" ]; then
		return 0
	else
		return 1
	fi
}

# Run 'ps' using the various commandlines needed (displays all process IDs for $NAME).
OW_ExecutePS()
{
	# This works on:
	# (args) AIX, Solaris, HPUX, Linux
	# (-ax,command) Linux, MacOSX
	correct_daemon_pattern="$DAEMON"
	if [ "x$OSNAME" = "xDARWIN" ]; then
		# On OSX, the daemon name is prepended with a '.' to avoid issues with
		# DYLD_LIBRARY_PATH.
		#
		# BUG_LOCATION_POINT: If things fail to work in the future, this is a
		# likely spot to check.
		correct_daemon_pattern="$SBINDIR/\\.$NAME"
	fi
	for all_process_flag in "" -ax ; do
		for command_type in args command; do
			if UNIX95=1 ps $all_process_flag -eo pid,$command_type >/dev/null 2>&1; then
				if local_pids=`UNIX95=1 ps $all_process_flag -eo pid,$command_type 2>/dev/null | grep $correct_daemon_pattern | grep -v grep | awk '{ print $1; }'` ; then
					if [ -n "$local_pids" ]; then
						echo $local_pids
						return 0
					fi
				fi
			fi
		done
	done
	return 1
}

# Run ps to determine if we are running.
OW_GetStatusFromPS()
{
	if OW_CimomdEnabled ; then
		startup_type=""
		enabled_text="enabled"
	else
		startup_type=" (manual startup)"
		enabled_text="disabled"
	fi

	if local_pids=`OW_ExecutePS 2>/dev/null`; then
		echo "$DESCRIPTIVE [$NAME] ($local_pids) is running${startup_type}."
		return 0
	else
		echo "$DESCRIPTIVE [$NAME] is $enabled_text and not running."
		return 3
	fi
}

# Use the PID file to determine if we are running.
OW_GetStatus()
{
	if OW_CimomdEnabled ; then
		startup_type=""
		enabled_text="enabled"
	else
		startup_type=" (manual startup)"
		enabled_text="disabled"
	fi

	if [ -s $PIDFILE ]; then
		PID=`cat $PIDFILE 2>/dev/null`
		if OW_TestRunning $PID ; then
			echo "$DESCRIPTIVE [$NAME] ($PID) is running${startup_type}."
			return 0
		else
			echo "$DESCRIPTIVE [$NAME] ($PID) is dead${startup_type}, but $PIDFILE exists."
			return 1
		fi
	else
		echo "$DESCRIPTIVE [$NAME] is $enabled_text and not running."
		return 3
	fi
}

OW_Status()
{
	quiet_status_checking=0
	if [ $# -gt 0 ] && [ "x$1" = "x--quiet" ]; then
		quiet_status_checking=1
	fi
	normal_status_text=`OW_GetStatus`
	normal_status=$?
	if [ $normal_status -eq 0 ]; then
		[ $quiet_status_checking = 1 ] || DisplayOutput "$normal_status_text"
		return $normal_status
	fi
	ps_status_text=`OW_GetStatusFromPS`
	ps_status=$?
	if [ $ps_status -eq 0 ]; then
		[ $quiet_status_checking = 1 ] || DisplayOutput "$ps_status_text"
		return $ps_status
	fi
	[ $quiet_status_checking = 1 ] || DisplayOutput "$normal_status_text"
	return $normal_status
}

OW_Start()
{
	if OW_CimomdEnabled ; then
		:
	else
		DisplayOutput "$DESCRIPTIVE is disabled: Refusing to start."
		return 1
	fi

	status_text=`OW_Status`
	status_code=$?
	if [ $status_code -eq 0 ] ; then
		DisplayOutput "Cannot start: $status_text"
		return 0
	fi
	if [ -f $PIDFILE ] ; then
		DisplayOutput "Stale $NAME pid file ($PIDFILE) found. Removing."
		rm -f $PIDFILE
	fi
	if [ $CRON_EXECUTION_REQUIRED = "0" ] ; then
		DisplayOutput $ECHO_NO_NEWLINE "Starting the $DESCRIPTIVE"
		$DAEMON $OPTIONS
		echo "."
		OW_Status
		return $?
	else
		if [ $EXECUTED_FROM_CRON != "0" ]; then
			# We're really running from cron.  Remove the entry.
			OW_RemoveCronEntry --pattern INIT_SCRIPT_@product_abbreviation@
			$DAEMON $OPTIONS
			sleep 3
			chmod +r $PIDFILE
			OW_Status
			return $?
		else
			# We need to start up this script with cron to ensure that the cimom
			# (or providers contained therein) do not malfunction in horrible ways
			# when detached (eg. executing hdiutil on MacOSX).
			if path_to_script=`PathToExecutable $0`; then
				:
			else
				DisplayOutput "Failed to locate the script.  Cannot install cron entry."
				return 1
			fi
			OW_CreateCronEntry --pattern INIT_SCRIPT_@product_abbreviation@ "$path_to_script" start
			DisplayOutput "The cron entry should start up the cimom within 1 minute."
			seconds_to_wait=70
			cimom_status=1
			dot_string="..."
			while [ $seconds_to_wait -ge 0 ] && [ $cimom_status -ne 0 ]; do
				seconds_to_wait=`echo $seconds_to_wait - 5 | bc`
				echo -e -n "\rWaiting$dot_string"
				dot_string="${dot_string}."
				sleep 5
				OW_Status --quiet
				cimom_status=$?
			done
			echo ""
			OW_Status --quiet
			cimom_status=$?
			if [ $cimom_status -ne 0 ]; then
				OW_RemoveCronEntry --pattern INIT_SCRIPT_@product_abbreviation@
			fi
			# Noisily print the status now.
			OW_Status
			cimom_status=$?
			if [ $cimom_status -ne 0 ]; then
				DisplayOutput "$DESCRIPTIVE did not start."
			fi
			return $cimom_status
		fi
	fi
	# This shouldn't be reached.
	return 1
}

OW_Stop()
{
	status_text=`OW_Status`
	status_code=$?
	if [ $status_code -ne 0 ] ; then
		DisplayOutput "Cannot stop: $status_text"
		return 0
	fi

	# Stop daemons.
	DisplayOutput "Shutting down $DESCRIPTIVE..."
	if [ $CRON_EXECUTION_REQUIRED -eq 1 ]; then
		# Remove the cron entry (just to be safe).
		OW_RemoveCronEntry --pattern INIT_SCRIPT_@product_abbreviation@ || true
	fi
	if OW_Status --quiet ; then
		DisplayOutput "Stopping $NAME"
		for PID in `cat $PIDFILE 2>/dev/null` `OW_ExecutePS`; do
			for death_signal in TERM KILL; do
				# Wait for up to 5 minutes for the cimom to shut down...
				max_delay=300
				DisplayOutput "Sending $NAME ($PID) a $death_signal signal"
				DisplayOutput "This may take up to $max_delay seconds."
				OW_SendSignal $death_signal $PID
				# expr returns 1 (error) when the arguments evaluate to 0
				while max_delay=`expr $max_delay - 1`; do
					sleep 1
					if OW_Status --quiet ; then
						:
					else
						break
					fi
				done
				if OW_Status --quiet ; then
					DisplayOutput "Signal $death_signal had no effect."
				else
					DisplayOutput "$NAME is no longer running."
					return 0
				fi
			done
		done
		if OW_Status --quiet ; then
			DisplayOutput "Could not stop $NAME"
			return 1
		fi
	else
		# Repeat the status message.
		DisplayOutput "Cannot stop: `OW_Status`"
		return 0
	fi
}

OW_Reload()
{
	if [ -r $PIDFILE ]; then
		PID=`cat $PIDFILE 2>/dev/null`
		if OW_TestRunning $PID; then
			DisplayOutput $ECHO_NO_NEWLINE "Reloading $DESCRIPTIVE"
			if OW_SendSignal HUP $PID; then
				sleep 3
				echo "."
				OW_Status
				return $?
			else
				DisplayOutput "Failed to send $DESCRIPTIVE a HUP signal"
				return 1
			fi
		fi
	fi
	if OW_Status --quiet; then
		PIDS=`OW_ExecutePS`
		if [ $? -eq 0 ]; then
			DisplayOutput $ECHO_NO_NEWLINE "Reloading $DESCRIPTIVE"
			failed_pids=""
			for pid in $PIDS; do
				if OW_SendSignal HUP $pid; then
					echo $ECHO_NO_NEWLINE "($pid)."
				else
					echo $ECHO_NO_NEWLINE "($pid)!"
					failed_pids="$failed_pids $pid"
				fi
			done
			echo "."
			if [ -n "$failed_pids" ]; then
				DisplayOutput "Failed to send HUP to $DESCRIPTIVE: $failed_pids"
				DisplayOutput "These may be threads that were terminated before the HUP could be sent."
			fi
			sleep 3
			OW_Status
			return $?
		else
			DisplayOutput "Cannot reload $DESCRIPTIVE: Failed to determine required PIDs."
			return 2
		fi
	else
		DisplayOutput "Cannot reload $DESCRIPTIVE: Not running"
		OW_Status --quiet
		return $?
	fi
}


# OSX (Darwin) Specific functions:
StartService()
{
	OW_Start
	return $?
}
StopService()
{
	OW_Stop
	return $?
}
RestartService()
{
	OW_Stop
	OW_Start
	return $?
}

OW_UsageError()
{
	OW_FatalError "Usage: `basename $0` {[re]start|stop|reload|force-reload|status}"
}

OW_Main()
{
	if [ $# -ne 1 ]; then
		# Exit with an error about usage.
		OW_UsageError
	fi
	# Set the current working directory
	cd /
	CheckRootUser
	if [ "$USER" = "root" ]; then
		:
	else
		DisplayOutput "This script must be run as root."
		exit 1
	fi

	if [ "x$DESCRIPTIVE" = "x" ]; then
		DESCRIPTIVE="OpenWBEM CIMOM Daemon"
	fi

	[ -x $DAEMON ] || OW_FatalError "Daemon file \"$DAEMON\" not found for $DESCRIPTIVE."

	# The functions start, stop, and restart are supposed to be executed through
	# "RunService" on Darwin.
	if [ "x$OSNAME" = "xDARWIN" ]; then
		case "$1" in
			start | stop | restart)
				RunService "$1"
				return $?
				;;
			*)
				;;
		esac
	fi

	# This should *hopefully* be sufficient on every other platform (and for the
	# options not handled in the specific cases listed above (eg. Darwin).
	case "$1" in
		start)
			OW_Start
			return $?
			;;
		stop)
			OW_Stop
			return $?
			;;
		restart|force-reload)
			OW_Stop
			OW_Start
			return $?
			;;
		reload)
			OW_Reload
			return $?
			;;
		status)
			OW_Status
			return $?
			;;
		# {start,stop}_msg came from the HPUX init script.
		start_msg)
			DisplayOutput "Starting $DESCRIPTIVE"
			;;
		stop_msg)
			DisplayOutput "Terminating $DESCRIPTIVE"
			;;
		*)
			# Exit with an error about usage.
			OW_UsageError
			;;
	esac
	return 0
}

###############################################################################
# Stuff that must be done outside of main (sourcing other files, etc.).
###############################################################################
OW_GetPlatformSettings

# Eventually, this should be removed.  At least for now, it's useful.
#
# If this is removed, the DisplayOutput function must be fixed to not have any
# output (cron doesn't like output to stdout or stderr.), or fds 1&2 need to be
# closed.
if [ $EXECUTED_FROM_CRON != "0" ]; then
	set -x
	CRON_STARTUP_LOG_NAME=`OW_MakeTemp /tmp/${NAME}_cron_startup.log`
	if [ $? -eq 0 ]; then
		exec 1>>$CRON_STARTUP_LOG_NAME
		exec 2>&1
	fi
fi

# Source function library.
if [ -n "$FUNCTION_FILE" ] && [ -f $FUNCTION_FILE ]; then
	. $FUNCTION_FILE
fi

OW_Main "$@"
exit $?

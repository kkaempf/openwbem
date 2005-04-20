#!/bin/bash

# This script is meant to be sourced from another script.  It MUST be done
# before any local overriding of the sigs TERM, INT, HUP, PIPE, or ALRM.
#
# If you want anything special to happen, you can do this three ways:
# (1) Set the TERM_HANDLER_COMMAND1 env var to something you want to execute
#     when the script is to be killed (HUP, INTR, TERM, PIPE, ALRM), but before
#     killing subprocesses.
# (2) Set the TERM_HANDLER_COMMAND2 env var to something you want to execute
#     after the subprocesses are killed.
# (3) Set an exit handler for the EXIT trap.  This will be (temporarily)
#     cleared before sub-processes are killed, and re-enabled (if it existed)
#     before the handler exits. 

term_handler_unexport()
{
	export -n $* 2>/dev/null || typeset +x $*
}

# ===========================================================================
# This is the main term handler function.
# ===========================================================================
term_handler_execute()
{
	TRAP_LOCAL_ERROR_CODE=$?

	# =====================================================================
	# Save the old settings (-u, -e, etc)
	# =====================================================================
	TRAP_LOCAL_SETTINGS=$-

	# Turn of exit on error, as some of the commands in this function may 
	# return error codes
	set +e
	
	# Turn off the error on undefined, as this script uses a few externally
	# defined variables (if they are defined).
	set +u

	# =====================================================================
	# Save the old trap handlers
	# =====================================================================
	TRAP_LOCAL_REENABLE_SCRIPT=/tmp/trap_commands-$$.sh
	trap > $TRAP_LOCAL_REENABLE_SCRIPT

	cat $TRAP_LOCAL_REENABLE_SCRIPT

	# =====================================================================
	# Save any preexisting exit handler.
	# =====================================================================
	TRAP_EXIT_HANDLER_RESTORE=`grep EXIT $TRAP_LOCAL_REENABLE_SCRIPT`

	# Remove the temporary file.
	rm -f $TRAP_LOCAL_REENABLE_SCRIPT

	# Unset the existing trap handlers for TERM INT HUP EXIT (ignore them).
	trap "" TERM INT HUP EXIT PIPE ALRM ABRT

	# The default action is to send the term signal.
	TERM_SIGNAL=term
	if [ $TRAP_LOCAL_ERROR_CODE -eq 0 ]
	then
		TRAP_LOCAL_ERROR_CODE=127
	else
		# FIXME! If processes need to act differently for different
		# signals, add a case for them.  Otherwise, they will get the
		# TERM signal. 
		local TRAP_NUMBER
		let "TRAP_NUMBER=$TRAP_LOCAL_ERROR_CODE-128"
		case $TRAP_LOCAL_ERROR_CODE in 
			1 ) TERM_SIGNAL=hup ;;  ## Hangup
			2 ) TERM_SIGNAL=term ;; ## Interrupt
			5 ) TERM_SIGNAL=trap ;; ## Trap
			6 ) TERM_SIGNAL=abrt ;; ## Abort
			13) TERM_SIGNAL=term ;; ## Pipe
			14) TERM_SIGNAL=alrm ;; ## Alarm
			15) TERM_SIGNAL=term ;; ## Term
		esac
	fi
	echo "TERM_SIGNAL=$TERM_SIGNAL"



	# =====================================================================
	# Run a script, function, or other command before killing the subprocesses.
	# =====================================================================
	if [ ! -z "$TERM_HANDLER_COMMAND1" ]
	then
		# Restore most of the settings (not the -e).
		set -$TRAP_LOCAL_SETTINGS
		set +e
		# Run their command or function.
		eval "$TERM_HANDLER_COMMAND1"
		TRAP_LOCAL_ERROR_CODE=$?
		set +u
	else
		echo "(Term Handler) TERM requested.  Killing subprocesses..."
	fi

	# =====================================================================
	# Kill the subprocesses
	# =====================================================================

	# Kill the subprocesses (at least, send a kill request).
	if [ -n "$TERM_SIGNAL" ]; then
		echo "Sending $TERM_SIGNAL signal to all subprocesses"
		# If the shell doesn't understand the kill -SIGNAL syntax, use
		# the default kill.
		kill -$TERM_SIGNAL 0 2>/dev/null || kill 0
	else
		echo "Sending term signal to all subprocesses"
		kill 0
	fi

	# Wait for them to exit.
	wait

	# =====================================================================
	# Run a script, function, or other command (the subprocesses should be dead).
	# =====================================================================
	if [ ! -z "$TERM_HANDLER_COMMAND2" ]
	then
		# Restore most of the settings (not the -e).
		set -$TRAP_LOCAL_SETTINGS
		set +e
		# Run their command or function.
		eval "$TERM_HANDLER_COMMAND2"
		TRAP_LOCAL_ERROR_CODE=$?
		set +u
	else
		echo "(Term Handler) Subprocesses killed."
	fi

	if [ -z "$INSIDE_SAFE_EXECUTE_DO_NOT_EXIT" ]
	then
		# Unset (to the defaults) the SIG, INT, HUP signals
		trap - TERM INT HUP PIPE ALRM ABRT

		# Restore the exit handler
		if [ "x$TRAP_EXIT_HANDLER_RESTORE" != "x" ]
		then
			eval "$TRAP_EXIT_HANDLER_RESTORE"
		else
			trap - EXIT
		fi

		# =====================================================================
		# Exit with an appropriate error code
		# =====================================================================
		exit $TRAP_LOCAL_ERROR_CODE	
	else
		# Restore the exit handler
		if [ ! -z "x$TRAP_EXIT_HANDLER_RESTORE" ]
		then
			eval "$TRAP_EXIT_HANDLER_RESTORE"
		else
			trap - EXIT
		fi

		set -$TRAP_LOCAL_SETTINGS
		return $TRAP_LOCAL_ERROR_CODE
	fi
} # term_handler_execute()

# Execute a program in the background, and wait on it.  This is needed to
# safely kill a process and have its children killed as well.  It also sets a
# variable to prevent any traps from exiting the script.
#
# This version should only be called when something needs to happen after a
# term (intr, etc) has been requested.  Otherwise just use safe_execute.
safe_execute_noexit()
{
	INSIDE_SAFE_EXECUTE_DO_NOT_EXIT=true
	# Make sure the flag isn't exported.
	term_handler_unexport INSIDE_SAFE_EXECUTE_DO_NOT_EXIT
	"$@" &
	echo "Waiting for '$@' to finish."
	local foo=$-
	set +e
	wait $!
	safe_execute_result=$?
	set -$foo
	echo "Finished executing '$@'. (error code $safe_execute_result)"
	unset INSIDE_SAFE_EXECUTE_DO_NOT_EXIT
	return $safe_execute_result
} 

# Execute a program in the background, and wait on it.  This is needed to
# safely kill a process and have its children killed as well.
safe_execute()
{
	"$@" &
	echo "Waiting for '$@' to finish."
	local foo=$-
	set +e
	wait $!
	safe_execute_result=$?
	set -$foo
	echo "Finished executing '$@'. (error code $safe_execute_result)"
	return $safe_execute_result
} 

# This function should rarely be used (the cron build script, where output must
# be emailed is an example).
disable_exit_on_signals()
{
	INSIDE_SAFE_EXECUTE_DO_NOT_EXIT=true
	# Make sure the flag isn't exported.
	term_handler_unexport INSIDE_SAFE_EXECUTE_DO_NOT_EXIT
}

enable_exit_on_signals()
{
	INSIDE_SAFE_EXECUTE_DO_NOT_EXIT=FOOBAR
	unset INSIDE_SAFE_EXECUTE_DO_NOT_EXIT
}

# ===========================================================================
# Enable the term hanler for various signals
# ===========================================================================
trap term_handler_execute TERM INT HUP PIPE ALRM ABRT

# Just in case the flag somehow got set, disable it now.  A user MUST enable it
# after the execution of this script.
enable_exit_on_signals

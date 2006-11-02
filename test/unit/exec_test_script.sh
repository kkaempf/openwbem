#!/bin/sh

# This script is used to test execution timeouts, with processes that spawn other processes.
#
# Parameters:
# $1 = { exit, wait_child, wait_pipe, sleep, <anything> }
#    exit = parent quits without waiting for child (the child becomes an orphan)
#    wait_child = parent waits for the child to quit.
#    wait_pipe = parent waits for the input pipe to be closed, signals the child, then waits for the child to quit.
#    sleep = parent sleeps.  No waiting for children, pipes, or anything else is done.
#    <anything> = parent defaults to waiting on child
# $2 = { ignore, <anything> }
#    ignore = parent process will ignore signals (including SIGTERM)
#    <anything> = parent defaults to accepting signals (and quitting).

# When executed, this script outputs the path to the file containing the useful
# output.  Everything else is sent to this file.  The reason for doing so is to
# be able to monitor what the child is doing even after the parent quits.

normal_delay=60

OW_OUTPUT_FILE=${OW_OUTPUT_FILE:-/tmp/ow_test_exec.$$}
export OW_OUTPUT_FILE

have_parent=${ALREADY_HAVE_PARENT:-0}
ALREADY_HAVE_PARENT=${ALREADY_HAVE_PARENT:-1}
export ALREADY_HAVE_PARENT

if [ x${2:-ignore} = xignore ]; then
	parent_ignore_term=1
else
	parent_ignore_term=0
fi


child()
{
	if [ ${have_parent} = 1 ]; then
		return 0
	fi
	return 1
}

type_text=Parent
if child; then
	type_text=Child
fi



print_output()
{
	echo "($type_text: $$ - `date +%Y%m%d%H%M%S`)" "$@" >> ${OW_OUTPUT_FILE}
}

term_handler()
{
	LOCAL_ERROR_CODE=$?
	if [ $LOCAL_ERROR_CODE -eq 0 ]; then
		LOCAL_ERROR_CODE=127
	else
		TRAP_NUMBER=${LOCAL_ERROR_CODE}
		if [ ${TRAP_NUMBER} -gt 128 ]; then
			TRAP_NUMBER=`expr $LOCAL_ERROR_CODE - 128`
		fi
		case $TRAP_NUMBER in
			1 ) TERM_SIGNAL=hup ;;  ## Hangup
			2 ) TERM_SIGNAL=term ;; ## Interrupt
			5 ) TERM_SIGNAL=trap ;; ## Trap
			6 ) TERM_SIGNAL=abrt ;; ## Abort
			13) TERM_SIGNAL=term ;; ## Pipe
			14) TERM_SIGNAL=alrm ;; ## Alarm
			15) TERM_SIGNAL=term ;; ## Term
			 *) TERM_SIGNAL="unknown($TRAP_NUMBER)" ;; # Something else.
		esac
	fi
	print_output "${type_text} received signal: ${TERM_SIGNAL}"

	if child; then
		print_output "${type_text} terminating sleep."
		kill $!
		wait $!
		print_output "${type_text} sleep terminated."
		exit 0
	elif [ ${parent_ignore_term} = 1 ]; then
		print_output "${type_text} ignoring signal."
		return 0
	fi

	exit ${LOCAL_ERROR_CODE}
}

shell_exited()
{
	print_output "${type_text} shell quitting."
}

trap term_handler TERM INT HUP PIPE ALRM ABRT
trap shell_exited EXIT


wait_for_child()
{
	print_output "Parent waiting for child"
	wait $!
	print_output "Parent done waiting for child"
}

if child; then
	print_output "Child starting."
	sleep ${normal_delay} &
	wait $!
	print_output "Child finished"
	exit 0
else
	echo "${OW_OUTPUT_FILE}"
	echo "($$ - `date +%Y%m%d%H%M%S`) Parent starting." > ${OW_OUTPUT_FILE}

	"$0" "$@" &

	ending_method=${1:-wait_child}

	if [ ${ending_method} = exit ]; then
		print_output "Parent quitting (child continues)"
		exit 0
	elif [ ${ending_method} = wait_child ]; then
		wait_for_child
		exit 0
	elif [ ${ending_method} = wait_pipe ]; then
		print_output "Parent reading from pipe"
		while read foo; do
			:
		done
		print_output "Parent pipe closed"
		print_output "Sending term to child."
		kill $!
		wait_for_child
		exit 0
	elif [ ${ending_method} = sleep ]; then
		print_output "Parent sleeping."
		# Do this one second at a time instead of as a block so that the parent doesn't quit too early.
		while [ ${normal_delay} -gt 0 ]; do
			sleep 1
			normal_delay=`expr ${normal_delay} - 1`
		done
		print_output "Parent done sleeping."
		exit 0
	else
		print_output "Don't know what to do... Waiting for child anyway."
		wait_for_child
		exit 0
	fi
fi

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
DAEMON=@sbindir@/$NAME
OPTIONS=
PIDFILE=/var/run/@PACKAGE_PREFIX@$NAME.pid

FUNCTION_FILE=/etc/rc.d/init.d/functions
SUSE=0

ret=0

# Set the current working directory
cd /

if [ $EUID != 0 ]; then
 echo "This script must be run as root."
 exit 1;
fi

# Source function library.
if [ -f $FUNCTION_FILE ]; then
 . $FUNCTION_FILE
else
 SUSE=1
fi

if [ "$DESCRIPTIVE" = "" ]; then
 DESCRIPTIVE="OpenWBEM CIMOM Daemon"
fi

lockfile=${SVIlock:-/var/lock/subsys/$NAME}

[ -x $DAEMON ] || exit 0

ow_fatal_error()
{
  echo "$@" >&2
  exit 1
}

# This function exists because some linux platforms have added broken
# shells which cannot properly handle SIGnnnn names for signals.  This
# will try various forms of the named signal, trying to make one work.
ow_send_signal()
{
  if [ $# -ne 2 ]; then
    echo "Incorrect number of arguments ($#).  Need a signal name and a PID"
    return 1
  fi

  signal=$1
  pid_to_signal=$2

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
      ow_fatal_error "Invalid signal: $signal" 
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

ow_test_running()
{
  if kill -0 $@ >/dev/null 2>&1; then
    return 0
  fi
  return 1
}

# See how we were called.
case "$1" in
 start)
  if [ -s $PIDFILE ]; then
    PID=`cat $PIDFILE`

    if ow_test_running $PID; then
      echo "$NAME ($PID) is already running."
      exit 1
    else
      echo "Stale $NAME pid file ($PIDFILE) found. Removing."
      rm -f $PIDFILE
    fi
  fi


  # Start daemons.
  echo -n "Starting the $DESCRIPTIVE"
  $DAEMON $OPTIONS
  ret=$?
  touch $lockfile
  echo "."
  ;;

 stop)

  if [ ! -s $PIDFILE ]; then
   exit 0;
  fi

  # Stop daemons.
  echo -n "Shutting down $DESCRIPTIVE"

  PID=`cat $PIDFILE`

  ow_send_signal TERM $PID || ow_fatal_error "Could not stop $DESCRIPTIVE"
  echo -n "."

  for num in 1 2 3 4 5 6 7
  do
    if ow_test_running $PID
    then
      echo -n "."
    else
      echo
      rm -f $lockfile
      echo "Exited"
      exit 0;
    fi
    sleep 2
  done
  if ow_test_running $PID
  then
    ow_send_signal KILL $PID || ow_fatal_error "Could not forcefully kill $DESCRIPTIVE"
    sleep 2
    if [ -e $PIDFILE ]
    then
      rm -f $PIDFILE 2> /dev/null
    fi
  fi

  echo
  rm -f $lockfile
  echo "Terminated"
  ret=0

  ;;

 restart|force-reload)

  $0 stop
  $0 start

  ;;

 reload)
  echo -n "Reloading $DESCRIPTIVE"
  PID=`cat $PIDFILE`
  ow_send_signal HUP $PID || ow_fatal_error "Unable to reload $DESCRIPTIVE"
  echo "."
  ret=0
 ;;

 status)
  if [ -f $PIDFILE ]; then
    PID=`cat $PIDFILE`
    if ow_test_running $PID ; then
      echo "$NAME ($PID) is running."
      exit 0
    else
      echo "$NAME ($PID) is dead, but $PIDFILE exists."
      exit 1
    fi
  else
    echo "$NAME is not running."
    exit 3
  fi
  ;;

 *)
  echo "Usage: $NAME {[re]start|stop|reload|force-reload|status}"
  ret=2
esac

exit $ret


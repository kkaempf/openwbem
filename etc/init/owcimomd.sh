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

# See how we were called.
case "$1" in
 start)
  if [ -s $PIDFILE ]; then
    PID=`cat $PIDFILE`

    if kill -0 $PID >/dev/null 2>&1; then
      echo "$NAME ($PID) is already running."
      exit 1
    else
      echo "Stale $NAME pid file ($PIDFILE) found. Removing."
      rm -f $PIDFILE
    fi
  fi
		  
			
  # Start daemons.
  echo -n "Starting the $DESCRIPTIVE"
  #ssd -S -n $NAME -x $DAEMON -- $OPTIONS 
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
  #$DAEMON -k

  PID=`cat $PIDFILE`
  
  # ssd -K -p $PIDFILE
  kill -s SIGTERM $PID
  echo -n "."

  for num in 1 2 3 4 5 6 7
  do
    if kill -0 $PID >/dev/null 2>&1
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
  if kill -0 $PID >/dev/null 2>&1
  then
    kill -s SIGKILL $PID
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
  kill -s SIGHUP $PID
  echo "."
  ret=0
 ;;

 status)
  if [ -f $PIDFILE ]; then
    PID=`cat $PIDFILE`
    if kill -0 $PID >/dev/null 2>&1; then
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


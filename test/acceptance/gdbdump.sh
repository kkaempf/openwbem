#!/bin/sh

if [ ! "$2" = "" ]; then
	echo "Usage: $0 <path_to_executable> <core_file_or_pid> ..."
	exit 1
else
	EXEC=$1
	shift
	CORES_OR_PIDS=$@
fi

OUT=gdbdump.out

echo "" > $OUT

for i in $CORES_OR_PIDS; do 
	echo "" >> $OUT
	echo "*********  $i  *********" >> $OUT
	echo "" >> $OUT
	gdb $EXEC $i <<EOS >> $OUT
bt
quit
EOS
done


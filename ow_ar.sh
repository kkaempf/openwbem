#!/bin/sh 

THIS=$0

AR=""

while [ $# -gt 0 ]
do 
	case "$1" in
		--)
			shift
			break
			;;	
		*)
			AR="$AR $1"
			;;
	esac
	shift
done
  

ARCHIVES=""
OBJECTS=""

MODE="$1"
shift

case "$MODE" in
	cru)
		TARGET="$1"
		shift
		;;
	x)
		$AR x $@ && exit 0
		;;
	*)
		echo "ERROR: $MODE passed to $THIS"
		exit 1
		;;
esac


while [ $# -gt 0 ]
do 
	case "$1" in
		*.o)
			OBJECTS="$OBJECTS $1"
			;;	
		*.a)
			ARCHIVES="$ARCHIVES $1"
			;;
		*)
			echo "ERROR: $1 passed to $THIS"
			exit 1
			;;
	esac
	shift
done

rm -rf ow_ar$$
mkdir ow_ar$$
OLDDIR=`pwd`
cd ow_ar$$
for i in $ARCHIVES;
do
	$AR x $OLDDIR/$i
done

if test -z "$ARCHIVES";
then
	NEWOBJS=""
else
	NEWOBJS="ow_ar$$/*.o"
fi

cd $OLDDIR
$AR cru $TARGET $NEWOBJS $OBJECTS

rm -rf ow_ar$$


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

# extract each archive into it's own directory so that we
# can safely handle name collisions, i.e. if files are named the
# same, also rename each *.o to be prefixed by it's archive name
# so that when the archive is extracted we don't overwrite files
# that have the same name
for i in $ARCHIVES;
do
	name=`basename $i .a`
	ar_dir=".${name}_dir"
	rm -rf $ar_dir
	mkdir $ar_dir
	cd $ar_dir
	$AR x ../$i
	for j in *.o; do
		mv $j ${name}_$j
	done
	cd ..
	NEWOBJS="$NEWOBJS $ar_dir/*.o"
done

$AR cru $TARGET $NEWOBJS $OBJECTS

# clean up each archive dir
for i in $ARCHIVES;
do
	name=`basename $i .a`
	ar_dir=".${name}_dir"
	rm -rf $ar_dir
done


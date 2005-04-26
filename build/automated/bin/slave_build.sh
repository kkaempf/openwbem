#!/bin/bash

set -x

set -a
set -e
set -u

# This script should be common for most platforms.  There is a target-specific
# script (specified in the platform-specific config file) which this must
# execute to produce output. 
# NOTE: This platform-specific script MUST echo values like:
# OW_OUTPUT_DIRECTORY_RELEASE=somewhere or
# OW_OUTPUT_DIRECTORY_DEBUG=somewhere_else, depending on the $BUILD_METHOD
# parameter passed to it.
# If there is no output directory, then report the output directory as none.
# eg. OW_OUTPUT_DIRECTORY_RELEASE=none

if [ $# -lt 2 ]
then
	echo "Usage: slave_build.sh CONFIG_FILE [debug|release]"
	exit 1
fi

CONFIG_FILE=$1
BUILD_METHOD=$2

shift 2


# Get the source directory so that scripts may be executed from there before
# the config file is sourced.
GetSourceDirectory()
{
	LBS_FLAGS=$-
	set +e
	if [ -e ./$0 ]
	then
		MY_DIR=`pwd`/`dirname $0`
	elif [ -e `dirname $0`/$0 ]
	then
		MY_DIR=`dirname $0`
	elif which $0 >/dev/null 2>/dev/null
	then
		temp=`which $0`
		MY_DIR=`dirname $temp`
		unset temp
	else
		echo "I can't locate myself!  PWD=" `pwd` ", \$0=$0"
		exit 1
	fi
	OW_SOURCE_DIR=$MY_DIR/../../..
	set -$LBS_FLAGS
	unset LBS_FLAGS
}
GetSourceDirectory

# Source the common_functions script so that the functions therein can be used
# in the config file.
. $MY_DIR/common_functions.sh

if [ -e $CONFIG_FILE ]
then
	. $CONFIG_FILE
	OW_BUILD_CONFIG_FILE_ALREADY_SOURCED=1
else
	echo "Config file does not exist!"
	exit 2
fi

echo "Using $CONFIG_FILE for the machine-specific config file."
OW_BUILD_CONFIG_FILE=$CONFIG_FILE
export OW_BUILD_CONFIG_FILE

# Set the path
export PATH="$BUILD_BIN_DIR:$PATH"

# Take anything else that was placed on the command line, and either stuff it
# into an array, or export an environment variable for it.
. recreate_env.sh

# Now, set the all of the $* parameters to be the leftover parameters.
set -- ${PARAMETERS[@]}
unset PARAMETERS

OS=`uname -s`

exit_handler()
{
	code=$?
	set +x
	if [ "$OS" = "Darwin" ]
	then
		# Ensure that this gets placed on a single line (regardless of surrounding output).
		echo -e "\nKILL_ME_PLEASE: $code\n"
	fi
	exit $code
}

trap exit_handler EXIT

# Execute the term handler, so if any problems occur, this script will kill all children.
. term_handler.sh

if [ ! -z "$OW_REMOTE_PATH" ]; then
	echo "Setting up the path, as was specified in the config file:"
	eval "PATH=$OW_REMOTE_PATH"
	export PATH
	echo "$PATH"
fi

if [ "x$BUILD_METHOD" = "xdebug" -o "x$BUILD_METHOD" = "xDEBUG" ]
then
	OW_BUILD_DEBUG=true
	BUILD_METHOD=debug
else
	OW_BUILD_DEBUG=false
	BUILD_METHOD=release
fi

export OW_BUILD_DEBUG

# if ow is installed the build may fail.  Just quit right now and save ourselves the hassle of figuring it out
if [ -d $OW_PREFIX/lib ]; then
	echo "$OW_PREFIX/lib detected!!! BUILD aborting now!!!"
  exit 3
fi


# clean up old build
rm -rf $LOCAL_BUILD_DIR/owstage

# get cvs - TODO: put this into it's own script later
mkdir -p $LOCAL_BUILD_DIR
cd $LOCAL_BUILD_DIR

safe_execute_noexit $USABLE_SHELL build_openwbem_for_vmx.sh

ow_results=$?
if [ $ow_results -ne 0 ]
then
	echo "Failed building OpenWBEM."
	exit $ow_results
fi

strip_files()
{
	if var_is_set OW_STRIP_FLAGS || var_is_set OW_STRIP_PROGRAM; then
		local strip_files_old_flags=$-
		set +e
		set +u
		for file; do
			echo "Stripping ${file}..."
			if var_is_set OW_STRIP_PROGRAM; then
				${OW_STRIP_PROGRAM} ${OW_STRIP_FLAGS} ${file}
			else
				strip ${OW_STRIP_FLAGS} ${file}
			fi
		done
		set -$strip_files_old_flags
	else
		echo "No strip flags known: Unable to strip ${@}"
	fi
}

strip_all_files()
{
	if var_is_set OW_STRIP_FLAGS || var_is_set OW_STRIP_PROGRAM; then
		for potential_executable in $(find ${LOCAL_BUILD_DIR}/owstage -type f); do
			if [ -x ${potential_executable} ]; then
				strip_files ${potential_executable}
			fi
		done
	else
		echo "Refusing to strip files: neither OW_STRIP_FLAGS nor OW_STRIP_PROGRAM is set."
	fi
}

# Strip any binaries/libraries...
if build_release; then
	strip_all_files
else
	echo "Not stripping files: Build mode is not release."
fi

if [ -z "$OW_PACKAGER_SCRIPT" ]
then
	echo "OW_PACKAGER_SCRIPT not specified in config file: I don't know how to package OW!"
	exit 3
fi

if [ "$OW_PACKAGER_SCRIPT" != "none" ]
then
	echo "Executing packaging script: $OW_PACKAGER_SCRIPT"
	safe_execute $OW_PACKAGER_SCRIPT $BUILD_METHOD
	echo "Packaging script finished."
else
	echo "No packaging script to execute"
	# Make the other scripts (possibly on other machines), happy by saying
	# there is no output directory (how could there be?).
	if [ "x$BUILD_METHOD" = "xdebug" -o "x$BUILD_METHOD" = "xDEBUG" ]
	then
		echo "OW_OUTPUT_DIRECTORY_DEBUG=none"
	else
		echo "OW_OUTPUT_DIRECTORY_RELEASE=none"
	fi
fi




#!/bin/bash
# This script is meant to be run from a cron job on buzz
# It can also be invoked manually (although this still needs testing).  In most
# cases, this script should NOT be used manually, as it was written to operate
# on multiple machines.  Executing it on your local machine would probably not
# do what you wanted.
#
# If there is enough demand, a local building script, which doesn't have most
# of this 'magic', can be written.  It may in fact turn out that the scripts
# which this [indirectly] invokes, may be usable for local building.
#
# This section here will update the build system (including the current script), and execute it
#
# Locate the build system.
#
# NOTE: The path located will be one directory higher than the bin directory,
# as such, there should be 'bin' and 'data' directories located in the path
# given.

PATH_TO_BUILD_SYSTEM=
BUILD_CONFIG_FILE_ALREADY_SOURCED=0
LOG_DIRECTORY=/this_is_broken
CVS_BRANCH_DATE="date not set"

# Locate the build system
LocateBuildSystem()
{
	if [ -f ./$0 ]
	then
		PATH_TO_BUILD_SYSTEM=`pwd`/`dirname $0`/..
	elif [ -f "`which $0`" ] >/dev/null 2>/dev/null
	then
		temp=`which $0`
		PATH_TO_BUILD_SYSTEM=`dirname $temp`/..
		unset temp
	else
		echo "I can't locate myself!  PWD=" `pwd` ", \$0=$0"
		exit 1
	fi
}

UpdateBuildSystem()
{
	if [ "${BUILD_SYSTEM_UPDATED:-x}" != "xtrue" ]
	then
		export CVS_RSH=ssh
		cd $PATH_TO_BUILD_SYSTEM
		cvs -Q update -Pd -l .  || return 1
		cvs -Q update -Pd bin   || return 1
		cvs -Q update -Pd data  || return 1
		export BUILD_SYSTEM_UPDATED=true

		cd bin
		exec $0 $@
	fi
}

SourceConfigFile()
{
	# hardcoded path just for the build machine
	var_is_set BUILD_CONFIG_FILE || BUILD_CONFIG_FILE=$PATH_TO_BUILD_SYSTEM/data/main_build_machine.config

	local OLD_DIR=`pwd`
	cd `dirname $BUILD_CONFIG_FILE`
	cvs up -A `basename $BUILD_CONFIG_FILE`
	cd $OLD_DIR
	# read in the config file
	. $BUILD_CONFIG_FILE
	BUILD_CONFIG_FILE_ALREADY_SOURCED=1
}

# check/create the lock file
CreateLockFile()
{
	# Wait up to 5 seconds in an attempt to create a lock file.
	if mutex_create $BUILD_LOCK_FILE 5; then
		:
	else
    echo "Build lock ($BUILD_LOCK_FILE) already exists."
    exit 1
	fi
}

# set up a func to remove the lock file when the script exits
RemoveLockFile()
{
	mutex_release $BUILD_LOCK_FILE
}

# Add the build bin directory to the path.
AddBuildBinToPath()
{
	if echo "$PATH" | grep -v "$PATH_TO_BUILD_SYSTEM/bin" >/dev/null; then
		export PATH="$PATH_TO_BUILD_SYSTEM/bin:$PATH"
	fi
}

ShouldBuildBranch()
{
	local LOCAL_BRANCH_TAG=$1
	local DO_BUILD=false

	# inputs to has_cvs_chaged_since_last_build are BRANCH and BUILD_DATE_FILE (set in the config file)
	export BRANCH=$LOCAL_BRANCH_TAG
	local old_path=$PATH
	if has_cvs_changed_since_last_build.sh; then
		echo "cvs has changed.  initiating build."
		DO_BUILD=true
	fi
	export PATH=$old_path
	if [ "$DO_BUILD" = true ] ; then
		return 0
	else
		return 1
	fi
}

CheckoutCodeFromCVS()
{
	local LOCAL_BRANCH_TAG=$1

	if [ ! -e $OW_SOURCE_DIR -o ! -e $OW_SOURCE_DIR/CVS/Root ]
	then
		mkdir -p $OW_SOURCE_PARENT
		cd $OW_SOURCE_PARENT
		cvs -d$OW_CVS_ROOT co $OW_CVS_MODULE
	fi

	cd $OW_SOURCE_DIR

	if [ "$BRANCH" != "HEAD" ]; then
		UPDATE_FLAG="-r $BRANCH"
	else
		UPDATE_FLAG="-A"
	fi

	cvs -q up $UPDATE_FLAG -dP > /dev/null
}

ModifyDateFileForBranch()
{
	local BRANCH=$1

	if [ -f $BUILD_DATE_FILE ]; then
		# Remove any prior entries for this branch.
		if grep -v "^$BRANCH" $BUILD_DATE_FILE > $BUILD_DATE_FILE.modified; then
			mv $BUILD_DATE_FILE.modified $BUILD_DATE_FILE
		else
			rm $BUILD_DATE_FILE
		fi
	fi
	echo "$BRANCH: $CVS_BRANCH_DATE" >> $BUILD_DATE_FILE
	date_conversion.sh "$CVS_BRANCH_DATE" > $BUILD_DATE_FILE.seconds
}

CreateLogDirectory()
{
	# Create a directory for log files.
	export LOG_DIRECTORY="/tmp/owcron_buildlog-$$"
	if [ -e ${LOG_DIRECTORY} -o -d ${LOG_DIRECTORY} ]
	then
		rm -rf ${LOG_DIRECTORY}
	fi
	mkdir -p ${LOG_DIRECTORY}
}


Main()
{
	# Automatically mark variables and functions for export to the environment
	set -a

	# Locate and update the build system (this script will restart once).
	LocateBuildSystem
	UpdateBuildSystem || echo "Failed to update build system"

	# Add the build bin directory to the path.
	AddBuildBinToPath

	# Exit immediately if a simple command exits with a non-zero status.
	set -e
	# Treat unset variables as an error when performing parameter expansion
	set -u

	# set some vars that aren't normally set when running under cron
	export CVS_RSH=ssh
	export PATH=/usr/lib/ccache/bin:$PATH

	# Run the common function script.
	. $PATH_TO_BUILD_SYSTEM/bin/common_functions.sh

	# Run the config file.
	var_is_set BUILD_CONFIG_FILE_ALREADY_SOURCE && [ "$BUILD_CONFIG_FILE_ALREADY_SOURCE" = "1" ] || SourceConfigFile

	# Create a lock file, and make sure that it is removed upon exit.
	CreateLockFile
	trap RemoveLockFile EXIT

	# Set the date in the file that the psycho killer uses to
	# determine if the build should be killed.  This is useful to
	# prevent the checkout from being killed, since the real date
	# isn't set until it is decided that the build should happen.
	CUR_DATE=`ssh $OW_CVS_SERVER date -R`
	date_conversion.sh "$CUR_DATE" > $BUILD_DATE_FILE.seconds

	# Set up a term handler, so if any problems occur, this script will kill all children.
	. term_handler.sh
	# Prevent the script from being exited until the email has been sent.
	disable_exit_on_signals

	BRANCHES_BUILT=

	# for each branch tag, check to see if it needs building.
	for BRANCH in $CVS_BRANCHES_TO_BUILD; do
		if ShouldBuildBranch $BRANCH
		then
			CheckoutCodeFromCVS $BRANCH
			echo "=============================="
			echo "Building branch $BRANCH"
			echo "=============================="

			# Modify the last build date for the branch
			CVS_BRANCH_DATE=$CUR_DATE
			ModifyDateFileForBranch $BRANCH

			# Create the log directory, and set the LOG_DIRECTORY env var.
			CreateLogDirectory

			# Build the branch
			CVS_BRANCH="$BRANCH"
			export CVS_BRANCH
			CVS_BRANCH_FLAG="-r$BRANCH"
			if [ "x$BRANCH" = "xHEAD" ] ; then
				CVS_BRANCH_FLAG="-A"
			else
				CVS_BRANCH_FLAG="-r$BRANCH"
			fi

			export CVS_BRANCH_FLAG CVS_BRANCH_DATE
			safe_execute_noexit bash -x main_build.sh

			BRANCHES_BUILT="$BRANCHES_BUILT $BRANCH"
		fi
	done

	if var_is_set BRANCHES_BUILT; then
		echo "Branches $BRANCHES_BUILT should have been built."
	else
		echo "no changes detected since last build."
		exit 2
	fi
}


# Run the main.
# Note: By doing it this way, it is possible to prevent the output from being
# displayed (if redirected), so that cron won't send emails.  The output could
# also be sent to a file which is accessible from the web server running on
# buzz.
Main

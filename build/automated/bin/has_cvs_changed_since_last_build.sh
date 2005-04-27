#!/bin/sh
# This script will return 0 if any changes have been checked into cvs since
# the last build, and no changes have occurred in the last 15 minutes.
# The point is to call it to see if a new build needs to be done.  There is
# no point in doing a new buld if nothing has changed.  Also we don't want to
# launch a build if anything has changed in the last 15 minutes, because
# someone may be in the process of checking in a large change.
#
# The way this works is by running cvs log and using the -d flag to pass in
# the last build date.  Any files that changed cause "selected revisions: 1"
# to be printed.  So we grep for that and count them up to know how many files
# changed.
#
# INPUT:
#  $BUILD_DATE_FILE - the filename of a file which contains the date the last 
#   build was run. The date should be in rfc 822 format (date -R)
#  $BRANCH - The branch to examine.  If not set, then HEAD will be used.
#  $COMMIT_DATE_FILE - the filename containing the last commit dates
#
# The current working directory must be the root of the cvs repository 
#  checkout.
#
# This will work even if cvs is not in the most recent state.
#
# Side effects: None

# Set the minimum time between a commit and a build at 15 minutes.
if [ -z "$MINIMUM_COMMIT_AGE" ]; then
	echo "MINIMUM_COMMIT_AGE env var not set.  Using default."
	MINIMUM_COMMIT_AGE=900
fi

set -x

if [ -z "$BUILD_DATE_FILE" ]; then
	echo "BUILD_DATE_FILE env var not set!"
	exit 1
fi

if [ -z "$COMMIT_DATE_FILE" ]; then
	echo "COMMIT_DATE_FILE env var not set!"
	exit 1
fi

if [ -f "$BUILD_DATE_FILE" ]; then
	:
else
	echo "WARNING: BUILD_DATE_FILE=$BUILD_DATE_FILE doesn't exist. Doing a build."
	exit 0
fi


TAG_TO_USE=
if [ "x$BRANCH" = "x" ]; then
	BRANCHOPT="-b"
	echo "WARNING: No branch specified when checking for changes.  Using HEAD."
	TAG_TO_USE=HEAD
else
	BRANCHOPT="-r$BRANCH"
	TAG_TO_USE=$BRANCH
fi

if grep "^$TAG_TO_USE" $BUILD_DATE_FILE >/dev/null 2>/dev/null; then
	LAST_BUILD_DATE=`grep $TAG_TO_USE $BUILD_DATE_FILE | tail -n1 | cut -f2- -d' '`
else
	echo "WARNING: Branch $TAG_TO_USE was not found in $BUILD_DATE_FILE.  Doing a build."
	exit 0
fi

# This check method is no longer used, and is here only for reference.
old_check_method()
{
	NUM_CHANGES_SINCE_LAST_BUILD=`cvs -q log -d">$LAST_BUILD_DATE" -N "$BRANCHOPT" | grep 'selected revisions: [^0]' | wc -l`
	NUM_RECENT_CHANGES=`cvs -q log -d">15 minutes ago" -N "$BRANCHOPT" | grep 'selected revisions: [^0]' | wc -l`

	echo "NUM_CHANGES_SINCE_LAST_BUILD=$NUM_CHANGES_SINCE_LAST_BUILD"
	echo "NUM_RECENT_CHANGES=$NUM_RECENT_CHANGES"

	if [ "$NUM_CHANGES_SINCE_LAST_BUILD" -gt 0 ] && [ "$NUM_RECENT_CHANGES" -eq 0 ]; then
		exit 0
	else
		exit 1
	fi
}


# This is the new method. 
# It copies a file from the CVS server, which contains a list of the last commit dates for each branch.
# It then calculates:
#   The current time in seconds (since 1970)
#   The time of the last commit in seconds (since 1970)
#   The time of the last build in seconds (since 1970)
#
# If the last commit was later than the last build, and the last commit is
# older than the minimum commit age, a build will happen.  
# Otherwise, no build will happen.

CVS_SERVER=`cat CVS/Root | sed 's/.*@\([^:]*\):.*/\1/'`
CVS_DIR=`cat CVS/Root | sed 's/.*:\(.*\)$/\1/'`
scp $CVS_SERVER:CVS_DIR/CVSROOT/`basename "$COMMIT_DATE_FILE"` "$COMMIT_DATE_FILE"

CVS_HAS_CHANGED=0

if grep "^${TAG_TO_USE}:" "$COMMIT_DATE_FILE" >/dev/null 2>/dev/null; then
	LAST_COMMIT_DATE=`grep $TAG_TO_USE $COMMIT_DATE_FILE | tail -n1 | cut -f2- -d' '`
else
	echo "WARNING: Branch $TAG_TO_USE has never had a commit. Not building."
	exit 1
fi

LAST_BUILD_SECONDS=`date_conversion.sh "$LAST_BUILD_DATE"`
LAST_COMMIT_SECONDS=`date_conversion.sh "$LAST_COMMIT_DATE"`
REMOTE_TIME=`ssh cvs date -R`
CURRENT_TIME=`date_conversion.sh "$REMOTE_TIME"`

BUILD_AGE=`echo "$CURRENT_TIME-$LAST_BUILD_SECONDS" | bc`
COMMIT_AGE=`echo "$CURRENT_TIME-$LAST_COMMIT_SECONDS" | bc`
TIME_DIFFERENCE=`echo "$LAST_COMMIT_SECONDS-$LAST_BUILD_SECONDS" | bc`
if [ $? -ne 0 ]; then
	echo "Failed to calculate time difference..."
	exit 1
fi

if [ $TIME_DIFFERENCE -gt 0 ]; then
	# A commit has occurred after the last build happened.
	if [ $COMMIT_AGE -gt $MINIMUM_COMMIT_AGE ]; then
		# The last commit
		exit 0
	else
		echo "Commit age below minimum threshold."
		exit 1
	fi
else
	echo "Last build happened $BUILD_AGE seconds ago ($TIME_DIFFERENCE seconds after the last commit)"
	exit 1
fi

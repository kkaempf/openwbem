#!/bin/sh -x

set

. common.sh

# NOTES:
# Anything that is in the variable OW_RELEASE_TEXT will be inserted into the rpm filename (and version).

##################### Linux Distributions ###################################
#
# Information on OW CLIENT Linux distributions:
#
# Distro        File with info...  What it shows...      Init.d        Glibc
# ===========================================================================
# RH 7.2      /etc/redhat-release    $5 ~ "7.2"        /etc/init.d     2.2.4
# RH 7.3      /etc/redhat-release    $5 ~ "7.3"        /etc/init.d     2.2.5
# RH 8.0      /etc/redhat-release    $5 ~ "8.0"        /etc/init.d     2.2.93
# 
# UL 1.0  /etc/UnitedLinux-release VERSION = 1.0       /etc/init.d     2.2.5
#
# SuSE 7.3    /etc/SuSE-release      $3 ~ "7.3"        /etc/init.d
# SuSE 8.0    /etc/SuSE-release      $3 ~ "8.0"        /etc/init.d     2.2.5
# SuSE 8.1    /etc/SuSE-release      $3 ~ "8.1"        /etc/init.d     2.2.5
# 
#############################################################################

# Variables that will be set:
#
# OSTYPE = output of uname (converted to uppercase): LINUX
# OSVEND = vendor (RedHat, UnitedLinux, SuSE)
# OSVER = version of the Linux distro. 
# The values will be UNKNOWN if the script is unable to determine.
GetPlatformInformation()
{
	# Some shells require that a variable be set before unsetting.
	OSTYPE=""
	OSVEND=""
	OSVER=""
	local RELEASE_PREFIX=""
	unset OSTYPE OSVEND OSVER 
	
	OSTYPE=`uname | tr '[:lower:]' '[:upper:]'`
	
	# If Red Hat...
	if [ -f /etc/redhat-release ]; then
		# Prevent false-positives from some Red Hat derivatives...
		if grep "Red Hat" /etc/redhat-release > /dev/null 2>&1; then
	        	OSVEND=RedHat
			RELEASE_PREFIX=rh
			#OSVER=`cut -d' ' -f5 /etc/redhat-release`
			if rpm -q redhat-release > /dev/null 2>&1; then
		        	OSVER=`rpm -q --qf '%{VERSION}' redhat-release`
			else
				# RedHat AS 2.1
				OSVER=`rpm -q --qf '%{VERSION}' redhat-release-as`
			fi
		else
			OSVEND=UNKNOWN
			OSVER=UNKNOWN
			RELEASE_PREFIX=unk
		fi
	# If SuSE...
	elif [ -f /etc/SuSE-release ]; then
		OSVEND=SuSE
		OSVER=`grep VERSION /etc/SuSE-release | cut -d' ' -f3`
		RELEASE_PREFIX=su	
	# If UnitedLinux...
	#
	elif [ -f /etc/UnitedLinux-release ]; then
		OSVEND=UnitedLinux
		OSVER=`grep VERSION /etc/UnitedLinux-release | cut -d' ' -f3`
		RELEASE_PREFIX=ul
	# If we've gotten this far, we're not on a supported distro...
	else
		OSVEND=UNKNOWN
		OSVER=UNKNOWN
		RELEASE_PREFIX=unk
	fi
	local OSVER_WITH_SPACES=`echo $OSVER | tr '[.]' '[ ]'`
	OS_MAJOR=`echo $OSVER_WITH_SPACES | awk '{ print $1; }'`
	OS_MINOR=`echo $OSVER_WITH_SPACES | awk '{ print $2; }'`
	OS_MICRO=`echo $OSVER_WITH_SPACES | awk '{ print $3; }'`
	# Assume (for now) that all versions with the same major number will work.
	RELEASE_TEXT=`echo $RELEASE_PREFIX$OS_MAJOR`
	[ "$OSVEND" = "RedHat" ] && [ "$OSVER" = "2.1AS" ] && RELEASE_TEXT="rhas21" || true
}

OW_STAGE_DIR=$LOCAL_BUILD_DIR/owstage

#install some things specific to the linux rpm
cd $LOCAL_BUILD_DIR/$OW_CVS_MODULE

install -d $OW_STAGE_DIR/etc/init.d
install etc/init/owcimomd $OW_STAGE_DIR/etc/init.d/owcimomd
install -d $OW_STAGE_DIR/etc/init.d
install -d $OW_STAGE_DIR/etc/pam.d
install etc/pam.d/openwbem $OW_STAGE_DIR/etc/pam.d/openwbem

install -d $OW_STAGE_DIR/var$OW_PREFIX/openwbem
install -d $OW_STAGE_DIR/$OW_PREFIX/libexec/openwbem
install -d $OW_STAGE_DIR/var$OW_PREFIX/openwbem
install -d $OW_STAGE_DIR/$OW_PREFIX/libexec/openwbem

# $LOCAL_BUILD_DIR/owstage has OW & our providers installed into it
cd $LOCAL_BUILD_DIR

for x in SOURCES BUILD SRPMS RPMS
do
	# Clear out the old rpms, if they exist.
	rm -rf $RPM_BUILD_DIR/$x
	mkdir -p $RPM_BUILD_DIR/$x
done

GetPlatformInformation
export OW_RELEASE_TEXT=$RELEASE_TEXT

# rpm requires a source even if it's not used, so create one.
tar czvf $RPM_BUILD_DIR/SOURCES/openwbem.tgz -C $OW_STAGE_DIR .


RPMBUILD=
if rpmbuild --version 2>/dev/null >/dev/null
then
	RPMBUILD=rpmbuild
else
	RPMBUILD=rpm
fi

rm -rf $RPM_BUILD_DIR/RPMS/{ppc,i386,i486,i586,i686}

$RPMBUILD -bb --define "_topdir ${RPM_BUILD_DIR}" $OW_BUILD_DATA_DIR/openwbem.spec

mkdir $LOCAL_BUILD_DIR/packages
cp $RPM_BUILD_DIR/RPMS/*/*rpm $LOCAL_BUILD_DIR/packages
echo "OW_OUTPUT_DIRECTORY_RELEASE=$LOCAL_BUILD_DIR/packages"



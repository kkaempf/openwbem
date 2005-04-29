#!/bin/sh

if [ -f `dirname ${0}`/common.sh ]; then
  . `dirname ${0}`/common.sh
else
  . common.sh
fi

cd ${LOCAL_BUILD_DIR}/${OW_CVS_MODULE}
./cvsbootstrap.sh

CONFIGURE_DEBUG_FLAGS=
if [ "x${OW_BUILD_DEBUG}" = "xtrue" ]
then 
  CONFIGURE_DEBUG_FLAGS="--enable-debug-mode"

  if [ `uname` = "Darwin" ]; then
	 var_is_set CXXFLAGS || CXXFLAGS=""
    CXXFLAGS="${CXXFLAGS} -Wno-deprecated" # dumb Apple gcc compiler warns on the definition of deprecated functions.
  fi
fi


# FIXME! Move most of this junk into the config files.

STATIC_SERVICE_FLAGS=
if [ `uname` = "AIX" ]; then
	# AIX must be static.
	STATIC_SERVICE_FLAGS="--enable-static-services"
	# AIX has a horribly slow shell.  Use bash instead.
	export CONFIG_SHELL=/usr/local/bin/bash
	export SHELL=/usr/local/bin/bash
else
	# Use CFLAGS, CXXFLAGS and LDFLAGS to tell the compiler/linker to look 
	# under ${OW_PREFIX} for headers and libs. 
	# NOTE: This cannot be used on AIX, as it requires fake libraries, and
	# whith those, the build breaks (this search path comes before
	# everything OW is building). 
  
	CFLAGS="${CFLAGS:-}"
	CXXFLAGS="${CXXFLAGS:-}"
	CPPFLAGS="${CPPFLAGS:-}"
	LDFLAGS="${LDFLAGS:-}"
	if var_is_set SSL_INCLUDE_DIR
			then
			CPPFLAGS="${CPPFLAGS}"
	else
			CPPFLAGS="${CPPFLAGS} -I${SSL_INCLUDE_DIR}"
	fi
	if var_is_set ZLIB_INCLUDE_DIR
			then
			CPPFLAGS="${CPPFLAGS}"
	else
			CPPFLAGS="${CPPFLAGS} -I${ZLIB_INCLUDE_DIR}"
	fi
	
	if var_is_set SSL_LIB_DIR
			then
			LDFLAGS="${LDFLAGS}"
	else
			LDFLAGS="${LDFLAGS} -L${SSL_LIB_DIR}"
	fi
	if var_is_set ZLIB_LIB_DIR
			then
			LDFLAGS="${LDFLAGS}"
	else
			LDFLAGS="${LDFLAGS} -L${ZLIB_LIB_DIR}"
	fi
	
	export CFLAGS CXXFLAGS CPPFLAGS LDFLAGS
fi

${SHELL} ./configure \
	${CONFIGURE_DEBUG_FLAGS} \
	--prefix=${OW_PREFIX} \
	--sysconfdir=/etc${OW_PREFIX} \
	--localstatedir=/var${OW_PREFIX} \
	--with-runtime-link-path=${OW_PREFIX}/lib \
	${STATIC_SERVICE_FLAGS} ${OW_CONFIGURE_FLAGS} 

make ${NUM_PROCS_MAKE_FLAG} clean

# If the LD_RUN_PATH environment variable is set at link time, 
# it's value is embedded in the resulting binary.  At run time, 
# The binary will look here first for shared libraries.  This way
# we link against the libraries we want at run-time even if libs
# by the same name are in /usr/lib or some other path in /etc/ld.so.conf
LD_RUN_PATH=${OW_PREFIX}/lib
export LD_RUN_PATH

make ${NUM_PROCS_MAKE_FLAG}
#make check
#OWLONGTEST=1 make check

# install OW
OW_STAGE_DIR=${LOCAL_BUILD_DIR}/owstage
# rm -rf OW_STAGE_DIR
make DESTDIR=${OW_STAGE_DIR} install

#edit the config file, turn off stuff we don't need.
FILE=${OW_STAGE_DIR}/etc${OW_PREFIX}/openwbem/openwbem.conf
EditConfig()
{
	cp ${FILE} ${FILE}~
	sed -e "${1}"  ${FILE}~ > ${FILE}
	if cmp -s ${FILE}~ ${FILE}; then
		diff ${FILE}~ ${FILE}
		echo "EditConfig didn't work for ${1}"
		exit 1
	fi
	rm ${FILE}~
}

EditConfig 's/http_server.http_port.*/http_server.http_port = -1/g'
EditConfig 's/\(owcimomd.authorization2_lib.*\)/;\1/g'
EditConfig 's/http_server.allow_local_authentication = false/http_server.allow_local_authentication = true/g'

# On AIX, the aix authentication should be used by default instead of the pam.  
# NOTE: PAM will work, but most AIX users won't have it installed (they have to
# download it for AIX 5.1 and earlier).  
if [ `uname` = "AIX" ]; then
	EditConfig "s|\(owcimomd.authentication_module = ${OW_PREFIX}/lib/openwbem/authentication\)/libpamauthentication.so|\1/libaixauthentication.so|g"
fi






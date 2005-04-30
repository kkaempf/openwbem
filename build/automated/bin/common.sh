#!/bin/sh

# Automatically mark variables and functions for export to the environment
set -a
# Exit immediately if a simple command exits with a non-zero status.
set -e
# After expanding each simple command, display it
set -x
# Treat unset variables as an error when performing parameter expansion
set -u

OW_BUILD_CONFIG_FILE_ALREADY_SOURCED=${OW_BUILD_CONFIG_FILE_ALREADY_SOURCED:-0}

if [ "${OW_BUILD_CONFIG_FILE_ALREADY_SOURCED}" = "0" ]; then
	
	if [ "x${OW_BUILD_CONFIG_FILE:+set}" != "xset" ]; then
		echo "error: \${OW_BUILD_CONFIG_FILE} is not set"
		exit 1
	fi

	if [ ! -f ${OW_BUILD_CONFIG_FILE} ]; then
		echo "${OW_BUILD_CONFIG_FILE} doesn't exist!"
		exit 1
	fi

	# read in the config file
	. ${OW_BUILD_CONFIG_FILE}
	OW_BUILD_CONFIG_FILE_ALREADY_SOURCED=1

	if echo "${PATH}" | grep -v ${OW_BUILD_BIN_DIR}; then
		PATH="${OW_BUILD_BIN_DIR}:${PATH}"
	fi
	
fi

. term_handler.sh

. common_functions.sh

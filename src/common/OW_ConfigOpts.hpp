/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef _OW_CONFIGOPTS_HPP__
#define _OW_CONFIGOPTS_HPP__

#include "OW_config.h"

#ifndef DEFAULT_HTTP_PORT
#define DEFAULT_HTTP_PORT "5988"
#endif

#ifndef DEFAULT_HTTPS_PORT
#define DEFAULT_HTTPS_PORT "5989"
#endif

#ifndef DEFAULT_LOG_FILE
#define DEFAULT_LOG_FILE "syslog"
#endif

#ifndef DEFAULT_OWLIB_DIR
#define DEFAULT_OWLIB_DIR OW_DEFAULT_LIB_DIR"/openwbem"
#endif

#ifndef DEFAULT_IFC_LIBS
#define DEFAULT_IFC_LIBS DEFAULT_OWLIB_DIR"/provifcs"
#endif

#ifndef DEFAULT_CIMOM_SERVICES_LOCATION
#define DEFAULT_CIMOM_SERVICES_LOCATION DEFAULT_OWLIB_DIR"/services"
#endif

#ifndef DEFAULT_CIMOM_REQHANDLER_LOCATION
#define DEFAULT_CIMOM_REQHANDLER_LOCATION DEFAULT_OWLIB_DIR"/requesthandlers"
#endif

#ifndef DEFAULT_CPP_PROVIDER_LOCATION
#define DEFAULT_CPP_PROVIDER_LOCATION DEFAULT_OWLIB_DIR"/c++providers"
#endif

#ifndef DEFAULT_SIMPLE_CPP_PROVIDER_LOCATION
#define DEFAULT_SIMPLE_CPP_PROVIDER_LOCATION DEFAULT_OWLIB_DIR"/simplec++providers"
#endif

#ifndef DEFAULT_NPI_PROVIDER_LOCATION
#define DEFAULT_NPI_PROVIDER_LOCATION DEFAULT_OWLIB_DIR"/npiproviders"
#endif

#ifndef DEFAULT_AUTH_MOD
#define DEFAULT_AUTH_MOD DEFAULT_OWLIB_DIR"/authentication/libpamauthentication"OW_SHAREDLIB_EXTENSION
#endif
									
#ifndef DEFAULT_AUTHORIZER_LIBS_LOCATION
#define DEFAULT_AUTHORIZER_LIBS_LOCATION DEFAULT_OWLIB_DIR"/auth_mods"
#endif

#ifndef DEFAULT_CONFIG_FILE
#define DEFAULT_CONFIG_FILE OW_DEFAULT_SYSCONF_DIR"/openwbem/openwbem.conf"
#endif

#ifndef DEFAULT_DIGEST_PASSWD_FILE
#define DEFAULT_DIGEST_PASSWD_FILE OW_DEFAULT_SYSCONF_DIR"/openwbem/digest_auth.passwd"
#endif

#ifndef DEFAULT_SIMPLE_PASSWD_FILE
#define DEFAULT_SIMPLE_PASSWD_FILE OW_DEFAULT_SYSCONF_DIR"/openwbem/simple_auth.passwd"
#endif

#ifndef DEFAULT_DATA_DIR
#define DEFAULT_DATA_DIR "/var/lib/openwbem"
#endif

#ifndef DEFAULT_LIBEXEC_DIR
#define DEFAULT_LIBEXEC_DIR OW_DEFAULT_LIBEXEC_DIR"/openwbem"
#endif

#ifndef DEFAULT_OPTIONS_REQUIRE_LOGIN
#define DEFAULT_OPTIONS_REQUIRE_LOGIN "true"
#endif

#ifndef DEFAULT_ALLOW_ANONYMOUS
#define DEFAULT_ALLOW_ANONYMOUS "false"
#endif

#ifndef DEFAULT_MAX_CONNECTIONS
#define DEFAULT_MAX_CONNECTIONS "30"
#endif

#ifndef  DEFAULT_DISABLE_INDICATIONS
#define  DEFAULT_DISABLE_INDICATIONS "false"
#endif

#ifndef DEFAULT_WQL_LIB
#define DEFAULT_WQL_LIB OW_DEFAULT_LIB_DIR"/libowwql"OW_SHAREDLIB_EXTENSION
#endif

#ifndef DEFAULT_ENABLE_DEFLATE
#define DEFAULT_ENABLE_DEFLATE "true"
#endif

#ifndef DEFAULT_USE_UDS
#define DEFAULT_USE_UDS "true"
#endif

#ifndef DEFAULT_CPPIFC_PROV_TTL
#define DEFAULT_CPPIFC_PROV_TTL "5"
#endif

#ifndef DEFAULT_REQ_HANDLER_TTL
#define DEFAULT_REQ_HANDLER_TTL "5"
#endif

#ifndef DEFAULT_MAX_CLASS_CACHE_SIZE
#define DEFAULT_MAX_CLASS_CACHE_SIZE 128
#endif

namespace OW_ConfigOpts
{
	static const char* const OW_DEBUG_opt = "owcimomd.debugflag";
	static const char* const CONFIG_FILE_opt = "owcimomd.config_file";
	static const char* const LOG_LOCATION_opt = "owcimomd.log_location";
	static const char* const LOG_LEVEL_opt = "owcimomd.log_level";
	static const char* const PROVIDER_IFC_LIBS_opt = "owcimomd.provider_ifc_libs";
	static const char* const ONLY_HELP_opt = "owcimomd.onlyhelp";
	static const char* const LIBEXEC_DIR_opt = "owcimomd.libexecdir";
	static const char* const OWLIB_DIR_opt = "owcimomd.owlibdir";
	static const char* const DATA_DIR_opt = "owcimomd.datadir";
	static const char* const ALLOW_ANONYMOUS_opt = "owcimomd.allow_anonymous";
	static const char* const PAM_ALLOWED_USERS_opt = "pam.allowed_users";
	static const char* const SIMPLE_AUTH_FILE_opt = "simple_auth.password_file";
	static const char* const AUTH_MOD_opt = "owcimomd.authentication_module";
	static const char* const DUMP_SOCKET_IO_opt = "owcimomd.dump_socket_io";
	static const char* const CPPIFC_PROV_LOC_opt = "cppprovifc.prov_location";
	static const char* const DISABLE_INDICATIONS_opt = "owcimomd.disable_indications";
	static const char* const WQL_LIB_opt = "owcimomd.wql_lib";
	static const char* const ACL_SUPERUSER_opt = "owcimomd.ACL_superuser";

	// 2.0 additions
	static const char* const CIMOM_SERVICES_LOCATION_opt = "owcimomd.services_path";
	static const char* const CIMOM_REQUEST_HANDLER_LOCATION_opt = "owcimomd.request_handler_path";
	static const char* const HTTP_SLP_DISABLED_opt = "http_server.disable.slp";
	static const char* const HTTP_PATH_opt = "internal.http.Path";
	static const char* const USER_NAME_opt = "internal.user.Name";
	static const char* const USE_UDS_opt = "http_server.use_UDS";
	static const char* const NPIIFC_PROV_LOC_opt = "npiprovifc.prov_location";
	static const char* const SERVICE_LIB_PATH_opt = "daemon.service_lib_path";
	static const char* const REQUEST_HANDLER_LIB_PATH_opt = "daemon.request_handler_lib_path";
	static const char* const SINGLE_THREAD_opt = "http_server.single_thread";
	static const char* const HTTP_PORT_opt = "http_server.http_port";
	static const char* const HTTPS_PORT_opt = "http_server.https_port";
	static const char* const ENABLE_DEFLATE_opt = "http_server.enable_deflate";
	static const char* const HTTP_USE_DIGEST_opt = "http_server.use_digest";
	static const char* const DIGEST_AUTH_FILE_opt = "http_server.digest_password_file";
	static const char* const MAX_CONNECTIONS_opt = "http_server.max_connections";
	static const char* const SSL_CERT_opt = "http_server.SSL_cert";
	static const char* const CPPIFC_PROV_TTL_opt = "cppprovifc.prov_TTL";
	static const char* const REQ_HANDLER_TTL_opt = "owcimomd.request_handler_TTL";
	static const char* const SIMPLECPPIFC_PROV_LOC_opt = "simplecppprovifc.prov_location";
	static const char* const MAX_CLASS_CACHE_SIZE_opt = "owcimomd.max_class_cache_size";
};

#endif //_OW_CONFIGOPTS_HPP__

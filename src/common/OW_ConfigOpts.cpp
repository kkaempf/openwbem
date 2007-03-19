/*******************************************************************************
* Copyright (C) 2004 Novell, Inc. All rights reserved.
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
*  - Neither the name of Novell, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Bart Whiteley
 */

#include "OW_config.h"
#include "OW_ConfigOpts.hpp"
#include "OW_String.hpp"
#include <cstring>

namespace OW_NAMESPACE
{
namespace ConfigOpts
{
	char const DEBUGFLAG_opt[] = "owcimomd.debugflag";
	char const CONFIG_FILE_opt[] = "owcimomd.config_file";
	char const LOG_LOCATION_opt[] = "owcimomd.log_location";
	char const LOG_LEVEL_opt[] = "owcimomd.log_level";
	char const PROVIDER_IFC_LIBS_opt[] = "owcimomd.provider_ifc_libs";
	char const ONLYHELP_opt[] = "owcimomd.onlyhelp";
	char const LIBEXECDIR_opt[] = "owcimomd.libexecdir";
	char const OWLIBDIR_opt[] = "owcimomd.owlibdir";
	char const DATADIR_opt[] = "owcimomd.datadir";
	char const ALLOW_ANONYMOUS_opt[] = "owcimomd.allow_anonymous";
	char const PAM_ALLOWED_USERS_opt[] = "pam.allowed_users";
	char const SIMPLE_AUTH_PASSWORD_FILE_opt[] = "simple_auth.password_file";
	char const AUTHENTICATION_MODULE_opt[] = "owcimomd.authentication_module";
	char const DUMP_SOCKET_IO_opt[] = "owcimomd.dump_socket_io";
	char const CPPPROVIFC_PROV_LOCATION_opt[] = "cppprovifc.prov_location";
	char const DISABLE_INDICATIONS_opt[] = "owcimomd.disable_indications";
	char const WQL_LIB_opt[] = "owcimomd.wql_lib";
	char const ACL_SUPERUSER_opt[] = "owcimomd.ACL_superuser";
	// 2.0 additions
	char const SERVICES_PATH_opt[] = "owcimomd.services_path";
	char const REQUEST_HANDLER_PATH_opt[] = "owcimomd.request_handler_path";
	char const SLP_ENABLE_ADVERTISEMENT_opt[] = "slp.enable_advertisement";
	char const HTTP_SERVER_USE_UDS_opt[] = "http_server.use_UDS";
	char const NPIPROVIFC_PROV_LOCATION_opt[] = "npiprovifc.prov_location";
	char const HTTP_SERVER_SINGLE_THREAD_opt[] = "http_server.single_thread";
	char const HTTP_SERVER_HTTP_PORT_opt[] = "http_server.http_port";
	char const HTTP_SERVER_HTTPS_PORT_opt[] = "http_server.https_port";
	char const HTTP_SERVER_ENABLE_DEFLATE_opt[] = "http_server.enable_deflate";
	char const HTTP_SERVER_USE_DIGEST_opt[] = "http_server.use_digest";
#ifndef OW_DISABLE_DIGEST
	char const HTTP_SERVER_DIGEST_PASSWORD_FILE_opt[] = "http_server.digest_password_file";
#endif
	char const HTTP_SERVER_MAX_CONNECTIONS_opt[] = "http_server.max_connections";
	char const HTTP_SERVER_SSL_CERT_opt[] = "http_server.SSL_cert";
	char const CPPPROVIFC_PROV_TTL_opt[] = "cppprovifc.prov_TTL";
	char const REQUEST_HANDLER_TTL_opt[] = "owcimomd.request_handler_TTL";
	char const MAX_CLASS_CACHE_SIZE_opt[] = "owcimomd.max_class_cache_size";
	// 3.0 additions
	char const HTTP_SERVER_REUSE_ADDR_opt[] = "http_server.reuse_addr";
	char const CMPIPROVIFC_PROV_LOCATION_opt[] = "cmpiprovifc.prov_location";
	char const CMPIPROVIFC_PROV_TTL_opt[] = "cmpiprovifc.prov_TTL";
	char const PERLPROVIFC_PROV_LOCATION_opt[] = "perlprovifc.prov_location";
	char const CHECK_REFERENTIAL_INTEGRITY_opt[] = "owcimomd.check_referential_integrity";
	char const POLLING_MANAGER_MAX_THREADS_opt[] = "owcimomd.polling_manager_max_threads";
	char const HTTP_SERVER_TIMEOUT_opt[] = "http_server.timeout";
	char const MAX_INDICATION_EXPORT_THREADS_opt[] = "owcimomd.max_indication_export_threads";
	char const RESTART_ON_ERROR_opt[] = "owcimomd.restart_on_error";
	char const AUTHORIZATION_LIB_opt[] = "owcimomd.authorization_lib";
	char const AUTHORIZATION2_LIB_opt[] = "owcimomd.authorization2_lib";
	char const HTTP_SERVER_LISTEN_ADDRESSES_opt[] = "http_server.listen_addresses";
	char const INTEROP_SCHEMA_NAMESPACE_opt[] = "owcimomd.interop_schema_namespace";
	char const HTTP_SERVER_UDS_FILENAME_opt[] = "http_server.uds_filename";
	char const HTTP_SERVER_ALLOW_LOCAL_AUTHENTICATION_opt[] = "http_server.allow_local_authentication";
	char const REMOTEPROVIFC_MAX_CONNECTIONS_PER_URL_opt[] = "remoteprovifc.max_connections_per_url";
	char const ALLOWED_USERS_opt[] = "owcimomd.allowed_users";
	char const HTTP_SERVER_DEFAULT_CONTENT_LANGUAGE_opt[] = "http_server.default_content_language";
	// 3.1 additions.
	char const HTTP_SERVER_SSL_CLIENT_VERIFICATION_opt[] = "http_server.ssl_client_verification";
	char const HTTP_SERVER_SSL_TRUST_STORE[] = "http_server.ssl_trust_store";
	char const ADDITIONAL_LOGS_opt[] = "owcimomd.additional_logs";
	char const DISABLE_CPP_PROVIDER_INTERFACE_opt[] = "owcimomd.disable_cpp_provider_interface";
	// These aren't a whole config option, but log names, which are substituted as part of the log options
	char const LOG_DEBUG_LOG_NAME[] = "debug";
	char const LOG_MAIN_LOG_NAME[] = "main";
	// These log options aren't usable as is, the log name has to be substituted in using Format()
	char const LOG_1_TYPE_opt[] = "log.%1.type";
	char const LOG_1_COMPONENTS_opt[] = "log.%1.components";
	char const LOG_1_CATEGORIES_opt[] = "log.%1.categories";
	char const LOG_1_LEVEL_opt[] = "log.%1.level";
	char const LOG_1_FORMAT_opt[] = "log.%1.format";
	char const LOG_1_LOCATION_opt[] = "log.%1.location";
	char const LOG_1_MAX_FILE_SIZE_opt[] = "log.%1.max_file_size";
	char const LOG_1_MAX_BACKUP_INDEX_opt[] = "log.%1.max_backup_index";
	// 3.2 additions.
	char const OWBI1IFC_PROV_LOCATION_opt[] = "owbi1provifc.prov_location";
	char const OWBI1IFC_PROV_TTL_opt[] = "owbi1provifc.prov_ttl";
	char const LOG_1_FLUSH_opt[] = "log.%1.flush";

	char const PIDFILE_opt[] = "owcimomd.pidfile";
	char const ADDITIONAL_CONFIG_FILES_DIRS_opt[] = "owcimomd.additional_config_files_dirs";

	char const EXPLICIT_REGISTRATION_NAMESPACES_opt[] = "owcimomd.explicit_registration_namespaces";
	char const PRIVILEGES_CONFIG_DIR_opt[] = "owcimomd.privileges_config_dir";
	char const INSTALLED_DEST_DIR_opt[] = "owcimomd.installed_dest_dir";
	char const PRIVILEGE_MONITOR_USE_LIB_PATH_opt[] = "owcimomd.privilege_monitor_use_lib_path";
	char const HTTP_SERVER_DEFAULT_AUTH_CHALLENGE_opt[] = "http_server.default_auth_challenge";
	char const HTTP_SERVER_ALLOW_DIGEST_AUTHENTICATION_opt[] = "http_server.allow_digest_authentication";
	char const HTTP_SERVER_ALLOW_BASIC_AUTHENTICATION_opt[] = "http_server.allow_basic_authentication";
	char const HTTP_SERVER_ALLOW_SPNEGO_AUTHENTICATION_opt[] = "http_server.allow_spnego_authentication";
	char const HTTP_SERVER_AUTHENTICATION_REALM_opt[] = "http_server.authentication_realm";
	char const HTTP_SERVER_SSL_KEY_opt[] = "http_server.SSL_key";
	char const READ_WRITE_LOCK_TIMEOUT_opt[] = "owcimomd.read_write_lock_timeout";

	String installed_owlibexec_dir(OW_DEFAULT_OWLIBEXECDIR);
	String installed_owdata_dir(OW_DEFAULT_STATE_DIR "/openwbem");
	String installed_owlib_dir(OW_DEFAULT_OWLIBDIR);

const NameAndDefault g_defaults[] = 
{
	{ ACL_SUPERUSER_opt ,  "" },
	{ ADDITIONAL_CONFIG_FILES_DIRS_opt, OW_DEFAULT_ADDITIONAL_CONFIG_FILES_DIRS },
	{ ADDITIONAL_LOGS_opt ,  "" },
	{ ALLOW_ANONYMOUS_opt ,  OW_DEFAULT_ALLOW_ANONYMOUS },
	{ ALLOWED_USERS_opt ,  OW_DEFAULT_ALLOWED_USERS },
	{ AUTHENTICATION_MODULE_opt ,  "" },
	{ AUTHORIZATION2_LIB_opt ,  "" },
	{ AUTHORIZATION_LIB_opt ,  "" },
	{ CHECK_REFERENTIAL_INTEGRITY_opt ,  OW_DEFAULT_CHECK_REFERENTIAL_INTEGRITY },
	{ REQUEST_HANDLER_PATH_opt ,  OW_DEFAULT_REQUEST_HANDLER_PATH },
	{ SERVICES_PATH_opt ,  OW_DEFAULT_SERVICES_PATH },
	{ CMPIPROVIFC_PROV_LOCATION_opt ,  OW_DEFAULT_CMPIPROVIFC_PROV_LOCATION },
	{ CMPIPROVIFC_PROV_TTL_opt ,  OW_DEFAULT_CMPIPROVIFC_PROV_TTL },
	{ CONFIG_FILE_opt ,  OW_DEFAULT_CONFIG_FILE },
	{ CPPPROVIFC_PROV_LOCATION_opt ,  OW_DEFAULT_CPPPROVIFC_PROV_LOCATION },
	{ CPPPROVIFC_PROV_TTL_opt ,  OW_DEFAULT_CPPPROVIFC_PROV_TTL },
	{ DATADIR_opt ,  OW_DEFAULT_DATADIR },
	{ DEBUGFLAG_opt ,  OW_DEFAULT_DEBUGFLAG },
#ifndef OW_DISABLE_DIGEST
	{ HTTP_SERVER_DIGEST_PASSWORD_FILE_opt ,  OW_DEFAULT_HTTP_SERVER_DIGEST_PASSWORD_FILE },
#endif
	{ DISABLE_CPP_PROVIDER_INTERFACE_opt ,  OW_DEFAULT_DISABLE_CPP_PROVIDER_INTERFACE },
	{ DISABLE_INDICATIONS_opt ,  OW_DEFAULT_DISABLE_INDICATIONS },
	{ DUMP_SOCKET_IO_opt ,  "" },
	{ EXPLICIT_REGISTRATION_NAMESPACES_opt, "" },
	{ HTTP_SERVER_ENABLE_DEFLATE_opt ,  OW_DEFAULT_HTTP_SERVER_ENABLE_DEFLATE },
	{ HTTP_SERVER_ALLOW_LOCAL_AUTHENTICATION_opt ,  OW_DEFAULT_HTTP_SERVER_ALLOW_LOCAL_AUTHENTICATION },
	{ HTTP_SERVER_ALLOW_DIGEST_AUTHENTICATION_opt ,  "" },
	{ HTTP_SERVER_ALLOW_BASIC_AUTHENTICATION_opt ,  "" },
	{ HTTP_SERVER_ALLOW_SPNEGO_AUTHENTICATION_opt, "" },
	{ HTTP_SERVER_AUTHENTICATION_REALM_opt, "" }, 
	{ HTTP_SERVER_HTTP_PORT_opt ,  OW_DEFAULT_HTTP_SERVER_HTTP_PORT },
	{ HTTP_SERVER_DEFAULT_CONTENT_LANGUAGE_opt ,  OW_DEFAULT_HTTP_SERVER_DEFAULT_CONTENT_LANGUAGE },
	{ HTTP_SERVER_SSL_CLIENT_VERIFICATION_opt ,  OW_DEFAULT_HTTP_SERVER_SSL_CLIENT_VERIFICATION },
	{ HTTP_SERVER_SSL_TRUST_STORE ,  OW_DEFAULT_HTTP_SERVER_SSL_TRUST_STORE },
	{ HTTP_SERVER_HTTPS_PORT_opt ,  OW_DEFAULT_HTTP_SERVER_HTTPS_PORT },
	{ HTTP_SERVER_TIMEOUT_opt ,  OW_DEFAULT_HTTP_SERVER_TIMEOUT },
	{ HTTP_SERVER_USE_DIGEST_opt ,  OW_DEFAULT_HTTP_SERVER_USE_DIGEST },
	{ HTTP_SERVER_DEFAULT_AUTH_CHALLENGE_opt, OW_DEFAULT_HTTP_SERVER_DEFAULT_AUTH_CHALLENGE },
	{ INTEROP_SCHEMA_NAMESPACE_opt ,  OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE },
	{ LIBEXECDIR_opt ,  OW_DEFAULT_OWLIBEXECDIR },
	{ HTTP_SERVER_LISTEN_ADDRESSES_opt ,  OW_DEFAULT_HTTP_SERVER_LISTEN_ADDRESSES },
	{ LOG_1_CATEGORIES_opt ,  "" },
	{ LOG_1_COMPONENTS_opt ,  OW_DEFAULT_LOG_1_COMPONENTS },
	{ LOG_1_FLUSH_opt, OW_DEFAULT_LOG_1_FLUSH },
	{ LOG_1_FORMAT_opt ,  OW_DEFAULT_LOG_1_FORMAT },
	{ LOG_1_LEVEL_opt ,  OW_DEFAULT_LOG_1_LEVEL },
	{ LOG_1_LOCATION_opt ,  "" },
	{ LOG_1_MAX_BACKUP_INDEX_opt , OW_DEFAULT_LOG_1_MAX_BACKUP_INDEX },
	{ LOG_1_MAX_FILE_SIZE_opt ,  OW_DEFAULT_LOG_1_MAX_FILE_SIZE },
	{ LOG_1_TYPE_opt ,  OW_DEFAULT_LOG_1_TYPE },
	{ LOG_DEBUG_LOG_NAME ,  "" },
	{ LOG_LEVEL_opt ,  OW_DEFAULT_LOG_LEVEL },
	{ LOG_LOCATION_opt ,  OW_DEFAULT_LOG_LOCATION },
	{ LOG_MAIN_LOG_NAME ,  "" },
	{ MAX_CLASS_CACHE_SIZE_opt ,  OW_DEFAULT_MAX_CLASS_CACHE_SIZE },
	{ HTTP_SERVER_MAX_CONNECTIONS_opt ,  OW_DEFAULT_HTTP_SERVER_MAX_CONNECTIONS },
	{ MAX_INDICATION_EXPORT_THREADS_opt ,  OW_DEFAULT_MAX_INDICATION_EXPORT_THREADS },
	{ NPIPROVIFC_PROV_LOCATION_opt ,  OW_DEFAULT_NPIPROVIFC_PROV_LOCATION },
	//{ ONLY_HELP_opt ,  OW_DEFAULT_ONLY_HELP },
	{ OWBI1IFC_PROV_LOCATION_opt, OW_DEFAULT_OWBI1IFC_PROV_LOCATION },
	{ OWBI1IFC_PROV_TTL_opt, OW_DEFAULT_OWBI1IFC_PROV_TTL },
	{ OWLIBDIR_opt ,  OW_DEFAULT_OWLIBDIR },
	{ PAM_ALLOWED_USERS_opt ,  "" },
	{ PERLPROVIFC_PROV_LOCATION_opt ,  OW_DEFAULT_PERLPROVIFC_PROV_LOCATION },
	{ PIDFILE_opt, OW_DEFAULT_PIDFILE },
	{ POLLING_MANAGER_MAX_THREADS_opt ,  OW_DEFAULT_POLLING_MANAGER_MAX_THREADS },
	{ PROVIDER_IFC_LIBS_opt ,  OW_DEFAULT_PROVIDER_IFC_LIBS },
	{ REMOTEPROVIFC_MAX_CONNECTIONS_PER_URL_opt ,  OW_DEFAULT_REMOTEPROVIFC_MAX_CONNECTIONS_PER_URL },
	{ REQUEST_HANDLER_TTL_opt ,  OW_DEFAULT_REQUEST_HANDLER_TTL },
	{ RESTART_ON_ERROR_opt ,  OW_DEFAULT_RESTART_ON_ERROR },
	{ HTTP_SERVER_REUSE_ADDR_opt ,  OW_DEFAULT_HTTP_SERVER_REUSE_ADDR },
	{ SIMPLE_AUTH_PASSWORD_FILE_opt ,  OW_DEFAULT_SIMPLE_AUTH_PASSWORD_FILE },
	{ HTTP_SERVER_SINGLE_THREAD_opt ,  OW_DEFAULT_HTTP_SERVER_SINGLE_THREAD },
	{ SLP_ENABLE_ADVERTISEMENT_opt ,  OW_DEFAULT_SLP_ENABLE_ADVERTISEMENT },
	{ HTTP_SERVER_SSL_CERT_opt ,  "" },
	{ HTTP_SERVER_SSL_KEY_opt ,  "" },
	{ HTTP_SERVER_UDS_FILENAME_opt ,  OW_DEFAULT_HTTP_SERVER_UDS_FILENAME },
	{ HTTP_SERVER_USE_UDS_opt ,  OW_DEFAULT_HTTP_SERVER_USE_UDS },
	{ WQL_LIB_opt ,  OW_DEFAULT_WQL_LIB },
	{ PRIVILEGES_CONFIG_DIR_opt, "" },
	{ READ_WRITE_LOCK_TIMEOUT_opt, OW_DEFAULT_READ_WRITE_LOCK_TIMEOUT},
	{ "zz_end" , "garbage" }
}; 

const NameAndDefault* const g_defaultsEnd = &g_defaults[0] + 
	(sizeof(g_defaults)/sizeof(*g_defaults)) - 1; 





} // end namespace ConfigOpts
} // end namespace OpenWBEM


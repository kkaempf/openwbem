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
#include <cstring>

namespace OW_NAMESPACE
{
namespace ConfigOpts
{
const NameAndDefault g_defaults[] = 
{
	{ ACL_SUPERUSER_opt ,  "" },
	{ ADDITIONAL_CONFIG_FILES_DIRS_opt, OW_DEFAULT_ADDITIONAL_CONFIG_FILES_DIRS },
	{ ADDITIONAL_LOGS_opt ,  "" },
	{ ALLOW_ANONYMOUS_opt ,  OW_DEFAULT_ALLOW_ANONYMOUS },
	{ ALLOWED_USERS_opt ,  OW_DEFAULT_ALLOWED_USERS },
	{ AUTHENTICATION_MODULE_opt ,  OW_DEFAULT_AUTHENTICATION_MODULE },
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
	{ DROP_ROOT_PRIVILEGES_opt, OW_DEFAULT_DROP_ROOT_PRIVILEGES },
	{ DUMP_SOCKET_IO_opt ,  "" },
	{ EXPLICIT_REGISTRATION_NAMESPACES_opt, "" },
	{ HTTP_SERVER_ENABLE_DEFLATE_opt ,  OW_DEFAULT_HTTP_SERVER_ENABLE_DEFLATE },
	{ HTTP_SERVER_ALLOW_LOCAL_AUTHENTICATION_opt ,  OW_DEFAULT_HTTP_SERVER_ALLOW_LOCAL_AUTHENTICATION },
	{ HTTP_SERVER_HTTP_PORT_opt ,  OW_DEFAULT_HTTP_SERVER_HTTP_PORT },
	{ HTTP_SERVER_DEFAULT_CONTENT_LANGUAGE_opt ,  OW_DEFAULT_HTTP_SERVER_DEFAULT_CONTENT_LANGUAGE },
	{ HTTP_SERVER_SSL_CLIENT_VERIFICATION_opt ,  OW_DEFAULT_HTTP_SERVER_SSL_CLIENT_VERIFICATION },
	{ HTTP_SERVER_SSL_TRUST_STORE ,  OW_DEFAULT_HTTP_SERVER_SSL_TRUST_STORE },
	{ HTTP_SERVER_HTTPS_PORT_opt ,  OW_DEFAULT_HTTP_SERVER_HTTPS_PORT },
	{ HTTP_SERVER_TIMEOUT_opt ,  OW_DEFAULT_HTTP_SERVER_TIMEOUT },
	{ HTTP_SERVER_USE_DIGEST_opt ,  OW_DEFAULT_HTTP_SERVER_USE_DIGEST },
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
	{ "zz_end" , "garbage" }
}; 

const NameAndDefault* const g_defaultsEnd = &g_defaults[0] + 
	(sizeof(g_defaults)/sizeof(*g_defaults)) - 1; 





} // end namespace ConfigOpts
} // end namespace OpenWBEM


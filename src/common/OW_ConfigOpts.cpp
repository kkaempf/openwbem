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
// keep these sorted.
const NameAndDefault g_defaults[] = 
{
	{ ACL_SUPERUSER_opt ,  "" },
	{ ADDITIONAL_LOGS_opt ,  "" },
	{ ALLOW_ANONYMOUS_opt ,  OW_DEFAULT_ALLOW_ANONYMOUS },
	{ ALLOWED_USERS_opt ,  OW_DEFAULT_ALLOWED_USERS },
	{ AUTH_MOD_opt ,  OW_DEFAULT_AUTH_MOD },
	{ AUTHORIZATION2_LIB_opt ,  "" },
	{ AUTHORIZATION_LIB_opt ,  "" },
	{ CHECK_REFERENTIAL_INTEGRITY_opt ,  OW_DEFAULT_CHECK_REFERENTIAL_INTEGRITY },
	{ CIMOM_REQUEST_HANDLER_LOCATION_opt ,  "" },
	{ CIMOM_SERVICES_LOCATION_opt ,  OW_DEFAULT_CIMOM_SERVICES_LOCATION },
	{ CMPIIFC_PROV_LOC_opt ,  "" },
	{ CMPIIFC_PROV_TTL_opt ,  OW_DEFAULT_CMPIIFC_PROV_TTL },
	{ CONFIG_FILE_opt ,  OW_DEFAULT_CONFIG_FILE },
	{ CPPIFC_PROV_LOC_opt ,  "" },
	{ CPPIFC_PROV_TTL_opt ,  OW_DEFAULT_CPPIFC_PROV_TTL },
	{ DATA_DIR_opt ,  OW_DEFAULT_DATA_DIR },
	{ DEBUG_opt ,  "" },
#ifndef OW_DISABLE_DIGEST
	{ DIGEST_AUTH_FILE_opt ,  "" },
#endif
	{ DISABLE_CPP_PROVIDER_INTERFACE_opt ,  OW_DEFAULT_DISABLE_CPP_PROVIDER_INTERFACE },
	{ DISABLE_INDICATIONS_opt ,  OW_DEFAULT_DISABLE_INDICATIONS },
	{ DROP_ROOT_PRIVILEGES_opt, OW_DEFAULT_DROP_ROOT_PRIVILEGES },
	{ DUMP_SOCKET_IO_opt ,  "" },
	{ ENABLE_DEFLATE_opt ,  OW_DEFAULT_ENABLE_DEFLATE },
	{ HTTP_ALLOW_LOCAL_AUTHENTICATION_opt ,  OW_DEFAULT_HTTP_ALLOW_LOCAL_AUTHENTICATION },
	{ HTTP_PORT_opt ,  OW_DEFAULT_HTTP_PORT },
	{ HTTP_SERVER_DEFAULT_CONTENT_LANGUAGE_opt ,  "" },
	{ HTTP_SERVER_SSL_CLIENT_VERIFICATION_opt ,  OW_DEFAULT_HTTP_SERVER_SSL_CLIENT_VERIFICATION },
	{ HTTP_SERVER_SSL_TRUST_STORE ,  OW_DEFAULT_HTTP_SERVER_SSL_TRUST_STORE },
	{ HTTPS_PORT_opt ,  OW_DEFAULT_HTTPS_PORT },
	{ HTTP_TIMEOUT_opt ,  OW_DEFAULT_HTTP_TIMEOUT },
	{ HTTP_USE_DIGEST_opt ,  OW_DEFAULT_USE_DIGEST },
	{ INTEROP_SCHEMA_NAMESPACE_opt ,  OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE },
	{ LIBEXEC_DIR_opt ,  OW_DEFAULT_LIBEXEC_DIR },
	{ LISTEN_ADDRESSES_opt ,  OW_DEFAULT_LISTEN_ADDRESSES },
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
	{ MAX_CLASS_CACHE_SIZE_opt ,  OW_DEFAULT_MAX_CLASS_CACHE_SIZE_S },
	{ MAX_CONNECTIONS_opt ,  OW_DEFAULT_MAX_CONNECTIONS },
	{ MAX_INDICATION_EXPORT_THREADS_opt ,  OW_DEFAULT_MAX_INDICATION_EXPORT_THREADS },
	{ NPIIFC_PROV_LOC_opt ,  "" },
	//{ ONLY_HELP_opt ,  OW_DEFAULT_ONLY_HELP },
	{ OWBI1IFC_PROV_LOC_opt, OW_DEFAULT_OWBI1IFC_PROV_LOC },
	{ OWBI1IFC_PROV_TTL_opt, OW_DEFAULT_OWBI1IFC_PROV_TTL },
	{ OWLIB_DIR_opt ,  OW_DEFAULT_OWLIB_DIR },
	{ PAM_ALLOWED_USERS_opt ,  "" },
	{ PERLIFC_PROV_LOC_opt ,  "" },
	{ PIDFILE_opt, OW_DEFAULT_PIDFILE },
	{ POLLING_MANAGER_MAX_THREADS_opt ,  OW_DEFAULT_POLLING_MANAGER_MAX_THREADS },
	{ PROVIDER_IFC_LIBS_opt ,  "" },
	{ REMOTEPROVIFC_MAX_CONNECTIONS_PER_URL_opt ,  OW_DEFAULT_REMOTEPROVIFC_MAX_CONNECTIONS_PER_URL },
	{ REQ_HANDLER_TTL_opt ,  OW_DEFAULT_REQ_HANDLER_TTL },
	{ RESTART_ON_ERROR_opt ,  OW_DEFAULT_RESTART_ON_ERROR },
	{ REUSE_ADDR_opt ,  OW_DEFAULT_REUSE_ADDR },
	{ SIMPLE_AUTH_FILE_opt ,  "" },
	{ SINGLE_THREAD_opt ,  OW_DEFAULT_SINGLE_THREAD },
	{ SLP_ENABLE_ADVERTISEMENT_opt ,  OW_DEFAULT_SLP_ENABLE_ADVERTISEMENT },
	{ SSL_CERT_opt ,  "" },
	{ UDS_FILENAME_opt ,  OW_DEFAULT_UDS_FILENAME },
	{ USE_UDS_opt ,  OW_DEFAULT_USE_UDS },
	{ WQL_LIB_opt ,  OW_DEFAULT_WQL_LIB },
	{ "zz_end" , "garbage" }
}; 

const NameAndDefault* const g_defaultsEnd = &g_defaults[0] + 
	(sizeof(g_defaults)/sizeof(*g_defaults)) - 1; 





} // end namespace ConfigOpts
} // end namespace OpenWBEM


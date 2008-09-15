/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Dan Nuffer
 */

#include "OW_config.h"

#include "krb5.h"
#include "vas.h"
#include "vas_gss.h"

#include "OW_Format.hpp"
#include "OW_String.hpp"
#include "OW_DateTime.hpp"
#include "OW_HTTPUtils.hpp"

#include <iostream>
#include <map>

using namespace std;
using namespace OpenWBEM;

// protocol description
// input is on stdin
// output is on stdout
// -> "Negotiate: <base64 data>"
// -> <connection id>
// <- "S" | "F" | "C" - Success, Failure, Continue
// If Success
// <- <username>
// If Failure:
// <- <reason>
// If Continue:
// <- <base64 data>

namespace
{
String
getGssError(const char *pfx, OM_uint32 gsserr, OM_uint32 gsserr_minor)
{
	OM_uint32 seq = 0;
	OM_uint32 major_status, minor_status;
	String rv(pfx);

	/* Use the GSSAPI to obtain the error message text */
	do
	{
		gss_buffer_desc buf = GSS_C_EMPTY_BUFFER;
		major_status = gss_display_status(&minor_status, gsserr,
		                                  GSS_C_GSS_CODE, GSS_C_NO_OID,
		                                  &seq, &buf);
		rv += Format(": %1", String(static_cast<char*>(buf.value), buf.length));
		gss_release_buffer(&minor_status, &buf);
	} while (!GSS_ERROR(major_status) && seq != 0);

	seq = 0;
	/* And the mechanism-specific error */
	do
	{
		gss_buffer_desc buf = GSS_C_EMPTY_BUFFER;
		major_status = gss_display_status(&minor_status, gsserr_minor,
		                                  GSS_C_MECH_CODE, GSS_C_NO_OID,
		                                  &seq, &buf);
		rv += Format(": %1", String(static_cast<char*>(buf.value), buf.length));
		gss_release_buffer(&minor_status, &buf);
	} while (!GSS_ERROR(major_status) && seq != 0);
	return rv;
}

struct Entry
{
	Entry()
		: ctx(GSS_C_NO_CONTEXT)
	{
		creationTime.setToCurrent();
	}
	gss_ctx_id_t ctx;
	DateTime creationTime;
};

typedef std::map<String, Entry> map_t;
map_t g_connections;

gss_ctx_id_t* 
getGssCtx(const String& connectionId)
{
	map_t::iterator i = g_connections.find(connectionId);
	if (i == g_connections.end())
	{
		map_t::value_type v(connectionId, Entry());
		i = g_connections.insert(v).first;
	}
	return &i->second.ctx;
}

void removeGssCtx(map_t::iterator i)
{
	OM_uint32 minor;
	if (GSS_C_NO_CONTEXT != i->second.ctx)
	{
		OM_uint32 gsserr = gss_delete_sec_context(&minor, &i->second.ctx, 0);
		if (gsserr != GSS_S_COMPLETE)
		{
			// hmm, not much to do here.
		}
	}
}

void
cleanGssCtx(const String& connectionId)
{
	map_t::iterator i = g_connections.find(connectionId);
	if (i != g_connections.end())
	{
		removeGssCtx(i);
	}
	DateTime now(DateTime::getCurrent());
	for (i = g_connections.begin(); i != g_connections.end(); /* nothing */)
	{
		DateTime expired(i->second.creationTime);
		expired.addSeconds(60);
		if (expired < now)
		{
			removeGssCtx(i++);
		}
		else
		{
			++i;
		}
	}
}

enum EModeFlag
{
	E_SERVER,
	E_CLIENT
};

EModeFlag mode;

}

int main(int argc, char** argv)
try
{
	if (argc != 2 && argc != 4)
	{
		cerr << "usage: server|client servername clientname" << endl;
		return 1;
	}

	char* serverPrincipalName = 0;
	char* clientPrincipalName = 0;
	if (String(argv[1]) == "server")
	{
		mode = E_SERVER;
	}
	else if (String(argv[1]) == "client")
	{
		mode = E_CLIENT;
		serverPrincipalName = argv[2];
		clientPrincipalName = argv[3];
	}
	else
	{
		cerr << "usage: server|client servername clientname" << endl;
		return 1;
	}

	// make these throw instead of having to check them all the time.
	cin.exceptions(std::ios::badbit | std::ios::failbit);
	cout.exceptions(std::ios::badbit | std::ios::failbit);
#ifdef OW_USE_VAS3_API
	vas_ctx_t* vas_ctx(0);
	vas_err_t rv = vas_ctx_alloc(&vas_ctx);
	if (rv != VAS_ERR_SUCCESS)
	{
		cerr << "vas_ctx_alloc() failed: " << rv << endl;
		return 1;
	}

	vas_id_t* vas_id(0);
	if (mode == E_SERVER)
	{
		rv = vas_id_alloc(vas_ctx, "host/", &vas_id);
	}
	else if (mode == E_CLIENT)
	{
		rv = vas_id_alloc(vas_ctx, clientPrincipalName, &vas_id);
	}

	if (rv != VAS_ERR_SUCCESS)
	{
		const char* errmsg = vas_err_get_string(vas_ctx, rv);
		cerr << "vas_id_alloc() failed: " << rv << ':' << (errmsg != 0 ? errmsg : "unknown") << endl;
		return 1;
	}

	rv = vas_id_establish_cred_keytab(vas_ctx, vas_id, VAS_ID_FLAG_USE_MEMORY_CCACHE, 0);
	if (rv != VAS_ERR_SUCCESS)
	{
		const char* errmsg = vas_err_get_string(vas_ctx, rv);
		cerr << "vas_id_establish_cred_keytab() failed: " << rv << ':' << (errmsg != 0 ? errmsg : "unknown") << endl;
		return 1;
	}

#elif defined(OW_USE_VAS2_API)
	vas_t* vas(0);
	int rv = vas_alloc(&vas, "host/");

	if (rv != 0)
	{
		cerr << "vas_alloc() failed: " << rv << endl;
		return 1;
	}

	rv = vas_opt_set(vas, VAS_OPT_KRB5_USE_MEMKEYTAB, (void*)"1");
	if (rv != 0)
	{
		cerr << "vas_opt_set() failed: " << rv << endl;
		return 1;
	}

	rv = vas_authenticate(vas, 0);
	if (rv != 0)
	{
		cerr << "vas_authenticate() failed: " << rv << endl;
		return 1;
	}
#endif

	while (cin)
	{
		String input = String::getLine(cin);

		String connectionId = String::getLine(cin);

		gss_ctx_id_t* pgss_ctx_id = getGssCtx(connectionId);

		OM_uint32 flags = GSS_C_MUTUAL_FLAG | GSS_C_REPLAY_FLAG | GSS_C_INTEG_FLAG;
		gss_buffer_desc in_token = { input.length(), const_cast<void*>(static_cast<const void*>(input.c_str())) };
		gss_buffer_desc out_token = { 0, 0 };
		OM_uint32 rv = 0;
#ifdef OW_USE_VAS3_API
		if (mode == E_SERVER)
		{
			rv = vas_gss_spnego_accept(vas_ctx, vas_id, 0, pgss_ctx_id, &flags, VAS_GSS_SPNEGO_ENCODING_BASE64, &in_token, &out_token, 0);
		}
		else if (mode == E_CLIENT)
		{
			rv = vas_gss_spnego_initiate(vas_ctx, vas_id, 0, pgss_ctx_id, serverPrincipalName, GSS_C_MUTUAL_FLAG, 
				VAS_GSS_SPNEGO_ENCODING_BASE64, &in_token, &out_token);
		}
#elif defined(OW_USE_VAS2_API)
		// unfortunately vas 2 has broken base64 encoding, so we have to do it.
		Array<char> rawData;
		if (!input.empty())
		{
			rawData = HTTPUtils::base64Decode(input.c_str());
			in_token.value = &rawData[0];
			in_token.length = rawData.size();
		}
		if (mode == E_SERVER)
		{
			rv = vas_gss_spnego_accept(vas, pgss_ctx_id, 0, /*VAS_GSS_SPNEGO_ENCODING_BASE64*/ VAS_GSS_SPNEGO_ENCODING_DER, 
				static_cast<const unsigned char*>(in_token.value), in_token.length, 
				reinterpret_cast<unsigned char**>(&out_token.value), &out_token.length, 0);
		}
		else if (mode == E_CLIENT)
		{
			rv = vas_gss_spnego_initiate(vas, pgss_ctx_id, serverPrincipalName, flags, 
				/*VAS_GSS_SPNEGO_ENCODING_BASE64*/ VAS_GSS_SPNEGO_ENCODING_DER, 
				static_cast<const unsigned char*>(in_token.value), in_token.length, 
				reinterpret_cast<unsigned char**>(&out_token.value), &out_token.length);
		}
		if (out_token.value != 0)
		{
			String base64Out = HTTPUtils::base64Encode(static_cast<const UInt8*>(out_token.value), out_token.length);
			free(out_token.value);
			out_token.value = base64Out.allocateCString(); // it'll get free()d below.
			out_token.length = base64Out.length();
		}
#endif
		switch (rv)
		{
			case GSS_S_COMPLETE:
			{
				// check we got mutual authentication
				if ((flags & GSS_C_MUTUAL_FLAG) != GSS_C_MUTUAL_FLAG)
				{
					cout << "F\n";
					cout << "Mutual authentication not established" << endl;
					continue;
				}

				// Get the client's name
				OM_uint32 minor_status = 0;
				gss_name_t clientName;
				OM_uint32 err = gss_inquire_context(&minor_status, *pgss_ctx_id, &clientName, 0, 0, 0, 0, 0, 0);
				if (err != GSS_S_COMPLETE) 
				{
					cout << "F\n";
					cout << getGssError("gss_inquire_context", err, minor_status) << endl;
					continue;
				}
		
				// Convert the client's name into a visible string
				gss_buffer_desc buf = GSS_C_EMPTY_BUFFER;
				err = gss_display_name(&minor_status, clientName, &buf, 0);
				if (err != GSS_S_COMPLETE) 
				{
					cout << "F\n";
					cout << getGssError("gss_display_name", err, minor_status) << endl;
					continue;
				}
		
				String username(static_cast<const char*>(buf.value), buf.length);

				cout << "S\n";
				cout << username << endl;
				cout << String(static_cast<char*>(out_token.value), out_token.length) << endl;
				
				gss_release_buffer(&minor_status, &buf);
				gss_release_name(&minor_status, &clientName);
				cleanGssCtx(connectionId);
			}
			break;
			
			case GSS_S_CONTINUE_NEEDED:
				cout << "C\n";
				// hmm, vas_gss_spnego_accept doesn't say the out_token data will be null terminated...
				cout << String(static_cast<char*>(out_token.value), out_token.length) << endl;
				break;

			default:
			{
				String err;
#ifdef OW_USE_VAS3_API
				const char* vasErrStr = vas_err_get_string(vas_ctx, rv);
				if (vasErrStr)
				{
					err = vasErrStr;
				}
#elif defined(OW_USE_VAS2_API)
				err = vas_error_str(vas);
#endif
				// replace newlines with spaces.
				size_t idx;
				while ((idx = err.indexOf('\n')) != String::npos)
				{
					err[idx] = ' ';
				}

				cout << "F\n";
				cout << err << endl;
				cleanGssCtx(connectionId);
			}
			break;

		}
		if (out_token.value != 0)
		{
			free(out_token.value);
		}
	}

	return 0;
}
catch (...)
{
	return 1;
}

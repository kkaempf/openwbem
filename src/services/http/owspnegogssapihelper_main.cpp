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
 * @author Dennis Sieben
 */

#include "OW_config.h"

#include "OW_Format.hpp"
#include "OW_String.hpp"
#include "OW_DateTime.hpp"
#include "OW_HTTPUtils.hpp"
// Logger part here must be removed when finished
#include "OW_Logger.hpp"
#include "OW_LogAppender.hpp"
#include "OW_SyslogAppender.hpp"

#include <iostream>
#include <map>

extern "C" {
#include <unistd.h>
#include <stdlib.h>
#ifdef OW_HAVE_KRB5
#include <krb5.h>
#ifdef OW_HEIMDAL
#include <gssapi.h>
#else
#include <gssapi/gssapi.h>
#include <gssapi/gssapi_generic.h>
#include <gssapi/gssapi_krb5.h>
#define GSS_C_NT_USER_NAME gss_nt_user_name
#define GSS_C_NT_HOSTBASED_SERVICE gss_nt_service_name
#endif /* OW_HEIMDAL */
#endif /* OW_HAVE_KRB5 */
}

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
/* This value stands for SPNEGO (1.3.6.1.5.5.2) the calculation
 * of this values is done as described in RFC 2078 3.1
 */
static gss_OID_desc spnego_oid_desc = {6,
                                 (void *) "\x2b\x06\x01\x05\x05\x02"
                                       };

	// The default identifier passed to the syslog appender.
	const char* const SPNEGO_LOG_IDENT = "owspnego";
	const char* const SPNEGO_LOG_FACILITY = "auth";

LogAppenderRef getlogappender()
{
	StringArray components;
	components.push_back("*");

	StringArray categories;
	categories.push_back("*");

	LogAppenderRef logappender = new SyslogAppender(components, categories,
		SyslogAppender::STR_DEFAULT_MESSAGE_PATTERN,
		SPNEGO_LOG_IDENT,
		SPNEGO_LOG_FACILITY);
	return logappender;
}

Logger logger("ow.helper", getlogappender());

// This defines the location of the keytab file
static String keytabfile()
{
	return "/etc/krb5.keytab";
}

String
getGssError(OM_uint32 gsserr, OM_uint32 gsserr_minor, const char *pfx="")
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
		rv += Format("%1", String(static_cast<char*>(buf.value), buf.length));
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


String
getServicename()
{
	// According to the posix standard only 255 characters are allowed for
	// the hostname
	String servicename("HTTP@");
	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)) == -1)
	{
		cerr << "Failed to get hostname for the server" << endl;
		return "";
	}
	return servicename + hostname;
}

int
get_gss_creds(gss_cred_id_t &server_creds,
              String &errormsg)
{
	gss_buffer_desc token = GSS_C_EMPTY_BUFFER;
	OM_uint32 major_status, minor_status, minor_status2;
	gss_name_t server_name = GSS_C_NO_NAME;
	String servicename(getServicename());
	
	token.value = (void *)servicename.allocateCString();
	token.length = servicename.length();

	major_status = gss_import_name(&minor_status, &token, 
	                               GSS_C_NT_HOSTBASED_SERVICE,
	                               &server_name);
	
	// Better override the date inside the token
	memset(&token, 0, sizeof(token));

	if (GSS_ERROR(major_status))
	{
		errormsg = "gss_import_name() failed: " + 
		           getGssError(major_status, minor_status);
		return 1;
	}

	major_status = gss_display_name(&minor_status, server_name, &token,
	                                NULL);

	if (GSS_ERROR(major_status))
	{
		// Comment from mod_auth_kerb
		/* Perhaps we could just ignore this error but it's safer to give
		 * up now, I think */
		OW_LOG_DEBUG(logger, Format("IMPORTANT: gss_display_name() failed: "
		             "%1", getGssError(major_status, minor_status)));

		errormsg = "gss_display_name() failed: " + 
		           getGssError(major_status, minor_status);
		return 1;
	}

	String temp(static_cast<char*>(token.value));
	OW_LOG_DEBUG(logger, Format("IMPORTANT: get_gss_creds Acquiring creds "
	             "for : %1", temp));

	gss_release_buffer(&minor_status, &token);

	major_status = gss_acquire_cred(&minor_status, server_name,
	                                GSS_C_INDEFINITE, GSS_C_NO_OID_SET,
	                                GSS_C_ACCEPT, &server_creds, NULL,
	                                NULL);

	if (gss_release_name(&minor_status2, &server_name) != GSS_S_COMPLETE)
	{
		errormsg = "gss_release_name() for the servername failed";
		return 1;
	}

	if (GSS_ERROR(major_status))
	{
		errormsg = "gss_acquire_cred() failed: " +
		           getGssError(major_status, minor_status);
		return 1;
	}
   
	return 0;
}

/* 
 * Despite input_token is just an input parameter we can't declare it const,
 * because gss_accept_sec_context accepts this input parameter only without
 * const
 */
OM_uint32
gssapi_spnego_accept(gss_buffer_desc &input_token,
                     gss_ctx_id_t &context,
                     OM_uint32 &ret_flags,
                     gss_buffer_desc &output_token,
                     gss_cred_id_t &server_creds,
                     String &errormsg)
{
	OM_uint32 major_status, minor_status;
	gss_name_t client_name = GSS_C_NO_NAME;
	gss_cred_id_t delegated_cred = GSS_C_NO_CREDENTIAL;

	major_status = gss_accept_sec_context(&minor_status,
	                                      &context,
	                                      server_creds,
	                                      &input_token,
	                                      GSS_C_NO_CHANNEL_BINDINGS,
	                                      &client_name,
	                                      NULL,
	                                      &output_token,
	                                      &ret_flags,
	                                      NULL,
	                                      &delegated_cred);

	OW_LOG_DEBUG(logger, Format("Length of output_token: %1",
	                            output_token.length));
	OW_LOG_DEBUG(logger, Format("Value of output_token: %1",
	                            static_cast<char*>(output_token.value)));
	
	if (GSS_ERROR(major_status))
	{
		errormsg = Format("gss_accept_sec_context() failed %1",
		                  getGssError(major_status, minor_status));
		return major_status;
	}
	
	if (delegated_cred != GSS_C_NO_CREDENTIAL)
	{
		// The client side send us a ticket with a credential which should be
		// delegated. This makes no sense here and is therefore not supported.
		// So here no code...
		// ... maybe to be added in the future
	}

	if (delegated_cred)
	{
		gss_release_cred(&minor_status, &delegated_cred);
	}
	return major_status;
}

/*
 * The String for the errormsg in this function is necessary because you can't
 * get the error message from the plain major_status. So you have to return
 * either the minor_status, too, or the direct error message. I've preferred the
 * errormessage, because here I have the ability, to give also feedback which
 * function has failed
 */
/* 
 * Despite input_token is just an input parameter we can't declare it const,
 * because gss_accept_sec_context accepts this input parameter only without
 * const
 */
OM_uint32
gssapi_spnego_init(gss_buffer_desc &input_token,
                   gss_ctx_id_t &context,
                   const OM_uint32 &req_flags,
                   gss_buffer_desc &output_token,
		   const String &principal,
                   String &errormsg)
{
	OM_uint32 major_status, minor_status;
	gss_buffer_desc input_name;
	gss_name_t server;

	input_name.value = (void *)principal.allocateCString();
	input_name.length = principal.length();

	major_status = gss_import_name(&minor_status,
	                               &input_name,
	                               GSS_C_NT_HOSTBASED_SERVICE,
	                               &server);

	memset(&input_name, 0, sizeof(input_name));

	if (GSS_ERROR(major_status))
	{
		errormsg = Format("gss_import_name failed: %1",
		                  getGssError(major_status, minor_status));
		return major_status;
	}

	// Here we decide which version to take
	// The call of one of this methods will block the client for about 25
	// seconds if no KDC can be reached
	
	major_status = gss_init_sec_context(&minor_status,
	                                    GSS_C_NO_CREDENTIAL,
	                                    &context,
	                                    server,
	                                    &spnego_oid_desc,
	                                    req_flags,
	                                    GSS_C_INDEFINITE,
	                                    GSS_C_NO_CHANNEL_BINDINGS,
	                                    &input_token,
	                                    NULL,
	                                    &output_token,
	                                    NULL,
	                                    NULL
	                                    );
	
	OW_LOG_DEBUG(logger, Format("Length of client output token %1",
	                            output_token.length));

	OW_LOG_DEBUG(logger, Format("Value of client output token %1",
	                            static_cast<char*>(output_token.value)));
	if (GSS_ERROR(major_status))
	{
		errormsg = Format("gss_init_sec_context failed: %1",
		                  getGssError(major_status, minor_status));
		gss_release_name(&minor_status, &server);
		return major_status;
	}

	gss_release_name(&minor_status, &server);
	return major_status;
}

enum EModeFlag
{
	E_SERVER,
	E_CLIENT
};

EModeFlag mode;

} // End of anonymous namespace

int main(int argc, char** argv)
try
{
	
	if (argc != 2 && argc != 4)
	{
		cerr << "usage: server|client servername clientname" << endl;
		return 1;
	}

	// Here we define if this binary is used as client or server part
	char* serverPrincipalName = 0;
	char* servicePrincipalName = 0;
	if (String(argv[1]) == "server")
	{
		mode = E_SERVER;
	}
	else if (String(argv[1]) == "client")
	{
		mode = E_CLIENT;
		serverPrincipalName = argv[2];
		servicePrincipalName = argv[3];
	}
	else
	{
		cerr << "usage: server|client servername clientname" << endl;
		return 1;
	}

	// make these throw instead of having to check them all the time.
	cin.exceptions(std::ios::badbit | std::ios::failbit);
	cout.exceptions(std::ios::badbit | std::ios::failbit);
	gss_cred_id_t server_creds = GSS_C_NO_CREDENTIAL;

	if (mode == E_SERVER)
	{
		// This is the code to specify the keytab file
		String ktname("KRB5_KTNAME=" + keytabfile());
		putenv(ktname.allocateCString());

		// This function sets the location of the keytab file. By default we
		// use /etc/krb5.keytab here. See the keytabfile function.
		// ATTENTION: This is no standarized GSSAPI function so maybe it
		// doesn't work with implementation other that MIT or Heimdal Kerberos
		gsskrb5_register_acceptor_identity(keytabfile().allocateCString());
	}
	else
	{
		// Nothing to do for the client before we have input
	}


	while (cin)
	{
		String input = String::getLine(cin);

		String connectionId = String::getLine(cin);

		gss_buffer_desc input_token = GSS_C_EMPTY_BUFFER;
		gss_buffer_desc output_token = GSS_C_EMPTY_BUFFER;
		
		gss_ctx_id_t* pcontext = getGssCtx(connectionId);
		String errormsg;

		/* Here we set the flags for the security context. Available are:
		 * 
		 * GSS_C_DELEG_FLAG - Delegate credentials to the remote peer.
		 * GSS_C_MUTUAL_FLAG - Request that the remote peer authenticate
		 *                     itself.
		 * GSS_C_REPLAY_FLAG -  Enable replay detection for messages
		 *                      protected with gss_wrap or gss_get_mic.
		 * GSS_C_SEQUENCE_FLAG -  Enable detection of out-of-sequence
		 *                        protected messages.
		 * GSS_C_CONF_FLAG -  Request that confidentiality service be made
		 *                    available (by calling gss_wrap).
		 * GSS_C_INTEG_FLAG - Request that integrity service be made
		 *                    available (by calling either gss_get_mic or
		 *                    gss_wrap).
		 * GSS_C_ANON_FLAG -  Do not reveal the initiator's identity to the
		 *                    acceptor.
		 */
		OM_uint32 flags = GSS_C_MUTUAL_FLAG |
		                  GSS_C_REPLAY_FLAG |
		                  GSS_C_INTEG_FLAG;

		if (input.length() == 0 && mode == E_SERVER)
		{
			OW_LOG_DEBUG(logger, "IMPORTANT: Token empty");
			cerr << "Token empty" << endl;
			return 1;
		}
		Array<char> rawData;
		if (!input.empty())
		{
			rawData = HTTPUtils::base64Decode(input.c_str());
			input_token.value = &rawData[0];
			input_token.length = rawData.size();
		}
		
		OM_uint32 rv;

		if (mode == E_SERVER)
		{
			if (get_gss_creds(server_creds, errormsg) != 0)
			{
				OW_LOG_DEBUG(logger,
				             Format("IMPORTANT: server-part: "
					            "An Error occured in "
				                    "get_gss_creds: %1", errormsg));
				// Signal this constant value from GSSAPI so
				// that in the final switch statement the
				// default entry is used.
				rv = GSS_S_BAD_NAME;
			}
			else
			{
				OW_LOG_DEBUG(logger, "Server started");
				rv = gssapi_spnego_accept(input_token,
				                          *pcontext,
				                          flags,
				                          output_token,
				                          server_creds,
				                          errormsg);
				OM_uint32 minor_status;
				if (GSS_ERROR(gss_release_cred(&minor_status,
				                               &server_creds)))
				{
					OW_LOG_DEBUG(logger, "Could not delete "
					                     "server "
					                     "credentials");
				}
				OW_LOG_DEBUG(logger, Format("Server finished "
				                            "with %1", rv));
			}
		}
		else
		{
			// Here we first strip of the trailing slash
			String servPrinc(servicePrincipalName);
			servPrinc.erase(servPrinc.length()-1);
			String principal(servPrinc + "@" +
			                        String(serverPrincipalName));
			OW_LOG_DEBUG(logger, Format("Client Input: %1",
			                            String(static_cast<char*>
			                                   (input_token.value),
			                                   input_token.length)));
			OW_LOG_DEBUG(logger, "Client started");
			rv = gssapi_spnego_init(input_token,
			                        *pcontext,
			                        flags,
			                        output_token,
						principal,
			                        errormsg);
			OW_LOG_DEBUG(logger, Format("Client finished with %1", rv));
		}
		if (output_token.value != 0)
		{
			String base64Out = HTTPUtils::base64Encode(static_cast<const UInt8*>
			                                           (output_token.value),
			                                           output_token.length);
			free(output_token.value);
			// it'll get free()d below.
			output_token.value = base64Out.allocateCString();
			output_token.length = base64Out.length();
		}

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
				OM_uint32 err = gss_inquire_context(&minor_status,
				                                    *pcontext,
				                                    &clientName,
				                                    0,
				                                    0,
				                                    0,
				                                    0,
				                                    0,
				                                    0);
				if (err != GSS_S_COMPLETE) 
				{
					cout << "F\n";
					cout << getGssError(err, minor_status,"gss_inquire_context")
					     << endl;
					continue;
				}
		
				// Convert the client's name into a visible string
				gss_buffer_desc buf = GSS_C_EMPTY_BUFFER;
				err = gss_display_name(&minor_status, clientName, &buf, 0);
				if (err != GSS_S_COMPLETE) 
				{
					cout << "F\n";
					cout << getGssError(err, minor_status, "gss_display_name")
					     << endl;
					continue;
				}
		
				String username(static_cast<const char*>(buf.value), buf.length);

				cout << "S\n";
				cout << username << endl;
				cout << String(static_cast<char*>(output_token.value),
				               output_token.length)
				     << endl;
				OW_LOG_DEBUG(logger, Format("Value of output token in success: %1",
				                            String(static_cast<char*>(output_token.value),
				                                   output_token.length)));
				
				gss_release_buffer(&minor_status, &buf);
				gss_release_name(&minor_status, &clientName);
				cleanGssCtx(connectionId);
			}
			break;
			
			case GSS_S_CONTINUE_NEEDED:
				cout << "C\n";
				/* The case that the output_token is != 0 for the server part is
				 * also covered in this step. So we needn't check for that too.
				 */
				cout << String(static_cast<char*>(output_token.value),
				               output_token.length)
				     << endl;
				break;

			default:
			{
				// Hier erfolgt die Fehlerausgabe
				cout << "F\n";
				cout << "Errorcode: " << rv << " Message: " << errormsg << endl;
				cleanGssCtx(connectionId);
			}
			break;

		}
		if (output_token.value != 0)
		{
			free(output_token.value);
		}
	}
	
	return 0;
}
catch (const exception &e)
{
	OW_LOG_DEBUG(logger, Format("An exception occured. Message is %1",e.what()));
	return 1;
}

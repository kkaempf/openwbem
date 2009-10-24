/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "blocxx/UnnamedPipe.hpp"
#include "blocxx/String.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C"
{
#if defined OW_HAVE_PAM_PAM_APPL_H
#include <pam/pam_appl.h>
#elif defined OW_HAVE_SECURITY_PAM_APPL_H
#include <security/pam_appl.h>
#endif
#if defined OW_HAVE_PAM_PAM_MISC_H
#include <pam/pam_misc.h>
#elif defined OW_HAVE_SECURITY_PAM_MISC_H
#include <security/pam_misc.h>
#endif
}

using namespace OW_NAMESPACE;

#if !defined(_pam_overwrite)
#define _pam_overwrite(x)        \
do {                             \
	register char *__xx__;       \
	if ((__xx__=(x)))            \
	{                            \
		while (*__xx__)          \
		{                        \
			*__xx__++ = '\0';    \
		}                        \
	}                            \
} while (0)

#endif


//////////////////////////////////////////////////////////////////////////////
#if defined(OW_HPUX) || defined(OW_SOLARIS) || defined(OW_AIX)
int
MY_PAM_conv(int num_msg, struct pam_message **msgm, struct pam_response **response, void *appdata_ptr)
#else
int
MY_PAM_conv(int num_msg, const struct pam_message **msgm, struct pam_response **response, void *appdata_ptr)
#endif
{
	int count=0;
	struct pam_response *reply;
	if (num_msg <= 0)
	{
		return PAM_CONV_ERR;
	}
	//D(("allocating empty response structure array."));
	reply = static_cast<struct pam_response *>(calloc(num_msg, sizeof(struct pam_response)));
	if (reply == NULL)
	{
		//D(("no memory for responses"));
		return PAM_CONV_ERR;
	}
	bool failed(false);
	//D(("entering conversation function."));
	for (count=0; count < num_msg; ++count)
	{
		char *string=NULL;
		if (failed == true)
		{
			break;
		}
		switch (msgm[count]->msg_style)
		{
			case PAM_PROMPT_ECHO_OFF:
				string = reinterpret_cast<char*>(appdata_ptr);
				if (string == NULL)
				{
					failed = true;
				}
				break;
				/*case PAM_PROMPT_ECHO_ON:
					string = read_string(CONV_ECHO_ON,msgm[count]->msg);
					if (string == NULL) {
						goto failed_conversation;
					}
					break;
				case PAM_ERROR_MSG:
					if (fprintf(stderr,"%s\n",msgm[count]->msg) < 0) {
						goto failed_conversation;
					}
					break;
				case PAM_TEXT_INFO:
					if (fprintf(stderr,"%s\n",msgm[count]->msg) < 0) {
						goto failed_conversation;
					}
					break;
				case PAM_BINARY_PROMPT:
					{
						void *pack_out=NULL;
						const void *pack_in = msgm[count]->msg;

						if (!pam_binary_handler_fn
								|| pam_binary_handler_fn(pack_in, &pack_out) != PAM_SUCCESS
								|| pack_out == NULL) {
							goto failed_conversation;
						}
						string = (char *) pack_out;
						pack_out = NULL;

						break;
					}*/
			default:
				fprintf(stderr, "erroneous conversation (%d)\n"
						  ,msgm[count]->msg_style);
				failed = true;
		}
		if (string)
		{								  /* must add to reply array */
			/* add string to list of responses */
			reply[count].resp_retcode = 0;
			reply[count].resp = string;
			string = NULL;
		}
	}
	/* New (0.59+) behavior is to always have a reply - this is
		compatable with the X/Open (March 1997) spec. */
	if (!failed)
	{
		*response = reply;
		reply = NULL;
	}
	else
	{
		if (reply)
		{
			for (count=0; count<num_msg; ++count)
			{
				if (reply[count].resp == NULL)
				{
					continue;
				}
				switch (msgm[count]->msg_style)
				{
					/*case PAM_PROMPT_ECHO_ON:*/
					case PAM_PROMPT_ECHO_OFF:
						_pam_overwrite(reply[count].resp);
						free(reply[count].resp);
						break;
					/*case PAM_BINARY_PROMPT:
						pam_binary_handler_free((void **) &reply[count].resp);
						break;
					case PAM_ERROR_MSG:
					case PAM_TEXT_INFO:
						// should not actually be able to get here...
						free(reply[count].resp);*/
				} // switch
				reply[count].resp = NULL;
			} // for
			free(reply);
			reply = NULL;
		} // if (reply)
		return PAM_CONV_ERR;
	} // else
	return PAM_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
bool
authenticate(const char* userName,
					const char* password)
{
	char* pPasswd = strdup(password);
	char* pUserName = strdup(userName);
	struct pam_conv conv = {
		MY_PAM_conv,
		pPasswd
	};
	pam_handle_t *pamh=NULL;
	int rval;
	rval = pam_start(OW_PACKAGE_PREFIX"openwbem", pUserName, &conv, &pamh);
	if (rval == PAM_SUCCESS)
	{
		rval = pam_authenticate(pamh, 0);	 /* is user really user? */
	}
	if (rval == PAM_SUCCESS)
	{
		rval = pam_acct_mgmt(pamh, 0);		 /* permitted access? */
	}
	if (rval == PAM_CONV_ERR)
	{
		pam_end(pamh, rval);
		free(pUserName);
		return false;
	}
	if (pam_end(pamh,rval) != PAM_SUCCESS)
	{		// close Linux-PAM
		pamh = NULL;
		return false;
	}
	free(pUserName);
	return( rval == PAM_SUCCESS ? true : false );		 /* indicate success */
}


//////////////////////////////////////////////////////////////////////////////
int
main()
{
	String name, passwd;
	UnnamedPipeRef stdIn = UnnamedPipe::createStdin();
	UnnamedPipeRef stdOut = UnnamedPipe::createStdout();
	while(true)
	{
		if (stdIn->readString(name) == -1)
		{
			// Error. do something?
			break;
		}

		if (stdIn->readString(passwd) == -1)
		{
			// Error. do something?
			break;
		}

		int authcc = (authenticate(name.c_str(), passwd.c_str())) ? 1 : 0;
		stdOut->writeInt(authcc);
	}
}


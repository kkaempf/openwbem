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

#include "OW_config.h"
#include "OW_ConfigOpts.hpp"
#include "OW_String.hpp"
#include "OW_AuthenticatorIFC.hpp"
#include <string.h>

extern "C"
{
#include <security/pam_appl.h>
#include <security/pam_misc.h>
}


/**
 * This is the Linux-PAM "conversation" function.
 * Refer to
 * http://www.kernel.org/pub/linux/libs/pam/Linux-PAM-html/pam_appl.html
 * @param appdata_ptr A pointer to the OW_HTTPReader instance which
 * 			called OW_OS::loadAuthenticator, which in turn called
 * 			pam_authenticate, which calls PamConv
 */
static int OW_PAM_conv(int num_msg, const struct pam_message **msgm,
							  struct pam_response **response, void *appdata_ptr);

class OW_LinuxPAMAuthentication : public OW_AuthenticatorIFC
{
	/**
	 * Authenticates a user
	 *
	 * @param userName
	 *   The name of the of the user being authenticated
	 * @param info
	 *   The authentication credentials
	 * @param details
	 *	An out parameter used to provide information as to why the
	 *   authentication failed.
	 * @return
	 *   True if user is authenticated
	 */
private:
	virtual OW_Bool doAuthenticate(OW_String &userName, const OW_String &info,
		OW_String &details);
	
	virtual void doInit(OW_ServiceEnvironmentIFCRef env)
	{
		m_allowedUsers = env->getConfigItem(OW_ConfigOpts::PAM_ALLOWED_USERS_opt);
	}

	OW_String m_allowedUsers;
};

// See misc_conv.c in libpam for an example.

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_LinuxPAMAuthentication::doAuthenticate(OW_String &userName, const OW_String &info, OW_String &details)
{
	if (info.length() < 1)
	{
		details = "You must authenticate to access this resource";
		return OW_Bool(false);
	}

	OW_Array<OW_String> allowedUsers = m_allowedUsers.tokenize();

	bool nameFound = false;
	for (size_t i = 0; i < allowedUsers.size(); i++)
	{
		if (allowedUsers[i].equals(userName))
		{
			nameFound = true;
			break;
		}
	}
	if (!nameFound)
	{
		details = "You must authenticate to access this resource";
		return OW_Bool(false);
	}

	char* pPasswd = strdup(info.c_str());
	char* pUserName = strdup(userName.c_str());

	struct pam_conv conv = {
		OW_PAM_conv,
		pPasswd
	};

	pam_handle_t *pamh=NULL;
	int rval;

	rval = pam_start("openwbem", pUserName, &conv, &pamh);

	if (rval == PAM_SUCCESS)
		rval = pam_authenticate(pamh, 0);	 /* is user really user? */

	if (rval == PAM_SUCCESS)
		rval = pam_acct_mgmt(pamh, 0);		 /* permitted access? */

	if (rval == PAM_CONV_ERR)
	{
		pam_end(pamh, rval);
		free(pUserName);
        details = "Error in Linux-PAM conversation function";
		return false;
	}


	if (pam_end(pamh,rval) != PAM_SUCCESS)
	{		// close Linux-PAM
		pamh = NULL;
		details = "Unable to close PAM transaction";
		return false;
	}

	free(pUserName);

	OW_Bool retval = ( rval == PAM_SUCCESS ? true : false ); /* indicate success */

	return retval;
}

//////////////////////////////////////////////////////////////////////////////
// Static
// TODO clean up, remove all stuff we don't support.
int
OW_PAM_conv(int num_msg, const struct pam_message **msgm,
				struct pam_response **response, void *appdata_ptr)
{
	int count=0;
	struct pam_response *reply;

	if (num_msg <= 0)
		return PAM_CONV_ERR;

	//D(("allocating empty response structure array."));

	reply = (struct pam_response *) calloc(num_msg,
														sizeof(struct pam_response));
	if (reply == NULL)
	{
		//D(("no memory for responses"));
		return PAM_CONV_ERR;
	}
	OW_Bool failed = false;

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
					if (fprintf(stdout,"%s\n",msgm[count]->msg) < 0) {
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
				//fprintf(stderr, "erroneous conversation (%d)\n"
				//		  ,msgm[count]->msg_style);
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
	
OW_AUTHENTICATOR_FACTORY(OW_LinuxPAMAuthentication);



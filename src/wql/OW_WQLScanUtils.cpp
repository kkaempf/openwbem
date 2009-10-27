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
 * @author Dan Nuffer
 */

/*-------------------------------------------------------------------------
 *
 * Portions Copyright (c) 1996-2001, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *-------------------------------------------------------------------------
 */
#include "OW_WQLScanUtils.hpp"
#include "blocxx/String.hpp"
#include "blocxx/StringBuffer.hpp"

namespace OW_NAMESPACE
{

blocxx::String
WQLRemoveStringEscapes(const blocxx::String& s)
{
	if (s.empty())
	{
		return s;
	}
	blocxx::StringBuffer newStr(s.length());
	int len = s.length();
	for (int i = 0; i < len; i++)
	{
		if (s[i] == '\'')
		{
			/*
			 * Note: if scanner is working right, unescaped quotes can
			 * only appear in pairs, so there should be another character.
			 */
			i++;
			newStr += s[i];
		}
		else if (s[i] == '\\')
		{
			i++;
			switch (s[i])
			{
				case 'b':
					newStr += '\b';
					break;
				case 'f':
					newStr += '\f';
					break;
				case 'n':
					newStr += '\n';
					break;
				case 'r':
					newStr += '\r';
					break;
				case 't':
					newStr += '\t';
					break;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					{
						int			k;
						long		octVal = 0;
						for (k = 0;
							 s[i + k] >= '0' && s[i + k] <= '7' && k < 3;
							 k++)
							octVal = (octVal << 3) + (s[i + k] - '0');
						i += k - 1;
						newStr += (static_cast<char>(octVal));
					}
					break;
				default:
					newStr += s[i];
					break;
			}					/* switch */
		}						/* s[i] == '\\' */
		else
			newStr += s[i];
	}
	return newStr.releaseString();
}

} // end namespace OW_NAMESPACE


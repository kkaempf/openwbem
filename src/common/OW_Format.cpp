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
#include "OW_Format.hpp"

OW_Format::operator OW_String() const
{
	return oss.toString();
}

OW_String OW_Format::toString() const
{
	return oss.toString();
}

const char* OW_Format::c_str() const
{
	return oss.c_str();
}

char OW_Format::process(OW_String& str, char numArgs)
{
	int i, len(str.length());
	char c(' ');
	OW_Bool err = false;


	for (i=0; i<len && c == ' ' && !err; ++i)
	{
		switch(str[i])
		{
			case '%':
				if (i + 1 < len)
				{
					++i;
					switch(str[i])
					{
						case '1': case '2': case '3': case '4': case '5': 
						case '6': case '7': case '8': case '9':
							c = str[i]; 
							break;
						case '%': 
							oss << str[i]; 
							break;
						default: 
							err = true;
					} // inner switch
				} else err = true; 
				break;
			default: 
				oss << str[i];
				break;
		} // outer switch
	} // for

	if ( i <= len && c > numArgs )
	{
		oss << "\n*** Parameter specifier too large.";
		err = true;
	}
	if (err)
	{
		oss << "\n*** Error in format string at \"" << str.substring(i-1) << "\".\n";
		str.erase();
		return '0';
	}
	str.erase(0, i);
	return c;
} // process



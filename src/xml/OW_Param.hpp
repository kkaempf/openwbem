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

//
// OW_Param.hpp
//

#ifndef OW_PARAM_HPP_INCLUDE_GUARD_
#define OW_PARAM_HPP_INCLUDE_GUARD_

#include	"OW_config.h"
#include	"OW_String.hpp"
#include	"OW_Array.hpp"

//
// Param allows parameters to be built for transmission
// in an XML request
//
class OW_Param
{
public:    
    static const int VALUENOTSET = 0;
    static const int VALUESET = 1;
	    
    OW_Param(OW_String argname, int mode, OW_String value);
    OW_Param(OW_String argname, OW_String value);
    OW_Param(OW_String argname, OW_Bool value);
	    
    OW_String getArgName() const;
    OW_String getArgValue() const;

private:
	OW_String m_argName;
    OW_String m_argValue;
    int m_argMode;
};

typedef OW_Array<OW_Param> OW_ParamArray;

#endif



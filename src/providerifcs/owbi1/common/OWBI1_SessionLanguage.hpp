/*******************************************************************************
* Copyright (C) 2001-2004 Novell, Inc. All rights reserved.
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
 * @author Jon Carey
 */

#ifndef OWBI1_SESSIONLANGUAGE_HPP_INCLUDE_GUARD
#define OWBI1_SESSIONLANGUAGE_HPP_INCLUDE_GUARD

#include "OWBI1_config.h"
#include "OWBI1_String.hpp"
#include "OWBI1_Array.hpp"
#include "OWBI1_IntrusiveCountableBase.hpp"
#include "OWBI1_CommonFwd.hpp"

namespace OWBI1
{

//////////////////////////////////////////////////////////////////////////////
class OWBI1_OWBI1PROVIFC_API SessionLanguage : public IntrusiveCountableBase
{
public:
	virtual ~SessionLanguage();
	
	/**
	 * Evaluate the best language to use, based upon what the http client specified, 
	 * as represented by this SessionLanguage object, and what the caller supports.
	 * @param languages The set of languages the caller supports.
	 * @param defaultLanguage The default language returned if no match is made.
	 */
	virtual String getBestLanguage(const StringArray& languages, const String& defaultLanguage = String()) const = 0;
	virtual void addContentLanguage(const String& contentLanguage) = 0;
	virtual String getContentLanguage() const = 0;

};


}	// end namespace OWBI1

#endif


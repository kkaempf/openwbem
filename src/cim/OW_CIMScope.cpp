/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
*  - Neither the name of Vintela, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc. OR THE CONTRIBUTORS
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
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_CIMScope.hpp"
#include "OW_String.hpp"
#include "OW_BinarySerialization.hpp"

namespace OW_NAMESPACE
{

using std::istream;
using std::ostream;
//////////////////////////////////////////////////////////////////////////////					
String
CIMScope::toString() const
{
	return toMOF();
}
//////////////////////////////////////////////////////////////////////////////					
String
CIMScope::toMOF() const
{
	switch (m_val)
	{
		case SCHEMA: 
		return "schema"; 
		break;
		
		case CLASS: 
		return "class"; 
		break;
		
		case ASSOCIATION: 
		return "association"; 
		break;
		
		case INDICATION: 
		return "indication"; 
		break;
		
		case QUALIFIER: 
		return "qualifier"; 
		break;
		
		case PROPERTY: 
		return "property"; 
		break;
		
		case REFERENCE: 
		return "reference"; 
		break;
		
		case METHOD: 
		return "method"; 
		break;
		
		case PARAMETER: 
		return "parameter"; 
		break;
		
		case ANY: 
		return "any"; 
		break;
		
		default: 
		return "BAD SCOPE"; 
		break;
	}

}
//////////////////////////////////////////////////////////////////////////////					
void
CIMScope::readObject(istream &istrm)
{
	// Don't do this, it'll double the size CIMBase::readSig( istrm, CIMSCOPESIG );
	UInt32 v;
	BinarySerialization::readLen(istrm, v);
	m_val = Scope(v);
}
//////////////////////////////////////////////////////////////////////////////					
void
CIMScope::writeObject(ostream &ostrm) const
{
	// Don't do this, it'll double the size CIMBase::writeSig( ostrm, CIMSCOPESIG );
	BinarySerialization::writeLen(ostrm, m_val);
}

//////////////////////////////////////////////////////////////////////////////					
void
CIMScope::setNull() 
{
	m_val = BAD;
}

} // end namespace OW_NAMESPACE


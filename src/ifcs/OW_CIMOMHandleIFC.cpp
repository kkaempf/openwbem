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
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMClassEnumeration.hpp"

const OW_Bool OW_CIMOMHandleIFC::DEEP(true);
const OW_Bool OW_CIMOMHandleIFC::SHALLOW(false);
const OW_Bool OW_CIMOMHandleIFC::INCLUDE_QUALIFIERS(true);
const OW_Bool OW_CIMOMHandleIFC::EXCLUDE_QUALIFIERS(false);
const OW_Bool OW_CIMOMHandleIFC::INCLUDE_CLASS_ORIGIN(true);
const OW_Bool OW_CIMOMHandleIFC::EXCLUDE_CLASS_ORIGIN(false);
const OW_Bool OW_CIMOMHandleIFC::LOCAL_ONLY(true);
const OW_Bool OW_CIMOMHandleIFC::NOT_LOCAL_ONLY(false);

OW_CIMFeatures
OW_CIMOMHandleIFC::getServerFeatures()
{
	return OW_CIMFeatures();
}

void
OW_CIMOMHandleIFC::exportIndication(const OW_CIMInstance& instance,
		const OW_CIMNameSpace& instNS)
{
	OW_THROWCIM(OW_CIMException::FAILED);
	(void)instance;
	(void)instNS;
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMClassEnumBuilder : public OW_CIMClassResultHandlerIFC
	{
	public:
		CIMClassEnumBuilder(OW_CIMClassEnumeration& e) : m_e(e) {}
	protected:
		virtual void doHandleClass(const OW_CIMClass &c)
		{
			m_e.addElement(c);
		}
	private:
		OW_CIMClassEnumeration& m_e;
	};
}
//////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration
OW_CIMOMHandleIFC::enumClassE(const OW_CIMObjectPath& path, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin)
{
	OW_CIMClassEnumeration rval;
	CIMClassEnumBuilder handler(rval);
	enumClass(path, handler, deep, localOnly, includeQualifiers,
		includeClassOrigin);
	return rval;
}



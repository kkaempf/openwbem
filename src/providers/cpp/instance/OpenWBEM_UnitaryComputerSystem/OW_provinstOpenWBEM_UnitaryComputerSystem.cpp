/*******************************************************************************
* Copyright (C) 2004 Novell, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_CppProviderIncludes.hpp"
#include "OW_SocketAddress.hpp"

namespace OpenWBEM
{

	using namespace WBEMFlags;
	using namespace std;

	namespace
	{
		const String COMPONENT_NAME("ow.provider.OpenWBEM_UnitaryComputerSystem");
	}

	class OpenWBEM_UnitaryComputerSystem : public CppReadOnlyInstanceProviderIFC, public CppSimpleInstanceProviderIFC
	{
	public:

		void doSimpleEnumInstances(
			const ProviderEnvironmentIFCRef &env,
			const String &ns,
			const CIMClass &cimClass,
			CIMInstanceResultHandlerIFC &result,
			EPropertiesFlag propertiesFlag)
		{
			CIMInstance newInst = cimClass.newInstance();

			newInst.setProperty("CreationClassName", CIMValue(cimClass.getName()));
			newInst.setProperty("Name", CIMValue(SocketAddress::getAnyLocalHost().getName()));
			if (propertiesFlag == E_ALL_PROPERTIES)
			{
				newInst.setProperty("NameFormat", CIMValue(String("IP")));
				UInt16Array dedicated;
				dedicated.append(1);	// Unknown
				newInst.setProperty("Dedicated", CIMValue(dedicated));

				// Unknown
				newInst.setProperty("ResetCapability", CIMValue(UInt16(2)));
			}

			result.handle(newInst);
		}

		virtual void getInstanceProviderInfo(InstanceProviderInfo &info)
		{
			info.addInstrumentedClass("OpenWBEM_UnitaryComputerSystem");
		}

	};

} // end OpenWBEM namespace.

OW_PROVIDERFACTORY(OpenWBEM::OpenWBEM_UnitaryComputerSystem, owprovinstOpenWBEM_UnitaryComputerSystem)


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
 */ 

#include "OW_CppReadOnlyInstanceProviderIFC.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_String.hpp"
#include "OW_CIMException.hpp"
#include "OW_WBEMFlags.hpp"
#include "OW_CIMDateTime.hpp"
#include "OW_Bool.hpp"
#include "OW_ResultHandlerIFC.hpp"


namespace OpenWBEM 
{

	using namespace WBEMFlags;
	using namespace std; 

	namespace
	{
		const String COMPONENT_NAME("ow.provider.OpenWBEM_UnitaryComputerSystem");
	}

	class OpenWBEM_UnitaryComputerSystem : public CppReadOnlyInstanceProviderIFC
	{
	public:
		virtual void modifyInstance(const ProviderEnvironmentIFCRef &env, 
									const String &ns, 
									const CIMInstance &modifiedInstance, 
									const CIMInstance &previousInstance, 
									WBEMFlags:: EIncludeQualifiersFlag includeQualifiers, 
									const StringArray *propertyList, 
									const CIMClass &theClass)
		{
		}

		virtual void enumInstanceNames(const ProviderEnvironmentIFCRef &env, 
									   const String &ns, 
									   const String &className, 
									   CIMObjectPathResultHandlerIFC &result, 
									   const CIMClass &cimClass)
		{
			(void)env;
			(void)className;
			CIMObjectPath cop(cimClass.getName(), ns);
			cop.setKeyValue("CreationClassName", CIMValue(cimClass.getName()));
			cop.setKeyValue("Name", CIMValue(SocketAddress::getAnyLocalHost().getName())); 
			result.handle(cop); 
		}

		CIMInstance makeInstance(const CIMClass& cimClass)
		{
			CIMInstance newInst = cimClass.newInstance();

			newInst.setProperty("CreationClassName", CIMValue(cimClass.getName()));
			newInst.setProperty("Name", CIMValue(SocketAddress::getAnyLocalHost().getName())); 
			newInst.setProperty("NameFormat", CIMValue(String("IP")));
			UInt16Array dedicated;
			dedicated.append(1);	// Unknown
			newInst.setProperty("Dedicated", CIMValue(dedicated));

			// Unknown
			newInst.setProperty("ResetCapability", CIMValue(UInt16(2)));

			return newInst;
		}


		virtual CIMInstance getInstance(const ProviderEnvironmentIFCRef &env, 
										const String &ns, 
										const CIMObjectPath &instanceName, 
										WBEMFlags:: ELocalOnlyFlag localOnly, 
										WBEMFlags:: EIncludeQualifiersFlag includeQualifiers, 
										WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin, 
										const StringArray *propertyList, 
										const CIMClass &cimClass)
		{
			(void) env;
			(void) ns;
			(void) localOnly;
			(void) includeQualifiers;
			(void) includeClassOrigin;
			(void) propertyList;

			String argHostname;
			String csname = SocketAddress::getAnyLocalHost().getName();
			CIMProperty prop = instanceName.getKey("Name");
			if (prop)
			{
				CIMValue cv = prop.getValue();
				if (cv)
				{
					cv.get(argHostname);
				}
			}

			if (!csname.equalsIgnoreCase(argHostname))
			{
				OW_THROWCIM(CIMException::NOT_FOUND); 
			}

			CIMInstance rval = makeInstance(cimClass);
			return rval; 
		}


		virtual void getInstanceProviderInfo(InstanceProviderInfo &info)
		{
			info.addInstrumentedClass("OpenWBEM_UnitaryComputerSystem"); 
		}

		virtual void initialize(const ProviderEnvironmentIFCRef &)
		{
		}
	};

} // end OpenWBEM namespace. 

OW_PROVIDERFACTORY(OpenWBEM::OpenWBEM_UnitaryComputerSystem, owprovinstOpenWBEM_UnitaryComputerSystem)


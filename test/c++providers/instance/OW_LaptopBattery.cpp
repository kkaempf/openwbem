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

#include "OW_config.h"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_StringStream.hpp"

#include <fstream>
#include <unistd.h>

using namespace OpenWBEM;
using namespace WBEMFlags;

namespace
{


	using std::ifstream;
	using std::ofstream;
	using std::endl;

	const int STAT_Other = 1;
	const int STAT_Unknown = 2;
	const int STAT_Fully_Charged = 3;
	const int STAT_Low = 4;
	const int STAT_Critical = 5;
	const int STAT_Charging = 6;
	const int STAT_Charging_and_High = 7;
	const int STAT_Charging_and_Low = 8;
	const int STAT_Charging_and_Critical = 9;
	const int STAT_Undefined = 10;
	const int STAT_Partially_Charge = 11;

	class LaptopBattery: public CppInstanceProviderIFC
	{
	public:
		/////////////////////////////////////////////////////////////////////////
		virtual void enumInstanceNames(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const String& className,
			CIMObjectPathResultHandlerIFC& result,
			const CIMClass& cimClass )
		{
			(void)env;
			(void)cimClass;
			CIMObjectPath instCop = CIMObjectPath(className, ns);
			char hostbuf[256];
			gethostname(hostbuf, 256);
			String hostname(hostbuf);
			instCop.setKeyValue("SystemCreationClassName",
				CIMValue(String("CIM_System")));
			instCop.setKeyValue("SystemName", CIMValue(hostname));
			instCop.setKeyValue("CreationClassName", CIMValue(className));
			instCop.setKeyValue("DeviceID", CIMValue(String("bat01")));
			result.handle(instCop);
		}

		/////////////////////////////////////////////////////////////////////////
		virtual void enumInstances(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const String& className,
			CIMInstanceResultHandlerIFC& result,
			ELocalOnlyFlag localOnly, 
			EDeepFlag deep, 
			EIncludeQualifiersFlag includeQualifiers, 
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList,
			const CIMClass& requestedClass,
			const CIMClass& cimClass )
		{
			(void)ns;
			(void)className;
			(void)env;
			CIMInstance inst = this->createLaptopBatInst(cimClass);
			result.handle(inst.clone(localOnly, deep, includeQualifiers, 
				includeClassOrigin, propertyList, requestedClass, cimClass));
		}

		/////////////////////////////////////////////////////////////////////////
		virtual CIMInstance getInstance(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const CIMObjectPath& instanceName,
			ELocalOnlyFlag localOnly,
			EIncludeQualifiersFlag includeQualifiers, 
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList, 
			const CIMClass& cimClass )
		{
			(void)ns;
			(void)instanceName;
			(void)env;
			CIMInstance rval = this->createLaptopBatInst(cimClass);
			return rval.clone(localOnly, includeQualifiers, includeClassOrigin, propertyList);
		}

		/////////////////////////////////////////////////////////////////////////
		virtual CIMObjectPath createInstance(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const CIMInstance& cimInstance )
		{

			(void)env;
			(void)ns;
			(void)cimInstance;
			OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support createInstance");
		}

		/////////////////////////////////////////////////////////////////////////
		virtual void modifyInstance(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const CIMInstance& modifiedInstance,
			const CIMInstance& previousInstance,
			EIncludeQualifiersFlag includeQualifiers,
			const StringArray* propertyList,
			const CIMClass& theClass)
		{
			(void)env;
			(void)ns;
			(void)modifiedInstance;
			(void)previousInstance;
			(void)includeQualifiers;
			(void)propertyList;
			(void)theClass;
			OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support modifyInstance");
		}

		/////////////////////////////////////////////////////////////////////////
		virtual void deleteInstance(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const CIMObjectPath& cop)
		{
			(void)env;
			(void)ns;
			(void)cop;
			OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support deleteInstance");
		}

		/**
		 * Fill in the params for a laptop battery instance
		 *
		 * @param cc a LaptopBattery CIMClass
		 *
		 * @return The laptop batter cim instance
		 */
		CIMInstance createLaptopBatInst(const CIMClass& cc);
	};





//////////////////////////////////////////////////////////////////////////////
	CIMInstance
		LaptopBattery::createLaptopBatInst(const CIMClass& cc)
	{
		CIMInstance rval = cc.newInstance();
		rval.setProperty("SystemCreationClassName",
			CIMValue(String("CIM_System")));
		char hostbuf[256];
		gethostname(hostbuf, 256);
		String hostname(hostbuf);
		rval.setProperty("SystemName", CIMValue(hostname));
		rval.setProperty("CreationClassName", CIMValue(cc.getName()));
		rval.setProperty("DeviceID", CIMValue(String("bat01")));

		// /proc/apm typically looks like:
		// 1.16 1.2 0x03 0x00 0x00 0x01 72% 183 min
		// or
		// 1.16 1.2 0x03 0x01 0x03 0x09 93% -1 ?
		ifstream infile("/proc/apm", std::ios::in);
		OStringStream oss;
		oss << infile.rdbuf();
		infile.close();
		String fileContents = oss.toString();
		StringArray toks = fileContents.tokenize();
		Int32 minutes;
		UInt16 percent;
		try
		{
			minutes = toks[7].toInt32();
			toks[6] = toks[6].substring(0, 2); // erase the last %
			percent = toks[6].toUInt16();
		}
		catch (const StringConversionException& e)
		{
			OW_THROWCIMMSG(CIMException::FAILED, "Failed parsing /proc/apm");
		}
		UInt16 status = STAT_Unknown;
		Bool charging = false;
		if (minutes == -1)
		{
			status = STAT_Charging;
			charging = true;
			minutes = 0;
		}
		else
		{
			if (percent > 80)
			{
				status = STAT_Fully_Charged;
			}
			else if (percent > 30)
			{
				status = STAT_Low;
			}
			else
			{
				status = STAT_Critical;
			}
		}
		rval.setProperty("EstimatedChargeRemaining", CIMValue(percent));
		rval.setProperty("EstimatedRunTime", CIMValue(static_cast<UInt32>(minutes)));
		rval.setProperty("BatteryStatus", CIMValue(status));
		rval.setProperty("Charging", CIMValue(charging));
		return rval;
	}

}


OW_PROVIDERFACTORY(LaptopBattery, laptopbat)

	

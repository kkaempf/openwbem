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
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_StringStream.hpp"

namespace
{

#include <fstream>
#include <unistd.h>

	using std::ifstream;
	using std::ofstream;
	using std::endl;

#define STAT_Other 1
#define STAT_Unknown 2
#define STAT_Fully_Charged 3
#define STAT_Low 4
#define STAT_Critical 5
#define STAT_Charging 6
#define STAT_Charging_and_High 7
#define STAT_Charging_and_Low 8
#define STAT_Charging_and_Critical 9
#define STAT_Undefined 10
#define STAT_Partially_Charge 11

	class OW_LaptopBattery: public OW_CppInstanceProviderIFC
	{
	public:
		virtual ~OW_LaptopBattery()
		{
		}

		/////////////////////////////////////////////////////////////////////////
		virtual OW_CIMObjectPathEnumeration enumInstanceNames(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			const OW_Bool& deep,
			const OW_CIMClass& cimClass )
		{
			(void)env;
			(void)cimClass;
			(void)deep;
			OW_CIMObjectPathEnumeration rval;
			OW_CIMObjectPath instCop = cop;
			char hostbuf[256];
			gethostname(hostbuf, 256);
			OW_String hostname(hostbuf);
			instCop.addKey("SystemCreationClassName",
				OW_CIMValue(OW_String("CIM_System")));
			instCop.addKey("SystemName", OW_CIMValue(hostname));
			instCop.addKey("CreationClassName", OW_CIMValue(cop.getObjectName()));
			instCop.addKey("DeviceID", OW_CIMValue(OW_String("bat01")));
			rval.addElement(instCop);
			return rval;
		}

		/////////////////////////////////////////////////////////////////////////
		virtual OW_CIMInstanceEnumeration enumInstances(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			const OW_Bool& deep,
			const OW_CIMClass& cimClass,
			const OW_Bool& localOnly )
		{
			(void)cop;
			(void)env;
			(void)localOnly;
			(void)deep;
			OW_CIMInstanceEnumeration rval;
			OW_CIMInstance inst = this->createLaptopBatInst(cimClass);
			rval.addElement(inst);
			return rval;
		}

		/////////////////////////////////////////////////////////////////////////
		virtual OW_CIMInstance getInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			const OW_CIMClass& cimClass,
			const OW_Bool& localOnly )
		{
			(void)cop;
			(void)env;
			(void)localOnly;
			OW_CIMInstance rval = this->createLaptopBatInst(cimClass);
			return rval;
		}

		/////////////////////////////////////////////////////////////////////////
		virtual OW_CIMObjectPath createInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			const OW_CIMInstance& cimInstance )
		{

			(void)env;
			(void)cop;
			(void)cimInstance;
			OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
		}

		/////////////////////////////////////////////////////////////////////////
		virtual void modifyInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			const OW_CIMInstance& cimInstance)
		{

			(void)env;
			(void)cop;
			(void)cimInstance;
			OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
		}

		/////////////////////////////////////////////////////////////////////////
		virtual void deleteInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop)
		{
			(void)env;
			(void)cop;
			OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
		}

		/**
		 * Fill in the params for a laptop battery instance
		 *
		 * @param cc a LaptopBattery CIMClass
		 *
		 * @return The laptop batter cim instance
		 */
		OW_CIMInstance createLaptopBatInst(const OW_CIMClass& cc);
	};





//////////////////////////////////////////////////////////////////////////////
	OW_CIMInstance
		OW_LaptopBattery::createLaptopBatInst(const OW_CIMClass& cc)
	{
		OW_CIMInstance rval = cc.newInstance();
		rval.setProperty("SystemCreationClassName",
			OW_CIMValue(OW_String("CIM_System")));
		char hostbuf[256];
		gethostname(hostbuf, 256);
		OW_String hostname(hostbuf);
		rval.setProperty("SystemName", OW_CIMValue(hostname));
		rval.setProperty("CreationClassName", OW_CIMValue(cc.getName()));
		rval.setProperty("DeviceID", OW_CIMValue(OW_String("bat01")));

		// /proc/apm typically looks like:
		// 1.16 1.2 0x03 0x00 0x00 0x01 72% 183 min
		// or
		// 1.16 1.2 0x03 0x01 0x03 0x09 93% -1 ?
		ifstream infile("/proc/apm", std::ios::in);
		OW_StringStream oss;
		oss << infile.rdbuf();
		infile.close();
		OW_String fileContents = oss.toString();
		OW_StringArray toks = fileContents.tokenize();
		OW_Int32 minutes;
		OW_UInt16 percent;
		try
		{
			minutes = toks[7].toInt32();
			toks[6] = toks[6].substring(0, 2); // erase the last %
			percent = toks[6].toUInt16();
		}
		catch (const OW_StringConversionException& e)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, "Failed parsing /proc/apm");
		}
		OW_UInt16 status = STAT_Unknown;
		OW_Bool charging = false;
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
		rval.setProperty("EstimatedChargeRemaining", OW_CIMValue(percent));
		rval.setProperty("EstimatedRunTime", OW_CIMValue((OW_UInt32)minutes));
		rval.setProperty("BatteryStatus", OW_CIMValue(status));
		rval.setProperty("Charging", OW_CIMValue(charging));
		return rval;
	}

}


OW_PROVIDERFACTORY(OW_LaptopBattery, laptopbat)

	

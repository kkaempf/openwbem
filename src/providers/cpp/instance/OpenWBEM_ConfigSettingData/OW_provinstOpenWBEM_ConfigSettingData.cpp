/****************************************************************************
* Copyright (C) 2005 Novell, Inc. All rights reserved.
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
****************************************************************************/

/**
 * @author Bart Whiteley
 */

#include "OW_config.h"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppSimpleAssociatorProviderIFC.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_ConfigException.hpp"
#include "OW_Format.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_FileSystem.hpp"
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace OpenWBEM
{

class OpenWBEM_ConfigSettingDataInstProv : public virtual CppInstanceProviderIFC,
	public virtual CppSimpleAssociatorProviderIFC
{
public:

	void initialize(const ProviderEnvironmentIFCRef& env)
	{
		loadConfigMap(env); 
	}

	////////////////////////////////////////////////////////////////////////////
	virtual ~OpenWBEM_ConfigSettingDataInstProv()
	{
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
	{
		(void)cimClass;
		env->getLogger()->logDebug("In OpenWBEM_ConfigSettingDataInstProv::enumInstanceNames");

		if (mapNeedsLoad(getConfigFile(env)))
		{
			loadConfigMap(env); 
		}

		CIMObjectPath newCop(className, ns);


		for (ConfigItemsMap::const_iterator iter = m_configItems.begin(); 
			  iter != m_configItems.end(); ++iter)
		{
			String id = makeID(iter->first); 
			newCop.setKeyValue("InstanceID", CIMValue(id)); 
			result.handle(newCop);
		}

	}


	////////////////////////////////////////////////////////////////////////////
	virtual CIMInstance getInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& cimClass )
	{
		(void)ns;
		env->getLogger()->logDebug("In OpenWBEM_ConfigSettingDataInstProv::getInstance");

		if (mapNeedsLoad(getConfigFile(env)))
		{
			loadConfigMap(env); 
		}

		CIMInstance newInst = cimClass.newInstance();
		newInst.updatePropertyValues(instanceName.getKeys());

		String id; 
		CIMValue cv = instanceName.getKeyValue("InstanceID"); 
		if (!cv || cv.getType() != CIMDataType::STRING || cv.isArray())
		{
			OW_THROWCIM(CIMException::INVALID_PARAMETER); 
		}
		cv.get(id); 

		StringArray sa = id.tokenize(":"); 
		if (sa.size() != 3)
		{
			OW_THROWCIM(CIMException::INVALID_PARAMETER); 
		}

		String& configName = sa[2]; 

		ConfigItemsMap::const_iterator iter = m_configItems.find(configName); 
		if (iter == m_configItems.end())
		{
			OW_THROWCIM(CIMException::NOT_FOUND); 
		}

		const ConfigData& cd = iter->second; 

		newInst.setProperty("Value", CIMValue(cd.Value)); 
		newInst.setProperty("CurrentEffectiveValue", CIMValue(cd.CurrentEffectiveValue));
		// This property is Required
		newInst.setProperty("ElementName", CIMValue(configName)); 
		newInst.setProperty("Caption", CIMValue(cd.Caption)); 
		newInst.setProperty("Description", CIMValue(cd.Description)); 

		return newInst.clone(localOnly,includeQualifiers,includeClassOrigin,propertyList);
	}

	////////////////////////////////////////////////////////////////////////////
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

	////////////////////////////////////////////////////////////////////////////
	virtual void modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass)
	{
		(void)ns;
		(void)includeQualifiers;
		(void)theClass;
		env->getLogger()->logDebug("In OpenWBEM_ConfigSettingDataInstProv::modifyInstance");

		if (propertyList)
		{
			if (propertyList->size() == 0)
			{
				return; 
			}
			if (propertyList->size() > 1 
				|| !(*propertyList)[0].equalsIgnoreCase("value"))
			{
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							   "Only the \"Value\" parameter can be modified"); 
			}
		}

		String oldvalue; 
		CIMValue cv = previousInstance.getPropertyValue("Value"); 
		if (!cv || cv.getType() != CIMDataType::STRING || cv.isArray())
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
						   "Bad or Missing \"Value\" property"); 
		}
		cv.get(oldvalue); 

		String newvalue; 

		cv = modifiedInstance.getPropertyValue("Value"); 
		if (!cv || cv.getType() != CIMDataType::STRING || cv.isArray())
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
						   "Bad or Missing \"Value\" property"); 
		}
		cv.get(newvalue); 

		if (oldvalue == newvalue)
		{
			// nothing to do 
			return; 
		}

		String configfile = getConfigFile(env); 

		std::ifstream file(configfile.c_str());
		if (!file)
		{
			OW_THROWCIMMSG(CIMException::FAILED, Format("Unable to read config"
						" file: %1", configfile).c_str());
		}

		cv = modifiedInstance.getPropertyValue("InstanceID"); 
		if (!cv || cv.getType() != CIMDataType::STRING || cv.isArray())
		{
			OW_THROWCIM(CIMException::INVALID_PARAMETER); 
		}
		String id; 
		cv.get(id); 

		StringArray sa = id.tokenize(":"); 
		if (sa.size() != 3)
		{
			OW_THROWCIM(CIMException::INVALID_PARAMETER); 
		}

		String configName = sa[2]; 


		TempFileStream tfs; 

		String line;
		bool foundItem = false; 
		while (file)
		{
			line = String::getLine(file);
			String tline = line; 
			if (!tline.empty() && tline[0] == ';')
			{
				do
				{
					tline = tline.substring(1); 
				} while (!tline.empty() && tline[0] == ';');
			}
			size_t idx = tline.indexOf('=');
			if (idx != String::npos)
			{
				String item = tline.substring(0, idx).trim();
				if (item == configName)
				{
					if (foundItem)
					{
						// We've already written this config item.  
						// omit remaining lines that resemble it. 
						continue; 
					}
					line = configName + " = " + newvalue; 
					foundItem = true; 
				}
			}
			tfs << line << "\n"; 
		}
		if (!foundItem)
		{
			tfs << "\n" << configName << " = " << newvalue << "\n"; 
		}
		file.close(); 
		tfs.rewind(); 

		// we can't use FileSystem::rename() because it only works if 
		// src and dest are on the same device. 
		String bkpfilename = configfile + ".bkp"; 
		std::ofstream bkpfile(bkpfilename.c_str()); 
		if (!bkpfile)
		{
			OW_THROWCIMMSG(CIMException::FAILED, Format("Error opening %1 for writing", 
														bkpfilename).c_str()); 
		}
		std::ifstream ifile(configfile.c_str()); 
		if (!ifile)
		{
			OW_THROWCIMMSG(CIMException::FAILED, Format("Error opening %1 for reading", 
														ifile).c_str()); 
		}
		bkpfile << ifile.rdbuf(); 
		bkpfile.close(); 
		ifile.close(); 

		std::ofstream ofile(configfile.c_str()); 
		if (!ofile)
		{
			OW_THROWCIMMSG(CIMException::FAILED, Format("Error opening %1 for writing", 
														ofile).c_str()); 
		}
		ofile << tfs.rdbuf(); 
		ofile.close(); 
		// no need to reload configMap here.  It will automatically be done 
		// on the next read operation (if there is one). 
	}

	////////////////////////////////////////////////////////////////////////////
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

	////////////////////////////////////////////////////////////////////////////
	virtual void getInstanceProviderInfo(InstanceProviderInfo &info)
	{
		info.addInstrumentedClass("OpenWBEM_ConfigSettingData");
	}
/**
 * This method is invoked in order to do the Associators, AssociatorNames, 
 * References, and ReferenceNames operation as
 * defined by the CIM operations over HTTP spec at
 * http://www.dmtf.org/download/spec/xmls/CIM_HTTP_Mapping10.htm#SecAssociators
 * This operation is used to enumerate CIM Instances of a particular 
 * Association class
 *
 * @param assocClass Defines the association that the objectName
 * 	object should be associated to. The provider uses this
 * 	information to identify which association must be
 * 	traversed in case it supports more than one
 * 	association.
 *
 * @param objectName Defines the source CIM Object whose associated
 * 	Objects are to be returned.  This is an instance name.
 * 	Instance name (modelpath).
 *
 * @param resultClass The resultClass input parameter, if not empty (""),
 * 	MUST be a valid CIM Class name.  It acts as a filter on the
 * 	returned set of Objects by mandating that the resultRole of each 
 *      returned Object MUST be an ObjectPath to either an Instance of this 
 *      Class (or one of its subclasses) or be this Class (or one of its 
 *      subclasses).
 *
 * @param role The role input parameter, if not empty (""), MUST be a
 * 	valid Property name.  It acts as a filter on the returned set of
 * 	Objects by mandating that each returned Object MUST be associated
 * 	to the source Object via an Association in which the source Object
 * 	plays the specified role (i.e. the name of the Property in the
 * 	Association Class that refers to the source Object MUST match the
 * 	value of this parameter).
 *
 * @param resultRole The resultRole input parameter if not empty (""),
 * 	MUST be a valid Property name.  It acts as a filter on the returned
 * 	set of Objects by mandating that each returned Object MUST contain
 *      a reference (ObjectPath) to an Instance or Class which is 
 * 	associated to the source Object via an Association in which the
 * 	Associated Object plays the specified role (i.e. the name of the
 * 	Property in the Association Class that refers to the Associated
 * 	Object MUST match the value of this parameter).
 *
 * @returns If successful, the method returns zero or more CIM Instances
 * 	meeting the requested criteria.
 *
 * @throws CIMException. - The following IDs can be expected.
 * 	CIM_ERR_ACCESS_DENIED
 * 	CIM_ERR_NOT_SUPPORTED
 * 	CIM_ERR_INVALID_NAMESPACE
 * 	CIM_ERR_INVALID_PARAMETER (including missing, duplicate,
 * 	unrecognized or otherwise incorrect parameters)
 * 	CIM_ERR_FAILED (some other unspecifed error occurred)
 * 
 * 
 * 
 *  +------------------+                               +-----------------+
 *  | ObjectNameClass  |                               |  ResultClass    |
 *  | ~~~~~~~~~~~~~~~  |                               |  ~~~~~~~~~~~    |
 *  +------------------+                               +-----------------+
 *     |                +----------------------------+                 |
 *     |                |  [Association] AssocClass  |                 |
 *     |   ObjectName   |  ~~~~~~~~~~~~~~~~~~~~~~~~  |                 |
 *     +----------------+ ObjectNameClass REF Role   |                 |
 *      (CIMObjectPath) | ResultClass REF ResultRole +-----------------+
 *                      |                            |  (CIMObjectPath)
 *                      +----------------------------+
 */
        virtual void doReferences(const ProviderEnvironmentIFCRef &env, 
                                  CIMInstanceResultHandlerIFC &result, 
                                  const String &ns, 
                                  const CIMObjectPath &objectName, 
                                  const CIMClass &assocClass, 
                                  const String &resultClass, 
                                  const String &role, 
                                  const String &resultRole) 
		{
			CIMInstance newInst = assocClass.newInstance(); 
			String lcname = objectName.getClassName(); 
			lcname.toLowerCase(); 
			String lrole = role; 
			lrole.toLowerCase(); 
			String lresultRole = resultRole; 
			lresultRole.toLowerCase(); 
			if (lcname == "openwbem_objectmanager")
			{
				if ((!lrole.empty() && lrole != "managedelement")
					|| (!lresultRole.empty() && lresultRole != "settingdata"))
				{
					OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, 
								   "Bad Role or ResultRole"); 
				}
				lrole = "ManagedElement"; 
				lresultRole = "SettingData"; 

				newInst.setProperty(lrole, CIMValue(objectName)); 
				CIMObjectPath settingCOP("OpenWBEM_ConfigSettingData", ns); 

				for (ConfigItemsMap::const_iterator iter = m_configItems.begin(); 
					  iter != m_configItems.end(); ++iter)
				{
					settingCOP.setKeyValue("InstanceID", 
										   CIMValue(makeID(iter->first))); 
					newInst.setProperty(lresultRole, CIMValue(settingCOP)); 
					result.handle(newInst); 
				}
			}
			else if (lcname == "openwbem_configsettingdata")
			{
				if ((!lrole.empty() && lrole != "settingdata")
					|| (!lresultRole.empty() && lresultRole != "managedelement"))
				{
					OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, 
								   "Bad Role or ResultRole"); 
				}
				lrole = "SettingData"; 
				lresultRole = "ManagedElement"; 

				CIMObjectPathArray cpa = env->getCIMOMHandle()->enumInstanceNamesA(ns, "OpenWBEM_ObjectManager"); 
				if (cpa.size() != 1)
				{
					OW_THROWCIMMSG(CIMException::FAILED, 
								   "Unable to retrieve a single instance of OpenWBEM_ObjectManager"); 
				}
				newInst.setProperty(lrole, CIMValue(objectName)); 
				newInst.setProperty(lresultRole, CIMValue(cpa[0])); 
				result.handle(newInst); 
			}
		}

///////////////////////////////////////////////////////////////////////////////
		virtual void getAssociatorProviderInfo(AssociatorProviderInfo &info)
		{
			info.addInstrumentedClass("OpenWBEM_ObjectManagerConfigSettingData");
		}

private: 
	struct ConfigData
	{
		String Value; 
		String CurrentEffectiveValue; 
		String Caption; 
		String Description; 
		//String InstanceID; 
		//String ElementName; 
	};

	typedef SortedVectorMap<String, ConfigData> ConfigItemsMap; 

	SortedVectorMap<String, ConfigData> m_configItems; 
	////////////////////////////////////////////////////////////////////////////

	time_t m_fileMTime; 
	////////////////////////////////////////////////////////////////////////////
	String makeID(const String& confName)
	{
		String rval = "OpenWBEM:ConfigItem:"; 
		rval += confName; 
		return rval; 
	}
	////////////////////////////////////////////////////////////////////////////
	bool mapNeedsLoad(const String& filename)
	{
		struct stat st; 

        if (stat(filename.c_str(), &st) != 0)
        {
			OW_THROWCIMMSG(CIMException::FAILED, Format("Cannot stat %1: %2",
						   filename, strerror(errno)).c_str()); 
        }
		
		if (st.st_mtime != m_fileMTime)
		{
			return true;
		}
		return false; 
	}

	////////////////////////////////////////////////////////////////////////////
	String getConfigFile(const ProviderEnvironmentIFCRef& env)
	{
		String filename = env->getConfigItem("owcimomd.config_file"); 

		if (filename.empty())
		{
			OW_THROWCIMMSG(CIMException::FAILED, "Cannot determine config file"); 
		}

		return filename; 
	}

	////////////////////////////////////////////////////////////////////////////
	void loadConfigMap(const ProviderEnvironmentIFCRef& env)
	{
		String filename = getConfigFile(env); 
		m_configItems.clear(); 

		struct stat st; 
        if (stat(filename.c_str(), &st) != 0)
        {
			OW_THROWCIMMSG(CIMException::FAILED, Format("Cannot stat %1: %2",
						   filename, strerror(errno)).c_str()); 
        }
		
		m_fileMTime = st.st_mtime; 

		std::ifstream file(filename.c_str());
		if (!file)
		{
			OW_THROWCIMMSG(CIMException::FAILED, Format("Unable to read config"
						" file: %1", filename).c_str());
		}

		String line;
		int lineNum = 0;
		while (file)
		{
			lineNum++;
			line = String::getLine(file);
			if (!line.empty())
			{
				// If comment line, ignore
				if (line[0] == '#' || line[0] == ';')
				{
					continue;
				}
				size_t idx = line.indexOf('=');
				if (idx != String::npos)
				{
					if (idx + 1 < line.length())
					{
						String itemValue = line.substring(idx + 1).trim();
						if (!itemValue.empty())
						{
							String item = line.substring(0, idx).trim();
							ConfigItemsMap::iterator it = m_configItems.find(item);
							if (it == m_configItems.end())
							{
								ConfigData cd; 
								cd.Value = itemValue; 
								cd.CurrentEffectiveValue = env->getConfigItem(item); 
								m_configItems.insert(std::make_pair(item, cd));
							}
						}
					}
				}
				else
				{
					OW_THROWCIMMSG(CIMException::FAILED, Format("Error in config file:"
						" %1 at line %2.\n  Line is %3", filename, lineNum,
						line).c_str());
				}
			}
		}
		file.close(); 

		for(ConfigOpts::NameAndDefault* niter = &ConfigOpts::g_defaults[0]; 
			 niter != ConfigOpts::g_defaultsEnd; ++niter)
		{
			String curValue = env->getConfigItem(niter->name, String(niter->defaultValue));
			ConfigItemsMap::iterator it = m_configItems.find(niter->name); 
			if (it == m_configItems.end())
			{
				ConfigData cd; 
				cd.CurrentEffectiveValue = curValue; 
				m_configItems.insert(std::make_pair(niter->name, cd));
			}
			else
			{
				it->second.CurrentEffectiveValue = curValue; 
			}
		}

	}


};


}




OW_PROVIDERFACTORY(OpenWBEM::OpenWBEM_ConfigSettingDataInstProv, owprovinstOpenWBEM_ConfigSettingData)



	


/*******************************************************************************
* Copyright (C) 2001-3 Caldera International, Inc All rights reserved.
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

#ifndef testSharedLibraryLoader_HPP_
#define testSharedLibraryLoader_HPP_

#include "OW_SharedLibrary.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_Reference.hpp"
#include "OW_ProviderIFCBaseIFC.hpp"
#include "OW_ProviderIFCLoader.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "UnitTestEnvironment.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMValue.hpp"

static int testFunction( int i )
{
	return i + 1;
}

static const char* versionFunction()
{
	return OW_VERSION;
}

typedef int (*fptype)(int);

class testSharedLibrary: public OW_SharedLibrary
{
	public:
		virtual ~testSharedLibrary(){}

	protected:
		virtual OW_Bool doGetFunctionPointer( const OW_String&, void** fp ) const
		{
			*fp = (void*)&testFunction;
			return true;
		}
};

class TestInstanceProvider : public OW_InstanceProviderIFC
{
public:
	virtual void enumInstances(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_String &, OW_CIMInstanceResultHandlerIFC &, 
		OW_Bool , 
		OW_Bool , 
		OW_Bool , 
		OW_Bool ,
		const OW_StringArray*,
		const OW_CIMClass& ,
		const OW_CIMClass &)
	{
	}
	virtual void enumInstanceNames(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_String &, OW_CIMObjectPathResultHandlerIFC &, const OW_CIMClass &)
	{
	}
	virtual OW_CIMInstance getInstance(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMObjectPath &, 
		OW_Bool ,
		OW_Bool , 
		OW_Bool ,
		const OW_StringArray* , 
		const OW_CIMClass &)
	{
		return OW_CIMInstance();
	}
	virtual OW_CIMObjectPath createInstance(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMInstance &)
	{
		return OW_CIMObjectPath();
	}
	virtual void modifyInstance(const OW_ProviderEnvironmentIFCRef &, const OW_String &, 
			const OW_CIMInstance& ,
			const OW_CIMInstance& ,
			OW_Bool ,
			const OW_StringArray* ,
			const OW_CIMClass& )
	{
	}
	virtual void deleteInstance(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMObjectPath &)
	{
	}
};

class TestMethodProvider : public OW_MethodProviderIFC
{
public:
	virtual OW_CIMValue invokeMethod(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMObjectPath &, const OW_String &, const OW_CIMParamValueArray &, OW_CIMParamValueArray &)
	{
		return OW_CIMValue();
	}
};

class TestAssociatorProvider : public OW_AssociatorProviderIFC
{
public:
	virtual void associatorNames(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMObjectPath &, const OW_CIMObjectPath &, OW_CIMObjectPathResultHandlerIFC &, const OW_String &, const OW_String &, const OW_String &)
	{
	}
	virtual void references(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMObjectPath &, const OW_CIMObjectPath &, OW_CIMInstanceResultHandlerIFC &, const OW_String &, const OW_Bool &, const OW_Bool &, const OW_StringArray *)
	{
	}
	virtual void referenceNames(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMObjectPath &, const OW_CIMObjectPath &, OW_CIMObjectPathResultHandlerIFC &, const OW_String &)
	{
	}
	virtual void associators(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMObjectPath &, const OW_CIMObjectPath &, OW_CIMInstanceResultHandlerIFC &, const OW_String &, const OW_String &, const OW_String &, const OW_Bool &, const OW_Bool &, const OW_StringArray *)
	{
	}
};

class TestPropertyProvider : public OW_PropertyProviderIFC
{
public:
	virtual OW_CIMValue getPropertyValue(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMObjectPath &, const OW_String &, const OW_String &)
	{
		return OW_CIMValue();
	}
	virtual void setPropertyValue(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMObjectPath &, const OW_String &, const OW_String &, const OW_CIMValue &)
	{
	}
};

class TestIndicationProvider : public OW_IndicationProviderIFC
{
public:
	virtual void deActivateFilter(const OW_ProviderEnvironmentIFCRef &, const OW_WQLSelectStatement &, const OW_String &, const OW_String&, const OW_StringArray&, bool ) 
	{
	}
	virtual void activateFilter(const OW_ProviderEnvironmentIFCRef &, const OW_WQLSelectStatement &, const OW_String &, const OW_String&, const OW_StringArray&, bool ) 
	{
	}
	virtual void authorizeFilter(const OW_ProviderEnvironmentIFCRef &, const OW_WQLSelectStatement &, const OW_String &, const OW_String&, const OW_StringArray&, const OW_String &) 
	{
	}
	virtual int mustPoll(const OW_ProviderEnvironmentIFCRef &, const OW_WQLSelectStatement &, const OW_String &, const OW_String&, const OW_StringArray&)
	{
		return 0;
	}
};

class testProviderMux: public OW_ProviderIFCBaseIFC
{
	public:
		testProviderMux( const char* const name )
			: OW_ProviderIFCBaseIFC(), m_name(name) {}
		virtual ~testProviderMux() {}
		virtual void doInit(const OW_ProviderEnvironmentIFCRef&,
			OW_InstanceProviderInfoArray& ia,
			OW_AssociatorProviderInfoArray& aa,
			OW_MethodProviderInfoArray& ma,
			OW_PropertyProviderInfoArray& pa,
			OW_IndicationProviderInfoArray& inda)
		{
			if (m_name == "lib1")
			{
				// instance provider registration
				{
					OW_InstanceProviderInfo ipi;
					ipi.setProviderName("TestInstanceProvider");
					ipi.addInstrumentedClass("SelfReg");
					ia.push_back(ipi);
				}
				{
					OW_InstanceProviderInfo ipi;
					ipi.setProviderName("TestInstanceProvider");
					OW_StringArray namespaces;
					namespaces.push_back("root");
					namespaces.push_back("root/good");
					OW_InstanceProviderInfo::ClassInfo ci("SelfRegTwoNamespaces", namespaces);
					ipi.addInstrumentedClass(ci);
					ia.push_back(ipi);
				}
				// method provider registration
				{
					OW_MethodProviderInfo mpi;
					mpi.setProviderName("TestMethodProvider");
					mpi.addInstrumentedClass("SelfReg");
					ma.push_back(mpi);
				}
				{
					OW_MethodProviderInfo mpi;
					mpi.setProviderName("TestMethodProvider");
					OW_StringArray namespaces;
					namespaces.push_back("root");
					namespaces.push_back("root/good");
					OW_StringArray methods;
					OW_MethodProviderInfo::ClassInfo ci("SelfRegTwoNamespaces", namespaces, methods);
					mpi.addInstrumentedClass(ci);
					ma.push_back(mpi);
				}
				{
					OW_MethodProviderInfo mpi;
					mpi.setProviderName("TestMethodProvider");
					OW_StringArray namespaces;
					OW_StringArray methods;
					methods.push_back("TestMethod");
					OW_MethodProviderInfo::ClassInfo ci("SelfRegOneMethod", namespaces, methods);
					mpi.addInstrumentedClass(ci);
					ma.push_back(mpi);
				}
				{
					OW_MethodProviderInfo mpi;
					mpi.setProviderName("TestMethodProvider");
					OW_StringArray namespaces;
					namespaces.push_back("root");
					OW_StringArray methods;
					methods.push_back("TestMethod");
					OW_MethodProviderInfo::ClassInfo ci("SelfRegOneNamespaceOneMethod", namespaces, methods);
					mpi.addInstrumentedClass(ci);
					ma.push_back(mpi);
				}
				// associator provider registration
				{
					OW_AssociatorProviderInfo api;
					api.setProviderName("TestAssociatorProvider");
					api.addInstrumentedClass("SelfReg");
					aa.push_back(api);
				}
				{
					OW_AssociatorProviderInfo api;
					api.setProviderName("TestAssociatorProvider");
					OW_StringArray namespaces;
					namespaces.push_back("root");
					namespaces.push_back("root/good");
					OW_AssociatorProviderInfo::ClassInfo ci("SelfRegTwoNamespaces", namespaces);
					api.addInstrumentedClass(ci);
					aa.push_back(api);
				}
				// property provider registration
				{
					OW_PropertyProviderInfo ppi;
					ppi.setProviderName("TestPropertyProvider");
					OW_StringArray namespaces;
					OW_StringArray properties;
					properties.push_back("TestProperty");
					OW_PropertyProviderInfo::ClassInfo ci("SelfReg", namespaces, properties);
					ppi.addInstrumentedClass(ci);
					pa.push_back(ppi);
				}
				{
					OW_PropertyProviderInfo ppi;
					ppi.setProviderName("TestPropertyProvider");
					OW_StringArray namespaces;
					namespaces.push_back("root");
					namespaces.push_back("root/good");
					OW_StringArray properties;
					properties.push_back("TestProperty");
					OW_PropertyProviderInfo::ClassInfo ci("SelfRegTwoNamespaces", namespaces, properties);
					ppi.addInstrumentedClass(ci);
					pa.push_back(ppi);
				}
				{
					OW_PropertyProviderInfo ppi;
					ppi.setProviderName("TestPropertyProvider");
					OW_StringArray namespaces;
					OW_StringArray properties;
					properties.push_back("TestProperty");
					OW_PropertyProviderInfo::ClassInfo ci("SelfRegOneProperty", namespaces, properties);
					ppi.addInstrumentedClass(ci);
					pa.push_back(ppi);
				}
				{
					OW_PropertyProviderInfo ppi;
					ppi.setProviderName("TestPropertyProvider");
					OW_StringArray namespaces;
					namespaces.push_back("root");
					OW_StringArray properties;
					properties.push_back("TestProperty");
					OW_PropertyProviderInfo::ClassInfo ci("SelfRegOneNamespaceOneProperty", namespaces, properties);
					ppi.addInstrumentedClass(ci);
					pa.push_back(ppi);
				}
				// indication provider registration
				{
					OW_IndicationProviderInfo indi;
					indi.setProviderName("TestIndicationProvider");
					indi.addInstrumentedClass("SelfReg");
					inda.push_back(indi);
				}
				{
					OW_IndicationProviderInfo indi;
					indi.setProviderName("TestIndicationProvider");
					OW_StringArray namespaces;
					namespaces.push_back("root");
					namespaces.push_back("root/good");
					OW_IndicationProviderInfo::ClassInfo ci("SelfRegTwoNamespaces", namespaces);
					indi.addInstrumentedClass(ci);
					inda.push_back(indi);
				}
				{
					OW_IndicationProviderInfo indi;
					indi.setProviderName("TestIndicationProvider1");
					indi.addInstrumentedClass("SelfReg2");
					inda.push_back(indi);
				}
				{
					OW_IndicationProviderInfo indi;
					indi.setProviderName("TestIndicationProvider2");
					indi.addInstrumentedClass("SelfReg2");
					inda.push_back(indi);
				}
				{
					// life-cycle type indication
					OW_IndicationProviderInfo indi;
					indi.setProviderName("TestIndicationProvider");
					OW_StringArray namespaces;
					OW_StringArray classes;
					classes.push_back("TestClass1");
					OW_IndicationProviderInfo::ClassInfo ci1("CIM_InstCreation", namespaces, classes);
					indi.addInstrumentedClass(ci1);
					OW_IndicationProviderInfo::ClassInfo ci2("CIM_InstModification", namespaces, classes);
					indi.addInstrumentedClass(ci2);
					OW_IndicationProviderInfo::ClassInfo ci3("CIM_InstDeletion", namespaces, classes);
					indi.addInstrumentedClass(ci3);
					inda.push_back(indi);
				}
				{
					// life-cycle type indication
					OW_IndicationProviderInfo indi;
					indi.setProviderName("TestIndicationProvider2");
					OW_StringArray namespaces;
					OW_StringArray classes;
					classes.push_back("TestClass2");
					classes.push_back("TestClass3");
					OW_IndicationProviderInfo::ClassInfo ci1("CIM_InstCreation", namespaces, classes);
					indi.addInstrumentedClass(ci1);
					OW_IndicationProviderInfo::ClassInfo ci2("CIM_InstModification", namespaces, classes);
					indi.addInstrumentedClass(ci2);
					OW_IndicationProviderInfo::ClassInfo ci3("CIM_InstDeletion", namespaces, classes);
					indi.addInstrumentedClass(ci3);
					inda.push_back(indi);
				}

			}
		}

		virtual OW_InstanceProviderIFCRef doGetInstanceProvider(
			const OW_ProviderEnvironmentIFCRef&, const char* provIdString)
		{
			if (OW_String(provIdString) == "TestInstanceProvider")
			{
				return OW_InstanceProviderIFCRef(new TestInstanceProvider);
			}
			OW_THROW(OW_NoSuchProviderException, provIdString);
		}

		virtual OW_MethodProviderIFCRef doGetMethodProvider(
			const OW_ProviderEnvironmentIFCRef&, const char* provIdString)
		{
			if (OW_String(provIdString) == "TestMethodProvider")
			{
				return OW_MethodProviderIFCRef(new TestMethodProvider);
			}
			OW_THROW(OW_NoSuchProviderException, provIdString);
		}

		virtual OW_PropertyProviderIFCRef doGetPropertyProvider(
			const OW_ProviderEnvironmentIFCRef&, const char* provIdString)
		{
			if (OW_String(provIdString) == "TestPropertyProvider")
			{
				return OW_PropertyProviderIFCRef(new TestPropertyProvider);
			}
			OW_THROW(OW_NoSuchProviderException, provIdString);
		}

		virtual OW_AssociatorProviderIFCRef doGetAssociatorProvider(
			const OW_ProviderEnvironmentIFCRef&, const char* provIdString)
		{
			if (OW_String(provIdString) == "TestAssociatorProvider")
			{
				return OW_AssociatorProviderIFCRef(new TestAssociatorProvider);
			}
			OW_THROW(OW_NoSuchProviderException, provIdString);
		}

		virtual OW_IndicationProviderIFCRef doGetIndicationProvider(
			const OW_ProviderEnvironmentIFCRef&, const char* provIdString)
		{
			if (OW_String(provIdString) == "TestIndicationProvider"
				|| OW_String(provIdString) == "TestIndicationProvider1"
				|| OW_String(provIdString) == "TestIndicationProvider2")
			{
				return OW_IndicationProviderIFCRef(new TestIndicationProvider);
			}
			OW_THROW(OW_NoSuchProviderException, provIdString);
		}

		virtual const char* getName() const { return m_name.c_str(); }

		virtual OW_PolledProviderIFCRefArray doGetPolledProviders(
			const OW_ProviderEnvironmentIFCRef&)
		{
			return OW_PolledProviderIFCRefArray();
		}

		virtual OW_IndicationExportProviderIFCRefArray doGetIndicationExportProviders(
			const OW_ProviderEnvironmentIFCRef&)
		{
			return OW_IndicationExportProviderIFCRefArray();
		}

		virtual void doUnloadProviders(const OW_ProviderEnvironmentIFCRef&)
		{
		}

private:
	OW_String m_name;

};

static OW_ProviderIFCBaseIFC* testCreateProviderMux()
{
	return new testProviderMux( "lib1" );
}

static OW_ProviderIFCBaseIFC* testCreateProviderMux2()
{
	return new testProviderMux( "lib2" );
}

static OW_ProviderIFCBaseIFC* testCreateProviderMux3()
{
	return new testProviderMux( "lib3" );
}

class testMuxSharedLibrary: public OW_SharedLibrary
{
	public:
		virtual ~testMuxSharedLibrary(){}

	protected:
		virtual OW_Bool doGetFunctionPointer( const OW_String& name, void**fp ) const
		{
			if (name == "getOWVersion")
				*fp = (void*)&versionFunction;
			else
				*fp = (void*)&testCreateProviderMux;
			return true;
		}
};
			
class testMuxSharedLibrary2: public OW_SharedLibrary
{
	public:
		virtual ~testMuxSharedLibrary2(){}

	protected:
		virtual OW_Bool doGetFunctionPointer( const OW_String& name, void**fp ) const
		{
			if (name == "getOWVersion")
				*fp = (void*)&versionFunction;
			else
				*fp = (void*)&testCreateProviderMux2;
			return true;
		}
};
			
class testMuxSharedLibrary3: public OW_SharedLibrary
{
	public:
		virtual ~testMuxSharedLibrary3(){}

	protected:
		virtual OW_Bool doGetFunctionPointer( const OW_String& name, void**fp ) const
		{
			if (name == "getOWVersion")
				*fp = (void*)&versionFunction;
			else
				*fp = (void*)&testCreateProviderMux3;
			return true;
		}
};
			
class testSharedLibraryLoader: public OW_SharedLibraryLoader
{
	public:
		virtual ~testSharedLibraryLoader(){}

		virtual OW_SharedLibraryRef loadSharedLibrary( const OW_String& name,
            OW_LoggerRef ) const
		{
			if ( name == "lib1" )
				return OW_SharedLibraryRef( new testMuxSharedLibrary );
			else if ( name == "lib2" )
				return OW_SharedLibraryRef( new testMuxSharedLibrary2 );
			else if ( name == "lib3" )
				return OW_SharedLibraryRef( new testMuxSharedLibrary3 );
			else
				return OW_SharedLibraryRef( new testSharedLibrary );
		}
};

namespace
{
	class DummyLogger : public OW_Logger
	{
	protected:
		virtual void doLogMessage(const OW_String &message, const OW_LogLevel) const
		{
			std::cout << message << std::endl;
		}
	};

	class testProviderEnvironment : public OW_ProviderEnvironmentIFC
	{
	public:

		testProviderEnvironment(const OW_LocalCIMOMHandle& ch, OW_LoggerRef l)
		: m_ch(new OW_LocalCIMOMHandle(ch)), m_logger(l)
		{}

		testProviderEnvironment()
		: m_ch(new OW_LocalCIMOMHandle()), m_logger(new DummyLogger)
		{}

		virtual OW_CIMOMHandleIFCRef getCIMOMHandle() const
		{
			return m_ch;
		}
		
		virtual OW_CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
		{
			return m_ch;
		}
		
		virtual OW_LoggerRef getLogger() const
		{
			return m_logger;
		}

		virtual OW_String getConfigItem(const OW_String &) const
		{
			return "";
		}


	private:
		OW_CIMOMHandleIFCRef m_ch;
		OW_LoggerRef m_logger;
	};

	OW_ProviderEnvironmentIFCRef createProvEnvRef(const OW_LocalCIMOMHandle& ch)
	{
		OW_LoggerRef log(new DummyLogger);
		log->setLogLevel(DebugLevel);
		return OW_ProviderEnvironmentIFCRef(new testProviderEnvironment(ch,
			log));
	}
}



class testMuxLoader: public OW_ProviderIFCLoaderBase
{
	public:
		testMuxLoader( OW_SharedLibraryLoaderRef sll ) :
			OW_ProviderIFCLoaderBase(sll, g_testEnvironment)
			{}
		virtual ~testMuxLoader(){}
		virtual void loadIFCs(
				OW_Array<OW_ProviderIFCBaseIFCRef>& out) const
		{
			OW_ProviderIFCBaseIFCRef rval;
			rval = createProviderIFCFromLib( "lib1" );
			if ( !rval.isNull()  )
			{
				out.push_back( rval );
			}

			rval = createProviderIFCFromLib( "lib2" );
			if ( !rval.isNull() )
			{
				out.push_back( rval );
			}

			rval = createProviderIFCFromLib( "lib3" );
			if ( !rval.isNull() )
			{
				out.push_back( rval );
			}

		}
};

class testMuxLoaderBad: public OW_ProviderIFCLoaderBase
{
	public:
		testMuxLoaderBad( OW_SharedLibraryLoaderRef sll ) :
			OW_ProviderIFCLoaderBase(sll, g_testEnvironment)
			{}
		virtual ~testMuxLoaderBad(){}
		virtual void loadIFCs(
				OW_Array<OW_ProviderIFCBaseIFCRef>& out) const
		{
			OW_ProviderIFCBaseIFCRef rval;
			rval = createProviderIFCFromLib( "libbad" );
			if ( !rval.isNull() )
			{
				out.push_back( rval );
			}

			rval = createProviderIFCFromLib( "libbad" );
			if ( !rval.isNull() )
			{
				out.push_back( rval );
			}

			rval = createProviderIFCFromLib( "libbad" );
			if ( !rval.isNull() )
			{
				out.push_back( rval );
			}

		}
};

static OW_SharedLibraryLoaderRef testCreateSharedLibraryLoader();

static OW_ProviderIFCLoaderRef testCreateMuxLoader()
{
	return OW_ProviderIFCLoaderRef( new testMuxLoader(
				testCreateSharedLibraryLoader() ) );
}

static OW_SharedLibraryLoaderRef testCreateSharedLibraryLoader()
{
	(void)testCreateMuxLoader; // to prevent silly compiler warnings
	return OW_SharedLibraryLoaderRef( new testSharedLibraryLoader );
}

#endif

/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Dan Nuffer
 */


#ifndef testSharedLibraryLoader_HPP_
#define testSharedLibraryLoader_HPP_

#include "blocxx/SharedLibrary.hpp"
#include "blocxx/SharedLibraryLoader.hpp"
#include "OW_ProviderIFCBaseIFC.hpp"
#include "OW_ProviderIFCLoader.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "UnitTestEnvironment.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMValue.hpp"
#include "OW_NoSuchProviderException.hpp"
#include "OW_CIMClass.hpp"
#include "OW_LocalOperationContext.hpp"
#include "blocxx/CerrLogger.hpp"

using namespace OpenWBEM;

static int testFunction( int i )
{
	//return i + 0xABCDEF01;
	return 0;
}

static const char* versionFunction()
{
	return OW_VERSION;
}

typedef int (*fptype)(int);

class testSharedLibrary: public blocxx::SharedLibrary
{
	public:
		virtual ~testSharedLibrary(){}

	protected:
		virtual bool doGetFunctionPointer( const blocxx::String&, void** fp ) const
		{
			*fp = (void*)(&testFunction);
			return true;
		}
};

class TestInstanceProvider : public InstanceProviderIFC
{
public:
	virtual void enumInstances(
			const ProviderEnvironmentIFCRef& env,
			const blocxx::String& ns,
			const blocxx::String& className,
			CIMInstanceResultHandlerIFC& result,
			WBEMFlags::ELocalOnlyFlag localOnly,
			WBEMFlags::EDeepFlag deep,
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const blocxx::StringArray* propertyList,
			const CIMClass& requestedClass,
			const CIMClass& cimClass )
	{
	}
	virtual void enumInstanceNames(const ProviderEnvironmentIFCRef &, const blocxx::String &, const blocxx::String &, CIMObjectPathResultHandlerIFC &, const CIMClass &)
	{
	}
	virtual CIMInstance getInstance(
		const ProviderEnvironmentIFCRef& env,
		const blocxx::String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList,
		const CIMClass& cimClass )
	{
		return CIMInstance(CIMNULL);
	}
	virtual CIMObjectPath createInstance(const ProviderEnvironmentIFCRef &, const blocxx::String &, const CIMInstance &)
	{
		return CIMObjectPath(CIMNULL);
	}
	virtual void modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const blocxx::String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const blocxx::StringArray* propertyList,
		const CIMClass& theClass)
	{
	}
	virtual void deleteInstance(const ProviderEnvironmentIFCRef &, const blocxx::String &, const CIMObjectPath &)
	{
	}
	virtual void shuttingDown(const ProviderEnvironmentIFCRef &)
	{
	}
};

class TestMethodProvider : public MethodProviderIFC
{
public:
	virtual CIMValue invokeMethod(const ProviderEnvironmentIFCRef &, const blocxx::String &, const CIMObjectPath &, const blocxx::String &, const CIMParamValueArray &, CIMParamValueArray &)
	{
		return CIMValue(CIMNULL);
	}
	virtual void shuttingDown(const ProviderEnvironmentIFCRef &)
	{
	}
};

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
class TestAssociatorProvider : public AssociatorProviderIFC
{
public:
	virtual void associatorNames(
		const ProviderEnvironmentIFCRef &,
		CIMObjectPathResultHandlerIFC &,
		const blocxx::String &,
		const CIMObjectPath &,
		const blocxx::String &,
		const blocxx::String &,
		const blocxx::String &,
		const blocxx::String &)
	{
	}
	virtual void references(
		const ProviderEnvironmentIFCRef& env,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String& ns,
		const CIMObjectPath& objectName,
		const blocxx::String& resultClass,
		const blocxx::String& role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList)
	{
	}
	virtual void referenceNames(
		const ProviderEnvironmentIFCRef &,
		CIMObjectPathResultHandlerIFC &,
		const blocxx::String &,
		const CIMObjectPath &,
		const blocxx::String &,
		const blocxx::String &)
	{
	}
	virtual void associators(
		const ProviderEnvironmentIFCRef& env,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String& ns,
		const CIMObjectPath& objectName,
		const blocxx::String& assocClass,
		const blocxx::String& resultClass,
		const blocxx::String& role,
		const blocxx::String& resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList)
	{
	}
	virtual void shuttingDown(const ProviderEnvironmentIFCRef &)
	{
	}
};
#endif

class TestIndicationProvider : public IndicationProviderIFC
{
public:
	virtual void deActivateFilter(const ProviderEnvironmentIFCRef &, const WQLSelectStatement &, const blocxx::String &, const blocxx::String&, const blocxx::StringArray&, bool)
	{
	}
	virtual void activateFilter(const ProviderEnvironmentIFCRef &, const WQLSelectStatement &, const blocxx::String &, const blocxx::String&, const blocxx::StringArray&, bool)
	{
	}
	virtual void authorizeFilter(const ProviderEnvironmentIFCRef &, const WQLSelectStatement &, const blocxx::String &, const blocxx::String&, const blocxx::StringArray&, const blocxx::String &)
	{
	}
	virtual int mustPoll(const ProviderEnvironmentIFCRef &, const WQLSelectStatement &, const blocxx::String &, const blocxx::String&, const blocxx::StringArray&)
	{
		return 0;
	}
	virtual void shuttingDown(const ProviderEnvironmentIFCRef &)
	{
	}
};

class testProviderMux: public ProviderIFCBaseIFC
{
	public:
		testProviderMux( const char* const name )
			: ProviderIFCBaseIFC(), m_name(name) {}
		virtual ~testProviderMux() {}
		virtual void doInit(const ProviderEnvironmentIFCRef&,
			InstanceProviderInfoArray& ia,
			SecondaryInstanceProviderInfoArray& si,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			AssociatorProviderInfoArray& aa,
#endif
			MethodProviderInfoArray& ma,
			IndicationProviderInfoArray& inda,
			QueryProviderInfoArray& qpia)
		{
			if (m_name == "lib1")
			{
				// instance provider registration
				{
					InstanceProviderInfo ipi;
					ipi.setProviderName("TestInstanceProvider");
					ipi.addInstrumentedClass("SelfReg");
					ia.push_back(ipi);
				}
				{
					InstanceProviderInfo ipi;
					ipi.setProviderName("TestInstanceProvider");
					blocxx::StringArray namespaces;
					namespaces.push_back("root");
					namespaces.push_back("root/good");
					InstanceProviderInfo::ClassInfo ci("SelfRegTwoNamespaces", namespaces);
					ipi.addInstrumentedClass(ci);
					ia.push_back(ipi);
				}
				// method provider registration
				{
					MethodProviderInfo mpi;
					mpi.setProviderName("TestMethodProvider");
					mpi.addInstrumentedClass("SelfReg");
					ma.push_back(mpi);
				}
				{
					MethodProviderInfo mpi;
					mpi.setProviderName("TestMethodProvider");
					blocxx::StringArray namespaces;
					namespaces.push_back("root");
					namespaces.push_back("root/good");
					blocxx::StringArray methods;
					MethodProviderInfo::ClassInfo ci("SelfRegTwoNamespaces", namespaces, methods);
					mpi.addInstrumentedClass(ci);
					ma.push_back(mpi);
				}
				{
					MethodProviderInfo mpi;
					mpi.setProviderName("TestMethodProvider");
					blocxx::StringArray namespaces;
					blocxx::StringArray methods;
					methods.push_back("TestMethod");
					MethodProviderInfo::ClassInfo ci("SelfRegOneMethod", namespaces, methods);
					mpi.addInstrumentedClass(ci);
					ma.push_back(mpi);
				}
				{
					MethodProviderInfo mpi;
					mpi.setProviderName("TestMethodProvider");
					blocxx::StringArray namespaces;
					namespaces.push_back("root");
					blocxx::StringArray methods;
					methods.push_back("TestMethod");
					MethodProviderInfo::ClassInfo ci("SelfRegOneNamespaceOneMethod", namespaces, methods);
					mpi.addInstrumentedClass(ci);
					ma.push_back(mpi);
				}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
				// associator provider registration
				{
					AssociatorProviderInfo api;
					api.setProviderName("TestAssociatorProvider");
					api.addInstrumentedClass("SelfReg");
					aa.push_back(api);
				}
				{
					AssociatorProviderInfo api;
					api.setProviderName("TestAssociatorProvider");
					blocxx::StringArray namespaces;
					namespaces.push_back("root");
					namespaces.push_back("root/good");
					AssociatorProviderInfo::ClassInfo ci("SelfRegTwoNamespaces", namespaces);
					api.addInstrumentedClass(ci);
					aa.push_back(api);
				}
#endif
				// indication provider registration
				{
					IndicationProviderInfo indi;
					indi.setProviderName("TestIndicationProvider");
					indi.addInstrumentedClass("SelfReg");
					inda.push_back(indi);
				}
				{
					IndicationProviderInfo indi;
					indi.setProviderName("TestIndicationProvider");
					blocxx::StringArray namespaces;
					namespaces.push_back("root");
					namespaces.push_back("root/good");
					IndicationProviderInfo::ClassInfo ci("SelfRegTwoNamespaces", namespaces);
					indi.addInstrumentedClass(ci);
					inda.push_back(indi);
				}
				{
					IndicationProviderInfo indi;
					indi.setProviderName("TestIndicationProvider1");
					indi.addInstrumentedClass("SelfReg2");
					inda.push_back(indi);
				}
				{
					IndicationProviderInfo indi;
					indi.setProviderName("TestIndicationProvider2");
					indi.addInstrumentedClass("SelfReg2");
					inda.push_back(indi);
				}
				{
					// life-cycle type indication
					IndicationProviderInfo indi;
					indi.setProviderName("TestIndicationProvider");
					blocxx::StringArray namespaces;
					blocxx::StringArray classes;
					classes.push_back("TestClass1");
					IndicationProviderInfo::ClassInfo ci1("CIM_InstCreation", namespaces, classes);
					indi.addInstrumentedClass(ci1);
					IndicationProviderInfo::ClassInfo ci2("CIM_InstModification", namespaces, classes);
					indi.addInstrumentedClass(ci2);
					IndicationProviderInfo::ClassInfo ci3("CIM_InstDeletion", namespaces, classes);
					indi.addInstrumentedClass(ci3);
					inda.push_back(indi);
				}
				{
					// life-cycle type indication
					IndicationProviderInfo indi;
					indi.setProviderName("TestIndicationProvider2");
					blocxx::StringArray namespaces;
					blocxx::StringArray classes;
					classes.push_back("TestClass2");
					classes.push_back("TestClass3");
					IndicationProviderInfo::ClassInfo ci1("CIM_InstCreation", namespaces, classes);
					indi.addInstrumentedClass(ci1);
					IndicationProviderInfo::ClassInfo ci2("CIM_InstModification", namespaces, classes);
					indi.addInstrumentedClass(ci2);
					IndicationProviderInfo::ClassInfo ci3("CIM_InstDeletion", namespaces, classes);
					indi.addInstrumentedClass(ci3);
					inda.push_back(indi);
				}

			}
		}

		virtual InstanceProviderIFCRef doGetInstanceProvider(
			const ProviderEnvironmentIFCRef&, const char* provIdString)
		{
			if (blocxx::String(provIdString) == "TestInstanceProvider")
			{
				return InstanceProviderIFCRef(new TestInstanceProvider);
			}
			OW_THROW(NoSuchProviderException, provIdString);
		}

		virtual MethodProviderIFCRef doGetMethodProvider(
			const ProviderEnvironmentIFCRef&, const char* provIdString)
		{
			if (blocxx::String(provIdString) == "TestMethodProvider")
			{
				return MethodProviderIFCRef(new TestMethodProvider);
			}
			OW_THROW(NoSuchProviderException, provIdString);
		}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		virtual AssociatorProviderIFCRef doGetAssociatorProvider(
			const ProviderEnvironmentIFCRef&, const char* provIdString)
		{
			if (blocxx::String(provIdString) == "TestAssociatorProvider")
			{
				return AssociatorProviderIFCRef(new TestAssociatorProvider);
			}
			OW_THROW(NoSuchProviderException, provIdString);
		}
#endif

		virtual IndicationProviderIFCRef doGetIndicationProvider(
			const ProviderEnvironmentIFCRef&, const char* provIdString)
		{
			if (blocxx::String(provIdString) == "TestIndicationProvider"
				|| blocxx::String(provIdString) == "TestIndicationProvider1"
				|| blocxx::String(provIdString) == "TestIndicationProvider2")
			{
				return IndicationProviderIFCRef(new TestIndicationProvider);
			}
			OW_THROW(NoSuchProviderException, provIdString);
		}

		virtual const char* getName() const { return m_name.c_str(); }

		virtual PolledProviderIFCRefArray doGetPolledProviders(
			const ProviderEnvironmentIFCRef&)
		{
			return PolledProviderIFCRefArray();
		}

		virtual IndicationExportProviderIFCRefArray doGetIndicationExportProviders(
			const ProviderEnvironmentIFCRef&)
		{
			return IndicationExportProviderIFCRefArray();
		}

		virtual void doUnloadProviders(const ProviderEnvironmentIFCRef&)
		{
		}

private:
	blocxx::String m_name;

};

static ProviderIFCBaseIFC* testCreateProviderMux()
{
	return new testProviderMux( "lib1" );
}

static ProviderIFCBaseIFC* testCreateProviderMux2()
{
	return new testProviderMux( "lib2" );
}

static ProviderIFCBaseIFC* testCreateProviderMux3()
{
	return new testProviderMux( "lib3" );
}

class testMuxSharedLibrary: public blocxx::SharedLibrary
{
	public:
		virtual ~testMuxSharedLibrary(){}

	protected:
		virtual bool doGetFunctionPointer( const blocxx::String& name, void**fp ) const
		{
			if (name == "getOWVersion")
				*fp = (void*)(&versionFunction);
			else
				*fp = (void*)(&testCreateProviderMux);
			return true;
		}
};

class testMuxSharedLibrary2: public blocxx::SharedLibrary
{
	public:
		virtual ~testMuxSharedLibrary2(){}

	protected:
		virtual bool doGetFunctionPointer( const blocxx::String& name, void**fp ) const
		{
			if (name == "getOWVersion")
				*fp = (void*)(&versionFunction);
			else
				*fp = (void*)(&testCreateProviderMux2);
			return true;
		}
};

class testMuxSharedLibrary3: public blocxx::SharedLibrary
{
	public:
		virtual ~testMuxSharedLibrary3(){}

	protected:
		virtual bool doGetFunctionPointer( const blocxx::String& name, void**fp ) const
		{
			if (name == "getOWVersion")
				*fp = (void*)(&versionFunction);
			else
				*fp = (void*)(&testCreateProviderMux3);
			return true;
		}
};

class testSharedLibraryLoader: public blocxx::SharedLibraryLoader
{
	public:
		virtual ~testSharedLibraryLoader(){}

		virtual blocxx::SharedLibraryRef loadSharedLibrary( const blocxx::String& name ) const
		{
			if ( name == "lib1" )
				return blocxx::SharedLibraryRef( new testMuxSharedLibrary );
			else if ( name == "lib2" )
				return blocxx::SharedLibraryRef( new testMuxSharedLibrary2 );
			else if ( name == "lib3" )
				return blocxx::SharedLibraryRef( new testMuxSharedLibrary3 );
			else
				return blocxx::SharedLibraryRef( new testSharedLibrary );
		}
};

class testMuxLoader: public ProviderIFCLoaderBase
{
	public:
		testMuxLoader( blocxx::SharedLibraryLoaderRef sll ) :
			ProviderIFCLoaderBase(sll, g_testEnvironment)
			{}
		virtual ~testMuxLoader(){}
		virtual void loadIFCs(
				blocxx::Array<ProviderIFCBaseIFCRef>& out) const
		{
			ProviderIFCBaseIFCRef rval;
			rval = createProviderIFCFromLib( "lib1" );
			if ( rval  )
			{
				out.push_back( rval );
			}

			rval = createProviderIFCFromLib( "lib2" );
			if ( rval )
			{
				out.push_back( rval );
			}

			rval = createProviderIFCFromLib( "lib3" );
			if ( rval )
			{
				out.push_back( rval );
			}

		}
};

class testMuxLoaderBad: public ProviderIFCLoaderBase
{
	public:
		testMuxLoaderBad( blocxx::SharedLibraryLoaderRef sll ) :
			ProviderIFCLoaderBase(sll, g_testEnvironment)
			{}
		virtual ~testMuxLoaderBad(){}
		virtual void loadIFCs(
				blocxx::Array<ProviderIFCBaseIFCRef>& out) const
		{
			ProviderIFCBaseIFCRef rval;
			rval = createProviderIFCFromLib( "libbad" );
			if ( rval )
			{
				out.push_back( rval );
			}

			rval = createProviderIFCFromLib( "libbad" );
			if ( rval )
			{
				out.push_back( rval );
			}

			rval = createProviderIFCFromLib( "libbad" );
			if ( rval )
			{
				out.push_back( rval );
			}

		}
};

static blocxx::SharedLibraryLoaderRef testCreateSharedLibraryLoader();

static ProviderIFCLoaderRef testCreateMuxLoader()
{
	return ProviderIFCLoaderRef( new testMuxLoader(
				testCreateSharedLibraryLoader() ) );
}

static blocxx::SharedLibraryLoaderRef testCreateSharedLibraryLoader()
{
	return blocxx::SharedLibraryLoaderRef( new testSharedLibraryLoader );
}

#endif

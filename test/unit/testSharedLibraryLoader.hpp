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

#ifndef testSharedLibraryLoader_HPP_
#define testSharedLibraryLoader_HPP_

#include "OW_SharedLibrary.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_Reference.hpp"
#include "OW_ProviderIFCBaseIFC.hpp"
#include "OW_ProviderIFCLoader.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "UnitTestEnvironment.hpp"

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

class testProviderMux: public OW_ProviderIFCBaseIFC
{
	public:
		testProviderMux( const char* const name )
			: OW_ProviderIFCBaseIFC(), m_name(name) {}
		virtual ~testProviderMux() {}
		virtual OW_InstanceProviderIFCRef doGetInstanceProvider(
			const OW_ProviderEnvironmentIFCRef&, const char*)
		{
			return OW_InstanceProviderIFCRef(0);
		}

		virtual OW_MethodProviderIFCRef doGetMethodProvider(
			const OW_ProviderEnvironmentIFCRef&, const char*)
		{
			return OW_MethodProviderIFCRef(0);
		}

		virtual OW_PropertyProviderIFCRef doGetPropertyProvider(
			const OW_ProviderEnvironmentIFCRef&, const char*)
		{
			return OW_PropertyProviderIFCRef(0);
		}

		virtual OW_AssociatorProviderIFCRef doGetAssociatorProvider(
			const OW_ProviderEnvironmentIFCRef&, const char*)
		{
			return OW_AssociatorProviderIFCRef(0);
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
			
class testSharedLibraryLoader: public OW_SharedLibraryLoader
{
	public:
		virtual ~testSharedLibraryLoader(){}

		virtual OW_SharedLibraryRef loadSharedLibrary( const OW_String& name,
            OW_LoggerRef ) const
		{
			if ( name == "lib1" )
				return OW_SharedLibraryRef( new testMuxSharedLibrary );
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
		return OW_ProviderEnvironmentIFCRef(new testProviderEnvironment(ch,
			OW_LoggerRef(new DummyLogger)));
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

			rval = createProviderIFCFromLib( "lib1" );
			if ( !rval.isNull() )
			{
				out.push_back( rval );
			}

			rval = createProviderIFCFromLib( "lib1" );
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

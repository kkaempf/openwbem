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
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_Exception.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_SSLCtxMgr.hpp"

int main( int iArgC, char *asArgV[] )
{
	try
	{
		if( iArgC != 2 )
		{
			cout << "Usage: " << asArgV[ 0 ] << " <URL>" << endl;
			cout << "Example: " << asArgV[ 0 ] << " https://Login:Password@cimom.example.com:5989/cimom" << endl;
			return 1;
		}

		OW_String sURL( asArgV[ 1 ] );

		if( sURL.startsWith( "https://" ) )
			OW_SSLCtxMgr::initClient("/etc/ssl/private/hostkey+cert.pem");
	
		OW_AutoPtrNoVec<OW_CIMXMLCIMOMHandle> pClient;

		pClient = new OW_CIMXMLCIMOMHandle( sURL );

		OW_CIMObjectPath RootPath(OW_Bool(true));
		OW_CIMObjectPath ClassPath( "MyClass" );
		OW_CIMObjectPath InstancePath( "MyClass" );
		
		OW_CIMUrl Url( "http", "DigitalLoom", "/cimom", 5988 );
		OW_CIMNameSpace NameSpace( Url, "root/cimv2" );
		pClient->createNameSpace( NameSpace );

		pClient->enumNameSpace( RootPath, true );

		OW_CIMClass Class( "MyClass" );
		pClient->createClass( ClassPath, Class );	
	
		pClient->enumClass( RootPath, true, true );	

		OW_CIMInstance Instance( Class.newInstance() );
		Instance.setProperty( "Name", OW_String("MyInstance") );
		InstancePath.addKey( "Name", OW_String("MyInstance") );
		pClient->createInstance( InstancePath, Instance );	

		pClient->enumInstances( RootPath, true, true );
		
		pClient->deleteInstance( InstancePath );
		
		pClient->deleteClass( ClassPath );	
		
		pClient->deleteNameSpace( NameSpace );	
		
		return 0;
	}
	catch (OW_Exception& e)
	{
		cerr << e << endl;
		return 1;
	}
	return 0;
}

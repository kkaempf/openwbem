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
#include "OW_HTTPXMLCIMListener.hpp"
#include "OW_Assertion.hpp"
#include "OW_Semaphore.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_HTTPClient.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMException.hpp"

#include <iostream> // for cout and cerr

using std::cout;
using std::endl;
using std::cerr;

OW_Semaphore sem(0);
OW_Semaphore test1sem(0);
OW_Semaphore test2sem(0);
OW_Mutex coutMutex;

class myCallBack : public OW_CIMListenerCallback
{
public:
	myCallBack() : m_count(0) {}
protected:
	virtual void doIndicationOccurred(OW_CIMInstance &ci,
		const OW_String &listenerPath)
	{
		(void)listenerPath;
		// remove time property so output will always look alike (and allow
		// a diff in the acceptance script).
		ci.removeProperty("IndicationTime");
		OW_MutexLock lock(coutMutex);
		++m_count;
		cout << "\nmyCallBack::doIndicationOccurred: Got indication: " << m_count << "\n";
		cout << ci.toString() << "\n";
		sem.signal();
	}
private:
	int m_count;
};

class test1CallBack : public OW_CIMListenerCallback
{
protected:
	virtual void doIndicationOccurred(OW_CIMInstance &ci,
		const OW_String &listenerPath)
	{
		(void)listenerPath;
		// remove time property so output will always look alike (and allow
		// a diff in the acceptance script).
		ci.removeProperty("IndicationTime");
		OW_MutexLock lock(coutMutex);
		cout << "\ntest1CallBack::doIndicationOccurred: Got indication:\n";
		cout << ci.toString() << "\n";
		test1sem.signal();
	}
};

class test2CallBack : public OW_CIMListenerCallback
{
protected:
	virtual void doIndicationOccurred(OW_CIMInstance &ci,
		const OW_String &listenerPath)
	{
		(void)listenerPath;
		// remove time property so output will always look alike (and allow
		// a diff in the acceptance script).
		ci.removeProperty("IndicationTime");
		OW_MutexLock lock(coutMutex);
		cout << "\ntest2CallBack::doIndicationOccurred: Got indication:\n";
		cout << ci.toString() << "\n";
		test2sem.signal();
	}
};



void createClass(OW_CIMOMHandleIFC& hdl)
{
	OW_CIMQualifierType cqt = hdl.getQualifierType("root/testsuite", "Key");
	OW_CIMQualifier cimQualifierKey(cqt);
	cimQualifierKey.setValue(OW_CIMValue(OW_Bool(true)));

	OW_CIMClass cimClass(OW_Bool(true));
	cimClass.setName("EXP_IndicationTestComputerSystem");
	cimClass.setSuperClass("CIM_ComputerSystem");

	OW_Array<OW_String> zargs;
	zargs.push_back("CreationClassName");
	zargs.push_back("string");
	zargs.push_back("true");
	zargs.push_back("Name");
	zargs.push_back("string");
	zargs.push_back("true");
	zargs.push_back("OptionalArg");
	zargs.push_back("boolean");
	zargs.push_back("false");

	OW_String name;
	OW_String type;
	OW_String isKey;

	for (size_t i = 0; i < zargs.size(); i += 3)
	{
		name = zargs[i];
		type = zargs[i + 1];
		isKey = zargs[i + 2];
		OW_CIMProperty cimProp(OW_Bool(true));
		if (type.equals("string"))
		{
			cimProp.setDataType(OW_CIMDataType::STRING);
		}
		else if (type.equals("uint16"))
		{
			cimProp.setDataType(OW_CIMDataType::UINT16);
		}
		else if (type.equals("boolean"))
		{
			cimProp.setDataType(OW_CIMDataType::BOOLEAN);
		}
		else if (type.equals("datatime"))
		{
			cimProp.setDataType(OW_CIMDataType::DATETIME);
		}
		else cimProp.setDataType(OW_CIMDataType::STRING);

		cimProp.setName(name);
		if (isKey.equals("true"))
		{
			cimProp.addQualifier(cimQualifierKey);
		}
		cimClass.addProperty(cimProp);
	}

	hdl.createClass("root/testsuite", cimClass);
}


void modifyClass(OW_CIMOMHandleIFC& hdl)
{
	OW_CIMClass cimClass = hdl.getClass("root/testsuite",
		"EXP_IndicationTestComputerSystem", false);
	OW_CIMProperty cimProp(OW_Bool(true));
	cimProp.setDataType(OW_CIMDataType::STRING);
	cimProp.setName("BrandNewProperty");
	cimClass.addProperty(cimProp);
	hdl.modifyClass("root/testsuite", cimClass);
}


void createInstance(OW_CIMOMHandleIFC& hdl, const OW_String& newInstance)
{
	OW_String fromClass = "EXP_IndicationTestComputerSystem";

	OW_CIMClass cimClass = hdl.getClass("root/testsuite", fromClass, false);

	OW_CIMInstance newInst = cimClass.newInstance();

	newInst.setName(newInstance);
	newInst.setProperty(OW_CIMProperty::NAME_PROPERTY,
							  OW_CIMValue(newInstance));
	newInst.setProperty("CreationClassName",
							  OW_CIMValue(fromClass));

	OW_CIMObjectPath cop(fromClass, newInst.getKeyValuePairs());
	cop.setNameSpace("root/testsuite");

	hdl.createInstance(cop, newInst);
}


void getInstance(OW_CIMOMHandleIFC& hdl, const OW_String& theInstance)
{
	OW_String ofClass = "EXP_IndicationTestComputerSystem";
	OW_CIMObjectPath cop(ofClass);
	cop.addKey("CreationClassName", OW_CIMValue(ofClass));
	cop.addKey("Name", OW_CIMValue(theInstance));

	OW_CIMInstance in = hdl.getInstance("root/testsuite", cop, false);
	//cout << in.toMOF();
}

void modifyInstance(OW_CIMOMHandleIFC& hdl, const OW_String& theInstance)
{
	OW_String ofClass = "EXP_IndicationTestComputerSystem";
	OW_CIMObjectPath cop(ofClass, "root/testsuite");
	cop.addKey("CreationClassName", OW_CIMValue(ofClass));
	cop.addKey("Name", OW_CIMValue(theInstance));

	OW_CIMInstance in = hdl.getInstance("root/testsuite", cop, false);

	in.setProperty(OW_CIMProperty("BrandNewProperty",
											OW_CIMValue(OW_Bool(true))));

	hdl.modifyInstance(cop, in);
}

void deleteInstance(OW_CIMOMHandleIFC& hdl, const OW_String& theInstance)
{
	OW_String ofClass = "EXP_IndicationTestComputerSystem";
	OW_CIMObjectPath cop(ofClass, "root/testsuite");
	cop.addKey("CreationClassName", OW_CIMValue(ofClass));
	cop.addKey("Name", OW_CIMValue(theInstance));
	hdl.deleteInstance("root/testsuite", cop);
}



void deleteClass(OW_CIMOMHandleIFC& hdl)
{
	OW_String delClass = "EXP_IndicationTestComputerSystem";
	hdl.deleteClass("root/testsuite", delClass);
}

class ListenerLogger : public OW_Logger
{
protected:
	virtual void doLogMessage(const OW_String &message, const OW_LogLevel) const
	{
		cout << message << endl;
	}
};

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 2 || argc > 3)
		{
			cerr << "Usage: " << argv[0] << " <URL> [dump file prefix]" << endl;
			exit(1);
		}

		OW_String url(argv[1]);

		if (argc == 3)
		{
			OW_String sockDumpOut = argv[2];
			OW_String sockDumpIn = argv[2];
			sockDumpOut += "SockDumpOut";
			sockDumpIn += "SockDumpIn";
			cerr << "argc = " << argc << " sockDumpOut = " << sockDumpOut << endl;
			OW_SocketBaseImpl::setDumpFiles(sockDumpIn.c_str(),
				sockDumpOut.c_str());
		}

		OW_CIMListenerCallbackRef mcb(new myCallBack);
		OW_CIMListenerCallbackRef test1cb(new test1CallBack);
		OW_CIMListenerCallbackRef test2cb(new test2CallBack);

		OW_LoggerRef logger(new ListenerLogger);

		OW_HTTPXMLCIMListener hxcl(logger);

		OW_CIMNameSpace path("/root/testsuite");
		OW_String ns("/root/testsuite");

		OW_CIMProtocolIFCRef httpClient(new OW_HTTPClient(url));
		OW_CIMXMLCIMOMHandle rch(httpClient);

		rch.execQueryE(path, "delete from CIM_IndicationSubscription", "wql2");


		OW_Array<OW_String> registrationHandles;
		OW_String handle;

		if (getenv("OWLONGTEST"))
		{
			handle = hxcl.registerForIndication(url, ns,
				"select * from OW_TestIndication1 where TheClass ISA "
				"\"TestClass1\"", "wql1", test1cb);
			registrationHandles.append(handle);

			handle = hxcl.registerForIndication(url, ns,
				"select * from OW_TestIndication2 where TheInstance ISA "
				"\"TestClass2\" and TheInstance.dummykey = \"foo\"", "wql1",
				test2cb);
			registrationHandles.append(handle);

			// Now wait for our test trigger providers to send out their indications.
			// we should get 8 OW_TestIndication1 indications.
			for (size_t i = 0; i < 8; ++i)
			{
				if (!test1sem.wait(25 * 1000))
				{
					OW_THROW(OW_Exception, "semaphore timed out");
				}
			}
			// we should get 6 OW_TestIndication2 indications.
			for (size_t i = 0; i < 6; ++i)
			{
				if (!test2sem.wait(25 * 1000))
				{
					OW_THROW(OW_Exception, "semaphore timed out");
				}
			}

			// now deregister
			OW_MutexLock guard1(coutMutex);
			cout << "Now deregistering for OW_TestIndication1 and OW_TestIndication2" << endl;
			guard1.release();

			for (size_t i = 0; i < registrationHandles.size(); ++i)
			{
				hxcl.deregisterForIndication(registrationHandles[i]);
			}
			registrationHandles.clear();

		}
		
		// now let's register for all the intrinsic method indications.


		try
		{
			deleteInstance(rch, "MyInstance");
		}
		catch(OW_CIMException& e)
		{
			OW_MutexLock guard2(coutMutex);
			cout << e.getMessage() << endl;
		}
		try
		{
			deleteClass(rch);
		}
		catch(OW_CIMException& e)
		{
			OW_MutexLock guard3(coutMutex);
			cout << e.getMessage() << endl;
		}

		handle = hxcl.registerForIndication(url, ns, "select * from CIM_ClassIndication", "wql1", mcb);
		registrationHandles.append(handle);

		handle = hxcl.registerForIndication(url, ns, "select * from CIM_ClassCreation", "wql1", mcb);
		registrationHandles.append(handle);

		handle = hxcl.registerForIndication(url, ns, "select * from CIM_ClassDeletion", "wql1", mcb);
		registrationHandles.append(handle);

		handle = hxcl.registerForIndication(url, ns, "select * from CIM_ClassModification", "wql1", mcb);
		registrationHandles.append(handle);

		handle = hxcl.registerForIndication(url, ns, "select * from CIM_InstModification", "wql1", mcb);
		registrationHandles.append(handle);

		handle = hxcl.registerForIndication(url, ns, "select * from CIM_InstDeletion", "wql1", mcb);
		registrationHandles.append(handle);

		handle = hxcl.registerForIndication(url, ns, "select * from CIM_InstRead", "wql1", mcb);
		registrationHandles.append(handle);

		handle = hxcl.registerForIndication(url, ns, "select * from CIM_InstCreation", "wql1", mcb);
		registrationHandles.append(handle);

		handle = hxcl.registerForIndication(url, ns, "select * from CIM_InstIndication", "wql1", mcb);
		registrationHandles.append(handle);


		//handle = hxcl.registerForIndication(url, ns, "select * from CIM_InstMethodCall", "wql1", mcb);
		//registrationHandles.append(handle);


		//cout << "Executing createClass()" << endl;
		createClass(rch);
		//cout << "Executing createInstance()" << endl;
		createInstance(rch, "MyInstance");
		//cout << "Executing modifyInstance()" << endl;
		modifyInstance(rch, "MyInstance");
		//cout << "Executing getInstance()" << endl;
		getInstance(rch, "MyInstance");
		//cout << "Executing deleteInstance()" << endl;
		deleteInstance(rch, "MyInstance");
		//cout << "Executing modifyClass()" << endl;
		modifyClass(rch);
		//cout << "Executing deleteClass()" << endl;
		deleteClass(rch);
		//cout << "Done running intrinsic methods." << endl;
		//invokeMethod(rch, 2); // TODO

		//cout << "Now waiting for intrinsic method indications" << endl;
		for (size_t i = 0; i < 21; ++i)
		{
			if (!sem.wait(25 * 1000))
			{
				OW_THROW(OW_Exception, "timeout on semaphore");
			}
			OW_MutexLock guard4(coutMutex);
			cout << i << endl;
		}

		OW_MutexLock guard5(coutMutex);
		cout << "Now deregistering and shutting down..." << endl;
		guard5.release();

		hxcl.shutdownHttpServer();
		for (size_t i = 0; i < registrationHandles.size(); ++i)
		{
			hxcl.deregisterForIndication(registrationHandles[i]);
		}

		// The acceptance script greps for "Success" in the output to tell
		// whether the test worked or not.
		cout << "Success!" << endl;
		return 0;
	}
	catch(OW_Exception& e)
	{
		cerr << e << endl;
	}
    catch(std::exception& e)
    {
        cerr << e.what() << endl;
    }
	catch(...)
	{
		cerr << "Caught unknown exception in main" << endl;
	}
	return 1;
}


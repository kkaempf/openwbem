/*
 * NPIBaseProvider.cpp
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE 
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE 
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author: Markus Mueller <markus_mueller@de.ibm.com>
 * Contributors:
 *
 * Description: <Some descriptive text>
 */

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMOMHandle.h>
#include <unistd.h>
#include <stdio.h>

#include <Pegasus/Provider2/SimpleResponseHandler.cpp>

#include "NPIBaseProvider.h"
#include "NPIExternal.h"


static const char * libPerl = "libperlProvider.so";

PEGASUS_NAMESPACE_BEGIN

NPIBaseProvider::NPIBaseProvider() { }

NPIBaseProvider::~NPIBaseProvider()
{
    NPI_terminate();
}

void NPIBaseProvider::NPI_initialize(CIMOMHandle& cimom, char * libraryName)
{
    DDD (cerr << "NPIBaseProvider::initialize() at " << (void *)(this) <<endl;)

    //_NPIoutputQueue = new MessageQueue();
    //_cimomHandle = new CIMOMHandle( _NPIoutputQueue, cimom.getRepository());
    _cimomHandle = cimom;
    _repository = cimom.getRepository();

    DDD (cerr<<"NPIBaseProvider: CimomHandle at "<<(void *)_repository<<endl;)

    _npiHandle = new(NPIHandle);
    _npiHandle->jniEnv = (void *)this;
    _npiHandle->errorOccurred = 0;
    _npiHandle->providerError = NULL;
    _npiHandle->thisObject = NULL;

    _libraryCount = 0;

    // load the library

    DDD( cerr << "NPIProvider:: - load library " << libraryName << "\n";)

    // provision for perl: library name ends with .pl
    //
    // hide perl library in providerError
    //
    // and start libperlProvider.so
    //
    Uint32 liblen = strlen(libraryName);
    const char * _libName;

    if (liblen > 3)
    {
        if (!strncasecmp(&libraryName[liblen-3],".pl",3))
        {
             // yes, found perl
             _libName = libPerl;

             // abuse the providerError field
             _npiHandle->providerError = libraryName;
             setOperationContext(libraryName);
        }
        else {
             _libName = libraryName;
             setOperationContext(NULL);
        }
    }
    else _libName = libraryName;

    char * _initSym = strdup(&_libName[3]);
    char * dot = strrchr( _initSym,(int) '.');
    if (dot) dot[0] = '\0';
    String initSym(_initSym);
    free(_initSym);
    initSym += "_initFunctionTable";
    _initSym = initSym.allocateCString();
    _libraryHandle = ::dlopen(_libName,RTLD_NOW|RTLD_GLOBAL);

    if (dlerror())
    {
        cerr << "NPIProvider::initialize() failed to load library " << _libName;
        throw CIMException(CIM_ERR_NOT_SUPPORTED);
    }
    DDD (cerr << "NPIProvider: initialize - activate " << initSym << endl;)

    FP_INIT_FT initFct = (FP_INIT_FT) ::dlsym(_libraryHandle, _initSym);
    if (initFct != NULL) {
        DDD(cerr << "FTABLE " << (void *)initFct<< endl;)
        _functionTable = initFct();

        ::CIMOMHandle ch = {(void *)_repository};
        if (_functionTable.fp_initialize != NULL)
            _functionTable.fp_initialize(_npiHandle, ch);
        else {
            cerr << "NPIProvider:  problem with ftable->initialize()\n";
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }
        DDD( cerr << "NPIProvider: initialized: ";)
        DDD(cerr<<"FTABLE.init "<<(void *)_functionTable.fp_initialize << endl;)
        DDD(cerr<<"FTABLE.get  "<<(void *)_functionTable.fp_getInstance<< endl;)
        DDD(cerr<<"FTABLE.enIn "<<(void *)_functionTable.
                             fp_enumInstances<< endl;)
        DDD(cerr<<"FTABLE.assoc "<<(void *)_functionTable.
                             fp_associators<< endl;)
    }
    else {
        cerr << "NPIProvider: initialize - problem with ftable\n";
        throw CIMException(CIM_ERR_NOT_SUPPORTED);
    }
}

void NPIBaseProvider::NPI_terminate()
{
    DDD (cerr<<"NPIBaseProvider::Explicit termination\n";)
    DDD (cerr<<"NPIBaseProvider:: my address was " << (void *)(this) <<endl;)
    if (_libraryHandle) {
        if (_functionTable.fp_cleanup != NULL)
            _functionTable.fp_cleanup(_npiHandle);
        dlclose(_libraryHandle);
        _libraryHandle = NULL;
    }
    //delete _NPIoutputQueue;
}

char * NPIBaseProvider::getOperationContext()
{
    return _operationContext;
}

void NPIBaseProvider::setOperationContext(char * oc)
{
    _operationContext = oc;
}



PEGASUS_NAMESPACE_END

//externalized functions
//#include "NPIExternalImpl.h"

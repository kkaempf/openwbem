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
#ifndef __NPIBASEPROVIDER_HPP__
#define __NPIBASEPROVIDER_HPP__

#include "NPIExternal.hpp"
#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider2/CIMInstanceProvider.h>


PEGASUS_NAMESPACE_BEGIN

class PEGASUS_PROVIDER_LINKAGE NPIBaseProvider : public virtual CIMBaseProvider
{
public:

    NPIBaseProvider();
    //virtual ~NPIBaseProvider();
    ~NPIBaseProvider();

    //CIMBaseProvider Interface
    virtual void initialize(CIMOMHandle & cimom) {}
    virtual void terminate() {}
    void NPI_initialize(CIMOMHandle & cimom, char * libraryName);
    void NPI_terminate();
 
    // this function is introduced to be overloaded if necessary

    char * getOperationContext();
    void setOperationContext(char * oc);

protected:

    NPIHandle * _npiHandle;
    CIMOMHandle _cimomHandle;
    CIMRepository * _repository;
    // necessary to construct cimomhandle with synchronous message queue
    //MessageQueue * _NPIoutputQueue;
    NPIenv * _env;
    int _libraryCount;
    void * _libraryHandle;
    FTABLE _functionTable;

    char * _operationContext; // needed for perl
};

PEGASUS_NAMESPACE_END

#endif // __NPIBASEPROVIDER_HPP__


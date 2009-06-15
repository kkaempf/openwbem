/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc., IBM Corp. All rights reserved.
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
*
* Author:        Markus Mueller <sedgewick_de@yahoo.de>
*
*******************************************************************************/
#ifndef OW_FTABLE_REF_HPP_
#define OW_FTABLE_REF_HPP_
#include "OW_config.h"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMProperty.hpp"
#include "blocxx/String.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMParameter.hpp"
#include "blocxx/Array.hpp"
#include "OW_CIMBase.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "blocxx/SharedLibraryReference.hpp"
#include "blocxx/DateTime.hpp"
#include "blocxx/Reference.hpp"
#include "cmpift.h"
#include "cmpisrv.h"

namespace OW_NAMESPACE
{

typedef struct {
	::MIs miVector;
	::CMPI_Broker broker;
	OpenWBEM::DateTime lastAccessTime;
} CompleteMI; 

typedef SharedLibraryReference< Reference<CompleteMI> > CMPIFTABLERef;

} // end namespace OW_NAMESPACE

#endif

/*******************************************************************************
* Copyright (C) 2008 Quest Software, Inc. All rights reserved.
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
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "DummyProvEnv.hpp"
#include "OW_RepositoryIFC.hpp"


using namespace OpenWBEM;
using namespace blocxx;

DummyProvEnv::~DummyProvEnv()
{
}

CIMOMHandleIFCRef DummyProvEnv::getCIMOMHandle() const
{
	return 0;
}

CIMOMHandleIFCRef DummyProvEnv::getRepositoryCIMOMHandle() const
{
	return 0;
}

RepositoryIFCRef DummyProvEnv::getRepository() const
{
	return 0;
}

RepositoryIFCRef DummyProvEnv::getAuthorizingRepository() const
{
	return 0;
}

String DummyProvEnv::getConfigItem(
	const String &name, const String& defRetVal) const
{
	return defRetVal;
}

StringArray DummyProvEnv::getMultiConfigItem(
	const String &itemName, const StringArray& defRetVal,
	const char* tokenizeSeparator) const
{
	return defRetVal;
}

String DummyProvEnv::getUserName() const
{
	return String();
}

OperationContext& DummyProvEnv::getOperationContext()
{
	return *static_cast<OperationContext *>(0);
}

ProviderEnvironmentIFCRef DummyProvEnv::clone() const
{
	return new DummyProvEnv();
}

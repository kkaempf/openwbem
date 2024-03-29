/*******************************************************************************
* Copyright (C) 2006 Novell, Inc. All rights reserved.
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
*******************************************************************************/
#ifndef OW_OOPCLONEDPROVIDERENV_CPP1_HPP_INCLUDE_GUARD_
#define OW_OOPCLONEDPROVIDERENV_CPP1_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/UnnamedPipe.hpp"
#include "OW_ProviderFwd.hpp"
#include "blocxx/Runnable.hpp"

namespace OW_NAMESPACE
{
class OOPProviderBase;

class OOPClonedProviderEnv : public blocxx::Runnable
{
public:
	OOPClonedProviderEnv(OOPProviderBase* pprov, const blocxx::UnnamedPipeRef& conn, const ProviderEnvironmentIFCRef& provEnv);
	virtual void run();
	virtual void doShutdown();

private:
	OOPProviderBase* m_pprov;
	blocxx::UnnamedPipeRef m_conn;
	ProviderEnvironmentIFCRef m_env;
};

}	// End of OW_NAMESPACE

#endif	//OW_OOPCLONEDPROVIDERENV_CPP1_HPP_INCLUDE_GUARD_

/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include <OW_ClientAuthCBIFC.hpp>
// note this comes *after* the OpenWBEM headers, because it has a
// #define ANY void
// which really screws up OpenWBEM
#include <boost/python.hpp>
#ifdef ANY
#undef ANY
#endif

namespace OpenWBEM
{

using namespace boost::python;
namespace {
struct ClientAuthCBIFCWrap : ClientAuthCBIFC
{
	ClientAuthCBIFCWrap(PyObject* self_)
		: self(self_) {}
	bool getCredentials(const String& realm, String& name,
						String& passwd, const String& details)
	{ 
		return call_method<int>(self, "getCredentials", 
				realm, name, passwd, details); 
	}
	PyObject* self;
};
}
void registerClientAuthCBIFC()
{
	class_<ClientAuthCBIFC, ClientAuthCBIFCWrap, boost::noncopyable>
		("ClientAuthCBIFC", no_init)
	   // .def("getCredentials", &ClientAuthCBIFC::getCredentials)
	;
}

} // end namespace OpenWBEM


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

#ifndef OW_XMLClass_HPP_
#define OW_XMLClass_HPP_

#include "OW_config.h"
#include "OW_XMLOperationGeneric.hpp"
#include "OW_XMLNode.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_String.hpp"

class OW_XMLClass : public OW_XMLOperationGeneric
{
public:
	static OW_String getNameSpace(const OW_XMLNode& localNameNode);
	static OW_CIMObjectPath getObjectWithPath(OW_XMLNode& node,
			OW_CIMClassArray& cArray, OW_CIMInstanceArray& iArray);
	static OW_CIMClass readClass(OW_XMLNode& childNode, OW_CIMObjectPath& path);
	static OW_CIMInstance readInstance(OW_XMLNode& childNode,
			OW_CIMObjectPath& path);
	static void getInstanceName(OW_XMLNode& result, OW_CIMObjectPath& cimPath);

};


#endif  // OW_XMLClass_HPP_


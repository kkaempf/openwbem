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

#ifndef OW_RESULTHANDLERIFC_HPP_
#define OW_RESULTHANDLERIFC_HPP_

#include "OW_config.h"
#include "OW_CIMFwd.hpp"

class OW_CIMClassResultHandlerIFC
{
public:
	void handleClass(const OW_CIMClass& c)
	{
		doHandleClass(c);
	}

protected:
	virtual void doHandleClass(const OW_CIMClass& c) = 0;

};

class OW_CIMInstanceResultHandlerIFC
{
public:
	void handleInstance(const OW_CIMInstance& i)
	{
		doHandleInstance(i);
	}

protected:
	virtual void doHandleInstance(const OW_CIMInstance& i) = 0;

};

class OW_CIMObjectPathResultHandlerIFC
{
public:
	void handleObjectPath(const OW_CIMObjectPath& cop)
	{
		doHandleObjectPath(cop);
	}

protected:
	virtual void doHandleObjectPath(const OW_CIMObjectPath& cop) = 0;

};

class OW_StringResultHandlerIFC
{
public:
	void handleString(const OW_String& s)
	{
		doHandleString(s);
	}

protected:
	virtual void doHandleString(const OW_String& s) = 0;

};

class OW_CIMQualifierTypeResultHandlerIFC
{
public:
	void handleQualifierType(const OW_CIMQualifierType& s)
	{
		doHandleQualifierType(s);
	}

protected:
	virtual void doHandleQualifierType(const OW_CIMQualifierType& qt) = 0;

};

#endif


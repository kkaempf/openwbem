/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
*******************************************************************************/

/**
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#ifndef OW_RESULTHANDLERIFC_HPP_
#define OW_RESULTHANDLERIFC_HPP_
#include "OW_config.h"
#include "OW_CIMFwd.hpp"

namespace OpenWBEM
{

/*
 * An abstract callback object to handle results from some form of operation.
 */
template <typename T>
class ResultHandlerIFC
{
public:
	/*
	 * Handle a result.
	 * An exception may be thrown by a derived class.
	 *
	 * @param x A value that needs to be handled.
	 */
	void handle(const T& x)
	{
		doHandle(x);
	}
	virtual ~ResultHandlerIFC();
protected:
	virtual void doHandle(const T& x) = 0;
};
template <typename T>
ResultHandlerIFC<T>::~ResultHandlerIFC()
{
}

typedef ResultHandlerIFC<CIMClass> CIMClassResultHandlerIFC;
typedef ResultHandlerIFC<CIMInstance> CIMInstanceResultHandlerIFC;
typedef ResultHandlerIFC<CIMObjectPath> CIMObjectPathResultHandlerIFC;
typedef ResultHandlerIFC<String> StringResultHandlerIFC;
typedef ResultHandlerIFC<CIMQualifierType> CIMQualifierTypeResultHandlerIFC;

} // end namespace OpenWBEM

#endif

/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
#ifndef OW_SELECT_HPP_
#define OW_SELECT_HPP_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_Array.hpp"
#include "OW_SelectableIFC.hpp"

namespace OpenWBEM
{

typedef Array<Select_t> SelectTypeArray;
class Select
{
public:
   /**
    * The value returned from select when the timeout value has expired
	*/
   static const int SELECT_TIMEOUT = -2;
   /**
    * The value returned from select when any error occurs other than timeout.
	*/
   static const int SELECT_ERROR = -1;
   /**
    * The value returned from select when select is interrupted by a signal.
	*/
   static const int SELECT_INTERRUPTED = -3;
   /**
	* Select returns as soon as input is available on any of Select_t
	* objects that are in given array.
	*
	* @param selarray An array of Select_t objects that will be used while
	*	waiting for input to become available.
	*
	* @param ms The timeout value specified in millseconds
	*
	* @return On success, the index in the selarray of the first Select_t
	* object that input has become available on. SELECT_ERROR on error.
	* SELECT_TIMEOUT if the given timeout value has expired.
	*/
   static int select(const SelectTypeArray& selarray, UInt32 ms = ~0U);
private:
	Select(); // don't allow instantiation.
};

} // end namespace OpenWBEM

typedef OpenWBEM::Select OW_Select;
typedef OpenWBEM::SelectTypeArray OW_SelectTypeArray;

#endif // OW_SELECT_HPP_

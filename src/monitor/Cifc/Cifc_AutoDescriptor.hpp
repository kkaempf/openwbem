#ifndef OW_AUTO_DESCRIPTOR_HPP_INCLUDE_GUARD_
#define OW_AUTO_DESCRIPTOR_HPP_INCLUDE_GUARD_

/*****************************************************************************
* Copyright (C) 2004-2006, Quest Software, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Quest Software, Inc., nor the
*       names of its contributors or employees may be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
* @author Kevin S. Van Horn
*/

/**
 * If you make improvements to this file, consider making the same
 * improvements in AutoDescriptor.hpp in BloCxx
 */

#include "OW_config.h"
#include "OW_AutoResource.hpp"

namespace OW_NAMESPACE
{

	struct AutoDescriptorPolicy
	{
		typedef int handle_type;

		static handle_type null()
		{
			return -1;
		}

		static void free(handle_type h);

		static bool equal(handle_type h1, handle_type h2)
		{
			return h1 == h2;
		}
	};

	/**
	* An analog of std::auto_ptr for descriptors.
	* @see AutoResource for details
	*/
	typedef AutoResource<AutoDescriptorPolicy> AutoDescriptor;

} // namespace OW_NAMESPACE

#endif

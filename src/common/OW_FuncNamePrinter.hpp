/*******************************************************************************
* Copyright (C) 2005, Vintela, Inc. All rights reserved.
* Copyright (C) 2006, Novell, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of 
*       Vintela, Inc., 
*       nor Novell, Inc., 
*       nor the names of its contributors or employees may be used to 
*       endorse or promote products derived from this software without 
*       specific prior written permission.
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
 * @author <see corresponding BloCxx header>
 */




#ifndef OW_FuncNamePrinter_HPP_INCLUDE_GUARD_
#define OW_FuncNamePrinter_HPP_INCLUDE_GUARD_

#include "OW_config.h"


// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

// For printing function names during debug
#ifdef OW_PRINT_FUNC_DEBUG
#include <blocxx/FuncNamePrinter.hpp>
#else
#define PRINT_FUNC_NAME_ARGS1(a)
#define PRINT_FUNC_NAME_ARGS2(a, b)
#define PRINT_FUNC_NAME_ARGS3(a, b, c)
#define PRINT_FUNC_NAME_ARGS4(a, b, c, d)
#define PRINT_FUNC_NAME_ARGS5(a, b, c, d, e)
#define PRINT_FUNC_NAME_ARGS6(a, b, c, d, e, f)
#define PRINT_FUNC_NAME_ARGS7(a, b, c, d, e, f, g)
#define PRINT_FUNC_NAME_ARGS8(a, b, c, d, e, f, g, h)
#define PRINT_FUNC_NAME_ARGS9(a, b, c, d, e, f, g, h, i)
#define PRINT_FUNC_NAME_ARGS10(a, b, c, d, e, f, g, h, i, j)
#define PRINT_FUNC_NAME
#endif /* #ifdef OW_PRINT_FUNC_DEBUG */


#endif //#define OW_FUNC_NAME_PRINTER_HPP

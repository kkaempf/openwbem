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


#ifndef OW_FUNC_NAME_PRINTER_HPP
#define OW_FUNC_NAME_PRINTER_HPP

#include "OW_config.h"
#include "OW_Format.hpp"
#include <stdio.h>
#include <unistd.h>


class OW_FuncNamePrinter
{
private:
	const char* m_funcName;
public:
	
	OW_FuncNamePrinter(const char* funcName) : m_funcName(funcName)
	{
		fprintf(stderr, "%d Entering: %s\n", getpid(), funcName);
	}
	template<typename A>
		OW_FuncNamePrinter(const char* funcName, const A& a) : m_funcName(funcName)
		{
			fprintf(stderr, "%d Entering: %s\n\t%s\n", getpid(), funcName, 
				format("(%1)", a).c_str());
		}
	template<typename A, typename B>
		OW_FuncNamePrinter(const char* funcName, const A& a, const B& b) : m_funcName(funcName)
		{
			fprintf(stderr, "%d Entering: %s\n\t%s\n", getpid(), funcName, 
				format("(%1, %2)", a, b).c_str());
		}
	template<typename A, typename B, typename C>
		OW_FuncNamePrinter(const char* funcName, const A& a, const B& b, const C& c) : m_funcName(funcName)
		{
			fprintf(stderr, "%d Entering: %s\n\t%s\n", getpid(), funcName, 
				format("(%1, %2, %3)", a, b, c).c_str());
		}
	template<typename A, typename B, typename C, typename D>
		OW_FuncNamePrinter(const char* funcName, const A& a, const B& b, const C& c, const D& d) : m_funcName(funcName)
		{
			fprintf(stderr, "%d Entering: %s\n\t%s\n", getpid(), funcName, 
				format("(%1, %2, %3, %4)", a, b, c, d).c_str());
		}
	template<typename A, typename B, typename C, typename D, typename E>
		OW_FuncNamePrinter(const char* funcName, const A& a, const B& b, const C& c, const D& d, const E& e) : m_funcName(funcName)
		{
			fprintf(stderr, "%d Entering: %s\n\t%s\n", getpid(), funcName, 
				format("(%1, %2, %3, %4, %5)", a, b, c, d, e).c_str());
		}
	template<typename A, typename B, typename C, typename D, typename E, typename F>
		OW_FuncNamePrinter(const char* funcName, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f) : m_funcName(funcName)
		{
			fprintf(stderr, "%d Entering: %s\n\t%s\n", getpid(), funcName, 
				format("(%1, %2, %3, %4, %5, %6)", a, b, c, d, e, f).c_str());
		}
	template<typename A, typename B, typename C, typename D, typename E, typename F,
	typename G>
		OW_FuncNamePrinter(const char* funcName, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g) : m_funcName(funcName)
		{
			fprintf(stderr, "%d Entering: %s\n\t%s\n", getpid(), funcName, 
				format("(%1, %2, %3, %4, %5, %6, %7)", a, b, c, d, e, f, g).c_str());
		}
	template<typename A, typename B, typename C, typename D, typename E, typename F,
	typename G, typename H>
		OW_FuncNamePrinter(const char* funcName, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h) : m_funcName(funcName)
		{
			fprintf(stderr, "%d Entering: %s\n\t%s\n", getpid(), funcName, 
				format("(%1, %2, %3, %4, %5, %6, %7, %8)", a, b, c, d, e, f, g, h).c_str());
		}
	template<typename A, typename B, typename C, typename D, typename E, typename F,
	typename G, typename H, typename I>
		OW_FuncNamePrinter(const char* funcName, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h, const I& i) : m_funcName(funcName)
		{
			fprintf(stderr, "%d Entering: %s\n\t%s\n", getpid(), funcName, 
				format("(%1, %2, %3, %4, %5, %6, %7, %8, %9)", a, b, c, d, e, f, g, h, i).c_str());
		}
	template<typename A, typename B, typename C, typename D, typename E, typename F,
	typename G, typename H, typename I, typename J>
		OW_FuncNamePrinter(const char* funcName, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h, const I& i, const J& j) : m_funcName(funcName)
		{
			fprintf(stderr, "%d Entering: %s\n\t%s\n", getpid(), funcName, 
				format("(%1, %2, %3, %4, %5, %6, %7, %8, %9)", a, b, c, d, e, f, g, h, format("%1, %2", i, j)).c_str());
		}

	~OW_FuncNamePrinter()
		{ fprintf(stderr, "%d Leaving:  %s\n", getpid(), m_funcName); }

}; // class OW_FuncNamePrinter



#endif //#define OW_FUNC_NAME_PRINTER_HPP

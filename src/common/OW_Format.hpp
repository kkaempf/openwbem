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


#ifndef OW_FORMAT_HPP
#define OW_FORMAT_HPP

#include "OW_config.h"
#include <iosfwd>
#include "OW_StringStream.hpp"
#include "OW_String.hpp"

//  Format class declaration  -----------------------------------------------//

class OW_Format
{
private:
	OW_StringStream oss;


	char process(OW_String& f, char c0);

	template<typename T>
		void put(const T& t)
		{ // t is inserted into oss

			if (!oss.good())
				return;

			oss << t;
		} // put

	friend std::ostream&
		operator<<(std::ostream& os, const OW_Format& f)
		{
			os << f.oss.toString();
			return os;
		}

public:
	
	operator OW_String() const;
	OW_String toString() const;
	const char* c_str() const;

	template<typename A>
		OW_Format(const char* ca, const A& a) : oss()
		{
			OW_String fmt(ca);
			while (fmt.length() > 0)
			{
				switch (process(fmt, '1'))
				{
					case '1': put(a); break;
				}
			}
		}
	template<typename A, typename B>
		OW_Format(const char* ca, const A& a, const B& b) : oss()
		{
			OW_String fmt(ca);
			while (fmt.length() > 0)
			{
				switch (process(fmt, '2'))
				{
					case '1': put(a); break;
					case '2': put(b); break;
				}
			}
		}
	template<typename A, typename B, typename C>
		OW_Format(const char* ca, const A& a, const B& b, const C& c) : oss()
		{
			OW_String fmt(ca);
			while (fmt.length() > 0)
			{
				switch (process(fmt, '3'))
				{
					case '1': put(a); break;
					case '2': put(b); break;
					case '3': put(c); break;
				}
			}
		}
	template<typename A, typename B, typename C, typename D>
		OW_Format(const char* ca, const A& a, const B& b, const C& c, const D& d) : oss()
		{
			OW_String fmt(ca);
			while (fmt.length() > 0)
			{
				switch (process(fmt, '4'))
				{
					case '1': put(a); break;
					case '2': put(b); break;
					case '3': put(c); break;
					case '4': put(d); break;
				}
			}
		}
	template<typename A, typename B, typename C, typename D, typename E>
		OW_Format(const char* ca, const A& a, const B& b, const C& c, const D& d, const E& e) : oss()
		{
			OW_String fmt(ca);
			while (fmt.length() > 0)
			{
				switch (process(fmt, '5'))
				{
					case '1': put(a); break;
					case '2': put(b); break;
					case '3': put(c); break;
					case '4': put(d); break;
					case '5': put(e); break;
				}
			}
		}
	template<typename A, typename B, typename C, typename D, typename E, typename F>
		OW_Format(const char* ca, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f) : oss()
		{
			OW_String fmt(ca);
			while (fmt.length() > 0)
			{
				switch (process(fmt, '6'))
				{
					case '1': put(a); break;
					case '2': put(b); break;
					case '3': put(c); break;
					case '4': put(d); break;
					case '5': put(e); break;
					case '6': put(f); break;
				}
			}
		}
	template<typename A, typename B, typename C, typename D, typename E, typename F,
	typename G>
		OW_Format(const char* ca, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g) : oss()
		{
			OW_String fmt(ca);
			while (fmt.length() > 0)
			{
				switch (process(fmt, '7'))
				{
					case '1': put(a); break;
					case '2': put(b); break;
					case '3': put(c); break;
					case '4': put(d); break;
					case '5': put(e); break;
					case '6': put(f); break;
					case '7': put(g); break;
				}
			}
		}
	template<typename A, typename B, typename C, typename D, typename E, typename F,
	typename G, typename H>
		OW_Format(const char* ca, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h) : oss()
		{
			OW_String fmt(ca);
			while (fmt.length() > 0)
			{
				switch (process(fmt, '8'))
				{
					case '1': put(a); break;
					case '2': put(b); break;
					case '3': put(c); break;
					case '4': put(d); break;
					case '5': put(e); break;
					case '6': put(f); break;
					case '7': put(g); break;
					case '8': put(h); break;
				}
			}
		}
	template<typename A, typename B, typename C, typename D, typename E, typename F,
	typename G, typename H, typename I>
		OW_Format(const char* ca, const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h, const I& i) : oss()
		{
			OW_String fmt(ca);
			while (fmt.length() > 0)
			{
				switch (process(fmt, '9'))
				{
					case '1': put(a); break;
					case '2': put(b); break;
					case '3': put(c); break;
					case '4': put(d); break;
					case '5': put(e); break;
					case '6': put(f); break;
					case '7': put(g); break;
					case '8': put(h); break;
					case '9': put(i); break;
				}
			}
		}

}; // class OW_Format


typedef OW_Format format;


#endif
// end of OW_Format.hpp ---------------------------------------------------------//

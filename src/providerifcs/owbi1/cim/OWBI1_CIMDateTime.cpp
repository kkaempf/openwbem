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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OWBI1_config.h"
#include "OWBI1_CIMDateTime.hpp"
#include "OWBI1_CIMDateTimeRep.hpp"
#include "OW_ExceptionIds.hpp"
#include "OWBI1_DateTime.hpp"
#include "OWBI1_DateTimeRep.hpp"

#include <cstdio>
#if defined(OWBI1_HAVE_ISTREAM) && defined(OWBI1_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

namespace OWBI1
{
using std::ostream;
using std::istream;
using namespace detail;

OWBI1_DEFINE_EXCEPTION_WITH_ID(CIMDateTime);

//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime()
	: m_rep(new CIMDateTimeRep(OpenWBEM::CIMDateTime()))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::~CIMDateTime()
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime(CIMNULL_t)
	: m_rep(new CIMDateTimeRep(OpenWBEM::CIMDateTime(OpenWBEM::CIMNULL)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime(const CIMDateTime& arg)
	: m_rep(arg.m_rep)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime(const String& arg) :
	m_rep(new CIMDateTimeRep(OpenWBEM::CIMDateTime(arg.c_str())))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime(const DateTime& arg) :
	m_rep(new CIMDateTimeRep(OpenWBEM::CIMDateTime(arg.getRep()->datetime)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime(UInt64 microSeconds) :
	m_rep(new CIMDateTimeRep(OpenWBEM::CIMDateTime(microSeconds)))
{
}

//////////////////////////////////////////////////////////////////////////////
CIMDateTime::CIMDateTime(const CIMDateTimeRepRef& rep)
	: m_rep(rep)
{
}

//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::operator= (const CIMDateTime& arg)
{
	m_rep = arg.m_rep;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime::operator CIMDateTime::safe_bool() const
{
	return (m_rep->datetime) ?
		&CIMDateTime::m_rep : 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDateTime::operator !() const
{
	return (!m_rep->datetime);
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setYear(UInt16 arg)
{
	m_rep->datetime.setYear(arg);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setMonth(UInt8 arg)
{
	m_rep->datetime.setMonth(arg);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setDays(UInt32 arg)
{
	m_rep->datetime.setDays(arg);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setDay(UInt32 arg)
{
	m_rep->datetime.setDay(arg);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setHours(UInt8 arg)
{
	m_rep->datetime.setHours(arg);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setMinutes(UInt8 arg)
{
	m_rep->datetime.setMinutes(arg);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setSeconds(UInt8 arg)
{
	m_rep->datetime.setSeconds(arg);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setMicroSeconds(UInt32 arg)
{
	m_rep->datetime.setMicroSeconds(arg);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMDateTime&
CIMDateTime::setUtc(Int16 arg)
{
	m_rep->datetime.setUtc(arg);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDateTime::equal(const CIMDateTime& arg) const
{
	return m_rep->datetime.equal(arg.m_rep->datetime);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMDateTime::readObject(istream &istrm)
{
	m_rep->datetime.readObject(istrm);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMDateTime::writeObject(ostream &ostrm) const
{
	m_rep->datetime.writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMDateTime::toString() const
{
	return m_rep->datetime.toString().c_str();
}
//////////////////////////////////////////////////////////////////////////////
DateTime
CIMDateTime::toDateTime() const
{
	return DateTime(toString());
}
//////////////////////////////////////////////////////////////////////////////
ostream&
operator<< (ostream& ostr, const CIMDateTime& arg)
{
	ostr << arg.toString();
	return ostr;
}

//////////////////////////////////////////////////////////////////////////////
UInt16
CIMDateTime::getYear() const {  return m_rep->datetime.getYear(); }
//////////////////////////////////////////////////////////////////////////////
UInt8
CIMDateTime::getMonth() const {  return m_rep->datetime.getMonth(); }
//////////////////////////////////////////////////////////////////////////////
UInt32
CIMDateTime::getDays() const {  return m_rep->datetime.getDays(); }
//////////////////////////////////////////////////////////////////////////////
UInt32
CIMDateTime::getDay() const {  return m_rep->datetime.getDay(); }
//////////////////////////////////////////////////////////////////////////////
UInt8
CIMDateTime::getHours() const {  return m_rep->datetime.getHours(); }
//////////////////////////////////////////////////////////////////////////////
UInt8
CIMDateTime::getMinutes() const {  return m_rep->datetime.getMinutes(); }
//////////////////////////////////////////////////////////////////////////////
UInt8
CIMDateTime::getSeconds() const {  return m_rep->datetime.getSeconds(); }
//////////////////////////////////////////////////////////////////////////////
UInt32
CIMDateTime::getMicroSeconds() const {  return m_rep->datetime.getMicroSeconds(); }
//////////////////////////////////////////////////////////////////////////////
Int16
CIMDateTime::getUtc() const {  return m_rep->datetime.getUtc(); }
//////////////////////////////////////////////////////////////////////////////
bool
CIMDateTime::isInterval() const {  return m_rep->datetime.isInterval();}
//////////////////////////////////////////////////////////////////////////////
void
CIMDateTime::setInterval(bool val) { m_rep->datetime.setInterval(val); }

//////////////////////////////////////////////////////////////////////////////
CIMDateTimeRepRef
CIMDateTime::getRep() const
{
	return m_rep;
}

//////////////////////////////////////////////////////////////////////////////
bool operator==(const CIMDateTime& x, const CIMDateTime& y)
{
	return x.equal(y);
}
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMDateTime& x, const CIMDateTime& y)
{
	return x.m_rep->datetime < y.m_rep->datetime;
}

//////////////////////////////////////////////////////////////////////////////
bool operator!=(const CIMDateTime& x, const CIMDateTime& y)
{
	return !(x == y);
}

//////////////////////////////////////////////////////////////////////////////
bool operator>(const CIMDateTime& x, const CIMDateTime& y)
{
	return y < x;
}

//////////////////////////////////////////////////////////////////////////////
bool operator<=(const CIMDateTime& x, const CIMDateTime& y)
{
	return !(y < x);
}

//////////////////////////////////////////////////////////////////////////////
bool operator>=(const CIMDateTime& x, const CIMDateTime& y)
{
	return !(x < y);
}

} // end namespace OWBI1


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
#ifndef __OW_CPI_HPP__
#define __OW_CPI_HPP__

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_CIM.hpp"
#include "OW_Exception.hpp"
#include "owcpiprov.h"

#define CPI_BASE_HDL_SIG	1000
#define CPI_VECT_SIG		(CPI_BASE_HDL_SIG+1)
#define CPI_CLASS_SIG		(CPI_BASE_HDL_SIG+2)
#define CPI_INSTANCE_SIG	(CPI_BASE_HDL_SIG+3)
#define CPI_METHOD_SIG		(CPI_BASE_HDL_SIG+4)
#define CPI_NS_SIG			(CPI_BASE_HDL_SIG+5)
#define CPI_OBJPATH_SIG		(CPI_BASE_HDL_SIG+6)
#define CPI_PARM_SIG		(CPI_BASE_HDL_SIG+7)
#define CPI_PROP_SIG		(CPI_BASE_HDL_SIG+8)
#define CPI_QUAL_SIG		(CPI_BASE_HDL_SIG+9)
#define CPI_QUALTYPE_SIG	(CPI_BASE_HDL_SIG+10)
#define CPI_VALUE_SIG		(CPI_BASE_HDL_SIG+11)
#define CPI_DATETIME_SIG	(CPI_BASE_HDL_SIG+12)
#define CPI_BOOL_SIG		(CPI_BASE_HDL_SIG+13)
#define CPI_UINT8_SIG		(CPI_BASE_HDL_SIG+14)
#define CPI_SINT8_SIG		(CPI_BASE_HDL_SIG+15)
#define CPI_UINT16_SIG		(CPI_BASE_HDL_SIG+16)
#define CPI_SINT16_SIG		(CPI_BASE_HDL_SIG+17)
#define CPI_UINT32_SIG		(CPI_BASE_HDL_SIG+18)
#define CPI_SINT32_SIG		(CPI_BASE_HDL_SIG+19)
#define CPI_UINT64_SIG		(CPI_BASE_HDL_SIG+20)
#define CPI_SINT64_SIG		(CPI_BASE_HDL_SIG+21)
#define CPI_REAL32_SIG		(CPI_BASE_HDL_SIG+22)
#define CPI_REAL64_SIG		(CPI_BASE_HDL_SIG+23)
#define CPI_CHAR16_SIG		(CPI_BASE_HDL_SIG+24)
#define CPI_STRING_SIG		(CPI_BASE_HDL_SIG+25)
#define CPI_OBJECT_SIG		(CPI_BASE_HDL_SIG+26)

struct CPI_VectorHandle
{
	CPI_VectorHandle()
		: m_sig(CPI_VECT_SIG)
		, m_type(0)
		, m_obj(0)
	{
	}

	CPI_VectorHandle(unsigned int type)
		: m_sig(CPI_VECT_SIG)
		, m_type(type)
		, m_obj(0)
	{
	}

	unsigned int m_sig;		// CPI_VECT_SIG
	unsigned int m_type;
	void* m_obj;
};

struct CPI_ObjHandleBase
{
	CPI_ObjHandleBase() : m_sig(0) {}
	CPI_ObjHandleBase(unsigned int sig) : m_sig(sig) {}
	virtual ~CPI_ObjHandleBase() {}
	virtual void deleteObj() = 0;
	unsigned int m_sig;
};

template <class T, unsigned int SIG>
struct CPI_ObjHandle : public CPI_ObjHandleBase
{
	CPI_ObjHandle()
		: CPI_ObjHandleBase(SIG)
		, m_pObj(0)
	{
	}

	CPI_ObjHandle(T* p)
		: CPI_ObjHandleBase(SIG)
		, m_pObj(p)
	{
	}

	CPI_ObjHandle(const CPI_ObjHandle<T, SIG>& arg)
		: CPI_ObjHandleBase(SIG)
		, m_pObj(arg.m_pObj)
	{
	}

	T* operator->() const { return m_pObj; }
	T& operator*() const { return *m_pObj; }
	virtual void deleteObj() { delete m_pObj; m_sig = 0; }

	T* m_pObj;
};

typedef CPI_ObjHandle<OW_CIMClass, CPI_CLASS_SIG> CPI_ClassHandle;
typedef CPI_ObjHandle<OW_CIMInstance, CPI_INSTANCE_SIG> CPI_InstanceHandle;
typedef CPI_ObjHandle<OW_CIMMethod, CPI_METHOD_SIG> CPI_MethodHandle;
typedef CPI_ObjHandle<OW_CIMNameSpace, CPI_NS_SIG> CPI_NameSpaceHandle;
typedef CPI_ObjHandle<OW_CIMObjectPath, CPI_OBJPATH_SIG> CPI_ObjectPathHandle;
typedef CPI_ObjHandle<OW_CIMParameter, CPI_PARM_SIG> CPI_ParameterHandle;
typedef CPI_ObjHandle<OW_CIMProperty, CPI_PROP_SIG> CPI_PropertyHandle;
typedef CPI_ObjHandle<OW_CIMQualifier, CPI_QUAL_SIG> CPI_QualifierHandle;
typedef CPI_ObjHandle<OW_CIMQualifierType, CPI_QUALTYPE_SIG> CPI_QualTypeHandle;
typedef CPI_ObjHandle<OW_CIMValue, CPI_VALUE_SIG> CPI_ValueHandle;
typedef CPI_ObjHandle<OW_CIMDateTime, CPI_DATETIME_SIG> CPI_DateTimeHandle;
typedef OW_Array<CPI_ObjHandleBase*> CPI_ObjVector;

class OW_CPI
{
public:
	static bool validObjectSig(unsigned int sig);
	static CPI_VectorHandle* castVectPtr(void* p);
	static CPI_ObjHandleBase* castObjPtr(void* p);
	static CPI_ClassHandle* castClassPtr(void* p);
	static CPI_InstanceHandle* castInstancePtr(void* p);
	static CPI_QualifierHandle* castQualifierPtr(void* p);
	static CPI_PropertyHandle* castPropertyPtr(void* p);
	static CPI_MethodHandle* castMethodPtr(void* p);
	static CPI_ParameterHandle* castParmPtr(void* p);
	static CPI_NameSpaceHandle* castNSPtr(void* p);


	static int objHdlToQual(cpiObjHdl ohdl, OW_CIMQualifier& qual);
	static int qualToObjHdl(const OW_CIMQualifier& qual, cpiObjHdl& ohdl);
	static int vectToQuals(cpiVectHdl vect, OW_CIMQualifierArray& qra);
	static int qualsToVect(const OW_CIMQualifierArray& qra, cpiVectHdl& vhdl);

	static int objHdlToProp(cpiObjHdl ohdl, OW_CIMProperty& prop);
	static int propToObjHdl(const OW_CIMProperty& prop, cpiObjHdl& ohdl);
	static int vectToProps(cpiVectHdl vect, OW_CIMPropertyArray& pra);
	static int propsToVect(const OW_CIMPropertyArray& pra, cpiVectHdl& vhdl);

	static int objHdlToMeth(cpiObjHdl ohdl, OW_CIMMethod& meth);
	static int methToObjHdl(const OW_CIMMethod& meth, cpiObjHdl& ohdl);
	static int vectToMeths(cpiVectHdl vect, OW_CIMMethodArray& mra);
	static int methsToVect(const OW_CIMMethodArray& mra, cpiVectHdl& vhdl);

	static int objHdlToParm(cpiObjHdl ohdl, OW_CIMParameter& parm);
	static int parmToObjHdl(const OW_CIMParameter& parm, cpiObjHdl& ohdl);
	static int vectToParms(cpiVectHdl vect, OW_CIMParameterArray& pra);
	static int parmsToVect(const OW_CIMParameterArray& pra, cpiVectHdl& vhdl);

};

int cpiVectorAlloc(cpiVectHdl* pVect, unsigned int type);
unsigned int cpiVectorType(cpiVectHdl vect);

#endif	// __OW_CPI_HPP__


/*******************************************************************************
* Copyright (C) 2004 Novell, Inc All rights reserved.
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
*  - Neither the name of Novell nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc OR THE CONTRIBUTORS
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
 */


#include "OW_config.h"
#include "OWBI1_SimpleAssociatorProviderIFC.hpp"
#include "OWBI1_CIMInstance.hpp"
#include "OWBI1_CIMClass.hpp"
#include "OWBI1_CIMObjectPath.hpp"
#include "OWBI1_CIMProperty.hpp"
#include "OWBI1_CIMValue.hpp"
#include "OWBI1_CIMNameSpace.hpp"
#include "OWBI1_ResultHandlerIFC.hpp"
#include "OWBI1_CIMOMHandleIFC.hpp"
#include "OWBI1_ProviderEnvironmentIFC.hpp"
#include "OWBI1_CIMPropertyList.hpp"


namespace OWBI1
{
namespace
{
	class AssocHelperResultHandlerIFC : public CIMInstanceResultHandlerIFC
	{
	public:
		AssocHelperResultHandlerIFC(const CIMObjectPath& objectName,
			const CIMName& resultClass, const CIMName& role,
			const CIMName& resultRole)
		: _objectName(objectName)
		, _resultClass(resultClass)
		, _role(role)
		, _resultRole(resultRole)
		{
		}
		void doHandle(const CIMInstance& inst)
		{
			if (_resultRole != "")
			{
				CIMProperty prop = inst.getProperty(_resultRole);

				if (prop)
				{
					CIMObjectPath cop;
					prop.getValue().get(cop);
					assocAuxHandler(cop);
				}
			}
			else
			{
				CIMPropertyArray cpa = inst.getProperties();
				for (CIMPropertyArray::const_iterator iter = cpa.begin();
					iter < cpa.end(); ++iter)
				{
					CIMDataType dt = iter->getDataType();
					if (!dt.isReferenceType())
					{
						continue;
					}
					CIMName propName = iter->getName();
					if (_role == propName)
					{
						continue;
					}
					CIMObjectPath cop;
					iter->getValue().get(cop);
					if (cop != _objectName)
					{
						assocAuxHandler(cop);
					}
				}
			}
		}
	protected:
		virtual void assocAuxHandler(const CIMObjectPath& cop) = 0;
	private:
		CIMObjectPath _objectName;
		CIMName _resultClass;
		CIMName _role;
		CIMName _resultRole;
	};


	////////////////////////////////////////////////////////////////////////////
	class _RHAssociators : public AssocHelperResultHandlerIFC
	{
	public:
		_RHAssociators(CIMInstanceResultHandlerIFC& result,
			const CIMObjectPath& objectName,
			const CIMName& resultClass,
			const CIMName& role,
			const CIMName& resultRole,
			const CIMOMHandleIFCRef& lch,
			const CIMPropertyList& propertyList)
		: AssocHelperResultHandlerIFC(objectName,resultClass ,role ,resultRole)
		, _realHandler(result)
		, _lch(lch)
		, _propertyList(propertyList)
		{
		}
	protected:
		virtual void assocAuxHandler(const CIMObjectPath &cop)
		{
			CIMInstance inst = _lch->getInstance(cop.getFullNameSpace().getNameSpace(),
				cop, _propertyList);
			_realHandler.handle(inst);
		}
	private:
		CIMInstanceResultHandlerIFC& _realHandler;
		const CIMOMHandleIFCRef& _lch;
		const CIMPropertyList _propertyList;
	};

	////////////////////////////////////////////////////////////////////////////
	class _RHAssociatorNames : public AssocHelperResultHandlerIFC
	{
	public:
		_RHAssociatorNames(CIMObjectPathResultHandlerIFC& result,
			const CIMObjectPath& objectName,
			const CIMName& resultClass,
			const CIMName& role,
			const CIMName& resultRole)
		: AssocHelperResultHandlerIFC(objectName,resultClass ,role ,resultRole)
		, _realHandler(result)
		{
		}
	protected:
		virtual void assocAuxHandler(const CIMObjectPath &cop)
		{
			_realHandler.handle(cop);
		}
	private:
		CIMObjectPathResultHandlerIFC& _realHandler;
	};

	////////////////////////////////////////////////////////////////////////////
	class _RHReferences : public CIMInstanceResultHandlerIFC
	{
	public:
		_RHReferences(CIMInstanceResultHandlerIFC& realHandler,
			const CIMPropertyList& propertyList)
		: _realHandler(realHandler)
		, _propertyList(propertyList)
		{
		}

		void doHandle(const CIMInstance& inst)
		{
			CIMInstance rval = inst.clone(_propertyList);
			_realHandler.handle(rval);
		}
	private:
		CIMInstanceResultHandlerIFC& _realHandler;
		const CIMPropertyList _propertyList;
	};

	//////////////////////////////////////////////////////////////////////////
	class _RHReferenceNames : public CIMInstanceResultHandlerIFC
	{
	public:
		_RHReferenceNames(CIMObjectPathResultHandlerIFC& realHandler, const String& ns)
		: _realHandler(realHandler)
		, _ns(ns)
		{
		}
		void doHandle(const CIMInstance& inst)
		{
			CIMObjectPath cop(_ns, inst);
			_realHandler.handle(cop);
		}
	private:
		CIMObjectPathResultHandlerIFC& _realHandler;
		String _ns;
	};

} // end anonymous namespace

void
BI1SimpleAssociatorProviderIFC::associators(const ProviderEnvironmentIFCRef &env,
	CIMInstanceResultHandlerIFC &result,
	const String &ns,
	const CIMObjectPath &objectName,
	const CIMName &assocClass,
	const CIMName &resultClass,
	const CIMName &role,
	const CIMName &resultRole,
	const CIMPropertyList& propertyList)
{

	CIMOMHandleIFCRef lch = env->getCIMOMHandle();
	CIMClass theAssocClass = lch->getClass(ns, assocClass);
	_RHAssociators rh(result, objectName, resultClass, role, resultRole,
		lch, propertyList);

	doReferences(env, rh, ns, objectName, theAssocClass, resultClass,
		role, resultRole);
}

void
BI1SimpleAssociatorProviderIFC::associatorNames(const ProviderEnvironmentIFCRef &env,
	CIMObjectPathResultHandlerIFC &result,
	const String &ns,
	const CIMObjectPath &objectName,
	const CIMName &assocClass,
	const CIMName &resultClass,
	const CIMName &role,
	const CIMName &resultRole)
{


	_RHAssociatorNames rh(result, objectName, resultClass, role, resultRole);

	CIMOMHandleIFCRef lch = env->getCIMOMHandle();
	CIMClass theAssocClass = lch->getClass(ns, assocClass);

	doReferences(env, rh, ns, objectName, theAssocClass, resultClass,
		role, resultRole);
}

void
BI1SimpleAssociatorProviderIFC::references(const ProviderEnvironmentIFCRef
	&env, CIMInstanceResultHandlerIFC &result,
	const String &ns,
	const CIMObjectPath &objectName,
	const CIMName &resultClass,
	const CIMName &role,
	const CIMPropertyList& propertyList)
{

	CIMOMHandleIFCRef lch = env->getCIMOMHandle();
	CIMClass theAssocClass = lch->getClass(ns, resultClass);

	_RHReferences rh(result, propertyList );
	doReferences(env, rh, ns, objectName, theAssocClass, "", role, "");
}
/**
* For definition of this operation, refer to
*  ttp://www.dmtf.org/download/spec/xmls/CIM_HTTP_Mapping10.htm#SecReferenceNames
* This operation is used to enumerate the association objects
* that refer to a particular target CIM Instance.
*
* @param resultClass see the reference method.
* @param objectName see the reference method.
* @param role see the reference method
*
* @returns If successful, the method returns the names of zero or more
* full CIM Instance paths of Objects meeting the requested criteria.
* @throws CIMException - as defined for associators method.
*/
void
BI1SimpleAssociatorProviderIFC::referenceNames(const ProviderEnvironmentIFCRef &env,
	CIMObjectPathResultHandlerIFC &result,
	const String &ns,
	const CIMObjectPath &objectName,
	const CIMName &resultClass,
	const CIMName &role)
{

	CIMOMHandleIFCRef lch = env->getCIMOMHandle();
	CIMClass theAssocClass = lch->getClass(ns, resultClass);
	_RHReferenceNames rh(result,ns);
	doReferences(env, rh,ns ,objectName , theAssocClass,"",role, "");
}

}

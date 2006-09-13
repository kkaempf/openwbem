/*
 *
 * cmpiBroker.cpp
 *
 * (C) Copyright IBM Corp. 2003
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author:        Adrian Schuur <schuur@de.ibm.com>
 *
 * Contributor:   Markus Mueller <sedgewick_de@yahoo.de>
 *
 * Description: CMPIBroker up-call support
 *
 */

#include <iostream>
#include "cmpisrv.h"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_ResultHandlers.hpp"
#include "OW_String.hpp"
#include "OW_Logger.hpp"
#include "OW_CIMOMHandleIFC.hpp"

using namespace OpenWBEM::WBEMFlags;
using OpenWBEM::Format;
//extern int traceAdapter;

namespace
{
	const OpenWBEM::String COMPONENT_NAME("ow.provider.cmpi.ifc");
}

#define CM_CIMOM() \
(* static_cast<OpenWBEM::ProviderEnvironmentIFCRef *>(CMPI_ThreadContext::getBroker()->hdl))->getCIMOMHandle()
#define CM_LOGGER() \
(* static_cast<OpenWBEM::ProviderEnvironmentIFCRef *>(CMPI_ThreadContext::getBroker()->hdl))->getLogger(COMPONENT_NAME)
#define CM_Context(ctx) (((CMPI_Context*)ctx)->ctx)
#define CM_Instance(ci) ((OpenWBEM::CIMInstance*)ci->hdl)
#define CM_ObjectPath(cop) ((OpenWBEM::CIMObjectPath*)cop->hdl)
#define CM_Args(args) ((OpenWBEM::CIMParamValueArray*)args->hdl)
#define CM_LocalOnly(flgs) (((flgs) & CMPI_FLAG_LocalOnly)!=0)
#define CM_ClassOrigin(flgs) \
(((flgs) & CMPI_FLAG_IncludeClassOrigin)!=0)
#define CM_IncludeQualifiers(flgs) \
(((flgs) & CMPI_FLAG_IncludeQualifiers) !=0)
#define CM_DeepInheritance(flgs) (((flgs) & CMPI_FLAG_DeepInheritance)!=0)


#define DDD(X) X

//////////////////////////////////////////////////////////////////////////////
// Convert a an array of null terminated strings to an OpenWBEM::StringArray
// It is assumed the 'l' parm is a pointer to a NULL terminated array of
// C strings.
//
OpenWBEM::StringArray* getList(const char** l, OpenWBEM::StringArray& sra)
{
	OpenWBEM::StringArray *pRa = NULL;
	sra.clear();
	if(l)
	{
		for(int i = 0; l[i]; i++)
		{
			sra.append(l[i]);
		}

		pRa = &sra;
	}

	return pRa;
}


OpenWBEM::CIMClass* mbGetClass(const CMPIBroker *, const OpenWBEM::CIMObjectPath &cop)
{

	OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbGetClass()");

	OpenWBEM::String clsId = cop.getNameSpace()+":"+cop.getClassName();
	OpenWBEM::CIMClass ccp;

	try
	{
		OpenWBEM::CIMClass cc=CM_CIMOM()->getClass(
			cop.getNameSpace(),
			cop.getClassName(),
			E_NOT_LOCAL_ONLY,
			E_INCLUDE_QUALIFIERS,
			E_EXCLUDE_CLASS_ORIGIN);

		return new OpenWBEM::CIMClass(cc);
	}
	catch(const OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mbGetClass code: %1, msg %2",
			e.type(), e.getMessage()));
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbGetClass");
	}

	return NULL;
}

static CMPIInstance* mbGetInstance(const CMPIBroker *, const CMPIContext *ctx,
	const CMPIObjectPath *cop, const char **properties, CMPIStatus *rc)
{
	OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbGetInstance()");

	CMPIFlags flgs =
		ctx->ft->getEntry(ctx,const_cast<char*>(CMPIInvocationFlags),NULL).value.uint32;

	OpenWBEM::StringArray props;
	const OpenWBEM::StringArray *pProps = getList(properties, props);

	OpenWBEM::CIMObjectPath qop(*CM_ObjectPath(cop));

	try
	{
		OpenWBEM::CIMInstance ci = CM_CIMOM()->getInstance(
			CM_ObjectPath(cop)->getNameSpace(),
			qop,
			CM_LocalOnly(flgs) ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY,
			CM_IncludeQualifiers(flgs) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			CM_ClassOrigin(flgs) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
			pProps
			);

		CMSetStatus(rc,CMPI_RC_OK);

		return (CMPIInstance*) new CMPI_Object(new OpenWBEM::CIMInstance(ci));
	}
	catch(const OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mbGetInstance code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbGetInstance");
		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}

	return NULL;
}

static CMPIObjectPath* mbCreateInstance(const CMPIBroker *, const CMPIContext *ctx,
	const CMPIObjectPath *cop, const CMPIInstance *ci, CMPIStatus *rc)
{
	(void) ctx;

	CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
	OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbCreateInstance()");

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	try
	{
		OpenWBEM::CIMObjectPath ncop=CM_CIMOM()->createInstance(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_Instance(ci));

		CMSetStatus(rc,CMPI_RC_OK);

		return (CMPIObjectPath*)
			new CMPI_Object(new OpenWBEM::CIMObjectPath(ncop));
	}
	catch(const OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mCreateInstance code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbCreateInstance");

		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}

#endif

	return NULL;
}

static CMPIStatus mbModifyInstance(const CMPIBroker *, const CMPIContext *ctx,
	const CMPIObjectPath *cop, const CMPIInstance *ci, const char**)
{
	// TODO handle propertylist
	OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbSetInstance()");

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	CMPIFlags flgs =
		ctx->ft->getEntry(ctx,const_cast<char*>(CMPIInvocationFlags),NULL).value.uint32;
	OpenWBEM::StringArray sProps;
	try
	{

		CM_CIMOM()->modifyInstance(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_Instance(ci),
			CM_IncludeQualifiers(flgs) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			&sProps);
		CMReturn(CMPI_RC_OK);
	}
	catch(OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mbSetInstance code: %1, msg %2",
			e.type(), e.getMessage()));
		CMReturn((CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbSetInstance");
		CMReturn(CMPI_RC_ERROR_SYSTEM);
	}
#endif

	CMReturn(CMPI_RC_ERR_FAILED);
}

static CMPIStatus mbDeleteInstance (const CMPIBroker *, const CMPIContext *ctx,
	const CMPIObjectPath *cop)
{
	(void) ctx;

	OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbDeleteInstance()");

#ifndef OW_DISABLE_INSTANCE_MANIPULATION

	OpenWBEM::CIMObjectPath qop(*CM_ObjectPath(cop));

	try
	{
		CM_CIMOM()->deleteInstance(
			CM_ObjectPath(cop)->getNameSpace(),
			qop);

		CMReturn(CMPI_RC_OK);
	}
	catch(OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mbDeleteInstance code: %1, msg %2",
			e.type(), e.getMessage()));
		CMReturn((CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbDeleteInstance");
		CMReturn(CMPI_RC_ERROR_SYSTEM);
	}

#endif

	CMReturn(CMPI_RC_ERROR);
}

static CMPIEnumeration* mbExecQuery(const CMPIBroker *, const CMPIContext *ctx,
	const CMPIObjectPath *cop, const char *query, const char *lang, CMPIStatus *rc)
{
	(void) ctx;
	OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbExecQuery()");

	try
	{
		OpenWBEM::CIMInstanceArray cia;
		OpenWBEM::CIMInstanceArrayBuilder result(cia);

		CM_CIMOM()->execQuery(
			CM_ObjectPath(cop)->getNameSpace(),
			result,
			OpenWBEM::String(query),
			OpenWBEM::String(lang));

		CMSetStatus(rc,CMPI_RC_OK);

		return new CMPI_ObjEnumeration(
			new OpenWBEM::Array<OpenWBEM::CIMInstance>(cia));
	}
	catch(OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mbExecQuery code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbExecQuery");

		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}
	return NULL;
}

static CMPIEnumeration* mbEnumInstances(const CMPIBroker *, const CMPIContext *ctx,
	const CMPIObjectPath *cop, const char **properties, CMPIStatus *rc)
{
   OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbEnumInstances()");
	CMPIFlags flgs = ctx->ft->getEntry(
		ctx,const_cast<char*>(CMPIInvocationFlags),NULL).value.uint32;

	OpenWBEM::StringArray props;
	OpenWBEM::StringArray *pProps = getList(properties, props);

	OpenWBEM::CIMInstanceArray cia;
	OpenWBEM::CIMInstanceArrayBuilder result(cia);

	try
	{
		CM_CIMOM()->enumInstances(
			CM_ObjectPath(cop)->getNameSpace(),
			CM_ObjectPath(cop)->getClassName(),
			result,
			CM_DeepInheritance(flgs) ? E_DEEP : E_SHALLOW,
			CM_LocalOnly(flgs) ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY,
			CM_IncludeQualifiers(flgs) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			CM_ClassOrigin(flgs) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
			pProps
			);
		CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_InstEnumeration( new OpenWBEM::CIMInstanceArray(cia));
	}
	catch(OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mbEnumInstances code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
		rc = NULL;	// Ensure rc doesn't get changed later
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbEnumInstances");
		CMSetStatus(rc, CMPI_RC_ERROR_SYSTEM);
		rc = NULL;	// Ensure rc doesn't get changed later
	}

	CMSetStatus(rc, CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbEnumInstanceNames(const CMPIBroker *, const CMPIContext *ctx,
	const CMPIObjectPath *cop, CMPIStatus *rc)
{
	(void) ctx;

	OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbEnumInstanceNames()");

	OpenWBEM::CIMObjectPathArray cia;
	OpenWBEM::CIMObjectPathArrayBuilder result(cia);

	try
	{
		CM_CIMOM()->enumInstanceNames(
			CM_ObjectPath(cop)->getNameSpace(),
			CM_ObjectPath(cop)->getClassName(),
			result);

		CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_OpEnumeration( new OpenWBEM::CIMObjectPathArray(cia));
	}
	catch(OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mbEnumInstanceNames code: %1, msg %2",
			e.type(), e.getMessage()));
		CMSetStatus(rc,(CMPIrc)e.getErrNo());
		rc = NULL;	// Ensure rc doesn't get changed later
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbEnumInstanceNames");
		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
		rc = NULL;	// Ensure rc doesn't get changed later
	}

	CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbAssociators(const CMPIBroker *, const CMPIContext *ctx,
	const CMPIObjectPath *cop, const char *assocClass, const char *resultClass,
	const char *role, const char *resultRole, const char **properties, CMPIStatus *rc)
{
	OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbAssociators()");

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

	CMPIFlags flgs = ctx->ft->getEntry(
		ctx,const_cast<char*>(CMPIInvocationFlags),NULL).value.uint32;

	OpenWBEM::StringArray props;
	OpenWBEM::StringArray *pProps = getList(properties, props);

	OpenWBEM::CIMInstanceArray cia;
	OpenWBEM::CIMInstanceArrayBuilder result(cia);

	try
	{
		CM_CIMOM()->associators(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			result,
			OpenWBEM::String(assocClass),
			OpenWBEM::String(resultClass),
			OpenWBEM::String(role),
			OpenWBEM::String(resultRole),
			CM_IncludeQualifiers(flgs) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			CM_ClassOrigin(flgs) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
			pProps);

		CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_ObjEnumeration( new OpenWBEM::CIMInstanceArray(cia));
	}
	catch(OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mbAssociators code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
		rc = NULL;	// Ensure rc doesn't get changed later
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbAssociators");
		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
		rc = NULL;	// Ensure rc doesn't get changed later
	}

#endif

	CMSetStatus(rc, CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbAssociatorNames(const CMPIBroker *, const CMPIContext *ctx,
	const CMPIObjectPath *cop, const char *assocClass, const char *resultClass,
	const char *role, const char *resultRole, CMPIStatus *rc)
{
	(void) ctx;
	OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbAssociatorNames()");

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

	OpenWBEM::CIMObjectPathArray cia;
	OpenWBEM::CIMObjectPathArrayBuilder result(cia);

	try
	{
		CM_CIMOM()->associatorNames(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			result,
			OpenWBEM::String(assocClass),
			OpenWBEM::String(resultClass),
			OpenWBEM::String(role),
			OpenWBEM::String(resultRole));

		CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_OpEnumeration(new OpenWBEM::CIMObjectPathArray(cia));
	}
	catch(OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mbAssociatorNames code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
		rc = NULL;	// Ensure rc doesn't get changed later
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbAssociatorNames");

		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
		rc = NULL;	// Ensure rc doesn't get changed later
	}

#endif

	CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbReferences(const CMPIBroker *, const CMPIContext *ctx,
	const CMPIObjectPath *cop,  const char *resultClass, const char *role ,
	const char **properties, CMPIStatus *rc)
{
	OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbReferences()");

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

	CMPIFlags flgs = ctx->ft->getEntry(
		ctx,const_cast<char*>(CMPIInvocationFlags),NULL).value.uint32;

	OpenWBEM::StringArray props;
	OpenWBEM::StringArray *pProps = getList(properties, props);

	OpenWBEM::CIMInstanceArray cia;
	OpenWBEM::CIMInstanceArrayBuilder result(cia);

	try
	{
		CM_CIMOM()->references(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			result,
			OpenWBEM::String(resultClass),
			OpenWBEM::String(role),
			CM_IncludeQualifiers(flgs) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			CM_ClassOrigin(flgs) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
			pProps);

		CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_ObjEnumeration(new OpenWBEM::CIMInstanceArray(cia));
	}
	catch(OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mbReferences code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
		rc = NULL;	// Ensure rc doesn't get changed later
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbReferences");
		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
		rc = NULL;	// Ensure rc doesn't get changed later
	}


#endif

	CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbReferenceNames(const CMPIBroker *, const CMPIContext *ctx,
	const CMPIObjectPath *cop, const char *resultClass, const char *role,
	CMPIStatus *rc)
{
	(void) ctx;

	OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbReferenceNames()");

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	OpenWBEM::CIMObjectPathArray cia;
	OpenWBEM::CIMObjectPathArrayBuilder result(cia);

	try
	{
		CM_CIMOM()->referenceNames(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			result,
			OpenWBEM::String(resultClass),
			OpenWBEM::String(role));

		CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_OpEnumeration(new OpenWBEM::CIMObjectPathArray(cia));
	}
	catch(OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mbReferenceNames code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
		rc = NULL;	// Ensure rc doesn't get changed later
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbReferenceNames");
		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
		rc = NULL;	// Ensure rc doesn't get changed later
	}

#endif

	CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIData mbInvokeMethod(const CMPIBroker *, const CMPIContext *ctx,
	const CMPIObjectPath *cop, const char *method, const CMPIArgs *in, CMPIArgs *out,
	CMPIStatus *rc)
{
	(void) ctx;

	OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbInvokeMethod()");

	try
	{
		OpenWBEM::CIMValue cv = CM_CIMOM()->invokeMethod(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			OpenWBEM::String(method),
			*CM_Args(in),
			*CM_Args(out));
		CMPIData data={(CMPIType) 0, CMPI_nullValue, {0} };
		OpenWBEM::CIMDataType vType=cv.getType();
		CMPIType t=type2CMPIType(vType,cv.isArray());
		value2CMPIData(cv,t,&data);
		CMSetStatus(rc,CMPI_RC_OK);
		return data;
	}
	catch(OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mbInvokeMethod code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbInvokeMethod");
		CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	}
	CMPIData data2={(CMPIType) 0, CMPI_nullValue, {0} };
	return data2;
}

static CMPIStatus mbSetProperty(const CMPIBroker *, const CMPIContext *ctx,
	const CMPIObjectPath *cop, const char *name, const CMPIValue *val,
	CMPIType type)
{
	(void) ctx;

	OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbSetProperty()");

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)

	CMPIrc rc;
	OpenWBEM::CIMValue v=value2CIMValue(val,type,&rc);

	try
	{
		CM_CIMOM()->setProperty(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			OpenWBEM::String(name),
			v);

		CMReturn(CMPI_RC_OK);
	}
	catch(OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mbSetProperty code: %1, msg %2",
			e.type(), e.getMessage()));
		CMReturn((CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbSetProperty");
		CMReturn(CMPI_RC_ERROR_SYSTEM);
	}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif
	CMReturn(CMPI_RC_ERR_FAILED);
}

static CMPIData mbGetProperty(const CMPIBroker *, const CMPIContext *ctx,
	const CMPIObjectPath *cop,const char *name, CMPIStatus *rc)
{
	(void) ctx;

	OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker: mbGetProperty()");

	CMPIData data={(CMPIType) 0, CMPI_nullValue, {0} };
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	try
	{
		OpenWBEM::CIMValue v = CM_CIMOM()->getProperty(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			OpenWBEM::String(name));

		OpenWBEM::CIMDataType vType=v.getType();
		CMPIType t=type2CMPIType(vType,v.isArray());
		value2CMPIData(v,t,&data);
		CMSetStatus(rc,CMPI_RC_OK);
		return data;
	}
	catch(OpenWBEM::CIMException &e)
	{
		OW_LOG_DEBUG(CM_LOGGER(), Format("CMPIBroker Exception in "
			"mbGetProperty code: %1, msg %2",
			e.type(), e.getMessage()));
		CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		OW_LOG_DEBUG(CM_LOGGER(), "CMPIBroker Exception in mbGetProperty");
		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	return data;
}





static CMPIBrokerFT broker_FT={
	0, // brokerClassification;
	CMPICurrentVersion,
	const_cast<char*>("OpenWBEM"),
	NULL,
	NULL,
	NULL,
	NULL,
	mbEnumInstanceNames,
	mbGetInstance,
	mbCreateInstance,
	mbModifyInstance,
	mbDeleteInstance,
	mbExecQuery,
	mbEnumInstances,
	mbAssociators,
	mbAssociatorNames,
	mbReferences,
	mbReferenceNames,
	mbInvokeMethod,
	mbSetProperty,
	mbGetProperty,
};

CMPIBrokerFT *CMPI_Broker_Ftab=& broker_FT;

//CMPIBroker *CMPI_Broker::staticBroker=NULL;

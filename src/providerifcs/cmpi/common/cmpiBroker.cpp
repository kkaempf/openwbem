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

using namespace OpenWBEM::WBEMFlags;
using OpenWBEM::Format;
//extern int traceAdapter;

#define CM_CIMOM() \
(* static_cast<OpenWBEM::ProviderEnvironmentIFCRef *>(CMPI_ThreadContext::getBroker()->hdl))->getCIMOMHandle()
#define CM_LOGGER() \
(* static_cast<OpenWBEM::ProviderEnvironmentIFCRef *>(CMPI_ThreadContext::getBroker()->hdl))->getLogger()
#define CM_Context(ctx) (((CMPI_Context*)ctx)->ctx)
#define CM_Instance(ci) ((OpenWBEM::CIMInstance*)ci->hdl)
#define CM_ObjectPath(cop) ((OpenWBEM::CIMObjectPath*)cop->hdl)
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
OpenWBEM::StringArray* getList(char** l, OpenWBEM::StringArray& sra)
{
	OpenWBEM::StringArray *pRa = NULL;
	sra.clear();
	if (l)
	{
		for (int i = 0; l[i]; i++)
		{
			sra.append(l[i]);
		}

		pRa = &sra;
	}

	return pRa;
}


OpenWBEM::CIMClass* mbGetClass(CMPIBroker *, const OpenWBEM::CIMObjectPath &cop)
{

	CM_LOGGER()->logDebug("CMPIBroker: mbGetClass()");

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
	catch(OpenWBEM::CIMException &e)
	{
		CM_LOGGER()->logDebug(Format("CMPIBroker Exception in "
			"mbGetClass code: %1, msg %2",
			e.type(), e.getMessage()));
	}
	catch(...)
	{
		CM_LOGGER()->logDebug("CMPIBroker Exception in mbGetClass");
	}

	return NULL;
}

static CMPIInstance* mbGetInstance(CMPIBroker *, CMPIContext *ctx,
	CMPIObjectPath *cop, char **properties, CMPIStatus *rc)
{
	CM_LOGGER()->logDebug("CMPIBroker: mbGetInstance()");

	CMPIFlags flgs =
		ctx->ft->getEntry(ctx,const_cast<char*>(CMPIInvocationFlags),NULL).value.uint32;

	OpenWBEM::StringArray props;
	OpenWBEM::StringArray *pProps = getList(properties, props);

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
	catch(OpenWBEM::CIMException &e)
	{
		CM_LOGGER()->logDebug(Format("CMPIBroker Exception in "
			"mbGetInstance code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		CM_LOGGER()->logDebug("CMPIBroker Exception in mbGetInstance");
		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}

	return NULL;
}

static CMPIObjectPath* mbCreateInstance(CMPIBroker *, CMPIContext *ctx,
	CMPIObjectPath *cop, CMPIInstance *ci, CMPIStatus *rc)
{
	CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
	CM_LOGGER()->logDebug("CMPIBroker: mbCreateInstance()");

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
	catch(OpenWBEM::CIMException &e)
	{
		CM_LOGGER()->logDebug(Format("CMPIBroker Exception in "
			"mCreateInstance code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		CM_LOGGER()->logDebug("CMPIBroker Exception in mbCreateInstance");

		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}

#endif

	return NULL;
}

static CMPIStatus mbSetInstance(CMPIBroker *, CMPIContext *ctx,
	CMPIObjectPath *cop, CMPIInstance *ci)
{
	CM_LOGGER()->logDebug("CMPIBroker: mbSetInstance()");

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
		CM_LOGGER()->logDebug(Format("CMPIBroker Exception in "
			"mbSetInstance code: %1, msg %2",
			e.type(), e.getMessage()));
		CMReturn((CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		CM_LOGGER()->logDebug("CMPIBroker Exception in mbSetInstance");
		CMReturn(CMPI_RC_ERROR_SYSTEM);
	}
#endif

	CMReturn(CMPI_RC_ERR_FAILED);
}

static CMPIStatus mbDeleteInstance (CMPIBroker *, CMPIContext *ctx,
	CMPIObjectPath *cop)
{
	CM_LOGGER()->logDebug("CMPIBroker: mbDeleteInstance()");

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
		CM_LOGGER()->logDebug(Format("CMPIBroker Exception in "
			"mbDeleteInstance code: %1, msg %2",
			e.type(), e.getMessage()));
		CMReturn((CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		CM_LOGGER()->logDebug("CMPIBroker Exception in mbDeleteInstance");
		CMReturn(CMPI_RC_ERROR_SYSTEM);
	}

#endif

	CMReturn(CMPI_RC_ERROR);
}

static CMPIEnumeration* mbExecQuery(CMPIBroker *, CMPIContext *ctx,
	CMPIObjectPath *cop, char *query, char *lang, CMPIStatus *rc)
{
	CM_LOGGER()->logDebug("CMPIBroker: mbExecQuery()");

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
		CM_LOGGER()->logDebug(Format("CMPIBroker Exception in "
			"mbExecQuery code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		CM_LOGGER()->logDebug("CMPIBroker Exception in mbExecQuery");

		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}
	return NULL;
}

static CMPIEnumeration* mbEnumInstances(CMPIBroker *, CMPIContext *ctx,
	CMPIObjectPath *cop, char **properties, CMPIStatus *rc)
{
	CM_LOGGER()->logDebug("CMPIBroker: mbEnumInstances()");

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
		CM_LOGGER()->logDebug(Format("CMPIBroker Exception in "
			"mbEnumInstances code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
		rc = NULL;	// Ensure rc doesn't get changed later
	}
	catch(...)
	{
		CM_LOGGER()->logDebug("CMPIBroker Exception in mbEnumInstances");
		CMSetStatus(rc, CMPI_RC_ERROR_SYSTEM);
		rc = NULL;	// Ensure rc doesn't get changed later
	}

	CMSetStatus(rc, CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbEnumInstanceNames(CMPIBroker *, CMPIContext *ctx,
	CMPIObjectPath *cop, CMPIStatus *rc)
{
	CM_LOGGER()->logDebug("CMPIBroker: mbEnumInstanceNames()");

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
		CM_LOGGER()->logDebug(Format("CMPIBroker Exception in "
			"mbEnumInstanceNames code: %1, msg %2",
			e.type(), e.getMessage()));
		CMSetStatus(rc,(CMPIrc)e.getErrNo());
		rc = NULL;	// Ensure rc doesn't get changed later
	}
	catch(...)
	{
		CM_LOGGER()->logDebug("CMPIBroker Exception in mbEnumInstanceNames");
		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
		rc = NULL;	// Ensure rc doesn't get changed later
	}

	CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbAssociators(CMPIBroker *, CMPIContext *ctx,
	CMPIObjectPath *cop, char *assocClass, char *resultClass,
	char *role, char *resultRole, char **properties, CMPIStatus *rc)
{
	CM_LOGGER()->logDebug("CMPIBroker: mbAssociators()");

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
		CM_LOGGER()->logDebug(Format("CMPIBroker Exception in "
			"mbAssociators code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
		rc = NULL;	// Ensure rc doesn't get changed later
	}
	catch(...)
	{
		CM_LOGGER()->logDebug("CMPIBroker Exception in mbAssociators");
		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
		rc = NULL;	// Ensure rc doesn't get changed later
	}

#endif

	CMSetStatus(rc, CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbAssociatorNames(CMPIBroker *, CMPIContext *ctx,
	CMPIObjectPath *cop, char *assocClass, char *resultClass,
	char *role, char *resultRole, CMPIStatus *rc)
{
	CM_LOGGER()->logDebug("CMPIBroker: mbAssociatorNames()");

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
		CM_LOGGER()->logDebug(Format("CMPIBroker Exception in "
			"mbAssociatorNames code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
		rc = NULL;	// Ensure rc doesn't get changed later
	}
	catch(...)
	{
		CM_LOGGER()->logDebug("CMPIBroker Exception in mbAssociatorNames");

		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
		rc = NULL;	// Ensure rc doesn't get changed later
	}

#endif

	CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbReferences(CMPIBroker *, CMPIContext *ctx,
	CMPIObjectPath *cop,  char *resultClass, char *role ,
	char **properties, CMPIStatus *rc)
{
	CM_LOGGER()->logDebug("CMPIBroker: mbReferences()");

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
		CM_LOGGER()->logDebug(Format("CMPIBroker Exception in "
			"mbReferences code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
		rc = NULL;	// Ensure rc doesn't get changed later
	}
	catch(...)
	{
		CM_LOGGER()->logDebug("CMPIBroker Exception in mbReferences");
		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
		rc = NULL;	// Ensure rc doesn't get changed later
	}


#endif

	CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbReferenceNames(CMPIBroker *, CMPIContext *ctx,
	CMPIObjectPath *cop, char *resultClass, char *role,
	CMPIStatus *rc)
{
	CM_LOGGER()->logDebug("CMPIBroker: mbReferenceNames()");

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
		CM_LOGGER()->logDebug(Format("CMPIBroker Exception in "
			"mbReferenceNames code: %1, msg %2",
			e.type(), e.getMessage()));

		CMSetStatus(rc,(CMPIrc)e.getErrNo());
		rc = NULL;	// Ensure rc doesn't get changed later
	}
	catch(...)
	{
		CM_LOGGER()->logDebug("CMPIBroker Exception in mbReferenceNames");
		CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
		rc = NULL;	// Ensure rc doesn't get changed later
	}

#endif

	CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIData mbInvokeMethod(CMPIBroker *, CMPIContext *ctx,
	CMPIObjectPath *cop, char *method, CMPIArgs *in, CMPIArgs *out,
	CMPIStatus *rc)
{
	CM_LOGGER()->logDebug("CMPIBroker: mbInvokeMethod()");
	CMPIData data={(CMPIType) 0, CMPI_nullValue, {0} };
	CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
	return data;
}

static CMPIStatus mbSetProperty(CMPIBroker *, CMPIContext *ctx,
	CMPIObjectPath *cop, char *name, CMPIValue *val,
	CMPIType type)
{
	CM_LOGGER()->logDebug("CMPIBroker: mbSetProperty()");

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
		CM_LOGGER()->logDebug(Format("CMPIBroker Exception in "
			"mbSetProperty code: %1, msg %2",
			e.type(), e.getMessage()));
		CMReturn((CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		CM_LOGGER()->logDebug("CMPIBroker Exception in mbSetProperty");
		CMReturn(CMPI_RC_ERROR_SYSTEM);
	}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif
	CMReturn(CMPI_RC_ERR_FAILED);
}

static CMPIData mbGetProperty(CMPIBroker *, CMPIContext *ctx,
	CMPIObjectPath *cop,char *name, CMPIStatus *rc)
{
	CM_LOGGER()->logDebug("CMPIBroker: mbGetProperty()");

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
		CM_LOGGER()->logDebug(Format("CMPIBroker Exception in "
			"mbGetProperty code: %1, msg %2",
			e.type(), e.getMessage()));
		CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch(...)
	{
		CM_LOGGER()->logDebug("CMPIBroker Exception in mbGetProperty");
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
	mbSetInstance,
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

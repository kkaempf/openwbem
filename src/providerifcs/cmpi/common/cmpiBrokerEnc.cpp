/*
 *
 * cmpiBrokerEnc.cpp
 *
 * Copyright (c) 2002, International Business Machines
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
 * Description: CMPIBroker Encapsulated type factory support
 *
 */

#include "cmpisrv.h"
#include "OW_CIMException.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"

#define CM_LOGGER(mb) \
(* static_cast<OpenWBEM::ProviderEnvironmentIFCRef *>(mb->hdl))->getLogger()

// Factory section

static CMPIInstance* mbEncNewInstance(CMPIBroker* mb, CMPIObjectPath* eCop,
						 CMPIStatus *rc)
{
	CM_LOGGER(mb)->logDebug("CMPIBrokerEnc: mbEncNewInstance()");

	OpenWBEM::CIMObjectPath * cop = static_cast<OpenWBEM::CIMObjectPath *>(eCop->hdl);

	OpenWBEM::CIMClass *cls=mbGetClass(mb,*cop);

	OpenWBEM::CIMInstance ci;

	if (cls && *cls)
	{
		//CMPIContext *ctx=CMPI_ThreadContext::getContext();
		//CMPIFlags flgs = ctx->ft->getEntry(
		//		ctx,CMPIInvocationFlags,rc).value.uint32;
		ci = cls->newInstance();

		/* cloning without include qualifiers means
					 losing key properties */
		//ci = ci.clone(((flgs & CMPI_FLAG_LocalOnly)!=0),
		//              ((flgs & CMPI_FLAG_IncludeQualifiers)!=0),
		//              ((flgs & CMPI_FLAG_IncludeClassOrigin)!=0));
	}
	else
		ci.setClassName(cop->getObjectName());

	CMPIInstance * neInst = (CMPIInstance *)new CMPI_Object(
					new OpenWBEM::CIMInstance(ci));
	CMSetStatus(rc,CMPI_RC_OK);
	return neInst;
}

static CMPIObjectPath* mbEncNewObjectPath(CMPIBroker* mb, char *ns, char *cls,
				  CMPIStatus *rc)
{
	(void) mb;

	CM_LOGGER(mb)->logDebug("CMPIBrokerEnc: mbEncNewObjectPath()");
	OpenWBEM::String className(cls);
	OpenWBEM::String nameSpace(ns);
	OpenWBEM::CIMObjectPath * cop = new OpenWBEM::CIMObjectPath(className,nameSpace);

	CMPIObjectPath * nePath = (CMPIObjectPath*)new CMPI_Object(cop);
	CMSetStatus(rc,CMPI_RC_OK);
	return nePath;
}

static CMPIArgs* mbEncNewArgs(CMPIBroker* mb, CMPIStatus *rc)
{
	(void) mb;

	CMSetStatus(rc,CMPI_RC_OK);
	return (CMPIArgs*)new CMPI_Object(new OpenWBEM::Array<OpenWBEM::CIMParamValue>());
}

static CMPIString* mbEncNewString(CMPIBroker* mb, char *cStr, CMPIStatus *rc)
{
	(void) mb;

	CMSetStatus(rc,CMPI_RC_OK);
	return (CMPIString*)new CMPI_Object(cStr);
}

CMPIString* mbIntNewString(char *s) {
	return mbEncNewString(NULL,s,NULL);
}

static CMPIArray* mbEncNewArray(CMPIBroker* mb, CMPICount count, CMPIType type,
								CMPIStatus *rc)
{
	(void) mb;

	CM_LOGGER(mb)->logDebug("CMPIBrokerEnc: mbEncNewArray()");
	CMSetStatus(rc,CMPI_RC_OK);
	CMPIData * dta = new CMPIData[count+1];
	dta->type = type;
	dta->value.uint32 = count;
	for (uint i=1; i<=count; i++)
	{
		dta[i].type = type;
		dta[i].state = CMPI_nullValue;
		dta[i].value.uint64 = 0;
	}

	return (CMPIArray *)new CMPI_Object(dta);
}

extern CMPIDateTime *newDateTime();

static CMPIDateTime* mbEncNewDateTime(CMPIBroker* mb, CMPIStatus *rc)
{
	(void) mb;

	CMSetStatus(rc,CMPI_RC_OK);
	return newDateTime();
}

extern CMPIDateTime *newDateTime(CMPIUint64,CMPIBoolean);

static CMPIDateTime* mbEncNewDateTimeFromBinary(CMPIBroker* mb,
	 CMPIUint64 time, CMPIBoolean interval ,CMPIStatus *rc)
{
	(void) mb;

	CMSetStatus(rc,CMPI_RC_OK);
	return newDateTime(time,interval);
}

extern CMPIDateTime *newDateTime(char*);

static CMPIDateTime* mbEncNewDateTimeFromString(CMPIBroker* mb,
		 char *t ,CMPIStatus *rc)
{
	(void) mb;

	CMSetStatus(rc,CMPI_RC_OK);
	return newDateTime(t);
}

static CMPIString* mbEncToString(CMPIBroker * mb, void * o, CMPIStatus * rc)
{
	(void) mb;
	CMPI_Object *obj = (CMPI_Object*)o;
	OpenWBEM::String str;

	if (obj == NULL)
	{
		str.format("** Null object ptr (0x%p) **", o);
		CMSetStatus(rc, CMPI_RC_ERR_FAILED);
		return (CMPIString*) new CMPI_Object(str);
	}

	if (obj->hdl == NULL)
	{
		str.format("** Null object hdl (*0x%p) **", o);
		CMSetStatus(rc, CMPI_RC_ERR_FAILED);
		return (CMPIString*) new CMPI_Object(str);
	}

	if (obj->ftab == (void*)CMPI_Instance_Ftab
		|| obj->ftab == (void*)CMPI_InstanceOnStack_Ftab)
	{
		str.format("** Object not supported (0x%x) **", (int)o);
		CMSetStatus(rc,CMPI_RC_ERR_FAILED);
		return (CMPIString*) new CMPI_Object(str);
	}
	else if (obj->ftab == (void*)CMPI_ObjectPath_Ftab
			 || obj->ftab == (void*)CMPI_ObjectPathOnStack_Ftab)
	{
		str = ((OpenWBEM::CIMObjectPath*)obj->hdl)->toString();
	}
	else if(obj->ftab == (void*) CMPI_String_Ftab)
	{
		str = *((OpenWBEM::String*) obj->hdl);
	}
	/* 
	else if (obj->ftab==(void*)CMPI_SelectExp_Ftab ||
		obj->ftab==(void*)CMPI_SelectCondDoc_Ftab ||
		obj->ftab==(void*)CMPI_SelectCondCod_Ftab ||
		obj->ftab==(void*)CMPI_SubCond_Ftab ||
		obj->ftab==(void*)CMPI_Predicate_Ftab)
	{
		sprintf(msg,"** Object not supported (0x%p) **",o);
		CMSetStatus(rc,CMPI_RC_ERR_FAILED);
		return (CMPIString*) new CMPI_Object(msg);
	}
	*/
	else 
	{
		str.format("** Object not recognized (0x%p) **", o);
		CMSetStatus(rc, CMPI_RC_ERR_FAILED);
		return (CMPIString*) new CMPI_Object(str);
	}

	return (CMPIString*) new CMPI_Object(str);
}

static CMPIBoolean mbEncClassPathIsA(CMPIBroker *mb, CMPIObjectPath *eCp,
					char *type, CMPIStatus *rc)
{
	(void) rc;

	OpenWBEM::CIMObjectPath* cop=static_cast<OpenWBEM::CIMObjectPath *>(eCp->hdl);

	OpenWBEM::String tcn(type);
 
	if (tcn == cop->getClassName()) return 1;
 
	OpenWBEM::CIMClass *cc=mbGetClass(mb,*cop);
	OpenWBEM::CIMObjectPath  scp(*cop);
	scp.setClassName(cc->getSuperClass());
																				
	for (; !scp.getClassName().empty(); )
	{
		cc=mbGetClass(mb,scp);
		if (cc->getName()==tcn) return 1;
		scp.setClassName(cc->getSuperClass());
	};
	return 0;
}

CMPIBoolean mbEncIsOfType(CMPIBroker *mb, void *o, char *type, CMPIStatus *rc)
{
	CMPI_Object *obj=(CMPI_Object*)o;
	char msg[128];
	if (obj==NULL) {
		sprintf(msg,"** Null object ptr (0x%x) **",(int)o);
		CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,msg);
		return 0;
	}
																				
	CMSetStatus(rc,CMPI_RC_OK);
 
	if (obj->ftab==(void*)CMPI_Instance_Ftab &&
		strcmp(type,"CMPIInstance")==0) return 1;
	if (obj->ftab!=(void*)CMPI_ObjectPath_Ftab &&
		strcmp(type,"CMPIObjectPath")==0) return 1;
	/* if (obj->ftab!=(void*)CMPI_SelectExp_Ftab &&
		strcmp(type,"CMPISelectExp")==0) return 1;
	if (obj->ftab!=(void*)CMPI_SelectCondDoc_Ftab &&
		strcmp(type,"CMPISelectCondDoc")==0) return 1;
	if (obj->ftab!=(void*)CMPI_SelectCondCod_Ftab &&
		strcmp(type,"CMPISelectCondCod")==0) return 1;
	if (obj->ftab!=(void*)CMPI_SubCond_Ftab &&
		strcmp(type,"CMPISubCond")==0) return 1;
	if (obj->ftab!=(void*)CMPI_Predicate_Ftab &&
		strcmp(type,"CMPIPredicate")==0) return 1;
	*/
	 if (obj->ftab!=(void*)CMPI_Array_Ftab &&
		strcmp(type,"CMPIArray")==0) return 1;
																				
	sprintf(msg,"** Object not recognized (0x%x) **",(int)o);
	CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,msg);
	return 0;
}


static CMPIString* mbEncGetType(CMPIBroker *mb, void *o, CMPIStatus *rc)
{
	CMPI_Object *obj=(CMPI_Object*)o;
	char msg[128];
	if (obj==NULL) {
		sprintf(msg,"** Null object ptr (0x%x) **",(int)o);
		CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,msg);
		return NULL; 
	}
																				
	CMSetStatus(rc,CMPI_RC_OK);
																				
	if (obj->ftab==(void*)CMPI_Instance_Ftab)
		return mb->eft->newString(mb,"CMPIInstance",rc);
	if (obj->ftab!=(void*)CMPI_ObjectPath_Ftab)
		return mb->eft->newString(mb,"CMPIObjectPath",rc);
	/* if (obj->ftab!=(void*)CMPI_SelectExp_Ftab)
		return mb->eft->newString(mb,"CMPISelectExp",rc);
	if (obj->ftab!=(void*)CMPI_SelectCondDoc_Ftab)
		return mb->eft->newString(mb,"CMPISelectCondDo",rc);
	if (obj->ftab!=(void*)CMPI_SelectCondCod_Ftab)
		return mb->eft->newString(mb,"CMPISelectCondCod",rc);
	if (obj->ftab!=(void*)CMPI_SubCond_Ftab)
		return mb->eft->newString(mb,"CMPISubCond",rc);
	if (obj->ftab!=(void*)CMPI_Predicate_Ftab)
		return mb->eft->newString(mb,"CMPIPredicate",rc);
	*/
	if (obj->ftab!=(void*)CMPI_Array_Ftab)
		return mb->eft->newString(mb,"CMPIArray",rc);
																				
	sprintf(msg,"** Object not recognized (0x%x) **",(int)o);
	CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,msg);
	return NULL;
}

#if defined (CMPI_VER_85)
static Formatter::Arg formatValue(va_list *argptr, CMPIStatus *rc) {
																				
   CMPIValue *val=va_arg(*argptr,CMPIValue*);
   CMPIType type=va_arg(*argptr,int);
   CMSetStatus(rc,CMPI_RC_OK);
																				
   if ((type & (CMPI_UINT|CMPI_SINT))==CMPI_UINT) {
	 CMPIUint64 u64;
	  switch (type) {
	  case CMPI_uint8:  u64=(CMPIUint64)val->uint8;  break;
	  case CMPI_uint16: u64=(CMPIUint64)val->uint16; break;
	  case CMPI_uint32: u64=(CMPIUint64)val->uint32; break;
	  case CMPI_uint64: u64=(CMPIUint64)val->uint64; break;
	  }
	  return Formatter::Arg(u64);
   }
   else if ((type & (CMPI_UINT|CMPI_SINT))==CMPI_SINT) {
	CMPISint64 s64;
	 switch (type) {
	  case CMPI_sint8:  s64=(CMPISint64)val->sint8;  break;
	  case CMPI_sint16: s64=(CMPISint64)val->sint16; break;
	  case CMPI_sint32: s64=(CMPISint64)val->sint32; break;
	  case CMPI_sint64: s64=(CMPISint64)val->sint64; break;
	  }
	  return Formatter::Arg(s64);
   }
   else if (type==CMPI_chars) return Formatter::Arg((const char*)val);
   else if (type==CMPI_string)
	  return Formatter::Arg((const char*)CMGetCharsPtr(val->string,NULL));
   else if (type==CMPI_real32)  return Formatter::Arg((CMPIReal64)val->real32);
																				
   else if (type==CMPI_real64)  return Formatter::Arg(val->real64);
   else if (type==CMPI_boolean) return Formatter::Arg((Boolean)val->boolean);
																				
   CMSetStatus(rc,CMPI_RC_ERR_INVALID_PARAMETER);
   return Formatter::Arg((Boolean)0);
}
 
CMPIString* mbEncGetMessage(CMPIBroker *mb, char *msgId, char *defMsg,
			CMPIStatus* rc, unsigned int count, ...) {
   MessageLoaderParms parms(msgId,defMsg);
   //cout<<"::: mbEncGetMessage() count: "<<count<<endl;
   if (count>0) {
	  va_list argptr;
	  va_start(argptr,count);
	  for (;;) {
		 if (count>0) parms.arg0=formatValue(&argptr,rc);
		 else break;
		 if (count>1) parms.arg1=formatValue(&argptr,rc);
		 else break;
		 if (count>2) parms.arg2=formatValue(&argptr,rc);
		 else break;
		 if (count>3) parms.arg3=formatValue(&argptr,rc);
		 else break;
		 if (count>4) parms.arg4=formatValue(&argptr,rc);
		 else break;
		 if (count>5) parms.arg5=formatValue(&argptr,rc);
		 else break;
		 if (count>6) parms.arg6=formatValue(&argptr,rc);
		 else break;
		 if (count>7) parms.arg7=formatValue(&argptr,rc);
		 else break;
		 if (count>8) parms.arg8=formatValue(&argptr,rc);
		 else break;
		 if (count>9) parms.arg9=formatValue(&argptr,rc);
		 break;
	  }
   }
   String nMsg=MessageLoader::getMessage(parms);
   return string2CMPIString(nMsg);
}
#endif



static CMPIBrokerEncFT brokerEnc_FT={
CMPICurrentVersion,
	mbEncNewInstance,
	mbEncNewObjectPath,
	mbEncNewArgs,
	mbEncNewString,
	mbEncNewArray,
	mbEncNewDateTime,
	mbEncNewDateTimeFromBinary,
	mbEncNewDateTimeFromString,
	NULL,
	mbEncClassPathIsA,
	mbEncToString,
	mbEncIsOfType,
	mbEncGetType,
#if defined (CMPI_VER_85)
	mbEncGetMessage,
#endif
};

CMPIBrokerEncFT *CMPI_BrokerEnc_Ftab=&brokerEnc_FT;

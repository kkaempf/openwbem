
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
 * Contributors:
 *
 * Description: CMPIBroker up-call support
 *
 */

#include <iostream>
#include "cmpisrv.h"
#include "OW_CIMException.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_LocalCIMOMHandle.hpp"

//extern int traceAdapter;


// mb->hdl is a pointer to OW_ProviderEnvironmentIFCRef
#define CM_CIMOM(mb) \
(* static_cast<OW_ProviderEnvironmentIFCRef *>(mb->hdl))->getCIMOMHandle()
#define CM_Context(ctx) (((CMPI_Context*)ctx)->ctx)
#define CM_Instance(ci) ((OW_CIMInstance*)ci->hdl)
#define CM_ObjectPath(cop) ((OW_CIMObjectPath*)cop->hdl)
#define CM_LocalOnly(flgs) (OW_Bool)(((flgs) & CMPI_FLAG_LocalOnly)!=0)
#define CM_ClassOrigin(flgs) (OW_Bool)(((flgs) & CMPI_FLAG_IncludeClassOrigin)!=0)
#define CM_IncludeQualifiers(flgs) (OW_Bool)(((flgs) & CMPI_FLAG_IncludeQualifiers) !=0)
#define CM_DeepInheritance(flgs) (((flgs) & CMPI_FLAG_DeepInheritance)!=0)


class OW_CIMObjectPathArrayBuilder :  public OW_CIMObjectPathResultHandlerIFC
{
public:
        OW_CIMObjectPathArrayBuilder(OW_CIMObjectPathArray& a_)
        : a(a_)
        {
        }
private:
        void doHandle(const OW_CIMObjectPath& x)
	{
		a.push_back(x);
	}
        OW_CIMObjectPathArray a;
};


class OW_CIMInstanceArrayBuilder : public OW_CIMInstanceResultHandlerIFC
{
public:
        OW_CIMInstanceArrayBuilder(OW_CIMInstanceArray& a_)
        : a(a_)
        {
        }
private:
        void doHandle(const OW_CIMInstance& x)
	{
		a.push_back(x);
	}
        OW_CIMInstanceArray a;
};


#define DDD(X) X


OW_CIMPropertyArray *getList(char** l) {
   (void) l;

   return new OW_CIMPropertyArray;
}


OW_CIMClass* mbGetClass(CMPIBroker *mb, const OW_CIMObjectPath &cop) {

   DDD(std::cout<<"--- mbGetClass()"<<std::endl;)

   CMPI_Broker *xBroker = (CMPI_Broker*)mb;
   OW_String clsId = cop.getNameSpace()+":"+cop.getObjectName();
   OW_CIMClass ccp;
   if (xBroker->clsCache) {
      if (!(ccp = xBroker->clsCache->getFromCache(clsId)))
           return new OW_CIMClass(ccp);
   }
   else xBroker->clsCache=new ClassCache();

   try {
      OW_CIMClass cc=CM_CIMOM(mb)->getClass(
                     cop.getNameSpace(),
                     cop.getObjectName(),
                     false, true, false);

      xBroker->clsCache->addToCache(cc, clsId);
      return new OW_CIMClass(cc);
   }
   catch (OW_CIMException &e) {
      DDD(std::cout<<"### exception: mbGetClass - code: "<<e.type()<<" msg: "<<e.getMessage()<<std::endl;)
   }
   return NULL;
}

static CMPIInstance* mbGetInstance(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char **properties, CMPIStatus *rc) {
   DDD(std::cout<<"--- mbGetInstance()"<<std::endl;)
   CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
   OW_CIMPropertyArray * props = getList(properties);

   OW_CIMObjectPath qop(
                     CM_ObjectPath(cop)->getObjectName(),
                     * props);
   try {
      OW_CIMInstance ci=CM_CIMOM(mb)->getInstance(
		  CM_ObjectPath(cop)->getNameSpace(),
		  qop,
		  CM_LocalOnly(flgs),
		  CM_IncludeQualifiers(flgs),
		  CM_ClassOrigin(flgs)
                  );
		  //, *props);
      delete props;
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return (CMPIInstance*)new CMPI_Object(new OW_CIMInstance(ci));
   }
   catch (OW_CIMException &e) {
      DDD(std::cout<<"### exception: mbGetInstance - code: "<<e.type()<<" msg: "<<e.getMessage()<<std::endl;)
      if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
   }
   delete props;
   return NULL;
}

static CMPIObjectPath* mbCreateInstance(CMPIBroker *mb, CMPIContext *ctx,
                CMPIObjectPath *cop, CMPIInstance *ci, CMPIStatus *rc) {
   (void) ctx;

   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
   DDD(std::cout<<"--- mbCreateInstance()"<<std::endl;)
   try {
      OW_CIMObjectPath ncop=CM_CIMOM(mb)->createInstance(
                  //OperationContext(*CM_Context(ctx)),
		  CM_ObjectPath(cop)->getNameSpace(),
                  *CM_Instance(ci));
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return (CMPIObjectPath*)new CMPI_Object(new OW_CIMObjectPath(ncop));
   }
   catch (OW_CIMException &e) {
      DDD(std::cout<<"### exception: mbCreateInstance - code: "<<e.type()<<" msg: "<<e.getMessage()<<std::endl;)
      if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERROR);
   return NULL;
}

static CMPIStatus mbSetInstance(CMPIBroker *mb, CMPIContext *ctx,
		CMPIObjectPath *cop, CMPIInstance *ci) {
   DDD(std::cout<<"--- mbSetInstance()"<<std::endl;)
   CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
   try {
      OW_StringArray * s_props = new OW_StringArray();

      CM_CIMOM(mb)->modifyInstance(
		  CM_ObjectPath(cop)->getNameSpace(),
                  *CM_Instance(ci),
		  CM_IncludeQualifiers(flgs),
                  s_props);
      delete s_props;
      CMReturn(CMPI_RC_OK);
   }
   catch (OW_CIMException &e) {
      DDD(std::cout<<"### exception: mbSetInstance - code: "<<e.type()<<" msg: "<<e.getMessage()<<std::endl;)
      CMReturn((CMPIrc)e.getErrNo());
   }
   CMReturn(CMPI_RC_ERR_FAILED);
}

static CMPIStatus mbDeleteInstance (CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop) {
   (void) ctx;

   DDD(std::cout<<"--- mbDeleteInstance()"<<std::endl;)

   OW_CIMObjectPath qop( CM_ObjectPath(cop)->getObjectName(),
		     CM_ObjectPath(cop)->getKeys());

   try {
      CM_CIMOM(mb)->deleteInstance(
		  CM_ObjectPath(cop)->getNameSpace(),
		  qop);
      CMReturn(CMPI_RC_OK);
   }
   catch (OW_CIMException &e) {
      DDD(std::cout<<"### exception: mbDeleteInstance - code: "<<e.type()<<" msg: "<<e.getMessage()<<std::endl;)
      CMReturn((CMPIrc)e.getErrNo());
   }
   CMReturn(CMPI_RC_ERROR);
}

static CMPIEnumeration* mbExecQuery(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *query, char *lang, CMPIStatus *rc) {
   (void) mb; (void) ctx; (void) cop; (void) query; (void) lang; (void) rc;
   DDD(std::cout<<"--- mbExecQuery()"<<std::endl;)

   try {
      OW_CIMInstanceArray cia;
      OW_CIMInstanceArrayBuilder result(cia);

      CM_CIMOM(mb)->execQuery(
		  CM_ObjectPath(cop)->getNameSpace(),
                  result,
		  OW_String(query),
		  OW_String(lang));

      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return new CMPI_ObjEnumeration(new OW_Array<OW_CIMInstance>(cia));
   }
   catch (OW_CIMException &e) {
      DDD(std::cout<<"### exception: mbExecQuery - code: "<<e.type()<<" msg: "<<e.getMessage()<<std::endl;)
      if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   return NULL;
}

static CMPIEnumeration* mbEnumInstances(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char **properties, CMPIStatus *rc) {
   DDD(std::cout<<"--- mbEnumInstances()"<<std::endl;)

   CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
   OW_CIMPropertyArray *props =getList(properties);

   OW_CIMInstanceArray cia;
   OW_CIMInstanceArrayBuilder result(cia);

   try {
               //((OW_LocalCIMOMHandle*)mb->hdl)->enumInstances(
		CM_CIMOM(mb)->enumInstances(
		  CM_ObjectPath(cop)->getNameSpace(),
		  CM_ObjectPath(cop)->getObjectName(),
                  result,
		  CM_DeepInheritance(flgs),
		  CM_LocalOnly(flgs),
		  CM_IncludeQualifiers(flgs),
		  CM_ClassOrigin(flgs)
                  );
		  // ,*props);
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      delete props;
      return new CMPI_InstEnumeration( new OW_CIMInstanceArray(cia));
   }
   catch (OW_CIMException &e) {
      DDD(std::cout<<"### exception: mbEnumInstances - code: "<<e.type()<<" msg: "<<e.getMessage()<<std::endl;)
      if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   delete props;
   return NULL;
}

static CMPIEnumeration* mbEnumInstanceNames(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, CMPIStatus *rc) {
   (void) ctx;

   DDD(std::cout<<"--- mbEnumInstanceNames()"<<std::endl;)

   OW_CIMObjectPathArray cia;
   OW_CIMObjectPathArrayBuilder result(cia);

   try {
               //((OW_LocalCIMOMHandle*)mb->hdl)->enumInstanceNames(
		CM_CIMOM(mb)->enumInstanceNames(
		  CM_ObjectPath(cop)->getNameSpace(),
		  CM_ObjectPath(cop)->getClassName(),
                  result);
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return new CMPI_OpEnumeration( new OW_CIMObjectPathArray(cia));
   }
   catch (OW_CIMException &e) {
      DDD(std::cout<<"### exception: mbEnumInstances - code: "<<e.type()<<" msg: "<<e.getMessage()<<std::endl;)
      if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   return NULL;
}

static CMPIEnumeration* mbAssociators(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *assocClass, char *resultClass,
                 char *role, char *resultRole, char **properties, CMPIStatus *rc) {
   DDD(std::cout<<"--- mbAssociators()"<<std::endl;)

   CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;

   OW_CIMPropertyArray *props =getList(properties);
   OW_StringArray * s_props = new OW_StringArray();

   OW_CIMInstanceArray cia;
   OW_CIMInstanceArrayBuilder result(cia);

   try {
		CM_CIMOM(mb)->associators(
		  CM_ObjectPath(cop)->getNameSpace(),
		  *CM_ObjectPath(cop),
		  result,
		  OW_String(assocClass),
		  OW_String(resultClass),
		  OW_String(role),
		  OW_String(resultRole),
		  CM_IncludeQualifiers(flgs),
		  CM_ClassOrigin(flgs),
		  s_props);
		  //*props);
      delete s_props;

      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      delete props;
      //return new CMPI_ObjEnumeration(new Array<CIMObject>(en));
      return new CMPI_ObjEnumeration( new OW_CIMInstanceArray(cia));

   }
   catch (OW_CIMException &e) {
      DDD(std::cout<<"### exception: mbAssociators - code: "<<e.type()<<" msg: "<<e.getMessage()<<std::endl;)
      if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   delete props;
   return NULL;
}

static CMPIEnumeration* mbAssociatorNames(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *assocClass, char *resultClass,
		 char *role, char *resultRole, CMPIStatus *rc) {
   (void) ctx;
   DDD(std::cout<<"--- mbAssociatorsNames()"<<std::endl;)

   OW_CIMObjectPathArray cia;
   OW_CIMObjectPathArrayBuilder result(cia);

   try {
		CM_CIMOM(mb)->associatorNames(
		  CM_ObjectPath(cop)->getNameSpace(),
		  *CM_ObjectPath(cop),
		  result,
		  OW_String(assocClass),
		  OW_String(resultClass),
		  OW_String(role),
		  OW_String(resultRole));
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return new CMPI_OpEnumeration(new OW_CIMObjectPathArray(cia));
   }
   catch (OW_CIMException &e) {
      DDD(std::cout<<"### exception: mbAssociatorsNames - code: "<<e.type()<<" msg: "<<e.getMessage()<<std::endl;)
      if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   return NULL;
}

static CMPIEnumeration* mbReferences(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop,  char *resultClass, char *role ,
		 char **properties, CMPIStatus *rc) {
   DDD(std::cout<<"--- mbReferences()"<<std::endl;)
   CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
   OW_CIMPropertyArray *props = getList(properties);

   OW_StringArray * s_props = new OW_StringArray();

   OW_CIMInstanceArray cia;
   OW_CIMInstanceArrayBuilder result(cia);

   try {
               //((OW_LocalCIMOMHandle*)mb->hdl)->references(
		CM_CIMOM(mb)->references(
		  CM_ObjectPath(cop)->getNameSpace(),
		  *CM_ObjectPath(cop),
		  result,
		  OW_String(resultClass),
		  OW_String(role),
		  CM_IncludeQualifiers(flgs),
		  CM_ClassOrigin(flgs),
		  s_props
                  );
		  //, *props);
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      delete props;
      delete s_props;
      return new CMPI_ObjEnumeration(new OW_CIMInstanceArray(cia));
   }
   catch (OW_CIMException &e) {
      DDD(std::cout<<"### exception: mbReferences - code: "<<e.type()<<" msg: "<<e.getMessage()<<std::endl;)
      if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   delete props;
   delete s_props;
   return NULL;
}

static CMPIEnumeration* mbReferenceNames(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *resultClass, char *role,
                 CMPIStatus *rc) {
   (void) ctx;

   DDD(std::cout<<"--- mbReferencesNames()"<<std::endl;)

   OW_CIMObjectPathArray cia;
   OW_CIMObjectPathArrayBuilder result(cia);

   try {
		CM_CIMOM(mb)->referenceNames(
		  CM_ObjectPath(cop)->getNameSpace(),
		  *CM_ObjectPath(cop),
		  result,
		  OW_String(resultClass),
		  OW_String(role));
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return new CMPI_OpEnumeration(new OW_CIMObjectPathArray(cia));
   }
   catch (OW_CIMException &e) {
      DDD(std::cout<<"### exception: mbReferencesNames - code: "<<e.type()<<" msg: "<<e.getMessage()<<std::endl;)
      if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   return NULL;
}

static CMPIData mbInvokeMethod(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *method, CMPIArgs *in, CMPIArgs *out,
		 CMPIStatus *rc) {
   (void) ctx; (void) mb; (void) cop; (void) method; (void) in; (void) out;

   CMPIData data={(CMPIType) 0, CMPI_nullValue, CMPIValue()};
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
   return data;
}

static CMPIStatus mbSetProperty(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *name, CMPIValue *val,
                 CMPIType type) {
   (void) ctx;

   DDD(std::cout<<"--- mbSetProperty()"<<std::endl;)

   CMPIrc rc;
   OW_CIMValue v=value2CIMValue(val,type,&rc);
   try {
      //((OW_LocalCIMOMHandle*)mb->hdl)->setProperty(
		CM_CIMOM(mb)->setProperty(
		  CM_ObjectPath(cop)->getNameSpace(),
		  *CM_ObjectPath(cop),
		  OW_String(name),
		  v);
      CMReturn(CMPI_RC_OK);
   }
   catch (OW_CIMException &e) {
      DDD(std::cout<<"### exception: mbSetProperty - code: "<<e.type()<<" msg: "<<e.getMessage()<<std::endl;)
      CMReturn((CMPIrc)e.getErrNo());
   }
   CMReturn(CMPI_RC_ERR_FAILED);
}

static CMPIData mbGetProperty(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop,char *name, CMPIStatus *rc) {

   //(void) ctx; (void) mb; (void) cop; (void) method; (void) in; (void) out;
   (void) ctx;

   DDD(std::cout<<"--- mbGetProperty()"<<std::endl;)

   CMPIData data={(CMPIType) 0, CMPI_nullValue, CMPIValue()};
   //CMPIData data= {0,0,{0}};
   try {
      //OW_CIMValue v = ((OW_LocalCIMOMHandle*)mb->hdl)->getProperty(
      OW_CIMValue v = CM_CIMOM(mb)->getProperty(
		  CM_ObjectPath(cop)->getNameSpace(),
		  *CM_ObjectPath(cop),
		  OW_String(name));
      OW_CIMDataType vType=v.getType();
      CMPIType t=type2CMPIType(vType,v.isArray());
      value2CMPIData(v,t,&data);
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return data;
   }
   catch (OW_CIMException &e) {
      DDD(std::cout<<"### exception: mbGetProperty - code: "<<e.type()<<" msg: "<<e.getMessage()<<std::endl;)
      if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   return data;
}





static CMPIBrokerFT broker_FT={
     0, // brokerClassification;
     CMPICurrentVersion,
     "OpenWBEM",
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

CMPIBroker *CMPI_Broker::staticBroker=NULL;

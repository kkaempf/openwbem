/*
 *
 * CmpiImpl.cpp
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
 * Description: CMPI C++ implemenation support
 *
 */


#include "CmpiInstance.h"
#include "CmpiObjectPath.h"
#include "CmpiResult.h"
#include "CmpiDateTime.h"

#include "CmpiBaseMI.h"
#include "CmpiInstanceMI.h"
#include "CmpiAssociationMI.h"
#include "CmpiMethodMI.h"
#include "CmpiPropertyMI.h"
#include "CmpiIndicationMI.h"
#include "CmpiArray.h"
#include "CmpiBroker.h"
#include "cmpisrv.h"

//CMPIBroker *CmpiBaseMI::broker=NULL;
//int CmpiBaseMI::oneTime=0;

CmpiProviderBase* CmpiProviderBase::base = NULL;

//---------------------------------------------------
//--
//	C to C++ base provider function drivers
//--
//---------------------------------------------------


CMPIStatus CmpiBaseMI::doDriveBaseCleanup
      (void* vi, CMPIContext* eCtx) {
   CMPIInstanceMI *mi=( CMPIInstanceMI*)vi;
   CmpiContext ctx(eCtx);
   CmpiStatus rc(CMPI_RC_OK);
   rc=((CmpiInstanceMI*)mi->hdl)->cleanup(ctx);
   delete (CmpiBaseMI*)mi->hdl;
   return rc.status();
}
CmpiStatus CmpiBaseMI::initialize(const CmpiContext& ctx) {
   return CmpiStatus(CMPI_RC_OK);
}

CmpiStatus CmpiBaseMI::cleanup(CmpiContext& ctx) {
   return CmpiStatus(CMPI_RC_OK);
}

//---------------------------------------------------
//--
//	C to C++ instance provider function drivers
//--
//---------------------------------------------------

CMPIStatus CmpiInstanceMI::driveEnumInstanceNames
   (CMPIInstanceMI* mi,CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop)
{
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eCop);
   return ((CmpiInstanceMI*)mi->hdl)->enumInstanceNames
      (ctx,rslt,cop).status();
}

CMPIStatus CmpiInstanceMI::driveEnumInstances
   (CMPIInstanceMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop, char* *properties)
{
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eCop);
   return ((CmpiInstanceMI*)mi->hdl)->enumInstances
      (ctx,rslt,cop,(const char**)properties).status();
}

CMPIStatus CmpiInstanceMI::driveGetInstance
   (CMPIInstanceMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop, char* *properties)
{
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eCop);
   return ((CmpiInstanceMI*)mi->hdl)->getInstance
      (ctx,rslt,cop,(const char**)properties).status();
}

CMPIStatus CmpiInstanceMI::driveCreateInstance
   (CMPIInstanceMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop, CMPIInstance* eInst)
{
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eCop);
   CmpiInstance inst(eInst);
   return ((CmpiInstanceMI*)mi->hdl)->createInstance
      (ctx,rslt,cop,inst).status();
}

CMPIStatus CmpiInstanceMI::driveSetInstance
   (CMPIInstanceMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop, CMPIInstance* eInst, char* *properties)
{
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eCop);
   CmpiInstance inst(eInst);
   return ((CmpiInstanceMI*)mi->hdl)->setInstance
      (ctx,rslt,cop,inst,(const char**)properties).status();
}

CMPIStatus CmpiInstanceMI::driveDeleteInstance
   (CMPIInstanceMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop)
{
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eCop);
   return ((CmpiInstanceMI*)mi->hdl)->deleteInstance
      (ctx,rslt,cop).status();
}

CMPIStatus CmpiInstanceMI::driveExecQuery
   (CMPIInstanceMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop, char* language ,char* query)
{
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eCop);
   return ((CmpiInstanceMI*)mi->hdl)->execQuery
    (ctx,rslt,cop,language,query).status();
}


//---------------------------------------------------
//--
//	Default Instance provider functions
//--
//---------------------------------------------------

CmpiStatus CmpiInstanceMI::enumInstanceNames
              (const CmpiContext& ctx, CmpiResult& rslt, const CmpiObjectPath& cop) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiInstanceMI::enumInstances
              (const CmpiContext& ctx, CmpiResult& rslt, const CmpiObjectPath& cop,
	       const char* *properties){
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiInstanceMI::getInstance
              (const CmpiContext& ctx, CmpiResult& rslt, const CmpiObjectPath& cop,
	       const char* *properties) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiInstanceMI::createInstance
              (const CmpiContext& ctx, CmpiResult& rslt, const CmpiObjectPath& cop,
               const CmpiInstance& inst) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiInstanceMI::setInstance
              (const CmpiContext& ctx, CmpiResult& rslt, const CmpiObjectPath& cop,
              const  CmpiInstance& inst, const char* *properties) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiInstanceMI::deleteInstance
              (const CmpiContext& ctx, CmpiResult& rslt, const CmpiObjectPath& cop) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiInstanceMI::execQuery
              (const CmpiContext& ctx, CmpiResult& rslt, const CmpiObjectPath& cop,
               const char* language, const char* query) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}


//---------------------------------------------------
//--
//	C to C++ association provider function drivers
//--
//---------------------------------------------------

CMPIStatus CmpiAssociationMI::driveAssociators
      (CMPIAssociationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eOp, char* assocClass, char* resultClass,
       char* role, char* resultRole, char** properties) {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eOp);
   return ((CmpiAssociationMI*)mi->hdl)->associators
      (ctx,rslt,cop,
       (const char*)assocClass,(const char*)resultClass,
       (const char*)role,(const char*)resultRole,(const char**)properties).status();
}

CMPIStatus CmpiAssociationMI::driveAssociatorNames
      (CMPIAssociationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eOp, char* assocClass, char* resultClass,
       char* role, char* resultRole) {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eOp);
   return ((CmpiAssociationMI*)mi->hdl)->associatorNames
      (ctx,rslt,cop,
       (const char*)assocClass,(const char*)resultClass,
       (const char*)role,(const char*)resultRole).status();
}

CMPIStatus CmpiAssociationMI::driveReferences
      (CMPIAssociationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eOp, char* resultClass, char* role ,
       char** properties) {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eOp);
   return ((CmpiAssociationMI*)mi->hdl)->references
      (ctx,rslt,cop,
       (const char*)resultClass,(const char*)role,(const char**)properties).status();
}

CMPIStatus CmpiAssociationMI::driveReferenceNames
      (CMPIAssociationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eOp, char* resultClass, char* role) {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eOp);
   return ((CmpiAssociationMI*)mi->hdl)->referenceNames
      (ctx,rslt,cop,
       (const char*)resultClass,(const char*)role).status();
}


//---------------------------------------------------
//--
//	Default Association provider functions
//--
//---------------------------------------------------

CmpiStatus CmpiAssociationMI::associators
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* assocClass, const char* resultClass,
       const char* role, const char* resultRole, const char** properties) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiAssociationMI::associatorNames
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* assocClass, const char* resultClass,
       const char* role, const char* resultRole) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiAssociationMI::references
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* resultClass, const char* role ,
       const char** properties) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiAssociationMI::referenceNames
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* resultClass, const char* role) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}


//---------------------------------------------------
//--
//	C to C++ method provider function drivers
//--
//---------------------------------------------------

CMPIStatus CmpiMethodMI::driveInvokeMethod
   (CMPIMethodMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop, char* methodName,
    CMPIArgs* eIn, CMPIArgs* eOut)
{
   const CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   const CmpiObjectPath cop(eCop);
   const CmpiArgs in(eIn);
   CmpiArgs out(eOut);
   return ((CmpiMethodMI*)mi->hdl)->invokeMethod
      (ctx,rslt,cop,(const char*)methodName,in,out).status();
}


//---------------------------------------------------
//--
//	Default Method provider functions
//--
//---------------------------------------------------

CmpiStatus CmpiMethodMI::invokeMethod
              (const CmpiContext& ctx, CmpiResult& rslt,
	       const CmpiObjectPath& ref, const char* methodName,
	       const CmpiArgs& in, CmpiArgs& out) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}


//---------------------------------------------------
//--
//	C to C++ property provider function drivers
//--
//---------------------------------------------------

CMPIStatus CmpiPropertyMI::driveSetProperty
      (CMPIPropertyMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eCop, char* name, CMPIData eData) {
   const CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   const CmpiObjectPath cop(eCop);
   const CmpiData data(eData);
   return ((CmpiPropertyMI*)mi->hdl)->setProperty
      (ctx,rslt,cop,(const char*)name,data).status();
}

CMPIStatus CmpiPropertyMI::driveGetProperty
      (CMPIPropertyMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eCop, char* name) {
   const CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   const CmpiObjectPath cop(eCop);
   return ((CmpiPropertyMI*)mi->hdl)->getProperty
      (ctx,rslt,cop,(const char*)name).status();
}


//---------------------------------------------------
//--
//	Default property provider functions
//--
//---------------------------------------------------

CmpiStatus CmpiPropertyMI::setProperty
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* name, const CmpiData& data) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiPropertyMI::getProperty
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* name) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}


//---------------------------------------------------
//--
//	C to C++ indication provider function drivers
//--
//---------------------------------------------------

CMPIStatus CmpiIndicationMI::driveAuthorizeFilter
      (CMPIIndicationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPISelectExp* eSe, char* ns, CMPIObjectPath* eCop, char* user){
   const CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   const CmpiObjectPath cop(eCop);
   const CmpiSelectExp se(eSe);
   return ((CmpiIndicationMI*)mi->hdl)->authorizeFilter
      (ctx,rslt,se,(const char*)ns,cop,(const char*)user).status();
}

CMPIStatus CmpiIndicationMI::driveMustPoll
      (CMPIIndicationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPISelectExp* eSe, char* ns, CMPIObjectPath* eCop){
   const CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   const CmpiObjectPath cop(eCop);
   const CmpiSelectExp se(eSe);
   return ((CmpiIndicationMI*)mi->hdl)->mustPoll
      (ctx,rslt,se,(const char*)ns,cop).status();
}

CMPIStatus CmpiIndicationMI::driveActivateFilter
      (CMPIIndicationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPISelectExp* eSe, char* ns, CMPIObjectPath* eCop, CMPIBoolean first){
   const CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   const CmpiObjectPath cop(eCop);
   const CmpiSelectExp se(eSe);
   return ((CmpiIndicationMI*)mi->hdl)->activateFilter
      (ctx,rslt,se,(const char*)ns,cop,first).status();
}

CMPIStatus CmpiIndicationMI::driveDeActivateFilter
      (CMPIIndicationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPISelectExp* eSe, char* ns, CMPIObjectPath* eCop, CMPIBoolean last){
   const CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   const CmpiObjectPath cop(eCop);
   const CmpiSelectExp se(eSe);
   return ((CmpiIndicationMI*)mi->hdl)->deActivateFilter
      (ctx,rslt,se,(const char*)ns,cop,last).status();
}


//---------------------------------------------------
//--
//	Default indication provider functions
//--
//---------------------------------------------------

CmpiStatus CmpiIndicationMI::authorizeFilter
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiSelectExp& se, const char* ns, const CmpiObjectPath& op, const char* user) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiIndicationMI::mustPoll
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiSelectExp& se, const char* ns, const CmpiObjectPath& op) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiIndicationMI::activateFilter
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiSelectExp& se, const char* ns, const CmpiObjectPath& op,
       CMPIBoolean first) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiIndicationMI::deActivateFilter
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiSelectExp& se, const char* ns, const CmpiObjectPath& op,
       CMPIBoolean last) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}






//---------------------------------------------------
//--
//	CmpiArray member functions
//--
//---------------------------------------------------

void *CmpiArray::makeArray(CMPIBroker *mb, CMPICount max, CMPIType type) {
   CMPIStatus rc;
   void *array=mb->eft->newArray(mb,max,type,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return array;
}

CMPICount CmpiArray::size() {
   CMPIStatus rc;
   CMPICount c=getEnc()->ft->getSize(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return c;
}

//---------------------------------------------------
//--
//	CmpiArrayIdx member functions
//--
//---------------------------------------------------


void CmpiArrayIdx::operator=(const CmpiString v) {
   CMPIStatus rc;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_string)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   rc=ar.getEnc()->ft->setElementAt(ar.getEnc(),idx,(CMPIValue*)&v,CMPI_string);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

void CmpiArrayIdx::operator=(const char* v) {
   CMPIStatus rc;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_chars)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   rc=ar.getEnc()->ft->setElementAt(ar.getEnc(),idx,(CMPIValue*)&v,CMPI_chars);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

void CmpiArrayIdx::operator=(const CMPISint8 v) {
   CMPIStatus rc;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_sint8)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   rc=ar.getEnc()->ft->setElementAt(ar.getEnc(),idx,(CMPIValue*)&v,CMPI_sint8);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

void CmpiArrayIdx::operator=(const CMPISint16 v) {
   CMPIStatus rc;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_sint16)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   rc=ar.getEnc()->ft->setElementAt(ar.getEnc(),idx,(CMPIValue*)&v,CMPI_sint16);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

void CmpiArrayIdx::operator=(const CMPISint32 v) {
   CMPIStatus rc;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_sint32)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   rc=ar.getEnc()->ft->setElementAt(ar.getEnc(),idx,(CMPIValue*)&v,CMPI_sint32);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

void CmpiArrayIdx::operator=(const int v) {
   CMPIStatus rc;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_sint32)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   rc=ar.getEnc()->ft->setElementAt(ar.getEnc(),idx,(CMPIValue*)&v,CMPI_sint32);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}


void CmpiArrayIdx::operator=(const CMPISint64 v) {
   CMPIStatus rc;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_sint64)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   rc=ar.getEnc()->ft->setElementAt(ar.getEnc(),idx,(CMPIValue*)&v,CMPI_sint64);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

void CmpiArrayIdx::operator=(const CMPIUint8 v) {
   CMPIStatus rc;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_uint8)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   rc=ar.getEnc()->ft->setElementAt(ar.getEnc(),idx,(CMPIValue*)&v,CMPI_uint8);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

void CmpiArrayIdx::operator=(const CMPIUint16 v) {
   CMPIStatus rc;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_uint16)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   rc=ar.getEnc()->ft->setElementAt(ar.getEnc(),idx,(CMPIValue*)&v,CMPI_uint16);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

void CmpiArrayIdx::operator=(const CMPIUint32 v) {
   CMPIStatus rc;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_uint32)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   rc=ar.getEnc()->ft->setElementAt(ar.getEnc(),idx,(CMPIValue*)&v,CMPI_uint32);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

void CmpiArrayIdx::operator=(const unsigned int v) {
   CMPIStatus rc;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_uint32)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   rc=ar.getEnc()->ft->setElementAt(ar.getEnc(),idx,(CMPIValue*)&v,CMPI_uint32);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

void CmpiArrayIdx::operator=(const CMPIUint64 v) {
   CMPIStatus rc;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_uint64)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   rc=ar.getEnc()->ft->setElementAt(ar.getEnc(),idx,(CMPIValue*)&v,CMPI_uint64);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}


void CmpiArrayIdx::operator>>(CmpiString& v) {
   CMPIStatus rc;
   CMPIData d;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_string)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   d=ar.getEnc()->ft->getElementAt(ar.getEnc(),idx,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   v=CmpiString(d.value.string);
}

void CmpiArrayIdx::operator>>(CMPISint8& v) {
   CMPIStatus rc;
   CMPIData d;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_sint8)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   d=ar.getEnc()->ft->getElementAt(ar.getEnc(),idx,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   v=d.value.sint8;
}

void CmpiArrayIdx::operator>>(CMPISint16& v) {
   CMPIStatus rc;
   CMPIData d;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_sint16)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   d=ar.getEnc()->ft->getElementAt(ar.getEnc(),idx,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   v=d.value.sint16;
}

void CmpiArrayIdx::operator>>(CMPISint32& v) {
   CMPIStatus rc;
   CMPIData d;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_sint32)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   d=ar.getEnc()->ft->getElementAt(ar.getEnc(),idx,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   v=d.value.sint32;
}

void CmpiArrayIdx::operator>>(int& v) {
   CMPIStatus rc;
   CMPIData d;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_sint32)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   d=ar.getEnc()->ft->getElementAt(ar.getEnc(),idx,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   v=(int)d.value.sint32;
}

void CmpiArrayIdx::operator>>(CMPISint64& v) {
   CMPIStatus rc;
   CMPIData d;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_sint64)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   d=ar.getEnc()->ft->getElementAt(ar.getEnc(),idx,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   v=(int)d.value.sint64;
}

void CmpiArrayIdx::operator>>(CMPIUint8& v) {
   CMPIStatus rc;
   CMPIData d;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_uint8)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   d=ar.getEnc()->ft->getElementAt(ar.getEnc(),idx,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   v=d.value.uint8;
}

void CmpiArrayIdx::operator>>(CMPIUint16& v) {
   CMPIStatus rc;
   CMPIData d;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_uint16)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   d=ar.getEnc()->ft->getElementAt(ar.getEnc(),idx,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   v=d.value.uint16;
}

void CmpiArrayIdx::operator>>(CMPIUint32& v) {
   CMPIStatus rc;
   CMPIData d;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_uint32)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   d=ar.getEnc()->ft->getElementAt(ar.getEnc(),idx,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   v=d.value.uint32;
}

void CmpiArrayIdx::operator>>(unsigned int& v) {
   CMPIStatus rc;
   CMPIData d;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_uint32)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   d=ar.getEnc()->ft->getElementAt(ar.getEnc(),idx,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   v=(unsigned int)d.value.uint32;
}

void CmpiArrayIdx::operator>>(CMPIUint64& v) {
   CMPIStatus rc;
   CMPIData d;
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=CMPI_uint64)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   d=ar.getEnc()->ft->getElementAt(ar.getEnc(),idx,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   v=(int)d.value.uint64;
}



//---------------------------------------------------
//--
//	CmpiData member functions
//--
//---------------------------------------------------


void CmpiData::operator>>(CmpiInstance& v){
   if (data.type!=CMPI_instance)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   else v=CmpiInstance(data.value.inst);
}

void CmpiData::operator>>(CmpiObjectPath& v) {
   if (data.type!=CMPI_ref)
      throw CMPI_RC_ERR_TYPE_MISMATCH;
   else v=CmpiObjectPath(data.value.ref);
}


//---------------------------------------------------
//--
//	CmpiInstance member functions
//--
//---------------------------------------------------


void *CmpiInstance::makeInstance(CMPIBroker *mb, const CmpiObjectPath& cop) {
   CMPIStatus rc;
   void *inst=mb->eft->newInstance(mb,((CmpiObjectPath&)cop).getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return inst;
}

CmpiBoolean CmpiInstance::doInstanceIsA(CMPIBroker *mb, const char *className) {
   CmpiObjectPath cop=getObjectPath();
   return cop.doClassPathIsA(mb,(char*)className);
}

CmpiData CmpiInstance::getProperty(const char* name) {
   CmpiData d;
   CMPIStatus rc;
   d.data=getEnc()->ft->getProperty(getEnc(),(char*)name,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return d;
}

CmpiData CmpiInstance::getProperty(const int pos, CmpiString *name) {
   CmpiData d;
   CMPIStatus rc;
   CMPIString *s;
   d.data=getEnc()->ft->getPropertyAt(getEnc(),pos,&s,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   if (name) *name=CmpiString(s);
   return d;
};

unsigned int CmpiInstance::getPropertyCount() {
   CMPIStatus rc;
   unsigned int c=getEnc()->ft->getPropertyCount(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return c;
}

void CmpiInstance::setProperty(const char* name, CmpiData data) {
   CMPIStatus rc=getEnc()->ft->setProperty(getEnc(),(char*)name,
      data.data.type!=CMPI_chars ? &data.data.value : (CMPIValue*)data.data.value.chars,
      data.data.type);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

CmpiObjectPath CmpiInstance::getObjectPath() {
   CMPIStatus rc;
   CmpiObjectPath cop(getEnc()->ft->getObjectPath(getEnc(),&rc));
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return cop;
}


//---------------------------------------------------
//--
//	CmpiStatus member functions
//--
//---------------------------------------------------


CmpiStatus::CmpiStatus() {
   st.rc=CMPI_RC_OK;
   st.msg=NULL;
}

CmpiStatus::CmpiStatus(CMPIrc rcp) {
   st.rc=rcp;
   st.msg=NULL;
}

//---------------------------------------------------
//--
//	CmpiObjectPath member functions
//--
//---------------------------------------------------



void *CmpiObjectPath::makeObjectPath(CMPIBroker *mb, const char *ns, const char *cls) {
   CMPIStatus rc;
   void *op=mb->eft->newObjectPath(mb,(char*)ns,(char*)cls,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return op;
}

void *CmpiObjectPath::makeObjectPath(CMPIBroker *mb, const CmpiString& ns, const char *cls) {
   CMPIStatus rc;
   void *op=mb->eft->newObjectPath(mb,CMGetCharPtr(ns.getEnc()),(char*)cls,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return op;
}

CmpiBoolean CmpiObjectPath::doClassPathIsA(CMPIBroker *mb, const char *className) {
   CMPIStatus rc;
   CmpiBoolean bv=mb->eft->classPathIsA(mb,getEnc(),(char*)className,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return bv;
}

CmpiString CmpiObjectPath::getNameSpace() const{
   CMPIStatus rc;
   CMPIString *s=getEnc()->ft->getNameSpace(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return CmpiString(s);
}

void CmpiObjectPath::setNameSpace(const char* ns) {
   CMPIStatus rc=getEnc()->ft->setNameSpace(getEnc(),(char*)ns);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

CmpiString CmpiObjectPath::getHostname() const {
   CMPIStatus rc;
   CMPIString *s=getEnc()->ft->getHostname(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return CmpiString(s);
}

void CmpiObjectPath::setHostname(const char* hn) {
   CMPIStatus rc=getEnc()->ft->setHostname(getEnc(),(char*)hn);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

CmpiString CmpiObjectPath::getClassName() const {
   CMPIStatus rc;
   CMPIString *s=getEnc()->ft->getClassName(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return CmpiString(s);
}

void CmpiObjectPath::setClassName(const char* cn) {
   CMPIStatus rc=getEnc()->ft->setClassName(getEnc(),(char*)cn);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

CmpiData CmpiObjectPath::getKey(const char* name) const {
   CmpiData d;
   CMPIStatus rc;
   d.data=getEnc()->ft->getKey(getEnc(),(char*)name,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return d;
}

unsigned int CmpiObjectPath::getKeyCount() const {
   CMPIStatus rc;
   unsigned int c=getEnc()->ft->getKeyCount(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return c;
}

CmpiData CmpiObjectPath::getKey(const int pos, CmpiString *name) const {
   CmpiData d;
   CMPIStatus rc;
   CMPIString *s;
   d.data=getEnc()->ft->getKeyAt(getEnc(),(int)pos,&s,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   if (name) *name=CmpiString(s);
   return d;
}

void CmpiObjectPath::setKey(const char* name, CmpiData data) {
   CMPIStatus rc=getEnc()->ft->addKey(getEnc(),(char*)name,
      data.data.type!=CMPI_chars ? &data.data.value : (CMPIValue*)data.data.value.chars,
      data.data.type);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}


//---------------------------------------------------
//--
//	CmpiResult member functions
//--
//---------------------------------------------------



void CmpiResult::returnData(const CmpiData& d)
{
   CMPIStatus rc=getEnc()->ft->returnData(getEnc(),
      &(const_cast<CmpiData&>(d).data.value),d.data.type);
      if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

void CmpiResult::returnData(const CmpiInstance& d)
{
   CMPIStatus rc=getEnc()->ft->returnInstance(getEnc(),d.getEnc());
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

void CmpiResult::returnData(const CmpiObjectPath& d)
{
   CMPIStatus rc = getEnc()->ft->returnObjectPath(getEnc(),d.getEnc());
   if (rc.rc != CMPI_RC_OK)
   {
      throw rc.rc;
   }
}

void CmpiResult::returnDone() {
   CMPIStatus rc=getEnc()->ft->returnDone(getEnc());
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}



//---------------------------------------------------
//--
//	CmpiBroker member functions
//--
//---------------------------------------------------


void CmpiBroker::deliverIndication(const CmpiContext& ctx, const char*,
                                   const CmpiInstance& inst)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

     // class 1 services
CmpiEnumeration CmpiBroker::enumInstanceNames(const CmpiContext& ctx,
                                              const CmpiObjectPath& cop)
{
   CMPIStatus rc;
   CMPIEnumeration* en=getEnc()->bft->enumInstanceNames
      (getEnc(),ctx.getEnc(),cop.getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return CmpiEnumeration(en);
}

CmpiInstance CmpiBroker::getInstance(const CmpiContext& ctx,
                         const CmpiObjectPath& cop,
                         const char** properties)
{
   CMPIStatus rc;
   CMPIInstance* en=getEnc()->bft->getInstance
      (getEnc(),ctx.getEnc(),cop.getEnc(),(char**)properties,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return CmpiInstance(en);
}


     // class 2 services
CmpiObjectPath CmpiBroker::createInstance(const CmpiContext& ctx,
                              const CmpiObjectPath& cop,
                              const CmpiInstance& inst)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

void CmpiBroker::setInstance(const CmpiContext& ctx, const CmpiObjectPath& cop,
                             const CmpiInstance& inst)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

void CmpiBroker::deleteInstance(const CmpiContext& ctx, const CmpiObjectPath& cop)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiEnumeration CmpiBroker::execQuery(const CmpiContext& ctx,
                          const  CmpiObjectPath& cop,
                          const char* query, const char* language)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}


CmpiEnumeration CmpiBroker::enumInstances(const CmpiContext& ctx,
                              const CmpiObjectPath& cop,
                              const char** properties)
{
   CMPIStatus rc;
   CMPIEnumeration* en=getEnc()->bft->enumInstances
      (getEnc(),ctx.getEnc(),cop.getEnc(),(char**)properties,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return CmpiEnumeration(en);
}

CmpiEnumeration CmpiBroker::associators(const CmpiContext& ctx,
                            const CmpiObjectPath& cop,
                            const char* assocClass, const char* resultClass,
		            const char* role, const char* resultRole,
			    const char** properties)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiEnumeration CmpiBroker::associatorNames(const CmpiContext& ctx,
                                const CmpiObjectPath& cop,
                                const char* assocClass, const char* resultClass,
		                const char* role, const char* resultRole)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiEnumeration CmpiBroker::references(const CmpiContext& ctx,
                           const CmpiObjectPath& cop,
                           const char* resultClass, const char* role,
			   const char** properties)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiEnumeration CmpiBroker::referenceNames(const CmpiContext& ctx,
                               const CmpiObjectPath& cop,
                               const char* resultClass, const char* role)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiData CmpiBroker::invokeMethod(const CmpiContext& ctx, const CmpiObjectPath& cop,
                 const char* methName, const CmpiArgs& in, CmpiArgs& out)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

void CmpiBroker::setProperty(const CmpiContext& ctx, const CmpiObjectPath& cop,
                 const char* name, const CmpiData& data)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiData CmpiBroker::getProperty(const CmpiContext& ctx, const CmpiObjectPath& cop,
                  const char* name)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}




//---------------------------------------------------
//--
//	CmpiString member functions
//--
//---------------------------------------------------




CmpiString::CmpiString(const CmpiString& s) {
   CmpiString *ss=(CmpiString*)&s;
   enc=ss->getEnc()->ft->clone(ss->getEnc(),NULL);
}



//---------------------------------------------------
//--
//	CmpiArgs member functions
//--
//---------------------------------------------------




void *CmpiArgs::makeArgs(CMPIBroker *mb) {
   CMPIStatus rc;
   void *args=mb->eft->newArgs(mb,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return args;
}

void CmpiArgs::setArg(const char* name, CmpiData& data) {
   CMPIStatus rc=getEnc()->ft->addArg(getEnc(),(char*)name,
         data.data.type!=CMPI_chars ? &data.data.value
	                            : (CMPIValue*)data.data.value.chars,
         data.data.type);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
}

CmpiData CmpiArgs::getArg(const int pos, CmpiString *name) {
   CmpiData d;
   CMPIStatus rc;
   CMPIString *s;
   d.data=getEnc()->ft->getArgAt(getEnc(),(int)pos,&s,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   if (name) *name=CmpiString(s);
   return d;
}

CmpiData CmpiArgs::getArg(const char* name) {
   CmpiData d;
   CMPIStatus rc;
   d.data=getEnc()->ft->getArg(getEnc(),(char*)name,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return d;
}

unsigned int CmpiArgs::getArgCount() {
   CMPIStatus rc;
   unsigned int c=getEnc()->ft->getArgCount(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return c;
}



//---------------------------------------------------
//--
//	CmpiObject member functions
//--
//---------------------------------------------------




CmpiString CmpiObject::doToString(CMPIBroker *mb) {
   CMPIStatus rc;
   CMPIString *str=mb->eft->toString(mb,enc,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return CmpiString(str);
}

CmpiBoolean CmpiObject::doIsA(CMPIBroker *mb, const char *typeName) {
   CMPIStatus rc;
   CmpiBoolean bv=mb->eft->isOfType(mb,enc,(char*)typeName,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return bv;
}



//---------------------------------------------------
//--
//	CmpiContext member functions
//--
//---------------------------------------------------



const char *CmpiContext::invocationFlags=CMPIInvocationFlags;

CmpiData CmpiContext::getEntry(const char* name) {
   CmpiData d;
   CMPIStatus rc;
   d.data=getEnc()->ft->getEntry(getEnc(),(char*)name,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return d;
}



//---------------------------------------------------
//--
//	CmpiDateTime member functions
//--
//---------------------------------------------------


void *CmpiDateTime::makeDateTime(CMPIBroker *mb) {
   CMPIStatus rc;
   void *dt=mb->eft->newDateTime(mb,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return dt;
}

void *CmpiDateTime::makeDateTime(CMPIBroker *mb, const char* utcTime) {
   CMPIStatus rc;
   void *dt=mb->eft->newDateTimeFromChars(mb,(char*)utcTime,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return dt;
}

void *CmpiDateTime::makeDateTime(CMPIBroker *mb, const CMPIUint64 binTime,
                                 const CmpiBoolean interval) {
   CMPIStatus rc;
   void *dt=mb->eft->newDateTimeFromBinary(mb,binTime,interval,&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return dt;
}

CmpiBoolean CmpiDateTime::isInterval() {
   CMPIStatus rc;
   CmpiBoolean bv=getEnc()->ft->isInterval(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return bv;
}

CMPIUint64 CmpiDateTime::getDateTime() {
   CMPIStatus rc;
   CMPIUint64 rv=getEnc()->ft->getBinaryFormat(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return rv;
};

//---------------------------------------------------
//--
//	CmpiEnumeration member functions
//--
//---------------------------------------------------
/*
CmpiEnumeration::CmpiEnumeration(CMPIEnumeration* enc) {
   this->enc=enc;
}

CMPIEnumeration *CmpiEnumeration::getEnc() const {
   return (CMPIEnumeration*)enc;
}

CmpiEnumeration::CmpiEnumeration() {
}
*/
CmpiBoolean CmpiEnumeration::hasNext()
{
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CmpiBoolean bv=getEnc()->ft->hasNext(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return bv;
}

CmpiData CmpiEnumeration::getNext()
{
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIData d=getEnc()->ft->getNext(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return CmpiData(d);
}
// drop cursor to 0
void CmpiEnumeration::toFirst()
{
   CMPIStatus rc={CMPI_RC_OK,NULL};
   if ((void*)getEnc()->ft == (void*)CMPI_ObjEnumeration_Ftab)
   {
		CMPI_ObjEnumeration* ie=(CMPI_ObjEnumeration*)getEnc();
      ie->cursor = 0;
   }
   else if ((void*)getEnc()->ft == (void*)CMPI_InstEnumeration_Ftab)
   {
		CMPI_InstEnumeration* ie=(CMPI_InstEnumeration*)getEnc();
      ie->cursor = 0;
   }
   else
   {
		CMPI_OpEnumeration* ie=(CMPI_OpEnumeration*)getEnc();
      ie->cursor = 0;
   }
}

/*
CmpiData CmpiEnumeration::toArray()
{
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIArray* a=getEnc()->ft->toArray(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw rc.rc;
   return CmpiData(CmpiArray(a));
}
*/

// compare CmpiData
bool CmpiData::operator == (const CmpiData& d)
{
   if (data.type == d.data.type)
   {
      // compare simplest types
      switch (data.type)
      {
         case CMPI_string:
         	return strcmp(CmpiString(data.value.string).charPtr(),CmpiString(d.data.value.string).charPtr()) == 0;
         case CMPI_chars: 
         	return strcmp(data.value.chars,d.data.value.chars) == 0;
         case CMPI_sint8 : return data.value.sint8 ==d.data.value.sint8;
         case CMPI_sint16: return data.value.sint16==d.data.value.sint16;
         case CMPI_sint32: return data.value.sint32==d.data.value.sint32;
         case CMPI_sint64: return data.value.sint64==d.data.value.sint64;
         case CMPI_uint8 : return data.value.uint8 ==d.data.value.uint8;
         case CMPI_uint16: return data.value.uint16==d.data.value.uint16;
         case CMPI_uint32: return data.value.uint32==d.data.value.uint32;
         case CMPI_uint64: return data.value.uint64==d.data.value.uint64;
         default:
            throw CMPI_RC_ERR_NOT_SUPPORTED;
      }
   }
   return false;
} 



/*
 *
 * cmpiResult.cpp
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
 * Contributors:
 *
 * Description: CMPIObjectPath support
 *
 */

#include <iostream>
#include "cmpisrv.h"

static CMPIStatus resultReturnData(CMPIResult* eRes, CMPIValue* data, CMPIType type) {
   (void) eRes;
   (void) data;
   (void) type;
#if 0
   CMPIrc rc;
   CIMValue v=value2CIMValue(data,type,&rc);
   if (eRes->ft==CMPI_ResultMeth_Ftab) {
      MethodResultResponseHandler* res=(MethodResultResponseHandler*)eRes->hdl;
      if (((CMPI_Result*)eRes)->flags & RESULT_set==0) {
         //res->processing();
         ((CMPI_Result*)eRes)->flags|=RESULT_set;
      }
      res->handle(v);
   }
   else {
      ValueResponseHandler* res=(ValueResponseHandler*)eRes->hdl;
      if (((CMPI_Result*)eRes)->flags & RESULT_set==0) {
         //res->processing();
         ((CMPI_Result*)eRes)->flags|=RESULT_set;
      }
      res->handle(v);
   }
#endif
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnInstance(CMPIResult* eRes, CMPIInstance* eInst) {
   OW_CIMInstanceResultHandlerIFC * res =
	static_cast<OW_CIMInstanceResultHandlerIFC *> (eRes->hdl);
   const OW_CIMInstance& inst= * (static_cast<OW_CIMInstance *>(eInst->hdl));
	std::cout << "inst to handle " << inst.toMOF() << std::endl;
   res->handle(inst);
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnObjectPath(CMPIResult* eRes, CMPIObjectPath* eRef) {
   (void)eRef;

   OW_CIMObjectPathResultHandlerIFC * res =
	static_cast<OW_CIMObjectPathResultHandlerIFC *>(eRes->hdl);
   
   const OW_CIMObjectPath& cop = *(static_cast<OW_CIMObjectPath *>(eRef->hdl));
	std::cout << "cop to handle " << cop.toMOF() << std::endl;
   res->handle(cop);
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnInstDone(CMPIResult* eRes) {
   (void) eRes;
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnRefDone(CMPIResult* eRes) {
   (void) eRes;
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnDataDone(CMPIResult* eRes) {
   (void) eRes;
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnMethDone(CMPIResult* eRes) {
   (void) eRes;
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultBadReturnData(CMPIResult* eRes, CMPIValue* data, CMPIType type) {
   (void) eRes;
   (void) data;
   (void) type;
   CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

static CMPIStatus resultBadReturnInstance(CMPIResult* eRes, CMPIInstance* eInst) {
   (void) eRes;
   (void) eInst;
   CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

static CMPIStatus resultBadReturnObjectPath(CMPIResult* eRes, CMPIObjectPath* eRef) {
   (void) eRes;
   (void) eRef;
   CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

static CMPIResultFT resultMeth_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     NULL,
     resultReturnData,
     resultBadReturnInstance,
     resultBadReturnObjectPath,
     resultReturnMethDone,
};

static CMPIResultFT resultData_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     NULL,
     resultReturnData,
     resultBadReturnInstance,
     resultBadReturnObjectPath,
     resultReturnDataDone,
};

static CMPIResultFT resultInstOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     NULL,
     resultBadReturnData,
     resultReturnInstance,
     resultBadReturnObjectPath,
     resultReturnInstDone,
};

static CMPIResultFT resultRefOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     NULL,
     resultBadReturnData,
     resultBadReturnInstance,
     resultReturnObjectPath,
     resultReturnRefDone,
};

static CMPIResultFT resultResponseOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     NULL,
     resultBadReturnData,
     resultReturnInstance,
     resultReturnObjectPath,
     resultReturnDataDone,
};

CMPIResultFT *CMPI_ResultMeth_Ftab=&resultMeth_FT;
CMPIResultFT *CMPI_ResultData_Ftab=&resultData_FT;
CMPIResultFT *CMPI_ResultInstOnStack_Ftab=&resultInstOnStack_FT;
CMPIResultFT *CMPI_ResultRefOnStack_Ftab=&resultRefOnStack_FT;
CMPIResultFT *CMPI_ResultResponseOnStack_Ftab=&resultResponseOnStack_FT;

CMPI_ResultOnStack::CMPI_ResultOnStack(
	const OW_CIMObjectPathResultHandlerIFC & handler) {
      hdl= (void *)(&handler);
      ft=CMPI_ResultRefOnStack_Ftab;
      flags=RESULT_ObjectPath;
   }

CMPI_ResultOnStack::CMPI_ResultOnStack(
	const OW_CIMInstanceResultHandlerIFC & handler) {
      hdl= (void *)(&handler);
      ft=CMPI_ResultInstOnStack_Ftab;
      flags=RESULT_Instance;
   }

#if 0
CMPI_ResultOnStack::CMPI_ResultOnStack(
	const MethodResultResultHandlerIFC & handler) {
      hdl= (void *)(&handler);
      ft=CMPI_ResultInstOnStack_Ftab;
      flags=RESULT_Method;
   }
#endif

CMPI_ResultOnStack::CMPI_ResultOnStack(const CMPIObjectPathValueResultHandler& handler) {
      hdl= (void *)&handler;
      ft=CMPI_ResultResponseOnStack_Ftab;
      flags=RESULT_Response;
}

CMPI_ResultOnStack::CMPI_ResultOnStack() {
      //hdl= static_cast<void* >(&handler);
      ft=CMPI_ResultResponseOnStack_Ftab;
      flags=RESULT_Response;
}

CMPI_ResultOnStack::~CMPI_ResultOnStack() {
//      cout<<"--- ~CMPI_ResultOnStack()"<<endl;
      //if (flags & RESULT_set==0)  ((ResponseHandler*)hdl)->processing();
      //if (flags & RESULT_done==0) ((ResponseHandler*)hdl)->complete();
  }


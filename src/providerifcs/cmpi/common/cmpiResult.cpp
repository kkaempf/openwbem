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
 * 
 * Contributor:   Markus Mueller <sedgewick_de@yahoo.de>
 *
 * Description: CMPIObjectPath support
 *
 */

#include <iostream>
#include "cmpisrv.h"
#include "OW_CIMException.hpp"
#include "CmpiProviderBase.h"

static CMPIStatus resultReturnData(CMPIResult* eRes, CMPIValue* data, CMPIType type) {
   CMPIrc rc;
   OW_CIMValue v=value2CIMValue(data,type,&rc);
   if (eRes->ft==CMPI_ResultMethOnStack_Ftab) {
      CMPIValueValueResultHandler* res=(CMPIValueValueResultHandler*)eRes->hdl;
      if (((CMPI_Result*)eRes)->flags & RESULT_set==0) {
         ((CMPI_Result*)eRes)->flags|=RESULT_set;
      }
      res->handle(v);
   }
   else {
      CMPIValueValueResultHandler* res=(CMPIValueValueResultHandler*)eRes->hdl;
      if (((CMPI_Result*)eRes)->flags & RESULT_set==0) {
         ((CMPI_Result*)eRes)->flags|=RESULT_set;
      }
      res->handle(v);
   }
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnInstance(CMPIResult* eRes, CMPIInstance* eInst)
{
	OW_CIMInstanceResultHandlerIFC * res =
		static_cast<OW_CIMInstanceResultHandlerIFC *> (eRes->hdl);
	const OW_CIMInstance& inst =
		 * (static_cast<OW_CIMInstance *>(eInst->hdl));
	try {
		std::cout << "inst to handle " << inst.toMOF() << std::endl;
   		res->handle(inst);
	}
	catch (const OW_CIMException& e)
	{
		//CMReturnWithChars(CmpiProviderBase::getBroker(),
		//		  (_CMPIrc)e.getErrNo(), e.getMessage());
		CMReturn((_CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CMReturn(CMPI_RC_ERROR);
	}
	CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnObject(CMPIResult* eRes, CMPIInstance* eInst)
{
	OW_CIMInstanceResultHandlerIFC * res =
		static_cast<OW_CIMInstanceResultHandlerIFC *> (eRes->hdl);
	const OW_CIMInstance& inst =
		* (static_cast<OW_CIMInstance *>(eInst->hdl));

	try {
		std::cout << "inst to handle " << inst.toMOF() << std::endl;
   		// TODO - turn instance into object
   		res->handle(inst);

	}
	catch (const OW_CIMException& e)
	{
		CMReturn((_CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CMReturn(CMPI_RC_ERROR);
	}
	CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnObjectPath(CMPIResult* eRes,
			 CMPIObjectPath* eRef)
{
	OW_CIMObjectPathResultHandlerIFC * res =
		static_cast<OW_CIMObjectPathResultHandlerIFC *>(eRes->hdl);
   
	const OW_CIMObjectPath& cop =
		* (static_cast<OW_CIMObjectPath *>(eRef->hdl));
	try {
		std::cout << "cop to handle " << cop.toMOF() << std::endl;
		res->handle(cop);
	}
	catch (const OW_CIMException& e)
	{
		CMReturn((_CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CMReturn(CMPI_RC_ERROR);
	}
	CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnInstDone(CMPIResult* eRes) 
{
	(void) eRes;
	CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnRefDone(CMPIResult* eRes)
{
	(void) eRes;
	CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnDataDone(CMPIResult* eRes)
{
	(void) eRes;
	CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnMethDone(CMPIResult* eRes)
{
	(void) eRes;
	CMReturn(CMPI_RC_OK);
}
static CMPIStatus resultReturnObjDone(CMPIResult* eRes)
{
	(void) eRes;
	CMReturn(CMPI_RC_OK);
}


static CMPIStatus resultBadReturnData(CMPIResult* eRes,
				 	CMPIValue* data, CMPIType type)
{
	(void) eRes;
	(void) data;
	(void) type;
	CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

static CMPIStatus resultBadReturnInstance(CMPIResult* eRes,
						 CMPIInstance* eInst)
{
	(void) eRes;
	(void) eInst;
	CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

static CMPIStatus resultBadReturnObjectPath(CMPIResult* eRes,
						 CMPIObjectPath* eRef)
{
	(void) eRes;
	(void) eRef;
	CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

static CMPIResultFT resultMethOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultReturnData,
     resultBadReturnInstance,
     resultBadReturnObjectPath,
     resultReturnMethDone,
};

static CMPIResultFT resultObjOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultBadReturnData,
     resultReturnObject,
     resultBadReturnObjectPath,
     resultReturnObjDone,
};

static CMPIResultFT resultData_FT={
     CMPICurrentVersion,
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
     resultBadReturnData,
     resultReturnInstance,
     resultBadReturnObjectPath,
     resultReturnInstDone,
};
                                                                                
static CMPIResultFT resultRefOnStack_FT={
     CMPICurrentVersion,
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
     resultBadReturnData,
     resultBadReturnInstance,
     resultBadReturnObjectPath,
     resultReturnDataDone,
};
                                                                                
//

CMPIResultFT *CMPI_ResultMeth_Ftab=&resultMethOnStack_FT;
CMPIResultFT *CMPI_ResultData_Ftab=&resultData_FT;
CMPIResultFT *CMPI_ResultMethOnStack_Ftab=&resultMethOnStack_FT;
CMPIResultFT *CMPI_ResultInstOnStack_Ftab=&resultInstOnStack_FT;
CMPIResultFT *CMPI_ResultObjOnStack_Ftab=&resultObjOnStack_FT;
CMPIResultFT *CMPI_ResultRefOnStack_Ftab=&resultRefOnStack_FT;
CMPIResultFT *CMPI_ResultResponseOnStack_Ftab=&resultResponseOnStack_FT;


CMPI_ResultOnStack::CMPI_ResultOnStack(
	const OW_CIMObjectPathResultHandlerIFC & handler)
{
	hdl = (void *)(&handler);
	ft = CMPI_ResultRefOnStack_Ftab;
	flags = RESULT_ObjectPath;
}

CMPI_ResultOnStack::CMPI_ResultOnStack(
	const OW_CIMInstanceResultHandlerIFC & handler)
{
	hdl = (void *)(&handler);
	ft = CMPI_ResultInstOnStack_Ftab;
	flags = RESULT_Instance;
}

#if 0
// ObjectResponseHandler
CMPI_ResultOnStack::CMPI_ResultOnStack(
	const OW_CIMInstanceResultHandlerIFC & handler) {
      hdl= (void *)(&handler);
      ft=CMPI_ResultInstOnStack_Ftab;
      flags=RESULT_Instance;
   }

CMPI_ResultOnStack::CMPI_ResultOnStack(
	const OW_MethodResultResultHandlerIFC & handler) {
      hdl= (void *)(&handler);
      ft=CMPI_ResultMethOnStack_Ftab;
      flags=RESULT_Method;
   }
#endif

CMPI_ResultOnStack::CMPI_ResultOnStack(const
				CMPIObjectPathValueResultHandler& handler)
{
	hdl = (void *)&handler;
	ft = CMPI_ResultResponseOnStack_Ftab;
	flags = RESULT_Response;
}

CMPI_ResultOnStack::CMPI_ResultOnStack(
				const CMPIValueValueResultHandler& handler)
{
	hdl = (void *)&handler;
	ft = CMPI_ResultResponseOnStack_Ftab;
	flags = RESULT_Response;
}

CMPI_ResultOnStack::CMPI_ResultOnStack() {
      //hdl= static_cast<void* >(&handler);
      ft=CMPI_ResultResponseOnStack_Ftab;
      flags=RESULT_Response;
}

CMPI_ResultOnStack::~CMPI_ResultOnStack() {
	std::cout << "--- ~CMPI_ResultOnStack()" << std::endl;
      //if (flags & RESULT_set==0)  ((ResponseHandler*)hdl)->processing();
      //if (flags & RESULT_done==0) ((ResponseHandler*)hdl)->complete();
  }


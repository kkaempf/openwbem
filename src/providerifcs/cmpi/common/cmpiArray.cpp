
/*
 *
 * cmpiArray.cpp
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
 * Description: CMPIArray support
 *              Data/value array support
 *
 */

#include "cmpisrv.h"

static CMPIStatus arrayRelease(CMPIArray* eArray)
{
	CMPIData *dta = (CMPIData *)eArray->hdl;
	if (dta)
	{
		if (dta->type & CMPI_ENC)
		{
			//	for (uint i=1; i<=dta->value.uint32; i++)
			//{
				//if (dta[i].state == 0)
					//((CMPIString *)dta[i].value.
					//	string)->ft->release(
					//(CMPIString*)dta[i].value.string);
			//}
		}
		delete[] dta;
		((CMPI_Object *)eArray)->unlinkAndDelete();
	}
	CMReturn(CMPI_RC_OK);
}

static CMPIArray* arrayClone(CMPIArray* eArray, CMPIStatus* rc)
{
	CMPIData * dta=(CMPIData *)eArray->hdl;
	CMPIData * nDta=new CMPIData[dta->value.uint32+1];
	CMPIArray * nArray=(CMPIArray *)new CMPI_Object(nDta, CMPI_Array_Ftab);
	CMPIStatus rrc={CMPI_RC_OK, NULL};

	if (dta->type & CMPI_ENC) for (uint i=1; i<=dta->value.uint32; i++)
		nDta[i].state=CMPI_nullValue;

	for (uint i=0; i<=dta->value.uint32; i++)
	{
		nDta[i] = dta[i];
		if (dta->type & CMPI_ENC && dta[i].state==0)
		{
			nDta[i].value.string =
				((CMPIString*)dta[i].value.string)->ft->clone(
					(CMPIString *)dta[i].value.string,&rrc);
			if (rrc.rc)
			{
				arrayRelease(nArray);
				if (rc) *rc=rrc;
				return NULL;
			}
		}
	}

	CMSetStatus(rc, CMPI_RC_OK);
	return nArray;
}

static CMPIData arrayGetElementAt(CMPIArray* eArray, CMPICount pos,
					 CMPIStatus* rc)
{
	CMPIData * dta=(CMPIData *)eArray->hdl;
	CMSetStatus(rc, CMPI_RC_OK);
	if (pos<dta->value.uint32) return dta[pos+1];

	CMPIData data={(CMPIType) 0, CMPI_nullValue, {0} };
	CMSetStatus(rc, CMPI_RC_ERR_NOT_FOUND);
	return data;
}

static CMPIStatus arraySetElementAt(CMPIArray* eArray, CMPICount pos,
					CMPIValue *val, CMPIType type)
{
	CMPIData * dta=(CMPIData *)eArray->hdl;

	if (pos<dta->value.uint32)
	{
		if ((dta->type&~CMPI_ARRAY)==type)
		{
			dta[pos+1].state=0;
			dta[pos+1].value=*val;
			CMReturn(CMPI_RC_OK);
		}
		//std::cout<<"--- arraySetElementAt(): "
		//	<< CMPI_RC_ERR_TYPE_MISMATCH is "
		//	<<(void*)(long)type << " should be "
		//	<<(void*)(long)dta->type<<std::endl;
		CMReturn(CMPI_RC_ERR_TYPE_MISMATCH);
	}
	CMReturn(CMPI_RC_ERR_NOT_FOUND);
}

static CMPICount arrayGetSize(CMPIArray* eArray, CMPIStatus* rc)
{
	CMPIData * dta=(CMPIData *)eArray->hdl;
	CMSetStatus(rc,CMPI_RC_OK);
	return dta->value.uint32;
}

static CMPIType arrayGetType(CMPIArray* eArray, CMPIStatus* rc)
{
	CMPIData * dta=(CMPIData *)eArray->hdl;
	CMSetStatus(rc,CMPI_RC_OK);
	return dta->type;
}

static CMPIArrayFT array_FT={
	CMPICurrentVersion,
	arrayRelease,
	arrayClone,
	arrayGetSize,
	arrayGetType,
	arrayGetElementAt,
	arraySetElementAt,
};

CMPIArrayFT *CMPI_Array_Ftab=&array_FT;


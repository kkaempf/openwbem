
/*
 *
 * cmpiValue.cpp
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
 * Description: value to CMPIdata and CMPIType converters
 *
 */


#include <iostream>
#include "cmpisrv.h"
#include "OW_Char16.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMDateTime.hpp"

#define CopyToArray(pt,ct) { pt##Array ar##pt(aSize); \
   for (int i=0; i<aSize; i++) ar##pt[i]=aData[i].value.ct; \
   v.set(OW_CIMValue(ar##pt)); }

#define CopyToStringArray(pt,ct) { OW_Array<pt> ar##pt(aSize); \
   for (int i=0; i<aSize; i++) ar##pt[i]=OW_String(((char*)aData[i].value)->ct)); \
   v.set(OW_CIMValue(ar##pt)); }

#define CopyToEncArray(pt,ct) { OW_Array<pt> ar##pt(aSize); \
   for (int i=0; i<aSize; i++) ar##pt[i]=*((pt*)aData[i].value.ct->hdl); \
   v.set(OW_CIMValue(ar##pt)); }

OW_CIMValue value2CIMValue(CMPIValue* data, CMPIType type, CMPIrc *rc) {
   OW_CIMValue v(OW_CIMNULL);
   if (rc) *rc=CMPI_RC_OK;

   if (type & CMPI_ARRAY) {
      CMPIArray *ar=data->array;
      CMPIData *aData=(CMPIData*)ar->hdl;
      CMPIType aType=aData->type;

      if (aType & CMPI_ARRAY) aType ^= CMPI_ARRAY;
      int aSize=aData->value.sint32;
      aData++;

      if ((aType & (CMPI_UINT|CMPI_SINT))==CMPI_SINT) {
         switch (aType) {
            case CMPI_sint32: CopyToArray(OW_Int32,sint32); break;
            case CMPI_sint16: CopyToArray(OW_Int16,sint16); break;
            case CMPI_sint8:  CopyToArray(OW_Int8,sint8);   break;
            case CMPI_sint64: CopyToArray(OW_Int64,sint64); break;
            default: ;
         }
      }

      //else if (aType==CMPI_chars)  CopyToStringArray(OW_String,chars)
      //else if (aType==CMPI_string) CopyToStringArray(OW_String,string->hdl)
      else if (aType==CMPI_chars)  {
        OW_Array<OW_String> helper(aSize); 
        for (int i=0; i<aSize; i++)
	 	helper[i]=OW_String( (char *)(aData[i].value.chars));
        v.set(OW_CIMValue(helper));
      }

      else if (aType==CMPI_string) {
        OW_Array<OW_String> helper(aSize); 
        for (int i=0; i<aSize; i++)
	 	helper[i]=OW_String((char *)(aData[i].value.string->hdl));
        v.set(OW_CIMValue(helper));
      }

      else if ((aType & (CMPI_UINT|CMPI_SINT))==CMPI_UINT) {
         switch (aType) {
            case CMPI_uint32: CopyToArray(OW_UInt32,uint32); break;
            case CMPI_uint16: CopyToArray(OW_UInt16,uint16); break;
            case CMPI_uint8:  CopyToArray(OW_UInt8,uint8);   break;
            case CMPI_uint64: CopyToArray(OW_UInt64,uint64); break;
            default: ;
         }
      }

      else switch (aType) {
         case CMPI_ref:      CopyToEncArray(OW_CIMObjectPath,ref); break;
         case CMPI_dateTime: CopyToEncArray(OW_CIMDateTime,dateTime); break;
         case CMPI_boolean:  CopyToArray(OW_Bool,boolean); break;
         case CMPI_char16:   CopyToArray(OW_Char16,char16); break;
         case CMPI_real32:   CopyToArray(OW_Real32,real32); break;
         case CMPI_real64:   CopyToArray(OW_Real64,real64); break;
         default:
         if (rc) *rc=CMPI_RC_ERR_NOT_SUPPORTED;
      }
      return OW_CIMValue(v);
   } // end of array processing

   else if ((type & (CMPI_UINT|CMPI_SINT))==CMPI_SINT) {
      switch (type) {
         //case CMPI_sint32: v.set(OW_CIMValue((OW_Int32)data->sint32)); break;
         //case CMPI_sint16: v.set(OW_CIMValue((OW_Int16)data->sint16)); break;
         //case CMPI_sint8:  v.set(OW_CIMValue((OW_Int8)data->sint8));   break;
         //case CMPI_sint64: v.set(OW_CIMValue((OW_Int64)data->sint64)); break;
         case CMPI_sint32: v.set(OW_CIMValue((OW_Int32)data->sint32)); break;
         case CMPI_sint16: v.set(OW_CIMValue((OW_Int16)data->sint16)); break;
         case CMPI_sint8:  v.set(OW_CIMValue((OW_Int8)data->sint8));   break;
         case CMPI_sint64: v.set(OW_CIMValue((OW_Int64)data->sint64)); break;
         default: ;
      }
   }

   //else if (type==CMPI_chars)  v.set(OW_CIMValue(OW_String(data->chars)));
   else if (type==CMPI_string) v.set(OW_CIMValue(OW_String((char*)data->string->hdl)));
   else if (type==CMPI_chars)  v.set(OW_CIMValue(OW_String((char *)data)));

   else if ((type & (CMPI_UINT|CMPI_SINT))==CMPI_UINT) {
      switch (type) {
         case CMPI_uint32: v.set(OW_CIMValue((OW_UInt32)data->sint32)); break;
         case CMPI_uint16: v.set(OW_CIMValue((OW_UInt16)data->sint16)); break;
         case CMPI_uint8:  v.set(OW_CIMValue((OW_UInt8)data->sint8));   break;
         case CMPI_uint64: v.set(OW_CIMValue((OW_UInt64)data->sint64)); break;
         default: ;
      }
   }

   else switch (type) {
      case CMPI_ref:      v.set(OW_CIMValue(*((OW_CIMObjectPath*)data->ref->hdl))); break;
      case CMPI_dateTime: v.set(OW_CIMValue(*((OW_CIMDateTime*)data->dateTime->hdl))); break;
      case CMPI_boolean:  v.set(OW_CIMValue((OW_Bool&)data->boolean)); break;
      case CMPI_char16:   v.set(OW_CIMValue((OW_Char16)data->char16)); break;
      case CMPI_real32:   v.set(OW_CIMValue((OW_Real32)data->real32)); break;
      case CMPI_real64:   v.set(OW_CIMValue((OW_Real64)data->real64)); break;
      default:
      if (rc) *rc=CMPI_RC_ERR_NOT_SUPPORTED;
   }
   //return OW_CIMValue(v);
   return v;
}



#define CopyFromArray(pt,ct) { OW_Array<pt> ar##pt; \
   v.get(ar##pt); \
   for (int i=0; i<aSize; i++) aData[i].value.ct=ar##pt[i]; }

#define CopyFromStringArray(pt,ct) { OW_Array<pt> ar##pt; \
   v.get(ar##pt); \
   for (int i=0; i<aSize; i++) { \
      aData[i].value.ct=(CMPIString*)new CMPI_Object(ar##pt[i]); } }

#define CopyFromEncArray(pt,ct,cn) { OW_Array<pt> ar##pt; \
   v.get(ar##pt); \
   for (int i=0; i<aSize; i++) { \
     aData[i].value.cn=(ct*)new CMPI_Object(new pt(ar##pt[i])); } }


CMPIrc value2CMPIData(const OW_CIMValue& v, CMPIType t, CMPIData *data) {

   data->type=t;
   data->state=0;

   if (t & CMPI_ARRAY) {
      int aSize=v.getArraySize();
      CMPIType aType=t&~CMPI_ARRAY;
      CMPIData *aData=new CMPIData[aSize+1];
      aData->type=aType;
      aData->value.sint32=aSize;
      aData++;

      if (aType & (CMPI_UINT|CMPI_SINT)==CMPI_SINT) {
         switch (aType) {
            case CMPI_sint32: CopyFromArray(OW_Int32,sint32); break;
            case CMPI_sint16: CopyFromArray(OW_Int16,sint16); break;
            case CMPI_sint8:  CopyFromArray(OW_Int8,sint8);   break;
            case CMPI_sint64: CopyFromArray(OW_Int64,sint64); break;
            default: ;
         }
      }
      else if (aType==CMPI_string) CopyFromStringArray(OW_String,string)
      else if (aType & (CMPI_UINT|CMPI_SINT)==CMPI_UINT) {
         switch (aType) {
            case CMPI_uint32: CopyFromArray(OW_UInt32,uint32); break;
            case CMPI_uint16: CopyFromArray(OW_UInt16,uint16); break;
            case CMPI_uint8:  CopyFromArray(OW_UInt8,uint8);   break;
            case CMPI_uint64: CopyFromArray(OW_UInt64,uint64); break;
            default: ;
         }
      }
      else switch (aType) {
         case CMPI_ref:      CopyFromEncArray(OW_CIMObjectPath,CMPIObjectPath,ref); break;
         case CMPI_dateTime: CopyFromEncArray(OW_CIMDateTime,CMPIDateTime,dateTime); break;
         case CMPI_boolean:  CopyFromArray(OW_Bool,boolean); break;
         case CMPI_char16:   CopyFromArray(OW_Char16,char16); break;
         case CMPI_real32:   CopyFromArray(OW_Real32,real32); break;
         case CMPI_real64:   CopyFromArray(OW_Real64,real64); break;
         default:
         return CMPI_RC_ERR_NOT_SUPPORTED;
      }
      data->value.array=(CMPIArray*)new CMPI_Object(aData-1);
   }  // end of array porocessing

   else if (t & (CMPI_UINT|CMPI_SINT)==CMPI_SINT) {
      switch (t) {
         case CMPI_sint32: v.get((OW_Int32&)data->value.sint32); break;
         case CMPI_sint16: v.get((OW_Int16&)data->value.sint16); break;
         case CMPI_sint8:  v.get((OW_Int8&)data->value.sint8);   break;
         case CMPI_sint64: v.get((OW_Int64&)data->value.sint64); break;
         default: ;
      }
   }

   else if (t==CMPI_string) {
      OW_String str;
      v.get(str);
std::cout << " string is " << v << std::endl;
      data->value.string=string2CMPIString(str);
   }

   else if (t & (CMPI_UINT|CMPI_SINT)==CMPI_UINT) {
      switch (t) {
         case CMPI_uint32: v.get((OW_UInt32&)data->value.uint32); break;
         case CMPI_uint16: v.get((OW_UInt16&)data->value.uint16); break;
         case CMPI_uint8:  v.get((OW_UInt8&)data->value.uint8);   break;
         case CMPI_uint64: v.get((OW_UInt64&)data->value.uint64); break;
         default: ;
      }
   }

   else switch (t) {
   case CMPI_ref: {
         OW_CIMObjectPath ref;
         v.get(ref);
         data->value.ref=(CMPIObjectPath*)new CMPI_Object(new OW_CIMObjectPath(ref));
      }
   case CMPI_dateTime: {
         OW_CIMDateTime dt;
         v.get(dt);
         data->value.dateTime=(CMPIDateTime*)new CMPI_Object(new OW_CIMDateTime(dt));
      }
   case CMPI_boolean:      v.get((OW_Bool&)data->value.boolean); break;
   case CMPI_char16:       v.get((OW_Char16&)data->value.char16); break;
   case CMPI_real32:       v.get((OW_Real32&)data->value.real32); break;
   case CMPI_real64:       v.get((OW_Real64&)data->value.real64); break;
   default:
      return CMPI_RC_ERR_NOT_SUPPORTED;
   }
   return CMPI_RC_OK;
}

CMPIType type2CMPIType(OW_CIMDataType pt, int array) {
   static CMPIType types[]={
	CMPI_uint8,	 // NULL,
        CMPI_uint8,      // UINT8,
        CMPI_sint8,      // SINT8,
        CMPI_uint16,     // UINT16,
        CMPI_sint16,     // SINT16,
        CMPI_uint32,     // UINT32,
        CMPI_sint32,     // SINT32,
        CMPI_uint64,     // UINT64,
        CMPI_sint64,     // SINT64,
        CMPI_string,     // STRING,
        CMPI_boolean,    // BOOLEAN,
        CMPI_real32,     // REAL32,
        CMPI_real64,     // REAL64,
        CMPI_dateTime,   // DATETIME,
        CMPI_char16,     // CHAR16,
        CMPI_ref,        // REFERENCE
    };
    int t=types[pt.getType()];
    if (array) t|=CMPI_ARRAY;
    return (CMPIType)t;
}

CMPIrc key2CMPIData(const OW_String& v, OW_CIMDataType t, CMPIData *data) {
   data->state=CMPI_keyValue;
   //case KeyBinding::NUMERIC: {
   if (t < OW_CIMDataType::STRING)
   {
         const char *vp=v.c_str();
         data->value.sint64=atoll(vp);
         data->type=CMPI_sint64;
         //delete vp;
   }
   //case KeyBinding::STRING:
   else if (t.getType() == OW_CIMDataType::STRING)
   {
      data->value.string=string2CMPIString(v);
      data->type=CMPI_string;
   }
   //case KeyBinding::BOOLEAN:
   else if (t.getType() == OW_CIMDataType::BOOLEAN)
   {
      data->value.boolean=(v.compareToIgnoreCase("true"));
      data->type=CMPI_boolean;
   }
   //case KeyBinding::REFERENCE:
   else if (t.getType() == OW_CIMDataType::BOOLEAN)
   {
      data->value.ref=(CMPIObjectPath*)new CMPI_Object(new OW_CIMObjectPath(v));
      data->type=CMPI_ref;
   }
   else
   {
      return CMPI_RC_ERR_NOT_SUPPORTED;
   }
   return CMPI_RC_OK;
}

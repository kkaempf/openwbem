
/*
 *
 * CmpiData.h
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
 * Description: CMPI C++ helper class
 *
 */

#ifndef _CmpiData_h_
#define _CmpiData_h_
#include "OW_config.h"

#include "cmpidt.h"
//#include "CmpiString.h"
#include "CmpiString.h"

class CmpiArray;
class CmpiInstance;
class CmpiObjectPath;

/** This class wraps a CMPIData value item. Data extraction uses the >> operator.
      Extraction operations can be appended to a property type retrieval statement
      like this:

        CmpiString name;

        cop.getKey("DeviceID")>>name;

       Type mismatches will be signalled by exceptions.
*/
class CmpiData {
   friend class CmpiInstance;
   friend class CmpiObjectPath;
   friend class CmpiResult;
   friend class CmpiArgs;
   friend class CmpiContext;
   friend class CmpiPropertyMI;
  protected:

   CMPIData data;

  public:
   /** Constructor - Empty constructor.
   */
   CmpiData(CMPIData& data)
      { this->data=data; }

   /** CmpiData actually is a CMPIData struct.
   */
//  public:

   /** Constructor - Empty constructor.
   */
   CmpiData() {}

   /** Constructor - String as input.
   */
   inline CmpiData(CmpiString& d)
      { data.value.chars=(char*)d.charPtr(); data.type=CMPI_chars;}

   /** Constructor - char* as input.
   */
   inline CmpiData(const char* d)
      { data.value.chars=(char*)d;  data.type=CMPI_chars; }

   /** Constructor - singed 8 bit as input.
   */
   inline CmpiData(CMPISint8 d)
      { data.value.sint8=d;  data.type=CMPI_sint8; }
   /** Constructor - singed 16 bit as input.
   */
   inline CmpiData(CMPISint16 d)
      { data.value.sint16=d; data.type=CMPI_sint16;}
   /** Constructor - singed 32 bit as input.
   */
   inline CmpiData(CMPISint32 d)
      { data.value.sint32=d; data.type=CMPI_sint32;}
   /** Constructor - singed 64 bit as input.
   */
   inline CmpiData(CMPISint64 d)
      { data.value.sint64=d; data.type=CMPI_sint64;}

   /** Constructor - unsinged 8 bit as input.
   */
   inline CmpiData(CMPIBoolean d)
      { data.value.boolean=d;  data.type=CMPI_boolean;}
   /** Constructor - unsinged 16 bit as input.
   */
   inline CmpiData(CMPIUint16 d)
      { data.value.sint16=d; data.type=CMPI_uint16;}
   /** Constructor - unsinged 32 bit as input.
   */
   inline CmpiData(CMPIUint32 d)
      { data.value.sint32=d; data.type=CMPI_uint32;}
   /** Constructor - unsinged 64 bit as input.
   */
   inline CmpiData(CMPIUint64 d)
      { data.value.sint64=d; data.type=CMPI_uint64;}
      
   /** Constructor - CmpiInstance as input.
   */
   inline CmpiData(CMPIInstance* d)
      { data.value.inst = d; data.type=CMPI_instance;}
      
   /** Constructor - CmpiObjectPath as input.
   */
   inline CmpiData(CMPIObjectPath* d)
      { data.value.ref = d; data.type=CMPI_ref;}
      
  /** Constructor - CmpiArray as input.
   */
   inline CmpiData(CMPIArray* d)
      { data.value.array = d; data.type=CMPI_ARRAY;}


   /** Extracting String.
   */
   inline void operator>>(CmpiString& v) const
      { if (data.type!=CMPI_string) throw CMPI_RC_ERR_TYPE_MISMATCH;
        else v=CmpiString(data.value.string);
      }

   /** Extracting signed 8 bit.
   */
   inline void operator>>(CMPISint8& v)
      { if (data.type!=CMPI_sint8) throw CMPI_RC_ERR_TYPE_MISMATCH;
        else v=data.value.sint8;
      }
   /** Extracting signed 16 bit.
   */
   inline void operator>>(CMPISint16& v)
      { if (data.type!=CMPI_sint16) throw CMPI_RC_ERR_TYPE_MISMATCH;
        else v=data.value.sint16;
      }
   /** Extracting signed 32 bit.
   */
   inline void operator>>(CMPISint32& v)
      { if (data.type!=CMPI_sint32) throw CMPI_RC_ERR_TYPE_MISMATCH;
        else v=data.value.sint32;
      }
   /** Extracting signed 64 bit.
   */
   inline void operator>>(CMPISint64& v)
      { if (data.type!=CMPI_sint64) throw CMPI_RC_ERR_TYPE_MISMATCH;
        else v=data.value.sint64;
      }

   /** Extracting unsigned 8 bit.
   */
   inline void operator>>(CMPIUint8& v)
      { if (data.type!=CMPI_uint8) throw CMPI_RC_ERR_TYPE_MISMATCH;
        else v=data.value.uint8;
      }
   /** Extracting unsigned 16 bit.
   */
   inline void operator>>(CMPIUint16& v)
      { if (data.type!=CMPI_uint16) throw CMPI_RC_ERR_TYPE_MISMATCH;
        else v=data.value.uint16;
      }
   /** Extracting unsigned 32 bit.
   */
   inline void operator>>(CMPIUint32& v)
      { if (data.type!=CMPI_uint32) throw CMPI_RC_ERR_TYPE_MISMATCH;
        else v=data.value.uint32;
      }
   /** Extracting unsigned 64 bit.
   */
   inline void operator>>(CMPIUint64& v)
      { if (data.type!=CMPI_uint64) throw CMPI_RC_ERR_TYPE_MISMATCH;
        else v=data.value.uint64;
      }

   /** Extracting Array.
   */
   void operator>>(CmpiArray& v);

   /** Extracting Instance.
   */
   void operator>>(CmpiInstance& v);

   /** Extracting ObjectPath.
   */
   void operator>>(CmpiObjectPath& v);

   bool operator==(const CmpiData& d);

   // check null value
   bool isNull() {return data.state == CMPI_nullValue;}  
};

#endif




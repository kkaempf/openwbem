/*
 *
 * CmpiStatus.h
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
 * Description: CMPI C++ magement Status wrapper
 *
 */

#ifndef _CmpiStatus_h_
#define _CmpiStatus_h_
#include "OW_config.h"

#include "cmpidt.h"
#include "cmpift.h"

#include "CmpiProviderBase.h"

/** This class represents the status of a provider function invocation.
*/

class CmpiStatus {
   friend class CmpiInstanceMI;
   friend class CmpiMethodMI;
   friend class CmpiBaseMI;
   friend class CmpiAssociationMI;
   friend class CmpiPropertyMI;
   friend class CmpiIndicationMI;
  protected:

   /** CmpiStatus actually is a CMPIStatus struct.
   */
   CMPIStatus st;

   /** status - Returns CMPIStatus struct, to be used by MI drivers only.
   */
   inline CMPIStatus status() const
      { return st; }
  private:

   /** Constructor - not to be used.
   */
   CmpiStatus();
  public:

   /** Constructor - set rc only.
       @param rc the return code.
   */
   CmpiStatus(const CMPIrc rc);

   /** Constructor - set rc and message.
       @param rc The return code.
       @param msg Descriptive message.
   */
   inline CmpiStatus(const CMPIrc rcp, const char *msg) {
     st.rc=rcp;
     st.msg=CMNewString(CmpiProviderBase::getBroker(),(char*)msg,NULL);
   }
  /*   CmpiStatus(const CMPIrc rc, const char *msg); */
   /** rc - get the rc value.
   */
   inline CMPIrc rc() const
      { return st.rc; }

   /** msg - get the msg component.
   */
   inline const char*  msg()
      { return CMGetCharPtr(st.msg); }
};

#endif




/*
 *
 * CmpiStatus.h
 *
 * Copyright (c) 2003, International Business Machines
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

#include "cmpidt.h"
#include "cmpift.h"

extern CMPIBroker* CmpiBaseMIBroker;

/** This class represents the status of a provider function invocation.
*/

class CmpiStatus {
   friend class CmpiInstanceMIDriver;
   friend class CmpiMethodMIDriver;
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
   CmpiStatus(const CMPIrc rc, const char *msg);

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




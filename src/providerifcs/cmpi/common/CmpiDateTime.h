
/*
 *
 * CmpiDateTime.h
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
 * Description: CMPI C++ DatetTime wrapper
 *
 */

#ifndef _CmpiDateTime_h_
#define _CmpiDateTime_h_
#include "OW_config.h"

#include "cmpidt.h"
#include "cmpift.h"

#include "CmpiObject.h"
#include "CmpiStatus.h"
#include "CmpiBaseMI.h"


/** This class represents the instance of a CIM DateTime artifact.
*/

class CmpiDateTime : public CmpiObject {
  protected:

   /** Protected constructor used internally.
   */
   inline CmpiDateTime(const CMPIDateTime* enc)
      { this->enc=(void*)enc; }

   /** Gets the encapsulated CMPIDateTime.
   */
   inline CMPIDateTime *getEnc() const
      { return (CMPIDateTime*)enc; }
   void *CmpiDateTime::makeDateTime(CMPIBroker *mb);
   void *CmpiDateTime::makeDateTime(CMPIBroker *mb, const char* utcTime);
   void *CmpiDateTime::makeDateTime(CMPIBroker *mb, const CMPIUint64 binTime,
                                 const CmpiBoolean interval);
  private:
  public:

   /** Constructor - Return current time and date.
   */
   inline CmpiDateTime() {
      enc=makeDateTime(CmpiProviderBase::getBroker());
   }

   /** Constructor - Initialize time and date according to <utcTime>.
	@param utcTime Date/Time definition in UTC format
   */
   inline CmpiDateTime(const char* utcTime) {
      enc=makeDateTime(CmpiProviderBase::getBroker(),utcTime);
   }

   /** Constructor - Initialize time and date according to <binTime>.
	@param binTime Date/Time definition in binary format in microsecods
	       starting since 00:00:00 GMT, Jan 1,1970.
 	@param interval defines Date/Time definition to be an interval value
   */
   inline CmpiDateTime(const CMPIUint64 binTime, const CmpiBoolean interval) {
      enc=makeDateTime(CmpiProviderBase::getBroker(),binTime,interval);
   }

    /**	isInterval - Tests whether time is an interval value.
	@return	CmpiBoolean true when time is an interval value.
    */
   CmpiBoolean isInterval();


    /**	getDateTime - Returns DateTime in binary format.
	@return	Uint64 DateTime in binary.
    */
   CMPIUint64 getDateTime();
};

#endif

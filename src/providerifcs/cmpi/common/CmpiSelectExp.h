
/*
 *
 * CmpiSelectExp.h
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
 * Description: CMPI C++ SelectExp wrapper
 *
 */

#ifndef _CmpiSelectExp_h_
#define _CmpiSelectExp_h_

#include "OW_config.h"

#include "cmpidt.h"
#include "cmpift.h"

/*#include "CmpiImpl.h" */

#include "CmpiObject.h"


/** This class represents the SelectExp of a CIM class. It is used manipulate
    SelectExps and their parts.
*/

class CmpiSelectExp : public CmpiObject {
   friend class CmpiBroker;
   friend class CmpiResult;
   friend class CmpiIndicationMI;
  protected:

   /** Protected constructor used by MIDrivers to encapsulate CMPISelectExp.
   */
   inline CmpiSelectExp(const CMPISelectExp* enc)
      { this->enc=(void*)enc; }

   /** Gets the encapsulated CMPISelectExp.
   */
   inline CMPISelectExp *getEnc() const
      { return (CMPISelectExp*)enc; }
   private:

   /** Constructor - Should not be called
   */
   CmpiSelectExp() {}
  public:
};

#endif

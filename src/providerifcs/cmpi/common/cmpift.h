
/*
 *
 * cmpift.h
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
 * Description: CMPI Encapsulated types Function Tables
 *
 */

#ifndef _CMPIFT_H_
#define _CMPIFT_H_

#include <cmpidt.h>
#include <cmpimacs.h>

#ifdef __cplusplus
extern "C" {
#endif

   struct _CMPIBrokerEncFT {
     int ftVersion;
     CMPIInstance* (*newInstance)
                 (CMPIBroker*,CMPIObjectPath*,CMPIStatus*);
     CMPIObjectPath* (*newObjectPath)
                 (CMPIBroker*,char*,char*,CMPIStatus*);
     CMPIArgs* (*newArgs)
                 (CMPIBroker*,CMPIStatus*);
     CMPIString* (*newString)
                 (CMPIBroker*,char*,CMPIStatus*);
     CMPIArray* (*newArray)
                 (CMPIBroker*,CMPICount,CMPIType,CMPIStatus*);
     CMPIDateTime* (*newDateTime)
                 (CMPIBroker*,CMPIStatus*);
     CMPIDateTime* (*newDateTimeFromBinary)
                 (CMPIBroker*,CMPIUint64,CMPIBoolean,CMPIStatus*);
     CMPIDateTime* (*newDateTimeFromChars)
                 (CMPIBroker*,char*,CMPIStatus*);
     CMPISelectExp* (*newSelectExp)
                 (CMPIBroker*,char*,char*,
                  CMPIArray**,CMPIStatus*);
     CMPIAdapter* (*loadMI)
                 (CMPIBroker*,char*,CMPIStatus*);

     CMPIString* (*toString)
                 (CMPIBroker*,void*,CMPIStatus*);
     CMPIBoolean (*isOfType)
                 (CMPIBroker*,void*,char*,CMPIStatus*);
     CMPIString* (*getType)
                 (CMPIBroker*,void*,CMPIStatus*);
   };

   struct _CMPIBrokerFT {
     unsigned long brokerClassification;
     int brokerVersion;
     char *brokerName;
     CMPIContext* (*prepareAttachThread)
                (CMPIBroker*,CMPIContext*);
     CMPIStatus (*attachThread)
                (CMPIBroker*,CMPIContext*);
     CMPIStatus (*detachThread)
                (CMPIBroker*,CMPIContext*);

     // class 0 services
     CMPIStatus (*deliverIndication)
                (CMPIBroker*,CMPIContext*,
                 char*,CMPIInstance*);
     // class 1 services
     CMPIEnumeration* (*enumInstanceNames)
                (CMPIBroker*,CMPIContext*,
                 CMPIObjectPath*,CMPIStatus*);
     CMPIInstance* (*getInstance)
                (CMPIBroker*,CMPIContext*,
                 CMPIObjectPath*,char**,CMPIStatus*);
     // class 2 services
     CMPIObjectPath* (*createInstance)
                (CMPIBroker*,CMPIContext*,
                 CMPIInstance*,CMPIStatus*);
     CMPIStatus (*setInstance)
                (CMPIBroker*,CMPIContext*,CMPIInstance*);
     CMPIStatus (*deleteInstance)
                (CMPIBroker*,CMPIContext*,
                 CMPIObjectPath*);
     CMPIEnumeration* (*execQuery)
                (CMPIBroker*,CMPIContext*,
                 CMPIObjectPath*,char*,char*,CMPIStatus*);
     CMPIEnumeration* (*enumInstances)
                (CMPIBroker*,CMPIContext*,
                 CMPIObjectPath*,char**,CMPIStatus*);
     CMPIEnumeration* (*associators)
                (CMPIBroker*,CMPIContext*,
                 CMPIObjectPath*,char*,char*,char*,char*,char**,
                 CMPIStatus*);
     CMPIEnumeration* (*associatorNames)
                (CMPIBroker*,CMPIContext*,
                 CMPIObjectPath*,char*,char*,char*,char*,
                 CMPIStatus*);
     CMPIEnumeration* (*references)
                (CMPIBroker*,CMPIContext*,
                 CMPIObjectPath*,char*,char*,char**,
                 CMPIStatus*);
     CMPIEnumeration* (*referenceNames)
                (CMPIBroker*,CMPIContext*,
                 CMPIObjectPath*,char*,char*,
                 CMPIStatus*);
     CMPIData (*invokeMethod)
                (CMPIBroker*,CMPIContext*,
                 CMPIObjectPath*,char*,CMPIArgs*,CMPIArgs*,CMPIStatus*);
     CMPIStatus (*setProperty)
                (CMPIBroker*,CMPIContext*,
                 CMPIObjectPath*,char*,CMPIValue*,
                 CMPIType);
     CMPIData (*getProperty)
                (CMPIBroker*,CMPIContext*,
                 CMPIObjectPath*,char*,CMPIStatus*);
   };

   struct _CMPIBroker {
     void *hdl;
     CMPIBrokerFT *bft;
     CMPIBrokerEncFT *eft;
   };



  struct _CMPIContextFT {
     int ftVersion;
     CMPIStatus (*release)
              (CMPIContext*);
     CMPIContext* (*clone)
              (CMPIContext*,CMPIStatus*);
     void*  (*reserved1)
              (CMPIContext*,CMPIStatus*);
     CMPIData (*getEntry)
              (CMPIContext*,char*,CMPIStatus*);
     CMPIData (*getEntryAt)
              (CMPIContext*,unsigned int,CMPIString**,CMPIStatus*);
     unsigned int (*getEntryCount)
              (CMPIContext*,CMPIStatus*);
     CMPIStatus (*addEntry)
              (CMPIContext*,char*,CMPIValue*,CMPIType);
  };

  struct _CMPIContext {
    void *hdl;
    CMPIContextFT *ft;
  };



  struct _CMPIResult {
     void *hdl;
     CMPIResultFT *ft;
  };

  struct _CMPIResultFT {
     int ftVersion;
     CMPIStatus (*release)
              (CMPIResult*);
     CMPIResult* (*clone)
              (CMPIResult*,CMPIStatus*);
     void* (*reserved1)
              (CMPIResult*,CMPIStatus*);
     CMPIStatus (*returnData)
              (CMPIResult*,CMPIValue*,CMPIType);
     CMPIStatus (*returnInstance)
              (CMPIResult*,CMPIInstance*);
     CMPIStatus (*returnObjectPath)
              (CMPIResult* eRes, CMPIObjectPath* eRef);
     CMPIStatus (*returnDone)
              (CMPIResult*);
  };


   struct _CMPIInstance {
     void *hdl;
     CMPIInstanceFT* ft;
};

   struct _CMPIInstanceFT {
     int ftVersion;
     CMPIStatus (*release)
              (CMPIInstance*);
     CMPIInstance* (*clone)
              (CMPIInstance*,CMPIStatus*);
     void* (*reserved1)
              (CMPIInstance*,CMPIStatus*);
     CMPIData (*getProperty)
              (CMPIInstance*,char*,CMPIStatus*);
     CMPIData (*getPropertyAt)
              (CMPIInstance*,unsigned int,CMPIString**,CMPIStatus*);
     unsigned int (*getPropertyCount)
              (CMPIInstance*,CMPIStatus*);
     CMPIStatus (*setProperty)
              (CMPIInstance*,char*,
               CMPIValue*,CMPIType);
     CMPIObjectPath* (*getObjectPath)
              (CMPIInstance*,CMPIStatus*);
   };


   struct _CMPIObjectPath {
     void *hdl;
     CMPIObjectPathFT* ft;
     #ifdef __cplusplusb
     #endif
   };

   struct _CMPIObjectPathFT {
     int ftVersion;
     CMPIStatus (*release)
              (CMPIObjectPath*);
     CMPIObjectPath* (*clone)
              (CMPIObjectPath*,CMPIStatus*);
     void* (*reserved1)
              (CMPIObjectPath*,CMPIStatus*);
     CMPIStatus (*setNameSpace)
              (CMPIObjectPath*,char*);
     CMPIString* (*getNameSpace)
              (CMPIObjectPath*,CMPIStatus*);
     CMPIStatus (*setClassName)
              (CMPIObjectPath*,char*);
     CMPIString* (*getClassName)
              (CMPIObjectPath*,CMPIStatus*);
     CMPIStatus (*addKey)
              (CMPIObjectPath*,char*,
               CMPIValue*,CMPIType);
     CMPIData (*getKey)
              (CMPIObjectPath*,char*,CMPIStatus*);
     CMPIData (*getKeyAt)
              (CMPIObjectPath*,unsigned int,CMPIString**,CMPIStatus*);
     unsigned int (*getKeyCount)
              (CMPIObjectPath*,CMPIStatus*);
     CMPIStatus (*setNameSpaceFromObjectPath)
              (CMPIObjectPath* opThis,
               CMPIObjectPath* src);
     CMPIBoolean (*classPathIsA)
              (CMPIObjectPath*,char*,CMPIStatus*);

     CMPIData (*getClassQualifier)         /* optional qualifier support */
              (CMPIObjectPath* opThis,
               char *qualifierName,
               CMPIStatus *rc);
     CMPIData (*getPropertyQualifier)
              (CMPIObjectPath* opThis,
               char *propertyName,
               CMPIStatus *rc);
     CMPIData (*getMethodQualifier)
              (CMPIObjectPath* opThis,
               char *methodName,
               CMPIStatus *rc);
     CMPIData (*getParameterQualifier)
              (CMPIObjectPath* opThis,
               char *methodName,
               char *paremeterName,
               CMPIStatus *rc);
   };



   struct _CMPISelectExp {
     void *hdl;
     CMPISelectExpFT* ft;
   };

   struct _CMPISelectExpFT {
     int ftVersion;
     CMPIStatus (*release)
              (CMPISelectExp*);
     CMPISelectExp* (*clone)
              (CMPISelectExp*,CMPIStatus*);
     void* (*reserved1)
              (CMPISelectExp*,CMPIStatus*);
     CMPIBoolean (*evaluate)
              (CMPISelectExp*,CMPIInstance*,CMPIStatus*);
     CMPIString* (*getString)
              (CMPISelectExp*,CMPIStatus*);
     CMPISelectCond* (*getDOC)
              (CMPISelectExp*,CMPIStatus*);
     CMPISelectCond* (*getCOD)
              (CMPISelectExp*,CMPIStatus*);
   };

   struct _CMPISelectCond {
     void *hdl;
     CMPISelectCondFT* ft;
   };

   struct _CMPISelectCondFT {
     int ftVersion;
     CMPIStatus (*release)
             (CMPISelectCond*);
     CMPISelectCond* (*clone)
              (CMPISelectCond*,CMPIStatus*);
     void* (*reserved1)
              (CMPISelectCond*,CMPIStatus*);
     CMPICount (*getCountAndType)
              (CMPISelectCond*,int*,CMPIStatus*);
     CMPISubCond* (*getSubCondAt)
              (CMPISelectCond*,unsigned int,CMPIStatus*);
   };


   struct _CMPISubCond {
     void *hdl;
     CMPISubCondFT* ft;
   };

   struct _CMPISubCondFT {
     int ftVersion;
     CMPIStatus (*release)
             (CMPISubCond*);
     CMPISubCond* (*clone)
              (CMPISubCond*,CMPIStatus*);
     void* (*reserved1)
              (CMPISubCond*,CMPIStatus*);
     CMPICount (*getCount)
              (CMPISubCond*,CMPIStatus*);
     CMPIPredicate* (*getPredicateAt)
              (CMPISubCond*,unsigned int,CMPIStatus*);
     CMPIPredicate* (*getPredicate)
              (CMPISubCond*,char*,CMPIStatus*);
   };

  struct _CMPIPredicate {
     void *hdl;
     CMPIPredicateFT* ft;
   };

   struct _CMPIPredicateFT {
     int ftVersion;
     CMPIStatus (*release)
             (CMPIPredicate*);
     CMPIPredicate* (*clone)
              (CMPIPredicate*,CMPIStatus*);
     void* (*reserved1)
              (CMPIPredicate*,CMPIStatus*);
     CMPIStatus (*getData)
              (CMPIPredicate*,CMPIType*,
               CMPIPredOp*,CMPIString**,CMPIString**);
     int (*evaluate)
              (CMPIPredicate*,CMPIValue*,
               CMPIType,CMPIStatus*);
   };


   struct _CMPIArgs {
     void *hdl;
     CMPIArgsFT* ft;
   };

   struct _CMPIArgsFT{
     int ftVersion;
     CMPIStatus (*release)
              (CMPIArgs*);
     CMPIArgs* (*clone)
              (CMPIArgs*,CMPIStatus*);
     void* (*reserved1)
              (CMPIArgs*,CMPIStatus*);
     CMPIStatus (*addArg)
              (CMPIArgs*,char*,CMPIValue*,
               CMPIType);
     CMPIData (*getArg)
              (CMPIArgs*,char*,CMPIStatus*);
     CMPIData (*getArgAt)
              (CMPIArgs*,unsigned int,CMPIString**,CMPIStatus*);
     unsigned int (*getArgCount)
              (CMPIArgs*,CMPIStatus*);
   };


   struct _CMPIString {
     void *hdl;
     CMPIStringFT* ft;
   };

   struct _CMPIStringFT {
     int ftVersion;
     CMPIStatus (*release)
             (CMPIString*);
     CMPIString* (*clone)
             (CMPIString*,CMPIStatus*);
     void* (*reserved1)
              (CMPIString*,CMPIStatus*);
     char* (*getCharPtr)
             (CMPIString*,CMPIStatus*);
   };

   struct _CMPIArray {
     void *hdl;
     CMPIArrayFT* ft;
   };

   struct _CMPIArrayFT {
     int ftVersion;
     CMPIStatus (*release)
             (CMPIArray*);
     CMPIArray* (*clone)
             (CMPIArray*,CMPIStatus*);
     void* (*reserved1)
              (CMPIArray*,CMPIStatus*);
     CMPICount (*getSize)
             (CMPIArray*,CMPIStatus*);
     CMPIType (*getSimpleType)
             (CMPIArray*,CMPIStatus*);
     CMPIData (*getElementAt)
             (CMPIArray*,CMPICount,CMPIStatus*);
     CMPIStatus (*setElementAt)
             (CMPIArray*,CMPICount,CMPIValue*,CMPIType);
   };


   struct _CMPIEnumeration {
     void *hdl;
     CMPIEnumerationFT* ft;
   };

   struct _CMPIEnumerationFT {
     int ftVersion;
     CMPIStatus (*release)
             (CMPIEnumeration*);
     CMPIEnumeration* (*clone)
             (CMPIEnumeration*,CMPIStatus*);
     CMPIStatus (*reserved1)
              (CMPIEnumeration*,CMPIEnumeration*,CMPIStatus*);
     CMPIData (*getNext)
             (CMPIEnumeration*,CMPIStatus*);
     CMPIBoolean (*hasNext)
             (CMPIEnumeration*,CMPIStatus*);
     CMPIArray* (*toArray)
             (CMPIEnumeration*,CMPIStatus*);
  };

  struct _CMPIDateTime {
     void *hdl;
     CMPIDateTimeFT *ft;
   };

   struct _CMPIDateTimeFT {
     int ftVersion;
     CMPIStatus (*release)
             (CMPIDateTime*);
     CMPIDateTime* (*clone)
             (CMPIDateTime*,CMPIStatus*);
     void* (*reserved1)
              (CMPIDateTime*,CMPIStatus*);
     CMPIUint64 (*getBinaryFormat)
             (CMPIDateTime*,CMPIStatus*);
     CMPIString* (*getStringFormat)
             (CMPIDateTime*,CMPIStatus*);
     CMPIBoolean (*isInterval)
              (CMPIDateTime*,CMPIStatus*);
  };


   typedef struct _CMPIInstanceMIFT CMPIInstanceMIFT;
   typedef struct _CMPIInstanceMI {
      void *hdl;
      CMPIInstanceMIFT *ft;
      //void *cimom;
   } CMPIInstanceMI;

   struct _CMPIInstanceMIFT {
     int ftVersion;
     int miVersion;
     char *miName;
     CMPIStatus (*cleanup)
            (CMPIInstanceMI*,CMPIContext*);
     CMPIStatus (*enumInstanceNames)
             (CMPIInstanceMI*,CMPIContext*,CMPIResult*,
              CMPIObjectPath*);
     CMPIStatus (*enumInstances)
             (CMPIInstanceMI*,CMPIContext*,CMPIResult*,
              CMPIObjectPath*,char**);
     CMPIStatus (*getInstance)
             (CMPIInstanceMI*,CMPIContext*,CMPIResult*,
              CMPIObjectPath*,char**);
     CMPIStatus (*createInstance)
             (CMPIInstanceMI*,CMPIContext*,CMPIResult*,
              CMPIObjectPath*,CMPIInstance*);
     CMPIStatus (*setInstance)
             (CMPIInstanceMI*,CMPIContext*,CMPIResult*,
              CMPIObjectPath*,CMPIInstance*,char**);
     CMPIStatus (*deleteInstance)
             (CMPIInstanceMI*,CMPIContext*,CMPIResult*,
              CMPIObjectPath*);
     CMPIStatus (*execQuery)
             (CMPIInstanceMI*,CMPIContext*,CMPIResult*,
              CMPIObjectPath*,char*,char*);
   };


   typedef struct _CMPIAssociationMIFT CMPIAssociationMIFT;
   typedef struct _CMPIAssociationMI {
      void *hdl;
      CMPIAssociationMIFT *ft;
      //void *cimom;
   } CMPIAssociationMI;

   struct _CMPIAssociationMIFT {
     int ftVersion;
     int miVersion;
     char *miName;
     CMPIStatus (*cleanup)
             (CMPIAssociationMI*,CMPIContext*);
     CMPIStatus (*associators)
             (CMPIAssociationMI*,CMPIContext*,CMPIResult*,
              CMPIObjectPath*,char*,char*,
              char*,char*,char**);
     CMPIStatus (*associatorNames)
             (CMPIAssociationMI*,CMPIContext*,CMPIResult*,
              CMPIObjectPath*,char*,char*,
              char*,char*);
     CMPIStatus (*references)
             (CMPIAssociationMI*,CMPIContext*,CMPIResult*,
              CMPIObjectPath*,char*,char*,char**);
     CMPIStatus (*referenceNames)
             (CMPIAssociationMI*,CMPIContext*,CMPIResult*,
              CMPIObjectPath*,char*,char*);
   };


   typedef struct _CMPIMethodMIFT CMPIMethodMIFT;
   typedef struct _CMPIMethodMI {
      void *hdl;
      CMPIMethodMIFT *ft;
      //void *cimom;
   } CMPIMethodMI;

   struct _CMPIMethodMIFT {
     int ftVersion;
     int miVersion;
     char *miName;
     CMPIStatus (*cleanup)
             (CMPIMethodMI*,CMPIContext*);
     CMPIStatus (*invokeMethod)
             (CMPIMethodMI*,CMPIContext*,CMPIResult*,
              CMPIObjectPath*,char*,CMPIArgs*,CMPIArgs*);
   };


   typedef struct _CMPIPropertyMIFT CMPIPropertyMIFT;
   typedef struct _CMPIPropertyMI {
      void *hdl;
      CMPIPropertyMIFT *ft;
      //void *cimom;
   } CMPIPropertyMI;

   struct _CMPIPropertyMIFT {
     int ftVersion;
     int miVersion;
     char *miName;
     CMPIStatus (*cleanup)
             (CMPIPropertyMI*,CMPIContext*);
     CMPIStatus (*setProperty)
             (CMPIPropertyMI*,CMPIContext*,CMPIResult*,
              CMPIObjectPath*,char*,CMPIData);
     CMPIStatus (*getProperty)
             (CMPIPropertyMI*,CMPIContext*,CMPIResult*,
              CMPIObjectPath*,char*);
   };


   typedef struct _CMPIIndicationMIFT CMPIIndicationMIFT;
   typedef struct _CMPIIndicationMI {
      void *hdl;
      CMPIIndicationMIFT *ft;
      //void *cimom;
   } CMPIIndicationMI;

   struct _CMPIIndicationMIFT {
     int ftVersion;
     int miVersion;
     char *miName;
     CMPIStatus (*cleanup)
             (CMPIIndicationMI*,CMPIContext*);
     CMPIStatus (*authorizeFilter)
             (CMPIIndicationMI*,CMPIContext*,CMPIResult*,
              CMPISelectExp*,char*,CMPIObjectPath*,char*);
     CMPIStatus (*mustPoll)
             (CMPIIndicationMI*,CMPIContext*,CMPIResult*,
              CMPISelectExp*,char*,CMPIObjectPath*);
     CMPIStatus (*activateFilter)
            (CMPIIndicationMI*,CMPIContext*,CMPIResult*,
             CMPISelectExp*,char*,CMPIObjectPath*,CMPIBoolean);
     CMPIStatus (*deActivateFilter)
             (CMPIIndicationMI*,CMPIContext*,CMPIResult*,
              CMPISelectExp*,char*,CMPIObjectPath*,CMPIBoolean);
   };


   struct _CMPIAdapter {
     void *hdl;
     CMPIAdapterFT *ft;
   };

   struct _CMPIAdapterFT {
     int ftVersion;
     int miVersion;
     char *miName;
      CMPIStatus (*release)
                (CMPIAdapter*);
      int (*getMIType)
                (CMPIAdapter*);
      CMPIStatus (*cleanup)
                (CMPIAdapter*,CMPIContext*);
      CMPIEnumeration* (*enumInstanceNames)
                (CMPIAdapter*,CMPIContext*,
                 CMPIObjectPath*,CMPIStatus*);
      CMPIEnumeration* (*enumInstances)
                (CMPIAdapter*,CMPIContext*,
                 CMPIObjectPath*,char**,CMPIStatus*);
      CMPIInstance* (*getInstance)
                (CMPIAdapter*,CMPIContext*,
                 CMPIObjectPath*,char**,CMPIStatus*);
      CMPIObjectPath* (*createInstance)
                 (CMPIAdapter*,CMPIContext*,
                  CMPIObjectPath*,CMPIInstance*,
                  CMPIStatus*);
      CMPIStatus (*setInstance)
                (CMPIAdapter*,CMPIContext*,
                 CMPIObjectPath*,CMPIInstance*);
      CMPIStatus (*deleteInstance)
                (CMPIAdapter*,CMPIContext*,
                 CMPIObjectPath*);
      CMPIEnumeration* (*execQuery)
                (CMPIAdapter*,CMPIContext*,
                 CMPIObjectPath*,char*,char*,char**,CMPIStatus*);

      CMPIEnumeration* (*associators)
                (CMPIAdapter*,CMPIContext*,
                 CMPIObjectPath*,
                 char*,char*,char*,char*,char**,
                 CMPIStatus*);
      CMPIEnumeration* (*associatorNames)
                (CMPIAdapter*,CMPIContext*,
                 CMPIObjectPath*,
                 char*,char*,char*,char*,
                 CMPIStatus*);
      CMPIEnumeration* (*references)
                (CMPIAdapter*,CMPIContext*,
                 CMPIObjectPath*,char*,char*,char**,
                 CMPIStatus*);
      CMPIEnumeration* (*referenceNames)
                (CMPIAdapter*,CMPIContext*,
                 CMPIObjectPath*,char*,char*,
                 CMPIStatus*);

      CMPIData (*invokeMethod)
                (CMPIAdapter*,CMPIContext*,
                 CMPIObjectPath*,
                 char*,CMPIArgs*,CMPIArgs*,CMPIStatus*);

      CMPIStatus (*setProperty)
                (CMPIAdapter*,CMPIContext*,
                 CMPIObjectPath*,
                 char*,CMPIValue*,CMPIType);
      CMPIData (*getProperty)
                (CMPIAdapter*,CMPIContext*,
                 CMPIObjectPath*,char*,CMPIStatus*);

      CMPIBoolean (*authorizeFilter)
                (CMPIAdapter*,CMPIContext*,
                 CMPISelectExp*,char*,
                 CMPIObjectPath*,char*,
                 CMPIStatus*);
      CMPIBoolean (*mustPoll)
                (CMPIAdapter*,CMPIContext*,
                 CMPISelectExp*,char*,
                 CMPIObjectPath*,CMPIStatus*);
      CMPIStatus (*activateFilter)
                (CMPIAdapter*,CMPIContext*,
                 CMPISelectExp*,char*,
                 CMPIObjectPath*,CMPIBoolean);
      CMPIStatus (*deActivateFilter)
                (CMPIAdapter*,CMPIContext*,
                 CMPISelectExp*,char*,
                 CMPIObjectPath*,CMPIBoolean);
   };

#ifdef __cplusplus
 };
#endif

#endif // _CMPIFT_H_





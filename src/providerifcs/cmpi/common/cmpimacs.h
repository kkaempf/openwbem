
/*
 *
 * cmpimacs.h
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
 

#ifndef _CMPIMACS_H_
#define _CMPIMACS_H_

// Various return helper macros
#define CMReturn(rc) \
      { CMPIStatus stat={(rc),NULL}; \
         return stat; }

#define CMReturnWithString(rc,str) \
      { CMPIStatus stat={(rc),(str)}; \
         return stat; }

#define CMReturnWithChars(b,rc,chars) \
      { CMPIStatus stat={(rc),NULL}; \
         stat.msg=(b)->eft->newString((b),(chars),NULL)) \
         return stat; }
	 
#define CMSetStatus(st,rcc) \
      { (st)->rc=(rcc); (st)->msg=NULL; }

#define CMSetStatusWithString(st,rcc,str) \
      { (st)->rc=(rcc); (st)->msg=(str); }
	 
#define CMSetStatusWithChars(b,st,rcc,chars) \
      { (st)->rc=(rcc); \
        (st)->msg=(b)->eft->newString((b),(chars),NULL); }
	 

#ifndef CMPI_NO_CONVENIENCE_SUPPORT
  
  #ifdef __cplusplus 
    #define EXTERN_C extern "C" 
  #else 
    #define EXTERN_C
  #endif

  #define CMIsNullObject(o)           ((o)==NULL || *((void**)(o))==NULL)
  #define CMIsNullValue(v)                   ((v.state) & CMPI_nullValue)
  #define CMIsKeyValue(v)                     ((v.state) & CMPI_keyValue)
  #define CMIsArray(v)                            ((v.type) & CMPI_ARRAY)

    // Life-cycle macros

  #define CMClone(o,rc)                        ((o)->ft->clone((o),(rc)))
  #define CMRelease(o)                            ((o)->ft->release((o)))

    // CMPIBroker factory macros

  #define CMNewInstance(b,c,rc)     ((b)->eft->newInstance((b),(c),(rc)))
  #define CMNewObjectPath(b,n,c,rc) \
                              ((b)->eft->newObjectPath((b),(n),(c),(rc)))
  #define CMNewString(b,s,rc)         ((b)->eft->newString((b),(s),(rc)))
  #define CMNewDateTime(b,rc)           ((b)->eft->newDateTime((b),(rc)))

  #define CMLoadMi(b,n,rc)               ((b)->eft->loadMI((b),(n),(rc)))
  #define CMNewArray(b,c,t,rc)     ((b)->eft->newArray((b),(c),(t),(rc)))
  #define CMNewArgs(b,rc)                   ((b)->eft->newArgs((b),(rc)))
  #define CMNewDateTime(b,rc)           ((b)->eft->newDateTime((b),(rc)))
  #define CMNewPIDateTimeFromBinary(b,d,i,rc) \
                       ((b)->ft->newDateTimeFromBinary((b),(d),(i),(rc)))
  #define CMNewDateTimeFromChars(b,d,rc) \
                            ((b->eft->newDateTimeFromChars((b),(d),(rc))))
  #define CMNewSelectExp(b,l,x,rc) \
                              ((b)->eft->*newSelectExp((b),(l),(x),)rc)))

    // Debugging macros

  #define CDToString(b,o,rc)    ((b)->eft->toString((b),(void*)(o),(rc)))
  #define CDIsOfType(b,o,t,rc) \
                             (b)->eft->isOfType((b),(void*)(o),(t),(rc)))
  #define CDgetType(b,o,rc)      ((b)->eft->getTpye((b),(void*)(o),(rc)))

    // CMPIInstance macros

  #define CMGetProperty(i,n,rc)      ((i)->ft->getProperty((i),(n),(rc)))
  #define CMSetProperty(i,n,v,t) \
                      ((i)->ft->setProperty((i),(n),(CMPIValue*)(v),(t)))
  #define CMGetPropertyCount(i,c)   ((i)->ft->setPropertyCount((i),(rc)))
  #define CMGetObjectPath(i,rc)        ((i)->ft->getObjectPath((i),(rc)))


   // CMPIObjectPath macros

  #define CMSetNameSpace(p,n)            ((p)->ft->setNameSpace((p),(n)))
  #define CMGetNameSpace(p,rc)          ((p)->ft->getNameSpace((p),(rc)))
  #define CMSetClassName(p,n)            ((p)->ft->setClassName((p),(n)))
  #define CMGetClassName(p,rc)          ((p)->ft->getClassName((p),(rc)))
  #define CMAddKey(p,n,v,t) \
                           ((p)->ft->addKey((p),(n),(CMPIValue*)(v),(t)))
  #define CMGetKey(p,n,rc)                ((p)->ft->getKey((p),(n),(rc)))
  #define CMGetKeyCount(p,rc)            ((p)->ft->getKeyCount((p),(rc)))
  #define CMClassPathIsA(p,pn,rc)  ((p)->ft->classPathIsA((p),(pn),(rc)))


    // CMPIArray macros

   #define CMGetArrayCount(a,rc)             ((a)->ft->getSize((a),(rc)))
   #define CMGetArrayType(a,rc)        ((a)->ft->getSimpleType((a),(rc)))
   #define CMGetArrayElementAt(a,n,rc) \
                                    ((a)->ft->getElementAt((a),(n),(rc)))
   #define CMSetArrayElementAt(a,n,v,t) \
                     ((a)->ft->setElementAt((a),(n),(CMPIValue*)(v),(t)))

    // CMPISelectExp macros

  #define CMGetSelExpString(s,rc)          ((s)->ft->getString((s),(rc)))
  #define CMEvaluateSelExp(s,i,r)        ((s)->ft->evaluate((s),(i),(r)))
  #define CMGetDoc(s,rc)                      ((s)->ft->getDOC((s),(rc)))
  #define CMGetCod(s,rc)                      ((s)->ft->getCOD((s),(rc)))

    // CMPISelectCond macros

  #define CMGetSubCondCount(c,rc) \
                                ((c)->ft->getCountAndType((c),NULL,(rc)))
  #define CMGetSubCondCountAndType(c,t,rc) \
                                 ((c)->ft->getCountAndType((c),(t),(rc)))
  #define CMGetSubCondAt(c,p,rc)    ((c)->ft->getSubCondAt((c),(p),(rc)))

    // CMPISubCond macros

  #define CMGetPredicateCount(s,rc)         ((s)->ft->getCount((s),(rc)))
  #define CMGetPredicateAt(s,p,rc) \
                                  ((s)->ft->getPredicateAt((s),(p),(rc)))
  #define CMGetPredicate(s,n,rc)    ((s)->ft->getPredicate((s),(n),(rc)))

    // CMPIPredicate macros

  #define CMGetPredicateData(p,t,o,n,v) \
                                  ((p)->ft->getData((p),(t),(o),(n),(v)))

    // CMPIDateTime macros

  #define CMGetStringFormat(d,rc)    ((d)->ft->getStringFormat((d),(rc)))
  #define CMGetBinaryFormat(d,rc)    ((d)->ft->getBinaryFormat((d),(rc)))
  #define CMIsInterval(d,rc)              ((d)->ft->isInterval((d),(rc)))

    // CMPIARgs macros

  #define CMAddArg(a,n,v,t) \
                           ((a)->ft->addArg((a),(n),(CMPIValue*)(v),(t)))
  #define CMGetArg(a,n,rc)                ((a)->ft->getArg((a),(n),(rc)))
  #define CMGetArgAt(a,p,n,rc)       ((a)->f->getArgAt((a),(p),(n),(rc)))
  #define CMGetArgCount(a,rc)            ((a)->ft->getArgCount((a),(rc)))


    // CMPIString Macros

  #define CMGetCharPtr(s)                                 ((char*)s->hdl)


    // CMPIEnumeration Macros

  #define CMGetNext(n,rc)                    ((n)->ft->getNext((n),(rc)))
  #define CMHasNext(n,rc)                    ((n)->ft->hasNext((n),(rc)))

    // CMPIResult Macros

  #define CMReturnData(r,v,t) \
                           ((r)->ft->returnData((r),(CMPIValue*)(v),(t)))
  #define CMReturnInstance(r,i)        ((r)->ft->returnInstance((r),(i)))
  #define CMReturnObjectPath(r,o)    ((r)->ft->returnObjectPath((r),(o)))
  #define CMReturnDone(r)                      ((r)->ft->returnDone((r)))

    // CMPIContext Macros

  #define CMAddContextEntry(c,n,v,t) \
                  ((c)->ft->addEntry((c),(n),(CMPIValue*)(v),(t)))
  #define CMGetContextEntry(c,n,rc)  \
                                 ((c)->ft->getEntry((c),(n),(rc)))
  #define CMGetContextEntryAt(e,p,n,d,rc) \
                         ((c)->ft->addEntry((c),(p),(n),(d),(rc)))
  #define CMGetContextEntryCount(c,rc) \
                                ((c)->ft->getEntryCount((c),(rc)))

    // CMPIBroker Macros

  #define CBGetClassification(b)         ((b)->bft->brokerClassification)
  #define CBBrokerVersion(b)                    ((b)->bft->brokerVersion)
  #define CBBrokerName(b)                          ((b)->bft->brokerName)
  #define CBPrepareAttachThread(b,c) \
                                 ((b)->bft->prepareAttachThread((b),(c)))
  #define CBAttachThread(b,c)           ((b)->bft->attachThread((b),(c)))
  #define CBDetachThread(b,c)           ((b)->bft->detachThread((b),(c)))
  #define CBDeliverIndication(b,c,n,i) \
                           ((b)->bft->deliverIndication((b),(c),(n),(i))) 
  #define CBEnumInstanceNames(b,c,p,rc) \
                          ((b)->bft->enumInstanceNames((b),(c),(p),(rc)))
  #define CBEnumInstances(b,c,p,pr,rc) \
                         ((b)->bft->enumInstances((b),(c),(p),(pr),(rc)))
  #define CBGetInstance(b,c,p,pr,rc) \
                           ((b)->bft->getInstance((b),(c),(p),(pr),(rc)))
  #define CBCreateInstance(b,p,i,rc) \
                             ((b)->bft->createInstance((b),(p),(i),(rc)))
  #define CBSetInstance(b,p,i)      ((b)->bft->_setInstance((b),(p),(i)))
  #define CBDeleteInstance(b,p)       ((b)->bft->deleteInstance((b),(p)))
  #define CBExecQuery(b,p,q,l,c,rc) \
                               ((b)->fb->execQuery((b),(q),(l),(c),(rc)))
  #define CBAssociators(b,a,p,r,rr,pr,rc) \
                      ((b)->bft->associators((b),(a),(p),(rr),(pr),(rc)))
  #define CBAssociatorNames(b,a,p,r,rr,rc) \
                   ((b)->bft->associatorNames((b),(a),(p),(r),(rr),(rc)))
  #define CBInvokeMethod(b,m,ai,ao,rc) \
                         ((b)->bft->invokeMethod((b),(m),(ai),(ao),(rc)))
  #define CBSetProperty(b,n,v,t) \
                     ((b)->bft->setProperty((b),(n),(CMPIValue*)(v),(t)))
  #define CBGetProperty(b,n,rc)       (b)->bft->getProperty((b),(n),(rc))


    // CMPIAdapter Macros

  #define CAMIVersion(a)                                 ((a)->miVersion)
  #define CAMAName(a)                                       ((a)->miName)
  #define CAGetMIType(a,rc)                ((a)->ft->getMIType((a),(rc)))
  #define CAEnumInstanceNames(a,f,p,c,rc) \
                       ((a)->ft->enumInstanceNames((a),(f),(p),(c),(rc)))
  #define CAEnumInstances(a,f,p,c,rc) \
                           ((a)->ft->enumInstances((a),(f),(p),(c),(rc)))
  #define CAGetInstance(a,p,c,rc) \
                                 ((a)->ft->getInstance((a),(p),(c),(rc)))
  #define CACreateInstance(a,p,i,rc) \
                              ((a)->ft->createInstance((a),(p),(i),(rc)))
  #define CASetInstance(a,p,i)        ((a)->ft->setInstance((a),(p),(i)))
  #define CADeleteInstance(a,p)        ((a)->ft->deleteInstance((a),(p)))
  #define CAExecQuery(a,p,q,l,c,rc) \
                               ((a)->ft->execQuery((a),(q),(l),(c),(rc)))
  #define CAAssociators(a,asc,p,r,rr,pr,rc) \
                     ((a)->ft->associators((a),(asc),(p),(rr),(pr),(rc)))
  #define CAAssociatorNames(a,asc,p,r,rr,rc) \
                  ((a)->ft->associatorNames((a),(asc),(p),(r),(rr),(rc)))
  #define CAInvokeMethod(a,m,ai,ao,rc) \
                          ((a)->ft->invokeMethod((a),(m),(ai),(ao),(rc)))
  #define CASetProperty(a,n,v,t) \
                      ((a)->ft->setProperty((a),(n),(CMPIValue*)(v),(t)))
  #define CAGetProperty(a,n,rc)      ((a)->ft->getProperty((a),(n),(rc)))
  #define CAAuthorizeFilter(a,f,n,p,o,rc) \
                      ((a)->ft->authorizeFilter((a),(f),(n),(p),o),(rc)))
  #define CAMustPoll(a,f,n,p,rc) \
                                ((a)->ft->mustPoll((a),(f),(n),(p),(rc)))
  #define CAActvateFilter(a,f,n,p,l) \
                            ((a)->f->activateFilter((a),(f),(n),(p),(l)))
  #define CADeActivateFilter(a,f,n,p,l) \
                          ((a)->f->deActivateFilter((a),(f),(n),(p),(l)))


   // MI factory stubs

  #define CMNoHook if (brkr)

  #define CMInstanceMIStub(cn,pfx,broker,hook) \
  static CMPIInstanceMIFT instMIFT__={ \
   CMPICurrentVersion, \
   CMPICurrentVersion, \
   "instance" #cn, \
   pfx##Cleanup, \
   pfx##EnumInstanceNames, \
   pfx##EnumInstances, \
   pfx##GetInstance, \
   pfx##CreateInstance, \
   pfx##SetInstance, \
   pfx##DeleteInstance, \
   pfx##ExecQuery, \
  }; \
  EXTERN_C \
  CMPIInstanceMI* cn##_Create_InstanceMI(CMPIBroker* brkr,CMPIContext *ctx) { \
   static CMPIInstanceMI mi={ \
      NULL, \
      &instMIFT__, \
   }; \
   broker=brkr; \
   hook; \
   return &mi;  \
  }

  #define CMAssociationMIStub(cn,pfx,broker,hook) \
  static CMPIAssociationMIFT assocMIFT__={ \
   CMPICurrentVersion, \
   CMPICurrentVersion, \
   "asscociation" #cn, \
   pfx##AssociationCleanup, \
   pfx##Associators, \
   pfx##AssociatorNames, \
   pfx##References, \
   pfx##ReferenceNames, \
  }; \
  EXTERN_C \
  CMPIAssociationMI* cn##_Create_AssociationMI(CMPIBroker* brkr,CMPIContext *ctx) { \
   static CMPIAssociationMI mi={ \
      NULL, \
      &assocMIFT__, \
   }; \
   broker=brkr; \
   hook; \
   return &mi;  \
  }

  #define CMMethodMIStub(cn,pfx,broker,hook) \
  static CMPIMethodMIFT methMIFT__={ \
   CMPICurrentVersion, \
   CMPICurrentVersion, \
   "method" #cn, \
   pfx##MethodCleanup, \
   pfx##InvokeMethod, \
  }; \
  EXTERN_C \
  CMPIMethodMI* cn##_Create_MethodMI(CMPIBroker* brkr,CMPIContext *ctx) { \
   static CMPIMethodMI mi={ \
      NULL, \
      &methMIFT__, \
   }; \
   broker=brkr; \
   hook; \
   return &mi; \
  }

  #define CMPropertyMIStub(cn,pfx,broker,hook) \
  static CMPIPropertyMIFT propMIFT__={ \
   CMPICurrentVersion, \
   CMPICurrentVersion, \
   "property" #cn, \
   pfx##PropertyCleanup, \
   pfx##SetProperty, \
   pfx##GetProperty, \
  }; \
  EXTERN_C \
  CMPIPropertyMI* cn##_Create_PropertyMI(CMPIBroker* brkr,CMPIContext *ctx) { \
   static CMPIPropertyMI mi={ \
      NULL, \
      &propMIFT__, \
   }; \
   broker=brkr; \
   hook; \
   return &mi; \
  }

  #define CMIndicationMIStub(cn,pfx,broker,hook) \
  static CMPIIndicationMIFT indMIFT__={ \
   CMPICurrentVersion, \
   CMPICurrentVersion, \
   "Indication" #cn, \
   pfx##IndicationCleanup, \
   pfx##AuthorizeFilter, \
   pfx##MustPoll, \
   pfx##ActivateFilter, \
   pfx##DeActivateFilter, \
  }; \
  EXTERN_C \
  CMPIIndicationMI* cn##_Create_IndicationMI(CMPIBroker* brkr,CMPIContext *ctx) { \
   static CMPIIndicationMI mi={ \
      NULL, \
      &indMIFT__, \
   }; \
   broker=brkr; \
   hook; \
   return &mi; \
 }


#endif // CMPI_NO_CONVENIENCE_SUPPORT
#endif // _CMPIMACS_H_

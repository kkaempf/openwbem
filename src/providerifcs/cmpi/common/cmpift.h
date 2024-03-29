
/*
 *
 * cmpift.h
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
 * Description: CMPI Encapsulated types Function Tables
 *
 */

#ifndef _CMPIFT_H_
#define _CMPIFT_H_
#include "OW_config.h"

#include <cmpidt.h>

#ifdef __cplusplus
extern "C" {
#endif



   /*
   //---------------------------------------------------
   //--
   //	_CMPIBrokerEncFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers to broker and factory services
       of encapsulated CMPIObjects. This table is made available
       by the Management Broker, aka CIMOM, whenever a provider
       is loaded and initialized.
   */
   struct _CMPIBrokerEncFT {

     /** Function table version
     */
     int ftVersion;

     /** Instance factory service.
         @param mb Broker this pointer
	 @param op ObjectPath containing namespace and classname.
	 @param rc Output: Service return status (suppressed when NULL).
         @return The newly created Instance.
     */
     CMPIInstance* (*newInstance)
                 (const CMPIBroker* mb,
				  const CMPIObjectPath* op, CMPIStatus* rc);

     /** ObjectPath factory service.
         @param mb Broker this pointer
	 @param ns Namespace
	 @param cn Classname.
	 @param rc Output: Service return status (suppressed when NULL).
         @return The newly created ObjectPath.
     */
     CMPIObjectPath* (*newObjectPath)
                 (const CMPIBroker* mb, 
				  const char* ns, const char* cn, CMPIStatus* rc);

     /** Args container factory service.
         @param mb Broker this pointer
	 @param rc Output: Service return status (suppressed when NULL).
         @return The newly created Args container.
     */
     CMPIArgs* (*newArgs)
                 (const CMPIBroker* mb, CMPIStatus* rc);

     /** String container factory service.
         @param mb Broker this pointer
	 @param data String data
	 @param rc Output: Service return status (suppressed when NULL).
         @return The newly created String.
     */
     CMPIString* (*newString)
                 (const CMPIBroker* mb, const char* data, CMPIStatus* rc);

     /** Array container factory service.
         @param mb Broker this pointer
	 @param max Maximum number of elements
	 @param type Element type
	 @param rc Output: Service return status (suppressed when NULL).
         @return The newly created Array.
     */
     CMPIArray* (*newArray)
                 (const CMPIBroker* mb, CMPICount max, CMPIType type, CMPIStatus* rc);

     /** DateTime factory service. Initialized with the time of day.
         @param mb Broker this pointer
	 @param rc Output: Service return status (suppressed when NULL).
         @return The newly created DateTime.
     */
     CMPIDateTime* (*newDateTime)
                 (const CMPIBroker* mb, CMPIStatus* rc);

     /** DateTime factory service. Initialized from <binTime>.
         @param mb Broker this pointer
	 @param binTime Date/Time definition in binary format in microsecods
	       starting since 00:00:00 GMT, Jan 1,1970.
 	 @param interval Wenn true, defines Date/Time definition to be an interval value
	 @param rc Output: Service return status (suppressed when NULL).
         @return The newly created DateTime.
     */
     CMPIDateTime* (*newDateTimeFromBinary)
                 (const CMPIBroker* mb, CMPIUint64 binTime, CMPIBoolean interval,
		  CMPIStatus* rc);

     /** DateTime factory service. Is initialized from <utcTime>.
         @param mb Broker this pointer
	 @param utcTime Date/Time definition in UTC format
	 @param rc Output: Service return status (suppressed when NULL).
         @return The newly created DateTime.
     */
     CMPIDateTime* (*newDateTimeFromChars)
                 (const CMPIBroker* mb, const char* utcTime, CMPIStatus* rc);

     /** SelectExp factory service. TBD.
         @param mb Broker this pointer
	 @param query The select expression.
	 @param lang The query language.
	 @param projection Output: Projection specification (suppressed when NULL).
	 @param rc Output: Service return status (suppressed when NULL).
         @return The newly created SelectExp.
     */
     CMPISelectExp* (*newSelectExp)
                 (const CMPIBroker* mb, const char* query, const char* lang,
                  CMPIArray** projection, CMPIStatus* st);

     /** Function to determine whether a CIM class is of <type> or any of
         <type> subclasses.
         @param mb Broker this pointer
	 @param op The class path (namespace and classname components).
	 @param type The type to tested for.
	 @param rc Output: Service return status (suppressed when NULL).
         @return True if test successful.
     */
     CMPIBoolean (*classPathIsA)
                 (const CMPIBroker* mb, const CMPIObjectPath* op, 
				  const char* type, CMPIStatus* rc);

     /** Attempts to transforms an CMPI object to a broker specific string format.
         Intended for debugging purposes only.
         @param mb Broker this pointer
	 @param object A valid CMPI object.
	 @param rc Output: Service return status (suppressed when NULL).
         @return String from representation of <object>.
     */
     CMPIString* (*toString)
                 (const CMPIBroker* mb, const void* object, CMPIStatus* rc);

     /** Verifies whether <object> is of CMPI type <type>.
         Intended for debugging purposes only.
         @param mb Broker this pointer
	 @param object A valid CMPI object.
	 @param type A string specifying a valid CMPI Object type
	         ("CMPIInstance", "CMPIObjectPath", etc).
	 @param rc Output: Service return status (suppressed when NULL).
         @return True if test successful.
     */
     CMPIBoolean (*isOfType)
                 (const CMPIBroker* mb, const void* object, const char* type, CMPIStatus* rc);

     /** Retrieves the CMPI type of <object>.
         Intended for debugging purposes only.
         @param mb Broker this pointer
	 @param object A valid CMPI object.
	 @param rc Output: Service return status (suppressed when NULL).
         @return CMPI object type.
     */
     CMPIString* (*getType)
                 (const CMPIBroker* mb, const void* object, CMPIStatus* rc);

     /** Retrieves translated message.
         @param mb Broker this pointer
	 @param msgId The message identifier.
	 @param defMsg The default message.
	 @param rc Output: Service return status (suppressed when NULL).
	 @param count The number of message substitution values.
         @return the trabslated message.
     */
     #if defined(CMPI_VER_85)
     CMPIString* (*getMessage)
                 (const CMPIBroker* mb, const char *msgId, const char *defMsg, CMPIStatus* rc, unsigned int count, ...);
     #endif /* CMPI_VER_85 */

	 #if defined(CMPI_VER_90)
	 CMPIStatus (*logMessage)
		(const CMPIBroker*,int,const char*,const char*,
		 const CMPIString*);
	 CMPIStatus (*trace)
		(const CMPIBroker*,int,const char*,const char*,
		 const CMPIString*);
	 #endif /* CMPI_VER_90 */ 
   };



   /*
   //---------------------------------------------------
   //--
   //	_CMPIBrokerFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers to broker CIMOM services
       (up-calls). This table is made available by the Management Broker,
       whenever a provider is loaded and initialized.
   */
   struct _CMPIBrokerFT {

     /** 32 bits describing CMPI features supported by this CIMOM.
         See CMPI_MB_Class_x and CMPI_MB_Supports_xxx flags.
     */
     CMPIUint32 brokerClassification;
     /** CIMOM version as defined by CIMOM
     */
     int brokerVersion;
     /** CIMOM name
     */
     char *brokerName;

     /** This function prepares the CMPI run time system to accept
         a thread that will be using CMPI services. The returned
	 CMPIContext object must be used by the subsequent attachThread()
	 and detachThread() invocations.
	 @param mb Broker this pointer.
	 @param ctx Old Context object
	 @return New Context object to be used by thread to be attached.
     */
     CMPIContext* (*prepareAttachThread)
                (const CMPIBroker* mb, const CMPIContext* ctx);

      /** This function informs the CMPI run time system that the current
         thread with Context will begin using CMPI services.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @return Service return status.
     */
     CMPIStatus (*attachThread)
                (const CMPIBroker*,const CMPIContext*);

      /** This function informs the CMPI run time system that the current thread
         will not be using CMPI services anymore. The Context object will be
	 freed during this operation.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @return Service return status.
     */
     CMPIStatus (*detachThread)
                (const CMPIBroker* mb, const CMPIContext* ctx);

     /* class 0 services */

      /** This function requests delivery of an Indication. The CIMOM will
         locate pertinent subscribers and notify them about the event.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param ns Namespace
	 @param ind Indication Instance
	 @return Service return status.
     */
     CMPIStatus (*deliverIndication)
                (const CMPIBroker* mb, const CMPIContext* ctx,
                 const char* ns, const CMPIInstance* ind);
     /* class 1 services */

      /** Enumerate Instance Names of the class (and subclasses) defined by <op>.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param op ObjectPath containing namespace and classname components.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Enumeration of ObjectPathes.
     */
     CMPIEnumeration* (*enumInstanceNames)
                (const CMPIBroker* mb, const CMPIContext* ctx,
                 const CMPIObjectPath* op, CMPIStatus* rc);

      /** Get Instance using <op> as reference. Instance structure can be
         controled using the CMPIInvocationFlags entry in <ctx>.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param op ObjectPath containing namespace, classname and key components.
	 @param properties If not NULL, the members of the array define one or more Property
	     names. Each returned Object MUST NOT include elements for any Properties
	     missing from this list
	 @param rc Output: Service return status (suppressed when NULL).
	 @return The Instance.
     */
     CMPIInstance* (*getInstance)
                (const CMPIBroker* mb, const CMPIContext* ctx,
                 const CMPIObjectPath* op, const char** properties, CMPIStatus* rc);

     /* class 2 services */

      /** Create Instance from <inst> using <op> as reference.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param op ObjectPath containing namespace, classname and key components.
	 @param inst Complete instance.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return The assigned instance reference.
     */
     CMPIObjectPath* (*createInstance)
                (const CMPIBroker* mb, const CMPIContext* ctx,
                 const CMPIObjectPath* op, const CMPIInstance* inst, CMPIStatus* rc);

      /** Replace an existing Instance from <inst> using <op> as reference.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param op ObjectPath containing namespace, classname and key components.
	 @param inst Complete instance.
	 @return Service return status.
     */
     CMPIStatus (*modifyInstance)
                (const CMPIBroker* mb, const CMPIContext* ctx,
                 const CMPIObjectPath* op,  const CMPIInstance* inst, const char ** properties);

      /** Delete an existing Instance using <op> as reference.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param op ObjectPath containing namespace, classname and key components.
	 @return Service return status.
     */
     CMPIStatus (*deleteInstance)
                (const CMPIBroker* mb, const CMPIContext* ctx,
                 const CMPIObjectPath* op);

      /** Query the enumeration of instances of the class (and subclasses) defined
         by <op> using <query> expression.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param op ObjectPath containing namespace and classname components.
	 @param query Query expression
	 @param lang Query Language
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Resulting eumeration of Instances.
     */
     CMPIEnumeration* (*execQuery)
                (const CMPIBroker* mb, const CMPIContext* ctx,
                 const CMPIObjectPath* op, const char* query, const char* lang, CMPIStatus* rc);

      /** Enumerate Instances of the class (and subclasses) defined by <op>.
         Instance structure and inheritance scope can be controled using the
	 CMPIInvocationFlags entry in <ctx>.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param op ObjectPath containing namespace and classname components.
	 @param properties If not NULL, the members of the array define one or more Property
	     names. Each returned Object MUST NOT include elements for any Properties
	     missing from this list
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Enumeration of Instances.
     */
     CMPIEnumeration* (*enumInstances)
                (const CMPIBroker* mb, const CMPIContext* ctx,
                 const CMPIObjectPath* op, const char** properties, CMPIStatus* rc);

      /** Enumerate instances associated with the Instance defined by the <op>.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param op Source ObjectPath containing namespace, classname and key components.
	 @param assocClass If not NULL, MUST be a valid Association Class name.
	    It acts as a filter on the returned set of Objects by mandating that
	    each returned Object MUST be associated to the source Object via an
	    Instance of this Class or one of its subclasses.
	 @param resultClass If not NULL, MUST be a valid Class name.
	    It acts as a filter on the returned set of Objects by mandating that
	    each returned Object MUST be either an Instance of this Class (or one
	    of its subclasses).
	 @param role If not NULL, MUST be a valid Property name.
	    It acts as a filter on the returned set of Objects by mandating
	    that each returned Object MUST be associated to the source Object
	    via an Association in which the source Object plays the specified role
	    (i.e. the name of the Property in the Association Class that refers
	    to the source Object MUST match the value of this parameter).
	 @param resultRole If not NULL, MUST be a valid Property name.
	    It acts as a filter on the returned set of Objects by mandating
	    that each returned Object MUST be associated to the source Object
	    via an Association in which the returned Object plays the specified role
	    (i.e. the name of the Property in the Association Class that refers to
	    the returned Object MUST match the value of this parameter).
	 @param properties If not NULL, the members of the array define one or more Property
	     names. Each returned Object MUST NOT include elements for any Properties
	     missing from this list
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Enumeration of Instances.
     */
     CMPIEnumeration* (*associators)
                (const CMPIBroker* mb,const CMPIContext* ctx,
                 const CMPIObjectPath* op, const char* assocClass, const char* resultClass,
		 const char* role, const char* resultRole, const char** properties, CMPIStatus* rc);

      /** Enumerate ObjectPaths associated with the Instance defined by <op>.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param op Source ObjectPath containing namespace, classname and key components.
	 @param assocClass If not NULL, MUST be a valid Association Class name.
	    It acts as a filter on the returned set of Objects by mandating that
	    each returned Object MUST be associated to the source Object via an
	    Instance of this Class or one of its subclasses.
	 @param resultClass If not NULL, MUST be a valid Class name.
	    It acts as a filter on the returned set of Objects by mandating that
	    each returned Object MUST be either an Instance of this Class (or one
	    of its subclasses).
	 @param role If not NULL, MUST be a valid Property name.
	    It acts as a filter on the returned set of Objects by mandating
	    that each returned Object MUST be associated to the source Object
	    via an Association in which the source Object plays the specified role
	    (i.e. the name of the Property in the Association Class that refers
	    to the source Object MUST match the value of this parameter).
	 @param resultRole If not NULL, MUST be a valid Property name.
	    It acts as a filter on the returned set of Objects by mandating
	    that each returned Object MUST be associated to the source Object
	    via an Association in which the returned Object plays the specified role
	    (i.e. the name of the Property in the Association Class that refers to
	    the returned Object MUST match the value of this parameter).
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Enumeration of ObjectPaths.
     */
     CMPIEnumeration* (*associatorNames)
                (const CMPIBroker* mb, const CMPIContext* ctx,
                 const CMPIObjectPath* op, const char* assocClass, const char* resultClass,
		 const char* role, const char* resultRole, CMPIStatus* rc);

       /** Enumerates the association instances that refer to the instance defined by
           <op>.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param op Source ObjectPath containing namespace, classname and key components.
	 @param resultClass If not NULL, MUST be a valid Class name.
	    It acts as a filter on the returned set of Objects by mandating that
	    each returned Object MUST be either an Instance of this Class (or one
	    of its subclasses).
	 @param role If not NULL, MUST be a valid Property name.
	    It acts as a filter on the returned set of Objects by mandating
	    that each returned Object MUST be associated to the source Object
	    via an Association in which the source Object plays the specified role
	    (i.e. the name of the Property in the Association Class that refers
	    to the source Object MUST match the value of this parameter).
	 @param properties If not NULL, the members of the array define one or more Property
	     names. Each returned Object MUST NOT include elements for any Properties
	     missing from this list
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Enumeration of ObjectPaths.
     */
     CMPIEnumeration* (*references)
                (const CMPIBroker* mb, const CMPIContext* ctx,
                 const CMPIObjectPath* op, const char* resultClass ,const char* role ,
		 const char** properties, CMPIStatus* rc);

       /** Enumerates the association ObjectPaths that refer to the instance defined by
           <op>.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param op Source ObjectPath containing namespace, classname and key components.
	 @param resultClass If not NULL, MUST be a valid Class name.
	    It acts as a filter on the returned set of Objects by mandating that
	    each returned Object MUST be either an Instance of this Class (or one
	    of its subclasses).
	 @param role If not NULL, MUST be a valid Property name.
	    It acts as a filter on the returned set of Objects by mandating
	    that each returned Object MUST be associated to the source Object
	    via an Association in which the source Object plays the specified role
	    (i.e. the name of the Property in the Association Class that refers
	    to the source Object MUST match the value of this parameter).
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Enumeration of ObjectPaths.
       */
     CMPIEnumeration* (*referenceNames)
                (const CMPIBroker* mb, const CMPIContext* ctx,
                 const CMPIObjectPath* op, const char* resultClass, const char* role,
                 CMPIStatus* rc);

       /** Invoke a named, extrinsic method of an Instance
         defined by the <op> parameter.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param op ObjectPath containing namespace, classname and key components.
	 @param method Method name
	 @param in Input parameters.
	 @param out Output parameters.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Method return value.
      */
     CMPIData (*invokeMethod)
                (const CMPIBroker* mb, const CMPIContext* ctx,
                 const CMPIObjectPath* op,const char* method,
		 const CMPIArgs* in, CMPIArgs* out, CMPIStatus* rc);

       /** Set the named property value of an Instance defined by the <op> parameter.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param op ObjectPath containing namespace, classname and key components.
	 @param name Property name
	 @param value Value.
	 @param type Value type.
	 @return Service return status.
      */
     CMPIStatus (*setProperty)
                (const CMPIBroker* mb, const CMPIContext* ctx,
                 const CMPIObjectPath* op, const char* name , const CMPIValue* value,
                 CMPIType type);

       /** Get the named property value of an Instance defined by the <op> parameter.
	 @param mb Broker this pointer.
	 @param ctx Context object
	 @param op ObjectPath containing namespace, classname and key components.
	 @param name Property name
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Property value.
      */
     CMPIData (*getProperty)
                (const CMPIBroker* mb, const CMPIContext* ctx,
                 const CMPIObjectPath*,const char*,CMPIStatus*);
   };



   /*
   //---------------------------------------------------
   //--
   //	_CMPIBroker Encapsulated object
   //--
   //---------------------------------------------------
   */


   /** This structure represents the Management Broker (CIM Object Manager).
   */
   struct _CMPIBroker {

       /** Opaque pointer to MB specific implementation data.
       */
     void *hdl;

       /** Pointer to MB service routines function table.
       */
     CMPIBrokerFT *bft;

       /** Pointer to MB factory service routines function table.
       */
     CMPIBrokerEncFT *eft;

       /** Pointer to MB extended services function table..
       */
#if defined(CMPI_VER_90)
     CMPIBrokerExtFT *xft;
#endif
   };



    //---------------------------------------------------
   //--
   //	_CMPIBrokerExtFT Function Table
   //--
   //---------------------------------------------------

#if defined(CMPI_VER_90)

#include "cmpios.h"

   struct timespec;

   /** This structure is a table of pointers to extended broker CIMOM
       services This table is made available by the Management Broker,
       whenever a provider is loaded and initialized.
       This is an extension used by Pegasus to support platform dependencies.
   */
   struct _CMPIBrokerExtFT {
     /** Function table version
     */
     int ftVersion;

     char *(*resolveFileName)
         (const char *filename);



     CMPI_THREAD_TYPE (*newThread)
        (CMPI_THREAD_RETURN (CMPI_THREAD_CDECL *start)(void *), void *parm, int detached);

     int (*joinThread)
        (CMPI_THREAD_TYPE thread, CMPI_THREAD_RETURN *retval );

     int (*exitThread)
        (CMPI_THREAD_RETURN return_code);

     int (*cancelThread)
        (CMPI_THREAD_TYPE thread);

     int (*threadSleep)
        (CMPIUint32 msec);

     int (*threadOnce)
        (int *once, void (*init)(void));


     int (*createThreadKey)
        (CMPI_THREAD_KEY_TYPE *key, void (*cleanup)(void*));

     int (*destroyThreadKey)
        (CMPI_THREAD_KEY_TYPE key);

     void *(*getThreadSpecific)
        (CMPI_THREAD_KEY_TYPE key);

     int (*setThreadSpecific)
        (CMPI_THREAD_KEY_TYPE key, void * value);



     CMPI_MUTEX_TYPE (*newMutex)
        (int opt);

     void (*destroyMutex)
        (CMPI_MUTEX_TYPE);

     void (*lockMutex)
        (CMPI_MUTEX_TYPE);

     void (*unlockMutex)
        (CMPI_MUTEX_TYPE);



     CMPI_COND_TYPE (*newCondition)
        (int opt);

     void (*destroyCondition)
        (CMPI_COND_TYPE);

     int (*condWait)
        (CMPI_COND_TYPE cond, CMPI_MUTEX_TYPE mutex);

     int (*timedCondWait)
        (CMPI_COND_TYPE cond, CMPI_MUTEX_TYPE mutex, struct timespec *wait);

     int (*signalCondition)
        (CMPI_COND_TYPE cond);
  };

#endif


   /*
   //---------------------------------------------------
   //--
   //	_CMPIContext Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to Context
       support sevices.
   */
  struct _CMPIContextFT {

       /** Function table version
       */
     int ftVersion;

       /** The Context object will not be used any further and may be freed by
           CMPI run time system.
	 @param ctx Context this pointer.
	 @return Service return status.
      */
     CMPIStatus (*release)
              (CMPIContext* ctx);

       /** Create an independent copy of the Context object.
	 @param ctx Context this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Pointer to copied Context object.
      */
     CMPIContext* (*clone)
              (const CMPIContext* ctx, CMPIStatus* rc);

       /** Gets a named Context entry value.
	 @param ctx Context this pointer.
	 @param name Context entry name.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Entry value.
      */
     CMPIData (*getEntry)
              (const CMPIContext* ctx, const char* name, CMPIStatus* rc);

       /** Gets a Context entry value defined by its index.
	 @param ctx Context this pointer.
	 @param index Position in the internal Data array.
	 @param name Output: Returned Context entry name (suppressed when NULL).
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Entry value.
      */
     CMPIData (*getEntryAt)
              (const CMPIContext* ctx, unsigned int index, CMPIString** name,
	       CMPIStatus* rc);

      /** Gets the number of entries contained in this Context.
	 @param ctx Context this pointer.
	 @return Number of entries.
      */
     unsigned int (*getEntryCount)
              (const CMPIContext* ctx, CMPIStatus* rc);

      /** adds/replaces a named Context entry
	 @param ctx Context this pointer.
         @param name Entry name.
         @param value Address of value structure.
         @param type Value type.
	 @return Service return status.
      */
     CMPIStatus (*addEntry)
              (const CMPIContext* ctx, const char* name, const CMPIValue* value, const CMPIType type);
  };




  /*
   //---------------------------------------------------
   //--
   //	_CMPIContextr Encapsulated object
   //--
   //---------------------------------------------------
   */


   /** This structure represents the Encapsulated Context object.
   */
  struct _CMPIContext {

       /** Opaque pointer to MB specific implementation data.
       */
	  /* spec says this should be const, but pegasus isn't */ 
    void *hdl;

       /** Pointer to the Context Function Table.
       */
	/* spec says this should be const, but pegasus isn't */ 
    CMPIContextFT *ft;
  };




  /*
   //---------------------------------------------------
   //--
   //	_CMPIResult Encapsulated object
   //--
   //---------------------------------------------------
   */


   /** This structure represents the Encapsulated Result object.
   */
  struct _CMPIResult {

       /** Opaque pointer to MB specific implementation data.
       */
	  /* spec says this should be const, but pegasus isn't */ 
     void *hdl;

       /** Pointer to the Result Function Table.
       */
	 /* spec says this should be const, but pegasus isn't */ 
     CMPIResultFT *ft;
  };


  /*
   //---------------------------------------------------
   //--
   //	_CMPIResult Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to Result
       support sevices. Result support services are used to explicity return
       data produced by provider functions.
   */
  struct _CMPIResultFT {

       /** Function table version
       */
     int ftVersion;

       /** The Result object will not be used any further and may be freed by
           CMPI run time system.
	 @param rslt Result this pointer.
	 @return Service return status.
      */
     CMPIStatus (*release)
              (CMPIResult* rslt);

       /** Create an independent copy of this Result object.
	 @param rslt Result this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Pointer to copied Result object.
      */
     CMPIResult* (*clone)
              (const CMPIResult* rslt,CMPIStatus* rc);

       /** Return a value/type pair.
	 @param rslt Result this pointer.
	 @param value Address of a Value object.
	 @param type Type of the Value object.
	 @return Service return status.
      */
     CMPIStatus (*returnData)
              (const CMPIResult* rslt,const CMPIValue* value,const CMPIType type);

       /** Return a Instance object.
	 @param rslt Result this pointer.
	 @param inst Instance to be returned.
	 @return Service return status.
      */
     CMPIStatus (*returnInstance)
              (const CMPIResult* rslt,const CMPIInstance* inst);

       /** Return a ObjectPath object..
	 @param rslt Result this pointer.
	 @param ref ObjectPath to be returned.
	 @return Service return status.
      */
     CMPIStatus (*returnObjectPath)
              (const CMPIResult* rslt, const CMPIObjectPath* ref);

       /** Indicates no further data to be returned.
	 @param rslt Result this pointer.
	 @return Service return status.
      */
     CMPIStatus (*returnDone)
              (const CMPIResult* rslt);
  };




  /*
   //---------------------------------------------------
   //--
   //	_CMPIInstance Encapsulated object
   //--
   //---------------------------------------------------
   */


   /** This structure represents the Encapsulated Instance object.
   */
   struct _CMPIInstance {

       /** Opaque pointer to MB specific implementation data.
       */
	   /* Spec says this should be const, but it isn't in pegasus */
     void *hdl;

       /** Pointer to the Instance Function Table.
       */
	 /* Spec says this should be const, but it isn't in pegasus */
     CMPIInstanceFT* ft;
   };



  /*
   //---------------------------------------------------
   //--
   //	_CMPIInstance Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to Instance
       support sevices.
   */
   struct _CMPIInstanceFT {

       /** Function table version
       */
     int ftVersion;

       /** The Instance object will not be used any further and may be freed by
           CMPI run time system.
	 @param inst Instance this pointer.
	 @return Service return status.
      */
     CMPIStatus (*release)
              (CMPIInstance* inst);

       /** Create an independent copy of this Instance object. The resulting
           object must be released explicitly.
	 @param inst Instance this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Pointer to copied Instance object.
      */
     CMPIInstance* (*clone)
              (const CMPIInstance* inst, CMPIStatus* rc);

       /** Gets a named property value.
	 @param inst Instance this pointer.
	 @param name Property name.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Property value.
      */
     CMPIData (*getProperty)
              (const CMPIInstance* inst, const char* name, CMPIStatus* rc);

       /** Gets a Property value defined by its index.
	 @param inst Instance this pointer.
	 @param index Position in the internal Data array.
	 @param name Output: Returned property name (suppressed when NULL).
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Property value.
      */
     CMPIData (*getPropertyAt)
              (const CMPIInstance* inst, unsigned int index, CMPIString** name,
	       CMPIStatus* rc);

      /** Gets the number of properties contained in this Instance.
	 @param inst Instance this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Number of properties.
      */
     unsigned int (*getPropertyCount)
              (const CMPIInstance* inst, CMPIStatus* rc);

      /** Adds/replaces a named Property.
	 @param inst Instance this pointer.
         @param name Entry name.
         @param value Address of value structure.
         @param type Value type.
	 @return Service return status.
      */
     CMPIStatus (*setProperty)
              (const CMPIInstance* inst, const char* name,
               const CMPIValue* value, CMPIType type);

      /** Generates an ObjectPath out of the namespace, classname and
	  key propeties of this Instance.
	 @param inst Instance this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
         @return the generated ObjectPath.
      */
     CMPIObjectPath* (*getObjectPath)
              (const CMPIInstance* inst, CMPIStatus* rc);

      /** Directs CMPI to ignore any setProperty operations for this
	  instance for any properties not in this list.
	 @param inst Instance this pointer.
	 @param propertyList If not NULL, the members of the array define one
	     or more Property names to be accepted by setProperty operations.
	 @param keys Array of key property names of this instance. This array
	     must be specified.
	 @return Service return status.
      */
     CMPIStatus (*setPropertyFilter)
              (CMPIInstance* inst, const char **propertyList, const char **keys);

#ifdef CMPI_VER_100
     CMPIStatus (*setObjectPath)
         (CMPIInstance*, const CMPIObjectPath*);
#endif
   };




  /*
   //---------------------------------------------------
   //--
   //	_CMPIObjectPath Encapsulated object
   //--
   //---------------------------------------------------
   */


   /** This structure represents the Encapsulated Instance object.
   */
   struct _CMPIObjectPath {

       /** Opaque pointer to MB specific implementation data.
       */
	   /* Spec says this should be const, but it isn't in pegasus */
     void *hdl;

       /** Pointer to the ObjectPath Function Table.
       */
	 /* Spec says this should be const, but it isn't in pegasus */
     CMPIObjectPathFT* ft;
   };



  /*
   //---------------------------------------------------
   //--
   //	_CMPIObjectPath Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to ObjectPath
       support sevices.
   */
   struct _CMPIObjectPathFT {

       /** Function table version
       */
     int ftVersion;

       /** The ObjectPath object will not be used any further and may be freed by
           CMPI run time system.
	 @param op ObjectPath this pointer.
	 @return Service return status.
      */
     CMPIStatus (*release)
              (CMPIObjectPath* op);

       /** Create an independent copy of this ObjectPath object. The resulting
           object must be released explicitly.
	 @param op ObjectPath this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Pointer to copied ObjectPath object.
      */
     CMPIObjectPath* (*clone)
              (const CMPIObjectPath* op,CMPIStatus* rc);

       /** Set/replace the namespace component.
	 @param op ObjectPath this pointer.
	 @param ns The namespace string
	 @return Service return status.
      */
     CMPIStatus (*setNameSpace)
              (CMPIObjectPath* op,const char* ns);

       /** Get the namespace component.
	 @param op ObjectPath this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return The namespace component.
      */
     CMPIString* (*getNameSpace)
              (const CMPIObjectPath* op, CMPIStatus* rc);

       /** Set/replace the hostname component.
	 @param op ObjectPath this pointer.
	 @param hn The hostname string
	 @return Service return status.
      */
     CMPIStatus (*setHostname)
              (CMPIObjectPath* op, const char* hn);

       /** Get the hostname component.
	 @param op ObjectPath this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return The hostname component.
      */
     CMPIString* (*getHostname)
              (const CMPIObjectPath* op,CMPIStatus* rc);

       /** Set/replace the classname component.
	 @param op ObjectPath this pointer.
	 @param cn The hostname string
	 @return Service return status.
      */
     CMPIStatus (*setClassName)
              (CMPIObjectPath* op, const char* cn);

       /** Get the classname component.
	 @param op ObjectPath this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return The classname component.
      */
     CMPIString* (*getClassName)
              (const CMPIObjectPath* op,CMPIStatus* rc);

      /** Adds/replaces a named key property.
	 @param op ObjectPath this pointer.
         @param name Key property name.
         @param value Address of value structure.
         @param type Value type.
	 @return Service return status.
      */
     CMPIStatus (*addKey)
              (CMPIObjectPath* op,const char* name,
               const CMPIValue* value, const CMPIType type);

       /** Gets a named key property value.
	 @param op ObjectPath this pointer.
	 @param name Key property name.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Entry value.
      */
     CMPIData (*getKey)
              (const CMPIObjectPath* op, const char* name, CMPIStatus* rc);

       /** Gets a key property value defined by its index.
	 @param op ObjectPath this pointer.
	 @param index Position in the internal Data array.
	 @param name Output: Returned property name (suppressed when NULL).
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Data value.
      */
     CMPIData (*getKeyAt)
              (const CMPIObjectPath* op,unsigned int index, CMPIString** name,
	       CMPIStatus* rc);

      /** Gets the number of key properties contained in this ObjectPath.
	 @param op ObjectPath this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Number of properties.
      */
     unsigned int (*getKeyCount)
              (const CMPIObjectPath* op, CMPIStatus* rc);

      /** Set/replace namespace and classname components from <src>.
	 @param op ObjectPath this pointer.
	 @param src Source input.
	 @return Service return status.
      */
     CMPIStatus (*setNameSpaceFromObjectPath)
              (CMPIObjectPath* op, const CMPIObjectPath* src);

      /** Set/replace hostname, namespace and classname components from <src>.
	 @param op ObjectPath this pointer.
	 @param src Source input.
	 @return Service return status.
      */
     CMPIStatus (*setHostAndNameSpaceFromObjectPath)
              (CMPIObjectPath* op,
               const CMPIObjectPath* src);



     /* optional qualifier support */


       /** Get class qualifier value.
	 @param op ObjectPath this pointer.
	 @param qName Qualifier name.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Qualifier value.
      */
     CMPIData (*getClassQualifier)
              (const CMPIObjectPath* op,
               const char *qName,
               CMPIStatus *rc);

       /** Get property qualifier value.
	 @param op ObjectPath this pointer.
	 @param pName Property name.
	 @param qName Qualifier name.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Qualifier value.
      */
     CMPIData (*getPropertyQualifier)
              (const CMPIObjectPath* op,
               const char *pName,
               const char *qName,
               CMPIStatus *rc);

       /** Get method qualifier value.
	 @param op ObjectPath this pointer.
	 @param mName Method name.
	 @param qName Qualifier name.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Qualifier value.
      */
     CMPIData (*getMethodQualifier)
              (const CMPIObjectPath* op,
               const char *methodName,
               const char *qName,
               CMPIStatus *rc);

       /** Get method parameter quailifier value.
	 @param op ObjectPath this pointer.
	 @param mName Method name.
	 @param pName Parameter name.
	 @param qName Qualifier name.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Qualifier value.
      */
     CMPIData (*getParameterQualifier)
              (const CMPIObjectPath* op,
               const char *mName,
               const char *pName,
               const char *qName,
               CMPIStatus *rc);

#if defined(CMPI_VER_86)
      /** Generates a well formed string representation of this ObjectPath
	 @param op ObjectPath this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return String representation.
      */
     CMPIString *(*toString)
              (const CMPIObjectPath* op, CMPIStatus *rc);
#endif
   };




  /*
   //---------------------------------------------------
   //--
   //	_CMPISelectExp Encapsulated object
   //--
   //---------------------------------------------------
   */


   /** This structure represents the Encapsulated SelectExp object.
   */
   struct _CMPISelectExp {

       /** Opaque pointer to MB specific implementation data.
       */
	   /* Spec says this should be const, but it isn't in pegasus */
     void *hdl;

       /** Pointer to the SelExp Function Table.
       */
	 /* Spec says this should be const, but it isn't in pegasus */
     CMPISelectExpFT* ft;
   };



  /*
   //---------------------------------------------------
   //--
   //	_CMPISelectExpFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to SelectExp
       support sevices.
   */
   struct _CMPISelectExpFT {

       /** Function table version
       */
     int ftVersion;

       /** The SelectExp object will not be used any further and may be freed by
           CMPI run time system.
	 @param se SelectExp this pointer.
	 @return Service return status.
      */
     CMPIStatus (*release)
              (CMPISelectExp* se);

       /** Create an independent copy of this SelectExp object. The resulting
           object must be released explicitly.
	 @param se SelectExp this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Pointer to copied SelectExp object.
      */
     CMPISelectExp* (*clone)
              (const CMPISelectExp* se, CMPIStatus* rc);

       /** Evaluate the instance using this select expression.
	 @param se SelectExp this pointer.
	 @param inst Instance to be evaluated.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return True or false incicator.
      */
     CMPIBoolean (*evaluate)
              (const CMPISelectExp* se, const CMPIInstance* inst, CMPIStatus* rc);

       /** Return the select expression in string format.
	 @param se SelectExp this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return The select expression.
      */
     CMPIString* (*getString)
              (const CMPISelectExp* se, CMPIStatus* rc);

       /** Return the select expression as disjunction of conjunctions.
	 @param se SelectExp this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return The disjunction.
      */
     CMPISelectCond* (*getDOC)
              (const CMPISelectExp* se, CMPIStatus* rc);

       /** Return the select expression as conjunction of disjunctions.
	 @param se SelectExp this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return The conjunction.
      */
     CMPISelectCond* (*getCOD)
              (const CMPISelectExp* se, CMPIStatus* rc);

       /** Evaluate this select expression by using a data value accessor routine.
	 @param se SelectExp this pointer.
	 @param accessor Address of data accessor routine.
	 @param parm Data accessor routine parameter.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return True or false incicator.
      */
#if defined(CMPI_VER_87)
     CMPIBoolean (*evaluateUsingAccessor)
              (const CMPISelectExp* se,  CMPIAccessor *accessor, void *parm, CMPIStatus* rc);
#endif
   };




  /*
   //---------------------------------------------------
   //--
   //	_CMPISelectCond Encapsulated object
   //--
   //---------------------------------------------------
   */


   /** This structure represents the Encapsulated SelectCond object.
   */
   struct _CMPISelectCond {

       /** Opaque pointer to MB specific implementation data.
       */
     void *hdl;

       /** Pointer to the SelCond Function Table.
       */
     CMPISelectCondFT* ft;
   };



  /*
   //---------------------------------------------------
   //--
   //	_CMPISelectCondFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to SelectCond
       support sevices.
   */
   struct _CMPISelectCondFT {

       /** Function table version
       */
     int ftVersion;

       /** The SelectCond object will not be used any further and may be freed by
           CMPI run time system.
	 @param sc SelectCond this pointer.
	 @return Service return status.
      */
     CMPIStatus (*release)
             (CMPISelectCond* sc);

       /** Create an independent copy of this SelectCond object. The resulting
           object must be released explicitly.
	 @param sc SelectCond this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Pointer to copied SelectExp object.
      */
     CMPISelectCond* (*clone)
              (const CMPISelectCond* sc, CMPIStatus* rc);

       /** Return the number of sub conditions that are partof this SelectCond.
           Optionally, the SelectCond type (COD or DOC) will be returned.
	 @param sc SelectCond this pointer.
	 @param type Output: SelectCond type (suppressed when NULL).
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Number of SubCond elements.
      */
     CMPICount (*getCountAndType)
              (const CMPISelectCond* sc, int* type, CMPIStatus* rc);

       /** Return a SubCond element based on its index.
	 @param sc SelectCond this pointer.
	 @param index Position in the internal SubCoind array.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return The indexed SubCond element.
      */
     CMPISubCond* (*getSubCondAt)
              (const CMPISelectCond* sc, unsigned int index, CMPIStatus* rc);
   };




  /*
   //---------------------------------------------------
   //--
   //	_CMPISubCond Encapsulated object
   //--
   //---------------------------------------------------
   */


   /** This structure represents the Encapsulated SubCond object.
   */
   struct _CMPISubCond {

       /** Opaque pointer to MB specific implementation data.
       */
     void *hdl;

       /** Pointer to the SubCond Function Table.
       */
     CMPISubCondFT* ft;
   };



  /*
   //---------------------------------------------------
   //--
   //	_CMPISubCondFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to SubCond
       support sevices.
   */
   struct _CMPISubCondFT {

       /** Function table version
       */
     int ftVersion;

       /** The SubCond object will not be used any further and may be freed by
           CMPI run time system.
	 @param sc SubCond this pointer.
	 @return Service return status.
      */
     CMPIStatus (*release)
             (CMPISubCond* sc);

       /** Create an independent copy of this SubCond object. The resulting
           object must be released explicitly.
	 @param se SubCond this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Pointer to copied SelectExp object.
      */
     CMPISubCond* (*clone)
              (const CMPISubCond* sc,CMPIStatus* rc);

       /** Return the number of predicates that are part of sub condition.
	 @param sc SubCond this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Number of Predicate elements.
      */
     CMPICount (*getCount)
              (const CMPISubCond* sc, CMPIStatus* rc);

       /** Return a Predicate element based on its index.
	 @param sc SubCond this pointer.
	 @param index Position in the internal Predicate array.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return The indexed Predicate element.
      */
     CMPIPredicate* (*getPredicateAt)
              (const CMPISubCond* sc, unsigned int index, CMPIStatus* rc);

       /** Return a named Predicate element.
	 @param sc SubCond this pointer.
	 @param name Predicate name (property name).
	 @param rc Output: Service return status (suppressed when NULL).
	 @return The named Predicate element.
      */
     CMPIPredicate* (*getPredicate)
              (const CMPISubCond* sc, const char* name, CMPIStatus* rc);
   };




  /*
   //---------------------------------------------------
   //--
   //	_CMPIPredicate Encapsulated object
   //--
   //---------------------------------------------------
   */


   /** This structure represents the Encapsulated Predicate object.
   */
  struct _CMPIPredicate {

       /** Opaque pointer to MB specific implementation data.
       */
     void *hdl;

       /** Pointer to the Predicate Function Table.
       */
     CMPIPredicateFT* ft;
   };




  /*
   //---------------------------------------------------
   //--
   //	_CMPIPredicateFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to SubCond
       support sevices.
   */
   struct _CMPIPredicateFT {

       /** Function table version
       */
     int ftVersion;

       /** The Predicate object will not be used any further and may be freed by
           CMPI run time system.
	 @param pr Predicate this pointer.
	 @return Service return status.
      */
     CMPIStatus (*release)
             (CMPIPredicate* pr);

       /** Create an independent copy of this Predicate object. The resulting
           object must be released explicitly.
	 @param pr Predicate this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Pointer to copied Predicate object.
      */
     CMPIPredicate* (*clone)
              (const CMPIPredicate* pr, CMPIStatus* rc);

       /** Get the predicate components.
	 @param pr Predicate this pointer.
	 @param type Property type.
	 @param op Predicate operation.
	 @param lhs Left hand side of predicate.
	 @param rhs Right hand side of predicate.
	 @return Service return status.
      */
     CMPIStatus (*getData)
              (const CMPIPredicate* pr, CMPIType* type,
               CMPIPredOp* op, CMPIString** lhs, CMPIString** rhs);

       /** Evaluate the predicate using a specific value.
	 @param pr Predicate this pointer.
	 @param type Property type.
	 @param value Address of value structure.
	 @param type Value type.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Evaluation result.
      */
#if defined(CMPI_VER_87) && !defined(CMPI_VER_100)
// Deprecated in the CMPI spec.
     int (*evaluate)
        (CMPIPredicate* pr, CMPIValue* value,
               CMPIType type, CMPIStatus* rc);
#endif

#if defined(CMPI_VER_100)
     CMPIBoolean (*evaluateUsingAccessor)
       (const CMPIPredicate*,  CMPIAccessor *, void *, CMPIStatus *rc);
#endif
   };




  /*
   //---------------------------------------------------
   //--
   //	_CMPIArgs Encapsulated object
   //--
   //---------------------------------------------------
   */


   /** This structure represents the Encapsulated Args object.
   */
   struct _CMPIArgs {

       /** Opaque pointer to MB specific implementation data.
       */
     void *hdl;

       /** Pointer to the Args Function Table.
       */
     CMPIArgsFT* ft;
   };



  /*
   //---------------------------------------------------
   //--
   //	_CMPIArgsFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to Args
       support sevices.
   */
   struct _CMPIArgsFT{

       /** Function table version
       */
     int ftVersion;

       /** The Args object will not be used any further and may be freed by
           CMPI run time system.
	 @param as Args this pointer.
	 @return Service return status.
      */
     CMPIStatus (*release)
              (CMPIArgs* as);

       /** Create an independent copy of this Args object. The resulting
           object must be released explicitly.
	 @param as Args this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Pointer to copied Args object.
      */
     CMPIArgs* (*clone)
              (const CMPIArgs* as, CMPIStatus* rc);

      /** Adds/replaces a named argument.
	 @param as Args this pointer.
         @param name Argument name.
         @param value Address of value structure.
         @param type Value type.
	 @return Service return status.
      */
     CMPIStatus (*addArg)
              (CMPIArgs* as, const char* name ,const CMPIValue* value,
               const CMPIType type);

       /** Gets a named argument value.
	 @param as Args this pointer.
	 @param name Argument name.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Argument value.
      */
     CMPIData (*getArg)
              (const CMPIArgs* as, const char* name, CMPIStatus* rc);

       /** Gets a Argument value defined by its index.
	 @param as Args this pointer.
	 @param index Position in the internal Data array.
	 @param name Output: Returned argument name (suppressed when NULL).
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Argument value.
      */
     CMPIData (*getArgAt)
              (const CMPIArgs* as, unsigned int index, CMPIString** name,
	       CMPIStatus* rc);

      /** Gets the number of arguments contained in this Args.
	 @param as Args this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Number of properties.
      */
     unsigned int (*getArgCount)
              (const CMPIArgs* as, CMPIStatus* rc);
   };




  /*
   //---------------------------------------------------
   //--
   //	_CMPIString Encapsulated object
   //--
   //---------------------------------------------------
   */


   /** This structure represents the Encapsulated String object.
   */
   struct _CMPIString {

       /** Opaque pointer to MB specific implementation data.
       */
	   /* Spec says this should be const, but it isn't in pegasus */
     void *hdl;

       /** Pointer to the String Function Table.
       */
	 /* Spec says this should be const, but it isn't in pegasus */
     CMPIStringFT* ft;
   };



  /*
   //---------------------------------------------------
   //--
   //	_CMPIStringFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to String
       support sevices.
   */
   struct _CMPIStringFT {

       /** Function table version
       */
     int ftVersion;

       /** The String object will not be used any further and may be freed by
           CMPI run time system.
	 @param st String this pointer.
	 @return Service return status.
       */
     CMPIStatus (*release)
             (CMPIString* st);

       /** Create an independent copy of this String object. The resulting
           object must be released explicitly.
	 @param st String this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Pointer to copied String object.
      */
     CMPIString* (*clone)
             (const CMPIString* st, CMPIStatus* rc);

       /** Get a pointer to a C char* representation of this String.
	 @param st String this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Pointer to char* representation.
      */
     char* (*getCharPtr)
             (const CMPIString* st, CMPIStatus* rc);
   };




  /*
   //---------------------------------------------------
   //--
   //	_CMPIArray Encapsulated object
   //--
   //---------------------------------------------------
   */


   /** This structure represents the Encapsulated Array object.
   */
   struct _CMPIArray {

       /** Opaque pointer to MB specific implementation data.
       */
	   /* Spec says this should be const, but it isn't in pegasus */
     void *hdl;

       /** Pointer to the Array Function Table.
       */
	 /* Spec says this should be const, but it isn't in pegasus */
     CMPIArrayFT* ft;
   };



  /*
   //---------------------------------------------------
   //--
   //	_CMPIArrayFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to Array
       support sevices.
   */
   struct _CMPIArrayFT {

       /** Function table version
       */
     int ftVersion;

       /** The Array object will not be used any further and may be freed by
           CMPI run time system.
	 @param ar Array this pointer.
	 @return Service return status.
       */
     CMPIStatus (*release)
             (CMPIArray* ar);

       /** Create an independent copy of this Array object. The resulting
           object must be released explicitly.
	 @param ar Array this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Pointer to copied Array object.
      */
     CMPIArray* (*clone)
             (const CMPIArray* ar, CMPIStatus* rc);

      /** Gets the number of elements contained in this Array.
	 @param ar Array this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Number of elements.
      */
     CMPICount (*getSize)
             (const CMPIArray* ar, CMPIStatus* rc);

      /** Gets the element type.
	 @param ar Array this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Number of elements.
      */
     CMPIType (*getSimpleType)
             (const CMPIArray* ar, CMPIStatus* rc);

       /** Gets an element value defined by its index.
	 @param ar Array this pointer.
	 @param index Position in the internal Data array.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Element value.
      */
     CMPIData (*getElementAt)
             (const CMPIArray* ar, CMPICount index, CMPIStatus* rc);

       /** Sets an element value defined by its index.
	 @param ar Array this pointer.
	 @param index Position in the internal Data array.
         @param value Address of value structure.
         @param type Value type.
	 @return Service return status.
      */
     CMPIStatus (*setElementAt)
             (CMPIArray* ar, CMPICount index, const CMPIValue* value, CMPIType type);
   };





  /*
   //---------------------------------------------------
   //--
   //	_CMPIEnumeration Encapsulated object
   //--
   //---------------------------------------------------
   */


   /** This structure represents the Encapsulated Enumeration object.
   */
   struct _CMPIEnumeration {

       /** Opaque pointer to MB specific implementation data.
       */
	   /* Spec says this should be const, but it isn't in pegasus */
     void *hdl;

       /** Pointer to the Enumeration Function Table.
       */
	 /* Spec says this should be const, but it isn't in pegasus */
     CMPIEnumerationFT* ft;
   };



  /*
   //---------------------------------------------------
   //--
   //	_CMPIEnumerationFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to Enumeration
       support sevices.
   */
   struct _CMPIEnumerationFT {

       /** Function table version
       */
     int ftVersion;

       /** The Enumeration object will not be used any further and may be freed by
           CMPI run time system.
	 @param en Enumeration this pointer.
	 @return Service return status.
       */
     CMPIStatus (*release)
             (CMPIEnumeration* en);

       /** Create an independent copy of this Enumeration object. The resulting
           object must be released explicitly.
	 @param en Enumeration this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Pointer to copied Enumeration object.
      */
     CMPIEnumeration* (*clone)
             (const CMPIEnumeration* en, CMPIStatus* rc);

       /** Get the next element of this Enumeration.
	 @param en Enumeration this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Element value.
      */
     CMPIData (*getNext)
             (const CMPIEnumeration* en, CMPIStatus* rc);

       /** Test for any elements left in this Enumeration.
	 @param en Enumeration this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return True or false.
      */
     CMPIBoolean (*hasNext)
             (const CMPIEnumeration* en, CMPIStatus* rc);

       /** Convert this Enumeration into an Array.
	 @param en Enumeration this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return The Array.
      */
     CMPIArray* (*toArray)
             (const CMPIEnumeration* en, CMPIStatus* rc);
  };





  /*
   //---------------------------------------------------
   //--
   //	_CMPIDateTime Encapsulated object
   //--
   //---------------------------------------------------
   */


   /** This structure represents the DateTime object.
   */
  struct _CMPIDateTime {

       /** Opaque pointer to MB specific implementation data.
       */
	  /* Spec says this should be const, but it isn't in pegasus */
     void *hdl;

       /** Pointer to the DateTime Function Table.
       */
	 /* Spec says this should be const, but it isn't in pegasus */
     CMPIDateTimeFT *ft;
   };



  /*
   //---------------------------------------------------
   //--
   //	_CMPIDateTimeFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to DateTime
       support sevices.
   */
   struct _CMPIDateTimeFT {

       /** Function table version
       */
     int ftVersion;

       /** The DateTime object will not be used any further and may be freed by
           CMPI run time system.
	 @param dt DateTime this pointer.
	 @return Service return status.
       */
     CMPIStatus (*release)
             (CMPIDateTime* dt);

       /** Create an independent copy of this DateTime object. The resulting
           object must be released explicitly.
	 @param dt DateTime this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return Pointer to copied DateTime object.
      */
     CMPIDateTime* (*clone)
             (const CMPIDateTime* dt, CMPIStatus* rc);

       /** Get DateTime setting in binary format (in microsecods
	       starting since 00:00:00 GMT, Jan 1,1970).
	 @param dt DateTime this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return DateTime in binary.
      */
     CMPIUint64 (*getBinaryFormat)
             (const CMPIDateTime* dt, CMPIStatus* rc);

       /** Get DateTime setting in UTC string format.
	 @param dt DateTime this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return DateTime as UTC string.
      */
     CMPIString* (*getStringFormat)
             (const CMPIDateTime* dt, CMPIStatus* rc);

       /** Tests whether DateTime is an interval value.
	 @param dt DateTime this pointer.
	 @param rc Output: Service return status (suppressed when NULL).
	 @return True if interval value.
      */
     CMPIBoolean (*isInterval)
              (const CMPIDateTime* dt, CMPIStatus* rc);
  };






  /*
   //---------------------------------------------------
   //--
   //	_CMPIInstanceMI Instance Provider object
   //--
   //---------------------------------------------------
   */


   /** This structure represents an Instance provider.
   */
   typedef struct _CMPIInstanceMIFT CMPIInstanceMIFT;
   typedef struct _CMPIInstanceMI {

       /** Opaque pointer to Provider specific implementation data.
       */
      void *hdl;

       /** Pointer to the Instance Provider Function Table.
       */
      CMPIInstanceMIFT *ft;
   } CMPIInstanceMI;



  /*
   //---------------------------------------------------
   //--
   //	_CMPIInstanceMIFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to Instance
       provider functions. This table must be returend during initialization
       by the provider.
   */
   struct _CMPIInstanceMIFT {

       /** Function table version
       */
     int ftVersion;

       /** Provider version
       */
     int miVersion;

       /** Provider name
       */
     char *miName;            

       /** Cleanup is called prior to unloading of the provider.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context
	 @param terminating When true, the terminating argument indicates that the
		MB is in the process of terminating and that cleanup must be done. 
		When set to false, the MI may respond with CMPI_IRC_DO_NOT_UNLOAD, or 
		CMPI_IRC_NEVER_UNLOAD indicating that unload will interfere with 
		current MI processing. 
	 @return Function return status.
      */
     CMPIStatus (*cleanup)
            (CMPIInstanceMI* mi, CMPIContext* ctx, CMPIBoolean terminating);

       /** Enumerate ObjectPaths of Instances serviced by this provider.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context.
	 @param rslt Result data container.
	 @param op ObjectPath containing namespace and classname components.
	 @return Function return status.
      */
     CMPIStatus (*enumInstanceNames)
             (CMPIInstanceMI* mi, CMPIContext* ctx, CMPIResult* rslt,
              CMPIObjectPath* op);

       /** Enumerate the Instances serviced by this provider.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context.
	 @param rslt Result data container.
	 @param op ObjectPath containing namespace and classname components.
	 @param properties If not NULL, the members of the array define one or
	     more Property names. Each returned Object MUST NOT include elements
	     for any Properties missing from this list.
	 @return Function return status.
      */
     CMPIStatus (*enumInstances)
             (CMPIInstanceMI* mi, CMPIContext* ctx, CMPIResult* rslt,
              CMPIObjectPath* op, const char** properties);

       /** Get the Instances defined by <op>.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context.
	 @param rslt Result data container.
	 @param op ObjectPath containing namespace, classname and key components.
	 @param properties If not NULL, the members of the array define one or
	     more Property names. Each returned Object MUST NOT include elements
	     for any Properties missing from this list.
	 @return Function return status.
      */
     CMPIStatus (*getInstance)
             (CMPIInstanceMI* mi, CMPIContext* ctx, CMPIResult* rslt,
              CMPIObjectPath* op, const char** properties);

       /** Create Instance from <inst> using <op> as reference.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context.
	 @param rslt Result data container.
	 @param op ObjectPath containing namespace, classname and key components.
	 @param inst The Instance.
	 @return Function return status.
      */
     CMPIStatus (*createInstance)
             (CMPIInstanceMI* mi, CMPIContext* ctx, CMPIResult* rslt,
              CMPIObjectPath* op, CMPIInstance* inst);

       /** Replace an existing Instance from <inst> using <op> as reference.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context.
	 @param rslt Result data container.
	 @param op ObjectPath containing namespace, classname and key components.
	 @param inst The Instance.
	 @param properties If not NULL, the members of the array define one or
	     more Property names. The process MUST NOT replace elements
	     for any Properties missing from this list. If NULL all properties
	     will be replaced.
	 @return Function return status.
      */
     CMPIStatus (*modifyInstance)
             (CMPIInstanceMI* mi,  CMPIContext* ctx, CMPIResult* rslt,
              CMPIObjectPath* op, CMPIInstance* inst, const char** properties);

       /** Delete an existing Instance defined by <op>.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context.
	 @param rslt Result data container.
	 @param op ObjectPath containing namespace, classname and key components.
	 @return Function return status.
      */
     CMPIStatus (*deleteInstance)
             (CMPIInstanceMI* mi, CMPIContext* ctx, CMPIResult* rslt,
              CMPIObjectPath* op);

      /** Query the enumeration of instances of the class (and subclasses) defined
         by <op> using <query> expression.
	 @param mi Provider this pointer.
	 @param ctx Context object
	 @param rslt Result data container.
	 @param op ObjectPath containing namespace and classname components.
	 @param query Query expression
	 @param lang Query language
	 @return Function return status.
      */
     CMPIStatus (*execQuery)
             (CMPIInstanceMI*, CMPIContext*, CMPIResult*,
              CMPIObjectPath*, char*, char*);
   };






  /*
   //---------------------------------------------------
   //--
   //	_CMPIAssociationMI Association Provider object
   //--
   //---------------------------------------------------
   */


   /** This structure represents an Association provider.
   */
   typedef struct _CMPIAssociationMIFT CMPIAssociationMIFT;
   typedef struct _CMPIAssociationMI {

       /** Opaque pointer to Provider specific implementation data.
       */
      void *hdl;

       /** Pointer to the Association Provider Function Table.
       */
      CMPIAssociationMIFT *ft;
   } CMPIAssociationMI;


  /*
   //---------------------------------------------------
   //--
   //	_CMPIAssociationMIFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to Association
       provider functions. This table must be returend during initialization
       by the provider.
   */
   struct _CMPIAssociationMIFT {

       /** Function table version
       */
     int ftVersion;

       /** Provider version
       */
     int miVersion;

       /** Provider name
       */
     char *miName;

       /** Cleanup is called prior to unloading of the provider.
	 @param mi Provider this pointer.
	 @param ctx Invocation Contconst
	 @param terminating When true, the terminating argument indicates that the
		MB is in the process of terminating and that cleanup must be done. 
		When set to false, the MI may respond with CMPI_IRC_DO_NOT_UNLOAD, or 
		CMPI_IRC_NEVER_UNLOAD indicating that unload will interfere with 
		current MI processing. 
	 @return Function return status.
      */
     CMPIStatus (*cleanup)
             (CMPIAssociationMI* mi, CMPIContext* ctx, CMPIBoolean terminating);

      /** Enumerate ObjectPaths associated with the Instance defined by <op>.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context
	 @param rslt Result data container.
	 @param op Source ObjectPath containing namespace, classname and key components.
	 @param assocClass If not NULL, MUST be a valid Association Class name.
	    It acts as a filter on the returned set of Objects by mandating that
	    each returned Object MUST be associated to the source Object via an
	    Instance of this Class or one of its subclasses.
	 @param resultClass If not NULL, MUST be a valid Class name.
	    It acts as a filter on the returned set of Objects by mandating that
	    each returned Object MUST be either an Instance of this Class (or one
	    of its subclasses).
	 @param role If not NULL, MUST be a valid Property name.
	    It acts as a filter on the returned set of Objects by mandating
	    that each returned Object MUST be associated to the source Object
	    via an Association in which the source Object plays the specified role
	    (i.e. the name of the Property in the Association Class that refers
	    to the source Object MUST match the value of this parameter).
	 @param resultRole If not NULL, MUST be a valid Property name.
	    It acts as a filter on the returned set of Objects by mandating
	    that each returned Object MUST be associated to the source Object
	    via an Association in which the returned Object plays the specified role
	    (i.e. the name of the Property in the Association Class that refers to
	 @param properties If not NULL, the members of the array define one or more Property
	     names. Each returned Object MUST NOT include elements for any Properties
	     missing from this list. If NULL all properties must be returned.
	    the returned Object MUST match the value of this parameter).
	 @return Function return status.
     */
     CMPIStatus (*associators)
             (CMPIAssociationMI* mi, CMPIContext* ctx, CMPIResult* rslt,
              CMPIObjectPath* op, char* asscClass, char* resultClass,
              char* role, char* resultRole, const char** properties);

      /** Enumerate ObjectPaths associated with the Instance defined by <op>.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context
	 @param rslt Result data container.
	 @param op Source ObjectPath containing namespace, classname and key components.
	 @param assocClass If not NULL, MUST be a valid Association Class name.
	    It acts as a filter on the returned set of Objects by mandating that
	    each returned Object MUST be associated to the source Object via an
	    Instance of this Class or one of its subclasses.
	 @param resultClass If not NULL, MUST be a valid Class name.
	    It acts as a filter on the returned set of Objects by mandating that
	    each returned Object MUST be either an Instance of this Class (or one
	    of its subclasses).
	 @param role If not NULL, MUST be a valid Property name.
	    It acts as a filter on the returned set of Objects by mandating
	    that each returned Object MUST be associated to the source Object
	    via an Association in which the source Object plays the specified role
	    (i.e. the name of the Property in the Association Class that refers
	    to the source Object MUST match the value of this parameter).
	 @param resultRole If not NULL, MUST be a valid Property name.
	    It acts as a filter on the returned set of Objects by mandating
	    that each returned Object MUST be associated to the source Object
	    via an Association in which the returned Object plays the specified role
	    (i.e. the name of the Property in the Association Class that refers to
	    the returned Object MUST match the value of this parameter).
	 @return Function return status.
     */
     CMPIStatus (*associatorNames)
             (CMPIAssociationMI* mi, CMPIContext* ctx, CMPIResult* rslt,
              CMPIObjectPath* op, char* assocClass, char* resultClass,
              char* role, char* resultRole);

       /** Enumerates the association instances that refer to the instance defined by
           <op>.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context
	 @param rslt Result data container.
	 @param op Source ObjectPath containing namespace, classname and key components.
	 @param resultClass If not NULL, MUST be a valid Class name.
	    It acts as a filter on the returned set of Objects by mandating that
	    each returned Object MUST be either an Instance of this Class (or one
	    of its subclasses).
	 @param role If not NULL, MUST be a valid Property name.
	    It acts as a filter on the returned set of Objects by mandating
	    that each returned Object MUST be associated to the source Object
	    via an Association in which the source Object plays the specified role
	    (i.e. the name of the Property in the Association Class that refers
	    to the source Object MUST match the value of this parameter).
	 @param properties If not NULL, the members of the array define one or more Property
	     names. Each returned Object MUST NOT include elements for any Properties
	     missing from this list
	 @return Function return status.
     */
     CMPIStatus (*references)
             (CMPIAssociationMI* mi, CMPIContext* ctx, CMPIResult* rslt,
              CMPIObjectPath* op, char* resultClass, char* role ,
			  const char** properties);

      /** Enumerates the association ObjectPaths that refer to the instance defined by
           <op>.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context
	 @param rslt Result data container.
	 @param op Source ObjectPath containing namespace, classname and key components.
	 @param resultClass If not NULL, MUST be a valid Class name.
	    It acts as a filter on the returned set of Objects by mandating that
	    each returned Object MUST be either an Instance of this Class (or one
	    of its subclasses).
	 @param role If not NULL, MUST be a valid Property name.
	    It acts as a filter on the returned set of Objects by mandating
	    that each returned Object MUST be associated to the source Object
	    via an Association in which the source Object plays the specified role
	    (i.e. the name of the Property in the Association Class that refers
	    to the source Object MUST match the value of this parameter).
	 @return Function return status.
      */
     CMPIStatus (*referenceNames)
             (CMPIAssociationMI*, CMPIContext*, CMPIResult*,
              CMPIObjectPath*, char*, char*);
   };






  /*
   //---------------------------------------------------
   //--
   //	_CMPIMethodMI Method Provider object
   //--
   //---------------------------------------------------
   */


   /** This structure represents an Method provider.
   */
   typedef struct _CMPIMethodMIFT CMPIMethodMIFT;
   typedef struct _CMPIMethodMI {

       /** Opaque pointer to Provider specific implementation data.
       */
      void *hdl;

       /** Pointer to the Method Provider Function Table.
       */
      CMPIMethodMIFT *ft;
   } CMPIMethodMI;


  /*
   //---------------------------------------------------
   //--
   //	_CMPIMethodMIFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to Method
       provider functions. This table must be returend during initialization
       by the provider.
   */
   struct _CMPIMethodMIFT {

       /** Function table version
       */
     int ftVersion;

       /** Provider version
       */
     int miVersion;

       /** Provider name
       */
     char *miName;

       /** Cleanup is called prior to unloading of the provider.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context
	 @param terminating When true, the terminating argument indicates that the
		MB is in the process of terminating and that cleanup must be done. 
		When set to false, the MI may respond with CMPI_IRC_DO_NOT_UNLOAD, or 
		CMPI_IRC_NEVER_UNLOAD indicating that unload will interfere with 
		current MI processing. 
	 @return Function return status.
      */
     CMPIStatus (*cleanup)
             (CMPIMethodMI* mi, CMPIContext* ctx, CMPIBoolean terminating);

      /** Invoke a named, extrinsic method of an Instance
         defined by the <op> parameter.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context
	 @param rslt Result data container.
	 @param op ObjectPath containing namespace, classname and key components.
	 @param method Method name
	 @param in Input parameters.
	 @param out Output parameters.
	 @return Function return status.
      */
     CMPIStatus (*invokeMethod)
             (CMPIMethodMI* mi, CMPIContext* ctx, CMPIResult* rslt,
              CMPIObjectPath* op, char* method, CMPIArgs* in, CMPIArgs* out);
   };





  /*
   //---------------------------------------------------
   //--
   //	_CMPIPropertyMI Property Provider object
   //--
   //---------------------------------------------------
   */


   /** This structure represents an Property provider.
   */
   typedef struct _CMPIPropertyMIFT CMPIPropertyMIFT;
   typedef struct _CMPIPropertyMI {

       /** Opaque pointer to Provider specific implementation data.
       */
      void *hdl;

       /** Pointer to the Property Provider Function Table.
       */
      CMPIPropertyMIFT *ft;
   } CMPIPropertyMI;



  /*
   //---------------------------------------------------
   //--
   //	_CMPIPropertyMIFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to Property
       provider functions. This table must be returend during initialization
       by the provider.
   */
   struct _CMPIPropertyMIFT {

       /** Function table version
       */
     int ftVersion;

       /** Provider version
       */
     int miVersion;

       /** Provider name
       */
     char *miName;

       /** Cleanup is called prior to unloading of the provider.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context
	 @return Function return status.
      */
     CMPIStatus (*cleanup)
             (CMPIPropertyMI* mi, CMPIContext* ctx, CMPIBoolean terminating);

      /** Set the named property value of an Instance defined by the <op> parameter.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context
	 @param rslt Result data container.
	 @param op ObjectPath containing namespace, classname and key components.
	 @param name Property name
	 @param data Property value.
	 @return Function return status.
      */
     CMPIStatus (*setProperty)
             (CMPIPropertyMI* mi, CMPIContext* ctx, CMPIResult* rslt,
              CMPIObjectPath* op, char* name, CMPIData data);

      /** Get the named property value of an Instance defined by the <op> parameter.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context
	 @param rslt Result data container.
	 @param op ObjectPath containing namespace, classname and key components.
	 @param name Property name
	 @return Function return status.
      */
     CMPIStatus (*getProperty)
             (CMPIPropertyMI*, CMPIContext*, CMPIResult*,
              CMPIObjectPath*, char*);
   };





  /*
   //---------------------------------------------------
   //--
   //	_CMPIIndicationMI Indication Provider object
   //--
   //---------------------------------------------------
   */


   /** This structure represents an Indication provider.
   */
   typedef struct _CMPIIndicationMIFT CMPIIndicationMIFT;
   typedef struct _CMPIIndicationMI {

       /** Opaque pointer to Provider specific implementation data.
       */
      void *hdl;

       /** Pointer to the Property Provider Function Table.
       */
      CMPIIndicationMIFT *ft;
   } CMPIIndicationMI;



  /*
   //---------------------------------------------------
   //--
   //	_CMPIIndicationMIFT Function Table
   //--
   //---------------------------------------------------
   */


   /** This structure is a table of pointers providing access to Indication
       provider functions. This table must be returend during initialization
       by the provider.
   */
   struct _CMPIIndicationMIFT {

       /** Function table version
       */
     int ftVersion;

       /** Provider version
       */
     int miVersion;

       /** Provider name
       */
     char *miName;

       /** Cleanup is called prior to unloading of the provider.
	 @param mi Provider this pointer.
	 @param ctx Invocation Context
	 @param terminating When true, the terminating argument indicates that the
		MB is in the process of terminating and that cleanup must be done. 
		When set to false, the MI may respond with CMPI_IRC_DO_NOT_UNLOAD, or 
		CMPI_IRC_NEVER_UNLOAD indicating that unload will interfere with 
		current MI processing. 
	 @return Function return status.
      */
     CMPIStatus (*cleanup)
             (CMPIIndicationMI* mi, CMPIContext* ctx, CMPIBoolean terminating);
     CMPIStatus (*authorizeFilter)
             (CMPIIndicationMI* mi, CMPIContext* ctx, // CMPIResult* rslt,
              CMPISelectExp* se, char* ns, CMPIObjectPath* op, char* user);
     CMPIStatus (*mustPoll)
             (CMPIIndicationMI* mi, CMPIContext* ctx, // CMPIResult* rslt,
              CMPISelectExp* se, char* ns, CMPIObjectPath* op);
     CMPIStatus (*activateFilter)
            (CMPIIndicationMI* mi, CMPIContext* ctx, // CMPIResult* rslt,
             CMPISelectExp* se, char* ns, CMPIObjectPath* op, CMPIBoolean first);
     CMPIStatus (*deActivateFilter)
             (CMPIIndicationMI* mi, CMPIContext* ctx, // CMPIResult* rslt,
              CMPISelectExp* se, char* ns, CMPIObjectPath* op, CMPIBoolean last);
	 CMPIStatus (*enableIndications)
	   		(CMPIIndicationMI* mi, CMPIContext* ctx); 
	 CMPIStatus (*disableIndications)
	   		(CMPIIndicationMI* mi, CMPIContext* ctx); 
   };


#include <cmpimacs.h>

#ifdef __cplusplus
 };
#endif

#endif /* _CMPIFT_H_ */

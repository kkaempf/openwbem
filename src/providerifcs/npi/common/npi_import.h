/*
 * npi_import.h
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE 
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE 
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author: Frank Scheffler <frank.scheffler@de.ibm.com>
 * Contributors: Heidi Neumann <heidineu@de.ibm.com>
 *               Viktor Mihajlovski <mihajlov@de.ibm.com>
 *               Adrian Schuur <schuur@de.ibm.com>
 *               Marcin Gozdalik <gozdal@gozdal.eu.org>
 *
 * Description: All the tool functions available for the C provider programmer.
 */
#ifndef _NPI_IMPORT_H_
#define _NPI_IMPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

  /* Vector functions */  
  Vector VectorNew ( NPIHandle * );
#define VectorAddTo(h,v,o) _VectorAddTo ( h, v, o.ptr )
  void _VectorAddTo ( NPIHandle *, Vector, void * );
#define VectorGet(h,v,i,o) o.ptr=_VectorGet( h, v, i)
  void *  _VectorGet( NPIHandle *, Vector, int );
  int VectorSize( NPIHandle *, Vector );	

  /* CIMValue functions */
  CIMValue CIMValueNewString ( NPIHandle *,
				   const char * );
  CIMValue CIMValueNewInteger ( NPIHandle *, int );
  CIMValue CIMValueNewRef ( NPIHandle *, CIMObjectPath );

  /* CIMParameter functions */
  CIMType CIMParameterGetType( NPIHandle *, CIMParameter );
  char * CIMParameterGetName( NPIHandle *, CIMParameter );
  CIMParameter CIMParameterNewString ( NPIHandle *, const char *, 
				       const char * );
  CIMParameter CIMParameterNewInteger ( NPIHandle *, const char *, int );
  CIMParameter CIMParameterNewRef ( NPIHandle *, const char *, CIMObjectPath );
  char * CIMParameterGetString ( NPIHandle *, CIMParameter );

  /* Instance functions */ 
  CIMInstance CIMClassNewInstance ( NPIHandle *, CIMClass );
  void CIMInstanceSetStringProperty ( NPIHandle *, CIMInstance, const char *, 
				      const char * );
  void CIMInstanceSetIntegerProperty ( NPIHandle *, CIMInstance, const char *,
				       const int );
  void CIMInstanceSetLongProperty ( NPIHandle *, CIMInstance, const char *,
				       const long long );
  void CIMInstanceSetBooleanProperty ( NPIHandle *, CIMInstance, const char *,
				       const unsigned char );
  void CIMInstanceSetRefProperty ( NPIHandle *, CIMInstance, const char *, 
				   CIMObjectPath );
  char * CIMInstanceGetStringValue( NPIHandle *, CIMInstance, 
				    const char *);
  int CIMInstanceGetIntegerValue ( NPIHandle * , CIMInstance, 
				   const char * ); 
  CIMObjectPath CIMInstanceGetRefValue ( NPIHandle * , CIMInstance, 
					 const char * ); 
  
  /* Object Path functions */
  CIMObjectPath CIMObjectPathFromCIMInstance ( NPIHandle *, CIMInstance );
  CIMObjectPath CIMObjectPathNew ( NPIHandle *, const char * );
  char * CIMObjectPathGetClassName ( NPIHandle *, CIMObjectPath );
  char * CIMObjectPathGetNameSpace ( NPIHandle *, CIMObjectPath );
  void CIMObjectPathSetNameSpace ( NPIHandle *, CIMObjectPath, const char * );
  void CIMObjectPathSetNameSpaceFromCIMObjectPath ( NPIHandle *, CIMObjectPath,
						    CIMObjectPath );
  char * CIMObjectPathGetStringKeyValue ( NPIHandle *, CIMObjectPath, 
					  const char * );
  int CIMObjectPathGetIntegerKeyValue ( NPIHandle *, CIMObjectPath, 
					const char * );
  CIMObjectPath CIMObjectPathGetRefKeyValue ( NPIHandle *, CIMObjectPath, 
					      const char * );
  void CIMObjectPathAddStringKeyValue ( NPIHandle *, CIMObjectPath, 
					const char *, const char * );
  void CIMObjectPathAddIntegerKeyValue ( NPIHandle *, CIMObjectPath, 
					 const char *, int);
  void CIMObjectPathAddRefKeyValue ( NPIHandle *, CIMObjectPath, 
				     const char *, CIMObjectPath);

  /* SelectExp functions */
  char * SelectExpGetSelectString ( NPIHandle *, SelectExp );
  
  /* CIMOM functions */
  CIMClass CIMOMGetClass ( NPIHandle *, CIMObjectPath, int );
  Vector CIMOMEnumInstanceNames ( NPIHandle * , CIMObjectPath , int );
  Vector CIMOMEnumInstances ( NPIHandle * , CIMObjectPath , int, int );
  CIMInstance CIMOMGetInstance( NPIHandle *, CIMObjectPath, int );

  void CIMOMDeliverProcessEvent(NPIHandle*,char*,CIMInstance);
  void CIMOMDeliverInstanceEvent(NPIHandle*,char*,CIMInstance,CIMInstance,CIMInstance);
  NPIHandle* CIMOMPrepareAttach(NPIHandle*);
  void CIMOMCancelAttach(NPIHandle*);
  void CIMOMAttachThread(NPIHandle*);
  void CIMOMDetachThread(NPIHandle*);

  /* Error handling functions */
  int errorCheck ( NPIHandle * );
  int errorReset ( NPIHandle * );
  void raiseError ( NPIHandle *, const char * );
  
  /* General functions */
  char * _ObjectToString( NPIHandle *, void * );
#define ObjectToString(nh,o) _ObjectToString(nh,o.ptr)
#define ObjectIsNull(o) (o.ptr==(void*)0)
  
#ifdef __cplusplus
}
#endif

#endif


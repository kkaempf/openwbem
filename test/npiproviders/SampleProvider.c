/*
 *    SampleProvider.c
 *
 *    CIMOM C language sample provider.
 *
 *    author: Frank Scheffler (frank.scheffler@de.ibm.com)
 *
 */

/* define the types of provider you want to write */
#define INSTANCE_PROVIDER


/* you have to include those two files to write an InstanceProvider */
#include "provider.h"
#include "npi_import.h"

/* defines the name for the provider
 * in this case the library has to be SampleProvider.dll (win) or 
 * libSampleProvider.so (unix)
 */
PROVIDER_NAME (sampleprovider)


/* these two functions have to be written for any CIM provider */

static void initialize ( NPIHandle *npiHandle, CIMOMHandle ch)
{
}
static void cleanup ( NPIHandle *npiHandle ) 
{
}

/* these are the functions implemented by InstanceProvider, they have to be 
 * coded. They show a little example of how to use the native library tool 
 * functions to communicate with the JAVA CIMOM. 
 */
static Vector enumInstanceNames ( NPIHandle *npiHandle, CIMObjectPath cop, 
				  int bool_deep, CIMClass cc ) 
{
	Vector vec = VectorNew(npiHandle);
  return vec;
}

static Vector enumInstances ( NPIHandle *npiHandle, CIMObjectPath cop, 
			      int bool_deep, CIMClass cc, int bool_localOnly) 
{
  Vector vec = VectorNew( npiHandle );
  CIMInstance ci = CIMClassNewInstance ( npiHandle, cc );
  CIMInstanceSetStringProperty ( npiHandle, ci, "PropertyOne", "one" );
  CIMInstanceSetIntegerProperty ( npiHandle, ci, "PropertyTwo", 2 );
  VectorAddTo ( npiHandle, vec, ci );
  return vec;

}

static CIMInstance getInstance ( NPIHandle *npiHandle, CIMObjectPath cop, 
				 CIMClass cc, int bool_localOnly ) 
{
  CIMInstance ci = CIMClassNewInstance ( npiHandle, cc );
  CIMInstanceSetStringProperty ( npiHandle, ci, "PropertyOne", "one" );
  CIMInstanceSetIntegerProperty ( npiHandle, ci, "PropertyTwo", 2 );
  return ci;

}

static CIMObjectPath createInstance ( NPIHandle *npiHandle, CIMObjectPath cop,
				      CIMInstance ci) 
{
  return CIMObjectPathFromCIMInstance ( npiHandle, ci );
}

static void setInstance ( NPIHandle *npiHandle, CIMObjectPath cop, 
			  CIMInstance ci) 
{
}

static void deleteInstance ( NPIHandle *npiHandle, CIMObjectPath cop) 
{
}


static Vector execQuery ( NPIHandle *npiHandle, CIMObjectPath cop, 
			  const char * str, int i, CIMClass cc) 
{
  return VectorNew( npiHandle );
}

/*

static Vector associators ( NPIHandle *npiHandle, CIMObjectPath cop1, 
	CIMObjectPath cop2, const char *str1, const char *str2, const char *str3,
	int i1, int i2, const char * charptrptr[], int i3)
{
	return VectorNew( npiHandle );
}

static Vector associatorNames ( NPIHandle *npiHandle, CIMObjectPath cop1, 
	CIMObjectPath cop2, const char *str1, const char *str2, const char * str3)
{
	return VectorNew( npiHandle );
}

static Vector references ( NPIHandle *npiHandle, CIMObjectPath cop1, 
	CIMObjectPath cop2, const char *str1, int i1, int i2, 
	const char * charptrptr[], int i3)
{
	return VectorNew( npiHandle );
}
static Vector referenceNames ( NPIHandle *npiHandle, CIMObjectPath cop1, 
	CIMObjectPath cop2, const char *str )
{
	return VectorNew( npiHandle );
}

static CIMValue invokeMethod ( NPIHandle *npiHandle, CIMObjectPath cop, 
			       const char *str, Vector vec1, Vector vec2)
{
	CIMValue cv;
	return cv;
}

*/


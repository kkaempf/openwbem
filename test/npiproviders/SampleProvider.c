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
	(void)npiHandle;
	(void)ch;
}
static void cleanup ( NPIHandle *npiHandle ) 
{
	(void)npiHandle;
}

/* these are the functions implemented by InstanceProvider, they have to be 
 * coded. They show a little example of how to use the native library tool 
 * functions to communicate with the JAVA CIMOM. 
 */
static Vector enumInstanceNames ( NPIHandle *npiHandle, CIMObjectPath cop, 
				  int bool_deep, CIMClass cc ) 
{
	Vector vec = VectorNew(npiHandle);
	(void)cop;
	(void)bool_deep;
	(void)cc;
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

  (void)cop;
  (void)bool_deep;
  (void)bool_localOnly;
}

static CIMInstance getInstance ( NPIHandle *npiHandle, CIMObjectPath cop, 
				 CIMClass cc, int bool_localOnly ) 
{
  CIMInstance ci = CIMClassNewInstance ( npiHandle, cc );
  CIMInstanceSetStringProperty ( npiHandle, ci, "PropertyOne", "one" );
  CIMInstanceSetIntegerProperty ( npiHandle, ci, "PropertyTwo", 2 );
  return ci;

  (void)cop;
  (void)bool_localOnly;
}

static CIMObjectPath createInstance ( NPIHandle *npiHandle, CIMObjectPath cop,
				      CIMInstance ci) 
{
	(void)cop;
  return CIMObjectPathFromCIMInstance ( npiHandle, ci );
}

static void setInstance ( NPIHandle *npiHandle, CIMObjectPath cop, 
			  CIMInstance ci) 
{
	(void)npiHandle;
	(void)cop;
	(void)ci;
}

static void deleteInstance ( NPIHandle *npiHandle, CIMObjectPath cop) 
{
	(void)npiHandle;
	(void)cop;
}


/*
static Vector execQuery ( NPIHandle *npiHandle, CIMObjectPath cop, 
			  const char * str, int i, CIMClass cc) 
{
	(void)npiHandle;
	(void)cop;
	(void)str;
	(void)i;
	(void)cc;
  return VectorNew( npiHandle );
}


static Vector associators ( NPIHandle *npiHandle, CIMObjectPath cop1, 
	CIMObjectPath cop2, const char *str1, const char *str2, const char *str3,
	int i1, int i2, const char * charptrptr[], int i3)
{
	(void)cop1;
	(void)cop2;
	(void)str1;
	(void)str2;
	(void)str3;
	(void)i1;
	(void)i2;
	(void)charptrptr;
	(void)i3;
	return VectorNew( npiHandle );
}

static Vector associatorNames ( NPIHandle *npiHandle, CIMObjectPath cop1, 
	CIMObjectPath cop2, const char *str1, const char *str2, const char * str3)
{
	(void)cop1;
	(void)cop2;
	(void)str1;
	(void)str2;
	(void)str3;
	return VectorNew( npiHandle );
}

static Vector references ( NPIHandle *npiHandle, CIMObjectPath cop1, 
	CIMObjectPath cop2, const char *str1, int i1, int i2, 
	const char * charptrptr[], int i3)
{
	(void)cop1;
	(void)cop2;
	(void)str1;
	(void)i1;
	(void)i2;
	(void)charptrptr;
	(void)i3;
	return VectorNew( npiHandle );
}
static Vector referenceNames ( NPIHandle *npiHandle, CIMObjectPath cop1, 
	CIMObjectPath cop2, const char *str )
{
	(void)cop1;
	(void)cop2;
	(void)str;
	return VectorNew( npiHandle );
}

static CIMValue invokeMethod ( NPIHandle *npiHandle, CIMObjectPath cop, 
			       const char *str, Vector vec1, Vector vec2)
{
	CIMValue cv;
	return cv;
	(void)npiHandle;
	(void)cop;
	(void)str;
	(void)vec1;
	(void)vec2;
}

*/


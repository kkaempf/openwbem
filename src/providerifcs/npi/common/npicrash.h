/*
 * npicrash.h
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE 
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE 
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author: Viktor <mihajlov@de.ibm.com>
 * Contributors:
 *
 * Description: Functions to report the location of fatal NPI provider errors.
 */

#ifdef __cplusplus 
extern "C" {
#endif

void * setupCrashHandler();
void  restoreCrashHandler(void*);

#ifdef __cplusplus 
}
#endif

/*
 * os_compile.h
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE 
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE 
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author: Frank Scheffler <frank.scheffler@de.ibm.com>
 * Contributors:
 *
 * Description: Checks for the correct OS compile definition.
 */
#ifndef _OS_COMPILE_H_
#define _OS_COMPILE_H_

#ifdef _COMPILE_WIN32
#ifdef _COMPILE_UNIX
#error "You mustn't define both _COMPILE_WIN32 and _COMPILE_UNIX"
#endif
#else
#ifndef _COMPILE_UNIX
#error "You have to define either _COMPILE_WIN32 or _COMPILE_UNIX"
#endif
#endif

#endif

/*
 * perlArgs.h
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
 * Author: Markus Mueller <markus_mueller@de.ibm.com>
 * Contributors:
 *
 * Description: <Some descriptive text>
 */

#include <stdio.h>
#include <stdlib.h>

/* used in perlProvider.h and perlNPI/perlNPI.xs */
typedef struct {
  char * scriptName;
  void * my_perl;
} PerlContext;

static inline char * argsforperl(void * arg)
{
    //char my_arg[sizeof(void *)/4 + 3];
    char * my_arg;
    my_arg = malloc(sizeof(void *)/4 + 3);
    //my_arg = (char *)malloc(32/4 + 3);
    //my_arg = (char *)alloca(32/4 + 3);
    sprintf(my_arg,"%8p",arg);
    //return savepv(my_arg);
    return my_arg;
}
   
 
static inline void * argsfromperl(char * arg)
{
    void * my_arg;
    sscanf(arg,"%p",&my_arg);
    return my_arg;
}

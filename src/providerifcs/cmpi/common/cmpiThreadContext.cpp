
/*
 *
 * cmpiThreadContext.cpp
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
 * Description: CMPI Thread Context support
 *
 */

#include "cmpisrv.h"
#include <pthread.h>
#include <limits.h>
#include "OW_NonRecursiveMutex.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_Format.hpp"

static const unsigned long NOKEY = PTHREAD_KEYS_MAX+1;
unsigned long CMPI_ThreadContext::theKey=NOKEY;
static OW_NonRecursiveMutex keyGuard;

CMPI_ThreadContext* CMPI_ThreadContext::getContext()
{
	return (CMPI_ThreadContext*)pthread_getspecific(theKey);
}

void CMPI_ThreadContext::setContext()
{
	pthread_key_t k;
	int rc = pthread_key_create(&k,NULL);
	if (rc != 0)
		OW_THROW(OW_Exception, format("pthread_key_create failed. error = %1", rc).c_str());

	rc = pthread_setspecific(k,this);
	if (rc != 0)
		OW_THROW(OW_Exception, format("pthread_setspecific failed. error = %1", rc).c_str());

	theKey = k;
	std::cout<<"--- setThreadContext(1) theKey: " << theKey << std::endl;
}

#undef KEY_VAL

void CMPI_ThreadContext::setThreadContext()
{
	// if this is the first time setThreadContext() has run.
	// hiKey is initially PTHREAD_KEYS_MAX+1, but then gets set as 
	// CMPI_ThreadContext objects are created.
	if (theKey == NOKEY)
	{
		// double-checked locking pattern
		OW_NonRecursiveMutexLock l(keyGuard);
		if (theKey == NOKEY)
		{
			m_prev=NULL;

			// this adds us to the tsd and sets m_key
			setContext();

			return;
		}
	}

	// another context already exists or existed

	// if we get one, then one exists
	m_prev=getContext();

	// set this as the context
	int rc = pthread_setspecific(theKey,this);
	if (rc != 0)
		OW_THROW(OW_Exception, format("pthread_setspecific failed. error = %1", rc).c_str());

	return;
}

void CMPI_ThreadContext::add(CMPI_Object *o)
{
	ENQ_TOP_LIST(o,CIMfirst,CIMlast,next,prev);
}

void CMPI_ThreadContext::addObject(CMPI_Object* o)
{
	CMPI_ThreadContext* ctx=getContext();
	ctx->add(o);
}

void CMPI_ThreadContext::remove(CMPI_Object *o)
{
	DEQ_FROM_LIST(o,CIMfirst,CIMlast,next,prev);
}

void CMPI_ThreadContext::remObject(CMPI_Object* o)
{
	CMPI_ThreadContext* ctx=getContext();
	ctx->remove(o);
}

CMPI_ThreadContext::CMPI_ThreadContext()
	: m_prev(0)
	, CIMfirst(0)
	, CIMlast(0)
{
	setThreadContext();
}

CMPI_ThreadContext::~CMPI_ThreadContext()
{

	for (CMPI_Object *nxt,*cur=CIMfirst; cur; cur=nxt)
	{
		nxt=cur->next;
		((CMPIInstance*)cur)->ft->release((CMPIInstance*)cur);
	}

	if (m_prev != NULL)
	{
		int rc = pthread_setspecific(theKey, m_prev);
		if (rc != 0)
			OW_THROW(OW_Exception, format("pthread_setspecific failed. error = %1", rc).c_str());
		return;
	}
}

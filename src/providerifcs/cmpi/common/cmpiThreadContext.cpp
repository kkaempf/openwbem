
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
 *
 * Contributor:   Markus Mueller <sedgewick_de@yahoo.de>
 *
 * Description: CMPI Thread Context support
 *
 */

#include "cmpisrv.h"
#include <pthread.h>
#include "blocxx/NonRecursiveMutex.hpp"
#include "blocxx/NonRecursiveMutexLock.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/ThreadImpl.hpp"
#include "OW_Exception.hpp"

pthread_key_t CMPI_ThreadContext::theKey;
namespace
{

//////////////////////////////////////////////////////////////////////
pthread_once_t once_control = PTHREAD_ONCE_INIT;

} // end unsigned namespace

CMPI_ThreadContext* CMPI_ThreadContext::getThreadContext()
{
	// set up our TLS which will be used to store the Thread* in.
	pthread_once(&once_control, &initializeTheKey);

	return (CMPI_ThreadContext*)pthread_getspecific(theKey);
}

namespace OW_NAMESPACE
{
OW_DECLARE_EXCEPTION(CMPI_ThreadContext)
OW_DEFINE_EXCEPTION(CMPI_ThreadContext)
}

void CMPI_ThreadContext::setContext()
{
	// set up our TLS which will be used to store the Thread* in.
	pthread_once(&once_control, &initializeTheKey);

	int rc = pthread_setspecific(theKey,this);
	if (rc != 0)
		OW_THROW(OpenWBEM::CMPI_ThreadContextException, blocxx::Format("pthread_setspecific failed. error = %1", rc).c_str());

	//std::cout<<"--- setThreadContext(1) theKey: " << theKey << std::endl;
}


void CMPI_ThreadContext::setThreadContext()
{
	// if we get one, then one exists
	m_prev=getThreadContext();

	// this adds us to the tsd and sets m_key
	setContext();
}

void CMPI_ThreadContext::add(CMPI_Object *o)
{
	ENQ_TOP_LIST(o,CIMfirst,CIMlast,next,prev);
}

void CMPI_ThreadContext::addObject(CMPI_Object* o)
{
	CMPI_ThreadContext* ctx=getThreadContext();
	ctx->add(o);
}

void CMPI_ThreadContext::remove(CMPI_Object *o)
{
	DEQ_FROM_LIST(o,CIMfirst,CIMlast,next,prev);
}

void CMPI_ThreadContext::remObject(CMPI_Object* o)
{
	CMPI_ThreadContext* ctx=getThreadContext();
	ctx->remove(o);
}

CMPIBroker * CMPI_ThreadContext::getBroker()
{
	return getThreadContext()->broker;
}

CMPIContext * CMPI_ThreadContext::getContext()
{
	return getThreadContext()->context;
}

CMPI_ThreadContext::CMPI_ThreadContext()
	: m_prev(0)
	, CIMfirst(0)
	, CIMlast(0)
{
	setThreadContext();
}

CMPI_ThreadContext::CMPI_ThreadContext(CMPIBroker * mb, CMPIContext * ctx)
	: m_prev(0)
	, CIMfirst(0)
	, CIMlast(0)
	, broker(mb)
	, context(ctx)
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

	pthread_setspecific(theKey, m_prev);
}

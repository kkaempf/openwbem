/*
 * npicrash.c
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


#if defined(i386) && defined(__GNUC__)
#define _GNU_SOURCE

#include <dlfcn.h>
#include <signal.h>
#include <malloc.h>
#include <stdio.h>
#include <dlfcn.h>
#include "npicrash.h"


/* i386 stack frame -- can we have make this less architecture dependent? */
struct stackframe
{
	struct stackframe * sf_previous;
	void * sf_ip;
};

/*
 * Note: besides the somewhat questionable stack frame above we also
 * use the  
 */
static void segv_handler(int signum, siginfo_t * info, void * obscure)
{
	Dl_info dllinfo;
	struct stackframe * sfp;
	void ** stackitems=obscure;

	(void)signum;
	(void)info;

	if (stackitems==NULL)
	{
		fprintf(stderr,
			"Segmentation violation occurred (no location information)\n");
		return;
	}

	fprintf(stderr,"Segmentation violation occurred at %10p",stackitems[19]);
	if ( dladdr(stackitems[19],&dllinfo) )
		fprintf(stderr," in %s(%s)%+d\n",
			dllinfo.dli_fname,dllinfo.dli_sname,
			stackitems[19]-dllinfo.dli_saddr);
	else
		fprintf(stderr," (no module information)\n");

	fprintf(stderr, "Exception backtrace:\n");
	sfp=stackitems?stackitems[11]:NULL;
	while (sfp)
	{
		if (dladdr(sfp->sf_ip,&dllinfo))
			fprintf(stderr,"%10p: %s(%s)\n",
				sfp->sf_ip,dllinfo.dli_fname,dllinfo.dli_sname);
		else
			fprintf(stderr,"%10p: no module information\n",sfp->sf_ip);
		sfp=sfp->sf_previous;
	}
}  
#endif // #if defined(i386) && defined(__GNUC__)

void * setupCrashHandler()
{
#if defined(i386) && defined(__GNUC__)
	/* set up signal handler and return the old handler info */
	sigset_t sigset;
	struct sigaction action, *oldaction;

	sigemptyset(&sigset);
	action.sa_sigaction=segv_handler;
	action.sa_mask=sigset;
	action.sa_flags=SA_SIGINFO|SA_ONESHOT;
	action.sa_restorer=NULL;

	if ( sigaction(SIGSEGV,&action,oldaction=malloc(sizeof(struct sigaction))) )
		perror("error setting signal handler:");
	fprintf(stderr,"Set new sigaction=%p(%p,%8x) oldaction=%p(%p,%8x)\n",
		&action,
		action.sa_sigaction,
		action.sa_flags,
		oldaction,
		oldaction->sa_sigaction,
		oldaction->sa_flags);
	return oldaction;
#else
	return 0;
#endif // #if defined(i386) && defined(__GNUC__)
}

void  restoreCrashHandler(void *oldac)
{
#if defined(i386) && defined(__GNUC__)
	/* restore the old signal handler */
	struct sigaction * oldaction=oldac, origaction;
	if (oldaction)
	{
		if (sigaction(SIGSEGV,oldaction,&origaction) )
			perror("error setting signal handler:");
		fprintf(stderr,"Reset sigaction from %p(%p,%8x) to %p(%p,%8x)\n",
			&origaction,
			origaction.sa_sigaction,
			origaction.sa_flags,
			oldaction,
			oldaction->sa_sigaction,
			oldaction->sa_flags);
		free(oldaction);
	}
#else
	(void)oldac;
#endif // #if defined(i386) && defined(__GNUC__)
}


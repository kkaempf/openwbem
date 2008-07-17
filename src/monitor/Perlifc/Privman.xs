#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include <fcntl.h>
#include "OW_privman.h"

#define BUFSZ 1024

void owpm_append_string(char const * s, void * v_arr)
{
	av_push((AV *)v_arr, newSVpv(s, 0));
}

char const * * owpm_av_to_strarr(AV * av, char const * first)
{
	int i;
	int start = first ? 1 : 0;
	int len = start + av_len(av) + 1;
	char const * * rv = (char const * *)malloc((len + 1) * sizeof(rv[0]));
	if (start)
	{
		rv[0] = first;
	}
	for (i = start; i < len; ++i)
	{
		rv[i] = SvPV_nolen(*av_fetch(av, i - start, 0));
	}
	rv[len] = (char const *)0;
	return rv;
}

struct conv
{
	char const * mode_str;
	unsigned flags;
} const owpm_mode_conversions[] =
{
	"<", E_OW_PRIVMAN_IN,
	">", E_OW_PRIVMAN_OUT,
	">>", E_OW_PRIVMAN_OUT | E_OW_PRIVMAN_APP,
	"+<", E_OW_PRIVMAN_IN | E_OW_PRIVMAN_OUT,
	"+>", E_OW_PRIVMAN_IN | E_OW_PRIVMAN_OUT | E_OW_PRIVMAN_TRUNC,
	"+>>", E_OW_PRIVMAN_IN | E_OW_PRIVMAN_OUT | E_OW_PRIVMAN_APP
};

unsigned owpm_mode2flags(char const * mode)
{
	size_t i;
	size_t n = sizeof(owpm_mode_conversions) / sizeof(struct conv);
	for (i = 0; i < n; ++i)
	{
		struct conv const * p = owpm_mode_conversions + i;	
		if (strcmp(mode, p->mode_str) == 0)
		{
			return p->flags;
		}
	}
	return 0;
}

void owpm_get_status_rep(AV * av, int * rep1, int * rep2, char const * func)
{
	if (av_len(av) != 1)
	{
		croak("Bad status value passed to %s", func);
	}
	*rep1 = (int)SvIV(*av_fetch(av, 0, 0));
	*rep2 = (int)SvIV(*av_fetch(av, 1, 0));
}

SV * owpm_make_status_rep(int rep1, int rep2)
{
	AV * reparr = (AV *)sv_2mortal((SV *)newAV());
	av_push(reparr, newSViv(rep1));
	av_push(reparr, newSViv(rep2));
	return newRV_inc((SV *)reparr);
}

#define ERRCHK(e) \
if (e) \
{ \
	croak("%s", errbuf); \
}

MODULE = Privman		PACKAGE = Privman		

int
open_descriptor(pathname, mode, perms)
	const char * pathname
	const char * mode
	unsigned perms
	INIT:
		char errbuf[BUFSZ];
		unsigned flags;
		int errnum;
	CODE:
		flags = owpm_mode2flags(mode);
		if (flags == 0)
		{
			croak("Unknown mode string %s", mode);
		}
		errnum = owprivman_open(pathname, flags, perms, &RETVAL, errbuf, BUFSZ);
		if (errnum == ENOENT)
		{
			RETVAL = -1;
		}
		else
		{
			ERRCHK(errnum);
		}
	OUTPUT:
		RETVAL

SV *
read_directory(pathname, keep_special)
	const char * pathname
	int keep_special
	INIT:
		char errbuf[BUFSZ];
		AV * results = (AV *)sv_2mortal((SV *)newAV());
		int e;
	CODE:
		e = owprivman_read_directory(
			pathname, keep_special, &owpm_append_string, (void *)results,
			errbuf, BUFSZ
		);
		ERRCHK(e);
		RETVAL = newRV_inc((SV *)results);
	OUTPUT:
		RETVAL

void
rename(old_path, new_path)
	const char * old_path
	const char * new_path
	INIT:
		char errbuf[BUFSZ];
	CODE:
		ERRCHK(owprivman_rename(old_path, new_path, errbuf, BUFSZ));

int
remove_file(path)
	const char * path
	INIT:
		char errbuf[BUFSZ];
	CODE:
		ERRCHK(owprivman_remove_file(path, &RETVAL, errbuf, BUFSIZ));
	OUTPUT:
		RETVAL

int
remove_dir(path)
	const char * path
	INIT:
		char errbuf[BUFSZ];
	CODE:
		ERRCHK(owprivman_remove_dir(path, &RETVAL, errbuf, BUFSIZ));
	OUTPUT:
		RETVAL

int
monitored_spawn(exec_path, app_name, av_argv, av_envp)
	const char * exec_path
	const char * app_name
	AV * av_argv
	AV * av_envp
	INIT:
		char errbuf[BUFSZ];
		char const * * argv = owpm_av_to_strarr(av_argv, exec_path);
		char const * * envp = owpm_av_to_strarr(av_envp, 0);
	CODE:
		ERRCHK(
			owprivman_monitored_spawn(
				exec_path, app_name, argv, envp, &RETVAL, errbuf, BUFSIZ));
	OUTPUT:
		RETVAL
	CLEANUP:
		free(argv);
		free(envp);

int
user_spawn(exec_path, av_argv, av_envp, user)
	const char * exec_path
	AV * av_argv
	AV * av_envp
	const char * user
	INIT:
		char errbuf[BUFSZ];
		char const * * argv = owpm_av_to_strarr(av_argv, exec_path);
		char const * * envp = owpm_av_to_strarr(av_envp, 0);
	CODE:
		ERRCHK(
			owprivman_user_spawn(
				exec_path, argv, envp, user, &RETVAL, errbuf, BUFSIZ));
	OUTPUT:
		RETVAL
	CLEANUP:
		free(argv);
		free(envp);

int
child_stdin(pid)
	int pid
	INIT:
		char errbuf[BUFSZ];
	CODE:
		ERRCHK(owprivman_child_stdin(pid, &RETVAL, errbuf, BUFSIZ));
	OUTPUT:
		RETVAL

int
child_stdout(pid)
	int pid
	INIT:
		char errbuf[BUFSZ];
	CODE:
		ERRCHK(owprivman_child_stdout(pid, &RETVAL, errbuf, BUFSIZ));
	OUTPUT:
		RETVAL

int
child_stderr(pid)
	int pid
	INIT:
		char errbuf[BUFSZ];
	CODE:
		ERRCHK(owprivman_child_stderr(pid, &RETVAL, errbuf, BUFSIZ));
	OUTPUT:
		RETVAL

SV *
child_status(pid)
	int pid
	INIT:
		char errbuf[BUFSZ];
		int rep1;
		int rep2;
	CODE:
		ERRCHK(owprivman_child_status(pid, &rep1, &rep2, errbuf, BUFSIZ));
		RETVAL = owpm_make_status_rep(rep1, rep2);
	OUTPUT:
		RETVAL

int running(av)
	AV * av
	INIT:
		int rep1;
		int rep2;
	CODE:
		owpm_get_status_rep(av, &rep1, &rep2, "Privman::running");
		RETVAL = owprivman_status_running(rep1, rep2);
	OUTPUT:
		RETVAL

int terminated(av)
	AV * av
	INIT:
		int rep1;
		int rep2;
	CODE:
		owpm_get_status_rep(av, &rep1, &rep2, "Privman::terminated");
		RETVAL = owprivman_status_terminated(rep1, rep2);
	OUTPUT:
		RETVAL

int exit_terminated(av)
	AV * av
	INIT:
		int rep1;
		int rep2;
	CODE:
		owpm_get_status_rep(av, &rep1, &rep2, "Privman::exit_terminated");
		RETVAL = owprivman_status_exit_terminated(rep1, rep2);
	OUTPUT:
		RETVAL

int exit_status(av)
	AV * av
	INIT:
		int rep1;
		int rep2;
	CODE:
		owpm_get_status_rep(av, &rep1, &rep2, "Privman::exit_status");
		RETVAL = owprivman_status_exit_status(rep1, rep2);
	OUTPUT:
		RETVAL

int terminated_successfully(av)
	AV * av
	INIT:
		int rep1;
		int rep2;
	CODE:
		owpm_get_status_rep(av, &rep1, &rep2, "Privman::terminated_successfully");
		RETVAL = owprivman_status_terminated_successfully(rep1, rep2);
	OUTPUT:
		RETVAL

int signal_terminated(av)
	AV * av
	INIT:
		int rep1;
		int rep2;
	CODE:
		owpm_get_status_rep(av, &rep1, &rep2, "Privman::signal_terminated");
		RETVAL = owprivman_status_signal_terminated(rep1, rep2);
	OUTPUT:
		RETVAL

int term_signal(av)
	AV * av
	INIT:
		int rep1;
		int rep2;
	CODE:
		owpm_get_status_rep(av, &rep1, &rep2, "Privman::term_signal");
		RETVAL = owprivman_status_term_signal(rep1, rep2);
	OUTPUT:
		RETVAL

int stopped(av)
	AV * av
	INIT:
		int rep1;
		int rep2;
	CODE:
		owpm_get_status_rep(av, &rep1, &rep2, "Privman::stopped");
		RETVAL = owprivman_status_stopped(rep1, rep2);
	OUTPUT:
		RETVAL

int stop_signal(av)
	AV * av
	INIT:
		int rep1;
		int rep2;
	CODE:
		owpm_get_status_rep(av, &rep1, &rep2, "Privman::stop_signal");
		RETVAL = owprivman_status_stop_signal(rep1, rep2);
	OUTPUT:
		RETVAL

SV *
child_wait_close_term(pid, wait_initial, wait_close, wait_term)
	int pid
	float wait_initial
	float wait_close
	float wait_term
	INIT:
		char errbuf[BUFSZ];
		int rep1;
		int rep2;
	CODE:
		ERRCHK(
			owprivman_child_wait_close_term(
				pid, wait_initial, wait_close, wait_term,
				&rep1, &rep2, errbuf, BUFSIZ));
		RETVAL = owpm_make_status_rep(rep1, rep2);
	OUTPUT:
		RETVAL


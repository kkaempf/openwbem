#ifndef OW_PRIVMAN_NOEXCEPT_H_INCLUDE_GUARD_
#define OW_PRIVMAN_NOEXCEPT_H_INCLUDE_GUARD_

// This is a pared down version of the OW_privman.h interface for use with
// Perl programs on AIX.  The important differences are that
// 1. There must be no exceptions thrown, not even exceptions caught internally.
// 2. Only the owprivman_open and owprivman_read_directory functions need
//    to be implemented.

#include <stdlib.h>
#include "OW_PrivManOpenFlags.h"

typedef void (*string_handler_t)(char const *, void *);

/*
 * For all of the following functions except the owprivman_status_* functions,
 * the return value is 0 for success and nonzero for failure.  On failure, an
 * error message is written to errbuf, which is an array of at least bufsize
 * characters, and the return value is the associated error code.
 */

/* *retval < 0 indicates that the file could not be opened because it does
   not exist.
*/
int owprivman_open(
	char const * pathname, unsigned flags, unsigned perms,
	int * retval, char * errbuf, size_t bufsize
);

int owprivman_read_directory(
	char const * pathname, int keep_special, string_handler_t h, void * p,
	char * errbuf, size_t bufsize
);

// The rest of these currently do nothing except return nonzero for failure.

int owprivman_rename(
	char const * old_path, char const * new_path,
	char * errbuf, size_t bufsize
);

int owprivman_remove_file(
	char const * path, int * retval, char * errbuf, size_t bufsize
);

int owprivman_remove_dir(
	char const * path, int * retval, char * errbuf, size_t bufsize
);

int owprivman_monitored_spawn(
	char const * exec_path, char const * app_name,
	char const * const * argv, char const * const * envp,
	int * retval, char * errbuf, size_t bufsize
);

int owprivman_user_spawn(
	char const * exec_path,
	char const * const * argv, char const * const * envp,
	char const * user,
	int * retval, char * errbuf, size_t bufsize
);

int owprivman_child_stdin(int pid, int * retval, char * errbuf, size_t bufsize);
int owprivman_child_stdout(int pid, int * retval, char * errbuf, size_t bufsize);
int owprivman_child_stderr(int pid, int * retval, char * errbuf, size_t bufsize);

int owprivman_child_status(int pid, int * rep1, int * rep2, char * errbuf, size_t bufsize);

int owprivman_status_running(int rep1, int rep2);
int owprivman_status_terminated(int rep1, int rep2);
int owprivman_status_exit_terminated(int rep1, int rep2);
int owprivman_status_exit_status(int rep1, int rep2);
int owprivman_status_terminated_successfully(int rep1, int rep2);
int owprivman_status_signal_terminated(int rep1, int rep2);
int owprivman_status_term_signal(int rep1, int rep2);
int owprivman_status_stopped(int rep1, int rep2);
int owprivman_status_stop_signal(int rep1, int rep2);

int owprivman_child_wait_close_term(
	int pid, float wait_initial, float wait_close, float wait_term,
	int * status_rep1, int * status_rep2, char * errbuf, size_t bufsize
);

#endif

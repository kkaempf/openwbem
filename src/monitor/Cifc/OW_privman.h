/*******************************************************************************
 * Copyright (C) 2007 Quest Software All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  - Neither the name of Quest Software nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL Quest Software OR THE CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

/**
 * @author Kevin VanHorn
 */

#ifndef OW_PRIVMAN_H_INCLUDE_GUARD_
#define OW_PRIVMAN_H_INCLUDE_GUARD_

#include "OW_config.h"
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

int owprivman_rename(
	char const * old_path, char const * new_path,
	char * errbuf, size_t bufsize
);

int owprivman_unlink(
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

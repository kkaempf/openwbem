/*-
 * Copyright (c) 1990, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)db.h	8.7 (Berkeley) 6/16/94
 */

#ifndef _DB_H_
#define	_DB_H_

#include "OW_config.h"
#include "OW_Types.h"

#include <sys/types.h>

#if defined(OW_HAVE_SYS_CDEFS_H)
# include <sys/cdefs.h>
#endif

#if !defined(__BEGIN_DECLS) || !defined(__END_DECLS)
# ifdef __cplusplus
#  define __BEGIN_DECLS extern "C" {
#  define __END_DECLS }
# else
#  define __BEGIN_DECLS
#  define __END_DECLS
# endif
#endif

#ifndef __P
# ifdef __cplusplus
#define __P(args) args throw()
# else
#define __P(args) args
# endif
#endif

#ifndef __PMT
#define __PMT(args) args 
#endif

#if defined(OW_HAVE_SYS_INT_TYPES_H)
#include <sys/int_types.h>
#endif

#include <limits.h>

#ifdef __DBINTERFACE_PRIVATE
#include "compat.h"
#endif

#define	RET_ERROR	-1		/* Return values. */
#define	RET_SUCCESS	 0
#define	RET_SPECIAL	 1


#define	MAX_PAGE_NUMBER	0xffffffff	/* >= # of pages in a file */
typedef OW_UInt32	pgno_t;
#define	MAX_PAGE_OFFSET	65535		/* >= # of bytes in a page */
typedef OW_UInt16	indx_t;
#define	MAX_REC_NUMBER	0xffffffff	/* >= # of records in a tree */
typedef OW_UInt32	recno_t;

/* Key/data structure -- a Data-Base Thang. */
typedef struct {
	void	*data;			/* data */
	size_t	 size;			/* data length */
} DBT;

/* Routine flags. */
#define	R_CURSOR	1		/* del, put, seq */
#define	__R_UNUSED	2		/* UNUSED */
#define	R_FIRST		3		/* seq */
#define	R_IAFTER	4		/* put (RECNO) */
#define	R_IBEFORE	5		/* put (RECNO) */
#define	R_LAST		6		/* seq (BTREE, RECNO) */
#define	R_NEXT		7		/* seq */
#define	R_NOOVERWRITE	8		/* put */
#define	R_PREV		9		/* seq (BTREE, RECNO) */
#define	R_SETCURSOR	10		/* put (RECNO) */
#define	R_RECNOSYNC	11		/* sync (RECNO) */

typedef enum { DB_BTREE, DB_HASH, DB_RECNO } DBTYPE;

/*
 * !!!
 * The following flags are included in the dbopen(3) call as part of the
 * open(2) flags.  In order to avoid conflicts with the open flags, start
 * at the top of the 16 or 32-bit number space and work our way down.  If
 * the open flags were significantly expanded in the future, it could be
 * a problem.  Wish I'd left another flags word in the dbopen call.
 *
 * !!!
 * None of this stuff is implemented yet.  The only reason that it's here
 * is so that the access methods can skip copying the key/data pair when
 * the DB_LOCK flag isn't set.
 */
#if UINT_MAX > 65535
#define	DB_LOCK		0x20000000	/* Do locking. */
#define	DB_SHMEM	0x40000000	/* Use shared memory. */
#define	DB_TXN		0x80000000	/* Do transactions. */
#else
#define	DB_LOCK		    0x2000	/* Do locking. */
#define	DB_SHMEM	    0x4000	/* Use shared memory. */
#define	DB_TXN		    0x8000	/* Do transactions. */
#endif


/* Access method description structure. */
typedef struct __db {
	DBTYPE type;			/* Underlying db type. */
	int (*close)	__PMT((struct __db *));
	int (*del)	__PMT((const struct __db *, const DBT *, OW_UInt32));
	int (*get)	__PMT((const struct __db *, const DBT *, DBT *, OW_UInt32));
	int (*put)	__PMT((const struct __db *, DBT *, const DBT *, OW_UInt32));
	int (*seq)	__PMT((const struct __db *, DBT *, DBT *, OW_UInt32));
	int (*sync)	__PMT((const struct __db *, OW_UInt32));
	void *internal;			/* Access method private. */
	int (*fd)	__PMT((const struct __db *));
} DB;

#define	BTREEMAGIC	0x053162
#define	BTREEVERSION	3

/* Structure used to pass parameters to the btree routines. */
typedef struct {
#define	R_DUP		0x01	/* duplicate keys */
	OW_UInt32	flags;
	OW_UInt32	cachesize;	/* bytes to cache */
	int	maxkeypage;	/* maximum keys per page */
	int	minkeypage;	/* minimum keys per page */
	OW_UInt32	psize;		/* page size */
	int	(*compare)	/* comparison function */
	    __PMT((const DBT *, const DBT *));
	size_t	(*prefix)	/* prefix function */
	    __PMT((const DBT *, const DBT *));
	int	lorder;		/* byte order */
} BTREEINFO;

#define	HASHMAGIC	0x061561
#define	HASHVERSION	2

/* Structure used to pass parameters to the hashing routines. */
typedef struct {
	OW_UInt32	bsize;		/* bucket size */
	OW_UInt32	ffactor;	/* fill factor */
	OW_UInt32	nelem;		/* number of elements */
	OW_UInt32	cachesize;	/* bytes to cache */
	OW_UInt32		/* hash function */
		(*hash) __PMT((const void *, size_t));
	int	lorder;		/* byte order */
} HASHINFO;

/* Structure used to pass parameters to the record routines. */
typedef struct {
#define	R_FIXEDLEN	0x01	/* fixed-length records */
#define	R_NOKEY		0x02	/* key not required */
#define	R_SNAPSHOT	0x04	/* snapshot the input */
	OW_UInt32	flags;
	OW_UInt32	cachesize;	/* bytes to cache */
	OW_UInt32	psize;		/* page size */
	int	lorder;		/* byte order */
	size_t	reclen;		/* record length (fixed-length records) */
	OW_UInt8	bval;		/* delimiting byte (variable-length records */
	char	*bfname;	/* btree file name */ 
} RECNOINFO;

#ifdef __DBINTERFACE_PRIVATE
/*
 * Little endian <==> big endian 32-bit swap macros.
 *	M_32_SWAP	swap a memory location
 *	P_32_SWAP	swap a referenced memory location
 *	P_32_COPY	swap from one location to another
 */
#define	M_32_SWAP(a) {							\
	OW_UInt32 _tmp = a;						\
	((char *)&a)[0] = ((char *)&_tmp)[3];				\
	((char *)&a)[1] = ((char *)&_tmp)[2];				\
	((char *)&a)[2] = ((char *)&_tmp)[1];				\
	((char *)&a)[3] = ((char *)&_tmp)[0];				\
}
#define	P_32_SWAP(a) {							\
	OW_UInt32 _tmp = *(OW_UInt32 *)a;				\
	((char *)a)[0] = ((char *)&_tmp)[3];				\
	((char *)a)[1] = ((char *)&_tmp)[2];				\
	((char *)a)[2] = ((char *)&_tmp)[1];				\
	((char *)a)[3] = ((char *)&_tmp)[0];				\
}
#define	P_32_COPY(a, b) {						\
	((char *)&(b))[0] = ((char *)&(a))[3];				\
	((char *)&(b))[1] = ((char *)&(a))[2];				\
	((char *)&(b))[2] = ((char *)&(a))[1];				\
	((char *)&(b))[3] = ((char *)&(a))[0];				\
}

/*
 * Little endian <==> big endian 16-bit swap macros.
 *	M_16_SWAP	swap a memory location
 *	P_16_SWAP	swap a referenced memory location
 *	P_16_COPY	swap from one location to another
 */
#define	M_16_SWAP(a) {							\
	OW_UInt16 _tmp = a;						\
	((char *)&a)[0] = ((char *)&_tmp)[1];				\
	((char *)&a)[1] = ((char *)&_tmp)[0];				\
}
#define	P_16_SWAP(a) {							\
	OW_UInt16 _tmp = *(OW_UInt16 *)a;				\
	((char *)a)[0] = ((char *)&_tmp)[1];				\
	((char *)a)[1] = ((char *)&_tmp)[0];				\
}
#define	P_16_COPY(a, b) {						\
	((char *)&(b))[0] = ((char *)&(a))[1];				\
	((char *)&(b))[1] = ((char *)&(a))[0];				\
}
#endif

__BEGIN_DECLS
#ifdef OW_WIN32
OW_DB_API 
#endif 
DB *dbopen __P((const char *, int, int, DBTYPE, const void *));

#ifdef __DBINTERFACE_PRIVATE
DB	*__bt_open __P((const char *, int, int, const BTREEINFO *, int));
DB	*__hash_open __P((const char *, int, int, const HASHINFO *, int));
DB	*__rec_open __P((const char *, int, int, const RECNOINFO *, int));
void	 __dbpanic __P((DB *dbp));
#endif
__END_DECLS
#endif /* !_DB_H_ */

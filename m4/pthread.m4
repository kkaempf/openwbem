AC_DEFUN(
	[OW_PTHREAD_SUPPORT],
	[
		dnl Now check for threading stuff
		OW_PUSH_VARIABLE(LIBS)
		if [[ "x$DO_NOT_USE_DASH_PTHREAD" = "x" ]]
		then
		  # Always link pthread dynamically.
		  LIBS="$DYNAMIC_LINK_FLAGS -pthread $CURRENT_LINK_MODE_FLAGS $LIBS"
		fi
		AC_MSG_CHECKING("pthread_mutexattr_settype")
		AC_TRY_COMPILE(
			[
		#include <pthread.h>
		#include <assert.h>
			],
			[
			pthread_mutexattr_t attr;
			pthread_mutex_t mutex;
			int res = 0;
			res = pthread_mutexattr_init(&attr);
			assert(res == 0);
			res = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
			assert(res == 0);
			res = pthread_mutex_init(&mutex, &attr);
			assert(res == 0);
			return 0;
			],
			[
				AC_DEFINE(HAVE_PTHREAD_MUTEXATTR_SETTYPE)
				AC_MSG_RESULT("yes")
			],
			[ AC_MSG_RESULT("no") ]
		)

		AC_MSG_CHECKING("pthread_spin_lock")
		AC_TRY_COMPILE(
			[
		#include <pthread.h>
		#include <assert.h>
			],
			[
			pthread_spinlock_t spinlock;
			if (pthread_spin_init(&spinlock, 0) != 0)
				return 1;
			if (pthread_spin_lock(&spinlock) != 0)
				return 1;
			if (pthread_spin_unlock(&spinlock) != 0)
				return 1;
			return 0;
			],
			[
				AC_DEFINE(HAVE_PTHREAD_SPIN_LOCK)
				AC_MSG_RESULT("yes")
			],
			[ AC_MSG_RESULT("no") ]
		)

		AC_MSG_CHECKING("pthread_barrier_init")
		AC_TRY_COMPILE(
			[
		#include <pthread.h>
		#include <assert.h>
			],
			[
			pthread_barrier_t barrier;
			assert(pthread_barrier_init(&barrier, NULL, 1) == 0);
			assert(pthread_barrier_wait(&barrier) == PTHREAD_BARRIER_SERIAL_THREAD);
			assert(pthread_barrier_destroy(&barrier) == 0);
			return 0;
			],
			[
				AC_DEFINE(HAVE_PTHREAD_BARRIER)
				AC_MSG_RESULT("yes")
			],
			[ AC_MSG_RESULT("no") ]
		)

		AC_CHECK_FUNCS(pthread_kill_other_threads_np)

		# restore flags after pthread tests
		OW_POP_VARIABLE(LIBS)
	]
)

/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
* Copyright (C) 2004 Novell, Inc. All rights reserved.
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
*  - Neither the name of Vintela, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#ifdef OW_HAVE_OPENSSL
#include "OW_SSLCtxMgr.hpp"
#include "OW_GetPass.hpp"
#include "OW_Format.hpp"
#include "OW_FileSystem.hpp"
#include "OW_RandomNumber.hpp"
#include "OW_ThreadImpl.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_Assertion.hpp"
#include "OW_MD5.hpp"
#include "OW_Array.hpp"
#include "OW_Exec.hpp"
#include "OW_Thread.hpp"

#include <openssl/rand.h>
#include <openssl/err.h>
#include <cstring>
#include <csignal>
#ifndef OW_WIN32
#include <sys/time.h>
#include <sys/resource.h>
#endif
#include <fcntl.h>

#ifdef OW_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef OW_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef OW_HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef OW_DEBUG
#include <iostream>
#endif

#include <fstream>

// This struct has to be in the global namespace
extern "C"
{
struct CRYPTO_dynlock_value
{
	OW_NAMESPACE::Mutex mutex;
};
}

namespace OW_NAMESPACE
{

namespace
{

OW_NAMESPACE::Mutex* mutex_buf = 0;

extern "C"
{

static struct CRYPTO_dynlock_value * dyn_create_function(const char *,int)
{
	return new CRYPTO_dynlock_value;
}

// these need to still be static, since they get exported because of extern "C"
static void dyn_lock_function(int mode, struct CRYPTO_dynlock_value *l,
				  const char *, int)
{
	if (mode & CRYPTO_LOCK)
	{
		l->mutex.acquire();
	}
	else
	{
		l->mutex.release();
	}
}

static void dyn_destroy_function(struct CRYPTO_dynlock_value *l,
				 const char *, int)
{
	delete l;
}

static unsigned long id_function()
{
	return (unsigned long)OW_NAMESPACE::ThreadImpl::thread_t_ToUInt64(OW_NAMESPACE::ThreadImpl::currentThread());
}

static void locking_function(int mode, int n, const char*, int)
{
	if (mode & CRYPTO_LOCK)
	{
		mutex_buf[n].acquire();
	}
	else
	{
		mutex_buf[n].release();
	}
}
} // end extern "C"

/**
 * @param randFilePath the directory name to check.
 */
bool randFilePathIsSecure(const String& randFilePath)
{
	OW_ASSERT(!randFilePath.empty());

#ifdef OW_WIN32
	// TODO: write this
	return false;
#else
	// only load or write the file if it's "the directory in which the file resides and all parent directories should have only write access
	// enabled for the directory owner" (Network Security with OpenSSL p. 101).  This is so that we don't load up a rogue random
	// file. If we load one someone created which we didn't write, or someone else gets it, our security is blown!
	// Also, check that the owner of each directory is either the current user or root, just to be completely paranoid!
	String dir;
	try
	{
		dir = FileSystem::Path::realPath(randFilePath);
	}
	catch (FileSystemException&)
	{
		return false;
	}
	OW_ASSERT(!dir.empty() && dir[0] == '/');

	// now check all dirs
	do
	{
		struct stat dirStats;
		if (::lstat(dir.c_str(), &dirStats) == -1)
		{
			return false;
		}
		else
		{
			// if either group or other write access is enabled, then fail.
			if ((dirStats.st_mode & S_IWGRP == S_IWGRP) ||
				(dirStats.st_mode & S_IWOTH == S_IWOTH) )
			{
				return false;
			}
			// no hard links allowed
			if (dirStats.st_nlink > 1)
			{
				return false;
			}
			// must own it or else root
			if (dirStats.st_uid != ::getuid() && dirStats.st_uid != 0)
			{
				return false;
			}
			// directory
			if (!S_ISDIR(dirStats.st_mode))
			{
				return false;
			}
		}


		size_t lastSlash = dir.lastIndexOf('/');
		dir = dir.substring(0, lastSlash);
	} while (!dir.empty());

	return true;
#endif
}

bool randFileIsSecure(const char* randFile)
{
	if (!randFilePathIsSecure(FileSystem::Path::dirname(randFile)))
	{
		return false;
	}

#ifdef OW_WIN32
	// TODO: write this
	return false;
#else

	// only load or write the file if it's "owned by the user ID of the application, and all access to group members and other users should be
	// disallowed. Additionally, the directory in which the file resides and all parent directories should have only write access
	// enabled for the directory owner" (Network Security with OpenSSL p. 101).  This is so that we don't load up a rogue random
	// file. If we load one someone created which we didn't write, or someone else gets it, our security is blown!
	struct stat randFileStats;
	if (::lstat(randFile, &randFileStats) == -1)
	{
		return false;
	}
	else
	{
		// if either group or other write access is enabled, then fail.
		if ((randFileStats.st_mode & S_IWGRP == S_IWGRP) ||
			(randFileStats.st_mode & S_IWOTH == S_IWOTH) )
		{
			return false;
		}
		// no hard links allowed
		if (randFileStats.st_nlink > 1)
		{
			return false;
		}
		// must own it
		if (randFileStats.st_uid != ::getuid())
		{
			return false;
		}
		// regular file
		if (!S_ISREG(randFileStats.st_mode))
		{
			return false;
		}
	}

	return true;
#endif
}

// These are used to generate random data via signal delivery timing differences.
// We have to use global data since it's modified from a signal handler.
volatile sig_atomic_t g_counter;
volatile unsigned char* g_data;
volatile sig_atomic_t g_dataIdx;
int g_dataSize;

extern "C"
{
// this needs to still be static, since it gets exported because of extern "C"
static void randomALRMHandler(int sig)
{
	if (g_dataIdx < g_dataSize)
	{
		g_data[g_dataIdx++] ^= g_counter & 0xFF;
	}
}
}

Mutex g_randomTimerGuard;

#ifndef OW_WIN32
// This function will continue to iterate until *iterations <= 0. *iterations may be set by another thread. *iterations should not be < 8.
void generateRandomTimerData(unsigned char* data, int size, int* iterations)
{
	OW_ASSERT(data != 0);
	OW_ASSERT(size > 0);
	OW_ASSERT(iterations != 0);

	// make sure we only have one thread running this at a time.
	MutexLock l(g_randomTimerGuard);

	// set up the global data for the signal handler
	g_data = data;
	g_dataSize = size;
	g_dataIdx = 0;

	// install our ALRM handler
	struct sigaction sa, osa;
	sa.sa_handler = randomALRMHandler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGALRM, &sa, &osa);

	// Start timer
	struct ::itimerval tv, otv;
	tv.it_value.tv_sec = 0;
	tv.it_value.tv_usec = 10 * 1000; // 10 ms
	tv.it_interval = tv.it_value;
	setitimer(ITIMER_REAL, &tv, &otv);
	
	while ((*iterations)-- > 0)
	{
		for (g_dataIdx = 0; g_dataIdx < g_dataSize;) // g_dataIdx++ in sigALRM
		{
			++g_counter;
		}
		for (int j = 0; j < g_dataSize; j++) // rotate the bits to accomodate for a possible lack of low-bit entropy
		{
			g_data[j] = (g_data[j]>>3) | (g_data[j]<<5);
		}
	}
	setitimer(ITIMER_REAL, &otv, 0);

	// reset signal handler
	sigaction(SIGALRM, &osa, 0);

}

// void printBuffer(unsigned char* buf, int size)
// {
//     for (int i = 0; i < size; ++i)
//     {
//         if (i % 10 == 0)
//         {
//             printf("\n");
//         }
//         printf("%2.2X ", buf[i]);
//     }
//     printf("\n");
//     fflush(stdout);
// }

void generateRandomDataFromFile(const char* name, int len)
{
	int fd = ::open(name, O_RDONLY);
	if (fd == -1)
	{
		return;
	}

	std::vector<char> buf(len);
	int bytesRead = ::read(fd, &buf[0], len);
	if (bytesRead == -1)
	{
		return;
	}
	buf.resize(bytesRead);
	::RAND_add(&buf[0], buf.size(), 0.0); // 0 entropy, since this could all be observable by someone else.
}

void generateRandomDataFromTime(double entropy)
{
	struct timeval tv;
	::gettimeofday(&tv, 0);
	::RAND_add(&tv, sizeof(tv), entropy);

	clock_t c(::clock());
	::RAND_add(&c, sizeof(c), entropy);

	struct rusage ru;
	::getrusage(RUSAGE_SELF, &ru);
	::RAND_add(&ru, sizeof(ru), entropy);

	::getrusage(RUSAGE_CHILDREN, &ru);
	::RAND_add(&ru, sizeof(ru), entropy);
}

struct cmd
{
	const char* command;
	double usefulness; // estimated number of bytes of entropy per 1K of output
};

// This list of sources comes from gnupg, prngd and egd.
const cmd randomSourceCommands[] =
{
	{ "advfsstat -b usr_domain", 0.01 },
	{ "advfsstat -l 2 usr_domain", 0.5 },
	{ "advfsstat -p usr_domain", 0.01 },
	{ "arp -a -n", 0.5 },
	{ "df", 0.5 },
	{ "df -i", 0.5 },
	{ "df -a", 0.5 },
	{ "df -in", 0.5 },
	{ "dmesg", 0.5 },
	{ "errpt -a", 0.5 },
	{ "ifconfig -a", 0.5 },
	{ "iostat", 0.5 },
	{ "ipcs -a", 0.5 },
	{ "last", 0.5 },
	{ "lastlog", 0.5 },
	{ "lpstat -t", 0.1 },
	{ "ls -alniR /var/log", 1.0 },
	{ "ls -alniR /var/adm", 1.0 },
	{ "ls -alni /var/spool/mail", 1.0 },
	{ "ls -alni /proc", 1.0 },
	{ "ls -alniR /tmp", 1.0 },
	{ "ls -alniR /var/tmp", 1.0 },
	{ "ls -alni /var/mail", 1.0 },
	{ "ls -alniR /var/db", 1.0 },
	{ "ls -alniR /etc", 1.0 },
	{ "ls -alniR /private/var/log", 1.0 },
	{ "ls -alniR /private/var/db", 1.0 },
	{ "ls -alniR /private/etc", 1.0 },
	{ "ls -alniR /private/tmp", 1.0 },
	{ "ls -alniR /private/var/tmp", 1.0 },
	{ "mpstat", 1.5 },
	{ "netstat -s", 1.5 },
	{ "netstat -n", 1.5 },
	{ "netstat -a -n", 1.5 },
	{ "netstat -anv", 1.5 },
	{ "netstat -i -n", 0.5 },
	{ "netstat -r -n", 0.1 },
	{ "netstat -m", 0.5 },
	{ "netstat -ms", 0.5 },
	{ "nfsstat", 0.5 },
	{ "ps laxww", 1.5 },
	{ "ps -laxww", 1.5 },
	{ "ps -al", 1.5 },
	{ "ps -el", 1.5 },
	{ "ps -efl", 1.5 },
	{ "ps -efly", 1.5 },
	{ "ps aux", 1.5 },
	{ "ps -A", 1.5 },
	{ "pfstat", 0.5 },
	{ "portstat", 0.5 },
	{ "pstat -p", 0.5 },
	{ "pstat -S", 0.5 },
	{ "pstat -A", 0.5 },
	{ "pstat -t", 0.5 },
	{ "pstat -v", 0.5 },
	{ "pstat -x", 0.5 },
	{ "pstat -t", 0.5 },
	{ "ripquery -nw 1 127.0.0.1", 0.5 },
	{ "sar -A 1 1", 0.5 },
	{ "snmp_request localhost public get 1.3.6.1.2.1.7.1.0", 0.5 },
	{ "snmp_request localhost public get 1.3.6.1.2.1.7.4.0", 0.5 },
	{ "snmp_request localhost public get 1.3.6.1.2.1.4.3.0", 0.5 },
	{ "snmp_request localhost public get 1.3.6.1.2.1.6.10.0", 0.5 },
	{ "snmp_request localhost public get 1.3.6.1.2.1.6.11.0", 0.5 },
	{ "snmp_request localhost public get 1.3.6.1.2.1.6.13.0", 0.5 },
	{ "snmp_request localhost public get 1.3.6.1.2.1.5.1.0", 0.5 },
	{ "snmp_request localhost public get 1.3.6.1.2.1.5.3.0", 0.5 },
	{ "tail -c 1024 /var/log/messages", 1.0 },
	{ "tail -c 1024 /var/log/syslog", 1.0 },
	{ "tail -c 1024 /var/log/system.log", 1.0 },
	{ "tail -c 1024 /var/log/debug", 1.0 },
	{ "tail -c 1024 /var/adm/messages", 1.0 },
	{ "tail -c 1024 /var/adm/syslog", 1.0 },
	{ "tail -c 1024 /var/adm/syslog/mail.log", 1.0 },
	{ "tail -c 1024 /var/adm/syslog/syslog.log", 1.0 },
	{ "tail -c 1024 /var/log/maillog", 1.0 },
	{ "tail -c 1024 /var/adm/maillog", 1.0 },
	{ "tail -c 1024 /var/adm/SPlogs/SPdaemon.log", 1.0 },
	{ "tail -c 1024 /usr/es/adm/cluster.log", 1.0 },
	{ "tail -c 1024 /usr/adm/cluster.log", 1.0 },
	{ "tail -c 1024 /var/adm/cluster.log", 1.0 },
	{ "tail -c 1024 /var/adm/ras/conslog", 1.0 },
	{ "tcpdump -c 100 -efvvx", 1 },
	{ "uptime", 0.5 },
	{ "vmstat", 2.0 },
	{ "vmstat -c", 2.0 },
	{ "vmstat -s", 2.0 },
	{ "vmstat -i", 2.0 },
	{ "vmstat -f", 2.0 },
	{ "w", 2.5 },
	{ "who -u", 0.5 },
	{ "who -i", 0.5 },
	{ "who -a", 0.5 },

	{ 0, 0 }
};

class RandomOutputGatherer : public Exec::OutputCallback
{
private:
	virtual void doHandleData(const char* data, size_t dataLen, Exec::EOutputSource outputSource, PopenStreams& theStream, size_t streamIndex)
	{
		if (outputSource == Exec::E_STDERR)
		{
			// for all the commands we run, anything output to stderr doesn't have any entropy.
			::RAND_add(data, dataLen, 0.0);
		}
		else
		{
			// streamIndex is the index into the PopenStreams array which correlates to randomSourceCommands
			::RAND_add(data, dataLen, randomSourceCommands[streamIndex].usefulness * static_cast<double>(dataLen) / 1024.0);
		}
		// the actual length of stuff we got could be random, but we can't say for sure, so it gets 0.0 entropy.
		::RAND_add(&dataLen, sizeof(dataLen), 0.0);
		::RAND_add(&outputSource, sizeof(outputSource), 0.0);
		// The timing is random too.
		generateRandomDataFromTime(0.1);
	}

};

String
locateInPath(const String& cmd, const String& path)
{
	StringArray pathElements(path.tokenize(":"));
	for (size_t i = 0; i < pathElements.size(); ++i)
	{
		String testCmd(pathElements[i] + '/' + cmd);
		if (FileSystem::exists(testCmd))
		{
			return testCmd;
		}
	}
	return cmd;
}

class RandomTimerThread : public Thread
{
	virtual Int32 run()
	{
		unsigned char buf[256]; // don't initialize to anything, as we may pick up some good random junk off the stack.
		int iterations = 8;
		generateRandomTimerData(buf, sizeof(buf), &iterations);
		::RAND_add(buf, sizeof(buf), 32); // 32 is if we assume 1 bit per byte, and most systems should have something better than that.

		generateRandomDataFromTime(0.1);
		
		return 0;
	}
};
#endif

void loadRandomness()
{
	// with OpenSSL 0.9.7 calling RAND_status() will try to load sufficient randomness, so hopefully we won't have to do anything.
	if (RAND_status() == 1)
	{
		return;
	}

#ifdef OW_WIN32
	HCRYPTPROV hProvider = 0;
	BYTE buf[64];

	if (CryptAcquireContext(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		if (CryptGenRandom(hProvider, sizeof(buf), buf))
		{
			RAND_add(buf, sizeof(buf), sizeof(buf));
		}
		CryptReleaseContext(hProvider, 0);
	}
	RAND_screen(); // provided by OpenSSL. Try doing something in addition to CryptGenRandom(), since we can't trust closed source.
#else

	// OpenSSL 0.9.7 does this automatically, so only try if we've got an older version of OpenSSL.
	if (SSLeay() < 0x00907000L)
	{
		// now try adding in /dev/random
		int loadedBytes = RAND_load_file("/dev/random", 1024);
		if (loadedBytes == 0)
		{
			// okay, no /dev/random... try adding in /dev/urandom
			RAND_load_file("/dev/urandom", 1024);
		}

		if (RAND_status() == 1)
		{
			return;
		}

		// now try adding in data from an entropy gathering daemon (egd)
		const char *names[] = { "/var/run/egd-pool","/dev/egd-pool","/etc/egd-pool","/etc/entropy", NULL };

		for (int i = 0; names[i]; i++)
		{
			if (RAND_egd(names[i]) != -1)
			{
				break;
			}
		}

		if (RAND_status() == 1)
		{
			return;
		}
	}

	// try loading up randomness from a previous run.
	char randFile[MAXPATHLEN];
	const char* rval = ::RAND_file_name(randFile, MAXPATHLEN);
	if (rval)
	{
		if (randFileIsSecure(randFile))
		{
			::RAND_load_file(randFile, -1);
		}
	}

	// don't check RAND_status() again, since we don't really trust the random file to be very secure--there are too many ways an attacker
	// could get or change it, so we'll do this other stuff as well.

	// we're on a really broken system.  We'll try to get some random data by:
	// - running commands that reflect random system activity.
	//   This is the same approach a egd daemon would do, but we do it only once to seed the randomness.
	//   The list of sources comes from gnupg, prngd and egd.
	// - use a timing based approach which gives decent randomness.
	// - use other variable things, such as pid, execution times, etc.
	//   most of these values have an entropy of 0, since they are observable to any other user on the system, so even though they are random, they're
	//   observable, and we can't count them as entropy.

	// do the time based ones before we start, after the timing tests, and then again after running commands.
	generateRandomDataFromTime(0.0);

	RandomTimerThread randomTimerThread;
	randomTimerThread.start();


	// - read some portions of files and dirs (e.g. /dev/mem) if possible
	const char* files[] = {
		"/dev/mem",
		0
	};
	for (const char** p = files; *p; ++p)
	{
		generateRandomDataFromFile(*p, 1024*1024*2);
	}

	generateRandomDataFromTime(0.1);

	pid_t myPid(::getpid());
	::RAND_add(&myPid, sizeof(myPid), 0.0);

	pid_t parentPid(::getppid());
	::RAND_add(&parentPid, sizeof(parentPid), 0.0);
	
	uid_t myUid(::getuid());
	::RAND_add(&myUid, sizeof(myUid), 0.0);

	gid_t myGid(::getgid());
	::RAND_add(&myGid, sizeof(myGid), 0.0);

	// now run commands
	Array<PopenStreams> streams;
	for (size_t i = 0; randomSourceCommands[i].command != 0; ++i)
	{
		StringArray cmd = StringArray(String(randomSourceCommands[i].command).tokenize());
		if (cmd[0] != '/')
		{
			const char* RANDOM_COMMAND_PATH = "/bin:/sbin:/usr/bin:/usr/sbin:/usr/ucb:/usr/etc:/usr/bsd:/etc:/usr/local/bin:/usr/local/sbin";
			cmd[0] = locateInPath(cmd[0], RANDOM_COMMAND_PATH);
		}
		streams.push_back(Exec::safePopen(cmd));
	}

	RandomOutputGatherer randomOutputGatherer;
	Array<Exec::ProcessStatus> processStatuses;
	const int RANDOM_COMMAND_TIMEOUT = 10;
	try
	{
		gatherOutput(randomOutputGatherer, streams, processStatuses, RANDOM_COMMAND_TIMEOUT);
	}
	catch (ExecTimeoutException&)
	{
		// ignore it.
	}

	// terminate all the processes and add their return code to the pool.
	for (size_t i = 0; i < streams.size(); ++i)
	{
		int rv = streams[i].getExitStatus();
		::RAND_add(&rv, sizeof(rv), 0.0);
	}

	randomTimerThread.join();

	generateRandomDataFromTime(0.1);
#endif
}

} // end unnamed namespace

SSL_CTX* SSLCtxMgr::m_ctxClient = 0;
SSL_CTX* SSLCtxMgr::m_ctxServer = 0;
certVerifyFuncPtr_t SSLCtxMgr::m_clientCertVerifyCB = 0;
certVerifyFuncPtr_t SSLCtxMgr::m_serverCertVerifyCB = 0;
//////////////////////////////////////////////////////////////////////////////
SSL_CTX*
SSLCtxMgr::initCtx(const String& keyfile)
{
	SSL_CTX* ctx = SSL_CTX_new(SSLv23_method());
	SSL_CTX_set_default_passwd_cb(ctx, pem_passwd_cb);
	if (!keyfile.empty())
	{
		if (SSL_CTX_use_certificate_chain_file(ctx, keyfile.c_str()) != 1)
		{
			OW_THROW(SSLException, Format("Couldn't read certificate file: %1",
				keyfile).c_str());
		}
		if (SSL_CTX_use_PrivateKey_file(ctx, keyfile.c_str(), SSL_FILETYPE_PEM) != 1)
		{
			OW_THROW(SSLException, "Couldn't read key file");
		}
	}

	loadRandomness();

	return ctx;
}
//////////////////////////////////////////////////////////////////////////////
namespace
{

class SSLGlobalWork
{
public:
	SSLGlobalWork()
	{
		if (!mutex_buf)
		{
			mutex_buf = new Mutex[CRYPTO_num_locks()];
		}
		SSL_library_init();
		SSL_load_error_strings();

		CRYPTO_set_id_callback(id_function);
		CRYPTO_set_locking_callback(locking_function);

	    // The following three CRYPTO_... functions are the OpenSSL functions
		// for registering the callbacks we implemented above
		CRYPTO_set_dynlock_create_callback(dyn_create_function);
		CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
		CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);
	}

	~SSLGlobalWork()
	{
		if (SSLCtxMgr::isClient() || SSLCtxMgr::isServer())
		{
			char randFile[MAXPATHLEN];
			const char* rval = RAND_file_name(randFile, MAXPATHLEN);
			if (rval)
			{
				// we only create this file is there's no chance an attacker could read or write it. see Network Security with OpenSSL p. 101
				if (randFilePathIsSecure(FileSystem::Path::dirname(randFile)))
				{
					RAND_write_file(randFile);
				}
			}
		}
		SSLCtxMgr::uninit();
		delete[] mutex_buf;
		mutex_buf = 0;
	}
private:
};

SSLGlobalWork g_sslGLobalWork;

} // end unnamed namespace

//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::loadDHParams(SSL_CTX* ctx, const String& file)
{
	DH* ret = 0;
	BIO* bio = BIO_new_file(file.c_str(),"r");
	if (bio == NULL)
	{
		OW_THROW(SSLException, "Couldn't open DH file");
	}
	ret = PEM_read_bio_DHparams(bio, NULL, NULL, NULL);
	BIO_free(bio);
	if (SSL_CTX_set_tmp_dh(ctx, ret) < 0)
	{
		OW_THROW(SSLException, "Couldn't set DH parameters");
	}
}
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::generateEphRSAKey(SSL_CTX* ctx)
{
	RSA* rsa;
	rsa = RSA_generate_key(512, RSA_F4, NULL, NULL);
	if (!SSL_CTX_set_tmp_rsa(ctx, rsa))
	{
		RSA_free(rsa);
		OW_THROW(SSLException,"Couldn't set RSA key");
	}
	RSA_free(rsa);
}
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::initClient(const String& keyfile)
{
	if (m_ctxClient)
	{
		uninitClient();
	}
	//String keyfile = Environment::getConfigItem(
	//		Environment::SSL_CERT_opt);
	m_ctxClient = initCtx(keyfile);
	/*
	if (!SSL_CTX_get_certificate(m_ctxClient))
	{
		std::cerr << "******** SSL_CTX has no cert!" << std::endl;
	}
	else
	{
		std::cerr << "******* SSL_CTX cert hash: " << std::hex << X509_subject_name_hash(SSL_CTX_get_certificate(m_ctxClient)) << std:: endl;
	}
	*/

}
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::initServer(const String& keyfile)
{
	if (m_ctxServer)
	{
		uninitServer();
	}
	//String keyfile = Environment::getConfigItem(
	//		Environment::SSL_CERT_opt);
	m_ctxServer = initCtx(keyfile);
	//String dhfile = "certs/dh1024.pem"; // TODO = GlobalConfig.getSSLDHFile();
	//loadDHParams(m_ctx, dhfile);
	generateEphRSAKey(m_ctxServer);
	String sessID("SSL_SESSION_");
	RandomNumber rn(0, 10000);
	sessID += String(static_cast<UInt32>(rn.getNextNumber()));
	int sessIDLen =
		(SSL_MAX_SSL_SESSION_ID_LENGTH < (sessID.length() * sizeof(char))) ?
		SSL_MAX_SSL_SESSION_ID_LENGTH : (sessID.length() * sizeof(char));
	SSL_CTX_set_session_id_context(m_ctxServer,
			reinterpret_cast<const unsigned char*>(sessID.c_str()), sessIDLen);
	SSL_CTX_set_verify(m_ctxServer,
		SSL_VERIFY_PEER /*| SSL_VERIFY_FAIL_IF_NO_PEER_CERT*/, NULL);
	/*
	// grabbed this from volutionlwc.  may need to use it (Filename issue on Win32)
	if (!SSL_CTX_load_verify_locations(ctx,VOL_CONFIG_DIR"/cacerts/volution-authority.cacert",NULL))
	{
		fprintf(stderr, "Couldn't set load_verify_location\n");
		exit(1);
	}
	SSL_CTX_set_verify_depth(ctx,1);
	*/
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
int
SSLCtxMgr::pem_passwd_cb(char* buf, int size, int /*rwflag*/,
	void* /*userData*/)
{
	String passwd;
	// TODO String = GlobalConfig.getSSLCertificatePassword();
	
	if (passwd.empty())
	{
		passwd = GetPass::getPass(
			"Enter the password for the SSL certificate: ");
	}
	strncpy(buf, passwd.c_str(), passwd.length());
	buf[size - 1] = '\0';
	return(passwd.length());
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
bool
SSLCtxMgr::checkClientCert(SSL* ssl, const String& hostName)
{
	return checkCert(ssl, hostName, m_clientCertVerifyCB);
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
bool
SSLCtxMgr::checkServerCert(SSL* ssl, const String& hostName)
{
	return checkCert(ssl, hostName, m_serverCertVerifyCB);
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
bool
SSLCtxMgr::checkCert(SSL* ssl, const String& hostName,
	certVerifyFuncPtr_t certVerifyCB)
{
	/* TODO this isn't working.
	if (SSL_get_verify_result(ssl)!=X509_V_OK)
	{
		cout << "SSL_get_verify_results failed." << endl;
		return false;
	}
	*/
	/*Check the cert chain. The chain length
	  is automatically checked by OpenSSL when we
	  set the verify depth in the ctx */
	/*Check the common name*/
	X509 *peer = SSL_get_peer_certificate(ssl);
	if (certVerifyCB)
	{
		if (certVerifyCB(peer, hostName) == 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
int
SSLCtxMgr::sslRead(SSL* ssl, char* buf, int len)
{
	int cc = SSL_ERROR_WANT_READ;
	int r, retries = 0;
	while(cc == SSL_ERROR_WANT_READ && retries < OW_SSL_RETRY_LIMIT)
	{
		r = SSL_read(ssl, buf, len);
		cc = SSL_get_error(ssl, r);
		retries++;
	}
	
	switch (cc)
	{
		case SSL_ERROR_NONE:
			return r;
		case SSL_ERROR_ZERO_RETURN:
			// TODO should this be an exception???
			return -1;
		default:
			//OW_THROW(SSLException, "SSL read problem");
			return -1;
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
int
SSLCtxMgr::sslWrite(SSL* ssl, const char* buf, int len)
{
	int r, cc, retries;
	int myLen = len;
	int offset = 0;
	while (myLen > 0)
	{
		cc = SSL_ERROR_WANT_WRITE;
		retries = 0;
		while(cc == SSL_ERROR_WANT_WRITE && retries < OW_SSL_RETRY_LIMIT)
		{
			r = SSL_write(ssl, buf + offset, myLen);
			cc = SSL_get_error(ssl, r);
			retries++;
		}

		if (cc == SSL_ERROR_NONE)
		{
			myLen -= r;
			offset += r;
		}
		else
		{
			return -1;
		}
	}
	return len;
}
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::uninit()
{
	uninitClient();
	uninitServer();
		
	// free up memory allocated in SSL_library_init()
	EVP_cleanup();
	// free up memory allocated in SSL_load_error_strings()
	ERR_free_strings();
}
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::uninitClient()
{
	if (m_ctxClient)
	{
		SSL_CTX_free(m_ctxClient);
		m_ctxClient = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::uninitServer()
{
	if (m_ctxServer)
	{
		SSL_CTX_free(m_ctxServer);
		m_ctxServer = NULL;
	}
}

namespace
{

//////////////////////////////////////////////////////////////////////////////
extern "C"
{
static int verify_callback(int ok, X509_STORE_CTX *store)
{
    char data[256];
    SSL* ssl =
        (SSL*) X509_STORE_CTX_get_ex_data(store,
                                          SSL_get_ex_data_X509_STORE_CTX_idx());
    OWSSLContext* owctx =
        (OWSSLContext*) SSL_get_ex_data(ssl, SSLServerCtx::SSL_DATA_INDEX);
    OW_ASSERT(owctx);

	/**
	 * This callback is called multiple times while processing a cert.
	 * Supposedly, it is called at each depth of the cert chain.
	 * However, even with a self-signed cert, it is called twice, both times
	 * at depth 0.  Since we will change the status from NOT_OK to OK, the
	 * next time through the preverify status will also be changed.  Therefore
	 * we need to make sure that we set the status on owctx to failed with
	 * any failure, and then that we don't inadvertantly change it to PASS
	 * on subsequent calls.
	 */
	if (!ok)
	{
		owctx->peerCertPassedVerify = OWSSLContext::VERIFY_FAIL;
	}
	else
	{
		// if the cert failed on a previous call, we don't want to change
		// the status.
		if (owctx->peerCertPassedVerify != OWSSLContext::VERIFY_FAIL)
		{
			owctx->peerCertPassedVerify = OWSSLContext::VERIFY_PASS;
		}
	}

#ifdef OW_DEBUG
    if (!ok)
    {
        X509 *cert = X509_STORE_CTX_get_current_cert(store);
        int  depth = X509_STORE_CTX_get_error_depth(store);
        int  err = X509_STORE_CTX_get_error(store);

        fprintf(stderr, "-Error with certificate at depth: %i\n", depth);
        X509_NAME_oneline(X509_get_issuer_name(cert), data, 256);
        fprintf(stderr, "  issuer   = %s\n", data);
        X509_NAME_oneline(X509_get_subject_name(cert), data, 256);
        fprintf(stderr, "  subject  = %s\n", data);
        fprintf(stderr, "  err %i:%s\n", err, X509_verify_cert_error_string(err));
    }
#endif

    return 1;
}
} // end extern "C"

} // end unnamed namespace

//////////////////////////////////////////////////////////////////////////////
SSLCtxBase::SSLCtxBase(const SSLOpts& opts)
	: m_ctx(0)
{
	m_ctx = SSL_CTX_new(SSLv23_method());
	SSL_CTX_set_default_passwd_cb(m_ctx, SSLCtxMgr::pem_passwd_cb); // TODO cb func moved elsewhere?
	if (!opts.keyfile.empty())
	{
		if (!(SSL_CTX_use_certificate_chain_file(m_ctx, opts.keyfile.c_str())))
		{
			OW_THROW(SSLException, Format("Couldn't read certificate file: %1",
				opts.keyfile).c_str());
		}
		if (!(SSL_CTX_use_PrivateKey_file(m_ctx, opts.keyfile.c_str(), SSL_FILETYPE_PEM)))
		{
			OW_THROW(SSLException, "Couldn't read key file");
		}
	}
	/*String CAFile = Environment::getConfigItem("SSL_CAFile");
	String CAPath;
	char* CAFileStr = NULL;
	char* CAPathStr = NULL;
	if (CAFile.length() > 0)
		CAFileStr = strdup(CAFile.c_str());
	if (CAPath.length() > 0)
		CAPathStr = const_cast<char*>(CAPath.c_str());
	if (!(SSL_CTX_load_verify_locations(ctx, CAFileStr, CAPathStr)))
		OW_THROW(SSLException, "Couldn't read CA list");
	free(CAFileStr);*/ // TODO do we need this?
	//String dhfile = "certs/dh1024.pem"; // TODO = GlobalConfig.getSSLDHFile();
	//loadDHParams(m_ctx, dhfile);

	loadRandomness();

	SSLCtxMgr::generateEphRSAKey(m_ctx); // TODO what the heck is this?
	String sessID("SSL_SESSION_");
	RandomNumber rn(0, 10000);
	sessID += String(static_cast<UInt32>(rn.getNextNumber()));
	int sessIDLen =
		(SSL_MAX_SSL_SESSION_ID_LENGTH < (sessID.length() * sizeof(char))) ?
		SSL_MAX_SSL_SESSION_ID_LENGTH : (sessID.length() * sizeof(char));
	SSL_CTX_set_session_id_context(m_ctx,
			reinterpret_cast<const unsigned char*>(sessID.c_str()), sessIDLen);


	if (opts.verifyMode != SSLOpts::MODE_DISABLED && !opts.trustStore.empty())
	{
		if (!OW_NAMESPACE::FileSystem::exists(opts.trustStore) )
		{
			OW_THROW(SSLException, Format("Error loading truststore %1",
										  opts.trustStore).c_str());
		}
		if (SSL_CTX_load_verify_locations(m_ctx,0,opts.trustStore.c_str()) != 1)
		{
			OW_THROW(SSLException, Format("Error loading truststore %1",
										  opts.trustStore).c_str());
		}
	}
	/* TODO remove.
	if (SSL_CTX_set_default_verify_paths(m_ctx) != 1)
	{
		OW_THROW(SSLException, "Error loading default CA store(s)");
	}
	*/
	switch (opts.verifyMode)
	{
	case SSLOpts::MODE_DISABLED:
		SSL_CTX_set_verify(m_ctx,
			SSL_VERIFY_NONE, 0);
		break;
	case SSLOpts::MODE_REQUIRED:
		SSL_CTX_set_verify(m_ctx,
			SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);
		break;
	case SSLOpts::MODE_OPTIONAL:
	case SSLOpts::MODE_AUTOUPDATE:
		SSL_CTX_set_verify(m_ctx,
			SSL_VERIFY_PEER, verify_callback);
		break;
	default:
		OW_ASSERT("Bad option, shouldn't happen" == 0);
		break;
	}

	SSL_CTX_set_verify_depth(m_ctx, 4);

}

//////////////////////////////////////////////////////////////////////////////
SSLCtxBase::~SSLCtxBase()
{
}

//////////////////////////////////////////////////////////////////////////////
SSL_CTX*
SSLCtxBase::getSSLCtx() const
{
	return m_ctx;
}

SSLOpts::SSLOpts()
	: verifyMode(MODE_DISABLED)
{
}





//////////////////////////////////////////////////////////////////////////////
SSLServerCtx::SSLServerCtx(const SSLOpts& opts)
	: SSLCtxBase(opts)
{
}
//////////////////////////////////////////////////////////////////////////////
SSLClientCtx::SSLClientCtx(const SSLOpts& opts)
	: SSLCtxBase(opts)
{
}

static Mutex m_mapGuard;

//////////////////////////////////////////////////////////////////////////////
SSLTrustStore::SSLTrustStore(const String& storeLocation)
	: m_store(storeLocation)
{
	m_mapfile = m_store + "/map";
	if (FileSystem::exists(m_mapfile))
	{
		MutexLock mlock(m_mapGuard);
		readMap();
	}
}

//////////////////////////////////////////////////////////////////////////////
bool
SSLTrustStore::getUser(const String& certhash, String& user, String& uid)
{
	MutexLock mlock(m_mapGuard);
	Map<String, UserInfo>::const_iterator iter = m_map.find(certhash);
	if (iter == m_map.end())
	{
		return false;
	}
	user = iter->second.user;
	uid = iter->second.uid;
	return true;
}

//////////////////////////////////////////////////////////////////////////////
void
SSLTrustStore::addCertificate(X509* cert, const String& user, const String& uid)
{
	static const int numtries = 1000;
	OW_ASSERT(cert);
	OStringStream ss;
	unsigned long hash = X509_subject_name_hash(cert);
	if (hash == 0)
	{
		OW_THROW(SSLException, "Unable to extract hash from certificate");
	}
	ss << std::hex << hash << std::ends;
	String filename = m_store + "/" + ss.toString() + ".";
	int i = 0;
	for (i = 0; i < numtries; ++i)
	{
		String temp = filename + String(i);
		if (FileSystem::exists(temp))
		{
			continue;
		}
		filename = temp;
		break;
	}
	if (i == numtries)
	{
		OW_THROW(SSLException, "Unable to find a valid filename to store cert");
	}
	FILE* fp = fopen(filename.c_str(), "w");
	if (!fp)
	{
		OW_THROW(SSLException, Format("Unable to open new cert file for writing: %1", filename).c_str());
	}
	// Undocumented function in OpenSSL.  We assume it returns 1 on success
	// like most OpenSSL funcs.
	if (PEM_write_X509(fp, cert) != 1)
	{
		OW_THROW(SSLException, Format("SSL error while writing certificate to: %1", filename).c_str());
	}
	fclose(fp);

	String digest = getCertMD5Fingerprint(cert);
	MutexLock mlock(m_mapGuard);
	UserInfo info;
	info.user = user;
	info.uid = uid;
	m_map[digest] = info;
	writeMap();
}

//////////////////////////////////////////////////////////////////////////////
String
SSLTrustStore::getCertMD5Fingerprint(X509* cert)
{
	unsigned char digest[16];
	unsigned int len = 16;
	X509_digest(cert, EVP_md5(), digest, &len);
	return MD5::convertBinToHex(digest);
}

//////////////////////////////////////////////////////////////////////////////
void
SSLTrustStore::writeMap()
{
	std::ofstream f(m_mapfile.c_str(), std::ios::out);
	if (!f)
	{
		OW_THROW(SSLException, Format("SSL error opening map file: %1",
									  m_mapfile).c_str());
	}
	for (Map<String, UserInfo>::const_iterator iter = m_map.begin();
		  iter != m_map.end(); ++iter)
	{
		f << iter->first << " " << iter->second.user
			<< " " << iter->second.uid << "\n";
	}
	f.close();
}

//////////////////////////////////////////////////////////////////////////////
void
SSLTrustStore::readMap()
{
	std::ifstream f(m_mapfile.c_str(), std::ios::in);
	if (!f)
	{
		OW_THROW(SSLException, Format("SSL error opening map file: %1",
									  m_mapfile).c_str());
	}
	int lineno = 0;
	while (f)
	{
		String line = String::getLine(f);
		if (!f)
		{
			break;
		}
		++lineno;
		StringArray toks = line.tokenize();
		if (toks.size() != 3 && toks.size() != 2)
		{
			OW_THROW(SSLException, Format("Error processing user map %1 at line %2",
										  m_mapfile, lineno).c_str());
		}
		UserInfo info;
		info.user = toks[1];
		if (toks.size() == 3)
		{
			info.uid = toks[2];
		}
		m_map.insert(std::make_pair(toks[0], info));
	}
#ifdef OW_DEBUG
	std::cerr << "cert<>user map initizialized with " << m_map.size() << " users" << std::endl;
#endif
	f.close();
}

//////////////////////////////////////////////////////////////////////////////

OWSSLContext::OWSSLContext()
    : peerCertPassedVerify(VERIFY_NONE)
{
}
//////////////////////////////////////////////////////////////////////////////
OWSSLContext::~OWSSLContext()
{
}


} // end namespace OW_NAMESPACE

#endif // #ifdef OW_HAVE_OPENSSL


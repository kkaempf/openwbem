/*******************************************************************************
* Copyright (C) 2001-2005 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_CryptographicRandomNumber.hpp"
#include "OW_ExceptionIds.hpp"

namespace OW_NAMESPACE
{
OW_DEFINE_EXCEPTION_WITH_ID(CryptographicRandomNumber);
}


#if defined(OW_HAVE_OPENSSL)
#include "OW_Assertion.hpp"
#include "OW_Exec.hpp"
#include "OW_Thread.hpp"
#include "OW_FileSystem.hpp"
#include "OW_ThreadOnce.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_SSLCtxMgr.hpp"
#include "OW_UnnamedPipe.hpp"

#include <vector>
#include <climits>
#include <csignal>

#include <openssl/rand.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

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

namespace OW_NAMESPACE
{

namespace
{

/////////////////////////////////////////////////////////////////////////////
int getNumBits(Int32 num)
{
	for (size_t i = 0; i < sizeof(num) * CHAR_BIT; ++i)
	{
		if (num < (1 << i))
		{
			return i;
		}
	}
	return sizeof(num) * CHAR_BIT;
}

/////////////////////////////////////////////////////////////////////////////
OnceFlag guard;
unsigned int seed = 0;

} // end unnamed namespace

/////////////////////////////////////////////////////////////////////////////
CryptographicRandomNumber::CryptographicRandomNumber(Int32 lowVal, Int32 highVal)
	: m_lowVal(lowVal)
	, m_highVal(highVal)
	, m_range(m_highVal - m_lowVal)
	, m_numBits(getNumBits(m_range))
{
	OW_ASSERT(lowVal < highVal);
	callOnce(guard, &initRandomness);
}

/////////////////////////////////////////////////////////////////////////////
Int32
CryptographicRandomNumber::getNextNumber()
{
	Int32 randNum = 0;
	do
	{
		ERR_clear_error();
		int rv = RAND_bytes(reinterpret_cast<unsigned char*>(&randNum), sizeof(randNum));
		if (rv != 1)
		{
			OW_THROW(CryptographicRandomNumberException, SSLCtxMgr::getOpenSSLErrorDescription().c_str());
		}
		// make it positive
		randNum = randNum < 0 ? -randNum : randNum;
		// filter out all the unnecessary high bits
		randNum &= ~(~0 << m_numBits);
	} while (randNum > m_range);

	return randNum + m_lowVal;
}

namespace
{

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

/////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////
class RandomOutputGatherer : public Exec::OutputCallback
{
private:
	virtual void doHandleData(const char* data, size_t dataLen, Exec::EOutputSource outputSource, PopenStreams& theStream, size_t streamIndex, Array<char>& inputBuffer)
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

/////////////////////////////////////////////////////////////////////////////
class RandomInputCallback : public Exec::InputCallback
{
private:
	virtual void doGetData(Array<char>& inputBuffer, PopenStreams& theStream, size_t streamIndex)
	{
		// none of the processes we run need data from stdin, so just close it.
		if (theStream.in()->isOpen())
		{
			theStream.in()->close();
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////
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
} // end unnamed namespace

/////////////////////////////////////////////////////////////////////////////
void
CryptographicRandomNumber::initRandomness()
{
#ifdef OW_WIN32
	// There are issues on win32 with calling RAND_status() w/out sufficient entropy 
	// in a threaded environment, so we'll just add some before calling RAND_status()
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
	::RAND_screen(); // provided by OpenSSL. Try doing something in addition to CryptGenRandom(), since we can't trust closed source.
#endif

	// with OpenSSL 0.9.7 calling RAND_status() will try to load sufficient randomness, so hopefully we won't have to do anything.
	if (::RAND_status() == 1)
	{
		return;
	}

#ifndef OW_WIN32
	// OpenSSL 0.9.7 does this automatically, so only try if we've got an older version of OpenSSL.
	if (::SSLeay() < 0x00907000L)
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
		if (cmd[0] != "/")
		{
			const char* RANDOM_COMMAND_PATH = "/bin:/sbin:/usr/bin:/usr/sbin:/usr/ucb:/usr/etc:/usr/bsd:/etc:/usr/local/bin:/usr/local/sbin";
			cmd[0] = locateInPath(cmd[0], RANDOM_COMMAND_PATH);
		}
		streams.push_back(Exec::safePopen(cmd));
	}

	RandomOutputGatherer randomOutputGatherer;
	RandomInputCallback randomInputCallback;
	Array<Exec::ProcessStatus> processStatuses;
	const int RANDOM_COMMAND_TIMEOUT = 10;
	try
	{
		Exec::processInputOutput(randomOutputGatherer, streams, processStatuses, randomInputCallback, RANDOM_COMMAND_TIMEOUT);
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

/////////////////////////////////////////////////////////////////////////////
void
CryptographicRandomNumber::saveRandomState()
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

} // end namespace OW_NAMESPACE

#endif // #if defined(OW_HAVE_OPENSSL)


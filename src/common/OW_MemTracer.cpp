/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NO T LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#define __OW_MEMTRACER_CPP__
#include "OW_config.h"

#ifdef OW_DEBUG_MEMORY

#include "OW_MemTracer.hpp"
#include "OW_Mutex.hpp"
#include <map>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <unistd.h>


#define OW_MEM_SIG 0xaaaaaaaa
#define OW_FREE_MEM_SIG 0xbbbbbbbb

static const char* const noFile = "<no file>";


class OW_MemTracer
{
public:

	class Entry
	{
	public:
		Entry (char const * file, int line, size_t sz)
		: m_file(file), m_line(line), m_size(sz), m_isDeleted(false) {}

		Entry()
		: m_file(NULL), m_line(-1), m_size(0), m_isDeleted(false) {}

		char const* getFile() const { return m_file; }
		int getLine() const { return m_line; }
		size_t getSize() const { return m_size; }
		void setDeleted() { m_isDeleted = true; }
		bool isDeleted() { return m_isDeleted; }

	private:
		char const* m_file;
		int m_line;
		size_t m_size;
		bool m_isDeleted;
	};
private:

	class Lock
	{
	public:
		Lock(OW_MemTracer & tracer) : m_tracer(tracer) { m_tracer.lock (); }
		~Lock()
		{
			try
			{
				m_tracer.unlock ();
			}
			catch (...)
			{
				// don't let exceptions escape
			}
		}

	private:
		OW_MemTracer& m_tracer;
	};

	typedef std::map<void*, Entry>::iterator iterator;
	friend class Lock;

public:

	OW_MemTracer();
	~OW_MemTracer();
	void add(void* p, char const* file, int line, size_t sz);
	void* remove(void * p);
	void dump();
	Entry getEntry(void* idx);
	void printEntry(void* p);
	void checkMap();

private:
	void lock() { m_lockCount++; }
	void unlock() { m_lockCount--; }

private:

	std::map<void*, Entry> m_map;
	int m_lockCount;
};

static OW_Mutex* memguard = NULL;
static OW_MemTracer* MemoryTracer = 0;
static bool _shuttingDown = false;


static bool noFree = false;
static bool aggressive = false;
static bool disabled = false;


//////////////////////////////////////////////////////////////////////////////
void
myAtExitFunction()
{
	_shuttingDown = true;
	if(MemoryTracer != 0)
	{
		fprintf(stderr, "*******************************************************************************\n");
		fprintf(stderr, "*   D U M P I N G   M E M O R Y\n");
		fprintf(stderr, "*******************************************************************************\n");
		MemoryTracer->dump();
		fprintf(stderr, "-------------------------------------------------------------------------------\n");
		fprintf(stderr, "-   D O N E   D U M P I N G   M E M O R Y\n");
		fprintf(stderr, "-------------------------------------------------------------------------------\n");
	}
	else
	{
		fprintf(stderr, "OpenWBEM: MemoryTracer object does not exist\n");
	}
}

static bool owInternal = false;

static bool initialized = false;

void
processEnv()
{
	if (!initialized)
	{
		if (getenv("OW_MEM_DISABLE") && getenv("OW_MEM_DISABLE")[0] == '1')
		{
			disabled = true;
		}
		if (getenv("OW_MEM_NOFREE") && getenv("OW_MEM_NOFREE")[0] == '1')
		{
			noFree = true;
		}
		if (getenv("OW_MEM_AGGRESSIVE") && getenv("OW_MEM_AGGRESSIVE")[0] == '1')
		{
			aggressive = true;
			fprintf(stderr, "MemTracer running in aggressive mode.\n");
		}
		initialized = true;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
allocMemTracer()
{
	owInternal = true;
	processEnv();
	if (!disabled)
	{
		if (memguard == 0)
		{
			memguard = new OW_Mutex;
			memguard->acquire();
		}
		if(MemoryTracer == 0)
		{
			atexit(myAtExitFunction);
			MemoryTracer = new OW_MemTracer;
		}
	}
	owInternal = false;
}

//////////////////////////////////////////////////////////////////////////////
void OW_DumpMemory()
{
	if(MemoryTracer != 0)
	{
		MemoryTracer->dump();
	}
	else
	{
		fprintf(stderr, "OpenWBEM: MemoryTracer object does not exist\n");
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_MemTracer::OW_MemTracer() : m_lockCount (0)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_MemTracer::~OW_MemTracer()
{
	try
	{
		dump();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}

//static int delCount = 0;
//////////////////////////////////////////////////////////////////////////////
static void*
checkSigs(void* p, size_t sz)
{
	assert(sz);
	assert(p);
	unsigned long* plong = (unsigned long*)((char*)p - 4);
	if(*plong != OW_MEM_SIG)
	{
		fprintf(stderr, "UNDERRUN: Beginning boundary problem.  "
			"Sig is %x\n", (unsigned int)*plong);
		MemoryTracer->printEntry(p);
		assert(0);
	}

	plong = (unsigned long*)((char*)p + sz);

	if(*plong != OW_MEM_SIG)
	{
		fprintf(stderr, "OVERRUN: Ending boundary problem.  "
			"Sig is %x\n", (unsigned int)*plong);
		MemoryTracer->printEntry(p);
		fflush(stderr);
		assert(0);
	}

	return (void*)((char*)p - 4);
}

//////////////////////////////////////////////////////////////////////////////
static void*
checkAndSwitchSigs(void* p, size_t sz)
{
	assert(sz);
	assert(p);
	unsigned long* plong = (unsigned long*)((char*)p - 4);
	if(*plong != OW_MEM_SIG)
	{
		fprintf(stderr, "UNDERRUN: Beginning boundary problem.  "
			"Sig is %x\n", (unsigned int)*plong);
		assert(0);
	}
	*plong = OW_FREE_MEM_SIG;


	plong = (unsigned long*)((char*)p + sz);

	if(*plong != OW_MEM_SIG)
	{
		fprintf(stderr, "OVERRUN: Ending boundary problem.  "
			"Sig is %x\n", (unsigned int)*plong);
		assert(0);
	}
	*plong = OW_FREE_MEM_SIG;

	return (void*)((char*)p - 4);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MemTracer::checkMap()
{
	for (iterator it = m_map.begin(); it != m_map.end(); ++it)
	{
		if (!it->second.isDeleted())
		{
			checkSigs(it->first, it->second.getSize());
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
void
OW_MemTracer::add(void* p, char const* file, int line, size_t sz)
{
	const char* pfile = noFile;
	if (file)
	{
		pfile = strdup(file);
	}
	m_map[p] = Entry(pfile, line, sz);
}

//////////////////////////////////////////////////////////////////////////////
void*
OW_MemTracer::remove(void* p)
{
	iterator it = m_map.find(p);
	if(it != m_map.end())
	{
		if (noFree)
		{
			if (it->second.isDeleted())
			{
				fprintf(stderr, "DOUBLE DELETE (NOFREE): Attempting to double "
					"delete memory at: %p\n", p);
				assert(0);
			}
		}

		void* ptrToFree = checkAndSwitchSigs(p, it->second.getSize());
		void* pfile = (void*) it->second.getFile();

		if (noFree)
		{
			it->second.setDeleted();
		}
		else
		{
			m_map.erase(it);
			if (pfile != noFile)
			{
				free(pfile);
			}
		}

		return ptrToFree;
	}

	fprintf(stderr, "Attempting to delete memory not in map: %p\n", p);

	if (!noFree)
	{
		fprintf(stderr, "Trying to check beginning signature...\n");
		unsigned long* plong = (unsigned long*)((char*)p - 4);
		if(*plong == OW_MEM_SIG)
		{
			fprintf(stderr, "MemTracer is broken\n");
			assert(0);
		}
		if(*plong == OW_FREE_MEM_SIG)
		{
			fprintf(stderr, "DOUBLE DELETE: This memory was previously freed by MemTracer, "
				"probably double delete\n");
			assert(0);
		}
		fprintf(stderr, "No signature detected.\n");
	}

	fprintf(stderr, "UNKNOWN ADDRESS\n");
	assert(0);
	return p;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MemTracer::printEntry(void* p)
{
	Entry entry = getEntry(p);

	fprintf(stderr, "\tFILE: %s", entry.getFile());
	fprintf(stderr, "\tLINE: %d", entry.getLine());
	fprintf(stderr, "\tSIZE: %d", entry.getSize());
	fprintf(stderr, "\tADDR: %x\n", (unsigned int)p);
}

//////////////////////////////////////////////////////////////////////////////
OW_MemTracer::Entry
OW_MemTracer::getEntry(void* idx)
{
	memguard->acquire();
	iterator it = m_map.find(idx);
	OW_MemTracer::Entry rval;
	if(it != m_map.end())
	{
		rval = it->second;
	}
	memguard->release();
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MemTracer::dump()
{
	memguard->acquire();
	if(m_map.size() != 0)
	{
		fprintf(stderr, "**** %d MEMORY LEAK(S) DETECTED\n", m_map.size());

		size_t total = 0;
		for(iterator it = m_map.begin(); it != m_map.end (); ++it)
		{
			if (!it->second.isDeleted())
			{
				fprintf(stderr, "\tFILE: %s", it->second.getFile());
				fprintf(stderr, "\tLINE: %d", it->second.getLine());
				fprintf(stderr, "\tSIZE: %d", it->second.getSize());
				fprintf(stderr, "\tADDR: %x\n", (unsigned int)it->first);

				total += it->second.getSize();
			}
		}

		fprintf(stderr, "***** END MEMORY LEAKS - TOTAL MEMORY LEAKED = %d\n", total);
	}
	memguard->release();
}


//////////////////////////////////////////////////////////////////////////////
static void
writeSigs(void *& p, size_t size)
{
	unsigned long* plong = (unsigned long*)p;
	*plong = OW_MEM_SIG;

	plong = (unsigned long*)((char*)p + size + 4);
	*plong = OW_MEM_SIG;

	p = (void*)((char*)p + 4);
}


static int internalNewCount = 0;
//////////////////////////////////////////////////////////////////////////////
static void*
doNew(size_t size, char const* file, int line)
{
	if (memguard)
	{
		memguard->acquire();
	}
	if (owInternal || disabled)
	{
		++internalNewCount;
		if (internalNewCount > 2 && !disabled)
		{
			fprintf(stderr, "INTERNAL NEW called more than twice!  "
				"Possible bug in OW_MemTracer.\n");
			assert(0);
		}
		void* rval =  malloc(size);

		if (memguard)
		{
			memguard->release();
		}
		return rval;
	}

	allocMemTracer();

	if (disabled)
	{
		return malloc(size);
	}

	if (aggressive)
	{
		MemoryTracer->checkMap();
	}

	void* p = malloc(size + 8);

   if (!p)
	{
		memguard->release();

		perror("malloc failed.");
		exit(errno);
	}

	writeSigs(p, size);

	owInternal = true;

	assert (MemoryTracer);
	MemoryTracer->add(p, file, line, size);

	owInternal = false;

	memguard->release();
	return p;
}

//////////////////////////////////////////////////////////////////////////////
void*
operator new[](size_t size, char const* file, int line)
{
	return doNew(size, file, line);
}

//////////////////////////////////////////////////////////////////////////////
void*
operator new(size_t size, char const* file, int line)
{
	return doNew(size, file, line);
}


//////////////////////////////////////////////////////////////////////////////
void*
operator new[](size_t size)
{
	return doNew(size, NULL, 0);
}

//////////////////////////////////////////////////////////////////////////////
void*
operator new(size_t size)
{
	return doNew(size, NULL, 0);
}


//////////////////////////////////////////////////////////////////////////////
static void
doDelete(void* p)
{
	if(p)
	{
		if (memguard)
		{
			memguard->acquire();
		}
		if (owInternal || disabled)
		{
			if (!disabled)
			{
				fprintf(stderr, "INTERNAL DELETE: %p\n", p);
			}
			free(p);
			if (memguard)
			{
				memguard->release();
			}
			return;
		}

		if (aggressive)
		{
			MemoryTracer->checkMap();
		}


		owInternal = true;
		if(MemoryTracer != 0)
		{
			p = MemoryTracer->remove((void*)((char*)p));
		}
		else
		{
			printf("** OW_MemTracer can't remove delete from map: ADDR: %x\n", (unsigned int)p);
		}

		if (!noFree)
		{
			free(p);
		}

		owInternal = false;
		memguard->release();
	}

	if(_shuttingDown)
	{
		memguard->release();
		fprintf(stderr, "delete called\n");
	}

}

//////////////////////////////////////////////////////////////////////////////
void
operator delete(void* p)
{
	doDelete(p);
}

void
operator delete[](void* p)
{
	doDelete(p);
}

#endif	// OW_DEBUG_MEMORY


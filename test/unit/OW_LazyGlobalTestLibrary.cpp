#include "OW_config.h"
#include "OW_Logger.hpp"
#include "OW_ScopeLogger.hpp"
#include "OW_LazyGlobal.hpp"
#include <utility>

using namespace OW_NAMESPACE;

namespace
{
	struct textpair
	{
		const char* const first;
		const char* const second;
	};
	struct TestScopeLoggerFactory
	{
		static ScopeLogger* create(const textpair& messages)
		{
			return new ScopeLogger(messages.first, messages.second, "test");
		}
	};
	typedef LazyGlobal<ScopeLogger, textpair, TestScopeLoggerFactory> GlobalScopeLogger;


	// foo here depends on this library being loaded after the loggers are completely functional.
	ScopeLogger foo("global constructor", "global destructor", "test");

	GlobalScopeLogger globalVariable = OW_LAZY_GLOBAL_INIT({"initialize", "destroy"});
}

extern "C"
{
	void forceInitialize()
	{
		Logger lgr("test");
		lgr.logDebug("preinit");
		globalVariable.get();
		lgr.logDebug("postinit");
	}
}

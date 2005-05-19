
#include "OW_config.h"
#include "OW_Array.hpp"
#include "OW_CerrLogger.hpp"
#include "OW_ConfigException.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Exec.hpp"
#include "OW_FileSystem.hpp"
#include "OW_Format.hpp"
#include "OW_Logger.hpp"
#include "OW_String.hpp"
#include "OW_TestResultAccumulator.hpp"

#include <iostream>
#include <ostream>
#include <typeinfo>

//FIXME: Make sure the newly added config file option is used sanely.

//FIXME: Several shell commands are used in this program (diff,
//    find. others?) Make sure their paths are found and calculated in
//    a reasonable way.

using namespace OW_NAMESPACE;

namespace test
{
	int const timeoutSeconds = 300;
	int const outputLimit = -1;
	int const badProcessStatus = -1;
	String const owmofinstalltool("owmofinstalltool");

	//FIXME: make this a proper singleton.
	struct Config
	{
		public:
			static void init(String const& stageDir, String const& configFile);
			static Config& config();
			String stageDir()const;
			String owmofinstalltool_CommandName()const;
			String mofDir()const;
			String testMofDir()const;
			String configFile()const;
		private:
			Config(String const& stageDir, String const& configFile);
			String stageDirValue;
			String owmofinstalltool_CommandNameValue;
			String testMofDirValue;
			String configFileValue;
			ConfigFile::ConfigMap configMapValue;
			String mofDirValue;
			static Config* configPtr;
	};

	Config* Config::configPtr = 0;

	void Config::init(String const& stageDir, String const& configFile)
	{
		static Config config(stageDir, configFile);
		Config::configPtr = &config;
	}

	Config& Config::config()
	{
		if (Config::configPtr)
		{
			return *Config::configPtr;
		}
		//FIXME: This should throw some half-way decent exception type.
		else { OW_THROW(ConfigException, "Config file not loaded yet."); }
	}

	Config::Config(String const& stageDir, String const& configFile)
		:stageDirValue(stageDir)
		 ,testMofDirValue(stageDir + "/../../testmof")
		 ,configFileValue(configFile)
	{
		try
		{
			loadConfigFile(configFileValue, configMapValue);
			mofDirValue = getConfigItem(configMapValue, ConfigOpts::MOF_DIR_opt);
			owmofinstalltool_CommandNameValue = getConfigItem(configMapValue, ConfigOpts::BIN_DIR_opt) + "/" + owmofinstalltool;
			OW_LOG_DEBUG(getLogger(), Format("Set stageDir to %1 .", stageDirValue));
			OW_LOG_DEBUG(getLogger(), Format("Set mofDir to %1 .", mofDirValue));
			OW_LOG_DEBUG(getLogger(), Format("Set owmofinstalltool_CommandNameValue to %1 .", owmofinstalltool_CommandNameValue));
		}
		catch(ConfigException const& c)
		{
			OW_LOG_DEBUG(getLogger(), Format("Caught ConfigException %1 . Rethrowing.", c.what()));
			throw;
		}
	}

	String Config::stageDir()const{return stageDirValue;}
	String Config::mofDir()const{return mofDirValue;}
	String Config::testMofDir()const{return testMofDirValue;}
	String Config::configFile()const{return configFileValue;}

	String Config::owmofinstalltool_CommandName()const
	{
		return owmofinstalltool_CommandNameValue;
	}

	/**
	 * Turn a thrown exceptioon from Exec::executeProcessAndGatherOutput
	 * into a debug message and a boolean flag.
	 */
	void wrapExceptionThrowingExec(
				StringArray const& command,
				String& output, int& processstatus,
				bool& threwException,
				int timeoutsecs = timeoutSeconds,
				int outputlimit = outputLimit
		)
	{
		try
		{
			Exec::executeProcessAndGatherOutput(command, output, processstatus, timeoutsecs, outputlimit);
			threwException = false;
		}
		catch(Exception const& e)
		{
			OW_LOG_DEBUG(getLogger(), Format("Caught exception of type %1 with message %2", e.type(), e.what()));
			threwException = true;
		}
		//FIXME: Remove before checkin.
		catch(...)
		{
			OW_LOG_DEBUG(getLogger(), "Caught unkown exception.");
			threwException = true;
		}
	}

	bool canFindLibs(String const& cmd)
	{
		StringArray command;
		command.push_back("/usr/bin/ldd");
		command.push_back(cmd);
		String output;
		int processStatus = 0;
		bool threwException = false;
		wrapExceptionThrowingExec(command, output, processStatus, threwException);
		OW_LOG_DEBUG(getLogger(), Format("ldd cmd: %1 %2", command[0], command[1]));
		OW_LOG_DEBUG(getLogger(), Format("ldd output: %1", output));
		OW_LOG_DEBUG(getLogger(), Format("ldd processStatus: %1", processStatus));
		OW_LOG_DEBUG(getLogger(), Format("ldd threwException: %1", threwException));
		return !threwException && (processStatus == 0) && (output.indexOf("not found") == String::npos);
	}

	/**
	 * Succeeds if the executable exists, is readable, and is not a
	 * directory.
	 */
	TestResultAccumulator saneExecutable()
	{
		using namespace FileSystem;
		TestResultAccumulator testResult;
		LOG_TEST;
		OW_LOG_DEBUG(getLogger(), Format("Testing sanity of executable %1 .", Config::config().owmofinstalltool_CommandName()));
		bool execExists = false;
		if (!(execExists = exists(Config::config().owmofinstalltool_CommandName())))
		{
			OW_LOG_ERROR(getLogger(), Format(" %1 does not exist!", Config::config().owmofinstalltool_CommandName()));
		}
		testResult&= execExists;
		bool execReadable = false;
		if (!(execReadable = canRead(Config::config().owmofinstalltool_CommandName())))
		{
			OW_LOG_ERROR(getLogger(), Format(" %1 cannot be read!", Config::config().owmofinstalltool_CommandName()));
		}
		testResult&= execReadable;
		bool execNotDir = false;
		if (!(execNotDir = !isDirectory(Config::config().owmofinstalltool_CommandName())))
		{
			OW_LOG_ERROR(getLogger(), Format(" %1 is a directory!", Config::config().owmofinstalltool_CommandName()));
		}
		testResult&= execNotDir;
		bool execExecutable = false;
		if (!(execExecutable = executable(Config::config().owmofinstalltool_CommandName())))
		{
			OW_LOG_ERROR(getLogger(), Format(" %1 is not executable!", Config::config().owmofinstalltool_CommandName()));
		}
		testResult&= execExecutable;
		bool bcanFindLibs = false;
		if (!(bcanFindLibs = canFindLibs(Config::config().owmofinstalltool_CommandName())))
		{
			OW_LOG_ERROR(getLogger(), Format(" Cannot find libs for %1 .", Config::config().owmofinstalltool_CommandName()));
		}
		testResult&= bcanFindLibs;
		return testResult;
	}

	/**
	 * Succeeds if the version output contains the tool name, OpenWBEM,
	 * and the OpenWBEM version number.
	 */
	TestResultAccumulator versionS(String const& versionOpt)
	{
		TestResultAccumulator testResult;
		LOG_TEST;
		StringArray command;
		command.push_back(Config::config().owmofinstalltool_CommandName());
		command.push_back("--config=" + Config::config().configFile());
		command.push_back(versionOpt);
		String output;
		int processStatus;
		bool threwException = false;
		wrapExceptionThrowingExec(command, output, processStatus, threwException);
		testResult&= (!threwException);
		testResult&= (output.indexOf(owmofinstalltool) != String::npos);
		testResult&= (output.indexOf("(OpenWBEM)") != String::npos);
		testResult&= (output.indexOf(OW_VERSION) != String::npos);
		testResult&= (processStatus == 0);
		if (!testResult)
		{
			OW_LOG_ERROR(getLogger(), Format("versionS failed, output of %1 %2 was %3 .", command[0], command[1], output));
			OW_LOG_ERROR(getLogger(), Format("processStatus %1", processStatus));
			OW_LOG_ERROR(getLogger(), Format("threwException %1", threwException));
			OW_LOG_ERROR(getLogger(), Format("OW_VERSION %1", OW_VERSION));
		}
		return testResult;
	}

	/**
	 * Succeeds if the version output contains the tool name, OpenWBEM,
	 * and the OpenWBEM version number.
	 */
	TestResultAccumulator version()
	{
		TestResultAccumulator testResult;
		LOG_TEST;
		testResult&= versionS("-v");
		testResult&= versionS("--version");
		return testResult;
	}

	/**
	 * Succeeds if the help option does not result in a thrown exception
	 * or a non-zero process return value.
	 */
	TestResultAccumulator helpS(String const& helpOpt)
	{
		TestResultAccumulator testResult;
		LOG_TEST;
		StringArray command;
		command.push_back(Config::config().owmofinstalltool_CommandName());
		command.push_back("--config=" + Config::config().configFile());
		command.push_back(helpOpt);
		String output;
		int processStatus = badProcessStatus;
		bool threwException = false;
		wrapExceptionThrowingExec(command, output, processStatus, threwException);
		testResult&= (!threwException);
		testResult&= (processStatus == 0);
		//FIXME: Put in some code that tests real invariants on the help
		//    output.
		return testResult;
	}

	/**
	 * Succeeds if the help option does not result in a thrown exception
	 * or a non-zero process return value.
	 */
	TestResultAccumulator help()
	{
		TestResultAccumulator testResult;
		LOG_TEST;
		testResult&= helpS("-h");
		testResult&= helpS("--help");
		return testResult;
	}

	String const emptyMofFileName("empty.mof");
	String const trivialMofFileName("trivial.mof");

	template<class function>
	TestResultAccumulator mof(function f, String filename)
	{
		TestResultAccumulator testResult;
		LOG_TEST;
		testResult&= f(filename);
		return testResult;
	}

	TestResultAccumulator diffFiles(String const& f0, String const& f1)
	{
		TestResultAccumulator testResult;
		LOG_TEST;
		StringArray command;
		command.push_back("/usr/bin/diff");
		command.push_back(f0);
		command.push_back(f1);
		String output;
		int processStatus = badProcessStatus;
		bool threwException = false;
		wrapExceptionThrowingExec(command, output, processStatus, threwException);
		testResult&= (!threwException);
		testResult&= (processStatus == 0);
		if (!testResult)
		{
			OW_LOG_DEBUG(getLogger()
				, Format("Fail. processStatus: %1 threwException %2 output %3"
					, processStatus, threwException, output));
		}
		return testResult;
	}

	TestResultAccumulator installMofFile(String filename)
	{
		using namespace FileSystem;
		TestResultAccumulator testResult;
		LOG_TEST;
		StringArray command;
		command.push_back(Config::config().owmofinstalltool_CommandName());
		command.push_back("--config=" + Config::config().configFile());
		command.push_back("--owmofc-options=-c -n root/cimv2");
		//FIXME: 'make install' should do this.
		command.push_back("--create-mofdir");
		command.push_back("--add");
		command.push_back(filename);
		String output;
		int processStatus = badProcessStatus;
		bool threwException = false;
		wrapExceptionThrowingExec(command, output, processStatus, threwException);
		testResult&= (!threwException);
		testResult&= (processStatus == 0);
		//FIXME: Add untokenize to OW_String.
		if (processStatus != 0)
		{
			String flattenedCommand;
			for (size_t c = 0; c < command.size() ; ++c)
			{flattenedCommand+= " " + command[c];}
			try
			{
			OW_LOG_ERROR(getLogger(), Format("command %1 failed! output was: %2"
					, flattenedCommand, output));
			}
			catch(Exception const& e)
			{
				OW_LOG_DEBUG(getLogger(), Format("Caught exception of type %1 with message %2", e.type(), e.what()));
				threwException = true;
			}
			catch(...)
			{
				OW_LOG_DEBUG(getLogger(), "Caught unkown exception.");
				threwException = true;
				throw;
			}
		}
		String installedFilename = Config::config().mofDir() + "/" + filename.substring(filename.lastIndexOf("/"));
		OW_LOG_DEBUG(getLogger()
			, Format("installedFilename expected to be: %1 which %2 ."
				, installedFilename
				, (exists(installedFilename)?"exists":"does not exist")));
		testResult&= exists(installedFilename);
		testResult&= diffFiles(filename, installedFilename);
		if (!testResult)
		{
			OW_LOG_DEBUG(getLogger()
				, Format("Fail. processStatus: %1 threwException %2 output %3"
					, processStatus, threwException, output));
		}
		else
		{
			OW_LOG_DEBUG(getLogger()
				, Format("Success. processStatus: %1 threwException %2 output %3"
					, processStatus, threwException, output));
		}
		return testResult;
	}

	TestResultAccumulator removeMofFile(String filename)
	{
		TestResultAccumulator testResult;
		LOG_TEST;
		StringArray command;
		command.push_back(Config::config().owmofinstalltool_CommandName());
		command.push_back("--config=" + Config::config().configFile());
		command.push_back("--remove");
		command.push_back(filename);
		String output;
		int processStatus = badProcessStatus;
		bool threwException = false;
		wrapExceptionThrowingExec(command, output, processStatus, threwException);
		testResult&= (!threwException);
		testResult&= (processStatus == 0);
		testResult&= !FileSystem::exists(filename);
		return testResult;
	}

	namespace NaddMoff
	{
		TestResultAccumulator emptyMof()
		{
			return mof(installMofFile, Config::config().testMofDir() + "/" + emptyMofFileName);
		}

		TestResultAccumulator trivialMof()
		{
			return mof(installMofFile, Config::config().testMofDir() + "/" + trivialMofFileName);
		}

		String getSchemaMofFileList()
		{
			//FIXME: Should I try to test more of the schema?
			String list = Config::config().stageDir() + "/../../../schemas/cim28/CIM_Core28.mof";
			return list;
		}

		TestResultAccumulator schemaMof()
		{
			TestResultAccumulator testResult;
			LOG_TEST;
			StringArray schemaMofFileList = getSchemaMofFileList().tokenize("\n");
			OW_LOG_DEBUG(getLogger(), getSchemaMofFileList());
			for (size_t c = 0 ; c < schemaMofFileList.size() ; ++c)
			{
				testResult&= mof(installMofFile, schemaMofFileList[c]);
			}
			return testResult;
		}
	}

	TestResultAccumulator addMof()
	{
		TestResultAccumulator testResult;
		LOG_TEST;
		using namespace NaddMoff;
		testResult&= testWrapper(&emptyMof);
		testResult&= testWrapper(&trivialMof);
		testResult&= testWrapper(&schemaMof);
		//testResult&= testWrapper(&complicatedMof);
		//testResult&= testWrapper(&instancesOfNonExistantClasses);
		//testResult&= testWrapper(&badClassesWithSameName);
		return testResult;
	}

	namespace NremoveMoff
	{
		TestResultAccumulator emptyMof()
		{
			return mof(removeMofFile, Config::config().testMofDir() + "/" + emptyMofFileName);
		}

		TestResultAccumulator trivialMof()
		{
			return mof(removeMofFile, Config::config().testMofDir() + "/" + trivialMofFileName);
		}
	}

	TestResultAccumulator removeMof()
	{
		TestResultAccumulator testResult;
		LOG_TEST;
		using namespace NremoveMoff;
		testResult&= testWrapper(&emptyMof);
		testResult&= testWrapper(&trivialMof);
		//testResult&= testWrapper(&complicatedMof);
		//testResult&= testWrapper(&instancesOfNonExistantClasses);
		//testResult&= testWrapper(&badClassesWithSameName);
		return testResult;
	}
}

int main(int argc, char* argv[])
{
	try
	{
		String stageDir;
		if (argc > 1)
		{
			stageDir = argv[1];
		}
		OW_LOG_DEBUG(getLogger(), Format("stageDir: %1", stageDir));
		String configFile;
		if (argc > 2)
		{
			configFile = argv[2];
		}
		OW_LOG_DEBUG(getLogger(), Format("configFile: %1", configFile));
		test::Config::init(stageDir, configFile);

		using namespace test;
		TestResultAccumulator testResult;
		LOG_TEST;
		testResult&= testWrapper(&test::saneExecutable);
		testResult&= testWrapper(&test::version);
		testResult&= testWrapper(&test::help);
		testResult&= testWrapper(&test::addMof);
		//testResult&= testWrapper(&test::removeMof);
		//testResult&= testWrapper(&test::verbose);
		return testResult.failures() != 0;
	}
	catch(Exception const& e)
	{
		OW_LOG_DEBUG(getLogger(), Format("Caught exception: e.what() %1 e.type() %2", e.what(), e.type()));
		return EXIT_FAILURE;
	}
	catch(std::exception const& e)
	{
		OW_LOG_DEBUG(getLogger(), Format("Caught exception: e.what() %1 typeid(e).name() %2", e.what(), typeid(e).name()));
		return EXIT_FAILURE;
	}
	catch(...)
	{
		OW_LOG_DEBUG(getLogger(), "Caught Unkown exception.");
		return EXIT_FAILURE;
	}
}

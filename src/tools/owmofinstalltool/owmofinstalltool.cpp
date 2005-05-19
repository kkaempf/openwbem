#include "OW_config.h"
#include "OW_Array.hpp"
#include "OW_Assertion.hpp"
#include "OW_CerrLogger.hpp"
#include "OW_CmdLineParser.hpp"
#include "OW_ConfigException.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Exec.hpp"
#include "OW_FileSystem.hpp"
#include "OW_Format.hpp"
#include "OW_Logger.hpp"
#include "OW_PidFile.hpp"
#include "OW_Thread.hpp"
#include "OW_String.hpp"

#include <cerrno>
#include <iostream>
#include <ostream>
extern "C"
{
	//FIXME: find or make a platform abstraction layer for kill.
#include <signal.h>
}

using namespace OW_NAMESPACE;

//FIXME: The spec and what this program does need to be made
//    consistent.

//FIXME: Do something about the kludgy path manipulation in this
//    program.

//FIXME: There's some confusion here about how errors should be
//    output. I need to do some thinking on the attributes of
//    OW_LOG_DEBUG, OW_LOG_ERROR, cerr, cout, etc.

//FIXME: I need to think about the relationship between verbosity and
//    debug logging. Verbosity is information that is useful to both
//    the programmer and the user. Debug logging is useful only to the
//    programmer.

//FIXME: I need to think about my error-handling strategy with respect
//    to return values versus exceptions.

//FIXME: Stop ignoring the test only flag.

//FIXME: Stop ignoring the check consistency flag.

//FIXME: Do something about upgrading.

LoggerRef createLogger()
{
	//FIXME: make this use a logger for a specific file.  FIXME: Why
	//on earth do I have this extra layer of indirection? I think I
	//planned to do more complicated initialization of this later.
	static LoggerRef logger(new CerrLogger());
	return logger;
}

LoggerRef getLogger()
{
	static LoggerRef logger(createLogger());
	return logger;
}

namespace CommandSyms
{
	/*
	 This enum represents the id or opcode part of a command which has
	 been decoded. Each symbol here represents a command this tool can
	 carry out.
	 */
	enum CommandSyms
	{
		E_display_version,
		E_set_verbosity,
		E_display_help,
		E_owmofc_options,

		//Add specified list of mof files to current openwbem install.
		E_add_mof,

		//Remove specified list of mof files from current openwbem install.
		E_remove_mof,

		E_set_config,

		//'mof dir' is the directory which contains the mof files, and
		//  which is kept in sync with the cimom repository.
		E_create_mof_dir,
		E_set_mof_dir,
		E_set_cimom_repository,

		//Set the location of mof dir backup.
		E_set_mof_dir_backup_location,

		//Set the location of the repository backup.
		E_set_repository_backup_location,

		//Don't commit any modifying operations; just print any messages
		//  which would be generated.
		E_set_test_only_flag,

		//If the repository and the mof directory are inconsistent, print
		//any error messages.
		E_check_consistency,

		//Replace the mof in the repository with what is in the directory.
		E_stomp_repository,

		//Replace the mof in the directory with what is in the repository.
		E_stomp_mofdir
	};

	int const numberOfCommandSyms = E_stomp_mofdir + 1;

	String commandSymName(CommandSyms sym)
	{
		static const char* commandSymNames[]=
		{
			"display_version",
			"set_verbosity",
			"display_help",
			"owmofc_options",
			"add_mof",
			"remove_mof",
			"set_config",
			"create_mof_dir",
			"set_mof_dir",
			"set_cimom_repository",
			"set_mof_dir_backup_location",
			"set_repository_backup_location",
			"set_test_only_flag",
			"check_consistency",
			"stomp_repository",
			"stomp_mofdir"
		};

		if (sym >= 0 && sym < numberOfCommandSyms)
		{
			return commandSymNames[sym];
		}
		else
		{
			return "invalid_command";
		}
	}
}

/*
 This enum represents command line options. It is not synonymous with
 CommandSyms because (a) command line options are not the only source
 of commands, and (b) I do not wish to force a one-to-one relationship
 between command-line options and commands. However none of this
 worked out as planned; there is a one-to-one relationship between
 command-line options and commands. In retrospect I don't think this
 layer of translation gained me anything.
 */
namespace OptionSyms
{
	enum OptionSyms
	{
		E_OPTconfig,
		E_OPTversion,
		E_OPTverbose,
		E_OPThelp,
		E_OPTcreate_mof_dir,
		E_OPTmofdir,
		E_OPTrepository,
		E_OPTowmofc_options,
		E_OPTmof_dir_backup_location,
		E_OPTrepository_backup_location,
		E_OPTtest,
		E_OPTcheck_consistency,
		E_OPTadd,
		E_OPTremove,
		E_OPTstomp_repository_with_mofdir,
		E_OPTstomp_mofdir_with_repository
	};

	int const numberOfOptionSyms = E_OPTstomp_mofdir_with_repository + 1;

	String optionSymName(OptionSyms sym)
	{
		static const char* optionSymNames[]=
		{
			"OPTconfig",
			"OPTversion",
			"OPTverbose",
			"OPThelp",
			"OPTcreate_mof_dir",
			"OPTmofdir",
			"OPTrepository",
			"OPTowmofc_options",
			"OPTmof_dir_backup_location",
			"OPTrepository_backup_location",
			"OPTtest",
			"OPTcheck_consistency",
			"OPTadd",
			"OPTremove",
			"OPTstomp_repository_with_mofdir",
			"OPTstomp_mofdir_with_repository"
		};

		if (sym >= 0 && sym < numberOfOptionSyms)
		{
			return optionSymNames[sym];
		}
		else
		{
			return "invalid_option";
		}
	}

	CmdLineParser::Option options[]=
	{
		{ E_OPTconfig, 'c', "config", CmdLineParser::E_REQUIRED_ARG, 0
			, "Set configuration file." },
		{ E_OPTversion, 'v', "version", CmdLineParser::E_NO_ARG, 0
			, "Display version information." },
		{ E_OPTverbose, '\0', "verbose", CmdLineParser::E_OPTIONAL_ARG, "0"
			, "Output verbose messages about what is being done and why." },
		{ E_OPThelp, 'h', "help", CmdLineParser::E_NO_ARG, 0
			, "Summary of syntax and options." },
		{ E_OPTcreate_mof_dir, '\0', "create-mofdir", CmdLineParser::E_OPTIONAL_ARG, "true"
			, "Create the mof directory if it does not exist." },
		{ E_OPTmofdir, 'm', "mofdir", CmdLineParser::E_REQUIRED_ARG, 0
			, "Specify mof directory to operate on." },
		{ E_OPTrepository, 'd', "repository", CmdLineParser::E_REQUIRED_ARG, 0
			, "Specify directory of repository to operate on." },
		{ E_OPTowmofc_options, 'd', "owmofc-options", CmdLineParser::E_REQUIRED_ARG, 0
			, "Specify owmofc options to use." },
		{ E_OPTmof_dir_backup_location, '\0', "mofdir-backup-location", CmdLineParser::E_REQUIRED_ARG, 0
			, "Specify location of mof directory backup." },
		{ E_OPTrepository_backup_location, '\0', "repository-backup-location", CmdLineParser::E_REQUIRED_ARG, 0
			, "Specify where to backup the repository." },
		{ E_OPTtest, 't', "test", CmdLineParser::E_OPTIONAL_ARG, "true"
			, "Do not commit any modifications to repository or mof directory; only show what would happen." },
		{ E_OPTcheck_consistency, '\0', "check_consistency", CmdLineParser::E_OPTIONAL_ARG
			, "true", "Check the consistency of the repository and the mof directory." },
		{ E_OPTadd, 'a', "add", CmdLineParser::E_REQUIRED_ARG, 0
			, "Add mof in comma-separated file list to openwbem installation." },
		{ E_OPTremove, 'r', "remove", CmdLineParser::E_REQUIRED_ARG, 0
			, "Remove mof in comma-separated file list from openwbem installation." },
		{ E_OPTstomp_repository_with_mofdir, '\0', "stomp_repository_with_mofdir"
			, CmdLineParser::E_NO_ARG, 0
			, "Replace the mof in the repository with the mof in the mof directory." },
		{ E_OPTstomp_mofdir_with_repository, '\0', "stomp_mofdir_with_repository"
			, CmdLineParser::E_NO_ARG, 0
			, "Replace the mof in the mof directory with the mof in the repository." },
		{0, 0, 0, CmdLineParser::E_NO_ARG, 0, 0}
	};
};

/*
 This struct represents a full decoded command, with both an
 identifying symbol (or id) and a list of parameters.

 FIXME: Consider writing some constructors for this type. For example,
 appropriate constructors would allow 'Command displayVersion =
 {E_display_version, StringArray()};' to be written as 'Command
 displayVersion(E_display_version);', a savings of 28% (17
 characters).
 */
struct Command
{
	CommandSyms::CommandSyms commandSym;
	StringArray parameters;
};
typedef Array<Command> CommandList;

struct CommandExecutionEnvironment
{
	size_t verbosity;
	bool testOnly;
	bool checkConsistency;
	bool createMofDir;
	String mofdir;
	String repositoryDir;
	String owmofcOptions;
	String startCimomScript;
	String owExecPrefix;
	String mofdirBackupLocation;
	String repositoryBackupLocation;
	ConfigFile::ConfigMap configMap;
};

//FIXME: Think about encapsulating this thing.
static CommandExecutionEnvironment commandExecutionEnvironment;

String assumeFileInMofDir(String const& fname)
{
	String fixedFname;
	if (fname.indexOf(commandExecutionEnvironment.mofdir) == 0)
	{
		fixedFname = fname;
	}
	else
	{
		fixedFname = commandExecutionEnvironment.mofdir + "/" + fname;
	}
	return fixedFname;
}

std::ostream& operator<< (std::ostream& lhs, Command const& rhs)
{
	lhs << "{ " << commandSymName(rhs.commandSym) << " ";
	if (!rhs.parameters.empty())
	{
		lhs << "{ ";
		for (size_t cur = 0 ; cur < rhs.parameters.size() - 1 ; ++cur)
		{
			lhs << rhs.parameters[cur] << " ";
		}
		lhs << rhs.parameters.back() << " } }";
	}
	else
	{
		lhs << "no parameters }";
	}
	return lhs;
}

/**
 * Push a command to set the config file on the command list.  The
 * CmdLineParser 'parser' is used to determine if a command line
 * argument was used to set the config file. If not,
 * OW_DEFAULT_CONFIG_FILE is used.
 */
void pushConfigCommand(CommandList& commands, CmdLineParser const& parser, LoggerRef const& logger)
{
	Command setConfig = {CommandSyms::E_set_config, StringArray()};
	String parameter = parser.getOptionValue(OptionSyms::E_OPTconfig
		, OW_DEFAULT_CONFIG_FILE );
	setConfig.parameters.push_back(parameter);
	OW_LOG_DEBUG(logger, Format("Pusing %1 onto list.", setConfig));
	commands.push_back(setConfig);
}

/**
 * putOptOnCmdListIfNeeded()
 *
 * If the command line option corresponding to optionSym is present on the
 *     command line, and has correct syntax, place the equivalent sequence
 *     of commands on the CommandList 'commands'.
 *
 * If no command line option corresponding to optionSym is present,
 *     place appropriate commands on the command list iff necessary
 *     for the default action.
 *
 * If the command line option corresponding to optionSym is present,
 *     but the syntax is incorrect, place nothing on the command list,
 *     and return false.
 *
 * Note: This function inserts commands into the command list in the
 *     order the equivalent options appear in OptionSyms, NOT the
 *     order the options occur on the command line.
 *
 * @param optionSym
 *
 *     The symbolic name of the command line option whose presence
 *     will be checked for.
 *
 * @param parser
 *
 *     A command line parser object from which to retrieve information
 *     about the command line.
 *
 * @param commands
 *
 *     A (potentially non-empty) list of commands to which will be
 *     added commands equivalent to optionSym.
 *
 * @return
 *
 *     true if the syntax of the command line option represented by
 *     optionSym is correct, false if it is not.
 *
 */
bool putOptOnCmdListIfNeeded(
	OptionSyms::OptionSyms optionSym,
	CmdLineParser const& parser,
	CommandList& commands,
	LoggerRef const& logger
)
{
	using namespace OptionSyms;
	using namespace CommandSyms;
	bool noParserError = true;
	OW_LOG_DEBUG(logger, Format("Checking for option %1 %2", optionSym, optionSymName(optionSym)));
	if (optionSym >= 0 && optionSym < numberOfOptionSyms)
	{
		if (parser.isSet(optionSym))
		{
			//FIXME: Try condensing this code by replacing this switch with a map.
			switch(optionSym)
			{
				case E_OPTconfig:
					pushConfigCommand(commands, parser, logger);
					break;

				case E_OPTversion:
					{
						Command displayVersion = {E_display_version, StringArray()};
						OW_LOG_DEBUG(logger, Format("Pushing %1 onto list.", displayVersion));
						commands.push_back(displayVersion);
					}
					break;

				case E_OPTverbose:
					{
						Command setVerbosity = {E_set_verbosity, StringArray()};
						String parameter = parser.getOptionValue(E_OPTverbose, "0");
						try
						{
							//FIXME: Find a way to check for validity of this
							//argument without throwing an exception.
							parameter.toUInt32(10);
							setVerbosity.parameters.push_back(parameter);
							OW_LOG_DEBUG(logger, Format("Pushing %1 onto list.", setVerbosity));
							commands.push_back(setVerbosity);
						}
						catch(StringConversionException const&)
						{
							OW_LOG_ERROR( logger, Format( "Parameter %1 of option %2 is not convertible to UInt32.",
										parameter, optionSymName(optionSym)
									)
								);
							noParserError = false;
						}
					}
					break;

				case E_OPThelp:
					{
 						Command displayHelp = {E_display_help, StringArray()};
						OW_LOG_DEBUG(logger, Format("Pushing %1 onto list.", displayHelp));
						commands.push_back(displayHelp);
					}
					break;

				case E_OPTadd:
					{
						Command add = {E_add_mof, StringArray()};
						String parameter = parser.getOptionValue(E_OPTadd, "");
						if (parameter != "")
						{
							StringArray parameters = parameter.tokenize(",");
							add.parameters.swap(parameters);
							OW_LOG_DEBUG(logger, Format("Pushing %1 onto list.", add));
							commands.push_back(add);
						}
						else{ noParserError = false;}
					}
					break;

				case E_OPTremove:
					{
						Command remove = {E_remove_mof, StringArray()};
						String parameter = parser.getOptionValue(E_OPTremove, "");
						if (parameter != "")
						{
							StringArray parameters = parameter.tokenize(",");
							remove.parameters.swap(parameters);
							OW_LOG_DEBUG(logger, Format("Pushing %1 onto list.", remove));
							commands.push_back(remove);
						}
						else { noParserError = false; }
					}
					break;

				case E_OPTcreate_mof_dir:
					{
						Command createMofDir = {E_create_mof_dir, StringArray()};
						String parameter = parser.getOptionValue(E_OPTcreate_mof_dir, "false");
						createMofDir.parameters.push_back(parameter);
						OW_LOG_DEBUG(logger, Format("Pushing %1 onto list.", createMofDir));
						commands.push_back(createMofDir);
					}
					break;

				case E_OPTmofdir:
					{
						Command setMofDir = {E_set_mof_dir, StringArray()};
						String parameter = parser.getOptionValue(E_OPTmofdir
							, commandExecutionEnvironment.mofdir.c_str());
						setMofDir.parameters.push_back(parameter);
						OW_LOG_DEBUG(logger, Format("Pushing %1 onto list.", setMofDir));
						commands.push_back(setMofDir);
					}
					break;

				case E_OPTrepository:
					{
						Command setRepository = {E_set_cimom_repository, StringArray()};
						String parameter = parser.getOptionValue(E_OPTrepository
							, commandExecutionEnvironment.repositoryDir.c_str());
						setRepository.parameters.push_back(parameter);
						OW_LOG_DEBUG(logger, Format("Pushing %1 onto list.", setRepository));
						commands.push_back(setRepository);
					}
					break;

				case E_OPTowmofc_options:
					{
						Command owmofcOptions = {E_owmofc_options, StringArray()};
						String parameter = parser.getOptionValue(E_OPTowmofc_options
							, "error: bad owmofc options");
						owmofcOptions.parameters.push_back(parameter);
						OW_LOG_DEBUG(logger, Format("Pushing $1 onto list.", owmofcOptions));
						commands.push_back(owmofcOptions);
					}
					break;

				case E_OPTmof_dir_backup_location:
					{
						Command setMofDirBackupLocation =
							{E_set_mof_dir_backup_location, StringArray()};
						String parameter = parser.getOptionValue(E_OPTmof_dir_backup_location
							, commandExecutionEnvironment.mofdirBackupLocation.c_str());
						setMofDirBackupLocation.parameters.push_back(parameter);
						OW_LOG_DEBUG(logger, Format("Pushing %1 onto list.", setMofDirBackupLocation));
						commands.push_back(setMofDirBackupLocation);
					}
					break;

				case E_OPTrepository_backup_location:
					{
						Command setRepositoryBackupLocation =
							{E_set_mof_dir_backup_location, StringArray()};
						String parameter = parser.getOptionValue(E_OPTmof_dir_backup_location
							, commandExecutionEnvironment.repositoryBackupLocation.c_str());
						setRepositoryBackupLocation.parameters.push_back(parameter);
						OW_LOG_DEBUG(logger, Format("Pushing %1 onto list.", setRepositoryBackupLocation));
						commands.push_back(setRepositoryBackupLocation);
					}
					break;

				case E_OPTtest:
					{
						Command setTestOnlyFlag = {E_set_test_only_flag, StringArray()};
						String parameter = parser.getOptionValue(E_OPTtest, "false");
						setTestOnlyFlag.parameters.push_back(parameter);
						OW_LOG_DEBUG(logger, Format("Pushing %1 onto list.", setTestOnlyFlag));
						commands.push_back(setTestOnlyFlag);
					}
					break;

				case E_OPTcheck_consistency:
					{
						Command checkConsistency = {E_check_consistency, StringArray()};
						String parameter = parser.getOptionValue(E_OPTcheck_consistency, "false");
						checkConsistency.parameters.push_back(parameter);
						OW_LOG_DEBUG(logger, Format("Pushing %1 onto list.", checkConsistency));
						commands.push_back(checkConsistency);
					}
					break;

				case E_OPTstomp_repository_with_mofdir:
					{
						Command stompRepositoryWithMofdir = {E_stomp_repository, StringArray()};
						OW_LOG_DEBUG(logger, Format("Pushing %1 onto list.", stompRepositoryWithMofdir));
						commands.push_back(stompRepositoryWithMofdir);
					}
					break;

				case E_OPTstomp_mofdir_with_repository:
					{
						Command stompMofdirWithRepository = {E_stomp_mofdir, StringArray()};
						OW_LOG_DEBUG(logger, Format("Pushing %1 onto list.", stompMofdirWithRepository));
						commands.push_back(stompMofdirWithRepository);
					}
					break;
				default:
					OW_ASSERTMSG(0, "Bogus option id.");
			}
		}
		else
		{
			noParserError = true;
		}
	}
	else
	{
		noParserError = false;
	}
	return noParserError;
}

/**
 * checkForOptionsWhichConflict
 *
 * @param commands
 *     The list of commands which will be checked for conflicting options.
 *
 * @return
 *     true iff there are no conflicting options in 'commands', false otherwise.
 */
bool checkForOptionsWhichConflict(CommandList const& commands, LoggerRef const& logger)
{
	using namespace ::OptionSyms;
	OW_LOG_DEBUG(logger, "Checking options which cannot go together.");
	bool ok = true;
	Array<int> counters(numberOfOptionSyms);
	String exactlyOneOfThese;
	for (CommandList::const_iterator current = commands.begin()
				; current != commands.end() ; ++current)
	{
		int currentOpt = current->commandSym;
		OW_LOG_DEBUG(logger, Format("Checking for opt %1 %2"
				, currentOpt, optionSymName(::OptionSyms::OptionSyms(currentOpt))));
		++counters[currentOpt];
		if (counters[currentOpt] > 1)
		{
			OW_LOG_ERROR(logger, Format("Error, cannot have two of any option, but two of %1 detected."
					, optionSymName(::OptionSyms::OptionSyms(currentOpt))));
			ok&= false;
		}
		switch(currentOpt)
		{
			//FIXME: it's only ok to have 1 of each of these.
			case E_OPTconfig:
			case E_OPTverbose:
			case E_OPTcreate_mof_dir:
			case E_OPTmofdir:
			case E_OPTrepository:
			case E_OPTowmofc_options:
			case E_OPTmof_dir_backup_location:
			case E_OPTrepository_backup_location:
			case E_OPTtest:
			case E_OPTcheck_consistency:
				/*
				It's ok to have all of these together, or in combination with any one of the others.
				*/
				break;

			case E_OPTversion:
			case E_OPThelp:
			case E_OPTadd:
			case E_OPTremove:
			case E_OPTstomp_repository_with_mofdir:
			case E_OPTstomp_mofdir_with_repository:
				//It's only safe to have exactly one of these.
				if (!exactlyOneOfThese.empty())
				{
					OW_LOG_ERROR(logger, Format("Cannot have %1 in combination with %2 ."
							, exactlyOneOfThese
							, optionSymName(::OptionSyms::OptionSyms(currentOpt))));
					ok = false;
				}
				else
				{
					exactlyOneOfThese = optionSymName(::OptionSyms::OptionSyms(currentOpt));
				}
				break;
			default:
				OW_ASSERTMSG(0, "Bogus option id.");
		}
	}
	return ok;
}

/**
 * For each possible option, check to see if it is on the command
 * line. If it is on the command line, push an appropriate command
 * onto the command list.
 */
bool parseCommandLine(int argc, char** argv, CommandList& commands, LoggerRef const& logger)
{
	using namespace OW_NAMESPACE;
	using namespace OptionSyms;

	OW_LOG_DEBUG(logger, "Parsing command line.");
	CmdLineParser parser(argc, argv, options
		, CmdLineParser::E_NON_OPTION_ARGS_ALLOWED);

	bool allGood = true;

	//kludge in the config if it isn't set on the command line.
	OW_LOG_DEBUG(logger, "Checking wether config file is set on the command line.");
	if (!parser.isSet(E_OPTconfig))
	{
		OW_LOG_DEBUG(logger, "Config file not set on the command line.");
		pushConfigCommand(commands, parser, logger);
	}
	else
	{
		OW_LOG_DEBUG(logger, "Config file is set on command line; proper command should be pushed on later.");
	}

	for (int currentOpt = 0 ; currentOpt < numberOfOptionSyms ; ++currentOpt)
	{
		OW_LOG_DEBUG(logger, Format("Checking for opt %1 %2"
				, currentOpt, optionSymName(::OptionSyms::OptionSyms(currentOpt))));
		bool ok = putOptOnCmdListIfNeeded(::OptionSyms::OptionSyms(currentOpt)
			, parser, commands, logger);
		if (ok)
		{
			OW_LOG_DEBUG(logger, Format("Syntax of opt %1 %2 ok."
					, currentOpt, optionSymName(::OptionSyms::OptionSyms(currentOpt))));
		}
		else
		{
			//FIXME: This should print the offending option exactly as the user typed it in.
			OW_LOG_ERROR(logger, Format("Syntax of opt %1 %2 %3 not ok.", currentOpt
					, optionSymName(::OptionSyms::OptionSyms(currentOpt)), "FIXME"));
		}
		allGood&= ok;
	}
	OW_LOG_DEBUG(logger, Format("Done parsing command line. Good? %1", allGood));
	return allGood;
}

/**
 * Find all the commands that are relevant to this execution of
 * owmofinstalltool. Sources are the openwbem config file, other
 * aspects of openwbem configuration, and the command line.
 *
 * @param commands
 *
 *     Command discovered will be push_back()ed onto this list in the
 *     order they are to be executed.
 *
 */
bool discoverCommands(int argc, char** argv, CommandList& commands, LoggerRef const& logger)
{
	return parseCommandLine(argc, argv, commands, logger);
}

bool canSignalPid(pid_t pid, LoggerRef const& logger)
{
	int error = ::kill(pid, 0);
	bool good = true;
	if (error)
	{
		OW_LOG_ERROR(logger, Format("pid %1 failed with error %2 .", pid, std::strerror(errno)));
		good = false;
	}
	else
	{
		good = true;
	}
	return good;
}

bool getCimomPid(pid_t& pid, LoggerRef const& logger)
{
	using namespace FileSystem;
	using namespace ConfigFile;
	bool good = true;
	String pidFile(getConfigItem(commandExecutionEnvironment.configMap, ConfigOpts::PIDFILE_opt, OW_DEFAULT_PIDFILE));
	if (exists(pidFile) && canRead(pidFile) && (pid = PidFile::readPid(pidFile.c_str())) != -1)
	{
		good = canSignalPid(pid, logger);
	}
	else
	{
		pid = -1;
		good = false;
	}
	return good;
}

namespace sanityCheck
{
	/**
	* @return
	*     true if the file exists, is readable, and is not a directory.
	*/
	bool oneFile(String const& fname, LoggerRef const& logger)
	{
		using namespace FileSystem;
		bool allGood = true;
		if (!(allGood&= exists(fname)))
		{
			OW_LOG_ERROR(logger, Format("File %1 does not exist.", fname));
			return allGood;
		}
		if (!(allGood&= canRead(fname)))
		{
			OW_LOG_ERROR(logger, Format("File %1 cannot be read.", fname));
		}
		if (!(allGood&= !isDirectory(fname)))
		{
			OW_LOG_ERROR(logger, Format("File %1 is a directory.", fname));
		}
		return allGood;
	}

	/**
	* @return
	*     true if the file exists, is readable, writeable, and is a directory.
	*/
	bool isRWDir(String const& fname, LoggerRef const& logger)
	{
		using namespace FileSystem;
		bool allGood = true;
		if (!(allGood&= exists(fname)))
		{
			OW_LOG_ERROR(logger, Format("File %1 does not exist.", fname));
			return allGood;
		}
		if (!(allGood&= canRead(fname)))
		{
			OW_LOG_ERROR(logger, Format("File %1 cannot be read.", fname));
		}
		if (!(allGood&= canWrite(fname)))
		{
			OW_LOG_ERROR(logger, Format("File %1 cannot be written.", fname));
		}
		if (!(allGood&= isDirectory(fname)))
		{
			OW_LOG_ERROR(logger, Format("File %1 is not a directory.", fname));
		}
		return allGood;
	}

	/**
	* @return
	*    true if dname can be created.
	*/
	bool canCreateDir(String const& dname, LoggerRef const& logger)
	{
		String dnameParent = dname.substring(0, dname.lastIndexOf("/"));
		return !dname.empty() && (isRWDir(dnameParent, logger) || canCreateDir(dnameParent, logger)) && !FileSystem::exists(dname);
	}

	/**
	* Sanity check the mof dir.
	* @return
	*     true if dname exists, is readable, writeable, and is a directory.
	*/
	bool mofDir(String const& dname, LoggerRef const& logger)
	{
		OW_LOG_DEBUG(logger, Format("Checking sanity of mofdir %1 .", dname));
		//FIXME: This should not check canCreateDir() unless
		//    commandExecutionEnvironment.createMofDir is true. However,
		//    commandExecutionEnvironment.createMofDir is not set until
		//    commands start getting applied.
		return isRWDir(dname, logger) || canCreateDir(dname, logger);
		//return true;
	}

	/**
	* Sanity check the repository dir.
	* @return
	*     true if dname exists, is readable, writeable, and is a directory.
 	*/
	bool repositoryDir(String const& dname, LoggerRef const& logger)
	{
		return isRWDir(dname, logger);
	}

	bool cimomRunning(LoggerRef const& logger)
	{
		pid_t dummy;
		return getCimomPid(dummy, logger);
	}

	/**
	* @return
	*   true if the add mof command can be executed sanely.
	*
	* 'sanely' means:
	*
	*	(a) The mof dir exists, and is both readable and writeable.
	*
	*	(b) The cimom repository exists.
	*
	*	(c) The mof files to be added are readable.
	*
	*  (d) The cimom is not running.
	*
	* @param parameters
	*   The list of files to be added.
	*/
	bool addMof(StringArray const& parameters, LoggerRef const& logger)
	{
		bool allGood = true;

		//Emit errors for any config problems.

		//Sanity check the mof dir.
		OW_LOG_DEBUG(logger, Format("Sanity checking %1 .", commandExecutionEnvironment.mofdir));
		allGood&= sanityCheck::mofDir(commandExecutionEnvironment.mofdir, logger);
		OW_LOG_DEBUG(logger, Format("%1", (allGood?"good":"bad")));

		//Sanity check the repositoryDir.
		OW_LOG_DEBUG(logger, Format("Sanity checking %1 .", commandExecutionEnvironment.repositoryDir));
		allGood&= sanityCheck::repositoryDir(commandExecutionEnvironment.repositoryDir, logger);
		OW_LOG_DEBUG(logger, Format("%1", (allGood?"good":"bad")));

		//Sanity check the backup location for the mof dir.
		//allGood&= sanityCheck::isRWDir(commandExecutionEnvironment.mofdirBackupLocation, logger);

		//Sanity check the backup location for the repository.
		//allGood&= sanityCheck::isRWDir(commandExecutionEnvironment.repositoryBackupLocation, logger);

		//Emit errors for any files in the parameters which do not exist.
		for (StringArray::const_iterator cur = parameters.begin() ; cur != parameters.end() ; ++cur)
		{
			allGood&= sanityCheck::oneFile(*cur, logger);
			OW_LOG_DEBUG(logger, Format("%1", (allGood?"good":"bad")));
		}
		return allGood;
	}

	/**
	* @return
	*   true if the remove command can be executed sanely with the
	*   files listed in parameters.
	*
	* @param parameters
	*   The list of files to be added.
	*/
	bool removeMof(StringArray const& parameters, LoggerRef const& logger)
	{
		bool allGood;
		//Sanity check the mof dir.
		allGood&= sanityCheck::mofDir(commandExecutionEnvironment.mofdir, logger);

		//Sanity check the backup location for the mof dir.
		allGood&= sanityCheck::isRWDir(commandExecutionEnvironment.mofdirBackupLocation, logger);

		//Sanity check the backup location for the repository.
		allGood&= sanityCheck::isRWDir(commandExecutionEnvironment.repositoryBackupLocation, logger);

		//Emit errors for any files in the parameters which do not exist.
		for (StringArray::const_iterator cur = parameters.begin() ; cur != parameters.end() ; ++cur)
		{
			allGood&= sanityCheck::oneFile(assumeFileInMofDir(*cur), logger);
		}

		return allGood;
	}

	bool isInt(String const& maybeInt)
	{
		try
		{
			maybeInt.toInt();
		}
		catch(StringConversionException const&)
		{
			return false;
		}
		return true;
	}
}

bool sanityCheckCommand(Command const& command, LoggerRef const& logger)
{
	using namespace CommandSyms;
	bool good = false;
	OW_LOG_DEBUG(logger, Format("Sanity checking command %1 .", commandSymName(command.commandSym)));
	switch(command.commandSym)
	{
		case E_display_version:
			good = (command.parameters.size() == 0);
			break;

		case E_set_verbosity:
			good = (command.parameters.size() == 0 || (command.parameters.size() == 1 && sanityCheck::isInt(command.parameters[0])));
			break;

		case E_display_help:
			good = (command.parameters.size() == 0);
			break;

		case E_owmofc_options:
			//Don't make any assumptions about what the owmofc options 'should' be.
			good = true;
			break;

		case E_add_mof:
			good = sanityCheck::addMof(command.parameters, logger);
			break;

		case E_remove_mof:
			good = sanityCheck::removeMof(command.parameters, logger);
			break;

		case E_set_config:
			good = command.parameters.size() == 1 && sanityCheck::oneFile(command.parameters[0], logger);
			break;

		case E_create_mof_dir:
			good = (command.parameters.size() == 0 || command.parameters.size() == 1);
			break;

		case E_set_mof_dir:
			good = (command.parameters.size()==1 && sanityCheck::mofDir(command.parameters[0], logger));
			break;

		case E_set_cimom_repository:
			good = (command.parameters.size()==1 && sanityCheck::repositoryDir(command.parameters[0], logger));
			break;

		case E_set_mof_dir_backup_location:
			good = (command.parameters.size()==1 && sanityCheck::isRWDir(command.parameters[0], logger));
			break;

		case E_set_repository_backup_location:
			good = (command.parameters.size()==1 && sanityCheck::isRWDir(command.parameters[0], logger));
			break;

		case E_set_test_only_flag:
			good = (command.parameters.size() == 0 || command.parameters.size() == 1);
			break;

		case E_check_consistency:
			good = (command.parameters.size() == 0 || command.parameters.size() == 1);
			break;

		case E_stomp_repository:
			good =
				( sanityCheck::repositoryDir(commandExecutionEnvironment.repositoryDir, logger)
					&& sanityCheck::mofDir(commandExecutionEnvironment.mofdir, logger));
			break;

		case E_stomp_mofdir:
			good =
				( sanityCheck::repositoryDir(commandExecutionEnvironment.repositoryDir, logger)
					&& sanityCheck::mofDir(commandExecutionEnvironment.mofdir, logger));
			break;
		default:
			OW_ASSERTMSG(0, "Bogus option id.");
	}
	OW_LOG_DEBUG(logger, Format("Sanity checking %1, %2", commandSymName(command.commandSym), (good?"succeeded":"failed")));
	return good;
}

/**
* Fill out commandExecutionEnvironment with reasonable values based
*     on the openwbem configuration, but not limited to the config
*     file itself.
*
* @return
*
*     true if no errors are encountered, false otherwise.
*/
bool setConfig(StringArray const& parameters, LoggerRef const& logger)
{
	using namespace ConfigFile;
	bool good = true;
	try
	{
		loadConfigFile(parameters[0], commandExecutionEnvironment.configMap);
		OW_LOG_DEBUG(logger, Format("Using config file %1 .", parameters[0]));
		commandExecutionEnvironment.verbosity = 0;
		commandExecutionEnvironment.mofdir =
			getConfigItem(commandExecutionEnvironment.configMap
				, ConfigOpts::MOF_DIR_opt);
		OW_LOG_DEBUG(logger, Format("Using mofdir %1 ."
				, commandExecutionEnvironment.mofdir));
		commandExecutionEnvironment.repositoryDir =
			getConfigItem(commandExecutionEnvironment.configMap
				, ConfigOpts::DATADIR_opt);
		OW_LOG_DEBUG(logger, Format("Using repositoryDir %1 ."
				, commandExecutionEnvironment.repositoryDir));
		commandExecutionEnvironment.startCimomScript = OW_DEFAULT_CIMOM_START_SCRIPT;
		OW_LOG_DEBUG(logger, Format("Using startCimomScript %1 ."
				, commandExecutionEnvironment.startCimomScript));

		commandExecutionEnvironment.owExecPrefix = getConfigItem(commandExecutionEnvironment.configMap, ConfigOpts::BIN_DIR_opt, OW_DEFAULT_BIN_DIR);
	}
	catch(ConfigException const& c)
	{
		OW_LOG_DEBUG(logger, Format("Caught ConfigException %1 .", c.what()));
		good = false;
	}
	return good;
}

bool displayVersion(StringArray const& /* Ignored.*/, LoggerRef const& /* Ignored.*/)
{
	return (std::cout << "owmofinstalltool (OpenWBEM) " << OW_VERSION << '\n');
}

bool setVerbosity(StringArray const& parameters, LoggerRef const& /* Ignored.*/)
{
	bool good;
	try
	{
		if (parameters.size() == 1)
		{
			commandExecutionEnvironment.verbosity = parameters[0].toUInt32();
			good = true;
		}
		else if (parameters.size() == 0)
		{
			commandExecutionEnvironment.verbosity = 1;
			good = true;
		}
		else
		{
			good = false;
		}
	}
	catch(StringConversionException const&)
	{
		good = false;
	}
	return good;
}

bool displayHelp(StringArray const& /* Ignored.*/, LoggerRef const& /* Ignored.*/)
{
	return (std::cout << CmdLineParser::getUsage(OptionSyms::options) << "\n");
}

/**
 * Copy the given source file to the destination
 *
 * @param destination	The name of the file to copy
 *
 * @param source	The destination for the destination
 *
 * @return true if the operation succeeds. Otherwise false.
 */
bool copyFile(String const& source,String const& destination, LoggerRef const& logger)
{
	//FIXME: make this work for non-unix.
	//also, refactor this with wrapSillyExceptionThrowingExec().
	StringArray command;
	//FIXME:
	command.push_back("/bin/cp");
	if (!FileSystem::executable(command[0]))
	{
		OW_LOG_ERROR(logger, Format("%1 is not executable!", command[0]));
		return false;
	}
	command.push_back(source);
	command.push_back(destination);
	String output;
	int processStatus = 1;
	int timeoutSeconds = 300;
	int outputLimit = -1;
	bool threwException = false;
	OW_LOG_DEBUG(logger, Format("Executing: %1 %2 %3", command[0], command[1], command[2]));
	try
	{
		Exec::executeProcessAndGatherOutput(command, output, processStatus, timeoutSeconds, outputLimit);
		threwException = false;
	}
	catch(Exception const& e)
	{
		OW_LOG_DEBUG(logger, Format("Caught exception of type %1 with message %2", e.type(), e.what()));
		threwException = true;
	}
	return !threwException && !processStatus;
}

/**
 * Copy the given source directory to the destination, recursing into subdirectories.
 *
 * @param destination	The name of the file to copy
 *
 * @param source	The destination for the destination
 *
 * @return true if the operation succeeds. Otherwise false.
 */
bool copyDir(String const& source,String const& destination, LoggerRef const& logger)
{
	//FIXME: make this work for non-unix.
	//also, refactor this with wrapSillyExceptionThrowingExec().
	StringArray command;
	//FIXME:
	command.push_back("/bin/cp");
	if (!FileSystem::executable(command[0]))
	{
		OW_LOG_ERROR(logger, Format("%1 is not executable!", command[0]));
		return false;
	}
	command.push_back("-R"); //FIXME: does cp -R recurse sanely on all our platforms?
	command.push_back(source);
	command.push_back(destination);
	String output;
	int processStatus;
	int timeoutSeconds = 100;
	int outputLimit = -1;
	bool threwException;
	try
	{
		Exec::executeProcessAndGatherOutput(command, output, processStatus, timeoutSeconds, outputLimit);
		threwException = false;
	}
	catch(Exception const& e)
	{
		OW_LOG_DEBUG(logger, Format("Caught exception of type %1 with message %2", e.type(), e.what()));
		threwException = true;
	}
	return !threwException && !processStatus;
}


/**
 * copy the mof file 'fname' into the mof directory.
 *
 * If targetDir is unspecified, the default is to use the mof dir in
 * the commandExecutionEnvironment.
 *
 * @param fname
 *   The absolute file name of the mof file.
 *
 * @param targetDir
 *   The directory the file will be copied into.
 */
bool copyMofFile(String const& fname, LoggerRef const& logger, String targetDir = "")
{
	if (targetDir.empty())
	{
		targetDir = commandExecutionEnvironment.mofdir;
	}
	return copyFile(fname, targetDir, logger);
}

bool shutdownCimom(LoggerRef const& logger)
{
	/*
	How should I shut down the cimom?
	(a) Finesse: Try to find its 'stop' script and run the script.

	(b) Brutality: Find the process owcimomd, and send it a succession
	    of kill signals.
	*/
	//FIXME: make this work for non-unix.
	//also, refactor this with wrapSillyExceptionThrowingExec().
	//FIXME: make this function do nothing if the cimom is not running.
	StringArray command;
	command.push_back(commandExecutionEnvironment.startCimomScript);
	command.push_back("stop");
	String output;
	int processStatus;
	int timeoutSeconds = 100;
	int outputLimit = -1;
	bool threwException;
	try
	{
		Exec::executeProcessAndGatherOutput(command, output, processStatus, timeoutSeconds, outputLimit);
		threwException = false;
	}
	catch(Exception const& e)
	{
		OW_LOG_DEBUG(logger, Format("Caught exception of type %1 with message %2", e.type(), e.what()));
		threwException = true;
	}
	//Did it really stop?
	if (sanityCheck::cimomRunning(logger))
	{
		OW_LOG_ERROR(logger, Format("Stopping script with %1 %2 failed!", commandExecutionEnvironment.startCimomScript, "stop"));
		exit(1);
	}
	else
	{
		OW_LOG_DEBUG(logger, Format("Stopping script with %1 %2 succeeded.", commandExecutionEnvironment.startCimomScript, "stop"));
	}
	return !threwException && !processStatus;
}

bool restartCimom(LoggerRef const& logger)
{
	/**
	 * How should the cimom be restarted?
	 *
	 * (0) Use the script.
	 *
	 * (1) hm, dunno.
	 */
	//FIXME: make this work for non-unix.
	//also, refactor this with wrapSillyExceptionThrowingExec().
	StringArray command;
	command.push_back(commandExecutionEnvironment.startCimomScript);
	command.push_back("start");
	String output;
	int processStatus;
	int timeoutSeconds = 100;
	int outputLimit = -1;
	bool threwException;
	try
	{
		Exec::executeProcessAndGatherOutput(command, output, processStatus, timeoutSeconds, outputLimit);
		threwException = false;
	}
	catch(Exception const& e)
	{
		OW_LOG_DEBUG(logger, Format("Caught exception of type %1 with message %2", e.type(), e.what()));
		threwException = true;
	}
	//Did it really start?
	if (!sanityCheck::cimomRunning(logger))
	{
		OW_LOG_ERROR(logger, Format("Starting script with %1 %2 failed!", commandExecutionEnvironment.startCimomScript, "start"));
	}
	else
	{
		OW_LOG_DEBUG(logger, Format("Starting script with %1 %2 succeeded.", commandExecutionEnvironment.startCimomScript, "start"));
	}
	return !threwException && !processStatus;
}

struct TemporaryCimomShutdown
{
	TemporaryCimomShutdown()
		:m_cimomWasRunning(sanityCheck::cimomRunning(getLogger()))
	{
		if (m_cimomWasRunning)
		{
			shutdownCimom(getLogger());
		}
	}

	~TemporaryCimomShutdown()
	{
		if (m_cimomWasRunning)
		{
			restartCimom(getLogger());
		}
	}

	bool m_cimomWasRunning;
};

bool compileDirectMof(String const& fname, LoggerRef const& logger)
{
	//FIXME: make this work for non-unix.
	//also, refactor this with wrapSillyExceptionThrowingExec().
	StringArray command;
	command.push_back(commandExecutionEnvironment.owExecPrefix + "/" + "owmofc");
	if (!FileSystem::executable(command[0]))
	{
		OW_LOG_ERROR(logger, Format("%1 is not executable!", command[0]));
		return false;
	}
	command.push_back(commandExecutionEnvironment.owmofcOptions);
	command.push_back("-d");
	command.push_back(commandExecutionEnvironment.repositoryDir);
	command.push_back(fname);
	String output;
	int processStatus;
	int timeoutSeconds = 20;
	int outputLimit = -1;
	bool threwException = false;
	OW_LOG_DEBUG(logger, Format("Running owmofc: %1 %2 %3 %4 %5", command[0], command[1], command[2], command[3], command[4]));
	try
	{
		Exec::executeProcessAndGatherOutput(command, output, processStatus, timeoutSeconds, outputLimit);
		threwException = false;
	}
	catch(Exception const& e)
	{
		OW_LOG_DEBUG(logger, Format("Caught exception of type %1 with message %2", e.type(), e.what()));
		threwException = true;
	}
	OW_LOG_DEBUG(logger, Format("output has length %1.", output.length()));
	OW_LOG_DEBUG(logger
		, Format("Got processStatus: %1 threwException %2 output: %3"
			, processStatus, threwException, output));
	return !threwException && !processStatus;
}

/**
 * Backup the mof directory and the repository.
 *
 * @return true if the backup succeeded.
 */
bool backupMofAndRepository(LoggerRef const& logger)
{
	//FIXME: check for errors and stuff.
	bool ok = true;
	OW_LOG_DEBUG(logger, Format("Backing up %1 to %2"
			, commandExecutionEnvironment.mofdir, commandExecutionEnvironment.mofdirBackupLocation));
	ok&= copyDir(commandExecutionEnvironment.mofdir, commandExecutionEnvironment.mofdirBackupLocation, logger);
	OW_LOG_DEBUG(logger, Format("Backing up %1 to %2"
			, commandExecutionEnvironment.mofdir, commandExecutionEnvironment.mofdirBackupLocation));
	ok&= copyDir(commandExecutionEnvironment.repositoryDir, commandExecutionEnvironment.repositoryBackupLocation, logger);
	return ok;
}


/**
 * Restore the repository and the mof dir from backups.
 *
 * @return true if the restoration succeeded.
 */
bool restoreMofAndRepositoryFromBackup(LoggerRef const& logger)
{
	//FIXME: check for errors and stuff.
	bool ok = true;
	OW_LOG_DEBUG(logger, Format("Restoring %1 from %2"
			, commandExecutionEnvironment.mofdir, commandExecutionEnvironment.mofdirBackupLocation));
	ok&= copyDir(commandExecutionEnvironment.mofdirBackupLocation, commandExecutionEnvironment.mofdir, logger);
	OW_LOG_DEBUG(logger, Format("Restoring %1 from %2"
			, commandExecutionEnvironment.mofdir, commandExecutionEnvironment.mofdirBackupLocation));
	ok&= copyDir(commandExecutionEnvironment.repositoryBackupLocation, commandExecutionEnvironment.repositoryDir, logger);
	return ok;
}

bool createMofDir(String const& mofdir, LoggerRef const& logger)
{
	bool ok = true;
	String dnameParent = mofdir.substring(0, mofdir.lastIndexOf("/"));
	if (!sanityCheck::isRWDir(dnameParent, logger))
	{
		//FIXME: This call to canCreateDir() makes this function really expensive.
		if (sanityCheck::canCreateDir(dnameParent, logger))
		{
			ok&= createMofDir(dnameParent, logger);
			if (!ok)
			{
				OW_LOG_ERROR(logger, Format("Failed to create %1 .", dnameParent));
				return false;
			}
		}
		else
		{
			OW_LOG_ERROR(logger, Format("Parent dir %1 not creatable.", dnameParent));
			return false;
		}
	}
	ok&= FileSystem::makeDirectory(mofdir);
	if (!ok)
	{
		//FIXME: do something about platforms that don't have std::strerror.
		OW_LOG_ERROR(logger, Format("Could not create %1 , errno %2 .", mofdir, std::strerror(errno)));
	}
	return ok;
}

/**
 * Add the mof files specified in the StringArray parameters to the
 * currently configured cimom.
 *
 * @param parameters
 *     Each element in this StringArray is the name of a mof
 *     file. add_mof() assumes the files have already been checked for
 *     existance and readabiltiy.
 *
 * FIXME: implement roll-back on failure.
 */
bool addMof(StringArray const& parameters, LoggerRef const& logger)
{
	using namespace FileSystem;
	//FIXME: do something if backups fail.
	backupMofAndRepository(logger);
	if ( commandExecutionEnvironment.createMofDir
		&& !exists(commandExecutionEnvironment.mofdir)
		&& sanityCheck::canCreateDir(commandExecutionEnvironment.mofdir, logger)
	)
	{
		createMofDir(commandExecutionEnvironment.mofdir, logger);
	}
	bool ok = true;
	for (StringArray::const_iterator current = parameters.begin() ; current != parameters.end() ; ++current)
	{
		//Make sure the file exists and is readable.
		if (canRead(*current)&&exists(*current))
		{
			OW_LOG_DEBUG(logger, Format("Compiling %1 .", *current));
			ok&= compileDirectMof(*current, logger);
			OW_LOG_DEBUG(logger, Format("Done compiling %1 .", *current));
			OW_LOG_DEBUG(logger, Format("Copying %1 .", *current));
			ok&= copyMofFile(*current, logger);
			OW_LOG_DEBUG(logger, Format("Done copying %1 .", *current));
		}
		else
		{
			OW_LOG_ERROR(logger, Format("%1 does not exist or is not readable.", *current));
			ok&= false;
		}
	}
	if (!ok)
	{
		restoreMofAndRepositoryFromBackup(logger);
	}
	return ok;
}

bool deleteMofInFileFromRepositry(String const& fname, LoggerRef const& logger)
{
	bool ok = true;
	//FIXME:
	//(0) does the mof compiler provide a way to remove mof?
	std::abort();
	return ok;
}

bool removeMofFile(String const& fname, LoggerRef const& logger)
{
	using namespace FileSystem;
	bool ok = true;
	if (exists(fname))
	{
		OW_LOG_DEBUG(logger, Format("Removing %1.",fname));
		ok&= removeFile(fname);
		if (ok && !exists(fname))
		{
			OW_LOG_DEBUG(logger, Format("%1 successfully removed.", fname));
		}
		else
		{
			OW_LOG_ERROR(logger, Format("Could not remove %1, errno: %2", fname, std::strerror(errno)));
		}
	}
	else
	{
		OW_LOG_ERROR(logger, Format("%1 does not exist!", fname));
		ok = false;
	}
	return ok;
}

bool removeMof(StringArray const& parameters, LoggerRef const& logger)
{
	using namespace FileSystem;
	//FIXME: do something if backups fail.
	backupMofAndRepository(logger);
	bool ok = true;
	for (StringArray::const_iterator current = parameters.begin() ; current != parameters.end() ; ++current)
	{
		//Make sure the file exists.
		//FIXME: Make sure the parent dir is writable.
		if (exists(*current))
		{
			OW_LOG_DEBUG(logger, Format("Removing mof in %1 from repository.", *current));
			ok&= deleteMofInFileFromRepositry(*current, logger);
			if (ok)
			{
				OW_LOG_DEBUG(logger, Format("Successfully removed mof in %1 from repository.", *current));
			}
			else
			{
				OW_LOG_ERROR(logger, Format("Failed in removal of mof in %1 from repository!", *current));
				break;
			}
			OW_LOG_DEBUG(logger, Format("Removing mof file %1 from mofdir.", *current));
			ok&= removeMofFile(*current, logger);
			if (ok)
			{
				OW_LOG_DEBUG(logger, Format("Successfully removed mof file %1 from mof dir.", *current));
			}
			else
			{
				OW_LOG_ERROR(logger, Format("Failed in removal of mof file %1 from mofdir!", *current));
				break;
			}
		}
		else
		{
			OW_LOG_ERROR(logger, Format("%1 does not exist.", *current));
			ok&= false;
		}
	}
	if (!ok)
	{
		restoreMofAndRepositoryFromBackup(logger);
	}
	return ok;
}

void setMofDirBackupLocation(StringArray const& parameters, LoggerRef const& logger)
{
	OW_LOG_DEBUG(logger, Format("Changing commandExecutionEnvironment.mofdirBackupLocation from %1 to %2 ."
		, commandExecutionEnvironment.mofdirBackupLocation, parameters[0]));
	commandExecutionEnvironment.mofdirBackupLocation = parameters[0];
}

void setRepositoryBackupLocation(StringArray const& parameters, LoggerRef const& logger)
{
	OW_LOG_DEBUG(logger, Format("Changing commandExecutionEnvironment.repositoryBackupLocation from %1 to %2 ."
		, commandExecutionEnvironment.repositoryBackupLocation, parameters[0]));
	commandExecutionEnvironment.repositoryBackupLocation = parameters[0];
}

void setMofDir(StringArray const& parameters, LoggerRef const& logger)
{
	OW_LOG_DEBUG(logger, Format("Changing commandExecutionEnvironment.mofdir from %1 to %2"
			, commandExecutionEnvironment.mofdir, parameters[0]));
	commandExecutionEnvironment.mofdir = parameters[0];
}

void setCreateMofDir(StringArray const& parameters, LoggerRef const& logger)
{
	OW_LOG_DEBUG(logger, Format("Changing commandExecutionEnvironment.createMofDir from %1 to %2 ."
			, commandExecutionEnvironment.createMofDir, parameters[0].toBool()));
	commandExecutionEnvironment.createMofDir = parameters[0].toBool();
}

void setCimomRepository(StringArray const& parameters, LoggerRef const& logger)
{
	OW_LOG_DEBUG(logger, Format("Changing commandExecutionEnvironment.repositoryDir from %1 to %2"
			, commandExecutionEnvironment.repositoryDir, parameters[0]));
	commandExecutionEnvironment.repositoryDir = parameters[0];
}

void setTestOnlyFlag(StringArray const& parameters, LoggerRef const& logger)
{
	OW_LOG_DEBUG(logger, Format("Changing commandExecutionEnvironment.testOnly from %1 to %2"
			, commandExecutionEnvironment.testOnly, parameters[0].toBool()));
	commandExecutionEnvironment.testOnly = parameters[0].toBool();
}

void setCheckConsistency(StringArray const& parameters, LoggerRef const& logger)
{
	OW_LOG_DEBUG(logger, Format("Changing commandExecutionEnvironment.checkConsistency from %1 to %2"
			, commandExecutionEnvironment.checkConsistency, parameters[0].toBool()));
	commandExecutionEnvironment.checkConsistency = parameters[0].toBool();
}

bool stompRepository(StringArray const& /* ignored */, LoggerRef const& logger)
{
	OW_LOG_DEBUG(logger, Format("Stomping repository %1 contents with contents of mof dir %2."
			, commandExecutionEnvironment.repositoryDir, commandExecutionEnvironment.mofdir));
	bool ok = true;
	OW_LOG_ERROR(logger, "Not implemented!");
	abort();
	return ok;
}

bool stompMofDir(StringArray const& /* ignored */, LoggerRef const& logger)
{
	OW_LOG_DEBUG(logger, Format("Stomping mof dir %1 contents with contents of repository %2."
			, commandExecutionEnvironment.mofdir, commandExecutionEnvironment.repositoryDir));
	bool ok = true;
	OW_LOG_ERROR(logger, "Not implemented!");
	abort();
	return ok;
}

bool applyCommand(Command const& command, LoggerRef const& logger)
{
	//FIXME: check for errors and stuff.
	using namespace ::CommandSyms;
	switch(command.commandSym)
	{
		case E_display_version:
			{
				displayVersion(command.parameters, logger);
			} break;

		case E_set_verbosity:
			{
				setVerbosity(command.parameters, logger);
			} break;

		case E_display_help:
			{
				displayHelp(command.parameters, logger);
			} break;

		case E_owmofc_options:
			{
				commandExecutionEnvironment.owmofcOptions = command.parameters[0];
				OW_LOG_DEBUG(logger, Format("Using owmofcOptions %1", commandExecutionEnvironment.owmofcOptions));
			} break;

		//Add specified list of mof files to current openwbem install.
		case E_add_mof:
			{
				addMof(command.parameters, logger);
			} break;

		//Remove specified list of mof files to current openwbem install.
		case E_remove_mof:
			{
				removeMof(command.parameters, logger);
			} break;

		case E_set_config:
			{
				setConfig(command.parameters, logger);
			} break;

		//'mof dir' is the directory which contains the mof files, and
		//  which is kept in sync with the cimom repository.
		case E_create_mof_dir:
			{
				setCreateMofDir(command.parameters, logger);
			} break;

		case E_set_mof_dir:
			{
				setMofDir(command.parameters, logger);
			} break;

		case E_set_cimom_repository:
			{
				setCimomRepository(command.parameters, logger);
			} break;


		case E_set_mof_dir_backup_location:
			{
				setMofDirBackupLocation(command.parameters, logger);
			} break;

		case E_set_repository_backup_location:
			{
				setRepositoryBackupLocation(command.parameters, logger);
			} break;

		//Don't commit any modifying operations; just print any messages
		//  which would generated.
		case E_set_test_only_flag:
			{
				setTestOnlyFlag(command.parameters, logger);
			} break;


		//If the repository and the mof directory are inconsistent, print
		//any error messages.
		case E_check_consistency:
			{
				setCheckConsistency(command.parameters, logger);
			} break;


		//Replace the mof in the repository with what is in the directory.
		case E_stomp_repository:
			{
				stompRepository(command.parameters, logger);
			} break;


		//Replace the mof in the directory with what is in the repository.
		case E_stomp_mofdir:
			{
				stompMofDir(command.parameters, logger);
			} break;
		default:
			OW_ASSERTMSG(0, "Bogus option id.");
	}
	//FIXME.
	return true;
}

bool applyCommands(CommandList const& commands, LoggerRef const& logger)
{
	bool allGood = true;
	for (
		CommandList::const_iterator current = commands.begin()
		; current != commands.end()
		; ++current
	)
	{
		allGood&= (sanityCheckCommand(*current, logger) && applyCommand(*current, logger));
	}
	return allGood;
}

int main(int argc, char** argv)
{
	try
	{
		CommandList commands;
		if (discoverCommands(argc, argv, commands, getLogger())
			&& applyCommands(commands, getLogger()))
		{
			return 0;
		}
		else{ return 1;}
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

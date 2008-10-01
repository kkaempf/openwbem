
/*******************************************************************************
* Copyright (C) 2008 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
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

#include "OW_config.h"

#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"

#include "OW_ConfigFile.hpp"
#include "OW_ConfigException.hpp"

#include "blocxx/Exception.hpp"
#include "blocxx/Format.hpp"

#include "FileSystemMockObjectScope.hpp"

#include <map>

using namespace OpenWBEM;

namespace BLOCXX_NAMESPACE
{
	// Used to make the tests easier.
	std::ostream& operator<<(std::ostream& o, const StringArray& arr)
	{
		o << "{ ";

		if( !arr.empty() )
		{
			StringArray::const_iterator iter = arr.begin();

			o << "\"" << *iter << "\"";

			for(++iter; iter != arr.end(); ++iter)
			{
				o << ", \"" << *iter << "\"";
			}
		}

		o << " }";

		return o;
	}
}

namespace
{
	typedef std::map<String, String> FSEntryMap;

	class SimpleFileSystem : public blocxx::FileSystemMockObject
	{
	public:
		SimpleFileSystem(const String& filename, const String& contents)
			: blocxx::FileSystemMockObject()
			, m_files()
		{
			addFile(filename, contents);
		}

		virtual String getFileContents(const String& filename)
		{
			FSEntryMap::const_iterator iter = m_files.find(filename);

			if( iter != m_files.end() )
			{
				return iter->second;
			}

			BLOCXX_THROW(blocxx::FileSystemException, Format("No such file: %1", filename).c_str());
		}

		void addFile(const String& filename, const String& contents)
		{
			m_files[filename] = contents;
		}

	private:
		FSEntryMap m_files;
	};
}

AUTO_UNIT_TEST(ConfigFileTest)
{
	String configFileName = "/etc/someconfig.conf";

	blocxx::FileSystemMockObjectScope m_mockfiles(FSMockObjectRef(new SimpleFileSystem(
				configFileName,
				String(
					"; comment1 = yes, I am a comment\n"
					"# comment2 = I am also a comment\n"
					"foo=bar\n"
					"   quux   =   flump\n"
					"noiz =                  \n"))));


	ConfigFile::ConfigMap config;

	unitAssertNoThrow(ConfigFile::loadConfigFile(configFileName, config));

	// Entries with various whitespace
	unitAssertEquals("bar", ConfigFile::getConfigItem(config, "foo"));
	unitAssertEquals("flump", ConfigFile::getConfigItem(config, "quux"));

	// An empty config value
	unitAssertEquals("", ConfigFile::getConfigItem(config, "noiz", "bad default"));

	// Requesting a value that isn't there.
	unitAssertEquals("bad default", ConfigFile::getConfigItem(config, "no such item", "bad default"));
	unitAssertEquals("bad default", ConfigFile::getConfigItem(config, "comment1", "bad default"));
	unitAssertEquals("bad default", ConfigFile::getConfigItem(config, "comment2", "bad default"));

	// Config files are case sensitive
	unitAssertEquals("bad default", ConfigFile::getConfigItem(config, "BAR", "bad default"));
	unitAssertEquals("bad default", ConfigFile::getConfigItem(config, "Bar", "bad default"));
}

AUTO_UNIT_TEST(MultiConfigValueTest)
{
	String configFileName = "/etc/someconfig.conf";

	blocxx::FileSystemMockObjectScope m_mockfiles(FSMockObjectRef(new SimpleFileSystem(
				configFileName,
				String(
					"foo = 1\n"
					"foo = 2\n"
					"foo = 3 \n"
					"foo = 4"))));



	ConfigFile::ConfigMap config;

	unitAssertNoThrow(ConfigFile::loadConfigFile(configFileName, config));

	// Get the last one...
	unitAssertEquals("4", ConfigFile::getConfigItem(config, "foo"));

	// Get all of them.
	unitAssertEquals(String("1,2,3,4").tokenize(","), ConfigFile::getMultiConfigItem(config, "foo"));
}

AUTO_UNIT_TEST(BadConfigFileTest)
{
	const String configFileName = "/etc/someconfig.conf";

	// No such file.
	{
		blocxx::FileSystemMockObjectScope m_mockfiles(FSMockObjectRef(new SimpleFileSystem(
					configFileName, String())));

		ConfigFile::ConfigMap config;

		unitAssertThrowsEx(ConfigFile::loadConfigFile(configFileName + ".does.not.exist", config), ConfigException);
	}

	// No equals
	{
		blocxx::FileSystemMockObjectScope m_mockfiles(FSMockObjectRef(new SimpleFileSystem(
					configFileName,
					String(
						"I have no equals\n"
						"i = should not parse this far"))));

		ConfigFile::ConfigMap config;

		unitAssertThrowsEx(ConfigFile::loadConfigFile(configFileName, config), ConfigException);

		unitAssertEquals("bad default", ConfigFile::getConfigItem(config, "i", "bad default"));
	}

	// No config item name
	{
		blocxx::FileSystemMockObjectScope m_mockfiles(FSMockObjectRef(new SimpleFileSystem(
					configFileName,
					String(
						"= bar\n"
						"i = should not parse this far"))));

		ConfigFile::ConfigMap config;

		unitAssertThrowsEx(ConfigFile::loadConfigFile(configFileName, config), ConfigException);

		unitAssertEquals("bad default", ConfigFile::getConfigItem(config, "i", "bad default"));
	}
}

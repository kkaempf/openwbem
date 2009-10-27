#include "OW_config.h"
#include "blocxx/FileSystem.hpp"
#include "blocxx/String.hpp"
#include "blocxx/Format.hpp"
#include <iostream>

using namespace blocxx;

bool check_security(const String& pathname)
{
	try
	{
		std::pair<FileSystem::Path::ESecurity, String> results = FileSystem::Path::security(pathname);

		if( results.first == FileSystem::Path::E_SECURE_DIR )
		{
			std::cout << "Secure Directory: " << results.second << std::endl;
			return true;
		}
		else if( results.first == FileSystem::Path::E_SECURE_FILE )
		{
			std::cout << "Secure File: " << results.second << std::endl;
			return true;
		}
		else
		{
			std::cerr << Format("Insecure (%1): %2", results.first, results.second) << std::endl;
		}
	}
	catch(const Exception& e)
	{
		std::cerr << "Caught exception: " << e << std::endl;
	}
	return false;
}

// Exit codes:
// 1 -- Insecure
// 2 -- Does not exist
// 3 -- Does not exist, could be made secure.

int main(int argc, const char** argv)
{
	if( argc == 2 )
	{
		String path = argv[1];

		if( !FileSystem::exists(path) )
		{
			if( !check_security(FileSystem::Path::dirname(path)) )
			{
				std::cerr << "Path does not exist: " << path << std::endl;
				return 2;
			}
			else
			{
				std::cout << "Path *could* be secure (if it existed): " << path << std::endl;
				return 3;
			}
		}
		else if( check_security(path) )
		{
			return 0;
		}
	}
	else
	{
		std::cerr << "Expected exactly one argument." << std::endl;
	}
	return 1;
}

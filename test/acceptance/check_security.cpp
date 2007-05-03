#include "OW_config.h"
#include "OW_FileSystem.hpp"
#include "OW_String.hpp"
#include "OW_Format.hpp"
#include <iostream>


namespace OW = OW_NAMESPACE;

bool check_security(const OW::String& pathname)
{
	try
	{
		std::pair<OW::FileSystem::Path::ESecurity, OW::String> results = OW::FileSystem::Path::security(pathname);

		if( results.first == OW::FileSystem::Path::E_SECURE_DIR )
		{
			std::cout << "Secure Directory: " << results.second << std::endl;
			return true;
		}
		else if( results.first == OW::FileSystem::Path::E_SECURE_FILE )
		{
			std::cout << "Secure File: " << results.second << std::endl;
			return true;
		}
		else
		{
			std::cerr << OW::Format("Insecure (%1): %2", results.first, results.second) << std::endl;
		}
	}
	catch(const OW::Exception& e)
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
		OW::String path = argv[1];

		if( !OW::FileSystem::exists(path) )
		{
			if( !check_security(OW::FileSystem::Path::dirname(path)) )
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

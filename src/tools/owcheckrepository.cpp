#include "OW_CIMRepository.hpp"
#undef NDEBUG
#include <cassert>
#include <iostream>

using namespace OpenWBEM;
using namespace std;

int main(int argc, char * * argv)
{
	if (argc != 2)
	{
		clog << "Usage: owcheckrepository <repository dir>\n";
		return 1;
	}
	char const * const repos_dir = argv[1];
	CIMRepository repos;
	try
	{
		repos.open(repos_dir);
		cout << hex << repos.checkFreeLists() << endl;
	}
	catch (Exception& e)
	{
		clog << "Failed: " << e << endl;
		return 1;
	}
	return 0;
}

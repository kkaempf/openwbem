#include "OW_CIMRepository.hpp"
#undef NDEBUG
#include <cassert>
#include <iostream>

using namespace OpenWBEM;
using namespace std;

int main(int argc, char * * argv)
{
	assert(argc == 2);
	char const * const repos_dir = argv[1];
	CIMRepository repos;
	repos.open(repos_dir);
	cout << hex << repos.checkFreeLists() << endl;
	return 0;
}



/**
 * @author Dan Nuffer
 */

#include "blocxx/UTF8Utils.hpp"
#include "blocxx/String.hpp"
#include <iostream>

using namespace OpenWBEM;
using namespace std;

int main()
{
	try
	{
		String in = String::getLine(cin);
		String utf8 = UTF8Utils::UCS4toUTF8(in.toUInt32(16));
		for (int i = 0; i < utf8.length(); ++i)
		{
			std::cout << "0x" << hex << UInt32(UInt8(utf8[i])) << ' ';
		}
	}
	catch (Exception& e)
	{
		cerr << e << endl;
		return 1;
	}
	return 0;
}


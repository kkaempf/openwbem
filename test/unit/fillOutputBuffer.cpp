#include <iostream>
#include <ostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>

// This program exists to dump output to a buffered stream (stdout).  The
// intention is to use this in testing the proper termination of an executed
// process if that process becomes blocked on output.  This would happen if the
// parent process doesn't start reading from its end of the pipe (eg. it is
// waiting for the child process to do the close-on-exec of its status pipe).

int main(int argc, char** argv)
{
	if( argc > 1 )
	{
		int bytes_to_write = atoi(argv[1]);
		std::vector<char> buffer(16384);
		for( size_t i = 0; i < buffer.size(); ++i )
		{
			buffer[i] = char((i % 26) + 'A');
		}

		while( std::cout && (bytes_to_write > 0) )
		{
			size_t count = std::min<size_t>(bytes_to_write, buffer.size());
			std::cout.write(&buffer[0], count);
			bytes_to_write -= count;
		}

		if( !std::cout )
		{
			return 3;
		}
		std::cout.flush();
		return 0;
	}
	return 1;
}

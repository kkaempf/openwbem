
#include "OW_config.h"

#include <iostream>
#include <stdlib.h>

using std::cerr;
using std::endl;

/*
OW_UNDERRUN=1
OW_OVERRUN=2
OW_UNKNOWN_ADDR=3
OW_DOUBLE_DELETE=4
OW_DOUBLE_DELETE_NOFREE=5
*/

void Usage(const char* cmd)
{
	cerr << 
		"Usage: " << cmd << " <mode>\n"
		"	1 = under run\n"
		"	2 = over run\n"
		"	3 = unknown address\n"
		"	4 = double delete\n"
		"	7 = aggressive" << endl;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		Usage(argv[0]);
		exit(9);
	}

	int mode = atoi(argv[1]);

	char* buf = NULL;
	char* buf2 = NULL;

	switch (mode)
	{
		case 1: // under run
			buf = new char[10];
			buf--;
			(*buf) = 's';
			buf++;
			delete buf;
			break;
		case 2: // over run
			buf = new char[10];
			buf[10] = 's';
			delete buf;
			break;
		case 4: // double delete
			buf = new char[10];
			delete buf; 
			delete buf;
			break;
		case 3: // unknown address
			buf = new char[10];
			buf += 5;
			delete buf; 
			break;
		case 7:
			buf = new char[10];
			buf[10] = 's';
			buf2 = new char[10];
			(void)buf2;
			break;
		default:
			Usage(argv[0]);
			exit(9);
			break;
	}

	exit(0);
}



#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <string.h>
#include <io.h>

#include <windows.h>
#include <dbghelp.h>
/*
#include <imagehlp.h>
#pragma hdrstop
#pragma comment( lib, "imagehlp.lib" )
*/

using namespace std;

namespace {

///////////////////////////////////////////////////////////////////////////////
string
left(const string& sarg, int count)
{
	return sarg.substr(0, count);
}

///////////////////////////////////////////////////////////////////////////////
string
right(const string& sarg, int count)
{
	if (count >= sarg.length())
	{
		return string(sarg);
	}

	return sarg.substr(sarg.length()-count);
}

///////////////////////////////////////////////////////////////////////////////
bool
startsWith(const string& arg1, const string& arg2)
{
	return (arg1.find(arg2) == 0);
}

///////////////////////////////////////////////////////////////////////////////
bool
endsWith(const string& arg1, const string arg2)
{
	bool cc = false;
	if (arg1.length() >= arg2.length())
	{
		int ndx = arg1.find(arg2);
		if (ndx != string::npos)
		{
			cc = ((arg1.length() - ndx) == arg2.length());
		}
	}
	return cc;
}


///////////////////////////////////////////////////////////////////////////////
char undecoratedNameBuf[4096];
char wkbfr[4096];

///////////////////////////////////////////////////////////////////////////////
int convertFile(const string& filename, const string& outfilename)
{
	ifstream inputFile(filename.c_str());

	if (!inputFile)
	{
		cerr << "Can't open " << filename << endl;
		return -1;
	}

	unsigned long ordinalNumber = 10;

	string beginning("??_EC");

    //string delDTOREnding("@@UAEPAXI@Z");	// end of a scalar/vector deleting dtor
    string delDTOREnding("AEPAXI@Z");	// end of a scalar/vector deleting dtor
    string scalarDelDTORStart("??_G");	// Beginning of a scalar deleting dtor
	string vectorDelDTORStart("??_E");	// Beginning of a vector deleting dtor

    string staticSymbols(" Static symbols");
    string scalarDeletingDestructor("??_G");
	string vectorDeletingDestructor("??_E");
    string newOperator("??2@YAPAXI@Z");
	string wk;

	string stringFromFile;
    string timeString;

	unsigned long baseMemoryAddress = 0;

	time_t curTime = ::time(NULL);
	timeString = "; Created by Map2Def on ";
	timeString += ::ctime(&curTime);

	// Now we have the filename without an extension
	//int ndx = outfilename.find('.');
	int ndx = outfilename.find_last_of('.');
	string libraryName = (ndx != string::npos) ? outfilename.substr(0, ndx) : outfilename;
	ndx = libraryName.find_last_of('\\');
	if (ndx != string::npos)
	{
		libraryName = libraryName.substr(ndx+1);
	}

	string libraryLine = "LIBRARY " + libraryName;

	baseMemoryAddress = (::rand() % 32767);

	//
	// If we're using Map2Def, we are most likely creating a DLL.
	// Visual C++ uses a fixed memory location for the DLLs it creates.
	// This results in memory address space conflicts and the loader
	// has to relocate your DLL. To fix that problem, we create a random
	// memory location for the DLL to load. This reduces the chances
	// of the loader having to relocate your DLL (therefore the EXE
	// loads faster).
	//

	sprintf(wkbfr, "  BASE=0x%04lX0000\n\n", baseMemoryAddress);
	libraryLine += wkbfr;

	ofstream defFile(outfilename.c_str());
	if (!defFile)
	{
		cerr << "Can't open output file " << outfilename << endl;
		inputFile.close();
		return -1;
	}

	cout << "Creating " << outfilename << endl;

	defFile << timeString;
	defFile << libraryLine;
	defFile << "EXPORTS\n" << endl;

	while (getline(inputFile, stringFromFile))
	{
		if (stringFromFile.length() <= 22)
		{
			if ( ( stringFromFile.length() >= staticSymbols.length() ) &&
				(staticSymbols.compare(left(stringFromFile, staticSymbols.length())) == 0))
			{
				break;
			}
			continue;
		}

		if (stringFromFile[5] != ':')
			continue;

		if (stringFromFile[21] != '?' && stringFromFile[21] != '_')
			continue;

		// We may have one here...
		stringFromFile = right(stringFromFile, stringFromFile.length()-21);
		int locationOfSpace = stringFromFile.find(' ');
		
		if (locationOfSpace == string::npos)
			continue;

		string outputLine;
		string decoratedName = left(stringFromFile, locationOfSpace);
		stringFromFile = right(stringFromFile, stringFromFile.length() - locationOfSpace);

		// Now see if this is an import
		locationOfSpace = stringFromFile.find(':');
		if (locationOfSpace != string::npos)
			continue;

		// Didn't find a colon, let's do one last check for <common>
		locationOfSpace = stringFromFile.find('<');
		if (locationOfSpace != string::npos)
			continue;

		// If this is a scalar/vector deleting dtor, skip it
		if (endsWith(decoratedName, delDTOREnding) 
			&& (startsWith(decoratedName, scalarDelDTORStart)
				|| startsWith(decoratedName, vectorDelDTORStart)))
		{
			continue;
		}

		// If this is a special name, skip it
		if (_strcmpi(decoratedName.c_str(), "__pRawDllMain") == 0
			|| _strcmpi(decoratedName.c_str(), "__afxForceEXTDLL") == 0
			|| _strcmpi(decoratedName.c_str(), newOperator.c_str()) == 0)
		{
			continue;
		}

		// Identify Data vs. function
		// This seems like a major hack, but in haste I'm going to give
		// it a try.
		const char *dataTag = "";
		ndx = decoratedName.rfind("@@");
		if(ndx != string::npos)
		{
			wk = decoratedName.substr(ndx);
			if(wk.length() > 2)
			{
				wk = wk.substr(2);
				if(wk[0] == '2')
				{
					ndx = wk.find('@');
					if(ndx == string::npos)
					{
						dataTag = "   DATA";
					}
				}
			}
		}

		sprintf(wkbfr, " %s @%lu NONAME%s",
			decoratedName.c_str(), ordinalNumber, dataTag);
		outputLine = wkbfr;
		memset(undecoratedNameBuf, 0, sizeof(undecoratedNameBuf));

		// It is unclear if the third parameter should be the size of the buffer or
		// the number of characters undecorated_name can hold. Documentation is shoddy
		if (UnDecorateSymbolName(decoratedName.c_str(), undecoratedNameBuf,
			sizeof(undecoratedNameBuf), UNDNAME_COMPLETE ) > 0)
		{
			outputLine += " ; ";
			outputLine += undecoratedNameBuf;
		}
		outputLine += "\n";
		ordinalNumber++;

		// Don't export symbols from anonymous namespace
		if (outputLine.find("`anonymous namespace'") == string::npos)
		{
			defFile << outputLine;
		}
	}

	defFile.close();
	inputFile.close();
	return 0;
}

}	// end of unnamed namespace

int main(int argc, char** argv)
{
	srand((unsigned) time(NULL));

	if (argc != 3)
	{
		cout << "Usage: owmap2def <.map file name> <.def file name>" << endl;
		return 1;
	}

	if (_access(argv[1], 4) != 0)
	{
		cout << "Map file " << argv[1] << " does not exist. Aborting" << endl;
		return 1;
	}

	return (convertFile(argv[1], argv[2]) != 0) ? 1 : 0;
/*
	if (argc < 2)
	{
		cout << "Usage: Map2Def file [file [file [file [...]]]]" << endl;
		return 1;
	}

	HANDLE findFileHandle = (HANDLE) NULL;
	WIN32_FIND_DATA findData;
	int index = 1;

	while (index < argc)
	{
		memset(&findData, 0, sizeof(findData));

		findFileHandle = FindFirstFile(argv[index], &findData);

		if (findFileHandle != INVALID_HANDLE_VALUE )
		{
			convertFile(findData.cFileName);
			while (FindNextFile(findFileHandle, &findData ) != FALSE )
			{
				convertFile(findData.cFileName);
			}

			FindClose(findFileHandle);
		}
		index++;
	}
*/
	return( EXIT_SUCCESS );
}

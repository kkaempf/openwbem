all : OW_GenCaseFoldingCompare OW_GenCaseFoldingCompareTests

OW_GenCaseFoldingCompare : OW_GenCaseFoldingCompare.cpp
	g++ -o OW_GenCaseFoldingCompare -I.. -L.. -lopenwbem -lssl -lcrypto OW_GenCaseFoldingCompare.cpp

OW_GenCaseFoldingCompareTests : OW_GenCaseFoldingCompareTests.cpp
	g++ -o OW_GenCaseFoldingCompareTests -I.. -L.. -lopenwbem -lssl -lcrypto OW_GenCaseFoldingCompareTests.cpp

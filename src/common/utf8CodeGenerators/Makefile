all : OW_GenCaseFoldingCompare OW_GenCaseFoldingCompareTests OW_GenCaseMappings OW_to_UTF8

% : %.cpp
	g++ -o $@ -g -I.. -I../../cim -I../../ifcs -L.. -lopenwbem -lssl -lcrypto $<


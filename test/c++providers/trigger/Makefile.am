include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libtriggertest1.a libtriggertest2.a


OW_SHARED_LIBS = libtriggertest1.so libtriggertest2.so

all-local: $(OW_SHARED_LIBS)
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done


libtriggertest2_a_SOURCES = \
OW_TriggerProviderTest2.cpp

libtriggertest1_a_SOURCES = \
OW_TriggerProviderTest1.cpp


INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider




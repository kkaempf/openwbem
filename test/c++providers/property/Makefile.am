include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libolver.a
SHAREDLIBADD =


OW_SHARED_LIBS = libolver.so

all-local: $(OW_SHARED_LIBS)
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done

libolver_a_SOURCES = \
OW_OpenLinuxVersion.cpp

EXTRA_DIST = OpenLinuxVersion.mof

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider

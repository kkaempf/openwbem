include $(top_srcdir)/Makefile-shared-lib.inc
include $(top_srcdir)/Makefile-shared-lib-inst.inc

noinst_LIBRARIES = libsimplecppprovifc.a
OW_SHARED_LIBS = libsimplecppprovifc.so
SHAREDLIBADD =

thelibdir = $(pkglibdir)/provifcs

if DISABLE_ASSOCIATION_TRAVERSAL
ASSOCIATION_S=
else
ASSOCIATION_S= \
OW_SimpleCppAssociatorProviderIFC.hpp \
OW_SimpleCppAssociatorProviderIFC.cpp
endif

libsimplecppprovifc_a_SOURCES = \
$(ASSOCIATION_S) \
OW_SimpleCppInstanceProviderIFC.hpp \
OW_SimpleCppInstanceProviderIFC.cpp \
OW_SimpleCppProviderBaseIFC.hpp \
OW_SimpleCppProviderBaseIFC.cpp \
OW_SimpleCppProviderIFC.cpp \
OW_SimpleCppProviderIFC.hpp \
OW_SimpleCppProxyProvider.cpp \
OW_SimpleCppProxyProvider.hpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/server


MAKEDYLIB=1
MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


OW_SHARED_LIBS = libowrepositoryhdb.$(LIB_EXT)

owstatlib_LIBRARIES = \
libowrepositoryhdb.a

SHAREDLIBADD = \
-L$(top_builddir)/src/common/ -lopenwbem \
-L$(top_builddir)/db/db -lowdb

thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

if DISABLE_ASSOCIATION_TRAVERSAL
ASSOCIATION_S=
ASSOCIATION_H=
else
ASSOCIATION_S= \
OW_AssocDb.cpp
ASSOCIATION_H= \
OW_AssocDb.hpp
endif

libowrepositoryhdb_a_SOURCES = \
$(ASSOCIATION_S) \
OW_CIMRepository.cpp \
OW_GenericHDBRepository.cpp \
OW_HDB.cpp \
OW_HDBException.cpp \
OW_HDBNode.cpp \
OW_Index.cpp \
OW_IndexImpl.cpp \
OW_InstanceRepository.cpp \
OW_MetaRepository.cpp

pkginclude_HEADERS = \
$(ASSOCIATION_H) \
OW_CIMRepository.hpp \
OW_GenericHDBRepository.hpp \
OW_HDBCommon.hpp \
OW_HDB.hpp \
OW_HDBNode.hpp \
OW_Index.hpp \
OW_InstanceRepository.hpp \
OW_MetaRepository.hpp


INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/db/include


include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


OW_SHARED_LIBS = libowrepositoryhdb.so

noinst_LIBRARIES = \
libowrepositoryhdb.a

thelibdir = $(libdir)

if DISABLE_ASSOCIATION_TRAVERSAL
ASSOCIATION_S=
else
ASSOCIATION_S= \
OW_AssocDb.cpp \
OW_AssocDb.hpp
endif

libowrepositoryhdb_a_SOURCES = \
$(ASSOCIATION_S) \
OW_CIMRepository.cpp \
OW_CIMRepository.hpp \
OW_GenericHDBRepository.cpp \
OW_GenericHDBRepository.hpp \
OW_HDBCommon.hpp \
OW_HDB.cpp \
OW_HDB.hpp \
OW_HDBNode.cpp \
OW_HDBNode.hpp \
OW_Index.hpp \
OW_Index.cpp \
OW_IndexImpl.cpp \
OW_InstanceRepository.cpp \
OW_InstanceRepository.hpp \
OW_MetaRepository.cpp \
OW_MetaRepository.hpp \
OW_RepositoryStreams.cpp \
OW_RepositoryStreams.hpp

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/db/include


include $(top_srcdir)/Makefile-shared-lib.inc

OW_SHARED_LIBS = libowrepositoryhdb.so

noinst_LIBRARIES = \
libowrepositoryhdb.a

thelibdir = $(libdir)

all-local: $(OW_SHARED_LIBS)

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -sf $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done

libowrepositoryhdb_a_SOURCES = \
OW_AssocDb.cpp \
OW_AssocDb.hpp \
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


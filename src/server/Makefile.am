include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libowserver.a
SHAREDLIBADD = -L$(top_builddir)/src/common -lopenwbem

OW_SHARED_LIBS = libowserver.so
thelibdir = $(libdir)

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs

libowserver_a_SOURCES =	\
OW_AuthManager.cpp \
OW_AuthManager.hpp \
OW_LoggerImpls.cpp \
OW_PidFile.cpp \
OW_PidFile.hpp \
OW_Platform.cpp \
OW_Platform.hpp


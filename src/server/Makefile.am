noinst_LIBRARIES = libplatformserver.a

INCLUDES = -I$(top_srcdir)/src \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/thread \
-I$(top_srcdir)/src/http \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/net/socket \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/cim	\
-I$(top_srcdir)/src/cimom \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/platform/@PLATFORMDIR@/client \
-I$(top_srcdir)/src/platform/@PLATFORMDIR@/common

libplatformserver_a_SOURCES =	\
OW_Logger.cpp \
OW_PidFile.cpp \
OW_PidFile.hpp \
OW_Platform.cpp \
OW_dlSharedLibrary.cpp \
OW_dlSharedLibrary.hpp \
OW_dlSharedLibraryLoader.cpp \
OW_dlSharedLibraryLoader.hpp

#libplatformserver_a_LIBADD = -ldl

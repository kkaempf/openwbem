daemonbin_PROGRAMS = newdaemon
daemonbindir = $(sbindir)
 
newdaemon_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o newdaemon

newdaemon_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/server -lowserver \
-L$(top_builddir)/db/db -lowdb
#remove the server line once things are separated

# OpenServer may need this one day, but Linux doesn't like it
#owcimomd_LDFLAGS = "-Wl,-Bexport"

newdaemon_SOURCES = \
main.cpp \
OW_DaemonEnv.cpp \
OW_DaemonEnv.hpp

INCLUDES = \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/thread \
-I$(top_srcdir)/src/http \
-I$(top_srcdir)/src/http/server \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/net/socket \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/server



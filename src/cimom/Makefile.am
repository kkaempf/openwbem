SUBDIRS = server common indication

# This causes automake to prepend $(PACKAGE_PREFIX_D) to the beginning of owcimomd when it's installed
#transform = s,owcimomd,$(PACKAGE_PREFIX_D)owcimomd,

cimombin_PROGRAMS = owcimomd
cimombindir = $(sbindir)

owcimomd_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owcimomd

owcimomd_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/cimom/server -lowserver \
-L$(top_builddir)/src/cimom/common -lowcimomcommon \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/db/db -lowdb $(SSL_LIBS) $(BINLINKAPPENDFLAGS)

owcimomd_SOURCES = \
owcimomd_main.cpp 

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/cimom/common

#use this rule to create the data dir
install-exec-local: install-static-cimom-main
	${INSTALL} -d ${DESTDIR}/@localstatedir@/openwbem

install-static-cimom-main : $(owcimomd_OBJECTS)
	if test "x$(STATIC_SERVICES)" != "x"; then \
		ar cru libowcimomd_main.a $^; \
		$(INSTALL_PROGRAM) libowcimomd_main.a $(DESTDIR)$(libdir); \
	fi

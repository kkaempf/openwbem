owlocalhelperbin_PROGRAMS = owlocalhelper
owlocalhelperbindir = $(libexecdir)/openwbem

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim

owlocalhelper_SOURCES = \
owlocalhelper_main.cpp

owlocalhelper_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owlocalhelper$(EXEEXT) 

# Do things a bit differently and statically link to libopenwbem to avoid restrictions with LD_LIBRARY_PATH and setuid binaries.
owlocalhelper_LDADD = \
$(top_builddir)/src/common/libopenwbem.a \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)


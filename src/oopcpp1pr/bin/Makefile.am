if ENABLE_CMPI
OWOOPCMPIPR_BIN=owoopcmpipr
else
OWOOPCMPIPR_BIN=
endif

owoopcpp1prbin_PROGRAMS = \
$(OWOOPCMPIPR_BIN) \
owoopcpp1pr

owoopcpp1prbindir = $(libexecdir)/openwbem

owoopcpp1pr_PROJECT_LIBS = \
-L$(top_builddir)/src/oopcpp1pr -lowoopcpp1pr \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/requesthandlers/binary -lowrequesthandlerbinary \
-L$(top_builddir)/src/common -lopenwbem

owoopcpp1pr_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS) $(APPARMOR_LIBS)

owoopcpp1pr_SOURCES = \
owoopcpp1pr_main.cpp

if ENABLE_CMPI
# cmpi

owoopcmpiprbindir = $(libexecdir)/openwbem

owoopcmpipr_PROJECT_LIBS = \
-L$(top_builddir)/src/oopcpp1pr -lowoopcpp1pr \
-L$(top_builddir)/src/providerifcs/cmpi -lcmpiprovifc \
-L$(top_builddir)/src/providerifcs/cmpi/common -lcmpisupport \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/requesthandlers/binary -lowrequesthandlerbinary \
-L$(top_builddir)/src/common -lopenwbem

owoopcmpipr_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS) $(APPARMOR_LIBS)

owoopcmpipr_SOURCES = \
owoopcmpipr_main.cpp

endif

INCLUDES = \
-I$(top_srcdir)/src/providerifcs/cmpi \
-I$(top_srcdir)/src/providerifcs/cmpi/common \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/oopcpp1pr \
-I$(top_srcdir)/src/monitor

if OW_STATIC_LINKING
install-exec-local: install-static-owoopcpp1pr-main install-static-owoopcmpipr-main

MAINLIB1=libowoopcpp1pr_main.a
MAINLIB2=libowoopcmpipr_main.a

$(MAINLIB1): $(owoopcpp1pr_OBJECTS)
	ar cru $@ $^

$(MAINLIB2): $(owoopcmpipr_OBJECTS)
	ar cru $@ $^

install-static-owoopcpp1pr-main : $(MAINLIB1)
	$(INSTALL_PROGRAM) $^ $(DESTDIR)$(libdir)

install-static-owoopcmpipr-main : $(MAINLIB2)
	$(INSTALL_PROGRAM) $^ $(DESTDIR)$(libdir)

clean-local:
	rm -f $(MAINLIB1) $(MAINLIB2)

else

#need this in both sections of the conditional
clean-local:

endif

OW_EXECUTABLES = $(owoopcpp1prbin_PROGRAMS)
include $(top_srcdir)/Makefile-executable.inc

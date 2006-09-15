owoopcpp1prbin_PROGRAMS = owoopcpp1pr
owoopcpp1prbindir = $(libexecdir)/openwbem

owoopcpp1pr_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owoopcpp1pr$(EXEEXT)

owoopcpp1pr_LDADD = \
-L$(top_builddir)/src/oopcpp1pr -lowoopcpp1pr \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/requesthandlers/binary -lowrequesthandlerbinary \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) $(BINLINKAPPENDFLAGS)

owoopcpp1pr_SOURCES = \
owoopcpp1pr_main.cpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/oopcpp1pr \
-I$(top_srcdir)/src/monitor

if OW_STATIC_LINKING
install-exec-local: install-static-owoopcpp1pr-main

MAINLIB=libowoopcpp1pr_main.a

$(MAINLIB): $(owoopcpp1pr_OBJECTS)
	ar cru $@ $^

install-static-owoopcpp1pr-main : $(MAINLIB)
	$(INSTALL_PROGRAM) $^ $(DESTDIR)$(libdir)

clean-local:
	rm -f $(MAINLIB)

else

#need this in both sections of the conditional
clean-local:

endif

bin_PROGRAMS = owmofc

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/http \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/common/socket

owmofc_SOURCES = \
CIMOMVisitor.hpp \
CIMOMVisitor.cpp \
main.cpp

owmofc_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owmofc

owmofc_LDADD = \
-L../common -lopenwbem \
-L. -lowmofc


if DISABLE_SCHEMA_MANIPULATION
DIRS_THAT_NEED_QUAL_DECL=
else
if DISABLE_QUALIFIER_DECLARATION
DIRS_THAT_NEED_QUAL_DECL=
else
DIRS_THAT_NEED_QUAL_DECL= mof
endif
endif


SUBDIRS = \
cim \
ifcs \
common \
 \
client \
 \
cimxmllistener \
 \
http \
 \
provider \
server \
 \
xml \
 \
wql \
authenticators \
services \
requesthandlers \
providerifcs \
providers \
repositories \
cimom \
 \
$(DIRS_THAT_NEED_QUAL_DECL) \
tools


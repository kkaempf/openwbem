if DISABLE_INSTANCE_MANIPULATION
MOFDIR=
else
if DISABLE_SCHEMA_MANIPULATION
MOFDIR=
else
if DISABLE_QUALIFIER_DECLARATION
MOFDIR=
else
MOFDIR= mof
endif
endif
endif

if DISABLE_INSTANCE_MANIPULATION
CIMXMLLISTENERDIR=
else
CIMXMLLISTENERDIR= cimxmllistener
endif

SUBDIRS = \
cim \
ifcs \
common \
 \
client \
 \
$(CIMXMLLISTENERDIR) \
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
$(MOFDIR) \
tools


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
http \
 \
xml \
 \
client \
 \
services \
$(CIMXMLLISTENERDIR) \
 \
 \
provider \
 \
wql \
authenticators \
authorizers \
requesthandlers \
repositories \
providerifcs \
cimom \
providers \
 \
$(MOFDIR) \
tools


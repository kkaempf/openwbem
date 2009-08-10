
SO_EXT = $(LIBRARY_BINARY_VERSION)
SOFILENAME = $@.$(SO_EXT)
# This DYFILENAME definition looks confusing.  It takes the target of the
# Darwin rule, makes sure there is a library version in it (only one), and
# forces the suffix as .dylib.
DYFILENAME = $(patsubst %.$(SO_EXT).$(SO_EXT).dylib,%.$(SO_EXT).dylib,$(sort $(patsubst %.dylib,%,$(patsubst %.dylib.bundle,%.dylib,$@))).$(SO_EXT).dylib)
TMPLIBDIR = .$@.libs


project_libs_var = $($(basename $(@))_PROJECT_LIBS)
external_libs_var = $($(basename $(@))_EXTERNAL_LIBS)

%.so %.sl : %.a
	rm -rf $(TMPLIBDIR)
	mkdir $(TMPLIBDIR)
	cd $(TMPLIBDIR) && \
	$(AR) x ../$<
	$(CXXLINK) $(CXXSHAREDLIBLINKFLAGS) -o $(SOFILENAME) $(TMPLIBDIR)/*.o $(SHAREDLIBADD)
	rm -rf $@ $(TMPLIBDIR) $(@:%.so=%.exp)
	ln -s $(SOFILENAME) $@
	chmod +x $(SOFILENAME)

# Rules for NetWare

#	Look only for symbols in the text "T" section because that is all the 
#	fake library stuff needs to be concerned with.
CREATE_IMPORTS=nm -Bpg $(TMPLIBDIR)/*.o | \
			$(AWK) '{ if( ($$2 == "T") && \
				      (substr($$3,1,1) != ".") && \
				      (substr($$3,1,1) != "_") ) \
				{ print $$3 } }' > $(@:%.nlm=%.imp)

#	Removed the following line
#	$(CXXLINK) $(CXXSHAREDLIBLINKFLAGS) -o $@ $(TMPLIBDIR)/*.o $(SHAREDLIBADD)
#	and changed it to a 'touch' command because we no longer need dynamic
#	libraries.  The 'touch' command speeds up the compile significantly
%.nlm : %.a
	rm -rf $(TMPLIBDIR)
	mkdir $(TMPLIBDIR)
	cd $(TMPLIBDIR) && \
	$(AR) x ../$<
	$(CREATE_IMPORTS)
	touch $@
	cp $@ $(SOFILENAME)
	rm -rf $(TMPLIBDIR)

DO_NOTHING=
VALID_CONDITION=$(1:0=)$(1:1=1)

BUILD_DYLIB=$(if $(call VALID_CONDITION,$(MAKEDYLIB)), \
$(CXXLINK) $(CXXSHAREDLIBLINKFLAGS) -o $(DYFILENAME) $(TMPLIBDIR)/*.o $(SHAREDLIBADD), \
$(DO_NOTHING))
BUILD_BUNDLE=$(if $(call VALID_CONDITION,$(MAKEBUNDLE)), \
$(CXXLINK) $(CXXBUNDLELIBLINKFLAGS) -o $(DYFILENAME).bundle $(TMPLIBDIR)/*.o $(SHAREDLIBADD), \
$(DO_NOTHING))


TOUCH_FILE_IF_NEEDED=$(if $(wildcard $(1)),$(DO_NOTHING),echo "The target for the rule was not created. Touching $(1)"; touch "$(1)")

define TOUCH_UNCREATED_LIBS
	$(call TOUCH_FILE_IF_NEEDED,$(DYFILENAME))
	$(call TOUCH_FILE_IF_NEEDED,$(DYFILENAME).bundle)
endef

# Rules for OSX (Darwin) so we can get the plugable modules built
# at the same time. Requires that either MAKEDYLIB or MAKEBUNDLE
# be inserted into Makefile.am in the appropriate directory.
%.$(SO_EXT).dylib.bundle %.$(SO_EXT).dylib : %.a
	rm -f $(@:%.dylib=%.dylib.bundle) $@ $(DYFILENAME)
	rm -rf $(TMPLIBDIR)
	mkdir $(TMPLIBDIR)
	cd $(TMPLIBDIR) && \
	$(AR) x ../$<
	$(BUILD_DYLIB)
	$(BUILD_BUNDLE)
	$(TOUCH_UNCREATED_LIBS)
	rm -rf $(TMPLIBDIR)
	@echo "$@ done"

%.dylib.bundle %.dylib : %.$(SO_EXT).dylib.bundle %.$(SO_EXT).dylib
	ln -f -v -s $(DYFILENAME) $(DYFILENAME:%.$(SO_EXT).dylib=%.dylib)
	ln -f -v -s $(DYFILENAME).bundle $(DYFILENAME:%.$(SO_EXT).dylib=%.dylib).bundle

# Prevent the generated libraries (and symlinks) from being removed as 'side
# effects'.  This improves recompilation (or installation) time in some cases.
.PRECIOUS: %.$(SO_EXT).dylib.bundle %.dylib.bundle %.$(SO_EXT).dylib %.dylib
.PRECIOUS: %.so.$(SO_EXT) %.so
.PRECIOUS: %.sl.$(SO_EXT) %.sl
.PRECIOUS: %.a

all-ow-shared-libs: $(OW_SHARED_LIBS)

clean-ow-shared-libs:
	rm -rf $(OW_SHARED_LIBS:%=.%.libs)
	rm -f $(OW_SHARED_LIBS:%=%*)
	rm -f $(OW_SHARED_LIBS:%.dylib.bundle=%.*.dylib*) $(OW_SHARED_LIBS:%.dylib.bundle=%.dylib*)
	rm -f *.fsl

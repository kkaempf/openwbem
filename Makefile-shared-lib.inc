
SO_EXT = $(LIBRARY_BINARY_VERSION)
SOFILENAME = $@.$(SO_EXT)
DYFILENAME = `basename -s .dylib $@`.$(SO_EXT).$(LIB_EXT)
TMPLIBDIR = .$@.libs


# Here's what this does:  It finds all symbols in each object file that are one
# of T,D,B (Global text symbol, Global data symbol, Global BSS symbol), where
# the first character is not a '.', and puts them in a list, which is then
# sorted (unique), and dumped in LIBNAME.exp 
# 
# The nm options are: Berleky style (3 column), no demangling, unsorted, global
# symbols only. 
CREATE_EXPORTS=nm -BCpg $(TMPLIBDIR)/*.o | \
			$(AWK) '{ if((($$2 == "T") || \
				   ($$2 == "D") || \
				   ($$2 == "B")) && \
				  (substr($$3,1,1) != ".")) \
				{ print $$3 } }' | \
			sort -u > $(@:%.so=%.exp)

%.so %.sl : %.a
	rm -rf $(TMPLIBDIR)
	mkdir $(TMPLIBDIR)
	cd $(TMPLIBDIR) && \
	$(AR) x ../$< 
	if [ "x$(AIX)" = "x1" ]; then $(CREATE_EXPORTS); fi
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

# Rules for Darwin so we can get the plugable modules built
# at the same time. Requires that either MAKEDYLIB or MAKEBUNDLE
# be inserted into Makefile.am in the appropriate directory.
# In the case of the four libraries that have to be both loadable
# and linkable, we FIXME EDGE

%.dylib : %.a
	rm -rf $(TMPLIBDIR)
	mkdir $(TMPLIBDIR)
	cd $(TMPLIBDIR) && \
	$(AR) x ../$< 
	if [ "x$(MAKEDYLIB)" = "x1" ]; then \
		$(CXXLINK) $(CXXSHAREDLIBLINKFLAGS) -o $(DYFILENAME) $(TMPLIBDIR)/*.o $(SHAREDLIBADD); \
	fi;
	if [ "x$(MAKEBUNDLE)" = "x1" ]; then \
		if [ "x$(MAKEDYLIB)" = "x1" ]; then \
		$(CXXLINK) $(CXXBUNDLELIBLINKFLAGS) -o $(DYFILENAME).bundle $(TMPLIBDIR)/*.o $(SHAREDLIBADD); \
		else \
		$(CXXLINK) $(CXXBUNDLELIBLINKFLAGS) -o $(DYFILENAME) $(TMPLIBDIR)/*.o $(SHAREDLIBADD); \
		fi; \
	fi;
	rm -rf $@ $(TMPLIBDIR) $(@:%.so=%.exp)
	ln -s $(DYFILENAME) $@
	chmod +x $(DYFILENAME)


all-ow-shared-libs: $(OW_SHARED_LIBS)

clean-ow-shared-libs:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done



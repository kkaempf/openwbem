
SO_EXT = $(OPENWBEM_VERSION)
SOFILENAME = $@.$(SO_EXT)
TMPLIBDIR = .$@.libs
BNDLIBDIR = /tmp/.openwbem.bundle


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

# Rules for Darwin so we can get the plugable modules built
# at the same time, then we sort them out later.

%.dylib : %.a
	rm -rf $(TMPLIBDIR)
	mkdir $(TMPLIBDIR)
	if [ ! -d $(BNDLIBDIR) ]; then mkdir $(BNDLIBDIR); fi
	cd $(TMPLIBDIR) && \
	$(AR) x ../$< 
	$(CXXLINK) $(CXXSHAREDLIBLINKFLAGS) -o $(SOFILENAME) $(TMPLIBDIR)/*.o $(SHAREDLIBADD)
	$(CXXLINK) $(CXXBUNDLELIBLINKFLAGS) -o $(BNDLIBDIR)/$(SOFILENAME) $(TMPLIBDIR)/*.o $(SHAREDLIBADD)
	rm -rf $@ $(TMPLIBDIR) $(@:%.so=%.exp)
	ln -s $(SOFILENAME) $@
	chmod +x $(SOFILENAME)
	ln -s $(BNDLIBDIR)/$(SOFILENAME) $(BNDLIBDIR)/$@
	chmod +x $(BNDLIBDIR)/$(SOFILENAME)


all-ow-shared-libs: $(OW_SHARED_LIBS)

clean-ow-shared-libs:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done



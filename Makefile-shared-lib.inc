
SO_EXT = $(OPENWBEM_VERSION)
SOFILENAME = $@.$(SO_EXT)
TMPLIBDIR = .$@.libs


%.so : %.a
	rm -rf $(TMPLIBDIR)
	mkdir $(TMPLIBDIR)
	cd $(TMPLIBDIR) && \
	$(AR) x ../$< 
	$(CXXLINK) $(CXXSHAREDLIBLINKFLAGS) -o $(SOFILENAME) $(TMPLIBDIR)/*.o $(SHAREDLIBADD)
	rm -rf $@ $(TMPLIBDIR)
	ln -s $(SOFILENAME) $@
	chmod +x $(SOFILENAME)

all-ow-shared-libs: $(OW_SHARED_LIBS)

clean-ow-shared-libs:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done




SO_EXT = $(OPENWBEM_VERSION)
SOFILENAME = $@.$(SO_EXT)
TMPLIBDIR = .$@.libs


#.a.so:
#$(LIBNAME).so : $(LIBNAME).a

%.so : %.a
	rm -rf $(TMPLIBDIR)
	mkdir $(TMPLIBDIR)
	cd $(TMPLIBDIR) && \
	$(AR) x ../$< 
	$(CXXLINK) $(CXXSHAREDLIBLINKFLAGS) -o $(SOFILENAME) $(TMPLIBDIR)/*.o $(SHAREDLIBADD)
	rm -rf $@ $(TMPLIBDIR)
	ln -s $(SOFILENAME) $@
	chmod +x $(SOFILENAME)

all-local: $(OW_SHARED_LIBS)

clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done



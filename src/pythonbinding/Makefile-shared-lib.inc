
SO_EXT = $(OWPYTHONBINDING_VERSION)
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
	rm -f $@
	ln -s $(SOFILENAME) $@
	chmod +x $(SOFILENAME)


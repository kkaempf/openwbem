install-ow-shared-libs:
	$(INSTALL_PROGRAM) -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		$(INSTALL_PROGRAM)  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -sf $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done

uninstall-ow-shared-libs:
	for i in $(OW_SHARED_LIBS); do \
		rm -f $(DESTDIR)$(thelibdir)/$$i.$(SO_EXT); \
		rm -f $(DESTDIR)$(thelibdir)/$$i; \
	done


install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -sf $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done

uninstall-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -f $(DESTDIR)$(thelibdir)/$$i.$(SO_EXT); \
		rm -f $(DESTDIR)$(thelibdir)/$$i; \
	done


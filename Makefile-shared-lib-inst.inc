install-ow-shared-libs:
	$(INSTALL_PROGRAM) -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		if [ "x$(STATIC_SERVICES)" = "x" ]; then \
			$(INSTALL_PROGRAM)  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
			ln -sf $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
		else \
			$(SHELL) $(top_srcdir)/fake_library_creator.sh $(EXPORT_PREFIX) $$i; \
			if [ -e `echo $$i | cut -f1 -d'.'`.fsl ]; then \
				$(INSTALL_PROGRAM) `echo $$i | cut -f1 -d'.'`.fsl $(DESTDIR)$(thelibdir)/$$i; \
				rm -f `echo $$i | cut -f1 -d'.'`.fsl; \
				if [ -e $(DESTDIR)$(libdir)/libopenwbem_all.a ]; then \
					echo "Adding the static version of $$i to $(DESTDIR)$(libdir)/libopenwbem_all.a"; \
					ar r $(DESTDIR)$(libdir)/libopenwbem_all.a `echo $$i | cut -f1 -d'.'`.a; \
				fi; \
			else \
				echo "Not installing $$i because no exports were found."; \
			fi; \
		fi; \
	done

uninstall-ow-shared-libs:
	for i in $(OW_SHARED_LIBS); do \
		rm -f $(DESTDIR)$(thelibdir)/$$i.$(SO_EXT); \
		rm -f $(DESTDIR)$(thelibdir)/$$i; \
	done


# This is necessary so that it can be used as a variable in the following script. If make expands it, non-bash shells puke on "for linkdir in ;"
export OW_EXTRA_LINK_DIRS

install-ow-shared-libs:
	$(INSTALL_PROGRAM) -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		if [ "x$(STATIC_SERVICES)" = "x" ]; then \
			if [ "$(LIB_EXT)" = "dylib" ]; then \
				$(INSTALL_PROGRAM) `echo $$i|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT)* $(DESTDIR)$(thelibdir); \
				rm -f $(DESTDIR)$(thelibdir)/`echo $$i|cut -f1 -d'.'`.$(LIB_EXT); \
				ln -sf `echo $$i|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT) $(DESTDIR)$(thelibdir)/`echo $$i|cut -f1 -d'.'`.$(LIB_EXT); \
				for linkdir in $$OW_EXTRA_LINK_DIRS; do \
					$(INSTALL_PROGRAM) -d $(DESTDIR)$$linkdir; \
					ln -sf $(thelibdir)/`echo $$i|cut -f1 -d'.'`.$(LIB_EXT) $(DESTDIR)$$linkdir; \
					ln -sf $(thelibdir)/`echo $$i|cut -f1 -d'.'`.$(LIB_EXT).bundle $(DESTDIR)$$linkdir; \
				done; \
			elif [ "$(LIB_EXT)" = "nlm" ]; then \
				$(INSTALL_PROGRAM)  $$i $(DESTDIR)$(thelibdir); \
				for linkdir in $$OW_EXTRA_LINK_DIRS; do \
					$(INSTALL_PROGRAM) -d $(DESTDIR)$$linkdir; \
					$(INSTALL_PROGRAM) $$i $(DESTDIR)$$linkdir; \
				done; \
			else \
				$(INSTALL_PROGRAM)  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
				rm -f $(DESTDIR)$(thelibdir)/$$i; \
				ln -sf $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
				for linkdir in $$OW_EXTRA_LINK_DIRS; do \
					$(INSTALL_PROGRAM) -d $(DESTDIR)$$linkdir; \
					ln -sf $(thelibdir)/$$i $(DESTDIR)$$linkdir; \
				done; \
			fi; \
		else \
			$(SHELL) $(top_srcdir)/fake_library_creator.sh $(EXPORT_PREFIX) $$i; \
			if [ -e `echo $$i | cut -f1 -d'.'`.fsl ]; then \
				$(INSTALL_PROGRAM) `echo $$i | cut -f1 -d'.'`.fsl $(DESTDIR)$(thelibdir)/$$i; \
				for linkdir in $$OW_EXTRA_LINK_DIRS; do \
					$(INSTALL_PROGRAM) `echo $$i | cut -f1 -d'.'`.fsl $(DESTDIR)$$linkdir/$$i; \
				done; \
				rm -f `echo $$i | cut -f1 -d'.'`.fsl; \
				if [ -e $(DESTDIR)$(libdir)/libopenwbem_all.a ]; then \
					echo "Adding the static version of $$i to $(DESTDIR)$(libdir)/libopenwbem_all.a"; \
					ar r $(DESTDIR)$(libdir)/libopenwbem_all.a `echo $$i | cut -f1 -d'.'`.a; \
				fi; \
			else \
				echo "Not installing $$i because no exports were found."; \
			fi; \
		fi; \
	done;

uninstall-ow-shared-libs:
	for i in $(OW_SHARED_LIBS); do \
		if [ "$(LIB_EXT)" = "dylib" ]; then \
			rm -f $(DESTDIR)$(thelibdir)/`echo $$i|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT); \
			rm -f $(DESTDIR)$(thelibdir)/`echo $$i|cut -f1 -d'.'`.$(LIB_EXT); \
			for linkdir in $$OW_EXTRA_LINK_DIRS; do \
				rm -f $(DESTDIR)$$linkdir/`echo $$i|cut -f1 -d'.'`.$(LIB_EXT); \
			done; \
		elif ["$(LIB_EXT)" = "nlm" ]; then \
			rm -f $(DESTDIR)$(thelibdir)/$$i; \
			for linkdir in $$OW_EXTRA_LINK_DIRS; do \
				rm -f $(DESTDIR)$$linkdir/$$i; \
			done; \
		else \
			rm -f $(DESTDIR)$(thelibdir)/$$i.$(SO_EXT); \
			rm -f $(DESTDIR)$(thelibdir)/$$i; \
			for linkdir in $$OW_EXTRA_LINK_DIRS; do \
				rm -f $(DESTDIR)$$linkdir/$$i; \
			done; \
		fi; \
	done;


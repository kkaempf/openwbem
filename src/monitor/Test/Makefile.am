# Notes for anyone modifying this file.  Arguments passed in the usage of a
# $(call <foo>,arg1,arg2,arg3) will include *any* space before or after each
# argN, so care must be taken to avoid inserting unnecessary whitespace.

noinst_PROGRAMS = \
read_privconfig montest monexectest userexectest userexecprog \
perl_launcher mpt_user mpt_mon

read_privconfig_SOURCES = read_privconfig.cpp
read_privconfig_PROJECT_LIBS = $(DEFAULT_PROJECT_LIBS)
read_privconfig_EXTERNAL_LIBS = $(DEFAULT_EXTERNAL_LIBS)

montest_SOURCES = montest.cpp
montest_PROJECT_LIBS = $(DEFAULT_PROJECT_LIBS)
montest_EXTERNAL_LIBS = $(DEFAULT_EXTERNAL_LIBS)

monexectest_SOURCES = monexectest.cpp
monexectest_PROJECT_LIBS = $(DEFAULT_PROJECT_LIBS)
monexectest_EXTERNAL_LIBS = $(DEFAULT_EXTERNAL_LIBS)

userexectest_SOURCES = userexectest.cpp
userexectest_PROJECT_LIBS = $(DEFAULT_PROJECT_LIBS)
userexectest_EXTERNAL_LIBS = $(DEFAULT_EXTERNAL_LIBS)

userexecprog_SOURCES = userexecprog.cpp
userexecprog_PROJECT_LIBS = $(DEFAULT_PROJECT_LIBS)
userexecprog_EXTERNAL_LIBS = $(DEFAULT_EXTERNAL_LIBS)

perl_launcher_SOURCES = perl_launcher.cpp
perl_launcher_PROJECT_LIBS = $(DEFAULT_PROJECT_LIBS)
perl_launcher_EXTERNAL_LIBS = $(DEFAULT_EXTERNAL_LIBS)

mpt_user_SOURCES = mpt_user.cpp
mpt_user_PROJECT_LIBS = $(DEFAULT_PROJECT_LIBS)
mpt_user_EXTERNAL_LIBS =

mpt_mon_SOURCES = mpt_mon.cpp
mpt_mon_PROJECT_LIBS = $(DEFAULT_PROJECT_LIBS)
mpt_mon_EXTERNAL_LIBS = $(DEFAULT_EXTERNAL_LIBS)

AM_CPPFLAGS = \
-I$(srcdir)/.. \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs

DEFAULT_PROJECT_LIBS = \
$(BLOCXX_LIBS) -L$(top_builddir)/src/common -lopenwbem

DEFAULT_EXTERNAL_LIBS = \
../libowprivman.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)

if ENABLE_MONITORED_PERL_IFC
PERL_CHECK_S = monitor_perl_test.0.sh
else
PERL_CHECK_S =
endif

priv_config_test_scripts = \
read_priv_config_test.0.sh \
read_priv_config_test.1.sh \
read_priv_config_test.2.sh \
read_priv_config_test.3.sh \
read_priv_config_test.4.sh \
read_priv_config_test.5.sh \
read_priv_config_test.6.sh

monitor_test_scripts = \
monitor_test.0.sh \
monitor_test.1.sh \
monitor_test.2.sh

monitor_exec_test_scripts = \
monitor_exec_test.0.sh \
monitor_exec_test.nopriv.0.sh \
monitor_exec_test.nopriv.1.sh \
monitor_exec_test.nopriv.2.sh

user_exec_test_scripts = \
user_exec_test.0.sh \
user_exec_test.1.sh \
user_exec_test.2.sh


# This may be hackish, but the old stuff was hard to read and difficult to modify.
monitor_test_nopriv_numbers = 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
monitor_test_nopriv_data = \
$(foreach testnum,$(monitor_test_nopriv_numbers),monitor_test_nopriv.$(testnum).inp monitor_test_nopriv.$(testnum).exceptions)

monitor_test_extensions = cfg inp test.tgz gold.tgz
monitor_exec_test_data = \
$(foreach ext,$(monitor_test_extensions),monitor_exec_test.0.$(ext))
monitor_test_data = \
$(foreach ext,$(monitor_test_extensions),monitor_test.0.$(ext))


check_SCRIPTS = \
$(priv_config_test_scripts) \
$(monitor_test_scripts) \
monitor_test_noconfig.0.sh \
monitor_test_username.0.sh \
monitor_test_nopriv.0.sh \
$(user_exec_test_scripts) \
$(monitor_exec_test_scripts) \
$(PERL_CHECK_S)

TESTS = $(check_SCRIPTS)

EXTRA_DIST = \
set_environment.sh.in \
$(priv_config_test_scripts:%.sh=%.gold) \
montest.sh \
monperltest.sh \
monperltest.pl \
monitor_test_noconfig.sh \
monitor_test_username.sh \
monexectest.sh \
userexectest.sh \
montest_nopriv.sh \
monexectest.sh \
$(priv_config_test_scripts:%.sh=%.cfg) \
$(priv_config_test_scripts:%.sh=%.paths) \
$(priv_config_test_scripts:%.sh=%.execpaths) \
$(monitor_test_data) \
monitor_test.nopriv.cfg \
monitor_test.nopriv.test.tgz \
$(monitor_exec_test_data) \
$(user_exec_test_scripts:%.sh=%.cfg) \
monitor_perl_test.0.inp \
monitor_perl_test.0.cfg

#monitor_perl_test.0.gold.out 
#$(monitor_exec_test_scripts:%.sh=%.gold.out)
#$(monitor_test_scripts:%.sh=%.gold.out)
# $(monitor_test_nopriv_data) 
# monitor_perl_test_mon.cfg

# The inpf here is NOT a typeo.  It is used by some of the scripts.
CLEANFILES = \
$(monitor_test_scripts) \
monitor_test_noconfig.0.sh \
monitor_test_nopriv.0.sh \
monitor_test_username.0.sh \
$(monitor_exec_test_scripts) \
$(priv_config_test_scripts) \
$(priv_config_test_scripts:%.sh=%.out) \
$(user_exec_test_scripts) \
monitor_perl_test.0.sh \
*.inpf \
montest.inp montest.out \
userexectest.expected userexectest.out \
set_environment.sh


#
# read priv config tests
#

# $(1) = whitespace seperated list of user identifiers (no need to actually exist)
define read_priv_config_template
	echo "#!/bin/bash" > $@
	echo 'echo Running $$0 $$@' >> $@
	echo ". ./set_environment.sh" >> $@
	echo "./read_privconfig $(srcdir)/$(@:%.sh=%.cfg) $(srcdir)/$(@:%.sh=%.paths) $(srcdir)/$(@:%.sh=%.execpaths) $(1) > $(@:%.sh=%.out) 2>&1" >> $@
	echo "/usr/bin/diff $(@:%.sh=%.out) $(srcdir)/$(@:%.sh=%.gold)" >> $@
	chmod +x $@
endef

read_priv_config_test.0.sh: Makefile
	$(call read_priv_config_template,foo)

read_priv_config_test.1.sh: Makefile
	$(call read_priv_config_template,bar)

read_priv_config_test.2.sh: Makefile
	$(call read_priv_config_template,ewe-vee equis foo)

read_priv_config_test.3.sh: Makefile
	$(call read_priv_config_template,open_r open_a unlink unpriv_user user_exec shutdown rename_to executable_path crack foo)

read_priv_config_test.4.sh: Makefile
	$(call read_priv_config_template,app1 app2 app3 app4)

read_priv_config_test.5.sh: Makefile
	$(call read_priv_config_template,app1 app2 app3 app4)

read_priv_config_test.6.sh: Makefile
	$(call read_priv_config_template,none you me fred bob joe sam)

#
# monitor tests
#

# $(1) = user
# $(2) = mode
# $(3) = nofail
# $(4) = test output file text (gold or test)
define monitor_test_template
	echo "#!/bin/bash" > $@
	echo 'echo Running $$0 $$@' >> $@
	echo "$(srcdir)/montest.sh $(srcdir)/monitor_test.0.cfg $(1) $(2) $(srcdir)/monitor_test.0.test.tgz $(srcdir)/monitor_test.0.inp $(srcdir)/$(@:%.sh=%.exceptions) $(srcdir)/monitor_test.0.$(4).tgz $(3)" >> $@
	chmod +x $@
endef

monitor_test.0.sh: Makefile
	$(call monitor_test_template,root,700,1,gold)

monitor_test.1.sh: Makefile
	$(call monitor_test_template,root,702,0,test)

monitor_test.2.sh: Makefile
	$(call monitor_test_template,owprovdr,777,0,test)


#
#
#
monitor_test_noconfig.0.sh: Makefile
	echo "#!/bin/bash" > $@
	echo "$(srcdir)/monitor_test_noconfig.sh $(srcdir)" >> $@
	chmod +x $@

monitor_test_username.0.sh: Makefile
	echo "#!/bin/bash" > $@
	echo "$(srcdir)/monitor_test_username.sh $(srcdir)" >> $@
	chmod +x $@

#
# monitor exec tests
#

# $(1) = privtype (OK, BAD_EXEC, BAD_CFG, etc)
# $(2) = config file
# $(3) = result file text (gold or test)
# This uses the target name as a base for the cfg and "gold" output files.
define monitor_exec_test_template
	echo "#!/bin/bash" > $@
	echo "set -e" >> $@
	echo 'echo Running $$0 $$@' >> $@
	echo 'for i in 0 1; do' >> $@
	echo 'MUE=$$i MXTPRIV=$(1) $(srcdir)/monexectest.sh $(srcdir)/$(2) root 700 $(srcdir)/monitor_exec_test.0.test.tgz $(srcdir)/monitor_exec_test.0.inp $(srcdir)/$(@:%.sh=%.exceptions) $(srcdir)/monitor_exec_test.0.$(3).tgz' >> $@
	echo 'done' >> $@
	chmod +x $@
endef

monitor_exec_test.0.sh: Makefile
	$(call monitor_exec_test_template,OK,monitor_exec_test.0.cfg,gold)

# Why does this one use a different config file than the others?
monitor_exec_test.nopriv.0.sh: Makefile
	$(call monitor_exec_test_template,OK,monitor_test.nopriv.cfg,test)

monitor_exec_test.nopriv.1.sh: Makefile
	$(call monitor_exec_test_template,BAD_EXEC,monitor_exec_test.0.cfg,test)

monitor_exec_test.nopriv.2.sh: Makefile
	$(call monitor_exec_test_template,BAD_CFG,monitor_exec_test.0.cfg,test)


#
# user exec tests.
#

# $(1) = nofail (0 or 1)
define user_exec_test_template
	echo "#!/bin/bash" > $@
	echo 'echo Running $$0 $$@' >> $@
	echo "$(srcdir)/userexectest.sh $(srcdir)/$(@:%.sh=%.cfg) root 700 $(1)" >> $@
	chmod +x $@
endef

user_exec_test.0.sh: Makefile
	$(call user_exec_test_template,1)

user_exec_test.1.sh: Makefile
	$(call user_exec_test_template,0)

user_exec_test.2.sh: Makefile
	$(call user_exec_test_template,0)


#
#
#
monitor_test_nopriv.0.sh: Makefile
	echo "#!/bin/bash" > $@
	echo "$(srcdir)/montest_nopriv.sh $(srcdir)" >> $@
	chmod +x $@

# Note: if/when we revert back to using monperltest.pl instead of
# monperltest_basic.pl, we need to also revert back to using
# monitor_perl_test.0.gold.out instead of monitor_perl_test.0.basic.gold.out, and
# mt.0.gold.tgz instead of mt.0.basic.gold.tgz
monitor_perl_test.0.sh: Makefile
	echo "#!/bin/bash" > $@
	echo "PERL_PATH=$(PERL) $(srcdir)/monperltest.sh $(srcdir)/$(@:%.sh=%.cfg) $(srcdir)/monitor_test.0.test.tgz $(srcdir)/$(@:%.sh=%.inp) $(srcdir)/$(@:%.sh=%.exceptions) $(srcdir)/monitor_test.0.basic.gold.tgz" >> $@
	chmod +x $@

helper_scripts = set_environment.sh
BUILT_SOURCES = $(helper_scripts)

local-scripts: $(helper_scripts)
	chmod +x $^

all-local: local-scripts

include $(top_srcdir)/Makefile-replacement.inc
include $(top_srcdir)/Makefile-executable.inc

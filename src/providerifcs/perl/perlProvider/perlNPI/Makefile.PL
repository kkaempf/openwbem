use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    'NAME'		=> 'perlNPI',
    'VERSION_FROM'	=> 'perlNPI.pm', # finds $VERSION
    'PREREQ_PM'		=> {}, # e.g., Module::Name => 1.1
    'LIBS'		=> ['-lpthread '], # e.g., '-lm'
    'DEFINE'		=> '-D_COMPILE_UNIX ', # e.g., '-DHAVE_SOMETHING'
    'INC'		=> '-I/usr/include -I/usr/include/g++ -I$(TOP_SRCDIR)/src/providerifcs/perl/ -I$(TOP_SRCDIR)/src/providerifcs/perl/perlProvider -I$(TOP_SRCDIR)/src/providerifcs/npi/common'
);

sub MY::postamble {
   return << 'MAKE_FRAG';
PREFIX = $(DESTDIR)/usr
UNINSTALL = $(PERLRUN) "-MExtUtils::Install" \
-e 'uninstall($$ARGV[0],1,0);'
uninstall :: uninstall_from_$(INSTALLDIRS)dirs
 
uninstall_from_perldirs ::
        @$(UNINSTALL) $(PERL_ARCHLIB)/auto/$(FULLEXT)/.packlist
 
uninstall_from_sitedirs ::
        @$(UNINSTALL) $(SITEARCHEXP)/auto/$(FULLEXT)/.packlist

MAKE_FRAG
}

use 5.006;
use ExtUtils::MakeMaker;
use Cwd;
use Config;

sub fix_if_relative_path($) {
	my ($p) = @_;
	if ($p =~ m|^[^/]|) {
		$p = Cwd::realpath("../$p");
	}
	$p
}

my $owcprivman_build = fix_if_relative_path('@owcprivman_build@');
my $owcprivman_install = fix_if_relative_path('@owcprivman_install@');
my $monitor_source = fix_if_relative_path('@monitor_source@');
my $owcommon = fix_if_relative_path('@owcommon@');
my $rpath = '@rpath@';
my $extra_lddlflags = '';
if ($rpath) {
	$extra_lddlflags = " $rpath:$owcprivman_install";
}

# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    CC                => '@OWCC@', # @OWCC@ instead of @CC@ because @CC@ gets expanded in the Makefile.am
    NAME              => 'Privman',
    VERSION_FROM      => 'lib/Privman.pm', # finds $VERSION
    PREREQ_PM         => {}, # e.g., Module::Name => 1.1
    ($] >= 5.005 ?     ## Add these new keywords supported since 5.005
      (ABSTRACT_FROM  => 'lib/Privman.pm', # retrieve abstract from module
       AUTHOR         => 'Kevin S. Van Horn') : ()),

    LIBS              => ["@OWLDFLAGS@ -L$owcprivman_build -lowcprivman"],
    DEFINE            => '', # e.g., '-DHAVE_SOMETHING'
	# -I$ow_common is needed ONLY because the coding guidelines for header
	# files forces us to include OW_config.h in OW_PrivManOpenFlags.h
    INC               => "-I. -I$monitor_source -I$monitor_source/Cifc -I$owcommon @OWCPPFLAGS@",
	LDDLFLAGS         => $Config{lddlflags} . $extra_lddlflags,
	# Un-comment this if you add C files to link with later:
	# OBJECT            => '$(O_FILES)', # link all the C files too
    LD                => '@OWCC@',
);

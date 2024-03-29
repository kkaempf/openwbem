#!/bin/bash

#set -x
set -e
set -o nounset # treat undefined variables as errors, not as empty string

ulimit -c unlimited

. ./set_environment.sh
thisdir=`/bin/pwd`

script_cfgpath=$1
testtgz=$2
inpf=$3
exception_list=$4
goldtgz=$5

# Convert $inpf to an absolute path
pushd `dirname $inpf` > /dev/null
inpf=`/bin/pwd`/`basename $inpf`
popd > /dev/null

user=owprovdr
config_dir_owner=root
config_dir_perms=700
gold_dir=montest_gold_dir
base_dir=/montest-689acb0e1ec89f45-7085a2a1780f5f42
test_dir=$base_dir/test_dir
safe_bin=$base_dir/safebin
lib_dir=$base_dir/lib
config_dir=$base_dir/config_dir
# script_cfgfname=`basename $script_cfgpath`
script_cfgfname=perl_script.cfg
srcdir=`dirname $script_cfgpath`
launcher_cfgfname=perl_launcher.cfg

if [ "${EUID:-EUID_NOT_SET}" = "0" ] || [ "${UID:-UID_NOT_SET}" = "0" ]; then
	:
else
	if id | grep 'uid=0' >/dev/null 2>&1; then
		:
	else
		echo "*** SKIPPED: You must run this test as root"
		exit 0
	fi
fi

root_group=`grep '^root' /etc/passwd | head -n 1 | cut -f3 -d':'`
extended_sed_flag=-r

if [ "x`uname`" = "xDarwin" ]; then
	extended_sed_flag=-E
fi

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$lib_dir
export LIBPATH=$LD_LIBRARY_PATH:$LIBPATH   # AIX
export SHLIB_PATH=$LD_LIBRARY_PATH         # HPUX
export DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH  # DARWIN (OS X)

# MPT=$srcdir/monperltest.pl
MPT=$srcdir/monperltest_basic.pl

rm -rf $base_dir
mkdir -p $base_dir
cp $inpf $base_dir
chmod go+r $base_dir/`basename $inpf`
mkdir -p $safe_bin
cp $MPT $safe_bin
chmod o+r $MPT
cp mpt_user $safe_bin
chmod a+rx $safe_bin/mpt_user
cp mpt_mon $safe_bin
chmod a+rx $safe_bin/mpt_mon

owlibexec_dir=$base_dir/libexec/openwbem
mkdir -p $owlibexec_dir
cp ../bin/owprivilegemonitor* $owlibexec_dir
ln -s $PERL_PATH $owlibexec_dir/perl
chmod og-w $owlibexec_dir/owprivilegemonitor*

mkdir -p $lib_dir
mkdir -p $lib_dir/openwbem/monitoredPerl
cp -R ../Perlifc/perlxsbuild/blib/arch $lib_dir/openwbem/monitoredPerl
cp -R ../Perlifc/perlxsbuild/blib/lib $lib_dir/openwbem/monitoredPerl
cp ../Cifc/libowcprivman* $lib_dir
cp ../libowprivman* $lib_dir
cp ../../common/libopenwbem* $lib_dir
chmod -R a+rx $lib_dir

mkdir -p $config_dir
cat > $config_dir/$launcher_cfgfname <<EOF
unpriv_user { owprovdr }
monitored_exec { $owlibexec_dir/perl @ $script_cfgfname }
EOF
cp $script_cfgpath $config_dir/$script_cfgfname
cp $srcdir/mpt_mon.cfg $config_dir
chown root:${root_group} $config_dir/$launcher_cfgfname
chown root:${root_group} $config_dir/$script_cfgfname
chmod og-rwx $config_dir/$launcher_cfgfname
chmod og-rwx $config_dir/$script_cfgfname
chmod og-rwx $config_dir/mpt_mon.cfg
chown $config_dir_owner $config_dir 
chmod $config_dir_perms $config_dir

mkdir -p $test_dir
tar -C $test_dir -xzf $testtgz
chown -R root:${root_group} $test_dir
chmod -R og-rwx $test_dir

./perl_launcher $config_dir $user $safe_bin $base_dir/`basename $inpf` \
	> monperltest.out 2>&1

`dirname $0`/check_for_exceptions.sh $exception_list monperltest.out || exit $?

rm -rf $gold_dir
mkdir -p $gold_dir
tar -C $gold_dir -xzf $goldtgz

# There is no easy way to overcome this without directly laying a hack here.
# Putting a dangling symlink in the tarfile causes the diff to abort.  Remove
# it from the test directory here.
#
# Some of the evil tests use the original test structure for the gold
# directory.  Hose it from there as well.
rm -f $test_dir/dangling $gold_dir/dangling

chown -R root:${root_group} $gold_dir
diff -r $gold_dir $test_dir
pushd $gold_dir > /dev/null
ls -lR | sed ${extended_sed_flag} -e 's|(^[^ ]+) .* ([^ ]+)$|\1 \2|' > $thisdir/monperltest.gold.ls
popd > /dev/null
pushd $test_dir > /dev/null
ls -lR | sed ${extended_sed_flag} -e 's|(^[^ ]+) .* ([^ ]+)$|\1 \2|' > $thisdir/monperltest.ls
popd > /dev/null

diff monperltest.gold.ls monperltest.ls

rm -f monperltest.out monperltest.gold.ls monperltest.ls
rm -rf $gold_dir
rm -rf $base_dir

#!/bin/bash

#set -x # DEBUG
set -e
set -o nounset # treat undefined variables as errors, not as empty string

. `dirname $0`/set_environment.sh
thisdir=`pwd`

cfgpath=$1
config_dir_owner=$2
config_dir_perms=$3
testtgz=$4
inpf=$5
goldout=$6
goldtgz=$7


gold_dir=monexectest_gold_dir
base_dir=/monexectest-689acb0e1ec89f45-7085a2a1780f5f42
config_dir=$base_dir/config_dir
test_dir=$base_dir/test_dir
safe_bin=$base_dir/safebin

cfgfname=`basename $cfgpath`

monexectest_path=$thisdir/monexectest
monexectest_cfgfname=monexectest.cfg

if [ `whoami` != root ]; then
  echo "*** SKIPPED: You must run this test as root"
  exit 0
fi

root_group=`grep '^root' /etc/passwd | head -n 1 | cut -f3 -d':'`
extended_sed_flag=-r

if [ "x`uname`" = "xDarwin" ]; then
	extended_sed_flag=-E
fi

umask 022
rm -rf $safe_bin
mkdir -p $safe_bin
cp $thisdir/montest $safe_bin
chmod o+rx $base_dir
chmod o+rx $safe_bin
chmod o+rx $safe_bin/montest
chmod og-w $safe_bin/montest
owlibexec_dir=$base_dir/libexec/openwbem
mkdir -p $owlibexec_dir
cp ../bin/owprivilegemonitor* $owlibexec_dir
chmod og-w $owlibexec_dir/owprivilegemonitor*

rm -rf $config_dir
mkdir -p $config_dir
cp $cfgpath $config_dir
if [ $MXTPRIV = OK ]; then
cat > $config_dir/$monexectest_cfgfname <<EOF
  unpriv_user { owprovdr }
  monitored_exec { $base_dir/safebin/montest @ $cfgfname }
EOF
elif [ $MXTPRIV = BAD_EXEC ]; then
cat > $config_dir/$monexectest_cfgfname <<EOF
  unpriv_user { owprovdr }
  monitored_exec { $base_dir/safebin/nonesuch @ $cfgfname }
EOF
elif [ $MXTPRIV = BAD_CFG ]; then
cat > $config_dir/$monexectest_cfgfname <<EOF
  unpriv_user { owprovdr }
  monitored_exec { $base_dir/safebin/montest @ nonesuch }
EOF
else
echo "MXTPRIV set to unrecognized value"
exit 1
fi

chown $config_dir_owner $config_dir 
chmod $config_dir_perms $config_dir
chown root:${root_group} $config_dir/$cfgfname
chown root:${root_group} $config_dir/$monexectest_cfgfname
chmod og-rwx $config_dir/$cfgfname
chmod og-rwx $config_dir/$monexectest_cfgfname

rm -rf $test_dir
mkdir -p $test_dir
gunzip -c $testtgz | tar -C $test_dir -xf -
chown -R root:${root_group} $test_dir
chmod -R og-rwx $test_dir

cat > monexectest.inp <<EOF
config_dir $config_dir/
app_name $cfgfname
user_name null
EOF
cat $inpf >> monexectest.inp
./monexectest $config_dir/ $monexectest_cfgfname $safe_bin/montest $cfgfname \
  montest+arg1+arg2 'IFS= +PATH=/bin+foo=bar' \
  < monexectest.inp > monexectest.out.tmp

sed ${extended_sed_flag} -e 's/^  OW_PRIVILEGE_MONITOR_DESCRIPTOR=[0-9]+;$/  OW_PRIVILEGE_MONITOR_DESCRIPTOR=XXX;/' \
	-e 's/^  .+PATH=.*;$/  XPATH=XXX;/' \
  < monexectest.out.tmp > monexectest.out
diff $goldout monexectest.out

rm -rf $gold_dir
mkdir -p $gold_dir
gunzip -c $goldtgz | tar -C $gold_dir -xf -
chown -R root:${root_group} $gold_dir
diff -r $gold_dir $test_dir
pushd $gold_dir > /dev/null
ls -lR | sed ${extended_sed_flag} -e 's|(^[^ ]+) .* ([^ ]+)$|\1 \2|' -e '/^total.*/d' > $thisdir/monexectest.gold.ls
popd > /dev/null
pushd $test_dir > /dev/null
ls -lR | sed ${extended_sed_flag} -e 's|(^[^ ]+) .* ([^ ]+)$|\1 \2|' -e '/^total.*/d' > $thisdir/monexectest.ls
popd > /dev/null

diff monexectest.gold.ls monexectest.ls

rm -f monexectest.inp monexectest.out monexectest.gold.ls monexectest.ls \
  monexectest.out.tmp
rm -rf $config_dir
rm -rf $test_dir
rm -rf $gold_dir
rm -rf $safe_bin
rm -rf $base_dir
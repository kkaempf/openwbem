#!/bin/bash

#set -x #DEBUG
set -e
set -o nounset # treat undefined variables as errors, not as null

. `dirname $0`/set_environment.sh
thisdir=`pwd`

cfgpath=$1
config_dir_owner=$2
config_dir_perms=$3
nofail=$4

base_dir=/userexectest-689acb0e1ec89f45-7085a2a1780f5f42
config_dir=$base_dir/config_dir
safe_bin=$base_dir/safebin
cfgfname=`basename $cfgpath`

if [ `whoami` != root ]; then
  echo "*** SKIPPED: You must run this test as root"
  exit 0
fi

root_group=`grep '^root' /etc/passwd | head -n 1 | cut -f3 -d':'`

umask 022
rm -rf $safe_bin
mkdir -p $safe_bin
cp $thisdir/userexecprog $safe_bin
chmod o+rx $base_dir
chmod o+rx $safe_bin
chmod o+rx $safe_bin/userexecprog
chmod og-w $safe_bin/userexecprog

owlibexec_dir=$base_dir/libexec/openwbem
mkdir -p $owlibexec_dir
cp ../bin/owprivilegemonitor* $owlibexec_dir
chmod og-w $owlibexec_dir/owprivilegemonitor*

rm -rf $config_dir
mkdir -p $config_dir
cp $cfgpath $config_dir
chown $config_dir_owner $config_dir 
chmod $config_dir_perms $config_dir
chown root:${root_group} $config_dir/$cfgfname
chmod og-rwx $config_dir/$cfgfname

if [ $nofail -ne 0 ]; then
./userexectest $config_dir/ $cfgfname \
  $safe_bin/userexecprog \
  userexecprog+37+stderr_string 'IFS= +PATH=/bin+foo=bar' \
  stdin_string
else
cat > userexectest.expected <<EOF
InsufficientPrivilegesException  CheckException: userSpawn: insufficient privileges
EOF
! ./userexectest $config_dir/ $cfgfname \
  $safe_bin/userexecprog \
  userexecprog+37+stderr_string 'IFS= +PATH=/bin+foo=bar' \
  stdin_string > userexectest.out
`dirname $0`/check_for_exceptions.sh userexectest.expected userexectest.out || exit $?
rm -f userexectest.{expected,out}
fi

rm -rf $config_dir
rm -rf $safe_bin
rm -rf $base_dir

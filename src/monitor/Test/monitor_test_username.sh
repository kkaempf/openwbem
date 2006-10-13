#!/bin/bash

#set -x
set -e
set -o nounset # treat undefined variables as errors, not as empty string

srcdir=$1

tmpdir=mtusername.tmpdir
testtgz=mtusername.test.tgz
rm -rf $tmpdir
mkdir $tmpdir
cd $tmpdir
touch alphard
touch bethe
touch gamow
chmod og-rwx *
tar cf - . | gzip > ../$testtgz
cd ..
rm -rf $tmpdir

inpf=mtusername.inpf
exceptionlist=mtusername.exceptions
thisdir=`/bin/pwd`
cfgfname=mtusername.cfg

cat > $inpf <<EOF
readDirectory /montest-689acb0e1ec89f45-7085a2a1780f5f42/test_dir/ omit_special
EOF

function run_one_test {
  $srcdir/montest.sh $thisdir/$cfgfname:$1 root 700 \
    $testtgz $inpf $exceptionlist $testtgz $2
}


# user name specified in config file

cat > $cfgfname <<EOF
unpriv_user { owprovdr }
read_dir {
  /montest-689acb0e1ec89f45-7085a2a1780f5f42/test_dir/
}
EOF

cat > $exceptionlist <<EOF
EOF
run_one_test null 1
run_one_test empty 1
run_one_test owprovdr 1

cat > $exceptionlist <<EOF
PrivilegeManagerException   PrivilegeManager::init() failed to spawn monitor: creation of monitor failed: user name specified as both owprovdr and not_owprovdr
EOF
run_one_test not_owprovdr 0


# user name NOT specified in config file

cat > $cfgfname <<EOF
read_dir {
  /montest-689acb0e1ec89f45-7085a2a1780f5f42/test_dir/
}
EOF

cat > $exceptionlist <<EOF
PrivilegeManagerException   PrivilegeManager::init() failed to spawn monitor: creation of monitor failed: must specify user name
EOF
run_one_test null 0
run_one_test empty 0

cat > $exceptionlist <<EOF
EOF
run_one_test owprovdr 1


rm -rf $tmpdir
rm -f $inpf $exceptionlist $cfgfname $testtgz

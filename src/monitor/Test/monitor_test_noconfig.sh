#!/bin/bash

set -e
set -o nounset # treat undefined variables as errors, not as empty string

srcdir=$1

tmpdir=mtnoconfig.tmpdir
testtgz=mtnoconfig.test.tgz
rm -rf $tmpdir
mkdir $tmpdir
cd $tmpdir
tar cf - . | gzip > ../$testtgz
cd ..
rmdir $tmpdir

inpf=mtnoconfig.inpf
goldout=mtnoconfig.goldout

function run_one_test {
  echo "run_one_test: $@"
  $srcdir/montest.sh $1 root 700 $testtgz $inpf $goldout $testtgz 1
}

function run_tests {
  echo "run_tests: $@"
  run_one_test noexist
  run_one_test configdir_null
  run_one_test configdir_empty
  run_one_test appname_null
  run_one_test appname_empty
}

cat > $inpf <<EOF
readDirectory /montest-689acb0e1ec89f45-7085a2a1780f5f42/test_dir/ keep_special
EOF
cat > $goldout <<EOF
User: owprovdr

readDirectory
Exception:
  type: PrivilegeManagerException
  msg:  PrivilegeManager::readDirectory: process has no privileges
EOF
run_tests readDirectory

cat > $inpf <<EOF
rename /montest-689acb0e1ec89f45-7085a2a1780f5f42/test_dir/rename1 /montest-689acb0e1ec89f45-7085a2a1780f5f42/test_dir/rename-to
EOF
cat > $goldout <<EOF
User: owprovdr

rename
Exception:
  type: PrivilegeManagerException
  msg:  PrivilegeManager::rename: process has no privileges
EOF
run_tests rename

cat > $inpf <<EOF
unlink /montest-689acb0e1ec89f45-7085a2a1780f5f42/test_dir/unlink-exists
EOF
cat > $goldout <<EOF
User: owprovdr

unlink
Exception:
  type: PrivilegeManagerException
  msg:  PrivilegeManager::unlink: process has no privileges
EOF
run_tests unlink

cat > $inpf <<EOF
open in 0 /montest-689acb0e1ec89f45-7085a2a1780f5f42/test_dir/bar
EOF
cat > $goldout <<EOF
User: owprovdr

open
Exception:
  type: PrivilegeManagerException
  msg:  PrivilegeManager::open: process has no privileges
EOF
run_tests open

cat > $inpf <<EOF
monitoredSpawn /bin/rm rmapp a+b+c PATH=/bin
EOF
cat > $goldout <<EOF
User: owprovdr

monitoredSpawn
Exception:
  type: PrivilegeManagerException
  msg:  PrivilegeManager::monitoredSpawn: process has no privileges
EOF
run_tests 'spawn(root)'

cat > $inpf <<EOF
userSpawn /bin/rm a+b+c PATH=/bin owprovdr
EOF
cat > $goldout <<EOF
User: owprovdr

userSpawn
Exception:
  type: PrivilegeManagerException
  msg:  PrivilegeManager::userSpawn: process has no privileges
EOF
run_tests 'spawn(owprovdr)'

rm -f $testtgz $inpf $goldout

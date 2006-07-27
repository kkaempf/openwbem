#!/bin/bash

#set -x # DEBUG
set -e
set -o nounset # treat undefined variables as errors, not as empty string

umask 022

. `dirname $0`/set_environment.sh
thisdir=`pwd`

cfgpath=$1
config_dir_owner=$2
config_dir_perms=$3
testtgz=$4
inpf=$5
goldout=$6
goldtgz=$7
nofail=$8

gold_dir=montest_gold_dir
base_dir=/montest-689acb0e1ec89f45-7085a2a1780f5f42
safe_bin=$base_dir/safebin
test_dir=$base_dir/test_dir
config_dir=$base_dir/config_dir
username=null

if [ -f `pwd`/${testtgz} ]; then
	 testtgz=`pwd`/${testtgz}
fi

if [ -f `pwd`/${goldtgz} ]; then
	 goldtgz=`pwd`/${goldtgz}
fi

cleanup()
{
	rm -f montest.inp montest.out montest.gold.ls montest.ls
	rm -rf $config_dir
	rm -rf $test_dir
	rm -rf $safe_bin
	rm -rf $gold_dir
	rm -rf $base_dir
}

# see if cfgpath contains a : and then split it into path and username.
tmp1=${cfgpath%:*}
tmp2=${cfgpath#*:}
if [ $cfgpath = $tmp1:$tmp2 ]; then
  cfgpath=$tmp1
  username=$tmp2
fi

if [ $cfgpath = noexist ]; then
  cfgfname=noexist
  username=owprovdr
elif [ $cfgpath = configdir_null ]; then
  cfgfname=noexist
  config_dir=null
  username=owprovdr
elif [ $cfgpath = configdir_empty ]; then
  cfgfname=noexist
  config_dir=empty
  username=owprovdr
elif [ $cfgpath = appname_null ]; then
  cfgfname=null
  username=owprovdr
elif [ $cfgpath = appname_empty ]; then
  cfgfname=empty
  username=owprovdr
else
  cfgfname=`basename $cfgpath`
fi

if [ `whoami` != root ]; then
  echo "*** SKIPPED: You must be root to run this test"
  exit 0
fi

cleanup

root_group=`grep '^root' /etc/passwd | head -n 1 | cut -f3 -d':'`
extended_sed_flag=-r

if [ "x`uname`" = "xDarwin" ]; then
	extended_sed_flag=-E
fi

if [ $config_dir != null -a $config_dir != empty ]; then
  rm -rf $config_dir
  mkdir -p $config_dir
  if [ $cfgfname != null -a $cfgfname != empty -a $cfgfname != noexist ]; then
    cp $cfgpath $config_dir
    chown root:${root_group} $config_dir/$cfgfname
    chmod og-rwx $config_dir/$cfgfname
  fi
  chown $config_dir_owner $config_dir 
  chmod $config_dir_perms $config_dir
fi


rm -rf $test_dir
mkdir -p $test_dir
old_dir=`pwd`
cd $test_dir
gunzip -c $testtgz | tar -xf -
cd ${old_dir}
chown -R root:${root_group} $test_dir
chmod -R og-rwx $test_dir

rm -rf $safe_bin
mkdir -p $safe_bin
owlibexec_dir=$base_dir/libexec/openwbem
mkdir -p $owlibexec_dir
cp ../bin/owprivilegemonitor* $owlibexec_dir
chmod og-w $owlibexec_dir/owprivilegemonitor*

cat > montest.inp <<EOF
config_dir $config_dir
app_name $cfgfname
user_name $username
EOF
cat $inpf >> montest.inp
execution_result=0
if ./montest < montest.inp > montest.out; then
	:
else
	execution_result=$?
fi

if [ $nofail -ne 0 ]; then
	if [ ${execution_result} != 0 ]; then
		echo "Execution of montest returned ${execution_result}.  Expecting success."
		exit ${execution_result}
	fi
else
	if [ ${execution_result} = 0 ]; then
		echo "Execution of montest returned ${execution_result}.  Expecting failure."
		exit ${execution_result}
	fi
fi

# if [ -f $config_dir/monitorlog ]; then
# cat $config_dir/monitorlog
# else
# echo "No log file found"
# fi

if diff $goldout montest.out; then
	:
else
	foo=$?
	echo "Execution output did not match \"gold\" output." >&2
	exit ${foo}
fi

rm -rf $gold_dir
mkdir -p $gold_dir
old_dir=`pwd`
cd ${gold_dir}
gunzip -c $goldtgz | tar -xf -
cd ${old_dir}
chown -R root:${root_group} $gold_dir
diff -r $gold_dir $test_dir
pushd $gold_dir > /dev/null
ls -lR | sed ${extended_sed_flag} -e 's|(^[^ ]+) .* ([^ ]+)$|\1 \2|' -e '/^total.*/d' > $thisdir/montest.gold.ls
popd > /dev/null
pushd $test_dir > /dev/null
ls -lR | sed ${extended_sed_flag} -e 's|(^[^ ]+) .* ([^ ]+)$|\1 \2|' -e '/^total.*/d' > $thisdir/montest.ls
popd > /dev/null

if diff montest.gold.ls montest.ls; then
	:
	# echo "Test output matches."
else
	foo=$?
	echo "File list did not match \"gold\" list." >&2
	exit ${foo}
fi

cleanup

%define prefix /usr
%define localstatedir /var/lib

%define startnum 36
%define killnum 64
%define daemonname owcimomd
%define owversion 3.0.1

Name        	: openwbem
Version     	: %{owversion}
Release     	: 1
Group       	: Administration/System

Summary     	: The OpenWBEM CIMOM

Copyright   	: BSD
Packager    	: Bart Whiteley <bartw AT users DOT sourceforge DOT net>
URL         	: http://www.openwbem.org/
BuildRoot   	: %{_tmppath}/%{name}-%{version}

Source0: %{name}-%{version}.tar.gz


%package devel
Group           : Programming/Library
Summary         : Headers files for OpenWBEM
Requires	: openwbem

#%package doc
#Group           : Programming/Library
#Summary         : Documentation files for OpenWBEM

#%package perlNPI
#Group       	: Administration/System
#Summary     	: PerlNPI provider interface for the OpenWBEM CIMOM
#Requires	: openwbem, perl

%description
The OpenWBEM CIMOM

%description devel
Headers files for OpenWBEM

#%description doc
#Documentation files for OpenWBEM

#%description perlNPI
#PerlNPI provider interface for the OpenWBEM CIMOM

%prep

%setup 

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure \
	--prefix=%{prefix} \
	--sysconfdir=/etc \
	--localstatedir=%{localstatedir}

make
#make docs

%Install
%{__rm} -rf $RPM_BUILD_ROOT
%{__mkdir} -p $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

install -d $RPM_BUILD_ROOT/etc/sysconfig/daemons
install etc/sysconfig/daemons/owcimomd $RPM_BUILD_ROOT/etc/sysconfig/daemons
install -d $RPM_BUILD_ROOT/etc/rc.d/init.d
install etc/init/owcimomd $RPM_BUILD_ROOT/etc/rc.d/init.d
install -d $RPM_BUILD_ROOT/etc/pam.d
install etc/pam.d/openwbem $RPM_BUILD_ROOT/etc/pam.d

install -d $RPM_BUILD_ROOT/%{prefix}/lib/openwbem/c++providers
install -d $RPM_BUILD_ROOT/${localstatedir}/openwbem

#fix /usr/lib/libowservicehttp.so since RPM can't.
rm -f $RPM_BUILD_ROOT/%{prefix}/lib/libowservicehttp.so
ln -s %{prefix}/lib/openwbem/services/libowservicehttp.so $RPM_BUILD_ROOT/%{prefix}/lib

#move the man pages into the correct spot
mkdir -p $RPM_BUILD_ROOT/%{_mandir}/
mv $RPM_BUILD_ROOT/%{prefix}/man/* $RPM_BUILD_ROOT/%{_mandir}/

#rm -f $RPM_BUILD_ROOT/%{prefix}/lib/perl5/*/*/perllocal.pod

# Load the schema. 
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$RPM_BUILD_ROOT/%{prefix}/lib \
    $RPM_BUILD_ROOT/%{prefix}/bin/owmofc -c -n root/cimv2 \
    -d $RPM_BUILD_ROOT/%{localstatedir}/openwbem \
    schemas/cim28/CIM_Schema28.mof mof/OpenWBEM_Interop.mof \
    mof/OpenWBEM_Acl1.0.mof


%Clean
%{__rm} -rf $RPM_BUILD_ROOT

%Post
ldconfig
if [ -x /usr/lib/LSB/init-install ]; then
  /usr/lib/LSB/init-install %{daemonname}
elif [ -x /bin/lisa ]; then
  lisa --SysV-init install %{daemonname} S%{startnum} 2:3:4:5 K%{killnum} 0:1:6
elif [ -x /sbin/chkconfig ]; then
  /sbin/chkconfig --add %{daemonname}
else
  for i in 0 1 6
  do
    ln -sf ../init.d/%{daemonname} /etc/rc.d/rc$i.d/K%{killnum}%{daemonname}
  done
  for i in 2 3 4 5
  do
    ln -sf ../init.d/%{daemonname} /etc/rc.d/rc$i.d/S%{startnum}%{daemonname}
  done
fi

# Except SUSE who needs to do this a little differently
if [ -d /etc/init.d ] && [ -d /etc/init.d/rc3.d ]; then
# New SuSE systems are hosed.
  ln -s /etc/rc.d/init.d/%{daemonname} /etc/init.d/%{daemonname}
  for i in 0 1 6
  do
    ln -sf ../%{daemonname} /etc/init.d/rc$i.d/K%{killnum}%{daemonname}
  done
  for i in 2 3 4 5
  do
    ln -sf ../%{daemonname} /etc/init.d/rc$i.d/S%{startnum}%{daemonname}
  done
fi

# create self-signed cert.
umask 077

if [ ! -f /etc/openwbem/hostkey+cert.pem ] ; then
FQDN=`hostname --fqdn`
if [ "x${FQDN}" = "x" ]; then
   FQDN=localhost.localdomain
fi

cat << EOF | sh /etc/openwbem/owgencert > /dev/null 2>&1
--
SomeState
SomeCity
SomeOrganization
SomeOrganizationalUnit
${FQDN}
root@${FQDN}
EOF
fi



%PreUn
# if -e operation, not -U 
if [ "$1" = "0" ]   
then
  # Stop the daemon
  /etc/rc.d/init.d/%{daemonname} stop
  if [ -x /sbin/chkconfig ]; then
    /sbin/chkconfig --del %{daemonname}
  elif [ -x /usr/lib/LSB/init-remove ]; then
    /usr/lib/LSB/init-remove %{daemonname} 
  elif [ -x /bin/lisa ]; then
    lisa --SysV-init remove %{daemonname} $1
  else
    for i in 0 1 6
    do
      rm -f /etc/rc.d/rc$i.d/K%{killnum}%{daemonname}
    done
    for i in 2 3 4 5
    do
      rm -f /etc/rc.d/rc$i.d/S%{startnum}%{daemonname}
    done
  fi

# SuSE systems
  if [ -d /etc/init.d ] && [ -d /etc/init.d/rc3.d ]; then
    rm -f /etc/init.d/%{daemonname}
    for i in 0 1 6
    do
      rm -f /etc/init.d/rc$i.d/K%{killnum}%{daemonname}
    done
    for i in 2 3 4 5
    do
      rm -f /etc/init.d/rc$i.d/S%{startnum}%{daemonname}
    done
  fi
fi

%PostUn
# if -e operation, not -U
if [ "$1" = "0" ]   
then
 ldconfig
fi

%Files
%defattr(-,root,root)
%doc README TODO AUTHORS COPYING ChangeLog INSTALL LICENSE
%doc doc/*
%dir %{prefix}/lib/openwbem
%{prefix}/lib/openwbem/*
%dir %{prefix}/libexec/openwbem
%dir %{prefix}/share/openwbem
%dir %{localstatedir}/openwbem
%dir /etc/openwbem
%config /etc/openwbem/*
%config /etc/sysconfig/daemons/%{daemonname}
%config /etc/pam.d/openwbem
%attr(0755,root,root) /etc/rc.d/init.d/%{daemonname}
%{prefix}/lib/lib*
%{prefix}/bin/*
%{prefix}/sbin/*
%{prefix}/libexec/openwbem/*
%{prefix}/share/openwbem/*
%{_mandir}/man1/*
%{_mandir}/man8/*

%Files devel
%defattr(-,root,root)
%dir %{prefix}/include/openwbem
%{prefix}/include/openwbem/*

#%Files doc
#%defattr(-,root,root)
#%doc docs/*

#%Files perlNPI
#%{prefix}/lib/perl5/site_perl
#%defattr(-,root,root)
#%{_mandir}/man3/*

%ChangeLog


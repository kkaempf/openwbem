# A note about this file.  If a company (maybe yours if you're reading this)
# is going to package up OpenWBEM as part of a commercial product, this
# RPM spec file should be used instead of the generic openwbem.spec file.
# DO NOT use this file as is, please substitute all instances of GENERIC
# with the name of the company.
%define GENERICprefix /opt/GENERIC
%define GENERIClocalstatedir /var/opt/GENERIC
%define GENERICsysconfdir /etc/opt/GENERIC

%define startnum 36
%define killnum 64
%define initname GENERIC-owcimomd
%define owversion 2.9.1
# You can define values like those above. 

# The name is prefixed by either "GENERIC-" or "lsb-GENERIC-" 
# depending on whether the application is certified to be LSB 
# compliant
Name        	: GENERIC-openwbem
# Increment the version every time the source code changes. 
Version     	: %{owversion}
# Increment the release every time the packaging changes 
# (such as a change to this SPEC file), but the version does not
# change.  If the version is incremented, set the release back to 1
#############################################################################
# NOTE: Any time this is built and the resulting RPM(s) are in
# any way different from the previous build, either the Version 
# or the Release must be incremented. 
#############################################################################
Release     	: 1
# You can also use environment variables for these values.  For example, 
# Version  :  %(echo $MAJOR.$MINOR.$MICRO)
# Release  :  %(echo $RELEASE)

Group       	: Administration/System

Summary     	: The OpenWBEM CIMOM

Copyright   	: Vintela/BSD style
Packager    	: support@GENERIC.com
URL         	: http://www.openwbem.org/

# This is necessary to build the RPM as a non-root user. 
BuildRoot   	: %{_tmppath}/%{name}-%{version}

# "yes" is the default, but we put it here explicitly to avoid someone
# setting it to "no"
AutoReqProv		: yes

Source0: openwbem-%{version}.tar.gz

# The following are sub-packages.
%package devel
Group           : Programming/Library
Summary         : Headers files for OpenWBEM
Requires	: GENERIC-openwbem

#%package doc
#Group           : Programming/Library
#Summary         : Documentation files for OpenWBEM

#%package perlNPI
#Group       	: Administration/System
#Summary     	: PerlNPI provider interface for the OpenWBEM CIMOM
#Requires	: GENERIC-openwbem, perl

%description
The OpenWBEM CIMOM

%description devel
Headers files for OpenWBEM

#%description doc
#Documentation files for OpenWBEM

#%description perlNPI
#PerlNPI provider interface for the OpenWBEM CIMOM

%prep

# Untar the sources. 
%setup -n openwbem-%{version}

%build
# If the LD_RUN_PATH environment variable is set at link time, 
# it's value is embedded in the resulting binary.  At run time, 
# The binary will look here first for shared libraries.  This way
# we link against the libraries we want at run-time even if libs
# by the same name are in /usr/lib or some other path in /etc/ld.so.conf
LD_RUN_PATH=/opt/GENERIC/lib
export LD_RUN_PATH

# Use CFLAGS, CXXFLAGS and LDFLAGS to tell the compiler/linker to look 
# under /opt/GENERIC for headers and libs. 
CFLAGS="-I/opt/GENERIC/include" \
 CXXFLAGS="-I/opt/GENERIC/include" \
 LDFLAGS="-L/opt/GENERIC/lib" \
	./configure --with-search-dir=/usr/kerberos \
	--prefix=%{GENERICprefix} \
	--sysconfdir=%{GENERICsysconfdir} \
	--localstatedir=%{GENERIClocalstatedir} \
   --with-perl=no


make -j2
#make docs

%Install
%{__rm} -rf $RPM_BUILD_ROOT
%{__mkdir} -p $RPM_BUILD_ROOT

# Tell 'make install' to install into the BuildRoot
make DESTDIR=$RPM_BUILD_ROOT install

# Here you have to prefix all destination directories with
# $RPM_BUILD_ROOT.  This is necessary to build the RPM as a 
# non-root user.  Note that the -o and -g options of install 
# won't work when the RPM is built as a non-root user.  Instead
# use %defattr and %attr in the %files section. 
install -d $RPM_BUILD_ROOT/etc/sysconfig/daemons
install etc/sysconfig/daemons/owcimomd $RPM_BUILD_ROOT/etc/sysconfig/daemons
install -d $RPM_BUILD_ROOT/etc/init.d
install etc/init/owcimomd $RPM_BUILD_ROOT/etc/init.d/%{initname}
install -d $RPM_BUILD_ROOT/etc/pam.d
install etc/pam.d/openwbem $RPM_BUILD_ROOT/etc/pam.d

#install -d $RPM_BUILD_ROOT/%{GENERICprefix}/lib/openwbem/c++providers
install -d $RPM_BUILD_ROOT/${GENERIClocalstatedir}/openwbem

#fix /usr/lib/libowservicehttp.so since RPM can't.
#rm $RPM_BUILD_ROOT/%{GENERICprefix}/lib/libowservicehttp.so
#ln -s %{GENERICprefix}/lib/openwbem/services/libowservicehttp.so $RPM_BUILD_ROOT/%{GENERICprefix}/lib



%Clean
%{__rm} -rf $RPM_BUILD_ROOT

%Post
# put /opt/GENERIC/lib in ld.so.conf
if ! grep -q '^/opt/GENERIC/lib$' /etc/ld.so.conf; then
	echo "/opt/GENERIC/lib" >> /etc/ld.so.conf
fi
# Any RPM that installs a shared library into any directory
# listed in /etc/ld.so.conf (or into /usr/lib if it hasn't been
# fixed yet to install into the correct FHS compliant paths) 
# must run ldconfig in its post install script. 
ldconfig

if [ -x /usr/lib/lsb/install_initd ]; then
/usr/lib/lsb/install_initd /etc/init.d/%{initname}
elif [ -x /sbin/chkconfig ]; then
/sbin/chkconfig --add %{initname}
else
for i in 0 1 6
do
  ln -sf ../init.d/%{initname} /etc/rc.d/rc$i.d/K%{killnum}%{initname}
done
for i in 2 3 4 5
do
  ln -sf ../init.d/%{initname} /etc/rc.d/rc$i.d/S%{startnum}%{initname}
done
fi


%PreUn
# if -e operation, not -U 
if [ "x$1" = "x0" ]   
then
  # Stop the daemon
  /etc/init.d/%{initname} stop
  if [ -x /usr/lib/lsb/remove_initd ]; then
    /usr/lib/lsb/remove_initd %{initname} 
  elif [ -x /sbin/chkconfig ]; then
    /sbin/chkconfig --del %{initname}
  else
    for i in 0 1 6
    do
      rm -f /etc/rc.d/rc$i.d/K%{killnum}%{initname}
    done
    for i in 2 3 4 5
    do
      rm -f /etc/rc.d/rc$i.d/S%{startnum}%{initname}
    done
  fi
fi

%PostUn
ldconfig

%Files
%defattr(-,root,root)
%doc PORTING.txt README TODO 
%doc *.HOWTO AUTHORS COPYING ChangeLog INSTALL LICENSE NEWS
%doc openwbem-faq.html
%dir %{GENERICprefix}/lib/openwbem
%{GENERICprefix}/lib/openwbem/*
%dir %{GENERICprefix}/libexec/openwbem
%dir %{GENERICprefix}/share/openwbem
%dir %{GENERIClocalstatedir}/openwbem
%config %{GENERICsysconfdir}/*
#%config /etc/sysconfig/daemons/%{daemonname}
%config /etc/pam.d/openwbem
%attr(0755,root,root) /etc/init.d/%{initname}
%{GENERICprefix}/lib/lib*
%{GENERICprefix}/bin/*
%{GENERICprefix}/sbin/*
%{GENERICprefix}/libexec/openwbem/*
%{GENERICprefix}/share/openwbem/*

# Files for sub-packages. 
%Files devel
%defattr(-,root,root)
%dir %{GENERICprefix}/include/openwbem
%{GENERICprefix}/include/openwbem/*

#%Files doc
#%defattr(-,root,root)
#%doc docs/*

#%Files perlNPI
#%{GENERICprefix}/lib/perl5/site_perl
#%defattr(-,root,root)
#%{GENERICprefix}/share/man/man3

%ChangeLog


###############################################################################
# owcimomd configuration file
# Note:
#	All lines that start with a '#' or a ';' character are ignored.
#
#	All of the options in this file are read by owcimomd at start up. The options
#	that are prefixed with "owcimomd." are meant specifically for owcimomd.
#	Other options are prefixed with an identifier of the component that is
#	specifically interested in the options. For example the
#  "cppprovifc.prov_location" option is meant specifically for the C++ provider
#	interface. This options is read from the config file by owcimomd and
#	made available to the C++ provider interface upon request.
###############################################################################

################################################################################
# 2.0
# owcimomd.services_path Specifies the directory where all services
# will be loaded by the CIMOM
owcimomd.services_path = @libdir@/openwbem/services

################################################################################
# 2.0
# owcimomd.request_handler_path Specifies the location where all request
# handlers will be loaded by the CIMOM
owcimomd.request_handler_path = @libdir@/openwbem/requesthandlers

################################################################################
# owcimomd.libexecdir specifies the locaction of the libexec directory.
# Binaries that owcimomd relies on are expected to be in this directory
# the default for is option is "/usr/local/libexec/openwbem"
owcimomd.libexecdir = @libexecdir@/openwbem/

################################################################################
# owcimomd.owlibdir specifies the locaction of the lib directory.
# Dynamicall loaded Libraries that owcimomd relies on are expected to be in
# this directory.
# The default for is option is "/usr/local/lib/openwbem"
owcimomd.owlibdir = @libdir@/openwbem/

################################################################################
# owcimomd.datadir specifies the directory where owcimomd will place its data
# file (repositories). The default for this options is "/var/lib/openwbem"
owcimomd.datadir = @localstatedir@/openwbem/

################################################################################
# If owcimomd.allow_anonymous is set to true anonymous logins are allowed by
# owcimomd (i.e. No user name or password is required).
;owcimomd.allow_anonymous = true

################################################################################
# owcimomd.log_location specifies the location of the log file that is
# generated by owcimomd. If this option is set to syslog, owcimomd will log
# all messages through the syslog interface (Recommended). If this option
# is set to anything else, it is assumed it is an absolute path to a file
# that owcimomd will write its log messages to. The default for this option
# is syslog.
;owcimomd.log_location = syslog

################################################################################
# The owcimomd.log_level option specifies the level of logging. This can be
# any one of the following:
#		"debug"		- All debug, custinfo and error messages are logged
#		"custinfo"	- All custinfo and error messages are logged
#		"error"		- Only error messages are logged. This is the default.
;owcimomd.log_level = error

################################################################################
# owcimomd.provider_ifc_libs specifies the location where all the provider
# interfaces will be loaded from. owcimomd assumes all shared libraries in
# this directory are provider interfaces. If a shared library in this directory
# does not support the provider interface api, it will be rejected. The default
# for this option is "/usr/local/lib/openwbem/provifcs"
owcimomd.provider_ifc_libs = @libdir@/openwbem/provifcs

################################################################################
# owcimomd.wql_lib specifies the location where the wql processor library
# will be loaded from.
# The default for this option is "/usr/local/lib/libowwql.so"
owcimomd.wql_lib = @libdir@/libowwql.so

################################################################################
# The owcimomd.dump_socket_io defines the directory where owcimomd will
# dump all socket i/o to log files.  This is usefull for debugging HTTP
# operations and XML.  The files will be called <dir>/owSockDumpIn and
# <dir>/owSockDumpOut.  If this option is not set, or has an empty value,
# dump files will not be used.  Warning: if this option is defined,
# CIM operattions could take twice as long!
;owcimomd.dump_socket_io = /tmp

################################################################################
# The owcimomd.debugflag specifies whether or not owcimomd will run in
# debug mode. If this option is true, owcimomd will not detache from the
# terminal and will send all logging to the terminal. While in this mode
# owcimomd can be properly terminated by simply hitting Ctrl-C or sending it
# a SIGTERM signal (SIGTERM also works if owcimomd.debugflag is false)
;owcimomd.debugflag = false

################################################################################
# 2.0
# the authentication module to be used by owcimomd.  This should be a
# an absolute path to the shared library containing the authentication module.
owcimomd.authentication_module = @libdir@/openwbem/authentication/libpamauthentication.so

################################################################################
# a space delimited list of system users who are allowed to acces the CIMOM
pam.allowed_users = root

################################################################################
# If the simple authentication module is used, this needs to be the path to
# the password file
simple_auth.password_file = @sysconfdir@/openwbem/simple_auth.passwd

################################################################################
# When this variable is set to true, the cimom will not attempt to
# deliver indications.  The default value is false.
;owcimomd.disable_indications = false

################################################################################
# owcimomd.ACL_superuser specifies the user name of the user that has access to
# all CIM data in all namespaces maintained by the CIMOM. This user can be used
# to administer the /root/security namespace which is where all ACL user rights
# are stored. ACL processing is not enabled until the OpenWBEM_Acl1.0.mof file
# has been imported.
;owcimomd.ACL_superuser =

################################################################################
# owcimomd.request_handler_TTL specifies how many minutes the request 
# handlers will stay loaded after they are accessed.  If the value of this
# option is -1, the request handlers will never be unloaded.  The default
# value is 5 minutes. 
;owcimomd.request_handler_TTL = 5

################################################################################
# One of the provider interfaces provided with owcimomd is the C++ provider
# interface. The cppprovifc.prov_location option specifies where the C++
# provider interface will load it's providers from. The default for this option
# is "/usr/local/lib/openwbem/c++providers"
cppprovifc.prov_location = @libdir@/openwbem/c++providers

################################################################################
# cppprovifc.prov_TTL specifies how many minutes the C++ provider manager
# will keep a provider in memory.  If a provider has not been accessed for 
# longer than this value, it will be unloaded and the memory associated with
# it will be freed.  If the value of this option is -1, the providers will 
# never be unloaded.  The default value is 5 minutes. 
;cppprovifc.prov_TTL = 5

################################################################################
# One of the provider interfaces provided with owcimomd is the NPI provider
# interface. The npiprovifc.prov_location option specifies where the NPI
# provider interface will load it's providers from. The default for this option
# is "/usr/local/lib/openwbem/npiproviders"
npiprovifc.prov_location = @libdir@/openwbem/npiproviders

################################################################################
# If OpenWBEM is built with zlib, it can use deflate compression in the HTTP
# responses.  This option controls whether it will acutally use it or not.
# The default is true.
;http_server.enable_deflate = true

################################################################################
# 2.0
# http_server.disable.slp - If set to "true" and the HTTP service has been
# configured to do slp registration, the slp registration will never take place.
# If the HTTP service was not configured with slp registration, this config
# item it ignored.
# The default is false
;http_server.disable.slp = false

################################################################################
# http_server.http_port option specifies the port number owcimomd will listen 
# on for all HTTP communications. The default for this option is 5988.
# Set this to -1 if you do not want to support HTTP connections (for
# instance, you only want to support HTTPS connections).  If a value of 0
# is given, a port will be dynamically assigned at run-time.
;http_server.http_port = 5988

################################################################################
# http_server.https_port specifies the port number owcimomd will listen on 
# for all HTTPS communications. The default for this option is 5989
# Set this to -1 if you do not want to support HTTPS connections.
# If a value of 0 is given, a port will be dynamically assigned at run-time.
;http_server.https_port = 5989

################################################################################
# http_server.max_connections specifies the maximum number of concurrent
# connections owcimomd will handle. The default for this option is 30
;http_server.max_connections = 30

################################################################################
# http_server.SSL_cert specifies the location of the file that contains the
# host's private key and certificate that will be used by Open SSL for HTTPS
# communications. The default for this option is
# /etc/ssl/private/hostkey+cert.pem
http_server.SSL_cert = /etc/ssl/private/hostkey+cert.pem

################################################################################
# Tell the http server to use digest authorization
http_server.use_digest = true

################################################################################
# If the digest authentication module is used, this needs to be the path to
# the password file
http_server.digest_password_file = @sysconfdir@/openwbem/digest_auth.passwd

################################################################################
# http_server.single_thread specifies whether or not owcimomd process connection
# in a separate thread or in the same thread as the server. This options is
# really only for debug purposes and should not be of any use to the
# typical user. The default for this option is false.
;http_server.single_thread = false

################################################################################
# http_server.use_UDS specifies whether the http server will listen on a 
# Unix Domain Socket.  The default value is true.
;http_server.use_UDS = true


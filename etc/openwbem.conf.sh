###############################################################################
# owcimomd configuration file
# Note:
#  All lines that start with a '#' or a ';' character are ignored.
#
#  All of the options in this file are read by owcimomd at start up. The file
#  will only be re-read on restart when a SIGHUP is received.
#  The options that are prefixed with "owcimomd." are meant specifically for
#  owcimomd.
#  Other options are prefixed with an identifier of the component that is
#  specifically interested in the options. For example the
#  "cppprovifc.prov_location" option is meant specifically for the C++ provider
#  interface. This option is read from the config file by owcimomd and
#  made available to the C++ provider interface upon request.
#  Config options may be specified more than once. If the value is singular,
#  The last read option will take effect. If the value is plural, all values
#  together will be in effect.
###############################################################################

################################################################################
# For each directory specified, all the files contained in the directory,
# whose names end in ".conf", will be loaded and processed as additional config
# files.
# This is a multi-valued option. ':' (windows) or ';' (POSIX) is the separator.
# This option will be evaluated after the main config file is parsed, and so
# additional directories specified in additional config files will not be 
# examined.
# The default is "@sysconfdir@/openwbem/openwbem.conf.d"
owcimomd.additional_config_files_dirs = @sysconfdir@/openwbem/openwbem.conf.d

################################################################################
# If owcimomd.allow_anonymous is set to true, anonymous logins are allowed by
# owcimomd, authentication is disabled, and no user name or password is required.
# The default is false
owcimomd.allow_anonymous = false

################################################################################
# log.main.type specifies the type of the main log used by owcimomd. If this
# option is set to:
# "syslog" - owcimomd will log all messages through the syslog interface
#            (Recommended).
# "null"   - logging will be disabled completely.
# "file"   - log messages will be written to the file identified by the
#            log.main.location config item.
# The default is "syslog"
log.main.type = syslog

################################################################################
# log.main.identity specifies the syslog identity.
# This option only is applicable if the log type == "syslog"
log.main.identity = @PACKAGE_PREFIX@owcimomd

################################################################################
# log.main.facility specifies the syslog facility.
# This option only is applicable if the log type == "syslog"
log.main.facility = daemon

################################################################################
# log.main.location specifies the location of the log file
# (if the type == "file".)
;log.main.location =

################################################################################
# An unsigned integer. The log file's maximum size in KB. 0 means unlimited.
# The default is "0"
;log.main.max_file_size = 0

################################################################################
# An unsigned integer. Determines how many backup files are kept before the
# oldest is erased. If set to 0, no backup files will be made and the log file
# will be truncated when it reaches max_file_size.
# Default is "1"
;log.main.max_backup_index = 1

################################################################################
# Determines whether each log messages is flushed to the log when it happens.
# Set this option to false if you wish to increase logging performance.
# This optional is only applicable to logs of type "file".
# Default is "true"
;log.main.flush = true

################################################################################
# A space delimited list of the components the log will output.
# Components used in owcimomd:
#   owcimomd
#
# Providers may use their own components.
# "*" means all components.
# The default is "*"
log.main.components = *

################################################################################
# A space delimited list of the categories the log will output.
# Categories used in owcimomd:
#   FATAL
#   ERROR
#   INFO
#   DEBUG
#
# "*" means all categories.
# If specified in this item, the predefined categores are not treated as
# levels, but as independent categories. No default is available, and if not
# set, no categories will be logged, and the level config item will be used.
;log.main.categories = *

################################################################################
# The log.main.level option specifies the level of logging. This can be
# any one of the following:
#   "FATAL" - Only fatal error messages are logged.
#   "ERROR" - All error and fatal error messages are logged. This is the default.
#   "INFO"  - All info, error and fatal error messages are logged
#   "DEBUG" - All debug, info, error and fatal error messages are logged
# If set, the log will output all predefined categories at and above the
# specified level.
# The default is "ERROR"
log.main.level = ERROR

################################################################################
# Specifies the format of the log messages. Text mixed with printf() style
# conversion specifiers.
# Available conversion specifiers:
# %c - The component (e.g. owcimomd)
# %d - The date. May be followed by a date format specifier enclosed between
#      braces. For example, %d{%H:%M:%S} or %d{%d %b %Y %H:%M:%S}. If no date
#      format specifier is given then ISO8601 format is assumed.
#      For more information of the date format specifiers, lookup the
#      documentation for the strftime() function found in the <ctime> header.
#      The only addition is %Q, which is the number of milliseconds.
# %F - The file name
# %l - The filename and line number. e.g. file.cpp(100)
# %L - The line number
# %M - The method name where the logging request was issued (only works on
#      C++ compilers which support __PRETTY_FUNCTION__ or C99's __func__)
# %m - The message
# %e - The message as XML CDATA. This includes the "<![CDATA[" and ending "]]>"
# %n - The platform dependent line separator character (\n) or characters (\r\n)
# %p - category, aka level, aka priority
# %r - The number of milliseconds elapsed since the start of the application
#      until the creation of the logging event
# %t - Thread id
# %P - Process id
# %% - %
# \n - newline
# \t - tab
# \r - linefeed
# \\ - \
# \x<hexDigits> - The character represented in hexadecimal.
#
#   It is possible to change the minimum field width, the maximum field width
# and justification. The optional format modifier is placed between the
# percent sign and the conversion character.
#   The first optional format modifier is the left justification flag which is
# the minus (-) character. The optional minimum field width modifier follows.
# It is an integer that represents the minimum number of characters to
# output. If the data item requires fewer characters, it is padded with
# spaces on either the left or the right, according to the justification
# flag. If the data item is larger than the minimum field width, the field
# is expanded to accommodate the data.
#   The maximum field width modifier is designated by a period followed by a
# decimal constant. If the data item is longer than the maximum field, then
# the extra characters are removed from the beginning of the data item
# (by default), or from the end if the left justification flag was specified.
#
# Examples:
#   Log4j TTCC layout:
#   "%r [%t] %-5p %c - %m"
#
#   Similar to TTCC, but with some fixed size fields:
#   "%-6r [%15.15t] %-5p %30.30c - %m"
#
#   XML output conforming to log4j.dtd 1.2 which can be processed by Chainsaw.
#   If used, this has to be on one line, it's split up here for readability.
#   "<log4j:event logger="%c" timestamp="%d{%s%Q}" level="%p" thread="%t">
#    <log4j:message>%e</log4j:message>
#    <log4j:locationInfo class="" method="" file="%F" line="%L"/>
#    </log4j:event>"
#
# The default is "[%t]%m"
log.main.format = [%t %-5p %c] %m

################################################################################
# If owcimomd is run in debug mode, then the debug log will be active.
log.debug.type = stderr
log.debug.components = *
;log.debug.categories = *
log.debug.level = DEBUG
log.debug.format = [%-5r %t %-5p %c %F:%L] %m

# Color version using ascii escape codes
;log.debug.format = \x1b[1;37;40m[\x1b[1;31;40m%-.6t\x1b[1;37;40m]\x1b[1;32;40m %m\x1b[0;37;40m

# More ascii colors:
# red:        \x1b[1;31;40m
# darkRed     \x1b[0;31;40m
# green       \x1b[1;32;40m
# darkGreen   \x1b[0;32;40m
# yellow      \x1b[1;33;40m
# darkYellow  \x1b[0;33;40m
# blue        \x1b[1;34;40m
# darkBlue    \x1b[0;34;40m
# purple      \x1b[1;35;40m
# darkPurple  \x1b[0;35;40m
# cyan        \x1b[1;36;40m
# darkCyan    \x1b[0;36;40m
# white       \x1b[1;37;40m
# darkWhite   \x1b[0;37;40m
# gray        \x1b[0;37;40m
# resetColor  \x1b[0;37;40m


################################################################################
# A space separated list of names for each additional log to configure.  For
# each log name, the following config items will apply:
# log.<log name>.components
# log.<log name>.categories
# log.<log name>.level
# log.<log name>.format
# log.<log name>.location
# log.<log name>.max_file_size
# log.<log name>.max_backup_index
# log.<log name>.flush
#
# This is a multi-valued option. Whitespace is the separator.
#
;owcimomd.additional_logs =

################################################################################
# These settings specify the logging used by all instances of the
# monitor, and have the same meanings as regular logger settings.
#
monitor.log.location = @localstatedir@/owcimomd.monitor.log
monitor.log.max_file_size = 8000
monitor.log.max_backup_index = 1
monitor.log.categories = INFO ERROR FATAL
monitor.log.format = [%P %d %-5p %c] %m


################################################################################
# owcimomd.wql_lib specifies the location where the wql processor library
# will be loaded from.
# To disable WQL, either set this option to empty or comment it out.
# The default is "@libdir@/libowwql.@LIB_EXT@"
owcimomd.wql_lib = @libdir@/libowwql.@LIB_EXT@

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
# Also using the command line parameter -d turns on debug mode.
;owcimomd.debugflag = false

################################################################################
# The authentication module to be used by owcimomd for HTTP Basic 
# authentication.  This should be an absolute path to the shared library 
# containing the authentication module.
# If empty, Basic authentication will be disabled.
owcimomd.authentication_module = @libdir@/openwbem/authentication/libpamclauthentication.@LIB_EXT@

################################################################################
# The maximum number of classes that will be cached by the cimom.
# The default is 128
owcimomd.max_class_cache_size = 128

################################################################################
# A space delimited list of system users who are allowed to access the CIMOM
# The special value * to allow all users to authenticate (for instance, if
# you choose to control access with ACLs instead).
# This option is enforced for all authentication methods. If
# owcimomd.allow_anonymous = true, it is not enforced.
# This is a multi-valued option. Whitespace is the separator.
# The default is *
owcimomd.allowed_users = root

################################################################################
# If the simple authentication module is used, this needs to be the path to
# the password file
simple_auth.password_file = @sysconfdir@/openwbem/simple_auth.passwd

################################################################################
# When this variable is set to true, the cimom will not attempt to
# deliver indications.
# The default is false
owcimomd.disable_indications = false

################################################################################
# A space-separated list of namespaces where providers won't be registerd
# for classes if no namespace was specified during provider self-registration.
# If providers use self-registration, and don't specify a namespace (only 
# a class name), they are registered for all namespaces in which the class
# is present by default.  This option allows the specified namespaces to be 
# excluded from this behavior.  
# This is a multi-valued option. Whitespace is the separator.
# Example: owcimomd.explicit_registration_namespaces = root/private root/cache
;owcimomd.explicit_registration_namespaces = 

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
# option is -1, the request handlers will never be unloaded.
# The default is 5
owcimomd.request_handler_TTL = 5

################################################################################
# owcimomd.check_referential_integrity specifies whether the CIM Repository
# will perform extra checks to help ensure referential integrity between
# associations and propagated keys.  Since these checks can add overhead,
# it is recommended to enable this option during development and debugging
# and disable it in production if the overhead is too expensive.
# The default is false
owcimomd.check_referential_integrity = false

################################################################################
# owcimomd.polling_manager_max_threads specifies the maximum number of
# concurrent threads that will be allowed to run by the polling manager.
# This should only be a concern if the cimom has a lot of polled providers.
# The default is 256
owcimomd.polling_manager_max_threads = 256

################################################################################
# owcimomd.max_indication_export_threads specifies the maximum number of
# concurrent threads to export indications that will be allowed to run by the
# indication server.
# This may need to be increased if a lot of indications are being exported.
# The default is 30
owcimomd.max_indication_export_threads = 30

################################################################################
# owcimomd.restart_on_error controls the use of the fatal signal handler.
# If this option is set to true, if owcimomd receives a fatal signal
# (SIGSEGV, SIGBUS, SIGILL, SIGFPE or SIGABRT) which may be caused by faulty
# code, then it will restart itself.  Note that this feature is disabled if
# the cimom is built in debug mode (OW_DEBUG is defined).
# This feature helps to keep the cimom running in the case a buggy provider
# (or the cimom itself) causes a fatal signal.
# The default is true
owcimomd.restart_on_error = true

################################################################################
# owcimomd.authorization_lib specifies the location the authorization
# library will be loaded from.
# There are 2 authorization interfaces, either one can be used. 
# This is the original authorization interface.  The new interface
# (Authorizer2IFC) uses the owcimomd.authorization2_lib configuration 
# item.  If both authorization options are empty or commented out, no
# authorization module is used. 
;owcimomd.authorization_lib = @libdir@/openwbem/libowsimpleauthorizer.@LIB_EXT@
;owcimomd.authorization_lib = @libdir@/openwbem/libowbasicauthorizer.@LIB_EXT@

################################################################################
# owcimomd.authorization2_lib specifies the location the authorization
# library will be loaded from.
# There are 2 authorization interfaces, either one can be used. 
# This is the new authorization interface.  The old interface
# uses the owcimomd.authorization_lib configuration 
# item.  If both authorization options are empty or commented out, no
# authorization module is used. 
;owcimomd.authorization2_lib = @libdir@/openwbem/libowsimpleauthorizer2.@LIB_EXT@

################################################################################
# owcimomd.interop_schema_namespace specifies the namespace which contains the
# CIM Interop schema.
# The default is root
owcimomd.interop_schema_namespace = root

################################################################################
# owcimomd.privileges_config_dir specifies the directory containing the 
# privileges file(s) the owcimomd monitor will use. The privileges file for
# owcimomd must be named owcimomd. Other privileges files must be places in the
# same directory.
owcimomd.privileges_config_dir = @sysconfdir@/openwbem/privileges

################################################################################
# cppprovifc.prov_TTL specifies how many minutes the C++ provider manager
# will keep a provider in memory.  If a provider has not been accessed for
# longer than this value, it will be unloaded and the memory associated with
# it will be freed.  If the value of this option is -1, the providers will
# never be unloaded.
# The default is 5
cppprovifc.prov_TTL = 5

################################################################################
# The remote provider interface uses a connection pool to re-use remote
# connections. remoteprovifc.max_connections_per_url specifies the maximum
# number of connections per url that will be pooled.  The value must be a
# unsigned integer.
# The default is 5
remoteprovifc.max_connections_per_url = 5

################################################################################
# If OpenWBEM is built with zlib, it can use deflate compression in the HTTP
# responses.  This option controls whether it will actually use it or not.
# Performance tests on a 100Mbit lan have shown that enabling deflate is slower 
# than if it is disabled.
# The default is true
http_server.enable_deflate = false

################################################################################
# http_server.listen_addresses option specifies the local addresses to listen
# on.  The option is a space delimited list.  Each item is either a hostname
# or an IP address.  The value 0.0.0.0 means to listen on all local addresses.
# This is a multi-valued option. Whitespace is the separator.
# The default is 0.0.0.0
http_server.listen_addresses = 0.0.0.0

################################################################################
# http_server.http_port option specifies the port number owcimomd will listen
# on for all HTTP communications.
# Set this to -1 if you do not want to support HTTP connections (for
# instance, you only want to support HTTPS connections).  If a value of 0
# is given, a port will be dynamically assigned at run-time.
# The default is 5988
http_server.http_port = -1

################################################################################
# http_server.https_port specifies the port number owcimomd will listen on
# for all HTTPS communications.
# Set this to -1 if you do not want to support HTTPS connections.
# If a value of 0 is given, a port will be dynamically assigned at run-time.
# The default is 5989
http_server.https_port = 5989

################################################################################
# http_server.max_connections specifies the maximum number of concurrent
# connections owcimomd will handle.
# The default is 30
http_server.max_connections = 30

################################################################################
# http_server.SSL_cert specifies the location of the file that contains the
# host's certificate that will be used by Open SSL for HTTPS communications.
http_server.SSL_cert = @sysconfdir@/openwbem/servercert.pem

################################################################################
# http_server.SSL_key specifies the location of the file that contains the
# host's private key that will be used by Open SSL for HTTPS communications.
# The default is the value of the http_server.SSL_cert option (meaning that
# both the private key and the certificate are in the same file).
http_server.SSL_key = @sysconfdir@/openwbem/serverkey.pem

################################################################################
# Specify the authentication mechanism the server will prompt clients to use.
# See RFC 2617 for more details.
# Valid values are: "Basic", "Digest", "Negotiate" (SPNEGO)
# The default is Basic
http_server.default_auth_challenge = Basic

################################################################################
# Tell the http server to allow Basic authentication
# You must configure an authentication module by setting 
# owcimomd.authentication_module
http_server.allow_basic_authentication = true

################################################################################
# Tell the http server to allow Digest authentication
# You must set up the digest password file specified by 
# http_server.digest_password_file using owdigestgenpass to use digest.
# Digest doesn't use the authentication module specified by the
# owcimomd.authentication_module config item.
http_server.allow_digest_authentication = false

################################################################################
# The realm to be sent with HTTP Basic or Digest authentication 
# challenges.  If this is not set, the hostname will be used. 
;http_server.authentication_realm = 

################################################################################
# Tell the http server to allow SPNEGO authentication
http_server.allow_spnego_authentication = false

################################################################################
# If the Digest authentication option is enabled, this needs to be the path to
# the password file.
http_server.digest_password_file = @sysconfdir@/openwbem/digest_auth.passwd

################################################################################
# Tell the http server to allow local authentication
# local authentication allows a local system user to authenticate without
# supplying a password, instead relying on filesystem permissions.  This can
# be enabled in conjunction with either http Basic or Digest.
# The default is false
http_server.allow_local_authentication = false

################################################################################
# Determines if the server should attempt to authenticate clients
# with SSL Client Certificate verification.
# disabled: no client certificate checking will take place.
# optional: A trusted cert is authenticated (no HTTP auth necessary)
#   An untrusted cert still passes the SSL handshake, but the client
#   will have to pass HTTP authentication.
# autoupdate: Same as optional, but previously unknown client certificates
#   which pass HTTP authentication are added to the trust store, so that
#   subsequent client connections with the same certificate won't
#   require HTTP authentication.
# required: A trusted cert is required for the the SSL handshake to
#   succeed.
# The default is disabled
http_server.ssl_client_verification = disabled

################################################################################
# Specify the directory containing the OpenSSL trust store.
# The default is "@sysconfdir@/openwbem/truststore"
http_server.ssl_trust_store = @sysconfdir@/openwbem/truststore

################################################################################
# http_server.single_thread specifies whether or not owcimomd process connection
# in a separate thread or in the same thread as the server. This option is
# really only for debug purposes and should not be of any use to the
# typical user.
# The default is false
http_server.single_thread = false

################################################################################
# http_server.use_UDS specifies whether the http server will listen on a
# Unix Domain Socket.
# The default is true
http_server.use_UDS = true

################################################################################
# http_server.reuse_addr specifies whether the http server will set the
# SO_REUSEADDR flag when it listens on a socket.  This is provided because some
# Unix kernels have security problems when this option is set.
# If this option is not turned on, you may not be able to immediately restart
# the daemon because it can't listen on the port until the kernel releases it.
# It's OK to use on current linux versions.  Definitely not on
# OLD (kernel < 1.3.60) ones.
# See http://monkey.org/openbsd/archive/misc/9601/msg00031.html
# or just google for "bind() Security Problems"
# If you specify interfaces to listen on other than 0.0.0.0 using the
# http_server.listen_addresses option, then there is no security problem with
# enabling this option.
# The default is true
http_server.reuse_addr = true

################################################################################
# http_server.timeout specifies the number of seconds that the server will use
# as a timeout when communicting with clients. The value must be > 0.
# The default is 300 (5 minutes).
http_server.timeout = 300

################################################################################
# http.default_content_language specified the default content-laguage that
# will be returned to an HTTP client when the accept-language header was used
# on the request and a provider has not explicity set the content-language.
# The default is en
http_server.default_content_language = en

################################################################################
# slp.enable_advertisement controls whether the slp provider advertises with SLP
# The default is true
slp.enable_advertisement = true

################################################################################
# The following options will probably not need to be modified.
################################################################################


################################################################################
# owcimomd.services_path Specifies the directory containing the services
# shared libraries to be loaded by the CIMOM.
# This is a multi-valued option. ':' (windows) or ';' (POSIX) is the separator.
# The default is "@libdir@/openwbem/services"
owcimomd.services_path = @libdir@/openwbem/services

################################################################################
# owcimomd.request_handler_path Specifies the directory containing the
# request handler shared libraries to be loaded by the CIMOM.
# This is a multi-valued option. ':' (windows) or ';' (POSIX) is the separator.
# The default is "@libdir@/openwbem/requesthandlers"
owcimomd.request_handler_path = @libdir@/openwbem/requesthandlers

################################################################################
# owcimomd.libexecdir specifies the locaction of the libexec directory.
# Binaries that owcimomd relies on are expected to be in this directory.
# The default is "@libexecdir@/openwbem"
owcimomd.libexecdir = @libexecdir@/openwbem

################################################################################
# owcimomd.owlibdir specifies the locaction of the lib directory.
# Dynamically loaded libraries that owcimomd relies on are expected to be in
# this directory.
# The default is "@libdir@/openwbem"
owcimomd.owlibdir = @libdir@/openwbem

################################################################################
# owcimomd.datadir specifies the directory containing the CIM repository
# The default is "@localstatedir@/openwbem/repository"
owcimomd.datadir = @localstatedir@/openwbem/repository

################################################################################
# owcimomd.provider_ifc_libs specifies the locations where all the provider
# interfaces will be loaded from. owcimomd assumes all shared libraries in
# these directories are provider interfaces. If a shared library in this directory
# does not support the provider interface api, it will be rejected.
# This is a multi-valued option. ':' (windows) or ';' (POSIX) is the separator.
# The default is "@libdir@/openwbem/provifcs"
owcimomd.provider_ifc_libs = @libdir@/openwbem/provifcs

################################################################################
# One of the provider interfaces provided with owcimomd is the C++ provider
# interface. The cppprovifc.prov_location option specifies where the C++
# provider interface will load it's providers from.
# This is a multi-valued option. ':' (windows) or ';' (POSIX) is the separator.
# The default is "@libdir@/openwbem/c++providers"
cppprovifc.prov_location = @libdir@/openwbem/c++providers

################################################################################
# http_server.uds_filename specifies the name of the unix domain socket the
# http server will listen on.
# The default is /tmp/OW@LCL@APIIPC_72859_Xq47Bf_P9r761-5_J-7_Q@PACKAGE_PREFIX@
http_server.uds_filename = /tmp/OW@LCL@APIIPC_72859_Xq47Bf_P9r761-5_J-7_Q@PACKAGE_PREFIX@

################################################################################
# One of the provider interfaces provided with owcimomd is the NPI provider
# interface. The npiprovifc.prov_location option specifies where the NPI
# provider interface will load it's providers from.
# This is a multi-valued option. ':' (windows) or ';' (POSIX) is the separator.
# The default is "@libdir@/openwbem/npiproviders"
npiprovifc.prov_location = @libdir@/openwbem/npiproviders

################################################################################
# One of the provider interfaces provided with owcimomd is the CMPI provider
# interface. The cmpiprovifc.prov_location option specifies where the CMPI
# provider interface will load it's providers from.
# This is a multi-valued option. ':' (windows) or ';' (POSIX) is the separator.
# The default is "@libdir@/openwbem/cmpiproviders"
cmpiprovifc.prov_location = @libdir@/openwbem/cmpiproviders

################################################################################
# cmpiprovifc.prov_TTL specifies how many minutes the CMPI provider manager
# will keep a provider in memory.  If a provider has not been accessed for
# longer than this value, it will be unloaded and the memory associated with
# it will be freed.  If the value of this option is -1, the providers will
# never be unloaded.
# The default is -1
cmpiprovifc.prov_TTL = -1

################################################################################
# One of the provider interfaces provided with owcimomd is the perl provider
# interface. The perlprovifc.prov_location option specifies where the perl
# provider interface will load it's providers from.
# This is a multi-valued option. ':' (windows) or ';' (POSIX) is the separator.
# The default is "@libdir@/openwbem/perlproviders"
perlprovifc.prov_location = @libdir@/openwbem/perlproviders

################################################################################
# The default built in provider interface is loaded and initialized by default.
# Set this option to true to disable it.
# The default is "false"
owcimomd.disable_cpp_provider_interface = false

################################################################################
# Specify the location of the owcimomd pidfile
# The default is "@PIDFILE_DIR@/@PACKAGE_PREFIX@owcimomd.pid"
owcimomd.pidfile = @PIDFILE_DIR@/@PACKAGE_PREFIX@owcimomd.pid

################################################################################
# Specify the time in seconds to use for the timeout when requests are waiting
# to acquire the owcimomd read/write lock. The syntax for this value is a
# floating point in the format accepted by ANSI C strtod().
# The default is 300
owcimomd.read_write_lock_timeout = 300

################################################################################
# The following options are deprecated
################################################################################


################################################################################
# THIS OPTION IS DEPRECATED.  Use owcimomd.allowed_users instead.
# A space delimited list of system users who are allowed to access the CIMOM.
# This option is only enforced by the pam authentication module, and has no
# effect if you are not using http Basic authentication together with the
# pam authentication module.
# Set this value to * to allow all users to authenticate (for instance, if
# you choose to control access with ACLs instead).
pam.allowed_users = *

################################################################################
# THIS OPTION IS DEPRECATED - Use log.main.type and log.main.location instead.
# owcimomd.log_location specifies the location of the log file that is
# generated by owcimomd. If this option is set to syslog, owcimomd will log
# all messages through the syslog interface (Recommended). If this option
# is set to null, logging will be disabled completely.  If this option
# is set to anything else, it is assumed it is an absolute path to a file
# that owcimomd will write its log messages to.
# The default is syslog
;owcimomd.log_location = syslog

################################################################################
# THIS OPTION IS DEPRECATED - Use log.main.level or log.main.categories
# The owcimomd.log_level option specifies the level of logging. This can be
# any one of the following:
#   "debug"    - All debug, info, error and fatalerror messages are logged
#   "info"     - All info, error and fatalerror messages are logged
#   "error"    - All error and fatalerror messages are logged. This is the default.
#   "fatalerror" - Only fatalerror messages are logged.
;owcimomd.log_level = error



package Privman;

use 5.006;
use strict;
use warnings;
use Symbol 'qualify_to_ref';

require Exporter;

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use Mytest3 ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	open_handle read_directory rename unlink monitored_spawn user_spawn
	child_stdin child_stdout child_stderr child_status
	running terminated exit_terminated exit_status
	terminated_successfully signal_terminated term_signal
	stopped stop_signal child_wait_close_term
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);

our $VERSION = '0.01';

require XSLoader;
XSLoader::load('Privman', $VERSION);

# Preloaded methods go here.

# <    in
# >    out (or out | trunc; these are the same)
# >>   out | app
# +<   in | out
# +>   in | out | trunc
# +>>  in | out | app

sub open_handle(*$$$) {
	my ($fh, $pathname, $mode, $perms) = @_;
	my $fd = open_descriptor($pathname, $mode, $perms);
	return undef() if $fd < 0;
	open(qualify_to_ref($fh, caller), $mode . "&=", $fd);
}

1;
__END__
# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

Privman - Perl extension for requesting elevated privileges (privilege manager)
from monitor.

=head1 SYNOPSIS

  use Privman;
  use Privman(':all');

=head1 DESCRIPTION

This module should only be used by Perl scripts run via the
PrivilegeManager::monitoredSpawn function.  It gives such Perl scripts the
ability to request from their associated monitor various privileged operations,
just as the PrivilegeManager class does for C++ programs.  The functions
provided parallell those in the PrivilegeManager, Process and Process::Status 
C++ classes:

	open_handle read_directory rename unlink monitored_spawn user_spawn
	child_stdin child_stdout child_stderr child_status
	running terminated exit_terminated exit_status
	terminated_successfully signal_terminated term_signal
	stopped stop_signal child_wait_close_term

(Need to say more here...)

THIS MODULE IS NOT KNOWN TO BE THREAD-SAFE, SO IT PROBABLY ISN'T.
Don't run with multiple Perl threads if you use this module.

=head2 EXPORT

None by default.

=head1 SEE ALSO

OW_Privman.hpp and OW_Process.hpp.

=head1 AUTHOR

Kevin S. Van Horn, E<lt>kevin.vanhorn@quest.com<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2005, Quest Software, Inc. All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 
    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Quest Software, Inc., nor the
      names of its contributors or employees may be used to endorse or promote
      products derived from this software without specific prior written
      permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

=cut

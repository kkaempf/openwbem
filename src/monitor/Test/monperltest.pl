use Privman;
use English;

sub report_process($) {
	my ($pid) = @_;
	local *IN;
	local *OUT;
	local *ERR;

	print "report_process...\n";
	print "running: ", Privman::running(Privman::child_status($pid)), "\n";

	open(IN, ">&=", Privman::child_stdin($pid));
	print IN "This is the input sent to mpt_user";
	close(IN);

	print "--- output stream ---\n";
	open(OUT, "<&=", Privman::child_stdout($pid));
	while (<OUT>) {
		print $_;
	}
	close(OUT);

	print "--- error stream ---\n";
	open(ERR, "<&=", Privman::child_stderr($pid));
	while (<ERR>) {
		print $_;
	}
	close(ERR);

	print "--- termination status ---\n";
	my $status = Privman::child_wait_close_term($pid, 3.0, 0.0, 3.0);
	my $running = Privman::running($status);
	print "running: $running\n";
	if (!$running) {
		my $terminated = Privman::terminated($status);
		print "terminated: $terminated\n";

		my $terminated_successfully = Privman::terminated_successfully($status);
		print "terminated_successfully: $terminated_successfully\n";

		my $exit_terminated = Privman::exit_terminated($status);
		my $exit_status = Privman::exit_status($status);
		print "exit_terminated: $exit_terminated\n";
		print "exit_status: $exit_status\n" if $exit_terminated;

		my $signal_terminated = Privman::signal_terminated($status);
		my $term_signal = Privman::term_signal($status);
		print "signal_terminated: $signal_terminated\n";
		print "term_signal: $term_signal\n" if $signal_terminated;

		my $stopped = Privman::stopped($status);
		my $stop_signal = Privman::stop_signal($status);
		print "stopped: $stopped\n";
		print "stop_signal: $stop_signal\n" if $stopped;
	}
}

sub output_username()
{
	if ($REAL_USER_ID != $EFFECTIVE_USER_ID) {
		print "ERROR: uid and euid differ";
	}
	print "User: ", (scalar getpwuid($REAL_USER_ID)), "\n";
}

$| = 1; # auto-flush STDOUT; debug?

output_username();

local *CMD;
open(CMD, "<", $ARGV[0]) or die "Can't open file $ARGV[0]: $!";

while (<CMD>) {
	my $s = $_;
	eval {
		next if $s =~ /^#/;
		if ($s =~ /^(\S+)/) {
			print "\n$1\n";
		}
		else {
			next;
		}
		if ($s =~ /^open\s+(\S+)\s+(\S+)\s+(\S+)\s*$/) {
			my $mode = $1;
			my $perm = oct($2);
			my $path = $3;
			my $writing = ($mode =~ /^(>|>>|\+<|\+>|\+>>)$/);
			my $contents;
			if ($writing) {
				$contents = <CMD>;
				$contents =~ s/^\s*(\S*)\s*$/\1/;
			}
			local *FH;
			if (!Privman::open_handle(FH, $path, $mode, $perm)) {
				print "Privman::open_handle failed\n";
			}
			if ($writing) {
				print FH $contents;
			}
			if ($mode =~ /^(<|\+<|\+>|\+>>)$/) { # reading
				if ($writing) {
					seek(FH, 0, 0); # beginning of file
				}
				print '  ';
				while (<FH>) {
					print $_;
				}
				print "\n"; 
				close(FH);
			}
		}
		elsif ($s =~ /^readDirectory\s+(\S+)\s+(keep_special|omit_special)\s+$/) {
			my $path = $1;
			my $keep_special = ($2 eq 'keep_special');
			my $entries = Privman::read_directory($path, $keep_special);
			foreach $x (sort(@$entries)) {
				print "  $x\n";
			}
		}
		elsif ($s =~ /^rename\s+(\S+)\s+(\S+)\s*$/) {
			Privman::rename($1, $2);
		}
		elsif ($s =~/^unlink\s+(\S+)\s*$/) {
			my $retval = Privman::unlink($1);
			print "$retval\n";
		}
		elsif ($s =~ /^monitoredSpawn\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s*$/) {
			my $exec_path = $1;
			my $app_name = $2;
			my $argv_str = $3;
			my $envp_str = $4;
			my @argv = split(/\+/, $argv_str);
			my @envp = split(/\+/, $envp_str);
			push @envp, "LD_LIBRARY_PATH=$ENV{'LD_LIBRARY_PATH'}";
			push @envp, "LIBPATH=$ENV{'LIBPATH'}";
			push @envp, "SHLIB_PATH=$ENV{'SHLIB_PATH'}";
			push @envp, "DYLD_LIBRARY_PATH=$ENV{'DYLD_LIBRARY_PATH'}";
			my $pid = Privman::monitored_spawn($exec_path, $app_name, \@argv, \@envp);
			report_process($pid);
		}
		elsif ($s =~ /^userSpawn\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s*$/) {
			my $exec_path = $1;
			my $argv_str = $2;
			my $envp_str = $3;
			my $user = $4;
			my @argv = split(/\+/, $argv_str);
			my @envp = split(/\+/, $envp_str);
			my $pid = Privman::user_spawn($exec_path, \@argv, \@envp, $user);
			report_process($pid);
		}
		elsif ($s =~ /\S/) {
			print "BAD INPUT LINE: $s";
		}
	};
	if ($@) {
		if ($@ =~ /IPCIOException/) {
			print "Exception:\n  type: IPCIOException\n";
		}
		else {
			my $s = $@;
			$s =~ s|^\S+: \d+ ||;
			$s =~ s| at \S+ line \d+, \S+ line \d+\.$||;
			if ($s =~ /^(\S+): (.*)$/) {
				my $type = $1;
				my $msg = $2;
				print "Exception:\n  type: $type\n  msg:  $msg\n";
			}
			else {
				print "ERROR: $s\n";
			}
		}
	}
	# print "\n";
}

use perlNPI;
use User::pwent;

print "I shan't be printed.";

#
# This provider implements CIM_Person (User27_Org.mof)
#

 
sub initialize {
    print time;
}

sub enumInstanceNames {
    my ($npiHandle, $vector, $cop, $tree, $class) = @_;
    print "perl: enumInstanceNames: CIM_Person ", "\n";
    print $npiHandle,' ',$vector,' ',$cop,' ',$tree,' ',$class, "\n";
    $_[1] = perlNPI::VectorNew($npiHandle);

    my $class = perlNPI::CIMOMGetClass($npiHandle, $cop, 0);
    $ci = perlNPI::CIMClassNewInstance($npiHandle, $class);
    my $mcop;

    while (true) {
       if (!($pw = getpwent())) {
          endpwent();
          print "Closing\n";
       }
       
       print "Got entry ", $pw->name, "\n";

       $mcop = perlNPI::CIMObjectPathFromCIMInstance($npiHandle, $ci);
       perlNPI::CIMObjectPathAddStringKeyValue($npiHandle, $mcop, "CreationClassName", "CIM_Person");
       perlNPI::CIMObjectPathAddStringKeyValue($npiHandle, $mcop, "Name", $pw->name);
       perlNPI::CIMObjectPathAddStringKeyValue($npiHandle, $mcop, "CommonName", $pw->gecos);
       perlNPI::CIMObjectPathAddIntegerKeyValue($npiHandle, $mcop, "UserId", $pw->uid );
       perlNPI::_VectorAddTo($npiHandle, $_[1], $mcop);
    }
    print "Closing ...\n";
    endpwent();

    print "perl: enumInstanceNames ended", "\n";
}

sub enumInstances {
    my ($npiHandle, $vector, $cop, $tree, $class, $lo) = @_;
    print "perl: enumInstances", "\n";
    print $npiHandle,' ',$vector,' ',$cop,' ',$tree,' ',$class, "\n";

}

sub getInstance {
    my ($npiHandle, $ci, $cop, $class, $lo) = @_;
    print "perl: getInstance", "\n";
    print $npiHandle,' ',$ci,' ',$cop,' ',$class,' ',$lo, "\n";
}

sub deleteInstance {
    my ($npiHandle, $cop) = @_;
    print "perl: delete",' ',$npiHandle,' ',$cop,"\n";
    $_[1] = "blah";
}

sub createInstance {
    my ($npiHandle, $cop, $ci) = @_;
    print "perl: create",' ',$npiHandle,' ',$cop,' ',$ci,"\n";
}

sub modifyInstance {
    my ($npiHandle, $cop, $ci) = @_;
    print "perl: modify",' ',$npiHandle,' ',$cop,' ',$ci,"\n";
}

sub associators {
    my ($npiHandle, $v, $assoc, $path, $resultclass,
        $role, $resultrole, $iq ,$ico) = @_;
    my $clsname = perlNPI::CIMObjectPathGetClassName($npiHandle, $path);
    print "Assocatiate with class ",$clsname,"\n";
}

use perlNPI;

print "I shan't be printed.";
 
sub initialize {
    print time;
}

sub enumInstanceNames {
    my ($npiHandle, $vector, $cop, $tree, $class) = @_;
    print "perl: enumInstanceNames", "\n";
    print $npiHandle,' ',$vector,' ',$cop,' ',$tree,' ',$class, "\n";
    $_[1] = perlNPI::VectorNew($npiHandle);
    my @packages = `rpm -qa`;
#   if ($tree == 0)
#   {
#      print "Tree is ",$tree,"\n";
#      my $dummies = perlNPI::CIMOMEnumInstanceNames($npiHandle, $cop, 1);
#   }

    
    my $class = perlNPI::CIMOMGetClass($npiHandle, $cop, 0);
    $ci = perlNPI::CIMClassNewInstance($npiHandle, $class);

    my $mcop;
    foreach my $item (@packages) {
        $mcop = perlNPI::CIMObjectPathFromCIMInstance($npiHandle, $ci);
	perlNPI::CIMObjectPathAddStringKeyValue($npiHandle, $mcop, "Name", $item);
	perlNPI::CIMObjectPathAddStringKeyValue($npiHandle, $mcop, "Version", "1");
	perlNPI::CIMObjectPathAddIntegerKeyValue($npiHandle, $mcop, "SoftwareElementState", 2);
	perlNPI::CIMObjectPathAddStringKeyValue($npiHandle, $mcop, "SoftwareElementID", $item);
	perlNPI::CIMObjectPathAddIntegerKeyValue($npiHandle, $mcop, "TargetOperatingSystem", 36);
        perlNPI::_VectorAddTo($npiHandle, $_[1], $mcop);
    }
    print "perl: enumInstanceNames ended", "\n";
}

sub enumInstances {
    my ($npiHandle, $vector, $cop, $tree, $class, $lo) = @_;
    print "perl: enumInstances", "\n";
    print $npiHandle,' ',$vector,' ',$cop,' ',$tree,' ',$class, "\n";
    $_[1] = perlNPI::VectorNew($npiHandle);
    my $class = perlNPI::CIMOMGetClass($npiHandle, $cop, 0);
    my $citmp = perlNPI::CIMClassNewInstance($npiHandle, $class);
    my @packages = `rpm -qa`;
my $cnt = 1;

    my $new_cop;
    foreach my $item (@packages) {
        $new_cop = perlNPI::CIMObjectPathFromCIMInstance($npiHandle, $citmp);
	perlNPI::CIMObjectPathAddStringKeyValue($npiHandle, $new_cop, "Name", $item);
        my $name =
           perlNPI::CIMObjectPathGetStringKeyValue($npiHandle, $new_cop, "Name");
        my $namespace = 
           perlNPI::CIMObjectPathGetNameSpace($npiHandle, $new_cop);

	getInstanceFromName($npiHandle, $ci, $name, $namespace, $class, $lo);
        print "enumInstance add_to_vector ", $name, "\n";
        perlNPI::_VectorAddTo($npiHandle, $_[1], $ci);
$cnt++;
if ($cnt == 20) {return;}
    }

    print "perl: enumInstances ended", "\n";
}


sub getInstance {
    my ($npiHandle, $ci, $cop, $class, $lo) = @_;
    print "perl: getInstance", "\n";
    print $npiHandle,' ',$ci,' ',$cop,' ',$class,' ',$lo, "\n";
    my $class = perlNPI::CIMOMGetClass($npiHandle, $cop, 0);
    $ci = perlNPI::CIMClassNewInstance($npiHandle, $class);
    my $name =
        perlNPI::CIMObjectPathGetStringKeyValue($npiHandle, $cop, "Name");
    my $namespace = 
        perlNPI::CIMObjectPathGetNameSpace($npiHandle, $cop);
    getInstanceFromName($npiHandle, $ci, $name, $namespace, $class, $lo);
    $_[1] = $ci;
    print "perl: getInstance ended", "\n";
}

sub getInstanceFromName {
    my ($npiHandle, $ci, $name, $namespace, $class, $lo) = @_;
    #print "get rpm for ", $name, "\n";
    $_[1] = perlNPI::CIMClassNewInstance ($npiHandle, $class);

    my $rpm_cmd = "rpm -q --qf '%{name}\n%{name}\n%{version}\n%{packager}\n%{summary}' ".$name;
    my @keys = ("Name", "SoftwareElementID", "Version", 
                "Manufacturer", "Caption");
    #           "Description", "SoftwareElementState", "
    my @items = `$rpm_cmd`;
    my $cnt = 0;
    
    foreach my $item (@items) {
    #print "Set ", $keys[$cnt], " to ", $item;
       perlNPI::CIMInstanceSetStringProperty($npiHandle,
           $_[1], $keys[$cnt], $item);
       $cnt++;
    }
    my $rpm_cmd = "rpm -q --qf '%{description}' ".$name;
    my $desc = `$rpm_cmd`;
    perlNPI::CIMInstanceSetStringProperty($npiHandle,
           $_[1], "Description", $desc);
    perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $_[1], "SoftwareElementState", 2);
    perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $_[1], "TargetOperatingSystem", 36);
}

sub deleteInstance {
    my ($npiHandle, $cop) = @_;
    print "perl: delete",' ',$npiHandle,' ',$cop,"\n";
    print "Name was ","\n";
    my $name = perlNPI::CIMObjectPathGetStringKeyValue($npiHandle, $cop, "Name");
    print "Name was ",$name,"\n";
    my $rpm_cmd = "rpm -e ".$name;
    print "delete ",$rpm_cmd,"\n";
    $_[1] = $rpm_cmd;
}

sub createInstance {
    my ($npiHandle, $cop, $ci) = @_;
    print "perl: create",' ',$npiHandle,' ',$cop,' ',$ci,"\n";
    perlNPI::raiseError($npiHandle, "Create Instance Not Supported");
}

sub modifyInstance {
    my ($npiHandle, $cop, $ci) = @_;
    print "perl: modify",' ',$npiHandle,' ',$cop,' ',$ci,"\n";
    perlNPI::raiseError($npiHandle, "Modify Instance Not Supported");
}

sub associators {
    my ($npiHandle, $v, $assoc, $path, $resultclass,
        $role, $resultrole, $iq ,$ico) = @_;
    my $clsname = perlNPI::CIMObjectPathGetClassName($npiHandle, $path);
    print "Associate with class ",$clsname,"\n";
    my $name =
        perlNPI::CIMObjectPathGetStringKeyValue($npiHandle, $path, "name");

    $_[1] = perlNPI::VectorNew($npiHandle);

    # used to build instances from
    my $cop = perlNPI::CIMObjectPathNew($npiHandle, "rpm_filecheck"); 
    perlNPI::CIMObjectPathSetNameSpace($npiHandle, $cop, "root/cimv2");
    my $cc = perlNPI::CIMOMGetClass($npiHandle, $cop, 0);

    # list all files from package $name
    my $rpm_cmd = "rpm -q --qf '%{name}' ".$name;
    my $pack_name = `$rpm_cmd`;
    $rpm_cmd = "rpm -q --qf '%{version}' ".$name;
    my $version = `$rpm_cmd`;
    
    my @filelist = `rpm -ql $name`;
    foreach my $item (@filelist) {
        $ci = perlNPI::CIMClassNewInstance($npiHandle,$cc);
        $rpm_cmd = "rpm -q --qf '%{name}' ".$name;
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "SoftwareElementID", $pack_name);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "Name", $pack_name);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "Version", $version);
        perlNPI::CIMInstanceSetIntegerProperty($npiHandle,
            $ci, "TargetOperatingSystem", 36);
        perlNPI::CIMInstanceSetIntegerProperty($npiHandle,
            $ci, "SoftwareElementState", 2);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "checkid", $item);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "filename", $item);
        perlNPI::_VectorAddTo($npiHandle, $_[1], $ci);
    }
}

sub associatorNames {
    my ($npiHandle, $v, $assoc, $path, $resultclass, $role, $resultrole) = @_;
    my $clsname = perlNPI::CIMObjectPathGetClassName($npiHandle, $path);
    print "Associate (names) with class ",$clsname,"\n";
    my $name =
        perlNPI::CIMObjectPathGetStringKeyValue($npiHandle, $path, "name");

    $_[1] = perlNPI::VectorNew($npiHandle);

    # used to build instances from
    my $cop = perlNPI::CIMObjectPathNew($npiHandle, "CIM_FileSpecification"); 
    perlNPI::CIMObjectPathSetNameSpace($npiHandle, $cop, "root/cimv2");
    my $cc = perlNPI::CIMOMGetClass($npiHandle, $cop, 0);

    # list all files from package $name
    my $rpm_cmd = "rpm -q --qf '%{name}' ".$name;
    my $pack_name = `$rpm_cmd`;
    $rpm_cmd = "rpm -q --qf '%{version}' ".$name;
    my $version = `$rpm_cmd`;
    my $ccop;
    
    my @filelist = `rpm -ql $name`;
    foreach my $item (@filelist) {
        $ci = perlNPI::CIMClassNewInstance($npiHandle,$cc);
        $rpm_cmd = "rpm -q --qf '%{name}' ".$name;
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "SoftwareElementID", $pack_name);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "Name", $pack_name);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "Version", $version);
        perlNPI::CIMInstanceSetIntegerProperty($npiHandle,
            $ci, "TargetOperatingSystem", 36);
        perlNPI::CIMInstanceSetIntegerProperty($npiHandle,
            $ci, "SoftwareElementState", 2);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "checkid", $item);

        #perlNPI::CIMInstanceSetStringProperty($npiHandle,
        #    $ci, "filename", $item);
	$ccop =  perlNPI::CIMObjectPathFromCIMInstance($npiHandle,$ci);
        perlNPI::CIMObjectPathSetNameSpace($npiHandle, $ccop, "root/cimv2");
        perlNPI::_VectorAddTo($npiHandle, $_[1], $ccop);
    }
}


sub references {
    my ($npiHandle, $v, $assoc, $path, $role, $iq ,$ico) = @_;
    my $clsname = perlNPI::CIMObjectPathGetClassName($npiHandle, $path);
    print "References of class ",$clsname,"\n";
    my $name =
        perlNPI::CIMObjectPathGetStringKeyValue($npiHandle, $path, "name");

    $_[1] = perlNPI::VectorNew($npiHandle);

    # used to build instances from
    my $cop = perlNPI::CIMObjectPathNew($npiHandle, "rpm_filecheck"); 
    perlNPI::CIMObjectPathSetNameSpace($npiHandle, $cop, "root/cimv2");
    my $cc = perlNPI::CIMOMGetClass($npiHandle, $cop, 0);

    # list all files from package $name
    my $rpm_cmd = "rpm -q --qf '%{name}' ".$name;
    my $pack_name = `$rpm_cmd`;
    $rpm_cmd = "rpm -q --qf '%{version}' ".$name;
    my $version = `$rpm_cmd`;
    
    my @filelist = `rpm -ql $name`;
    foreach my $item (@filelist) {
        $ci = perlNPI::CIMClassNewInstance($npiHandle,$cc);
        $rpm_cmd = "rpm -q --qf '%{name}' ".$name;
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "SoftwareElementID", $pack_name);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "Name", $pack_name);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "Version", $version);
        perlNPI::CIMInstanceSetIntegerProperty($npiHandle,
            $ci, "TargetOperatingSystem", 36);
        perlNPI::CIMInstanceSetIntegerProperty($npiHandle,
            $ci, "SoftwareElementState", 2);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "checkid", $item);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "filename", $item);
        #perlNPI::_VectorAddTo($npiHandle, $_[1], $ci);
    }
}

sub referenceNames {
    my ($npiHandle, $v, $assoc, $path, $role) = @_;
    my $clsname = perlNPI::CIMObjectPathGetClassName($npiHandle, $path);
    print "ReferenceNames of class ",$clsname,"\n";
    my $name =
        perlNPI::CIMObjectPathGetStringKeyValue($npiHandle, $path, "name");

    $_[1] = perlNPI::VectorNew($npiHandle);

    # used to build instances from
    my $cop = perlNPI::CIMObjectPathNew($npiHandle, "rpm_filecheck"); 
    perlNPI::CIMObjectPathSetNameSpace($npiHandle, $cop, "root/cimv2");
    my $cc = perlNPI::CIMOMGetClass($npiHandle, $cop, 0);

    # list all files from package $name
    my $rpm_cmd = "rpm -q --qf '%{name}' ".$name;
    my $pack_name = `$rpm_cmd`;
    $rpm_cmd = "rpm -q --qf '%{version}' ".$name;
    my $version = `$rpm_cmd`;
    
    my @filelist = `rpm -ql $name`;
    foreach my $item (@filelist) {
        $ci = perlNPI::CIMClassNewInstance($npiHandle,$cc);
        $rpm_cmd = "rpm -q --qf '%{name}' ".$name;
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "SoftwareElementID", $pack_name);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "Name", $pack_name);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "Version", $version);
        perlNPI::CIMInstanceSetIntegerProperty($npiHandle,
            $ci, "TargetOperatingSystem", 36);
        perlNPI::CIMInstanceSetIntegerProperty($npiHandle,
            $ci, "SoftwareElementState", 2);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "checkid", $item);
        perlNPI::CIMInstanceSetStringProperty($npiHandle,
            $ci, "filename", $item);
        #perlNPI::_VectorAddTo($npiHandle, $_[1], $ci);
    }
}

use threads;
use perlNPI;

print "I shan't be printed.";

#$thr;
 
sub initialize {
    print time;
}

sub enumInstanceNames {
    my ($npiHandle, $vector, $cop, $tree, $class) = @_;
    print "perl: enumInstanceNames", "\n";
    print $npiHandle,' ',$vector,' ',$cop,' ',$tree,' ',$class, "\n";
    $_[1] = perlNPI::VectorNew($npiHandle);
    my @filesystems = `df -T | grep '/'|tr -s ' ' `;
    
    my $class = perlNPI::CIMOMGetClass($npiHandle, $cop, 0);
    $ci = perlNPI::CIMClassNewInstance($npiHandle, $class);

    # Get FS type
    my $classname = perlNPI::CIMObjectPathGetClassName($npiHandle, $cop);
    ($ln,$fs) = split(/_/, $classname); ($fst, $ln) = split(/File/, $fs);
    $fst=~ tr/A-Z/a-z/;
    
    print 'enumInstanceNames for ', $classname, " type ", $fst,"\n";

    my $mcop;
    foreach my $item (@filesystems) {
        @filesys = split(/ /, $item);
        if (index($filesys[1], $fst) >= 0) {
print "Filesystem :", $filesys[0]," ",$filesys[1]," ", $fst, " Index:", index($filesys[1], $fst), "\n";

        $mcop = perlNPI::CIMObjectPathFromCIMInstance($npiHandle, $ci);
	perlNPI::CIMObjectPathAddStringKeyValue($npiHandle, $mcop, "Name", $filesys[0]);
	perlNPI::CIMObjectPathAddStringKeyValue($npiHandle, $mcop, "CreationClassName", "Linux_LocalFileSystem");
	perlNPI::CIMObjectPathAddStringKeyValue($npiHandle, $mcop, "CSCreationClassName", "CIM_System");
	perlNPI::CIMObjectPathAddStringKeyValue($npiHandle, $mcop, "CSName", "my linux");
print "Added ", $filesys[0],"\n";
        perlNPI::_VectorAddTo($npiHandle, $_[1], $mcop);
       }
    }
    print "perl: enumInstanceNames ended", "\n";
}

sub enumInstances {
    my ($npiHandle, $vector, $cop, $tree, $class, $lo) = @_;
    print "perl: enumInstances", "\n";
    print $npiHandle,' ',$vector,' ',$cop,' ',$tree,' ',$class, "\n";
    $_[1] = perlNPI::VectorNew($npiHandle);
    my $class = perlNPI::CIMOMGetClass($npiHandle, $cop, 0);

    my @filesystems = `df -T | grep '/'|tr -s ' ' `;

    # Get FS type
    my $classname = perlNPI::CIMObjectPathGetClassName($npiHandle, $cop);
    ($ln,$fs) = split(/_/, $classname); ($fst, $ln) = split(/File/, $fs);
    $fst=~ tr/A-Z/a-z/;

    my $ci;
    foreach my $item (@filesystems) {
        @filesys = split(/ /, $item);
        if (index($filesys[1], $fst) >= 0) {

        $ci = perlNPI::CIMClassNewInstance($npiHandle, $class);
	perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci, "Name", $filesys[0]);
	perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci, "CreationClassName", "Linux_LocalFileSystem");
	perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci, "CSCreationClassName", "CIM_System");
	perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci, "CSName", "my linux");
	perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci, "Root", $filesys[6]);
	perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $ci, "BlockSize", 1024);
	perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $ci, "FileSystemSize", $filesys[2]);
	perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $ci, "AvailableSpace", $filesys[4]);
	#perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $ci, "ReadOnly", 0);
	#perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $ci, "CaseSensitive", 1);
	perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci, "FileSystemType", $filesys[1]);
        if ($filesys[6] != '') {
           fillInodeInfo ($npiHandle, $ci, $filesys[6]);
        }
        perlNPI::_VectorAddTo($npiHandle, $_[1], $ci);
    }
}


sub getInstance {
    my ($npiHandle, $ci, $cop, $class, $lo) = @_;
    print "perl: getInstance", "\n";
    print $npiHandle,' ',$ci,' ',$cop,' ',$class,' ',$lo, "\n";
    my $class = perlNPI::CIMOMGetClass($npiHandle, $cop, 0);

    # Get FS type
    my $classname = perlNPI::CIMObjectPathGetClassName($npiHandle, $cop);
    ($ln,$fs) = split(/_/, $classname); ($fst, $ln) = split(/File/, $fs);
    $fst=~ tr/A-Z/a-z/;


    my $name =
        perlNPI::CIMObjectPathGetStringKeyValue($npiHandle, $cop, "Name");
    my $namespace = 
        perlNPI::CIMObjectPathGetNameSpace($npiHandle, $cop);

    my $filesystem = `df -T $name | grep '/'|tr -s ' ' `;

    @filesys = split(/ /, $filesystem);
    if (index($filesys[1], $fst) >= 0) {

    $ci = perlNPI::CIMClassNewInstance($npiHandle, $class);

    perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci, "Name", $filesys[0]);
    perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci, "CreationClassName", "Linux_LocalFileSystem");
    perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci, "CSCreationClassName", "CIM_System");
    perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci, "CSName", "my linux");
    perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci, "Root", $filesys[6]);
    perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $ci, "BlockSize", 1024);
    perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $ci, "FileSystemSize", $filesys[2]);
    perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $ci, "AvailableSpace", $filesys[4]);
    #perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $ci, "ReadOnly", 0);
    #perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $ci, "CaseSensitive", 1);
    perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci, "FileSystemType", $filesys[1]);
    if ($filesys[6] != '') {
       fillInodeInfo ($npiHandle, $ci, $filesys[6]);
    }

    $_[1] = $ci;
    }
    print "perl: getInstance ended", "\n";
}

sub fillInodeInfo {
    my ($npiHandle, $ci, $name) = @_;
    #print "get rpm for ", $name, "\n";

    my @inodes = `df -i | grep '/' $name | tr -s ' '`;
    
    perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $_[1], "FreeInodes", $inodes[3]);
    perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $_[1], "TotalInodes", $inodes[1]);
    }
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

sub fsWatcher {
   my ($npiHandle, $expr) = @_;
   perlNPI::CIMOMAttachThread($npiHandle);
   
   while (true) {
      my @filesystems = `df -T | grep '/'|tr -s ' ' `;
      print "fsWatcher\n";

      foreach my $item (@filesystems) {
         @filesys = split(/ /, $item);
         print "WATCH Filesystem :", $filesys[0]," ",$filesys[1],"\n";
         if ($filesys[5] > 60) {
            print "Event for Filesystem :", $filesys[0], "\n";
            my $cop =
               perlNPI::CIMObjectPathNew($npiHandle, "CIM_InstModification");
            perlNPI::CIMObjectPathSetNameSpace($npiHandle, $cop, "root/cimv2");
            my $cls = perlNPI::CIMOMGetClass($npiHandle, $cop, 0);
            # no errorcheck
            my $acp =
               perlNPI::CIMObjectPathNew($npiHandle, "CIM_LocalFileSystem");
            perlNPI::CIMObjectPathSetNameSpace($npiHandle, $acp, "root/cimv2");
            my $acls = perlNPI::CIMOMGetClass($npiHandle, $acp, 0);
            my $ci_mod = perlNPI::CIMClassNewInstance($npiHandle, $cls);
            my $ci_act = perlNPI::CIMClassNewInstance($npiHandle, $acls);
            perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci_act, 
              "CSCreationClassName", "se habla blah");
            perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci_act, 
              "CSName", "my_sysname");
            perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci_act, 
              "CreationClassName", "CIM_LocalFileSystem");
            perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci_act, 
              "Name", $filesys[0]);
            perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $ci_act, 
              "Name", sprintf("%d", $filesys[5]));
            my $ci_prev = perlNPI::CIMClassNewInstance($npiHandle, $acls);
            perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci_prev, 
              "CSCreationClassName", "se habla blah");
            perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci_prev, 
              "CSName", "my_sysname");
            perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci_prev, 
              "CreationClassName", "CIM_LocalFileSystem");
            perlNPI::CIMInstanceSetStringProperty($npiHandle, $ci_prev, 
              "Name", $filesys[0]);
            perlNPI::CIMInstanceSetIntegerProperty($npiHandle, $ci_prev, 
              "Name", 55);
            perlNPI::CIMOMDeliverInstanceEvent($npiHandle, "root/cimv2",
                    $ci_mod, $ci_act, $ci_prev);
         }
      }
   }
}

sub activateFilter {
    my ($npiHandle, $exp, $eventType, $cop, $lastAct) = @_;
    print "activate Filter of type",$eventType,"\n";
    if ((index($eventType, "CIM_InstModification") > 0) |
        (index($eventType, "CIM_InstCreation") > 0)) {
       print "start thread\n";
       my $sxpr = PerlNPI::SelectExpGetSelectString($npiHandle, $exp);
       print "start thread for expr ", $sxpr, "\n";
       my $thr = threads->new(\&fsWatcher, $npiHandle, $sxpr);       
    }
}

sub deactivateFilter {
    my ($npiHandle, $exp, $eventType, $cop, $lastAct) = @_;
    print "deactivate Filter of type",$eventType,"\n";
}


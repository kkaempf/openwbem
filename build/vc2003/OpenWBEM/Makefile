all: releasebuild debugbuild
	-@echo All libraries and executibles are now built

releasebuild:
	-@echo Building Release Versions...
	pushd libopenwbem & nmake & popd
	pushd libowxml & nmake & popd
	pushd libowhttp & nmake & popd
	pushd libowclient & nmake & popd
	pushd libowmofc & nmake & popd
	pushd libowwqlcommon & nmake & popd
	pushd libowwql & nmake & popd
	pushd libcimxmllistener & nmake & popd
	pushd libowdb & nmake & popd
	pushd libowrepositoryhdb & nmake & popd
	pushd tools & nmake & popd

debugbuild:
	-@echo Building Debug Versions...
	pushd libopenwbem & nmake "DEBUG=1" & popd
	pushd libowxml & nmake "DEBUG=1" & popd
	pushd libowhttp & nmake "DEBUG=1" & popd
	pushd libowclient & nmake "DEBUG=1" & popd
	pushd libowmofc & nmake "DEBUG=1" & popd
	pushd libowwqlcommon & nmake "DEBUG=1" & popd
	pushd libowwql & nmake "DEBUG=1" & popd
	pushd libcimxmllistener & nmake "DEBUG=1" & popd
	pushd libowdb & nmake "DEBUG=1" & popd
	pushd libowrepositoryhdb & nmake "DEBUG=1" & popd
	pushd tools & nmake "DEBUG=1" & popd

clean:
	-@echo Cleaning up all libraries and executibles...
	pushd libopenwbem & nmake clean & popd
	pushd libopenwbem & nmake "DEBUG=1" clean & popd
	pushd libowxml & nmake clean & popd
	pushd libowxml & nmake "DEBUG=1" clean & popd
	pushd libowhttp & nmake clean & popd
	pushd libowhttp & nmake "DEBUG=1" clean & popd
	pushd libowclient & nmake clean & popd
	pushd libowclient & nmake "DEBUG=1" clean & popd
	pushd libowmofc & nmake clean & popd
	pushd libowmofc & nmake "DEBUG=1" clean & popd
	pushd libowwqlcommon & nmake clean & popd
	pushd libowwqlcommon & nmake "DEBUG=1" clean & popd
	pushd libowwql & nmake clean & popd
	pushd libowwql & nmake "DEBUG=1" clean & popd
	pushd libcimxmllistener & nmake clean & popd
	pushd libcimxmllistener & nmake "DEBUG=1" clean & popd
	pushd libowdb & nmake clean & popd
	pushd libowdb & nmake "DEBUG=1" clean & popd
	pushd libowrepositoryhdb & nmake clean & popd
	pushd libowrepositoryhdb & nmake "DEBUG=1" clean & popd
	pushd tools & nmake clean & popd
	pushd tools & nmake "DEBUG=1" clean & popd


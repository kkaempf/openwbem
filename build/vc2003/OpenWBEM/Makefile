
all:
	echo "MAKEDIR = $(MAKEDIR)"
	pushd libopenwbem & nmake & popd
	pushd libowxml & nmake & popd
	pushd libowhttp & nmake & popd
	pushd libowclient & nmake & popd
	pushd libowdb & nmake & popd
	pushd libowmofc & nmake & popd
	pushd libcimxmllistener & nmake & popd
	pushd libowrepositoryhdb & nmake & popd
	pushd tools & nmake & popd

clean:
	echo "MAKEDIR = $(MAKEDIR)"
	pushd libopenwbem & nmake clean & popd
	pushd libowxml & nmake clean & popd
	pushd libowhttp & nmake clean & popd
	pushd libowclient & nmake clean & popd
	pushd libowdb & nmake clean & popd
	pushd libowmofc & nmake clean & popd
	pushd libcimxmllistener & nmake clean & popd
	pushd libowrepositoryhdb & nmake clean & popd
	pushd tools & nmake clean & popd


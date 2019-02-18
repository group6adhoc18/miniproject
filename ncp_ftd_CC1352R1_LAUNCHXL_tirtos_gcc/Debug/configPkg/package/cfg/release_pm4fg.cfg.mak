# invoke SourceDir generated makefile for release.pm4fg
release.pm4fg: .libraries,release.pm4fg
.libraries,release.pm4fg: package/cfg/release_pm4fg.xdl
	$(MAKE) -f /home/sascha/workspace_openthread2/ncp_ftd_CC1352R1_LAUNCHXL_tirtos_gcc/src/makefile.libs

clean::
	$(MAKE) -f /home/sascha/workspace_openthread2/ncp_ftd_CC1352R1_LAUNCHXL_tirtos_gcc/src/makefile.libs clean


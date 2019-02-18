# invoke SourceDir generated makefile for release.pem4f
release.pem4f: .libraries,release.pem4f
.libraries,release.pem4f: package/cfg/release_pem4f.xdl
	$(MAKE) -f C:\Users\jery\ccsworkspace_v8\light_sensor_cfg_CC1352R1_LAUNCHXL_tirtos_ccs/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\jery\ccsworkspace_v8\light_sensor_cfg_CC1352R1_LAUNCHXL_tirtos_ccs/src/makefile.libs clean


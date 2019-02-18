## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,m4fg linker.cmd package/cfg/release_pm4fg.om4fg

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/release_pm4fg.xdl
	$(SED) 's"^\"\(package/cfg/release_pm4fgcfg.cmd\)\"$""\"/home/sascha/workspace_openthread2/ncp_ftd_CC1352R1_LAUNCHXL_tirtos_gcc/Debug/configPkg/\1\""' package/cfg/release_pm4fg.xdl > $@
	-$(SETDATE) -r:max package/cfg/release_pm4fg.h compiler.opt compiler.opt.defs

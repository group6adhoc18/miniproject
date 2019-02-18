################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
otsupport/%.obj: ../otsupport/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/devTools/ccs/ccsv8/tools/compiler/ti-cgt-arm_18.1.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O3 --opt_for_speed=5 --include_path="C:/Users/jery/ccsworkspace_v8/libopenthread_mtd_CC1352R1_LAUNCHXL_ccs/config" --include_path="C:/Users/jery/ccsworkspace_v8/libmbedcrypto_CC1352R1_LAUNCHXL_ccs/config" --include_path="C:/Users/jery/ccsworkspace_v8/light_sensor_cfg_CC1352R1_LAUNCHXL_tirtos_ccs" --include_path="C:/ti/simplelink_cc13x2_sdk_2_30_00_45/source/third_party/openthread/examples/platforms" --include_path="C:/ti/simplelink_cc13x2_sdk_2_30_00_45/source/third_party/openthread/include" --include_path="C:/ti/simplelink_cc13x2_sdk_2_30_00_45/source/third_party/openthread/src/core" --include_path="C:/ti/simplelink_cc13x2_sdk_2_30_00_45/source/third_party/openthread/third_party/mbedtls/repo/include" --include_path="C:/Users/jery/ccsworkspace_v8/libmbedcrypto_CC1352R1_LAUNCHXL_ccs" --include_path="C:/Users/jery/ccsworkspace_v8/light_sensor_cfg_CC1352R1_LAUNCHXL_tirtos_ccs/platform/crypto" --include_path="C:/ti/simplelink_cc13x2_sdk_2_30_00_45/source/ti/posix/ccs" --include_path="D:/devTools/ccs/ccsv8/tools/compiler/ti-cgt-arm_18.1.4.LTS/include" --define=OPENTHREAD_CONFIG_FILE='"openthread-config-cc1352-ccs-mtd.h"' --define=OPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-cc1352-config-ide.h"' --define=MBEDTLS_CONFIG_FILE='"mbedtls-config-cc1352-ccs.h"' --define=HAVE_CONFIG_H --define=SIMPLELINK_OPENTHREAD_SDK_BUILD=1 --define=SIMPLELINK_OPENTHREAD_CONFIG_MTD=1 --define=SIMPLELINK_OPENTHREAD_CONFIG_CC1352=1 --define=BOARD_DISPLAY_USE_LCD=1 --define=Board_EXCLUDE_NVS_EXTERNAL_FLASH --define=NDEBUG --define=DeviceFamily_CC13X2 -g --c99 --gcc --printf_support=nofloat --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --gen_data_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="otsupport/$(basename $(<F)).d_raw" --obj_directory="otsupport" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '



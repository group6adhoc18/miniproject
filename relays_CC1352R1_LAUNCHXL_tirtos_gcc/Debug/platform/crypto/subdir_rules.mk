################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
platform/crypto/%.o: ../platform/crypto/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/opt/ti/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-exceptions -DOPENTHREAD_CONFIG_FILE='"openthread-config-cc1352-gcc-mtd.h"' -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-cc1352-config-ide.h"' -DMBEDTLS_CONFIG_FILE='"mbedtls-config-cc1352-gcc.h"' -DHAVE_CONFIG_H -DSIMPLELINK_OPENTHREAD_SDK_BUILD=1 -DSIMPLELINK_OPENTHREAD_CONFIG_MTD=1 -DSIMPLELINK_OPENTHREAD_CONFIG_CC1352=1 -DBOARD_DISPLAY_USE_LCD=1 -DBoard_EXCLUDE_NVS_EXTERNAL_FLASH -DNDEBUG -DDeviceFamily_CC13X2 -I"/home/sascha/workspace_openthread2/libopenthread_mtd_CC1352R1_LAUNCHXL_gcc/config" -I"/home/sascha/workspace_openthread2/libmbedcrypto_CC1352R1_LAUNCHXL_gcc/config" -I"/home/sascha/workspace_openthread2/shade_CC1352R1_LAUNCHXL_tirtos_gcc" -I"/opt/ti/simplelink_cc13x2_sdk_2_30_00_45/source/third_party/openthread/examples/platforms" -I"/opt/ti/simplelink_cc13x2_sdk_2_30_00_45/source/third_party/openthread/include" -I"/opt/ti/simplelink_cc13x2_sdk_2_30_00_45/source/third_party/openthread/src/core" -I"/opt/ti/simplelink_cc13x2_sdk_2_30_00_45/source/third_party/openthread/third_party/mbedtls/repo/include" -I"/home/sascha/workspace_openthread2/libmbedcrypto_CC1352R1_LAUNCHXL_gcc" -I"/home/sascha/workspace_openthread2/shade_CC1352R1_LAUNCHXL_tirtos_gcc/platform/crypto" -I"/opt/ti/simplelink_cc13x2_sdk_2_30_00_45/source/ti/posix/gcc" -I"/opt/ti/simplelink_cc13x2_sdk_2_30_00_45/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include/newlib-nano" -I"/opt/ti/simplelink_cc13x2_sdk_2_30_00_45/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include" -I"/opt/ti/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -fno-common -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '



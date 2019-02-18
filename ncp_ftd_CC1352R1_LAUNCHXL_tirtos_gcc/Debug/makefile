################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /opt/ti/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major

GEN_OPTS__FLAG := @"configPkg/compiler.opt"
GEN_CMDS__FLAG := -Wl,-T"configPkg/linker.cmd"

ORDERED_OBJS += \
"./CC1352R1_LAUNCHXL.o" \
"./CC1352R1_LAUNCHXL_fxns.o" \
"./ccfg.o" \
"./main.o" \
"./ncp.o" \
"./otstack.o" \
"./missing/cxxhelpers.o" \
"./otsupport/otrtosapi.o" \
"./platform/alarm.o" \
"./platform/alarm_micro.o" \
"./platform/debug_uart.o" \
"./platform/diag.o" \
"./platform/misc.o" \
"./platform/platform.o" \
"./platform/radio.o" \
"./platform/random.o" \
"./platform/spi_slave.o" \
"./platform/uart.o" \
"./platform/crypto/aes_alt.o" \
"./platform/crypto/crypto_misc.o" \
"./platform/crypto/ecjpake_alt.o" \
"./platform/crypto/sha256_alt.o" \
"./platform/nv/crc.o" \
"./platform/nv/nvoctp.o" \
"./platform/nv/settings.o" \
-Wl,-T"../CC1352R1_LAUNCHXL_TIRTOS.lds" \
$(GEN_CMDS__FLAG) \
-lopenthread_diag_ncp \
-lopenthread_ncp_ncp \
-lopenthread_ncp \
-lopenthread_platform_utils_ncp \
-lmbedcrypto \
-l:"ti/display/lib/display.am4fg" \
-l:"ti/grlib/lib/gcc/m4f/grlib.a" \
-l:"third_party/spiffs/lib/gcc/m4f/spiffs_cc26xx.a" \
-l:"ti/drivers/rf/lib/rf_multiMode_cc13x2_v1.am4fg" \
-l:"ti/drivers/lib/drivers_cc13x2_v1.am4fg" \
-l:"ti/dpl/lib/dpl_cc13x2_v1.am4fg" \
-l:"ti/devices/cc13x2_cc26x2_v1/driverlib/bin/gcc/driverlib.lib" \
-lgcc \
-lc \
-lm \
-lnosys \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include missing/subdir_vars.mk
-include otsupport/subdir_vars.mk
-include platform/subdir_vars.mk
-include platform/crypto/subdir_vars.mk
-include platform/nv/subdir_vars.mk
-include subdir_rules.mk
-include missing/subdir_rules.mk
-include otsupport/subdir_rules.mk
-include platform/subdir_rules.mk
-include platform/crypto/subdir_rules.mk
-include platform/nv/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
ncp_ftd_CC1352R1_LAUNCHXL_tirtos_gcc.out \

EXE_OUTPUTS__QUOTED += \
"ncp_ftd_CC1352R1_LAUNCHXL_tirtos_gcc.out" \

BIN_OUTPUTS += \
ncp_ftd_CC1352R1_LAUNCHXL_tirtos_gcc.hex \

BIN_OUTPUTS__QUOTED += \
"ncp_ftd_CC1352R1_LAUNCHXL_tirtos_gcc.hex" \


# All Target
all: $(OBJS) $(LDS_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "ncp_ftd_CC1352R1_LAUNCHXL_tirtos_gcc.out"

dependents:
	-cd /home/sascha/workspace_openthread2/libopenthread_diag_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize && $(MAKE) all
	-cd /home/sascha/workspace_openthread2/libopenthread_ncp_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize && $(MAKE) all
	-cd /home/sascha/workspace_openthread2/libopenthread_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize && $(MAKE) all
	-cd /home/sascha/workspace_openthread2/libopenthread_platform_utils_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize && $(MAKE) all
	-cd /home/sascha/workspace_openthread2/libmbedcrypto_CC1352R1_LAUNCHXL_gcc/OptimizeSize && $(MAKE) all

# Tool invocations
ncp_ftd_CC1352R1_LAUNCHXL_tirtos_gcc.out: $(OBJS) $(LDS_SRCS) $(GEN_CMDS) /home/sascha/workspace_openthread2/libopenthread_diag_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize/libopenthread_diag_ncp.a /home/sascha/workspace_openthread2/libopenthread_ncp_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize/libopenthread_ncp_ncp.a /home/sascha/workspace_openthread2/libopenthread_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize/libopenthread_ncp.a /home/sascha/workspace_openthread2/libopenthread_platform_utils_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize/libopenthread_platform_utils_ncp.a /home/sascha/workspace_openthread2/libmbedcrypto_CC1352R1_LAUNCHXL_gcc/OptimizeSize/libmbedcrypto.a
	@echo 'Building target: "$@"'
	@echo 'Invoking: GNU Linker'
	"/opt/ti/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc" -fno-exceptions -DOPENTHREAD_CONFIG_MAX_JOINER_ENTRIES=4 -DOPENTHREAD_CONFIG_MAX_JOINER_ROUTER_ENTRIES=4 -DOPENTHREAD_CONFIG_FILE='"openthread-config-cc1352-gcc-ncp.h"' -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-cc1352-config-ide.h"' -DMBEDTLS_CONFIG_FILE='"mbedtls-config-cc1352-gcc.h"' -DHAVE_CONFIG_H -DPLATFORM_SPI_CRC_SUPPORT -DSIMPLELINK_OPENTHREAD_SDK_BUILD=1 -DSIMPLELINK_OPENTHREAD_CONFIG_NCP=1 -DSIMPLELINK_OPENTHREAD_CONFIG_CC1352=1 -DBoard_EXCLUDE_NVS_EXTERNAL_FLASH -DNDEBUG -DDeviceFamily_CC13X2 -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -fno-common -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Wl,-Map,"ncp_ftd_CC1352R1_LAUNCHXL_tirtos_gcc.map" -nostartfiles -static -Wl,--gc-sections -L"/home/sascha/workspace_openthread2/libopenthread_diag_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize" -L"/home/sascha/workspace_openthread2/libopenthread_ncp_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize" -L"/home/sascha/workspace_openthread2/libopenthread_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize" -L"/home/sascha/workspace_openthread2/libopenthread_platform_utils_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize" -L"/home/sascha/workspace_openthread2/libmbedcrypto_CC1352R1_LAUNCHXL_gcc/OptimizeSize" -L"/opt/ti/simplelink_cc13x2_sdk_2_30_00_45/source" -L"/opt/ti/simplelink_cc13x2_sdk_2_30_00_45/kernel/tirtos/packages" -L"/opt/ti/simplelink_cc13x2_sdk_2_30_00_45/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/fpv4-sp/hard" -L"/opt/ti/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/arm-none-eabi/lib" -specs=nosys.specs -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 --specs=nano.specs -o"ncp_ftd_CC1352R1_LAUNCHXL_tirtos_gcc.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

ncp_ftd_CC1352R1_LAUNCHXL_tirtos_gcc.hex: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: GNU Objcopy Utility'
	"/opt/ti/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-objcopy"  $(EXE_OUTPUTS__QUOTED) "ncp_ftd_CC1352R1_LAUNCHXL_tirtos_gcc.hex"
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "CC1352R1_LAUNCHXL.o" "CC1352R1_LAUNCHXL_fxns.o" "ccfg.o" "main.o" "ncp.o" "otstack.o" "missing/cxxhelpers.o" "otsupport/otrtosapi.o" "platform/alarm.o" "platform/alarm_micro.o" "platform/debug_uart.o" "platform/diag.o" "platform/misc.o" "platform/platform.o" "platform/radio.o" "platform/random.o" "platform/spi_slave.o" "platform/uart.o" "platform/crypto/aes_alt.o" "platform/crypto/crypto_misc.o" "platform/crypto/ecjpake_alt.o" "platform/crypto/sha256_alt.o" "platform/nv/crc.o" "platform/nv/nvoctp.o" "platform/nv/settings.o" 
	-$(RM) "CC1352R1_LAUNCHXL.d" "CC1352R1_LAUNCHXL_fxns.d" "ccfg.d" "main.d" "ncp.d" "otstack.d" "missing/cxxhelpers.d" "otsupport/otrtosapi.d" "platform/alarm.d" "platform/alarm_micro.d" "platform/debug_uart.d" "platform/diag.d" "platform/misc.d" "platform/platform.d" "platform/radio.d" "platform/random.d" "platform/spi_slave.d" "platform/uart.d" "platform/crypto/aes_alt.d" "platform/crypto/crypto_misc.d" "platform/crypto/ecjpake_alt.d" "platform/crypto/sha256_alt.d" "platform/nv/crc.d" "platform/nv/nvoctp.d" "platform/nv/settings.d" 
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/sascha/workspace_openthread2/libopenthread_diag_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize/libopenthread_diag_ncp.a:
/home/sascha/workspace_openthread2/libopenthread_ncp_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize/libopenthread_ncp_ncp.a:
/home/sascha/workspace_openthread2/libopenthread_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize/libopenthread_ncp.a:
/home/sascha/workspace_openthread2/libopenthread_platform_utils_ncp_CC1352R1_LAUNCHXL_gcc/OptimizeSize/libopenthread_platform_utils_ncp.a:
/home/sascha/workspace_openthread2/libmbedcrypto_CC1352R1_LAUNCHXL_gcc/OptimizeSize/libmbedcrypto.a:

-include ../makefile.targets


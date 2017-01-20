################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := c:/ti/ccsv6/tools/compiler/msp430_4.3.4

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./src/main_for_demo_2.7inch_TI_LaunchPad.obj" \
"./src/Pervasive_Displays_small_EPD/EPD_hardware_driver.obj" \
"./src/Pervasive_Displays_small_EPD/EPD_interface.obj" \
"./src/Pervasive_Displays_small_EPD/EPD_partial_interface.obj" \
"./src/Pervasive_Displays_small_EPD/COG_FPL/G2_Aurora_Mb/EPD_G2_Aurora_Mb.obj" \
"./src/Pervasive_Displays_small_EPD/COG_FPL/G1_Aurora_Mb/EPD_G1_Aurora_Mb.obj" \
"./src/Images/demo_2.7inch_TI_LaunchPad/Black.obj" \
"./src/Images/demo_2.7inch_TI_LaunchPad/LP01.obj" \
"./src/Images/demo_2.7inch_TI_LaunchPad/LP02.obj" \
"./src/Images/demo_2.7inch_TI_LaunchPad/LP03.obj" \
"./src/Images/demo_2.7inch_TI_LaunchPad/LP04.obj" \
"./src/Images/demo_2.7inch_TI_LaunchPad/LP05.obj" \
"./src/Images/demo_2.7inch_TI_LaunchPad/LP06.obj" \
"./src/Images/demo_2.7inch_TI_LaunchPad/LP07.obj" \
"./src/Images/demo_2.7inch_TI_LaunchPad/LP08.obj" \
"./src/Images/demo_2.7inch_TI_LaunchPad/LP09.obj" \
"./src/Images/demo_2.7inch_TI_LaunchPad/White.obj" \
"../lnk_msp430f5529.cmd" \
-l"libc.a" \
-l"libmath.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include src/subdir_vars.mk
-include src/Pervasive_Displays_small_EPD/subdir_vars.mk
-include src/Pervasive_Displays_small_EPD/COG_FPL/G2_Aurora_Mb/subdir_vars.mk
-include src/Pervasive_Displays_small_EPD/COG_FPL/G1_Aurora_Mb/subdir_vars.mk
-include src/Images/demo_2.7inch_TI_LaunchPad/subdir_vars.mk
-include subdir_rules.mk
-include src/subdir_rules.mk
-include src/Pervasive_Displays_small_EPD/subdir_rules.mk
-include src/Pervasive_Displays_small_EPD/COG_FPL/G2_Aurora_Mb/subdir_rules.mk
-include src/Pervasive_Displays_small_EPD/COG_FPL/G1_Aurora_Mb/subdir_rules.mk
-include src/Images/demo_2.7inch_TI_LaunchPad/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
EPD_Extension_Board.out \

EXE_OUTPUTS__QUOTED += \
"EPD_Extension_Board.out" \

BIN_OUTPUTS += \
EPD_Extension_Board.hex \

BIN_OUTPUTS__QUOTED += \
"EPD_Extension_Board.hex" \


# All Target
all: EPD_Extension_Board.out

# Tool invocations
EPD_Extension_Board.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: MSP430 Linker'
	"c:/ti/ccsv6/tools/compiler/msp430_4.3.4/bin/cl430" -vmspx --abi=eabi --data_model=restricted --opt_for_speed=1 -g --define=__MSP430F5529__ --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal -z -m"EPD_Extension_Board.map" --heap_size=160 --stack_size=160 --use_hw_mpy=F5 --cinit_hold_wdt=on -i"c:/ti/ccsv6/ccs_base/msp430/include" -i"c:/ti/ccsv6/tools/compiler/msp430_4.3.4/lib" -i"c:/ti/ccsv6/tools/compiler/msp430_4.3.4/include" -i"c:/ti/ccsv6/ccs_base/msp430/lib/5xx_6xx_FRxx" --reread_libs --warn_sections --diag_suppress=10374 --xml_link_info="EPD_Extension_Board_linkInfo.xml" --rom_model -o "EPD_Extension_Board.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

EPD_Extension_Board.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: MSP430 Hex Utility'
	"c:/ti/ccsv6/tools/compiler/msp430_4.3.4/bin/hex430" --memwidth=8 --romwidth=8 -o "EPD_Extension_Board.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "src\main_for_demo_2.7inch_TI_LaunchPad.pp" "src\Pervasive_Displays_small_EPD\EPD_hardware_driver.pp" "src\Pervasive_Displays_small_EPD\EPD_interface.pp" "src\Pervasive_Displays_small_EPD\EPD_partial_interface.pp" "src\Pervasive_Displays_small_EPD\COG_FPL\G2_Aurora_Mb\EPD_G2_Aurora_Mb.pp" "src\Pervasive_Displays_small_EPD\COG_FPL\G1_Aurora_Mb\EPD_G1_Aurora_Mb.pp" "src\Images\demo_2.7inch_TI_LaunchPad\Black.pp" "src\Images\demo_2.7inch_TI_LaunchPad\LP01.pp" "src\Images\demo_2.7inch_TI_LaunchPad\LP02.pp" "src\Images\demo_2.7inch_TI_LaunchPad\LP03.pp" "src\Images\demo_2.7inch_TI_LaunchPad\LP04.pp" "src\Images\demo_2.7inch_TI_LaunchPad\LP05.pp" "src\Images\demo_2.7inch_TI_LaunchPad\LP06.pp" "src\Images\demo_2.7inch_TI_LaunchPad\LP07.pp" "src\Images\demo_2.7inch_TI_LaunchPad\LP08.pp" "src\Images\demo_2.7inch_TI_LaunchPad\LP09.pp" "src\Images\demo_2.7inch_TI_LaunchPad\White.pp" 
	-$(RM) "src\main_for_demo_2.7inch_TI_LaunchPad.obj" "src\Pervasive_Displays_small_EPD\EPD_hardware_driver.obj" "src\Pervasive_Displays_small_EPD\EPD_interface.obj" "src\Pervasive_Displays_small_EPD\EPD_partial_interface.obj" "src\Pervasive_Displays_small_EPD\COG_FPL\G2_Aurora_Mb\EPD_G2_Aurora_Mb.obj" "src\Pervasive_Displays_small_EPD\COG_FPL\G1_Aurora_Mb\EPD_G1_Aurora_Mb.obj" "src\Images\demo_2.7inch_TI_LaunchPad\Black.obj" "src\Images\demo_2.7inch_TI_LaunchPad\LP01.obj" "src\Images\demo_2.7inch_TI_LaunchPad\LP02.obj" "src\Images\demo_2.7inch_TI_LaunchPad\LP03.obj" "src\Images\demo_2.7inch_TI_LaunchPad\LP04.obj" "src\Images\demo_2.7inch_TI_LaunchPad\LP05.obj" "src\Images\demo_2.7inch_TI_LaunchPad\LP06.obj" "src\Images\demo_2.7inch_TI_LaunchPad\LP07.obj" "src\Images\demo_2.7inch_TI_LaunchPad\LP08.obj" "src\Images\demo_2.7inch_TI_LaunchPad\LP09.obj" "src\Images\demo_2.7inch_TI_LaunchPad\White.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets


################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/Pervasive_Displays_small_EPD/COG_FPL/G2_Aurora_Mb/EPD_G2_Aurora_Mb.obj: ../src/Pervasive_Displays_small_EPD/COG_FPL/G2_Aurora_Mb/EPD_G2_Aurora_Mb.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"c:/ti/ccsv6/tools/compiler/msp430_4.3.4/bin/cl430" -vmspx --abi=eabi --data_model=restricted -O0 --opt_for_speed=1 --include_path="c:/ti/ccsv6/ccs_base/msp430/include" --include_path="D:/kit project/Partial Update_for MSP430F5529 LP/EPD extension board for partial update v1.0beta/EPD_Extension_Board/src/Images/demo_2.6inch_ECG" --include_path="c:/ti/ccsv6/tools/compiler/msp430_4.3.4/include" --include_path="D:/kit project/Partial Update_for MSP430F5529 LP/EPD extension board for partial update v1.0beta/EPD_Extension_Board/src" --include_path="D:/kit project/Partial Update_for MSP430F5529 LP/EPD extension board for partial update v1.0beta/EPD_Extension_Board/src/Pervasive_Displays_small_EPD" -g --define=__MSP430F5529__ --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="src/Pervasive_Displays_small_EPD/COG_FPL/G2_Aurora_Mb/EPD_G2_Aurora_Mb.pp" --obj_directory="src/Pervasive_Displays_small_EPD/COG_FPL/G2_Aurora_Mb" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '



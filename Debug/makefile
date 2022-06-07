################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include USB_HOST/Target/subdir.mk
-include USB_HOST/App/subdir.mk
-include Middlewares/Third_Party/FatFs/src/option/subdir.mk
-include Middlewares/Third_Party/FatFs/src/subdir.mk
-include Middlewares/ST/STM32_USB_Host_Library/Core/Src/subdir.mk
-include Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/subdir.mk
-include FATFS/Target/subdir.mk
-include FATFS/App/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := PMPS_Projekat_AjlaPuskarevic_EdinMustafic
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
PMPS_Projekat_AjlaPuskarevic_EdinMustafic.elf \

MAP_FILES += \
PMPS_Projekat_AjlaPuskarevic_EdinMustafic.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
PMPS_Projekat_AjlaPuskarevic_EdinMustafic.list \


# All Target
all: main-build

# Main-build Target
main-build: PMPS_Projekat_AjlaPuskarevic_EdinMustafic.elf secondary-outputs

# Tool invocations
PMPS_Projekat_AjlaPuskarevic_EdinMustafic.elf PMPS_Projekat_AjlaPuskarevic_EdinMustafic.map: $(OBJS) $(USER_OBJS) C:\Users\Student\STM32CubeIDE\workspace_1.9.0\PMPS_Projekat_AjlaPuskarevic_EdinMustafic\STM32F407VGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "PMPS_Projekat_AjlaPuskarevic_EdinMustafic.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Student\STM32CubeIDE\workspace_1.9.0\PMPS_Projekat_AjlaPuskarevic_EdinMustafic\STM32F407VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="PMPS_Projekat_AjlaPuskarevic_EdinMustafic.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

PMPS_Projekat_AjlaPuskarevic_EdinMustafic.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "PMPS_Projekat_AjlaPuskarevic_EdinMustafic.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) PMPS_Projekat_AjlaPuskarevic_EdinMustafic.elf PMPS_Projekat_AjlaPuskarevic_EdinMustafic.list PMPS_Projekat_AjlaPuskarevic_EdinMustafic.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets

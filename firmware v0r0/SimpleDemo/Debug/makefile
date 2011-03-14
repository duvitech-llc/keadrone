################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include HAL_UART/subdir.mk
-include HAL_SPI/subdir.mk
-include HAL_Analog/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: SimpleDemo.axf

# Tool invocations
SimpleDemo.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\admin\Desktop\Keadrone\CMSISv1p30_LPC17xx\Debug" -L"C:\Users\admin\Desktop\Keadrone\FreeRTOS_Library\Debug" -Xlinker --gc-sections -Xlinker -Map=SimpleDemo.map -mcpu=cortex-m3 -mthumb -T "SimpleDemo_Debug.ld" -o"SimpleDemo.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) SimpleDemo.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size SimpleDemo.axf; # arm-none-eabi-objcopy -O ihex SimpleDemo.axf SimpleDemo.hex ;
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets

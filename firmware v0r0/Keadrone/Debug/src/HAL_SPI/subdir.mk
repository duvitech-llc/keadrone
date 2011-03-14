################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/HAL_SPI/lpc17xx_clkpwr.c \
../src/HAL_SPI/lpc17xx_libcfg_default.c \
../src/HAL_SPI/lpc17xx_ssp.c 

OBJS += \
./src/HAL_SPI/lpc17xx_clkpwr.o \
./src/HAL_SPI/lpc17xx_libcfg_default.o \
./src/HAL_SPI/lpc17xx_ssp.o 

C_DEPS += \
./src/HAL_SPI/lpc17xx_clkpwr.d \
./src/HAL_SPI/lpc17xx_libcfg_default.d \
./src/HAL_SPI/lpc17xx_ssp.d 


# Each subdirectory must supply rules for building sources it contributes
src/HAL_SPI/%.o: ../src/HAL_SPI/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__USE_CMSIS=CMSISv1p30_LPC17xx -D__CODE_RED -D__REDLIB__ -I"C:\Users\admin\Desktop\Keadrone\CMSISv1p30_LPC17xx\inc" -I"C:\Users\admin\Desktop\Keadrone\SimpleDemo" -I"C:\Users\admin\Desktop\Keadrone\SimpleDemo\src" -I"C:\Users\admin\Desktop\Keadrone\FreeRTOS_Library\include" -I"C:\Users\admin\Desktop\Keadrone\FreeRTOS_Library\portable" -O0 -g3 -fsigned-char -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



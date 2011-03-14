################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL_SPI/lpc17xx_clkpwr.c \
../HAL_SPI/lpc17xx_libcfg_default.c \
../HAL_SPI/lpc17xx_ssp.c \
../HAL_SPI/main_spi.c 

OBJS += \
./HAL_SPI/lpc17xx_clkpwr.o \
./HAL_SPI/lpc17xx_libcfg_default.o \
./HAL_SPI/lpc17xx_ssp.o \
./HAL_SPI/main_spi.o 

C_DEPS += \
./HAL_SPI/lpc17xx_clkpwr.d \
./HAL_SPI/lpc17xx_libcfg_default.d \
./HAL_SPI/lpc17xx_ssp.d \
./HAL_SPI/main_spi.d 


# Each subdirectory must supply rules for building sources it contributes
HAL_SPI/%.o: ../HAL_SPI/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__USE_CMSIS=CMSISv1p30_LPC17xx -D__CODE_RED -D__REDLIB__ -I"C:\Users\admin\Desktop\Keadrone\CMSISv1p30_LPC17xx\inc" -I"C:\Users\admin\Desktop\Keadrone\SimpleDemo" -I"C:\Users\admin\Desktop\Keadrone\SimpleDemo\src" -I"C:\Users\admin\Desktop\Keadrone\SimpleDemo\HAL_SPI" -I"C:\Users\admin\Desktop\Keadrone\SimpleDemo\HAL_UART" -I"C:\Users\admin\Desktop\Keadrone\FreeRTOS_Library\include" -I"C:\Users\admin\Desktop\Keadrone\FreeRTOS_Library\portable" -I"C:\Users\admin\Desktop\Keadrone\SimpleDemo\HAL_Analog" -O0 -g3 -fsigned-char -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL_UART/uart.c 

OBJS += \
./HAL_UART/uart.o 

C_DEPS += \
./HAL_UART/uart.d 


# Each subdirectory must supply rules for building sources it contributes
HAL_UART/%.o: ../HAL_UART/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__USE_CMSIS=CMSISv1p30_LPC17xx -D__CODE_RED -D__REDLIB__ -I"C:\Users\admin\Desktop\Keadrone\CMSISv1p30_LPC17xx\inc" -I"C:\Users\admin\Desktop\Keadrone\SimpleDemo" -I"C:\Users\admin\Desktop\Keadrone\SimpleDemo\src" -I"C:\Users\admin\Desktop\Keadrone\SimpleDemo\HAL_SPI" -I"C:\Users\admin\Desktop\Keadrone\SimpleDemo\HAL_UART" -I"C:\Users\admin\Desktop\Keadrone\FreeRTOS_Library\include" -I"C:\Users\admin\Desktop\Keadrone\FreeRTOS_Library\portable" -I"C:\Users\admin\Desktop\Keadrone\SimpleDemo\HAL_Analog" -O0 -g3 -fsigned-char -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



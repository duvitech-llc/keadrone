################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/DCM.c \
../src/DCM_Data.c \
../src/Vector.c \
../src/calculations_heading.c \
../src/dAccelerometer_BMA180.c \
../src/dAnalogLpc.c \
../src/dGyro_ITG-3200.c \
../src/dServoPwm.c \
../src/dServoTimers.c \
../src/main.c 

OBJS += \
./src/DCM.o \
./src/DCM_Data.o \
./src/Vector.o \
./src/calculations_heading.o \
./src/dAccelerometer_BMA180.o \
./src/dAnalogLpc.o \
./src/dGyro_ITG-3200.o \
./src/dServoPwm.o \
./src/dServoTimers.o \
./src/main.o 

C_DEPS += \
./src/DCM.d \
./src/DCM_Data.d \
./src/Vector.d \
./src/calculations_heading.d \
./src/dAccelerometer_BMA180.d \
./src/dAnalogLpc.d \
./src/dGyro_ITG-3200.d \
./src/dServoPwm.d \
./src/dServoTimers.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__USE_CMSIS=CMSISv1p30_LPC17xx -D__CODE_RED -D__REDLIB__ -I"C:\Users\admin\Desktop\Keadrone\CMSISv1p30_LPC17xx\inc" -I"C:\Users\admin\Desktop\Keadrone\FreeRTOS_Library\include" -I"C:\Users\admin\Desktop\Keadrone\FreeRTOS_Library\portable" -I"C:\Users\admin\Desktop\Keadrone\Keadrone" -I"C:\Users\admin\Desktop\Keadrone\Keadrone\HAL_UART" -I"C:\Users\admin\Desktop\Keadrone\Keadrone\src" -O0 -g3 -fsigned-char -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



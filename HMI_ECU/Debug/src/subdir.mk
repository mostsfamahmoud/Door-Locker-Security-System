################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/HMI_Application.c \
../src/gpio.c \
../src/keypad.c \
../src/lcd.c \
../src/timer.c \
../src/uart.c 

OBJS += \
./src/HMI_Application.o \
./src/gpio.o \
./src/keypad.o \
./src/lcd.o \
./src/timer.o \
./src/uart.o 

C_DEPS += \
./src/HMI_Application.d \
./src/gpio.d \
./src/keypad.d \
./src/lcd.d \
./src/timer.d \
./src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



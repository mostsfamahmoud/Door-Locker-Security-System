################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Control_Application.c \
../src/buzzer.c \
../src/dc_motor.c \
../src/external_eeprom.c \
../src/gpio.c \
../src/timer.c \
../src/twi.c \
../src/uart.c 

OBJS += \
./src/Control_Application.o \
./src/buzzer.o \
./src/dc_motor.o \
./src/external_eeprom.o \
./src/gpio.o \
./src/timer.o \
./src/twi.o \
./src/uart.o 

C_DEPS += \
./src/Control_Application.d \
./src/buzzer.d \
./src/dc_motor.d \
./src/external_eeprom.d \
./src/gpio.d \
./src/timer.d \
./src/twi.d \
./src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



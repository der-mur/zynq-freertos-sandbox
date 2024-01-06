################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/gpio/axi_gpio0_if.c \
../src/gpio/axi_gpio1_if.c \
../src/gpio/ps7_gpio_if.c 

OBJS += \
./src/gpio/axi_gpio0_if.o \
./src/gpio/axi_gpio1_if.o \
./src/gpio/ps7_gpio_if.o 

C_DEPS += \
./src/gpio/axi_gpio0_if.d \
./src/gpio/axi_gpio1_if.d \
./src/gpio/ps7_gpio_if.d 


# Each subdirectory must supply rules for building sources it contributes
src/gpio/%.o: ../src/gpio/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -IF:/Documents/GitHub/zynq-freertos-sandbox/2023.2/zybo-z7-20/hw-proj1/vitis_classic/freertos_single_core_pfm/export/freertos_single_core_pfm/sw/freertos_single_core_pfm/freertos10_xilinx_domain/bspinclude/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



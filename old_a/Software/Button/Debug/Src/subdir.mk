################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/adc.c \
../Src/gpio.c \
../Src/main.c \
../Src/spi.c \
../Src/stm32f0xx_hal_msp.c \
../Src/stm32f0xx_hal_timebase_TIM.c \
../Src/stm32f0xx_it.c \
../Src/system_stm32f0xx.c \
../Src/usart.c 

OBJS += \
./Src/adc.o \
./Src/gpio.o \
./Src/main.o \
./Src/spi.o \
./Src/stm32f0xx_hal_msp.o \
./Src/stm32f0xx_hal_timebase_TIM.o \
./Src/stm32f0xx_it.o \
./Src/system_stm32f0xx.o \
./Src/usart.o 

C_DEPS += \
./Src/adc.d \
./Src/gpio.d \
./Src/main.d \
./Src/spi.d \
./Src/stm32f0xx_hal_msp.d \
./Src/stm32f0xx_hal_timebase_TIM.d \
./Src/stm32f0xx_it.d \
./Src/system_stm32f0xx.d \
./Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F030x6 -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/Inc" -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/Drivers/STM32F0xx_HAL_Driver/Inc" -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy" -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/Drivers/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/Drivers/CMSIS/Include" -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/Radio/nRF905/Inc" -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/System" -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/App"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



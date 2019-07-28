################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../App/App.cpp 

OBJS += \
./App/App.o 

CPP_DEPS += \
./App/App.d 


# Each subdirectory must supply rules for building sources it contributes
App/%.o: ../App/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU G++ Compiler'
	@echo $(PWD)
	arm-none-eabi-g++ -mcpu=cortex-m0 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F030x6 -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/Inc" -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/Drivers/STM32F0xx_HAL_Driver/Inc" -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy" -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/Drivers/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/Drivers/CMSIS/Include" -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/App" -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/System" -I"C:/Users/Nils/STMWorkbench/workspace/LCD-Counter-Button/Radio/nRF905/Inc"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



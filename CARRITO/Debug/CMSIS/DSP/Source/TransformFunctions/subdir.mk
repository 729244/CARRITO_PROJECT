################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/DSP/Source/TransformFunctions/TransformFunctions.c 

C_DEPS += \
./CMSIS/DSP/Source/TransformFunctions/TransformFunctions.d 

OBJS += \
./CMSIS/DSP/Source/TransformFunctions/TransformFunctions.o 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/DSP/Source/TransformFunctions/%.o: ../CMSIS/DSP/Source/TransformFunctions/%.c CMSIS/DSP/Source/TransformFunctions/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK66FN2M0VMD18 -DCPU_MK66FN2M0VMD18_cm4 -DSDK_OS_FREE_RTOS -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\board" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\source" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\freertos\freertos_kernel\include" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\freertos\freertos_kernel\portable\GCC\ARM_CM4F" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\drivers" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\drivers\freertos" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\CMSIS\DSP\Include" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\device" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\CMSIS" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\component\uart" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\component\serial_manager" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\utilities" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\component\lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-CMSIS-2f-DSP-2f-Source-2f-TransformFunctions

clean-CMSIS-2f-DSP-2f-Source-2f-TransformFunctions:
	-$(RM) ./CMSIS/DSP/Source/TransformFunctions/TransformFunctions.d ./CMSIS/DSP/Source/TransformFunctions/TransformFunctions.o

.PHONY: clean-CMSIS-2f-DSP-2f-Source-2f-TransformFunctions


################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/DSP/Source/FilteringFunctions/FilteringFunctions.c 

C_DEPS += \
./CMSIS/DSP/Source/FilteringFunctions/FilteringFunctions.d 

OBJS += \
./CMSIS/DSP/Source/FilteringFunctions/FilteringFunctions.o 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/DSP/Source/FilteringFunctions/%.o: ../CMSIS/DSP/Source/FilteringFunctions/%.c CMSIS/DSP/Source/FilteringFunctions/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK66FN2M0VMD18 -DCPU_MK66FN2M0VMD18_cm4 -DSDK_OS_FREE_RTOS -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\board" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\source" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\freertos\freertos_kernel\include" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\freertos\freertos_kernel\portable\GCC\ARM_CM4F" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\drivers" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\drivers\freertos" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\CMSIS\DSP\Include" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\device" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\CMSIS" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\component\uart" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\component\serial_manager" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\utilities" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\component\lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-CMSIS-2f-DSP-2f-Source-2f-FilteringFunctions

clean-CMSIS-2f-DSP-2f-Source-2f-FilteringFunctions:
	-$(RM) ./CMSIS/DSP/Source/FilteringFunctions/FilteringFunctions.d ./CMSIS/DSP/Source/FilteringFunctions/FilteringFunctions.o

.PHONY: clean-CMSIS-2f-DSP-2f-Source-2f-FilteringFunctions

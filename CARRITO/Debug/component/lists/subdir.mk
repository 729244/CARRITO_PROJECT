################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/lists/fsl_component_generic_list.c 

C_DEPS += \
./component/lists/fsl_component_generic_list.d 

OBJS += \
./component/lists/fsl_component_generic_list.o 


# Each subdirectory must supply rules for building sources it contributes
component/lists/%.o: ../component/lists/%.c component/lists/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK66FN2M0VMD18 -DCPU_MK66FN2M0VMD18_cm4 -DSDK_OS_FREE_RTOS -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\board" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\source" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\freertos\freertos_kernel\include" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\freertos\freertos_kernel\portable\GCC\ARM_CM4F" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\drivers" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\drivers\freertos" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\CMSIS\DSP\Include" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\device" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\CMSIS" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\component\uart" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\component\serial_manager" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\utilities" -I"D:\Documentos\Embebidos_II\Practicas\CARRITO\component\lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-component-2f-lists

clean-component-2f-lists:
	-$(RM) ./component/lists/fsl_component_generic_list.d ./component/lists/fsl_component_generic_list.o

.PHONY: clean-component-2f-lists


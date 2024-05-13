################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/serial_manager/fsl_component_serial_manager.c \
../component/serial_manager/fsl_component_serial_port_uart.c 

C_DEPS += \
./component/serial_manager/fsl_component_serial_manager.d \
./component/serial_manager/fsl_component_serial_port_uart.d 

OBJS += \
./component/serial_manager/fsl_component_serial_manager.o \
./component/serial_manager/fsl_component_serial_port_uart.o 


# Each subdirectory must supply rules for building sources it contributes
component/serial_manager/%.o: ../component/serial_manager/%.c component/serial_manager/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK66FN2M0VMD18 -DCPU_MK66FN2M0VMD18_cm4 -DSDK_OS_FREE_RTOS -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\board" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\source" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\freertos\freertos_kernel\include" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\freertos\freertos_kernel\portable\GCC\ARM_CM4F" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\drivers" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\drivers\freertos" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\CMSIS\DSP\Include" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\device" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\CMSIS" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\component\uart" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\component\serial_manager" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\utilities" -I"Z:\EMBEBIDOS 2\Proyecto final\CARRITO_PROJECT\CARRITO_PROJECT\CARRITO\component\lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-component-2f-serial_manager

clean-component-2f-serial_manager:
	-$(RM) ./component/serial_manager/fsl_component_serial_manager.d ./component/serial_manager/fsl_component_serial_manager.o ./component/serial_manager/fsl_component_serial_port_uart.d ./component/serial_manager/fsl_component_serial_port_uart.o

.PHONY: clean-component-2f-serial_manager


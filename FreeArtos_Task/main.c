/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "fsl_dspi.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_pit.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#include <stdint.h>
#include <stdbool.h>
#include "fsl_clock.h"
#include "pwm_controller.h"
#include "fsl_gpio.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_DSPI_SLAVE_BASEADDR SPI0
#define TRANSFER_SIZE 50U /*! Transfer dataSize */
#define ECCHO 13u
#define TRIGGER 25u
#define TURN_RIGHT 1
#define TURN_LEFT -1
#define STRAIGHT 2

#define FORDWARD 0
#define BACKWARD 1

/* DSPI user callback */
void DSPI_SlaveUserCallback(SPI_Type *base, dspi_slave_handle_t *handle, status_t status, void *userData);
/* Task priorities. */
#define hello_task_PRIORITY (configMAX_PRIORITIES - 1)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void spi_init(void *pvParameters);
static void get_size_spi(void *pvParameters);
static void get_objets_spi(void *pvParameters);
static void ultrasonic_init(void *pvParameters);
static void ultrasonic(void *pvParameters);
static void motor_init(void *pvParameters);
static void turn(void *pvParameters);
static void turn15(void *pvParameters);
static void stopAndResetDirection(void *pvParameters);
/*******************************************************************************
 * Variables
 ******************************************************************************/
//SPI
uint8_t slaveRxSize[1U] = {0U};
uint8_t slaveRxData[TRANSFER_SIZE] = {0U};
dspi_slave_handle_t g_s_handle;
uint8_t flag_spi = 0;
//ULTASONIC
uint8_t edge = 0;
uint8_t flag = 1;
uint32_t usec_start = 0;
uint32_t usec_count = 0;
uint32_t usec_final = 0;
double cm = 0;
//CARRO
uint8_t g_current_direction = STRAIGHT;
uint8_t angle;
int8_t direction = 0;
uint8_t fordward_backward_selector;

TaskHandle_t spi_init_handle;
TaskHandle_t get_size_spi_handle;
TaskHandle_t ultrasonic_init_handle;
TaskHandle_t ultrasonic_handle;
TaskHandle_t get_objets_spi_handle;
TaskHandle_t motor_init_handle;
TaskHandle_t reset_direction_handle;
TaskHandle_t turn15_handle;
TaskHandle_t turn_handle;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */
void DSPI_SlaveUserCallback(SPI_Type *base, dspi_slave_handle_t *handle, status_t status, void *userData)
{
    if (flag_spi == 0)
    {
        PRINTF("This is DSPI slave transfer completed callback. \r\n");
        PRINTF("It's a successful transfer. \r\n\r\n");
        for(uint8_t i = 0; i<TRANSFER_SIZE;i++){
        	if((i & 0x0fU) == 0u){
        		PRINTF("\r\n");
        	}
        	PRINTF(" %02X",slaveRxData[i]);
        }
		PRINTF("\r\n");
	    xTaskResumeFromISR(get_size_spi_handle);
    }else{
	    xTaskResumeFromISR(get_objets_spi_handle);
    }
}
void PORTD_IRQHandler(void)
{
    GPIO_PortClearInterruptFlags(GPIOD, 1U << ECCHO);
	if (edge == 0){
			PIT_StartTimer(PIT, kPIT_Chnl_0);
			usec_start = PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_0);
			edge = 1;
		}
	else{
		usec_count = PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_0);
		PIT_StopTimer(PIT, kPIT_Chnl_0);
		usec_final = COUNT_TO_USEC((usec_start-usec_count),CLOCK_GetFreq(kCLOCK_BusClk));
		cm = usec_final/59;
		PRINTF("CM: %d\r\n", (uint32_t)cm);
		edge = 0;
		flag = 1;
		xTaskResumeFromISR(ultrasonic_handle);
	}
}
int main(void)
{
    /* Init board hardware. */
	CLOCK_EnableClock(kCLOCK_PortD);
	CLOCK_EnableClock(kCLOCK_PortE);
	CLOCK_SetSimSafeDivs();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    xTaskCreate(spi_init, "spi_init", configMINIMAL_STACK_SIZE + 100, NULL, 2U, &spi_init_handle);
    xTaskCreate(motor_init, "motor_init", configMINIMAL_STACK_SIZE + 100, NULL, 2U, &motor_init_handle);
    xTaskCreate(ultrasonic_init,"ultrasonic_init",configMINIMAL_STACK_SIZE + 100,NULL,2U,&ultrasonic_init_handle);

    xTaskCreate(get_size_spi, "get_size_spi", configMINIMAL_STACK_SIZE + 100, NULL, 1U, &get_size_spi_handle);
    xTaskCreate(get_objets_spi,"get_objets_spi",configMINIMAL_STACK_SIZE + 100,NULL,1U,&get_objets_spi_handle);

    xTaskCreate(ultrasonic,"ultrasonic",configMINIMAL_STACK_SIZE + 100,NULL,1U,&ultrasonic_handle);
    xTaskCreate(turn,"turn",configMINIMAL_STACK_SIZE + 100,NULL,1U,&turn_handle);
    xTaskCreate(turn15,"turn15",configMINIMAL_STACK_SIZE + 100,NULL,1U,&turn15_handle);
    xTaskCreate(stopAndResetDirection,"stopAndResetDirection",configMINIMAL_STACK_SIZE + 100,NULL,1U,&reset_direction_handle);
    vTaskStartScheduler();
    for (;;)
        ;
}

static void spi_init(void *pvParameters)
{
	dspi_slave_config_t slaveConfig;
	dspi_transfer_t slaveXfer;

	/* Slave config */
	slaveConfig.whichCtar                  = kDSPI_Ctar0;
	slaveConfig.ctarConfig.bitsPerFrame    = 8U;
	slaveConfig.ctarConfig.cpol            = kDSPI_ClockPolarityActiveHigh;
	slaveConfig.ctarConfig.cpha            = kDSPI_ClockPhaseFirstEdge;
	slaveConfig.enableContinuousSCK        = false;
	slaveConfig.enableRxFifoOverWrite      = false;
	slaveConfig.enableModifiedTimingFormat = false;
	slaveConfig.samplePoint                = kDSPI_SckToSin0Clock;

	DSPI_SlaveInit(EXAMPLE_DSPI_SLAVE_BASEADDR, &slaveConfig);

	DSPI_SlaveTransferCreateHandle(EXAMPLE_DSPI_SLAVE_BASEADDR, &g_s_handle, DSPI_SlaveUserCallback, NULL);
	NVIC_SetPriority(SPI0_IRQn, 3u);
	vTaskDelete(spi_init_handle);
}
static void get_size_spi(void *pvParameters)
{
	dspi_transfer_t slaveXfer;
	while(1){
		flag_spi = 1;
		slaveRxSize[1] = 0x00;
		/* Set slave transfer ready to receive data */
		slaveXfer.txData      = NULL;
		slaveXfer.rxData      = slaveRxSize;
		slaveXfer.dataSize    = 1U;
		slaveXfer.configFlags = kDSPI_SlaveCtar0;

		/* Slave start receive */
		DSPI_SlaveTransferNonBlocking(EXAMPLE_DSPI_SLAVE_BASEADDR, &g_s_handle, &slaveXfer);
		vTaskSuspend(get_size_spi_handle);
	}
}
static void get_objets_spi(void *pvParameters){
	dspi_transfer_t slaveXfer;
	while(1){
		vTaskSuspend(get_objets_spi_handle);
    	flag_spi = 0;
		for (uint8_t i = 0; i < slaveRxSize[1]*5; i++)
		{
			slaveRxData[i] = 0U;
		}

		/* Set slave transfer ready to receive data */
		slaveXfer.txData      = NULL;
		slaveXfer.rxData      = slaveRxData;
		slaveXfer.dataSize    = slaveRxSize[1]*5;
		slaveXfer.configFlags = kDSPI_SlaveCtar0;

		/* Slave start receive */
		DSPI_SlaveTransferNonBlocking(EXAMPLE_DSPI_SLAVE_BASEADDR, &g_s_handle, &slaveXfer);
	}
}
static void ultrasonic_init(void *pvParameters){
    /* Define the init structure for the input switch pin */
    gpio_pin_config_t input = {
        kGPIO_DigitalInput,
        0,
    };

    /* Define the init structure for the output LED pin */
    gpio_pin_config_t output = {
        kGPIO_DigitalOutput,
        0,
    };
    PORT_SetPinMux(PORTD, ECCHO, kPORT_MuxAsGpio);
    GPIO_PinInit(GPIOD, ECCHO, &input);
    PORT_SetPinInterruptConfig(PORTD, ECCHO, kPORT_InterruptEitherEdge);
    PORT_SetPinMux(PORTE, TRIGGER, kPORT_MuxAsGpio);
    GPIO_PinInit(GPIOE, TRIGGER, &output);
    GPIO_PortClear(GPIOE, 1U << TRIGGER);
	pit_config_t pitConfig;
	PIT_GetDefaultConfig(&pitConfig);
	PIT_Init(PIT, &pitConfig);
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, USEC_TO_COUNT(500000,  CLOCK_GetFreq(kCLOCK_BusClk)));
	PIT_StopTimer(PIT, kPIT_Chnl_0);
	NVIC_SetPriority(PORTD_IRQn, 2u);
	__set_BASEPRI(10 << (8 - __NVIC_PRIO_BITS));
	EnableIRQ(PORTD_IRQn);
    vTaskDelete(ultrasonic_init_handle);
}
static void ultrasonic(void *pvParameters){
	while(1){
		GPIO_PortToggle(GPIOE, 1U << TRIGGER);
		vTaskDelay(pdMS_TO_TICKS(10));
		GPIO_PortToggle(GPIOE, 1U << TRIGGER);
		vTaskSuspend(ultrasonic_handle);
	}
}
static void motor_init(void *pvParameters){
	init_pwm(pwm_channel_2);
	init_pwm(pwm_channel_1);
	vTaskDelay(pdMS_TO_TICKS(5000));
    vTaskDelete(motor_init_handle);
}
static void stopAndResetDirection(void *pvParameters){
	while(1){
		vTaskSuspend(reset_direction_handle);
		pwm_update(pwm_channel_2, 0);
		pwm_update(pwm_channel_1, 0);
		g_current_direction = STRAIGHT;
		vTaskDelay(pdMS_TO_TICKS(350));
	}
}

static void turn(void *pvParameters){
	while(1){
		vTaskSuspend(turn_handle);
		switch(angle){
		case 15:
			fordward_backward_selector = FORDWARD;
			vTaskResume(turn_handle);
			//turn15(direction,FORDWARD);
			vTaskResume(reset_direction_handle);
			//stopAndResetDirection();
			break;
		case 30:
			direction=direction*-1;
			fordward_backward_selector = BACKWARD;
			vTaskResume(turn15_handle);
			//turn15(direction*-1,BACKWARD);
			vTaskResume(reset_direction_handle);
			//stopAndResetDirection();
			direction=direction*-1;
			fordward_backward_selector = FORDWARD;
			vTaskResume(turn15_handle);
			//turn15(direction,FORDWARD);
			vTaskResume(reset_direction_handle);
			//stopAndResetDirection();
			break;
		case 45:
			direction=direction*-1;
			fordward_backward_selector = BACKWARD;
			vTaskResume(turn15_handle);
			//turn15(direction*-1,BACKWARD);
			vTaskResume(reset_direction_handle);
			//stopAndResetDirection();
			direction=direction*-1;
			fordward_backward_selector = FORDWARD;
			vTaskResume(turn15_handle);
			vTaskResume(turn15_handle);
			//turn15(direction,FORDWARD);
			//turn15(direction,FORDWARD);
			vTaskResume(reset_direction_handle);
			//stopAndResetDirection();
			break;
		case 60:
			direction=direction*-1;
			fordward_backward_selector = BACKWARD;
			vTaskResume(turn15_handle);
			vTaskResume(turn15_handle);
			//turn15(direction*-1,BACKWARD);
			//turn15(direction*-1,BACKWARD);
			vTaskResume(reset_direction_handle);
			//stopAndResetDirection();
			direction=direction*-1;
			fordward_backward_selector = FORDWARD;
			vTaskResume(turn15_handle);
			vTaskResume(turn15_handle);
			//turn15(direction,FORDWARD);
			//turn15(direction,FORDWARD);
			vTaskResume(reset_direction_handle);
			//stopAndResetDirection();
			break;
		case 75:
			direction=direction*-1;
			fordward_backward_selector = BACKWARD;
			vTaskResume(turn15_handle);
			vTaskResume(turn15_handle);
			//turn15(direction*-1,BACKWARD);
			//turn15(direction*-1,BACKWARD);
			vTaskResume(reset_direction_handle);
			//stopAndResetDirection();
			direction=direction*-1;
			fordward_backward_selector = FORDWARD;
			vTaskResume(turn15_handle);
			vTaskResume(turn15_handle);
			vTaskResume(turn15_handle);
			//turn15(direction,FORDWARD);
			//turn15(direction,FORDWARD);
			//turn15(direction,FORDWARD);
			vTaskResume(reset_direction_handle);
			//stopAndResetDirection();
			break;
		case 90:
			direction=direction*-1;
			fordward_backward_selector = BACKWARD;
			vTaskResume(turn15_handle);
			vTaskResume(turn15_handle);
			vTaskResume(turn15_handle);
			//turn15(direction*-1,BACKWARD);
			//turn15(direction*-1,BACKWARD);
			//turn15(direction*-1,BACKWARD);
			vTaskResume(reset_direction_handle);
			//stopAndResetDirection();
			direction=direction*-1;
			fordward_backward_selector = FORDWARD;
			vTaskResume(turn15_handle);
			vTaskResume(turn15_handle);
			vTaskResume(turn15_handle);
			//turn15(direction,FORDWARD);
			//turn15(direction,FORDWARD);
			//turn15(direction,FORDWARD);
			vTaskResume(reset_direction_handle);
			//stopAndResetDirection();
			break;
		case 180:
			//???
			//turn(90,direction);
			//turn(90,direction);
			vTaskResume(reset_direction_handle);
			//stopAndResetDirection();
			break;
		}
	}
}

static void turn15(void *pvParameters){
	while(1){
		vTaskSuspend(turn15_handle);
		//Funcion para girar 15° hacia una dirección
		if(direction != g_current_direction){
			if(TURN_RIGHT == direction){
				pwm_update(pwm_channel_2, -300);
				g_current_direction = TURN_RIGHT;
				vTaskDelay(pdMS_TO_TICKS(100));
			}
			else{
				pwm_update(pwm_channel_2, 300);
				g_current_direction = TURN_LEFT;
				vTaskDelay(pdMS_TO_TICKS(100));
			}
		}
		if(FORDWARD == fordward_backward_selector){
			pwm_update(pwm_channel_1, 50);
		}
		else{
			init_pwm(pwm_channel_1);
			pwm_update(pwm_channel_1, -75);
		}
		vTaskDelay(pdMS_TO_TICKS(660));
	}
}


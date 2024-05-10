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

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#include "fsl_uart_freertos.h"
#include "fsl_uart.h"

#include "overclock.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_UART            UART2
#define DEMO_UART_CLKSRC     SYS_CLK
#define DEMO_UART_CLK_FREQ   CLOCK_GetFreq(SYS_CLK)
#define DEMO_UART_RX_TX_IRQn UART2_RX_TX_IRQn
/* Task priorities. */
#define uart_task_PRIORITY (configMAX_PRIORITIES - 1)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void uart_task(void *pvParameters);

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t *to_send_GGA  = "$PUBX,40,GGA,0,1,0,0*5B";
uint8_t *to_send_RMC  = "$PUBX,40,RMC,0,0,0,0*47";
char *send_ring_overrun     = "\r\nRing buffer overrun!\r\n";
char *send_hardware_overrun = "\r\nHardware buffer overrun!\r\n";
uint8_t background_buffer[1000];
uint8_t recv_buffer[100];
uint8_t data_gps[100];

uart_rtos_handle_t handle;
struct _uart_handle t_handle;

uart_rtos_config_t uart_config = {
    .baudrate    = 57600,
    .parity      = kUART_ParityDisabled,
    .stopbits    = kUART_OneStopBit,
    .buffer      = background_buffer,
    .buffer_size = sizeof(background_buffer),
};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */
int main(void)
{
    /* Init board hardware. */
	CLOCK_EnableClock(kCLOCK_PortD);

	    /* PORTB16 (pin E10) is configured as UART4_RX */
	PORT_SetPinMux(PORTD, 2U, kPORT_MuxAlt3);

	    /* PORTB17 (pin E9) is configured as UART4_TX */
	PORT_SetPinMux(PORTD, 3U, kPORT_MuxAlt3);

	CLOCK_EnableClock(kCLOCK_PortB);

		    /* PORTB16 (pin E10) is configured as UART4_RX */
	PORT_SetPinMux(PORTB, 16U, kPORT_MuxAlt3);

		    /* PORTB17 (pin E9) is configured as UART4_TX */
	PORT_SetPinMux(PORTB, 17U, kPORT_MuxAlt3);

	SIM->SOPT5 = ((SIM->SOPT5 &
		                   /* Mask bits to zero which are setting */
		                   (~(SIM_SOPT5_UART0TXSRC_MASK)))

		                  /* UART 0 transmit data source select: UART0_TX pin. */
		                  | SIM_SOPT5_UART0TXSRC(SOPT5_UART0TXSRC_UART_TX));
	overclocking();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    PRINTF("HOLA");

    NVIC_SetPriority(DEMO_UART_RX_TX_IRQn, 3);
    __set_BASEPRI(10 << (8 - __NVIC_PRIO_BITS));
    if (xTaskCreate(uart_task, "Uart_task", configMINIMAL_STACK_SIZE + 100, NULL, uart_task_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    vTaskStartScheduler();
    for (;;)
        ;
}

/*!
 * @brief Task responsible for loopback.
 */
static void uart_task(void *pvParameters)
{
    int error;
    size_t n = 0;

    uart_config.srcclk = DEMO_UART_CLK_FREQ;
    uart_config.base   = DEMO_UART;

    if (kStatus_Success != UART_RTOS_Init(&handle, &t_handle, &uart_config))
    {
        vTaskSuspend(NULL);
    }

    /* Send introduction message. */
    /*
    if (kStatus_Success != UART_RTOS_Send(&handle, (uint8_t *)to_send_GGA, strlen(to_send_GGA)))
    {
        vTaskSuspend(NULL);
    }

    if (kStatus_Success != UART_RTOS_Send(&handle, (uint8_t *)to_send_RMC, strlen(to_send_RMC)))
        {
            vTaskSuspend(NULL);
        }
	*/
//    /* Receive user input and send it back to terminal. */
    int peso = 0;
    int fin = 0;
    int start_capture = 0;
    do
    {
        error = UART_RTOS_Receive(&handle, recv_buffer, sizeof(recv_buffer), &n);
        if (error == kStatus_UART_RxHardwareOverrun)
        {
            PRINTF("UART Rx Hardware Overrun \n");
        }
        if (error == kStatus_UART_RxRingBufferOverrun)
        {
        	 PRINTF("UART Rx ring buffer Overrun \n");
        }
        if (n > 0)
        {
        	for(int i =0; i < 100; i++){
				if(recv_buffer[i] == 0x24){
					start_capture = 1;
				}
				if(start_capture){
					data_gps[start_capture - 1] = recv_buffer[i];
					start_capture++;
				}
				if(recv_buffer[i] == 0x0A){
					start_capture = 0;
					if(data_gps[3] == 0x52 | (data_gps[3] == 0x47 & data_gps[4] == 0x47)){
						PRINTF(data_gps);
					}
					for(int t =0; t < 100; t++){
						data_gps[t] = 0;
					}
				}
        	}
        }
    } while (1);
    UART_RTOS_Deinit(&handle);
    vTaskSuspend(NULL);
}

/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*  Standard C Included Files */
#include <string.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/*  SDK Included Files */
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"
#include "fsl_i2c_freertos.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_I2C_MASTER_BASE     (I2C0_BASE)
#define EXAMPLE_I2C_MASTER_IRQN     (I2C0_IRQn)
#define EXAMPLE_I2C_MASTER_CLK_SRC  (I2C0_CLK_SRC)
#define EXAMPLE_I2C_MASTER_CLK_FREQ CLOCK_GetFreq((I2C0_CLK_SRC))

#define EXAMPLE_I2C_SLAVE_BASE     (I2C1_BASE)
#define EXAMPLE_I2C_SLAVE_IRQN     (I2C1_IRQn)
#define EXAMPLE_I2C_SLAVE_CLK_SRC  (I2C1_CLK_SRC)
#define EXAMPLE_I2C_SLAVE_CLK_FREQ CLOCK_GetFreq((I2C1_CLK_SRC))

#define SINGLE_BOARD   0
#define BOARD_TO_BOARD 1

#define EXAMPLE_CONNECT_I2C SINGLE_BOARD
#if (EXAMPLE_CONNECT_I2C == BOARD_TO_BOARD)
#define isMASTER         0
#define isSLAVE          1
#define I2C_MASTER_SLAVE isMASTER
#endif
#if (EXAMPLE_CONNECT_I2C == BOARD_TO_BOARD)
#define EXAMPLE_I2C_DEALY_COUNT 1000u
#endif

#define EXAMPLE_I2C_MASTER ((I2C_Type *)EXAMPLE_I2C_MASTER_BASE)
#define EXAMPLE_I2C_SLAVE  ((I2C_Type *)EXAMPLE_I2C_SLAVE_BASE)

#define DEVICE_ID 0x10
#define ADDRESS_READ 0x03
#define I2C_BAUDRATE               (100000) /* 100K */
#define I2C_DATA_LENGTH            (6)     /* MAX is 256 */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

uint8_t g_slave_buff[I2C_DATA_LENGTH];
uint8_t g_master_buff[I2C_DATA_LENGTH];

uint8_t data_to_sent[100];

i2c_master_handle_t *g_m_handle;
i2c_slave_handle_t g_s_handle;
SemaphoreHandle_t i2c_sem;

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Task priorities. */
#define slave_task_PRIORITY  (configMAX_PRIORITIES - 1)
#define master_task_PRIORITY (configMAX_PRIORITIES - 2)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void slave_task(void *pvParameters);
#if ((I2C_MASTER_SLAVE == isMaster) || (EXAMPLE_CONNECT_I2C == SINGLE_BOARD))
static void master_task(void *pvParameters);
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

int main(void)
{
    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    /* Set interrupt priorities */
    NVIC_SetPriority(EXAMPLE_I2C_SLAVE_IRQN, 2);
    NVIC_SetPriority(EXAMPLE_I2C_MASTER_IRQN, 3);

    PRINTF("\r\n==FreeRTOS I2C example start.==\r\n");
#if (EXAMPLE_CONNECT_I2C == SINGLE_BOARD)
    PRINTF("This example use one i2c instance as master and another as slave on one board.\r\n");
#elif (EXAMPLE_CONNECT_I2C == BOARD_TO_BOARD)
    PRINTF("This example use two boards to connect with one as master and another as slave.\r\n");
#endif

    if (xTaskCreate(master_task, "Slave_task", configMINIMAL_STACK_SIZE + 100, NULL, slave_task_PRIORITY, NULL) !=
        pdPASS)
    {
        PRINTF("Failed to create slave task");
        while (1)
            ;
    }

    vTaskStartScheduler();
    for (;;)
        ;
}

/*!
 * @brief Data structure and callback function for slave I2C communication.
 */

typedef struct _callback_message_t
{
    status_t async_status;
    SemaphoreHandle_t sem;
} callback_message_t;

#if (I2C_MASTER_SLAVE == isSLAVE)
static void i2c_slave_callback(I2C_Type *base, i2c_slave_transfer_t *xfer, void *userData)
{
    callback_message_t *cb_msg = (callback_message_t *)userData;
    BaseType_t reschedule      = 0;

    switch (xfer->event)
    {
        /*  Transmit request */
        case kI2C_SlaveTransmitEvent:
            /*  Update information for transmit process */
            xfer->data     = g_slave_buff;
            xfer->dataSize = I2C_DATA_LENGTH;
            break;

        /*  Receive request */
        case kI2C_SlaveReceiveEvent:
            /*  Update information for received process */
            xfer->data     = g_slave_buff;
            xfer->dataSize = I2C_DATA_LENGTH;
            break;

        /*  Transfer done */
        case kI2C_SlaveCompletionEvent:
            cb_msg->async_status = xfer->completionStatus;
            xSemaphoreGiveFromISR(cb_msg->sem, &reschedule);
            portYIELD_FROM_ISR(reschedule);
            break;

        default:
            break;
    }
}
#endif

static void master_task(void *pvParameters)
{
    i2c_rtos_handle_t master_rtos_handle;
    i2c_master_config_t masterConfig;
    i2c_master_transfer_t masterXfer;
    uint32_t sourceClock;
    status_t status;

    /* Set up i2c master to send data to slave */
    for (uint32_t i = 0; i < I2C_DATA_LENGTH; i++)
    {
        g_master_buff[i] = i;
    }

    PRINTF("Master will send data :");
    for (uint32_t i = 0; i < I2C_DATA_LENGTH; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", g_master_buff[i]);
    }
    PRINTF("\r\n\r\n");

    /*
     * masterConfig.baudRate_Bps = 100000U;
     * masterConfig.enableStopHold = false;
     * masterConfig.glitchFilterWidth = 0U;
     * masterConfig.enableMaster = true;
     */
    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = I2C_BAUDRATE;
    sourceClock               = EXAMPLE_I2C_MASTER_CLK_FREQ;

    status = I2C_RTOS_Init(&master_rtos_handle, EXAMPLE_I2C_MASTER, &masterConfig, sourceClock);
    if (status != kStatus_Success)
    {
        PRINTF("I2C master: error during init, %d", status);
    }

    g_m_handle = &master_rtos_handle.drv_handle;

    memset(&masterXfer, 0, sizeof(masterXfer));
    masterXfer.slaveAddress   = DEVICE_ID;
    masterXfer.direction      = kI2C_Write;
    masterXfer.subaddress     = 0x0A;
    masterXfer.subaddressSize = 1;
    masterXfer.data           = g_master_buff;
    masterXfer.dataSize       = I2C_DATA_LENGTH;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    status = I2C_RTOS_Transfer(&master_rtos_handle, &masterXfer);
    if (status != kStatus_Success)
    {
        PRINTF("I2C master: error during write transaction, %d", status);
    }
#if (EXAMPLE_CONNECT_I2C == BOARD_TO_BOARD)
    /* Delay to wait slave is ready */
    SDK_DelayAtLeastUs(5000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
#endif
    /* Set up master to receive data from slave. */

    for (uint32_t i = 0; i < I2C_DATA_LENGTH; i++)
    {
        g_master_buff[i] = 0;
    }

    masterXfer.slaveAddress   = DEVICE_ID;
    masterXfer.direction      = kI2C_Read;
    masterXfer.subaddress     = ADDRESS_READ;
    masterXfer.subaddressSize = 1;
    masterXfer.data           = g_master_buff;
    masterXfer.dataSize       = I2C_DATA_LENGTH;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    status = I2C_RTOS_Transfer(&master_rtos_handle, &masterXfer);
    if (status != kStatus_Success)
    {
        PRINTF("I2C master: error during read transaction, %d", status);
    }
#if (EXAMPLE_CONNECT_DSPI == BOARD_TO_BOARD)
    else
    {
        xSemaphoreGive(i2c_sem);
    }
#endif

#if (EXAMPLE_CONNECT_I2C == SINGLE_BOARD)
    /* Transfer completed. Check the data. */
    for (uint32_t i = 0; i < I2C_DATA_LENGTH; i++)
    {
        if (g_slave_buff[i] != g_master_buff[i])
        {
            PRINTF("\r\nError occurred in the transfer ! \r\n");
            break;
        }
    }
#endif

    PRINTF("Master received data :");
    for (uint32_t i = 0; i < I2C_DATA_LENGTH; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", g_master_buff[i]);
    }
    PRINTF("\r\n\r\n");

    PRINTF("\r\nEnd of FreeRTOS I2C example.\r\n");

    vTaskSuspend(NULL);
}

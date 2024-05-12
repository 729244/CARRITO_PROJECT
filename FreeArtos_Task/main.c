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
#include "overclock.h"
#include "GPS.h"

#include "fsl_uart_freertos.h"
#include "fsl_uart.h"

#include "fsl_i2c.h"
#include "fsl_i2c_freertos.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_DSPI_SLAVE_BASEADDR SPI2
#define TRANSFER_SIZE 50U /*! Transfer dataSize */
#define ECCHO 10u
#define TRIGGER 0u
#define TURN_RIGHT 1
#define TURN_LEFT -1
#define STRAIGHT 2

#define FORDWARD 0
#define BACKWARD 1

/* DSPI user callback */
void DSPI_SlaveUserCallback(SPI_Type *base, dspi_slave_handle_t *handle, status_t status, void *userData);
/* Task priorities. */
#define hello_task_PRIORITY (configMAX_PRIORITIES - 1)


//I2C Definitions
#define EXAMPLE_I2C_MASTER_BASE     (I2C0_BASE)
#define EXAMPLE_I2C_MASTER_IRQN     (I2C0_IRQn)
#define EXAMPLE_I2C_MASTER_CLK_SRC  (I2C0_CLK_SRC)
#define EXAMPLE_I2C_MASTER_CLK_FREQ CLOCK_GetFreq((I2C0_CLK_SRC))

#define EXAMPLE_I2C_MASTER ((I2C_Type *)EXAMPLE_I2C_MASTER_BASE)

#define DEVICE_ID 0x10
#define ADDRESS_READ 0x03
#define I2C_BAUDRATE               (100000) /* 100K */
#define I2C_DATA_LENGTH            (6)     /* MAX is 256 */

//UART Definitions
#define DEMO_UART            UART2
#define DEMO_UART_CLKSRC     SYS_CLK
#define DEMO_UART_CLK_FREQ   CLOCK_GetFreq(SYS_CLK)
#define DEMO_UART_RX_TX_IRQn UART2_RX_TX_IRQn
/* Task priorities. */
#define uart_task_PRIORITY (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void spi_init(void *pvParameters);
static void uart_init(void *pvParameters);
static void uart_get_gps_data(void *pvParameters);
static void get_size_spi(void *pvParameters);
static void i2c_init(void *pvParameters);
static void i2c_read_compass(void *pvParameters);
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

//I2C
i2c_rtos_handle_t master_rtos_handle;
i2c_master_config_t masterConfig;
i2c_master_transfer_t masterXfer;
uint32_t sourceClock_i2c;
uint8_t g_master_buff[I2C_DATA_LENGTH];
i2c_master_handle_t *g_m_handle;

//UART
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

GPSData gps_lon_lat;

TaskHandle_t spi_init_handle;
TaskHandle_t get_size_spi_handle;
TaskHandle_t i2c_init_handle;
TaskHandle_t i2c_read_compass_handle;
TaskHandle_t uart_init_handle;
TaskHandle_t uart_read_gps_handle;
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
        //PRINTF("This is DSPI slave transfer completed callback. \r\n");
        //PRINTF("It's a successful transfer. \r\n\r\n");
        for(uint8_t i = 0; i<((slaveRxSize[0]-48)*5)+1;i++){
        	if((i & 0x0fU) == 0u){
        		PRINTF("\r\n");
        	}
        	PRINTF(" %c",slaveRxData[i]);
        }
		PRINTF("\r\n");
		slaveRxSize[0] == 0x00;
	    xTaskResumeFromISR(get_size_spi_handle);
    }else{
    	PRINTF("%c",slaveRxSize[0]);
    	PRINTF("\r\n");
    	if(slaveRxSize[0]== 48){ //verify if there is 0 in size
    		xTaskResumeFromISR(get_size_spi_handle);
    	}
    	else{
    		 xTaskResumeFromISR(get_objets_spi_handle);
    	}
    }
}
void PORTA_IRQHandler(void)
{
    GPIO_PortClearInterruptFlags(GPIOA, 1U << ECCHO);
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
	overclocking();
    /* Init board hardware. */
	CLOCK_EnableClock(kCLOCK_PortA);
	CLOCK_EnableClock(kCLOCK_PortB);
	CLOCK_EnableClock(kCLOCK_PortC);
	CLOCK_EnableClock(kCLOCK_PortD);
	CLOCK_EnableClock(kCLOCK_PortE);
	CLOCK_SetSimSafeDivs();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
	//UART pin init
	CLOCK_EnableClock(kCLOCK_PortD);
	PORT_SetPinMux(PORTD, 2U, kPORT_MuxAlt3);
	PORT_SetPinMux(PORTD, 3U, kPORT_MuxAlt3);

	//I2C pin init
	const port_pin_config_t porte_pin24_config = {/* Internal pull-up resistor is enabled */
                                                    kPORT_PullUp,
                                                    /* Fast slew rate is configured */
                                                    kPORT_FastSlewRate,
                                                    /* Passive filter is disabled */
                                                    kPORT_PassiveFilterDisable,
                                                    /* Open drain is enabled */
                                                    kPORT_OpenDrainEnable,
                                                    /* Low drive strength is configured */
                                                    kPORT_LowDriveStrength,
                                                    /* Pin is configured as I2C0_SCL */
                                                    kPORT_MuxAlt2,
                                                    /* Pin Control Register fields [15:0] are not locked */
                                                    kPORT_UnlockRegister};
    /* I2C0_SCL */
    PORT_SetPinConfig(PORTB, 24U, &porte_pin24_config);

    const port_pin_config_t porte_pin25_config = {/* Internal pull-up resistor is enabled */
                                                    kPORT_PullUp,
                                                    /* Fast slew rate is configured */
                                                    kPORT_FastSlewRate,
                                                    /* Passive filter is disabled */
                                                    kPORT_PassiveFilterDisable,
                                                    /* Open drain is enabled */
                                                    kPORT_OpenDrainEnable,
                                                    /* Low drive strength is configured */
                                                    kPORT_LowDriveStrength,
                                                    /* Pin is configured as I2C0_SDA */
                                                    kPORT_MuxAlt2,
                                                    /* Pin Control Register fields [15:0] are not locked */
                                                    kPORT_UnlockRegister};
    /*I2C0_SDA */
    PORT_SetPinConfig(PORTB, 25U, &porte_pin25_config);

	NVIC_SetPriority(EXAMPLE_I2C_MASTER_IRQN, 3);

	
    PRINTF("INICIO\r\n");
    xTaskCreate(spi_init, "spi_init", configMINIMAL_STACK_SIZE + 100, NULL, 2U, &spi_init_handle);
    //xTaskCreate(motor_init, "motor_init", configMINIMAL_STACK_SIZE + 100, NULL, 3U, &motor_init_handle);
    xTaskCreate(ultrasonic_init,"ultrasonic_init",configMINIMAL_STACK_SIZE + 100,NULL,2U,&ultrasonic_init_handle);

    xTaskCreate(get_size_spi, "get_size_spi", configMINIMAL_STACK_SIZE + 100, NULL, 1U, &get_size_spi_handle);
    xTaskCreate(get_objets_spi,"get_objets_spi",configMINIMAL_STACK_SIZE + 100,NULL,1U,&get_objets_spi_handle);

    xTaskCreate(ultrasonic,"ultrasonic",configMINIMAL_STACK_SIZE + 100,NULL,1U,&ultrasonic_handle);
    //xTaskCreate(turn,"turn",configMINIMAL_STACK_SIZE + 100,NULL,1U,&turn_handle);
    //xTaskCreate(turn15,"turn15",configMINIMAL_STACK_SIZE + 100,NULL,1U,&turn15_handle);
    //xTaskCreate(stopAndResetDirection,"stopAndResetDirection",configMINIMAL_STACK_SIZE + 100,NULL,1U,&reset_direction_handle);
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
	NVIC_SetPriority(SPI2_IRQn, 3u);
	vTaskDelete(spi_init_handle);
}

static void uart_init(void *pvParameters)
{
	uart_config.srcclk = DEMO_UART_CLK_FREQ;
    uart_config.base   = DEMO_UART;

    if (kStatus_Success != UART_RTOS_Init(&handle, &t_handle, &uart_config))
    {
        vTaskSuspend(NULL);
    }
	vTaskDelete(uart_init_handle);
}

static void uart_read_gps(void *pvParameters)
{
	int error;
	size_t n = 0;
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
				if(recv_buffer[i] == 0x24){ //$
					start_capture = 1;
				}
				if(start_capture){
					data_gps[start_capture - 1] = recv_buffer[i]; //Rellenar datos
					start_capture++;
				}
				if(recv_buffer[i] == 0x0A){ //New line
					start_capture = 0;
					//TODO: verify this to work correctly
					if(data_gps[3] == 0x52){
						decodeGPRMC(data_gps, &gps_lon_lat);
						vTaskSuspend(uart_read_gps_handle);
					}
					else if((data_gps[3] == 0x47) & (data_gps[4] == 0x47)){
						decodeGPGGA(data_gps, &gps_lon_lat);
						vTaskSuspend(uart_read_gps_handle);
					}
					for(int t =0; t < 100; t++){
						data_gps[t] = 0;
					}
				}
        	}
        }
    } while (1);
}

static void i2c_init(void *pvParameters)
{
	status_t status;

	/*
		* masterConfig.baudRate_Bps = 100000U;
		* masterConfig.enableStopHold = false;
		* masterConfig.glitchFilterWidth = 0U;
		* masterConfig.enableMaster = true;
		*/
	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = I2C_BAUDRATE;
	sourceClock_i2c               = EXAMPLE_I2C_MASTER_CLK_FREQ;

	status = I2C_RTOS_Init(&master_rtos_handle, EXAMPLE_I2C_MASTER, &masterConfig, sourceClock_i2c);
	if (status != kStatus_Success)
	{
		PRINTF("I2C master: error during init, %d", status);
	}
	g_m_handle = &master_rtos_handle.drv_handle;
	vTaskDelete(i2c_init_handle);
}

static void i2c_read_compass(void *pvParameters)
{

	status_t status;
	while(1){
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
		//TODO: need to test this and do the strcuture save the data
		vTaskSuspend(i2c_read_compass_handle);
	}
}

static void get_size_spi(void *pvParameters)
{
	dspi_transfer_t slaveXfer;
	while(1){
		flag_spi = 1;
		slaveRxSize[0] = 0x00;
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
		for (uint8_t i = 0; i < (slaveRxSize[0]-48)*5+1; i++)
		{
			slaveRxData[i] = 0U;
		}
		uint32_t size_tranfer = 0;
		if(slaveRxSize[0] == 48){
			size_tranfer = 0;
		}else{
			size_tranfer = ((slaveRxSize[0]-48)*5)+1;
		}
		/* Set slave transfer ready to receive data */
		slaveXfer.txData      = NULL;
		slaveXfer.rxData      = slaveRxData;
		slaveXfer.dataSize    = size_tranfer;
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
    PORT_SetPinMux(PORTA, ECCHO, kPORT_MuxAsGpio);
    GPIO_PinInit(GPIOA, ECCHO, &input);
    PORT_SetPinInterruptConfig(PORTA, ECCHO, kPORT_InterruptEitherEdge);
    PORT_SetPinMux(PORTD, TRIGGER, kPORT_MuxAsGpio);
    GPIO_PinInit(GPIOD, TRIGGER, &output);
    GPIO_PortClear(GPIOD, 1U << TRIGGER);
	pit_config_t pitConfig;
	PIT_GetDefaultConfig(&pitConfig);
	PIT_Init(PIT, &pitConfig);
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, USEC_TO_COUNT(500000,  CLOCK_GetFreq(kCLOCK_BusClk)));
	PIT_StopTimer(PIT, kPIT_Chnl_0);
	NVIC_SetPriority(PORTA_IRQn, 2u);
	__set_BASEPRI(10 << (8 - __NVIC_PRIO_BITS));
	EnableIRQ(PORTA_IRQn);
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


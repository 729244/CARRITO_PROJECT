#include <stdint.h>
#include <stdbool.h>
#include "fsl_clock.h"
#include "pwm_controller.h"
#include "fsl_gpio.h"

#define TURN_RIGHT 1
#define TURN_LEFT -1
#define STRAIGHT 2

#define FORDWARD 0
#define BACKWARD 1


void delay(uint32_t t_us);
void turn(uint8_t angle,int8_t direction);
void turn15(int8_t direction, uint8_t fordward_backward_selector);
void stopAndResetDirection();
void init_ojitos();
uint8_t g_current_direction = STRAIGHT;

int main()
{	CLOCK_SetSimSafeDivs();
	init_pwm(pwm_channel_2);
	init_pwm(pwm_channel_1);
	init_ojitos();
	delay(5000000);
	uint32_t valor = 0;
	//PWM2 direction
	while(1)
	{
		valor = GPIO_PinRead(GPIOD, 4);
		GPIO_PinWrite(GPIOD, 5, valor);
		/*
		pwm_update(pwm_channel_1, 50);
		delay(1000000);
		stopAndResetDirection();

		turn(90,TURN_RIGHT);
		pwm_update(pwm_channel_1, 50);
		delay(1000000);
		stopAndResetDirection();

		turn(180,TURN_LEFT);
		pwm_update(pwm_channel_1, 50);
		delay(1000000);
		stopAndResetDirection();

		turn(90,TURN_RIGHT);
		init_pwm(pwm_channel_1);
		pwm_update(pwm_channel_1, -100);
		pwm_update(pwm_channel_1, -100);
		delay(1000000);
		stopAndResetDirection();
		delay(1000000);
		*/

	}
	return 0;
}

void init_ojitos(){
	//PWM 3(pin 4) Y 4(pin 5), 3 PARA ECHO, 4 PARA TRIGGER
	//RELOJ
	CLOCK_EnableClock(kCLOCK_PortD);
	//PCR
	PORT_SetPinMux(PORTD, 4, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTD, 5, kPORT_MuxAsGpio);
	//GPIO DIRECTION
	gpio_pin_config_t config;
	config.pinDirection = kGPIO_DigitalOutput;
	config.outputLogic = 0;
	GPIO_PinInit(GPIOD, 5, &config);
	config.pinDirection = kGPIO_DigitalInput;
	GPIO_PinInit(GPIOD, 4, &config);
}

void stopAndResetDirection(){
	pwm_update(pwm_channel_2, 0);
	pwm_update(pwm_channel_1, 0);
	g_current_direction = STRAIGHT;
	delay(350000);
}

void turn(uint8_t angle,int8_t direction){
	switch(angle){
	case 15:
		turn15(direction,FORDWARD);
		stopAndResetDirection();
		break;
	case 30:
		turn15(direction*-1,BACKWARD);
		stopAndResetDirection();
		turn15(direction,FORDWARD);
		stopAndResetDirection();
		break;
	case 45:
		turn15(direction*-1,BACKWARD);
		stopAndResetDirection();
		turn15(direction,FORDWARD);
		turn15(direction,FORDWARD);
		stopAndResetDirection();
		break;
	case 60:
		turn15(direction*-1,BACKWARD);
		turn15(direction*-1,BACKWARD);
		stopAndResetDirection();
		turn15(direction,FORDWARD);
		turn15(direction,FORDWARD);
		stopAndResetDirection();
		break;
	case 75:
		turn15(direction*-1,BACKWARD);
		turn15(direction*-1,BACKWARD);
		stopAndResetDirection();
		turn15(direction,FORDWARD);
		turn15(direction,FORDWARD);
		turn15(direction,FORDWARD);
		stopAndResetDirection();
		break;
	case 90:
		turn15(direction*-1,BACKWARD);
		turn15(direction*-1,BACKWARD);
		turn15(direction*-1,BACKWARD);
		stopAndResetDirection();
		turn15(direction,FORDWARD);
		turn15(direction,FORDWARD);
		turn15(direction,FORDWARD);
		stopAndResetDirection();
		break;
	case 180:
		turn(90,direction);
		turn(90,direction);
		stopAndResetDirection();
		break;
	}
}

void turn15(int8_t direction, uint8_t fordward_backward_selector){
	//Funcion para girar 15° hacia una dirección
	if(direction != g_current_direction){
		if(TURN_RIGHT == direction){
			pwm_update(pwm_channel_2, -300);
			g_current_direction = TURN_RIGHT;
			delay(100000);
		}
		else{
			pwm_update(pwm_channel_2, 300);
			g_current_direction = TURN_LEFT;
			delay(100000);
		}
	}
	if(FORDWARD == fordward_backward_selector){
		pwm_update(pwm_channel_1, 50);
	}
	else{
		init_pwm(pwm_channel_1);
		pwm_update(pwm_channel_1, -75);
	}
	delay(660000);
}

void volatile delay(uint32_t t_us)
{
	uint32_t n_counts = USEC_TO_COUNT(t_us, CLOCK_GetFreq(kCLOCK_CoreSysClk))/10;
	uint32_t count;
	for(count = 0; count < n_counts; count ++);
}

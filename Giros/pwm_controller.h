/*
 * author: Luis Roberto Vega Campos
 * date  : 09/November/2023
 * brief : This module control pwm of FRDDRONE K66,
 * 		   this pwm is general, for servo, and motor drivers
 */

#ifndef PWM_CONTROLLER_H_
#define PWM_CONTROLLER_H_

#include <stdint.h>
#include "MK66F18.h"
#include "fsl_ftm.h"
#include "fsl_port.h"
#include "fsl_clock.h"

#define FTM0_IRQ        42
#define PWM_FTM         FTM0
#define NEUTRAL_TIME_US 1500
#define MAX_PWM_TIME_US 2000
#define MIN_PWM_TIME_US 1000

typedef enum _pwm_t
{
	pwm_channel_1 = 0,
	pwm_channel_2,
	pwm_channel_3,
	pwm_channel_4,
	pwm_channel_5,
	pwm_channel_6
} pwm_t;

// Initialize pwm module -------------------
// pwm : pwm_channel
void init_pwm(pwm_t pwm);
// -----------------------------------------

// Update pwm channel ----------------------
// pwm : pwm_channel
// duty: duty_cycle, range -> (-500,500).
//       For motor driver -500 is full backward and 500 is full forward
void pwm_update(pwm_t pwm, int16_t duty);
// -----------------------------------------

#endif /* PWM_CONTROLLER_H_ */

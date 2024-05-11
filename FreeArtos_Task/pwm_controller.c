/*
 * author: Luis Roberto Vega Campos
 * date  : 09/November/2023
 * brief : This module control pwm of FRDDRONE K66,
 * 		   this pwm is general, for servo, and motor drivers
 */
#include "pwm_controller.h"

volatile static bool       g_ftm_initialized   = false;
volatile static uint32_t   g_clock_frequency   = 0;
volatile static uint16_t   g_pwm_duty[6]       = {0};
volatile static bool       g_pwm_enable[6]     = {0};
volatile static bool       g_pwm_updated[6]    = {0};
const           ftm_chnl_t g_channels[]        = {kFTM_Chnl_0,
											      kFTM_Chnl_3,
											      kFTM_Chnl_4,
											      kFTM_Chnl_5,
											      kFTM_Chnl_6,
											      kFTM_Chnl_7};

const           clock_ip_name_t g_port_clocks[] = {kCLOCK_PortC,
												   kCLOCK_PortA,
												   kCLOCK_PortD,
												   kCLOCK_PortD,
												   kCLOCK_PortD,
												   kCLOCK_PortD};

const           PORT_Type *        g_pwm_port[] = {PORTC, PORTA, PORTD, PORTD, PORTD, PORTD};

const           uint32_t           g_pwm_pin[]  = {1,     6,     4,     5,     6,     7};

const           port_mux_t         g_port_alt[] = {kPORT_MuxAlt4,
												   kPORT_MuxAlt3,
												   kPORT_MuxAlt4,
												   kPORT_MuxAlt4,
												   kPORT_MuxAlt4,
												   kPORT_MuxAlt4};


// Initialize pwm module -------------------
// pwm : pwm_channelPWM_FTM
void init_pwm(pwm_t pwm)
{
	ftm_config_t config;
	__disable_irq();
	if(false == g_ftm_initialized)
	{
		g_clock_frequency = CLOCK_GetFreq(kCLOCK_BusClk);
		FTM_GetDefaultConfig(&config);
		config.bdmMode = kFTM_BdmMode_3;
		config.prescale = kFTM_Prescale_Divide_4;
		FTM_Init(PWM_FTM, &config);
		FTM_EnableInterrupts(PWM_FTM, kFTM_TimeOverflowInterruptEnable);
		g_ftm_initialized = true;
		NVIC_EnableIRQ(FTM0_IRQ);
		NVIC_SetPriority(FTM0_IRQ, 0);
	}
	ftm_chnl_pwm_signal_param_t pwm_config =
	{
			g_channels[(uint8_t)pwm],
			kFTM_HighTrue,
			50u,
			0u,
			false,
			false
	};
	CLOCK_EnableClock(g_port_clocks[(uint8_t)pwm]);
	PORT_SetPinMux((PORT_Type*)g_pwm_port[(uint8_t)pwm], g_pwm_pin[(uint8_t)pwm], g_port_alt[(uint8_t)pwm]);

	FTM_StopTimer(PWM_FTM);
	FTM_SetupPwm(PWM_FTM, &pwm_config, 1u, kFTM_EdgeAlignedPwm, 50u, g_clock_frequency/4);
	PWM_FTM->CONTROLS[(uint8_t)g_channels[(uint8_t)pwm]].CnV = USEC_TO_COUNT(NEUTRAL_TIME_US, g_clock_frequency/4);
	g_pwm_duty[(uint8_t)pwm]   = NEUTRAL_TIME_US;
	g_pwm_enable[(uint8_t)pwm] =  true;
	FTM_SetSoftwareTrigger(PWM_FTM, true);
	FTM_StartTimer(PWM_FTM, kFTM_SystemClock);
	__enable_irq();
}
// -----------------------------------------

// Update pwm channel ----------------------
// pwm : pwm_channel
// duty: duty_cycle, range -> (-500,500).
//       For motor driver -500 is full backward and 500 is full forward
void pwm_update(pwm_t pwm, int16_t duty)
{
	if(500 < duty)
	{
		duty = 500;
	}
	else if(-500 > duty)
	{
		duty = -500;
	}
	g_pwm_duty[(uint8_t)pwm]    = (uint16_t)(NEUTRAL_TIME_US + duty);
	g_pwm_updated[(uint8_t)pwm] = true;
}
// -----------------------------------------

void FTM0_IRQHandler()
{
	uint8_t index = 0;
	for(index = 0; index < 6; index ++)
	{
		if(true == g_pwm_enable[index] && true == g_pwm_updated[index])
		{
			FTM_StopTimer(PWM_FTM);
			PWM_FTM->CONTROLS[(uint8_t)g_channels[(uint8_t)index]].CnV = USEC_TO_COUNT(g_pwm_duty[index], g_clock_frequency/4);
			FTM_SetSoftwareTrigger(PWM_FTM, true);
			FTM_StartTimer(PWM_FTM, kFTM_SystemClock);
			g_pwm_updated[index] = false;
		}
	}
	FTM_ClearStatusFlags(PWM_FTM, kFTM_TimeOverflowInterruptEnable);
}

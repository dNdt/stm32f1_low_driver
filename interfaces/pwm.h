/*
 * pwm.h
 *
 *  Created on: Oct 28, 2017
 *      Author: demid
 */

#pragma once

#include "stm32f1xx_hal_conf.h"
#include "mc_hardware_interface_pwm.h"
#include "pin.h"

typedef struct {
	TIM_TypeDef *TIMx;
	uint32_t chnl; // (TIM_CHANNEL_1, ..., TIM_CHANNEL_4)
	uint16_t period_pwm; // usec
	const pin* const pwmPin;
}pwmCfg_t;

class pwm : public PWM::pwm_base {
public:
	pwm(const pwmCfg_t* const cfg) : cfg(cfg) {	};
	void init		(void);
	void reinit 	(void) const;
	void on			(void) ;
	void off		(void) ;
	void setDuty 	(const float &duty) const;
private:
	const pwmCfg_t* const cfg;
	TIM_HandleTypeDef htim_pwm;
};

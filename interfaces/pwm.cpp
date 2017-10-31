/*
 * pwm.cpp
 *
 *  Created on: Oct 28, 2017
 *      Author: demid
 */

#include "pwm.h"
#include "string.h"

void pwm_clock_en( TIM_TypeDef *TIMx){
	switch( (uint32_t) TIMx){
#ifdef TIM1
	case TIM1_BASE: __HAL_RCC_TIM1_CLK_ENABLE(); break;
#endif
#ifdef TIM2
	case TIM2_BASE: __HAL_RCC_TIM2_CLK_ENABLE(); break;
#endif
	}
}

void pwm::init(){
	pwm_clock_en(cfg->TIMx);
	cfg->pwmPin->init();
	uint32_t one_tick = 0;
	if(cfg->TIMx == TIM1){
		 one_tick = cfg->period_pwm *72; // APB2_clock = 72 000 000
	}
	else if(cfg->TIMx == TIM2){
		 one_tick = cfg->period_pwm *36; // APB2_clock = 72 000 000
	}
	uint16_t prescaler = one_tick/0xFFFF;
	htim_pwm.Instance =  cfg->TIMx;
	htim_pwm.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim_pwm.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim_pwm.Init.Period = one_tick / ( prescaler + 1);
	htim_pwm.Init.Prescaler = prescaler;
	HAL_TIM_PWM_Init(&htim_pwm);
	TIM_OC_InitTypeDef pwm_cfg;
	memset(&pwm_cfg, 0, sizeof(pwm_cfg));
	pwm_cfg.OCMode = TIM_OCMODE_PWM1;
	pwm_cfg.OCFastMode = TIM_OCFAST_ENABLE;
	HAL_TIM_PWM_ConfigChannel(&htim_pwm, &pwm_cfg, cfg->chnl);
}

void pwm::on() {
	HAL_TIM_PWM_Start(&htim_pwm, cfg->chnl);
}

void pwm::off() {
	HAL_TIM_PWM_Stop(&htim_pwm, cfg->chnl);
}

void pwm::reinit(void) const{
}

void pwm::setDuty(const float &duty)const{
	switch (cfg->chnl){
		case TIM_CHANNEL_1:
			htim_pwm.Instance->CCR1 = (uint32_t)(htim_pwm.Instance->ARR * duty);
			break;
		case TIM_CHANNEL_2:
			htim_pwm.Instance->CCR2 = (uint32_t)(htim_pwm.Instance->ARR * duty);
			break;
		case TIM_CHANNEL_3:
			htim_pwm.Instance->CCR3 = (uint32_t)(htim_pwm.Instance->ARR * duty);
			break;
		case TIM_CHANNEL_4:
			htim_pwm.Instance->CCR4 = (uint32_t)(htim_pwm.Instance->ARR * duty);
			break;
		default:
			break;
		}
}

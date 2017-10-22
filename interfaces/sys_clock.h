/*
 * sys_clock.h
 *
 *  Created on: Oct 22, 2017
 *      Author: demid
 */
#pragma once

#include "stm32f1xx_hal_conf.h"
#include "mc_hardware_interface_sys_clock.h"

#ifdef __cplusplus

#ifdef HAL_GPIO_MODULE_ENABLED

using namespace SYSCLOCK;

typedef struct {
	extosc extclock;
	uint32_t sysclock;
	uint32_t apb1clock;
	uint32_t apb2clock;
}sysclockCfg_t;

class sysclock : public sysClock_base{
public:
	sysclock(const sysclockCfg_t* const cfg) : cfg(cfg){};
	result init ();
private:
	result findPrescallers(int &AHB, int &APB1, int &APB2 );
	result getPLLmask( int mul, uint32_t &pllMask);
	result getAHBmask( int div, uint32_t &ahbMask); //возвращает маску регистра соответсвующую значению делителя, если маск не найдена возвращает 0
	result getAPBmask( int div, uint32_t &apbMask); //возвращает маску регистра соответсвующую значению делителя, если маск не найдена возвращает 0
	uint32_t getFlashLatencyMask(); //возвращает маску регистра соответсвующую значению делителя, если маск не найдена возвращает 0
	const sysclockCfg_t* const cfg;
};

#endif

#endif


/*
 * sys_clock.cpp
 *
 *  Created on: Oct 22, 2017
 *      Author: demid
 */


#include "sys_clock.h"
#include "stm32f1xx_hal_conf.h"

result sysclock::init(){
	if(this->cfg->sysclock > 72000000 || this->cfg->apb1clock > 36000000 || this->cfg->apb2clock > 72000000){
		return ERR;
	}
	// Enable Power Control clock
	__HAL_RCC_PWR_CLK_ENABLE();
	//Переключаеся на тактирование от внутреннего генератора на 8МГц
	HAL_RCC_DeInit();
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	//приведём внешнюю частоту тактирования к 8МГц
	switch(this->cfg->extclock){
	case(MHz8):
		RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
		break;
	case(MHz16):
		RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
		break;
	default:
		return ERR;
		break;
	}
	//Находим множитель для получения желаемой системной частоты. Считаем что на входе умножителя 8МГц
	int mul = this->cfg->sysclock/8000000;
	uint32_t pllMulMask  = 0;
	if(this->getPLLmask(mul, pllMulMask) == ERR){
		return ERR;
	}
	RCC_OscInitStruct.PLL.PLLMUL = pllMulMask;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	// сконфигурируем множетили шин
	int AHBdiv = 0, APB1div = 0, APB2div = 0;
	if(this->findPrescallers(AHBdiv, APB1div, APB2div) == ERR){
		return ERR;
	}
	//проверим наличие прескеллеров для данного МК (обязательно!)
	uint32_t AHPdivMask = 0, APB1divMask = 0, APB2divMask = 0;
	if( getAHBmask(AHBdiv, AHPdivMask) == ERR || getAPBmask(APB1div, APB1divMask) == ERR ||  getAPBmask(APB2div, APB2divMask) == ERR){
		return ERR; //один из прескеллеров не определён для данного МК
	}
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
	  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = AHPdivMask;
	RCC_ClkInitStruct.APB1CLKDivider = APB1divMask;
	RCC_ClkInitStruct.APB2CLKDivider = APB2divMask;
	//определим время отклика флэш в зависимости от частоты системной шины
	uint32_t flashLatencyMask = getFlashLatencyMask();
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, flashLatencyMask);
	return OK;
}

// находит значения делителей, в случае успеха возвращает OK
result sysclock::findPrescallers(int &AHB, int &APB1, int &APB2 ){
	for(int AHBtmp = 1; AHBtmp < 512; AHBtmp *= 2){
		for(int APB1tmp = 1; APB1tmp < 16; APB1tmp *= 2){
			int newAPB1 = this->cfg->sysclock / AHBtmp / APB1tmp;
			if( this->cfg->apb1clock % newAPB1 == 0 ){
				//делители подходящие для первой шины найдены, пробуем подобрать для второй
				for(int APB2tmp = 1; APB2tmp < 16; APB2tmp *= 2 ){
					int newAPB2 = this->cfg->sysclock / AHBtmp / APB2tmp;
					if(this->cfg->apb2clock % newAPB2 == 0){
						//все коэффициенты найдены успешно
						AHB = AHBtmp;
						APB1 = APB1tmp;
						APB2 = APB2tmp;
						return OK;
					}
				}
			}
		}
	}
	return ERR;
}

//преобразует числовое значение в маску регистра для данного контроллера
result sysclock::getPLLmask( int mul, uint32_t &pllMask){
	switch(mul){
		case(2): pllMask = RCC_PLL_MUL2; return OK;
		case(3): pllMask = RCC_PLL_MUL3; return OK;
		case(4): pllMask = RCC_PLL_MUL4; return OK;
		case(5): pllMask = RCC_PLL_MUL5; return OK;
		case(6): pllMask = RCC_PLL_MUL6; return OK;
		case(7): pllMask = RCC_PLL_MUL7; return OK;
		case(8): pllMask = RCC_PLL_MUL8; return OK;
		case(9): pllMask = RCC_PLL_MUL9; return OK;
		default: return ERR;
		}
		return ERR;
}

//преобразует числовое значение в маску регистра для данного контроллера
result sysclock::getAHBmask( int div, uint32_t &ahbMask){
	switch(div){
	case(1): ahbMask = RCC_SYSCLK_DIV1; return OK;
	case(2): ahbMask = RCC_SYSCLK_DIV2; return OK;
	case(4): ahbMask = RCC_SYSCLK_DIV4; return OK;
	case(8): ahbMask = RCC_SYSCLK_DIV8; return OK;
	case(16): ahbMask = RCC_SYSCLK_DIV16; return OK;
	case(64): ahbMask = RCC_SYSCLK_DIV64; return OK;
	case(128): ahbMask = RCC_SYSCLK_DIV128; return OK;
	case(256): ahbMask = RCC_SYSCLK_DIV256; return OK;
	case(512): ahbMask = RCC_SYSCLK_DIV512; return OK;
	default: return ERR;
	}
	return ERR;
}

result sysclock::getAPBmask( int div, uint32_t &apbMask){

	switch(div){
	case(1): apbMask = RCC_HCLK_DIV1; return OK;
	case(2): apbMask = RCC_HCLK_DIV2; return OK;
	case(4): apbMask = RCC_HCLK_DIV4; return OK;
	case(8): apbMask = RCC_HCLK_DIV8; return OK;
	case(16): apbMask = RCC_HCLK_DIV16; return OK;
	default: return ERR;
	}
	return ERR;
}

uint32_t sysclock::getFlashLatencyMask(){
	uint32_t sysClock = this->cfg->sysclock;
	uint32_t retMask = 0;
	if(sysClock <= 24000000){
		retMask = FLASH_LATENCY_0;
	}
	if( (sysClock > 24000000) && (sysClock <= 48000000)){
		retMask = FLASH_LATENCY_1;
	}
	if( (sysClock > 48000000) ){
		retMask = FLASH_LATENCY_2;
	}
	return retMask;
}


/*
 * uart.h
 *
 *  Created on: Nov 1, 2017
 *      Author: demid
 */

#pragma once

#include "mc_hardware_interface_uart.h"
#include "pin.h"

using namespace UART;

typedef struct{
	USART_TypeDef *uartInst;
	uint32_t baudRate;
	DMA_Channel_TypeDef *dmaTxInst;
	const pin* const txPin;
	const pin* const rxPin;
}uartCfg_t;

class uart: public uart_base{
public:
			 uart	( const uartCfg_t* const cfg ): cfg(cfg) {};
	void	 init	( void );
	result	 tx		( uint8_t *txBuf, uint16_t ln, uint32_t timeoutMs = 0 );
	result	 rx		( uint8_t *rxBuf, uint16_t rxBufLn, uint32_t timeoutMs = 0 );
	void 	 dmaIrqHandler(void); // TX  Handler
	void	 txIrqCallback(void);
	void 	 irqHandler(void);
	void 	 rxIrqCallback(void);

private:
	const uartCfg_t* const cfg;
	UART_HandleTypeDef huart;
	DMA_HandleTypeDef hdma;
	SemaphoreHandle_t txSem = NULL; // семафор синхронизации по завершению отправки
	StaticSemaphore_t txSemBuffer; //
	SemaphoreHandle_t rxSem = NULL; // семафор синхронизации по завершению приёма
	StaticSemaphore_t rxSemBuffer; //
	QueueHandle_t rxQueue; // очередь приёма данных по уарт
	StaticQueue_t xQueueBuffer;
#define QUEUE_LENGTH 500
#define ITEM_SIZE sizeof(uint8_t)
	uint8_t ucQueueStorage[ QUEUE_LENGTH * ITEM_SIZE ];

};

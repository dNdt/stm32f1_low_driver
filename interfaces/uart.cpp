/*
 * uart.cpp
 *
 *  Created on: Nov 1, 2017
 *      Author: demid
 */

#include "user_os.h"
#include "uart.h"
#include "stm32f1xx_hal_conf.h"


static void uart_clock_en( USART_TypeDef *UARTx){
	switch( (uint32_t) UARTx){
#ifdef UART1
	case USART1_BASE: __HAL_RCC_USART1_CLK_ENABLE(); break;
#endif
#ifdef UART2
	case USART2_BASE: __HAL_RCC_USART2_CLK_ENABLE(); break;
#endif
#ifdef UART3
	case USART3_BASE: 	__HAL_RCC_USART3_CLK_ENABLE(); break;
#endif
	}
}

static IRQn_Type dma_channel_irqn( DMA_Channel_TypeDef *DMAx){
	IRQn_Type ret;
	switch( (uint32_t) DMAx){
	case DMA1_Channel1_BASE: ret = DMA1_Channel1_IRQn; break;
	case DMA1_Channel2_BASE: ret = DMA1_Channel2_IRQn; break;
	case DMA1_Channel3_BASE: ret = DMA1_Channel3_IRQn; break;
	case DMA1_Channel4_BASE: ret = DMA1_Channel4_IRQn; break;
	case DMA1_Channel5_BASE: ret = DMA1_Channel5_IRQn; break;
	case DMA1_Channel6_BASE: ret = DMA1_Channel6_IRQn; break;
	case DMA1_Channel7_BASE: ret = DMA1_Channel7_IRQn; break;
	default: while(1); break;
	}
	return ret;
}

void uart::init(){
	cfg->txPin->init();
	cfg->rxPin->init();
	uart_clock_en(cfg->uartInst);
//	__HAL_RCC_USART3_CLK_ENABLE();
//	инит уарта с дма
	this->huart.Instance = cfg->uartInst;
	this->huart.Init.BaudRate = cfg->baudRate;
	this->huart.Init.WordLength = UART_WORDLENGTH_8B;
	this->huart.Init.StopBits = UART_STOPBITS_1;
	this->huart.Init.Mode = UART_MODE_TX_RX;
	this->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart.Init.Parity  = UART_PARITY_NONE;
	huart.Init.OverSampling  = UART_OVERSAMPLING_16;
	HAL_UART_Init(&this->huart);
	HAL_NVIC_SetPriority(USART3_IRQn, configMAX_PRIORITIES - 2, 0);
	HAL_NVIC_EnableIRQ(USART3_IRQn);
	if ( cfg->dmaTxInst != NULL ) {
		DMA_HandleTypeDef *dmatx = NULL;
		dmatx = &this->hdma;
		dmatx->Instance =  cfg->dmaTxInst;
		dmatx->Init.Direction = DMA_MEMORY_TO_PERIPH;
		dmatx->Init.PeriphInc = DMA_PINC_DISABLE;
		dmatx->Init.MemInc = DMA_MINC_ENABLE;
		dmatx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		dmatx->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		dmatx->Init.Mode = DMA_NORMAL;
		dmatx->Init.Priority = DMA_PRIORITY_HIGH;
		HAL_DMA_Init ( dmatx );
		dmatx->Parent = &this->huart;
	}
	HAL_NVIC_SetPriority(dma_channel_irqn(cfg->dmaTxInst), configMAX_PRIORITIES - 2, 0);
	HAL_NVIC_EnableIRQ(dma_channel_irqn(cfg->dmaTxInst));
	this->txSem = xSemaphoreCreateBinaryStatic(&this->txSemBuffer);
	this->rxSem = xSemaphoreCreateBinaryStatic(&this->rxSemBuffer);
	this->rxQueue = xQueueCreateStatic(QUEUE_LENGTH, ITEM_SIZE, this->ucQueueStorage, &this->xQueueBuffer );
}


UART::result uart::rx(uint8_t *rxBuf, uint16_t rxBufLn, uint32_t timeoutMs ){
	(void) rxBuf; (void) rxBufLn;
	result rv = UART::result::ERR;
	if(HAL_UART_Receive_IT(&this->huart, rxBuf, rxBufLn) == HAL_OK){
		if(xSemaphoreTake(this->rxSem, ( TickType_t)timeoutMs) == pdTRUE){
			rv = UART::result::OK;
		}else{
			rv = UART::result::TIMEOUT;
		}
	}else{
		rv = UART::result::ERR;
	}
	return rv;
}


UART::result uart::tx(uint8_t *txBuf, uint16_t ln, uint32_t timeoutMs ){
	//почистим семафор на всякий случай
	xSemaphoreTake(this->txSem, 0);
	if(HAL_UART_Transmit_DMA(&this->huart, txBuf, ln) != HAL_OK){
		return UART::result::ERR;
	}
	if(xSemaphoreTake(this->txSem, ( TickType_t ) timeoutMs) == pdTRUE){
		return UART::result::OK;
	}else{
		return UART::result::TIMEOUT;
	}
}

void uart::irqHandler(){
	 HAL_UART_IRQHandler(&this->huart);
}

void uart::dmaIrqHandler(void){
	HAL_DMA_IRQHandler(this->huart.hdmatx);
}

void uart::txIrqCallback(void){
	xSemaphoreGiveFromISR( this->txSem , NULL );
}

void uart::rxIrqCallback(void){
	xSemaphoreGiveFromISR( this->rxSem , NULL );
}


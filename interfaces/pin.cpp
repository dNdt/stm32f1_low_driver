#include "pin.h"

static void gpio_clk_en (  const GPIO_TypeDef* GPIOx, const uint32_t remap_mode ) {
    if ( remap_mode != 0 ) {
        __HAL_RCC_AFIO_CLK_ENABLE();
        SET_BIT( AFIO->MAPR, remap_mode );
    }

    switch ( (uint32_t)GPIOx ) {
#ifdef GPIOA
        case GPIOA_BASE: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
#endif

#ifdef GPIOB
        case GPIOB_BASE: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
#endif

#ifdef GPIOC
        case GPIOC_BASE: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
#endif

#ifdef GPIOD
        case GPIOD_BASE: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
#endif

#ifdef GPIOE
        case GPIOE_BASE: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
#endif

#ifdef GPIOF
        case GPIOF_BASE: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
#endif

#ifdef GPIOG
        case GPIOG_BASE: __HAL_RCC_GPIOG_CLK_ENABLE(); break;
#endif

#ifdef GPIOH
        case GPIOH_BASE: __HAL_RCC_GPIOH_CLK_ENABLE(); break;
#endif
    }
}

void pin::init ( void ) const {
    gpio_clk_en( this->GPIOx, this->remap_mode );
    HAL_GPIO_Init( this->GPIOx, ( GPIO_InitTypeDef * )this->cfg );
}


void pin::set ( void ) const {
    HAL_GPIO_WritePin ( this->GPIOx, ( uint16_t )this->cfg->Pin, GPIO_PIN_SET);
}

void pin::reset ( void ) const {
    HAL_GPIO_WritePin ( this->GPIOx, ( uint16_t )this->cfg->Pin, GPIO_PIN_RESET);
}

void pin::toggle ( void ) const {
    HAL_GPIO_TogglePin ( this->GPIOx, ( uint16_t )this->cfg->Pin );
}

void pin::set ( bool state ) const {
    if ( state ) {
        this->set();
    } else {
        this->reset();
    }
}

void pin::set ( int state ) const {
    this->set( static_cast< bool >( state ) );
}

void pin::set ( uint8_t state ) const {
    this->set( static_cast< bool >( state ) );
}

bool pin::read ( void ) const {
    return HAL_GPIO_ReadPin ( this->GPIOx, ( uint16_t )this->cfg->Pin );
}


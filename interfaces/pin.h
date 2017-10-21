#pragma once

#ifdef __cplusplus

#include "stm32f1xx_hal_conf.h"
#include "mk_hardware_interfaces_pin.h"

#ifdef HAL_GPIO_MODULE_ENABLED
class pin : public pin_base {
public:
    pin( GPIO_TypeDef* const GPIOx, const GPIO_InitTypeDef* const cfg, const uint32_t remap_mode ) :
        GPIOx( GPIOx ), cfg( cfg ), remap_mode( remap_mode ) {}

    void	init    ( void )            const;                        // Перед инициализацией включается тактирование портов.

    void    set     ( void )            const;
    void    reset   ( void )            const;
    void	toggle	( void )            const;

    void    set     ( bool state )      const;
    void    set     ( int state )       const;
    void    set     ( uint8_t state )   const;

    bool    read    ( void )            const;

private:
            GPIO_TypeDef*           const GPIOx;
    const   GPIO_InitTypeDef*       const cfg;
    const   uint32_t                remap_mode;
};

#endif

#endif

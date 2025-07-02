#ifndef LED_BASE_HPP
#define LED_BASE_HPP

#include "gpio.h"

#define LED_ON()      HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET)
#define LED_OFF()     HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET)

class LedBase
{
public:
    void TurnOn();
    void TurnOff();
};

#endif

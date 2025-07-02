#ifndef COMMON_INC_H
#define COMMON_INC_H

/* ----------------------------------C Scope---------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif

#include "oled.h"
#include "rtos_inc.h"

void Main();

#ifdef __cplusplus
}

/* ----------------------------------C++ Scope---------------------------------------- */
#include "can_base.hpp"
#include "led_base.hpp"
#include "adc_base.hpp"
#include "voltage.hpp"
#include "interface_can.hpp"

#endif
#endif

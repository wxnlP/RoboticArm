#ifndef ADC_BASE_HPP
#define ADC_BASE_HPP
#include "adc.h"

class AdcBase
{
public:
    void Init();
    uint32_t GetAnalogValue();
};

#endif

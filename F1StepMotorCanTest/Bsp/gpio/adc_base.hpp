#ifndef ADC_BASE_HPP
#define ADC_BASE_HPP

#include "adc.h"

class AdcBase
{
private:
    ADC_HandleTypeDef *adcInstance;
public:
    explicit AdcBase(ADC_HandleTypeDef *iAdc) : adcInstance(iAdc){};
    void Init();
    uint32_t GetAnalogValue();
};

#endif

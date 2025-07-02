#ifndef VOLTAGE_HPP
#define VOLTAGE_HPP

#include "adc_base.hpp"

class Voltage
{
private:
    AdcBase *adcBase;
public:
    explicit Voltage(AdcBase *iAdc) : adcBase(iAdc){};
    void Init();
    float GetVoltageActual();
};


#endif

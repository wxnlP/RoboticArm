#ifndef VOLTAGE_HPP
#define VOLTAGE_HPP

#include "adc_base.hpp"

class Voltage : public AdcBase
{
public:
    float GetVoltageActual();
};


#endif

#include "voltage.hpp"


/* @brief  获取实际的电压值
 * @param  无参数
 * @retval 无返回值
 */
float Voltage::GetVoltageActual()
{
    uint32_t value = this->GetAnalogValue();
    float temp = ((float)value / 4095.0f) * 3.30f;
    return (temp / 3.126f) * 12.6f;
}
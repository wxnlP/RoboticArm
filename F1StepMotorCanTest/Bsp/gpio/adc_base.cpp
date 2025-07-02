#include "adc_base.hpp"


/* @brief  初始化ADC1测量电池电压（连续非扫描模式）
 * @param  无参数
 * @retval 无返回值
 */
void AdcBase::Init()
{
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
}

/* @brief  获取ADC模拟值
 * @param  无参数
 * @retval 无返回值
 */
uint32_t AdcBase::GetAnalogValue()
{
    return HAL_ADC_GetValue(&hadc1);
}


/* 电池电压测量函数,串口查看 */
// static void ADC_Voltage_Uart(void)
// {
//     uint16_t analog_value = 0;
//     double voltage = 0.0;
//     char message[25] = "";
//     while (1)
//     {
//         analog_value = ADC_Voltage_AdcBase();
//         voltage = ADC_Voltage_Actual();
//         sprintf(message, "ADC: %hu   %.2fV\r\n", analog_value, voltage);
//         HAL_UART_Transmit(&huart1, (uint8_t *) message, strlen(message), HAL_MAX_DELAY);
//         vTaskDelay(500);
//     }
// }


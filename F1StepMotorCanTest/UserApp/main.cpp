#include "common_inc.h"

/* ---------Objects Initialization-------------------------------------------------------------- */
// LED初始化
LedBase led;
Voltage voltage;

/* ---------Thread Definitions-------------------------------------------- */
// LED Thread
osThreadId_t ledThreadHandle;
static void ThreadLed(void *arg)
{
    (void)arg;
    while (true)
    {
        led.TurnOn();
        osDelay(100);
        led.TurnOff();
        osDelay(100);
    }
}

// Voltage Thread
osThreadId_t voltageThreadHandle;
static void ThreadVoltage(void *arg)
{
    (void)arg;
    OLED_Init();
    voltage.Init();
    double voltage_ = 0.0;
    while (true)
    {
        voltage_ = voltage.GetVoltageActual();
        OLED_ShowVoltage(88, 2, voltage_, 16);
        osDelay(500);
    }
}


void Main()
{
    /* -------------------------------------------------------------------- */
    osThreadAttr_t ledAttr = {};
    ledAttr.name = "ledTask";
    ledAttr.stack_size = 128 * 4;
    ledAttr.priority = osPriorityNormal;
    ledThreadHandle = osThreadNew(ThreadLed, nullptr, &ledAttr);

    /* -------------------------------------------------------------------- */
    osThreadAttr_t voltageAttr = {};
    voltageAttr.name  = "voltageTask";
    voltageAttr.stack_size = 128 * 4;
    voltageAttr.priority = osPriorityNormal;
    voltageThreadHandle = osThreadNew(ThreadVoltage, nullptr, &voltageAttr);
}
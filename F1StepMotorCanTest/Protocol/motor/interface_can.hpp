#ifndef INTERFACE_CAN_HPP
#define INTERFACE_CAN_HPP

#include "can_base.hpp"

typedef enum
{
    SYS_VERSION,            /* 读取固件版本和对应的硬件版本 */
    SYS_RI,                 /* 读取相电阻和相电感 */
    SYS_PID,                /* 读取PID参数 */
    SYS_BUS_VOLTAGE,        /* 读取总线电压 */
    SYS_PHASE_CURRENT,      /* 读取相电流 */
    SYS_ENCODER,            /* 读取经过线性化校准后的编码器值 */
    SYS_INPUT_PULSE,        /* 读取输入脉冲数 */
    SYS_TARGET_POSITION,    /* 读取电机目标位置 */
    SYS_REAL_POSITION_OPEN, /* 读取电机实时设定的目标位置（开环模式的实时位置） */
    SYS_REAL_SPEED,         /* 读取电机实时转速 */
    SYS_REAL_POSITION,      /* 读取电机实时位置 */
    SYS_POSITION_ERROR,     /* 读取电机位置误差 */
    SYS_STATUS_FLAG,        /* 读取电机状态标志位（使能/到位/堵转） */
    SYS_DRIVER_CONFIG,      /* 读取驱动配置参数 */
    SYS_STATUS,             /* 读取系统状态参数 */
    SYS_TO_ZERO_FLAG,       /* 读取回零标志位 */
}SystemParams_t;

class InterfaceCan
{
private:
    CanContext canContext = {};
    uint8_t cmd_buffer[20] = {};
public:
    void SendCmd(const uint8_t *cmd, uint8_t length);

    void ResetToZero(uint8_t addr);
    void ReleaseStallProtection(uint8_t addr);
    void ReadSystemParams(uint8_t addr, SystemParams_t sysParam);
    void ModifyCtrlMode(uint8_t addr, bool saveFlag, uint8_t ctrlMode);
    void ModifyIdAddress(uint8_t addr, bool saveFlag, uint8_t newAddr);
    void MotorEnable(uint8_t addr, bool enable, bool multiSyn);
    void VelocityControl(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t accel, bool multiSyn);
    void PositionControl(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t accel, uint32_t pulseNum, bool raMode, bool multiSyn);
    void StopNow(uint8_t addr, bool multiSyn);
    void MultiMachineSyn(uint8_t addr);
    void SetZeroPosition(uint8_t addr, bool saveFlag);
    void ToZeroPosition(uint8_t addr, uint8_t zeroMode, bool multiSyn);
    void ToZeroInerrupt(uint8_t addr);
    void ModifyToZeroParams(uint8_t addr, bool saveFlag, uint8_t zeroMode, uint8_t zeroDir, uint16_t zeroVel,
                            uint32_t timeOut, uint16_t icVel, uint16_t icEle, uint16_t icTime, bool autoTrigger);
};

#endif

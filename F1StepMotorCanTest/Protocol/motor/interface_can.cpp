/**
 * 张大头步进电机驱动，Emm_v5.0
 *
 * * 1.地址（ID_Addr）默认为1，可设置范围为1-255，0为广播地址
 * -- 地址 1-10 可以在菜单 ID_Addr 上进行选择修改， 大于 10 的地址通过串口/CAN通讯发送命令进行修改。
 * -- 0为广播地址，以0地址发送命令，则所有的闭环电机都会执行这条命令；但只有地址为1的电机会回复命令，其他地址不回复。
 * * 2.不同的命令对应不同的功能码和指令数据
 *
 * * 3.校验字节可选0x6B、XOR校验、CRC-8校验三种，默认为0x6B
 * -- 0x6B: 每条命令的最后一个字节都是0x6B作为校验
 * -- XOR: 每条命令最后一个字节都是将前面全部的字节进行异或作为校验
 * -- CRC-8: 每条命令最后一个字节都是将前面全部的字节进行CRC-8计算作为校验
 *
 *
 */

#include "interface_can.hpp"

static CanBase canBase(&hcan);

void InterfaceCan::SendCmd(const uint8_t* cmd, uint8_t length)
{
    // 去除ID地址和功能码后的命令长度
    uint8_t length_ = length - 2;
    uint8_t i = 0, j = 0, pkgsNum = 0;
    while (i < length_)
    {
        // 记录未发送的数据字节数
        length_ -= i;
        canContext.txMsg.StdId = 0x00;
        // 扩展格式的报文ID
        canContext.txMsg.ExtId = ((uint32_t)cmd[0] << 8) | (uint32_t)pkgsNum;
        // 每次包发送的第一个字节固定为功能码
        canContext.pMsgData[0] = cmd[1];
        canContext.txMsg.RTR = CAN_RTR_DATA;
        canContext.txMsg.IDE = CAN_ID_EXT;
        if (length_ < 8)
        {
            // 发生length_个字节数据
            for (j = 0; j < length_; j++, i++)
            {
                canContext.pMsgData[j + 1] = cmd[j + 2];
                canContext.txMsg.DLC = length_;
            }
        }
        else
        {
            // 发生8个字节数据
            for (j = 0; j < 7; j++, i++)
            {
                canContext.pMsgData[j + 1] = cmd[j + 2];
                canContext.txMsg.DLC = 8;
            }
        }
        // 发送数据
        canBase.SendMessage(&canContext.txMsg, canContext.pMsgData);
        // 记录发送的第几包数据
        ++pkgsNum;
    }
}

/* -------------触发动作命令列表-------------------------------------------------------------- */

/**
 * @brief 将当前的位置角度清零
 * @note 将当前位置角度、位置误差、脉冲数等全部清零。
 * @param addr 电机地址
 * @retval 地址 + 0x0A + 状态码 + 校验字节
 */
void InterfaceCan::ResetToZero(uint8_t addr)
{
    cmd_buffer[0] = addr; // 地址
    cmd_buffer[1] = 0x0A; // 功能码
    cmd_buffer[2] = 0x6D; // 辅助码
    cmd_buffer[3] = 0x6B; // 校验码
    this->SendCmd(cmd_buffer, 4);
}

/**
 * @brief 解除堵转保护
 * @note 电机发生堵转后，发送该命令可以解除堵转保护
 * @param addr 电机地址
 * @retval 地址 + 0x0E + 状态码 + 校验字节
 */
void InterfaceCan::ReleaseStallProtection(uint8_t addr)
{
    cmd_buffer[0] = addr; // 地址
    cmd_buffer[1] = 0x0E; // 功能码
    cmd_buffer[2] = 0x52; // 辅助码
    cmd_buffer[3] = 0x6B; // 校验码
    this->SendCmd(cmd_buffer, 4);
}

/* -------------读取参数命令列表-------------------------------------------------------------- */
/**
 * @brief 读取系统参数
 * @param addr 电机地址
 * @param sysParam 要读取的系统参数
 * @retval 与要读取的系统参数相关
 */
void InterfaceCan::ReadSystemParams(uint8_t addr, SystemParams_t sysParam)
{
    // 记录命令字节数
    uint8_t i = 0;
    // 地址
    cmd_buffer[i] = addr;
    i++;
    // 参数命令
    switch (sysParam)
    {
        case SYS_VERSION            : cmd_buffer[i] = 0x1F; i++; break;
        case SYS_RI                 : cmd_buffer[i] = 0x20; i++; break;
        case SYS_PID                : cmd_buffer[i] = 0x21; i++; break;
        case SYS_BUS_VOLTAGE        : cmd_buffer[i] = 0x24; i++; break;
        case SYS_PHASE_CURRENT      : cmd_buffer[i] = 0x27; i++; break;
        case SYS_ENCODER            : cmd_buffer[i] = 0x31; i++; break;
        case SYS_INPUT_PULSE        : cmd_buffer[i] = 0x32; i++; break;
        case SYS_TARGET_POSITION    : cmd_buffer[i] = 0x33; i++; break;
        case SYS_REAL_POSITION_OPEN : cmd_buffer[i] = 0x34; i++; break;
        case SYS_REAL_SPEED         : cmd_buffer[i] = 0x35; i++; break;
        case SYS_REAL_POSITION      : cmd_buffer[i] = 0x36; i++; break;
        case SYS_POSITION_ERROR     : cmd_buffer[i] = 0x37; i++; break;
        case SYS_STATUS_FLAG        : cmd_buffer[i] = 0x3A; i++; break;
        case SYS_DRIVER_CONFIG      : cmd_buffer[i] = 0x42; i++; cmd_buffer[i] = 0x6c; i++; break;
        case SYS_STATUS             : cmd_buffer[i] = 0x43; i++; cmd_buffer[i] = 0x7A; i++; break;
        case SYS_TO_ZERO_FLAG       : cmd_buffer[i] = 0x3B; i++; break;
        default: break;
    }
    // 校验位
    cmd_buffer[i] = 0x6B;
    i++;
    this->SendCmd(cmd_buffer, i);
}

/* -------------修改参数命令列表-------------------------------------------------------------- */
/**
 * @brief 修改控制模式
 * @param addr 电机地址
 * @param saveFlag 是否储存标志，false为不储存，true为储存
 * @param ctrlMode 控制模式
 *                 0:关闭脉冲输入引脚
 *                 1:开环模式
 *                 2:闭环模式；
 *                 3:让En端口复用为多圈限位开关输入引脚，Dir端口复用为到位输出高电平功能
 * @retval 地址 + 0x46 + 命令状态 + 校验字节
 */
void InterfaceCan::ModifyCtrlMode(uint8_t addr, bool saveFlag, uint8_t ctrlMode)
{
    cmd_buffer[0] = addr;
    cmd_buffer[1] = 0x46;
    cmd_buffer[2] = 0x69;
    cmd_buffer[3] = saveFlag;
    cmd_buffer[4] = ctrlMode;
    cmd_buffer[5] = 0x6B;
    this->SendCmd(cmd_buffer, 6);
}

/**
 * @brief 修改任意ID地址
 * @param addr 电机地址
 * @param saveFlag 是否储存标志，false为不储存，true为储存
 * @param newAddr 新的电机地址
 * @retval 地址 + 0xAE + 命令状态 + 校验字节
 */
void InterfaceCan::ModifyIdAddress(uint8_t addr, bool saveFlag, uint8_t newAddr)
{
    cmd_buffer[0] = addr;
    cmd_buffer[1] = 0xAE;
    cmd_buffer[2] = 0x4B;
    cmd_buffer[3] = saveFlag;
    cmd_buffer[4] = newAddr;
    cmd_buffer[5] = 0x6B;
    this->SendCmd(cmd_buffer, 6);
}


/* -------------控制动作命令列表-------------------------------------------------------------- */
/**
 * @brief 电机使能控制
 * @param addr 电机地址
 * @param enableFlag 电机使能标志
 * @param multiSyn 多机同步标志
 * @retval 地址 + 0xF3 + 命令状态 + 校验字节
 */
void InterfaceCan::MotorEnable(uint8_t addr, bool enableFlag, bool multiSyn)
{
    cmd_buffer[0] = addr;
    cmd_buffer[1] = 0xF3;
    cmd_buffer[2] = 0xAB;
    cmd_buffer[3] = enableFlag;
    cmd_buffer[4] = multiSyn;
    cmd_buffer[5] = 0x6B;
    this->SendCmd(cmd_buffer, 6);
}

/**
 * @brief 速度模式控制
 * @param addr 电机地址
 * @param dir 方向，0为顺时针，其他值为逆时针
 * @param vel 速度，范围是0~5000RPM（转/分钟）
 * @param accel 加速度，范围是0~255，0是直接启动
 * @param multiSyn 多机同步标志
 * @retval 地址 + 0xF6 + 命令状态 + 校验字节
 */
void InterfaceCan::VelocityControl(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t accel, bool multiSyn)
{
    cmd_buffer[0] = addr;
    cmd_buffer[1] = 0xF6;
    cmd_buffer[2] = dir;
    cmd_buffer[3] = (uint8_t)(vel >> 8);
    cmd_buffer[4] = (uint8_t)(vel >> 0);
    cmd_buffer[5] = accel;
    cmd_buffer[6] = multiSyn;
    cmd_buffer[7] = 0x6B;
    this->SendCmd(cmd_buffer, 8);
}

/**
 * @brief 位置模式控制
 * @param addr 电机地址
 * @param dir 方向
 *            0:顺时针
 *            others:逆时针
 * @param vel 速度，范围是0~5000RPM（转/分钟）
 * @param accel 加速度，范围是0~255，0是直接启动
 * @param pulseNum 脉冲数，范围0~(2^32-1)
 * @param raMode 相对/绝对模式标志，false为相对模式，true为绝对模式
 * @param multiSyn 多机同步标志
 * @retval 地址 + 0xFD + 命令状态 + 校验字节
 */
void InterfaceCan::PositionControl(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t accel, uint32_t pulseNum, bool raMode, bool multiSyn)
{
    cmd_buffer[0] = addr;
    cmd_buffer[1] = 0xFD;
    cmd_buffer[2] = dir;
    cmd_buffer[3] = (uint8_t)(vel >> 8);
    cmd_buffer[4] = (uint8_t)(vel >> 0);
    cmd_buffer[5] = accel;
    cmd_buffer[6] = (uint8_t)(pulseNum >> 24);
    cmd_buffer[7] = (uint8_t)(pulseNum >> 16);
    cmd_buffer[8] = (uint8_t)(pulseNum >> 8);
    cmd_buffer[9] = (uint8_t)(pulseNum >> 0);
    cmd_buffer[10] = raMode;
    cmd_buffer[11] = multiSyn;
    cmd_buffer[12] = 0x6B;
    this->SendCmd(cmd_buffer, 13);
}

/**
 * @brief 立即停止
 * @note 让电机立即停止转动（紧急刹车），可用于速度模式和位置模式都可以。
 * @param addr 电机地址
 * @param multiSyn 多机同步标志
 * @retval 地址 + 0xFE + 命令状态 + 校验字节
 */
void InterfaceCan::StopNow(uint8_t addr, bool multiSyn)
{
    cmd_buffer[0] = addr;
    cmd_buffer[1] = 0xFE;
    cmd_buffer[2] = 0x98;
    cmd_buffer[3] = multiSyn;
    cmd_buffer[4] = 0x6B;
    this->SendCmd(cmd_buffer, 5);
}

/**
 * @brief 多机同步运动
 * @note 让多个电机同步开始运动
 * @param addr 电机地址
 * @retval 地址 + 0xFF + 命令状态 + 校验字节
 */
void InterfaceCan::MultiMachineSyn(uint8_t addr)
{
    cmd_buffer[0] = addr;
    cmd_buffer[1] = 0xFF;
    cmd_buffer[2] = 0x66;
    cmd_buffer[3] = 0x6B;
    this->SendCmd(cmd_buffer, 4);
}

/* -------------原点回零命令列表-------------------------------------------------------------- */
/**
 * @brief 设置单圈回零的零点位置
 * @note 可以让电机转到想要的位置，然后发送该命令设置单圈回零的零点位置
 * @param addr 电机地址
 * @param saveFlag 是否储存标志，false为不储存，true为储存
 * @retval 地址 + 0x93 + 命令状态 + 校验字节
 */
void InterfaceCan::SetZeroPosition(uint8_t addr, bool saveFlag)
{
    cmd_buffer[0] = addr;
    cmd_buffer[1] = 0x93;
    cmd_buffer[2] = 0x88;
    cmd_buffer[3] = saveFlag;
    cmd_buffer[4] = 0x6B;
    this->SendCmd(cmd_buffer, 5);
}

/**
 * @brief 回到零点
 * @note 设置完原点回零参数后，可以发送该命令触发原点回零功能
 * @param addr 电机地址
 * @param zeroMode 回零模式
 *                 0:单圈就近回零
 *                 1:单圈方向回零
 *                 2:多圈无限位碰撞回零
 *                 3:多圈有限位开关回零
 * @param multiSyn 多机同步标志
 * @retval 地址 + 0x9A + 命令状态 + 校验字节
 */
void InterfaceCan::ToZeroPosition(uint8_t addr, uint8_t zeroMode, bool multiSyn)
{
    cmd_buffer[0] = addr;
    cmd_buffer[1] = 0x9A;
    cmd_buffer[2] = zeroMode;
    cmd_buffer[3] = multiSyn;
    cmd_buffer[4] = 0x6B;
    this->SendCmd(cmd_buffer, 5);
}

/**
 * @brief 强制中断并退出回零操作
 * @note 正在回零的过程中，可以使用该命令强制中断并退出回零操作
 * @param addr 电机地址
 * @retval 地址 + 0x9C + 命令状态 + 校验字节
 */
void InterfaceCan::ToZeroInerrupt(uint8_t addr)
{
    cmd_buffer[0] = addr;
    cmd_buffer[1] = 0x9C;
    cmd_buffer[2] = 0x48;
    cmd_buffer[3] = 0x6B;
    this->SendCmd(cmd_buffer, 4);
}

/**
 * @brief 修改原点回零参数
 * @param addr 电机地址
 * @param saveFlag 是否储存标志，false为不储存，true为储存
 * @param zeroMode 回零模式
 *                 0:单圈就近回零
 *                 1:单圈方向回零
 *                 2:多圈无限位碰撞回零
 *                 3:多圈有限位开关回零
 * @param zeroDir 回零方向
 *                0:顺时针
 *                others:逆时针
 * @param zeroVel 回零速度，单位:RPM（转/分钟）
 * @param timeOut 回零超时时间，单位:Ms（毫秒）
 * @param icVel 无限位碰撞回零检测速度，单位:RPM（转/分钟）
 * @param icEle 无限位碰撞回零检测电流，单位:Ma（毫安）
 * @param icTime 无限位碰撞回零检测时间，单位:Ms（毫秒）
 * @param autoTrigger 上电自动触发回零
 * @retval 地址 + 0x4C + 命令状态 + 校验字节
 */
void InterfaceCan::ModifyToZeroParams(uint8_t addr, bool saveFlag, uint8_t zeroMode, uint8_t zeroDir, uint16_t zeroVel,
    uint32_t timeOut, uint16_t icVel, uint16_t icEle, uint16_t icTime, bool autoTrigger)
{
    cmd_buffer[0] = addr;
    cmd_buffer[1] = 0x4C;
    cmd_buffer[2] = 0xAE;
    cmd_buffer[3] = saveFlag;
    cmd_buffer[4] = zeroMode;
    cmd_buffer[5] = zeroDir;
    cmd_buffer[6] = (uint8_t)(zeroVel >> 8);
    cmd_buffer[7] = (uint8_t)(zeroVel >> 0);
    cmd_buffer[8] = (uint8_t)(timeOut >> 24);
    cmd_buffer[9] = (uint8_t)(timeOut >> 16);
    cmd_buffer[10] = (uint8_t)(timeOut >> 8);
    cmd_buffer[11] = (uint8_t)(timeOut >> 0);
    cmd_buffer[12] = (uint8_t)(icVel >> 8);
    cmd_buffer[13] = (uint8_t)(icVel >> 0);
    cmd_buffer[14] = (uint8_t)(icEle >> 8);
    cmd_buffer[15] = (uint8_t)(icEle >> 0);
    cmd_buffer[16] = (uint8_t)(icTime >> 8);
    cmd_buffer[17] = (uint8_t)(icTime >> 0);
    cmd_buffer[18] = autoTrigger;
    cmd_buffer[19] = 0x6B;
    this->SendCmd(cmd_buffer, 20);
}

#include "can_base.hpp"


void CanBase::FilterInit()
{
    // 启用CAN功能⭐
    HAL_CAN_Start(canInstance);
    // 过滤器组，单CAN取值范围0~13
    canFilter.FilterBank = 0;
    // 过滤器标识符高16位
    canFilter.FilterIdHigh = 0x0000;
    // 过滤器标识符低16位
    canFilter.FilterIdLow = 0x0000;
    // 过滤器掩码号或标识号高16位
    canFilter.FilterMaskIdHigh = 0x0000;
    // 过滤器掩码号或标识号低16位
    canFilter.FilterMaskIdLow = 0x0000;
    // 过滤器字宽
    canFilter.FilterScale = CAN_FILTERSCALE_32BIT;
    // 过滤器工作模式
    canFilter.FilterMode = CAN_FILTERMODE_IDMASK;
    //    canFilter.SlaveStartFilterBank = x;  // 双CAN时从CAN选择过滤器组，单个CAN此参数无意义
    // 报文ID存在FIFO0还是FIFO1
    canFilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    // 激活过滤器
    canFilter.FilterActivation = CAN_FILTER_ENABLE;
    if (HAL_CAN_ConfigFilter(canInstance, &canFilter) != HAL_OK) {
        Error_Handler();
    }
}


uint8_t CanBase::SendMessage(CAN_TxHeaderTypeDef* TxMessage, uint8_t* pData)
{
    uint32_t TxMailbox;
    // 发送数据并记录结果
    return (HAL_CAN_AddTxMessage(canInstance, TxMessage, pData, &TxMailbox) == HAL_OK) ? 1 : 0;
}

uint8_t CanBase::SendMessage(uint32_t ID, uint8_t Length, uint8_t* pData)
{
    /* 初始化消息头 */
    if (Length > 8) {
        return -1;
    }

    // 标准格式ID
    canFrame.txMsg.StdId = ID;
    // 扩展格式ID
    canFrame.txMsg.ExtId = 0;
    // 远程请求位，区分数据帧(0)和遥控帧(1)
    canFrame.txMsg.RTR = CAN_RTR_DATA;
    // 扩展标志位，区分标准格式(0)和扩展格式(1)，选用一致格式则另一种格式的ID无效
    canFrame.txMsg.IDE = CAN_ID_STD;
    // 数据长度
    canFrame.txMsg.DLC = Length;
    canFrame.txMsg.TransmitGlobalTime = DISABLE;
    /* 传输数据 */
    // 接收使用的邮箱号
    uint32_t TxMailbox;
    if (HAL_CAN_AddTxMessage(canInstance, &canFrame.txMsg, pData, &TxMailbox) == HAL_OK) {
        return 1;
    }
    return 0;
}

uint8_t CanBase::ReceiveMessage(CAN_RxHeaderTypeDef* RxMessage, uint8_t* pData)
{
    return (HAL_CAN_GetRxMessage(canInstance, CAN_RX_FIFO0, RxMessage, pData) == HAL_OK) ? 1 : 0;
}

uint8_t CanBase::ReceiveMessage(uint32_t *pID, uint8_t *pLength, uint8_t* pData)
{
    /* 接收数据 */
    if (HAL_CAN_GetRxMessage(canInstance, CAN_RX_FIFO0, &canFrame.rxMsg, pData) != HAL_OK) {
        return 0;
    }
    // 判断是标准格式还是扩展格式
    if (canFrame.rxMsg.IDE == CAN_ID_STD) {
        *pID = canFrame.rxMsg.StdId;
    } else {
        *pID = canFrame.rxMsg.ExtId;
    }
    if (canFrame.rxMsg.RTR == CAN_RTR_DATA) {
        *pLength = canFrame.rxMsg.DLC;
    }
    return 1;
}

uint8_t CanBase::ReceiveFlag()
{
    if (HAL_CAN_GetRxFifoFillLevel(canInstance, CAN_RX_FIFO0) > 0) {
        return 1;
    }
    return 0;
}

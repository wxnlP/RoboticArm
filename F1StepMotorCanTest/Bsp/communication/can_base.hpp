#ifndef CAN_BASE_HPP
#define CAN_BASE_HPP

#include "can.h"

typedef struct
{
    CAN_TxHeaderTypeDef txMsg;
    CAN_RxHeaderTypeDef rxMsg;
    uint8_t* pMsgData;
} CanFrame;

class CanBase {
private:
    CanFrame canFrame = {};
    CAN_FilterTypeDef canFilter = {};
    CAN_HandleTypeDef *canInstance;
public:
    explicit CanBase(CAN_HandleTypeDef *iCan) : canInstance(iCan){};
    //
    void FilterInit();
    //
    uint8_t SendMessage(uint32_t ID, uint8_t Length, uint8_t* pData);
    //
    uint8_t SendMessage(CAN_TxHeaderTypeDef* TxMessage, uint8_t* pData);
    //
    uint8_t ReceiveMessage(uint32_t *pID, uint8_t *pLength, uint8_t* pData);
    //
    uint8_t ReceiveMessage(CAN_RxHeaderTypeDef* RxMessage, uint8_t* pData);
    //
    uint8_t ReceiveFlag();
};


#endif

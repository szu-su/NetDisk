#include "protocol.h"

PDU *mkPDU(uint uiMsgLen)
{
    uint uiPDULen = sizeof(PDU) + uiMsgLen;
    PDU *pdu = (PDU*)malloc(uiPDULen);
    memset(pdu,0,uiPDULen); //清空
    pdu->uiPDULen = uiPDULen;
    pdu->uiMsgLen  = uiMsgLen;
    return pdu;
}

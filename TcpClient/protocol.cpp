#include"protocol.h"

PDU *mkPDU(uint uiMsgLen)
{
    uint uiPDULen=sizeof(PDU)+uiMsgLen;//实际sizeof的长度没有计算uiMsgLen，所以需要加上
    PDU *pdu=(PDU*)malloc(uiPDULen);
    if(NULL==pdu)
    {
        exit(EXIT_FAILURE);
    }
    memset(pdu,0,uiPDULen);//清空
    pdu->uiPDULen=uiPDULen;
    pdu->uiMsgLen=uiMsgLen;

    return pdu;
}

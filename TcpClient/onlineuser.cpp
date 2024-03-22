#include "onlineuser.h"
#include "ui_onlineuser.h"
#include"tcpclient.h"
OnlineUser::OnlineUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineUser)
{
    ui->setupUi(this);
}

OnlineUser::~OnlineUser()
{
    delete ui;
}

void OnlineUser::showUser(PDU *pdu)
{

    if(pdu==NULL)return;
    uint uiSize=pdu->uiMsgLen/32;
    char caTmp[32];
    ui->onlinelistWidget->clear();//清空列表，因为关上显示用户，再打开会重复出现
    for(uint i=0;i<uiSize;i++)
    {
        memcpy(caTmp,(char*)(pdu->caMsg)+i*32,32);
        qDebug()<<"caTmp"<<caTmp;

        ui->onlinelistWidget->addItem(caTmp);//放到列表显示。
    }

}

void OnlineUser::on_addFriend_button_clicked()
{
    QListWidgetItem *pItem=ui->onlinelistWidget->currentItem();//获得当前列表所选文字
    qDebug()<<"在添加好友点击了"<<pItem->text();
    if(pItem)
    {
        QString friendName=pItem->text();
        QString loginName=TcpClient::getInstance().getLoginName();
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
        memcpy(pdu->caData,friendName.toStdString().c_str(),friendName.size());
        memcpy(pdu->caData+32,loginName.toStdString().c_str(),loginName.size());
        //前32个字节存储要添加的用户，后32个字节存储谁请求的
        qDebug()<<"发送添加好友请求";
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }

}


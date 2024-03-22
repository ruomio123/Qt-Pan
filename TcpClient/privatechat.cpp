#include "privatechat.h"
#include "ui_privatechat.h"
#include "protocol.h"
#include"tcpclient.h"
#include<QMessageBox>
PrivateChat::PrivateChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivateChat)
{
    ui->setupUi(this);


}

PrivateChat::~PrivateChat()
{
    delete ui;
}

PrivateChat &PrivateChat::getInstance()
{
    static PrivateChat instance;
    return instance;
}

void PrivateChat::setChatName(QString strName)
{
    m_strChatName=strName;
    m_strLoginName=TcpClient::getInstance().getLoginName();
}

void PrivateChat::updateMsg(const PDU *pdu)
{
    if(pdu==NULL)
    {
        return;
    }
    else
    {
        char caLoginName[32]={'\0'};
        memcpy(caLoginName,pdu->caData,32);
        //谁发送了什么信息
        QString strMsg=QString("%1 :%2").arg(caLoginName).arg((char*)pdu->caMsg);
        //显示在界面
        ui->showMessage->append(strMsg);
    }
}

void PrivateChat::on_sendMessage_clicked()
{
    QString strMsg=ui->inputMessage->text();
    ui->inputMessage->clear();
    if(!strMsg.isEmpty())
    {
        //发送信息给指定用户
        PDU *pdu=mkPDU(strMsg.toUtf8().size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;
        memcpy(pdu->caData,m_strLoginName.toStdString().c_str(),m_strLoginName.size());
        memcpy(pdu->caData+32,m_strChatName.toStdString().c_str(),m_strChatName.size());
        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);

        /*
         * 这段代码是想让发送者也显示自己的消息，因为两个人聊天，不能看不到自己发的消息吧
         */
        char caLoginName[32]={'\0'};
        memcpy(caLoginName,pdu->caData,32);
        QString strSendName=caLoginName;
        PrivateChat::getInstance().setChatName(strSendName);
        PrivateChat::getInstance().updateMsg(pdu);//调用update，更新显示消息
        free(pdu);
        pdu=nullptr;


    }
    else
    {
        QMessageBox::warning(this,"警告","发送信息不能为空");


    }
}


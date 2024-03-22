#include "friend.h"
#include"onlineuser.h"
#include"protocol.h"
#include"tcpclient.h"
#include<QInputDialog>
#include<QDebug>
#include"privatechat.h"
#include<QMessageBox>
#include <QPainter>
Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    m_pShowMsgTE=new QTextEdit;
    m_pFriendList=new QListWidget;//朋友列表
    m_pInputMsgLE=new QLineEdit;//信息输入框

    m_pDelFriendPB=new QPushButton("删除好友");
    m_pFlushFriendPB=new QPushButton("刷新好友");
    m_pShowOnlineUsrPB=new QPushButton("显示在线用户");
    m_pSearchUsrPB=new QPushButton("查找用户");

    m_pMsgSendPB=new QPushButton("群发");
    m_pPrivateChatPB=new QPushButton("私聊");


    QVBoxLayout *pRightPBL=new QVBoxLayout;//垂直布局
    pRightPBL->addWidget(m_pDelFriendPB);
    pRightPBL->addWidget(m_pFlushFriendPB);
    pRightPBL->addWidget(m_pShowOnlineUsrPB);
    pRightPBL->addWidget(m_pSearchUsrPB);
    pRightPBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL=new QHBoxLayout;//按钮和列表，显示框水平布局
    pTopHBL->addWidget( m_pShowMsgTE);
    pTopHBL->addWidget( m_pFriendList);
    pTopHBL->addLayout(pRightPBL);

    QHBoxLayout *pMsgHBL=new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    m_pOnline=new OnlineUser;

    QVBoxLayout *pMain=new QVBoxLayout;//垂直
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();//先隐藏不显示
    //如果online窗口不显示，是因为online主窗口没有选择水平布局
    setLayout(pMain);

    connect(m_pShowOnlineUsrPB,SIGNAL(clicked(bool)),this,SLOT(showOnline()));

    connect(m_pSearchUsrPB,SIGNAL(clicked(bool)),this,SLOT(searchUser()));//关联信号槽

    connect(m_pFlushFriendPB,SIGNAL(clicked(bool)),this,SLOT(flushFriend()));

    connect(m_pDelFriendPB,SIGNAL(clicked(bool)),this,SLOT(delFriend()));

    connect(m_pPrivateChatPB,SIGNAL(clicked(bool)),this,SLOT(privateChat()));

    connect(m_pMsgSendPB,SIGNAL(clicked(bool)),this,SLOT(groupChat()));
}



void Friend::showAllOnlineUser(PDU *pdu)
{
    if(pdu==NULL)return;
    m_pOnline->showUser(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    if(pdu==NULL)return;

    m_pFriendList->clear();
    uint uiSize=pdu->uiMsgLen/32;//计数人数
    char caName[32]={'\0'};
    for(uint i=0;i<uiSize;i++)
    {
        memcpy(caName,(char*)(pdu->caMsg)+i*32,32);
        m_pFriendList->addItem(caName);
    }
}

void Friend::updateGroupMsg(PDU *pdu)
{
    QString strMsg=QString("%1 : %2").arg(pdu->caData).arg((char*)pdu->caMsg);
    m_pShowMsgTE->append(strMsg);

}

void Friend::paintEvent(QPaintEvent *event)
{
    QPainter *painter=new QPainter(this);
    QPixmap pic = QPixmap(":/image/liella.jpg");
    painter->drawPixmap(0,0,this->width(),this->height(),pic);
    painter->end();//不写会有问题，一直提示是否用完，用完必须释放
}

void Friend::showOnline()
{
    if(m_pOnline->isHidden())
    {
        m_pOnline->show();
        PDU *pdu=mkPDU(0);
        //just a request ,the length is 0
        pdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);//
        free(pdu);
        pdu=nullptr;
    }
    else
    {
        m_pOnline->hide();
    }
}

void Friend::searchUser()
{
    //通过名字搜索
    m_strSearchName=QInputDialog::getText(this,"搜索","用户名");
    if(!m_strSearchName.isEmpty())
    {
        qDebug()<<"输入了搜索用户"<<m_strSearchName;
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USER_REQUEST;//消息类型为查找用户
        memcpy(pdu->caData,m_strSearchName.toStdString().c_str(),m_strSearchName.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);//
        free(pdu);
        pdu=nullptr;
    }
}

void Friend::flushFriend()
{
  //把自己的用户名发送给服务器，服务器才能根据你的名字查找好友
    QString strName=TcpClient::getInstance().getLoginName();
    qDebug()<<"大家好，我是"<<strName<<"我刷新了好友";
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType= ENUM_MSG_TYPE_FLUSH_FRIEND_REQUST;
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::delFriend()
{
    if(m_pFriendList->currentItem()==NULL)
    {
        return;
    }
    QString strFriendName=m_pFriendList->currentItem()->text();
    QString strSelfName=TcpClient::getInstance().getLoginName();
    qDebug()<<"要删除"<<strFriendName;
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    memcpy(pdu->caData,strSelfName.toStdString().c_str(),strSelfName.size());
    memcpy(pdu->caData+32,strFriendName.toStdString().c_str(),strFriendName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;

}

void Friend::privateChat()
{
    if(m_pFriendList->currentItem()!=NULL)
    {
        QString strFriendName=m_pFriendList->currentItem()->text();
        PrivateChat::getInstance().setChatName(strFriendName);//和谁聊天
        if(PrivateChat::getInstance().isHidden())
        {
            PrivateChat::getInstance().show();
        }
    }
    else
    {
        QMessageBox::warning(this,"私聊","请选择私聊的人");
    }
}

void Friend::groupChat()
{
    QString strMsg=m_pInputMsgLE->text();
    this->m_pInputMsgLE->clear();
   if(strMsg.isEmpty())
    {
        //空
        QMessageBox::warning(this,"群聊","信息发送不能为空");
    }
    else
    {
        //不为空就产生一个pdu
        PDU *pdu=mkPDU(strMsg.size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
        //谁发的
        QString loginName=TcpClient::getInstance().getLoginName();
        memcpy(pdu->caData,loginName.toStdString().c_str(),loginName.size());
         //放数据
        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());

        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        //wirte函数要求就是char *类型


    }
}

#include "mytcpserver.h"
#include<QDebug>
MyTcpServer::MyTcpServer()
{

}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
   //只要有客户端连接到服务器，自动调用该函数
    qDebug()<<"-----new client connect-----";
    MyTcpSocket *pTcpSocket=new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_tcpSocketList.append(pTcpSocket);

    connect(pTcpSocket,SIGNAL(offline(MyTcpSocket*)),this,SLOT(deleteSocket(MyTcpSocket*)));
    //发出下线信号
}

MyTcpServer &MyTcpServer::getInstance()
{
    //单例模式
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::repost(const char *friendName, PDU *pdu)
{
    QString strName = friendName;
    qDebug()<<"朋友的名字"<<strName;
    qDebug()<<"猜测pdu可能为空"<<pdu;
    /*
     * 历经两个小时的不断debug，终于知道了哪里出问题
     * 2023/12/30
     *就是pdu,发过来是空的,（c语言）
    */
    // 空直接返回
    if (friendName == NULL || pdu == NULL) return;


    for (int i = 0; i < m_tcpSocketList.size(); i++)
    {
        MyTcpSocket *socket = m_tcpSocketList.at(i);

        qDebug()<<"测试有无进入循环"<<socket->getName();
        // 检查 socket 是否有效
        if (socket == NULL)
            continue;

        if (socket->getName() == strName)
        {
            // 发送此pdu
            qDebug()<<"找到了这个朋友准备发送";
            socket->write((char*)pdu, pdu->uiPDULen);
            break;
        }
    }
}


void MyTcpServer::deleteSocket(MyTcpSocket *mysocket)
{
   QList<MyTcpSocket*>::iterator iter=m_tcpSocketList.begin();
   for(;iter!=m_tcpSocketList.end();iter++)
   {
       if(mysocket==*iter)
       {
           (*iter)->deleteLater();
           *iter=NULL;
           m_tcpSocketList.erase(iter);
           //删除指针指向的对象和列表里放的
           break;
       }
   }
   for(int i=0;i<m_tcpSocketList.size();i++)
   {
       qDebug()<<m_tcpSocketList.at(i)->getName();
   }
}

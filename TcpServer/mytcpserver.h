#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include<QList>
#include"mytcpsocket.h"
class MyTcpServer : public QTcpServer
{
    Q_OBJECT //继承信号槽
public:
    MyTcpServer();
    void incomingConnection(qintptr socketDescriptor);
    static MyTcpServer& getInstance();
    void repost(const char *friendName,PDU *pdu);

public slots:
    void deleteSocket(MyTcpSocket *mysocket);


private:
    QList<MyTcpSocket*> m_tcpSocketList;
};

#endif // MYTCPSERVER_H

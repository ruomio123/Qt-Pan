#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include"protocol.h"
#include<QDebug>
#include"operatedb.h"
#include<QFileDialog>
#include<QFile>
#include<QTimer>
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString getName();

    qint64 iTotal() const;
    void setITotal(qint64 newITotal);

    qint64 iRecved() const;
    void setIRecved(qint64 newIRecved);

    bool bUpload() const;
    void setBUpload(bool newBUpload);


//    QFile&  getfile();
//    void setFile(QFile &newFile);

signals:
    void offline(MyTcpSocket *mysocket);//要回收socket对象，不然会占用服务器资源

public slots:
    void receiveMsg();
    void clientOffline();
    void sendFileToClient();
private:
    QString m_strName;

    QFile m_file;//打开的文件
    qint64 m_iTotal;//文件总大小
    qint64 m_iRecved;//文件接收了多少
    bool m_bUpload;//是否处于上传文件状态
    QTimer *m_pTimer;

    void registerRequest(PDU *pdu);
    void loginRequest(PDU *pdu);
    void allOnlineRequest(PDU *pdu);
    void searchUserRequest(PDU *pdu);
    void addFriendRequest( PDU *pdu);
    void addFriendAgree(PDU *pdu);
    void addFriendRefuse(PDU *pdu);
    void flushFriendRequest( PDU *pdu);
    void deleteFriendRequest(PDU *pdu);
    void privateChatRequest( PDU *pdu);
    void groupChatRequest( PDU *pdu);
    void createDirRequest( PDU *pdu);
    void flushFileRequest( PDU *pdu);
    void deleteDirRequest( PDU *pdu);
    void renameFileRequest( PDU *pdu);
    void enterDirRequest( PDU *pdu);
    void uploadRequest(PDU *pdu);
    void downloadRequest(PDU *pdu);
};

#endif // MYTCPSOCKET_H

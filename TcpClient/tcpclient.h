#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include<QFile>
#include<QTcpSocket>
#include"protocol.h"
#include"operatewidget.h"
QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig();//加载配置,alt+enter可以快捷添加定义

    static TcpClient &getInstance();

    QTcpSocket &getTcpSocket();
    QString strCurPath() const;
    void setStrCurPath(const QString &newStrCurPath);

public slots:
    void showConnect();
    void receiveMsg();
    QString getLoginName();
private slots:
   // void on_send_pb_clicked();

    void on_login_Button_clicked();

    void on_register_Button_clicked();

    void on_logout_Button_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP;//IP
    quint16 m_usPort;//用户端口
    QString loginName;//登录使用的用户名
    //连接服务器，和服务器进行数据交互
    QTcpSocket m_tcpSocket;

    QString m_strCurPath;//记录当前用户文件夹的路径

    QFile m_file;
    void handleRegisterRespond(PDU *pdu);
    void handleLoginRespond(PDU *pdu);
    void handleAllOnlineRespond(PDU *pdu);
    void handleSearchUserRespond(PDU *pdu);
    void handleAddFriendRequest(PDU *pdu);
    void handleAddFriendRespond(PDU *pdu);
    void handleFlushFriendRespond(PDU *pdu);
    void handleDeleteFriendRespond(PDU *pdu);
    void handlePrivateChatRequest(PDU *pdu);
    void handleGroupChatRequest(PDU *pdu);
    void handleCreateDirRespond(PDU *pdu);
    void handleFlushFileRespond(PDU *pdu);
    void handleDeleteDirRespond(PDU *pdu);
    void handleRenameFileRespond(PDU *pdu);
    void handleEnterDirRespond(PDU *pdu);
    void handleUploadRespond(PDU *pdu);
    void handleDownloadRespond(PDU *pdu);
};
#endif // TCPCLIENT_H

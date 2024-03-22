#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>
#include<QFile>
#include<QTcpSocket>
QT_BEGIN_NAMESPACE
namespace Ui { class TcpServer; }
QT_END_NAMESPACE

class TcpServer : public QWidget
{
public:
    TcpServer(QWidget *parent = nullptr);
    ~TcpServer();
    void loadConfig();

private:
    Ui::TcpServer *ui;
    QString m_strIP;//IP
    quint16 m_usPort;//用户端口
};
#endif // TCPSERVER_H

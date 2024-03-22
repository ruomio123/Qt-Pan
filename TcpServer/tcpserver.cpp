#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "mytcpserver.h"
#include<QByteArray>
#include<QDebug>
#include<QMessageBox>
#include<QHostAddress>
TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    resize(300,300);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}
void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly))//按住ctrl,点击open，可以读源码
    {
        QByteArray baData=file.readAll();
        QString strData=baData.toStdString().c_str();

        file.close();

        //替换字符串
        strData.replace("\r\n"," ");
        //分割字符串，取出ip和端口号
        QStringList strList=strData.split(" ");

        m_strIP=strList.at(0);
        m_usPort=strList.at(1).toUShort();
        qDebug()<<"ip:"<<m_strIP<<" port:"<<m_usPort;

    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed");
    }
}


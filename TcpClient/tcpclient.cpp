#include "tcpclient.h"
#include "ui_tcpclient.h"
#include<QByteArray>
#include<QDebug>
#include<QMessageBox>
#include<QHostAddress>
#include"privatechat.h"
TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);

    loadConfig();

    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));//发送方，信号，接收方，处理
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(receiveMsg()));
    //服务器发送数据给客户端，客户端收到数据就会发出readRead()信号

    //连接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);


}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly))//按住ctrl,点击open，可以读源码
    {
        QByteArray baData=file.readAll();
        QString strData=baData.toStdString().c_str();
      //  qDebug()<<strData;
        file.close();

        //替换字符串
        strData.replace("\r\n"," ");
        //分割字符串，取出ip和端口号
        QStringList strList=strData.split(" ");
//        for(auto str:strList)
//        {
//            qDebug()<<"--"<<str;
//        }
        m_strIP=strList.at(0);
        m_usPort=strList.at(1).toUShort();
       // qDebug()<<"ip:"<<m_strIP<<" port:"<<m_usPort;


    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed");
    }
}

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
    //产生一个tcpclient对象，在哪里都可以调用
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

void TcpClient::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");//标题，文本

}

/*接收消息
 *
 *switch部分代码冗余，建议封装成函数(2023-12-29)
 *
 *2024-1-26 已修改
*/
void TcpClient::receiveMsg() {
    if(!OperateWidget::getInstance().getBook()->bDownloadState())
    {

    qDebug() << "-------" << m_tcpSocket.bytesAvailable();
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));

    switch (pdu->uiMsgType) {
    case ENUM_MSG_TYPE_REGISTER_RESPOND:
        handleRegisterRespond(pdu);
        break;
    case ENUM_MSG_TYPE_LOGIN_RESPOND:
        handleLoginRespond(pdu);
        break;
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:
        handleAllOnlineRespond(pdu);
        break;
    case ENUM_MSG_TYPE_SEARCH_USER_RESPOND:
        handleSearchUserRespond(pdu);
        break;
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        handleAddFriendRequest(pdu);
        break;
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
        handleAddFriendRespond(pdu);
        break;
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
        handleFlushFriendRespond(pdu);
        break;
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:
        handleDeleteFriendRespond(pdu);
        break;
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
        handlePrivateChatRequest(pdu);
        break;
    case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
        handleGroupChatRequest(pdu);
        break;
    case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:
        handleCreateDirRespond(pdu);
        break;
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:
        handleFlushFileRespond(pdu);
        break;
    case ENUM_MSG_TYPE_DEL_DIR_RESPOND:
        handleDeleteDirRespond(pdu);
        break;
    case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:
        handleRenameFileRespond(pdu);
        break;
    case ENUM_MSG_TYPE_ENTER_DIR_RESPOND:
        handleEnterDirRespond(pdu);
        break;
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:
        handleUploadRespond(pdu);
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:
        handleDownloadRespond(pdu);
        break;
    default:
        break;
    }

    free(pdu);
    pdu = NULL;
    }
    else
    {
      QByteArray buffer=getTcpSocket().readAll();
      m_file.write(buffer);
      Book *pBook=OperateWidget::getInstance().getBook();
      pBook->setIRecved(pBook->iRecved()+buffer.size());
      if(pBook->iTotal()==pBook->iRecved())
      {
        m_file.close();
        pBook->setITotal(0);
        pBook->setIRecved(0);
        pBook->setBDownloadState(0);
        QMessageBox::information(this,"提示","下载文件成功");
      }
      else if(pBook->iTotal()<pBook->iRecved())
      {
        m_file.close();
        pBook->setITotal(0);
        pBook->setIRecved(0);
        pBook->setBDownloadState(0);
        QMessageBox::critical(this,"错误","下载文件失败");
      }

    }
}
void TcpClient::handleRegisterRespond( PDU *pdu) {
    if (strcmp(pdu->caData, REGISTER_OK) == 0) {
        QMessageBox::information(this, "注册", REGISTER_OK);
    } else if (strcmp(pdu->caData, REGISTER_FAILED) == 0) {
        QMessageBox::critical(this, "注册", REGISTER_FAILED);
    }
}

void TcpClient::handleLoginRespond( PDU *pdu) {
    if (strcmp(pdu->caData, LOGIN_OK) == 0) {
        setStrCurPath(QString("D://QTProject//Pan//%1").arg(getLoginName()));
        QMessageBox::information(this, "登录", LOGIN_OK);
        OperateWidget::getInstance().show();
        this->hide();
    } else if (strcmp(pdu->caData, LOGIN_FAILED) == 0) {
        QMessageBox::critical(this, "登录", LOGIN_FAILED);
    }
}

void TcpClient::handleAllOnlineRespond( PDU *pdu) {
    OperateWidget::getInstance().getFriend()->showAllOnlineUser(pdu);
}

void TcpClient::handleSearchUserRespond(PDU *pdu) {
    if (strcmp(pdu->caData, SEARCH_USER_NO) == 0) {
        QMessageBox::information(this, "提示", QString("%1: 不存在").arg(OperateWidget::getInstance().getFriend()->m_strSearchName));
    } else if (strcmp(pdu->caData, SEARCH_USER_OFFLINE) == 0) {
        QMessageBox::information(this, "提示", QString("%1: 存在但不在线").arg(OperateWidget::getInstance().getFriend()->m_strSearchName));
    } else if (strcmp(pdu->caData, SEARCH_USER_ONLINE) == 0) {
        QMessageBox::information(this, "提示", QString("%1: 在线").arg(OperateWidget::getInstance().getFriend()->m_strSearchName));
    }
}

void TcpClient::handleAddFriendRequest(PDU *pdu) {
    char caName[32] = {'\0'};
    strncpy(caName, pdu->caData + 32, 32);
    int ret = QMessageBox::information(this, "添加好友", QString("%1 想添加你为好友").arg(caName), QMessageBox::Yes, QMessageBox::No);

    PDU *respdu = mkPDU(0);
    memcpy(respdu->caData, pdu->caData, 64);

    if (ret == QMessageBox::Yes) {
        qDebug() << this->loginName << "同意添加好友";
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
    } else if (ret == QMessageBox::No) {
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
    }

    m_tcpSocket.write((char*)respdu, respdu->uiPDULen);
    free(respdu);
    respdu = NULL;
}

void TcpClient::handleAddFriendRespond(PDU *pdu) {
    qDebug() << "客户端收到了添加好友的回复";
    QMessageBox::information(this, "添加好友", pdu->caData);
}

void TcpClient::handleFlushFriendRespond( PDU *pdu) {
    OperateWidget::getInstance().getFriend()->updateFriendList(pdu);
}

void TcpClient::handleDeleteFriendRespond(PDU *pdu) {
    qDebug() << "客户端收到删除好友成功回复";
    QMessageBox::information(this, "提示", "删除好友成功");
}

void TcpClient::handlePrivateChatRequest(PDU *pdu) {
    if (PrivateChat::getInstance().isHidden()) {
        PrivateChat::getInstance().show();
    }

    char caLoginName[32] = {'\0'};
    memcpy(caLoginName, pdu->caData, 32);
    QString strSendName = caLoginName;
    PrivateChat::getInstance().setChatName(strSendName);
    PrivateChat::getInstance().updateMsg(pdu);
}

void TcpClient::handleGroupChatRequest(PDU *pdu) {
    OperateWidget::getInstance().getFriend()->updateGroupMsg(pdu);
}

void TcpClient::handleCreateDirRespond(PDU *pdu) {
    QMessageBox::information(this, "提示", pdu->caData);
}

void TcpClient::handleFlushFileRespond(PDU *pdu) {
    OperateWidget::getInstance().getBook()->updateFileList(pdu);
}

void TcpClient::handleDeleteDirRespond(PDU *pdu) {
    QMessageBox::information(this, "提示", pdu->caData);
}

void TcpClient::handleRenameFileRespond(PDU *pdu) {
    qDebug() << "???";
    QMessageBox::information(this, "提示", pdu->caData);
}

void TcpClient::handleEnterDirRespond(PDU *pdu) {
    QMessageBox::information(this, "提示", pdu->caData);
}

void TcpClient::handleUploadRespond(PDU *pdu)
{
    QMessageBox::information(this,"上传文件",pdu->caData);

}

void TcpClient::handleDownloadRespond(PDU *pdu)
{

    qDebug()<<pdu->caData;
    char caFileName[32]={'\0'};
    qint64 total;
    sscanf(pdu->caData,"%s %lld",caFileName,&total);
    OperateWidget::getInstance().getBook()->setITotal(total);
    if(strlen(caFileName)>0&&OperateWidget::getInstance().getBook()->iTotal()>0)
    {
        OperateWidget::getInstance().getBook()->setBDownloadState(true);
        m_file.setFileName(OperateWidget::getInstance().getBook()->strSaveFilePath());
        if(!m_file.open(QIODevice::WriteOnly))
        {
          QMessageBox::warning(this,"下载文件","获得保存文件路径失败");
        }
    }

}


void TcpClient::on_login_Button_clicked()
{

    QString strName=ui->name_Edit->text();
    QString strPwd=ui->pwd_Edit->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty())
    {
        loginName=strName;
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=  ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);//前32个字节用于存放用户名
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);//发送
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::critical(this,"登录","登陆失败：用户名或密码为空");
    }
}


void TcpClient::on_register_Button_clicked()
{
    QString strName=ui->name_Edit->text();
    QString strPwd=ui->pwd_Edit->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty())
    {
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=  ENUM_MSG_TYPE_REGISTER_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);//前32个字节用于存放用户名
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);//发送
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::critical(this,"注册失败","用户名或密码为空");
    }
}


void TcpClient::on_logout_Button_clicked()
{

}

QString TcpClient::strCurPath() const
{
    return m_strCurPath;
}

void TcpClient::setStrCurPath(const QString &newStrCurPath)
{
    m_strCurPath = newStrCurPath;
}

QString TcpClient::getLoginName()
{
    return loginName;
}


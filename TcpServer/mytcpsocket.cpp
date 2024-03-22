#include "mytcpsocket.h"
#include"mytcpserver.h"
#include<QDir>
#include<QFileInfoList>
MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this,SIGNAL(readyRead()),this,SLOT(receiveMsg()));
    //进行封装，一个socket处理一个客户端

    connect(this,SIGNAL(disconnected()),this,SLOT(clientOffline()));
    //只要客户端挂掉，就自动发出disconnect信号

    setBUpload(false);
    m_pTimer=new QTimer;
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(sendFileToClient()));
}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::receiveMsg() {
    if(!bUpload())
    {
     //不是上传文件状态
    qDebug() << "-------" << this->bytesAvailable();
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen, sizeof(uint));

    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    this->read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));

    switch (pdu->uiMsgType) {
    case ENUM_MSG_TYPE_REGISTER_REQUEST:
        registerRequest(pdu);
        break;

    case ENUM_MSG_TYPE_LOGIN_REQUEST:
        loginRequest(pdu);
        break;

    case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
        allOnlineRequest(pdu);
        break;

    case ENUM_MSG_TYPE_SEARCH_USER_REQUEST:
        searchUserRequest(pdu);
        break;

    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        addFriendRequest(pdu);
        break;

    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:
        addFriendAgree(pdu);
        break;

    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
        addFriendRefuse(pdu);
        break;

    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUST:
        flushFriendRequest(pdu);
        break;

    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
        deleteFriendRequest(pdu);
        break;

    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
        privateChatRequest(pdu);
        break;

    case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
        groupChatRequest(pdu);
        break;

    case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:
        createDirRequest(pdu);
        break;

    case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:
        flushFileRequest(pdu);
        break;

    case ENUM_MSG_TYPE_DEL_DIR_REQUEST:
        deleteDirRequest(pdu);
        break;

    case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:
        renameFileRequest(pdu);
        break;

    case ENUM_MSG_TYPE_ENTER_DIR_REQUEST:
        enterDirRequest(pdu);
        break;

    case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
        uploadRequest(pdu);
        break;
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:
        downloadRequest(pdu);
    default:
        break;
    }

    free(pdu);
    pdu = NULL;
   }
   else
   {
    PDU *respdu;
      //接收上传数据
    QByteArray buff=readAll();
    m_file.write(buff);
     setIRecved(iRecved()+buff.size());
    if(iRecved()>=iTotal())
     {
        //收完了
        m_file.close();
        setBUpload(false);

        respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
        memcpy(respdu->caData,UPLOAD_FILE_OK,strlen(UPLOAD_FILE_OK));
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
    }
    else if(iRecved()<iTotal())
    {
        m_file.close();
        setBUpload(false);
        memcpy(respdu->caData,UPLOAD_FILE_FAILED,strlen(UPLOAD_FILE_FAILED));
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
    }

  }
}
void MyTcpSocket::clientOffline()
{
   //在线状态改为非在线状态
    OperateDB::getInstance().handleOffline(m_strName.toStdString().c_str());
  emit offline(this);//该对象的地址
}

void MyTcpSocket::sendFileToClient()
{
  m_pTimer->stop();
  char *pData=new char[4096];
  qint64 ret=0;//实际读取到的数据
  while(1)
  {
    ret=m_file.read(pData,4096);
    if(ret>0&&ret<=4096)
    {
        write(pData,ret);
    }
    else if(ret==0)
    {
        m_file.close();
        break;
    }
    else if(ret<0)
    {
        qDebug()<<"发送文件内容到客户过程中失败";
        m_file.close();
        break;
    }
  }
  delete []pData;
  pData=NULL;
}

//QFile& MyTcpSocket::getfile()
//{
//    return m_file;
//}

//void MyTcpSocket::setFile(QFile &newFile)
//{
//    m_file = std::move(newFile);  // 使用移动语义，避免拷贝
//}

bool MyTcpSocket::bUpload() const
{
    return m_bUpload;
}

void MyTcpSocket::setBUpload(bool newBUpload)
{
    m_bUpload = newBUpload;
}

qint64 MyTcpSocket::iRecved() const
{
    return m_iRecved;
}

void MyTcpSocket::setIRecved(qint64 newIRecved)
{
    m_iRecved = newIRecved;
}

qint64 MyTcpSocket::iTotal() const
{
    return m_iTotal;
}

void MyTcpSocket::setITotal(qint64 newITotal)
{
    m_iTotal = newITotal;
}

void MyTcpSocket::registerRequest( PDU *pdu) {
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    strncpy(caPwd, pdu->caData + 32, 32);

    bool ret = OperateDB::getInstance().handleRegister(caName, caPwd);

    PDU *respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_REGISTER_RESPOND;

    if (ret) {
        strcpy(respdu->caData, REGISTER_OK);
        QDir dir;
        dir.mkdir(QString("D://QTProject//Pan/%1").arg(caName));
    } else {
        strcpy(respdu->caData, REGISTER_FAILED);
    }

    write((char*)respdu, respdu->uiPDULen);
    free(respdu);
    respdu = NULL;
}

void MyTcpSocket::loginRequest( PDU *pdu) {
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    strncpy(caPwd, pdu->caData + 32, 32);

    bool ret = OperateDB::getInstance().handleLogin(caName, caPwd);

    PDU *respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;

    if (ret) {
        strcpy(respdu->caData, LOGIN_OK);
        m_strName = caName;
    } else {
        strcpy(respdu->caData, LOGIN_FAILED);
    }

    write((char*)respdu, respdu->uiPDULen);
    free(respdu);
    respdu = NULL;
}

void MyTcpSocket::allOnlineRequest(PDU *pdu) {
    QStringList ret = OperateDB::getInstance().handleAllOnline();

    uint uiMsgLen = ret.size() * 32;
    PDU *respdu = mkPDU(uiMsgLen);
    respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;

    for (int i = 0; i < ret.size(); i++) {
        memcpy((char*)(respdu->caMsg) + i * 32, ret.at(i).toStdString().c_str(), ret.at(i).size());
    }

    write((char*)respdu, respdu->uiPDULen);
    free(respdu);
    respdu = NULL;
}

void MyTcpSocket::searchUserRequest( PDU *pdu) {
    int state = OperateDB::getInstance().handleSearchUser(pdu->caData);

    PDU *respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USER_RESPOND;

    if (state == -1) {
        strcpy(respdu->caData, SEARCH_USER_NO);
    } else if (state == 0) {
        strcpy(respdu->caData, SEARCH_USER_OFFLINE);
    } else if (state == 1) {
        strcpy(respdu->caData, SEARCH_USER_ONLINE);
    }

    write((char*)respdu, respdu->uiPDULen);
    free(respdu);
    respdu = NULL;
}

void MyTcpSocket::addFriendRequest( PDU *pdu) {
    qDebug() << "服务器收到了添加好友请求";
    char loginName[32] = {'\0'};
    char friendName[32] = {'\0'};
    strncpy(friendName, pdu->caData, 32);
    strncpy(loginName, pdu->caData + 32, 32);
    int ret = OperateDB::getInstance().handleAddFriend(friendName, loginName);
    PDU *respdu = NULL;

    if (ret == -1) {
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        strcpy(respdu->caData, UNKNOW_ERROR);
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
    } else if (ret == 0) {
        qDebug() << "好友已经存在";
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        strcpy(respdu->caData, EXISTED_FRIEND);
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
    } else if (ret == 1) {
        qDebug() << "转发给朋友";
        MyTcpServer::getInstance().repost(friendName, pdu);
    } else if (ret == 2) {
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        strcpy(respdu->caData, ADD_FRIEND_OFFLINE);
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
    } else if (ret == 3) {
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        strcpy(respdu->caData, ADD_FRIEND_NOTEXIST);
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
    }
}

void MyTcpSocket::addFriendAgree( PDU *pdu) {
    qDebug() << "mytcpsocket.cpp：其中一方点击了同意";
    char loginName[32] = {'\0'};
    char friendName[32] = {'\0'};
    strncpy(friendName, pdu->caData, 32);
    strncpy(loginName, pdu->caData + 32, 32);
    OperateDB::getInstance().AddFriend(friendName, loginName);
}

void MyTcpSocket::addFriendRefuse(PDU *pdu) {
    char caName[32] = {'\0'};
    strncpy(caName, pdu->caData + 32, 32);
    MyTcpServer::getInstance().repost(caName, pdu);
}

void MyTcpSocket::flushFriendRequest( PDU *pdu) {
    char loginName[32] = {'\0'};
    strncpy(loginName, pdu->caData, 32);
    qDebug() << "我是服务器，我要找" << loginName << "的好友";
    QStringList ret = OperateDB::getInstance().handleFlushFriend(loginName);

    uint uiMsgLen = ret.size() * 32;
    PDU *respdu = mkPDU(uiMsgLen);
    respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;

    for (int i = 0; i < ret.size(); i++) {
        memcpy((char*)(respdu->caMsg) + i * 32, ret.at(i).toStdString().c_str(), ret.at(i).size());
    }

    write((char*)respdu, respdu->uiPDULen);
    free(respdu);
    respdu = NULL;
}

void MyTcpSocket::deleteFriendRequest( PDU *pdu) {
    char caSelfName[32] = {'\0'};
    char caFriendName[32] = {'\0'};
    strncpy(caSelfName, pdu->caData, 32);
    strncpy(caFriendName, pdu->caData + 32, 32);
    OperateDB::getInstance().handleDelFriend(caSelfName, caFriendName);
    PDU *respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
    strcpy(respdu->caData, DEL_FRIEND_OK);
    write((char*)respdu, respdu->uiPDULen);
    free(respdu);
    respdu = NULL;
}

void MyTcpSocket::privateChatRequest(PDU *pdu) {
    char caFriendName[32] = {'\0'};
    memcpy(caFriendName, pdu->caData + 32, 32);
    qDebug() << "我想私聊" << caFriendName;
    MyTcpServer::getInstance().repost(caFriendName, pdu);
}

void MyTcpSocket::groupChatRequest(PDU *pdu) {
    char loginName[32] = {'\0'};
    strncpy(loginName, pdu->caData, 32);
    QStringList myFriend = OperateDB::getInstance().handleFlushFriend(loginName);

    for (int i = 0; i < myFriend.size(); i++) {
        MyTcpServer::getInstance().repost(myFriend.at(i).toStdString().c_str(), pdu);
    }
}

void MyTcpSocket::createDirRequest( PDU *pdu) {
    QDir dir;
    QString strCurPath = QString("%1").arg((char*)(pdu->caMsg));
    qDebug() << "当前路径" << (char*)(pdu->caData);
    bool ret = dir.exists(strCurPath);
    PDU *respdu = NULL;

    if (ret) {
        char caNewDir[32] = {'\0'};
        memcpy(caNewDir, pdu->caData + 32, 32);
        QString strNewPath = strCurPath + "/" + caNewDir;
        ret = dir.exists(strNewPath);

        if (ret) {
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
            strcpy(respdu->caData, FILE_NAME_EXIST);
        } else {
            dir.mkdir(strNewPath);
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
            strcpy(respdu->caData, CREATE_DIR_OK);
        }
    } else {
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
        strcpy(respdu->caData, DIR_NO_EXIST);
    }

    write((char*)respdu, respdu->uiPDULen);
    free(respdu);
    respdu = NULL;
}

void MyTcpSocket::flushFileRequest(PDU *pdu) {
    char *pCurPath = new char[pdu->uiMsgLen];
    memcpy(pCurPath, pdu->caMsg, pdu->uiMsgLen);
    QDir dir(pCurPath);
    qDebug() << "刷新" << pCurPath;
    QFileInfoList fileInfoList = dir.entryInfoList();
    int iFileCount = fileInfoList.size();
    PDU *respdu = mkPDU(sizeof(FileInfo) * iFileCount);
    respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
    FileInfo *pFileInfo = NULL;
    QString strFileName;

    for (int i = 0; i < iFileCount; i++) {
        pFileInfo = (FileInfo*)(respdu->caMsg) + i;
        strFileName = fileInfoList[i].fileName();
        memcpy(pFileInfo->caFileName, strFileName.toStdString().c_str(), strFileName.size());

        if (fileInfoList[i].isDir()) {
            pFileInfo->iFileType = 0;
        } else if (fileInfoList[i].isFile()) {
            pFileInfo->iFileType = 1;
        }
    }

    write((char*)respdu, respdu->uiPDULen);
    free(respdu);
    respdu = NULL;
}

void MyTcpSocket::deleteDirRequest( PDU *pdu) {
    char caName[32] = {'\0'};
    strcpy(caName, pdu->caData);
    char *pPath = new char[pdu->uiMsgLen];
    memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
    QString strPath = QString("%1/%2").arg(pPath).arg(caName);
    qDebug() << strPath;

    bool ret = false;
    QFileInfo fileInfo(strPath);

    if (fileInfo.isDir()) {
        QDir dir(strPath);
        ret = dir.removeRecursively();
    } else if (fileInfo.isFile()) {
        ret = false;
    }

    PDU *respdu = NULL;

    if (ret) {
        respdu = mkPDU(strlen(DEL_DIR_OK) + 1);
        respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
        memcpy(respdu->caData, DEL_DIR_OK, strlen(DEL_DIR_OK));
    } else {
        respdu = mkPDU(strlen(DEL_DIR_FAILED) + 1);
        respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
        memcpy(respdu->caData, DEL_DIR_OK, strlen(DEL_DIR_FAILED));
    }

    write((char*)respdu, respdu->uiPDULen);
    free(respdu);
    respdu = NULL;
}

void MyTcpSocket::renameFileRequest( PDU *pdu) {
    char caOldName[32] = {'\0'};
    char caNewName[32] = {'\0'};
    strncpy(caOldName, pdu->caData, 32);
    strncpy(caNewName, pdu->caData + 32, 32);

    char *pPath = new char[pdu->uiMsgLen];
    memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);

    QString strOldPath = QString("%1/%2").arg(pPath).arg(caOldName);
    QString strNewPath = QString("%1/%2").arg(pPath).arg(caNewName);

    QDir dir;
    bool ret = dir.rename(strOldPath, strNewPath);
    PDU *respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;

    if (ret) {
        memcpy(respdu->caData, RENAME_FILE_OK, strlen(RENAME_FILE_OK));
    } else {
        memcpy(respdu->caData, RENAME_FILE_FAILED, strlen(RENAME_FILE_FAILED));
    }

    write((char*)respdu, respdu->uiPDULen);
    free(respdu);
    respdu = NULL;
}

void MyTcpSocket::enterDirRequest(PDU *pdu) {
    char *pPath = new char[pdu->uiMsgLen];
    memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
    QString strPath = QString("%1").arg(pPath);

    QFileInfo fileInfo(strPath);
    PDU *respdu = NULL;

    if (fileInfo.isFile()) {
        respdu = mkPDU(0);
        memcpy(respdu->caData, ENTER_DIR_FAILED, strlen(ENTER_DIR_FAILED));
        respdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
    } else if (fileInfo.isDir()) {
        qDebug() << "进入文件夹";
        QDir dir(strPath);
        QFileInfoList fileInfoList = dir.entryInfoList();
        int iFileCount = fileInfoList.size();
        PDU *respdu = mkPDU(sizeof(FileInfo) * iFileCount);
        respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
        FileInfo *pFileInfo = NULL;
        QString strFileName;

        for (int i = 0; i < iFileCount; i++) {
            pFileInfo = (FileInfo*)(respdu->caMsg) + i;
            strFileName = fileInfoList[i].fileName();
            memcpy(pFileInfo->caFileName, strFileName.toStdString().c_str(), strFileName.size());

            if (fileInfoList[i].isDir()) {
                pFileInfo->iFileType = 0;
            } else if (fileInfoList[i].isFile()) {
                pFileInfo->iFileType = 1;
            }
        }

        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
    }
}

void MyTcpSocket::uploadRequest(PDU *pdu)
{
    char caFileName[32]={'\0'};
    qint64 fileSize=0;
    sscanf(pdu->caData,"%s %lld",caFileName,&fileSize);
    char *pPath=new char[pdu->uiMsgLen];
    memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
    QString strPath=QString("%1/%2").arg(pPath).arg(caFileName);
    qDebug()<<strPath;
    delete []pPath;
    pPath=NULL;

    m_file.setFileName(strPath);
    /*
     *以只写的方式打开，若文件不存在，则自动创建文件
     *可以改为多线程
     */
    if(m_file.open(QIODevice::WriteOnly))
    {
        setBUpload(true);
        setITotal(fileSize);
        setIRecved(0);
    }
}

void MyTcpSocket::downloadRequest(PDU *pdu)
{
    char caFileName[32]={'\0'};
    strcpy(caFileName,pdu->caData);
    char *pPath=new char[pdu->uiMsgLen];
    memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
    QString strPath=QString("%1/%2").arg(pPath).arg(caFileName);
    qDebug()<<strPath;
    delete []pPath;
    pPath=NULL;

    QFileInfo fileInfo(strPath);
    qint64 fileSize=fileInfo.size();
    PDU *respdu=mkPDU(0);
    sprintf(respdu->caData,"%s %lld",caFileName,fileSize);
    respdu->uiMsgType=ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;

    write((char*)respdu,respdu->uiPDULen);
    free(respdu);
    respdu=NULL;

    m_file.setFileName(strPath);
    m_file.open(QIODevice::ReadOnly);
    m_pTimer->start(1000);
}


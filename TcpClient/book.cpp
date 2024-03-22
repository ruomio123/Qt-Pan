#include "book.h"
#include "ui_book.h"
#include"protocol.h"
#include"tcpclient.h"
#include<QInputDialog>
#include<QMessageBox>
#include<QPainter>
#include<QIcon>
#include<QPixmap>
#include<QFileDialog>
Book::Book(QWidget *parent)
    : QWidget{parent},
    ui(new Ui::Book)
{

   ui->setupUi(this);
//    关联信号槽
   m_pTimer=new QTimer;
   setBDownloadState(false);
   connect(ui->m_CreateButton,SIGNAL(clicked(bool)),this,SLOT(createDir()));

   connect(ui->m_FlushDirButton,SIGNAL(clicked(bool)),this,SLOT(flushDir()));

   connect(ui->m_DeleteButton,SIGNAL(clicked(bool)),this,SLOT(delDir()));

   connect(ui->m_RenameButton,SIGNAL(clicked(bool)),this,SLOT(renameFile()));

//   connect(ui->listWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(on_listWidget_itemDoubleClicked()));

   connect(ui->m_UploadButton,SIGNAL(clicked(bool)),this,SLOT(uploadFile()));

   connect(m_pTimer,SIGNAL(timeout()),this,SLOT(uploadFileData()));
   //如果上传完文件马上发送请求，内容和请求可能会粘在一块，所以要设置定时器

   connect(ui->m_DownloadButton,SIGNAL(clicked(bool)),this,SLOT(downloadFile()));
}

void Book::paintEvent(QPaintEvent *event)
{
   QPainter *painter=new QPainter(this);
   QPixmap pic = QPixmap(":/image/five.jpg");
   painter->drawPixmap(0,0,this->width(),this->height(),pic);
   painter->end();
}

void Book::updateFileList(const PDU *pdu)
{

   if(pdu==NULL)return;
   else
   {
       ui->listWidget->clear();
       FileInfo *pFileInfo=NULL;
       int count=pdu->uiMsgLen/sizeof(FileInfo);
       for(int i=0;i<count;i++)
       {
           pFileInfo=(FileInfo*)(pdu->caMsg)+i;
           qDebug()<<"测试"<<pFileInfo->caFileName<<" "<<pFileInfo->iFileType;
           QListWidgetItem *pItem=new QListWidgetItem;
           if(pFileInfo->iFileType==0)
           {
                pItem->setIcon(QIcon(QPixmap(":/image/dir.png")));
           }
           else if(pFileInfo->iFileType==1)
           {
                pItem->setIcon(QIcon(QPixmap(":/image/file.png")));
           }
           pItem->setText(pFileInfo->caFileName);
           ui->listWidget->addItem(pItem);
       }
   }
}

void Book::delDir()
{
   QString strCurPath=TcpClient::getInstance().strCurPath();//获取当前路径
   QListWidgetItem *pItem=ui->listWidget->currentItem();//获取当前选中的一行
   if(pItem==NULL)
   {
       QMessageBox::warning(this,"警告","选中文件不能为空");
   }
   else
   {
       QString strDelName=pItem->text();//获取文件名
       qDebug()<<"删除文件名"<<strDelName;
       PDU *pdu=mkPDU(strCurPath.size()+1);
       pdu->uiMsgType=ENUM_MSG_TYPE_DEL_DIR_REQUEST;
       memcpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.size());
       memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
       TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
       free(pdu);
       pdu=NULL;
   }
}

void Book::renameFile()
{
   QString strCurPath=TcpClient::getInstance().strCurPath();//获取当前路径
    QListWidgetItem *pItem=ui->listWidget->currentItem();//获取当前选中的一行
   if(pItem==NULL)
   {
       QMessageBox::warning(this,"警告","选中文件不能为空");
   }
   else
   {
       QString strOldName=pItem->text();//获取文件名
       QString strNewName=QInputDialog::getText(this,"重命名","请输入新的文件名");
       if(strNewName.isEmpty())
       {
           QMessageBox::warning(this,"警告","名字不能为空");

       }
       else
       {
           PDU *pdu=mkPDU(strCurPath.size()+1);
           pdu->uiMsgType=ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
           strncpy(pdu->caData,strOldName.toStdString().c_str(),strOldName.size());
           strncpy(pdu->caData+32,strNewName.toStdString().c_str(),strNewName.size());
           memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
           TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
           free(pdu);
           pdu=NULL;
       }
   }
}

void Book::uploadFile()
{
   m_strUploadFilePath=QFileDialog::getOpenFileName();//获取选中路径

   if(m_strUploadFilePath.isEmpty())
   {
       QMessageBox::warning(this,"警告","上传文件名字不能为空");

   }
   else
   {
       int index=m_strUploadFilePath.lastIndexOf('/');
       QString strFileName=m_strUploadFilePath.right(m_strUploadFilePath.size()-index-1);
       qDebug()<<strFileName;

       QFile file(m_strUploadFilePath);
       qint64 fileSize=file.size();//文件大小
       qDebug()<<fileSize;
       QString strCurPath=TcpClient::getInstance().strCurPath();
       PDU *pdu=mkPDU(strCurPath.size()+1);
       pdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
       memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
       sprintf(pdu->caData,"%s %lld",strFileName.toStdString().c_str(),fileSize);//拼接字符串的形式写入数据
       TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
       free(pdu);
       pdu=NULL;
       m_pTimer->start(1000);

   }
}

void Book::uploadFileData()
{
   m_pTimer->stop();
   QFile file(m_strUploadFilePath);
   if(file.open(QIODevice::ReadOnly))
   {
       //打开成功
       char *pBuffer=new char[4096];
       qint64 ret=0;
       while(1)
       {
           ret=file.read(pBuffer,4096);//实际读到的数据大小
           if(ret>0&&ret<=4096)
           {
                TcpClient::getInstance().getTcpSocket().write(pBuffer,ret);
           }
           else if(ret==0)
           {
                qDebug()<<"读完";
                break;//读完了
           }
           else
           {
                QMessageBox::warning(this,"上传文件","上传文件失败:读取失败");
                break;
           }
       }
       file.close();
       delete []pBuffer;
       pBuffer=NULL;
   }
   else
   {
       QMessageBox::warning(this,"上传文件","打开文件失败");
   }
}

void Book::downloadFile()
{

   QListWidgetItem *pItem=ui->listWidget->currentItem();//获取当前所选中的文件名字
   if(pItem==NULL)
   {
       QMessageBox::warning(this, "下载文件", "请选择要下载的文件");
   }
   else
   {
       QString strCurPath=TcpClient::getInstance().strCurPath();
       PDU *pdu=mkPDU(strCurPath.size()+1);
       pdu->uiMsgType=ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
       QString  strFileName=pItem->text();
       memcpy(pdu->caData,strFileName.toStdString().c_str(),strFileName.size());
       memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
       QString strSaveFilePath=QFileDialog::getSaveFileName();//弹出窗口选择路径
       if(strSaveFilePath.isEmpty())
       {
           QMessageBox::warning(this,"警告","选中路径不能为空");
           setStrSaveFilePath(NULL);
       }
       else
       {
           setStrSaveFilePath(strSaveFilePath);

       }
       TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);

   }
}

void Book::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
   QString strDirName=item->text();//获取文件名
   qDebug()<<strDirName;
   QString strCurPath=TcpClient::getInstance().strCurPath();
   QString strNewPath=QString("%1/%2").arg(strCurPath).arg(strDirName);

   TcpClient::getInstance().setStrCurPath(strNewPath);//****关键代码**更新路径

   PDU *pdu=mkPDU(strNewPath.size()+1);
   pdu->uiMsgType=ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
   memcpy(pdu->caMsg,strNewPath.toStdString().c_str(),strNewPath.size());
   TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
   free(pdu);
   pdu=NULL;
}

qint64 Book::iRecved() const
{
   return m_iRecved;
}

void Book::setIRecved(qint64 newIRecved)
{
   m_iRecved = newIRecved;
}

qint64 Book::iTotal() const
{
   return m_iTotal;
}

void Book::setITotal(qint64 newITotal)
{
   m_iTotal = newITotal;
}

bool Book::bDownloadState() const
{
   return m_bDownloadState;
}

void Book::setBDownloadState(bool newBDownloadState)
{
   m_bDownloadState = newBDownloadState;
}

QString Book::strSaveFilePath() const
{
   return m_strSaveFilePath;
}

void Book::setStrSaveFilePath(const QString &newStrSaveFilePath)
{
   m_strSaveFilePath = newStrSaveFilePath;
}
void Book::createDir()
{
//   获取输入的文件夹名字
   QString strNewDir=QInputDialog::getText(this,"新建","输入新文件夹名字");
   if(strNewDir.isEmpty())
   {
       QMessageBox::warning(this,"警告","文件夹名字不能为空");
   }
   else
   {
       if(strNewDir.size()>32)
       {
            QMessageBox::warning(this,"警告","文件夹名字长度不能超过32");
       }
       else
       {

            QString strLoginName=TcpClient::getInstance().getLoginName();
            QString strCurPath=TcpClient::getInstance().strCurPath();

            PDU *pdu=mkPDU(strCurPath.size()+1);
            pdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            memcpy(pdu->caData,strLoginName.toStdString().c_str(),strLoginName.size());
            //       首地址， 字符串大小
            memcpy(pdu->caData+32,strNewDir.toStdString().c_str(),strNewDir.size());
            memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
            qDebug()<<"客户端当前pdu"<<((char*)pdu->caMsg);
            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);//发送服务器
            free(pdu);
            pdu=NULL;//防止内存泄露
       }


   }


}

void Book::flushDir()
{
   //  首先，获取当前所在文件夹的路径,创建pdu
   QString strCurPath=TcpClient::getInstance().strCurPath();
   PDU *pdu=mkPDU(strCurPath.size()+1);
   pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
   memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());

   //   创建完毕，准备发送
   TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
   free(pdu);
   pdu=NULL;

}


//void Book::on_listWidget_doubleClicked(const QModelIndex &index)
//{
//   QString strDirName=index.data().toString();//获取文件名
//   qDebug()<<strDirName;
//}


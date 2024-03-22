#ifndef BOOK_H
#define BOOK_H
#include"protocol.h"
#include <QWidget>
#include<QListWidget>
#include<QTimer>
namespace Ui {
class Book;
}
class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event)override;//重写绘图事件

    void updateFileList(const PDU*pdu);//刷新文件列表

    QString strSaveFilePath() const;
    void setStrSaveFilePath(const QString &newStrSaveFilePath);

    bool bDownloadState() const;
    void setBDownloadState(bool newBDownloadState);

    qint64 iTotal() const;
    void setITotal(qint64 newITotal);

    qint64 iRecved() const;
    void setIRecved(qint64 newIRecved);

public slots:
//    槽函数
    void createDir();
    void flushDir();
    void delDir();//删除文件夹
    void renameFile();//重命名
    void uploadFile();//上传文件
    void uploadFileData();//传输数据
    void downloadFile();//下载文件

signals:

private slots:
   void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
  // void on_listWidget_doubleClicked(const QModelIndex &index);效果同上

private:
    Ui::Book *ui;
    QString m_strUploadFilePath;//保存打开的路径
    QTimer *m_pTimer;//定时器
    QString m_strSaveFilePath;
    bool m_bDownloadState;//下载状态

    qint64 m_iTotal;//总的文件大小
    qint64 m_iRecved;//已收到多少
};

#endif // BOOK_H

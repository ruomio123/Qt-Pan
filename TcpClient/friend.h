#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include<QListWidget>
#include<QLineEdit>
#include<QTextEdit>
#include<QPushButton>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include"onlineuser.h"
#include"protocol.h"

class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);

    void showAllOnlineUser(PDU *pdu);
    void updateFriendList(PDU *pdu);//更新好友列表
    void updateGroupMsg(PDU *pdu);
    void paintEvent(QPaintEvent *event) override;
signals:

public slots:
    void showOnline();//显示在线用户槽函数
    void searchUser();//查找用户
    void flushFriend();//刷新好友列表
    void delFriend();//删除好友
    void privateChat();//私聊
    void groupChat();//群聊

private:
    QTextEdit *m_pShowMsgTE;
    QListWidget *m_pFriendList;//朋友列表
    QLineEdit *m_pInputMsgLE;//信息输入框

    QPushButton *m_pDelFriendPB;//删除好友
    QPushButton *m_pFlushFriendPB;//刷新好友
    QPushButton *m_pShowOnlineUsrPB;//查看在线用户
    QPushButton *m_pSearchUsrPB;//查找用户

    QPushButton *m_pMsgSendPB;//群聊
    QPushButton *m_pPrivateChatPB;//私聊

    OnlineUser *m_pOnline;
public:
    QString m_strSearchName;//临时保存查找的用户名字
};

#endif // FRIEND_H

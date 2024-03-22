#ifndef ONLINEUSER_H
#define ONLINEUSER_H

#include <QWidget>
#include"protocol.h"
//显示在线好友的类
namespace Ui {
class OnlineUser;
}

class OnlineUser : public QWidget
{
    Q_OBJECT

public:
    explicit OnlineUser(QWidget *parent = nullptr);
    ~OnlineUser();
    void showUser(PDU *pdu);

private slots:
    void on_addFriend_button_clicked();

private:
    Ui::OnlineUser *ui;
};

#endif // ONLINEUSER_H

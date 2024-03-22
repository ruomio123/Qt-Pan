#ifndef OPERATEWIDGET_H
#define OPERATEWIDGET_H

#include <QWidget>
#include<qlistwidget>
#include"friend.h"
#include"book.h"
#include<QStackedWidget>
class OperateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OperateWidget(QWidget *parent = nullptr);

    static OperateWidget &getInstance();

    Friend *getFriend();

    Book *getBook();
signals:

private:
    QListWidget *m_pListW;
    Friend *m_pFriend;
    Book *m_pBook;

    QStackedWidget *m_pSW;//堆栈窗口，一次只显示一个

};

#endif // OPERATEWIDGET_H

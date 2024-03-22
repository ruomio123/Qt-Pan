#include "operatewidget.h"

OperateWidget::OperateWidget(QWidget *parent)
    : QWidget{parent}
{
    this->resize(700,500);
    m_pListW=new QListWidget(this);//
    m_pListW->addItem("好友");
    m_pListW->addItem("图书");

    m_pFriend=new Friend;
    m_pBook=new Book;

    m_pSW=new QStackedWidget;
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pBook);
    //默认显示第一个窗口

    QHBoxLayout *pMain=new QHBoxLayout;
    pMain->addWidget(m_pListW);
    pMain->addWidget(m_pSW);

    setLayout(pMain);

    connect(m_pListW,SIGNAL(currentRowChanged(int)),m_pSW,SLOT(setCurrentIndex(int)));
}

OperateWidget &OperateWidget::getInstance()
{
    static OperateWidget instance;//不管调用多少次，始终使用同一个界面
    return instance;

}

Friend *OperateWidget::getFriend()
{
    return m_pFriend;
}

Book *OperateWidget::getBook()
{
    return m_pBook;
}

#include "tcpclient.h"
#include "operatewidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpClient::getInstance().show();

    return a.exec();
}

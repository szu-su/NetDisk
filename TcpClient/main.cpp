#include "tcpclient.h"

#include <QApplication>
//#include "book.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    TcpClient w;
//    w.show();
    TcpClient::getInstance().show();
//    Book w;
//    w.show();
    return a.exec();
}

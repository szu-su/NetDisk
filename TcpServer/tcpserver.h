#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "mytcpserver.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class TcpServer; }
QT_END_NAMESPACE

class TcpServer : public QWidget
{
    Q_OBJECT

public:
    TcpServer(QWidget *parent = nullptr);
    ~TcpServer();
    void loadConfig();  //配置文件加载

private:
    Ui::TcpServer *ui;
    //全局变量 读取到的内容
    QString m_strIP;    //保存本地ip
    quint16 m_usPort;   //保存端口

};
#endif // TCPSERVER_H

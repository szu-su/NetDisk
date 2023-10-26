#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include "protocol.h"
#include "opewidget.h"
QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

    void loadConfig();  //配置文件加载
    void linkHost();  //连接服务器

    static TcpClient &getInstance(); //
    QTcpSocket &getTcpSocket();
    QString loginName();
    QString curPath();
    void setCurPath(QString strCurPath);

public slots:
    void showConnect();
    void recvMsg();
private slots:
//    void on_send_pb_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_cancel_pb_clicked();

private:
    Ui::TcpClient *ui;

    //全局变量 读取到的内容
    QString m_strIP;    //保存本地ip
    quint16 m_usPort;   //保存端口

    //连接服务器，与服务器进行数据交互
    QTcpSocket m_tcpSocket;
    QString m_strLoginName;

    QString m_strCurPath; //当前路径

};
#endif // TCPCLIENT_H

#include "tcpserver.h"
#include "ui_tcpserver.h"

#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QFile>
TcpServer::TcpServer(QWidget *parent):
    QWidget(parent),
    ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);

}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if (file.open(QIODevice::ReadOnly))// 作用域：：只读打开
    {
        QByteArray baData  = file.readAll();  //读取文件所有内容
        QString strData = baData.toStdString().c_str();

        file.close();

        strData.replace("\r\n"," ");
        QStringList strList = strData.split(" ");
        //        for (int i = 0; i<strList.size(); i++){
        //            qDebug() <<"--->" <<strList[i];
        //        }
        //        qDebug()<<strData;

        m_strIP =strList.at(0);
        m_usPort = strList.at(1).toUShort();
        //        qDebug()<<m_strIP;
        //        qDebug()<<m_usPort;

    }else{
        QMessageBox::critical(this,"open config","open config failed");
    }
}


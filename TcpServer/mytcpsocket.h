#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"
#include "opedb.h"
#include <QDir>
#include <QFile>
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString getName();
signals:
    void offline(MyTcpSocket *mysocket );//去掉socket
public slots:
    void recvMsg();
    void clientOffline();
private:
    QString m_strName;
    QFile m_file;
    qint64 m_iTotal;
    qint64 m_iRecved;
    bool m_bUpload;
};

#endif // MYTCPSOCKET_H

#include "tcpclient.h"
#include "./ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include "privatechat.h"
TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    resize(500,250);
    loadConfig();
    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(recvMsg()));

    linkHost();
}

TcpClient::~TcpClient()
{
    delete ui;
}

//配置文件加载
void TcpClient::loadConfig()
{
    QFile file(":/client.config");
    if (file.open(QIODevice::ReadOnly))// 作用域：：只读打开
    {
        QByteArray baData  = file.readAll();  //读取文件所有内容
        QString strData = baData.toStdString().c_str();

        file.close();

        strData.replace("\r\n"," ");
        QStringList strList = strData.split(" ");

        m_strIP =strList.at(0);
        m_usPort = strList.at(1).toUShort();


    }else{
        QMessageBox::critical(this,"open config","open config failed");
    }



}

//连接服务器
void TcpClient::linkHost()
{

    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);
}

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;

}
 //返回m_tcpSocket引用
QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::loginName()
{
    return m_strLoginName;
}

QString TcpClient::curPath()
{
    return m_strCurPath;
}

void TcpClient::setCurPath(QString strCurPath)
{
    m_strCurPath =strCurPath;
}

void TcpClient::showConnect()
{
    QMessageBox::information(this, "LinkServer", "link server success!");
}

void TcpClient::recvMsg()
{
    qDebug()<<m_tcpSocket.bytesAvailable();
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));
    uint uiMsgLen = uiPDULen -sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)pdu +sizeof(uint),uiPDULen-sizeof(uint));
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGIST_RESPOND:
    {
        if (0 == strcmp(pdu->caData,REGIST_OK)){
            QMessageBox::information(this,"RESGIST","RESGIST SUCCESS!");
        }else if(0 == strcmp(pdu->caData,REGIST_FAILED)){
            QMessageBox::warning(this,"RESGIST",REGIST_FAILED);
        }
        break;
        //    qDebug()<<pdu->uiMsgType<<caName<<caPwd;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND: //登录回复
    {
        //接受登录成功数据
        if (0 == strcmp(pdu->caData,LOGIN_OK))
        {
            m_strCurPath = QString("./%1").arg(m_strLoginName);
            QMessageBox::information(this,"LOGIN","LOGIN SUCCESS!");

            //登录成功后跳转并隐藏
            OpeWidget::getInstance().show();
            this->hide();


        }else if(0 == strcmp(pdu->caData,LOGIN_FAILED)){
            QMessageBox::warning(this,"LOGIN",LOGIN_FAILED);
        }
        break;
        //    qDebug()<<pdu->uiMsgType<<caName<<caPwd;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND: //在线回复
    {
        OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USER_RESPOND: //查找回复
    {
        if(0==strcmp(SEARCH_USR_NO,pdu->caData))
        {
            QMessageBox::information(this,"Search",QString("%1: not exist").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }else if(0==strcmp(SEARCH_USR_ONLINE,pdu->caData))
        {
            QMessageBox::information(this,"Search",QString("%1: online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }
        else if(0==strcmp(SEARCH_USR_OFFLINE,pdu->caData))
        {
            QMessageBox::information(this,"Search",QString("%1: offline").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }
        OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST: //是否同意添加好友请求
    {

        char caName[32] = {'\0'};
        strncpy(caName,pdu->caData+32,32);
        int ret = QMessageBox::information(this,"添加好友",QString("%1 want to add you as friend ?").arg(caName),QMessageBox::Yes,QMessageBox::No);
        PDU *respdu = mkPDU(0);
        memcpy(respdu->caData,respdu->caData,32);
        if (ret == QMessageBox::Yes) //同意添加好友
        {
            respdu->uiMsgType =ENUM_MSG_TYPE_ADD_FRIEND_AGGREE;
        }else if (ret == QMessageBox::No){
            respdu->uiMsgType =ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
        }
        m_tcpSocket.write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND: //添加好友回复
    {
        QMessageBox::information(this,"添加好友",pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
    {
        OpeWidget::getInstance().getFriend()->updataFriendList(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DEL_FRIEND_REQUEST:
    {
        char caName[32] ={'\0'};
        memcpy(caName,pdu->caData,32);

        QMessageBox::information(this,"删除好友",QString("%1 delete you").arg(caName));

        break;
    }
    case ENUM_MSG_TYPE_DEL_FRIEND_RESPOND:
    {
        QMessageBox::information(this,"删除好友","删除好友成功");
        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
    {
        if(PrivateChat::getInstance().isHidden())
        {
            PrivateChat::getInstance().show();
        }
        char caSendName[32] = {'\0'};
        memcpy(caSendName,pdu->caData,32);
        QString strSendName = caSendName;
        PrivateChat::getInstance().setChatName(strSendName);
        PrivateChat::getInstance().updateMsg(pdu);
        break;
    }
    case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:
    {
        qDebug()<<pdu->caData;
        QMessageBox::information(this,"Create DIR",pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:
    {
        OpeWidget::getInstance().getBook()->updateFileList(pdu);
        QString strEnterDir = OpeWidget::getInstance().getBook()->enterDir();
        if(!strEnterDir.isEmpty()){
            m_strCurPath = m_strCurPath + "/" + strEnterDir;
            qDebug()<<m_strCurPath;
        }
        break;
    }
    case ENUM_MSG_TYPE_DEL_FILE_RESPOND:
    {
        QMessageBox::information(this,"Delete Dir",pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:
    {
        qDebug()<<pdu->caData;
        QMessageBox::information(this,"Rename File",pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_ENTER_DIR_RESPOND:
    {
        OpeWidget::getInstance().getBook()->clearEnterDir();
        QMessageBox::information(this,"enter File",pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:
    {
        QMessageBox::information(this,"upload file",pdu->caData);
        break;
    }
    default:
        break;
    }

    free(pdu);
    pdu=NULL;

}

#if 0
void TcpClient::on_send_pb_clicked()
{
    QString strMsg = ui->lineEdit->text();
    if (!strMsg.isEmpty()){
        PDU *pdu = mkPDU(strMsg.size());
        pdu->uiMsgType =8888;
        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else{
        QMessageBox::warning(this,"SendMessage","Message Must Not Empty");

    }
}
#endif


void TcpClient::on_login_pb_clicked()
{
    QString strName = ui->name_input->text();
    QString strPwd= ui->pwd_input->text();
    if(!strName.isEmpty() &&!strPwd.isEmpty()){
        m_strLoginName = strName;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;

    }else{
        QMessageBox::critical(this, "LOGIN", "LOGIN Failed");

    }
}


void TcpClient::on_regist_pb_clicked()
{
    QString strName = ui->name_input->text();
    QString strPwd= ui->pwd_input->text();
    if(!strName.isEmpty() &&!strPwd.isEmpty()){
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;

    }else{
      QMessageBox::critical(this, "Regist", "Regist Failed");

    }
}


void TcpClient::on_cancel_pb_clicked()
{

}


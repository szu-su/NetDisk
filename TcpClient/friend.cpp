#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QDebug>
#include "privatechat.h"
#include <QMessageBox>
Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    m_pShowMsgTE = new QTextEdit;
    m_pFriendListWidget =new QListWidget;
    m_pInputMsgLE = new QLineEdit;


    m_pDelFriendPB =new QPushButton("删除好友");
    m_pFlushFriendPB=new QPushButton("刷新好友");
    m_pShowOnlineUsrPB=new QPushButton("显示在线用户");
    m_pSearchUrsPB=new QPushButton("查找用户");
    m_pMsgSendPB=new QPushButton("发生信息");
    m_pPrivateChatPB=new QPushButton("私聊");



    QVBoxLayout *pRightPBVBL = new QVBoxLayout;
    pRightPBVBL->addWidget(m_pDelFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUsrPB);
    pRightPBVBL->addWidget(m_pSearchUrsPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL= new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);


    m_pOnline = new Online;

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();

    setLayout(pMain);

    //关联显示在线用户信号
    connect(m_pShowOnlineUsrPB,SIGNAL(clicked(bool)),this,SLOT(showOnline()));
    //关联查找用户信号
    connect(m_pSearchUrsPB,SIGNAL(clicked(bool)),this,SLOT(searchUsr()));
    //刷新好友列表用户信号
    connect(m_pFlushFriendPB,SIGNAL(clicked(bool)),this,SLOT(flushFriend()));
    //删除好友用户信号
    connect(m_pDelFriendPB,SIGNAL(clicked(bool)),this,SLOT(delFriend()));
    //私聊好友用户信号
    connect(m_pPrivateChatPB,SIGNAL(clicked(bool)),this,SLOT(privateChat()));

}

void Friend::showAllOnlineUsr(PDU *pdu)
{
    if (NULL  ==pdu)
    {
        return;
    }
    m_pOnline->showUsr(pdu);
}

void Friend::updataFriendList(PDU *pdu)
{
    if (NULL  ==pdu)
    {
        return;
    }
    uint uiSize =pdu->uiMsgLen/32;
    char caName[32] = {'\0'};
    for (uint i =0; i<uiSize;i++){
        memcpy(caName,(char*)(pdu->caMsg)+i*32,32);
        m_pFriendListWidget->addItem(caName);
    }
}

void Friend::showOnline()
{
    if (m_pOnline->isHidden())
    {
        m_pOnline->show();
        //请求信息
        PDU *pdu =mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else{
        m_pOnline->hide();
    }
}

void Friend::searchUsr()
{
    m_strSearchName = QInputDialog::getText(this,"Search","用户名：");
    if(!m_strSearchName.isEmpty()){
        qDebug() <<m_strSearchName;
        PDU *pdu =mkPDU(0);
        memcpy(pdu->caData,m_strSearchName.toStdString().c_str(),m_strSearchName.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USER_REQUEST;

        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;

    }

}

void Friend::flushFriend()
{
    QString strName = TcpClient::getInstance().loginName();
    PDU *pdu =mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::delFriend()
{
    if(NULL!=m_pFriendListWidget->currentItem())
    {
        QString strFriendName = m_pFriendListWidget->currentItem()->text();
        PDU *pdu =mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_FRIEND_REQUEST;
        QString strSelfName = TcpClient::getInstance().loginName();
        memcpy(pdu->caData,strSelfName.toStdString().c_str(),strSelfName.size());
        memcpy(pdu->caData+32,strFriendName.toStdString().c_str(),strFriendName.size());

        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }



}

void Friend::privateChat()
{

    if(NULL!=m_pFriendListWidget->currentItem())
    {
        QString strChatName = m_pFriendListWidget->currentItem()->text();
        PrivateChat::getInstance().setChatName(strChatName);
        if (PrivateChat::getInstance().isHidden())
        {
            PrivateChat::getInstance().show();
        }
    }else
    {
        QMessageBox::warning(this,"私聊","选择私聊对象");
    }
}

#include "mytcpsocket.h"
#include <QDebug>
#include <string.h>
#include "mytcpserver.h"
#include <QDir>
#include <QFileInfoList>

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    connect(this,SIGNAL(disconnected()),this,SLOT(clientOffline())); //客服端断掉自动调用

    m_bUpload =false;
}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::recvMsg()
{
    if(!m_bUpload){
        qDebug()<<this->bytesAvailable();
        uint uiPDULen = 0;
        this->read((char*)&uiPDULen,sizeof(uint));
        uint uiMsgLen = uiPDULen -sizeof(PDU);
        PDU *pdu = mkPDU(uiMsgLen);
        this->read((char*)pdu +sizeof(uint),uiPDULen-sizeof(uint));
        switch(pdu->uiMsgType){
        case ENUM_MSG_TYPE_REGIST_REQUEST:
        {
            char caName[32] ={'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            bool ret = OpeDB::getInstance().handleRegist(caName,caPwd);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
            if(ret){
                strcpy(respdu->caData,REGIST_OK);
                QDir dir;
                dir.mkdir(QString("./%1").arg(caName)); //创建文件夹

            }else{
                strcpy(respdu->caData,REGIST_FAILED);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
            //    qDebug()<<pdu->uiMsgType<<caName<<caPwd;
        }

        case ENUM_MSG_TYPE_LOGIN_REQUEST:
        {
            char caName[32] ={'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            bool ret = OpeDB::getInstance().handleLogin(caName,caPwd);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
            if(ret){
                strcpy(respdu->caData,LOGIN_OK);
                m_strName = caName;
            }else{
                strcpy(respdu->caData,LOGIN_FAILED);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            qDebug()<<pdu->uiMsgType<<caName<<caPwd;
            break;
        }

        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
        {
            QStringList ret =OpeDB::getInstance().handleAllOnline();
            uint uiMsgLen = ret.size() * 32;
            PDU *respdu = mkPDU(uiMsgLen);
            respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
            for (int i =0; i<ret.size();i++){
                memcpy((char*)(respdu->caMsg)+i*32,
                       ret.at(i).toStdString().c_str(),ret.at(i).size());
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }

        case ENUM_MSG_TYPE_SEARCH_USER_REQUEST:
        {
            int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USER_RESPOND;
            if (ret == -1){
                strcpy(respdu->caData,SEARCH_USR_NO);
            }else if(ret ==1){
                strcpy(respdu->caData,SEARCH_USR_ONLINE);
            }else if(ret ==0){
                strcpy(respdu->caData,SEARCH_USR_OFFLINE);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }

        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            char caPerName[32] ={'\0'};
            char caName[32] = {'\0'};
            strncpy(caPerName,pdu->caData,32);
            strncpy(caName,pdu->caData+32,32);
            int ret = OpeDB::getInstance().handleAddFriend(caPerName,caName);
            qDebug() <<"ret = " <<ret;
            PDU *respdu = mkPDU(0);

            if (-1 == ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,UNKONW_ERROR);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu=NULL;
            }else if( 0== ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,EXISTED_FRIEND);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu=NULL;
            }else if( 1== ret)
            {
                MyTcpServer::getInstance().resend(caPerName,pdu);


            }else if( 2== ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,ADD_FRIEND_OFFLINE);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu=NULL;
            }else if( 3== ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,ADD_FRIEND_NOEXIST);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu=NULL;
            }

            break;
        }


            //删除好友那章有补充
        case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
        {
            char caPerName[32] ={'\0'};
            char caName[32] = {'\0'};
            strncpy(caPerName,pdu->caData,32);
            strncpy(caName,pdu->caData+32,32);
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
        {
            break;
        }

        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
        {
            char caName[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            QStringList ret = OpeDB::getInstance().handleFlushFriend(caName);
            //            qDebug()<<ret;
            uint uiMsgLen = ret.size()*32;
            PDU *respdu = mkPDU(uiMsgLen);
            for (int i = 0; i<ret.size();i++)
            {
                memcpy((char*)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),ret.at(i).size());
            }
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;

            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }

        case ENUM_MSG_TYPE_DEL_FRIEND_REQUEST:
        {
            char caSelfName[32] ={'\0'};
            char caFriendName[32] = {'\0'};
            strncpy(caSelfName,pdu->caData,32);
            strncpy(caFriendName,pdu->caData+32,32);
            OpeDB::getInstance().handlleDelFriend(caSelfName,caFriendName);

            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FRIEND_RESPOND;
            strcpy(respdu->caData,DEL_FRIEND_OK);

            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            MyTcpServer::getInstance().resend(caFriendName,pdu);
            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
        {

            char caPerName[32] ={'\0'};
            memcpy(caPerName,pdu->caData+32,32);
            MyTcpServer::getInstance().resend(caPerName,pdu);
            qDebug()<<caPerName;

            //            char caFriendName[32] = {'\0'};
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:
        {
            QDir dir;
            QString strCurPath = QString("%1").arg((char*)(pdu->caMsg));
            bool ret = dir.exists(strCurPath);
            PDU *respdu =NULL;

            if(ret){
                //当前目录存在
                char caNewDir[32] ={'\0'};
                memcpy(caNewDir,pdu->caData+32,32);
                QString strNewPath = strCurPath + "/" +caNewDir;
                ret = dir.exists(strNewPath);

                if(ret){
                    //创建的文件名已存在
                    respdu = mkPDU(0);
                    respdu->uiMsgType =ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->caData,FILE_NAME_EXIST);
                }else
                {
                    dir.mkdir(strNewPath);
                    respdu = mkPDU(0);
                    respdu->uiMsgType =ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->caData,CREATE_DIR_OK);
                }
            }else
            {
                //当前目录不存在
                respdu = mkPDU(0);
                respdu->uiMsgType =ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                strcpy(respdu->caData,DIR_NO_EXIST);

            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu =NULL;
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:
        {
            char *pCurPath = new char[pdu->uiMsgLen];
            memcpy(pCurPath,pdu->caMsg,pdu->uiMsgLen);

            QDir dir(pCurPath);
            QFileInfoList fileInfoList = dir.entryInfoList();
            int iFileCount = fileInfoList.size();

            PDU *respdu = mkPDU(sizeof(FileInfo)*(iFileCount));
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;

            FileInfo *pFileInfo =NULL;
            QString strFileName;

            for (int i = 0; i<iFileCount;i++){
                //                if(QString(".")== fileInfoList[i].fileName() ||QString("..")== fileInfoList[i].fileName()){
                //                    continue;
                //                }
                pFileInfo=(FileInfo*)(respdu->caMsg)+i;
                strFileName = fileInfoList[i].fileName();

                memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.size());
                if(fileInfoList[i].isDir())
                {
                    pFileInfo->iFileType = 0;
                }else if(fileInfoList[i].isFile()){
                    pFileInfo->iFileType = 1;
                }


                //                qDebug()<<fileInfoList[i].fileName()<<fileInfoList[i].size();
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu =NULL;
            break;
        }
        case ENUM_MSG_TYPE_DEL_FILE_REQUEST:
        {
            char caName[32] = {'\0'};
            strcpy(caName,pdu->caData);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);

            QString strPath =QString("%1/%2").arg(pPath).arg(caName);
            qDebug()<<strPath;

            QFileInfo fileInfo(strPath);
            bool ret = false;
            if(fileInfo.isDir()){
                QDir dir;
                dir.setPath(strPath);
                ret = dir.removeRecursively();
            }else if(fileInfo.isFile()){
                ret = false;
            }

            PDU *respdu =NULL;
            if(ret){
                respdu = mkPDU(strlen(DEL_DIR_OK)+1);
                memcpy(respdu->caData,DEL_DIR_OK,strlen(DEL_DIR_OK));
                respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;
            }
            else{
                respdu = mkPDU(strlen(DEL_DIR_FAILURED)+1);
                memcpy(respdu->caData,DEL_DIR_FAILURED,strlen(DEL_DIR_FAILURED));
                respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;
            }

            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu =NULL;
            break;
        }
        case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:{
            char caOldNAME[32] ={'\0'};
            char canNewNAME[32] ={'\0'};
            char *pPath = new char[pdu->uiMsgLen];
            strncpy(caOldNAME,pdu->caData,32);
            strncpy(canNewNAME,pdu->caData+32,32);
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);


            QString strOldPath =QString("%1/%2").arg(pPath).arg(caOldNAME);
            QString strNewPath =QString("%1/%2").arg(pPath).arg(canNewNAME);
            qDebug()<<strOldPath;
            qDebug()<<strNewPath;
            QDir dir;
            bool ret = dir.rename(strOldPath,strNewPath);
            PDU *respdu =mkPDU(0);
            respdu->uiMsgType =ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
            if(ret){
                strcpy(respdu->caData,RENAME_FILE_OK);
            }else{
                strcpy(respdu->caData,RENAME_FILE_FAILURED);
            }


            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu =NULL;
            break;
        }
        case ENUM_MSG_TYPE_ENTER_DIR_REQUEST:
        {
            char caEnterNAME[32] ={'\0'};
            char *pPath = new char[pdu->uiMsgLen];
            strncpy(caEnterNAME,pdu->caData,32);
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);

            QString strPath =QString("%1/%2").arg(pPath).arg(caEnterNAME);
            QFileInfo fileInfo(strPath);
            PDU *respdu =NULL;
            if(fileInfo.isDir())
            {
                QDir dir(strPath);
                QFileInfoList fileInfoList = dir.entryInfoList();
                int iFileCount = fileInfoList.size();

                respdu = mkPDU(sizeof(FileInfo)*(iFileCount));
                respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;

                FileInfo *pFileInfo =NULL;
                QString strFileName;

                for (int i = 0; i<iFileCount;i++){

                    pFileInfo=(FileInfo*)(respdu->caMsg)+i;
                    strFileName = fileInfoList[i].fileName();

                    memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.size());
                    if(fileInfoList[i].isDir())
                    {
                        pFileInfo->iFileType = 0;
                    }else if(fileInfoList[i].isFile()){
                        pFileInfo->iFileType = 1;
                    }
                }
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu =NULL;


            }else if(fileInfo.isFile()){
                respdu =mkPDU(0);
                respdu->uiMsgType =ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
                strcpy(respdu->caData,ENTER_DIR_FAILURED);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu =NULL;
            }
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
        {
            char caFileName[32] = {'\0'};
            qint64 filesize  = 0;
            sscanf(pdu->caData,"%s %lld",caFileName,&filesize);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            QString strPath = QString("%1/%2").arg(pPath).arg(caFileName);
            qDebug() <<strPath;

            delete []pPath;
            pPath = NULL;

            m_file.setFileName(strPath);
            if(m_file.open(QIODevice::WriteOnly))
            //以只写的方式打开文件，若文件不存在，则自动创建文件
            {
                m_bUpload = true;
                m_iTotal = filesize;
                m_iRecved = 0;
            }
            break;
        }
        default:
            break;
        }
        free(pdu);
        pdu=NULL;
    }
    else
    {
        PDU *respdu = NULL;
        respdu =mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
        QByteArray buff = readAll();
        m_file.write(buff);
        m_iRecved += buff.size();
        if(m_iTotal==m_iRecved)
        {
            m_file.close();
            m_bUpload =false;
            strcpy(respdu->caData,UPLOAD_FILE_OK);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu =NULL;
        }else if(m_iTotal<m_iRecved){
            m_file.close();
            m_bUpload =false;
            strcpy(respdu->caData,UPLOAD_FILE_FAILURED);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu =NULL;
        }


    }
}

void MyTcpSocket::clientOffline()
{
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);//发送信号
}

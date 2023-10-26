#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
Book::Book(QWidget *parent)
    : QWidget{parent}
{
    m_strEnterDir.clear();
    m_pTimer = new QTimer;
    m_pBookListW = new QListWidget;
    m_pReturnPB =new QPushButton("Return");
    m_pCreateDirPB =new QPushButton("Create FileDir");
    m_pDelDirPB =new QPushButton("Delete FileDir");
    m_pRenamePB =new QPushButton("Rename File");
    m_pFlushPB =new QPushButton("Flush File");

    QVBoxLayout *pDirVBL =  new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushPB);

    QVBoxLayout *pFileVBL =  new QVBoxLayout;
    m_pUploadPB =new QPushButton("上传文件");
    m_pDownloadPB =new QPushButton("下载文件");
    m_pDelFilePB =new QPushButton("删除文件");
    m_pShareFilePB=new QPushButton("共享文件");

    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownloadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);

    QHBoxLayout *pMain =new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB,SIGNAL(clicked(bool)),this,SLOT(createDir()));
    connect(m_pFlushPB,SIGNAL(clicked(bool)),this,SLOT(flushFile()));
    connect(m_pDelDirPB,SIGNAL(clicked(bool)),this,SLOT(delDir()));
    connect(m_pRenamePB,SIGNAL(clicked(bool)),this,SLOT(renameFile()));

    connect(m_pBookListW,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(enterDir(QModelIndex)));
    connect(m_pReturnPB,SIGNAL(clicked(bool)),this,SLOT(returnPre()));
    connect(m_pUploadPB,SIGNAL(clicked(bool)),this,SLOT(uploadFile()));
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(uploadFileData()));

}

void Book::updateFileList(const PDU *pdu)
{
    if(NULL ==pdu){
        return;
    }
    int row =m_pBookListW->count();
    QListWidgetItem *pItemTmp = NULL;
    while(m_pBookListW->count()>0)
    {
        pItemTmp=  m_pBookListW->item(row-1);
        m_pBookListW->removeItemWidget(pItemTmp);
        delete pItemTmp;
        row= row-1;
    }


    FileInfo *pFileInfo =NULL;
    int iCount = pdu->uiMsgLen/sizeof(FileInfo);
    for (int i =0; i<iCount;i++){
        pFileInfo = (FileInfo*)(pdu->caMsg)+i;
        QListWidgetItem *pItem  = new QListWidgetItem;
        if(0==pFileInfo->iFileType){
            pItem->setIcon(QIcon(QPixmap(":/Image/DirIcon.png")));
        }else if(1==pFileInfo->iFileType)
        {
            pItem->setIcon(QIcon(QPixmap(":/Image/file.png")));
        }
        pItem->setText(pFileInfo->caFileName);
        m_pBookListW->addItem(pItem);
//        qDebug()<<pFileInfo->caFileName<<pFileInfo->iFileType;
    }
}

void Book::clearEnterDir()
{
    m_strEnterDir.clear();
}

QString Book::enterDir()
{
    return m_strEnterDir;
}

void Book::createDir()
{
    QString strNewDir = QInputDialog::getText(this,"Create New Dir","New Dir Name");
    if(!strNewDir.isEmpty())
    {
        if(strNewDir.size()>32)
        {
            QMessageBox::warning(this,"Create New Dir","New Dir Name Length Not pass 32 Words");

        }else
        {
            QString strName = TcpClient::getInstance().loginName();
            QString strCurPATH = TcpClient::getInstance().curPath();
            PDU *pdu = mkPDU(strCurPATH.size()+1);

            pdu->uiMsgType =ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
            strncpy(pdu->caData+32,strNewDir.toStdString().c_str(),strNewDir.size());

            memcpy(pdu->caMsg,strCurPATH.toStdString().c_str(),strCurPATH.size());

            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu= NULL;
        }

    }else
    {
        QMessageBox::warning(this,"Create New Dir","Name is Not Empty");
    }


}

void Book::flushFile()
{
    QString strCurPATH = TcpClient::getInstance().curPath();
    PDU *pdu = mkPDU(strCurPATH.size()+1);
    pdu->uiMsgType= ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strncpy((char*)(pdu->caMsg),strCurPATH.toStdString().c_str(),strCurPATH.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu= NULL;
}

void Book::delDir()
{
    QString strCurPATH = TcpClient::getInstance().curPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL ==pItem)
    {
        QMessageBox::warning(this, "Delete Dir", "Please choose your file to delete");
    }else{
        QString strDelName = pItem->text();
        PDU *pdu = mkPDU(strCurPATH.size()+1);
        pdu->uiMsgType= ENUM_MSG_TYPE_DEL_FILE_REQUEST;
        strncpy((char*)(pdu->caData),strDelName.toStdString().c_str(),strDelName.size());
        strncpy((char*)(pdu->caMsg),strCurPATH.toStdString().c_str(),strCurPATH.size());

        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu= NULL;

    }

}

void Book::renameFile()
{
   QString strCurPATH = TcpClient::getInstance().curPath();
   QListWidgetItem *pItem = m_pBookListW->currentItem();
   if(NULL ==pItem)
    {
        QMessageBox::warning(this, "Rename Dir", "Please choose your file to rename");
    }else
   {
        QString strOldName = pItem->text();
        QString strNewName = QInputDialog::getText(this,"Rename Dir", "Please input your file to rename");
        if(!strNewName.isEmpty()){

            PDU *pdu = mkPDU(strCurPATH.size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
            strncpy((char*)(pdu->caData),strOldName.toStdString().c_str(),strOldName.size());
            strncpy((char*)(pdu->caData+32),strNewName.toStdString().c_str(),strNewName.size());
            strncpy((char*)(pdu->caMsg),strCurPATH.toStdString().c_str(),strCurPATH.size());


            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu= NULL;

        }else{
            QMessageBox::warning(this, "Rename Dir", "New name must no empty!");
        }
   }

}

void Book::returnPre()
{
   QString strCurPATH = TcpClient::getInstance().curPath();
   QString strRootPath = "./" +TcpClient::getInstance().loginName();
   if(strCurPATH == strRootPath){
      QMessageBox::warning(this, "Return", "Return failed!");
   }
   else
   {
      int index = strCurPATH.lastIndexOf('/');
      strCurPATH.remove(index,strCurPATH.size() - index);
      TcpClient::getInstance().setCurPath(strCurPATH);

      clearEnterDir();
      flushFile();
   }
}

void Book::enterDir(const QModelIndex &index)
{
   QString strDirName = index.data().toString();
   m_strEnterDir =strDirName;
   QString strCurPATH = TcpClient::getInstance().curPath();

   PDU *pdu = mkPDU(strCurPATH.size()+1);
   pdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
   strncpy((char*)(pdu->caData),strDirName.toStdString().c_str(),strDirName.size());
   strncpy((char*)(pdu->caMsg),strCurPATH.toStdString().c_str(),strCurPATH.size());

   TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
   free(pdu);
   pdu= NULL;

}

void Book::uploadFile()
{
   QString strCurPATH = TcpClient::getInstance().curPath();
   m_strUploadFilePath = QFileDialog::getOpenFileName();
   qDebug()<<m_strUploadFilePath;
   if(!m_strUploadFilePath.isEmpty()){
      int index = m_strUploadFilePath.lastIndexOf('/');
      QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size()-index -1 );
      qDebug()<<strFileName;

      QFile file(m_strUploadFilePath);
      qint64 fileSize = file.size(); //获得文件大小

      QString strCurPATH = TcpClient::getInstance().curPath();
      PDU *pdu = mkPDU(strCurPATH.size()+1);
      pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
      memcpy(pdu->caMsg,strCurPATH.toStdString().c_str(),strCurPATH.size());
      sprintf(pdu->caData,"%s %lld",strFileName.toStdString().c_str(),fileSize);

      TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
      free(pdu);
      pdu= NULL;

      m_pTimer->start(1000);
   }else{
      QMessageBox::warning(this,"upload file","filename is empty");
   }
}

void Book::uploadFileData()
{
   m_pTimer->stop();
   QFile file(m_strUploadFilePath);
   if(!file.open(QIODevice::ReadOnly)){
      QMessageBox::warning(this,"upload file","open file failed");
      return;
   }
   char *pBuffer =new char[4096];
   qint64 ret= 0;
   while(true){
      ret = file.read(pBuffer,4096);
      if(ret >0 && ret <= 4096){
        TcpClient::getInstance().getTcpSocket().write(pBuffer,ret);
      }else if(ret ==0){
        break;
      }else{
        QMessageBox::warning(this,"upload file","upload file failed : read file failed");
        break;
      }
   }
   file.close();
   delete []pBuffer;
   pBuffer =NULL;
}
